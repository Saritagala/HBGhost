//////////////////////////////////////////////////////////////////////
//
// WorldLog.cpp: implementation of the WorldLog class.
//
//////////////////////////////////////////////////////////////////////

#include "worldlog.h"
#include <stdlib.h>
#include <stdio.h>
#include <direct.h>

#pragma warning (disable : 6385 6031 6284 4996 4244 6054 6001 6011)

extern void		PutLogList(char* cMsg);
extern void		PutLogServ(char* cMsg);
extern void		PutEventLog(char* cMsg);
extern void		PutGameLogData(char* cMsg);
extern class	XSocket* G_pListenSock;
extern class	XSocket* G_pXWorldLogSock;
extern char		G_cTxt[500];
extern char		G_cData50000[50000];
extern bool		G_bIsMainLogActive;
int			G_cClientCheckCount;
bool			G_bDBMode = true; // false

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWorldLog::CWorldLog(HWND hWnd)
{
	UINT32 dwTime;
	dwTime = timeGetTime();
	int i;

	m_hWnd = hWnd;
	for (i = 0; i < DEF_MAXCLIENTSOCK; i++) m_pClientList[i] = 0;
	for (i = 0; i < DEF_MSGQUEUESIZE; i++) m_pMsgQueue[i] = 0;
	for (i = 0; i < DEF_MAXGAMESERVERS; i++) m_pGameList[i] = 0;
	for (i = 0; i < DEF_MAXGAMESERVERSMAPS; i++) m_cMapList[i] = 0;
	for (i = 0; i < DEF_MAXMAINLOGSOCK; i++) m_pMainLogSock[i] = 0;
	for (i = 0; i < DEF_MAXMAINLOGSOCK; i++) m_bisMainRegistered[i] = false;
	for (i = 0; i < DEF_MAXPLAYERACCOUNTS; i++) m_pAccountList[i] = 0;
	for (i = 0; i < DEF_MAXITEMS; i++) m_pItemList[i] = 0;
	ZeroMemory(m_cWorldLogName, sizeof(m_cWorldLogName));
	ZeroMemory(m_pMsgBuffer, sizeof(m_pMsgBuffer + 1));
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
	m_bIsWorldRegistered = false;
	m_iTotalMainLogSock = 0;
	m_cGMLogCount = 0;
	ZeroMemory(m_cGMLogBuffer, sizeof(m_cGMLogBuffer));
	m_cItemLogCount = 0;
	ZeroMemory(m_cItemLogBuffer, sizeof(m_cItemLogBuffer));
	m_cCrusadeLogCount = 0;
	ZeroMemory(m_cCrusadeLogBuffer, sizeof(m_cCrusadeLogBuffer));
	ZeroMemory(m_sGameServerList, sizeof(m_sGameServerList));

	// customization
	m_bGameServerList = false;

	G_cClientCheckCount = 0;
	m_dwGameTime1 = dwTime;
}

CWorldLog::~CWorldLog(void)
{
	int i;

	for (i = 0; i < DEF_MAXCLIENTSOCK; i++) {
		if (m_pClientList[i] != 0) m_pClientList[i] = 0;
	}
	for (i = 0; i < DEF_MAXPLAYERACCOUNTS; i++) {
		if (m_pAccountList[i] != 0) {
			SendEventToMLS(MSGID_REQUEST_CLEARACCOUNTSTATUS, DEF_MSGTYPE_CONFIRM, m_pAccountList[i]->cAccountName, 10, -1);
			delete m_pAccountList[i];
		}
	}

	for (i = 0; i < DEF_MSGQUEUESIZE; i++) {
		if (m_pMsgQueue[i] != 0) delete m_pMsgQueue[i];
	}

	for (i = 0; i < DEF_MAXGAMESERVERS; i++) {
		if (m_pGameList[i] != 0) delete m_pGameList[i];
	}

	for (i = 0; i < DEF_MAXGAMESERVERSMAPS; i++) {
		if (m_cMapList[i] != 0) delete m_cMapList[i];
	}

	for (i = 0; i < DEF_MAXMAINLOGSOCK; i++) {
		if (m_pMainLogSock[i] != 0) delete m_pMainLogSock[i];
	}
	for (i = 0; i < DEF_MAXITEMS; i++) {
		if (m_pItemList[i] != 0) delete m_pItemList[i];
	}
}

bool CWorldLog::bInit()
{
	int i;

	if (bReadServerConfigFile("WLserver.cfg") == false) return false;

	for (i = 0; i < DEF_MAXMAINLOGSOCK; i++) {
		Sleep(200);
		m_pMainLogSock[i] = new class XSocket(m_hWnd, DEF_SERVERSOCKETBLOCKLIMIT);
		m_pMainLogSock[i]->bConnect(m_cMainLogAddress, m_iMainLogPort, WM_ONMAINLOGSOCKETEVENT + i);
		m_pMainLogSock[i]->bInitBufferSize(DEF_MSGBUFFERSIZE);
		wsprintf(G_cTxt, "(!) Try to connect Main-log-socket(%d)... Addr:%s  Port:%d", i, m_cMainLogAddress, m_iMainLogPort);
		PutLogList(G_cTxt);
	}

	m_iTotalMainLogSock = 0;
	if (bReadItemConfigFile("Item.cfg") == false) return false;
	if (bReadItemConfigFile("Item2.cfg") == false) return false;
	if (bReadItemConfigFile("Item3.cfg") == false) return false;

	// VAMP - sql connection
	try
	{
		con.Connect(
			"localhost\\sqlexpress@HBGhost",
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
		return false;
	}

	PutLogList("(!) SQL SERVER CONNECTED");

	return true;
}

bool CWorldLog::bInitSQLServer(void)
{
	// hypnotoad: uncoded sql initialization
	return true;
}

void CWorldLog::CleanupLogFiles()
{
	PutGMLogData(0, 0, true);
	PutItemLogData(0, 0, true);
	PutCrusadeLogData(0, 0, true);
}

bool CWorldLog::bReadServerConfigFile(char* cFn)
{
	FILE* pFile;
	HANDLE hFile;
	UINT32  dwFileSize;
	char* cp, * token, cReadMode, cTotalList;
	char seps[] = "= \t\n";
	class CStrTok* pStrTok;

	pFile = 0;
	cTotalList = 0;
	cReadMode = 0;
	hFile = CreateFileA(cFn, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
	dwFileSize = GetFileSize(hFile, 0);
	if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);
	pFile = fopen(cFn, "rt");
	if (pFile == 0) {
		PutLogList("(!) Cannot open configuration file.");
		return false;
	}
	else {
		PutLogList("(!) Reading configuration file...");
		cp = new char[dwFileSize + 1];
		ZeroMemory(cp, dwFileSize + 1);
		fread(cp, dwFileSize, 1, pFile);
		pStrTok = new class CStrTok(cp, seps);
		token = pStrTok->pGet();
		while (token != 0) {
			if (cReadMode != 0) {
				switch (cReadMode) {

				case 1: // gate-server-port
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

				case 5:
					strncpy(m_cWorldServerAddress, token, 15);
					wsprintf(G_cTxt, "(*) World-Log-Server Address : %s", m_cWorldServerAddress);
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
					m_bGameServerList = true;
					cReadMode = 0;
					break;
				case 9:

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
				if (memcmp(token, "world-server-address", 20) == 0)			cReadMode = 5;
				if (memcmp(token, "main-log-server-address", 23) == 0)		cReadMode = 6;
				if (memcmp(token, "main-log-server-port", 20) == 0)			cReadMode = 7;
				if (memcmp(token, "game-server-list", 16) == 0)				cReadMode = 8;
			}
			token = pStrTok->pGet();
		}
		delete pStrTok;
		delete[] cp;
		fclose(pFile);
	}
	PutLogList(" ");
	return true;
}

bool CWorldLog::bAccept(class XSocket* pXSock, bool bCheck)
{
	int i, x;
	class XSocket* pTmpSock;
	bool bFlag;

	bFlag = false;
	for (i = 1; i < DEF_MAXCLIENTSOCK; i++) { //loop 1
		if (m_pClientList[i] == 0) { //if client null
			m_pClientList[i] = new class CClient(m_hWnd);
			pXSock->bAccept(m_pClientList[i]->m_pXSock, WM_ONCLIENTSOCKETEVENT + i);
			m_pClientList[i]->m_pXSock->bInitBufferSize(DEF_MSGBUFFERSIZE);
			ZeroMemory(m_pClientList[i]->m_cIPaddress, sizeof(m_pClientList[i]->m_cIPaddress));
			m_pClientList[i]->m_pXSock->iGetPeerAddress(m_pClientList[i]->m_cIPaddress);
			if (bCheck == false) { //if client checked already
				for (x = 0; x < DEF_MAXHGSERVERLIST; x++) { //loop 2
					if (memcmp(m_sGameServerList[x << 4], m_pClientList[i]->m_cIPaddress, 16) == 0) { // if ip = accepted HG
						bFlag = true;
						m_pClientList[i]->m_cMode = 2;
					} //end if HG server matched server accepted
				} //end loop 2
				if (bFlag != true) { //if HG not accepted
					wsprintf(G_cTxt, "<%d> <%s> Unauthorized Access", i, m_pClientList[i]->m_cIPaddress);
					PutEventLog(G_cTxt);
					PutLogList(G_cTxt);
					delete m_pClientList[i];
					m_pClientList[i] = 0;
					return false;
				} //end if hg not accepted
			} //end client checked
			wsprintf(G_cTxt, "Client Accepted(%d)", i);
			PutLogList(G_cTxt);
			return true;
		} //end client == null
	} //end loop 1
	pTmpSock = new class XSocket(m_hWnd, DEF_SERVERSOCKETBLOCKLIMIT);
	pXSock->bAccept(pTmpSock, 0);
	delete pTmpSock;
	return false;
}

void CWorldLog::OnClientSubLogSocketEvent(UINT message, WPARAM wParam, LPARAM lParam) //if client not HG handler
{
	int iClientH, iRet, iTmp;

	iTmp = WM_ONCLIENTSOCKETEVENT;
	iClientH = (int)(message - iTmp);
	if (m_pClientList[iClientH] == 0) return;
	iRet = m_pClientList[iClientH]->m_pXSock->iOnSocketEvent(wParam, lParam);
	switch (iRet) {

	case DEF_XSOCKEVENT_READCOMPLETE:
		OnClientRead(iClientH);
		break;

	case DEF_XSOCKEVENT_BLOCK:
		m_iBlockCount++;
		wsprintf(G_cTxt, "<%d> Client Socket BLOCKED! (total blocks: %d)", iClientH, m_iBlockCount);
		PutLogList(G_cTxt);
		break;

	case DEF_XSOCKEVENT_CONFIRMCODENOTMATCH:
		wsprintfA(G_cTxt, "<%d> Confirmcode notmatch!", iClientH);
		PutLogList(G_cTxt);
		delete m_pClientList[iClientH];
		m_pClientList[iClientH] = 0;
		break;

	case DEF_XSOCKEVENT_MSGSIZETOOLARGE:
	case DEF_XSOCKEVENT_SOCKETERROR:
	case DEF_XSOCKEVENT_SOCKETCLOSED:
	case DEF_XSOCKEVENT_QUENEFULL:
		wsprintf(G_cTxt, "<%d> Connection Lost! Delete Client.", iClientH); //is not seting account to null on dissconnect.
		PutLogList(G_cTxt);
		DeleteClient(iClientH, 0);
		delete m_pClientList[iClientH];
		m_pClientList[iClientH] = 0;
		break;
	}
}

void CWorldLog::OnClientRead(int iClientH)
{
	char* pData, cKey;
	UINT32  dwMsgSize;

	if (m_pClientList[iClientH] == 0) return;

	pData = m_pClientList[iClientH]->m_pXSock->pGetRcvDataPointer(&dwMsgSize, &cKey);

	if (m_pClientList[iClientH]->m_cMode == 2) {
		if (bPutMsgQuene(DEF_MSGFROM_CLIENT, pData, dwMsgSize, iClientH, cKey) == false) {
			PutLogList("@@@@@@ CRITICAL ERROR in CLIENT MsgQuene!!! @@@@@@");
		}
		return;
	}
	if (bPutMsgQuene(DEF_MSGFROM_BOT, pData, dwMsgSize, iClientH, cKey) == false) {
		PutLogList("@@@@@@ CRITICAL ERROR in BOT MsgQuene!!! @@@@@@");
	}
	return;
}

void CWorldLog::OnMainSubLogSocketEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
	int iMainH, iRet, iTmp;
	char* pData, cKey;
	UINT32  dwMsgSize;

	iTmp = WM_ONMAINLOGSOCKETEVENT;
	iMainH = (int)(message - iTmp);
	if (m_pMainLogSock[iMainH] == 0) return;
	iRet = m_pMainLogSock[iMainH]->iOnSocketEvent(wParam, lParam);
	switch (iRet) {

	case DEF_XSOCKEVENT_CONNECTIONESTABLISH:
		wsprintf(G_cTxt, "(!) Main-log-socket(%d) connected.", iMainH);
		PutLogList(G_cTxt);
		m_iActiveMainLogSock++;
		if (m_bIsWorldRegistered == false) {
			m_bIsWorldRegistered = true;
			RegisterWorldServer(iMainH);
			RegisterWorldGameServer();
		}
		else {
			RegisterWorldServerSocket(iMainH);
		}
		break;

	case DEF_XSOCKEVENT_READCOMPLETE:
		pData = m_pMainLogSock[iMainH]->pGetRcvDataPointer(&dwMsgSize, &cKey);
		if (bPutMsgQuene(DEF_MSGFROM_LOGSERVER, pData, dwMsgSize, iMainH, cKey) == false) {
			PutLogList("@@@@@@ CRITICAL ERROR in LOGSERVER MsgQuene!!! @@@@@@");
		}
		break;

	case DEF_XSOCKEVENT_BLOCK:
		m_iBlockCount++;
		wsprintf(G_cTxt, "<%d> MainLog Socket BLOCKED! (total blocks: %d)", iMainH, m_iBlockCount);
		PutLogList(G_cTxt);
		break;

	case DEF_XSOCKEVENT_CONFIRMCODENOTMATCH:
		wsprintfA(G_cTxt, "<%d> Confirmcode notmatch!", iMainH);
		PutLogList(G_cTxt);
		delete m_pMainLogSock[iMainH];
		m_pMainLogSock[iMainH] = 0;
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
		m_pMainLogSock[iMainH] = new class XSocket(m_hWnd, DEF_SERVERSOCKETBLOCKLIMIT);
		m_pMainLogSock[iMainH]->bConnect(m_cMainLogAddress, m_iMainLogPort, (WM_ONMAINLOGSOCKETEVENT + iMainH));
		m_pMainLogSock[iMainH]->bInitBufferSize(DEF_MSGBUFFERSIZE);
		m_bisMainRegistered[iMainH] = false;
		if ((m_iActiveMainLogSock <= 0) && (m_bIsWorldRegistered == true)) m_bIsWorldRegistered = false;
		break;
	}
}

bool CWorldLog::bPutMsgQuene(char cFrom, char* pData, UINT32 dwMsgSize, int iIndex, char cKey)
{
	if (m_pMsgQueue[m_iQueueTail] != 0) return false;

	m_pMsgQueue[m_iQueueTail] = new class CMsg;
	if (m_pMsgQueue[m_iQueueTail] == 0) return false;

	if (m_pMsgQueue[m_iQueueTail]->bPut(cFrom, pData, dwMsgSize, iIndex, cKey) == false) return false;

	m_iQueueTail++;
	if (m_iQueueTail >= DEF_MSGQUEUESIZE) m_iQueueTail = 0;

	return true;
}

bool CWorldLog::bGetMsgQuene(char* pFrom, char* pData, UINT32* pMsgSize, int* pIndex, char* pKey)
{

	if (m_pMsgQueue[m_iQueueHead] == 0) return false;

	m_pMsgQueue[m_iQueueHead]->Get(pFrom, pData, pMsgSize, pIndex, pKey);

	delete m_pMsgQueue[m_iQueueHead];
	m_pMsgQueue[m_iQueueHead] = 0;

	m_iQueueHead++;
	if (m_iQueueHead >= DEF_MSGQUEUESIZE) m_iQueueHead = 0;

	return true;
}

void CWorldLog::OnTimer()
{
	UINT32 dwTime;
	dwTime = timeGetTime();

	MsgProcess();
	if ((dwTime - m_dwGameTime1) > 1000) {
		CheckClientTimeout();
		m_dwGameTime1 = dwTime;
	}
}

void CWorldLog::MsgProcess()
{
	char* pData = 0, cFrom, cKey;
	UINT32    dwMsgSize, * dwpMsgID;
	int     iClientH;
	char cDump[1000];

	ZeroMemory(m_pMsgBuffer, DEF_MSGBUFFERSIZE + 1);
	pData = (char*)m_pMsgBuffer;
	while (bGetMsgQuene(&cFrom, pData, &dwMsgSize, &iClientH, &cKey) == true) {
		dwpMsgID = (UINT32*)(pData + DEF_INDEX4_MSGID);
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
			break;

		case MSGID_REQUEST_SAVEELVINEOCCUPYFLAGDATA:
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

void CWorldLog::ClientRegisterGameserver(int iClientH, char* pData)
{
	char* cp, * cp2, * cMapNames;
	char cGameServerName[11];
	char cGameServerSecurity[11];
	char cGameServerIP[16];
	int iGameServerPort;
	char cGameServerLanAddress[11];
	char iTotalMaps;
	UINT16* wp;
	UINT32* dwp;
	char cData[1110];
	int i, x;
	bool bBool;
	bool bCheckHGServer;

	bBool = true;
	bCheckHGServer = false;
	if (m_pClientList[iClientH] == 0) return;
	ZeroMemory(cGameServerName, sizeof(cGameServerName));
	ZeroMemory(cGameServerIP, sizeof(cGameServerIP));
	ZeroMemory(cGameServerSecurity, sizeof(cGameServerSecurity));
	ZeroMemory(cGameServerLanAddress, sizeof(cGameServerLanAddress));

	cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);

	memcpy(cGameServerName, cp, 10);
	cp += 10;

	memcpy(cGameServerIP, cp, 16);
	cp += 16;

	wp = (UINT16*)cp;
	iGameServerPort = *wp;
	cp += 2;

	iTotalMaps = *cp;
	cp++;

	// Hypnotoad - fix preventing extasis hack from getting config files and connecting
	for (i = 0; i < DEF_MAXHGSERVERLIST; i++) {
		if (memcmp(m_sGameServerList[i << 4], cGameServerIP, 16) == 0) {
			bCheckHGServer = true;
		}
	}
	if ((bCheckHGServer == false) && (m_bGameServerList == true)) {
		wsprintf(G_cTxt, "(XXX) Unauthorized Access! Name(%s) Addr(%s) Port(%d) Maps(%d)", cGameServerName, cGameServerIP, iGameServerPort, iTotalMaps);
		PutLogList(G_cTxt);
		ResponseRegisterGameServer(iClientH, false);
		return;
	}

	for (i = 0; i < DEF_MAXGAMESERVERS; i++) {
		if (m_pGameList[i] != 0) {
			if (memcmp(m_pGameList[i]->m_cGameServerName, cGameServerName, 10) == 0) {
				ResponseRegisterGameServer(iClientH, false);
				wsprintf(G_cTxt, "(!) Game Server Registration - fail : Server-Name(%s) already exists.", cGameServerName);
				PutLogList(G_cTxt);
				return;
			}
		}
	}

	for (i = 0; i < DEF_MAXGAMESERVERS; i++) {
		if (m_pGameList[i] == 0) {
			cMapNames = (char*)cp;
			m_pGameList[i] = new class CGame(iClientH, cGameServerName, cGameServerIP, iGameServerPort, cGameServerLanAddress);
			for (x = 0; x < iTotalMaps; x++) {
				if (bClientRegisterMaps(i, cp) == 0) {
					bBool = false;
				}
				cp += 11;
			}
			if (bBool == true) {
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
					!bSendClientConfig(iClientH, "Potion.cfg")) {
					wsprintf(G_cTxt, "(X) Game Server(%s) Registration - Fail: Cannot send configuration data!", cGameServerName);
					PutLogList(G_cTxt);
					ResponseRegisterGameServer(iClientH, false);
					return;
				}

				wsprintf(G_cTxt, "(O) Game Server Registration - Success( Name: %s | Addr: %s | Port: %d )", cGameServerName, cGameServerIP, iGameServerPort);
				PutLogList(G_cTxt);
				ResponseRegisterGameServer(iClientH, true);

				ZeroMemory(cData, sizeof(cData));

				cp2 = (char*)(cData);

				memcpy(cp2, m_cWorldLogName, 30);
				cp2 += 30;

				memcpy(cp2, cGameServerName, 10);
				cp2 += 10;

				memcpy(cp2, cGameServerIP, 16);
				cp2 += 16;

				dwp = (UINT32*)cp2;
				*dwp = (UINT32)iGameServerPort;
				cp2 += 4;

				dwp = (UINT32*)cp2;
				*dwp = (UINT32)iTotalMaps;
				cp2 += 4;

				memcpy(cp2, cMapNames, iTotalMaps * 11);
				cp2 += iTotalMaps * 11;

				wsprintf(G_cTxt, "(!) Game Server Registration to MLS... ( Name: %s | Addr: %s | Port: %d )", cGameServerName, cGameServerIP, iGameServerPort);
				PutLogList(G_cTxt);

				SendEventToMLS(MSGID_REGISTER_WORLDSERVER_GAMESERVER, DEF_MSGTYPE_CONFIRM, cData, 64 + ((iTotalMaps) * 11), -1);
			}
			else {
				wsprintf(G_cTxt, "(X) Game Server(%s) Registration - Fail: Map Duplicated!", cGameServerName);
				PutLogList(G_cTxt);
				ResponseRegisterGameServer(iClientH, false);
				return;
			}
			return;
		}
		if ((m_pGameList[i] != 0) && (i >= DEF_MAXGAMESERVERS)) {
			ResponseRegisterGameServer(iClientH, false);
			PutLogList("(!) Game Server Registeration - Fail : No more Game Server can be connected.");
		}
	}
}

void CWorldLog::ResponseRegisterGameServer(int iClientH, bool bSuccesfull)
{
	int iRet;
	UINT32* dwp;
	UINT16* wp;
	char cData[20];

	if (m_pClientList[iClientH] == 0) return;
	ZeroMemory(cData, sizeof(cData));

	dwp = (UINT32*)(cData + DEF_INDEX4_MSGID);
	*dwp = MSGID_RESPONSE_REGISTERGAMESERVER;
	wp = (UINT16*)(cData + DEF_INDEX2_MSGTYPE);

	if (bSuccesfull) {
		*wp = DEF_MSGTYPE_CONFIRM;
	}
	else {
		*wp = DEF_MSGTYPE_REJECT;
	}

	iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 6, DEF_USE_ENCRYPTION);

	switch (iRet) {
	case DEF_XSOCKEVENT_QUENEFULL:
	case DEF_XSOCKEVENT_SOCKETERROR:
	case DEF_XSOCKEVENT_CRITICALERROR:
	case DEF_XSOCKEVENT_SOCKETCLOSED:
		DeleteClient(iClientH, 0);
		delete m_pClientList[iClientH];
		m_pClientList[iClientH] = 0;
		break;
	}
}

