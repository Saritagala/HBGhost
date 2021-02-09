//////////////////////////////////////////////////////////////////////
//
// WorldLog.cpp: implementation of the WorldLog class.
//
//////////////////////////////////////////////////////////////////////

#include "worldlog.h"
#include <stdlib.h>
#include <stdio.h>
#include <direct.h>

extern void		PutLogList(char * cMsg);
extern void		PutLogServ(char * cMsg);
extern void		PutEventLog(char * cMsg);
extern void		PutGameLogData(char *cMsg);
extern class	XSocket * G_pListenSock;
extern class	XSocket * G_pXWorldLogSock;
extern char		G_cTxt[500];
extern char		G_cData50000[50000];
extern BOOL		G_bIsMainLogActive;
int			G_cClientCheckCount;
bool			G_bDBMode = true; // false

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWorldLog::CWorldLog(HWND hWnd)
{
DWORD dwTime;
dwTime = timeGetTime();
 int i;
	
	m_hWnd = hWnd;
	for (i = 0; i < DEF_MAXCLIENTSOCK; i++) m_pClientList[i] = NULL;
	for (i = 0; i < DEF_MSGQUEUESIZE; i++) m_pMsgQueue[i] = NULL;
	for (i = 0; i < DEF_MAXGAMESERVERS; i++) m_pGameList[i] = NULL;
	for (i = 0; i < DEF_MAXGAMESERVERSMAPS; i++) m_cMapList[i] = NULL;
	for (i = 0; i < DEF_MAXMAINLOGSOCK; i++) m_pMainLogSock[i] = NULL;
	for (i = 0; i < DEF_MAXMAINLOGSOCK; i++) m_bisMainRegistered[i] = FALSE;
	for (i = 0; i < DEF_MAXPLAYERACCOUNTS; i++) m_pAccountList[i] = NULL;
	for (i = 0; i < DEF_MAXITEMS; i++) m_pItemList[i] = NULL;
	ZeroMemory(m_cWorldLogName, sizeof(m_cWorldLogName));
	ZeroMemory(m_pMsgBuffer, sizeof(m_pMsgBuffer+1));
	ZeroMemory(m_cWorldServerAddress, sizeof(m_cWorldServerAddress));
	ZeroMemory(m_cMainLogAddress, sizeof(m_cMainLogAddress));
	ZeroMemory(cGameSecurity, sizeof(cGameSecurity));
	m_iWorldServerPort = 0;
	m_iWorldServerInternalPort = 0;
	m_iTotalPlayers = 0;
	m_iMainLogPort = 0;
	m_iQueueHead = 0;
	m_iQueueTail = 0;
	m_iBlockCount = 0;
	m_cBackupDrive = 0;
	m_iActiveMainLogSock = 0;
	m_iCurMainLogSockIndex = 0;
	m_bIsWorldRegistered = FALSE;
	m_iTotalMainLogSock = 0;
	m_cGMLogCount = 0;
	ZeroMemory(m_cGMLogBuffer, sizeof(m_cGMLogBuffer));
	m_cItemLogCount = 0;
	ZeroMemory(m_cItemLogBuffer, sizeof(m_cItemLogBuffer));
	m_cCrusadeLogCount = 0;
	ZeroMemory(m_cCrusadeLogBuffer, sizeof(m_cCrusadeLogBuffer));
	ZeroMemory(m_sGameServerList, sizeof(m_sGameServerList));

	// customization
	m_bGameServerList = FALSE;

	G_cClientCheckCount = 0;
	m_dwGameTime1 = dwTime;
}

CWorldLog::~CWorldLog(void)
{
 int i;
	
	for (i = 0; i < DEF_MAXCLIENTSOCK; i++) {
		if (m_pClientList[i] != NULL) m_pClientList[i] = NULL;
	}
 	for (i = 0; i < DEF_MAXPLAYERACCOUNTS; i++) {
		if (m_pAccountList[i] != NULL) {
			SendEventToMLS(MSGID_REQUEST_CLEARACCOUNTSTATUS, DEF_MSGTYPE_CONFIRM, m_pAccountList[i]->cAccountName, 10, -1);
			delete m_pAccountList[i];
		}
	}

	for (i = 0; i < DEF_MSGQUEUESIZE; i++) {
		if (m_pMsgQueue[i] != NULL) delete m_pMsgQueue[i];
	}

	for (i = 0; i < DEF_MAXGAMESERVERS; i++) {
		if (m_pGameList[i] != NULL) delete m_pGameList[i];
	}

	for (i = 0; i < DEF_MAXGAMESERVERSMAPS; i++) {
		if (m_cMapList[i] != NULL) delete m_cMapList[i];
	}

	for (i = 0; i < DEF_MAXMAINLOGSOCK; i++) {
		if (m_pMainLogSock[i] != NULL) delete m_pMainLogSock[i];
	}
	for (i = 0; i < DEF_MAXITEMS; i++) {
		if (m_pItemList[i] != NULL) delete m_pItemList[i];
	}
}

bool CWorldLog::bInit()
{
 int i;

	if (bReadServerConfigFile("WLserver.cfg") == FALSE) return FALSE;
	//if (bReadServerConfigFile("GateServer.cfg") == 0) return FALSE;

	for (i = 0; i < DEF_MAXMAINLOGSOCK; i++) {
		Sleep(200);
		m_pMainLogSock[i] = new class XSocket(m_hWnd, DEF_SERVERSOCKETBLOCKLIMIT);
		m_pMainLogSock[i]->bConnect(m_cMainLogAddress, m_iMainLogPort, WM_ONMAINLOGSOCKETEVENT+i);
		m_pMainLogSock[i]->bInitBufferSize(DEF_MSGBUFFERSIZE);
		wsprintf(G_cTxt, "(!) Try to connect Main-log-socket(%d)... Addr:%s  Port:%d", i, m_cMainLogAddress, m_iMainLogPort);
		PutLogList(G_cTxt);
	}

	//if (bInitSQLServer() == NULL) return FALSE;
	m_iTotalMainLogSock = 0;
	if (bReadItemConfigFile("Item.cfg") == FALSE) return FALSE;
	if (bReadItemConfigFile("Item2.cfg") == FALSE) return FALSE;
	if (bReadItemConfigFile("Item3.cfg") == FALSE) return FALSE;

	// VAMP - sql connection
	try
	{
		con.Connect(
			"LOCALHOST\\SQLEXPRESS@HBGhost",
			"",
			"",
			SA_SQLServer_Client);
	}
	catch (SAException& x)
	{
		char cTemp[256];
		ZeroMemory(cTemp, sizeof(cTemp));
		wsprintf(cTemp, "(!!!) SQL SERVER ERROR: %s", (const char*)x.ErrText());
		PutLogList(cTemp);
		return FALSE;
	}

	PutLogList("(!) SQL SERVER CONNECTED: @HBGhost");

	return TRUE;
}

bool CWorldLog::bInitSQLServer(void)
{
	// hypnotoad: uncoded sql initialization
	/*
	CoInitialize(NULL);
	"<Data Source=character;User ID=sa;pwd=SQL_axszdc;Provider=MSDASQL.1;>"
	wsprintf(G_cTxt, "select * from CHARACTER_T where CharID = (select MAX(CharID) from CHARACTER_T)", ??????);
	PutLogList("(!) Success Connect Database!");
	wsprintf(G_cTxt, "Can't Connect Database (%x)", ??????);
	MessageBox(m_hWnd, G_cTxt, "DB connect Error", MB_OK+MB_ICONERROR);
	return 0;
	*/
	return true;
}

void CWorldLog::CleanupLogFiles()
{
	PutGMLogData(NULL, NULL, TRUE);
	PutItemLogData(NULL, NULL, TRUE);
	PutCrusadeLogData(NULL, NULL, TRUE);
}

bool CWorldLog::bReadServerConfigFile(char *cFn)
{
 FILE * pFile;
 HANDLE hFile;
 DWORD  dwFileSize;
 char * cp, * token, cReadMode, cTotalList;
 char seps[] = "= \t\n";
 class CStrTok * pStrTok;

	pFile = NULL;
	cTotalList  = 0;
	cReadMode	= 0;
	hFile = CreateFileA(cFn, GENERIC_READ, NULL, NULL, OPEN_EXISTING, NULL, NULL);
	dwFileSize = GetFileSize(hFile, NULL);
	if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);
	pFile = fopen(cFn, "rt");
	if (pFile == NULL) {
		PutLogList("(!) Cannot open configuration file.");
		return FALSE;
	}
	else {
		PutLogList("(!) Reading configuration file...");
		cp = new char[dwFileSize+1];
		ZeroMemory(cp, dwFileSize+1);
		fread(cp, dwFileSize, 1, pFile);
		pStrTok = new class CStrTok(cp, seps);
		token = pStrTok->pGet();
		while (token != NULL) {
			if (cReadMode != 0) {
				switch (cReadMode) {

				case 1: // gate-server-port
					ZeroMemory(m_cWorldServerAddress, sizeof(m_cWorldServerAddress));
					char ServerAddr[50];
					::gethostname(ServerAddr, 50); 
					struct hostent *pHostEnt;
					pHostEnt = ::gethostbyname(ServerAddr);
					if (pHostEnt != NULL) {
						wsprintf(ServerAddr, "%d.%d.%d.%d",
							(pHostEnt->h_addr_list[0][0] & 0x00ff),
							(pHostEnt->h_addr_list[0][1] & 0x00ff),
							(pHostEnt->h_addr_list[0][2] & 0x00ff),
							(pHostEnt->h_addr_list[0][3] & 0x00ff));
					}
					strcpy(m_cWorldServerAddress, ServerAddr);
					wsprintf(G_cTxt, "(*) World-Server address : %s", m_cWorldServerAddress);
					PutLogList(G_cTxt);

					m_iWorldServerPort = atoi(token);
					wsprintf(G_cTxt, "(*) World-Server port : %d", m_iWorldServerPort);
					PutLogList(G_cTxt);
					cReadMode = 0;
					break;

				case 2: // backup-drive-letter
					m_cBackupDrive = token[0];
					wsprintf(G_cTxt, "(*) Backup Drive Letter : %c", m_cBackupDrive);
					PutLogList(G_cTxt);
					cReadMode = 0;
					break;

				case 3: // internal-world-server-port
					m_iWorldServerInternalPort = atoi(token);
					wsprintf(G_cTxt, "(*) World-Server Internal port : %d", m_iWorldServerInternalPort);
					PutLogList(G_cTxt);
					cReadMode = 0;
					break;

				case 4:	// world-server-name
					strncpy(m_cWorldLogName, token, 10);
					wsprintf(G_cTxt, "(*) World-Server-Name : %s", m_cWorldLogName);
					PutLogList(G_cTxt);
					cReadMode = 0;
					break;
				
				case 6: // main-log-server-address
					strncpy(m_cMainLogAddress, token, 15);
					wsprintf(G_cTxt, "(*) Main-Log-Server Address : %s", m_cMainLogAddress);
					PutLogList(G_cTxt);
					cReadMode = 0;
					break;

				case 7: // main-log-server-port
					m_iMainLogPort = atoi(token);
					wsprintf(G_cTxt, "(*) Main-Log-Server port : %d", m_iMainLogPort);
					PutLogList(G_cTxt);
					cReadMode = 0;
					break;
				
				case 8: // game-server-list
					strcpy(m_sGameServerList[cTotalList << 4], token);
					wsprintf(G_cTxt, "Permitted Game Server Address: %s", m_sGameServerList[cTotalList << 4]);
					PutLogList(G_cTxt);
					cTotalList++;
					m_bGameServerList = TRUE;
					cReadMode = 0;
					break;
				case 9:
					
					/*strncpy(cGameSecurity, token, 11);
					wsprintf(G_cTxt, "Wlserver Security: %s", cGameSecurity);
					PutLogList(G_cTxt);*/
					cReadMode = 0;
					break;
				default:
					break;
				
				}
			}
			else {
				if (memcmp(token, "world-server-port", 17) == 0)			cReadMode = 1;
				if (memcmp(token, "backup-drive-letter", 19) == 0)			cReadMode = 2;
				if (memcmp(token, "internal-world-server-port", 26) == 0)	cReadMode = 3;
				if (memcmp(token, "world-server-name", 17) == 0)			cReadMode = 4;
				// case 5 removed and obtained automatically - "world-server-address"
				if (memcmp(token, "main-log-server-address", 23) == 0)		cReadMode = 6;
				if (memcmp(token, "main-log-server-port", 20) == 0)			cReadMode = 7;
				if (memcmp(token, "game-server-list", 16) == 0)				cReadMode = 8;
				//if (memcmp(token, "Security-WL", 11) == 0)				cReadMode = 9;
			}
			token = pStrTok->pGet();
		}
		delete pStrTok;
		delete cp;
		fclose(pFile);
	}
	PutLogList(" ");
	return TRUE;
}

BOOL CWorldLog::bAccept(class XSocket * pXSock, BOOL bCheck)
{
 int i, x;
 class XSocket * pTmpSock;
 BOOL bFlag;

	bFlag = FALSE;
	for (i = 1; i < DEF_MAXCLIENTSOCK; i++) { //loop 1
		if (m_pClientList[i] == NULL) { //if client null
			m_pClientList[i] = new class CClient(m_hWnd);
			pXSock->bAccept(m_pClientList[i]->m_pXSock, WM_ONCLIENTSOCKETEVENT + i); 
			m_pClientList[i]->m_pXSock->bInitBufferSize(DEF_MSGBUFFERSIZE);
			ZeroMemory(m_pClientList[i]->m_cIPaddress, sizeof(m_pClientList[i]->m_cIPaddress));
			m_pClientList[i]->m_pXSock->iGetPeerAddress(m_pClientList[i]->m_cIPaddress);
			if (bCheck == FALSE) { //if client checked already
				for (x = 0; x < DEF_MAXHGSERVERLIST; x++) { //loop 2
					if (memcmp(m_sGameServerList[x << 4], m_pClientList[i]->m_cIPaddress, 16) == 0) { // if ip = accepted HG
						bFlag = TRUE;
						m_pClientList[i]->m_cMode = 2;
					} //end if HG server matched server accepted
				} //end loop 2
				if (bFlag != TRUE) { //if HG not accepted
					wsprintf(G_cTxt, "<%d> <%s> Unauthorized Access", i, m_pClientList[i]->m_cIPaddress);
					PutEventLog(G_cTxt);
					PutLogList(G_cTxt);
					delete m_pClientList[i];
					m_pClientList[i] = NULL;
					return FALSE;
				} //end if hg not accepted
			} //end client checked
			wsprintf(G_cTxt,"Client Accepted(%d)", i);
			PutLogList(G_cTxt);
			return TRUE;
		} //end client == null
	} //end loop 1
	pTmpSock = new class XSocket(m_hWnd, DEF_SERVERSOCKETBLOCKLIMIT);
	pXSock->bAccept(pTmpSock, NULL); 
	delete pTmpSock;
	return FALSE;
}

void CWorldLog::OnClientSubLogSocketEvent(UINT message, WPARAM wParam, LPARAM lParam) //if client not HG handler
{
 int iClientH, iRet, iTmp;

	iTmp = WM_ONCLIENTSOCKETEVENT;
	iClientH = message - iTmp;
	if (m_pClientList[iClientH] == NULL) return;
	iRet = m_pClientList[iClientH]->m_pXSock->iOnSocketEvent(wParam, lParam);
	switch (iRet) {

	case DEF_XSOCKEVENT_READCOMPLETE:
		OnClientRead(iClientH);
		break;

	case DEF_XSOCKEVENT_BLOCK:
		m_iBlockCount++;
		wsprintf(G_cTxt, "<%d> Client Socket BLOCKED! (total blocks: %s)", iClientH, m_iBlockCount);
		PutLogList(G_cTxt);
		break;

	case DEF_XSOCKEVENT_CONFIRMCODENOTMATCH:
		wsprintfA(G_cTxt, "<%d> Confirmcode notmatch!", iClientH);
		PutLogList(G_cTxt);
		delete m_pClientList[iClientH];
		m_pClientList[iClientH] = NULL;
		break;

	case DEF_XSOCKEVENT_MSGSIZETOOLARGE:
	case DEF_XSOCKEVENT_SOCKETERROR:
	case DEF_XSOCKEVENT_SOCKETCLOSED:
	case DEF_XSOCKEVENT_QUENEFULL:
		wsprintf(G_cTxt, "<%d> Connection Lost! Delete Client.", iClientH); //is not seting account to null on dissconnect.
		PutLogList(G_cTxt);
		DeleteClient(iClientH, 0);
		delete m_pClientList[iClientH];
		m_pClientList[iClientH] = NULL;
		break;
	}
}

void CWorldLog::OnClientRead(int iClientH)
{
 char  * pData, cKey;
 DWORD  dwMsgSize;
	
	if (m_pClientList[iClientH] == NULL) return;
 
	pData = m_pClientList[iClientH]->m_pXSock->pGetRcvDataPointer(&dwMsgSize, &cKey);
	
	if (m_pClientList[iClientH]->m_cMode == 2) {
		if (bPutMsgQuene(DEF_MSGFROM_CLIENT, pData, dwMsgSize, iClientH, cKey) == FALSE) {
			PutLogList("@@@@@@ CRITICAL ERROR in CLIENT MsgQuene!!! @@@@@@");
		}
		return;
	}
	if (bPutMsgQuene(DEF_MSGFROM_BOT, pData, dwMsgSize, iClientH, cKey) == FALSE) {
		PutLogList("@@@@@@ CRITICAL ERROR in BOT MsgQuene!!! @@@@@@");
	}
	return;
}

void CWorldLog::OnMainSubLogSocketEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
 int iMainH, iRet, iTmp;
 char  * pData, cKey;
 DWORD  dwMsgSize;

	iTmp = WM_ONMAINLOGSOCKETEVENT;
	iMainH = message - iTmp;
	if (m_pMainLogSock[iMainH] == NULL) return;
	iRet = m_pMainLogSock[iMainH]->iOnSocketEvent(wParam, lParam);
	switch (iRet) {

	case DEF_XSOCKEVENT_CONNECTIONESTABLISH:
		wsprintf(G_cTxt, "(!) Main-log-socket(%d) connected.", iMainH);
		PutLogList(G_cTxt);
		m_iActiveMainLogSock++;
		if (m_bIsWorldRegistered == FALSE) {
			m_bIsWorldRegistered = TRUE;
			RegisterWorldServer(iMainH);
			RegisterWorldGameServer();
		}
		else {
			RegisterWorldServerSocket(iMainH);
		}
		break;

	case DEF_XSOCKEVENT_READCOMPLETE:
		pData = m_pMainLogSock[iMainH]->pGetRcvDataPointer(&dwMsgSize, &cKey);
		if (bPutMsgQuene(DEF_MSGFROM_LOGSERVER, pData, dwMsgSize, iMainH, cKey) == FALSE) {
			PutLogList("@@@@@@ CRITICAL ERROR in LOGSERVER MsgQuene!!! @@@@@@");
		}
		break;

	case DEF_XSOCKEVENT_BLOCK:
		m_iBlockCount++;
		wsprintf(G_cTxt, "<%d> MainLog Socket BLOCKED! (total blocks: %s)", iMainH, m_iBlockCount);
		PutLogList(G_cTxt);
		break;

	case DEF_XSOCKEVENT_CONFIRMCODENOTMATCH:
		wsprintfA(G_cTxt, "<%d> Confirmcode notmatch!", iMainH);
		PutLogList(G_cTxt);
		delete m_pMainLogSock[iMainH];
		m_pMainLogSock[iMainH] = NULL;
		break;

	case DEF_XSOCKEVENT_MSGSIZETOOLARGE:
	case DEF_XSOCKEVENT_SOCKETERROR:
	case DEF_XSOCKEVENT_SOCKETCLOSED:
	case DEF_XSOCKEVENT_QUENEFULL:
		wsprintf(G_cTxt, "(!) Main-Log-Socket(%d) connection lost! Try to Reconnect...", iMainH);
		PutLogList(G_cTxt);
		m_iActiveMainLogSock--;
		delete m_pMainLogSock[iMainH];
		m_iTotalMainLogSock++;
		m_pMainLogSock[iMainH]= new class XSocket(m_hWnd, DEF_SERVERSOCKETBLOCKLIMIT);
		m_pMainLogSock[iMainH]->bConnect(m_cMainLogAddress, m_iMainLogPort, (WM_ONMAINLOGSOCKETEVENT + iMainH));
		m_pMainLogSock[iMainH]->bInitBufferSize(DEF_MSGBUFFERSIZE);
		m_bisMainRegistered[iMainH] = FALSE;
		if ((m_iActiveMainLogSock <= 0) && (m_bIsWorldRegistered == TRUE)) m_bIsWorldRegistered = FALSE;
		break;
	}
}

BOOL CWorldLog::bPutMsgQuene(char cFrom, char * pData, DWORD dwMsgSize, int iIndex, char cKey)
{
	if (m_pMsgQueue[m_iQueueTail] != NULL) return FALSE;

	m_pMsgQueue[m_iQueueTail] = new class CMsg;
	if (m_pMsgQueue[m_iQueueTail] == NULL) return FALSE;

	if (m_pMsgQueue[m_iQueueTail]->bPut(cFrom, pData, dwMsgSize, iIndex, cKey) == FALSE) return FALSE;

	m_iQueueTail++;
	if (m_iQueueTail >= DEF_MSGQUEUESIZE) m_iQueueTail = 0;
	
	return TRUE;
}

BOOL CWorldLog::bGetMsgQuene(char * pFrom, char * pData, DWORD * pMsgSize, int * pIndex, char * pKey)
{

	if (m_pMsgQueue[m_iQueueHead] == NULL) return FALSE;

	m_pMsgQueue[m_iQueueHead]->Get(pFrom, pData, pMsgSize, pIndex, pKey);

	delete m_pMsgQueue[m_iQueueHead];
	m_pMsgQueue[m_iQueueHead] = NULL;

	m_iQueueHead++;
	if (m_iQueueHead >= DEF_MSGQUEUESIZE) m_iQueueHead = 0;

	return TRUE;
}

void CWorldLog::OnTimer()
{
DWORD dwTime;
dwTime = timeGetTime();

	MsgProcess();
if ((dwTime - m_dwGameTime1) > 1000)	{
		CheckClientTimeout();
		m_dwGameTime1 = dwTime;
}
}

void CWorldLog::MsgProcess()
{
 char   * pData=NULL, cFrom, cKey;
 DWORD    dwMsgSize, * dwpMsgID;
 int     iClientH;
 char cDump[1000];

	ZeroMemory(m_pMsgBuffer, DEF_MSGBUFFERSIZE+1);
	pData = (char *)m_pMsgBuffer;
	while (bGetMsgQuene(&cFrom, pData, &dwMsgSize, &iClientH, &cKey) == TRUE) {
		dwpMsgID   = (DWORD *)(pData + DEF_INDEX4_MSGID);
		//wsprintf(G_cTxt, "(X) Recieved Message (0x%.8X)", *dwpMsgID);
		//PutLogList(G_cTxt);
		switch (*dwpMsgID) {
			
			case MSGID_REQUEST_ENTERGAME:
				EnterGame(iClientH, pData);
				break;

			case MSGID_REQUEST_REGISTERGAMESERVER:
				ClientRegisterGameserver(iClientH, pData);
				break;

			case MSGID_RESPONSE_REGISTER_WORLDSERVERSOCKET:
				ClientMSLConfirmed(iClientH);
				break;

			case MSGID_REQUEST_CHARINFOLIST:
				CharInfoList(iClientH, pData);
				break;
					
			case MSGID_REQUEST_FORCEDISCONECTACCOUNT:
				ForceDisconnectAccount(iClientH, pData);
				break;

			case MSGID_REQUEST_SETACCOUNTINITSTATUS:
				SetAccountStatusInit(iClientH, pData);
				break;

			case MSGID_REQUEST_DELETECHARACTER:
				RequestDeleteCharacter(iClientH, pData);
				break;

			case MSGID_REQUEST_CREATENEWCHARACTER:
				RequestCreateNewCharacter(iClientH, pData);
				break;

			case MSGID_SENDSERVERSHUTDOWNMSG:
				SendServerShutdownMessage();
				break;

			case MSGID_REQUEST_SETACCOUNTWAITSTATUS:
				RequestSetAccountWaitStatus(pData);
				break;
				
			case MSGID_REQUEST_CREATENEWGUILD:
				RequestCreateNewGuild(iClientH, pData);
				break;
				
			case MSGID_REQUEST_DISBANDGUILD:
				RequestDisbandGuild(iClientH, pData);
				break;

			case MSGID_REQUEST_UPDATEGUILDINFO_NEWGUILDSMAN:
				UpdateGuildInfoNewGuildsman(iClientH, pData);
				break;

			case MSGID_REQUEST_UPDATEGUILDINFO_DELGUILDSMAN:
				UpdateGuildInfoDeleteGuildman(iClientH, pData);
				break;

			case MSGID_REQUEST_SAVEARESDENOCCUPYFLAGDATA:
				//RequestSaveOccupyFlag(iClientH, pData, 1);
				break;

			case MSGID_REQUEST_SAVEELVINEOCCUPYFLAGDATA:
				//RequestSaveOccupyFlag(iClientH, pData, 2);
				break;

			case MSGID_GAMESERVERSHUTDOWNED:
				ShutdownGameServer(iClientH);
				break;
				
			case MSGID_ENTERGAMECONFIRM:
				EnterGameConfirm(iClientH, pData);
				break;

			case MSGID_GAMEMASTERLOG:
				PutGMLogData(pData, dwMsgSize, 0);
				break;

			case MSGID_GAMEITEMLOG:
				PutItemLogData(pData, dwMsgSize, 0);
				break;

			case MSGID_CRUSADELOG:
				PutCrusadeLogData(pData, dwMsgSize, 0);
				break;

			case MSGID_REQUEST_PLAYERDATA:
				RequestPlayerData(iClientH, pData);
				break;

			case MSGID_REQUEST_SAVEPLAYERDATA:
				RequestSavePlayerData(iClientH, pData, dwMsgSize, 0, 0);
				break;

			case MSGID_REQUEST_SAVEPLAYERDATA_REPLY:
				RequestSavePlayerData(iClientH, pData, dwMsgSize, 1, 1);
				break;

			case MSGID_REQUEST_SAVEPLAYERDATALOGOUT:
				RequestSavePlayerData(iClientH, pData, dwMsgSize, 1, 0);
				break;

			case MSGID_REQUEST_NOSAVELOGOUT:
				RequestNoSaveLogout(iClientH, pData);
				break;
				
			default:
				wsprintf(cDump, "Unknown message received! (0x%.8X)", *dwpMsgID);
				PutLogList(cDump);
				PutPacketLogData(*dwpMsgID, pData, dwMsgSize);
				break;
		}
		break;
	}
	UpdateScreen();
}

void CWorldLog::ClientRegisterGameserver(int iClientH, char * pData)
{
 char * cp, *cp2, *cMapNames;
 char cGameServerName[11];
 char cGameServerSecurity[11];
 char cGameServerIP[16];
 int iGameServerPort;
 char cGameServerLanAddress[11];
 char iTotalMaps;
 WORD *wp;
 DWORD *dwp;
 char cData[1110];
 int i, x;
 BOOL bBool;
 BOOL bCheckHGServer;

	bBool = TRUE;
	bCheckHGServer = FALSE;
	if (m_pClientList[iClientH] == NULL) return;
	ZeroMemory(cGameServerName, sizeof(cGameServerName));
	ZeroMemory(cGameServerIP, sizeof(cGameServerIP));
	ZeroMemory(cGameServerSecurity, sizeof(cGameServerSecurity));
	ZeroMemory(cGameServerLanAddress, sizeof(cGameServerLanAddress));

	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

	memcpy(cGameServerName, cp, 10);
	cp += 10;

	memcpy(cGameServerIP, cp, 16);
	cp += 16;

	wp = (WORD *)cp;
	iGameServerPort = *wp;
	cp += 2;

	iTotalMaps = *cp;
	cp++;

	/*memcpy(cGameServerSecurity, cp, 10);
	cp += 10;

	memcpy(cGameServerLanAddress, cp, 16);
	cp += 16;*/

	// Hypnotoad - fix preventing extasis hack from getting config files and connecting
	for (i = 0; i < DEF_MAXHGSERVERLIST; i++) {
		if (memcmp(m_sGameServerList[i << 4], cGameServerIP, 16) == 0) {
			bCheckHGServer = TRUE;
		}
	}
	/*if((memcmp(cGameServerSecurity, cGameSecurity, strlen(cGameServerSecurity)) != 0)) {
		wsprintf(G_cTxt,"(XXX) Unauthorized Access! Name(%s) Addr(%s)- security fail(%s) - != 0", cGameServerName, cGameServerIP, cGameSecurity);
		PutLogList(G_cTxt);		
		ResponseRegisterGameServer(iClientH, FALSE);
		return;
	}
	if(strlen(cGameServerSecurity) <= 0) {
		wsprintf(G_cTxt,"(XXX) Unauthorized Access! Name(%s) Addr(%s)- security fail(%s) - <= 0", cGameServerName, cGameServerIP, cGameSecurity);
		PutLogList(G_cTxt);		
		ResponseRegisterGameServer(iClientH, FALSE);
		return;
}*/
	//end security
	if ((bCheckHGServer == FALSE) && (m_bGameServerList == TRUE)) {
		wsprintf(G_cTxt,"(XXX) Unauthorized Access! Name(%s) Addr(%s) Port(%d) Maps(%d)", cGameServerName, cGameServerIP, iGameServerPort, iTotalMaps);
		PutLogList(G_cTxt);		
		ResponseRegisterGameServer(iClientH, FALSE);
		return;
	}
	
	for (i = 0; i < DEF_MAXGAMESERVERS; i++) {
		if (m_pGameList[i] != NULL) {
			if (memcmp(m_pGameList[i]->m_cGameServerName, cGameServerName, 10) == 0) {
				ResponseRegisterGameServer(iClientH, FALSE);
				wsprintf(G_cTxt, "(!) Game Server Registration - fail : Server-Name(%s) already exists.", cGameServerName);
				PutLogList(G_cTxt);
				return;
			}
		}
	}

	for (i = 0; i < DEF_MAXGAMESERVERS; i++) {
		if (m_pGameList[i] == NULL) {
			cMapNames = (char *)cp;
			m_pGameList[i] = new class CGame(iClientH, cGameServerName, cGameServerIP, iGameServerPort, cGameServerLanAddress);
			for (x = 0; x < iTotalMaps; x++) {
				if (bClientRegisterMaps(i, cp) == 0) {
					bBool = FALSE;
				}
				cp += 11;
			}
			if (bBool == TRUE) {
				// thanks SlammeR for providing 3.51 config files
				if (!bSendClientConfig(iClientH, "Npc.cfg") || 
					!bSendClientConfig(iClientH, "Item.cfg") ||
					!bSendClientConfig(iClientH, "Item2.cfg") || 
					!bSendClientConfig(iClientH, "Magic.cfg") ||
					!bSendClientConfig(iClientH, "Skill.cfg") || 
					!bSendClientConfig(iClientH, "Item3.cfg") ||
					!bSendClientConfig(iClientH, "Quest.cfg") || 
					!bSendClientConfig(iClientH, "BuildItem.cfg") ||
					!bSendClientConfig(iClientH, "DupItemID.cfg") || 
					!bSendClientConfig(iClientH, "Noticement.txt") ||
					//!bSendClientConfig(iClientH, "WLserver.cfg") ||
					//!bSendClientConfig(iClientH, "NpcItem.cfg") || 
					!bSendClientConfig(iClientH, "Potion.cfg")) {
					wsprintf(G_cTxt, "(X) Game Server(%s) Registration - Fail: Cannot send configuration data!", cGameServerName);
					PutLogList(G_cTxt);
					ResponseRegisterGameServer(iClientH, FALSE);
					return;
				}
				/*bSendClientConfig(iClientH, "GameData\\ElvineOccupyFlag.txt");
				bSendClientConfig(iClientH, "GameData\\AresdenOccupyFlag.txt");*/
				
				wsprintf(G_cTxt, "(O) Game Server Registration - Success( Name: %s | Addr: %s | Port: %d )", cGameServerName, cGameServerIP, iGameServerPort);
				PutLogList(G_cTxt);
				ResponseRegisterGameServer(iClientH, TRUE);

				ZeroMemory(cData, sizeof(cData));
	
				cp2 = (char *)(cData);

				memcpy(cp2, m_cWorldLogName, 30);
				cp2 += 30;

				memcpy(cp2, cGameServerName, 10);
				cp2 += 10;

				memcpy(cp2, cGameServerIP, 16);
				cp2 += 16;

				dwp = (DWORD *)cp2;
				*dwp = (DWORD)iGameServerPort;
				cp2 += 4;
				
				dwp = (DWORD *)cp2;
				*dwp = (DWORD)iTotalMaps;
				cp2 += 4;

				memcpy(cp2, cMapNames, iTotalMaps*11);
				cp2 += iTotalMaps*11;

				wsprintf(G_cTxt, "(!) Game Server Registration to MLS... ( Name: %s | Addr: %s | Port: %d )", cGameServerName, cGameServerIP, iGameServerPort);
				PutLogList(G_cTxt);

				SendEventToMLS(MSGID_REGISTER_WORLDSERVER_GAMESERVER, DEF_MSGTYPE_CONFIRM, cData, 64 + ((iTotalMaps)*11), -1);
			}
			else {
				wsprintf(G_cTxt, "(X) Game Server(%s) Registration - Fail: Map Duplicated!", cGameServerName);
				PutLogList(G_cTxt);
				ResponseRegisterGameServer(iClientH, FALSE);
				return;
			}
		return;
	}
	if((m_pGameList[i] != NULL)&& (i >= DEF_MAXGAMESERVERS)) {
		ResponseRegisterGameServer(iClientH, FALSE);
		PutLogList("(!) Game Server Registeration - Fail : No more Game Server can be connected.");
		}
	}
}

