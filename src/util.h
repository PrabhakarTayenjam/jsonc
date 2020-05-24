#ifndef __UTIL_H__
#define __UTIL_H__

#include <stddef.h> // for size_t

typedef size_t hash_t;

hash_t get_hash(const char *str);

#endif
