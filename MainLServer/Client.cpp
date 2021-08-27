#include "Client.h"

CClient::CClient(HWND hWnd)
{
	m_pXSock = 0;
	m_pXSock = new class XSocket(hWnd, DEF_CLIENTSOCKETBLOCKLIMIT);
	m_pXSock->bInitBufferSize(DEF_MSGBUFFERSIZE);
	ZeroMemory(m_cIPaddress, sizeof(m_cIPaddress));
	ZeroMemory(m_cWorldName, sizeof(m_cWorldName));
	ZeroMemory(m_cWorldServerAddress, sizeof(m_cWorldServerAddress));
	m_cMode = 0;;
	m_iWorldPort = 0;
}
CClient::~CClient()
{
if (m_pXSock != 0) delete m_pXSock;
}