/*
copyright (c) 2004 djhuevo

lib: elf-lib 0.1r
a minimalistic elf parsing library

see license.txt at elf-lib root directoty
*/

#ifndef _ELF_LIB_H_
#define _ELF_LIB_H_
#define _ELF_LIB_VER_0_1r_

#include "elf.h"

typedef struct {
    Elf32_Shdr *sec_hdr;
    unsigned char *data;
} t_elfSection;

typedef struct {
    Elf32_Phdr *prg_hdr;
} t_elfProgram;


typedef struct {
    //FILE *fd;
    unsigned char *base;
    Elf32_Ehdr *elf_hdr;
    unsigned char *sechdr_strtbl;
    t_elfProgram *elfprgs;
    t_elfSection *elfsecs;
} t_elf;


int elf_getElfHeader(t_elf *elf);
int elf_loadElf(t_elf *elf);
int elf_closeElf(t_elf *elf);
int elf_dumpElfHeader(t_elf *elf);

int elf_getProgramOffset(Elf32_Ehdr *elfHeader, int section);
int elf_getProgramHeader(t_elf *elf, t_elfProgram *elf_prg, int program);
int elf_dumpProgramHeader(t_elfProgram *elf_prg);

int elf_getSectionOffset(Elf32_Ehdr *elfHeader, int section);
int elf_getSectionHeader(t_elf *elf, t_elfSection *elf_sec, int section);
int elf_getSectionData(t_elf *elf, t_elfSection *elf_sec);
int elf_getSection(t_elf *elf, t_elfSection *elf_sec, int section);
int elf_dumpSectionHeader(t_elfSection *elf_sec, unsigned char *stringtable);

int elf_getData(t_elf *elf, unsigned char **data, int offset, int size);

t_elfSection *get_seccion_by_name(t_elf *elf, char *name);
t_elfSection *get_seccion_with_addr(t_elf *elf, Elf32_Addr addr);
t_elfSection *get_seccion_with_offset(t_elf *elf, Elf32_Off offset);

#endif
