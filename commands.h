node_t* find_object   (node_t* node, const char* name);
bool find_path_stack  (node_t* node, const char* target, stack_t* stk);
void tree_guess       (node_t* node);
void get_definition   (const char* object_name);
void compare_objects  (const char* object1, const char* object2);
void print_definition (stack_t* path, const char* object_name);
void print_comparison (stack_t* path_1, stack_t* path_2, const char* object1, const char* object2);
