#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <uchar.h>

#if __clang__
#define nonnull  _Nonnull
#define nullable _Nullable
#else
#define nonnull
#define nullable
#endif

enum : size_t { NBTS_STACK_BUFFER_SIZE = 2048 };

enum nbts_type : uint8_t {
	NBTS_END = 0,
	NBTS_BYTE = 1,
	NBTS_SHORT = 2,
	NBTS_INT = 3,
	NBTS_LONG = 4,
	NBTS_FLOAT = 5,
	NBTS_DOUBLE = 6,
	NBTS_STRING = 8,
	NBTS_BYTE_ARRAY = 7,
	NBTS_INT_ARRAY = 11,
	NBTS_LONG_ARRAY = 12,
	NBTS_LIST = 9,
	NBTS_COMPOUND = 10,
};

enum : uint8_t { NBTS_TYPE_ENUM_SIZE = 13 };

enum [[nodiscard]] nbts_error {
	NBTS_OK,
	NBTS_READ_ERR,
	NBTS_WRITE_ERR,
	NBTS_UNEXPECTED_EOF,
	NBTS_UNEXPECTED_END_TAG,
	NBTS_INVALID_ID,
	NBTS_INVALID_SIZE,
};

typedef uint8_t nbts_char;
typedef int8_t nbts_byte;
typedef int16_t nbts_short;
typedef int32_t nbts_int;
typedef int64_t nbts_long;
typedef float nbts_float;
typedef double nbts_double;
typedef int32_t nbts_size;
typedef uint16_t nbts_strsize;

typedef enum nbts_error
nbts_handler_fn(void *userdata, size_t name_size, FILE *restrict nonnull stream);

typedef enum nbts_error nbts_skip_fn(FILE *restrict nonnull stream);

struct nbts_handler {
	nbts_handler_fn *handle_byte;
	nbts_handler_fn *handle_short;
	nbts_handler_fn *handle_int;
	nbts_handler_fn *handle_long;
	nbts_handler_fn *handle_float;
	nbts_handler_fn *handle_double;
	nbts_handler_fn *handle_string;
	nbts_handler_fn *handle_byte_array;
	nbts_handler_fn *handle_int_array;
	nbts_handler_fn *handle_long_array;
	nbts_handler_fn *handle_list;
	nbts_handler_fn *handle_compound;
};

enum nbts_error nbts_parse_uint8(uint8_t *restrict nonnull dest, FILE *restrict nonnull stream);
enum nbts_error nbts_parse_uint16(uint16_t *restrict nonnull dest, FILE *restrict nonnull stream);
enum nbts_error nbts_parse_uint32(uint32_t *restrict nonnull dest, FILE *restrict nonnull stream);
enum nbts_error nbts_parse_uint64(uint64_t *restrict nonnull dest, FILE *restrict nonnull stream);

enum nbts_error
nbts_parse_typeid(enum nbts_type *restrict nonnull dest, FILE *restrict nonnull stream);
enum nbts_error nbts_parse_size(nbts_size *restrict nonnull dest, FILE *restrict nonnull stream);
enum nbts_error
nbts_parse_strsize(nbts_strsize *restrict nonnull dest, FILE *restrict nonnull stream);

enum nbts_error nbts_parse_byte(nbts_byte *restrict nonnull dest, FILE *restrict nonnull stream);
enum nbts_error nbts_parse_short(nbts_short *restrict nonnull dest, FILE *restrict nonnull stream);
enum nbts_error nbts_parse_int(nbts_int *restrict nonnull dest, FILE *restrict nonnull stream);
enum nbts_error nbts_parse_long(nbts_long *restrict nonnull dest, FILE *restrict nonnull stream);
enum nbts_error nbts_parse_float(nbts_float *restrict nonnull dest, FILE *restrict nonnull stream);
enum nbts_error
nbts_parse_double(nbts_double *restrict nonnull dest, FILE *restrict nonnull stream);
enum nbts_error
nbts_parse_string(nbts_char *restrict nonnull dest, size_t size, FILE *restrict nonnull stream);
enum nbts_error
nbts_parse_byte_array(nbts_byte *restrict nonnull dest, size_t size, FILE *restrict nonnull stream);
enum nbts_error
nbts_parse_int_array(nbts_int *restrict nonnull dest, size_t size, FILE *restrict nonnull stream);
enum nbts_error
nbts_parse_long_array(nbts_long *restrict nonnull dest, size_t size, FILE *restrict nonnull stream);

enum nbts_error nbts_parse_list(
	enum nbts_type type,
	size_t size,
	FILE *restrict nonnull stream,
	struct nbts_handler const *restrict handler,
	void *restrict userdata);

enum nbts_error nbts_parse_compound(
	FILE *restrict nonnull stream,
	struct nbts_handler const *restrict handler,
	void *restrict userdata);

enum nbts_error nbts_parse_tag(
	FILE *restrict nonnull stream,
	struct nbts_handler const *restrict handler,
	void *restrict userdata);

enum nbts_error nbts_parse_network_tag(
	FILE *restrict nonnull stream,
	struct nbts_handler const *restrict handler,
	void *restrict userdata);

enum nbts_error nbts_skip_byte(FILE *restrict nonnull stream);
enum nbts_error nbts_skip_short(FILE *restrict nonnull stream);
enum nbts_error nbts_skip_int(FILE *restrict nonnull stream);
enum nbts_error nbts_skip_long(FILE *restrict nonnull stream);
enum nbts_error nbts_skip_float(FILE *restrict nonnull stream);
enum nbts_error nbts_skip_double(FILE *restrict nonnull stream);
enum nbts_error nbts_skip_string(FILE *restrict nonnull stream);
enum nbts_error nbts_skip_byte_array(FILE *restrict nonnull stream);
enum nbts_error nbts_skip_int_array(FILE *restrict nonnull stream);
enum nbts_error nbts_skip_long_array(FILE *restrict nonnull stream);
enum nbts_error nbts_skip_list(FILE *restrict nonnull stream);
enum nbts_error nbts_skip_compound(FILE *restrict nonnull stream);

nbts_handler_fn *
nbts_get_handler_fn(enum nbts_type type, struct nbts_handler const *restrict nonnull handler);
nbts_skip_fn *nbts_get_skip_fn(enum nbts_type type);

#undef nonnull
#undef nullable
