/*
  HOOK BOOT debugging tool
 */

#include "common.h"

#define __VERBOSE__ 1

PSP_MODULE_INFO("HOOK BOOT 0.94", 0x1000, 1, 1);   // 0x1000 = Kernel MODE
PSP_MAIN_THREAD_ATTR(0); // 0 for kernel mode too

#define CONFIG_NAME "hookboot.ini"

const char hbar_str[] = "-------------------------------------------------------\n";
#define HBAR() printf(hbar_str)

void _sceKernelIcacheClearAll(void);
int exec_module;

/////////////////////////////////////////////////////////////////////////////
// pre-patch KILL NID & mark ENTRY address
// return filesize
/////////////////////////////////////////////////////////////////////////////
int pre_patch(void)
{
	int dst_fd;
	int file_size;
#if MODULE_HOOK
	t_elf elElf;
	int num_hook;
#endif
	int file_size_o;
	unsigned char *szBuffer;

	// non module hook?
	if(get_num_hook(HOOK_TYPE_MODULE)==0 && !strchr(cfg_target_path,'@') )
	{
		// set file length only
		file_size = get_file_size(cfg_target_path);
		return file_size;
	}

#if 1//MODULE_HOOK
	// Module Mode pre-patch
	// kill file first
	file_size   = -2;
	file_size_o = -1;
	dst_fd = sceIoOpen(cfg_patch_path,PSP_O_RDONLY , 0);

	file_size   = get_file_size(cfg_patch_path);
	file_size_o = get_file_size(cfg_target_path);

	if( (file_size>0) && (file_size == file_size_o) )
	{
		printf("Bypass pre-patch program\n");
		return file_size;
	}

	// re-build patch file
	sceIoRemove(cfg_patch_path);

	// LOADING source program
	szBuffer = load_file(cfg_target_path,&file_size);

#if MODULE_HOOK
	if(get_num_hook(HOOK_TYPE_MODULE)>0)
	{
		// pre-patch program

		printf("kill NID & pre-patch SYSTEM CALL entryes\n");
		if(szBuffer==0)
		{
			printf("error can't load %s\n",cfg_target_path);
			return 0;
		}

		// check ELF
		elElf.base = szBuffer;
		if(elf_loadElf(&elElf)<0)
		{
			printf("error can't analyse elf\n");
			free(szBuffer);
			return 0;
		}

		// PRE-HOOK NID
		printf("MARK & HOOK NID\n");
		num_hook = psp_patch_NID(&elElf);

		printf("%d SYSTEM CALL hooked\n",num_hook);
		elf_closeElf(&elElf);
	}
#endif

	// write file & close
	printf("write program to '%s'\n",cfg_patch_path);
	dst_fd = sceIoOpen(cfg_patch_path,PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
	if(dst_fd < 0)
	{
		printf("Can't open %s\n",cfg_patch_path);
	}
	else
	{
		sceIoWrite(dst_fd,szBuffer ,file_size);
		sceIoClose(dst_fd);
	}

	// purge work buffer
	free(szBuffer);

	return file_size;
#endif
}

void wait_exit(void)
{
	printf("\n\nPUSH CIRCLE to EXIT\n\n");
	wait_key();
	pspDebugInstallKprintfHandler(NULL);
	sceKernelExitGame();
}

int sceKernelDevkitVersion(void);

//////////////////////////////////////////////////////////////////////////////////////
// ENTRY
//////////////////////////////////////////////////////////////////////////////////////

// DevKit r3
extern const char* g_elf_name;
extern int g_elf_namesize;

static char *argp[2];

int main(int argc, char *argv[])
{
	int i;
	unsigned char *freePtr;
	int filesize;
	int num_hook;
	int result;
	int num_m,num_s;

	//sceIoUnassign("disc0");
	//sceIoAssign("disc0:", "msstor0p1:/" , "fatms0:/" , 0x00, NULL, 0);

#if 0
	// get big heap to load program area 20MB
	patch_file_buf = calloc(20*1024*1024); // 20MB patch buf & load buf
	cfg_file_buf   = malloc(1024*1024); // 1MB
#endif

	sceCtrlSetSamplingCycle( 0 );
	sceCtrlSetSamplingMode( 0 );
	pspDebugScreenInit();
	printf("HOOK BOOT Ver.0.94 SemiFinal\n");

	// initialize kernel API entry
	init_kernel_api();

	//
	psp_devkit_ver = sceKernelDevkitVersion();
	printf("PSP Devkit version %08X\n",psp_devkit_ver);

	make_dir_path(psp_boot_path,g_elf_name);
//	if (argc > 0)
//		make_dir_path(psp_boot_path,argv[0]);
	printf("root path '%s'\n",psp_boot_path);

	// check heap point
	freePtr = malloc(256);
	for(i=0;i<1024;i++)
		freePtr[i] = 0xcc;
	free(freePtr);
	// align 256
	freePtr = (unsigned char *)((((int)freePtr)+0xff)&0xffffff00);

	printf("FREE MEMORY TOP:%08X\n",(int)freePtr);

	HBAR();
	sceKernelDelayThread(1000000);

	// set root path
	sceIoChdir(psp_boot_path);

	// load cofigration
	if(!cfg_load(CONFIG_NAME))
	{
		printf("Config Load Error\n");
		wait_exit();
	}

#if 0
	// KernelExportVersionPatch
	printf("KernelExportVersionPatched %d\n",psp_lib_patch(PSP_KMEM_TOP,PSP_KMEM_SIZE,PSP_DRV_TYPE_EXPORT,0x00,0x12));
	wait_key();
#endif
	// kernel version patch
	//patch_kernel_version(0x0101,0x0102,0x01100100);


	//set to ...
//	sceIoChdir(cfg_umd0_path);
	HBAR();

	// TITLE , PARGET
	printf("TITLE :%s\n",cfg_title);
	printf("TARGET:'%s'\n",cfg_target_path);
	printf("PATCH :'%s'\n",cfg_patch_path);

	// PATH ASSIGN
	char *ptr;

	//printf("PATH REPLACE----->\n");
	ptr = path_list;
	while(ptr[0] != 0x00)
	{
		printf("PATH:'%s'=",ptr);
		ptr += strlen(ptr)+1;
		printf("'%s'\n",ptr);
		ptr += strlen(ptr)+1;
	}
	//printf("<-----PATH REPLACE\n");

	num_m = get_num_hook(HOOK_TYPE_MODULE);
	num_s = get_num_hook(HOOK_TYPE_SYSCALL);
	printf("SYSCALL VECTOR:%08X\n",(int)cfg_sysvec_addr);
	if(cfg_sysvec_addr && check_free_vector()==0)
	{
		printf("Do not compare SYSTEMCALL vector !!!!!\n");
		if(num_s) wait_exit();
		cfg_sysvec_addr = 0;
	}

	// hook nums
	if(num_m==0 && strchr(cfg_target_path,'@')==0)
	{
		// if no-module hook then bypass PATCHED file
		strcpy(cfg_patch_path,cfg_target_path);
	}

	// SYSTEMCALL / VECTOR CHECK
	if(num_s>0 && (cfg_sysvec_addr==0))
	{
		printf("Error:Can't hook SYSTEMCALL because unknown VECTOR address\n");
		wait_exit();
	}

	// SHOW request hooks
	if(num_m || num_s)
	{
		printf("request hook (MODULE/SYSCALL): %3d/%3d\n",num_m,num_s);
	}
	else
	{
		printf("error NO HOOK select\n");
		wait_exit();
	}

	// UMD emu mode
	printf("UMD EMULATION MODE:%s\n",mode_umd_emu ? "ON" : "OFF");

	// log mode
	printf("LOG OUT DEVICE:%s%s\n",cfg_log_val&1 ? "DISP " : "",cfg_log_val&2 ? "FILE " : "");
	log_set_device(cfg_log_val);

	HBAR();

	// mound UMD
	//if(mode_umd_emu==0)
	sceIoAssign("disc0:","umd0:","isofs:",1,0,0);

	// PRE-PATCH
	filesize = pre_patch();
	if(filesize==0)
	{
		printf("pre-patch error\n");
		wait_exit();
	}
	// LOAD PROGRAM
	printf("Loading PROGRAM:");
	exec_module = sceKernelLoadModule(cfg_patch_path, 0, 0);
	if(exec_module>=0) printf("OK\n");
	else
	{
		printf("NG[%08X]\n",exec_module);
		wait_exit();
	}

	// start UMD & loadModule emulation
	printf("HOOK MODULE PATCH entries\n");
	num_hook = func_hook_start( (unsigned int *)freePtr,filesize);
	printf("%d SYSTEM CALL hooked\n",num_hook);

	// kernel version patch
	if(cfg_devkit_ver)
	{
		printf("devkit version:");
		if( patch_devkit_version(cfg_devkit_ver) )
			printf("%08X->%08X\n",psp_devkit_ver,cfg_devkit_ver);
		else
			printf("Can't patch\n");
	}
	// kernel driver version patch
	if(cfg_kernel_ver)
	{
		int num = patch_kernel_version(cfg_kernel_ver);
		printf("Kernel Driver version %d patched\n",num);
	}

	// KernelImportVersionPatch
	if(cfg_import_hack_ver)
	{
		printf("ImportDriverVersionPatched %d\n",psp_lib_patch((unsigned int)freePtr,filesize,PSP_DRV_TYPE_IMPORT,cfg_import_hack_ver,cfg_import_set_ver));
	}

	HBAR();

#if 0
	// start capture KPrintf
	pspDebugInstallKprintfHandler(log_kprint_handler);
	sceKernelLoadModule("test:/this/is/not/a/file.prx", 0, NULL);
#endif

	if(cfg_wait_run)
	{

		printf("***** PUSH CIRCLE to RUN *****\n\n");
		if(! (wait_key() & PSP_CTRL_CIRCLE ))
		{
			func_hook_stop();
			wait_exit();
			return 0;
		}
	}

	// clear screen
	pspDebugScreenInit();
	printf("RUN\n");

	// RUN target
	//result=sceKernelStartModule(exec_module, 0, 0, 0, 0);
	argp[0] = cfg_target_path;
	argp[1] = 0;

	result=sceKernelStartModule(exec_module, 1, argp, 0, 0);
	if(result < 0)
	{
		printf("Can't start program result=%08X\n",result);
		func_hook_stop();
		wait_exit();
		return 0;
	}
	log_printf("Program Started\n");
	return 0;
}

#if 0
void _exit(void)
{
}
#endif

