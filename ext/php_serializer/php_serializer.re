#include<ruby.h>
#include<php_serializer.h>

#define YYFILL(n) do { } while (0)
#define YYCTYPE unsigned char
#define YYCURSOR cursor
#define YYLIMIT limit
#define YYMARKER marker

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

static inline long parse_iv(unsigned char *p)
{
  char cursor;
  long result = 0;
  int neg = 0;

  switch (*p) {
    case '-':
      neg++;
      /* fall-through */
    case '+':
      p++;
  }

  while (1) {
    cursor = (char)*p;
    if (cursor >= '0' && cursor <= '9') {
      result = result * 10 + (unsigned int)(cursor - (unsigned char)'0');
    } else {
      break;
    }
    p++;
  }
  if (neg) return -result;
  return result;
}

static inline unsigned int parse_uiv(unsigned char *p)
{
  unsigned char cursor;
  unsigned int result = 0;

  if (*p == '+') {
    p++;
	}

  while (1) {
    cursor = *p;
    if (cursor >= '0' && cursor <= '9') {
      result = result * 10 + (unsigned int)(cursor - (unsigned char)'0');
    } else {
      break;
    }
    p++;
  }
  return result;
}

static inline void raise_incomplete_serialization()
{
  rb_raise (rb_eTypeError, "Incomplete Serialization.");
}


/*!re2c
uiv = [+]? [0-9]+;
iv = [+-]? [0-9]+;
nv = [+-]? ([0-9]* "." [0-9]+|[0-9]+ "." [0-9]*);
nvexp = (iv | nv) [eE] [+-]? iv;
any = [\000-\377];
object = [OC];
*/
static VALUE
unserialize_intern (unsigned char **p, unsigned char *max)
{
  VALUE rval = Qnil;
  unsigned char *cursor, *limit, *marker, *start;

  limit = max;
  cursor = *p;

	if (YYCURSOR >= YYLIMIT) {
		return 0;
	}

	start = cursor;

/*!re2c

"N;"	{
  *p = YYCURSOR;
	return rval;
}

"b:" [01] ";"	{
  *p = YYCURSOR;
  rval = parse_iv(start + 2) ? Qtrue : Qfalse;
	return rval;
}

"i:" iv ";"	{
  *p = YYCURSOR;
  rval = LONG2FIX (parse_iv(start + 2));
	return rval;
}

"d:" ("NAN" | "-"? "INF") ";"	{
	const char *str;

  *p = YYCURSOR;
	str = (const char *) start + 2;
  if (!strncmp(str, "NAN", 3)) {
   rval = rb_const_get(rb_cFloat, rb_intern("NAN"));
  } else if (!strncmp(str, "INF", 3)) {
    rval = rb_const_get(rb_cFloat, rb_intern("INFINITY"));
  } else if (!strncmp(str, "-INF", 4)) {
   rval = DBL2NUM(-RFLOAT_VALUE(rb_const_get(rb_cFloat, rb_intern("INFINITY"))));
  }
  return rval;
}

"d:" (iv | nv | nvexp) ";"	{
  long str_len;
	VALUE rb_str;
	const char *str_p;

  *p = YYCURSOR;
	str_p = (const char *) start + 2;
  str_len = YYCURSOR - start - 3;
  rb_str = rb_str_new (str_p, str_len);
  rval = DBL2NUM (rb_str_to_dbl (rb_str, 0));
  return rval;
}

"s:" uiv ":" ["] 	{
  unsigned int len, maxlen;
  char *str;

  len = parse_uiv(start + 2);
  maxlen = max - YYCURSOR;
  if (maxlen < len) {
   raise_incomplete_serialization();
  }

  str = (char*)YYCURSOR;
  YYCURSOR += len;

  if (*(YYCURSOR) != '"') {
   raise_incomplete_serialization();
  }

  if (*(YYCURSOR + 1) != ';') {
   raise_incomplete_serialization();
  }

  YYCURSOR += 2;
  *p = YYCURSOR;
  rval = rb_str_new (str, len);
  return rval;
}

"a:" uiv ":" "{" {
  VALUE hash = Qnil;
  VALUE array = Qnil;
  VALUE h_key = Qnil;
  VALUE h_val = Qnil;
  int is_array = true;
  long elements;

  *p = YYCURSOR;

  elements = parse_iv(start + 2);
  hash = rb_hash_new ();
  array = rb_ary_new ();
  for (long i = 0; i < elements; i++)
  {
    if (*p >= max)
      rb_raise(rb_eTypeError, "Incomplete array.");
    h_key = unserialize_intern (p, max);

    if (*p >= max)
      rb_raise(rb_eTypeError, "Incomplete array.");
    h_val = unserialize_intern (p, max);

    rb_hash_aset (hash, h_key, h_val);
    if (TYPE (h_key) != T_FIXNUM || FIX2LONG (h_key) != i)
      is_array = false;

    if (is_array)
      rb_ary_push (array, h_val);
  }
  if (is_array && RARRAY_LEN(array) != elements) {
    rb_raise(rb_eTypeError, "Incomplete array.");
  } else if (!is_array && RHASH_SIZE(hash) != elements) {
    rb_raise(rb_eTypeError, "Incomplete hash.");
  }
  *p += 1;
  rval = is_array ? array : hash;
  return rval;
}

any	{ rb_raise (rb_eTypeError, "Invalid Serialization."); }
*/

return Qnil;
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
  unsigned char *str_p = (unsigned char *) RSTRING_PTR (input);
  long str_len = RSTRING_LEN (input);
  return unserialize_intern (&str_p, str_p + str_len);
}
