#ifndef __COMMON___
#define __COMMON___

#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <pspumd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// setting
#define MAX_FILE_PATH 192
#define PATH_HEAP_SIZE (1024)

// support pre-PATCH MODULE HOOK
#define MODULE_HOOK 1
#define MIN_BUILD   0

#include "devkit401.h"

#include "sstring.h"
#include "stubk.h"

#include "elf.h"
#include "elf-lib.h"

#include "ui.h"
#include "log.h"

#include "func_hook.h"
#include "psp_elf.h"
#include "file.h"
#include "path.h"
#include "config.h"
#include "kpatch.h"
#include "file_emu.h"

#define MODE_PSP

#endif
