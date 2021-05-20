#include "MainLog.h"
#include <stdlib.h>
#include <stdio.h>
#include <direct.h>


extern void		PutLogList(char * cMsg);
extern char		G_cTxt[500];
extern char		G_cData50000[50000];

bool CMainLog::bAccept(class XSocket * pXSock, bool bWorld)
{
 int i, x;
 int sTotalConnection;
 class XSocket * pTmpSock;
 bool bFlag;
 sTotalConnection = 0;

	bFlag = false;
	for (i = 1; i < DEF_MAXCLIENTSOCK; i++) { //loop 1
		if (m_pClientList[i] == NULL) { //if client null
			m_pClientList[i] = new class CClient(m_hWnd);
			pXSock->bAccept(m_pClientList[i]->m_pXSock, WM_ONCLIENTSOCKETEVENT + i); 
			m_pClientList[i]->m_pXSock->bInitBufferSize(DEF_MSGBUFFERSIZE);
			ZeroMemory(m_pClientList[i]->m_cIPaddress, sizeof(m_pClientList[i]->m_cIPaddress));
			m_pClientList[i]->m_pXSock->iGetPeerAddress(m_pClientList[i]->m_cIPaddress);
			
			//no overflowing sockets
			for (x = 1; x < DEF_MAXCLIENTSOCK; x++)
			if(m_pClientList[x] != NULL) {
			if(strcmp(m_pClientList[x]->m_cIPaddress, m_pClientList[i]->m_cIPaddress) == 0) sTotalConnection++;
			}

			if(sTotalConnection > 9) {
				delete m_pClientList[i];
				m_pClientList[i] = NULL;
				return false;
			}

			//wsprintf(G_cTxt,"Client Accepted(%d)", i);
			//PutLogList(G_cTxt);
			return true;
		} //end client == null
	} //end loop 1
	pTmpSock = new class XSocket(m_hWnd, DEF_SERVERSOCKETBLOCKLIMIT);
	pXSock->bAccept(pTmpSock, NULL); 
	delete pTmpSock;
	return false;
}

CMainLog::CMainLog(HWND hWnd)
{
int i;

	for (i = 0; i < DEF_MAXCLIENTSOCK; i++) m_pClientList[i] = NULL;
	for (i = 0; i < DEF_MSGQUEUESIZE; i++) m_pMsgQueue[i] = NULL;
	for (i = 0; i < DEF_MAXGAME; i++) m_pGameList[i] = NULL;
	for (i = 0; i < DEF_MAXMAPS; i++) m_pMapList[i] = NULL;
	for (i = 0; i < DEF_MAXACCOUNTS; i++) m_pAccountList[i] = NULL;
	for (i = 0; i < DEF_MAXCHARACTER; i++) m_pCharList[i] = NULL;

	m_hWnd = hWnd;
	ZeroMemory(m_cMainServerAddress, sizeof(m_cMainServerAddress));
	m_iQueueHead = 0;
	m_iQueueTail = 0;
	m_iMainServerInternalPort = 0;
	m_iValidAccounts = m_iTotalWorld = m_iTotalGame = 0;
}

CMainLog::~CMainLog(void)
{
	int i;
	for (i = 0; i < DEF_MAXCLIENTSOCK; i++)
		if (m_pClientList[i] != NULL) delete m_pClientList[i];
	for (i = 0; i < DEF_MSGQUEUESIZE; i++) 
		if(m_pMsgQueue[i] != NULL) delete m_pMsgQueue[i];
	for (i = 0; i < DEF_MAXGAME; i++)
		if(m_pGameList[i] != NULL) delete m_pGameList[i];
	for (i = 0; i < DEF_MAXMAPS; i++)
		if(m_pMapList[i] == NULL) delete m_pMapList[i];
	for (i = 0; i < DEF_MAXACCOUNTS; i++)
		if(m_pAccountList[i] == NULL) delete m_pAccountList[i];
	for (i = 0; i < DEF_MAXCHARACTER; i++)
		if(m_pCharList[i] == NULL) delete m_pCharList[i];
}

bool CMainLog::bInit()
{
	if (bReadServerConfigFile("HMLServer.cfg") == false) return false;
	
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
		return false;
	}

	PutLogList("(!) SQL SERVER CONNECTED: @HBGhost");
	return true;
}

void CMainLog::MsgProcess()
{
 char   * pData=NULL, *cp, cFrom, cKey, cTemp[50];
 DWORD    dwMsgSize, * dwpMsgID;
  WORD   * wpMsgType;
 int     i, iClientH;
 char cDump[1000];
 ZeroMemory(cDump, sizeof(cDump));
 ZeroMemory(cTemp, sizeof(cTemp));

	ZeroMemory(m_pMsgBuffer, DEF_MSGBUFFERSIZE+1);
	pData = (char *)m_pMsgBuffer;
	while (bGetMsgQuene(&cFrom, pData, &dwMsgSize, &iClientH, &cKey) == true) {
		dwpMsgID   = (DWORD *)(pData + DEF_INDEX4_MSGID);
		switch (*dwpMsgID) {
			
			//----------From Client-------------//
			case MSGID_REQUEST_LOGIN: //client request login
				RequestLogin(iClientH, pData);
			break;
			case MSGID_REQUEST_CREATENEWCHARACTER: //message from client
					ResponseCharacter(iClientH, pData, 0);
			break;
			case MSGID_REQUEST_DELETECHARACTER:
					DeleteCharacter(iClientH, pData, 0);
				break;
			case MSGID_REQUEST_CHANGEPASSWORD:
					ChangePassword(iClientH, pData);
				break;
			case MSGID_REQUEST_ENTERGAME:
					RequestEnterGame(iClientH, pData, 0);
				break;
			case MSGID_REQUEST_CREATENEWACCOUNT:
				CreateAccount(iClientH, pData);
				break;
			//---------From WLserver-------------//
			case MSGID_REGISTER_WORLDSERVERSOCKET: //last message for activation
				wpMsgType  = (WORD *)(pData + DEF_INDEX2_MSGTYPE);
				switch(*wpMsgType) {
					case DEF_MSGTYPE_CONFIRM:
						RegisterSocketWL(iClientH, pData, true);
					break;
					case DEF_MSGTYPE_REJECT:
						RegisterSocketWL(iClientH, pData, false);
					break;
				}
			break;
			case MSGID_WORLDSERVERACTIVATED:
				if(m_pClientList[iClientH] == NULL) return;
				for (i = 1; i < DEF_MAXCLIENTSOCK; i++)
					if((m_pClientList[i] != NULL) && (strcmp(m_pClientList[i]->m_cWorldName, m_pClientList[iClientH]->m_cWorldName) == 0))
				m_pClientList[i]->m_cMode = 1; //WLserver Activated
				m_iTotalWorld++;
				PutLogList(" ");
				wsprintf(cDump, "(!) WorldServer(%s) Activated", m_pClientList[iClientH]->m_cWorldName);
				PutLogList(cDump);
				break;

			case MSGID_REQUEST_REMOVEGAMESERVER: //remove game server
				wpMsgType  = (WORD *)(pData + DEF_INDEX2_MSGTYPE);
				switch(*wpMsgType) {
				case DEF_MSGTYPE_CONFIRM:
			    RegisterGameServer(iClientH, pData, false);
				break;
				}
				break;

			case MSGID_REGISTER_WORLDSERVER_GAMESERVER: //ad game server
				wpMsgType  = (WORD *)(pData + DEF_INDEX2_MSGTYPE);
				switch(*wpMsgType) {
					case DEF_MSGTYPE_CONFIRM:
					RegisterGameServer(iClientH, pData, true);
					break;
				}
				break;

			case MSGID_RESPONSE_CHARINFOLIST: //get all character info
					TotalChar(iClientH, pData);
				break;

			case MSGID_RESPONSE_CHARACTERLOG:
				wpMsgType  = (WORD *)(pData + DEF_INDEX2_MSGTYPE);
				switch(*wpMsgType) {
				case DEF_LOGRESMSGTYPE_NEWCHARACTERDELETED:
					DeleteCharacter(iClientH, pData, 1);
					break;
				case DEF_LOGRESMSGTYPE_NEWCHARACTERCREATED:
					ResponseCharacter(iClientH, pData, 1);
					break;
				case DEF_LOGRESMSGTYPE_NEWCHARACTERFAILED:
						cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

						memcpy(cTemp, cp, 10);
						cp += 10;

					for (i = 0; i < DEF_MAXACCOUNTS; i++)
						if((m_pAccountList[i] != NULL) && (strcmp(m_pAccountList[i]->cAccountName, cTemp) == 0)) {
							if(m_pClientList[m_pAccountList[i]->iClientH] != NULL)
							SendEventToWLS(DEF_LOGRESMSGTYPE_NEWCHARACTERFAILED, DEF_LOGRESMSGTYPE_NEWCHARACTERFAILED, 0, 0, m_pAccountList[i]->iClientH);
					}
					break;
				case DEF_LOGRESMSGTYPE_ALREADYEXISTINGCHARACTER:
						cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

						memcpy(cTemp, cp, 10);
						cp += 10;

					for (i = 0; i < DEF_MAXACCOUNTS; i++)
						if((m_pAccountList[i] != NULL) && (strcmp(m_pAccountList[i]->cAccountName, cTemp) == 0)) {
							if(m_pClientList[m_pAccountList[i]->iClientH] != NULL)
							SendEventToWLS(DEF_LOGRESMSGTYPE_ALREADYEXISTINGCHARACTER, DEF_LOGRESMSGTYPE_ALREADYEXISTINGCHARACTER, 0, 0, m_pAccountList[i]->iClientH);
					}
					break;
				}
				break;
			   case MSGID_REQUEST_CLEARACCOUNTSTATUS:
				   cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

						memcpy(cTemp, cp, 10);
						cp += 10;

					for (i = 0; i < DEF_MAXACCOUNTS; i++)
						if((m_pAccountList[i] != NULL) && (strcmp(m_pAccountList[i]->cAccountName, cTemp) == 0)) {
							wsprintf(G_cTxt, "(!)Clear Account (%s)", cTemp);
							PutLogList(G_cTxt);
							m_iValidAccounts--;
							delete m_pAccountList[i];
							m_pAccountList[i] = NULL;
						}
				break;
			
			default:

	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
	memcpy(m_pClientList[iClientH]->m_cWorldName, cp, 30);
	cp += 30;
	memcpy(m_pClientList[iClientH]->m_cWorldServerAddress, cp, 16);
	cp += 16;
	wsprintf(G_cTxt, "(OOO) World Server(%s:%s) registration success!", m_pClientList[iClientH]->m_cWorldName, m_pClientList[iClientH]->m_cWorldServerAddress);
	PutLogList(G_cTxt);


				break;


				break;
		}
		break;
	}
	UpdateScreen();
}

bool CMainLog::bGetMsgQuene(char * pFrom, char * pData, DWORD * pMsgSize, int * pIndex, char * pKey)
{

	if (m_pMsgQueue[m_iQueueHead] == NULL) return false;

	m_pMsgQueue[m_iQueueHead]->Get(pFrom, pData, pMsgSize, pIndex, pKey);

	delete m_pMsgQueue[m_iQueueHead];
	m_pMsgQueue[m_iQueueHead] = NULL;

	m_iQueueHead++;
	if (m_iQueueHead >= DEF_MSGQUEUESIZE) m_iQueueHead = 0;

	return true;
}
bool CMainLog::bPutMsgQuene(char cFrom, char * pData, DWORD dwMsgSize, int iIndex, char cKey)
{
	if (m_pMsgQueue[m_iQueueTail] != NULL) return false;

	m_pMsgQueue[m_iQueueTail] = new class CMsg;
	if (m_pMsgQueue[m_iQueueTail] == NULL) return false;

	if (m_pMsgQueue[m_iQueueTail]->bPut(cFrom, pData, dwMsgSize, iIndex, cKey) == false) return false;

	m_iQueueTail++;
	if (m_iQueueTail >= DEF_MSGQUEUESIZE) m_iQueueTail = 0;
	
	return true;
}
void CMainLog::OnTimer()
{
DWORD dwTime;
dwTime = timeGetTime();
MsgProcess();
}
//send message to WLserver
void CMainLog::SendEventToWLS(DWORD dwMsgID, WORD wMsgType, char * pData, DWORD dwMsgSize, int iWorldH)
{
 int iRet;
 DWORD * dwp;
 char * cp;
 WORD * wp;
 int m_iCurWorldLogSockIndex;

	if(iWorldH == -1) return;
	else m_iCurWorldLogSockIndex = iWorldH;

	ZeroMemory(G_cData50000, sizeof(G_cData50000));
	
	dwp  = (DWORD *)(G_cData50000 + DEF_INDEX4_MSGID);
	*dwp = dwMsgID;
	wp   = (WORD *)(G_cData50000 + DEF_INDEX2_MSGTYPE);
	*wp  = wMsgType;

	cp = (char *)(G_cData50000 + DEF_INDEX2_MSGTYPE + 2);

	memcpy((char *)cp, pData, dwMsgSize);

	if ((iWorldH != -1)) { //if WLserver registered
		switch(dwMsgID) {
			case MSGID_RESPONSE_REGISTER_WORLDSERVERSOCKET:
				iRet = m_pClientList[m_iCurWorldLogSockIndex]->m_pXSock->iSendMsg(G_cData50000, dwMsgSize+6, DEF_USE_ENCRYPTION);
				break;
			default:
				iRet = m_pClientList[m_iCurWorldLogSockIndex]->m_pXSock->iSendMsg(G_cData50000, dwMsgSize+6, DEF_USE_ENCRYPTION); //send anyway..
				break;
		}
		switch (iRet) {
		case DEF_XSOCKEVENT_QUENEFULL:
		case DEF_XSOCKEVENT_SOCKETERROR:
		case DEF_XSOCKEVENT_CRITICALERROR:
		case DEF_XSOCKEVENT_SOCKETCLOSED:
			delete m_pClientList[m_iCurWorldLogSockIndex];
			m_pClientList[m_iCurWorldLogSockIndex] = NULL;
			wsprintf(G_cTxt, "(!) Connection Lost (%d) - (%s)", m_iCurWorldLogSockIndex, m_pClientList[m_iCurWorldLogSockIndex]->m_cWorldName);
			PutLogList(G_cTxt);
			break;
		}
	}
}


