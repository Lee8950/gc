#include <gc.hpp>

ecl::gc::Allocator::Allocator(int Xms, int Xmx)
 : max_mem(Xmx), min_mem(Xms), cur_mem(Xms), allocated(nullptr), stack_ptr(-1)
{
    mems = new unsigned char[cur_mem];
}

void *ecl::gc::Allocator::gc_new(uint64_t size)
{
    // Step 0: If size > max_mem, then fail.
    if(size > max_mem)
        throw std::runtime_error("Allocator out of memory. Consider raising Xmx of Allocator.");
    // Step 1: Find available space.
    //  Case 1: if none was allocated, then is simple.
    if(allocated == nullptr)
    {
        while(size<cur_mem) {
            if(size * 2 > max_mem) size = max_mem;
            else size *= 2;
        }
        AllocatedMemoryBlock *amb = createAMB(0, size);
        return amb->start_address + mems;
    }
    //  Case 2: There are already allocated blocks.
    else
    {

    }

    // Step 2: If any space was found, return a AMB managing this block.
    //          If none was found, try collect.

    // Step 3: Collect.

    // Step 4: If collected, try step 1 again.
    //          If none was found, expand mem space
}

ecl::gc::AllocatedMemoryBlock::AllocatedMemoryBlock()
    : next_block(nullptr)
{

}

ecl::gc::AllocatedMemoryBlock *ecl::gc::Allocator::createAMB(uint64_t pos, uint64_t size)
{
    auto amb = new AllocatedMemoryBlock;
    amb->start_address = pos;
    amb->block_size = size;
    GCMarkerStack[stack_ptr].root_register_helper(amb);
    return amb;
}

void ecl::gc::GCMarker::root_register_helper(ecl::gc::AllocatedMemoryBlock *new_amb)
{
    if(root == nullptr)
    {
        root = new_amb;
    }
    else
    {
        auto p = root;
        while(p->next_block != nullptr) p = p->next_block;
        p->next_block = new_amb;
    }
}

ecl::gc::GCMarker::GCMarker(uint64_t *ssp, Allocator *allocator)
 : stack_size_pointer(ssp), root(nullptr)
{
    *ssp += 1;
    allocator->register_gc_marker(*this);
}

void ecl::gc::Allocator::register_gc_marker(ecl::gc::GCMarker &gc_marker)
{
    GCMarkerStack[stack_ptr] = gc_marker;
}

void ecl::gc::GCMarker::operator delete(void *) noexcept
{
    // Let it go
}