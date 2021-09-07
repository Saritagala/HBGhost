// GateCore.cpp: implementation of the CGateCore class.
//
//////////////////////////////////////////////////////////////////////

#include "GateCore.h"

extern char G_cTxt[120];
extern void PutLogList(char * cMsg);
extern void PutItemLogFileList(char * cStr);
extern void PutLogFileList(char * cStr);

extern unsigned __stdcall ThreadProc(void *ch);
extern bool G_bThreadFlag;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGateCore::CGateCore(HWND hWnd)
{
 int i;
	
	m_hWnd = hWnd;
	m_bF1pressed = m_bF12pressed = false;

	m_iAutoShutdownCount = 10;

	// v2.17 2002-6-3 ���� ���� 
	m_iBuildDate = 0;

	for (i = 0; i < DEF_MAXCLIENTS; i++)
		m_pClientList[i] = 0;

	for (i = 0; i < DEF_MAXMONITORS; i++)
		m_cMonitorStatus[i] = 0;

	memset(m_cAddress,0,10*16);

	iTotalWorldServerClients = 0 ;

	m_pPartyManager = new class PartyManager(this);

	m_bBlockGameServer = false;
}


CGateCore::~CGateCore()
{
 int i;

 	// �����带 ���δ�.
	G_bThreadFlag = false;

	for (i = 0; i < DEF_MAXCLIENTS; i++)
	if (m_pClientList[i] != 0) 
	{
		delete m_pClientList[i];
		m_pClientList[i] = 0 ;
	}

	delete m_pPartyManager;
}


bool CGateCore::bInit()
{
	if (bReadProgramConfigFile("GateServer.cfg") == false) return false;

	unsigned int uAddr;
	_beginthreadex(0, 0, ThreadProc, 0, 0, &uAddr);

	return true;
}

bool CGateCore::bReadProgramConfigFile(char * cFn)
{
 FILE * pFile;
 HANDLE hFile;
 UINT32  dwFileSize;
 char * cp, * token, cReadMode, cTxt[120];
 char seps[] = "= ,\t\n";
 int	iIPindex = 0;

	cReadMode = 0;

	hFile = CreateFile(cFn, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
	dwFileSize = GetFileSize(hFile, 0);
	if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);

	ZeroMemory(m_cGateServerAddr, sizeof(m_cGateServerAddr));

	pFile = fopen(cFn, "rt");
	if (pFile == 0) {
		// �α׼����� �ʱ�ȭ ������ ���� �� ����.
		PutLogList("(!) Cannot open configuration file.");
		return false;
	}
	else {
		PutLogList("(!) Reading configuration file...");
		cp = new char[dwFileSize+1];
		ZeroMemory(cp, dwFileSize+1);
		fread(cp, dwFileSize, 1, pFile);

		token = strtok( cp, seps );   
		while( token != 0 )   {
			
			if (cReadMode != 0) {
				switch (cReadMode) {
				case 1:
				
				
					m_iGateServerPort = atoi(token);
					wsprintf(cTxt, "(*) Gate-server port : %d", m_iGateServerPort);
					PutLogList(cTxt);
					cReadMode = 0;
					break;

				// v2.17 2002-6-3 ���� ���� 
				case 2:
					m_iBuildDate = atoi(token);
					wsprintf(cTxt, "(*) Build Date : %d", m_iBuildDate);
					PutLogList(cTxt);
					cReadMode = 0;
					break; 

				case 3:
					strcpy(m_cAddress[iIPindex],token);	
					iIPindex++;
					cReadMode = 0;
					break; 
				case 4:
					ZeroMemory(m_cGateServerAddr, sizeof(m_cGateServerAddr));
					
					strcpy(m_cGateServerAddr,token );
					wsprintf(cTxt, "(*) Gate-server address : %s", m_cGateServerAddr);
					PutLogList(cTxt);
					cReadMode = 0;
					break;
				} 

			}
			else {
				if (memcmp(token, "gate-server-port", 15) == 0)     cReadMode = 1;
				// v2.17 2002-6-3 ���� ����
				if (memcmp(token, "build-date", 10) == 0) cReadMode = 2;
				if (memcmp(token, "game-server-list",16) == 0) cReadMode = 3; //2002-11-27�� ���� ����
				if (memcmp(token, "gate-server-address",19) == 0) cReadMode = 4 ;//2003-1-17�� ������ �߰� IP�뿪 �ΰ� ���°�츦 ���ؼ� 
				
			}
			token = strtok( 0, seps );
		}

		delete[] cp;
		fclose(pFile);
	}


	//2003-1-17�� ������ �߰� ����Ʈ ���� �����Ǹ� �ʳ־� �ִ� ��� 
	/*if ( m_cGateServerAddr == 0)
	{
				
		char ServerAddr[50];
		::gethostname(ServerAddr,50); 
		struct hostent *pHostEnt;
		pHostEnt = ::gethostbyname(ServerAddr);
		if( pHostEnt != 0 ){
			wsprintf(ServerAddr, "%d.%d.%d.%d",
			( pHostEnt->h_addr_list[0][0] & 0x00ff ),
			( pHostEnt->h_addr_list[0][1] & 0x00ff ),
			( pHostEnt->h_addr_list[0][2] & 0x00ff ),
			( pHostEnt->h_addr_list[0][3] & 0x00ff ) );
		}
		strcpy(m_cGateServerAddr,ServerAddr );

		wsprintf(cTxt, "(*) Gate server address : %s", m_cGateServerAddr);
		PutLogList(cTxt);
	}*/

	return true;

}

