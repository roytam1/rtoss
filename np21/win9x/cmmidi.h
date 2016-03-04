
// ---- com manager midi for windows

#ifdef __cplusplus
extern "C" {
#endif

extern const TCHAR cmmidi_midimapper[];
#if defined(VERMOUTH_LIB)
extern const TCHAR cmmidi_vermouth[];
#endif
#if defined(MT32SOUND_DLL)
extern const TCHAR cmmidi_mt32sound[];
#endif
extern LPCTSTR cmmidi_mdlname[12];

void cmmidi_initailize(void);
COMMNG cmmidi_create(LPCTSTR midiout, LPCTSTR midiin, LPCTSTR module);
void cmmidi_recvdata(HMIDIIN hdr, UINT32 data);
void cmmidi_recvexcv(HMIDIIN hdr, MIDIHDR *data);

#ifdef __cplusplus
}
#endif

