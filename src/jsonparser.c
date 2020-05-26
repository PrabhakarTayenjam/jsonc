#include "jsonparser.h"
#include "jsonvalidator.h"
#include "jsonerror.h"
#include "strutil.h"

#include <stdio.h>
#include <stdlib.h>


static json_obj_t json_parse_json_obj(char **buf_ptr);

static json_arr_t
json_parse_json_arr(char **buf_ptr)
{
    json_arr_t j_arr = NULL;
    json_t json_value;
    enum json_dtype json_datatype;
    void *json_data;

    json_arr_init(&j_arr);

    // skipping first '['
    (*buf_ptr)++;
    while (1) {
        *buf_ptr = str_stripl(*buf_ptr);
        switch (**buf_ptr) {
            case '[': {
                json_datatype = json_dtype_arr;
                json_data = (void*)json_parse_json_arr(buf_ptr);
                break;
            }
            case ']': { /*** Json array parsing is completed ***/
                (*buf_ptr)++;   // skipping ']'
                return j_arr;
            }
            case '{': {
                json_datatype = json_dtype_obj;
                json_data = json_parse_json_obj(buf_ptr);
                break;
            }
            case '}': {
                /*
                    Execution should not reached here
                    This char should be handle in 'json_parse_json_obj'
                */
                break;
            }
            case ',': {
                (*buf_ptr)++;   // just skip
                /*
                    Its a comma, don't insert in the array
                    Continue the loop
                */
                continue;
            }
            case '"': {
                json_datatype = json_dtype_str;

                (*buf_ptr)++;   // skip '"'
                json_data = str_get_m(buf_ptr, "\"");
                (*buf_ptr)++;   // skip '"'
                
                break;
            }
            default: {
                if (**buf_ptr == 't' || **buf_ptr == 'f' || **buf_ptr == 'n') {
                    json_datatype = json_dtype_ltr;
                } else {
                    json_datatype = json_dtype_num;
                }

                // dlm: } -> for end of obj     ] -> for end of arr \r -> for windows
                json_data = str_get_m(buf_ptr, ",\n\r}]");
            }
        } // end of switch

        json_value = json_value_create(json_data, json_datatype);
        if(json_value == NULL){
            return NULL;
        } else
            json_arr_push_back(&j_arr, json_value);
    }
}

static json_obj_t
json_parse_json_obj(char **buf_ptr)
{
    json_obj_t j_obj = NULL;
    json_t json_value;
    enum json_dtype json_tag;
    void *json_data;
    char *key;

    // skip '{'
    (*buf_ptr)++;
    while (1) {
        *buf_ptr = str_stripl(*buf_ptr);

        if (**buf_ptr != '}') {
            *buf_ptr = str_skip(*buf_ptr, '"');
            key = str_get_m(buf_ptr, "\"");
            *buf_ptr = str_skip(*buf_ptr, ':');
            *buf_ptr = str_stripl(*buf_ptr);
        }

        switch (**buf_ptr) {
            case '{': {
                json_tag = json_dtype_obj;
                json_data = json_parse_json_obj(buf_ptr);
                break;
            }
            case '}': { /*** Json object parsing is completed ***/
                (*buf_ptr)++;
                return j_obj;
            }
            case '[': {
                json_tag = json_dtype_arr;
                json_data = (void*)json_parse_json_arr(buf_ptr);
                break;
            }
            case ']': {
                /*
                    Execution should not reached this block
                    This char should be handle in 'json_parse_json_arr'
                */
                break;
            }
            case '"': {
                json_tag = json_dtype_str;
                
                (*buf_ptr)++;  // skip the "
                json_data = str_get_m(buf_ptr, "\"");
                (*buf_ptr)++; // skip the "

                break;
            }
            default: {
                if (**buf_ptr == 't' || **buf_ptr == 'f') {
                    json_tag = json_dtype_ltr;
                } else if (**buf_ptr == ','){
                    /* Execution should not reached here */
                    (*buf_ptr)++;  
                } else {
                    json_tag = json_dtype_num;
                }

                json_data = str_get_m(buf_ptr, ",\n\r}");          
            }
        } // end of switch  

        json_value = json_value_create(json_data, json_tag);

        if(json_value == NULL){
            return NULL;
        } else
            json_obj_insert(&j_obj, key, json_value); 
    }
}

char *
read_json_file(const char *file_path)
{
    size_t len;
    char *buffer;
    FILE *fp;

    fp = fopen(file_path, "r");
    if (fp == NULL) {
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    len = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    buffer = (char*)malloc(len + 1);
    if (buffer == NULL) {
        return NULL;
    }
    fread(buffer, 1, len, fp);
    fclose(fp);

    return buffer;
}

json_t 
json_parse(char *file_path)
{
    char *json_buffer = NULL; // contains all the contents of the json file
    char *tmp_buffer; 
    json_t json_root = NULL; // json root
    void *j_val = NULL;
    enum json_dtype dtype;

    json_buffer = read_json_file(file_path);
    if (json_buffer == NULL) {
        json_error = json_error_file_not_exist;
        return NULL;
    }

    tmp_buffer = json_buffer;
    json_buffer = str_stripl(json_buffer);

    if(!json_is_valid(json_buffer)){
        json_error = json_error_invalid_syntax;
        return NULL;
    }

    if(*json_buffer == '[') {
        j_val = json_parse_json_arr(&json_buffer);
        dtype = json_dtype_arr;
    }
    else {
        j_val = json_parse_json_obj(&json_buffer);
        dtype = json_dtype_obj;
    }

    json_root = json_value_create(j_val, dtype);
    
    free(tmp_buffer);

    if (json_root == NULL) {
        json_error = json_error_parsing_failed;
        return NULL;
    }

    return json_root;
}