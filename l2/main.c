#include <stdio.h>
#include <stdlib.h>

#include <inttypes.h>
#include <string.h>

#include <ctype.h>
#include <stdbool.h>

#include <limits.h>


#define max(a, b) (a) > (b) ? (a) : (b)

#ifndef PATH_MAX
#define PATH_MAX 260
#endif

#define KEY_LEN 257
#define COMMAND_BUFFER_LENGTH 1 + 1 + KEY_LEN + 1 + 20 + 1 + 1



void str_to_lower(char *string, size_t length) {
    for (size_t char_index = 0; char_index < length - 1 && string[char_index] != '\0'; ++char_index) {
        string[char_index] = tolower(string[char_index]);
    }

    string[length - 1] = '\0';
}


typedef struct {
    char key[KEY_LEN];
    uint64_t value;
} key_value;


key_value key_value_new(char key[KEY_LEN], uint64_t value) {
    key_value kv;

    strncpy(kv.key, key, KEY_LEN);
    kv.key[KEY_LEN - 1] = '\0';
    kv.value = value;

    return kv;
}



typedef struct node {
    struct node *left;
    struct node *right;
    key_value value;
    int64_t height;
} node;


node* node_new(key_value value) {
    node *new = calloc(1, sizeof(node));

    if (!new) return NULL;

    new->left = NULL;
    new->right = NULL;
    new->value = value;
    new->height = 0;

    return new;
}


void node_free(node *current) {
    if (current == NULL) return;
    node_free(current->left);
    node_free(current->right);
    free(current);
}


int64_t node_height(node *current) {
    if (current == NULL) return -1;
    return current->height;
}

short node_balance(node *current) {
    if (current == NULL) return 0;
    return node_height(current->left) - node_height(current->right);
}



typedef enum {
    ok,
    error,
    exists,
} avl_result;

typedef struct {
    node *root;
} avl;


avl* avl_new() {
    return calloc(1, sizeof(avl));
}


/*

    A              B     
   / \            / \    
  B   z     =>   x   A   
 / \                / \  
x   y              y   z 

*/
node* avl_rotate_right(node *top) {
    node *new_top = top->left;
    node *moved_child = new_top->right;

    // rotate
    new_top->right = top;
    top->left = moved_child;

    // update heights
    top->height = max(node_height(top->left), node_height(top->right)) + 1;
    new_top->height = max(node_height(new_top->left), node_height(new_top->right)) + 1;

    return new_top;
}

/*

  A               B   
 / \             / \  
x   B     =>    A   z 
   / \         / \    
  y   z       x   y   

*/
node* avl_rotate_left(node *top) {
    node *right_son = top->right;
    node *moved_child = right_son->left;

    // rotate
    right_son->left = top;
    top->right = moved_child;

    // update heights
    top->height = max(node_height(top->left), node_height(top->right)) + 1;
    right_son->height = max(node_height(right_son->left), node_height(right_son->right)) + 1;

    return right_son;
}


node* avl_rebalance(node *current) {
    short balance = node_balance(current);

    if (balance > 1) {
        if (node_balance(current->left) < 0) current->left = avl_rotate_left(current->left);
        current = avl_rotate_right(current);
    }

    if (balance < -1) { 
        if (node_balance(current->right) > 0) current->right = avl_rotate_right(current->right);
        current = avl_rotate_left(current);
    }

    return current;
}


node* _avl_insert(node *current, key_value value, avl_result *result) {
    // insert a leaf
    if (current == NULL) {
        node *new_node = node_new(value);
        if (new_node == NULL) *result = error;
        else *result = ok;
        return new_node;
    }

    // choose a branch
    int cmp = strcmp(value.key, current->value.key);

    if (cmp < 0) {
        current->left = _avl_insert(current->left, value, result);
    } else if (cmp > 0) {
        current->right = _avl_insert(current->right, value, result);
    } else {
        return current;
    }

    // update height
    current->height = max(node_height(current->left), node_height(current->right)) + 1;

    return avl_rebalance(current);
}

avl_result avl_insert(avl *tree, key_value value) {
    avl_result ok = exists;

    str_to_lower(value.key, KEY_LEN);
    tree->root = _avl_insert(tree->root, value, &ok);
    return ok;
}


node* _avl_delete(node *current, const char *key, avl_result *result) {
    if (current == NULL) return NULL;

    // choose a branch
    int cmp = strcmp(key, current->value.key);

    if (cmp < 0) {
        current->left = _avl_delete(current->left, key, result);
    } else if (cmp > 0) {
        current->right = _avl_delete(current->right, key, result);
    } else {
        *result = ok;
        // only one child or no children
        if (current->left == NULL) {
            node *right_child = current->right;
            free(current);
            return right_child;
        } else if (current->right == NULL) {
            node *left_child = current->left;
            free(current);
            return left_child;
        }

        // two children: get minimal element in the right child
        node *minimal = current->right;
        while (minimal->left != NULL) minimal = minimal->left;

        // copy from it and delete
        current->value = minimal->value;
        // strncpy(current->value.key, minimal->value.key, KEY_LEN);
        // current->value.value = minimal->value.value;

        avl_result _;
        current->right = _avl_delete(current->right, minimal->value.key, &_);
    }

    // update height
    current->height = max(node_height(current->left), node_height(current->right)) + 1;

    return avl_rebalance(current);
}

