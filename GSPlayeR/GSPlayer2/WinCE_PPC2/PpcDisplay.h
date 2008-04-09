#ifndef __PPCDISPLAY_H__
#define __PPCDISPLAY_H__

#ifdef __cplusplus
extern "C" {
#endif
	void ToggleDisplay();
	BOOL GetDisplayStatus();
	void TurnOnDisplay();
	void TurnOffDisplay();
#ifdef __cplusplus
};
#endif

#endif // __PPCDISPLAY_H__