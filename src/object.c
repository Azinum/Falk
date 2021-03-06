/* object.c */
/* Author: Azinum */
/* Date: 2016-08-30 */

#include <limits.h>
#include <string.h>
#include <math.h>

#include "include/object.h"
#include "include/table.h"
#include "include/falk.h"

extern_c

unsigned char is_number(const char* string) {
    if (strlen(string) <= 0)
        return 0;
    
    char* end;
    strtod(string, &end);

    if (*end != '\0')
        return 0;

    return 1;
}

double to_number(const char* string) {
    if (strlen(string) <= 0)
        return 0;
    
    char* end;
    double number = strtod(string, &end);
    
    if (*end != '\0')
        return INT8_MIN;
    
    return number;
}

void print_object(Object object) {
begin:
    switch (object.type) {
        case T_IDENTIFIER: {
            printf("Identifier\n");
        }
            break;
        
        case T_NUMBER: {
            printf("%.8g\n", object.value.number);
        }
            break;
            
        case T_CSTRING: {
            printf("%s\n", object.value.string);
        }
            break;
            
        case T_SCOPE: {
            printf("Scope: (%p)\n", object.value.ptr);
        }
            break;
            
        case T_CFUNCTION: {
            printf("Cfunction: (%p)\n", object.value.ptr);
        }
            break;
            
        case T_NULL: {
            puts("null");
        }
            break;
            
        case 0: {
            puts("null");
        }
            break;
        
        case T_VAR: {
            printf("Variable @(%p) => ", (void*)object.value.obj);
            if (object.value.obj != NULL) {
                object = *object.value.obj;
                goto begin;
            } else {
                puts("Undefined");
            }
        }
            break;
        
        default: {
            printf("Extern data @: (%p). Type: %i.\n", object.value.ptr, object.type);
        }
            break;
    }
}


int object_is_true(Object obj) {
    switch (obj.type) {
        case T_NUMBER:
            if (obj.value.number != 0)
                return 1;
            break;
            
        case T_CSTRING:
            return ((strcmp(obj.value.string, "")) == 0) ? 0 : 1;
            
        default:
            if (obj.value.ptr != NULL)
                return 1;
            break;
    }
    return 0;
}


int object_is_cstring(Object obj) {
    if (obj.type == T_CSTRING)
        return 1;
    return 0;
}

int object_is_number(Object obj) {
    if (obj.type == T_NUMBER) {
        return 1;
    }
    return 0;
}

char* object2string(Object obj) {
    char what[128];
    for (int i = 0; i < 128; i++)
        what[i] = '\0';
    
    char* ret;
    switch (obj.type) {
        case T_NUMBER: {
            sprintf(what, "%.6g", obj.value.number);
            ret = newx(char, strlen(what));
            strcpy(ret, what);
        }
            break;
            
        case T_CSTRING: {
            sprintf(what, "%s", obj.value.string);
            ret = newx(char, strlen(what));
            strcpy(ret, what);
        }
            break;
            
        case T_SCOPE: {
            sprintf(what, "%p", obj.value.ptr);
            ret = newx(char, strlen(what));
            strcpy(ret, what);
        }
            break;
        
        case T_CFUNCTION: {
            sprintf(what, "%p", obj.value.ptr);
            ret = newx(char, strlen(what));
            strcpy(ret, what);
        }
            break;
        
        case T_POINTER: {
            sprintf(what, "%p", obj.value.ptr);
            ret = newx(char, strlen(what));
            strcpy(ret, what);
        }
            break;
        
        case T_NULL: {
            sprintf(what, "%s", "null");
            ret = newx(char, strlen(what));
            strcpy(ret, what);
        }
            break;
        
        default:
            ret = newx(char, 1);
            *ret = '\0';
            break;
    }
    return ret;
}

extern_c_end