void CWorldLog::ResponseRegisterGameServer(int iClientH, BOOL bSuccesfull)
{
 int iRet;
 DWORD * dwp;
 WORD * wp;
 char cData[20];

	if (m_pClientList[iClientH] == NULL) return;
	ZeroMemory(cData, sizeof(cData));
	
	dwp  = (DWORD *)(cData + DEF_INDEX4_MSGID);
	*dwp = MSGID_RESPONSE_REGISTERGAMESERVER;
	wp   = (WORD *)(cData + DEF_INDEX2_MSGTYPE);

	if (bSuccesfull) {
		*wp  = DEF_MSGTYPE_CONFIRM;
	}
	else {
		*wp  =  DEF_MSGTYPE_REJECT;
	}
	
	iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 6, DEF_USE_ENCRYPTION);

	switch (iRet) {
	case DEF_XSOCKEVENT_QUENEFULL:
	case DEF_XSOCKEVENT_SOCKETERROR:
	case DEF_XSOCKEVENT_CRITICALERROR:
	case DEF_XSOCKEVENT_SOCKETCLOSED:
		DeleteClient(iClientH, 0);
		delete m_pClientList[iClientH];
		m_pClientList[iClientH] = NULL;
		break;
	}
}

bool CWorldLog::bClientRegisterMaps(int iClientH, char *pData)
{
 int i;

	for (i = 0; i < DEF_MAXGAMESERVERS; i++) {
		if (m_cMapList[i] != NULL) {
			if (memcmp(m_cMapList[i]->m_cName, pData, 10) == 0) {
				wsprintf(G_cTxt, "(X) CRITICAL ERROR! Map(%s) duplicated!", pData);
				PutLogList(G_cTxt);
				return 0;
			}
		}
	}
	for (i = 0; i < DEF_MAXGAMESERVERS; i++) {
		if (m_cMapList[i] == NULL) {
			m_cMapList[i] = new class CMap(iClientH, pData);
			wsprintf(G_cTxt, "(O) Server(%s) registers map (%s)", m_pGameList[iClientH]->m_cGameServerName, pData);
			PutLogList(G_cTxt);
			return 1;			
		}
	}
	wsprintf(G_cTxt, "(X) No more map space left. Map(%s) cannot be added.", pData);
	PutLogList(G_cTxt);
	return 0;
}

bool CWorldLog::bSendClientConfig(int iClientH, char *cFile)
{
 DWORD dwMsgID, lpNumberOfBytesRead;
 DWORD *dwp;
 WORD *wp;
 int iRet;

	if (strcmp(cFile, "Npc.cfg") == 0) {
		dwMsgID = MSGID_NPCCONFIGURATIONCONTENTS;
	}
	else if (strcmp(cFile, "Item.cfg") == 0) {
		dwMsgID = MSGID_ITEMCONFIGURATIONCONTENTS;
	}
	else if (strcmp(cFile, "Item2.cfg") == 0) {
		dwMsgID = MSGID_ITEMCONFIGURATIONCONTENTS;
	}
	else if (strcmp(cFile, "Item3.cfg") == 0) {
		dwMsgID = MSGID_ITEMCONFIGURATIONCONTENTS;
	}
	else if (strcmp(cFile, "Magic.cfg") == 0) {
		dwMsgID = MSGID_MAGICCONFIGURATIONCONTENTS;
	}
	else if (strcmp(cFile, "Skill.cfg") == 0) {
		dwMsgID = MSGID_SKILLCONFIGURATIONCONTENTS;
	}
	else if (strcmp(cFile, "Potion.cfg") == 0) {
		dwMsgID = MSGID_PORTIONCONFIGURATIONCONTENTS;
	}
	else if (strcmp(cFile, "Quest.cfg") == 0) {
		dwMsgID = MSGID_QUESTCONFIGURATIONCONTENTS;
	}
	else if (strcmp(cFile, "BuildItem.cfg") == 0) {
		dwMsgID = MSGID_BUILDITEMCONFIGURATIONCONTENTS;
	}
	else if (strcmp(cFile, "DupItemID.cfg") == 0) {
		dwMsgID = MSGID_DUPITEMIDFILECONTENTS;
	}
	else if (strcmp(cFile, "Noticement.txt") == 0) {
		dwMsgID = MSGID_NOTICEMENTFILECONTENTS;
	}
	/*else if (strcmp(cFile, "GameData\\AresdenOccupyFlag.txt") == 0) {
		dwMsgID = MSGID_ARESDENOCCUPYFLAGSAVEFILECONTENTS;
	}
	else if (strcmp(cFile, "GameData\\ElvineOccupyFlag.txt") == 0) {
		dwMsgID = MSGID_ELVINEOCCUPYFLAGSAVEFILECONTENTS;
	}
	else if (strcmp(cFile, "NpcItem.cfg") == 0) {
		dwMsgID = MSGID_NPCITEMCONFIGCONTENTS;
	}
	else if (strcmp(cFile, "WLserver.cfg") == 0) {
		dwMsgID = MSGID_WLSERVERFILECONTENTS;
	}*/
	else { 
		return false;
	}

	HANDLE hFile = CreateFile(cFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, 0);
	DWORD dwFileSize = GetFileSize(hFile, NULL);
	if (dwFileSize == -1) {
		wsprintf(G_cTxt, "(X) CRITICAL ERROR! Cannot open configuration file(%s)!", cFile);
		PutLogList(cFile);
		return false;
	}

	ZeroMemory(G_cData50000, sizeof(G_cData50000));

	wsprintf(G_cTxt, "(!) Reading %s configuration file...", cFile);
	PutLogList(G_cTxt);
	SetFilePointer(hFile, 0, 0, FILE_BEGIN);

	ReadFile(hFile, G_cData50000+6, dwFileSize, &lpNumberOfBytesRead, NULL);
	CloseHandle(hFile);
	
	dwp = (DWORD *)(G_cData50000);
	*dwp = dwMsgID;

	wp = (WORD *)(G_cData50000 + DEF_INDEX2_MSGTYPE);
	*wp = DEF_MSGTYPE_CONFIRM;

	iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(G_cData50000, dwFileSize+8, DEF_USE_ENCRYPTION);

	switch (iRet) {
	case DEF_XSOCKEVENT_QUENEFULL:
	case DEF_XSOCKEVENT_SOCKETERROR:
	case DEF_XSOCKEVENT_CRITICALERROR:
	case DEF_XSOCKEVENT_SOCKETCLOSED:
		wsprintf(G_cTxt,"(X) Cannot send configuration file(%s) contents to GameServer!", cFile);
		PutLogList(G_cTxt);
		DeleteClient(iClientH, 0);
		delete m_pClientList[iClientH];
		m_pClientList[iClientH] = NULL;
		return false;

	default:
		wsprintf(G_cTxt, "(O) Send configuration file(%s) contents to GameServer.", cFile);
		PutLogList(G_cTxt);
		break;
	}

	return true;
}

void CWorldLog::DeleteClient(int iClientH, short Mod)
{
 char cData[120];
 char *cp;
 int i,j;

	if((Mod == 0) || (Mod < 0)) {
	if (m_pClientList[iClientH] == NULL) return;
	for (i=0; i < DEF_MAXGAMESERVERS; i++) {
		if ((m_pGameList[i] != NULL) && (m_pGameList[i]->m_iTrackSock == iClientH)) {
			wsprintf(G_cTxt, "(!) Game server connection lost! (%s)", m_pGameList[i]->m_cGameServerName);
			PutLogList(G_cTxt);
		
			ZeroMemory(cData, sizeof(cData));
			
			cp = (char *)cData;
			memcpy(cp, m_cWorldLogName, 30);
			cp += 30;

			memcpy(cp, m_pGameList[i]->m_cGameServerName, 10);
			cp += 10;

			SendEventToMLS(MSGID_REQUEST_REMOVEGAMESERVER, DEF_MSGTYPE_CONFIRM, cData, 40, -1);

			for (j = 0; j < DEF_MAXPLAYERACCOUNTS; j++) {
				if ((m_pAccountList[j] != NULL) && (strcmp(m_pAccountList[j]->m_cOnGameServer, m_pGameList[i]->m_cGameServerName) == 0)) {

					ZeroMemory(cData, sizeof(cData));

					cp = (char *)cData;
					memcpy(cp, m_pAccountList[j]->cAccountName, 10);
					cp += 10;

					wsprintf(G_cTxt, "(TestLog) Account(%s) log out request to MLS...", m_pAccountList[j]->cAccountName);
					PutLogList(G_cTxt);

					SendEventToMLS(MSGID_REQUEST_CLEARACCOUNTSTATUS, DEF_MSGTYPE_CONFIRM, cData, 10, -1);
					SendEventToMLS(MSGID_REQUEST_CLEARACCOUNTSTATUS, DEF_MSGTYPE_CONFIRM, m_pAccountList[j]->cAccountName, 10, -1);
					OnPlayerAccountMessage(DEF_MSGACCOUNTSET_NULL, m_pAccountList[j]->cAccountName, NULL, NULL, NULL);
				}
			}//account loop

			for (j = 0; j < DEF_MAXGAMESERVERSMAPS; j++) {
				if ((m_cMapList[j] != NULL) && (m_cMapList[j]->iIndex == i)) {
					wsprintf(G_cTxt, "(!) Delete map (%s)", m_cMapList[j]->m_cName);
					PutLogList(G_cTxt);
					delete m_cMapList[j];
					m_cMapList[j] = NULL;
				}
			}//map loop
				delete m_pGameList[i];
				m_pGameList[i] = NULL;
		}
	}//mapserver loop
	}//mod 0
	if((Mod == 1) || (Mod < 0)) {
		if(m_pAccountList[iClientH] == NULL) return;
					wsprintf(G_cTxt, "(TestLog) Account(%s) log out request to MLS(Delete)...", m_pAccountList[iClientH]->cAccountName);
					PutLogList(G_cTxt);
					SendEventToMLS(MSGID_REQUEST_CLEARACCOUNTSTATUS, DEF_MSGTYPE_CONFIRM, m_pAccountList[iClientH]->cAccountName, 10, -1);
					OnPlayerAccountMessage(DEF_MSGACCOUNTSET_NULL, m_pAccountList[iClientH]->cAccountName, NULL, NULL, NULL);
	}//Mod 1 for accounts
}

void CWorldLog::ShutdownGameServer(int iClientH)
{
 int i;

	for (i = 0; i < DEF_MAXGAMESERVERS; i++) {
		if ((m_pGameList[i] != NULL) && (m_pGameList[i]->m_iTrackSock == iClientH)) {
			m_pGameList[i]->m_cGameServerMode = 3;
		}
	}
}

void CWorldLog::EnterGameConfirm(int iClientH, char *pData)
{
 char cGameServerName[11], cCharacterName[11], cAccountName[11], cAddress[16], cAccountPass[16];
 char cTemp[100];
 char cCharNameCheck[11];
 int iLevel, iCheckLevel, i;
 char *cp;
 int *ip, iRet;
 DWORD *dwp;
 WORD *wp;

	if (m_pClientList[iClientH] == NULL) return;

	ZeroMemory(cAccountName, sizeof(cAccountName));
	ZeroMemory(cCharacterName, sizeof(cCharacterName));
	ZeroMemory(cGameServerName, sizeof(cGameServerName));
	ZeroMemory(cAddress, sizeof(cAddress));
	ZeroMemory(cCharNameCheck, sizeof(cCharNameCheck));
	ZeroMemory(cTemp, sizeof(cTemp));
	ZeroMemory(cAccountPass, sizeof(cAccountPass));

	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

	memcpy(cAccountName, cp, 10);
	cp += 10;

	memcpy(cCharacterName, cp, 10); //password
	cp += 10;

	memcpy(cGameServerName, cp, 10);
	cp += 10;

	memcpy(cAddress, cp, 16);
	cp += 16;

	ip = (int *)cp;
	iLevel = *ip;
	cp += 4;

	switch(iGetCharacterInformation(cAccountName, cCharNameCheck, &iCheckLevel)) { // get real acconts pass
	case 1:
	case 2:
		if ((strcmp(cCharacterName, cCharNameCheck) != 0) || (iCheckLevel != iLevel)) {
			wsprintf(G_cTxt, "(TestLog) ERROR! Account(%s) Password or level mismatch. Request Game Server to disconnect without data save...", cAccountName);
			PutLogList(G_cTxt);
			wsprintf(G_cTxt, "(TestLog) %s - %s, %d - %d", cCharacterName, cCharNameCheck, iLevel, iCheckLevel);
			PutLogList(G_cTxt);
			dwp = (DWORD *)(cTemp);
			*dwp = MSGID_REQUEST_FORCEDISCONECTACCOUNT;
			wp  = (WORD *)(cTemp + DEF_INDEX2_MSGTYPE);
			*wp = DEF_MSGTYPE_CONFIRM;
			cp  = (char *)(cTemp + DEF_INDEX2_MSGTYPE + 2);
			memcpy(cp, cAccountName, 10);
			cp += 10;
			iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cTemp, 16, DEF_USE_ENCRYPTION);
			switch(iRet) {
			case DEF_XSOCKEVENT_QUENEFULL:
			case DEF_XSOCKEVENT_SOCKETERROR:
			case DEF_XSOCKEVENT_CRITICALERROR:
			case DEF_XSOCKEVENT_SOCKETCLOSED:
				DeleteClient(iClientH, 0);
				delete m_pClientList[iClientH];
				m_pClientList[iClientH] = NULL;
				return;
			}					
			return;
		}	
		break;
	}
//set account timeout to -1
for (i = 0; i < DEF_MAXPLAYERACCOUNTS; i++)
if((m_pAccountList[i] != NULL) && (strcmp(m_pAccountList[i]->cAccountName, cAccountName) == 0)) m_pAccountList[i]->Timeout = -1;
	OnPlayerAccountMessage(DEF_MSGACCOUNTSET_PLAYING, cAccountName, NULL, NULL, cGameServerName);
	return;
}

/*
void CWorldLog::RequestPlayerData(int iClientH, char *pData)
{
 char cCharacterName[11], cAccountName[11], cFileName[255], cDir[63];
 char *cp, *cp2;
 DWORD *dwp;
 WORD *wp;
 char cVerifyAccountName[11];
 char cGuildName[21], cGuildRank;
 int iVerifyLevel;
 FILE * pFile;
 int iRet, iSize;
 HANDLE hFile;
 DWORD dwFileSize, dwVerifyGuildGUID, dwGuildGUID, lpNumberOfBytesRead;
 BOOL bGuildCheck;

	pFile = NULL;
	bGuildCheck = FALSE;
	if (m_pClientList[iClientH] == NULL) return;

	ZeroMemory(G_cData50000, sizeof(G_cData50000));
	ZeroMemory(cCharacterName, sizeof(cCharacterName));
	ZeroMemory(cAccountName, sizeof(cAccountName));

	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

	memcpy(cCharacterName, cp, 10);
	cp += 10;

	memcpy(cAccountName, cp, 10);
	cp += 10;

	if (strlen(cCharacterName) == 0) PutLogList("(X) CharName NULL!");
	if (strlen(cAccountName) == 0) PutLogList("(X) AccountName NULL!");

	if (G_bDBMode == TRUE) {
		// todo: add SQL Code Here
		ZeroMemory(G_cData50000, sizeof(G_cData50000));
		iSize = -1;
		//iSize = iCreateNewCharacterSQL(cCharacterName, cAccountName, G_cData50000, bGuildCheck);
		if (iSize == -1) {
			wsprintf(G_cTxt, "(TestLog) Send character data error(%s) data...", cCharacterName);
			PutLogList(G_cTxt);

			dwp = (DWORD *)(G_cData50000);
			*dwp = MSGID_RESPONSE_PLAYERDATA;
			
			wp  = (WORD *)(G_cData50000 + DEF_INDEX2_MSGTYPE);
			*wp = DEF_MSGTYPE_REJECT;

			cp  = (char *)(G_cData50000 + DEF_INDEX2_MSGTYPE + 2);
			memcpy(cp, cCharacterName, 10);
			cp += 10;

			iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(G_cData50000, 16, DEF_USE_ENCRYPTION);
		}
		else {
			
		}
	}
	else {	
		VerifyCharacterIntegrity(cCharacterName, cVerifyAccountName, &iVerifyLevel, cGuildName, &cGuildRank, &dwGuildGUID);

		ZeroMemory(cFileName, sizeof(cFileName));
		ZeroMemory(cDir, sizeof(cDir));
		strcat(cFileName, "Character");
		strcat(cFileName, "\\");
 		strcat(cFileName, "\\");
		wsprintf(cDir, "AscII%d", *cCharacterName);
		strcat(cFileName, cDir);
		strcat(cFileName, "\\");
		strcat(cFileName, "\\");
		strcat(cFileName, cCharacterName);	
		strcat(cFileName, ".txt");	

		hFile = CreateFileA(cFileName, GENERIC_READ, NULL, NULL, OPEN_EXISTING, NULL, NULL);
		dwFileSize = GetFileSize(hFile, NULL);
		if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);

		pFile = fopen(cFileName, "rt");
		if ((pFile != NULL) && (strcmp(cAccountName, cVerifyAccountName) == 0)) {

			if(dwFileSize > 30000) { //file size
			dwp = (DWORD *)(G_cData50000);
			*dwp = MSGID_RESPONSE_PLAYERDATA;
			
			wp  = (WORD *)(G_cData50000 + DEF_INDEX2_MSGTYPE);
			*wp = DEF_MSGTYPE_REJECT;

			cp  = (char *)(G_cData50000 + DEF_INDEX2_MSGTYPE + 2);
			memcpy(cp, cCharacterName, 10);
			cp += 10;

			iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(G_cData50000, 16, DEF_USE_ENCRYPTION);

			wsprintf(G_cTxt, "(TestLog) Character(%s) data size to big", cCharacterName);
			PutLogList(G_cTxt);
			} //file size to big
					
			dwp = (DWORD *)(G_cData50000);
			*dwp = MSGID_RESPONSE_PLAYERDATA;
			
			wp  = (WORD *)(G_cData50000 + DEF_INDEX2_MSGTYPE);
			*wp = DEF_MSGTYPE_CONFIRM; //send character daata

			cp  = (char *)(G_cData50000 + DEF_INDEX2_MSGTYPE + 2);
			memcpy(cp, cCharacterName, 10);
			cp += 10;
			
			// HGServer 2.0+ doesnt use this anymore
			//*cp = 2; //2.0+ fix
			//cp++; //2.0+ fix
			
			if (cGuildRank >= 0) {
				VerifyGuildIntegrity(cGuildName, &dwVerifyGuildGUID);
				if (dwVerifyGuildGUID == dwGuildGUID) {
					bGuildCheck = TRUE;
				}
			}
			
			*cp = bGuildCheck;
			cp++;			

			// can cause hgserver to crash because of this packet packet size!!!
			// need to make fix in hgserver in regards to MSGID_RESPONSE_PLAYERDATA's dwSize
			// eg if (dwSize > BUFFER) return; or something this is extremely exploitable
			// all you have to do is send a huge txt file or packet to the server and crash
			cp2 = new char[dwFileSize+1];
			ZeroMemory(cp2, dwFileSize+1);
			ReadFile(hFile, cp2, dwFileSize, &lpNumberOfBytesRead, NULL);
			CloseHandle(hFile);

			memcpy(cp, cp2, dwFileSize + 1);
			cp += dwFileSize + 1;

			iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(G_cData50000, dwFileSize + 19, DEF_USE_ENCRYPTION);

			wsprintf(G_cTxt, "(TestLog) Send character(%s) data...", cCharacterName);
			PutLogList(G_cTxt);
		}
		else {
			wsprintf(G_cTxt, "(X) RequestPlayerData Error! Fn(%s) Account(%s, %s)", cFileName, cAccountName, cVerifyAccountName);
			PutLogList(G_cTxt);
			
			dwp = (DWORD *)(G_cData50000);
			*dwp = MSGID_RESPONSE_PLAYERDATA;
			
			wp  = (WORD *)(G_cData50000 + DEF_INDEX2_MSGTYPE);
			*wp = DEF_MSGTYPE_REJECT; //player data error
			
			cp  = (char *)(G_cData50000 + DEF_INDEX2_MSGTYPE + 2);
			memcpy(cp, cCharacterName, 10);
			cp += 10;

			iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(G_cData50000, 16, DEF_USE_ENCRYPTION);
		}
	}
	switch(iRet) {
	case DEF_XSOCKEVENT_QUENEFULL:
	case DEF_XSOCKEVENT_SOCKETERROR:
	case DEF_XSOCKEVENT_CRITICALERROR:
	case DEF_XSOCKEVENT_SOCKETCLOSED:
		DeleteClient(iClientH, 0);
		delete m_pClientList[iClientH];
		m_pClientList[iClientH] = NULL;
		return;
	}	
}
*/
void CWorldLog::RequestPlayerData(int iClientH, char* pData)
{
	char cCharacterName[11], cAccountName[11], cFileName[255], cDir[63];
	char* cp, * cp2;
	int* ip;
	DWORD* dwp;
	WORD* wp;
	char cVerifyAccountName[11];
	char cGuildName[21], cGuildRank;
	int iVerifyLevel, iCharacterDBid;
	FILE* pFile;
	int iRet;
	long iSize;
	HANDLE hFile;
	DWORD dwFileSize, dwVerifyGuildGUID, dwGuildGUID, lpNumberOfBytesRead, dwTime;
	BOOL bGuildCheck;

	pFile = NULL;
	bGuildCheck = FALSE;
	if (m_pClientList[iClientH] == NULL) return;

	ZeroMemory(G_cData50000, sizeof(G_cData50000));
	ZeroMemory(cCharacterName, sizeof(cCharacterName));
	ZeroMemory(cAccountName, sizeof(cAccountName));

	dwTime = timeGetTime();

	cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);

	memcpy(cCharacterName, cp, 10);
	cp += 10;

	memcpy(cAccountName, cp, 10);
	cp += 10;

	if (strlen(cCharacterName) == 0) { PutLogList("(X) CharName NULL!"); return; }
	if (strlen(cAccountName) == 0) { PutLogList("(X) AccountName NULL!"); return; }

	int accDBID = iGetAccountDatabaseID(cAccountName);


	if (G_bDBMode == TRUE) {
		ZeroMemory(G_cData50000, sizeof(G_cData50000));
		iSize = -1;

		cp2 = new char[50000];
		ZeroMemory(cp2, 50000);

		SACommand com, com2;

		try
		{
			com.setConnection(&con);
			com.setCommandText("SELECT [Character-ID], [Account-ID], RTRIM([Character-Name]) AS 'Character-Name', [Character-Profile], [Character-Location], [Character-Guild-Name], [Character-Guild-GUID], [Character-Guild-Rank], [Character-Loc-Map], [Character-Loc-X], [Character-Loc-Y], [Character-HP], [Character-MP], [Character-SP], [Character-Level], [Character-Rating], [Character-Strength], [Character-Intelligence], [Character-Vitality], [Character-Dexterity], [Character-Magic], [Character-Charisma], [Character-Luck], [Character-Experience], [Character-LU-Pool], [Character-Ek-Count], [Character-Pk-Count], [Character-Reward-Gold], [Character-Down-Skill-Index], [Character-ID1], [Character-ID2], [Character-ID3], [Character-Sex], [Character-Skin], [Character-Hair-Style], [Character-Hair-Colour], [Character-Underwear], [Character-Hunger], [Character-Shutup-Time], [Character-Rating-Time], [Character-Force-Time], [Character-SP-Time], [Character-Admin-Level], [Character-Block-Date], [Character-Quest-Number], [Character-Quest-ID], [Character-Quest-Reward], [Character-Quest-Amount], [Character-Contribution], [Character-War-Contribution], [Character-Event-ID], [Character-Criticals], [Character-Fightzone-ID], [Character-Ability-Time], [Character-Lock-Map], [Character-Lock-Time], [Character-Crusade-Job], [Character-Crusade-GUID], [Character-Construct-Points], [Character-Death-Time], [Character-Party-ID], [Character-Majestics], [Character-Appr1], [Character-Appr2], [Character-Appr3], [Character-Appr4], [Character-Appr-Colour], [Character-Magic-Mastery], [Character-Skill-Mastery], [Character-Skill-SSN], [Character-Equip-Status], [Character-Item-Pos-X], [Character-Item-Pos-Y]  FROM         Characters WHERE     ([Character-Name] = :1 AND [Account-ID] = :2)");
			com.Param(1).setAsString() = cCharacterName;
			com.Param(2).setAsLong() = accDBID;

			com.Execute();

			if (com.isResultSet())
			{
				char cTest[256];
				char cTemp[500];

				strcat(cp2, "  ");
				iSize += 2;

				while (com.FetchNext())
				{
					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-dbid = %s", com.Field("Character-ID").asString());
					iCharacterDBid = atoi(com.Field("Character-ID").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-loc-map = %s", com.Field("Character-Loc-Map").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-loc-x = %s", com.Field("Character-Loc-X").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-loc-y = %s", com.Field("Character-Loc-Y").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "sex-status = %s", com.Field("Character-Sex").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "skin-status = %s", com.Field("Character-Skin").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "hairstyle-status = %s", com.Field("Character-Hair-Style").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "haircolor-status = %s", com.Field("Character-Hair-Colour").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "underwear-status = %s", com.Field("Character-Underwear").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-guild-name = %s", com.Field("Character-Guild-Name").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-guild-rank = %s", com.Field("Character-Guild-Rank").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-HP = %s", com.Field("Character-HP").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-LEVEL = %s", com.Field("Character-Level").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-STR = %s", com.Field("Character-Strength").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-INT = %s", com.Field("Character-Intelligence").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-DEX = %s", com.Field("Character-Dexterity").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-MAG = %s", com.Field("Character-Magic").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-VIT = %s", com.Field("Character-Vitality").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-CHARISMA = %s", com.Field("Character-Charisma").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-LUCK = %s", com.Field("Character-Luck").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-EXP = %s", com.Field("Character-Experience").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-location = %s", com.Field("Character-Location").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-MP = %s", com.Field("Character-MP").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-SP = %s", com.Field("Character-SP").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-LU_Pool = %s", com.Field("Character-LU-Pool").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-EK-Count = %s", com.Field("Character-Ek-Count").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-PK-Count = %s", com.Field("Character-Pk-Count").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-reward-gold = %s", com.Field("Character-Reward-Gold").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "skill-SSN = %s", com.Field("Character-Skill-SSN").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-profile = %s", com.Field("Character-Profile").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "hunger-status = %s", com.Field("Character-Hunger").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "admin-user-level = %s", com.Field("Character-Admin-Level").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "timeleft-shutup = %s", com.Field("Character-Shutup-Time").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "timeleft-rating = %s", com.Field("Character-Rating-Time").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);


					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-RATING = %s", com.Field("Character-Rating").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-guild-GUID = %s", com.Field("Character-Guild-GUID").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-downskillindex = %s", com.Field("Character-Down-Skill-Index").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-IDnum1 = %s", com.Field("Character-ID1").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-IDnum2 = %s", com.Field("Character-ID2").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-IDnum3 = %s", com.Field("Character-ID3").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "penalty-block-date = %s", com.Field("Character-Block-Date").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-quest-number = %s", com.Field("Character-Quest-Number").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "quest-reward-type = %s", com.Field("Character-Quest-Reward").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "quest-reward-amount = %s", com.Field("Character-Quest-Amount").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-contribution = %s", com.Field("Character-Contribution").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-quest-ID = %s", com.Field("Character-Quest-ID").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "timeleft-force-recall = %s", com.Field("Character-Force-Time").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "special-event-id = %s", com.Field("Character-Event-ID").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "super-attack-left = %s", com.Field("Character-Criticals").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "reserved-fightzone-id = %s", com.Field("Character-Fightzone-ID").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "special-ability-time = %s", com.Field("Character-Ability-Time").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-war-contribution = %s", com.Field("Character-War-Contribution").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "locked-map-name = %s", com.Field("Character-Lock-Map").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "locked-map-time = %s", com.Field("Character-Lock-Time").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "crusade-job = %s", com.Field("Character-Crusade-Job").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "construct-point = %s", com.Field("Character-Construct-Points").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "crusade-GUID = %s", com.Field("Character-Crusade-GUID").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "dead-penalty-time = %s", com.Field("Character-Death-Time").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "party-id = %s", com.Field("Character-Party-ID").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "gizon-item-upgade-left = %s", com.Field("Character-Majestics").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					com2.setConnection(&con);
					com2.setCommandText("SELECT * FROM CharItems WHERE [Character-ID] = :1 ORDER BY [Item-ID]");
					com2.Param(1).setAsLong() = iCharacterDBid;

					com2.Execute();

					if (com2.isResultSet())
					{
						long col_count = com2.FieldCount();

						char cTemp2[500];

						while (com2.FetchNext())
						{
							ZeroMemory(cTemp2, sizeof(cTemp2));
							strcpy(cTemp2, "character-item = ");
							for (int k = 3; k < col_count + 1; k++)
							{
								strcat(cTemp2, com2.Field(k).asString());
								strcat(cTemp2, " ");
							}

							strcat(cp2, cTemp2);
							strcat(cp2, "  ");
							iSize += (strlen(cTemp2) + 2);
						}
					}

					com2.setConnection(&con);
					com2.setCommandText("SELECT * FROM BankItems WHERE [Character-ID] = :1 ORDER BY [Item-ID]");
					com2.Param(1).setAsLong() = iCharacterDBid;

					com2.Execute();

					if (com2.isResultSet())
					{
						long col_count = com2.FieldCount();

						char cTemp2[500];

						while (com2.FetchNext())
						{
							ZeroMemory(cTemp2, sizeof(cTemp2));
							strcpy(cTemp2, "character-bank-item = ");
							for (int k = 3; k < col_count + 1; k++)
							{
								strcat(cTemp2, com2.Field(k).asString());
								strcat(cTemp2, " ");
							}

							strcat(cp2, cTemp2);
							strcat(cp2, "  ");
							iSize += (strlen(cTemp2) + 2);
						}
					}

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "magic-mastery = %s", com.Field("Character-Magic-Mastery").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "skill-mastery = %s", com.Field("Character-Skill-Mastery").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "item-equip-status = %s", com.Field("Character-Equip-Status").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "item-position-x = %s", com.Field("Character-Item-Pos-X").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "item-position-y = %s", com.Field("Character-Item-Pos-Y").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);
					
					break;
				}

				// end string
				strcat(cp2, "[EOF]");
				iSize += 3;
			}
			else
			{
				wsprintf(G_cTxt, "(X) RequestPlayerData Error! Account(%s, %s)", cAccountName, cVerifyAccountName);
				PutLogList(G_cTxt);

				dwp = (DWORD*)(G_cData50000);
				*dwp = MSGID_RESPONSE_PLAYERDATA;

				wp = (WORD*)(G_cData50000 + DEF_INDEX2_MSGTYPE);
				*wp = DEF_MSGTYPE_REJECT;

				cp = (char*)(G_cData50000 + DEF_INDEX2_MSGTYPE + 2);
				memcpy(cp, cCharacterName, 10);
				cp += 10;
				iSize += 10;

				iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(G_cData50000, 16, DEF_USE_ENCRYPTION);

				delete cp2;

				return;
			}
		}
		catch (SAException& x)
		{
			try
			{
				con.Rollback();
			}
			catch (SAException&)
			{
			}

			char cTemp[256];
			ZeroMemory(cTemp, sizeof(cTemp));
			wsprintf(cTemp, "(!!!) SQL SERVER ERROR (RequestPlayerData): %s", (const char*)x.ErrText());
			PutLogList(cTemp);
		}

		if (iSize != -1 && iSize > 10) {
			VerifyCharacterIntegrity(cCharacterName, cVerifyAccountName, &iVerifyLevel, cGuildName, &cGuildRank, &dwGuildGUID);

			dwp = (DWORD*)(G_cData50000);
			*dwp = MSGID_RESPONSE_PLAYERDATA;

			wp = (WORD*)(G_cData50000 + DEF_INDEX2_MSGTYPE);
			*wp = DEF_MSGTYPE_CONFIRM;

			cp = (char*)(G_cData50000 + DEF_INDEX2_MSGTYPE + 2);
			memcpy(cp, cCharacterName, 10);
			cp += 10;

			*cp = 2;
			cp++;

			if (cGuildRank >= 0) {
				VerifyGuildIntegrity(cGuildName, &dwVerifyGuildGUID);
				if (dwVerifyGuildGUID == dwGuildGUID) {
					bGuildCheck = TRUE;
				}
			}

			*cp = bGuildCheck;
			cp++;

			memcpy(cp, cp2, iSize + 1);
			cp += iSize + 1;

			iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(G_cData50000, iSize + 19, DEF_USE_ENCRYPTION);

			wsprintf(G_cTxt, "(!) Send character(%s) data from SQL - %i ms", cCharacterName, (int)(timeGetTime() - dwTime));
			PutLogList(G_cTxt);
			UpdateLastLoginTime(cCharacterName);
		}
		else {
			wsprintf(G_cTxt, "(X) RequestPlayerData Error! Account(%s, %s)", cAccountName, cVerifyAccountName);
			PutLogList(G_cTxt);

			dwp = (DWORD*)(G_cData50000);
			*dwp = MSGID_RESPONSE_PLAYERDATA;

			wp = (WORD*)(G_cData50000 + DEF_INDEX2_MSGTYPE);
			*wp = DEF_MSGTYPE_REJECT;

			cp = (char*)(G_cData50000 + DEF_INDEX2_MSGTYPE + 2);
			memcpy(cp, cCharacterName, 10);
			cp += 10;

			iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(G_cData50000, 16, DEF_USE_ENCRYPTION);
		}

		delete cp2;
	}
	else {
		VerifyCharacterIntegrity(cCharacterName, cVerifyAccountName, &iVerifyLevel, cGuildName, &cGuildRank, &dwGuildGUID);

		ZeroMemory(cFileName, sizeof(cFileName));
		ZeroMemory(cDir, sizeof(cDir));
		strcat(cFileName, "Character");
		strcat(cFileName, "\\");
		strcat(cFileName, "\\");
		wsprintf(cDir, "AscII%d", *cCharacterName);
		strcat(cFileName, cDir);
		strcat(cFileName, "\\");
		strcat(cFileName, "\\");
		strcat(cFileName, cCharacterName);
		strcat(cFileName, ".txt");

		hFile = CreateFileA(cFileName, GENERIC_READ, NULL, NULL, OPEN_EXISTING, NULL, NULL);
		dwFileSize = GetFileSize(hFile, NULL);
		if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);

		pFile = fopen(cFileName, "rt");
		if ((pFile != NULL) && (strcmp(cAccountName, cVerifyAccountName) == 0)) {

			dwp = (DWORD*)(G_cData50000);
			*dwp = MSGID_RESPONSE_PLAYERDATA;

			wp = (WORD*)(G_cData50000 + DEF_INDEX2_MSGTYPE);
			*wp = DEF_MSGTYPE_CONFIRM;

			cp = (char*)(G_cData50000 + DEF_INDEX2_MSGTYPE + 2);
			memcpy(cp, cCharacterName, 10);
			cp += 10;

			// HGServer 2.0+ doesnt use this anymore
			*cp = 2;
			cp++;

			if (cGuildRank >= 0) {
				VerifyGuildIntegrity(cGuildName, &dwVerifyGuildGUID);
				if (dwVerifyGuildGUID == dwGuildGUID) {
					bGuildCheck = TRUE;
				}
			}

			*cp = bGuildCheck;
			cp++;

			// can cause hgserver to crash because of this packet packet size!!!
			// need to make fix in hgserver in regards to MSGID_RESPONSE_PLAYERDATA's dwSize
			// eg if (dwSize > BUFFER) return; or something this is extremely exploitable
			// all you have to do is send a huge txt file or packet to the server and crash
			cp2 = new char[dwFileSize + 1];
			ZeroMemory(cp2, dwFileSize + 1);
			ReadFile(hFile, cp2, dwFileSize, &lpNumberOfBytesRead, NULL);
			CloseHandle(hFile);

			memcpy(cp, cp2, dwFileSize + 1);
			cp += dwFileSize + 1;

			iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(G_cData50000, dwFileSize + 19, DEF_USE_ENCRYPTION);

			wsprintf(G_cTxt, "(TestLog) Send character(%s) data...", cCharacterName);
			PutLogList(G_cTxt);
		}
		else {
			wsprintf(G_cTxt, "(X) RequestPlayerData Error! Fn(%s) Account(%s, %s)", cFileName, cAccountName, cVerifyAccountName);
			PutLogList(G_cTxt);

			dwp = (DWORD*)(G_cData50000);
			*dwp = MSGID_RESPONSE_PLAYERDATA;

			wp = (WORD*)(G_cData50000 + DEF_INDEX2_MSGTYPE);
			*wp = DEF_MSGTYPE_REJECT;

			cp = (char*)(G_cData50000 + DEF_INDEX2_MSGTYPE + 2);
			memcpy(cp, cCharacterName, 10);
			cp += 10;

			iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(G_cData50000, 16, DEF_USE_ENCRYPTION);
		}
	}
	switch (iRet) {
	case DEF_XSOCKEVENT_QUENEFULL:
	case DEF_XSOCKEVENT_SOCKETERROR:
	case DEF_XSOCKEVENT_CRITICALERROR:
	case DEF_XSOCKEVENT_SOCKETCLOSED:
		DeleteClient(iClientH, 0);
		delete m_pClientList[iClientH];
		m_pClientList[iClientH] = NULL;
		return;
	}
}
bool CWorldLog::bVerifyCharacterGuildSQL(char *pData, DWORD dwGuildGUID)
{

	// if (strcmp("NONE", pData) == 0) return 0;
	// wsprintf(cTxt, "select * from GUILD_T where cGuildName '%s'", pData);
	// wsprintf(cTxt, "%x-(1241) sql (%s)"
	// PutGameLogData(cTxt);
	// MessageBox(m_hWnd, cTxt, "DB connect Error", MB_OK+MB_ICONERROR);
	// if (????? != dwGuildGUID) return 0;
	return TRUE;

}

