#ifndef PHP_SERIALIZER_H
#define PHP_SERIALIZER_H
#include <sys/types.h>

typedef struct
{
  char *c;
  size_t len;
  size_t a;                     //allocated_size
} php_str;

static VALUE serialize (VALUE module, VALUE in_data);
static VALUE unserialize (VALUE module, VALUE in_data);
static void serialize_intern (php_str * buf, VALUE in_data);
static VALUE unserialize_intern (char **str_p, long *count, long limit);

#define PHP_STR_PREALLOC 128
#define PHP_STR_START_SIZE 78
#define PHP_STR_REALLOC(a,b) realloc((a),(b))

#define php_str_0(x) do {                     \
	  if ((x)->c) {                         \
			    (x)->c[(x)->len] = '\0';                  \
			  }                               \
} while (0)

#define php_str_appendc(dest, c) \
	  php_str_appendc_ex((dest), (c))

#define php_str_appendc_ex(dest, ch) do {         \
	  register size_t __nl;                     \
	  php_str_alloc4((dest), 1, __nl);            \
	  (dest)->len = __nl;                       \
	  ((unsigned char *) (dest)->c)[(dest)->len - 1] = (ch);      \
} while (0)

#define php_STR_DO_REALLOC(d) \
	  (d)->c = PHP_STR_REALLOC((d)->c, (d)->a + 1)

#define php_str_alloc4(d, n, newlen) do {         \
	if (!(d)->c) {                          \
		(d)->len = 0;                       \
		newlen = (n);                       \
		(d)->a = newlen < PHP_STR_START_SIZE            \
		? PHP_STR_START_SIZE                \
		: (newlen >= (INT_MAX - PHP_STR_PREALLOC)? newlen \
				: (newlen + PHP_STR_PREALLOC));   \
		php_STR_DO_REALLOC(d);                \
	} else {                            \
		newlen = (d)->len + (n);                  \
		if (newlen >= (d)->a) {                   \
			(d)->a = newlen + PHP_STR_PREALLOC;         \
			if ((d)->a >= INT_MAX) {          \
				rb_raise(rb_eTypeError, "String size overflow");  \
			}                           \
			php_STR_DO_REALLOC(d);              \
		}                             \
	}                               \
} while (0)

#define php_str_appendl(dest, src, len) \
	php_str_appendl_ex((dest), (src), (len))

#define php_str_appendl_ex(dest, src, nlen) do {      \
	register size_t __nl;                     \
	php_str *__dest = (php_str *) (dest);           \
	\
	php_str_alloc4(__dest, (nlen), __nl);         \
	memcpy(__dest->c + __dest->len, (src), (nlen));         \
	__dest->len = __nl;                       \
} while (0)

/* input: buf points to the END of the buffer */
#define php_str_print_unsigned4(buf, num, vartype, result) do { \
	  char *__p = (buf);                        \
	  vartype __num = (num);                      \
	  *__p = '\0';                          \
	  do {                              \
			    *--__p = (char) (__num % 10) + '0';             \
			    __num /= 10;                        \
			  } while (__num > 0);                      \
	  result = __p;                         \
} while (0)

/* buf points to the END of the buffer */
#define php_str_print_long4(buf, num, vartype, result) do { \
	  if (num < 0) {                          \
			    /* this might cause problems when dealing with LONG_MIN   \
					 *        and machines which don't support long long.  Works   \
					 *               flawlessly on 32bit x86 */               \
			    php_str_print_unsigned4((buf), -(num), vartype, (result));  \
			    *--(result) = '-';                      \
			  } else {                            \
					    php_str_print_unsigned4((buf), (num), vartype, (result)); \
					  }                               \
} while (0)

#define php_str_append_generic_ex(dest, num, vartype, func) do {  \
	  char __b[32];                             \
	  char *__t;                                \
	    php_str_print##func##4 (__b + sizeof(__b) - 1, (num), vartype, __t);  \
	  php_str_appendl_ex((dest), __t, __b + sizeof(__b) - 1 - __t); \
} while (0)

#define php_str_append_long(dest, val) \
	  php_str_append_long_ex((dest), (val))

#define php_str_append_long_ex(dest, num) \
	  php_str_append_generic_ex((dest), (num), unsigned long, _long)

#define php_str_free(s) \
	  php_str_free_ex((s))

#define php_str_free_ex(s) do {               \
	  php_str *__s = (php_str *) (s);               \
	  if (__s->c) {                         \
			    free(__s->c);                   \
			    __s->c = NULL;                        \
			  }                               \
	  __s->a = __s->len = 0;                      \
} while (0)

#endif
