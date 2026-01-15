#ifndef HEADER_STACK
#define HEADER_STACK

#include <malloc.h>

// the stack structure
typedef struct {
    void *data;
    long ptr, size, isize;
} Stack;

/*
* Creates a stack.
* @param capacity The capacity of the stack
* @param type     The type of each item of the stack
* @returns A stack, don't forget to free it with FreeStack
*/
#define CreateStack(capcity, type) \
    (Stack) { \
        .data = calloc(capacity, sizeof(type)), \
        .ptr = 0, .size = capacity, .isize = sizeof(type) \
    }

/*
* Frees a stack created with CreateStack.
* @param stack The stack to free
*/
#define FreeStack(stack) \
    if (stack.data && stack.size && stack.isize) { \
        free(stack.data); \
        stack.size = 0, stack.ptr = 0, stack.isize = 0; \
    }

/*
* Pushes an item to the end of the stack.
* @param stack the stack
* @param item  The item.
* @param type  The item type, please don't lie.
*/
#define Push(stack, item, type) \
    if (sizeof(type) == sizeof(item) && stack.ptr < stack.capacity) \
        ((type *)stack.data)[stack.ptr++] = item

/*
* Pops the last item from the stack. 
* If it is not possible to reach it returns 0
* @param stack The stack
* @param type  The item type, please don't lie.
* @returns The item poped.
*/
#define PopLast(stack, type) \
    (sizeof(type) * stack.capacity == stack.isize * stack.capacity && stack.ptr > 0)? \
        ((type *)stack.data)[--stack.ptr] \
        : 0 // 0 per default

#endif