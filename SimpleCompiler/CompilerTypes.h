#pragma once

#include <stdbool.h>


#ifndef PRJM_F_SIZE
#define PRJM_F_SIZE 8
#endif

#if PRJM_F_SIZE == 4
typedef float PRJM_F;
#else
typedef double PRJM_F;
#endif


struct prjm_eel_exptreenode;

/**
 * @brief Math function pointer with one argument.
 */
typedef PRJM_F (prjm_eel_math_func1)(PRJM_F);

/**
 * @brief Math function pointer with two arguments.
 */
typedef PRJM_F (prjm_eel_math_func2)(PRJM_F, PRJM_F);

/**
 * @brief Math function pointer with three arguments.
 */
typedef PRJM_F (prjm_eel_math_func3)(PRJM_F, PRJM_F, PRJM_F);

/**
 * @brief Node function for a single expression.
 */
typedef void (prjm_eel_expr_func_t)(struct prjm_eel_exptreenode* ctx, PRJM_F** ret_val);

/**
 * @brief Buffer pointer for megabuf/gmegabuf.
 */
typedef PRJM_F** prjm_eel_mem_buffer;

/**
 * @brief Structure containing information about an available function implementation.
 * This struct is used to fill the intrinsic function table and is used to add additional,
 * externally-defined functions to the parser.
 */
typedef struct prjm_eel_function_def
{
    char* name; /*!< The lower-case name of the function in the expression syntax */
    prjm_eel_expr_func_t* func; /*!< A pointer to the function implementation */
    void* math_func; /*!< When using math_func1 or math_func2 in @a func, a pointer to the C library math function. */
    int arg_count; /*!< Number of accepted parameters, 1 to 3. */
    bool is_const_eval; /*!< If true, the function can be evaluated to a constant value at compile time. */
    bool is_state_changing; /*!< If true, the function will change the execution state (set memory) */
} prjm_eel_function_def_t;

typedef struct prjm_eel_function_list_item
{
    prjm_eel_function_def_t* function;
    struct prjm_eel_function_list_item* next;
} prjm_eel_function_list_item_t;

typedef struct
{
    prjm_eel_function_list_item_t* first;
} prjm_eel_function_list_t;

typedef const prjm_eel_function_def_t* prjm_eel_intrinsic_function_list;
typedef prjm_eel_intrinsic_function_list* prjm_eel_intrinsic_function_list_ptr;


typedef struct prjm_eel_variable_def
{
    char* name; /*!< The lower-case name of the variable in the expression syntax. */
    PRJM_F value; /*!< The internal value of the variable. */
    PRJM_F* value_ptr; /*!< The pointer to the actual value of the variable. Either &value or any external pointer. */
} prjm_eel_variable_def_t;

typedef struct prjm_eel_variable_entry
{
    prjm_eel_variable_def_t* variable;
    struct prjm_eel_variable_entry* next;
} prjm_eel_variable_entry_t;

typedef struct
{
    prjm_eel_variable_entry_t* first;
} prjm_eel_variable_list_t;

struct prjm_eel_exptreenode;

typedef struct prjm_eel_exptreenode_list_item
{
    struct prjm_eel_exptreenode* expr;
    struct prjm_eel_exptreenode_list_item* next;
} prjm_eel_exptreenode_list_item_t;

/**
 * @brief A single function, variable or constant in the expression tree.
 * The assigned function will determine how to access the other members.
 */
typedef struct prjm_eel_exptreenode
{
    prjm_eel_expr_func_t* func;
    void* math_func;
    PRJM_F value; /*!< A constant, numerical value. Also used as temp value. */
    union
    {
        prjm_eel_variable_def_t* var; /*!< Variable reference. */
        prjm_eel_mem_buffer memory_buffer; /*!< megabuf/gmegabuf memory block. */
    };
    struct prjm_eel_exptreenode** args; /*!< Function arguments. Last element must be a NULL pointer*/
    prjm_eel_exptreenode_list_item_t* list;  /*!< Next argument in the instruction list. */
} prjm_eel_exptreenode_t;


typedef enum prjm_eel_compiler_node_type
{
    PRJM_EEL_NODE_FUNC_EXPRESSION,
    PRJM_EEL_NODE_FUNC_INSTRUCTIONLIST
} prjm_eel_compiler_node_type_t;


typedef struct prjm_eel_compiler_node
{
    prjm_eel_compiler_node_type_t type; /*!< Node type. Mostly expression, but can contain other types as well. */
    prjm_eel_exptreenode_t* tree_node; /*!< Generated tree node for this expression */
    bool is_const_expr; /*!< If true, this node and all sub nodes only consist of constant expressions, e.g. no variables used */
    bool is_state_changing; /*!< If true, the function will change the execution state (set memory) */
} prjm_eel_compiler_node_t;

typedef struct prjm_eel_compiler_arg_item
{
    prjm_eel_compiler_node_t* node; /*!< Expression for this argument. */
    struct prjm_eel_compiler_arg_item* next; /*!< Next argument in list. */
} prjm_eel_compiler_arg_node_t;

typedef struct prjm_eel_compiler_arg_list
{
    int count; /*!< Argument count in this list */
    prjm_eel_compiler_arg_node_t* begin; /*!< First argument in the list. */
    prjm_eel_compiler_arg_node_t* end; /*!< Last argument in the list. */
} prjm_eel_compiler_arg_list_t;

typedef struct
{
    char* error;
    int line;
    int column;
} prjm_eel_compiler_error_t;

typedef struct
{
    prjm_eel_function_list_t functions;
    prjm_eel_variable_list_t variables;
    prjm_eel_mem_buffer memory;
    prjm_eel_mem_buffer global_memory;
    prjm_eel_compiler_error_t error;
    prjm_eel_exptreenode_t* compile_result;
} prjm_eel_compiler_context_t;

typedef struct
{
    prjm_eel_exptreenode_t* program;
    prjm_eel_compiler_context_t* cctx;
} prjm_eel_program_t;