avl_result avl_delete(avl *tree, const char *key) {
    if (tree->root == NULL) return false;

    char lowercase_key[KEY_LEN];
    strncpy(lowercase_key, key, KEY_LEN);
    str_to_lower(lowercase_key, KEY_LEN);

    avl_result deleted = error; 
    tree->root = _avl_delete(tree->root, lowercase_key, &deleted);
    return deleted;
}


uint64_t _avl_find(node *current, const char *key, avl_result *found) {
    if (current == NULL) return 0;

    int cmp = strcmp(current->value.key, key);

    if (cmp == 0) {
        *found = ok;
        return current->value.value;
    } else if (cmp > 0) {
        return _avl_find(current->left, key, found);
    } else {
        return _avl_find(current->right, key, found);
    }
}

uint64_t avl_find(avl tree, const char *key, avl_result *result) {
    char lowercase_key[KEY_LEN];
    strncpy(lowercase_key, key, KEY_LEN);
    str_to_lower(lowercase_key, KEY_LEN);

    *result = error;
    uint64_t value = _avl_find(tree.root, lowercase_key, result);
    if (*result == ok) return value;
    return 0;
}


void avl_free(avl *tree) {
    node_free(tree->root);
    free(tree);
}


void _avl_inorder_print(node *current) {
    if (current == NULL) return;

    printf("[");
    _avl_inorder_print(current->left);
    printf("]%s:%"PRIu64, current->value.key, current->value.value);
    // printf("%s", n->value.key);
    printf("{");
    _avl_inorder_print(current->right);
    printf("}");
}

void avl_inorder_print(avl tree) {
    _avl_inorder_print(tree.root);
    printf("\n");
}


void _avl_save_to_path(node *current, FILE *file) {
    if (current == NULL) return;

    _avl_save_to_path(current->left, file);
    fprintf(file, "%s %"PRIu64"\n", current->value.key, current->value.value);
    _avl_save_to_path(current->right, file);
}

int avl_save_to_path(avl tree, const char *path) {
    FILE *file = fopen(path, "w");
    if (file == NULL) {
        return error;
    }

    _avl_save_to_path(tree.root, file);

    fclose(file);

    return ok;
}

avl* avl_load_from_path(const char *path) {
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        return NULL;
    }

    char key[KEY_LEN];
    uint64_t value;

    avl *tree = avl_new();
    while (fscanf(file, "%s %"PRIu64, key, &value) != EOF) {
        avl_insert(tree, key_value_new(key, value));
    }

    fclose(file);

    return tree;
}



int main() {
    avl *tree = avl_new();

    // avl_insert(tree, key_value_new("A", 1));
    // avl_insert(tree, key_value_new("B", 2));
    // avl_insert(tree, key_value_new("C", 3));
    // avl_insert(tree, key_value_new("D", 4));
    // avl_insert(tree, key_value_new("z", 0));
    // avl_insert(tree, key_value_new("x", 8));
    // avl_insert(tree, key_value_new("y", 0));

    // avl_inorder_print(*tree);

    // avl_result found = error;
    // uint64_t value = avl_find(*tree, "x", &found);
    // if (found == ok) printf("OK:%"PRIu64"\n", value);
    // else printf("NoSuchWord\n");

    // avl_delete(tree, "b");
    // avl_delete(tree, "d");
    // avl_delete(tree, "y");

    // avl_inorder_print(*tree);

    char command_buffer[COMMAND_BUFFER_LENGTH];

    while (fgets(command_buffer, COMMAND_BUFFER_LENGTH, stdin) != NULL) {
        if (command_buffer[0] != '+' && command_buffer[0] != '-' && command_buffer[0] != '!') {
            memmove(command_buffer + 2, command_buffer, COMMAND_BUFFER_LENGTH);
            command_buffer[0] = '?';
            command_buffer[1] = ' ';
        }

        if (command_buffer[0] == '!') {
            char command[5];
            char path[PATH_MAX];

            sscanf(command_buffer, "! %s %s", command, path);

            switch (command[0]) {
                case 'S':;
                    if (avl_save_to_path(*tree, path) == ok) printf("OK\n");
                    else printf("ERROR: Could not write to a file %s\n", path);
                    break;
                case 'L':;
                    avl *temp_tree = avl_load_from_path(path);
                    if (temp_tree) {
                        printf("OK\n");
                        avl_free(tree);
                        tree = temp_tree;
                    } else {
                        printf("ERROR: Could not read from file %s\n", path);
                    }
                    break;
            }
        } else {
            char command = 0;
            char key[KEY_LEN];
            uint64_t value = 0;

            sscanf(command_buffer, "%c %s %"PRIu64, &command, key, &value);

            switch (command) {
                case '+':;
                    avl_result result = avl_insert(tree, key_value_new(key, value));
                    switch (result) {
                        case error:
                            printf("ERROR: Buy more RAM, lol\n");
                            break;
                        case exists:
                            printf("Exists\n");
                            break;
                        case ok:
                            printf("OK\n");
                            break;
                    }
                    break;
                case '-':;
                    if (avl_delete(tree, key) == ok) printf("OK\n");
                    else printf("NoSuchWord\n");
                    break;
                default:;
                    avl_result found = error;
                    uint64_t value = avl_find(*tree, key, &found);
                    if (found == ok) printf("OK: %"PRIu64"\n", value);
                    else printf("NoSuchWord\n");
            }
        }
    }

    avl_free(tree);

    return 0;
}