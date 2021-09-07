// GateCore.h: interface for the CGateCore class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GATECORE_H__2CB6AC22_DFA6_11D2_B145_00001C7030A6__INCLUDED_)
#define AFX_GATECORE_H__2CB6AC22_DFA6_11D2_B145_00001C7030A6__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <direct.h>
#include <winbase.h>
#include <mmsystem.h>
#include <process.h>
#include <stddef.h>
#include "Client.h"
#include "MessageIndex.h"
#include "NetMessages.h"
#include "PartyManager.h"


#define DEF_MAXCLIENTS		500
#define DEF_MAXGAMESERVERS	20
#define DEF_NORESPONSELIMIT 30000	// 30�ʰ� ������ ������ ������ ������������ ����� ���̴�.
#define DEF_MAXMONITORS		10

class CGateCore  
{
public:
	void PartyOperation(int iClientH, char * pData);
	void SendMsgToMonitor(char * pData, UINT32 dwMsgSize);
	void CheckAutoShutdownProcess();
	void SendMsgToAllGameServers(int iClientH, char * pData, UINT32 dwMsgSize, bool bIsOwnSend = true);
	void ItemLog(char * pData);
	void _SendServerShutDownMsg(int iClientH, char cCode);
	void SendServerShutDownMsg(char cCode, bool bISShutdown = false);
	void CheckGlobalCommand();
	void OnKeyDown(WPARAM wParam, LPARAM lParam);
	void _SendTotalGameServerClients(int iClientH, int iTotalClients);
	void SendTotalClientsToGameServer();
	void SendMsgToMonitor(UINT32 dwMsg, UINT16 wMsgType, char * pGameServerName, short sTotalClient = 0);
	void CheckGameServerActivity();
	void OnTimer(int iID);
	void OnKeyUp(WPARAM wParam, LPARAM lParam);
	void ResponseRegisterGameServer(int iClientH, bool bRet);
	void RegisterGameServerHandler(int iClientH, char * pData);
	void OnClientRead(int iClientH);
	bool bAccept(class XSocket * pXSock);
	void OnClientSocketEvent(UINT message, WPARAM wParam, LPARAM lParam);
	bool bReadProgramConfigFile(char * cFn);
	bool bInit();

	bool m_bBlockGameServer;
	
	CGateCore(HWND hWnd);
	virtual ~CGateCore();

	HWND m_hWnd;
	char m_cGateServerAddr[16];
	int  m_iGateServerPort;

	bool m_bF1pressed, m_bF12pressed;

	bool  m_bIsAutoShutdownProcess;
	int   m_iAutoShutdownCount;
	UINT32 m_dwAutoShutdownTime;

	//2002-11-27�� ���� ����
	char m_cAddress[10][16];	

	// v2.17 ���� ���� 
	int m_iBuildDate;
	int iTotalWorldServerClients; 
	char m_cMonitorStatus[DEF_MAXMONITORS];

	class CClient * m_pClientList[DEF_MAXCLIENTS];
	class PartyManager * m_pPartyManager;
};

#endif // !defined(AFX_GATECORE_H__2CB6AC22_DFA6_11D2_B145_00001C7030A6__INCLUDED_)
