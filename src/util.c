#include "util.h"

/*
Calculates the hash of a short string using
hash(i) = hash(i - 1) * 33 ^ str[i];
hash * 33 + c
*/
/*static hash_t djb2(unsigned char *str) {
    hash_t hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash;
}*/

/*
Calculates the hash of a short string using
hash(i) = hash(i - 1) * 65599 + str[i]
*/
inline static
hash_t sdbm(const char *str)
{
    hash_t hash = 0;
    int c;
    while ((c = *str++))
        hash = c + (hash << 6) + (hash << 16) - hash;
    return hash;
}

/*
Calculates the hash of a short string using
This function is used for hashing the hash_key for avl tree
*/
inline hash_t
get_hash(const char *str)
{
    return sdbm(str);
}