void CGateCore::OnClientSocketEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
 int iClientH, iTemp, iRet;

	iTemp = WM_ONCLIENTSOCKETEVENT;
	iClientH = (int)(message - iTemp);

	if (m_pClientList[iClientH] == 0) return;

	iRet = m_pClientList[iClientH]->m_pXSock->iOnSocketEvent(wParam, lParam);
	switch (iRet) {
	case DEF_XSOCKEVENT_READCOMPLETE:
		// �޽����� ���ŵǾ���. 
		OnClientRead(iClientH);
		break;
	
	case DEF_XSOCKEVENT_BLOCK:
		PutLogList("Socket BLOCKED!");
		break;
	
	case DEF_XSOCKEVENT_CONFIRMCODENOTMATCH:
		wsprintf(G_cTxt,"<%d> Confirmcode notmatch!", iClientH);
		PutLogList(G_cTxt);
		delete m_pClientList[iClientH];
		m_pClientList[iClientH] = 0;
		break;
	case DEF_XSOCKEVENT_CRITICALERROR:
		// ġ������ �����̴�.
	case DEF_XSOCKEVENT_QUENEFULL:
		// ť�� ����á��.
	case DEF_XSOCKEVENT_MSGSIZETOOLARGE:
		// �����ؾ� �� �޽��� ũ�Ⱑ ���ۺ��� ũ��.	�����ؾ߸� �Ѵ�.
	case DEF_XSOCKEVENT_SOCKETERROR:
		// ���Ͽ� ������ ����.
	case DEF_XSOCKEVENT_SOCKETCLOSED:
		// ������ ������.
		if (m_pClientList[iClientH]->m_bIsGameServer == true) {
			wsprintf(G_cTxt,"*** CRITICAL ERROR! Game Server(%s) connection lost!!! ***", m_pClientList[iClientH]->m_cName);
			// ���� �ٿ��� �˸���.
			SendMsgToMonitor(MSGID_GAMESERVERDOWN, DEF_MSGTYPE_CONFIRM, m_pClientList[iClientH]->m_cName);
			m_pPartyManager->GameServerDown(iClientH);
  		}
		else wsprintf(G_cTxt,"WARNING! Monitor(%d) connection lost!", iClientH);
		PutLogList(G_cTxt);
		delete m_pClientList[iClientH];
		m_pClientList[iClientH] = 0;
		break;
	}
}

bool CGateCore::bAccept(class XSocket * pXSock)
{
 int i, iTotalip = 0;
 class XSocket * pTmpSock;
 bool	bTrue = false;

	// ����ִ� �迭�� ã�´�. 
	for (i = 1; i < DEF_MAXCLIENTS; i++)
	if (m_pClientList[i] == 0) {
		
		m_pClientList[i] = new class CClient(m_hWnd);

		pXSock->bAccept(m_pClientList[i]->m_pXSock, WM_ONCLIENTSOCKETEVENT + i); 
		m_pClientList[i]->m_pXSock->bInitBufferSize(DEF_MSGBUFFERSIZE);

		m_pClientList[i]->m_pXSock->iGetPeerAddress(m_pClientList[i]->m_cAddress);  //2002-11-27�� ���� ����

		//centu: Anti-Downer
		for (int x = 1; x < DEF_MAXCLIENTS; x++) 
			if(m_pClientList[x] != 0) { 
				if(strcmp(m_pClientList[x]->m_cAddress, m_pClientList[i]->m_cAddress) == 0) iTotalip++; 
			} 
			if(iTotalip > 10) {
				delete m_pClientList[i]; 
				m_pClientList[i] = 0; 
				return false; 
			} 
			/*if (strlen(m_pClientList[i]->m_cAddress) < 10) {
				delete m_pClientList[i]; 
				m_pClientList[i] = 0; 
				return false; 
			}*/

		for(int j = 0;j < 10 ; j++)  //2002-11-27�� ���� ���� �㰡�� ������ üũ
		{
			if(memcmp(m_cAddress[j],m_pClientList[i]->m_cAddress,16) == 0) 
			{
				bTrue = true;
				break;
			}
			
		}
		
		if(bTrue != true)
		{
			wsprintf(G_cTxt,"<%d> <%s> ���� ��Ŀ��.", i,m_pClientList[i]->m_cAddress);
			PutLogFileList(G_cTxt);
			
			delete m_pClientList[i] ;
			m_pClientList[i]  = 0 ;
			
			return false;
		}

		wsprintf(G_cTxt,"<%d> Accept! Add New Client.", i);
		PutLogList(G_cTxt);

		m_pClientList[i]->m_dwAliveTime = timeGetTime();
		return true;
	}

	// ����ִ� �迭�� ���� ������ ���� �� ����. Accept�� �� �׳� ���´�.
	pTmpSock = new class XSocket(m_hWnd, 300);
	pXSock->bAccept(pTmpSock, 0); 
	delete pTmpSock;
	//delete pXSock;

	return false;
}

