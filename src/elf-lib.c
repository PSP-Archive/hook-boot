/*
copyright (c) 2005 djhuevo

lib: elf-lib 0.1r2
a minimalistic elf parsing library

see license.txt at elf-lib root directoty
*/

#include "common.h"

#if MODULE_HOOK

// main elf
int elf_getElfHeader(t_elf *elf)
{
	elf->elf_hdr = (Elf32_Ehdr *)elf->base;
    return 0;
}

int elf_loadElf(t_elf *elf) {
    int i;
    t_elfSection symbols;

    if(elf_getElfHeader(elf)<0) return -1;

    if(!(elf->elf_hdr->ident[0]==ELFMAG0
       &&elf->elf_hdr->ident[1]==ELFMAG1
       &&elf->elf_hdr->ident[2]==ELFMAG2
       &&elf->elf_hdr->ident[3]==ELFMAG3)) return -2;

    elf_getSection(elf, &symbols, elf->elf_hdr->shstrndx);
    elf->sechdr_strtbl=symbols.data;

    elf->elfprgs=(t_elfProgram *)malloc(sizeof(t_elfProgram)*elf->elf_hdr->phnum);
    elf->elfsecs=(t_elfSection *)malloc(sizeof(t_elfSection)*elf->elf_hdr->shnum);
    memset(elf->elfprgs, 0x00, sizeof(t_elfProgram)*elf->elf_hdr->phnum);
    memset(elf->elfsecs, 0x00, sizeof(t_elfSection)*elf->elf_hdr->shnum);  

    for(i=0;i<elf->elf_hdr->phnum;i++)
        elf_getProgramHeader(elf, &elf->elfprgs[i], i);

    for(i=0;i<elf->elf_hdr->shnum;i++)
        elf_getSectionHeader(elf, &elf->elfsecs[i], i);

    return 0;
}

int elf_closeElf(t_elf *elf) {
    int i;
    free(elf->elfsecs);
    free(elf->elfprgs);
    free(elf->sechdr_strtbl);
    for(i=0;i<elf->elf_hdr->shnum;i++) {
        if(elf->elfsecs[i].data!=NULL) free(elf->elfsecs[i].data);
    }
	return 0;
}

#if 0
int elf_dumpElfHeader(t_elf *elf) {
    printf("ident:      .ELF\n");
    printf("type:       %d\n",elf->elf_hdr->type);
    printf("machine:    %d\n",elf->elf_hdr->machine);
    printf("version:    %d\n",elf->elf_hdr->version);
    printf("entry:      0x%08x\n",(int)elf->elf_hdr->entry);
    printf("phoff:      0x%08x\n",elf->elf_hdr->phoff);
    printf("shoff:      0x%08x\n",elf->elf_hdr->shoff);
    printf("flags:      0x%x\n",elf->elf_hdr->flags);
    printf("ehsize:     %d\n",elf->elf_hdr->ehsize);
    printf("phentsize:  %d\n",elf->elf_hdr->phentsize);
    printf("phnum:      %d\n",elf->elf_hdr->phnum);
    printf("shentsize:  %d\n",elf->elf_hdr->shentsize);
    printf("shnum:      %d\n",elf->elf_hdr->shnum);
    printf("shstrndx:   %d\n",elf->elf_hdr->shstrndx);
	return 0;
}
#endif


// programs
int elf_getProgramOffset(Elf32_Ehdr *elfHeader, int section) {
    if(section>elfHeader->phnum) return -1;
    return elfHeader->phoff+(section*elfHeader->phentsize);
}

int elf_getProgramHeader(t_elf *elf, t_elfProgram *elf_prg, int program) {
    int offset;
    offset=elf_getProgramOffset(elf->elf_hdr, program);
    if(offset<0) return -1;

    elf_prg->prg_hdr = (Elf32_Phdr *)(elf->base+offset);

	return 0;
}

#if 0
int elf_dumpProgramHeader(t_elfProgram *elf_prg) {
    printf("type:   %d\n",elf_prg->prg_hdr->type);
    printf("offset: 0x%08x\n",elf_prg->prg_hdr->offset);
    printf("vaddr:  0x%08x\n",(int)elf_prg->prg_hdr->vaddr);
    printf("paddr:  0x%08x\n",(int)elf_prg->prg_hdr->paddr);
    printf("filesz: %d\n",elf_prg->prg_hdr->filesz);
    printf("memsz:  %d\n",elf_prg->prg_hdr->memsz);
    printf("flags:  0x%x\n",elf_prg->prg_hdr->flags);
    printf("align:  %d\n",elf_prg->prg_hdr->align);
	return 0;
}
#endif

// sections
int elf_getSectionOffset(Elf32_Ehdr *elfHeader, int section) {
    if(section>elfHeader->shnum) return -1;
    return elfHeader->shoff+(section*elfHeader->shentsize);
}

int elf_getSectionHeader(t_elf *elf, t_elfSection *elf_sec, int section) {
    int offset;
    offset=elf_getSectionOffset(elf->elf_hdr, section);
    if(offset<0) return -1;

	elf_sec->sec_hdr = (Elf32_Shdr *)(elf->base+offset);
	return 0;
}

int elf_getSectionData(t_elf *elf, t_elfSection *elf_sec) {
    elf_getData(elf, &elf_sec->data, elf_sec->sec_hdr->offset, elf_sec->sec_hdr->size);
	return 0;
}

int elf_getSection(t_elf *elf, t_elfSection *elf_sec, int section)
{
    elf_getSectionHeader(elf, elf_sec, section);
    elf_getSectionData(elf, elf_sec);
	return 0;
}

#if 0
int elf_dumpSectionHeader(t_elfSection *elf_sec, unsigned char *stringtable) {
    if(stringtable!=NULL)
        printf("name:       %s\n",stringtable+elf_sec->sec_hdr->name);

    printf("type:       %d\n",elf_sec->sec_hdr->type);
    printf("flags:      0x%x\n",elf_sec->sec_hdr->flags);
    printf("addr:       0x%08x\n",(int)elf_sec->sec_hdr->addr);
    printf("offset:     0x%08x\n",elf_sec->sec_hdr->offset);
    printf("size:       %d\n",elf_sec->sec_hdr->size);
    printf("link:       %d\n",elf_sec->sec_hdr->link);
    printf("info:       %d\n",elf_sec->sec_hdr->info);
    printf("addralign:  %d\n",elf_sec->sec_hdr->addralign);
    printf("entsize:    %d\n",elf_sec->sec_hdr->entsize);
	return 0;
}
#endif

// others
int elf_getData(t_elf *elf, unsigned char **data, int offset, int size)
{
	*data = (elf->base+offset);
	return 1;
}

#endif
