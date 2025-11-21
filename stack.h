#ifndef STACK_H
#define STACK_H


#ifndef STACK_TYPE
#define STACK_TYPE node_t*
#endif

typedef STACK_TYPE stack_type;

enum error_types
{
    NO_ERRORS     = 0b00000,
    BAD_MAIN_PTR  = 0b10000,
    BAD_STACK_PTR = 0b01000,
    BAD_SIZE      = 0b00100,
    BAD_CAPACITY  = 0b00010,
    MEMORY_ERROR  = 0b00001

};

enum status
{
    SUC = 0,
    FAL = 1
};

struct stack_t
{
    stack_type* data;
    ssize_t size;
    ssize_t capacity;
};

enum status stack_ctor(stack_t* stk, int len);
enum status stack_push(stack_t* stk, stack_type value);
enum status stack_pop (stack_t* stk, stack_type* pop_value = NULL);
void stack_dtor(stack_t* stk);

unsigned int stack_verif(stack_t* stk);
stack_type hash_djb2 (stack_t* stk);
void stack_dump(stack_t* stk, unsigned int error_code);

// void stk_printf(unsigned int index, stack_type value);

extern bool IS_WAS_DUMP;
extern const ssize_t VERY_BIG_NUMBER;
extern unsigned int CODE;
extern const stack_type PTICHKA;

#define VERIFY      if(!IS_WAS_DUMP)                       \
                    {                                      \
                        if((CODE = stack_verif(stk)) != 0) \
                        {                                  \
                            stack_dump(stk, CODE);         \
                            IS_WAS_DUMP = true;            \
                            return FAL;                \
                        }                                  \
                    }

#ifdef DEBUG
#define ON_DEBUG(code) code
#define ON_DEBUG_ELSE(code)
#else
#define ON_DEBUG(code)
#define ON_DEBUG_ELSE(code) code
#endif //DEBUG

#endif