bool CWorldLog::bClientRegisterMaps(int iClientH, char* pData)
{
	int i;

	for (i = 0; i < DEF_MAXGAMESERVERS; i++) {
		if (m_cMapList[i] != 0) {
			if (memcmp(m_cMapList[i]->m_cName, pData, 10) == 0) {
				wsprintf(G_cTxt, "(X) CRITICAL ERROR! Map(%s) duplicated!", pData);
				PutLogList(G_cTxt);
				return 0;
			}
		}
	}
	for (i = 0; i < DEF_MAXGAMESERVERS; i++) {
		if (m_cMapList[i] == 0) {
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

bool CWorldLog::bSendClientConfig(int iClientH, char* cFile)
{
	UINT32 dwMsgID; 
	DWORD lpNumberOfBytesRead;
	UINT32* dwp;
	UINT16* wp;
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
	else {
		return false;
	}

	HANDLE hFile = CreateFile(cFile, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
	UINT32 dwFileSize = GetFileSize(hFile, 0);
	if (dwFileSize == -1) {
		wsprintf(G_cTxt, "(X) CRITICAL ERROR! Cannot open configuration file(%s)!", cFile);
		PutLogList(cFile);
		return false;
	}

	ZeroMemory(G_cData50000, sizeof(G_cData50000));

	wsprintf(G_cTxt, "(!) Reading %s configuration file...", cFile);
	PutLogList(G_cTxt);
	SetFilePointer(hFile, 0, 0, FILE_BEGIN);

	ReadFile(hFile, G_cData50000 + 6, dwFileSize, &lpNumberOfBytesRead, 0);
	CloseHandle(hFile);

	dwp = (UINT32*)(G_cData50000);
	*dwp = dwMsgID;

	wp = (UINT16*)(G_cData50000 + DEF_INDEX2_MSGTYPE);
	*wp = DEF_MSGTYPE_CONFIRM;

	iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(G_cData50000, dwFileSize + 8, DEF_USE_ENCRYPTION);

	switch (iRet) {
	case DEF_XSOCKEVENT_QUENEFULL:
	case DEF_XSOCKEVENT_SOCKETERROR:
	case DEF_XSOCKEVENT_CRITICALERROR:
	case DEF_XSOCKEVENT_SOCKETCLOSED:
		wsprintf(G_cTxt, "(X) Cannot send configuration file(%s) contents to GameServer!", cFile);
		PutLogList(G_cTxt);
		DeleteClient(iClientH, 0);
		delete m_pClientList[iClientH];
		m_pClientList[iClientH] = 0;
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
	char* cp;
	int i, j;

	if ((Mod == 0) || (Mod < 0)) {
		if (m_pClientList[iClientH] == 0) return;
		for (i = 0; i < DEF_MAXGAMESERVERS; i++) {
			if ((m_pGameList[i] != 0) && (m_pGameList[i]->m_iTrackSock == iClientH)) {
				wsprintf(G_cTxt, "(!) Game server connection lost! (%s)", m_pGameList[i]->m_cGameServerName);
				PutLogList(G_cTxt);

				ZeroMemory(cData, sizeof(cData));

				cp = (char*)cData;
				memcpy(cp, m_cWorldLogName, 30);
				cp += 30;

				memcpy(cp, m_pGameList[i]->m_cGameServerName, 10);
				cp += 10;

				SendEventToMLS(MSGID_REQUEST_REMOVEGAMESERVER, DEF_MSGTYPE_CONFIRM, cData, 40, -1);

				for (j = 0; j < DEF_MAXPLAYERACCOUNTS; j++) {
					if ((m_pAccountList[j] != 0) && (strcmp(m_pAccountList[j]->m_cOnGameServer, m_pGameList[i]->m_cGameServerName) == 0)) {

						ZeroMemory(cData, sizeof(cData));

						cp = (char*)cData;
						memcpy(cp, m_pAccountList[j]->cAccountName, 10);
						cp += 10;

						wsprintf(G_cTxt, "(TestLog) Account(%s) log out request to MLS...", m_pAccountList[j]->cAccountName);
						PutLogList(G_cTxt);

						SendEventToMLS(MSGID_REQUEST_CLEARACCOUNTSTATUS, DEF_MSGTYPE_CONFIRM, cData, 10, -1);
						SendEventToMLS(MSGID_REQUEST_CLEARACCOUNTSTATUS, DEF_MSGTYPE_CONFIRM, m_pAccountList[j]->cAccountName, 10, -1);
						OnPlayerAccountMessage(DEF_MSGACCOUNTSET_NULL, m_pAccountList[j]->cAccountName, 0, 0, 0);
					}
				}//account loop

				for (j = 0; j < DEF_MAXGAMESERVERSMAPS; j++) {
					if ((m_cMapList[j] != 0) && (m_cMapList[j]->iIndex == i)) {
						wsprintf(G_cTxt, "(!) Delete map (%s)", m_cMapList[j]->m_cName);
						PutLogList(G_cTxt);
						delete m_cMapList[j];
						m_cMapList[j] = 0;
					}
				}//map loop
				delete m_pGameList[i];
				m_pGameList[i] = 0;
			}
		}//mapserver loop
	}//mod 0
	if ((Mod == 1) || (Mod < 0)) {
		if (m_pAccountList[iClientH] == 0) return;
		wsprintf(G_cTxt, "(TestLog) Account(%s) log out request to MLS(Delete)...", m_pAccountList[iClientH]->cAccountName);
		PutLogList(G_cTxt);
		SendEventToMLS(MSGID_REQUEST_CLEARACCOUNTSTATUS, DEF_MSGTYPE_CONFIRM, m_pAccountList[iClientH]->cAccountName, 10, -1);
		OnPlayerAccountMessage(DEF_MSGACCOUNTSET_NULL, m_pAccountList[iClientH]->cAccountName, 0, 0, 0);
	}//Mod 1 for accounts
}

void CWorldLog::ShutdownGameServer(int iClientH)
{
	int i;

	for (i = 0; i < DEF_MAXGAMESERVERS; i++) {
		if ((m_pGameList[i] != 0) && (m_pGameList[i]->m_iTrackSock == iClientH)) {
			m_pGameList[i]->m_cGameServerMode = 3;
		}
	}
}

void CWorldLog::EnterGameConfirm(int iClientH, char* pData)
{
	char cGameServerName[11], cCharacterName[11], cAccountName[11], cAddress[16], cAccountPass[16];
	char cTemp[100];
	char cCharNameCheck[11];
	int iLevel, iCheckLevel, i;
	char* cp;
	int* ip, iRet;
	UINT32* dwp;
	UINT16* wp;

	if (m_pClientList[iClientH] == 0) return;

	ZeroMemory(cAccountName, sizeof(cAccountName));
	ZeroMemory(cCharacterName, sizeof(cCharacterName));
	ZeroMemory(cGameServerName, sizeof(cGameServerName));
	ZeroMemory(cAddress, sizeof(cAddress));
	ZeroMemory(cCharNameCheck, sizeof(cCharNameCheck));
	ZeroMemory(cTemp, sizeof(cTemp));
	ZeroMemory(cAccountPass, sizeof(cAccountPass));

	cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);

	memcpy(cAccountName, cp, 10);
	cp += 10;

	memcpy(cCharacterName, cp, 10); //password
	cp += 10;

	memcpy(cGameServerName, cp, 10);
	cp += 10;

	memcpy(cAddress, cp, 16);
	cp += 16;

	ip = (int*)cp;
	iLevel = *ip;
	cp += 4;

	switch (iGetCharacterInformation(cAccountName, cCharNameCheck, &iCheckLevel)) { // get real acconts pass
	case 1:
	case 2:
		if ((memcmp(cCharacterName, cCharNameCheck, 10) != 0) || (iCheckLevel != iLevel)) {
			wsprintf(G_cTxt, "(TestLog) ERROR! Account(%s) Password or level mismatch. Request Game Server to disconnect without data save...", cAccountName);
			PutLogList(G_cTxt);
			wsprintf(G_cTxt, "(TestLog) %s - %s, %d - %d", cCharacterName, cCharNameCheck, iLevel, iCheckLevel);
			PutLogList(G_cTxt);
			dwp = (UINT32*)(cTemp);
			*dwp = MSGID_REQUEST_FORCEDISCONECTACCOUNT;
			wp = (UINT16*)(cTemp + DEF_INDEX2_MSGTYPE);
			*wp = DEF_MSGTYPE_CONFIRM;
			cp = (char*)(cTemp + DEF_INDEX2_MSGTYPE + 2);
			memcpy(cp, cAccountName, 10);
			cp += 10;
			iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cTemp, 16, DEF_USE_ENCRYPTION);
			switch (iRet) {
			case DEF_XSOCKEVENT_QUENEFULL:
			case DEF_XSOCKEVENT_SOCKETERROR:
			case DEF_XSOCKEVENT_CRITICALERROR:
			case DEF_XSOCKEVENT_SOCKETCLOSED:
				DeleteClient(iClientH, 0);
				delete m_pClientList[iClientH];
				m_pClientList[iClientH] = 0;
				break;
			}
			return;
		}
		break;
	}
	//set account timeout to -1
	for (i = 0; i < DEF_MAXPLAYERACCOUNTS; i++)
		if ((m_pAccountList[i] != 0) && (strcmp(m_pAccountList[i]->cAccountName, cAccountName) == 0)) m_pAccountList[i]->Timeout = -1;

	OnPlayerAccountMessage(DEF_MSGACCOUNTSET_PLAYING, cAccountName, 0, 0, cGameServerName);
}

void CWorldLog::RequestPlayerData(int iClientH, char* pData)
{
	char cCharacterName[11], cAccountName[11];
	char* cp, * cp2;
	
	UINT32* dwp;
	UINT16* wp;
	char cVerifyAccountName[11];
	char cGuildName[21], cGuildRank;
	int iVerifyLevel, iCharacterDBid;
	
	int iRet, iSize;
	UINT32 dwVerifyGuildGUID, dwGuildGUID, dwTime;
	bool bGuildCheck;

	bGuildCheck = false;
	if (m_pClientList[iClientH] == 0) return;

	ZeroMemory(G_cData50000, sizeof(G_cData50000));
	ZeroMemory(cCharacterName, sizeof(cCharacterName));
	ZeroMemory(cAccountName, sizeof(cAccountName));

	dwTime = timeGetTime();

	cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);

	memcpy(cCharacterName, cp, 10);
	cp += 10;

	memcpy(cAccountName, cp, 10);
	cp += 10;

	if (strlen(cCharacterName) == 0) { PutLogList("(X) CharName 0!"); return; }
	if (strlen(cAccountName) == 0) { PutLogList("(X) AccountName 0!"); return; }

	int accDBID = iGetAccountDatabaseID(cAccountName);


	if (G_bDBMode == true) {
		ZeroMemory(G_cData50000, sizeof(G_cData50000));
		iSize = -1;

		cp2 = new char[50000];
		ZeroMemory(cp2, 50000);

		SACommand com, com2;

		try
		{
			com.setConnection(&con);
			com.setCommandText("SELECT [Character-Penalty-Year], [Character-Penalty-Month], [Character-Penalty-Day], [Character-ID], RTRIM([Character-Name]) AS 'Character-Name', [Character-Location], [Character-Guild-Name], [Character-Guild-GUID], [Character-Guild-Rank], [Character-Loc-Map], [Character-Loc-X], [Character-Loc-Y], [Character-HP], [Character-MP], [Character-SP], [Character-Level], [Character-Rating], [Character-Strength], [Character-Intelligence], [Character-Vitality], [Character-Dexterity], [Character-Magic], [Character-Charisma], [Character-Luck], [Character-Experience], [Character-LU-Pool], [Character-Ek-Count], [Character-Pk-Count], [Character-Reward-Gold], [Character-ID1], [Character-ID2], [Character-ID3], [Character-Sex], [Character-Skin], [Character-Hair-Style], [Character-Hair-Colour], [Character-Underwear], [Character-Hunger], [Character-Shutup-Time], [Character-Rating-Time], [Character-Force-Time], [Character-SP-Time], [Character-Admin-Level], [Character-Quest-Number1], [Character-Quest-ID1], [Character-Quest-Count1], [Character-Quest-Reward1], [Character-Quest-Amount1], [Character-Quest-Completed1], [Character-Quest-Number2], [Character-Quest-ID2], [Character-Quest-Count2], [Character-Quest-Reward2], [Character-Quest-Amount2], [Character-Quest-Completed2], [Character-Quest-Number3], [Character-Quest-ID3], [Character-Quest-Count3], [Character-Quest-Reward3], [Character-Quest-Amount3], [Character-Quest-Completed3], [Character-Quest-Number4], [Character-Quest-ID4], [Character-Quest-Count4], [Character-Quest-Reward4], [Character-Quest-Amount4], [Character-Quest-Completed4], [Character-Quest-Number5], [Character-Quest-ID5], [Character-Quest-Count5], [Character-Quest-Reward5], [Character-Quest-Amount5], [Character-Quest-Completed5], [Character-Contribution], [Character-War-Contribution], [Character-Event-ID], [Character-Criticals], [Character-Ability-Time], [Character-Lock-Map], [Character-Lock-Time], [Character-Crusade-Job], [Character-Crusade-GUID], [Character-Construct-Points], [Character-Death-Time], [Character-Party-ID], [Character-Majestics], [Character-Appr1], [Character-Appr2], [Character-Appr3], [Character-Appr4], [Character-Appr-Colour], [Character-Magic-Mastery], [Character-Skill-Mastery], [Character-Skill-SSN], [Character-Equip-Status], [Character-Item-Pos-X], [Character-Item-Pos-Y], [Character-Deaths], [Character-TotalDGKills], [Character-DGPoints], [Character-TotalDGDeaths], [Character-DGKills], [Character-Max-Ek], [Character-Heldenian-GUID], [Character-Wanted-Level], [Character-Coin-Points], [Character-Team], [Character-Class] FROM Characters WHERE [Character-Name] = :1 AND [Account-ID] = :2");
			com.Param(1).setAsString() = cCharacterName;
			com.Param(2).setAsLong() = accDBID;

			com.Execute();

			if (com.isResultSet())
			{
				char cTemp[500];

				strcat(cp2, "  ");
				iSize += 2;

				while (com.FetchNext())
				{
					iCharacterDBid = com.Field("Character-ID").asLong();

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-dbid = %d", iCharacterDBid);
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-loc-map = %s", com.Field("Character-Loc-Map").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-loc-x = %d", com.Field("Character-Loc-X").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-loc-y = %d", com.Field("Character-Loc-Y").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "sex-status = %d", com.Field("Character-Sex").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "skin-status = %d", com.Field("Character-Skin").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "hairstyle-status = %d", com.Field("Character-Hair-Style").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "haircolor-status = %d", com.Field("Character-Hair-Colour").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "underwear-status = %d", com.Field("Character-Underwear").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-guild-name = %s", com.Field("Character-Guild-Name").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-guild-rank = %d", com.Field("Character-Guild-Rank").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-HP = %d", com.Field("Character-HP").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-MP = %d", com.Field("Character-MP").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-SP = %d", com.Field("Character-SP").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-LEVEL = %d", com.Field("Character-Level").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-STR = %d", com.Field("Character-Strength").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-VIT = %d", com.Field("Character-Vitality").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-DEX = %d", com.Field("Character-Dexterity").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-INT = %d", com.Field("Character-Intelligence").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-MAG = %d", com.Field("Character-Magic").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-CHARISMA = %d", com.Field("Character-Charisma").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-LUCK = %d", com.Field("Character-Luck").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-EXP = %d", com.Field("Character-Experience").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-location = %s", com.Field("Character-Location").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "penalty-block-date = %d %d %d", com.Field("Character-Penalty-Year").asLong(), com.Field("Character-Penalty-Month").asLong(), com.Field("Character-Penalty-Day").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-LU_Pool = %d", com.Field("Character-LU-Pool").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-EK-Count = %d", com.Field("Character-Ek-Count").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-PK-Count = %d", com.Field("Character-Pk-Count").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-reward-gold = %d", com.Field("Character-Reward-Gold").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "hunger-status = %d", com.Field("Character-Hunger").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "admin-user-level = %d", com.Field("Character-Admin-Level").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "timeleft-shutup = %d", com.Field("Character-Shutup-Time").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "timeleft-rating = %d", com.Field("Character-Rating-Time").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-RATING = %d", com.Field("Character-Rating").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-guild-GUID = %d", com.Field("Character-Guild-GUID").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-IDnum1 = %d", com.Field("Character-ID1").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-IDnum2 = %d", com.Field("Character-ID2").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-IDnum3 = %d", com.Field("Character-ID3").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-quest-number = %d %d %d %d %d", com.Field("Character-Quest-Number1").asLong(), com.Field("Character-Quest-Number2").asLong(), com.Field("Character-Quest-Number3").asLong(), com.Field("Character-Quest-Number4").asLong(), com.Field("Character-Quest-Number5").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-quest-ID = %d %d %d %d %d", com.Field("Character-Quest-ID1").asLong(), com.Field("Character-Quest-ID2").asLong(), com.Field("Character-Quest-ID3").asLong(), com.Field("Character-Quest-ID4").asLong(), com.Field("Character-Quest-ID5").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "current-quest-count = %d %d %d %d %d", com.Field("Character-Quest-Count1").asLong(), com.Field("Character-Quest-Count2").asLong(), com.Field("Character-Quest-Count3").asLong(), com.Field("Character-Quest-Count4").asLong(), com.Field("Character-Quest-Count5").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "quest-reward-type = %d %d %d %d %d", com.Field("Character-Quest-Reward1").asLong(), com.Field("Character-Quest-Reward2").asLong(), com.Field("Character-Quest-Reward3").asLong(), com.Field("Character-Quest-Reward4").asLong(), com.Field("Character-Quest-Reward5").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "quest-reward-amount = %d %d %d %d %d", com.Field("Character-Quest-Amount1").asLong(), com.Field("Character-Quest-Amount2").asLong(), com.Field("Character-Quest-Amount3").asLong(), com.Field("Character-Quest-Amount4").asLong(), com.Field("Character-Quest-Amount5").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-quest-completed = %d %d %d %d %d", com.Field("Character-Quest-Completed1").asLong(), com.Field("Character-Quest-Completed2").asLong(), com.Field("Character-Quest-Completed3").asLong(), com.Field("Character-Quest-Completed4").asLong(), com.Field("Character-Quest-Completed5").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-contribution = %d", com.Field("Character-Contribution").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "timeleft-force-recall = %d", com.Field("Character-Force-Time").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "timeleft-firm-staminar = %d", com.Field("Character-SP-Time").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "special-event-id = %d", com.Field("Character-Event-ID").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "super-attack-left = %d", com.Field("Character-Criticals").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "special-ability-time = %d", com.Field("Character-Ability-Time").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-war-contribution = %d", com.Field("Character-War-Contribution").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "locked-map-name = %s", com.Field("Character-Lock-Map").asString());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "locked-map-time = %d", com.Field("Character-Lock-Time").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "crusade-job = %d", com.Field("Character-Crusade-Job").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "construct-point = %d", com.Field("Character-Construct-Points").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "crusade-GUID = %d", com.Field("Character-Crusade-GUID").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "dead-penalty-time = %d", com.Field("Character-Death-Time").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "party-id = %d", com.Field("Character-Party-ID").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "gizon-item-upgrade-left = %d", com.Field("Character-Majestics").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-Deaths = %d", com.Field("Character-Deaths").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-TotalDGKills = %d", com.Field("Character-TotalDGKills").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-DGPoints = %d", com.Field("Character-DGPoints").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-TotalDGDeaths = %d", com.Field("Character-TotalDGDeaths").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-DGKills = %d", com.Field("Character-DGKills").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "max-ek = %d", com.Field("Character-Max-Ek").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "heldenian-GUID = %d", com.Field("Character-Heldenian-GUID").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-wanted-level = %d", com.Field("Character-Wanted-Level").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "coin-points = %d", com.Field("Character-Coin-Points").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-team = %d", com.Field("Character-Team").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "character-class = %d", com.Field("Character-Class").asLong());
					strcat(cp2, cTemp);
					strcat(cp2, "  ");
					iSize += (strlen(cTemp) + 2);

					com2.setConnection(&con);
					com2.setCommandText("SELECT * FROM CharItems WHERE [Character-ID] = :1 ORDER BY [Item-ID]");
					com2.Param(1).setAsLong() = iCharacterDBid;

					com2.Execute();

					if (com2.isResultSet())
					{
						int col_count = com2.FieldCount();

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

					com2.setCommandText("SELECT * FROM BankItems WHERE [Account-ID] = :1 ORDER BY [Item-ID]");
					com2.Param(1).setAsLong() = accDBID;

					com2.Execute();

					if (com2.isResultSet())
					{
						int col_count = com2.FieldCount();

						char cTemp3[500];

						while (com2.FetchNext())
						{
							ZeroMemory(cTemp3, sizeof(cTemp3));
							strcpy(cTemp3, "character-bank-item = ");
							for (int k = 3; k < col_count + 1; k++)
							{
								strcat(cTemp3, com2.Field(k).asString());
								strcat(cTemp3, " ");
							}

							strcat(cp2, cTemp3);
							strcat(cp2, "  ");
							iSize += (strlen(cTemp3) + 2);
						}
					}

					com2.Close();

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
					wsprintf(cTemp, "skill-SSN = %s", com.Field("Character-Skill-SSN").asString());
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

				com.Close();

				// end string
				strcat(cp2, "[EOF]");
				iSize += 3;
			}
			else
			{
				wsprintf(G_cTxt, "(X) RequestPlayerData Error! Account(%s, %s)", cAccountName, cVerifyAccountName);
				PutLogList(G_cTxt);

				dwp = (UINT32*)(G_cData50000);
				*dwp = MSGID_RESPONSE_PLAYERDATA;

				wp = (UINT16*)(G_cData50000 + DEF_INDEX2_MSGTYPE);
				*wp = DEF_MSGTYPE_REJECT;

				cp = (char*)(G_cData50000 + DEF_INDEX2_MSGTYPE + 2);
				memcpy(cp, cCharacterName, 10);
				cp += 10;
				iSize += 10;

				iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(G_cData50000, 16, DEF_USE_ENCRYPTION);

				delete[] cp2;

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

			dwp = (UINT32*)(G_cData50000);
			*dwp = MSGID_RESPONSE_PLAYERDATA;

			wp = (UINT16*)(G_cData50000 + DEF_INDEX2_MSGTYPE);
			*wp = DEF_MSGTYPE_CONFIRM;

			cp = (char*)(G_cData50000 + DEF_INDEX2_MSGTYPE + 2);
			memcpy(cp, cCharacterName, 10);
			cp += 10;

			*cp = 2;
			cp++;

			if (cGuildRank >= 0) {
				VerifyGuildIntegrity(cGuildName, &dwVerifyGuildGUID);
				if (dwVerifyGuildGUID == dwGuildGUID) {
					bGuildCheck = true;
				}
			}

			*cp = bGuildCheck;
			cp++;

			memcpy(cp, cp2, iSize + 1);
			cp += iSize + 1;

			iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(G_cData50000, iSize + 19, DEF_USE_ENCRYPTION);

			UpdateLastLoginTime(iCharacterDBid);
		}
		else {
			wsprintf(G_cTxt, "(X) RequestPlayerData Error! Account(%s, %s)", cAccountName, cVerifyAccountName);
			PutLogList(G_cTxt);

			dwp = (UINT32*)(G_cData50000);
			*dwp = MSGID_RESPONSE_PLAYERDATA;

			wp = (UINT16*)(G_cData50000 + DEF_INDEX2_MSGTYPE);
			*wp = DEF_MSGTYPE_REJECT;

			cp = (char*)(G_cData50000 + DEF_INDEX2_MSGTYPE + 2);
			memcpy(cp, cCharacterName, 10);
			cp += 10;

			iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(G_cData50000, 16, DEF_USE_ENCRYPTION);
		}

		delete[] cp2;
	}

	switch (iRet) {
	case DEF_XSOCKEVENT_QUENEFULL:
	case DEF_XSOCKEVENT_SOCKETERROR:
	case DEF_XSOCKEVENT_CRITICALERROR:
	case DEF_XSOCKEVENT_SOCKETCLOSED:
		DeleteClient(iClientH, 0);
		delete m_pClientList[iClientH];
		m_pClientList[iClientH] = 0;
		break;
	}
}
bool CWorldLog::bVerifyCharacterGuildSQL(char* pData, UINT32 dwGuildGUID)
{
	return true;
}

void CWorldLog::SendEventToMLS(UINT32 dwMsgID, UINT16 wMsgType, char* pData, UINT32 dwMsgSize, int iMainH)
{
	int iRet, i;
	UINT32* dwp;
	char* cp;
	UINT16* wp;

	if (iMainH == -1) {
		for (i = 0; i < DEF_MAXMAINLOGSOCK; i++) {
			if (m_pMainLogSock[m_iCurMainLogSockIndex] != 0) break;
			m_iCurMainLogSockIndex++;
			if (m_iCurMainLogSockIndex >= 10) m_iCurMainLogSockIndex = 0;
		}
	}
	else m_iCurMainLogSockIndex = iMainH;

	ZeroMemory(G_cData50000, sizeof(G_cData50000));

	dwp = (UINT32*)(G_cData50000 + DEF_INDEX4_MSGID);
	*dwp = dwMsgID;
	wp = (UINT16*)(G_cData50000 + DEF_INDEX2_MSGTYPE);
	*wp = wMsgType;

	cp = (char*)(G_cData50000 + DEF_INDEX2_MSGTYPE + 2);

	memcpy((char*)cp, pData, dwMsgSize);

	if ((m_bisMainRegistered[m_iCurMainLogSockIndex] == true) || (iMainH != -1)) {
		iRet = m_pMainLogSock[m_iCurMainLogSockIndex]->iSendMsg(G_cData50000, dwMsgSize + 6, DEF_USE_ENCRYPTION);
		switch (iRet) {
		case DEF_XSOCKEVENT_QUENEFULL:
		case DEF_XSOCKEVENT_SOCKETERROR:
		case DEF_XSOCKEVENT_CRITICALERROR:
		case DEF_XSOCKEVENT_SOCKETCLOSED:
			wsprintf(G_cTxt, "(X) Main-log-socket(%d) send error!", m_iCurMainLogSockIndex);
			PutLogList(G_cTxt);
			delete m_pMainLogSock[m_iCurMainLogSockIndex];
			m_pMainLogSock[m_iCurMainLogSockIndex] = 0;
			m_bisMainRegistered[m_iCurMainLogSockIndex] = false;
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
	}
}

void CWorldLog::RegisterWorldServer(int iMainH)
{
	char cData[100];
	UINT32* dwp;
	char* cp;

	ZeroMemory(cData, sizeof(cData));

	cp = (char*)cData;
	memcpy(cp, m_cWorldLogName, 30);
	cp += 30;

	memcpy(cp, m_cWorldServerAddress, 16);
	cp += 16;

	dwp = (UINT32*)cp;
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
	UINT32* dwp;
	char* cp;

	ZeroMemory(cData, sizeof(cData));

	cp = (char*)cData;
	memcpy(cp, m_cWorldLogName, 30);
	cp += 30;

	memcpy(cp, m_cWorldServerAddress, 16);
	cp += 16;

	dwp = (UINT32*)cp;
	*dwp = m_iWorldServerPort;
	cp += 4;

	SendEventToMLS(MSGID_REGISTER_WORLDSERVERSOCKET, DEF_MSGTYPE_CONFIRM, cData, 50, iMainH);

	wsprintf(G_cTxt, "(!) Try to register world server socket(%d) to MLS...", iMainH);

	PutLogList(G_cTxt);
}

void CWorldLog::CharInfoList(int iClientH, char* pData)
{
	short sAppr1, sAppr2, sAppr3, sAppr4;
	int* ip, iSpace1, iSpace2, i, iLevel, iStr, iVit, iInt, iDex, iMag, iCharisma, iApprColor, iSaveYear, iSaveMonth, iSaveDay, iSaveHour, iSaveMinute;
	char* cp, * cp2, cData[2000], cAccountName[11], cInfo[110], cMapName[256], cCharName[11], cSex, cSkin, cTotalChar;
	UINT32* dwp, dwCharID, iExp;
	UINT16* wp;

	ZeroMemory(cData, sizeof(cData));
	ZeroMemory(cAccountName, sizeof(cAccountName));
	ZeroMemory(cInfo, sizeof(cInfo));

	cp2 = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);

	memcpy(cAccountName, cp2, 10);

	dwp = (UINT32*)(pData + 16);
	dwCharID = *dwp;

	wsprintf(G_cTxt, "(TestLog) Getting character data in account(%s)-ID(%d)...", cAccountName, dwCharID);
	PutLogList(G_cTxt);

	memcpy(cData, cAccountName, 10);

	dwp = (UINT32*)(cData + 10);
	*dwp = dwCharID;

	dwp = (UINT32*)(cData + 14);
	*dwp = MSGID_RESPONSE_CHARACTERLOG;

	wp = (UINT16*)(cData + 18);
	*wp = DEF_MSGTYPE_CONFIRM;

	cp = (char*)(cData + 20);
	cp2 = (char*)(pData + 20);
	memcpy(cp, cp2, 18);
	cp += 18;

	cp2 = (char*)(pData + 37);
	cTotalChar = *cp2;
	cp2++;

	dwp = (UINT32*)cp2;
	iSpace1 = (int)dwp;
	cp2 += 4;

	dwp = (UINT32*)cp2;
	iSpace2 = (int)dwp;
	cp2 += 4;

	wsprintf(G_cTxt, "(TestLog) Total Char: %d", cTotalChar);
	PutLogList(G_cTxt);

	for (i = 1; i <= cTotalChar; i++) {
		ZeroMemory(cMapName, sizeof(cMapName));
		ZeroMemory(cCharName, sizeof(cCharName));

		memcpy(cCharName, cp2, 10);
		memcpy(&cInfo[i * 11], cp2, 10);
		cp2 += 10;

		memcpy(cp, cCharName, 10);
		cp += 10;

		if ((iGetCharacterData(cCharName, cMapName, &sAppr1, &sAppr2, &sAppr3, &sAppr4, &iApprColor, &cSex, &cSkin, &iLevel, &iExp, &iStr, &iVit, &iDex, &iInt, &iMag, &iCharisma, &iSaveYear, &iSaveMonth, &iSaveDay, &iSaveHour, &iSaveMinute)) != true) {
			*cp = 0;
			cp += 40;
			break;
		}

		*cp = 1;
		cp++;

		wp = (UINT16*)cp;
		*wp = (int)sAppr1;
		cp += 2;

		wp = (UINT16*)cp;
		*wp = (int)sAppr2;
		cp += 2;

		wp = (UINT16*)cp;
		*wp = (int)sAppr3;
		cp += 2;

		wp = (UINT16*)cp;
		*wp = (int)sAppr4;
		cp += 2;

		wp = (UINT16*)cp;
		*wp = (int)cSex;
		cp += 2;

		wp = (UINT16*)cp;
		*wp = (int)cSkin;
		cp += 2;

		wp = (UINT16*)cp;
		*wp = (int)iLevel;
		cp += 2;

		dwp = (UINT32*)cp;
		*dwp = (UINT32)iExp;
		cp += 4;

		wp = (UINT16*)cp;
		*wp = (int)iStr;
		cp += 2;

		wp = (UINT16*)cp;
		*wp = (int)iVit;
		cp += 2;

		wp = (UINT16*)cp;
		*wp = (int)iDex;
		cp += 2;

		wp = (UINT16*)cp;
		*wp = (int)iInt;
		cp += 2;

		wp = (UINT16*)cp;
		*wp = (int)iMag;
		cp += 2;

		wp = (UINT16*)cp;
		*wp = (int)iCharisma;
		cp += 2;

		ip = (int*)cp;
		*ip = iApprColor;
		cp += 4;

		wp = (UINT16*)cp;
		*wp = (int)iSaveYear;
		cp += 2;

		wp = (UINT16*)cp;
		*wp = (int)iSaveMonth;
		cp += 2;

		wp = (UINT16*)cp;
		*wp = (int)iSaveDay;
		cp += 2;

		wp = (UINT16*)cp;
		*wp = (int)iSaveHour;
		cp += 2;

		wp = (UINT16*)cp;
		*wp = (int)iSaveMinute;
		cp += 2;

		memcpy(cp, cMapName, 10);
		cp += 10;
	}
	dwp = (UINT32*)cp;
	*dwp = (UINT32)iSpace1;
	cp += 4;

	dwp = (UINT32*)cp;
	*dwp = (UINT32)iSpace2;
	cp += 4;

	SendEventToMLS(MSGID_RESPONSE_CHARINFOLIST, DEF_MSGTYPE_CONFIRM, cData, (cTotalChar * 65) + 46, -1);
}

int CWorldLog::iGetCharacterDatabaseID(char* cCharName)
{
	SACommand com;

	
	try
	{
		com.setConnection(&con);
		com.setCommandText("SELECT [Character-ID] FROM Characters WHERE [Character-Name] = :1");
		com.Param(1).setAsString() = cCharName;

		com.Execute();

		if (com.isResultSet())
		{
			int id;
			while (com.FetchNext())
			{
				if (com.Field("Character-ID").asLong() > 0) {
					id = com.Field("Character-ID").asLong();
					com.Close();
					return id;
				}
			}
		}
		com.Close();
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

		
	}
	return -1;
}

int CWorldLog::iGetAccountDatabaseID(char* cAccName)
{
	SACommand com;

	try
	{
		com.setConnection(&con);
		com.setCommandText("SELECT [Account-ID] FROM Accounts WHERE [Account-Name] = :1");
		com.Param(1).setAsString() = cAccName;

		com.Execute();

		if (com.isResultSet())
		{
			int id;
			while (com.FetchNext())
			{
				if (com.Field("Account-ID").asLong() > 0) {
					id = com.Field("Account-ID").asLong();
					com.Close();
					return id;
				}
			}
		}
		com.Close();
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

		
	}
	return -1;
}

void CWorldLog::UpdateLastLoginTime(int iCharacterDBid)
{
	SACommand com;

	try
	{
		com.setConnection(&con);
		com.setCommandText("UPDATE Characters SET [Character-Last-Login] = GETDATE() WHERE [Character-ID] = :1");
		com.Param(1).setAsLong() = iCharacterDBid;

		com.Execute();
		com.Close();

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

bool CWorldLog::iGetCharacterData(char* cCharName, char* cMapName, short* sAppr1, short* sAppr2, short* sAppr3, short* sAppr4, int* iApprColor, char* cSex, char* cSkin, int* iLevel, UINT32* iExp, int* iStr, int* iVit, int* iDex, int* iInt, int* iMag, int* iCharisma, int* iSaveYear, int* iSaveMonth, int* iSaveDay, int* iSaveHour, int* iSaveMinute)
{
	
	int iCharacterDBid;

	*iApprColor = 0;
	if (iSaveYear != 0) *iSaveYear = 0;
	if (iSaveMonth != 0) *iSaveMonth = 0;
	if (iSaveDay != 0) *iSaveDay = 0;
	if (iSaveHour != 0) *iSaveHour = 0;
	if (iSaveMinute != 0) *iSaveMinute = 0;
	if (G_bDBMode == true) {
		SACommand com;

		try
		{
			com.setConnection(&con);
			com.setCommandText("SELECT [Character-ID], [Account-ID], RTRIM([Character-Name]) AS 'Character-Name', [Character-Location], [Character-Guild-Name], [Character-Guild-GUID], [Character-Guild-Rank], [Character-Loc-Map], [Character-Loc-X], [Character-Loc-Y], [Character-HP], [Character-MP], [Character-SP], [Character-Level], [Character-Rating], [Character-Strength], [Character-Intelligence], [Character-Vitality], [Character-Dexterity], [Character-Magic], [Character-Charisma], [Character-Luck], [Character-Experience], [Character-LU-Pool], [Character-Ek-Count], [Character-Pk-Count], [Character-Reward-Gold], [Character-ID1], [Character-ID2], [Character-ID3], [Character-Sex], [Character-Skin], [Character-Hair-Style], [Character-Hair-Colour], [Character-Underwear], [Character-Hunger], [Character-Shutup-Time], [Character-Rating-Time], [Character-Force-Time], [Character-SP-Time], [Character-Admin-Level], [Character-Contribution], [Character-War-Contribution], [Character-Event-ID], [Character-Criticals], [Character-Ability-Time], [Character-Lock-Map], [Character-Lock-Time], [Character-Crusade-Job], [Character-Crusade-GUID], [Character-Construct-Points], [Character-Death-Time], [Character-Party-ID], [Character-Majestics], [Character-Appr1], [Character-Appr2], [Character-Appr3], [Character-Appr4], [Character-Appr-Colour] FROM Characters WHERE [Character-Name] = :1");
			com.Param(1).setAsString() = cCharName;

			com.Execute();

			if (com.isResultSet())
			{
				while (com.FetchNext())
				{
					// db character ID
					iCharacterDBid = com.Field("Character-ID").asLong();

					// char appr1
					*sAppr1 = com.Field("Character-Appr1").asLong();

					// char appr2
					*sAppr2 = com.Field("Character-Appr2").asLong();

					// char appr3
					*sAppr3 = com.Field("Character-Appr3").asLong();

					// char appr1
					*sAppr4 = com.Field("Character-Appr4").asLong();

					// char gender
					*cSex = com.Field("Character-Sex").asLong();

					// char skin
					*cSkin = com.Field("Character-Skin").asLong();

					// char level
					*iLevel = com.Field("Character-Level").asLong();

					// char str
					*iStr = com.Field("Character-Strength").asLong();

					// char dex
					*iDex = com.Field("Character-Dexterity").asLong();

					// char vit
					*iVit = com.Field("Character-Vitality").asLong();

					// char mag
					*iMag = com.Field("Character-Magic").asLong();

					// char int
					*iInt = com.Field("Character-Intelligence").asLong();

					// char charisma
					*iCharisma = com.Field("Character-Charisma").asLong();

					// char exp
					*iExp = com.Field("Character-Experience").asLong();

					// char appr colour
					*iApprColor = com.Field("Character-Appr-Colour").asLong();

					// char map
					strcpy(cMapName, com.Field("Character-Loc-Map").asString());

					// char save year
					*iSaveYear = 0;

					// char save month
					*iSaveMonth = 0;

					// char save day
					*iSaveDay = 0;

					// char save hour
					*iSaveHour = 0;

					// char save minute
					*iSaveMinute = 0;

					com.Close();
					return true;
				}
			}
			else
			{
				com.Close();
				return false;
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

			return false;
		}
	}

	return true;
}

void CWorldLog::RequestSavePlayerData(int iClientH, char* pData, UINT32 dwMsgSize, bool bVar1, bool bVar2)
{
	char cAccountName[11], cCharName[11], cAccountPassword[11], cData[256], cTemp[128], cLocation[11], cGuildName[21];
	char cMapName[11], cMagicM[101], cItemX[201], cItemY[201], cItemName[21], cEquipStatus[51], cSkillMastery[171], cSkillSSN[171];
	int iGuildGUID, iGuildRank, iHP, iMP, iSP, iLevel, iRating, iStr, iInt, iDex, iVit, iMag, iCharisma, iLU_Pool;
	int iEK, iPK, iHunger, iShutUp, iRatingTime, iForceRecall, iContribution, iCrits, iAbilityTime, iMajestics, iApprColour;
	int iRewardGold, i, iWarContribution, iCrusadeDuty, iConstructPts;
	short sAppr1, sAppr2, sAppr3, sAppr4, sCharID1, sCharID2, sCharID3, isX, isY, iNumItems, iNumBankItems;

	short iItemElement1, iItemElement2, iItemElement3, iItemElement4;

	int iAccountID;

	char* cp, sSex, sSkin, sHairStyle, sHairColour, sUnderwear;
	UINT32* dwp, iCharDBID, iExp;
	UINT16* wp;
	int* ip, iRet;
	short* sp;
	UINT32   dwTime, dwCrusadeGUID;
	bool bFlag;

	int iItemNum2, iItemNum3, iItemNum4, iItemNum5, iItemColour, iItemNum7, iItemNum8, iItemNum9, iItemAttribute;
	UINT32 iItemNum1;
	UINT16 iItemNum10;

	if (m_pClientList[iClientH] == 0) return;

	ZeroMemory(cAccountName, sizeof(cAccountName));
	ZeroMemory(cCharName, sizeof(cCharName));
	ZeroMemory(cAccountPassword, sizeof(cAccountPassword));
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

	dwp = (UINT32*)cp;
	iCharDBID = *dwp;
	cp += 4;

	bFlag = (bool)*cp;
	cp++;

	//-----------------------------------------------------
	sp = (short*)cp;
	sCharID1 = *sp;
	cp += 2;

	sp = (short*)cp;
	sCharID2 = *sp;
	cp += 2;

	sp = (short*)cp;
	sCharID3 = *sp;
	cp += 2;

	ip = (int*)cp;
	iLevel = *ip;
	cp += 4;

	ip = (int*)cp;
	iStr = *ip;
	cp += 4;

	ip = (int*)cp;
	iVit = *ip;
	cp += 4;

	ip = (int*)cp;
	iDex = *ip;
	cp += 4;

	ip = (int*)cp;
	iInt = *ip;
	cp += 4;

	ip = (int*)cp;
	iMag = *ip;
	cp += 4;

	ip = (int*)cp;
	iCharisma = *ip;
	cp += 4;

	dwp = (UINT32*)cp;
	iExp = *dwp;
	cp += 4;

	ip = (int*)cp;
	iLU_Pool = *ip;
	cp += 4;

	sSex = *cp;
	cp++;

	sSkin = *cp;
	cp++;

	sHairStyle = *cp;
	cp++;

	sHairColour = *cp;
	cp++;

	sUnderwear = *cp;
	cp++;

	ip = (int*)cp;
	iApprColour = *ip;
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

	memcpy(cLocation, cp, 10);
	cp += 10;

	memcpy(cMapName, cp, 10);
	cp += 10;

	sp = (short*)cp;
	isX = *sp;
	cp += 2;

	sp = (short*)cp;
	isY = *sp;
	cp += 2;

	ip = (int*)cp;
	int iAdminUserLevel = *ip;
	cp += 4;

	ip = (int*)cp;
	iContribution = *ip;
	cp += 4;

	ip = (int*)cp;
	iAbilityTime = *ip;
	cp += 4;

	char cLockedMapName[11];
	ZeroMemory(cLockedMapName, sizeof(cLockedMapName));
	memcpy(cLockedMapName, cp, 10);
	cp += 10;

	ip = (int*)cp;
	int iLockedMapTime = *ip;
	cp += 4;

	//Penalty Block
	ip = (int*)cp;
	int iPenaltyBlockYear = *ip;
	cp += 4;

	ip = (int*)cp;
	int iPenaltyBlockMonth = *ip;
	cp += 4;

	ip = (int*)cp;
	int iPenaltyBlockDay = *ip;
	cp += 4;

	memcpy(cGuildName, cp, 20);
	cp += 20;

	ip = (int*)cp;
	iGuildGUID = *ip;
	cp += 4;

	ip = (int*)cp;
	iGuildRank = *ip;
	cp += 4;

	int iQuest[5], iQuestID[5], iCurQuestCount[5], iQuestRewardType[5], iQuestRewardAmount[5];
	bool bIsQuestCompleted[5];

	for (i = 0; i < 5; i++)
	{
		ip = (int*)cp;
		iQuest[i] = *ip;
		cp += 4;

		ip = (int*)cp;
		iQuestID[i] = *ip;
		cp += 4;

		ip = (int*)cp;
		iCurQuestCount[i] = *ip;
		cp += 4;

		ip = (int*)cp;
		iQuestRewardType[i] = *ip;
		cp += 4;

		ip = (int*)cp;
		iQuestRewardAmount[i] = *ip;
		cp += 4;

		bIsQuestCompleted[i] = (bool)*cp;
		cp++;
	}

	ip = (int*)cp;
	int iSpecialEventID = *ip;
	cp += 4;

	ip = (int*)cp;
	iWarContribution = *ip;
	cp += 4;

	ip = (int*)cp;
	iCrusadeDuty = *ip;
	cp += 4;

	dwp = (UINT32*)cp;
	dwCrusadeGUID = *dwp;
	cp += 4;

	ip = (int*)cp;
	iConstructPts = *ip;
	cp += 4;

	ip = (int*)cp;
	iRating = *ip;
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
	iEK = *ip;
	cp += 4;

	ip = (int*)cp;
	iPK = *ip;
	cp += 4;

	ip = (int*)cp;
	iRewardGold = *ip;
	cp += 4;

	ip = (int*)cp;
	iHunger = *ip;
	cp += 4;

	ip = (int*)cp;
	iCrits = *ip;
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
	int iSPTime = *ip;
	cp += 4;

	ip = (int*)cp;
	int iDeadTime = *ip;
	cp += 4;

	ip = (int*)cp;
	int iPartyID = *ip;
	cp += 4;

	ip = (int*)cp;
	iMajestics = *ip;
	cp += 4;

	//
	ip = (int*)cp;
	int iDeaths = *ip;
	cp += 4;

	ip = (int*)cp;
	int iTotalDGKills = *ip;
	cp += 4;

	ip = (int*)cp;
	int iDGPoints = *ip;
	cp += 4;

	ip = (int*)cp;
	int iTotalDGDeaths = *ip;
	cp += 4;

	ip = (int*)cp;
	int iDGKills = *ip;
	cp += 4;

	ip = (int*)cp;
	int iMaxEK = *ip;
	cp += 4;

	dwp = (UINT32*)cp;
	UINT32 dwHeldenianGUID = *dwp;
	cp += 4;

	ip = (int*)cp;
	int iWantedLevel = *ip;
	cp += 4;

	ip = (int*)cp;
	int iCoinPoints = *ip;
	cp += 4;

	ip = (int*)cp;
	int iTeam = *ip;
	cp += 4;

	ip = (int*)cp;
	int iClass = *ip;
	cp += 4;

	memcpy(cMagicM, cp, 100);
	cp += 100;

	memcpy(cEquipStatus, cp, 50);
	cp += 50;

	memcpy(cItemX, cp, 200);
	cp += 200;

	memcpy(cItemY, cp, 200);
	cp += 200;

	// Skill
	memcpy(cSkillMastery, cp, 170);
	cp += 170;

	memcpy(cSkillSSN, cp, 170);
	cp += 170;

	if ((dwMsgSize - 40) <= 0) {
		PutLogList("(X) Character data body empty: Cannot create & save data file.");
		return;
	}
	if (G_bDBMode == true) {

		int iItems = 1, iBItems = 1;

		SACommand com;
		SACommand com2;
		SACommand com3;
		SACommand com4;
		SACommand com5;

		try
		{
			iAccountID = iGetAccountDatabaseID(cAccountName);

			com3.setConnection(&con);
			com3.setCommandText("DELETE FROM CharItems WHERE [Character-ID] = :1");
			com3.Param(1).setAsLong() = iCharDBID;

			com3.Execute();
			com3.Close();

			sp = (short*)cp;
			iNumItems = *sp;
			cp += 2;

			if (iNumItems > 0)
			{
				try
				{
					
					com2.setConnection(&con);
					for (i = 0; i < iNumItems; i++)
					{
						
						ZeroMemory(cItemName, sizeof(cItemName));
						memcpy(cItemName, cp, 20);
						cp += 20;

						dwp = (UINT32*)cp;
						iItemNum1 = *dwp;
						cp += 4;

						sp = (short*)cp;
						iItemNum2 = *sp;
						cp += 2;

						sp = (short*)cp;
						iItemNum3 = *sp;
						cp += 2;

						sp = (short*)cp;
						iItemNum4 = *sp;
						cp += 2;

						sp = (short*)cp;
						iItemNum5 = *sp;
						cp += 2;

						sp = (short*)cp;
						iItemColour = *sp;
						cp += 2;

						sp = (short*)cp;
						iItemNum7 = *sp;
						cp += 2;

						sp = (short*)cp;
						iItemNum8 = *sp;
						cp += 2;

						sp = (short*)cp;
						iItemNum9 = *sp;
						cp += 2;

						wp = (UINT16*)cp;
						iItemNum10 = *wp;
						cp += 2;

						dwp = (UINT32*)cp;
						iItemAttribute = *dwp;
						cp += 4;

						sp = (short*)cp;
						iItemElement1 = *sp;
						cp += 2;

						sp = (short*)cp;
						iItemElement2 = *sp;
						cp += 2;

						sp = (short*)cp;
						iItemElement3 = *sp;
						cp += 2;

						sp = (short*)cp;
						iItemElement4 = *sp;
						cp += 2;

						com2.setCommandText("INSERT INTO CharItems([Character-ID], ItemName, ItemNum1, ItemNum2, ItemNum3, ItemNum4, ItemNum5, [Item-Colour], ItemNum7, ItemNum8, ItemNum9, ItemNum10, [Item-Attribute], [Item-Element1], [Item-Element2], [Item-Element3], [Item-Element4]) VALUES(:1, :2, :3, :4, :5, :6, :7, :8, :9, :10, :11, :12, :13, :14, :15, :16, :17)");
						com2.Param(1).setAsLong() = iCharDBID;
						com2.Param(2).setAsString() = cItemName;
						com2.Param(3).setAsLong() = iItemNum1;
						com2.Param(4).setAsLong() = iItemNum2;
						com2.Param(5).setAsLong() = iItemNum3;
						com2.Param(6).setAsLong() = iItemNum4;
						com2.Param(7).setAsLong() = iItemNum5;
						com2.Param(8).setAsLong() = iItemColour;
						com2.Param(9).setAsLong() = iItemNum7;
						com2.Param(10).setAsLong() = iItemNum8;
						com2.Param(11).setAsLong() = iItemNum9;
						com2.Param(12).setAsLong() = iItemNum10;
						com2.Param(13).setAsLong() = iItemAttribute;
						com2.Param(14).setAsLong() = iItemElement1;
						com2.Param(15).setAsLong() = iItemElement2;
						com2.Param(16).setAsLong() = iItemElement3;
						com2.Param(17).setAsLong() = iItemElement4;

						com2.Execute();
					}

					com2.Close();
					
				}
				catch (SAException& x)
				{
					con.Rollback();

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "(!!!) SQL SERVER ERROR (RequestSavePlayerData - CharItem): %s", (const char*)x.ErrText());
					PutLogList(cTemp);

				}
			}

			com5.setConnection(&con);
			com5.setCommandText("DELETE FROM BankItems WHERE [Account-ID] = :1");
			com5.Param(1).setAsLong() = iAccountID;

			com5.Execute();
			com5.Close();

			
			sp = (short*)cp;
			iNumBankItems = *sp;
			cp += 2;

			if (iNumBankItems > 0)
			{
				try
				{
					
					com4.setConnection(&con);
					for (i = 0; i < iNumBankItems; i++)
					{
						
						ZeroMemory(cItemName, sizeof(cItemName));
						memcpy(cItemName, cp, 20);
						cp += 20;

						dwp = (UINT32*)cp;
						iItemNum1 = *dwp;
						cp += 4;

						sp = (short*)cp;
						iItemNum2 = *sp;
						cp += 2;

						sp = (short*)cp;
						iItemNum3 = *sp;
						cp += 2;

						sp = (short*)cp;
						iItemNum4 = *sp;
						cp += 2;

						sp = (short*)cp;
						iItemNum5 = *sp;
						cp += 2;

						sp = (short*)cp;
						iItemColour = *sp;
						cp += 2;

						sp = (short*)cp;
						iItemNum7 = *sp;
						cp += 2;

						sp = (short*)cp;
						iItemNum8 = *sp;
						cp += 2;

						sp = (short*)cp;
						iItemNum9 = *sp;
						cp += 2;

						wp = (UINT16*)cp;
						iItemNum10 = *wp;
						cp += 2;

						dwp = (UINT32*)cp;
						iItemAttribute = *dwp;
						cp += 4;

						sp = (short*)cp;
						iItemElement1 = *sp;
						cp += 2;

						sp = (short*)cp;
						iItemElement2 = *sp;
						cp += 2;

						sp = (short*)cp;
						iItemElement3 = *sp;
						cp += 2;

						sp = (short*)cp;
						iItemElement4 = *sp;
						cp += 2;

						com4.setCommandText("INSERT INTO BankItems([Account-ID], ItemName, ItemNum1, ItemNum2, ItemNum3, ItemNum4, ItemNum5, [Item-Colour], ItemNum7, ItemNum8, ItemNum9, ItemNum10, [Item-Attribute], [Item-Element1], [Item-Element2], [Item-Element3], [Item-Element4]) VALUES(:1, :2, :3, :4, :5, :6, :7, :8, :9, :10, :11, :12, :13, :14, :15, :16, :17)");
						com4.Param(1).setAsLong() = iAccountID;
						com4.Param(2).setAsString() = cItemName;
						com4.Param(3).setAsLong() = iItemNum1;
						com4.Param(4).setAsLong() = iItemNum2;
						com4.Param(5).setAsLong() = iItemNum3;
						com4.Param(6).setAsLong() = iItemNum4;
						com4.Param(7).setAsLong() = iItemNum5;
						com4.Param(8).setAsLong() = iItemColour;
						com4.Param(9).setAsLong() = iItemNum7;
						com4.Param(10).setAsLong() = iItemNum8;
						com4.Param(11).setAsLong() = iItemNum9;
						com4.Param(12).setAsLong() = iItemNum10;
						com4.Param(13).setAsLong() = iItemAttribute;
						com4.Param(14).setAsLong() = iItemElement1;
						com4.Param(15).setAsLong() = iItemElement2;
						com4.Param(16).setAsLong() = iItemElement3;
						com4.Param(17).setAsLong() = iItemElement4;
						com4.Execute();
					}
					com4.Close();

				}
				catch (SAException& x)
				{
					con.Rollback();

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "(!!!) SQL SERVER ERROR (RequestSavePlayerData - BankItem): %s", (const char*)x.ErrText());
					PutLogList(cTemp);

				}
			}

			com.setConnection(&con);

			com.setCommandText("UPDATE Characters SET [Character-ID1] = :2, [Character-ID2] = :3, [Character-ID3] = :4, [Character-Level] = :5, [Character-Strength] = :6, [Character-Vitality] = :7, [Character-Dexterity] = :8, [Character-Intelligence] = :9, [Character-Magic] = :10, [Character-Charisma] = :11, [Character-Experience] = :12, [Character-LU-Pool] = :13, [Character-Sex] = :14, [Character-Skin] = :15, [Character-Hair-Style] = :16, [Character-Hair-Colour] = :17, [Character-Underwear] = :18, [Character-Appr-Colour] = :19, [Character-Appr1] = :20, [Character-Appr2] = :21, [Character-Appr3] = :22, [Character-Appr4] = :23, [Character-Location] = :24, [Character-Loc-Map] = :25, [Character-Loc-X] = :26, [Character-Loc-Y] = :27, [Character-Admin-Level] = :28, [Character-Contribution] = :29, [Character-Ability-Time] = :30, [Character-Lock-Map] = :31, [Character-Lock-Time] = :32, [Character-Guild-Name] = :33, [Character-Guild-GUID] = :34, [Character-Guild-Rank] = :35, [Character-Quest-Number1] = :36, [Character-Quest-ID1] = :37, [Character-Quest-Count1] = :38, [Character-Quest-Reward1] = :39, [Character-Quest-Amount1] = :40, [Character-Quest-Completed1] = :41, [Character-Quest-Number2] = :42, [Character-Quest-ID2] = :43, [Character-Quest-Count2] = :44, [Character-Quest-Reward2] = :45, [Character-Quest-Amount2] = :46, [Character-Quest-Completed2] = :47, [Character-Quest-Number3] = :48, [Character-Quest-ID3] = :49, [Character-Quest-Count3] = :50, [Character-Quest-Reward3] = :51, [Character-Quest-Amount3] = :52, [Character-Quest-Completed3] = :53, [Character-Quest-Number4] = :54, [Character-Quest-ID4] = :55, [Character-Quest-Count4] = :56, [Character-Quest-Reward4] = :57, [Character-Quest-Amount4] = :58, [Character-Quest-Completed4] = :59, [Character-Quest-Number5] = :60, [Character-Quest-ID5] = :61, [Character-Quest-Count5] = :62, [Character-Quest-Reward5] = :63, [Character-Quest-Amount5] = :64, [Character-Quest-Completed5] = :65, [Character-Event-ID] = :66, [Character-War-Contribution] = :67, [Character-Crusade-Job] = :68, [Character-Crusade-GUID] = :69, [Character-Construct-Points] = :70, [Character-Rating] = :71, [Character-HP] = :72, [Character-MP] = :73, [Character-SP] = :74, [Character-EK-Count] = :75, [Character-PK-Count] = :76, [Character-Reward-Gold] = :77, [Character-Hunger] = :78, [Character-Criticals] = :79, [Character-Shutup-Time] = :80, [Character-Rating-Time] = :81, [Character-Force-Time] = :82, [Character-SP-Time] = :83, [Character-Death-Time] = :84, [Character-Party-ID] = :85, [Character-Majestics] = :86, [Character-Deaths] = :87, [Character-TotalDGKills] = :88, [Character-DGPoints] = :89, [Character-TotalDGDeaths] = :90, [Character-DGKills] = :91, [Character-Max-Ek] = :92, [Character-Heldenian-GUID] = :93, [Character-Wanted-Level] = :94, [Character-Coin-Points] = :95, [Character-Team] = :96, [Character-Class] = :97, [Character-Magic-Mastery] = :98, [Character-Equip-Status] = :99, [Character-Item-Pos-X] = :100, [Character-Item-Pos-Y] = :101, [Character-Skill-Mastery] = :102, [Character-Skill-SSN] = :103, [Character-Penalty-Year] = :104, [Character-Penalty-Month] = :105, [Character-Penalty-Day] = :106 WHERE [Character-ID] = :1");

			com.Param(1).setAsLong() = iCharDBID; // fijo

			com.Param(2).setAsLong() = sCharID1;
			com.Param(3).setAsLong() = sCharID2;
			com.Param(4).setAsLong() = sCharID3;
			com.Param(5).setAsLong() = iLevel;
			com.Param(6).setAsLong() = iStr;
			com.Param(7).setAsLong() = iVit;
			com.Param(8).setAsLong() = iDex;
			com.Param(9).setAsLong() = iInt;
			com.Param(10).setAsLong() = iMag;
			com.Param(11).setAsLong() = iCharisma;
			com.Param(12).setAsLong() = iExp;
			com.Param(13).setAsLong() = iLU_Pool;
			com.Param(14).setAsLong() = sSex;
			com.Param(15).setAsLong() = sSkin;
			com.Param(16).setAsLong() = sHairStyle;
			com.Param(17).setAsLong() = sHairColour;
			com.Param(18).setAsLong() = sUnderwear;
			com.Param(19).setAsLong() = iApprColour;
			com.Param(20).setAsLong() = sAppr1;
			com.Param(21).setAsLong() = sAppr2;
			com.Param(22).setAsLong() = sAppr3;
			com.Param(23).setAsLong() = sAppr4;
			com.Param(24).setAsString() = cLocation;
			com.Param(25).setAsString() = cMapName;
			com.Param(26).setAsLong() = isX;
			com.Param(27).setAsLong() = isY;
			com.Param(28).setAsLong() = iAdminUserLevel;
			com.Param(29).setAsLong() = iContribution;
			com.Param(30).setAsLong() = iAbilityTime;
			com.Param(31).setAsString() = cLockedMapName;
			com.Param(32).setAsLong() = iLockedMapTime;
			com.Param(33).setAsString() = cGuildName;
			com.Param(34).setAsLong() = iGuildGUID;
			com.Param(35).setAsLong() = iGuildRank;
			com.Param(36).setAsLong() = iQuest[0];
			com.Param(37).setAsLong() = iQuestID[0];
			com.Param(38).setAsLong() = iCurQuestCount[0];
			com.Param(39).setAsLong() = iQuestRewardType[0];
			com.Param(40).setAsLong() = iQuestRewardAmount[0];
			com.Param(41).setAsLong() = (char)bIsQuestCompleted[0];
			com.Param(42).setAsLong() = iQuest[1];
			com.Param(43).setAsLong() = iQuestID[1];
			com.Param(44).setAsLong() = iCurQuestCount[1];
			com.Param(45).setAsLong() = iQuestRewardType[1];
			com.Param(46).setAsLong() = iQuestRewardAmount[1];
			com.Param(47).setAsLong() = (char)bIsQuestCompleted[1];
			com.Param(48).setAsLong() = iQuest[2];
			com.Param(49).setAsLong() = iQuestID[2];
			com.Param(50).setAsLong() = iCurQuestCount[2];
			com.Param(51).setAsLong() = iQuestRewardType[2];
			com.Param(52).setAsLong() = iQuestRewardAmount[2];
			com.Param(53).setAsLong() = (char)bIsQuestCompleted[2];
			com.Param(54).setAsLong() = iQuest[3];
			com.Param(55).setAsLong() = iQuestID[3];
			com.Param(56).setAsLong() = iCurQuestCount[3];
			com.Param(57).setAsLong() = iQuestRewardType[3];
			com.Param(58).setAsLong() = iQuestRewardAmount[3];
			com.Param(59).setAsLong() = (char)bIsQuestCompleted[3];
			com.Param(60).setAsLong() = iQuest[4];
			com.Param(61).setAsLong() = iQuestID[4];
			com.Param(62).setAsLong() = iCurQuestCount[4];
			com.Param(63).setAsLong() = iQuestRewardType[4];
			com.Param(64).setAsLong() = iQuestRewardAmount[4];
			com.Param(65).setAsLong() = (char)bIsQuestCompleted[4];
			com.Param(66).setAsLong() = iSpecialEventID;
			com.Param(67).setAsLong() = iWarContribution;
			com.Param(68).setAsLong() = iCrusadeDuty;
			com.Param(69).setAsLong() = dwCrusadeGUID;
			com.Param(70).setAsLong() = iConstructPts;
			com.Param(71).setAsLong() = iRating;
			com.Param(72).setAsLong() = iHP;
			com.Param(73).setAsLong() = iMP;
			com.Param(74).setAsLong() = iSP;
			com.Param(75).setAsLong() = iEK;
			com.Param(76).setAsLong() = iPK;
			com.Param(77).setAsLong() = iRewardGold;
			com.Param(78).setAsLong() = iHunger;
			com.Param(79).setAsLong() = iCrits;
			com.Param(80).setAsLong() = iShutUp;
			com.Param(81).setAsLong() = iRatingTime;
			com.Param(82).setAsLong() = iForceRecall;
			com.Param(83).setAsLong() = iSPTime;
			com.Param(84).setAsLong() = iDeadTime;
			com.Param(85).setAsLong() = iPartyID;
			com.Param(86).setAsLong() = iMajestics;
			com.Param(87).setAsLong() = iDeaths;
			com.Param(88).setAsLong() = iTotalDGKills;
			com.Param(89).setAsLong() = iDGPoints;
			com.Param(90).setAsLong() = iTotalDGDeaths;
			com.Param(91).setAsLong() = iDGKills;
			com.Param(92).setAsLong() = iMaxEK;
			com.Param(93).setAsLong() = dwHeldenianGUID;
			com.Param(94).setAsLong() = iWantedLevel;
			com.Param(95).setAsLong() = iCoinPoints;
			com.Param(96).setAsLong() = iTeam;
			com.Param(97).setAsLong() = iClass;
			com.Param(98).setAsString() = cMagicM;
			com.Param(99).setAsString() = cEquipStatus;
			com.Param(100).setAsString() = cItemX;
			com.Param(101).setAsString() = cItemY;

			com.Param(102).setAsString() = cSkillMastery;
			com.Param(103).setAsString() = cSkillSSN;

			com.Param(104).setAsLong() = iPenaltyBlockYear;
			com.Param(105).setAsLong() = iPenaltyBlockMonth;
			com.Param(106).setAsLong() = iPenaltyBlockDay;

			com.Execute();
			com.Close();

		}
		catch (SAException& x)
		{
			con.Rollback();

			ZeroMemory(cTemp, sizeof(cTemp));
			wsprintf(cTemp, "(!!!) SQL SERVER ERROR (RequestSavePlayerData): %s", (const char*)x.ErrText());
			PutLogList(cTemp);
			return;
		}
	}
	if ((bVar1 == true) && (bFlag == 1)) {
		ZeroMemory(cData, sizeof(cData));

		cp = (char*)cData;
		memcpy(cp, cAccountName, 10);
		cp += 10;

		wsprintf(G_cTxt, "(TestLog) Account(%s) log out request to MLS...", cAccountName);
		PutLogList(G_cTxt);

		SendEventToMLS(MSGID_REQUEST_CLEARACCOUNTSTATUS, DEF_MSGTYPE_CONFIRM, cData, 10, -1);
		OnPlayerAccountMessage(DEF_MSGACCOUNTSET_NULL, cAccountName, 0, 0, 0);
	}
	if (bVar2 == true) {
		ZeroMemory(cData, sizeof(cData));

		dwp = (UINT32*)(cData);
		*dwp = MSGID_RESPONSE_SAVEPLAYERDATA_REPLY;

		wp = (UINT16*)(cData + DEF_INDEX2_MSGTYPE);
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
			m_pClientList[iClientH] = 0;
			break;
		}
	}
}

void CWorldLog::WriteOccupyFlagFile(char* cFn)
{
	// INCOMPLETE
	// hypnotoad: useless function? write flag is obsolete
}

void CWorldLog::UpdateNoticement(WPARAM wParam)
{
	int i;

	switch (wParam) {
	case VK_F2:
		for (i = 0; i < DEF_MAXGAMESERVERS; i++) {
			if (m_pGameList[i] != 0) {
				bSendClientConfig(m_pGameList[i]->m_iTrackSock, "Noticement.txt");
			}
		}
		PutLogList("(!) Updating noticement contents(noticement.txt) to game servers...");
		break;
	}
}

void CWorldLog::EnterGame(int iClientH, char* pData)
{
	char cData[256], cCharNameCheck[11], cMapName[11], cCharacterName[11], cAccountName[11], cAccountPassword[11], cAddress[16];
	int iPort, iCode, iLevel, iRet;
	char* cp;
	UINT16* wp;
	UINT32* dwp;
	int i;
	bool bflag;
	bflag = false;

	if (m_pClientList[iClientH] == 0) return;
	ZeroMemory(cData, sizeof(cData));
	ZeroMemory(cCharNameCheck, sizeof(cCharNameCheck));
	ZeroMemory(cMapName, sizeof(cMapName));
	ZeroMemory(cCharacterName, sizeof(cCharacterName));
	ZeroMemory(cAccountName, sizeof(cAccountName));
	ZeroMemory(cAccountPassword, sizeof(cAccountPassword));
	ZeroMemory(cAddress, sizeof(cAddress));

	cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);

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
		if ((m_cMapList[i] != 0) && (memcmp(m_cMapList[i]->m_cName, cMapName, 10) == 0)) bflag = true;

	for (i = 0; i < DEF_MAXPLAYERACCOUNTS; i++)
		if ((m_pAccountList[i] != 0) && (memcmp(m_pAccountList[i]->cAccountName, cAccountName, 10) == 0)) m_pAccountList[i]->Timeout = 20;

	if (iCode == -1) {
		dwp = (UINT32*)(cData);
		*dwp = MSGID_RESPONSE_ENTERGAME;
		wp = (UINT16*)(cData + DEF_INDEX2_MSGTYPE);
		*wp = DEF_ENTERGAMERESTYPE_REJECT;
		cp = (char*)(cData + DEF_INDEX2_MSGTYPE + 2);
		*cp = 3;
		iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 7, DEF_USE_ENCRYPTION);

		if (bflag == false) {
			wsprintf(G_cTxt, "(TestLog) ReqEnterGame Reject! Account(%s) status 0", cAccountName);
			PutLogList(G_cTxt);
			SendEventToMLS(MSGID_REQUEST_CLEARACCOUNTSTATUS, DEF_MSGTYPE_CONFIRM, cAccountName, 10, -1);
			OnPlayerAccountMessage(DEF_MSGACCOUNTSET_NULL, cAccountName, 0, 0, 0);
			delete m_pClientList[iClientH];
			m_pClientList[iClientH] = 0;
			return;
		}
	}
	else if (iCode == -2) {
		dwp = (UINT32*)(cData);
		*dwp = MSGID_RESPONSE_ENTERGAME;
		wp = (UINT16*)(cData + DEF_INDEX2_MSGTYPE);
		*wp = DEF_ENTERGAMERESTYPE_REJECT;
		cp = (char*)(cData + DEF_INDEX2_MSGTYPE + 2);
		*cp = 5;
		iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 7, DEF_USE_ENCRYPTION);

		if (bflag == false) {
			wsprintf(G_cTxt, "(TestLog) ReqEnterGame Reject! Account(%s) status 0", cAccountName);
			PutLogList(G_cTxt);
			SendEventToMLS(MSGID_REQUEST_CLEARACCOUNTSTATUS, DEF_MSGTYPE_CONFIRM, cAccountName, 10, -1);
			OnPlayerAccountMessage(DEF_MSGACCOUNTSET_NULL, cAccountName, 0, 0, 0);
			delete m_pClientList[iClientH];
			m_pClientList[iClientH] = 0;
			return;
		}
	}
	else {
		if (iGetCharacterInformation(cAccountName, cCharNameCheck, &iLevel) == false) {
			wsprintf(G_cTxt, "(TestLog) ReqEnterGame Reject! Account(%s) status 0", cAccountName);
			PutLogList(G_cTxt);
			SendEventToMLS(MSGID_REQUEST_CLEARACCOUNTSTATUS, DEF_MSGTYPE_CONFIRM, cAccountName, 10, -1);
			OnPlayerAccountMessage(DEF_MSGACCOUNTSET_NULL, cAccountName, 0, 0, 0);
			delete m_pClientList[iClientH];
			m_pClientList[iClientH] = 0;
			return;
		}
		dwp = (UINT32*)(cData);
		*dwp = MSGID_RESPONSE_ENTERGAME;
		wp = (UINT16*)(cData + DEF_INDEX2_MSGTYPE);
		*wp = DEF_ENTERGAMERESTYPE_CONFIRM;

		cp = (char*)(cData + DEF_INDEX2_MSGTYPE + 2);

		memcpy(cp, cAddress, 16);
		cp += 16;

		wp = (UINT16*)cp;
		*wp = iPort;
		cp += 2;

		iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 24, DEF_USE_ENCRYPTION);
	}
	switch (iRet) {
	case DEF_XSOCKEVENT_QUENEFULL:
	case DEF_XSOCKEVENT_SOCKETERROR:
	case DEF_XSOCKEVENT_CRITICALERROR:
	case DEF_XSOCKEVENT_SOCKETCLOSED:
		delete m_pClientList[iClientH];
		m_pClientList[iClientH] = 0;
		break;
	}
}

