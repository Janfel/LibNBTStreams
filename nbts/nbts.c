#include <nbts/nbts.h>

#include <endian.h>

#include <stdio.h>
#include <string.h>

#define NBTS_BYTE_ORDER BIG_ENDIAN

#if NBTS_BYTE_ORDER == BIG_ENDIAN
#define nbt16toh(...) be16toh(__VA_ARGS__)
#define nbt32toh(...) be32toh(__VA_ARGS__)
#define nbt64toh(...) be64toh(__VA_ARGS__)
#elif NBTS_BYTE_ORDER == LITTLE_ENDIAN
#define nbt16toh(...) le16toh(__VA_ARGS__)
#define nbt32toh(...) le32toh(__VA_ARGS__)
#define nbt64toh(...) le64toh(__VA_ARGS__)
#else
#error "Byte order not supported"
#endif

#if defined(__FLOAT_WORD_ORDER) && __FLOAT_WORD_ORDER != BYTE_ORDER
#error "Float word order must be the same as byte order"
#endif

#if __clang__
#define nonnull  _Nonnull
#define nullable _Nullable
#else
#define nonnull
#define nullable
#endif

#define TRY(EXPR, ...)                 \
	{                                  \
		enum nbts_error _err = (EXPR); \
		__VA_ARGS__;                   \
		if (_err) return _err;         \
	}

#define CATCH(ERR, ...)  \
	if (_err == (ERR)) { \
		__VA_ARGS__;     \
	}

// NOLINTBEGIN(bugprone-easily-swappable-parameters)

static enum nbts_error
xfread(void *restrict nonnull ptr, size_t size, size_t count, FILE *restrict nonnull stream)
{
	if (fread(ptr, size, count, stream) != count) {
		if (ferror(stream)) return NBTS_READ_ERR;
		if (feof(stream)) return NBTS_UNEXPECTED_EOF;
	}
	return NBTS_OK;
}

enum nbts_error nbts_parse_uint8(uint8_t *restrict nonnull dest, FILE *restrict nonnull stream)
{
	return xfread(dest, sizeof(*dest), 1, stream);
}

enum nbts_error nbts_parse_uint16(uint16_t *restrict nonnull dest, FILE *restrict nonnull stream)
{
	TRY(xfread(dest, sizeof(*dest), 1, stream));
	*dest = nbt16toh(*dest);
	return NBTS_OK;
}

enum nbts_error nbts_parse_uint32(uint32_t *restrict nonnull dest, FILE *restrict nonnull stream)
{
	TRY(xfread(dest, sizeof(*dest), 1, stream));
	*dest = nbt32toh(*dest);
	return NBTS_OK;
}

enum nbts_error nbts_parse_uint64(uint64_t *restrict nonnull dest, FILE *restrict nonnull stream)
{
	TRY(xfread(dest, sizeof(*dest), 1, stream));
	*dest = nbt64toh(*dest);
	return NBTS_OK;
}

enum nbts_error
nbts_parse_typeid(enum nbts_type *restrict nonnull dest, FILE *restrict nonnull stream)
{
	uint8_t result = 0;
	TRY(nbts_parse_uint8(&result, stream));
	if (!(result < NBTS_TYPE_ENUM_SIZE)) return NBTS_INVALID_ID;
	static_assert(sizeof(*dest) == sizeof(result));
	memcpy(dest, &result, sizeof(result));
	return NBTS_OK;
}

enum nbts_error nbts_parse_size(nbts_size *restrict nonnull dest, FILE *restrict nonnull stream)
{
	nbts_int result = 0;
	TRY(nbts_parse_int(&result, stream));
	if (result < 0) return NBTS_INVALID_SIZE;
	static_assert(sizeof(*dest) == sizeof(result));
	memcpy(dest, &result, sizeof(result));
	return NBTS_OK;
}

enum nbts_error
nbts_parse_strsize(nbts_strsize *restrict nonnull dest, FILE *restrict nonnull stream)
{
	return nbts_parse_uint16(dest, stream);
}

enum nbts_error nbts_parse_byte(nbts_byte *restrict nonnull dest, FILE *restrict nonnull stream)
{
	uint8_t result = 0;
	TRY(nbts_parse_uint8(&result, stream));
	static_assert(sizeof(*dest) == sizeof(result));
	memcpy(dest, &result, sizeof(result));
	return NBTS_OK;
}

enum nbts_error nbts_parse_short(nbts_short *restrict nonnull dest, FILE *restrict nonnull stream)
{
	uint16_t result = 0;
	TRY(nbts_parse_uint16(&result, stream));
	static_assert(sizeof(*dest) == sizeof(result));
	memcpy(dest, &result, sizeof(result));
	return NBTS_OK;
}

enum nbts_error nbts_parse_int(nbts_int *restrict nonnull dest, FILE *restrict nonnull stream)
{
	uint32_t result = 0;
	TRY(nbts_parse_uint32(&result, stream));
	static_assert(sizeof(*dest) == sizeof(result));
	memcpy(dest, &result, sizeof(result));
	return NBTS_OK;
}

