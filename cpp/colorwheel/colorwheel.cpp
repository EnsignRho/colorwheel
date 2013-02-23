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


// Colors for generating the color bar
int			gnRed			= 255;
int			gnGrn			= 255;
int			gnBlu			= 255;
int			gnGray			= 0;
int			gnPastel		= 0;
int			gnLinear		= 0;




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
			gnHwnd = CreateWindow(L"colorChart", L"colorChart", WS_CHILD, tnX, tnY, tnWidth, tnHeight, gnHwndParent, NULL, wcex.hInstance, NULL);
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
	}




//////////
//
// Called to un-subclass the form
//
//////
	COLORWHEEL_API void colorwheel_un_subclass_form(void)
	{
		SetWindowLong(gnHwnd, GWL_WNDPROC, (long)lnOldWndProcAddress);
	}




//////////
//
// Called to set the color values for the next color render operation
//
//////
	COLORWHEEL_API void colorwheel_set_rgb_grayscale_adjustments(int tnRed, int tnGrn, int tnBlu, int tnGray, int tnPastel, int tnLinear)
	{
		// These are standard RGB values
		gnRed		= max(min(tnRed, 255), 0);
		gnGrn		= max(min(tnGrn, 255), 0);
		gnBlu		= max(min(tnBlu, 255), 0);

		// These are all percents
		gnGray		= max(min(tnBlu, 100), 0);
		gnPastel	= max(min(tnBlu, 100), 0);
		gnLinear	= max(min(tnBlu, 100), 0);
	}




//////////
//
// Gets the color at the indicated pixel
//
//////
	COLORWHEEL_API int colorwheel_get_rgb_at_xy(int x, int y)
	{
		// Look into the bitmap and see what's at that location
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
		if (hwnd == gnHwnd && uMsg == WM_PAINT)
		{
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
		}

		// Do normal drawing
		return(DefWindowProc(hwnd, uMsg, wParam, lParam));
	}
