#include <bits/stdc++.h>

const int GC_TAG_NONE = 0x00;
const int GC_TAG_ROOT = 0x01;
const int GC_TAG_MARK = 0x02;
const size_t PTRSIZE = sizeof(char*);

struct Allocation {
    void* ptr;
    size_t size;
    char tag;
    void (*dtor)(void*);
    Allocation *next;
};

struct AllocationMapping {
    size_t capacity;
    size_t min_capacity;
    double downsize_factor;
    double upsize_factor;
    double sweep_factor;
    size_t sweep_limit;
    size_t size;
    Allocation** allocs;
};

struct GarbageCollector {
    AllocationMapping* allocs;
    bool paused;
    void *bos;
    size_t min_size;
};

static bool is_prime(size_t n)
{
    /* https://stackoverflow.com/questions/1538644/c-determine-if-a-number-is-prime */
    if (n <= 3)
        return n > 1;     // as 2 and 3 are prime
    else if (n % 2==0 || n % 3==0)
        return false;     // check if n is divisible by 2 or 3
    else {
        for (size_t i=5; i*i<=n; i+=6) {
            if (n % i == 0 || n%(i + 2) == 0)
                return false;
        }
        return true;
    }
}

static size_t next_prime(size_t n)
{
    while (!is_prime(n)) ++n;
    return n;
}

double gc_allocation_map_load_factor(AllocationMapping* am)
{
    return static_cast<double>(am->size) / static_cast<double>(am->capacity);
}

// init
AllocationMapping* gc_allocation_map_new(size_t min_capacity,
                                         size_t capacity,
                                         double sweep_factor,
                                         double downsize_factor,
                                         double upsize_factor)
{
    AllocationMapping* am = (AllocationMapping*)malloc(sizeof(AllocationMapping));
    am->min_capacity = next_prime(min_capacity);
    am->capacity = std::max(am->min_capacity, next_prime(capacity));
    am->sweep_factor = sweep_factor;
    am->sweep_limit = static_cast<int>(sweep_factor * am->capacity); // When over sweep limit, start gc?
    am->downsize_factor = downsize_factor;
    am->upsize_factor = upsize_factor;
    am->allocs = (Allocation**)calloc(am->capacity, sizeof(Allocation*));
    am->size = 0;
}

void gc_start(GarbageCollector* gc, void* bos)
{
    gc_start_handler(gc, bos, 1024, 1024);
}

void gc_start_handler(GarbageCollector* gc,
                      void* bos,
                      size_t initial_capacity,
                      size_t min_capacity,
                      double downsize_load_factor = 0.2,
                      double upsize_load_factor = 0.8,
                      double sweep_factor = 0.5)
{
    // start gc
    gc->paused = false;
    gc->bos = bos;
    double cap = initial_capacity < min_capacity ? min_capacity : initial_capacity;
    gc->allocs = gc_allocation_map_new(min_capacity, cap, sweep_factor, downsize_load_factor, upsize_load_factor);
}

void gc_pause(GarbageCollector* gc)
{
    gc->paused = true;
}

void gc_resume(GarbageCollector* gc)
{
    gc->paused = false;
}

size_t gc_hash(void *ptr)
{
    return ((uintptr_t)ptr) >> 3;
}

Allocation* gc_allocation_map_get(AllocationMapping* am, void* ptr)
{
    size_t index = gc_hash(ptr) % am->capacity;
    Allocation* cur = am->allocs[index];
    while(cur) {
        if(cur->ptr == ptr) {
            return cur;
        }
        cur = cur->next;
    }
    return 0;
}

void gc_mark_alloc(GarbageCollector* gc, void* ptr)
{
    // Locate Allocation fast by using hashmap
    Allocation* alloc = gc_allocation_map_get(gc->allocs, ptr);
    if(alloc && !(alloc->tag & GC_TAG_MARK))
    {
        alloc->tag |= GC_TAG_MARK;
        for(char* p = (char*)alloc->ptr;
            p <= (char*)alloc->ptr+alloc->size-PTRSIZE;
            ++p)
        {
            gc_mark_alloc(gc, *(void**)p);
        }
    }
}

void gc_mark_stack(GarbageCollector* gc)
{
    void *tos = __builtin_frame_address(0);
    void *bos = gc->bos;
    for(char* p = (char*)tos; p <= (char*)bos - PTRSIZE; ++p)
    {
        gc_mark_alloc(gc, *(void**)p);
    }
}

void gc_mark_roots(GarbageCollector* gc)
{
    for(size_t i = 0; i < gc->allocs->capacity; i++)
    {
        Allocation* chunk = gc->allocs->allocs[i];
        while(chunk)
        {
            if(chunk->tag & GC_TAG_ROOT) {
                gc_mark_alloc(gc, chunk->ptr);
            }
            chunk = chunk->next;
        }
    }
}

void gc_mark(GarbageCollector* gc)
{
    gc_mark_roots(gc);
    void(*volatile _mark_stack)(GarbageCollector*) = gc_mark_stack;
    jmp_buf ctx;
    memset(&ctx, 0, sizeof(jmp_buf));
    setjmp(ctx);
    _mark_stack(gc);
}

size_t gc_run(GarbageCollector* gc)
{
    gc_mark(gc);
    return
}

size_t gc_stop(GarbageCollector* gc)
{

}


int main()
{
    return 0;
}