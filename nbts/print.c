#include <nbts/print.h>

#include <inttypes.h>
#include <stdlib.h>
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

#define FPRINT_BUFSIZE NBTS_MAX_STACK_BUFFER_SIZE
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

	return 0;
}

enum nbts_error nbts_fprint_string(
	FILE *restrict nonnull ostream, FILE *restrict nonnull istream, size_t string_size, char quote)
{
	TRYF(fputc(quote, ostream));

	nbts_char substr[FPRINT_BUFSIZE];
	for (size_t i = 0; i < (string_size / FPRINT_BUFSIZE) + 1; ++i) {
		size_t rest_size = string_size - i * FPRINT_BUFSIZE;
		size_t substr_size = rest_size < FPRINT_BUFSIZE ? rest_size : FPRINT_BUFSIZE;
		TRY(nbts_parse_string(substr, substr_size, istream));
		TRY(fprint_substring(ostream, substr, substr_size, quote));
	}

	TRYF(fputc(quote, ostream));

	return 0;
}

// NOLINTEND(bugprone-easily-swappable-parameters)

enum nbts_error
nbts_fprint_byte_array(FILE *restrict nonnull ostream, nbts_byte const *restrict array, size_t array_size)
{
	TRYF(fputs("[B;", stream));

	for (size_t i = 0; i < array_size; ++i) {
		if (i) TRYF(fputc(',', stream));
		TRY(nbts_fprint_byte(stream, array[i]));
	}

	TRYF(fputc(']', stream));

	return 0;
}

enum nbts_error
nbts_fprint_int_array(FILE *restrict nonnull ostream, nbts_int const *restrict array, size_t array_size)
{
	TRYF(fputs("[I;", stream));

	for (size_t i = 0; i < array_size; ++i) {
		if (i) TRYF(fputc(',', stream));
		TRY(nbts_fprint_int(stream, array[i]));
	}

	TRYF(fputc(']', stream));

	return 0;
}

enum nbts_error
nbts_fprint_long_array(FILE *restrict nonnull ostream, nbts_long const *restrict array, size_t array_size)
{
	TRYF(fputs("[L;", stream));

	for (size_t i = 0; i < array_size; ++i) {
		if (i) TRYF(fputc(',', stream));
		TRY(nbts_fprint_long(stream, array[i]));
	}

	TRYF(fputc(']', stream));

	return 0;
}

#define HANDLER_WRAPPER(TYPE)                                            \
	static enum nbts_error handle_##TYPE(                                \
		void *userdata, size_t name_size, FILE *restrict nonnull stream) \
	{                                                                    \
		return nbts_print_handle_##TYPE(userdata, name_size, stream);    \
	}

HANDLER_WRAPPER(byte)
HANDLER_WRAPPER(short)
HANDLER_WRAPPER(int)
HANDLER_WRAPPER(long)
HANDLER_WRAPPER(float)
HANDLER_WRAPPER(double)
HANDLER_WRAPPER(string)
HANDLER_WRAPPER(byte_array)
HANDLER_WRAPPER(int_array)
HANDLER_WRAPPER(long_array)
HANDLER_WRAPPER(list)
HANDLER_WRAPPER(compound)

struct nbts_handler const nbts_print_handler = {
	.handle_byte = &handle_byte,
	.handle_short = &handle_short,
	.handle_int = &handle_int,
	.handle_long = &handle_long,
	.handle_float = &handle_float,
	.handle_double = &handle_double,
	.handle_string = &handle_string,
	.handle_byte_array = &handle_byte_array,
	.handle_int_array = &handle_int_array,
	.handle_long_array = &handle_long_array,
	.handle_list = &handle_list,
	.handle_compound = &handle_compound,
};

struct nbts_print_handler_data nbts_print_handler_data(FILE *nonnull ostream)
{
	return (struct nbts_print_handler_data){.ostream = ostream};
}

#define PREAMBLE()                                                        \
	size_t index = data->index++;                                         \
	if (index) {                                                          \
		TRYF(fputs(", ", data->ostream));                                 \
	}                                                                     \
	if (name_size) {                                                      \
		char quote = data->use_singlequotes ? '\'' : '"';                 \
		TRY(nbts_fprint_string(data->ostream, stream, name_size, quote)); \
		TRYF(fputc(':', data->ostream));                                  \
	}

enum nbts_error nbts_print_handle_byte(
	struct nbts_print_handler_data *restrict nonnull data,
	size_t name_size,
	FILE *restrict nonnull stream)
{
	PREAMBLE();

	nbts_byte value = 0;
	TRY(nbts_parse_byte(&value, stream));
	TRY(nbts_fprint_byte(data->ostream, value));
	return NBTS_OK;
}

enum nbts_error nbts_print_handle_short(
	struct nbts_print_handler_data *restrict nonnull data,
	size_t name_size,
	FILE *restrict nonnull stream)
{
	PREAMBLE();

	nbts_short value = 0;
	TRY(nbts_parse_short(&value, stream));
	TRY(nbts_fprint_short(data->ostream, value));
	return NBTS_OK;
}

enum nbts_error nbts_print_handle_int(
	struct nbts_print_handler_data *restrict nonnull data,
	size_t name_size,
	FILE *restrict nonnull stream)
{
	PREAMBLE();

	nbts_int value = 0;
	TRY(nbts_parse_int(&value, stream));
	TRY(nbts_fprint_int(data->ostream, value));
	return NBTS_OK;
}