void CGateCore::OnClientRead(int iClientH)
{
 char  * cp, * pData, cTemp[22],cTxt[256] ;
 UINT32 * dwpMsgID, dwMsgSize, dwTime, * dwp ;
 UINT16  * wp;
 int i , iRet; 
 bool bFindGameServer= false ; 
 	
	dwTime = timeGetTime();
	pData = m_pClientList[iClientH]->m_pXSock->pGetRcvDataPointer(&dwMsgSize);
	
	dwpMsgID = (UINT32 *)(pData + DEF_INDEX4_MSGID);

	
	switch (*dwpMsgID) {
	case MSGID_PARTYOPERATION:
		PartyOperation(iClientH, pData);
		break;
	
	case MSGID_SERVERSTOCKMSG:
		// ���� �޽����� ��� ���� �����鿡�� �� ����
		SendMsgToAllGameServers(iClientH, pData, dwMsgSize, false);
		break;
	case MSGID_REQUEST_GAMESERVER_SHUTDOWN:
		cp  = (char *)(pData + DEF_INDEX2_MSGTYPE);
		wp  = (UINT16 *)cp;
		if (*wp != 0x5A8E) {
			// Ȯ���ڵ� ����ġ. ����.
			delete m_pClientList[iClientH];
			m_pClientList[iClientH] = 0;
			return;
		}
		cp += 2 ;
		if (memcmp(cp, "1dkld$#@01", 10 ) != 0) 
			return ;

		cp += 10 ;
		
		strcpy(cTemp,cp) ;

		for (i = 1; i < DEF_MAXCLIENTS; i++) 
			if ((m_pClientList[i] != 0) && ( strcmp(m_pClientList[i]->m_cName,cTemp) == 0) 
					&& ( m_pClientList[i]->m_bIsGameServer == true)) {

				ZeroMemory(cTxt, sizeof(cTxt));
				dwp  = (UINT32 *)(cTxt + DEF_INDEX4_MSGID);
				*dwp = MSGID_GAMESERVERSHUTDOWNED;
				wp   = (UINT16 *)(cTxt + DEF_INDEX2_MSGTYPE);
				*wp  = DEF_MSGTYPE_CONFIRM;
		
				iRet = m_pClientList[i]->m_pXSock->iSendMsg(cTxt, 6);

				switch (iRet) {
				case DEF_XSOCKEVENT_MSGSIZETOOLARGE:
				case DEF_XSOCKEVENT_QUENEFULL:
				case DEF_XSOCKEVENT_SOCKETERROR:
				case DEF_XSOCKEVENT_CRITICALERROR:
				case DEF_XSOCKEVENT_SOCKETCLOSED:
					// �޽����� ������ ������ �߻��ߴٸ� �����Ѵ�.
					memcpy(cTemp,m_pClientList[i]->m_cName,20) ;

					delete m_pClientList[i];
					m_pClientList[i] = 0;

					wsprintf(G_cTxt,"*** CRITICAL ERROR! Game Server(%s) connection lost!!! ***", cTemp);
					PutLogList(G_cTxt);
					// ���� �ٿ��� �˸���.
					SendMsgToMonitor(MSGID_GAMESERVERDOWN, DEF_MSGTYPE_CONFIRM, cTemp);
					break;
				}
				bFindGameServer = true ;
			}
			
		if( bFindGameServer == false) {
			wsprintf(cTxt,"(!) AUTO SHUTDOWN PROCESS (%s) SERVER CAN'T FIND !",cTemp) ;
			PutLogList(cTxt) ;
		}
			
		break; 

	case MSGID_REQUEST_ALLGAMESERVER_SHUTDOWN:
		cp  = (char *)(pData + DEF_INDEX2_MSGTYPE);
		wp  = (UINT16 *)cp;
		if (*wp != 0x5A8E) {
			// Ȯ���ڵ� ����ġ. ����.
			delete m_pClientList[iClientH];
			m_pClientList[iClientH] = 0;
			return;
		}
		cp += 2 ;

		if (memcmp(cp, "1dkld$#@01", 10 ) != 0) 
			return ;

		PutLogList(" ");
		PutLogList("(!) AUTO SHUTDOWN PROCESS STARTED!");

		for (i = 1; i < DEF_MAXCLIENTS; i++) 
			if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_bIsGameServer == true)) 
				m_pClientList[i]->m_bIsShutDown = true ;
		
		break ;


	
	case MSGID_REQUEST_ALL_SERVER_REBOOT:
	case MSGID_REQUEST_EXEC_1DOTBAT:
	case MSGID_REQUEST_EXEC_2DOTBAT:
		cp  = (char *)(pData + DEF_INDEX2_MSGTYPE);
		wp  = (UINT16 *)cp;
		if (*wp != 0x5A8E) {
			// Ȯ���ڵ� ����ġ. ����.
			delete m_pClientList[iClientH];
			m_pClientList[iClientH] = 0;
			return;
		}
		// ���� �޽����� �״�� ��� ����Ϳ��� ����.
		SendMsgToMonitor(pData, dwMsgSize);
		break;

	case MSGID_GATEWAY_CURRENTCONN:
		SendMsgToMonitor(pData, dwMsgSize);
		break;

	case MSGID_MONITORALIVE:
		cp  = (char *)(pData + DEF_INDEX2_MSGTYPE);
		wp  = (UINT16 *)cp;

		if (m_cMonitorStatus[*wp] != 1) 
			 m_cMonitorStatus[*wp] = 1;
		else m_cMonitorStatus[*wp] = 2;
		InvalidateRect(m_hWnd, 0, true);

		// ���� �޽����� �״�� ��� ����Ϳ��� ����.
		char cTemp[20] ;
		ZeroMemory(cTemp,sizeof(cTemp)) ;
		wsprintf(cTemp,"Total User %d",iTotalWorldServerClients) ;
		SendMsgToMonitor(*dwpMsgID, *wp, cTemp);
		break;
		
	case MSGID_ITEMLOG:
		// ������ ���� �α� 
		ItemLog(pData);
		break;
	
	case MSGID_REQUEST_REGISTERGAMESERVER:
		// ���Ӽ����κ��� ��� ��û 
		RegisterGameServerHandler(iClientH, pData);
		break;

	case MSGID_GAMESERVERALIVE:
		// ���Ӽ����� ��� ������ �˸��� �޽���
		m_pClientList[iClientH]->m_dwAliveTime = dwTime;
		cp  = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
		wp  = (UINT16 *)cp;
		m_pClientList[iClientH]->m_iTotalClients = (int)*wp;

		if (m_pClientList[iClientH]->m_sIsActivated >= 0) 
			m_pClientList[iClientH]->m_sIsActivated = 1;

		SendMsgToMonitor(MSGID_GAMESERVERALIVE, DEF_MSGTYPE_CONFIRM, m_pClientList[iClientH]->m_cName,m_pClientList[iClientH]->m_iTotalClients);
		break;

	}
}

