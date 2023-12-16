/* Generated by re2c 3.1 on Sat Dec 16 19:17:02 2023 */
#line 1 "ext/php_serializer/php_serializer.re"
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


#line 201 "ext/php_serializer/php_serializer.re"

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


#line 215 "ext/php_serializer/php_serializer.c"
{
	YYCTYPE yych;
	if ((YYLIMIT - YYCURSOR) < 7) YYFILL(7);
	yych = *YYCURSOR;
	switch (yych) {
		case 'N': goto yy3;
		case 'a': goto yy4;
		case 'b': goto yy5;
		case 'd': goto yy6;
		case 'i': goto yy7;
		case 's': goto yy8;
		default: goto yy1;
	}
yy1:
	++YYCURSOR;
yy2:
#line 331 "ext/php_serializer/php_serializer.re"
	{ rb_raise (rb_eTypeError, "Invalid Serialization."); }
#line 234 "ext/php_serializer/php_serializer.c"
yy3:
	yych = *++YYCURSOR;
	switch (yych) {
		case ';': goto yy9;
		default: goto yy2;
	}
yy4:
	yych = *(YYMARKER = ++YYCURSOR);
	switch (yych) {
		case ':': goto yy10;
		default: goto yy2;
	}
yy5:
	yych = *(YYMARKER = ++YYCURSOR);
	switch (yych) {
		case ':': goto yy12;
		default: goto yy2;
	}
yy6:
	yych = *(YYMARKER = ++YYCURSOR);
	switch (yych) {
		case ':': goto yy13;
		default: goto yy2;
	}
yy7:
	yych = *(YYMARKER = ++YYCURSOR);
	switch (yych) {
		case ':': goto yy14;
		default: goto yy2;
	}
yy8:
	yych = *(YYMARKER = ++YYCURSOR);
	switch (yych) {
		case ':': goto yy15;
		default: goto yy2;
	}
yy9:
	++YYCURSOR;
#line 219 "ext/php_serializer/php_serializer.re"
	{
  *p = YYCURSOR;
	return rval;
}
#line 278 "ext/php_serializer/php_serializer.c"
yy10:
	yych = *++YYCURSOR;
	switch (yych) {
		case '+': goto yy16;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9': goto yy17;
		default: goto yy11;
	}
yy11:
	YYCURSOR = YYMARKER;
	goto yy2;
yy12:
	yych = *++YYCURSOR;
	switch (yych) {
		case '0':
		case '1': goto yy18;
		default: goto yy11;
	}
yy13:
	yych = *++YYCURSOR;
	switch (yych) {
		case '+': goto yy19;
		case '-': goto yy20;
		case '.': goto yy21;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9': goto yy22;
		case 'I': goto yy23;
		case 'N': goto yy24;
		default: goto yy11;
	}
yy14:
	yych = *++YYCURSOR;
	switch (yych) {
		case '+':
		case '-': goto yy25;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9': goto yy26;
		default: goto yy11;
	}
yy15:
	yych = *++YYCURSOR;
	switch (yych) {
		case '+': goto yy27;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9': goto yy28;
		default: goto yy11;
	}
yy16:
	yych = *++YYCURSOR;
	switch (yych) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9': goto yy17;
		default: goto yy11;
	}
yy17:
	++YYCURSOR;
	if ((YYLIMIT - YYCURSOR) < 2) YYFILL(2);
	yych = *YYCURSOR;
	switch (yych) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9': goto yy17;
		case ':': goto yy29;
		default: goto yy11;
	}
yy18:
	yych = *++YYCURSOR;
	switch (yych) {
		case ';': goto yy30;
		default: goto yy11;
	}
yy19:
	yych = *++YYCURSOR;
	switch (yych) {
		case '.': goto yy21;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9': goto yy22;
		default: goto yy11;
	}
yy20:
	yych = *++YYCURSOR;
	switch (yych) {
		case '.': goto yy21;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9': goto yy22;
		case 'I': goto yy23;
		default: goto yy11;
	}
yy21:
	yych = *++YYCURSOR;
	switch (yych) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9': goto yy31;
		default: goto yy11;
	}
yy22:
	++YYCURSOR;
	if ((YYLIMIT - YYCURSOR) < 4) YYFILL(4);
	yych = *YYCURSOR;
	switch (yych) {
		case '.': goto yy31;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9': goto yy22;
		case ';': goto yy32;
		case 'E':
		case 'e': goto yy33;
		default: goto yy11;
	}