int CWorldLog::iGetMapInformation(char* cMapName, char* cAddress, int* iPort, int iClientH)
{
	int i, iIndex;

	for (i = 0; i < DEF_MAXGAMESERVERSMAPS; i++) {
		if ((m_cMapList[i] != 0) && (strcmp(m_cMapList[i]->m_cName, cMapName) == 0)) {
			iIndex = m_cMapList[i]->iIndex;
			if (m_pGameList[iIndex] != 0) {
				ZeroMemory(cAddress, sizeof(cAddress));
				strcpy(cAddress, m_pGameList[iIndex]->m_cGameServerAddress);
				if (memcmp(m_pClientList[iClientH]->m_cIPaddress, m_pGameList[iIndex]->m_cGameServerAddress, strlen(m_pClientList[iClientH]->m_cIPaddress)) == 0) strcpy(cAddress, m_pGameList[iIndex]->m_cGameServerLanAddress);
				*iPort = m_pGameList[iIndex]->m_iGameServerPort;
				return 0;
			}
			return -1;
		}
	}
	return -2;
}

void CWorldLog::RequestNoSaveLogout(int iClientH, char* pData)
{
	char* cp, cTotalChar;
	char cAccountName[11], cCharacterName[11], cGameServerName[11], cData[256];

	if (m_pClientList[iClientH] == 0) return;
	ZeroMemory(cAccountName, sizeof(cAccountName));
	ZeroMemory(cCharacterName, sizeof(cCharacterName));
	ZeroMemory(cGameServerName, sizeof(cGameServerName));

	cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);

	memcpy(cCharacterName, cp, 10);
	cp += 10;

	memcpy(cAccountName, cp, 10);
	cp += 10;

	memcpy(cGameServerName, cp, 10);
	cp += 10;

	cTotalChar = *cp;
	cp++;

	if (cTotalChar == 1) {
		cp = (char*)(cData);
		memcpy(cp, cAccountName, 10);
		cp += 10;

		wsprintf(G_cTxt, "(TestLog) Account(%s) log out request to MLS...", cAccountName);
		PutLogList(G_cTxt);

		SendEventToMLS(MSGID_REQUEST_CLEARACCOUNTSTATUS, DEF_MSGTYPE_CONFIRM, cData, 10, -1);
		OnPlayerAccountMessage(0, cAccountName, 0, 0, 0);
	}
}

