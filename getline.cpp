#include <stdio.h>
#include <stdlib.h>

ssize_t getline_simple(char **line, size_t *capacity, FILE *fp)
{
    int ch = 0;
    size_t size = 0;

    *line = (char*)calloc(*capacity/sizeof(char), sizeof(char));
    if (*line == NULL)
        return -1;

    while ((ch = fgetc(fp)) != EOF && ch != '\n')
    {
        if (size + 1 >= *capacity)
        {
            *capacity *= 2;
            char *temp = (char*)realloc(*line, *capacity);
            if (temp == NULL)
            {
                free(*line);
                return -1;
            }
            *line = temp;
        }

        (*line)[size++] = ch;
    }

    (*line)[size] = '\0';

    return size;
}

// int main() {
//     char *line = NULL;
//     size_t buff = 64;
//     printf("Enter string: ");
//     int length = getline_simple(&line, &buff, stdin);
//
//     if (length != -1)
//     {
//         printf("your string: %s\n", line);
//     }
//     else
//     {
//         printf("Error\n");
//     }
//
//     free(line);
//     return 0;
// }
