#ifndef __KPATHC_H__
#define __KPATHC_H__

// Kernel memory Area
#define PSP_KMEM_TOP  0x88000000
#define PSP_KMEM_SIZE 0x00800000
#define PSP_KMEM_END  0x88804000

// user memory Area
#define PSP_UMEM_TOP 0x08804000
#define PSP_UMEM_END 0x09FFC000

// LIB TYPE
#define PSP_DRV_TYPE_IMPORT 0x0005
#define PSP_DRV_TYPE_EXPORT 0x0004

// MIPS assempler
#define MIPS_LUI(R,IMM) 0x3c000000|(R<<16)|((unsigned int)(IMM)&0xffff)
#define MIPS_ORI(RS,RT,IMM) 0x34000000|(RS<<21)|(RS<<16)|((unsigned int)(IMM)&0xffff)
#define MIPS_ADDIU(RS,RT,IMM) 0x24000000|(RS<<21)|(RS<<16)|((unsigned int)(IMM)&0xffff)
#define MIPS_NOP 0x00000000
#define MIPS_J(ADDR) (0x08000000 + ((((unsigned int)(ADDR))&0x0ffffffc)>>2))
#define MIPS_JR(R) (0x00000008 + ((R)<<20))
#define MIPS_SYSCALL(NUM) (0x0000000C+((NUM)<<6))
#define MIPS_NOP 0x00000000

// import/export version
int psp_lib_patch(unsigned int top,unsigned int size,unsigned int type,unsigned int old_ver,unsigned int new_ver);

// devkit driver vetsion
int patch_kernel_version(unsigned short new_ver);

void clear_cache(void);

#endif
