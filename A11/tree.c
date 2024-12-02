#include "tree.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"  // Added to use strcmp and strncpy

struct tree_node* find(const char* name, struct tree_node* root)
{
  if (root == NULL)
    return NULL;
  
  int cmp = strcmp(name, root->data.name);
  if (cmp < 0)
    return find(name, root->left);
  else if (cmp > 0)
    return find(name, root->right);
  else
    return root;
}

struct tree_node* insert(const char* name, struct tree_node* root)
{
  if (root == NULL) {
    struct tree_node* new_node = malloc(sizeof(struct tree_node));
    if (new_node == NULL)
      return NULL;
    strncpy(new_node->data.name, name, sizeof(new_node->data.name));
    new_node->left = NULL;
    new_node->right = NULL;
    return new_node;
  }

  int cmp = strcmp(name, root->data.name);
  if (cmp < 0)
    root->left = insert(name, root->left);
  else if (cmp > 0)
    root->right = insert(name, root->right);

  return root;
}

void clear(struct tree_node* root)
{
  if (root != NULL) {
    clear(root->left);
    clear(root->right);
    free(root);
  }
}

void print(struct tree_node* root)
{
  if (root != NULL) {
    printf("%s\n", root->data.name);
    if (root->left != NULL)
      printf(" l:%s\n", root->left->data.name);
    if (root->right != NULL)
      printf(" r:%s\n", root->right->data.name);
    print(root->left);
    print(root->right);
  }
}

void printSorted(struct tree_node* root)
{
  if (root != NULL) {
    printSorted(root->left);
    printf("%s\n", root->data.name);
    printSorted(root->right);
  }
}

