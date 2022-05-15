/*

.lib.stub              : ライブラリリンクリスト、top.btmで挟んである
.rodata.stubmodulename : "DW 00000000,モジュール名"の羅列(セクションは不問）

.text.stub             : FUNCエントリ(jr $ra + nop)
.rodata.stubidtable    : FUNC-NiDの羅列

// import libraly
struct lib_stub
{
	char  *p_module_name  //module名へのポインタ
	int16 flags1;         //0000
	int16 flags2;         //4001
	int16 0005;           //
	int16 num_funcs;      //
	int32 *pNID;          //NIDへのポインタ
	funcc_list *pfunc;    //プログラムのダイレクトエントリポイント
};

*/

#include "common.h"

//#define SHOW_LIB_STAT
//#define SHOW_FUNC_STAT

int get_word(unsigned char *ptr)
{
	return ptr[0] + (ptr[1]<<8);
}

int get_dword(unsigned char *ptr)
{
	return ptr[0] + (ptr[1]<<8) + (ptr[2]<<16) + (ptr[3]<<24);
}

#if MODULE_HOOK

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////

Elf32_Shdr *search_section(t_elf *elElf,const char *section_name)
{
	int i;
	Elf32_Shdr *sec_hdr;
	char *sec_name;

	for(i=0;i<elElf->elf_hdr->shnum;i++)
	{
		sec_hdr  = elElf->elfsecs[i].sec_hdr;
		sec_name = elElf->sechdr_strtbl+sec_hdr->name;
		if( str_cmp(sec_name,section_name)==0)
		{
			return sec_hdr;
		}
	}
	return NULL;
}

int get_program_offset(t_elf *elElf,int addr)
{
	int i;
	Elf32_Phdr *prg_hdr;
	int start , size;

	for(i=0;i<elElf->elf_hdr->phnum;i++)
	{
		prg_hdr = elElf->elfprgs[i].prg_hdr;
		start = (int)prg_hdr->vaddr;
		size  = prg_hdr->filesz;

		if( (start<=addr) && ((start+size)>=addr) )
		{
			return addr - start + prg_hdr->offset;
		}
	}
	return -1;
}

//
// ELFのファンクションエントリ、RET (J RA12) + NOP  にNIDを埋め込むパッチ
//
int psp_patch_NID(t_elf *elElf)
{
	int i,j;
	Elf32_Shdr *sec_hdr;
	int offset , size;
	PSP_IP_LIB *lib_hdr;
	int num_hook = 0;

	//int lib_name_pos,lib_flags,unk8,nums,lib_nid_pos,lib_ent_pos;

	unsigned char *lib_name , *nid_data , *enter_data;
	int name_file_pos,nid_file_pos,ent_file_pos;

	// get STUB section
	sec_hdr = search_section(elElf,".lib.stub");
	if(!sec_hdr)
	{
		printf("section '.lib.stub' not found\n");
		return 0;
	}

	// get section data
	offset = sec_hdr->offset;
	size   = sec_hdr->size;

#ifdef SHOW_LIB_STAT
	printf("|ver|uk|flag|unk8|nums|  pNID  | pENTER |name\n");
#endif
	for(i=0;i<size;i+=20)
	{
		// get import header
		elf_getData(elElf,(unsigned char **)(&lib_hdr),offset+i,20);

		// get data NAME,NID list,ENTRY point list
		name_file_pos = get_program_offset(elElf,lib_hdr->offset_name);
		elf_getData(elElf,&lib_name,name_file_pos,256);
		nid_file_pos = get_program_offset(elElf,lib_hdr->offset_nid);
		elf_getData(elElf,&nid_data  , nid_file_pos ,4*lib_hdr->num_funcs);
		ent_file_pos = get_program_offset(elElf,lib_hdr->offset_ent);
		elf_getData(elElf,&enter_data,ent_file_pos ,8*lib_hdr->num_funcs);

#ifdef SHOW_LIB_STAT
		printf("| %02X|%02X|%04X|%4X|%4d|%08X|%8X|%s\n",
			lib_hdr->version,
			lib_hdr->unk_02,
			lib_hdr->flags,
			lib_hdr->unk_08,
			lib_hdr->num_funcs,
			lib_hdr->offset_nid,
			lib_hdr->offset_ent,
			lib_name);
#endif

#if 0
		log_printf("Import LIB:%s Ver.%02X",lib_name,lib_hdr->version);
		// down version
		if(lib_hdr->version != 0)
		{
			log_printf("->00\n",lib_hdr->version);
			lib_hdr->version = 0x00;
		}
		else log_printf("\n");
#endif

#ifndef MODE_PSP
		// on PC patch
		for(j=0;j<lib_hdr->num_funcs;j++)
		{
			int nid = get_dword(nid_data+j*4);
#ifdef SHOW_FUNC_STAT
			printf("NID POS[%08x]=%08X , ENT POS[%08X]=%08X,%08X\n"
				,nid_file_pos+4*j,nid
				,ent_file_pos+8*j,get_dword(enter_data+j*8),get_dword(enter_data+j*8+4)
			);
#endif
			// insert NID to ENTRY POINT
			enter_data[j*8+4] = nid_data[j*4];
			enter_data[j*8+5] = nid_data[j*4+1];
			enter_data[j*8+6] = nid_data[j*4+2];
			enter_data[j*8+7] = nid_data[j*4+3];
		}
#else // MINGW

		// ON PSP patch
		// show function pointers
		for(j=0;j<lib_hdr->num_funcs;j++)
		{
			int nid = get_dword(nid_data+j*4);
			const HOOK_LIST *func;
			const char *result_str;

			func = search_hook_nid(nid);
			if(!func)
			{
				// NOT FOUND
#ifdef SHOW_FUNC_STAT
//				printf("NID FILE[%08x]=%08X , ENT FILE[%08X]=%08X,%08X :"
//					,nid_file_pos+4*j,nid
//					,ent_file_pos+8*j,get_dword(enter_data+j*8),get_dword(enter_data+j*8+4)
//				);
#endif
				log_printf("UNKNOWN NID=%08X\n",nid);
			}
			else
			{
				if(func->hook_func)
				{
					// FOUND NID & HOOK FUNC
					switch(get_hook_type(nid))
					{
					case HOOK_TYPE_MODULE:
						// insert NID to ENTRY POINT
						enter_data[j*8+4] = nid_data[j*4];
						enter_data[j*8+5] = nid_data[j*4+1];
						enter_data[j*8+6] = nid_data[j*4+2];
						enter_data[j*8+7] = nid_data[j*4+3];
#if 1
						// KILL NID
						nid_data[j*4]   = 0;
						nid_data[j*4+1] = 0;
						nid_data[j*4+2] = 0;
						nid_data[j*4+3] = 0;
#endif
						// count
						num_hook++;
						result_str = "HOOK MARKING";
						break;
					default:
						result_str = "HOOK BYPASS ";
					}
				}
				else result_str = "NO-SUPPORTED";
				// result
				log_printf("%08X %s offset=%08X (%s)\n",nid,result_str,ent_file_pos+j*8,func->func_name);
			}
		}
#endif // MINGW
	}
	return num_hook;
}
#endif
