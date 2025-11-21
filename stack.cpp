#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "akinator.h"
#include "stack.h"

bool IS_WAS_DUMP              = false;
const ssize_t VERY_BIG_NUMBER = 1000000000;
unsigned int CODE             = 0b00000;
// const stack_type PTICHKA      = 0xEBA1BABE;
// stack_type global_valid_hash  = 5381;

enum status stack_ctor(stack_t* stk, int len)
{
    int canary_size = 0;

    ON_DEBUG (canary_size = 2;)

    stk->data = (stack_type*)calloc(len + canary_size, sizeof(stack_type));

    ON_DEBUG (stk->data[0] = stk->data[len+1] = PTICHKA;)

    stk->size = 0;
    stk->capacity = len;

    VERIFY

    return SUC;
}

enum status stack_push(stack_t* stk, stack_type value)
{
    VERIFY

    if (stk->size == stk->capacity)
    {
        ON_DEBUG (ssize_t old_capacity = stk->capacity;)
        stk->capacity = (stk->capacity == 0) ? 1 : stk->capacity * 2;

        stack_type* new_data = (stack_type*)realloc(stk->data,
            (stk->capacity ON_DEBUG( + 2 )) * sizeof(stack_type));

        if (new_data == NULL)
        {
            return FAL;
        }

        stk->data = new_data;

        ON_DEBUG (for (ssize_t i = old_capacity + 1; i <= stk->capacity; i++)
                    {
                        stk->data[i] = 0;
                    })

        ON_DEBUG (stk->data[stk->capacity + 1] = PTICHKA;)
    }

    stk->data[stk->size ON_DEBUG( + 1 )] = value;
    stk->size++;
    // global_valid_hash = global_valid_hash * 33 + value;

    VERIFY

    return SUC;
}

enum status stack_pop(stack_t* stk, stack_type* pop_value)
{
    VERIFY

    if (stk->size == 0)
    {
        printf("Error: stack is empty!\n");
        return FAL;
    }

    stk->size--;

    if (pop_value != NULL)
        *pop_value = stk->data[stk->size ON_DEBUG( + 1 )];

    stk->data[stk->size ON_DEBUG( + 1 )] = 0;
    // global_valid_hash = (global_valid_hash - *pop_value) / 33;

    VERIFY

    return SUC;
}

void stack_dtor(stack_t* stk)
{
    free(stk->data);
    stk->data = NULL;

    stk->size = -1;
    stk->capacity = -1;
}

unsigned int stack_verif(stack_t* stk)
{
    unsigned int error_code = 0b00000;

    if (stk == NULL)
        error_code |= 0b10000;

    else if (stk->data == NULL)
        error_code |= 0b01000;

    else if (stk->size >= stk->capacity + 1 || stk->size > VERY_BIG_NUMBER || stk->size < 0)
        error_code |= 0b00100;

    else if (stk->capacity > VERY_BIG_NUMBER || stk->capacity <= 0 )
        error_code |= 0b00010;

    ON_DEBUG (
    else if (stk->data[0] != PTICHKA || stk->data[stk->capacity+1] != PTICHKA || global_valid_hash != hash_djb2(stk))
        error_code |= 0b00001;
    )

    return error_code;
}

void stack_dump(stack_t* stk, unsigned int error_code)
{
    printf("stack_dump()");

    printf("\nstack: [%p]", stk);
    if ((error_code & BAD_MAIN_PTR))
    {
        printf("BAD MAIN POINTER\n");
        return;
    }

    printf("\n{\n");

    printf("size     = %d ", stk->size);
    if (error_code & BAD_SIZE)
        printf("BAD SIZE");

    printf("\ncapacity = %d ", stk->capacity);
    if (error_code & BAD_CAPACITY)
        printf("BAD CAPACITY");

    printf("\ndata [%p] ", stk->data);
    if (error_code & BAD_STACK_PTR)
    {
        printf("BAD STACK POINTER\n");
        return;
    }

    printf("\n");

    ON_DEBUG (
    if (error_code & MEMORY_ERROR)
        printf("MEMORY_ERROR\n");
    )

    for (ssize_t i = 0; i < stk->capacity ON_DEBUG(+ 2); i++)
    {
        ON_DEBUG (
        if(i < stk->size + 1 && i != 0 && i != stk->capacity + 1)
            printf("*");
        )

        ON_DEBUG_ELSE (
        if(i < stk->size)
            printf("*");
        )

        // stk_printf(i, stk->data[i]);
    }

    printf("\n}\n");
}
//
// stack_type hash_djb2 (stack_t* stk)
// {
//     stack_type hash = 5381;
//
//     for(int i = 0; i < stk->size; i++)
//     {
//         hash *= 33;
//         hash += stk->data[i];
//     }
//
//     return hash;
// }
