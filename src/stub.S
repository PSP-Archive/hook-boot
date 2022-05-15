	.set noreorder

#include "common.s"

	STUB_START	"LoadCoreForKernel",0x40010000,0x00010005
	STUB_FUNC	0xd8779ac6,sceKernelIcacheClearAll
#	STUB_FUNC	0x99a695f0,sceKernelRegisterLibrary
#	STUB_FUNC	0x0e760dba,sceKernelLinkLibraryEntries
#	STUB_FUNC	0xae7c6e76,sceKernelRegisterModule
#	STUB_FUNC	0x99a695f0,sceKernelRegisterLibrary
	STUB_END

	STUB_START	"KDebugForKernel",0x40010000,0x00020005
	STUB_FUNC	0x7ceb2c09,pspDebugInstallKprintfHandler
	STUB_FUNC	0x84f370bc,Kprintf
	STUB_END