void CWorldLog::RequestCreateNewCharacter(int iClientH, char* pData)
{
	char cData[3000], cCharacterName[111], cTotalCharNames[111], cNewCharName[11], cAccountName[11], cPassword[11], cUnknown1[31], cTxt[11], cTxt2[120], cMapName[11];
	char* cp;
	UINT32* dwp, dwCharID, dwTime = timeGetTime(), iExp;
	UINT16* wp;
	short sAppr1, sAppr2, sAppr3, sAppr4;
	char cSex, cSkin, cTotalChar, cNewGender, cNewSkin, cNewStr, cNewVit, cNewDex, cNewInt, cNewMag, cNewChr;
	int iTempAppr2, iTempAppr3, iTempAppr1, i, iLevel, iStr, iVit, iInt, iDex, iMag, iCharisma, iApprColor, iSaveYear, iSaveMonth, iSaveDay, iSaveHour, iSaveMinute;
	int* ip;

	sAppr1 = 0;
	sAppr2 = 0;
	sAppr3 = 0;
	sAppr4 = 0;
	ZeroMemory(cData, sizeof(cData));
	ZeroMemory(cCharacterName, sizeof(cCharacterName));
	ZeroMemory(cNewCharName, sizeof(cNewCharName));
	ZeroMemory(cAccountName, sizeof(cAccountName));
	ZeroMemory(cPassword, sizeof(cPassword));

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

	dwp = (UINT32*)cp;
	dwCharID = *dwp;
	cp += 4;

	cTotalChar = (char)*cp;
	cp++;

	memcpy(cTotalCharNames, cp, cTotalChar * 11);
	cp += cTotalChar * 11;


	cp = (char*)(cData);
	memcpy(cp, cAccountName, 10);
	cp += 10;

	dwp = (UINT32*)(cData + 10);
	*dwp = dwCharID;
	dwp = (UINT32*)(cData + 14);
	*dwp = MSGID_RESPONSE_CHARACTERLOG;
	wp = (UINT16*)(cData + 18);
	*wp = DEF_LOGRESMSGTYPE_NEWCHARACTERFAILED;


	sAppr1 = 0;
	sAppr1 = sAppr1 | iTempAppr1;
	sAppr1 = sAppr1 | (iTempAppr2 << 8);
	sAppr1 = sAppr1 | (iTempAppr3 << 4);

	if (G_bDBMode == true) {
		bool exists = bCheckCharacterExists(cNewCharName);

		int iAccountDBid = iGetAccountDatabaseID(cAccountName);

		if (iAccountDBid < 0)
		{
			cp = (char*)(cData);
			memcpy(cp, cAccountName, 10);
			cp += 10;

			dwp = (UINT32*)(cData + 10);
			*dwp = dwCharID;
			dwp = (UINT32*)(cData + 14);
			*dwp = MSGID_RESPONSE_CHARACTERLOG;
			wp = (UINT16*)(cData + 18);
			*wp = DEF_LOGRESMSGTYPE_NEWCHARACTERFAILED;
		}

		if (exists)
		{
			wsprintf(G_cTxt, "(X) Cannot create new character - Already existing character(%s)", cNewCharName);
			PutLogList(G_cTxt);

			cp = (char*)(cData);
			memcpy(cp, cAccountName, 10);
			cp += 10;

			dwp = (UINT32*)(cData + 10);
			*dwp = dwCharID;
			dwp = (UINT32*)(cData + 14);
			*dwp = MSGID_RESPONSE_CHARACTERLOG;
			wp = (UINT16*)(cData + 18);
			*wp = DEF_LOGRESMSGTYPE_ALREADYEXISTINGCHARACTER;

			SendEventToMLS(MSGID_RESPONSE_CHARACTERLOG, DEF_LOGRESMSGTYPE_ALREADYEXISTINGCHARACTER, cData, 20, -1);

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
				com.setCommandText("INSERT INTO Characters ([Account-ID], [Character-Name], [Character-Sex], [Character-Skin], [Character-Hair-Style], [Character-Hair-Colour], [Character-Underwear], [Character-Strength], [Character-Vitality], [Character-Dexterity], [Character-Intelligence], [Character-Magic], [Character-Charisma], [Character-Appr1], [Character-Appr2], [Character-Appr3], [Character-Appr4], [Character-Class]) VALUES(:1, :2, :3, :4, :5, :6, :7, :8, :9, :10, :11, :12, :13, :14, :15, :16, :17, :18)");
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

				if (cNewStr == 14)
				{
					com.Param(18).setAsLong() = 1; // War
				}
				else if (cNewMag == 14)
				{
					com.Param(18).setAsLong() = 2; // Mage
				}
				else if (cNewChr == 14)
				{
					com.Param(18).setAsLong() = 3; // Archer
				}

				com.Execute();
				com.Close();

				int iCharDBID = 0;

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
				getID.Close();


				addGold.setConnection(&con);
				addGold.setCommandText("INSERT INTO CharItems([Character-ID], ItemName, ItemNum1, ItemNum2, ItemNum3, ItemNum4, ItemNum5, [Item-Colour], ItemNum7, ItemNum8, ItemNum9, ItemNum10, [Item-Attribute], [Item-Element1], [Item-Element2], [Item-Element3], [Item-Element4]) VALUES (:1, :2, :3, :4, :5, :6, :7, :8, :9, :10, :11, :12, :13, :14, :15, :16, :17)");
				
				addGold.Param(1).setAsLong() = iCharDBID; // char id
				addGold.Param(2).setAsString() = "Dagger"; // item name
				addGold.Param(3).setAsLong() = 1; // item number 1 (amount)
				addGold.Param(4).setAsLong() = 0;
				addGold.Param(5).setAsLong() = 0;
				addGold.Param(6).setAsLong() = 0;
				addGold.Param(7).setAsLong() = 0;
				addGold.Param(8).setAsLong() = 0;
				addGold.Param(9).setAsLong() = 0;
				addGold.Param(10).setAsLong() = 0;
				addGold.Param(11).setAsLong() = 0;
				addGold.Param(12).setAsLong() = 300;
				addGold.Param(13).setAsLong() = 0;
				addGold.Param(14).setAsLong() = 0;
				addGold.Param(15).setAsLong() = 0;
				addGold.Param(16).setAsLong() = 0;
				addGold.Param(17).setAsLong() = 0;

				addGold.Execute();

				addGold.setCommandText("INSERT INTO CharItems([Character-ID], ItemName, ItemNum1, ItemNum2, ItemNum3, ItemNum4, ItemNum5, [Item-Colour], ItemNum7, ItemNum8, ItemNum9, ItemNum10, [Item-Attribute], [Item-Element1], [Item-Element2], [Item-Element3], [Item-Element4]) VALUES (:1, :2, :3, :4, :5, :6, :7, :8, :9, :10, :11, :12, :13, :14, :15, :16, :17)");
				
				addGold.Param(1).setAsLong() = iCharDBID; // char id
				addGold.Param(2).setAsString() = "RecallScroll"; // item name
				addGold.Param(3).setAsLong() = 1; // item number 1 (amount)
				addGold.Param(4).setAsLong() = 0;
				addGold.Param(5).setAsLong() = 0;
				addGold.Param(6).setAsLong() = 0;
				addGold.Param(7).setAsLong() = 0;
				addGold.Param(8).setAsLong() = 0;
				addGold.Param(9).setAsLong() = 0;
				addGold.Param(10).setAsLong() = 0;
				addGold.Param(11).setAsLong() = 0;
				addGold.Param(12).setAsLong() = 1;
				addGold.Param(13).setAsLong() = 0;
				addGold.Param(14).setAsLong() = 0;
				addGold.Param(15).setAsLong() = 0;
				addGold.Param(16).setAsLong() = 0;
				addGold.Param(17).setAsLong() = 0;

				addGold.Execute();

				addGold.setCommandText("INSERT INTO CharItems([Character-ID], ItemName, ItemNum1, ItemNum2, ItemNum3, ItemNum4, ItemNum5, [Item-Colour], ItemNum7, ItemNum8, ItemNum9, ItemNum10, [Item-Attribute], [Item-Element1], [Item-Element2], [Item-Element3], [Item-Element4]) VALUES (:1, :2, :3, :4, :5, :6, :7, :8, :9, :10, :11, :12, :13, :14, :15, :16, :17)");
				
				addGold.Param(1).setAsLong() = iCharDBID; // char id
				addGold.Param(2).setAsString() = "RedPotion"; // item name
				addGold.Param(3).setAsLong() = 1; // item number 1 (amount)
				addGold.Param(4).setAsLong() = 0;
				addGold.Param(5).setAsLong() = 0;
				addGold.Param(6).setAsLong() = 0;
				addGold.Param(7).setAsLong() = 0;
				addGold.Param(8).setAsLong() = 0;
				addGold.Param(9).setAsLong() = 0;
				addGold.Param(10).setAsLong() = 0;
				addGold.Param(11).setAsLong() = 0;
				addGold.Param(12).setAsLong() = 1;
				addGold.Param(13).setAsLong() = 0;
				addGold.Param(14).setAsLong() = 0;
				addGold.Param(15).setAsLong() = 0;
				addGold.Param(16).setAsLong() = 0;
				addGold.Param(17).setAsLong() = 0;

				addGold.Execute();

				addGold.setCommandText("INSERT INTO CharItems([Character-ID], ItemName, ItemNum1, ItemNum2, ItemNum3, ItemNum4, ItemNum5, [Item-Colour], ItemNum7, ItemNum8, ItemNum9, ItemNum10, [Item-Attribute], [Item-Element1], [Item-Element2], [Item-Element3], [Item-Element4]) VALUES (:1, :2, :3, :4, :5, :6, :7, :8, :9, :10, :11, :12, :13, :14, :15, :16, :17)");
				
				addGold.Param(1).setAsLong() = iCharDBID; // char id
				addGold.Param(2).setAsString() = "BluePotion"; // item name
				addGold.Param(3).setAsLong() = 1; // item number 1 (amount)
				addGold.Param(4).setAsLong() = 0;
				addGold.Param(5).setAsLong() = 0;
				addGold.Param(6).setAsLong() = 0;
				addGold.Param(7).setAsLong() = 0;
				addGold.Param(8).setAsLong() = 0;
				addGold.Param(9).setAsLong() = 0;
				addGold.Param(10).setAsLong() = 0;
				addGold.Param(11).setAsLong() = 0;
				addGold.Param(12).setAsLong() = 1;
				addGold.Param(13).setAsLong() = 0;
				addGold.Param(14).setAsLong() = 0;
				addGold.Param(15).setAsLong() = 0;
				addGold.Param(16).setAsLong() = 0;
				addGold.Param(17).setAsLong() = 0;

				addGold.Execute();

				addGold.setCommandText("INSERT INTO CharItems([Character-ID], ItemName, ItemNum1, ItemNum2, ItemNum3, ItemNum4, ItemNum5, [Item-Colour], ItemNum7, ItemNum8, ItemNum9, ItemNum10, [Item-Attribute], [Item-Element1], [Item-Element2], [Item-Element3], [Item-Element4]) VALUES (:1, :2, :3, :4, :5, :6, :7, :8, :9, :10, :11, :12, :13, :14, :15, :16, :17)");
				
				addGold.Param(1).setAsLong() = iCharDBID; // char id
				addGold.Param(2).setAsString() = "GreenPotion"; // item name
				addGold.Param(3).setAsLong() = 1; // item number 1 (amount)
				addGold.Param(4).setAsLong() = 0;
				addGold.Param(5).setAsLong() = 0;
				addGold.Param(6).setAsLong() = 0;
				addGold.Param(7).setAsLong() = 0;
				addGold.Param(8).setAsLong() = 0;
				addGold.Param(9).setAsLong() = 0;
				addGold.Param(10).setAsLong() = 0;
				addGold.Param(11).setAsLong() = 0;
				addGold.Param(12).setAsLong() = 1;
				addGold.Param(13).setAsLong() = 0;
				addGold.Param(14).setAsLong() = 0;
				addGold.Param(15).setAsLong() = 0;
				addGold.Param(16).setAsLong() = 0;
				addGold.Param(17).setAsLong() = 0;

				addGold.Execute();
				addGold.Close();
				
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

				dwp = (UINT32*)(cData + 10);
				*dwp = dwCharID;
				dwp = (UINT32*)(cData + 14);
				*dwp = MSGID_RESPONSE_CHARACTERLOG;
				wp = (UINT16*)(cData + 18);
				*wp = DEF_LOGRESMSGTYPE_NEWCHARACTERFAILED;
			}
		}

	}

	wsprintf(G_cTxt, "(!) Character Created: %s", cNewCharName);
	PutLogList(G_cTxt);

	ZeroMemory(cData, sizeof(cData));

	cp = (char*)(cData);
	memcpy(cData, cAccountName, 10);
	cp += 10;

	dwp = (UINT32*)(cData + 10);
	*dwp = dwCharID;

	dwp = (UINT32*)(cData + 14);
	*dwp = MSGID_RESPONSE_CHARACTERLOG;

	wp = (UINT16*)(cData + 18);
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

		if ((iGetCharacterData(cCharacterName, cMapName, &sAppr1, &sAppr2, &sAppr3, &sAppr4, &iApprColor, &cSex, &cSkin, &iLevel, &iExp, &iStr, &iVit, &iDex, &iInt, &iMag, &iCharisma, &iSaveYear, &iSaveMonth, &iSaveDay, &iSaveHour, &iSaveMinute)) != true) {
			*cp = 0;
			cp += 40;
			break;
		}
		*cp = 1;
		cp++;

		memcpy(cp, cMapName, 10);
		cp += 10;

		wp = (UINT16*)cp;
		*wp = (int)sAppr1;
		cp += 2;

		wp = (UINT16*)cp;
		*wp = (int)sAppr2;
		cp += 2;

		wp = (UINT16*)cp;
		*wp = (int)sAppr3;
		cp += 2;

		wp = (UINT16*)cp;
		*wp = (int)sAppr4;
		cp += 2;

		wp = (UINT16*)cp;
		*wp = (int)cSex;
		cp += 2;

		wp = (UINT16*)cp;
		*wp = (int)cSkin;
		cp += 2;

		wp = (UINT16*)cp;
		*wp = (int)iLevel;
		cp += 2;

		dwp = (UINT32*)cp;
		*dwp = (UINT32)iExp;
		cp += 4;

		wp = (UINT16*)cp;
		*wp = (int)iStr;
		cp += 2;

		wp = (UINT16*)cp;
		*wp = (int)iVit;
		cp += 2;

		wp = (UINT16*)cp;
		*wp = (int)iDex;
		cp += 2;

		wp = (UINT16*)cp;
		*wp = (int)iInt;
		cp += 2;

		wp = (UINT16*)cp;
		*wp = (int)iMag;
		cp += 2;

		wp = (UINT16*)cp;
		*wp = (int)iCharisma;
		cp += 2;

		ip = (int*)cp;
		*ip = iApprColor;
		cp += 4;

		wp = (UINT16*)cp;
		*wp = (int)iSaveYear;
		cp += 2;

		wp = (UINT16*)cp;
		*wp = (int)iSaveMonth;
		cp += 2;

		wp = (UINT16*)cp;
		*wp = (int)iSaveDay;
		cp += 2;

		wp = (UINT16*)cp;
		*wp = (int)iSaveHour;
		cp += 2;

		wp = (UINT16*)cp;
		*wp = (int)iSaveMinute;
		cp += 2;


	}

	SendEventToMLS(MSGID_RESPONSE_CHARACTERLOG, DEF_LOGRESMSGTYPE_NEWCHARACTERCREATED, cData, (cTotalChar * 65) + 32, -1);
}

