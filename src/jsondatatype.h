#ifndef __JSONDATATYPES_H__
#define __JSONDATATYPES_H__

#include <stdbool.h>

// json data types
typedef enum json_dtype
{
    json_dtype_ltr,     // true/false/null
    json_dtype_num,     // numbers
    json_dtype_str,     // strings
    json_dtype_arr,     // [...]
    json_dtype_obj      // {...}
} json_dtype_t;

typedef struct json_value *json_t;
typedef struct json_obj   *json_obj_t;
typedef struct json_arr   *json_arr_t;


/***************************** code for json value *****************************/

json_t json_value_create(void *value, json_dtype_t dtype);

void json_value_free(json_t value);

json_obj_t json_value_to_obj(json_t val);

json_arr_t json_value_to_arr(json_t val);

char *json_value_to_str(json_t val);

void *json_value_to_val(json_t val);

bool  json_value_is_true(json_t val);

bool  json_value_is_false(json_t val);

bool  json_value_is_null(json_t val);

/***************************** code for json object *****************************/

bool json_obj_insert(json_obj_t *json_obj_p, const char *key, json_t value);

json_t json_obj_get(const json_obj_t j_obj, const char *key);

void json_obj_free(json_obj_t j_obj);

/***************************** code for json array *****************************/

void json_arr_init(json_arr_t *json_arr_p);

int json_arr_push_back(json_arr_t *json_arr_p, json_t value);

int json_arr_len(json_arr_t json_arr);

json_t json_arr_get(const json_arr_t json_arr, int idx);

void json_arr_free(json_arr_t arr);


#endif