void CWorldLog::SendEventToMLS(DWORD dwMsgID, WORD wMsgType, char* pData, DWORD dwMsgSize, int iMainH)
{
	int iRet, i;
	DWORD* dwp;
	char* cp;
	WORD* wp;

	if (iMainH == -1) {
		for (i = 0; i < DEF_MAXMAINLOGSOCK; i++) {
			if (m_pMainLogSock[m_iCurMainLogSockIndex] != NULL) break;
			m_iCurMainLogSockIndex++;
			if (m_iCurMainLogSockIndex >= 10) m_iCurMainLogSockIndex = 0;
		}
	}
	else m_iCurMainLogSockIndex = iMainH;

	ZeroMemory(G_cData50000, sizeof(G_cData50000));

	dwp = (DWORD*)(G_cData50000 + DEF_INDEX4_MSGID);
	*dwp = dwMsgID;
	wp = (WORD*)(G_cData50000 + DEF_INDEX2_MSGTYPE);
	*wp = wMsgType;

	cp = (char*)(G_cData50000 + DEF_INDEX2_MSGTYPE + 2);

	memcpy((char*)cp, pData, dwMsgSize);

	if ((m_bisMainRegistered[m_iCurMainLogSockIndex] == TRUE) || (iMainH != -1)) {
		iRet = m_pMainLogSock[m_iCurMainLogSockIndex]->iSendMsg(G_cData50000, dwMsgSize + 6, DEF_USE_ENCRYPTION);
		switch (iRet) {
		case DEF_XSOCKEVENT_QUENEFULL:
		case DEF_XSOCKEVENT_SOCKETERROR:
		case DEF_XSOCKEVENT_CRITICALERROR:
		case DEF_XSOCKEVENT_SOCKETCLOSED:
			wsprintf(G_cTxt, "(X) Main-log-socket(%d) send error!", m_iCurMainLogSockIndex);
			PutLogList(G_cTxt);
			delete m_pMainLogSock[m_iCurMainLogSockIndex];
			m_pMainLogSock[m_iCurMainLogSockIndex] = NULL;
			m_bisMainRegistered[m_iCurMainLogSockIndex] = FALSE;
			m_iActiveMainLogSock--;
			m_pMainLogSock[m_iCurMainLogSockIndex] = new class XSocket(m_hWnd, DEF_SERVERSOCKETBLOCKLIMIT);
			m_pMainLogSock[m_iCurMainLogSockIndex]->bConnect(m_cMainLogAddress, m_iMainLogPort, (WM_ONMAINLOGSOCKETEVENT + m_iCurMainLogSockIndex));
			m_pMainLogSock[m_iCurMainLogSockIndex]->bInitBufferSize(DEF_MSGBUFFERSIZE);
			wsprintf(G_cTxt, "(!) Try to reconnect Main-log-socket(%d)... Addr:%s  Port:%d", m_iCurMainLogSockIndex, m_cMainLogAddress, m_iMainLogPort);
			PutLogList(G_cTxt);
			break;
		}
	}
	else {
		wsprintf(G_cTxt, "(X) No MLS-Socket Available!  MsgID(%x)", dwMsgID);
		PutLogList(G_cTxt);
		return;
	}
}

void CWorldLog::RegisterWorldServer(int iMainH)
{
 char cData[100];
 DWORD * dwp;
 char *cp;

	ZeroMemory(cData, sizeof(cData));

	cp = (char *)cData;
	memcpy(cp, m_cWorldLogName, 30);
	cp += 30;

	memcpy(cp, m_cWorldServerAddress, 16);
	cp += 16;
	
	dwp = (DWORD *)cp;
	*dwp = m_iWorldServerPort;
	cp += 4;
	
	SendEventToMLS(MSGID_REGISTER_WORLDSERVER, DEF_MSGTYPE_CONFIRM, cData, 50, iMainH);

	PutLogList(" ");

	wsprintf(G_cTxt, "(!) Try to register world server(%s) to MLS...", m_cWorldLogName);
	PutLogList(G_cTxt);
}

void CWorldLog::RegisterWorldServerSocket(int iMainH)
{
 char cData[100];
 DWORD * dwp;
 char *cp;

	ZeroMemory(cData, sizeof(cData));

	cp = (char *)cData;
	memcpy(cp, m_cWorldLogName, 30);
	cp += 30;

	memcpy(cp, m_cWorldServerAddress, 16);
	cp += 16;
	
	dwp = (DWORD *)cp;
	*dwp = m_iWorldServerPort;
	cp += 4;

	SendEventToMLS(MSGID_REGISTER_WORLDSERVERSOCKET, DEF_MSGTYPE_CONFIRM, cData, 50, iMainH);

	wsprintf(G_cTxt, "(!) Try to register world server socket(%d) to MLS...", iMainH);

	PutLogList(G_cTxt);
}

void CWorldLog::CharInfoList(int iClientH, char *pData)
{
 short sAppr1, sAppr2, sAppr3, sAppr4;
 int *ip, iSpace1, iSpace2, i, iLevel, iStr, iVit, iInt, iDex, iMag, iCharisma, iExp, iApprColor, iSaveYear, iSaveMonth, iSaveDay, iSaveHour, iSaveMinute;
 char  * cp, * cp2, cData[2000], cAccountName[11], cInfo[110], cMapName[256], cCharName[11], cSex, cSkin, cTotalChar;
 DWORD *dwp, dwCharID;
 WORD *wp;

	ZeroMemory(cData, sizeof(cData));
	ZeroMemory(cAccountName, sizeof(cAccountName));
	ZeroMemory(cInfo, sizeof(cInfo));

	cp2 = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
	
	memcpy(cAccountName, cp2, 10);

	dwp = (DWORD *)(pData + 16);
	dwCharID = *dwp;

	wsprintf(G_cTxt, "(TestLog) Getting character data in account(%s)-ID(%d)...", cAccountName, dwCharID);
	PutLogList(G_cTxt);

	memcpy(cData, cAccountName, 10);

	dwp  = (DWORD *)(cData + 10);
	*dwp = dwCharID;

	dwp  = (DWORD *)(cData + 14);
	*dwp = MSGID_RESPONSE_CHARACTERLOG;

	wp   = (WORD *)(cData + 18);
	*wp  = DEF_MSGTYPE_CONFIRM;

	cp  = (char *)(cData + 20);
	cp2 = (char *)(pData + 20);
	memcpy(cp, cp2, 18);
	cp += 18;

	cp2 = (char *)(pData + 37);
	cTotalChar = *cp2;
	cp2++;

	dwp = (DWORD *)cp2;
	iSpace1 = (int)dwp;
	cp2 += 4;

	dwp = (DWORD *)cp2;
	iSpace2 = (int)dwp;
	cp2 += 4;

	wsprintf(G_cTxt, "(TestLog) Total Char: %d", cTotalChar);
	PutLogList(G_cTxt);

	for (i = 1; i <= cTotalChar; i++) {
		ZeroMemory(cMapName, sizeof(cMapName));
		ZeroMemory(cCharName, sizeof(cCharName));

		memcpy(cCharName, cp2, 10);
		memcpy(&cInfo[i*11], cp2, 10);
		cp2 += 10;
		
		memcpy(cp, cCharName, 10);
		cp += 10;

		if ((iGetCharacterData(cCharName, cMapName, &sAppr1, &sAppr2, &sAppr3, &sAppr4, &iApprColor, &cSex, &cSkin, &iLevel, &iExp, &iStr, &iVit, &iDex, &iInt, &iMag, &iCharisma, &iSaveYear, &iSaveMonth, &iSaveDay, &iSaveHour, &iSaveMinute)) != TRUE) {
			*cp = 0;
			cp += 40;
			break;
		}

		*cp = 1;
		cp++;

		wp = (WORD *)cp;
		*wp = (int)sAppr1;
		cp += 2;

		wp = (WORD *)cp;
		*wp = (int)sAppr2;
		cp += 2;

		wp = (WORD *)cp;
		*wp = (int)sAppr3;
		cp += 2;

		wp = (WORD *)cp;
		*wp = (int)sAppr4;
		cp += 2;

		wp = (WORD *)cp;
		*wp = (int)cSex;
		cp += 2;

		wp = (WORD *)cp;
		*wp = (int)cSkin;
		cp += 2;

		wp = (WORD *)cp;
		*wp = (int)iLevel;
		cp += 2;

		dwp = (DWORD *)cp;
		*dwp = (int)iExp;
		cp += 4;

		wp = (WORD *)cp;
		*wp = (int)iStr;
		cp += 2;

		wp = (WORD *)cp;
		*wp = (int)iVit;
		cp += 2;

		wp = (WORD *)cp;
		*wp = (int)iDex;
		cp += 2;

		wp = (WORD *)cp;
		*wp = (int)iInt;
		cp += 2;

		wp = (WORD *)cp;
		*wp = (int)iMag;
		cp += 2;

		wp = (WORD *)cp;
		*wp = (int)iCharisma;
		cp += 2;

		ip = (int *)cp;
		*ip = iApprColor;
		cp += 4;

		wp = (WORD *)cp;
		*wp = (int)iSaveYear;
		cp += 2;
		
		wp = (WORD *)cp;
		*wp = (int)iSaveMonth;
		cp += 2;

		wp = (WORD *)cp;
		*wp = (int)iSaveDay;
		cp += 2;

		wp = (WORD *)cp;
		*wp = (int)iSaveHour;
		cp += 2;

		wp = (WORD *)cp;
		*wp = (int)iSaveMinute;
		cp += 2;
		
		memcpy(cp, cMapName, 10);
		cp += 10;
	}
	dwp = (DWORD *)cp;
	*dwp = (DWORD)iSpace1;
	cp += 4;

	dwp = (DWORD *)cp;
	*dwp = (DWORD)iSpace2;
	cp += 4;

	SendEventToMLS(MSGID_RESPONSE_CHARINFOLIST, DEF_MSGTYPE_CONFIRM, cData, (cTotalChar*65) + 46, -1);
}

int CWorldLog::iGetCharacterDatabaseID(char* cCharName)
{
	SACommand com;

	try
	{
		com.setConnection(&con);
		com.setCommandText("SELECT     [Character-ID] FROM         Characters WHERE     ([Character-Name] = :1)");
		com.Param(1).setAsString() = cCharName;

		com.Execute();

		if (com.isResultSet())
		{
			while (com.FetchNext())
			{
				if (com.Field("Character-ID").asLong() > 0)
					return com.Field("Character-ID").asLong();
			}
		}
		else
		{
			return -1;
		}
	}
	catch (SAException& x)
	{
		try
		{
			con.Rollback();
		}
		catch (SAException&)
		{
		}

		char cTemp[256];
		ZeroMemory(cTemp, sizeof(cTemp));
		wsprintf(cTemp, "(!!!) SQL SERVER ERROR (iGetCharacterDatabaseID): %s", (const char*)x.ErrText());
		PutLogList(cTemp);

		return -1;
	}
}

int CWorldLog::iGetAccountDatabaseID(char* cAccName)
{
	SACommand com;

	try
	{
		com.setConnection(&con);
		com.setCommandText("SELECT     [Account-ID] FROM         Accounts WHERE     ([Account-Name] = :1)");
		com.Param(1).setAsString() = cAccName;

		com.Execute();

		if (com.isResultSet())
		{
			while (com.FetchNext())
			{
				if (com.Field("Account-ID").asLong() > 0)
					return com.Field("Account-ID").asLong();
			}
		}
		else
		{
			return -1;
		}
	}
	catch (SAException& x)
	{
		try
		{
			con.Rollback();
		}
		catch (SAException&)
		{
		}

		char cTemp[256];
		ZeroMemory(cTemp, sizeof(cTemp));
		wsprintf(cTemp, "(!!!) SQL SERVER ERROR (iGetAccountDatabaseID): %s", (const char*)x.ErrText());
		PutLogList(cTemp);

		return -1;
	}
}

void CWorldLog::UpdateLastLoginTime(char* cCharacterName)
{
	SACommand com;

	try
	{
		com.setConnection(&con);
		com.setCommandText("UPDATE Characters SET [Character-Last-Login] = GETDATE() WHERE     ([Character-Name] = :1)");
		com.Param(1).setAsString() = cCharacterName;

		com.Execute();

	}
	catch (SAException& x)
	{
		try
		{
			con.Rollback();
		}
		catch (SAException&)
		{
		}

		char cTemp[256];
		ZeroMemory(cTemp, sizeof(cTemp));
		wsprintf(cTemp, "(!!!) SQL SERVER ERROR (UpdateLastLoginTime): %s", (const char*)x.ErrText());
		PutLogList(cTemp);
	}
}

int CWorldLog::iGetCharacterData(char * cCharName, char * cMapName, short * sAppr1, short * sAppr2, short * sAppr3, short * sAppr4, int * iApprColor, char * cSex, char * cSkin, int * iLevel, int * iExp, int * iStr, int * iVit, int * iDex, int * iInt, int * iMag, int * iCharisma, int * iSaveYear, int * iSaveMonth, int * iSaveDay, int * iSaveHour, int * iSaveMinute)
{
 DWORD dwFileSize, lpNumberOfBytesRead;
 char * cp, * token, cReadModeA, cReadModeB, cFileName[255], cDir[11];
 char seps[] = "= \t\n";
 class CStrTok * pStrTok;
 HANDLE hFile;
 int iCharacterDBid;

	*iApprColor = 0;
	if (iSaveYear != 0) *iSaveYear = 0;
	if (iSaveMonth != 0) *iSaveMonth = 0;
	if (iSaveDay != 0) *iSaveDay = 0;
	if (iSaveHour != 0) *iSaveHour = 0;
	if (iSaveMinute !=  0) *iSaveMinute = 0;
	if (G_bDBMode == TRUE) {
		SACommand com;

		try
		{
			com.setConnection(&con);
			com.setCommandText("SELECT     [Character-ID], [Account-ID], RTRIM([Character-Name]) AS 'Character-Name', [Character-Profile], [Character-Location], [Character-Guild-Name], [Character-Guild-GUID], [Character-Guild-Rank], [Character-Loc-Map], [Character-Loc-X], [Character-Loc-Y], [Character-HP], [Character-MP], [Character-SP], [Character-Level], [Character-Rating], [Character-Strength], [Character-Intelligence], [Character-Vitality], [Character-Dexterity], [Character-Magic], [Character-Charisma], [Character-Luck], [Character-Experience], [Character-LU-Pool], [Character-Ek-Count], [Character-Pk-Count], [Character-Reward-Gold], [Character-Down-Skill-Index], [Character-ID1], [Character-ID2], [Character-ID3], [Character-Sex], [Character-Skin], [Character-Hair-Style], [Character-Hair-Colour], [Character-Underwear], [Character-Hunger], [Character-Shutup-Time], [Character-Rating-Time], [Character-Force-Time], [Character-SP-Time], [Character-Admin-Level], [Character-Block-Date], [Character-Quest-Number], [Character-Quest-ID], [Character-Quest-Reward], [Character-Quest-Amount], [Character-Contribution], [Character-War-Contribution], [Character-Event-ID], [Character-Criticals], [Character-Fightzone-ID], [Character-Ability-Time], [Character-Lock-Map], [Character-Lock-Time], [Character-Crusade-Job], [Character-Crusade-GUID], [Character-Construct-Points], [Character-Death-Time], [Character-Party-ID], [Character-Majestics], [Character-Appr1], [Character-Appr2], [Character-Appr3], [Character-Appr4], [Character-Appr-Colour] FROM         Characters WHERE     ([Character-Name] = :1)");
			com.Param(1).setAsString() = cCharName;

			com.Execute();

			if (com.isResultSet())
			{
				long col_count = com.FieldCount();

				char cTest[256];
				char cTemp[120];

				while (com.FetchNext())
				{
					// db character ID
					ZeroMemory(cTemp, sizeof(cTemp));
					strcpy(cTemp, com.Field("Character-ID").asString());
					iCharacterDBid = atoi(cTemp);

					// char appr1
					ZeroMemory(cTemp, sizeof(cTemp));
					strcpy(cTemp, com.Field("Character-Appr1").asString());
					*sAppr1 = atoi(cTemp);

					// char appr2
					ZeroMemory(cTemp, sizeof(cTemp));
					strcpy(cTemp, com.Field("Character-Appr2").asString());
					*sAppr2 = atoi(cTemp);

					// char appr3
					ZeroMemory(cTemp, sizeof(cTemp));
					strcpy(cTemp, com.Field("Character-Appr3").asString());
					*sAppr3 = atoi(cTemp);

					// char appr1
					ZeroMemory(cTemp, sizeof(cTemp));
					strcpy(cTemp, com.Field("Character-Appr4").asString());
					*sAppr4 = atoi(cTemp);

					// char gender
					ZeroMemory(cTemp, sizeof(cTemp));
					strcpy(cTemp, com.Field("Character-Sex").asString());
					*cSex = atoi(cTemp);

					// char skin
					ZeroMemory(cTemp, sizeof(cTemp));
					strcpy(cTemp, com.Field("Character-Skin").asString());
					*cSkin = atoi(cTemp);

					// char level
					ZeroMemory(cTemp, sizeof(cTemp));
					strcpy(cTemp, com.Field("Character-Level").asString());
					*iLevel = atoi(cTemp);

					// char str
					ZeroMemory(cTemp, sizeof(cTemp));
					strcpy(cTemp, com.Field("Character-Strength").asString());
					*iStr = atoi(cTemp);

					// char dex
					ZeroMemory(cTemp, sizeof(cTemp));
					strcpy(cTemp, com.Field("Character-Dexterity").asString());
					*iDex = atoi(cTemp);

					// char vit
					ZeroMemory(cTemp, sizeof(cTemp));
					strcpy(cTemp, com.Field("Character-Vitality").asString());
					*iVit = atoi(cTemp);

					// char mag
					ZeroMemory(cTemp, sizeof(cTemp));
					strcpy(cTemp, com.Field("Character-Magic").asString());
					*iMag = atoi(cTemp);

					// char int
					ZeroMemory(cTemp, sizeof(cTemp));
					strcpy(cTemp, com.Field("Character-Intelligence").asString());
					*iInt = atoi(cTemp);

					// char charisma
					ZeroMemory(cTemp, sizeof(cTemp));
					strcpy(cTemp, com.Field("Character-Charisma").asString());
					*iCharisma = atoi(cTemp);

					// char exp
					ZeroMemory(cTemp, sizeof(cTemp));
					strcpy(cTemp, com.Field("Character-Experience").asString());
					*iExp = atoi(cTemp);

					// char appr colour
					ZeroMemory(cTemp, sizeof(cTemp));
					strcpy(cTemp, com.Field("Character-Appr-Colour").asString());
					*iApprColor = atoi(cTemp);

					// char map
					ZeroMemory(cTemp, sizeof(cTemp));
					strcpy(cTemp, com.Field("Character-Loc-Map").asString());
					strcpy(cMapName, cTemp);

					// char save year
					ZeroMemory(cTemp, sizeof(cTemp));
					strcpy(cTemp, "0");
					*iSaveYear = atoi(cTemp);

					// char save month
					ZeroMemory(cTemp, sizeof(cTemp));
					strcpy(cTemp, "0");
					*iSaveMonth = atoi(cTemp);

					// char save day
					ZeroMemory(cTemp, sizeof(cTemp));
					strcpy(cTemp, "0");
					*iSaveDay = atoi(cTemp);

					// char save hour
					ZeroMemory(cTemp, sizeof(cTemp));
					strcpy(cTemp, "0");
					*iSaveHour = atoi(cTemp);

					// char save minute
					ZeroMemory(cTemp, sizeof(cTemp));
					strcpy(cTemp, "0");
					*iSaveMinute = atoi(cTemp);

					com.Close();
					return TRUE;
				}
			}
			else
			{
				com.Close();
				return FALSE;
			}
		}
		catch (SAException& x)
		{
			try
			{
				con.Rollback();
			}
			catch (SAException&)
			{
			}

			char cTemp[256];
			ZeroMemory(cTemp, sizeof(cTemp));
			wsprintf(cTemp, "(!!!) SQL SERVER ERROR (iGetCharacterData): %s", (const char*)x.ErrText());
			PutLogList(cTemp);

			return FALSE;
		}
	}
	else {
		ZeroMemory(cFileName, sizeof(cFileName));
		strcat(cFileName, "Character");
		strcat(cFileName, "\\");
 		strcat(cFileName, "\\");
		wsprintf(cDir, "AscII%d", *cCharName);
		strcat(cFileName, cDir);
		strcat(cFileName, "\\");
 		strcat(cFileName, "\\");
		strcat(cFileName, cCharName);	
		strcat(cFileName, ".txt");
		wsprintf(G_cTxt, "(TestLog) Getting character file %s", cFileName);
		PutLogList(G_cTxt);

		hFile = CreateFileA(cFileName, GENERIC_READ, NULL, NULL, OPEN_EXISTING, NULL, NULL);
		if (hFile == INVALID_HANDLE_VALUE) return -1;
		dwFileSize = GetFileSize(hFile, NULL);
		cp = new char[dwFileSize+1];
		ZeroMemory(cp, dwFileSize+1);
		ReadFile(hFile, cp, dwFileSize, &lpNumberOfBytesRead, NULL);
		CloseHandle(hFile);
		pStrTok = new class CStrTok(cp, seps);
		cReadModeA = 0;
		token = pStrTok->pGet();
		while (token != NULL) {
			if (cReadModeA != 0) {
				switch (cReadModeA) {
				case 1: // appr1
					*sAppr1 = atoi(token);
					cReadModeA = 0;
					break;

				case 2: // appr2
					*sAppr2 = atoi(token);
					cReadModeA = 0;
					break;

				case 3: // appr3
					*sAppr3 = atoi(token);
					cReadModeA = 0;
					break;
					
				case 4: // appr4
					*sAppr4 = atoi(token);
					cReadModeA = 0;
					break;

				case 5: // sex-status
					*cSex = atoi(token);
					cReadModeA = 0;
					break;

				case 6: // skin-status
					*cSkin = atoi(token);
					cReadModeA = 0;
					break;

				case 7: // character-LEVEL
					*iLevel = atoi(token);
					cReadModeA = 0;
					break;

				case 8: // character-STR
					*iStr = atoi(token);
					cReadModeA = 0;
					break;

				case 9: // character-VIT
					*iVit = atoi(token);
					cReadModeA = 0;
					break;

				case 10: // character-DEX
					*iDex = atoi(token);
					cReadModeA = 0;
					break;

				case 11: // character-INT
					*iInt = atoi(token);
					cReadModeA = 0;
					break;

				case 12: // character-MAG
					*iMag = atoi(token);
					cReadModeA = 0;
					break;

				case 13: // character-CHARISMA
					*iCharisma = atoi(token);
					cReadModeA = 0;
					break;

				case 14: // character-EXP
					*iExp = atoi(token);
					cReadModeA = 0;
					break;

				case 15: // appr-color
					*iApprColor = atoi(token);
					cReadModeA = 0;
					break;
					
				case 16: // file-saved-date
					switch (cReadModeB) {
					case 1:
						if (iSaveYear != 0) *iSaveYear = atoi(token);
						cReadModeB = 2;
						break;

					case 2:
						if (iSaveMonth != 0) *iSaveMonth = atoi(token);
						cReadModeB = 3;
						break;

					case 3:
						if (iSaveDay != 0) *iSaveDay = atoi(token);
						cReadModeB = 4;
						break;

					case 4:
						if (iSaveHour != 0) *iSaveHour = atoi(token);
						cReadModeB = 5;
						break;

					case 5:
						if (iSaveMinute != 0) *iSaveMinute = atoi(token);
						cReadModeA = 0;
						cReadModeB = 0;
						break;
					}
					break;

				case 17: // character-loc-map
					strcpy(cMapName, token);
					cReadModeA = 0;
					break;
				}
			}	
			else {
				if (memcmp(token, "appr1", 5) == 0)					cReadModeA = 1;
				if (memcmp(token, "appr2", 5) == 0)					cReadModeA = 2;
				if (memcmp(token, "appr3", 5) == 0)					cReadModeA = 3;
				if (memcmp(token, "appr4", 5) == 0)					cReadModeA = 4;
				if (memcmp(token, "sex-status", 10) == 0)			cReadModeA = 5;
				if (memcmp(token, "skin-status", 11) == 0)			cReadModeA = 6;
				if (memcmp(token, "character-LEVEL", 15) == 0)		cReadModeA = 7;
				if (memcmp(token, "character-STR", 13) == 0)		cReadModeA = 8;
				if (memcmp(token, "character-VIT", 13) == 0)		cReadModeA = 9;
				if (memcmp(token, "character-DEX", 13) == 0)		cReadModeA = 10;
				if (memcmp(token, "character-INT", 13) == 0)		cReadModeA = 11;
				if (memcmp(token, "character-MAG", 13) == 0)		cReadModeA = 12;
				if (memcmp(token, "character-CHARISMA", 18) == 0)	cReadModeA = 13;
				if (memcmp(token, "character-EXP", 13) == 0)		cReadModeA = 14;
				if (memcmp(token, "appr-color", 10) == 0)			cReadModeA = 15;
				if (memcmp(token, "file-saved-date:", 16) == 0) {	
					cReadModeA = 16; 
					cReadModeB = 1;
				}
				if (memcmp(token, "character-loc-map", 17) == 0)	cReadModeA = 17;
			}
			token = pStrTok->pGet();
		}
		delete pStrTok;
		delete cp;
	}
	return TRUE;
}

