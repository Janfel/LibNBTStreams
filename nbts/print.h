#pragma once

#include <nbts/nbts.h>

#if __clang__
#define nonnull  _Nonnull
#define nullable _Nullable
#else
#define nonnull
#define nullable
#endif

extern struct nbts_handler const nbts_print_handler;

struct nbts_print_handler_data {
	FILE *nonnull ostream;
	size_t index;
	bool use_singlequotes;
};

struct nbts_print_handler_data nbts_print_handler_data(FILE *nonnull ostream);

enum nbts_error nbts_print_handle_byte(
	struct nbts_print_handler_data *restrict nonnull data,
	size_t name_size,
	FILE *restrict nonnull stream);

enum nbts_error nbts_print_handle_short(
	struct nbts_print_handler_data *restrict nonnull data,
	size_t name_size,
	FILE *restrict nonnull stream);

enum nbts_error nbts_print_handle_int(
	struct nbts_print_handler_data *restrict nonnull data,
	size_t name_size,
	FILE *restrict nonnull stream);

enum nbts_error nbts_print_handle_long(
	struct nbts_print_handler_data *restrict nonnull data,
	size_t name_size,
	FILE *restrict nonnull stream);

enum nbts_error nbts_print_handle_float(
	struct nbts_print_handler_data *restrict nonnull data,
	size_t name_size,
	FILE *restrict nonnull stream);

enum nbts_error nbts_print_handle_double(
	struct nbts_print_handler_data *restrict nonnull data,
	size_t name_size,
	FILE *restrict nonnull stream);

enum nbts_error nbts_print_handle_string(
	struct nbts_print_handler_data *restrict nonnull data,
	size_t name_size,
	FILE *restrict nonnull stream);

enum nbts_error nbts_print_handle_byte_array(
	struct nbts_print_handler_data *restrict nonnull data,
	size_t name_size,
	FILE *restrict nonnull stream);

enum nbts_error nbts_print_handle_int_array(
	struct nbts_print_handler_data *restrict nonnull data,
	size_t name_size,
	FILE *restrict nonnull stream);

enum nbts_error nbts_print_handle_long_array(
	struct nbts_print_handler_data *restrict nonnull data,
	size_t name_size,
	FILE *restrict nonnull stream);

enum nbts_error nbts_print_handle_list(
	struct nbts_print_handler_data *restrict nonnull data,
	size_t name_size,
	FILE *restrict nonnull stream);

enum nbts_error nbts_print_handle_compound(
	struct nbts_print_handler_data *restrict nonnull data,
	size_t name_size,
	FILE *restrict nonnull stream);


#undef nonnull
#undef nullable
