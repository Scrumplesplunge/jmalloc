#include "jmalloc.h"

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <memory.h>

// The allocator state is maintained in `free_list`. Upon initialization, the
// `free_list` is initialized as a single entry representing the entire of the
// available heap space. The free list is sorted by memory address, making it
// easy to merge adjacent nodes when freeing.
struct free_node {
  struct free_node* next;
  int size;
};
enum { alloc_unit = sizeof(struct free_node) };
#define jalign(x) (((x) + alloc_unit - 1) / alloc_unit * alloc_unit)

static struct free_node* free_list;

void jinit(void* buffer, int size) {
  char* aligned = (char*)jalign((intptr_t)buffer);
  size = (size + aligned - (char*)buffer) & ~(alloc_unit - 1);
  free_list = (void*)aligned;
  free_list->next = NULL;
  free_list->size = size;
}

void jstats() {
  int fragments = 0;
  int free_space = 0;
  struct free_node* node = free_list;
  while (node) {
    fragments++;
    free_space += node->size;
    node = node->next;
  }
  fprintf(stderr, "fragments: %d\nfree space: %d\n", fragments, free_space);
}

void* jmalloc(int size) {
  size = jalign(size);
  // Find the first node big enough to house the allocation.
  struct free_node** node = &free_list;
  while (*node && (*node)->size < size) node = &(*node)->next;
  if (*node == NULL) {
    fprintf(stderr, "heap exhausted.\n");
    exit(1);
  }
  if (size == (*node)->size) {
    // Allocation consumes the entire node.
    void* buffer = (void*)(*node);
    *node = (*node)->next;
    return buffer;
  } else {
    // Allocation splits the node.
    (*node)->size -= size;
    return (char*)(*node) + (*node)->size;
  }
}

void jfree(void* p, int size) {
  if (!p) return;
  size = jalign(size);
  if (!free_list) {
    // The heap was completely exhausted and this is the only available memory.
    free_list = p;
    free_list->size = size;
    free_list->next = NULL;
    return;
  }
  // Find the last node which is before the allocation in memory.
  struct free_node** node = &free_list;
  while ((*node)->next && (void*)(*node)->next < p) node = &(*node)->next;
  char* end = (char*)(*node) + (*node)->size;
  if (p == end) {
    // Extend the existing node.
    (*node)->size += size;
    end += size;
    struct free_node* next = (*node)->next;
    if ((char*)next == end) {
      // The newly freed memory makes this node adjacent to the
      // next node, so we can merge that one in too.
      (*node)->size += next->size;
      (*node)->next = next->next;
    }
  } else {
    // Add a new free node.
    struct free_node* new_node = p;
    new_node->size = size;
    new_node->next = (*node)->next;
    (*node)->next = new_node;
  }
}
