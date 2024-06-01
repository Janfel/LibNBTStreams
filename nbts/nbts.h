#pragma once

#include <stdint.h>
#include <stdio.h>

#if __clang__
#define nonnull  _Nonnull
#define nullable _Nullable
#else
#define nonnull
#define nullable
#endif

/// The size of buffer that modules will allocate on the stack.
enum : size_t { NBTS_STACK_BUFFER_SIZE = 2048 };

/// The NBT tag type.
///
/// This enum matches the ID byte of an NBT tag in both size and meaning, so
/// they can be considered the same under `sizeof()` and `memcpy()`.
enum nbts_type : uint8_t {
	NBTS_END = 0,          ///< Marks the end of a compound tag, **no name field**.
	NBTS_BYTE = 1,         ///< A signed 8 bit integer.
	NBTS_SHORT = 2,        ///< A signed 16 bit integer.
	NBTS_INT = 3,          ///< A signed 32 bit integer.
	NBTS_LONG = 4,         ///< A signed 64 bit integer.
	NBTS_FLOAT = 5,        ///< An IEEE754 binary32 float.
	NBTS_DOUBLE = 6,       ///< An IEEE754 binary64 float.
	NBTS_STRING = 8,       ///< A \ref nbts_strsize prefixed Modified UTF-8 string.
	NBTS_BYTE_ARRAY = 7,   ///< A \ref nbts_size prefixed array of \ref nbts_byte.
	NBTS_INT_ARRAY = 11,   ///< A \ref nbts_size prefixed array of \ref nbts_int.
	NBTS_LONG_ARRAY = 12,  ///< A \ref nbts_size prefixed array of \ref nbts_long.
	NBTS_LIST = 9,         ///< A \ref nbts_type and \ref nbts_size followed by that many payloads.
	NBTS_COMPOUND = 10,    ///< Marks the beginning of a compound tag.
};

/// The size of \ref nbts_type.
enum : uint8_t { NBTS_TYPE_ENUM_SIZE = 13 };

/// Error codes returned by this library.
///
/// Any value below NBTS_CUSTOM_ERR is reserved. Users of this library may use
/// any value of NBTS_CUSTOM_ERR or greater for their own purposes.
enum [[nodiscard]] nbts_error {
	NBTS_OK,                  ///< No error.
	NBTS_READ_ERR,            ///< Error reading from the input stream.
	NBTS_WRITE_ERR,           ///< Error writing to the output stream.
	NBTS_UNEXPECTED_EOF,      ///< Reached EOF while parsing.
	NBTS_UNEXPECTED_END_TAG,  ///< The parsed tag was an END tag.
	NBTS_INVALID_ID,          ///< The value of the ID byte was out of range.
	NBTS_INVALID_SIZE,        ///< The size of a list or array was negative.
	NBTS_CUSTOM_ERR = 1000,   ///< The first value reserved for application-specific errors.
};

/// NBT Modified UTF-8 character.
typedef uint8_t nbts_char;
/// NBT Byte data type (ID \ref NBTS_BYTE).
typedef int8_t nbts_byte;
/// NBT Short data type (ID \ref NBTS_SHORT).
typedef int16_t nbts_short;
/// NBT Int data type (ID \ref NBTS_INT).
typedef int32_t nbts_int;
/// NBT Long data type (ID \ref NBTS_LONG).
typedef int64_t nbts_long;
/// NBT Float data type (ID \ref NBTS_FLOAT).
typedef float nbts_float;
/// NBT Double data type (ID \ref NBTS_DOUBLE).
typedef double nbts_double;
/// NBT size type used for lists and arrays.
typedef int32_t nbts_size;
/// NBT size type used for strings.
typedef uint16_t nbts_strsize;