enum nbts_error nbts_print_handle_long(
	struct nbts_print_handler_data *restrict nonnull data,
	size_t name_size,
	FILE *restrict nonnull stream)
{
	PREAMBLE();

	nbts_long value = 0;
	TRY(nbts_parse_long(&value, stream));
	TRY(nbts_fprint_long(data->ostream, value));
	return NBTS_OK;
}

enum nbts_error nbts_print_handle_float(
	struct nbts_print_handler_data *restrict nonnull data,
	size_t name_size,
	FILE *restrict nonnull stream)
{
	PREAMBLE();

	nbts_float value = 0;
	TRY(nbts_parse_float(&value, stream));
	TRY(nbts_fprint_float(data->ostream, value));
	return NBTS_OK;
}

enum nbts_error nbts_print_handle_double(
	struct nbts_print_handler_data *restrict nonnull data,
	size_t name_size,
	FILE *restrict nonnull stream)
{
	PREAMBLE();

	nbts_double value = 0;
	TRY(nbts_parse_double(&value, stream));
	TRY(nbts_fprint_double(data->ostream, value));
	return NBTS_OK;
}

enum nbts_error nbts_print_handle_string(
	struct nbts_print_handler_data *restrict nonnull data,
	size_t name_size,
	FILE *restrict nonnull stream)
{
	PREAMBLE();

	nbts_strsize string_size = 0;
	TRY(nbts_parse_strsize(&string_size, stream));
	TRY(nbts_fprint_string(
		data->ostream, stream, string_size, data->use_singlequotes ? '\'' : '"'));

	return NBTS_OK;
}

enum nbts_error nbts_print_handle_byte_array(
	struct nbts_print_handler_data *restrict nonnull data,
	size_t name_size,
	FILE *restrict nonnull stream)
{
	PREAMBLE();

	nbts_size array_size = 0;
	TRY(nbts_parse_size(&array_size, stream));

	if ((size_t) array_size < NBTS_MAX_STACK_BUFFER_SIZE) {
		nbts_byte array[array_size + 1];
		TRY(nbts_parse_byte_array(array, array_size, stream));
		TRY(nbts_fprint_byte_array(data->ostream, array, array_size));
		return NBTS_OK;
	}

	nbts_byte *array = malloc(array_size * sizeof(*array));
	if (!array) return NBTS_BAD_ALLOC;

	enum nbts_error err = NBTS_OK;
	if ((err = nbts_parse_byte_array(array, array_size, stream))) goto cleanup;
	if ((err = nbts_fprint_byte_array(data->ostream, array, array_size))) goto cleanup;

cleanup:
	free(array);
	return err;
}

enum nbts_error nbts_print_handle_int_array(
	struct nbts_print_handler_data *restrict nonnull data,
	size_t name_size,
	FILE *restrict nonnull stream)
{
	PREAMBLE();

	nbts_size array_size = 0;
	TRY(nbts_parse_size(&array_size, stream));

	if ((size_t) array_size < NBTS_MAX_STACK_BUFFER_SIZE) {
		nbts_int array[array_size + 1];
		TRY(nbts_parse_int_array(array, array_size, stream));
		TRY(nbts_fprint_int_array(data->ostream, array, array_size));
		return NBTS_OK;
	}

	nbts_int *array = malloc(array_size * sizeof(*array));
	if (!array) return NBTS_BAD_ALLOC;

	enum nbts_error err = NBTS_OK;
	if ((err = nbts_parse_int_array(array, array_size, stream))) goto cleanup;
	if ((err = nbts_fprint_int_array(data->ostream, array, array_size))) goto cleanup;

cleanup:
	free(array);
	return err;
}

enum nbts_error nbts_print_handle_long_array(
	struct nbts_print_handler_data *restrict nonnull data,
	size_t name_size,
	FILE *restrict nonnull stream)
{
	PREAMBLE();

	nbts_size array_size = 0;
	TRY(nbts_parse_size(&array_size, stream));

	if ((size_t) array_size < NBTS_MAX_STACK_BUFFER_SIZE) {
		nbts_long array[array_size + 1];
		TRY(nbts_parse_long_array(array, array_size, stream));
		TRY(nbts_fprint_long_array(data->ostream, array, array_size));
		return NBTS_OK;
	}

	nbts_long *array = malloc(array_size * sizeof(*array));
	if (!array) return NBTS_BAD_ALLOC;

	enum nbts_error err = NBTS_OK;
	if ((err = nbts_parse_long_array(array, array_size, stream))) goto cleanup;
	if ((err = nbts_fprint_long_array(data->ostream, array, array_size))) goto cleanup;

cleanup:
	free(array);
	return err;
}

enum nbts_error nbts_print_handle_list(
	struct nbts_print_handler_data *restrict nonnull data,
	size_t name_size,
	FILE *restrict nonnull stream)
{
	PREAMBLE();

	enum nbts_type type = 0;
	TRY(nbts_parse_typeid(&type, stream));

	nbts_size size = 0;
	TRY(nbts_parse_size(&size, stream));

	TRYF(fputc('[', data->ostream));

	data->index = 0;
	TRY(nbts_parse_list(type, size, stream, &nbts_print_handler, data));
	data->index = index + 1;

	TRYF(fputc(']', data->ostream));
	return NBTS_OK;
}

enum nbts_error nbts_print_handle_compound(
	struct nbts_print_handler_data *restrict nonnull data,
	size_t name_size,
	FILE *restrict nonnull stream)
{
	PREAMBLE();

	TRYF(fputc('{', data->ostream));

	data->index = 0;
	TRY(nbts_parse_compound(stream, &nbts_print_handler, data));
	data->index = index + 1;

	TRYF(fputc('}', data->ostream));
	return NBTS_OK;
}
