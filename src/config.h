#ifndef __CONFIG__
#define __CONFIG__

// input
extern unsigned int psp_devkit_ver;
extern char psp_boot_path[MAX_FILE_PATH];

// FILE PATH
extern char cfg_title[80];
extern char cfg_umd0_path[MAX_FILE_PATH];
extern char cfg_target_path[MAX_FILE_PATH];
extern char cfg_patch_path[MAX_FILE_PATH];

// Version Patch
extern unsigned int cfg_kernel_ver;
extern unsigned short cfg_devkit_ver;
extern unsigned char cfg_import_hack_ver;
extern unsigned char cfg_import_set_ver;

// DEBUG
extern unsigned int cfg_log_val;

// SYSTEM SETTING
extern unsigned int cfg_sysvec_addr;

//
extern unsigned int cfg_wait_run;

int cfg_load(const char *path);

#endif