/// The type of an NBT handler callback.
///
/// Each callback shall be valid for one specific value of \ref nbts_type. The
/// callback will be called with the user-provided value for `userdata`. It
/// shall first read `name_size` bytes from `stream`, forming the name of the
/// tag. It shall then parse one payload of the type for which it is valid and
/// leave the `stream` after the last byte of the payload.
typedef enum nbts_error
nbts_handler_fn(void *nullable userdata, nbts_strsize name_size, FILE *restrict nonnull stream);

/// Structure holding \ref nbts_handler_fn instances for all values of \ref nbts_type.
struct nbts_handler {
	/// Array mapping \ref nbts_type values to \ref nbts_handler_fn pointers.
	///
	/// If the resulting pointer is `nullptr`, the payload is skipped as if by
	/// \ref nbts_skip_handler.
	nbts_handler_fn *nullable handle[NBTS_TYPE_ENUM_SIZE];
};

/// Reads one `uint8_t` from `stream` into `dest`.
enum nbts_error nbts_parse_uint8(uint8_t *restrict nonnull dest, FILE *restrict nonnull stream);
/// Reads one `uint16_t` from `stream` into `dest`, handling endian conversion.
enum nbts_error nbts_parse_uint16(uint16_t *restrict nonnull dest, FILE *restrict nonnull stream);
/// Reads one `uint32_t` from `stream` into `dest`, handling endian conversion.
enum nbts_error nbts_parse_uint32(uint32_t *restrict nonnull dest, FILE *restrict nonnull stream);
/// Reads one `uint64_t` from `stream` into `dest`, handling endian conversion.
enum nbts_error nbts_parse_uint64(uint64_t *restrict nonnull dest, FILE *restrict nonnull stream);

/// Reads one \ref nbts_type from `stream` into `dest`.
///
/// Returns \ref NBTS_INVALID_ID if the value is out of range.
enum nbts_error
nbts_parse_typeid(enum nbts_type *restrict nonnull dest, FILE *restrict nonnull stream);

/// Reads one \ref nbts_size from `stream` into `dest`.
///
/// Returns \ref NBTS_INVALID_SIZE if the value is negative.
enum nbts_error nbts_parse_size(nbts_size *restrict nonnull dest, FILE *restrict nonnull stream);

/// Reads one \ref nbts_strsize from `stream` into `dest`.
enum nbts_error
nbts_parse_strsize(nbts_strsize *restrict nonnull dest, FILE *restrict nonnull stream);

/// Reads one \ref nbts_byte from `stream` into `dest`.
enum nbts_error nbts_parse_byte(nbts_byte *restrict nonnull dest, FILE *restrict nonnull stream);
/// Reads one \ref nbts_short from `stream` into `dest`.
enum nbts_error nbts_parse_short(nbts_short *restrict nonnull dest, FILE *restrict nonnull stream);
/// Reads one \ref nbts_int from `stream` into `dest`.
enum nbts_error nbts_parse_int(nbts_int *restrict nonnull dest, FILE *restrict nonnull stream);
/// Reads one \ref nbts_long from `stream` into `dest`.
enum nbts_error nbts_parse_long(nbts_long *restrict nonnull dest, FILE *restrict nonnull stream);
/// Reads one \ref nbts_float from `stream` into `dest`.
enum nbts_error nbts_parse_float(nbts_float *restrict nonnull dest, FILE *restrict nonnull stream);
/// Reads one \ref nbts_double from `stream` into `dest`.
enum nbts_error
nbts_parse_double(nbts_double *restrict nonnull dest, FILE *restrict nonnull stream);

/// Reads a string of `size` \ref nbts_char from `stream` into `dest`.
enum nbts_error
nbts_parse_string(nbts_char *restrict nonnull dest, size_t size, FILE *restrict nonnull stream);

/// Reads an array of `size` \ref nbts_byte from `stream` into `dest`.
enum nbts_error
nbts_parse_byte_array(nbts_byte *restrict nonnull dest, size_t size, FILE *restrict nonnull stream);

/// Reads an array of `size` \ref nbts_int from `stream` into `dest`.
enum nbts_error
nbts_parse_int_array(nbts_int *restrict nonnull dest, size_t size, FILE *restrict nonnull stream);

