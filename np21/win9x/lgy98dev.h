#pragma once

#ifdef __cplusplus
extern "C" {
#endif
	
#define NE2000_PMEM_SIZE    (32*1024)
#define NE2000_PMEM_START   (16*1024)
#define NE2000_PMEM_END     (NE2000_PMEM_SIZE+NE2000_PMEM_START)
#define NE2000_MEM_SIZE     NE2000_PMEM_END

typedef struct {
	UINT16	base;
	//REG8	macaddr[6];
	REG8	irq;
	
    REG8 cmd;
    UINT32 start;
    UINT32 stop;
    REG8 boundary;
    REG8 tsr; //
    REG8 tpsr; //
    UINT16 tcnt; //
    UINT16 rcnt; //
    UINT32 rsar; //
    REG8 rsr;
    REG8 rxcr;
    REG8 isr;
    REG8 dcfg;
    REG8 imr;
    REG8 phys[6]; /* mac address */
    REG8 curpag;
    REG8 mult[8]; /* multicast mask array */
    //qemu_irq irq;
    int isa_io_base;
    //PCIDevice *pci_dev;
    VLANClientState *vc;
    REG8 macaddr[6];
    REG8 mem[NE2000_MEM_SIZE];
	/*
	REG8	cmd;
	REG8	isr;
	REG8	tpsr;
	UINT16	rcnt;
	UINT16	tcnt;

	VLANClientState *vc;*/
} LGY98;

#ifdef __cplusplus
}
#endif