void CGateCore::RegisterGameServerHandler(int iClientH, char * pData)
{
 char cTxt[100], cName[11], cAddress[16], * cp;
 int  i, iPort, iTotalMaps;
 UINT32 * dwp, dwBuildDate;
 UINT16  * wp;
 bool  bIsSuccess = true;
 UINT32 dwProcess;

	// ���� ������ ����Ѵ�.

	ZeroMemory(cName, sizeof(cName));
	ZeroMemory(cAddress, sizeof(cAddress));

	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
	memcpy(cName, cp, 10);
	cp += 10;

	memcpy(cAddress, cp, 16);
	cp += 16;

	wp = (UINT16 *)cp;
	iPort = (int)*wp;
	cp += 2;

	iTotalMaps = (int)*cp;
	cp++;

	for (i = 0; i < iTotalMaps; i++) {
		cp += 11;
	}

	dwp =(UINT32 *)cp;
	dwProcess = (UINT32)*dwp;
	// v2.17 ���� ����
	cp += 4;
	
	dwp =(UINT32 *)cp;
	dwBuildDate = *dwp;
	cp += 4;


	m_pClientList[iClientH]->m_dwProcessHandle = dwProcess;
	m_pClientList[iClientH]->m_bIsGameServer = true;

	if (!m_bBlockGameServer) 
	{
		if (m_iBuildDate != (int)dwBuildDate) {
			// ���� ��¥�� ���� �ʴ� ���� �߰� 
			ResponseRegisterGameServer(iClientH, false);
			wsprintf(cTxt, "(!) Game Server Registration - fail : Server(%s:%s:%d) build date mismatch.", cName, cAddress, (int)dwBuildDate);
			PutLogList(cTxt);
			m_pClientList[iClientH]->m_sIsActivated = -1;
			// ���� �̸��� ����Ѵ�.
			memcpy(m_pClientList[iClientH]->m_cName, cName, 10);
			return;
		}
		// �������
		// ���� ���� �̸��� ���� �ִ� ������ �ִ��� �˻��Ѵ�. 
		for (i = 1; i < DEF_MAXCLIENTS; i++)
			if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cName, cName, 10) == 0)) {
				// ���� �̸��� ���� ���� ������ �̹� ���ӵǾ� �ִ�.
				ResponseRegisterGameServer(iClientH, false);
				wsprintf(cTxt, "(!) Game Server Registration - fail : Server(%s) already exists.", cName);
				PutLogList(cTxt);
				m_pClientList[iClientH]->m_sIsActivated = -2;
				// ���� �̸��� ����Ѵ�.
				memcpy(m_pClientList[iClientH]->m_cName, cName, 10);

				return;
			}


		m_pClientList[iClientH]->m_sIsActivated = 1;
		// ���������� ��������� ���ƴ�. 
		wsprintf(cTxt, "(!) Game Server Registration - Success( Name:%s | Hd:%lx | date %d)", cName, dwProcess, (int)dwBuildDate);
		PutLogList(cTxt);
		// ���� �̸��� ����Ѵ�.
		memcpy(m_pClientList[iClientH]->m_cName, cName, 10);


		ResponseRegisterGameServer(iClientH, true);
	}
	else
	{
		ResponseRegisterGameServer(iClientH, false);
		PutLogList("(!) Game Server Registration - fail : No more GameServers are allowed.");
		m_pClientList[iClientH]->m_sIsActivated = -3;
	}
}

void CGateCore::ResponseRegisterGameServer(int iClientH, bool bRet)
{
 char cData[20], cTempName[22];
 UINT32 * dwp;
 UINT16  * wp;
 int iRet;

	ZeroMemory(cData, sizeof(cData));
	ZeroMemory(cTempName, sizeof(cTempName));

	dwp  = (UINT32 *)(cData + DEF_INDEX4_MSGID);
	*dwp = MSGID_RESPONSE_REGISTERGAMESERVER;
	wp   = (UINT16 *)(cData + DEF_INDEX2_MSGTYPE);
	if (bRet == true) 
		 *wp  = DEF_MSGTYPE_CONFIRM;
	else *wp  = DEF_MSGTYPE_REJECT;

	iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 6);

	switch (iRet) {
	case DEF_XSOCKEVENT_MSGSIZETOOLARGE:
	case DEF_XSOCKEVENT_QUENEFULL:
	case DEF_XSOCKEVENT_SOCKETERROR:
	case DEF_XSOCKEVENT_CRITICALERROR:
	case DEF_XSOCKEVENT_SOCKETCLOSED:
		// �޽����� ������ ������ �߻��ߴٸ� �����Ѵ�.
		memcpy(cTempName,m_pClientList[iClientH]->m_cName,20) ;

		delete m_pClientList[iClientH];
		m_pClientList[iClientH] = 0;
		
		wsprintf(G_cTxt,"*** CRITICAL ERROR! Game Server(%s) connection lost!!! ***", cTempName);
		PutLogList(G_cTxt);
		
		// ���� �ٿ��� �˸���.
		SendMsgToMonitor(MSGID_GAMESERVERDOWN, DEF_MSGTYPE_CONFIRM, cTempName);
		return;
	}
}

void CGateCore::OnKeyUp(WPARAM wParam, LPARAM lParam)
{
 int i, iCnt;
 char cTxt[120];

	switch (wParam) {
	case VK_F1:
		m_bF1pressed = false;
		break;

	// v2.17 2002-6-3 �������� 
	case VK_F2:
		// ��ϵǾ����� Activated���� ���� ���� ������ ����Ʈ�� ����Ѵ�.
		PutLogList(" ");
		iCnt = 0;
		for (i = 1; i < DEF_MAXCLIENTS; i++) 
		if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_bIsGameServer == true) && (m_pClientList[i]->m_sIsActivated <= 0)) {
			
			if ( m_pClientList[i]->m_sIsActivated == 0 ) 
				wsprintf(cTxt, "(X) Server(%s) Registered but not activated.", m_pClientList[i]->m_cName);
			else if( m_pClientList[i]->m_sIsActivated == -1 )
				wsprintf(cTxt, "(X) Server(%s) build date mismatch !", m_pClientList[i]->m_cName);
			else if( m_pClientList[i]->m_sIsActivated == -2 )
				wsprintf(cTxt, "(X) Server(%s) have Same Game Name !", m_pClientList[i]->m_cName);

			PutLogList(cTxt);
			iCnt++;
		}
		wsprintf(cTxt, "%d servers are not activated.", iCnt);
		PutLogList(cTxt);
		break;

	case VK_F3:
		// �۵����� ���� ����Ʈ�� ���δ�.
		PutLogList(" ");
		iCnt = 0;
		for (i = 1; i < DEF_MAXCLIENTS; i++) 
		if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_bIsGameServer == true) && (m_pClientList[i]->m_sIsActivated > 0 )) {
			wsprintf(cTxt, "(!) Server(%s) activated.", m_pClientList[i]->m_cName);
			PutLogList(cTxt);
			iCnt++;
		}
		wsprintf(cTxt, "%d servers are activated.", iCnt);
		PutLogList(cTxt);
		break;
	// �������

	case VK_F4:
		// Auto-Shut down process
		if (m_bF1pressed == true) {

			m_iAutoShutdownCount = 10;
			m_dwAutoShutdownTime = timeGetTime();

			for (i = 1; i < DEF_MAXCLIENTS; i++) 
				if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_bIsGameServer == true))
					m_pClientList[i]->m_bIsShutDown = true ;

			PutLogList(" ");
			PutLogList("(!) AUTO SHUTDOWN PROCESS STARTED!");
		}
		break;
	
	case VK_F5:
		if (!m_bBlockGameServer)
		{
			m_bBlockGameServer = true;
			PutLogList("(!) No more GameServers are allowed.");
		}
		else 
		{
			m_bBlockGameServer = false;
			PutLogList("(!) All GameServers are allowed.");
		}
		break;
	
	case VK_F6:
		if (m_bF1pressed == true) {

			PutLogList("(!) Send Server shutdown Message-1");
			SendServerShutDownMsg(1,true);
			
		}
		break;

	case VK_F7:
		if (m_bF1pressed == true) {

			PutLogList("(!) Send Server shutdown Message-2");
			SendServerShutDownMsg(2,true);
			
		}
		break;

	case VK_F12:
		m_bF12pressed = false;
		break;
	}
}

