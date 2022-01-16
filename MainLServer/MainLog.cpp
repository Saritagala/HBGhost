#include "MainLog.h"
#include <stdlib.h>
#include <stdio.h>
#include <direct.h>

#pragma warning (disable : 6011 4996 6385 6386 4244)

extern void		PutLogList(char* cMsg);
extern char		G_cTxt[500];
extern char		G_cData50000[50000];

bool CMainLog::bAccept(class XSocket* pXSock, bool bWorld)
{
	int i, x;
	int sTotalConnection;
	class XSocket* pTmpSock;
	bool bFlag;
	sTotalConnection = 0;

	bFlag = false;
	for (i = 1; i < DEF_MAXCLIENTSOCK; i++) { //loop 1
		if (m_pClientList[i] == 0) { //if client null
			m_pClientList[i] = new class CClient(m_hWnd);
			pXSock->bAccept(m_pClientList[i]->m_pXSock, WM_ONCLIENTSOCKETEVENT + i);
			m_pClientList[i]->m_pXSock->bInitBufferSize(DEF_MSGBUFFERSIZE);
			ZeroMemory(m_pClientList[i]->m_cIPaddress, sizeof(m_pClientList[i]->m_cIPaddress));
			m_pClientList[i]->m_pXSock->iGetPeerAddress(m_pClientList[i]->m_cIPaddress);

			//no overflowing sockets
			for (x = 1; x < DEF_MAXCLIENTSOCK; x++)
				if (m_pClientList[x] != 0) {
					if (strcmp(m_pClientList[x]->m_cIPaddress, m_pClientList[i]->m_cIPaddress) == 0) sTotalConnection++;
				}

			if (sTotalConnection > 20) {
				delete m_pClientList[i];
				m_pClientList[i] = 0;
				return false;
			}

			return true;
		} //end client == null
	} //end loop 1
	pTmpSock = new class XSocket(m_hWnd, DEF_SERVERSOCKETBLOCKLIMIT);
	pXSock->bAccept(pTmpSock, 0);
	delete pTmpSock;
	return false;
}

CMainLog::CMainLog(HWND hWnd)
{
	int i;

	for (i = 0; i < DEF_MAXCLIENTSOCK; i++) m_pClientList[i] = 0;
	for (i = 0; i < DEF_MSGQUEUESIZE; i++) m_pMsgQueue[i] = 0;
	for (i = 0; i < DEF_MAXGAME; i++) m_pGameList[i] = 0;
	for (i = 0; i < DEF_MAXMAPS; i++) m_pMapList[i] = 0;
	for (i = 0; i < DEF_MAXACCOUNTS; i++) m_pAccountList[i] = 0;
	for (i = 0; i < DEF_MAXCHARACTER; i++) m_pCharList[i] = 0;

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
		if (m_pClientList[i] != 0) delete m_pClientList[i];
	for (i = 0; i < DEF_MSGQUEUESIZE; i++)
		if (m_pMsgQueue[i] != 0) delete m_pMsgQueue[i];
	for (i = 0; i < DEF_MAXGAME; i++)
		if (m_pGameList[i] != 0) delete m_pGameList[i];
	for (i = 0; i < DEF_MAXMAPS; i++)
		if (m_pMapList[i] == 0) delete m_pMapList[i];
	for (i = 0; i < DEF_MAXACCOUNTS; i++)
		if (m_pAccountList[i] == 0) delete m_pAccountList[i];
	for (i = 0; i < DEF_MAXCHARACTER; i++)
		if (m_pCharList[i] == 0) delete m_pCharList[i];
}

bool CMainLog::bInit()
{
	if (bReadServerConfigFile("HMLServer.cfg") == false) return false;

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

void CMainLog::MsgProcess()
{
	char* pData = 0, * cp, cFrom, cKey, cTemp[11];
	UINT32    dwMsgSize, * dwpMsgID;
	UINT16* wpMsgType;
	int     i, iClientH;
	ZeroMemory(cTemp, sizeof(cTemp));

	ZeroMemory(m_pMsgBuffer, DEF_MSGBUFFERSIZE + 1);
	pData = (char*)m_pMsgBuffer;
	while (bGetMsgQuene(&cFrom, pData, &dwMsgSize, &iClientH, &cKey) == true)
	{
		dwpMsgID = (UINT32*)(pData + DEF_INDEX4_MSGID);
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
			wpMsgType = (UINT16*)(pData + DEF_INDEX2_MSGTYPE);
			switch (*wpMsgType) {
			case DEF_MSGTYPE_CONFIRM:
				RegisterSocketWL(iClientH, pData, true);
				break;
			case DEF_MSGTYPE_REJECT:
				RegisterSocketWL(iClientH, pData, false);
				break;
			}
			break;
		case MSGID_WORLDSERVERACTIVATED:
			if (m_pClientList[iClientH] == 0) return;
			for (i = 1; i < DEF_MAXCLIENTSOCK; i++) {
				if ((m_pClientList[i] != 0) && (strcmp(m_pClientList[i]->m_cWorldName, m_pClientList[iClientH]->m_cWorldName) == 0))
					m_pClientList[i]->m_cMode = 1; //WLserver Activated
			}
			m_iTotalWorld++;
			PutLogList(" ");
			wsprintf(G_cTxt, "(!) WorldServer(%s) Activated", m_pClientList[iClientH]->m_cWorldName);
			PutLogList(G_cTxt);
			break;

		case MSGID_REQUEST_REMOVEGAMESERVER: //remove game server
			wpMsgType = (UINT16*)(pData + DEF_INDEX2_MSGTYPE);
			switch (*wpMsgType) {
			case DEF_MSGTYPE_CONFIRM:
				RegisterGameServer(iClientH, pData, false);
				break;
			}
			break;

		case MSGID_REGISTER_WORLDSERVER_GAMESERVER: //ad game server
			wpMsgType = (UINT16*)(pData + DEF_INDEX2_MSGTYPE);
			switch (*wpMsgType) {
			case DEF_MSGTYPE_CONFIRM:
				RegisterGameServer(iClientH, pData, true);
				break;
			}
			break;

		case MSGID_RESPONSE_CHARINFOLIST: //get all character info
			TotalChar(iClientH, pData);
			break;

		case MSGID_RESPONSE_CHARACTERLOG:
			wpMsgType = (UINT16*)(pData + DEF_INDEX2_MSGTYPE);
			switch (*wpMsgType) {
			case DEF_LOGRESMSGTYPE_NEWCHARACTERDELETED:
				DeleteCharacter(iClientH, pData, 1);
				break;
			case DEF_LOGRESMSGTYPE_NEWCHARACTERCREATED:
				ResponseCharacter(iClientH, pData, 1);
				break;
			case DEF_LOGRESMSGTYPE_NEWCHARACTERFAILED:
				cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);

				memcpy(cTemp, cp, 10);
				cp += 10;

				for (i = 0; i < DEF_MAXACCOUNTS; i++) {
					if ((m_pAccountList[i] != 0) && (memcmp(m_pAccountList[i]->cAccountName, cTemp, 10) == 0)) {
						if (m_pClientList[m_pAccountList[i]->iClientH] != 0)
							SendEventToWLS(DEF_LOGRESMSGTYPE_NEWCHARACTERFAILED, DEF_LOGRESMSGTYPE_NEWCHARACTERFAILED, 0, 0, m_pAccountList[i]->iClientH);
					}
				}
				break;
			case DEF_LOGRESMSGTYPE_ALREADYEXISTINGCHARACTER:
				cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);

				memcpy(cTemp, cp, 10);
				cp += 10;

				for (i = 0; i < DEF_MAXACCOUNTS; i++) {
					if ((m_pAccountList[i] != 0) && (memcmp(m_pAccountList[i]->cAccountName, cTemp, 10) == 0)) {
						if (m_pClientList[m_pAccountList[i]->iClientH] != 0)
							SendEventToWLS(DEF_LOGRESMSGTYPE_ALREADYEXISTINGCHARACTER, DEF_LOGRESMSGTYPE_ALREADYEXISTINGCHARACTER, 0, 0, m_pAccountList[i]->iClientH);
					}
				}
				break;
			}
			break;

		case MSGID_REQUEST_CLEARACCOUNTSTATUS:
			cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);

			memcpy(cTemp, cp, 10);
			cp += 10;

			for (i = 0; i < DEF_MAXACCOUNTS; i++) {
				if ((m_pAccountList[i] != 0) && (memcmp(m_pAccountList[i]->cAccountName, cTemp, 10) == 0)) {
					m_iValidAccounts--;
					delete m_pAccountList[i];
					m_pAccountList[i] = 0;
				}
			}
			break;

		default:
			cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);
			memcpy(m_pClientList[iClientH]->m_cWorldName, cp, 30);
			cp += 30;
			memcpy(m_pClientList[iClientH]->m_cWorldServerAddress, cp, 16);
			cp += 16;
			wsprintf(G_cTxt, "(OOO) World Server(%s:%s) registration success!", m_pClientList[iClientH]->m_cWorldName, m_pClientList[iClientH]->m_cWorldServerAddress);
			PutLogList(G_cTxt);
			break;
		}
		break;
	}
	UpdateScreen();
}

