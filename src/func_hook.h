#ifndef __FUNC_HOOK_H__
#define __FUNC_HOOK_H__

#define MAX_PATH_HOOK 32

extern int mode_umd_emu;
extern int mode_umd_capture;

typedef struct hook_entry_list
{
	unsigned int Nid;      // NID
	const char *func_name; // syscall name
	void *stub_entry;      // stub entry in this module
	void *hook_func;       // hook function entry
	void *api_ptr;         // pointer of api entry addres
}HOOK_LIST;

//
// path list
//
#define MAX_PATH_LIST 16
extern const char *hook_path_list[MAX_PATH_LIST*2];

//
// API LIST
//
extern const HOOK_LIST hook_list_all[];

// hook type
enum{
	HOOK_TYPE_NOHOOK,
	HOOK_TYPE_MODULE,
	HOOK_TYPE_SYSCALL,
//
	HOOK_TYPE_MAX
};

//
#define NID_ALL 0xffffffff
int set_hook_type(unsigned int nid,int hook_type);
int get_hook_type(unsigned int nid);
int get_num_hook(int hook_type);

// mode set
int set_umd_emulation_mode(int mode);

// system vector
int check_free_vector(void);

int func_hook_start(unsigned int *base,int size);
int func_hook_stop(void);

//
const HOOK_LIST *search_hook_nid(unsigned int NID);

// DEVKIT patch
int patch_devkit_version(unsigned int devkit_ver);
//
extern int func_num_hook_list;


// KernelThread Controll
enum {
	CMD_IDLE=0,
	CMD_LOAD_MODULE,
	CMD_LOAD_MODULE_BYID,
	CMD_IMPORT_VER_PATCH,
	CMD_LOAD_EXEC,
//
	CMD_EXIT
};
void set_kthread_cmd(int cmd);

#endif