void CMainLog::OnClientSubLogSocketEvent(UINT message, WPARAM wParam, LPARAM lParam) //From Wl or client
{
 int iWorldH, iRet, iTmp, i, iTotalSock;
 char  * pData, cKey;
 DWORD  dwMsgSize;
 iTotalSock = 0;

	iTmp = WM_ONCLIENTSOCKETEVENT;
	iWorldH = message - iTmp;
	if (m_pClientList[iWorldH] == NULL) return;
	iRet = m_pClientList[iWorldH]->m_pXSock->iOnSocketEvent(wParam, lParam);
	switch (iRet) {

	case DEF_XSOCKEVENT_CONNECTIONESTABLISH:

		break;

	case DEF_XSOCKEVENT_READCOMPLETE:
		pData = m_pClientList[iWorldH]->m_pXSock->pGetRcvDataPointer(&dwMsgSize, &cKey);
		if (bPutMsgQuene(DEF_MSGFROM_LOGSERVER, pData, dwMsgSize, iWorldH, cKey) == false) {
			PutLogList("@@@@@@ CRITICAL ERROR in LOGSERVER MsgQuene!!! @@@@@@");
		}
		break;

	case DEF_XSOCKEVENT_BLOCK:

		break;

	case DEF_XSOCKEVENT_CONFIRMCODENOTMATCH:

		break;

	case DEF_XSOCKEVENT_MSGSIZETOOLARGE:
	case DEF_XSOCKEVENT_SOCKETERROR:
	case DEF_XSOCKEVENT_SOCKETCLOSED:
	case DEF_XSOCKEVENT_QUENEFULL:
		//delete world socket
		if(m_pClientList[iWorldH]->m_iWorldPort != 0) {
		wsprintf(G_cTxt, "(X) World-Server-socket(%d) error!", iWorldH);
		PutLogList(G_cTxt);
		for (i = 1; i < DEF_MAXCLIENTSOCK; i++)
		if((m_pClientList[i] != NULL) &&(strcmp(m_pClientList[i]->m_cWorldName, m_pClientList[iWorldH]->m_cWorldName) == 0)) iTotalSock++;
		if(iTotalSock - 1 <= 0) {
		PutLogList(" ");
		wsprintf(G_cTxt, "(X) World-Server(%s) Deleted!", m_pClientList[iWorldH]->m_cWorldName);
		PutLogList(G_cTxt);
		m_iTotalWorld--;
		}
		}
		//delete an account
		if (m_pClientList[iWorldH]->m_cMode == 2 && m_pClientList[iWorldH]->m_cWorldName != NULL) {

			wsprintf(G_cTxt, "(onClientSubLogSocketEvent) Delete Account: %s", m_pClientList[iWorldH]->m_cWorldName);
			PutLogList(G_cTxt);

			DeleteAccount(-1, m_pClientList[iWorldH]->m_cWorldName); //delete account name
		}
		delete m_pClientList[iWorldH];
		m_pClientList[iWorldH] = NULL;
		break;
	}
}
void CMainLog::RegisterSocketWL(int iClientH, char *pData, bool bRegister)
{
char cData[100];
 DWORD * dwp;
 char *cp;
 	 ZeroMemory(cData, sizeof(cData));
	if(m_pClientList[iClientH] == NULL) return; //WL socket connected
	 if(bRegister == false) {
		delete m_pClientList[iClientH];
		m_pClientList[iClientH] = NULL;
		wsprintf(G_cTxt, "(!) Socket Register Failed (%d)", iClientH);
		PutLogList(G_cTxt);
		 return;
	 }
	
	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

	memcpy(m_pClientList[iClientH]->m_cWorldName, cp, 30);
	cp += 30;

	memcpy(m_pClientList[iClientH]->m_cWorldServerAddress, cp, 16);
	cp += 16;
	
	dwp = (DWORD *)cp;
	m_pClientList[iClientH]->m_iWorldPort = *dwp;
	cp += 4;

	wsprintf(G_cTxt, "(O) Client (%d) is socket to World Server(%s)", iClientH, m_pClientList[iClientH]->m_cWorldName);
	PutLogList(G_cTxt);

	SendEventToWLS(MSGID_RESPONSE_REGISTER_WORLDSERVERSOCKET, DEF_MSGTYPE_CONFIRM, cData, 0, iClientH);
}
void CMainLog::RegisterGameServer(int iClientH, char *pData, bool bRegister)
{
 char cData[100], m_cWorldName[31], m_cGameName[11], m_cGameAddress[16];
 DWORD * dwp;
 char *cp, *cMapNames;
 int m_iGamePort, m_iTotalMaps, i, x;
 bool bwlregister;
 bwlregister = false;
 ZeroMemory(cData, sizeof(cData));
 ZeroMemory(m_cWorldName, sizeof(m_cWorldName));
 ZeroMemory(m_cGameName, sizeof(m_cGameName));
 ZeroMemory(m_cGameAddress, sizeof(m_cGameAddress));

	if(m_pClientList[iClientH] == NULL) return;
	switch(bRegister) {
	case true:
 	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

	memcpy(m_cWorldName, cp, 30);
	cp += 30;

	memcpy(m_cGameName, cp, 10);
	cp += 10;

	memcpy(m_cGameAddress, cp, 16);
	cp += 16;

	dwp = (DWORD *)cp;
	m_iGamePort = *dwp;
	cp += 4;

	dwp = (DWORD *)cp;
	m_iTotalMaps = *dwp;
	cp += 4;

	for (i = 1; i < DEF_MAXCLIENTSOCK; i++)
	if((m_pClientList[i] != NULL) && (strcmp(m_pClientList[i]->m_cWorldName, m_cWorldName) == 0) && (m_pClientList[i]->m_cMode == 1)) bwlregister = true;
	if(bwlregister == true) {
	wsprintf(G_cTxt, "(OOO) %s-Game Server(%s:%s:%d) registration success", m_cWorldName, m_cGameName, m_cGameAddress, m_iGamePort);
	PutLogList(G_cTxt);
	for (i = 0; i < DEF_MAXGAME; i++)
		if (m_pGameList[i] == NULL) {
			cMapNames = (char *)cp;
			m_pGameList[i] = new class CGame(iClientH, m_cGameName, m_cGameAddress, m_iGamePort, m_cWorldName, m_iTotalMaps);
			for (x = 0; x < m_iTotalMaps; x++) {
				if (bClientRegisterMaps(i, cp) == 0) {

				}
				cp += 11;
			}
			m_iTotalGame++;
			return;
		} //if game server == nULL
	}// WLserver is registered
	break;
	case false:
	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

	memcpy(m_cWorldName, cp, 30);
	cp += 30;

	memcpy(m_cGameName, cp, 10);
	cp += 10;
	
	for (i = 0; i < DEF_MAXGAME; i++)
		if ((m_pGameList[i] != NULL)&&(strcmp(m_pGameList[i]->m_cWorldName, m_cWorldName) == 0)&&(strcmp(m_pGameList[i]->m_cGameName, m_cGameName) == 0)) {
		PutLogList(" ");
		wsprintf(G_cTxt, "Delete Game Server(%s) in World Server(%s)", m_pGameList[i]->m_cGameName, m_cWorldName);
		PutLogList(G_cTxt);
		//delete maps on game server
		for (x = 0; x < DEF_MAXMAPS; x++)
		if((m_pMapList[x] != NULL) && (m_pMapList[x]->iIndex == i)) {
		wsprintf(G_cTxt, "Delete Map(%s) in Game Server(%s)", m_pMapList[x]->m_cMapName, m_pGameList[i]->m_cGameName);
		PutLogList(G_cTxt);
		delete m_pMapList[x];
		m_pMapList[x] = NULL;
		}
		delete m_pGameList[i];
		m_pGameList[i] = NULL;
		m_iTotalGame--;
		}
	break;
	}
}
bool CMainLog::bClientRegisterMaps(int iClientH, char *pData)
{
 int i;

	for (i = 0; i < DEF_MAXMAPS; i++) {
		if (m_pMapList[i] != NULL) {
			if (memcmp(m_pMapList[i]->m_cMapName, pData, 10) == 0) {
				wsprintf(G_cTxt, "(X) CRITICAL ERROR! Map(%s) duplicated!", pData);
				PutLogList(G_cTxt);
				return false;
			}
		}
	}
	for (i = 0; i < DEF_MAXMAPS; i++) {
		if (m_pMapList[i] == NULL) {
			m_pMapList[i] = new class CMap(iClientH, pData);
			wsprintf(G_cTxt, "(!) Map(%s) registration: WS1-%s MapNum(%d)", pData, m_pGameList[iClientH]->m_cGameName, i);
			PutLogList(G_cTxt);
			return true;			
		}
	}
	wsprintf(G_cTxt, "(X) No more map space left. Map(%s) cannot be added.", pData);
	PutLogList(G_cTxt);
	return false;
}
void CMainLog::RequestLogin(int iClientH, char *pData)
{
 int  iMessage, iAccount, i, iDBID = -1;
 char cData[200], cMesg, cTotalChar;
 DWORD *dwp;
 SYSTEMTIME SysTime;
 char *cp, *cp2, cAccountName[11], cAccountPass[11], cWorldName[30];
 ZeroMemory(cAccountName, sizeof(cAccountName));
 ZeroMemory(cAccountPass, sizeof(cAccountPass));
 ZeroMemory(cWorldName, sizeof(cWorldName));
 ZeroMemory(cData, sizeof(cData));
 cMesg = 0;
 cTotalChar = 0;
 iAccount = -1;

		if(m_pClientList[iClientH] == NULL) return;

	   cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

		memcpy(cAccountName, cp, 10);
		cp += 10;
		memcpy(cAccountPass, cp, 10);
		cp += 10;
		memcpy(cWorldName, cp, 30);
		cp += 30;

		iMessage = GetAccountInfo(iClientH, cAccountName, cAccountPass, cWorldName, &iAccount, &iDBID);
		switch(iMessage){
		case 0: //0 errors check password and send success message
			if(m_pAccountList[iAccount] == NULL) cMesg = 1;
			else if(strcmp(m_pAccountList[iAccount]->cAccountName, cAccountName) != 0) cMesg = 1;
			else if(strcmp(m_pAccountList[iAccount]->cWorldName, cWorldName) != 0) cMesg = 2;
			else if(strcmp(m_pAccountList[iAccount]->cPassword, cAccountPass) != 0) cMesg = 3;
		break;
		case 1:
			SendEventToWLS(DEF_LOGRESMSGTYPE_NOTEXISTINGACCOUNT, DEF_LOGRESMSGTYPE_NOTEXISTINGACCOUNT, cData, 0, iClientH);
			return;
			break;
		case 2:
			SendEventToWLS(DEF_LOGRESMSGTYPE_PASSWORDMISMATCH, DEF_LOGRESMSGTYPE_PASSWORDMISMATCH, cData, 0, iClientH);
			return;
			break;
		case 3:
			SendEventToWLS(DEF_LOGRESMSGTYPE_NOTEXISTINGWORLDSERVER, DEF_LOGRESMSGTYPE_NOTEXISTINGWORLDSERVER, cData, 0, iClientH);
			return;
			break;
		}
		switch(cMesg) {
		case 1:
		SendEventToWLS(DEF_LOGRESMSGTYPE_NOTEXISTINGACCOUNT, DEF_LOGRESMSGTYPE_NOTEXISTINGACCOUNT, cData, 0, iClientH);
		break;
		case 2:
		SendEventToWLS(DEF_LOGRESMSGTYPE_NOTEXISTINGWORLDSERVER, DEF_LOGRESMSGTYPE_NOTEXISTINGWORLDSERVER, cData, 0, iClientH);
		break;
		case 3:
		SendEventToWLS(DEF_LOGRESMSGTYPE_PASSWORDMISMATCH, DEF_LOGRESMSGTYPE_PASSWORDMISMATCH, cData, 0, iClientH);
		break;
		case 0: //successful log on
		GetLocalTime(&SysTime);
		m_pAccountList[iAccount]->dAccountID = iDBID;//(int)(SysTime.wYear + SysTime.wMonth + SysTime.wDay + SysTime.wHour + SysTime.wMinute + timeGetTime());

		if (!(m_pAccountList[iAccount]->iClientH == iClientH))
		{
			DeleteAccount(iAccount);
			return;
		}

		int iTotal;

		iTotal = 0;

		for (i = 0; i < DEF_MAXCHARACTER; i++)
		{
			if ((m_pCharList[i] != NULL) && (m_pCharList[i]->iTracker == iDBID))
			{
				cTotalChar++;
				iTotal++;
			}

			if (iTotal >= 4) break;
		}

		SendCharacterData(iAccount, iDBID, cTotalChar, iClientH, cAccountName);

		/*for (i = 0; i < DEF_MAXCHARACTER; i++)
			if((m_pCharList[i] != NULL) && (m_pCharList[i]->iTracker == iAccount)) cTotalChar++;

		wsprintf(G_cTxt, "TestTotal(%d)", cTotalChar);
		PutLogList(G_cTxt);

		cp2 = (char *)(cData);
		memcpy(cp2, cAccountName, 10);
		cp2 += 10;

		dwp = (DWORD *)cp2;
		*dwp = m_pAccountList[iAccount]->dAccountID;
		cp2 += 4;

		cp2 += 17;

		*cp2 = cTotalChar;
		cp2 ++;

		dwp = (DWORD *)cp2; ///valid time
		*dwp = 11758870; // is number of valid time
		cp2 += 4;

		dwp = (DWORD *)cp2; //ip
		*dwp = 11758874; // number to check valid time....
		cp2 += 4;

		for (i = 0; i < DEF_MAXCHARACTER; i++)
			if((m_pCharList[i] != NULL) && (m_pCharList[i]->iTracker == iAccount)) {
			memcpy(cp2, m_pCharList[i]->cCharacterName, 10);
			cp2 += 10;
		}

		for (i = 0; i < DEF_MAXCLIENTSOCK; i++)
			if((m_pClientList[i] != NULL) && (strcmp(m_pClientList[i]->m_cWorldName, cWorldName) == 0)&& (m_pClientList[i]->m_cMode == 1)) {
			SendEventToWLS(MSGID_REQUEST_CHARINFOLIST, DEF_MSGTYPE_CONFIRM, cData, 49+(cTotalChar*10), i);
			break;
			}*/
		break;
		}
}

