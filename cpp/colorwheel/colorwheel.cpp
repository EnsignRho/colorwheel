//////////
//
// colorwheel.cpp
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
// February 25, 2013 - Changes to rotation algorithm
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




// Note: To view properly, set tabs to 4 characters.
#include "stdafx.h"
#include "colorwheel.h"

const float	_PI						= 3.1415926536f;
const float	_2PI					= 6.2831853072f;
const float	_2PI_3					= 2.0943951024f;
const float	_PI2					= 1.5707963268f;
const float	_4PI_3					= 4.1887902048f;

// HWND of the form we'll draw onto
RECT		gnRect					= { 0, 0, 0, 0 };
int			gnActualWidth			= 0;
HWND		gnHwndParent			= NULL;
HWND		gnHwnd					= NULL;
HDC			gnHdc					= NULL;
HDC			gnHdc2					= NULL;
HBITMAP		gnHbmp					= NULL;
BITMAPINFO	gbmi					= { 0 };
char*		gcBits					= NULL;		// Bits of the DIBSection
WNDPROC		lnOldWndProcAddress		= NULL;
int			gnAlgorithm				= 1;
int			gnMouseX				= 0;
int			gnMouseY				= 0;
UINT_PTR	gnHTimer				= NULL;
bool		glManualColor			= true;



// Colors for generating the color bar
int			gnRed			= 255;
int			gnGrn			= 255;
int			gnBlu			= 255;
int			gnGray			= 0;
int			gnPastel		= 50;
int			gnLinear		= 50;
int			gnRotation		= 0;


// Structure for accessing RGB data within the DIB section
struct SRGB
{
	unsigned char	blu;
	unsigned char	grn;
	unsigned char	red;
};




//////////
//
// Called to indicate what the HWND value is we'll subclass
//
//////
	COLORWHEEL_API void colorwheel_set_subclass_form(HWND tnHwndParent, int tnX, int tnY, int tnWidth, int tnHeight)
	{
		ATOM		lResult;
		WNDCLASSEX	wcex;


		//////////
		// Store the HWND and rect
		//////
			gnHwndParent = tnHwndParent;
			SetRect(&gnRect, 0, 0, tnWidth, tnHeight);


		//////////
		// Create a child window
		//////
			memset(&wcex, 0, sizeof(wcex));
			wcex.cbSize         = sizeof(WNDCLASSEX);
			wcex.style          = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
			wcex.lpfnWndProc    = (WNDPROC)GetWindowLong(gnHwndParent, GWL_WNDPROC);
			wcex.hInstance      = GetModuleHandle(NULL);
			wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
			wcex.hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH);
			wcex.lpszClassName  = L"colorChart";
			lResult = RegisterClassEx(&wcex);
			gnHwnd = CreateWindowEx(WS_EX_NOPARENTNOTIFY, L"colorChart", L"colorChart", WS_CHILD, tnX, tnY, tnWidth, tnHeight, gnHwndParent, NULL, wcex.hInstance, NULL);
			int error = GetLastError();
			gnHdc = GetDC(gnHwnd);
			gnHdc2 = CreateCompatibleDC(gnHdc);
			ShowWindow(gnHwnd, SW_SHOW);


		//////////
		// Compute the actual width
		//////
			if ((tnWidth * 3) % 4 != 0)
			{
				// It has to be rounded up to be dword aligned
				gnActualWidth = tnWidth * 3 + (4 - ((tnWidth * 3) % 4));

			} else {
				// It is evenly divisible by 4 when multiplied by 3 (dword aligned)
				gnActualWidth = tnWidth * 3;
			}


		//////////
		// Create a DIB section of the appropriate size
		//////
			memset(&gbmi, 0, sizeof(gbmi));
			gbmi.bmiHeader.biSize			= sizeof(gbmi);
			gbmi.bmiHeader.biWidth			= tnWidth;
			gbmi.bmiHeader.biHeight			= tnHeight;
			gbmi.bmiHeader.biCompression	= 0;
			gbmi.bmiHeader.biPlanes			= 1;
			gbmi.bmiHeader.biBitCount		= 24;
			gbmi.bmiHeader.biSizeImage		= gnActualWidth * tnHeight;
			gbmi.bmiHeader.biXPelsPerMeter	= 3270;
			gbmi.bmiHeader.biYPelsPerMeter	= 3270;
			gnHbmp = CreateDIBSection(gnHdc, &gbmi, DIB_RGB_COLORS, (void**)&gcBits, NULL, 0);
			// Put the bitmap into the dc
			SelectObject(gnHdc2, gnHbmp);
			// Make the bitmap black
			memset(gcBits, 0, gnActualWidth * tnHeight);


		//////////
		// Draw the window
		//////
			InvalidateRect(gnHwnd, &gnRect, true);
			lnOldWndProcAddress = (WNDPROC)GetWindowLong(gnHwnd, GWL_WNDPROC);
			SetWindowLong(gnHwnd, GWL_WNDPROC, (long)&colorChartWndProc);
			gnHTimer = SetTimer(gnHwnd, 39, 10, &TimerProc);
	}




