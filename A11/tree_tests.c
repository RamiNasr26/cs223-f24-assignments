#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"

void check(int expr, const char* message) {
  if (!expr) {
    printf("%s: FAILED\n", message);
    exit(1);
  }
  else {
    printf("%s: PASSED\n", message);
  }
}

int main (int argc, char* argv[]) 
{
  printf("Running tests...\n");

  struct tree_node* root = NULL;
  root = insert("M", root);

  check(root != 0, "test 1: insert for empty tree");
  check(strncmp(root->data.name, "M", 1) == 0, "test 2: name set correctly");
  check(root->left == 0, "test 3: left is null");
  check(root->right == 0, "test 4: right is null");

  root = insert("A", root);

  check(strncmp(root->left->data.name, "A", 1) == 0, "test 5: name set correctly");
  check(root->right == 0, "test 6: right is null");

  root = insert("X", root);

  check(strncmp(root->right->data.name, "X", 1) == 0, "test 7: name set correctly");

  root = insert("P", root);

  struct tree_node* item = find("B", root);
  check(item == NULL, "test 8: item not in tree");

  item = find("X", root);
  check(item != NULL, "test 9: item in tree");
  check(strncmp(item->data.name, "X", 1) == 0, "test 10: found correct object");

  print(root);
  printSorted(root);
  clear(root);
  return 0;
}

