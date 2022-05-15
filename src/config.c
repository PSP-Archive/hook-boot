/*
 *HOOK BOOT debugging tool
 */

#include "common.h"

#define __VERBOSE__ 0

/////////////////////////////////////////////////////////////////////////////
// extern value
/////////////////////////////////////////////////////////////////////////////

unsigned int psp_devkit_ver;
char psp_boot_path[MAX_FILE_PATH];

char cfg_title[80];
char cfg_target_path[MAX_FILE_PATH];
char cfg_patch_path[MAX_FILE_PATH];

unsigned int cfg_log_val;
unsigned int cfg_sysvec_addr;

unsigned int cfg_kernel_ver;
unsigned short cfg_devkit_ver;

unsigned char cfg_import_hack_ver;
unsigned char cfg_import_set_ver;

unsigned int cfg_wait_run;

/////////////////////////////////////////////////////////////////////////////
// internal
/////////////////////////////////////////////////////////////////////////////
static char config_path[MAX_FILE_PATH];

/////////////////////////////////////////////////////////////////////////////
// string 
/////////////////////////////////////////////////////////////////////////////

enum{
	SECTION_NID,
	SECTION_KPATCH,
//
	SECTION_TITLE,
	SECTION_UMD_MODE,
//
	SECTION_PATH_ASSIGN,
	SECTION_TARGET_PATH,
	SECTION_CONFIG_PATH,
	SECTION_PATCH_PATH,
	SECTION_LOG_DEVICE,
	SECTION_SYSTEM_VECTOR,
	SECTION_DEVKIT_VER,
	SECTION_KERNEL_VER,
	SECTION_IMPORT_VER,
	SECTION_WAIT_RUN,
//
	SECTION_MAX
};

const char *section_str[]= {
	"NID",
	"KPATCH",
//
	"TITLE",
	"UMD_EMU",
//
	"ASSIGN_PATH",
	"TARGET_PATH",
	"CONFIG_PATH",
	"PATCH_PATH",
	"LOG_DEVICE",
	"SYSCALL_VECTOR",
	"DEVKIT_VER",
	"KERNEL_VER",
	"IMPORT_VER",
	"WAIT_RUN",
	NULL
};

const char *log_str[]= {
	"DISP",
	"FILE",
	"##",
	"##",
	"##",
	"##",
	"##",
	"DELAY",
//
	NULL
};

const char *hook_type_str[]= {
	"N",
	"M",
	"S",
//
	NULL
};