int CMainLog::iGetAccountDatabaseID(char* cAccName)
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

void CMainLog::SendCharacterData(int iAccount, int iAccountID, char cTotalChar, int iClientH, char* cAccountName)
{
	char* cp, cData[2000];
	DWORD* dwp;
	WORD* wp;
	short iUperVersion, iLowerVersion;
	int iSpace1, iSpace2, i, iRows;
	bool bConnected = false;

	ZeroMemory(cData, sizeof(cData));
	iUperVersion = DEF_UPERVERSION;
	iLowerVersion = DEF_LOWERVERSION;
	iSpace1 = 11758870;
	iSpace2 = 11758874;

	cp = (char*)(cData); //outgoing messag

	dwp = (DWORD*)cp;
	*dwp = iUperVersion;
	cp += 2;
//2
	dwp = (DWORD*)cp;
	*dwp = iLowerVersion;
	cp += 2;
//4
	//used for account status
	cp++;
//5
	dwp = (DWORD*)cp;
	*dwp = m_pAccountList[iAccount]->m_iAccntYear;
	cp += 2;
//7
	dwp = (DWORD*)cp;
	*dwp = m_pAccountList[iAccount]->m_iAccntMonth;
	cp += 2;
//9
	dwp = (DWORD*)cp;
	*dwp = m_pAccountList[iAccount]->m_iAccntDay;
	cp += 2;
//11
	dwp = (DWORD*)cp;
	*dwp = m_pAccountList[iAccount]->m_iPassYear;
	cp += 2;
//13
	dwp = (DWORD*)cp;
	*dwp = m_pAccountList[iAccount]->m_iPassMonth;
	cp += 2;
//15
	dwp = (DWORD*)cp;
	*dwp = m_pAccountList[iAccount]->m_iPassDay;
	cp += 2;
//17
	if (m_pAccountList[iAccount] != NULL) {

		if (m_pClientList[iClientH] == NULL) {
			DeleteAccount(iAccount);
			return;
		}

		if (strcmp(m_pClientList[iClientH]->m_cWorldName, m_pAccountList[iAccount]->cAccountName) != 0) {
			DeleteAccount(iAccount);
			return;
		}
	}


	*cp = cTotalChar;
	cp++;
//18
	int accDBID = iGetAccountDatabaseID(cAccountName);
	iRows = 0;

	SACommand com;


	try
	{
		com.setConnection(&con);
		com.setCommandText("SELECT     [Character-ID], [Account-ID], RTRIM([Character-Name]) AS 'Character-Name', [Character-Profile], [Character-Location], [Character-Guild-Name], [Character-Guild-GUID], [Character-Guild-Rank], [Character-Loc-Map], [Character-Loc-X], [Character-Loc-Y], [Character-HP], [Character-MP], [Character-SP], [Character-Level], [Character-Rating], [Character-Strength], [Character-Intelligence], [Character-Vitality], [Character-Dexterity], [Character-Magic], [Character-Charisma], [Character-Luck], [Character-Experience], [Character-LU-Pool], [Character-Ek-Count], [Character-Pk-Count], [Character-Reward-Gold], [Character-Down-Skill-Index], [Character-ID1], [Character-ID2], [Character-ID3], [Character-Sex], [Character-Skin], [Character-Hair-Style], [Character-Hair-Colour], [Character-Underwear], [Character-Hunger], [Character-Shutup-Time], [Character-Rating-Time], [Character-Force-Time], [Character-SP-Time], [Character-Admin-Level], [Character-Block-Date], [Character-Quest-Number], [Character-Quest-ID], [Character-Quest-Reward], [Character-Quest-Amount], [Character-Contribution], [Character-War-Contribution], [Character-Event-ID], [Character-Criticals], [Character-Fightzone-ID], [Character-Ability-Time], [Character-Lock-Map], [Character-Lock-Time], [Character-Crusade-Job], [Character-Crusade-GUID], [Character-Construct-Points], [Character-Death-Time], [Character-Party-ID], [Character-Majestics], [Character-Appr1], [Character-Appr2], [Character-Appr3], [Character-Appr4], [Character-Appr-Colour] FROM         Characters WHERE     ([Account-ID] = :1)");
		com.Param(1).setAsLong() = accDBID;

		com.Execute();

		if (com.isResultSet())
		{

			char cTest[256];
			char cTemp[120];

			while (com.FetchNext())
			{
				// char map
				ZeroMemory(cTemp, sizeof(cTemp));
				strcpy(cTemp, com.Field("Character-Name").asString());
				memcpy(cp, cTemp, 10);
				cp += 10;

				*cp = 1;
				cp++;

				// db character ID
				/*ZeroMemory(cTemp, sizeof(cTemp));
				strcpy(cTemp, com.Field("Character-ID").asString());
				iCharacterDBid = atoi(cTemp);*/

				// char appr1
				ZeroMemory(cTemp, sizeof(cTemp));
				strcpy(cTemp, com.Field("Character-Appr1").asString());
				wp = (WORD*)cp;
				*wp = atoi(cTemp);
				cp += 2;

				// char appr2
				ZeroMemory(cTemp, sizeof(cTemp));
				strcpy(cTemp, com.Field("Character-Appr2").asString());
				wp = (WORD*)cp;
				*wp = atoi(cTemp);
				cp += 2;

				// char appr3
				ZeroMemory(cTemp, sizeof(cTemp));
				strcpy(cTemp, com.Field("Character-Appr3").asString());
				wp = (WORD*)cp;
				*wp = atoi(cTemp);
				cp += 2;

				// char appr1
				ZeroMemory(cTemp, sizeof(cTemp));
				strcpy(cTemp, com.Field("Character-Appr4").asString());
				wp = (WORD*)cp;
				*wp = atoi(cTemp);
				cp += 2;

				// char gender
				ZeroMemory(cTemp, sizeof(cTemp));
				strcpy(cTemp, com.Field("Character-Sex").asString());
				wp = (WORD*)cp;
				*wp = atoi(cTemp);
				cp += 2;

				// char skin
				ZeroMemory(cTemp, sizeof(cTemp));
				strcpy(cTemp, com.Field("Character-Skin").asString());
				wp = (WORD*)cp;
				*wp = atoi(cTemp);
				cp += 2;

				// char level
				ZeroMemory(cTemp, sizeof(cTemp));
				strcpy(cTemp, com.Field("Character-Level").asString());
				wp = (WORD*)cp;
				*wp = atoi(cTemp);
				cp += 2;

				// char exp
				ZeroMemory(cTemp, sizeof(cTemp));
				strcpy(cTemp, com.Field("Character-Experience").asString());
				dwp = (DWORD*)cp;
				*dwp = atoi(cTemp);
				cp += 4;

				// char str
				ZeroMemory(cTemp, sizeof(cTemp));
				strcpy(cTemp, com.Field("Character-Strength").asString());
				wp = (WORD*)cp;
				*wp = atoi(cTemp);
				cp += 2;

				// char vit
				ZeroMemory(cTemp, sizeof(cTemp));
				strcpy(cTemp, com.Field("Character-Vitality").asString());
				wp = (WORD*)cp;
				*wp = atoi(cTemp);
				cp += 2;

				// char dex
				ZeroMemory(cTemp, sizeof(cTemp));
				strcpy(cTemp, com.Field("Character-Dexterity").asString());
				wp = (WORD*)cp;
				*wp = atoi(cTemp);
				cp += 2;

				// char int
				ZeroMemory(cTemp, sizeof(cTemp));
				strcpy(cTemp, com.Field("Character-Intelligence").asString());
				wp = (WORD*)cp;
				*wp = atoi(cTemp);
				cp += 2;

				// char mag
				ZeroMemory(cTemp, sizeof(cTemp));
				strcpy(cTemp, com.Field("Character-Magic").asString());
				wp = (WORD*)cp;
				*wp = atoi(cTemp);
				cp += 2;

				// char charisma
				ZeroMemory(cTemp, sizeof(cTemp));
				strcpy(cTemp, com.Field("Character-Charisma").asString());
				wp = (WORD*)cp;
				*wp = atoi(cTemp);
				cp += 2;

				// char appr colour
				ZeroMemory(cTemp, sizeof(cTemp));
				strcpy(cTemp, com.Field("Character-Appr-Colour").asString());
				dwp = (DWORD*)cp;
				*dwp = atoi(cTemp);
				cp += 4;

				// char save year
				wp = (WORD*)cp;
				*wp = 0;
				cp += 2;

				// char save month
				wp = (WORD*)cp;
				*wp = 0;
				cp += 2;

				// char save day
				wp = (WORD*)cp;
				*wp = 0;
				cp += 2;

				// char save hour
				wp = (WORD*)cp;
				*wp = 0;
				cp += 2;

				// char save minute
				wp = (WORD*)cp;
				*wp = 0;
				cp += 2;

				// char map
				ZeroMemory(cTemp, sizeof(cTemp));
				strcpy(cTemp, com.Field("Character-Loc-Map").asString());
				memcpy(cp, cTemp, 10);
				cp += 10;

				iRows++;
			}
		}
		else
		{
			com.Close();
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
		wsprintf(cTemp, "(!!!) SQL SERVER ERROR (SendCharacterData): %s", (const char*)x.ErrText());
		PutLogList(cTemp);
	}

	/*if (iRows != cTotalChar)
	{
		//testlog
		char cTest[256];
		wsprintf(cTest, "Account: (%s), Rows Affected: (%i), Total Char: (%i)", cAccountName, iRows, cTotalChar);
		PutLogList(cTest);
		DeleteAccount(iAccount);
		return;
	}*/

	dwp = (DWORD*)cp;
	*dwp = (DWORD)iSpace1;
	cp += 4;

	dwp = (DWORD*)cp;
	*dwp = (DWORD)iSpace2;
	cp += 4;

	for (i = 0; i < DEF_MAXCLIENTSOCK; i++)
		if ((m_pClientList[i] != NULL) && (strcmp(m_pClientList[i]->m_cWorldName, m_pAccountList[iAccount]->cAccountName) == 0)) {
			if ((i == iClientH) && (strcmp(m_pClientList[iClientH]->m_cWorldName, m_pAccountList[iAccount]->cAccountName) == 0))
			{
				if (m_pAccountList[iAccount]->iClientH == iClientH)
				{
					SendEventToWLS(DEF_LOGRESMSGTYPE_CONFIRM, DEF_LOGRESMSGTYPE_CONFIRM, cData, 26 + (65 * cTotalChar), i);
					bConnected = true;
					break;
				}
			}
		}

	if (bConnected == false)
		DeleteAccount(iAccount);
}

void CMainLog::TotalChar(int iClientH, char *pData)
{
 char *cp, *cp2, cAccountName[11], cWorldName[11], cData[2000], cTotalChar; //cp2 out going message
 DWORD *dwp, dAccountid, dMsg, dMsgType;
 int i, iAccount, iTracker, iAccountDBID;
 short iUperVersion, iLowerVersion;
 ZeroMemory(cAccountName, sizeof(cAccountName));
 ZeroMemory(cWorldName, sizeof(cWorldName)); // vamp - to check socket jump
 ZeroMemory(cData, sizeof(cData));
 iUperVersion = DEF_UPERVERSION;
 iLowerVersion = DEF_LOWERVERSION;
 cTotalChar = 0;

		if(m_pClientList[iClientH] == NULL) return;

	   cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2); //incomeing message

	   memcpy(cAccountName, cp, 10);
	   cp += 10;

	   dwp = (DWORD *)cp;
	   dAccountid = *dwp;
	   cp += 4;

		dwp = (DWORD *)cp;
		dMsg = *dwp;
		cp += 4;

		dwp = (DWORD *)cp;
		dMsgType = *dwp;
		cp += 2;

		switch(dMsg) {
			
		case MSGID_RESPONSE_CHARACTERLOG:
			switch(dMsgType) {
			case DEF_MSGTYPE_CONFIRM:
				for (i = 0; i < DEF_MAXACCOUNTS; i++)
					  if((m_pAccountList[i] != NULL) && (strcmp(m_pAccountList[i]->cAccountName, cAccountName) == 0)) {
					   iTracker = m_pAccountList[i]->iClientH;
					   iAccountDBID = m_pAccountList[i]->dAccountID;

					   iAccount= i;
					   m_pAccountList[i]->iClientH = NULL; //set iclient to null becouse this is only to be sure he didnt dc on logon
					   if(m_pAccountList[i]->dAccountID != dAccountid) {
						   DeleteAccount(i);
						   return;
					   }
					   if(m_pClientList[iTracker] == NULL) {
						   DeleteAccount(i);
						   return;
					 }
					   memcpy(cWorldName, m_pClientList[iTracker]->m_cWorldName, 10);
					   if (strcmp(cWorldName, cAccountName) != 0) {
						   
						   DeleteAccount(i);
						   return;
					   }
					   break;
				 }
				for (i = 0; i < DEF_MAXCHARACTER; i++)
					if((m_pCharList[i] != NULL) && (m_pCharList[i]->iTracker == iAccountDBID)) cTotalChar++; // iAccount

				if (iAccount < 0) return;

				cp2 = (char *)(cData); //outgoing messag

				dwp = (DWORD *)cp2;
				*dwp = iUperVersion;
				cp2 += 2;

				dwp = (DWORD *)cp2;
				*dwp = iLowerVersion;
				cp2 += 2;

				//used for account status
				cp2++;

				dwp = (DWORD *)cp2;
				*dwp = m_pAccountList[iAccount]->m_iAccntYear;
				cp2 += 2;

				dwp = (DWORD *)cp2;
				*dwp = m_pAccountList[iAccount]->m_iAccntMonth;
				cp2 += 2;

				dwp = (DWORD *)cp2;
				*dwp = m_pAccountList[iAccount]->m_iAccntDay;
				cp2 += 2;

				dwp = (DWORD *)cp2;
				*dwp = m_pAccountList[iAccount]->m_iPassYear;
				cp2 += 2;

				dwp = (DWORD *)cp2;
				*dwp = m_pAccountList[iAccount]->m_iPassMonth;
				cp2 += 2;

				dwp = (DWORD *)cp2;
				*dwp = m_pAccountList[iAccount]->m_iPassDay;
				cp2 += 2;

			     *cp2 = cTotalChar;
			     cp2++;

			 	//cp2  = (char *)(cData + 20);
				cp = (char *)(pData + 44); //incomeing
				memcpy(cp2, cp, 8+(65*cTotalChar)); // get message between total*
				cp2 += 8+(65*cTotalChar);

				SendEventToWLS(DEF_LOGRESMSGTYPE_CONFIRM, DEF_LOGRESMSGTYPE_CONFIRM, cData, 26+(65*cTotalChar), iTracker);
			break;
			}
			break;
		}
}
void CMainLog::ResponseCharacter(int iClientH, char *pData, char cMode)
{
 char *cp, *cp2, cAccountName[11], cAccountPass[11], cWorldName[30], cData[2000], cTotalChar; //cp2 out going message
 DWORD *dwp, dwAccountid;
 int i, iMessage, iAccount, iTracker, iDBID = -1;
 SYSTEMTIME SysTime;
  ZeroMemory(cAccountName, sizeof(cAccountName));
   ZeroMemory(cAccountPass, sizeof(cAccountPass));
    ZeroMemory(cWorldName, sizeof(cWorldName));
	ZeroMemory(cData, sizeof(cData));
	cTotalChar = 0;
	iTracker = -1;
	iAccount = -1;

 	if(m_pClientList[iClientH] == NULL) return;

	switch(cMode) {
		case 0: //message from client to WL
		 cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2 + 10); //incomeing message
		
		 memcpy(cAccountName, cp, 10);
		 cp += 10;
		 memcpy(cAccountPass, cp, 10);
		 cp += 10;
		 memcpy(cWorldName, cp, 30);
		 cp += 30;

		iMessage = GetAccountInfo(iClientH, cAccountName, cAccountPass, cWorldName, &iAccount, &iDBID);

		if(iMessage > 0) {
		SendEventToWLS(DEF_LOGRESMSGTYPE_NEWCHARACTERFAILED, DEF_LOGRESMSGTYPE_NEWCHARACTERFAILED, 0, 0, iClientH);
		return;
		}
		if(strcmp(m_pAccountList[iAccount]->cPassword, cAccountPass) != 0) {
		SendEventToWLS(DEF_LOGRESMSGTYPE_NEWCHARACTERFAILED, DEF_LOGRESMSGTYPE_NEWCHARACTERFAILED, 0, 0, iClientH);
		return;
		}

		for (i = 0; i < DEF_MAXCHARACTER; i++)
		if((m_pCharList[i] != NULL) && (m_pCharList[i]->iTracker == iDBID)) cTotalChar++; // iAccount

		GetLocalTime(&SysTime);
		m_pAccountList[iAccount]->dAccountID = iDBID; //(int)(SysTime.wYear + SysTime.wMonth + SysTime.wDay + SysTime.wHour + SysTime.wMinute + timeGetTime());

		cp2 = (char *)(cData); //outgoing messag
		cp = (char *)(pData + 6); //incomeing
		memcpy(cp2, cp, 71); // get message between total*
		cp2 += 71;

		dwp = (DWORD *)cp2;
		*dwp = m_pAccountList[iAccount]->dAccountID;
	    cp2 += 4;

	    *cp2 = cTotalChar;
	    cp2++;

		for (i = 0; i < DEF_MAXCHARACTER; i++)
			if((m_pCharList[i] != NULL) && (m_pCharList[i]->iTracker == iDBID)) { // iAccount
			memcpy(cp2, m_pCharList[i]->cCharacterName, 11);
			cp2 += 11;
		}

		for (i = 0; i < DEF_MAXCLIENTSOCK; i++)
			if((m_pClientList[i] != NULL) && (strcmp(m_pClientList[i]->m_cWorldName, cWorldName) == 0)&& (m_pClientList[i]->m_cMode == 1)) {
			SendEventToWLS(MSGID_REQUEST_CREATENEWCHARACTER, DEF_MSGTYPE_CONFIRM, cData, 76+(cTotalChar * 11), i);
			break;
		}
		break;
		case 1: //message from WL to Client
			cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2); //incomeing message
			
			memcpy(cAccountName, cp, 10);
			cp += 10;

		    dwp = (DWORD *)cp;
			dwAccountid = *dwp;
			cp += 4;

			for (i = 0; i < DEF_MAXACCOUNTS; i++)
				if((m_pAccountList[i] != NULL) && (strcmp(m_pAccountList[i]->cAccountName, cAccountName) == 0)) {
				  iTracker = m_pAccountList[i]->iClientH;
				  iAccount= i;
				  m_pAccountList[i]->iClientH = NULL; //set iclient to null becouse this is only to be sure he didnt dc on logon
				if(m_pAccountList[i]->dAccountID != dwAccountid) {
					SendEventToWLS(DEF_LOGRESMSGTYPE_NEWCHARACTERFAILED, DEF_LOGRESMSGTYPE_NEWCHARACTERFAILED, 0, 0, iTracker);
					DeleteAccount(i);
					return;
				}
				if(m_pClientList[iTracker] == NULL) {
					DeleteAccount(i);
					return;
				}
				}

			cp = (char *)(pData + 26); //incomeing
			memcpy(cAccountPass, cp, 10); //new char name
			cp += 10;
			cTotalChar = (char)*cp;

			cp = (char *)(pData + 26); //incomeing
			cp2 = (char *)(cData); //outgoing messag
			memcpy(cp2, cp, 11+(cTotalChar*65)); // get message between total*
			cp2 += 11+(cTotalChar*65);

			if(SaveAccountInfo(iAccount, NULL, cAccountPass, NULL, 1, iTracker) == false) {

			}

			SendEventToWLS(DEF_LOGRESMSGTYPE_NEWCHARACTERCREATED, DEF_LOGRESMSGTYPE_NEWCHARACTERCREATED, cData, 11+(cTotalChar*65), iTracker);
			break;
	}
}
void CMainLog::DeleteCharacter(int iClientH, char* pData, char cMode)
{
	char* cp, * cp2, cAccountName[11], cAccountPass[11], cWorldName[30], cData[500], cTotalChar, cCharName[11]; //cp2 out going message
	DWORD* dwp, dwAccountid;
	int i, iMessage, iAccount, iTracker, iDBID;
	SYSTEMTIME SysTime;
	ZeroMemory(cAccountName, sizeof(cAccountName));
	ZeroMemory(cAccountPass, sizeof(cAccountPass));
	ZeroMemory(cWorldName, sizeof(cWorldName));
	ZeroMemory(cCharName, sizeof(cCharName));
	ZeroMemory(cData, sizeof(cData));
	iAccount = -1;
	iTracker = -1;
	iDBID = -1;
	cTotalChar = 0;

	if (m_pClientList[iClientH] == NULL) return;

	switch (cMode) {
	case 0:
		cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2); //incomeing message

		memcpy(cCharName, cp, 10);
		cp += 10;
		memcpy(cAccountName, cp, 10);
		cp += 10;
		memcpy(cAccountPass, cp, 10);
		cp += 10;
		memcpy(cWorldName, cp, 30);
		cp += 30;

		iMessage = GetAccountInfo(iClientH, cAccountName, cAccountPass, cWorldName, &iAccount, &iDBID);

		if (iMessage > 0) {
			SendEventToWLS(DEF_LOGRESMSGTYPE_NOTEXISTINGACCOUNT, DEF_LOGRESMSGTYPE_NOTEXISTINGACCOUNT, 0, 0, iClientH);
			return;
		}
		if (strcmp(m_pAccountList[iAccount]->cPassword, cAccountPass) != 0) {
			SendEventToWLS(DEF_LOGRESMSGTYPE_PASSWORDMISMATCH, DEF_LOGRESMSGTYPE_PASSWORDMISMATCH, 0, 0, iClientH);
			return;
		}
		//delete charcter in account file
		/*if(SaveAccountInfo(iAccount, NULL, m_pAccountList[iAccount]->cWorldName, cCharName, 3, iClientH) == false) {
			SendEventToWLS(DEF_LOGRESMSGTYPE_NOTEXISTINGCHARACTER, DEF_LOGRESMSGTYPE_NOTEXISTINGCHARACTER, 0, 0, iClientH);
			return;
		}*/
		for (i = 0; i < DEF_MAXCHARACTER; i++)
			if ((m_pCharList[i] != NULL) && (m_pCharList[i]->iTracker == iDBID)) {
				if (memcmp(m_pCharList[i]->cCharacterName, cCharName, strlen(cCharName)) == 0) {
					delete m_pCharList[i];
					m_pCharList[i] = NULL;
				}
			}
		for (i = 0; i < DEF_MAXCHARACTER; i++)
			if ((m_pCharList[i] != NULL) && (m_pCharList[i]->iTracker == iDBID))	cTotalChar++;

		GetLocalTime(&SysTime);
		m_pAccountList[iAccount]->dAccountID = iDBID;//(int)(SysTime.wYear + SysTime.wMonth + SysTime.wDay + SysTime.wHour + SysTime.wMinute + timeGetTime()); 

		cp2 = (char*)(cData); //outgoing messag

		memcpy(cp2, cAccountName, 10);
		cp2 += 10;

		dwp = (DWORD*)cp2;
		*dwp = m_pAccountList[iAccount]->dAccountID;
		cp2 += 4;

		memcpy(cp2, cCharName, 10);
		cp2 += 10;

		*cp2 = cTotalChar;
		cp2++;

		for (i = 0; i < DEF_MAXCHARACTER; i++)
			if ((m_pCharList[i] != NULL) && (m_pCharList[i]->iTracker == iDBID)) {
				memcpy(cp2, m_pCharList[i]->cCharacterName, 11);
				cp2 += 11;
			}

		for (i = 0; i < DEF_MAXCLIENTSOCK; i++)
			if ((m_pClientList[i] != NULL) && (strcmp(m_pClientList[i]->m_cWorldName, cWorldName) == 0) && (m_pClientList[i]->m_cMode == 1)) {
				SendEventToWLS(MSGID_REQUEST_DELETECHARACTER, DEF_MSGTYPE_CONFIRM, cData, 25 + (cTotalChar * 11), i);
				break;
			}
		break;
	case 1:
		cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2); //incomeing message

		memcpy(cAccountName, cp, 10);
		cp += 10;

		dwp = (DWORD*)cp;
		dwAccountid = *dwp;
		cp += 4;

		for (i = 0; i < DEF_MAXACCOUNTS; i++)
			if ((m_pAccountList[i] != NULL) && (strcmp(m_pAccountList[i]->cAccountName, cAccountName) == 0)) {
				iTracker = m_pAccountList[i]->iClientH;
				iAccount = i;
				m_pAccountList[i]->iClientH = NULL; //set iclient to null becouse this is only to be sure he didnt dc on logon
				if (m_pAccountList[i]->dAccountID != dwAccountid) {
					SendEventToWLS(DEF_LOGRESMSGTYPE_NOTEXISTINGCHARACTER, DEF_LOGRESMSGTYPE_NOTEXISTINGCHARACTER, 0, 0, iTracker);

					wsprintf(G_cTxt, "(DeleteCharacter) Delete Account: %i", i);
					PutLogList(G_cTxt);
					DeleteAccount(i);
					return;
				}
				if (m_pClientList[iTracker] == NULL) {
					wsprintf(G_cTxt, "(DeleteCharacter) Delete Account: %i", i);
					PutLogList(G_cTxt);
					DeleteAccount(i);
					return;
				}
			}
		cp = (char*)(pData + 27); //incomeing
		cTotalChar = *cp;

		cp = (char*)(pData + 26); //incomeing
		cp2 = (char*)(cData); //outgoing messag
		memcpy(cp2, cp, 2 + (cTotalChar * 65)); // get message between total*
		cp2 += 2 + (cTotalChar * 65);

		SendEventToWLS(DEF_LOGRESMSGTYPE_CHARACTERDELETED, DEF_LOGRESMSGTYPE_CHARACTERDELETED, cData, 2 + (cTotalChar * 65), iTracker);

		break;
	}
}
/*
void CMainLog::ChangePassword(int iClientH, char *pData)
{
char *cp, cAccountName[11], cAccountPass[11], cNewPass[11], cCheckNewPass[11];
int iAccount, iMessage;
  ZeroMemory(cAccountName, sizeof(cAccountName));
   ZeroMemory(cAccountPass, sizeof(cAccountPass));
    ZeroMemory(cNewPass, sizeof(cNewPass));
	ZeroMemory(cCheckNewPass, sizeof(cCheckNewPass));
	iAccount = -1;

	if(m_pClientList[iClientH] == NULL) return;


		cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2); //incomeing message
		
		memcpy(cAccountName, cp, 10);
		cp += 10;
		memcpy(cAccountPass, cp, 10);
		cp += 10;
		memcpy(cNewPass, cp, 10);
		cp += 10;
		memcpy(cCheckNewPass, cp, 10);
		cp += 10;

		iMessage = GetAccountInfo(iClientH, cAccountName, cAccountPass, NULL, &iAccount, 1);

		if((iMessage > 0) || (iAccount <= -1)) {
		SendEventToWLS(DEF_LOGRESMSGTYPE_NOTEXISTINGACCOUNT, DEF_LOGRESMSGTYPE_NOTEXISTINGACCOUNT, 0, 0, iClientH);
		return;
		}
		if(strcmp(m_pAccountList[iAccount]->cPassword, cAccountPass) != 0) {
		SendEventToWLS(DEF_LOGRESMSGTYPE_PASSWORDMISMATCH, DEF_LOGRESMSGTYPE_PASSWORDMISMATCH, 0, 0, iClientH);
		return;
		}
		if(strcmp(cNewPass, cCheckNewPass) != 0) {
		SendEventToWLS(DEF_LOGRESMSGTYPE_PASSWORDCHANGEFAIL, DEF_LOGRESMSGTYPE_PASSWORDCHANGEFAIL, 0, 0, iClientH);
		return;
		}
		//change password in account file
		if(SaveAccountInfo(iAccount, NULL, cNewPass, NULL, 2, iClientH) == false) {
			SendEventToWLS(DEF_LOGRESMSGTYPE_NOTEXISTINGACCOUNT, DEF_LOGRESMSGTYPE_NOTEXISTINGACCOUNT, 0, 0, iClientH);
			return;
		}
		else {
			SendEventToWLS(DEF_LOGRESMSGTYPE_PASSWORDCHANGESUCCESS, DEF_LOGRESMSGTYPE_PASSWORDCHANGESUCCESS, 0, 0, iClientH);
			return;
		}

}
*/

