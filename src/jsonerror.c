#include "jsonerror.h"

char *json_error_string[json_error_noerror + 1] = {
    "json_error_file_not_exist",
    "json_error_invalid_syntax",
    "json_error_parsing_failed",
    "json_error_dtype_mismatch_access",
    "json_error_arr_out_of_bound_access",
    "json_error_hash_collision",
    "json_error_noerror"
};


enum json_error_type json_error = json_error_noerror;

char *
json_error_str(enum json_error_type json_error)
{
    return json_error_string[json_error];
}