void CWorldLog::RequestDeleteCharacter(int iClientH, char* pData)
{
	char cAccountName[11], cCharacterName[11], cInfo[111], cData[2000], cMapName[11], cCharList[256], cFileName[255], cTxt[100], cDir[100];
	char* cp;
	UINT32* dwp, dwCharID, iExp;
	UINT16* wp;
	short sAppr1, sAppr2, sAppr3, sAppr4;
	char cSex, cSkin, cTotalChar;
	int i, iLevel, iStr, iVit, iInt, iDex, iMag, iCharisma, iApprColor, iSaveYear, iSaveMonth, iSaveDay, iSaveHour, iSaveMinute;
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

	dwp = (UINT32*)cp;
	dwCharID = *dwp;
	cp += 4;

	memcpy(cCharacterName, cp, 10);
	cp += 10;

	cTotalChar = *cp;
	cp++;

	memcpy(cInfo, cp, cTotalChar * 11);

	if (G_bDBMode == true) {
		SACommand com, com2;

		try
		{
			com.setConnection(&con);
			com.setCommandText("DELETE FROM Characters WHERE [Character-Name] = :1");
			com.Param(1).setAsString() = cCharacterName;

			com.Execute();

			
			com.Close();

			com2.setConnection(&con);
			com2.setCommandText("DELETE FROM GuildMembers WHERE [Character-Name] = :1");
			com2.Param(1).setAsString() = cCharacterName;

			com2.Execute();

			com2.Close();

			char cTemp[256];
			ZeroMemory(cTemp, sizeof(cTemp));
			wsprintf(cTemp, "(!) Deleted Character From SQL: (%s)", cCharacterName);
			PutLogList(cTemp);

			ZeroMemory(cData, sizeof(cData));

			cp = (char*)(cData);
			memcpy(cData, cAccountName, 10);
			cp += 10;

			dwp = (UINT32*)cp; 
			*dwp = dwCharID;
			cp += 4;

			dwp = (UINT32*)cp; 
			*dwp = MSGID_RESPONSE_CHARACTERLOG;
			cp += 4;

			wp = (UINT16*)cp; 
			*wp = DEF_LOGRESMSGTYPE_NEWCHARACTERDELETED;
			cp += 2;

			*cp = 0;
			cp++;

			*cp = (char)cTotalChar;
			cp++;

			for (i = 0; i < cTotalChar; i++) {
				ZeroMemory(cMapName, sizeof(cMapName));
				ZeroMemory(cCharList, sizeof(cCharList));
				memcpy(cCharList, &cInfo[i * 11], 10);
				memcpy(cp, &cInfo[i * 11], 11);
				cp += 11;

				if ((iGetCharacterData(cCharList, cMapName, &sAppr1, &sAppr2, &sAppr3, &sAppr4, &iApprColor, &cSex, &cSkin, &iLevel, &iExp, &iStr, &iVit, &iDex, &iInt, &iMag, &iCharisma, &iSaveYear, &iSaveMonth, &iSaveDay, &iSaveHour, &iSaveMinute)) != true) {
					*cp = 0;
					cp += 40;
					break;
				}

				*cp = 1;
				cp++;

				wp = (UINT16*)cp;
				*wp = (int)sAppr1;
				cp += 2;

				wp = (UINT16*)cp;
				*wp = (int)sAppr2;
				cp += 2;

				wp = (UINT16*)cp;
				*wp = (int)sAppr3;
				cp += 2;

				wp = (UINT16*)cp;
				*wp = (int)sAppr4;
				cp += 2;

				wp = (UINT16*)cp;
				*wp = (int)cSex;
				cp += 2;

				wp = (UINT16*)cp;
				*wp = (int)cSkin;
				cp += 2;

				wp = (UINT16*)cp;
				*wp = (int)iLevel;
				cp += 2;

				dwp = (UINT32*)cp;
				*dwp = (UINT32)iExp;
				cp += 4;

				wp = (UINT16*)cp;
				*wp = (int)iStr;
				cp += 2;

				wp = (UINT16*)cp;
				*wp = (int)iVit;
				cp += 2;

				wp = (UINT16*)cp;
				*wp = (int)iDex;
				cp += 2;

				wp = (UINT16*)cp;
				*wp = (int)iInt;
				cp += 2;

				wp = (UINT16*)cp;
				*wp = (int)iMag;
				cp += 2;

				wp = (UINT16*)cp;
				*wp = (int)iCharisma;
				cp += 2;

				ip = (int*)cp;
				*ip = iApprColor;
				cp += 4;

				wp = (UINT16*)cp;
				*wp = (int)iSaveYear;
				cp += 2;

				wp = (UINT16*)cp;
				*wp = (int)iSaveMonth;
				cp += 2;

				wp = (UINT16*)cp;
				*wp = (int)iSaveDay;
				cp += 2;

				wp = (UINT16*)cp;
				*wp = (int)iSaveHour;
				cp += 2;

				wp = (UINT16*)cp;
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

			char cTemp2[256];
			ZeroMemory(cTemp2, sizeof(cTemp2));
			wsprintf(cTemp2, "(!!!) SQL SERVER ERROR (RequestDeleteCharacter): %s", (const char*)x.ErrText());
			PutLogList(cTemp2);

			return;
		}
	}
	SendEventToMLS(MSGID_RESPONSE_CHARACTERLOG, DEF_LOGRESMSGTYPE_NEWCHARACTERDELETED, cData, (cTotalChar * 65) + 20, -1);
}
void CWorldLog::UpdateGuildsmanSQL(char* cMemberName, char* cGuildName, bool bUpdateType)
{
	// NOTE: bUpdateType seems to me guildmaster delete or just member delete
	// for example if updatetype = 0 then delete guild and all members --- only guildmaster can do this
}

int CWorldLog::UpdateGuildFile_NewGuildsMan(char* cFile, char* pData, char* cp)
{
	// INCOMPLETE
	// hypnotoad: useless function?
	return 0;
}

int CWorldLog::UpdateGuildFile_DeleteGuildsMan(char* cFile, char* pData, char* cp)
{
	// INCOMPLETE
	// hypnotoad: useless function?
	return 0;
}

void CWorldLog::RequestCreateNewGuild(int iClientH, char* pData)
{
	char cFileName[255], cData[500];
	char cTxt[500];
	char cTxt2[100];
	char cGuildMasterName[11], cGuildLocation[11], cDir[255], cGuildName[21];
	char* cp;
	UINT32* dwp, dwGuildGUID;
	UINT16* wp;
	int iRet;
	

	if (m_pClientList[iClientH] == 0) return;
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

	dwp = (UINT32*)(pData + 66);
	dwGuildGUID = *dwp;
	cp += 4;

	if (G_bDBMode == true) {
		bool exists = bCheckGuildExists(cGuildName);

		if (exists)
		{
			wsprintf(cTxt2, "(X) Cannot create new guild - Already existing guild name: Name(%s)", cFileName);
			PutLogList(cTxt2);

			cp = (char*)(cData + 16);

			dwp = (UINT32*)cData;
			*dwp = MSGID_RESPONSE_CREATENEWGUILD;

			wp = (UINT16*)(cData + DEF_INDEX2_MSGTYPE);
			*wp = DEF_MSGTYPE_REJECT;

			cp = (char*)(cData + DEF_INDEX2_MSGTYPE + 2);
			memcpy(cp, cGuildMasterName, 10);
			cp += 10;

			iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 16, DEF_USE_ENCRYPTION);
			
		}
		else
		{
			SACommand com;

			try
			{
				com.setConnection(&con);
				com.setCommandText("INSERT INTO Guilds ([Guild-Name], [Guild-GUID], [Guild-Location], [Guild-Master]) VALUES (:1, :2, :3, :4)");
				com.Param(1).setAsString() = cGuildName;
				com.Param(2).setAsLong() = dwGuildGUID;
				com.Param(3).setAsString() = cGuildLocation;
				com.Param(4).setAsString() = cGuildMasterName;

				com.Execute();

				com.Close();

				dwp = (UINT32*)cData;
				*dwp = MSGID_RESPONSE_CREATENEWGUILD;

				wp = (UINT16*)(cData + DEF_INDEX2_MSGTYPE);
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

				dwp = (UINT32*)cData;
				*dwp = MSGID_RESPONSE_CREATENEWGUILD;

				wp = (UINT16*)(cData + DEF_INDEX2_MSGTYPE);
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
			m_pClientList[iClientH] = 0;
			break;
		}
	}
}

void CWorldLog::RequestDisbandGuild(int iClientH, char* pData)
{
	char cTemp[500];
	UINT32* dwp;
	UINT16* wp;
	char* cp;
	char cTxt[100];
	char cGuildMasterName[11], cGuildName[21];
	int iRet;

	if (m_pClientList[iClientH] == 0) return;
	
	ZeroMemory(cTxt, sizeof(cTxt));
	
	ZeroMemory(cTemp, sizeof(cTemp));
	ZeroMemory(cGuildMasterName, sizeof(cGuildMasterName));
	ZeroMemory(cGuildName, sizeof(cGuildName));

	cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);
	memcpy(cGuildMasterName, cp, 10);
	cp += 30;

	memcpy(cGuildName, cp, 20);
	cp += 20;

	if (G_bDBMode == true) {
		SACommand com, com2;

		try
		{
			com.setConnection(&con);
			com.setCommandText("DELETE FROM Guilds WHERE [Guild-Name] = :1");
			com.Param(1).setAsString() = cGuildName;

			com.Execute();

			com.Close();

			com2.setConnection(&con);
			com2.setCommandText("DELETE FROM GuildMembers WHERE [Guild-Name] = :1");
			com2.Param(1).setAsString() = cGuildName;

			com2.Execute();

			com2.Close();

			char cTemp[256];
			ZeroMemory(cTemp, sizeof(cTemp));
			wsprintf(cTemp, "(!) Deleted Guild From SQL: (%s)", cGuildName);

			dwp = (UINT32*)(cTemp);
			*dwp = MSGID_RESPONSE_DISBANDGUILD;

			wp = (UINT16*)(cTemp + DEF_INDEX2_MSGTYPE);
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

			dwp = (UINT32*)(cTemp);
			*dwp = MSGID_RESPONSE_DISBANDGUILD;

			wp = (UINT16*)(cTemp + DEF_INDEX2_MSGTYPE);
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
		m_pClientList[iClientH] = 0;
		break;
	}
}
void CWorldLog::UpdateGuildInfoNewGuildsman(int iClientH, char* pData)
{
	char* cp, cCharacterName[11], cGuildName[21];
	int iGuildID = -1;

	if (m_pClientList[iClientH] == 0) return;

	ZeroMemory(cCharacterName, sizeof(cCharacterName));
	ZeroMemory(cGuildName, sizeof(cGuildName));
	
	cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);
	
	memcpy(cCharacterName, cp, 10);
	cp += 10;

	memcpy(cGuildName, cp, 20);
	cp += 20;

	if (G_bDBMode == true) 
	{
			SACommand com;

			try
			{
				com.setConnection(&con);
				com.setCommandText("INSERT INTO GuildMembers ([Guild-Name], [Character-Name]) VALUES (:1, :2)");
				com.Param(1).setAsString() = cGuildName;
				com.Param(2).setAsString() = cCharacterName;

				com.Execute();

				com.Close();

				wsprintf(G_cTxt, "(!) New Guildsman added: (%s) Guild(%s)", cCharacterName, cGuildName);
				PutLogList(G_cTxt);

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
			}
		
	}
}

