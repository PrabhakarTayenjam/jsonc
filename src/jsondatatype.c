#include "jsondatatype.h"
#include "util.h"
#include "jsonerror.h"

#include <stdlib.h> // for malloc
#include <string.h> // for strlen

// json value
struct json_value
{
    enum json_dtype dtype;
    void *value;
};

// json object
struct json_obj
{
    hash_t hash_key;
    char   *str_key;
    struct json_value *jvalue;

    int height;
    struct json_obj *left;
    struct json_obj *right;  
};

// json array
struct json_arr
{
    size_t len;
    size_t capacity;
    struct json_value **array;
};


/***************************** code for json_obj value *****************************/

struct json_value *
json_value_create(void *value, enum json_dtype dtype)
{
    struct json_value *json_value;

    if (value == NULL){
        return NULL;
    }

    json_value = (struct json_value *)malloc(sizeof(struct json_value));

    json_value->dtype = dtype;
    json_value->value = value;
    return json_value;
}

void
json_value_free(struct json_value *value)
{
    if(value == NULL)
        return;

    switch (value->dtype) {
        case json_dtype_ltr:
        case json_dtype_num:
        case json_dtype_str: {
            // All of these are char* malloc by get_str_m
            free(value->value);
            break;
        }
        case json_dtype_arr: {
            json_arr_free(value->value);
            break;
        }
        case json_dtype_obj: {
            json_obj_free(value->value);
            break;
        }
    }

    free(value);
}

struct json_obj *
json_value_to_obj(struct json_value *val)
{
    if(val == NULL)
        return NULL;

    if(val->dtype != json_dtype_obj) {
        json_error = json_error_dtype_mismatch_access;
        return NULL;
    }

    return (struct json_obj *)(val->value);
}

struct json_arr *
json_value_to_arr(struct json_value *val)
{
    if(val == NULL)
        return NULL;
    
    if(val->dtype != json_dtype_arr) {
        json_error = json_error_dtype_mismatch_access;
        return NULL;
    }

    return (struct json_arr *)(val->value);
}

char *
json_value_to_str(struct json_value *val)
{
    if(val == NULL)
        return NULL;
    
    if(val->dtype == json_dtype_arr || val->dtype == json_dtype_arr) {
        json_error = json_error_dtype_mismatch_access;
        return NULL;
    }

    return (char *)(val->value);
}

void *
json_value_to_val(struct json_value *val)
{
    if(val == NULL)
        return NULL;

    return val->value;
}

bool
json_value_is_true(struct json_value *val)
{
    if(val == NULL)
        return false;

    if(val->dtype == json_dtype_arr || val->dtype == json_dtype_arr)
        return false;

    if(strcmp((char *)(val->value), "true") == 0)
        return true;
    return false;
}

bool
json_value_is_false(struct json_value *val)
{
    if(val == NULL)
        return false;
    
    if(val->dtype == json_dtype_arr || val->dtype == json_dtype_arr)
        return false;

    if(strcmp((char *)(val->value), "false") == 0)
        return true;
    return false;
}

bool
json_value_is_null(struct json_value *val)
{
    if(val == NULL)
        return false;
    
    if(val->dtype == json_dtype_arr || val->dtype == json_dtype_arr)
        return false;

    if(strcmp((char *)(val->value), "null") == 0)
        return true;
    return false;
}

/***************************** end of code for json_obj value *****************************/

/***************************** code for json_obj tree *****************************/

static inline int
get_node_ht(struct json_obj *node)
{
    if (node)
        return node->height;
    return 0;
}

static inline int
get_bln_factor(struct json_obj *node)
{
    if (node == NULL)
        return 0;

    return (get_node_ht(node->left) - get_node_ht(node->right));
}

static inline void
update_node_height(struct json_obj **node_p)
{
    int a = get_node_ht((*node_p)->left);
    int b = get_node_ht((*node_p)->right);

    (*node_p)->height = ((a > b) ? a : b) + 1;
}

static struct json_obj *
json_obj_create(const char *key, struct json_value *value)
{
    struct json_obj *node;
    
    if(value == NULL || key == NULL) {
        return NULL;
    }
    
    node = (struct json_obj *)malloc(sizeof(struct json_obj));
    node->str_key = (char *)malloc(strlen(key) + 1);

    strcpy(node->str_key, key);
    node->hash_key = get_hash(key);
    node->jvalue = value;
    node->height = 0;
    node->left = NULL;
    node->right = NULL;
    
    return node;
}

static struct json_obj *
json_obj_rotate_left(struct json_obj *node)
{
    struct json_obj *new_node;
    struct json_obj *right;

    if(node == NULL)
        return NULL;

    new_node = node->right;
    right = new_node->left;
    new_node->left = node;
    node->right = right;

    update_node_height(&node);
    update_node_height(&new_node);

    return new_node;
}

static struct json_obj *
json_obj_rotate_right(struct json_obj *node)
{
    struct json_obj *new_node;
    struct json_obj *left;

    if(node == NULL)
        return NULL;

    new_node = node->left;
    left = new_node->right;
    new_node->right = node;
    node->left = left;

    update_node_height(&node);
    update_node_height(&new_node);

    return new_node;
}

