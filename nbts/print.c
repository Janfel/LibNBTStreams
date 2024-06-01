#include <nbts/print.h>

#include <inttypes.h>
#include <string.h>

#if __clang__
#define nonnull  _Nonnull
#define nullable _Nullable
#else
#define nonnull
#define nullable
#endif

#define TRY(EXPR)                      \
	{                                  \
		enum nbts_error _err = (EXPR); \
		if (_err) return _err;         \
	}

#define TRYF(EXPR)                                \
	{                                             \
		if ((EXPR) == EOF) return NBTS_WRITE_ERR; \
	}

static int
xfwrite(void const *restrict nonnull ptr, size_t size, size_t count, FILE *restrict nonnull stream)
{
	if (fwrite(ptr, size, count, stream) != count) return EOF;
	return 1;
}

#define COVARIANT_CAST(FUNC, ...)                                                            \
	(_Generic(                                                                               \
		(&FUNC)((void *) 0, (nbts_strsize) 0, (FILE *restrict nonnull) 0),                   \
		enum nbts_error: (enum nbts_error(*)(void *, nbts_strsize, FILE *restrict nonnull))( \
			&FUNC)))

struct nbts_handler const nbts_print_handler = {
	.handle[NBTS_BYTE] = COVARIANT_CAST(nbts_print_handle_byte),
	.handle[NBTS_SHORT] = COVARIANT_CAST(nbts_print_handle_short),
	.handle[NBTS_INT] = COVARIANT_CAST(nbts_print_handle_int),
	.handle[NBTS_LONG] = COVARIANT_CAST(nbts_print_handle_long),
	.handle[NBTS_FLOAT] = COVARIANT_CAST(nbts_print_handle_float),
	.handle[NBTS_DOUBLE] = COVARIANT_CAST(nbts_print_handle_double),
	.handle[NBTS_STRING] = COVARIANT_CAST(nbts_print_handle_string),
	.handle[NBTS_BYTE_ARRAY] = COVARIANT_CAST(nbts_print_handle_byte_array),
	.handle[NBTS_INT_ARRAY] = COVARIANT_CAST(nbts_print_handle_int_array),
	.handle[NBTS_LONG_ARRAY] = COVARIANT_CAST(nbts_print_handle_long_array),
	.handle[NBTS_LIST] = COVARIANT_CAST(nbts_print_handle_list),
	.handle[NBTS_COMPOUND] = COVARIANT_CAST(nbts_print_handle_compound),
};

struct nbts_print_handler_data nbts_print_handler_data(FILE *nonnull ostream)
{
	return (struct nbts_print_handler_data){.ostream = ostream};
}

static inline enum nbts_error print_prefix(
	struct nbts_print_handler_data *restrict nonnull data,
	nbts_strsize name_size,
	FILE *restrict nonnull stream)
{
	if (data->index) {
		TRYF(fputs(", ", data->ostream));
	}

	if (name_size) {
		char quote = data->use_singlequotes ? '\'' : '"';
		TRY(nbts_fprint_string(data->ostream, stream, name_size, quote));
		TRYF(fputc(':', data->ostream));
	}

	return NBTS_OK;
}

enum nbts_error nbts_print_handle_byte(
	struct nbts_print_handler_data *restrict nonnull data,
	nbts_strsize name_size,
	FILE *restrict nonnull stream)
{
	TRY(print_prefix(data, name_size, stream));

	nbts_byte value = 0;
	TRY(nbts_parse_byte(&value, stream));
	TRY(nbts_fprint_byte(data->ostream, value));

	data->index += 1;
	return NBTS_OK;
}

enum nbts_error nbts_print_handle_short(
	struct nbts_print_handler_data *restrict nonnull data,
	nbts_strsize name_size,
	FILE *restrict nonnull stream)
{
	TRY(print_prefix(data, name_size, stream));

	nbts_short value = 0;
	TRY(nbts_parse_short(&value, stream));
	TRY(nbts_fprint_short(data->ostream, value));

	data->index += 1;
	return NBTS_OK;
}