void CWorldLog::UpdateGuildInfoDeleteGuildman(int iClientH, char* pData)
{
	char* cp, cCharacterName[11], cGuildName[21];
	int iGuildID;

	if (m_pClientList[iClientH] == 0) return;

	ZeroMemory(cCharacterName, sizeof(cCharacterName));
	ZeroMemory(cGuildName, sizeof(cGuildName));

	cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);

	memcpy(cCharacterName, cp, 10);
	cp += 10;

	memcpy(cGuildName, cp, 20);
	cp += 20;

	if (G_bDBMode == true)
	{
		SACommand com;

		try
		{
			com.setConnection(&con);
			com.setCommandText("DELETE FROM GuildMembers WHERE [Character-Name] = :1");
			com.Param(1).setAsString() = cCharacterName;

			com.Execute();

			com.Close();

			wsprintf(G_cTxt, "(!) Guildsman deleted: (%s) Guild(%s)", cCharacterName, cGuildName);
			PutLogList(G_cTxt);

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
		}

	}
}

int CWorldLog::OnPlayerAccountMessage(UINT32 dwMsgID, char* cAccountName, char* cPassword, int iLevel, char* pData3)
{
	int* ip, i, x, iClientH, iRet;
	UINT32* dwp;
	char* cp, cData[120];
	UINT16* wp;

	switch (dwMsgID) {

	case DEF_MSGACCOUNTSET_NULL:
		for (i = 0; i < DEF_MAXPLAYERACCOUNTS; i++) {
			if (m_pAccountList[i] != 0) {
				if (strcmp(m_pAccountList[i]->cAccountName, cAccountName) == 0) {
					wsprintf(G_cTxt, "(TestLog) Set account(%s) connection status 0...", m_pAccountList[i]->cAccountName);
					PutLogList(G_cTxt);
					delete m_pAccountList[i];
					m_pAccountList[i] = 0;
					return 1;
				}
			}
		}
		break;

	case DEF_MSGACCOUNTSET_INIT:
		for (i = 0; i < DEF_MAXPLAYERACCOUNTS; i++) { //1
			if (m_pAccountList[i] != 0) { //2
				ZeroMemory(m_pAccountList[i]->cPassword, sizeof(m_pAccountList[i]->cPassword));
				if (strcmp(m_pAccountList[i]->cAccountName, cAccountName) == 0) { //3 this is buged
					if (m_pAccountList[i]->cAccountType == 3) { //4
						strcpy(m_pAccountList[i]->cPassword, cPassword);
						m_pAccountList[i]->m_iLevel = iLevel;
						iClientH = -1;
						for (x = 0; x < DEF_MAXGAMESERVERS; x++) { //5
							if ((m_pGameList[x] != 0) && (strcmp(m_pGameList[x]->m_cGameServerName, m_pAccountList[i]->m_cOnGameServer) == false)) {//6
								iClientH = m_pGameList[x]->m_iTrackSock;
							}//6
						}//5
						if ((iClientH != -1) && (m_pClientList[iClientH] != 0)) { // 7
							ZeroMemory(cData, sizeof(cData));

							dwp = (UINT32*)(cData);
							*dwp = MSGID_REQUEST_CHECKACCOUNTPASSWORD;

							wp = (UINT16*)(cData + DEF_INDEX2_MSGTYPE);
							*wp = DEF_MSGTYPE_CONFIRM;

							cp = (char*)(cData + DEF_INDEX2_MSGTYPE + 2);

							memcpy(cp, cAccountName, 10);
							cp += 10;

							memcpy(cp, cPassword, 10);
							cp += 10;

							ip = (int*)cp;
							*ip = iLevel;
							cp += 4;

							iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 30, DEF_USE_ENCRYPTION);
							switch (iRet) { // 8
							case DEF_XSOCKEVENT_QUENEFULL:
							case DEF_XSOCKEVENT_SOCKETERROR:
							case DEF_XSOCKEVENT_CRITICALERROR:
							case DEF_XSOCKEVENT_SOCKETCLOSED:
								DeleteClient(iClientH, 0);
								delete m_pClientList[iClientH];
								m_pClientList[iClientH] = 0;
								return 0;
							}//8			
						}//7
						return 1;
					}//4
					m_pAccountList[i]->cAccountType = 1;
					m_pAccountList[i]->dwLogInTime = timeGetTime();
					wsprintf(G_cTxt, "(TestLog) Set account(%s)-test(%s)-Level(%d) connection status 1 INIT...", m_pAccountList[i]->cAccountName, m_pAccountList[i]->cPassword, iLevel);
					PutLogList(G_cTxt);
					return 1;
				}//3
			}//2
			if (m_pAccountList[i] == 0) {
				m_pAccountList[i] = new class CAccount(cAccountName, timeGetTime());
				m_pAccountList[i]->cAccountType = 1;
				strcpy(m_pAccountList[i]->cPassword, cPassword);
				m_pAccountList[i]->m_iLevel = (int)iLevel;
				wsprintf(G_cTxt, "(TestLog) Set account(%s)-test(%s)-Level(%d) connection status 2 INIT(create & set)...", m_pAccountList[i]->cAccountName, m_pAccountList[i]->cPassword, iLevel);
				PutLogList(G_cTxt);
				m_iTotalPlayers++;
				return 1;
			}
		}//1 end loop accounts
		break;

	case DEF_MSGACCOUNTSET_WAIT:
		for (i = 0; i < DEF_MAXPLAYERACCOUNTS; i++) {
			if (m_pAccountList[i] != 0) {
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
			if (m_pAccountList[i] == 0) {
				m_pAccountList[i] = new class CAccount(cAccountName, timeGetTime());
				m_pAccountList[i]->cAccountType = 2;
				m_pAccountList[i]->m_iLevel = iLevel;
				wsprintf(G_cTxt, "(TestLog) Set account(%s) connection status WAIT(create & set)...", cAccountName);
				PutLogList(G_cTxt);
				return 1;
			}
		}
		break;

	case DEF_MSGACCOUNTSET_PLAYING:
		for (i = 0; i < DEF_MAXPLAYERACCOUNTS; i++) {
			if (m_pAccountList[i] != 0) {
				if (strcmp(m_pAccountList[i]->cAccountName, cAccountName) == 0) {
					m_pAccountList[i]->cAccountType = 3;
					ZeroMemory(m_pAccountList[i]->m_cOnGameServer, sizeof(m_pAccountList[i]->m_cOnGameServer));
					memcpy(m_pAccountList[i]->m_cOnGameServer, pData3, 10);
					wsprintf(G_cTxt, "(TestLog) Set account(%s) connection status PLAYING...", cAccountName);
					PutLogList(G_cTxt);
					return 1;
				}
			}
			if (m_pAccountList[i] == 0) {
				m_pAccountList[i] = new class CAccount(cAccountName, timeGetTime());
				m_pAccountList[i]->cAccountType = 3;
				ZeroMemory(m_pAccountList[i]->m_cOnGameServer, sizeof(m_pAccountList[i]->m_cOnGameServer));
				memcpy(m_pAccountList[i]->m_cOnGameServer, pData3, 10);
				wsprintf(G_cTxt, "(TestLog) Set account(%s) connection status PLAYING(create & set)...", cAccountName);
				PutLogList(G_cTxt);
				return 2;
			}
		}//account loop
		break;
	}
	return 0;
}

