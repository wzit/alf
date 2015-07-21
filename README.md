# alf

[![Linux Build Status](https://img.shields.io/travis/dawikur/alf/master.svg?label=Linux%20test%20build)](https://travis-ci.org/dawikur/alf)

Allocator using custom memory pool.

### How to use it?

    std::vector<int , alf::allocator<int>> v;

This will create a vector with a default memory alocator - simply call new and delete.

    std::list<int, alf::allocator<alf::fixed_size_memory_policy<1024>>> l1;
    std::list<char, alf::allocator<alf::fixed_size_memory_policy<1024>>> l2;

This will create one allocator (used by two lists) with memory pool of fixed size - 1024. It will throw std::memory_exception if no memory is left. alf is NOT thread-safe itself!

    using namespace alf;
    std::list<int, allocator<dynamic_size_extended_memory_policy<512,
                                                                 tag<0>>>> l0;
    std::list<int, allocator<dynamic_size_extended_memory_policy<512,
                                                                 tag<1>>>> l1;

Creates two list, both are using their own memory pool (see `tag<...>`) of dynamic allocating memory pool of chunks of 512 bytes.


### Performance

TODO

### Summary

TODO
