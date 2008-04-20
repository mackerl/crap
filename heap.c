#include "heap.h"
#include "utils.h"

#include <assert.h>

#define INDEX(P) *((size_t *) (heap->index_offset + (char *) (P)))
#define COMPARE(P,Q) (heap->compare (P, Q))

void heap_init (heap_t * heap, size_t offset, int (*compare) (void *, void *))
{
    heap->entries = NULL;
    heap->num_entries = 0;
    heap->max_entries = 0;
    heap->index_offset = offset;
    heap->compare = compare;
}


/**
 * The heap has a bubble at @c position; shuffle the bubble downwards to an
 * appropriate point, and place @c item in it.  */
static void shuffle_down (heap_t * heap, size_t position, void * item)
{
    while (1) {
        size_t child = position * 2 + 1;
        if (child + 1 > heap->num_entries)
            break;

        if (child + 1 < heap->num_entries
            && COMPARE (heap->entries[child + 1], heap->entries[child]))
            ++child;
        
        if (COMPARE (item, heap->entries[child]))
            break;

        heap->entries[position] = heap->entries[child];
        INDEX (heap->entries[position]) = position;
        position = child;
    }

    heap->entries[position] = item;
    INDEX (item) = position;
}


/**
 * The heap has a bubble at @c position; shuffle the bubble upwards as far as we
 * might need to insert @c item, and then call @c shuffle_down.  */
static void shuffle_up (heap_t * heap, size_t position, void * item)
{
    while (position > 0) {
        size_t parent = (position - 1) >> 1;
        if (!COMPARE (item, heap->entries[parent]))
            break;

        heap->entries[position] = heap->entries[parent];
        INDEX (heap->entries[position]) = position;
        position = parent;
    }

    shuffle_down (heap, position, item);
}


void heap_insert (heap_t * heap, void * item)
{
    /* Create a bubble at the end.  */
    ARRAY_EXTEND (heap->entries, heap->num_entries, heap->max_entries);

    shuffle_up (heap, heap->num_entries - 1, item);
}


void heap_remove (heap_t * heap, void * item)
{
    --heap->num_entries;
    if (INDEX (item) != heap->num_entries)
        /* Shuffle the item from the end into the bubble.  */
        shuffle_up (heap, INDEX (item), heap->entries[heap->num_entries]);
}


void * heap_front (heap_t * heap)
{
    assert (heap->num_entries != 0);
    return heap->entries[0];
}


void * heap_pop (heap_t * heap)
{
    assert (heap->num_entries != 0);
    void * result = heap->entries[0];
    if (--heap->num_entries != 0)
        shuffle_down (heap, 0, heap->entries[heap->num_entries]);
    return result;
}