enum nbts_error nbts_print_handle_int(
	struct nbts_print_handler_data *restrict nonnull data,
	nbts_strsize name_size,
	FILE *restrict nonnull stream)
{
	TRY(print_prefix(data, name_size, stream));

	nbts_int value = 0;
	TRY(nbts_parse_int(&value, stream));
	TRY(nbts_fprint_int(data->ostream, value));

	data->index += 1;
	return NBTS_OK;
}

enum nbts_error nbts_print_handle_long(
	struct nbts_print_handler_data *restrict nonnull data,
	nbts_strsize name_size,
	FILE *restrict nonnull stream)
{
	TRY(print_prefix(data, name_size, stream));

	nbts_long value = 0;
	TRY(nbts_parse_long(&value, stream));
	TRY(nbts_fprint_long(data->ostream, value));

	data->index += 1;
	return NBTS_OK;
}

enum nbts_error nbts_print_handle_float(
	struct nbts_print_handler_data *restrict nonnull data,
	nbts_strsize name_size,
	FILE *restrict nonnull stream)
{
	TRY(print_prefix(data, name_size, stream));

	nbts_float value = 0;
	TRY(nbts_parse_float(&value, stream));
	TRY(nbts_fprint_float(data->ostream, value));

	data->index += 1;
	return NBTS_OK;
}

enum nbts_error nbts_print_handle_double(
	struct nbts_print_handler_data *restrict nonnull data,
	nbts_strsize name_size,
	FILE *restrict nonnull stream)
{
	TRY(print_prefix(data, name_size, stream));

	nbts_double value = 0;
	TRY(nbts_parse_double(&value, stream));
	TRY(nbts_fprint_double(data->ostream, value));

	data->index += 1;
	return NBTS_OK;
}

enum nbts_error nbts_print_handle_string(
	struct nbts_print_handler_data *restrict nonnull data,
	nbts_strsize name_size,
	FILE *restrict nonnull stream)
{
	TRY(print_prefix(data, name_size, stream));

	nbts_strsize string_size = 0;
	TRY(nbts_parse_strsize(&string_size, stream));
	TRY(nbts_fprint_string(
		data->ostream, stream, string_size, data->use_singlequotes ? '\'' : '"'));

	data->index += 1;
	return NBTS_OK;
}

enum nbts_error nbts_print_handle_byte_array(
	struct nbts_print_handler_data *restrict nonnull data,
	nbts_strsize name_size,
	FILE *restrict nonnull stream)
{
	TRY(print_prefix(data, name_size, stream));

	nbts_size array_size = 0;
	TRY(nbts_parse_size(&array_size, stream));
	TRY(nbts_fprint_byte_array(data->ostream, stream, array_size));

	data->index += 1;
	return NBTS_OK;
}

enum nbts_error nbts_print_handle_int_array(
	struct nbts_print_handler_data *restrict nonnull data,
	nbts_strsize name_size,
	FILE *restrict nonnull stream)
{
	TRY(print_prefix(data, name_size, stream));

	nbts_size array_size = 0;
	TRY(nbts_parse_size(&array_size, stream));
	TRY(nbts_fprint_byte_array(data->ostream, stream, array_size));

	data->index += 1;
	return NBTS_OK;
}

enum nbts_error nbts_print_handle_long_array(
	struct nbts_print_handler_data *restrict nonnull data,
	nbts_strsize name_size,
	FILE *restrict nonnull stream)
{
	TRY(print_prefix(data, name_size, stream));

	nbts_size array_size = 0;
	TRY(nbts_parse_size(&array_size, stream));
	TRY(nbts_fprint_byte_array(data->ostream, stream, array_size));

	data->index += 1;
	return NBTS_OK;
}