void CGateCore::OnTimer(int iID)
{
	switch (iID) {
	case 0:
		// ���� ������ �׾������� üũ�Ѵ�. 
		CheckGameServerActivity();
		// ��ü �̿��� ���� �� ���Ӽ����� �˷��ش�.
		SendTotalClientsToGameServer();
		// Global command�� ������ �Ѵٸ� ������.
		CheckGlobalCommand();
		// �ڵ� �˴ٿ� ������ �Ǵ�
		CheckAutoShutdownProcess();
		// ��Ƽ ��� ���� Ȯ�� üũ 
		m_pPartyManager->CheckMemberActivity();
		break;
	}
}

void CGateCore::CheckGameServerActivity()
{
 int i;
 UINT32    dwTime;
 char     cTxt[120];

	dwTime = timeGetTime();

	for (i = 1; i < DEF_MAXCLIENTS; i++) 
	if (m_pClientList[i] != 0) {
		if ((m_pClientList[i]->m_bIsGameServer == true) && ((dwTime - m_pClientList[i]->m_dwAliveTime) > DEF_NORESPONSELIMIT)) {
 			// �� ���� ������ ���� ������ �ߴܵǾ���. ������ ���������� �ʾ����� �������� ���� �ߴ� �����̴�. 
			wsprintf(cTxt, "*** CRITICAL ERROR! Game server(%s) does not response!", m_pClientList[i]->m_cName);
			PutLogList(cTxt);
			if (m_pClientList[i]->m_sIsActivated > 0) 
				m_pClientList[i]->m_sIsActivated = 0;
			// Monitor Ŭ���̾�Ʈ���� �˸���.
			SendMsgToMonitor(MSGID_GAMESERVERDOWN, DEF_MSGTYPE_CONFIRM, m_pClientList[i]->m_cName);
		}
	}
}

void CGateCore::SendMsgToMonitor(UINT32 dwMsg, UINT16 wMsgType, char *  pGameServerName,short sTotalClient)
{
 int     i, iRet;
 char  * cp, cTxt[120], cData[200];
 UINT32 * dwp;
 UINT16  * wp;
 short * sp ;
	ZeroMemory(cData,sizeof(cData)) ;

	dwp  = (UINT32 *)(cData + DEF_INDEX4_MSGID);
	*dwp = dwMsg; //MSGID_GAMESERVERDOWN;
	wp   = (UINT16 *)(cData + DEF_INDEX2_MSGTYPE);
	*wp  = wMsgType; //DEF_MSGTYPE_CONFIRM;

	cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);
	memcpy(cp, pGameServerName,20) ;
	cp += 20 ;

	sp = (short * ) cp ;
	*sp = sTotalClient ;
	cp += 2 ;

	for (i = 1; i < DEF_MAXCLIENTS; i++) 
	if (m_pClientList[i] != 0) {
		if (m_pClientList[i]->m_bIsGameServer == false) {
			// ����͵鿡�� �޽����� �����Ѵ�.
			iRet = m_pClientList[i]->m_pXSock->iSendMsg(cData, 28);

			switch (iRet) {
			case DEF_XSOCKEVENT_MSGSIZETOOLARGE:
			case DEF_XSOCKEVENT_QUENEFULL:
			case DEF_XSOCKEVENT_SOCKETERROR:
			case DEF_XSOCKEVENT_CRITICALERROR:
			case DEF_XSOCKEVENT_SOCKETCLOSED:
				// �޽����� ������ ������ �߻��ߴٸ� �����Ѵ�.
				delete m_pClientList[i];
				m_pClientList[i] = 0;
		
				wsprintf(cTxt,"*** CRITICAL ERROR! Monitor(%d) connection lost!!! ***", i);
				PutLogList(cTxt);
				return;
			}
		}
	}

}

void CGateCore::SendMsgToMonitor(char * pData, UINT32 dwMsgSize)
{
 int  i, iRet;
 char cTxt[120];
	
	for (i = 1; i < DEF_MAXCLIENTS; i++) 
	if (m_pClientList[i] != 0) {
		if (m_pClientList[i]->m_bIsGameServer == false) {
			// ����͵鿡�� �޽����� �����Ѵ�.
			iRet = m_pClientList[i]->m_pXSock->iSendMsg(pData, dwMsgSize);

			switch (iRet) {
			case DEF_XSOCKEVENT_MSGSIZETOOLARGE:
			case DEF_XSOCKEVENT_QUENEFULL:
			case DEF_XSOCKEVENT_SOCKETERROR:
			case DEF_XSOCKEVENT_CRITICALERROR:
			case DEF_XSOCKEVENT_SOCKETCLOSED:
				// �޽����� ������ ������ �߻��ߴٸ� �����Ѵ�.
				delete m_pClientList[i];
				m_pClientList[i] = 0;
		
				wsprintf(cTxt,"*** CRITICAL ERROR! Monitor(%d) connection lost!!! ***", i);
				PutLogList(cTxt);
				return;
			}
		}
	}

}