enum nbts_error nbts_parse_long(nbts_long *restrict nonnull dest, FILE *restrict nonnull stream)
{
	uint64_t result = 0;
	TRY(nbts_parse_uint64(&result, stream));
	static_assert(sizeof(*dest) == sizeof(result));
	memcpy(dest, &result, sizeof(result));
	return NBTS_OK;
}

enum nbts_error nbts_parse_float(nbts_float *restrict nonnull dest, FILE *restrict nonnull stream)
{
	uint32_t result = 0;
	TRY(nbts_parse_uint32(&result, stream));
	static_assert(sizeof(*dest) == sizeof(result));
	memcpy(dest, &result, sizeof(result));
	return NBTS_OK;
}

enum nbts_error nbts_parse_double(nbts_double *restrict nonnull dest, FILE *restrict nonnull stream)
{
	uint64_t result = 0;
	TRY(nbts_parse_uint64(&result, stream));
	static_assert(sizeof(*dest) == sizeof(result));
	memcpy(dest, &result, sizeof(result));
	return NBTS_OK;
}

enum nbts_error
nbts_parse_string(nbts_char *restrict nonnull dest, size_t size, FILE *restrict nonnull stream)
{
	return xfread(dest, sizeof(*dest), size, stream);
}

enum nbts_error
nbts_parse_byte_array(nbts_byte *restrict nonnull dest, size_t size, FILE *restrict nonnull stream)
{
	return xfread(dest, sizeof(*dest), size, stream);
}

enum nbts_error
nbts_parse_int_array(nbts_int *restrict nonnull dest, size_t size, FILE *restrict nonnull stream)
{
	TRY(xfread(dest, sizeof(*dest), size, stream));
	for (size_t i = 0; i < size; ++i) dest[i] = nbt32toh(dest[i]);
	return NBTS_OK;
}

enum nbts_error
nbts_parse_long_array(nbts_long *restrict nonnull dest, size_t size, FILE *restrict nonnull stream)
{
	TRY(xfread(dest, sizeof(*dest), size, stream));
	for (size_t i = 0; i < size; ++i) dest[i] = nbt64toh(dest[i]);
	return NBTS_OK;
}

enum nbts_error nbts_parse_list(
	enum nbts_type type,
	size_t size,
	FILE *restrict nonnull stream,
	struct nbts_handler const *restrict handler,
	void *restrict userdata)
{
	nbts_handler_fn *handler_fn = handler ? handler->handle[type] : nbts_skip_handler.handle[type];

	for (size_t i = 0; i < size; ++i) TRY(handler_fn(userdata, 0, stream));

	return NBTS_OK;
}

enum nbts_error nbts_parse_compound(
	FILE *restrict nonnull stream,
	struct nbts_handler const *restrict handler,
	void *restrict userdata)
{
	while (1) TRY(nbts_parse_tag(stream, handler, userdata), CATCH(NBTS_UNEXPECTED_END_TAG, break));
	return NBTS_OK;
}

enum nbts_error nbts_parse_tag(
	FILE *restrict nonnull stream,
	struct nbts_handler const *restrict handler,
	void *restrict userdata)
{
	enum nbts_type type = 0;
	TRY(nbts_parse_typeid(&type, stream));

	if (type == NBTS_END) return NBTS_UNEXPECTED_END_TAG;

	nbts_handler_fn *handler_fn = handler ? handler->handle[type] : nbts_skip_handler.handle[type];

	nbts_strsize name_size = 0;
	TRY(nbts_parse_strsize(&name_size, stream));
	TRY(handler_fn(userdata, name_size, stream));
	return NBTS_OK;
}

enum nbts_error nbts_parse_network_tag(
	FILE *restrict nonnull stream,
	struct nbts_handler const *restrict handler,
	void *restrict userdata)
{
	enum nbts_type type = 0;
	TRY(nbts_parse_typeid(&type, stream));

	if (type == NBTS_END) return NBTS_UNEXPECTED_END_TAG;

	nbts_handler_fn *handler_fn = handler ? handler->handle[type] : nbts_skip_handler.handle[type];

	TRY(handler_fn(userdata, 0, stream));
	return NBTS_OK;
}

struct nbts_handler const nbts_skip_handler = {
	.handle[NBTS_END] = &nbts_skip_end,
	.handle[NBTS_BYTE] = &nbts_skip_byte,
	.handle[NBTS_SHORT] = &nbts_skip_short,
	.handle[NBTS_INT] = &nbts_skip_int,
	.handle[NBTS_LONG] = &nbts_skip_long,
	.handle[NBTS_FLOAT] = &nbts_skip_float,
	.handle[NBTS_DOUBLE] = &nbts_skip_double,
	.handle[NBTS_STRING] = &nbts_skip_string,
	.handle[NBTS_BYTE_ARRAY] = &nbts_skip_byte_array,
	.handle[NBTS_INT_ARRAY] = &nbts_skip_int_array,
	.handle[NBTS_LONG_ARRAY] = &nbts_skip_long_array,
	.handle[NBTS_LIST] = &nbts_skip_list,
	.handle[NBTS_COMPOUND] = &nbts_skip_compound,
};

