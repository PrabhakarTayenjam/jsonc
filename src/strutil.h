#ifndef __STRUTIL_H__
#define __STRUTIL_H__

/*
 *If a function name ends with '_m' suffix, it is a mutator.
 *It may modify the concerned argument.
 */

char *str_stripl(char *str);

char *str_strip_m(char *str);

char *str_skip(char *str, char ch);

char *str_get_m(char **str, char *dlm);

#endif