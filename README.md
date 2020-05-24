# jsonc
A simple and fast json processing library in C. Its supports validation of json string, parsing of json string and building json.


### To validate a json

    // read the json file
    char *json_str = read_json_file(json_file_path);

    // validate
    if(json_is_valid(json_string)) {
        // Valid json string
    } else {
        Invalid json string
    }


### Json parsing:

    json_t json = json_parse(json_file_path);

### Accessing data from json object

    // get the json object from json value first
    json_obj_t j_obj = json_value_to_obj(json);
    // OR
    json_obj_t j_arr = (json_obj_t)json_value_to_val(json);

    // access the data by its key
    json_t value = json_obj_get(j_obj, "key");

    // get the desire data
    char *str = json_value_to_str(value);
    // OR
    char *str = (char *)json_value_to_val(value);

### Accessing data from json array

    // get the json array from json value first
    json_arr_t j_arr = json_value_to_arr(json);
    // OR
    json_arr_t j_arr = (json_arr_t)json_value_to_val(json);

    // access by index
    json_t value = json_arr_get(idx);

    // get the desire data
    char *str = json_value_to_str(value);
    // OR
    char *str = (char *)json_value_to_val(value);

### Json array can also be traversed

    for(int i = 0; i < json_arr_len(j_arr); ++i) {
        json_value value = json_array_get(j_arr, i);
        
        // process value
    }

### Error handling

    // When some error occurs, json_error is set accordingly
    json_t json = json_parse(json_file_path);
    if(json == NULL) {
        printf("Json error: %s", json_error_str(json_error));
    }

### After finishing, json must be freed

    json_value_free(json);