//////////
//
// Called to un-subclass the form
//
//////
	COLORWHEEL_API void colorwheel_un_subclass_form(void)
	{
		KillTimer(gnHwnd, 39);
		SetWindowLong(gnHwnd, GWL_WNDPROC, (long)lnOldWndProcAddress);
	}




//////////
//
// Called to set the color values for the next color render operation
//
//////
	COLORWHEEL_API void colorwheel_set_rgb_grayscale_adjustments(int tnRed, int tnGrn, int tnBlu, int tnGray, int tnPastel, int tnLinear, int tnRotation, int tnAlgorithm)
	{
		DWORD lnThreadId;


		// These are standard RGB values
		gnRed		= max(min(tnRed,		255), 0);
		gnGrn		= max(min(tnGrn,		255), 0);
		gnBlu		= max(min(tnBlu,		255), 0);

		// These are all percents
		gnGray		= max(min(tnGray,		100), 0);
		gnPastel	= max(min(tnPastel,		100), 0);
		gnLinear	= max(min(tnLinear,		100), 0);

		// Rotation is the color rotation, from 0..255
		gnRotation	= max(min(tnRotation,	255), 0);

		// Store the rendering algorithm
		gnAlgorithm	= max(min(tnAlgorithm,	4), 1);

		// Redraw the colorChart
		CreateThread(NULL, NULL, &buildColorWheelThreadProc, NULL, NULL, &lnThreadId);
	}

	DWORD WINAPI buildColorWheelThreadProc(LPVOID lpParameter)
	{
		unsigned char	lnRed, lnGrn, lnBlu;
		int				lnY;
		float			lfW, lfH, lfWStep, lfHStep;
		SRGB*			lrgb;


		// Grab the step per pixel across
		lfWStep		= _2PI / (float)gbmi.bmiHeader.biWidth;
		lfHStep		= _2PI / (float)gbmi.bmiHeader.biHeight;

		// Generate the colored center line
		for (	lnY = 0, lfH = 0.0f;
				lnY < gbmi.bmiHeader.biHeight;
				lfH += lfHStep, lnY++)
		{
			// Compute the location for this row
			lrgb = (SRGB*)(gcBits + ((gbmi.bmiHeader.biHeight - lnY - 1) * gnActualWidth));

			// Repeat for every pixel across
			for (lfW = 0.0f; lfW < _2PI; lfW += lfWStep, lrgb++)
			{
				// Grab the colors for this location
				iGetColorAtCoordinate(lfW, lfH, lnRed, lnGrn, lnBlu);

				// Store the color
				lrgb->red	= lnRed;
				lrgb->grn	= lnGrn;
				lrgb->blu	= lnBlu;
			}
		}
		// When we get here, the bitmap is updated

		// Refresh the colorWheel
		InvalidateRect(gnHwnd, NULL, false);
		return(0);
	}

	void iGetColorAtCoordinate(float theta, float phi, unsigned char& lnRed, unsigned char& lnGrn, unsigned char& lnBlu)
	{
		float r1, g1, b1;
		float r2, g2, b2;
		float r3, g3, b3;
		float r4, g4, b4;
		float r5, g5, b5;
		float lfGray, lfMask, lfMaskM, lfTheta;

		// Grab colors
		iDeriveColors(theta, r1, g1, b1);
		iDeriveColors(phi, r2, g2, b2);

		// Combine
		switch (gnAlgorithm)
		{
			case 1:
			default:
				lnRed = (unsigned char)((255.0f - r1 + 255.0f - r2) / 2.0);
				lnGrn = (unsigned char)((255.0f - g1 + 255.0f - g2) / 2.0);
				lnBlu = (unsigned char)((255.0f - b1 + 255.0f - b2) / 2.0);
				break;

			case 2:
				lnRed = (unsigned char)((r1 + 255.0f - r2) / 2.0);
				lnGrn = (unsigned char)((g1 + 255.0f - g2) / 2.0);
				lnBlu = (unsigned char)((b1 + 255.0f - b2) / 2.0);
				break;

			case 3:
				lnRed = (unsigned char)((255.0f - r1 + r2) / 2.0);
				lnGrn = (unsigned char)((255.0f - g1 + g2) / 2.0);
				lnBlu = (unsigned char)((255.0f - b1 + b2) / 2.0);
				break;

			case 4:
				lnRed = (unsigned char)((r1 + r2) / 2.0);
				lnGrn = (unsigned char)((g1 + g2) / 2.0);
				lnBlu = (unsigned char)((b1 + b2) / 2.0);
				break;
		}

		// We rotate the colors
//		if (gnRotation != 0)
//		{
			r1 = (float)lnRed;
			g1 = (float)lnGrn;
			b1 = (float)lnBlu;
			// Note:  r2,g2,b2 will be computed below as our final output color

			// Determine our rotation as 0..2pi
			lfTheta = _2PI * (float)gnRotation / 255.0f;

			// Get colors at each interval (Red at red, red at grn, red at blu,
			iDeriveColorsRotated(lfTheta,			r1,g1,b1, r3,g3,b3);
			iDeriveColorsRotated(lfTheta + _2PI_3,	g1,b1,r1, r4,g4,b4);
			iDeriveColorsRotated(lfTheta + _4PI_3,	b1,r1,g1, r5,g5,b5);

			// Based on lfTheta, apply appropriate color interval
			lnRed = (unsigned char)((r3 + r4 + r5) / 2.0f);
			lnGrn = (unsigned char)((g3 + g4 + g5) / 2.0f);
			lnBlu = (unsigned char)((b3 + b4 + b5) / 2.0f);
//		}

		if (gnPastel != 50)
		{
			// Apply pastel effect
			lfMask	= (((float)gnPastel - 50.0f) / 50.0f);
			if (lfMask < 0.0f)
			{
				// It's gone negative
				lfMaskM = 1.0f + lfMask;
				lfMask = 0.0f;

			} else {
				// Still positive
				lfMaskM = 1.0f - lfMask;
			}

			// Pastel proportionally
			lnRed	= (unsigned char)min((lfMask * 255.0f) + (lfMaskM * (float)lnRed), 255.0f);
			lnGrn	= (unsigned char)min((lfMask * 255.0f) + (lfMaskM * (float)lnGrn), 255.0f);
			lnBlu	= (unsigned char)min((lfMask * 255.0f) + (lfMaskM * (float)lnBlu), 255.0f);
		}

		if (gnLinear != 50)
		{
			// Apply linear effect
			lfMaskM	= 1.0f + (((float)gnLinear - 50.0f) / 50.0f);

			// Linear proportionally
			lnRed	= (unsigned char)min(lfMaskM * (float)lnRed, 255.0f);
			lnGrn	= (unsigned char)min(lfMaskM * (float)lnGrn, 255.0f);
			lnBlu	= (unsigned char)min(lfMaskM * (float)lnBlu, 255.0f);
		}

		if (gnGray != 0)
		{
			// Apply grayscaling
			lfGray	= (float)lnRed * 0.35f + (float)lnGrn * 0.54f + (float)lnBlu * 0.11f;
			lfMask	= (float)(max(min(gnGray, 100), 0)) / 100.0f;
			lfMaskM	= 1.0f - lfMask;

			// Grayscale proportionally
			lnRed	= (unsigned char)((lfMask * lfGray) + (lfMaskM * (float)lnRed));
			lnGrn	= (unsigned char)((lfMask * lfGray) + (lfMaskM * (float)lnGrn));
			lnBlu	= (unsigned char)((lfMask * lfGray) + (lfMaskM * (float)lnBlu));
		}
	}

	void iDeriveColorsRotated(float tfTheta, float& tfR1, float& tfG1, float& tfB1, float& tfR2, float& tfG2, float& tfB2)
	{
		// Make sure it's >= 0.0 and <= _2PI
		while (tfTheta < 0.0)
			tfTheta += _2PI;
		while (tfTheta > _2PI)
			tfTheta -= _2PI;

		// Based on the range, adjust the color
		if (tfTheta >= 0.0f && tfTheta < _2PI_3)
		{
			// In the range 0..2*pi/3
			tfR2 = tfR1 * (_2PI_3 - tfTheta) / _2PI_3;
			tfG2 = tfG1;
			tfB2 = tfB1 * tfTheta / _2PI_3;

		} else if (tfTheta >= _2PI_3 && tfTheta < _4PI_3) {
			// In the range 2*pi/3..4*pi/3
			tfR2 = tfR1 * (tfTheta - _2PI_3) / _2PI_3;
			tfG2 = tfG1 * (_4PI_3 - tfTheta) / _2PI_3;
			tfB2 = tfB1;

		} else {
			// In the range 4*pi/3..2*pi
			tfR2 = tfR1;
			tfG2 = tfG1 * (tfTheta - _4PI_3) / _2PI_3;
			tfB2 = tfB1 * (1.0f - ((tfTheta - _4PI_3) / _2PI_3));
		}
	}

	void iDeriveColors(float theta, float& lfRed, float& lfGrn, float& lfBlu)
	{
		// Make sure it's >= 0.0 and <= _2PI
		while (theta < 0.0)
			theta += _2PI;
		while (theta > _2PI)
			theta -= _2PI;

		// See where its color falls
		if (theta >= 0.0 && theta < _2PI_3)
		{
			// Green if due 0
			lfRed = (float)0.0;
			lfGrn = (float)255.0 * cos(_PI2 * theta / _2PI_3);
			lfBlu = (float)255.0 * sin(_PI2 * theta / _2PI_3);

		} else if (theta >= _2PI_3 && theta < _4PI_3) {
			// Blue if due _2PI_3
			theta -= _2PI_3;
			lfRed = (float)255.0 * sin(_PI2 * theta / _2PI_3);
			lfGrn = (float)0.0;
			lfBlu = (float)255.0 * cos(_PI2 * theta / _2PI_3);

		} else {
			// It's >= _4PI_3 <= _2PI
			// Red if due _4PI_3
			theta -= _4PI_3;
			lfRed = (float)255.0 * cos(_PI2 * theta / _2PI_3);
			lfGrn = (float)255.0 * sin(_PI2 * theta / _2PI_3);
			lfBlu = (float)0.0;
		}
	}