/*
void CWorldLog::RequestSavePlayerData(int iClientH, char *pData, DWORD dwMsgSize, BOOL bVar1, BOOL bVar2)
{
 char cAccountName[11], cCharName[11], cAccountPassword[11], cData[256];
 char *cp;
 DWORD *dwp;
 WORD *wp;
 int iRet;
 FILE * pFile;
 char cFileName[255];
 char cDir[63];
 HANDLE hFile;
 DWORD  dwFileSize, dwCharID;
 BOOL bFlag;

	if (m_pClientList[iClientH] == NULL) return;
	ZeroMemory(cAccountName, sizeof(cAccountName));
	ZeroMemory(cCharName, sizeof(cCharName));
	ZeroMemory(cAccountPassword, sizeof(cAccountPassword));
	ZeroMemory(cFileName, sizeof(cFileName));
	ZeroMemory(cDir, sizeof(cDir));

	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

	memcpy(cCharName, cp, 10);
	cp += 10;

	memcpy(cAccountName, cp, 10);
	cp += 10;

	memcpy(cAccountPassword, cp, 10);
	cp += 10;

	if (G_bDBMode == TRUE) {
		dwp = (DWORD *)cp;
		dwCharID = *dwp;
		cp += 4;
	}
		
	bFlag = (BOOL)*cp;
	cp++;
	
	if ((dwMsgSize - 40) <= 0) {
		PutLogList("(X) Character data body empty: Cannot create & save data file.");
		return;
	}
	if (G_bDBMode == TRUE) {
		// todo: add SQL Code Here
		//iSaveCharacterSQL(dwCharID, cp-40)
	}
	else {
		strcat(cFileName, "Character");
		strcat(cFileName, "\\");
 		strcat(cFileName, "\\");
		wsprintf(cDir, "AscII%d", *cCharName);
		strcat(cFileName, cDir);
		strcat(cFileName, "\\");
		strcat(cFileName, "\\");
		strcat(cFileName, cCharName);	
		strcat(cFileName, ".txt");

		hFile = CreateFileA(cFileName, GENERIC_READ, NULL, NULL, OPEN_EXISTING, NULL, NULL);
		dwFileSize = GetFileSize(hFile, NULL);
		if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);

		pFile = fopen(cFileName, "wt");
		if (pFile == NULL) {
			wsprintf(G_cTxt, "(X) Cannot create character data file : Name(%s)", cCharName);
			PutLogList(G_cTxt);
		}
		else {
			wsprintf(G_cTxt, "(O) Character data file saved : Name(%s)", cCharName);
			PutLogList(G_cTxt);
			fwrite(cp, 1, strlen(cp), pFile);
			if (pFile != NULL) fclose(pFile);
		}
	}
	if ((bVar1 == TRUE) && (bFlag == 1)) {
		ZeroMemory(cData, sizeof(cData));

		cp = (char *)cData;
		memcpy(cp, cAccountName, 10);
		cp += 10;

		wsprintf(G_cTxt, "(TestLog) Account(%s) log out request to MLS...", cAccountName);
		PutLogList(G_cTxt);

		SendEventToMLS(MSGID_REQUEST_CLEARACCOUNTSTATUS, DEF_MSGTYPE_CONFIRM, cData, 10, -1);
		OnPlayerAccountMessage(DEF_MSGACCOUNTSET_NULL, cAccountName, 0, 0, 0);
	}
	if (bVar2 == TRUE) {
		ZeroMemory(cData, sizeof(cData));
	
		dwp = (DWORD *)(cData);
		*dwp = MSGID_RESPONSE_SAVEPLAYERDATA_REPLY;
		
		wp  = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
		*wp = DEF_MSGTYPE_CONFIRM;
		
		cp  = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);
		memcpy(cp, cCharName, 10);
		cp += 10;

		iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 16, DEF_USE_ENCRYPTION);

		switch(iRet) {
		case DEF_XSOCKEVENT_QUENEFULL:
		case DEF_XSOCKEVENT_SOCKETERROR:
		case DEF_XSOCKEVENT_CRITICALERROR:
		case DEF_XSOCKEVENT_SOCKETCLOSED:
			DeleteClient(iClientH, 0);
			delete m_pClientList[iClientH];
			m_pClientList[iClientH] = NULL;
			return;
		}
	}
}
*/
void CWorldLog::RequestSavePlayerData(int iClientH, char* pData, DWORD dwMsgSize, BOOL bVar1, BOOL bVar2)
{
	char cAccountName[11], cCharName[11], cAccountPassword[11], cData[256], cTemp[25000], cProfile[31], cLocation[11], cGuildName[21];
	char cMapName[11], cMagicM[101], cItemX[201], cItemY[201], cItemName[21], cEquipStatus[51];
	int iGuildGUID, iGuildRank, isX, isY, iHP, iMP, iSP, iLevel, iRating, iStr, iInt, iDex, iVit, iMag, iCharisma, iExp, iLU_Pool;
	int iEK, iPK, iHunger, iShutUp, iRatingTime, iForceRecall, iContribution, iCrits, iAbilityTime, iMajestics, iApprColour;
	int iRewardGold, i, iNumItems, iNumBankItems, iWarContribution, iCrusadeDuty, iConstructPts;
	short sSex, sSkin, sHairStyle, sHairColour, sUnderwear, sAppr1, sAppr2, sAppr3, sAppr4, sCharID1, sCharID2, sCharID3;

	char* pContents, * token;
	char   seps[] = "= \t\n";
	class  CStrTok* pStrTok;
	char   cReadModeA, cReadModeB;
	char* cp;
	DWORD* dwp;
	WORD* wp;
	int* ip, iRet, iCharDBID;
	short* sp;
	FILE* pFile;
	char cFileName[255];
	char cDir[63];
	HANDLE hFile;
	DWORD  dwFileSize, dwCharID, dwTime, dwCrusadeGUID;
	BOOL bFlag;
	char cCommand[25000], cValues[256];
	int iItemID, iItemNum1, iItemNum2, iItemNum3, iItemNum4, iItemNum5, iItemColour, iItemNum7, iItemNum8, iItemNum9, iItemNum10, iItemAttribute, iItemElement;


	if (m_pClientList[iClientH] == NULL) return;

	ZeroMemory(cAccountName, sizeof(cAccountName));
	ZeroMemory(cCharName, sizeof(cCharName));
	ZeroMemory(cAccountPassword, sizeof(cAccountPassword));
	ZeroMemory(cFileName, sizeof(cFileName));
	ZeroMemory(cDir, sizeof(cDir));
	ZeroMemory(cProfile, sizeof(cProfile));
	ZeroMemory(cLocation, sizeof(cLocation));
	ZeroMemory(cGuildName, sizeof(cGuildName));
	ZeroMemory(cMapName, sizeof(cMapName));
	ZeroMemory(cMagicM, sizeof(cMagicM));
	ZeroMemory(cItemX, sizeof(cItemX));
	ZeroMemory(cItemY, sizeof(cItemY));

	dwTime = timeGetTime();

	cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);

	memcpy(cCharName, cp, 10);
	cp += 10;

	memcpy(cAccountName, cp, 10);
	cp += 10;

	memcpy(cAccountPassword, cp, 10);
	cp += 10;

	ip = (int*)cp;
	iCharDBID = *ip;
	cp += 4;

	if (iCharDBID < 0 || iCharDBID > 100000) return;

	bFlag = (BOOL)*cp;
	cp++;

	memcpy(cProfile, cp, 30);
	cp += 30;

	memcpy(cLocation, cp, 10);
	cp += 10;

	memcpy(cGuildName, cp, 20);
	cp += 20;

	ip = (int*)cp;
	iGuildGUID = *ip;
	cp += 4;

	ip = (int*)cp;
	iGuildRank = *ip;
	cp += 4;

	sp = (short*)cp;
	sCharID1 = *sp;
	cp += 2;

	sp = (short*)cp;
	sCharID2 = *sp;
	cp += 2;

	sp = (short*)cp;
	sCharID3 = *sp;
	cp += 2;

	memcpy(cMapName, cp, 10);
	cp += 10;

	ip = (int*)cp;
	isX = *ip;
	cp += 4;

	ip = (int*)cp;
	isY = *ip;
	cp += 4;

	ip = (int*)cp;
	iHP = *ip;
	cp += 4;

	ip = (int*)cp;
	iMP = *ip;
	cp += 4;

	ip = (int*)cp;
	iSP = *ip;
	cp += 4;

	ip = (int*)cp;
	iLevel = *ip;
	cp += 4;

	ip = (int*)cp;
	iRating = *ip;
	cp += 4;

	ip = (int*)cp;
	iStr = *ip;
	cp += 4;

	ip = (int*)cp;
	iInt = *ip;
	cp += 4;

	ip = (int*)cp;
	iDex = *ip;
	cp += 4;

	ip = (int*)cp;
	iVit = *ip;
	cp += 4;

	ip = (int*)cp;
	iMag = *ip;
	cp += 4;

	ip = (int*)cp;
	iCharisma = *ip;
	cp += 4;

	ip = (int*)cp;
	iExp = *ip;
	cp += 4;

	ip = (int*)cp;
	iLU_Pool = *ip;
	cp += 4;

	ip = (int*)cp;
	iEK = *ip;
	cp += 4;

	ip = (int*)cp;
	iPK = *ip;
	cp += 4;

	ip = (int*)cp;
	iRewardGold = *ip;
	cp += 4;

	sp = (short*)cp;
	sSex = *sp;
	cp += 2;

	sp = (short*)cp;
	sSkin = *sp;
	cp += 2;

	sp = (short*)cp;
	sHairStyle = *sp;
	cp += 2;

	sp = (short*)cp;
	sHairColour = *sp;
	cp += 2;

	sp = (short*)cp;
	sUnderwear = *sp;
	cp += 2;

	ip = (int*)cp;
	iHunger = *ip;
	cp += 4;

	ip = (int*)cp;
	iShutUp = *ip;
	cp += 4;

	ip = (int*)cp;
	iRatingTime = *ip;
	cp += 4;

	ip = (int*)cp;
	iForceRecall = *ip;
	cp += 4;

	ip = (int*)cp;
	iContribution = *ip;
	cp += 4;

	ip = (int*)cp;
	iCrits = *ip;
	cp += 4;

	ip = (int*)cp;
	iAbilityTime = *ip;
	cp += 4;

	ip = (int*)cp;
	iMajestics = *ip;
	cp += 4;

	ip = (int*)cp;
	iCrits = *ip;
	cp += 4;

	sp = (short*)cp;
	sAppr1 = *sp;
	cp += 2;

	sp = (short*)cp;
	sAppr2 = *sp;
	cp += 2;

	sp = (short*)cp;
	sAppr3 = *sp;
	cp += 2;

	sp = (short*)cp;
	sAppr4 = *sp;
	cp += 2;

	ip = (int*)cp;
	iApprColour = *ip;
	cp += 4;

	ip = (int*)cp;
	iWarContribution = *ip;
	cp += 4;

	ip = (int*)cp;
	iCrusadeDuty = *ip;
	cp += 4;

	dwp = (DWORD*)cp;
	dwCrusadeGUID = *dwp;
	cp += 4;

	ip = (int*)cp;
	iConstructPts = *ip;
	cp += 4;

	memcpy(cMagicM, cp, 100);
	cp += 100;

	memcpy(cItemX, cp, 200);
	cp += 200;

	memcpy(cItemY, cp, 200);
	cp += 200;

	memcpy(cEquipStatus, cp, 50);
	cp += 50;

	ip = (int*)cp;
	iNumItems = *ip;
	cp += 4;

	ip = (int*)cp;
	iNumBankItems = *ip;
	cp += 4;

	if ((dwMsgSize - 40) <= 0) {
		PutLogList("(X) Character data body empty: Cannot create & save data file.");
		return;
	}
	if (G_bDBMode == TRUE) {

		//iCharDBID = iGetCharacterDatabaseID(cCharName);
		int iItems = 1, iBItems = 1;

		char cMsgTemp[120];
		ZeroMemory(cMsgTemp, sizeof(cMsgTemp));
		wsprintf(cMsgTemp, "Char ID: %d", iCharDBID);
		PutLogList(cMsgTemp);

		SACommand com;
		SACommand com2;
		SACommand com3;
		SACommand com4;
		SACommand com5;

		try
		{
			com3.setConnection(&con);
			com3.setCommandText("DELETE FROM CharItems WHERE ([Character-ID] = :1)");
			com3.Param(1).setAsLong() = iCharDBID;

			com3.Execute();

			if (iNumItems > 0)
			{
				wsprintf(cCommand, "INSERT INTO CharItems ([Item-ID], [Character-ID], ItemName, ItemNum1, ItemNum2, ItemNum3, ItemNum4, ItemNum5, [Item-Colour], ItemNum7, ItemNum8, ItemNum9, ItemNum10, [Item-Attribute], [Item-Element]) ");


				for (i = 0; i < iNumItems; i++)
				{
					ZeroMemory(cItemName, sizeof(cItemName));
					memcpy(cItemName, cp, 20);
					cp += 20;

					iItemID = i + 1;

					ip = (int*)cp;
					iItemNum1 = *ip;
					cp += 4;

					ip = (int*)cp;
					iItemNum2 = *ip;
					cp += 4;

					ip = (int*)cp;
					iItemNum3 = *ip;
					cp += 4;

					ip = (int*)cp;
					iItemNum4 = *ip;
					cp += 4;

					ip = (int*)cp;
					iItemNum5 = *ip;
					cp += 4;

					ip = (int*)cp;
					iItemColour = *ip;
					cp += 4;

					ip = (int*)cp;
					iItemNum7 = *ip;
					cp += 4;

					ip = (int*)cp;
					iItemNum8 = *ip;
					cp += 4;

					ip = (int*)cp;
					iItemNum9 = *ip;
					cp += 4;

					ip = (int*)cp;
					iItemNum10 = *ip;
					cp += 4;

					ip = (int*)cp;
					iItemAttribute = *ip;
					cp += 4;

					ip = (int*)cp;
					iItemElement = *ip;
					cp += 4;

					wsprintf(cValues, " SELECT %d, %d, '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d ",
						iItemID, iCharDBID, cItemName, iItemNum1, iItemNum2, iItemNum3,
						iItemNum4, iItemNum5, iItemColour, iItemNum7, iItemNum8, iItemNum9,
						iItemNum10, iItemAttribute, iItemElement);

					strcat(cCommand, cValues);

					if (i == iNumItems - 1) {}// strcat(cCommand, "GO");
					else strcat(cCommand, "UNION ALL");
				}

				com2.setConnection(&con);
				com2.setCommandText(cCommand);
				com2.Execute();
			}

			com5.setConnection(&con);
			com5.setCommandText("DELETE FROM BankItems WHERE ([Character-ID] = :1)");
			com5.Param(1).setAsLong() = iCharDBID;

			com5.Execute();

			ZeroMemory(cCommand, sizeof(cCommand));

			if (iNumBankItems > 0)
			{
				wsprintf(cCommand, "INSERT INTO BankItems ([Item-ID], [Character-ID], ItemName, ItemNum1, ItemNum2, ItemNum3, ItemNum4, ItemNum5, [Item-Colour], ItemNum7, ItemNum8, ItemNum9, ItemNum10, [Item-Attribute], [Item-Element]) ");


				for (i = 0; i < iNumBankItems; i++)
				{
					ZeroMemory(cItemName, sizeof(cItemName));
					memcpy(cItemName, cp, 20);
					cp += 20;

					iItemID = i + 1;

					ip = (int*)cp;
					iItemNum1 = *ip;
					cp += 4;

					ip = (int*)cp;
					iItemNum2 = *ip;
					cp += 4;

					ip = (int*)cp;
					iItemNum3 = *ip;
					cp += 4;

					ip = (int*)cp;
					iItemNum4 = *ip;
					cp += 4;

					ip = (int*)cp;
					iItemNum5 = *ip;
					cp += 4;

					ip = (int*)cp;
					iItemColour = *ip;
					cp += 4;

					ip = (int*)cp;
					iItemNum7 = *ip;
					cp += 4;

					ip = (int*)cp;
					iItemNum8 = *ip;
					cp += 4;

					ip = (int*)cp;
					iItemNum9 = *ip;
					cp += 4;

					ip = (int*)cp;
					iItemNum10 = *ip;
					cp += 4;

					ip = (int*)cp;
					iItemAttribute = *ip;
					cp += 4;

					ip = (int*)cp;
					iItemElement = *ip;
					cp += 4;

					wsprintf(cValues, " SELECT %d, %d, '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d ",
						iItemID, iCharDBID, cItemName, iItemNum1, iItemNum2, iItemNum3,
						iItemNum4, iItemNum5, iItemColour, iItemNum7, iItemNum8, iItemNum9,
						iItemNum10, iItemAttribute, iItemElement);

					strcat(cCommand, cValues);

					if (i == iNumBankItems - 1) {} //strcat(cCommand, ";");
					else strcat(cCommand, "UNION ALL");
				}

				com4.setConnection(&con);
				com4.setCommandText(cCommand);
				com4.Execute();
			}

			com.setConnection(&con);
			com.setCommandText("UPDATE Characters SET [Character-Construct-Points] = :54, [Character-Crusade-GUID] = :53, [Character-Crusade-Job] = :52, [Character-War-Contribution] = :51, [Character-ID3] = :50, [Character-ID2] = :49, [Character-ID1] = :48, [Character-Equip-Status] = :47, [Character-Item-Pos-Y] = :46, [Character-Item-Pos-X] = :45,  [Character-Magic-Mastery] = :44, [Character-Appr-Colour] = :43, [Character-Appr4] = :42, [Character-Appr3] = :41, [Character-Appr2] = :40, [Character-Appr1] = :39, [Character-Majestics] = :38, [Character-Ability-Time] = :37, [Character-Criticals] = :36, [Character-Contribution] = :35, [Character-Force-Time] = :34, [Character-Rating-Time] = :33, [Character-Shutup-Time] = :32, [Character-Hunger] = :31, [Character-Underwear] = :30, [Character-Hair-Colour] = :29, [Character-Hair-Style] = :28, [Character-Skin] = :27, [Character-Sex] = :26, [Character-Reward-Gold] = :25, [Character-PK-Count] = :24, [Character-EK-Count] = :23, [Character-LU-Pool] = :22, [Character-Experience] = :21, [Character-Charisma] = :20, [Character-Magic] = :19, [Character-Vitality] = :18, [Character-Dexterity] = :17, [Character-Intelligence] = :16, [Character-Strength] = :15, [Character-Rating] = :14, [Character-Level] = :13, [Character-SP] = :12, [Character-MP] = :11, [Character-HP] = :10, [Character-Loc-Y] = :9, [Character-Loc-X] = :8, [Character-Loc-Map] = :7, [Character-Guild-Rank] = :6, [Character-Guild-GUID] = :5, [Character-Guild-Name] = :4, [Character-Location] = :3, [Character-Profile] = :2 WHERE ([Character-Name] = :1)");
			com.Param(1).setAsString() = cCharName;
			com.Param(2).setAsString() = cProfile;
			com.Param(3).setAsString() = cLocation;
			com.Param(4).setAsString() = cGuildName;
			com.Param(5).setAsLong() = iGuildGUID;
			com.Param(6).setAsLong() = iGuildRank;
			com.Param(7).setAsString() = cMapName;
			com.Param(8).setAsLong() = isX;
			com.Param(9).setAsLong() = isY;
			com.Param(10).setAsLong() = iHP;
			com.Param(11).setAsLong() = iMP;
			com.Param(12).setAsLong() = iSP;
			com.Param(13).setAsLong() = iLevel;
			com.Param(14).setAsLong() = iRating;
			com.Param(15).setAsLong() = iStr;
			com.Param(16).setAsLong() = iInt;
			com.Param(17).setAsLong() = iDex;
			com.Param(18).setAsLong() = iVit;
			com.Param(19).setAsLong() = iMag;
			com.Param(20).setAsLong() = iCharisma;
			com.Param(21).setAsLong() = iExp;
			com.Param(22).setAsLong() = iLU_Pool;
			com.Param(23).setAsLong() = iEK;
			com.Param(24).setAsLong() = iPK;
			com.Param(25).setAsLong() = iRewardGold;
			com.Param(26).setAsLong() = sSex;
			com.Param(27).setAsLong() = sSkin;
			com.Param(28).setAsLong() = sHairStyle;
			com.Param(29).setAsLong() = sHairColour;
			com.Param(30).setAsLong() = sUnderwear;
			com.Param(31).setAsLong() = iHunger;
			com.Param(32).setAsLong() = iShutUp;
			com.Param(33).setAsLong() = iRatingTime;
			com.Param(34).setAsLong() = iForceRecall;
			com.Param(35).setAsLong() = iContribution;
			com.Param(36).setAsLong() = iCrits;
			com.Param(37).setAsLong() = iAbilityTime;
			com.Param(38).setAsLong() = iMajestics;
			com.Param(39).setAsLong() = sAppr1;
			com.Param(40).setAsLong() = sAppr2;
			com.Param(41).setAsLong() = sAppr3;
			com.Param(42).setAsLong() = sAppr4;
			com.Param(43).setAsLong() = iApprColour;
			com.Param(44).setAsString() = cMagicM;
			com.Param(45).setAsString() = cItemX;
			com.Param(46).setAsString() = cItemY;
			com.Param(47).setAsString() = cEquipStatus;
			com.Param(48).setAsLong() = sCharID1;
			com.Param(49).setAsLong() = sCharID2;
			com.Param(50).setAsLong() = sCharID3;
			com.Param(51).setAsLong() = iWarContribution;
			com.Param(52).setAsLong() = iCrusadeDuty;
			com.Param(53).setAsLong() = dwCrusadeGUID;
			com.Param(54).setAsLong() = iConstructPts;
			
			com.Execute();

			long affected = com.RowsAffected();

			ZeroMemory(cTemp, sizeof(cTemp));
			wsprintf(cTemp, "(!) Saved Player Data to SQL (%s)   %i ms", cCharName, (int)(timeGetTime() - dwTime));
			PutLogList(cTemp);
		}
		catch (SAException& x)
		{
			con.Rollback();

			ZeroMemory(cTemp, sizeof(cTemp));
			wsprintf(cTemp, "(!!!) SQL SERVER ERROR (RequestSavePlayerData(item)): %s", (const char*)x.ErrText());
			PutLogList(cTemp);
			return;
		}
	}
	else {
		strcat(cFileName, "Character");
		strcat(cFileName, "\\");
		strcat(cFileName, "\\");
		wsprintf(cDir, "AscII%d", *cCharName);
		strcat(cFileName, cDir);
		strcat(cFileName, "\\");
		strcat(cFileName, "\\");
		strcat(cFileName, cCharName);
		strcat(cFileName, ".txt");

		hFile = CreateFileA(cFileName, GENERIC_READ, NULL, NULL, OPEN_EXISTING, NULL, NULL);
		dwFileSize = GetFileSize(hFile, NULL);
		if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);

		pFile = fopen(cFileName, "wt");
		if (pFile == NULL) {
			wsprintf(G_cTxt, "(X) Cannot create character data file : Name(%s)", cCharName);
			PutLogList(G_cTxt);
		}
		else {
			wsprintf(G_cTxt, "(O) Character data file saved : Name(%s)", cCharName);
			PutLogList(G_cTxt);
			fwrite(cp, 1, strlen(cp), pFile);
			fclose(pFile);
		}
	}
	if ((bVar1 == TRUE) && (bFlag == 1)) {
		ZeroMemory(cData, sizeof(cData));

		cp = (char*)cData;
		memcpy(cp, cAccountName, 10);
		cp += 10;

		wsprintf(G_cTxt, "(TestLog) Account(%s) log out request to MLS...", cAccountName);
		PutLogList(G_cTxt);

		SendEventToMLS(MSGID_REQUEST_CLEARACCOUNTSTATUS, DEF_MSGTYPE_CONFIRM, cData, 10, -1);
		OnPlayerAccountMessage(DEF_MSGACCOUNTSET_NULL, cAccountName, 0, 0, 0);
	}
	if (bVar2 == TRUE) {
		ZeroMemory(cData, sizeof(cData));

		dwp = (DWORD*)(cData);
		*dwp = MSGID_RESPONSE_SAVEPLAYERDATA_REPLY;

		wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
		*wp = DEF_MSGTYPE_CONFIRM;

		cp = (char*)(cData + DEF_INDEX2_MSGTYPE + 2);
		memcpy(cp, cCharName, 10);
		cp += 10;

		iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 16, DEF_USE_ENCRYPTION);

		switch (iRet) {
		case DEF_XSOCKEVENT_QUENEFULL:
		case DEF_XSOCKEVENT_SOCKETERROR:
		case DEF_XSOCKEVENT_CRITICALERROR:
		case DEF_XSOCKEVENT_SOCKETCLOSED:
			DeleteClient(iClientH, 0);
			delete m_pClientList[iClientH];
			m_pClientList[iClientH] = NULL;
			return;
		}
	}
}

void CWorldLog::WriteOccupyFlagFile(char *cFn)
{
	// INCOMPLETE
	// hypnotoad: useless function? write flag is obsolete
/* HANDLE hFile;
 DWORD dwFileSize;
 char cTempFile[256];

	hFile = CreateFile(cFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, 0);
	if (hFile == INVALID_HANDLE_VALUE) return;
	dwFileSize = GetFileSize(hFile, NULL);
	CloseHandle(hFile);
	ZeroMemory(cTempFile, sizeof(cTempFile));
	strcpy(cTempFile, cFn);
	for (i = 0; i < (strlen(cTempFile)-1); i++) {
		ZeroMemory(cBuffer, sizeof(dwFileSize+1));
		pFile = fopen(cFn, "rt");
		fread(cBuffer, dwFileSize, 1, pFile);
		fclose(pFile);
		pFile = fopen(cTempFile, "wt");
		fwrite(cBuffer, dwFileSize, 1, pFile);
		fclose(pFile);
		delete cBuffer;
		return;
	}*/
}

void CWorldLog::UpdateNoticement(WPARAM wParam)
{
 int i;
	
	switch(wParam) {
	case VK_F2:
		for (i = 0; i < DEF_MAXGAMESERVERS; i++) {
			if (m_pGameList[i] != NULL) {
				bSendClientConfig(m_pGameList[i]->m_iTrackSock, "Noticement.txt");
			}
		}
		PutLogList("(!) Updating noticement contents(noticement.txt) to game servers...");
	}
}

void CWorldLog::EnterGame(int iClientH, char * pData)
{
 char cData[256], cCharNameCheck[11], cMapName[11], cCharacterName[11], cAccountName[11], cAccountPassword[11], cAddress[16];
 int iPort, iCode, iLevel, iRet;
 char * cp;
 WORD *wp;
 DWORD *dwp;
 int i;
 bool bflag;
 bflag = FALSE;

	if (m_pClientList[iClientH] == NULL) return;
	ZeroMemory(cData, sizeof(cData));
	ZeroMemory(cCharNameCheck, sizeof(cCharNameCheck));
	ZeroMemory(cMapName, sizeof(cMapName));
	ZeroMemory(cCharacterName, sizeof(cCharacterName));
	ZeroMemory(cAccountName, sizeof(cAccountName));
	ZeroMemory(cAccountPassword, sizeof(cAccountPassword));
	ZeroMemory(cAddress, sizeof(cAddress));

	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

	memcpy(cCharacterName, cp, 10);
	cp += 10;

	memcpy(cMapName, cp, 10);
	cp += 10;

	memcpy(cAccountName, cp, 10);
	cp += 10;

	memcpy(cAccountPassword, cp, 10);
	cp += 10;

	iCode = iGetMapInformation(cMapName, cAddress, &iPort, iClientH);
	wsprintf(G_cTxt, "(TestLog) ReqEnterGame: Accnt(%s) Pass(%s) Char(%s) Map(%s) Addr(%s) Port(%d) Code(%d)", cAccountName, cAccountPassword, cCharacterName, cMapName, cAddress, iPort, iCode);
	PutLogList(G_cTxt);
	//game char fix
		for (i = 0; i < DEF_MAXGAMESERVERSMAPS; i++)
		if((m_cMapList[i] != NULL) && (strcmp(m_cMapList[i]->m_cName, cMapName) == 0)) bflag = TRUE;
		for (i = 0; i < DEF_MAXPLAYERACCOUNTS; i++)
			if((m_pAccountList[i] != NULL) && (strcmp(m_pAccountList[i]->cAccountName, cAccountName) == 0)) m_pAccountList[i]->Timeout = 20;

	if (iCode == -1) {
		dwp  = (DWORD *)(cData);
		*dwp = MSGID_RESPONSE_ENTERGAME;
		wp   = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
		*wp  = DEF_ENTERGAMERESTYPE_REJECT;
		cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);
		*cp = 3;
		iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 7, DEF_USE_ENCRYPTION);

	if(bflag == FALSE) {
			wsprintf(G_cTxt, "(TestLog) ReqEnterGame Reject! Account(%s) status NULL", cAccountName);
			PutLogList(G_cTxt);
			SendEventToMLS(MSGID_REQUEST_CLEARACCOUNTSTATUS, DEF_MSGTYPE_CONFIRM, cAccountName, 10, -1);
			OnPlayerAccountMessage(DEF_MSGACCOUNTSET_NULL, cAccountName, NULL, NULL, NULL);
			delete m_pClientList[iClientH];
			m_pClientList[iClientH] = NULL;
			return;
		}
	}
	else if (iCode == -2) {
		dwp  = (DWORD *)(cData);
		*dwp = MSGID_RESPONSE_ENTERGAME;
		wp   = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
		*wp  = DEF_ENTERGAMERESTYPE_REJECT;
		cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);
		*cp = 5;
		iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 7, DEF_USE_ENCRYPTION);
		
	if(bflag == FALSE) {
			wsprintf(G_cTxt, "(TestLog) ReqEnterGame Reject! Account(%s) status NULL", cAccountName);
			PutLogList(G_cTxt);
			SendEventToMLS(MSGID_REQUEST_CLEARACCOUNTSTATUS, DEF_MSGTYPE_CONFIRM, cAccountName, 10, -1);
			OnPlayerAccountMessage(DEF_MSGACCOUNTSET_NULL, cAccountName, NULL, NULL, NULL);
			delete m_pClientList[iClientH];
			m_pClientList[iClientH] = NULL;
			return;
		}
	}
	else {
		if (iGetCharacterInformation(cAccountName, cCharNameCheck, &iLevel) == FALSE) {
			wsprintf(G_cTxt, "(TestLog) ReqEnterGame Reject! Account(%s) status NULL", cAccountName);
			PutLogList(G_cTxt);
			SendEventToMLS(MSGID_REQUEST_CLEARACCOUNTSTATUS, DEF_MSGTYPE_CONFIRM, cAccountName, 10, -1);
			OnPlayerAccountMessage(DEF_MSGACCOUNTSET_NULL, cAccountName, NULL, NULL, NULL);
			delete m_pClientList[iClientH];
			m_pClientList[iClientH] = NULL;
			return;
		}
		dwp  = (DWORD *)(cData);
		*dwp = MSGID_RESPONSE_ENTERGAME;
		wp   = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
		*wp  = DEF_ENTERGAMERESTYPE_CONFIRM;
	
		cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);
		
		memcpy(cp, cAddress, 16);
		cp += 16;

		wp = (WORD *)cp;
		*wp = iPort;
		cp += 2;

		iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 24, DEF_USE_ENCRYPTION);
	}
	switch(iRet) {
	case DEF_XSOCKEVENT_QUENEFULL:
	case DEF_XSOCKEVENT_SOCKETERROR:
	case DEF_XSOCKEVENT_CRITICALERROR:
	case DEF_XSOCKEVENT_SOCKETCLOSED:
		delete m_pClientList[iClientH];
		m_pClientList[iClientH] = NULL;
		return;
	}
}

int CWorldLog::iGetMapInformation(char *cMapName, char * cAddress, int * iPort, int iClientH)
{
 int i, iIndex;

	for (i = 0; i < DEF_MAXGAMESERVERSMAPS; i++) {
		if ((m_cMapList[i] != NULL) && (strcmp(m_cMapList[i]->m_cName, cMapName) == 0)) {
			iIndex = m_cMapList[i]->iIndex;
			if (m_pGameList[iIndex] != NULL) {
				ZeroMemory(cAddress, sizeof(cAddress));
				strcpy(cAddress, m_pGameList[iIndex]->m_cGameServerAddress);
				if(memcmp(m_pClientList[iClientH]->m_cIPaddress, m_pGameList[iIndex]->m_cGameServerAddress, strlen(m_pClientList[iClientH]->m_cIPaddress)) == 0) strcpy(cAddress, m_pGameList[iIndex]->m_cGameServerLanAddress);
				*iPort = m_pGameList[iIndex]->m_iGameServerPort;
				return 0;
			}
			return -1;
		}
	}
	return -2;
}

