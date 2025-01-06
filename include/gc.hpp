#pragma once
#ifndef ECL_GC_HPP
#define ECL_GC_HPP

#include <atomic>
#include <stdexcept>

namespace ecl {

namespace gc {

class Allocator;


struct AllocatedMemoryBlock {

    uint64_t start_address;

    uint64_t block_size;

    AllocatedMemoryBlock *next_block;

};

// Here's the idea: At the start of each function that we hope to be managed through gc allocator,
//                  we create a GCMarker object at top. It will be the root of Mark-Sweep impl,
//                  and will be auto recycled through deconstructor. Ugly impl but it will work.                  
class GCMarker {

private:

    AllocatedMemoryBlock* root;

public:
    GCMarker();

    ~GCMarker();


};

class Allocator {

private:

    uint64_t max_mem;

    uint64_t min_mem;

    uint64_t cur_mem;

    AllocatedMemoryBlock* allocated;

    unsigned char *mems;

    AllocatedMemoryBlock* createAMB(uint64_t pos, uint64_t size);

public:

    Allocator(int Xms, int Xmx);

    void* gc_new(uint64_t size);

    AllocatedMemoryBlock* allocate();

    void collect();

    void mark_from_roots();

};



}

}


#endif