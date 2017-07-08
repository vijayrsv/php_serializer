#include<ruby.h>
#include<php_serializer.h>

void
Init_php_serializer ()
{
  VALUE php_serializer_module = rb_define_module ("PhpSerializer");
  rb_define_singleton_method (php_serializer_module, "serialize", serialize,
                              1);
  rb_define_singleton_method (php_serializer_module, "unserialize",
                              unserialize, 1);
}

static inline void
serialize_long (php_str * buf, long val)
{
  php_str_appendl (buf, "i:", 2);
  php_str_append_long (buf, val);
  php_str_appendc (buf, ';');
}

static inline void
serialize_float (php_str * buf, VALUE flt)
{
  VALUE str = Qnil;
  str = rb_funcall (flt, rb_intern ("to_s"), 0);
  php_str_appendl (buf, "d:", 2);
  php_str_appendl (buf, RSTRING_PTR (str), RSTRING_LEN (str));
  php_str_appendc (buf, ';');
}

static inline void
serialize_string (php_str * buf, char *str, int len)
{
  php_str_appendl (buf, "s:", 2);
  php_str_append_long (buf, len);
  php_str_appendl (buf, ":\"", 2);
  php_str_appendl (buf, str, len);
  php_str_appendl (buf, "\";", 2);
}

static inline void
serialize_array (php_str * buf, VALUE arr)
{
  long arr_len = RARRAY_LEN (arr);
  php_str_appendl (buf, "a:", 2);
  php_str_append_long (buf, arr_len);
  php_str_appendl (buf, ":{", 2);
  for (long i = 0; i < arr_len; i++)
    {
      php_str_appendl (buf, "i:", 2);
      php_str_append_long (buf, i);
      php_str_appendl (buf, ";", 1);
      serialize_intern (buf, RARRAY_AREF (arr, i));
    }
  php_str_appendl (buf, "}", 1);
}

static inline int
rb_hash_each (VALUE key, VALUE val, VALUE rbuf)
{
  php_str buf = { 0 };
  serialize_intern (&buf, key);
  serialize_intern (&buf, val);
  rb_str_cat (rbuf, buf.c, buf.len);
  php_str_free (&buf);
  return ST_CONTINUE;
}

static inline void
serialize_hash (php_str * buf, VALUE hash)
{
  long hash_size;
  VALUE rbuf = rb_str_new_cstr ("");
  hash_size = RHASH_SIZE (hash);
  php_str_appendl (buf, "a:", 2);
  php_str_append_long (buf, hash_size);
  php_str_appendl (buf, ":{", 2);
  rb_hash_foreach (hash, rb_hash_each, rbuf);
  php_str_appendl (buf, StringValueCStr (rbuf), RSTRING_LEN (rbuf));
  php_str_appendl (buf, "}", 1);
}

static inline void
serialize_symbol (php_str * buf, VALUE sym)
{
  VALUE str = rb_sym_to_s (sym);
  serialize_string (buf, RSTRING_PTR (str), RSTRING_LEN (str));
}

static void
serialize_intern (php_str * buf, VALUE in_data)
{
  switch (TYPE (in_data))
    {
    case T_NIL:
      php_str_appendl (buf, "N;", 2);
      break;
    case T_TRUE:
      php_str_appendl (buf, "b:1;", 4);
      break;
    case T_FALSE:
      php_str_appendl (buf, "b:0;", 4);
      break;
    case T_FIXNUM:
      serialize_long (buf, FIX2LONG (in_data));
      break;
    case T_BIGNUM:
      serialize_long (buf, NUM2LONG (in_data));
      break;
    case T_STRING:
      serialize_string (buf, RSTRING_PTR (in_data), RSTRING_LEN (in_data));
      break;
    case T_SYMBOL:
      serialize_symbol (buf, in_data);
      break;
    case T_ARRAY:
      serialize_array (buf, in_data);
      break;
    case T_HASH:
      serialize_hash (buf, in_data);
      break;
    case T_FLOAT:
      serialize_float (buf, in_data);
      break;
    default:
      /* raise exception */
      rb_raise (rb_eTypeError, "%s is not valid value",
                RSTRING_PTR (rb_any_to_s (rb_obj_class (in_data))));
      break;
    }
}