enum nbts_error nbts_print_handle_list(
	struct nbts_print_handler_data *restrict nonnull data,
	nbts_strsize name_size,
	FILE *restrict nonnull stream)
{
	TRY(print_prefix(data, name_size, stream));

	enum nbts_type type = 0;
	TRY(nbts_parse_typeid(&type, stream));

	nbts_size size = 0;
	TRY(nbts_parse_size(&size, stream));

	TRYF(fputc('[', data->ostream));

	size_t index = data->index;
	data->index = 0;
	TRY(nbts_parse_list(type, size, stream, &nbts_print_handler, data));
	data->index = index;

	TRYF(fputc(']', data->ostream));

	data->index += 1;
	return NBTS_OK;
}

enum nbts_error nbts_print_handle_compound(
	struct nbts_print_handler_data *restrict nonnull data,
	nbts_strsize name_size,
	FILE *restrict nonnull stream)
{
	TRY(print_prefix(data, name_size, stream));

	TRYF(fputc('{', data->ostream));

	size_t index = data->index;
	data->index = 0;
	TRY(nbts_parse_compound(stream, &nbts_print_handler, data));
	data->index = index;

	TRYF(fputc('}', data->ostream));

	data->index += 1;
	return NBTS_OK;
}


enum nbts_error nbts_fprint_bool(FILE *restrict nonnull stream, nbts_byte x)
{
	TRYF(x ? fputs("true", stream) : fputs("false", stream));
	return NBTS_OK;
}

enum nbts_error nbts_fprint_byte(FILE *restrict nonnull stream, nbts_byte x)
{
	TRYF(fprintf(stream, "%" PRId8 "B", x));
	return NBTS_OK;
}

enum nbts_error nbts_fprint_short(FILE *restrict nonnull stream, nbts_short x)
{
	TRYF(fprintf(stream, "%" PRId16 "S", x));
	return NBTS_OK;
}

enum nbts_error nbts_fprint_int(FILE *restrict nonnull stream, nbts_int x)
{
	TRYF(fprintf(stream, "%" PRId32, x));
	return NBTS_OK;
}

enum nbts_error nbts_fprint_long(FILE *restrict nonnull stream, nbts_long x)
{
	TRYF(fprintf(stream, "%" PRId64 "L", x));
	return NBTS_OK;
}

enum nbts_error nbts_fprint_float(FILE *restrict nonnull stream, nbts_float x)
{
	TRYF(fprintf(stream, "%fF", x));
	return NBTS_OK;
}

enum nbts_error nbts_fprint_double(FILE *restrict nonnull stream, nbts_double x)
{
	TRYF(fprintf(stream, "%f", x));
	return NBTS_OK;
}

// NOLINTBEGIN(bugprone-easily-swappable-parameters)

static nbts_char const *
fprint_memchr(nbts_char const *restrict string, size_t string_size, char quote)
{
	nbts_char const *bp = memchr(string, '\\', string_size);
	nbts_char const *qp = memchr(string, quote, string_size);
	if (bp && qp) return bp < qp ? bp : qp;
	return bp ? bp : qp;
}

static enum nbts_error fprint_substring(
	FILE *restrict nonnull ostream,
	nbts_char const *restrict string,
	size_t string_size,
	char quote)
{
	nbts_char const *a = string;
	nbts_char const *z = &string[string_size];
	for (nbts_char const *b = nullptr; (b = fprint_memchr(a, z - a, quote)); a = b + 1) {
		TRYF(xfwrite(a, sizeof(*a), b - a, ostream));
		TRYF(fputc('\\', ostream));
		TRYF(fputc(*b, ostream));
	}

	TRYF(xfwrite(a, sizeof(*a), z - a, ostream));

	return NBTS_OK;
}

enum nbts_error nbts_fprint_string(
	FILE *restrict nonnull ostream, FILE *restrict nonnull istream, size_t string_size, char quote)
{
	enum : size_t { BUFSIZE = NBTS_STACK_BUFFER_SIZE / sizeof(nbts_char) };

	TRYF(fputc(quote, ostream));

	nbts_char substr[BUFSIZE];
	for (size_t i = 0; i < (string_size / BUFSIZE) + 1; ++i) {
		size_t rest_size = string_size - i * BUFSIZE;
		size_t substr_size = rest_size < BUFSIZE ? rest_size : BUFSIZE;
		TRY(nbts_parse_string(substr, substr_size, istream));
		TRY(fprint_substring(ostream, substr, substr_size, quote));
	}

	TRYF(fputc(quote, ostream));

	return NBTS_OK;
}

