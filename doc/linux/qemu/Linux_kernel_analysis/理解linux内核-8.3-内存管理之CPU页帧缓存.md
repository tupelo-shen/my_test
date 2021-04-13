As we will see later in this chapter, the kernel often requests and releases single page frames. To boost system performance, each memory zone defines a per-CPU page frame cache. Each per-CPU cache includes some pre-allocated page frames to be used for single memory requests issued by the local CPU.

Actually, there are two caches for each memory zone and for each CPU: a hot cache, which stores page frames whose contents are likely to be included in the CPU’s hardware cache, and a cold cache.

Taking a page frame from the hot cache is beneficial for system performance if either the kernel or a User Mode process will write into the page frame right after the allocation. In fact, every access to a memory cell of the page frame will result in a line of the hardware cache being “stolen” from another page frame—unless, of course, the hardware cache already includes a line that maps the cell of the “hot” page frame just accessed.

Conversely, taking a page frame from the cold cache is convenient if the page frame is going to be filled with a DMA operation. In this case, the CPU is not involved and no line of the hardware cache will be modified. Taking the page frame from the cold cache preserves the reserve of hot page frames for the other kinds of memory allocation requests.

The main data structure implementing the per-CPU page frame cache is an array of per_cpu_pageset data structures stored in the pageset field of the memory zone descriptor. The array includes one element for each CPU; this element, in turn, consists of two per_cpu_pages descriptors, one for the hot cache and the other for the cold cache. The fields of the per_cpu_pages descriptor are listed in Table 8-7.

Table 8-7. The fields of the per_cpu_pages descriptor

The kernel monitors the size of the both the hot and cold caches by using two watermarks: if the number of page frames falls below the low watermark, the kernel replenishes the proper cache by allocating batch single page frames from the buddy system; otherwise, if the number of page frames rises above the high watermark, the
kernel releases to the buddy system batch page frames in the cache. The values of
batch, low, and high essentially depend on the number of page frames included in the
memory zone.

## Allocating page frames through the per-CPU page frame caches

The buffered_rmqueue() function allocates page frames in a given memory zone. It makes use of the per-CPU page frame caches to handle single page frame requests.

The parameters are the address of the memory zone descriptor, the order of the memory allocation request order, and the allocation flags gfp_flags. If the __GFP_COLD flag is set in gfp_flags, the page frame should be taken from the cold cache, otherwise it should be taken from the hot cache (this flag is meaningful only for single page frame requests). The function essentially executes the following operations:

1. If order is not equal to 0, the per-CPU page frame cache cannot be used: the function jumps to step 4.

2. Checks whether the memory zone’s local per-CPU cache identified by the value of the __GFP_COLD flag has to be replenished (the count field of the per_cpu_pages descriptor is lower than or equal to the low field). In this case, it executes the following substeps:

    * Allocates batch single page frames from the buddy system by repeatedly invoking the __rmqueue() function.

    * Inserts the descriptors of the allocated page frames in the cache’s list.

    * Updates the value of count by adding the number of page frames actually allocated.

3. If count is positive, the function gets a page frame from the cache’s list, decreases count, and jumps to step 5. (Observe that a per-CPU page frame cache could be empty; this happens when the __rmqueue() function invoked in step 2a fails to allocate any page frames.)

4. Here, the memory request has not yet been satisfied, either because the request spans several contiguous page frames, or because the selected page frame cache is empty. Invokes the __rmqueue() function to allocate the requested page frames from the buddy system.

5. If the memory request has been satisfied, the function initializes the page descriptor of the (first) page frame: clears some flags, sets the private field to zero, and sets the page frame reference counter to one. Moreover, if the __GPF_ZERO flag in gfp_flags is set, it fills the allocated memory area with zeros.

6. Returns the page descriptor address of the (first) page frame, or NULL if the memory allocation request failed.

## Releasing page frames to the per-CPU page frame caches

In order to release a single page frame to a per-CPU page frame cache, the kernel makes use of the free_hot_page() and free_cold_page() functions. Both of them are simple wrappers for the free_hot_cold_page() function, which receives as its parameters the descriptor address page of the page frame to be released and a cold flag specifying either the hot cache or the cold cache.

The free_hot_cold_page() function executes the following operations:

1. Gets from the page->flags field the address of the memory zone descriptor including the page frame (see the earlier section “Non-Uniform Memory Access (NUMA)”).

2. Gets the address of the per_cpu_pages descriptor of the zone’s cache selected by the cold flag.

3. Checks whether the cache should be depleted: if count is higher than or equal to high, invokes the free_pages_bulk() function, passing to it the zone descriptor, the number of page frames to be released (batch field), the address of the cache’s list, and the number zero (for 0-order page frames). In turn, the latter function invokes repeatedly the __free_pages_bulk() function to releases the specified number of page frames—taken from the cache’s list—to the buddy system of the memory zone.

4. Adds the page frame to be released to the cache’s list, and increases the count field.

It should be noted that in the current version of the Linux 2.6 kernel, no page frame is ever released to the cold cache: the kernel always assumes the freed page frame is hot with respect to the hardware cache. Of course, this does not mean that the cold cache is empty: the cache is replenished by buffered_rmqueue() when the low watermark has been reached.