void CWorldLog::RequestNoSaveLogout(int iClientH, char *pData)
{
 char *cp, cTotalChar;
 char cAccountName[11], cCharacterName[11], cGameServerName[11], cData[256];

	if (m_pClientList[iClientH] == NULL) return;
	ZeroMemory(cAccountName, sizeof(cAccountName));
	ZeroMemory(cCharacterName, sizeof(cCharacterName));
	ZeroMemory(cGameServerName, sizeof(cGameServerName));

	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

	memcpy(cCharacterName, cp, 10);
	cp += 10;

	memcpy(cAccountName, cp, 10);
	cp += 10;

	memcpy(cGameServerName, cp, 10);
	cp += 10;

	cTotalChar = *cp;
	cp++;

	if (cTotalChar == 1) {
		cp  = (char *)(cData);
		memcpy(cp, cAccountName, 10);
		cp += 10;

		wsprintf(G_cTxt, "(TestLog) Account(%s) log out request to MLS...", cAccountName);
		PutLogList(G_cTxt);

		SendEventToMLS(MSGID_REQUEST_CLEARACCOUNTSTATUS, DEF_MSGTYPE_CONFIRM, cData, 10, -1);
		OnPlayerAccountMessage(0, cAccountName, 0, 0, 0);
	}
}

/*
void CWorldLog::RequestCreateNewCharacter(int iClientH, char *pData)
{
 char cData[3000], cFileName[255], cCharacterName[111], cTotalCharNames[111], cNewCharName[11], cAccountName[11], cPassword[11], cUnknown1[31], cTxt[11], cTxt2[120], cMapName[11], cDir[11];
 char *cp;
 DWORD *dwp, dwCharID;
 WORD *wp;
 short sAppr1, sAppr2, sAppr3, sAppr4;
 char cSex, cSkin, cTotalChar, cNewGender, cNewSkin, cNewStr, cNewVit, cNewDex, cNewInt, cNewMag, cNewChr; 
 int iTempAppr2, iTempAppr3, iTempAppr1, i, iLevel, iStr, iVit, iInt, iDex, iMag, iCharisma, iExp, iApprColor, iSaveYear, iSaveMonth, iSaveDay, iSaveHour, iSaveMinute;
 int *ip;
 FILE * pFile;

	pFile = 0;
	sAppr1 = 0;
	sAppr2 = 0;
	sAppr3 = 0;
	sAppr4 = 0;
	ZeroMemory(cData, sizeof(cData));
	ZeroMemory(cFileName, sizeof(cFileName));
	ZeroMemory(cCharacterName, sizeof(cCharacterName));
	ZeroMemory(cNewCharName, sizeof(cNewCharName));
	ZeroMemory(cAccountName, sizeof(cAccountName));
	ZeroMemory(cPassword, sizeof(cPassword));
	ZeroMemory(cDir, sizeof(cDir));
	ZeroMemory(cTxt, sizeof(cTxt));
	ZeroMemory(cTotalCharNames, sizeof(cTotalCharNames));
	ZeroMemory(cUnknown1, sizeof(cUnknown1));	
	ZeroMemory(cTxt2, sizeof(cTxt2));

	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
	memcpy(cNewCharName, cp, 10);
	cp += 10;

	memcpy(cAccountName, cp, 10);
	cp += 10;

	memcpy(cPassword, cp, 10);
	cp += 10;

	memcpy(cUnknown1, cp, 30);
	cp += 30;

	wsprintf(G_cTxt, "(TestLog) Request create new character(%s). Account(%s) Password(%s) OtherChars(%s)", cNewCharName, cAccountName, cPassword, cUnknown1);
	PutLogList(G_cTxt);

	cNewGender = *cp;
	cp++;

	cNewSkin = *cp;
	cp++; 

	iTempAppr2 = *cp;
	cp++;

	iTempAppr3 = *cp;
	cp++;

	iTempAppr1 = *cp;
	cp++;

	cNewStr = *cp;
	cp++;

	cNewVit = *cp;
	cp++;

	cNewDex = *cp;
	cp++;

	cNewInt = *cp;
	cp++;

	cNewMag = *cp;
	cp++;

	cNewChr = *cp;
	cp++;
 
	dwp = (DWORD *)cp;
	dwCharID = *dwp;
	cp += 4;

	cTotalChar = (char)*cp;
	cp++;

	wsprintf(G_cTxt, "CreateNewChar:(%s) Account:(%s) Str(%d)Int(%d)Vit(%d)Dex(%d)Mag(%d)Chr(%d)Gender(%d)", cNewCharName, cAccountName, cNewStr, cNewInt, cNewVit, cNewDex, cNewMag, cNewChr, cNewGender-1);
	PutLogList(G_cTxt);

	memcpy(cTotalCharNames, cp, cTotalChar*11);
	cp += cTotalChar*11;

	sAppr1 = 0;
	sAppr1 = sAppr1 | iTempAppr1;
	sAppr1 = sAppr1 | (iTempAppr2 << 8);
	sAppr1 = sAppr1 | (iTempAppr3 << 4);

	if (G_bDBMode == TRUE) {
		// todo: add SQL Code Here
	
	}
	else {
		strcat(cFileName, "Character");
		strcat(cFileName, "\\");
		strcat(cFileName, "\\");
		wsprintf(cTxt, "AscII%d", *cNewCharName);
		strcat(cFileName, cTxt);
		strcpy(cDir, cFileName);
		strcat(cFileName,"\\");
		strcat(cFileName,"\\");					  
		strcat(cFileName, cNewCharName);
		strcat(cFileName, ".txt");

		_mkdir("Character");
		_mkdir(cDir);

		ZeroMemory(cData, sizeof(cData));
		if (fopen(cFileName, "rt") != NULL) {
		
			wsprintf(G_cTxt, "(X) Cannot create new character - Already existing character(%s)", cFileName);
			PutLogList(G_cTxt);
					
			cp		= (char *)(cData);
			memcpy(cp, cAccountName, 10);
			cp += 10;

			dwp     = (DWORD *)(cData + 10);
			*dwp    = dwCharID;
			dwp		= (DWORD *)(cData + 14);
			*dwp	= MSGID_RESPONSE_CHARACTERLOG;
			wp		= (WORD *)(cData + 18);
			*wp		= DEF_LOGRESMSGTYPE_ALREADYEXISTINGCHARACTER;

			SendEventToMLS(MSGID_RESPONSE_CHARACTERLOG, DEF_LOGRESMSGTYPE_ALREADYEXISTINGCHARACTER, cData, 20, -1);
			if (pFile != 0) fclose(pFile);
			return;
		}
		pFile = fopen(cFileName, "wt");
		if (pFile == NULL) {
			wsprintf(cTxt, "(!) Cannot create new character - cannot create file : Name(%s)", cFileName);
			PutLogList(cTxt);

			cp		= (char *)(cData);
			memcpy(cp, cAccountName, 10);
			cp += 10;

			dwp     = (DWORD *)(cData + 10);
			*dwp    = dwCharID;
			dwp		= (DWORD *)(cData + 14);
			*dwp	= MSGID_RESPONSE_CHARACTERLOG;
			wp		= (WORD *)(cData + 18);
			*wp     = DEF_LOGRESMSGTYPE_NEWCHARACTERFAILED;
		}
		else {
			CreateNewCharacterFile(pData, cFileName, cNewCharName, cAccountName, cPassword, cNewGender, cNewSkin, iTempAppr1, iTempAppr2, iTempAppr3, cNewStr, cNewVit, cNewDex, cNewInt, cNewMag, cNewChr, sAppr1, sAppr2, sAppr3, sAppr4);
			fwrite(pData, 1, strlen(pData), pFile);
			if (pFile != NULL) fclose(pFile);
		}
		
		memcpy(&cTotalCharNames[cTotalChar*11], cNewCharName, 10);
		cTotalChar += 1;

	}
	wsprintf(G_cTxt, "(O) New character(%s) created.", cNewCharName);
	PutLogList(G_cTxt);
		
	ZeroMemory(cData, sizeof(cData));

	cp = (char *)(cData);
	memcpy(cData, cAccountName, 10);
	cp += 10;

	dwp  = (DWORD *)(cData + 10);
	*dwp = dwCharID;

	dwp  = (DWORD *)(cData + 14);
	*dwp = MSGID_RESPONSE_CHARACTERLOG;

	wp   = (WORD *)(cData + 18);
	*wp  = DEF_LOGRESMSGTYPE_NEWCHARACTERCREATED;

	cp  = (char *)(cData + 20);

	memcpy(cp, cNewCharName, 10);
	cp += 10;

	*cp = (char)cTotalChar;
	cp++;

	for (i = 0; i < cTotalChar; i++) {
		
		memcpy(cp, &cTotalCharNames[i*11], 10);
		cp += 10;
		
		ZeroMemory(cCharacterName, sizeof(cCharacterName));
		memcpy(cCharacterName, &cTotalCharNames[i*11], 10);

		if ((iGetCharacterData(cCharacterName, cMapName, &sAppr1, &sAppr2, &sAppr3, &sAppr4, &iApprColor, &cSex, &cSkin, &iLevel, &iExp, &iStr, &iVit, &iDex, &iInt, &iMag, &iCharisma, &iSaveYear, &iSaveMonth, &iSaveDay, &iSaveHour, &iSaveMinute)) != TRUE) {
			*cp = 0;
			cp += 40;
			break;
		}
		*cp = 1;
		cp++;

		wp = (WORD *)cp;
		*wp = (int)sAppr1;
		cp += 2;

		wp = (WORD *)cp;
		*wp = (int)sAppr2;
		cp += 2;

		wp = (WORD *)cp;
		*wp = (int)sAppr3;
		cp += 2;

		wp = (WORD *)cp;
		*wp = (int)sAppr4;
		cp += 2;

		wp = (WORD *)cp;
		*wp = (int)cSex;
		cp += 2;

		wp = (WORD *)cp;
		*wp = (int)cSkin;
		cp += 2;

		wp = (WORD *)cp;
		*wp = (int)iLevel;
		cp += 2;

		dwp = (DWORD *)cp;
		*dwp = (int)iExp;
		cp += 4;

		wp = (WORD *)cp;
		*wp = (int)iStr;
		cp += 2;

		wp = (WORD *)cp;
		*wp = (int)iVit;
		cp += 2;

		wp = (WORD *)cp;
		*wp = (int)iDex;
		cp += 2;

		wp = (WORD *)cp;
		*wp = (int)iInt;
		cp += 2;

		wp = (WORD *)cp;
		*wp = (int)iMag;
		cp += 2;

		wp = (WORD *)cp;
		*wp = (int)iCharisma;
		cp += 2;

		ip = (int *)cp;
		*ip = iApprColor;
		cp += 4;

		wp = (WORD *)cp;
		*wp = (int)iSaveYear;
		cp += 2;
		
		wp = (WORD *)cp;
		*wp = (int)iSaveMonth;
		cp += 2;

		wp = (WORD *)cp;
		*wp = (int)iSaveDay;
		cp += 2;

		wp = (WORD *)cp;
		*wp = (int)iSaveHour;
		cp += 2;

		wp = (WORD *)cp;
		*wp = (int)iSaveMinute;
		cp += 2;
		
		memcpy(cp, cMapName, 10);
		cp += 10;
	}

	SendEventToMLS(MSGID_RESPONSE_CHARACTERLOG, DEF_LOGRESMSGTYPE_NEWCHARACTERCREATED, cData, (cTotalChar*65)+32, -1);

}

void CWorldLog::RequestDeleteCharacter(int iClientH, char *pData)
{
 char cAccountName[11], cCharacterName[11], cInfo[111], cData[2000], cMapName[11], cCharList[256], cFileName[255], cTxt[100], cDir[100];
 char *cp;
 DWORD *dwp, dwCharID;
 WORD *wp;
 short sAppr1, sAppr2, sAppr3, sAppr4;
 char cSex, cSkin, cTotalChar; 
 int i, iLevel, iStr, iVit, iInt, iDex, iMag, iCharisma, iExp, iApprColor, iSaveYear, iSaveMonth, iSaveDay, iSaveHour, iSaveMinute;
 int *ip;

	ZeroMemory(cAccountName, sizeof(cAccountName));
	ZeroMemory(cCharacterName, sizeof(cCharacterName)); 
	ZeroMemory(cInfo, sizeof(cInfo));
	ZeroMemory(cFileName, sizeof(cFileName));
	ZeroMemory(cTxt, sizeof(cTxt));
	ZeroMemory(cDir, sizeof(cDir));

	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
	memcpy(cAccountName, cp, 10);
	cp += 10;

	dwp = (DWORD *)cp;
	dwCharID = *dwp;
	cp += 4;

	memcpy(cCharacterName, cp, 10);
	cp += 10;

	cTotalChar = *cp;
	cp++;

	memcpy(cInfo, cp, cTotalChar*11);

	if (G_bDBMode == TRUE) {
		// todo: add SQL Code Here
		
	}
	else {
		strcat(cFileName, "Character");
		strcat(cFileName, "\\");
		strcat(cFileName, "\\");
		wsprintf(cTxt, "AscII%d", *cCharacterName);
		strcat(cFileName, cTxt);
		strcpy(cDir, cFileName);
		strcat(cFileName,"\\");
		strcat(cFileName,"\\");					  
		strcat(cFileName, cCharacterName);
		strcat(cFileName, ".txt");

		DeleteFile(cFileName);

		wsprintf(G_cTxt, "Character(%s) deleted! Account(%s)", cCharacterName, cAccountName);
		PutLogList(G_cTxt);
		PutGameLogData(G_cTxt);

		ZeroMemory(cData, sizeof(cData));

		cp = (char *)(cData);
		memcpy(cData, cAccountName, 10);
		
		dwp  = (DWORD *)(cData + 10);
		*dwp = dwCharID;

		dwp  = (DWORD *)(cData + 14);
		*dwp = MSGID_RESPONSE_CHARACTERLOG;

		wp   = (WORD *)(cData + 18);
		*wp  = DEF_LOGRESMSGTYPE_NEWCHARACTERDELETED;

		cp  = (char *)(cData + 20);
		
		*cp = 0;
		cp++;

		*cp = (char)cTotalChar;
		cp++;

		for (i = 0; i < cTotalChar; i++) {
			ZeroMemory(cMapName, sizeof(cMapName));
			ZeroMemory(cCharList, sizeof(cCharList));
			memcpy(cCharList, &cInfo[i*11], 10);
			memcpy(cp, &cInfo[i*11], 10);
			cp += 10;
				
			if ((iGetCharacterData(cCharList, cMapName, &sAppr1, &sAppr2, &sAppr3, &sAppr4, &iApprColor, &cSex, &cSkin, &iLevel, &iExp, &iStr, &iVit, &iDex, &iInt, &iMag, &iCharisma, &iSaveYear, &iSaveMonth, &iSaveDay, &iSaveHour, &iSaveMinute)) != TRUE) {
				*cp = 0;
				cp += 40;
				break;
			}
			wsprintf(G_cTxt, "(TestLog) Account(%s) Char(%s)", cAccountName, cCharList);
			PutLogList(G_cTxt);

			*cp = 1;
			cp++;

			wp = (WORD *)cp;
			*wp = (int)sAppr1;
			cp += 2;

			wp = (WORD *)cp;
			*wp = (int)sAppr2;
			cp += 2;

			wp = (WORD *)cp;
			*wp = (int)sAppr3;
			cp += 2;

			wp = (WORD *)cp;
			*wp = (int)sAppr4;
			cp += 2;

			wp = (WORD *)cp;
			*wp = (int)cSex;
			cp += 2;

			wp = (WORD *)cp;
			*wp = (int)cSkin;
			cp += 2;

			wp = (WORD *)cp;
			*wp = (int)iLevel;
			cp += 2;

			dwp = (DWORD *)cp;
			*dwp = (int)iExp;
			cp += 4;

			wp = (WORD *)cp;
			*wp = (int)iStr;
			cp += 2;

			wp = (WORD *)cp;
			*wp = (int)iVit;
			cp += 2;

			wp = (WORD *)cp;
			*wp = (int)iDex;
			cp += 2;

			wp = (WORD *)cp;
			*wp = (int)iInt;
			cp += 2;

			wp = (WORD *)cp;
			*wp = (int)iMag;
			cp += 2;

			wp = (WORD *)cp;
			*wp = (int)iCharisma;
			cp += 2;

			ip = (int *)cp;
			*ip = iApprColor;
			cp += 4;

			wp = (WORD *)cp;
			*wp = (int)iSaveYear;
			cp += 2;
			
			wp = (WORD *)cp;
			*wp = (int)iSaveMonth;
			cp += 2;

			wp = (WORD *)cp;
			*wp = (int)iSaveDay;
			cp += 2;

			wp = (WORD *)cp;
			*wp = (int)iSaveHour;
			cp += 2;

			wp = (WORD *)cp;
			*wp = (int)iSaveMinute;
			cp += 2;
			
			memcpy(cp, cMapName, 10);
		 	cp += 10;
		}
	}
	SendEventToMLS(MSGID_RESPONSE_CHARACTERLOG, DEF_LOGRESMSGTYPE_NEWCHARACTERDELETED, cData, (cTotalChar*65)+20, -1);
}
*/
void CWorldLog::RequestCreateNewCharacter(int iClientH, char* pData)
{
	char cData[3000], cFileName[255], cCharacterName[111], cTotalCharNames[111], cNewCharName[11], cAccountName[11], cPassword[11], cUnknown1[31], cTxt[11], cTxt2[120], cMapName[11], cDir[11];
	char* cp;
	DWORD* dwp, dwCharID, dwTime = timeGetTime();
	WORD* wp;
	short sAppr1, sAppr2, sAppr3, sAppr4;
	char cSex, cSkin, cTotalChar, cNewGender, cNewSkin, cNewStr, cNewVit, cNewDex, cNewInt, cNewMag, cNewChr;
	int iTempAppr2, iTempAppr3, iTempAppr1, i, iLevel, iStr, iVit, iInt, iDex, iMag, iCharisma, iExp, iApprColor, iSaveYear, iSaveMonth, iSaveDay, iSaveHour, iSaveMinute;
	int* ip;
	FILE* pFile;

	pFile = 0;
	sAppr1 = 0;
	sAppr2 = 0;
	sAppr3 = 0;
	sAppr4 = 0;
	ZeroMemory(cData, sizeof(cData));
	ZeroMemory(cFileName, sizeof(cFileName));
	ZeroMemory(cCharacterName, sizeof(cCharacterName));
	ZeroMemory(cNewCharName, sizeof(cNewCharName));
	ZeroMemory(cAccountName, sizeof(cAccountName));
	ZeroMemory(cPassword, sizeof(cPassword));
	ZeroMemory(cDir, sizeof(cDir));
	ZeroMemory(cTxt, sizeof(cTxt));
	ZeroMemory(cTotalCharNames, sizeof(cTotalCharNames));
	ZeroMemory(cUnknown1, sizeof(cUnknown1));
	ZeroMemory(cTxt2, sizeof(cTxt2));
	
	cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);

	memcpy(cNewCharName, cp, 10);
	cp += 10;

	memcpy(cAccountName, cp, 10);
	cp += 10;

	memcpy(cPassword, cp, 10);
	cp += 10;

	memcpy(cUnknown1, cp, 30);
	cp += 30;

	wsprintf(G_cTxt, "(TestLog) Request create new character(%s). Account(%s) Password(%s) OtherChars(%s)", cNewCharName, cAccountName, cPassword, cUnknown1);
	PutLogList(G_cTxt);

	cNewGender = *cp;
	cp++;

	cNewSkin = *cp;
	cp++;

	iTempAppr2 = *cp;
	cp++;

	iTempAppr3 = *cp;
	cp++;

	iTempAppr1 = *cp;
	cp++;

	cNewStr = *cp;
	cp++;

	cNewVit = *cp;
	cp++;

	cNewDex = *cp;
	cp++;

	cNewInt = *cp;
	cp++;

	cNewMag = *cp;
	cp++;

	cNewChr = *cp;
	cp++;

	dwp = (DWORD*)cp;
	dwCharID = *dwp;
	cp += 4;

	cTotalChar = (char)*cp;
	cp++;

	memcpy(cTotalCharNames, cp, cTotalChar * 11);
	cp += cTotalChar * 11;

	
		cp = (char*)(cData);
		memcpy(cp, cAccountName, 10);
		cp += 10;

		dwp = (DWORD*)(cData + 10);
		*dwp = dwCharID;
		dwp = (DWORD*)(cData + 14);
		*dwp = MSGID_RESPONSE_CHARACTERLOG;
		wp = (WORD*)(cData + 18);
		*wp = DEF_LOGRESMSGTYPE_NEWCHARACTERFAILED;
	

	sAppr1 = 0;
	sAppr1 = sAppr1 | iTempAppr1;
	sAppr1 = sAppr1 | (iTempAppr2 << 8);
	sAppr1 = sAppr1 | (iTempAppr3 << 4);

	if (G_bDBMode == TRUE) {
		BOOL exists = bCheckCharacterExists(cNewCharName);

		int iAccountDBid = iGetAccountDatabaseID(cAccountName);

		if (iAccountDBid < 0)
		{
			cp = (char*)(cData);
			memcpy(cp, cAccountName, 10);
			cp += 10;

			dwp = (DWORD*)(cData + 10);
			*dwp = dwCharID;
			dwp = (DWORD*)(cData + 14);
			*dwp = MSGID_RESPONSE_CHARACTERLOG;
			wp = (WORD*)(cData + 18);
			*wp = DEF_LOGRESMSGTYPE_NEWCHARACTERFAILED;
		}

		if (exists)
		{
			wsprintf(G_cTxt, "(X) Cannot create new character - Already existing character(%s)", cNewCharName);
			PutLogList(G_cTxt);

			cp = (char*)(cData);
			memcpy(cp, cAccountName, 10);
			cp += 10;

			dwp = (DWORD*)(cData + 10);
			*dwp = dwCharID;
			dwp = (DWORD*)(cData + 14);
			*dwp = MSGID_RESPONSE_CHARACTERLOG;
			wp = (WORD*)(cData + 18);
			*wp = DEF_LOGRESMSGTYPE_ALREADYEXISTINGCHARACTER;

			SendEventToMLS(MSGID_RESPONSE_CHARACTERLOG, DEF_LOGRESMSGTYPE_ALREADYEXISTINGCHARACTER, cData, 20, -1);
			fclose(pFile);
			return;
		}
		else
		{
			SACommand com;
			SACommand getID;
			SACommand addGold;

			try
			{
				com.setConnection(&con);
				com.setCommandText("INSERT INTO Characters ([Account-ID], [Character-Name], [Character-Sex], [Character-Skin], [Character-Hair-Style], [Character-Hair-Colour], [Character-Underwear], [Character-Strength], [Character-Vitality], [Character-Dexterity], [Character-Intelligence], [Character-Magic], [Character-Charisma], [Character-Appr1], [Character-Appr2], [Character-Appr3], [Character-Appr4]) VALUES (:1, :2, :3, :4, :5, :6, :7, :8, :9, :10, :11, :12, :13, :14, :15, :16, :17)");
				com.Param(1).setAsLong() = iAccountDBid;
				com.Param(2).setAsString() = cNewCharName;
				com.Param(3).setAsLong() = cNewGender;
				com.Param(4).setAsLong() = cNewSkin;
				com.Param(5).setAsLong() = iTempAppr2;
				com.Param(6).setAsLong() = iTempAppr3;
				com.Param(7).setAsLong() = iTempAppr1;
				com.Param(8).setAsLong() = cNewStr;
				com.Param(9).setAsLong() = cNewVit;
				com.Param(10).setAsLong() = cNewDex;
				com.Param(11).setAsLong() = cNewInt;
				com.Param(12).setAsLong() = cNewMag;
				com.Param(13).setAsLong() = cNewChr;
				com.Param(14).setAsLong() = sAppr1;
				com.Param(15).setAsLong() = sAppr2;
				com.Param(16).setAsLong() = sAppr3;
				com.Param(17).setAsLong() = sAppr4;

				com.Execute();

				int iCharDBID = 0;
				char cItemName[11];

				getID.setConnection(&con);
				getID.setCommandText("SELECT [Character-ID] FROM Characters WHERE [Character-Name] LIKE :1");
				getID.Param(1).setAsString() = cNewCharName;

				getID.Execute();

				if (getID.isResultSet())
				{
					while (getID.FetchNext())
					{
						iCharDBID = getID.Field("Character-ID").asLong();
					}
				}

				ZeroMemory(cItemName, sizeof(cItemName));
				strcpy(cItemName, "Gold");

				addGold.setConnection(&con);
				addGold.setCommandText("INSERT INTO CharItems VALUES (:1, :2, :3, :4, :5, :6, :7, :8, :9, :10, :11, :12, :13, :14, :15)");
				addGold.Param(1).setAsLong() = 1; // item id
				addGold.Param(2).setAsLong() = iCharDBID; // char id
				addGold.Param(3).setAsString() = cItemName; // item name
				addGold.Param(4).setAsLong() = 400; // item number 1 (amount)
				addGold.Param(5).setAsLong() = 0;
				addGold.Param(6).setAsLong() = 0;
				addGold.Param(7).setAsLong() = 0;
				addGold.Param(8).setAsLong() = 0;
				addGold.Param(9).setAsLong() = 0;
				addGold.Param(10).setAsLong() = 0;
				addGold.Param(11).setAsLong() = 0;
				addGold.Param(12).setAsLong() = 0;
				addGold.Param(13).setAsLong() = 0;
				addGold.Param(14).setAsLong() = 0;
				addGold.Param(15).setAsLong() = 0;

				addGold.Execute();

				ZeroMemory(G_cTxt, sizeof(G_cTxt));
				wsprintf(G_cTxt, "Character Created:(%s) Account:(%s) Str(%d)Int(%d)Vit(%d)Dex(%d)Mag(%d)Chr(%d)Gender(%d)", cNewCharName, cAccountName, cNewStr, cNewInt, cNewVit, cNewDex, cNewMag, cNewChr, cNewGender - 1);
				PutLogList(G_cTxt);

				//ZeroMemory(G_cTxt, sizeof(G_cTxt));
				//wsprintf(G_cTxt, "Character Created:(%s) - %i ms", (int)timeGetTime()-dwTime);
				//PutLogList(G_cTxt);


			}
			catch (SAException& x)
			{
				try
				{
					con.Rollback();
				}
				catch (SAException&)
				{
				}

				char cTemp[256];
				ZeroMemory(cTemp, sizeof(cTemp));
				wsprintf(cTemp, "(!!!) SQL SERVER ERROR (RequestCreateNewCharacter): %s", (const char*)x.ErrText());
				PutLogList(cTemp);

				cp = (char*)(cData);
				memcpy(cp, cAccountName, 10);
				cp += 10;

				dwp = (DWORD*)(cData + 10);
				*dwp = dwCharID;
				dwp = (DWORD*)(cData + 14);
				*dwp = MSGID_RESPONSE_CHARACTERLOG;
				wp = (WORD*)(cData + 18);
				*wp = DEF_LOGRESMSGTYPE_NEWCHARACTERFAILED;
			}
		}

	}
	else {
		strcat(cFileName, "Character");
		strcat(cFileName, "\\");
		strcat(cFileName, "\\");
		wsprintf(cTxt, "AscII%d", *cNewCharName);
		strcat(cFileName, cTxt);
		strcpy(cDir, cFileName);
		strcat(cFileName, "\\");
		strcat(cFileName, "\\");
		strcat(cFileName, cNewCharName);
		strcat(cFileName, ".txt");

		_mkdir("Character");
		_mkdir(cDir);

		ZeroMemory(cData, sizeof(cData));
		if (fopen(cFileName, "rt") != NULL) {

			wsprintf(G_cTxt, "(X) Cannot create new character - Already existing character(%s)", cFileName);
			PutLogList(G_cTxt);

			cp = (char*)(cData);
			memcpy(cp, cAccountName, 10);
			cp += 10;

			dwp = (DWORD*)(cData + 10);
			*dwp = dwCharID;
			dwp = (DWORD*)(cData + 14);
			*dwp = MSGID_RESPONSE_CHARACTERLOG;
			wp = (WORD*)(cData + 18);
			*wp = DEF_LOGRESMSGTYPE_ALREADYEXISTINGCHARACTER;

			SendEventToMLS(MSGID_RESPONSE_CHARACTERLOG, DEF_LOGRESMSGTYPE_ALREADYEXISTINGCHARACTER, cData, 20, -1);
			fclose(pFile);
			return;
		}
		pFile = fopen(cFileName, "wt");
		if (pFile == NULL) {
			wsprintf(cTxt, "(!) Cannot create new character - cannot create file : Name(%s)", cFileName);
			PutLogList(cTxt);

			cp = (char*)(cData);
			memcpy(cp, cAccountName, 10);
			cp += 10;

			dwp = (DWORD*)(cData + 10);
			*dwp = dwCharID;
			dwp = (DWORD*)(cData + 14);
			*dwp = MSGID_RESPONSE_CHARACTERLOG;
			wp = (WORD*)(cData + 18);
			*wp = DEF_LOGRESMSGTYPE_NEWCHARACTERFAILED;
		}
		else {
			CreateNewCharacterFile(pData, cFileName, cNewCharName, cAccountName, cPassword, cNewGender, cNewSkin, iTempAppr1, iTempAppr2, iTempAppr3, cNewStr, cNewVit, cNewDex, cNewInt, cNewMag, cNewChr, sAppr1, sAppr2, sAppr3, sAppr4);
			fwrite(pData, 1, strlen(pData), pFile);
			fclose(pFile);
		}

		memcpy(&cTotalCharNames[cTotalChar * 11], cNewCharName, 10);
		cTotalChar++;

	}
	wsprintf(G_cTxt, "(O) New character (%s) created.", cNewCharName);
	PutLogList(G_cTxt);

	ZeroMemory(cData, sizeof(cData));

	cp = (char*)(cData);
	memcpy(cData, cAccountName, 10);
	cp += 10;

	dwp = (DWORD*)(cData + 10);
	*dwp = dwCharID;

	dwp = (DWORD*)(cData + 14);
	*dwp = MSGID_RESPONSE_CHARACTERLOG;

	wp = (WORD*)(cData + 18);
	*wp = DEF_LOGRESMSGTYPE_NEWCHARACTERCREATED;

	cp = (char*)(cData + 20);

	memcpy(cp, cNewCharName, 10);
	cp += 10;

	*cp = (char)cTotalChar;
	cp++;

	for (i = 0; i < cTotalChar; i++) {

		memcpy(cp, &cTotalCharNames[i * 11], 10);
		cp += 10;

		ZeroMemory(cCharacterName, sizeof(cCharacterName));
		memcpy(cCharacterName, &cTotalCharNames[i * 11], 10);

		if ((iGetCharacterData(cCharacterName, cMapName, &sAppr1, &sAppr2, &sAppr3, &sAppr4, &iApprColor, &cSex, &cSkin, &iLevel, &iExp, &iStr, &iVit, &iDex, &iInt, &iMag, &iCharisma, &iSaveYear, &iSaveMonth, &iSaveDay, &iSaveHour, &iSaveMinute)) != TRUE) {
			*cp = 0;
			cp += 40;
			break;
		}
		*cp = 1;
		cp++;

		memcpy(cp, cMapName, 10);
		cp += 10;

		wp = (WORD*)cp;
		*wp = (int)sAppr1;
		cp += 2;

		wp = (WORD*)cp;
		*wp = (int)sAppr2;
		cp += 2;

		wp = (WORD*)cp;
		*wp = (int)sAppr3;
		cp += 2;

		wp = (WORD*)cp;
		*wp = (int)sAppr4;
		cp += 2;

		wp = (WORD*)cp;
		*wp = (int)cSex;
		cp += 2;

		wp = (WORD*)cp;
		*wp = (int)cSkin;
		cp += 2;

		wp = (WORD*)cp;
		*wp = (int)iLevel;
		cp += 2;

		dwp = (DWORD*)cp;
		*dwp = (int)iExp;
		cp += 4;

		wp = (WORD*)cp;
		*wp = (int)iStr;
		cp += 2;

		wp = (WORD*)cp;
		*wp = (int)iVit;
		cp += 2;

		wp = (WORD*)cp;
		*wp = (int)iDex;
		cp += 2;

		wp = (WORD*)cp;
		*wp = (int)iInt;
		cp += 2;

		wp = (WORD*)cp;
		*wp = (int)iMag;
		cp += 2;

		wp = (WORD*)cp;
		*wp = (int)iCharisma;
		cp += 2;

		ip = (int*)cp;
		*ip = iApprColor;
		cp += 4;

		wp = (WORD*)cp;
		*wp = (int)iSaveYear;
		cp += 2;

		wp = (WORD*)cp;
		*wp = (int)iSaveMonth;
		cp += 2;

		wp = (WORD*)cp;
		*wp = (int)iSaveDay;
		cp += 2;

		wp = (WORD*)cp;
		*wp = (int)iSaveHour;
		cp += 2;

		wp = (WORD*)cp;
		*wp = (int)iSaveMinute;
		cp += 2;


	}

	SendEventToMLS(MSGID_RESPONSE_CHARACTERLOG, DEF_LOGRESMSGTYPE_NEWCHARACTERCREATED, cData, (cTotalChar * 65) + 32, -1);
}