//////////
//
// Gets the color at the indicated pixel
//
//////
	COLORWHEEL_API int colorwheel_get_rgb_at_xy(int* tnX, int* tnY)
	{
		SRGB* lrgb;


		// In the cases where we're not using our already computed rgb color, we have to extract the most recent
		// Note:  This could be shifted to the mouse inquiry area
		if (!glManualColor)
		{
			// Look into the bitmap and see what's at that location
			*tnX	= gnMouseX;
			*tnY	= gnMouseY;
			lrgb	= (SRGB*)(gcBits + ((gbmi.bmiHeader.biHeight - min(gnMouseY, gbmi.bmiHeader.biHeight - 1) - 1) * gnActualWidth) + (min(gnMouseX, gbmi.bmiHeader.biWidth) * 3));

			// Grab the color channels of that pixel
			gnRed	= lrgb->red;
			gnGrn	= lrgb->grn;
			gnBlu	= lrgb->blu;
		}

		// Return the now explicit color
		return(RGB(gnRed, gnGrn, gnBlu));
	}

	COLORWHEEL_API int colorwheel_set_rgb(int tnRgb)
	{
		glManualColor = true;
		gnRed = (tnRgb & 0x0000ff);
		gnGrn = (tnRgb & 0x00ff00) >> 8;
		gnBlu = (tnRgb & 0xff0000) >> 16;
		return(0);
	}




