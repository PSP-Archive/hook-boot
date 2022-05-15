/*

*/

#include "common.h"

#define EXPORT_NAME_OFFSET_MAX 0x10000
#define EXPORT_NAME_MIN  4
#define EXPORT_NAME_MAX 32

static char *check_export_module_name(unsigned int *ptr)
{
	unsigned char *name_ptr = (unsigned char *)(*ptr);
	int offs = (int)name_ptr - (int)ptr;
	int i;
	
	// offset check
	if(offs <0 || offs >= EXPORT_NAME_OFFSET_MAX)
		return 0;

	// ascii check
	for(i=0;i<EXPORT_NAME_MAX;i++)
	{
		char chr = name_ptr[i];
		if(chr==0)
			break;

		if( (chr<0x20) || (chr>'z') )
			return 0;
	}
	if( (i < EXPORT_NAME_MIN) || (i > EXPORT_NAME_MAX))
		return 0;

	// OK
	return name_ptr;
}

int psp_lib_patch(unsigned int top,unsigned int size,unsigned int type,unsigned int old_ver,unsigned int new_ver)
{
	unsigned char *start,*end;
	unsigned int *ptr;
	int num = 0;

	if(size==0)
	{
		size = (unsigned int)PSP_UMEM_END - top;
	}
	size &=0xfffffff0;

	start = ((unsigned char *)top) + 4;
	end   = ((unsigned char *)top) + size;

	log_printf("Library Version patch:base=%08X,size=%08X,type=%02X,ver%02X->%02X\n",top,size,type,old_ver,new_ver);

	for(ptr=(unsigned int *)start;ptr!=(unsigned int *)end;ptr++)
	{
if(*(unsigned char *)ptr ==old_ver) // FAST PASS
		if(
		  (ptr[0]&0x000000ff)==old_ver  &&     // ATTL,ATTH:00:VER
		  (ptr[1]&0xff0000ff)==type)           // 00:num_func:00:TYPE
		{
			unsigned char *name_ptr = check_export_module_name(ptr-1);
			if(name_ptr)
			{
					ptr[0] = (ptr[0]&0xffffff00) | new_ver;
log_printf("%08X:ImportLib %s Version %02X->%02X\n",(int)(ptr-1),name_ptr,old_ver,new_ver);
					num++;
			}
		}
	}
	clear_cache();
	return num;
}

void sceKernelDcacheWritebackAll(void);

void clear_cache(void)
{
// sceKernelIcacheClearAll : 0A00526C
//	printf("sceKernelIcacheClearAll = %08X\n",(int)sceKernelIcacheClearAll);
	sceKernelDcacheWritebackAll();
	sceKernelIcacheClearAll();
}

// kernel version patch
int patch_kernel_version(unsigned short new_ver)
{
	unsigned short *wptr = (unsigned short *)PSP_KMEM_TOP;
	unsigned short attr_code = 0x1007; // driver ATTR
	unsigned int size = PSP_KMEM_SIZE;
	unsigned char *bptr;
	int num = 0;
	int i;

//printf("patch_kernel_version(%04X,%04X)\n",old_ver,new_ver);

	for(i=0;i< (size/2)-16;i+=2)
	{
		if(wptr[i]==attr_code)
		{
			bptr = (unsigned char *)&wptr[i+2];
			if( (bptr[0]=='s') &&
			    (bptr[1]=='c') &&
			    (bptr[2]=='e') &&
			    strlen(bptr)<27)
			{
				if(wptr[i+1] < new_ver)
				{

log_printf("Kernel[%08X] %s:Ver %04X->%04X\n",(int)&wptr[i],bptr,&wptr[i+1],new_ver);
					wptr[i+1] = new_ver;
					num++;
				}
			}
		}
	}
	clear_cache();
	return num;
}

#if 0

/*

L880e706c:sceUmdManThreadManagerReadAsync

umd9660‚©‚ç‚ÌƒŠƒ“ƒNƒGƒ“ƒgƒŠ

L88100e9c:
	j	$880e706c ; j 880F0000

‚Â‚È‚¬
880F0000:
	li    r2,entry_h & 0xffff0000
	addiu r2,r2,#entry_h & 0x0000ffff
	jr    r2



UmdManCheckDeviceReady
L88100e84:
	j	$880df598
*/

int hook_sceUmdManThreadManagerReadAsync(int r4,int r5,int r6)
{
	int ret;
	int (*sceUmdManThreadManagerReadAsync)(int,int,int) ;

	// true entry point
//	sceUmdManThreadManagerReadAsync = 0x880e706c;
	sceUmdManThreadManagerReadAsync = 0x880df598; // UmdManCheckDeviceReady

log_printf("sceUmdManThreadManagerReadAsync(%08X,%08X,%08X)\n",r4,r5,r6);

	ret = sceUmdManThreadManagerReadAsync(r4,r5,r6);
	return ret;
}

unsigned int sceUmdManThreadManagerReadAsync_hack(void *hook_func)
{
#if 1
	// UmdManCheckDeviceReady
	unsigned int *stub  = (unsigned int *)0x88100e84;
	hook_func = (void *)hook_sceUmdManThreadManagerReadAsync;
#else
	unsigned int *stub  = (unsigned int *)0x88100e9c;
	hook_func = (void *)hook_sceUmdManThreadManagerReadAsync;
#endif
	unsigned int *stub2 = (unsigned int *)0x883f0000;

	// stub entry change
	stub[0] = MIPS_J(stub2);

	// goto monitor entry
	stub2[0] = MIPS_LUI(2,((unsigned int)hook_func)>>16);
	stub2[1] = MIPS_ORI(2,2,((unsigned int)hook_func)&0xffff);
	stub2[2] = MIPS_JR(2);
	stub2[3] = MIPS_NOP;

	clear_cache();
	return 0;
}

#endif
