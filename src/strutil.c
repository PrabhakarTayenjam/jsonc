#include "strutil.h"

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

inline char *
str_stripl(char *str)
{
    while (str && *str != '\0' && (*str == ' ' || *str == '\n' || *str == '\r' || *str == '\t')) {
        str++;
    }
    
    return str;
}

inline char *
str_strip_m(char *str)
{
    int idx = strlen(str) - 1;

    // strip from end first because the str pointer might changed
    while (idx >= 0) {
        if (str[idx] == ' ' || str[idx] == '\n' || str[idx] == '\r' || str[idx] == '\t') {
            idx--;
        } else break;
    }

    // if the str contains only whitespaces
    if (idx == -1)
        return "";
    str[idx + 1] = '\0';

    while (str && (*str == ' ' || *str == '\n' || *str == '\r' || *str == '\t')) {
        str++;
    }
    
    return str;
}

inline char *
str_skip(char *str, char ch)
{
    // forward str to ch
    while (str && *str != '\0') { 
        if (*str == ch) // || *str == '\n' || *str == '}')
            return str + 1;
        str++;
    }
    return str;
}

inline char *
str_get_m(char **str_ptr, char *dlm)
{
    /*
        dlm is excluded
        ***str_ptr is increemented to dlm*** (DO NOT CHANGE THE INCREEMENT)
        ***This functions allocates a new string
    */
    char *sub_str;
    int len;
    char *dlm_itr;
    bool matched = false;
    char *start = start = *str_ptr;

    while (*str_ptr && **str_ptr != '\0') {
        dlm_itr = dlm;
        while(dlm_itr && *dlm_itr != '\0') {
            if (**str_ptr == *dlm_itr) {
                matched = true;
                break;
            }
            dlm_itr++;
        }

        if (matched)
            break;

        (*str_ptr)++;
    }

    len = *str_ptr - start;
    sub_str = (char*)malloc(len + 1);
    strncpy(sub_str, start, len);
    sub_str[len] = '\0';
    
    return sub_str;
}