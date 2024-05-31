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
	nbts_strsize name_size,
	FILE *restrict nonnull stream);

enum nbts_error nbts_print_handle_short(
	struct nbts_print_handler_data *restrict nonnull data,
	nbts_strsize name_size,
	FILE *restrict nonnull stream);

enum nbts_error nbts_print_handle_int(
	struct nbts_print_handler_data *restrict nonnull data,
	nbts_strsize name_size,
	FILE *restrict nonnull stream);

enum nbts_error nbts_print_handle_long(
	struct nbts_print_handler_data *restrict nonnull data,
	nbts_strsize name_size,
	FILE *restrict nonnull stream);

enum nbts_error nbts_print_handle_float(
	struct nbts_print_handler_data *restrict nonnull data,
	nbts_strsize name_size,
	FILE *restrict nonnull stream);

enum nbts_error nbts_print_handle_double(
	struct nbts_print_handler_data *restrict nonnull data,
	nbts_strsize name_size,
	FILE *restrict nonnull stream);

enum nbts_error nbts_print_handle_string(
	struct nbts_print_handler_data *restrict nonnull data,
	nbts_strsize name_size,
	FILE *restrict nonnull stream);

enum nbts_error nbts_print_handle_byte_array(
	struct nbts_print_handler_data *restrict nonnull data,
	nbts_strsize name_size,
	FILE *restrict nonnull stream);

enum nbts_error nbts_print_handle_int_array(
	struct nbts_print_handler_data *restrict nonnull data,
	nbts_strsize name_size,
	FILE *restrict nonnull stream);

enum nbts_error nbts_print_handle_long_array(
	struct nbts_print_handler_data *restrict nonnull data,
	nbts_strsize name_size,
	FILE *restrict nonnull stream);

enum nbts_error nbts_print_handle_list(
	struct nbts_print_handler_data *restrict nonnull data,
	nbts_strsize name_size,
	FILE *restrict nonnull stream);

enum nbts_error nbts_print_handle_compound(
	struct nbts_print_handler_data *restrict nonnull data,
	nbts_strsize name_size,
	FILE *restrict nonnull stream);

enum nbts_error nbts_fprint_bool(FILE *restrict nonnull stream, nbts_byte x);
enum nbts_error nbts_fprint_byte(FILE *restrict nonnull stream, nbts_byte x);
enum nbts_error nbts_fprint_short(FILE *restrict nonnull stream, nbts_short x);
enum nbts_error nbts_fprint_int(FILE *restrict nonnull stream, nbts_int x);
enum nbts_error nbts_fprint_long(FILE *restrict nonnull stream, nbts_long x);
enum nbts_error nbts_fprint_float(FILE *restrict nonnull stream, nbts_float x);
enum nbts_error nbts_fprint_double(FILE *restrict nonnull stream, nbts_double x);
enum nbts_error nbts_fprint_string(
	FILE *restrict nonnull ostream, FILE *restrict nonnull istream, size_t string_size, char quote);
enum nbts_error nbts_fprint_byte_array(
	FILE *restrict nonnull ostream, FILE *restrict nonnull istream, size_t array_size);
enum nbts_error nbts_fprint_int_array(
	FILE *restrict nonnull ostream, FILE *restrict nonnull istream, size_t array_size);
enum nbts_error nbts_fprint_long_array(
	FILE *restrict nonnull ostream, FILE *restrict nonnull istream, size_t array_size);

#undef nonnull
#undef nullable
