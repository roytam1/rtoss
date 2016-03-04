#pragma once

#if defined(SUPPORT_LGY98)

#ifdef __cplusplus
extern "C" {
#endif

//void IOOUTCALL ideio_w16(UINT port, REG16 value);
//REG16 IOINPCALL ideio_r16(UINT port);
	
void  IOOUTCALL lgy98_ob200_8(UINT addr, REG8 value);
REG8  IOOUTCALL lgy98_ib200_8(UINT addr);
void  IOOUTCALL lgy98_ob200_16(UINT addr, REG16 value);
REG16 IOOUTCALL lgy98_ib200_16(UINT addr);

void lgy98_reset(const NP2CFG *pConfig);
void lgy98_bind(void);
void lgy98_init(void);
void lgy98_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif /* SUPPORT_LGY98 */