void CGateCore::SendTotalClientsToGameServer()
{
 int i ;

	// �� ����� ���� ����Ѵ�.
	iTotalWorldServerClients = 0;
	for (i = 0; i < DEF_MAXCLIENTS; i++) 
	if (m_pClientList[i] != 0) {
		if (m_pClientList[i]->m_bIsGameServer == true) 
			iTotalWorldServerClients += m_pClientList[i]->m_iTotalClients;
	}

	// �� ���Ӽ������� �˷��ش�.
	for (i = 0; i < DEF_MAXCLIENTS; i++) 
	if (m_pClientList[i] != 0) {
		if (m_pClientList[i]->m_bIsGameServer == true) 
			_SendTotalGameServerClients(i, iTotalWorldServerClients);
	}
}

void CGateCore::_SendTotalGameServerClients(int iClientH, int iTotalClients)
{
 char cData[120],cTempName[22];
 UINT32 * dwp;
 UINT16  * wp;
 char  * cp;
 int     iRet;

	ZeroMemory(cData, sizeof(cData));
	ZeroMemory(cTempName, sizeof(cTempName));

	dwp  = (UINT32 *)(cData + DEF_INDEX4_MSGID);
	*dwp = MSGID_TOTALGAMESERVERCLIENTS;
	wp   = (UINT16 *)(cData + DEF_INDEX2_MSGTYPE);
	*wp  = DEF_MSGTYPE_CONFIRM;

	cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);
	wp = (UINT16 *)cp;
	*wp = iTotalClients;
	cp += 2;
	
	iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 8);

	switch (iRet) {
	case DEF_XSOCKEVENT_MSGSIZETOOLARGE:
	case DEF_XSOCKEVENT_QUENEFULL:
	case DEF_XSOCKEVENT_SOCKETERROR:
	case DEF_XSOCKEVENT_CRITICALERROR:
	case DEF_XSOCKEVENT_SOCKETCLOSED:
		// �޽����� ������ ������ �߻��ߴٸ� �����Ѵ�.
		memcpy(cTempName,m_pClientList[iClientH]->m_cName,20) ;

		delete m_pClientList[iClientH];
		m_pClientList[iClientH] = 0;
		
		wsprintf(G_cTxt,"*** CRITICAL ERROR! Game Server(%s) connection lost!!! ***", cTempName);
		PutLogList(G_cTxt);
		
		// ���� �ٿ��� �˸���.
		SendMsgToMonitor(MSGID_GAMESERVERDOWN, DEF_MSGTYPE_CONFIRM, cTempName);
		return;
	}
}

void CGateCore::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	switch (wParam) {
	case VK_F1:
		m_bF1pressed = true;
		break;
	case VK_F12:
		m_bF12pressed = true;
		break;
	}
}

void CGateCore::CheckGlobalCommand()
{
 int i, iRet;
 char cData[256],cTempName[22];
 UINT32 * dwp;
 UINT16  * wp;
 

	ZeroMemory(cTempName,sizeof(cTempName)) ;

	if ((m_bF1pressed == true) && (m_bF12pressed == true)) {
		PutLogList("(!) SEND GLOBAL COMMAND: SHUTDOWN ALL GAME SERVER");

		for (i = 0; i < DEF_MAXCLIENTS; i++) 
		if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_bIsGameServer == true)) {
			
			ZeroMemory(cData, sizeof(cData));
			dwp  = (UINT32 *)(cData + DEF_INDEX4_MSGID);
			*dwp = MSGID_GAMESERVERSHUTDOWNED;
			wp   = (UINT16 *)(cData + DEF_INDEX2_MSGTYPE);
			*wp  = DEF_MSGTYPE_CONFIRM;
	
			iRet = m_pClientList[i]->m_pXSock->iSendMsg(cData, 6);

			switch (iRet) {
			case DEF_XSOCKEVENT_MSGSIZETOOLARGE:
			case DEF_XSOCKEVENT_QUENEFULL:
			case DEF_XSOCKEVENT_SOCKETERROR:
			case DEF_XSOCKEVENT_CRITICALERROR:
			case DEF_XSOCKEVENT_SOCKETCLOSED:
				// �޽����� ������ ������ �߻��ߴٸ� �����Ѵ�.
				memcpy(cTempName,m_pClientList[i]->m_cName,20) ;

				delete m_pClientList[i];
				m_pClientList[i] = 0;
		
				wsprintf(G_cTxt,"*** CRITICAL ERROR! Game Server(%s) connection lost!!! ***", cTempName);
				PutLogList(G_cTxt);
		
				// ���� �ٿ��� �˸���.
				SendMsgToMonitor(MSGID_GAMESERVERDOWN, DEF_MSGTYPE_CONFIRM, cTempName);
				return;
			}
		}
	}
}

void CGateCore::SendServerShutDownMsg(char cCode, bool bISShutdown )
{
 int i;
 char cTxt[256] ; 

	// �� ���Ӽ������� �˷��ش�.
	for (i = 0; i < DEF_MAXCLIENTS; i++) 
	if (m_pClientList[i] != 0) {
		if ((m_pClientList[i]->m_bIsGameServer == true)&& ((m_pClientList[i]->m_bIsShutDown == true ) || (bISShutdown == true))) {
			_SendServerShutDownMsg(i, cCode);
			wsprintf(cTxt,"(!) SEND SHUTDOWN MESSAGE TO (%s) SERVER !",m_pClientList[i]->m_cName) ;
			PutLogList(cTxt) ;
		}
	}
}

