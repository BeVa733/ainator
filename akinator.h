#ifndef TREE_H
#define TREE_H

enum node_info_type
{
    OBJECT   = 0,
    QUESTION = 1
};

struct node_t
{
    char* data;
    node_t* yes;
    node_t* no;
    node_t* parent;
    enum node_info_type info;
};

enum ans_info
{
    ANOTHER = -1,
    NO      = 0,
    YES     = 1,
};

enum user_commands
{
    GUESS      = 0,
    DEFINITION = 1,
    COMPARE    = 2,
    SHOW_TREE  = 3,
    EXIT       = 4
};

struct path_t
{
    char** path;
    int size;
    int capacity;
};

struct command_table_entry
{
    enum user_commands cmd;
    const char* name;
    const char* description;
    void (*function)(void);
};

extern node_t* head;
extern const command_table_entry command_table[];

node_t* node_ctor(void);
ssize_t getline_simple(char **line, size_t *capacity, FILE *fp);

void place_after(node_t* node_before, node_t* new_node, enum ans_info answer);
void tree_dtor(node_t* node);
void tree_graph(node_t* node);
void tree_dot(FILE* f, node_t* node);

void append_near (node_t* current, char* users_object, char* users_question);
bool get_users_answer (void);
enum user_commands get_user_cmd(void);

node_t* read_tree (const char* filename);
node_t* read_node(char** curr_pos);
char* read_name(char** curr_pos);
long int chek_file_size (FILE* file);

void tree_save (node_t* node, const char* filename);
void write_node(node_t* node, FILE* file);

char* cp1251_to_utf8(const char* cp1251_str);

node_t* find_object     (node_t* node, const char* name);
bool find_path          (node_t* node, const char* target, path_t* path);
bool find_path_recursive(node_t* node, const char* target, path_t* path);
void tree_guess         (node_t* node);
void get_definition     (const char* object_name);
void compare_objects    (const char* object1, const char* object2);
void print_definition   (path_t* path);
void print_comparison   (path_t* path_1, path_t* path_2);

void command_guess     (void);
void command_definition(void);
void command_compare   (void);
void command_show_tree (void);
void command_exit      (void);

#endif
