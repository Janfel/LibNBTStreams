#include <nbts/nbts.h>

#include <endian.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(__FLOAT_WORD_ORDER) && __FLOAT_WORD_ORDER != BYTE_ORDER
#error "Float word order must be the same as byte order"
#endif

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
	nbts_handler_fn *handler_fn = nbts_get_handler_fn(type, handler);
	if (handler_fn) {
		for (size_t i = 0; i < size; ++i) TRY(handler_fn(userdata, 0, stream));
	} else {
		nbts_skip_fn *skip_fn = nbts_get_skip_fn(type);
		for (size_t i = 0; i < size; ++i) TRY(skip_fn(stream));
	}
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

// NOLINTNEXTLINE(misc-no-recursion)
enum nbts_error nbts_parse_tag(
	FILE *restrict nonnull stream,
	struct nbts_handler const *restrict handler,
	void *restrict userdata)
{
	enum nbts_type type = 0;
	TRY(nbts_parse_typeid(&type, stream));

	if (type == NBTS_END) return NBTS_UNEXPECTED_END_TAG;

	nbts_handler_fn *handler_fn = nbts_get_handler_fn(type, handler);
	if (!handler_fn) {
		nbts_skip_fn *skip_fn = nbts_get_skip_fn(type);
		TRY(nbts_skip_string(stream));
		TRY(skip_fn(stream));
		return NBTS_OK;
	}

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

	nbts_handler_fn *handler_fn = nbts_get_handler_fn(type, handler);
	if (!handler_fn) {
		nbts_skip_fn *skip_fn = nbts_get_skip_fn(type);
		// TRY(nbts_skip_string(stream));
		TRY(skip_fn(stream));
		return NBTS_OK;
	}

	nbts_strsize name_size = 0;
	// TRY(nbts_parse_strsize(&name_size, stream));
	TRY(handler_fn(userdata, name_size, stream));
	return NBTS_OK;
}

enum nbts_error nbts_skip_end([[maybe_unused]] FILE *restrict nonnull stream) { return NBTS_OK; }

enum nbts_error nbts_skip_byte(FILE *restrict nonnull stream)
{
	if (fseek(stream, sizeof(nbts_byte), SEEK_CUR) == -1) return NBTS_READ_ERR;
	return NBTS_OK;
}

enum nbts_error nbts_skip_short(FILE *restrict nonnull stream)
{
	if (fseek(stream, sizeof(nbts_short), SEEK_CUR) == -1) return NBTS_READ_ERR;
	return NBTS_OK;
}

enum nbts_error nbts_skip_int(FILE *restrict nonnull stream)
{
	if (fseek(stream, sizeof(nbts_int), SEEK_CUR) == -1) return NBTS_READ_ERR;
	return NBTS_OK;
}

enum nbts_error nbts_skip_long(FILE *restrict nonnull stream)
{
	if (fseek(stream, sizeof(nbts_long), SEEK_CUR) == -1) return NBTS_READ_ERR;
	return NBTS_OK;
}

enum nbts_error nbts_skip_float(FILE *restrict nonnull stream)
{
	if (fseek(stream, sizeof(nbts_float), SEEK_CUR) == -1) return NBTS_READ_ERR;
	return NBTS_OK;
}

enum nbts_error nbts_skip_double(FILE *restrict nonnull stream)
{
	if (fseek(stream, sizeof(nbts_double), SEEK_CUR) == -1) return NBTS_READ_ERR;
	return NBTS_OK;
}

enum nbts_error nbts_skip_string(FILE *restrict nonnull stream)
{
	nbts_strsize size = 0;
	TRY(nbts_parse_strsize(&size, stream));
	if (fseek(stream, (long) (size * sizeof(nbts_char)), SEEK_CUR) == -1) return NBTS_READ_ERR;
	return NBTS_OK;
}

enum nbts_error nbts_skip_byte_array(FILE *restrict nonnull stream)
{
	nbts_size size = 0;
	TRY(nbts_parse_size(&size, stream));
	if (fseek(stream, (long) (size * sizeof(nbts_byte)), SEEK_CUR) == -1) return NBTS_READ_ERR;
	return NBTS_OK;
}

enum nbts_error nbts_skip_int_array(FILE *restrict nonnull stream)
{
	nbts_size size = 0;
	TRY(nbts_parse_int(&size, stream));
	if (fseek(stream, (long) (size * sizeof(nbts_int)), SEEK_CUR) == -1) return NBTS_READ_ERR;
	return NBTS_OK;
}

enum nbts_error nbts_skip_long_array(FILE *restrict nonnull stream)
{
	nbts_size size = 0;
	TRY(nbts_parse_size(&size, stream));
	if (fseek(stream, (long) (size * sizeof(nbts_long)), SEEK_CUR) == -1) return NBTS_READ_ERR;
	return NBTS_OK;
}

enum nbts_error nbts_skip_list(FILE *restrict nonnull stream)
{
	enum nbts_type type = 0;
	TRY(nbts_parse_typeid(&type, stream));

	nbts_size size = 0;
	TRY(nbts_parse_size(&size, stream));

	nbts_skip_fn *skip_fn = nbts_get_skip_fn(type);
	for (nbts_size i = 0; i < size; ++i) {
		TRY(skip_fn(stream));
	}

	return NBTS_OK;
}

// NOLINTNEXTLINE(misc-no-recursion)
enum nbts_error nbts_skip_compound(FILE *restrict nonnull stream)
{
	while (1) TRY(nbts_parse_tag(stream, nullptr, nullptr), CATCH(NBTS_UNEXPECTED_END_TAG, break));
	return NBTS_OK;
}

nbts_handler_fn *
nbts_get_handler_fn(enum nbts_type type, struct nbts_handler const *restrict nonnull handler)
{
	switch (type) {
	case NBTS_END: return nullptr;
	case NBTS_BYTE: return handler->handle_byte;
	case NBTS_SHORT: return handler->handle_short;
	case NBTS_INT: return handler->handle_int;
	case NBTS_LONG: return handler->handle_long;
	case NBTS_FLOAT: return handler->handle_float;
	case NBTS_DOUBLE: return handler->handle_double;
	case NBTS_STRING: return handler->handle_string;
	case NBTS_BYTE_ARRAY: return handler->handle_byte_array;
	case NBTS_INT_ARRAY: return handler->handle_int_array;
	case NBTS_LONG_ARRAY: return handler->handle_long_array;
	case NBTS_LIST: return handler->handle_list;
	case NBTS_COMPOUND: return handler->handle_compound;
	}
	return nullptr;
}

nbts_skip_fn *nbts_get_skip_fn(enum nbts_type type)
{
	switch (type) {
	case NBTS_END: return &nbts_skip_end;
	case NBTS_BYTE: return &nbts_skip_byte;
	case NBTS_SHORT: return &nbts_skip_short;
	case NBTS_INT: return &nbts_skip_int;
	case NBTS_LONG: return &nbts_skip_long;
	case NBTS_FLOAT: return &nbts_skip_float;
	case NBTS_DOUBLE: return &nbts_skip_double;
	case NBTS_STRING: return &nbts_skip_string;
	case NBTS_BYTE_ARRAY: return &nbts_skip_byte_array;
	case NBTS_INT_ARRAY: return &nbts_skip_int_array;
	case NBTS_LONG_ARRAY: return &nbts_skip_long_array;
	case NBTS_LIST: return &nbts_skip_list;
	case NBTS_COMPOUND: return &nbts_skip_compound;
	}
	return nullptr;
}

// NOLINTEND(bugprone-easily-swappable-parameters)
