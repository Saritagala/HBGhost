// Client.cpp: implementation of the CClient class.
//
//////////////////////////////////////////////////////////////////////

#include "Client.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CClient::CClient(HWND hWnd)
{
	m_pXSock = 0;
	m_pXSock = new class XSocket(hWnd, 300);
	m_pXSock->bInitBufferSize(DEF_MSGBUFFERSIZE);

	ZeroMemory(m_cName, sizeof(m_cName));
	ZeroMemory(m_cAddress, sizeof(m_cAddress));

	m_bIsGameServer = false;
	// v2.17 2002-6-3 ���� ���� 
	m_sIsActivated = 0;
	// v2.17 2002-6-10 ������ ���� 
	m_bIsShutDown = false ;

	m_iTotalClients = 0;
	
}

CClient::~CClient()
{
	if (m_pXSock != 0) delete m_pXSock;
}
