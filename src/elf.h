/*
Copyright (C) 1996 by Josh Vanderhoof

You may redistribute this file under the terms
of the GNU Library General Public License.
*/

#ifndef ELF_H
#define ELF_H
/*
########################################
# elf.h - this contains the definitions from
# `Executable and Linkable Format (ELF)'
# Tool Interface Standards(TIS), Portable
# Formats Specification, Version 1.1
########################################
*/


/* ########## Data Representation ########## */

/* 32-Bit Data Types */
typedef void *Elf32_Addr;
typedef unsigned short int Elf32_Half;
typedef unsigned int Elf32_Off;
typedef int Elf32_Sword;
typedef unsigned int Elf32_Word;

/* ########## ELF Header ########## */

/* ELF Header */
#define EI_NIDENT 	16

typedef struct Elf32_Ehdr
	{
	unsigned char ident[EI_NIDENT];
	Elf32_Half type;
	Elf32_Half machine;
	Elf32_Word version;
	Elf32_Addr entry;
	Elf32_Off phoff;
	Elf32_Off shoff;
	Elf32_Word flags;
	Elf32_Half ehsize;
	Elf32_Half phentsize;
	Elf32_Half phnum;
	Elf32_Half shentsize;
	Elf32_Half shnum;
	Elf32_Half shstrndx;
	} Elf32_Ehdr;

#define ET_NONE 	0
#define ET_REL 	1
#define ET_EXEC 	2
#define ET_DYN 	3
#define ET_CORE 	4
#define ET_ LOPROC 	0xff00
#define ET_HIPROC 	0xffff

#define EM_NONE 	0
#define EM_M32	1
#define EM_SPARC	2
#define EM_386 	3
#define EM_68K	4
#define EM_88K	5
#define EM_860	7
#define EM_MIPS	8

#define EV_NONE 	0
#define EV_CURRENT 	1

/* ########## ELF Identification ########## */

#define EI_MAG0 	0
#define EI_MAG1 	1
#define EI_MAG2 	2
#define EI_MAG3 	3
#define EI_CLASS 	4
#define EI_DATA 	5
#define EI_VERSION 	6
#define EI_PAD 	7

#define ELFMAG0 	0x7f
#define ELFMAG1 	'E'
#define ELFMAG2 	'L'
#define ELFMAG3 	'F'

#define ELFCLASSNONE 	0
#define ELFCLASS32 	1
#define ELFCLASS64 	2

#define ELFDATANONE 	0
#define ELFDATA2LSB 	1
#define ELFDATA2MSB 	2

/* ########## Sections ########## */

/* Special Section Indexes */
#define SHN_UNDEF 	0
#define SHN_LORESERVE 	0xff00
#define SHN_LOPROC 	0xff00
#define SHN_HIPROC 	0xff1f
#define SHN_ABS 	0xfff1
#define SHN_COMMON 	0xfff2
#define SHN_HIRESERVE 	0xffff

/* Section Header */
typedef struct Elf32_Shdr
	{
	Elf32_Word name;
	Elf32_Word type;
	Elf32_Word flags;
	Elf32_Addr addr;
	Elf32_Off offset;
	Elf32_Word size;
	Elf32_Word link;
	Elf32_Word info;
	Elf32_Word addralign;
	Elf32_Word entsize;
	} Elf32_Shdr;

/* Section Types */
#define SHT_NULL 	0
#define SHT_PROGBITS 	1
#define SHT_SYMTAB 	2
#define SHT_STRTAB 	3
#define SHT_RELA 	4
#define SHT_HASH 	5
#define SHT_DYNAMIC 	6
#define SHT_NOTE 	7
#define SHT_NOBITS 	8
#define SHT_REL 		9
#define SHT_SHLIB 	10
#define SHT_DYNSYM 	11
#define SHT_LOPROC 	0x70000000
#define SHT_HIPROC 	0x7fffffff
#define SHT_LOUSER 	0x80000000
#define SHT_HIUSER 	0xffffffff

/* Section Attribute Flags */
#define SHF_WRITE 	0x1
#define SHF_ALLOC 	0x2
#define SHF_EXECINSTR 	0x4
#define SHF_MASKPROC 	0xf0000000

/* ########## Symbol Table ########## */

#define STN_UNDEF 	0

/* Symbol Table Entry */
typedef struct Elf32_Sym
	{
	Elf32_Word name;
	Elf32_Addr value;
	Elf32_Word size;
	unsigned char info;
	unsigned char other;
	Elf32_Half shndx;
	} Elf32_Sym;

#define ELF32_ST_BIND(i) 	((i) >> 4)
#define ELF32_ST_TYPE(i) 	((i) & 0xf)
#define ELF32_ST_INFO(b, t) 	((b) << 4) | ((t) & 0xf))

/* Symbol Binding */
#define STB_LOCAL 	0
#define STB_GLOBAL 	1
#define STB_WEAK 	2
#define STB_LOPROC 	13
#define STB_HIPROC 	15

/* Symbol Types */
#define STT_NOTYPE 	0
#define STT_OBJECT 	1
#define STT_FUNC 	2
#define STT_SECTION 	3
#define STT_FILE 	4
#define STT_LOPROC 	13
#define STT_HIPROC 	15

/* ########## Relocation ######### */

/* Relocation Entries */
typedef struct Elf32_Rel
	{
	Elf32_Addr offset;
	Elf32_Word info;
	} Elf32_Rel;

typedef struct Elf32_Rela
	{
	Elf32_Addr offset;
	Elf32_Word info;
	Elf32_Sword addend;
	} Elf32_Rela;

#define ELF32_R_SYM(i) 	((i) >> 8)
#define ELF32_R_TYPE(i) 	((unsigned char)(i))
#define ELF32_R_INFO(s, t) 	(((s) << 8) | (unsigned char)(t))

#define R_386_NONE 	0
#define R_386_32 	1
#define R_386_PC32 	2
#define R_386_GOT32 	3
#define R_386_PLT32 	4
#define R_386_COPY 	5
#define R_386_GLOB_DAT 	6
#define R_386_JMP_SLOT 	7
#define R_386_RELATIVE 	8
#define R_386_GOTOFF 	9
#define R_386_GOTPC 	10

/* ########## Program Header ########## */

/* Program Header */
typedef struct  Elf32_Phdr
	{
	Elf32_Word type;
	Elf32_Off offset;
	Elf32_Addr vaddr;
	Elf32_Addr paddr;
	Elf32_Word filesz;
	Elf32_Word memsz;
	Elf32_Word flags;
	Elf32_Word align;
	} Elf32_Phdr;

/* Segment Types */
#define PT_NULL 	0
#define PT_LOAD 	1
#define PT_DYNAMIC 	2
#define PT_INTERP 	3
#define PT_NOTE 	4
#define PT_SHLIB 	5
#define PT_PHDR 	6
#define PT_LOPROC 	0x70000000
#define PT_HIPROC 	0x7fffffff

#define PHF_EXEC 	0x1
#define PHF_WRITE 	0x2
#define PHF_READ 	0x4

#endif
