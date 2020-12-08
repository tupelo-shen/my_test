/**\file
 *  This file defines the wrapper for the platform/OS related functions
 *  The function definitions needs to be modified according to the platform 
 *  and the Operating system used.
 *  This file should be handled with greatest care while porting the driver
 *  to a different platform running different operating system other than
 *  Linux 2.6.xx.
 * \internal
 * ----------------------------REVISION HISTORY-----------------------------
 * Synopsys			01/Aug/2007			Created
 */
 
#include "synopGMAC_plat.h"

/**
  * This is a wrapper function for Memory allocation routine. In linux Kernel 
  * it it kmalloc function
  * @param[in] bytes in bytes to allocate
  */


#if defined(LOONGSON_2G1A) || defined(LOONGSON_2F1A)
dma_addr_t __attribute__((weak)) gmac_dmamap(unsigned long va,size_t size)
{
	return va;
}
#endif

void *plat_alloc_memory(u32 bytes) 
{
	return (void*)malloc((size_t)bytes, M_DEVBUF, M_DONTWAIT);
}

/**
  * This is a wrapper function for consistent dma-able Memory allocation routine. 
  * In linux Kernel, it depends on pci dev structure
  * @param[in] bytes in bytes to allocate
  */

#if defined(LOONGSON_2G1A) || defined(LOONGSON_2F1A)
void *plat_alloc_consistent_dmaable_memory(u32 size, u32 *addr)
{
	void *buf;
	buf = (void*)malloc((size_t)size, M_DEVBUF, M_DONTWAIT);
	CPU_IOFlushDCache(buf, size, SYNC_W);

	*addr = buf;
	buf = CACHED_TO_UNCACHED(buf);
	return buf;
}
#endif

/**
  * This is a wrapper function for Memory free routine. In linux Kernel 
  * it it kfree function
  * @param[in] buffer pointer to be freed
  */
void plat_free_memory(void *buffer) 
{
	free(buffer,M_DEVBUF);
	return ;
}

#if defined(LOONGSON_2G1A) || defined(LOONGSON_2F1A)
dma_addr_t plat_dma_map_single(void *hwdev, void *ptr,
				size_t size, int direction)
{
	unsigned long addr = (unsigned long) ptr;
	CPU_IOFlushDCache(addr,size, direction);
	/* return addr; */
	return gmac_dmamap(addr,size);
}
#endif

/**
  * This is a wrapper function for platform dependent delay 
  * Take care while passing the argument to this function 
  * @param[in] buffer pointer to be freed
  */
void plat_delay(u32 delay)
{
	while (delay--);
	return;
}


