# jmalloc

jmalloc is a simple general-purpose allocator for tiny projects. You provide
a static buffer and jmalloc will dish it out.

Usage:

    // The buffer doesn't have to be aligned, but it improves utilization.
    alignas(16) char buffer[12345];
    // This has to be called before allocating. If you forget, you'll get
    // "heap exhausted".
    jinit(buffer, sizeof(buffer));
    // Now you can allocate!
    void* p = jmalloc(42);
    ...
    // When you free, you have to specify the size of the thing you are freeing.
    // Normally you already know this, but if you don't, just allocate a little
    // bit of extra space for storing the size.
    jfree(p, 42);