static inline enum nbts_error skip_name(nbts_strsize name_size, FILE *restrict nonnull stream)
{
	if (name_size && fseek(stream, name_size * (long) sizeof(nbts_char), SEEK_CUR) == -1)
		return NBTS_READ_ERR;
	return NBTS_OK;
}

static inline enum nbts_error
skip_name_and_data(nbts_strsize name_size, FILE *restrict nonnull stream, long data_size)
{
	if (fseek(stream, name_size * (long) sizeof(nbts_char) + data_size, SEEK_CUR) == -1)
		return NBTS_READ_ERR;
	return NBTS_OK;
}

enum nbts_error nbts_skip_end(void * /**/, nbts_strsize name_size, FILE *restrict nonnull stream)
{
	TRY(skip_name(name_size, stream));
	return NBTS_OK;
}

enum nbts_error nbts_skip_byte(void * /**/, nbts_strsize name_size, FILE *restrict nonnull stream)
{
	TRY(skip_name_and_data(name_size, stream, sizeof(nbts_byte)));
	return NBTS_OK;
}

enum nbts_error nbts_skip_short(void * /**/, nbts_strsize name_size, FILE *restrict nonnull stream)
{
	TRY(skip_name_and_data(name_size, stream, sizeof(nbts_short)));
	return NBTS_OK;
}

enum nbts_error nbts_skip_int(void * /**/, nbts_strsize name_size, FILE *restrict nonnull stream)
{
	TRY(skip_name_and_data(name_size, stream, sizeof(nbts_int)));
	return NBTS_OK;
}

enum nbts_error nbts_skip_long(void * /**/, nbts_strsize name_size, FILE *restrict nonnull stream)
{
	TRY(skip_name_and_data(name_size, stream, sizeof(nbts_long)));
	return NBTS_OK;
}

enum nbts_error nbts_skip_float(void * /**/, nbts_strsize name_size, FILE *restrict nonnull stream)
{
	TRY(skip_name_and_data(name_size, stream, sizeof(nbts_float)));
	return NBTS_OK;
}

enum nbts_error nbts_skip_double(void * /**/, nbts_strsize name_size, FILE *restrict nonnull stream)
{
	TRY(skip_name_and_data(name_size, stream, sizeof(nbts_double)));
	return NBTS_OK;
}

enum nbts_error nbts_skip_string(void * /**/, nbts_strsize name_size, FILE *restrict nonnull stream)
{
	TRY(skip_name(name_size, stream));

	nbts_strsize size = 0;
	TRY(nbts_parse_strsize(&size, stream));
	if (fseek(stream, (long) (size * sizeof(nbts_char)), SEEK_CUR) == -1) return NBTS_READ_ERR;
	return NBTS_OK;
}

enum nbts_error
nbts_skip_byte_array(void * /**/, nbts_strsize name_size, FILE *restrict nonnull stream)
{
	TRY(skip_name(name_size, stream));

	nbts_size size = 0;
	TRY(nbts_parse_size(&size, stream));
	if (fseek(stream, (long) (size * sizeof(nbts_byte)), SEEK_CUR) == -1) return NBTS_READ_ERR;
	return NBTS_OK;
}

enum nbts_error
nbts_skip_int_array(void * /**/, nbts_strsize name_size, FILE *restrict nonnull stream)
{
	TRY(skip_name(name_size, stream));

	nbts_size size = 0;
	TRY(nbts_parse_int(&size, stream));
	if (fseek(stream, (long) (size * sizeof(nbts_int)), SEEK_CUR) == -1) return NBTS_READ_ERR;
	return NBTS_OK;
}

enum nbts_error
nbts_skip_long_array(void * /**/, nbts_strsize name_size, FILE *restrict nonnull stream)
{
	TRY(skip_name(name_size, stream));

	nbts_size size = 0;
	TRY(nbts_parse_size(&size, stream));
	if (fseek(stream, (long) (size * sizeof(nbts_long)), SEEK_CUR) == -1) return NBTS_READ_ERR;
	return NBTS_OK;
}

enum nbts_error nbts_skip_list(void * /**/, nbts_strsize name_size, FILE *restrict nonnull stream)
{
	TRY(skip_name(name_size, stream));

	enum nbts_type type = 0;
	TRY(nbts_parse_typeid(&type, stream));

	nbts_size size = 0;
	TRY(nbts_parse_size(&size, stream));

	nbts_handler_fn *skip_fn = nbts_skip_handler.handle[type];
	for (nbts_size i = 0; i < size; ++i) {
		TRY(skip_fn(nullptr, 0, stream));
	}

	return NBTS_OK;
}

enum nbts_error
nbts_skip_compound(void * /**/, nbts_strsize name_size, FILE *restrict nonnull stream)
{
	TRY(skip_name(name_size, stream));
	while (1) TRY(nbts_parse_tag(stream, nullptr, nullptr), CATCH(NBTS_UNEXPECTED_END_TAG, break));
	return NBTS_OK;
}

// NOLINTEND(bugprone-easily-swappable-parameters)
