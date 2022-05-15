
int get_word(unsigned char *ptr);
int get_dword(unsigned char *ptr);

int psp_patch_NID(t_elf *elElf);

Elf32_Shdr *search_section(t_elf *elElf,const char *section_name);

typedef struct psp_import_lib
{
	unsigned int   offset_name;	// +00
	unsigned char  version;		// +07
	unsigned char  unk_02;		// +06
	unsigned short flags;		// +04
	unsigned short unk_08;		// +08
	unsigned short num_funcs;	// +10
	unsigned int   offset_nid;	// +12
	unsigned int   offset_ent;	// +16
}PSP_IP_LIB;


