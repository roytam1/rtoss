/* apngfix.c - standalone APNG repair / normalize tool.
 *
 * Fixes structurally defective APNG files that strict parsers (such as
 * stb_image's APNG extension, browsers, or picview) reject or miscount:
 *
 *   - acTL num_frames != actual fcTL count  -> rewritten to match
 *   - fcTL chunks but missing acTL          -> acTL inserted (plays=0)
 *   - acTL but zero fcTL chunks             -> acTL dropped (plain PNG)
 *   - duplicate acTL chunks                 -> extras dropped
 *   - acTL placed after IDAT                -> moved before first IDAT
 *   - broken fcTL/fdAT sequence numbers     -> resequenced from 0
 *   - bad chunk CRCs                         -> recomputed (data kept)
 *   - trailing garbage after IEND           -> stripped
 *   - missing IEND                          -> appended
 *
 * Unrepairable (fatal, no output written):
 *   - bad PNG signature, truncated chunk, fdAT before any fcTL,
 *     IDAT interleaved after fdAT, malformed fcTL length.
 *
 * Frame pixel data is NEVER recompressed or altered; only chunk headers
 * (acTL counts, sequence numbers), chunk ordering (acTL move), chunk
 * set (insert/drop acTL, IEND, garbage) and CRCs change.
 *
 * Usage: apngfix [-s|--still] input.apng output.apng
 * Exit: 0 = output written (repaired or already clean),
 *       1 = fatal error (nothing written).
 *
 * Still mode (-s): convert a SINGLE-frame APNG to a plain PNG showing
 * frame 0 (what viewers display). The fcTL-first layout keeps its IDAT
 * data; the hidden-default layout converts frame 0's fdAT data to IDAT
 * and drops the unseen default image. Refuses multi-frame files, plain
 * PNGs, and sub-rectangle frame 0 (cannot flatten without compositing).
 * Pixel data is copied verbatim, never recompressed.
 *
 * Standalone C89, libc only. No zlib needed (payloads copied verbatim).
 *
 * Compile (MinGW): gcc -O2 -o apngfix.exe apngfix.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define APNGFIX_MAX_FILE (256UL * 1024UL * 1024UL)

/* ---- CRC32 (zlib polynomial 0xEDB88320, table driven) ---- */
static unsigned long crc_tab[256];
static int crc_ready = 0;

static void crc_init(void)
{
    unsigned long c;
    int n, k;
    for (n = 0; n < 256; n++) {
        c = (unsigned long)n;
        for (k = 0; k < 8; k++)
            c = (c & 1) ? (0xEDB88320UL ^ (c >> 1)) : (c >> 1);
        crc_tab[n] = c;
    }
    crc_ready = 1;
}

static unsigned long crc_update(unsigned long crc, unsigned char *buf, size_t len)
{
    size_t n;
    if (!crc_ready) crc_init();
    crc ^= 0xFFFFFFFFUL;
    for (n = 0; n < len; n++)
        crc = crc_tab[(crc ^ buf[n]) & 0xFF] ^ (crc >> 8);
    return crc ^ 0xFFFFFFFFUL;
}

/* ---- big-endian helpers ---- */
static unsigned long rd32(unsigned char *p)
{
    return ((unsigned long)p[0] << 24) | ((unsigned long)p[1] << 16) |
           ((unsigned long)p[2] << 8) | (unsigned long)p[3];
}

static void wr32(unsigned char *p, unsigned long v)
{
    p[0] = (unsigned char)(v >> 24);
    p[1] = (unsigned char)(v >> 16);
    p[2] = (unsigned char)(v >> 8);
    p[3] = (unsigned char)v;
}

static unsigned long fourcc(unsigned char *p)
{
    return rd32(p);
}

#define FCC(a,b,c,d) (((unsigned long)(a) << 24) | ((unsigned long)(b) << 16) | \
                      ((unsigned long)(c) << 8) | (unsigned long)(d))

/* ---- chunk model ---- */
typedef struct {
    unsigned long type;
    unsigned long len;
    unsigned char *data; /* malloc'd payload (len bytes, may be NULL if len==0) */
    int keep;            /* 1 = emit to output */
    long file_off;
} Chunk;

static void free_chunks(Chunk *ch, size_t n)
{
    size_t i;
    for (i = 0; i < n; i++)
        free(ch[i].data);
    free(ch);
}

