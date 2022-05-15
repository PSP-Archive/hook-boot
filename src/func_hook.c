//
// systemcal1
//
#include "common.h"
#include <stdarg.h>

#if MIN_BUILD
#define LOG_OUTPUT 0
#else
#define LOG_OUTPUT 1
#endif

#define UMD_BLOCK_MODE 1 // ISO9660 fs & BLOCK MODE ACCESS

//#define WAIT() sceKernelDelayThread(1000);

////////////////////////////////////////////////////////////////////
// prottype
////////////////////////////////////////////////////////////////////
int sceKernelLoadModuleByID(int id, u32 zero, mod_param_t* mod);
int sceKernelStopModule(int modid, u32 arglen, void* args, u32 *status, u32 zero);
int sceUmdGetErrorStat(void);
int sceIoGetAsyncStat(int fd, int poll, long long *res);
int sceIoWriteAsync(int fd, const void *data, int size);
int sceIoReadAsync(int fd, void *data, int size);
int sceIoCloseAsync(int fd);
unsigned int sceIoLseek32Async(int fd, unsigned int offset, int whence);
int sceUmdGetDiscInfo(int unk1,int unk2);
int sceUmdWaitDriveStatWithTimer(int delay);
int sceUmdDeactivate(int unint,const char *drive);
int sceUmdWaitDriveStat(int a);
int sceUmdWaitDriveStatCB(void);

#if 0
// arg1 = count , arg2 = event
typedef int (*SceKernelCallbackFunction)(int arg1, int arg2, void *arg);
#endif

////////////////////////////////////////////////////////////////////
// hame OS API FUNC ENTRY & HOOK FUNC ENTRY
////////////////////////////////////////////////////////////////////

