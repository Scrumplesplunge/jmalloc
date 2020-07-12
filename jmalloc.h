#ifndef JMALLOC_H
#define JMALLOC_H

// Initialize the allocator with a buffer which it can allocate from.
void jinit(char* buffer, int size);

// Output diagnostic stats about the allocator.
void jstats();

// Allocate a chunk of memory with a given size. The allocator
// will iterate over the free list and use the first node which
// is big enough to house the allocation. If the allocation
// perfectly matches the size of a free node, that node will be
// removed entirely. Otherwise, it will be shrunk.
void* jmalloc(int size);

// Free a previously allocated chunk with the given size. The
// size must match the one used when allocating the memory. The
// allocator will iterate over the free list to find the right
// place to insert the new free node, and will either insert a
// new entry, extend the previous/next nodes, or merge the
// previous/next nodes together.
void jfree(void* p, int size);

#endif  // JMALLOC_H
