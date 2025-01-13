#include <gc.hpp>

ecl::gc::Allocator::Allocator(int Xms, int Xmx)
 : max_mem(Xmx), min_mem(Xms), cur_mem(Xms), allocated(nullptr), stack_ptr(-1)
{
    mems = new unsigned char[cur_mem];
    allocated = new AllocatedMemoryBlock;
    allocated->block_size = 0;
    allocated->start_address = 0;
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
        auto p = allocated;
        while(p->next_block != nullptr)
        {
            // Available
            if(p->next_block->start_address - (p->start_address + p->block_size) <= size)
            {
                AllocatedMemoryBlock *amb = createAMB(p->start_address+p->block_size, size);
                amb->next_block = p->next_block;
                p->next_block = amb;
                break;
            }
            p = p->next_block;
        }
    }

    // Step 2: If any space was found, return a AMB managing this block.
    //          If none was found, try collect.

    // Step 3: Mark-Sweep turn.
    for(int i = 0; i < GCMarkerStack.size(); i++)
    {
        for(int j = 0; j < GCMarkerStack[i].roots.size(); j++)
        {
            // Mark
            GCMarkerStack[i].roots[j]->color = 1;
        }
    }
    AllocatedMemoryBlock* p = allocated;
    // Sweep
    while(p->next_block != nullptr)
    {
        // Not marked
        if(p->next_block->color == 0)
        {
            // delete not marked block
            auto q = p->next_block;
            p->next_block = q->next_block;
            delete q;
        }
        else
        {
            p->next_block->color = 0;
        }
        p = p->next_block;
    }

    // Step 4: If collected, try step 1 again.
    //          If none was found, expand mem space
}

ecl::gc::AllocatedMemoryBlock::AllocatedMemoryBlock()
    : next_block(nullptr), color(0)
{

}

ecl::gc::AllocatedMemoryBlock *ecl::gc::Allocator::createAMB(uint64_t pos, uint64_t size)
{
    auto amb = new AllocatedMemoryBlock;
    amb->start_address = pos;
    amb->block_size = size;
    GCMarkerStack.back().root_register_helper(amb);
    return amb;
}

void ecl::gc::Allocator::pop_gc_marker()
{
    GCMarkerStack.pop_back();
}

void ecl::gc::GCMarker::root_register_helper(ecl::gc::AllocatedMemoryBlock *new_amb)
{
    roots.push_back(new_amb);
}

ecl::gc::GCMarker::GCMarker(Allocator *allocator_)
{
    allocator = allocator_;
    allocator_->register_gc_marker(*this);
}

ecl::gc::GCMarker::~GCMarker()
{
    allocator->pop_gc_marker();
}

void ecl::gc::Allocator::register_gc_marker(ecl::gc::GCMarker &gc_marker)
{
    GCMarkerStack.push_back(gc_marker);
}

void ecl::gc::GCMarker::operator delete(void *) noexcept
{
    // Let it go
}