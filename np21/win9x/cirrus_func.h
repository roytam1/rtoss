/*
typedef void (*cirrus_bitblt_rop_t) (struct CirrusVGAState *s,
                                     uint8_t * dst, const uint8_t * src,
				     int dstpitch, int srcpitch,
				     int bltwidth, int bltheight);
typedef void (*cirrus_fill_t)(struct CirrusVGAState *s,
                              uint8_t *dst, int dst_pitch, int width, int height);

void cirrus_bitblt_rop_nop(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);

void cirrus_bitblt_rop_fwd_0(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_fwd_src_and_dst(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_fwd_src_and_notdst(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_fwd_notdst(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_fwd_src(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_fwd_1(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_fwd_notsrc_and_dst(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_fwd_src_xor_dst(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_fwd_src_or_dst(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_fwd_notsrc_or_notdst(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_fwd_src_notxor_dst(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_fwd_src_or_notdst(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_fwd_notsrc(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_fwd_notsrc_or_dst(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_fwd_notsrc_and_notdst(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);

void cirrus_bitblt_rop_bkwd_0(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_bkwd_src_and_dst(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_bkwd_src_and_notdst(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_bkwd_notdst(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_bkwd_src(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_bkwd_1(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_bkwd_notsrc_and_dst(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_bkwd_src_xor_dst(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_bkwd_src_or_dst(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_bkwd_notsrc_or_notdst(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_bkwd_src_notxor_dst(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_bkwd_src_or_notdst(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_bkwd_notsrc(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_bkwd_notsrc_or_dst(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_bkwd_notsrc_and_notdst(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);

void cirrus_bitblt_rop_nop_8(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_nop_16(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_nop_24(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_nop_32(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);

void cirrus_bitblt_rop_fwd_transp_0_8(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_fwd_transp_src_and_dst_8(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_fwd_transp_src_and_notdst_8(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_fwd_transp_notdst_8(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_fwd_transp_src_8(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_fwd_transp_1_8(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_fwd_transp_notsrc_and_dst_8(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_fwd_transp_src_xor_dst_8(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_fwd_transp_src_or_dst_8(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_fwd_transp_notsrc_or_notdst_8(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_fwd_transp_src_notxor_dst_8(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_fwd_transp_src_or_notdst_8(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_fwd_transp_notsrc_8(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_fwd_transp_notsrc_or_dst_8(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_fwd_transp_notsrc_and_notdst_8(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);

void cirrus_bitblt_rop_fwd_transp_0_16(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_fwd_transp_src_and_dst_16(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_fwd_transp_src_and_notdst_16(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_fwd_transp_notdst_16(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_fwd_transp_src_16(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_fwd_transp_1_16(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_fwd_transp_notsrc_and_dst_16(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_fwd_transp_src_xor_dst_16(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_fwd_transp_src_or_dst_16(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_fwd_transp_notsrc_or_notdst_16(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_fwd_transp_src_notxor_dst_16(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_fwd_transp_src_or_notdst_16(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_fwd_transp_notsrc_16(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_fwd_transp_notsrc_or_dst_16(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_fwd_transp_notsrc_and_notdst_16(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);

void cirrus_bitblt_rop_bkwd_transp_0_8(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_bkwd_transp_src_and_dst_8(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_bkwd_transp_src_and_notdst_8(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_bkwd_transp_notdst_8(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_bkwd_transp_src_8(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_bkwd_transp_1_8(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_bkwd_transp_notsrc_and_dst_8(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_bkwd_transp_src_xor_dst_8(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_bkwd_transp_src_or_dst_8(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_bkwd_transp_notsrc_or_notdst_8(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_bkwd_transp_src_notxor_dst_8(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_bkwd_transp_src_or_notdst_8(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_bkwd_transp_notsrc_8(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_bkwd_transp_notsrc_or_dst_8(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_bkwd_transp_notsrc_and_notdst_8(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);

void cirrus_bitblt_rop_bkwd_transp_0_16(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_bkwd_transp_src_and_dst_16(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_bkwd_transp_src_and_notdst_16(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_bkwd_transp_notdst_16(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_bkwd_transp_src_16(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_bkwd_transp_1_16(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_bkwd_transp_notsrc_and_dst_16(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_bkwd_transp_src_xor_dst_16(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_bkwd_transp_src_or_dst_16(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_bkwd_transp_notsrc_or_notdst_16(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_bkwd_transp_src_notxor_dst_16(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_bkwd_transp_src_or_notdst_16(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_bkwd_transp_notsrc_16(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_bkwd_transp_notsrc_or_dst_16(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);
void cirrus_bitblt_rop_bkwd_transp_notsrc_and_notdst_16(struct CirrusVGAState *s, uint8_t * dst, const uint8_t * src, int dstpitch, int srcpitch, int bltwidth, int bltheight);

void cirrus_patternfill_0),
void cirrus_patternfill_src_and_dst),
void cirrus_bitblt_fill_nop),
void cirrus_patternfill_src_and_notdst),
void cirrus_patternfill_notdst),
void cirrus_patternfill_src),
void cirrus_patternfill_1),
void cirrus_patternfill_notsrc_and_dst),
void cirrus_patternfill_src_xor_dst),
void cirrus_patternfill_src_or_dst),
void cirrus_patternfill_notsrc_or_notdst),
void cirrus_patternfill_src_notxor_dst),
void cirrus_patternfill_src_or_notdst),
void cirrus_patternfill_notsrc),
void cirrus_patternfill_notsrc_or_dst),
void cirrus_patternfill_notsrc_and_notdst),

void cirrus_colorexpand_transp_0),
void cirrus_colorexpand_transp_src_and_dst),
void cirrus_colorexpand_transp_src_and_notdst),
void cirrus_colorexpand_transp_notdst),
void cirrus_colorexpand_transp_src),
void cirrus_colorexpand_transp_1),
void cirrus_colorexpand_transp_notsrc_and_dst),
void cirrus_colorexpand_transp_src_xor_dst),
void cirrus_colorexpand_transp_src_or_dst),
void cirrus_colorexpand_transp_notsrc_or_notdst),
void cirrus_colorexpand_transp_src_notxor_dst),
void cirrus_colorexpand_transp_src_or_notdst),
void cirrus_colorexpand_transp_notsrc),
void cirrus_colorexpand_transp_notsrc_or_dst),
void cirrus_colorexpand_transp_notsrc_and_notdst),

void cirrus_colorexpand_0),
void cirrus_colorexpand_src_and_dst),
void cirrus_colorexpand_src_and_notdst),
void cirrus_colorexpand_notdst),
void cirrus_colorexpand_src),
void cirrus_colorexpand_1),
void cirrus_colorexpand_notsrc_and_dst),
void cirrus_colorexpand_src_xor_dst),
void cirrus_colorexpand_src_or_dst),
void cirrus_colorexpand_notsrc_or_notdst),
void cirrus_colorexpand_src_notxor_dst),
void cirrus_colorexpand_src_or_notdst),
void cirrus_colorexpand_notsrc),
void cirrus_colorexpand_notsrc_or_dst),
void cirrus_colorexpand_notsrc_and_notdst),

void cirrus_colorexpand_pattern_transp_0),
void cirrus_colorexpand_pattern_transp_src_and_dst),
void cirrus_colorexpand_pattern_transp_src_and_notdst),
void cirrus_colorexpand_pattern_transp_notdst),
void cirrus_colorexpand_pattern_transp_src),
void cirrus_colorexpand_pattern_transp_1),
void cirrus_colorexpand_pattern_transp_notsrc_and_dst),
void cirrus_colorexpand_pattern_transp_src_xor_dst),
void cirrus_colorexpand_pattern_transp_src_or_dst),
void cirrus_colorexpand_pattern_transp_notsrc_or_notdst),
void cirrus_colorexpand_pattern_transp_src_notxor_dst),
void cirrus_colorexpand_pattern_transp_src_or_notdst),
void cirrus_colorexpand_pattern_transp_notsrc),
void cirrus_colorexpand_pattern_transp_notsrc_or_dst),
void cirrus_colorexpand_pattern_transp_notsrc_and_notdst),

void cirrus_colorexpand_pattern_0),
void cirrus_colorexpand_pattern_src_and_dst),
void cirrus_colorexpand_pattern_src_and_notdst),
void cirrus_colorexpand_pattern_notdst),
void cirrus_colorexpand_pattern_src),
void cirrus_colorexpand_pattern_1),
void cirrus_colorexpand_pattern_notsrc_and_dst),
void cirrus_colorexpand_pattern_src_xor_dst),
void cirrus_colorexpand_pattern_src_or_dst),
void cirrus_colorexpand_pattern_notsrc_or_notdst),
void cirrus_colorexpand_pattern_src_notxor_dst),
void cirrus_colorexpand_pattern_src_or_notdst),
void cirrus_colorexpand_pattern_notsrc),
void cirrus_colorexpand_pattern_notsrc_or_dst),
void cirrus_colorexpand_pattern_notsrc_and_notdst),

void cirrus_fill_0),
void cirrus_fill_src_and_dst),
void cirrus_fill_src_and_notdst),
void cirrus_fill_notdst),
void cirrus_fill_src),
void cirrus_fill_1),
void cirrus_fill_notsrc_and_dst),
void cirrus_fill_src_xor_dst),
void cirrus_fill_src_or_dst),
void cirrus_fill_notsrc_or_notdst),
void cirrus_fill_src_notxor_dst),
void cirrus_fill_src_or_notdst),
void cirrus_fill_notsrc),
void cirrus_fill_notsrc_or_dst),
void cirrus_fill_notsrc_and_notdst),
*/