void CMainLog::ChangePassword(int iClientH, char* pData)
{
	char   cTemp[100], cTemp2[100];
	FILE* pFile;
	HANDLE hFile;
	int    iSize;
	char   cBuffer[2000];
	char   cBuffer2[2000];
	char   cTxt[500], cTxt2[500], cTmp[100];
	int    iTest = -1;
	fpos_t pos;
	char* cp, cAccNewPass[11], cAccName[11], cAccPass[11], cWorldServerName[31];
	WORD* wp;
	DWORD* dwp;
	DWORD  dwSize = 0;
	memset(cBuffer, 0, 2000);
	memset(cBuffer2, 0, 2000);
	memset(cTxt, 0, 500);
	memset(cTxt2, 0, 500);
	memset(cTmp, 0, 100);

	ZeroMemory(cTemp, 100);
	ZeroMemory(cTemp2, 100);

	ZeroMemory(cAccNewPass, sizeof(cAccNewPass));
	ZeroMemory(cAccName, sizeof(cAccName));
	ZeroMemory(cAccPass, sizeof(cAccPass));

	cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);

	memcpy(cAccName, cp, 10);
	cp += 10;

	memcpy(cAccPass, cp, 10);
	cp += 10;

	memcpy(cAccNewPass, cp, 10);
	cp += 10;

	if (memcmp(cAccNewPass, cp, 10))
	{
		SendEventToWLS(DEF_LOGRESMSGTYPE_PASSWORDCHANGEFAIL, DEF_LOGRESMSGTYPE_PASSWORDCHANGEFAIL, 0, 0, iClientH);
		return;
	}

	/*   if(CheckAccountLogin(pData, pData+10, iClientH, "NONE") == DEF_LOGRESMSGTYPE_PASSWORDMISMATCH, 0, 0, iClientH)
	   {
		   SendEventToWLS(DEF_LOGRESMSGTYPE_PASSWORDCHANGEFAIL, DEF_LOGRESMSGTYPE_PASSWORDCHANGEFAIL, 0, 0, iClientH);
		   return;
	   }*/

	SACommand com, com2;

	try
	{
		com2.setConnection(&con);
		com2.setCommandText("UPDATE Accounts SET [Account-Password] = :3 WHERE ([Account-Name] = :1 AND [Account-Password] = :2)");
		com2.Param(1).setAsString() = cAccName;
		com2.Param(2).setAsString() = cAccPass;
		com2.Param(3).setAsString() = cAccNewPass;

		com2.Execute();

		if (com2.RowsAffected() > 0)
			SendEventToWLS(DEF_LOGRESMSGTYPE_PASSWORDCHANGESUCCESS, DEF_LOGRESMSGTYPE_PASSWORDCHANGESUCCESS, 0, 0, iClientH);
		else
			SendEventToWLS(DEF_LOGRESMSGTYPE_PASSWORDCHANGEFAIL, DEF_LOGRESMSGTYPE_PASSWORDCHANGEFAIL, 0, 0, iClientH);
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
		wsprintf(cTemp, "(!!!) SQL SERVER ERROR (ChangePassword): %s", (const char*)x.ErrText());
		PutLogList(cTemp);

		SendEventToWLS(DEF_LOGRESMSGTYPE_PASSWORDCHANGEFAIL, DEF_LOGRESMSGTYPE_PASSWORDCHANGEFAIL, 0, 0, iClientH);
		return;
	}
}
void CMainLog::RequestEnterGame(int iClientH, char *pData, char cMode)
{
 char *cp, *cp2, cData[100], cData2[100], cAccountName[11], cAccountPass[11], cWorldName[30], m_cMapName[31], cCharName[15], G_cCmdLineTokenA[120];
 int  i, x, m_iLevel, iMessage, iAccount, iGameServer, iDBID = -1, iInUse;
 DWORD *dwp;
 bool bMapOnline, bAccountUse;
 bMapOnline = false;
 bAccountUse = false;
 ZeroMemory(cAccountName, sizeof(cAccountName));
 ZeroMemory(cAccountPass, sizeof(cAccountPass));
 ZeroMemory(cWorldName, sizeof(cWorldName));
 ZeroMemory(m_cMapName, sizeof(m_cMapName));
 ZeroMemory(cData, sizeof(cData));
 ZeroMemory(cData2, sizeof(cData2));
 ZeroMemory(G_cCmdLineTokenA, sizeof(G_cCmdLineTokenA));


		wsprintf(G_cTxt, "(!)RequestEnterGame");
		PutLogList(G_cTxt);

		if(m_pClientList[iClientH] == NULL) return;

		switch(cMode) {
		
		case 0:
	    cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

		memcpy(cCharName, cp, 10);
		cp += 10;

		memcpy(m_cMapName, cp, 10);
		cp += 10;

		memcpy(cAccountName, cp, 10);
		cp += 10;

		memcpy(cAccountPass, cp, 10);
		cp += 10;

		dwp = (DWORD *)cp;
		m_iLevel = *dwp;
		cp += 4;

        memcpy(cWorldName, cp, 30);
		cp += 30;

		memcpy(cp, G_cCmdLineTokenA, 120);
		cp += 120;
		
		for (i = 0; i < DEF_MAXACCOUNTS; i++)
			if ((m_pAccountList[i] != NULL) && (strcmp(m_pAccountList[i]->cAccountName, cAccountName) == 0))
			{
				bAccountUse = true;
				iInUse = i;
			}
			if (bAccountUse == true) {
				//cp2 = (char *)(cData); //outgoing messag - to WLserv
				//*cp2 = 5;
				//PutLogList("Account In Use");
				//SendEventToWLS(DEF_ENTERGAMERESTYPE_REJECT, DEF_ENTERGAMERESTYPE_REJECT, cData, 1, iClientH);
				DeleteAccount(iInUse);
				//return;
			}

		//worldserver Decline messages
		//1 - Char Trial level is made
		//2 - ip groop expired
		//3 - cant connect to game server down..
		//4 - Character data differs
		//6 - max users on server
		//5 - game server offline
		//7 - world server full
		iMessage = GetAccountInfo(iClientH, cAccountName, cAccountPass, cWorldName, &iAccount, &iDBID);
		if(iMessage > 0) {
		cp2 = (char *)(cData); //outgoing messag - to WLserv
		*cp2 = 4;
		SendEventToWLS(DEF_ENTERGAMERESTYPE_REJECT, DEF_ENTERGAMERESTYPE_REJECT, cData, 1, iClientH);
		return;
		}
		m_pAccountList[iAccount]->dAccountID = iDBID;
		if(strcmp(m_pAccountList[iAccount]->cPassword, cAccountPass) != 0) {
		cp2 = (char *)(cData); //outgoing messag - to WLserver
		*cp2 = 4;
		SendEventToWLS(DEF_ENTERGAMERESTYPE_REJECT, DEF_ENTERGAMERESTYPE_REJECT, cData, 1, iClientH);
		return;
		}
		for (i = 0; i < DEF_MAXGAME; i++)
			if((m_pGameList[i] != NULL)&& (strcmp(m_pGameList[i]->m_cWorldName, cWorldName) == 0)) {
			for (x = 0; x < DEF_MAXMAPS; x++)
				if((m_pMapList[x] != NULL)&&(m_pMapList[x]->iIndex == i)&& (strcmp(m_pMapList[x]->m_cMapName, m_cMapName) == 0)) {
					iGameServer = i;
					bMapOnline = true;
				}
			}
			if(bMapOnline == false){
			cp2 = (char *)(cData); //outgoing messag - to WLserver
			*cp2 = 3;
			SendEventToWLS(DEF_ENTERGAMERESTYPE_REJECT, DEF_ENTERGAMERESTYPE_REJECT, cData, 1, iClientH);
			return;
			}

		cp2 = (char *)(cData); //outgoing messag - to WLserver

		memcpy(cp2, cAccountName, 10);
		cp2 += 10;

		memcpy(cp2, cAccountPass, 10);
		cp2 += 10;

		dwp = (DWORD *)cp2;
		*dwp = m_iLevel;
		cp2 += 4;

		if (m_pAccountList[iAccount] == NULL) return;
		m_pAccountList[iAccount]->dAccountID = iDBID;
		m_iValidAccounts++;
		for (i = 0; i < DEF_MAXCLIENTSOCK; i++)
			if((m_pClientList[i] != NULL) && (strcmp(m_pClientList[i]->m_cWorldName, cWorldName) == 0)&& (m_pClientList[i]->m_cMode == 1)) {
			SendEventToWLS(MSGID_REQUEST_SETACCOUNTINITSTATUS, DEF_MSGTYPE_CONFIRM, cData, 24, i);
			break;
		}
		cp2 = (char *)(cData2); //outgoing messag - to Client
		
		memcpy(cp2, m_pGameList[iGameServer]->m_cGameServerAddress, 16);
		cp2 += 16;
		
		dwp = (DWORD *)cp2;
		*dwp = m_pGameList[iGameServer]->m_iGamePort;
		cp2 += 2;

		memcpy(cp2, m_pGameList[iGameServer]->m_cGameName, 20);
		cp2 += 20;

		m_pClientList[iClientH]->m_cMode = 3; //set client to playing
		SendEventToWLS(DEF_ENTERGAMERESTYPE_CONFIRM, DEF_ENTERGAMERESTYPE_CONFIRM, cData2, 38, iClientH);
		break;

		case 1:

		break;
		}
}
bool CMainLog::SaveAccountInfo(int iAccount, char cAccountName[11], char cTemp[11], char cCharName[11], char cMode, int iClientH)
{
	char cFileName[255], cDir[63], cTxt[50], cAccount[11] , cTxt2[2000], cData[2000];
	int iLine, i;
	int    iSize;
	short iMinus;
	int    iCharPos = -1;
    int    iTest = -1;
	bool bDeleteLine;
	HANDLE hFile;
	fpos_t pos;
	DWORD  dwSize = 0;
	DWORD dwFileSize;
	FILE * pFile;

	if(m_pClientList[iClientH] == NULL) return false;

	memset(cData, 0, 2000);
    memset(cTxt2, 0, 2000);
	dwFileSize = 0;
	iLine = 0;
	bDeleteLine = false;
	iMinus = 0;
	ZeroMemory(cAccount, sizeof(cAccount));
	ZeroMemory(cTxt, sizeof(cTxt));
	ZeroMemory(cTxt2, sizeof(cTxt2));
	ZeroMemory(cData, sizeof(cData));
	if((cAccountName != NULL) && (iAccount == -1)) {
	for (i = 0; i < DEF_MAXACCOUNTS; i++)
		if((m_pAccountList[i] != NULL)&& (strcmp(m_pAccountList[i]->cAccountName, cAccountName) == 0)) {
		strcpy(cAccount, cAccountName);
		iAccount = i;
		break;
		}
	}
	else if(iAccount != -1) strcpy(cAccount, m_pAccountList[iAccount]->cAccountName); 
	if((iAccount == -1)) return false;

		ZeroMemory(cFileName, sizeof(cFileName));
		ZeroMemory(cDir, sizeof(cDir));
		strcat(cFileName, "Account");
		strcat(cFileName, "\\");
 		strcat(cFileName, "\\");
		wsprintf(cDir, "AscII%d", *cAccount);
		strcat(cFileName, cDir);
		strcat(cFileName, "\\");
		strcat(cFileName, "\\");
		strcat(cFileName, cAccount);	
		strcat(cFileName, ".txt");

		hFile = CreateFileA(cFileName, GENERIC_READ, NULL, NULL, OPEN_EXISTING, NULL, NULL);
		iSize = GetFileSize(hFile, NULL);
		if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);

	switch(cMode) {
	case 1: //save new character
			pFile = fopen(cFileName, "at");
			if (pFile == NULL) {
				wsprintf(G_cTxt, "(X) Account none exist : Name(%s)", cAccount);
				PutLogList(G_cTxt);
				if (pFile != NULL) fclose(pFile);
				return false;
			}
			wsprintf(cTxt, "\naccount-character  = %s", cTemp);
			fwrite(cTxt, 1, strlen(cTxt), pFile);
			fclose(pFile);
			return true;
		break;

	case 2: //password change
		ZeroMemory(cTemp, sizeof(cTemp));
		wsprintf(G_cTxt, "(X) PasswordChange(%s)", cTemp);
		PutLogList(G_cTxt);
		wsprintf(cTxt, "account-password = %s", cTemp);
		pFile = fopen(cFileName, "rt");
			if (pFile == NULL) {
					SendEventToWLS(DEF_LOGRESMSGTYPE_NOTEXISTINGACCOUNT, DEF_LOGRESMSGTYPE_NOTEXISTINGACCOUNT, 0, 0, iClientH);
				if (pFile != NULL) fclose(pFile);
				return false;
			}
		fgetpos(pFile, &pos);
        iSize = fread(cData, 1, iSize, pFile);
		fread(cData, dwFileSize, 1, pFile);
		 for (i = 0; i < iSize; i++)
        {
            if (memcmp((char*)cData+i, "[CHARACTERS]", 12) == 0)
            {
                iCharPos = i;
            }
            if (memcmp((char*)cData+i, "account-password = ", 19) == 0)
            {
                iTest = i;
            }
        }
		if (iTest == -1)
        {
			SendEventToWLS(DEF_LOGRESMSGTYPE_NOTEXISTINGACCOUNT, DEF_LOGRESMSGTYPE_NOTEXISTINGACCOUNT, 0, 0, iClientH);
            fclose(pFile);
            return false;
        }
        else
        {
            memcpy(cTxt2, cData, iTest+19);
			memcpy(cTxt2+iTest+19, cTxt, strlen(cTxt));
            memcpy(cTxt2+iTest+19+strlen(cTxt), cData+iTest+19+strlen(m_pAccountList[iAccount]->cPassword), iSize-19-iTest-strlen(m_pAccountList[iAccount]->cPassword));
			SaveInfo(cFileName, cTxt2, 1);
        }
		fclose(pFile);
		return true;
		break;

	case 3: //delete character
		ZeroMemory(cTemp, sizeof(cTemp));
		wsprintf(G_cTxt, "(X) Character Delete(%s)", cCharName);
		PutLogList(G_cTxt);
		if(strcmp(cTemp, "WS1")) wsprintf(cTxt, "account-character  = %s", cCharName);
		else wsprintf(cTxt, "account-character-%s  = %s", cTemp, cCharName);
		pFile = fopen(cFileName, "rt");
			if (pFile == NULL) {
					SendEventToWLS(DEF_LOGRESMSGTYPE_NOTEXISTINGACCOUNT, DEF_LOGRESMSGTYPE_NOTEXISTINGACCOUNT, 0, 0, iClientH);
				if (pFile != NULL) fclose(pFile);
				return false;
			}
		fgetpos(pFile, &pos);
        iSize = fread(cData, 1, iSize, pFile);
		fread(cData, dwFileSize, 1, pFile);
		 for (int i = 0; i < iSize; i++)
        {
            if (memcmp((char*)cData+i, "[CHARACTERS]", 12) == 0)
            {
                iCharPos = i;
            }
            if (memcmp((char*)cData+i, cTxt, strlen(cTxt)) == 0)
            {
                iTest = i;
            }
        }
		if (iTest == -1)
        {
			SendEventToWLS(DEF_LOGRESMSGTYPE_NOTEXISTINGCHARACTER, DEF_LOGRESMSGTYPE_NOTEXISTINGCHARACTER, 0, 0, iClientH);
            fclose(pFile);
            return false;
        }
        else
        {
            memcpy(cTxt2, cData, iTest-1);
            memcpy(cTxt2+iTest-1, cData+iTest+strlen(cTxt), iSize-strlen(cTxt)-iTest);
			SaveInfo(cFileName, cTxt2, 1);
        }
		fclose(pFile);
		return true;
		break;
	}
	return false;
}
void CMainLog::SaveInfo(char cFileName[255], char *pData, DWORD dwStartSize)
{
	//DWORD dwFileSize;
	FILE * pFile;

		//hFile = CreateFileA(cFileName, GENERIC_READ, NULL, NULL, OPEN_EXISTING, NULL, NULL);
		//dwFileSize = GetFileSize(hFile, NULL);
		//if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);
		pFile = fopen(cFileName, "wt");
		if(pFile != NULL)
		if(sizeof(pData) > 0) fwrite(pData, dwStartSize, strlen(pData), pFile);
		if (pFile != NULL) fclose(pFile);
}
/*
int CMainLog::GetAccountInfo(int iClientH, char cAccountName[11], char cAccountPass[11], char cWorldName[30], int * iAccount, char cMode)
{
	char cFileName[255], cDir[63], cTxt[50];
	char *cp, *token, cReadModeA, cReadModeB;
	char seps[] = "= \t\n";
	class CStrTok * pStrTok;
	int i, iAccountid;
	HANDLE hFile;
	DWORD dwFileSize;
	FILE * pFile;

	bool bWorld;
	bWorld = false;
	cReadModeA = cReadModeB = 0;
	iAccountid = 0;

	ZeroMemory(cTxt, sizeof(cTxt));
	if((cMode == NULL)) wsprintf(cTxt, "account-character-%s", cWorldName);

	if (strlen(cAccountName) == 0) return 1; // if account blank
	if (strlen(cAccountPass) == 0) return 2; // if password blank
	if ((cMode == NULL) && (strlen(cWorldName) == 0)) return 3; // if world dosnt exist

	if(cMode == NULL) {
	for (i = 0; i < DEF_MAXCLIENTSOCK; i++)
		if((m_pClientList[i] != NULL) && (strcmp(m_pClientList[i]->m_cWorldName, cWorldName) == 0) && (m_pClientList[i]->m_cMode == 1)) bWorld = true; // World exist
	if(bWorld == false) return 3;
	}

		ZeroMemory(cFileName, sizeof(cFileName));
		ZeroMemory(cDir, sizeof(cDir));
		strcat(cFileName, "Account");
		strcat(cFileName, "\\");
 		strcat(cFileName, "\\");
		wsprintf(cDir, "AscII%d", *cAccountName);
		strcat(cFileName, cDir);
		strcat(cFileName, "\\");
		strcat(cFileName, "\\");
		strcat(cFileName, cAccountName);	
		strcat(cFileName, ".txt");	

		hFile = CreateFileA(cFileName, GENERIC_READ, NULL, NULL, OPEN_EXISTING, NULL, NULL);
		dwFileSize = GetFileSize(hFile, NULL);
		if(hFile != INVALID_HANDLE_VALUE)  CloseHandle(hFile);
		pFile = fopen(cFileName, "rt");
		if (pFile == NULL) {
			wsprintf(G_cTxt, "(!)Account Dose not Exist (%s)", cFileName);
			PutLogList(G_cTxt);
			return 1;
		}
		cp = new char[dwFileSize+1];
		ZeroMemory(cp, dwFileSize+1);
		fread(cp, dwFileSize, 1, pFile);
		pStrTok = new class CStrTok(cp, seps);
		token = pStrTok->pGet();
			while (token != NULL) {
				if (cReadModeA != 0) {
					switch (cReadModeA) {
						case 1:
							if(strlen(token) <= 0) {
								delete pStrTok;
								delete cp;
								return 1;
							}
							for (i = 0; i < DEF_MAXACCOUNTS; i++)
								if(m_pAccountList[i] == NULL) {
								m_pAccountList[i] = new class CAccount(token, cWorldName, iClientH);
								m_pClientList[iClientH]->m_cMode = 2;
								strcpy(m_pClientList[iClientH]->m_cWorldName, token); // save account name
								iAccountid = i;
								*iAccount = i;
								cReadModeA = 0;
								break;
								}
							cReadModeA = 0;
							break;
						case 2:
							if(strlen(token) <= 0) {
								delete pStrTok;
								delete cp;
								delete m_pAccountList[iAccountid];
								m_pAccountList[iAccountid] = NULL;
								return 1;
							}
							strcpy(m_pAccountList[iAccountid]->cPassword, token);
							cReadModeA = 0;
							break;
						case 3:
							if(strlen(token) <= 0) {
								delete pStrTok;
								delete cp;
								delete m_pAccountList[iAccountid];
								m_pAccountList[iAccountid] = NULL;
								return 1;
							}
							m_pAccountList[iAccountid]->iAccountValid = atoi(token);
							cReadModeA = 0;
							break;
						case 4:
							switch(cReadModeB) {
								case 1:
									if(strlen(token) <= 0) {
										delete pStrTok;
										delete cp;
										delete m_pAccountList[iAccountid];
										m_pAccountList[iAccountid] = NULL;
										return 1;
									}
								m_pAccountList[iAccountid]->m_iAccntYear = atoi(token);
								cReadModeB = 2;
								break;
								case 2:
								if(strlen(token) <= 0) {
										delete pStrTok;
										delete cp;
										delete m_pAccountList[iAccountid];
										m_pAccountList[iAccountid] = NULL;
										return 1;
									}
								m_pAccountList[iAccountid]->m_iAccntMonth = atoi(token);
								cReadModeB = 3;
								break;
								case 3:
								if(strlen(token) <= 0) {
										delete pStrTok;
										delete cp;
										delete m_pAccountList[iAccountid];
										m_pAccountList[iAccountid] = NULL;
										return 1;
									}
								m_pAccountList[iAccountid]->m_iAccntDay = atoi(token);
								cReadModeB = 0;
								cReadModeA = 0;
								break;
							}
							break;
						case 5:
							switch(cReadModeB) {
								case 1:
									if(strlen(token) <= 0) {
										delete pStrTok;
										delete cp;
										delete m_pAccountList[iAccountid];
										m_pAccountList[iAccountid] = NULL;
										return 1;
									}
								m_pAccountList[iAccountid]->m_iPassYear = atoi(token);
								cReadModeB = 2;
								break;
								case 2:
								if(strlen(token) <= 0) {
										delete pStrTok;
										delete cp;
										delete m_pAccountList[iAccountid];
										m_pAccountList[iAccountid] = NULL;
										return 1;
									}
								m_pAccountList[iAccountid]->m_iPassMonth = atoi(token);
								cReadModeB = 3;
								break;
								case 3:
								if(strlen(token) <= 0) {
										delete pStrTok;
										delete cp;
										delete m_pAccountList[iAccountid];
										m_pAccountList[iAccountid] = NULL;
										return 1;
									}
								m_pAccountList[iAccountid]->m_iPassDay = atoi(token);
								cReadModeB = 0;
								cReadModeA = 0;
								break;
							}
							break;
						case 6:
							for (i = 0; i < DEF_MAXCHARACTER; i++)
								if(m_pCharList[i] == NULL) {
								m_pCharList[i] = new class CCharacter(token, iAccountid);
								cReadModeA = 0;
								break;
								}
							cReadModeA = 0;
							break;
					}
				}
				else {
					if (memcmp(token, "account-name", 12) == 0)			cReadModeA = 1;
					if (memcmp(token, "account-password", 16) == 0)			cReadModeA = 2;
					if (memcmp(token, "account-valid-time", 18) == 0)			cReadModeA = 3;
					if (memcmp(token, "account-valid-date", 18) == 0) {
						cReadModeB = 1;
						cReadModeA = 4;
					}
					if (memcmp(token, "account-change-password", 23) == 0) {
						cReadModeB = 1;
						cReadModeA = 5;
					}
					if((cMode == NULL) && (strcmp(cWorldName, "WS1") == 0))
					if (memcmp(token, "account-character", 17) == 0)			cReadModeA = 6;
					else if ((cMode == NULL) && (memcmp(token, cTxt, strlen(cTxt)) == 0))			cReadModeA = 6;
				}
				token = pStrTok->pGet();
			}
		delete pStrTok;
		delete cp;
		fclose(pFile);
	return 0;
}
*/

