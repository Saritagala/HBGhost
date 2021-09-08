// DXC_ddraw.h: interface for the DXC_ddraw class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DXC_DDRAW_H__C52EBA83_6D9E_11D2_A8E6_00001C7030A6__INCLUDED_)
#define AFX_DXC_DDRAW_H__C52EBA83_6D9E_11D2_A8E6_00001C7030A6__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <math.h>
#include <stdio.h>
#include <windows.h>
#include <memory.h>
#include <string.h>
#include "ddraw.h"
#include "Misc.h"
#include "GlobalDef.h"
//#include "Game.h"

class DXC_ddraw  
{
public:	
	void * operator new (size_t size) 
	{
		return HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);	
	};	
	void operator delete(void * mem)
	{
		HeapFree(GetProcessHeap(), HEAP_NO_SERIALIZE, mem); 		
	};

	DXC_ddraw();
	virtual ~DXC_ddraw();
	bool bInit(HWND hWnd);
	void ColorTransferRGB(COLORREF fcolor, int * iR, int * iG, int * iB);
	HRESULT InitFlipToGDI(HWND hWnd);
	void ChangeDisplayMode(HWND hWnd);
	void DrawText(LPRECT pRect, char * pString, COLORREF rgb);
	void _ReleaseBackBufferDC();
	void _GetBackBufferDC();
	void TextOut(int x, int y, char * cStr, COLORREF rgb);
	void _TestPixelFormat();
	UINT32 _dwColorMatch(IDirectDrawSurface7 * pdds4, UINT16 wColorKey);
	UINT32 _dwColorMatch(IDirectDrawSurface7 * pdds4, COLORREF rgb);
	long _CalcMinValue(int iS, int iD, char cMode);
	long _CalcMaxValue(int iS, int iD, char cMode, char cMethod, float dAlpha);
	HRESULT iSetColorKey(IDirectDrawSurface7 * pdds4, UINT16 wColorKey);
	void PutPixel(short sX, short sY, UINT16 wR, UINT16 wG, UINT16 wB);
	void DrawShadowBox(short sX, short sY, short dX, short dY, int iType = 0, bool border = false);
	void ClearBackB4();
	IDirectDrawSurface7 * pCreateOffScreenSurface(UINT16 iSzX, UINT16 iSzY);
	HRESULT iSetColorKey(IDirectDrawSurface7 * pdds4, COLORREF rgb);
	HRESULT iFlip();
	bool Screenshot(LPCTSTR FileName, LPDIRECTDRAWSURFACE7 lpDDS);

	//class CGame* m_pGame;
	
	long    m_lTransG100[64][64], m_lTransRB100[64][64];
	long    m_lTransG70[64][64], m_lTransRB70[64][64];
	long    m_lTransG50[64][64], m_lTransRB50[64][64];
	long    m_lTransG25[64][64], m_lTransRB25[64][64];
	long    m_lTransG2[64][64], m_lTransRB2[64][64];
	long    m_lFadeG[64][64],  m_lFadeRB[64][64];

	bool m_bFullMode;

	LPDIRECTDRAW7		 m_lpDD4;
	LPDIRECTDRAWSURFACE7 m_lpFrontB4, m_lpBackB4, m_lpBackB4flip;
	LPDIRECTDRAWSURFACE7 m_lpPDBGS;// Pre-Draw Background Surface
	UINT16 * m_pBackB4Addr;
	RECT  m_rcClipArea, m_rcFlipping;
	short  m_sBackB4Pitch;
	char   m_cPixelFormat;
	HDC m_hDC;
	HFONT m_hFontInUse;

	bool m_init;

	bool IsWin2();
};

#endif // !defined(AFX_DXC_DDRAW_H__C52EBA83_6D9E_11D2_A8E6_00001C7030A6__INCLUDED_)
