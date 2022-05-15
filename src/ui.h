#ifndef  __UI__
#define __UI__

#define SENSE_KEY (PSP_CTRL_CIRCLE|PSP_CTRL_TRIANGLE|PSP_CTRL_CROSS|PSP_CTRL_SQUARE|PSP_CTRL_START|PSP_CTRL_SELECT)

#define printf pspDebugScreenPrintf
#define DPRINT pspDebugScreenPrintf

int get_key(void);
int wait_key(void);

#endif