bool CMainLog::bGetMsgQuene(char* pFrom, char* pData, UINT32* pMsgSize, int* pIndex, char* pKey)
{

	if (m_pMsgQueue[m_iQueueHead] == 0) return false;

	m_pMsgQueue[m_iQueueHead]->Get(pFrom, pData, pMsgSize, pIndex, pKey);

	delete m_pMsgQueue[m_iQueueHead];
	m_pMsgQueue[m_iQueueHead] = 0;

	m_iQueueHead++;
	if (m_iQueueHead >= DEF_MSGQUEUESIZE) m_iQueueHead = 0;

	return true;
}
bool CMainLog::bPutMsgQuene(char cFrom, char* pData, UINT32 dwMsgSize, int iIndex, char cKey)
{
	if (m_pMsgQueue[m_iQueueTail] != 0) return false;

	m_pMsgQueue[m_iQueueTail] = new class CMsg;
	if (m_pMsgQueue[m_iQueueTail] == 0) return false;

	if (m_pMsgQueue[m_iQueueTail]->bPut(cFrom, pData, dwMsgSize, iIndex, cKey) == false) return false;

	m_iQueueTail++;
	if (m_iQueueTail >= DEF_MSGQUEUESIZE) m_iQueueTail = 0;

	return true;
}
void CMainLog::OnTimer()
{
	MsgProcess();
}
//send message to WLserver
void CMainLog::SendEventToWLS(UINT32 dwMsgID, UINT16 wMsgType, char* pData, UINT32 dwMsgSize, int iWorldH)
{
	int iRet;
	UINT32* dwp;
	char* cp;
	UINT16* wp;
	int m_iCurWorldLogSockIndex;

	if (iWorldH == -1) return;
	else m_iCurWorldLogSockIndex = iWorldH;

	ZeroMemory(G_cData50000, sizeof(G_cData50000));

	dwp = (UINT32*)(G_cData50000 + DEF_INDEX4_MSGID);
	*dwp = dwMsgID;
	wp = (UINT16*)(G_cData50000 + DEF_INDEX2_MSGTYPE);
	*wp = wMsgType;

	cp = (char*)(G_cData50000 + DEF_INDEX2_MSGTYPE + 2);

	memcpy((char*)cp, pData, dwMsgSize);

	if (iWorldH != -1 && m_pClientList[m_iCurWorldLogSockIndex] != 0) { //if WLserver registered
		switch (dwMsgID) {
		case MSGID_RESPONSE_REGISTER_WORLDSERVERSOCKET:
			iRet = m_pClientList[m_iCurWorldLogSockIndex]->m_pXSock->iSendMsg(G_cData50000, dwMsgSize + 6, DEF_USE_ENCRYPTION);
			break;
		default:
			iRet = m_pClientList[m_iCurWorldLogSockIndex]->m_pXSock->iSendMsg(G_cData50000, dwMsgSize + 6, DEF_USE_ENCRYPTION); //send anyway..
			break;
		}
		switch (iRet) {
		case DEF_XSOCKEVENT_QUENEFULL:
		case DEF_XSOCKEVENT_SOCKETERROR:
		case DEF_XSOCKEVENT_CRITICALERROR:
		case DEF_XSOCKEVENT_SOCKETCLOSED:
			delete m_pClientList[m_iCurWorldLogSockIndex];
			m_pClientList[m_iCurWorldLogSockIndex] = 0;
			wsprintf(G_cTxt, "(!) Connection Lost (%d) - (%s)", m_iCurWorldLogSockIndex, m_pClientList[m_iCurWorldLogSockIndex]->m_cWorldName);
			PutLogList(G_cTxt);
			break;
		}
	}
}


