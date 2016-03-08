
#if defined(SUPPORT_LGY98)

typedef void IOReadHandler(void *opaque, const UINT8 *buf, int size);
typedef int IOCanRWHandler(void *opaque);
//typedef ssize_t (IOReadvHandler)(void *, const struct iovec *, int);
typedef void (NetCleanup) (struct tagVLANClientState *);
typedef void (LinkStatusChanged)(struct tagVLANClientState *);

struct tagVLANState {
    int id;
    struct tagVLANClientState *first_client;
    struct tagVLANState *next;
    unsigned int nb_guest_devs, nb_host_devs;
};

struct tagVLANClientState{
    IOReadHandler *fd_read;
    //LGY98_IOReadvHandler *fd_readv;
    /* Packets may still be sent if this returns zero.  It's used to
       rate-limit the slirp code.  */
    IOCanRWHandler *fd_can_read;
    NetCleanup *cleanup;
    LinkStatusChanged *link_status_changed;
    int link_down;
    void *opaque;
    struct tagVLANClientState *next;
    struct tagVLANState *vlan;
    char *model;
    char *name;
    char info_str[256];
};

typedef struct tagVLANState VLANState;
typedef struct tagVLANClientState VLANClientState;

void np2net_setVC(VLANClientState *vc1);
void np2net_init();
int np2net_reset(CHAR* tapname);
void np2net_shutdown();
void np2net_getmacaddr(REG8 *macaddr);
void np2net_suspend();
void np2net_resume();

void np2net_send_packet(VLANClientState *vc1, const UINT8 *buf, int size);

#endif	/* SUPPORT_LGY98 */