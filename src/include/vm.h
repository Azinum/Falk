/* vm.h */
/* Author: Azinum */
/* Date: 2016-09-19 */

#ifndef vm_h
#define vm_h

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "list.h"
#include "object.h"
#include "lex.h"
#include "table.h"

extern_c

#define VM_DEBUG 0

list_define(Stack, Object);
list_define(Instruction_list, void*);

#define vm_begin goto *(VM->program[VM->ip])
#define vm_next goto *(VM->program[++VM->ip])
#define vm_skip(N) (VM->ip += N)

#define vm_jump(N) goto *(VM->program[VM->ip += N])

#define vm_getip(I) (VM->program[I])

#define vm_stack_push(VALUE, MSG) \
if (VM->stack.top < VM->stack.size) { \
    VM->stack.value[VM->stack.top++] = VALUE; \
} \
else { \
    vm_throw_error(VM, VM_ERR_STACK, VM_ERRC_STACK_OVERFLOW, MSG); \
}

#define vm_stack_pop() \
VM->stack.top--

#define vmcase(CASE, BODY, ...) { \
    CASE : { BODY } \
    vm_next; \
}

#define op_arith(L, R, OP) \
(op_arith_issafe(L, R, T_NUMBER) ? (L.value.number OP R.value.number) : (-1))

/*
** this macro is for type checking, to check if two objects has the same type
*/
#define op_arith_issafe(L, R, TYPE) (L.type == TYPE && R.type == TYPE)

/*
** compare type between two objects (binary only)
*/
#define comp_type(A, B, TYPE) ((A.type | B.type) == TYPE)

/*
** TODO: throw exception on error
*/
#define num_arith(OP, MSG) \
if (VM->stack.top > 1) { \
    Object left = obj_convert(list_get_from_top(&VM->stack, -1)); \
    left.value.number = op_arith( \
        left, \
        obj_convert(list_get_top(&VM->stack)), \
    OP); \
    vm_stack_pop(); \
    vm_stack_pop(); \
    vm_stack_push(left, "num_arith"); \
    vm_next; \
} \
vm_throw_error(VM, VM_ERR_STACK, VM_ERRC_STACK_NOT_ENOUGH_ITEMS, MSG); \

#define num_assign(OP, MSG) { \
if (VM->stack.top > 0) { \
    Object left = list_get_from_top(&VM->stack, -1); \
    const Object right = obj_convert(list_get_top(&VM->stack)); \
    if (left.type == T_VAR) { \
        if (op_arith_issafe((*left.value.obj), right, T_NUMBER)) { \
            left.value.obj->value.number OP right.value.number; \
        } \
    } \
    vm_stack_pop(); \
    vm_next; \
} \
    vm_throw_error(VM, VM_ERR_STACK, VM_ERRC_STACK_NOT_ENOUGH_ITEMS, MSG); \
}

#define vm_throw_error(VM, ERR, ERRC, MSG) VM_throw_error(VM, ERR, ERRC, MSG); vm_begin;

/*
** WARNING!
** order reserved
*/
enum VM_instructions {
    VMI_EQ_ASSIGN,
    VMI_ADD,
    VMI_SUB,
    VMI_DIV,
    VMI_MUL,
    VMI_PUSHK,
    VMI_PUSHI,
    VMI_EXIT,
    VMI_GOTO,
    VMI_IF,
    VMI_POP,
    VMI_LT,
    VMI_GT,
    VMI_EQ,
    VMI_LEQ,
    VMI_GEQ,
    VMI_PUSHP,
    VMI_CALLF,
    VMI_ADD_ASSIGN,
    VMI_SUB_ASSIGN,
    VMI_MUL_ASSIGN,
    VMI_DIV_ASSIGN,
    VMI_FUNC_DEFINE,
};


enum VM_exec_mode {
    VM_EXEC_INTERPRET,      /* includes lexing, parsing and final vm execution step */
    VM_EXEC_FILE,   /* deserialize file and convert to vm instructions */
    VM_PARSE_ONLY,
};

static const char* VMI_info[] = {
    "VMI_EQ_ASSIGN",
    "VMI_ADD",
    "VMI_SUB",
    "VMI_DIV",
    "VMI_MUL",
    "VMI_PUSHK",
    "VMI_PUSHI",
    "VMI_EXIT",
    "VMI_GOTO",
    "VMI_IF",
    "VMI_POP",
    "VMI_LT",
    "VMI_GT",
    "VMI_EQ",
    "VMI_LEQ",
    "VMI_GEQ",
    "VMI_PUSHP",
    "VMI_CALLF",
    "VMI_ADD_ASSIGN",
    "VMI_SUB_ASSIGN",
    "VMI_MUL_ASSIGN",
    "VMI_DIV_ASSIGN",
    "VMI_FUNC_DEFINE"
};

