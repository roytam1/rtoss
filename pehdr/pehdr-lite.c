#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc, char *argv[])
{
    FILE *fh;
	unsigned int hdr_pos, ohdr_pos;
    unsigned char buf[128] = {0};
    char tmpstr[64];
    unsigned int tmp_ui = 0;
    unsigned short tmp_us = 0;
    unsigned short num_sects = 0;
    unsigned long uninit_datasize = 0;

    unsigned long img_base = 0;

    unsigned long export_va = 0;
    unsigned long export_vsize = 0;
    unsigned long export_ptr = 0;

    unsigned long export_names_ptr = 0;
    unsigned long export_names_addr_fn = 0;
    unsigned long export_names_addr_name = 0;
    unsigned long export_names_addr_odr = 0;

    unsigned long export_names_ptr_pa = 0;
    unsigned long export_names_addr_fn_pa = 0;
    unsigned long export_names_addr_name_pa = 0;
    unsigned long export_names_addr_odr_pa = 0;

    unsigned long import_va = 0;
    unsigned long import_vsize = 0;
    unsigned long import_ptr = 0;

    unsigned long import_fnl = 0;
    unsigned long import_mnames = 0;
    unsigned long import_fal = 0;

    unsigned long import_fnl_pa = 0;
    unsigned long import_mnames_pa = 0;
    unsigned long import_fal_pa = 0;

    unsigned long import_fn_va = 0;
    unsigned long import_fn_pa = 0;

    unsigned long delay_import_va = 0;
    unsigned long delay_import_vsize = 0;
    unsigned long delay_import_ptr = 0;

    unsigned long delay_import_fnl = 0;
    unsigned long delay_import_mnames = 0;

    unsigned long delay_import_fnl_pa = 0;
    unsigned long delay_import_mnames_pa = 0;

    unsigned long img_csize = 0;
    unsigned long img_dsize = 0;

    unsigned long sec_va = 0;
    unsigned long sec_pa = 0;
    unsigned long sec_prd = 0;

	unsigned int dllflag, subsysflag, bssflag;
	unsigned long subsysver, osver, bin_size, oldchksum, newchksum;
	int subsysver_set, osver_set, hdr_changed, bin_changed;

	char* csbuf = 0;

    time_t tt;
	int i,j,k;
    //int mm = byteorder_mm();

	dllflag = subsysflag = bssflag = subsysver = osver = subsysver_set = osver_set = hdr_changed = bin_changed = oldchksum = newchksum = 0;

    if (argc < 2) {
        fprintf(stderr,
            "Missing input file.\n\nUsage: %s <PE-file> [+|-dll] [+|-win] [+|-con] [-bssfix] [-osver <number.number>] [-subsysver <number.number>]",argv[0]);
        return 1;
    }

    if ((fh = fopen(argv[1], "r+b")) == NULL) {
        fprintf(stderr,
            "Unable to open %s.\n",
            argv[1]);
        perror(0);
        return 1;
    }

	for(i = 2; i<argc; i++) {
		if(strncmp(argv[i],"-bssfix",7) == 0) {
			fprintf(stdout, " -> -bssfix set\n");
			bssflag = 1; continue;
		}
		if(strncmp(argv[i],"+dll",4) == 0) {
			fprintf(stdout, " -> +DLL set\n");
			dllflag = 1; continue;
		}
		if(strncmp(argv[i],"-dll",4) == 0) {
			fprintf(stdout, " -> -DLL set\n");
			dllflag = -1; continue;
		}
		if(strncmp(argv[i],"+win",4) == 0 || strncmp(argv[i],"-con",4) == 0) {
			fprintf(stdout, " -> +WIN set\n");
			subsysflag = 2; continue;
		}
		if(strncmp(argv[i],"-win",4) == 0 || strncmp(argv[i],"+con",4) == 0) {
			fprintf(stdout, " -> +CON set\n");
			subsysflag = 3; continue;
		}
		if(strncmp(argv[i],"-osver",6) == 0 && (i+1 < argc)) {
			if(sscanf(argv[i+1],"%d.%d",&j,&k)) {
				fprintf(stdout, " -> -osver = %d.%d\n",j,k);
				osver_set = 1;
				i++;
				osver = (j & 0xffff)<<16 | (k & 0xffff); continue;
			}
		}
		if(strncmp(argv[i],"-subsysver",10) == 0 && (i+1 < argc)) {
			if(sscanf(argv[i+1],"%d.%d",&j,&k)) {
				fprintf(stdout, " -> -subsysver = %d.%d\n",j,k);
				subsysver_set = 1;
				i++;
				subsysver = (j & 0xffff)<<16 | (k & 0xffff); continue;
			}
		}
	}

    /* Read MS-DOS Segment */
	/*typedef struct _IMAGE_DOS_HEADER
	{
	     WORD e_magic;		//  0-1
	     WORD e_cblp;		//	2-3
	     WORD e_cp;			//  4-5
	     WORD e_crlc;		//  6-7
	     WORD e_cparhdr;	//  8-9
	     WORD e_minalloc;	// 10-11
	     WORD e_maxalloc;	// 12-13
	     WORD e_ss;			// 14-15
	     WORD e_sp;			// 16-17
	     WORD e_csum;		// 18-19
	     WORD e_ip;			// 20-21
	     WORD e_cs;			// 22-23
	     WORD e_lfarlc;		// 24-25
	     WORD e_ovno;		// 26-27
	     WORD e_res[4];		// 28-35
	     WORD e_oemid;		// 36-37
	     WORD e_oeminfo;	// 38-39
	     WORD e_res2[10];	// 40-59
	     LONG e_lfanew;		// 60-63
	} IMAGE_DOS_HEADER, *PIMAGE_DOS_HEADER;
	*/
    if (!fread(buf, 64, 1, fh)) {
        fprintf(stderr,
            "Unable to read %d bytes, @%ld.\n",
            2, ftell(fh));
        perror(0);
        fclose(fh);
        return 1;
    }

    /* Check header mark : MZ */
    if (buf[0] != 0x4d || buf[1] != 0x5a) {
        fprintf(stderr,
            "%s is missing Mark Zbikowski header.\n",
            argv[1]);
        fclose(fh);
        return 2;
    }

    /* Get offset (from 0) to IMAGE_NT_HEADERS */
    tmp_ui = *(unsigned int*)(buf+60);

    fseek(fh, tmp_ui - 64, SEEK_CUR);

    /* Read IMAGE_NT_HEADER signature */
	/*typedef struct _IMAGE_NT_HEADERS {
	  DWORD                   Signature;		// 0-3
	  IMAGE_FILE_HEADER       FileHeader;		// 4- [see (a)]
	  IMAGE_OPTIONAL_HEADER32 OptionalHeader;	// [see (b)]
	} IMAGE_NT_HEADERS32, *PIMAGE_NT_HEADERS32;
	*/
    if (!fread(buf, 4, 1, fh)) {
        fprintf(stderr,
            "Unable to read %d bytes, @%ld.\n",
            4, ftell(fh));
        perror(0);
        fclose(fh);
        return 1;
    }

    /* Check signature : PE'x0'x0 */
    if (buf[0] != 0x50 || buf[1] != 0x45 ||
        buf[2] != 0x00 || buf[3] != 0x00) {
        fprintf(stderr,
            "%s is missing valid Portable Executable signature.\n",
            argv[1]);
        fclose(fh);
        return 2;
    }

	hdr_pos = ftell(fh);
	//fprintf(stdout, "IMAGE_FILE_HEADER position = 0x%08X\n", hdr_pos);

    /* (a) Read IMAGE_FILE_HEADER:
    typedef struct _IMAGE_FILE_HEADER {
        WORD  Machine;				//  0-1
        WORD  NumberOfSections;		//  2-3
        DWORD TimeDateStamp;		//  4-7
        DWORD PointerToSymbolTable;	//  8-11
        DWORD NumberOfSymbols;		// 12-15
        WORD  SizeOfOptionalHeader;	// 16-17
        WORD  Characteristics;		// 18-19
    } IMAGE_FILE_HEADER, *PIMAGE_FILE_HEADER;
    */
    if (!fread(buf, 20, 1, fh)) {
        fprintf(stderr,
            "Unable to read %d bytes, @%ld.\n",
            4, hdr_pos);
        perror(0);
        fclose(fh);
        return 1;
    }

	num_sects = *(unsigned short*)(buf+2);
    /* Bittype */
    /*tmp_us = *(unsigned short*)(buf);

    switch (tmp_us) {
    case 0x014c: fprintf(stdout, "Machine: x86 (I386)\n"); break;
    case 0x0166: fprintf(stdout, "Machine: MIPS R4000\n"); break;
    case 0x0184: fprintf(stdout, "Machine: Alpha AXP\n"); break;
    case 0x01C4: fprintf(stdout, "Machine: ARM Thumb-2 (NT)\n"); break;
    case 0x01F0: fprintf(stdout, "Machine: PowerPC\n"); break;
    case 0x0200: fprintf(stdout, "Machine: IA64 (Intel Itanium)\n"); break;
    case 0x0284: fprintf(stdout, "Machine: Alpha AXP (64-bit)\n"); break;
    case 0x8664: fprintf(stdout, "Machine: x64 (AMD64)\n"); break;
    case 0xAA64: fprintf(stdout, "Machine: ARM64\n"); break;
    default: fprintf(stderr,
            "Unable to recognize machine type 0x%04x\n",
            tmp_us);
        fclose(fh);
        return 2;
    }
	fprintf(stdout, "SizeOfOptionalHeader = 0x%04X\n", *(unsigned short*)(buf+16));*/

	tmp_us = *(unsigned short*)(buf+18);
	fprintf(stdout, "Characteristics = 0x%04X\n", tmp_us);

	if(dllflag) {
		if(dllflag>0) {
			tmp_us |= 0x2000;
		} else if (dllflag<0) {
			tmp_us = tmp_us & (0xffff - 0x2000);
		}
	fprintf(stdout, "new Characteristics = 0x%04X\n", tmp_us);

		bin_changed = 1;

		buf[18] = tmp_us & 0xff;
		buf[19] = (tmp_us >> 8) & 0xff;

		fseek(fh, hdr_pos,SEEK_SET);
		fwrite(buf, 1, 20, fh);
		fseek(fh, hdr_pos+20,SEEK_SET);
	}

    /* Timestamp */
    /*tmp_ui = *(unsigned int*)(buf+4);

    tt = tmp_ui;
    strftime(tmpstr, 31, "%a %Y-%m-%d %H:%M:%S", localtime(&tt));
    fprintf(stdout,
        "Time   : %s (%d)\n",
        tmpstr, tmp_ui);*/

    /* ... */
	ohdr_pos = ftell(fh);
	//fprintf(stdout, "IMAGE_OPTIONAL_HEADER position = 0x%08X\n", ohdr_pos);
	/* (b)
	typedef struct _IMAGE_OPTIONAL_HEADER {
	  WORD                 Magic;						//  0-1
	  BYTE                 MajorLinkerVersion;			//  2
	  BYTE                 MinorLinkerVersion;			//  3
	  DWORD                SizeOfCode;					//  4-7
	  DWORD                SizeOfInitializedData;		//  8-11
	  DWORD                SizeOfUninitializedData;		// 12-15
	  DWORD                AddressOfEntryPoint;			// 16-19
	  DWORD                BaseOfCode;					// 20-23
	  DWORD                BaseOfData;					// 24-27
	  DWORD                ImageBase;					// 28-31
	  DWORD                SectionAlignment;			// 32-35
	  DWORD                FileAlignment;				// 36-39
	  WORD                 MajorOperatingSystemVersion;	// 40-41
	  WORD                 MinorOperatingSystemVersion;	// 42-43
	  WORD                 MajorImageVersion;			// 44-45
	  WORD                 MinorImageVersion;			// 46-47
	  WORD                 MajorSubsystemVersion;		// 48-49
	  WORD                 MinorSubsystemVersion;		// 50-51
	  DWORD                Win32VersionValue;			// 52-55
	  DWORD                SizeOfImage;					// 56-59
	  DWORD                SizeOfHeaders;				// 60-63
	  DWORD                CheckSum;					// 64-67
	  WORD                 Subsystem;					// 68-69
	  WORD                 DllCharacteristics;			// 70-71
	  DWORD                SizeOfStackReserve;			// 72-75
	  DWORD                SizeOfStackCommit;			// 76-79
	  DWORD                SizeOfHeapReserve;			// 80-83
	  DWORD                SizeOfHeapCommit;			// 84-87
	  DWORD                LoaderFlags;					// 88-91
	  DWORD                NumberOfRvaAndSizes;			// 92-95
	  IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES]; // 96- [see (c)]
	} IMAGE_OPTIONAL_HEADER, *PIMAGE_OPTIONAL_HEADER;

	typedef struct _IMAGE_OPTIONAL_HEADER64 {
	  WORD                 Magic;						//   0-1 = 0x020B
	  BYTE                 MajorLinkerVersion;			//   2
	  BYTE                 MinorLinkerVersion;			//   3
	  DWORD                SizeOfCode;					//   4-7
	  DWORD                SizeOfInitializedData;		//   8-11
	  DWORD                SizeOfUninitializedData;		//  12-15
	  DWORD                AddressOfEntryPoint;			//  16-19
	  DWORD                BaseOfCode;					//  20-23
	  ULONGLONG            ImageBase;					//  24-31
	  DWORD                SectionAlignment;			//  32-35
	  DWORD                FileAlignment;				//  36-39
	  WORD                 MajorOperatingSystemVersion;	//  40-41
	  WORD                 MinorOperatingSystemVersion;	//  42-43
	  WORD                 MajorImageVersion;			//  44-45
	  WORD                 MinorImageVersion;			//  46-47
	  WORD                 MajorSubsystemVersion;		//  48-49
	  WORD                 MinorSubsystemVersion;		//  50-51
	  DWORD                Win32VersionValue;			//  52-55
	  DWORD                SizeOfImage;					//  56-59
	  DWORD                SizeOfHeaders;				//  60-63
	  DWORD                CheckSum;					//  64-67
	  WORD                 Subsystem;					//  68-69
	  WORD                 DllCharacteristics;			//  70-71
	  ULONGLONG            SizeOfStackReserve;			//  72-79
	  ULONGLONG            SizeOfStackCommit;			//  80-87
	  ULONGLONG            SizeOfHeapReserve;			//  88-95
	  ULONGLONG            SizeOfHeapCommit;			//  96-103
	  DWORD                LoaderFlags;					// 104-107
	  DWORD                NumberOfRvaAndSizes;			// 108-111
	  IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES]; // 112- [see (c)]
	} IMAGE_OPTIONAL_HEADER64, *PIMAGE_OPTIONAL_HEADER64;
	*/

    if (!fread(buf, 96, 1, fh)) {
        fprintf(stderr,
            "Unable to read %d bytes, @%ld.\n",
            96, ohdr_pos);
        perror(0);
        fclose(fh);
        return 1;
    }
	img_csize = *(unsigned long*)(buf+4);
	img_dsize = *(unsigned long*)(buf+8);
	uninit_datasize = *(unsigned long*)(buf+12);
	img_base = *(unsigned long*)(buf+28);
	fprintf(stdout, "ImageBase = 0x%08X\n", img_base);

	/*fprintf(stdout, "Linker = %d.%d\n", buf[2], buf[3]);
	fprintf(stdout, "SizeOfUninitializedData = 0x%p\n", *(unsigned long*)(buf+12));
	fprintf(stdout, "AddressOfEntryPoint = 0x%p\n", *(unsigned long*)(buf+16));
	fprintf(stdout, "ImageBase = 0x%p\n", *(unsigned long*)(buf+28));
	fprintf(stdout, "ImageVersion = %d.%d\n", *(unsigned short*)(buf+44), *(unsigned short*)(buf+46));*/
	fprintf(stdout, "CheckSum = 0x%08X\n", *(unsigned long*)(buf+64));
	fprintf(stdout, "OperatingSystemVersion = %d.%d\n", *(unsigned short*)(buf+40), *(unsigned short*)(buf+42));
	fprintf(stdout, "SubsystemVersion = %d.%d\n", *(unsigned short*)(buf+48), *(unsigned short*)(buf+50));
	fprintf(stdout, "Subsystem = 0x%04X\n", *(unsigned short*)(buf+68));

	oldchksum = *(unsigned long*)(buf+64);

	if(subsysflag) {
		hdr_changed = 1;
		bin_changed = 1;
		buf[68] = subsysflag & 0xff;
		buf[69] = (subsysflag >> 8) & 0xff;
		/*buf[70] = (subsysflag >> 16) & 0xff;
		buf[71] = (subsysflag >> 24) & 0xff;*/

		fprintf(stdout, "new Subsystem = 0x%04X\n", subsysflag);
	}
	if(osver_set) {
		hdr_changed = 1;
		bin_changed = 1;
		buf[42] = osver & 0xff;
		buf[43] = (osver >> 8) & 0xff;
		buf[40] = (osver >> 16) & 0xff;
		buf[41] = (osver >> 24) & 0xff;

		fprintf(stdout, "new OperatingSystemVersion = 0x%08X\n", osver);
	}
	if(subsysver_set) {
		hdr_changed = 1;
		bin_changed = 1;
		buf[50] = subsysver & 0xff;
		buf[51] = (subsysver >> 8) & 0xff;
		buf[48] = (subsysver >> 16) & 0xff;
		buf[49] = (subsysver >> 24) & 0xff;

		fprintf(stdout, "new SubsystemVersion = 0x%08X\n", subsysver);
	}

	if(hdr_changed) {
		fseek(fh, ohdr_pos,SEEK_SET);
		fwrite(buf, 1, 96, fh);
		fseek(fh, ohdr_pos+96,SEEK_SET);
		hdr_changed = 0;
	}

	ohdr_pos = ftell(fh);
	/* (c)
	#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES    16
	typedef struct _IMAGE_DATA_DIRECTORY {
	    DWORD   VirtualAddress;	// 0-3
	    DWORD   Size;			// 4-7
	} IMAGE_DATA_DIRECTORY, *PIMAGE_DATA_DIRECTORY;
	// = 16 * 8 = 128 bytes
	*/
    if (!fread(buf, 128, 1, fh)) {
        fprintf(stderr,
            "Unable to read %d bytes, @%ld.\n",
            128, ohdr_pos);
        perror(0);
        fclose(fh);
        return 1;
    }

	for(i=0; i< 16; i++) {
		fprintf(stdout, " Dict[%i] VirtualAddress = %08X, Size = %d\n", i, *(unsigned long *)(buf+(8*i)), *(unsigned long *)(buf+(8*i)+4));
	}

	export_va = *(unsigned long *)(buf);
	export_vsize = *(unsigned long *)(buf+4);
	import_va = *(unsigned long *)(buf+8);
	import_vsize = *(unsigned long *)(buf+12);
	delay_import_va = *(unsigned long *)(buf+104);
	delay_import_vsize = *(unsigned long *)(buf+108);

	/* (d)
	#define IMAGE_SIZEOF_SHORT_NAME 8
	typedef struct _IMAGE_SECTION_HEADER {
	    BYTE    Name[IMAGE_SIZEOF_SHORT_NAME];	//  0-7
	    union {
	            DWORD   PhysicalAddress;
	            DWORD   VirtualSize;
	    } Misc;									//  8-11
	    DWORD   VirtualAddress;					// 12-15
	    DWORD   SizeOfRawData;					// 16-19
	    DWORD   PointerToRawData;				// 20-23
	    DWORD   PointerToRelocations;			// 24-27
	    DWORD   PointerToLinenumbers;			// 28-31
	    WORD    NumberOfRelocations;			// 32-33
	    WORD    NumberOfLinenumbers;			// 34-35
	    DWORD   Characteristics;				// 36-39
	} IMAGE_SECTION_HEADER, *PIMAGE_SECTION_HEADER;
	*/
	for(i=0; i< num_sects; i++) {
		ohdr_pos = ftell(fh);
	    if (!fread(buf, 40, 1, fh)) {
	        fprintf(stderr,
	            "Unable to read %d bytes, @%ld.\n",
	            40, ohdr_pos);
	        perror(0);
	        fclose(fh);
	        return 1;
	    }
		fprintf(stdout, "Header '%s' @ 0x%08x\n", (unsigned char*)(buf), ohdr_pos);
		sec_pa = *(unsigned long *)(buf+8);
		sec_va = *(unsigned long *)(buf+12);
		sec_prd = *(unsigned long *)(buf+20);

		fprintf(stdout, "   PhysicalAddress = 0x%08x\n", sec_pa);
		fprintf(stdout, "   VirtualAddress = 0x%08x\n", sec_va);
		fprintf(stdout, "   PointerToRawData = 0x%08x\n", sec_prd);

		fprintf(stdout, "    sec_va=%08x, export_va=%08x, %d\n",sec_va,export_va, (sec_va <= export_va));
		if ((sec_va <= export_va) &&
			((sec_va + sec_pa) > export_va)) {
			export_ptr = export_va - sec_va + sec_prd;
			fprintf(stdout, " ==> ExportPointerToRawData = %08x\n", export_ptr);
		}
		fprintf(stdout, "    sec_va=%08x, import_va=%08x, %d\n",sec_va,import_va, (sec_va <= import_va));
		if ((sec_va <= import_va) &&
			((sec_va + sec_pa) > import_va)) {
			import_ptr = import_va - sec_va + sec_prd;
			fprintf(stdout, " ==> ImportPointerToRawData = %08x\n", import_ptr);
		}
		fprintf(stdout, "    sec_va=%08x, delay_import_va=%08x, %d\n",sec_va,delay_import_va, (sec_va <= delay_import_va));
		if ((sec_va <= delay_import_va) &&
			((sec_va + sec_pa) > delay_import_va)) {
			delay_import_ptr = delay_import_va - sec_va + sec_prd;
			fprintf(stdout, " ==> DelayImportPointerToRawData = %08x\n", delay_import_ptr);
		}

		if(strncmp(".bss",(unsigned char*)(buf),8)==0 && bssflag) {
			fprintf(stdout, "Found BSS section!\n");
			fprintf(stdout, "Old SizeOfRawData = %08X\n", *(unsigned long *)(buf+16));
			*(unsigned long *)(buf+16) = uninit_datasize;
			fprintf(stdout, "New SizeOfRawData = %08X\n", *(unsigned long *)(buf+16));
			hdr_changed = 1;
			bin_changed = 1;
		}
		if(hdr_changed) {
			fseek(fh, ohdr_pos,SEEK_SET);
			fwrite(buf, 1, 40, fh);
			hdr_changed = 0;
			fseek(fh, ohdr_pos+40,SEEK_SET);
		}
	}

if(export_ptr) {
	fseek(fh, export_ptr,SEEK_SET);
    if (!fread(buf, 40, 1, fh)) {
        fprintf(stderr,
            "Unable to read %d bytes, @%ld.\n",
            40, export_ptr);
        perror(0);
        fclose(fh);
        return 1;
    }
/*
	typedef	struct	tagPE_EXPORT_TABLE
	{
		DWORD		Characteristics;		//  0-3
		DWORD		TimeDateStamp;			//  4-7
		WORD		MajorVersion;			//  8-9
		WORD		MinorVersion;			// 10-11
		DWORD		Name;					// 12-15
		DWORD		Base;					// 16-19
		DWORD		NumberOfFunctions;		// 20-23
		DWORD		NumberOfNames;			// 24-27
		DWORD		AddressOfFunctions;		// 28-31
		DWORD		AddressOfNames;			// 32-35
		DWORD		AddressOfOrdinals;		// 36-39
	} PE_EXPORT_TABLE;
*/
	export_names_ptr=*(unsigned long *)(buf+12);
	export_names_addr_fn=*(unsigned long *)(buf+28);
	export_names_addr_name=*(unsigned long *)(buf+32);
	export_names_addr_odr=*(unsigned long *)(buf+36);

/*
	fprintf(stdout, "Exports\n    Characteristics = %08x\n    TimeDateStamp=%08x\n    MajorVersion=%04x\n    MinorVersion=%04x\n    Name=%08x\n    Base=%08x\n    NumberOfFunctions=%08x\n    NumberOfNames=%08x\n    AddressOfFunctions=%08x\n    AddressOfNames=%08x\n    AddressOfOrdinals=%08x\n\n",
		*(unsigned long *)(buf),
		*(unsigned long *)(buf+4),
		*(unsigned short *)(buf+8),
		*(unsigned short *)(buf+10),
		export_names_ptr,
		*(unsigned long *)(buf+16),
		*(unsigned long *)(buf+20),
		*(unsigned long *)(buf+24),
		export_names_addr_fn,
		export_names_addr_name,
		export_names_addr_odr
	);
*/
	export_names_ptr_pa = export_names_ptr - export_va + export_ptr;
	export_names_addr_fn_pa = export_names_addr_fn - export_va + export_ptr;
	export_names_addr_name_pa = export_names_addr_name - export_va + export_ptr;
	export_names_addr_odr_pa = export_names_addr_odr - export_va + export_ptr;

	//fprintf(stdout, "   export_names_addr_name_pa=%08x, export_names_addr_name=%08x, %d\n",export_names_addr_name_pa,export_names_addr_name, (export_va <= export_names_addr_name) && ((sec_va + sec_prd) > export_names_addr_name));


}

if(import_ptr) {
	fseek(fh, import_ptr,SEEK_SET);
    if (!fread(buf, 20, 1, fh)) {
        fprintf(stderr,
            "Unable to read %d bytes, @%ld.\n",
            20, import_ptr);
        perror(0);
        fclose(fh);
        return 1;
    }

    import_fnl = *(unsigned long *)(buf);
    import_mnames = *(unsigned long *)(buf+12);
    import_fal = *(unsigned long *)(buf+16);

/*
	typedef	struct	tagPE_IMPORT_TABLE
	{
		DWORD		FunctionNameList;		//  0-3
		DWORD		TimeDateStamp;			//  4-7
		DWORD		ForwarderChain;			//  8-11
		DWORD		ModuleName;				// 12-15
		DWORD		FunctionAddressList;	// 16-19
	} PE_IMPORT_TABLE;
*/

    import_fnl_pa = import_fnl - import_va + import_ptr;
    import_mnames_pa = import_mnames - import_va + import_ptr;
    import_fal_pa = import_fal - import_va + import_ptr;

	fprintf(stdout, "Imports\n    FunctionNameList=%08x\n    TimeDateStamp=%08x\n    ForwarderChain=%08x\n    ModuleName=%08x\n    FunctionAddressList=%08x\n",
		import_fnl,
		*(unsigned long *)(buf+4),
		*(unsigned long *)(buf+8),
		import_mnames,
		import_fal
	);

	fprintf(stdout, "   import_fnl_pa=%08x, import_fnl=%08x, %d\n",import_fnl_pa,import_fnl, (import_va <= import_fnl) && ((sec_va + sec_prd) > import_fnl));
	fprintf(stdout, "   import_mnames_pa=%08x, import_mnames=%08x, %d\n",import_mnames_pa,import_mnames, (import_va <= import_mnames) && ((sec_va + sec_prd) > import_mnames));
	fprintf(stdout, "   import_fal_pa=%08x, import_fal=%08x, %d\n",import_fal_pa,import_fal, /*(import_va <= import_fal) &&*/ ((sec_va + sec_prd) > import_fal));

/*
	typedef	struct	tagPE_EXPORT_IMPORT_INFO
	{
		DWORD		ExportVirtualAddress;	//  0-3
		DWORD		ExportPointerToRawData;	//  4-7
		DWORD		ImportVirtualAddress;	//  8-11
		DWORD		ImportPointerToRawData;	// 12-15
	} PE_EXPORT_IMPORT_INFO;

	typedef	struct	tagPE_EXPORT_IMPORT_INFO64
	{
		ULONGLONG		ExportVirtualAddress;	//  0-7
		ULONGLONG		ExportPointerToRawData;	//  8-15
		ULONGLONG		ImportVirtualAddress;	// 16-23
		ULONGLONG		ImportPointerToRawData;	// 24-31
	} PE_EXPORT_IMPORT_INFO64; */

	fseek(fh, import_fal_pa,SEEK_SET);
    if (!fread(buf, 16, 1, fh)) {
        fprintf(stderr,
            "Unable to read %d bytes, @%ld.\n",
            16, import_fal_pa);
        perror(0);
        fclose(fh);
        return 1;
    }

    import_fn_va = *(unsigned long *)(buf);

	fprintf(stdout, "Import Info\n    ExportVirtualAddress = %08x\n    ExportPointerToRawData=%08x\n    ImportVirtualAddress=%08x\n    ImportPointerToRawData=%08x\n\n",
		import_fn_va,
		*(unsigned long *)(buf+4),
		*(unsigned long *)(buf+8),
		*(unsigned long *)(buf+12)
	);
    import_fn_pa = import_fn_va - import_va + import_ptr;
	fprintf(stdout, "   import_fn_pa=%08x, import_fn_va=%08x, %d\n",import_fn_pa,import_fn_va, (import_va <= import_fn_va) && ((sec_va + sec_prd) > import_fn_va));
}

if(delay_import_ptr) {
	fseek(fh, delay_import_ptr,SEEK_SET);
    if (!fread(buf, 32, 1, fh)) {
        fprintf(stderr,
            "Unable to read %d bytes, @%ld.\n",
            32, delay_import_ptr);
        perror(0);
        fclose(fh);
        return 1;
    }
/*
typedef	struct	tagPE_DELAY_IMPORT_TABLE
{
	DWORD		Attributes;					//  0-3
	DWORD		ModuleName;					//  4-7
	DWORD		Module;						//  8-11
	DWORD		FunctionAddressList;		// 12-15
	DWORD		FunctionNameList;			// 16-19
	DWORD		BoundImportAddressTable;	// 20-23
	DWORD		UnloadImportAddressTable;	// 24-27
	DWORD		TimeDateStamp;				// 28-31
} PE_DELAY_IMPORT_TABLE;
*/
    delay_import_fnl = *(unsigned long *)(buf+16);
    delay_import_mnames = *(unsigned long *)(buf+4);

	fprintf(stdout, "Delay Imports\n    Attributes = %08x\n    ModuleName=%08x\n    Module=%08x\n    FunctionAddressList=%08x\n    FunctionNameList=%08x\n    BoundImportAddressTable=%08x\n    UnloadImportAddressTable=%08x\n    TimeDateStamp=%08x\n",
		*(unsigned long *)(buf),
		delay_import_mnames,
		*(unsigned long *)(buf+8),
		*(unsigned long *)(buf+12),
		delay_import_fnl,
		*(unsigned long *)(buf+20),
		*(unsigned long *)(buf+24),
		*(unsigned long *)(buf+28)
	);

    if (delay_import_fnl > img_base) delay_import_fnl-= img_base;
    if (delay_import_mnames > img_base) delay_import_mnames-= img_base;

    delay_import_fnl_pa = delay_import_fnl - delay_import_va + delay_import_ptr;
    delay_import_mnames_pa = delay_import_mnames - delay_import_va + delay_import_ptr;

	fprintf(stdout, "   delay_import_fnl_pa=%08x, delay_import_fnl=%08x, %d\n",delay_import_fnl_pa,delay_import_fnl, (delay_import_va <= delay_import_fnl) && ((sec_va + sec_prd) > delay_import_fnl));
	fprintf(stdout, "   delay_import_mnames_pa=%08x, delay_import_mnames=%08x, %d\n",delay_import_mnames_pa,delay_import_mnames, /*(delay_import_va <= delay_import_mnames) &&*/ ((sec_va + sec_prd) > delay_import_mnames));
}


	/* recalculate PE checksum */
	if(bin_changed) {
	    fseek(fh, 0, SEEK_END);
	    bin_size = (int) ftell(fh) + 1;
	    fseek(fh, 0, SEEK_SET);
		csbuf = (char*) calloc(bin_size,1);
		fread(csbuf, 1, bin_size, fh);

		*(unsigned long*)(csbuf+ohdr_pos+64) = 0; // clear old checksum first
		for(i=0;i < bin_size; i+=2) {
			newchksum += *(unsigned short*)(csbuf+i);
			newchksum = 0xffff & (newchksum + (newchksum >> 16));
		}
		*(unsigned long*)(buf+64) = newchksum + bin_size - 1; // need to add file length
		free(csbuf);
	fprintf(stdout, "New CheckSum = 0x%08X\n", *(unsigned long*)(buf+64));
		fseek(fh, ohdr_pos,SEEK_SET);
		fwrite(buf, 1, 96, fh);
	}

    fclose(fh);
    return 0;
}
