#ifndef __JSON_PARSER_H__ 
#define __JSON_PARSER_H__

#include "jsondatatype.h"

json_t json_parse(char *json_file_path);

char *read_json_file(const char *file_path);

#endif