static inline long
get_lval (char **src, char delimiter, long *count, long limit)
{
  long val = 0;
  int is_negative = 0;
  while (1)
    {
      if (*count >= limit)
        {
          rb_raise (rb_eTypeError, "Incomplete Serialization.");
        }
      else if (**src >= '0' && **src <= '9')
        {
          val = (val * 10) + (**src - 48);
        }
      else if (**src == delimiter)
        {
          if (is_negative)
            val *= -1;
          return val;
        }
      else if (**src == '-')
        {
          is_negative = 1;
        }
      else if (**src == '+')
        {
          //do nothing;
        }
      else
        {
          rb_raise (rb_eTypeError, "Integer is expected.");
        }
      (*src)++;
      (*count)++;
    }
}

static inline VALUE
get_dval (char **src, char delimiter, long *count, long limit)
{
  VALUE val = Qnil;
  php_str buf = { 0 };
  char *c;
  while (1)
    {
      c = *src;
      if (*count >= limit)
        {
          rb_raise (rb_eTypeError, "Incomplete Serialization.");
        }
      else if (*c >= '0' && *c <= '9')
        {
          php_str_appendl (&buf, c, 1);
        }
      else if (*c == delimiter)
        {
          val = rb_str_new (buf.c, buf.len);
          php_str_free (&buf);
          return DBL2NUM (rb_str_to_dbl (val, 0));
        }
      else if (*c == '-' || *c == '+' || *c == '.' || *c == 'e' || *c == 'E')
        {
          php_str_appendl (&buf, c, 1);
        }
      else
        {
          rb_raise (rb_eTypeError, "Malformed decimal value.");
        }
      (*src)++;
      (*count)++;
    }
}

static VALUE
unserialize_intern (char **str_ptr_ptr, long *count, long limit)
{
  char type;
  long _lval;
  int is_array = 1;
  VALUE val = Qnil;
  VALUE hash = Qnil;
  VALUE array = Qnil;
  VALUE h_key = Qnil;
  VALUE h_val = Qnil;

  if (*count >= limit)
    return val;

  type = **str_ptr_ptr;
  (*str_ptr_ptr) += 2;
  (*count) += 2;

  switch (type)
    {
    case 'i':
      _lval = get_lval (str_ptr_ptr, ';', count, limit);
      val = LONG2FIX (_lval);
      (*count)++;
      break;
    case 'b':
      _lval = get_lval (str_ptr_ptr, ';', count, limit);
      val = _lval ? Qtrue : Qfalse;
      (*count)++;
      break;
    case 's':
      _lval = get_lval (str_ptr_ptr, ':', count, limit);
      (*str_ptr_ptr) += 2;
      val = rb_str_new (*str_ptr_ptr, _lval);
      (*str_ptr_ptr) += (_lval + 1);
      (*count) += (_lval + 4);
      break;
    case 'd':
      val = get_dval (str_ptr_ptr, ';', count, limit);
      (*count)++;
      break;
    case 'N':
      //Do nothing for NULL;
      break;
    case 'a':
      _lval = get_lval (str_ptr_ptr, ':', count, limit);
      (*str_ptr_ptr) += 2;
      (*count) += 2;
      hash = rb_hash_new ();
      array = rb_ary_new ();
      for (long i = 0; i < _lval; i++)
        {
          h_key = unserialize_intern (str_ptr_ptr, count, limit);
          h_val = unserialize_intern (str_ptr_ptr, count, limit);
          rb_hash_aset (hash, h_key, h_val);
          if (TYPE (h_key) != T_FIXNUM || FIX2LONG (h_key) != i)
            is_array = 0;
          if (is_array)
            rb_ary_push (array, h_val);
        }
      val = is_array ? array : hash;
      (*count)++;
      break;
    default:
      rb_raise (rb_eTypeError, "Invalid Serialization.");
      break;
    }
  if (*count < limit && val != Qnil)
    (*str_ptr_ptr)++;
  return val;
}

static VALUE
serialize (VALUE module, VALUE input)
{
  php_str buf = { 0 };
  VALUE serialized_data = Qnil;
  serialize_intern (&buf, input);
  serialized_data = rb_str_new (buf.c, buf.len);
  php_str_free (&buf);
  return serialized_data;
}

static VALUE
unserialize (VALUE module, VALUE input)
{
  char *str_p = RSTRING_PTR (input);
  long count = 0, limit = RSTRING_LEN (input);
  return unserialize_intern (&str_p, &count, limit);
}
