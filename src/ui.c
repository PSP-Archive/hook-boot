/*
 *Wab launcher V2.5 the original :=)
 */

#include "common.h"

/////////////////////////////////////////////////////////////////////////////
// UI
/////////////////////////////////////////////////////////////////////////////
int get_key(void)
{
#if 0
	// 4.0.1
	SceCtrlData pad;
	sceCtrlReadBufferPositive(&pad, 1);
	return pad.Buttons;
#else
	// r3
	ctrl_data_t pad;
	sceCtrlReadBufferPositive(&pad, 1);
	return pad.buttons;
#endif
}

int wait_key(void)
{
	int key;
	while(1)
		if(!( get_key() & SENSE_KEY)) break;

	while(1)
		if( (key=get_key()) & SENSE_KEY) break;

	return key;
}