int CMainLog::GetAccountInfo(int iClientH, char cAccountName[11], char cAccountPass[11], char cWorldName[30], int* iAccount, int* iDBID, char cMode)
{
	char cFileName[255], cDir[63], cTxt[50];
	char* cp, * token, cReadModeA, cReadModeB;
	char seps[] = "= \t\n";
	class CStrTok* pStrTok;
	int i, iAccountid, iAccountDBid;
	HANDLE hFile;
	DWORD dwFileSize;
	FILE* pFile;

	bool bWorld;
	bWorld = false;
	cReadModeA = cReadModeB = 0;
	iAccountid = 0;

	ZeroMemory(cTxt, sizeof(cTxt));
	if ((cMode == NULL)) wsprintf(cTxt, "account-character-%s", cWorldName);

	if (strlen(cAccountName) == 0) return 1; // if account blank
	if (strlen(cAccountPass) == 0) return 2; // if password blank
	if ((cMode == NULL) && (strlen(cWorldName) == 0)) return 3; // if world dosnt exist

	if (m_pClientList[iClientH] == NULL) return 3;

	if (cMode == NULL) {
		for (i = 0; i < DEF_MAXCLIENTSOCK; i++)
			if ((m_pClientList[i] != NULL) && (strcmp(m_pClientList[i]->m_cWorldName, cWorldName) == 0) && (m_pClientList[i]->m_cMode == 1)) bWorld = true; // World exist
		if (bWorld == false) return 3;
	}

	SACommand com;
	SACommand com2;

	//bool exists = bCheckAccountExists(cAccountName);

	try
	{
		//if(exists)
		//{
		com.setConnection(&con);
		com.setCommandText("SELECT     [Account-ID], RTRIM([Account-Name]) AS 'Account-Name', RTRIM([Account-Password]) AS 'Account-Password', [Account-Valid-Time], [Account-Valid-Year], [Account-Valid-Month], [Account-Valid-Day], [Account-Change-Password], [Account-Gender], [Account-Age], [Account-Birth-Year], [Account-Birth-Month], [Account-Birth-Day], [Account-Real-Name], [Account-SSN], [Account-Email], [Account-Quiz], [Account-Answer], [Account-Created-By], [Account-Created-Date], [Account-Locked] FROM         Accounts WHERE     ([Account-Name] = :1) AND ([Account-Password] = :2)");
		com.Param(1).setAsString() = cAccountName;
		com.Param(2).setAsString() = cAccountPass;

		com.Execute();

		if (com.RowsAffected() == 0) return 2;

		if (com.isResultSet())
		{

			char cTest[256];
			char cTemp[120];

			while (com.FetchNext())
			{
				// db account ID
				ZeroMemory(cTemp, sizeof(cTemp));
				strcpy(cTemp, com.Field("Account-ID").asString());
				iAccountDBid = atoi(cTemp);

				*iDBID = iAccountDBid;

				// find characters associated with this account
				com2.setConnection(&con);
				com2.setCommandText("SELECT RTRIM([Character-Name]) AS 'Character-Name' FROM Characters WHERE [Account-ID] = :1");
				com2.Param(1).setAsLong() = iAccountDBid;

				// db account ID
				ZeroMemory(cTemp, sizeof(cTemp));
				strcpy(cTemp, com.Field("Account-ID").asString());
				iAccountDBid = atoi(cTemp);

				// account name
				ZeroMemory(cTemp, sizeof(cTemp));
				strcpy(cTemp, com.Field("Account-Name").asString());

				for (i = 0; i < DEF_MAXACCOUNTS; i++)
					if (m_pAccountList[i] == NULL) {
						m_pAccountList[i] = new class CAccount(cTemp, cWorldName, iClientH);
						m_pClientList[iClientH]->m_cMode = 2;
						strcpy(m_pClientList[iClientH]->m_cWorldName, cTemp); // save account name
						iAccountid = i;
						*iAccount = i;
						break;
					}

				if (m_pClientList[iClientH] == NULL)
				{
					DeleteAccount(iAccountid);
					return 3;
				}

				//wsprintf(cTest, "Testing (%s) with (%s)", cTemp, cAccountName);
				//PutLogList(cTest);
				if (strcmp(cTemp, cAccountName) != 0)
				{
					DeleteAccount(iAccountid);
					return 1;
				}

				m_pAccountList[iAccountid]->dAccountID = iAccountDBid;

				// account password
				ZeroMemory(cTemp, sizeof(cTemp));
				strcpy(cTemp, com.Field("Account-Password").asString());
				memcpy(m_pAccountList[iAccountid]->cPassword, cTemp, 10);

				// account valid time
				ZeroMemory(cTemp, sizeof(cTemp));
				strcpy(cTemp, com.Field("Account-Valid-Time").asString());
				m_pAccountList[iAccountid]->iAccountValid = atoi(cTemp);

				// account valid year
				ZeroMemory(cTemp, sizeof(cTemp));
				strcpy(cTemp, com.Field("Account-Valid-Year").asString());
				m_pAccountList[iAccountid]->m_iAccntYear = atoi(cTemp);

				// account valid month
				ZeroMemory(cTemp, sizeof(cTemp));
				strcpy(cTemp, com.Field("Account-Valid-Month").asString());
				m_pAccountList[iAccountid]->m_iAccntMonth = atoi(cTemp);

				// account valid day
				ZeroMemory(cTemp, sizeof(cTemp));
				strcpy(cTemp, com.Field("Account-Valid-Day").asString());
				m_pAccountList[iAccountid]->m_iAccntDay = atoi(cTemp);

				// password change year
				ZeroMemory(cTemp, sizeof(cTemp));
				strcpy(cTemp, "0");//com.Field("Account-Change-Password").asString());
				m_pAccountList[iAccountid]->m_iPassYear = atoi(cTemp);

				// password change month
				ZeroMemory(cTemp, sizeof(cTemp));
				strcpy(cTemp, "0");//com.Field("Account-Change-Password").asString());
				m_pAccountList[iAccountid]->m_iPassMonth = atoi(cTemp);

				// account valid day
				ZeroMemory(cTemp, sizeof(cTemp));
				strcpy(cTemp, "0");//com.Field("Account-Change-Password").asString());
				m_pAccountList[iAccountid]->m_iPassDay = atoi(cTemp);

				// account locked status
				/*ZeroMemory(cTemp, sizeof(cTemp));
				strcpy(cTemp, com.Field("Account-Locked").asString());
				m_pAccountList[iAccountid]->iLocked = atoi(cTemp);*/

				com2.Execute();

				if (com2.isResultSet())
				{
					while (com2.FetchNext())
					{
						ZeroMemory(cTemp, sizeof(cTemp));
						strcpy(cTemp, com2.Field("Character-Name").asString());

						int j;

						for (j = 0; j < DEF_MAXCHARACTER; j++)
						{
							if (m_pCharList[j] == NULL) {
								m_pCharList[j] = new class CCharacter(cTemp, iAccountDBid);
								break;
							}
						}
					}
					com2.Close();
				}
			}
			com.Close();
			//}
			//else
			//{
			//	return 2;
			//}
		}
		else
		{
			return 1;
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
		wsprintf(cTemp, "(!!!) SQL SERVER ERROR (GetAccountInfo): %s", (const char*)x.ErrText());
		PutLogList(cTemp);

		return 1;
	}

	return 0;
}

void CMainLog::PutPacketLogData(DWORD dwMsgID, char *cData, DWORD dwMsgSize)
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
	if (pFile != NULL) fclose(pFile);
}