//////////
//
// Callback to intercept the HWND to draw the overlain color image
//
//////
	LRESULT CALLBACK colorChartWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		HDC			lhdc;
		RECT		lrc;
		PAINTSTRUCT ps;
		

		// If we are painting, paint our areas
		if (hwnd == gnHwnd)
		{
			switch (uMsg)
			{
				case WM_PAINT:
					// Get the client rect
					GetClientRect(hwnd, &lrc);

					// Begin painting
					lhdc = BeginPaint(hwnd, &ps);

					// Draw our bitmap
					BitBlt(	lhdc, 
							0, 0, lrc.right, lrc.bottom,
							gnHdc2, 0, 0, SRCCOPY);

					// End painting
					EndPaint(hwnd, &ps);
					ValidateRect(hwnd, &lrc);
					break;
			}
		}

		// Do normal drawing
		return(DefWindowProc(hwnd, uMsg, wParam, lParam));
	}

	void CALLBACK TimerProc(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime)
	{
		POINT	pt;
		RECT	rectc;
		RECT	rectp;


		// Check to see if the left mouse button is down
		if (GetAsyncKeyState(VK_LBUTTON) != 0)
		{
			// Find out where the cursor is
			GetCursorPos(&pt);
			GetWindowRect(gnHwndParent, &rectp);
			GetWindowRect(hWnd, &rectc);

			// Are we within our rectangle?
			if (PtInRect(&rectc, pt))
			{
				glManualColor = false;
				gnMouseX = pt.x - rectc.left;
				gnMouseY = pt.y - rectc.top;
			}
		}
	}