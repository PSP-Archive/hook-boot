#include "common.h"

static void dummy_entry(void)
{
	printf("NO Link Kernel API Called\n");
	while(1) sceKernelDelayThread(100000);
}

// convert kernel API entry 'J' to linear address
void convert_kernel_api(void *stub_top,int num)
{
	unsigned int *ptr;
	unsigned int stub_op;

//printf("void convert_kernel_api(%08x,%d)\n",(int)stub_top,num);

	for(ptr = (unsigned int *)stub_top;num;num--,ptr+=2)
	{
		stub_op = ptr[0];

		printf("STUB %08X=%08X,%08X\n",(int)ptr,stub_op,ptr[1]);


		if( (stub_op&0xfc000000) == 0x08000000) // J ?
		{
			// return kernel entry point
			ptr[0] = ((stub_op&0x03ffffff)<<2) | 0x80000000;
		}
		else
		{
			printf("NO Link Kernel API Found %08X\n",stub_op);
			ptr[0] = (unsigned int)dummy_entry;
		}
	}
}

// convert all Kernek API J to linear address
int init_kernel_api(void)
{
// sceKernelIcacheClearAll : 0A00526C

	convert_kernel_api(&sceKernelIcacheClearAll,1);
	convert_kernel_api(&pspDebugInstallKprintfHandler,2);
	return 0;
}
