#ifndef _GMBXWC_H_
#define _GMBXWC_H_

#include <stdlib.h>
#include <windows.h>

/* Struct layouts for the raw binary blobs */
typedef struct {
    unsigned char min_trail;
    unsigned char max_trail;
    unsigned char action_type; /* 0 = Leaf Trail, 1 = Leaf Single, 2 = Intermediate Node */
    unsigned char reserved;
    unsigned long pool_offset; /* If action_type == 2, this is the NEXT window index! */
} ResourceTrailWindow;

typedef struct {
    unsigned long unicode;
    unsigned short mb_val;
} ExtBMapping;

typedef struct {
    unsigned long start_index;   /* GB18030 linear calculation index start */
    unsigned long end_index;     /* GB18030 linear calculation index end */
    unsigned long start_unicode; /* Corresponding starting Unicode codepoint */
} GB18030Range;

/* Unified Program Header Mapping for Binary Blobs */
typedef struct {
    unsigned long magic;         /* 'CPBL', 'C219', or 'GB18' */
    unsigned long code_page;
    unsigned long off_windows;
    unsigned long off_pool;
    unsigned long off_dir;
    unsigned long off_pages;
    unsigned long off_extra;     /* Points to ExtB table OR GB18030 range table */
    unsigned long extra_count;   /* Holds count for extra tracking entries */
    unsigned long is_32bit_pool;
    unsigned long def_char;   /* NEW: Code Page specified default char for unmapped */
} CodePageHeader;

/* Unified Context Structure */
typedef struct {
    unsigned long magic;
    unsigned long code_page;
    int is_stateful_ebcdic;
    int is_gb18030;
    int is_32bit_pool;
    int is_valid;

    const ResourceTrailWindow* trail_windows;
    const unsigned short* pool16;
    const unsigned long* pool32;
    const unsigned short* wchar_directory;
    const unsigned short* wchar_page_pool;
    unsigned long def_char; /* NEW: Code Page specified default char for unmapped */
    
    /* Extension B Mapping context */
    const ExtBMapping* ext_b_table;
    unsigned long ext_b_count;

    /* GB18030 Range Context */
    const GB18030Range* gb_ranges;
    unsigned long gb_range_count;

    /* Table entry arrays */
    const unsigned short* dbcs_lead_table;
    const unsigned short* sbcs_table;
    const unsigned short* dbcs_first_byte_table;
} CodePageContext;

#define EBCDIC_MODE_SBCS 0
#define EBCDIC_MODE_DBCS 1

/* DLL import/export linkage (empty for static builds) */
#if defined(GMBXWC_BUILD_DLL)
#define GMBXWC_API __declspec(dllexport)
#elif defined(GMBXWC_USE_DLL)
#define GMBXWC_API __declspec(dllimport)
#else
#define GMBXWC_API
#endif

/* Unified Conversion Functions */
GMBXWC_API CodePageContext* InitCodePageConverter(const unsigned char* blob_data);
GMBXWC_API void FreeCodePageConverter(CodePageContext* ctx);
GMBXWC_API unsigned long CodePage_MB2WC(const CodePageContext* ctx, 
                      const unsigned char* src, unsigned long src_len, 
                      wchar_t* dest, unsigned long dest_max, BOOL* lpbUnmapped);

GMBXWC_API unsigned long CodePage_WC2MB(const CodePageContext* ctx, 
                      const wchar_t* src, unsigned long src_len, 
                      unsigned char* dest, unsigned long dest_max, BOOL* lpbUnmapped);

/* Embedded-table inventory (gmbxwc.dll resources, dense 0-based index).
   The index IS the integer identifier: pass it to CodePage_CreateConverter,
   CodePage_IndexedMB2WC, or CodePage_IndexedWC2MB. */
typedef struct {
    unsigned long code_page;   /* Windows codepage ID, e.g. 932, 54936, 21937, 20000 */
    const char* blob_name;     /* CPBL filename baked into the DLL, e.g. "CP932.DAT" */
    const char* display_name;  /* Human-readable label, e.g. "Shift-JIS (CP932)" */
} EmbeddedTableInfo;

GMBXWC_API unsigned long CodePage_EmbeddedCount(void);
GMBXWC_API BOOL CodePage_EmbeddedInfo(unsigned long index, EmbeddedTableInfo* p_info);
GMBXWC_API BOOL CodePage_FindIndexForCodePage(unsigned long code_page, unsigned long* p_index);
GMBXWC_API CodePageContext* CodePage_CreateConverter(unsigned long index);
GMBXWC_API unsigned long CodePage_IndexedMB2WC(unsigned long index,
                      const unsigned char* src, unsigned long src_len,
                      wchar_t* dest, unsigned long dest_max, BOOL* lpbUnmapped);
GMBXWC_API unsigned long CodePage_IndexedWC2MB(unsigned long index,
                      const wchar_t* src, unsigned long src_len,
                      unsigned char* dest, unsigned long dest_max, BOOL* lpbUnmapped);

#if 0
/* sample code for using library */
void RenderFilename(const CodePageContext* ctx, const unsigned char* raw_mb_str, size_t len) {
    wchar_t* w_buffer;
    size_t required_chars;

    /* Step 1: Query necessary character allocation length */
    required_chars = CodePage_MB2WC(ctx, raw_mb_str, len, NULL, 0);
    
    if (required_chars == 0) return;

    /* Step 2: Allocate safely on local heap (Win32 traditional style) */
    w_buffer = (wchar_t*)LocalAlloc(LMEM_FIXED, (required_chars + 1) * sizeof(wchar_t));
    if (!w_buffer) return;

    /* Step 3: Run safe actual transformation */
    CodePage_MB2WC(ctx, raw_mb_str, len, w_buffer, required_chars);
    w_buffer[required_chars] = L'\0'; /* Force null-terminator */

    /* Step 4: Draw to Win32 GDI Canvas */
    TextOutW(hdc, 10, 10, w_buffer, required_chars);

    LocalFree(w_buffer);
}

#include "cp932_embedded.h" /* Contains embedded 'cp932_blob_data' array */

/* Application Global or Subsystem State variables */
CodePageContext g_CtxShiftJIS;   /* Embedded Engine Context */
CodePageContext g_CtxEBCDIC;     /* External Engine Context */
unsigned char* g_EbcdicBlob = NULL; /* Alloc tracking pointer for external data */

/* Call this during your Image Viewer initialization phase */
void InitApplicationEncodings(void) {
    /* 1. INITIALIZE EMBEDDED TABLES */
    /* Point straight to the array compiled directly into the binary image */
    g_CtxShiftJIS = InitCodePageConverter(cp932_blob_data);

    /* 2. INITIALIZE EXTERNAL TABLES */
    /* Attempt to pull heavy data from a sidecar file next to the EXE */
    g_EbcdicBlob = LoadExternalCodePageBlob("CP933.DAT");
    if (g_EbcdicBlob != NULL) {
        g_CtxEBCDIC = InitCodePageConverter(g_EbcdicBlob);
    } else {
        /* Handle graceful fallback or mark the context invalid */
        g_CtxEBCDIC.is_valid = 0;
    }
}

/* Call this when the image viewer window is closed or exiting */
void CleanupApplicationEncodings(void) {
    /* Embedded contexts require zero teardown (they live in read-only program space) */
    
    /* External tables must explicitly release their allocated heap footprint */
    if (g_EbcdicBlob != NULL) {
        FreeExternalCodePageBlob(g_EbcdicBlob);
        g_EbcdicBlob = NULL;
    }
}
#endif
#endif /* _GMBXWC_H_ */