/// Reads an array of `size` \ref nbts_long from `stream` into `dest`.
enum nbts_error
nbts_parse_long_array(nbts_long *restrict nonnull dest, size_t size, FILE *restrict nonnull stream);

/// Parses `size` payloads of `type` from `stream`.
///
/// For each payload, `handler` is called with `userdata`. If `handler` is
/// `nullptr`, or any individual \ref nbts_handler_fn is `nullptr`, the payload
/// is skipped as if by \ref nbts_skip_handler.
enum nbts_error nbts_parse_list(
	enum nbts_type type,
	size_t size,
	FILE *restrict nonnull stream,
	struct nbts_handler const *restrict nullable handler,
	void *restrict nullable userdata);

/// Parses named tags until the next \ref NBTS_END tag inclusive.
///
/// For each tag, `handler` is called with `userdata`. If `handler` is
/// `nullptr`, or any individual \ref nbts_handler_fn is `nullptr`, the payload
/// is skipped as if by \ref nbts_skip_handler.
enum nbts_error nbts_parse_compound(
	FILE *restrict nonnull stream,
	struct nbts_handler const *restrict nullable handler,
	void *restrict nullable userdata);

/// Parses one NBT tag, calling `handler` with `userdata`.
///
/// If `handler` is `nullptr`, or any individual \ref nbts_handler_fn is
/// `nullptr`, the payload is skipped as if by \ref nbts_skip_handler.
enum nbts_error nbts_parse_tag(
	FILE *restrict nonnull stream,
	struct nbts_handler const *restrict nullable handler,
	void *restrict nullable userdata);

/// Parses one **unnamed** NBT tag, calling `handler` with `userdata`.
///
/// This corresponds to the Network NBT format introduced in Protocol 764.
///
/// If `handler` is `nullptr`, or any individual \ref nbts_handler_fn is
/// `nullptr`, the payload is skipped as if by \ref nbts_skip_handler.
enum nbts_error nbts_parse_network_tag(
	FILE *restrict nonnull stream,
	struct nbts_handler const *restrict nullable handler,
	void *restrict nullable userdata);

extern struct nbts_handler const nbts_skip_handler;

enum nbts_error
nbts_skip_end(void *nullable, nbts_strsize name_size, FILE *restrict nonnull stream);
enum nbts_error
nbts_skip_byte(void *nullable, nbts_strsize name_size, FILE *restrict nonnull stream);
enum nbts_error
nbts_skip_short(void *nullable, nbts_strsize name_size, FILE *restrict nonnull stream);
enum nbts_error
nbts_skip_int(void *nullable, nbts_strsize name_size, FILE *restrict nonnull stream);
enum nbts_error
nbts_skip_long(void *nullable, nbts_strsize name_size, FILE *restrict nonnull stream);
enum nbts_error
nbts_skip_float(void *nullable, nbts_strsize name_size, FILE *restrict nonnull stream);
enum nbts_error
nbts_skip_double(void *nullable, nbts_strsize name_size, FILE *restrict nonnull stream);
enum nbts_error
nbts_skip_string(void *nullable, nbts_strsize name_size, FILE *restrict nonnull stream);
enum nbts_error
nbts_skip_byte_array(void *nullable, nbts_strsize name_size, FILE *restrict nonnull stream);
enum nbts_error
nbts_skip_int_array(void *nullable, nbts_strsize name_size, FILE *restrict nonnull stream);
enum nbts_error
nbts_skip_long_array(void *nullable, nbts_strsize name_size, FILE *restrict nonnull stream);
enum nbts_error
nbts_skip_list(void *nullable, nbts_strsize name_size, FILE *restrict nonnull stream);
enum nbts_error
nbts_skip_compound(void *nullable, nbts_strsize name_size, FILE *restrict nonnull stream);

#undef nonnull
#undef nullable
