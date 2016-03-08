

#pragma once
/*
#define cpu_to_le16wu(p, v) STOREINTELWORD(p, v) // XXX: 
#define cpu_to_le32wu(p, v) STOREINTELDWORD(p, v) // XXX: 

#define le16_to_cpu(a)		LOADINTELWORD(a)
#define le32_to_cpu(a)		LOADINTELDWORD(a)
#define cpu_to_le16w(a,b)	STOREINTELWORD(a,b)
#define cpu_to_le32w(a,b)	STOREINTELDWORD(a,b)
*/
#define cpu_to_le16wu(p, v) STOREINTELWORD(p, v) // XXX: 
#define cpu_to_le32wu(p, v) STOREINTELDWORD(p, v) // XXX: 

#define le16_to_cpu(a)		((UINT16)(a))
#define le32_to_cpu(a)		((UINT32)(a))
#define cpu_to_le16w(a,b)	STOREINTELWORD(a,b)
#define cpu_to_le32w(a,b)	STOREINTELDWORD(a,b)

#define TARGET_PAGE_BITS 12 // i386
#define TARGET_PAGE_SIZE (1 << TARGET_PAGE_BITS)
#define TARGET_PAGE_MASK ~(TARGET_PAGE_SIZE - 1)
#define TARGET_PAGE_ALIGN(addr) (((addr) + TARGET_PAGE_SIZE - 1) & TARGET_PAGE_MASK)

#define xglue(x, y) x ## y
#define glue(x, y) xglue(x, y)
#define stringify(s)	tostring(s)
#define tostring(s)	#s

#define IO_MEM_SHIFT       3
#define IO_MEM_NB_ENTRIES  (1 << (TARGET_PAGE_BITS  - IO_MEM_SHIFT))

#define IO_MEM_RAM         (0 << IO_MEM_SHIFT) /* hardcoded offset */
#define IO_MEM_ROM         (1 << IO_MEM_SHIFT) /* hardcoded offset */
#define IO_MEM_UNASSIGNED  (2 << IO_MEM_SHIFT)
#define IO_MEM_NOTDIRTY    (3 << IO_MEM_SHIFT)

typedef void CPUWriteMemoryFunc(void *opaque, target_phys_addr_t addr, uint32_t_ value);
typedef uint32_t_ CPUReadMemoryFunc(void *opaque, target_phys_addr_t addr);

typedef void QEMUResetHandler(void *opaque);

typedef void (IOPortWriteFunc)(void *opaque, uint32_t_ address, uint32_t_ data);
typedef uint32_t_ (IOPortReadFunc)(void *opaque, uint32_t_ address);

struct QEMUFile {
	int dummy;
};
typedef struct QEMUFile QEMUFile;

/// ‚Â‚­‚ç‚ñ‚Æ‚¢‚©‚ñ‚Ë
target_phys_addr_t isa_mem_base = 0;

static void cpu_physical_memory_set_dirty(ram_addr_t addr)
{
    //phys_ram_dirty[addr >> TARGET_PAGE_BITS] = 0xff; // XXX:
}

void vga_hw_update(void){}

void qemu_console_copy(DisplayState *ds, int src_x, int src_y, int dst_x, int dst_y, int w, int h){}
/*{
    if (is_graphic_console()) {
        dpy_copy(ds, src_x, src_y, dst_x, dst_y, w, h);
    }
}*/
void cpu_physical_sync_dirty_bitmap(target_phys_addr_t start_addr, target_phys_addr_t end_addr){}

int register_ioport_write(int start, int length, int size, IOPortWriteFunc *func, void *opaque){return 0;}
int register_ioport_read(int start, int length, int size, IOPortReadFunc *func, void *opaque){return 0;}

//int cpu_register_io_memory(int io_index, CPUReadMemoryFunc **mem_read, CPUWriteMemoryFunc **mem_write, void *opaque){return 0;}
int cpu_register_io_memory(int io_index,
                           CPUReadMemoryFunc **mem_read,
                           CPUWriteMemoryFunc **mem_write,
                           void *opaque)
{
    //int i, subwidth = 0;

    //if (io_index <= 0) {
    //    io_index = 0;
    //} else {
    //    if (io_index >= IO_MEM_NB_ENTRIES)
    //        return -1;
    //}

    //for(i = 0;i < 3; i++) {
    //    if (!mem_read[i] || !mem_write[i])
    //        subwidth = IO_MEM_SUBWIDTH;
    //    io_mem_read[io_index][i] = mem_read[i];
    //    io_mem_write[io_index][i] = mem_write[i];
    //}
    //io_mem_opaque[io_index] = opaque;
    //return (io_index << IO_MEM_SHIFT) | subwidth;
	return 0;
}

void qemu_register_coalesced_mmio(target_phys_addr_t addr, ram_addr_t size){}

CPUWriteMemoryFunc **cpu_get_io_memory_write(int io_index){return NULL;}
CPUReadMemoryFunc **cpu_get_io_memory_read(int io_index){return NULL;}

void qemu_register_reset(QEMUResetHandler *func, void *opaque){}

void qemu_free(void *ptr)
{
    free(ptr);
}

void *qemu_malloc(size_t size)
{
    return malloc(size);
}

void *qemu_mallocz(size_t size)
{
    void *ptr;
    ptr = qemu_malloc(size);
    if (!ptr)
        return NULL;
    memset(ptr, 0, size);
    return ptr;
}