void CMainLog::CleanupLogFiles()
{
//	PutGMLogData(NULL, NULL, true);
//	PutItemLogData(NULL, NULL, true);
//	PutCrusadeLogData(NULL, NULL, true);
}
bool CMainLog::bReadServerConfigFile(char *cFn)
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
		return false;
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

				case 1: // Mainserver addresss
					strncpy(m_cMainServerAddress, token, 15);
					wsprintf(G_cTxt, "(*) Main-Log-Server Address : %s", m_cMainServerAddress);
					PutLogList(G_cTxt);
					cReadMode = 0;
					break;

				case 2: // backup-drive-letter
					m_cBackupDrive = token[0];
					wsprintf(G_cTxt, "(*) Backup Drive Letter : %c", m_cBackupDrive);
					PutLogList(G_cTxt);
					cReadMode = 0;
					break;

				case 3: // internal-main-server-port
					m_iMainServerInternalPort = atoi(token);
					wsprintf(G_cTxt, "(*) Main-Server Internal port : %d", m_iMainServerInternalPort);
					PutLogList(G_cTxt);
					cReadMode = 0;
					break;

				case 4:	// log server port
					m_iMainServerPort = atoi(token);
					wsprintf(G_cTxt, "(*) Main-Server Internal port : %d", m_iMainServerPort);
					PutLogList(G_cTxt);
					cReadMode = 0;
					break;
				
				}
			}
			else {
				if (memcmp(token, "log-server-address", 18) == 0)			cReadMode = 1;
				if (memcmp(token, "backup-drive-letter", 19) == 0)			cReadMode = 2;
				if (memcmp(token, "internal-log-server-port", 24) == 0)	cReadMode = 3;
				if (memcmp(token, "log-server-port", 15) == 0)			cReadMode = 4;
			}
			token = pStrTok->pGet();
		}
		delete pStrTok;
		delete cp;
		fclose(pFile);
	}
	
	return true;
}
void CMainLog::DeleteAccount(int iClientH, char cAccountName[11])
{
	int i, x;

	//useing account name
	if(iClientH == -1) {
		if(strlen(cAccountName) <= 0) return;
		for (i = 0; i < DEF_MAXACCOUNTS; i++)
			if((m_pAccountList[i] != NULL) && (strcmp(m_pAccountList[i]->cAccountName, cAccountName) == 0)) {
				for (x = 0; x < DEF_MAXCHARACTER; x++)
					if((m_pCharList[x] != NULL) && (m_pCharList[x]->iTracker == i)) {
					delete m_pCharList[x];
					m_pCharList[x] = NULL;
					}
					if(m_pClientList[m_pAccountList[i]->iClientH] != NULL) {
					delete m_pClientList[m_pAccountList[i]->iClientH];
					m_pClientList[m_pAccountList[i]->iClientH] = NULL;
					}
					wsprintf(G_cTxt, "(!)Account Deleted(%s)", m_pAccountList[i]->cAccountName);
					PutLogList(G_cTxt);
					delete m_pAccountList[i];
					m_pAccountList[i] = NULL;
			}
	}
	//ussing account id
	if(m_pAccountList[iClientH] == NULL) return;
	for (x = 0; x < DEF_MAXCHARACTER; x++)
		if((m_pCharList[x] != NULL) && (m_pCharList[x]->iTracker == iClientH)) {
		delete m_pCharList[x];
		m_pCharList[x] = NULL;
		}
		if(m_pClientList[m_pAccountList[iClientH]->iClientH] != NULL) {
			delete m_pClientList[m_pAccountList[iClientH]->iClientH];
			m_pClientList[m_pAccountList[iClientH]->iClientH] = NULL;
		}
		wsprintf(G_cTxt, "(!)Account Deleted(%s)", m_pAccountList[iClientH]->cAccountName);
		PutLogList(G_cTxt);
		delete m_pAccountList[iClientH];
		m_pAccountList[iClientH] = NULL;

}

