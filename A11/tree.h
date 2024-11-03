#ifndef tree_H_
#define tree_H_

struct element_type
{
  char name[64];
};

struct tree_node 
{
  struct element_type data;
  struct tree_node* left;
  struct tree_node* right;
}; 

extern struct tree_node* find(const char* name, struct tree_node* root);
extern struct tree_node* insert(const char* name, struct tree_node* root);
extern void clear(struct tree_node* root);
extern void print(struct tree_node* root);
extern void printSorted(struct tree_node* root);

#endif