/////////////////////////////////////////////////////////////////////////////
// ANALYZE CONFIG 
/////////////////////////////////////////////////////////////////////////////
static int cfg_analyze(int num_args,char **args)
{
	int section;
	int hook_type;
	char tmp_path[MAX_FILE_PATH];

	unsigned int addr , odata , pdata;

	section = str2num(section_str,args[0]);
	if(section<0)
	{
		printf("UNKMOWN SECTION '%s'\n",args[0]);
		return -1;
	}
#if 0
	printf("SECTION '%s'\n",section_str[section]);
#endif
	switch(section)
	{
	case SECTION_TITLE:
		if(num_args<2) break;
		strcpy(cfg_title,args[1]);
		return 0;
	case SECTION_UMD_MODE:
		if(num_args<2) break;
		set_umd_emulation_mode( str2hex(args[1]) );
		return 0;

	case SECTION_PATH_ASSIGN:
		if(num_args<3) break;

		make_full_path(tmp_path,psp_boot_path,args[2]);
		if(!add_path_list(args[1]) || !add_path_list(tmp_path) )
			return -1; // buffer FULL
		return 0;

	case SECTION_TARGET_PATH:
		if(num_args<2) break;
		make_full_path(cfg_target_path,psp_boot_path,args[1]);
		return 0;
	case SECTION_CONFIG_PATH:
		if(num_args<2) break;
		make_full_path(config_path,psp_boot_path,args[1]);
		return 0;
	case SECTION_PATCH_PATH:
		if(num_args<2) break;
		make_full_path(cfg_patch_path,psp_boot_path,args[1]);
		return 0;
	case SECTION_LOG_DEVICE:
		if(num_args<2) break;
		cfg_log_val |= 1<<str2num(log_str,args[1]);
		return 0;
	case SECTION_SYSTEM_VECTOR:
		if(num_args<2) break;
		if(num_args>=3)
		{
			unsigned int devkit_ver = str2hex(args[2]);
			if( psp_devkit_ver != devkit_ver)
				return 0;
		}
		cfg_sysvec_addr = str2hex(args[1]);
		return 0;
	case SECTION_DEVKIT_VER:	// version
		if(num_args<2) break;
		cfg_devkit_ver = (unsigned short)str2hex(args[1]);
		return 0;
	case SECTION_KERNEL_VER:	// kernel driver version
		if(num_args<2) break;
		cfg_kernel_ver = str2hex(args[1]);
		return 0;
	case SECTION_IMPORT_VER:	// driver import version
		if(num_args<3) break;
		cfg_import_hack_ver = str2hex(args[1]);
		cfg_import_set_ver  = str2hex(args[2]);
		return 0;

	case SECTION_WAIT_RUN: // wait button before run
		if(num_args<2) break;
		cfg_wait_run = str2hex(args[1]);
		return 0;

	case SECTION_NID:
		if(num_args<2) break;
		// mode
		if(num_args<3) hook_type = HOOK_TYPE_MODULE; // module
		else hook_type = str2num(hook_type_str,args[2]); // N / M / S
		if(hook_type<0 || hook_type>=HOOK_TYPE_MAX) break;

		// set
		set_hook_type( str2hex(args[1]) , hook_type);

		return 0;
	case SECTION_KPATCH: // KPATCH addr,org_data,patch_data,[kernel_devkit_ver]
		if(num_args<4) break;
		addr = str2hex(args[1]);
		odata = str2hex(args[2]);
		pdata = str2hex(args[3]);
		if(num_args>4)
		{
			unsigned int devkit_ver = str2hex(args[4]);
			if(psp_devkit_ver != devkit_ver)
			{
				return 0;
			}
		}
		if( *(unsigned int *)addr != odata)
		{
printf("NO PATCH %08X MEM=%08X : REQ=%08X\n",(unsigned int)addr,*(unsigned int *)addr,odata);
			return 0;
		}
		*(unsigned int *)addr = pdata;
printf("PATCH %08X=%08X->%08X\n",addr,odata,pdata);
		return 0;
	}
#if 1
	printf("PARAMETER ERROR\n");
#endif
	return -2;
}

/////////////////////////////////////////////////////////////////////////////
// load & get config file
/////////////////////////////////////////////////////////////////////////////
int cfg_load(const char *path)
{
	unsigned char *file_buf , *ptr;

	int num_args;
	char *args[10];

	// initialize configs
	cfg_target_path[0] = 0;
	cfg_patch_path[0] = 0;
	init_path_list();

	// NID
	set_hook_type(0xffffffff,HOOK_TYPE_NOHOOK);

	cfg_kernel_ver = 0;
	cfg_devkit_ver = 0;
	cfg_import_hack_ver = 0;
	cfg_import_set_ver  = 0;

	cfg_log_val = 0;
	cfg_sysvec_addr = 0;

	cfg_wait_run = 0;

	while(path[0]!=0)
	{
		printf("LOAD CONFIG '%s'\n",path);

		// load generic file
		file_buf = ptr = load_file(path,NULL);
		if(!ptr)
		{
			printf("Load Config file %s\n",path);
			return 0;
		}
		// next file clear
		config_path[0] = 0;
		path = config_path;

		// analyze config gile
		do
		{
			// analyze line
			ptr=get_line_args(ptr,&num_args,args);

			if(num_args==0)
			{
				printf("syntax error\n");
				continue;
			}
			// command
			switch(args[0][0])
			{
			case 0x00:
			case ';':
			case '/':
			case '#':
				break;
			default:
				if(cfg_analyze(num_args,args)<0)
					break;
			}
		}while(ptr);
		free(file_buf);

	}
	return 1;
}