/*
void CMainLog::CreateAccount(int iClientH, char* pData)
{//By Kiruku- from unadvised
	char* cp, cTxt[11], cDir[255], cAccountName[11], cFileName[255], cAccountPass[11], cCheckNewPass[11], m_cEmailAddr[50], m_cAccountQuiz[45], m_cAccountAnswer[20];
	int iAccount, iMessage;
	ZeroMemory(cFileName, sizeof(cFileName));
	ZeroMemory(cAccountName, sizeof(cAccountName));
	ZeroMemory(cAccountPass, sizeof(cAccountPass));
	ZeroMemory(cCheckNewPass, sizeof(cCheckNewPass));
	ZeroMemory(cTxt, sizeof(cTxt));
	iAccount = -1;
	iMessage = 0;
	FILE* pFile;
	if (m_pClientList[iClientH] == NULL) return;


	cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2); //incomeing message

	memcpy(cAccountName, cp, 10);
	cp += 10;

	memcpy(cAccountPass, cp, 10);
	cp += 10;

	memcpy(m_cEmailAddr, cp, 50);
	cp += 50;


	cp += 10; //Gender

	//memcpy(m_cAccountAge, cp, 10); //created...
	cp += 10;

	//   memcpy(cp, " ", 4);
	cp += 4;

	//   memcpy(cp, " ", 2);
	cp += 2;

	//   memcpy(cp, " ", 2);
	cp += 2;

	//   memcpy(m_cAccountCountry, cp, 17); //actually not needed...
	cp += 17;

	//   memcpy(m_cAccountSSN, cp, 28); //whatever this is its not needed to create =p
	cp += 28;

	memcpy(m_cAccountQuiz, cp, 45);
	cp += 45;

	memcpy(m_cAccountAnswer, cp, 20);
	cp += 20;

	//      memcpy(G_cCmdLineTokenA_Lowercase, cp, 50); no idea for what this is


	strcat(cFileName, "Account");
	strcat(cFileName, "\\");
	strcat(cFileName, "\\");
	wsprintf(cTxt, "AscII%d", *cAccountName);
	strcat(cFileName, cTxt);
	strcpy(cDir, cFileName);
	strcat(cFileName, "\\");
	strcat(cFileName, "\\");
	strcat(cFileName, cAccountName);
	strcat(cFileName, ".txt");

	_mkdir("Account");
	_mkdir(cDir);

	if (fopen(cFileName, "rt") != NULL) {

		wsprintf(G_cTxt, "(X) Cannot create new Account - Already existing Account(%s)", cFileName);
		PutLogList(G_cTxt);
		SendEventToWLS(DEF_LOGRESMSGTYPE_ALREADYEXISTINGACCOUNT, DEF_LOGRESMSGTYPE_ALREADYEXISTINGACCOUNT, 0, 0, iClientH);

		return;
	}

	pFile = fopen(cFileName, "wt");
	if (pFile == NULL) {
		wsprintf(G_cTxt, "(!) Cannot create new Account - cannot create file : Name(%s)", cFileName);
		PutLogList(G_cTxt);
		SendEventToWLS(DEF_LOGRESMSGTYPE_NEWACCOUNTFAILED, DEF_LOGRESMSGTYPE_NEWACCOUNTFAILED, 0, 0, iClientH);
	}
	else {
		CreateNewAccountFile(pData, cFileName, cAccountName, cAccountPass, m_cEmailAddr, m_cAccountQuiz, m_cAccountAnswer);
		fwrite(pData, 1, strlen(pData), pFile);
		fclose(pFile);
		wsprintf(G_cTxt, "(O) New Accout(%s) created.", cAccountName);
		PutLogList(G_cTxt);
		SendEventToWLS(DEF_LOGRESMSGTYPE_NEWACCOUNTCREATED, DEF_LOGRESMSGTYPE_NEWACCOUNTCREATED, 0, 0, iClientH);
	}
}

void CMainLog::CreateNewAccountFile(char* pData, char* cFn, char* cAccountName, char* cAccountPassword, char* m_cEmailAddr, char* m_cAccountQuiz, char* m_cAccountAnswer)
{//By Kiruku- from unadvised

	SYSTEMTIME SysTime;
	char cTxt[120];

	ZeroMemory(cTxt, sizeof(cTxt));
	GetLocalTime(&SysTime);
	ZeroMemory(pData, sizeof(pData));

	wsprintf(cTxt, "Account-generated:  Time(%d/%d/%d/%d/%d)", SysTime.wYear, SysTime.wMonth, SysTime.wDay, SysTime.wHour, SysTime.wMinute);
	strcat(pData, cTxt);
	strcat(pData, "\n");

	strcat(pData, "[NAME]\n");

	strcat(pData, "account-name     = ");
	strcat(pData, cAccountName);
	strcat(pData, "\n\n");

	strcat(pData, "[PASSWORD]");
	strcat(pData, "\n\n");

	strcat(pData, "account-password   = ");
	strcat(pData, cAccountPassword);
	strcat(pData, "\n\n");

	strcat(pData, "[ACCOUNT-STATUS]\n\n");

	strcat(pData, "account-valid-time = 0\n");
	strcat(pData, "account-valid-date = ");
	wsprintf(cTxt, "%d %d %d", SysTime.wYear, SysTime.wMonth, SysTime.wDay);
	strcat(pData, cTxt);
	strcat(pData, "\n\n");
	strcat(pData, "account-change-password = 2008 6 14\n");
	strcat(pData, "[ETC]\n\n");

	strcat(pData, "account-Gender   =  \n");
	strcat(pData, "account-Age      = \n");
	strcat(pData, "account-Birth-Year =  \n");

	strcat(pData, "account-Birth-Month =  ");
	strcat(pData, "\n");

	strcat(pData, "account-Birth-Day   =  ");
	strcat(pData, "\n");

	strcat(pData, "account-RealName = ");
	strcat(pData, "\n");

	strcat(pData, "account-SSN      = -\n");

	strcat(pData, "account-Email     = ");
	strcat(pData, m_cEmailAddr);
	strcat(pData, "\n");

	strcat(pData, "account-Quiz     = ");
	strcat(pData, m_cAccountQuiz);
	strcat(pData, "\n");

	strcat(pData, "account-Answer   =");
	strcat(pData, m_cAccountAnswer);
	strcat(pData, "\n");

	strcat(pData, "account-created-by = Client Account Creator");
	strcat(pData, "\n\n");

	strcat(pData, "[CHARACTERS]");
	strcat(pData, "\n\n\n");

}
*/

