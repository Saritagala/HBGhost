// SoundBuffer.h: interface for the CSoundBuffer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOUNDBUFFER_H__A0D1F3DC_F322_4310_9295_88EAD41F19DA__INCLUDED_)
#define AFX_SOUNDBUFFER_H__A0D1F3DC_F322_4310_9295_88EAD41F19DA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "dsound.h"

#define DEF_MAXSOUNDBUFFERS		2	


class CSoundBuffer  
{
public:
	void _ReleaseSoundBuffer();
	void bStop(bool bIsNoRewind = false);
	void SetVolume(LONG Volume);
	LPDIRECTSOUNDBUFFER GetIdleBuffer();
	bool Play(bool bLoop = false, long lPan = 0, int iVol = 0);
	bool _LoadWavContents(char cBufferIndex, FILE* pFile, DWORD dwSize, DWORD dwPos);
	bool bCreateBuffer_LoadWavFileContents(char cBufferIndex);
	bool _bCreateSoundBuffer(char cBufferIndex, UINT32 dwBufSize, DWORD dwFreq, DWORD dwBitsPerSample, DWORD dwBlkAlign, bool bStereo);
	CSoundBuffer(LPDIRECTSOUND lpDS, DSCAPS DSCaps, char * pWavFileName, bool bIsSingleLoad = false);
	virtual ~CSoundBuffer();

	LPDIRECTSOUND	m_lpDS;
	DSCAPS			m_DSCaps;

	char				m_cWavFileName[32];
	LPDIRECTSOUNDBUFFER m_lpDSB[DEF_MAXSOUNDBUFFERS];
	char				m_cCurrentBufferIndex;

	bool				m_bIsSingleLoad;
	bool				m_bIsLooping;
	DWORD				m_dwTime;
};

#endif // !defined(AFX_SOUNDBUFFER_H__A0D1F3DC_F322_4310_9295_88EAD41F19DA__INCLUDED_)