void CMainLog::OnClientSubLogSocketEvent(UINT message, WPARAM wParam, LPARAM lParam) //From Wl or client
{
	int iWorldH, iRet, iTmp, i, iTotalSock;
	char* pData, cKey;
	UINT32  dwMsgSize;
	iTotalSock = 0;

	iTmp = WM_ONCLIENTSOCKETEVENT;
	iWorldH = (int)(message - iTmp);
	if (m_pClientList[iWorldH] == 0) return;
	iRet = m_pClientList[iWorldH]->m_pXSock->iOnSocketEvent(wParam, lParam);
	switch (iRet) {

	case DEF_XSOCKEVENT_READCOMPLETE:
		pData = m_pClientList[iWorldH]->m_pXSock->pGetRcvDataPointer(&dwMsgSize, &cKey);
		if (bPutMsgQuene(DEF_MSGFROM_LOGSERVER, pData, dwMsgSize, iWorldH, cKey) == false) {
			PutLogList("@@@@@@ CRITICAL ERROR in LOGSERVER MsgQuene!!! @@@@@@");
		}
		break;

	case DEF_XSOCKEVENT_BLOCK:
		PutLogList("Socket BLOCKED!");
		break;

	case DEF_XSOCKEVENT_CONFIRMCODENOTMATCH:
		wsprintf(G_cTxt, "<%d> Confirmcode notmatch!", iWorldH);
		PutLogList(G_cTxt);
		delete m_pClientList[iWorldH];
		m_pClientList[iWorldH] = 0;
		break;

	case DEF_XSOCKEVENT_MSGSIZETOOLARGE:
	case DEF_XSOCKEVENT_SOCKETERROR:
	case DEF_XSOCKEVENT_SOCKETCLOSED:
	case DEF_XSOCKEVENT_QUENEFULL:
		//delete world socket
		if (m_pClientList[iWorldH]->m_iWorldPort != 0) {
			wsprintf(G_cTxt, "(X) World-Server-socket(%d) error!", iWorldH);
			PutLogList(G_cTxt);
			for (i = 1; i < DEF_MAXCLIENTSOCK; i++) {
				if ((m_pClientList[i] != 0) && (strcmp(m_pClientList[i]->m_cWorldName, m_pClientList[iWorldH]->m_cWorldName) == 0)) iTotalSock++;
			}
			if (iTotalSock - 1 <= 0) {
				PutLogList(" ");
				wsprintf(G_cTxt, "(X) World-Server(%s) Deleted!", m_pClientList[iWorldH]->m_cWorldName);
				PutLogList(G_cTxt);
				m_iTotalWorld--;
			}
		}
		//delete an account
		if (m_pClientList[iWorldH]->m_cMode == 2 && m_pClientList[iWorldH]->m_cWorldName != 0) {

			DeleteAccount(-1, m_pClientList[iWorldH]->m_cWorldName); //delete account name
		}
		delete m_pClientList[iWorldH];
		m_pClientList[iWorldH] = 0;
		break;
	}
}
void CMainLog::RegisterSocketWL(int iClientH, char* pData, bool bRegister)
{
	char cData[100];
	UINT32* dwp;
	char* cp;
	ZeroMemory(cData, sizeof(cData));
	if (m_pClientList[iClientH] == 0) return; //WL socket connected
	if (bRegister == false) {
		delete m_pClientList[iClientH];
		m_pClientList[iClientH] = 0;
		wsprintf(G_cTxt, "(!) Socket Register Failed (%d)", iClientH);
		PutLogList(G_cTxt);
		return;
	}

	cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);

	memcpy(m_pClientList[iClientH]->m_cWorldName, cp, 30);
	cp += 30;

	memcpy(m_pClientList[iClientH]->m_cWorldServerAddress, cp, 16);
	cp += 16;

	dwp = (UINT32*)cp;
	m_pClientList[iClientH]->m_iWorldPort = *dwp;
	cp += 4;

	wsprintf(G_cTxt, "(O) Client (%d) is socket to World Server(%s)", iClientH, m_pClientList[iClientH]->m_cWorldName);
	PutLogList(G_cTxt);

	SendEventToWLS(MSGID_RESPONSE_REGISTER_WORLDSERVERSOCKET, DEF_MSGTYPE_CONFIRM, cData, 0, iClientH);
}
void CMainLog::RegisterGameServer(int iClientH, char* pData, bool bRegister)
{
	char cData[100], m_cWorldName[31], m_cGameName[11], m_cGameAddress[16];
	UINT32* dwp;
	char* cp, * cMapNames;
	int m_iGamePort, m_iTotalMaps, i, x;
	bool bwlregister;
	bwlregister = false;
	ZeroMemory(cData, sizeof(cData));
	ZeroMemory(m_cWorldName, sizeof(m_cWorldName));
	ZeroMemory(m_cGameName, sizeof(m_cGameName));
	ZeroMemory(m_cGameAddress, sizeof(m_cGameAddress));

	if (m_pClientList[iClientH] == 0) return;
	if (bRegister) {

		cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);

		memcpy(m_cWorldName, cp, 30);
		cp += 30;

		memcpy(m_cGameName, cp, 10);
		cp += 10;

		memcpy(m_cGameAddress, cp, 16);
		cp += 16;

		dwp = (UINT32*)cp;
		m_iGamePort = *dwp;
		cp += 4;

		dwp = (UINT32*)cp;
		m_iTotalMaps = *dwp;
		cp += 4;

		for (i = 1; i < DEF_MAXCLIENTSOCK; i++) {
			if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cWorldName, m_cWorldName, 30) == 0) && (m_pClientList[i]->m_cMode == 1)) bwlregister = true;
		}
		if (bwlregister == true) {
			wsprintf(G_cTxt, "(OOO) %s-Game Server(%s:%s:%d) registration success", m_cWorldName, m_cGameName, m_cGameAddress, m_iGamePort);
			PutLogList(G_cTxt);
			for (i = 0; i < DEF_MAXGAME; i++) {
				if (m_pGameList[i] == 0) {
					cMapNames = (char*)cp;
					m_pGameList[i] = new class CGame(iClientH, m_cGameName, m_cGameAddress, m_iGamePort, m_cWorldName, m_iTotalMaps);
					for (x = 0; x < m_iTotalMaps; x++) {
						if (bClientRegisterMaps(i, cp) == 0) {

						}
						cp += 11;
					}
					m_iTotalGame++;
					break;
				}
			} //if game server == nULL
		}// WLserver is registered
	}
	else {

		cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);

		memcpy(m_cWorldName, cp, 30);
		cp += 30;

		memcpy(m_cGameName, cp, 10);
		cp += 10;

		for (i = 0; i < DEF_MAXGAME; i++) {
			if ((m_pGameList[i] != 0) && (memcmp(m_pGameList[i]->m_cWorldName, m_cWorldName, 30) == 0) && (memcmp(m_pGameList[i]->m_cGameName, m_cGameName, 10) == 0)) {
				PutLogList(" ");
				wsprintf(G_cTxt, "Delete Game Server(%s) in World Server(%s)", m_pGameList[i]->m_cGameName, m_cWorldName);
				PutLogList(G_cTxt);
				//delete maps on game server
				for (x = 0; x < DEF_MAXMAPS; x++) {
					if ((m_pMapList[x] != 0) && (m_pMapList[x]->iIndex == i)) {
						wsprintf(G_cTxt, "Delete Map(%s) in Game Server(%s)", m_pMapList[x]->m_cMapName, m_pGameList[i]->m_cGameName);
						PutLogList(G_cTxt);
						delete m_pMapList[x];
						m_pMapList[x] = 0;
					}
				}
				delete m_pGameList[i];
				m_pGameList[i] = 0;
				m_iTotalGame--;
			}
		}
	}
}
bool CMainLog::bClientRegisterMaps(int iClientH, char* pData)
{
	int i;

	for (i = 0; i < DEF_MAXMAPS; i++) {
		if (m_pMapList[i] != 0) {
			if (memcmp(m_pMapList[i]->m_cMapName, pData, 10) == 0) {
				wsprintf(G_cTxt, "(X) CRITICAL ERROR! Map(%s) duplicated!", pData);
				PutLogList(G_cTxt);
				return false;
			}
		}
	}
	for (i = 0; i < DEF_MAXMAPS; i++) {
		if (m_pMapList[i] == 0) {
			m_pMapList[i] = new class CMap(iClientH, pData);
			wsprintf(G_cTxt, "(!) Map(%s) registration: %s MapNum(%d)", pData, m_pGameList[iClientH]->m_cGameName, i+1);
			PutLogList(G_cTxt);
			return true;
		}
	}
	wsprintf(G_cTxt, "(X) No more map space left. Map(%s) cannot be added.", pData);
	PutLogList(G_cTxt);
	return false;
}
void CMainLog::RequestLogin(int iClientH, char* pData)
{
	int  iMessage, iAccount, i, iDBID = -1;
	char cData[200], cMesg;
	UINT32* dwp;

	char cTotalChar;
	
	char* cp, cAccountName[11], cAccountPass[11], cWorldName[30], *cp2;
	ZeroMemory(cAccountName, sizeof(cAccountName));
	ZeroMemory(cAccountPass, sizeof(cAccountPass));
	ZeroMemory(cWorldName, sizeof(cWorldName));
	ZeroMemory(cData, sizeof(cData));
	cMesg = 0;
	cTotalChar = 0;
	iAccount = -1;

	if (m_pClientList[iClientH] == 0) return;

	cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);

	memcpy(cAccountName, cp, 10);
	cp += 10;
	memcpy(cAccountPass, cp, 10);
	cp += 10;
	memcpy(cWorldName, cp, 30);
	cp += 30;

	iMessage = GetAccountInfo(iClientH, cAccountName, cAccountPass, cWorldName, &iAccount, &iDBID);
	switch (iMessage) {
	case 0: //0 errors check password and send success message
		if (m_pAccountList[iAccount] == 0) cMesg = 1;
		else if (strcmp(m_pAccountList[iAccount]->cAccountName, cAccountName) != 0) cMesg = 1;
		else if (strcmp(m_pAccountList[iAccount]->cWorldName, cWorldName) != 0) cMesg = 2;
		else if (strcmp(m_pAccountList[iAccount]->cPassword, cAccountPass) != 0) cMesg = 3;
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
	switch (cMesg) {
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
		
		m_pAccountList[iAccount]->dAccountID = iDBID;

		if (!(m_pAccountList[iAccount]->iClientH == iClientH))
		{
			DeleteAccount(iAccount);
			return;
		}

		SendCharacterData(iAccount, iDBID, iClientH, cAccountName);
		break;
	}
}

