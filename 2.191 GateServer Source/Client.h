// Client.h: interface for the CClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLIENT_H__2CB6AC23_DFA6_11D2_B145_00001C7030A6__INCLUDED_)
#define AFX_CLIENT_H__2CB6AC23_DFA6_11D2_B145_00001C7030A6__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <windows.h>
#include "XSocket.h"
#include "UserMessages.h"


#define DEF_MSGBUFFERSIZE	30000

class CClient  
{
public:
	CClient(HWND hWnd);
	virtual ~CClient();

	bool   m_bIsGameServer;		// ���Ӽ����ΰ� ������ΰ� �Ǻ��� 
	bool   m_bIsShutDown;       // v2.17 �˴ٿ� ���ΰ� �Ǻ���
	char   m_cName[11];			// ���� ������ �̸�. 
	char   m_cAddress[16];		// ���� ������ �ּ� 
	DWORD  m_dwProcessHandle;	// ���Ӽ����� ���μ��� �ڵ� 
	DWORD  m_dwAliveTime;		// ���Ӽ����κ����� ��������� �˸��� �޽���
	int    m_iTotalClients;		// ���Ӽ����� �� ����� �� 
	// v2.17 2002-6-3 ���� ����
	short   m_sIsActivated;


	class XSocket * m_pXSock;
};

#endif // !defined(AFX_CLIENT_H__2CB6AC23_DFA6_11D2_B145_00001C7030A6__INCLUDED_)
