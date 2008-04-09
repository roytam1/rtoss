#ifndef __MT19937AR_H__
#define __MT19937AR_H__

void __cdecl init_genrand(unsigned long s);
void __cdecl init_by_array(unsigned long init_key[], unsigned long key_length);
static void __cdecl next_state(void);
unsigned long __cdecl genrand_int32(void);
long __cdecl genrand_int31(void);
double __cdecl genrand_real1(void);
double __cdecl genrand_real2(void);
double __cdecl genrand_real3(void);

#define srand init_genrand
#define rand genrand_int32

#endif // __MT19937AR_H__