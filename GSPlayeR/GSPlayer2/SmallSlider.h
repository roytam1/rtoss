#ifndef __SMALLSLIDER_H__
#define __SMALLSLIDER_H__

#define SSCONTROL _T("SmallSlider")
#define SSWIDTH 11
#define SSHEIGHT 64

#ifdef _cplusplus
extern "C" {
#endif

void InitializeSmallSliderControl();
void UninitializeSmallSliderControl();
LRESULT CALLBACK SmallSliderControl(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#ifdef _cplusplus
};
#endif
#endif