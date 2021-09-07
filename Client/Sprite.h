// Sprite.h: interface for the CSprite class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPRITE_H__0089D9E2_74E6_11D2_A8E6_00001C7030A6__INCLUDED_)
#define AFX_SPRITE_H__0089D9E2_74E6_11D2_A8E6_00001C7030A6__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <vector>

#include "DXC_ddraw.h"
#include "Mydib.h"

typedef struct stBrushtag
{
	short sx;
	short sy;
	short szx;
	short szy;
	short pvx;
	short pvy;
} stBrush;

void ReadFramePositions(HANDLE hPakFile, std::vector<int>& framePositions, int frames);

class CSprite  
{
public:
	void * operator new (size_t size) 
	{	return HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);
	};
	
	void operator delete(void * mem)
	{	HeapFree(GetProcessHeap(), HEAP_NO_SERIALIZE, mem);
	};

	CSprite(HANDLE hPakFile, class DXC_ddraw * pDDraw, char * cPakFileName, short sNthFile, bool bAlphaEffect = true, std::vector<int>* framePositions = 0);
	virtual ~CSprite();

	void PutSpriteRGB(int sX, int sY, int sFrame, int sRed, int sGreen, int sBlue, UINT32 dwTime);

	void PutSpriteFast(int sX, int sY, int sFrame, UINT32 dwTime);
	void PutShiftSpriteFast(int sX, int sY, int shX, int shY, int sFrame, UINT32 dwTime);
	void PutShiftTransSprite2(int sX, int sY, int shX, int shY, int sFrame, UINT32 dwTime);
	void PutSpriteFastFrontBuffer(int sX, int sY, int sFrame, UINT32 dwTime);
	void PutSpriteFastWidth(int sX, int sY, int sFrame, int sWidth, UINT32 dwTime, bool bVertical);
	void PutSpriteFastNoColorKey(int sX, int sY, int sFrame, UINT32 dwTime);
	void PutSpriteFastDst(LPDIRECTDRAWSURFACE7 lpDstS, int sX, int sY, int sFrame, UINT32 dwTime);
	void PutSpriteFastNoColorKeyDst(LPDIRECTDRAWSURFACE7 lpDstS, int sX, int sY, int sFrame, UINT32 dwTime);

	void PutTransSprite(int sX, int sY, int sFrame, UINT32 dwTime, int alphaDepth = 30);
	void PutTransSprite2(int sX, int sY, int sFrame, UINT32 dwTime);
	void PutTransSprite_NoColorKey(int sX, int sY, int sFrame, UINT32 dwTime, int alphaDepth = 0);
	void PutTransSpriteRGB_NoColorKey(int sX, int sY, int sFrame, int sRed, int sGreen, int sBlue, UINT32 dwTime);
	void PutTransSpriteRGB(int sX, int sY, int sFrame, int sRed, int sGreen, int sBlue, UINT32 dwTime);
	void PutTransSprite70(int sX, int sY, int sFrame, UINT32 dwTime);
	void PutTransSprite50(int sX, int sY, int sFrame, UINT32 dwTime);
	void PutTransSprite25(int sX, int sY, int sFrame, UINT32 dwTime);
	void PutTransSprite70_NoColorKey(int sX, int sY, int sFrame, UINT32 dwTime);
	void PutTransSprite50_NoColorKey(int sX, int sY, int sFrame, UINT32 dwTime);
	void PutTransSprite25_NoColorKey(int sX, int sY, int sFrame, UINT32 dwTime);

	void PutShadowSpriteClip(int sX, int sY, int sFrame, UINT32 dwTime);
	void PutShadowSprite(int sX, int sY, int sFrame, UINT32 dwTime);

	void PutRevTransSprite(int sX, int sY, int sFrame, UINT32 dwTime, int alphaDepth = 0);

	void PutFadeSprite(short sX, short sY, short sFrame, UINT32 dwTime);
	void PutFadeSpriteDst(UINT16 * pDstAddr, short sPitch, short sX, short sY, short sFrame, UINT32 dwTime);


	void _SetAlphaDegree();
	bool _bCheckCollison(int sX, int sY, short sFrame, int msX, int msY);
	void _GetSpriteRect(int sX, int sY, int sFrame);
	void _iCloseSprite();
	bool _iOpenSprite();
	void iRestore();
	IDirectDrawSurface7 *  _pMakeSpriteSurface();
		
	RECT	m_rcBound;
	UINT32	m_dwRefTime;
	bool	m_bIsSurfaceEmpty;
	bool	m_bOnCriticalSection;
	bool	m_bAlphaEffect;
	short	m_sPivotX, m_sPivotY;
	class	DXC_ddraw * m_pDDraw;
	UINT16*	m_pSurfaceAddr;
	UINT32	m_dwBitmapFileStartLoc;
	short	m_sPitch;
	int		m_iTotalFrame;
	char	m_cAlphaDegree;
	UINT16	m_wBitmapSizeX, m_wBitmapSizeY;
	UINT16	m_wColorKey;
	char	m_cPakFileName[16];
	stBrush* m_stBrush;
	LPDIRECTDRAWSURFACE7 m_lpSurface;
};

#endif // !defined(AFX_SPRITE_H__0089D9E2_74E6_11D2_A8E6_00001C7030A6__INCLUDED_)
