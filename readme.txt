HOOK BOOT Ver.0.94

API HOOK monitor , API emulation , and dynamic kernel & program patch.

This is very dangerous software to work with Kernelmode.
Use it in your own risk. 

-- what is HOOK BOOT ?

It patches BOOT.BIN, and it can hook any API. 

1.API monitoring to output DISPLAY or FILE.
  To analyze API argument and how to use it.
  The place of log file is "ms0:/LOG.txt"

2.UMD emulation.
  It can boot some game from MS, for save battery and fast access.

3.Information on the import function can be known. 
  To analyze.

4.Dynamic patch for Kernel and user memory.

-- HISTRY

V0.94
  -supported ISO9660 file system emulation (alpha version)
   The hook of open,close and seek should be necessary.
   sceDopen/sceDread can't work yet.
   result of Async function are not correct.
   see PSP/GAME/MINGOLF/hookboot.ini
  -UMD BLOCK MODE emulation.
   The hook of open,close,seek and read functions should be necessary.
  -added SYSCALL entry hook mode
   support sceCreateCallback() hook with SYSCALL hook.
  -fix some bugs.

V0.93
  -bugfix didn't replace path in sceIoDread.
   (in MetagGearAC!D,but it not work yet.)
  -added cache clear controll.
   (fix freeze SYSCALL HOOK with FILE log.)
  -UMD callback hook method changed,sceCreateCallback hook is unnecessary.
  -The handle 1 of sceIoWrite output to log.(SYSTEM LOG)
  -added hook API sceIoSync and sceIoChstat.
  -path replace in sceIoDevctl.

V0.92B
  -minimize AREA(un supported funcs doesn't report API name)
  -fix CODED ARMS with firmware1.00

V0.92A
  -bypass kernel version patch case no KERNEL_VER parameter.(work on1.50?)
  -bugfix hook func sceIoLseek,sceIoLseekAsync and sceIoGetAsyncStat.

V0.92
  -supported dynamic import library version patch.
   now bootable PSP 1.50 software on psp 1.00 without prx replace.
   try PSP 1.52 software on your psp 1.50.
   see "IMPORT_VER" parameter in 'CODEDARMS/boot.ini'.
  -added "WAIT_RUN=[wait_mode]" parameter.
  -support sceIoLseek and sceIoLseekAsync hook again(request).
  -added sceKernelLoadExec hook.

V0.91
  -change config syntax & handling
  -change path replace handling
  -bugfix freeze in hook of sceKernelStopModule.
  -bugfix no umd emulation mode.
  -devkit version & kernek driver version patch

V0.90
 -remove setting CUI and added config file
  ->Multi game in one MS
 -Adjust wait time in sceOpenAsinc
  ->NAMCO MUSIUM
 -Change Memory Allocation
  -> RIDGE RACERS
  -> PIPOSARU ACADEMIEA
 -added systemcall hook mode (only Firmware V1.00)
 -cleanup all
 -fix sceKernelDeleteCallback() re-entry bug.
 -added hook API sceIoLseek32,sceIoLseek32Async,sceUmdGetDiscInfo

V0.10 1st release

-- TECHNOLOGY

1.MODULE patch mode
 1) before module load
   The target NID are removed from the file, and filled up in the ENTRY point. 
 2)after load
   The entry points are searched with NID in memory and hook it.

2.SYSTEM CALL hook mode
  modify and hook SYSTEMCALL jump vector table.

-- LIMITATION

The UMD emulation is still developing,not a perfect.

1.MODULE patch mode
  supported non-encrypted ELF only.
  supported single module only,sub modules can't hook.

2.SYSTEM CALL hook mode
  direct bootup without pre-patch.
  supported encrypted ~PSP file (EBOOT.BIN).
  still some problem.(can't work sceCreateCallback)

-- How to use 

caes PuzzleBobble

1.copy files in MS_ROOT_DIR to MS root.
2.copy UMD giles to '/PSP/GAME/PBOBBLE/UMD0/'
3.copy '/PSP/HOOKBOOT/EBOOT.PBP' to '/PSP/GAME/PBOBBLE/'
  (use PBP tool for Formware 1.50)
4.execute '/PSP/GAME/PBOBBLE/EBOOT.PBP' on PSP.

  see '/PSP/HOOKBOOT/redame.txt'

-- UMD emulation

1.loadable KernelModule from MS.
2.emulate the UMD Drive condition.

-- FUTURE PLAN

1.UMD emulation with FTP client.

2.perfect version up kernel patch
  supported.

-- check list (V0.94)

Working

MINNA NO GOLF PORTABLE (UMD direct)
CODED ARMS (again)

Not working
CODED ARMS (UMD direct)