int CMainLog::iGetAccountDatabaseID(char* cAccName)
{
	SACommand com;

	try
	{
		com.setConnection(&con);
		com.setCommandText("SELECT [Account-ID] FROM Accounts WHERE [Account-Name] = :1");
		com.Param(1).setAsString() = cAccName;

		com.Execute();

		int id = -1;
		if (com.isResultSet())
		{
			
			while (com.FetchNext())
			{
				if (com.Field("Account-ID").asLong() > 0) 
				{
					id = com.Field("Account-ID").asLong();
					break;
				}
			}
			
		}
		com.Close();
		return id;
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

void CMainLog::SendCharacterData(int iAccount, int iAccountID, int iClientH, char* cAccountName)
{
	char* cp, cData[2000];
	UINT32* dwp;
	UINT16* wp;
	int i;
	bool bConnected = false;
	char iRows;

	ZeroMemory(cData, sizeof(cData));

	cp = (char*)(cData); //outgoing messag

	dwp = (UINT32*)cp;
	*dwp = DEF_UPERVERSION;
	cp += 2;
	//2
	dwp = (UINT32*)cp;
	*dwp = DEF_LOWERVERSION;
	cp += 2;
	//4
		//used for account status
	cp++;
	//5
	dwp = (UINT32*)cp;
	*dwp = m_pAccountList[iAccount]->m_iAccntYear;
	cp += 2;
	//7
	dwp = (UINT32*)cp;
	*dwp = m_pAccountList[iAccount]->m_iAccntMonth;
	cp += 2;
	//9
	dwp = (UINT32*)cp;
	*dwp = m_pAccountList[iAccount]->m_iAccntDay;
	cp += 2;
	//11
	dwp = (UINT32*)cp;
	*dwp = m_pAccountList[iAccount]->m_iPassYear;
	cp += 2;
	//13
	dwp = (UINT32*)cp;
	*dwp = m_pAccountList[iAccount]->m_iPassMonth;
	cp += 2;
	//15
	dwp = (UINT32*)cp;
	*dwp = m_pAccountList[iAccount]->m_iPassDay;
	cp += 2;
	//17
	
	if (m_pAccountList[iAccount] != 0) {

		if (m_pClientList[iClientH] == 0) {
			DeleteAccount(iAccount);
			return;
		}

		if (strcmp(m_pClientList[iClientH]->m_cWorldName, m_pAccountList[iAccount]->cAccountName) != 0) {
			DeleteAccount(iAccount);
			return;
		}
	}

	char* pTotal;
	
	pTotal = (char*)cp;
	cp++;
	//18

	int accDBID = iGetAccountDatabaseID(cAccountName);
	iRows = 0;

	SACommand com;


	try
	{
		com.setConnection(&con);
		com.setCommandText("SELECT [Character-ID], RTRIM([Character-Name]) AS 'Character-Name', [Character-Location], [Character-Guild-Name], [Character-Guild-GUID], [Character-Guild-Rank], [Character-Loc-Map], [Character-Loc-X], [Character-Loc-Y], [Character-HP], [Character-MP], [Character-SP], [Character-Level], [Character-Rating], [Character-Strength], [Character-Intelligence], [Character-Vitality], [Character-Dexterity], [Character-Magic], [Character-Charisma], [Character-Luck], [Character-Experience], [Character-LU-Pool], [Character-Ek-Count], [Character-Pk-Count], [Character-Reward-Gold], [Character-ID1], [Character-ID2], [Character-ID3], [Character-Sex], [Character-Skin], [Character-Hair-Style], [Character-Hair-Colour], [Character-Underwear], [Character-Hunger], [Character-Shutup-Time], [Character-Rating-Time], [Character-Force-Time], [Character-SP-Time], [Character-Admin-Level], [Character-Contribution], [Character-War-Contribution], [Character-Event-ID], [Character-Criticals], [Character-Ability-Time], [Character-Lock-Map], [Character-Lock-Time], [Character-Crusade-Job], [Character-Crusade-GUID], [Character-Construct-Points], [Character-Death-Time], [Character-Party-ID], [Character-Majestics], [Character-Appr1], [Character-Appr2], [Character-Appr3], [Character-Appr4], [Character-Appr-Colour] FROM Characters WHERE [Account-ID] = :1");
		com.Param(1).setAsLong() = accDBID;

		com.Execute();

		if (com.isResultSet())
		{
			while (com.FetchNext())
			{
				// char map
				memcpy(cp, com.Field("Character-Name").asString(), 10);
				cp += 10;
				//10
				*cp = 1;
				cp++;
				//11
				// char appr1
				wp = (UINT16*)cp;
				*wp = com.Field("Character-Appr1").asLong();
				cp += 2;
				//13
				// char appr2
				wp = (UINT16*)cp;
				*wp = com.Field("Character-Appr2").asLong();
				cp += 2;
				//15
				// char appr3
				wp = (UINT16*)cp;
				*wp = com.Field("Character-Appr3").asLong();
				cp += 2;
				//17
				// char appr1
				wp = (UINT16*)cp;
				*wp = com.Field("Character-Appr4").asLong();
				cp += 2;
				//19
				// char gender
				wp = (UINT16*)cp;
				*wp = com.Field("Character-Sex").asLong();
				cp += 2;
				//21
				// char skin
				wp = (UINT16*)cp;
				*wp = com.Field("Character-Skin").asLong();
				cp += 2;
				//23
				// char level
				wp = (UINT16*)cp;
				*wp = com.Field("Character-Level").asLong();
				cp += 2;
				//25
				// char exp
				dwp = (UINT32*)cp;
				*dwp = com.Field("Character-Experience").asLong();
				cp += 4;
				//29
				// char str
				wp = (UINT16*)cp;
				*wp = com.Field("Character-Strength").asLong();
				cp += 2;
				//31
				// char vit
				wp = (UINT16*)cp;
				*wp = com.Field("Character-Vitality").asLong();
				cp += 2;
				//33
				// char dex
				wp = (UINT16*)cp;
				*wp = com.Field("Character-Dexterity").asLong();
				cp += 2;
				//35
				// char int
				wp = (UINT16*)cp;
				*wp = com.Field("Character-Intelligence").asLong();
				cp += 2;
				//37
				// char mag
				wp = (UINT16*)cp;
				*wp = com.Field("Character-Magic").asLong();
				cp += 2;
				//39
				// char charisma
				wp = (UINT16*)cp;
				*wp = com.Field("Character-Charisma").asLong();
				cp += 2;
				//41
				// char appr colour
				dwp = (UINT32*)cp;
				*dwp = com.Field("Character-Appr-Colour").asLong();
				cp += 4;
				//45
				// char save year
				wp = (UINT16*)cp;
				*wp = 0;
				cp += 2;
				//47
				// char save month
				wp = (UINT16*)cp;
				*wp = 0;
				cp += 2;
				//49
				// char save day
				wp = (UINT16*)cp;
				*wp = 0;
				cp += 2;
				//51
				// char save hour
				wp = (UINT16*)cp;
				*wp = 0;
				cp += 2;
				//53
				// char save minute
				wp = (UINT16*)cp;
				*wp = 0;
				cp += 2;
				//55
				// char map
				memcpy(cp, com.Field("Character-Loc-Map").asString(), 10);
				cp += 10;
				//65
				iRows++;
				
			}
		}
		com.Close();
		*pTotal = iRows;
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

	dwp = (UINT32*)cp;
	*dwp = 11758870;
	cp += 4;
//22
	dwp = (UINT32*)cp;
	*dwp = 11758874;
	cp += 4;
//26
	for (i = 0; i < DEF_MAXCLIENTSOCK; i++)
		if ((m_pClientList[i] != 0) && (strcmp(m_pClientList[i]->m_cWorldName, m_pAccountList[iAccount]->cAccountName) == 0)) {
			if ((i == iClientH) && (strcmp(m_pClientList[iClientH]->m_cWorldName, m_pAccountList[iAccount]->cAccountName) == 0))
			{
				if (m_pAccountList[iAccount]->iClientH == iClientH)
				{
					SendEventToWLS(DEF_LOGRESMSGTYPE_CONFIRM, DEF_LOGRESMSGTYPE_CONFIRM, cData, 26 + (65 * iRows), i);
					bConnected = true;
					break;
				}
			}
		}

	if (bConnected == false)
		DeleteAccount(iAccount);
}

void CMainLog::TotalChar(int iClientH, char* pData)
{
	char* cp, * cp2, cAccountName[11], cWorldName[11], cData[2000], cTotalChar; //cp2 out going message
	UINT32* dwp, dAccountid, dMsg, dMsgType;
	int i, iAccount, iTracker, iAccountDBID;
	short iUperVersion, iLowerVersion;
	ZeroMemory(cAccountName, sizeof(cAccountName));
	ZeroMemory(cWorldName, sizeof(cWorldName)); // vamp - to check socket jump
	ZeroMemory(cData, sizeof(cData));
	iUperVersion = DEF_UPERVERSION;
	iLowerVersion = DEF_LOWERVERSION;
	cTotalChar = 0;

	if (m_pClientList[iClientH] == 0) return;

	cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2); //incomeing message

	memcpy(cAccountName, cp, 10);
	cp += 10;

	dwp = (UINT32*)cp;
	dAccountid = *dwp;
	cp += 4;

	dwp = (UINT32*)cp;
	dMsg = *dwp;
	cp += 4;

	dwp = (UINT32*)cp;
	dMsgType = *dwp;
	cp += 2;

	switch (dMsg) {

	case MSGID_RESPONSE_CHARACTERLOG:
		switch (dMsgType) {
		case DEF_MSGTYPE_CONFIRM:
			for (i = 0; i < DEF_MAXACCOUNTS; i++)
				if ((m_pAccountList[i] != 0) && (memcmp(m_pAccountList[i]->cAccountName, cAccountName, 10) == 0)) {
					iTracker = m_pAccountList[i]->iClientH;
					iAccountDBID = m_pAccountList[i]->dAccountID;

					iAccount = i;
					m_pAccountList[i]->iClientH = 0; //set iclient to null becouse this is only to be sure he didnt dc on logon
					if (m_pAccountList[i]->dAccountID != dAccountid) {
						DeleteAccount(i);
						return;
					}
					if (m_pClientList[iTracker] == 0) {
						DeleteAccount(i);
						return;
					}
					memcpy(cWorldName, m_pClientList[iTracker]->m_cWorldName, 10);
					if (memcmp(cWorldName, cAccountName, 10) != 0) {

						DeleteAccount(i);
						return;
					}
					break;
				}
			for (i = 0; i < DEF_MAXCHARACTER; i++)
				if ((m_pCharList[i] != 0) && (m_pCharList[i]->iTracker == iAccountDBID)) cTotalChar++; // iAccount

			if (iAccount < 0) return;

			cp2 = (char*)(cData); //outgoing messag

			dwp = (UINT32*)cp2;
			*dwp = iUperVersion;
			cp2 += 2;

			dwp = (UINT32*)cp2;
			*dwp = iLowerVersion;
			cp2 += 2;

			//used for account status
			cp2++;

			dwp = (UINT32*)cp2;
			*dwp = m_pAccountList[iAccount]->m_iAccntYear;
			cp2 += 2;

			dwp = (UINT32*)cp2;
			*dwp = m_pAccountList[iAccount]->m_iAccntMonth;
			cp2 += 2;

			dwp = (UINT32*)cp2;
			*dwp = m_pAccountList[iAccount]->m_iAccntDay;
			cp2 += 2;

			dwp = (UINT32*)cp2;
			*dwp = m_pAccountList[iAccount]->m_iPassYear;
			cp2 += 2;

			dwp = (UINT32*)cp2;
			*dwp = m_pAccountList[iAccount]->m_iPassMonth;
			cp2 += 2;

			dwp = (UINT32*)cp2;
			*dwp = m_pAccountList[iAccount]->m_iPassDay;
			cp2 += 2;

			*cp2 = cTotalChar;
			cp2++;

			cp = (char*)(pData + 44); //incomeing
			memcpy(cp2, cp, 8 + (65 * cTotalChar)); // get message between total*
			cp2 += 8 + (65 * cTotalChar);

			SendEventToWLS(DEF_LOGRESMSGTYPE_CONFIRM, DEF_LOGRESMSGTYPE_CONFIRM, cData, 26 + (65 * cTotalChar), iTracker);
			break;
		}
		break;
	}

}
void CMainLog::ResponseCharacter(int iClientH, char* pData, char cMode)
{
	char* cp, * cp2, cAccountName[11], cAccountPass[11], cWorldName[30], cData[2000], cTotalChar; //cp2 out going message
	UINT32* dwp, dwAccountid;
	int i, iMessage, iAccount, iTracker, iDBID = -1;
	
	ZeroMemory(cAccountName, sizeof(cAccountName));
	ZeroMemory(cAccountPass, sizeof(cAccountPass));
	ZeroMemory(cWorldName, sizeof(cWorldName));
	ZeroMemory(cData, sizeof(cData));
	cTotalChar = 0;
	iTracker = -1;
	iAccount = -1;

	if (m_pClientList[iClientH] == 0) return;

	switch (cMode) {
	case 0: //message from client to WL
		cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2 + 10); //incomeing message

		memcpy(cAccountName, cp, 10);
		cp += 10;
		memcpy(cAccountPass, cp, 10);
		cp += 10;
		memcpy(cWorldName, cp, 30);
		cp += 30;

		iMessage = GetAccountInfo(iClientH, cAccountName, cAccountPass, cWorldName, &iAccount, &iDBID);

		if (iMessage > 0) {
			SendEventToWLS(DEF_LOGRESMSGTYPE_NEWCHARACTERFAILED, DEF_LOGRESMSGTYPE_NEWCHARACTERFAILED, 0, 0, iClientH);
			return;
		}
		if (strcmp(m_pAccountList[iAccount]->cPassword, cAccountPass) != 0) {
			SendEventToWLS(DEF_LOGRESMSGTYPE_NEWCHARACTERFAILED, DEF_LOGRESMSGTYPE_NEWCHARACTERFAILED, 0, 0, iClientH);
			return;
		}

		for (i = 0; i < DEF_MAXCHARACTER; i++) {
			if ((m_pCharList[i] != 0) && (m_pCharList[i]->iTracker == iDBID)) cTotalChar++; // iAccount
		}
		
		m_pAccountList[iAccount]->dAccountID = iDBID; 

		cp2 = (char*)(cData); //outgoing messag
		cp = (char*)(pData + 6); //incomeing
		memcpy(cp2, cp, 71); // get message between total*
		cp2 += 71;

		dwp = (UINT32*)cp2;
		*dwp = m_pAccountList[iAccount]->dAccountID;
		cp2 += 4;

		*cp2 = cTotalChar;
		cp2++;

		for (i = 0; i < DEF_MAXCHARACTER; i++) {
			if ((m_pCharList[i] != 0) && (m_pCharList[i]->iTracker == iDBID)) { // iAccount
				memcpy(cp2, m_pCharList[i]->cCharacterName, 11);
				cp2 += 11;
			}
		}

		for (i = 0; i < DEF_MAXCLIENTSOCK; i++) {
			if ((m_pClientList[i] != 0) && (strcmp(m_pClientList[i]->m_cWorldName, cWorldName) == 0) && (m_pClientList[i]->m_cMode == 1)) {
				SendEventToWLS(MSGID_REQUEST_CREATENEWCHARACTER, DEF_MSGTYPE_CONFIRM, cData, 76 + (cTotalChar * 11), i);
				break;
			}
		}
		break;
	case 1: //message from WL to Client
		cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2); //incomeing message

		memcpy(cAccountName, cp, 10);
		cp += 10;

		dwp = (UINT32*)cp;
		dwAccountid = *dwp;
		cp += 4;

		for (i = 0; i < DEF_MAXACCOUNTS; i++) {
			if ((m_pAccountList[i] != 0) && (memcmp(m_pAccountList[i]->cAccountName, cAccountName, 10) == 0)) {
				iTracker = m_pAccountList[i]->iClientH;
				iAccount = i;
				m_pAccountList[i]->iClientH = 0; //set iclient to null becouse this is only to be sure he didnt dc on logon
				if (m_pAccountList[i]->dAccountID != dwAccountid) {
					SendEventToWLS(DEF_LOGRESMSGTYPE_NEWCHARACTERFAILED, DEF_LOGRESMSGTYPE_NEWCHARACTERFAILED, 0, 0, iTracker);
					DeleteAccount(i);
					return;
				}
				if (m_pClientList[iTracker] == 0) {
					DeleteAccount(i);
					return;
				}
			}
		}

		cp = (char*)(pData + 20); //incomeing
		cp2 = (char*)(cData); //outgoing messag
		memcpy(cp2, cp, 10); //new char name
		cp2 += 10;

		char* pTotal;
		
		pTotal = (char*)cp2;
		cp2++;
		//18

		int accDBID = iGetAccountDatabaseID(cAccountName);
		char iRows = 0;

		SACommand com;

		UINT16* wp;
		UINT32* dwp;

		try
		{
			com.setConnection(&con);
			com.setCommandText("SELECT [Character-ID], RTRIM([Character-Name]) AS 'Character-Name', [Character-Location], [Character-Guild-Name], [Character-Guild-GUID], [Character-Guild-Rank], [Character-Loc-Map], [Character-Loc-X], [Character-Loc-Y], [Character-HP], [Character-MP], [Character-SP], [Character-Level], [Character-Rating], [Character-Strength], [Character-Intelligence], [Character-Vitality], [Character-Dexterity], [Character-Magic], [Character-Charisma], [Character-Luck], [Character-Experience], [Character-LU-Pool], [Character-Ek-Count], [Character-Pk-Count], [Character-Reward-Gold], [Character-ID1], [Character-ID2], [Character-ID3], [Character-Sex], [Character-Skin], [Character-Hair-Style], [Character-Hair-Colour], [Character-Underwear], [Character-Hunger], [Character-Shutup-Time], [Character-Rating-Time], [Character-Force-Time], [Character-SP-Time], [Character-Admin-Level], [Character-Contribution], [Character-War-Contribution], [Character-Event-ID], [Character-Criticals], [Character-Ability-Time], [Character-Lock-Map], [Character-Lock-Time], [Character-Crusade-Job], [Character-Crusade-GUID], [Character-Construct-Points], [Character-Death-Time], [Character-Party-ID], [Character-Majestics], [Character-Appr1], [Character-Appr2], [Character-Appr3], [Character-Appr4], [Character-Appr-Colour] FROM Characters WHERE [Account-ID] = :1");
			com.Param(1).setAsLong() = accDBID;

			com.Execute();

			if (com.isResultSet())
			{
				while (com.FetchNext())
				{
					// char map
					memcpy(cp2, com.Field("Character-Name").asString(), 10);
					cp2 += 10;
					//10
					*cp2 = 1;
					cp2++;
					//11
					// char appr1
					wp = (UINT16*)cp2;
					*wp = com.Field("Character-Appr1").asLong();
					cp2 += 2;
					//13
					// char appr2
					wp = (UINT16*)cp2;
					*wp = com.Field("Character-Appr2").asLong();
					cp2 += 2;
					//15
					// char appr3
					wp = (UINT16*)cp2;
					*wp = com.Field("Character-Appr3").asLong();
					cp2 += 2;
					//17
					// char appr1
					wp = (UINT16*)cp2;
					*wp = com.Field("Character-Appr4").asLong();
					cp2 += 2;
					//19
					// char gender
					wp = (UINT16*)cp2;
					*wp = com.Field("Character-Sex").asLong();
					cp2 += 2;
					//21
					// char skin
					wp = (UINT16*)cp2;
					*wp = com.Field("Character-Skin").asLong();
					cp2 += 2;
					//23
					// char level
					wp = (UINT16*)cp2;
					*wp = com.Field("Character-Level").asLong();
					cp2 += 2;
					//25
					// char exp
					dwp = (UINT32*)cp2;
					*dwp = com.Field("Character-Experience").asLong();
					cp2 += 4;
					//29
					// char str
					wp = (UINT16*)cp2;
					*wp = com.Field("Character-Strength").asLong();
					cp2 += 2;
					//31
					// char vit
					wp = (UINT16*)cp2;
					*wp = com.Field("Character-Vitality").asLong();
					cp2 += 2;
					//33
					// char dex
					wp = (UINT16*)cp2;
					*wp = com.Field("Character-Dexterity").asLong();
					cp2 += 2;
					//35
					// char int
					wp = (UINT16*)cp2;
					*wp = com.Field("Character-Intelligence").asLong();
					cp2 += 2;
					//37
					// char mag
					wp = (UINT16*)cp2;
					*wp = com.Field("Character-Magic").asLong();
					cp2 += 2;
					//39
					// char charisma
					wp = (UINT16*)cp2;
					*wp = com.Field("Character-Charisma").asLong();
					cp2 += 2;
					//41
					// char appr colour
					dwp = (UINT32*)cp2;
					*dwp = com.Field("Character-Appr-Colour").asLong();
					cp2 += 4;
					//45
					// char save year
					wp = (UINT16*)cp2;
					*wp = 0;
					cp2 += 2;
					//47
					// char save month
					wp = (UINT16*)cp2;
					*wp = 0;
					cp2 += 2;
					//49
					// char save day
					wp = (UINT16*)cp2;
					*wp = 0;
					cp2 += 2;
					//51
					// char save hour
					wp = (UINT16*)cp2;
					*wp = 0;
					cp2 += 2;
					//53
					// char save minute
					wp = (UINT16*)cp2;
					*wp = 0;
					cp2 += 2;
					//55
					// char map
					memcpy(cp2, com.Field("Character-Loc-Map").asString(), 10);
					cp2 += 10;
					//65
					iRows++;

				}
			}
			com.Close();
			*pTotal = iRows;
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

		SendEventToWLS(DEF_LOGRESMSGTYPE_NEWCHARACTERCREATED, DEF_LOGRESMSGTYPE_NEWCHARACTERCREATED, cData, 11 + (iRows * 65), iTracker);
		break;
	}
}
void CMainLog::DeleteCharacter(int iClientH, char* pData, char cMode)
{
	char* cp, * cp2, cAccountName[11], cAccountPass[11], cWorldName[30], cData[500], cTotalChar, cCharName[11]; //cp2 out going message
	UINT32* dwp, dwAccountid;
	int i, iMessage, iAccount, iTracker, iDBID;
	
	ZeroMemory(cAccountName, sizeof(cAccountName));
	ZeroMemory(cAccountPass, sizeof(cAccountPass));
	ZeroMemory(cWorldName, sizeof(cWorldName));
	ZeroMemory(cCharName, sizeof(cCharName));
	ZeroMemory(cData, sizeof(cData));
	iAccount = -1;
	iTracker = -1;
	iDBID = -1;
	cTotalChar = 0;

	if (m_pClientList[iClientH] == 0) return;

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
		
		for (i = 0; i < DEF_MAXCHARACTER; i++)
			if ((m_pCharList[i] != 0) && (m_pCharList[i]->iTracker == iDBID)) {
				if (memcmp(m_pCharList[i]->cCharacterName, cCharName, 10) == 0) {
					delete m_pCharList[i];
					m_pCharList[i] = 0;
				}
			}
		for (i = 0; i < DEF_MAXCHARACTER; i++)
			if ((m_pCharList[i] != 0) && (m_pCharList[i]->iTracker == iDBID))	cTotalChar++;

		m_pAccountList[iAccount]->dAccountID = iDBID;

		cp2 = (char*)(cData); //outgoing messag

		memcpy(cp2, cAccountName, 10);
		cp2 += 10;

		dwp = (UINT32*)cp2;
		*dwp = m_pAccountList[iAccount]->dAccountID;
		cp2 += 4;

		memcpy(cp2, cCharName, 10);
		cp2 += 10;

		*cp2 = cTotalChar;
		cp2++;

		for (i = 0; i < DEF_MAXCHARACTER; i++)
			if ((m_pCharList[i] != 0) && (m_pCharList[i]->iTracker == iDBID)) {
				memcpy(cp2, m_pCharList[i]->cCharacterName, 11);
				cp2 += 11;
			}

		for (i = 0; i < DEF_MAXCLIENTSOCK; i++)
			if ((m_pClientList[i] != 0) && (strcmp(m_pClientList[i]->m_cWorldName, cWorldName) == 0) && (m_pClientList[i]->m_cMode == 1)) {
				SendEventToWLS(MSGID_REQUEST_DELETECHARACTER, DEF_MSGTYPE_CONFIRM, cData, 25 + (cTotalChar * 11), i);
				break;
			}
		break;
	case 1:
		cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2); //incomeing message

		memcpy(cAccountName, cp, 10);
		cp += 10;

		dwp = (UINT32*)cp;
		dwAccountid = *dwp;
		cp += 4;

		for (i = 0; i < DEF_MAXACCOUNTS; i++) {
			if ((m_pAccountList[i] != 0) && (memcmp(m_pAccountList[i]->cAccountName, cAccountName, 10) == 0)) {
				iTracker = m_pAccountList[i]->iClientH;
				iAccount = i;
				m_pAccountList[i]->iClientH = 0; //set iclient to null becouse this is only to be sure he didnt dc on logon
				if (m_pAccountList[i]->dAccountID != dwAccountid) {
					SendEventToWLS(DEF_LOGRESMSGTYPE_NOTEXISTINGCHARACTER, DEF_LOGRESMSGTYPE_NOTEXISTINGCHARACTER, 0, 0, iTracker);

					wsprintf(G_cTxt, "(DeleteCharacter) Delete Account: %i", i);
					PutLogList(G_cTxt);
					DeleteAccount(i);
					return;
				}
				if (m_pClientList[iTracker] == 0) {
					wsprintf(G_cTxt, "(DeleteCharacter) Delete Account: %i", i);
					PutLogList(G_cTxt);
					DeleteAccount(i);
					return;
				}
			}
		}

		cp2 = (char*)(cData);
		cp2++;

		char* pTotal;
		
		pTotal = (char*)cp2;
		cp2++;
		//18

		int accDBID = iGetAccountDatabaseID(cAccountName);
		char iRows = 0;

		SACommand com;

		UINT16* wp;
		UINT32* dwp;

		try
		{
			com.setConnection(&con);
			com.setCommandText("SELECT [Character-ID], RTRIM([Character-Name]) AS 'Character-Name', [Character-Location], [Character-Guild-Name], [Character-Guild-GUID], [Character-Guild-Rank], [Character-Loc-Map], [Character-Loc-X], [Character-Loc-Y], [Character-HP], [Character-MP], [Character-SP], [Character-Level], [Character-Rating], [Character-Strength], [Character-Intelligence], [Character-Vitality], [Character-Dexterity], [Character-Magic], [Character-Charisma], [Character-Luck], [Character-Experience], [Character-LU-Pool], [Character-Ek-Count], [Character-Pk-Count], [Character-Reward-Gold], [Character-ID1], [Character-ID2], [Character-ID3], [Character-Sex], [Character-Skin], [Character-Hair-Style], [Character-Hair-Colour], [Character-Underwear], [Character-Hunger], [Character-Shutup-Time], [Character-Rating-Time], [Character-Force-Time], [Character-SP-Time], [Character-Admin-Level], [Character-Contribution], [Character-War-Contribution], [Character-Event-ID], [Character-Criticals], [Character-Ability-Time], [Character-Lock-Map], [Character-Lock-Time], [Character-Crusade-Job], [Character-Crusade-GUID], [Character-Construct-Points], [Character-Death-Time], [Character-Party-ID], [Character-Majestics], [Character-Appr1], [Character-Appr2], [Character-Appr3], [Character-Appr4], [Character-Appr-Colour] FROM Characters WHERE [Account-ID] = :1");
			com.Param(1).setAsLong() = accDBID;

			com.Execute();

			if (com.isResultSet())
			{
				while (com.FetchNext())
				{
					// char map
					memcpy(cp2, com.Field("Character-Name").asString(), 10);
					cp2 += 10;
					//10
					*cp2 = 1;
					cp2++;
					//11
					// char appr1
					wp = (UINT16*)cp2;
					*wp = com.Field("Character-Appr1").asLong();
					cp2 += 2;
					//13
					// char appr2
					wp = (UINT16*)cp2;
					*wp = com.Field("Character-Appr2").asLong();
					cp2 += 2;
					//15
					// char appr3
					wp = (UINT16*)cp2;
					*wp = com.Field("Character-Appr3").asLong();
					cp2 += 2;
					//17
					// char appr1
					wp = (UINT16*)cp2;
					*wp = com.Field("Character-Appr4").asLong();
					cp2 += 2;
					//19
					// char gender
					wp = (UINT16*)cp2;
					*wp = com.Field("Character-Sex").asLong();
					cp2 += 2;
					//21
					// char skin
					wp = (UINT16*)cp2;
					*wp = com.Field("Character-Skin").asLong();
					cp2 += 2;
					//23
					// char level
					wp = (UINT16*)cp2;
					*wp = com.Field("Character-Level").asLong();
					cp2 += 2;
					//25
					// char exp
					dwp = (UINT32*)cp2;
					*dwp = com.Field("Character-Experience").asLong();
					cp2 += 4;
					//29
					// char str
					wp = (UINT16*)cp2;
					*wp = com.Field("Character-Strength").asLong();
					cp2 += 2;
					//31
					// char vit
					wp = (UINT16*)cp2;
					*wp = com.Field("Character-Vitality").asLong();
					cp2 += 2;
					//33
					// char dex
					wp = (UINT16*)cp2;
					*wp = com.Field("Character-Dexterity").asLong();
					cp2 += 2;
					//35
					// char int
					wp = (UINT16*)cp2;
					*wp = com.Field("Character-Intelligence").asLong();
					cp2 += 2;
					//37
					// char mag
					wp = (UINT16*)cp2;
					*wp = com.Field("Character-Magic").asLong();
					cp2 += 2;
					//39
					// char charisma
					wp = (UINT16*)cp2;
					*wp = com.Field("Character-Charisma").asLong();
					cp2 += 2;
					//41
					// char appr colour
					dwp = (UINT32*)cp2;
					*dwp = com.Field("Character-Appr-Colour").asLong();
					cp2 += 4;
					//45
					// char save year
					wp = (UINT16*)cp2;
					*wp = 0;
					cp2 += 2;
					//47
					// char save month
					wp = (UINT16*)cp2;
					*wp = 0;
					cp2 += 2;
					//49
					// char save day
					wp = (UINT16*)cp2;
					*wp = 0;
					cp2 += 2;
					//51
					// char save hour
					wp = (UINT16*)cp2;
					*wp = 0;
					cp2 += 2;
					//53
					// char save minute
					wp = (UINT16*)cp2;
					*wp = 0;
					cp2 += 2;
					//55
					// char map
					memcpy(cp2, com.Field("Character-Loc-Map").asString(), 10);
					cp2 += 10;
					//65
					iRows++;

				}
			}
			com.Close();
			*pTotal = iRows;
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

		SendEventToWLS(DEF_LOGRESMSGTYPE_CHARACTERDELETED, DEF_LOGRESMSGTYPE_CHARACTERDELETED, cData, 2 + (iRows * 65), iTracker);

		break;
	}
}