void CMainLog::CreateAccount(int iClientH, char* pData)
{
	char* cp;
	char m_cAccountName[11], m_cAccountPassword[11], m_cEmailAddr[51], m_cGender[11], m_cAccountAge[11];
	char m_cDno1[41], m_cDno2[21], m_cDno3[21], m_cAccountSSN[29], m_cAccountCountry[18], m_cAccountQuiz[46];
	char m_cAccountAnswe[21], G_cCmdLineTokenA_Lowercase[51];
	char cFn[120], cFile[20000], cBuffer[256];

	SYSTEMTIME SysTime;

	GetLocalTime(&SysTime);
	FILE* pFile;
	HANDLE hFile;

	DWORD  dwFileSize;

	ZeroMemory(m_cAccountName, sizeof(m_cAccountName));
	ZeroMemory(m_cAccountPassword, sizeof(m_cAccountPassword));
	ZeroMemory(m_cEmailAddr, sizeof(m_cEmailAddr));
	ZeroMemory(m_cGender, sizeof(m_cGender));
	ZeroMemory(m_cAccountAge, sizeof(m_cAccountAge));
	ZeroMemory(m_cDno1, sizeof(m_cDno1));
	ZeroMemory(m_cDno2, sizeof(m_cDno2));
	ZeroMemory(m_cDno3, sizeof(m_cDno3));
	ZeroMemory(m_cAccountCountry, sizeof(m_cAccountCountry));
	ZeroMemory(m_cAccountSSN, sizeof(m_cAccountSSN));
	ZeroMemory(m_cAccountQuiz, sizeof(m_cAccountQuiz));
	ZeroMemory(m_cAccountAnswe, sizeof(m_cAccountAnswe));
	ZeroMemory(G_cCmdLineTokenA_Lowercase, sizeof(G_cCmdLineTokenA_Lowercase));
	ZeroMemory(cFile, sizeof(cFile));
	ZeroMemory(cBuffer, sizeof(cBuffer));
	ZeroMemory(cFn, sizeof(cFn));
	

	cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);

	memcpy(m_cAccountName, cp, 10);
	cp += 10;

	memcpy(m_cAccountPassword, cp, 10);
	cp += 10;

	memcpy(m_cEmailAddr, cp, 50);
	cp += 50;

	memcpy(m_cGender, cp, 10);
	cp += 10;

	memcpy(m_cAccountAge, cp, 10);
	cp += 10;

	memcpy(m_cDno1, cp, 4);
	cp += 4;

	memcpy(m_cDno2, cp, 4);
	cp += 2;

	memcpy(m_cDno3, cp, 4);
	cp += 2;

	memcpy(m_cAccountCountry, cp, 17);
	cp += 17;

	memcpy(m_cAccountSSN, cp, 28);
	cp += 28;

	memcpy(m_cAccountQuiz, cp, 45);
	cp += 45;

	memcpy(m_cAccountAnswe, cp, 20);
	cp += 20;

	memcpy(G_cCmdLineTokenA_Lowercase, cp, 50);
	cp += 50;


		bool exists = bCheckAccountExists(m_cAccountName);

		if (exists)
		{
			wsprintf(cBuffer, "(!) Cannot Duplicate Account: %s", m_cAccountName);
			PutLogList(cBuffer);
			if (m_pClientList[iClientH] != NULL)
				SendEventToWLS(DEF_LOGRESMSGTYPE_ALREADYEXISTINGACCOUNT, DEF_LOGRESMSGTYPE_ALREADYEXISTINGACCOUNT, 0, 0, iClientH);
			return;
		}
		else
		{
			SACommand com;

			try
			{
				com.setConnection(&con);
				com.setCommandText("INSERT INTO Accounts ([Account-Name], [Account-Password], [Account-Email], [Account-Quiz], [Account-Answer]) VALUES (:1, :2, :3, :4, :5)");
				com.Param(1).setAsString() = m_cAccountName;
				com.Param(2).setAsString() = m_cAccountPassword;
				com.Param(3).setAsString() = m_cEmailAddr;
				com.Param(4).setAsString() = m_cAccountQuiz;
				com.Param(5).setAsString() = m_cAccountAnswe;

				com.Execute();

				wsprintf(cBuffer, "(!) Account Created: %s", m_cAccountName);
				PutLogList(cBuffer);

				com.Close();

				if (m_pClientList[iClientH] != NULL)
					SendEventToWLS(DEF_LOGRESMSGTYPE_NEWACCOUNTCREATED, DEF_LOGRESMSGTYPE_NEWACCOUNTCREATED, 0, 0, iClientH);
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
				wsprintf(cTemp, "(!!!) SQL SERVER ERROR (CreateAccount): %s", (const char*)x.ErrText());
				PutLogList(cTemp);

				try { com.Close(); }
				catch (...) {}

				if (m_pClientList[iClientH] != NULL)
					SendEventToWLS(DEF_LOGRESMSGTYPE_NEWACCOUNTFAILED, DEF_LOGRESMSGTYPE_NEWACCOUNTFAILED, 0, 0, iClientH);
			}
		}

}

bool CMainLog::bCheckAccountExists(char* cAccountName)
{
	SACommand com;

	try
	{
		com.setConnection(&con);
		com.setCommandText("SELECT     COUNT(*) FROM         Accounts WHERE     ([Account-Name] = :1)");
		com.Param(1).setAsString() = cAccountName;

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
		wsprintf(cTemp, "(!!!) SQL SERVER ERROR (bCheckAccountExists): %s", (const char*)x.ErrText());
		PutLogList(cTemp);

		return false;
	}
}