int CWorldLog::iGetCharacterInformation(char* cAccountName, char* Password, int* iLevel)
{
	int i;

	for (i = 0; i < DEF_MAXPLAYERACCOUNTS; i++) {
		if ((m_pAccountList[i] != 0) && (strcmp(m_pAccountList[i]->cAccountName, cAccountName) == 0)) {
			strcpy(Password, m_pAccountList[i]->cPassword);
			*iLevel = m_pAccountList[i]->m_iLevel;
			return m_pAccountList[i]->cAccountType;
		}
	}
	return 0;
}

void CWorldLog::SetAccountStatusInit(int iClientH, char* pData)
{
	char cAccountName[11], cPassword[11];
	char* cp;
	int* ip, iLevel, iRet;

	ZeroMemory(cAccountName, sizeof(cAccountName));
	ZeroMemory(cPassword, sizeof(cPassword));

	cp = (char*)(pData + 6);
	memcpy(cAccountName, cp, 10);
	cp += 10;

	memcpy(cPassword, cp, 10);
	cp += 10;

	ip = (int*)cp;
	iLevel = *ip;
	cp += 4;

	iRet = OnPlayerAccountMessage(DEF_MSGACCOUNTSET_INIT, cAccountName, cPassword, iLevel, 0);
}

void CWorldLog::CheckClientTimeout()
{
	int i;
	UINT32 dwTime;
	char cData[100];
	char* cp;

	for (i = 0; i < DEF_MAXPLAYERACCOUNTS; i++)
		if (m_pAccountList[i] != 0) {
			if (m_pAccountList[i]->Timeout < 0) break; //if timer set no wait
			if (m_pAccountList[i]->Timeout == 0) { //client timedout

				ZeroMemory(cData, sizeof(cData));

				cp = (char*)(cData);
				memcpy(cp, m_pAccountList[i]->cAccountName, 10);
				cp += 10;

				wsprintf(G_cTxt, "(TestLog) Account(%s) TIMEOUT - Clear conn status request to MLS...", m_pAccountList[i]->cAccountName);
				PutLogList(G_cTxt);

				SendEventToMLS(MSGID_REQUEST_CLEARACCOUNTSTATUS, DEF_MSGTYPE_CONFIRM, m_pAccountList[i]->cAccountName, 10, -1); //send Main account null
				OnPlayerAccountMessage(DEF_MSGACCOUNTSET_NULL, m_pAccountList[i]->cAccountName, 0, 0, 0); // set account null
			}
			m_pAccountList[i]->Timeout--;
		}

	G_cClientCheckCount++;
	if (G_cClientCheckCount > 10) {
		G_cClientCheckCount = 0;
		dwTime = timeGetTime();
		for (i = 0; i < DEF_MAXPLAYERACCOUNTS; i++) {
			if (m_pAccountList[i] == 0) break;
			if ((m_pAccountList[i]->cAccountType != 1) && (m_pAccountList[i]->cAccountType != 2)) break; 
			if ((dwTime - m_pAccountList[i]->dwLogInTime) <= 20000) break; // has to be over 20seconds. to continue

			ZeroMemory(cData, sizeof(cData));

			cp = (char*)(cData);
			memcpy(cp, m_pAccountList[i]->cAccountName, 10);
			cp += 10;

			wsprintf(G_cTxt, "(TestLog) Account(%s) TIMEOUT - Clear conn status request to MLS...", m_pAccountList[i]->cAccountName);
			PutLogList(G_cTxt);

			SendEventToMLS(MSGID_REQUEST_CLEARACCOUNTSTATUS, DEF_MSGTYPE_CONFIRM, m_pAccountList[i]->cAccountName, 10, -1);

			OnPlayerAccountMessage(DEF_MSGACCOUNTSET_NULL, m_pAccountList[i]->cAccountName, 0, 0, 0);
		}
	}
}

void CWorldLog::ForceDisconnectAccount(int iClientH, char* pData)
{
	char cAccountName[11], cGameServerName[11], cData[120];
	char* cp;
	int i, x, iRet;
	UINT16* wp, wCount;
	UINT32* dwp;

	wp = (UINT16*)(pData + 4);
	wCount = *wp;

	cp = (char*)(pData + 6);
	ZeroMemory(cAccountName, sizeof(cAccountName));
	memcpy(cAccountName, cp, 10);

	for (i = 0; i < DEF_MAXPLAYERACCOUNTS; i++) {
		if (m_pAccountList[i] != 0) {
			if (memcmp(m_pAccountList[i]->cAccountName, cAccountName, 10) == 0) { // == true
				if (m_pAccountList[i]->cAccountType == 1) return;
				if (m_pAccountList[i]->cAccountType == 2) return;
				ZeroMemory(cGameServerName, sizeof(cGameServerName));
				memcpy(cGameServerName, m_pAccountList[i]->m_cOnGameServer, 10);
				for (x = 0; x < DEF_MAXGAMESERVERS; x++) {
					if (m_pGameList[x] != 0) {
						if (memcmp(m_pGameList[x]->m_cGameServerName, cGameServerName, 10) == 0) { // == true
							iClientH = m_pGameList[x]->m_iTrackSock;

							ZeroMemory(cData, sizeof(cData));

							dwp = (UINT32*)cData;
							*dwp = MSGID_REQUEST_FORCEDISCONECTACCOUNT;

							wp = (UINT16*)(cData + DEF_INDEX2_MSGTYPE);
							*wp = wCount;

							cp = (char*)(cData + DEF_INDEX2_MSGTYPE + 2);
							memcpy(cp, cAccountName, 10);
							cp += 10;

							wsprintf(G_cTxt, "(TestLog) Force Disconnect Account(%s) in Game Server(%s)... Count(%d)", cAccountName, cGameServerName, wCount);
							PutLogList(G_cTxt);

							iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 16, DEF_USE_ENCRYPTION);

							switch (iRet) {
							case DEF_XSOCKEVENT_QUENEFULL:
							case DEF_XSOCKEVENT_SOCKETERROR:
							case DEF_XSOCKEVENT_CRITICALERROR:
							case DEF_XSOCKEVENT_SOCKETCLOSED:
								DeleteClient(iClientH, 0);
								delete m_pClientList[iClientH];
								m_pClientList[iClientH] = 0;
								break;
							}
							return;
						}
					}
				}
			}
		}
	}
	ZeroMemory(cData, sizeof(cData));
	cp = (char*)cData;
	memcpy(cp, cAccountName, 10);
	cp += 10;
	wsprintf(G_cTxt, "(TestLog) Account(%s) log out request to MLS...", cAccountName);
	PutLogList(G_cTxt);
	SendEventToMLS(MSGID_REQUEST_CLEARACCOUNTSTATUS, DEF_MSGTYPE_CONFIRM, cData, 10, -1);

}