#define HOOK_FUNC(RET_TYPE,NAME,ARGS) \
RET_TYPE (*os_##NAME) ARGS;\
static RET_TYPE hook_##NAME ARGS

#define HOOK_FUNC_(RET_TYPE,NAME,ARGS) \
static RET_TYPE hook_##NAME ARGS

////////////////////////////////////////////////////////////////////
// API NAME string
////////////////////////////////////////////////////////////////////

// API NAME
#if LOG_OUTPUT

#define HOOK_ENTRYE(NID,NAME) const char name_##NAME[]=#NAME;
#define HOOK_ENTRYP(NID,NAME) const char name_##NAME[]=#NAME;
#define HOOK_ENTRY_(NID,NAME) const char name_##NAME[]=#NAME;
#include "api_list.h"
#undef HOOK_ENTRYE
#undef HOOK_ENTRYP
#undef HOOK_ENTRY_

#else

static const char str_no_name[] = "noname";
#define HOOK_ENTRYE(NID,NAME) const char name_##NAME[]="";
#define HOOK_ENTRYP(NID,NAME) const char name_##NAME[]="";
#define HOOK_ENTRY_(NID,NAME) const char name_##NAME[]="";
#include "api_list.h"
#undef HOOK_ENTRYE
#undef HOOK_ENTRYP
#undef HOOK_ENTRY_

#endif

////////////////////////////////////////////////////////////////////
// ...
////////////////////////////////////////////////////////////////////
unsigned char func_hook_type[];

////////////////////////////////////////////////////////////////////
// mode
////////////////////////////////////////////////////////////////////
int mode_umd_emu = 1;

////////////////////////////////////////////////////////////////////
// sub routines
////////////////////////////////////////////////////////////////////

// get string
static char str_addr_buf[9];

// (string or number) to string
const char *get_str(const char *str)
{
	if( is_str(str) ) return str;
	//
	sprintf(str_addr_buf,"%08X",(unsigned int)str);
	return str_addr_buf;
}

#if UMD_BLOCK_MODE
static int check_file_emu(int fd)
{
	FILE_EMU_STATE *state = get_file_emu_state(fd);
	if(state)
	{
		log_printf("Do not supported ISO96660 fs and block access emulation\n");
		return 1;
	}
	return 0;
}
#endif

////////////////////////////////////////////////////////////////////
// function entry / return log
////////////////////////////////////////////////////////////////////
#if LOG_OUTPUT

// return hex
static void log_ret_h(int ret)
{	log_printf("RET(%08X)\n",ret); }

// return long long int HEX
static void log_ret_ll(long long int ret)
{	log_printf("RET(%08X%08X)\n",(int)(ret>>32),(int)(ret)); }

static void log_s(const char *str,const char *name)
{	log_printf("%s(%s):",name,get_str(str)); }

static void log_s_h(const char *str,int hex,const char *name)
{	log_printf("%s(%s,%08X):",name,get_str(str),hex); }

static void log_s_h_h(const char *str,int hex1,int hex2,const char *name)
{	log_printf("%s(%s,%08X,%08X):",name,get_str(str),hex1,hex2);}

#if 0
static void log_s_h_h_h(const char *str,int hex1,int hex2,int hex3,const char *name)
{	log_printf("%s(%s,%08X,%08X,%08X):",name,get_str(str),hex1,hex2,hex3); }
#endif

static void log_s_h_h_h_h_h(const char *str,int hex1,int hex2,int hex3,int hex4,int hex5,const char *name)
{	log_printf("%s(%s,%08X,%08X,%08X,%08X,%08X):",name,get_str(str),hex1,hex2,hex3,hex4,hex5); }

static void log_h(int hex1,const char *name)
{	log_printf("%s(%08X):",name,hex1); }

static void log_h_h(int hex1,int hex2,const char *name)
{	log_printf("%s(%08X,%08X):",name,hex1,hex2); }

static void log_h_h_h(int hex1,int hex2,int hex3,const char *name)
{	log_printf("%s(%08X,%08X,%08X):",name,hex1,hex2,hex3); }

static void log_h_h_h_h_h(int hex1,int hex2,int hex3,int hex4,int hex5,const char *name)
{	log_printf("%s(%08X,%08X,%08X,%08X,%08X):",name,hex1,hex2,hex3,hex4,hex5); }

static void log_h_ll_h(int hex1,long long int llint1,int hex2,const char *name)
{	log_printf("%s(%08X,%08X%08X,%08X):",name,hex1,(int)(llint1>>32),(int)(llint1),hex2); }
////////////////////////////////

static void log_s_s_s_h_h_h(const char *str1,const char *str2,const char *str3,int hex1,int hex2,int hex3,const char *name)
{	log_printf("%s(%s,%s,%s,%08X,%08X,%08X):",name,get_str(str1),get_str(str2),get_str(str3),hex1,hex2,hex3); }

static void log_h_s(int hex,const char *str,const char *name)
{	log_printf("%s(%08X,%s):",name,hex,get_str(str)); }

//
static void log_v(const char *name)
{
	log_printf("%s():",name);
}
////////////////////////////////////////////////////////////////////

#else // LOG_OUTOUT 
#define log_ret_h(RET) {}
#define log_ret_ll(RET) {}
#define log_s(S1,N) {}
#define log_s_h(S1,H2,N) {}
#define log_s_h_h(S1,H2,H3,N) {}
#define log_str_hex_hex_hex(S,H1,H2,H3,H4,N) {}
#define log_s_h_h_h_h_h(S,H1,H2,H3,H4,H5,N) {}

#define log_h(H,N) {}
#define log_h_h(H1,H2,N) {}
#define log_h_h_h(H1,H2,H3,N) {}
#define log_h_h_h_h_h(H1,H2,H3,H4,H5,N) {}

#define log_h_ll_h(H1,L2,H3,N) {}

#define log_s_s_s_h_h_h(S1,S2,S3,H4,H5,H6,N) {}
#define log_h_s(H,S,N) {}
#define log_v(N) {}

#endif


////////////////////////////////////////////////////////////////////
// path transrator
////////////////////////////////////////////////////////////////////
#define PATH_HOOK_DISC 0x0001
#define PATH_HOOK_UMD0 0x0002
#define PATH_HOOK_UMD1 0x0004

static const char *makePath(char *hooked_path,const char *path)
{
	char *ptr , *path1 , *path2;

	if(!is_str(path)) return path;

	ptr = path_list;
	while(ptr[0] != 0x00)
	{
		path1 = ptr;
		ptr += strlen(path1)+1;
		path2 = ptr;
		ptr += strlen(path2)+1;

		// try replace path
		strcpy(hooked_path,path);
		if(str_rep(hooked_path,path1,path2))
		{
#if LOG_OUTPUT
			log_printf("[ChangePath->'%s']",hooked_path);
#endif

			// separate & mark FILE EMU MODE
			hooked_path[strlen(hooked_path)+1] = 0x00; // for block mode
			
			if( (ptr=strchr(hooked_path,'@')) != 0)
			{
				// ISO9660 mode , separate path in ISO 
				*ptr++=0x00; // '@' -> '\x00'
			}

			return hooked_path;
		}
	}
	return path;
}

////////////////////////////////////////////////////////////////////
// Driver Import Version Check
////////////////////////////////////////////////////////////////////
#define FIRST_BOTTOM_SEARCH 0x200000
#define ADDED_BOTTOM_SEARCH 0x020000

void import_ver_hack(void)
{
	if(cfg_import_hack_ver)
	{
		// kernel area
		if( psp_lib_patch(PSP_KMEM_TOP,PSP_KMEM_SIZE,PSP_DRV_TYPE_IMPORT,cfg_import_hack_ver,cfg_import_set_ver)==0)
		{
			// TRY user area
#if 0
			psp_lib_patch(PSP_UMEM_TOP,0,PSP_DRV_TYPE_IMPORT,cfg_import_hack_ver,cfg_import_set_ver);
#else
			// BOTTOM 2MB first
			if( psp_lib_patch(PSP_UMEM_END-FIRST_BOTTOM_SEARCH,0,PSP_DRV_TYPE_IMPORT,cfg_import_hack_ver,cfg_import_set_ver)>0)
			{
				// add more 128KB
				psp_lib_patch(PSP_UMEM_END-FIRST_BOTTOM_SEARCH-ADDED_BOTTOM_SEARCH,ADDED_BOTTOM_SEARCH,PSP_DRV_TYPE_IMPORT,cfg_import_hack_ver,cfg_import_set_ver);
			}
			else
			{
				// FULL SEARCH
				psp_lib_patch(PSP_UMEM_TOP,PSP_UMEM_END-PSP_UMEM_TOP-FIRST_BOTTOM_SEARCH,PSP_DRV_TYPE_IMPORT,cfg_import_hack_ver,cfg_import_set_ver);
			}
#endif
		}
	}
}

////////////////////////////////////////////////////////////////////
// MODULE MGR kernelmode TSR
////////////////////////////////////////////////////////////////////
extern int exec_module;
int main(void);

/*
power up   00 -> 02
activate   02 -> 12 -> 22
Deactivate 02 -> 0A 
activate   0A -> 22
*/
#define UMD_START    0x00
#define UMD_POWERUP  0x02
#define UMD_TRYACTIV 0x12
#define UMD_DEACTIVE 0x0A
#define UMD_ACTIVE   0x22

static int thid = 0;

// use !!semaphore!!
static char threadCMD;

// FOR LoadModule
static char mload_path[MAX_FILE_PATH];
static int mload_id;
static int mload_zero;
static mod_param_t *mload_mod;
static int mload_ret;

// FOR UMD callback

static volatile int umd_callback_id;
static volatile int umd_callback_count;
static volatile SceKernelCallbackFunction umd_callback_func = NULL;
static void *umd_callback_arg;
static volatile int umd_callback_request;
static int umd_stat = UMD_POWERUP;

////////////////////////////////////////////////////////////////////
// MODULE MGR HOOK
////////////////////////////////////////////////////////////////////

HOOK_FUNC_(int,sceKernelLoadModuleByID,(int id, u32 zero, mod_param_t* mod))
{
	int ret;

	log_h_h_h(id,zero,(int)mod,name_sceKernelLoadModuleByID);
//	ret = sceKernelLoadModuleByID(id,zero,mod);
//	if(ret < 0)
	{
		// use kernel thread
#if LOG_OUTPUT
//		log_printf("TRY with KernelThread\n");
#endif
		mload_id   = id;
		mload_zero = zero;
		mload_mod  = mod;
		// use semaphore
		set_kthread_cmd(CMD_LOAD_MODULE_BYID);
		ret = mload_ret;
	}
	log_ret_h(ret);
	return ret;
}

HOOK_FUNC_(int,sceKernelLoadModule,(const char *path, u32 zero, mod_param_t* mod))
{
	int ret;

	log_s_h_h(path,zero,(int)mod,name_sceKernelLoadModule);
	makePath(mload_path,path);

//	ret = sceKernelLoadModule(mload_path,zero,mod);
//	if(ret < 0)
	{
		// use kernel thread
#if LOG_OUTPUT
//		log_printf("TRY with KernelThread\n");
#endif
		mload_zero = zero;
		mload_mod = mod;
		// use semaphore
		set_kthread_cmd(CMD_LOAD_MODULE);
		ret = mload_ret;
	}
	log_ret_h(ret);
	return ret;
}

HOOK_FUNC(int,sceKernelStartModule,(int modid, u32 arglen, void* args, u32 *status, u32 zero))
{
	int ret;

	log_h_h_h_h_h(modid,arglen,(int)args,(int)status,zero,name_sceKernelStartModule);

	ret = os_sceKernelStartModule(modid,arglen,args,status,zero);
	log_ret_h(ret);
	return ret;
}

HOOK_FUNC_(int,sceKernelStopModule,(int modid, u32 arglen, void* args, u32 *status, u32 zero))
{
	log_h_h_h_h_h(modid,arglen,(int)args,(int)status,zero,name_sceKernelStopModule);
	int ret = sceKernelStopModule(modid,arglen,args,status,zero);
	log_ret_h(ret);
	return ret;
}

////////////////////////////////////////////////////////////////////
// FILE PATH HOOK
////////////////////////////////////////////////////////////////////

HOOK_FUNC_(int,sceIoGetAsyncStat,(int fd, int poll, long long *res))
{
	log_h_h_h(fd,poll,(int)res,name_sceIoGetAsyncStat);
	int ret = sceIoGetAsyncStat(fd,poll,res);
	log_ret_h(ret);
	return ret;
}

HOOK_FUNC_(int,sceIoClose,(int fd))
{
	log_h(fd,name_sceIoClose);
	int ret = sceIoClose(fd);
#if UMD_BLOCK_MODE
	file_mode_close(fd);
#endif
	log_ret_h(ret);
	return ret;
}

HOOK_FUNC_(int,sceIoCloseAsync,(int fd))
{
	log_h(fd,name_sceIoCloseAsync);
	int ret = sceIoCloseAsync(fd);
#if UMD_BLOCK_MODE
	file_mode_close(fd);
#endif
	log_ret_h(ret);
	return ret;
}

HOOK_FUNC_(int,sceIoOpen,(const char* file, int flags, int perms))
{
	char path_buf[MAX_FILE_PATH];
	const char *hooked_path;
	int fd;

	log_s_h_h(file,flags,perms,name_sceIoOpen);
	hooked_path = makePath(path_buf,file);
	fd = sceIoOpen(hooked_path,flags,perms);

#if UMD_BLOCK_MODE
	file_emu_open(hooked_path,fd,fd);
#endif
	log_ret_h(fd);
	return fd;
}

int sceIoWaitAsync(int,long long int *);

HOOK_FUNC_(int,sceIoOpenAsync,(const char* file, int flags, int perms))
{
	char path_buf[MAX_FILE_PATH];
	const char *hooked_path;
	int fd;

	log_s_h_h(file,flags,perms,name_sceIoOpenAsync);
	hooked_path = makePath(path_buf,file);

	fd = sceIoOpenAsync(hooked_path,flags,perms);
#if UMD_BLOCK_MODE
	file_emu_open(hooked_path,fd,0);
#endif
	//if(hooked_path == path_buf)
	sceKernelDelayThread(5000); // for MANCO MUSIUM , majong fight club
	log_ret_h(fd);
	return fd;
}

HOOK_FUNC_(int,sceIoRead,(int fd, void *data, int size))
{
	int ret;

	log_h_h_h(fd,(int)data,size,name_sceIoRead);
#if UMD_BLOCK_MODE
	FILE_EMU_STATE *state = get_file_emu_state(fd);
	if(state)
	{
		ret = sceIoRead(fd,data,size*state->secsize);
		if(ret>=0) ret /= state->secsize;
	}
	else
#endif
	ret = sceIoRead(fd,data,size);
	log_ret_h(ret);
#if 0
	// MetalGearAC!D
	if( (ret<0) && (fd==0xfffffffe) && size==0)
	{
		log_printf("HACK MGA\n");
		ret = 0;
	}
#endif

	return ret;
}

HOOK_FUNC_(int,sceIoReadAsync,(int fd, void *data, int size))
{
	int ret;

	log_h_h_h(fd,(int)data,size,name_sceIoReadAsync);
#if UMD_BLOCK_MODE
	FILE_EMU_STATE *state = get_file_emu_state(fd);
	if(state)
	{
		int read_size = sceIoRead(state->isofd,data,size*state->secsize);
		if(read_size>=0)
		{
			// return read size
			ret = sceIoLseek32Async(fd,read_size/state->secsize,PSP_SEEK_SET);
		}
		else ret = 0;
	}
	else
#endif
	ret = sceIoReadAsync(fd,data,size);
	log_ret_h(ret);
	return ret;
}

HOOK_FUNC_(int,sceIoWrite,(int fd, const void *data, int size))
{

	log_h_h_h(fd,(int)data,size,name_sceIoWrite);
	//if(mode_umd_emu) return 0;
	int ret = sceIoWrite(fd,data,size);

	if( (fd==1) && (size<128))
	{
		log_write(data,size);
	}
	log_ret_h(ret);
	return ret;
}

HOOK_FUNC_(int,sceIoWriteAsync,(int fd, const void *data, int size))
{
	log_h_h_h(fd,(int)data,size,name_sceIoWriteAsync);
	int ret = sceIoWriteAsync(fd,data,size);
	log_ret_h(ret);
	return ret;
}

long long int sceIoLseek(int fd, long long int offset, int whence);
HOOK_FUNC_(long long int,sceIoLseek,(int fd, long long int offset, int whence))
{
	long long int ret;

	log_h_ll_h(fd,offset,whence,name_sceIoLseek);

#if UMD_BLOCK_MODE
	FILE_EMU_STATE *state = get_file_emu_state(fd);
	if(state)
	{
		if(whence==PSP_SEEK_END)
		{
			offset = state->filesize - offset;
			whence = PSP_SEEK_SET;
		}
		ret = sceIoLseek(fd,offset*state->secsize + state->offset,whence);
		if(ret<0x80000000) ret = ret /state->secsize - state->offset;
	}
	else
#endif
	ret = sceIoLseek(fd,offset,whence);
	log_ret_ll(ret);
	return ret;
}

int sceIoLseekAsync(int fd, long long int offset, int whence);
HOOK_FUNC_(int,sceIoLseekAsync,(int fd, long long int offset, int whence))
{
	int ret;
	log_h_ll_h(fd,offset,whence,name_sceIoLseekAsync);

#if UMD_BLOCK_MODE
	FILE_EMU_STATE *state = get_file_emu_state(fd);
	if(state)
	{
		if(whence==PSP_SEEK_END)
		{
			offset = state->filesize - offset;
			whence = PSP_SEEK_SET;
		}
		sceIoLseek32(state->isofd,offset*state->secsize + state->offset,whence);
	}
#endif
	ret = sceIoLseekAsync(fd,offset,whence);

	log_ret_h(ret);
	return ret;
}

HOOK_FUNC_(unsigned int,sceIoLseek32,(int fd, unsigned int offset, int whence))
{
	int ret;
	log_h_h_h(fd,offset,whence,name_sceIoLseek32);

#if UMD_BLOCK_MODE
	FILE_EMU_STATE *state = get_file_emu_state(fd);
	if(state)
	{
		if(whence==PSP_SEEK_END)
		{
			offset = state->filesize - offset;
			whence = PSP_SEEK_SET;
		}
		ret = sceIoLseek32(fd,offset*state->secsize + state->offset,whence);
		if(ret>=0) ret = ret /state->secsize - state->offset;

	}
	else
#endif
	ret = sceIoLseek32(fd,offset,whence);
	log_ret_h(ret);
	return ret;
}

HOOK_FUNC_(unsigned int,sceIoLseek32Async,(int fd, unsigned int offset, int whence))
{
	unsigned int ret;
	log_h_h_h(fd,offset,whence,name_sceIoLseek32Async);

#if UMD_BLOCK_MODE
	FILE_EMU_STATE *state = get_file_emu_state(fd);
	if(state)
	{
		if(whence==PSP_SEEK_END)
		{
			offset = state->filesize - offset;
			whence = PSP_SEEK_SET;
		}
		sceIoLseek32(state->isofd,offset*state->secsize + state->offset,whence);
	}
#endif
	ret = sceIoLseek32Async(fd,offset,whence);

	log_ret_h(ret);
	return ret;
}

//////////////////////////////////////////////////////////////////////////////

HOOK_FUNC_(int,sceIoDopen,(const char *dirname))
{
	char path_buf[MAX_FILE_PATH];
	const char *hooked_path;
	int ret;
	log_s(dirname,name_sceIoDopen);
	hooked_path = makePath(path_buf,dirname);
	ret = sceIoDopen(hooked_path);
#if UMD_BLOCK_MODE
	check_file_emu(ret);
#endif
	log_ret_h(ret);
	return ret;
}

HOOK_FUNC_(int,sceIoDread,(int fd, SceIoDirent* dir))
{
	int ret;
	log_h_h(fd,(int)dir,name_sceIoDread);
#if LOG_OUTPUT
	log_printf("name=%s:",dir->d_name);
#endif
#if UMD_BLOCK_MODE
	check_file_emu(fd);
#endif
	ret = sceIoDread(fd,dir);
	log_ret_h(ret);
	return ret;
}

HOOK_FUNC_(int,sceIoChdir,(const char *path))
{
	char path_buf[MAX_FILE_PATH];
	const char *hooked_path;
	int ret;

	log_s(path,name_sceIoChdir);
	hooked_path = makePath(path_buf,path);
	ret = sceIoChdir(hooked_path);
	log_ret_h(ret);
	return ret;
}

//int sceIoGetstat(const char *name, void *buf);
HOOK_FUNC_(int,sceIoGetstat,(const char *name, void *buf))
{
	char path_buf[MAX_FILE_PATH];
	const char *hooked_path;
	int ret;

	log_s_h(name,(int)buf,name_sceIoGetstat);
	hooked_path = makePath(path_buf,name);
	ret = sceIoGetstat(hooked_path,buf);
	log_ret_h(ret);
	return ret;
}

int sceIoSync(const char *devname, int flag);
HOOK_FUNC_(int,sceIoSync,(const char *devname, int flag))
{
	char path_buf[MAX_FILE_PATH];
	const char *hooked_path;
	int ret;

	log_s_h(devname,flag,name_sceIoSync);
	hooked_path = makePath(path_buf,devname);
	ret = sceIoSync(hooked_path,flag);
	log_ret_h(ret);
	return ret;
}

int sceIoChstat(const char *name, void *buf, unsigned int cbit);
HOOK_FUNC_(int,sceIoChstat,(const char *name, void *buf, unsigned int cbit))
{
	char path_buf[MAX_FILE_PATH];
	const char *hooked_path;
	int ret;

	log_s_h_h(name,(int)buf,cbit,name_sceIoChstat);
	hooked_path = makePath(path_buf,name);
	ret = sceIoChstat(hooked_path,buf,cbit);
	log_ret_h(ret);
	return ret;
}

// mscmch0
// cmd = 2015804
//
HOOK_FUNC_(int,sceIoDevctl,(const char *dev, unsigned int cmd, void *indata, int inlen, void *outdata, int outlen))
{
	char path_buf[MAX_FILE_PATH];
	const char *hooked_path;
	int ret;

	log_s_h_h_h_h_h(dev,cmd,(int)indata,inlen,(int)outdata,outlen,name_sceIoDevctl);
	hooked_path = makePath(path_buf,dev);
	ret = sceIoDevctl(hooked_path,cmd,indata,inlen,outdata,outlen);
	log_ret_h(ret);
	return ret;
}

// MGS:(disc0:,umd0:,isofs:,1,0,0)
HOOK_FUNC_(int,sceIoAssign,(const char *dev1, const char *dev2, const char *dev3, int mode, void* unk1, long unk2))
{
	log_s_s_s_h_h_h(dev1,dev2,dev3,mode,(int)unk1,(int)unk2,name_sceIoAssign);

#if 0
	if(mode_umd_emu)
	{
		if(!strcmp(dev1,"disc0:") && !strcmp(dev2,"umd0:"))
		{
			log_printf("Block Access mode !!!!!\n");
			sceIoAssign(dev1, "msstor0p1:" , "fatms0:" , mode , unk1 , unk2);
			return 0;
		}
	}
#endif
	int ret = sceIoAssign(dev1,dev2,dev3,mode,unk1,unk2);
	log_ret_h(ret);
	return ret;
}

////////////////////////////////////////////////////////////////////
// UMD emulation
////////////////////////////////////////////////////////////////////

int sceUmdUnRegisterUMDCallBack(int cbid);
int sceUmdGetDriveStat(void);
int sceKernelDeleteCallback(int id);

HOOK_FUNC_(int,sceUmdCheckMedium,(int a))
{
	int ret;

	log_h(a,name_sceUmdCheckMedium);
	ret = mode_umd_emu ? 1 : sceUmdCheckMedium(a);
	log_ret_h(ret);
	return ret;
}

// unit 1=power on , 2=?
// drive "disc0:"
HOOK_FUNC_(int,sceUmdActivate,(int unit, const char *drive))
{
	int ret;

	log_h_s(unit,drive,name_sceUmdActivate);
	// call first !
	if(mode_umd_emu)
	{
		//sceKernelDelayThread(10000);
		umd_stat = UMD_ACTIVE;
		umd_callback_request++;
		//while(umd_callback_request)sceKernelDelayThread(1000); // NAMCO MUS
		ret = 0;
	}
	else
		ret = sceUmdActivate(unit,drive);

	log_ret_h(ret);
	return ret;
}

HOOK_FUNC_(int,sceUmdDeactivate,(int unit, const char *drive))
{
	int ret;

	log_h_s(unit,drive,name_sceUmdDeactivate);
	if(mode_umd_emu)
	{
		//sceKernelDelayThread(10000);
		umd_stat = UMD_DEACTIVE;
		umd_callback_request++;
		//while(umd_callback_request)sceKernelDelayThread(1000); // NAMCO MUS
		ret = 0;
	}
	else
	{
		ret = sceUmdDeactivate(unit,drive);
	}
	log_ret_h(ret);
	return ret;
}

HOOK_FUNC_(int,sceUmdWaitDriveStat,(int a))
{
	int ret;

	log_h(a,name_sceUmdWaitDriveStat);
	if(mode_umd_emu)
	{
		while(!(umd_stat & a))
			sceKernelDelayThread(10000);
		ret = 0;
	}
	else ret = sceUmdWaitDriveStat(a);
	log_ret_h(ret);
	return ret;
}

HOOK_FUNC_(int,sceUmdWaitDriveStatWithTimer,(int delay))
{
	int ret;

	log_h(delay,name_sceUmdWaitDriveStatWithTimer);
	if(mode_umd_emu) return 0;
	ret = sceUmdWaitDriveStatWithTimer(delay);
	log_ret_h(ret);
	return ret;
}

HOOK_FUNC_(int,sceUmdWaitDriveStatCB,(void))
{
	int ret;

	log_v(name_sceUmdWaitDriveStatCB);
	if(mode_umd_emu) return 0;
	ret = sceUmdWaitDriveStatCB();
	log_ret_h(ret);
	return ret;
}

// retrun
//  01 = out
//  02 = in
//  04 = change
//  08 = not RDY
//  10 = RDY
//  20 = READABLE
HOOK_FUNC_(int,sceUmdGetDriveStat,(void))
{
	int ret;

	log_v(name_sceUmdGetDriveStat);
	ret = mode_umd_emu ? umd_stat : sceUmdGetDriveStat();

	//test
	//sceKernelDelayThread(10000);
	log_ret_h(ret);
	return ret;
}

// 
HOOK_FUNC_(int,sceUmdGetErrorStat,(void))
{
	log_v(name_sceUmdGetErrorStat);
	int ret = sceUmdGetErrorStat();
	log_ret_h(ret);
	return ret;
}

HOOK_FUNC_(int,sceUmdGetDiscInfo,(int unk1,int unk2))
{
	log_h_h(unk1,unk2,name_sceUmdGetDiscInfo);
	int ret = sceUmdGetDiscInfo(unk1,unk2);
	log_ret_h(ret);
	return ret;
}

static SceKernelCallbackInfo info;

HOOK_FUNC_(int,sceUmdRegisterUMDCallBack,(int cbid))
{
	int ret;
	log_h(cbid,name_sceUmdRegisterUMDCallBack);
	if(mode_umd_emu)
	{
		memset(&info,0,sizeof(info));
		info.size = sizeof(info);
		if(sceKernelReferCallbackStatus(cbid,&info)>=0)
		{
			// callback OK , HOOK it
			umd_callback_request = 0; // wait for next event

			umd_callback_id     = cbid;
			umd_callback_arg    = info.common;
			umd_callback_func   = info.callback;
			umd_callback_count  = 0;

			log_printf("{HOOK UMD-CALLBACK '%s',%08X,%08X}"
				,info.name ,(int)(info.callback) ,(int)(info.common) );
		}
		ret = cbid;
	}
	else
	{
		// log_printf("Can't hook UMD_CALLBACK\n");
		ret = sceUmdRegisterUMDCallBack(cbid);
	}
	log_ret_h(ret);
	return ret;
}

HOOK_FUNC_(int,sceUmdUnRegisterUMDCallBack,(int cbid))
{
	int ret;
	log_h(cbid,name_sceUmdUnRegisterUMDCallBack);
	if(umd_callback_func && (umd_callback_id ==cbid) )
	{
#if LOG_OUTPUT
		log_printf("{remove hooked UMD-CALLBACK}");
#endif
		umd_callback_func = NULL;
		ret = 1;
	}
	else
	{
		ret = sceUmdUnRegisterUMDCallBack(cbid);
	}
	log_ret_h(ret);
	return ret;
}

////////////////////////////////////////////////////////////////////
// CallbackManager
////////////////////////////////////////////////////////////////////

HOOK_FUNC(int,sceKernelCreateCallback,(const char *name, void *func, void *arg))
{
	int ret;
	log_s_h_h(name,(int)func,(int)arg,name_sceKernelCreateCallback);
	ret = os_sceKernelCreateCallback(name,func,arg);
	log_ret_h(ret);
	return ret;
}

HOOK_FUNC_(int,sceKernelDeleteCallback,(int id))
{
	log_h(id,name_sceKernelDeleteCallback);
	int ret = sceKernelDeleteCallback(id);
	log_ret_h(ret);
	return ret;
}

#if 0
HOOK_FUNC_(int,sceKernelDelayThreadCB,(int delay))
{
	log_h(delay,name_sceKernelDelayThreadCB);
	int ret = sceKernelDelayThreadCB(delay);
	log_ret_h(ret);
	return ret;
}
#endif

////////////////////////////////////////////////////////////////////
// LOAD EXE 
////////////////////////////////////////////////////////////////////

HOOK_FUNC_(int,sceKernelRegisterExitCallback,(int cbid))
{
	int ret;
	log_h(cbid,name_sceKernelRegisterExitCallback);
	ret = sceKernelRegisterExitCallback(cbid);
	log_ret_h(ret);
	return ret;
}

////////////////////////////////////////////////////////////////////
// version controll
////////////////////////////////////////////////////////////////////
unsigned int sceKernelDevkitVersion(void);
#if 0
HOOK_FUNC_(int,sceKernelDevkitVersion,(void))
{
	int ret;
	log_v(name_sceKernelDevkitVersion);
	ret = sceKernelDevkitVersion();
	if(cfg_devkit_ver)
		ret = cfg_devkit_ver;

	log_ret_h(ret);
	return ret;
}
#endif

////////////////////////////////////////////////////////////////////
// LoadExec
////////////////////////////////////////////////////////////////////
HOOK_FUNC_(int,sceKernelLoadExec,(const char *file, void *unk))
{
	log_s_h(file,(int)unk,name_sceKernelLoadExec);
	sceKernelDelayThread(200000);
	return sceKernelLoadExec(file,unk);
#if 0
	char path_buf[MAX_FILE_PATH];
	const char *hooked_path;
	//int dst_fd;

	sceKernelStopModule(exec_module, 0, 0, 0, 0);

	hooked_path = makePath(path_buf,file);
	sceKernelDelayThread(1000000);

	set_kthread_cmd(CMD_LOAD_EXEC);
	return 0;

	sceIoChdir(psp_boot_path);
	dst_fd = sceIoOpen("autoexec",PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
	if(dst_fd >=0)
	{
		sceIoWrite(dst_fd,file,strlen(file)+1);
		sceIoClose(dst_fd);
		// reboot HOOK BOOT
	log_printf("\nRE-BOOT %s\n",g_elf_name);

		boot_path = g_elf_name;
	}
	hooked_path = makePath(path_buf,file);
	//func_hook_stop();

	sceKernelDelayThread(200000);
	return sceKernelLoadExec(hooked_path,unk);
#endif
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// MODULE MGR & UMD ENULATION TSR in kernelmode
////////////////////////////////////////////////////////////////////

/* Callback thread */
int KernelExecThread(void *arg)
{
//	int old_key;
//	log_printf("\nStart Emulate Thread\n");
	threadCMD = CMD_IDLE;

//	old_key = get_key();

	//sceKernelSleepThreadCB();
	while(1)
	{
#if 0
		log_set_bypass(get_key() & PSP_CTRL_NOTE);
#endif
		switch(threadCMD)
		{
		case CMD_IDLE:
			if(umd_callback_request>0)
			{
				sceKernelDelayThread(5000);
				if(umd_callback_func)
				{
#if LOG_OUTPUT
					log_printf("{UMD-CALLBACK (*%08X)(%08X,%08X,%08X)}",(int)umd_callback_func,umd_callback_count,umd_stat,(int)umd_callback_arg);
#endif
					int callback_ret = (*umd_callback_func)(umd_callback_count++,umd_stat,umd_callback_arg);
					if(callback_ret)
					{
#if LOG_OUTPUT
						log_printf("{unload UMD-CALLBACK}\n");
#endif
						umd_callback_func = NULL;
					}
				}
				umd_callback_request--;
			}
			sceKernelDelayThread(100000);
			break;
		case CMD_LOAD_MODULE:
#if LOG_OUTPUT
//			log_printf("LoadModule[KernelThread]('%s',%08X):",mload_path,mload_zero);
#endif
			mload_ret = sceKernelLoadModule(mload_path,mload_zero,mload_mod);
			import_ver_hack();

			threadCMD=CMD_IDLE;
			sceKernelDelayThread(1000); // short sleep
			break;
		case CMD_LOAD_MODULE_BYID:
#if LOG_OUTPUT
//			log_printf("LoadModuleByID[Kernelhread](%08X,%08X):",mload_id,mload_zero);
#endif
			mload_ret = sceKernelLoadModuleByID(mload_id,mload_zero,mload_mod);
			import_ver_hack();

			threadCMD=CMD_IDLE;
			sceKernelDelayThread(1000); // short sleep
			break;
#if 0
		case CMD_LOAD_EXEC:
			main();
			return 1;
#endif
		case CMD_EXIT:
			return 1;
		}
	}
}

void set_kthread_cmd(int cmd)
{
	while(threadCMD!=CMD_IDLE)
		sceKernelDelayThread(1000);
	threadCMD = cmd;
	while(threadCMD==cmd)
		sceKernelDelayThread(1000);
}

////////////////////////////////////////////////////////////////////
// search hook NID
////////////////////////////////////////////////////////////////////
const HOOK_LIST *search_hook_nid(unsigned int NID)
{
	const HOOK_LIST *list;

	for(list=hook_list_all ; list->Nid ; list++)
	{
		if(list->Nid == NID)
			return list;
	}
	return 0;
}

////////////////////////////////////////////////////////////////////
// patch entry point
////////////////////////////////////////////////////////////////////
#define SYSCALL_TBL ((unsigned int *)cfg_sysvec_addr)

/*

0000-: zero
0210-
0000-0083             zero fill  
0084-
13EC-1B43 (880204E0-) zero fill
1B44-1FFF (88022240-) table
2000-2FFF (88023530-) driver entry
*/

// jamp table 88023530-882752F , size 4000 , 4064 entry
// SYSCALL number

/*

0000-: zero
0210-

*/
static unsigned int syscall_free_ent;

int check_free_vector(void)
{
	int i;

	// no SYSTEMCALL VECTOR
	if(SYSCALL_TBL==NULL) return 0;

	syscall_free_ent = SYSCALL_TBL[0x2fff];
//printf("ENT %08X\n",syscall_free_ent);
	if(syscall_free_ent==0)
		return 0; // may be not free vector

	for(i=0x2ffe;i;i--)
	{
		if(SYSCALL_TBL[i] != syscall_free_ent)
			break;
	}

//printf("FREE %04X\n",i);

	if(i>0x2ff0)
		return 0; // may be not vector end

	if( SYSCALL_TBL[0x3000] == syscall_free_ent)
		return 0; // not vector border

	return i;
}

#if MODULE_HOOK
// HOOK MODULE one
static int hook_module_one(unsigned int *base,int size,unsigned int Nid,void *func,const char *func_name)
{
	int i;

printf(".");
	for(i=0;i<size/4;i++)
	{
		if( (base[i] == 0x03e00008) && (base[i+1]==Nid) )
		{
			log_printf("NID=%08X,M #%s [%08X]\n",Nid,func_name,i*4);
			base[i]   = MIPS_J(func);
			base[i+1] = MIPS_NOP;
			return 1;
		}
	}
	log_printf("#NID=%08X,N #%s not found\n",Nid,func_name,i*4);
	clear_cache();
	return 0;
}
#endif

//#define SYCALL_HOOK_START 0x3000
#define SYCALL_HOOK_START 0x2fe0

static int hook_systemcall_all(unsigned int *base,int size)
{
	int num_m , num_s;
	const HOOK_LIST *list;
	unsigned int rep_num = SYCALL_HOOK_START;
	int i;
	int vec_num;

	vec_num = check_free_vector();
	if(vec_num)
		printf("maximum used VECTOR is %04X\n",vec_num);

	num_m = 0;
	num_s = 0;
	for(list=hook_list_all,i=0;list->Nid!=0;list++,i++)
	{
		if(list->hook_func) // only support funcs
		{
			switch( func_hook_type[i] )
			{
			case HOOK_TYPE_MODULE:
#if MODULE_HOOK
				// MODULE HOOK
				if(hook_module_one(base,size,list->Nid,list->hook_func,list->func_name))
					num_m++;
				break;
#endif
			case HOOK_TYPE_SYSCALL:
				// SYSTEM CALL HOOK
				if(list->stub_entry)
				{
					unsigned int *stub_entry  = (unsigned int *)list->stub_entry;
					unsigned int  syscall_num = (stub_entry[1]>>6);

					if(list->api_ptr)
					{
						// SYSCALL ENTRY HOOK
						*(unsigned int *)list->api_ptr = SYSCALL_TBL[syscall_num];
						//*sceKernelCreateCallback = SYSCALL_TBL[syscall_num];
						SYSCALL_TBL[syscall_num] = (unsigned int)list->hook_func;
						clear_cache();

						num_s++;

					}
					else
					{
						// SYSCALL SWAP HOOK
						// copy original vector
						--rep_num;
						SYSCALL_TBL[rep_num] = SYSCALL_TBL[syscall_num];
						stub_entry[1]        = MIPS_SYSCALL(rep_num);

						// patch original entry to hook function
						SYSCALL_TBL[syscall_num] = (unsigned int)list->hook_func;
						clear_cache();

						log_printf("SYSTEMCALL HOOK %s %04X -> %04X\n",list->func_name,syscall_num,rep_num);
						num_s++;
					}

				}
				break;
			}
		}
	}
	printf("\n");
	if(vec_num)
		printf("minimum HOOKED VECTOR is %04X\n",rep_num);

	return num_m + num_s;
}

static int restore_hook(void)
{
	int num_hook = 0;
	const HOOK_LIST *list = hook_list_all;
	unsigned int rep_num = SYCALL_HOOK_START;
	int i;

	for(i=0;i<num_hook;i++)
	{
		list = &hook_list_all[i];

		if(list->hook_func) // only support funcs
		{
			switch( func_hook_type[i] )
			{
			case HOOK_TYPE_SYSCALL:
				// SYSTEM CALL HOOK
				if(list->stub_entry && list->api_ptr && (SYSCALL_TBL!=NULL) )
				{
					unsigned int *stub_entry  = (unsigned int *)list->stub_entry;
					unsigned int  syscall_num = (stub_entry[1]>>6);

					if(list->api_ptr)
					{
						SYSCALL_TBL[syscall_num] = *(unsigned int *)list->api_ptr;
						num_hook++;
					}
					else
					{
						unsigned int *stub_entry  = (unsigned int *)list->stub_entry;
						unsigned int  syscall_num = (stub_entry[1]>>6);

						// copy original vector
						--rep_num;
						SYSCALL_TBL[rep_num] = SYSCALL_TBL[syscall_num];
						stub_entry[1]        = MIPS_SYSCALL(rep_num);

						// patch original entry to hook function
						SYSCALL_TBL[syscall_num] = (unsigned int)list->hook_func;
						clear_cache();

						num_hook++;
					}
					//log_printf("SYSTEMCALL RESTORE %s %04X -> %04X\n",list->func_name,syscall_num,rep_num);
				}
				break;
			}
		}
		list++;
	}
	return num_hook;
}

////////////////////////////////////////////////////////////////////
// init & setup
////////////////////////////////////////////////////////////////////

int func_hook_start(unsigned int *base,int size)
{
	int num_hook;

	// start hook thread
	thid = sceKernelCreateThread("KernelExecThread", KernelExecThread, 0x11, 0xFA0, 0, 0);
	if(thid <0)
		return 0;
	sceKernelStartThread(thid, 0, 0);

	// hook systemcalls
	num_hook = hook_systemcall_all(base,size);

	// clear file mode emulation
	init_file_emu();

	return num_hook;
}

int func_hook_stop(void)
{
	int num_restore = 0;

	num_restore = restore_hook();

	// terminate thread
	threadCMD = CMD_EXIT;
	sceKernelDelayThread(100000);
	threadCMD = CMD_EXIT;
	sceKernelDelayThread(100000);

	return num_restore;
}

////////////////////////////////////////////////////////////////////
// API HOOK LISTS
////////////////////////////////////////////////////////////////////

/*
PUYO
977de386:sceKernelLoadModule
50f0c1ec:sceKernelStartModule
d1ff982a:sceKernelStopModule
c6183d47:sceUmdActivate
6b4a146c:sceUmdGetDriveStat
*/


const HOOK_LIST hook_list_all[] =
{
// SUPPORTED FUNCS
#define HOOK_ENTRYE(NID,NAME) {NID,name_##NAME,NAME,hook_##NAME,(&os_##NAME)},
#define HOOK_ENTRY_(NID,NAME) {NID,name_##NAME,NAME,hook_##NAME,NULL},
#define HOOK_ENTRYP(NID,NAME)
#include "api_list.h"
#undef HOOK_ENTRYE
#undef HOOK_ENTRY_
#undef HOOK_ENTRYP

#if 0
// UNSUPPORTED FUNCS
#define HOOK_ENTRYP(NID,NAME) {NID,name_##NAME,NULL,NULL,NULL},
#define HOOK_ENTRYE(NID,NAME)
#define HOOK_ENTRY_(NID,NAME)
#include "api_list.h"
#undef HOOK_ENTRYE
#undef HOOK_ENTRY_
#undef HOOK_ENTRYP
#endif

{0,0,0,0}
};

// number of 
#define NUM_HOOK (sizeof(hook_list_all) / sizeof(HOOK_LIST) -1)

// hook type marks
unsigned char func_hook_type[NUM_HOOK];
static const int func_num_hook = NUM_HOOK;

//static const dummp_str = "DUMMY";

////////////////////////////////////////////////////////////////////
// HOOK SELECT CONTROLL
////////////////////////////////////////////////////////////////////
int set_hook_type(unsigned int nid,int hook_type)
{
	int i;
	int num = 0;

//printf("set_hook_type(%08X,%d)\n",nid,hook_type);

	for(i=0;i<NUM_HOOK;i++)
	{
		if(nid==0xffffffff)
		{
			func_hook_type[i] = hook_type;
			num++;

//printf(".");

		}
		else if(hook_list_all[i].Nid == nid)
		{
			func_hook_type[i] = hook_type;
			num++;
			break;
		}
	}
	return num;
}

int get_hook_type(unsigned int nid)
{
	int i;
	for(i=0;i<NUM_HOOK;i++)
	{
		if(hook_list_all[i].Nid == nid)
			return func_hook_type[i];
	}
	return 0;
}

int get_num_hook(int hook_type)
{
	int i;
	int num = 0;
	for(i=0;i<NUM_HOOK;i++)
	{
		if(func_hook_type[i] == hook_type)
			num++;
	}
	return num;
}

// mode set
int set_umd_emulation_mode(int mode)
{
	mode_umd_emu = mode;
	return mode_umd_emu;
};

/*
// devkit op-code direct patch

V1.00 : DevkitVersion()
8800aa9c 3C020100 li  r2,#01000000
8000c754 03e00008 jr  r31
8800c758 34420300 ori r2,r3,#0300

V1.50 : DevkitVersion()
8800c750 3C030105 li  r3,#01050000
8000c754 03e00008 jr  r31
8800c758 34620001 ori r2,r3,#0001
*/

int patch_devkit_version(unsigned int devkit_ver)
{
	if(SYSCALL_TBL)
	{
		unsigned int *stub_entry  = (unsigned int *)sceKernelDevkitVersion;
		unsigned int  syscall_num = (stub_entry[1]>>6);
		unsigned int *func_entry  = (unsigned int *)SYSCALL_TBL[syscall_num];
//		printf("OLD sceDevkitVersion %08X\n",sceKernelDevkitVersion());
//log_printf("sceDevkitVersion %08X[%08X,%08X,%08X]\n",(int)func_entry,func_entry[0],func_entry[1],func_entry[2]);
		if(
			((func_entry[0] & 0xff000000)==0x3c000000) &&
			(func_entry[1] == 0x03e00008) &&
			((func_entry[2] & 0xff000000)==0x34000000) )
		{
			func_entry[0] = 0x3c020000 | (devkit_ver>>16);
			func_entry[1] = 0x03e00008;
			func_entry[2] = 0x34420000 | (devkit_ver&0xffff);
//			printf("NEW sceDevkitVersion %08x\n",sceKernelDevkitVersion());
log_printf("patch sceDevkitVersion %08X[%08X,%08X,%08X]\n",(int)func_entry,func_entry[0],func_entry[1],func_entry[2]);
			return 1;
		}
	}
	return 0;
}