static struct json_obj *
json_obj_insert_helper(struct json_obj *j_obj, struct json_obj *node)
{
    struct json_obj *new_node;
    struct json_obj *left;
    struct json_obj *right;
    int bl_factor;

    if (j_obj == NULL)
        return node;

    if (node->hash_key < j_obj->hash_key) {
        j_obj->left = json_obj_insert_helper(j_obj->left, node);
    }
    else if (node->hash_key > j_obj->hash_key) {
        j_obj->right = json_obj_insert_helper(j_obj->right, node);
    }
    else {
        /*  Collision occured. Collision must be avoided. Improved the hash function  */
        json_error = json_error_hash_collision;
        return NULL;
    }

    update_node_height(&j_obj);
    bl_factor = get_bln_factor(j_obj);

    left = j_obj->left;
    right = j_obj->right;

    if (bl_factor > 1 && node->hash_key < left->hash_key) {
        // left left
        return json_obj_rotate_right(j_obj);
    } else if (bl_factor > 1 && node->hash_key > left->hash_key) {
        // left right
        new_node = json_obj_rotate_left(left);
        j_obj->left = new_node;
        return json_obj_rotate_right(j_obj);
    } else if (bl_factor < -1 && node->hash_key > right->hash_key) {
        // right right
        return json_obj_rotate_left(j_obj);
    } else if (bl_factor < -1 && node->hash_key < right->hash_key) {
        // right left
        new_node = json_obj_rotate_right(right);
        j_obj->right = new_node;
        return json_obj_rotate_left(j_obj);
    }
    // *** return unchanged j_obj
    return j_obj;
}

static struct json_obj *
json_obj_get_helper(const struct json_obj *j_obj, hash_t hash_key)
{
    if(j_obj == NULL)
        return NULL;

    if(hash_key == j_obj->hash_key)
        return (struct json_obj *)j_obj;
    
    if(hash_key < j_obj->hash_key)
        return json_obj_get_helper(j_obj->left, hash_key);
    else
        return json_obj_get_helper(j_obj->right, hash_key);
}

bool
json_obj_insert(struct json_obj **json_obj_p, const char *key, struct json_value *value)
{
    struct json_obj *node;

    if(json_obj_p == NULL || key == NULL || value == NULL) {
        return false;
    }

    node = json_obj_create(key, value);
    if(node == NULL) {
        return NULL;
    }

    *json_obj_p = json_obj_insert_helper(*json_obj_p, node);
    if(*json_obj_p == NULL)
        return false;
    return true;
}

json_t
json_obj_get(const json_obj_t j_obj, const char *key)
{
    hash_t hash_key;
    struct json_obj *node;

    if(j_obj == NULL || key == NULL)
        return NULL;

    hash_key = get_hash(key);
    node = json_obj_get_helper(j_obj, hash_key);
    if(node == NULL)
        return NULL;
    return node->jvalue;
}

void
json_obj_free(json_obj_t j_obj)
{
    /* free the nodes in a post-order fashion */
    if (j_obj == NULL)
        return;

    json_obj_free(j_obj->left);
    json_obj_free(j_obj->right);
    json_value_free(j_obj->jvalue);
    free(j_obj->str_key);
    free(j_obj);
}

/***************************** end of code for json object *****************************/

/***************************** code for json array *****************************/

void
json_arr_init(json_arr_t *json_arr_p)
{
    *json_arr_p = (struct json_arr *)malloc(sizeof(struct json_arr));
    (*json_arr_p)->len = 0;
    (*json_arr_p)->capacity = 4; // power of two
    (*json_arr_p)->array = (struct json_value **)malloc((*json_arr_p)->capacity * sizeof(struct json_value));
}

int
json_arr_push_back(struct json_arr **json_arr_p, struct json_value *value)
{
    if(json_arr_p == NULL || *json_arr_p == NULL || value == NULL) {
        return -1;
    }

    if((*json_arr_p)->len >= (*json_arr_p)->capacity) {
        (*json_arr_p)->capacity *= 2;        

        (*json_arr_p)->array = 
            (struct json_value **)realloc((*json_arr_p)->array, sizeof(struct json_value) * (*json_arr_p)->capacity);
    }

    (*json_arr_p)->array[(*json_arr_p)->len] = value;
    (*json_arr_p)->len += 1;
    return (*json_arr_p)->len - 1;
}

int
json_arr_len(struct json_arr *j_arr)
{
    return j_arr->len;
}

struct json_value *
json_arr_get(struct json_arr *j_arr, int idx)
{
    if(j_arr == NULL || j_arr->array == NULL) {
        return NULL;
    }

    if(idx < 0 || idx >= j_arr->len) {
        json_error = json_error_arr_out_of_bound_access;
        return NULL;
    }

    return j_arr->array[idx];
}

void
json_arr_free(struct json_arr *j_arr)
{
    if(j_arr != NULL) {
        for(int i = 0; i < j_arr->len; ++i)
            json_value_free(j_arr->array[i]);

        free(j_arr->array);
        free(j_arr);
    }
}

/***************************** end of code for json array *****************************/