void CWorldLog::RequestDeleteCharacter(int iClientH, char* pData)
{
	char cAccountName[11], cCharacterName[11], cInfo[111], cData[2000], cMapName[11], cCharList[256], cFileName[255], cTxt[100], cDir[100];
	char* cp;
	DWORD* dwp, dwCharID;
	WORD* wp;
	short sAppr1, sAppr2, sAppr3, sAppr4;
	char cSex, cSkin, cTotalChar;
	int i, iLevel, iStr, iVit, iInt, iDex, iMag, iCharisma, iExp, iApprColor, iSaveYear, iSaveMonth, iSaveDay, iSaveHour, iSaveMinute;
	int* ip;

	ZeroMemory(cAccountName, sizeof(cAccountName));
	ZeroMemory(cCharacterName, sizeof(cCharacterName));
	ZeroMemory(cInfo, sizeof(cInfo));
	ZeroMemory(cFileName, sizeof(cFileName));
	ZeroMemory(cTxt, sizeof(cTxt));
	ZeroMemory(cDir, sizeof(cDir));

	cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);
	memcpy(cAccountName, cp, 10);
	cp += 10;

	dwp = (DWORD*)cp;
	dwCharID = *dwp;
	cp += 4;

	memcpy(cCharacterName, cp, 10);
	cp += 10;

	cTotalChar = *cp;
	cp++;

	memcpy(cInfo, cp, cTotalChar * 11);

	if (G_bDBMode == TRUE) {
		SACommand com;

		try
		{
			com.setConnection(&con);
			com.setCommandText("DELETE FROM Characters WHERE [Character-Name] = :1");
			com.Param(1).setAsString() = cCharacterName;

			com.Execute();

			long affected = com.RowsAffected();

			char cTemp[256];
			ZeroMemory(cTemp, sizeof(cTemp));
			wsprintf(cTemp, "(!) Deleted Character From SQL: (%s)", cCharacterName);

			ZeroMemory(cData, sizeof(cData));

			cp = (char*)(cData);
			memcpy(cData, cAccountName, 10);

			dwp = (DWORD*)(cData + 10);
			*dwp = dwCharID;

			dwp = (DWORD*)(cData + 14);
			*dwp = MSGID_RESPONSE_CHARACTERLOG;

			wp = (WORD*)(cData + 18);
			*wp = DEF_LOGRESMSGTYPE_NEWCHARACTERDELETED;

			cp = (char*)(cData + 20);

			*cp = 0;
			cp++;

			*cp = (char)cTotalChar;
			cp++;

			for (i = 0; i < cTotalChar; i++) {
				ZeroMemory(cMapName, sizeof(cMapName));
				ZeroMemory(cCharList, sizeof(cCharList));
				memcpy(cCharList, &cInfo[i * 11], 10);
				memcpy(cp, &cInfo[i * 11], 10);
				cp += 10;

				if ((iGetCharacterData(cCharList, cMapName, &sAppr1, &sAppr2, &sAppr3, &sAppr4, &iApprColor, &cSex, &cSkin, &iLevel, &iExp, &iStr, &iVit, &iDex, &iInt, &iMag, &iCharisma, &iSaveYear, &iSaveMonth, &iSaveDay, &iSaveHour, &iSaveMinute)) != TRUE) {
					*cp = 0;
					cp += 40;
					break;
				}
				wsprintf(G_cTxt, "(TestLog) Account(%s) Char(%s)", cAccountName, cCharList);
				PutLogList(G_cTxt);

				*cp = 1;
				cp++;

				wp = (WORD*)cp;
				*wp = (int)sAppr1;
				cp += 2;

				wp = (WORD*)cp;
				*wp = (int)sAppr2;
				cp += 2;

				wp = (WORD*)cp;
				*wp = (int)sAppr3;
				cp += 2;

				wp = (WORD*)cp;
				*wp = (int)sAppr4;
				cp += 2;

				wp = (WORD*)cp;
				*wp = (int)cSex;
				cp += 2;

				wp = (WORD*)cp;
				*wp = (int)cSkin;
				cp += 2;

				wp = (WORD*)cp;
				*wp = (int)iLevel;
				cp += 2;

				dwp = (DWORD*)cp;
				*dwp = (int)iExp;
				cp += 4;

				wp = (WORD*)cp;
				*wp = (int)iStr;
				cp += 2;

				wp = (WORD*)cp;
				*wp = (int)iVit;
				cp += 2;

				wp = (WORD*)cp;
				*wp = (int)iDex;
				cp += 2;

				wp = (WORD*)cp;
				*wp = (int)iInt;
				cp += 2;

				wp = (WORD*)cp;
				*wp = (int)iMag;
				cp += 2;

				wp = (WORD*)cp;
				*wp = (int)iCharisma;
				cp += 2;

				ip = (int*)cp;
				*ip = iApprColor;
				cp += 4;

				wp = (WORD*)cp;
				*wp = (int)iSaveYear;
				cp += 2;

				wp = (WORD*)cp;
				*wp = (int)iSaveMonth;
				cp += 2;

				wp = (WORD*)cp;
				*wp = (int)iSaveDay;
				cp += 2;

				wp = (WORD*)cp;
				*wp = (int)iSaveHour;
				cp += 2;

				wp = (WORD*)cp;
				*wp = (int)iSaveMinute;
				cp += 2;

				memcpy(cp, cMapName, 10);
				cp += 10;
			}
		}
		catch (SAException& x)
		{
			try
			{
				con.Rollback();
			}
			catch (SAException&)
			{
			}

			char cTemp[256];
			ZeroMemory(cTemp, sizeof(cTemp));
			wsprintf(cTemp, "(!!!) SQL SERVER ERROR (RequestDeleteCharacter): %s", (const char*)x.ErrText());
			PutLogList(cTemp);

			return;
		}
	}
	else {
		strcat(cFileName, "Character");
		strcat(cFileName, "\\");
		strcat(cFileName, "\\");
		wsprintf(cTxt, "AscII%d", *cCharacterName);
		strcat(cFileName, cTxt);
		strcpy(cDir, cFileName);
		strcat(cFileName, "\\");
		strcat(cFileName, "\\");
		strcat(cFileName, cCharacterName);
		strcat(cFileName, ".txt");

		DeleteFile(cFileName);

		wsprintf(G_cTxt, "Character(%s) deleted! Account(%s)", cCharacterName, cAccountName);
		PutLogList(G_cTxt);
		PutGameLogData(G_cTxt);

		ZeroMemory(cData, sizeof(cData));

		cp = (char*)(cData);
		memcpy(cData, cAccountName, 10);

		dwp = (DWORD*)(cData + 10);
		*dwp = dwCharID;

		dwp = (DWORD*)(cData + 14);
		*dwp = MSGID_RESPONSE_CHARACTERLOG;

		wp = (WORD*)(cData + 18);
		*wp = DEF_LOGRESMSGTYPE_NEWCHARACTERDELETED;

		cp = (char*)(cData + 20);

		*cp = 0;
		cp++;

		*cp = (char)cTotalChar;
		cp++;

		for (i = 0; i < cTotalChar; i++) {
			ZeroMemory(cMapName, sizeof(cMapName));
			ZeroMemory(cCharList, sizeof(cCharList));
			memcpy(cCharList, &cInfo[i * 11], 10);
			memcpy(cp, &cInfo[i * 11], 10);
			cp += 10;

			if ((iGetCharacterData(cCharList, cMapName, &sAppr1, &sAppr2, &sAppr3, &sAppr4, &iApprColor, &cSex, &cSkin, &iLevel, &iExp, &iStr, &iVit, &iDex, &iInt, &iMag, &iCharisma, &iSaveYear, &iSaveMonth, &iSaveDay, &iSaveHour, &iSaveMinute)) != TRUE) {
				*cp = 0;
				cp += 40;
				break;
			}
			wsprintf(G_cTxt, "(TestLog) Account(%s) Char(%s)", cAccountName, cCharList);
			PutLogList(G_cTxt);

			*cp = 1;
			cp++;

			wp = (WORD*)cp;
			*wp = (int)sAppr1;
			cp += 2;

			wp = (WORD*)cp;
			*wp = (int)sAppr2;
			cp += 2;

			wp = (WORD*)cp;
			*wp = (int)sAppr3;
			cp += 2;

			wp = (WORD*)cp;
			*wp = (int)sAppr4;
			cp += 2;

			wp = (WORD*)cp;
			*wp = (int)cSex;
			cp += 2;

			wp = (WORD*)cp;
			*wp = (int)cSkin;
			cp += 2;

			wp = (WORD*)cp;
			*wp = (int)iLevel;
			cp += 2;

			dwp = (DWORD*)cp;
			*dwp = (int)iExp;
			cp += 4;

			wp = (WORD*)cp;
			*wp = (int)iStr;
			cp += 2;

			wp = (WORD*)cp;
			*wp = (int)iVit;
			cp += 2;

			wp = (WORD*)cp;
			*wp = (int)iDex;
			cp += 2;

			wp = (WORD*)cp;
			*wp = (int)iInt;
			cp += 2;

			wp = (WORD*)cp;
			*wp = (int)iMag;
			cp += 2;

			wp = (WORD*)cp;
			*wp = (int)iCharisma;
			cp += 2;

			ip = (int*)cp;
			*ip = iApprColor;
			cp += 4;

			wp = (WORD*)cp;
			*wp = (int)iSaveYear;
			cp += 2;

			wp = (WORD*)cp;
			*wp = (int)iSaveMonth;
			cp += 2;

			wp = (WORD*)cp;
			*wp = (int)iSaveDay;
			cp += 2;

			wp = (WORD*)cp;
			*wp = (int)iSaveHour;
			cp += 2;

			wp = (WORD*)cp;
			*wp = (int)iSaveMinute;
			cp += 2;

			memcpy(cp, cMapName, 10);
			cp += 10;
		}
	}
	SendEventToMLS(MSGID_RESPONSE_CHARACTERLOG, DEF_LOGRESMSGTYPE_NEWCHARACTERDELETED, cData, (cTotalChar * 65) + 20, -1);
}
void CWorldLog::UpdateGuildsmanSQL(char *cMemberName, char *cGuildName, bool bUpdateType)
{
	/*
	// NOTE: bUpdateType seems to me guildmaster delete or just member delete
	// for example if updatetype = 0 then delete guild and all members --- only guildmaster can do this

	// if (strcmp("NONE", cGuildName)== 0) return;
	// memset(cTxt, 0, 256);
	// memset(cTxt2, 0, 256);
	if (bUpdateType == 0) {
		// wsprintf(cTxt, "delete GUILD_T where cGuildName = '%s' ", cGuildName)
		// wsprintf(cTxt2, "delete GUILDMEMBER_T where cGuildName = '%s' ", cGuildName)
		// strcat(cTxt, cTxt2)
	}
	else {
		// wsprintf(cTxt, "delete GUILDMEMBER_T where cMemberName = '%s' ", cMemberName)
	}
	if (bUpdateType != 0) {
		// wsprintf(cTxt, "select * from GUILD_T where cGuildName = '%s' ", cGuildName)
	}
	else {
		return;
	}
	// wsprintf(cTxt, "%x-(3057) sql (%s)", ??????);
	// PutGameLogData(cTxt);
	// MessageBox(m_hWnd, cTxt, "DB Error", MB_OK+MB_ICONERROR);
	*/

}

int CWorldLog::UpdateGuildFile_NewGuildsMan(char * cFile, char * pData, char * cp)
{
	// INCOMPLETE
	// hypnotoad: useless function?
/* FILE * pFile;
 HANDLE hFile;
 DWORD dwFileSize;
 char cTxt[500];
 int iCount;

	for (i = 0; i < DEF_MAXSUBLOGSOCK; i++) {


	}
		hFile = CreateFile(cFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, 0);
		if (hFile == INVALID_HANDLE_VALUE) return -1;
		dwFileSize = GetFileSize(hFile, NULL);
		CloseHandle(hFile);
		pFile = fopen(cFile, "rt");
		if (pFile == NULL) return -1;
		
		cp = new char[dwFileSize+10];
		ZeroMemory(cp, dwFileSize+10);

		fread(cp, dwFileSize, 1, pFile);
		fclose(pFile);
		
		ZeroMemory(cTxt, sizeof(cTxt));
		
		iCount = 0;
		if (*cp != 0) {
			*cp = pData;
			strcat(cTxt, cData);
			if (*cp == 10) {
				if (strlen(cTxt)+1) <= 10) {

				}
			}
		}
		ZeroMemory(cp, dwFileSize+10);



	fopen(cFile, "wt+");
	fwrite(cp, 1, strlen(cp), pFile);
	fclose(pFile);

	delete cp;
	return 0;*/
	return 0;
}

int CWorldLog::UpdateGuildFile_DeleteGuildsMan(char * cFile, char * pData, char * cp)
{
	// INCOMPLETE
	// hypnotoad: useless function?
	return 0;
}

/*
void CWorldLog::RequestCreateNewGuild(int iClientH, char *pData)
{
 char cFileName[255], cData[500];
 char cTxt[500];
 char cTxt2[100];
 char cGuildMasterName[11], cGuildLocation[11], cDir[255], cGuildName[21];
 char *cp;
 DWORD *dwp, dwGuildGUID;
 WORD *wp;
 int iRet;
 SYSTEMTIME SysTime;
 FILE * pFile;

	if (m_pClientList[iClientH] == NULL) return;
	ZeroMemory(cFileName, sizeof(cFileName));
	ZeroMemory(cTxt, sizeof(cTxt));
	ZeroMemory(cTxt2, sizeof(cTxt2));
 	ZeroMemory(cDir, sizeof(cDir));
	ZeroMemory(cData, sizeof(cData));
	ZeroMemory(cGuildMasterName, sizeof(cGuildMasterName));
	ZeroMemory(cGuildName, sizeof(cGuildName));
	ZeroMemory(cGuildLocation, sizeof(cGuildLocation));

	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
	memcpy(cGuildMasterName, cp, 10);
	cp += 30;

	memcpy(cGuildName, cp, 20);
	cp += 20;

	memcpy(cGuildLocation, cp, 10);
	cp += 10;

	dwp  = (DWORD *)cp;
 	dwGuildGUID = *dwp;
	cp += 4;

	if (G_bDBMode == TRUE) {
		// todo: add sql code here
		
	}
	else {
		strcat(cFileName, "Guild");
 		strcat(cFileName, "\\");
		wsprintf(cTxt2, "AscII%d", *cGuildName);
		strcat(cFileName, cTxt2);
		strcat(cDir, cFileName);
		strcat(cFileName, "\\");
 		strcat(cFileName, "\\");
		strcat(cFileName, cGuildName);
		strcat(cFileName, ".txt");

		_mkdir("Guild");
		_mkdir(cDir);

		pFile = fopen(cFileName, "rt");
		if (pFile != NULL) {
			wsprintf(cTxt2, "(X) Cannot create new guild - Already existing guild name: Name(%s)", cFileName);
			PutLogList(cTxt2);
			
			cp  = (char *)(cData + 16);

			dwp  = (DWORD *)cData;
			*dwp = MSGID_RESPONSE_CREATENEWGUILD;
			
			wp  = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
			*wp = DEF_MSGTYPE_REJECT;

			cp  = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);
			memcpy(cp, cGuildMasterName, 10);
			cp += 10;

			iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 16, DEF_USE_ENCRYPTION);
			if (pFile != NULL) fclose(pFile);
		}
		else {
			pFile = fopen(cFileName, "wt");
			if (pFile == NULL) {
				wsprintf(cTxt2, "(X) Cannot create new guild - cannot create file : Name(%s)", cFileName);
				PutLogList(cTxt2);
			
				cp  = (char *)(cData + 16);
				
				dwp  = (DWORD *)cData;
				*dwp = MSGID_RESPONSE_CREATENEWGUILD;

				wp  = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
				*wp = DEF_MSGTYPE_REJECT;

				cp  = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);
				memcpy(cp, cGuildMasterName, 10);
				cp += 10;

				iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 16, DEF_USE_ENCRYPTION);
			}
			else {
				wsprintf(cTxt2, "(O) New guild created : Name(%s)", cFileName);
				PutLogList(cTxt2);
				
				ZeroMemory(cTxt2, sizeof(cTxt2));
				ZeroMemory(cTxt, sizeof(cTxt));
				GetLocalTime(&SysTime);
					
				wsprintf(cTxt, ";Guild file - Updated %4d/%2d/%2d/%2d/%2d", SysTime.wYear, SysTime.wMonth, SysTime.wDay, SysTime.wHour, SysTime.wMinute);
				strcat(cTxt, "\n");
				strcat(cTxt, ";Just created\n\n");
						
				strcat(cTxt, "[GUILD-INFO]\n\n");

				strcat(cTxt, "guildmaster-name     = ");
				strcat(cTxt, cGuildMasterName);
				strcat(cTxt, "\n");

				strcat(cTxt, "guild-GUID           = ");
				wsprintf(cTxt2,"%d", dwGuildGUID);
				strcat(cTxt, cTxt2);
				strcat(cTxt, "\n");

				strcat(cTxt, "guild-location       = ");
				strcat(cTxt, cGuildLocation);
				strcat(cTxt, "\n");

				strcat(cTxt, "guild-level       = 0");
				strcat(cTxt, "\n\n");

				strcat(cTxt, "[GUILDSMAN]\n\n");

				fwrite(cTxt, 1, strlen(cTxt), pFile);		
				
				dwp  = (DWORD *)cData;
				*dwp = MSGID_RESPONSE_CREATENEWGUILD;
				
				wp  = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
				*wp = DEF_MSGTYPE_CONFIRM;

				cp  = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);
				memcpy(cp, cGuildMasterName, 10);
				cp += 10;

				iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 16, DEF_USE_ENCRYPTION);
				if (pFile != NULL) fclose(pFile);
			}
		}
		switch(iRet) {
		case DEF_XSOCKEVENT_QUENEFULL:
		case DEF_XSOCKEVENT_SOCKETERROR:
		case DEF_XSOCKEVENT_CRITICALERROR:
		case DEF_XSOCKEVENT_SOCKETCLOSED:
			DeleteClient(iClientH, 0);
			delete m_pClientList[iClientH];
			m_pClientList[iClientH] = NULL;
			return;
		}	
	}
}

void CWorldLog::RequestDisbandGuild(int iClientH, char *pData)
{
 char cTemp[500];
 DWORD *dwp;
 WORD *wp;
 char *cp;
 char cFileName[255], cTxt[100], cDir[100];
 char cGuildMasterName[11], cGuildName[21];
 int iRet;
 FILE * pFile;

	if (m_pClientList[iClientH] == NULL) return;
	ZeroMemory(cFileName, sizeof(cFileName));
	ZeroMemory(cTxt, sizeof(cTxt));
	ZeroMemory(cDir, sizeof(cDir));
	ZeroMemory(cTemp, sizeof(cTemp));
	ZeroMemory(cGuildMasterName, sizeof(cGuildMasterName));
	ZeroMemory(cGuildName, sizeof(cGuildName));

	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
	memcpy(cGuildMasterName, cp, 10);
	cp += 30;

	memcpy(cGuildName, cp, 20);
	cp += 20;

	if (G_bDBMode == TRUE) {
		// todo: add sql code here
		
	}
	else {
		strcat(cFileName, "Guild");
		strcat(cFileName, "\\");
		strcat(cFileName, "\\");
		wsprintf(cTxt, "AscII%d", *cGuildName);
		strcat(cFileName, cTxt);
		strcat(cDir, cFileName);
		strcat(cFileName, "\\");
 		strcat(cFileName, "\\");
		strcat(cFileName, cGuildName);
		strcat(cFileName, ".txt");
		
		pFile = fopen(cFileName, "rt");
		if (pFile != NULL) {
			fclose(pFile);
			wsprintf(G_cTxt, "(O) Disband Guild - Deleting guild file... : Name(%s)", cFileName);
			PutLogList(G_cTxt);
			if (DeleteFile(cFileName) != NULL) {
				dwp = (DWORD *)(cTemp);
				*dwp = MSGID_RESPONSE_DISBANDGUILD;

				wp  = (WORD *)(cTemp + DEF_INDEX2_MSGTYPE);
				*wp = DEF_MSGTYPE_CONFIRM;

				cp  = (char *)(cTemp + DEF_INDEX2_MSGTYPE + 2);
				memcpy(cp, cGuildMasterName, 10);
				cp += 10;

				iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cTemp, 16, DEF_USE_ENCRYPTION);
			}
			else {
				dwp = (DWORD *)(cTemp);
				*dwp = MSGID_RESPONSE_DISBANDGUILD;

				wp  = (WORD *)(cTemp + DEF_INDEX2_MSGTYPE);
				*wp = DEF_MSGTYPE_REJECT;

				cp  = (char *)(cTemp + DEF_INDEX2_MSGTYPE + 2);
				memcpy(cp, cGuildMasterName, 10);
				cp += 10;

				iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cTemp, 16, DEF_USE_ENCRYPTION);
			}
		}
		else {
			dwp = (DWORD *)(cTemp);
			*dwp = MSGID_RESPONSE_DISBANDGUILD;

			wp  = (WORD *)(cTemp + DEF_INDEX2_MSGTYPE);
			*wp = DEF_MSGTYPE_REJECT;

			cp  = (char *)(cTemp + DEF_INDEX2_MSGTYPE + 2);
			memcpy(cp, cGuildMasterName, 10);
			cp += 10;

			iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cTemp, 16, DEF_USE_ENCRYPTION);
		}
	}
	switch(iRet) {
	case DEF_XSOCKEVENT_QUENEFULL:
	case DEF_XSOCKEVENT_SOCKETERROR:
	case DEF_XSOCKEVENT_CRITICALERROR:
	case DEF_XSOCKEVENT_SOCKETCLOSED:
		DeleteClient(iClientH, 0);
		delete m_pClientList[iClientH];
		m_pClientList[iClientH] = NULL;
		return;
	}
}
*/
void CWorldLog::RequestCreateNewGuild(int iClientH, char* pData)
{
	char cFileName[255], cData[500];
	char cTxt[500];
	char cTxt2[100];
	char cGuildMasterName[11], cGuildLocation[11], cDir[255], cGuildName[21];
	char* cp;
	DWORD* dwp, dwGuildGUID;
	WORD* wp;
	int iRet;
	SYSTEMTIME SysTime;
	FILE* pFile;

	if (m_pClientList[iClientH] == NULL) return;
	ZeroMemory(cFileName, sizeof(cFileName));
	ZeroMemory(cTxt, sizeof(cTxt));
	ZeroMemory(cTxt2, sizeof(cTxt2));
	ZeroMemory(cDir, sizeof(cDir));
	ZeroMemory(cData, sizeof(cData));
	ZeroMemory(cGuildMasterName, sizeof(cGuildMasterName));
	ZeroMemory(cGuildName, sizeof(cGuildName));
	ZeroMemory(cGuildLocation, sizeof(cGuildLocation));

	cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);
	memcpy(cGuildMasterName, cp, 10);
	cp += 30;

	memcpy(cGuildName, cp, 20);
	cp += 20;

	memcpy(cGuildLocation, cp, 10);
	cp += 10;

	dwp = (DWORD*)(pData + 66);
	dwGuildGUID = *dwp;
	cp += 4;

	if (G_bDBMode == TRUE) {
		BOOL exists = bCheckGuildExists(cGuildName);

		if (exists)
		{
			wsprintf(cTxt2, "(X) Cannot create new guild - Already existing guild name: Name(%s)", cFileName);
			PutLogList(cTxt2);

			cp = (char*)(cData + 16);

			dwp = (DWORD*)cData;
			*dwp = MSGID_RESPONSE_CREATENEWGUILD;

			wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
			*wp = DEF_MSGTYPE_REJECT;

			cp = (char*)(cData + DEF_INDEX2_MSGTYPE + 2);
			memcpy(cp, cGuildMasterName, 10);
			cp += 10;

			iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 16, DEF_USE_ENCRYPTION);
		}
		else
		{
			SACommand com;

			char cTemp[256];
			ZeroMemory(cTemp, sizeof(cTemp));
			wsprintf(cTemp, "%d", dwGuildGUID);

			try
			{
				com.setConnection(&con);
				com.setCommandText("INSERT INTO Guilds ([Guild-Name], [Guild-GUID], [Guild-Location], [Guild-Master]) VALUES (:1, :2, :3, :4)");
				com.Param(1).setAsString() = cGuildName;
				com.Param(2).setAsString() = cTemp;
				com.Param(3).setAsString() = cGuildLocation;
				com.Param(4).setAsString() = cGuildMasterName;

				com.Execute();

				dwp = (DWORD*)cData;
				*dwp = MSGID_RESPONSE_CREATENEWGUILD;

				wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
				*wp = DEF_MSGTYPE_CONFIRM;

				cp = (char*)(cData + DEF_INDEX2_MSGTYPE + 2);
				memcpy(cp, cGuildMasterName, 10);
				cp += 10;

				iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 16, DEF_USE_ENCRYPTION);


			}
			catch (SAException& x)
			{
				try
				{
					con.Rollback();
				}
				catch (SAException&)
				{
				}

				char cTemp[256];
				ZeroMemory(cTemp, sizeof(cTemp));
				wsprintf(cTemp, "(!!!) SQL SERVER ERROR (RequestCreateNewGuild): %s", (const char*)x.ErrText());
				PutLogList(cTemp);

				cp = (char*)(cData + 16);

				dwp = (DWORD*)cData;
				*dwp = MSGID_RESPONSE_CREATENEWGUILD;

				wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
				*wp = DEF_MSGTYPE_REJECT;

				cp = (char*)(cData + DEF_INDEX2_MSGTYPE + 2);
				memcpy(cp, cGuildMasterName, 10);
				cp += 10;

				iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 16, DEF_USE_ENCRYPTION);
			}
		}
		switch (iRet) {
		case DEF_XSOCKEVENT_QUENEFULL:
		case DEF_XSOCKEVENT_SOCKETERROR:
		case DEF_XSOCKEVENT_CRITICALERROR:
		case DEF_XSOCKEVENT_SOCKETCLOSED:
			DeleteClient(iClientH, 0);
			delete m_pClientList[iClientH];
			m_pClientList[iClientH] = NULL;
			return;
		}

	}
	else {

		strcat(cFileName, "Guild");
		strcat(cFileName, "\\");
		wsprintf(cTxt2, "AscII%d", *cGuildName);
		strcat(cFileName, cTxt2);
		strcat(cDir, cFileName);
		strcat(cFileName, "\\");
		strcat(cFileName, "\\");
		strcat(cFileName, cGuildName);
		strcat(cFileName, ".txt");

		_mkdir("Guild");
		_mkdir(cDir);

		pFile = fopen(cFileName, "rt");
		if (pFile != NULL) {
			wsprintf(cTxt2, "(X) Cannot create new guild - Already existing guild name: Name(%s)", cFileName);
			PutLogList(cTxt2);

			cp = (char*)(cData + 16);

			dwp = (DWORD*)cData;
			*dwp = MSGID_RESPONSE_CREATENEWGUILD;

			wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
			*wp = DEF_MSGTYPE_REJECT;

			cp = (char*)(cData + DEF_INDEX2_MSGTYPE + 2);
			memcpy(cp, cGuildMasterName, 10);
			cp += 10;

			iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 16, DEF_USE_ENCRYPTION);
			fclose(pFile);
		}
		else {
			pFile = fopen(cFileName, "wt");
			if (pFile == NULL) {
				wsprintf(cTxt2, "(X) Cannot create new guild - cannot create file : Name(%s)", cFileName);
				PutLogList(cTxt2);

				cp = (char*)(cData + 16);

				dwp = (DWORD*)cData;
				*dwp = MSGID_RESPONSE_CREATENEWGUILD;

				wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
				*wp = DEF_MSGTYPE_REJECT;

				cp = (char*)(cData + DEF_INDEX2_MSGTYPE + 2);
				memcpy(cp, cGuildMasterName, 10);
				cp += 10;

				iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 16, DEF_USE_ENCRYPTION);
			}
			else {
				wsprintf(cTxt2, "(O) New guild created : Name(%s)", cFileName);
				PutLogList(cTxt2);

				ZeroMemory(cTxt2, sizeof(cTxt2));
				ZeroMemory(cTxt, sizeof(cTxt));
				GetLocalTime(&SysTime);

				wsprintf(cTxt, ";Guild file - Updated %4d/%2d/%2d/%2d/%2d", SysTime.wYear, SysTime.wMonth, SysTime.wDay, SysTime.wHour, SysTime.wMinute);
				strcat(cTxt, "\n");
				strcat(cTxt, ";Just created\n\n");

				strcat(cTxt, "[GUILD-INFO]\n\n");

				strcat(cTxt, "guildmaster-name     = ");
				strcat(cTxt, cGuildMasterName);
				strcat(cTxt, "\n");

				strcat(cTxt, "guild-GUID           = ");
				wsprintf(cTxt2, "%d", dwGuildGUID);
				strcat(cTxt, cTxt2);
				strcat(cTxt, "\n");

				strcat(cTxt, "guild-location       = ");
				strcat(cTxt, cGuildLocation);
				strcat(cTxt, "\n\n");

				strcat(cTxt, "[GUILDSMAN]\n\n");

				fwrite(cTxt, 1, strlen(cTxt), pFile);

				dwp = (DWORD*)cData;
				*dwp = MSGID_RESPONSE_CREATENEWGUILD;

				wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
				*wp = DEF_MSGTYPE_CONFIRM;

				cp = (char*)(cData + DEF_INDEX2_MSGTYPE + 2);
				memcpy(cp, cGuildMasterName, 10);
				cp += 10;

				iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 16, DEF_USE_ENCRYPTION);
				fclose(pFile);
			}
		}
		switch (iRet) {
		case DEF_XSOCKEVENT_QUENEFULL:
		case DEF_XSOCKEVENT_SOCKETERROR:
		case DEF_XSOCKEVENT_CRITICALERROR:
		case DEF_XSOCKEVENT_SOCKETCLOSED:
			DeleteClient(iClientH, 0);
			delete m_pClientList[iClientH];
			m_pClientList[iClientH] = NULL;
			return;
		}
	}
}

