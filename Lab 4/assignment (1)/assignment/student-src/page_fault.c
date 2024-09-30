#include "paging.h"
#include "swapops.h"
#include "stats.h"

/*  --------------------------------- PROBLEM 6 --------------------------------------
    Page fault handler.

    When the CPU encounters an invalid address mapping in a page table,
    it invokes the OS via this handler.

    Your job is to put a mapping in place so that the translation can
    succeed. You can use free_frame() to make an available frame.
    Update the page table with the new frame, and don't forget
    to fill in the frame table.

    Lastly, you must fill your newly-mapped page with data. If the page
    has never mapped before, just zero the memory out. Otherwise, the
    data will have been swapped to the disk when the page was
    evicted. Call swap_read() to pull the data back in.

    HINTS:
         - You will need to use the global variable current_process when
           setting the frame table entry.

    ----------------------------------------------------------------------------------
 */
void page_fault(vaddr_t address) {
    /* First, split the faulting address and locate the page table entry */
   vpn_t virtual_pn = vaddr_vpn(address);

   pte_t *pg_table = (pte_t *)(mem + (PTBR * PAGE_SIZE));      // gets the page table
   pte_t *pg_table_entry = &pg_table[virtual_pn];


    /* It's a page fault, so the entry obviously won't be valid. Grab
       a frame to use by calling free_frame(). */
   pfn_t empty_pfn = free_frame();    


    /* Update the page table entry. Make sure you set any relevant bits. */
   pg_table_entry->pfn = empty_pfn;
   pg_table_entry->dirty = 0;
   pg_table_entry->valid = 1;

   int pfn = pg_table_entry->pfn;

    /* Update the frame table. Make sure you set any relevant bits. */
   frame_table[pfn].vpn = virtual_pn;
   frame_table[pfn].process = current_process;
   frame_table[pfn].referenced = 0;
   frame_table[pfn].mapped = 1;

    /* Initialize the page's memory. On a page fault, it is not enough
     * just to allocate a new frame. We must load in the old data from
     * disk into the frame. If there was no old data on disk, then
     * we need to clear out the memory (why?).
     *
     * 1) Get a pointer to the new frame in memory.
     * 2) If the page has swap set, then we need to load in data from memory
     *    using swap_read().
     * 3) Else, just clear the memory.
     *
     * Otherwise, zero the page's memory. If the page is later written
     * back, swap_write() will automatically allocate a swap entry.
     */

   void *new_pg = (void *)(mem + (pfn * PAGE_SIZE)); // creates the new page in memory

   if (swap_exists(pg_table_entry)){
      swap_read(pg_table_entry, new_pg); // if we can swap the old entry with the new page, then we do it
   }
   else {
      memset(new_pg, 0, PAGE_SIZE * sizeof(uint8_t)); // if not, then we allocate the new memory
   }

   stats.page_faults++; //increment because we have a page fault  

}