int CWorldLog::RequestSetAccountWaitStatus(char* pData)
{
	char cAccountName[11];
	char* cp;
	int iLevel, * ip;
	int iRet;

	ZeroMemory(cAccountName, sizeof(cAccountName));
	cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);

	memcpy(cAccountName, cp, 10);
	cp += 10;

	ip = (int*)cp;
	iLevel = *ip;
	cp += 4;

	iRet = OnPlayerAccountMessage(DEF_MSGACCOUNTSET_WAIT, cAccountName, 0, iLevel, 0);
	return iRet;

}

void CWorldLog::RegisterWorldGameServer()
{
	char cData[1101], cTemp[1200];
	char* cp, * cp2;
	UINT32* dwp;
	int i, x;
	char cTotalMaps;

	for (i = 0; i < DEF_MAXGAMESERVERS; i++) {
		if (m_pGameList[i] != 0) {
			ZeroMemory(cData, sizeof(cData));
			cp2 = (char*)(cData);
			cTotalMaps = 0;
			for (x = 0; x < DEF_MAXGAMESERVERSMAPS; x++) {
				if ((m_cMapList[x] != 0) && (i == m_cMapList[x]->iIndex)) {
					memcpy(cp2, m_cMapList[i]->m_cName, 10);
					cp2 += 11;
					cTotalMaps++;
				}
			}

			ZeroMemory(cTemp, sizeof(cTemp));
			cp = (char*)(cTemp);

			memcpy(cp, m_cWorldLogName, 30);
			cp += 30;

			memcpy(cp, m_pGameList[i]->m_cGameServerName, 10);
			cp += 10;

			memcpy(cp, m_pGameList[i]->m_cGameServerAddress, 16);
			cp += 16;

			dwp = (UINT32*)cp;
			*dwp = m_pGameList[i]->m_iGameServerPort;
			cp += 4;

			dwp = (UINT32*)cp;
			*dwp = cTotalMaps;
			cp += 4;

			memcpy(cp, cData, cTotalMaps * 11);
			cp += cTotalMaps * 11;

			wsprintf(G_cTxt, "(!) Game Server Registration to MLS... ( Name:%s | Addr:%s | Port:%d )", m_pGameList[i]->m_cGameServerName, m_pGameList[i]->m_cGameServerAddress, m_pGameList[i]->m_iGameServerPort);
			PutLogList(G_cTxt);

			SendEventToMLS(MSGID_REGISTER_WORLDSERVER_GAMESERVER, DEF_MSGTYPE_CONFIRM, cTemp, 64 + (cTotalMaps * 11), -1);
		}
	}
}

void CWorldLog::ClientMSLConfirmed(int iClientH)
{
	m_bisMainRegistered[iClientH] = true;
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

int CWorldLog::iCreateNewCharacterSQL(char* cCharacterName, char* cAccountName, char* pData, bool bGuildCheck)
{
	// REFERENCE: sub_414DB0
	return -1;
}

int CWorldLog::iSaveCharacterSQL(UINT32 dwCharID, char* pData)
{
	return -1;
}


bool CWorldLog::bReadItemConfigFile(char* cFn)
{
	FILE* pFile;
	HANDLE hFile;
	UINT32  dwFileSize;
	char* cp, * token, cReadModeA, cReadModeB;
	char seps[] = "= \t\n";
	class CStrTok* pStrTok;
	int iItemH;

	cReadModeA = 0;
	cReadModeB = 0;
	pFile = 0;
	hFile = CreateFileA(cFn, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
	dwFileSize = GetFileSize(hFile, 0);
	if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);
	pFile = fopen(cFn, "rt");
	if (pFile == 0) {
		wsprintf(G_cTxt, "(X) CRITICAL ERROR! Cannot open (%s) file !", cFn);
		PutLogList(G_cTxt);
		return false;
	}
	else {
		wsprintf(G_cTxt, "(!) Reading %s config file...", cFn);
		PutLogList(G_cTxt);
		cp = new char[dwFileSize + 1];
		ZeroMemory(cp, dwFileSize + 1);
		fread(cp, dwFileSize, 1, pFile);
		pStrTok = new class CStrTok(cp, seps);
		token = pStrTok->pGet();
		while (token != 0) {
			if (cReadModeA != 0) {
				switch (cReadModeA) {
				case 1:
					switch (cReadModeB) {
					case 1:
						iItemH = atoi(token);
						if (m_pItemList[iItemH] != 0) {
							wsprintf(G_cTxt, "(!!!) CRITICAL ERROR! Duplicate ItemIDnum(%d)", iItemH);
							PutLogList(G_cTxt);
							delete[] cp;
							delete pStrTok;
							return false;
						}
						m_pItemList[iItemH] = new class CItem();
						cReadModeB = 2;
						break;

					case 2:
						ZeroMemory(m_pItemList[iItemH]->m_cItemName, sizeof(m_pItemList[iItemH]->m_cItemName));
						memcpy(m_pItemList[iItemH]->m_cItemName, token, strlen(token));
						wsprintf(G_cTxt, "(O) ItemName(%s)", token);

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
				if (memcmp(token, "Item", 4) == 0) {
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
		delete[] cp;
		fclose(pFile);
	}

	PutLogList(" ");
	return true;
}

int CWorldLog::iGetItemNumber(char* cItemName)
{
	int i;

	for (i = 0; i < DEF_MAXITEMS; i++) {
		if (m_pItemList[i] != 0) {
			if (strcmp(m_pItemList[i]->m_cItemName, cItemName) == 0) {
				return i;
			}
		}
	}
	return 0;
}

void CWorldLog::PutGMLogData(char* pData, UINT32 dwMsgSize, bool bIsSave)
{
	FILE* pFile;
	char cBuffer[512];
	SYSTEMTIME SysTime;
	UINT32 dwTime;

	pFile = 0;
	dwTime = timeGetTime();
	if (bIsSave == 0) {

		if (m_cGMLogCount == 0) m_dwGMLogTime = dwTime;
		if (dwMsgSize <= 6) return;
		*(pData + dwMsgSize) = 0;
		m_cGMLogCount++;
		GetLocalTime(&SysTime);
		ZeroMemory(cBuffer, sizeof(cBuffer));
		wsprintf(cBuffer, "%04d:%02d:%02d:%02d:%02d:%02d\t", SysTime.wYear, SysTime.wMonth, SysTime.wDay, SysTime.wHour, SysTime.wMinute, SysTime.wSecond);
		strcat(cBuffer, pData + 6);
		strcat(cBuffer, "\n");
		strcat(m_cGMLogBuffer, cBuffer);
	}
	else {
		if (m_cGMLogCount == 0) return;
		GetLocalTime(&SysTime);
	}
	if ((m_cGMLogCount < 30) && ((dwTime - m_dwGMLogTime) <= 5000)) {
		bIsSave = true;
		return;
	}
	ZeroMemory(cBuffer, sizeof(cBuffer));
	CreateDirectory("GMLogData", 0);
	wsprintf(cBuffer, "GMLogData\\GMEvents%04d%02d%02d.log", SysTime.wYear, SysTime.wMonth, SysTime.wDay);
	pFile = fopen(cBuffer, "at");
	if (pFile == 0) return;
	fwrite(m_cGMLogBuffer, 1, strlen(m_cGMLogBuffer), pFile);
	fclose(pFile);
	m_cGMLogCount = 0;
	ZeroMemory(m_cGMLogBuffer, sizeof(m_cGMLogBuffer));

}

void CWorldLog::PutItemLogData(char* pData, UINT32 dwMsgSize, bool bIsSave)
{
	FILE* pFile;
	char cBuffer[512];
	SYSTEMTIME SysTime;
	UINT32 dwTime;

	pFile = 0;
	dwTime = timeGetTime();
	if (bIsSave == 0) {

		if (m_cItemLogCount == 0) m_dwItemLogTime = dwTime;
		if (dwMsgSize <= 6) return;
		*(pData + dwMsgSize) = 0;
		m_cItemLogCount++;
		GetLocalTime(&SysTime);
		ZeroMemory(cBuffer, sizeof(cBuffer));
		wsprintf(cBuffer, "%04d:%02d:%02d:%02d:%02d:%02d\t", SysTime.wYear, SysTime.wMonth, SysTime.wDay, SysTime.wHour, SysTime.wMinute, SysTime.wSecond);
		strcat(cBuffer, pData + 6);
		strcat(cBuffer, "\n");
		strcat(m_cItemLogBuffer, cBuffer);
	}
	else {
		if (m_cItemLogCount == 0) return;
		GetLocalTime(&SysTime);
	}
	if ((m_cItemLogCount < 150) && ((dwTime - m_dwItemLogTime) <= 5000)) {
		bIsSave = true;
		return;
	}
	ZeroMemory(cBuffer, sizeof(cBuffer));
	CreateDirectory("ItemLogData", 0);
	wsprintf(cBuffer, "ItemLogData\\ItemEvents%04d%02d%02d.log", SysTime.wYear, SysTime.wMonth, SysTime.wDay);
	pFile = fopen(cBuffer, "at");
	if (pFile == 0) return;
	fwrite(m_cItemLogBuffer, 1, strlen(m_cItemLogBuffer), pFile);
	fclose(pFile);
	m_cItemLogCount = 0;
	ZeroMemory(m_cItemLogBuffer, sizeof(m_cItemLogBuffer));
}

void CWorldLog::PutCrusadeLogData(char* pData, UINT32 dwMsgSize, bool bIsSave)
{
	FILE* pFile;
	char cBuffer[512];
	SYSTEMTIME SysTime;
	UINT32 dwTime;

	pFile = 0;
	dwTime = timeGetTime();
	if (bIsSave == 0) {

		if (m_cCrusadeLogCount == 0) m_dwItemLogTime = dwTime;
		if (dwMsgSize <= 6) return;
		*(pData + dwMsgSize) = 0;
		m_cCrusadeLogCount++;
		GetLocalTime(&SysTime);
		ZeroMemory(cBuffer, sizeof(cBuffer));
		wsprintf(cBuffer, "%04d:%02d:%02d:%02d:%02d:%02d\t", SysTime.wYear, SysTime.wMonth, SysTime.wDay, SysTime.wHour, SysTime.wMinute, SysTime.wSecond);
		strcat(cBuffer, pData + 6);
		strcat(cBuffer, "\n");
		strcat(m_cCrusadeLogBuffer, cBuffer);
	}
	else {
		if (m_cCrusadeLogCount == 0) return;
		GetLocalTime(&SysTime);
	}
	if ((m_cCrusadeLogCount < 100) && ((dwTime - m_dwCrusadeLogTime) <= 5000)) {
		bIsSave = true;
		return;
	}
	ZeroMemory(cBuffer, sizeof(cBuffer));
	CreateDirectory("CrusadeLogData", 0);
	wsprintf(cBuffer, "CrusadeLogData\\CrusadeEvents%04d%02d%02d.log", SysTime.wYear, SysTime.wMonth, SysTime.wDay);
	pFile = fopen(cBuffer, "at");
	if (pFile == 0) return;
	fwrite(m_cCrusadeLogBuffer, 1, strlen(m_cCrusadeLogBuffer), pFile);
	fclose(pFile);
	m_cCrusadeLogCount = 0;
	ZeroMemory(m_cCrusadeLogBuffer, sizeof(m_cCrusadeLogBuffer));
}

void CWorldLog::RequestSaveOccupyFlag(int iClientH, char* pData, char cType)
{

}


void CWorldLog::VerifyCharacterIntegrity(char* cCharacterName, char* cAccountName, int* iLevel, char* cGuildName, char* cGuildRank, UINT32* dwGuildGUID)
{
	
	
	char cCharacterMap[11];

	if (G_bDBMode == true)
	{
		SACommand com;

		try
		{
			com.setConnection(&con);
			com.setCommandText("SELECT [Character-ID], [Account-ID], RTRIM([Character-Name]) AS 'Character-Name', [Character-Location], [Character-Guild-Name], [Character-Guild-GUID], [Character-Guild-Rank], [Character-Loc-Map], [Character-Loc-X], [Character-Loc-Y], [Character-HP], [Character-MP], [Character-SP], [Character-Level], [Character-Rating], [Character-Strength], [Character-Intelligence], [Character-Vitality], [Character-Dexterity], [Character-Magic], [Character-Charisma], [Character-Luck], [Character-Experience], [Character-LU-Pool], [Character-Ek-Count], [Character-Pk-Count], [Character-Reward-Gold], [Character-ID1], [Character-ID2], [Character-ID3], [Character-Sex], [Character-Skin], [Character-Hair-Style], [Character-Hair-Colour], [Character-Underwear], [Character-Hunger], [Character-Shutup-Time], [Character-Rating-Time], [Character-Force-Time], [Character-SP-Time], [Character-Admin-Level], [Character-Contribution], [Character-War-Contribution], [Character-Event-ID], [Character-Criticals], [Character-Ability-Time], [Character-Lock-Map], [Character-Lock-Time], [Character-Crusade-Job], [Character-Crusade-GUID], [Character-Construct-Points], [Character-Death-Time], [Character-Party-ID], [Character-Majestics], [Character-Appr1], [Character-Appr2], [Character-Appr3], [Character-Appr4], [Character-Appr-Colour] FROM Characters WHERE [Character-Name] = :1");
			com.Param(1).setAsString() = cCharacterName;

			com.Execute();

			if (com.isResultSet())
			{

				while (com.FetchNext())
				{
					// char map
					memcpy(cCharacterMap, com.Field("Character-Loc-Map").asString(), 10);

					// char account id
					char cTemp[11];
					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "%d", com.Field("Account-ID").asLong());
					strcpy(cAccountName, cTemp);

					// char level
					*iLevel = com.Field("Character-Level").asLong();

					// char guild
					strcpy(cGuildName, com.Field("Character-Guild-Name").asString());

					// char guild rank
					*cGuildRank = com.Field("Character-Guild-Rank").asLong();

					// char guild GUID
					*dwGuildGUID = com.Field("Character-Guild-GUID").asLong();
				}
			}
			com.Close();
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

			char cTemp2[256];
			ZeroMemory(cTemp2, sizeof(cTemp2));
			wsprintf(cTemp2, "(!!!) SQL SERVER ERROR (iGetCharacterData): %s", (const char*)x.ErrText());
			PutLogList(cTemp2);

		}
	}

}

void CWorldLog::VerifyGuildIntegrity(char* cGuildName, UINT32* dwGuildGUID)
{

	if (G_bDBMode == true)
	{
		SACommand com;

		try
		{
			com.setConnection(&con);
			com.setCommandText("SELECT [Guild-GUID] FROM Guilds WHERE [Guild-Name] = :1");
			com.Param(1).setAsString() = cGuildName;

			com.Execute();

			if (com.isResultSet())
			{
				
				while (com.FetchNext())
				{
					// guild GUID
					*dwGuildGUID = com.Field("Guild-GUID").asLong();
				}
			}
			com.Close();
			
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

			
		}
	}

}

bool CWorldLog::bCheckCharacterExists(char* cCharacterName)
{
	SACommand com;

	try
	{
		com.setConnection(&con);
		com.setCommandText("SELECT COUNT([Character-ID]) FROM Characters WHERE [Character-Name] = :1");
		com.Param(1).setAsString() = cCharacterName;

		com.Execute();

		if (com.isResultSet())
		{
			while (com.FetchNext())
			{
				if (com.Field(1).asLong() > 0)
				{
					com.Close();
					return true;
				}
				else
				{
					com.Close();
					return false;
				}
			}
		}
		else
		{
			com.Close();
			return false;
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

		return true;
	}
	return false;
}

bool CWorldLog::bCheckGuildExists(char* cGuildName)
{
	SACommand com;

	try
	{
		com.setConnection(&con);
		com.setCommandText("SELECT COUNT([Guild-ID]) FROM Guilds WHERE [Guild-Name] = :1");
		com.Param(1).setAsString() = cGuildName;

		com.Execute();

		if (com.isResultSet())
		{
			while (com.FetchNext())
			{
				if (com.Field(1).asLong() > 0)
				{
					com.Close();
					return true;
				}
				else
				{
					com.Close();
					return false;
				}
			}
		}
		else
		{
			com.Close();
			return false;
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

		return true;
	}
	return false;
}
void CWorldLog::PutPacketLogData(UINT32 dwMsgID, char* cData, UINT32 dwMsgSize)
{
	FILE* pFile;
	char DbgBuffer[15000];

	pFile = fopen("PacketLog.txt", "at");
	if (pFile == 0) return;

	if (dwMsgSize > 10000) dwMsgSize = 10000;
	int i = 0, j, a;
	while (i < (int)dwMsgSize) {

		wsprintf(DbgBuffer, "MSGID -> 0x%.8X\n", dwMsgID);
		fwrite(DbgBuffer, 1, strlen(DbgBuffer), pFile);
		memset(DbgBuffer, 0, sizeof(DbgBuffer));
		strcpy(DbgBuffer, "DATA -> ");
		for (j = i; j < i + 16 && j < (int)dwMsgSize; j++)
			wsprintf(&DbgBuffer[strlen(DbgBuffer)], "%.2X ", (unsigned char)cData[j]);

		for (a = strlen(DbgBuffer); a < 56; a += 3)
			strcat(DbgBuffer, "   ");

		strcat(DbgBuffer, "\t\t\t");
		for (j = i; j < i + 16 && j < (int)dwMsgSize; j++)
			DbgBuffer[strlen(DbgBuffer)] = isprint((unsigned char)cData[j]) ? cData[j] : '.';

		fwrite(DbgBuffer, 1, strlen(DbgBuffer), pFile);
		fwrite("\r\n", 1, 2, pFile);
		i = j;
	}
	fwrite("\r\n\r\n", 1, 4, pFile);
	fclose(pFile);
}
void CWorldLog::ServerList(bool Client)
{
	int i, j, maps;
	maps = 0;
	if (Client == true)
	{
		for (j = 0; j < DEF_MAXGAMESERVERSMAPS; j++) {
			for (i = 0; i < DEF_MAXGAMESERVERS; i++) {
				if (m_pGameList[i] != 0 && (m_cMapList[j] != 0)) {
					if (m_cMapList[j]->iIndex == i) maps++;
					wsprintf(G_cTxt, "Name(%s) IP(%s) Maps(%d)", m_pGameList[i]->m_cGameServerName, m_pGameList[i]->m_cGameServerAddress, maps);
					PutLogServ(G_cTxt);
				}
			}
		}
	}
	else {
		for (i = 0; i < DEF_MAXPLAYERACCOUNTS; i++) {
			if (m_pAccountList[i] != 0) {
				wsprintf(G_cTxt, "Name(%s)", m_pAccountList[i]->cAccountName);
				PutLogServ(G_cTxt);
			}
		}
	}
}
