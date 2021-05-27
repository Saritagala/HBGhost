#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "NetMessages.h"


class CGame  
{
public:
	CGame (int iClientH, char *cGameServerName, char *cGameServerIP, int iGameServerPort, char *cWorldName, int iTotalMaps);
	virtual ~CGame ();
	char  m_cGameName[11];
	char  m_cGameServerAddress[16];
	int   m_iGamePort;
	char  m_cWorldName[31];					// 4h
	short m_iTotalMaps;
	int m_iTrackSock; //
};