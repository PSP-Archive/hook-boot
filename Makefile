TARGET = hookboot
OBJS = src/main.o src/log.o src/ui.o src/elf-lib.o src/psp_elf.o src/func_hook.o src/sstring.o src/config.o src/file.o src/path.o src/stub.o src/kpatch.o src/file_emu.o src/iso9660.o src/stubk.o src/stub.o

INCDIR = 
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR =
USE_PSPSDK_LIBC = 1
#LIBS= -lpspumd
LIBS=
LDFLAGS = -T hookboot.x

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = HOOK BOOT Ver.0.94
#PSP_EBOOT_UNKPNG = HOOKBOOT.png
PSP_EBOOT_ICON = HOOKBOOT.png

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
