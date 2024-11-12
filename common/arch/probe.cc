#include "probe.h"
#include "intel.h"
#include "arm.h"
#include "ppc.h"

int arch_probe(void)
{
  if (arch_probed)
    return 1;
#if defined(__i386__) || defined(__x86_64__)
  arch_intel_probe();
#elif defined(__arm__) || defined(__aarch64__)
  arch_arm_probe();
#elif defined(__powerpc__) || defined(__ppc__)
  arch_ppc_probe();
#endif
  arch_probed = 1;
  return 1;
}

// do this once using the magic of c++.
int arch_probed = arch_probe();
