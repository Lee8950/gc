#pragma once
#ifndef ECL_GC_HPP
#define ECL_GC_HPP

#include <atomic>
#include <stdexcept>
#include <vector>

namespace ecl {

namespace gc {

constexpr uint64_t max_stack_depth = 1 << 8;

class Allocator;

struct AllocatedMemoryBlock {

    AllocatedMemoryBlock();

    uint64_t start_address;

    uint64_t block_size;

    // 0 for not connected to root, will be gced
    int color;

    AllocatedMemoryBlock *next_block;

};

// Here's the idea: At the start of each function that we hope to be managed through gc allocator,
//                  we create a GCMarker object at top. It will be the root of Mark-Sweep impl,
//                  and will be auto recycled through deconstructor. Ugly impl but it will work.                  
class GCMarker {

private:

    void* operator new(std::size_t) = delete;

    void operator delete(void*) noexcept;

    uint64_t *stack_size_pointer;

public:

    int color = 0;

    // STL is good
    std::vector<AllocatedMemoryBlock*> roots;

    GCMarker(uint64_t *ssp, Allocator *allocator);

    ~GCMarker();

    void root_register_helper(AllocatedMemoryBlock* new_amb);

};

class Allocator {

private:

    uint64_t max_mem;

    uint64_t min_mem;

    uint64_t cur_mem;

    AllocatedMemoryBlock* allocated;

    unsigned char *mems;

    AllocatedMemoryBlock* createAMB(uint64_t pos, uint64_t size);

    GCMarker GCMarkerStack[max_stack_depth];

    uint64_t stack_ptr;

public:

    void register_gc_marker(GCMarker &gc_marker);

    Allocator(int Xms, int Xmx);

    void* gc_new(uint64_t size);

    AllocatedMemoryBlock* allocate();

    void collect();

    void mark_from_roots();

};



}

}


#endif