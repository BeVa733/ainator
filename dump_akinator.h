#ifndef DUMP_AKINATOR_H
#define DUMP_AKINATOR_H

#include "akinator.h"

const int STR_MAX_LEN = 100;

extern unsigned int verify_result;
extern char error_msg[STR_MAX_LEN];

enum tree_verif_error
{
    TREE_NO_ERRORS        = 0b00000000,
    TREE_BAD_MAIN_PTR     = 0b10000000,
    TREE_CYCLE_DETECTED   = 0b01000000,
    TREE_INVALID_NODE     = 0b00100000,
    TREE_BAD_DATA_PTR     = 0b00010000,
    TREE_INVALID_CHILDREN = 0b00001000,
    TREE_PARENT_MISMATCH  = 0b00000100
};

#define TREE_VERIFY                                                         \
    verify_result = tree_verif(head);                                       \
    if (verify_result != TREE_NO_ERRORS)                                    \
    {                                                                       \
        sprintf(error_msg, "Verification error: 0x%04X", verify_result);    \
        tree_dump_add_info(error_msg, head);                                \
    }

unsigned int tree_verif(node_t* node);

void tree_dump_init(void);
void tree_dump_close(void);
void tree_dump_add_info(const char* info, node_t* tree);
bool tree_verify_recursive(node_t* node, bool* visited_nodes, int* visited_count, int capacity);


#endif