void CGateCore::_SendServerShutDownMsg(int iClientH, char cCode)
{
 char cData[120],cTempName[22];
 UINT32 * dwp;
 UINT16  * wp;
 char  * cp;
 int     iRet;

	ZeroMemory(cData, sizeof(cData));
	ZeroMemory(cTempName, sizeof(cTempName));


	dwp  = (UINT32 *)(cData + DEF_INDEX4_MSGID);
	*dwp = MSGID_SENDSERVERSHUTDOWNMSG;
	wp   = (UINT16 *)(cData + DEF_INDEX2_MSGTYPE);
	*wp  = DEF_MSGTYPE_CONFIRM;

	cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);
	wp = (UINT16 *)cp;
	*wp = cCode;
	cp += 2;
	
	iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 8);

	switch (iRet) {
	case DEF_XSOCKEVENT_MSGSIZETOOLARGE:
	case DEF_XSOCKEVENT_QUENEFULL:
	case DEF_XSOCKEVENT_SOCKETERROR:
	case DEF_XSOCKEVENT_CRITICALERROR:
	case DEF_XSOCKEVENT_SOCKETCLOSED:
		// �޽����� ������ ������ �߻��ߴٸ� �����Ѵ�.
		memcpy(cTempName,m_pClientList[iClientH]->m_cName,20) ;

		delete m_pClientList[iClientH];
		m_pClientList[iClientH] = 0;
		
		wsprintf(G_cTxt,"*** CRITICAL ERROR! Game Server(%s) connection lost!!! ***", cTempName);
		PutLogList(G_cTxt);
		
		// ���� �ٿ��� �˸���.
		SendMsgToMonitor(MSGID_GAMESERVERDOWN, DEF_MSGTYPE_CONFIRM, cTempName);
		return;
	}
}

void CGateCore::ItemLog(char *pData)
{
 char * cp, cGiveName[11], cRecvName[11], cItemName[21], cStr[256];
 short * sp, sItemType, sItemV1, sItemV2, sItemV3;

	ZeroMemory(cGiveName, sizeof(cGiveName));
	ZeroMemory(cRecvName, sizeof(cRecvName));
	ZeroMemory(cItemName, sizeof(cItemName));
	ZeroMemory(cStr, sizeof(cStr));

	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
	memcpy(cGiveName, cp, 10);
	cp += 10;

	memcpy(cRecvName, cp, 10);
	cp += 10;

	memcpy(cItemName, cp, 20);
	cp += 20;

	sItemType = (short)*cp;
	cp++;

	sp = (short *)cp;
	sItemV1 = *sp;
	cp += 2;

	sp = (short *)cp;
	sItemV2 = *sp;
	cp += 2;

	sp = (short *)cp;
	sItemV3 = *sp;
	cp += 2;

	wsprintf(cStr, "Item(%s %d %d %d %d) From(%s)-To(%s)", cItemName, sItemType, sItemV1, sItemV2, sItemV3, cGiveName, cRecvName);
	PutItemLogFileList(cStr);

	PutLogList(cStr);
}



void CGateCore::SendMsgToAllGameServers(int iClientH, char *pData, UINT32 dwMsgSize, bool bIsOwnSend)
{
 int i;

	if (bIsOwnSend == true) {
		for (i = 0; i < DEF_MAXCLIENTS; i++)
		if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_bIsGameServer == true)) {
			m_pClientList[i]->m_pXSock->iSendMsg(pData, dwMsgSize);
		}
	}
	else {
		for (i = 0; i < DEF_MAXCLIENTS; i++)
		if ((i != iClientH) && (m_pClientList[i] != 0) && (m_pClientList[i]->m_bIsGameServer == true)) {
			m_pClientList[i]->m_pXSock->iSendMsg(pData, dwMsgSize);
		}
	}
}

void CGateCore::CheckAutoShutdownProcess()
{
 int i, iRet;
 UINT32 dwTime = timeGetTime();
 char  cData[128], cTempName[22];
 UINT16 * wp;
 UINT32 * dwp;
	ZeroMemory(cTempName,sizeof(cTempName)); 

	m_bIsAutoShutdownProcess = false ;

	for (i = 1; i < DEF_MAXCLIENTS; i++) 
		if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_bIsGameServer == true) && (m_pClientList[i]->m_bIsShutDown == true))
			m_bIsAutoShutdownProcess = true ;
			
	if (m_bIsAutoShutdownProcess == false) return;

	if ((dwTime - m_dwAutoShutdownTime) > 20000) {
		m_dwAutoShutdownTime = dwTime;
	}
	else return;
	
 	m_iAutoShutdownCount--;
	if (m_iAutoShutdownCount <= 0) m_iAutoShutdownCount = 0;
	switch (m_iAutoShutdownCount) {
	default:
		PutLogList(" ") ;
		SendServerShutDownMsg(1);
		break;

	case 2:

		PutLogList("LAST SHUTDOWN MESSAGE") ;
		SendServerShutDownMsg(2);
		break;

	case 1:
		
		for (i = 0; i < DEF_MAXCLIENTS; i++) 
		if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_bIsGameServer == true) && (m_pClientList[i]->m_bIsShutDown == true)) {
			wsprintf(G_cTxt,"(!) SEND GLOBAL COMMAND: SHUTDOWN (%s) GAME SERVER",m_pClientList[i]->m_cName) ;
			PutLogList(G_cTxt) ;

			ZeroMemory(cData, sizeof(cData));
			dwp  = (UINT32 *)(cData + DEF_INDEX4_MSGID);
			*dwp = MSGID_GAMESERVERSHUTDOWNED;
			wp   = (UINT16 *)(cData + DEF_INDEX2_MSGTYPE);
			*wp  = DEF_MSGTYPE_CONFIRM;
	
			iRet = m_pClientList[i]->m_pXSock->iSendMsg(cData, 6);

			switch (iRet) {
			case DEF_XSOCKEVENT_MSGSIZETOOLARGE:
			case DEF_XSOCKEVENT_QUENEFULL:
			case DEF_XSOCKEVENT_SOCKETERROR:
			case DEF_XSOCKEVENT_CRITICALERROR:
			case DEF_XSOCKEVENT_SOCKETCLOSED:
				// �޽����� ������ ������ �߻��ߴٸ� �����Ѵ�.
				memcpy(cTempName,m_pClientList[i]->m_cName,20) ;

				delete m_pClientList[i];
				m_pClientList[i] = 0;

				wsprintf(G_cTxt,"*** CRITICAL ERROR! Game Server(%s) connection lost!!! ***", cTempName);
				PutLogList(G_cTxt);
				// ���� �ٿ��� �˸���.
				SendMsgToMonitor(MSGID_GAMESERVERDOWN, DEF_MSGTYPE_CONFIRM, cTempName);
				return;
			}
		}

		break;
	}
}



