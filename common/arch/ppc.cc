#include <stdio.h>
#include "probe.h"
#include "ppc.h"

#ifdef HAVE_PPC64LE
#include <sys/auxv.h>
#include <asm/cputable.h>
#endif /* HAVE_PPC64LE */

/* flags we export */
int arch_ppc_crc32 = 0;

#ifndef PPC_FEATURE2_VEC_CRYPTO
#define PPC_FEATURE2_VEC_CRYPTO 0x02000000
#endif

#ifndef AT_HWCAP2
#define AT_HWCAP2  26
#endif

int arch_ppc_probe(void)
{
  arch_ppc_crc32 = 0;

#ifdef HAVE_PPC64LE
  if (getauxval(AT_HWCAP2) & PPC_FEATURE2_VEC_CRYPTO)
    arch_ppc_crc32 = 1;
#endif /* HAVE_PPC64LE */

  return 0;
}