static Token vm_asm_keywords[] = {
    {"if", VMI_IF},
    {"push", VMI_PUSHK},
    {"pushi", VMI_PUSHI},
    {"pop", VMI_POP},
    {"call", VMI_CALLF},
    {"goto", VMI_GOTO},
    {"func", VMI_FUNC_DEFINE},
    {"exit", VMI_EXIT},
    {"add", VMI_ADD},
    {"sub", VMI_SUB},
    {"mul", VMI_MUL},
    {"div", VMI_DIV},
    {"eq_assign", VMI_EQ_ASSIGN},
    {"add_assign", VMI_ADD_ASSIGN},
    {"sub_assign", VMI_SUB_ASSIGN},
    {"mul_assign", VMI_MUL_ASSIGN},
    {"div_assign", VMI_DIV_ASSIGN},
    {"eq", VMI_EQ},
    {"lt", VMI_LT},
    {"gt", VMI_GT},
    {"leq", VMI_LEQ},
    {"geq", VMI_GEQ},
};


enum VM_errors {
    VM_NO_ERROR = 0,
    VM_ERR_STACK,
    VM_ERR_ARITH,
    VM_INS_CONV_ERR,    /* instruction conversion error */
    VM_ERR_CALL,    /* function call error */
    VM_ERR_REGISTER,
    VM_ERR_VM,
    VM_ERR_LABEL,
};

enum VM_error_causes {
    VM_ERRC_NONE,
    /* stack */
    VM_ERRC_STACK_NOT_ENOUGH_ITEMS,
    VM_ERRC_STACK_OVERFLOW,
    VM_ERRC_STACK_NOT_INIT,
    /* arith */
    VM_ERRC_ARITH_INVALID_TYPES,
    
    VM_ERRC_NOT_A_FUNC,
    VM_ERRC_NOT_A_NUMBER,
    VM_ERRC_INVALID_LABEL,
    VM_ERRC_NUMBER_TOO_BIG,
    
    VM_ERRC_CALL_INVALID_NUM_ARGS,
    
};

static const char* VM_error_messages[] = {
    "",
    "Stack Error: ",
    "Arithmetic Error: ",
    "Instruction Conversion Error: ",
    "Call Error: ",
    "Register Error: ",
    "VM Error: ",
    "Label Error: ",
};

static const char* VM_error_cause_messages[] = {
    "",
    /* messages for stack */
    "Not enough items on stack; ",
    "Stack overflow; ",
    "Stack has not been initialized yet; ",

    /* arithmetic error causes */
    "Invalid types on arithmetic operation; ",
    "Not a function; ",
    "Not a number; ",
    
    "Not a valid label; ",
    "Number too big; ",
    
    "Invalid amount of args; ",
};

typedef struct Scope {
    struct Scope* global;
    HashTable* variables;
} Scope;

typedef struct VM_instance {
    unsigned char init;     /* VM initialized? */
    unsigned char exit_on_error;
    int status;
    void** program;
    unsigned int ip;     /* pointer to an instruction */
    unsigned int program_size;
    Instruction_list* instructions;     /* all instructions */
    Stack stack;   /* list of objects */
    Scope* global;  /* global scope */
    Object* dummy, obj_null;
    unsigned int stack_size;
} VM_instance;

int VM_execute(VM_instance* VM, int mode, char* input);

Object variable_find(VM_instance* VM, char* name);

int VM_init(VM_instance* VM);

void VM_instance_free(VM_instance* VM);

void** VM_ins_add_instructions(int insc, void* ins, ...);

void** VM_list2instructions(VM_instance* VM, Tokenlist* list);

void** VM_string2bytecode(VM_instance* VM, char* input);

void** VM_asm2bytecode(VM_instance* VM, char* input);

void VM_push_cfunction(VM_instance* VM, char* name, Cfunction function);

void VM_throw_error(VM_instance* VM, int error, int cause, const char* msg);

void VM_debug_print_vmi(VM_instance* VM, void* vmi);

void VM_print_stack(VM_instance* VM);

extern_c_end

#endif /* vm_h */
