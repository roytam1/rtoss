/* gmbxwc_dynload.h - GetProcAddress typedefs for gmbxwc.dll (C89).
 *
 * Explicit run-time linking: LoadLibrary the DLL, fill GmbxwcApi with
 * GmbxwcApi_Load, then call through the table. Export names are the plain
 * C function names (no decoration). All functions use the default C
 * calling convention (__cdecl under default compiler flags).
 *
 * Sample:
 *
 *   #include "gmbxwc_dynload.h"
 *   {
 *       HMODULE hDll = LoadLibraryA("gmbxwc.dll");
 *       GmbxwcApi api;
 *       CodePageContext* ctx;
 *       if (hDll != NULL && GmbxwcApi_Load(hDll, &api)) {
 *           ctx = api.CreateConverter(0);
 *           ...
 *           api.FreeCodePageConverter(ctx);
 *       }
 *       if (hDll != NULL) FreeLibrary(hDll);
 *   }
 *
 * Free every context before FreeLibrary: contexts point into DLL resources.
 */
#ifndef _GMBXWC_DYNLOAD_H_
#define _GMBXWC_DYNLOAD_H_

#include "gmbxwc.h"

/* The DLL boundary uses __cdecl. Spell it out: some consumers
 * (e.g. GreenPad VS projects defaulting to FastCall) do not use
 * a cdecl default, and an implicit convention would corrupt the
 * stack on x86. Same reason chardet pointers spell __cdecl out.
 * Harmless where __cdecl is already the default, and ignored on
 * x64/ARM64. Override with -DGMBXWC_CALL=... if ever needed. */
#ifndef GMBXWC_CALL
#define GMBXWC_CALL __cdecl
#endif

typedef CodePageContext* (GMBXWC_CALL *PFN_InitCodePageConverter)(const unsigned char* blob_data);
typedef void (GMBXWC_CALL *PFN_FreeCodePageConverter)(CodePageContext* ctx);
typedef unsigned long (GMBXWC_CALL *PFN_CodePage_MB2WC)(const CodePageContext* ctx,
                      const unsigned char* src, unsigned long src_len,
                      wchar_t* dest, unsigned long dest_max, BOOL* lpbUnmapped);
typedef unsigned long (GMBXWC_CALL *PFN_CodePage_WC2MB)(const CodePageContext* ctx,
                      const wchar_t* src, unsigned long src_len,
                      unsigned char* dest, unsigned long dest_max, BOOL* lpbUnmapped);
typedef unsigned long (GMBXWC_CALL *PFN_CodePage_EmbeddedCount)(void);
typedef BOOL (GMBXWC_CALL *PFN_CodePage_EmbeddedInfo)(unsigned long index, EmbeddedTableInfo* p_info);
typedef BOOL (GMBXWC_CALL *PFN_CodePage_FindIndexForCodePage)(unsigned long code_page, unsigned long* p_index);
typedef CodePageContext* (GMBXWC_CALL *PFN_CodePage_CreateConverter)(unsigned long index);
typedef unsigned long (GMBXWC_CALL *PFN_CodePage_IndexedMB2WC)(unsigned long index,
                      const unsigned char* src, unsigned long src_len,
                      wchar_t* dest, unsigned long dest_max, BOOL* lpbUnmapped);
typedef unsigned long (GMBXWC_CALL *PFN_CodePage_IndexedWC2MB)(unsigned long index,
                      const wchar_t* src, unsigned long src_len,
                      unsigned char* dest, unsigned long dest_max, BOOL* lpbUnmapped);

/* Resolved entry-point table; fill with GmbxwcApi_Load. */
typedef struct {
    PFN_InitCodePageConverter InitCodePageConverter;
    PFN_FreeCodePageConverter FreeCodePageConverter;
    PFN_CodePage_MB2WC MB2WC;
    PFN_CodePage_WC2MB WC2MB;
    PFN_CodePage_EmbeddedCount EmbeddedCount;
    PFN_CodePage_EmbeddedInfo EmbeddedInfo;
    PFN_CodePage_FindIndexForCodePage FindIndexForCodePage;
    PFN_CodePage_CreateConverter CreateConverter;
    PFN_CodePage_IndexedMB2WC IndexedMB2WC;
    PFN_CodePage_IndexedWC2MB IndexedWC2MB;
} GmbxwcApi;

/* Resolve all 10 exports. Returns TRUE on success, FALSE if the module
   handle is bad, pApi is NULL, or any export is missing (pApi untouched). */
static BOOL GmbxwcApi_Load(HMODULE hDll, GmbxwcApi* pApi) {
    GmbxwcApi api;
    if (hDll == NULL || pApi == NULL) {
        return FALSE;
    }
    api.InitCodePageConverter =
        (PFN_InitCodePageConverter)GetProcAddress(hDll, "InitCodePageConverter");
    api.FreeCodePageConverter =
        (PFN_FreeCodePageConverter)GetProcAddress(hDll, "FreeCodePageConverter");
    api.MB2WC =
        (PFN_CodePage_MB2WC)GetProcAddress(hDll, "CodePage_MB2WC");
    api.WC2MB =
        (PFN_CodePage_WC2MB)GetProcAddress(hDll, "CodePage_WC2MB");
    api.EmbeddedCount =
        (PFN_CodePage_EmbeddedCount)GetProcAddress(hDll, "CodePage_EmbeddedCount");
    api.EmbeddedInfo =
        (PFN_CodePage_EmbeddedInfo)GetProcAddress(hDll, "CodePage_EmbeddedInfo");
    api.FindIndexForCodePage =
        (PFN_CodePage_FindIndexForCodePage)GetProcAddress(hDll, "CodePage_FindIndexForCodePage");
    api.CreateConverter =
        (PFN_CodePage_CreateConverter)GetProcAddress(hDll, "CodePage_CreateConverter");
    api.IndexedMB2WC =
        (PFN_CodePage_IndexedMB2WC)GetProcAddress(hDll, "CodePage_IndexedMB2WC");
    api.IndexedWC2MB =
        (PFN_CodePage_IndexedWC2MB)GetProcAddress(hDll, "CodePage_IndexedWC2MB");
    if (api.InitCodePageConverter == NULL
        || api.FreeCodePageConverter == NULL
        || api.MB2WC == NULL
        || api.WC2MB == NULL
        || api.EmbeddedCount == NULL
        || api.EmbeddedInfo == NULL
        || api.FindIndexForCodePage == NULL
        || api.CreateConverter == NULL
        || api.IndexedMB2WC == NULL
        || api.IndexedWC2MB == NULL) {
        return FALSE;
    }
    *pApi = api;
    return TRUE;
}

#endif /* _GMBXWC_DYNLOAD_H_ */
