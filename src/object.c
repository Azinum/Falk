/* object.c */
/* author: Azinum */
/* date: 30/08/16 */

#include "object.h"

Token tokens[] = {
    {"+", OP_ADD},
    {"-", OP_SUB},
    {"*", OP_MUL},
    {"/", OP_DIV}
};

unsigned char is_number(const char* string) {
    char* end;
    strtod(string, &end);

    if (*end != '\0')
        return 0;

    return 1;
}

double to_number(const char* string) {
    char* end;
    double number = strtod(string, &end);
    
    if (*end != '\0')
        return -1;
    
    return number;
}