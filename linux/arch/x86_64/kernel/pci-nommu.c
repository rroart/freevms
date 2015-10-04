#include <linux/mm.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/string.h>

/*
 * Dummy IO MMU functions
 */

extern unsigned long end_pfn;

void *pci_alloc_consistent(struct pci_dev *hwdev, size_t size,
                           dma_addr_t *dma_handle)
{
    void *ret;
    int gfp = GFP_ATOMIC;

    if (hwdev == NULL ||
            end_pfn > (hwdev->dma_mask>>PAGE_SHIFT) ||  /* XXX */
            (u32)hwdev->dma_mask < 0xffffffff)
        gfp |= GFP_DMA;
    ret = (void *)__get_free_pages(gfp, get_order(size));

    if (ret != NULL)
    {
        memset(ret, 0, size);
        *dma_handle = virt_to_bus(ret);
    }
    return ret;
}

void pci_free_consistent(struct pci_dev *hwdev, size_t size,
                         void *vaddr, dma_addr_t dma_handle)
{
    free_pages((unsigned long)vaddr, get_order(size));
}
