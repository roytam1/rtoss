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
    case 0x01F0: fprintf(stdout, "Machine: PowerPC\n"); break;
    case 0x0200: fprintf(stdout, "Machine: IA64 (Intel Itanium)\n"); break;
    case 0x8664: fprintf(stdout, "Machine: x64 (AMD64)\n"); break;
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
	*/

    if (!fread(buf, 96, 1, fh)) {
        fprintf(stderr,
            "Unable to read %d bytes, @%ld.\n",
            96, ohdr_pos);
        perror(0);
        fclose(fh);
        return 1;
    }
	uninit_datasize = *(unsigned long*)(buf+12);
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