void CMainLog::ChangePassword(int iClientH, char* pData)
{
	
	char* cp, cAccNewPass[11], cAccName[11], cAccPass[11];
	
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

	SACommand com, com2;

	try
	{
		com2.setConnection(&con);
		com2.setCommandText("UPDATE Accounts SET [Account-Password] = :3 WHERE [Account-Name] = :1 AND [Account-Password] = :2");
		com2.Param(1).setAsString() = cAccName;
		com2.Param(2).setAsString() = cAccPass;
		com2.Param(3).setAsString() = cAccNewPass;

		com2.Execute();

		if (com2.RowsAffected() > 0)
			SendEventToWLS(DEF_LOGRESMSGTYPE_PASSWORDCHANGESUCCESS, DEF_LOGRESMSGTYPE_PASSWORDCHANGESUCCESS, 0, 0, iClientH);
		else
			SendEventToWLS(DEF_LOGRESMSGTYPE_PASSWORDCHANGEFAIL, DEF_LOGRESMSGTYPE_PASSWORDCHANGEFAIL, 0, 0, iClientH);
		
		com2.Close();
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
	}
}
void CMainLog::RequestEnterGame(int iClientH, char* pData, char cMode)
{
	char* cp, * cp2, cData[100], cData2[100], cAccountName[11], cAccountPass[11], cWorldName[30], m_cMapName[31], cCharName[15], G_cCmdLineTokenA[120];
	int  i, x, m_iLevel, iMessage, iAccount, iGameServer, iDBID = -1, iInUse;
	UINT32* dwp;
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

	if (m_pClientList[iClientH] == 0) return;

	switch (cMode) {

	case 0:
		cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);

		memcpy(cCharName, cp, 10);
		cp += 10;

		memcpy(m_cMapName, cp, 10);
		cp += 10;

		memcpy(cAccountName, cp, 10);
		cp += 10;

		memcpy(cAccountPass, cp, 10);
		cp += 10;

		dwp = (UINT32*)cp;
		m_iLevel = *dwp;
		cp += 4;

		memcpy(cWorldName, cp, 30);
		cp += 30;

		memcpy(cp, G_cCmdLineTokenA, 120);
		cp += 120;

		for (i = 0; i < DEF_MAXACCOUNTS; i++)
			if ((m_pAccountList[i] != 0) && (memcmp(m_pAccountList[i]->cAccountName, cAccountName, 10) == 0))
			{
				bAccountUse = true;
				iInUse = i;
			}
		if (bAccountUse == true) {
			
			DeleteAccount(iInUse);
			
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
		if (iMessage > 0) {
			cp2 = (char*)(cData); //outgoing messag - to WLserv
			*cp2 = 4;
			SendEventToWLS(DEF_ENTERGAMERESTYPE_REJECT, DEF_ENTERGAMERESTYPE_REJECT, cData, 1, iClientH);
			return;
		}
		m_pAccountList[iAccount]->dAccountID = iDBID;
		if (strcmp(m_pAccountList[iAccount]->cPassword, cAccountPass) != 0) {
			cp2 = (char*)(cData); //outgoing messag - to WLserver
			*cp2 = 4;
			SendEventToWLS(DEF_ENTERGAMERESTYPE_REJECT, DEF_ENTERGAMERESTYPE_REJECT, cData, 1, iClientH);
			return;
		}
		for (i = 0; i < DEF_MAXGAME; i++)
			if ((m_pGameList[i] != 0) && (strcmp(m_pGameList[i]->m_cWorldName, cWorldName) == 0)) {
				for (x = 0; x < DEF_MAXMAPS; x++)
					if ((m_pMapList[x] != 0) && (m_pMapList[x]->iIndex == i) && (strcmp(m_pMapList[x]->m_cMapName, m_cMapName) == 0)) {
						iGameServer = i;
						bMapOnline = true;
					}
			}
		if (bMapOnline == false) {
			cp2 = (char*)(cData); //outgoing messag - to WLserver
			*cp2 = 3;
			SendEventToWLS(DEF_ENTERGAMERESTYPE_REJECT, DEF_ENTERGAMERESTYPE_REJECT, cData, 1, iClientH);
			return;
		}

		cp2 = (char*)(cData); //outgoing messag - to WLserver

		memcpy(cp2, cAccountName, 10);
		cp2 += 10;

		memcpy(cp2, cAccountPass, 10);
		cp2 += 10;

		dwp = (UINT32*)cp2;
		*dwp = m_iLevel;
		cp2 += 4;

		if (m_pAccountList[iAccount] == 0) return;
		m_pAccountList[iAccount]->dAccountID = iDBID;
		m_iValidAccounts++;
		for (i = 0; i < DEF_MAXCLIENTSOCK; i++)
			if ((m_pClientList[i] != 0) && (strcmp(m_pClientList[i]->m_cWorldName, cWorldName) == 0) && (m_pClientList[i]->m_cMode == 1)) {
				SendEventToWLS(MSGID_REQUEST_SETACCOUNTINITSTATUS, DEF_MSGTYPE_CONFIRM, cData, 24, i);
				break;
			}
		cp2 = (char*)(cData2); //outgoing messag - to Client

		memcpy(cp2, m_pGameList[iGameServer]->m_cGameServerAddress, 16);
		cp2 += 16;

		dwp = (UINT32*)cp2;
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


int CMainLog::GetAccountInfo(int iClientH, char cAccountName[11], char cAccountPass[11], char cWorldName[30], int* iAccount, int* iDBID, char cMode)
{
	
	int i, iAccountid, iAccountDBid;
	
	bool bWorld;
	bWorld = false;
	
	iAccountid = 0;

	
	if (strlen(cAccountName) == 0) return 1; // if account blank
	if (strlen(cAccountPass) == 0) return 2; // if password blank
	if ((cMode == 0) && (strlen(cWorldName) == 0)) return 3; // if world dosnt exist

	if (m_pClientList[iClientH] == 0) return 3;

	if (cMode == 0) {
		for (i = 0; i < DEF_MAXCLIENTSOCK; i++)
			if ((m_pClientList[i] != 0) && (strcmp(m_pClientList[i]->m_cWorldName, cWorldName) == 0) && (m_pClientList[i]->m_cMode == 1)) bWorld = true; // World exist
		if (bWorld == false) return 3;
	}

	SACommand com;
	SACommand com2;

	
	try
	{
		com.setConnection(&con);
		com.setCommandText("SELECT [Account-ID], RTRIM([Account-Name]) AS 'Account-Name', RTRIM([Account-Password]) AS 'Account-Password', [Account-Valid-Time], [Account-Valid-Year], [Account-Valid-Month], [Account-Valid-Day], [Account-Change-Password], [Account-Gender], [Account-Age], [Account-Birth-Year], [Account-Birth-Month], [Account-Birth-Day], [Account-Real-Name], [Account-SSN], [Account-Email], [Account-Quiz], [Account-Answer], [Account-Created-By], [Account-Created-Date], [Account-Locked] FROM Accounts WHERE [Account-Name] = :1 AND [Account-Password] = :2");

		com.Param(1).setAsString() = cAccountName;
		com.Param(2).setAsString() = cAccountPass;

		com.Execute();

		if (com.RowsAffected() == 0)
		{
			com.Close();
			return 1;
		}

		if (com.isResultSet())
		{
			char cTemp[11];
			ZeroMemory(cTemp, sizeof(cTemp));

			while (com.FetchNext())
			{
				// db account ID
				iAccountDBid = com.Field("Account-ID").asLong();

				*iDBID = iAccountDBid;

				// account name
				memcpy(cTemp, com.Field("Account-Name").asString(), 10);

				for (i = 0; i < DEF_MAXACCOUNTS; i++) {
					if (m_pAccountList[i] == 0) {
						m_pAccountList[i] = new class CAccount(cTemp, cWorldName, iClientH);
						m_pClientList[iClientH]->m_cMode = 2;
						strcpy(m_pClientList[iClientH]->m_cWorldName, cTemp); // save account name
						iAccountid = i;
						*iAccount = i;
						break;
					}
				}

				if (m_pClientList[iClientH] == 0)
				{
					DeleteAccount(iAccountid);
					com.Close();
					return 3;
				}

				m_pAccountList[iAccountid]->dAccountID = iAccountDBid;

				// account password
				memcpy(m_pAccountList[iAccountid]->cPassword, com.Field("Account-Password").asString(), 10);

				// account valid time
				m_pAccountList[iAccountid]->iAccountValid = com.Field("Account-Valid-Time").asLong();

				// account valid year
				m_pAccountList[iAccountid]->m_iAccntYear = com.Field("Account-Valid-Year").asLong();

				// account valid month
				m_pAccountList[iAccountid]->m_iAccntMonth = com.Field("Account-Valid-Month").asLong();

				// account valid day
				m_pAccountList[iAccountid]->m_iAccntDay = com.Field("Account-Valid-Day").asLong();

				// password change year
				m_pAccountList[iAccountid]->m_iPassYear = 0;

				// password change month
				m_pAccountList[iAccountid]->m_iPassMonth = 0;

				// account valid day
				m_pAccountList[iAccountid]->m_iPassDay = 0;

				// find characters associated with this account
				com2.setConnection(&con);
				com2.setCommandText("SELECT RTRIM([Character-Name]) AS 'Character-Name' FROM Characters WHERE [Account-ID] = :1");
				com2.Param(1).setAsLong() = iAccountDBid;

				com2.Execute();

				if (com2.isResultSet())
				{
					while (com2.FetchNext())
					{
						ZeroMemory(cTemp, sizeof(cTemp));
						memcpy(cTemp, com2.Field("Character-Name").asString(), 10);

						int j;

						for (j = 0; j < DEF_MAXCHARACTER; j++)
						{
							if (m_pCharList[j] == 0) {
								m_pCharList[j] = new class CCharacter(cTemp, iAccountDBid);
								break;
							}
						}
					}
					com2.Close();
				}
			}
			com.Close();
			
		}
		else
		{
			com.Close();
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

void CMainLog::PutPacketLogData(UINT32 dwMsgID, char* cData, UINT32 dwMsgSize)
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
	if (pFile != 0) fclose(pFile);
}

void CMainLog::CleanupLogFiles()
{
}
bool CMainLog::bReadServerConfigFile(char* cFn)
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
		delete[] cp;
		fclose(pFile);
	}

	return true;
}

void CMainLog::DeleteAccount(int iClientH, char cAccountName[11])
{
	int i, x;

	//useing account name
	if (iClientH == -1) {
		if (strlen(cAccountName) <= 0) return;
		for (i = 0; i < DEF_MAXACCOUNTS; i++) {
			if ((m_pAccountList[i] != 0) && (strcmp(m_pAccountList[i]->cAccountName, cAccountName) == 0)) {
				for (x = 0; x < DEF_MAXCHARACTER; x++) {
					if ((m_pCharList[x] != 0) && (m_pCharList[x]->iTracker == i)) {
						delete m_pCharList[x];
						m_pCharList[x] = 0;
					}
				}
				if (m_pClientList[m_pAccountList[i]->iClientH] != 0) {
					delete m_pClientList[m_pAccountList[i]->iClientH];
					m_pClientList[m_pAccountList[i]->iClientH] = 0;
				}
				delete m_pAccountList[i];
				m_pAccountList[i] = 0;
			}
		}
	}
	//ussing account id
	if (m_pAccountList[iClientH] == 0) return;
	for (x = 0; x < DEF_MAXCHARACTER; x++) {
		if ((m_pCharList[x] != 0) && (m_pCharList[x]->iTracker == iClientH)) {
			delete m_pCharList[x];
			m_pCharList[x] = 0;
		}
	}
	if (m_pClientList[m_pAccountList[iClientH]->iClientH] != 0) {
		delete m_pClientList[m_pAccountList[iClientH]->iClientH];
		m_pClientList[m_pAccountList[iClientH]->iClientH] = 0;
	}
	delete m_pAccountList[iClientH];
	m_pAccountList[iClientH] = 0;

}

void CMainLog::CreateAccount(int iClientH, char* pData)
{
	char* cp;
	char m_cAccountName[11], m_cAccountPassword[11], m_cEmailAddr[51];
	char m_cAccountQuiz[46];
	char m_cAccountAnswe[21];
	char cBuffer[56];

	ZeroMemory(m_cAccountName, sizeof(m_cAccountName));
	ZeroMemory(m_cAccountPassword, sizeof(m_cAccountPassword));
	ZeroMemory(m_cEmailAddr, sizeof(m_cEmailAddr));
	ZeroMemory(m_cAccountQuiz, sizeof(m_cAccountQuiz));
	ZeroMemory(m_cAccountAnswe, sizeof(m_cAccountAnswe));
	ZeroMemory(cBuffer, sizeof(cBuffer));

	cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);

	memcpy(m_cAccountName, cp, 10);
	cp += 10;

	memcpy(m_cAccountPassword, cp, 10);
	cp += 10;

	memcpy(m_cEmailAddr, cp, 50);
	cp += 50;

	cp += 10;

	cp += 10;

	cp += 4;

	cp += 2;

	cp += 2;

	cp += 17;

	cp += 28;

	memcpy(m_cAccountQuiz, cp, 45);
	cp += 45;

	memcpy(m_cAccountAnswe, cp, 20);
	cp += 20;

	cp += 50;


	bool exists = bCheckAccountExists(m_cAccountName);

	if (exists)
	{
		wsprintf(cBuffer, "(!) Cannot Duplicate Account: %s", m_cAccountName);
		PutLogList(cBuffer);
		if (m_pClientList[iClientH] != 0) {
			SendEventToWLS(DEF_LOGRESMSGTYPE_ALREADYEXISTINGACCOUNT, DEF_LOGRESMSGTYPE_ALREADYEXISTINGACCOUNT, 0, 0, iClientH);
		}
		return;
	}
	else
	{
		SACommand com;

		try
		{
			com.setConnection(&con);
			com.setCommandText("INSERT INTO Accounts([Account-Name], [Account-Password], [Account-Email], [Account-Quiz], [Account-Answer]) VALUES(:1, :2, :3, :4, :5)");
			com.Param(1).setAsString() = m_cAccountName;
			com.Param(2).setAsString() = m_cAccountPassword;
			com.Param(3).setAsString() = m_cEmailAddr;
			com.Param(4).setAsString() = m_cAccountQuiz;
			com.Param(5).setAsString() = m_cAccountAnswe;

			com.Execute();

			wsprintf(cBuffer, "(!) Account Created: %s", m_cAccountName);
			PutLogList(cBuffer);

			com.Close();

			if (m_pClientList[iClientH] != 0)
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

			if (m_pClientList[iClientH] != 0)
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
		com.setCommandText("SELECT COUNT(*) FROM Accounts WHERE [Account-Name] = :1");
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

		
	}
	return false;
}