int main(int argc, char **argv)
{
    const char *inpath, *outpath;
    FILE *f;
    long fsize;
    unsigned char *filebuf;
    size_t nch, cap, i;
    Chunk *ch;
    size_t pos;
    int repairs = 0;
    int fatal = 0;
    /* classification */
    size_t ihdr_at = 0, actl_at = 0, iend_at = 0, first_idat_at = 0;
    int have_ihdr = 0, have_actl = 0, have_iend = 0;
    size_t fctl_count = 0, fdat_count = 0, idat_count = 0;
    int seen_idat = 0, seen_fdat = 0;
    unsigned long actl_frames = 0;
    unsigned long canvas_w = 0, canvas_h = 0;
    unsigned long f0_w = 0, f0_h = 0, f0_x = 0, f0_y = 0;
    int fctl_before_idat = 0;
    int want_still = 0, converted = 0, frame0_in_idat = 0;
    FILE *out;
    static const unsigned char sig[8] = {137,80,78,71,13,10,26,10};

    if (argc == 4 && (strcmp(argv[1], "-s") == 0 || strcmp(argv[1], "--still") == 0)) {
        want_still = 1;
        inpath = argv[2];
        outpath = argv[3];
    } else if (argc == 3) {
        inpath = argv[1];
        outpath = argv[2];
    } else {
        printf("usage: apngfix [-s|--still] input.apng output.apng\n");
        printf("  -s: convert single-frame APNG to plain PNG (frame 0)\n");
        return 1;
    }

    f = fopen(inpath, "rb");
    if (!f) { printf("apngfix: cannot open %s\n", inpath); return 1; }
    fseek(f, 0, SEEK_END);
    fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (fsize <= 0 || (unsigned long)fsize > APNGFIX_MAX_FILE) {
        printf("apngfix: bad file size\n");
        fclose(f);
        return 1;
    }
    filebuf = (unsigned char *)malloc((size_t)fsize);
    if (!filebuf) { printf("apngfix: out of memory\n"); fclose(f); return 1; }
    if (fread(filebuf, 1, (size_t)fsize, f) != (size_t)fsize) {
        printf("apngfix: short read\n");
        free(filebuf); fclose(f);
        return 1;
    }
    fclose(f);

    if ((size_t)fsize < 8 || memcmp(filebuf, sig, 8) != 0) {
        printf("apngfix: %s: not a PNG file (bad signature)\n", inpath);
        free(filebuf);
        return 1;
    }

    /* ---- walk chunks ---- */
    nch = 0; cap = 256;
    ch = (Chunk *)malloc(cap * sizeof(Chunk));
    if (!ch) { printf("apngfix: out of memory\n"); free(filebuf); return 1; }
    pos = 8;
    while (pos + 8 <= (size_t)fsize && !fatal) {
        unsigned long len, type, need;
        unsigned long want, got;
        if (nch >= cap) {
            Chunk *nc;
            cap *= 2;
            nc = (Chunk *)realloc(ch, cap * sizeof(Chunk));
            if (!nc) { printf("apngfix: out of memory\n"); fatal = 1; break; }
            ch = nc;
        }
        len = rd32(filebuf + pos);
        type = fourcc(filebuf + pos + 4);
        if (len > (unsigned long)fsize || pos + 12 + len > (size_t)fsize) {
            printf("apngfix: %s: truncated chunk at offset %lu\n", inpath, (unsigned long)pos);
            fatal = 1;
            break;
        }
        need = len;
        ch[nch].type = type;
        ch[nch].len = len;
        ch[nch].file_off = (long)pos;
        ch[nch].keep = 1;
        ch[nch].data = NULL;
        if (need) {
            ch[nch].data = (unsigned char *)malloc(need ? need : 1);
            if (!ch[nch].data) { printf("apngfix: out of memory\n"); fatal = 1; break; }
            memcpy(ch[nch].data, filebuf + pos + 8, need);
        }
        /* CRC check (type + data) */
        want = rd32(filebuf + pos + 8 + len);
        {
            unsigned long c;
            unsigned char hdr[4];
            wr32(hdr, type);
            c = crc_update(0, hdr, 4);
            if (len) c = crc_update(c, ch[nch].data, len);
            got = c;
        }
        if (want != got) {
            printf("apngfix: %s: bad CRC in %.4s chunk at %lu (recomputing)\n",
                   inpath, (char *)(filebuf + pos + 4), (unsigned long)pos);
            repairs++;
        }
        /* classify */
        if (!have_ihdr) {
            if (type != FCC('I','H','D','R')) {
                printf("apngfix: %s: first chunk is not IHDR\n", inpath);
                fatal = 1;
                break;
            }
            have_ihdr = 1;
            ihdr_at = nch;
            if (len != 13) {
                printf("apngfix: %s: malformed IHDR (len %lu)\n", inpath, len);
                fatal = 1;
                break;
            }
            canvas_w = rd32(ch[nch].data);
            canvas_h = rd32(ch[nch].data + 4);
        } else if (type == FCC('a','c','T','L')) {
            if (len != 8) {
                printf("apngfix: %s: malformed acTL (len %lu)\n", inpath, len);
                fatal = 1;
                break;
            }
            if (!have_actl) {
                have_actl = 1;
                actl_at = nch;
                actl_frames = rd32(ch[nch].data);
            } else {
                printf("apngfix: %s: duplicate acTL at %lu (dropping)\n",
                       inpath, (unsigned long)pos);
                ch[nch].keep = 0;
                repairs++;
            }
        } else if (type == FCC('f','c','T','L')) {
            if (len != 26) {
                printf("apngfix: %s: malformed fcTL (len %lu)\n", inpath, len);
                fatal = 1;
                break;
            }
            if (!have_actl) {
                /* repaired below by inserting acTL; count it */
            }
            if (!seen_idat) fctl_before_idat = 1;
            f0_w = rd32(ch[nch].data + 4);
            f0_h = rd32(ch[nch].data + 8);
            f0_x = rd32(ch[nch].data + 12);
            f0_y = rd32(ch[nch].data + 16);
            fctl_count++;
        } else if (type == FCC('I','D','A','T')) {
            if (seen_fdat) {
                printf("apngfix: %s: IDAT after fdAT (unrepairable)\n", inpath);
                fatal = 1;
                break;
            }
            if (!seen_idat) { seen_idat = 1; first_idat_at = nch; }
            idat_count++;
        } else if (type == FCC('f','d','A','T')) {
            if (fctl_count == 0) {
                printf("apngfix: %s: fdAT before any fcTL (unrepairable)\n", inpath);
                fatal = 1;
                break;
            }
            seen_fdat = 1;
            fdat_count++;
        } else if (type == FCC('I','E','N','D')) {
            if (!have_iend) {
                have_iend = 1;
                iend_at = nch;
            } else {
                /* extra IEND = garbage: release, do not store or count */
                free(ch[nch].data);
                ch[nch].data = NULL;
                printf("apngfix: %s: extra IEND at %lu (dropping)\n",
                       inpath, (unsigned long)pos);
                repairs++;
            }
            /* everything after first IEND is garbage */
            pos += 12 + len;
            break;
        }
        pos += 12 + len;
        nch++;
        /* count the IEND chunk too */
        if (type == FCC('I','E','N','D')) {
            /* nch++ already done above via flow; adjust: we broke before it */
        }
    }

    if (!fatal) {
        /* account for loop-exit counting: the IEND break skipped nch++ */
        if (have_iend && (iend_at >= nch)) {
            /* IEND was stored but not counted */
            nch = iend_at + 1;
        }
        if (pos < (size_t)fsize) {
            printf("apngfix: %s: %lu trailing bytes after IEND (stripping)\n",
                   inpath, (unsigned long)((size_t)fsize - pos));
            repairs++;
        }
        if (!have_iend && have_ihdr) {
            /* append missing IEND */
            if (nch >= cap) {
                Chunk *nc = (Chunk *)realloc(ch, (cap + 1) * sizeof(Chunk));
                if (!nc) { printf("apngfix: out of memory\n"); fatal = 1; }
                else { ch = nc; cap++; }
            }
            if (!fatal) {
                printf("apngfix: %s: missing IEND (appending)\n", inpath);
                ch[nch].type = FCC('I','E','N','D');
                ch[nch].len = 0;
                ch[nch].data = NULL;
                ch[nch].keep = 1;
                ch[nch].file_off = -1;
                have_iend = 1;
                iend_at = nch;
                nch++;
                repairs++;
            }
        }
    }

    /* ---- structural repairs (skipped in still mode) ---- */
    if (!fatal && !want_still && have_ihdr) {
        /* acTL after IDAT: move it before first IDAT */
        if (have_actl && seen_idat && actl_at > first_idat_at) {
            Chunk tmp;
            size_t j;
            printf("apngfix: %s: acTL after IDAT (moving before first IDAT)\n", inpath);
            tmp = ch[actl_at];
            for (j = actl_at; j > first_idat_at; j--)
                ch[j] = ch[j - 1];
            ch[first_idat_at] = tmp;
            repairs++;
        }
        /* fcTL without acTL: insert acTL (frames = fcTL count, plays = 0) */
        if (fctl_count > 0 && !have_actl) {
            Chunk *nc;
            size_t j;
            printf("apngfix: %s: fcTL without acTL (inserting acTL, frames=%lu)\n",
                   inpath, (unsigned long)fctl_count);
            nc = (Chunk *)realloc(ch, (cap + 1) * sizeof(Chunk));
            if (!nc) { printf("apngfix: out of memory\n"); fatal = 1; }
            else {
                unsigned char *ad;
                ch = nc; cap++;
                ad = (unsigned char *)malloc(8);
                if (!ad) { printf("apngfix: out of memory\n"); fatal = 1; }
                else {
                    wr32(ad, (unsigned long)fctl_count);
                    wr32(ad + 4, 0);
                    for (j = nch; j > ihdr_at + 1; j--)
                        ch[j] = ch[j - 1];
                    ch[ihdr_at + 1].type = FCC('a','c','T','L');
                    ch[ihdr_at + 1].len = 8;
                    ch[ihdr_at + 1].data = ad;
                    ch[ihdr_at + 1].keep = 1;
                    ch[ihdr_at + 1].file_off = -1;
                    nch++;
                    have_actl = 1;
                    actl_frames = (unsigned long)fctl_count;
                    repairs++;
                }
            }
        }
        /* acTL with zero fcTL: drop it (plain PNG) */
        if (have_actl && fctl_count == 0) {
            size_t j;
            for (j = 0; j < nch; j++) {
                if (ch[j].keep && ch[j].type == FCC('a','c','T','L')) {
                    printf("apngfix: %s: acTL with no frames (dropping, plain PNG)\n", inpath);
                    free(ch[j].data);
                    ch[j].data = NULL;
                    ch[j].keep = 0;
                    have_actl = 0;
                    repairs++;
                    break;
                }
            }
        }
        /* acTL num_frames mismatch: patch to actual fcTL count */
        if (have_actl && fctl_count > 0) {
            size_t j;
            for (j = 0; j < nch; j++) {
                if (ch[j].keep && ch[j].type == FCC('a','c','T','L')) {
                    if (rd32(ch[j].data) != (unsigned long)fctl_count) {
                        printf("apngfix: %s: acTL num_frames %lu -> %lu\n",
                               inpath, rd32(ch[j].data), (unsigned long)fctl_count);
                        wr32(ch[j].data, (unsigned long)fctl_count);
                        actl_frames = (unsigned long)fctl_count;
                        repairs++;
                    } else {
                        actl_frames = (unsigned long)fctl_count;
                    }
                    break;
                }
            }
        }
        /* resequence fcTL/fdAT from 0 in file order */
        {
            unsigned long seq = 0;
            int seq_bad = 0;
            for (i = 0; i < nch; i++) {
                if (!ch[i].keep) continue;
                if (ch[i].type == FCC('f','c','T','L') || ch[i].type == FCC('f','d','A','T')) {
                    if (rd32(ch[i].data) != seq) {
                        wr32(ch[i].data, seq);
                        seq_bad = 1;
                    }
                    seq++;
                }
            }
            if (seq_bad) {
                printf("apngfix: %s: resequenced %lu fcTL/fdAT chunks\n", inpath, seq);
                repairs++;
            }
        }
    }

    /* ---- single-frame APNG to still PNG (frame 0 becomes the image) ---- */
    if (!fatal && want_still && have_ihdr) {
        frame0_in_idat = fctl_before_idat && idat_count > 0;
        if (!have_actl || fctl_count != 1) {
            printf("apngfix: %s: not a single-frame APNG (fcTL count = %lu)\n",
                   inpath, (unsigned long)fctl_count);
            fatal = 1;
        } else if (actl_frames != 1) {
            /* Declared multi-frame but only 1 fcTL on disk (truncated or
               non-standard): refuse rather than silently dropping frames. */
            printf("apngfix: %s: acTL declares %lu frames, not single-frame\n",
                   inpath, actl_frames);
            fatal = 1;
        } else if (canvas_w == 0 || canvas_h == 0) {
            printf("apngfix: %s: zero-size canvas\n", inpath);
            fatal = 1;
        } else if (f0_x != 0 || f0_y != 0 || f0_w != canvas_w || f0_h != canvas_h) {
            printf("apngfix: %s: frame 0 is a sub-rect (%lux%lu+%lu+%lu of %lux%lu), cannot flatten\n",
                   inpath, f0_w, f0_h, f0_x, f0_y, canvas_w, canvas_h);
            fatal = 1;
        } else if (!frame0_in_idat && fdat_count == 0) {
            printf("apngfix: %s: frame 0 has no data\n", inpath);
            fatal = 1;
        } else {
            for (i = 0; i < nch; i++) {
                if (!ch[i].keep) continue;
                if (ch[i].type == FCC('a','c','T','L') ||
                    ch[i].type == FCC('f','c','T','L')) {
                    ch[i].keep = 0;
                } else if (ch[i].type == FCC('f','d','A','T')) {
                    if (frame0_in_idat) {
                        ch[i].keep = 0; /* stray: stb renders frame 0 from IDAT */
                    } else {
                        if (ch[i].len < 4) {
                            printf("apngfix: %s: malformed fdAT\n", inpath);
                            fatal = 1;
                            break;
                        }
                        memmove(ch[i].data, ch[i].data + 4, ch[i].len - 4);
                        ch[i].len -= 4;
                        ch[i].type = FCC('I','D','A','T');
                    }
                } else if (ch[i].type == FCC('I','D','A','T')) {
                    if (!frame0_in_idat)
                        ch[i].keep = 0; /* drop unseen default image */
                }
            }
            if (!fatal) {
                printf("apngfix: %s -> %s: 1-frame APNG to still PNG %lux%lu (frame 0 from %s%s)\n",
                       inpath, outpath, canvas_w, canvas_h,
                       frame0_in_idat ? "IDAT" : "fdAT",
                       (!frame0_in_idat && idat_count > 0) ? "; default image discarded" : "");
                converted = 1;
            }
        }
    }

    if (fatal) {
        free_chunks(ch, nch);
        free(filebuf);
        return 1;
    }

    /* ---- write output (fresh CRCs) ---- */
    out = fopen(outpath, "wb");
    if (!out) {
        printf("apngfix: cannot write %s\n", outpath);
        free_chunks(ch, nch);
        free(filebuf);
        return 1;
    }
    {
        static const unsigned char sigw[8] = {137,80,78,71,13,10,26,10};
        unsigned char hdr[8];
        fwrite(sigw, 1, 8, out);
        for (i = 0; i < nch; i++) {
            unsigned long c;
            unsigned char th[4];
            if (!ch[i].keep) continue;
            wr32(hdr, ch[i].len);
            wr32(hdr + 4, ch[i].type);
            fwrite(hdr, 1, 8, out);
            if (ch[i].len) fwrite(ch[i].data, 1, ch[i].len, out);
            wr32(th, ch[i].type);
            c = crc_update(0, th, 4);
            if (ch[i].len) c = crc_update(c, ch[i].data, ch[i].len);
            wr32(hdr, c);
            fwrite(hdr, 1, 4, out);
        }
    }
    fclose(out);
    free_chunks(ch, nch);
    free(filebuf);

    /* silence unused-var warnings on exotic compilers */
    (void)ihdr_at; (void)iend_at; (void)first_idat_at;
    (void)idat_count; (void)fdat_count;

    if (converted) {
        /* conversion already reported */
    } else if (repairs == 0)
        printf("apngfix: %s: OK (%lu frame%s), no repairs needed\n",
               inpath, actl_frames ? actl_frames : 0,
               actl_frames == 1 ? "" : "s");
    else
        printf("apngfix: %s -> %s: %d repair%s applied (%lu frame%s)\n",
               inpath, outpath, repairs, repairs == 1 ? "" : "s",
               actl_frames ? actl_frames : 0,
               actl_frames == 1 ? "" : "s");
    return 0;
}