// NOLINTEND(bugprone-easily-swappable-parameters)

static enum nbts_error fprint_byte_subarray(
	FILE *restrict nonnull ostream, nbts_byte const *restrict array, size_t array_size)
{
	for (size_t i = 0; i < array_size; ++i) {
		if (i) TRYF(fputc(',', ostream));
		TRY(nbts_fprint_byte(ostream, array[i]));
	}
	return NBTS_OK;
}

static enum nbts_error fprint_int_subarray(
	FILE *restrict nonnull ostream, nbts_int const *restrict array, size_t array_size)
{
	for (size_t i = 0; i < array_size; ++i) {
		if (i) TRYF(fputc(',', ostream));
		TRY(nbts_fprint_int(ostream, array[i]));
	}
	return NBTS_OK;
}

static enum nbts_error fprint_long_subarray(
	FILE *restrict nonnull ostream, nbts_long const *restrict array, size_t array_size)
{
	for (size_t i = 0; i < array_size; ++i) {
		if (i) TRYF(fputc(',', ostream));
		TRY(nbts_fprint_long(ostream, array[i]));
	}
	return NBTS_OK;
}

enum nbts_error nbts_fprint_byte_array(
	FILE *restrict nonnull ostream, FILE *restrict nonnull istream, size_t array_size)
{
	enum : size_t { BUFSIZE = NBTS_STACK_BUFFER_SIZE / sizeof(nbts_byte) };

	TRYF(fputs("[B;", ostream));

	nbts_byte subarr[BUFSIZE];
	for (size_t i = 0; i < (array_size / BUFSIZE) + 1; ++i) {
		if (i) TRYF(fputc(',', ostream));
		size_t rest_size = array_size - i * BUFSIZE;
		size_t subarr_size = rest_size < BUFSIZE ? rest_size : BUFSIZE;
		TRY(nbts_parse_byte_array(subarr, subarr_size, istream));
		TRY(fprint_byte_subarray(ostream, subarr, subarr_size));
	}

	TRYF(fputc(']', ostream));

	return NBTS_OK;
}

enum nbts_error nbts_fprint_int_array(
	FILE *restrict nonnull ostream, FILE *restrict nonnull istream, size_t array_size)
{
	enum : size_t { BUFSIZE = NBTS_STACK_BUFFER_SIZE / sizeof(nbts_int) };

	TRYF(fputs("[I;", ostream));

	nbts_int subarr[BUFSIZE];
	for (size_t i = 0; i < (array_size / BUFSIZE) + 1; ++i) {
		if (i) TRYF(fputc(',', ostream));
		size_t rest_size = array_size - i * BUFSIZE;
		size_t subarr_size = rest_size < BUFSIZE ? rest_size : BUFSIZE;
		TRY(nbts_parse_int_array(subarr, subarr_size, istream));
		TRY(fprint_int_subarray(ostream, subarr, subarr_size));
	}

	TRYF(fputc(']', ostream));

	return NBTS_OK;
}

enum nbts_error nbts_fprint_long_array(
	FILE *restrict nonnull ostream, FILE *restrict nonnull istream, size_t array_size)
{
	enum : size_t { BUFSIZE = NBTS_STACK_BUFFER_SIZE / sizeof(nbts_long) };

	TRYF(fputs("[L;", ostream));

	nbts_long subarr[BUFSIZE];
	for (size_t i = 0; i < (array_size / BUFSIZE) + 1; ++i) {
		if (i) TRYF(fputc(',', ostream));
		size_t rest_size = array_size - i * BUFSIZE;
		size_t subarr_size = rest_size < BUFSIZE ? rest_size : BUFSIZE;
		TRY(nbts_parse_long_array(subarr, subarr_size, istream));
		TRY(fprint_long_subarray(ostream, subarr, subarr_size));
	}

	TRYF(fputc(']', ostream));

	return NBTS_OK;
}
