#ifndef ARM_H
#define ARM_H

extern int arch_neon;           /* true if we have ARM NEON or ASIMD abilities */
extern int arch_aarch64_crc32;  /* true if we have AArch64 CRC32/CRC32C abilities */
extern int arch_aarch64_pmull;  /* true if we have AArch64 PMULL abilities */

extern int arch_arm_probe(void);

#endif // ARM_H