void CWorldLog::RequestDisbandGuild(int iClientH, char* pData)
{
	char cTemp[500];
	DWORD* dwp;
	WORD* wp;
	char* cp;
	char cFileName[255], cTxt[100], cDir[100];
	char cGuildMasterName[11], cGuildName[21];
	int iRet;
	FILE* pFile;

	if (m_pClientList[iClientH] == NULL) return;
	ZeroMemory(cFileName, sizeof(cFileName));
	ZeroMemory(cTxt, sizeof(cTxt));
	ZeroMemory(cDir, sizeof(cDir));
	ZeroMemory(cTemp, sizeof(cTemp));
	ZeroMemory(cGuildMasterName, sizeof(cGuildMasterName));
	ZeroMemory(cGuildName, sizeof(cGuildName));

	cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);
	memcpy(cGuildMasterName, cp, 10);
	cp += 30;

	memcpy(cGuildName, cp, 20);
	cp += 20;

	if (G_bDBMode == TRUE) {
		SACommand com;

		try
		{
			com.setConnection(&con);
			com.setCommandText("DELETE FROM Guilds WHERE [Guild-Name] = :1");
			com.Param(1).setAsString() = cGuildName;

			com.Execute();

			long affected = com.RowsAffected();

			char cTemp[256];
			ZeroMemory(cTemp, sizeof(cTemp));
			wsprintf(cTemp, "(!) Deleted Guild From SQL: (%s)", cGuildName);

			dwp = (DWORD*)(cTemp);
			*dwp = MSGID_RESPONSE_DISBANDGUILD;

			wp = (WORD*)(cTemp + DEF_INDEX2_MSGTYPE);
			*wp = DEF_MSGTYPE_CONFIRM;

			cp = (char*)(cTemp + DEF_INDEX2_MSGTYPE + 2);
			memcpy(cp, cGuildMasterName, 10);
			cp += 10;

			iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cTemp, 16, DEF_USE_ENCRYPTION);

		}
		catch (SAException& x)
		{
			try
			{
				con.Rollback();
			}
			catch (SAException&)
			{
			}

			char cTemp[256];
			ZeroMemory(cTemp, sizeof(cTemp));
			wsprintf(cTemp, "(!!!) SQL SERVER ERROR (RequestDisbandGuild): %s", (const char*)x.ErrText());
			PutLogList(cTemp);

			dwp = (DWORD*)(cTemp);
			*dwp = MSGID_RESPONSE_DISBANDGUILD;

			wp = (WORD*)(cTemp + DEF_INDEX2_MSGTYPE);
			*wp = DEF_MSGTYPE_REJECT;

			cp = (char*)(cTemp + DEF_INDEX2_MSGTYPE + 2);
			memcpy(cp, cGuildMasterName, 10);
			cp += 10;

			iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cTemp, 16, DEF_USE_ENCRYPTION);
		}
	}
	else {
		strcat(cFileName, "Guild");
		strcat(cFileName, "\\");
		strcat(cFileName, "\\");
		wsprintf(cTxt, "AscII%d", *cGuildName);
		strcat(cFileName, cTxt);
		strcat(cDir, cFileName);
		strcat(cFileName, "\\");
		strcat(cFileName, "\\");
		strcat(cFileName, cGuildName);
		strcat(cFileName, ".txt");

		pFile = fopen(cFileName, "rt");
		if (pFile != NULL) {
			fclose(pFile);
			wsprintf(G_cTxt, "(O) Disband Guild - Deleting guild file... : Name(%s)", cFileName);
			PutLogList(G_cTxt);
			if (DeleteFile(cFileName) != NULL) {
				dwp = (DWORD*)(cTemp);
				*dwp = MSGID_RESPONSE_DISBANDGUILD;

				wp = (WORD*)(cTemp + DEF_INDEX2_MSGTYPE);
				*wp = DEF_MSGTYPE_CONFIRM;

				cp = (char*)(cTemp + DEF_INDEX2_MSGTYPE + 2);
				memcpy(cp, cGuildMasterName, 10);
				cp += 10;

				iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cTemp, 16, DEF_USE_ENCRYPTION);
			}
			else {
				dwp = (DWORD*)(cTemp);
				*dwp = MSGID_RESPONSE_DISBANDGUILD;

				wp = (WORD*)(cTemp + DEF_INDEX2_MSGTYPE);
				*wp = DEF_MSGTYPE_REJECT;

				cp = (char*)(cTemp + DEF_INDEX2_MSGTYPE + 2);
				memcpy(cp, cGuildMasterName, 10);
				cp += 10;

				iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cTemp, 16, DEF_USE_ENCRYPTION);
			}
		}
		else {
			dwp = (DWORD*)(cTemp);
			*dwp = MSGID_RESPONSE_DISBANDGUILD;

			wp = (WORD*)(cTemp + DEF_INDEX2_MSGTYPE);
			*wp = DEF_MSGTYPE_REJECT;

			cp = (char*)(cTemp + DEF_INDEX2_MSGTYPE + 2);
			memcpy(cp, cGuildMasterName, 10);
			cp += 10;

			iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cTemp, 16, DEF_USE_ENCRYPTION);
		}
	}
	switch (iRet) {
	case DEF_XSOCKEVENT_QUENEFULL:
	case DEF_XSOCKEVENT_SOCKETERROR:
	case DEF_XSOCKEVENT_CRITICALERROR:
	case DEF_XSOCKEVENT_SOCKETCLOSED:
		DeleteClient(iClientH, 0);
		delete m_pClientList[iClientH];
		m_pClientList[iClientH] = NULL;
		return;
	}
}
void CWorldLog::UpdateGuildInfoNewGuildsman(int iClientH, char *pData)
{
	// INCOMPLETE
	// hypnotoad: useless function?
/*
 char *cp;
 char cFileName[255], cTxt[30], cDir[100], cGuildMemberName[11], cGuildName[21];

	if (m_pClientList[iClientH] == NULL) return;
	ZeroMemory(cFileName, sizeof(cFileName));
	ZeroMemory(cTxt, sizeof(cTxt));
	ZeroMemory(cDir, sizeof(cDir));
	ZeroMemory(cGuildMemberName, sizeof(cGuildMemberName));
	ZeroMemory(cGuildName, sizeof(cGuildName));

	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
	
	memcpy(cGuildMemberName, cp, 10);
	cp += 10;

	memcpy(cGuildName, cp, 20);
	cp += 20;

	strcat(cFileName, "Guild");
	strcat(cFileName, "\\");
	strcat(cFileName, "\\");
	wsprintf(cTxt, "AscII%d", *cGuildName);
	strcat(cFileName, cTxt);
	strcat(cDir, cFileName);
	strcat(cFileName, "\\");
 	strcat(cFileName, "\\");
	strcat(cFileName, cGuildName);
	strcat(cFileName, ".txt");

	ZeroMemory(cTxt, sizeof(cTxt));
	wsprintf(cTxt, "guildsman-name       = %s", cGuildMemberName);
	UpdateGuildFile_NewGuildsMan(cFileName, "[GUILDSMAN]", cGuildMemberName);*/
}

void CWorldLog::UpdateGuildInfoDeleteGuildman(int iClientH, char *pData)
{
	// INCOMPLETE
	// hypnotoad: useless function?
/*
 char *cp;
 char cFileName[255], cTxt[30], cDir[100], cGuildMemberName[11], cGuildName[21];

	if (m_pClientList[iClientH] == NULL) return;
	ZeroMemory(cFileName, sizeof(cFileName));
	ZeroMemory(cTxt, sizeof(cTxt));
	ZeroMemory(cDir, sizeof(cDir));
	ZeroMemory(cGuildMemberName, sizeof(cGuildMemberName));
	ZeroMemory(cGuildName, sizeof(cGuildName));

	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
	
	memcpy(cGuildMemberName, cp, 10);
	cp += 10;

	memcpy(cGuildName, cp, 20);
	cp += 20;

	strcat(cFileName, "Guild");
	strcat(cFileName, "\\");
	strcat(cFileName, "\\");
	wsprintf(cTxt, "AscII%d", *cGuildName);
	strcat(cFileName, cTxt);
	strcat(cDir, cFileName);
	strcat(cFileName, "\\");
 	strcat(cFileName, "\\");
	strcat(cFileName, cGuildName);
	strcat(cFileName, ".txt");

	ZeroMemory(cTxt, sizeof(cTxt));
	wsprintf(cTxt, "guildsman-name       = %s", cGuildMemberName);
	UpdateGuildFile_DeleteGuildsMan(cFileName, "[GUILDSMAN]", cGuildMemberName);*/
}

int CWorldLog::OnPlayerAccountMessage(DWORD dwMsgID, char * cAccountName, char * cPassword, int iLevel, char * pData3)
{
 int *ip, i, x, iClientH, iRet;
 DWORD * dwp;
 char * cp, cData[120];
 WORD * wp;

	switch (dwMsgID) {

	case DEF_MSGACCOUNTSET_NULL:
		for (i = 0; i < DEF_MAXPLAYERACCOUNTS; i++) {
			if (m_pAccountList[i] != NULL) {
				if (strcmp(m_pAccountList[i]->cAccountName, cAccountName) == 0) {
					wsprintf(G_cTxt, "(TestLog) Set account(%s) connection status NULL...", m_pAccountList[i]->cAccountName);
					PutLogList(G_cTxt);
					delete m_pAccountList[i];
					m_pAccountList[i] = NULL;
					return 1;
				}
			}
		}

		// VAMP - fix duped chars
		/*for (i = 0; i < DEF_MAXCHARACTER; i++) {
			if (m_pCharList[i] != NULL) {
				if (strcmp(m_pCharList[i]->, cAccountName) == 0) {
					wsprintf(G_cTxt, "(TestLog) Set account(%s) connection status NULL...", m_pAccountList[i]->cAccountName);
					PutLogList(G_cTxt);
					delete m_pAccountList[i];
					m_pAccountList[i] = NULL;
					return 1;
				}
			}
		}*/
		return 0;

	case DEF_MSGACCOUNTSET_INIT:
		for (i = 0; i < DEF_MAXPLAYERACCOUNTS; i++) { //1
			if (m_pAccountList[i] != NULL) { //2
				ZeroMemory(m_pAccountList[i]->cPassword, sizeof(m_pAccountList[i]->cPassword));
				if (strcmp(m_pAccountList[i]->cAccountName, cAccountName) == 0) { //3 this is buged
					if (m_pAccountList[i]->cAccountType == 3) { //4
						strcpy(m_pAccountList[i]->cPassword, cPassword);
						m_pAccountList[i]->m_iLevel = iLevel;
						iClientH = -1;
						for (x = 0; x < DEF_MAXGAMESERVERS; x++) { //5
							if ((m_pGameList[x] != NULL) && (strcmp(m_pGameList[x]->m_cGameServerName, m_pAccountList[i]->m_cOnGameServer) == FALSE)) {//6
								iClientH = m_pGameList[x]->m_iTrackSock;
							}//6
						}//5
						if ((iClientH != -1) && (m_pClientList[iClientH] != 0)) { // 7
							ZeroMemory(cData, sizeof(cData));

							dwp = (DWORD *)(cData);
							*dwp = MSGID_REQUEST_CHECKACCOUNTPASSWORD;

							wp  = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
							*wp = DEF_MSGTYPE_CONFIRM;

							cp  = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);
							
							memcpy(cp, cAccountName, 10);
							cp += 10;

							memcpy(cp, cPassword, 10);
							cp += 10;

							ip = (int *)cp;
							*ip = iLevel;
							cp += 4;
					
							iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 30, DEF_USE_ENCRYPTION);
							switch(iRet) { // 8
							case DEF_XSOCKEVENT_QUENEFULL:
							case DEF_XSOCKEVENT_SOCKETERROR:
							case DEF_XSOCKEVENT_CRITICALERROR:
							case DEF_XSOCKEVENT_SOCKETCLOSED:
								DeleteClient(iClientH, 0);
								delete m_pClientList[iClientH];
								m_pClientList[iClientH] = NULL;
								return 0;
							}//8			
						}//7
						return 1;
					}//4
				m_pAccountList[i]->cAccountType = 1;
				m_pAccountList[i]->dwLogInTime = timeGetTime();
				wsprintf(G_cTxt, "(TestLog) Set account(%s)-test(%s)-Level(%d) connection status 1 INIT...", m_pAccountList[i]->cAccountName,m_pAccountList[i]->cPassword, iLevel);
				PutLogList(G_cTxt);
				return 1;
				}//3
			}//2
			if (m_pAccountList[i] == NULL) {
				m_pAccountList[i] = new class CAccount(cAccountName, timeGetTime());
				m_pAccountList[i]->cAccountType = 1;
				strcpy(m_pAccountList[i]->cPassword, cPassword);
				m_pAccountList[i]->m_iLevel = (int)iLevel;
				wsprintf(G_cTxt, "(TestLog) Set account(%s)-test(%s)-Level(%d) connection status 2 INIT(create & set)...", m_pAccountList[i]->cAccountName,m_pAccountList[i]->cPassword, iLevel);
				PutLogList(G_cTxt);
				m_iTotalPlayers++;
				return 1;
			}
		}//1 end loop accounts
		return 0;

	case DEF_MSGACCOUNTSET_WAIT:
		for (i = 0; i < DEF_MAXPLAYERACCOUNTS; i++) {
			if (m_pAccountList[i] != NULL) {
				if (strcmp(m_pAccountList[i]->cAccountName, cAccountName) == 0) {
					m_pAccountList[i]->cAccountType = 2;
					m_pAccountList[i]->m_iLevel = iLevel;
					m_pAccountList[i]->dwLogInTime = timeGetTime();
					wsprintf(G_cTxt, "(TestLog) Set account(%s) connection status WAIT...", cAccountName);
					PutLogList(G_cTxt);
					return 1;
				}
			}
		}
		for (i = 0; i < DEF_MAXPLAYERACCOUNTS; i++) {
			if (m_pAccountList[i] == NULL) {
				m_pAccountList[i] = new class CAccount(cAccountName, timeGetTime());
				m_pAccountList[i]->cAccountType = 2;
				m_pAccountList[i]->m_iLevel = iLevel;
				wsprintf(G_cTxt, "(TestLog) Set account(%s) connection status WAIT(create & set)...", cAccountName);
				PutLogList(G_cTxt);
				return 1;
			}
		}
		return 0;

	case DEF_MSGACCOUNTSET_PLAYING:
		for (i = 0; i < DEF_MAXPLAYERACCOUNTS; i++) {
			if (m_pAccountList[i] != NULL) {
				if (strcmp(m_pAccountList[i]->cAccountName, cAccountName) == 0) {
					m_pAccountList[i]->cAccountType = 3;
					ZeroMemory(m_pAccountList[i]->m_cOnGameServer, sizeof(m_pAccountList[i]->m_cOnGameServer));
					memcpy(m_pAccountList[i]->m_cOnGameServer, pData3, 10);
					wsprintf(G_cTxt, "(TestLog) Set account(%s) connection status PLAYING...", cAccountName);
					PutLogList(G_cTxt);
					return 1;
				}
			}
			if (m_pAccountList[i] == NULL) {
				m_pAccountList[i] = new class CAccount(cAccountName, timeGetTime());
				m_pAccountList[i]->cAccountType = 3;
				ZeroMemory(m_pAccountList[i]->m_cOnGameServer, sizeof(m_pAccountList[i]->m_cOnGameServer));
				memcpy(m_pAccountList[i]->m_cOnGameServer, pData3, 10);
				wsprintf(G_cTxt, "(TestLog) Set account(%s) connection status PLAYING(create & set)...", cAccountName);
				PutLogList(G_cTxt);
				return 2;
			}
		}//account loop
		return 0;
	}
	return 0;
}

int CWorldLog::iGetCharacterInformation(char * cAccountName, char * Password, int * iLevel)
{
 int i;

	for (i = 0; i < DEF_MAXPLAYERACCOUNTS; i++) {
		if ((m_pAccountList[i] != NULL) && (strcmp(m_pAccountList[i]->cAccountName, cAccountName) == 0)) {
			strcpy(Password, m_pAccountList[i]->cPassword);
			*iLevel = m_pAccountList[i]->m_iLevel;
			return m_pAccountList[i]->cAccountType;
		}
	}
	return 0;
}

void CWorldLog::SetAccountStatusInit(int iClientH, char *pData)
{
 char cAccountName[11], cPassword[11];
 char *cp;
 int *ip, iLevel, iRet;

	ZeroMemory(cAccountName, sizeof(cAccountName));
	ZeroMemory(cPassword, sizeof(cPassword));
	
	cp = (char *)(pData + 6);
	memcpy(cAccountName, cp, 10);
	cp += 10;

	memcpy(cPassword, cp, 10);
	cp += 10;

	ip = (int *)cp;
	iLevel = *ip;
	cp += 4;

	iRet = OnPlayerAccountMessage(DEF_MSGACCOUNTSET_INIT, cAccountName, cPassword, iLevel, 0);
}

void CWorldLog::CheckClientTimeout()
{
 int i;
 DWORD dwTime;
 char cData[100];
	char * cp;

for (i = 0; i < DEF_MAXPLAYERACCOUNTS; i++)
if(m_pAccountList[i] != NULL) {
	if(m_pAccountList[i]->Timeout < 0) break; //if timer set no wait
	if(m_pAccountList[i]->Timeout == 0) { //client timedout
			
			memcpy(cp, m_pAccountList[i]->cAccountName, 10);
			cp += 10;
			
			wsprintf(G_cTxt, "(TestLog) Account(%s) TIMEOUT - Clear conn status request to MLS...", m_pAccountList[i]->cAccountName);
			PutLogList(G_cTxt);
			
			SendEventToMLS(MSGID_REQUEST_CLEARACCOUNTSTATUS, DEF_MSGTYPE_CONFIRM, m_pAccountList[i]->cAccountName, 10, -1); //send Main account null
			OnPlayerAccountMessage(DEF_MSGACCOUNTSET_NULL, m_pAccountList[i]->cAccountName, NULL, NULL, NULL); // set account null
	}
	m_pAccountList[i]->Timeout--;
}

	G_cClientCheckCount++;
	if (G_cClientCheckCount > 10) {
		G_cClientCheckCount = 0;
		dwTime = timeGetTime();
		for (i = 0; i < DEF_MAXPLAYERACCOUNTS; i++) {
			if (m_pAccountList[i] == NULL) break;
			if ((m_pAccountList[i]->cAccountType != 1) || (m_pAccountList[i]->cAccountType != 2)) break;
			if ((dwTime - m_pAccountList[i]->dwLogInTime) <=  20000) break; // has to be over 20seconds. to continue
			
			ZeroMemory(cData, sizeof(cData));
			
			cp = (char *)(cData);
			memcpy(cp, m_pAccountList[i]->cAccountName, 10);
			cp += 10;
			
			wsprintf(G_cTxt, "(TestLog) Account(%s) TIMEOUT - Clear conn status request to MLS...", m_pAccountList[i]->cAccountName);
			PutLogList(G_cTxt);
			
			SendEventToMLS(MSGID_REQUEST_CLEARACCOUNTSTATUS, DEF_MSGTYPE_CONFIRM, m_pAccountList[i]->cAccountName, 10, -1);
			
			OnPlayerAccountMessage(DEF_MSGACCOUNTSET_NULL, m_pAccountList[i]->cAccountName, NULL, NULL, NULL);
		}
	}
}

void CWorldLog::ForceDisconnectAccount(int iClientH, char *pData)
{
 char cAccountName[11], cGameServerName[11], cData[120];
 char  * cp;
 int i, x, iRet;
 WORD *wp, wCount;
 DWORD *dwp;

	wp = (WORD *)(pData + 4);
	wCount = *wp;

	cp = (char *)(pData + 6);
	ZeroMemory(cAccountName, sizeof(cAccountName));
	memcpy(cAccountName, cp, 10);

	for (i = 0; i < DEF_MAXPLAYERACCOUNTS; i++) {
		if (m_pAccountList[i] != NULL) {
			if (strcmp(m_pAccountList[i]->cAccountName, cAccountName) == TRUE) {
				if (m_pAccountList[i]->cAccountType == 1) return;
				if (m_pAccountList[i]->cAccountType == 2) return;
				ZeroMemory(cGameServerName, sizeof(cGameServerName));
				strcpy(cGameServerName, m_pAccountList[i]->m_cOnGameServer);
				for (x = 0; x < DEF_MAXGAMESERVERS; x++) {
					if (m_pGameList[x] != NULL) {
						if (strcmp(m_pGameList[x]->m_cGameServerName, cGameServerName) == TRUE) {
							iClientH = m_pGameList[x]->m_iTrackSock;

							ZeroMemory(cData, sizeof(cData));

							dwp  = (DWORD *)cData;
							*dwp = MSGID_REQUEST_FORCEDISCONECTACCOUNT;

							wp  = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
							*wp = wCount;

							cp  = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);
							memcpy(cp, cAccountName, 10);
							cp += 10;

							wsprintf(G_cTxt, "(TestLog) Force Disconnect Account(%s) in Game Server(%s)... Count(%d)", cAccountName, cGameServerName, wCount);
							PutLogList(G_cTxt);

							iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 16, DEF_USE_ENCRYPTION);

							switch(iRet) {
							case DEF_XSOCKEVENT_QUENEFULL:
							case DEF_XSOCKEVENT_SOCKETERROR:
							case DEF_XSOCKEVENT_CRITICALERROR:
							case DEF_XSOCKEVENT_SOCKETCLOSED:
								DeleteClient(iClientH, 0);
								delete m_pClientList[iClientH];
								m_pClientList[iClientH] = NULL;
								return;
							}					
							return;
						}
					}
				}
			}
		}
	}
	ZeroMemory(cData, sizeof(cData));
	cp = (char *)cData;
	memcpy(cp, cAccountName, 10);
	cp += 10;
	wsprintf(G_cTxt, "(TestLog) Account(%s) log out request to MLS...", cAccountName);
	PutLogList(G_cTxt);
	SendEventToMLS(MSGID_REQUEST_CLEARACCOUNTSTATUS, DEF_MSGTYPE_CONFIRM, cData, 10, -1);
	
}

int CWorldLog::RequestSetAccountWaitStatus(char *pData)
{
 char cAccountName[11];
 char *cp;
 int iLevel, *ip;
 int iRet;

	ZeroMemory(cAccountName, sizeof(cAccountName));
	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
	
	memcpy(cAccountName, cp, 10);
	cp += 10;

	ip = (int *)cp;
	iLevel = *ip;
	cp += 4;

	iRet = OnPlayerAccountMessage(DEF_MSGACCOUNTSET_WAIT, cAccountName, 0, iLevel, 0);	
	return iRet;

}

void CWorldLog::RegisterWorldGameServer()
{
 char cData[1101], cTemp[1200];
 char *cp, *cp2;
 DWORD *dwp;
 int i, x;
 char cTotalMaps;

	for (i = 0; i < DEF_MAXGAMESERVERS; i++) {
		if (m_pGameList[i] != NULL) {
			ZeroMemory(cData, sizeof(cData));
			cp2 = (char *)(cData);
			cTotalMaps = 0;
			for (x = 0; x < DEF_MAXGAMESERVERSMAPS; x++) {
				if ((m_cMapList[x] != NULL) && (i == m_cMapList[x]->iIndex)) {
					memcpy(cp, m_cMapList[i]->m_cName, 10);
					cp2 += 11;
					cTotalMaps += 1;
				}
			}

			ZeroMemory(cTemp, sizeof(cTemp));
			cp = (char *)(cTemp);
			
			memcpy(cp, m_cWorldLogName, 30);
			cp += 30;

			memcpy(cp, m_pGameList[i]->m_cGameServerName, 10);
			cp += 10;

			memcpy(cp, m_pGameList[i]->m_cGameServerAddress, 16);
			cp += 16;

			dwp = (DWORD *)cp;
			*dwp = m_pGameList[i]->m_iGameServerPort;
			cp += 4;
			
			dwp = (DWORD *)cp;
			*dwp = cTotalMaps;
			cp += 4;

			memcpy(cp, cData, cTotalMaps*11);
			cp += cTotalMaps*11;

			wsprintf(G_cTxt, "(!) Game Server Registration to MLS... ( Name:%s | Addr:%s | Port:%d )", m_pGameList[i]->m_cGameServerName, m_pGameList[i]->m_cGameServerAddress, m_pGameList[i]->m_iGameServerPort);
			PutLogList(G_cTxt);

			SendEventToMLS(MSGID_REGISTER_WORLDSERVER_GAMESERVER, DEF_MSGTYPE_CONFIRM, cTemp, 64 + (cTotalMaps*11), -1);
		}
	}
}

void CWorldLog::ClientMSLConfirmed(int iClientH)
{
	m_bisMainRegistered[iClientH] = TRUE;
	wsprintf(G_cTxt, "(O) Client(%d) MLS-socket confirmed.", iClientH);
	PutLogList(G_cTxt);
}

void CWorldLog::SendServerShutdownMessage()
{
 char cData[120];

	ZeroMemory(cData, sizeof(cData));
	strcpy(cData, "A3E9D1G0U7");
	SendEventToMLS(MSGID_SENDSERVERSHUTDOWNMSG, 0, cData, 10, -1);

}

int CWorldLog::iCreateNewCharacterSQL(char * cCharacterName, char * cAccountName, char *pData, bool bGuildCheck)
{
	// REFERENCE: sub_414DB0
	// wsprintf(cTxt, "select * from CHARACTER_T where upper(cCharName) = upper('%s') and left(cCharName,1) = '%c'", cCharacterName);
	// wsprintf(cTxt, "select * from CHARACTER_T where cCharName = ('%s')", cCharacterName);
	// wsprintf(G_cTxt, "CharName(%s) have unknown ItemID (%d)", cCharacterName, ???????);
	// PutLogList(G_cTxt);
	// wsprintf(cTxt, ""select * from BANKITEM_T where CharID = %ld", ??????);
	// wsprintf(G_cTxt, "CharName(%s) have unknown BankItemID (%d)", cCharacterName, ???????);
	// PutLogList(G_cTxt);
	// wsprintf(cTxt, "%x-(6001) sql (%s)", ??????);
	// PutGameLogData(cTxt);
	// MessageBox(m_hWnd, cTxt, "DB Error", MB_OK+MB_ICONERROR);
	return -1;
}

int CWorldLog::iSaveCharacterSQL(DWORD dwCharID, char *pData)
{
	// wsprintf(cTxt, "select * from CHARACTER_T where charID = %u", dwCharID);
	// wsprintf(cTxt, "select * from SKILL_T where CharID = %ld", ?????);
	// wsprintf(cTxt, "select * from ITEM_T where CharID = %ld", ?????);
	// iGetItemNumber(cItemName);
	// wsprintf(cTxt, "Over Item Count")
	// PutGameLogData(cTxt);
	// wsprintf(cTxt, "sql(%s)-5809 (%s)", ???????, ??????);
	// PutGameLogData(cTxt);
	// wsprintf(cTxt, "Over Item Count")
	// wsprintf(cTxt, "sql(%s)-5896 (%s)", ???????, ??????);
	// wsprintf(cTxt, "select * from BANKITEM_T where CharID = %ld", ???????);
	// wsprintf(cTxt, "Over BankItem Count")
	// PutGameLogData(cTxt);
	// wsprintf(cTxt, "sql(%s)-6039 (%s)", ???????, ??????);
	// wsprintf(cTxt, "Over BankItem Count")
	// PutGameLogData(cTxt);
	// wsprintf(cTxt, "sql(%s)-6119 (%s)", ???????, ??????);
	return -1;
}

BOOL CWorldLog::bReadItemConfigFile(char *cFn)
{
 FILE * pFile;
 HANDLE hFile;
 DWORD  dwFileSize;
 char * cp, * token, cReadModeA, cReadModeB;
 char seps[] = "= \t\n";
 class CStrTok * pStrTok;
 int iItemH;

	cReadModeA = 0;
	cReadModeB = 0;
	pFile = NULL;
	hFile = CreateFileA(cFn, GENERIC_READ, NULL, NULL, OPEN_EXISTING, NULL, NULL);
	dwFileSize = GetFileSize(hFile, NULL);
	if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);
	pFile = fopen(cFn, "rt");
	if (pFile == NULL) {
		wsprintf(G_cTxt, "(X) CRITICAL ERROR! Cannot open (%s) file !", cFn);
		PutLogList(G_cTxt);
		return FALSE;
	}
	else {
		wsprintf(G_cTxt, "(!) Reading %s config file...", cFn);
		PutLogList(G_cTxt);
		cp = new char[dwFileSize+1];
		ZeroMemory(cp, dwFileSize+1);
		fread(cp, dwFileSize, 1, pFile);
		pStrTok = new class CStrTok(cp, seps);
		token = pStrTok->pGet();
		while (token != NULL) {
			if (cReadModeA != 0) {
				switch (cReadModeA) {
				case 1:
					switch (cReadModeB) {
					case 1:
						iItemH = atoi(token);
						if (m_pItemList[iItemH] != NULL) {
							wsprintf(G_cTxt, "(!!!) CRITICAL ERROR! Duplicate ItemIDnum(%d)", iItemH);
							PutLogList(G_cTxt);
							delete cp;
							delete pStrTok;
							return FALSE;
						}
						m_pItemList[iItemH] = new class CItem();
						cReadModeB = 2;
						break;

					case 2:
						ZeroMemory(m_pItemList[iItemH]->m_cItemName, sizeof(m_pItemList[iItemH]->m_cItemName));
						memcpy(m_pItemList[iItemH]->m_cItemName, token, strlen(token));
						wsprintf(G_cTxt, "(O) ItemName(%s)", token);
						//PutLogList(G_cTxt);
						cReadModeA = 0;
						cReadModeB = 0;
						break;

					default:
						break;

					}

			default:
				break;

				}
			}
			else {
				if (memcmp(token, "Item", 4) == 0)	{
					cReadModeA = 1;
					cReadModeB = 1;
				}
				if (memcmp(token, "[ENDITEMLIST]", 13) == 0) {
					cReadModeA = 2;
					cReadModeB = 2;
				}
			}
			token = pStrTok->pGet();
		}
		delete pStrTok;
		delete cp;
		fclose(pFile);
	}
	
	PutLogList(" ");
	return TRUE;
}

int CWorldLog::iGetItemNumber(char *cItemName)
{
 int i;

	for (i = 0; i < DEF_MAXITEMS; i++) {
		if (m_pItemList[i] != NULL) {
			if (strcmp(m_pItemList[i]->m_cItemName, cItemName) == 0) {
				return i;
			}
		}
	}
	return 0;
}

void CWorldLog::PutGMLogData(char * pData, DWORD dwMsgSize, BOOL bIsSave)
{
 FILE * pFile;
 char cBuffer[512];
 SYSTEMTIME SysTime;
 DWORD dwTime;

	pFile = NULL;
	if (bIsSave == 0) {
		dwTime = timeGetTime();
		if (m_cGMLogCount == 0) m_dwGMLogTime = dwTime;
		if (dwMsgSize <= 6) return;
		*(pData + dwMsgSize) = 0;
		m_cGMLogCount++;
		GetLocalTime(&SysTime);
		ZeroMemory(cBuffer, sizeof(cBuffer));
		wsprintf(cBuffer, "%04d:%02d:%02d:%02d:%02d:%02d\t", SysTime.wYear, SysTime.wMonth, SysTime.wDay, SysTime.wHour, SysTime.wMinute);
		strcat(cBuffer, pData + 6);
		strcat(cBuffer, "\n");
		strcat(m_cGMLogBuffer, cBuffer);
	}
	else {
		if (m_cGMLogCount == 0) return;
		GetLocalTime(&SysTime);
	}
	if ((m_cGMLogCount < 30) && ((dwTime - m_dwGMLogTime) <= 5000)) {
		bIsSave = TRUE;
		return;
	}
	ZeroMemory(cBuffer, sizeof(cBuffer));
	CreateDirectory("GMLogData", 0);
	wsprintf(cBuffer, "GMLogData\\GMEvents%04d%02d%02d.log", SysTime.wYear, SysTime.wMonth, SysTime.wDay);
	pFile = fopen(cBuffer, "at");
	if (pFile == NULL) return;
	fwrite(m_cGMLogBuffer, 1, strlen(m_cGMLogBuffer), pFile);
	fclose(pFile);
	m_cGMLogCount = 0;
	ZeroMemory(m_cGMLogBuffer, sizeof(m_cGMLogBuffer));

}

void CWorldLog::PutItemLogData (char * pData, DWORD dwMsgSize, BOOL bIsSave)
{
 FILE * pFile;
 char cBuffer[512];
 SYSTEMTIME SysTime;
 DWORD dwTime;

	pFile = NULL;
	if (bIsSave == 0) {
		dwTime = timeGetTime();
		if (m_cItemLogCount == 0) m_dwItemLogTime = dwTime;
		if (dwMsgSize <= 6) return;
		*(pData + dwMsgSize) = 0;
		m_cItemLogCount++;
		GetLocalTime(&SysTime);
		ZeroMemory(cBuffer, sizeof(cBuffer));
		wsprintf(cBuffer, "%04d:%02d:%02d:%02d:%02d:%02d\t", SysTime.wYear, SysTime.wMonth, SysTime.wDay, SysTime.wHour, SysTime.wMinute);
		strcat(cBuffer, pData + 6);
		strcat(cBuffer, "\n");
		strcat(m_cItemLogBuffer, cBuffer);
	}
	else {
		if (m_cItemLogCount == 0) return;
		GetLocalTime(&SysTime);
	}
	if ((m_cItemLogCount < 150) && ((dwTime - m_dwItemLogTime) <= 5000)) {
		bIsSave = TRUE;
		return;
	}
	ZeroMemory(cBuffer, sizeof(cBuffer));
	CreateDirectory("ItemLogData", 0);
	wsprintf(cBuffer, "ItemLogData\\ItemEvents%04d%02d%02d.log", SysTime.wYear, SysTime.wMonth, SysTime.wDay);
	pFile = fopen(cBuffer, "at");
	if (pFile == NULL) return;
	fwrite(m_cItemLogBuffer, 1, strlen(m_cItemLogBuffer), pFile);
	fclose(pFile);
	m_cItemLogCount = 0;
	ZeroMemory(m_cItemLogBuffer, sizeof(m_cItemLogBuffer));
}

