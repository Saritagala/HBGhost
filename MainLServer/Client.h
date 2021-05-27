#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "XSocket.h"
#include "NetMessages.h"

#define DEF_CLIENTSOCKETBLOCKLIMIT	15

class CClient  
{
public:
	CClient(HWND hWnd);
	virtual ~CClient();
	class XSocket * m_pXSock;
	char  m_cIPaddress[16];			// 10h
	char  m_cWorldName[31];
	char  m_cWorldServerAddress[16];
	int   m_iWorldPort;
	char  m_cMode;					// 4h
};