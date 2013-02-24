//////////
//
// colorwheel.h
//
//////
// Version 1.00
// Copyright (c) 2013 by Rick C. Hodgin
//////
// Last update:
// February 23, 2013
//////
// Change log:
// February 23, 2013 - Initial creation
//////
//
// This software is released as Liberty Software under a Repeat License, as governed
// by the Public Benefit License v1.0 or later (PBL).
//
// You are free to use, copy, modify and share this software. However, it can only
// be released under the PBL version indicated, and every project must include a copy
// of the pbl.txt document for its version as is at http://www.libsf.org/licenses/.
//
// For additional information about this project, or to view the license, see:
//
// http://www.libsf.org/
// http://www.libsf.org/licenses/
// https://github.com/RickCHodgin/resourcex
//
// Thank you. And may The Lord bless you richly as you lift up your life, your
// talents, your gifts, your praise, unto Him. In Jesus' name I pray. Amen.
//
//////////




#ifdef COLORWHEEL_EXPORTS
    #define COLORWHEEL_API __declspec(dllexport)
#else
    #define COLORWHEEL_API __declspec(dllimport)
#endif

// Note: To view properly, set tabs to 4 characters.
COLORWHEEL_API void		colorwheel_set_subclass_form				(HWND tnHwndParent, int tnX, int tnY, int tnWidth, int tnHeight);
COLORWHEEL_API void		colorwheel_un_subclass_form					(void);
COLORWHEEL_API void		colorwheel_set_rgb_grayscale_adjustments	(int tnRed, int tnGrn, int tnBlu, int tnGray, int tnPastel, int tnLinear, int tnRotation, int tnAlgorithm);
COLORWHEEL_API int		colorwheel_get_rgb_at_xy					(int* tnX, int* tnY);
COLORWHEEL_API int		colorwheel_set_rgb							(int tnRgb);
COLORWHEEL_API void		colorwheel_set_algorithms					(int tnColorChart);

// Local/internal function prototype definitions
DWORD WINAPI			buildColorWheelThreadProc					(LPVOID lpParameter);
LRESULT CALLBACK		colorChartWndProc							(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void					iGetColorAtCoordinate						(float theta, float phi, unsigned char& lnRed, unsigned char& lnGrn, unsigned char& lnBlu);
void					iDeriveColors								(float theta, float& lfRed, float& lfGrn, float& lfBlu);
void CALLBACK			TimerProc									(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime);