void CWorldLog::PutCrusadeLogData(char * pData, DWORD dwMsgSize, BOOL bIsSave)
{
 FILE * pFile;
 char cBuffer[512];
 SYSTEMTIME SysTime;
 DWORD dwTime;

	pFile = NULL;
	if (bIsSave == 0) {
		dwTime = timeGetTime();
		if (m_cCrusadeLogCount == 0) m_dwItemLogTime = dwTime;
		if (dwMsgSize <= 6) return;
		*(pData + dwMsgSize) = 0;
		m_cCrusadeLogCount++;
		GetLocalTime(&SysTime);
		ZeroMemory(cBuffer, sizeof(cBuffer));
		wsprintf(cBuffer, "%04d:%02d:%02d:%02d:%02d:%02d\t", SysTime.wYear, SysTime.wMonth, SysTime.wDay, SysTime.wHour, SysTime.wMinute);
		strcat(cBuffer, pData + 6);
		strcat(cBuffer, "\n");
		strcat(m_cCrusadeLogBuffer, cBuffer);
	}
	else {
		if (m_cCrusadeLogCount == 0) return;
		GetLocalTime(&SysTime);
	}
	if ((m_cCrusadeLogCount < 100) && ((dwTime - m_dwCrusadeLogTime) <= 5000)) {
		bIsSave = TRUE;
		return;
	}
	ZeroMemory(cBuffer, sizeof(cBuffer));
	CreateDirectory("CrusadeLogData", 0);
	wsprintf(cBuffer, "CrusadeLogData\\CrusadeEvents%04d%02d%02d.log", SysTime.wYear, SysTime.wMonth, SysTime.wDay);
	pFile = fopen(cBuffer, "at");
	if (pFile == NULL) return;
	fwrite(m_cCrusadeLogBuffer, 1, strlen(m_cCrusadeLogBuffer), pFile);
	fclose(pFile);
	m_cCrusadeLogCount = 0;
	ZeroMemory(m_cCrusadeLogBuffer, sizeof(m_cCrusadeLogBuffer));
}

void CWorldLog::RequestSaveOccupyFlag(int iClientH, char *pData, char cType)
{
 /*char cFileName[256];
 FILE * pFile;
 char *cp;

	if (m_pClientList[iClientH] == NULL) return;
	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
	if (strlen(cp) <= 0) { 
		PutLogList("(!) Occupy Flag Data Error! Data size 0"); 
		return;
	}
	_mkdir("GameData");
	ZeroMemory(cFileName, sizeof(cFileName));
	strcat(cFileName, "GameData");
	strcat(cFileName, "\\");
 	strcat(cFileName, "\\");
	if (cType = 1) {
		strcat(cFileName, "AresdenOccupyFlag.Txt");
	}
	else {
		strcat(cFileName, "ElvineOccupyFlag.Txt");
	}
	WriteOccupyFlagFile(cFileName);
	pFile = fopen(cFileName, "wt");
	if (pFile == NULL) {
		wsprintf(G_cTxt, "(!) Cannot create OccupyFlag data file : Name(%s)", cFileName);
		PutLogList(G_cTxt);
	}
	else if (strlen(cp) >= DEF_MSGBUFFERSIZE) {
		wsprintf(G_cTxt, "(!) OccupyFlag data file saved : Name(%s)", cFileName);
		PutLogList(G_cTxt);
		fwrite(cp, 1, strlen(cp), pFile);
	}
	if (pFile != NULL) fclose(pFile);*/

}

void CWorldLog::CreateNewCharacterFile(char *pData, char *cFn, char * cNewCharName, char * cAccountName, char * cPassword, char cNewGender, char cNewSkin, int iTempAppr1, int iTempAppr2, int iTempAppr3, char cNewStr, char cNewVit, char cNewDex, char cNewInt, char cNewMag, char cNewChr, short sAppr1, short sAppr2, short sAppr3, short sAppr4)
{
 SYSTEMTIME SysTime;
 char cTxt[120];
 int i, iRand;

	ZeroMemory(cTxt, sizeof(cTxt));
	GetLocalTime(&SysTime);
	ZeroMemory(pData, sizeof(pData));
	strcat(pData, "[FILE-DATE]\n\n");

	wsprintf(cTxt, "file-saved-date:  Time(%d/%d/%d/%d/%d)", SysTime.wYear, SysTime.wMonth, SysTime.wDay, SysTime.wHour, SysTime.wMinute);
	strcat(pData, cTxt);
	strcat(pData, "\n\n");

	strcat(pData, "[NAME-ACCOUNT]\n\n");

	strcat(pData, "character-name     = ");
	strcat(pData, cNewCharName);
	strcat(pData, "\n");

	strcat(pData, "account-name       = ");
	strcat(pData, cAccountName);
	strcat(pData, "\n");

	strcat(pData, "account-password   = ");
	strcat(pData, cPassword);
	strcat(pData, "\n\n");

	strcat(pData, "[STATUS]\n\n");

	strcat(pData, "character-location   = NONE\n");
	strcat(pData, "character-guild-name = NONE\n");
	strcat(pData, "character-guild-rank = -1\n");
	strcat(pData, "\n");

	strcat(pData, "character-loc-map  = default\n");
	strcat(pData, "character-loc-x\t  = -1\n");
	strcat(pData, "character-loc-y\t  = -1\n\n");

	wsprintf(cTxt, "character-HP       = %d", 37);
	strcat(pData, cTxt);
	strcat(pData,"\n");

	wsprintf(cTxt, "character-MP       = %d", 27);
	strcat(pData, cTxt);
	strcat(pData,"\n");

	wsprintf(cTxt, "character-SP       = %d", 22);
	strcat(pData, cTxt);
	strcat(pData,"\n");

	strcat(pData, "character-DefenseRatio = 10\n");

	strcat(pData, "character-LEVEL    = 1\n");
	
	wsprintf(cTxt, "character-STR      = %d", cNewStr);
	strcat(pData, cTxt);
	strcat(pData,"\n");

	wsprintf(cTxt, "character-INT      = %d", cNewInt);
	strcat(pData, cTxt);
	strcat(pData,"\n");

	wsprintf(cTxt, "character-VIT      = %d", cNewVit);
	strcat(pData, cTxt);
	strcat(pData,"\n");

	wsprintf(cTxt, "character-DEX      = %d", cNewDex);
	strcat(pData, cTxt);
	strcat(pData,"\n");

	wsprintf(cTxt, "character-MAG      = %d", cNewMag);
	strcat(pData, cTxt);
	strcat(pData,"\n");

	wsprintf(cTxt, "character-CHARISMA = %d", cNewChr);
	strcat(pData, cTxt);
	strcat(pData,"\n");

	strcat(pData, "character-LUCK     = 10");
	strcat(pData,"\n");

	strcat(pData, "character-EXP      = 0");
	strcat(pData,"\n");

	strcat(pData, "character-LU_Str    = 0");
	strcat(pData,"\n");

	strcat(pData, "character-LU_Vit    = 0");
	strcat(pData,"\n");

	strcat(pData, "character-LU_Int    = 0");
	strcat(pData,"\n");

	strcat(pData, "character-LU_Mag    = 0");
	strcat(pData,"\n");

	strcat(pData, "character-LU_Char   = 0");
	strcat(pData,"\n");

	strcat(pData, "character-EK-Count  = 0");
	strcat(pData,"\n");

	strcat(pData, "character-PK-Count  = 0");
	strcat(pData,"\n");

	wsprintf(cTxt, "sex-status       = %d", cNewGender);
	strcat(pData, cTxt);
	strcat(pData,"\n");

	wsprintf(cTxt, "skin-status      = %d", cNewSkin);
	strcat(pData, cTxt);
	strcat(pData,"\n");

	wsprintf(cTxt, "hairstyle-status = %d", iTempAppr2);
	strcat(pData, cTxt);
	strcat(pData,"\n");

	wsprintf(cTxt, "haircolor-status = %d", iTempAppr3);
	strcat(pData, cTxt);
	strcat(pData,"\n");

	wsprintf(cTxt, "underwear-status = %d", iTempAppr1);
	strcat(pData, cTxt);
	strcat(pData,"\n\n");

	wsprintf(cTxt, "appr1 = %d", sAppr1);
	strcat(pData, cTxt);
	strcat(pData,"\n");

	wsprintf(cTxt, "appr2 = %d", sAppr2);
	strcat(pData, cTxt);
	strcat(pData,"\n");

	wsprintf(cTxt, "appr3 = %d", sAppr3);
	strcat(pData, cTxt);
	strcat(pData,"\n");

	wsprintf(cTxt, "appr4 = %d", sAppr4);
	strcat(pData, cTxt);
	strcat(pData,"\n\n");

	strcat(pData, "[ITEMLIST]\n\n");
	strcat(pData, "character-item = Dagger               1 0 0 0 0 0 0 0 0 300 0\n");
	strcat(pData, "character-item = RecallScroll         1 0 0 0 0 0 0 0 0 300 0\n");
	strcat(pData, "character-item = BigRedPotion         1 0 0 0 0 0 0 0 0 300 0\n");
	strcat(pData, "character-item = BigGreenPotion	     1 0 0 0 0 0 0 0 0 300 0\n");
	strcat(pData, "character-item = BigBluePotion        1 0 0 0 0 0 0 0 0 300 0\n");
	strcat(pData, "character-item = Map			         1 0 0 0 0 0 0 0 0 300 0\n");

	iRand = (rand() %16);
	switch(iRand) {
		case 1:
		case 2:
		case 3:
		case 4:
		case 7:
		case 8:
		case 15:
			strcat(pData, "character-item = WoodShield           1 0 0 0 0 0  0 0 0 300 0\n");
			break;
				
		default:
			if (cNewGender == 1) {
				wsprintf(cTxt, "character-item = KneeTrousers(M)     1 0 0 0 0 %d 0 0 0 300 0\n", iRand);
			}
			else {
				wsprintf(cTxt, "character-item = Chemise(W)	         1 0 0 0 0 %d 0 0 0 300 0\n", iRand);
			}
			strcat(pData, cTxt);
			strcat(pData,"\n");
			break;
	}
	
	strcat(pData, "[MAGIC-SKILL-MASTERY]\n\n");
	strcat(pData, "magic-mastery     = 0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000");
	strcat(pData,"\n\n");

	strcat(pData, "skill-mastery     = 0 0 0 3 20 24 0 24 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 ");
//	for (i = 0; i < 60; i++) {
		//ZeroMemory(cTxt, sizeof(cTxt));
		//wsprintf(cTxt, "0 0 0 3 20 24 0 24 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 ");
		//strcat(pData, cTxt);
//	}
	strcat(pData,"\n");

	strcat(pData, "skill-SSN     = ");
	for (i = 0; i < 60; i++) {
		strcat(pData, "0 ");
	}
	strcat(pData,"\n\n");
	
	strcat(pData, "[ITEM-EQUIP-STATUS]\n\n");
	strcat(pData, "item-equip-status = 00000110000000000000000000000000000000000000000000");
	strcat(pData,"\n\n");

	strcat(pData,"[EOF]");
	strcat(pData,"\n\n\n\n");
}

/*
void CWorldLog::VerifyCharacterIntegrity(char *cCharacterName, char *cAccountName, int *iLevel, char * cGuildName, char *cGuildRank, DWORD *dwGuildGUID)
{
 FILE * pFile;
 HANDLE hFile;
 DWORD  dwFileSize;
 char * cp, * token, cReadMode, cTotalList;
 char seps[] = "= \t\n";
 class CStrTok * pStrTok;
 char cFileName[255];
 char cDir[63];
 char cCharacterMap[11];

	ZeroMemory(cFileName, sizeof(cFileName));
	ZeroMemory(cDir, sizeof(cDir));
	strcat(cFileName, "Character");
	strcat(cFileName, "\\");
 	strcat(cFileName, "\\");
	wsprintf(cDir, "AscII%d", *cCharacterName);
	strcat(cFileName, cDir);
	strcat(cFileName, "\\");
	strcat(cFileName, "\\");
	strcat(cFileName, cCharacterName);	
	strcat(cFileName, ".txt");

	wsprintf(G_cTxt, "(TestLog) Verify Character Integrity (%s)", cFileName);
	PutLogList(G_cTxt);
	
	cTotalList  = 0;
	cReadMode	= 0;
	hFile = CreateFileA(cFileName, GENERIC_READ, NULL, NULL, OPEN_EXISTING, NULL, NULL);
	dwFileSize = GetFileSize(hFile, NULL);
	if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);
	pFile = fopen(cFileName, "rt");
	if (pFile != NULL) {
		cp = new char[dwFileSize+1];
		ZeroMemory(cp, dwFileSize+1);
		fread(cp, dwFileSize, 1, pFile);
		pStrTok = new class CStrTok(cp, seps);
		token = pStrTok->pGet();
		while (token != NULL) {
			if (cReadMode != 0) {
				switch (cReadMode) {
				case 1:
					strcpy(cCharacterMap, token);
					cReadMode = 0;
					break;

				case 2:
					strcpy(cAccountName, token);
					cReadMode = 0;
					break;

				case 3:
					*iLevel = atoi(token);
					cReadMode = 0;
					break;

				case 4:
					strcpy(cGuildName, token);
					cReadMode = 0;
					break;
					
				case 5:
					*cGuildRank = atoi(token);
					cReadMode = 0;
					break;

				case 6:
					*dwGuildGUID = atoi(token);
					cReadMode = 0;
					break;

				case 7:
					break;
				}
			}
			else {
				if (memcmp(token, "character-loc-map", 17) == 0)			cReadMode = 1;
				if (memcmp(token, "account-name", 12) == 0)					cReadMode = 2;
				if (memcmp(token, "character-LEVEL", 15) == 0)				cReadMode = 3;
				if (memcmp(token, "character-guild-name", 20) == 0)			cReadMode = 4;
				if (memcmp(token, "character-guild-rank", 20) == 0)			cReadMode = 5;
				if (memcmp(token, "character-guild-GUID", 20) == 0)			cReadMode = 6;
				if (memcmp(token, "[EOF]", 5) == 0)							cReadMode = 7;
			}
			token = pStrTok->pGet();
		}
		delete pStrTok;
		delete cp;
	}
	if (pFile != NULL) fclose(pFile);
	return;
}

void CWorldLog::VerifyGuildIntegrity(char *cGuildName, DWORD *dwGuildGUID)
{
 HANDLE hFile;
 DWORD  dwFileSize;
 char * cp, * token, cReadMode, cTotalList;
 char seps[] = "= \t\n";
 class CStrTok * pStrTok;
 char cFileName[255];
 char cDir[63];
 FILE * pFile;

	ZeroMemory(cFileName, sizeof(cFileName));
	ZeroMemory(cDir, sizeof(cDir));
	strcat(cFileName, "Guild");
	strcat(cFileName, "\\");
 	strcat(cFileName, "\\");
	wsprintf(cDir, "AscII%d", *cGuildName);
	strcat(cFileName, cDir);
	strcat(cFileName, "\\");
	strcat(cFileName, "\\");
	strcat(cFileName, cGuildName);	
	strcat(cFileName, ".txt");

	wsprintf(G_cTxt, "(TestLog) Verify Guild Integrity (%s)", cFileName);
	PutLogList(G_cTxt);
	
	cTotalList  = 0;
	cReadMode	= 0;
	hFile = CreateFileA(cFileName, GENERIC_READ, NULL, NULL, OPEN_EXISTING, NULL, NULL);
	dwFileSize = GetFileSize(hFile, NULL);
	if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);
	pFile = fopen(cFileName, "rt");
	if (pFile != NULL) {
		cp = new char[dwFileSize+1];
		ZeroMemory(cp, dwFileSize+1);
		fread(cp, dwFileSize, 1, pFile);
		pStrTok = new class CStrTok(cp, seps);
		token = pStrTok->pGet();
		while (token != NULL) {
			if (cReadMode != 0) {
				switch (cReadMode) {
				case 1:
					*dwGuildGUID = atoi(token);
					cReadMode = 0;
					break;
				}
			}
			else {
				if (memcmp(token, "guild-GUID", 10) == 0)			cReadMode = 1;
			}
			token = pStrTok->pGet();
		}
		delete pStrTok;
		delete cp;
	}
	if (pFile != NULL) fclose(pFile);
	return;
}
*/
void CWorldLog::VerifyCharacterIntegrity(char* cCharacterName, char* cAccountName, int* iLevel, char* cGuildName, char* cGuildRank, DWORD* dwGuildGUID)
{
	FILE* pFile;
	HANDLE hFile;
	DWORD  dwFileSize;
	char* cp, * token, cReadMode, cTotalList;
	char seps[] = "= \t\n";
	class CStrTok* pStrTok;
	char cFileName[255];
	char cDir[63];
	char cCharacterMap[11];

	if (G_bDBMode == TRUE)
	{
		SACommand com;

		try
		{
			com.setConnection(&con);
			com.setCommandText("SELECT     [Character-ID], [Account-ID], RTRIM([Character-Name]) AS 'Character-Name', [Character-Profile], [Character-Location], [Character-Guild-Name], [Character-Guild-GUID], [Character-Guild-Rank], [Character-Loc-Map], [Character-Loc-X], [Character-Loc-Y], [Character-HP], [Character-MP], [Character-SP], [Character-Level], [Character-Rating], [Character-Strength], [Character-Intelligence], [Character-Vitality], [Character-Dexterity], [Character-Magic], [Character-Charisma], [Character-Luck], [Character-Experience], [Character-LU-Pool], [Character-Ek-Count], [Character-Pk-Count], [Character-Reward-Gold], [Character-Down-Skill-Index], [Character-ID1], [Character-ID2], [Character-ID3], [Character-Sex], [Character-Skin], [Character-Hair-Style], [Character-Hair-Colour], [Character-Underwear], [Character-Hunger], [Character-Shutup-Time], [Character-Rating-Time], [Character-Force-Time], [Character-SP-Time], [Character-Admin-Level], [Character-Block-Date], [Character-Quest-Number], [Character-Quest-ID], [Character-Quest-Reward], [Character-Quest-Amount], [Character-Contribution], [Character-War-Contribution], [Character-Event-ID], [Character-Criticals], [Character-Fightzone-ID], [Character-Ability-Time], [Character-Lock-Map], [Character-Lock-Time], [Character-Crusade-Job], [Character-Crusade-GUID], [Character-Construct-Points], [Character-Death-Time], [Character-Party-ID], [Character-Majestics], [Character-Appr1], [Character-Appr2], [Character-Appr3], [Character-Appr4], [Character-Appr-Colour] FROM         Characters WHERE     ([Character-Name] = :1)");
			com.Param(1).setAsString() = cCharacterName;

			com.Execute();

			if (com.isResultSet())
			{
				long col_count = com.FieldCount();

				char cTest[256];
				char cTemp[120];

				while (com.FetchNext())
				{
					// char map
					ZeroMemory(cTemp, sizeof(cTemp));
					strcpy(cTemp, com.Field("Character-Loc-Map").asString());
					strcpy(cCharacterMap, cTemp);

					// char account id
					ZeroMemory(cTemp, sizeof(cTemp));
					strcpy(cTemp, com.Field("Account-ID").asString());
					strcpy(cAccountName, cTemp);

					// char level
					ZeroMemory(cTemp, sizeof(cTemp));
					strcpy(cTemp, com.Field("Character-Level").asString());
					*iLevel = atoi(cTemp);

					// char guild
					ZeroMemory(cTemp, sizeof(cTemp));
					strcpy(cTemp, com.Field("Character-Guild-Name").asString());
					strcpy(cGuildName, cTemp);

					// char guild rank
					ZeroMemory(cTemp, sizeof(cTemp));
					strcpy(cTemp, com.Field("Character-Guild-Rank").asString());
					*cGuildRank = atoi(cTemp);

					// char guild GUID
					ZeroMemory(cTemp, sizeof(cTemp));
					strcpy(cTemp, com.Field("Character-Guild-GUID").asString());
					*dwGuildGUID = atoi(cTemp);
				}
			}

			return;
		}
		catch (SAException& x)
		{
			try
			{
				con.Rollback();
			}
			catch (SAException&)
			{
			}

			char cTemp[256];
			ZeroMemory(cTemp, sizeof(cTemp));
			wsprintf(cTemp, "(!!!) SQL SERVER ERROR (iGetCharacterData): %s", (const char*)x.ErrText());
			PutLogList(cTemp);

			return;
		}
	}
	else
	{
		ZeroMemory(cFileName, sizeof(cFileName));
		ZeroMemory(cDir, sizeof(cDir));
		strcat(cFileName, "Character");
		strcat(cFileName, "\\");
		strcat(cFileName, "\\");
		wsprintf(cDir, "AscII%d", *cCharacterName);
		strcat(cFileName, cDir);
		strcat(cFileName, "\\");
		strcat(cFileName, "\\");
		strcat(cFileName, cCharacterName);
		strcat(cFileName, ".txt");

		wsprintf(G_cTxt, "(TestLog) Verify Character Integrity (%s)", cFileName);
		PutLogList(G_cTxt);

		cTotalList = 0;
		cReadMode = 0;
		hFile = CreateFileA(cFileName, GENERIC_READ, NULL, NULL, OPEN_EXISTING, NULL, NULL);
		dwFileSize = GetFileSize(hFile, NULL);
		if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);
		pFile = fopen(cFileName, "rt");
		if (pFile != NULL) {
			cp = new char[dwFileSize + 1];
			ZeroMemory(cp, dwFileSize + 1);
			fread(cp, dwFileSize, 1, pFile);
			pStrTok = new class CStrTok(cp, seps);
			token = pStrTok->pGet();
			while (token != NULL) {
				if (cReadMode != 0) {
					switch (cReadMode) {
					case 1:
						strcpy(cCharacterMap, token);
						cReadMode = 0;
						break;

					case 2:
						strcpy(cAccountName, token);
						cReadMode = 0;
						break;

					case 3:
						*iLevel = atoi(token);
						cReadMode = 0;
						break;

					case 4:
						strcpy(cGuildName, token);
						cReadMode = 0;
						break;

					case 5:
						*cGuildRank = atoi(token);
						cReadMode = 0;
						break;

					case 6:
						*dwGuildGUID = atoi(token);
						cReadMode = 0;
						break;

					case 7:
						break;
					}
				}
				else {
					if (memcmp(token, "character-loc-map", 17) == 0)			cReadMode = 1;
					if (memcmp(token, "account-name", 12) == 0)					cReadMode = 2;
					if (memcmp(token, "character-LEVEL", 15) == 0)				cReadMode = 3;
					if (memcmp(token, "character-guild-name", 20) == 0)			cReadMode = 4;
					if (memcmp(token, "character-guild-rank", 20) == 0)			cReadMode = 5;
					if (memcmp(token, "character-guild-GUID", 20) == 0)			cReadMode = 6;
					if (memcmp(token, "[EOF]", 5) == 0)							cReadMode = 7;
				}
				token = pStrTok->pGet();
			}
			delete pStrTok;
			delete cp;
			fclose(pFile);
		}
		return;
	}
}

void CWorldLog::VerifyGuildIntegrity(char* cGuildName, DWORD* dwGuildGUID)
{
	HANDLE hFile;
	DWORD  dwFileSize;
	char* cp, * token, cReadMode, cTotalList;
	char seps[] = "= \t\n";
	class CStrTok* pStrTok;
	char cFileName[255];
	char cDir[63];


	if (G_bDBMode == TRUE)
	{
		SACommand com;

		try
		{
			com.setConnection(&con);
			com.setCommandText("SELECT     [Guild-GUID] FROM         Guilds WHERE     ([Guild-Name] = :1)");
			com.Param(1).setAsString() = cGuildName;

			com.Execute();

			if (com.isResultSet())
			{
				char cTemp[120];

				while (com.FetchNext())
				{
					// guild GUID
					ZeroMemory(cTemp, sizeof(cTemp));
					strcpy(cTemp, com.Field("Guild-GUID").asString());
					*dwGuildGUID = atoi(cTemp);
				}
			}

			return;
		}
		catch (SAException& x)
		{
			try
			{
				con.Rollback();
			}
			catch (SAException&)
			{
			}

			char cTemp[256];
			ZeroMemory(cTemp, sizeof(cTemp));
			wsprintf(cTemp, "(!!!) SQL SERVER ERROR (VerifyGuildIntegrity): %s", (const char*)x.ErrText());
			PutLogList(cTemp);

			return;
		}
	}
	else
	{
		FILE* pFile;

		ZeroMemory(cFileName, sizeof(cFileName));
		ZeroMemory(cDir, sizeof(cDir));
		strcat(cFileName, "Guild");
		strcat(cFileName, "\\");
		strcat(cFileName, "\\");
		wsprintf(cDir, "AscII%d", *cGuildName);
		strcat(cFileName, cDir);
		strcat(cFileName, "\\");
		strcat(cFileName, "\\");
		strcat(cFileName, cGuildName);
		strcat(cFileName, ".txt");

		wsprintf(G_cTxt, "(TestLog) Verify Guild Integrity (%s)", cFileName);
		PutLogList(G_cTxt);

		cTotalList = 0;
		cReadMode = 0;
		hFile = CreateFileA(cFileName, GENERIC_READ, NULL, NULL, OPEN_EXISTING, NULL, NULL);
		dwFileSize = GetFileSize(hFile, NULL);
		if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);
		pFile = fopen(cFileName, "rt");
		if (pFile != NULL) {
			cp = new char[dwFileSize + 1];
			ZeroMemory(cp, dwFileSize + 1);
			fread(cp, dwFileSize, 1, pFile);
			pStrTok = new class CStrTok(cp, seps);
			token = pStrTok->pGet();
			while (token != NULL) {
				if (cReadMode != 0) {
					switch (cReadMode) {
					case 1:
						*dwGuildGUID = atoi(token);
						cReadMode = 0;
						break;
					}
				}
				else {
					if (memcmp(token, "guild-GUID", 10) == 0)			cReadMode = 1;
				}
				token = pStrTok->pGet();
			}
			delete pStrTok;
			delete cp;
			fclose(pFile);
		}
	}
	return;
}

BOOL CWorldLog::bCheckCharacterExists(char* cCharacterName)
{
	SACommand com;

	try
	{
		com.setConnection(&con);
		com.setCommandText("SELECT     COUNT([Character-ID]) FROM         Characters WHERE     ([Character-Name] = :1)");
		com.Param(1).setAsString() = cCharacterName;

		com.Execute();

		if (com.isResultSet())
		{
			while (com.FetchNext())
			{
				if (com.Field(1).asLong() > 0) return TRUE;
				else return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
	}
	catch (SAException& x)
	{
		try
		{
			con.Rollback();
		}
		catch (SAException&)
		{
		}

		char cTemp[256];
		ZeroMemory(cTemp, sizeof(cTemp));
		wsprintf(cTemp, "(!!!) SQL SERVER ERROR (bCheckCharacterExists): %s", (const char*)x.ErrText());
		PutLogList(cTemp);

		return TRUE;
	}
}

BOOL CWorldLog::bCheckGuildExists(char* cGuildName)
{
	SACommand com;

	try
	{
		com.setConnection(&con);
		com.setCommandText("SELECT     COUNT([Guild-ID]) FROM         Guilds WHERE     ([Guild-Name] = :1)");
		com.Param(1).setAsString() = cGuildName;

		com.Execute();

		if (com.isResultSet())
		{
			while (com.FetchNext())
			{
				if (com.Field(1).asLong() > 0) return TRUE;
				else return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
	}
	catch (SAException& x)
	{
		try
		{
			con.Rollback();
		}
		catch (SAException&)
		{
		}

		char cTemp[256];
		ZeroMemory(cTemp, sizeof(cTemp));
		wsprintf(cTemp, "(!!!) SQL SERVER ERROR (bCheckGuildExists): %s", (const char*)x.ErrText());
		PutLogList(cTemp);

		return TRUE;
	}
}
void CWorldLog::PutPacketLogData(DWORD dwMsgID, char *cData, DWORD dwMsgSize)
{
 FILE * pFile;
 char DbgBuffer[15000];

	pFile = fopen("PacketLog.txt", "at");
	if (pFile == NULL) return;

	if (dwMsgSize > 10000) dwMsgSize = 10000;
	int i=0, j, a;
	while(i<(int)dwMsgSize){

		wsprintf(DbgBuffer, "MSGID -> 0x%.8X\n", dwMsgID);
		fwrite(DbgBuffer, 1, strlen(DbgBuffer), pFile);
		memset(DbgBuffer,0,sizeof(DbgBuffer));
		strcpy(DbgBuffer, "DATA -> ");
		for(j=i;j < i+16 && j < (int)dwMsgSize;j++)
			wsprintf(&DbgBuffer[strlen(DbgBuffer)],"%.2X ",(unsigned char)cData[j]);

		for(a=strlen(DbgBuffer);a < 56; a+=3)
			strcat(DbgBuffer,"   ");

		strcat(DbgBuffer,"\t\t\t");
		for(j=i;j < i+16 && j < (int)dwMsgSize;j++)
			DbgBuffer[strlen(DbgBuffer)] = isprint((unsigned char)cData[j]) ? cData[j]:'.';

		fwrite(DbgBuffer, 1, strlen(DbgBuffer), pFile);
		fwrite("\r\n", 1, 2, pFile);
		i=j;
	}
	fwrite("\r\n\r\n", 1, 4, pFile);
	fclose(pFile);
}
void CWorldLog::ServerList(bool Client)
{
	int i, j, maps;
	maps = 0;
	if(Client == TRUE) {
	for (j = 0; j < DEF_MAXGAMESERVERSMAPS; j++);
		for (i = 0; i < DEF_MAXGAMESERVERS; i++);
		if (m_pGameList[i] != NULL && (m_cMapList[j] != NULL)) {
		if(m_cMapList[j]->iIndex == i) maps++;
wsprintf(G_cTxt, "Name(%s) IP(%s) Maps(%d)", m_pGameList[i]->m_cGameServerName, m_pGameList[i]->m_cGameServerAddress, maps);
PutLogServ(G_cTxt);
		}
	}
if(Client == FALSE) {
for (i = 0; i < DEF_MAXPLAYERACCOUNTS; i++);
			if (m_pAccountList[i] != NULL) {
wsprintf(G_cTxt, "Name(%s)", m_pAccountList[i]->cAccountName);
PutLogServ(G_cTxt);
			}
}
}
//chat window
void CWorldLog::ParseCommand(bool dlb, char* pMsg) {
char   seps[] = "= \t\n";
char   * token, * token2;
class  CStrTok * pStrTok;
char buff[100];
BOOL bFlag;
if (pMsg == NULL) return;
pStrTok = new class CStrTok(pMsg, seps);
token = pStrTok->pGet();
token2 = pStrTok->pGet();
bFlag = TRUE;
if (memcmp(pMsg,"",1) == 0) {
bFlag = FALSE;
wsprintf(buff,"(!!!) invalid Message");
PutLogServ(buff);
return;
}
if (memcmp(pMsg,"/kick ",6) == 0) {
bFlag = TRUE;
if(dlb == TRUE) return;
if(token == NULL) return;
int i;
char cTargetName[11];
ZeroMemory(cTargetName, sizeof(cTargetName));
	if (token != NULL) {
		// token       
		if (strlen(token) > 10) 
			 memcpy(cTargetName, token, 10);
		else memcpy(cTargetName, token, strlen(token));
	}
	if(dlb == FALSE) {
for (i = 0; i < DEF_MAXPLAYERACCOUNTS; i++) 
if ((m_pAccountList[i] != NULL) && (memcmp(m_pAccountList[i]->cAccountName, cTargetName, strlen(cTargetName)) == 0)) {	
SendEventToMLS(MSGID_REQUEST_CLEARACCOUNTSTATUS, DEF_MSGTYPE_CONFIRM, m_pAccountList[i]->cAccountName, 10, -1);
	OnPlayerAccountMessage(DEF_MSGACCOUNTSET_NULL, m_pAccountList[i]->cAccountName, NULL, NULL, NULL);
wsprintf(buff,"Player (%s) Kicked!", cTargetName);
PutLogServ(buff);
return;
}
	}
SendEventToMLS(MSGID_REQUEST_CLEARACCOUNTSTATUS, DEF_MSGTYPE_CONFIRM, cTargetName, 10, -1);
wsprintf(buff,"Player (%s) Not on WL Server(Msg Sent to ML)!", cTargetName);
PutLogServ(buff);
return;
}
if (memcmp(pMsg,"/ban ",5) == 0) {
bFlag = TRUE;
if(dlb == FALSE) return;
if(token == NULL) return;
int i;
char cTargetName[11];
ZeroMemory(cTargetName, sizeof(cTargetName));
	if (token != NULL) {
		// token       
		if (strlen(token) > 10) 
			 memcpy(cTargetName, token, 10);
		else memcpy(cTargetName, token, strlen(token));
	}
	if(dlb == TRUE) {
for (i = 0; i < DEF_MAXGAMESERVERS; i++)
if ((m_pGameList[i] != NULL) && (memcmp(m_pGameList[i]->m_cGameServerName, cTargetName, strlen(cTargetName)) == 0)) {
	//write to Wlsrver.cfg then refresh the .cfg before delete.
DeleteClient(i, 0);
wsprintf(buff,"Gserver (%s) Banned!", cTargetName);
PutLogServ(buff);
return;
}
	}
wsprintf(buff,"Gserver (%s) Not on WL Server!", cTargetName);
PutLogServ(buff);
return;
}
if ((!bFlag) && (strlen(pMsg) != 0)) {
 wsprintf(buff,"(!!!) invalid Message");
 PutLogServ(buff);
}
}