yy23:
	yych = *++YYCURSOR;
	switch (yych) {
		case 'N': goto yy34;
		default: goto yy11;
	}
yy24:
	yych = *++YYCURSOR;
	switch (yych) {
		case 'A': goto yy35;
		default: goto yy11;
	}
yy25:
	yych = *++YYCURSOR;
	switch (yych) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9': goto yy26;
		default: goto yy11;
	}
yy26:
	++YYCURSOR;
	if (YYLIMIT <= YYCURSOR) YYFILL(1);
	yych = *YYCURSOR;
	switch (yych) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9': goto yy26;
		case ';': goto yy36;
		default: goto yy11;
	}
yy27:
	yych = *++YYCURSOR;
	switch (yych) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9': goto yy28;
		default: goto yy11;
	}
yy28:
	++YYCURSOR;
	if ((YYLIMIT - YYCURSOR) < 2) YYFILL(2);
	yych = *YYCURSOR;
	switch (yych) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9': goto yy28;
		case ':': goto yy37;
		default: goto yy11;
	}
yy29:
	yych = *++YYCURSOR;
	switch (yych) {
		case '{': goto yy38;
		default: goto yy11;
	}
yy30:
	++YYCURSOR;
#line 224 "ext/php_serializer/php_serializer.re"
	{
  *p = YYCURSOR;
  rval = parse_iv(start + 2) ? Qtrue : Qfalse;
	return rval;
}
#line 558 "ext/php_serializer/php_serializer.c"
yy31:
	++YYCURSOR;
	if ((YYLIMIT - YYCURSOR) < 4) YYFILL(4);
	yych = *YYCURSOR;
	switch (yych) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9': goto yy31;
		case ';': goto yy32;
		case 'E':
		case 'e': goto yy33;
		default: goto yy11;
	}
yy32:
	++YYCURSOR;
#line 251 "ext/php_serializer/php_serializer.re"
	{
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
#line 594 "ext/php_serializer/php_serializer.c"
yy33:
	yych = *++YYCURSOR;
	switch (yych) {
		case '+':
		case '-': goto yy39;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9': goto yy40;
		default: goto yy11;
	}
yy34:
	yych = *++YYCURSOR;
	switch (yych) {
		case 'F': goto yy41;
		default: goto yy11;
	}
yy35:
	yych = *++YYCURSOR;
	switch (yych) {
		case 'N': goto yy41;
		default: goto yy11;
	}
yy36:
	++YYCURSOR;
#line 230 "ext/php_serializer/php_serializer.re"
	{
  *p = YYCURSOR;
  rval = LONG2FIX (parse_iv(start + 2));
	return rval;
}
#line 632 "ext/php_serializer/php_serializer.c"
yy37:
	yych = *++YYCURSOR;
	switch (yych) {
		case '"': goto yy42;
		default: goto yy11;
	}
yy38:
	++YYCURSOR;
#line 291 "ext/php_serializer/php_serializer.re"
	{
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
#line 681 "ext/php_serializer/php_serializer.c"
yy39:
	yych = *++YYCURSOR;
	switch (yych) {
		case '+':
		case '-': goto yy43;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9': goto yy40;
		default: goto yy11;
	}
yy40:
	++YYCURSOR;
	if (YYLIMIT <= YYCURSOR) YYFILL(1);
	yych = *YYCURSOR;
	switch (yych) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9': goto yy40;
		case ';': goto yy32;
		default: goto yy11;
	}
yy41:
	yych = *++YYCURSOR;
	switch (yych) {
		case ';': goto yy44;
		default: goto yy11;
	}
yy42:
	++YYCURSOR;
#line 264 "ext/php_serializer/php_serializer.re"
	{
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
#line 752 "ext/php_serializer/php_serializer.c"
yy43:
	yych = *++YYCURSOR;
	switch (yych) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9': goto yy40;
		default: goto yy11;
	}
yy44:
	++YYCURSOR;
#line 236 "ext/php_serializer/php_serializer.re"
	{
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
#line 785 "ext/php_serializer/php_serializer.c"
}
#line 332 "ext/php_serializer/php_serializer.re"


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
