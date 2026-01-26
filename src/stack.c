#include "stack.h"

#include <string.h>

void Insert(Stack *stack, size_t idx, void *item) {
    if (idx + stack->ptr >= stack->size) {
        // out of range...
        // get the old data ptr
        void *oldptr = stack->data;

        // allocates more space
        stack->data = calloc((stack->size + 1) * 2, stack->isize);
        stack->size = (stack->size + 1) * 2;

        // copy old data in with the next item inserted
        memcpy(stack->data                                   , oldptr                             , idx * stack->isize               );
        memcpy((char *)stack->data + idx * stack->isize      , item                               , stack->isize                     );
        memcpy((char *)stack->data + (idx + 1) * stack->isize, (char *)oldptr + idx * stack->isize, (stack->ptr - idx) * stack->isize);

        // free the old ptr
        free(oldptr);
    }
    else {
        // enough place in the buffer
        // move copy after the others item that were after and the item at its place (idx)
        memcpy((char *)stack->data + (idx + 1) * stack->isize, (char *)stack->data + idx * stack->isize, (stack->ptr - idx) * stack->isize);
        memcpy((char *)stack->data + idx * stack->isize      , item                                    , stack->isize                     );
    }
}

void RemoveAt(Stack *stack, size_t idx) {
    void *item = (char *)stack->data + idx * stack->isize;
    memmove(item, (char *)item + stack->isize, (stack->ptr - idx) * stack->isize);
    stack->ptr--;
}