void CGateCore::PartyOperation(int iClientH, char *pData)
{
 char * cp, cName[12], cData[120];
 UINT32 * dwp;
 UINT16 * wp, wRequestType;
 int iRet, iGSCH, iPartyID;
 bool bRet;

	cp = (char *)pData;
	cp += 4;
	wp = (UINT16 *)cp;
	wRequestType = *wp;
	cp += 2;

	wp = (UINT16 *)cp;
	iGSCH = (UINT16)*wp;
	cp += 2;

	ZeroMemory(cName, sizeof(cName));
	memcpy(cName, cp, 10);
	cp += 10;

	wp = (UINT16 *)cp;
	iPartyID = (UINT16)*wp;
	cp += 2;

	//testcode
	//wsprintf(G_cTxt, "Party Operation Type: %d Name: %s PartyID:%d", wRequestType, cName, iPartyID);
	//PutLogList(G_cTxt);

	ZeroMemory(cData, sizeof(cData));
	cp = (char *)cData;
	dwp = (UINT32 *)cp;
	*dwp = MSGID_PARTYOPERATION;
	cp += 4;
	wp = (UINT16 *)cp;
	
	switch (wRequestType) {
	case 1: // ��Ƽ ���� ��û 
		iPartyID = m_pPartyManager->iCreateNewParty(iClientH, cName);
		if (iPartyID == 0) {
			// ��Ƽ ���� ����! 
			*wp = 1; // ��Ƽ ���� ���� ��û�� ���� �����̴�.
			cp += 2;
			*cp = 0; // ���� ���� 
			cp++;
			wp = (UINT16 *)cp;
			*wp = iGSCH;
			cp += 2;
			memcpy(cp, cName, 10);
			cp += 10;
			wp = (UINT16 *)cp;
			*wp = (UINT16)iPartyID;
			cp += 2;
			iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 22);
		}
		else {
			// ��Ƽ�� �������.
			*wp = 1; // ��Ƽ ���� ���� ��û�� ���� �����̴�.
			cp += 2;
			*cp = 1; // ���� ���� 
			cp++;
			wp = (UINT16 *)cp;
			*wp = iGSCH;
			cp += 2;
			memcpy(cp, cName, 10);
			cp += 10;
			wp = (UINT16 *)cp;
			*wp = (UINT16)iPartyID;
			cp += 2;
			iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 22);
		}
		break;

	case 2: // ��Ƽ �ػ� ��û 
		break;

	case 3: // ��� �߰� ��û
		bRet = m_pPartyManager->bAddMember(iClientH, iPartyID, cName);
		if (bRet == true) {
			// ��� �߰� ����
			*wp = 4; // ��� �߰��� ���� �����̴�.
			cp += 2;
			*cp = 1; // �߰� ���� 
			cp++;
			wp = (UINT16 *)cp;
			*wp = iGSCH;
			cp += 2;
			memcpy(cp, cName, 10);
			cp += 10;
			wp = (UINT16 *)cp;
			*wp = (UINT16)iPartyID;
			cp += 2;
			// ��Ƽ ����� �߰��Ǿ��ٴ� ������ ��� ������ �����Ѵ�.
			SendMsgToAllGameServers(0, cData, 22, true);
		}
		else {
			// ��� �߰� ����
			*wp = 4; // ��� �߰��� ���� �����̴�.
			cp += 2;
			*cp = 0; // �߰� ���� 
			cp++;
			wp = (UINT16 *)cp;
			*wp = iGSCH;
			cp += 2;
			memcpy(cp, cName, 10);
			cp += 10;
			wp = (UINT16 *)cp;
			*wp = (UINT16)iPartyID;
			cp += 2;
			iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 22);
		}
		break;

	case 4: // ��� ���� ��û
		bRet = m_pPartyManager->bRemoveMember(iPartyID, cName);
		if (bRet == true) {
			// ��� ���� ����
			*wp = 6; // ��� ���ſ� ���� �����̴�.
			cp += 2;
			*cp = 1; // ���� ���� 
			cp++;
			wp = (UINT16 *)cp;
			*wp = iGSCH;
			cp += 2;
			memcpy(cp, cName, 10);
			cp += 10;
			wp = (UINT16 *)cp;
			*wp = (UINT16)iPartyID;
			cp += 2;
			// ��Ƽ ����� ���ŵǾ��ٴ� ������ ��� ������ �����Ѵ�.
			SendMsgToAllGameServers(0, cData, 22, true);
		}
		else {
			// ��� ���� ����
			*wp = 6; // ��� ���ſ� ���� �����̴�.
			cp += 2;
			*cp = 0; // ���� ���� 
			cp++;
			wp = (UINT16 *)cp;
			*wp = iGSCH;
			cp += 2;
			memcpy(cp, cName, 10);
			cp += 10;
			wp = (UINT16 *)cp;
			*wp = (UINT16)iPartyID;
			cp += 2;
			iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 22);
		}
		break;

	case 5: // ��Ƽ ��� Ȯ�� ��û. ���� �̵� ���¸� Ŭ�����Ѵ�.
		m_pPartyManager->bCheckPartyMember(iClientH, iGSCH, iPartyID, cName);
		break;

	case 6: // ��Ƽ ���� ���� ��û. ��Ƽ�� �� ������ ���� �̸��� �˷��ش�.
		m_pPartyManager->bGetPartyInfo(iClientH, iGSCH, cName, iPartyID);
		break;

	case 7: // ����� ���� �̵� ���� ��ȯ: �̰� ���õ� ���� ���� �ð� ���� Ȯ���� �ȵǸ� ��Ƽ���� �����Ѵ�.
		m_pPartyManager->SetServerChangeStatus(cName, iPartyID);	
		break;
	}
}

