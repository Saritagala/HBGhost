﻿// Game.cpp: implementation of the CGame class.
//
//////////////////////////////////////////////////////////////////////

#include "Game.h"
#include "Team.h"

class CDebugWindow *DbgWnd;
class Crusade* CSade;
class Team* c_team;

extern void PutLogList(char * cMsg);
extern char G_cTxt[512];
extern char	G_cData50000[50000];
extern void PutLogFileList(char * cStr);
extern void PutAdminLogFileList(char * cStr);
extern void PutItemLogFileList(char * cStr);
extern void PutLogEventFileList(char * cStr);
extern void PutHackLogFileList(char * cStr);
extern void PutPvPLogFileList(char * cStr);
extern FILE * pLogFile;
extern HWND	G_hWnd;

#pragma warning (disable : 4996 4018)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern bool	G_bIsThread;
extern unsigned __stdcall ThreadProc(void *ch);

char _tmp_cTmpDirX[9] =
{
	0, 0, 1, 1, 1, 0, -1, -1, -1
};

char _tmp_cTmpDirY[9] =
{
	0, -1, -1, 0, 1, 1, 1, 0, -1
};

char _tmp_cEmptyPosX[25] = { 0, 1, 1, 0, -1, -1, -1, 0, 1, 2, 2, 2, 2, 1, 0, -1, -2, -2, -2, -2, -2, -1, 0, 1, 2 };
char _tmp_cEmptyPosY[25] = { 0, 0, 1, 1, 1, 0, -1, -1, -1, -1, 0, 1, 2, 2, 2, 2, 2, 1, 0, -1, -2, -2, -2, -2, -2 };

int ITEMSPREAD_FIEXD_COORD[25][2] =
{
	{ 0, 0 },	//1
	{ 1, 0 },	//2
	{ 1, 1 },	//3
	{ 0, 1 },	//4
	{ -1, 1 },	//5
	{ -1, 0 },	//6
	{ -1, -1 },	//7
	{ 0, -1 },	//8
	{ 1, -1 },	//9
	{ 2, -1 },	//10
	{ 2, 0 },	//11
	{ 2, 1 },	//12
	{ 2, 2 },	//13
	{ 1, 2 },	//14
	{ 0, 2 },	//15
	{ -1, 2 },	//16
	{ -2, 2 },	//17
	{ -2, 1 },	//18
	{ -2, 0 },	//19
	{ -2, -1 },	//20
	{ -2, -2 },	//21
	{ -1, -2 },	//22
	{ 0, -2 },	//23
	{ 1, -2 },	//24
	{ 2, -2 },	//25
};

/*********************************************************************************************************************
**  CGame::CGame(HWND hWnd)																							**
**  DESCRIPTION			:: general variable initializer																**
**  LAST_UPDATED		:: March 13, 2005; 9:27 AM; Hypnotoad														**
**	RETURN_VALUE		:: none																						**
**  NOTES				::	- m_bIsWLServerAvailable is new															**
**							- m_cServerName changed from m_cServerName[11] to m_cServerName[12]						**
**							- set m_sForceRecallTime to 0															**
**							- /// Highlighted 1 /// is incorrect													**
**							- missing <17> variables																**
**							- <6> unidentified variables															**
**	MODIFICATION		::	- Added external variable initialization to:											**
**							m_bEnemyKillMode, m_iEnemyKillAdjust													**
**							m_bAdminSecurity m_sRaidTimeMonday m_sRaidTimeTuesday m_sRaidTimeWednesday				**
**							m_sRaidTimeThursday m_sRaidTimeFriday m_sRaidTimeSaturday m_sRaidTimeSunday				**
**							m_sCharPointLimit m_sSlateSuccessRate													**
**********************************************************************************************************************/
CGame::CGame(HWND hWnd)
{
	int i, x;
	c_team = new Team;
	m_bSQLMode = false;
	m_bAntiHackMode = false;
	m_bBlockLocalConn = false;
	m_bIsGameServerRegistered = false;
	ReceivedAllConfig		= false;
	m_bIsGameStarted = false;
	m_hWnd           = hWnd;
	m_pMainLogSock   = 0;
	m_pGateSock      = 0;
	m_bIsLogSockAvailable   = false;
	m_bIsGateSockAvailable  = false;
	m_bIsItemAvailable      = false;
	m_bIsBuildItemAvailable = false;
	m_bIsNpcAvailable       = false;
	m_bIsMagicAvailable     = false;
	m_bIsSkillAvailable     = false;
	m_bIsQuestAvailable     = false;
	m_bIsPotionAvailable    = false;
	m_bIsWLServerAvailable  = false; // new
	ZeroMemory(m_cServerName, sizeof(m_cServerName)); // changed to 12
	m_sForceRecallTime = 0; // new
	for (i = 0; i < DEF_MAXCLIENTS; i++) m_pClientList[i] = 0;
	for (i = 0; i < DEF_MAXMAPS; i++) m_pMapList[i] = 0;
	for (i = 0; i < DEF_MAXITEMTYPES; i++) m_pItemConfigList[i] = 0;
	for (i = 0; i < DEF_MAXNPCTYPES; i++) m_pNpcConfigList[i] = 0;
	for (i = 0; i < DEF_MAXNPCS; i++) m_pNpcList[i] = 0;
	for (i = 0; i < DEF_MSGQUENESIZE; i++) m_pMsgQuene[i] = 0;
	for (i = 0; i < DEF_MAXMAGICTYPE; i++) m_pMagicConfigList[i] = 0;
	for (i = 0; i < DEF_MAXSKILLTYPE; i++) m_pSkillConfigList[i] = 0;
	for (i = 0; i < DEF_MAXQUESTTYPE; i++) m_pQuestConfigList[i] = 0;
	for (i = 0; i < DEF_MAXDYNAMICOBJECTS; i++) m_pDynamicObjectList[i] = 0;
	for (i = 0; i < DEF_MAXDELAYEVENTS; i++) m_pDelayEventList[i] = 0;
	for (i = 0; i < DEF_MAXNOTIFYMSGS; i++) m_pNoticeMsgList[i] = 0;
	for (i = 0; i < DEF_MAXFISHS; i++) m_pFish[i] = 0;
	for (i = 0; i < DEF_MAXMINERALS; i++) m_pMineral[i] = 0;
	for (i = 0; i < DEF_MAXPOTIONTYPES; i++) {
		m_pPotionConfigList[i] = 0;
		m_pCraftingConfigList[i] = 0;// Crafting
	}
	for (i = 0; i < DEF_MAXSUBLOGSOCK; i++) {
		m_pSubLogSock[i] = 0;
		m_bIsSubLogSockAvailable[i] = false;

	}
	for (i = 0; i < DEF_MAXBUILDITEMS; i++) m_pBuildItemList[i] = 0;
	for (i = 0; i < DEF_MAXDUPITEMID; i++) m_pDupItemIDList[i] = 0;
	//// Highlighted 1 ///
	for (i = 0; i < DEF_MAXCLIENTS; i++) { // asm is 5000, this is 2000 ?
		m_stPartyInfo[i].iTotalMembers = 0;
		for(x = 0; x < DEF_MAXPARTYMEMBERS; x++) 
			m_stPartyInfo[i].iIndex[x] = 0;
	}

	// Centuu
	for (i = 0; i < DEF_MAXGUILDS; i++) {
		m_stGuildInfo[i].markX = -1;
		m_stGuildInfo[i].markY = -1;
		strcpy(m_stGuildInfo[i].cGuildName, "NONE");
		m_stGuildInfo[i].dwMarkTime = 0;
		ZeroMemory(m_stGuildInfo[i].cMap, sizeof(m_stGuildInfo[i].cMap));
	}

	for (i = 0; i < DEF_MAXGUILDS; i++) {
		m_stSummonGuild[i].sX = -1;
		m_stSummonGuild[i].sY = -1;
		strcpy(m_stSummonGuild[i].cGuildName, "NONE");
		ZeroMemory(m_stSummonGuild[i].cMap, sizeof(m_stSummonGuild[i].cMap));
	}

	for (i = 0; i < DEF_MAXGUILDS; i++) {
		m_stGuild[i].iGuildLevel = 0;
		strcpy(m_stGuild[i].cGuildName, "NONE");
		for (x = 0; x < DEF_MAXBANKITEMS; x++) {
			m_stGuild[i].m_pItemInBankList[x] = 0;
		}
	}

	//// End Highlight ///
	m_iQueneHead = 0;
	m_iQueneTail = 0;
	m_iTotalClients = 0;
	m_iMaxClients   = 0;
	m_iTotalMaps    = 0;
	m_iTotalGameServerClients    = 0;
	m_iTotalGameServerMaxClients = 0;
	m_MaxUserSysTime.wHour   = 0;
	m_MaxUserSysTime.wMinute = 0;
	m_bIsServerShutdowned = false;
	m_cShutDownCode = 0;
	m_iMiddlelandMapIndex = -1;
	m_iBTFieldMapIndex = -1;
	m_iGodHMapIndex = -1;
	m_iRampartMapIndex = -1;
	m_iAresdenOccupyTiles = 0;
	m_iElvineOccupyTiles  = 0;
	m_iCurMsgs = 0;
	m_iMaxMsgs = 0;
	m_pNoticementData = 0;

	m_sJailTime = 0;

	srvHour = 0; 
	srvMinute = 0;
	
	m_iAutoRebootingCount	= 0;
	m_bReceivedItemList = false;
	///////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////Modifications////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////
	m_bEnemyKillMode		= false; 
	m_iEnemyKillAdjust      = 1; 
	m_bAdminSecurity = false;
	m_sRaidTimeMonday		= 0; 
	m_sRaidTimeTuesday      = 0; 
	m_sRaidTimeWednesday	= 0; 
	m_sRaidTimeThursday     = 0; 
	m_sRaidTimeFriday		= 0; 
	m_sRaidTimeSaturday     = 0; 
	m_sRaidTimeSunday		= 0;
	m_sCharPointLimit		= 0;
	m_sSlateSuccessRate		= 0;
	///////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////
	m_bNpcItemConfig = true;
	m_iNotifyCleanMap = true;

	m_iExpSetting = 0;
	m_iBuildDate = 0;
	m_iClearMapTime = 0;
	m_iGoldRate = 0;
	m_iGuildCost = 0;
	m_iDKCost = 0;

	m_sHeldenianFinish = m_sCrusadeFinish = m_sApocalypseFinish = m_sDeathmatchFinish = m_sCTF_Finish =
		m_sDropFinish = m_sCityTeleportFinish = m_sShinningFinish = m_sCandyFinish = m_sTeamArenaFinish =
		m_sRevelationFinish = 0;

	for (i = 1; i <= 15; i++)
	{
		aHBTopKills[i] = 0;
		ZeroMemory(aHBTopClientH[i], sizeof(aHBTopClientH[i]));
	}
}

CGame::~CGame()
{
}

/*********************************************************************************************************************
**  bool CGame::bAccept																								**
**  DESCRIPTION			:: function following player connect														**
**  LAST_UPDATED		:: March 13, 2005; 9:34 AM; Hypnotoad														**
**	RETURN_VALUE		:: bool																						**
**  NOTES				::	- added (m_bIsWLServerAvailable == false)												**
**							- modified "<%d> New client: %s" to "<%d> Client connected: (%s)"						**
**							- added "Maximum Players: %d" code														**
**  MODIFICATION		::	- added print message if CLOSE_ANYWAY													**
**********************************************************************************************************************/
bool CGame::bAccept(class XSocket* pXSock)
{
	int i, iTotalip = 0, a;
	class XSocket* pTmpSock;
	char cIPtoBan[21];
	FILE* pFile;

	if ((m_bIsGateSockAvailable == false) || (m_bIsLogSockAvailable == false) ||
		(m_bIsItemAvailable == false) || (m_bIsNpcAvailable == false) ||
		(m_bIsMagicAvailable == false) || (m_bIsSkillAvailable == false) ||
		(m_bIsPotionAvailable == false) || (m_bOnExitProcess == true) ||
		(m_bIsQuestAvailable == false) || (m_bIsBuildItemAvailable == false) ||
		(m_bIsGameStarted == false))
		goto CLOSE_ANYWAY;
	for (i = 1; i < DEF_MAXCLIENTS; i++) {
		if (m_pClientList[i] == 0) {
			m_pClientList[i] = new class CClient(m_hWnd);
			bAddClientShortCut(i);
			m_pClientList[i]->m_dwSPTime = m_pClientList[i]->m_dwMPTime = m_pClientList[i]->m_dwHPTime =
				m_pClientList[i]->m_dwAutoSaveTime = m_pClientList[i]->m_dwTime = m_pClientList[i]->m_dwHungerTime =
				m_pClientList[i]->m_dwExpStockTime = m_pClientList[i]->m_dwRecentAttackTime =
				m_pClientList[i]->m_dwAutoExpTime = m_pClientList[i]->m_dwSpeedHackCheckTime = timeGetTime();
			pXSock->bAccept(m_pClientList[i]->m_pXSock, WM_ONCLIENTSOCKETEVENT + i);
			ZeroMemory(m_pClientList[i]->m_cIPaddress, sizeof(m_pClientList[i]->m_cIPaddress));
			m_pClientList[i]->m_pXSock->iGetPeerAddress(m_pClientList[i]->m_cIPaddress);

			if (m_bAntiHackMode) {
				a = i;

				if (m_bBlockLocalConn) {
					if (strcmp(m_cGameServerAddr, m_pClientList[i]->m_cIPaddress) == 0)
					{
						goto CLOSE_CONN;
					}
				}

				for (int v = 0; v < DEF_MAXBANNED; v++)
				{
					if (strcmp(m_stBannedList[v].m_cBannedIPaddress, m_pClientList[i]->m_cIPaddress) == 0)
					{
						goto CLOSE_CONN;
					}
				}
				//centu: Anti-Downer
				for (int j = 1; j < DEF_MAXCLIENTS; j++) {
					if (m_pClientList[j] != 0) {
						if (strcmp(m_pClientList[j]->m_cIPaddress, m_pClientList[i]->m_cIPaddress) == 0) iTotalip++;
					}
				}
				if (iTotalip > 9) {
					ZeroMemory(cIPtoBan, sizeof(cIPtoBan));
					strcpy(cIPtoBan, m_pClientList[i]->m_cIPaddress);
					//opens cfg file
					pFile = fopen("GameConfigs\\BannedList.cfg", "a");
					//shows log
					wsprintf(G_cTxt, "<%d> IP Banned: (%s)", i, cIPtoBan);
					PutLogList(G_cTxt);
					//modifys cfg file
					fprintf(pFile, "banned-ip = %s", cIPtoBan);
					fprintf(pFile, "\n");
					fclose(pFile);

					for (int x = 0; x < DEF_MAXBANNED; x++)
						if (strlen(m_stBannedList[x].m_cBannedIPaddress) == 0)
							strcpy(m_stBannedList[x].m_cBannedIPaddress, cIPtoBan);

					goto CLOSE_CONN;
				}
			}
			wsprintf(G_cTxt, "<%d> Client connected: (%s)", i, m_pClientList[i]->m_cIPaddress);
			PutLogList(G_cTxt);
			m_iTotalClients++;
			if (m_iTotalClients > m_iMaxClients) {
				m_iMaxClients = m_iTotalClients;

			}
			return true;
		}
	}
CLOSE_ANYWAY:;
	/****MODIFICATION****/
	PutLogList("(!) bAccept->CLOSE_ANYWAY");
	/********END*********/
	pTmpSock = new class XSocket(m_hWnd, DEF_SERVERSOCKETBLOCKLIMIT);
	pXSock->bAccept(pTmpSock, 0);
	delete pTmpSock;
	return false;
CLOSE_CONN:;
	delete m_pClientList[a];
	m_pClientList[a] = 0;
	RemoveClientShortCut(a);
	return false;
}

/*********************************************************************************************************************
**  void CGame::OnClientSocketEvent																					**
**  DESCRIPTION			:: socket connection manager																**
**  LAST_UPDATED		:: March 13, 2005; 9:58 AM; Hypnotoad														**
**	RETURN_VALUE		:: void																						**
**  NOTES				::	- removed DEF_XSOCKEVENT_BLOCK															**
**							- modified DeleteClient																	**
**	MODIFICATION		::	- Added Logout Hack Check code															**
**********************************************************************************************************************/
void CGame::OnClientSocketEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
 UINT iTmp;
 int iClientH, iRet;
 UINT32 dwTime = timeGetTime();

	iTmp = WM_ONCLIENTSOCKETEVENT;
	iClientH = (int)(message - iTmp);
// SNOOPY: Trying to trap 004030B6 error
// 004030D0  |> 8B4D F4        MOV ECX,UINT32 PTR SS:[EBP-C]
// 004030D3  |. 8B55 FC        MOV EDX,UINT32 PTR SS:[EBP-4]
// 004030D6  |. 837C8A 7C 00   CMP UINT32 PTR DS:[EDX+ECX*4+7C],0
// 004030DB  |. 75 05          JNZ SHORT HGserver.004030E2
// 004030DD  |. E9 AB020000    JMP HGserver.0040338D
// 004030E2  |> 8B45 10        MOV EAX,UINT32 PTR SS:[EBP+10]
	if (iClientH <= 0) return;	

	if (m_pClientList[iClientH] == 0) return;
	iRet = m_pClientList[iClientH]->m_pXSock->iOnSocketEvent(wParam, lParam);
	switch (iRet) {
	case DEF_XSOCKEVENT_READCOMPLETE:
		OnClientRead(iClientH);
		m_pClientList[iClientH]->m_dwTime = timeGetTime();
		break;
	
	case DEF_XSOCKEVENT_CONFIRMCODENOTMATCH:
		wsprintf(G_cTxt,"<%d> Confirmcode notmatch!", iClientH);
		PutLogList(G_cTxt);
		DeleteClient(iClientH, false, true, true, false);
		break;
	case DEF_XSOCKEVENT_MSGSIZETOOLARGE:
	case DEF_XSOCKEVENT_SOCKETERROR:
	case DEF_XSOCKEVENT_SOCKETCLOSED:
		wsprintf(G_cTxt,"<%d> Client connection Lost! (%s)", iClientH, m_pClientList[iClientH]->m_cIPaddress);
		PutLogList(G_cTxt); 
		
		DeleteClient(iClientH, true, true, true, false);
		break;
	}													    
}

/*********************************************************************************************************************
**  bool CGame::bInit()		   																						**
**  DESCRIPTION			:: general variable initializer																**
**  LAST_UPDATED		:: March 13, 2005; 8:00 PM; Hypnotoad														**
**	RETURN_VALUE		:: bool																						**
**  NOTES				::	- Added Nulliy to "m_pTeleportDestination[i]"											**
**							- reduced MaxLevel from 3500 to 300														**
**							- changed DEF_MAXGATESERVERSTOCKMSGSIZE from 10000 to 30000								**
**							- added m_bIsApocalypseStarter, m_bIsApocalypseGateOpen, m_cApocalypseMapName,			**
**							  m_bHeldinianDuty, m_bF10pressed, m_cHeldenianMapName, m_bIsHeldenianReady				**
**							- missing <13> variables																**
**							- <6> variables unidentified															**
**	MODIFICATION		::	- Added read to Settings.cfg, AdminList.cfg, BannedList.cfg, AdminSettings.cfg			**
**							- Added Admin and Banned List initialization											**
**							- Added initialization to Apocalypse end/start and Heldenian Timer						**
**********************************************************************************************************************/
bool CGame::bInit()		   
{
 char * cp, cTxt[120];
 UINT32 * dwp;
 UINT16 * wp;
 int  i;
 SYSTEMTIME SysTime;
 UINT32 dwTime = timeGetTime();

	PutLogList("(!) INITIALIZING GAME SERVER...");
	for (i = 0; i < DEF_MAXCLIENTS+1; i++) 
		m_iClientShortCut[i] = 0;
	if (m_pMainLogSock != 0) delete m_pMainLogSock;
	if (m_pGateSock != 0) delete m_pGateSock;
	for (i = 0; i < DEF_MAXSUBLOGSOCK; i++)
		if (m_pSubLogSock[i] != 0) delete m_pSubLogSock[i];
	for (i = 0; i < DEF_MAXCLIENTS; i++)
		if (m_pClientList[i] != 0) delete m_pClientList[i];
	for (i = 0; i < DEF_MAXNPCS; i++)
		if (m_pNpcList[i] != 0) delete m_pNpcList[i];
	for (i = 0; i < DEF_MAXMAPS; i++)
		if (m_pMapList[i] != 0) delete m_pMapList[i];
	for (i = 0; i < DEF_MAXITEMTYPES; i++)
		if (m_pItemConfigList[i] != 0) delete m_pItemConfigList[i];
	for (i = 0; i < DEF_MAXNPCTYPES; i++)
		if (m_pNpcConfigList[i] != 0) delete m_pNpcConfigList[i];
	for (i = 0; i < DEF_MAXMAGICTYPE; i++)
		if (m_pMagicConfigList[i] != 0) delete m_pMagicConfigList[i];
	for (i = 0; i < DEF_MAXSKILLTYPE; i++)
		if (m_pSkillConfigList[i] != 0) delete m_pSkillConfigList[i];
	for (i = 0; i < DEF_MAXQUESTTYPE; i++)
		if (m_pQuestConfigList[i] != 0) delete m_pQuestConfigList[i];
	for (i = 0; i < DEF_MAXDYNAMICOBJECTS; i++)
		if (m_pDynamicObjectList[i] != 0) delete m_pDynamicObjectList[i];
	for (i = 0; i < DEF_MAXDELAYEVENTS; i++)
		if (m_pDelayEventList[i] != 0) delete m_pDelayEventList[i];
	for (i = 0; i < DEF_MAXNOTIFYMSGS; i++)
		if (m_pNoticeMsgList[i] != 0) delete m_pNoticeMsgList[i];
	for (i = 0; i < DEF_MAXFISHS; i++)
		if (m_pFish[i] != 0) delete m_pFish[i];
	for (i = 0; i < DEF_MAXMINERALS; i++)
		if (m_pMineral[i] != 0) delete m_pMineral[i];
	for (i = 0; i < DEF_MAXPOTIONTYPES; i++) {
		if (m_pPotionConfigList[i] != 0) delete m_pPotionConfigList[i];
		if (m_pCraftingConfigList[i] != 0) delete m_pCraftingConfigList[i];	// Crafting
	}
	for (i = 0; i < DEF_MAXBUILDITEMS; i++) 
		if (m_pBuildItemList[i] != 0) delete m_pBuildItemList[i];

	for (i = 0; i < DEF_MAXNPCTYPES; i++)
		m_iNpcConstructionPoint[i] = 0;
	for (i = 0; i < DEF_MAXSCHEDULE; i++) {
		m_stCrusadeWarSchedule[i].iDay = -1;
		m_stCrusadeWarSchedule[i].iHour = -1;
		m_stCrusadeWarSchedule[i].iMinute = -1;
	}	
	m_iNpcConstructionPoint[1]  = 100; // MS
	m_iNpcConstructionPoint[2]  = 100; // MS
	m_iNpcConstructionPoint[3]  = 100; // MS
	m_iNpcConstructionPoint[4]  = 100; // MS
	m_iNpcConstructionPoint[5]  = 100; // MS
	m_iNpcConstructionPoint[6]  = 100; // MS
	m_iNpcConstructionPoint[43] = 1000; // LWB
	m_iNpcConstructionPoint[44] = 2000; // GHK
	m_iNpcConstructionPoint[45] = 3000; // GHKABS
	m_iNpcConstructionPoint[46] = 2000; // TK
	m_iNpcConstructionPoint[47] = 3000; // BG
	m_iNpcConstructionPoint[51] = 1500; // Catapult
	m_bIsGameStarted = false;
	m_pMainLogSock = 0;
	m_pGateSock = 0;
	m_bIsLogSockAvailable   = false;
	m_bIsGateSockAvailable  = false;
	m_bIsItemAvailable      = false;
	m_bIsBuildItemAvailable = false;
	m_bIsNpcAvailable       = false;
	m_bIsMagicAvailable     = false;
	m_bIsSkillAvailable     = false;
	m_bIsQuestAvailable     = false;
	m_bIsPotionAvailable    = false;
	m_bIsWLServerAvailable  = false;
	for (i = 0; i < DEF_MAXCLIENTS; i++)
		m_pClientList[i] = 0;
	for (i = 0; i < DEF_MAXMAPS; i++)
		m_pMapList[i] = 0;
	for (i = 0; i < DEF_MAXITEMTYPES; i++)
		m_pItemConfigList[i] = 0;
	for (i = 0; i < DEF_MAXNPCTYPES; i++)
		m_pNpcConfigList[i] = 0;
	for (i = 0; i < DEF_MAXNPCS; i++)
		m_pNpcList[i] = 0;
	for (i = 0; i < DEF_MSGQUENESIZE; i++)
		m_pMsgQuene[i] = 0;
	for (i = 0; i < DEF_MAXMAGICTYPE; i++)
		m_pMagicConfigList[i] = 0;
	for (i = 0; i < DEF_MAXSKILLTYPE; i++)
		m_pSkillConfigList[i] = 0;
	for (i = 0; i < DEF_MAXQUESTTYPE; i++)
		m_pQuestConfigList[i] = 0;
	for (i = 0; i < DEF_MAXDYNAMICOBJECTS; i++)
		m_pDynamicObjectList[i] = 0;
	for (i = 0; i < DEF_MAXDELAYEVENTS; i++)
		m_pDelayEventList[i] = 0;
	for (i = 0; i < DEF_MAXNOTIFYMSGS; i++)
		m_pNoticeMsgList[i] = 0;
	for (i = 0; i < DEF_MAXFISHS; i++)
		m_pFish[i] = 0;
	for (i = 0; i < DEF_MAXMINERALS; i++)
		m_pMineral[i] = 0;
	for (i = 0; i < DEF_MAXPOTIONTYPES; i++) {
		m_pPotionConfigList[i] = 0;
		m_pCraftingConfigList[i] = 0;	// Crafting
	}
	for (i = 0; i < DEF_MAXSUBLOGSOCK; i++) {
		m_pSubLogSock[i] = 0;
		m_bIsSubLogSockAvailable[i] = false;
	}
	for (i = 0; i < DEF_MAXBUILDITEMS; i++) 
		m_pBuildItemList[i] = 0;
	for (i = 0; i < DEF_MAXCRUSADESTRUCTURES; i++) {
		ZeroMemory(m_stCrusadeStructures[i].cMapName, sizeof(m_stCrusadeStructures[i].cMapName));
		m_stCrusadeStructures[i].cType = 0;
		m_stCrusadeStructures[i].dX    = 0;
		m_stCrusadeStructures[i].dY    = 0;
	}

	for (i = 0; i < DEF_MAXTELEPORTLIST; i++)
		m_pTeleportConfigList[i] = 0;

	// new
	for (i = 0; i < DEF_MAXTELEPORTDESTINATION; i++)
		m_pTeleportDestination[i] = 0;
	for (i = 0; i < DEF_MAXGUILDS; i++)
		m_pGuildTeleportLoc[i].m_iV1 = 0;

	for (i = 0; i < DEF_MAXCRUSADESTRUCTURES; i++) {
		m_stMiddleCrusadeStructureInfo[i].cType = 0;
		m_stMiddleCrusadeStructureInfo[i].cSide = 0;
		m_stMiddleCrusadeStructureInfo[i].sX = 0;
		m_stMiddleCrusadeStructureInfo[i].sY = 0;
	}
	m_iTotalMiddleCrusadeStructures = 0;
	m_pNoticementData = 0;
	m_iQueneHead = 0;
	m_iQueneTail = 0;
	m_iTotalClients = 0;
	m_iMaxClients   = 0;
	m_iTotalMaps    = 0;
	m_iTotalGameServerClients    = 0;
	m_iTotalGameServerMaxClients = 0;
	m_MaxUserSysTime.wHour   = 0;
	m_MaxUserSysTime.wMinute = 0;
	m_bIsServerShutdowned = false;
	m_cShutDownCode = 0;
	m_iMiddlelandMapIndex = -1;
	m_iAresdenMapIndex    = -1;
	m_iElvineMapIndex     = -1;
	m_iAresdenOccupyTiles = 0;
	m_iElvineOccupyTiles  = 0;
	m_iSubLogSockInitIndex   = 0;
	m_iCurMsgs = 0;
	m_iMaxMsgs = 0;
	///////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////Modifications////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////
	//Magn0S:: New events schedules
	for (i = 0; i < DEF_MAXSCHEDULE; i++) {
		
		m_stApocalypseScheduleStart[i].iDay = -1;
		m_stApocalypseScheduleStart[i].iHour = -1;
		m_stApocalypseScheduleStart[i].iMinute = -1;
		
		m_stHeldenianSchedule[i].iDay = -1;
		m_stHeldenianSchedule[i].StartiHour = -1;
		m_stHeldenianSchedule[i].StartiMinute = -1;
	
		m_stGladArenaSchedule[i].iDay = -1;
		m_stGladArenaSchedule[i].iHour = -1;
		m_stGladArenaSchedule[i].iMinute = -1;
		
		m_stCTFSchedule[i].iDay = -1;
		m_stCTFSchedule[i].iHour = -1;
		m_stCTFSchedule[i].iMinute = -1;
		
		m_stInvasionSchedule[i].iDay = -1;
		m_stInvasionSchedule[i].iHour = -1;
		m_stInvasionSchedule[i].iMinute = -1;
		
		m_stCandyFurySchedule[i].iDay = -1;
		m_stCandyFurySchedule[i].iHour = -1;
		m_stCandyFurySchedule[i].iMinute = -1;
		
		m_stBeholderSchedule[i].iDay = -1;
		m_stBeholderSchedule[i].iHour = -1;
		m_stBeholderSchedule[i].iMinute = -1;
		
		m_stBagProtectSchedule[i].iDay = -1;
		m_stBagProtectSchedule[i].iHour = -1;
		m_stBagProtectSchedule[i].iMinute = -1;
		
		m_stTeamArenaSchedule[i].iDay = -1;
		m_stTeamArenaSchedule[i].iHour = -1;
		m_stTeamArenaSchedule[i].iMinute = -1;
		
		m_stShinningSchedule[i].iDay = -1;
		m_stShinningSchedule[i].iHour = -1;
		m_stShinningSchedule[i].iMinute = -1;
		
		m_stHappyHourSchedule[i].iDay = -1;
		m_stHappyHourSchedule[i].iHour = -1;
		m_stHappyHourSchedule[i].iMinute = -1;
		
		m_stFuryHourSchedule[i].iDay = -1;
		m_stFuryHourSchedule[i].iHour = -1;
		m_stFuryHourSchedule[i].iMinute = -1;
	}

	for (i = 0; i < DEF_MAXADMINS; i++) {
		ZeroMemory(m_stAdminList[i].m_cGMName, sizeof(m_stAdminList[i].m_cGMName));
	}

	for (i = 0; i < DEF_MAXBANNED; i++) {
		ZeroMemory(m_stBannedList[i].m_cBannedIPaddress, sizeof(m_stBannedList[i].m_cBannedIPaddress));
	}
	///////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////
	
	m_iStrategicStatus = 0;
	m_iCollectedMana[0] = 0;
	m_iCollectedMana[1] = 0;
	m_iCollectedMana[2] = 0;
	m_iAresdenMana = 0;
	m_iElvineMana  = 0;
	m_dwSpecialEventTime = m_dwWhetherTime = m_dwGameTime1 = m_dwGameTime2 = m_dwGameTime3 = m_dwGameTime4 = m_dwGameTime5 = m_dwGameTime6 = m_dwFishTime = dwTime;
	m_bIsSpecialEventTime = false;
	GetLocalTime(&SysTime);
	m_dwCanFightzoneReserveTime = dwTime - ((SysTime.wHour%2)*60*60 + SysTime.wMinute*60)*1000;
	for(i =0; i < DEF_MAXFIGHTZONE; i++) 
		m_iFightZoneReserve[i] = 0 ;
	m_iFightzoneNoForceRecall = 0;
	for (i = 1; i < DEF_MAXEXPTABLE; i++) { //centu - lvl jump fixed
		m_iLevelExpTable[i] = iGetLevelExp(i);
	}
	
	m_iLevelExp20     = m_iLevelExpTable[DEF_LEVELLIMIT];
	m_iGameServerMode = 0;
	if (bReadProgramConfigFile("GServer.cfg") == false) {
		PutLogList(" ");
		PutLogList("(!!!) CRITICAL ERROR! Cannot execute server! GServer.cfg file contents error!");
		return false;	
	}
	if (bReadTopPlayersFile("GameConfigs\\TopPlayers.cfg") == false) {
		PutLogList(" ");
		PutLogList("(!!!) CRITICAL ERROR! Cannot execute server! TopPlayers.cfg file contents error!");
		return false;
	}
	/****MODIFICATION****/
	if (bReadSettingsConfigFile("GameConfigs\\Settings.cfg") == false) {
		PutLogList(" ");
		PutLogList("(!!!) CRITICAL ERROR! Cannot execute server! Settings.cfg file contents error!");
		return false;
	}
	if (bReadAdminListConfigFile("GameConfigs\\AdminList.cfg") == false) {
		PutLogList(" ");
		PutLogList("(!!!) CRITICAL ERROR! Cannot execute server! AdminList.cfg file contents error!");
		return false;
	}
	if (bReadBannedListConfigFile("GameConfigs\\BannedList.cfg") == false) {
		PutLogList(" ");
		PutLogList("(!!!) CRITICAL ERROR! Cannot execute server! BannedList.cfg file contents error!");
		return false;
	}
	if (bReadAdminSetConfigFile("GameConfigs\\AdminSettings.cfg") == false) {
		PutLogList(" ");
		PutLogList("(!!!) CRITICAL ERROR! Cannot execute server! AdminSettings.cfg file contents error!");
		return false;
	}
	if (bDecodeTeleportList("GameConfigs\\TeleportList.cfg") == false) 
	{	PutLogList(" ");
		PutLogList("(!!!) CRITICAL ERROR! Cannot execute server! TeleportList.cfg file contents error!");
		return false;
	}	
	
	/********END*********/
	srand((unsigned)time(0));   
	m_pMainLogSock = new class XSocket(m_hWnd, DEF_SERVERSOCKETBLOCKLIMIT);
	m_pMainLogSock->bConnect(m_cLogServerAddr, m_iLogServerPort, WM_ONLOGSOCKETEVENT);
	m_pMainLogSock->bInitBufferSize(DEF_MSGBUFFERSIZE);
	wsprintf(cTxt, "(!) Try to Connect main-log-socket... Addr:%s  Port:%d", m_cLogServerAddr, m_iLogServerPort);
	PutLogList(cTxt);
	m_pGateSock = new class XSocket(m_hWnd, DEF_SERVERSOCKETBLOCKLIMIT);
	m_pGateSock->bConnect(m_cGateServerAddr, m_iGateServerPort, WM_ONGATESOCKETEVENT);
	m_pGateSock->bInitBufferSize(DEF_MSGBUFFERSIZE);
	m_iGateSockConnRetryTimes = 1;
	wsprintf(cTxt, "(!) Try to Connect Gate Server... Addr:%s  Port:%d", m_cGateServerAddr, m_iGateServerPort);
	PutLogList(cTxt);
	// modification - m_bF5pressed
	m_bF1pressed = m_bF4pressed = m_bF12pressed = m_bF5pressed = false; 
	m_bOnExitProcess = false;
	for (i = 0; i <= 100; i++) {
		m_iSkillSSNpoint[i] = _iCalcSkillSSNpoint(i);
	}
	GetLocalTime(&SysTime);
	// centu - day/night time
	if (srvHour >= 19 || srvHour <= 6)   
		 m_cDayOrNight = 2;
	else m_cDayOrNight = 1;

	bReadNotifyMsgListFile("GameConfigs\\notice.txt");
	m_dwNoticeTime = dwTime;
	m_iCurSubLogSockIndex    = 0;
	m_iSubLogSockFailCount   = 0;
	m_iSubLogSockActiveCount = 0;
	m_pNoticementData      = 0;
	m_dwNoticementDataSize = 0;
	m_dwMapSectorInfoTime = dwTime;
	m_iMapSectorInfoUpdateCount = 0;
	m_iCrusadeCount  = 0;
	m_bIsCrusadeMode = false;
	m_wServerID_GSS = iDice(1,65535);
	// changed DEF_MAXGATESERVERSTOCKMSGSIZE from 10000 to 30000
	ZeroMemory(m_cGateServerStockMsg, sizeof(m_cGateServerStockMsg));
	cp = (char *)m_cGateServerStockMsg;
	dwp = (UINT32 *)cp;
	*dwp = MSGID_SERVERSTOCKMSG;
	cp += 4;
	wp = (UINT16 *)cp;
	*wp = DEF_MSGTYPE_CONFIRM;
	cp += 2;
	m_iIndexGSS = 6;
	m_dwCrusadeGUID   = 0;
	m_iCrusadeWinnerSide = 0;
	m_iLastCrusadeWinner = 0;
	// <MISSING_VARIABLE_HERE> 107704h - unknown variable and unreferenced!
	m_bIsCrusadeWarStarter = false;
	m_iLatestCrusadeDayOfWeek = -1;
	m_iFinalShutdownCount = 0;
	m_bIsApocalypseMode = false;
	m_bIsApocalypseStarter = false; // new
	m_bIsApocalypseGateOpen = false; // new
	ZeroMemory(m_cApocalypseMapName, sizeof(m_cApocalypseMapName)); // new
	bDeathmatch = false;
	iLastKill = -1;
	iTotalKills = 0;
	dwTimeLastKill = dwTime;
	iDGtop1=iDGtop2=iDGtop3=iDGtop4=iDGtop5=iDGtop6=iDGtop7=iDGtop8=iDGtop9=iDGtop10=0;
	m_bHappyHour = false;
	m_bFuryHour = false;
	// 	Heldenian
	m_bIsHeldenianMode			 = false;
	m_bHeldenianWarInitiated	 = false;
	m_cHeldenianType			 = 0;
	m_sLastHeldenianWinner		 = 0; 
	m_cHeldenianWinner			 = -1;
	m_dwHeldenianGUID			 = 0;
	m_dwHeldenianWarStartTime	 = 0;
	m_dwHeldenianFinishTime		 = 0;
	m_cHeldenianFirstDestroyedTowerWinner = -1;
	m_iHeldenianAresdenDead		 = 0;
	m_iHeldenianElvineDead		 = 0;
	m_iHeldenianAresdenFlags	 = 0;
	m_iHeldenianElvineFlags		 = 0;
	m_iHeldenianAresdenLeftTower = 0;
	m_iHeldenianElvineLeftTower  = 0;
	ZeroMemory(m_cHeldenianMapName, sizeof(m_cHeldenianMapName)); // new
	m_bF10pressed = false; // new

	dwCrusadeFinishTime = 0;
	dwEventFinishTime = 0;

	m_dwCleanTime = dwTime;

	//Magn0S:: Start to add new Server variables
	m_iMaxArmorDrop = 0;
	m_iMaxWeaponDrop = 0;
	m_iMaxAttrWeaponDrop = 0;
	m_iMaxStatedArmor = 0;
	m_iMaxStatedWeapon = 0;

	for (i = 0; i < 13; i++)
		m_iArmorDrop[i] = 0;
	for (i = 0; i < 13; i++)
		m_iWeaponDrop[i] = 0;
	for (i = 0; i < 9; i++)
		m_iAttrWeaponDrop[i] = 0;
	for (i = 0; i < 8; i++)
		m_iStatedArmorDrop[i] = 0;
	for (i = 0; i < 4; i++)
		m_iStatedWeaponDrop[i] = 0;

	for (i = 0; i < 10; i++)
		m_bNullDrop[i] = true;

	m_iServerPhyDmg = 0;
	m_iServerMagDmg = 0;

	return true;
}

/*********************************************************************************************************************
**  void CGame::OnClientRead																						**
**  DESCRIPTION			:: reports error message if client data invalid												**
**  LAST_UPDATED		:: March 13, 2005; 8:25 PM; Hypnotoad														**
**	RETURN_VALUE		:: void																						**
**  NOTES				:: none																						**
**	MODIFICATION		:: none																						**
**********************************************************************************************************************/
void CGame::OnClientRead(int iClientH)
{
 char  * pData, cKey;
 UINT32  dwMsgSize;
	
	if (m_pClientList[iClientH] == 0) return;
	pData = m_pClientList[iClientH]->m_pXSock->pGetRcvDataPointer(&dwMsgSize, &cKey);
	if (bPutMsgQuene(DEF_MSGFROM_CLIENT, pData, dwMsgSize, iClientH, cKey) == false) {
		PutLogList("@@@@@@ CRITICAL ERROR in MsgQuene!!! (OnClientRead) @@@@@@");
	}
}

/*********************************************************************************************************************
**  void CGame::DisplayInfo																							**
**  DESCRIPTION			:: prints header																			**
**  LAST_UPDATED		:: March 13, 2005; 9:06 PM; Hypnotoad														**
**	RETURN_VALUE		:: void																						**
**  NOTES				:: none																						**
**	MODIFICATION		:: none																						**
**********************************************************************************************************************/
void CGame::DisplayInfo(HDC hdc)
{
 char cTxt[350];  
 int  i, iLine;

    TextOut(hdc, 650, 4, "[Server Config]", 16);

	TextOut(hdc, 605, 25+15, "Server Time:", 13);
	wsprintf(cTxt, "%.2d:%.2d", srvHour, srvMinute);
	TextOut(hdc, 735, 25+15, cTxt, strlen(cTxt));

	TextOut(hdc, 605, 40+30, "Players Online:", 16);
	wsprintf(cTxt, "%d/%d", m_iTotalClients, m_iMaxClients);
	TextOut(hdc, 735, 40 + 30, cTxt, strlen(cTxt));

	wsprintf(cTxt, "%d", m_iTotalClients);
	PutLogOnline(cTxt);

	TextOut(hdc, 605, 55 + 30, "Max Resistance:", 16);
	wsprintf(cTxt, "%d%%", m_iMaxResist * 7);
	TextOut(hdc, 735, 55 + 30, cTxt, strlen(cTxt));

	TextOut(hdc, 605, 70 + 30, "Max Absorption:", 16);
	wsprintf(cTxt, "%d%%", m_iMaxAbs * 3);
	TextOut(hdc, 735, 70 + 30, cTxt, strlen(cTxt));

	TextOut(hdc, 605, 85 + 30, "Max Recovery:", 14);
	wsprintf(cTxt, "%d%%", m_iMaxRec * 7);
	TextOut(hdc, 735, 85 + 30, cTxt, strlen(cTxt));

	TextOut(hdc, 605, 100 + 30, "Max Damage:", 12);
	wsprintf(cTxt, "%d%%", m_iMaxHPCrit * 7);
	TextOut(hdc, 735, 100 + 30, cTxt, strlen(cTxt));

    ZeroMemory(cTxt, sizeof(cTxt));
    TextOut(hdc, 605, 130+30, "[MAPS]                  [MOBS]", 30);

    iLine = 0;
    for (i = 0; i < DEF_MAXMAPS; i++)
    if (m_pMapList[i] != 0) {
        ZeroMemory(G_cTxt, sizeof(G_cTxt));
        wsprintf(G_cTxt, m_pMapList[i]->m_cName);
        TextOut(hdc, 605, 145+30 + iLine*14, G_cTxt, strlen(G_cTxt));
        wsprintf(G_cTxt, "%d",m_pMapList[i]->m_iTotalActiveObject);
        TextOut(hdc, 740, 145+30 + iLine*14, G_cTxt, strlen(G_cTxt));
        iLine++;
    }

	TextOut(hdc, 40, 400, "[Server Rates]", 15);

	TextOut(hdc, 8, 430, "1st Drop:", 10);
	wsprintf(cTxt, "%d%%", m_iPrimaryDropRate);
	TextOut(hdc, 138, 430, cTxt, strlen(cTxt));

	TextOut(hdc, 8, 445, "2nd Drop:", 10);
	wsprintf(cTxt, "%d%%", m_iSecondaryDropRate);
	TextOut(hdc, 138, 445, cTxt, strlen(cTxt));

	TextOut(hdc, 8, 460, "Rare Drop:", 11);
	wsprintf(cTxt, "%d%%", m_iRareDropRate);
	TextOut(hdc, 138, 460, cTxt, strlen(cTxt));

	TextOut(hdc, 8, 475, "Fragile Drop:", 14);
	wsprintf(cTxt, "%d%%", m_iFragileDropRate);
	TextOut(hdc, 138, 475, cTxt, strlen(cTxt));

	TextOut(hdc, 8, 490, "Gold Drop:", 11);
	wsprintf(cTxt, "%d%%", m_iGoldRate);
	TextOut(hdc, 138, 490, cTxt, strlen(cTxt));
 
}

/////////////////////////////////////////////////////////////////////////////////////
//  void CGame::CheckDenialServiceAttack(UINT32 dwClientTime, char cKey)
//  description			: checks if player send continuouslly same packet
//  DS attack could be done using a packer sniffer to capture a client packet then send it continuouslly.
/////////////////////////////////////////////////////////////////////////////////////
void CGame::CheckDenialServiceAttack(int iClientH, UINT32 dwClientTime)
{
	UINT32 dwTime = timeGetTime();
	if (m_pClientList[iClientH] == 0) return;
	//1st chek: find a Denial of service attack by packet sent time
	if (m_pClientList[iClientH]->m_dwDSLAT == 0)
	{	// Start with 1st msg
		m_pClientList[iClientH]->m_dwDSLAT = dwClientTime;
		m_pClientList[iClientH]->m_dwDSLATOld = dwClientTime;
		m_pClientList[iClientH]->m_dwDSLATS = dwTime;
		m_pClientList[iClientH]->m_iDSCount = 0;
	}
	else
	{
		if (dwClientTime >= m_pClientList[iClientH]->m_dwDSLAT)
		{	// current message was sent later than previous (normal case)
			m_pClientList[iClientH]->m_dwDSLAT = dwClientTime;
		}
		else
		{	// current message was sent before previous
			if (m_pClientList[iClientH]->m_dwDSLATOld == dwClientTime)
			{	// If we receive more late msg with same time
				m_pClientList[iClientH]->m_iDSCount++;
				if (((dwTime - m_pClientList[iClientH]->m_dwDSLATS) > 10 * 1000)
					&& (m_pClientList[iClientH]->m_iDSCount > 5))
				{	// Receiving a "late" msg more than 10 sec after !
					// This is an attack!
					wsprintf(G_cTxt, "DS check:   PC(%s) - Denial of service attack! (Disc.) \tIP(%s)"
						, m_pClientList[iClientH]->m_cCharName
						, m_pClientList[iClientH]->m_cIPaddress);
					PutHackLogFileList(G_cTxt);
					PutLogList(G_cTxt);
					DeleteClient(iClientH, true, true, true, true);
				}
			}
			else
			{	// else this message become late msg
				m_pClientList[iClientH]->m_dwDSLATOld = dwClientTime;
				m_pClientList[iClientH]->m_iDSCount = 1;
				m_pClientList[iClientH]->m_dwDSLATS = dwTime;
			}
		}
	}
}

/*********************************************************************************************************************
**  void CGame::ClientMotionHandler																					**
**  DESCRIPTION			:: controls most client actions																**
**  LAST_UPDATED		:: March 13, 2005; 10:19 PM; Hypnotoad														**
**	RETURN_VALUE		:: void																						**
**  NOTES				:: - in DEF_OBJECTATTACKMOVE iRet 1's iClientMotion_Attack_Handler bResponse set to true	**
**	MODIFICATION		:: - added checks and logging to 2 variants of magic hacks (instant packet, speed)			**
**********************************************************************************************************************/
void CGame::ClientMotionHandler(int iClientH, char * pData)
{
 UINT32 * dwp, dwClientTime;
 UINT16 * wp, wCommand, wTargetObjectID;
 short * sp, sX, sY, dX, dY, wType;
 char  * cp, cDir;
 int   iRet, iTemp;
	
	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
	if (m_pClientList[iClientH]->m_bIsKilled == true) return;
	
	wp = (UINT16 *)(pData + DEF_INDEX2_MSGTYPE);
	wCommand = *wp;

	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

	sp = (short *)cp;
	sX = *sp;
	cp += 2;

	sp = (short *)cp;
	sY = *sp;
	cp += 2;

	cDir = *cp;
	cp++;

	sp = (short *)cp;
	dX = *sp;
	cp += 2;

	sp = (short *)cp;
	dY = *sp;
	cp += 2;

	sp = (short *)cp;
	wType = *sp;
	cp += 2;

	if ((wCommand == DEF_OBJECTATTACK) || (wCommand == DEF_OBJECTATTACKMOVE)) {
		wp = (UINT16 *)cp;
		wTargetObjectID = *wp;
		cp += 2;
	}

	dwp = (UINT32 *)cp;
	dwClientTime = *dwp;
	cp += 4;


	if (m_bAntiHackMode) CheckDenialServiceAttack(iClientH, dwClientTime);

	m_pClientList[iClientH]->m_dwLastActionTime = timeGetTime();

	switch (wCommand) {
	case DEF_OBJECTSTOP:
		iRet = iClientMotion_Stop_Handler(iClientH, sX, sY, cDir);
		if (iRet == 1) {
			SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTSTOP, 0, 0, 0);
		}
		else if (iRet == 2) {
			SendObjectMotionRejectMsg(iClientH);
		}
		break;

	case DEF_OBJECTRUN:
		iRet = iClientMotion_Move_Handler(iClientH, sX, sY, cDir, 1);
		if (iRet == 1) {
			SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTRUN, 0, 0, 0);
		}
		if ((m_pClientList[iClientH] != 0) && (m_pClientList[iClientH]->m_iHP <= 0)) {
			ClientKilledHandler(iClientH, 0, 0, 1);
		}
		bCheckClientMoveFrequency(iClientH, dwClientTime);
		break;

	case DEF_OBJECTMOVE:
		iRet = iClientMotion_Move_Handler(iClientH, sX, sY, cDir, 2);
		if (iRet == 1) {
			SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTMOVE, 0, 0, 0);
		}
		if ((m_pClientList[iClientH] != 0) && (m_pClientList[iClientH]->m_iHP <= 0)) {
			ClientKilledHandler(iClientH, 0, 0, 1);
		}
		bCheckClientMoveFrequency(iClientH, dwClientTime);
		break;

	case DEF_OBJECTDAMAGEMOVE:
		iRet = iClientMotion_Move_Handler(iClientH, sX, sY, cDir, 0);
		if (iRet == 1) {
			SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTDAMAGEMOVE, m_pClientList[iClientH]->m_iLastDamage, 0, 0);
		}
		if ((m_pClientList[iClientH] != 0) && (m_pClientList[iClientH]->m_iHP <= 0)) {
			ClientKilledHandler(iClientH, 0, 0, 1);
		}
		break;

	case DEF_OBJECTATTACKMOVE:
		iRet = iClientMotion_Move_Handler(iClientH, sX, sY, cDir, 0);
		if ((iRet == 1) && (m_pClientList[iClientH] != 0)) {
			SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTATTACKMOVE, 0, 0, 0);
			// changed bResponse to true
			iClientMotion_Attack_Handler(iClientH, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, dX, dY, wType, cDir, wTargetObjectID, false, true);
		}
		if ((m_pClientList[iClientH] != 0) && (m_pClientList[iClientH]->m_iHP <= 0)) {
			ClientKilledHandler(iClientH, 0, 0, 1);
		}
		bCheckClientAttackFrequency(iClientH, dwClientTime);
		break;

	case DEF_OBJECTATTACK:
		_CheckAttackType(iClientH, &wType);
		iRet = iClientMotion_Attack_Handler(iClientH, sX, sY, dX, dY, wType, cDir, wTargetObjectID, true, false);
		if (iRet == 1) {
			if (wType >= 20) {
				m_pClientList[iClientH]->m_iSuperAttackLeft--;
				if (m_pClientList[iClientH]->m_iSuperAttackLeft < 0) m_pClientList[iClientH]->m_iSuperAttackLeft = 0;
			}
			SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTATTACK, dX, dY, wType);
		}
		else if (iRet == 2) {
			SendObjectMotionRejectMsg(iClientH);
		}
		bCheckClientAttackFrequency(iClientH, dwClientTime);
		break;

	case DEF_OBJECTGETITEM:
		iRet = iClientMotion_GetItem_Handler(iClientH, sX, sY, cDir);
		if (iRet == 1) {
			SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTGETITEM, 0, 0, 0);
		}
		else if (iRet == 2) {
			SendObjectMotionRejectMsg(iClientH);
		}
		break;

	case DEF_OBJECTMAGIC:
		iRet = iClientMotion_Magic_Handler(iClientH, sX, sY, cDir);
		if (iRet == 1) {
			/****MODIFICATION****/
			if (m_pClientList[iClientH]->m_bMagicPauseTime == false) {
				m_pClientList[iClientH]->m_bMagicPauseTime = true;
				iTemp = 10;
				SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTMAGIC, dX, iTemp, 0);
				m_pClientList[iClientH]->m_iSpellCount++;
				bCheckClientMagicFrequency(iClientH, dwClientTime);
			}
			else {
				wsprintf(G_cTxt, "(!) Cast Delay Hack: (%s) Player: (%s) - player casting too fast.", m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName);
				PutHackLogFileList(G_cTxt);
			}
			/********END*********/
		}
		else if (iRet == 2) {
			SendObjectMotionRejectMsg(iClientH);
		}
		break;

	default:
		break;
	}
}

/*********************************************************************************************************************
**  int CGame::iClientMotion_Move_Handler(int iClientH, short sX, short sY, char cDir, char cMoveType)				**
**  DESCRIPTION			:: Handles how player or npc run, walk, attack, or get flown by attack						**
**  LAST_UPDATED		:: March 14, 2005; 11:34 AM; Hypnotoad														**
**	RETURN_VALUE		:: int																						**
**  NOTES				::	- added speed hack detection															**
**							- cMoveType changed from bRun															**
**							- added apocalypse gate teleporter														**
**							- added motion isBlocked (1050)															**
**	MODIFICATION		:: none																						**
**********************************************************************************************************************/
int CGame::iClientMotion_Move_Handler(int iClientH, short sX, short sY, char cDir, char cMoveType)
{
 char  * cp, cData[3000];
 class CTile * pTile;
 UINT32 * dwp, dwTime;
 UINT16  * wp, wObjectID;
 short * sp, dX, dY, sDOtype, wV1, wV2, sTemp;
 int   * ip, iRet, iSize, iDamage, iTemp, iTemp2, iDestX, iDestY;
 bool  bRet, bIsBlocked = false;
 char cTemp[11], cDestMapName[11], cDestDir;

	if (m_pClientList[iClientH] == 0) return 0;
	if ((cDir <= 0) || (cDir > 8))       return 0;
	if (m_pClientList[iClientH]->m_bIsKilled == true) return 0;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return 0;
	if ((sX != m_pClientList[iClientH]->m_sX) && (sY != m_pClientList[iClientH]->m_sY)) return 2;
	
	dwTime = timeGetTime();
	m_pClientList[iClientH]->m_dwLastActionTime = dwTime;
	
	// new - speed hack detection
	if (cMoveType == 2) {
		if (m_pClientList[iClientH]->m_iRecentWalkTime <= dwTime - 16) {
			m_pClientList[iClientH]->m_iRecentWalkTime = dwTime;
		}
		else if ((m_pClientList[iClientH]->m_sV1 >= 1) && (dwTime < m_pClientList[iClientH]->m_iRecentWalkTime)) {
			bIsBlocked = true;
			m_pClientList[iClientH]->m_sV1 = 0;	
		}
		else {
			m_pClientList[iClientH]->m_sV1++;	
		}
		m_pClientList[iClientH]->m_iRecentWalkTime = dwTime;
		if (bIsBlocked == false) m_pClientList[iClientH]->m_iMoveMsgRecvCount++;
		if ((m_pClientList[iClientH]->m_iMoveMsgRecvCount >= 3) && (m_pClientList[iClientH]->m_dwMoveLAT != 0)) {
			if ((m_pClientList[iClientH]->m_dwMoveLAT - dwTime) < 20) {
				wsprintf(G_cTxt, "(!) 3.51 Walk Speeder: (%s) Player: (%s) walk difference: %d. Speed Hack?", m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName, (dwTime - m_pClientList[iClientH]->m_dwMoveLAT));
				PutHackLogFileList(G_cTxt);
				bIsBlocked = true;
			}
		}
		m_pClientList[iClientH]->m_dwMoveLAT = dwTime;
		m_pClientList[iClientH]->m_iMoveMsgRecvCount = 0;
	}
	else if (cMoveType == 1) {
		if (m_pClientList[iClientH]->m_iRecentRunTime <= dwTime - 8) {
			m_pClientList[iClientH]->m_iRecentRunTime = dwTime;
		}
		else if ((m_pClientList[iClientH]->m_sV1 >= 1) && (dwTime < m_pClientList[iClientH]->m_iRecentRunTime)) {
			bIsBlocked = true;
			m_pClientList[iClientH]->m_sV1 = 0;
		}
		else {
			m_pClientList[iClientH]->m_sV1++;
		}
		m_pClientList[iClientH]->m_iRecentRunTime = dwTime;
		if (bIsBlocked == false) m_pClientList[iClientH]->m_iRunMsgRecvCount++;
		if ((m_pClientList[iClientH]->m_iRunMsgRecvCount >= 3) && (m_pClientList[iClientH]->m_dwRunLAT != 0)) {
			if ((dwTime - m_pClientList[iClientH]->m_dwRunLAT) < 12) {
				wsprintf(G_cTxt, "(!) 3.51 Run Speeder: (%s) Player: (%s) run difference: %d. Speed Hack?", m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName, (dwTime - m_pClientList[iClientH]->m_dwRunLAT));
				PutHackLogFileList(G_cTxt);
				bIsBlocked = true;
			}
		}
		m_pClientList[iClientH]->m_dwRunLAT	= dwTime;
		m_pClientList[iClientH]->m_iRunMsgRecvCount = 0;
	}
	else if (cMoveType > 3) cMoveType -= 4; // Move type 4+ indicates move generated by server, not client !
	// end speed hack detection
	
	if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex] != 0) {
		int iStX = m_pClientList[iClientH]->m_sX / 20;
		int iStY = m_pClientList[iClientH]->m_sY / 20;
		m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_stTempSectorInfo[iStX][iStY].iPlayerActivity++;
		switch (m_pClientList[iClientH]->m_cSide) {
		case 1: m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_stTempSectorInfo[iStX][iStY].iAresdenActivity++;  break;
		case 2: m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_stTempSectorInfo[iStX][iStY].iElvineActivity++;   break;
		case 0:
		default: m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_stTempSectorInfo[iStX][iStY].iNeutralActivity++; break;
		}
	}

	ClearSkillUsingStatus(iClientH);	
	dX = m_pClientList[iClientH]->m_sX;
	dY = m_pClientList[iClientH]->m_sY;
	switch (cDir) {
		case 1:	dY--; break;
		case 2:	dX++; dY--;	break;
		case 3:	dX++; break;
		case 4:	dX++; dY++;	break;
		case 5: dY++; break;
		case 6:	dX--; dY++;	break;
		case 7:	dX--; break;
		case 8:	dX--; dY--;	break;
	}
	
	bRet = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bGetMoveable(dX, dY, &sDOtype);
	if (m_pClientList[iClientH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] != 0) bRet = false;

	// Capture the Flag
	if (m_bIsCTFEvent && (m_pClientList[iClientH]->m_iStatus & 0x80000) != 0) {
		ZeroMemory(cDestMapName, sizeof(cDestMapName));
		bool bRet2 = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSearchTeleportDest(dX, dY, cDestMapName, &iDestX, &iDestY, &cDestDir);
		if (bRet2 &&
			(memcmp(cDestMapName, "elvine", 6) != 0 &&
				memcmp(cDestMapName, "aresden", 7) != 0 &&
				memcmp(cDestMapName, "2ndmiddle", 9) != 0 &&
				memcmp(cDestMapName, "middleland", 10) != 0 &&
				memcmp(cDestMapName, "middled1n", 9) != 0)) {
			bRet = false;
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_EVENT, 0, 10, 0, 0);
		}
	}

	if ((bRet == true) && (bIsBlocked == false)) {
		
		m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->ClearOwner(1, iClientH, DEF_OWNERTYPE_PLAYER, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY);
		m_pClientList[iClientH]->m_sX   = dX;
		m_pClientList[iClientH]->m_sY   = dY;
		m_pClientList[iClientH]->m_cDir = cDir;
		RefreshPartyCoords(iClientH);
		minimap_update(iClientH);
		minimap_update_mark(iClientH);
		m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetOwner(iClientH, DEF_OWNERTYPE_PLAYER, dX, dY);
		// Capture the Flag
		if (m_bIsCTFEvent && (m_pClientList[iClientH]->m_iStatus & 0x80000) != 0) {
			switch (m_pClientList[iClientH]->m_cSide) {
			case 1:
				if (m_pClientList[iClientH]->m_cMapIndex == m_iAresdenMapIndex && dX == 151 && dY == 128) {
					m_pClientList[iClientH]->m_iStatus ^= 0x80000;
					SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);

					m_iCTFEventFlag[1] = iAddDynamicObjectList(0, 0, DEF_DYNAMICOBJECT_ELVINEFLAG2, m_iElvineMapIndex, 151, 132, 0, 0);
					m_cCFTEventCount[0]++;
					UpdateEventStatus();

					m_pClientList[iClientH]->m_iEnemyKillCount += 200;
					if (m_pClientList[iClientH]->m_iEnemyKillCount > m_pClientList[iClientH]->m_iMaxEK)
					{
						m_pClientList[iClientH]->m_iMaxEK = m_pClientList[iClientH]->m_iEnemyKillCount;
					}
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_ENEMYKILLS, m_pClientList[iClientH]->m_iEnemyKillCount, m_pClientList[iClientH]->m_iMaxEK, 0, 0);
					ShowClientMsg(iClientH, "You've received 200 EKs for holding the enemy flag!");
					calcularTop15HB(iClientH);
					m_iCTFEventFlagHolder[0] = -1;
					SetIceFlag(iClientH, DEF_OWNERTYPE_PLAYER, false);

					for (int i = 1; i < DEF_MAXCLIENTS; i++)
						if (m_pClientList[i] != 0 && m_pClientList[i]->m_cSide != 0) SendNotifyMsg(0, i, DEF_NOTIFY_EVENT, 0, 6 + m_pClientList[iClientH]->m_cSide - 1, 0, 0);
					
					if (m_cCFTEventCount[0] == 10)
					{
						AdminOrder_SetEvent();
					}
				}
				break;
			case 2:
				if (m_pClientList[iClientH]->m_cMapIndex == m_iElvineMapIndex && dX == 151 && dY == 132) {
					m_pClientList[iClientH]->m_iStatus ^= 0x80000;
					SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);

					m_iCTFEventFlag[0] = iAddDynamicObjectList(0, 0, DEF_DYNAMICOBJECT_ARESDENFLAG2, m_iAresdenMapIndex, 151, 128, 0, 0);
					m_cCFTEventCount[1]++;
					UpdateEventStatus();

					m_pClientList[iClientH]->m_iEnemyKillCount += 200;
					if (m_pClientList[iClientH]->m_iEnemyKillCount > m_pClientList[iClientH]->m_iMaxEK)
					{
						m_pClientList[iClientH]->m_iMaxEK = m_pClientList[iClientH]->m_iEnemyKillCount;
					}
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_ENEMYKILLS, m_pClientList[iClientH]->m_iEnemyKillCount, m_pClientList[iClientH]->m_iMaxEK, 0, 0);
					ShowClientMsg(iClientH, "You've received 200 EKs for holding the enemy flag!");
					calcularTop15HB(iClientH);
					m_iCTFEventFlagHolder[1] = -1;
					SetIceFlag(iClientH, DEF_OWNERTYPE_PLAYER, false);

					for (int i = 1; i < DEF_MAXCLIENTS; i++)
						if (m_pClientList[i] != 0 && m_pClientList[i]->m_cSide != 0) SendNotifyMsg(0, i, DEF_NOTIFY_EVENT, 0, 6 + m_pClientList[iClientH]->m_cSide - 1, 0, 0);
					
					if (m_cCFTEventCount[1] == 10)
					{
						AdminOrder_SetEvent();
					}
				}
				break;
			}
		}
		pTile = (class CTile*)(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_pTile + dX + dY * m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_sSizeY);
		switch (sDOtype) {
		case DEF_DYNAMICOBJECT_SPIKE:
			if (!(m_pClientList[iClientH]->m_bIsNeutral == true && (m_pClientList[iClientH]->m_sAppr2 & 0xF000) == 0)) {
				iDamage = iDice(2, 4);
				if (m_pClientList[iClientH]->m_iAdminUserLevel == 0)
					m_pClientList[iClientH]->m_iHP -= iDamage;
			}
			break;
		case DEF_DYNAMICOBJECT_MAGICTRAP: // kamal
			iTemp = pTile->m_wDynamicObjectID;
			if (m_pClientList[m_pDynamicObjectList[iTemp]->m_sOwner] != 0 && m_pClientList[m_pDynamicObjectList[iTemp]->m_sOwner]->m_cSide != m_pClientList[iClientH]->m_cSide) {
				if (m_pDynamicObjectList[iTemp]->m_iV1 != 0) PlayerMagicHandler(m_pDynamicObjectList[iTemp]->m_sOwner, dX, dY, m_pDynamicObjectList[iTemp]->m_iV1-1,0,0, true);
				if (m_pDynamicObjectList[iTemp]->m_iV2 != 0) PlayerMagicHandler(m_pDynamicObjectList[iTemp]->m_sOwner, dX, dY, m_pDynamicObjectList[iTemp]->m_iV2-1,0,0, true);
				if (m_pDynamicObjectList[iTemp]->m_iV3 != 0) PlayerMagicHandler(m_pDynamicObjectList[iTemp]->m_sOwner, dX, dY, m_pDynamicObjectList[iTemp]->m_iV3-1,0,0, true);

				SendEventToNearClient_TypeB(MSGID_DYNAMICOBJECT, DEF_MSGTYPE_REJECT, m_pDynamicObjectList[iTemp]->m_cMapIndex, m_pDynamicObjectList[iTemp]->m_sX, m_pDynamicObjectList[iTemp]->m_sY, m_pDynamicObjectList[iTemp]->m_sType, iTemp, 0, (short)0);

				m_pMapList[m_pDynamicObjectList[iTemp]->m_cMapIndex]->SetDynamicObject(0, 0, m_pDynamicObjectList[iTemp]->m_sX, m_pDynamicObjectList[iTemp]->m_sY, dwTime);

				delete m_pDynamicObjectList[iTemp];
				m_pDynamicObjectList[iTemp] = 0;
			}
			break;
			// Capture the Flag
		case DEF_DYNAMICOBJECT_ARESDENFLAG2:
		case DEF_DYNAMICOBJECT_ELVINEFLAG2:
			if (m_bIsCTFEvent && ((m_pClientList[iClientH]->m_cSide == 1 && sDOtype == DEF_DYNAMICOBJECT_ELVINEFLAG2 && m_iCTFEventFlagHolder[1] == -1) || (m_pClientList[iClientH]->m_cSide == 2 && sDOtype == DEF_DYNAMICOBJECT_ARESDENFLAG2 && m_iCTFEventFlagHolder[0] == -1))) {
				iTemp = pTile->m_wDynamicObjectID;

				SendEventToNearClient_TypeB(MSGID_DYNAMICOBJECT, DEF_MSGTYPE_REJECT, m_pDynamicObjectList[iTemp]->m_cMapIndex, m_pDynamicObjectList[iTemp]->m_sX, m_pDynamicObjectList[iTemp]->m_sY, m_pDynamicObjectList[iTemp]->m_sType, iTemp, 0, (short)0);

				m_pMapList[m_pDynamicObjectList[iTemp]->m_cMapIndex]->SetDynamicObject(0, 0, m_pDynamicObjectList[iTemp]->m_sX, m_pDynamicObjectList[iTemp]->m_sY, dwTime);

				delete m_pDynamicObjectList[iTemp];
				m_pDynamicObjectList[iTemp] = 0;

				m_iCTFEventFlagHolder[m_pClientList[iClientH]->m_cSide - 1] = iClientH;
				m_pClientList[iClientH]->m_dwCTFHolderTime = dwTime;

				m_pClientList[iClientH]->m_iStatus |= 0x80000;

				SetInvisibilityFlag(iClientH, DEF_OWNERTYPE_PLAYER, false);
				SetIceFlag(iClientH, DEF_OWNERTYPE_PLAYER, true);

				if (m_pClientList[iClientH]->m_bIsSpecialAbilityEnabled == true) {
					m_pClientList[iClientH]->m_bIsSpecialAbilityEnabled = false;
					m_pClientList[iClientH]->m_iSpecialAbilityTime = DEF_SPECABLTYTIMESEC;
					sTemp = m_pClientList[iClientH]->m_sAppr4;
					sTemp = sTemp & 0xFF0F;
					m_pClientList[iClientH]->m_sAppr4 = sTemp;
				}

				SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);

				for (int i = 1; i < DEF_MAXCLIENTS; i++)
					if (m_pClientList[i] != 0 && m_pClientList[i]->m_cSide != 0) SendNotifyMsg(0, i, DEF_NOTIFY_EVENT, 0, 4 + m_pClientList[iClientH]->m_cSide - 1, 0, 0);

			}
			break;
		}
		if (m_pClientList[iClientH]->m_iHP <= 0) m_pClientList[iClientH]->m_iHP = 0;
		dwp  = (UINT32 *)(cData + DEF_INDEX4_MSGID);
		*dwp = MSGID_RESPONSE_MOTION;
		wp   = (UINT16 *)(cData + DEF_INDEX2_MSGTYPE);
		*wp  = DEF_OBJECTMOVE_CONFIRM;
		
		cp = (char *)(cData + DEF_INDEX2_MSGTYPE+2);
		
		// centu - 800x600
		sp  = (short *)cp;
		*sp = (short)(dX - 12); 
		cp += 2;
		
		sp  = (short *)cp;
		*sp = (short)(dY - 9); 
		cp += 2;
		
		*cp = cDir;
		cp++;
		if (cMoveType == 1) {
			if (m_pClientList[iClientH]->m_iSP > 0) {
				*cp = 0;
				if (m_pClientList[iClientH]->m_iTimeLeft_FirmStaminar == 0) {
					m_pClientList[iClientH]->m_iSP--;
					*cp = 1;
				}
			}
			else {
				*cp = 0;
				if (m_pClientList[iClientH]->m_iTimeLeft_FirmStaminar == 0) {
					
					*cp = 1;
				}
				if (m_pClientList[iClientH]->m_iSP < 0) {
					m_pClientList[iClientH]->m_iSP = 0;
					DeleteClient(iClientH, true, true);
					return 0;
				}
			}
		}
		else *cp = 0;
		cp++;

		*cp = (char)pTile->m_iOccupyStatus;
		cp++;
		
		ip = (int *)cp;
		*ip = m_pClientList[iClientH]->m_iHP;
		cp += 4;
		
		// centu - 800x600
		iSize = iComposeMoveMapData((short)(dX - 12), (short)(dY - 9), iClientH, cDir, cp);	
		iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, iSize + 12 + 1 + 4);
		switch (iRet) {
		case DEF_XSOCKEVENT_QUENEFULL:
		case DEF_XSOCKEVENT_SOCKETERROR:
		case DEF_XSOCKEVENT_CRITICALERROR:
		case DEF_XSOCKEVENT_SOCKETCLOSED:
			DeleteClient(iClientH, true, true, true, false);
			return 0;
		}
		// new - apocalypse gate teleport
		if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsGateAvailable == true) {	
			bRet = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bApocalypseGateTeleporter(dX, dY, &*cTemp, &wV1, &wV2);
			if (bRet) {
				RequestTeleportHandler(iClientH, "2   ", cTemp, wV1, wV2);
			}
		}
		// end - apocalypse gate teleport
	}
	else {
		// new motion
		m_pClientList[iClientH]->m_bIsMoveBlocked = true;
		dwp  = (UINT32 *)(cData + DEF_INDEX4_MSGID);
		*dwp = MSGID_RESPONSE_MOTION;
		wp   = (UINT16 *)(cData + DEF_INDEX2_MSGTYPE);
		if (bIsBlocked == true) *dwp = DEF_OBJECTMOTION_BLOCKED;
		else *wp = DEF_OBJECTMOVE_REJECT;

		wObjectID = (UINT16)iClientH;

		cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);

		wp  = (UINT16 *)cp;
		*wp = wObjectID;
		cp += 2;

		sp  = (short *)cp;
		sX  = m_pClientList[wObjectID]->m_sX;
		*sp = sX;
		cp += 2;

		sp  = (short *)cp;
		sY  = m_pClientList[wObjectID]->m_sY;
		*sp = sY;
		cp += 2;

		sp  = (short *)cp;
		*sp = m_pClientList[wObjectID]->m_sType;
		cp += 2;

		*cp = m_pClientList[wObjectID]->m_cDir;
		cp++;

		memcpy(cp, m_pClientList[wObjectID]->m_cCharName, 10);
		cp += 10;

		sp  = (short *)cp;
		*sp = m_pClientList[wObjectID]->m_sAppr1;
		cp += 2;

		sp  = (short *)cp;
		*sp = m_pClientList[wObjectID]->m_sAppr2;
		cp += 2;

		sp  = (short *)cp;
		*sp = m_pClientList[wObjectID]->m_sAppr3;
		cp += 2;

		sp  = (short *)cp;
		*sp = m_pClientList[wObjectID]->m_sAppr4;
		cp += 2;

		ip  = (int *)cp;
		*ip = m_pClientList[wObjectID]->m_iApprColor;
		cp += 4;
		
		// new
		ip  = (int *)cp;
		iTemp = m_pClientList[wObjectID]->m_iStatus;
		iTemp = 0x0FFFFFFF & iTemp;
		iTemp2 = iGetPlayerABSStatus(wObjectID, iClientH);
		iTemp  = (iTemp | (iTemp2 << 28));
		*ip = iTemp;
		cp += 4;

		iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 42);
		
		switch (iRet) {
		case DEF_XSOCKEVENT_QUENEFULL:
		case DEF_XSOCKEVENT_SOCKETERROR:
		case DEF_XSOCKEVENT_CRITICALERROR:
		case DEF_XSOCKEVENT_SOCKETCLOSED:
			DeleteClient(iClientH, true, true);
			break;
		}
		return 0;
	}
	m_pClientList[iClientH]->stoptime = 0;
	return 1;
}

/*********************************************************************************************************************
**  void CGame::RequestInitPlayerHandler(int iClientH, char * pData, char cKey)										**
**  DESCRIPTION			:: player login varifier																	**
**  LAST_UPDATED		:: March 14, 2005; 9:52 PM; Hypnotoad														**
**	RETURN_VALUE		:: void																						**
**  NOTES				::	- expanded DeleteClient and bSendMsgToLS												**
**	MODIFICATION		:: none																						**
**********************************************************************************************************************/
void CGame::RequestInitPlayerHandler(int iClientH, char * pData, char cKey)
{
 int i;
 char * cp, cCharName[11], cAccountName[11], cAccountPassword[11], cTxt[120];
 bool bIsObserverMode;
	
	// SNOOPY; added trap for faultlly client ID
	if (iClientH < 1) return;

	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == true) return;

	ZeroMemory(cCharName, sizeof(cCharName));
	ZeroMemory(cAccountName, sizeof(cAccountName));
	ZeroMemory(cAccountPassword, sizeof(cAccountPassword));

	ZeroMemory(m_pClientList[iClientH]->m_cCharName, sizeof(m_pClientList[iClientH]->m_cCharName));
	ZeroMemory(m_pClientList[iClientH]->m_cAccountName, sizeof(m_pClientList[iClientH]->m_cAccountName));
	ZeroMemory(m_pClientList[iClientH]->m_cAccountPassword, sizeof(m_pClientList[iClientH]->m_cAccountPassword));
	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
	memcpy(cCharName, cp, 10);
	cp += 10;

	ZeroMemory(cTxt, sizeof(cTxt));
	memcpy(cTxt, cCharName, 10);
	m_Misc.bDecode(cKey, cTxt);
	ZeroMemory(cCharName, sizeof(cCharName));
	memcpy(cCharName, cTxt, 10);


	memcpy(cAccountName, cp, 10);
	cp += 10;
	
	ZeroMemory(cTxt, sizeof(cTxt));
	memcpy(cTxt, cAccountName, 10);
	m_Misc.bDecode(cKey, cTxt);
	ZeroMemory(cAccountName, sizeof(cAccountName));
	memcpy(cAccountName, cTxt, 10);

	memcpy(cAccountPassword, cp, 10);
	cp += 10;

	ZeroMemory(cTxt, sizeof(cTxt));
	memcpy(cTxt, cAccountPassword, 10);
	m_Misc.bDecode(cKey, cTxt);
	ZeroMemory(cAccountPassword, sizeof(cAccountPassword));
	memcpy(cAccountPassword, cTxt, 10);
	
	bIsObserverMode = (bool)*cp;
	cp++;

	for (i = 1; i < DEF_MAXCLIENTS; i++)
	if ((m_pClientList[i] != 0) && (iClientH != i) && (memcmp(m_pClientList[i]->m_cAccountName, cAccountName, 10) == 0)) {
		if (memcmp(m_pClientList[i]->m_cAccountPassword, cAccountPassword, 10) == 0) { 
			wsprintf(G_cTxt, "(!) <%d> Duplicate account player! Deleted with data save : CharName(%s) AccntName(%s) IP(%s)", i, m_pClientList[i]->m_cCharName, m_pClientList[i]->m_cAccountName, m_pClientList[i]->m_cIPaddress);
			PutLogList(G_cTxt);
			DeleteClient(i, true, true, false);
		}
		else {
			memcpy(m_pClientList[iClientH]->m_cCharName, cCharName, 10);
			memcpy(m_pClientList[iClientH]->m_cAccountName, cAccountName, 10);
			memcpy(m_pClientList[iClientH]->m_cAccountPassword, cAccountPassword, 10);
			DeleteClient(iClientH, false, false, false);
			return;
		}
	}	
	
	for (i = 1; i < DEF_MAXCLIENTS; i++)
	if ((m_pClientList[i] != 0) && (iClientH != i) && (memcmp(m_pClientList[i]->m_cCharName, cCharName, 10) == 0)) {
		if (memcmp(m_pClientList[i]->m_cAccountPassword, cAccountPassword, 10) == 0) { 
			wsprintf(G_cTxt, "(!) <%d> Duplicate player! Deleted with data save : CharName(%s) IP(%s)", i, m_pClientList[i]->m_cCharName, m_pClientList[i]->m_cIPaddress);
			PutLogList(G_cTxt);
			DeleteClient(i, true, true, false);	
		}
		else {
			memcpy(m_pClientList[iClientH]->m_cCharName, cCharName, 10);
			memcpy(m_pClientList[iClientH]->m_cAccountName, cAccountName, 10);
			memcpy(m_pClientList[iClientH]->m_cAccountPassword, cAccountPassword, 10);
			DeleteClient(iClientH, false, false, true, false); // added true at 4
			return;
		}
	}

	memcpy(m_pClientList[iClientH]->m_cCharName, cCharName, 10);
	memcpy(m_pClientList[iClientH]->m_cAccountName, cAccountName, 10);
	memcpy(m_pClientList[iClientH]->m_cAccountPassword, cAccountPassword, 10);
	m_pClientList[iClientH]->m_bIsObserverMode = bIsObserverMode;
	bSendMsgToLS(MSGID_REQUEST_PLAYERDATA, iClientH, true); // added true to 3
}

/*********************************************************************************************************************
**  void CGame::RequestInitDataHandler																				**
**  DESCRIPTION			:: manages player data/variables on enter zone												**
**  LAST_UPDATED		:: March 15, 2005; 7:32 PM; Hypnotoad														**
**	RETURN_VALUE		:: void																						**
**  NOTES				::	- added bIsResurrected to save process time												**
**						 	- added m_cWarType																		**
**						 	- expanded DeleteClient to include true(4)												**
**						 	- missing <4> cp++																		**
**						 	- added temporary position list to stop items from scrambling on enter zone				**
**						 	- added checks to _iGetArrowItemIndex, and CalcTotalItemEffect							**
**						 	- forces player above 100 and in own town to become citizen								**
**						 	- completely redid force recall on login												**
**						 	- m_bIsInsideEnemyBuilding stores if have been inside enemy buildings					**
**						 	- now heldinain compatible (stores GUID and summon points)								**
**						 	- changed "fightzone" to "fight"														**
**						 	- notifies player if apocalypse gate is open as he enters zone							**
**	MODIFICATION		:: none																						**
**********************************************************************************************************************/
void CGame::RequestInitDataHandler(int iClientH, char* pData, char cKey, bool bIsResurrected)
{
	short* sp, sSummonPoints;
	UINT32* dwp;
	UINT16* wp;
	char* cp, cPlayerName[11], cTxt[120], cPoints;
	int* ip, i, iTotalItemA, iTotalItemB, iSize, iRet;
	SYSTEMTIME SysTime;
	char* pBuffer = 0;
	int iMapSide;
	bool bIsItemListNull;
	POINT TempItemPosList[DEF_MAXITEMS];
	bool* bp;

	if (m_pClientList[iClientH] == 0) return;
	pBuffer = new char[DEF_MSGBUFFERSIZE + 1];
	ZeroMemory(pBuffer, DEF_MSGBUFFERSIZE + 1);

	// new - check to save time process time when resurrecting
	if (bIsResurrected == false) {
		cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);
		ZeroMemory(cPlayerName, sizeof(cPlayerName));

		memcpy(cPlayerName, cp, 10);
		ZeroMemory(cTxt, sizeof(cTxt));

		memcpy(cTxt, cPlayerName, 10);
		m_Misc.bDecode(cKey, cTxt);

		ZeroMemory(cPlayerName, sizeof(cPlayerName));
		memcpy(cPlayerName, cTxt, 10);

		if (memcmp(m_pClientList[iClientH]->m_cCharName, cPlayerName, 10) != 0) {
			DeleteClient(iClientH, false, true, true, false); // added true to 4
			return;
		}
	}
	dwp = (UINT32*)(pBuffer + DEF_INDEX4_MSGID);
	*dwp = MSGID_PLAYERCHARACTERCONTENTS;
	wp = (UINT16*)(pBuffer + DEF_INDEX2_MSGTYPE);
	*wp = DEF_MSGTYPE_CONFIRM;

	cp = (char*)(pBuffer + DEF_INDEX2_MSGTYPE + 2);
	//6
	ip = (int*)cp;
	*ip = m_pClientList[iClientH]->m_iHP;
	cp += 4;
	//10
	ip = (int*)cp;
	*ip = m_pClientList[iClientH]->m_iMP;
	cp += 4;
	//14
	ip = (int*)cp;
	*ip = m_pClientList[iClientH]->m_iSP;
	cp += 4;
	//18
	ip = (int*)cp;
	*ip = m_pClientList[iClientH]->m_iDefenseRatio;
	cp += 4;
	//22
	ip = (int*)cp;
	*ip = m_pClientList[iClientH]->m_iHitRatio;
	cp += 4;
	//26
	ip = (int*)cp;
	*ip = m_pClientList[iClientH]->m_iLevel;
	cp += 4;
	//30
	ip = (int*)cp;
	*ip = m_pClientList[iClientH]->m_iStr;
	cp += 4;
	//34
	ip = (int*)cp;
	*ip = m_pClientList[iClientH]->m_iInt;
	cp += 4;
	//38
	ip = (int*)cp;
	*ip = m_pClientList[iClientH]->m_iVit;
	cp += 4;
	//42
	ip = (int*)cp;
	*ip = m_pClientList[iClientH]->m_iDex;
	cp += 4;
	//46
	ip = (int*)cp;
	*ip = m_pClientList[iClientH]->m_iMag;
	cp += 4;
	//50
	ip = (int*)cp;
	*ip = m_pClientList[iClientH]->m_iCharisma;
	cp += 4;
	//54
	ip = (int*)cp;
	*ip = m_pClientList[iClientH]->m_iLU_Pool;
	cp += 4;
	//58
	dwp = (UINT32*)cp;
	*dwp = m_pClientList[iClientH]->m_iExp;
	cp += 4;
	//62
	ip = (int*)cp;
	*ip = m_pClientList[iClientH]->m_iEnemyKillCount;
	cp += 4;
	//66
	ip = (int*)cp;
	*ip = m_pClientList[iClientH]->m_iMaxEK;
	cp += 4;
	//70
	ip = (int*)cp;
	*ip = m_pClientList[iClientH]->m_iPKCount;
	cp += 4;
	//74
	ip = (int*)cp;
	*ip = m_pClientList[iClientH]->m_iRewardGold;
	cp += 4;
	//78
	memcpy(cp, m_pClientList[iClientH]->m_cLocation, 10);
	cp += 10;
	//88
	memcpy(cp, m_pClientList[iClientH]->m_cGuildName, 20);
	cp += 20;
	//108
	ip = (int*)cp;
	*ip = m_pClientList[iClientH]->m_iGuildRank;
	cp += 4;
	//112
	ip = (int*)cp;
	*ip = m_pClientList[iClientH]->m_iSuperAttackLeft;
	cp += 4;
	//116

	// kazin
	bp = (bool*)cp;
	*bp = _candy_boost;
	cp++;
	//117
	bp = (bool*)cp;
	*bp = _revelation;
	cp++;
	//118
	bp = (bool*)cp;
	*bp = _city_teleport;
	cp++;
	//119
	bp = (bool*)cp;
	*bp = _drop_inhib;
	cp++;
	//120
	bp = (bool*)cp;
	*bp = c_team->bteam;
	cp++;
	//121

	bp = (bool*)cp;
	*bp = bShinning;
	cp++;
	//122
	ip = (int*)cp;
	*ip = m_pClientList[iClientH]->m_iAdminUserLevel;
	cp += 4;
	//126
	bp = (bool*)cp;
	*bp = m_bHappyHour;
	cp++;
	//127
	bp = (bool*)cp;
	*bp = m_bFuryHour;
	cp++;
	//128
	ip = (int*)cp;
	*ip = m_pClientList[iClientH]->m_iClass;
	cp += 4;
	//132
	ip = (int*)cp;
	*ip = m_pClientList[iClientH]->m_iCoinPoints;
	cp += 4;
	//136

	iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(pBuffer, 136);
	switch (iRet) {
	case DEF_XSOCKEVENT_QUENEFULL:
	case DEF_XSOCKEVENT_SOCKETERROR:
	case DEF_XSOCKEVENT_CRITICALERROR:
	case DEF_XSOCKEVENT_SOCKETCLOSED:
		DeleteClient(iClientH, true, true, true, false); // added true (4)
		PutLogList("HERE 1");
		if (pBuffer != 0) delete[] pBuffer;
		return;
	}

	dwp = (UINT32*)(pBuffer + DEF_INDEX4_MSGID);
	*dwp = MSGID_PLAYERITEMLISTCONTENTS;
	wp = (UINT16*)(pBuffer + DEF_INDEX2_MSGTYPE);
	*wp = DEF_MSGTYPE_CONFIRM;

	iTotalItemA = 0;
	for (i = 0; i < DEF_MAXITEMS; i++)
		if (m_pClientList[iClientH]->m_pItemList[i] != 0)
			iTotalItemA++;

	// new
	bIsItemListNull = false;
	if (!bIsItemListNull) {
		bIsItemListNull = true;
		for (i = 0; i < DEF_MAXITEMS - 1; i++) {
			if ((m_pClientList[iClientH]->m_pItemList[i] == 0) && (m_pClientList[iClientH] != 0)) {
				m_pClientList[iClientH]->m_ItemPosList[i].x = TempItemPosList[i].x;
				m_pClientList[iClientH]->m_ItemPosList[i].y = TempItemPosList[i].y;
				bIsItemListNull = false;
			}
		}
	}
	m_pClientList[iClientH]->m_cArrowIndex = _iGetArrowItemIndex(iClientH); // new
	CalcTotalItemEffect(iClientH, -1, false); // new

	cp = (char*)(pBuffer + DEF_INDEX2_MSGTYPE + 2);
	*cp = iTotalItemA;
	cp++;

	for (i = 0; i < iTotalItemA; i++) {
		if (m_pClientList[iClientH]->m_pItemList[i] == 0) {
			wsprintf(G_cTxt, "RequestInitDataHandler error: Client(%s) Item(%d)", m_pClientList[iClientH]->m_cCharName, i);
			PutLogFileList(G_cTxt);
			PutLogList(G_cTxt);
			DeleteClient(iClientH, false, true, true, false); // added true (4)
			if (pBuffer != 0) delete[] pBuffer;
			return;
		}

		memcpy(cp, m_pClientList[iClientH]->m_pItemList[i]->m_cName, 20);
		cp += 20;

		dwp = (UINT32*)cp;
		*dwp = m_pClientList[iClientH]->m_pItemList[i]->m_dwCount;
		cp += 4;

		*cp = m_pClientList[iClientH]->m_pItemList[i]->m_cItemType;
		cp++;

		*cp = m_pClientList[iClientH]->m_pItemList[i]->m_cEquipPos;
		cp++;

		*cp = (char)m_pClientList[iClientH]->m_bIsItemEquipped[i];
		cp++;

		sp = (short*)cp;
		*sp = m_pClientList[iClientH]->m_pItemList[i]->m_sLevelLimit;
		cp += 2;

		*cp = m_pClientList[iClientH]->m_pItemList[i]->m_cGenderLimit;
		cp++;

		wp = (UINT16*)cp;
		*wp = m_pClientList[iClientH]->m_pItemList[i]->m_wCurLifeSpan;
		cp += 2;

		wp = (UINT16*)cp;
		*wp = m_pClientList[iClientH]->m_pItemList[i]->m_wWeight;
		cp += 2;

		sp = (short*)cp;
		*sp = m_pClientList[iClientH]->m_pItemList[i]->m_sSprite;
		cp += 2;

		sp = (short*)cp;
		*sp = m_pClientList[iClientH]->m_pItemList[i]->m_sSpriteFrame;
		cp += 2;

		*cp = m_pClientList[iClientH]->m_pItemList[i]->m_cItemColor;
		cp++;

		*cp = (char)m_pClientList[iClientH]->m_pItemList[i]->m_sItemSpecEffectValue2;
		cp++;

		dwp = (UINT32*)cp;
		*dwp = m_pClientList[iClientH]->m_pItemList[i]->m_dwAttribute;
		cp += 4;

		//Magn0S:: Add to send new infos to client
		sp = (short*)cp;
		*sp = m_pClientList[iClientH]->m_pItemList[i]->m_sNewEffect1;
		cp += 2;

		sp = (short*)cp;
		*sp = m_pClientList[iClientH]->m_pItemList[i]->m_sNewEffect2;
		cp += 2;

		sp = (short*)cp;
		*sp = m_pClientList[iClientH]->m_pItemList[i]->m_sNewEffect3;
		cp += 2;

		sp = (short*)cp;
		*sp = m_pClientList[iClientH]->m_pItemList[i]->m_sNewEffect4;
		cp += 2;

		sp = (short*)cp;
		*sp = m_pClientList[iClientH]->m_pItemList[i]->m_sItemEffectValue1;
		cp += 2;

		sp = (short*)cp;
		*sp = m_pClientList[iClientH]->m_pItemList[i]->m_sItemEffectValue2;
		cp += 2;

		sp = (short*)cp;
		*sp = m_pClientList[iClientH]->m_pItemList[i]->m_sItemEffectValue3;
		cp += 2;

		sp = (short*)cp;
		*sp = m_pClientList[iClientH]->m_pItemList[i]->m_sItemEffectValue4;
		cp += 2;

		sp = (short*)cp;
		*sp = m_pClientList[iClientH]->m_pItemList[i]->m_sItemEffectValue5;
		cp += 2;

		sp = (short*)cp;
		*sp = m_pClientList[iClientH]->m_pItemList[i]->m_sItemEffectValue6;
		cp += 2;

		sp = (short*)cp;
		*sp = m_pClientList[iClientH]->m_pItemList[i]->m_sItemEffectType;
		cp += 2;

		wp = (UINT16*)cp;
		*wp = m_pClientList[iClientH]->m_pItemList[i]->m_wMaxLifeSpan;
		cp += 2;

		ip = (int*)cp;
		*ip = m_pClientList[iClientH]->m_pItemList[i]->m_iClass;
		cp += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iClientH]->m_pItemList[i]->m_iReqStat;
		cp += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iClientH]->m_pItemList[i]->m_iQuantStat;
		cp += 4;
	}
	iTotalItemB = 0;
	for (i = 0; i < DEF_MAXBANKITEMS; i++)
		if (m_pClientList[iClientH]->m_pItemInBankList[i] != 0)
			iTotalItemB++;

	*cp = iTotalItemB;
	cp++;

	for (i = 0; i < iTotalItemB; i++) {
		if (m_pClientList[iClientH]->m_pItemInBankList[i] == 0) {
			wsprintf(G_cTxt, "RequestInitDataHandler error: Client(%s) Bank-Item(%d)", m_pClientList[iClientH]->m_cCharName, i);
			PutLogFileList(G_cTxt);
			PutLogList(G_cTxt);
			DeleteClient(iClientH, false, true, true, false); // added true (4)
			if (pBuffer != 0) delete[] pBuffer;
			return;
		}
		memcpy(cp, m_pClientList[iClientH]->m_pItemInBankList[i]->m_cName, 20);
		cp += 20;

		dwp = (UINT32*)cp;
		*dwp = m_pClientList[iClientH]->m_pItemInBankList[i]->m_dwCount;
		cp += 4;

		*cp = m_pClientList[iClientH]->m_pItemInBankList[i]->m_cItemType;
		cp++;

		*cp = m_pClientList[iClientH]->m_pItemInBankList[i]->m_cEquipPos;
		cp++;

		sp = (short*)cp;
		*sp = m_pClientList[iClientH]->m_pItemInBankList[i]->m_sLevelLimit;
		cp += 2;

		*cp = m_pClientList[iClientH]->m_pItemInBankList[i]->m_cGenderLimit;
		cp++;

		wp = (UINT16*)cp;
		*wp = m_pClientList[iClientH]->m_pItemInBankList[i]->m_wCurLifeSpan;
		cp += 2;

		wp = (UINT16*)cp;
		*wp = m_pClientList[iClientH]->m_pItemInBankList[i]->m_wWeight;
		cp += 2;

		sp = (short*)cp;
		*sp = m_pClientList[iClientH]->m_pItemInBankList[i]->m_sSprite;
		cp += 2;

		sp = (short*)cp;
		*sp = m_pClientList[iClientH]->m_pItemInBankList[i]->m_sSpriteFrame;
		cp += 2;

		*cp = m_pClientList[iClientH]->m_pItemInBankList[i]->m_cItemColor;
		cp++;

		*cp = (char)m_pClientList[iClientH]->m_pItemInBankList[i]->m_sItemSpecEffectValue2;
		cp++;

		dwp = (UINT32*)cp;
		*dwp = m_pClientList[iClientH]->m_pItemInBankList[i]->m_dwAttribute;
		cp += 4;

		//Magn0S
		sp = (short*)cp;
		*sp = m_pClientList[iClientH]->m_pItemInBankList[i]->m_sNewEffect1;
		cp += 2;

		sp = (short*)cp;
		*sp = m_pClientList[iClientH]->m_pItemInBankList[i]->m_sNewEffect2;
		cp += 2;

		sp = (short*)cp;
		*sp = m_pClientList[iClientH]->m_pItemInBankList[i]->m_sNewEffect3;
		cp += 2;

		sp = (short*)cp;
		*sp = m_pClientList[iClientH]->m_pItemInBankList[i]->m_sNewEffect4;
		cp += 2;

		sp = (short*)cp;
		*sp = m_pClientList[iClientH]->m_pItemInBankList[i]->m_sItemEffectValue1;
		cp += 2;

		sp = (short*)cp;
		*sp = m_pClientList[iClientH]->m_pItemInBankList[i]->m_sItemEffectValue2;
		cp += 2;

		sp = (short*)cp;
		*sp = m_pClientList[iClientH]->m_pItemInBankList[i]->m_sItemEffectValue3;
		cp += 2;

		sp = (short*)cp;
		*sp = m_pClientList[iClientH]->m_pItemInBankList[i]->m_sItemEffectValue4;
		cp += 2;

		sp = (short*)cp;
		*sp = m_pClientList[iClientH]->m_pItemInBankList[i]->m_sItemEffectValue5;
		cp += 2;

		sp = (short*)cp;
		*sp = m_pClientList[iClientH]->m_pItemInBankList[i]->m_sItemEffectValue6;
		cp += 2;

		sp = (short*)cp;
		*sp = m_pClientList[iClientH]->m_pItemInBankList[i]->m_sItemEffectType;
		cp += 2;

		wp = (UINT16*)cp;
		*wp = m_pClientList[iClientH]->m_pItemInBankList[i]->m_wMaxLifeSpan;
		cp += 2;

		ip = (int*)cp;
		*ip = m_pClientList[iClientH]->m_pItemInBankList[i]->m_iClass;
		cp += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iClientH]->m_pItemInBankList[i]->m_iReqStat;
		cp += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iClientH]->m_pItemInBankList[i]->m_iQuantStat;
		cp += 4;
	}

	for (i = 0; i < DEF_MAXMAGICTYPE; i++) {
		*cp = m_pClientList[iClientH]->m_cMagicMastery[i];
		cp++;
	}

	for (i = 0; i < DEF_MAXSKILLTYPE; i++) {
		*cp = m_pClientList[iClientH]->m_cSkillMastery[i];
		cp++;
	}

	//Magn0S:: Added new variables / Centuu - added class
	iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(pBuffer, 7 + iTotalItemA * 80 + iTotalItemB * 79 + DEF_MAXMAGICTYPE + DEF_MAXSKILLTYPE);
	switch (iRet) {
	case DEF_XSOCKEVENT_QUENEFULL:
	case DEF_XSOCKEVENT_SOCKETERROR:
	case DEF_XSOCKEVENT_CRITICALERROR:
	case DEF_XSOCKEVENT_SOCKETCLOSED:
		DeleteClient(iClientH, true, true, true, false); // added true (4)
		PutLogList("HERE 2");
		if (pBuffer != 0) delete[] pBuffer;
		return;
	}

	dwp = (UINT32*)(pBuffer + DEF_INDEX4_MSGID);
	*dwp = MSGID_RESPONSE_INITDATA;
	wp = (UINT16*)(pBuffer + DEF_INDEX2_MSGTYPE);
	*wp = DEF_MSGTYPE_CONFIRM;

	cp = (char*)(pBuffer + DEF_INDEX2_MSGTYPE + 2);

	if (m_pClientList[iClientH]->m_bIsObserverMode == false)
		bGetEmptyPosition(&m_pClientList[iClientH]->m_sX, &m_pClientList[iClientH]->m_sY, m_pClientList[iClientH]->m_cMapIndex);
	else GetMapInitialPoint(m_pClientList[iClientH]->m_cMapIndex, &m_pClientList[iClientH]->m_sX, &m_pClientList[iClientH]->m_sY);

	wp = (UINT16*)cp;
	*wp = iClientH;
	cp += 2;

	sp = (short*)cp;
	*sp = m_pClientList[iClientH]->m_sX - 14 - 5;
	cp += 2;

	sp = (short*)cp;
	*sp = m_pClientList[iClientH]->m_sY - 12 - 5;
	cp += 2;

	sp = (short*)cp;
	*sp = m_pClientList[iClientH]->m_sType;
	cp += 2;

	sp = (short*)cp;
	*sp = m_pClientList[iClientH]->m_sAppr1;
	cp += 2;

	sp = (short*)cp;
	*sp = m_pClientList[iClientH]->m_sAppr2;
	cp += 2;

	sp = (short*)cp;
	*sp = m_pClientList[iClientH]->m_sAppr3;
	cp += 2;

	sp = (short*)cp;
	*sp = m_pClientList[iClientH]->m_sAppr4;
	cp += 2;

	ip = (int*)cp;
	*ip = m_pClientList[iClientH]->m_iApprColor;
	cp += 4;

	ip = (int*)cp;
	*ip = m_pClientList[iClientH]->m_iStatus;
	cp += 4;

	memcpy(cp, m_pClientList[iClientH]->m_cMapName, 10);
	cp += 10;

	memcpy(cp, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, 10);
	cp += 10;

	if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsFixedDayMode == true)
		*cp = 1;
	else *cp = m_cDayOrNight;
	cp++;

	if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsFixedDayMode == true)
		*cp = 0;
	else *cp = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cWhetherStatus;
	cp++;

	ip = (int*)cp;
	*ip = m_pClientList[iClientH]->m_iContribution;
	cp += 4;

	if (m_pClientList[iClientH]->m_bIsObserverMode == false) {
		m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetOwner(iClientH, DEF_OWNERTYPE_PLAYER, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY);
	}

	*cp = (char)m_pClientList[iClientH]->m_bIsObserverMode;
	cp++;

	ip = (int*)cp;
	*ip = m_pClientList[iClientH]->m_iRating;
	cp += 4;

	ip = (int*)cp;
	*ip = m_pClientList[iClientH]->m_iHP;
	cp += 4;

	// new
	if (m_pClientList[iClientH]->m_pIsProcessingAllowed == true) {
		cPoints = 0;
		if ((m_pClientList[iClientH]->m_cSide != m_sLastHeldenianWinner) && (m_sLastHeldenianWinner != 0)) {
			cPoints = 100;
		}
	}
	*cp = cPoints;
	cp++;

	// centu - 800x600
	iSize = iComposeInitMapData(m_pClientList[iClientH]->m_sX - 12, m_pClientList[iClientH]->m_sY - 9, iClientH, cp);
	iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(pBuffer, 66 + iSize); // SephirotH fix - 66 - Centuu: 85?
	switch (iRet) {
	case DEF_XSOCKEVENT_QUENEFULL:
	case DEF_XSOCKEVENT_SOCKETERROR:
	case DEF_XSOCKEVENT_CRITICALERROR:
	case DEF_XSOCKEVENT_SOCKETCLOSED:
		DeleteClient(iClientH, true, true, true, false); // new true (4)
		PutLogList("HERE 3 ");
		if (pBuffer != 0) delete[] pBuffer;
		return;
	}
	SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_LOG, DEF_MSGTYPE_CONFIRM, 0, 0, 0);

	//Magn0S:: Update NPC's Info on Apoc
	if ((m_pClientList[iClientH]->m_bIsOnApocMap == true) && (m_bIsApocalypseMode)) {
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_MONSTERCOUNTAPOC, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_iTotalAliveObject, 0, 0, 0);
	}

	if (m_bIsCrusadeMode)
	{
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_CITYHP, iCityHP[0], iCityHP[1], 0, 0);
	}

	if (m_pClientList[iClientH]->m_iAdminUserLevel > 0)
	{
		
		m_pClientList[iClientH]->m_iStatus = m_pClientList[iClientH]->m_iStatus | 0x00040000;
	}

	
	// new
	if ((m_pClientList[iClientH]->m_iLevel > 49) && (m_pClientList[iClientH]->m_bIsPlayerCivil)) {
		ForceChangePlayMode(iClientH);
	}

	// centu - rep limit
	if (m_pClientList[iClientH]->m_iRating < -5000)
		m_pClientList[iClientH]->m_iRating = -5000;

	if (m_pClientList[iClientH]->m_iRating > 5000)
		m_pClientList[iClientH]->m_iRating = 5000;

	if ((m_pClientList[iClientH]->m_cSide == 1) &&
		(memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, "elvine", 6) == 0) &&
		(m_pClientList[iClientH]->m_iAdminUserLevel == 0)) {
		m_pClientList[iClientH]->m_dwWarBeginTime = timeGetTime();
		m_pClientList[iClientH]->m_bIsWarLocation = true;
		CheckForceRecallTime(iClientH);
	}
	else if ((m_pClientList[iClientH]->m_cSide == 2) &&
		(memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, "aresden", 7) == 0) &&
		(m_pClientList[iClientH]->m_iAdminUserLevel == 0)) {
		m_pClientList[iClientH]->m_dwWarBeginTime = timeGetTime();
		m_pClientList[iClientH]->m_bIsWarLocation = true;
		CheckForceRecallTime(iClientH);
	}
	else if ((m_pClientList[iClientH]->m_cSide == 0) && (m_bIsCrusadeMode == true)) {
		if ((strcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, "elvine") == 0) ||
			(strcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, "aresden") == 0)) {
			if (m_pClientList[iClientH]->m_iAdminUserLevel == 0) {
				m_pClientList[iClientH]->m_bIsWarLocation = true;
				m_pClientList[iClientH]->m_dwWarBeginTime = timeGetTime();
				m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 1;
			}
		}
		if (m_pClientList[iClientH]->m_iAdminUserLevel == 0) {
			m_pClientList[iClientH]->m_bIsWarLocation = false;
			m_pClientList[iClientH]->m_dwWarBeginTime = timeGetTime();
			m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 0;
		}
	}

	iMapSide = iGetMapLocationSide(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName);

	m_pClientList[iClientH]->m_bIsInsideEnemyBuilding = false; // new
	if (m_pClientList[iClientH]->m_cSide != iMapSide && iMapSide != 0) {
		if ((iMapSide <= 2) && (m_pClientList[iClientH]->m_iAdminUserLevel < 1)) {
			if (m_pClientList[iClientH]->m_cSide != 0) {
				m_pClientList[iClientH]->m_dwWarBeginTime = timeGetTime();
				m_pClientList[iClientH]->m_bIsWarLocation = true;
				m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 1;
				m_pClientList[iClientH]->m_bIsInsideEnemyBuilding = true; // new
			}
		}
	}
	else {
		if ((m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsFightZone == true) &&
			(m_iFightzoneNoForceRecall == false) &&
			(m_pClientList[iClientH]->m_iAdminUserLevel == 0)) {
			m_pClientList[iClientH]->m_dwWarBeginTime = timeGetTime();
			m_pClientList[iClientH]->m_bIsWarLocation = true;
			GetLocalTime(&SysTime);
			m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 2 * 60 * 20 - ((SysTime.wHour % 2) * 20 * 60 + SysTime.wMinute * 20) - 2 * 20;
		}
		else {
			if ((memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, "arejail", 7) == 0) ||
				(memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, "elvjail", 7) == 0)) {
				if (m_pClientList[iClientH]->m_iAdminUserLevel == 0) {
					m_pClientList[iClientH]->m_bIsWarLocation = true;
					m_pClientList[iClientH]->m_dwWarBeginTime = timeGetTime();
					if (m_pClientList[iClientH]->m_iTimeLeft_ForceRecall == 0) {
						m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 100;
					}
					else if (m_pClientList[iClientH]->m_iTimeLeft_ForceRecall > 100) {
						m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 100;
					}
				}
			}
		}
	}

	if (m_pClientList[iClientH]->m_iTimeLeft_ForceRecall > 0) {
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_FORCERECALLTIME, m_pClientList[iClientH]->m_iTimeLeft_ForceRecall, 0, 0, 0);
		wsprintf(G_cTxt, "(!) Game Server Force Recall Time  %d (%d)min", m_pClientList[iClientH]->m_iTimeLeft_ForceRecall, m_pClientList[iClientH]->m_iTimeLeft_ForceRecall / 20);
		PutLogList(G_cTxt);
	}

	if ((memcmp(m_pClientList[iClientH]->m_cMapName, "fightzone1", 10) != 0) && (bDeathmatch))
	{
		m_pClientList[iClientH]->m_iDGKills = 0;
		m_pClientList[iClientH]->m_iDGDeaths = 0;
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_DEATHMATCHSTART, 0, 0, 0, 0);
	}
	if ((memcmp(m_pClientList[iClientH]->m_cMapName, "team", 4) != 0) && (c_team->bteam))
		SendAlertMsg(iClientH, "Event Team Arena Enabled");

	if (bShinning)
		SendAlertMsg(iClientH, "Event Shinning Enabled");

	if (m_bHappyHour)
		SendAlertMsg(iClientH, "Happy Hour Activated! Ek x40");

	if (m_bFuryHour)
		SendAlertMsg(iClientH, "Fury Hour Activated! Ek x100");

	if ((memcmp(m_pClientList[iClientH]->m_cMapName, "team", 4) == 0) && (c_team->bteam)) {
		if (m_pClientList[iClientH]->IsLocation("elvine"))
			RequestTeleportHandler(iClientH, "2", "elvine", -1, -1, true);
		else
			RequestTeleportHandler(iClientH, "2", "aresden", -1, -1, true);
	}

	if ((memcmp(m_pClientList[iClientH]->m_cMapName, "fightzone1", 10) == 0) && (bDeathmatch))
	{
		if (m_pClientList[iClientH]->IsLocation("elvine"))
			RequestTeleportHandler(iClientH, "2", "elvine", -1, -1);
		else
			RequestTeleportHandler(iClientH, "2", "aresden", -1, -1);
	}

	SendNotifyMsg(0, iClientH, DEF_NOTIFY_SAFEATTACKMODE, 0, 0, 0, 0);
	SendNotifyMsg(0, iClientH, DEF_NOTIFY_DOWNSKILLINDEXSET, m_pClientList[iClientH]->m_iDownSkillIndex, 0, 0, 0);
	SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMPOSLIST, 0, 0, 0, 0);		
	_SendQuestContents(iClientH);
	_CheckQuestEnvironment(iClientH);

	if (m_pClientList[iClientH]->m_iSpecialAbilityTime == 0) {
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_SPECIALABILITYENABLED, 0, 0, 0, 0);
	}

	if (m_bIsCrusadeMode == true) {
		if (m_pClientList[iClientH]->m_dwCrusadeGUID == 0) {
			m_pClientList[iClientH]->m_iCrusadeDuty			= 0;
			m_pClientList[iClientH]->m_iConstructionPoint	= 0;
			m_pClientList[iClientH]->m_dwCrusadeGUID		= m_dwCrusadeGUID;
		}
		else if (m_pClientList[iClientH]->m_dwCrusadeGUID != m_dwCrusadeGUID) {
			m_pClientList[iClientH]->m_iCrusadeDuty       = 0;
			m_pClientList[iClientH]->m_iConstructionPoint = 0;
			m_pClientList[iClientH]->m_iWarContribution   = 0;
			m_pClientList[iClientH]->m_dwCrusadeGUID = m_dwCrusadeGUID;
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_CRUSADE, (UINT32)m_bIsCrusadeMode, 0, 0, 0, -1);		
		}
		m_pClientList[iClientH]->m_cWarType = 1;
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_CRUSADE, (UINT32)m_bIsCrusadeMode, m_pClientList[iClientH]->m_iCrusadeDuty, 0, 0);
	}
	// Heldenian
	else if (m_bIsHeldenianMode == true) 
	{
		sSummonPoints = 10000;
		if (m_pClientList[iClientH]->m_dwHeldenianGUID == 0) 
		{	m_pClientList[iClientH]->m_dwHeldenianGUID    = m_dwHeldenianGUID;
			m_pClientList[iClientH]->m_iConstructionPoint = sSummonPoints;
			m_pClientList[iClientH]->m_iWarContribution   = 0;
			if (   (m_pMapList[m_pClientList[iClientH]->m_cMapIndex] != 0) 
				&& (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsHeldenianMap == 1))
			{	// Player connecting on Heldenian map, and not been advised of Heldenian start will recall
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_0BE8, 0 , 0, 0, 0);	
				m_pClientList[iClientH]->m_bIsWarLocation = true;
				m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 3;
			}
		}else if (m_pClientList[iClientH]->m_dwHeldenianGUID != m_dwHeldenianGUID) 
		{	m_pClientList[iClientH]->m_dwHeldenianGUID    = m_dwHeldenianGUID;
			m_pClientList[iClientH]->m_iConstructionPoint = sSummonPoints;
			m_pClientList[iClientH]->m_iWarContribution   = 0;
			if (   (m_pMapList[m_pClientList[iClientH]->m_cMapIndex] != 0) 
				&& (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsHeldenianMap == 1))
			{	// Player connecting on Heldenian map, and not been advised of this Heldenian start will recall
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_0BE8, 0 , 0, 0, 0);	
				m_pClientList[iClientH]->m_bIsWarLocation = true;
				m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 3;
		}	}
		if (m_cHeldenianWinner == -1) 
		{	if (m_bHeldenianWarInitiated == true) 
			{	SendNotifyMsg(0, iClientH, DEF_NOTIFY_HELDENIANSTART, 0, 0, 0, 0);
			}else
			{	SendNotifyMsg(0, iClientH, DEF_NOTIFY_HELDENIANTELEPORT, 0, 0, 0, 0);
			}
		}else // Force recall loosers
		{	if (   (m_pClientList[iClientH]->m_cSide != m_cHeldenianWinner)
				&& (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsHeldenianMap == 1))
			{	m_pClientList[iClientH]->m_bIsWarLocation = true;
				m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 2;
		}	}
		m_pClientList[iClientH]->m_cWarType = 2;// character have been informed of heldenian starting...
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_CONSTRUCTIONPOINT, m_pClientList[iClientH]->m_iConstructionPoint, m_pClientList[iClientH]->m_iWarContribution, 1, 0);
		UpdateHeldenianStatus(iClientH);
	}
	else if ((m_pClientList[iClientH]->m_cWarType == 1) && (m_pClientList[iClientH]->m_dwCrusadeGUID == m_dwCrusadeGUID)) {
		m_pClientList[iClientH]->m_iCrusadeDuty = 0;
		m_pClientList[iClientH]->m_iConstructionPoint = 0;
	}
	else if ((m_pClientList[iClientH]->m_dwCrusadeGUID != 0) && (m_pClientList[iClientH]->m_cWarType == 1)) {
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_CRUSADE, (UINT32)m_bIsCrusadeMode, 0, 0, 0, -1);
	}
	// The Heldenian he has participated is over...
	else if ( (m_pClientList[iClientH]->m_dwHeldenianGUID == m_dwHeldenianGUID)
			&& (m_pClientList[iClientH]->m_dwHeldenianGUID != 0)) 
	{	m_pClientList[iClientH]->m_iConstructionPoint = 0;
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_HELDENIANEND, 0, 0, 0, 0);
		m_pClientList[iClientH]->m_cWarType = 0; // Character is informed of Hedenian end... so he'll get XP later
	}
	else {
		m_pClientList[iClientH]->m_iWarContribution = 0; 
		m_pClientList[iClientH]->m_dwCrusadeGUID = 0;
		m_pClientList[iClientH]->m_iConstructionPoint = 0;
	}

	SendNotifyMsg(0, iClientH, DEF_NOTIFY_CONSTRUCTIONPOINT, m_pClientList[iClientH]->m_iConstructionPoint, m_pClientList[iClientH]->m_iWarContribution, 1, 0);
	SendNotifyMsg(0, iClientH, DEF_NOTIFY_GIZONITEMUPGRADELEFT, m_pClientList[iClientH]->m_iGizonItemUpgradeLeft, 0, 0, 0);
	
	//50Cent - Critical Count Login Fix
	SendNotifyMsg(0, iClientH, DEF_NOTIFY_SUPERATTACKLEFT, 0, 0, 0, 0);

	// centu - sends max stats to client, when log in
	SendNotifyMsg(0, iClientH, DEF_MAX_STATS, m_sCharStatLimit, 0, 0, 0);
	// centu - sends max level to client, when log in
	SendNotifyMsg(0, iClientH, DEF_MAX_LEVEL, m_iPlayerMaxLevel, 0, 0, 0);

	// SNOOPY: Send gate positions if applicable.
	Notify_ApocalypseGateState(iClientH);

	//Magn0S:: Update all events for Clients
	SendNotifyMsg(0, iClientH, DEF_NOTIFY_EVENTUPDATER, (int)bDeathmatch, (int)m_bIsCrusadeMode, (int)m_bIsApocalypseMode, 0, (int)m_bIsHeldenianMode, (int)m_bIsCTFEvent);


	minimap_update(iClientH);
	minimap_update_mark(iClientH);

	NotifyMapRestrictions(iClientH, false);

	if (m_bIsCTFEvent) UpdateEventStatus();
}

/*********************************************************************************************************************
**  void CGame::DeleteClient(int iClientH, bool bSave, bool bNotify, bool bCountLogout, bool bForceCloseConn)		**
**  DESCRIPTION			:: manages disconnect player																**
**  LAST_UPDATED		:: March 16, 2005; 12:02 AM; Hypnotoad														**
**	RETURN_VALUE		:: void																						**
**  NOTES				:: n/a																						**
**	MODIFICATION		:: n/a																						**
**********************************************************************************************************************/
void CGame::DeleteClient(int iClientH, bool bSave, bool bNotify, bool bCountLogout, bool bForceCloseConn)
{
	int i, iExH;
	char * cp, cData[120], cTmpMap[30];
	UINT32 * dwp;
	UINT16 * wp;

	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == true) {

		// Capture the Flag
		if (m_bIsCTFEvent && (m_pClientList[iClientH]->m_iStatus & 0x80000) != 0) {
			switch (m_pClientList[iClientH]->m_cSide) {
			case 1:
				m_iCTFEventFlag[1] = iAddDynamicObjectList(0, 0, DEF_DYNAMICOBJECT_ELVINEFLAG2, m_iElvineMapIndex, 151, 132, 0, 0);
				break;
			case 2:
				m_iCTFEventFlag[0] = iAddDynamicObjectList(0, 0, DEF_DYNAMICOBJECT_ARESDENFLAG2, m_iAresdenMapIndex, 151, 128, 0, 0);
				break;
			}

			m_iCTFEventFlagHolder[m_pClientList[iClientH]->m_cSide - 1] = -1;

			for (i = 1; i < DEF_MAXCLIENTS; i++)
				if (i != iClientH && m_pClientList[i] != 0 && m_pClientList[i]->m_cSide != 0) SendNotifyMsg(0, i, DEF_NOTIFY_EVENT, 0, 8 + m_pClientList[iClientH]->m_cSide - 1, 0, 0);
		}

		//Magn0S:: Arena Gladiator
		if (memcmp(m_pClientList[iClientH]->m_cMapName, "fightzone1", 10) == 0)
		{
			RequestArenaStatus(iClientH, false);
			m_pClientList[iClientH]->m_iDGKills = 0;
			m_pClientList[iClientH]->m_iDGDeaths = 0;
		}

		if (m_pClientList[iClientH]->m_cExchangeMode != 0) {
			iExH = m_pClientList[iClientH]->m_iExchangeH;
			_ClearExchangeStatus(iExH);
			_ClearExchangeStatus(iClientH);
		}

		if ((m_pClientList[iClientH]->m_iAllocatedFish != 0) && (m_pFish[m_pClientList[iClientH]->m_iAllocatedFish] != 0)) 
			m_pFish[m_pClientList[iClientH]->m_iAllocatedFish]->m_sEngagingCount--;

		if (bNotify == true) 
			SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_LOG, DEF_MSGTYPE_REJECT, 0, 0, 0);
		
		RemoveFromTarget(iClientH, DEF_OWNERTYPE_PLAYER);

		minimap_clear(iClientH);

		for (i = 1; i < DEF_MAXCLIENTS; i++) 
		if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_iWhisperPlayerIndex	== iClientH)) {
			m_pClientList[i]->m_iWhisperPlayerIndex = -1;
			SendNotifyMsg(0, i, DEF_NOTIFY_WHISPERMODEOFF, 0, 0, 0, m_pClientList[iClientH]->m_cCharName);
		}

		ZeroMemory(cData, sizeof(cData));
		cp = (char *)cData;
		*cp = GSM_DISCONNECT;
		cp++;
		memcpy(cp, m_pClientList[iClientH]->m_cCharName, 10);
		cp += 10;
		bStockMsgToGateServer(cData, 11);

		m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->ClearOwner(2, iClientH, DEF_OWNERTYPE_PLAYER,
				m_pClientList[iClientH]->m_sX, 
				m_pClientList[iClientH]->m_sY);

		bRemoveFromDelayEventList(iClientH, DEF_OWNERTYPE_PLAYER, 0);
	}

	if ((bSave == true) && (m_pClientList[iClientH]->m_bIsOnServerChange == false)) {
		
		if (m_pClientList[iClientH]->m_bIsKilled == true) {
			m_pClientList[iClientH]->m_sX = -1;
			m_pClientList[iClientH]->m_sY = -1;

			strcpy(cTmpMap,m_pClientList[iClientH]->m_cMapName) ;

			ZeroMemory(m_pClientList[iClientH]->m_cMapName, sizeof(m_pClientList[iClientH]->m_cMapName));

			if (m_pClientList[iClientH]->m_cSide == 0) {
				strcpy(m_pClientList[iClientH]->m_cMapName, "default");
			}
			else {
				if (memcmp(m_pClientList[iClientH]->m_cLocation, "are", 3) == 0) {
					if (m_bIsCrusadeMode == true) {
						if (m_pClientList[iClientH]->m_iDeadPenaltyTime > 0) {
							ZeroMemory(m_pClientList[iClientH]->m_cLockedMapName, sizeof(m_pClientList[iClientH]->m_cLockedMapName));
							strcpy(m_pClientList[iClientH]->m_cLockedMapName, "aresden");
							m_pClientList[iClientH]->m_iLockedMapTime = 60*5;
							m_pClientList[iClientH]->m_iDeadPenaltyTime = 60*10;
						}
						else {
							m_pClientList[iClientH]->m_iDeadPenaltyTime = 60*10;
						}
					}

					if (strcmp(cTmpMap, "elvine") == 0){
						strcpy(m_pClientList[iClientH]->m_cLockedMapName, "elvjail");
						m_pClientList[iClientH]->m_iLockedMapTime = m_sJailTime;
						memcpy(m_pClientList[iClientH]->m_cMapName, "elvjail", 7);
					}else memcpy(m_pClientList[iClientH]->m_cMapName, "cityhall_1", 10);
				}
				else {
					if (m_bIsCrusadeMode == true) {
						if (m_pClientList[iClientH]->m_iDeadPenaltyTime > 0) {
							ZeroMemory(m_pClientList[iClientH]->m_cLockedMapName, sizeof(m_pClientList[iClientH]->m_cLockedMapName));
							strcpy(m_pClientList[iClientH]->m_cLockedMapName, "elvine");
							m_pClientList[iClientH]->m_iLockedMapTime = 60*5;
							m_pClientList[iClientH]->m_iDeadPenaltyTime = 60*10;
						}
						else {
							m_pClientList[iClientH]->m_iDeadPenaltyTime = 60*10;
						}
					}
					if (strcmp(cTmpMap, "aresden") == 0){
						strcpy(m_pClientList[iClientH]->m_cLockedMapName, "arejail");
						m_pClientList[iClientH]->m_iLockedMapTime = m_sJailTime;
						memcpy(m_pClientList[iClientH]->m_cMapName, "arejail", 7);

					}else memcpy(m_pClientList[iClientH]->m_cMapName, "cityhall_2", 10);
				}
			}
		}
		else if (bForceCloseConn == true) {
			ZeroMemory(m_pClientList[iClientH]->m_cMapName, sizeof(m_pClientList[iClientH]->m_cMapName));
			memcpy(m_pClientList[iClientH]->m_cMapName, "bisle", 5);
			m_pClientList[iClientH]->m_sX = -1;
			m_pClientList[iClientH]->m_sY = -1;

			ZeroMemory(m_pClientList[iClientH]->m_cLockedMapName, sizeof(m_pClientList[iClientH]->m_cLockedMapName));
			strcpy(m_pClientList[iClientH]->m_cLockedMapName, "bisle");
			m_pClientList[iClientH]->m_iLockedMapTime = 10*60;
		}

		if (m_pClientList[iClientH]->m_bIsObserverMode == true) {
			ZeroMemory(m_pClientList[iClientH]->m_cMapName, sizeof(m_pClientList[iClientH]->m_cMapName));
			if (m_pClientList[iClientH]->m_cSide == 0) {
				switch (iDice(1,2)) {
				case 1:
					memcpy(m_pClientList[iClientH]->m_cMapName, "aresden", 7);
					break;
				case 2:
					memcpy(m_pClientList[iClientH]->m_cMapName, "elvine", 6);
					break;
				}
			}
			else {
				memcpy(m_pClientList[iClientH]->m_cMapName, m_pClientList[iClientH]->m_cLocation, 10);
			}
			m_pClientList[iClientH]->m_sX = -1;
			m_pClientList[iClientH]->m_sY = -1;
		}

		if (m_pClientList[iClientH]->m_bIsInitComplete == true) {

			if (m_pClientList[iClientH]->m_iPartyID != 0) {
				ZeroMemory(cData, sizeof(cData));
				cp = (char *)cData;

				dwp = (UINT32 *)cp;
				*dwp = MSGID_PARTYOPERATION;
				cp += 4;

				wp = (UINT16*)cp;
				*wp = 4;
				cp += 2;

				wp = (UINT16 *)cp;
				*wp = iClientH;
				cp += 2;

				memcpy(cp, m_pClientList[iClientH]->m_cCharName, 10);
				cp += 10;

				wp = (UINT16 *)cp;
				*wp = m_pClientList[iClientH]->m_iPartyID;
				cp += 2;
		
				SendMsgToGateServer(MSGID_PARTYOPERATION, iClientH, cData);
			}
			if (bSendMsgToLS(MSGID_REQUEST_SAVEPLAYERDATALOGOUT, iClientH, bCountLogout) == FALSE) LocalSavePlayerData(iClientH);
		}
		else bSendMsgToLS(MSGID_REQUEST_SAVEPLAYERDATALOGOUT, iClientH, bCountLogout);
	}
	else {
		if (m_pClientList[iClientH]->m_bIsOnServerChange == false) {
			if (m_pClientList[iClientH]->m_iPartyID != 0) {
				ZeroMemory(cData, sizeof(cData));
				cp = (char *)cData;

				dwp = (UINT32 *)cp;
				*dwp = MSGID_PARTYOPERATION;
				cp += 4;

				wp = (UINT16*)cp;
				*wp = 4;
				cp += 2;

				wp = (UINT16 *)cp;
				*wp = iClientH;
				cp += 2;

				memcpy(cp, m_pClientList[iClientH]->m_cCharName, 10);
				cp += 10;

				wp = (UINT16 *)cp;
				*wp = m_pClientList[iClientH]->m_iPartyID;
				cp += 2;

				SendMsgToGateServer(MSGID_PARTYOPERATION, iClientH, cData);
			}

			bSendMsgToLS(MSGID_REQUEST_SAVEPLAYERDATALOGOUT, iClientH, bCountLogout);
		}
		else {
			if (m_pClientList[iClientH]->m_iPartyID != 0) {
				ZeroMemory(cData, sizeof(cData));
				cp = (char *)cData;

				dwp = (UINT32 *)cp;
				*dwp = MSGID_PARTYOPERATION;
				cp += 4;

				wp = (UINT16*)cp;
				*wp = 7;
				cp += 2;

				wp = (UINT16 *)cp;
				*wp = 0;
				cp += 2;

				memcpy(cp, m_pClientList[iClientH]->m_cCharName, 10);
				cp += 10;

				wp = (UINT16 *)cp;
				*wp = m_pClientList[iClientH]->m_iPartyID;
				cp += 2;
			
				SendMsgToGateServer(MSGID_PARTYOPERATION, iClientH, cData);
			}

			bSendMsgToLS(MSGID_REQUEST_SETACCOUNTWAITSTATUS, iClientH, false); 
		}
	}

	if (m_pClientList[iClientH]->m_iPartyID != 0) {
		for (i = 0; i < DEF_MAXPARTYMEMBERS; i++) {
			if (m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iIndex[i] == iClientH) {
				m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iIndex[i] = 0;
				m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iTotalMembers--;
				wsprintf(G_cTxt, "PartyID:%d member:%d Out(Delete) Total:%d", m_pClientList[iClientH]->m_iPartyID, iClientH, m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iTotalMembers);
				PutLogList(G_cTxt);
				break;
			}
		}
		for (i = 0; i < DEF_MAXPARTYMEMBERS - 1; i++) {
			if ((m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iIndex[i] == 0) && (m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iIndex[i + 1] != 0)) {
				m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iIndex[i] = m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iIndex[i + 1];
				m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iIndex[i + 1] = 0;
			}
		}
	}

	m_iTotalClients--;

	delete m_pClientList[iClientH];
	m_pClientList[iClientH] = 0;

	RemoveClientShortCut(iClientH);
}

/*********************************************************************************************************************
**  void CGame::OnTimer(char cType)																					**
**  DESCRIPTION			:: events occuring automatically after a set period of time									**
**  LAST_UPDATED		:: March 17, 2005; 2:24 PM; Hypnotoad														**
**	RETURN_VALUE		:: void																						**
**  NOTES				::	- added "&& (m_bIsWLServerAvailable == true)" to list of Send Messages <commented>		**
**							- changed m_iSubLogSockInitIndex location												**
**							- added new functions ApocalypseWarStarter(), AutomaticApocalypseEnd(),					**
**							DoAbaddonThunderDamageHandler(), HeldenianWarStarter(), AutomaticHeldenianEnd(),		**
**							AutomatedHeldenianTimer()																**
**	MODIFICATION		::  n/a			 																			**
**********************************************************************************************************************/
void CGame::OnTimer(char cType)
{
 UINT32 dwTime = timeGetTime();
 int i;
	NpcProcess();
	MsgProcess();
	c_team->TeamTimer();

	// Centuu - 3000 -> 1000
	if ((dwTime - m_dwGameTime2) > 1000) {
		CheckClientResponseTime();
		SendMsgToGateServer(MSGID_GAMESERVERALIVE, 0, 0);
		CheckDayOrNightMode();
		InvalidateRect(G_hWnd, 0, true);
		_CheckGateSockConnection();
		if ((m_bIsGameStarted == false)     && (m_bIsItemAvailable == true)      && 
			(m_bIsNpcAvailable == true)     && (m_bIsGateSockAvailable == true)  &&
			(m_bIsLogSockAvailable == true) && (m_bIsMagicAvailable == true)     &&
			(m_bIsSkillAvailable == true)   && (m_bIsPotionAvailable == true)   &&
			(m_bIsQuestAvailable == true)   && (m_bIsBuildItemAvailable == true) && 
			(m_iSubLogSockActiveCount == DEF_MAXSUBLOGSOCK)) { 
			PutLogList("Sending start message...");
			SendMessage(m_hWnd, WM_USER_STARTGAMESIGNAL, 0, 0);
			m_bIsGameStarted = true;
		}
		m_dwGameTime2 = dwTime;
	}
	if ((dwTime - m_dwGameTime6) > 1000) {
		DelayEventProcessor();
		SendStockMsgToGateServer();
		if (m_iFinalShutdownCount != 0) {
			m_iFinalShutdownCount--;
			wsprintf(G_cTxt, "Final Shutdown...%d", m_iFinalShutdownCount);
			PutLogList(G_cTxt);
			if (m_iFinalShutdownCount <= 1) {
				SendMessage(m_hWnd, WM_CLOSE, 0, 0);
				return;
				
			}
		}
		// new location
		if (m_iSubLogSockInitIndex < DEF_MAXSUBLOGSOCK) {
			m_pSubLogSock[m_iSubLogSockInitIndex] = new class XSocket(m_hWnd, DEF_SERVERSOCKETBLOCKLIMIT);
			m_pSubLogSock[m_iSubLogSockInitIndex]->bConnect(m_cLogServerAddr, m_iLogServerPort, (WM_ONLOGSOCKETEVENT + m_iSubLogSockInitIndex + 1));
			m_pSubLogSock[m_iSubLogSockInitIndex]->bInitBufferSize(DEF_MSGBUFFERSIZE);
			wsprintf(G_cTxt, "(!) Try to connect sub-log-socket(%d)... Addr:%s  Port:%d", m_iSubLogSockInitIndex, m_cLogServerAddr, m_iLogServerPort);
			PutLogList(G_cTxt);
			m_iSubLogSockInitIndex++;
		}
		m_dwGameTime6 = dwTime; // moved
	}
	// Centuu - 3000 -> 1000
	if ((dwTime - m_dwGameTime3) > 1000) {
		SyncMiddlelandMapInfo();
		CheckDynamicObjectList();
		DynamicObjectEffectProcessor();
		NoticeHandler();

		DeleteMark();
		if (m_bIsCTFEvent || bDeathmatch || c_team->bteam || bShinning || _drop_inhib || _candy_boost ||
			_revelation || _city_teleport || m_bHappyHour || m_bFuryHour)
		{
			EventEnd();
		}

		//SNOOPY:Check if Apocalypse Gate nedd to be closed
		OpenCloseApocalypseGate();
		m_dwGameTime3 = dwTime;
	}

	if (m_iNotifyCleanMap == true)
	{	
		if ((dwTime - m_dwCleanTime) > (m_iClearMapTime-30)*1000) // 30 sec
		{	
			for (i = 1; i < DEF_MAXCLIENTS; i++) 
			{	if (m_pClientList[i] != 0) 
				{	
					SendNotifyMsg(0, i, DEF_NOTIFY_IPACCOUNTINFO, 0, 0, 0, "The maps will be cleaned in 30 seconds. Get drops!");					
				}
				m_iNotifyCleanMap = false;
			}	
		}
	}
	
	if ((dwTime - m_dwCleanTime) > m_iClearMapTime*1000) // 2700*1000 = 45 min 1800*1000 = 30 min 3600*1000 = 1h	
	{	ClearMap();
		for (i = 1; i < DEF_MAXCLIENTS; i++)
		{
			if (m_pClientList[i] != 0)
			{
				SendNotifyMsg(0, i, DEF_NOTIFY_IPACCOUNTINFO, 0, 0, 0, "Maps cleaned!");
			}
		}
		m_dwCleanTime = dwTime;
	}

	// Centuu - 600 -> 1000
	if ((dwTime - m_dwGameTime4) > 1000) { // time millisecond = *60*10*10
		MobGenerator();

		m_dwGameTime4 = dwTime;
	}
	if ((dwTime - m_dwGameTime5) > 180000) {
		m_dwGameTime5 = dwTime;
		srand((unsigned)time(0));   
	}
	// Centuu - 4000 -> 1000
	if ((dwTime - m_dwFishTime) > 1000) {

		SendCollectedMana();
		if (m_bIsCrusadeMode) CrusadeEnd();
		if ((m_bIsCrusadeMode == false) && (m_bIsCrusadeWarStarter == true)) CrusadeWarStarter();
		if ((m_bIsApocalypseStarter == true) && (m_bIsApocalypseMode == false)) ApocalypseStarter();
		if ((m_bIsHeldenianReady == true) && (m_bIsHeldenianMode == false)) HeldenianWarStarter();

		//SNOOPY: check if Heldenian war must be started or finished.
		if (m_bIsHeldenianMode == true)		  
		{	HeldenianStartWarNow();
			HeldenianEndWarNow();
		}

		if (!m_bIsCTFEvent)	CaptureTheFlagStarter();
		if (!bDeathmatch) DeathmatchStarter();
		if (!c_team->bteam) TeamArenaStarter();
		if (!bShinning)		ShinningStarter();
		if (!_city_teleport) InvasionStarter();
		if (!_revelation) BeholderStarter();
		if (!_candy_boost) CandyStarter();
		if (!_drop_inhib) BagProtectionStarter();
		if (!m_bHappyHour) HappyHourStarter();
		if (!m_bFuryHour) FuryHourStarter();
		m_dwFishTime = dwTime;
	}
	
	if ((dwTime - m_dwWhetherTime) > 20000) {
		WeatherProcessor();
		m_dwWhetherTime = dwTime;
	}
	
	if ((dwTime - m_dwCanFightzoneReserveTime) > 7200000) {
		m_dwCanFightzoneReserveTime = dwTime;
	}

	if ((m_bIsServerShutdowned == false) && (m_bOnExitProcess == true) && ((dwTime - m_dwExitProcessTime) > 1000*2)) {
		if (_iForcePlayerDisconect(15) == 0) {
			PutLogList("(!) GAME SERVER SHUTDOWN PROCESS COMPLETED! All players are disconnected.");
			m_bIsServerShutdowned = true;
			if ((m_cShutDownCode == 3) || (m_cShutDownCode == 4)) {
				PutLogFileList("(!!!) AUTO-SERVER-REBOOTING!");
				bInit();
				m_iAutoRebootingCount++;
			}
			else {
				if (m_iFinalShutdownCount == 0)	m_iFinalShutdownCount = 20;
			}
		}
		m_dwExitProcessTime = dwTime;
	}

	if ((dwTime - m_dwMapSectorInfoTime) > 10000) {
		UpdateMapSectorInfo();
		MineralGenerator();
		m_iMapSectorInfoUpdateCount++;
		if (m_iMapSectorInfoUpdateCount >= 5) {
			AgingMapSectorInfo();
			m_iMapSectorInfoUpdateCount = 0;
		}
		m_dwMapSectorInfoTime = dwTime;
	}
}

void CGame::DeleteMark()
{
	UINT32 dwTime = timeGetTime();
	for (int i = 0; i < DEF_MAXGUILDS; i++)
	{
		if (string(m_stGuildInfo[i].cGuildName) == "NONE") continue;
		if (dwTime < m_stGuildInfo[i].dwMarkTime) continue;
		m_stGuildInfo[i].markX = -1;
		m_stGuildInfo[i].markY = -1;
		strcpy(m_stGuildInfo[i].cGuildName, "NONE");
		m_stGuildInfo[i].dwMarkTime = 0;
		ZeroMemory(m_stGuildInfo[i].cMap, sizeof(m_stGuildInfo[i].cMap));
	}
}

// Centu - Crusade end in 1h30
void CGame::CrusadeEnd()
{
	UINT32 dwTime = timeGetTime();
	if (dwTime < dwCrusadeFinishTime) return;
	ManualEndCrusadeMode(0);
}

void CGame::EventEnd()
{
	UINT32 dwTime = timeGetTime();
	if (dwTime < dwEventFinishTime) return;

	if (m_bIsCTFEvent)	AdminOrder_SetEvent();
	if (bDeathmatch)	GlobalEndGladiatorArena();
	
	if (c_team->bteam) 
	{
		c_team->bteam = false;
		dwEventFinishTime = 0;
		c_team->DisableEvent();
		NotifyEvents();
	}
	
	if (bShinning)		ManageShinning();
	if (_city_teleport) CityTeleport();

	if (_revelation)
	{
		dwEventFinishTime = 0;
		_revelation = false;
		NotifyEvents();
	}
	if (_candy_boost)
	{
		dwEventFinishTime = 0;
		_candy_boost = false;
		NotifyEvents();
	}
	if (_drop_inhib)
	{
		dwEventFinishTime = 0;
		_drop_inhib = false;
		NotifyEvents();
	}

	if (m_bHappyHour) ManageHappyHour();
	if (m_bFuryHour) ManageFuryHour();
}


/*********************************************************************************************************************
**  void CGame::OnTimer(char cType)																					**
**  DESCRIPTION			:: events occuring indefinately while a player is connected									**
**  LAST_UPDATED		:: March 17, 2005; 2:24 PM; Hypnotoad														**
**	RETURN_VALUE		:: void																						**
**  NOTES				::	- changed autosave message to log server's bFlag to true								**
**							- increased exp gained in autoexp. now level rather than level/2						**
**							- recall player if crusade is false and is a civilian inside a combatant only zone		**
**							- simplified alot of tasks to improve serverside latency								**
**	MODIFICATION		::  - includes link to _bCheckCharacterData to check for inconsistancies (see that function)**
**********************************************************************************************************************/
void CGame::CheckClientResponseTime()
{
 int i, iPlusTime, iMaxSuperAttack;
 short sTemp;
 UINT32 dwTime;
 short sItemIndex;
 int iDir;

	 if (m_bIsGameStarted)
	 {
		 srvMinute++;
		 if (srvMinute == 60)
		 {
			 srvMinute = 0;
			 srvHour++;
			 if (srvHour == 24) srvHour = 0;
		 }
	 }

	dwTime = timeGetTime();
	for (i = 1; i < DEF_MAXCLIENTS; i++) {
		if (m_pClientList[i] != 0) {		
			if ((dwTime - m_pClientList[i]->m_dwTime) > DEF_CLIENTTIMEOUT ) {
				if (m_pClientList[i]->m_bIsInitComplete == true) {
					wsprintf(G_cTxt, "Client Timeout: %s", m_pClientList[i]->m_cIPaddress);
					PutLogList(G_cTxt);
					DeleteClient(i, true, true, true, false);
				}
				else if ((dwTime - m_pClientList[i]->m_dwTime) > DEF_CLIENTTIMEOUT) {
					DeleteClient(i, false, false, true, false);
				}
			}
			else if (m_pClientList[i]->m_bIsInitComplete == true) {

				if (string(m_pClientList[i]->m_cMapName) == "team" && !c_team->bteam)
				{
					if (m_pClientList[i]->IsLocation("elvine"))
						RequestTeleportHandler(i, "2", "elvine", -1, -1, true);
					else
						RequestTeleportHandler(i, "2", "aresden", -1, -1, true);
				}

				CheckDestroyFragileItem(i);

				if (ObtenerX(m_pClientList[i]->m_cGuildName) == -1 && ObtenerY(m_pClientList[i]->m_cGuildName) == -1)
				{
					minimap_clear_mark(i);
				}

				m_pClientList[i]->m_iTimeLeft_ShutUp--;
				if (m_pClientList[i]->m_iTimeLeft_ShutUp < 0) m_pClientList[i]->m_iTimeLeft_ShutUp = 0;
				m_pClientList[i]->m_iTimeLeft_Rating--;
				if (m_pClientList[i]->m_iTimeLeft_Rating < 0) m_pClientList[i]->m_iTimeLeft_Rating = 0;
				
				if (((dwTime - m_pClientList[i]->m_dwHungerTime) > DEF_HUNGERTIME) && (m_pClientList[i]->m_bIsKilled == false)) {
					if ((m_pClientList[i]->m_iLevel > DEF_LEVELLIMIT) && (m_pClientList[i]->m_iAdminUserLevel < 1 )) 
					
					{
						m_pClientList[i]->m_iHungerStatus--;
						SendNotifyMsg(0, i, DEF_NOTIFY_HUNGER, m_pClientList[i]->m_iHungerStatus, 0, 0, 0); // MORLA2 - Muestra el hunger status
					}
					if (m_pClientList[i]->m_iHungerStatus <= 0) m_pClientList[i]->m_iHungerStatus = 0;
					m_pClientList[i]->m_dwHungerTime = dwTime;
					
					if ( (m_pClientList[i]->m_iHP > 0) && (m_pClientList[i]->m_iHungerStatus < 100) ) { // MORLA2 - Para que informe el hunger constantemente
						SendNotifyMsg(0, i, DEF_NOTIFY_HUNGER, m_pClientList[i]->m_iHungerStatus, 0, 0, 0);
					}
				}

				/*****MODIFICATION*****/
				// checks all player data for general and packet errors
				if (_bCheckCharacterData(i) == false) {
					DeleteClient(i, true, true);
					break;	
				}
				/*********END**********/
				// Snoopy added Tile where you can't stay (behind crusade structures)
				if (   (m_bIsCrusadeMode == true)
					&& (m_pClientList[i]->m_iAdminUserLevel == 0)
					&& (m_pMapList[m_pClientList[i]->m_cMapIndex]->bGetIsStayAllowedTile(m_pClientList[i]->m_sX, m_pClientList[i]->m_sY) == false))
				{	iDir = iDice(1,8);
					if (iDir == 5) iDir++;
					int iDamage = m_pClientList[i]->m_iHP/8 + iDice(1,20);					
					if (iDamage < 40) iDamage = 30 + iDice(1,20);
					if (iDamage > m_pClientList[i]->m_iHP) iDamage = m_pClientList[i]->m_iHP -1;
					m_pClientList[i]->m_iHP -= iDamage;					
					m_pClientList[i]->m_iLastDamage = iDamage;		
					m_pClientList[i]->m_dwRecentAttackTime = dwTime;
					SendNotifyMsg(0, i, DEF_NOTIFY_DAMAGEMOVE, iDir, iDamage, 0, 0);
				}
				// Centu - can't stay in flag position
				if ((m_bIsCTFEvent)
					&& (m_pClientList[i]->m_iAdminUserLevel == 0)
					&& (m_pMapList[m_pClientList[i]->m_cMapIndex]->bGetIsStayAllowedTile(m_pClientList[i]->m_sX, m_pClientList[i]->m_sY) == false))
				{
					if (m_pClientList[i]->m_cMapIndex == m_iAresdenMapIndex && m_pClientList[i]->m_cSide == 1 ||
						m_pClientList[i]->m_cMapIndex == m_iElvineMapIndex && m_pClientList[i]->m_cSide == 2) {
						iDir = iDice(1, 8);

						int iDamage = m_pClientList[i]->m_iHP / 8 + iDice(1, 20);
						if (iDamage < 40) iDamage = 30 + iDice(1, 20);
						if (iDamage > m_pClientList[i]->m_iHP) iDamage = m_pClientList[i]->m_iHP - 1;
						m_pClientList[i]->m_iHP -= iDamage;
						m_pClientList[i]->m_iLastDamage = iDamage;
						m_pClientList[i]->m_dwRecentAttackTime = dwTime;
						SendNotifyMsg(0, i, DEF_NOTIFY_DAMAGEMOVE, iDir, iDamage, 0, 0);
					}
				}
				if ((m_pClientList[i]->m_iHungerStatus <= 40) && (m_pClientList[i]->m_iHungerStatus >= 0)) 
					iPlusTime = (40 - m_pClientList[i]->m_iHungerStatus)*1000;
				else iPlusTime = 0;
				iPlusTime = abs(iPlusTime);
				if ((dwTime - m_pClientList[i]->m_dwHPTime) > (UINT32)(DEF_HPUPTIME + iPlusTime)) {
					TimeHitPointsUp(i);
					m_pClientList[i]->m_dwHPTime = dwTime;
				}
				if ((dwTime - m_pClientList[i]->m_dwMPTime) > (UINT32)(DEF_MPUPTIME + iPlusTime)) {
					TimeManaPointsUp(i);
					m_pClientList[i]->m_dwMPTime = dwTime;
				}
				if ((dwTime - m_pClientList[i]->m_dwSPTime) > (UINT32)(DEF_SPUPTIME + iPlusTime)) {
					TimeStaminarPointsUp(i);
					m_pClientList[i]->m_dwSPTime = dwTime;
				}
				if ((m_pClientList[i]->m_bIsPoisoned == true) && ((dwTime - m_pClientList[i]->m_dwPoisonTime) > DEF_POISONTIME)) {
					PoisonEffect(i, 0);
					m_pClientList[i]->m_dwPoisonTime = dwTime;
				}

				// Capture the Flag
				if (m_bIsCTFEvent && (m_pClientList[i]->m_iStatus & 0x80000) != 0 && (dwTime - m_pClientList[i]->m_dwCTFHolderTime) > 10 * 60 * 1000) {
					m_pClientList[i]->m_iStatus ^= 0x80000;

					SendEventToNearClient_TypeA(i, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);

					switch (m_pClientList[i]->m_cSide) {
					case 1:
						m_iCTFEventFlag[1] = iAddDynamicObjectList(0, 0, DEF_DYNAMICOBJECT_ELVINEFLAG2, m_iElvineMapIndex, 151, 132, 0, 0);
						break;
					case 2:
						m_iCTFEventFlag[0] = iAddDynamicObjectList(0, 0, DEF_DYNAMICOBJECT_ARESDENFLAG2, m_iAresdenMapIndex, 151, 128, 0, 0);
						break;
					}

					m_iCTFEventFlagHolder[m_pClientList[i]->m_cSide - 1] = -1;
					SetIceFlag(i, DEF_OWNERTYPE_PLAYER, false);

					for (int x = 1; x < DEF_MAXCLIENTS; x++)
						if (m_pClientList[x] != 0 && m_pClientList[x]->m_cSide != 0) SendNotifyMsg(0, x, DEF_NOTIFY_EVENT, 0, 8 + m_pClientList[i]->m_cSide - 1, 0, 0);
				}

				if ((dwTime - m_pClientList[i]->m_dwExpStockTime) > (UINT32)DEF_EXPSTOCKTIME) {
					m_pClientList[i]->m_dwExpStockTime = dwTime;
					CheckCrusadeResultCalculation(i);
					CheckHeldenianResultCalculation(i); // new
				}

				// Centuu - shinning bow+15
				if (m_pClientList[i]->m_iClass == 3) 
				{
					sItemIndex = m_pClientList[i]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND];
					if (sItemIndex != -1)
					{
						if (m_pClientList[i]->m_pItemList[sItemIndex]->m_sIDnum == 140)
						{
							if (((m_pClientList[i]->m_pItemList[sItemIndex]->m_dwAttribute & 0xF0000000) >> 28) == 15)
							{
								sTemp = m_pClientList[i]->m_sAppr4;
								sTemp = sTemp | 0x0004;
							}
							else
							{
								sTemp = m_pClientList[i]->m_sAppr4;
								sTemp = sTemp & 0xFFFB;
							}
						}
						else
						{
							sTemp = m_pClientList[i]->m_sAppr4;
							sTemp = sTemp & 0xFFFB;
						}
					}
					else
					{
						sTemp = m_pClientList[i]->m_sAppr4;
						sTemp = sTemp & 0xFFFB;
					}
					if (m_pClientList[i]->m_sAppr4 != sTemp) // has changed
					{
						m_pClientList[i]->m_sAppr4 = sTemp;
						SendEventToNearClient_TypeA(i, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
					}
				}

				if (m_pClientList[i]->m_iSpecialAbilityTime == 3) {
					SendNotifyMsg(0, i, DEF_NOTIFY_SPECIALABILITYENABLED, 0, 0, 0, 0);
					sItemIndex = m_pClientList[i]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND];
					if (sItemIndex != -1) {
						if ((m_pClientList[i]->m_pItemList[sItemIndex]->m_sIDnum == 865) || (m_pClientList[i]->m_pItemList[sItemIndex]->m_sIDnum == 866)) {
							if(((m_pClientList[i]->m_iInt) > 99) && ((m_pClientList[i]->m_iMag) > 99)){
								m_pClientList[i]->m_cMagicMastery[94] = 1;
								SendNotifyMsg(0, i, DEF_NOTIFY_RESUR_ON, 0, 0, 0, 0);
							}
						}
					}
				}
				m_pClientList[i]->m_iSpecialAbilityTime--;
				if (m_pClientList[i]->m_iSpecialAbilityTime < 0) m_pClientList[i]->m_iSpecialAbilityTime = 0;
				if (m_pClientList[i]->m_bIsSpecialAbilityEnabled == true) {
					if (((dwTime - m_pClientList[i]->m_dwSpecialAbilityStartTime)/1000) > m_pClientList[i]->m_iSpecialAbilityLastSec) {
						SendNotifyMsg(0, i, DEF_NOTIFY_SPECIALABILITYSTATUS, 3, 0, 0, 0);
						m_pClientList[i]->m_bIsSpecialAbilityEnabled = false;
						m_pClientList[i]->m_iSpecialAbilityTime = DEF_SPECABLTYTIMESEC;
						sTemp = m_pClientList[i]->m_sAppr4;
						sTemp = 0xFF0F & sTemp;
						m_pClientList[i]->m_sAppr4 = sTemp;
						SendEventToNearClient_TypeA(i, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
					}
				}
				m_pClientList[i]->m_iLockedMapTime--;
				if (m_pClientList[i]->m_iLockedMapTime < 0) {
					m_pClientList[i]->m_iLockedMapTime = 0;
					ZeroMemory(m_pClientList[i]->m_cLockedMapName, sizeof(m_pClientList[i]->m_cLockedMapName));
					strcpy(m_pClientList[i]->m_cLockedMapName, "NONE");
				}
				// changed to include ImpossibleZone recall
				m_pClientList[i]->m_iDeadPenaltyTime--;
				if (m_pClientList[i]->m_iDeadPenaltyTime < 0) m_pClientList[i]->m_iDeadPenaltyTime = 0;	
				if (m_pClientList[i]->m_bIsWarLocation == true) {
					if (m_pClientList[i]->m_bIsInsideEnemyBuilding == true) {

						m_pClientList[i]->m_iTimeLeft_ForceRecall = 0;
						m_pClientList[i]->m_dwWarBeginTime = dwTime;
						m_pClientList[i]->m_bIsWarLocation = false;
						SendNotifyMsg(0, i, DEF_NOTIFY_TOBERECALLED, 0, 0, 0, 0);
						RequestTeleportHandler(i, "0   ");
						
					}
					if (m_pClientList[i]->m_bIsImpossibleZone == true) {
						m_pClientList[i]->m_iTimeLeft_ForceRecall = 0;
						m_pClientList[i]->m_bIsImpossibleZone = false;
						SendNotifyMsg(0, i, DEF_NOTIFY_TOBERECALLED, 0, 0, 0, 0);
						RequestTeleportHandler(i, "0   ");
						
					}
				}
				// Every 3 seconds, give Hero flag effect if Flag set arround...
				if (   (m_bHeldenianWarInitiated == true)
					&& (bCheckHeldenianMap(i, DEF_OWNERTYPE_PLAYER) == 1)) 
				{		SetHeroFlag(i, DEF_OWNERTYPE_PLAYER, true);
				}else 
				{		SetHeroFlag(i, DEF_OWNERTYPE_PLAYER, false);
				}
				
				if (m_pClientList[i]->m_iSkillMsgRecvCount >= 2) {
					DeleteClient(i, true, true, true, false);
				}
				else {
					m_pClientList[i]->m_iSkillMsgRecvCount = 0;
				}

				if ((m_pClientList[i]->m_iLevel < m_pMapList[m_pClientList[i]->m_cMapIndex]->m_iLevelLimit) && 
					(m_pClientList[i]->m_iAdminUserLevel < 1)) {
					SendNotifyMsg(0, i, DEF_NOTIFY_TOBERECALLED, 0, 0, 0, 0);
					RequestTeleportHandler(i, "0   ");
				}

				if ((m_pMapList[m_pClientList[i]->m_cMapIndex]->m_iUpperLevelLimit != 0) &&
					(m_pClientList[i]->m_iLevel > m_pMapList[m_pClientList[i]->m_cMapIndex]->m_iUpperLevelLimit) && 
					(m_pClientList[i]->m_iAdminUserLevel < 1)) {
					SendNotifyMsg(0, i, DEF_NOTIFY_TOBERECALLED, 0, 0, 0, 0);
					RequestTeleportHandler(i, "0   ");
				}

				// new - recall player if crusade is false and is a civilian inside a combatant only zone
				
				if ((m_bIsCrusadeMode == false) && (m_pClientList[i]->m_bIsInCombatantOnlyZone == true) && (m_pMapList[i]->m_bIsCitizenLimit == true)) {
					SendNotifyMsg(0, i, DEF_NOTIFY_TOBERECALLED, 0, 0, 0, 0);
					RequestTeleportHandler(i, "0   ");

				}

				// KaoZureS - Heldenian Maps Auto Recall
				
				if ((m_bIsHeldenianMode == false) &&
				((memcmp(m_pMapList[m_pClientList[i]->m_cMapIndex]->m_cName, "BtField", 7) == 0) ||
				(memcmp(m_pMapList[m_pClientList[i]->m_cMapIndex]->m_cName, "GodH", 4) == 0) ||
				(memcmp(m_pMapList[m_pClientList[i]->m_cMapIndex]->m_cName, "HRampart", 8) == 0)) &&
				(m_pClientList[i]->m_cSide != m_sLastHeldenianWinner) && (m_pClientList[i]->m_iAdminUserLevel == 0)) {
					if (m_pClientList[i]->m_cSide == 1) {
						RequestTeleportHandler(i, "2   ", "aresden", -1, -1); }
					else { RequestTeleportHandler(i, "2   ", "elvine", -1, -1); }
				}


				// SNOOPY:added Apoc teleport Gates here		
				Use_ApocalypseGate(i);
				
			
				m_pClientList[i]->m_iSuperAttackCount++;
				if (m_pClientList[i]->m_iSuperAttackCount > 12) 
				{
					m_pClientList[i]->m_iSuperAttackCount = 0;
					iMaxSuperAttack = ((m_pClientList[i]->m_iLevel) / 10);
					if (m_pClientList[i]->m_iSuperAttackLeft < iMaxSuperAttack) m_pClientList[i]->m_iSuperAttackLeft++;
					SendNotifyMsg(0, i, DEF_NOTIFY_SUPERATTACKLEFT, 0, 0, 0, 0);
				}
				
				
				m_pClientList[i]->m_iTimeLeft_FirmStaminar--;
				if (m_pClientList[i]->m_iTimeLeft_FirmStaminar < 0) m_pClientList[i]->m_iTimeLeft_FirmStaminar = 0;
				
				if (m_pClientList[i]->m_bIsSendingMapStatus == true) _SendMapStatus(i);
				if (m_pClientList[i]->m_iConstructionPoint > 0) {
					CheckCommanderConstructionPoint(i);
				}
			}
			SendNotifyMsg(0, i, DEF_SERVER_TIME, srvMinute, srvHour, 0, 0);
		}
	}
	
}

bool CGame::SpecialWeapon_DS(int iClientH)
{
	int ix, iy, sX, sY;
	short sOwnerH;
	char  cOwnerType;

	if (m_pClientList[iClientH] == 0) return false;

	sX = m_pClientList[iClientH]->m_sX;
	sY = m_pClientList[iClientH]->m_sY;

	for (ix = sX - 20; ix <= sX + 20; ix++) {
		for (iy = sY - 20; iy <= sY + 20; iy++) {
			m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
			if ((sOwnerH != 0)
				&& (cOwnerType == DEF_OWNERTYPE_NPC)
				&& (m_pNpcList[sOwnerH] != 0))
			{
				if (((m_pNpcList[sOwnerH]->m_sType == 31) || (m_pNpcList[sOwnerH]->m_sType == 52))
					&& (m_pNpcList[sOwnerH]->m_bIsKilled == false))
				{
					return true;
				}
			}
		}
	}
	return false;
}


void CGame::CalcExpStock(int iClientH)
{

	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
	if (m_pClientList[iClientH]->m_iExpStock <= 0) return;
	if (m_pClientList[iClientH]->m_iExp <= 0) return;
	if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cType == DEF_MAPTYPE_NOPENALTY_NOREWARD) {
		m_pClientList[iClientH]->m_iExpStock = 0;
		return;
	}

	m_pClientList[iClientH]->m_iExp += m_pClientList[iClientH]->m_iExpStock;
	m_pClientList[iClientH]->m_iAutoExpAmount += m_pClientList[iClientH]->m_iExpStock;
	m_pClientList[iClientH]->m_iExpStock = 0;

	if (bCheckLimitedUser(iClientH) == false) {
		// ÃƒÂ¼Ã‡Ã¨Ã†Ã‡ Â»Ã§Â¿Ã«Ã€Ãš ÃÂ¦Ã‡Ã‘Â¿Â¡ Ã‡Ã˜Â´Ã§ÂµÃ‡ÃÃ¶ Â¾ÃŠÃ€Â¸Â¸Ã© Â°Ã¦Ã‡Ã¨Ã„Â¡Â°Â¡ Â¿ÃƒÂ¶ÃºÂ´Ã™Â´Ã‚ Ã…Ã«ÂºÂ¸Â¸Â¦ Ã‡Ã‘Â´Ã™.
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_EXP, 0, 0, 0, 0);
	}
	bCheckLevelUp(iClientH);

}

/*********************************************************************************************************************
**  void CGame::OnMainLogSocketEvent(UINT message, WPARAM wParam, LPARAM lParam)									**
**  DESCRIPTION			:: main log socket connection manager														**
**  LAST_UPDATED		:: March 17, 2005; 4:41 PM; Hypnotoad														**
**	RETURN_VALUE		:: void																						**
**  NOTES				::	- removed DEF_XSOCKEVENT_BLOCK															**
**	MODIFICATION		::	n/a																						**
**********************************************************************************************************************/
void CGame::OnMainLogSocketEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
 int   iRet;
	
	if (m_pMainLogSock == 0) return;
	iRet = m_pMainLogSock->iOnSocketEvent(wParam, lParam);
	switch (iRet) {
	case DEF_XSOCKEVENT_CONNECTIONESTABLISH:
		PutLogList("(!!!) Main-log-socket connected!");
		break;
		
	case DEF_XSOCKEVENT_READCOMPLETE:
		OnMainLogRead();
		break;

	case DEF_XSOCKEVENT_CONFIRMCODENOTMATCH:
	case DEF_XSOCKEVENT_MSGSIZETOOLARGE:
	case DEF_XSOCKEVENT_SOCKETERROR:
	case DEF_XSOCKEVENT_SOCKETCLOSED:
		delete m_pMainLogSock;
		m_pMainLogSock = 0;
		PutLogList("(!!!) Main-log-socket connection lost!");
		m_bIsLogSockAvailable = false;
		if (m_bOnExitProcess == false) {
			m_cShutDownCode      = 3;
			m_bOnExitProcess     = true;
			m_dwExitProcessTime  = timeGetTime();
			PutLogList("(!!!) GAME SERVER SHUTDOWN PROCESS BEGIN(by main-log-socket connection Lost)!!!");
		}
		break;
	}
}

/*********************************************************************************************************************
**  void CGame::OnMainLogRead()																						**
**  DESCRIPTION			:: report critical error if mainlog isn't available											**
**  LAST_UPDATED		:: March 17, 2005; 4:42 PM; Hypnotoad														**
**	RETURN_VALUE		:: void																						**
**  NOTES				::	n/a																						**
**	MODIFICATION		::	n/a																						**
**********************************************************************************************************************/
void CGame::OnMainLogRead()
{
 UINT32 dwMsgSize;
 char * pData, cKey;

	pData = m_pMainLogSock->pGetRcvDataPointer(&dwMsgSize, &cKey);

	if (bPutMsgQuene(DEF_MSGFROM_LOGSERVER, pData, dwMsgSize, 0, cKey) == false) {
		PutLogList("@@@@@@ CRITICAL ERROR in MsgQuene!!! (OnMainLogRead) @@@@@@");
	}	
}


bool CGame::bSendMsgToLS(UINT32 dwMsg, int iClientH, bool bFlag, char* pData)
{
 UINT32 * dwp;
 UINT16  * wp;
 int     iRet, i, iSize;
 char    cCharName[12], cAccountName[11], cAccountPassword[11], cAddress[16], cGuildName[21], cTxt[120], * cp;
 char    cGuildLoc[11], cTemp[120];
 int   * ip, iSendSize;	

	// v1.41
	ZeroMemory(G_cData50000, sizeof(G_cData50000));
	ZeroMemory(cCharName, sizeof(cCharName));
	ZeroMemory(cAccountName, sizeof(cAccountName));
	ZeroMemory(cAccountPassword, sizeof(cAccountPassword));
	ZeroMemory(cAddress,   sizeof(cAddress));
	ZeroMemory(cGuildName, sizeof(cGuildName));
	ZeroMemory(cGuildLoc,  sizeof(cGuildLoc));

	switch (dwMsg) {
	case MSGID_SENDSERVERSHUTDOWNMSG:
		if (m_pMainLogSock == 0) return false;
		dwp  = (UINT32 *)(G_cData50000 + DEF_INDEX4_MSGID);
		*dwp = MSGID_SENDSERVERSHUTDOWNMSG;
		wp   = (UINT16 *)(G_cData50000 + DEF_INDEX2_MSGTYPE);
		*wp  = DEF_MSGTYPE_CONFIRM;
		iRet = m_pMainLogSock->iSendMsg(G_cData50000, 6);
		break;

	case MSGID_REQUEST_CREATENEWGUILD:
		if (_bCheckSubLogSocketIndex() == false) return false;	
		if (m_pClientList[iClientH] == 0) return false;
		dwp  = (UINT32 *)(G_cData50000 + DEF_INDEX4_MSGID);
		*dwp = MSGID_REQUEST_CREATENEWGUILD;
		wp   = (UINT16 *)(G_cData50000 + DEF_INDEX2_MSGTYPE);
		*wp  = DEF_MSGTYPE_CONFIRM;
		cp = (char *)(G_cData50000 + DEF_INDEX2_MSGTYPE + 2);
		memcpy(cCharName, m_pClientList[iClientH]->m_cCharName, 10);
		memcpy(cAccountName, m_pClientList[iClientH]->m_cAccountName, 10);
		memcpy(cAccountPassword, m_pClientList[iClientH]->m_cAccountPassword, 10);
		memcpy(cGuildName, m_pClientList[iClientH]->m_cGuildName, 20);
		memcpy(cGuildLoc, m_pClientList[iClientH]->m_cLocation, 10);
		memcpy((char *)cp, cCharName, 10);
		cp += 10;
		memcpy((char *)cp, cAccountName, 10);
		cp += 10;
		memcpy((char *)cp, cAccountPassword, 10);
		cp += 10;
		memcpy((char *)cp, cGuildName, 20);
		cp += 20;
		memcpy((char *)cp, cGuildLoc, 10);
		cp += 10;
		ip = (int *)cp;
		*ip = m_pClientList[iClientH]->m_iGuildGUID;
		cp += 4;
		iRet = m_pSubLogSock[m_iCurSubLogSockIndex]->iSendMsg(G_cData50000, 70);
		iSendSize = 70;
		break;

	case MSGID_REQUEST_SAVEPLAYERDATA_REPLY:
	case MSGID_REQUEST_SAVEPLAYERDATA:
	case MSGID_REQUEST_SAVEPLAYERDATALOGOUT:
		if (_bCheckSubLogSocketIndex() == false) return false;		
		if (m_pClientList[iClientH] == 0) return false;
		dwp  = (UINT32 *)(G_cData50000 + DEF_INDEX4_MSGID);
		*dwp = dwMsg;
		wp   = (UINT16 *)(G_cData50000 + DEF_INDEX2_MSGTYPE);
		*wp  = DEF_MSGTYPE_CONFIRM;
		cp = (char *)(G_cData50000 + DEF_INDEX2_MSGTYPE + 2);
		memcpy(cCharName, m_pClientList[iClientH]->m_cCharName, 10);
		memcpy(cAccountName, m_pClientList[iClientH]->m_cAccountName, 10);
		memcpy(cAccountPassword, m_pClientList[iClientH]->m_cAccountPassword, 10);
		memcpy((char *)cp, cCharName, 10);
		cp += 10;
		memcpy((char *)cp, cAccountName, 10);
		cp += 10;
		memcpy((char *)cp, cAccountPassword, 10);
		cp += 10;

		if (m_bSQLMode) {
			// v2.14 DB ÀúÀå ¼Óµµ Çâ»ó 
			dwp = (UINT32*)cp;
			*dwp = m_pClientList[iClientH]->m_dwCharID;
			cp += 4;
		}

		*cp = (char)bFlag;
		cp++;

		iSize = _iComposePlayerDataFileContents(iClientH, cp);

		if (m_bSQLMode) {
			iRet = m_pSubLogSock[m_iCurSubLogSockIndex]->iSendMsg(G_cData50000, 41 + iSize);
			iSendSize = 41 + iSize;
		}
		else {
			iRet = m_pSubLogSock[m_iCurSubLogSockIndex]->iSendMsg(G_cData50000, 37 + iSize);
			iSendSize = 37 + iSize;
		}
		break;
		
	case MSGID_GAMEMASTERLOG:
		if (_bCheckSubLogSocketIndex() == false) return false;
		if (m_pClientList[iClientH] == 0) return false;
		if (pData == 0) return false ;
		dwp  = (UINT32 *)(G_cData50000 + DEF_INDEX4_MSGID);
		*dwp = MSGID_GAMEMASTERLOG;
		wp   = (UINT16 *)(G_cData50000 + DEF_INDEX2_MSGTYPE);
		*wp  = DEF_MSGTYPE_CONFIRM;
		cp = (char *)(G_cData50000 + DEF_INDEX2_MSGTYPE + 2);
		iSize =  strlen(pData) ;
		memcpy((char *)cp, pData, iSize);
		iRet = m_pSubLogSock[m_iCurSubLogSockIndex]->iSendMsg(G_cData50000, 6 + iSize);
		iSendSize = 6 + iSize;
		break;

	case MSGID_GAMEITEMLOG:
		if (_bCheckSubLogSocketIndex() == false) return false;
		if (pData == 0) return false ;
		dwp  = (UINT32 *)(G_cData50000 + DEF_INDEX4_MSGID);
		*dwp = MSGID_GAMEITEMLOG;
		wp   = (UINT16 *)(G_cData50000 + DEF_INDEX2_MSGTYPE);
		*wp  = DEF_MSGTYPE_CONFIRM;
		cp = (char *)(G_cData50000 + DEF_INDEX2_MSGTYPE + 2);
		iSize =  strlen(pData) ;
		memcpy((char *)cp, pData, iSize);
		iRet = m_pSubLogSock[m_iCurSubLogSockIndex]->iSendMsg(G_cData50000, 6 + iSize);
		iSendSize = 6 + iSize;
		break;

	
	case MSGID_GAMESERVERSHUTDOWNED:
		if (m_pMainLogSock == 0) return false;
		dwp  = (UINT32 *)(G_cData50000 + DEF_INDEX4_MSGID);
		*dwp = MSGID_GAMESERVERSHUTDOWNED;
		wp   = (UINT16 *)(G_cData50000 + DEF_INDEX2_MSGTYPE);
		*wp  = DEF_MSGTYPE_CONFIRM;
		iRet = m_pMainLogSock->iSendMsg(G_cData50000, 6);
		iSendSize = 6;
		break;

	case MSGID_REQUEST_SETACCOUNTWAITSTATUS:
	case MSGID_REQUEST_SETACCOUNTINITSTATUS:
		if (_bCheckSubLogSocketIndex() == false) return false;
		if (m_pClientList[iClientH] == 0) return false;
		dwp  = (UINT32 *)(G_cData50000 + DEF_INDEX4_MSGID);
		*dwp = dwMsg;
		wp   = (UINT16 *)(G_cData50000 + DEF_INDEX2_MSGTYPE);
		*wp  = DEF_MSGTYPE_CONFIRM;
		cp = (char *)(G_cData50000 + DEF_INDEX2_MSGTYPE + 2);
		memcpy(cp, m_pClientList[iClientH]->m_cAccountName, 10);
		cp += 10;
		ip = (int *)cp;
		*ip = m_pClientList[iClientH]->m_iLevel;
		cp += 4;
		iRet = m_pSubLogSock[m_iCurSubLogSockIndex]->iSendMsg(G_cData50000, 20);
		iSendSize = 20; // 16
		break;

	case MSGID_ENTERGAMECONFIRM:
		if (_bCheckSubLogSocketIndex() == false) return false;
		if (m_pClientList[iClientH] == 0) return false;
		dwp  = (UINT32 *)(G_cData50000 + DEF_INDEX4_MSGID);
		*dwp = MSGID_ENTERGAMECONFIRM;
		wp   = (UINT16 *)(G_cData50000 + DEF_INDEX2_MSGTYPE);
		*wp  = DEF_MSGTYPE_CONFIRM;
		cp = (char *)(G_cData50000 + DEF_INDEX2_MSGTYPE + 2);
		memcpy(cp, m_pClientList[iClientH]->m_cAccountName, 10);
		cp += 10;
		memcpy(cp, m_pClientList[iClientH]->m_cAccountPassword, 10);
		cp += 10;
		memcpy(cp, m_cServerName, 10);
		cp += 10;
		ZeroMemory(cTxt, sizeof(cTxt));
		m_pClientList[iClientH]->m_pXSock->iGetPeerAddress(cTxt);
		memcpy(cp, cTxt, 16);
		cp += 16;
		ip = (int *)cp;
		*ip = m_pClientList[iClientH]->m_iLevel;
		cp += 4;
		wsprintf(G_cTxt, "Confirmed. Account: (%s) Name: (%s) Level: (%d)", m_pClientList[iClientH]->m_cAccountName, m_pClientList[iClientH]->m_cCharName, m_pClientList[iClientH]->m_iLevel);
		PutLogList(G_cTxt);

		iRet = m_pSubLogSock[m_iCurSubLogSockIndex]->iSendMsg(G_cData50000, 56);
		iSendSize = 56;
		break;
	
	case MSGID_REQUEST_REGISTERGAMESERVER:
		if (m_pMainLogSock == 0) return false;		
		wsprintf(cTxt, "(!) Try to register game server(%s)", m_cServerName);
		PutLogList(cTxt);
		dwp  = (UINT32 *)(G_cData50000 + DEF_INDEX4_MSGID);
		*dwp = MSGID_REQUEST_REGISTERGAMESERVER;
		wp   = (UINT16 *)(G_cData50000 + DEF_INDEX2_MSGTYPE);
		*wp  = DEF_MSGTYPE_CONFIRM;
		cp = (char *)(G_cData50000 + DEF_INDEX2_MSGTYPE + 2);
		memcpy(cAccountName, m_cServerName, 10);
		if (m_iGameServerMode == 1)	{
			memcpy(cAddress, m_cGameServerAddrExternal, strlen(m_cGameServerAddrExternal));
		}
		else if (m_iGameServerMode == 2)	{
			memcpy(cAddress, m_cGameServerAddr, strlen(m_cGameServerAddr));
		}
		memcpy(cp, cAccountName, 10);
		cp += 10;
		memcpy(cp, cAddress, 16);
		cp += 16;
		wp  = (UINT16 *)cp;
		*wp = m_iGameServerPort;
		cp += 2;
		*cp = m_iTotalMaps;
		cp++;
		
		for (i = 0; i < m_iTotalMaps; i++) {
			memcpy(cp, m_pMapList[i]->m_cName, 11);
			cp += 11;
		}
		iRet = m_pMainLogSock->iSendMsg(G_cData50000, 35 + m_iTotalMaps*11);
		
		iSendSize = 35 + m_iTotalMaps * 11;
		break;

	case MSGID_REQUEST_PLAYERDATA:
		if (_bCheckSubLogSocketIndex() == false) return false;
		if (m_pClientList[iClientH] == 0) return false;
		dwp  = (UINT32 *)(G_cData50000 + DEF_INDEX4_MSGID);
		*dwp = MSGID_REQUEST_PLAYERDATA;
		wp   = (UINT16 *)(G_cData50000 + DEF_INDEX2_MSGTYPE);
		*wp  = DEF_MSGTYPE_CONFIRM;
		cp = (char *)(G_cData50000 + DEF_INDEX2_MSGTYPE + 2);
		if (strlen(m_pClientList[iClientH]->m_cCharName) == 0) PutLogList("(X) CharName 0!");
		memcpy(cCharName, m_pClientList[iClientH]->m_cCharName, 10);
		memcpy(cAccountName, m_pClientList[iClientH]->m_cAccountName, 10);
		memcpy(cAccountPassword, m_pClientList[iClientH]->m_cAccountPassword, 10);
		memcpy((char *)cp, cCharName, 10);
		cp += 10;
		memcpy((char *)cp, cAccountName, 10);
		cp += 10;
		memcpy((char *)cp, cAccountPassword, 10);
		cp += 10;
		ZeroMemory(cTemp, sizeof(cTemp));
		iRet = m_pClientList[iClientH]->m_pXSock->iGetPeerAddress(cTemp);
		memcpy((char *)cp, cTemp, 16);
		cp += 16;
		iRet = m_pSubLogSock[m_iCurSubLogSockIndex]->iSendMsg(G_cData50000, 52);
		iSendSize = 52;
		break;

	case MSGID_REQUEST_NOSAVELOGOUT:
		if (_bCheckSubLogSocketIndex() == false) return false;
		if (m_pClientList[iClientH] == 0) return false;
		dwp  = (UINT32 *)(G_cData50000 + DEF_INDEX4_MSGID);
		*dwp = MSGID_REQUEST_NOSAVELOGOUT;
		wp   = (UINT16 *)(G_cData50000 + DEF_INDEX2_MSGTYPE);
		*wp  = DEF_MSGTYPE_CONFIRM;
		cp = (char *)(G_cData50000 + DEF_INDEX2_MSGTYPE + 2);
		memcpy(cCharName, m_pClientList[iClientH]->m_cCharName, 10);
		memcpy(cAccountName, m_pClientList[iClientH]->m_cAccountName, 10);
		memcpy(cAccountPassword, m_pClientList[iClientH]->m_cAccountPassword, 10);
		memcpy((char *)cp, cCharName, 10);
		cp += 10;
		memcpy((char *)cp, cAccountName, 10);
		cp += 10;
		memcpy((char *)cp, cAccountPassword, 10);
		cp += 10;
		*cp = (char)bFlag;
		cp++;
		iRet = m_pSubLogSock[m_iCurSubLogSockIndex]->iSendMsg(G_cData50000, 37);
		iSendSize = 37;
		break;

	case MSGID_REQUEST_DISBANDGUILD:
		if (_bCheckSubLogSocketIndex() == false) return false;
		if (m_pClientList[iClientH] == 0) return false;
		dwp  = (UINT32 *)(G_cData50000 + DEF_INDEX4_MSGID);
		*dwp = MSGID_REQUEST_DISBANDGUILD;
		wp   = (UINT16 *)(G_cData50000 + DEF_INDEX2_MSGTYPE);
		*wp  = DEF_MSGTYPE_CONFIRM;
		cp = (char *)(G_cData50000 + DEF_INDEX2_MSGTYPE + 2);
		memcpy(cCharName, m_pClientList[iClientH]->m_cCharName, 10);
		memcpy(cAccountName, m_pClientList[iClientH]->m_cAccountName, 10);
		memcpy(cAccountPassword, m_pClientList[iClientH]->m_cAccountPassword, 10);
		memcpy(cGuildName, m_pClientList[iClientH]->m_cGuildName, 20);
		memcpy((char *)cp, cCharName, 10);
		cp += 10;
		memcpy((char *)cp, cAccountName, 10);
		cp += 10;
		memcpy((char *)cp, cAccountPassword, 10);
		cp += 10;
		memcpy((char *)cp, cGuildName, 20);
		cp += 20;
		iRet = m_pSubLogSock[m_iCurSubLogSockIndex]->iSendMsg(G_cData50000, 56);
		iSendSize = 56;
		break;

	case MSGID_3D001242h:
	case MSGID_3D00123Eh:
		if (_bCheckSubLogSocketIndex() == false) return false;
		dwp  = (UINT32 *)(G_cData50000 + DEF_INDEX4_MSGID);
		*dwp = MSGID_3D001242h;
		wp   = (UINT16 *)(G_cData50000 + DEF_INDEX2_MSGTYPE);
		*wp  = DEF_MSGTYPE_CONFIRM;
		cp = (char *)(G_cData50000 + DEF_INDEX2_MSGTYPE + 2);
		iRet = m_pSubLogSock[m_iCurSubLogSockIndex]->iSendMsg(G_cData50000, 6);
		iSendSize = 6;
		break;
		
	case MSGID_REQUEST_HELDENIAN_WINNER: // Sub-log-socket	
		if (_bCheckSubLogSocketIndex() == false) return false;		
		dwp  = (UINT32 *)(G_cData50000 + DEF_INDEX4_MSGID);
		*dwp = MSGID_REQUEST_HELDENIAN_WINNER;
		wp   = (UINT16 *)(G_cData50000 + DEF_INDEX2_MSGTYPE);
		*wp  = DEF_MSGTYPE_CONFIRM;
		cp = (char *)(G_cData50000 + DEF_INDEX2_MSGTYPE + 2);
		if (m_cHeldenianWinner == 1)
			memcpy(cp, "aresden", 7);
		else if (m_cHeldenianWinner == 2)
			memcpy(cp, "elvine", 6);
		else 
			memcpy(cp, "draw", 4);
		cp += 7;
		ip = (int *)cp;
		ip = (int *)m_dwHeldenianWarStartTime;
		cp += 4;
		ip = (int *)cp;
		ip = (int *)m_dwHeldenianFinishTime;
		cp += 4;
		ip = (int *)cp;
		ip = (int *)m_cHeldenianType;
		cp += 4;
		iRet = m_pSubLogSock[m_iCurSubLogSockIndex]->iSendMsg(G_cData50000, 25);
		iSendSize = 25;
		break;
	
	case MSGID_REQUEST_UPDATEGUILDINFO_NEWGUILDSMAN:
		if (_bCheckSubLogSocketIndex() == false) return false;
		if (m_pClientList[iClientH] == 0) return false;
		dwp  = (UINT32 *)(G_cData50000 + DEF_INDEX4_MSGID);
		*dwp = MSGID_REQUEST_UPDATEGUILDINFO_NEWGUILDSMAN;
		wp   = (UINT16 *)(G_cData50000 + DEF_INDEX2_MSGTYPE);
		*wp  = DEF_MSGTYPE_CONFIRM;
		cp = (char *)(G_cData50000 + DEF_INDEX2_MSGTYPE + 2);
		memcpy(cCharName, m_pClientList[iClientH]->m_cCharName, 10);
		memcpy(cGuildName, m_pClientList[iClientH]->m_cGuildName, 20);
		memcpy((char *)cp, cCharName, 10);
		cp += 10;
		memcpy((char *)cp, cGuildName, 20);
		cp += 20;
		iRet = m_pSubLogSock[m_iCurSubLogSockIndex]->iSendMsg(G_cData50000, 36);
		iSendSize = 36;
		break;

	case MSGID_REQUEST_UPDATEGUILDINFO_DELGUILDSMAN:
		if (_bCheckSubLogSocketIndex() == false) return false;
		if (m_pClientList[iClientH] == 0) return false;
		dwp  = (UINT32 *)(G_cData50000 + DEF_INDEX4_MSGID);
		*dwp = MSGID_REQUEST_UPDATEGUILDINFO_DELGUILDSMAN;
		wp   = (UINT16 *)(G_cData50000 + DEF_INDEX2_MSGTYPE);
		*wp  = DEF_MSGTYPE_CONFIRM;
		cp = (char *)(G_cData50000 + DEF_INDEX2_MSGTYPE + 2);
		memcpy(cCharName, m_pClientList[iClientH]->m_cCharName, 10);
		memcpy(cGuildName, m_pClientList[iClientH]->m_cGuildName, 20);
		memcpy((char *)cp, cCharName, 10);
		cp += 10;
		memcpy((char *)cp, cGuildName, 20);
		cp += 20;
		iRet = m_pSubLogSock[m_iCurSubLogSockIndex]->iSendMsg(G_cData50000, 36);
		iSendSize = 36;
		break;
	}

	switch (iRet) {
	case DEF_XSOCKEVENT_QUENEFULL:
	case DEF_XSOCKEVENT_SOCKETERROR:
	case DEF_XSOCKEVENT_CRITICALERROR:
	case DEF_XSOCKEVENT_SOCKETCLOSED:
		wsprintf(G_cTxt, "(!!!) Sub-log-socket(%d) send error!", m_iCurSubLogSockIndex);
		PutLogList(G_cTxt);
		PutLogFileList(G_cTxt);
		delete m_pSubLogSock[m_iCurSubLogSockIndex];
		m_pSubLogSock[m_iCurSubLogSockIndex] = 0;
		m_bIsSubLogSockAvailable[m_iCurSubLogSockIndex] = false;
		m_iSubLogSockActiveCount--;
		m_pSubLogSock[m_iCurSubLogSockIndex] = new class XSocket(m_hWnd, DEF_SERVERSOCKETBLOCKLIMIT);
		m_pSubLogSock[m_iCurSubLogSockIndex]->bConnect(m_cLogServerAddr, m_iLogServerPort, (WM_ONLOGSOCKETEVENT + m_iCurSubLogSockIndex + 1));
		m_pSubLogSock[m_iCurSubLogSockIndex]->bInitBufferSize(DEF_MSGBUFFERSIZE);
		wsprintf(G_cTxt, "(!) Try to reconnect sub-log-socket(%d)... Addr:%s  Port:%d", m_iCurSubLogSockIndex, m_cLogServerAddr, m_iLogServerPort);
		PutLogList(G_cTxt);		
		m_iSubLogSockFailCount++;
		if (_bCheckSubLogSocketIndex() == false) return false;
		m_pSubLogSock[m_iCurSubLogSockIndex]->iSendMsg(G_cData50000, iSendSize);
		return false;
	}

	return true;
}

/*********************************************************************************************************************
**  void CGame::ResponsePlayerDataHandler(char * pData, UINT32 dwSize)												**
**  DESCRIPTION			:: action taken following player login														**
**  LAST_UPDATED		:: March 17, 2005; 6:24 PM; Hypnotoad														**
**	RETURN_VALUE		:: void																						**
**  NOTES				::	- new code inside LOGRESMSGTYPE_REJECT 													**
**	MODIFICATION		::	n/a																						**
**********************************************************************************************************************/
void CGame::ResponsePlayerDataHandler(char * pData, UINT32 dwSize)
{
 char * cp, cCharName[11], cTxt[120], cData[120];
 int  i; 
 UINT32 * dwp;
 UINT16  * wp;
 
	// centu - fix for wl src
	if (dwSize >= DEF_MSGBUFFERSIZE+1) return;

	ZeroMemory(cCharName, sizeof(cCharName));
	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
	memcpy(cCharName, cp, 10);
	cp += 10;
	for (i = 1; i < DEF_MAXCLIENTS; i++) 
	if (m_pClientList[i] != 0) {
		if (memcmp(m_pClientList[i]->m_cCharName, cCharName, 10) == 0) {
			wp = (UINT16 *)(pData + DEF_INDEX2_MSGTYPE);
			switch (*wp) {
			case DEF_LOGRESMSGTYPE_CONFIRM:
				InitPlayerData(i, pData, dwSize); 
				break;
	
			case DEF_LOGRESMSGTYPE_REJECT:
				// new
				
				ZeroMemory(cData, sizeof(cData));
				dwp  = (UINT32 *)(cData + DEF_INDEX4_MSGID);
				*dwp = MSGID_RESPONSE_INITPLAYER;
				wp   = (UINT16 *)(cData + DEF_INDEX2_MSGTYPE);
				*wp  = DEF_MSGTYPE_CONFIRM;
				m_pClientList[i]->m_pXSock->iSendMsg(cData, 6);
				
				// end
				wsprintf(G_cTxt, "(HACK?) Not existing character(%s) data request! Rejected!", m_pClientList[i]->m_cCharName);
				PutLogList(G_cTxt);
				DeleteClient(i, false, false, true, false);
				break;
			
			default:
				break;
			}				
			return;
		}	
	}
	wsprintf(cTxt, "(!)Non-existing player data received from Log server: CharName(%s)", cCharName);
	PutLogList(cTxt);
}

/*********************************************************************************************************************
**  bool CGame::bReadProgramConfigFile(char * cFn)																	**
**  DESCRIPTION			:: function that reads GServer.cfg															**
**  LAST_UPDATED		:: March 17, 2005; 12:09 PM; Hypnotoad														**
**	RETURN_VALUE		:: void																						**
**  NOTES				::	- with DMZ walkaround is internet and lan option necissary?								**
**							- does dns work? (unable to test)														**
**	MODIFICATION		::	- game-server-internal-address (internal lan ip address)								**
**							- game-server-external-address (DNS Server)												**
**							- game-server-mode (if inside LAN put "LAN" if not put "INTERNET" (without quotes)		**
**							- gate-server-dns (if set to "true" put in alphabetical dns address)					**
**							- log-server-dns (both dns options benefitial if ip is dynamic and server is no-ip.com	**
**********************************************************************************************************************/
bool CGame::bReadProgramConfigFile(char * cFn)
{	
	FILE * pFile;
	HANDLE hFile;
	UINT32  dwFileSize;
	char * cp, * token, cReadMode, cTxt[120], cGSMode[16] = "";
	char seps[] = "= \t\n";	
	class CStrTok * pStrTok;

	cReadMode = 0;

	hFile = CreateFile(cFn, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
	dwFileSize = GetFileSize(hFile, 0);
	if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);
	pFile = fopen(cFn, "rt");
	if (pFile != 0) {
	
		PutLogList("(!) Reading configuration file...");
		cp = new char[dwFileSize+2];
		ZeroMemory(cp, dwFileSize+2);
		fread(cp, dwFileSize, 1, pFile);

		pStrTok = new class CStrTok(cp, seps);
		token = pStrTok->pGet();
		while( token != 0 )   {

			if (cReadMode != 0) {
				switch (cReadMode) {
				case 1:
					if (strlen(token) > 10) {
						wsprintf(cTxt, "(!!!) Game server name(%s) must within 10 chars!", token);
						PutLogList(cTxt);
						return false;
					}
					strcpy(m_cServerName, token);
					wsprintf(cTxt, "(*) Game server name : %s", m_cServerName);
					PutLogList(cTxt);
					cReadMode = 0;
					break;

				case 2:
					
					m_iGameServerPort = atoi(token);
					wsprintf(cTxt, "(*) Game server port : %d", m_iGameServerPort);
					PutLogList(cTxt);
					cReadMode = 0;
					break;

				case 5:
					if (strlen(token) > 10) {
						wsprintf(cTxt, "(!!!) CRITICAL ERROR! Map name(%s) must within 10 chars!", token);
						PutLogList(cTxt);
						return false;
					}
					if (_bRegisterMap(token) == false) {
						return false;
					}
					cReadMode = 0;
					break;

				case 3:
					ZeroMemory(m_cLogServerAddr, sizeof(m_cLogServerAddr));
					if (strlen(token) > 15) {
						wsprintf(cTxt, "(!!!) Log server address(%s) must within 15 chars!", token);
						PutLogList(cTxt);
						return false;
					}
					strcpy(m_cLogServerAddr, token);
					wsprintf(cTxt, "(*) Log server address : %s", m_cLogServerAddr);
					PutLogList(cTxt);
					cReadMode = 0;
					break;

				case 4:
					m_iLogServerPort = atoi(token);
					wsprintf(cTxt, "(*) Log server port : %d", m_iLogServerPort);
					PutLogList(cTxt);
					cReadMode = 0;
					break;

				case 6:
					ZeroMemory(m_cGateServerAddr, sizeof(m_cGateServerAddr));

					if (strlen(token) > 15) {
						wsprintf(cTxt, "(!!!) Gate server address(%s) must within 15 chars!", token);
						PutLogList(cTxt);
						return false;
					}
					strcpy(m_cGateServerAddr, token);
					wsprintf(cTxt, "(*) Gate server address : %s", m_cGateServerAddr);
					PutLogList(cTxt);
					cReadMode = 0;
					break;

				case 7:
					m_iGateServerPort = atoi(token);
					wsprintf(cTxt, "(*) Gate server port : %d", m_iGateServerPort);
					PutLogList(cTxt);
					cReadMode = 0;
					break;

				case 8:
					ZeroMemory(m_cGameServerAddrInternal, sizeof(m_cGameServerAddrInternal));
					if (strlen(token) > 15) {
						wsprintf(cTxt, "(!!!) Internal (LAN) Game server address(%s) must within 15 chars!", token);
						PutLogList(cTxt);
						return false;
					}
					strcpy(m_cGameServerAddrInternal, token);
					wsprintf(cTxt, "(*) Internal (LAN) Game server address : %s", m_cGameServerAddrInternal);
					PutLogList(cTxt);
					cReadMode = 0;
					break;

				case 9:
					ZeroMemory(m_cGameServerAddrExternal, sizeof(m_cGameServerAddrExternal));
					if (strlen(token) > 15) {
						wsprintf(cTxt, "(!!!) External (Internet) Game server address(%s) must within 15 chars!", token);
						PutLogList(cTxt);
						return false;
					}
					strcpy(m_cGameServerAddrExternal, token);
					wsprintf(cTxt, "(*) External (Internet) Game server address : %s", m_cGameServerAddrExternal);
					PutLogList(cTxt);
					cReadMode = 0;
					break;

				case 10:
					ZeroMemory(m_cGameServerAddr, sizeof(m_cGameServerAddr));
					if (strlen(token) > 15) {
						wsprintf(cTxt, "(!!!) Game server address(%s) must within 15 chars!", token);
						PutLogList(cTxt);
						return false;
					}
					strcpy(m_cGameServerAddr, token);
					wsprintf(cTxt, "(*) Game server address : %s", m_cGameServerAddr);
					PutLogList(cTxt);
					cReadMode = 0;
					break;

				case 11:
					if ((memcmp(token, "lan", 3) == 0) || (memcmp(token, "LAN", 3) == 0))
					{
						m_iGameServerMode = 1;
						memcpy(cGSMode, "LAN", 3);
					}
					if ((memcmp(token, "internet", 8) == 0) || (memcmp(token, "INTERNET", 8) == 0))
					{
						m_iGameServerMode = 2;
						memcpy(cGSMode, "INTERNET", 8);
					}
					if (m_iGameServerMode == 0)
					{
						wsprintf(cTxt, "(!!!) Game server mode(%s) must be either LAN/lan/INTERNET/internet", token);
						PutLogList(cTxt);
						return false;
					}
					else if (m_iGameServerMode == 1) // Default address is always internal in ROUTER mode
					{
						ZeroMemory(m_cGameServerAddr, sizeof(m_cGameServerAddr));
						strcpy(m_cGameServerAddr, m_cGameServerAddrInternal);
					}
					else if (m_iGameServerMode == 2) // Internal are always normal in INTERNET mode
					{
						ZeroMemory(m_cGameServerAddrInternal, sizeof(m_cGameServerAddrInternal));
						strcpy(m_cGameServerAddrInternal, m_cGameServerAddr);
						ZeroMemory(m_cGameServerAddrExternal, sizeof(m_cGameServerAddrExternal));
						strcpy(m_cGameServerAddrExternal, m_cGameServerAddr);
					}
					wsprintf(cTxt, "(*) Game server mode : %s", cGSMode);
					PutLogList(cTxt);
					cReadMode = 0;
					break;

				case 12:
					m_iBuildDate = atoi(token);
					cReadMode = 0;
					break;
				}
			}
			else {
				if (memcmp(token, "game-server-name", 16) == 0)					cReadMode = 1;
				if (memcmp(token, "game-server-port", 16) == 0)					cReadMode = 2;
				
				if (memcmp(token, "game-server-map", 15) == 0)					cReadMode = 5;
				
				if (memcmp(token, "log-server-address", 18) == 0)				cReadMode = 3;
				if (memcmp(token, "internal-log-server-port", 24) == 0)			cReadMode = 4;

				if (memcmp(token, "gate-server-address", 19) == 0)				cReadMode = 6;
				if (memcmp(token, "gate-server-port", 16) == 0)					cReadMode = 7;

				if (memcmp(token, "game-server-internal-address", 28) == 0)		cReadMode = 8; // modification
				if (memcmp(token, "game-server-external-address", 28) == 0)		cReadMode = 9; // modification
				if (memcmp(token, "game-server-address", 19) == 0)				cReadMode = 10;// modification
				if (memcmp(token, "game-server-mode", 16) == 0)					cReadMode = 11;// modification

				if (memcmp(token, "build-date", 10) == 0)					cReadMode = 12;
			}
			token = pStrTok->pGet();
		}
		delete pStrTok;
		fclose(pFile);
		return true;
	}

	return false;
}


/*********************************************************************************************************************
**  bool CGame::_bDecodePlayerDatafileContents(int iClientH, char * pData, UINT32 dwSize)							**
**  DESCRIPTION			:: decodes <PLAYERNAME>.txt file in Character folder										**
**  LAST_UPDATED		:: March 17, 2005; 12:42 PM; Hypnotoad														**
**	RETURN_VALUE		:: bool																						**
**  NOTES				::	- MOST OF THIS FUNCTION UPGRADE IS IGNORED DUE TO MYSQL<->ASCII ISSUES					**
**							- changed stat point checks code														**
**	MODIFICATION		::	n/a																						**
**********************************************************************************************************************/

bool CGame::_bDecodePlayerDatafileContents(int iClientH, char * pData, UINT32 dwSize)
{
	char * pContents, * token, * pOriginContents, cTmpName[11], cTxt[120];
	char   seps[] = "= \t\n";
	char   cReadModeA, cReadModeB;
	int    i, iItemIndex, iItemInBankIndex, iTotalGold, iNotUsedItemPrice;
	class  CStrTok * pStrTok;
	short  sTmpType, sTmpAppr1;
	bool   bRet;
	int    iTemp, iValue;
	SYSTEMTIME SysTime;
	UINT32 iDateSum1, iDateSum2;
	bool   bIsNotUsedItemFound = false;

	if (m_pClientList[iClientH] == 0) return false;

	iTotalGold = 0;
	iItemIndex = 0;
	iItemInBankIndex  = 0;
	iNotUsedItemPrice = 0;
	cReadModeA = 0;
	cReadModeB = 0;

	pContents = new char[dwSize+2];
	ZeroMemory(pContents, dwSize+2);
	memcpy(pContents, pData, dwSize);
	pOriginContents = pContents;
	pStrTok = new class CStrTok(pContents, seps);
	token = pStrTok->pGet();

	while( token != 0 ) {
		if (cReadModeA != 0) {
			switch (cReadModeA) {
			case 1:
				// Ã‡ÃƒÂ·Â¹Ã€ÃŒÂ¾Ã®Â°Â¡ Â¼Ã“Ã‡Ã˜Ã€Ã–Â´Ã‚ Â¸ÃŠ Ã€ÃŒÂ¸Â§Ã€Â» Â¾Ã²Â´Ã‚Â´Ã™. 
				ZeroMemory(m_pClientList[iClientH]->m_cMapName, sizeof(m_pClientList[iClientH]->m_cMapName));
				strcpy(m_pClientList[iClientH]->m_cMapName, token);
				// Ã€ÃŒÃÂ¦ Ã€ÃŒ Â¸ÃŠÃ€Ã‡ Ã€ÃŽÂµÂ¦Â½ÂºÂ°ÂªÃ€Â» ÃƒÂ£Â¾Ã† Ã‡Ã’Â´Ã§Ã‡Ã‘Â´Ã™. 
				ZeroMemory(cTmpName, sizeof(cTmpName));
				strcpy(cTmpName, token);
				for (i = 0; i < DEF_MAXMAPS; i++)
					if ((m_pMapList[i] != 0)	&& (memcmp(m_pMapList[i]->m_cName, cTmpName, 10) == 0)) {
						// Â¸ÃŠÃ€Ã‡ Ã€ÃŽÂµÂ¦Â½ÂºÂ´Ã‚ iÃ€ÃŒÂ´Ã™.
						m_pClientList[iClientH]->m_cMapIndex = (char)i;
					}

					if (m_pClientList[iClientH]->m_cMapIndex == -1) {
						wsprintf(cTxt, "(!) Player(%s) tries to enter unknown map : %s", m_pClientList[iClientH]->m_cCharName, cTmpName); 
						PutLogList(cTxt);
						delete[] pContents;
						delete pStrTok;
						return false;
					}

					cReadModeA = 0;
					break;

			case 2:
				// Ã‡ÃƒÂ·Â¹Ã€ÃŒÂ¾Ã®Ã€Ã‡ Â¸ÃŠÂ»Ã³Ã€Ã‡ Ã€Â§Ã„Â¡ X
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_sX ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_sX = atoi(token);
				cReadModeA = 0;
				break;

			case 3:
				// Ã‡ÃƒÂ·Â¹Ã€ÃŒÂ¾Ã®Ã€Ã‡ Â¸ÃŠÂ»Ã³Ã€Ã‡ Ã€Â§Ã„Â¡ Y
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_sY ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_sY = atoi(token);
				cReadModeA = 0;
				break;

			case 4:
				// Ã‡ÃƒÂ·Â¹Ã€ÃŒÂ¾Ã®Ã€Ã‡ Â°Ã¨ÃÂ¤ Â»Ã³Ã…Ã‚Â¸Â¦ Â¾Ã²Â´Ã‚Â´Ã™. <- Ã€ÃŒ Â±Ã¢Â´Ã‰Ã€Âº Ã‡Ã¶Ã€Ã§ Â¾Â²ÃÃ¶ Â¾ÃŠÂ´Ã‚Â´Ã™. Â°Ã¨ÃÂ¤ÃÂ¤ÂºÂ¸Â´Ã‚ ÂºÂ°ÂµÂµÂ·ÃŽ Ã€Ã¼Â´ÃžÂµÃ‡Â¹Ã‡Â·ÃŽ 
				
				cReadModeA = 0;
				break;

			case 5:
				// Â¼Ã’ÃÃ¶Ã‡ÃÂ°Ã­ Ã€Ã–Â´Ã‚ Â¾Ã†Ã€ÃŒÃ…Ã› ÃÂ¤ÂºÂ¸Â¸Â¦ Ã€ÃÂ¾Ã® ÃƒÃŠÂ±Ã¢ÃˆÂ­ Ã‡Ã‘Â´Ã™.
				switch(cReadModeB) {
			case 1:
				// New 07/05/2004
				// v2.12
				if (iItemIndex >= DEF_MAXITEMS) {
					delete[] pContents;
					delete pStrTok;
					return false;
				}

				// tokenÂ°ÂªÃ€Âº Â¾Ã†Ã€ÃŒÃ…Ã›Ã€Ã‡ Ã€ÃŒÂ¸Â§. Ã€ÃŒ Ã€ÃŒÂ¸Â§Ã€Â» Â°Â¡ÃÃ¸ Â¾Ã†Ã€ÃŒÃ…Ã›Ã€Ã‡ Ã†Â¯Â¼ÂºÃ€Â» m_pItemConfigListÂ¿Â¡Â¼Â­ ÃƒÂ£Â¾Ã† Ã‡Ã’Â´Ã§Ã‡Ã‘Â´Ã™.
				if (_bInitItemAttr(m_pClientList[iClientH]->m_pItemList[iItemIndex], token) == false) {
					// Ã‡ÃƒÂ·Â¹Ã€ÃŒÂ¾Ã®Â°Â¡ Â¼Ã’ÃÃ¶Ã‡Ã‘ Â¾Ã†Ã€ÃŒÃ…Ã›Ã€ÃŒ Â¸Â®Â½ÂºÃ†Â®Â¿Â¡ Â¾Ã¸Â´Ã™. Â¿Â¡Â·Â¯
					wsprintf(cTxt, "(!!!) Client(%s)-Item(%s) is not existing Item!", m_pClientList[iClientH]->m_cCharName, token); 
					PutLogList(cTxt);

					// Debug code @@@@@@@@@@@@@@@
					HANDLE hFile;
					DWORD  nWrite;
					hFile = CreateFile("Error.Log", GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
					WriteFile(hFile, (char *)pContents, dwSize+2, &nWrite, 0);
					CloseHandle(hFile);
					///////////// @@@@@@@@@@@@@@@

					delete[] pContents;
					delete pStrTok;
					return false;
				}
				cReadModeB = 2;
				break;

			case 2:
				// m_dwCount
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Item Data format - m_dwCount ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}

				iTemp = atoi(token);
				if (iTemp < 0) iTemp = 1;
				if ( iGetItemWeight(m_pClientList[iClientH]->m_pItemList[iItemIndex], iTemp) > _iCalcMaxLoad(iClientH) ) {
					iTemp = 1;
					wsprintf(G_cTxt, "(!) Player(%s) Item (%s) too heavy for player to carry", m_pClientList[iClientH]->m_cCharName, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName);
					PutLogFileList(G_cTxt);
					PutLogList(G_cTxt);
				}

				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwCount = (UINT32)iTemp;
				cReadModeB = 3;

				if (strcmp(m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName, "Gold") == 0)
					iTotalGold += iTemp;
				break;

			case 3:
				// m_sTouchEffectType
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Item Data format - m_sTouchEffectType ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectType = atoi(token);
				cReadModeB = 4;
				break;

			case 4:
				// m_sTouchEffectValue1
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Item Data format - m_sTouchEffectValue1 ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 = atoi(token);
				cReadModeB = 5;
				break;

			case 5:
				// m_sTouchEffectValue2
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Item Data format - m_sTouchEffectValue2 ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 = atoi(token);
				cReadModeB = 6;
				break;

			case 6:
				// m_sTouchEffectValue3
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Item Data format - m_sTouchEffectValue3 ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 = atoi(token);
				cReadModeB = 7;
				break;

			case 7:
				// m_cItemColor
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Item Data format - m_cItemColor ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor = atoi(token);
				cReadModeB = 8;
				break;

			case 8:
				// m_sItemSpecEffectValue1
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Item Data format - m_sItemSpecEffectValue1 ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue1 = atoi(token);
				cReadModeB = 9;
				break;

			case 9:
				// m_sItemSpecEffectValue2
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Item Data format - m_sItemSpecEffectValue2 ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2 = atoi(token);
				cReadModeB = 10;
				break;

			case 10:
				// m_sItemSpecEffectValue3
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Item Data format - m_sItemSpecEffectValue3 ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue3 = atoi(token);
				cReadModeB = 11;
				break;

			case 11:
				// m_wCurLifeSpan
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Item Data format - m_wCurLifeSpan ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan = atoi(token);
				cReadModeB = 12;
				break;

			case 12:
				// m_dwAttribute
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Item Data format - m_dwAttribute ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = atoi(token);

				// v1.4 Â°Â³Ã€ÃŽ Â°Ã­Ã€Â¯ Â¾Ã†Ã€ÃŒÃ…Ã›Ã€Â» ÃˆÂ®Ã€ÃŽÃ‡Ã‘Â´Ã™. 
				if (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectType == DEF_ITET_UNIQUE_OWNER) {
					if ( (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 != m_pClientList[iClientH]->m_sCharIDnum1) ||
						(m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 != m_pClientList[iClientH]->m_sCharIDnum2) ||
						(m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 != m_pClientList[iClientH]->m_sCharIDnum3) ) {
							// Ã€ÃšÂ½Ã…Ã€Ã‡ Â°ÃÃ€ÃŒ Â¾Ã†Â´Ã‘ Â¾Ã†Ã€ÃŒÃ…Ã›Ã€Â» Â°Â®Â°Ã­ Ã€Ã–Â´Ã™. 
							wsprintf(cTxt, "(!) Player(%s) Item(%s) %d %d %d - %d %d %d", m_pClientList[iClientH]->m_cCharName, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName,
								m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1,
								m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2,
								m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3,
								m_pClientList[iClientH]->m_sCharIDnum1,
								m_pClientList[iClientH]->m_sCharIDnum2,
								m_pClientList[iClientH]->m_sCharIDnum3); 
							PutLogList(cTxt);
							
						}
				}

				cReadModeB = 13;

				//Custom-Made
				if ((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0x00000001 ) != 0) {
					//ItemSpecialEffectValue1
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wMaxLifeSpan = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue1;
					//ItemSpecialEffectValue2
				}

				_AdjustRareItemValue(m_pClientList[iClientH]->m_pItemList[iItemIndex]);

				if ((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan <= 0) && 
					(m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ALTERITEMDROP)) {
						wsprintf(G_cTxt, "(!) Player(%s) DEF_ITEMEFFECTTYPE_ALTERITEMDROP !", m_pClientList[iClientH]->m_cCharName);
						PutLogFileList(G_cTxt);
						m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan = 1;
					}
					bCheckAndConvertPlusWeaponItem(iClientH, iItemIndex);
					if (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemType == DEF_ITEMTYPE_NOTUSED) {
						iNotUsedItemPrice += m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wPrice;
						delete m_pClientList[iClientH]->m_pItemList[iItemIndex];
						m_pClientList[iClientH]->m_pItemList[iItemIndex] = 0;
						bIsNotUsedItemFound = true;
					}
					else
						if (_bCheckDupItemID(m_pClientList[iClientH]->m_pItemList[iItemIndex]) == true) {
							_bItemLog(DEF_ITEMLOG_DUPITEMID, iClientH, 0, m_pClientList[iClientH]->m_pItemList[iItemIndex]);

							iNotUsedItemPrice += m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wPrice;
							delete m_pClientList[iClientH]->m_pItemList[iItemIndex];
							m_pClientList[iClientH]->m_pItemList[iItemIndex] = 0;
						}
						
				break;

			//Magn0S:: Add new variables for itens attributes
			case 13: 
				if (_bGetIsStringIsNumber(token) == false) {
					ZeroMemory(G_cTxt, sizeof(G_cTxt));
					wsprintf(G_cTxt, "(!)13 CharacterContent(%s) - Item(%s) Invalid item attribute2!", m_pClientList[iClientH]->m_cCharName, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName);
					PutLogFileList(G_cTxt);
				}

				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sNewEffect1 = atoi(token);

				cReadModeB = 14;
				break;

			case 14: //
				if (_bGetIsStringIsNumber(token) == false) {
					ZeroMemory(G_cTxt, sizeof(G_cTxt));
					wsprintf(G_cTxt, "(!) 14 CharacterContent(%s) - Item(%s) Invalid item attribute2!", m_pClientList[iClientH]->m_cCharName, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName);
					PutLogFileList(G_cTxt);
				}

				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sNewEffect2 = atoi(token);

				cReadModeB = 15;
				break;

			case 15: //
				if (_bGetIsStringIsNumber(token) == false) {
					ZeroMemory(G_cTxt, sizeof(G_cTxt));
					wsprintf(G_cTxt, "(!)15 CharacterContent(%s) - Item(%s) Invalid item attribute2!", m_pClientList[iClientH]->m_cCharName, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName);
					PutLogFileList(G_cTxt);
				}

				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sNewEffect3 = atoi(token);

				cReadModeB = 16;
				break;

			case 16: //
				if (_bGetIsStringIsNumber(token) == false) {
					ZeroMemory(G_cTxt, sizeof(G_cTxt));
					wsprintf(G_cTxt, "(!)16 CharacterContent(%s) - Item(%s) Invalid item attribute2!", m_pClientList[iClientH]->m_cCharName, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName);
					PutLogFileList(G_cTxt);
				}

				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sNewEffect4 = atoi(token);

				cReadModeA = 0;
				cReadModeB = 0;
				iItemIndex++;
				break;
				}
				break;

			case 6:
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_cSex ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_cSex = atoi(token);
				cReadModeA = 0;
				break;

			case 7:
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_cSkin ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_cSkin = atoi(token);
				cReadModeA = 0;
				break;

			case 8:
				// Ã„Â³Â¸Â¯Ã…ÃÃ€Ã‡ Ã‡Ã¬Â¾Ã® Â½ÂºÃ…Â¸Ã€Ã 
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_cHairStyle ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_cHairStyle = atoi(token);
				cReadModeA = 0;
				break;

			case 9:
				// Ã„Â³Â¸Â¯Ã…ÃÃ€Ã‡ Â¸Ã“Â¸Â®Â»Ã¶ 
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_cHairColor ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_cHairColor = atoi(token);
				cReadModeA = 0;
				break;

			case 10:
				// Ã„Â³Â¸Â¯Ã…ÃÃ€Ã‡ Â¼Ã“Â¿ÃŠÂ»Ã¶
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_cUnderwear ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_cUnderwear = atoi(token);
				cReadModeA = 0;
				break;

			case 11:
				// Â¾Ã†Ã€ÃŒÃ…Ã› Ã€Ã¥Ã‚Ã¸ Â»Ã³Ã…Ã‚Â°Â¡ Â¹ÃÂµÃ¥Â½Ãƒ Â¸Â¶ÃÃ¶Â¸Â·Ã€ÃŒÂ¾Ã®Â¾ÃŸ Ã‡Ã‘Â´Ã™!!!!!

				// Â¾Ã†Ã€ÃŒÃ…Ã› Ã€Ã¥Ã‚Ã¸ Â»Ã³Ã…Ã‚ ÃƒÃŠÂ±Ã¢ÃˆÂ­Ã‡Ã‘ ÃˆÃ„ Â¼Â³ÃÂ¤Ã‡Ã‘Â´Ã™.
				for (i = 0; i < DEF_MAXITEMEQUIPPOS; i++) 
					m_pClientList[iClientH]->m_sItemEquipmentStatus[i] = -1;

				// Â¾Ã†Ã€ÃŒÃ…Ã› Ã€Ã¥Ã‚Ã¸Â»Ã³Ã…Ã‚Â¸Â¦ ÃƒÃŠÂ±Ã¢ÃˆÂ­Ã‡Ã‘Â´Ã™.
				for (i = 0; i < DEF_MAXITEMS; i++) 
					m_pClientList[iClientH]->m_bIsItemEquipped[i] = false;

				// v1.41 Â»Ã§Â¿Ã«Ã‡ÃÃÃ¶ Â¾ÃŠÂ´Ã‚ Â¾Ã†Ã€ÃŒÃ…Ã›Ã€Â» Â¼Ã’ÃÃ¶Ã‡ÃÂ°Ã­ Ã€Ã–Â¾ÃºÂ´Ã™Â¸Ã© Ã€Ã¥Ã‚Ã¸Â»Ã³Ã…Ã‚Â¸Â¦ Â¸Ã°ÂµÃŽ Â¹Â«Â½Ãƒ. 
				if (bIsNotUsedItemFound == false) {
					for (i = 0; i < DEF_MAXITEMS; i++) {
						
						if ((token[i] == '1') && (m_pClientList[iClientH]->m_pItemList[i] != 0)) {
							// Ã€Ã¥Ã‚Ã¸ÂµÃ‰ Â¼Ã¶ Ã€Ã–Â´Ã‚ Â¼ÂºÃÃºÃ€Ã‡ Â¾Ã†Ã€ÃŒÃ…Ã›Ã€ÃŒÂ¶Ã³Â¸Ã© Ã€Ã¥Ã‚Ã¸ 
							if (m_pClientList[iClientH]->m_pItemList[i]->m_cItemType == DEF_ITEMTYPE_EQUIP) 
								m_pClientList[iClientH]->m_bIsItemEquipped[i] = true;
							else m_pClientList[iClientH]->m_bIsItemEquipped[i] = false;
						}
						else m_pClientList[iClientH]->m_bIsItemEquipped[i] = false;

						if ((m_pClientList[iClientH] != 0) && (m_pClientList[iClientH]->m_bIsItemEquipped[i] == true)) {
							// Â¾Ã†Ã€ÃŒÃ…Ã›Ã€ÃŒ Ã€Ã¥Ã‚Ã¸ÂµÃ‡Â¾Ã® Ã€Ã–Â´Ã™Â¸Ã© Ã€Ã¥Ã‚Ã¸ ÃˆÂ¿Â°ÃºÂ¸Â¦ Â°Ã¨Â»ÃªÃ‡Ã‘Â´Ã™.
							if (bEquipItemHandler(iClientH, i) == false) // false
								m_pClientList[iClientH]->m_bIsItemEquipped[i] = false; // Â¸Â¸Â¾Ã  Ã†Â¯Â¼ÂºÃ„Â¡ ÂºÂ¯ÂµÂ¿Ã€Â¸Â·ÃŽ Ã€Ã¥Ã‚Ã¸ÂµÃˆ Â¾Ã†Ã€ÃŒÃ…Ã›Ã€ÃŒ Ã€Ã¥Ã‚Ã¸ÂµÃ‡ÃÃ¶ Â¾ÃŠÂ¾Ã†Â¾ÃŸ Ã‡Ã‘Â´Ã™Â¸Ã© Ã€Ã¥Ã‚Ã¸ Â¾ÃˆÃ‡Ã‘Â°ÃÃ€Â» Ã‡Â¥Â½Ãƒ 
						}
					}
				}

				cReadModeA = 0;
				break;

			case 12:
				// Â¼Ã’Â¼Ã“ Â±Ã¦ÂµÃ¥Ã€ÃŒÂ¸Â§Ã€Â» ÃƒÃŠÂ±Ã¢ÃˆÂ­Ã‡Ã‘Â´Ã™.
				// Â±Ã¦ÂµÃ¥ Ã€ÃŒÂ¸Â§Ã€Âº ÃƒÃ–Â´Ã« 20Ã€Ãš 
				ZeroMemory(m_pClientList[iClientH]->m_cGuildName, sizeof(m_pClientList[iClientH]->m_cGuildName));
				strcpy(m_pClientList[iClientH]->m_cGuildName, token);
				cReadModeA = 0;
				break;

			case 13:
				// Â±Ã¦ÂµÃ¥ Â·Â©Ã…Â© 
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_iGuildRank ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_iGuildRank = atoi(token);
				cReadModeA = 0;
				break;

			case 14:
				// HP 
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_iHP ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_iHP = atoi(token);
				cReadModeA = 0;
				break;

			case 15:
				// DefenseRatio Â»Ã§Â¿Ã«Ã‡ÃÃÃ¶ Â¾ÃŠÂ´Ã‚Â´Ã™. Â¹Â«Ã€Ã‡Â¹ÃŒÃ‡Ã˜ ÃÃ¼ 
				cReadModeA = 0;
				break;

			case 16:
				// HitRatio  Â»Ã§Â¿Ã«Ã‡ÃÃÃ¶ Â¾ÃŠÂ´Ã‚Â´Ã™. Â¹Â«Ã€Ã‡Â¹ÃŒÃ‡Ã˜ ÃÃ¼ 
				cReadModeA = 0;
				break;

			case 17:
				// Level  
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_iLevel ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_iLevel = atoi(token);
				cReadModeA = 0;
				break;

			case 18:
				// Str 
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_iStr ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_iStr = atoi(token);
				cReadModeA = 0;
				break;

			case 19:
				// Int 
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_iInt ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_iInt = atoi(token);
				cReadModeA = 0;
				break;

			case 20:
				// Vit 
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_iVit ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_iVit = atoi(token);
				cReadModeA = 0;
				break;

			case 21:
				// Dex 
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_iDex ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_iDex = atoi(token);
				cReadModeA = 0;
				break;

			case 22:
				// Mag 
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_iMag ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_iMag = atoi(token);
				cReadModeA = 0;
				break;

			case 23:
				// Charisma 
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_iCharisma ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_iCharisma = atoi(token);
				cReadModeA = 0;
				break;

			case 24:
				// Luck 
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_iLuck ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_iLuck = atoi(token);
				cReadModeA = 0;
				break;

			case 25:
				// Exp 
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_iExp ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_iExp = atoi(token);
				cReadModeA = 0;
				break;

			case 26:
				// Magic-Mastery Â¼Â³ÃÂ¤Ã‡Ã‘Â´Ã™. // 0Ã€ÃŒÂ¸Ã© Â»Ã§Â¿Ã« ÂºÃ’Â°Â¡. 1Ã€ÃŒÂ¸Ã© Â¹Ã¨Â¿Ã® Â»Ã³Ã…Ã‚.
				for (i = 0; i < DEF_MAXMAGICTYPE; i++) {
					m_pClientList[iClientH]->m_cMagicMastery[i] = token[i] - 48;
				}
				cReadModeA = 0;
				break;

			case 27:
				// Skill-Mastery Â¼Â³ÃÂ¤Ã‡Ã‘Â´Ã™.
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_cSkillMastery ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_cSkillMastery[cReadModeB] = atoi(token);
				cReadModeB++;

				if (cReadModeB >= DEF_MAXSKILLTYPE) {
					cReadModeA = 0;
					cReadModeB = 0;
				}
				break;
				//
			case 28:
				// Warehouse
			switch(cReadModeB) {
			case 1:
				if (iItemInBankIndex >= DEF_MAXBANKITEMS) {
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				if (_bInitItemAttr(m_pClientList[iClientH]->m_pItemInBankList[iItemInBankIndex], token) == false) {
					// Ã‡ÃƒÂ·Â¹Ã€ÃŒÂ¾Ã®Â°Â¡ Ã€ÃºÃ€Ã¥Ã‡ÃÂ°Ã­ Ã€Ã–Â´Ã‚ Â¾Ã†Ã€ÃŒÃ…Ã›Ã€ÃŒ Â¸Â®Â½ÂºÃ†Â®Â¿Â¡ Â¾Ã¸Â´Ã™. Â¿Â¡Â·Â¯
					wsprintf(cTxt, "(!!!) Client(%s)-Bank Item(%s) is not existing Item!", m_pClientList[iClientH]->m_cCharName, token); 
					PutLogList(cTxt);

					// Debug code @@@@@@@@@@@@@@@
					HANDLE hFile;
					DWORD  nWrite;
					hFile = CreateFile("Error.Log", GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
					WriteFile(hFile, (char *)pOriginContents, dwSize+2, &nWrite, 0);
					CloseHandle(hFile);
					///////////// @@@@@@@@@@@@@@@

					delete[] pContents;
					delete pStrTok;
					return false;
				}
				cReadModeB = 2;
				break;

			case 2:
				// m_dwCount
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Bank Data format - m_dwCount ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}

				iTemp = atoi(token);
				if (iTemp < 0) iTemp = 1;

				if ( iGetItemWeight(m_pClientList[iClientH]->m_pItemInBankList[iItemInBankIndex], iTemp) > _iCalcMaxLoad(iClientH) ) {
					iTemp = 1;
					wsprintf(G_cTxt, "(!) Player(%s) Item(%s) _iCalcMaxLoad !", m_pClientList[iClientH]->m_cCharName, m_pClientList[iClientH]->m_pItemInBankList[iItemInBankIndex]->m_cName);
					PutLogFileList(G_cTxt);
					PutLogList(G_cTxt);
				}

				m_pClientList[iClientH]->m_pItemInBankList[iItemInBankIndex]->m_dwCount = (UINT32)iTemp;
				cReadModeB = 3;

				if (strcmp(m_pClientList[iClientH]->m_pItemInBankList[iItemInBankIndex]->m_cName, "Gold") == 0)
					iTotalGold += iTemp;
				break;

			case 3:
				// m_sTouchEffectType
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Bank Data format - m_sTouchEffectType ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_pItemInBankList[iItemInBankIndex]->m_sTouchEffectType = atoi(token);
				cReadModeB = 4;
				break;

			case 4:
				// m_sTouchEffectValue1
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Bank Data format - m_sTouchEffectValue1 ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_pItemInBankList[iItemInBankIndex]->m_sTouchEffectValue1 = atoi(token);
				cReadModeB = 5;
				break;

			case 5:
				// m_sTouchEffectValue2
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Bank Data format - m_sTouchEffectValue2 ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_pItemInBankList[iItemInBankIndex]->m_sTouchEffectValue2 = atoi(token);
				cReadModeB = 6;
				break;

			case 6:
				// m_sTouchEffectValue3
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Bank Data format - m_sTouchEffectValue3 ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_pItemInBankList[iItemInBankIndex]->m_sTouchEffectValue3 = atoi(token);
				cReadModeB = 7;
				break;

			case 7:
				// m_cItemColor
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Bank Data format - m_cItemColor ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_pItemInBankList[iItemInBankIndex]->m_cItemColor = atoi(token);
				cReadModeB = 8;
				break;

			case 8:
				// m_sItemSpecEffectValue1
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Bank Data format - m_sItemSpecEffectValue1 ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_pItemInBankList[iItemInBankIndex]->m_sItemSpecEffectValue1 = atoi(token);
				cReadModeB = 9;
				break;

			case 9:
				// m_sItemSpecEffectValue2
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Bank Data format - m_sItemSpecEffectValue2 ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_pItemInBankList[iItemInBankIndex]->m_sItemSpecEffectValue2 = atoi(token);
				cReadModeB = 10;
				break;

			case 10:
				// m_sItemSpecEffectValue3
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Bank Data format - m_sItemSpecEffectValue3 ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_pItemInBankList[iItemInBankIndex]->m_sItemSpecEffectValue3 = atoi(token);
				cReadModeB = 11;
				break;

			case 11:
				// m_wCurLifeSpan
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Bank Data format - m_wCurLifeSpan ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_pItemInBankList[iItemInBankIndex]->m_wCurLifeSpan = atoi(token);

				cReadModeB = 12;
				break;

			case 12:
				// m_dwAttribute
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Bank Data format - m_dwAttribute ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_pItemInBankList[iItemInBankIndex]->m_dwAttribute = atoi(token);

				cReadModeB = 13;

				if ((m_pClientList[iClientH]->m_pItemInBankList[iItemInBankIndex]->m_dwAttribute & 0x00000001) != 0) {
					// ItemSpecialEffectValue1
					m_pClientList[iClientH]->m_pItemInBankList[iItemInBankIndex]->m_wMaxLifeSpan = m_pClientList[iClientH]->m_pItemInBankList[iItemInBankIndex]->m_sItemSpecEffectValue1;
				}

				iValue = (m_pClientList[iClientH]->m_pItemInBankList[iItemInBankIndex]->m_dwAttribute & 0xF0000000) >> 28;
				if (iValue > 0) {
					switch (m_pClientList[iClientH]->m_pItemInBankList[iItemInBankIndex]->m_cCategory) {
						case 5:
						case 6:
							m_pClientList[iClientH]->m_pItemInBankList[iItemInBankIndex]->m_wMaxLifeSpan = m_pClientList[iClientH]->m_pItemInBankList[iItemInBankIndex]->m_sItemSpecEffectValue1;
						break;
						}
					}
				_AdjustRareItemValue(m_pClientList[iClientH]->m_pItemInBankList[iItemInBankIndex]);
					if ((m_pClientList[iClientH]->m_pItemInBankList[iItemInBankIndex]->m_wCurLifeSpan <= 0) && 
						(m_pClientList[iClientH]->m_pItemInBankList[iItemInBankIndex]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ALTERITEMDROP)) {
						wsprintf(G_cTxt, "(!) Player(%s) DEF_ITEMEFFECTTYPE_ALTERITEMDROP !", m_pClientList[iClientH]->m_cCharName);
						PutLogFileList(G_cTxt);
						m_pClientList[iClientH]->m_pItemInBankList[iItemInBankIndex]->m_wCurLifeSpan = 1;
					}

					if (m_pClientList[iClientH]->m_pItemInBankList[iItemInBankIndex]->m_cItemType == DEF_ITEMTYPE_NOTUSED) {
						iNotUsedItemPrice += m_pClientList[iClientH]->m_pItemInBankList[iItemInBankIndex]->m_wPrice;
						delete m_pClientList[iClientH]->m_pItemInBankList[iItemInBankIndex];
						m_pClientList[iClientH]->m_pItemInBankList[iItemInBankIndex] = 0;
					}
					else 
						if (_bCheckDupItemID(m_pClientList[iClientH]->m_pItemInBankList[iItemInBankIndex]) == true) {
							_bItemLog(DEF_ITEMLOG_DUPITEMID, iClientH, 0, m_pClientList[iClientH]->m_pItemInBankList[iItemInBankIndex]);

							iNotUsedItemPrice += m_pClientList[iClientH]->m_pItemInBankList[iItemInBankIndex]->m_wPrice;
							delete m_pClientList[iClientH]->m_pItemInBankList[iItemInBankIndex];
							m_pClientList[iClientH]->m_pItemInBankList[iItemInBankIndex] = 0;		
						}
				break;

			//Magn0S:: New Variable for itens attributes
			case 13: //
				if (_bGetIsStringIsNumber(token) == false) {
					ZeroMemory(G_cTxt, sizeof(G_cTxt));
					wsprintf(G_cTxt, "(!)13 CharacterContent(%s) - Bank Invalid item m_sNewEffect1", m_pClientList[iClientH]->m_cCharName);
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}

				m_pClientList[iClientH]->m_pItemInBankList[iItemInBankIndex]->m_sNewEffect1 = atoi(token);

				cReadModeB = 14;
				break;

			case 14: //
				if (_bGetIsStringIsNumber(token) == false) {
					ZeroMemory(G_cTxt, sizeof(G_cTxt));
					wsprintf(G_cTxt, "(!) 14 CharacterContent(%s) - Bank Invalid item m_sNewEffect2", m_pClientList[iClientH]->m_cCharName);
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}

				m_pClientList[iClientH]->m_pItemInBankList[iItemInBankIndex]->m_sNewEffect2 = atoi(token);

				cReadModeB = 15;
				break;

			case 15: //
				if (_bGetIsStringIsNumber(token) == false) {
					ZeroMemory(G_cTxt, sizeof(G_cTxt));
					wsprintf(G_cTxt, "(!)15 CharacterContent(%s) - Bank Invalid item m_sNewEffect3", m_pClientList[iClientH]->m_cCharName);
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}

				m_pClientList[iClientH]->m_pItemInBankList[iItemInBankIndex]->m_sNewEffect3 = atoi(token);

				cReadModeB = 16;
				break;

			case 16: //
				if (_bGetIsStringIsNumber(token) == false) {
					ZeroMemory(G_cTxt, sizeof(G_cTxt));
					wsprintf(G_cTxt, "(!)16 CharacterContent(%s) - Bank Invalid item m_sNewEffect4", m_pClientList[iClientH]->m_cCharName);
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}

				m_pClientList[iClientH]->m_pItemInBankList[iItemInBankIndex]->m_sNewEffect4 = atoi(token);

				cReadModeA = 0;
				cReadModeB = 0;
				iItemInBankIndex++;
				break;
			}
			break;

			case 29:
				ZeroMemory(m_pClientList[iClientH]->m_cLocation, sizeof(m_pClientList[iClientH]->m_cLocation));
				strcpy(m_pClientList[iClientH]->m_cLocation, token);
				if (memcmp(m_pClientList[iClientH]->m_cLocation+3,"hunter",6) == 0)
					m_pClientList[iClientH]->m_bIsPlayerCivil = true;
				cReadModeA = 0;
				break;


			case 30:
				// m_iMP
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_iMP ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_iMP = atoi(token);
				cReadModeA = 0;
				break;

			case 31:
				// m_iSP
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_iSP ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_iSP = atoi(token);
				cReadModeA = 0;
				break;

			case 32:
				// m_cLU_Pool
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_iLU_Pool ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_iLU_Pool = atoi(token);
				cReadModeA = 0;
				break;
			
			case 38:
				// m_iEnemyKillCount
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_iEnemyKillCount ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_iEnemyKillCount = atoi(token);
				cReadModeA = 0;
				break;

			case 39:
				// m_iPKCount
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_iPKCount ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_iPKCount = atoi(token);
				cReadModeA = 0;
				break;

			case 40:
				// m_iRewardGold
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_iRewardGold ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_iRewardGold = atoi(token);
				cReadModeA = 0;
				break;

			case 41:
				// Skill-SSN Â¼Â³ÃÂ¤Ã‡Ã‘Â´Ã™.
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_iSkillSSN ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_iSkillSSN[cReadModeB] = atoi(token);
				cReadModeB++;

				if (cReadModeB >= DEF_MAXSKILLTYPE) {
					cReadModeA = 0;
					cReadModeB = 0;
				}
				break;

			case 42:
				if (token != 0) {
					ZeroMemory(m_pClientList[iClientH]->m_cProfile, sizeof(m_pClientList[iClientH]->m_cProfile));
					strcpy(m_pClientList[iClientH]->m_cProfile, token);
				}
				cReadModeA = 0;
				break;

			case 43:
				// Hunger-Status Â¼Â³ÃÂ¤Ã‡Ã‘Â´Ã™.
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_iHungerStatus ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_iHungerStatus = atoi(token);
				cReadModeA = 0;
				break;

			case 44:
				// AdminUserLevel
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_iAdminUserLevel ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_iAdminUserLevel = 0; //Default it to 0
				if (atoi(token) > 0) {
					for (i = 0; i < DEF_MAXADMINS; i++) {
						if(strlen(m_stAdminList[i].m_cGMName) == 0) break; //No more GM's on list
						if ((strlen(m_stAdminList[i].m_cGMName)) == (strlen(m_pClientList[iClientH]->m_cCharName))) {
							if(memcmp(m_stAdminList[i].m_cGMName,m_pClientList[iClientH]->m_cCharName,strlen(m_pClientList[iClientH]->m_cCharName)) == 0){
								m_pClientList[iClientH]->m_iAdminUserLevel = atoi(token);
								break; //Break goes to cReadModeA = 0, so no need to do it again
							}
						}
					}
				}
				cReadModeA = 0;
				break;
				
				

			case 45:
				// TimeLeft_ShutUp
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_iTimeLeft_ShutUp ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_iTimeLeft_ShutUp = atoi(token);
				cReadModeA = 0;
				break;

			case 46:
				// TimeLeft_Rating
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_iTimeLeft_Rating ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_iTimeLeft_Rating = atoi(token);
				cReadModeA = 0;
				break;

			case 47:
				// Rating
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_iRating ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_iRating = atoi(token);
				cReadModeA = 0;
				break;

			case 48:
				// Guild GUID: Ã€ÃŒÃ€Ã¼Â¿Â¡ Â»Ã½Â¼ÂºÂµÃ‡Â¾ÃºÂ´Ã¸ Â±Ã¦ÂµÃ¥ÂµÃ©Ã€Âº Â¸Ã°ÂµÃŽ -1Ã€ÃŒ ÂµÃ‡ Â¹Ã¶Â¸Â± Â°ÃÃ€ÃŒÂ´Ã™.
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_iGuildGUID ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_iGuildGUID = atoi(token);
				cReadModeA = 0;
				break;

			case 49:
				// Down Skill Index
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_iDownSkillIndex ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_iDownSkillIndex = atoi(token);
				cReadModeA = 0;
				break;

			case 50:
				m_pClientList[iClientH]->m_ItemPosList[cReadModeB-1].x = atoi(token);
				if (m_pClientList[iClientH]->m_ItemPosList[cReadModeB - 1].x < -10) m_pClientList[iClientH]->m_ItemPosList[cReadModeB - 1].x = -10;

				cReadModeB++;
				if (cReadModeB >= DEF_MAXITEMS) {
					cReadModeA = 0;
					cReadModeB = 0;
				}
				break;

			case 51:
				m_pClientList[iClientH]->m_ItemPosList[cReadModeB-1].y = atoi(token);
				// v1.3 Ã€ÃŸÂ¸Ã¸ÂµÃˆ Ã€Â§Ã„Â¡Â°ÂªÃ€Â» ÂºÂ¸ÃÂ¤Ã‡Ã‘Â´Ã™. 
				if (m_pClientList[iClientH]->m_ItemPosList[cReadModeB-1].y < -10) m_pClientList[iClientH]->m_ItemPosList[cReadModeB-1].y = -10;
				cReadModeB++;
				if (cReadModeB >= DEF_MAXITEMS) {
					cReadModeA = 0;
					cReadModeB = 0;
				}
				break;

			case 52:
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_sCharIDnum1 ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}

				m_pClientList[iClientH]->m_sCharIDnum1 = atoi(token);
				cReadModeA = 0;
				break;

			case 53:
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_sCharIDnum2 ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}

				m_pClientList[iClientH]->m_sCharIDnum2 = atoi(token);
				cReadModeA = 0;
				break;

			case 54:
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_sCharIDnum3 ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}

				m_pClientList[iClientH]->m_sCharIDnum3 = atoi(token);
				cReadModeA = 0;
				break;

			case 55:
				switch (cReadModeB) {
			case 1:
				// Penalty Block Year
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_iPenaltyBlockYear ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_iPenaltyBlockYear = atoi(token);

				cReadModeB = 2;
				break;

			case 2:
				// Penalty Block Month
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_iPenaltyBlockMonth ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_iPenaltyBlockMonth = atoi(token);

				cReadModeB = 3;
				break;

			case 3:
				// Penalty Block day
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_iPenaltyBlockDay ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_iPenaltyBlockDay = atoi(token);

				cReadModeA = 0;
				cReadModeB = 0;
				break;
				}
				break;


			case 56:
				switch (cReadModeB) {
				case 1:
					if (_bGetIsStringIsNumber(token) == false) {
						wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - Connection closed. 56 A ", m_pClientList[iClientH]->m_cCharName);
						PutLogList(cTxt);
						delete[] pContents;
						delete pStrTok;
						return false;
					}

					m_pClientList[iClientH]->m_iQuest[0] = atoi(token);
					cReadModeB = 2;
					break;
				case 2:
					if (_bGetIsStringIsNumber(token) == false) {
						wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - Connection closed. 56 B ", m_pClientList[iClientH]->m_cCharName);
						PutLogList(cTxt);
						delete[] pContents;
						delete pStrTok;
						return false;
					}

					m_pClientList[iClientH]->m_iQuest[1] = atoi(token);
					cReadModeB = 3;
					break;
				case 3:
					if (_bGetIsStringIsNumber(token) == false) {
						wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - Connection closed. 56 C ", m_pClientList[iClientH]->m_cCharName);
						PutLogList(cTxt);
						delete[] pContents;
						delete pStrTok;
						return false;
					}

					m_pClientList[iClientH]->m_iQuest[2] = atoi(token);
					cReadModeB = 4;
					break;
				case 4:
					if (_bGetIsStringIsNumber(token) == false) {
						wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - Connection closed. 56 D ", m_pClientList[iClientH]->m_cCharName);
						PutLogList(cTxt);
						delete[] pContents;
						delete pStrTok;
						return false;
					}

					m_pClientList[iClientH]->m_iQuest[3] = atoi(token);
					cReadModeB = 5;
					break;
				case 5:
					if (_bGetIsStringIsNumber(token) == false) {
						wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - Connection closed. 56 E ", m_pClientList[iClientH]->m_cCharName);
						PutLogList(cTxt);
						delete[] pContents;
						delete pStrTok;
						return false;
					}

					m_pClientList[iClientH]->m_iQuest[4] = atoi(token);
					cReadModeA = 0;
					cReadModeB = 0;
					break;
				}
				break;
			case 57:
				switch (cReadModeB) {
				case 1:
					if (_bGetIsStringIsNumber(token) == false) {
						wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - Connection closed. 57 A ", m_pClientList[iClientH]->m_cCharName);
						PutLogList(cTxt);
						delete[] pContents;
						delete pStrTok;
						return false;
					}

					m_pClientList[iClientH]->m_iCurQuestCount[0] = atoi(token);
					cReadModeB = 2;
					break;
				case 2:
					if (_bGetIsStringIsNumber(token) == false) {
						wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - Connection closed. 57 B ", m_pClientList[iClientH]->m_cCharName);
						PutLogList(cTxt);
						delete[] pContents;
						delete pStrTok;
						return false;
					}

					m_pClientList[iClientH]->m_iCurQuestCount[1] = atoi(token);
					cReadModeB = 3;
					break;
				case 3:
					if (_bGetIsStringIsNumber(token) == false) {
						wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - Connection closed. 57 C ", m_pClientList[iClientH]->m_cCharName);
						PutLogList(cTxt);
						delete[] pContents;
						delete pStrTok;
						return false;
					}

					m_pClientList[iClientH]->m_iCurQuestCount[2] = atoi(token);
					cReadModeB = 4;
					break;
				case 4:
					if (_bGetIsStringIsNumber(token) == false) {
						wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - Connection closed. 57 D ", m_pClientList[iClientH]->m_cCharName);
						PutLogList(cTxt);
						delete[] pContents;
						delete pStrTok;
						return false;
					}

					m_pClientList[iClientH]->m_iCurQuestCount[3] = atoi(token);
					cReadModeB = 5;
					break;
				case 5:
					if (_bGetIsStringIsNumber(token) == false) {
						wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - Connection closed. 57 E ", m_pClientList[iClientH]->m_cCharName);
						PutLogList(cTxt);
						delete[] pContents;
						delete pStrTok;
						return false;
					}

					m_pClientList[iClientH]->m_iCurQuestCount[4] = atoi(token);
					cReadModeA = 0;
					cReadModeB = 0;
					break;
				}
				break;

			case 58:
				cReadModeA = 0;
				break;

			case 59:
				switch (cReadModeB) {
				case 1:
					if (_bGetIsStringIsNumber(token) == false) {
						wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - Connection closed. 59 A ", m_pClientList[iClientH]->m_cCharName);
						PutLogList(cTxt);
						delete[] pContents;
						delete pStrTok;
						return false;
					}

					m_pClientList[iClientH]->m_iQuestRewardType[0] = atoi(token);
					cReadModeB = 2;
					break;
				case 2:
					if (_bGetIsStringIsNumber(token) == false) {
						wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - Connection closed. 59 B ", m_pClientList[iClientH]->m_cCharName);
						PutLogList(cTxt);
						delete[] pContents;
						delete pStrTok;
						return false;
					}

					m_pClientList[iClientH]->m_iQuestRewardType[1] = atoi(token);
					cReadModeB = 3;
					break;
				case 3:
					if (_bGetIsStringIsNumber(token) == false) {
						wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - Connection closed. 59 C ", m_pClientList[iClientH]->m_cCharName);
						PutLogList(cTxt);
						delete[] pContents;
						delete pStrTok;
						return false;
					}

					m_pClientList[iClientH]->m_iQuestRewardType[2] = atoi(token);
					cReadModeB = 4;
					break;
				case 4:
					if (_bGetIsStringIsNumber(token) == false) {
						wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - Connection closed. 59 D ", m_pClientList[iClientH]->m_cCharName);
						PutLogList(cTxt);
						delete[] pContents;
						delete pStrTok;
						return false;
					}

					m_pClientList[iClientH]->m_iQuestRewardType[3] = atoi(token);
					cReadModeB = 5;
					break;
				case 5:
					if (_bGetIsStringIsNumber(token) == false) {
						wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - Connection closed. 59 E ", m_pClientList[iClientH]->m_cCharName);
						PutLogList(cTxt);
						delete[] pContents;
						delete pStrTok;
						return false;
					}

					m_pClientList[iClientH]->m_iQuestRewardType[4] = atoi(token);
					cReadModeA = 0;
					cReadModeB = 0;
					break;
				}
				break;

			case 60:
				switch (cReadModeB) {
				case 1:
					if (_bGetIsStringIsNumber(token) == false) {
						wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - Connection closed. 60 A ", m_pClientList[iClientH]->m_cCharName);
						PutLogList(cTxt);
						delete[] pContents;
						delete pStrTok;
						return false;
					}

					m_pClientList[iClientH]->m_iQuestRewardAmount[0] = atoi(token);
					cReadModeB = 2;
					break;
				case 2:
					if (_bGetIsStringIsNumber(token) == false) {
						wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - Connection closed. 60 B ", m_pClientList[iClientH]->m_cCharName);
						PutLogList(cTxt);
						delete[] pContents;
						delete pStrTok;
						return false;
					}

					m_pClientList[iClientH]->m_iQuestRewardAmount[1] = atoi(token);
					cReadModeB = 3;
					break;
				case 3:
					if (_bGetIsStringIsNumber(token) == false) {
						wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - Connection closed. 60 C ", m_pClientList[iClientH]->m_cCharName);
						PutLogList(cTxt);
						delete[] pContents;
						delete pStrTok;
						return false;
					}

					m_pClientList[iClientH]->m_iQuestRewardAmount[2] = atoi(token);
					cReadModeB = 4;
					break;
				case 4:
					if (_bGetIsStringIsNumber(token) == false) {
						wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - Connection closed. 60 D ", m_pClientList[iClientH]->m_cCharName);
						PutLogList(cTxt);
						delete[] pContents;
						delete pStrTok;
						return false;
					}

					m_pClientList[iClientH]->m_iQuestRewardAmount[3] = atoi(token);
					cReadModeB = 5;
					break;
				case 5:
					if (_bGetIsStringIsNumber(token) == false) {
						wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - Connection closed. 60 E ", m_pClientList[iClientH]->m_cCharName);
						PutLogList(cTxt);
						delete[] pContents;
						delete pStrTok;
						return false;
					}

					m_pClientList[iClientH]->m_iQuestRewardAmount[4] = atoi(token);
					cReadModeA = 0;
					cReadModeB = 0;
					break;
				}
				break;

			case 61:
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_iContribution ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}

				m_pClientList[iClientH]->m_iContribution = atoi(token);
				cReadModeA = 0;
				break;

			case 62:
				switch (cReadModeB) {
				case 1:
					if (_bGetIsStringIsNumber(token) == false) {
						wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - Connection closed. 62 A ", m_pClientList[iClientH]->m_cCharName);
						PutLogList(cTxt);
						delete[] pContents;
						delete pStrTok;
						return false;
					}

					m_pClientList[iClientH]->m_iQuestID[0] = atoi(token);
					cReadModeB = 2;
					break;
				case 2:
					if (_bGetIsStringIsNumber(token) == false) {
						wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - Connection closed. 62 B ", m_pClientList[iClientH]->m_cCharName);
						PutLogList(cTxt);
						delete[] pContents;
						delete pStrTok;
						return false;
					}

					m_pClientList[iClientH]->m_iQuestID[1] = atoi(token);
					cReadModeB = 3;
					break;
				case 3:
					if (_bGetIsStringIsNumber(token) == false) {
						wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - Connection closed. 62 C ", m_pClientList[iClientH]->m_cCharName);
						PutLogList(cTxt);
						delete[] pContents;
						delete pStrTok;
						return false;
					}

					m_pClientList[iClientH]->m_iQuestID[2] = atoi(token);
					cReadModeB = 4;
					break;
				case 4:
					if (_bGetIsStringIsNumber(token) == false) {
						wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - Connection closed. 62 D ", m_pClientList[iClientH]->m_cCharName);
						PutLogList(cTxt);
						delete[] pContents;
						delete pStrTok;
						return false;
					}

					m_pClientList[iClientH]->m_iQuestID[3] = atoi(token);
					cReadModeB = 5;
					break;
				case 5:
					if (_bGetIsStringIsNumber(token) == false) {
						wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - Connection closed. 62 E ", m_pClientList[iClientH]->m_cCharName);
						PutLogList(cTxt);
						delete[] pContents;
						delete pStrTok;
						return false;
					}

					m_pClientList[iClientH]->m_iQuestID[4] = atoi(token);
					cReadModeA = 0;
					cReadModeB = 0;
					break;
				}
				break;

			case 63:
				switch (cReadModeB) {
				case 1:
					if (_bGetIsStringIsNumber(token) == false) {
						wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - Connection closed. 63 A ", m_pClientList[iClientH]->m_cCharName);
						PutLogList(cTxt);
						delete[] pContents;
						delete pStrTok;
						return false;
					}

					m_pClientList[iClientH]->m_bIsQuestCompleted[0] = (bool)atoi(token);
					cReadModeB = 2;
					break;
				case 2:
					if (_bGetIsStringIsNumber(token) == false) {
						wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - Connection closed. 63 B ", m_pClientList[iClientH]->m_cCharName);
						PutLogList(cTxt);
						delete[] pContents;
						delete pStrTok;
						return false;
					}

					m_pClientList[iClientH]->m_bIsQuestCompleted[1] = (bool)atoi(token);
					cReadModeB = 3;
					break;
				case 3:
					if (_bGetIsStringIsNumber(token) == false) {
						wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - Connection closed. 63 C ", m_pClientList[iClientH]->m_cCharName);
						PutLogList(cTxt);
						delete[] pContents;
						delete pStrTok;
						return false;
					}

					m_pClientList[iClientH]->m_bIsQuestCompleted[2] = (bool)atoi(token);
					cReadModeB = 4;
					break;
				case 4:
					if (_bGetIsStringIsNumber(token) == false) {
						wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - Connection closed. 63 D ", m_pClientList[iClientH]->m_cCharName);
						PutLogList(cTxt);
						delete[] pContents;
						delete pStrTok;
						return false;
					}

					m_pClientList[iClientH]->m_bIsQuestCompleted[3] = (bool)atoi(token);
					cReadModeB = 5;
					break;
				case 5:
					if (_bGetIsStringIsNumber(token) == false) {
						wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - Connection closed. 63 E ", m_pClientList[iClientH]->m_cCharName);
						PutLogList(cTxt);
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pClientList[iClientH]->m_bIsQuestCompleted[4] = (bool)atoi(token);
					cReadModeA = 0;
					cReadModeB = 0;
					break;
				}
				break;

			case 64:
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_iTimeLeft_ForceRecall ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = atoi(token);
				cReadModeA = 0;
				break;

			case 65:
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_iTimeLeft_FirmStaminar ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}

				m_pClientList[iClientH]->m_iTimeLeft_FirmStaminar = atoi(token);
				cReadModeA = 0;
				break;

			case 66:
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_iSpecialEventID ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}

				m_pClientList[iClientH]->m_iSpecialEventID = atoi(token);
				cReadModeA = 0;
				break;

			case 67:
				switch (cReadModeB) {
			case 1:
				// FightZone Number
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_iFightzoneNumber ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_iFightzoneNumber = atoi(token);

				cReadModeB = 2;
				break;

			case 2:
				// Â¿Â¹Â¾Ã Ã‡Ã‘ Â½ÃƒÂ°Â£ 
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_iReserveTime ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_iReserveTime = atoi(token);


				cReadModeB = 3;
				break;
			case 3:
				// Â³Â²Ã€Âº Ã†Â¼Ã„ÃÂ¼Ã½Ã€Ãš 
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_iFightZoneTicketNumber ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_iFightZoneTicketNumber = atoi(token);

				cReadModeA = 0;
				cReadModeB = 0;
				break;

				}
				break;

			case 70:
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_iSuperAttackLeft ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}

				m_pClientList[iClientH]->m_iSuperAttackLeft = atoi(token);
				cReadModeA = 0;
				break;

			case 71:
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_iSpecialAbilityTime ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}

				m_pClientList[iClientH]->m_iSpecialAbilityTime = atoi(token);
				cReadModeA = 0;
				break;

			case 72:
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_iWarContribution ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}

				m_pClientList[iClientH]->m_iWarContribution = atoi(token);
				cReadModeA = 0;
				break;

			case 73:
				if (strlen(token) > 10) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_cLockedMapName ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				ZeroMemory(m_pClientList[iClientH]->m_cLockedMapName, sizeof(m_pClientList[iClientH]->m_cLockedMapName));
				strcpy(m_pClientList[iClientH]->m_cLockedMapName, token);
				cReadModeA = 0;
				break;

			case 74:
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_iLockedMapTime ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}

				m_pClientList[iClientH]->m_iLockedMapTime = atoi(token);
				cReadModeA = 0;
				break;

			case 75:
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_iCrusadeDuty ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}

				m_pClientList[iClientH]->m_iCrusadeDuty = atoi(token);
				cReadModeA = 0;
				break;

			case 76:
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_iConstructionPoint ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}

				m_pClientList[iClientH]->m_iConstructionPoint = atoi(token);
				cReadModeA = 0;
				break;

			case 77:
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_dwCrusadeGUID ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}

				m_pClientList[iClientH]->m_dwCrusadeGUID = atoi(token);
				cReadModeA = 0;
				break;

			case 78:
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_iDeadPenaltyTime ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}

				m_pClientList[iClientH]->m_iDeadPenaltyTime = atoi(token);
				cReadModeA = 0;
				break;

			case 79: // v2.06 12-4
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_iPartyID ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}

				m_pClientList[iClientH]->m_iPartyID = atoi(token);
				if (m_pClientList[iClientH]->m_iPartyID != 0) m_pClientList[iClientH]->m_iPartyStatus = DEF_PARTYSTATUS_CONFIRM;
				cReadModeA = 0;
				break;

			case 80: // v2.15 ÃÃ¶ÃÂ¸Â¾Ã†Ã€ÃŒÃ…Ã›Â¾Ã·Â±Ã—Â·Â¹Ã€ÃŒÂµÃ¥
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_iGizonItemUpgradeLeft ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}

				m_pClientList[iClientH]->m_iGizonItemUpgradeLeft = atoi(token);
				if (m_pClientList[iClientH]->m_iGizonItemUpgradeLeft < 0) m_pClientList[iClientH]->m_iGizonItemUpgradeLeft = 0;
				cReadModeA = 0;
				break;

			case 81: 
				cReadModeA = 0; 
				break;

			case 82:
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_dwHeldenianGUID ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}

				m_pClientList[iClientH]->m_dwHeldenianGUID = atoi(token);
				cReadModeA = 0;
				break;

			case 84:
				// m_iMaxEK
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_iMaxEK ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_iMaxEK = atoi(token);
				cReadModeA = 0;
				break;

			case 85:
				// m_iDeaths MORLA 2.2 - Nuevos datos para el pj
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_iDeaths ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_iDeaths = atoi(token);
				cReadModeA = 0;
				break;

			case 86:
				// m_iDGPoints
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_iDGPoints ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_iDGPoints = atoi(token);
				cReadModeA = 0;
				break;

			case 87:
				// character-DGKills
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_iDGKills ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_iDGKills = atoi(token);
				cReadModeA = 0;
				break;

			case 88:
				// character-DGDeaths
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_iTotalDGDeaths ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_iTotalDGDeaths = atoi(token);
				cReadModeA = 0;
				break;

			case 89:
				// character-Kills
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_iTotalDGKills ", m_pClientList[iClientH]->m_cCharName); 
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_iTotalDGKills = atoi(token);
				cReadModeA = 0;
				break;

			case 90:
				// Wanted System
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_iWantedLevel ", m_pClientList[iClientH]->m_cCharName);
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_iWantedLevel   = atoi(token);
				cReadModeA = 0;
				break;

			case 91:
				// Coins
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - Connection closed (Coins). ", m_pClientList[iClientH]->m_cCharName);
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pClientList[iClientH]->m_iCoinPoints = atoi(token);
				cReadModeA = 0;
				break;

			case 92:
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - iteam ", m_pClientList[iClientH]->m_cCharName);
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}

				m_pClientList[iClientH]->iteam = atoi(token);
				cReadModeA = 0;
				break;

			case 93:
				if (_bGetIsStringIsNumber(token) == false) {
					wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_iClass ", m_pClientList[iClientH]->m_cCharName);
					PutLogList(cTxt);
					delete[] pContents;
					delete pStrTok;
					return false;
				}

				m_pClientList[iClientH]->m_iClass = atoi(token);
				cReadModeA = 0;
				break;

			case 94:
				if (m_bSQLMode) {
					if (_bGetIsStringIsNumber(token) == false) {
						wsprintf(cTxt, "(!!!) Player(%s) data file error! wrong Data format - m_dwCharID ", m_pClientList[iClientH]->m_cCharName);
						PutLogList(cTxt);
						delete[] pContents;
						delete pStrTok;
						return false;
					}

					m_pClientList[iClientH]->m_dwCharID = atoi(token);
				}
				cReadModeA = 0;
				break;
				
			}
		}
		else {
			if (memcmp(token, "character-loc-map", 17) == 0)		 cReadModeA = 1;
			if (memcmp(token, "character-loc-x", 15) == 0)			 cReadModeA = 2;
			if (memcmp(token, "character-loc-y", 15) == 0)			 cReadModeA = 3;
			if (memcmp(token, "character-account-status", 21) == 0)  cReadModeA = 4;
			if (memcmp(token, "character-item", 14) == 0) {
				cReadModeA = 5;
				cReadModeB = 1;
				m_pClientList[iClientH]->m_pItemList[iItemIndex] = new class CItem;
			}
			if (memcmp(token, "character-bank-item", 18) == 0) {
				cReadModeA = 28;
				cReadModeB = 1;
				m_pClientList[iClientH]->m_pItemInBankList[iItemInBankIndex] = new class CItem;
			}
			if (memcmp(token, "sex-status", 10) == 0)        cReadModeA = 6;
			if (memcmp(token, "skin-status", 11) == 0)       cReadModeA = 7;
			if (memcmp(token, "hairstyle-status", 16) == 0)  cReadModeA = 8;
			if (memcmp(token, "haircolor-status", 16) == 0)  cReadModeA = 9;
			if (memcmp(token, "underwear-status", 16) == 0)  cReadModeA = 10;
			if (memcmp(token, "item-equip-status", 17) == 0)    cReadModeA = 11;
			if (memcmp(token, "character-guild-name", 20) == 0) cReadModeA = 12;
			if (memcmp(token, "character-guild-rank", 20) == 0) cReadModeA = 13;
			if (memcmp(token, "character-HP", 12) == 0)         cReadModeA = 14;
			if (memcmp(token, "character-DefenseRatio",22)==0)  cReadModeA = 15;
			if (memcmp(token, "character-HitRatio", 18) == 0)   cReadModeA = 16;
			if (memcmp(token, "character-LEVEL", 15) == 0)      cReadModeA = 17;
			if (memcmp(token, "character-STR", 13) == 0)        cReadModeA = 18;
			if (memcmp(token, "character-INT", 13) == 0)        cReadModeA = 19;
			if (memcmp(token, "character-VIT", 13) == 0)        cReadModeA = 20;
			if (memcmp(token, "character-DEX", 13) == 0)        cReadModeA = 21;
			if (memcmp(token, "character-MAG", 13) == 0)        cReadModeA = 22;
			if (memcmp(token, "character-CHARISMA", 18) == 0)   cReadModeA = 23;
			if (memcmp(token, "character-LUCK", 14) == 0)       cReadModeA = 24;
			if (memcmp(token, "character-EXP", 13) == 0)        cReadModeA = 25;
			if (memcmp(token, "magic-mastery", 13) == 0)        cReadModeA = 26;
			if (memcmp(token, "skill-mastery", 13) == 0) {
				cReadModeA = 27;
				cReadModeB = 0;
			}
			if (memcmp(token, "character-location", 18) == 0)   cReadModeA = 29;
			if (memcmp(token, "character-MP", 12) == 0)         cReadModeA = 30;
			if (memcmp(token, "character-SP", 12) == 0)         cReadModeA = 31;
			if (memcmp(token, "character-LU_Pool", 17) == 0)     cReadModeA = 32;
			
			if (memcmp(token, "character-EK-Count",18) == 0)    cReadModeA = 38;
			if (memcmp(token, "character-PK-Count",18) == 0)    cReadModeA = 39;
			if (memcmp(token, "character-reward-gold",21) == 0) cReadModeA = 40;
			if (memcmp(token, "skill-SSN", 9) == 0) 			cReadModeA = 41;
			if (memcmp(token, "character-profile", 17) == 0)	cReadModeA = 42;
			if (memcmp(token, "hunger-status", 13) == 0) 		cReadModeA = 43;
			if (memcmp(token, "admin-user-level", 16) == 0) 	cReadModeA = 44;
			if (memcmp(token, "timeleft-shutup", 15) == 0) 		cReadModeA = 45;
			if (memcmp(token, "timeleft-rating", 15) == 0) 		cReadModeA = 46;
			if (memcmp(token, "character-RATING", 16) == 0)	    cReadModeA = 47;
			if (memcmp(token, "character-guild-GUID", 20) == 0) cReadModeA = 48;
			if (memcmp(token, "character-downskillindex", 24) == 0) cReadModeA = 49;
			if (memcmp(token, "item-position-x", 16) == 0) {
				cReadModeA = 50;
				cReadModeB = 1;
			}
			if (memcmp(token, "item-position-y", 16) == 0) {
				cReadModeA = 51;
				cReadModeB = 1;
			}
			if (memcmp(token, "character-IDnum1",16) == 0)		cReadModeA = 52;
			if (memcmp(token, "character-IDnum2",16) == 0)		cReadModeA = 53;
			if (memcmp(token, "character-IDnum3",16) == 0)		cReadModeA = 54;
			if (memcmp(token, "penalty-block-date",18) == 0) {
				cReadModeA = 55;
				cReadModeB = 1;
			}
			if (memcmp(token, "character-quest-number", 22) == 0) {
				cReadModeA = 56;
				cReadModeB = 1;
			}
			if (memcmp(token, "current-quest-count", 19) == 0) {
				cReadModeA = 57;
				cReadModeB = 1;
			}
			if (memcmp(token, "quest-reward-type", 17) == 0) {
				cReadModeA = 59;
				cReadModeB = 1;
			}
			if (memcmp(token, "quest-reward-amount", 19) == 0) {
				cReadModeA = 60;
				cReadModeB = 1;
			}
			
			if (memcmp(token, "character-contribution", 22) == 0)   cReadModeA = 61;

			if (memcmp(token, "character-quest-ID", 18) == 0) {
				cReadModeA = 62;
				cReadModeB = 1;
			}

			if (memcmp(token, "character-quest-completed", 25) == 0) {
				cReadModeA = 63;
				cReadModeB = 1;
			}

			if (memcmp(token, "timeleft-force-recall", 21) == 0)	cReadModeA = 64;
			if (memcmp(token, "timeleft-firm-staminar", 22) == 0)	cReadModeA = 65;
			if (memcmp(token, "special-event-id", 16) == 0)			cReadModeA = 66;
			if (memcmp(token, "super-attack-left", 17) == 0)		cReadModeA = 70;
			if (memcmp(token, "reserved-fightzone-id", 21) == 0)	{
				cReadModeA = 67;
				cReadModeB = 1;
			}
			if (memcmp(token, "special-ability-time", 20) == 0)       cReadModeA = 71; 
			if (memcmp(token, "character-war-contribution", 26) == 0) cReadModeA = 72; 
			if (memcmp(token, "locked-map-name", 15) == 0) cReadModeA = 73;
			if (memcmp(token, "locked-map-time", 15) == 0) cReadModeA = 74;
			if (memcmp(token, "crusade-job", 11) == 0)     cReadModeA = 75;
			if (memcmp(token, "construct-point", 15) == 0) cReadModeA = 76;
			if (memcmp(token, "crusade-GUID", 12) == 0)    cReadModeA = 77;
			if (memcmp(token, "dead-penalty-time", 17) == 0) cReadModeA = 78;
			if (memcmp(token, "party-id", 8) == 0)           cReadModeA = 79;
			if (memcmp(token, "gizon-item-upgrade-left", 23) == 0) cReadModeA = 80;


			if (memcmp(token, "ip2", 3)== 0) cReadModeA = 81;

			if (memcmp(token, "heldenian-GUID", 14) == 0)    cReadModeA = 82;

			
			if (memcmp(token, "max-ek", 6) == 0) cReadModeA = 84;

			if (memcmp(token, "character-Deaths", 16) == 0)    cReadModeA = 85; // MORLA 2.2 - Nuevos datos para el PJ
			if (memcmp(token, "character-DGPoints", 18) == 0)    cReadModeA = 86;
			if (memcmp(token, "character-DGKills", 17) == 0)    cReadModeA = 87;
			if (memcmp(token, "character-TotalDGDeaths", 23) == 0)    cReadModeA = 88;
			if (memcmp(token, "character-TotalDGKills", 22) == 0)    cReadModeA = 89;
			if (memcmp(token, "character-wanted-level", 22) == 0)    cReadModeA = 90; // // Wanted System
			if (memcmp(token, "coin-points", 11) == 0)    cReadModeA = 91; // // Coins

			if (memcmp(token, "character-team", 14) == 0)    cReadModeA = 92;
			if (memcmp(token, "character-class", 15) == 0)    cReadModeA = 93;
			
			if (m_bSQLMode) {
				if (memcmp(token, "character-dbid", 14) == 0)    cReadModeA = 94;
			}

			if (memcmp(token, "[EOF]", 5) == 0) goto DPDC_STOP_DECODING;
		}
		token = pStrTok->pGet();
	}													  

DPDC_STOP_DECODING:;	

	delete pStrTok;
	delete[] pContents;
	if ((cReadModeA != 0) || (cReadModeB != 0)) {
		wsprintf(cTxt, "(!!!) Player(%s) data file contents error(%d %d)! Connection closed.", m_pClientList[iClientH]->m_cCharName, cReadModeA, cReadModeB);
		PutLogList(cTxt);
		// Debug code @@@@@@@@@@@@@@@
		HANDLE hFile2;
		DWORD  nWrite2;
		hFile2 = CreateFile("Error.Log", GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
		WriteFile(hFile2, (char *)pOriginContents, dwSize+2, &nWrite2, 0);
		CloseHandle(hFile2);
		///////////// @@@@@@@@@@@@@@@
		return false;
	}

	// verified all data below
	bRet = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bIsValidLoc(m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY);
	if (bRet == false) {
		GetMapInitialPoint(m_pClientList[iClientH]->m_cMapIndex, &m_pClientList[iClientH]->m_sX, &m_pClientList[iClientH]->m_sY, m_pClientList[iClientH]->m_cLocation);
	}
	
	if ((m_pClientList[iClientH]->m_iStr < 10) || (m_pClientList[iClientH]->m_iStr > m_sCharStatLimit)) return false;
	if ((m_pClientList[iClientH]->m_iDex < 10) || (m_pClientList[iClientH]->m_iDex > m_sCharStatLimit)) return false;
	if ((m_pClientList[iClientH]->m_iVit < 10) || (m_pClientList[iClientH]->m_iVit > m_sCharStatLimit)) return false;
	if ((m_pClientList[iClientH]->m_iInt < 10) || (m_pClientList[iClientH]->m_iInt > m_sCharStatLimit)) return false;
	if ((m_pClientList[iClientH]->m_iMag < 10) || (m_pClientList[iClientH]->m_iMag > m_sCharStatLimit)) return false;
	if ((m_pClientList[iClientH]->m_iCharisma < 10) || (m_pClientList[iClientH]->m_iCharisma > m_sCharStatLimit)) return false;
	
	if (m_Misc.bCheckValidName(m_pClientList[iClientH]->m_cAccountName) == false) return false;
	if (m_pClientList[iClientH]->m_iPenaltyBlockYear != 0) {
		GetLocalTime(&SysTime);
		iDateSum1 = (UINT32)(m_pClientList[iClientH]->m_iPenaltyBlockYear*10000 + m_pClientList[iClientH]->m_iPenaltyBlockMonth*100 + m_pClientList[iClientH]->m_iPenaltyBlockDay);
		iDateSum2 = (UINT32)(SysTime.wYear*10000 + SysTime.wMonth*100 + SysTime.wDay);
		if (iDateSum1 >= iDateSum2) return false;
	}
	if (m_pClientList[iClientH]->m_iReserveTime != 0) {
		GetLocalTime(&SysTime);
		iDateSum1 = (UINT32)m_pClientList[iClientH]->m_iReserveTime;
		iDateSum2 = (UINT32)(SysTime.wMonth*10000 + SysTime.wDay*100 + SysTime.wHour);
		if (iDateSum2 >= iDateSum1) {
			// SNOOPY: Bug here ii wrong, should be iClientH
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_FIGHTZONERESERVE, -2, 0, 0, 0);
			m_pClientList[iClientH]->m_iFightzoneNumber = 0; 
			m_pClientList[iClientH]->m_iReserveTime = 0;
			m_pClientList[iClientH]->m_iFightZoneTicketNumber = 0;
		}
	}
	if (m_pClientList[iClientH]->m_iAdminUserLevel < 0) m_pClientList[iClientH]->m_iAdminUserLevel = 0;
	if (m_pClientList[iClientH]->m_cSex == 1) {
		sTmpType = 1;
	}
	else if (m_pClientList[iClientH]->m_cSex == 2) {
		sTmpType = 4; 
	}
	switch (m_pClientList[iClientH]->m_cSkin) {
	case 1:
		break;
	case 2:
		sTmpType++;
		break;
	case 3:
		sTmpType += 2;
		break;
	}
	if (m_pClientList[iClientH]->m_iAdminUserLevel >= 10) sTmpType = m_pClientList[iClientH]->m_iAdminUserLevel;
	sTmpAppr1 = (m_pClientList[iClientH]->m_cHairStyle << 8) | (m_pClientList[iClientH]->m_cHairColor << 4) | (m_pClientList[iClientH]->m_cUnderwear);
	m_pClientList[iClientH]->m_sType  = sTmpType;
	m_pClientList[iClientH]->m_sAppr1 = sTmpAppr1;
	iCalcTotalWeight(iClientH);
	if (m_pClientList[iClientH]->m_sCharIDnum1 == 0) {
		int _i, _iTemp1, _iTemp2;
		short _sID1, _sID2, _sID3;
		_iTemp1 = 1;
		_iTemp2 = 1;
		for (_i = 0; _i < 10; _i++) {
			_iTemp1 += m_pClientList[iClientH]->m_cCharName[_i];
			_iTemp2 += abs(m_pClientList[iClientH]->m_cCharName[_i] ^ m_pClientList[iClientH]->m_cCharName[_i]);
		}
		_sID1 = (short)timeGetTime(); 
		_sID2 = (short)_iTemp1; 
		_sID3 = (short)_iTemp2; 
		m_pClientList[iClientH]->m_sCharIDnum1 = _sID1;
		m_pClientList[iClientH]->m_sCharIDnum2 = _sID2;
		m_pClientList[iClientH]->m_sCharIDnum3 = _sID3;
	}
	m_pClientList[iClientH]->m_iRewardGold += iNotUsedItemPrice;
	m_pClientList[iClientH]->m_iSpeedHackCheckExp = m_pClientList[iClientH]->m_iExp;
	return true;
}


int CGame::_iComposePlayerDataFileContents(int iClientH, char* pData)
{
	if (m_bSQLMode) {
		char  cTxt[256], cTmp[256];
		char* cp;

		int   i, * ip, iSize = 0;
		short* sp, * pIDX;
		short Itemidx, BankItemidx;
		UINT32* dwp;

		if (m_pClientList[iClientH] == 0) return 0;

		cp = pData;

		sp = (short*)cp;
		*sp = m_pClientList[iClientH]->m_sCharIDnum1;
		cp += 2;
		iSize += 2;

		sp = (short*)cp;
		*sp = m_pClientList[iClientH]->m_sCharIDnum2;
		cp += 2;
		iSize += 2;

		sp = (short*)cp;
		*sp = m_pClientList[iClientH]->m_sCharIDnum3;
		cp += 2;
		iSize += 2;

		ip = (int*)cp;
		*ip = m_pClientList[iClientH]->m_iLevel;
		cp += 4;
		iSize += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iClientH]->m_iStr;
		cp += 4;
		iSize += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iClientH]->m_iVit;
		cp += 4;
		iSize += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iClientH]->m_iDex;
		cp += 4;
		iSize += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iClientH]->m_iInt;
		cp += 4;
		iSize += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iClientH]->m_iMag;
		cp += 4;
		iSize += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iClientH]->m_iCharisma;
		cp += 4;
		iSize += 4;

		dwp = (UINT32*)cp;
		*dwp = m_pClientList[iClientH]->m_iExp;
		cp += 4;
		iSize += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iClientH]->m_iLU_Pool;
		cp += 4;
		iSize += 4;

		*cp = m_pClientList[iClientH]->m_cSex;
		cp++;
		iSize++;

		*cp = m_pClientList[iClientH]->m_cSkin;
		cp++;
		iSize++;

		*cp = m_pClientList[iClientH]->m_cHairStyle;
		cp++;
		iSize++;

		*cp = m_pClientList[iClientH]->m_cHairColor;
		cp++;
		iSize++;

		*cp = m_pClientList[iClientH]->m_cUnderwear;
		cp++;
		iSize++;

		ip = (int*)cp;
		*ip = m_pClientList[iClientH]->m_iApprColor;
		cp += 4;
		iSize += 4;

		sp = (short*)cp;
		*sp = m_pClientList[iClientH]->m_sAppr1;
		cp += 2;
		iSize += 2;

		sp = (short*)cp;
		*sp = m_pClientList[iClientH]->m_sAppr2;
		cp += 2;
		iSize += 2;

		sp = (short*)cp;
		*sp = m_pClientList[iClientH]->m_sAppr3;
		cp += 2;
		iSize += 2;

		sp = (short*)cp;
		*sp = m_pClientList[iClientH]->m_sAppr4;
		cp += 2;
		iSize += 2;

		memcpy(cp, m_pClientList[iClientH]->m_cLocation, 10);
		cp += 10;
		iSize += 10;

		memcpy(cp, m_pClientList[iClientH]->m_cMapName, 10);
		cp += 10;
		iSize += 10;

		sp = (short*)cp;
		*sp = m_pClientList[iClientH]->m_sX;
		cp += 2;
		iSize += 2;

		sp = (short*)cp;
		*sp = m_pClientList[iClientH]->m_sY;
		cp += 2;
		iSize += 2;

		ip = (int*)cp;
		*ip = m_pClientList[iClientH]->m_iAdminUserLevel;
		cp += 4;
		iSize += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iClientH]->m_iContribution;
		cp += 4;
		iSize += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iClientH]->m_iSpecialAbilityTime;
		cp += 4;
		iSize += 4;

		memcpy(cp, m_pClientList[iClientH]->m_cLockedMapName, 10);
		cp += 10;
		iSize += 10;

		ip = (int*)cp;
		*ip = m_pClientList[iClientH]->m_iLockedMapTime;
		cp += 4;
		iSize += 4;

		//--
		ip = (int*)cp;
		*ip = m_pClientList[iClientH]->m_iPenaltyBlockYear;
		cp += 4;
		iSize += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iClientH]->m_iPenaltyBlockMonth;
		cp += 4;
		iSize += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iClientH]->m_iPenaltyBlockDay;
		cp += 4;
		iSize += 4;
		//--

		memcpy(cp, m_pClientList[iClientH]->m_cGuildName, 20);
		cp += 20;
		iSize += 20;

		ip = (int*)cp;
		*ip = m_pClientList[iClientH]->m_iGuildGUID;
		cp += 4;
		iSize += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iClientH]->m_iGuildRank;
		cp += 4;
		iSize += 4;

		for (i = 0; i < 5; i++)
		{
			ip = (int*)cp;
			*ip = m_pClientList[iClientH]->m_iQuest[i];
			cp += 4;
			iSize += 4;

			ip = (int*)cp;
			*ip = m_pClientList[iClientH]->m_iQuestID[i];
			cp += 4;
			iSize += 4;

			ip = (int*)cp;
			*ip = m_pClientList[iClientH]->m_iCurQuestCount[i];
			cp += 4;
			iSize += 4;

			ip = (int*)cp;
			*ip = m_pClientList[iClientH]->m_iQuestRewardType[i];
			cp += 4;
			iSize += 4;

			ip = (int*)cp;
			*ip = m_pClientList[iClientH]->m_iQuestRewardAmount[i];
			cp += 4;
			iSize += 4;

			*cp = (char)m_pClientList[iClientH]->m_bIsQuestCompleted[i];
			cp++;
			iSize++;
		}

		ip = (int*)cp;
		*ip = m_pClientList[iClientH]->m_iSpecialEventID;
		cp += 4;
		iSize += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iClientH]->m_iWarContribution;
		cp += 4;
		iSize += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iClientH]->m_iCrusadeDuty;
		cp += 4;
		iSize += 4;

		dwp = (UINT32*)cp;
		*dwp = m_pClientList[iClientH]->m_dwCrusadeGUID;
		cp += 4;
		iSize += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iClientH]->m_iConstructionPoint;
		cp += 4;
		iSize += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iClientH]->m_iRating;
		cp += 4;
		iSize += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iClientH]->m_iHP;
		cp += 4;
		iSize += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iClientH]->m_iMP;
		cp += 4;
		iSize += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iClientH]->m_iSP;
		cp += 4;
		iSize += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iClientH]->m_iEnemyKillCount;
		cp += 4;
		iSize += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iClientH]->m_iPKCount;
		cp += 4;
		iSize += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iClientH]->m_iRewardGold;
		cp += 4;
		iSize += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iClientH]->m_iHungerStatus;
		cp += 4;
		iSize += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iClientH]->m_iSuperAttackLeft;
		cp += 4;
		iSize += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iClientH]->m_iTimeLeft_ShutUp;
		cp += 4;
		iSize += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iClientH]->m_iTimeLeft_Rating;
		cp += 4;
		iSize += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iClientH]->m_iTimeLeft_ForceRecall;
		cp += 4;
		iSize += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iClientH]->m_iTimeLeft_FirmStaminar;
		cp += 4;
		iSize += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iClientH]->m_iDeadPenaltyTime;
		cp += 4;
		iSize += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iClientH]->m_iPartyID;
		cp += 4;
		iSize += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iClientH]->m_iGizonItemUpgradeLeft;
		cp += 4;
		iSize += 4;

		//
		ip = (int*)cp;
		*ip = m_pClientList[iClientH]->m_iDeaths;
		cp += 4;
		iSize += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iClientH]->m_iTotalDGKills;
		cp += 4;
		iSize += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iClientH]->m_iDGPoints;
		cp += 4;
		iSize += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iClientH]->m_iTotalDGDeaths;
		cp += 4;
		iSize += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iClientH]->m_iDGKills;
		cp += 4;
		iSize += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iClientH]->m_iMaxEK;
		cp += 4;
		iSize += 4;

		dwp = (UINT32*)cp;
		*dwp = m_pClientList[iClientH]->m_dwHeldenianGUID;
		cp += 4;
		iSize += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iClientH]->m_iWantedLevel;
		cp += 4;
		iSize += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iClientH]->m_iCoinPoints;
		cp += 4;
		iSize += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iClientH]->iteam;
		cp += 4;
		iSize += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iClientH]->m_iClass;
		cp += 4;
		iSize += 4;
		//

		ZeroMemory(cTxt, sizeof(cTxt));
		ZeroMemory(cTmp, sizeof(cTmp));

		for (i = 0; i < DEF_MAXMAGICTYPE; i++) {
			wsprintf(cTmp, "%d", m_pClientList[iClientH]->m_cMagicMastery[i]);
			strcat(cTxt, cTmp);
		}
		memcpy(cp, cTxt, 100);
		cp += 100;
		iSize += 100;

		ZeroMemory(cTxt, sizeof(cTxt));
		ZeroMemory(cTmp, sizeof(cTmp));
		for (i = 0; i < DEF_MAXITEMS; i++)
		{
			wsprintf(cTmp, "%d", (char)m_pClientList[iClientH]->m_bIsItemEquipped[i]);
			strcat(cTxt, cTmp);
		}
		memcpy(cp, cTxt, 50);
		cp += 50;
		iSize += 50;

		ZeroMemory(cTxt, sizeof(cTxt));
		ZeroMemory(cTmp, sizeof(cTmp));
		for (i = 0; i < DEF_MAXITEMS; i++)
		{
			wsprintf(cTmp, "%d ", (short)m_pClientList[iClientH]->m_ItemPosList[i].x);
			strcat(cTxt, cTmp);
		}
		memcpy(cp, cTxt, 200);
		cp += 200;
		iSize += 200;

		ZeroMemory(cTxt, sizeof(cTxt));
		ZeroMemory(cTmp, sizeof(cTmp));
		for (i = 0; i < DEF_MAXITEMS; i++)
		{
			wsprintf(cTmp, "%d ", (short)m_pClientList[iClientH]->m_ItemPosList[i].y);
			strcat(cTxt, cTmp);
		}
		memcpy(cp, cTxt, 200);
		cp += 200;
		iSize += 200;

		// skills
		ZeroMemory(cTxt, sizeof(cTxt));
		ZeroMemory(cTmp, sizeof(cTmp));
		for (i = 0; i < DEF_MAXSKILLTYPE; i++) {
			wsprintf(cTmp, "%d ", m_pClientList[iClientH]->m_cSkillMastery[i]);
			strcat(cTxt, cTmp);
		}
		memcpy(cp, cTxt, 170);
		cp += 170;
		iSize += 170;

		ZeroMemory(cTxt, sizeof(cTxt));
		ZeroMemory(cTmp, sizeof(cTmp));
		for (i = 0; i < DEF_MAXSKILLTYPE; i++) {
			wsprintf(cTmp, "%d ", m_pClientList[iClientH]->m_iSkillSSN[i]);
			strcat(cTxt, cTmp);
		}
		memcpy(cp, cTxt, 170);
		cp += 170;
		iSize += 170;

		Itemidx = 0;

		pIDX = (short*)cp;
		cp += 2;
		iSize += 2;

		UINT16* wp;

		for (i = 0; i < DEF_MAXITEMS; i++)
		{
			if (m_pClientList[iClientH]->m_pItemList[i] != 0)
			{
				memcpy(cp, m_pClientList[iClientH]->m_pItemList[i]->m_cName, 20);
				cp += 20;
				iSize += 20;

				dwp = (UINT32*)cp;
				*dwp = m_pClientList[iClientH]->m_pItemList[i]->m_dwCount;
				cp += 4;
				iSize += 4;

				sp = (short*)cp;
				*sp = m_pClientList[iClientH]->m_pItemList[i]->m_sTouchEffectType;
				cp += 2;
				iSize += 2;

				sp = (short*)cp;
				*sp = m_pClientList[iClientH]->m_pItemList[i]->m_sTouchEffectValue1;
				cp += 2;
				iSize += 2;

				sp = (short*)cp;
				*sp = m_pClientList[iClientH]->m_pItemList[i]->m_sTouchEffectValue2;
				cp += 2;
				iSize += 2;

				sp = (short*)cp;
				*sp = m_pClientList[iClientH]->m_pItemList[i]->m_sTouchEffectValue3;
				cp += 2;
				iSize += 2;

				sp = (short*)cp;
				*sp = (short)m_pClientList[iClientH]->m_pItemList[i]->m_cItemColor;
				cp += 2;
				iSize += 2;

				sp = (short*)cp;
				*sp = m_pClientList[iClientH]->m_pItemList[i]->m_sItemSpecEffectValue1;
				cp += 2;
				iSize += 2;

				sp = (short*)cp;
				*sp = m_pClientList[iClientH]->m_pItemList[i]->m_sItemSpecEffectValue2;
				cp += 2;
				iSize += 2;

				sp = (short*)cp;
				*sp = m_pClientList[iClientH]->m_pItemList[i]->m_sItemSpecEffectValue3;
				cp += 2;
				iSize += 2;

				wp = (UINT16*)cp;
				*wp = m_pClientList[iClientH]->m_pItemList[i]->m_wCurLifeSpan;
				cp += 2;
				iSize += 2;

				dwp = (UINT32*)cp;
				*dwp = m_pClientList[iClientH]->m_pItemList[i]->m_dwAttribute;
				cp += 4;
				iSize += 4;

				sp = (short*)cp;
				*sp = m_pClientList[iClientH]->m_pItemList[i]->m_sNewEffect1;
				cp += 2;
				iSize += 2;

				sp = (short*)cp;
				*sp = m_pClientList[iClientH]->m_pItemList[i]->m_sNewEffect2;
				cp += 2;
				iSize += 2;

				sp = (short*)cp;
				*sp = m_pClientList[iClientH]->m_pItemList[i]->m_sNewEffect3;
				cp += 2;
				iSize += 2;

				sp = (short*)cp;
				*sp = m_pClientList[iClientH]->m_pItemList[i]->m_sNewEffect4;
				cp += 2;
				iSize += 2;

				Itemidx++;
			}
		}

		*pIDX = Itemidx;

		pIDX = (short*)cp;
		cp += 2;
		iSize += 2;

		BankItemidx = 0;

		for (i = 0; i < DEF_MAXBANKITEMS; i++)
		{
			if (m_pClientList[iClientH]->m_pItemInBankList[i] != 0)
			{
				memcpy(cp, m_pClientList[iClientH]->m_pItemInBankList[i]->m_cName, 20);
				cp += 20;
				iSize += 20;

				dwp = (UINT32*)cp;
				*dwp = m_pClientList[iClientH]->m_pItemInBankList[i]->m_dwCount;
				cp += 4;
				iSize += 4;

				sp = (short*)cp;
				*sp = m_pClientList[iClientH]->m_pItemInBankList[i]->m_sTouchEffectType;
				cp += 2;
				iSize += 2;

				sp = (short*)cp;
				*sp = m_pClientList[iClientH]->m_pItemInBankList[i]->m_sTouchEffectValue1;
				cp += 2;
				iSize += 2;

				sp = (short*)cp;
				*sp = m_pClientList[iClientH]->m_pItemInBankList[i]->m_sTouchEffectValue2;
				cp += 2;
				iSize += 2;

				sp = (short*)cp;
				*sp = m_pClientList[iClientH]->m_pItemInBankList[i]->m_sTouchEffectValue3;
				cp += 2;
				iSize += 2;

				sp = (short*)cp;
				*sp = (short)m_pClientList[iClientH]->m_pItemInBankList[i]->m_cItemColor;
				cp += 2;
				iSize += 2;

				sp = (short*)cp;
				*sp = m_pClientList[iClientH]->m_pItemInBankList[i]->m_sItemSpecEffectValue1;
				cp += 2;
				iSize += 2;

				sp = (short*)cp;
				*sp = m_pClientList[iClientH]->m_pItemInBankList[i]->m_sItemSpecEffectValue2;
				cp += 2;
				iSize += 2;

				sp = (short*)cp;
				*sp = m_pClientList[iClientH]->m_pItemInBankList[i]->m_sItemSpecEffectValue3;
				cp += 2;
				iSize += 2;

				wp = (UINT16*)cp;
				*wp = m_pClientList[iClientH]->m_pItemInBankList[i]->m_wCurLifeSpan;
				cp += 2;
				iSize += 2;

				dwp = (UINT32*)cp;
				*dwp = m_pClientList[iClientH]->m_pItemInBankList[i]->m_dwAttribute;
				cp += 4;
				iSize += 4;

				sp = (short*)cp;
				*sp = m_pClientList[iClientH]->m_pItemInBankList[i]->m_sNewEffect1;
				cp += 2;
				iSize += 2;

				sp = (short*)cp;
				*sp = m_pClientList[iClientH]->m_pItemInBankList[i]->m_sNewEffect2;
				cp += 2;
				iSize += 2;

				sp = (short*)cp;
				*sp = m_pClientList[iClientH]->m_pItemInBankList[i]->m_sNewEffect3;
				cp += 2;
				iSize += 2;

				sp = (short*)cp;
				*sp = m_pClientList[iClientH]->m_pItemInBankList[i]->m_sNewEffect4;
				cp += 2;
				iSize += 2;

				BankItemidx++;
			}
		}

		*pIDX = BankItemidx;

		return iSize;
	}
	else {
		SYSTEMTIME SysTime;
		char  cTxt[120], cTmp[21];
		POINT TempItemPosList[DEF_MAXITEMS];
		int   i;

		if (m_pClientList[iClientH] == NULL) return 0;

		GetLocalTime(&SysTime);
		strcat(pData, "[FILE-DATE]\n\n");

		wsprintf(cTxt, "file-saved-date: %d %d %d %d %d\n", SysTime.wYear, SysTime.wMonth, SysTime.wDay, SysTime.wHour, SysTime.wMinute);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		strcat(pData, "[NAME-ACCOUNT]\n\n");
		strcat(pData, "character-name     = ");
		strcat(pData, m_pClientList[iClientH]->m_cCharName);
		strcat(pData, "\n");
		strcat(pData, "account-name       = ");
		strcat(pData, m_pClientList[iClientH]->m_cAccountName);
		strcat(pData, "\n\n");

		strcat(pData, "[STATUS]\n\n");
		strcat(pData, "character-profile 	=");
		if (strlen(m_pClientList[iClientH]->m_cProfile) == 0) {
			strcat(pData, "__________");
		}
		else strcat(pData, m_pClientList[iClientH]->m_cProfile);
		strcat(pData, "\n");

		strcat(pData, "character-location   = ");
		strcat(pData, m_pClientList[iClientH]->m_cLocation);
		strcat(pData, "\n");

		strcat(pData, "character-guild-name = ");
		if (m_pClientList[iClientH]->m_iGuildRank != -1) {
			strcat(pData, m_pClientList[iClientH]->m_cGuildName);
		}
		else strcat(pData, "NONE");
		strcat(pData, "\n");

		strcat(pData, "character-guild-GUID = ");
		if (m_pClientList[iClientH]->m_iGuildRank != -1) {
			ZeroMemory(cTxt, sizeof(cTxt));
			wsprintf(cTxt, "%d", m_pClientList[iClientH]->m_iGuildGUID);
			strcat(pData, cTxt);
		}
		else strcat(pData, "-1");
		strcat(pData, "\n");

		strcat(pData, "character-guild-rank = ");
		itoa(m_pClientList[iClientH]->m_iGuildRank, cTxt, 10);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		strcat(pData, "character-loc-map = ");
		strcat(pData, m_pClientList[iClientH]->m_cMapName);
		strcat(pData, "\n");
		
		strcat(pData, "character-loc-x   = ");
		itoa(m_pClientList[iClientH]->m_sX, cTxt, 10);
		strcat(pData, cTxt);
		strcat(pData, "\n");
		
		strcat(pData, "character-loc-y   = ");
		itoa(m_pClientList[iClientH]->m_sY, cTxt, 10);
		strcat(pData, cTxt);
		strcat(pData, "\n\n");
		//
		if (m_pClientList[iClientH]->m_iHP <= 0) m_pClientList[iClientH]->m_iHP = iGetMaxHP(iClientH);
		wsprintf(cTxt, "character-HP       = %d", m_pClientList[iClientH]->m_iHP);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		if (m_pClientList[iClientH]->m_iMP < 0) m_pClientList[iClientH]->m_iMP = 0;
		wsprintf(cTxt, "character-MP       = %d", m_pClientList[iClientH]->m_iMP);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		if (m_pClientList[iClientH]->m_iSP < 0) m_pClientList[iClientH]->m_iSP = 0; // v1.1
		wsprintf(cTxt, "character-SP       = %d", m_pClientList[iClientH]->m_iSP);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "character-LEVEL    = %d", m_pClientList[iClientH]->m_iLevel);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "character-RATING   = %d", m_pClientList[iClientH]->m_iRating);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "character-STR      = %d", m_pClientList[iClientH]->m_iStr);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "character-INT      = %d", m_pClientList[iClientH]->m_iInt);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "character-VIT      = %d", m_pClientList[iClientH]->m_iVit);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "character-DEX      = %d", m_pClientList[iClientH]->m_iDex);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "character-MAG      = %d", m_pClientList[iClientH]->m_iMag);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "character-CHARISMA = %d", m_pClientList[iClientH]->m_iCharisma);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "character-LUCK     = %d", m_pClientList[iClientH]->m_iLuck);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "character-EXP      = %d", m_pClientList[iClientH]->m_iExp);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "character-LU_Pool  = %d", m_pClientList[iClientH]->m_iLU_Pool);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "character-EK-Count = %d", m_pClientList[iClientH]->m_iEnemyKillCount);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "character-PK-Count = %d", m_pClientList[iClientH]->m_iPKCount);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "character-reward-gold = %d", m_pClientList[iClientH]->m_iRewardGold);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "character-downskillindex = %d", m_pClientList[iClientH]->m_iDownSkillIndex);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "character-IDnum1 = %d", m_pClientList[iClientH]->m_sCharIDnum1);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "character-IDnum2 = %d", m_pClientList[iClientH]->m_sCharIDnum2);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "character-IDnum3 = %d", m_pClientList[iClientH]->m_sCharIDnum3);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "party-rank = %d", m_pClientList[iClientH]->m_iPartyRank);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		strcat(pData, "sex-status       = ");
		itoa(m_pClientList[iClientH]->m_cSex, cTxt, 10);
		strcat(pData, cTxt);
		strcat(pData, "\n");
		strcat(pData, "skin-status      = ");
		itoa(m_pClientList[iClientH]->m_cSkin, cTxt, 10);
		strcat(pData, cTxt);
		strcat(pData, "\n");
		strcat(pData, "hairstyle-status = ");
		itoa(m_pClientList[iClientH]->m_cHairStyle, cTxt, 10);
		strcat(pData, cTxt);
		strcat(pData, "\n");
		strcat(pData, "haircolor-status = ");
		itoa(m_pClientList[iClientH]->m_cHairColor, cTxt, 10);
		strcat(pData, cTxt);
		strcat(pData, "\n");
		strcat(pData, "underwear-status = ");
		itoa(m_pClientList[iClientH]->m_cUnderwear, cTxt, 10);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "hunger-status    = %d", m_pClientList[iClientH]->m_iHungerStatus);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "timeleft-shutup  = %d", m_pClientList[iClientH]->m_iTimeLeft_ShutUp);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "timeleft-rating  = %d", m_pClientList[iClientH]->m_iTimeLeft_Rating);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "timeleft-force-recall  = %d", m_pClientList[iClientH]->m_iTimeLeft_ForceRecall);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "timeleft-firm-staminar = %d", m_pClientList[iClientH]->m_iTimeLeft_FirmStaminar);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "admin-user-level = %d", m_pClientList[iClientH]->m_iAdminUserLevel);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "penalty-block-date = %d %d %d", m_pClientList[iClientH]->m_iPenaltyBlockYear, m_pClientList[iClientH]->m_iPenaltyBlockMonth, m_pClientList[iClientH]->m_iPenaltyBlockDay);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		//Magn0S:: Multi Quest
		strcat(pData, "character-quest-number = ");
		for (i = 0; i < DEF_MAXQUEST; i++) {
			wsprintf(cTxt, "%d ", m_pClientList[iClientH]->m_iQuest[i]);
			strcat(pData, cTxt);
		}
		strcat(pData, "\n");

		strcat(pData, "character-quest-ID = ");
		for (i = 0; i < DEF_MAXQUEST; i++) {
			wsprintf(cTxt, "%d ", m_pClientList[iClientH]->m_iQuestID[i]);
			strcat(pData, cTxt);
		}
		strcat(pData, "\n");

		strcat(pData, "current-quest-count = ");
		for (i = 0; i < DEF_MAXQUEST; i++) {
			wsprintf(cTxt, "%d ", m_pClientList[iClientH]->m_iCurQuestCount[i]);
			strcat(pData, cTxt);
		}
		strcat(pData, "\n");

		strcat(pData, "quest-reward-type = ");
		for (i = 0; i < DEF_MAXQUEST; i++) {
			wsprintf(cTxt, "%d ", m_pClientList[iClientH]->m_iQuestRewardType[i]);
			strcat(pData, cTxt);
		}
		strcat(pData, "\n");

		strcat(pData, "quest-reward-amount = ");
		for (i = 0; i < DEF_MAXQUEST; i++) {
			wsprintf(cTxt, "%d ", m_pClientList[iClientH]->m_iQuestRewardAmount[i]);
			strcat(pData, cTxt);
		}
		strcat(pData, "\n");

		wsprintf(cTxt, "character-contribution = %d", m_pClientList[iClientH]->m_iContribution);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "character-war-contribution = %d", m_pClientList[iClientH]->m_iWarContribution);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		strcat(pData, "character-quest-completed = ");
		for (i = 0; i < DEF_MAXQUEST; i++) {
			wsprintf(cTxt, "%d ", (int)m_pClientList[iClientH]->m_bIsQuestCompleted[i]);
			strcat(pData, cTxt);
		}
		strcat(pData, "\n");

		wsprintf(cTxt, "special-event-id = %d", m_pClientList[iClientH]->m_iSpecialEventID);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "super-attack-left = %d", m_pClientList[iClientH]->m_iSuperAttackLeft);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "reserved-fightzone-id = %d %d %d", m_pClientList[iClientH]->m_iFightzoneNumber, m_pClientList[iClientH]->m_iReserveTime, m_pClientList[iClientH]->m_iFightZoneTicketNumber);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "special-ability-time = %d", m_pClientList[iClientH]->m_iSpecialAbilityTime);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "locked-map-name = %s", m_pClientList[iClientH]->m_cLockedMapName);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "locked-map-time = %d", m_pClientList[iClientH]->m_iLockedMapTime);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "crusade-job = %d", m_pClientList[iClientH]->m_iCrusadeDuty);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "crusade-GUID = %d", m_pClientList[iClientH]->m_dwCrusadeGUID);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "construct-point = %d", m_pClientList[iClientH]->m_iConstructionPoint);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "dead-penalty-time = %d", m_pClientList[iClientH]->m_iDeadPenaltyTime);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "party-id = %d", m_pClientList[iClientH]->m_iPartyID);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "gizon-item-upgrade-left = %d", m_pClientList[iClientH]->m_iGizonItemUpgradeLeft);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		// MORLA 2.2- Nuevos datos para el pj
		wsprintf(cTxt, "character-Deaths = %d", m_pClientList[iClientH]->m_iDeaths);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "character-TotalDGKills = %d", m_pClientList[iClientH]->m_iTotalDGKills);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "character-DGPoints = %d", m_pClientList[iClientH]->m_iDGPoints);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "character-TotalDGDeaths = %d", m_pClientList[iClientH]->m_iTotalDGDeaths);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "character-DGKills = %d", m_pClientList[iClientH]->m_iDGKills);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "max-ek = %d", m_pClientList[iClientH]->m_iMaxEK);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		strcat(pData, "ip2 = ");
		strcat(pData, m_pClientList[iClientH]->m_cIP2);
		strcat(pData, "\n");

		strcat(pData, "ip = ");
		strcat(pData, m_pClientList[iClientH]->m_cIPaddress);
		strcat(pData, "\n");

		wsprintf(cTxt, "heldenian-GUID = %d", m_pClientList[iClientH]->m_dwHeldenianGUID);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "character-wanted-level = %d", m_pClientList[iClientH]->m_iWantedLevel);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "coin-points = %d", m_pClientList[iClientH]->m_iCoinPoints);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "character-team = %d", m_pClientList[iClientH]->iteam);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		// Centuu : 1 War | 2 Mage | 3 Archer 
		wsprintf(cTxt, "character-class = %d", m_pClientList[iClientH]->m_iClass);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		strcat(pData, "\n\n");

		strcat(pData, "appr1 = ");
		itoa(m_pClientList[iClientH]->m_sAppr1, cTxt, 10);
		strcat(pData, cTxt);
		strcat(pData, "\n");
		strcat(pData, "appr2 = ");
		
		itoa(m_pClientList[iClientH]->m_sAppr2, cTxt, 10);
		strcat(pData, cTxt);
		strcat(pData, "\n");
		strcat(pData, "appr3 = ");
		itoa(m_pClientList[iClientH]->m_sAppr3, cTxt, 10);
		strcat(pData, cTxt);
		strcat(pData, "\n");
		strcat(pData, "appr4 = ");
		itoa(m_pClientList[iClientH]->m_sAppr4, cTxt, 10);
		strcat(pData, cTxt);
		strcat(pData, "\n");
		
		strcat(pData, "appr-color = ");
		itoa(m_pClientList[iClientH]->m_iApprColor, cTxt, 10);
		strcat(pData, cTxt);
		strcat(pData, "\n\n");

		strcat(pData, "[ITEMLIST]\n\n");

		for (i = 0; i < DEF_MAXITEMS; i++) { 
			TempItemPosList[i].x = 40;
			TempItemPosList[i].y = 30;
		}

		for (i = 0; i < DEF_MAXITEMS; i++) {
			if (m_pClientList[iClientH]->m_pItemList[i] != NULL) {

				if (m_pClientList[iClientH]->m_pItemList[i]->teamcape) continue;
				if (m_pClientList[iClientH]->m_pItemList[i]->teamboots) continue;

				TempItemPosList[i].x = m_pClientList[iClientH]->m_ItemPosList[i].x;
				TempItemPosList[i].y = m_pClientList[iClientH]->m_ItemPosList[i].y;

				strcat(pData, "character-item = ");
				memset(cTmp, ' ', 21);
				strcpy(cTmp, m_pClientList[iClientH]->m_pItemList[i]->m_cName);
				cTmp[strlen(m_pClientList[iClientH]->m_pItemList[i]->m_cName)] = (char)' ';
				cTmp[20] = NULL;
				strcat(pData, cTmp);
				strcat(pData, " ");
				itoa(m_pClientList[iClientH]->m_pItemList[i]->m_dwCount, cTxt, 10);
				strcat(pData, cTxt);
				strcat(pData, " ");
				itoa(m_pClientList[iClientH]->m_pItemList[i]->m_sTouchEffectType, cTxt, 10);
				strcat(pData, cTxt);
				strcat(pData, " ");
				itoa(m_pClientList[iClientH]->m_pItemList[i]->m_sTouchEffectValue1, cTxt, 10);
				strcat(pData, cTxt);
				strcat(pData, " ");
				itoa(m_pClientList[iClientH]->m_pItemList[i]->m_sTouchEffectValue2, cTxt, 10);
				strcat(pData, cTxt);
				strcat(pData, " ");
				itoa(m_pClientList[iClientH]->m_pItemList[i]->m_sTouchEffectValue3, cTxt, 10);
				strcat(pData, cTxt);
				strcat(pData, " ");
				itoa(m_pClientList[iClientH]->m_pItemList[i]->m_cItemColor, cTxt, 10);
				strcat(pData, cTxt);
				strcat(pData, " ");
				itoa(m_pClientList[iClientH]->m_pItemList[i]->m_sItemSpecEffectValue1, cTxt, 10);
				strcat(pData, cTxt);
				strcat(pData, " ");
				itoa(m_pClientList[iClientH]->m_pItemList[i]->m_sItemSpecEffectValue2, cTxt, 10);
				strcat(pData, cTxt);
				strcat(pData, " ");
				itoa(m_pClientList[iClientH]->m_pItemList[i]->m_sItemSpecEffectValue3, cTxt, 10);
				strcat(pData, cTxt);
				strcat(pData, " ");
				itoa(m_pClientList[iClientH]->m_pItemList[i]->m_wCurLifeSpan, cTxt, 10);
				strcat(pData, cTxt);
				strcat(pData, " ");
				itoa(m_pClientList[iClientH]->m_pItemList[i]->m_dwAttribute, cTxt, 10);
				strcat(pData, cTxt);
				strcat(pData, " ");
				//----------------------------------------------------------------------------
				itoa(m_pClientList[iClientH]->m_pItemList[i]->m_sNewEffect1, cTxt, 10);
				strcat(pData, cTxt);
				strcat(pData, " ");
				//--------------------
				itoa(m_pClientList[iClientH]->m_pItemList[i]->m_sNewEffect2, cTxt, 10);
				strcat(pData, cTxt);
				strcat(pData, " ");
				//--------------------
				itoa(m_pClientList[iClientH]->m_pItemList[i]->m_sNewEffect3, cTxt, 10);
				strcat(pData, cTxt);
				strcat(pData, " ");
				//--------------------
				itoa(m_pClientList[iClientH]->m_pItemList[i]->m_sNewEffect4, cTxt, 10);
				strcat(pData, cTxt);
				strcat(pData, "\n");
			}
		}
		strcat(pData, "\n\n");

		for (i = 0; i < DEF_MAXITEMS; i++) {
			m_pClientList[iClientH]->m_ItemPosList[i].x = TempItemPosList[i].x;
			m_pClientList[iClientH]->m_ItemPosList[i].y = TempItemPosList[i].y;
		}

		for (i = 0; i < DEF_MAXBANKITEMS; i++) {
			if (m_pClientList[iClientH]->m_pItemInBankList[i] != NULL) {
				strcat(pData, "character-bank-item = ");
				memset(cTmp, ' ', 21);
				strcpy(cTmp, m_pClientList[iClientH]->m_pItemInBankList[i]->m_cName);
				cTmp[strlen(m_pClientList[iClientH]->m_pItemInBankList[i]->m_cName)] = (char)' ';
				cTmp[20] = NULL;
				strcat(pData, cTmp);
				strcat(pData, " ");
				itoa(m_pClientList[iClientH]->m_pItemInBankList[i]->m_dwCount, cTxt, 10);
				strcat(pData, cTxt);
				strcat(pData, " ");
				itoa(m_pClientList[iClientH]->m_pItemInBankList[i]->m_sTouchEffectType, cTxt, 10);
				strcat(pData, cTxt);
				strcat(pData, " ");
				itoa(m_pClientList[iClientH]->m_pItemInBankList[i]->m_sTouchEffectValue1, cTxt, 10);
				strcat(pData, cTxt);
				strcat(pData, " ");
				itoa(m_pClientList[iClientH]->m_pItemInBankList[i]->m_sTouchEffectValue2, cTxt, 10);
				strcat(pData, cTxt);
				strcat(pData, " ");
				itoa(m_pClientList[iClientH]->m_pItemInBankList[i]->m_sTouchEffectValue3, cTxt, 10);
				strcat(pData, cTxt);
				strcat(pData, " ");
				itoa(m_pClientList[iClientH]->m_pItemInBankList[i]->m_cItemColor, cTxt, 10);
				strcat(pData, cTxt);
				strcat(pData, " ");
				itoa(m_pClientList[iClientH]->m_pItemInBankList[i]->m_sItemSpecEffectValue1, cTxt, 10);
				strcat(pData, cTxt);
				strcat(pData, " ");
				itoa(m_pClientList[iClientH]->m_pItemInBankList[i]->m_sItemSpecEffectValue2, cTxt, 10);
				strcat(pData, cTxt);
				strcat(pData, " ");
				itoa(m_pClientList[iClientH]->m_pItemInBankList[i]->m_sItemSpecEffectValue3, cTxt, 10);
				strcat(pData, cTxt);
				strcat(pData, " ");
				itoa(m_pClientList[iClientH]->m_pItemInBankList[i]->m_wCurLifeSpan, cTxt, 10);
				strcat(pData, cTxt);
				strcat(pData, " ");
				itoa(m_pClientList[iClientH]->m_pItemInBankList[i]->m_dwAttribute, cTxt, 10);
				strcat(pData, cTxt);
				strcat(pData, " ");
				//----------------------------------------------------------------------------
				itoa(m_pClientList[iClientH]->m_pItemInBankList[i]->m_sNewEffect1, cTxt, 10);
				strcat(pData, cTxt);
				strcat(pData, " ");
				//--------------------
				itoa(m_pClientList[iClientH]->m_pItemInBankList[i]->m_sNewEffect2, cTxt, 10);
				strcat(pData, cTxt);
				strcat(pData, " ");
				//--------------------
				itoa(m_pClientList[iClientH]->m_pItemInBankList[i]->m_sNewEffect3, cTxt, 10);
				strcat(pData, cTxt);
				strcat(pData, " ");
				//--------------------
				itoa(m_pClientList[iClientH]->m_pItemInBankList[i]->m_sNewEffect4, cTxt, 10);
				strcat(pData, cTxt);
				strcat(pData, "\n");
			}
		}
		strcat(pData, "\n\n");

		strcat(pData, "[MAGIC-SKILL-MASTERY]\n\n");

		strcat(pData, "//------------------012345678901234567890123456789012345678901234567890");
		strcat(pData, "\n");

		strcat(pData, "magic-mastery     = ");
		for (i = 0; i < DEF_MAXMAGICTYPE; i++) {
			wsprintf(cTxt, "%d", m_pClientList[iClientH]->m_cMagicMastery[i]);
			strcat(pData, cTxt);
		}
		strcat(pData, "\n");

		strcat(pData, "skill-mastery     = ");

		for (i = 0; i < DEF_MAXSKILLTYPE; i++) {
			ZeroMemory(cTxt, sizeof(cTxt));
			wsprintf(cTxt, "%d ", m_pClientList[iClientH]->m_cSkillMastery[i]);

			strcat(pData, cTxt); 
		}
		strcat(pData, "\n");

		strcat(pData, "skill-SSN     = ");
		for (i = 0; i < DEF_MAXSKILLTYPE; i++) {
			ZeroMemory(cTxt, sizeof(cTxt));
			wsprintf(cTxt, "%d ", m_pClientList[iClientH]->m_iSkillSSN[i]);

			strcat(pData, cTxt); 
		}
		strcat(pData, "\n");

		strcat(pData, "[ITEM-EQUIP-STATUS]\n\n");
		strcat(pData, "item-equip-status = ");

		ZeroMemory(cTxt, sizeof(cTxt));
		strcpy(cTxt, "00000000000000000000000000000000000000000000000000");

		for (i = 0; i < DEF_MAXITEMS; i++) {
			if (m_pClientList[iClientH]->m_pItemList[i] != NULL) {
				if ((m_pClientList[iClientH]->m_bIsItemEquipped[i] == true) &&
					(m_pClientList[iClientH]->m_pItemList[i]->m_cItemType == DEF_ITEMTYPE_EQUIP)) {
					cTxt[i] = '1';
				}
			}
		}
		strcat(pData, cTxt);
		strcat(pData, "\n");

		strcat(pData, "item-position-x = ");
		for (i = 0; i < DEF_MAXITEMS; i++) {
			ZeroMemory(cTxt, sizeof(cTxt));
			wsprintf(cTxt, "%d ", m_pClientList[iClientH]->m_ItemPosList[i].x);
			strcat(pData, cTxt);
		}
		strcat(pData, "\n");

		strcat(pData, "item-position-y = ");
		for (i = 0; i < DEF_MAXITEMS; i++) {
			ZeroMemory(cTxt, sizeof(cTxt));
			wsprintf(cTxt, "%d ", m_pClientList[iClientH]->m_ItemPosList[i].y);
			strcat(pData, cTxt);
		}
		strcat(pData, "\n\n");

		strcat(pData, "[EOF]");
		strcat(pData, "\n\n\n\n");

		return strlen(pData);
	}
}

/*********************************************************************************************************************
**  bool CGame::_bGetIsStringIsNumber(char * pStr)																	**
**  DESCRIPTION			:: checks if valid ASCII value																**
**  LAST_UPDATED		:: March 17, 2005; 2:24 PM; Hypnotoad														**
**	RETURN_VALUE		:: bool																						**
**  NOTES				::	n/a																						**
**	MODIFICATION		::	n/a																						**
**********************************************************************************************************************/
bool CGame::_bGetIsStringIsNumber(char * pStr)
{
 int i;
	for (i = 0; i < (int)strlen(pStr); i++)
	if ((pStr[i] != '-') && ((pStr[i] < (char)'0') || (pStr[i] > (char)'9'))) return false;
	
	return true;
}

/*********************************************************************************************************************
**  void CGame::ChatMsgHandler(int iClientH, char * pData, UINT32 dwMsgSize)											**
**  DESCRIPTION			:: manages chatting																			**
**  LAST_UPDATED		:: March 18, 2005; 7:28 PM; Hypnotoad														**
**	RETURN_VALUE		:: void																						**
**  NOTES				::	- new Crop DeleteNpc after specific amount of time										**
**							- disabled /setobservermode																**
**							- "/gns " changed to 5 to work correctly !												**
**	MODIFICATION		::	- all chat is logged if enabled 														**
**							- /enableadmincreateitem is disabled													**
**							- REMOVED" /blueball, /redball, /yellowball, /weather, /time, /checkstatus, /summonguild**
**							/summondeath, /kill, /revive, /unsummondemon, /setserk, /setfreeze, /setstatus, /storm	**
**							- ADDED: /version, /clearnpc, /clearmap, /who, /checkrep, /send, /summonall				** 			
**********************************************************************************************************************/
void CGame::ChatMsgHandler(int iClientH, char * pData, UINT32 dwMsgSize)
{
 int i, iRet;
 UINT16 * wp;
 int  * ip;
 char * cp, * cp2;
 char   cBuffer[256], cTemp[256], cSendMode = 0;

	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
	if (dwMsgSize > 113) return;
	if (m_pClientList[iClientH]->m_iTimeLeft_ShutUp > 0) return;
	if (memcmp((pData + 10), m_pClientList[iClientH]->m_cCharName, strlen(m_pClientList[iClientH]->m_cCharName)) != 0) return;
	if ((m_pClientList[iClientH]->m_bIsObserverMode == true) && (m_pClientList[iClientH]->m_iAdminUserLevel == 0)) return;
 
	if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex] != 0) {
		int iStX = m_pClientList[iClientH]->m_sX / 20;
		int iStY = m_pClientList[iClientH]->m_sY / 20;
		m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_stTempSectorInfo[iStX][iStY].iPlayerActivity++;

		switch (m_pClientList[iClientH]->m_cSide) {
		case 0: m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_stTempSectorInfo[iStX][iStY].iNeutralActivity++; break;
		case 1: m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_stTempSectorInfo[iStX][iStY].iAresdenActivity++; break;
		case 2: m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_stTempSectorInfo[iStX][iStY].iElvineActivity++;  break;
		}
	}

	cp = (char *)(pData + 21);
	
	// Modification - chat logging
	switch (m_bLogChatOption) {
		// Chat Logs of only players
		case 1:
			if (m_pClientList[iClientH]->m_iAdminUserLevel == 0){
				ZeroMemory(cTemp,sizeof(cTemp));
				pData[dwMsgSize-1] = 0;
				wsprintf(cTemp,"Loc(%s) IP(%s) PC(%s):\"%s\"",m_pClientList[iClientH]->m_cMapName,m_pClientList[iClientH]->m_cIPaddress,m_pClientList[iClientH]->m_cCharName,cp);
				bSendMsgToLS(MSGID_GAMEMASTERLOG,iClientH, false,cTemp);
			}
			break;
		// Chat Logs of only GM
		case 2:
			if (m_pClientList[iClientH]->m_iAdminUserLevel > 0){
				ZeroMemory(cTemp,sizeof(cTemp));
				pData[dwMsgSize-1] = 0;
				wsprintf(cTemp,"Loc(%s) IP(%s) GM(%s):\"%s\"",m_pClientList[iClientH]->m_cMapName,m_pClientList[iClientH]->m_cIPaddress,m_pClientList[iClientH]->m_cCharName,cp);
				bSendMsgToLS(MSGID_GAMEMASTERLOG,iClientH, false,cTemp);
			}
			break;
		// Chat logs of all
		case 3:
			if (m_pClientList[iClientH]->m_iAdminUserLevel > 0){
				ZeroMemory(cTemp,sizeof(cTemp));
				pData[dwMsgSize-1] = 0;
				wsprintf(cTemp,"Loc(%s) IP(%s) GM(%s):\"%s\"",m_pClientList[iClientH]->m_cMapName,m_pClientList[iClientH]->m_cIPaddress,m_pClientList[iClientH]->m_cCharName,cp);
				bSendMsgToLS(MSGID_GAMEMASTERLOG,iClientH, false,cTemp);
			}
			else{
				ZeroMemory(cTemp,sizeof(cTemp));
				pData[dwMsgSize-1] = 0;
				wsprintf(cTemp,"Loc(%s) IP(%s) PC(%s):\"%s\"",m_pClientList[iClientH]->m_cMapName,m_pClientList[iClientH]->m_cIPaddress,m_pClientList[iClientH]->m_cCharName,cp);
				bSendMsgToLS(MSGID_GAMEMASTERLOG,iClientH, false,cTemp);
			}	
			break;
		// Chat logs of none
		case 4:		
			break;
	}

	switch (*cp) {
	case '@':
		*cp = 32;
		if ((strlen(cp) < 90) && (m_pClientList[iClientH]->m_iGuildRank == 0)) {
			ZeroMemory(cTemp, sizeof(cTemp));
			cp2 = (char *)cTemp;
			*cp2 = GSM_CHATMSG;
			cp2++;
			*cp2 = 1;
			cp2++;
			ip = (int *)cp2;
			*ip = m_pClientList[iClientH]->m_iGuildGUID;
			cp2 += 4;
			memcpy(cp2, m_pClientList[iClientH]->m_cCharName, 10);
			cp2 += 10;
			wp  = (UINT16 *)cp2;
			*wp = (UINT16)strlen(cp);
			cp2 += 2;
			strcpy(cp2, cp);
			cp2 += strlen(cp);
			bStockMsgToGateServer(cTemp, strlen(cp) + 18);
		}
				
		if ( (m_pClientList[iClientH]->m_iTimeLeft_ShutUp == 0) && (m_pClientList[iClientH]->m_iLevel > 1) && 
			 (m_pClientList[iClientH]->m_iSP >= 3) ) {
			if (m_pClientList[iClientH]->m_iTimeLeft_FirmStaminar == 0) {
				m_pClientList[iClientH]->m_iSP -= 3;
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_SP, 0, 0, 0, 0);
			}
			cSendMode = 1;
		}
		else 
			if (m_pClientList[iClientH]->m_iTimeLeft_ShutUp > 0) cSendMode = 0;
		break;

	case '$':
		*cp = 32;
		if ((m_pClientList[iClientH]->m_iTimeLeft_ShutUp == 0) && (m_pClientList[iClientH]->m_iSP >= 3)){
			if (m_pClientList[iClientH]->m_iTimeLeft_FirmStaminar == 0) {
				m_pClientList[iClientH]->m_iSP -= 3;
				SendNotifyMsg(0,iClientH,DEF_NOTIFY_SP,0,0,0,0);
			}
			cSendMode = 4;
		}
		else
		
			if (m_pClientList[iClientH]->m_iTimeLeft_ShutUp > 0) cSendMode = 0;
			
		break;

	case '^':
		*cp = 32;
		if ((strlen(cp) < 90) && (m_pClientList[iClientH]->m_iAdminUserLevel > 0)) {
			ZeroMemory(cTemp, sizeof(cTemp));
			cp2 = (char *)cTemp;
			*cp2 = GSM_CHATMSG;
			cp2++;
			*cp2 = 10;
			cp2++;
			ip = (int *)cp2;
			*ip = 0;
			cp2 += 4;
			memcpy(cp2, m_pClientList[iClientH]->m_cCharName, 10);
			cp2 += 10;
			wp  = (UINT16 *)cp2;
			*wp = (UINT16)strlen(cp);
			cp2 += 2;
			strcpy(cp2, cp);
			cp2 += strlen(cp);
			bStockMsgToGateServer(cTemp, strlen(cp) + 18);
		}
		if ((strlen(cp) < 90) && (m_pClientList[iClientH]->m_iGuildRank != -1)) {
			ZeroMemory(cTemp, sizeof(cTemp));
			cp2 = (char *)cTemp;
			*cp2 = GSM_CHATMSG;
			cp2++;
			*cp2 = 1;
			cp2++;
			ip = (int *)cp2;
			*ip = m_pClientList[iClientH]->m_iGuildGUID;
			cp2 += 4;
			memcpy(cp2, m_pClientList[iClientH]->m_cCharName, 10);
			cp2 += 10;
			wp  = (UINT16 *)cp2;
			*wp = (UINT16)strlen(cp);
			cp2 += 2;
			strcpy(cp2, cp);
			cp2 += strlen(cp);
			bStockMsgToGateServer(cTemp, strlen(cp) + 18);
		}

		if ( (m_pClientList[iClientH]->m_iTimeLeft_ShutUp == 0) && (m_pClientList[iClientH]->m_iLevel > 10) && 
			(m_pClientList[iClientH]->m_iSP > 5) && m_pClientList[iClientH]->m_iGuildRank != -1) {
				if (m_pClientList[iClientH]->m_iTimeLeft_FirmStaminar == 0) {
					m_pClientList[iClientH]->m_iSP -= 3;
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_SP, 0, 0, 0, 0);
				}
				cSendMode = 1;
			}
		else {
			if (m_pClientList[iClientH]->m_iTimeLeft_ShutUp > 0) cSendMode = 0;
			if (m_pClientList[iClientH]->m_iHP <= 0) cSendMode = 0;
		}
		if (m_pClientList[iClientH]->m_iAdminUserLevel > 0) cSendMode = 10;
		break;

	case '!':
		*cp = 32;
		if ((strlen(cp) < 90) && (m_pClientList[iClientH]->m_iAdminUserLevel > 0)) {
			ZeroMemory(cTemp, sizeof(cTemp));
			cp2 = (char *)cTemp;
			*cp2 = GSM_CHATMSG;
			cp2++;
			*cp2 = 10;
			cp2++;
			ip = (int *)cp2;
			*ip = 0;
			cp2 += 4;
			memcpy(cp2, m_pClientList[iClientH]->m_cCharName, 10);
			cp2 += 10;
			wp  = (UINT16 *)cp2;
			*wp = (UINT16)strlen(cp);
			cp2 += 2;
			strcpy(cp2, cp);
			cp2 += strlen(cp);
			bStockMsgToGateServer(cTemp, strlen(cp) + 18);
		}
		if ( (m_pClientList[iClientH]->m_iTimeLeft_ShutUp == 0) && (m_pClientList[iClientH]->m_iLevel > 10) && 
			 (m_pClientList[iClientH]->m_iSP >= 5) ) {
			if (m_pClientList[iClientH]->m_iTimeLeft_FirmStaminar == 0) {
				m_pClientList[iClientH]->m_iSP -= 5;
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_SP, 0, 0, 0, 0);
			}
			cSendMode = 2;
		}
		else {
			if (m_pClientList[iClientH]->m_iTimeLeft_ShutUp > 0) cSendMode = 0;
			if (m_pClientList[iClientH]->m_iHP <= 0) cSendMode = 0;
		}
		if (m_pClientList[iClientH]->m_iAdminUserLevel > 0) cSendMode = 14;
		break;

	case '~':
		*cp = 32;
		if ( (m_pClientList[iClientH]->m_iTimeLeft_ShutUp == 0) && (m_pClientList[iClientH]->m_iLevel > 1) && 
			 (m_pClientList[iClientH]->m_iSP >= 3) ) {
			if (m_pClientList[iClientH]->m_iTimeLeft_FirmStaminar == 0) {
				m_pClientList[iClientH]->m_iSP -= 3;
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_SP, 0, 0, 0, 0);
			}
			cSendMode = 3;
		}
		else {
			if (m_pClientList[iClientH]->m_iTimeLeft_ShutUp > 0) cSendMode = 0;
			if (m_pClientList[iClientH]->m_iHP <= 0) cSendMode = 0;
		}
		break;

	//Magn0S:: Market Chat
	case '%':
		*cp = 32;
		if ((m_pClientList[iClientH]->m_iTimeLeft_ShutUp == 0) && (m_pClientList[iClientH]->m_iLevel > 1) &&
			(m_pClientList[iClientH]->m_iSP >= 3)) {
			if (m_pClientList[iClientH]->m_iTimeLeft_FirmStaminar == 0) {
				m_pClientList[iClientH]->m_iSP -= 3;
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_SP, 0, 0, 0, 0);
			}
			cSendMode = 15;
		}
		else {
			if (m_pClientList[iClientH]->m_iTimeLeft_ShutUp > 0) cSendMode = 0;
			if (m_pClientList[iClientH]->m_iHP <= 0) cSendMode = 0;
		}
		break;

	case '/':
		ZeroMemory(cBuffer, sizeof(cBuffer));
		memcpy(cBuffer, cp, dwMsgSize - 21);
		cp = (char *)(cBuffer);

		if (memcmp(cp, "/fi ", 4) == 0) {
			CheckAndNotifyPlayerConnection(iClientH, cp, dwMsgSize - 21);
			
		}

		else if (memcmp(cp, "/to ", 4) == 0) {
			ToggleWhisperPlayer(iClientH, cp, dwMsgSize - 21);
			
		}

		else if (memcmp(cp, "/setpf ", 7) == 0) {
			SetPlayerProfile(iClientH, cp, dwMsgSize - 21);
			
		}

		else if (memcmp(cp, "/pf ", 4) == 0) {
			GetPlayerProfile(iClientH, cp, dwMsgSize - 21);
			
		}

		else if (memcmp(cp, "/shutup ", 8) == 0) {
			ShutUpPlayer(iClientH, cp, dwMsgSize - 21);
			
		}

		else if (memcmp(cp, "/rep+ ", 6) == 0) {
			SetPlayerReputation(iClientH, cp, 1, dwMsgSize - 21);
			
		}

		else if (memcmp(cp, "/rep- ", 6) == 0) {
			SetPlayerReputation(iClientH, cp, 0, dwMsgSize - 21);
			
		}

		else if (memcmp(cp, "/hold", 5) == 0) {
			SetSummonMobAction(iClientH, 1, dwMsgSize - 21);
			
		}

		else if (memcmp(cp, "/tgt ", 5) == 0) {
			SetSummonMobAction(iClientH, 2, dwMsgSize - 21, cp);
			
		}

		else if (memcmp(cp, "/free", 5) == 0) {
			SetSummonMobAction(iClientH, 0, dwMsgSize - 21);
			
		}

		else if (memcmp(cp, "/closeconn ", 11) == 0) {
			AdminOrder_CloseConn(iClientH, cp, dwMsgSize - 21);
			
		}

		else if (memcmp(cp, "/ban ", 5) == 0) {
			UserCommand_BanGuildsman(iClientH, cp, dwMsgSize - 21);
			
		}

		else if (memcmp(cp, "/attack ", 8) == 0) {
			AdminOrder_CallGuard(iClientH, cp, dwMsgSize - 21);
			
		}

		else if (memcmp(cp, "/createfish ", 12) == 0) {
			
		}

		else if (memcmp(cp, "/teleport ", 10) == 0 || memcmp(cp, "/tp ", 4) == 0) {
			AdminOrder_Teleport(iClientH, cp, dwMsgSize - 21);
			
		}

		else if (memcmp(cp, "/goto ", 6) == 0) {
			AdminOrder_GoTo(iClientH, cp, dwMsgSize - 21);
			
		}

		else if (memcmp(cp, "/summondemon ", 13) == 0) {
			AdminOrder_SummonDemon(iClientH);
			
		}
		
		else if (memcmp(cp, "/summonplayer ", 14) == 0) {
			AdminOrder_SummonPlayer(iClientH, cp, dwMsgSize - 21);
			
		}

		else if (memcmp(cp, "/unsummonall ", 13) == 0) {
			AdminOrder_UnsummonAll(iClientH);
						
		}

		else if (memcmp(cp, "/unsummonboss", 13) == 0) {
			AdminOrder_UnsummonBoss(iClientH);
			
		}

		else if (memcmp(cp, "/checkip ", 9) == 0) {
			AdminOrder_CheckIP(iClientH, cp, dwMsgSize - 21);
			
		}

		else if (memcmp(cp, "/polymorph ", 11) == 0) {
			
		}

		else if (memcmp(cp, "/setinvi ", 9) == 0) {
			AdminOrder_SetInvi(iClientH, cp, dwMsgSize - 21);
			
		}

		else if (memcmp(cp, "/gns ", 5) == 0) { // changed to 5...
			AdminOrder_GetNpcStatus(iClientH, cp, dwMsgSize - 21);
			
		}

		else if (memcmp(cp, "/setattackmode ", 15) == 0) {
			AdminOrder_SetAttackMode(iClientH, cp, dwMsgSize - 21);
				
		}

		else if (memcmp(cp, "/setforcerecalltime ", 20) == 0) {
			AdminOrder_SetForceRecallTime(iClientH, cp, dwMsgSize - 21);
			
		}

		else if (memcmp(cp, "/summon ", 8) == 0) {
			AdminOrder_Summon(iClientH, cp, dwMsgSize - 21);
			
		}

		else if (memcmp(cp, "/disconnectall", 14) == 0) {
			AdminOrder_DisconnectAll(iClientH, cp, dwMsgSize - 21);
			
		}

		else if (memcmp(cp, "/createitem ", 12) == 0) {
			AdminOrder_CreateItem(iClientH, cp, dwMsgSize - 21);
			
		}

		//Magn0S:: Added fragile itens
		else if (memcmp(cp, "/fragile ", 8) == 0) {
			AdminOrder_CreateFragileItem(iClientH, cp, dwMsgSize - 21);
		}

		else if (memcmp(cp, "/check", 6) == 0) {
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_SERVERTIME, 0, 0, 0, 0);
		}
		

		else if (memcmp(cp, "/enableadmincommand ", 20) == 0) {
			AdminOrder_EnableAdminCommand(iClientH, cp, dwMsgSize - 21);
		}

		else if (memcmp(cp, "/monstercount", 13) == 0) {
			AdminOrder_MonsterCount(iClientH,cp,dwMsgSize - 21);
		}


		else if (memcmp(cp, "/begincrusade", 13) == 0) {
			if (m_pClientList[iClientH]->m_iAdminUserLevel > 2) {
				GlobalStartCrusadeMode();
				wsprintf(cTemp, "(%s) GM Order(%s): begincrusadetotalwar", m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName);
				bSendMsgToLS(MSGID_GAMEMASTERLOG, iClientH, false, cTemp);
			}
			
		}

		
		else if (memcmp(cp, "/endcrusade", 11) == 0) {
			if (m_pClientList[iClientH]->m_iAdminUserLevel > 2) {
				ManualEndCrusadeMode(0);
				wsprintf(cTemp, "(%s) GM Order(%s): endcrusadetotalwar", m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName);
				bSendMsgToLS(MSGID_GAMEMASTERLOG, iClientH, false, cTemp);
			}
			
		}

		else if (strcmp(cp, "/happy") == 0)
		{
			if (m_pClientList[iClientH]->m_iAdminUserLevel < 3) return;
			ManageHappyHour();
		}

		else if (strcmp(cp, "/fury") == 0)
		{
			if (m_pClientList[iClientH]->m_iAdminUserLevel < 3) return;
			ManageFuryHour();
		}

		else if (strcmp(cp, "/team") == 0)
		{
			if (m_pClientList[iClientH]->m_iAdminUserLevel < 3) return;
			if (c_team->bteam) c_team->bteam = false; 
			else c_team->bteam = true;

			if (c_team->bteam) {
				dwEventFinishTime = timeGetTime() + m_sTeamArenaFinish * 60 * 1000;
				c_team->EnableEvent();
			}
			else
			{
				dwEventFinishTime = 0;
				c_team->DisableEvent();
			}
			
			NotifyEvents();
		}

		else if (memcmp(cp, "/createparty", 12) == 0) {
			if (m_pClientList[iClientH]->IsInMap("team")) return;
			RequestCreatePartyHandler(iClientH);
			
		}

		else if (memcmp(cp, "/joinparty ", 11) == 0) {
			RequestJoinPartyHandler(iClientH,cp,dwMsgSize - 21);
			
		}

		else if (memcmp(cp, "/dismissparty", 13) == 0) {
			RequestDismissPartyHandler(iClientH);
			
		}

		else if (memcmp(cp, "/getpartyinfo", 13) == 0) {
			GetPartyInfoHandler(iClientH);
			
		}

		else if (memcmp(cp, "/deleteparty", 12) == 0) {
			RequestDeletePartyHandler(iClientH);
			
		}

		else if ((memcmp(cp, "/shutdown", 9) == 0) && (m_pClientList[iClientH]->m_iAdminUserLevel >= m_iAdminLevelShutdown)) {
			m_cShutDownCode      = 2;
			m_bOnExitProcess     = true;
			m_dwExitProcessTime  = timeGetTime();
			PutLogList("(!) GAME SERVER SHUTDOWN PROCESS BEGIN(by Admin-Command)!!!");
			bSendMsgToLS(MSGID_GAMESERVERSHUTDOWNED, 0);
			
		}

		else if (memcmp(cp, "/beginapocalypse", 16) == 0) // SNOOPY reactivated these function
		{	if (   (m_pClientList[iClientH]->m_iAdminUserLevel >= 3)
				&& (m_pClientList[iClientH]->m_bIsAdminCommandEnabled == true)) 
			{	wsprintf(cTemp, "GM Order(%-10s): /beginapocalypse", m_pClientList[iClientH]->m_cCharName);
				bSendMsgToLS(MSGID_GAMEMASTERLOG, iClientH, false, cTemp);
				GlobalStartApocalypseMode(iClientH, 0);
			}
		}
		else if (memcmp(cp, "/endapocalypse", 14) == 0)
		{	if (   (m_pClientList[iClientH]->m_iAdminUserLevel >= 3) 	
				&& (m_pClientList[iClientH]->m_bIsAdminCommandEnabled == true)) 
			{	wsprintf(cTemp, "GM Order(%-10s): /endapocalypse", m_pClientList[iClientH]->m_cCharName);
				bSendMsgToLS(MSGID_GAMEMASTERLOG, iClientH, false, cTemp);
				GlobalEndApocalypseMode(iClientH);
			}
		}	

		else if (memcmp(cp, "/ctf", 4) == 0) 
		{
			if ((m_pClientList[iClientH]->m_iAdminUserLevel >= 3)
				&& (m_pClientList[iClientH]->m_bIsAdminCommandEnabled == true))
			{
				AdminOrder_SetEvent();
			}
		}

		else if (memcmp(cp, "/criticals", 10) == 0) {
			PlayerOrder_Criticals(iClientH);
		}

		else if (memcmp(cp, "/beginheldenian ", 16) == 0)	//m_iAdminLevelSpecialEvents
		{	
			if ((m_pClientList[iClientH]->m_iAdminUserLevel >= 3)
				&& (m_pClientList[iClientH]->m_bIsAdminCommandEnabled == true))
			{
				wsprintf(cTemp, "GM Order(%-10s): /beginheldenian", m_pClientList[iClientH]->m_cCharName);
				bSendMsgToLS(MSGID_GAMEMASTERLOG, iClientH, false, cTemp);
				ManualStartHeldenianMode(iClientH, cp, dwMsgSize - 21);
			}
		}		
		else if (memcmp(cp, "/endheldenian ", 14) == 0)		//m_iAdminLevelSpecialEvents
		{	
			if ((m_pClientList[iClientH]->m_iAdminUserLevel >= 3)
				&& (m_pClientList[iClientH]->m_bIsAdminCommandEnabled == true))
			{
				wsprintf(cTemp, "GM Order(%-10s): /endheldenian", m_pClientList[iClientH]->m_cCharName);
				bSendMsgToLS(MSGID_GAMEMASTERLOG, iClientH, false, cTemp);
				ManualEndHeldenianMode(iClientH, cp, dwMsgSize - 21);
			}
		}

		// Modification 
		// New 19/05/2004
		else if (memcmp(cp, "/clearnpc", 9) == 0)
		{
			AdminOrder_ClearNpc(iClientH);
			
		}
		else if (memcmp(cp, "/clearmap", 9) == 0) 
		{
			AdminOrder_CleanMap(iClientH, cp, dwMsgSize);
		}

		else if (memcmp(cp, "/setobservermode ", 17) == 0) {
			if (m_pClientList[iClientH]->m_iAdminUserLevel >= m_iAdminLevelObserver)
				AdminOrder_SetObserverMode(iClientH);
		}

		// kazin
		else if (memcmp(cp, "/candyboost", 11) == 0) {
			if (m_pClientList[iClientH]->m_iAdminUserLevel >= 3)
			{
				dwEventFinishTime = timeGetTime() + m_sCandyFinish * 60 * 1000;
				_candy_boost = !_candy_boost;
				NotifyEvents();
			}
		}
		else if (memcmp(cp, "/revelation", 11) == 0) {
			if (m_pClientList[iClientH]->m_iAdminUserLevel >= 3)
			{
				dwEventFinishTime = timeGetTime() + m_sRevelationFinish * 60 * 1000;
				_revelation = !_revelation;
				NotifyEvents();
			}
		}
		else if (memcmp(cp, "/cityteleport", 13) == 0) {
			if (m_pClientList[iClientH]->m_iAdminUserLevel >= 3)
			{
				CityTeleport();
			}
		}
		else if (memcmp(cp, "/dropinhib", 10) == 0) {
			if (m_pClientList[iClientH]->m_iAdminUserLevel >= 3)
			{
				dwEventFinishTime = timeGetTime() + m_sDropFinish * 60 * 1000;
				_drop_inhib = !_drop_inhib;
				NotifyEvents();
			}
		}

		else if (strcmp(cp, "/shinning") == 0)
		{
			if (m_pClientList[iClientH]->m_iAdminUserLevel < 3) return;
			
			ManageShinning();
		}

		else if (strcmp(cp, "/clearbag") == 0)
		{
			if (m_pClientList[iClientH]->m_iAdminUserLevel < 1) return;
			for (i = 0; i < DEF_MAXITEMS; i++) 
			{
				if (m_pClientList[iClientH]->m_bIsItemEquipped[i]) continue;
				ItemDepleteHandler(iClientH, i, false, true);
			}
		}
		else if (memcmp(cp, "/skills", 7) == 0)
		{
			AutoSkill(iClientH);
		}
		else if (memcmp(cp, "/who", 4) == 0) 
		{
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_TOTALUSERS, 0, 0, 0, 0);
			//Magn0S:: Put as same as /online
			char cMsg[120];
			int iAres = 0, iElv = 0, iTrav = 0, iGM = 0;
			for (int i = 1; i < DEF_MAXCLIENTS; i++) {
				if (m_pClientList[i] != 0) {
					if (memcmp(m_pClientList[i]->m_cLocation, "are", 3) == 0 &&
						m_pClientList[i]->m_iAdminUserLevel == 0) iAres++;
					else if (memcmp(m_pClientList[i]->m_cLocation, "elv", 3) == 0 &&
						m_pClientList[i]->m_iAdminUserLevel == 0) iElv++;
					else if (memcmp(m_pClientList[i]->m_cLocation, "NONE", 4) == 0 &&
						m_pClientList[i]->m_iAdminUserLevel == 0) iTrav++;
					else if (m_pClientList[i]->m_iAdminUserLevel > 0) iGM++;
				}
			}
			ZeroMemory(cMsg, sizeof(cMsg));
			wsprintf(cMsg, "Ares: %d | Elv: %d | Trav: %d | GM: %d", iAres, iElv, iTrav, iGM);
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, cMsg);
			
		}
		
		//Magn0S:: Added new commands
		else if (memcmp(cp, "/admins", 7) == 0) {
			PlayerCommandCheckAdmins(iClientH);
		}
		else if (memcmp(cp, "/setek ", 7) == 0) {
			AdminOrder_SetEk(iClientH, cp, dwMsgSize - 21);
		}
		else if (memcmp(cp, "/setcoin ", 9) == 0) {
			AdminOrder_SetCoin(iClientH, cp, dwMsgSize - 21);
		}
		else if (memcmp(cp, "/setmp ", 7) == 0) {
			AdminOrder_SetMAJ(iClientH, cp, dwMsgSize - 21);
		}
		else if (memcmp(cp, "/setrep ", 8) == 0) {
			AdminOrder_SetRep(iClientH, cp, dwMsgSize - 21);
		}
		else if (memcmp(cp, "/setcontrib ", 12) == 0) {
			AdminOrder_SetContrib(iClientH, cp, dwMsgSize - 21);
		}
		else if (memcmp(cp, "/gmlevel ", 9) == 0) {
			AdminOrder_SetAdminLvl(iClientH, cp, dwMsgSize - 21);
		}
		else if (memcmp(cp, "/showdmg", 8) == 0) {
			if (m_pClientList[iClientH]->bShowDmg == true) {
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, "Show Damage is now[OFF]");
				m_pClientList[iClientH]->bShowDmg = false;
			}
			else {
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, "Show Damage is now[ON]");
				m_pClientList[iClientH]->bShowDmg = true;
			}
		}
		else if (memcmp(cp, "/updatefiles", 12) == 0) {
			if (m_pClientList[iClientH]->m_iAdminUserLevel >= 3) {
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, "Server are updating .cfg files.");

				bReadSettingsConfigFile("GameConfigs\\Settings.cfg");
				bReadAdminListConfigFile("GameConfigs\\AdminList.cfg");
				bReadBannedListConfigFile("GameConfigs\\BannedList.cfg");
				bReadAdminSetConfigFile("GameConfigs\\AdminSettings.cfg");
			}
		}
		
		//End----------------------------------------------------------------------------

		else if (memcmp(cp, "/deathmatch", 11) == 0) {
            if (m_pClientList[iClientH]->m_iAdminUserLevel < 3) return;


		}
		
		else if (memcmp(cp, "/addcaptain ", 12) == 0) {
            PlayerCommandAddRank(iClientH, cp, dwMsgSize - 21, 3);
            
        }
		else if (memcmp(cp, "/addsummoner ", 13) == 0) {
			PlayerCommandAddRank(iClientH, cp, dwMsgSize - 21, 2);

		}
		else if (memcmp(cp, "/addrecluiter ", 14) == 0) {
			PlayerCommandAddRank(iClientH, cp, dwMsgSize - 21, 1);

		}
		else if (memcmp(cp, "/delrank ", 9) == 0) {
            PlayerOrder_DeleteRank(iClientH, cp, dwMsgSize - 21);
            
        }

		else if (memcmp(cp, "/bum ", 5) == 0) { // MORLA - 2.12 Cagar pc del player
			AdminOrder_BanPj(iClientH, cp, dwMsgSize - 21);
			
		}

		else if (memcmp(cp, "/banip ", 7) == 0) {
			AdminOrder_BanIP(iClientH, cp, dwMsgSize - 21);
			
		}

		//Magn0S:: Add GM List
		else if (memcmp(cp, "/addgm ", 7) == 0) {
			AdminOrder_AddGM(iClientH, cp, dwMsgSize - 21);

		}

		else if (memcmp(cp, "/summonguild", 12) == 0) {
			AdminOrder_SummonGuild(iClientH);
			
		}

		else if (memcmp(cp, "/checkrep ", 10) == 0) {
			AdminOrder_CheckRep(iClientH,cp, dwMsgSize - 21);
			
		}		

		else if (memcmp(cp, "/send ", 5) == 0) {
			AdminOrder_Pushplayer(iClientH, cp, dwMsgSize -21);
			
		}

		else if (memcmp(cp, "/summonall ", 11) == 0) {
			AdminOrder_SummonAll(iClientH, cp, dwMsgSize - 21);
			
		}

		//LifeX Fix Revive 14/10/2019
		else if (memcmp(cp, "/revive ", 8) == 0) {
			AdminOrder_Revive(iClientH, cp, dwMsgSize - 21);

		}

		return;
	}
	pData[dwMsgSize-1] = 0;
	
	// SNOOPY: designed non corean confuse language		
	if ((m_pClientList[iClientH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_CONFUSE ] == 1)) {
		// Confuse Language
		cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 17);		
		while (*cp != 0) {
			if ((cp[0] != 0) && (cp[1] != 0)) 
			{	switch (iDice(1,12)) {
				case 1:	memcpy(cp, "ma", 2); break;
				case 2:	memcpy(cp, "fo", 2); break;
				case 3:	memcpy(cp, "ru", 2); break;
				case 4:	memcpy(cp, "be", 2); break;
				case 5:	memcpy(cp, "xy", 2); break;
				case 6:	memcpy(cp, "ce", 2); break;
				case 7:	memcpy(cp, "li", 2); break;
				case 8:	memcpy(cp, "re", 2); break;
				case 9:	memcpy(cp, "su", 2); break;
				case 10:memcpy(cp, "to", 2); break;
				case 11:memcpy(cp, "ju", 2); break;
				case 12:memcpy(cp, "de", 2); break;
				}
				if (cp[1] != ' ')
				switch (iDice(1,12)) {
				case 1:	
				case 2:	
				case 3:	
				case 4:	
				case 5:	
				case 6:	memcpy(cp, "a ", 2); break;
				case 7:	
				case 8:	memcpy(cp, "e ", 2); break;
				case 9:	memcpy(cp, "i ", 2); break;
				case 10:memcpy(cp, "o ", 2); break;
				case 11:memcpy(cp, "u ", 2); break;
				case 12:memcpy(cp, "y ", 2); break;
				}	
				if (cp[0] != ' ')
				switch (iDice(1,12)) {
				case 1:	
				case 2:	
				case 3:	
				case 4:	
				case 5:	
				case 6:	memcpy(cp, " c", 2); break;
				case 7:	
				case 8:	memcpy(cp, " s", 2); break;
				case 9:	memcpy(cp, " t", 2); break;
				case 10:memcpy(cp, " l", 2); break;
				case 11:memcpy(cp, " r", 2); break;
				case 12:memcpy(cp, " n", 2); break;
				}				
				cp += 2;
			}
			else cp++; 
		}
	}
		
	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 17);
	if ((cSendMode == 0) && (m_pClientList[iClientH]->m_iWhisperPlayerIndex != -1)) {
		cSendMode = 20;
 		if (*cp == '#') cSendMode = 0;
		if (m_pClientList[iClientH]->m_iTimeLeft_ShutUp > 0) cSendMode = 0;
	}
	wp = (UINT16 *)(pData + DEF_INDEX2_MSGTYPE);
	*wp = (UINT16)iClientH;
	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 16);
	*cp = cSendMode;

	if (cSendMode != 20) {
		for (i = 1; i < DEF_MAXCLIENTS; i++)
		if (m_pClientList[i] != 0) {
			switch (cSendMode) {
			case 0:
				if (m_pClientList[i]->m_bIsInitComplete == false) break;
				if ((m_pClientList[i]->m_cMapIndex == m_pClientList[iClientH]->m_cMapIndex) &&
					// centu - 800x600 
					(m_pClientList[i]->m_sX > m_pClientList[iClientH]->m_sX - 12) && // 10
					 (m_pClientList[i]->m_sX < m_pClientList[iClientH]->m_sX + 12) && // 10
					 (m_pClientList[i]->m_sY > m_pClientList[iClientH]->m_sY - 9) && // 7
					 (m_pClientList[i]->m_sY < m_pClientList[iClientH]->m_sY + 9)) { // 7
					iRet = m_pClientList[i]->m_pXSock->iSendMsg(pData, dwMsgSize);
				}
				break;
					
			case 1:
				if (m_pClientList[i]->m_bIsInitComplete == false) break;
				if ((memcmp(m_pClientList[i]->m_cGuildName, m_pClientList[iClientH]->m_cGuildName, 20) == 0) &&
					(memcmp(m_pClientList[i]->m_cGuildName, "NONE", 4) != 0)) {					
					iRet = m_pClientList[i]->m_pXSock->iSendMsg(pData, dwMsgSize);
				}
				break;
					
			case 2:
			case 10:
			case 14:
			case 15:
				iRet = m_pClientList[i]->m_pXSock->iSendMsg(pData, dwMsgSize);
				break;
					
			case 3:
				if (m_pClientList[i]->m_bIsInitComplete == false) break;
				if ( (m_pClientList[i]->m_cSide == m_pClientList[iClientH]->m_cSide) ) 
					iRet = m_pClientList[i]->m_pXSock->iSendMsg(pData, dwMsgSize);
				break;

			case 4:
				if (m_pClientList[i]->m_bIsInitComplete == false) break;
				if ((m_pClientList[i]->m_iPartyID != 0) && (m_pClientList[i]->m_iPartyID == m_pClientList[iClientH]->m_iPartyID) ) 
					iRet = m_pClientList[i]->m_pXSock->iSendMsg(pData, dwMsgSize);
				break;
			}
		}
	}
	else {
		iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(pData, dwMsgSize);	
		if (m_pClientList[iClientH]->m_iWhisperPlayerIndex == 10000) {
			ZeroMemory(cBuffer, sizeof(cBuffer));
			cp = (char *)cBuffer;
			*cp = GSM_WHISFERMSG;
			cp++;
			memcpy(cp, m_pClientList[iClientH]->m_cWhisperPlayerName, 10);
			cp += 10;

			wp = (UINT16 *)cp;
			*wp = (UINT16)dwMsgSize;
			cp += 2;
			memcpy(cp, pData, dwMsgSize);
			cp += dwMsgSize;
			bStockMsgToGateServer(cBuffer, dwMsgSize+13);
		}
		else{
			if (m_pClientList[ m_pClientList[iClientH]->m_iWhisperPlayerIndex ] != 0 && strcmp(m_pClientList[iClientH]->m_cWhisperPlayerName, m_pClientList[ m_pClientList[iClientH]->m_iWhisperPlayerIndex ]->m_cCharName) == 0){
				iRet = m_pClientList[m_pClientList[iClientH]->m_iWhisperPlayerIndex]->m_pXSock->iSendMsg(pData, dwMsgSize);	
				switch (m_bLogChatOption){
					case 1:
						if (m_pClientList[m_pClientList[iClientH]->m_iWhisperPlayerIndex]->m_iAdminUserLevel == 0){
							ZeroMemory(cTemp,sizeof(cTemp));
							wsprintf(cTemp,"GM Whisper   (%s):\"%s\"\tto Player(%s)",m_pClientList[iClientH]->m_cCharName, pData+21, m_pClientList[iClientH]->m_cWhisperPlayerName);
							bSendMsgToLS(MSGID_GAMEMASTERLOG,iClientH, false,cTemp);
						}
						break;
					case 2:
						if (m_pClientList[m_pClientList[iClientH]->m_iWhisperPlayerIndex]->m_iAdminUserLevel > 0){
							ZeroMemory(cTemp,sizeof(cTemp));
							wsprintf(cTemp,"GM Whisper   (%s):\"%s\"\tto GM(%s)",m_pClientList[iClientH]->m_cCharName, pData+21, m_pClientList[iClientH]->m_cWhisperPlayerName);
							bSendMsgToLS(MSGID_GAMEMASTERLOG,iClientH, false,cTemp);
						}
						break;
					case 3:
						if (m_pClientList[m_pClientList[iClientH]->m_iWhisperPlayerIndex]->m_iAdminUserLevel > 0){
							ZeroMemory(cTemp,sizeof(cTemp));
							wsprintf(cTemp,"GM Whisper   (%s):\"%s\"\tto GM(%s)",m_pClientList[iClientH]->m_cCharName, pData+21, m_pClientList[iClientH]->m_cWhisperPlayerName);
							bSendMsgToLS(MSGID_GAMEMASTERLOG,iClientH, false,cTemp);
						}
						else{
							ZeroMemory(cTemp,sizeof(cTemp));
							wsprintf(cTemp,"Player Whisper   (%s):\"%s\"\tto Player(%s)",m_pClientList[iClientH]->m_cCharName, pData+21, m_pClientList[iClientH]->m_cWhisperPlayerName);
							bSendMsgToLS(MSGID_GAMEMASTERLOG,iClientH, false,cTemp);
						}
						break;
					case 4:
						break;
				}
			}
		}
	}
}

/*********************************************************************************************************************
**  void CGame::ChatMsgHandlerGSM(int iMsgType, int iV1, char * pName, char * pData, UINT32 dwMsgSize)				**
**  DESCRIPTION			:: manages Npc variables																	**
**  LAST_UPDATED		:: March 18, 2005; 6:30 PM; Hypnotoad														**
**	RETURN_VALUE		:: void																						**
**  NOTES				::	- new Crop DeleteNpc after specific amount of time										**
**	MODIFICATION		::	n/a																						**
**********************************************************************************************************************/
void CGame::ChatMsgHandlerGSM(int iMsgType, int iV1, char * pName, char * pData, UINT32 dwMsgSize)
{
 int i, iRet;
 UINT32 * dwp;
 UINT16 * wp;
 short * sp;
 char * cp, cTemp[256], cSendMode = 0;

	ZeroMemory(cTemp, sizeof(cTemp));
	
	dwp = (UINT32 *)cTemp;
	*dwp = MSGID_COMMAND_CHATMSG;
	
	wp  = (UINT16 *)(cTemp + DEF_INDEX2_MSGTYPE);
	*wp = 0;

	cp  = (char *)(cTemp + DEF_INDEX2_MSGTYPE + 2);
	sp  = (short *)cp;
	*sp = 0;
	cp += 2;

	sp  = (short *)cp;
	*sp = 0;
	cp += 2;
	
	memcpy(cp, pName, 10);
	cp += 10;

	*cp = (char)iMsgType;
	cp++;

	memcpy(cp, pData, dwMsgSize);
	cp += dwMsgSize;

	switch (iMsgType) {
	case 1:
		for (i = 1; i < DEF_MAXCLIENTS; i++)
		if (m_pClientList[i] != 0) {
			if (m_pClientList[i]->m_bIsInitComplete == false) break;
			if ( (m_pClientList[i]->m_iGuildGUID == iV1) && (m_pClientList[i]->m_iGuildGUID != 0)) {
				iRet = m_pClientList[i]->m_pXSock->iSendMsg(cTemp, dwMsgSize +22);
			}
		}
		break;

	case 2:
	case 10:
		for (i = 1; i < DEF_MAXCLIENTS; i++)
		if (m_pClientList[i] != 0) {
			iRet = m_pClientList[i]->m_pXSock->iSendMsg(cTemp, dwMsgSize +22);
		}
		break;
	}
}

/*********************************************************************************************************************
**  int CGame::iClientMotion_Attack_Handler(int iClientH, short sX, short sY, short dX, short dY, short wType,		**
**		char cDir, UINT16 wTargetObjectID, bool bResponse, bool bIsDash)												**
**  DESCRIPTION			:: controls player attack																	**
**  LAST_UPDATED		:: March 19, 2005; 1:21 PM; Hypnotoad														**
**	RETURN_VALUE		:: int																						**
**  NOTES				::	- added checks for Firebow and Directionbow to see if player is m_bIsInsideWarehouse,	**
**							m_bIsInsideWizardTower, m_bIsInCombatantOnlyZone										**
**							- added ability to attack moving object													**
**							- fixed attack unmoving object															**
**	MODIFICATION		::	n/a																						**
**********************************************************************************************************************/
int CGame::iClientMotion_Attack_Handler(int iClientH, short sX, short sY, short dX, short dY, short wType, char cDir, UINT16 wTargetObjectID, bool bResponse, bool bIsDash)
{
 char cData[100];
 UINT32 * dwp, dwTime;
 UINT16  * wp;
 int     iRet, tdX = 0, tdY = 0, i;
 short   sOwner, sAbsX, sAbsY;
 char    cOwnerType;
 bool    bNearAttack = false, var_AC = false;
 short sItemIndex;
 int tX, tY, iErr;
 UINT32 iExp;

	if (m_pClientList[iClientH] == 0) return 0;
	if ((cDir <= 0) || (cDir > 8))       return 0;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return 0;
	if (m_pClientList[iClientH]->m_bIsKilled == true) return 0;
	dwTime = timeGetTime();
	m_pClientList[iClientH]->m_dwLastActionTime = dwTime;
	m_pClientList[iClientH]->m_iAttackMsgRecvCount++;
	if (m_pClientList[iClientH]->m_iAttackMsgRecvCount >= 7) {
		if (m_pClientList[iClientH]->m_dwAttackLAT != 0) {
			if ((dwTime - m_pClientList[iClientH]->m_dwAttackLAT) < (3500)) {		
				DeleteClient(iClientH, true, true, true);
				return 0;
			}
		}
		m_pClientList[iClientH]->m_dwAttackLAT = dwTime;
		m_pClientList[iClientH]->m_iAttackMsgRecvCount = 0;
	}
	if ((wTargetObjectID != 0) && (wType != 2)) {
		if (wTargetObjectID < DEF_MAXCLIENTS) {
			if (m_pClientList[wTargetObjectID] != 0) {
				tdX = m_pClientList[wTargetObjectID]->m_sX;
				tdY = m_pClientList[wTargetObjectID]->m_sY;
			}
		}
		else if ((wTargetObjectID > 10000) && (wTargetObjectID < (10000 + DEF_MAXNPCS))) {
			if (m_pNpcList[wTargetObjectID - 10000] != 0) {
				tdX = m_pNpcList[wTargetObjectID - 10000]->m_sX;
				tdY = m_pNpcList[wTargetObjectID - 10000]->m_sY;
			}
		}
		m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwner, &cOwnerType, dX, dY);
		if (sOwner == (wTargetObjectID - 10000)) {
			tdX = m_pNpcList[sOwner]->m_sX;
			dX = tdX;
			tdY = m_pNpcList[sOwner]->m_sY;
			dY = tdY;
			bNearAttack = false;
			var_AC = true;
		}
		if (var_AC != true) {
			if ((tdX == dX) && (tdY == dY)) {
				bNearAttack = false;	
			}
			else if ((abs(tdX - dX) <= 1) && (abs(tdY - dY) <= 1)) {
				dX = tdX;
				dY = tdY;
				bNearAttack = true;
			}
		}
	}
	if ((dX < 0) || (dX >= m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_sSizeX) || (dY < 0) || (dY >= m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_sSizeY)) return 0;
	if ((sX != m_pClientList[iClientH]->m_sX) || (sY != m_pClientList[iClientH]->m_sY)) return 2;
	
	if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex] != 0)
	{
		int iStX = m_pClientList[iClientH]->m_sX / 20;
		int iStY = m_pClientList[iClientH]->m_sY / 20;
		m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_stTempSectorInfo[iStX][iStY].iPlayerActivity++;
		switch (m_pClientList[iClientH]->m_cSide) {
		case 1: m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_stTempSectorInfo[iStX][iStY].iAresdenActivity++; break;
		case 2: m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_stTempSectorInfo[iStX][iStY].iElvineActivity++;  break;
		case 0:
		default: m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_stTempSectorInfo[iStX][iStY].iNeutralActivity++; break;
		}
	}

	sAbsX = abs(sX - dX);
	sAbsY = abs(sY - dY);
	if ((wType != 2) && (wType < 20)) {
		if (var_AC == false) {
			sItemIndex = m_pClientList[iClientH]->m_sItemEquipmentStatus[ DEF_EQUIPPOS_TWOHAND ];
			if (sItemIndex != -1) {
				if (m_pClientList[iClientH]->m_pItemList[sItemIndex] == 0) return 0;
				if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 845 || m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 1037) {
					if ((sAbsX > 4) || (sAbsY > 4)) wType = 0;
				}
				else{
					if ((sAbsX > 1) || (sAbsY > 1)) wType = 0;
				}
			}
			else{
				if ((sAbsX > 1) || (sAbsY > 1)) wType = 0;
			}
		}
		else{
			cDir = m_Misc.cGetNextMoveDir(sX, sY, dX, dY);
			if ((m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bCheckFlySpaceAvailable(sX, sY, cDir, sOwner)) != false)
				wType = 0;
		}
	}
	ClearSkillUsingStatus(iClientH);
	m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->ClearOwner(0, iClientH, DEF_OWNERTYPE_PLAYER, sX, sY);
	m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetOwner(iClientH, DEF_OWNERTYPE_PLAYER, sX, sY);
	m_pClientList[iClientH]->m_cDir = cDir;
	iExp = 0;
	m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwner, &cOwnerType, dX, dY);
	if (sOwner != 0) {
		if ((wType != 0) && ((dwTime - m_pClientList[iClientH]->m_dwRecentAttackTime) > 100)) { 
			if ((m_pClientList[iClientH]->m_pIsProcessingAllowed == false) && (m_pClientList[iClientH]->m_bIsInsideWarehouse == false) 
				&& (m_pClientList[iClientH]->m_bIsInsideWizardTower == false) && (m_pClientList[iClientH]->m_bIsInsideEnemyBuilding == false)) {
				// Magic Weapons - Add Special Effect for Weapon
				short sType, sManaCost;
				UINT32 dwType1, dwType2, dwValue1, dwValue2;
				bool bIce = false;

				if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND] != -1) {
					sItemIndex = m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND];
				}
				else if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND] != -1) {
					sItemIndex = m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND];
				}
				else sItemIndex = -1;

				if (sItemIndex != -1 && m_pClientList[iClientH]->m_pItemList[sItemIndex] != 0) {
					//Magn0S:: Battle Staffs coded by Magn0S
					if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ATTACK_MAGICITEM) {
						short sType;
						switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1) {
						case 0: // Normal BStaffs
							if (wType >= 20) {
								sType = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue3;
								BattleMageMagicHandler(iClientH, dX, dY, sType, true, 1);
							}
							else {
								sType = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
								BattleMageMagicHandler(iClientH, dX, dY, sType, true, 0);
							}
							break;

						case 1: // Triple Attacks
							switch (cDir) {
							case 1: // Cima / Baixo
							case 5:
								if (wType >= 20) {
									sType = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue3;
									BattleMageMagicHandler(iClientH, dX, dY, sType, true, 1);
									BattleMageMagicHandler(iClientH, dX + 1, dY, sType, true, 1);
									BattleMageMagicHandler(iClientH, dX - 1, dY, sType, true, 1);
								}
								else {
									sType = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
									BattleMageMagicHandler(iClientH, dX, dY, sType, true, 0);
									BattleMageMagicHandler(iClientH, dX + 1, dY, sType, true, 0);
									BattleMageMagicHandler(iClientH, dX - 1, dY, sType, true, 0);
								}
								break;

							case 2: // Diagonais
							case 6:
								if (wType >= 20) {
									sType = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue3;
									BattleMageMagicHandler(iClientH, dX, dY, sType, true, 1);
									BattleMageMagicHandler(iClientH, dX + 1, dY + 1, sType, true, 1);
									BattleMageMagicHandler(iClientH, dX - 1, dY - 1, sType, true, 1);
								}
								else {
									sType = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
									BattleMageMagicHandler(iClientH, dX, dY, sType, true, 0);
									BattleMageMagicHandler(iClientH, dX + 1, dY + 1, sType, true, 0);
									BattleMageMagicHandler(iClientH, dX - 1, dY - 1, sType, true, 0);
								}
								break;

							case 3: // Frente / Atras
							case 7:
								if (wType >= 20) {
									sType = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue3;
									BattleMageMagicHandler(iClientH, dX, dY, sType, true, 1);
									BattleMageMagicHandler(iClientH, dX, dY + 1, sType, true, 1);
									BattleMageMagicHandler(iClientH, dX, dY - 1, sType, true, 1);
								}
								else {
									sType = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
									BattleMageMagicHandler(iClientH, dX, dY, sType, true, 0);
									BattleMageMagicHandler(iClientH, dX, dY + 1, sType, true, 0);
									BattleMageMagicHandler(iClientH, dX, dY - 1, sType, true, 0);
								}
								break;

							case 4: // Diagonais
							case 8:
								if (wType >= 20) {
									sType = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue3;
									BattleMageMagicHandler(iClientH, dX, dY, sType, true, 1);
									BattleMageMagicHandler(iClientH, dX - 1, dY + 1, sType, true, 1);
									BattleMageMagicHandler(iClientH, dX + 1, dY - 1, sType, true, 1);
								}
								else {
									sType = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
									BattleMageMagicHandler(iClientH, dX, dY, sType, true, 0);
									BattleMageMagicHandler(iClientH, dX - 1, dY + 1, sType, true, 0);
									BattleMageMagicHandler(iClientH, dX + 1, dY - 1, sType, true, 0);
								}
								break;

							}
							break;

						default:
							if (wType >= 20) {
								sType = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue3;
								BattleMageMagicHandler(iClientH, dX, dY, sType, true, 1);
							}
							else {
								sType = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
								BattleMageMagicHandler(iClientH, dX, dY, sType, true, 0);
							}
							break;
						}
					}
					//-----
					if ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x00F00000) != 0) {
						dwType1 = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x00F00000) >> 20;
						dwValue1 = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x000F0000) >> 16;
						dwType2 = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x0000F000) >> 12;
						dwValue2 = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x00000F00) >> 8;
					}
					if (dwType1 == ITEMSTAT_MAGIC) {
						sManaCost = (short)dwValue2 * 2;
						if (m_pClientList[iClientH]->m_iMP >= sManaCost) {
							switch (dwValue1) {
							// Fire-Strike
							case 1:
								sType = 30;
								SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_MAGIC, m_pClientList[iClientH]->m_cMapIndex, sX, sY, dX, dY, (sType + 100), m_pClientList[iClientH]->m_sType);
								break;
							// Lightning
							case 2:
								sType = 43;
								SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_MAGIC, m_pClientList[iClientH]->m_cMapIndex, sX, sY, dX, dY, (sType + 100), m_pClientList[iClientH]->m_sType);
								break;
							// Chill-Wind
							case 3:
								sType = 45;
								bIce = true;
								SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_MAGIC, m_pClientList[iClientH]->m_cMapIndex, sX, sY, dX, dY, (sType + 100), m_pClientList[iClientH]->m_sType);
								break;
							// Ice-Strike
							case 4:
								sType = 57;
								bIce = true;
								SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_MAGIC, m_pClientList[iClientH]->m_cMapIndex, sX, sY, dX, dY, (sType + 100), m_pClientList[iClientH]->m_sType);
								break;
							// Energy-Strike
							case 5:
								sType = 60;
								SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_MAGIC, m_pClientList[iClientH]->m_cMapIndex, sX, sY, dX, dY, (sType + 100), m_pClientList[iClientH]->m_sType);
								break;
							// Mass-Fire-Strike
							case 6:
								sType = 61;
								SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_MAGIC, m_pClientList[iClientH]->m_cMapIndex, sX, sY, dX, dY, (sType + 100), m_pClientList[iClientH]->m_sType);
								break;
							// Mass-Chill-Wind
							case 7:
								sType = 63;
								bIce = true;
								SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_MAGIC, m_pClientList[iClientH]->m_cMapIndex, sX, sY, dX, dY, (sType + 100), m_pClientList[iClientH]->m_sType);
								break;
							// Earthworm-Strike
							case 8:
								sType = 64;
								SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_MAGIC, m_pClientList[iClientH]->m_cMapIndex, sX, sY, dX, dY, (sType + 100), m_pClientList[iClientH]->m_sType);
								break;
							// Bloody-Shock-Wave
							case 9:
								sType = 70;
								SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_MAGIC, m_pClientList[iClientH]->m_cMapIndex, sX, sY, dX, dY, (sType + 100), m_pClientList[iClientH]->m_sType);
								break;
							// Mass-Ice-Strike
							case 10:
								sType = 72;
								bIce = true;
								SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_MAGIC, m_pClientList[iClientH]->m_cMapIndex, sX, sY, dX, dY, (sType + 100), m_pClientList[iClientH]->m_sType);
								break;
							// Lightning-Strike
							case 11:
								sType = 74;
								SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_MAGIC, m_pClientList[iClientH]->m_cMapIndex, sX, sY, dX, dY, (sType + 100), m_pClientList[iClientH]->m_sType);
								break;
							// Ice-Whirlwind
							case 12:
								sType = 75;
								bIce = true;
								SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_MAGIC, m_pClientList[iClientH]->m_cMapIndex, sX, sY, dX, dY, (sType + 100), m_pClientList[iClientH]->m_sType);
								break;
							// Meteor-Strike
							case 13:
								sType = 81;
								SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_MAGIC, m_pClientList[iClientH]->m_cMapIndex, sX, sY, dX, dY, (sType + 100), m_pClientList[iClientH]->m_sType);
								break;
							// Mass-Magic-Missile
							case 14:
								sType = 82;
								SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_MAGIC, m_pClientList[iClientH]->m_cMapIndex, sX, sY, dX, dY, (sType + 100), m_pClientList[iClientH]->m_sType);
								break;
							// Blizzard
							case 15:
								sType = 91;
								bIce = true;
								SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_MAGIC, m_pClientList[iClientH]->m_cMapIndex, sX, sY, dX, dY, (sType + 100), m_pClientList[iClientH]->m_sType);
								break;
							// Earth-Shock-Wave
							case 16:
								sType = 96;
								SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_MAGIC, m_pClientList[iClientH]->m_cMapIndex, sX, sY, dX, dY, (sType + 100), m_pClientList[iClientH]->m_sType);
								break;
							}

							// Centuu - magic items frozen
							switch (cOwnerType) {
							case DEF_OWNERTYPE_PLAYER:
								if (m_pClientList[sOwner]->m_iAdminUserLevel < 1) {
									if ((bIce) && (m_pClientList[sOwner]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwner, cOwnerType, (int)sType) == false))
									{
										if (m_pClientList[sOwner]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
										{
											m_pClientList[sOwner]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
											SetIceFlag(sOwner, cOwnerType, true);
											bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
												sOwner, cOwnerType, 0, 0, 0, 1, 0, 0);
											SendNotifyMsg(0, sOwner, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_ICE, 1, 0, 0);
										}
									}
								}
								break;
							case DEF_OWNERTYPE_NPC:
								if ((bIce) && (m_pNpcList[sOwner]->m_iHP > 0) && (bCheckResistingIceSuccess(m_pClientList[iClientH]->m_cDir, sOwner, cOwnerType, (int)sType) == false)) {
									if (m_pNpcList[sOwner]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0)
									{
										m_pNpcList[sOwner]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
										SetIceFlag(sOwner, cOwnerType, true);
										bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (m_pMagicConfigList[sType]->m_sValue10 * 1000),
											sOwner, cOwnerType, 0, 0, 0, 1, 0, 0);
									}
								}
								break;
							}
						}
					}
					else if (dwType1 == ITEMSTAT_POISONING) {
						// Centuu - fix for poison
						switch (cOwnerType) {
						case DEF_OWNERTYPE_PLAYER:
							if (m_pClientList[sOwner]->m_iAdminUserLevel < 1) 
							{
								if (!m_pClientList[sOwner]->m_bIsPoisoned) 
								{
									if (bCheckResistingPoisonSuccess(sOwner, cOwnerType) == false)
									{
										m_pClientList[sOwner]->m_bIsPoisoned = true;
										m_pClientList[sOwner]->m_iPoisonLevel = 40;
										m_pClientList[sOwner]->m_dwPoisonTime = dwTime;
										SetPoisonFlag(sOwner, cOwnerType, true);
										SendNotifyMsg(0, sOwner, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_POISON, m_pClientList[sOwner]->m_iPoisonLevel, 0, 0);
									}
								}
							}
							break;
						case DEF_OWNERTYPE_NPC:
							if (bCheckResistingPoisonSuccess(sOwner, cOwnerType) == false)
							{	if (m_pNpcList[sOwner]->m_iPoisonLevel < 100)
									m_pNpcList[sOwner]->m_iPoisonLevel = 100;
								SetPoisonFlag(sOwner, DEF_OWNERTYPE_NPC, true);
							}
							break;
						}
					}
				}
				// End of Magic Weapons
				sItemIndex = m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND];
				if (sItemIndex != -1 && m_pClientList[iClientH]->m_pItemList[sItemIndex] != 0) {
					if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 845 || // StormBringer
						m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 1037) { // StunSB
						
						iExp += iCalculateAttackEffect(sOwner, cOwnerType, iClientH, DEF_OWNERTYPE_PLAYER, dX, dY, wType, bNearAttack, bIsDash, false);
					}
					else if(m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 1050){ // hunt bow 
						iErr = 0; 
						for(i = 1; i < 2; i++){ 
							m_Misc.GetPoint2(sX, sY, dX, dY, &tX, &tY, &iErr, i); 
							m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwner, &cOwnerType, tX, tY); 
							iExp += iCalculateAttackEffect(sOwner, cOwnerType, iClientH, DEF_OWNERTYPE_PLAYER, tX, tY, wType, bNearAttack, bIsDash, true); // 1 
							if (((abs(tdX - dX) >= 1) && (abs(tdY - dY) >= 1)) || ((abs(tdX - dX) <= 1) && (abs(tdY - dY) <= 1))) { 
								m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwner, &cOwnerType, dX, dY); 
								iExp += iCalculateAttackEffect(sOwner, cOwnerType, iClientH, DEF_OWNERTYPE_PLAYER, dX, dY, wType, bNearAttack, bIsDash, false); // 0 
								while (((tdX - dX) >= 1) && ((tdY - dY) >= 1)) { 
									dX += 4; dY += 4; tY += 4; tX += 4; 
								}
								while (((tdX - dX) >= 1) && ((tdY - dY) >= 1)) { 
									dX += 4; dY += 4; tY += 4; tX += 4; 
								} 
							} 
						} 
					}
					else if(m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 873){ // Firebow
						if ((m_pClientList[iClientH]->m_sAppr2 & 0xF000) != 0) {
							
							iAddDynamicObjectList(iClientH, DEF_OWNERTYPE_PLAYER_INDIRECT, DEF_DYNAMICOBJECT_FIRE3, m_pClientList[iClientH]->m_cMapIndex, dX, dY, (iDice(1,7)+3)*1000, 8);
							
							iExp += iCalculateAttackEffect(sOwner, cOwnerType, iClientH, DEF_OWNERTYPE_PLAYER, dX, dY, wType, bNearAttack, bIsDash, false); // 0
						}
					}
					else{
						iExp += iCalculateAttackEffect(sOwner, cOwnerType, iClientH, DEF_OWNERTYPE_PLAYER, dX, dY, wType, bNearAttack, bIsDash, false); // 0
					}
				}
				else{
					iExp += iCalculateAttackEffect(sOwner, cOwnerType, iClientH, DEF_OWNERTYPE_PLAYER, dX, dY, wType, bNearAttack, bIsDash, false); // 0
				}
			}
			else{
				iExp += iCalculateAttackEffect(sOwner, cOwnerType, iClientH, DEF_OWNERTYPE_PLAYER, dX, dY, wType, bNearAttack, bIsDash, false); // 0
			}
			if (m_pClientList[iClientH] == 0) return 0;
			m_pClientList[iClientH]->m_dwRecentAttackTime = dwTime;
		}
	}
	else _CheckMiningAction(iClientH, dX, dY);

	if (iExp != 0) {
		GetExp(iClientH, iExp, true);
	}

	if (bResponse == true) {
		dwp  = (UINT32 *)(cData + DEF_INDEX4_MSGID);
		*dwp = MSGID_RESPONSE_MOTION;
		wp   = (UINT16 *)(cData + DEF_INDEX2_MSGTYPE);
		*wp  = DEF_OBJECTMOTION_ATTACK_CONFIRM;
	
		iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 6);
		switch (iRet) {
		case DEF_XSOCKEVENT_QUENEFULL:
		case DEF_XSOCKEVENT_SOCKETERROR:
		case DEF_XSOCKEVENT_CRITICALERROR:
		case DEF_XSOCKEVENT_SOCKETCLOSED:
			DeleteClient(iClientH, true, true, true);
			return 0;
		}
	}

	return 1;
}

void CGame::MsgProcess()
{
 char   * pData, cFrom, cKey;
 UINT32    dwMsgSize, * dwpMsgID;
 UINT16   * wpMsgType, * wp;
 int      i, iClientH;
 char   m_msgBuff[100];

 char *cp, cData[120];
UINT32 dwTime = timeGetTime();
UINT32 * dwp, dwTimeRcv;
	char cBuffer[256];
	int iRet;

	ZeroMemory(cBuffer, 256);
	
	if ((m_bF5pressed == true) && (m_bF1pressed == true)) {
		PutLogList("(XXX) RELOADING CONFIGS MANUALY...");
			for (i = 1; i < DEF_MAXCLIENTS; i++)
				if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_bIsInitComplete == true)) {
					bSendMsgToLS(MSGID_REQUEST_SAVEPLAYERDATA, i);
				}
				bInit();
		}

	if ((m_bF1pressed == true) && (m_bF4pressed == true) && (m_bOnExitProcess == false)) {
		// Â¸Ã°ÂµÃ§ Ã…Â¬Â¶Ã³Ã€ÃŒÂ¾Ã°Ã†Â®Ã€Ã‡ ÂµÂ¥Ã€ÃŒÃ…ÃÂ¸Â¦ Ã€ÃºÃ€Ã¥Ã‡Ã‘ ÃˆÃ„ Ã‡ÃÂ·ÃŽÂ±Ã—Â·Â¥Ã€Â» ÃÂ¾Â·Ã¡Ã‡Ã˜Â¾ÃŸ Ã‡Ã‘Â´Ã™. 
		m_cShutDownCode      = 2;
		m_bOnExitProcess     = true;
		m_dwExitProcessTime  = timeGetTime();
		// Â°Ã”Ã€Ã“ Â¼Â­Â¹Ã¶ Â¼Ã‹Â´Ã™Â¿Ã®Ã€ÃŒ Â½ÃƒÃ€Ã›ÂµÃ‡Â¸Ã© Â´ÃµÃ€ÃŒÂ»Ã³Ã€Ã‡ Ã…Â¬Â¶Ã³Ã€ÃŒÂ¾Ã°Ã†Â® ÃÂ¢Â¼Ã“Ã€ÃŒ Â°Ã…ÂºÃŽÂµÃˆÂ´Ã™. 
		PutLogList("(!) GAME SERVER SHUTDOWN PROCESS BEGIN(by Local command)!!!");
		// Â·ÃŽÂ±Ã— Â¼Â­Â¹Ã¶Â¿Â¡Â°Ã” Ã‡Ã¶Ã€Ã§ Â¼Â­Â¹Ã¶Â°Â¡ Â¼Ã‹Â´Ã™Â¿Ã® ÃÃŸÃ€Ã“Ã€Â» Â¾Ã‹Â¸Â°Â´Ã™. 
		bSendMsgToLS(MSGID_GAMESERVERSHUTDOWNED, 0);
		// OccupyFlag ÂµÂ¥Ã€ÃŒÃ…Ã Ã€ÃºÃ€Ã¥ 

		
		return;
	}
	
	ZeroMemory(m_pMsgBuffer, DEF_MSGBUFFERSIZE+1);
	pData = (char *)m_pMsgBuffer;

	m_iCurMsgs = 0;
	while (bGetMsgQuene(&cFrom, pData, &dwMsgSize, &iClientH, &cKey) == true) {
		
		m_iCurMsgs++;
		if (m_iCurMsgs > m_iMaxMsgs) m_iMaxMsgs = m_iCurMsgs;

		switch (cFrom) {

		case DEF_MSGFROM_GATESERVER:
			dwpMsgID   = (UINT32 *)(pData + DEF_INDEX4_MSGID);
			wpMsgType  = (UINT16 *)(pData + DEF_INDEX2_MSGTYPE);
			switch (*dwpMsgID) {
			case MSGID_PARTYOPERATION:
				PartyOperationResultHandler(pData);
				break;

			case MSGID_SERVERSTOCKMSG:
				ServerStockMsgHandler(pData);
				break;
									
			case MSGID_SENDSERVERSHUTDOWNMSG:
				wp = (UINT16 *)(pData + DEF_INDEX2_MSGTYPE + 2);
				switch (*wp) {
				case 1:
					PutLogList("(!) Send server shutdown announcement - 1.");
					for (i = 1; i < DEF_MAXCLIENTS; i++)
					if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_bIsInitComplete == true)) {
						SendNotifyMsg(0, i, DEF_NOTIFY_SERVERSHUTDOWN, 1, 0, 0, 0);
					}
					break;
				case 2:
					PutLogList("(!) Send server shutdown announcement - 2.");
					for (i = 1; i < DEF_MAXCLIENTS; i++)
					if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_bIsInitComplete == true)) {
						SendNotifyMsg(0, i, DEF_NOTIFY_SERVERSHUTDOWN, 2, 0, 0, 0);
					}
					break;
				}
				bSendMsgToLS(MSGID_SENDSERVERSHUTDOWNMSG, 0, 0); 
				break;
			
			case MSGID_GAMESERVERSHUTDOWNED:
				m_cShutDownCode      = 1;
				m_bOnExitProcess     = true;
				m_dwExitProcessTime  = timeGetTime();
				PutLogList("(!) GAME SERVER SHUTDOWN PROCESS BEGIN(by Global command)!!!");
				bSendMsgToLS(MSGID_GAMESERVERSHUTDOWNED, 0);
				
				break;
			
			case MSGID_TOTALGAMESERVERCLIENTS:
				wp = (UINT16 *)(pData + DEF_INDEX2_MSGTYPE + 2);
				m_iTotalGameServerClients = (int)*wp;
				if (m_iTotalGameServerClients > m_iTotalGameServerMaxClients) 
					m_iTotalGameServerMaxClients = m_iTotalGameServerClients;
				break;
			
			case MSGID_RESPONSE_REGISTERGAMESERVER:
				switch (*wpMsgType) {
				case DEF_MSGTYPE_CONFIRM:
					PutLogList("(!) Game Server registration to Gate Server - Success!");
					m_bIsGateSockAvailable = true;
					//50Cent - HG start process fix
                    bSendMsgToLS(MSGID_REQUEST_REGISTERGAMESERVER, 0);
					break;

				case DEF_MSGTYPE_REJECT:
					PutLogList("(!) Game Server registration to Gate Server - Fail!");
					PutLogList(" ");
					PutLogList("(!!!) STOPPED!");
					break;
				}
				break;
			}
			break;

		case DEF_MSGFROM_LOGSERVER:
			dwpMsgID   = (UINT32 *)(pData + DEF_INDEX4_MSGID);
			wpMsgType  = (UINT16 *)(pData + DEF_INDEX2_MSGTYPE);

			switch (*dwpMsgID) {
			case MSGID_REQUEST_CHECKACCOUNTPASSWORD:
				// Â°Ã¨ÃÂ¤Ã€Ã‡ Ã†ÃÂ½ÂºÂ¿Ã¶ÂµÃ¥Â¸Â¦ ÂºÃ±Â±Â³ 
				RequestCheckAccountPasswordHandler(pData, dwMsgSize);
				break;
			
			case MSGID_REQUEST_FORCEDISCONECTACCOUNT:
				// Â°Â­ÃÂ¦ Â°Ã¨ÃÂ¤ ÃÂ¢Â¼Ã“ ÃÂ¾Â·Ã¡. ÂµÂ¥Ã€ÃŒÃ…ÃÂ¸Â¦ Ã€ÃºÃ€Ã¥Ã‡ÃÂ°Ã­ ÃÂ¢Â¼Ã“Ã€Â» ÃÂ¾Â·Ã¡Â½ÃƒÃ…Â²Â´Ã™. 
				wpMsgType  = (UINT16 *)(pData + DEF_INDEX2_MSGTYPE);
				ForceDisconnectAccount((char *)(pData + DEF_INDEX2_MSGTYPE + 2), *wpMsgType);
				break;
			
			case MSGID_RESPONSE_SAVEPLAYERDATA_REPLY:
				// Ã„Â³Â¸Â¯Ã…ÃÃ€Ã‡ ÂµÂ¥Ã€ÃŒÃ…ÃÂ°Â¡ Ã€ÃºÃ€Ã¥ÂµÃ‡Â¾ÃºÃ€Â½Ã€Â» Â¾Ã‹Â¸Â®Â´Ã‚ Â¸ÃžÂ½ÃƒÃÃ¶ 
				ResponseSavePlayerDataReplyHandler(pData, dwMsgSize);
				break;
			
			case MSGID_GUILDNOTIFY:
				// Â´Ã™Â¸Â¥ Â°Ã”Ã€Ã“Â¼Â­Â¹Ã¶Â¿Â¡Â¼Â­ Â¹ÃŸÂ»Ã½Ã‡Ã‘ Â±Ã¦ÂµÃ¥ Ã€ÃŒÂºÂ¥Ã†Â®Â¿Â¡ Â´Ã«Ã‡Ã‘ Ã…Ã«ÂºÂ¸(Â±Ã¦ÂµÃ¥Â¿Ã¸ Â°Â¡Ã€Ã”, Â±Ã¦ÂµÃ¥Â¿Ã¸ Ã…Â»Ã…Ã°, Â±Ã¦ÂµÃ¥ Ã‡Ã˜Â»Ãª ÂµÃ®)
				GuildNotifyHandler(pData, dwMsgSize);
				break;
			
			case MSGID_RESPONSE_DISBANDGUILD:
				// Â±Ã¦ÂµÃ¥ Ã‡Ã˜Â»ÃªÂ¿Ã¤Â±Â¸Â¿Â¡ Â´Ã«Ã‡Ã‘ Ã€Ã€Â´Ã¤ 
				ResponseDisbandGuildHandler(pData, dwMsgSize);
				break;

			case MSGID_RESPONSE_CREATENEWGUILD:
				// Ã„Â³Â¸Â¯Ã…ÃÃ€Ã‡ Â±Ã¦ÂµÃ¥Â»Ã½Â¼Âº Â¿Ã¤Â±Â¸Â¿Â¡ Â´Ã«Ã‡Ã‘ Ã€Ã€Â´Ã¤. 
				ResponseCreateNewGuildHandler(pData, dwMsgSize);
				break;
			
			case MSGID_RESPONSE_REGISTERGAMESERVER:
				switch (*wpMsgType) {
				case DEF_MSGTYPE_CONFIRM:
					PutLogList("(!) Game Server registration to Log Server - Success!");
					m_bIsLogSockAvailable = true;
					break;

				case DEF_MSGTYPE_REJECT:
					PutLogList("(!) Game Server registration to Log Server - Fail!");
					break;
				}
		
				if (m_bIsBuildItemAvailable == false) {
					// Â¸Â¸Â¾Ã  ÂºÃ´ÂµÃ¥Â¾Ã†Ã€ÃŒÃ…Ã› ÃÂ¤Ã€Ã‡ ÃˆÂ­Ã€ÃÂ¿Â¡Â¼Â­ Â¿Â¡Â·Â¯Â°Â¡ Ã€Ã–Â¾ÃºÂ´Ã™Â¸Ã© Â°Ã”Ã€Ã“Â¼Â­Â¹Ã¶ ÂµÃ®Â·ÃÂ¿Â¡ Â»Ã³Â°Ã¼Â¾Ã¸Ã€ÃŒ ÂµÂ¿Ã€Ã› ÂºÃ’Â°Â¡Â´Ã‰.
					PutLogList(" ");
					PutLogList("(!!!) STOPPED! Build-Item configuration error.");
				}
								
				if (m_bIsItemAvailable == false) {
					// Â¸Â¸Â¾Ã  Â¾Ã†Ã€ÃŒÃ…Ã› ÃÂ¤Ã€Ã‡ ÃˆÂ­Ã€ÃÂ¿Â¡Â¼Â­ Â¿Â¡Â·Â¯Â°Â¡ Ã€Ã–Â¾ÃºÂ´Ã™Â¸Ã© Â°Ã”Ã€Ã“Â¼Â­Â¹Ã¶ ÂµÃ®Â·ÃÂ¿Â¡ Â»Ã³Â°Ã¼Â¾Ã¸Ã€ÃŒ ÂµÂ¿Ã€Ã› ÂºÃ’Â°Â¡Â´Ã‰.
					PutLogList(" ");
					PutLogList("(!!!) STOPPED! Item configuration error.");
				}
				
				if (m_bIsNpcAvailable == false) {
					// Â¸Â¸Â¾Ã  Â¾Ã†Ã€ÃŒÃ…Ã› ÃÂ¤Ã€Ã‡ ÃˆÂ­Ã€ÃÂ¿Â¡Â¼Â­ Â¿Â¡Â·Â¯Â°Â¡ Ã€Ã–Â¾ÃºÂ´Ã™Â¸Ã© Â°Ã”Ã€Ã“Â¼Â­Â¹Ã¶ ÂµÃ®Â·ÃÂ¿Â¡ Â»Ã³Â°Ã¼Â¾Ã¸Ã€ÃŒ ÂµÂ¿Ã€Ã› ÂºÃ’Â°Â¡Â´Ã‰.
					PutLogList(" ");
					PutLogList("(!!!) STOPPED! Npc configuration error.");
				}

				if (m_bIsMagicAvailable == false) {
					// Â¸Â¸Â¾Ã  MAGIC ÃÂ¤Ã€Ã‡ ÃˆÂ­Ã€ÃÂ¿Â¡Â¼Â­ Â¿Â¡Â·Â¯Â°Â¡ Ã€Ã–Â¾ÃºÂ´Ã™Â¸Ã© Â°Ã”Ã€Ã“Â¼Â­Â¹Ã¶ ÂµÃ®Â·ÃÂ¿Â¡ Â»Ã³Â°Ã¼Â¾Ã¸Ã€ÃŒ ÂµÂ¿Ã€Ã› ÂºÃ’Â°Â¡Â´Ã‰.
					PutLogList(" ");
					PutLogList("(!!!) STOPPED! MAGIC configuration error.");
				}

				if (m_bIsSkillAvailable == false) {
					// Â¸Â¸Â¾Ã  SKILL ÃÂ¤Ã€Ã‡ ÃˆÂ­Ã€ÃÂ¿Â¡Â¼Â­ Â¿Â¡Â·Â¯Â°Â¡ Ã€Ã–Â¾ÃºÂ´Ã™Â¸Ã© Â°Ã”Ã€Ã“Â¼Â­Â¹Ã¶ ÂµÃ®Â·ÃÂ¿Â¡ Â»Ã³Â°Ã¼Â¾Ã¸Ã€ÃŒ ÂµÂ¿Ã€Ã› ÂºÃ’Â°Â¡Â´Ã‰.
					PutLogList(" ");
					PutLogList("(!!!) STOPPED! SKILL configuration error.");
				}

				if (m_bIsQuestAvailable == false) {
					// Â¸Â¸Â¾Ã  SKILL ÃÂ¤Ã€Ã‡ ÃˆÂ­Ã€ÃÂ¿Â¡Â¼Â­ Â¿Â¡Â·Â¯Â°Â¡ Ã€Ã–Â¾ÃºÂ´Ã™Â¸Ã© Â°Ã”Ã€Ã“Â¼Â­Â¹Ã¶ ÂµÃ®Â·ÃÂ¿Â¡ Â»Ã³Â°Ã¼Â¾Ã¸Ã€ÃŒ ÂµÂ¿Ã€Ã› ÂºÃ’Â°Â¡Â´Ã‰.
					PutLogList(" ");
					PutLogList("(!!!) STOPPED! QUEST configuration error.");
				}

				if (m_bIsPotionAvailable == false) {
					// Â¸Â¸Â¾Ã  Potion ÃÂ¤Ã€Ã‡ ÃˆÂ­Ã€ÃÂ¿Â¡Â¼Â­ Â¿Â¡Â·Â¯Â°Â¡ Ã€Ã–Â¾ÃºÂ´Ã™Â¸Ã© Â°Ã”Ã€Ã“Â¼Â­Â¹Ã¶ ÂµÃ®Â·ÃÂ¿Â¡ Â»Ã³Â°Ã¼Â¾Ã¸Ã€ÃŒ ÂµÂ¿Ã€Ã› ÂºÃ’Â°Â¡Â´Ã‰.
					PutLogList(" ");
					PutLogList("(!!!) STOPPED! POTION configuration error.");
				}

				// centu - leer NpcItem.cfg 
				if (_bDecodeNpcItemConfigFileContents("GameConfigs\\NpcItem.cfg") == false)
				{
					PutLogList(" ");
					PutLogList("(!!!) STOPPED! NpcItem configuration error.");
				}
				break;

			case MSGID_RESPONSE_PLAYERDATA:
				// Ã‡ÃƒÂ·Â¹Ã€ÃŒÂ¾Ã® ÂµÂ¥Ã€ÃŒÃ…ÃÂ°Â¡ ÂµÂµÃ‚Ã¸Ã‡ÃŸÂ´Ã™. 
				ResponsePlayerDataHandler(pData, dwMsgSize);
				break;

			case MSGID_BUILDITEMCONFIGURATIONCONTENTS:
				// Build Item contents
				PutLogList("(!) BUILD-ITEM configuration contents received. Now decoding...");
				m_bIsBuildItemAvailable = _bDecodeBuildItemConfigFileContents((char *)(pData + DEF_INDEX2_MSGTYPE + 2), dwMsgSize);
				break;
			
			case MSGID_ITEMCONFIGURATIONCONTENTS:
				// Â·ÃŽÂ±Ã—Â¼Â­Â¹Ã¶Â·ÃŽÂºÃŽÃ…Ã Â¾Ã†Ã€ÃŒÃ…Ã› Ã„ÃÃ‡Ã‡Â±Ã”Â·Â¹Ã€ÃŒÂ¼Ã‡ ÂµÂ¥Ã€ÃŒÃ…ÃÂ°Â¡ ÂµÂµÃ‚Ã¸Ã‡ÃŸÂ´Ã™.
				PutLogList("(!) ITEM configuration contents received. Now decoding...");
				m_bIsItemAvailable = _bDecodeItemConfigFileContents((char *)(pData + DEF_INDEX2_MSGTYPE + 2), dwMsgSize);
				break;

			case MSGID_NPCCONFIGURATIONCONTENTS:
				// NPC Ã„ÃÃ‡Ã‡Â±Ã”Â·Â¹Ã€ÃŒÂ¼Ã‡ ÂµÂ¥Ã€ÃŒÃ…ÃÂ°Â¡ ÂµÂµÃ‚Ã¸Ã‡ÃŸÂ´Ã™. 
				PutLogList("(!) NPC configuration contents received. Now decoding...");
				m_bIsNpcAvailable = _bDecodeNpcConfigFileContents((char *)(pData + DEF_INDEX2_MSGTYPE + 2), dwMsgSize);
				break;
			
			case MSGID_MAGICCONFIGURATIONCONTENTS:
				PutLogList("(!) MAGIC configuration contents received. Now decoding...");
				m_bIsMagicAvailable = _bDecodeMagicConfigFileContents((char *)(pData + DEF_INDEX2_MSGTYPE + 2), dwMsgSize);
				break;

			case MSGID_SKILLCONFIGURATIONCONTENTS:
				PutLogList("(!) SKILL configuration contents received. Now decoding...");
				m_bIsSkillAvailable = _bDecodeSkillConfigFileContents((char *)(pData + DEF_INDEX2_MSGTYPE + 2), dwMsgSize);
				break;

			case MSGID_QUESTCONFIGURATIONCONTENTS:
				PutLogList("(!) QUEST configuration contents received. Now decoding...");
				m_bIsQuestAvailable = _bDecodeQuestConfigFileContents((char *)(pData + DEF_INDEX2_MSGTYPE + 2), dwMsgSize);
				break;

			case MSGID_POTIONCONFIGURATIONCONTENTS:
				PutLogList("(!) POTION configuration contents received. Now decoding...");
				m_bIsPotionAvailable = _bDecodePotionConfigFileContents((char *)(pData + DEF_INDEX2_MSGTYPE + 2), dwMsgSize);
				break;

			case MSGID_DUPITEMIDFILECONTENTS:
				PutLogList("(!) DupItemID file contents received. Now decoding...");
				_bDecodeDupItemIDFileContents((char *)(pData + DEF_INDEX2_MSGTYPE + 2), dwMsgSize);
				break;

			case MSGID_NOTICEMENTFILECONTENTS:
				PutLogList("(!) Noticement file contents received. Now decoding...");
				_bDecodeNoticementFileContents((char *)(pData + DEF_INDEX2_MSGTYPE + 2), dwMsgSize);
				break;

			// v2.17 2002-8-7 // 2002-09-06 #1
			case MSGID_NPCITEMCONFIGCONTENTS:
				break;

			}
			break;
		
		case DEF_MSGFROM_CLIENT:

			dwpMsgID = (UINT32 *)(pData + DEF_INDEX4_MSGID);
			switch (*dwpMsgID) { // 84148741

			case MSGID_REQUEST_SETTRAP: // kamal
				RequestSetTrapHandler(iClientH, pData);
				break;

			case MSGID_REQUEST_ONLINE:
				
				RequestOnlines(iClientH); //no necesitamos la data
				
				break;

			case MSGID_REQUEST_TOPEK:
				for (i = 1; i <= 15; i++)
				{
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_TOP15HB, i, aHBTopKills[i], 0, aHBTopClientH[i]);
				}
				break;

			case MSGID_REQUEST_SKILLPOINT:
				for (i = 0; i < 24; i++)
				{
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_SKILLPOINT, i, m_pClientList[iClientH]->m_iSkillSSN[i], 0, 0);
				}
				break;

			case MSGID_REQUEST_PING:
				
				//Change ping response

				if (m_pClientList[iClientH] == 0) return;

				cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
				dwp = (UINT32 *)cp;
				dwTimeRcv = *dwp;

				dwp = (UINT32 *)(cBuffer + DEF_INDEX4_MSGID);
				*dwp = MSGID_RESPONSE_PING;
				wp  = (UINT16 *)(cBuffer + DEF_INDEX2_MSGTYPE);
				*wp = 0;

				cp = (char *)(cBuffer + DEF_INDEX2_MSGTYPE + 2);
				dwp = (UINT32 *)cp;
				*dwp = dwTimeRcv;
				cp += 4;

				iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cBuffer, 10);
				break;

			case DEF_REQUEST_ANGEL: // Angels by Snoopy...
				GetAngelHandler(iClientH, pData, dwMsgSize);
				break;

			case DEF_REQUEST_CHANGE_CLASS:
				ChangeClassHandler(iClientH, pData, dwMsgSize);
				break;

			case DEF_REQUEST_RESET_STATS:
				ResetStatsHandler(iClientH, pData, dwMsgSize);
				break;

			case MSGID_REQUEST_LEAVEARENA:
				RequestLeaveArena(iClientH);
				break;

			case MSGID_REQUEST_SUMMONGUILD:
				for (i = 0; i < DEF_MAXGUILDS; i++)
				{
					if (string(m_stSummonGuild[i].cGuildName) == "NONE") continue;
					if (string(m_stSummonGuild[i].cGuildName) == m_pClientList[iClientH]->m_cGuildName)
					{
						RequestTeleportHandler(iClientH, "2   ", m_stSummonGuild[i].cMap, m_stSummonGuild[i].sX, m_stSummonGuild[i].sY);
						break;
					}
				}
				break;
						
			case MSGID_REQUEST_INITPLAYER:
				RequestInitPlayerHandler(iClientH, pData, cKey);
				break;

			case MSGID_REQUEST_INITDATA:
				if (m_bAntiHackMode) {
					// Anti Bump 
					if (m_pClientList[iClientH]->m_bIsClientConnected == true) {
						if (m_pClientList[iClientH] == 0) break;
						wsprintf(G_cTxt, "(!!!) Client (%s) connection closed!. Sniffer suspect!.", m_pClientList[iClientH]->m_cCharName);
						PutLogList(G_cTxt);
						ZeroMemory(cData, sizeof(cData));
						cp = (char*)cData;
						*cp = GSM_DISCONNECT;
						cp++;
						memcpy(cp, m_pClientList[iClientH]->m_cCharName, 10);
						cp += 10;
						bStockMsgToGateServer(cData, 11);
						m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->ClearOwner(2, iClientH, DEF_OWNERTYPE_PLAYER, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY);
						bRemoveFromDelayEventList(iClientH, DEF_OWNERTYPE_PLAYER, 0);
						bSendMsgToLS(MSGID_REQUEST_SAVEPLAYERDATALOGOUT, iClientH, false);
						if ((dwTime - m_dwGameTime2) > 3000) { // 3 segs
							m_pClientList[iClientH]->m_bIsClientConnected = false;
							DeleteClient(iClientH, true, true, true, true);
						}
						break;
					}
					else {
						m_pClientList[iClientH]->m_bIsClientConnected = true;
						RequestInitDataHandler(iClientH, pData, cKey, false);
					}
				}
				else {
					RequestInitDataHandler(iClientH, pData, cKey, false);
				}
				break;

			case MSGID_COMMAND_CHECKCONNECTION:
				CheckConnectionHandler(iClientH, pData);
				break;

			case DEF_REQUEST_RESURRECTPLAYER_YES:
				RequestResurrectPlayer(iClientH, true);
				break;

			case DEF_REQUEST_RESURRECTPLAYER_NO:
				RequestResurrectPlayer(iClientH, false);
				break;

			case MSGID_REQUEST_SELLITEMLIST:
				RequestSellItemListHandler(iClientH, pData);
				break;
			
			case MSGID_REQUEST_RESTART:
				RequestRestartHandler(iClientH);
				break;

			case MSGID_REQUEST_PANNING:
				iRequestPanningMapDataRequest(iClientH, pData);
				break;

			case MSGID_REQUEST_NOTICEMENT:
				RequestNoticementHandler(iClientH, pData);
				break;
			
			case MSGID_BWM_COMMAND_SHUTUP:
				_BWM_Command_Shutup(pData);
				break;
			
			case MSGID_BWM_INIT:
				_BWM_Init(iClientH, pData);
				break;
			
			case MSGID_REQUEST_SETITEMPOS:
				_SetItemPos(iClientH, pData);
				break;
			
			case MSGID_ADMINUSER:
				break;
			
			case MSGID_REQUEST_FULLOBJECTDATA:
				RequestFullObjectData(iClientH, pData);
				break;
			
			case MSGID_REQUEST_RETRIEVEITEM:
				RequestRetrieveItemHandler(iClientH, pData);
				break;

			case MSGID_REQUEST_CIVILRIGHT:
				RequestCivilRightHandler(iClientH, pData);
				break;

			case MSGID_REQUEST_TELEPORT:
				RequestTeleportHandler(iClientH, pData);
				break;

			case MSGID_COMMAND_COMMON:
				ClientCommonHandler(iClientH, pData);
				break;

			case MSGID_COMMAND_MOTION:
				ClientMotionHandler(iClientH, pData);
				break;

			case MSGID_COMMAND_CHATMSG:
				ChatMsgHandler(iClientH, pData, dwMsgSize);
				break;

			case MSGID_REQUEST_CREATENEWGUILD:
				RequestCreateNewGuildHandler(iClientH, pData, dwMsgSize);
				break;

			case MSGID_REQUEST_DISBANDGUILD:
				RequestDisbandGuildHandler(iClientH, pData, dwMsgSize);
				break;

			case MSGID_REQUEST_FIGHTZONE_RESERVE:
				FightzoneReserveHandler(iClientH, pData, dwMsgSize);
				break;

			case MSGID_LEVELUPSETTINGS:
				LevelUpSettingsHandler(iClientH, pData, dwMsgSize);
				break;

			case MSGID_STATECHANGEPOINT:
				StateChangeHandler(iClientH, pData, dwMsgSize);
				break;

			case MSGID_REQUEST_HELDENIAN_TP_LIST:	// 0x0EA03206
				RequestHeldenianTeleportList(iClientH, pData, dwMsgSize);
				break;
			case MSGID_REQUEST_HELDENIAN_TP:		// 0x0EA03208
				RequestHeldenianTeleportNow(iClientH, pData, dwMsgSize);
				break;

			case MSGID_REQUEST_HELDENIAN_SCROLL:
				RequestHeldenianScroll(iClientH, pData, dwMsgSize);
				break;

			case MSGID_REQUEST_TELEPORT_LIST:
				//Magn0S:: Changed
				RequestTeleportListHandler(iClientH, pData, dwMsgSize);
				break;

			case MSGID_REQUEST_CHARGED_TELEPORT:
				RequestChargedTeleportHandler(iClientH, pData, dwMsgSize);
				break;
				
			//Magn0S:: Quest List
			case DEF_MSGID_REQUEST_QUEST_LIST:
				RequestQuestList(iClientH, pData, dwMsgSize);
				break;

			// Centuu: "Do you really wanna change city?"
			case DEF_REQUEST_CHANGECITY_YES:
				PlayerOrder_ChangeCity(iClientH, true);
				break;
			

			default:
				if (m_pClientList[iClientH] != 0) {
					wsprintf(m_msgBuff, "Unknown message received from %d! (0x%.8X) Delete Client", iClientH ,*dwpMsgID);
					PutLogList(m_msgBuff);
					DeleteClient(iClientH, true, true); // v1.4
				}
				else
				{
					wsprintf(m_msgBuff, "Unknown message received! (0x%.8X)", *dwpMsgID);
					PutLogList(m_msgBuff);
				}
				break;
			}	
			break;

		
		}
	}

}


bool CGame::bPutMsgQuene(char cFrom, char * pData, UINT32 dwMsgSize, int iIndex, char cKey)
{
	
	// Â¸ÃžÂ½ÃƒÃÃ¶ Ã…Â¥Â°Â¡ Â´Ã™ ÃƒÂ¡Â´Ã™Â¸Ã© Â¿Â¡Â·Â¯ 
	if (m_pMsgQuene[m_iQueneTail] != 0) return false;

	// Â¸ÃžÂ¸Ã°Â¸Â® Ã‡Ã’Â´Ã§Ã€ÃŒ Ã€ÃŸÂ¸Ã¸ÂµÃ‡Â¾Ã®ÂµÂµ Â¿Â¡Â·Â¯ 
	m_pMsgQuene[m_iQueneTail] = new class CMsg;
	if (m_pMsgQuene[m_iQueneTail] == 0) return false;

	if (m_pMsgQuene[m_iQueneTail]->bPut(cFrom, pData, dwMsgSize, iIndex, cKey) == false) return false;

	m_iQueneTail++;
	if (m_iQueneTail >= DEF_MSGQUENESIZE) m_iQueneTail = 0;

	

	return true;
}

void CGame::AdminOrder_SetObserverMode(int iClientH)
{

	if (m_pClientList[iClientH] == 0) return;

	if (m_pClientList[iClientH]->m_bIsObserverMode == true) {
		// Â»Ãµ Ã€Â§Ã„Â¡Â¿Â¡ Ã‡Â¥Â½ÃƒÃ‡Ã‘Â´Ã™. 
		m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetOwner(iClientH, DEF_OWNERTYPE_PLAYER, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY);
		SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_LOG, DEF_MSGTYPE_CONFIRM, 0, 0, 0);
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_OBSERVERMODE, 0, 0, 0, 0);
		m_pClientList[iClientH]->m_bIsObserverMode = false;
	}
	else {
		// Ã‡Ã¶Ã€Ã§ Ã€Â§Ã„Â¡Â¿Â¡Â¼Â­ ÃÃ¶Â¿Ã®Â´Ã™. Â¾Ã¸Â¾Ã®ÃÃ¸ Ã‡Ã Ã€Â§Â¸Â¦ Â¾Ã‹Â·ÃÃÃ Â¾ÃŸ Ã‡Ã‘Â´Ã™. Â¾Ã†ÃÃ· Â¹ÃŒÂ±Â¸Ã‡Ã¶
		m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->ClearOwner(1, iClientH, DEF_OWNERTYPE_PLAYER, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY);
		SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_LOG, DEF_MSGTYPE_REJECT, 0, 0, 0);

		SendNotifyMsg(0, iClientH, DEF_NOTIFY_OBSERVERMODE, 1, 0, 0, 0);
		m_pClientList[iClientH]->m_bIsObserverMode = true;
	}
}


bool CGame::bGetMsgQuene(char * pFrom, char * pData, UINT32 * pMsgSize, int * pIndex, char * pKey)
{
	
	// Â°Â¡ÃÂ®Â°Â¥ Â¸ÃžÂ½ÃƒÃÃ¶Â°Â¡ Â¾Ã¸Ã€Â¸Â¸Ã© Â¿Â¡Â·Â¯  
	if (m_pMsgQuene[m_iQueneHead] == 0) return false;

	m_pMsgQuene[m_iQueneHead]->Get(pFrom, pData, pMsgSize, pIndex, pKey);

	delete m_pMsgQuene[m_iQueneHead];
	m_pMsgQuene[m_iQueneHead] = 0;

	m_iQueneHead++;
	if (m_iQueneHead >= DEF_MSGQUENESIZE) m_iQueneHead = 0;


	return true;
}


void CGame::ClientCommonHandler(int iClientH, char * pData)
{
 UINT16 * wp, wCommand;
 short * sp, sX, sY; 
 int   * ip, iV1, iV2, iV3, iV4;
 char  * cp, cDir, * pString;
	
	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
	if (m_pClientList[iClientH]->m_bIsKilled == true) return;

	wp = (UINT16 *)(pData + DEF_INDEX2_MSGTYPE);
	wCommand = *wp;

	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

	sp = (short *)cp;
	sX = *sp;
	cp += 2;

	sp = (short *)cp;
	sY = *sp;
	cp += 2;

	cDir = *cp;
	cp++;
 
	ip = (int *)cp;
	iV1 = *ip;
	cp += 4;

	ip = (int *)cp;
	iV2 = *ip;
	cp += 4;

	ip = (int *)cp;
	iV3 = *ip;
	cp += 4;

	pString = cp;
	cp += 30;
	
	ip = (int *)cp;
	iV4 = *ip;
	cp += 4;
	
	switch (wCommand) {

	//ZeroEoyPnk - NewMessage
	case DEF_RESQUEST_PARTYHP:
		RefreshPartyStatus(iClientH);
		RefreshPartyCoords(iClientH);
		break;

	// Crafting
	case DEF_COMMONTYPE_CRAFTITEM:
		ReqCreateCraftingHandler(iClientH, pData);
		break;

	case DEF_COMMONTYPE_REQDKSET: // MORLA 2.5 - DK Set
		GetDkSet(iClientH);
		break;

	// New 15/05/2004
	case DEF_COMMONTYPE_REQ_CREATESLATE:
		ReqCreateSlateHandler(iClientH, pData);
		break;

// 2.06 - by KLKS
	case DEF_COMMONTYPE_REQ_CHANGEPLAYMODE:
		RequestChangePlayMode(iClientH);
		break;
//

	case DEF_COMMONTYPE_SETGUILDTELEPORTLOC:
		RequestSetGuildTeleportLocHandler(iClientH, iV1, iV2, m_pClientList[iClientH]->m_iGuildGUID, "middleland");
		break;

	case DEF_COMMONTYPE_SETGUILDCONSTRUCTLOC:
		RequestSetGuildConstructLocHandler(iClientH, iV1, iV2, m_pClientList[iClientH]->m_iGuildGUID, pString);
		break;
	
	case DEF_COMMONTYPE_GUILDTELEPORT:
		RequestGuildTeleportHandler(iClientH);
		break;
	
	case DEF_COMMONTYPE_SUMMONWARUNIT:
		RequestSummonWarUnitHandler(iClientH, sX, sY, iV1, iV2, iV3);
		break;
	
	case DEF_COMMONTYPE_REQUEST_HELP:
		RequestHelpHandler(iClientH);
		break;
	
	case DEF_COMMONTYPE_REQUEST_MAPSTATUS:
		MapStatusHandler(iClientH, iV1, pString);
		break;

	case DEF_COMMONTYPE_REQUEST_SELECTCRUSADEDUTY:
		SelectCrusadeDutyHandler(iClientH, iV1);
		break;
	
	case DEF_COMMONTYPE_REQUEST_CANCELQUEST:
		CancelQuestHandler(iClientH, iV1);
		break;

	case DEF_COMMONTYPE_REQUEST_QUESTPRIZE:
		RequestQuestPrize(iClientH, iV1);
		break;
	
	case DEF_COMMONTYPE_REQUEST_ACTIVATESPECABLTY:
		ActivateSpecialAbilityHandler(iClientH);
		break;
	
	case DEF_COMMONTYPE_REQUEST_JOINPARTY:
		JoinPartyHandler(iClientH, iV1, pString);
		break;
	
	case DEF_COMMONTYPE_GETMAGICABILITY:
		GetMagicAbilityHandler(iClientH);
		break;

	case DEF_COMMONTYPE_BUILDITEM:
		BuildItemHandler(iClientH, pData);
		break;
	
	case DEF_COMMONTYPE_QUESTACCEPTED:
		QuestAcceptedHandler(iClientH);
		break;
	
	case DEF_COMMONTYPE_CANCELEXCHANGEITEM:
		CancelExchangeItem(iClientH);
		break;
	
	case DEF_COMMONTYPE_CONFIRMEXCHANGEITEM:
		ConfirmExchangeItem(iClientH);
		break;
	
	case DEF_COMMONTYPE_SETEXCHANGEITEM:
		SetExchangeItem(iClientH, iV1, iV2);
		break;
	
	case DEF_COMMONTYPE_REQ_GETHEROMANTLE:
		GetHeroMantleHandler(iClientH, iV1, pString);
		break;
	
	case DEF_COMMONTYPE_REQ_GETOCCUPYFLAG:
		GetOccupyFlagHandler(iClientH);
		break;

	case DEF_COMMONTYPE_REQ_SETDOWNSKILLINDEX:
		SetDownSkillIndexHandler(iClientH, iV1);
		break;

	case DEF_COMMONTYPE_TALKTONPC:
		NpcTalkHandler(iClientH, iV1, iV2);
		break;
	
	case DEF_COMMONTYPE_REQ_CREATEPOTION:
		ReqCreatePotionHandler(iClientH, pData);
		break;
	
	case DEF_COMMONTYPE_REQ_GETFISHTHISTIME:
		ReqGetFishThisTimeHandler(iClientH);
		break;
	
	case DEF_COMMONTYPE_REQ_REPAIRITEMCONFIRM:
		ReqRepairItemCofirmHandler(iClientH, iV1, pString);
		break;

	//50Cent - Repair All
    case DEF_COMMONTYPE_REQ_REPAIRALL:
        RequestRepairAllItemsHandler(iClientH);
        break;
    case DEF_COMMONTYPE_REQ_REPAIRALLDELETE:
        RequestRepairAllItemsDeleteHandler(iClientH, iV1);
        break;
    case DEF_COMMONTYPE_REQ_REPAIRALLCONFIRM:
        RequestRepairAllItemsConfirmHandler(iClientH);
        break;
	
	case DEF_COMMONTYPE_REQ_REPAIRITEM:
		ReqRepairItemHandler(iClientH, iV1, iV2, pString);
		break;
	
	case DEF_COMMONTYPE_REQ_SELLITEMCONFIRM:
		ReqSellItemConfirmHandler(iClientH, iV1, iV2, pString);
		break;
	
	case DEF_COMMONTYPE_REQ_SELLITEM:
		ReqSellItemHandler(iClientH, iV1, iV2, iV3, pString);
		break;
	
	case DEF_COMMONTYPE_REQ_USESKILL:
		UseSkillHandler(iClientH, iV1, iV2, iV3);
		break;
	
	case DEF_COMMONTYPE_REQ_USEITEM:
		UseItemHandler(iClientH, iV1, iV2, iV3, iV4);
		break;
	
	case DEF_COMMONTYPE_REQ_GETREWARDMONEY:
		GetRewardMoneyHandler(iClientH);
		break;

	case DEF_COMMONTYPE_ITEMDROP:
		DropItemHandler(iClientH, iV1, iV2, pString, true);
		break;
	
	case DEF_COMMONTYPE_EQUIPITEM:
		bEquipItemHandler(iClientH, iV1);
		break;

	case DEF_COMMONTYPE_REQ_PURCHASEITEM:
		RequestPurchaseItemHandler(iClientH, pString, iV1);
		break;

	case DEF_COMMONTYPE_REQ_PURCHASEITEM2:
		RequestPurchaseItemHandler2(iClientH, pString, iV1, iV2);
		break;

	case DEF_COMMONTYPE_REQ_STUDYMAGIC:
		RequestStudyMagicHandler(iClientH, pString);
		break;

	case DEF_COMMONTYPE_REQ_TRAINSKILL:
		break;

	case DEF_COMMONTYPE_GIVEITEMTOCHAR:
		GiveItemHandler(iClientH, cDir, iV1, iV2, iV3, iV4, pString);
		break;

	case DEF_COMMONTYPE_EXCHANGEITEMTOCHAR:
		ExchangeItemHandler(iClientH, cDir, iV1, iV2, iV3, iV4, pString);
		break;

	case DEF_COMMONTYPE_JOINGUILDAPPROVE:
		JoinGuildApproveHandler(iClientH, pString);
		break;
	
	case DEF_COMMONTYPE_JOINGUILDREJECT:
		JoinGuildRejectHandler(iClientH, pString);
		break;

	case DEF_COMMONTYPE_DISMISSGUILDAPPROVE:
		DismissGuildApproveHandler(iClientH, pString);
		break;

	case DEF_COMMONTYPE_DISMISSGUILDREJECT:
		DismissGuildRejectHandler(iClientH, pString);
		break;

	case DEF_COMMONTYPE_RELEASEITEM:
		ReleaseItemHandler(iClientH, iV1, true);
		break;

	case DEF_COMMONTYPE_TOGGLECOMBATMODE:
		ToggleCombatModeHandler(iClientH);
		break;

	case DEF_COMMONTYPE_MAGIC:
		PlayerMagicHandler(iClientH, iV1, iV2, (iV3 - 100));
		break;

	case DEF_COMMONTYPE_TOGGLESAFEATTACKMODE:
		ToggleSafeAttackModeHandler(iClientH);
		break;

	case DEF_COMMONTYPE_REQ_GETOCCUPYFIGHTZONETICKET:
		GetFightzoneTicketHandler(iClientH);
		break;

	// Upgrade Item
	case DEF_COMMONTYPE_UPGRADEITEM:
		RequestItemUpgradeHandler(iClientH,iV1);
		break;

	case DEF_COMMONTYPE_ENCHANTITEM:
		RequestItemEnchantHandler(iClientH, iV1, iV2);
		break;

	case DEF_COMMONTYPE_REQGUILDNAME:
		RequestGuildNameHandler(iClientH, iV1, iV2);
		break;

	case DEF_COMMONTYPE_REQGUILDRANK:
		RequestGuildMemberRank(iClientH, pString, iV2);
		break;

	case DEF_COMMONTYPE_REQRANGO:
		break;

	//50Cent - HP Bar
	case DEF_COMMONTYPE_REQ_GETNPCHP:
		if ((iV1 - 10000 <= 0) || (iV1 - 10000 >= DEF_MAXNPCS)) return;
        if (m_pNpcList[iV1 - 10000] == 0) return;
		if(m_pNpcList[iV1 - 10000]->m_iHP > m_pNpcList[iV1 - 10000]->m_iMaxHP) m_pNpcList[iV1 - 10000]->m_iMaxHP = m_pNpcList[iV1 - 10000]->m_iHP;
		SendNotifyMsg(0, iClientH, DEF_SEND_NPCHP, m_pNpcList[iV1 - 10000]->m_iHP, m_pNpcList[iV1 - 10000]->m_iMaxHP, 0, 0);
		break;

	case DEF_COMMONTYPE_REQUEST_ACCEPTJOINPARTY:
		RequestAcceptJoinPartyHandler(iClientH, iV1);
		break;

	case DEF_COMMONTYPE_REQARENASTATUS:
		RequestArenaStatus(iClientH);
		break;

	//Magn0S:: Set Function to receive orders from Client
	case DEF_COMMONTYPE_CLIENTMSG:
		ReceivedClientOrder(iClientH, iV1, iV2, iV3, pString);
		break;

	default:
		wsprintf(G_cTxt,"Unknown message received! (0x%.8X)",wCommand);
		PutLogList(G_cTxt);
		break;
	}
}


/////////////////////////////////////////////////////////////////////////////////////
//  int CGame::iClientMotion_Stop_Handler(int iClientH, short sX, short sY, char cDir)
//  description			:: checks if player is stopped
//  last updated		:: October 29, 2004; 6:46 PM; Hypnotoad
//	return value		:: int
/////////////////////////////////////////////////////////////////////////////////////
int CGame::iClientMotion_Stop_Handler(int iClientH, short sX, short sY, char cDir)
{
 char cData[100];
 UINT32 * dwp;
 UINT16  * wp;
 int     iRet;
 short   sOwnerH;
 char    cOwnerType;
	
	if (m_pClientList[iClientH] == 0) return 0;
	if ((cDir <= 0) || (cDir > 8))       return 0;
	if (m_pClientList[iClientH]->m_bIsKilled == true) return 0;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return 0;
	
	if ((sX != m_pClientList[iClientH]->m_sX) || (sY != m_pClientList[iClientH]->m_sY)) return 2;

	if (m_pClientList[iClientH]->m_bSkillUsingStatus[19] == true) {
		m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, sX, sY);		
		if (sOwnerH != 0) {
			DeleteClient(iClientH, true, true);
			return 0;
		}
	}

	ClearSkillUsingStatus(iClientH);

	int iStX, iStY;
	if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex] != 0) {
		iStX = m_pClientList[iClientH]->m_sX / 20;
		iStY = m_pClientList[iClientH]->m_sY / 20;
		m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_stTempSectorInfo[iStX][iStY].iPlayerActivity++;

		switch (m_pClientList[iClientH]->m_cSide) {
		case 0: m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_stTempSectorInfo[iStX][iStY].iNeutralActivity++; break;
		case 1: m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_stTempSectorInfo[iStX][iStY].iAresdenActivity++; break;
		case 2: m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_stTempSectorInfo[iStX][iStY].iElvineActivity++;  break;
		}
	}

	m_pClientList[iClientH]->m_cDir = cDir;
	
	m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->ClearOwner(0, iClientH, DEF_OWNERTYPE_PLAYER, sX, sY);
	m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetOwner(iClientH, DEF_OWNERTYPE_PLAYER, sX, sY);

	dwp  = (UINT32 *)(cData + DEF_INDEX4_MSGID);
	*dwp = MSGID_RESPONSE_MOTION;
	wp   = (UINT16 *)(cData + DEF_INDEX2_MSGTYPE);
	*wp  = DEF_OBJECTMOTION_CONFIRM;

	iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 6);
	switch (iRet) {
	case DEF_XSOCKEVENT_QUENEFULL:
	case DEF_XSOCKEVENT_SOCKETERROR:
	case DEF_XSOCKEVENT_CRITICALERROR:
	case DEF_XSOCKEVENT_SOCKETCLOSED:
		DeleteClient(iClientH, true, true);
		return 0;
	}

	return 1;
}

void CGame::GlobalStartGladiatorArena()
{
	int i;

	bDeathmatch = true;
	dwEventFinishTime = timeGetTime() + m_sDeathmatchFinish * 60 * 1000;
	for (i = 0; i < DEF_MAXCLIENTS; i++) {
		if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_bIsInitComplete == true)) {
			m_pClientList[i]->m_iDGKills = 0;
			m_pClientList[i]->m_iDGDeaths = 0;
			SendNotifyMsg(0, i, DEF_NOTIFY_DEATHMATCHSTART, 0, 0, 0, 0);
			SendNotifyMsg(0, i, DEF_NOTIFY_EVENTUPDATER, (int)bDeathmatch, (int)m_bIsCrusadeMode, (int)m_bIsApocalypseMode, 0, (int)m_bIsHeldenianMode, (int)m_bIsCTFEvent);
		}
	}
	
}

void CGame::GlobalEndGladiatorArena()
{
	int i;
	bDeathmatch = false;
	dwEventFinishTime = 0;
	for (i = 0; i < DEF_MAXCLIENTS; i++) {
		if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_bIsInitComplete == true)) {
			SendNotifyMsg(0, i, DEF_NOTIFY_DEATHMATCHEND, 0, 0, 0, 0);
			SendNotifyMsg(0, i, DEF_NOTIFY_EVENTUPDATER, (int)bDeathmatch, (int)m_bIsCrusadeMode, (int)m_bIsApocalypseMode, 0, (int)m_bIsHeldenianMode, (int)m_bIsCTFEvent);
			if (memcmp(m_pMapList[m_pClientList[i]->m_cMapIndex]->m_cName, "fightzone1", 10) == 0) {
				RequestTeleportHandler(i, "0   ");
			}
		}
	}
}

void CGame::ManualStartGladiatorArena(int iClientH, char* pData, UINT32 dwMsgSize)
{
	char cBuff[256] = {}, seps[] = "= \t\n";

	memcpy(cBuff, pData, dwMsgSize);

	if (bDeathmatch == true) return;
	if (m_pClientList[iClientH]->m_iAdminUserLevel < 3) {
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
		return;
	}

	GlobalStartGladiatorArena();

	wsprintf(G_cTxt, "Admin Order(%s): Start Gladiator Arena.", m_pClientList[iClientH]->m_cCharName);
	PutAdminLogFileList(G_cTxt);
	PutLogList(G_cTxt);
	PutLogEventFileList(G_cTxt);
}

void CGame::ManualEndGladiatorArena(int iClientH, char* pData, UINT32 dwMsgSize)
{
	char cBuff[256], seps[] = "= \t\n";
	ZeroMemory(cBuff, sizeof(cBuff));
	memcpy(cBuff, pData, dwMsgSize);

	if (bDeathmatch == false) return;
	if (m_pClientList[iClientH]->m_iAdminUserLevel < 3) {
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
		return;
	}

	GlobalEndGladiatorArena();

	wsprintf(G_cTxt, "Admin Order(%s): Closing Gladiator.", m_pClientList[iClientH]->m_cCharName);
	PutAdminLogFileList(G_cTxt);
	PutLogList(G_cTxt);
	PutLogEventFileList(G_cTxt);
}

void CGame::ClientKilledHandler(int iClientH, int iAttackerH, char cAttackerType, short sDamage)
{
 char  * cp, cAttackerName[21], cData[120];
 short sAttackerWeapon;
 int   * ip, i, iExH;
 bool  bIsSAattacked = false;
 int Killedi;
	
	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
 	if (m_pClientList[iClientH]->m_bIsKilled == true) return;

	// »çÅõÀåÀÎ °æ¿ì ½Ã°£À» ÀÔ·ÂÇÑ´Ù.
	// 2002-7-4 »çÅõÀåÀÇ °¹¼ö¸¦ ´Ã¸± ¼ö ÀÖµµ·Ï 

	m_pClientList[iClientH]->m_bIsKilled = true;
	// HP´Â 0ÀÌ´Ù.
	m_pClientList[iClientH]->m_iHP = 0;
	m_pClientList[iClientH]->m_iDeaths++; // MORLA 2.2 - Le suma una muerte al pj
	SendNotifyMsg(0, iClientH, DEF_NOTIFY_REPDGDEATHS, 0, m_pClientList[iClientH]->m_iDeaths, 0, 0);

	if (m_pClientList[iClientH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_HOLDOBJECT ] != 0)
	{
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_MAGICEFFECTOFF, DEF_MAGICTYPE_HOLDOBJECT, m_pClientList[iClientH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_HOLDOBJECT ], 0, 0);
		m_pClientList[iClientH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_HOLDOBJECT ] = 0;
		bRemoveFromDelayEventList(iClientH, DEF_OWNERTYPE_PLAYER, DEF_MAGICTYPE_HOLDOBJECT);
	}

	if ( m_pClientList[iClientH]->m_bIsPoisoned ) 
	{
		m_pClientList[iClientH]->m_bIsPoisoned = false;
		SetPoisonFlag(iClientH, DEF_OWNERTYPE_PLAYER, false);
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_MAGICEFFECTOFF, DEF_MAGICTYPE_POISON, 0, 0, 0);
	}

	// Snoopy: Remove all magic effects and flags
	for (i = 0; i < DEF_MAXMAGICEFFECTS; i++)
		m_pClientList[iClientH]->m_cMagicEffectStatus[i] = 0;

	SetDefenseShieldFlag(iClientH, DEF_OWNERTYPE_PLAYER, false);
	SetMagicProtectionFlag(iClientH, DEF_OWNERTYPE_PLAYER, false);
	SetProtectionFromArrowFlag(iClientH, DEF_OWNERTYPE_PLAYER, false);
	SetIllusionMovementFlag(iClientH, DEF_OWNERTYPE_PLAYER, false);
	SetIllusionFlag(iClientH, DEF_OWNERTYPE_PLAYER, false);
	SetInhibitionCastingFlag(iClientH, DEF_OWNERTYPE_PLAYER, false);
	SetPoisonFlag(iClientH, DEF_OWNERTYPE_PLAYER, false);
	SetIceFlag(iClientH, DEF_OWNERTYPE_PLAYER, false);
	SetBerserkFlag(iClientH, DEF_OWNERTYPE_PLAYER, false);
	SetInvisibilityFlag(iClientH, DEF_OWNERTYPE_PLAYER, false);
	SetSlateFlag(iClientH, DEF_NOTIFY_SLATECLEAR, false);

	if (m_pClientList[iClientH]->m_cExchangeMode != 0) {
		iExH = m_pClientList[iClientH]->m_iExchangeH;
		_ClearExchangeStatus(iExH);
		_ClearExchangeStatus(iClientH);
	}
	// ÇöÀç ÀÌ NPC¸¦ °ø°Ý´ë»óÀ¸·Î »ï°íÀÖ´Â °´Ã¼µéÀ» ÇØÁ¦ÇÑ´Ù.
	RemoveFromTarget(iClientH, DEF_OWNERTYPE_PLAYER, 0, iAttackerH, cAttackerType);
	
	ZeroMemory(cAttackerName, sizeof(cAttackerName));
	switch (cAttackerType) {
	case DEF_OWNERTYPE_PLAYER_INDIRECT:
	case DEF_OWNERTYPE_PLAYER:
		if (m_pClientList[iAttackerH] != 0)
			memcpy(cAttackerName, m_pClientList[iAttackerH]->m_cCharName, 10);
		break;
	case DEF_OWNERTYPE_NPC:
		if (m_pNpcList[iAttackerH] != 0)
			memcpy(cAttackerName, m_pNpcList[iAttackerH]->m_cNpcName, 20);
		break ;
	}
	SendNotifyMsg(0, iClientH, DEF_NOTIFY_HP, 0, 0, 0, 0);
	SendNotifyMsg(0, iClientH, DEF_NOTIFY_KILLED, 0, 0, 0, 0);
	// ´Ù¸¥ Å¬¶óÀÌ¾ðÆ®¿¡°Ô Á×´Â µ¿ÀÛ Àü¼Û.
	if (cAttackerType == DEF_OWNERTYPE_PLAYER) {
		sAttackerWeapon = ((m_pClientList[iAttackerH]->m_sAppr2 & 0x0FF0) >> 4);	
	}
	else sAttackerWeapon = 1;
	SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTDYING, sDamage, sAttackerWeapon, 0);
	m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->ClearOwner(12, iClientH, DEF_OWNERTYPE_PLAYER, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY);
	m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetDeadOwner(iClientH, DEF_OWNERTYPE_PLAYER, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY);
	if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cType == DEF_MAPTYPE_NOPENALTY_NOREWARD) return;
	if (   (m_bHeldenianWarInitiated == true) 
		&& (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsHeldenianMap == true)) 
	{	if (m_pClientList[iClientH]->m_cSide == 1) 
		{	m_iHeldenianAresdenDead++;
		}else if (m_pClientList[iClientH]->m_cSide == 2) 
		{	m_iHeldenianElvineDead++;
		}
		UpdateHeldenianStatus(-1);
	}

	if (cAttackerType == DEF_OWNERTYPE_PLAYER) {
		// v1.432
		// Æ¯¼ö ´É·ÂÀÌ ÀÖ´Â ¹«±â·Î °ø°ÝÀ» ´çÇß´Ù.
		switch (m_pClientList[iAttackerH]->m_iSpecialAbilityType) {
		case 1:
		case 2:
		case 3:
		case 5:
			bIsSAattacked = true;
			break;
		}
		
		if (iAttackerH == iClientH) return; // ÀÚÆøÀÌ´Ù.
		// ÇÃ·¹ÀÌ¾î°¡ ÇÃ·¹ÀÌ¾î¸¦ Á×¿´´Ù¸é PKÀÎÁö ÀüÅõÁß ½Â¸®ÀÎÁö¸¦ ÆÇº°ÇÏ¿© °æÇèÄ¡¸¦ ¿Ã¸°´Ù. 

		
			if (m_pClientList[iAttackerH] != 0 && m_pClientList[iClientH] != 0)
			{
				if (m_pClientList[iAttackerH]->IsInMap("team") && m_pClientList[iClientH]->IsInMap("team"))
				{
					c_team->Kill(iAttackerH, iClientH);
					return;
				}
			}
		

		// Capture the Flag
		if (m_bIsCTFEvent && (m_pClientList[iClientH]->m_iStatus & 0x80000) != 0) {
			m_pClientList[iClientH]->m_iStatus ^= 0x80000;

			SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);

			switch (m_pClientList[iClientH]->m_cSide) {
			case 1:
				m_iCTFEventFlag[1] = iAddDynamicObjectList(0, 0, DEF_DYNAMICOBJECT_ELVINEFLAG2, m_iElvineMapIndex, 151, 132, 0, 0);
				break;
			case 2:
				m_iCTFEventFlag[0] = iAddDynamicObjectList(0, 0, DEF_DYNAMICOBJECT_ARESDENFLAG2, m_iAresdenMapIndex, 151, 128, 0, 0);
				break;
			}

			m_iCTFEventFlagHolder[m_pClientList[iClientH]->m_cSide - 1] = -1;

			for (i = 1; i < DEF_MAXCLIENTS; i++)
				if (m_pClientList[i] != 0 && m_pClientList[i]->m_cSide != 0) SendNotifyMsg(0, i, DEF_NOTIFY_EVENT, 0, 8 + m_pClientList[iClientH]->m_cSide - 1, 0, 0);
		}

		//Magn0S:: Arena Gladiator
		if ((memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, "fightzone1", 10) == 0) && (iAttackerH != iClientH) && (cAttackerType != DEF_OWNERTYPE_NPC))
		{
			m_pClientList[iClientH]->m_iDGDeaths++;
			m_pClientList[iAttackerH]->m_iDGKills++;

			m_pClientList[iAttackerH]->m_iEnemyKillCount += 5;
			if (m_pClientList[iAttackerH]->m_iEnemyKillCount > m_pClientList[iAttackerH]->m_iMaxEK)
			{
				m_pClientList[iAttackerH]->m_iMaxEK = m_pClientList[iAttackerH]->m_iEnemyKillCount;
			}

			SendNotifyMsg(0, iAttackerH, DEF_NOTIFY_ENEMYKILLS, m_pClientList[iAttackerH]->m_iEnemyKillCount, m_pClientList[iAttackerH]->m_iMaxEK, 0, 0);

			calcularTop15HB(iAttackerH);

			RequestArenaStatus(iClientH, true);
			RequestArenaStatus(iAttackerH, true);

			return;
		}
		else 
		{
			//    PK   
			if (m_pClientList[iClientH]->m_cSide == 0) {
				// 
				if (m_pClientList[iClientH]->m_iPKCount == 0) {

					//  PK
					ApplyPKpenalty(iAttackerH, iClientH);
				}
				else {
					//  PK  
					PK_KillRewardHandler(iAttackerH, iClientH);
				}
			}
			else {
				if (m_pClientList[iClientH]->m_iGuildRank == -1) {
					if (m_pClientList[iAttackerH]->m_cSide == 0) {
						if (m_pClientList[iClientH]->m_iPKCount == 0) {
							//PK
							ApplyPKpenalty(iAttackerH, iClientH);
						}
						else {
						}
					}
					else {
						if (m_pClientList[iClientH]->m_cSide == m_pClientList[iAttackerH]->m_cSide) {

							if (m_pClientList[iClientH]->m_iPKCount == 0) {
								ApplyPKpenalty(iAttackerH, iClientH);
							}
							else {
								PK_KillRewardHandler(iAttackerH, iClientH);
							}
						}
						else {
							EnemyKillRewardHandler(iAttackerH, iClientH);
						}
					}
				}
				else {
					if (m_pClientList[iAttackerH]->m_cSide == 0) {
						if (m_pClientList[iClientH]->m_iPKCount == 0) {
							ApplyPKpenalty(iAttackerH, iClientH);
						}
						else {
						}
					}
					else {
						if (m_pClientList[iClientH]->m_cSide == m_pClientList[iAttackerH]->m_cSide) {

							if (m_pClientList[iClientH]->m_iPKCount == 0) {
								ApplyPKpenalty(iAttackerH, iClientH);
							}
							else {
								PK_KillRewardHandler(iAttackerH, iClientH);
							}
						}
						else {
							EnemyKillRewardHandler(iAttackerH, iClientH);
						}
					}
				}
			}

			if (m_pClientList[iClientH]->m_iPKCount == 0) {
				// Innocent
				if (m_pClientList[iAttackerH]->m_cSide == 0) {
					if (m_pClientList[iClientH]->m_iAdminUserLevel < 1) m_pClientList[iClientH]->m_iExp -= iDice(1, 10000);
					if (m_pClientList[iClientH]->m_iExp < 0) m_pClientList[iClientH]->m_iExp = 0;
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_EXP, 0, 0, 0, 0);
				}
				else {
					if (m_pClientList[iClientH]->m_cSide == m_pClientList[iAttackerH]->m_cSide) {
						if (m_pClientList[iClientH]->m_iAdminUserLevel < 1) m_pClientList[iClientH]->m_iExp -= iDice(1, 10000);
						if (m_pClientList[iClientH]->m_iExp < 0) m_pClientList[iClientH]->m_iExp = 0;
						SendNotifyMsg(0, iClientH, DEF_NOTIFY_EXP, 0, 0, 0, 0);
					}
					else {

						ApplyCombatKilledPenalty(iClientH, 2, bIsSAattacked);
					}
				}
			}
			else if ((m_pClientList[iClientH]->m_iPKCount >= 1) && (m_pClientList[iClientH]->m_iPKCount <= 3)) {
				// Criminal 
				ApplyCombatKilledPenalty(iClientH, 3, bIsSAattacked);
			}
			else if ((m_pClientList[iClientH]->m_iPKCount >= 4) && (m_pClientList[iClientH]->m_iPKCount <= 11)) {
				// Murderer 
				ApplyCombatKilledPenalty(iClientH, 6, bIsSAattacked);
			}
			else if ((m_pClientList[iClientH]->m_iPKCount >= 12)) {
				// Slaughterer 
				ApplyCombatKilledPenalty(iClientH, 12, bIsSAattacked);
			}
			char cTxt[128];
			ZeroMemory(cTxt, sizeof(cTxt));
			wsprintf(cTxt, "%s killed %s", m_pClientList[iAttackerH]->m_cCharName, m_pClientList[iClientH]->m_cCharName);
			for (Killedi = 0; Killedi < DEF_MAXCLIENTS; Killedi++) {
				if (Killedi == iAttackerH) continue;
				if (m_pClientList[Killedi] != 0) {
					SendNotifyMsg(0, Killedi, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, cTxt);
				}
			}
			ZeroMemory(cTxt, sizeof(cTxt));
			wsprintf(cTxt, "%s(%s) killed %s(%s) in %s(%d,%d)", m_pClientList[iAttackerH]->m_cCharName, m_pClientList[iAttackerH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName, m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cMapName, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY);
			PutPvPLogFileList(cTxt); // Centu : log pvp
		}
	}
	else if (cAttackerType == DEF_OWNERTYPE_NPC) {

		_bPKLog(DEF_PKLOG_BYNPC,iClientH,0,cAttackerName) ;

		// ÇÃ·¹ÀÌ¾î°¡ ¸÷¿¡°Ô Á×¾ú´Ù. Èñ»ý´çÇÑ ÇÃ·¹ÀÌ¾îÀÇ µî±Þ¿¡ ¸Â´Â Æä³ÎÆ¼¸¦ ÃëÇÑ´Ù.
		if (m_pClientList[iClientH]->m_iPKCount > 0)
			ApplyCombatKilledPenalty(iClientH, m_pClientList[iClientH]->m_iPKCount, bIsSAattacked);
		
		// ¸¸¾à °ø°ÝÇÑ NPC°¡ ÀüÀï¿ë À¯´ÏÆ®¶ó¸é ÁöÈÖ°ü¿¡°Ô °Ç¼³ Æ÷ÀÎÆ® ºÎ°¡
		if (m_pNpcList[iAttackerH]->m_iGuildGUID != 0) {
						
			if (m_pNpcList[iAttackerH]->m_cSide != m_pClientList[iClientH]->m_cSide) {
				// ÀüÀï¿ë ±¸Á¶¹° È¤Àº À¯´ÏÆ®°¡ Àû ÇÃ·¹ÀÌ¾î¸¦ Á×¿´´Ù. ¹Ù·Î Åëº¸ÇÑ´Ù.
				// ÇöÀç ¼­¹ö¿¡ ÁöÈÖ°üÀÌ ÀÖ´Ù¸é °ð¹Ù·Î Åëº¸. ¾øÀ¸¸é ´Ù¸¥ ¼­¹ö·Î ¾Ë·ÁÁÜ.
				for (i = 1; i < DEF_MAXCLIENTS; i++)
				if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_iGuildGUID == m_pNpcList[iAttackerH]->m_iGuildGUID) &&
					(m_pClientList[i]->m_iCrusadeDuty == 3)) {
					m_pClientList[i]->m_iConstructionPoint += ((m_pClientList[iClientH]->m_iLevel) / 2);

					// ÁöÈÖ°ü¿¡°Ô ¹Ù·Î Åëº¸.
					SendNotifyMsg(0, i, DEF_NOTIFY_CONSTRUCTIONPOINT, m_pClientList[i]->m_iConstructionPoint, m_pClientList[i]->m_iWarContribution, 0, 0);
					return;
				}
				
				// ÇöÀç ¼­¹ö¿¡ ¾ø´Ù. ´Ù¸¥ ¼­¹öÀÇ ÁöÈÖ°ü¿¡°Ô ¾Ë·Á¾ß ÇÑ´Ù.
				ZeroMemory(cData, sizeof(cData));
				cp = (char *)cData;
				*cp = GSM_CONSTRUCTIONPOINT;
				cp++;
				ip = (int*)cp;
				*ip = m_pNpcList[iAttackerH]->m_iGuildGUID;
				cp += 4;
				ip = (int*)cp;
				*ip = ((m_pClientList[iClientH]->m_iLevel) / 2);
				cp += 4;
				bStockMsgToGateServer(cData, 9);
			}
		}
	}
	else if (cAttackerType == DEF_OWNERTYPE_PLAYER_INDIRECT) {
		_bPKLog(DEF_PKLOG_BYOTHER,iClientH,0,0) ;
		// ÇÃ·¹ÀÌ¾î°¡ Á×¾úÁö¸¸ °ø°ÝÀÚ°¡ °£Á¢ÀûÀÌ´Ù. ¾Æ¹«·± ¿µÇâÀÌ ¾ø´Ù.
		if (m_pClientList[iClientH]->m_iAdminUserLevel < 1) m_pClientList[iClientH]->m_iExp -= iDice(1, 10000);
		if (m_pClientList[iClientH]->m_iExp < 0) m_pClientList[iClientH]->m_iExp = 0;
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_EXP, 0, 0, 0, 0);
	}
	else if (cAttackerType == DEF_MAGICTYPE_CREATE_DYNAMIC) {
		EnemyKillRewardHandler(iAttackerH, iClientH);
	}
}

/*********************************************************************************************************************
**  int CGame::iDice(int iThrow, int iRange)																		**
**  description			:: produces a random number between the throw and range										**
**  last updated		:: November 20, 2004; 10:24 PM; Hypnotoad													**
**	return value		:: int																						**
**********************************************************************************************************************/
int CGame::iDice(int iThrow, int iRange)
{
 int i, iRet;

	if (iRange <= 0) return 0;
	iRet = 0;
	for (i = 1; i <= iThrow; i++) {
		iRet += (rand() % iRange) + 1;
	}
	return iRet;
}

void CGame::TimeManaPointsUp(int iClientH)
{
 int iMaxMP, iTotal;
 float dV1, dV2, dV3;

	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsKilled == true) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
	if (m_pClientList[iClientH]->m_iHungerStatus <= 0) return;
	if (m_pClientList[iClientH]->m_bSkillUsingStatus[19] == true) return;
	if ((m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bMapRegens == false) && (m_pClientList[iClientH]->m_iAdminUserLevel == 0)) {
		return;
	}

	iMaxMP = iGetMaxMP(iClientH);// v1.4
	if (m_pClientList[iClientH]->m_iMP < iMaxMP) {
		iTotal = iDice(1, (m_pClientList[iClientH]->m_iMag));
		if (m_pClientList[iClientH]->m_iAddMP != 0) {
			dV2 = (float)iTotal;
			dV3 = (float)m_pClientList[iClientH]->m_iAddMP;
			dV1 = (dV3 / 100.0f)*dV2;
			iTotal += (int)dV1;
		}

		m_pClientList[iClientH]->m_iMP += iTotal;
		
		if (m_pClientList[iClientH]->m_iMP > iMaxMP) 
			m_pClientList[iClientH]->m_iMP = iMaxMP;

		SendNotifyMsg(0, iClientH, DEF_NOTIFY_MP, 0, 0, 0, 0);
	}
}

// 05/29/2004 - Hypnotoad - fixed infinite sp bug
void CGame::TimeStaminarPointsUp(int iClientH)
{
 int iMaxSP, iTotal=0;
	float dV1, dV2, dV3;

	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsKilled == true) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
	if (m_pClientList[iClientH]->m_iHungerStatus <= 0) return;
	if (m_pClientList[iClientH]->m_bSkillUsingStatus[19] == true) return;
	if ((m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bMapRegens == false) && (m_pClientList[iClientH]->m_iAdminUserLevel == 0)) {
		return;
	}
	
	iMaxSP = iGetMaxSP(iClientH);
	if (m_pClientList[iClientH]->m_iSP < iMaxSP) {

		iTotal = iDice(1, (m_pClientList[iClientH]->m_iVit/3)); // Staminar Point
		if (m_pClientList[iClientH]->m_iAddSP != 0) {
			dV2 = (float)iTotal;
			dV3 = (float)m_pClientList[iClientH]->m_iAddSP;
			dV1 = (dV3 / 100.0f)*dV2;
			iTotal += (int)dV1;
		}

		m_pClientList[iClientH]->m_iSP += iTotal+75;
		if (m_pClientList[iClientH]->m_iSP > iMaxSP) 
			m_pClientList[iClientH]->m_iSP = iMaxSP;

		SendNotifyMsg(0, iClientH, DEF_NOTIFY_SP, 0, 0, 0, 0);
	}
}

void CGame::ToggleCombatModeHandler(int iClientH)
{
	short sAppr2;

	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
	if (m_pClientList[iClientH]->m_bIsKilled == true) return;
	if (m_pClientList[iClientH]->m_bSkillUsingStatus[19] == true) return;

	sAppr2 = (short)((m_pClientList[iClientH]->m_sAppr2 & 0xF000) >> 12);

	m_pClientList[iClientH]->m_bIsAttackModeChange = true; // v2.172


	if (sAppr2 == 0 && m_pClientList[iClientH]->m_bIsPolymorph == false) {
		m_pClientList[iClientH]->m_sAppr2 = (0xF000 | m_pClientList[iClientH]->m_sAppr2);
	}
	else {
		m_pClientList[iClientH]->m_sAppr2 = (0x0FFF & m_pClientList[iClientH]->m_sAppr2);
	}

	SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);

}

void CGame::OnGateSocketEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
 int   iRet;
	
	if (m_pGateSock == 0) return;

	iRet = m_pGateSock->iOnSocketEvent(wParam, lParam);

	switch (iRet) {
	case DEF_XSOCKEVENT_UNSENTDATASENDCOMPLETE:
		PutLogList("(!!!) Gate-socket connected!");
		SendMsgToGateServer(MSGID_REQUEST_REGISTERGAMESERVER, 0);
		m_iGateSockConnRetryTimes = 0; // centuu
		break;

	case DEF_XSOCKEVENT_CONNECTIONESTABLISH:
		// Â°Ã”Ã€ÃŒÃ†Â®Â¼Â­Â¹Ã¶Â·ÃŽÃ€Ã‡ Â¿Â¬Â°Ã¡Ã€ÃŒ Ã€ÃŒÂ·Ã§Â¾Ã® ÃÂ³Ã€Â¸Â¹Ã‡Â·ÃŽ Â°Ã”Ã€Ã“Â¼Â­Â¹Ã¶ ÂµÃ®Â·Ã Â¸ÃžÂ½ÃƒÃÃ¶Â¸Â¦ Ã€Ã¼Â¼Ã›Ã‡Ã‘Â´Ã™.
		// v1.41 gate-socketÃ€Â» Ã€Ã§Â¿Â¬Â°Ã¡Ã‡Ã’ Â¶Â§ Ã„Â«Â¿Ã®Ã†Ãƒ Ã‡ÃÂ´Ã‚ ÂºÂ¯Â¼Ã¶. Â¼Â­Â¹Ã¶ Ã€Ã›ÂµÂ¿ ÂµÂµÃÃŸ gate-socketÃ€ÃŒ Â¼Ã’Ã„ÃÂ¿Â¡Â·Â¯Â·ÃŽ Â²Ã·Â±Ã¢Â¸Ã© Â°Ã°Â¹Ã™Â·ÃŽ Ã€Ã§ÃÂ¢Â¼Ã“Ã€Â» 
		// Â½ÃƒÂµÂµÃ‡ÃÂ¸Ã§ Ã€ÃŒ Â°ÂªÃ€ÃŒ Ã€ÃÃÂ¤Ã„Â¡ Ã€ÃŒÂ»Ã³Ã€ÃŒ ÂµÃ‡Â¸Ã© Â¿Â¬Â°Ã¡ Â½Ã‡Ã†ÃÂ·ÃŽ Â°Â£ÃÃ–ÂµÃ‡Â¾Ã® Ã€ÃšÂµÂ¿ Â¼Â­Â¹Ã¶ Â¼Ã‹Â´Ã™Â¿Ã® Â¸Ã°ÂµÃ¥(4)Â·ÃŽ ÂµÃ©Â¾Ã®Â°Â£Â´Ã™. 
		m_iGateSockConnRetryTimes = 0;
		break;
	
	case DEF_XSOCKEVENT_READCOMPLETE:
		// Â¸ÃžÂ½ÃƒÃÃ¶Â°Â¡ Â¼Ã¶Â½Ã…ÂµÃ‡Â¾ÃºÂ´Ã™.
		OnGateRead();
		break;
	
	case DEF_XSOCKEVENT_BLOCK:
		break;
	
	case DEF_XSOCKEVENT_CONFIRMCODENOTMATCH:
		// ÃˆÂ®Ã€ÃŽÃ„ÃšÂµÃ¥Â°Â¡ Ã€ÃÃ„Â¡Ã‡ÃÃÃ¶ Â¾ÃŠÂ´Ã‚Â´Ã™.
	case DEF_XSOCKEVENT_MSGSIZETOOLARGE:
		// Â¼Ã¶Â½Ã…Ã‡Ã˜Â¾ÃŸ Ã‡Ã’ Â¸ÃžÂ½ÃƒÃÃ¶ Ã…Â©Â±Ã¢Â°Â¡ Â¹Ã¶Ã†Ã›ÂºÂ¸Â´Ã™ Ã…Â©Â´Ã™.	ÃÂ¾Â·Ã¡Ã‡Ã˜Â¾ÃŸÂ¸Â¸ Ã‡Ã‘Â´Ã™.
	case DEF_XSOCKEVENT_SOCKETERROR:
		// Â¼Ã’Ã„ÃÂ¿Â¡ Â¿Â¡Â·Â¯Â°Â¡ Â³ÂµÂ´Ã™.
	case DEF_XSOCKEVENT_SOCKETCLOSED:
		// Â¼Ã’Ã„ÃÃ€ÃŒ Â´ÃÃ‡Ã»Â´Ã™.
		delete m_pGateSock;
		m_pGateSock = 0;
		PutLogList("(!!!) Gate-socket connection lost!");
		m_bIsGateSockAvailable = false;

		// v1.41 Gate ServerÂ·ÃŽÃ€Ã‡ Ã€Ã§Â¿Â¬Â°Ã¡ Â½ÃƒÂµÂµ 
		m_pGateSock = new class XSocket(m_hWnd, DEF_SERVERSOCKETBLOCKLIMIT);
		m_pGateSock->bConnect(m_cGateServerAddr, m_iGateServerPort, WM_ONGATESOCKETEVENT);
		m_pGateSock->bInitBufferSize(DEF_MSGBUFFERSIZE);
		// v1.41 Â¿Â¬Â°Ã¡ Â½ÃƒÂ°Â£ ÃƒÂ¼Ã…Â©Â¿Ã« 
		m_iGateSockConnRetryTimes = 1;

		wsprintf(G_cTxt, "(!!!) Try to reconnect gate-socket... Addr:%s  Port:%d", m_cGateServerAddr, m_iGateServerPort);
		PutLogList(G_cTxt);

		
		break;
	}
}

void CGame::OnGateRead()
{
 UINT32 dwMsgSize;
 char * pData, cKey;

	pData = m_pGateSock->pGetRcvDataPointer(&dwMsgSize, &cKey);

	if (bPutMsgQuene(DEF_MSGFROM_GATESERVER, pData, dwMsgSize, 0, cKey) == false) {
		// Â¸ÃžÂ½ÃƒÃÃ¶ Ã…Â¥Â¿Â¡ Ã€ÃŒÂ»Ã³Ã€ÃŒ Â»Ã½Â°Ã¥Â´Ã™. Ã„Â¡Â¸Ã­Ã€Ã»Ã€ÃŽ Â¿Â¡Â·Â¯.
		PutLogList("@@@@@@ CRITICAL ERROR in MsgQuene!!! (OnGateRead) @@@@@@");
	}	
}

void CGame::SendMsgToGateServer(UINT32 dwMsg, int iClientH, char * pData)
{
 UINT32 * dwp;
 UINT16  * wp;
 int     iRet, i;
 char    cData[1000], cCharName[11], cAccountName[11], cAccountPassword[11], cAddress[16], cGuildName[21], cTxt[120], * cp;

	if (m_pGateSock == 0) {
		PutLogList("(!) SendMsgToGateServer fail - Socket to Gate-Server not available.");
		return;
	}

	ZeroMemory(cData, sizeof(cData));
	ZeroMemory(cCharName, sizeof(cCharName));
	ZeroMemory(cAccountName, sizeof(cAccountName));
	ZeroMemory(cAccountPassword, sizeof(cAccountPassword));
	ZeroMemory(cAddress,  sizeof(cAddress));
	ZeroMemory(cGuildName, sizeof(cGuildName));

	switch (dwMsg) {
	// New 07/05/2004
	case MSGID_PARTYOPERATION:
		iRet = m_pGateSock->iSendMsg(pData, 50);
		break;

	case MSGID_SERVERSTOCKMSG:
		iRet = m_pGateSock->iSendMsg(pData, m_iIndexGSS+1);
		break;
	
	case MSGID_ITEMLOG:
		// Â¾Ã†Ã€ÃŒÃ…Ã› Ã€Ã¼Â´Ãž Â·ÃŽÂ±Ã—Â´Ã™. Â»Ã§Â¿Ã«Ã‡ÃÃÃ¶ Â¾ÃŠÃ€Â½.
		dwp  = (UINT32 *)(cData + DEF_INDEX4_MSGID);
		*dwp = MSGID_ITEMLOG;
		wp   = (UINT16 *)(cData + DEF_INDEX2_MSGTYPE);
		*wp  = DEF_MSGTYPE_CONFIRM;
		
		cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);
		memcpy(cp, pData, 47);
		cp += 47;

		iRet = m_pGateSock->iSendMsg(cData, 53);
		break;
	
	case MSGID_REQUEST_REGISTERGAMESERVER:
		// Â°Ã”Ã€ÃŒÃ†Â® Â¼Â­Â¹Ã¶Â¿Â¡Â°Ã” Â°Ã”Ã€Ã“Â¼Â­Â¹Ã¶ ÂµÃ®Â·ÃÃ€Â» Â¿Ã¤ÃƒÂ»Ã‡Ã‘Â´Ã™.
		wsprintf(cTxt, "(!) Try to register game server(%s) - GateServer", m_cServerName);
		PutLogList(cTxt);
		
		dwp  = (UINT32 *)(cData + DEF_INDEX4_MSGID);
		*dwp = MSGID_REQUEST_REGISTERGAMESERVER;
		wp   = (UINT16 *)(cData + DEF_INDEX2_MSGTYPE);
		*wp  = DEF_MSGTYPE_CONFIRM;
		
		cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);

		memcpy(cp, m_cServerName, 10);
		cp += 10;

		if (m_iGameServerMode == 1)
		{
			memcpy(cAddress, m_cGameServerAddrInternal, strlen(m_cGameServerAddrInternal));
		}
		else if (m_iGameServerMode == 2)
		{
			memcpy(cAddress, m_cGameServerAddr, strlen(m_cGameServerAddr));
		}
		memcpy(cp, cAddress, 16);
		cp += 16;

		wp  = (UINT16 *)cp;
		*wp = m_iGameServerPort;
		cp += 2;

		*cp = m_iTotalMaps;
		cp++;

		for (i = 0; i < m_iTotalMaps; i++) {
			memcpy(cp, m_pMapList[i]->m_cName, 11);
			cp += 11;
		}
		
		dwp = (UINT32 *)cp;
		*dwp = (UINT32)GetCurrentProcessId();	 // Ã‡ÃÂ·ÃŽÂ¼Â¼Â½Âº Ã‡ÃšÂµÃ©Ã€Â» Â±Ã¢Â·ÃÃ‡Ã‘Â´Ã™.
		cp += 4;

		dwp = (UINT32 *)cp;
		*dwp = (UINT32)m_iBuildDate;
		cp += 4;

		iRet = m_pGateSock->iSendMsg(cData, 45 + m_iTotalMaps*11);
		break;

	case MSGID_GAMESERVERALIVE:
		// ÃÂ¤Â±Ã¢Ã€Ã»Ã€Â¸Â·ÃŽ Â°Ã”Ã€Ã“ Â¼Â­Â¹Ã¶Ã€Ã‡ ÃÂ¤ÂºÂ¸Â¸Â¦ Ã€Ã¼Â¼Ã› 
		// Â¸Â¸Â¾Ã  Â·ÃŽÂ±Ã— Â¼Â­Â¹Ã¶Â¿ÃÃ€Ã‡ Â¼Ã’Ã„Ã Â¿Â¬Â°Ã¡Ã€ÃŒ Â²Ã·Â¾Ã®ÃÂ³Â´Ã™Â¸Ã© Â¸ÃžÂ½ÃƒÃÃ¶Â¸Â¦ ÂºÂ¸Â³Â»ÃÃ¶ Â¾ÃŠÂ¾Ã† Â¼Â­Â¹Ã¶Â¿Â¡ Ã€ÃŒÂ»Ã³Ã€ÃŒ Â»Ã½Â°Ã¥Ã€Â½Ã€Â» Â¾Ã‹Â¸Â®Â°Ã” Ã€Â¯ÂµÂµÃ‡Ã‘Â´Ã™.
		if (m_bIsLogSockAvailable == false) return;
		
		dwp  = (UINT32 *)(cData + DEF_INDEX4_MSGID);
		*dwp = MSGID_GAMESERVERALIVE;
		wp   = (UINT16 *)(cData + DEF_INDEX2_MSGTYPE);
		*wp  = DEF_MSGTYPE_CONFIRM;

		cp  = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);
		wp  = (UINT16 *)cp;
		*wp = (UINT16)m_iTotalClients;
		cp += 2;
		
		iRet = m_pGateSock->iSendMsg(cData, 8);
		break;
	}

	switch (iRet) {
	case DEF_XSOCKEVENT_QUENEFULL:
	case DEF_XSOCKEVENT_SOCKETERROR:
	case DEF_XSOCKEVENT_CRITICALERROR:
	case DEF_XSOCKEVENT_SOCKETCLOSED:
		// Â°Ã”Ã€ÃŒÃ†Â® Â¼Â­Â¹Ã¶Â·ÃŽ Â¸ÃžÂ½ÃƒÃÃ¶Â¸Â¦ ÂºÂ¸Â³Â¾Â¶Â§ Â¿Â¡Â·Â¯Â°Â¡ Â¹ÃŸÂ»Ã½Ã‡ÃŸÂ´Ã™.
		PutLogList("(***) Socket to Gate-Server crashed! Critical error!");
		delete m_pGateSock;
		m_pGateSock = 0;
		m_bIsGateSockAvailable = false;

		// v1.41 Gate ServerÂ·ÃŽÃ€Ã‡ Ã€Ã§Â¿Â¬Â°Ã¡ Â½ÃƒÂµÂµ: Â¼Â­Â¹Ã¶Â¸Â¦ Â¼Ã‹Â´Ã™Â¿Ã® Ã‡ÃÂ´Ã‚ Â°ÃÃ€ÃŒ Â¾Ã†Â´ÃÂ´Ã™. 
		m_pGateSock = new class XSocket(m_hWnd, DEF_SERVERSOCKETBLOCKLIMIT);
		m_pGateSock->bConnect(m_cGateServerAddr, m_iGateServerPort, WM_ONGATESOCKETEVENT);
		m_pGateSock->bInitBufferSize(DEF_MSGBUFFERSIZE);
		// v1.41 Â¿Â¬Â°Ã¡ Â½ÃƒÂ°Â£ ÃƒÂ¼Ã…Â©Â¿Ã« 
		m_iGateSockConnRetryTimes = 1;	

		wsprintf(G_cTxt, "(!) Try to reconnect gate-socket... Addr:%s  Port:%d", m_cGateServerAddr, m_iGateServerPort);
		PutLogList(G_cTxt);

		
		break;
	}
}

void CGame::Quit()
{
 int i;	

	// Â¾Â²Â·Â¹ÂµÃ¥Â¸Â¦ ÃÃ—Ã€ÃŽÂ´Ã™.
	G_bIsThread = false;
	Sleep(300);
		
	if (m_pMainLogSock != 0) delete m_pMainLogSock;
	if (m_pGateSock != 0) delete m_pGateSock;

	for (i = 0; i < DEF_MAXSUBLOGSOCK; i++)
		if (m_pSubLogSock[i] != 0) delete m_pSubLogSock[i];

	for (i = 0; i < DEF_MAXCLIENTS; i++)
	if (m_pClientList[i] != 0) delete m_pClientList[i];

	for (i = 0; i < DEF_MAXNPCS; i++)
	if (m_pNpcList[i] != 0) delete m_pNpcList[i];

	for (i = 0; i < DEF_MAXMAPS; i++)
	if (m_pMapList[i] != 0) delete m_pMapList[i];

	for (i = 0; i < DEF_MAXITEMTYPES; i++)
	if (m_pItemConfigList[i] != 0) delete m_pItemConfigList[i];

	for (i = 0; i < DEF_MAXNPCTYPES; i++)
	if (m_pNpcConfigList[i] != 0) delete m_pNpcConfigList[i];

	for (i = 0; i < DEF_MAXMAGICTYPE; i++)
	if (m_pMagicConfigList[i] != 0) delete m_pMagicConfigList[i];

	for (i = 0; i < DEF_MAXSKILLTYPE; i++)
	if (m_pSkillConfigList[i] != 0) delete m_pSkillConfigList[i];

	for (i = 0; i < DEF_MAXQUESTTYPE; i++)
	if (m_pQuestConfigList[i] != 0) delete m_pQuestConfigList[i];

	for (i = 0; i < DEF_MAXDYNAMICOBJECTS; i++)
	if (m_pDynamicObjectList[i] != 0) delete m_pDynamicObjectList[i];

	for (i = 0; i < DEF_MAXDELAYEVENTS; i++)
	if (m_pDelayEventList[i] != 0) delete m_pDelayEventList[i];

	for (i = 0; i < DEF_MAXNOTIFYMSGS; i++)
	if (m_pNoticeMsgList[i] != 0) delete m_pNoticeMsgList[i];

	for (i = 0; i < DEF_MAXMINERALS; i++)
	if (m_pMineral[i] != 0) delete m_pMineral[i];

	for (i = 0; i < DEF_MAXPOTIONTYPES; i++) {
		if (m_pPotionConfigList[i] != 0) delete m_pPotionConfigList[i];
		if (m_pCraftingConfigList[i] != 0) delete m_pCraftingConfigList[i];	// Crafting
	}

	for (i = 0; i < DEF_MAXBUILDITEMS; i++) 
	if (m_pBuildItemList[i] != 0) delete m_pBuildItemList[i];

	for (i = 0; i < DEF_MAXDUPITEMID; i++)
	if (m_pDupItemIDList[i] != 0) delete m_pDupItemIDList[i];

	if (m_pNoticementData != 0) delete[] m_pNoticementData;

}

UINT32 CGame::iGetLevelExp(int iLevel)
{
	UINT32 iRet;
	
	if (iLevel == 0) return 0;
	
	iRet = iGetLevelExp(iLevel - 1) + iLevel * ( 50 + (iLevel * (iLevel / 17) * (iLevel / 17) ) );

	return iRet;
}

// 2003-04-14 ÃÃ¶ÃÂ¸ Ã†Ã·Ã€ÃŽÃ†Â®Â¸Â¦ Â·Â¹ÂºÂ§ Â¼Ã¶ÃÂ¤Â¿Â¡ Â¾ÂµÂ¼Ã¶ Ã€Ã–Â´Ã™...
/////////////////////////////////////////////////////////////////////////////////////
//  StateChangeHandler(int iClientH, char * pData, UINT32 dwMsgSize)
//  desc		 :: ÃÃ¶ÃÂ¸Ã†Ã·Ã€ÃŽÃ†Â®Â¸Â¦ Â·Â¹ÂºÂ§Â¾Ã· Ã†Ã·Ã€ÃŽÃ†Â®Â·ÃŽ ÂµÂ¹Â¸Â°Â´Ã™...
//	return value :: void
//  date		 :: [2003-04-14]    stupid koreans
/////////////////////////////////////////////////////////////////////////////////////
void CGame::StateChangeHandler(int iClientH, char* pData, UINT32 dwMsgSize)
{
	char* cp, cStateChange1, cStateChange2, cStateChange3;
	char cStr, cVit, cDex, cInt, cMag, cChar;
	int iOldStr, iOldVit, iOldDex, iOldInt, iOldMag, iOldChar;
	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
	if (m_pClientList[iClientH]->m_iGizonItemUpgradeLeft <= 0) return;
	cStr = cVit = cDex = cInt = cMag = cChar = 0;
	cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);
	cStateChange1 = *cp;
	cp++;
	cStateChange2 = *cp;
	cp++;
	cStateChange3 = *cp;
	cp++;
	iOldStr = m_pClientList[iClientH]->m_iStr;
	iOldVit = m_pClientList[iClientH]->m_iVit;
	iOldDex = m_pClientList[iClientH]->m_iDex;
	iOldInt = m_pClientList[iClientH]->m_iInt;
	iOldMag = m_pClientList[iClientH]->m_iMag;
	iOldChar = m_pClientList[iClientH]->m_iCharisma;

	if (!bChangeState(cStateChange1, &cStr, &cVit, &cDex, &cInt, &cMag, &cChar))
	{	// SNOOPY: Stats order was wrong here !
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_STATECHANGE_FAILED, 0, 0, 0, 0);
		return;
	}
	if (!bChangeState(cStateChange2, &cStr, &cVit, &cDex, &cInt, &cMag, &cChar))
	{	// SNOOPY: Stats order was wrong here !
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_STATECHANGE_FAILED, 0, 0, 0, 0);
		return;
	}
	if (!bChangeState(cStateChange3, &cStr, &cVit, &cDex, &cInt, &cMag, &cChar))
	{	// SNOOPY: Stats order was wrong here !
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_STATECHANGE_FAILED, 0, 0, 0, 0);
		return;
	}

	if (cStr < 0 || cVit < 0 || cDex < 0 || cInt < 0 || cMag < 0 || cChar < 0
		|| cStr + cVit + cDex + cInt + cMag + cChar != 3)
	{
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_STATECHANGE_FAILED, 0, 0, 0, 0);
		return;
	}
	if ((m_pClientList[iClientH]->m_iStr - cStr > m_sCharStatLimit)
		|| (m_pClientList[iClientH]->m_iStr - cStr < 10))
	{
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_STATECHANGE_FAILED, 0, 0, 0, 0);
		return;
	}
	if ((m_pClientList[iClientH]->m_iVit - cVit > m_sCharStatLimit)
		|| (m_pClientList[iClientH]->m_iVit - cVit < 10))
	{
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_STATECHANGE_FAILED, 0, 0, 0, 0);
		return;
	}
	if ((m_pClientList[iClientH]->m_iDex - cDex > m_sCharStatLimit)
		|| (m_pClientList[iClientH]->m_iDex - cDex < 10))
	{
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_STATECHANGE_FAILED, 0, 0, 0, 0);
		return;
	}
	if ((m_pClientList[iClientH]->m_iInt - cInt > m_sCharStatLimit)
		|| (m_pClientList[iClientH]->m_iInt - cInt < 10))
	{
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_STATECHANGE_FAILED, 0, 0, 0, 0);
		return;
	}
	if ((m_pClientList[iClientH]->m_iMag - cMag > m_sCharStatLimit)
		|| (m_pClientList[iClientH]->m_iMag - cMag < 10))
	{
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_STATECHANGE_FAILED, 0, 0, 0, 0);
		return;
	}
	if ((m_pClientList[iClientH]->m_iCharisma - cChar > m_sCharStatLimit)
		|| (m_pClientList[iClientH]->m_iCharisma - cChar < 10))
	{
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_STATECHANGE_FAILED, 0, 0, 0, 0);
		return;
	}
	m_pClientList[iClientH]->m_iLU_Pool += 3;
	m_pClientList[iClientH]->m_iGizonItemUpgradeLeft--;
	m_pClientList[iClientH]->m_iStr -= cStr;
	m_pClientList[iClientH]->m_iVit -= cVit;
	m_pClientList[iClientH]->m_iDex -= cDex;
	m_pClientList[iClientH]->m_iInt -= cInt;
	m_pClientList[iClientH]->m_iMag -= cMag;
	m_pClientList[iClientH]->m_iCharisma -= cChar;
	if (cInt > 0) bCheckMagicInt(iClientH);
	// Snoopy: Check max values here or character will be considered as hacker if reducing mana or vit
	if (m_pClientList[iClientH]->m_iHP > iGetMaxHP(iClientH)) m_pClientList[iClientH]->m_iHP = iGetMaxHP(iClientH);
	if (m_pClientList[iClientH]->m_iMP > iGetMaxMP(iClientH)) m_pClientList[iClientH]->m_iMP = iGetMaxMP(iClientH);
	if (m_pClientList[iClientH]->m_iSP > iGetMaxSP(iClientH)) m_pClientList[iClientH]->m_iSP = iGetMaxSP(iClientH);
	SendNotifyMsg(0, iClientH, DEF_NOTIFY_GIZONITEMUPGRADELEFT, m_pClientList[iClientH]->m_iGizonItemUpgradeLeft, 0, 0, 0);
	SendNotifyMsg(0, iClientH, DEF_NOTIFY_STATECHANGE_SUCCESS, 0, 0, 0, 0);
}

// 2003-04-14 ÃÃ¶ÃÂ¸ Ã†Ã·Ã€ÃŽÃ†Â®Â¸Â¦ Â·Â¹ÂºÂ§ Â¼Ã¶ÃÂ¤Â¿Â¡ Â¾ÂµÂ¼Ã¶ Ã€Ã–Â´Ã™...
/////////////////////////////////////////////////////////////////////////////////////
//  bChangeState(char cStateChange 
//  		  ,char* cStr, char *cVit,char *cDex,char *cInt,char *cMag,char *cChar)
//  desc		 :: ÃÃ¶ÃÂ¸ Ã†Ã·Ã€ÃŽÃ†Â®Â·ÃŽ Ã†Â¯Â¼ÂºÂ°Âª Â¹Ã™Â²ÃœÆ’â€Ã€Â» Â´ÃµÃ‡Ã˜ ÃÃ˜Â´Ã™...
//	return value :: BOOLÃ‡Ã¼ 0(FASLE) Â¿Â¡Â·Â¯ Â·Â¹ÂºÂ§ Â¼Ã¶ÃÂ¤ ÂºÃ’Â°Â¡...
//  date		 :: [2003-04-14] 
/////////////////////////////////////////////////////////////////////////////////////
bool CGame::bChangeState(char cStateChange, char* cStr, char *cVit,char *cDex,char *cInt,char *cMag,char *cChar)
{	// centu - fixed
	if(cStateChange == DEF_STR)
	{	++*cStr; 
	}else if(cStateChange == DEF_VIT)
	{	++*cVit; 
	}else if(cStateChange == DEF_DEX)
	{	++*cDex; 
	}else if(cStateChange == DEF_INT)
	{	++*cInt; 
	}else if(cStateChange == DEF_MAG)
	{	++*cMag; 
	}else if(cStateChange == DEF_CHR)
	{	++*cChar; 
	}else
	{	return 0;
	}
	return cStateChange;
}

void CGame::LevelUpSettingsHandler(int iClientH, char * pData, UINT32 dwMsgSize)
{
	char* cp;
	// sleeq - fix for negative level up points
	short* sp;
	short cStr, cVit, cDex, cInt, cMag, cChar;

	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
	if (m_pClientList[iClientH]->m_iLU_Pool < 0) 
	{
		//Ã‡Ã˜Ã„Â¿Ã€ÃŽÂ°Â¡??
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_SETTING_FAILED, 0, 0, 0, 0);
		return ;
	}
	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

	sp = (short*)cp;
	cStr = *sp;
	cp += 2;

	sp = (short*)cp;
	cVit = *sp;
	cp += 2;

	sp = (short*)cp;
	cDex = *sp;
	cp += 2;

	sp = (short*)cp;
	cInt = *sp;
	cp += 2;

	sp = (short*)cp;
	cMag = *sp;
	cp += 2;

	sp = (short*)cp;
	cChar = *sp;
	cp += 2;


	if ( (cStr + cVit + cDex + cInt + cMag + cChar) > m_pClientList[iClientH]->m_iLU_Pool) { // -3
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_SETTING_FAILED, 0, 0, 0, 0);
		return;
	}

	// Level-Up SettingÂ°ÂªÂ¿Â¡ Â¿Ã€Â·Ã¹Â°Â¡ Ã€Ã–Â´Ã‚ÃÃ¶ Â°Ã‹Â»Ã§Ã‡Ã‘Â´Ã™.
	if ((m_pClientList[iClientH]->m_iStr + cStr > m_sCharStatLimit) || (cStr < 0))
		return;

	if ((m_pClientList[iClientH]->m_iDex + cDex > m_sCharStatLimit) || (cDex < 0))
		return;

	if ((m_pClientList[iClientH]->m_iInt + cInt > m_sCharStatLimit) || (cInt < 0))
		return;

	if ((m_pClientList[iClientH]->m_iVit + cVit > m_sCharStatLimit) || (cVit < 0))
		return;

	if ((m_pClientList[iClientH]->m_iMag + cMag > m_sCharStatLimit) || (cMag < 0))
		return;

	if ((m_pClientList[iClientH]->m_iCharisma + cChar > m_sCharStatLimit) || (cChar < 0))
		return;

	int iTotalSetting = m_pClientList[iClientH]->m_iStr + m_pClientList[iClientH]->m_iDex + m_pClientList[iClientH]->m_iVit +
		m_pClientList[iClientH]->m_iInt + m_pClientList[iClientH]->m_iMag + m_pClientList[iClientH]->m_iCharisma;

	if (iTotalSetting + m_pClientList[iClientH]->m_iLU_Pool - 3 > ((m_pClientList[iClientH]->m_iLevel - 1) * 3 + 70))
	{
		m_pClientList[iClientH]->m_iLU_Pool = 3 + (m_pClientList[iClientH]->m_iLevel - 1) * 3 + 70 - iTotalSetting;

		if (m_pClientList[iClientH]->m_iLU_Pool < 3)
			m_pClientList[iClientH]->m_iLU_Pool = 3;
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_SETTING_FAILED, 0, 0, 0, 0);
		return;
	}

	//(Â·Â¹ÂºÂ§ Ã†Â¯Â¼ÂºÂ°Âª + Â·Â¹ÂºÂ§Â¾Ã· Â½ÃƒÃ…Â³ Ã†Ã·Ã€ÃŽÃ†Â® ÃD > Â·Â¹ÂºÂ§Â¾Ã· Ã†Â¯Â¼ÂºÂ°Âª ÃÂ¤Â»Ã³Ã„Â¡)Ã€ÃŒÂ¸Ã© ÃƒÂ³Â¸Â® ÂºÃ’Â°Â¡..
	if (iTotalSetting + (cStr + cVit + cDex + cInt + cMag + cChar) >	((m_pClientList[iClientH]->m_iLevel)*3 + 70)) 
	{
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_SETTING_FAILED, 0, 0, 0, 0);
		return;
	}

	m_pClientList[iClientH]->m_iLU_Pool -= (cStr + cVit + cDex + cInt + cMag + cChar);

	// Â¿Ã€Â·Ã¹Â°Â¡ Â¾Ã¸Ã€Â¸Â¸Ã© Â°ÂªÃ€Â» Ã‡Ã’Â´Ã§Ã‡Ã‘Â´Ã™.
	m_pClientList[iClientH]->m_iStr  += cStr;
	m_pClientList[iClientH]->m_iVit  += cVit;
	m_pClientList[iClientH]->m_iDex  += cDex;
	m_pClientList[iClientH]->m_iInt  += cInt;
	m_pClientList[iClientH]->m_iMag  += cMag;
	m_pClientList[iClientH]->m_iCharisma += cChar;

	SendNotifyMsg(0, iClientH, DEF_NOTIFY_SETTING_SUCCESS, 0, 0, 0, 0);

}

// v1.4311-3 ÃƒÃŸÂ°Â¡ Â»Ã§Ã…ÃµÃ€Ã¥ Â¿Â¹Â¾Ã  Ã‡Ã”Â¼Ã¶ FightzoneReserveHandler
void CGame::FightzoneReserveHandler(int iClientH, char * pData, UINT32 dwMsgSize)
{
 char cData[100];
 int iFightzoneNum ,* ip ,  iEnableReserveTime ;
 UINT32 * dwp, dwGoldCount ;
 UINT16  * wp, wResult;
 int     iRet, iResult=1, iCannotReserveDay;
 SYSTEMTIME SysTime;
 
	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
 
	GetLocalTime(&SysTime);
	iEnableReserveTime  = 2*20*60 - ((SysTime.wHour%2)*20*60 + SysTime.wMinute*20) - 5*20;
	
	dwGoldCount = dwGetItemCount(iClientH, "Gold");
		
	ip = (int *)(pData + DEF_INDEX2_MSGTYPE + 2);
	iFightzoneNum = *ip;

	if ((iFightzoneNum < 1) || (iFightzoneNum > DEF_MAXFIGHTZONE)) return;

	iCannotReserveDay = (SysTime.wDay + m_pClientList[iClientH]->m_cSide + iFightzoneNum ) % 2 ;
	if (iEnableReserveTime <= 0 ){
		wResult = DEF_MSGTYPE_REJECT;
		iResult = 0 ;
	} else if (m_iFightZoneReserve[iFightzoneNum-1] != 0){
		wResult = DEF_MSGTYPE_REJECT;
		iResult = -1 ;
	} else if ( dwGoldCount < 1500 ) {
		wResult = DEF_MSGTYPE_REJECT;
		iResult = -2 ;
	} else if( iCannotReserveDay ) {
		wResult = DEF_MSGTYPE_REJECT;
		iResult = -3 ;
	} else if( m_pClientList[iClientH]-> m_iFightzoneNumber != 0 ) {
		wResult = DEF_MSGTYPE_REJECT;
		iResult = -4 ;
	} else {
		wResult = DEF_MSGTYPE_CONFIRM;

		SetItemCount(iClientH, "Gold", dwGoldCount - 1500);
		iCalcTotalWeight(iClientH);
		
		m_iFightZoneReserve[iFightzoneNum-1] = iClientH;
		
		m_pClientList[iClientH]->m_iFightzoneNumber  = iFightzoneNum ;
		m_pClientList[iClientH]->m_iReserveTime	 =  SysTime.wMonth*10000 + SysTime.wDay*100 + SysTime.wHour ;  

		if (SysTime.wHour%2 )	m_pClientList[iClientH]->m_iReserveTime++;
		else					m_pClientList[iClientH]->m_iReserveTime += 2;
		wsprintf(G_cTxt, "(*) Reserve FIGHTZONETICKET : Char(%s) TICKENUMBER (%d)", m_pClientList[iClientH]->m_cCharName, m_pClientList[iClientH]->m_iReserveTime);		
		PutLogFileList(G_cTxt);
		PutLogList(G_cTxt);

		m_pClientList[iClientH]->m_iFightZoneTicketNumber  = 50 ;  
		iResult = 1 ;
	}  

	ZeroMemory(cData, sizeof(cData));

	dwp = (UINT32 *)(cData + DEF_INDEX4_MSGID);
	*dwp = MSGID_RESPONSE_FIGHTZONE_RESERVE;

	wp = (UINT16 *)(cData + DEF_INDEX2_MSGTYPE);
	*wp =  wResult ;

	ip  = (int *)(cData + DEF_INDEX2_MSGTYPE+2) ; 
	*ip =  iResult ;
	ip+= 4;
	
	// Â»Ã§Ã…ÃµÃ€Ã¥ Â¿Â¹Â¾Ã  Ã€Ã€Â´Ã¤ Â¸ÃžÂ¼Â¼ÃÃ¶  Ã…Â¬Â¶Ã³Ã€ÃŒÂ¾Ã°Ã†Â®Â¿Â¡Â°Ã” Ã€Ã¼Â¼Ã›
		
	iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 10);
 
	switch (iRet) {
		case DEF_XSOCKEVENT_QUENEFULL:
		case DEF_XSOCKEVENT_SOCKETERROR:
		case DEF_XSOCKEVENT_CRITICALERROR:
		case DEF_XSOCKEVENT_SOCKETCLOSED:
		// Â¸ÃžÂ½ÃƒÃÃ¶Â¸Â¦ ÂºÂ¸Â³Â¾Â¶Â§ Â¿Â¡Â·Â¯Â°Â¡ Â¹ÃŸÂ»Ã½Ã‡ÃŸÂ´Ã™Â¸Ã© ÃÂ¦Â°Ã…Ã‡Ã‘Â´Ã™.
		DeleteClient(iClientH, true, true);
		break;
	}
}

bool CGame::bCheckLimitedUser(int iClientH)
{
	if (m_pClientList[iClientH] == 0) return false;
	
	if ( (memcmp(m_pClientList[iClientH]->m_cLocation, "NONE", 4) == 0) && 
		 (m_pClientList[iClientH]->m_iExp >= m_iLevelExp20 ) ) {
		// Â¿Â©Ã‡Ã Ã€ÃšÂ°Â¡ Â·Â¹ÂºÂ§ 20 Â°Ã¦Ã‡Ã¨Ã„Â¡Â¸Â¦ Â¾Ã²Â¾ÃºÂ´Ã™Â¸Ã© 19Â¼Ã¶ÃÃ˜Ã€Â¸Â·ÃŽ ÃˆÂ¯Â¿Ã¸. 

		m_pClientList[iClientH]->m_iExp = m_iLevelExp20 - 1;	
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_TRAVELERLIMITEDLEVEL, 0, 0, 0, 0);
		return true;
	}


	return false;
}

void CGame::RequestCivilRightHandler(int iClientH, char *pData)
{
 char * cp, cData[100];
 UINT32 * dwp;
 UINT16 * wp, wResult;
 int  iRet;

	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
	if ((m_bAdminSecurity == true) && (m_pClientList[iClientH]->m_iAdminUserLevel > 0 && m_pClientList[iClientH]->m_iAdminUserLevel < 7)) return;

	// ?íš‘ì¨”íš‘ íš‰íš— ì¨ì¨‹?ì¨©?íš‰ ì©Œíš˜ì©Œíš™?íš‘ ?íšœì¨ˆíš¢ì¨ì±• ì©íš„ì¨”íš“ì§¹íš‰?ì¨© ì§¸ì§•íšì²¬ ì©Œì²  ì©ì²©ì¨ˆíš¢. 
	if (memcmp(m_pClientList[iClientH]->m_cLocation, "NONE", 4) != 0) wResult = 0;
	else wResult = 1;

	
	if (wResult == 1) {
		// íš‰ì² ?ì±Œ ì¨íšŽ?íš‰ ?íš‘ì¨ì§ ?ì¨© íš‰íš˜ì¨ˆì±Œíš‰íš—ì¨ˆíš¢.
		ZeroMemory(m_pClientList[iClientH]->m_cLocation, sizeof(m_pClientList[iClientH]->m_cLocation));
		strcpy(m_pClientList[iClientH]->m_cLocation, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName);
	}

	// Side íš‰íš˜ì¨ˆì±Œ
	if (memcmp(m_pClientList[iClientH]->m_cLocation, "are", 3) == 0) 
		m_pClientList[iClientH]->m_cSide = 1;

	else if (memcmp(m_pClientList[iClientH]->m_cLocation, "elv", 3) == 0) 
		m_pClientList[iClientH]->m_cSide = 2;

	dwp  = (UINT32 *)(cData + DEF_INDEX4_MSGID);
	*dwp = MSGID_RESPONSE_CIVILRIGHT;
	wp   = (UINT16 *)(cData + DEF_INDEX2_MSGTYPE);
	*wp  = wResult;

	// v1.41 ì¨íšŽ ?íš‘ì¨ì§  ì©íšì¨Œíšíšíš¥ 
	cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);
	memcpy(cp, m_pClientList[iClientH]->m_cLocation, 10);
	cp += 10;

	// ??ì¨ˆì±„ ì¨íš§ì©íš„íšì² ì¨ì§ íš‡ì§­ì¨‹ì²˜?íš‘ì©ì±¨íšˆì§°ì©”ì§•ì§¸íšš ?ì²´ì©Œíš¤
	iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 16);
	switch (iRet) {
	case DEF_XSOCKEVENT_QUENEFULL:
	case DEF_XSOCKEVENT_SOCKETERROR:
	case DEF_XSOCKEVENT_CRITICALERROR:
	case DEF_XSOCKEVENT_SOCKETCLOSED:
		// ì¨íš§ì©íš„íšì² ì¨ì§ ì¨˜ì¨ì¨€ì©ì¨‹ì§  ì©”ì§•ì¨Œì§±ì§¸ì§• ì¨”íš©ì¨©ì²µíš‰íš©ì¨ˆíš¢ì¨ì±• íšì§ì§¸íš‡íš‰íš—ì¨ˆíš¢.
		DeleteClient(iClientH, true, true);
		return;
	}
	// íšˆì§±ì©Œì¨˜?íš‘ ì¨”íš¢ì§¼ì±¤ì¨”íš‰ì¨Œíš“ ì©”íš¥ì©ì±Œ?ì¨© ì¨©ì²œì¨Œíš“ ì¨˜ì¨ì¨€ì©ì¨ˆíš¢. 
	SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
	// new
	if (m_pClientList[iClientH]->m_iPartyID != 0) {
		RequestDeletePartyHandler(iClientH);
	}
	
	RequestChangePlayMode(iClientH); // new
}

void CGame::CheckSpecialEvent(int iClientH)
{
	char cItemName[21], cData[100], * cp;
	short* sp;
	class CItem* pItem;
	UINT32* dwp;
	UINT16* wp;
	int iEraseReq, iRet;
	SYSTEMTIME SysTime;

	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_cSide == 0) return;
	ZeroMemory(cItemName, sizeof(cItemName));
	strcpy(cItemName, "MemorialRing");
	pItem = new class CItem;
	if (pItem == 0) return;
	if (_bInitItemAttr(pItem, cItemName) == false) {
		delete pItem;
	}
	else {
		bAddItem(iClientH, pItem, 0);
		if (_bAddClientItemList(iClientH, pItem, &iEraseReq) == true) {
			if (m_pClientList[iClientH]->m_iCurWeightLoad < 0) m_pClientList[iClientH]->m_iCurWeightLoad = 0;
			wsprintf(G_cTxt, "(*) Get MemorialRing (%s) Char(%s)", m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName);
			PutLogFileList(G_cTxt);
			pItem->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
			pItem->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
			pItem->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
			pItem->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;
			pItem->m_cItemColor = 9;
			GetLocalTime(&SysTime);
			char ID[9];
			wsprintf(ID, "%d%d%d", SysTime.wYear, SysTime.wMonth, SysTime.wDay);
			m_pClientList[iClientH]->m_iSpecialEventID = (int)ID;
			_bItemLog(DEF_ITEMLOG_GET, iClientH, -1, pItem, 0);
			ZeroMemory(cData, sizeof(cData));
			dwp = (UINT32*)(cData + DEF_INDEX4_MSGID);
			*dwp = MSGID_NOTIFY;
			wp = (UINT16*)(cData + DEF_INDEX2_MSGTYPE);
			*wp = DEF_NOTIFY_ITEMOBTAINED;
			cp = (char*)(cData + DEF_INDEX2_MSGTYPE + 2);
			*cp = 1;
			cp++;
			memcpy(cp, pItem->m_cName, 20);
			cp += 20;
			dwp = (UINT32*)cp;
			*dwp = pItem->m_dwCount;
			cp += 4;
			*cp = pItem->m_cItemType;
			cp++;
			*cp = pItem->m_cEquipPos;
			cp++;
			*cp = (char)0;
			cp++;
			sp = (short*)cp;
			*sp = pItem->m_sLevelLimit;
			cp += 2;
			*cp = pItem->m_cGenderLimit;
			cp++;
			wp = (UINT16*)cp;
			*wp = pItem->m_wCurLifeSpan;
			cp += 2;
			wp = (UINT16*)cp;
			*wp = pItem->m_wWeight;
			cp += 2;
			sp = (short*)cp;
			*sp = pItem->m_sSprite;
			cp += 2;
			sp = (short*)cp;
			*sp = pItem->m_sSpriteFrame;
			cp += 2;
			*cp = pItem->m_cItemColor;
			cp++;
			*cp = (char)pItem->m_sItemSpecEffectValue2;
			cp++;
			dwp = (UINT32*)cp;
			*dwp = pItem->m_dwAttribute;
			cp += 4;
			iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 53);
			switch (iRet) {
			case DEF_XSOCKEVENT_QUENEFULL:
			case DEF_XSOCKEVENT_SOCKETERROR:
			case DEF_XSOCKEVENT_CRITICALERROR:
			case DEF_XSOCKEVENT_SOCKETCLOSED:
				DeleteClient(iClientH, true, true, true, false);
				return;
			}
		}
	}
}

// 05/21/2004 - Hypnotoad - send player to jail
void CGame::ApplyPKpenalty(short sAttackerH, short sVictumH)
{
	int iV1, iV2;

	if (m_pClientList[sAttackerH] == 0) return;
	if (m_pClientList[sVictumH] == 0) return;
	// Â¾ÃˆÃ€Ã¼ Â°Ã¸Â°Ã Â¸Ã°ÂµÃ¥Â°Â¡ Ã„Ã‘ÃÃ¸ Â»Ã³Ã…Ã‚Â¿Â¡Â¼Â­ Â°Ã¸Â°ÃÃ€Â¸Â·ÃŽ Ã€ÃŽÃ‡Ã˜ Â°Â°Ã€Âº Ã†Ã­Ã€ÃŒ ÃÃ—Ã€Â»Â¼Ã¶Â´Ã‚ Â¾Ã¸ÃÃ¶Â¸Â¸ Â¸Â¸Â¾Ã  Ã€Ã–Ã€Â»Â°Ã¦Â¿Ã¬ Â¹Â«Â½Ãƒ 
	// Â´Ãœ Â°Ã¸Â°ÃÃ€ÃšÂ°Â¡ Â¹Ã¼ÃÃ‹Ã€ÃšÂ°Â¡ Â¾Ã†Â´ÃÂ¾Ã®Â¾ÃŸÂ¸Â¸ Ã‡Ã˜Â´Ã§ÂµÃˆÂ´Ã™.
	if ((m_pClientList[sAttackerH]->m_bIsSafeAttackMode == true) && (m_pClientList[sAttackerH]->m_iPKCount == 0)) return; 

	// PK Count ÃÃµÂ°Â¡  
	m_pClientList[sAttackerH]->m_iPKCount++;

	_bPKLog(DEF_PKLOG_BYPK,sAttackerH,sVictumH,0) ;

	iV1 = iDice(((m_pClientList[sVictumH]->m_iLevel )/2)+1, 50);
	iV2 = iDice(((m_pClientList[sAttackerH]->m_iLevel)/2)+1, 50);

	m_pClientList[sAttackerH]->m_iExp -= iV1; 
	m_pClientList[sAttackerH]->m_iExp -= iV2;
	if (m_pClientList[sAttackerH]->m_iExp < 0) m_pClientList[sAttackerH]->m_iExp = 0;

	SendNotifyMsg(0, sAttackerH, DEF_NOTIFY_PKPENALTY, 0, 0, 0, 0);	

	SendEventToNearClient_TypeA(sAttackerH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);

	if (strcmp(m_pClientList[sAttackerH]->m_cLocation, "aresden") == 0) {
		if ((strcmp(m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_cName, "aresden") == 0)) {

				// Â¾Ã†Â·Â¹Â½ÂºÂµÂ§ Â±Ã™Â±Â³Â¿Â¡Â¼Â­ PKÂ¸Â¦ Ã‡ÃŸÂ´Ã™. ÂºÃ­Â¸Â®ÂµÃ¹ Ã…ÃšÂ·Â¹Ã†Ã·Ã†Â® 5ÂºÃ
				// v2.16 Â¼ÂºÃˆÃ„Â´Ã Â¼Ã¶ÃÂ¤
				ZeroMemory(m_pClientList[sAttackerH]->m_cLockedMapName, sizeof(m_pClientList[sAttackerH]->m_cLockedMapName));
				strcpy(m_pClientList[sAttackerH]->m_cLockedMapName, "arejail");
				m_pClientList[sAttackerH]->m_iLockedMapTime = m_sJailTime;
				RequestTeleportHandler(sAttackerH, "2   ", "arejail", -1, -1);
						
			}
	}

	else if (strcmp(m_pClientList[sAttackerH]->m_cLocation, "elvine") == 0) {
		if ((strcmp(m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_cName, "elvine") == 0)) {

				// Â¿Â¤Â¹Ã™Ã€ÃŽ Â±Ã™Â±Â³Â¿Â¡Â¼Â­ PKÂ¸Â¦ Ã‡ÃŸÂ´Ã™. ÂºÃ­Â¸Â®ÂµÃ¹ Ã…ÃšÂ·Â¹Ã†Ã·Ã†Â® 5ÂºÃ
				ZeroMemory(m_pClientList[sAttackerH]->m_cLockedMapName, sizeof(m_pClientList[sAttackerH]->m_cLockedMapName));
				strcpy(m_pClientList[sAttackerH]->m_cLockedMapName, "elvjail");
				m_pClientList[sAttackerH]->m_iLockedMapTime = m_sJailTime;
				RequestTeleportHandler(sAttackerH, "2   ", "elvjail", -1, -1);
					
			}
	}
}


// 05/17/2004 - Hypnotoad - register pk log
void CGame::PK_KillRewardHandler(short sAttackerH, short sVictumH)
{
	if (m_pClientList[sAttackerH] == 0) return;
	if (m_pClientList[sVictumH] == 0)   return;

	_bPKLog(DEF_PKLOG_BYPLAYER,sAttackerH,sVictumH,0) ;

	if (m_pClientList[sAttackerH]->m_iPKCount == 0) {
		// PKÂ¸Â¦ Ã€Ã¢Ã€Âº Â»Ã§Â¶Ã·Ã€ÃŒ PKÂ¸Ã© Â¾Ã†Â¹Â«Â·Â± ÂµÃ¦Ã€ÃŒ Â¾Ã¸Â´Ã™.

		// Ã†Ã·Â»Ã³Â±ÃÂ¸Â¸ Â´Â©Ã€Ã». Â°Ã¦Ã‡Ã¨Ã„Â¡Â´Ã‚ Â¿Ã€Â¸Â£ÃÃ¶ Â¾ÃŠÂ´Ã‚Â´Ã™. 
		m_pClientList[sAttackerH]->m_iRewardGold += iGetExpLevel(m_pClientList[sVictumH]->m_iExp) * 3;
		
		if (m_pClientList[sAttackerH]->m_iRewardGold < 0) 
			m_pClientList[sAttackerH]->m_iRewardGold = 0;

		SendNotifyMsg(0, sAttackerH, DEF_NOTIFY_PKCAPTURED, m_pClientList[sVictumH]->m_iPKCount, m_pClientList[sVictumH]->m_iLevel, 0, m_pClientList[sVictumH]->m_cCharName);
	}
}

void CGame::EnemyKillRewardHandler(int iAttackerH, int iClientH)
{
	// enemy-kill-mode = 1 | 0
	// if m_bEnemyKillMode is true than death match mode

	// DEATHMATCH MODE:
	// Aresden kills Elvine in Aresden and gets EK
	// Elvine kills Aresden in Aresden and gets an EK 
	// Elvine kills Aresden in Elvine and gets an EK
	// Aresden kills Elvine in Elvine and gets an EK

	// CLASSIC MODE:
	// Aresden kills Elvine in Aresden and gets EK
	// Elvine kills Aresden in Aresden and doesnt get an EK 
	// Elvine kills Aresden in Elvine and gets an EK
	// Aresden kills Elvine in Elvine and doesnt get an EK
	
	UINT32 iRewardExp;

	int iRangoAttacker = 1;
	
	// if the attacker doesnt exist no result
	if (m_pClientList[iAttackerH] == 0) return;
	// if the target doesnt exist no result
	if (m_pClientList[iClientH] == 0)   return;

	// Log the EK (killed by enemy)
	_bPKLog(DEF_PKLOG_BYENERMY,iAttackerH,iClientH,0) ;

	if (m_pClientList[iAttackerH]->m_iPKCount == 0) {
		// Ã€Ã»Ã€Â» Ã€Ã¢Ã€Âº Â»Ã§Â¶Ã·Ã€ÃŒ PKÂ¸Ã© Â¾Ã†Â¹Â«Â·Â± ÂµÃ¦Ã€ÃŒ Â¾Ã¸Â´Ã™.
	
		// ÃˆÃ±Â»Ã½Ã€ÃšÃ€Ã‡ Â½ÃƒÂ¹ÃŽ, Â±Ã¦ÂµÃ¥Â¿Ã¸ Â¿Â©ÂºÃŽÂ¿Â¡ ÂµÃ»Â¶Ã³ Ã†Ã·Â»Ã³Ã€ÃŒ Â´ÃžÂ¶Ã³ÃÃ¸Â´Ã™. 
		
		// Â±Ã¦ÂµÃ¥Â¿Ã¸Ã€ÃŒ Â¾Ã†Â´ÃÂ¹Ã‡Â·ÃŽ Â½ÃƒÂ¹ÃŽ. (Â¿Â©Ã‡Ã Ã€ÃšÃ€Ã‡ Â°Ã¦Â¿Ã¬Â¿Â¡Â´Ã‚ Ã€ÃŒ Ã‡Ã”Â¼Ã¶Â°Â¡ ÃˆÂ£ÃƒÃ¢ÂµÃ‡ÃÃ¶ Â¾ÃŠÃ€Â¸Â¹Ã‡Â·ÃŽ)
		// v2.15 Ã€Ã»Ã€Â» ÃÃ—Â¿Â´Ã€Â»Â¶Â§ Ã†Ã²Â±Ã•Â°Ã¦Ã‡Ã¨Ã„Â¡Â¸Â¦ Â¾Ã²Â±Ã¢Ã€Â§Ã‡Ã˜ 
		iRewardExp = (iDice(3, (3*iGetExpLevel(m_pClientList[iClientH]->m_iExp))) + iGetExpLevel(m_pClientList[iClientH]->m_iExp))/ 3 ;

		if (m_bIsCrusadeMode == true || m_bIsHeldenianMode == true) {
			// Ã…Â©Â·Ã§Â¼Â¼Ã€ÃŒÂµÃ¥ Â¸Ã°ÂµÃ¥Â¶Ã³Â¸Ã© Â°Ã¦Ã‡Ã¨Ã„Â¡Ã€Ã‡ (1/3)*3Ã€Â» Â¸Ã•Ã€Ãº ÃÃ–Â°Ã­ Â³ÂªÂ¸Ã“ÃÃ¶Â´Ã‚ 6Â¹Ã¨Â·ÃŽ Ã€Ã¼Ã€Ã¯ Â°Ã¸Ã‡Ã¥ÂµÂµÂ¿Â¡ Â´Â©Ã€Ã» 

			GetExp(iAttackerH, (iRewardExp / 3) * 4);
			m_pClientList[iAttackerH]->m_iWarContribution += (iRewardExp - (iRewardExp/3))*12;

			m_pClientList[iAttackerH]->m_iConstructionPoint += (m_pClientList[iClientH]->m_iLevel) / 2;

			// Â¾Ã‹Â·ÃÃÃ˜Â´Ã™.
			SendNotifyMsg(0, iAttackerH, DEF_NOTIFY_CONSTRUCTIONPOINT, m_pClientList[iAttackerH]->m_iConstructionPoint, m_pClientList[iAttackerH]->m_iWarContribution, 0, 0);

		}
		else {
			// Ã€ÃÂ¹Ã Â¸Ã°ÂµÃ¥.

			GetExp(iAttackerH, iRewardExp);
		}

		// ÃˆÃ±Â»Ã½Ã€ÃšÃ€Ã‡ Â·Â¹ÂºÂ§Ã€ÃŒ 80Ã€ÃŒÂ»Ã³Ã€ÃŒÂ°Ã­
		if (memcmp(m_pClientList[iClientH]->m_cLocation, m_pClientList[iClientH]->m_cMapName, 10) != 0) {
			// ÃˆÃ±Â»Ã½Ã€ÃšÂ°Â¡ ÃÃ—Ã€Âº Â°Ã·Ã€ÃŒ Ã€ÃšÂ½Ã…Ã€Ã‡ Â¸Â¶Ã€Â»Ã€ÃŒ Â¾Ã†Â´ÃÂ¶Ã³Â¸Ã© EKÂ·ÃŽ Ã€ÃŽÃÂ¤ 
			if (m_bHappyHour)
				m_pClientList[iAttackerH]->m_iEnemyKillCount += 40;
			else if (m_bFuryHour)
				m_pClientList[iAttackerH]->m_iEnemyKillCount += 100;
			else
				m_pClientList[iAttackerH]->m_iEnemyKillCount += m_iEnemyKillAdjust;
			
			if (m_pClientList[iAttackerH]->m_iEnemyKillCount > m_pClientList[iAttackerH]->m_iMaxEK)
			{
				m_pClientList[iAttackerH]->m_iMaxEK = m_pClientList[iAttackerH]->m_iEnemyKillCount;
			}
			calcularTop15HB(iAttackerH);
			
			SendNotifyMsg(0, iAttackerH, DEF_NOTIFY_ENEMYKILLS, m_pClientList[iAttackerH]->m_iEnemyKillCount, m_pClientList[iAttackerH]->m_iMaxEK, 0, 0);
			

		}

		// Ã†Ã·Â»Ã³Â±Ã Â´Â©Ã€Ã» 
		m_pClientList[iAttackerH]->m_iRewardGold += iDice(1, (iGetExpLevel(m_pClientList[iClientH]->m_iExp)));

		if (m_pClientList[iAttackerH]->m_iRewardGold < 0)
			m_pClientList[iAttackerH]->m_iRewardGold = 0;		

		// Ã€Ã»Ã€Â» Ã€Ã¢Â¾Ã’Â´Ã™Â´Ã‚ Â¸ÃžÂ½ÃƒÃÃ¶ ÂºÂ¸Â³Â¿ 
		SendNotifyMsg(0, iAttackerH, DEF_NOTIFY_ENEMYKILLREWARD, iClientH, 0, 0, 0);
	}
}

// 05/22/2004 - Hypnotoad - register in pk log
void CGame::ApplyCombatKilledPenalty(int iClientH, int cPenaltyLevel, bool bIsSAattacked)
{  
	UINT32 iExp;

	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;

	// Crusade
	if (m_bIsCrusadeMode == true || m_bIsHeldenianMode == true) {
		// Ã…Â©Â·Ã§Â¼Â¼Ã€ÃŒÂµÃ¥ Â¸Ã°ÂµÃ¥Ã€ÃŽ Â°Ã¦Â¿Ã¬ Ã€Ã¼Ã€Ã¯ÃÃŸÂ¿Â¡ ÃÃ—Â¾Ã®ÂµÂµ Â¾Ã†Â¹Â«Â·Â± Ã†Ã¤Â³ÃŽÃ†Â¼Â°Â¡ Â¾Ã¸Â´Ã™.
		// PKcountÂ¸Â¸ Â°Â¨Â¼Ã’ 
		if (m_pClientList[iClientH]->m_iPKCount > 0) {
			m_pClientList[iClientH]->m_iPKCount--;
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_PKPENALTY, 0, 0, 0, 0);	
			// v2.15 
			_bPKLog(DEF_PKLOG_REDUCECRIMINAL,0,iClientH,0) ;

		}
		
	}
	else {
		// PKcount Â°Â¨Â¼Ã’ 
		if (m_pClientList[iClientH]->m_iPKCount > 0) {
			m_pClientList[iClientH]->m_iPKCount--;
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_PKPENALTY, 0, 0, 0, 0);	
			// v2.15
			_bPKLog(DEF_PKLOG_REDUCECRIMINAL,0,iClientH,0) ;
		}

		// Ã‡ÃƒÂ·Â¹Ã€ÃŒÂ¾Ã®Â·ÃŽÂºÃŽÃ…ÃÃ€Ã‡ Â°Ã¸Â°ÃÃ€Â» Â¹ÃžÂ°Ã­ Ã€Ã¼Ã€Ã¯ÃÃŸ Â»Ã§Â¸ÃÃ‡ÃŸÂ´Ã™.
		iExp = iDice(1, (5*cPenaltyLevel*(m_pClientList[iClientH]->m_iLevel)));

		// ÃÃŸÂ¸Â³Ã€ÃŽ Â°Ã¦Â¿Ã¬ ÃÃ—Â¾ÃºÃ€Â» Â¶Â§ Â¶Â³Â¾Ã®ÃÃ¶Â´Ã‚ Â°Ã¦Ã‡Ã¨Ã„Â¡Â´Ã‚ 1/3
		if (m_pClientList[iClientH]->m_bIsNeutral == true) iExp = iExp / 3;

		m_pClientList[iClientH]->m_iExp -= iExp;
		if (m_pClientList[iClientH]->m_iExp < 0) m_pClientList[iClientH]->m_iExp = 0;

		SendNotifyMsg(0, iClientH, DEF_NOTIFY_EXP, 0, 0, 0, 0);

		// v1.41 ÃÃŸÂ¸Â³Ã€Âº Â¾Ã†Ã€ÃŒÃ…Ã›Ã€ÃŒ Â¶Â³Â¾Ã®ÃÃ¶ÃÃ¶ Â¾ÃŠÂ´Ã‚Â´Ã™.
		if (m_pClientList[iClientH]->m_bIsNeutral != true) {
			_PenaltyItemDrop(iClientH, cPenaltyLevel, bIsSAattacked);
		}
	}
}

int CGame::iAddDynamicObjectList(short sOwner, char cOwnerType, short sType, char cMapIndex, short sX, short sY, UINT32 dwLastTime, int iV1, int iV2, int iV3)
{
 int i;
 short sPreType;
 UINT32 dwTime, dwRegisterTime;

	m_pMapList[cMapIndex]->bGetDynamicObject(sX, sY, &sPreType, &dwRegisterTime);
	if (sPreType != 0) return 0;

	switch (sType) {	
	case DEF_DYNAMICOBJECT_FIRE3:
	case DEF_DYNAMICOBJECT_FIRE:
		if (m_pMapList[cMapIndex]->bGetIsMoveAllowedTile(sX, sY) == false)
			return 0;
		if (dwLastTime != 0)  {
			switch (m_pMapList[cMapIndex]->m_cWhetherStatus) {
			case 1:	dwLastTime = dwLastTime - (dwLastTime / 2);       break;
			case 2:	dwLastTime = (dwLastTime / 2) - (dwLastTime / 3); break;
			case 3:	dwLastTime = (dwLastTime / 3) - (dwLastTime / 4); break;
			// SNOOPY: Added reduced duration if snow whether
			case 4:	dwLastTime = dwLastTime - (dwLastTime / 6);       break;
			case 5:	dwLastTime = dwLastTime - (dwLastTime / 4);		  break;
			case 6:	dwLastTime = dwLastTime - (dwLastTime / 2);		  break;
			default:break;
			}
		}
		else dwLastTime = 1000;
		break;

	// SNOOPY: Increased duration if snow whether
	case DEF_DYNAMICOBJECT_ICESTORM:
		if (dwLastTime != 0)  {
			switch (m_pMapList[cMapIndex]->m_cWhetherStatus) {
			case 1:	break;
			// Reduced duration if heavy rain
			case 2:	dwLastTime = dwLastTime  - (dwLastTime / 8); break;
			case 3:	dwLastTime = dwLastTime  - (dwLastTime / 4); break;
			// Increased duration if snow whether
			case 4:	dwLastTime = dwLastTime + (dwLastTime / 2);       break;
			case 5:	dwLastTime = dwLastTime *2;		  break;
			case 6:	dwLastTime = dwLastTime *4;		  break;
			default:break;
			}
		}
		else dwLastTime = 1000;
		break;
	
	case DEF_DYNAMICOBJECT_FISHOBJECT:
	case DEF_DYNAMICOBJECT_FISH:
		if (m_pMapList[cMapIndex]->bGetIsWater(sX, sY) == false)
			return 0;
		break;

	case DEF_DYNAMICOBJECT_ARESDENFLAG1:
	case DEF_DYNAMICOBJECT_ELVINEFLAG1:
	case DEF_DYNAMICOBJECT_MINERAL1:
	case DEF_DYNAMICOBJECT_MINERAL2:
		if (m_pMapList[cMapIndex]->bGetMoveable(sX, sY) == false)
			return 0;
		m_pMapList[cMapIndex]->SetTempMoveAllowedFlag(sX, sY, false);
		break;

	}
   	
	for (i = 1; i < DEF_MAXDYNAMICOBJECTS; i++) 
	if (m_pDynamicObjectList[i] == 0) {
		dwTime = timeGetTime();
		
		if (dwLastTime != 0) 
			dwLastTime += (iDice(1,4)*1000);

		m_pDynamicObjectList[i] = new class CDynamicObject(sOwner, cOwnerType, sType, cMapIndex, sX, sY, dwTime, dwLastTime, iV1, iV2, iV3);
		m_pMapList[cMapIndex]->SetDynamicObject(i, sType, sX, sY, dwTime);
		
		SendEventToNearClient_TypeB(MSGID_DYNAMICOBJECT, DEF_MSGTYPE_CONFIRM, cMapIndex, sX, sY, sType, i, 0, (short)0);

		return i;
	}
	return 0;
}

void CGame::CheckDynamicObjectList()
{
 int i;
 UINT32 dwTime = timeGetTime(), dwRegisterTime;
 short sType;

	// Â³Â¯Â¾Â¾ ÃˆÂ¿Â°ÃºÂ¿Â¡ ÂµÃ»Â¸Â¥ ÂµÂ¿Ã€Ã» Â°Â´ÃƒÂ¼Ã€Ã‡ ÃÃ¶Â¼Ã“Â½ÃƒÂ°Â£ Â´ÃœÃƒÃ Ã€Â» Â°Ã¨Â»ÃªÃ‡Ã‘Â´Ã™. 
	for (i = 1; i < DEF_MAXDYNAMICOBJECTS; i++)  
	{	if ( (m_pDynamicObjectList[i] != 0) && (m_pDynamicObjectList[i]->m_dwLastTime != 0) ) 
		{	switch (m_pDynamicObjectList[i]->m_sType) {
			case DEF_DYNAMICOBJECT_FIRE3:
			case DEF_DYNAMICOBJECT_FIRE:
				switch (m_pMapList[m_pDynamicObjectList[i]->m_cMapIndex]->m_cWhetherStatus) {
				case 0: 
					break;
				case 1: 
				case 2:
				case 3:
					m_pDynamicObjectList[i]->m_dwLastTime = m_pDynamicObjectList[i]->m_dwLastTime - (m_pDynamicObjectList[i]->m_dwLastTime/10) * m_pMapList[m_pDynamicObjectList[i]->m_cMapIndex]->m_cWhetherStatus;
					break;
				case 4: 	// SNOOPY: Added snow whether
				case 5:
				case 6:
					m_pDynamicObjectList[i]->m_dwLastTime = m_pDynamicObjectList[i]->m_dwLastTime - (m_pDynamicObjectList[i]->m_dwLastTime/20) * m_pMapList[m_pDynamicObjectList[i]->m_cMapIndex]->m_cWhetherStatus;
					break;
				}
				break;
	}	}	}

	// Â½ÃƒÂ°Â£Ã€ÃŒ Â¸Â¸Â·Ã¡ÂµÃˆ Â°Â´ÃƒÂ¼Â¸Â¦ ÃÂ¾Â·Ã¡Â½ÃƒÃ…Â²Â´Ã™. ÃÃ¶Â¼Ã“Â½ÃƒÂ°Â£Ã€ÃŒ NULLÃ€ÃŒÂ¸Ã© Â¿ÂµÂ¿Ã¸ÃˆÃ· Â³Â²Â¾Ã†Ã€Ã–Â´Ã‚ ÂµÂ¿Ã€Ã» Â°Â´ÃƒÂ¼Ã€ÃŒÂ´Ã™.
	for (i = 1; i < DEF_MAXDYNAMICOBJECTS; i++) { 
		if ( (m_pDynamicObjectList[i] != 0) && (m_pDynamicObjectList[i]->m_dwLastTime != 0) && 
			 ((dwTime - m_pDynamicObjectList[i]->m_dwRegisterTime) >= m_pDynamicObjectList[i]->m_dwLastTime) ) {
		
			// ÃÃ¶Â¼Ã“Â½ÃƒÂ°Â£Ã€ÃŒ Â°Ã¦Â°ÃºÃ‡Ã‘ ÂµÂ¿Ã€Ã» Â°Â´ÃƒÂ¼Â¸Â¦ ÃƒÂ£Â¾Ã’Â´Ã™.
			m_pMapList[ m_pDynamicObjectList[i]->m_cMapIndex ]->bGetDynamicObject( m_pDynamicObjectList[i]->m_sX, m_pDynamicObjectList[i]->m_sY, &sType, &dwRegisterTime );
			// ÂµÃ®Â·ÃÂ½ÃƒÂ°Â£Ã€ÃŒ Ã€ÃÃ„Â¡Ã‡Ã‘Â´Ã™Â¸Ã© Â°Â´ÃƒÂ¼Â°Â¡ Â»Ã§Â¶Ã³ÃÃ¸Â´Ã™Â´Ã‚ Â¸ÃžÂ½ÃƒÃÃ¶Â¸Â¦ ÂºÂ¸Â³Â»ÃÃ Â¾ÃŸ Ã‡Ã‘Â´Ã™.
			
			if (dwRegisterTime == m_pDynamicObjectList[i]->m_dwRegisterTime) {
				SendEventToNearClient_TypeB(MSGID_DYNAMICOBJECT, DEF_MSGTYPE_REJECT, m_pDynamicObjectList[i]->m_cMapIndex, m_pDynamicObjectList[i]->m_sX, m_pDynamicObjectList[i]->m_sY, m_pDynamicObjectList[i]->m_sType, i, 0, (short)0);
				// Â¸ÃŠÂ¿Â¡Â¼Â­ Â»Ã¨ÃÂ¦Ã‡Ã‘Â´Ã™.
				m_pMapList[m_pDynamicObjectList[i]->m_cMapIndex]->SetDynamicObject(0, 0, m_pDynamicObjectList[i]->m_sX, m_pDynamicObjectList[i]->m_sY, dwTime);
			}

			switch (sType) {
			case DEF_DYNAMICOBJECT_FISHOBJECT:
			case DEF_DYNAMICOBJECT_FISH:
				// Â¹Â°Â°Ã­Â±Ã¢ Â¿Ã€ÂºÃªÃÂ§Ã†Â®Â°Â¡ Â»Ã§Â¶Ã³ÃÂ³Ã€Â¸Â¹Ã‡Â·ÃŽ ÃƒÂ³Â¸Â®Ã‡Ã‘Â´Ã™.
				bDeleteFish(m_pDynamicObjectList[i]->m_sOwner, 2); // Â½ÃƒÂ°Â£Ã€ÃŒ ÃÃ¶Â³ÂªÂ¼Â­ Â¹Â°Â°Ã­Â±Ã¢Â°Â¡ Â»Ã§Â¶Ã³ÃÃ¸Â´Ã™.
				break;
			}
	
			// Â¸Â®Â½ÂºÃ†Â®Â¸Â¦ Â»Ã¨ÃÂ¦Ã‡Ã‘Â´Ã™.
			delete m_pDynamicObjectList[i];
			m_pDynamicObjectList[i] = 0;
		}
	}
}

void CGame::RequestFullObjectData(int iClientH, char *pData)
{
 UINT32 * dwp;
 UINT16  * wp, wObjectID;
 char  * cp, cData[100];
 short * sp, sX, sY;
 int     sTemp, sTemp2;
 int   * ip, iRet;

	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;

	wp = (UINT16 *)(pData + DEF_INDEX2_MSGTYPE);
	wObjectID = *wp;
	
	ZeroMemory(cData, sizeof(cData));
	dwp  = (UINT32 *)(cData + DEF_INDEX4_MSGID);
	*dwp = MSGID_EVENT_MOTION;
	wp   = (UINT16 *)(cData + DEF_INDEX2_MSGTYPE);
	*wp  = DEF_OBJECTSTOP;
	
	cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);

	if (wObjectID < 10000) {
		// Ã„Â³Â¸Â¯Ã…ÃÃ€Ã‡ ÃÂ¤ÂºÂ¸Â¸Â¦ Â¿Ã¸Ã‡Ã‘Â´Ã™. 
		// Ã€ÃŸÂ¸Ã¸ÂµÃˆ Ã€ÃŽÂµÂ¦Â½ÂºÂ°ÂªÃ€ÃŒÂ°Ã…Â³Âª ÃÂ¸Ã€Ã§Ã‡ÃÃÃ¶ Â¾ÃŠÂ´Ã‚ Ã‡ÃƒÂ·Â¹Ã€ÃŒÂ¾Ã®Â¶Ã³Â¸Ã© Â¹Â«Â½Ãƒ.
		if ( (wObjectID == 0) || (wObjectID >= DEF_MAXCLIENTS) ) return;
		if (m_pClientList[wObjectID] == 0) return;
		
		wp  = (UINT16 *)cp;
		*wp = wObjectID;			// ObjectID
		cp += 2;
		sp  = (short *)cp;
		sX  = m_pClientList[wObjectID]->m_sX;
		*sp = sX;
		cp += 2;
		sp  = (short *)cp;
		sY  = m_pClientList[wObjectID]->m_sY;
		*sp = sY;
		cp += 2;
		sp  = (short *)cp;
		*sp = m_pClientList[wObjectID]->m_sType;
		cp += 2;
		*cp = m_pClientList[wObjectID]->m_cDir;
		cp++;
		memcpy(cp, m_pClientList[wObjectID]->m_cCharName, 10);
		cp += 10;
		sp  = (short *)cp;
		*sp = m_pClientList[wObjectID]->m_sAppr1;
		cp += 2;
		sp  = (short *)cp;
		*sp = m_pClientList[wObjectID]->m_sAppr2;
		cp += 2;
		sp  = (short *)cp;
		*sp = m_pClientList[wObjectID]->m_sAppr3;
		cp += 2;
		sp  = (short *)cp;
		*sp = m_pClientList[wObjectID]->m_sAppr4;
		cp += 2;
		//v1.4 ApprColor
		ip = (int *)cp;
		*ip = m_pClientList[wObjectID]->m_iApprColor;
		cp += 4;
		
		ip  = (int *)cp;
		
		// sStatusÃ€Ã‡ Â»Ã³Ã€Â§ 4ÂºÃ±Ã†Â®Â°Â¡ FOE Â°Ã¼Â°Ã¨Â¸Â¦ Â³ÂªÃ…Â¸Â³Â½Â´Ã™. 
		sTemp = m_pClientList[wObjectID]->m_iStatus;
		sTemp = 0x0FFFFFFF & sTemp;// Â»Ã³Ã€Â§ 4ÂºÃ±Ã†Â® Ã…Â¬Â¸Â®Â¾Ã®
		sTemp2 = iGetPlayerABSStatus(wObjectID, iClientH); 
		sTemp  = (sTemp | (sTemp2 << 28));//Original : 12
		
		*ip = sTemp;
		
		cp += 4;//Original 2
		
		if (m_pClientList[wObjectID]->m_bIsKilled == true) // v1.4
			 *cp = 1;
		else *cp = 0;
		cp++;

		iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 41); // v1.4
	}
	else {
		// NPCÃ€Ã‡ ÃÂ¤ÂºÂ¸Â¸Â¦ Â¿Ã¸Ã‡Ã‘Â´Ã™.
		// Ã€ÃŸÂ¸Ã¸ÂµÃˆ Ã€ÃŽÂµÂ¦Â½Âº Â°ÂªÃ€ÃŒÂ°Ã…Â³Âª Â»Ã½Â¼ÂºÂµÃ‡ÃÃ¶ Â¾ÃŠÃ€Âº NPCÂ¶Ã³Â¸Ã© Â¹Â«Â½Ãƒ 
		if ( ((wObjectID - 10000) == 0) || ((wObjectID - 10000) >= DEF_MAXNPCS) ) return;
		if (m_pNpcList[wObjectID - 10000] == 0) return;
		
		wp  = (UINT16 *)cp;
		*wp = wObjectID;			
		cp += 2;

		wObjectID -= 10000;

		sp  = (short *)cp;
		sX  = m_pNpcList[wObjectID]->m_sX;
		*sp = sX;
		cp += 2;
		sp  = (short *)cp;
		sY  = m_pNpcList[wObjectID]->m_sY;
		*sp = sY;
		cp += 2;
		sp  = (short *)cp;
		*sp = m_pNpcList[wObjectID]->m_sType;
		cp += 2;
		*cp = m_pNpcList[wObjectID]->m_cDir;
		cp++;
		memcpy(cp, m_pNpcList[wObjectID]->m_cName, 5);
		cp += 5;
		sp  = (short *)cp;
		*sp = m_pNpcList[wObjectID]->m_sAppr2;
		cp += 2;
		
		ip  = (int *)cp;
	
		sTemp = m_pNpcList[wObjectID]->m_iStatus;
		sTemp = 0x0FFFFFFF & sTemp;// Â»Ã³Ã€Â§ 4ÂºÃ±Ã†Â® Ã…Â¬Â¸Â®Â¾Ã®
		
		sTemp2 = iGetNpcRelationship(wObjectID, iClientH);
		sTemp  = (sTemp | (sTemp2 << 28));//Original : 12	
		*ip = sTemp;
		
		cp += 4;//Original 2

		if (m_pNpcList[wObjectID]->m_bIsKilled == true) // v1.4
			 *cp = 1;
		else *cp = 0;
		cp++;

		iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 27); // v1.4 //Original : 25
	}

	switch (iRet) {
	case DEF_XSOCKEVENT_QUENEFULL:
	case DEF_XSOCKEVENT_SOCKETERROR:
	case DEF_XSOCKEVENT_CRITICALERROR:
	case DEF_XSOCKEVENT_SOCKETCLOSED:
		// Â¸ÃžÂ½ÃƒÃÃ¶Â¸Â¦ ÂºÂ¸Â³Â¾Â¶Â§ Â¿Â¡Â·Â¯Â°Â¡ Â¹ÃŸÂ»Ã½Ã‡ÃŸÂ´Ã™Â¸Ã© ÃÂ¦Â°Ã…Ã‡Ã‘Â´Ã™.
		DeleteClient(iClientH, true, true);
		break;
	}
}

void CGame::Effect_Damage_Spot(short sAttackerH, char cAttackerType, short sTargetH, char cTargetType, short sV1, short sV2, short sV3, bool bExp, int iAttr)
{
int iPartyID, iDamage, iSideCondition, iIndex, iRemainLife, iTemp, iMaxSuperAttack, iRepDamage, iExtraDmg;
 char cAttackerSide, cDamageMoveDir, cDamageMod[256];
 UINT32 dwTime;
 float dTmp1, dTmp2, dTmp3;
 short sAtkX, sAtkY, sTgtX, sTgtY, dX, dY, sItemIndex;
 UINT32 iExp;

 	if (cAttackerType == DEF_OWNERTYPE_PLAYER)
		if (m_pClientList[sAttackerH] == 0) return;

	if (cAttackerType == DEF_OWNERTYPE_NPC)
		if (m_pNpcList[sAttackerH] == 0) return;

	if (   (cAttackerType == DEF_OWNERTYPE_PLAYER) 
		&& (m_bIsHeldenianMode == true)				// No spell work before war has begun
		&& (m_bHeldenianWarInitiated == false)
		&& (m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex] != 0) 
		&& (m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_bIsHeldenianMap == 1)) return; 
	
	dwTime = timeGetTime();
	iDamage = (iDice(sV1, sV2) + sV3);
	if (iDamage <= 0) iDamage = 0;

	switch (cAttackerType) {
	case DEF_OWNERTYPE_PLAYER:	
		//LifeX Fix Admin Hit NPC
		if ((m_bAdminSecurity == true) && (m_pClientList[sAttackerH]->m_iAdminUserLevel > 0 && m_pClientList[sAttackerH]->m_iAdminUserLevel < 7))
		{
			return;
		}

		if (m_pClientList[sAttackerH]->IsInsideTeam())
		{
			if (m_pClientList[sAttackerH]->iteam == m_pClientList[sTargetH]->iteam) return;
		}	

		if (m_pClientList[sAttackerH]->m_cHeroArmourBonus > 0) iDamage += m_pClientList[sAttackerH]->m_cHeroArmourBonus;
		if ((m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_LHAND] == -1) || (m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND] == -1)) {
			sItemIndex = m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND];
			if ((sItemIndex != -1) && (m_pClientList[sAttackerH]->m_pItemList[sItemIndex] != 0)) {
				if (m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 861 || m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 862) {
					float damageTemp = (float)iDamage;
					damageTemp *= 1.5f; // O el valor con punto flotante que ustedes dispongan... JustThink
					iDamage = (int)damageTemp;
				}

				if (m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 865 || m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 866) {
					float damageTemp = (float)iDamage;
					damageTemp *= 0.5f; // O el valor con punto flotante que ustedes dispongan... JustThink
					iDamage = (int)damageTemp;
				}

				if (m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 988) {
					float damageTemp = (float)iDamage;
					damageTemp *= 2.0f; // O el valor con punto flotante que ustedes dispongan... JustThink
					iDamage = (int)damageTemp;
				}

				if (m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 1005) {
					float damageTemp = (float)iDamage;
					damageTemp *= 2.1f; // O el valor con punto flotante que ustedes dispongan... JustThink
					iDamage = (int)damageTemp;
				}

				if (m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 863 || m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 864) {
					if (m_pClientList[sAttackerH]->m_iRating > 0) {
						iRepDamage = m_pClientList[sAttackerH]->m_iRating/200;
						iDamage += iRepDamage;
					}
					if (cTargetType == DEF_OWNERTYPE_PLAYER) {
						if (m_pClientList[sTargetH] != 0) {
							if (m_pClientList[sTargetH]->m_iRating < 0) {
								iRepDamage = (abs(m_pClientList[sTargetH]->m_iRating)/200);
								iDamage += iRepDamage;
							}
						}
					}
				}
			}
			sItemIndex = m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_NECK];
			if ((sItemIndex != -1) && (m_pClientList[sAttackerH]->m_pItemList[sItemIndex] != 0)) {
				if (m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 859) { // NecklaceOfKloness  
					if (cTargetType == DEF_OWNERTYPE_PLAYER) {
						if (m_pClientList[sTargetH] != 0) {
							if (m_pClientList[sTargetH]->m_iRating < 0) {
								iRepDamage = (abs(m_pClientList[sTargetH]->m_iRating) / 200);
								iDamage += iRepDamage;
							}
						}
					}
				}
			}
		}

		if ((m_bIsCrusadeMode == false) && (m_pClientList[sAttackerH]->m_bIsPlayerCivil == true) && (cTargetType == DEF_OWNERTYPE_PLAYER)) return;

		dTmp1 = (float)iDamage;
		if ((m_pClientList[sAttackerH]->m_iMag) <= 0)
			 dTmp2 = 1.0f;
		else dTmp2 = (float)(m_pClientList[sAttackerH]->m_iMag);	
		dTmp2 = dTmp2 / 3.3f;
		dTmp3 = dTmp1 + (dTmp1 * (dTmp2 / 100.0f));
		iDamage = (int)(dTmp3 +0.5f);

		iDamage += m_pClientList[sAttackerH]->m_iAddMagicalDamage;
		if (iDamage <= 0) iDamage = 0;

		if (m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_bIsFightZone == true) 
			iDamage += iDamage/3;

		if (bCheckHeldenianMap(sAttackerH, DEF_OWNERTYPE_PLAYER) == 1) {
			iDamage += iDamage/3;
		}

		//Magn0S:: Extra Dmg Map Restriction
		if (m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->bMapBonusDmg == true) {
			iDamage += iDamage / 2;
		}

		//Magn0S:: Extra Dmg Map Restriction
		if (m_iServerMagDmg > 0) {
			iExtraDmg = iDamage / 20;
			iDamage += iExtraDmg * m_iServerMagDmg;
		}

		if ((cTargetType == DEF_OWNERTYPE_PLAYER) && (m_bIsCrusadeMode == true) && (m_pClientList[sAttackerH]->m_iCrusadeDuty == 1)) {
			if (m_pClientList[sAttackerH]->m_iLevel <= 80) {
				iDamage += (iDamage * 7) / 10;
			}
			else if (m_pClientList[sAttackerH]->m_iLevel > 80 && m_pClientList[sAttackerH]->m_iLevel <= 100) {
				iDamage += iDamage / 2;
			}
			else if (m_pClientList[sAttackerH]->m_iLevel > 100) {
				iDamage += iDamage / 3;
			}
		}
		
		cAttackerSide = m_pClientList[sAttackerH]->m_cSide;
		sAtkX = m_pClientList[sAttackerH]->m_sX;
		sAtkY = m_pClientList[sAttackerH]->m_sY;
		iPartyID = m_pClientList[sAttackerH]->m_iPartyID;
		break;

	case DEF_OWNERTYPE_NPC:
		cAttackerSide = m_pNpcList[sAttackerH]->m_cSide;
		sAtkX = m_pNpcList[sAttackerH]->m_sX;
		sAtkY = m_pNpcList[sAttackerH]->m_sY;
		break;
	}

	switch (cTargetType) {
	case DEF_OWNERTYPE_PLAYER:

		if (m_pClientList[sTargetH] == 0) return;
		if (m_pClientList[sTargetH]->m_bIsInitComplete == false) return;
		if (m_pClientList[sTargetH]->m_bIsKilled == true) return;

		if ((m_pClientList[sTargetH]->m_iStatus & 0x400000) != 0) return;

		if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_bIsCrusadeMode == false) && 
			(m_pClientList[sTargetH]->m_iPKCount == 0) && (m_pClientList[sTargetH]->m_bIsPlayerCivil == true)) return;

		if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sTargetH]->m_bIsNeutral == true) && 
			(m_pClientList[sTargetH]->m_iPKCount == 0) && (m_pClientList[sTargetH]->m_bIsOwnLocation == true)) return;
		
		if ((dwTime - m_pClientList[sTargetH]->m_dwTime) > DEF_RAGPROTECTIONTIME) return;
		if ((m_pMapList[m_pClientList[sTargetH]->m_cMapIndex]->m_bIsAttackEnabled == false) && (m_pClientList[sTargetH]->m_iAdminUserLevel == 0)) return;
		if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH]->m_bIsNeutral == true) && (m_pClientList[sTargetH]->m_iPKCount == 0)) return;
		if ((m_pClientList[sTargetH]->m_iPartyID != 0) && (iPartyID == m_pClientList[sTargetH]->m_iPartyID)) return;
		m_pClientList[sTargetH]->m_dwLogoutHackCheck = dwTime;

		if (cAttackerType == DEF_OWNERTYPE_PLAYER) { 
			if (m_pClientList[sAttackerH]->m_bIsSafeAttackMode == true) {
				iSideCondition = iGetPlayerRelationship(sAttackerH, sTargetH);
				if ((iSideCondition != 7) && (iSideCondition != 2) && (iSideCondition != 6)) {
					if (m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_bIsFightZone == true) {
						if (m_pClientList[sAttackerH]->m_iGuildGUID == m_pClientList[sTargetH]->m_iGuildGUID) {
							return;
						}
					}
					return;
				}
			}
			//Magn0S:: Put self safe for bmages.
			if ((m_pClientList[sAttackerH]->m_bIsSelfSafe == true) && (m_pClientList[sTargetH] == m_pClientList[sAttackerH]))
				return;

			if (m_pMapList[m_pClientList[sTargetH]->m_cMapIndex]->iGetAttribute(m_pClientList[sTargetH]->m_sX, m_pClientList[sTargetH]->m_sY, 0x00000005) != 0) return;
		}

		ClearSkillUsingStatus(sTargetH);
		
		switch (iAttr) {
		case 1:
			if (m_pClientList[sTargetH]->m_iAddAbsEarth != 0) {
				dTmp1 = (float)iDamage;
				dTmp2 = (float)m_pClientList[sTargetH]->m_iAddAbsEarth;
				dTmp3 = (dTmp2/100.0f)*dTmp1;
				iDamage = iDamage - (int)(dTmp3);
				if (iDamage < 0) iDamage = 0;
			}
			break;

		case 2:
			if (m_pClientList[sTargetH]->m_iAddAbsLight != 0) {
				dTmp1 = (float)iDamage;
				dTmp2 = (float)m_pClientList[sTargetH]->m_iAddAbsLight;
				dTmp3 = (dTmp2/100.0f)*dTmp1;
				iDamage = iDamage - (int)(dTmp3);
				if (iDamage < 0) iDamage = 0;
			}
			break;

		case 3:
			if (m_pClientList[sTargetH]->m_iAddAbsFire != 0) {
				dTmp1 = (float)iDamage;
				dTmp2 = (float)m_pClientList[sTargetH]->m_iAddAbsFire;
				dTmp3 = (dTmp2/100.0f)*dTmp1;
				iDamage = iDamage - (int)(dTmp3);
				if (iDamage < 0) iDamage = 0;
			}
			break;

		case 4:
			if (m_pClientList[sTargetH]->m_iAddAbsWater != 0) {
				dTmp1 = (float)iDamage;
				dTmp2 = (float)m_pClientList[sTargetH]->m_iAddAbsWater;
				dTmp3 = (dTmp2/100.0f)*dTmp1;
				iDamage = iDamage - (int)(dTmp3);
				if (iDamage < 0) iDamage = 0;
			}
			break;

		//Magn0S: New Elements
		case 5:
			if (m_pClientList[sTargetH]->m_iAddAbsHoly != 0) {
				dTmp1 = (float)iDamage;
				dTmp2 = (float)m_pClientList[sTargetH]->m_iAddAbsHoly;
				dTmp3 = (dTmp2 / 100.0f) * dTmp1;
				iDamage = iDamage - (int)(dTmp3);
				if (iDamage < 0) iDamage = 0;
			}
			break;

		case 6:
			if (m_pClientList[sTargetH]->m_iAddAbsUnholy != 0) {
				dTmp1 = (float)iDamage;
				dTmp2 = (float)m_pClientList[sTargetH]->m_iAddAbsUnholy;
				dTmp3 = (dTmp2 / 100.0f) * dTmp1;
				iDamage = iDamage - (int)(dTmp3);
				if (iDamage < 0) iDamage = 0;
			}
			break;

		default: break;
		}

		iIndex = m_pClientList[sTargetH]->m_iMagicDamageSaveItemIndex;
		if ((iIndex != -1) && (iIndex >= 0) && (iIndex < DEF_MAXITEMS)) {
			
			switch (m_pClientList[sTargetH]->m_pItemList[iIndex]->m_sIDnum) {
			case 335:
				dTmp1 = (float)iDamage;
				dTmp2 = dTmp1 * 0.2f;
				dTmp3 = dTmp1 - dTmp2;
				iDamage = (int)(dTmp3 +0.5f);
				break;
						
			case 337:
				dTmp1 = (float)iDamage;
				dTmp2 = dTmp1 * 0.1f;
				dTmp3 = dTmp1 - dTmp2;
				iDamage = (int)(dTmp3 +0.5f);
				break;
			}
			if (iDamage <= 0) iDamage = 0;
			
			iRemainLife = m_pClientList[sTargetH]->m_pItemList[iIndex]->m_wCurLifeSpan;			
			if (iRemainLife <= iDamage) {
				ItemDepleteHandler(sTargetH, iIndex, true, true);
			}
			else {
				m_pClientList[sTargetH]->m_pItemList[iIndex]->m_wCurLifeSpan -= iDamage;
				SendNotifyMsg(0, sTargetH, DEF_NOTIFY_CURLIFESPAN, iIndex, m_pClientList[sTargetH]->m_pItemList[iIndex]->m_wCurLifeSpan, 0, 0);
			}
		}

		if (m_pClientList[sTargetH]->m_iAddAbsMD != 0) {
			dTmp1 = (float)iDamage;
			dTmp2 = (float)m_pClientList[sTargetH]->m_iAddAbsMD;
			dTmp3 = (dTmp2/100.0f)*dTmp1;
			iDamage = iDamage - (int)dTmp3;
		}

		if (cTargetType == DEF_OWNERTYPE_PLAYER) {
			iDamage -= (iDice(1, m_pClientList[sTargetH]->m_iVit/10) - 1);
			if (iDamage <= 0) iDamage = 0;
		}
	
		if ((m_pClientList[sTargetH]->m_bIsLuckyEffect == true) && 
			(iDice(1,10) == 5) && (m_pClientList[sTargetH]->m_iHP <= iDamage)) {
			iDamage = m_pClientList[sTargetH]->m_iHP - 1;
		}

		if (m_pClientList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT] == 2)
			iDamage = iDamage / 2;

		if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sTargetH]->m_bIsSpecialAbilityEnabled == true)) {
			switch (m_pClientList[sTargetH]->m_iSpecialAbilityType) {
			case 60: // Bloody Armors
				if (m_pClientList[sAttackerH]->m_iHP <= iDamage / 10) {
					iDamage = m_pClientList[sAttackerH]->m_iHP - 1;
				}
				m_pClientList[sAttackerH]->m_iHP -= iDamage / 10;
				SendNotifyMsg(0, sAttackerH, DEF_NOTIFY_HP, 0, 0, 0, 0);
				break;
			case 51:
			case 52:
				return;
			}
		}
		
		m_pClientList[sTargetH]->m_iHP -= iDamage;

		//Magn0S:: Add the code showdmg
		if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH] != 0)) {
			if (m_pClientList[sAttackerH]->bShowDmg == true) {
				ZeroMemory(cDamageMod, sizeof(cDamageMod));
				wsprintf(cDamageMod, "You did %d Damage to %s", iDamage, m_pClientList[sTargetH]->m_cCharName);
				SendNotifyMsg(0, sAttackerH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, cDamageMod);
			}
		}

		if (m_pClientList[sTargetH]->m_iHP <= 0) {
			ClientKilledHandler(sTargetH, sAttackerH, cAttackerType, iDamage);	
		}
		else {
			if (iDamage > 0) {
				if (m_pClientList[sTargetH]->m_iAddTransMana > 0) {
					dTmp1 = (float)m_pClientList[sTargetH]->m_iAddTransMana;
					dTmp2 = (float)iDamage;
					dTmp3 = (dTmp1/100.0f)*dTmp2 +1.0f;

					iTemp = iGetMaxMP(sTargetH);
					m_pClientList[sTargetH]->m_iMP += (int)dTmp3;
					if (m_pClientList[sTargetH]->m_iMP > iTemp) m_pClientList[sTargetH]->m_iMP = iTemp;
				}

				if (m_pClientList[sTargetH]->m_iAddChargeCritical > 0) {
					if (iDice(1,100) <= (m_pClientList[sTargetH]->m_iAddChargeCritical)) {
						iMaxSuperAttack = ((m_pClientList[sTargetH]->m_iLevel) / 10);
						if (m_pClientList[sTargetH]->m_iSuperAttackLeft < iMaxSuperAttack) m_pClientList[sTargetH]->m_iSuperAttackLeft++;
						SendNotifyMsg(0, sTargetH, DEF_NOTIFY_SUPERATTACKLEFT, 0, 0, 0, 0);
					}
				}

				m_pClientList[sTargetH]->m_dwRecentAttackTime = dwTime;
				
				SendNotifyMsg(0, sTargetH, DEF_NOTIFY_HP, 0, 0, 0, 0);
				SendEventToNearClient_TypeA(sTargetH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTDAMAGE, iDamage, 0, 0);

				if (m_pClientList[sTargetH]->m_bSkillUsingStatus[19] != true) {
					m_pMapList[m_pClientList[sTargetH]->m_cMapIndex]->ClearOwner(0, sTargetH, DEF_OWNERTYPE_PLAYER, m_pClientList[sTargetH]->m_sX, m_pClientList[sTargetH]->m_sY);
					m_pMapList[m_pClientList[sTargetH]->m_cMapIndex]->SetOwner(sTargetH, DEF_OWNERTYPE_PLAYER, m_pClientList[sTargetH]->m_sX, m_pClientList[sTargetH]->m_sY);
				}
			
				if (m_pClientList[sTargetH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_HOLDOBJECT ] != 0) {
					SendNotifyMsg(0, sTargetH, DEF_NOTIFY_MAGICEFFECTOFF, DEF_MAGICTYPE_HOLDOBJECT, m_pClientList[sTargetH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_HOLDOBJECT ], 0, 0);							
					m_pClientList[sTargetH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_HOLDOBJECT ] = 0;
					bRemoveFromDelayEventList(sTargetH, DEF_OWNERTYPE_PLAYER, DEF_MAGICTYPE_HOLDOBJECT);
				}
			}
		}

		sTgtX = m_pClientList[sTargetH]->m_sX;
		sTgtY = m_pClientList[sTargetH]->m_sY;
		break;

	case DEF_OWNERTYPE_NPC:
		if (m_pNpcList[sTargetH] == 0) return;
		if (m_pNpcList[sTargetH]->m_iHP <= 0) return;
		if ((m_bIsCrusadeMode == true) && (cAttackerSide == m_pNpcList[sTargetH]->m_cSide)) return;
		
		sTgtX = m_pNpcList[sTargetH]->m_sX;
		sTgtY = m_pNpcList[sTargetH]->m_sY;

		switch (m_pNpcList[sTargetH]->m_cActionLimit) {
		case 1:
		case 2:
			return;

		case 4:
			if (sTgtX == sAtkX) {
				if (sTgtY == sAtkY) return;
				else if (sTgtY > sAtkY) cDamageMoveDir = 5;
				else if (sTgtY < sAtkY) cDamageMoveDir = 1;
			}
			else if (sTgtX > sAtkX) {
				if (sTgtY == sAtkY)     cDamageMoveDir = 3;
				else if (sTgtY > sAtkY) cDamageMoveDir = 4;
				else if (sTgtY < sAtkY) cDamageMoveDir = 2;
			}
			else if (sTgtX < sAtkX) {
				if (sTgtY == sAtkY)     cDamageMoveDir = 7;
				else if (sTgtY > sAtkY) cDamageMoveDir = 6;
				else if (sTgtY < sAtkY) cDamageMoveDir = 8;							
			}

			dX = m_pNpcList[sTargetH]->m_sX + _tmp_cTmpDirX[cDamageMoveDir];
			dY = m_pNpcList[sTargetH]->m_sY + _tmp_cTmpDirY[cDamageMoveDir];

			if (m_pMapList[m_pNpcList[sTargetH]->m_cMapIndex]->bGetMoveable(dX, dY, 0) == false) {
				cDamageMoveDir = iDice(1,8);
				dX = m_pNpcList[sTargetH]->m_sX + _tmp_cTmpDirX[cDamageMoveDir];
				dY = m_pNpcList[sTargetH]->m_sY + _tmp_cTmpDirY[cDamageMoveDir];			
				if (m_pMapList[m_pNpcList[sTargetH]->m_cMapIndex]->bGetMoveable(dX, dY, 0) == false) return;
			}
						
			m_pMapList[m_pNpcList[sTargetH]->m_cMapIndex]->ClearOwner(5, sTargetH, DEF_OWNERTYPE_NPC, m_pNpcList[sTargetH]->m_sX, m_pNpcList[sTargetH]->m_sY);
			m_pMapList[m_pNpcList[sTargetH]->m_cMapIndex]->SetOwner(sTargetH, DEF_OWNERTYPE_NPC, dX, dY);
			m_pNpcList[sTargetH]->m_sX   = dX;
			m_pNpcList[sTargetH]->m_sY   = dY;
			m_pNpcList[sTargetH]->m_cDir = cDamageMoveDir;
					
			SendEventToNearClient_TypeA(sTargetH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTMOVE, 0, 0, 0);
				
			dX = m_pNpcList[sTargetH]->m_sX + _tmp_cTmpDirX[cDamageMoveDir];
			dY = m_pNpcList[sTargetH]->m_sY + _tmp_cTmpDirY[cDamageMoveDir];
					
			if (m_pMapList[m_pNpcList[sTargetH]->m_cMapIndex]->bGetMoveable(dX, dY, 0) == false) {
				cDamageMoveDir = iDice(1,8);
				dX = m_pNpcList[sTargetH]->m_sX + _tmp_cTmpDirX[cDamageMoveDir];
				dY = m_pNpcList[sTargetH]->m_sY + _tmp_cTmpDirY[cDamageMoveDir];
						
				if (m_pMapList[m_pNpcList[sTargetH]->m_cMapIndex]->bGetMoveable(dX, dY, 0) == false) return;
			}
						
			m_pMapList[m_pNpcList[sTargetH]->m_cMapIndex]->ClearOwner(5, sTargetH, DEF_OWNERTYPE_NPC, m_pNpcList[sTargetH]->m_sX, m_pNpcList[sTargetH]->m_sY);
			m_pMapList[m_pNpcList[sTargetH]->m_cMapIndex]->SetOwner(sTargetH, DEF_OWNERTYPE_NPC, dX, dY);
			m_pNpcList[sTargetH]->m_sX   = dX;
			m_pNpcList[sTargetH]->m_sY   = dY;
			m_pNpcList[sTargetH]->m_cDir = cDamageMoveDir;
					
			SendEventToNearClient_TypeA(sTargetH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTMOVE, 0, 0, 0);

			return;
		}

		if (cAttackerType == DEF_OWNERTYPE_PLAYER) {
			switch (m_pNpcList[sTargetH]->m_sType) {
			case 40:
			case 41:
				if ((m_pClientList[sAttackerH]->m_cSide == 0) || (m_pNpcList[sTargetH]->m_cSide == m_pClientList[sAttackerH]->m_cSide)) return;
				break;
			}
		}

		switch (m_pNpcList[sTargetH]->m_sType) {
				case 67: // McGaffin
				case 68: // Perry
				case 69: // Devlin
				return;
		}
		
		if (m_pNpcList[sTargetH]->m_iAbsDamage > 0) {
			dTmp1 = (float)iDamage;
			dTmp2 = (float)(m_pNpcList[sTargetH]->m_iAbsDamage)/100.0f;
			dTmp3 = dTmp1 * dTmp2;
			dTmp2 = dTmp1 - dTmp3;
			iDamage = (int)dTmp2;
			if (iDamage < 0) iDamage = 1;
		}

		if (m_pNpcList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT] == 2)
			iDamage = iDamage / 2;
		
		m_pNpcList[sTargetH]->m_iHP -= iDamage;
		//Magn0S:: Add the code showdmg
		if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH] != 0)) {
			if (m_pClientList[sAttackerH]->bShowDmg == true) {
				ZeroMemory(cDamageMod, sizeof(cDamageMod));
				wsprintf(cDamageMod, "You did %d Damage to %s", iDamage, m_pNpcList[sTargetH]->m_cNpcName);
				SendNotifyMsg(0, sAttackerH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, cDamageMod);
			}
		}

		if (m_pNpcList[sTargetH]->m_iHP <= 0) {
			NpcKilledHandler(sAttackerH, cAttackerType, sTargetH, iDamage);
		}
		else {
			switch (cAttackerType) {
			case DEF_OWNERTYPE_PLAYER:
				if ((m_pNpcList[sTargetH]->m_sType != 21) && (m_pNpcList[sTargetH]->m_sType != 55) && (m_pNpcList[sTargetH]->m_sType != 56)
					&& (m_pNpcList[sTargetH]->m_cSide == cAttackerSide)) return ;
				break;

			case DEF_OWNERTYPE_NPC:
				if (m_pNpcList[sAttackerH]->m_cSide == m_pNpcList[sTargetH]->m_cSide) return;
				break;
			}
			
			SendEventToNearClient_TypeA(sTargetH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTDAMAGE, iDamage, 0, 0);
						
			if ((iDice(1,3) == 2) && (m_pNpcList[sTargetH]->m_cActionLimit == 0)) {
				if ((cAttackerType == DEF_OWNERTYPE_NPC) && 
					(m_pNpcList[sAttackerH]->m_sType == m_pNpcList[sTargetH]->m_sType) &&
					(m_pNpcList[sAttackerH]->m_cSide == m_pNpcList[sTargetH]->m_cSide)) return;

				m_pNpcList[sTargetH]->m_cBehavior          = DEF_BEHAVIOR_ATTACK;
				m_pNpcList[sTargetH]->m_sBehaviorTurnCount = 0;		
				m_pNpcList[sTargetH]->m_iTargetIndex = sAttackerH;
				m_pNpcList[sTargetH]->m_cTargetType  = cAttackerType;

				m_pNpcList[sTargetH]->m_dwTime = dwTime;

				if (m_pNpcList[sTargetH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_HOLDOBJECT ] != 0) {
					m_pNpcList[sTargetH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_HOLDOBJECT ] = 0;
					bRemoveFromDelayEventList(sTargetH, DEF_OWNERTYPE_NPC, DEF_MAGICTYPE_HOLDOBJECT);
				}

				if ( (m_pNpcList[sTargetH]->m_iNoDieRemainExp > 0) && (m_pNpcList[sTargetH]->m_bIsSummoned != true) && 
					 (cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH] != 0) ) {
					if (m_pNpcList[sTargetH]->m_iNoDieRemainExp > iDamage) {
						iExp = iDamage;

						if (m_pClientList[sAttackerH]->m_iAddExp > 0) {
							dTmp1 = (float)m_pClientList[sAttackerH]->m_iAddExp;
							dTmp2 = (float)iExp;
							dTmp3 = (dTmp1/100.0f)*dTmp2;
							iExp += (UINT32)dTmp3;
						}

						if (m_pClientList[sAttackerH]->m_iLevel > 100 ) {
							switch (m_pNpcList[sTargetH]->m_sType) {
							case 55:
							case 56:
								iExp = 0;
								break;
							default: break;
							}
						}
											
						if (bExp == true) 
							 GetExp(sAttackerH, iExp, true);
						else GetExp(sAttackerH, (iExp/2), true);
						m_pNpcList[sTargetH]->m_iNoDieRemainExp -= iDamage;
					}
					else {
						iExp = m_pNpcList[sTargetH]->m_iNoDieRemainExp;

						if (m_pClientList[sAttackerH]->m_iAddExp > 0) {
							dTmp1 = (float)m_pClientList[sAttackerH]->m_iAddExp;
							dTmp2 = (float)iExp;
							dTmp3 = (dTmp1/100.0f)*dTmp2;
							iExp += (UINT32)dTmp3;
						}

						if (m_pClientList[sAttackerH]->m_iLevel > 100 ) {
							switch (m_pNpcList[sTargetH]->m_sType) {
							case 55:
							case 56:
								iExp = 0;
								break;
							default: break;
							}
						}
						
						if (bExp == true) 
							 GetExp(sAttackerH, iExp, true);
						else GetExp(sAttackerH, (iExp/2), true);
						m_pNpcList[sTargetH]->m_iNoDieRemainExp = 0;
					}
				}
			}
		}
		break;
	}
}

void CGame::Effect_Damage_Spot_Type2(short sAttackerH, char cAttackerType, short sTargetH, char cTargetType, short sAtkX, short sAtkY, short sV1, short sV2, short sV3, bool bExp, int iAttr)
{
 int iPartyID, iDamage, iSideCondition, iIndex, iRemainLife, iTemp, iMaxSuperAttack, iExtraDmg, iRepDamage;
 char cAttackerSide, cDamageMoveDir, cDamageMinimum, cDamageMod[256];
 UINT32 dwTime;
 float dTmp1, dTmp2, dTmp3;
 short sTgtX, sTgtY, sItemIndex;
 UINT32 iExp;

	if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH] == 0)) return;
	if ((cAttackerType == DEF_OWNERTYPE_NPC) && (m_pNpcList[sAttackerH] == 0)) return;
	
	if (   (cAttackerType == DEF_OWNERTYPE_PLAYER) 
		&& (m_bIsHeldenianMode == true)				// No spell work before war has begun
		&& (m_bHeldenianWarInitiated == false)
		&& (m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex] != 0) 
		&& (m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_bIsHeldenianMap == 1)) return; 
	
	dwTime = timeGetTime();
	sTgtX = 0;
	sTgtY = 0;
	iDamage = (iDice(sV1, sV2) + sV3);
	if (iDamage <= 0) iDamage = 0;

	switch (cAttackerType) {
	case DEF_OWNERTYPE_PLAYER:
		//LifeX Fix Admin Hit NPC
		if ((m_bAdminSecurity == true) && (m_pClientList[sAttackerH]->m_iAdminUserLevel > 0 && m_pClientList[sAttackerH]->m_iAdminUserLevel < 7))
		{
			return;
		}

		if (m_pClientList[sAttackerH]->IsInsideTeam())
		{
			if (m_pClientList[sAttackerH]->iteam == m_pClientList[sTargetH]->iteam) return;
		}
		if (m_pClientList[sAttackerH]->m_cHeroArmourBonus > 0) iDamage += m_pClientList[sAttackerH]->m_cHeroArmourBonus;
		if ((m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_LHAND] == -1) || (m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND] == -1)) {
			sItemIndex = m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND];
			if ((sItemIndex != -1) && (m_pClientList[sAttackerH]->m_pItemList[sItemIndex] != 0)) {
				if (m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 861 || m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 862) {
					float damageTemp = (float)iDamage;
					damageTemp *= 1.5f; // O el valor con punto flotante que ustedes dispongan... JustThink
					iDamage = (int)damageTemp;
				}
				if (m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 865 || m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 866) {
					float damageTemp = (float)iDamage;
					damageTemp *= 0.5f; // O el valor con punto flotante que ustedes dispongan... JustThink
					iDamage = (int)damageTemp;
				}

				if (m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 988) {
					float damageTemp = (float)iDamage;
					damageTemp *= 2.0f; // O el valor con punto flotante que ustedes dispongan... JustThink
					iDamage = (int)damageTemp;
				}

				if (m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 1005) {
					float damageTemp = (float)iDamage;
					damageTemp *= 2.1f; // O el valor con punto flotante que ustedes dispongan... JustThink
					iDamage = (int)damageTemp;
				}
				if (m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 863 || m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 864) {
					if (m_pClientList[sAttackerH]->m_iRating > 0) {
						iRepDamage = m_pClientList[sAttackerH]->m_iRating/200;
						iDamage += iRepDamage;
					}
					if (cTargetType == DEF_OWNERTYPE_PLAYER) {
						if (m_pClientList[sTargetH] != 0) {
							if (m_pClientList[sTargetH]->m_iRating < 0) {
								iRepDamage = (abs(m_pClientList[sTargetH]->m_iRating)/200);
								iDamage += iRepDamage;
							}
						}
					}
				}
			}
			sItemIndex = m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_NECK];
			if ((sItemIndex != -1) && (m_pClientList[sAttackerH]->m_pItemList[sItemIndex] != 0)) {
				if (m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 859) { // NecklaceOfKloness  
					if (cTargetType == DEF_OWNERTYPE_PLAYER) {
						if (m_pClientList[sTargetH] != 0) {
							if (m_pClientList[sTargetH]->m_iRating < 0) {
								iRepDamage = (abs(m_pClientList[sTargetH]->m_iRating) / 200);
								iDamage += iRepDamage;
							}
						}
					}
				}
			}
		}

		if ((m_bIsCrusadeMode == false) && (m_pClientList[sAttackerH]->m_bIsPlayerCivil == true) && (cTargetType == DEF_OWNERTYPE_PLAYER)) return;

		dTmp1 = (float)iDamage;
		if ((m_pClientList[sAttackerH]->m_iMag) <= 0)
			 dTmp2 = 1.0f;
		else dTmp2 = (float)(m_pClientList[sAttackerH]->m_iMag);	
		dTmp2 = dTmp2 / 3.3f;
		dTmp3 = dTmp1 + (dTmp1 * (dTmp2 / 100.0f));
		iDamage = (int)(dTmp3 +0.5f);
		if (iDamage <= 0) iDamage = 0;
		// order switched with above
		iDamage += m_pClientList[sAttackerH]->m_iAddMagicalDamage;

		if (m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_bIsFightZone == true) 
			iDamage += iDamage/3;

		if (bCheckHeldenianMap(sAttackerH, DEF_OWNERTYPE_PLAYER) == 1) {
			iDamage += iDamage/3;
		}

		//Magn0S:: Extra Dmg Map Restriction
		if (m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->bMapBonusDmg == true) {
			iDamage += iDamage / 2;
		}

		//Magn0S:: Extra Dmg Map Restriction
		if (m_iServerMagDmg > 0) {
			iExtraDmg = iDamage / 20;
			iDamage += iExtraDmg * m_iServerMagDmg;
		}

		if ((cTargetType == DEF_OWNERTYPE_PLAYER) && (m_bIsCrusadeMode == true) && (m_pClientList[sAttackerH]->m_iCrusadeDuty == 1)) {
			if (m_pClientList[sAttackerH]->m_iLevel <= 80) {
				iDamage += (iDamage * 7) / 10;
			}
			else if (m_pClientList[sAttackerH]->m_iLevel > 80 && m_pClientList[sAttackerH]->m_iLevel <= 100) {
				iDamage += iDamage / 2;
			}
			else if (m_pClientList[sAttackerH]->m_iLevel > 100) {
				iDamage += iDamage / 3;
			}
		}
		
		cAttackerSide = m_pClientList[sAttackerH]->m_cSide;
		iPartyID = m_pClientList[sAttackerH]->m_iPartyID;
		break;

	case DEF_OWNERTYPE_NPC:
		cAttackerSide = m_pNpcList[sAttackerH]->m_cSide;
		break;
	}

	switch (cTargetType) {
	case DEF_OWNERTYPE_PLAYER:

		if (m_pClientList[sTargetH] == 0) return;
		if (m_pClientList[sTargetH]->m_bIsInitComplete == false) return;
		if (m_pClientList[sTargetH]->m_bIsKilled == true) return;
		if ((dwTime - m_pClientList[sTargetH]->m_dwTime) > DEF_RAGPROTECTIONTIME) return;
		if (m_pClientList[sTargetH]->m_cMapIndex == -1) return;
		if ((m_pMapList[m_pClientList[sTargetH]->m_cMapIndex]->m_bIsAttackEnabled == false) && (m_pClientList[sTargetH]->m_iAdminUserLevel == 0)) return;
		if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sTargetH]->m_bIsNeutral == true) && (m_pClientList[sTargetH]->m_iPKCount == 0)) return;

		if ((m_pClientList[sTargetH]->m_iStatus & 0x400000) != 0) return;

		if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_bIsCrusadeMode == false) && 
			(m_pClientList[sTargetH]->m_iPKCount == 0) && (m_pClientList[sTargetH]->m_bIsPlayerCivil == true)) return;

		if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sTargetH]->m_bIsNeutral == true) && 
			(m_pClientList[sTargetH]->m_iPKCount == 0) && (m_pClientList[sTargetH]->m_bIsOwnLocation == true)) return;
		
		if ((m_pClientList[sTargetH]->m_iPartyID != 0) && (iPartyID == m_pClientList[sTargetH]->m_iPartyID)) return;
		m_pClientList[sTargetH]->m_dwLogoutHackCheck = dwTime;

		if (cAttackerType == DEF_OWNERTYPE_PLAYER) { 
			if (m_pClientList[sAttackerH]->m_bIsSafeAttackMode == true) {
				iSideCondition = iGetPlayerRelationship(sAttackerH, sTargetH);
				if ((iSideCondition != 7) && (iSideCondition != 2) && (iSideCondition != 6)) {
					if (m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_bIsFightZone == true) {
						if (m_pClientList[sAttackerH]->m_iGuildGUID == m_pClientList[sTargetH]->m_iGuildGUID) {
							return;
						}
					}
					return;
				}
			}
			//Magn0S:: Put self safe for bmages.
			if ((m_pClientList[sAttackerH]->m_bIsSelfSafe == true) && (m_pClientList[sTargetH] == m_pClientList[sAttackerH]))
				return;

			if (m_pMapList[m_pClientList[sTargetH]->m_cMapIndex]->iGetAttribute(m_pClientList[sTargetH]->m_sX, m_pClientList[sTargetH]->m_sY, 0x00000005) != 0) return;
		}

		ClearSkillUsingStatus(sTargetH);
		
		switch (iAttr) {
		case 1:
			if (m_pClientList[sTargetH]->m_iAddAbsEarth != 0) {
				dTmp1 = (float)iDamage;
				dTmp2 = (float)m_pClientList[sTargetH]->m_iAddAbsEarth;
				dTmp3 = (dTmp2/100.0f)*dTmp1;
				iDamage = iDamage - (int)(dTmp3);
				if (iDamage < 0) iDamage = 0;
			}
			break;

		case 2:
			if (m_pClientList[sTargetH]->m_iAddAbsLight != 0) {
				dTmp1 = (float)iDamage;
				dTmp2 = (float)m_pClientList[sTargetH]->m_iAddAbsLight;
				dTmp3 = (dTmp2/100.0f)*dTmp1;
				iDamage = iDamage - (int)(dTmp3);
				if (iDamage < 0) iDamage = 0;
			}
			break;

		case 3:
			if (m_pClientList[sTargetH]->m_iAddAbsFire != 0) {
				dTmp1 = (float)iDamage;
				dTmp2 = (float)m_pClientList[sTargetH]->m_iAddAbsFire;
				dTmp3 = (dTmp2/100.0f)*dTmp1;
				iDamage = iDamage - (int)(dTmp3);
				if (iDamage < 0) iDamage = 0;
			}
			break;

		case 4:
			if (m_pClientList[sTargetH]->m_iAddAbsWater != 0) {
				dTmp1 = (float)iDamage;
				dTmp2 = (float)m_pClientList[sTargetH]->m_iAddAbsWater;
				dTmp3 = (dTmp2/100.0f)*dTmp1;
				iDamage = iDamage - (int)(dTmp3);
				if (iDamage < 0) iDamage = 0;
			}
			break;

		//Magn0S: New Elements
		case 5:
			if (m_pClientList[sTargetH]->m_iAddAbsHoly != 0) {
				dTmp1 = (float)iDamage;
				dTmp2 = (float)m_pClientList[sTargetH]->m_iAddAbsHoly;
				dTmp3 = (dTmp2 / 100.0f) * dTmp1;
				iDamage = iDamage - (int)(dTmp3);
				if (iDamage < 0) iDamage = 0;
			}
			break;

		case 6:
			if (m_pClientList[sTargetH]->m_iAddAbsUnholy != 0) {
				dTmp1 = (float)iDamage;
				dTmp2 = (float)m_pClientList[sTargetH]->m_iAddAbsUnholy;
				dTmp3 = (dTmp2 / 100.0f) * dTmp1;
				iDamage = iDamage - (int)(dTmp3);
				if (iDamage < 0) iDamage = 0;
			}
			break;

		default: break;
		}

		iIndex = m_pClientList[sTargetH]->m_iMagicDamageSaveItemIndex;
		if ((iIndex != -1) && (iIndex >= 0) && (iIndex < DEF_MAXITEMS)) {
			
			switch (m_pClientList[sTargetH]->m_pItemList[iIndex]->m_sIDnum) {
			case 335:
				dTmp1 = (float)iDamage;
				dTmp2 = dTmp1 * 0.2f;
				dTmp3 = dTmp1 - dTmp2;
				iDamage = (int)(dTmp3 +0.5f);
				break;
						
			case 337:
				dTmp1 = (float)iDamage;
				dTmp2 = dTmp1 * 0.1f;
				dTmp3 = dTmp1 - dTmp2;
				iDamage = (int)(dTmp3 +0.5f);
				break;
			}
			if (iDamage <= 0) iDamage = 0;
			
			iRemainLife = m_pClientList[sTargetH]->m_pItemList[iIndex]->m_wCurLifeSpan;			
			if (iRemainLife <= iDamage) {
				ItemDepleteHandler(sTargetH, iIndex, true, true);
			}
			else {
				m_pClientList[sTargetH]->m_pItemList[iIndex]->m_wCurLifeSpan -= iDamage;
				SendNotifyMsg(0, sTargetH, DEF_NOTIFY_CURLIFESPAN, iIndex, m_pClientList[sTargetH]->m_pItemList[iIndex]->m_wCurLifeSpan, 0, 0);
			}
		}

		if (m_pClientList[sTargetH]->m_iAddAbsMD != 0) {
			dTmp1 = (float)iDamage;
			dTmp2 = (float)m_pClientList[sTargetH]->m_iAddAbsMD;
			dTmp3 = (dTmp2/100.0f)*dTmp1;
			iDamage = iDamage - (int)dTmp3;
		}

		if (cTargetType == DEF_OWNERTYPE_PLAYER) {
			iDamage -= (iDice(1, m_pClientList[sTargetH]->m_iVit/10) - 1);
			if (iDamage <= 0) iDamage = 0;
		}
	
		if (m_pClientList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT] == 2)
			iDamage = iDamage / 2;
		
		if (m_pClientList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT] == 5) break;

		if ((m_pClientList[sTargetH]->m_bIsLuckyEffect == true) && 
			(iDice(1,10) == 5) && (m_pClientList[sTargetH]->m_iHP <= iDamage)) {
			iDamage = m_pClientList[sTargetH]->m_iHP - 1;
		}

		if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sTargetH]->m_bIsSpecialAbilityEnabled == true)) {
			switch (m_pClientList[sTargetH]->m_iSpecialAbilityType) {
			case 60: // Bloody Armors
				if (m_pClientList[sAttackerH]->m_iHP <= iDamage / 10) {
					iDamage = m_pClientList[sAttackerH]->m_iHP - 1;
				}
				m_pClientList[sAttackerH]->m_iHP -= iDamage / 10;
				SendNotifyMsg(0, sAttackerH, DEF_NOTIFY_HP, 0, 0, 0, 0);
				break;
			case 51:
			case 52:
				return;
			}
		}
		
		m_pClientList[sTargetH]->m_iHP -= iDamage;

		//Magn0S:: Add the code showdmg
		if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH] != 0)) {
			if (m_pClientList[sAttackerH]->bShowDmg == true) {
				ZeroMemory(cDamageMod, sizeof(cDamageMod));
				wsprintf(cDamageMod, "You did %d Damage to %s", iDamage, m_pClientList[sTargetH]->m_cCharName);
				SendNotifyMsg(0, sAttackerH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, cDamageMod);
			}
		}

		if (m_pClientList[sTargetH]->m_iHP <= 0) {
			ClientKilledHandler(sTargetH, sAttackerH, cAttackerType, iDamage);	
		}
		else {
			if (iDamage > 0) {
				if (m_pClientList[sTargetH]->m_iAddTransMana > 0) {
					dTmp1 = (float)m_pClientList[sTargetH]->m_iAddTransMana;
					dTmp2 = (float)iDamage;
					dTmp3 = (dTmp1/100.0f)*dTmp2 +1.0f;

					iTemp = iGetMaxMP(sTargetH);
					m_pClientList[sTargetH]->m_iMP += (int)dTmp3;
					if (m_pClientList[sTargetH]->m_iMP > iTemp) m_pClientList[sTargetH]->m_iMP = iTemp;
				}

				if (m_pClientList[sTargetH]->m_iAddChargeCritical > 0) {
					if (iDice(1,100) <= (m_pClientList[sTargetH]->m_iAddChargeCritical)) {
						iMaxSuperAttack = ((m_pClientList[sTargetH]->m_iLevel) / 10);
						if (m_pClientList[sTargetH]->m_iSuperAttackLeft < iMaxSuperAttack) m_pClientList[sTargetH]->m_iSuperAttackLeft++;
						SendNotifyMsg(0, sTargetH, DEF_NOTIFY_SUPERATTACKLEFT, 0, 0, 0, 0);
					}
				}
				
				if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_bIsFightZone == true)) {
					cDamageMinimum = 80;
				}
				else {
					cDamageMinimum = 50;
				}

				if (iDamage >= cDamageMinimum) {
					sTgtX = m_pClientList[sTargetH]->m_sX;
					sTgtY = m_pClientList[sTargetH]->m_sY;
					if (sTgtX == sAtkX) {
						if (sTgtY == sAtkY) return;
							else if (sTgtY > sAtkY) cDamageMoveDir = 5;
							else if (sTgtY < sAtkY) cDamageMoveDir = 1;
						}
						else if (sTgtX > sAtkX) {
							if (sTgtY == sAtkY)     cDamageMoveDir = 3;
							else if (sTgtY > sAtkY) cDamageMoveDir = 4;
							else if (sTgtY < sAtkY) cDamageMoveDir = 2;
						}
					else if (sTgtX < sAtkX) {
						if (sTgtY == sAtkY)     cDamageMoveDir = 7;
						else if (sTgtY > sAtkY) cDamageMoveDir = 6;
						else if (sTgtY < sAtkY) cDamageMoveDir = 8;							
					}
					m_pClientList[sTargetH]->m_iLastDamage = iDamage;
					SendNotifyMsg(0, sTargetH, DEF_NOTIFY_HP, 0, 0, 0, 0);
					SendNotifyMsg(0, sTargetH, DEF_NOTIFY_DAMAGEMOVE, cDamageMoveDir, iDamage, 0, 0);
				}

				SendNotifyMsg(0, sTargetH, DEF_NOTIFY_HP, 0, 0, 0, 0);
				SendEventToNearClient_TypeA(sTargetH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTDAMAGE, iDamage, 0, 0);

				if (m_pClientList[sTargetH]->m_bSkillUsingStatus[19] != true) {
					m_pMapList[m_pClientList[sTargetH]->m_cMapIndex]->ClearOwner(0, sTargetH, DEF_OWNERTYPE_PLAYER, m_pClientList[sTargetH]->m_sX, m_pClientList[sTargetH]->m_sY);
					m_pMapList[m_pClientList[sTargetH]->m_cMapIndex]->SetOwner(sTargetH, DEF_OWNERTYPE_PLAYER, m_pClientList[sTargetH]->m_sX, m_pClientList[sTargetH]->m_sY);
				}
			
				if (m_pClientList[sTargetH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_HOLDOBJECT ] != 0) {
					SendNotifyMsg(0, sTargetH, DEF_NOTIFY_MAGICEFFECTOFF, DEF_MAGICTYPE_HOLDOBJECT, m_pClientList[sTargetH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_HOLDOBJECT ], 0, 0);							
					m_pClientList[sTargetH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_HOLDOBJECT ] = 0;
					bRemoveFromDelayEventList(sTargetH, DEF_OWNERTYPE_PLAYER, DEF_MAGICTYPE_HOLDOBJECT);
				}
			}
		}
		break;

	case DEF_OWNERTYPE_NPC:
		if (m_pNpcList[sTargetH] == 0) return;
		if (m_pNpcList[sTargetH]->m_iHP <= 0) return;
		if ((m_bIsCrusadeMode == true) && (cAttackerSide == m_pNpcList[sTargetH]->m_cSide)) return;
		
		switch (m_pNpcList[sTargetH]->m_cActionLimit) {
		case 1:
		case 2:
		case 4:
		case 6:
			return;

		case 3:
		case 5:
			if (cAttackerType == DEF_OWNERTYPE_PLAYER) {
				switch (m_pNpcList[sTargetH]->m_sType) {
				case 40:
				case 41:
					if ((m_pClientList[sAttackerH]->m_cSide == 0) || (m_pNpcList[sTargetH]->m_cSide == m_pClientList[sAttackerH]->m_cSide)) return;
					break;
				}
			}
		}
	
		if (m_pNpcList[sTargetH]->m_iAbsDamage > 0) {
			dTmp1 = (float)iDamage;
			dTmp2 = (float)(m_pNpcList[sTargetH]->m_iAbsDamage)/100.0f;
			dTmp3 = dTmp1 * dTmp2;
			dTmp2 = dTmp1 - dTmp3;
			iDamage = (int)dTmp2;
			if (iDamage < 0) iDamage = 1;
		}

		if (m_pNpcList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT] == 2)
			iDamage = iDamage / 2;
		
		m_pNpcList[sTargetH]->m_iHP -= iDamage;

		//Magn0S:: Add the code showdmg
		if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH] != 0)) {
			if (m_pClientList[sAttackerH]->bShowDmg == true) {
				ZeroMemory(cDamageMod, sizeof(cDamageMod));
				wsprintf(cDamageMod, "You did %d Damage to %s", iDamage, m_pNpcList[sTargetH]->m_cNpcName);
				SendNotifyMsg(0, sAttackerH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, cDamageMod);
			}
		}

		if (m_pNpcList[sTargetH]->m_iHP <= 0) {
			NpcKilledHandler(sAttackerH, cAttackerType, sTargetH, iDamage);
		}
		else {
			switch (cAttackerType) {
			case DEF_OWNERTYPE_PLAYER:
				if ((m_pNpcList[sTargetH]->m_sType != 21) && (m_pNpcList[sTargetH]->m_sType != 55) && (m_pNpcList[sTargetH]->m_sType != 56)
					&& (m_pNpcList[sTargetH]->m_cSide == cAttackerSide)) return ;
				break;

			case DEF_OWNERTYPE_NPC:
				if (m_pNpcList[sAttackerH]->m_cSide == m_pNpcList[sTargetH]->m_cSide) return;
				break;
			}
			
			SendEventToNearClient_TypeA(sTargetH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTDAMAGE, iDamage, 0, 0);		
			if ((iDice(1,3) == 2) && (m_pNpcList[sTargetH]->m_cActionLimit == 0)) {
				if ((cAttackerType == DEF_OWNERTYPE_NPC) && 
					(m_pNpcList[sAttackerH]->m_sType == m_pNpcList[sTargetH]->m_sType) &&
					(m_pNpcList[sAttackerH]->m_cSide == m_pNpcList[sTargetH]->m_cSide)) return;

				m_pNpcList[sTargetH]->m_cBehavior          = DEF_BEHAVIOR_ATTACK;
				m_pNpcList[sTargetH]->m_sBehaviorTurnCount = 0;		
				m_pNpcList[sTargetH]->m_iTargetIndex = sAttackerH;
				m_pNpcList[sTargetH]->m_cTargetType  = cAttackerType;
				m_pNpcList[sTargetH]->m_dwTime = dwTime;

				if (m_pNpcList[sTargetH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_HOLDOBJECT ] != 0) {
					m_pNpcList[sTargetH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_HOLDOBJECT ] = 0;
					bRemoveFromDelayEventList(sTargetH, DEF_OWNERTYPE_NPC, DEF_MAGICTYPE_HOLDOBJECT);
				}

				if ((m_pNpcList[sTargetH]->m_iNoDieRemainExp > 0) && (m_pNpcList[sTargetH]->m_bIsSummoned != true) && (cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH] != 0)) {
					if (m_pNpcList[sTargetH]->m_iNoDieRemainExp > iDamage) {
						iExp = iDamage;

						if (m_pClientList[sAttackerH]->m_iAddExp > 0) {
							dTmp1 = (float)m_pClientList[sAttackerH]->m_iAddExp;
							dTmp2 = (float)iExp;
							dTmp3 = (dTmp1/100.0f)*dTmp2;
							iExp += (UINT32)dTmp3;
						}

						if (m_pClientList[sAttackerH]->m_iLevel > 100 ) {
							switch (m_pNpcList[sTargetH]->m_sType) {
							case 55:
							case 56:
								iExp = 0;
								break;
							default: break;
							}
						}
											
						if (bExp == true) 
							 GetExp(sAttackerH, iExp, true);
						else GetExp(sAttackerH, (iExp/2), true);
						m_pNpcList[sTargetH]->m_iNoDieRemainExp -= iDamage;
					}
					else {
						iExp = m_pNpcList[sTargetH]->m_iNoDieRemainExp;

						if (m_pClientList[sAttackerH]->m_iAddExp > 0) {
							dTmp1 = (float)m_pClientList[sAttackerH]->m_iAddExp;
							dTmp2 = (float)iExp;
							dTmp3 = (dTmp1/100.0f)*dTmp2;
							iExp += (UINT32)dTmp3;
						}

						if (m_pClientList[sAttackerH]->m_iLevel > 100 ) {
							switch (m_pNpcList[sTargetH]->m_sType) {
							case 55:
							case 56:
								iExp = 0;
								break;
							default: break;
							}
						}
						
						if (bExp == true) 
							 GetExp(sAttackerH, iExp, true);
						else GetExp(sAttackerH, (iExp/2), true);
						m_pNpcList[sTargetH]->m_iNoDieRemainExp = 0;
					}
				}
			}
		}
		break;
	}
}

void CGame::Effect_Damage_Spot_DamageMove(short sAttackerH, char cAttackerType, short sTargetH, char cTargetType, short sAtkX, short sAtkY, short sV1, short sV2, short sV3, bool bExp, int iAttr)
{
 int iDamage, iSideCondition, iIndex, iRemainLife, iTemp, iExtraDmg, iMaxSuperAttack;
 UINT32 dwTime, wWeaponType;
 char cAttackerSide, cDamageMoveDir, cDamageMod[256];
 float dTmp1, dTmp2, dTmp3;
 int iPartyID, iMoveDamage;
 short sTgtX, sTgtY;
 //Crusade
 UINT32 iExp;

	if (cAttackerType == DEF_OWNERTYPE_PLAYER)
		if (m_pClientList[sAttackerH] == 0) return;

	if (cAttackerType == DEF_OWNERTYPE_NPC)
		if (m_pNpcList[sAttackerH] == 0) return;

	if (   (cAttackerType == DEF_OWNERTYPE_PLAYER) 
		&& (m_bIsHeldenianMode == true)				// No spell work before war has begun
		&& (m_bHeldenianWarInitiated == false)
		&& (m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex] != 0) 
		&& (m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_bIsHeldenianMap == 1)) return; 

	dwTime = timeGetTime();
	sTgtX = 0;
	sTgtY = 0;
	
	// ÇØ´ç Å¸ÄÏ¿¡°Ô ´ë¹ÌÁö¸¦ ¸ÔÀÎ´Ù. 
	iDamage = (iDice(sV1, sV2) + sV3) ;
	if (iDamage <= 0) iDamage = 0;

	iPartyID = 0;

	// °ø°ÝÀÚ°¡ ÇÃ·¹ÀÌ¾î¶ó¸é Mag¿¡ µû¸¥ º¸³Ê½º ´ë¹ÌÁö¸¦ °¡»ê 
	switch (cAttackerType) {
	case DEF_OWNERTYPE_PLAYER:

		dTmp1 = (float)iDamage;
		if ((m_pClientList[sAttackerH]->m_iMag) <= 0)
			 dTmp2 = 1.0f;
		else dTmp2 = (float)(m_pClientList[sAttackerH]->m_iMag);
			
		dTmp2 = dTmp2 / 3.3f;
		dTmp3 = dTmp1 + (dTmp1 * (dTmp2 / 100.0f));
		iDamage = (int)(dTmp3 +0.5f);
		if (iDamage <= 0) iDamage = 0;

		// v1.432 2001 4 7 13 7
		iDamage += m_pClientList[sAttackerH]->m_iAddMagicalDamage;

		// v1.44 »çÅõÀåÀÌ¸é ´ë¹ÌÁö 1.33¹è 
		if (m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_bIsFightZone == true) 
			iDamage += iDamage/3;

		// Crusade : Àü¸éÀü ¸ðµåÀÏ¶§ ´ëÀÎ °ø°Ý·Â 1.33¹è 
		if ((cTargetType == DEF_OWNERTYPE_PLAYER) && (m_bIsCrusadeMode == true) && (m_pClientList[sAttackerH]->m_iCrusadeDuty == 1))
		{
			// v2.15 Àú·¦ÀÇ °æ¿ì´Â µ¥¹ÌÁö°¡ Áõ°¡ÇÑ´Ù. 1.7 ¹è 
			if (m_pClientList[sAttackerH]->m_iLevel <= 80)
			{
				iDamage += (iDamage * 7) / 10;
			} // v2.15 Àú·¦ÀÇ °æ¿ì´Â µ¥¹ÌÁö°¡ Áõ°¡ÇÑ´Ù. 1.5 ¹è 
			else if (m_pClientList[sAttackerH]->m_iLevel > 80 && m_pClientList[sAttackerH]->m_iLevel <= 100)
			{
				iDamage += iDamage / 2;
			}
			else if (m_pClientList[sAttackerH]->m_iLevel > 100)
			{
				iDamage += iDamage / 3;
			}
		}

		if (m_pClientList[sAttackerH]->m_cHeroArmourBonus > 0) {
			iDamage += m_pClientList[sAttackerH]->m_cHeroArmourBonus;
		}

		wWeaponType = ((m_pClientList[sAttackerH]->m_sAppr2 & 0x0FF0) >> 4);
		if (wWeaponType == 34) {
			iDamage += iDamage/3;
		}

		if (bCheckHeldenianMap(sAttackerH, DEF_OWNERTYPE_PLAYER) == 1) {
			iDamage += iDamage/3;
		}

		//Magn0S:: Extra Dmg Map Restriction
		if (m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->bMapBonusDmg == true) {
			iDamage += iDamage / 2;
		}

		//Magn0S:: Extra Dmg Map Restriction
		if (m_iServerMagDmg > 0) {
			iExtraDmg = iDamage / 20;
			iDamage += iExtraDmg * m_iServerMagDmg;
		}

		cAttackerSide = m_pClientList[sAttackerH]->m_cSide;
		
		iPartyID = m_pClientList[sAttackerH]->m_iPartyID;
		break;

	case DEF_OWNERTYPE_NPC:
		cAttackerSide = m_pNpcList[sAttackerH]->m_cSide;
		break;
	}

	switch (cTargetType) {
	case DEF_OWNERTYPE_PLAYER:
		// °ø°Ý ´ë»óÀÌ Á¸ÀçÇÏÁö ¾ÊÀ¸¸é ¸®ÅÏ 
		if (m_pClientList[sTargetH] == 0) return;
		if (m_pClientList[sTargetH]->m_bIsInitComplete == false) return;
		// ÀÌ¹Ì Á×¾î ÀÖ´Ù¸é Ã³¸® ¾ÈÇÔ.
		if (m_pClientList[sTargetH]->m_bIsKilled == true) return;
		// ·¢À¸·Î ÀÎÇØ º¸È£¸¦ ¹Þ¾Æ¾ß ÇÑ´Ù¸é 
		if ((dwTime - m_pClientList[sTargetH]->m_dwTime) > DEF_RAGPROTECTIONTIME) return;
		// °ø°ÝÀÚ°¡ À§Ä¡ÇÑ ¸ÊÀÌ °ø°Ý ºÒ°¡´É ¸ÊÀÌ¶ó¸é 
		// v2.03 ¼­¹ö ´Ù¿îµÇ¾î¼­ °íÄ§ 
		if (m_pClientList[sTargetH]->m_cMapIndex == -1) return;
		if ((m_pMapList[ m_pClientList[sTargetH]->m_cMapIndex ]->m_bIsAttackEnabled == false) && (m_pClientList[sTargetH]->m_iAdminUserLevel == 0)) return;
		// v1.41 °ø°ÝÀÚ°¡ Áß¸³ÀÌ¸é ´ëÀÎ °ø°ÝÀÌ ºÒ°¡´ÉÇÏ´Ù. 
		if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH]->m_bIsNeutral == true) && (m_pClientList[sTargetH]->m_iPKCount == 0)) return;

		// v2.172 °°ÀºÆí³¢¸®´Â ¸¶À»¿¡¼­ °ø°Ý´çÇÏÁö ¾Ê´Â´Ù. ¹üÁËÀÚ¿Í NPC´Â ¿¹¿Ü Áß¸³µµ °ø°Ý ¾ÊµÇ°Ô 
		if ((m_bIsCrusadeMode == false) && (m_pClientList[sTargetH]->m_iPKCount == 0) && (cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sTargetH]->m_bIsPlayerCivil == true)) return ;
		if ((m_bIsCrusadeMode == false) && (m_pClientList[sTargetH]->m_iPKCount == 0) && (cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH]->m_bIsPlayerCivil == true)) return ;

		if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sTargetH]->m_bIsNeutral == true) && (m_pClientList[sTargetH]->m_iPKCount == 0) && (m_pClientList[sTargetH]->m_bIsPlayerCivil == true)) return;

		// 01-12-17 °°Àº ÆÄÆ¼¿øÀÌ¸é °ø°Ý ºÒ°¡ 
		if ((m_pClientList[sTargetH]->m_iPartyID != 0) && (iPartyID == m_pClientList[sTargetH]->m_iPartyID)) return;
		m_pClientList[sTargetH]->m_dwLogoutHackCheck = dwTime;

		// ¸¸¾à °ø°ÝÀÚ°¡ ÇÃ·¹ÀÌ¾îÀÌ°í °ø°ÝÀÚ°¡ ¾ÈÀü °ø°Ý ¸ðµå¶ó¸é °ø°ÝÀÇ ÀÇ¹Ì°¡ ¾ø´Ù. 
		if (cAttackerType == DEF_OWNERTYPE_PLAYER) { 
			
			if (m_pClientList[sAttackerH]->m_bIsSafeAttackMode == true) {
				iSideCondition = iGetPlayerRelationship(sAttackerH, sTargetH);
				if ((iSideCondition != 7) && (iSideCondition != 2) && (iSideCondition != 6)) {
					// ¾Æ¹« È¿°ú ¾øÀ½. ¸¶³ª ¼Òºñ·®ÀÌ ÁÙ¾úÀ¸¹Ç·Î 
				
					// °°Àº ÆíÀÌÁö¸¸ ¸¸¾à »çÅõÀå ³»¿¡¼­ ÆíÀÌ °°´Ù¸é °ø°Ý Ã³¸®¸¦ ÇØ¾ßÇÑ´Ù.
					if (m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_bIsFightZone == true) {
						if (m_pClientList[sAttackerH]->m_iGuildGUID == m_pClientList[sTargetH]->m_iGuildGUID) {
							// »çÅõÀå ³»¿¡¼­ ´Ù¸¥ ±æµå´Ù. °ø°ÝÀÌ °¡´ÉÇÏ´Ù. 
							return;
						}
					}
					return;
				}
			}

			//Magn0S:: Put self safe for bmages.
			if ((m_pClientList[sAttackerH]->m_bIsSelfSafe == true) && (m_pClientList[sTargetH] == m_pClientList[sAttackerH]))
				return;

			// ¸¶¹ý º¸È£ È¤Àº ¾ÈÀü ¿µ¿ªÀÌ¶ó¸é °ø°Ý ¼º°ø ¸øÇÔ 
			if (m_pMapList[m_pClientList[sTargetH]->m_cMapIndex]->iGetAttribute(m_pClientList[sTargetH]->m_sX, m_pClientList[sTargetH]->m_sY, 0x00000005) != 0) return;
		}
		
		ClearSkillUsingStatus(sTargetH);


		switch (iAttr) { //<< ACA TIENE LOS ATRIBUTOS POR ELEMENTOS
		case 1:
			if (m_pClientList[sTargetH]->m_iAddAbsEarth != 0) {
				dTmp1 = (float)iDamage;
				dTmp2 = (float)m_pClientList[sTargetH]->m_iAddAbsEarth;
				dTmp3 = (dTmp2/100.0f)*dTmp1;
				iDamage = iDamage - (int)(dTmp3);
				if (iDamage < 0) iDamage = 0;
			}
			break;

		case 2:
			if (m_pClientList[sTargetH]->m_iAddAbsLight != 0) {
				dTmp1 = (float)iDamage;
				dTmp2 = (float)m_pClientList[sTargetH]->m_iAddAbsLight;
				dTmp3 = (dTmp2/100.0f)*dTmp1;
				iDamage = iDamage - (int)(dTmp3);
				if (iDamage < 0) iDamage = 0;
			}
			break;

		case 3:
			if (m_pClientList[sTargetH]->m_iAddAbsFire != 0) {
				dTmp1 = (float)iDamage;
				dTmp2 = (float)m_pClientList[sTargetH]->m_iAddAbsFire;
				dTmp3 = (dTmp2/100.0f)*dTmp1;
				iDamage = iDamage - (int)(dTmp3);
				if (iDamage < 0) iDamage = 0;
			}
			break;

		case 4:
			if (m_pClientList[sTargetH]->m_iAddAbsWater != 0) {
				dTmp1 = (float)iDamage;
				dTmp2 = (float)m_pClientList[sTargetH]->m_iAddAbsWater;
				dTmp3 = (dTmp2/100.0f)*dTmp1;
				iDamage = iDamage - (int)(dTmp3);
				if (iDamage < 0) iDamage = 0;
			}
			break;

		//Magn0S: New Elements
		case 5:
			if (m_pClientList[sTargetH]->m_iAddAbsHoly != 0) {
				dTmp1 = (float)iDamage;
				dTmp2 = (float)m_pClientList[sTargetH]->m_iAddAbsHoly;
				dTmp3 = (dTmp2 / 100.0f) * dTmp1;
				iDamage = iDamage - (int)(dTmp3);
				if (iDamage < 0) iDamage = 0;
			}
			break;

		case 6:
			if (m_pClientList[sTargetH]->m_iAddAbsUnholy != 0) {
				dTmp1 = (float)iDamage;
				dTmp2 = (float)m_pClientList[sTargetH]->m_iAddAbsUnholy;
				dTmp3 = (dTmp2 / 100.0f) * dTmp1;
				iDamage = iDamage - (int)(dTmp3);
				if (iDamage < 0) iDamage = 0;
			}
			break;

		default: break;
		}
		
		iIndex = m_pClientList[sTargetH]->m_iMagicDamageSaveItemIndex;
		if ((iIndex != -1) && (iIndex >= 0) && (iIndex < DEF_MAXITEMS)) {
			switch (m_pClientList[sTargetH]->m_pItemList[iIndex]->m_sIDnum) {
			case 335: // 20%
				dTmp1 = (float)iDamage;
				dTmp2 = dTmp1 * 0.2f;
				dTmp3 = dTmp1 - dTmp2;
				iDamage = (int)(dTmp3 +0.5f);
				break;
						
			case 337: // 10%
				dTmp1 = (float)iDamage;
				dTmp2 = dTmp1 * 0.1f;
				dTmp3 = dTmp1 - dTmp2;
				iDamage = (int)(dTmp3 +0.5f);
				break;
			}
			if (iDamage <= 0) iDamage = 0;
			
			iRemainLife = m_pClientList[sTargetH]->m_pItemList[iIndex]->m_wCurLifeSpan;			
			if (iRemainLife <= iDamage) {
				// ¸¶¹ý µ¥¹ÌÁö Àý°¨ ¾ÆÀÌÅÛÀÌ ºÎ¼­Áø´Ù. 
				ItemDepleteHandler(sTargetH, iIndex, true, true);
			}
			else {
				// ¸¶¹ý µ¥¹ÌÁö Àý°¨ ¾ÆÀÌÅÛÀÇ ¼ö¸í¸¸ ÁÙÀÎ´Ù. 
				m_pClientList[sTargetH]->m_pItemList[iIndex]->m_wCurLifeSpan -= iDamage;
				SendNotifyMsg(0, sTargetH, DEF_NOTIFY_CURLIFESPAN, iIndex, m_pClientList[sTargetH]->m_pItemList[iIndex]->m_wCurLifeSpan, 0, 0);
			}
		}

		// v1.42 Ãß°¡µÈ °íÁ¤ ¸¶¹ý ´ë¹ÌÁö Àý°¨
		if (m_pClientList[sTargetH]->m_iAddAbsMD != 0) {
			dTmp1 = (float)iDamage;
			dTmp2 = (float)m_pClientList[sTargetH]->m_iAddAbsMD;
			dTmp3 = (dTmp2/100.0f)*dTmp1;
			iDamage = iDamage - (int)dTmp3;
		}

		// v1.4 VIT¿¡ µû¸¥ ´ë¹ÌÁö °¨¼Ò 
		if (cTargetType == DEF_OWNERTYPE_PLAYER) {
			iDamage -= (iDice(1, m_pClientList[sTargetH]->m_iVit/10) - 1);
			if (iDamage <= 0) iDamage = 0;
		}

		// ¸¸¾à ¸¶¹ý º¸È£ÁßÀÌ¶ó¸é ´ë¹ÌÁö´Â 1/2 
		if (m_pClientList[sTargetH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_PROTECT ] == 2)
			iDamage = iDamage / 2;
	
		if ((m_pClientList[sTargetH]->m_bIsLuckyEffect == true) && 
			(iDice(1,10) == 5) && (m_pClientList[sTargetH]->m_iHP <= iDamage)) {
			// Çà¿îÈ¿°ú·Î Á×À½À» ¸ð¸éÇÑ´Ù.
			iDamage = m_pClientList[sTargetH]->m_iHP - 1;
		}

		// v1.432 Æ¯¼ö ´É·Â Áß ¸ðµç ´ë¹ÌÁö¸¦ ¸·´Â È¿°ú°¡ È°¼ºÈ­ µÈ °æ¿ì ´ë¹ÌÁö¸¦ ÀÔÁö ¾Ê´Â´Ù.
		if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sTargetH]->m_bIsSpecialAbilityEnabled == true)) {
			switch (m_pClientList[sTargetH]->m_iSpecialAbilityType) {
			case 60: // Bloody Armors
				if (m_pClientList[sAttackerH]->m_iHP <= iDamage / 10) {
					iDamage = m_pClientList[sAttackerH]->m_iHP - 1;
				}
				m_pClientList[sAttackerH]->m_iHP -= iDamage / 10;
				SendNotifyMsg(0, sAttackerH, DEF_NOTIFY_HP, 0, 0, 0, 0);
				break;
			case 51:
			case 52:
				// 
				return;
			}
		}

		m_pClientList[sTargetH]->m_iHP -= iDamage;

		//Magn0S:: Add the code showdmg
		if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH] != 0)) {
			if (m_pClientList[sAttackerH]->bShowDmg == true) {
				ZeroMemory(cDamageMod, sizeof(cDamageMod));
				wsprintf(cDamageMod, "You did %d Damage to %s", iDamage, m_pClientList[sTargetH]->m_cCharName);
				SendNotifyMsg(0, sAttackerH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, cDamageMod);
			}
		}

		if (m_pClientList[sTargetH]->m_iHP <= 0) {
			ClientKilledHandler(sTargetH, sAttackerH, cAttackerType, iDamage);	
		}
		else {
			if (iDamage > 0) {
				if (m_pClientList[sTargetH]->m_iAddTransMana > 0) {
					dTmp1 = (float)m_pClientList[sTargetH]->m_iAddTransMana;
					dTmp2 = (float)iDamage;
					dTmp3 = (dTmp1/100.0f)*dTmp2 +1.0f;

					iTemp = iGetMaxMP(sTargetH);
					m_pClientList[sTargetH]->m_iMP += (int)dTmp3;
					if (m_pClientList[sTargetH]->m_iMP > iTemp) m_pClientList[sTargetH]->m_iMP = iTemp;
				}

				if (m_pClientList[sTargetH]->m_iAddChargeCritical > 0) {
					if (iDice(1,100) <= (m_pClientList[sTargetH]->m_iAddChargeCritical)) {
						iMaxSuperAttack = ((m_pClientList[sTargetH]->m_iLevel) / 10);
						if (m_pClientList[sTargetH]->m_iSuperAttackLeft < iMaxSuperAttack) m_pClientList[sTargetH]->m_iSuperAttackLeft++;
						SendNotifyMsg(0, sTargetH, DEF_NOTIFY_SUPERATTACKLEFT, 0, 0, 0, 0);
					}
				}

				if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_bIsFightZone == true)) 
					 iMoveDamage = 80;
				else iMoveDamage = 50;

				if (iDamage >= iMoveDamage) {
					sTgtX = m_pClientList[sTargetH]->m_sX;
					sTgtY = m_pClientList[sTargetH]->m_sY;

					if (sTgtX == sAtkX) {
						if (sTgtY == sAtkY)     goto EDSD_SKIPDAMAGEMOVE;
						else if (sTgtY > sAtkY) cDamageMoveDir = 5;
						else if (sTgtY < sAtkY) cDamageMoveDir = 1;
					}
					else if (sTgtX > sAtkX) {
						if (sTgtY == sAtkY)     cDamageMoveDir = 3;
						else if (sTgtY > sAtkY) cDamageMoveDir = 4;
						else if (sTgtY < sAtkY) cDamageMoveDir = 2;
					}
					else if (sTgtX < sAtkX) {
						if (sTgtY == sAtkY)     cDamageMoveDir = 7;
						else if (sTgtY > sAtkY) cDamageMoveDir = 6;
						else if (sTgtY < sAtkY) cDamageMoveDir = 8;							
					}

					// v1.44 ¹Ð·Á³¯¶§ ´ë¹ÌÁö¸¦ ÀÔ·ÂÇÑ´Ù.
					m_pClientList[sTargetH]->m_iLastDamage = iDamage;
					// ¹ÞÀº ´ë¹ÌÁö¸¦ Åëº¸ÇÑ´Ù. <- HP¸¦ ±×´ë·Î ¾Ë¸°´Ù.
					SendNotifyMsg(0, sTargetH, DEF_NOTIFY_HP, 0, 0, 0, 0);
					// Æ¨°Ü ³ª°¡¶ó´Â ¸Þ½ÃÁö ÀÔ·Â 	
					SendNotifyMsg(0, sTargetH, DEF_NOTIFY_DAMAGEMOVE, cDamageMoveDir, iDamage, 0, 0);
				}
				else {
EDSD_SKIPDAMAGEMOVE:;
					// ¹ÞÀº ´ë¹ÌÁö¸¦ Åëº¸ÇÑ´Ù. <- HP¸¦ ±×´ë·Î ¾Ë¸°´Ù.
					SendNotifyMsg(0, sTargetH, DEF_NOTIFY_HP, 0, 0, 0, 0);
					// Ãæ°ÝÀ» ¹Þ¾Ò´Ù¸é Ãæ°Ýµ¿ÀÛ Àü¼Û 
					SendEventToNearClient_TypeA(sTargetH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTDAMAGE, iDamage, 0, 0);
				}

				// v1.4 Á×ÀºÃ´ÇÏ°í ÀÖ´Â °æ¿ì´Â Owner À§Ä¡¸¦ ¿Å±ä´Ù. 
				if (m_pClientList[sTargetH]->m_bSkillUsingStatus[19] != true) {
					m_pMapList[m_pClientList[sTargetH]->m_cMapIndex]->ClearOwner(0, sTargetH, DEF_OWNERTYPE_PLAYER, m_pClientList[sTargetH]->m_sX, m_pClientList[sTargetH]->m_sY);
					m_pMapList[m_pClientList[sTargetH]->m_cMapIndex]->SetOwner(sTargetH, DEF_OWNERTYPE_PLAYER, m_pClientList[sTargetH]->m_sX, m_pClientList[sTargetH]->m_sY);
				}
			
				if (m_pClientList[sTargetH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_HOLDOBJECT ] != 0) {
					// Hold-Person µÇ¾î ÀÖ¾ú´ø »óÅÂ¶ó¸é Ç®¸°´Ù. ¸¶¹ý°ø°Ý ÆÐ·²¶óÀÌÁî µÈ °Íµµ Ç®¸°´Ù.
					// 1: Hold-Person 
					// 2: Paralize
					SendNotifyMsg(0, sTargetH, DEF_NOTIFY_MAGICEFFECTOFF, DEF_MAGICTYPE_HOLDOBJECT, m_pClientList[sTargetH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_HOLDOBJECT ], 0, 0);
							
					m_pClientList[sTargetH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_HOLDOBJECT ] = 0;
					bRemoveFromDelayEventList(sTargetH, DEF_OWNERTYPE_PLAYER, DEF_MAGICTYPE_HOLDOBJECT);
				}
			}
		}
		break;

	case DEF_OWNERTYPE_NPC:
		if (m_pNpcList[sTargetH] == 0) return;
		if (m_pNpcList[sTargetH]->m_iHP <= 0) return;
		if ((m_bIsCrusadeMode == true) && (cAttackerSide == m_pNpcList[sTargetH]->m_cSide)) return;
		
		switch (m_pNpcList[sTargetH]->m_cActionLimit) {
		case 1:
		case 2:
		case 4:
			return;
		}
		
		// ÀüÀï¿ë ±¸Á¶¹°Áß ±×·£µå ¸ÅÁ÷ Á¦³×·¹ÀÌÅÍ, ¿¡³ÊÁö ½Çµå Á¦³×·¹ÀÌÅÍ´Â ¾Æ±ºÀÌ³ª Áß¸³ÀÇ °ø°Ý¿¡ ´ë¹ÌÁö¸¦ ÀÔÁö ¾Ê´Â´Ù.
		if (cAttackerType == DEF_OWNERTYPE_PLAYER) {
			switch (m_pNpcList[sTargetH]->m_sType) {
			case 40:
			case 41:
				if ((m_pClientList[sAttackerH]->m_cSide == 0) || (m_pNpcList[sTargetH]->m_cSide == m_pClientList[sAttackerH]->m_cSide)) return;
				break;
			}
		}

		switch (m_pNpcList[sTargetH]->m_sType) {
			case 67: // McGaffin
			case 68: // Perry
			case 69: // Devlin
				iDamage = 0;
			break;
		}
		
		if (m_pNpcList[sTargetH]->m_iAbsDamage > 0) {
			dTmp1 = (float)iDamage;
			dTmp2 = (float)(m_pNpcList[sTargetH]->m_iAbsDamage)/100.0f;
			dTmp3 = dTmp1 * dTmp2;
			dTmp2 = dTmp1 - dTmp3;
			iDamage = (int)dTmp2;
			if (iDamage < 0) iDamage = 1;
		}

		//Pfm
		if (m_pNpcList[sTargetH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_PROTECT ] == 2)
			iDamage = iDamage / 2;
		
		m_pNpcList[sTargetH]->m_iHP -= iDamage;

		//Magn0S:: Add the code showdmg
		if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH] != 0)) {
			if (m_pClientList[sAttackerH]->bShowDmg == true) {
				ZeroMemory(cDamageMod, sizeof(cDamageMod));
				wsprintf(cDamageMod, "You did %d Damage to %s", iDamage, m_pNpcList[sTargetH]->m_cNpcName);
				SendNotifyMsg(0, sAttackerH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, cDamageMod);
			}
		}

		if (m_pNpcList[sTargetH]->m_iHP <= 0) {
			NpcKilledHandler(sAttackerH, cAttackerType, sTargetH, iDamage);
		}
		else {
			switch (cAttackerType) {
			case DEF_OWNERTYPE_PLAYER:		
				if ((m_pNpcList[sTargetH]->m_sType != 21) && (m_pNpcList[sTargetH]->m_sType != 55) && (m_pNpcList[sTargetH]->m_sType != 56)
					&& (m_pNpcList[sTargetH]->m_cSide == cAttackerSide)) return;
				break;

			case DEF_OWNERTYPE_NPC:
				if (m_pNpcList[sAttackerH]->m_cSide == m_pNpcList[sTargetH]->m_cSide) return;
				break;
			}
			
			// Ãæ°ÝÀ» ¹Þ¾Ò´Ù¸é Ãæ°Ýµ¿ÀÛ Àü¼Û
			SendEventToNearClient_TypeA(sTargetH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTDAMAGE, iDamage, 0, 0);
			
			if ((iDice(1,3) == 2) && (m_pNpcList[sTargetH]->m_cActionLimit == 0)) {

				// µ¿Á·ÀÌ°í ÆíÀÌ °°À¸¸é ¹Ý°ÝÇÏÁö ¾Ê´Â´Ù.
				if ((cAttackerType == DEF_OWNERTYPE_NPC) && 
					(m_pNpcList[sAttackerH]->m_sType == m_pNpcList[sTargetH]->m_sType) &&
					(m_pNpcList[sAttackerH]->m_cSide == m_pNpcList[sTargetH]->m_cSide)) return;

				// ActionLimit°¡ 1ÀÌ¸é ¹Ý°ÝÀ» ÇÏÁö ¾Ê´Â´Ù. ¿ÀÁ÷ ¿òÁ÷ÀÏ¼ö¸¸ ÀÖÀ¸´Ï.
				m_pNpcList[sTargetH]->m_cBehavior          = DEF_BEHAVIOR_ATTACK;
				m_pNpcList[sTargetH]->m_sBehaviorTurnCount = 0;		
				m_pNpcList[sTargetH]->m_iTargetIndex = sAttackerH;
				m_pNpcList[sTargetH]->m_cTargetType  = cAttackerType;
				
				// ¿©±â¼­ Ç¥È¿ µ¿ÀÛ°°Àº°ÍÀ» À§ÇÑ ¸Þ½ÃÁö ¹ß¼Û.

				// Damage¸¦ ÀÔÀº Ãæ°ÝÀ¸·Î ÀÎÇÑ Áö¿¬È¿°ú.
				m_pNpcList[sTargetH]->m_dwTime = dwTime;

				if (m_pNpcList[sTargetH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_HOLDOBJECT ] != 0) {
					// Hold µÇ¾î ÀÖ¾ú´ø »óÅÂ¶ó¸é Ç®¸°´Ù. 	
					m_pNpcList[sTargetH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_HOLDOBJECT ] = 0;
					bRemoveFromDelayEventList(sTargetH, DEF_OWNERTYPE_NPC, DEF_MAGICTYPE_HOLDOBJECT);
				}

				

				// NPC¿¡ ´ëÇÑ °ø°ÝÀÌ ¼º°øÇßÀ¸¹Ç·Î °ø°ÝÀÚ°¡ ÇÃ·¹ÀÌ¾î¶ó¸é ÀÔÈù ´ë¹ÌÁö ¸¸Å­ÀÇ °æÇèÄ¡¸¦ °ø°ÝÀÚ¿¡°Ô ÁØ´Ù. 
				if ( (m_pNpcList[sTargetH]->m_iNoDieRemainExp > 0) && (m_pNpcList[sTargetH]->m_bIsSummoned != true) && 
					 (cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH] != 0) ) {
					// ExpStockÀ» ¿Ã¸°´Ù. ´Ü ¼ÒÈ¯¸÷ÀÎ °æ¿ì °æÇèÄ¡¸¦ ¿Ã¸®Áö ¾Ê´Â´Ù.
					if (m_pNpcList[sTargetH]->m_iNoDieRemainExp > iDamage) {
						// Crusade
						iExp = iDamage;

						//v2.03 918 °æÇèÄ¡ Áõ°¡ 
						if (m_pClientList[sAttackerH]->m_iAddExp > 0) {
							dTmp1 = (float)m_pClientList[sAttackerH]->m_iAddExp;
							dTmp2 = (float)iExp;
							dTmp3 = (dTmp1/100.0f)*dTmp2;
							iExp += (UINT32)dTmp3;
						}
						
						// v2.17 2002-8-6 °ø°ÝÀÚÀÇ ·¹º§ÀÌ 100 ÀÌ»óÀÌ¸é Åä³¢³ª °í¾çÀÌ¸¦ ÀâÀ»¶§ °æÇèÄ¡°¡ ¿Ã¶ó°¡Áö ¾Ê´Â´Ù.
						if (m_pClientList[sAttackerH]->m_iLevel > 100 ) {
							switch (m_pNpcList[sTargetH]->m_sType) {
							case 55:
							case 56:
								iExp = 0 ;
							break ;
							default: break;
							}
						}

						if (bExp == true) 
							 GetExp(sAttackerH, iExp); 
						else GetExp(sAttackerH, (iExp/2)); 
						m_pNpcList[sTargetH]->m_iNoDieRemainExp -= iDamage;
					}
					else {
						// Crusade
						iExp = m_pNpcList[sTargetH]->m_iNoDieRemainExp;

						//v2.03 918 °æÇèÄ¡ Áõ°¡ 
						if (m_pClientList[sAttackerH]->m_iAddExp > 0) {
							dTmp1 = (float)m_pClientList[sAttackerH]->m_iAddExp;
							dTmp2 = (float)iExp;
							dTmp3 = (dTmp1/100.0f)*dTmp2;
							iExp += (UINT32)dTmp3;
						}

						// v2.17 2002-8-6 °ø°ÝÀÚÀÇ ·¹º§ÀÌ 100 ÀÌ»óÀÌ¸é Åä³¢³ª °í¾çÀÌ¸¦ ÀâÀ»¶§ °æÇèÄ¡°¡ ¿Ã¶ó°¡Áö ¾Ê´Â´Ù.
						if (m_pClientList[sAttackerH]->m_iLevel > 100 ) {
							switch (m_pNpcList[sTargetH]->m_sType) {
							case 55:
							case 56:
								iExp = 0 ;
							break ;
							default: break;
							}
						}


						if (bExp == true) 
							 GetExp(sAttackerH, iExp); 
						else GetExp(sAttackerH, (iExp/2)); 
						m_pNpcList[sTargetH]->m_iNoDieRemainExp = 0;
					}
				}
			}
		}
		break;
	}
}

void CGame::Effect_HpUp_Spot(short sAttackerH, char cAttackerType, short sTargetH, char cTargetType, short sV1, short sV2, short sV3)
{
 int iHP, iMaxHP;
 UINT32 dwTime = timeGetTime();

	if (cAttackerType == DEF_OWNERTYPE_PLAYER)
		if (m_pClientList[sAttackerH] == 0) return;

	iHP = iDice(sV1, sV2) + sV3;

	switch (cTargetType) {
	case DEF_OWNERTYPE_PLAYER:
		if (m_pClientList[sTargetH] == 0) return;
		if (m_pClientList[sTargetH]->m_bIsKilled == true) return;
		iMaxHP = iGetMaxHP(sTargetH);
		if (m_pClientList[sTargetH]->m_iSideEffect_MaxHPdown != 0) 
			iMaxHP = iMaxHP - (iMaxHP/m_pClientList[sTargetH]->m_iSideEffect_MaxHPdown);
		if (m_pClientList[sTargetH]->m_iHP < iMaxHP) {
			m_pClientList[sTargetH]->m_iHP += iHP;
			if (m_pClientList[sTargetH]->m_iHP > iMaxHP) m_pClientList[sTargetH]->m_iHP = iMaxHP;
			if (m_pClientList[sTargetH]->m_iHP <= 0)     m_pClientList[sTargetH]->m_iHP = 1;
			SendNotifyMsg(0, sTargetH, DEF_NOTIFY_HP, 0, 0, 0, 0);
		}
		break;

	case DEF_OWNERTYPE_NPC:
		if (m_pNpcList[sTargetH] == 0) return;
		if (m_pNpcList[sTargetH]->m_iHP <= 0) return;
		if (m_pNpcList[sTargetH]->m_bIsKilled == true) return;
		iMaxHP = m_pNpcList[sTargetH]->m_iHitDice*4;
		if (m_pNpcList[sTargetH]->m_iHP < iMaxHP) {
			m_pNpcList[sTargetH]->m_iHP += iHP;
			if (m_pNpcList[sTargetH]->m_iHP > iMaxHP) m_pNpcList[sTargetH]->m_iHP = iMaxHP;
			if (m_pNpcList[sTargetH]->m_iHP <= 0)     m_pNpcList[sTargetH]->m_iHP = 1;
		}
		break;
	}
}

void CGame::Effect_SpDown_Spot(short sAttackerH, char cAttackerType, short sTargetH, char cTargetType, short sV1, short sV2, short sV3)
{
 int iSP, iMaxSP;
 UINT32 dwTime = timeGetTime();

	if (cAttackerType == DEF_OWNERTYPE_PLAYER)
		if (m_pClientList[sAttackerH] == 0) return;

	// Ã‡Ã˜Â´Ã§ Ã…Â¸Ã„ÃÃ€Ã‡ SpÂ¸Â¦ Â³Â»Â¸Â°Â´Ã™.
	iSP = iDice(sV1, sV2) + sV3;
	
	switch (cTargetType) {
	case DEF_OWNERTYPE_PLAYER:
		if (m_pClientList[sTargetH] == 0) return;
		if (m_pClientList[sTargetH]->m_bIsKilled == true) return;
	
		// New 19/05/2004
		// Is the user having an invincibility slate
		if ((m_pClientList[sTargetH]->m_iStatus & 0x400000) != 0) return;

		iMaxSP = iGetMaxSP(sTargetH);
		if (m_pClientList[sTargetH]->m_iSP > 0) {
			
			//v1.42 
			if (m_pClientList[sTargetH]->m_iTimeLeft_FirmStaminar == 0) {
				m_pClientList[sTargetH]->m_iSP -= iSP;
				if (m_pClientList[sTargetH]->m_iSP < 0) m_pClientList[sTargetH]->m_iSP = 0;
				SendNotifyMsg(0, sTargetH, DEF_NOTIFY_SP, 0, 0, 0, 0);
			}
		}
		break;

	case DEF_OWNERTYPE_NPC:
		// NPCÂ´Ã‚ Â½ÂºÃ…Ã‚Â¹ÃŒÂ³ÃŠ Â°Â³Â³Ã¤Ã€ÃŒ Â¾Ã¸Â´Ã™.
		break;
	}
}


void CGame::Effect_SpUp_Spot(short sAttackerH, char cAttackerType, short sTargetH, char cTargetType, short sV1, short sV2, short sV3)
{
 int iSP, iMaxSP;
 UINT32 dwTime = timeGetTime();

	if (cAttackerType == DEF_OWNERTYPE_PLAYER)
		if (m_pClientList[sAttackerH] == 0) return;

	// Ã‡Ã˜Â´Ã§ Ã…Â¸Ã„ÃÃ€Ã‡ SpÂ¸Â¦ Â¿ÃƒÂ¸Â°Â´Ã™.
	iSP = iDice(sV1, sV2) + sV3;
	
	switch (cTargetType) {
	case DEF_OWNERTYPE_PLAYER:
		if (m_pClientList[sTargetH] == 0) return;
		if (m_pClientList[sTargetH]->m_bIsKilled == true) return;
	
		iMaxSP = iGetMaxSP(sTargetH);
		if (m_pClientList[sTargetH]->m_iSP < iMaxSP) {
			m_pClientList[sTargetH]->m_iSP += iSP;
			
			if (m_pClientList[sTargetH]->m_iSP > iMaxSP) 
				m_pClientList[sTargetH]->m_iSP = iMaxSP;

			SendNotifyMsg(0, sTargetH, DEF_NOTIFY_SP, 0, 0, 0, 0);
		}
		break;

	case DEF_OWNERTYPE_NPC:
		// NPCÂ´Ã‚ Â½ÂºÃ…Ã‚Â¹ÃŒÂ³ÃŠ Â°Â³Â³Ã¤Ã€ÃŒ Â¾Ã¸Â´Ã™.
		break;
	}
}

void CGame::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	switch (wParam) {
	case VK_F1:
		m_bF1pressed = true;
		break;
	case VK_F4:
		m_bF4pressed = true;
		break;
	case VK_F5:
		m_bF5pressed = true;
		break;
	case VK_F12:
		m_bF12pressed = true;
		break;
	}
}

void CGame::OnKeyUp(WPARAM wParam, LPARAM lParam)
{
 int i;
 
	switch (wParam) {
	case VK_F2:
				
		break;
	
	case VK_F1:
		m_bF1pressed = false;
		break;
	case VK_F4:
		m_bF4pressed = false;
		break;
	case VK_F5:
		m_bF5pressed = false;
		break;
	case VK_F12:
		m_bF12pressed = false;
		break;
	
	case VK_F6:
		if (m_bF1pressed == true) {
			PutLogList("(!) Send server shutdown announcement 1...");
			for (i = 1; i < DEF_MAXCLIENTS; i++)
			if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_bIsInitComplete == true)) {
				SendNotifyMsg(0, i, DEF_NOTIFY_SERVERSHUTDOWN, 1, 0, 0, 0);
			}
		}
		break;

	case VK_F7:
		if (m_bF1pressed == true) {
			PutLogList("(!) Send server shutdown announcement 2...");
			for (i = 1; i < DEF_MAXCLIENTS; i++)
			if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_bIsInitComplete == true)) {
				SendNotifyMsg(0, i, DEF_NOTIFY_SERVERSHUTDOWN, 2, 0, 0, 0);
			}
		}
		break;

	case VK_F9:

		break;

	case VK_F11:

		break;

	
	case VK_1:
	case VK_4:

		break;


	case VK_2:

		break;

	case VK_3:

		break;


		//Crusade Testcode
	case VK_HOME:

		break;

	case VK_INSERT:

		break;

	case VK_DELETE:

		break;
	}
}

/*********************************************************************************************************************
**  bool CGame::bRegisterDelayEvent(int iDelayType, int iEffectType, UINT32 dwLastTime, int iTargetH,				**
**									   char cTargetType, char cMapIndex, int dX, int dY, int iV1, int iV2, int iV3) **
**  description			:: initiates the delayed event process														**
**  last updated		:: November 20, 2004; 9:30 PM; Hypnotoad													**
**	return value		:: bool																						**
**********************************************************************************************************************/
bool CGame::bRegisterDelayEvent(int iDelayType, int iEffectType, UINT32 dwLastTime, int iTargetH, char cTargetType, char cMapIndex, int dX, int dY, int iV1, int iV2, int iV3)
{
 int i;

	for (i = 0; i < DEF_MAXDELAYEVENTS; i++) 
	if (m_pDelayEventList[i] == 0) {
		m_pDelayEventList[i] = new class CDelayEvent;
		m_pDelayEventList[i]->m_iDelayType	= iDelayType;
		m_pDelayEventList[i]->m_iEffectType = iEffectType;
		m_pDelayEventList[i]->m_cMapIndex	= cMapIndex;
		m_pDelayEventList[i]->m_dX			= dX;
		m_pDelayEventList[i]->m_dY			= dY;
		m_pDelayEventList[i]->m_iTargetH    = iTargetH;
		m_pDelayEventList[i]->m_cTargetType = cTargetType;
		m_pDelayEventList[i]->m_iV1         = iV1;
		m_pDelayEventList[i]->m_iV2         = iV2;
		m_pDelayEventList[i]->m_iV3         = iV3; 
		m_pDelayEventList[i]->m_dwTriggerTime = dwLastTime;
		return true;
	}
	return false;
}

void CGame::DelayEventProcessor()
{
 int i, iSkillNum, iResult;
 UINT32 dwTime = timeGetTime();
 int iTemp;

	for (i = 0; i < DEF_MAXDELAYEVENTS; i++) 
	if ((m_pDelayEventList[i] != 0) && (m_pDelayEventList[i]->m_dwTriggerTime < dwTime)) {

		// Ã€ÃŒÂºÂ¥Ã†Â®Â°Â¡ ÂµÂ¿Ã€Ã›Ã‡Ã’ Â½ÃƒÂ°Â£Ã€ÃŒ ÂµÃ†Â´Ã™. ÂµÂ¿Ã€Ã›ÃˆÃ„ Â»Ã¨ÃÂ¦ÂµÃˆÂ´Ã™.
		switch (m_pDelayEventList[i]->m_iDelayType) {

		case DEF_DELAYEVENTTYPE_TAMING_SKILL: // Taming
			switch (m_pDelayEventList[i]->m_cTargetType) {
			case DEF_OWNERTYPE_PLAYER:				
				if (m_pClientList[m_pDelayEventList[i]->m_iTargetH] == 0) break;
				// If not the right Taming usage exit
				if (m_pClientList[m_pDelayEventList[i]->m_iTargetH]->m_bSkillUsingStatus[22] == false) break;
				if (m_pClientList[m_pDelayEventList[i]->m_iTargetH]->m_iSkillUsingTimeID[22] != m_pDelayEventList[i]->m_iV2) break;
				_TamingHandler(m_pDelayEventList[i]->m_iTargetH
								, 22
								, m_pDelayEventList[i]->m_cMapIndex
								, m_pDelayEventList[i]->m_dX
								, m_pDelayEventList[i]->m_dY);
				m_pClientList[m_pDelayEventList[i]->m_iTargetH]->m_bSkillUsingStatus[22] = false;
				m_pClientList[m_pDelayEventList[i]->m_iTargetH]->m_iSkillUsingTimeID[22] = 0;
				break;
			}
			break;

		case DEF_DELAYEVENTTYPE_ANCIENT_TABLET:
			if ((m_pClientList[m_pDelayEventList[i]->m_iTargetH]->m_iStatus & 0x400000) != 0) {
				iTemp = 1;
			}
			else if ((m_pClientList[m_pDelayEventList[i]->m_iTargetH]->m_iStatus & 0x800000) != 0) {
				iTemp = 3;
			}
			else if ((m_pClientList[m_pDelayEventList[i]->m_iTargetH]->m_iStatus & 0x10000) != 0) {
				iTemp = 4;
			}

			SendNotifyMsg(0, m_pDelayEventList[i]->m_iTargetH, DEF_NOTIFY_SLATE_STATUS, iTemp, 0, 0, 0);
			SetSlateFlag(m_pDelayEventList[i]->m_iTargetH, iTemp, false);
			break;

		case DEF_DELAYEVENTTYPE_CALCMETEORSTRIKEEFFECT:
			CalcMeteorStrikeEffectHandler(m_pDelayEventList[i]->m_cMapIndex);
			break;

		case DEF_DELAYEVENTTYPE_DOMETEORSTRIKEDAMAGE:
			DoMeteorStrikeDamageHandler(m_pDelayEventList[i]->m_cMapIndex);
			break;
		
		case DEF_DELAYEVENTTYPE_METEORSTRIKE:
			MeteorStrikeHandler(m_pDelayEventList[i]->m_cMapIndex);
			break;
		
		case DEF_DELAYEVENTTYPE_USEITEM_SKILL:
			// Â¾Ã†Ã€ÃŒÃ…Ã› Â»Ã§Â¿Ã«Â¿Â¡ ÂµÃ»Â¸Â¥ Â°Ã¡Â°Ãº Â°Ã¨Â»Ãª, Ã…Ã«ÂºÂ¸ 
			switch (m_pDelayEventList[i]->m_cTargetType) {
			case DEF_OWNERTYPE_PLAYER:
				iSkillNum = m_pDelayEventList[i]->m_iEffectType;
				
				if ( m_pClientList[m_pDelayEventList[i]->m_iTargetH] == 0 ) break;
				// Â±Ã¢Â¼Ãº Â»Ã§Â¿Ã«Ã€ÃŒ Â¹Â«ÃˆÂ¿ÃˆÂ­ ÂµÃ‡Â¾ÃºÂ´Ã™Â¸Ã© Â¹Â«Â½Ãƒ. 
				if ( m_pClientList[m_pDelayEventList[i]->m_iTargetH]->m_bSkillUsingStatus[iSkillNum] == false ) break;
				// Â±Ã¢Â¼Ãº Â»Ã§Â¿Ã« Â½ÃƒÂ°Â£ IDÂ°Â¡ Â´ÃžÂ¶Ã³ÂµÂµ Â¹Â«Â½Ãƒ v1.12
				if ( m_pClientList[m_pDelayEventList[i]->m_iTargetH]->m_iSkillUsingTimeID[iSkillNum] != m_pDelayEventList[i]->m_iV2) break;
				
				// Â±Ã¢Â¼Ãº Â»Ã§Â¿Ã« Â»Ã³Ã…Ã‚ Ã‡Ã˜ÃÂ¦ 
				m_pClientList[m_pDelayEventList[i]->m_iTargetH]->m_bSkillUsingStatus[iSkillNum] = false;
				m_pClientList[m_pDelayEventList[i]->m_iTargetH]->m_iSkillUsingTimeID[iSkillNum] = 0;
				
				// Ã€ÃŒÃÂ¦ SkillÃ€Ã‡ ÃˆÂ¿Â°ÃºÂ¿Â¡ ÂµÃ»Â¸Â¥ Â°Ã¨Â»ÃªÃ€Â» Ã‡Ã‘Â´Ã™. 
				iResult = iCalculateUseSkillItemEffect(m_pDelayEventList[i]->m_iTargetH, m_pDelayEventList[i]->m_cTargetType,
				 	                                   m_pDelayEventList[i]->m_iV1, iSkillNum, m_pDelayEventList[i]->m_cMapIndex, m_pDelayEventList[i]->m_dX, m_pDelayEventList[i]->m_dY);

				// Â±Ã¢Â¼Ãº Â»Ã§Â¿Ã«Ã€ÃŒ ÃÃŸÃÃ¶ ÂµÃ‡Â¾ÃºÃ€Â½Ã€Â» Â¾Ã‹Â¸Â°Â´Ã™.
				SendNotifyMsg(0, m_pDelayEventList[i]->m_iTargetH, DEF_NOTIFY_SKILLUSINGEND, iResult, 0, 0, 0);
				break;
			}
			break;
		
		case DEF_DELAYEVENTTYPE_DAMAGEOBJECT:
			break;

		case DEF_DELAYEVENTTYPE_KILL_ABADDON:
			NpcKilledHandler(m_pDelayEventList[i]->m_iTargetH, DEF_OWNERTYPE_NPC, m_pDelayEventList[i]->m_iTargetH, 0);
			break;

		case DEF_DELAYEVENTTYPE_END_APOCALYPSE:
			GlobalEndApocalypseMode(0);	
			break;

		case DEF_DELAYEVENTTYPE_MAGICRELEASE: 
			// Removes the aura after time
			switch (m_pDelayEventList[i]->m_cTargetType) {
			case DEF_OWNERTYPE_PLAYER:
				if (m_pClientList[ m_pDelayEventList[i]->m_iTargetH ] == 0) break;
				
				SendNotifyMsg(0, m_pDelayEventList[i]->m_iTargetH, DEF_NOTIFY_MAGICEFFECTOFF, 
					          m_pDelayEventList[i]->m_iEffectType, m_pClientList[ m_pDelayEventList[i]->m_iTargetH ]->m_cMagicEffectStatus[ m_pDelayEventList[i]->m_iEffectType ], 0, 0);
				
				m_pClientList[ m_pDelayEventList[i]->m_iTargetH ]->m_cMagicEffectStatus[ m_pDelayEventList[i]->m_iEffectType ] = 0;
				
				// Inbitition casting 
				if (m_pDelayEventList[i]->m_iEffectType == DEF_MAGICTYPE_INHIBITION)
					m_pClientList[m_pDelayEventList[i]->m_iTargetH]->m_bInhibition = false;

				// Invisibility È¿°ú ÇØÁ¦ 
				if (m_pDelayEventList[i]->m_iEffectType == DEF_MAGICTYPE_INVISIBILITY)
					SetInvisibilityFlag(m_pDelayEventList[i]->m_iTargetH, DEF_OWNERTYPE_PLAYER, false);

				// Berserk È¿°ú ÇØÁ¦
				if (m_pDelayEventList[i]->m_iEffectType == DEF_MAGICTYPE_BERSERK)
					SetBerserkFlag(m_pDelayEventList[i]->m_iTargetH, DEF_OWNERTYPE_PLAYER, false);

				// Confusion
				if (m_pDelayEventList[i]->m_iEffectType == DEF_MAGICTYPE_CONFUSE)
					switch(m_pDelayEventList[i]->m_iV1){
						case 3: SetIllusionFlag(m_pDelayEventList[i]->m_iTargetH, DEF_OWNERTYPE_PLAYER, false); break;
						case 4: SetIllusionMovementFlag(m_pDelayEventList[i]->m_iTargetH, DEF_OWNERTYPE_PLAYER, false); break;
					}	

				// Protection Magic
				if (m_pDelayEventList[i]->m_iEffectType == DEF_MAGICTYPE_PROTECT) {
					switch(m_pDelayEventList[i]->m_iV1){
						case 1:
							SetProtectionFromArrowFlag(m_pDelayEventList[i]->m_iTargetH, DEF_OWNERTYPE_PLAYER, false);
							break;
						case 2:
						case 5:
							SetMagicProtectionFlag(m_pDelayEventList[i]->m_iTargetH, DEF_OWNERTYPE_PLAYER, false);
							break;
						case 3:
						case 4:
							SetDefenseShieldFlag(m_pDelayEventList[i]->m_iTargetH, DEF_OWNERTYPE_PLAYER, false);
							break;
					}
				}

				
				// polymorph È¿°ú ÇØÁ¦ 
				if (m_pDelayEventList[i]->m_iEffectType == DEF_MAGICTYPE_POLYMORPH) {
					m_pClientList[m_pDelayEventList[i]->m_iTargetH]->m_sType = m_pClientList[m_pDelayEventList[i]->m_iTargetH]->m_sOriginalType;
					m_pClientList[m_pDelayEventList[i]->m_iTargetH]->m_bIsPolymorph = false;
					SendEventToNearClient_TypeA(m_pDelayEventList[i]->m_iTargetH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
				}

				// Ice È¿°ú ÇØÁ¦ 
				if (m_pDelayEventList[i]->m_iEffectType == DEF_MAGICTYPE_ICE)
					SetIceFlag(m_pDelayEventList[i]->m_iTargetH, DEF_OWNERTYPE_PLAYER, false);
				break;
			
			case DEF_OWNERTYPE_NPC:
				if (m_pNpcList[ m_pDelayEventList[i]->m_iTargetH ] == 0) break;
				
				m_pNpcList[ m_pDelayEventList[i]->m_iTargetH ]->m_cMagicEffectStatus[ m_pDelayEventList[i]->m_iEffectType ] = 0;
				
				// Invisibility ÃˆÂ¿Â°Ãº Ã‡Ã˜ÃÂ¦ 
				if (m_pDelayEventList[i]->m_iEffectType == DEF_MAGICTYPE_INVISIBILITY)
					SetInvisibilityFlag(m_pDelayEventList[i]->m_iTargetH, DEF_OWNERTYPE_NPC, false);

				// Berserk ÃˆÂ¿Â°Ãº Ã‡Ã˜ÃÂ¦
				if (m_pDelayEventList[i]->m_iEffectType == DEF_MAGICTYPE_BERSERK)
					SetBerserkFlag(m_pDelayEventList[i]->m_iTargetH, DEF_OWNERTYPE_NPC, false);

				// polymorph ÃˆÂ¿Â°Ãº Ã‡Ã˜ÃÂ¦ 
				if (m_pDelayEventList[i]->m_iEffectType == DEF_MAGICTYPE_POLYMORPH) {
					m_pClientList[m_pDelayEventList[i]->m_iTargetH]->m_sType = m_pClientList[m_pDelayEventList[i]->m_iTargetH]->m_sOriginalType;
					m_pClientList[m_pDelayEventList[i]->m_iTargetH]->m_bIsPolymorph = false;
					SendEventToNearClient_TypeA(m_pDelayEventList[i]->m_iTargetH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
				}

				// Ice ÃˆÂ¿Â°Ãº Ã‡Ã˜ÃÂ¦ 
				if (m_pDelayEventList[i]->m_iEffectType == DEF_MAGICTYPE_ICE)
					SetIceFlag(m_pDelayEventList[i]->m_iTargetH, DEF_OWNERTYPE_NPC, false);
				
				// Illusion
				if (m_pDelayEventList[i]->m_iEffectType == DEF_MAGICTYPE_CONFUSE)
					SetIllusionFlag(m_pDelayEventList[i]->m_iTargetH, DEF_OWNERTYPE_NPC, false);

				// Protection Magic
				if (m_pDelayEventList[i]->m_iEffectType == DEF_MAGICTYPE_PROTECT) {
					switch(m_pDelayEventList[i]->m_iV1){
						case 1:
							SetProtectionFromArrowFlag(m_pDelayEventList[i]->m_iTargetH, DEF_OWNERTYPE_NPC, false);
							break;
						case 2:
						case 5:
							SetMagicProtectionFlag(m_pDelayEventList[i]->m_iTargetH, DEF_OWNERTYPE_NPC, false);
							break;
						case 3:
						case 4:
							SetDefenseShieldFlag(m_pDelayEventList[i]->m_iTargetH, DEF_OWNERTYPE_NPC, false);
							break;
					}
				}
				break;
			}
			break;
		}
		
		delete m_pDelayEventList[i];
		m_pDelayEventList[i] = 0;
	}
}
bool CGame::bRemoveFromDelayEventList(int iH, char cType, int iEffectType)
{
 int i;

	for (i = 0; i < DEF_MAXDELAYEVENTS; i++) 
	if (m_pDelayEventList[i] != 0) {
		
		if (iEffectType == 0) {
			// Effect ÃÂ¾Â·Ã¹Â¿Â¡ Â»Ã³Â°Ã¼Â¾Ã¸Ã€ÃŒ Â¸Ã°ÂµÃŽ Â»Ã¨ÃÂ¦ 	
			if ( (m_pDelayEventList[i]->m_iTargetH == iH) && (m_pDelayEventList[i]->m_cTargetType == cType) ) {
				delete m_pDelayEventList[i];
				m_pDelayEventList[i] = 0;
			}
		}
		else {
			// Ã‡Ã˜Â´Ã§ EffectÂ¸Â¸ Â»Ã¨ÃÂ¦.
			if ( (m_pDelayEventList[i]->m_iTargetH == iH) && (m_pDelayEventList[i]->m_cTargetType == cType) &&
				 (m_pDelayEventList[i]->m_iEffectType == iEffectType) ) {
				delete m_pDelayEventList[i];
				m_pDelayEventList[i] = 0;
			}
		}
	}
	
	return true;
}

void CGame::SendObjectMotionRejectMsg(int iClientH)
{
 char  * cp, cData[30];
 UINT32 * dwp;
 UINT16  * wp;
 short * sp;
 int     iRet;

	if (m_pClientList[iClientH] == 0) return; // add by SNOOPY

	m_pClientList[iClientH]->m_bIsMoveBlocked = true; // v2.171
	
	// Ã€ÃŒÂµÂ¿Ã€ÃŒ ÂºÃ’Â°Â¡Â´Ã‰Ã‡ÃÂ´Ã™. 
	dwp  = (UINT32 *)(cData + DEF_INDEX4_MSGID);
 	*dwp = MSGID_RESPONSE_MOTION;
	wp   = (UINT16 *)(cData + DEF_INDEX2_MSGTYPE);
	*wp  = DEF_OBJECTMOTION_REJECT;
	
	cp   = (char *)(cData + DEF_INDEX2_MSGTYPE + 2); 
	sp  = (short *)cp;
	*sp = m_pClientList[iClientH]->m_sX; 
	cp += 2;
	sp  = (short *)cp;
	*sp = m_pClientList[iClientH]->m_sY; 
	cp += 2;
	
	iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 10);
	switch (iRet) {
	case DEF_XSOCKEVENT_QUENEFULL:
	case DEF_XSOCKEVENT_SOCKETERROR:
	case DEF_XSOCKEVENT_CRITICALERROR:
	case DEF_XSOCKEVENT_SOCKETCLOSED:
		// Â¸ÃžÂ½ÃƒÃÃ¶Â¸Â¦ ÂºÂ¸Â³Â¾Â¶Â§ Â¿Â¡Â·Â¯Â°Â¡ Â¹ÃŸÂ»Ã½Ã‡ÃŸÂ´Ã™Â¸Ã© ÃÂ¦Â°Ã…Ã‡Ã‘Â´Ã™.
		DeleteClient(iClientH, true, true);
		break;
	}
}

int CGame::_iGetTotalClients()
{
 int i, iTotal;

	iTotal = 0;
	for (i = 1; i < DEF_MAXCLIENTS; i++) 
	if (m_pClientList[i] != 0) iTotal++;

	return iTotal;
}

void CGame::DynamicObjectEffectProcessor()
{
 int i, ix, iy, iIndex;
 short sOwnerH, iDamage, sType, sAttacker;
 char  cOwnerType, cAttackerOwnerType;
 UINT32 dwTime = timeGetTime(), dwRegisterTime;
	
	// 
	for (i = 0; i < DEF_MAXDYNAMICOBJECTS; i++) 
	if (m_pDynamicObjectList[i] != 0) {

		cAttackerOwnerType = m_pDynamicObjectList[i]->m_cOwnerType;
		if (cAttackerOwnerType == DEF_OWNERTYPE_PLAYER_INDIRECT)
		{
			sAttacker = m_pDynamicObjectList[i]->m_sOwner;
			if (m_pClientList[sAttacker] == 0)
			{
				sAttacker = 0;
				cAttackerOwnerType = 0;
			}
			else if (strcmp(m_pClientList[sAttacker]->m_cCharName, m_pDynamicObjectList[i]->cOwnerName) != 0)
			{
				sAttacker = 0;
				cAttackerOwnerType = 0;
			}
		}
		switch (m_pDynamicObjectList[i]->m_sType) {

		case DEF_DYNAMICOBJECT_PCLOUD_BEGIN:
			// Ã†Ã·Ã€ÃŒÃÃ° Ã…Â¬Â¶Ã³Â¿Ã¬ÂµÃ¥
			for (ix = m_pDynamicObjectList[i]->m_sX -1; ix <= m_pDynamicObjectList[i]->m_sX+1; ix++)
			for (iy = m_pDynamicObjectList[i]->m_sY -1; iy <= m_pDynamicObjectList[i]->m_sY+1; iy++) {
				
				m_pMapList[m_pDynamicObjectList[i]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
				if (sOwnerH != 0) {
					// Poison DamageÂ¸Â¦ Ã€Ã”Â´Ã‚Â´Ã™.
					switch (cOwnerType) {
					case DEF_OWNERTYPE_PLAYER:
						if (m_pClientList[sOwnerH] == 0) break;
						if (m_pClientList[sOwnerH]->m_bIsKilled == true) break;
						// v1.41 ÃÃŸÂ¸Â³Ã€ÃŒÂ°Ã­ Ã€Ã¼Ã…ÃµÂ¸Ã°ÂµÃ¥Â°Â¡ Â¾Ã†Â´ÃÂ¸Ã© Ã‡Ã‡Ã‡Ã˜Â¸Â¦ Ã€Ã”ÃÃ¶ Â¾ÃŠÂ´Ã‚Â´Ã™.
						// Ã€ÃŒÂ·Â± Â½Ã„Ã€Â¸Â·ÃŽ Â´Ã«Â¹ÃŒÃÃ¶Ã€Ã‡ Ã…Â©Â±Ã¢Â¸Â¦ Â°Ã¡ÃÂ¤
						// Snoopy: Increazed a bit ..because rain/snow will reduce it.
						if (m_pDynamicObjectList[i]->m_iV1 <= 20) // Was 20 or 40 in spells definition ....
							 iDamage = iDice(2,8); // 1..8 >> 2..8
						else iDamage = iDice(2,10); // 1..8 >> 2..10	

						// SNOOPY: Whether Bouns
						iDamage += iGetWhetherMagicBonusEffect(46, m_pMapList[m_pDynamicObjectList[i]->m_cMapIndex]->m_cWhetherStatus);

						// New 17/05/2004 Changed
						if (m_pClientList[sOwnerH]->m_iAdminUserLevel == 0)
							m_pClientList[sOwnerH]->m_iHP -= iDamage;
						
						if (m_pClientList[sOwnerH]->m_iHP <= 0) {
							// Ã‡ÃƒÂ·Â¹Ã€ÃŒÂ¾Ã®Â°Â¡ Â»Ã§Â¸ÃÃ‡ÃŸÂ´Ã™.
							ClientKilledHandler(sOwnerH, sOwnerH, cOwnerType, iDamage);	
	  					}
						else {
							if (iDamage > 0) {
								// Â¹ÃžÃ€Âº Â´Ã«Â¹ÃŒÃÃ¶Â¸Â¦ Ã…Ã«ÂºÂ¸Ã‡Ã‘Â´Ã™. <- HPÂ¸Â¦ Â±Ã—Â´Ã«Â·ÃŽ Â¾Ã‹Â¸Â°Â´Ã™.
								SendNotifyMsg(0, sOwnerH, DEF_NOTIFY_HP, 0, 0, 0, 0);

								if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_HOLDOBJECT ] != 0) {
									// Â¸Â¶ÂºÃ± Â»Ã³Ã…Ã‚Â°Â¡ Ã‡Â®Â¸Â°Â´Ã™.	
									// 1: Hold-Person 
									// 2: Paralize
									SendNotifyMsg(0, sOwnerH, DEF_NOTIFY_MAGICEFFECTOFF, DEF_MAGICTYPE_HOLDOBJECT, m_pClientList[sOwnerH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_HOLDOBJECT ], 0, 0);
							
									m_pClientList[sOwnerH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_HOLDOBJECT ] = 0;
									bRemoveFromDelayEventList(sOwnerH, DEF_OWNERTYPE_PLAYER, DEF_MAGICTYPE_HOLDOBJECT);
								}
						 	}

							// v1.42 Poison ÃˆÂ¿Â°Ãº
							if ( (bCheckResistingMagicSuccess(1, sOwnerH, DEF_OWNERTYPE_PLAYER, 100) == false) &&
								 (m_pClientList[sOwnerH]->m_bIsPoisoned == false) &&
								(bCheckResistingPoisonSuccess(sOwnerH, DEF_OWNERTYPE_PLAYER) == false)) {
								
								m_pClientList[sOwnerH]->m_bIsPoisoned  = true;
								m_pClientList[sOwnerH]->m_iPoisonLevel = m_pDynamicObjectList[i]->m_iV1;
								m_pClientList[sOwnerH]->m_dwPoisonTime = dwTime;
								// ÃÃŸÂµÂ¶ÂµÃ‡Â¾ÃºÃ€Â½Ã€Â» Â¾Ã‹Â¸Â°Â´Ã™. 
								SetPoisonFlag(sOwnerH, cOwnerType, true);// poison aura appears from dynamic objects
								SendNotifyMsg(0, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_POISON, m_pClientList[sOwnerH]->m_iPoisonLevel, 0, 0);
							}
						}
						break;

					case DEF_OWNERTYPE_NPC:
						// Â¸Ã³Â½ÂºÃ…ÃÃ€Ã‡ ÃÃŸÂµÂ¶ÃˆÂ¿Â°ÃºÂ´Ã‚ Â¾Ã†ÃÃ· Â±Â¸Ã‡Ã¶ Â¾ÃˆÂµÃŠ
						if (m_pNpcList[sOwnerH] == 0) break;

						// Ã€ÃŒÂ·Â± Â½Ã„Ã€Â¸Â·ÃŽ Â´Ã«Â¹ÃŒÃÃ¶Ã€Ã‡ Ã…Â©Â±Ã¢Â¸Â¦ Â°Ã¡ÃÂ¤
						if (m_pDynamicObjectList[i]->m_iV1 <= 20)
							 iDamage = iDice(2,8);
						else iDamage = iDice(2,10);	
						// SNOOPY: Whether Bouns
						iDamage += iGetWhetherMagicBonusEffect(46, m_pMapList[m_pDynamicObjectList[i]->m_cMapIndex]->m_cWhetherStatus);

						if (iDamage < 1 ) iDamage = 1;				
						// HP

						// Ã€Ã¼Ã€Ã¯Â¿Ã« Â±Â¸ÃÂ¶Â¹Â°ÃÃŸ Â±Ã—Â·Â£ÂµÃ¥ Â¸Ã…ÃÃ· ÃÂ¦Â³Ã—Â·Â¹Ã€ÃŒÃ…Ã, Â¿Â¡Â³ÃŠÃÃ¶ Â½Ã‡ÂµÃ¥ ÃÂ¦Â³Ã—Â·Â¹Ã€ÃŒÃ…ÃÂ´Ã‚ Ã‡ÃŠÂµÃ¥Â·Ã¹ Â¸Â¶Â¹Ã½Â¿Â¡ Â´Ã«Â¹ÃŒÃÃ¶Â¸Â¦ Ã€Ã”ÃÃ¶ Â¾ÃŠÂ´Ã‚Â´Ã™.
						switch (m_pNpcList[sOwnerH]->m_cActionLimit) {
						case 0: // normal monster
						case 1: // never runner monster
							m_pNpcList[sOwnerH]->m_iHP -= iDamage;
							// SNOOPY: Poison cloud can poison npc
							if ((bCheckResistingMagicSuccess(0, sOwnerH, DEF_OWNERTYPE_NPC, 100) == false)
								&& (bCheckResistingPoisonSuccess(sOwnerH, DEF_OWNERTYPE_NPC) == false))
							{
								if (m_pNpcList[sOwnerH]->m_iPoisonLevel < m_pDynamicObjectList[i]->m_iV1)
									m_pNpcList[sOwnerH]->m_iPoisonLevel = m_pDynamicObjectList[i]->m_iV1;
								SetPoisonFlag(sOwnerH, cOwnerType, true);// poison aura appears from dynamic objects
							}
							if (m_pNpcList[sOwnerH]->m_iHP <= 0)
							{
								NpcKilledHandler(sAttacker, cAttackerOwnerType, sOwnerH, 0);
							}
							else
							{
								if (iDice(1, 3) == 2)
									m_pNpcList[sOwnerH]->m_dwTime = dwTime;

								if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] != 0) {
									// Hold 	
									m_pNpcList[sOwnerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] = 0;
								}

								// NPC
								SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTDAMAGE, iDamage, 0, 0);
							}
							break;
						default:
							iDamage = 0;
							break;
						}
						break;
					}
				}
			}
			break;
						
		case DEF_DYNAMICOBJECT_ICESTORM:
			for (ix = m_pDynamicObjectList[i]->m_sX -2; ix <= m_pDynamicObjectList[i]->m_sX+2; ix++)
			for (iy = m_pDynamicObjectList[i]->m_sY -2; iy <= m_pDynamicObjectList[i]->m_sY+2; iy++) {
				
				m_pMapList[m_pDynamicObjectList[i]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
				if (sOwnerH != 0) {
					switch (cOwnerType) {
					case DEF_OWNERTYPE_PLAYER:
						if (m_pClientList[sOwnerH] == 0) break;
						if (m_pClientList[sOwnerH]->m_bIsKilled == true) break;

						// Increased damages a bit
						iDamage = iDice(3,4) + 5;

						// SNOOPY: Whether Bouns
						iDamage += iGetWhetherMagicBonusEffect(55, m_pMapList[m_pDynamicObjectList[i]->m_cMapIndex]->m_cWhetherStatus);

						if (m_pClientList[sOwnerH]->m_iAdminUserLevel == 0)
						m_pClientList[sOwnerH]->m_iHP -= iDamage;
						
						if (m_pClientList[sOwnerH]->m_iHP <= 0) {
							// Ã‡ÃƒÂ·Â¹Ã€ÃŒÂ¾Ã®Â°Â¡ Â»Ã§Â¸ÃÃ‡ÃŸÂ´Ã™.
							ClientKilledHandler(sOwnerH, sOwnerH, cOwnerType, iDamage);	
	  					}
						else {
							if (iDamage > 0) {

									SendNotifyMsg(0, sOwnerH, DEF_NOTIFY_HP, 0, 0, 0, 0);

								if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_HOLDOBJECT ] == 1) {

									SendNotifyMsg(0, sOwnerH, DEF_NOTIFY_MAGICEFFECTOFF, DEF_MAGICTYPE_HOLDOBJECT, m_pClientList[sOwnerH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_HOLDOBJECT ], 0, 0);
							
									m_pClientList[sOwnerH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_HOLDOBJECT ] = 0;
									bRemoveFromDelayEventList(sOwnerH, DEF_OWNERTYPE_PLAYER, DEF_MAGICTYPE_HOLDOBJECT);
								}
						 	}

							if ( (bCheckResistingIceSuccess(1, sOwnerH, DEF_OWNERTYPE_PLAYER, m_pDynamicObjectList[i]->m_iV1) == false) &&
								 (m_pClientList[sOwnerH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_ICE ] == 0) ) {
								
								m_pClientList[sOwnerH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_ICE ] = 1;
								SetIceFlag(sOwnerH, cOwnerType, true);
								// ÃˆÂ¿Â°ÃºÂ°Â¡ Ã‡Ã˜ÃÂ¦ÂµÃ‰ Â¶Â§ Â¹ÃŸÂ»Ã½Ã‡Ã’ ÂµÃ´Â·Â¹Ã€ÃŒ Ã€ÃŒÂºÂ¥Ã†Â®Â¸Â¦ ÂµÃ®Â·ÃÃ‡Ã‘Â´Ã™.
								bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (20*1000), 
									                sOwnerH, cOwnerType, 0, 0, 0, 1, 0, 0);

								SendNotifyMsg(0, sOwnerH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_ICE, 1, 0, 0);
							}
						}
						break;

					case DEF_OWNERTYPE_NPC:
						if (m_pNpcList[sOwnerH] == 0) break;
						
						iDamage = iDice(3,4) + 5;
						// SNOOPY: Whether Bouns
						iDamage += iGetWhetherMagicBonusEffect(55, m_pMapList[m_pDynamicObjectList[i]->m_cMapIndex]->m_cWhetherStatus);

						switch (m_pNpcList[sOwnerH]->m_sType) {
						case 40: // ESG
						case 41: // GMG
						case 67: // McGaffin
						case 68: // Perry
						case 69: // Devlin
							iDamage = 0;
							break;
						}
						
						switch (m_pNpcList[sOwnerH]->m_cActionLimit) {
						case 0:
						case 3:
						case 5:
							m_pNpcList[sOwnerH]->m_iHP -= iDamage;
							break;
						case 8: // Heldenian gate.
							iDamage = iDamage/2;
							break;
						}

						if (m_pNpcList[sOwnerH]->m_iHP <= 0) {
							// NPCÂ°Â¡ Â»Ã§Â¸ÃÃ‡ÃŸÂ´Ã™.
							NpcKilledHandler(sOwnerH, cOwnerType, sOwnerH, 0); //v1.2 Ã…Â¸Â¼Â­ ÃÃ—Ã€Â¸Â¸Ã© Â¸Â¶ÃÃ¶Â¸Â· Â´Ã«Â¹ÃŒÃÃ¶Â°Â¡ 0. Â¾Ã†Ã€ÃŒÃ…Ã›Ã€Â» Â½Â±Â°Ã” Â±Â¸Ã‡ÃÃÃ¶ Â¸Ã¸Ã‡ÃÂ°Ã” Ã‡ÃÂ±Ã¢ Ã€Â§Ã‡Ã”.
						}
						else {
							// DamageÂ¸Â¦ Ã€Ã”Ã€Âº ÃƒÃ¦Â°ÃÃ€Â¸Â·ÃŽ Ã€ÃŽÃ‡Ã‘ ÃÃ¶Â¿Â¬ÃˆÂ¿Â°Ãº.
							if (iDice(1,3) == 2)
								m_pNpcList[sOwnerH]->m_dwTime = dwTime;
							
							if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_HOLDOBJECT ] != 0) {
								// Hold ÂµÃ‡Â¾Ã® Ã€Ã–Â¾ÃºÂ´Ã¸ Â»Ã³Ã…Ã‚Â¶Ã³Â¸Ã© Ã‡Â®Â¸Â°Â´Ã™. 	
								m_pNpcList[sOwnerH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_HOLDOBJECT ] = 0;
							}
						
							// NPCÂ¸Â¦ ÂµÂµÂ¸ÃÂ°Â¡Â´Ã‚ Â¸Ã°ÂµÃ¥Â·ÃŽ Ã€Ã¼ÃˆÂ¯Â½ÃƒÃ…Â²Â´Ã™.
					 		SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTDAMAGE, iDamage, 0, 0);

							// v1.42 Ice ÃˆÂ¿Â°Ãº
							if ( (bCheckResistingIceSuccess(1, sOwnerH, DEF_OWNERTYPE_NPC, m_pDynamicObjectList[i]->m_iV1) == false) &&
								 (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_ICE ] == 0) ) {
																
								m_pNpcList[sOwnerH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_ICE ] = 1;
								SetIceFlag(sOwnerH, cOwnerType, true);
								// ÃˆÂ¿Â°ÃºÂ°Â¡ Ã‡Ã˜ÃÂ¦ÂµÃ‰ Â¶Â§ Â¹ÃŸÂ»Ã½Ã‡Ã’ ÂµÃ´Â·Â¹Ã€ÃŒ Ã€ÃŒÂºÂ¥Ã†Â®Â¸Â¦ ÂµÃ®Â·ÃÃ‡Ã‘Â´Ã™.
								bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (20*1000), 
								                    sOwnerH, cOwnerType, 0, 0, 0, 1, 0, 0);
							}
						}
					   	break;
					}
				}

				// ÃÃ—Ã€ÂºÃƒÂ´Ã‡ÃÂ°Ã­ Ã€Ã–Â´Ã‚ Ã„Â³Â¸Â¯Ã€ÃŒ Ã€Ã–Â´Ã™Â¸Ã©
				m_pMapList[m_pDynamicObjectList[i]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, ix, iy);
				if ( (cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != 0) &&
			 	     (m_pClientList[sOwnerH]->m_iHP > 0) ) {
					// ÃÃ—Ã€Âº ÃƒÂ´Ã‡ÃÂ°Ã­ Ã€Ã–Â´Ã‚ Ã‡ÃƒÂ·Â¹Ã€ÃŒÂ¾Ã®Â´Ã™.
					iDamage = iDice(6,4) + 10; // Increased serioully.
					m_pClientList[sOwnerH]->m_iHP -= iDamage;
						
					if (m_pClientList[sOwnerH]->m_iHP <= 0) {
						// Ã‡ÃƒÂ·Â¹Ã€ÃŒÂ¾Ã®Â°Â¡ Â»Ã§Â¸ÃÃ‡ÃŸÂ´Ã™.
						ClientKilledHandler(sOwnerH, sOwnerH, cOwnerType, iDamage);	
	  				}
					else {
						if (iDamage > 0) {
							m_pClientList[sOwnerH]->m_dwRecentAttackTime = dwTime;
							// Â¹ÃžÃ€Âº Â´Ã«Â¹ÃŒÃÃ¶Â¸Â¦ Ã…Ã«ÂºÂ¸Ã‡Ã‘Â´Ã™. <- HPÂ¸Â¦ Â±Ã—Â´Ã«Â·ÃŽ Â¾Ã‹Â¸Â°Â´Ã™.
							SendNotifyMsg(0, sOwnerH, DEF_NOTIFY_HP, 0, 0, 0, 0);
						}
					}
				}

				// Â±Ã™ÃƒÂ³Â¿Â¡ Fire ObjectÂ°Â¡ Ã€Ã–Â´Ã™Â¸Ã© Â¼Ã¶Â¸Ã­Ã€Â» ÃÃ™Ã€ÃŽÂ´Ã™.
				m_pMapList[m_pDynamicObjectList[i]->m_cMapIndex]->bGetDynamicObject(ix, iy, &sType, &dwRegisterTime, &iIndex);
				if (((sType == DEF_DYNAMICOBJECT_FIRE) || (sType == DEF_DYNAMICOBJECT_FIRE3)) && (m_pDynamicObjectList[iIndex] != 0)) 
					m_pDynamicObjectList[iIndex]->m_dwLastTime = m_pDynamicObjectList[iIndex]->m_dwLastTime - (m_pDynamicObjectList[iIndex]->m_dwLastTime/10);
			}
			break;
		
		case DEF_DYNAMICOBJECT_FIRE3:
		case DEF_DYNAMICOBJECT_FIRE:
			// Fire-WallÂ·Ã¹Ã€Ã‡ Ã…Â¸Â´Ã‚ ÂºÃ’Â²Ã‰
			if (m_pDynamicObjectList[i]->m_iCount == 1) {
				// Â±Ã™ÃƒÂ³Â¿Â¡ Ã…Â¸Â´Ã‚ Â¹Â°Â°Ã‡Ã€ÃŒ Ã€Ã–Â´Ã™Â¸Ã© Â¹Ã¸ÃÃ¸Â´Ã™. 
				CheckFireBluring(m_pDynamicObjectList[i]->m_cMapIndex, m_pDynamicObjectList[i]->m_sX, m_pDynamicObjectList[i]->m_sY);
			}
			m_pDynamicObjectList[i]->m_iCount++;
			if (m_pDynamicObjectList[i]->m_iCount > 10) m_pDynamicObjectList[i]->m_iCount = 10; 

				for (ix = m_pDynamicObjectList[i]->m_sX;    ix <= m_pDynamicObjectList[i]->m_sX;   ix++)
				for (iy = m_pDynamicObjectList[i]->m_sY;    iy <= m_pDynamicObjectList[i]->m_sY;   iy++) {
				
				m_pMapList[m_pDynamicObjectList[i]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
				if (sOwnerH != 0) {
					// Fire DamageÂ¸Â¦ Ã€Ã”Â´Ã‚Â´Ã™.
					switch (cOwnerType) {
					
					case DEF_OWNERTYPE_PLAYER:
						if (m_pClientList[sOwnerH] == 0) break;
						if (m_pClientList[sOwnerH]->m_bIsKilled == true) break;
						// v1.41 ÃÃŸÂ¸Â³Ã€ÃŒÂ°Ã­ Ã€Ã¼Ã…ÃµÂ¸Ã°ÂµÃ¥Â°Â¡ Â¾Ã†Â´ÃÂ¸Ã© Ã‡Ã‡Ã‡Ã˜Â¸Â¦ Ã€Ã”ÃÃ¶ Â¾ÃŠÂ´Ã‚Â´Ã™.
						// Snoopy: Increazed a bit ..because rain/snow will reduce it.	.
						iDamage = iDice(2,4);			
						// SNOOPY: Whether Bouns
						iDamage += iGetWhetherMagicBonusEffect(40, m_pMapList[m_pDynamicObjectList[i]->m_cMapIndex]->m_cWhetherStatus);
						// New 17/05/2004
						if (m_pClientList[sOwnerH]->m_iAdminUserLevel == 0)
							m_pClientList[sOwnerH]->m_iHP -= iDamage;
						
						if (m_pClientList[sOwnerH]->m_iHP <= 0) {
							// Ã‡ÃƒÂ·Â¹Ã€ÃŒÂ¾Ã®Â°Â¡ Â»Ã§Â¸ÃÃ‡ÃŸÂ´Ã™.
							ClientKilledHandler(sOwnerH, sOwnerH, cOwnerType, iDamage);	
	  					}
						else {
							if (iDamage > 0) {
								// Â¹ÃžÃ€Âº Â´Ã«Â¹ÃŒÃÃ¶Â¸Â¦ Ã…Ã«ÂºÂ¸Ã‡Ã‘Â´Ã™. <- HPÂ¸Â¦ Â±Ã—Â´Ã«Â·ÃŽ Â¾Ã‹Â¸Â°Â´Ã™.
								SendNotifyMsg(0, sOwnerH, DEF_NOTIFY_HP, 0, 0, 0, 0);

								if (m_pClientList[sOwnerH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_HOLDOBJECT ] != 0) {
									// Hold-Person ÂµÃ‡Â¾Ã® Ã€Ã–Â¾ÃºÂ´Ã¸ Â»Ã³Ã…Ã‚Â¶Ã³Â¸Ã© Ã‡Â®Â¸Â°Â´Ã™. Fire FieldÂ·ÃŽÂ´Ã‚ Ã†ÃÂ·Â²Â¶Ã³Ã€ÃŒÃÃ® ÂµÃˆÂ°ÃÂµÂµ Ã‡Â®Â¸Â°Â´Ã™. 	
									// 1: Hold-Person 
									// 2: Paralize	
									SendNotifyMsg(0, sOwnerH, DEF_NOTIFY_MAGICEFFECTOFF, DEF_MAGICTYPE_HOLDOBJECT, m_pClientList[sOwnerH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_HOLDOBJECT ], 0, 0);
							
									m_pClientList[sOwnerH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_HOLDOBJECT ] = 0;
									bRemoveFromDelayEventList(sOwnerH, DEF_OWNERTYPE_PLAYER, DEF_MAGICTYPE_HOLDOBJECT);
								}
						 	}
						}
						break;

					case DEF_OWNERTYPE_NPC:
						if (m_pNpcList[sOwnerH] == 0) break;
						
						iDamage = iDice(2,4);	
						// SNOOPY: Whether Bouns
						iDamage += iGetWhetherMagicBonusEffect(40, m_pMapList[m_pDynamicObjectList[i]->m_cMapIndex]->m_cWhetherStatus);
						if (iDamage < 1 ) iDamage=1;

						// Ã€Ã¼Ã€Ã¯Â¿Ã« Â±Â¸ÃÂ¶Â¹Â°ÃÃŸ Â±Ã—Â·Â£ÂµÃ¥ Â¸Ã…ÃÃ· ÃÂ¦Â³Ã—Â·Â¹Ã€ÃŒÃ…Ã, Â¿Â¡Â³ÃŠÃÃ¶ Â½Ã‡ÂµÃ¥ ÃÂ¦Â³Ã—Â·Â¹Ã€ÃŒÃ…ÃÂ´Ã‚ Ã‡ÃŠÂµÃ¥Â·Ã¹ Â¸Â¶Â¹Ã½Â¿Â¡ Â´Ã«Â¹ÃŒÃÃ¶Â¸Â¦ Ã€Ã”ÃÃ¶ Â¾ÃŠÂ´Ã‚Â´Ã™.
						switch (m_pNpcList[sOwnerH]->m_sType) {
						case 40: // ESG
						case 41: // GMG
						case 67: // McGaffin
						case 68: // Perry
						case 69: // Devlin
							iDamage = 0;
							break;
						}
												
						// HPÂ¿Â¡Â¼Â­ Â»Â«Â´Ã™. Action LimitÂ¿Â¡ ÂµÃ»Â¶Ã³ ÃƒÂ³Â¸Â®Ã‡Ã‘Â´Ã™.
						switch (m_pNpcList[sOwnerH]->m_cActionLimit) {
						case 0: // Ã€ÃÂ¹Ã
						case 3: // Â´ÃµÂ¹ÃŒÂ·Ã¹
						case 5: // Â°Ã‡ÃƒÃ Â¹Â° 
						case 8: // Heldenian gate.
							m_pNpcList[sOwnerH]->m_iHP -= iDamage;
							break;
						}
						if (m_pNpcList[sOwnerH]->m_iHP <= 0) {
							// NPCÂ°Â¡ Â»Ã§Â¸ÃÃ‡ÃŸÂ´Ã™.
							NpcKilledHandler(sOwnerH, cOwnerType, sOwnerH, 0); //v1.2 Ã…Â¸Â¼Â­ ÃÃ—Ã€Â¸Â¸Ã© Â¸Â¶ÃÃ¶Â¸Â· Â´Ã«Â¹ÃŒÃÃ¶Â°Â¡ 0. Â¾Ã†Ã€ÃŒÃ…Ã›Ã€Â» Â½Â±Â°Ã” Â±Â¸Ã‡ÃÃÃ¶ Â¸Ã¸Ã‡ÃÂ°Ã” Ã‡ÃÂ±Ã¢ Ã€Â§Ã‡Ã”.
						}
						else {
							// DamageÂ¸Â¦ Ã€Ã”Ã€Âº ÃƒÃ¦Â°ÃÃ€Â¸Â·ÃŽ Ã€ÃŽÃ‡Ã‘ ÃÃ¶Â¿Â¬ÃˆÂ¿Â°Ãº.
							if (iDice(1,3) == 2)
								m_pNpcList[sOwnerH]->m_dwTime = dwTime;
							
							if (m_pNpcList[sOwnerH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_HOLDOBJECT ] != 0) {
								// Hold ÂµÃ‡Â¾Ã® Ã€Ã–Â¾ÃºÂ´Ã¸ Â»Ã³Ã…Ã‚Â¶Ã³Â¸Ã© Ã‡Â®Â¸Â°Â´Ã™. 	
								m_pNpcList[sOwnerH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_HOLDOBJECT ] = 0;
							}
						
							// NPCÂ¸Â¦ ÂµÂµÂ¸ÃÂ°Â¡Â´Ã‚ Â¸Ã°ÂµÃ¥Â·ÃŽ Ã€Ã¼ÃˆÂ¯Â½ÃƒÃ…Â²Â´Ã™.
					 		SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTDAMAGE, iDamage, 0, 0);
						}
					   	break;
					}
				}

				// ÃÃ—Ã€ÂºÃƒÂ´Ã‡ÃÂ°Ã­ Ã€Ã–Â´Ã‚ Ã„Â³Â¸Â¯Ã€ÃŒ Ã€Ã–Â´Ã™Â¸Ã©
				m_pMapList[m_pDynamicObjectList[i]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, ix, iy);
				if ( (cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != 0) &&
			 	     (m_pClientList[sOwnerH]->m_iHP > 0) ) {
					// ÃÃ—Ã€Âº ÃƒÂ´Ã‡ÃÂ°Ã­ Ã€Ã–Â´Ã‚ Ã‡ÃƒÂ·Â¹Ã€ÃŒÂ¾Ã®Â´Ã™.
					iDamage = iDice(1,6);
					m_pClientList[sOwnerH]->m_iHP -= iDamage;
						
					if (m_pClientList[sOwnerH]->m_iHP <= 0) {
						// Ã‡ÃƒÂ·Â¹Ã€ÃŒÂ¾Ã®Â°Â¡ Â»Ã§Â¸ÃÃ‡ÃŸÂ´Ã™.
						ClientKilledHandler(sOwnerH, sOwnerH, cOwnerType, iDamage);	
	  				}
					else {
						if (iDamage > 0) {
							m_pClientList[sOwnerH]->m_dwRecentAttackTime = dwTime;
							// Â¹ÃžÃ€Âº Â´Ã«Â¹ÃŒÃÃ¶Â¸Â¦ Ã…Ã«ÂºÂ¸Ã‡Ã‘Â´Ã™. <- HPÂ¸Â¦ Â±Ã—Â´Ã«Â·ÃŽ Â¾Ã‹Â¸Â°Â´Ã™.
							SendNotifyMsg(0, sOwnerH, DEF_NOTIFY_HP, 0, 0, 0, 0);
						}
					}
				}

				// Â±Ã™ÃƒÂ³Â¿Â¡ Ice ObjectÂ°Â¡ Ã€Ã–Â´Ã™Â¸Ã© Â¼Ã¶Â¸Ã­Ã€Â» ÃÃ™Ã€ÃŽÂ´Ã™.
				m_pMapList[m_pDynamicObjectList[i]->m_cMapIndex]->bGetDynamicObject(ix, iy, &sType, &dwRegisterTime, &iIndex);
				if ((sType == DEF_DYNAMICOBJECT_ICESTORM) && (m_pDynamicObjectList[iIndex] != 0)) 
					m_pDynamicObjectList[iIndex]->m_dwLastTime = m_pDynamicObjectList[iIndex]->m_dwLastTime - (m_pDynamicObjectList[iIndex]->m_dwLastTime/10);
			}
			break;
		}
	}
} 

void CGame::CheckAndNotifyPlayerConnection(int iClientH, char * pMsg, UINT32 dwSize)
{
 char   seps[] = "= \t\n";
 char   * token, * cp, cName[11], cBuff[256], cPlayerLocation[120];
 class  CStrTok * pStrTok;
 int i;
 UINT16 * wp;

	if (m_pClientList[iClientH] == 0) return;
	if (dwSize <= 0) return;

	ZeroMemory(cPlayerLocation,sizeof(cPlayerLocation));
	ZeroMemory(cName, sizeof(cName));
	ZeroMemory(cBuff, sizeof(cBuff));
	memcpy(cBuff, pMsg, dwSize);

	pStrTok = new class CStrTok(cBuff, seps);
	token = pStrTok->pGet();
	token = pStrTok->pGet();

	if (token == 0) {
		delete pStrTok;
		return;
	}

	if (strlen(token) > 10) 
		 memcpy(cName, token, 10);	
	else memcpy(cName, token, strlen(token));

	// cNameÃ€Ã‡ Ã€ÃŒÂ¸Â§Ã€Â» Â°Â¡ÃÃ¸ Ã‡ÃƒÂ·Â¹Ã€ÃŒÂ¾Ã®Â°Â¡ ÃÂ¢Â¼Ã“ÃÃŸÃ€ÃŽÃÃ¶ ÃƒÂ£Â´Ã‚Â´Ã™. 
	for (i = 1; i < DEF_MAXCLIENTS; i++) 
	if ((m_pClientList[i] != 0) && (memcmp(cName, m_pClientList[i]->m_cCharName, 10) == 0)) {
		// Â°Â°Ã€Âº Ã€ÃŒÂ¸Â§Ã€Â» Â°Â¡ÃÃ¸ Ã„Â³Â¸Â¯Ã…ÃÂ¸Â¦ ÃƒÂ£Â¾Ã’Â´Ã™. 
		if(m_pClientList[iClientH]->m_iAdminUserLevel > 0){ // GM's get more info
			cp = (char *)cPlayerLocation;

			memcpy(cp,m_pClientList[i]->m_cMapName,10);
			cp += 10;

			wp = (UINT16 *)cp;
			*wp = m_pClientList[i]->m_sX;
			cp += 2;

			wp = (UINT16 *)cp;
			*wp = m_pClientList[i]->m_sY;
			cp += 2;
		}
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_PLAYERONGAME, 0, 0, 0, m_pClientList[i]->m_cCharName,0,0,0,0,0,0,cPlayerLocation);

		delete pStrTok;
		return;
	}

	ZeroMemory(cBuff, sizeof(cBuff));
	cp = (char *)cBuff;
	*cp = GSM_REQUEST_FINDCHARACTER;
	cp++;

	wp = (UINT16 *)cp;
	*wp = m_wServerID_GSS;
	cp += 2;

	wp = (UINT16 *)cp;
	*wp = iClientH;
	cp += 2;

	memcpy(cp, cName, 10);
	cp += 10;

	memcpy(cp, m_pClientList[iClientH]->m_cCharName, 10);
	cp += 10;

	bStockMsgToGateServer(cBuff, 25);

	delete pStrTok;
}



void CGame::ToggleWhisperPlayer(int iClientH, char * pMsg, UINT32 dwMsgSize)
{
 char   seps[] = "= \t\n";
 char   * token, cName[11], cBuff[256];
 class  CStrTok * pStrTok;
 int i;
 char *cp;
 UINT16 *wp;

	if (m_pClientList[iClientH] == 0) return;
	if (dwMsgSize <= 0) return;

	ZeroMemory(cName, sizeof(cName));
	ZeroMemory(cBuff, sizeof(cBuff));
	memcpy(cBuff, pMsg, dwMsgSize);

	pStrTok = new class CStrTok(cBuff, seps);
	token = pStrTok->pGet();
	token = pStrTok->pGet();
   
	if (token == 0) {
		m_pClientList[iClientH]->m_iWhisperPlayerIndex = -1;
		ZeroMemory(m_pClientList[iClientH]->m_cWhisperPlayerName, sizeof(m_pClientList[iClientH]->m_cWhisperPlayerName));
		m_pClientList[iClientH]->m_bIsCheckingWhisperPlayer = false;
		// Whisper mode enabled
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_WHISPERMODEOFF, 0, 0, 0, cName);
	}
	else {
		if (strlen(token) > 10) 
			 memcpy(cName, token, 10);
		else memcpy(cName, token, strlen(token));

		m_pClientList[iClientH]->m_iWhisperPlayerIndex = -1;

		for (i = 1; i < DEF_MAXCLIENTS; i++) 
		if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cCharName, cName, 10) == 0)) {
			if (i == iClientH) {
				delete pStrTok;
				// Whisper self XenX
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, "You shouldn't talk to yourself!");
				return;
			}
			m_pClientList[iClientH]->m_iWhisperPlayerIndex = i;
			ZeroMemory(m_pClientList[iClientH]->m_cWhisperPlayerName, sizeof(m_pClientList[iClientH]->m_cWhisperPlayerName));
			memcpy(m_pClientList[iClientH]->m_cWhisperPlayerName, cName, 10);
			break;
		}

		if (m_pClientList[iClientH]->m_iWhisperPlayerIndex == -1) {
			ZeroMemory(cBuff, sizeof(cBuff));
			cp = (char *)cBuff;
			*cp = GSM_REQUEST_FINDCHARACTER;
			cp++;

			wp = (UINT16 *)cp;
			*wp = m_wServerID_GSS;
			cp += 2;

			wp = (UINT16 *)cp;
			*wp = iClientH;
			cp += 2;

			memcpy(cp, cName, 10);
			cp += 10;

			memcpy(cp, m_pClientList[iClientH]->m_cCharName, 10);
			cp += 10;

			bStockMsgToGateServer(cBuff, 25);

			ZeroMemory(m_pClientList[iClientH]->m_cWhisperPlayerName, sizeof(m_pClientList[iClientH]->m_cWhisperPlayerName));
			memcpy(m_pClientList[iClientH]->m_cWhisperPlayerName, cName, 10);
			m_pClientList[iClientH]->m_bIsCheckingWhisperPlayer = true;
		}
		else{
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_WHISPERMODEON, 0, 0, 0, m_pClientList[iClientH]->m_cWhisperPlayerName);
			return;
		}
		//player not online XenX
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, "This player is not online!");
	}
	delete pStrTok;
}


void CGame::SetPlayerProfile(int iClientH, char * pMsg, UINT32 dwMsgSize)
{
 char cTemp[256];
 int i;


	if (m_pClientList[iClientH] == 0) return;
	if ((dwMsgSize - 7) <= 0) return;

	ZeroMemory(cTemp, sizeof(cTemp));
	memcpy(cTemp, (pMsg + 7), dwMsgSize - 7);
	
	// Â°Ã¸Â¹Ã©Â¶ÃµÃ€Â» Â¾Ã°Â´ÃµÂ¹Ã™Â·ÃŽ Â¹Ã™Â²Ã›Â´Ã™. 
	for (i = 0; i < 256; i++)
		if (cTemp[i] == ' ') cTemp[i] = '_';

	// Â¿Â¡Â·Â¯ Â¹Ã¦ÃÃ¶ Ã„ÃšÂµÃ¥ 
	cTemp[255] = 0;

	ZeroMemory(m_pClientList[iClientH]->m_cProfile, sizeof(m_pClientList[iClientH]->m_cProfile));
	strcpy(m_pClientList[iClientH]->m_cProfile, cTemp);
}

void CGame::GetPlayerProfile(int iClientH, char * pMsg, UINT32 dwMsgSize)
{
 char   seps[] = "= \t\n";
 char   * token, cName[11], cBuff[256], cBuff2[500];
 class  CStrTok * pStrTok;
 int i;

	if (m_pClientList[iClientH] == 0) return;
	if ((dwMsgSize)	<= 0) return;

	ZeroMemory(cName, sizeof(cName));
	ZeroMemory(cBuff, sizeof(cBuff));
	memcpy(cBuff, pMsg, dwMsgSize);

	pStrTok = new class CStrTok(cBuff, seps);
	token = pStrTok->pGet();
	token = pStrTok->pGet();
   	
	if (token != 0) {
		// tokenÃ€ÃŒ Â°Ã° Ã‡ÃÂ·ÃŽÃ‡ÃŠÃ€Â» Â¾Ã²Â°Ã­Ã€Ãš Ã‡ÃÂ´Ã‚ Â»Ã§Â¿Ã«Ã€Ãš Ã€ÃŒÂ¸Â§ 
		if (strlen(token) > 10) 
			 memcpy(cName, token, 10);
		else memcpy(cName, token, strlen(token));
		
		for (i = 1; i < DEF_MAXCLIENTS; i++) 
		if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cCharName, cName, 10) == 0)) {
		
			ZeroMemory(cBuff2, sizeof(cBuff2));
			wsprintf(cBuff2, "%s Profile: %s", cName, m_pClientList[i]->m_cProfile);
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_PLAYERPROFILE, 0, 0, 0, cBuff2);

			delete pStrTok;
			return;
		}
		// Ã‡Ã¶Ã€Ã§ ÃÂ¢Â¼Ã“ÃÃŸÃ€ÃŒ Â¾Ã†Â´ÃÂ´Ã™.
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_PLAYERNOTONGAME, 0, 0, 0, cName);
	}

	delete pStrTok;
	return;
}

/*********************************************************************************************************************
** bool CGame::SetAngelFlag(short sOwnerH, char cOwnerType, int iStatus, iTemp)  Snoopy								** 
** description   :: Sets the staus to send or not Angels to every client											**
*********************************************************************************************************************/
void CGame::SetAngelFlag(short sOwnerH, char cOwnerType, int iStatus, int iTemp) 
{	if (cOwnerType != DEF_OWNERTYPE_PLAYER) return;
	if (m_pClientList[sOwnerH] == 0) return;
	switch (iStatus) {
	case 1: // STR Angel
		m_pClientList[sOwnerH]->m_iStatus = m_pClientList[sOwnerH]->m_iStatus | 0x00001000;  
		break;
	case 2: // DEX Angel
		m_pClientList[sOwnerH]->m_iStatus = m_pClientList[sOwnerH]->m_iStatus | 0x00002000;
		break;
	case 3: // INT Angel
		m_pClientList[sOwnerH]->m_iStatus = m_pClientList[sOwnerH]->m_iStatus | 0x00004000;
		break;
	case 4: // MAG Angel
		m_pClientList[sOwnerH]->m_iStatus = m_pClientList[sOwnerH]->m_iStatus | 0x00008000;
		break;
	default:// Remove all Angels
		m_pClientList[sOwnerH]->m_iStatus = m_pClientList[sOwnerH]->m_iStatus & 0xFFFF00FF;
		break;
	}
	if (iTemp > 9)
	{
		int iAngelicStars = (iTemp / 3) * (iTemp / 5);
		m_pClientList[sOwnerH]->m_iStatus = m_pClientList[sOwnerH]->m_iStatus | (iAngelicStars << 8);
	}
	SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
}

int CGame::_iGetPlayerNumberOnSpot(short dX, short dY, char cMapIndex, char cRange)
{
 int ix, iy, iSum = 0;
 short sOwnerH;
 char  cOwnerType;

	for (ix = dX - cRange; ix <= dX + cRange; ix++)
	for (iy = dY - cRange; iy <= dY + cRange; iy++) {
		m_pMapList[cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
		if ((sOwnerH != 0) && (cOwnerType == DEF_OWNERTYPE_PLAYER))
			iSum++;
	}

	return iSum;
}

bool CGame::bAnalyzeCriminalAction(int iClientH, short dX, short dY, bool bIsCheck)
{
 int   iNamingValue, tX, tY;
 short sOwnerH;
 char  cOwnerType, cName[11], cNpcName[21];
 char  cNpcWaypoint[11];

	if (m_pClientList[iClientH] == 0) return false;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return false;
	if (m_bIsCrusadeMode == true) return false;

	m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);

	if ((cOwnerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sOwnerH] != 0)) {
		if (_bGetIsPlayerHostile(iClientH, sOwnerH) != true) {
			if (bIsCheck == true) return true;

			ZeroMemory(cNpcName, sizeof(cNpcName));
			if (strcmp(m_pClientList[iClientH]->m_cMapName, "aresden") == 0) 
				strcpy(cNpcName, "Guard-Aresden");			
			else if (strcmp(m_pClientList[iClientH]->m_cMapName, "elvine") == 0) 
				strcpy(cNpcName, "Guard-Elvine");
			else  if (strcmp(m_pClientList[iClientH]->m_cMapName, "default") == 0) 
				strcpy(cNpcName, "Guard-Neutral");

			iNamingValue = m_pMapList[ m_pClientList[iClientH]->m_cMapIndex ]->iGetEmptyNamingValue();
			if (iNamingValue != -1) {

				ZeroMemory(cNpcWaypoint, sizeof(cNpcWaypoint));
				ZeroMemory(cName, sizeof(cName));
			   	wsprintf(cName, "XX%d", iNamingValue);
				cName[0] = '_';
				cName[1] = m_pClientList[iClientH]->m_cMapIndex+65;
				
				tX = (int)m_pClientList[iClientH]->m_sX;
				tY = (int)m_pClientList[iClientH]->m_sY;
				if (bCreateNewNpc(cNpcName, cName, m_pMapList[ m_pClientList[iClientH]->m_cMapIndex ]->m_cName, 0, 0, DEF_MOVETYPE_RANDOM, 
					              &tX, &tY, cNpcWaypoint, 0, 0, -1, false, true) == false) {
					m_pMapList[ m_pClientList[iClientH]->m_cMapIndex ]->SetNamingValueEmpty(iNamingValue);
				} 
				else {
					bSetNpcAttackMode(cName, iClientH, DEF_OWNERTYPE_PLAYER, true);
				}
			}
		}
	}
	return false;
}

bool CGame::_bGetIsPlayerHostile(int iClientH, int sOwnerH)
{
	if (m_pClientList[iClientH] == 0) return false;
	if (m_pClientList[sOwnerH]  == 0) return false;

	// Ã€ÃšÂ±Ã¢ Ã€ÃšÂ½Ã…Â¿Â¡ Â´Ã«Ã‡Ã‘ Â°Ã¸Â°ÃÃ€Âº Â¹Ã¼ÃÃ‹Â¶Ã³ ÂºÂ¼ Â¼Ã¶ Â¾Ã¸Â´Ã™. 
	if (iClientH == sOwnerH) return true;

	if (m_pClientList[iClientH]->m_cSide == 0) {
		// Â°Ã¸Â°ÃÃ€ÃšÂ´Ã‚ Â¿Â©Ã‡Ã Ã€ÃšÃ€ÃŒÂ´Ã™.
		// Â»Ã³Â´Ã«Â¹Ã¦Ã€ÃŒ Â¹Ã¼ÃÃ‹Ã€ÃšÃ€ÃŒÂ¸Ã© Ã€Ã»Â¼ÂºÃ€ÃŒÂ¸Ã§ Â±Ã— Ã€ÃŒÂ¿ÃœÂ¿Â¡Â´Ã‚ Â¾Ã†Â´ÃÂ´Ã™. 
		if (m_pClientList[sOwnerH]->m_iPKCount != 0) 
			 return true;
		else return false;
	}
	else {
		// Â°Ã¸Â°ÃÃ€ÃšÂ´Ã‚ Â¼Ã’Â¼Ã“ Â¸Â¶Ã€Â»Ã€Â» Â°Â®Â°Ã­ Ã€Ã–Â´Ã™. 
		if (m_pClientList[iClientH]->m_cSide != m_pClientList[sOwnerH]->m_cSide) {
			// Â¼Â­Â·ÃŽ Â»Ã§Ã€ÃŒÂµÃ¥Â°Â¡ Â´Ã™Â¸Â£Â´Ã™. 
			if (m_pClientList[sOwnerH]->m_cSide == 0) {
				if (m_pClientList[sOwnerH]->m_iPKCount != 0) 
				 	 return true;
				else return false;
			}
			else return true;
		}
		else {
			if (m_pClientList[sOwnerH]->m_iPKCount != 0) 
			 	 return true;
			else return false;
		}
	}

	return false;
}

void CGame::CheckDayOrNightMode()
{
 	
 int cPrevMode;
 int  i;

	// Ã‡Ã¶Ã€Ã§ Â½ÃƒÂ°Â£Â¿Â¡ ÂµÃ»Â¶Ã³ Â³Â· ÃˆÂ¤Ã€Âº Â¹Ã£Ã€Â» Â°Ã¡ÃÂ¤Ã‡Ã‘Â´Ã™. 
	// ÃÃ–, Â¾ÃŸÂ°Â£ Â¸Ã°ÂµÃ¥ Â¼Â³ÃÂ¤ 
	cPrevMode = m_cDayOrNight;
 
	if (srvHour >= 19 || srvHour <= 6) 
		 m_cDayOrNight = 2;
	else m_cDayOrNight = 1;

	if (cPrevMode != m_cDayOrNight) {
		// ÃÃ–, Â¾ÃŸÂ°Â£ Â¸Ã°ÂµÃ¥Â°Â¡ ÂºÂ¯Â°Ã¦ÂµÃ‡Â¾ÃºÂ´Ã™. ÃÃ–, Â¾ÃŸÂ°Â£ Â¸Ã°ÂµÃ¥Ã€Ã‡ Â¿ÂµÃ‡Ã¢Ã€Â» Â¹ÃžÂ´Ã‚ Â¸ÃŠÂ¿Â¡ ÃÂ¸Ã€Ã§Ã‡ÃÂ´Ã‚ Ã…Â¬Â¶Ã³Ã€ÃŒÂ¾Ã°Ã†Â®ÂµÃ©Â¿Â¡Â°Ã” Â¾Ã‹Â¸Â°Â´Ã™. 
		for (i = 1; i < DEF_MAXCLIENTS; i++) 
		if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_bIsInitComplete == true)) {
			if ((m_pClientList[i]->m_cMapIndex >= 0) && 
				(m_pMapList[m_pClientList[i]->m_cMapIndex] != 0) &&
				(m_pMapList[m_pClientList[i]->m_cMapIndex]->m_bIsFixedDayMode == false))
				SendNotifyMsg(0, i, DEF_NOTIFY_TIMECHANGE, m_cDayOrNight, 0, 0, 0);	
		}
	}
}


void CGame::ShutUpPlayer(int iClientH, char * pMsg, UINT32 dwMsgSize)
{
 char   seps[] = "= \t\n";
 char   * token, cName[11], cBuff[256];
 class  CStrTok * pStrTok;
 int i, iTime;

	if (m_pClientList[iClientH] == 0) return;
	if ((dwMsgSize)	<= 0) return;

	if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelShutup) {
		// Admin user levelÃ€ÃŒ Â³Â·Â¾Ã†Â¼Â­ Ã€ÃŒ Â±Ã¢Â´Ã‰Ã€Â» Â»Ã§Â¿Ã«Ã‡Ã’ Â¼Ã¶ Â¾Ã¸Â´Ã™.
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
		return;
	}

	ZeroMemory(cName, sizeof(cName));
	ZeroMemory(cBuff, sizeof(cBuff));
	memcpy(cBuff, pMsg, dwMsgSize);

	pStrTok = new class CStrTok(cBuff, seps);
	token = pStrTok->pGet();
	token = pStrTok->pGet();
   	
	if (token != 0) {
		// tokenÃ€ÃŒ Â°Ã° ÃƒÂ¤Ã†ÃƒÃ€Â» ÂºÃ’Â°Â¡Â´Ã‰Ã‡ÃÂ°Ã” Â¸Â¸ÂµÃ© Â»Ã§Â¿Ã«Ã€Ãš Ã€ÃŒÂ¸Â§ 
		if (strlen(token) > 10) 
			 memcpy(cName, token, 10);
		else memcpy(cName, token, strlen(token));
		
		// Â´Ã™Ã€Â½ Ã…Ã¤Ã…Â«Ã€Âº ÃÂ¤ÃÃ¶ Â½ÃƒÃ…Â³ Â½ÃƒÂ°Â£. 
		token = pStrTok->pGet();
		if (token == 0) 
			 iTime = 0;
		else iTime = atoi(token);
	
		if (iTime < 0) iTime = 0;
	 		
		for (i = 1; i < DEF_MAXCLIENTS; i++) 
		if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cCharName, cName, 10) == 0)) {
			
			m_pClientList[i]->m_iTimeLeft_ShutUp = iTime * 20; // 1Ã€ÃŒ 3ÃƒÃŠÂ´Ã™. 20Ã€ÃŒÂ¸Ã© 1ÂºÃ 
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_PLAYERSHUTUP, iTime, 0, 0, cName);
			SendNotifyMsg(0, i, DEF_NOTIFY_PLAYERSHUTUP, iTime, 0, 0, cName);

			// Admin Log
			wsprintf(G_cTxt, "GM Order(%s): Shutup PC(%s) (%d)Min", m_pClientList[iClientH]->m_cCharName, 
				     m_pClientList[i]->m_cCharName, iTime);
			PutAdminLogFileList(G_cTxt);

			delete pStrTok;
			return;
		}
		// Ã‡Ã¶Ã€Ã§ ÃÂ¢Â¼Ã“ÃÃŸÃ€ÃŒ Â¾Ã†Â´ÃÂ´Ã™.
		ZeroMemory(cBuff, sizeof(cBuff));

		char *cp;
		UINT16 *wp;

		cp = (char *)cBuff;

		*cp = GSM_REQUEST_SHUTUPPLAYER;
		cp++;

		wp = (UINT16 *)cp;
		*wp = m_wServerID_GSS;
		cp += 2;

		wp = (UINT16 *)cp;
		*wp = (UINT16)iClientH;
		cp += 2;

		memcpy(cp, cName, 10);
		cp += 10;

		wp = (UINT16 *)cp;
		*wp = iTime;
		cp += 2;

		memcpy(cp, m_pClientList[iClientH]->m_cCharName, 10);
		cp += 10;

		bStockMsgToGateServer(cBuff, 27);
	}

	delete pStrTok;
}


void CGame::SetPlayerReputation(int iClientH, char * pMsg, char cValue, UINT32 dwMsgSize)
{
 char   seps[] = "= \t\n";
 char   * token, cName[11], cBuff[256];
 class  CStrTok * pStrTok;
 int i;

	if (m_pClientList[iClientH] == 0) return;
	if ((dwMsgSize)	<= 0) return;
	if (m_pClientList[iClientH]->m_iLevel < 40) return;

	if ((m_pClientList[iClientH]->m_iTimeLeft_Rating != 0) || (m_pClientList[iClientH]->m_iPKCount != 0)) {
		// Æò°¡¸¦ ³»¸± ½Ã°£ÀÌ µÇÁö ¾Ê¾Ò°Å³ª ¹üÁËÀÚ´Â Æò°¡¸¦ ³»¸± ¼ö ¾ø´Ù. ±â´ÉÀ» »ç¿ëÇÒ ¼ö ¾ø´Ù.
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_CANNOTRATING, m_pClientList[iClientH]->m_iTimeLeft_Rating, 0, 0, 0);
		return;
	}
	else if (memcmp(m_pClientList[iClientH]->m_cLocation, "NONE", 4) == 0) {
		// ¼Ò¼Ó ¸¶À»ÀÌ ¾ø´Â »ç¶÷Àº ÆòÆÇÀ» ³»¸± ¼ö ¾ø´Ù.
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_CANNOTRATING, 0, 0, 0, 0);
		return;	
	}

	ZeroMemory(cName, sizeof(cName));
	ZeroMemory(cBuff, sizeof(cBuff));
	memcpy(cBuff, pMsg, dwMsgSize);

	pStrTok = new class CStrTok(cBuff, seps);
	token = pStrTok->pGet();
	token = pStrTok->pGet();
   	
	if (token != 0) {
		// tokenÀÌ °ð ÆòÆÇÀ» Æò°¡ÇÒ »ç¿ëÀÚ ÀÌ¸§ 
		if (strlen(token) > 10) 
			 memcpy(cName, token, 10);
		else memcpy(cName, token, strlen(token));
		
		for (i = 1; i < DEF_MAXCLIENTS; i++) 
		if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cCharName, cName, 10) == 0)) {
			
			if (i != iClientH) {
				if(memcmp(m_pClientList[iClientH]->m_cIPaddress,  m_pClientList[i]->m_cIPaddress,  strlen(m_pClientList[iClientH]->m_cIPaddress)) == 0)
                {
                    SendNotifyMsg(iClientH, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, "You can't send rep to a player with your same IP.");
                }
                else
                {
					// ÀÚ±â ÀÚ½ÅÀ» Æò°¡ÇÒ ¼ö´Â ¾ø´Ù.
					if (cValue == 0) 
						m_pClientList[i]->m_iRating -= m_iRatingAdjust;
					else if (cValue == 1)
						m_pClientList[i]->m_iRating += m_iRatingAdjust;
					
					// centu - rep limit
					if (m_pClientList[i]->m_iRating < -5000)
						m_pClientList[i]->m_iRating = -5000;

					if (m_pClientList[i]->m_iRating > 5000)
						m_pClientList[i]->m_iRating = 5000;

					// Æò°¡¸¦ ³»·ÈÀ¸¹Ç·Î ½Ã°£ °£°ÝÀ» Àç ÇÒ´ç. 
					m_pClientList[iClientH]->m_iTimeLeft_Rating = 20*60;

					SendNotifyMsg(0, i, DEF_NOTIFY_RATINGPLAYER, cValue, 0, 0, cName);
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_RATINGPLAYER, cValue, 0, 0, cName);

					SendNotifyMsg(0, i, DEF_NOTIFY_REPDGDEATHS, 0, 0, m_pClientList[i]->m_iRating, 0); // MORLA 2.4 - Actualiza la REP
					
					delete pStrTok;
					return;
				}
			}
		}
		// ÇöÀç Á¢¼ÓÁßÀÌ ¾Æ´Ï´Ù.
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_PLAYERNOTONGAME, 0, 0, 0, cName);
	}
	
	delete pStrTok;
}

bool CGame::bReadNotifyMsgListFile(char * cFn)
{
 FILE * pFile;
 HANDLE hFile;
 UINT32  dwFileSize;
 char * cp, * token, cReadMode;
 char seps[] = "=\t\n;";
 class CStrTok * pStrTok;
 int   i;

	cReadMode = 0;
	m_iTotalNoticeMsg = 0;

	hFile = CreateFile(cFn, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
	dwFileSize = GetFileSize(hFile, 0);
	if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);

	pFile = fopen(cFn, "rt");
	if (pFile == 0) {
		PutLogList("(!) Notify Message list file not found!...");
		return false;
	}
	else {
		PutLogList("(!) Reading Notify Message list file...");
		cp = new char[dwFileSize+2];
		ZeroMemory(cp, dwFileSize+2);
		fread(cp, dwFileSize, 1, pFile);

		pStrTok = new class CStrTok(cp, seps);
		token = pStrTok->pGet();
		while( token != 0 )   {
			
			if (cReadMode != 0) {
				switch (cReadMode) {
				case 1:
					for (i = 0; i < DEF_MAXNOTIFYMSGS; i++) 
					if (m_pNoticeMsgList[i] == 0) {
						m_pNoticeMsgList[i] = new class CMsg;	
						m_pNoticeMsgList[i]->bPut(0, token, strlen(token), 0, 0);
						m_iTotalNoticeMsg++;
						break;
					}			
					cReadMode = 0;
					break;
				}
			}
			else {
				if (memcmp(token, "notify_msg", 10) == 0) cReadMode = 1;
			}
			
			token = pStrTok->pGet();
			
		}

		delete pStrTok;
		delete[] cp;

		fclose(pFile);
	}

	return true;
}
void CGame::NoticeHandler()
{
 char  cTemp, cBuffer[1000], cKey;
 UINT32 dwSize, dwTime = timeGetTime();
 int i, iMsgIndex, iTemp;

	// Â°Ã¸ÃÃ¶Â»Ã§Ã‡Ã—Ã€ÃŒ 1Â°Â³ Ã€ÃŒÃ‡ÃÂ¶Ã³Â¸Ã© ÂºÂ¸Â³Â¾ Ã‡ÃŠÂ¿Ã¤Â°Â¡ Â¾Ã¸Â´Ã™.
	if (m_iTotalNoticeMsg <= 1) return;

	if ((dwTime - m_dwNoticeTime) > DEF_NOTICETIME) {
		// Â°Ã¸ÃÃ¶Â»Ã§Ã‡Ã—Ã€Â» Ã€Ã¼Â¼Ã›Ã‡Ã’ Â½ÃƒÂ°Â£Ã€ÃŒ ÂµÃ‡Â¾ÃºÂ´Ã™. 
		m_dwNoticeTime = dwTime;	
		do {
			iMsgIndex = iDice(1, m_iTotalNoticeMsg) - 1;
		} while (iMsgIndex == m_iPrevSendNoticeMsg);

		m_iPrevSendNoticeMsg = iMsgIndex;
		
		ZeroMemory(cBuffer, sizeof(cBuffer));
		if (m_pNoticeMsgList[iMsgIndex] != 0) {
			m_pNoticeMsgList[iMsgIndex]->Get(&cTemp, cBuffer, &dwSize, &iTemp, &cKey);
		}
		
		for (i = 1; i < DEF_MAXCLIENTS; i++)
		if (m_pClientList[i] != 0) {
			SendNotifyMsg(0, i, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, cBuffer);
		}
	}
}


void CGame::ResponseSavePlayerDataReplyHandler(char * pData, UINT32 dwMsgSize)
{
 char * cp, cCharName[11];
 int i;

	ZeroMemory(cCharName, sizeof(cCharName));
	
	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
	memcpy(cCharName, cp, 10);

	// Ã€ÃŒÃÂ¦ Ã€ÃŒ Ã€ÃŒÂ¸Â§Ã€Â» Â°Â®Â´Ã‚ Ã…Â¬Â¶Ã³Ã€ÃŒÂ¾Ã°Ã†Â®Â¸Â¦ ÃƒÂ£Â¾Ã† ÃÂ¢Â¼Ã“Ã€Â» Â²Ã·Â¾Ã®ÂµÂµ ÃÃÂ´Ã™Â´Ã‚ Â¸ÃžÂ½ÃƒÃÃ¶Â¸Â¦ ÂºÂ¸Â³Â½Â´Ã™. 
	for (i = 0; i < DEF_MAXCLIENTS; i++) 
	if (m_pClientList[i] != 0) {
		if (memcmp(m_pClientList[i]->m_cCharName, cCharName, 10) == 0) {
			// Â°Â°Ã€Âº Ã€ÃŒÂ¸Â§Ã€Â» Â°Â¡ÃÃ¸ Ã„Â³Â¸Â¯Ã…ÃÂ¸Â¦ ÃƒÂ£Â¾Ã’Â´Ã™. ÃÂ¢Â¼Ã“Ã€Â» ÃÂ¾Â·Ã¡Ã‡ÃÂ¶Ã³Â´Ã‚ Â¸ÃžÂ½ÃƒÃÃ¶Â¸Â¦ ÂºÂ¸Â³Â½Â´Ã™.
			SendNotifyMsg(0, i, DEF_NOTIFY_SERVERCHANGE, 0, 0, 0, 0);
		}
	}
}

int CGame::iGetExpLevel(UINT32 iExp)
{
 int i;

	 for (i = 1; i <= DEF_MAXEXPTABLE; i++)
	 {
		 if ((m_iLevelExpTable[i] <= iExp) && (m_iLevelExpTable[i + 1] > iExp)) return i;
	 }
	return 0;
}

void CGame::AdminOrder_CallGuard(int iClientH, char * pData, UINT32 dwMsgSize)
{
 char   seps[] = "= \t\n";
 char   * token, cName[11], cTargetName[11], cBuff[256], cNpcName[21], cNpcWaypoint[11];
 class  CStrTok * pStrTok;
 int i, iNamingValue, tX, tY;

	if (m_pClientList[iClientH] == 0) return;
	if ((dwMsgSize)	<= 0) return;

	if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelCallGaurd) {
		// Admin user levelÃ€ÃŒ Â³Â·Â¾Ã†Â¼Â­ Ã€ÃŒ Â±Ã¢Â´Ã‰Ã€Â» Â»Ã§Â¿Ã«Ã‡Ã’ Â¼Ã¶ Â¾Ã¸Â´Ã™.
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
		return;
	}

	ZeroMemory(cName, sizeof(cName));
	ZeroMemory(cNpcWaypoint, sizeof(cNpcWaypoint));
	ZeroMemory(cTargetName, sizeof(cTargetName));
	ZeroMemory(cNpcName, sizeof(cNpcName));
	ZeroMemory(cBuff, sizeof(cBuff));
	memcpy(cBuff, pData, dwMsgSize);

	pStrTok = new class CStrTok(cBuff, seps);
	token = pStrTok->pGet();
	token = pStrTok->pGet();
   	
	if (token != 0) {
		// tokenÃ€ÃŒ Â°Ã° ÃƒÂ¤Ã†ÃƒÃ€Â» ÂºÃ’Â°Â¡Â´Ã‰Ã‡ÃÂ°Ã” Â¸Â¸ÂµÃ© Â»Ã§Â¿Ã«Ã€Ãš Ã€ÃŒÂ¸Â§ 
		if (strlen(token) > 10) 
			 memcpy(cTargetName, token, 10);
		else memcpy(cTargetName, token, strlen(token));
	 		
		for (i = 1; i < DEF_MAXCLIENTS; i++) 
		if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cCharName, cTargetName, 10) == 0)) {
			// Â¸Ã±Ã‡Â¥ Ã„Â³Â¸Â¯Ã…ÃÂ¸Â¦ ÃƒÂ£Â¾Ã’Â´Ã™. 	
			
			if (memcmp(m_pClientList[i]->m_cMapName, "aresden", 7) == 0) 
				strcpy(cNpcName, "Guard-Aresden");			
			else if (memcmp(m_pClientList[i]->m_cMapName, "elvine", 6) == 0) 
				strcpy(cNpcName, "Guard-Elvine");
			else strcpy(cNpcName, "Guard-Neutral");  // <- Ã€ÃŒÂ°ÃÃ€Âº Â¸Â¶Ã€Â»Ã€ÃŒ Â¾Ã†Â´ÃÂ¹Ã‡Â·ÃŽ ÃÃŸÂ¸Â³ Â°Â¡ÂµÃ¥Â¸Â¦ Â»Ã½Â¼ÂºÂ½ÃƒÃ…Â²Â´Ã™.
			
			iNamingValue = m_pMapList[ m_pClientList[i]->m_cMapIndex ]->iGetEmptyNamingValue();
			if (iNamingValue != -1) {
				// Â´ÃµÃ€ÃŒÂ»Ã³ Ã€ÃŒ Â¸ÃŠÂ¿Â¡ NPCÂ¸Â¦ Â¸Â¸ÂµÃ©Â¼Ã¶ Â¾Ã¸Â´Ã™. Ã€ÃŒÂ¸Â§Ã€Â» Ã‡Ã’Â´Ã§Ã‡Ã’ Â¼Ã¶ Â¾Ã¸Â±Ã¢ Â¶Â§Â¹Â®.
			
				// NPCÂ¸Â¦ Â»Ã½Â¼ÂºÃ‡Ã‘Â´Ã™.
				wsprintf(cName, "XX%d", iNamingValue);
				cName[0] = '_';
				cName[1] = m_pClientList[i]->m_cMapIndex+65;
				
				tX = (int)m_pClientList[i]->m_sX;
				tY = (int)m_pClientList[i]->m_sY;
				if (bCreateNewNpc(cNpcName, cName, m_pMapList[ m_pClientList[i]->m_cMapIndex ]->m_cName, 0, 0, DEF_MOVETYPE_RANDOM, 
					              &tX, &tY, cNpcWaypoint, 0, 0, -1, false, true) == false) {
					// Â½Ã‡Ã†ÃÃ‡ÃŸÃ€Â¸Â¹Ã‡Â·ÃŽ Â¿Â¹Â¾Ã ÂµÃˆ NameValueÂ¸Â¦ Ã‡Ã˜ÃÂ¦Â½ÃƒÃ…Â²Â´Ã™.
					m_pMapList[ m_pClientList[i]->m_cMapIndex ]->SetNamingValueEmpty(iNamingValue);
				} 
				else {
					// Â°Â¡ÂµÃ¥Â¸Â¦ Â»Ã½Â¼ÂºÂ½ÃƒÃ„Ã—Â´Ã™. Â°Ã¸Â°Ã Â¸Ã±Ã‡Â¥Â¸Â¦ Ã‡Ã’Â´Ã§Ã‡Ã‘Â´Ã™.
					bSetNpcAttackMode(cName, i, DEF_OWNERTYPE_PLAYER, true);
				}
			}

			delete pStrTok;
			return;
		}

		// Ã‡Ã¶Ã€Ã§ ÃÂ¢Â¼Ã“ÃÃŸÃ€ÃŒ Â¾Ã†Â´ÃÂ´Ã™.
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_PLAYERNOTONGAME, 0, 0, 0, cTargetName);
	}

	delete pStrTok;
}

void CGame::AdminOrder_SummonDemon(int iClientH)
{
 char cName[21], cNpcName[21], cNpcWaypoint[11];	
 int iNamingValue, tX, tY;

	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsAdminCommandEnabled == false) return;

	if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelSummonDemon) {
		// Admin user levelÃ€ÃŒ Â³Â·Â¾Ã†Â¼Â­ Ã€ÃŒ Â±Ã¢Â´Ã‰Ã€Â» Â»Ã§Â¿Ã«Ã‡Ã’ Â¼Ã¶ Â¾Ã¸Â´Ã™.
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
		return;
	}

	iNamingValue = m_pMapList[ m_pClientList[iClientH]->m_cMapIndex ]->iGetEmptyNamingValue();
	if (iNamingValue != -1) {
		// Â´ÃµÃ€ÃŒÂ»Ã³ Ã€ÃŒ Â¸ÃŠÂ¿Â¡ NPCÂ¸Â¦ Â¸Â¸ÂµÃ©Â¼Ã¶ Â¾Ã¸Â´Ã™. Ã€ÃŒÂ¸Â§Ã€Â» Ã‡Ã’Â´Ã§Ã‡Ã’ Â¼Ã¶ Â¾Ã¸Â±Ã¢ Â¶Â§Â¹Â®.

		// NPCÂ¸Â¦ Â»Ã½Â¼ÂºÃ‡Ã‘Â´Ã™.
		ZeroMemory(cNpcName, sizeof(cNpcName));
		strcpy(cNpcName, "Demon");	
		
		ZeroMemory(cName, sizeof(cName));
		wsprintf(cName, "XX%d", iNamingValue);
		cName[0] = '_';
		cName[1] = m_pClientList[iClientH]->m_cMapIndex+65;
		
		ZeroMemory(cNpcWaypoint, sizeof(cNpcWaypoint));

		tX = (int)m_pClientList[iClientH]->m_sX;
		tY = (int)m_pClientList[iClientH]->m_sY;
		if (bCreateNewNpc(cNpcName, cName, m_pMapList[ m_pClientList[iClientH]->m_cMapIndex ]->m_cName, 0, (rand() % 9), 
			              DEF_MOVETYPE_RANDOM, &tX, &tY, cNpcWaypoint, 0, 0, -1, false, false) == false) {
			// Â½Ã‡Ã†ÃÃ‡ÃŸÃ€Â¸Â¹Ã‡Â·ÃŽ Â¿Â¹Â¾Ã ÂµÃˆ NameValueÂ¸Â¦ Ã‡Ã˜ÃÂ¦Â½ÃƒÃ…Â²Â´Ã™.
			m_pMapList[ m_pClientList[iClientH]->m_cMapIndex ]->SetNamingValueEmpty(iNamingValue);
		} 
		else {
			// Admin Log
			wsprintf(G_cTxt, "Admin Order(%s): Summon Demon", m_pClientList[iClientH]->m_cCharName);
			PutAdminLogFileList(G_cTxt);
		}
	}

}

// v1.4311-3 ÃƒÃŸÂ°Â¡ Â¿Ã®Â¿ÂµÃ€ÃšÂ°Â¡ Â»Ã§Ã…ÃµÃ€Ã¥Ã€Â» Â¿Â¹Â¾Ã Ã‡Ã‘Â´Ã™.
void CGame::AdminOrder_ReserveFightzone(int iClientH, char * pData, UINT32 dwMsgSize)
{
 char   seps[] = "= \t\n";
 char   * token, cBuff[256];
 class  CStrTok * pStrTok;
 int iNum ;

	if (m_pClientList[iClientH] == 0) return;
	if ((dwMsgSize)	<= 0) return;


	if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelReserveFightzone) {
		// Admin user levelÃ€ÃŒ Â³Â·Â¾Ã†Â¼Â­ Ã€ÃŒ Â±Ã¢Â´Ã‰Ã€Â» Â»Ã§Â¿Ã«Ã‡Ã’ Â¼Ã¶ Â¾Ã¸Â´Ã™.
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
		return;
	}

	ZeroMemory(cBuff, sizeof(cBuff));
	memcpy(cBuff, pData, dwMsgSize);

	pStrTok = new class CStrTok(cBuff, seps);
	token = pStrTok->pGet();
	token = pStrTok->pGet();
	
	if( m_iFightzoneNoForceRecall == 0 ) {
		m_iFightzoneNoForceRecall = 1 ; 
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_FIGHTZONERESERVE, -5, 0, 0, 0);
	} else {
		m_iFightzoneNoForceRecall = 0 ; 
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_FIGHTZONERESERVE, -4, 0, 0, 0);
	}

	if (token == 0) {
		delete pStrTok;
		return;
	}

	iNum = atoi(token) - 1;
			
	if ( m_iFightZoneReserve[iNum] != -1) {
		// Admin Log
		wsprintf(G_cTxt, "Admin Order(%s):  %d FightzoneReserved", m_pClientList[iClientH]->m_cCharName, iNum );
		PutAdminLogFileList(G_cTxt);
		
		m_iFightZoneReserve[iNum] = - 1 ;        // Â¿Ã®Â¿ÂµÃ€ÃšÂ°Â¡ Â¿Â¹Â¾Ã Ã‡Ã‘ Â»Ã§Ã…ÃµÃ€Ã¥Ã€Âº -1 Â°ÂªÃ€ÃŒ ÂµÃ©Â¾Ã®Â°Â£Â´Ã™.
												 // Ã€ÃŒÃÂ¦ÂºÃŽÃ…Ã Â°Ã¨Â¼Ã“ Â´Ã™Â¸Â¥ Ã€Â¯Ã€ÃºÂ´Ã‚ Â¿Â¹Â¾Ã Ã€ÃŒ ÂºÃ’Â°Â¡Â´Ã‰Ã‡ÃÂ´Ã™.
		// Â¿Ã®Â¿ÂµÃ€ÃšÂ°Â¡ Â»Ã§Ã…ÃµÃ€Ã¥ Â¿Â¹Â¾Ã Â¿Â¡ Â¼ÂºÂ°Ã¸Ã‡ÃÂ´Ã‚ Â°Ã¦Â¿Ã¬ 
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_FIGHTZONERESERVE, iNum+1, 0, 0, 0);
		
	}  else {	// Â¿Ã®Â¿ÂµÃ€ÃšÂ°Â¡ Â¿Â¹Â¾Ã Ã‡Ã‘ Â»Ã§Ã…ÃµÃ€Ã¥Ã€Â» Â´Ã™Â½Ãƒ Â¿Â¹Â¾Ã Ã‡ÃÂ¸Ã© Â¿Â¹Â¾Ã Ã€ÃŒ ÃƒÃ«Â¼Ã’ÂµÃˆÂ´Ã™.
		wsprintf(G_cTxt, "Admin Order(%s):  %d Cancel FightzoneReserved", m_pClientList[iClientH]->m_cCharName, iNum );
		PutAdminLogFileList(G_cTxt);

		m_iFightZoneReserve[iNum] = 0 ;       
		// Â¿Ã®Â¿ÂµÃ€ÃšÂ°Â¡ Â»Ã§Ã…ÃµÃ€Ã¥ Â¿Â¹Â¾Ã Ã€Â» ÃƒÃ«Â¼Ã’Ã‡ÃÂ´Ã‚ Â°Ã¦Â¿Ã¬       -3 Â°ÂªÃ€Â» Â³Ã‘Â°ÃœÃÃ˜Â´Ã™.
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_FIGHTZONERESERVE, -3, 0, 0, 0);

	}
		
}


void CGame::AdminOrder_CloseConn(int iClientH, char * pData, UINT32 dwMsgSize)
{
 char   seps[] = "= \t\n";
 char   * token, cTargetName[11], cBuff[256];
 class  CStrTok * pStrTok;
 int i;

	if (m_pClientList[iClientH] == 0) return;
	if ((dwMsgSize)	<= 0) return;

	

	if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelGMCloseconn) {
		// Admin user levelÃ€ÃŒ Â³Â·Â¾Ã†Â¼Â­ Ã€ÃŒ Â±Ã¢Â´Ã‰Ã€Â» Â»Ã§Â¿Ã«Ã‡Ã’ Â¼Ã¶ Â¾Ã¸Â´Ã™.
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
		return;
	}

	ZeroMemory(cTargetName, sizeof(cTargetName));
	ZeroMemory(cBuff, sizeof(cBuff));
	memcpy(cBuff, pData, dwMsgSize);

	pStrTok = new class CStrTok(cBuff, seps);
	token = pStrTok->pGet();
	token = pStrTok->pGet();
	if (token != 0) {
		// tokenÃ€ÃŒ Â°Ã° ÃƒÂ¤Ã†ÃƒÃ€Â» ÂºÃ’Â°Â¡Â´Ã‰Ã‡ÃÂ°Ã” Â¸Â¸ÂµÃ© Â»Ã§Â¿Ã«Ã€Ãš Ã€ÃŒÂ¸Â§ 
		if (strlen(token) > 10) 
			 memcpy(cTargetName, token, 10);
		else memcpy(cTargetName, token, strlen(token));
		
		for (i = 1; i < DEF_MAXCLIENTS; i++) 
		if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cCharName, cTargetName, 10) == 0)) {
			// Â¸Ã±Ã‡Â¥ Ã„Â³Â¸Â¯Ã…ÃÂ¸Â¦ ÃƒÂ£Â¾Ã’Â´Ã™. Â°Â­ÃÂ¦Â·ÃŽ ÃÂ¢Â¼Ã“Ã€Â» Â²Ã·Â´Ã‚Â´Ã™.	
			if (m_pClientList[i]->m_bIsInitComplete == true) {
				// v1.22 Â°Â­ÃÂ¦ ÃÂ¢Â¼Ã“ ÃÂ¾Â·Ã¡ÂµÃˆ Ã…Â¬Â¶Ã³Ã€ÃŒÂ¾Ã°Ã†Â®Â´Ã‚ ÂºÃ­Â¸Â®ÂµÃ¹ Â¾Ã†Ã€ÃÂ·Â£ÂµÃ¥Â·ÃŽ Ã…ÃšÂ·Â¹Ã†Ã·Ã†Â®ÂµÃˆÂ´Ã™.
				// Admin Log
				wsprintf(G_cTxt, "Admin Order(%s): Close Conn", m_pClientList[iClientH]->m_cCharName);
				PutAdminLogFileList(G_cTxt);
				DeleteClient(i, true, true);
			}

			delete pStrTok;
			return;
		}
		// Ã‡Ã¶Ã€Ã§ ÃÂ¢Â¼Ã“ÃÃŸÃ€ÃŒ Â¾Ã†Â´ÃÂ´Ã™.
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_PLAYERNOTONGAME, 0, 0, 0, cTargetName);
	}

	delete pStrTok;
}

void CGame::AdminOrder_CreateFish(int iClientH, char * pData, UINT32 dwMsgSize)
{
 char   seps[] = "= \t\n";
 char   * token, cBuff[256], cItemName[21];
 class  CStrTok * pStrTok;
 class  CItem   * pItem;
 int tX, tY, iType;

	if (m_pClientList[iClientH] == 0) return;
	if ((dwMsgSize)	<= 0) return;

	if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelCreateFish) {
		// Admin user levelÃ€ÃŒ Â³Â·Â¾Ã†Â¼Â­ Ã€ÃŒ Â±Ã¢Â´Ã‰Ã€Â» Â»Ã§Â¿Ã«Ã‡Ã’ Â¼Ã¶ Â¾Ã¸Â´Ã™.
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
		return;
	}

	tX = tY = iType = 0;

	ZeroMemory(cBuff, sizeof(cBuff));
	memcpy(cBuff, pData, dwMsgSize);

	pStrTok = new class CStrTok(cBuff, seps);
	token = pStrTok->pGet();
	
	token = pStrTok->pGet();
	if (token != 0) {
		tX = atoi(token);
	}

	token = pStrTok->pGet();
	if (token != 0) {
		tY = atoi(token);
	}

	token = pStrTok->pGet();
	if (token != 0) {
		iType = atoi(token);
	}

	if ((tX != 0) && (tY != 0) && (iType != 0)) {
		// Ã€ÃŒ ÃÃ‚Ã‡Â¥Â¿Â¡ Â¹Â°Â°Ã­Â±Ã¢Â¸Â¦ Â¸Â¸ÂµÃ§Â´Ã™.
		pItem = new class CItem;
		if (pItem == 0) {
			delete pStrTok;
			return;
		}
		ZeroMemory(cItemName, sizeof(cItemName));
		strcpy(cItemName, "Fish");
		if (_bInitItemAttr(pItem, cItemName) == true) {
   			iCreateFish(m_pClientList[iClientH]->m_cMapIndex, tX, tY, iType, pItem, 1, 60000*20);
		}
		else delete pItem;
   	}

	delete pStrTok;
}

void CGame::AdminOrder_Teleport(int iClientH, char * pData, UINT32 dwMsgSize)
{
 char   seps[] = "= \t\n";
 char   * token, cBuff[256], cMapName[11];
 class  CStrTok * pStrTok;
 int dX, dY;
 bool   bFlag;

	dX = dY = -1;

	if (m_pClientList[iClientH] == 0) return;
	if ((dwMsgSize)	<= 0) return;

	if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelTeleport) {
		// Admin user levelÃ€ÃŒ Â³Â·Â¾Ã†Â¼Â­ Ã€ÃŒ Â±Ã¢Â´Ã‰Ã€Â» Â»Ã§Â¿Ã«Ã‡Ã’ Â¼Ã¶ Â¾Ã¸Â´Ã™.
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
		return;
	}

	ZeroMemory(cMapName, sizeof(cMapName));

	ZeroMemory(cBuff, sizeof(cBuff));
	memcpy(cBuff, pData, dwMsgSize);

	pStrTok = new class CStrTok(cBuff, seps);
	token = pStrTok->pGet();
	
	token = pStrTok->pGet();
	if (token != 0) {
		
		strcpy(cMapName, token);
	}

	token = pStrTok->pGet();
	if (token != 0) {
		dX = atoi(token);
	}

	token = pStrTok->pGet();
	if (token != 0) {
		dY = atoi(token);
	}

	bFlag = false;

	// centuu
	for (int m = 0; m < DEF_MAXMAPS; m++) { //Enum all maps
		if (m_pMapList[m] != 0) {	//Is allocated map
			if (memcmp(m_pMapList[m]->m_cName, cMapName, 10) == 0) {
				bFlag = true;
				break;
			}
		}
	}
	
	if (bFlag)
		RequestTeleportHandler(iClientH, "2   ", cMapName, dX, dY);
	else ShowClientMsg(iClientH, "The map is currently offline.");

	delete pStrTok;
}

int CGame::_iCalcPlayerNum(char cMapIndex, short dX, short dY, char cRadius)
{
 int ix, iy, iRet;
 class CTile * pTile;	

	// ÃƒÂ³Â¸Â® Â¼Ã“ÂµÂµÂ¸Â¦ Â³Ã´Ã€ÃŒÂ±Ã¢ Ã€Â§Ã‡Ã˜ Ã‡Ã”Â¼Ã¶Â¸Â¦ Ã„ÃÃ‡ÃÃÃ¶ Â¾ÃŠÂ´Ã‚Â´Ã™.
	if ((cMapIndex < 0)	|| (cMapIndex > DEF_MAXMAPS)) return 0;
	if (m_pMapList[cMapIndex] == 0) return 0;

	iRet = 0;
	for (ix = dX - cRadius; ix <= dX + cRadius; ix++)
	for (iy = dY - cRadius; iy <= dY + cRadius; iy++) {
		if ( (ix < 0) || (ix >= m_pMapList[cMapIndex]->m_sSizeX) || 
			 (iy < 0) || (iy >= m_pMapList[cMapIndex]->m_sSizeY) ) {
			// ÃÃ‚Ã‡Â¥Â¸Â¦ Â¹Ã¾Â¾Ã®Â³ÂªÂ¹Ã‡Â·ÃŽ ÃƒÂ³Â¸Â®Ã‡ÃÃÃ¶ Â¾ÃŠÂ´Ã‚Â´Ã™.	
		}
		else {
			pTile = (class CTile *)(m_pMapList[cMapIndex]->m_pTile + ix + iy*m_pMapList[cMapIndex]->m_sSizeY);
			if ((pTile->m_sOwner != 0) && (pTile->m_cOwnerClass == DEF_OWNERTYPE_PLAYER))
				iRet++;
		}
	}

	return iRet;
}


void CGame::WeatherProcessor()
{
char cPrevMode;
int i, j;
UINT32 dwTime;
	dwTime = timeGetTime();
// SNOOPY for snow in winter / acomodado a estaciones de Argentina CENTU
SYSTEMTIME SysTime;

	for (i = 0; i < DEF_MAXMAPS; i++) 
	{
		if ((m_pMapList[i] != 0) && (m_pMapList[i]->m_bIsFixedDayMode == false)) 
		{	cPrevMode = m_pMapList[i]->m_cWhetherStatus;
			if (m_pMapList[i]->m_cWhetherStatus != 0) 
			{	if ((dwTime - m_pMapList[i]->m_dwWhetherStartTime) > m_pMapList[i]->m_dwWhetherLastTime) 
					m_pMapList[i]->m_cWhetherStatus = 0;
			}else 
			{	GetLocalTime(&SysTime);
				switch((short)SysTime.wMonth) {
				case 6: 
				case 7:
				case 8: // Winter
					if (iDice(1,100) == 13) 
					{	m_pMapList[i]->m_cWhetherStatus = iDice(3,2);
						m_pMapList[i]->m_dwWhetherStartTime = dwTime;
						m_pMapList[i]->m_dwWhetherLastTime  = 60000*3 + 60000*iDice(2,7);					
					}
					break;
				case 12:
				case 1: 
				case 2:  // Summer weather: rare thunderstorms heavy rain
					// Chrismas
					if (((short)SysTime.wMonth == 12) && ((short)SysTime.wDay >= 8)) 						
					{	m_pMapList[i]->m_cWhetherStatus = iDice(1,2)+4;
						m_pMapList[i]->m_dwWhetherStartTime = dwTime;
						m_pMapList[i]->m_dwWhetherLastTime  = 60000*3 + 60000*iDice(2,7);					
					}
					else
					{
						if (iDice(1,800) == 13) 
						{	m_pMapList[i]->m_cWhetherStatus = 3;
							m_pMapList[i]->m_dwWhetherStartTime = dwTime;
							m_pMapList[i]->m_dwWhetherLastTime  = 60000 + 60000*iDice(1,3);
						}
					}
					break;								
				default:   // Standard Weather
					if (iDice(1,300) == 13) 
					{	m_pMapList[i]->m_cWhetherStatus = iDice(1,3); 
						m_pMapList[i]->m_dwWhetherStartTime = dwTime;
						m_pMapList[i]->m_dwWhetherLastTime  = 60000*3 + 60000*iDice(1,7);
					}
					break;
				}
			}
			if (m_pMapList[i]->m_bIsSnowEnabled == true) 
			{	m_pMapList[i]->m_cWhetherStatus = iDice(1,3)+3;
				m_pMapList[i]->m_dwWhetherStartTime = dwTime;
				m_pMapList[i]->m_dwWhetherLastTime  = 60000*3 + 60000*iDice(1,7);
			}

			if (cPrevMode != m_pMapList[i]->m_cWhetherStatus) 
			{	m_pMapList[i]->m_cThunder = 0;
				for (j = 1; j < DEF_MAXCLIENTS; j++) 
				if ((m_pClientList[j] != 0) && (m_pClientList[j]->m_bIsInitComplete == true) && (m_pClientList[j]->m_cMapIndex == i)) 
					SendNotifyMsg(0, j, DEF_NOTIFY_WHETHERCHANGE, m_pMapList[i]->m_cWhetherStatus, 0, 0, 0);
			}
		} //If
	} //for Loop
}

int CGame::iGetPlayerRelationship(int iClientH, int iOpponentH)
{
 int iRet;
	
	if (m_pClientList[iClientH] == 0) return 0;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return 0;

	if (m_pClientList[iOpponentH] == 0) return 0;
	if (m_pClientList[iOpponentH]->m_bIsInitComplete == false) return 0;

	iRet = 0;

	if (m_pClientList[iClientH]->m_iPKCount != 0) {
		// ÂºÂ»Ã€ÃŽÃ€ÃŒ Â¹Ã¼ÃÃ‹Ã€ÃšÃ€ÃŒÃ‡ÃÂ¸Ã© Â»Ã³Â´Ã«Â¹Ã¦Ã€ÃŒ Â¸Â¶Ã€Â»Ã€ÃŒ Â°Â°Ã€Âº Â°Ã¦Â¿Ã¬ Ã€Â§Ã‡Ã¹. Â´Ã™Â¸Â£Â¸Ã© Ã€Ã» 
		if ((memcmp(m_pClientList[iClientH]->m_cLocation, m_pClientList[iOpponentH]->m_cLocation, 10) == 0) &&
			(memcmp(m_pClientList[iClientH]->m_cLocation, "NONE", 4) != 0) && (memcmp(m_pClientList[iOpponentH]->m_cLocation, "NONE", 4) != 0)) {
			 iRet = 7; // Ã€Â§Ã‡Ã¹ 
		}
		else iRet = 2; // Ã€Ã»   
	}
	else if (m_pClientList[iOpponentH]->m_iPKCount != 0) {
		// Â»Ã³Â´Ã«Â¹Ã¦Ã€ÃŒ Â¹Ã¼ÃÃ‹Ã€ÃšÃ€ÃŒÃ‡ÃÂ¸Ã©
		if ((memcmp(m_pClientList[iClientH]->m_cLocation, m_pClientList[iOpponentH]->m_cLocation, 10) == 0) &&
			(memcmp(m_pClientList[iClientH]->m_cLocation, "NONE", 4) != 0)) 
			 iRet = 6; // Â¸Â¶Ã€Â»Ã€ÃŒ Â°Â°Ã€Â¸Â¸Ã© PK
		else iRet = 2; // Â´Ã™Â¸Â£Â¸Ã© Â±Ã—Â³Ã‰ Ã€Ã» 
	}
	else {
		if (m_pClientList[iClientH]->m_cSide != m_pClientList[iOpponentH]->m_cSide) {
			if ((m_pClientList[iClientH]->m_cSide != 0) && (m_pClientList[iOpponentH]->m_cSide != 0)) {
				// ÂµÃ‘ Â´Ã™ 0(Traveler)Ã€ÃŒ Â¾Ã†Â´ÃÂ¸Ã© Ã€Ã»Ã€ÃŒÂ´Ã™.
				iRet = 2;
			}
			else {
				iRet = 0;
			}
		}	
		else {
			// Â¾Ã†Â±ÂºÃ€ÃŒÂ´Ã™. Â±Ã¦ÂµÃ¥Â¿Ã¸Â¿Â©ÂºÃŽÂ¸Â¦ Ã†Ã‡Â´Ãœ.
			if ((memcmp(m_pClientList[iClientH]->m_cGuildName, m_pClientList[iOpponentH]->m_cGuildName, 20) == 0) &&
				(memcmp(m_pClientList[iClientH]->m_cGuildName, "NONE", 4) != 0) ) {
				// Â±Ã¦ÂµÃ¥ Ã€ÃŒÂ¸Â§Ã€ÃŒ Â°Â°Â´Ã™. 
				if (m_pClientList[iOpponentH]->m_iGuildRank == 0)
					 iRet = 5;	// Â±Ã¦ÂµÃ¥ Â·Â©Ã…Â©Â°Â¡ 0. Â±Ã¦ÂµÃ¥Â¸Â¶Â½ÂºÃ…ÃÃ€ÃŒÂ´Ã™.
				else iRet = 3;	// Â°Â°Ã€Âº Â±Ã¦ÂµÃ¥Â¿Ã¸
			}
			else 
			if ((memcmp(m_pClientList[iClientH]->m_cLocation, m_pClientList[iOpponentH]->m_cLocation, 10) == 0) &&
				(memcmp(m_pClientList[iClientH]->m_cGuildName, "NONE", 4) != 0) &&
				(memcmp(m_pClientList[iOpponentH]->m_cGuildName, "NONE", 4) != 0) &&
				(memcmp(m_pClientList[iClientH]->m_cGuildName, m_pClientList[iOpponentH]->m_cGuildName, 20) != 0)) {
				// Â±Ã¦ÂµÃ¥ Ã€Â§Ã„Â¡Â´Ã‚ Â°Â°Â°Ã­ Â±Ã¦ÂµÃ¥ Ã€ÃŒÂ¸Â§Ã€ÃŒ Â´Ã™Â¸Â£Â´Ã™.
				iRet = 4; // Â´Ã™Â¸Â¥ Â±Ã¦ÂµÃ¥Â¿Ã¸ 
			}
			else iRet = 1; // Â±Ã—Â³Ã‰ Â°Â°Ã€ÂºÃ†Ã­ 
		}
	}

	return iRet;
}

int CGame::iGetPlayerABSStatus(int iWhatH, int iRecvH)
{
	int iRet;

	if (m_pClientList[iWhatH] == 0) return 0;
	if (m_pClientList[iRecvH] == 0) return 0;

	iRet = 0;


		if (m_pClientList[iWhatH]->m_iPKCount != 0) {
			iRet = 8;
		}

		if (m_pClientList[iWhatH]->m_cSide != 0) {
			iRet = iRet | 4;
		}

		if (m_pClientList[iWhatH]->m_cSide == 1) {
			iRet = iRet | 2;
		}

		if (m_pClientList[iWhatH]->m_bIsPlayerCivil == true)
			iRet = iRet | 1;

		return iRet;
}

int CGame::_iForcePlayerDisconect(int iNum)
{
 int i, iCnt;

	iCnt = 0;
	for (i = 1; i < DEF_MAXCLIENTS; i++)
	if (m_pClientList[i] != 0) {
		if (m_pClientList[i]->m_bIsInitComplete == true)
			 DeleteClient(i, true, true);
		else DeleteClient(i, false, false);
		iCnt++;
		if (iCnt >= iNum) break;
	}

	return iCnt;
}

void CGame::SpecialEventHandler()
{
 UINT32 dwTime;
 
	// Ã†Â¯ÂºÂ°Ã‡Ã‘ Ã€ÃŒÂºÂ¥Ã†Â®Â¸Â¦ Â»Ã½Â¼ÂºÃ‡Ã‘Â´Ã™. 
	dwTime = timeGetTime();

	if ((dwTime - m_dwSpecialEventTime) < DEF_SPECIALEVENTTIME) return; // DEF_SPECIALEVENTTIME
	m_dwSpecialEventTime  = dwTime;
	m_bIsSpecialEventTime = true;
	
	switch (iDice(1,180)) {
	case 98: m_cSpecialEventType = 2; break; // ÂµÂ¥Â¸Ã³ ÃˆÂ¤Ã€Âº Ã€Â¯Â´ÃÃ„ÃœÃ€ÃŒ Â³ÂªÂ¿Ãƒ Â°Â¡Â´Ã‰Â¼ÂºÃ€Âº 30ÂºÃÂ¿Â¡ 1Â¹Ã¸ 1/30
	default: m_cSpecialEventType = 1; break;
	}
}

void CGame::AdminOrder_CheckIP(int iClientH, char *pData, UINT32 dwMsgSize)
{
 char   seps[] = "= \t\n";
 char   * token, cBuff[256], cIP[21], cInfoString[500];
 class  CStrTok * pStrTok;
 int i;

	if (m_pClientList[iClientH] == 0) return;
	if ((dwMsgSize)	<= 0) return;

	if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelCheckIP) {
		// Admin user levelÃ€ÃŒ Â³Â·Â¾Ã†Â¼Â­ Ã€ÃŒ Â±Ã¢Â´Ã‰Ã€Â» Â»Ã§Â¿Ã«Ã‡Ã’ Â¼Ã¶ Â¾Ã¸Â´Ã™.
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
		return;
	}

	ZeroMemory(cBuff, sizeof(cBuff));
	memcpy(cBuff, pData, dwMsgSize);

	pStrTok = new class CStrTok(cBuff, seps);
	token = pStrTok->pGet();
	token = pStrTok->pGet();
	
	if (token != 0) {
		// tokenÃ€ÃŒ IP addressÃ€ÃŒÂ´Ã™. 
		ZeroMemory(cIP, sizeof(cIP));
		strcpy(cIP, token);

		for (i = 1; i < DEF_MAXCLIENTS; i++) 
		if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cIPaddress, cIP, strlen(cIP)) == 0)) {
			// Â¿Ã¤Â±Â¸Ã‡Ã‘ ÃÃ–Â¼Ã’Â¿Ã Ã€ÃÃ„Â¡Ã‡ÃÂ´Ã‚ Ã…Â¬Â¶Ã³Ã€ÃŒÂ¾Ã°Ã†Â® Â¹ÃŸÂ°ÃŸ. 
			ZeroMemory(cInfoString, sizeof(cInfoString));
			wsprintf(cInfoString, "Name(%s/%s) Loc(%s: %d %d) Level(%d:%d) Init(%d) IP(%s)", 
				     m_pClientList[i]->m_cAccountName, m_pClientList[i]->m_cCharName, m_pClientList[i]->m_cMapName, 
					 m_pClientList[i]->m_sX, m_pClientList[i]->m_sY, 
					 m_pClientList[i]->m_iLevel, 0,
					 m_pClientList[i]->m_bIsInitComplete, m_pClientList[i]->m_cIPaddress);

			SendNotifyMsg(0, iClientH, DEF_NOTIFY_IPACCOUNTINFO, 0, 0, 0, cInfoString);
		}
	}

	
	delete pStrTok;
}

void CGame::ToggleSafeAttackModeHandler(int iClientH) //v1.1
{
	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
	if (m_pClientList[iClientH]->m_bIsKilled == true) return;

	// Â¾ÃˆÃ€Ã¼ Â°Ã¸Â°Ã Â¸Ã°ÂµÃ¥Â¸Â¦ Ã…Ã¤Â±Ã›Ã‡Ã‘Â´Ã™. 
	if (m_pClientList[iClientH]->m_bIsSafeAttackMode == true) 
		 m_pClientList[iClientH]->m_bIsSafeAttackMode = false;
	else m_pClientList[iClientH]->m_bIsSafeAttackMode = true;

	SendNotifyMsg(0, iClientH, DEF_NOTIFY_SAFEATTACKMODE, 0, 0, 0, 0);
}

void CGame::ForceDisconnectAccount(char *pAccountName, UINT16 wCount)
{
 int i;

	// ÃÃŸÂºÂ¹ÂµÃˆ Â°Ã¨ÃÂ¤Ã€Â» Â°Â®Â°Ã­Ã€Ã–Â´Ã‚ Ã„Â³Â¸Â¯Ã…ÃÂ¸Â¦ Â»Ã¨ÃÂ¦Ã‡Ã‘Â´Ã™. 
	for (i = 1; i < DEF_MAXCLIENTS; i++)
	if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cAccountName, pAccountName, 10) == 0)) {
		wsprintf(G_cTxt, "<%d> Force disconnect account: CharName(%s) AccntName(%s) Count(%d)", i, m_pClientList[i]->m_cCharName, m_pClientList[i]->m_cAccountName, wCount);
		PutLogList(G_cTxt);
		
		//v1.4312
		SendNotifyMsg(0, i, DEF_NOTIFY_FORCEDISCONN, wCount, 0, 0, 0);
	}
}

void CGame::AdminOrder_Polymorph(int iClientH, char *pData, UINT32 dwMsgSize)
{
 char   seps[] = "= \t\n";
 char   * token, cBuff[256]; 
 class  CStrTok * pStrTok;

	if (m_pClientList[iClientH] == 0) return;
	if ((dwMsgSize)	<= 0) return;

	if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelPolymorph) {
		// Admin user levelÃ€ÃŒ Â³Â·Â¾Ã†Â¼Â­ Ã€ÃŒ Â±Ã¢Â´Ã‰Ã€Â» Â»Ã§Â¿Ã«Ã‡Ã’ Â¼Ã¶ Â¾Ã¸Â´Ã™.
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
		return;
	}

	ZeroMemory(cBuff, sizeof(cBuff));
	memcpy(cBuff, pData, dwMsgSize);

	pStrTok = new class CStrTok(cBuff, seps);
	token = pStrTok->pGet();
	token = pStrTok->pGet();

	if (token != 0) {
		if (memcmp(token, "off", 3) == 0 || memcmp(token, "0", 1) == 0)
			m_pClientList[iClientH]->m_sType = m_pClientList[iClientH]->m_sOriginalType;

		else if (memcmp(token, "Slime", 5) == 0)
			m_pClientList[iClientH]->m_sType = 10;

		else if (memcmp(token, "Skeleton", 8) == 0)
			m_pClientList[iClientH]->m_sType = 11;

		else if (memcmp(token, "Stone-Golem", 11) == 0)
			m_pClientList[iClientH]->m_sType = 12;

		else if (memcmp(token, "Cyclops", 7) == 0)
			m_pClientList[iClientH]->m_sType = 13;

		else if (memcmp(token, "Orc", 3) == 0)
			m_pClientList[iClientH]->m_sType = 14;

		else if (memcmp(token, "Giant-Ant", 9) == 0)
			m_pClientList[iClientH]->m_sType = 16;

		else if (memcmp(token, "Scorpion", 8) == 0)
			m_pClientList[iClientH]->m_sType = 17;

		else if (memcmp(token, "Zombie", 6) == 0)
			m_pClientList[iClientH]->m_sType = 18;

		else if (memcmp(token, "Amphis", 6) == 0)
			m_pClientList[iClientH]->m_sType = 22;

		else if (memcmp(token, "Clay-Golem", 10) == 0)
			m_pClientList[iClientH]->m_sType = 23;

		else if (memcmp(token, "Hellbound", 9) == 0)
			m_pClientList[iClientH]->m_sType = 27;

		else if (memcmp(token, "Troll", 5) == 0)
			m_pClientList[iClientH]->m_sType = 28;

		else if (memcmp(token, "Ogre", 4) == 0)
			m_pClientList[iClientH]->m_sType = 29;

		else if (memcmp(token, "Liche", 5) == 0)
			m_pClientList[iClientH]->m_sType = 30;

		else if (memcmp(token, "Demon", 5) == 0)
			m_pClientList[iClientH]->m_sType = 31;

		else if (memcmp(token, "Unicorn", 7) == 0)
			m_pClientList[iClientH]->m_sType = 32;

		else if (memcmp(token, "WereWolf", 8) == 0)
			m_pClientList[iClientH]->m_sType = 33;

		else if (memcmp(token, "Stalker", 7) == 0)
			m_pClientList[iClientH]->m_sType = 48;

		else if (memcmp(token, "Hellclaw", 8) == 0)
			m_pClientList[iClientH]->m_sType = 49;

		else if (memcmp(token, "Tigerworm", 8) == 0)
			m_pClientList[iClientH]->m_sType = 50;

		else if (memcmp(token, "Gagoyle", 7) == 0)
			m_pClientList[iClientH]->m_sType = 52;

		else if (memcmp(token, "Beholder", 8) == 0)
			m_pClientList[iClientH]->m_sType = 53;

		else if (memcmp(token, "Dark-Elf", 8) == 0)
			m_pClientList[iClientH]->m_sType = 54;

		else if (memcmp(token, "Rabbit", 6) == 0)
			m_pClientList[iClientH]->m_sType = 55;

		else if (memcmp(token, "Cat", 3) == 0)
			m_pClientList[iClientH]->m_sType = 56;

		else if (memcmp(token, "Giant-Frog", 10) == 0)
			m_pClientList[iClientH]->m_sType = 57;

		else if (memcmp(token, "Mountain-Giant", 14) == 0)
			m_pClientList[iClientH]->m_sType = 58;

		else if (memcmp(token, "Ettin", 5) == 0)
			m_pClientList[iClientH]->m_sType = 59;

		else if (memcmp(token, "Cannibal-Plant", 13) == 0)
			m_pClientList[iClientH]->m_sType = 60;

		else if (memcmp(token, "Rudolph", 7) == 0)
			m_pClientList[iClientH]->m_sType = 61;

		else if (memcmp(token, "DireBoar", 8) == 0)
			m_pClientList[iClientH]->m_sType = 62;

		else if (memcmp(token, "Frost", 5) == 0)
			m_pClientList[iClientH]->m_sType = 63;

		else if (memcmp(token, "Crops", 5) == 0)
			m_pClientList[iClientH]->m_sType = 64;

		else if (memcmp(token, "Ice-Golem", 9) == 0)
			m_pClientList[iClientH]->m_sType = 65;

		else if (memcmp(token, "Barlog", 6) == 0)
			m_pClientList[iClientH]->m_sType = 70;

		else if (memcmp(token, "Centaurus", 9) == 0)
			m_pClientList[iClientH]->m_sType = 71;

		else if (memcmp(token, "Claw-Turtle", 11) == 0)
			m_pClientList[iClientH]->m_sType = 72;

		else if (memcmp(token, "Giant-Crayfish", 14) == 0)
			m_pClientList[iClientH]->m_sType = 74;

		else if (memcmp(token, "Giant-Lizard", 12) == 0)
			m_pClientList[iClientH]->m_sType = 75;

		else if (memcmp(token, "Giant-Plant", 11) == 0)
			m_pClientList[iClientH]->m_sType = 76;

		else if (memcmp(token, "MasterMage-Orc", 14) == 0)
			m_pClientList[iClientH]->m_sType = 77;

		else if (memcmp(token, "Minotaurs", 9) == 0)
			m_pClientList[iClientH]->m_sType = 78;

		else if (memcmp(token, "Nizie", 5) == 0)
			m_pClientList[iClientH]->m_sType = 79;

		else if (memcmp(token, "Tentocle", 8) == 0)
			m_pClientList[iClientH]->m_sType = 80;
	}
	SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
	delete pStrTok;
}

void CGame::AdminOrder_SetInvi(int iClientH, char *pData, UINT32 dwMsgSize)
{
 char   seps[] = "= \t\n";
 char   * token, cBuff[256]; 
 class  CStrTok * pStrTok;

	if (m_pClientList[iClientH] == 0) return;
	if ((dwMsgSize)	<= 0) return;

	if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelSetInvis) {
		// Admin user levelÃ€ÃŒ Â³Â·Â¾Ã†Â¼Â­ Ã€ÃŒ Â±Ã¢Â´Ã‰Ã€Â» Â»Ã§Â¿Ã«Ã‡Ã’ Â¼Ã¶ Â¾Ã¸Â´Ã™.
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
		return;
	}

	ZeroMemory(cBuff, sizeof(cBuff));
	memcpy(cBuff, pData, dwMsgSize);

	pStrTok = new class CStrTok(cBuff, seps);
	token = pStrTok->pGet();
	token = pStrTok->pGet();

	if (token != 0) {
		// Ã€ÃŒ Â°ÂªÃ€ÃŒ Â¹Â®Ã€Ãš '1'Ã€ÃŒÂ¸Ã© Ã…ÃµÂ¸Ã­Ã€Â¸Â·ÃŽ Â¼Â¼Ã†Â®. '0'Ã€ÃŒÂ¸Ã© Ã‡Ã˜ÃÂ¦ 
		if (token[0] == '1') SetInvisibilityFlag(iClientH, DEF_OWNERTYPE_PLAYER, true);
		else if (token[0] == '0') SetInvisibilityFlag(iClientH, DEF_OWNERTYPE_PLAYER, false);
	}

	delete pStrTok;
}

bool CGame::bOnClose()
{
	if (m_bIsServerShutdowned == false) 
		if (MessageBox(0, "Player data not saved! Shutdown server now?", m_cServerName, MB_ICONEXCLAMATION | MB_YESNO) == IDYES) return true;
		else return false;
	else return true;
}

void CGame::LocalSavePlayerData(int iClientH)
{
	char* pData, * cp, cFn[256], cDir[256], cTxt[256], cCharDir[256];
	int    iSize;
	FILE* pFile;
	SYSTEMTIME SysTime;

	if (m_pClientList[iClientH] == NULL) return;

	if (m_bSQLMode) return;

	pData = new char[30000];
	if (pData == NULL) return;
	ZeroMemory(pData, 30000);

	cp = (char*)(pData);
	iSize = _iComposePlayerDataFileContents(iClientH, cp);

	GetLocalTime(&SysTime);
	ZeroMemory(cCharDir, sizeof(cDir));
	wsprintf(cCharDir, "Character_%d_%d_%d_%d", SysTime.wMonth, SysTime.wDay, SysTime.wHour, SysTime.wMinute);

	ZeroMemory(cDir, sizeof(cDir));
	ZeroMemory(cFn, sizeof(cFn));
	strcat(cFn, cCharDir);
	strcat(cFn, "\\");
	strcat(cFn, "\\");
	wsprintf(cTxt, "AscII%d", (unsigned char)m_pClientList[iClientH]->m_cCharName[0]);
	strcat(cFn, cTxt);
	strcpy(cDir, cFn);
	strcat(cFn, "\\");
	strcat(cFn, "\\");
	strcat(cFn, m_pClientList[iClientH]->m_cCharName);
	strcat(cFn, ".txt");

	_mkdir(cCharDir);
	_mkdir(cDir);

	if (iSize == 0) {
		PutLogList("(!) Character data body empty: Cannot create & save temporal player data file.");
		delete[] pData;
		return;
	}

	pFile = fopen(cFn, "wt");
	if (pFile == NULL) {
		wsprintf(cTxt, "(!) Cannot create temporal player data file : Name(%s)", cFn);
		PutLogList(cTxt);
	}
	else {
		wsprintf(cTxt, "(!) temporal player data file saved : Name(%s)", cFn);
		PutLogList(cTxt);
		fwrite(cp, iSize, 1, pFile);
		fclose(pFile);
	}

	delete[] pData;
}

void CGame::CheckFireBluring(char cMapIndex, int sX, int sY)
{
 int ix, iy, iItemNum;
  short sItemID;
 char  cItemColor;
 class CItem * pItem;
 UINT32 dwItemAttr;

	for (ix = sX -1; ix <= sX +1; ix++)
	for (iy = sY -1; iy <= sY +1; iy++) {
		// Â¸Â¸Â¾Ã  Ã€ÃŒ Ã€Â§Ã„Â¡Â¿Â¡ Â¹ÃŸÃˆÂ­Â¼Âº Â¾Ã†Ã€ÃŒÃ…Ã›Ã€ÃŒ Ã€Ã–Â´Ã™Â¸Ã© ÃÃ¶Â¿Ã¬Â°Ã­ ÂºÃ’ Â¿Ã€ÂºÃªÃÂ§Ã†Â®Â¸Â¦ Â³ÃµÂ´Ã‚Â´Ã™.	
		iItemNum = m_pMapList[cMapIndex]->iCheckItem(ix, iy);
				
		switch (iItemNum) {
		case 355: 
			// Â¼Â®Ã…ÂºÃ€ÃŒÂ´Ã™. Â¾Ã†Ã€ÃŒÃ…Ã›Ã€Â» ÃÃ¶Â¿Ã¬Â°Ã­ ÂºÃ’Ã€Â» Â¸Â¸ÂµÃ§Â´Ã™.
			pItem = m_pMapList[cMapIndex]->pGetItem(ix, iy, &sItemID, &cItemColor, &dwItemAttr);

			if (pItem != 0) delete pItem;
			iAddDynamicObjectList(0, 0, DEF_DYNAMICOBJECT_FIRE, cMapIndex, ix, iy, 6000);	

			SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_SETITEM, cMapIndex,
				ix, iy, sItemID, 0, cItemColor, dwItemAttr);

			break;
		}
	}
}

void CGame::AdminOrder_GetNpcStatus(int iClientH, char * pData, UINT32 dwMsgSize)
{
 char   seps[] = "= \t\n";
 char   * token, cBuff[256], cNpcName[256];
 class  CStrTok * pStrTok;
 int i;

	if (m_pClientList[iClientH] == 0) return;
	if ((dwMsgSize)	<= 0) return;

	if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelGetNpcStatus) {
		// Admin user levelÃ€ÃŒ Â³Â·Â¾Ã†Â¼Â­ Ã€ÃŒ Â±Ã¢Â´Ã‰Ã€Â» Â»Ã§Â¿Ã«Ã‡Ã’ Â¼Ã¶ Â¾Ã¸Â´Ã™.
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
		return;
	}

	ZeroMemory(cBuff, sizeof(cBuff));
	memcpy(cBuff, pData, dwMsgSize);

	pStrTok = new class CStrTok(cBuff, seps);
	
	token = pStrTok->pGet();
	token = pStrTok->pGet();
	if (token != 0) {
		ZeroMemory(cNpcName, sizeof(cNpcName));
		strcpy(cNpcName, token);
	}

	for (i = 0; i < DEF_MAXNPCS; i++) 
	if (m_pNpcList[i] != 0) {
		if (memcmp(m_pNpcList[i]->m_cName, cNpcName, 5) == 0) {
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINIFO, 1, i, 0, 0);	
		}
	}

	delete pStrTok;
}

static int ___iCAB5[]  = {0,0, 0,1,2};
static int ___iCAB6[]  = {0,0, 0,0,0};
static int ___iCAB7[]  = {0,0, 1,2,3};
static int ___iCAB8[]  = {0,0, 1,3,5};
static int ___iCAB9[]  = {0,0, 2,4,8};
static int ___iCAB10[] = {0,0, 1,2,3};

int CGame::iGetComboAttackBonus(int iSkill, int iComboCount)
{
	if (iComboCount <= 1) return 0;
	if (iComboCount > 6) return 0;
	switch (iSkill) {
	case 5:
		return ___iCAB5[iComboCount];
	case 6:
		return ___iCAB6[iComboCount];
	case 7:
		return ___iCAB7[iComboCount];
	case 8:
		return ___iCAB8[iComboCount];
	case 9:
		return ___iCAB9[iComboCount];
	case 10:
		return ___iCAB10[iComboCount];
	case 14:
		return ___iCAB6[iComboCount];
	case 21:
		return ___iCAB10[iComboCount];
	}

	return 0;
}

void CGame::_BWM_Init(int iClientH, char *pData)
{
	if (m_pClientList[iClientH] == 0) return;

	m_pClientList[iClientH]->m_bIsBWMonitor = true;
	wsprintf(G_cTxt, "(*) BWMonitor(%d) registered.", iClientH);
	PutLogList(G_cTxt);
}

void CGame::_BWM_Command_Shutup(char *pData)
{
 char * cp, cName[11];
 int i;

	cp = (char *)(pData + 16);

	ZeroMemory(cName, sizeof(cName));
	memcpy(cName, cp, 10);

	for (i = 1; i < DEF_MAXCLIENTS; i++) 
	if (m_pClientList[i] != 0) {
		if (memcmp(m_pClientList[i]->m_cCharName, cName, 10) == 0) {
			m_pClientList[i]->m_iTimeLeft_ShutUp = 20*3*10; // 1Ã€ÃŒ 3ÃƒÃŠÂ´Ã™. 20Ã€ÃŒÂ¸Ã© 1ÂºÃ Â¿Ã¥Ã€Â» Ã‡ÃÂ¸Ã© Â¹Â«ÃÂ¶Â°Ã‡ Ã†Ã¤Â³ÃŽÃ†Â¼ 10ÂºÃ  
			SendNotifyMsg(0, i, DEF_NOTIFY_PLAYERSHUTUP, 20*3*10, 0, 0, cName);
			
			// Admin Log
			wsprintf(G_cTxt, "BWordMonitor shutup(%s %d)", cName, 20*3*10);
			PutAdminLogFileList(G_cTxt);

			break;
		}
	}
}

int CGame::iGetMaxHP(int iClientH, bool bBloodEffect)
{
	int iRet;
	if (m_pClientList[iClientH] == 0) return 0;
	iRet = (3*m_pClientList[iClientH]->m_iVit) + (2 * m_pClientList[iClientH]->m_iLevel) + ((m_pClientList[iClientH]->m_iStr)/2);

	if ((bBloodEffect == true) && (m_pClientList[iClientH]->m_iSideEffect_MaxHPdown != 0))
		iRet -= (iRet/m_pClientList[iClientH]->m_iSideEffect_MaxHPdown);

	return iRet;
}

int CGame::iGetMaxMP(int iClientH)
{

	if (m_pClientList[iClientH] == 0) return 0;

	return (2*(m_pClientList[iClientH]->m_iMag)) + (2 * (m_pClientList[iClientH]->m_iLevel)) + ((m_pClientList[iClientH]->m_iInt)/2);

}

int CGame::iGetMaxSP(int iClientH)
{

	if (m_pClientList[iClientH] == 0) return 0;

	return (2*(m_pClientList[iClientH]->m_iStr)) + (2*(m_pClientList[iClientH]->m_iLevel));
	
}

void CGame::OnSubLogSocketEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
 UINT iTmp;
 int iLogSockH, iRet;

	iTmp = (WM_ONLOGSOCKETEVENT + 1);
	iLogSockH = (int)(message - iTmp);
	
	if (m_pSubLogSock[iLogSockH] == 0) return;

	iRet = m_pSubLogSock[iLogSockH]->iOnSocketEvent(wParam, lParam);

	switch (iRet) {
	case DEF_XSOCKEVENT_UNSENTDATASENDCOMPLETE:
		wsprintf(G_cTxt, "(!!!) Log Socket Connection Established Log#(%d) Address:%s  Port:%d", iLogSockH, m_cLogServerAddr, m_iLogServerPort);
		PutLogList(G_cTxt);

		m_bIsSubLogSockAvailable[iLogSockH] = true;
		m_iSubLogSockActiveCount++;
		break;

	case DEF_XSOCKEVENT_CONNECTIONESTABLISH:
		// Â·ÃŽÂ±Ã—Â¼Â­Â¹Ã¶Â·ÃŽÃ€Ã‡ Â¿Â¬Â°Ã¡Ã€ÃŒ Ã€ÃŒÂ·Ã§Â¾Ã® ÃÂ³Â´Ã™.
		wsprintf(G_cTxt, "(!!!) Sub-log-socket(%d) connected.", iLogSockH);
		PutLogList(G_cTxt);
		break;
	
	case DEF_XSOCKEVENT_READCOMPLETE:
		// Â¸ÃžÂ½ÃƒÃÃ¶Â°Â¡ Â¼Ã¶Â½Ã…ÂµÃ‡Â¾ÃºÂ´Ã™. 
		OnSubLogRead(iLogSockH);
		break;
	
	case DEF_XSOCKEVENT_BLOCK:
		wsprintf(G_cTxt, "(!!!) Sub-log-socket(%d) BLOCKED!", iLogSockH);
		PutLogList(G_cTxt);
		break;
	
	case DEF_XSOCKEVENT_CONFIRMCODENOTMATCH:
	case DEF_XSOCKEVENT_MSGSIZETOOLARGE:
	case DEF_XSOCKEVENT_SOCKETERROR:
	case DEF_XSOCKEVENT_SOCKETCLOSED:
		// Â¼Ã’Ã„ÃÃ€ÃŒ Â´ÃÃ‡Ã»Â´Ã™.
		delete m_pSubLogSock[iLogSockH];
		m_pSubLogSock[iLogSockH] = 0;
		m_bIsSubLogSockAvailable[iLogSockH] = false;
		
		m_iSubLogSockFailCount++;
		m_iSubLogSockActiveCount--;

		wsprintf(G_cTxt, "(!!!) Sub-log-socket(%d) connection lost!", iLogSockH);
		PutLogList(G_cTxt);
		PutLogFileList(G_cTxt);
		
		m_pSubLogSock[iLogSockH] = new class XSocket(m_hWnd, DEF_SERVERSOCKETBLOCKLIMIT);
		m_pSubLogSock[iLogSockH]->bConnect(m_cLogServerAddr, m_iLogServerPort, (WM_ONLOGSOCKETEVENT + iLogSockH + 1));
		m_pSubLogSock[iLogSockH]->bInitBufferSize(DEF_MSGBUFFERSIZE);

		wsprintf(G_cTxt, "(!!!) Try to reconnect sub-log-socket(%d)... Addr:%s  Port:%d", iLogSockH, m_cLogServerAddr, m_iLogServerPort);
		PutLogList(G_cTxt);
		
		break;
	}
}

void CGame::OnSubLogRead(int iIndex)
{
 UINT32 dwMsgSize;
 char * pData, cKey;

	pData = m_pSubLogSock[iIndex]->pGetRcvDataPointer(&dwMsgSize, &cKey);

	if (bPutMsgQuene(DEF_MSGFROM_LOGSERVER, pData, dwMsgSize, 0, cKey) == false) {
		// Â¸ÃžÂ½ÃƒÃÃ¶ Ã…Â¥Â¿Â¡ Ã€ÃŒÂ»Ã³Ã€ÃŒ Â»Ã½Â°Ã¥Â´Ã™. Ã„Â¡Â¸Ã­Ã€Ã»Ã€ÃŽ Â¿Â¡Â·Â¯.
		PutLogList("@@@@@@ CRITICAL ERROR in MsgQuene!!! @@@@@@");
	}	
}

bool CGame::_bCheckSubLogSocketIndex()
{
 int  iCnt;
 bool bLoopFlag;

	// Â¸ÃžÂ½ÃƒÃÃ¶Â¸Â¦ ÂºÂ¸Â³Â¾ Sub-log-socket Ã€ÃŽÂµÂ¦Â½Âº Ã€Ã¼ÃÃ¸ 
	m_iCurSubLogSockIndex++;
	if (m_iCurSubLogSockIndex >= DEF_MAXSUBLOGSOCK) m_iCurSubLogSockIndex = 0;

	iCnt = 0;
	bLoopFlag = false;
	while (bLoopFlag == false) {
		if ((m_pSubLogSock[m_iCurSubLogSockIndex] != 0) && (m_bIsSubLogSockAvailable[m_iCurSubLogSockIndex] == true)) 
			 bLoopFlag = true;
		else m_iCurSubLogSockIndex++;

		iCnt++;
		if (iCnt >= DEF_MAXSUBLOGSOCK) {
			// ÂºÂ¸Â³Â¾ Â¼Ã¶ Ã€Ã–Â´Ã‚ Sub-log-socketÃ€ÃŒ Ã‡ÃÂ³ÂªÂµÂµ Â¾Ã¸Â´Ã™. Â°Â­ÃÂ¦ Â¼Ã‹Â´Ã™Â¿Ã® Â¸Ã°ÂµÃ¥Â·ÃŽ Ã€Ã¼ÃˆÂ¯ 
			if (m_bOnExitProcess == false) {
				m_cShutDownCode      = 3;
				m_bOnExitProcess     = true;
				m_dwExitProcessTime  = timeGetTime();
				// Â°Ã”Ã€Ã“ Â¼Â­Â¹Ã¶ Â¼Ã‹Â´Ã™Â¿Ã®Ã€ÃŒ Â½ÃƒÃ€Ã›ÂµÃ‡Â¸Ã© Â´ÃµÃ€ÃŒÂ»Ã³Ã€Ã‡ Ã…Â¬Â¶Ã³Ã€ÃŒÂ¾Ã°Ã†Â® ÃÂ¢Â¼Ã“Ã€ÃŒ Â°Ã…ÂºÃŽÂµÃˆÂ´Ã™. 
				PutLogList("(!) GAME SERVER SHUTDOWN PROCESS BEGIN(by Log-server connection Lost)!!!");
			}
			return false;
		}
	}

	return true;
}

void CGame::_CheckGateSockConnection()
{
	// Ã€ÃŒÂ¹ÃŒ Â¼Â­Â¹Ã¶Â°Â¡ Â¼Ã‹Â´Ã™Â¿Ã®ÂµÃˆ Â»Ã³Ã…Ã‚Â¶Ã³Â¸Ã© Ã„Â«Â¿Ã®Ã†Ãƒ Ã‡Ã’ Ã‡ÃŠÂ¿Ã¤Â°Â¡ Â¾Ã¸Â´Ã™.
	if (m_bIsServerShutdowned == true) return;

	if (m_iGateSockConnRetryTimes != 0) {
		wsprintf(G_cTxt, "(!!!) Gate-socket connection counting...%d", m_iGateSockConnRetryTimes); 
		PutLogList(G_cTxt);
		m_iGateSockConnRetryTimes++;
	}

	// 60ÃƒÃŠÂ°Â£ Gate-socketÃ€ÃŒ Â¿Â¬Â°Ã¡ÂµÃ‡ÃÃ¶ Â¾ÃŠÂ¾Ã’Â´Ã™Â¸Ã© Ã€ÃšÂµÂ¿ Ã€Ã§ÂºÃŽÃ†Ãƒ Â¸Ã°ÂµÃ¥Â·ÃŽ ÂµÃ©Â¾Ã®Â°Â£Â´Ã™.
	if (m_iGateSockConnRetryTimes > 20) {
		// Â°Ã”Ã€ÃŒÃ†Â® Â¼Â­Â¹Ã¶Â¿ÃÃ€Ã‡ Â¿Â¬Â°Ã¡Ã€ÃŒ Â²Ã·Â°Ã¥Ã€Â¸Â¹Ã‡Â·ÃŽ ÂºÂ¹Â±Â¸Ã‡Ã’ Â¼Ã¶ Â¾Ã¸Â´Ã™. Ã€ÃšÂµÂ¿Ã€Â¸Â·ÃŽ Â¼Â­Â¹Ã¶Â¸Â¦ Â¼Ã‹Â´Ã™Â¿Ã®Ã‡Ã‘Â´Ã™.
		if (m_bOnExitProcess == false) {
			m_iFinalShutdownCount = 1;
			m_bOnExitProcess     = true;
			m_dwExitProcessTime  = timeGetTime();
			// Â°Ã”Ã€Ã“ Â¼Â­Â¹Ã¶ Â¼Ã‹Â´Ã™Â¿Ã®Ã€ÃŒ Â½ÃƒÃ€Ã›ÂµÃ‡Â¸Ã© Â´ÃµÃ€ÃŒÂ»Ã³Ã€Ã‡ Ã…Â¬Â¶Ã³Ã€ÃŒÂ¾Ã°Ã†Â® ÃÂ¢Â¼Ã“Ã€ÃŒ Â°Ã…ÂºÃŽÂµÃˆÂ´Ã™. 
			PutLogList("(!) GAME SERVER SHUTDOWN PROCESS BEGIN(by gate-server connection Lost)!!!");
		}
	}
}

void CGame::AdminOrder_SetAttackMode(int iClientH, char *pData, UINT32 dwMsgSize)
{
 char   seps[] = "= \t\n";
 char   * token, cBuff[256]; 
 class  CStrTok * pStrTok;
 int    i;

	if (m_pClientList[iClientH] == 0) return;
	if ((dwMsgSize)	<= 0) return;
	

	if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelSetAttackMode) {
		// Admin user levelÃ€ÃŒ Â³Â·Â¾Ã†Â¼Â­ Ã€ÃŒ Â±Ã¢Â´Ã‰Ã€Â» Â»Ã§Â¿Ã«Ã‡Ã’ Â¼Ã¶ Â¾Ã¸Â´Ã™.
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
		return;
	}

	ZeroMemory(cBuff, sizeof(cBuff));
	memcpy(cBuff, pData, dwMsgSize);

	pStrTok = new class CStrTok(cBuff, seps);
	token = pStrTok->pGet();
	token = pStrTok->pGet();
	
	if (token != 0) {
		// Ã€ÃŒ Â°ÂªÃ€ÃŒ Â¹Â®Ã€Ãš '1'Ã€ÃŒÂ¸Ã© Â°Ã¸Â°Ã Â¸Ã°ÂµÃ¥ Â¼Â³ÃÂ¤. 0Ã€ÃŒÂ¸Ã© Â°Ã¸Â°Ã Â¸Ã°ÂµÃ¥ Ã‡Ã˜ÃÂ¦  
		if (token[0] == '1') {
			m_pMapList[ m_pClientList[iClientH]->m_cMapIndex ]->m_bIsAttackEnabled = true;
			// Â°Â¢ Ã…Â¬Â¶Ã³Ã€ÃŒÂ¾Ã°Ã†Â®ÂµÃ©Â¿Â¡Â°Ã” Â°Ã¸Â°ÃÂ¸Ã°ÂµÃ¥ Ã€Â¯ÃˆÂ¿Ã€Ã“Ã€Â» Â¾Ã‹Â¸Â°Â´Ã™.
			for (i = 1; i < DEF_MAXCLIENTS; i++) 
			if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_bIsInitComplete == true) && (m_pClientList[i]->m_cMapIndex == m_pClientList[iClientH]->m_cMapIndex)) 
				SendNotifyMsg(0, i, DEF_NOTIFY_GLOBALATTACKMODE, (int)m_pMapList[ m_pClientList[iClientH]->m_cMapIndex ]->m_bIsAttackEnabled, 0, 0, 0);	
		}
		else if (token[0] == '0') {
			m_pMapList[ m_pClientList[iClientH]->m_cMapIndex ]->m_bIsAttackEnabled = false;
			
			// Â°Â¢ Ã…Â¬Â¶Ã³Ã€ÃŒÂ¾Ã°Ã†Â®ÂµÃ©Â¿Â¡Â°Ã” Â°Ã¸Â°ÃÂ¸Ã°ÂµÃ¥ Â¹Â«ÃˆÂ¿Ã€Ã“Ã€Â» Â¾Ã‹Â¸Â°Â´Ã™.
			for (i = 1; i < DEF_MAXCLIENTS; i++) 
			if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_bIsInitComplete == true) && (m_pClientList[i]->m_cMapIndex == m_pClientList[iClientH]->m_cMapIndex)) 
				SendNotifyMsg(0, i, DEF_NOTIFY_GLOBALATTACKMODE, (int)m_pMapList[ m_pClientList[iClientH]->m_cMapIndex ]->m_bIsAttackEnabled, 0, 0, 0);	
		}
	}

	delete pStrTok;
}


void CGame::AdminOrder_UnsummonAll(int iClientH)
{
 int i;

	if (m_pClientList[iClientH] == 0) return;

	if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelUnsummonAll) {
		// Admin user levelÃ€ÃŒ Â³Â·Â¾Ã†Â¼Â­ Ã€ÃŒ Â±Ã¢Â´Ã‰Ã€Â» Â»Ã§Â¿Ã«Ã‡Ã’ Â¼Ã¶ Â¾Ã¸Â´Ã™.
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
		return;
	}

	// Â¼Ã’ÃˆÂ¯ÂµÃˆ Â¸Ã°ÂµÃ§ Â¸Ã³Â½ÂºÃ…ÃÂ¸Â¦ ÃÃ—Ã€ÃŽÂ´Ã™. 
	for (i = 1; i < DEF_MAXNPCS; i++)
	if (m_pNpcList[i] != 0) {
		if ((m_pNpcList[i]->m_bIsSummoned == true) && (m_pNpcList[i]->m_bIsKilled == false)) 
			NpcKilledHandler(iClientH, DEF_OWNERTYPE_PLAYER, i, 0);
	}
}

void CGame::AdminOrder_Summon(int iClientH, char *pData, UINT32 dwMsgSize)
{
 char   seps[] = "= \t\n";
 char   * token, cBuff[256], cName_Master[10], cName_Slave[10], cNpcName[256], cWaypoint[11], cSA; 
 int    pX, pY, j, iNum, iNamingValue;
 class  CStrTok * pStrTok;
 bool   bMaster = false;

	if (m_pClientList[iClientH] == 0) return;
	if ((dwMsgSize)	<= 0) return;

	if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelSummon) {
		// Admin user levelÃ€ÃŒ Â³Â·Â¾Ã†Â¼Â­ Ã€ÃŒ Â±Ã¢Â´Ã‰Ã€Â» Â»Ã§Â¿Ã«Ã‡Ã’ Â¼Ã¶ Â¾Ã¸Â´Ã™.
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
		return;
	}

	ZeroMemory(cBuff, sizeof(cBuff));
	memcpy(cBuff, pData, dwMsgSize);

	pStrTok = new class CStrTok(cBuff, seps);
	token = pStrTok->pGet();
	token = pStrTok->pGet();

	if (token == 0) {
		delete pStrTok;
		return;
	}

	ZeroMemory(cNpcName, sizeof(cNpcName));
	if (strlen(token) > 20) return;
	else {
		strcpy(cNpcName, token);
	}
	token = pStrTok->pGet();
	
	if (token != 0) 
		 iNum = atoi(token);
	else iNum = 1;

	if (iNum <= 0)  iNum = 1;
	if (iNum >= 50)  iNum = 50;
		
	cSA = 0;
	pX = m_pClientList[iClientH]->m_sX;
	pY = m_pClientList[iClientH]->m_sY;

	wsprintf(G_cTxt, "(!) Admin Order: Summon(%s)-(%d)", cNpcName, iNum);
	PutLogList(G_cTxt);

	iNamingValue = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->iGetEmptyNamingValue();

	if (iNamingValue != -1)
	{
		ZeroMemory(cName_Master, sizeof(cName_Master));
		wsprintf(cName_Master, "XX%d", iNamingValue);
		cName_Master[0] = '_';
		cName_Master[1] = m_pClientList[iClientH]->m_cMapIndex + 65;
			
		if (!bCreateNewNpc(cNpcName, cName_Master, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, (rand() % 3), cSA, DEF_MOVETYPE_RANDOM, &pX, &pY, cWaypoint, 0, 0, -1, false, false, false, true))
		{
			m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetNamingValueEmpty(iNamingValue);
		}
	}


	for (j = 0; j < (iNum - 1); j++) 
	{
		iNamingValue = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->iGetEmptyNamingValue();
		if (iNamingValue != -1)
		{
			ZeroMemory(cName_Slave, sizeof(cName_Slave));
			wsprintf(cName_Slave, "XX%d", iNamingValue);
			cName_Slave[0] = '_';
			cName_Slave[1] = m_pClientList[iClientH]->m_cMapIndex + 65;
			
			if (bCreateNewNpc(cNpcName, cName_Slave, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, (rand() % 3), cSA, DEF_MOVETYPE_RANDOM, &pX, &pY, cWaypoint, 0, 0, -1, false, false, false) == false) {
				m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetNamingValueEmpty(iNamingValue);
			}
			else {
				bSetNpcFollowMode(cName_Slave, cName_Master, DEF_OWNERTYPE_NPC);
			}
		}
	}

	delete pStrTok;
}

void CGame::AdminOrder_SummonAll(int iClientH, char *pData, UINT32 dwMsgSize)
{

 char   seps[] = "= \t\n";
 char   * token, * cp, cBuff[256], cLocation[11], cMapName[11];
 UINT16   *wp;
 int    pX, pY, i;
 class  CStrTok * pStrTok;

	if (m_pClientList[iClientH] == 0) return;
	if ((dwMsgSize) <= 0) return;
	if (m_pClientList[iClientH]->m_bIsAdminCommandEnabled == false) return;

	if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelSummonAll) {
		 // Admin user levelÃ€ÃŒ Â³Â·Â¾Ã†Â¼Â­ Ã€ÃŒ Â±Ã¢Â´Ã‰Ã€Â» Â»Ã§Â¿Ã«Ã‡Ã’ Â¼Ã¶ Â¾Ã¸Â´Ã™.
		 SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
		 return;
	}

	ZeroMemory(cBuff, sizeof(cBuff));
	memcpy(cBuff, pData, dwMsgSize);

	pStrTok = new class CStrTok(cBuff, seps);
	token = pStrTok->pGet();
	token = pStrTok->pGet();

	if (token == 0) {
		 delete pStrTok;
		 return;
	}

	pX = m_pClientList[iClientH]->m_sX;
	pY = m_pClientList[iClientH]->m_sY;


	memcpy(cMapName, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, 11);

	if (strlen(token) > 10)
	memcpy(cLocation, token, 10);
	else memcpy(cLocation, token, strlen(token));

	for (i = 0; i < DEF_MAXCLIENTS; i++)
		 if ((m_pClientList[i] != 0) && (strcmp(m_pClientList[i]->m_cLocation, token) == 0)) {
			  RequestTeleportHandler(i, "2   ", cMapName, pX, pY);
		 }

	wsprintf(G_cTxt,"GM Order(%s): PC(%s) Summoned to (%s)", m_pClientList[iClientH]->m_cLocation,
	   cLocation, cMapName);
	bSendMsgToLS(MSGID_GAMEMASTERLOG, iClientH, false, G_cTxt);

	ZeroMemory(cBuff, sizeof(cBuff));
	cp = (char *)cBuff;
	*cp = GSM_REQUEST_SUMMONALL;
	cp++;

	memcpy(cp, cLocation, 10);
	cp += 10;

	memcpy(cp, cMapName, 10);
	cp += 10;

	wp = (UINT16 *)cp;
	*wp = m_pClientList[iClientH]->m_sX;
	cp += 2;

	wp = (UINT16 *)cp;
	*wp = m_pClientList[iClientH]->m_sY;
	cp += 2;

	bStockMsgToGateServer(cBuff, 25);

	delete pStrTok;
}

void CGame::AdminOrder_SummonPlayer(int iClientH, char *pData, UINT32 dwMsgSize)
{
	char   seps[] = "= \t\n";
	char   * token, * cp, cBuff[256], cMapName[11], cName[11]; 
	UINT16   *wp;
	int    pX, pY, i;
	class  CStrTok * pStrTok;

	if (m_pClientList[iClientH] == 0) return;
	if ((dwMsgSize)	<= 0) return;

	if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelSummonPlayer) {
		// Admin user levelÃ€ÃŒ Â³Â·Â¾Ã†Â¼Â­ Ã€ÃŒ Â±Ã¢Â´Ã‰Ã€Â» Â»Ã§Â¿Ã«Ã‡Ã’ Â¼Ã¶ Â¾Ã¸Â´Ã™.
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
		return;
	}

	ZeroMemory(cBuff, sizeof(cBuff));
	ZeroMemory(cName, sizeof(cName));
	memcpy(cBuff, pData, dwMsgSize);

	pStrTok = new class CStrTok(cBuff, seps);
	token = pStrTok->pGet();
	token = pStrTok->pGet();

	if (token == 0) {
		delete pStrTok;
		return;
	}

	pX = m_pClientList[iClientH]->m_sX;
	pY = m_pClientList[iClientH]->m_sY;
	memcpy(cMapName, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, 11);

	if (strlen(token) > 10)
		memcpy(cName, token, 10);
	else memcpy(cName, token, strlen(token));

	for (i = 0; i < DEF_MAXCLIENTS; i++)
		if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cCharName, cName, 10) == 0)) {
			if (i == iClientH) {
				delete pStrTok;
				return;
			}
			RequestTeleportHandler(i, "2   ", cMapName, pX, pY);
			delete pStrTok;
			return;
		}

	// find char on other hg's
	wsprintf(G_cTxt,"GM Order(%s): PC(%s) Summoned to (%s)", m_pClientList[iClientH]->m_cCharName,
				cName, cMapName);
	bSendMsgToLS(MSGID_GAMEMASTERLOG, iClientH, false, G_cTxt);

	ZeroMemory(cBuff, sizeof(cBuff));
	cp = (char *)cBuff;
	*cp = GSM_REQUEST_SUMMONPLAYER;
	cp++;

	memcpy(cp, cName, 10);
	cp += 10;

	memcpy(cp, cMapName, 10);
	cp += 10;

	wp = (UINT16 *)cp;
	*wp = m_pClientList[iClientH]->m_sX;
	cp += 2;

	wp = (UINT16 *)cp;
	*wp = m_pClientList[iClientH]->m_sY;
	cp += 2;

	bStockMsgToGateServer(cBuff, 25);

	delete pStrTok;
}

void CGame::AdminOrder_DisconnectAll(int iClientH, char *pData, UINT32 dwMsgSize)
{

	if (m_pClientList[iClientH] == 0) return;
	if ((dwMsgSize)	<= 0) return;
	if (m_pClientList[iClientH]->m_bIsAdminCommandEnabled == false) return;

	if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelDisconnectAll) {
		// Admin user levelÃ€ÃŒ Â³Â·Â¾Ã†Â¼Â­ Ã€ÃŒ Â±Ã¢Â´Ã‰Ã€Â» Â»Ã§Â¿Ã«Ã‡Ã’ Â¼Ã¶ Â¾Ã¸Â´Ã™.
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
		return;
	}

	_iForcePlayerDisconect(DEF_MAXCLIENTS);
}

void CGame::RequestNoticementHandler(int iClientH, char * pData)
{
 char * cp, cData[120];
 int  * ip, iRet, iClientSize;
 UINT32 * dwp;
 UINT16  * wp;
 
	if (m_pClientList[iClientH] == 0) return;
	if (m_dwNoticementDataSize < 10) return;

	ip = (int *)(pData + DEF_INDEX2_MSGTYPE + 2);
	iClientSize = *ip;

	if (iClientSize != m_dwNoticementDataSize) {
		// Ã…Â¬Â¶Ã³Ã€ÃŒÂ¾Ã°Ã†Â®Â°Â¡ Â°Â®Â°Ã­ Ã€Ã–Â´Ã‚ Ã†Ã„Ã€Ã Â»Ã§Ã€ÃŒÃÃ®Â¿Ã Â´Ã™Â¸Â£Â¸Ã© Â³Â»Â¿Ã«Ã€Â» Â¸Ã°ÂµÃŽ ÂºÂ¸Â³Â½Â´Ã™.
		cp = new char[m_dwNoticementDataSize + 2 + DEF_INDEX2_MSGTYPE + 2];
		ZeroMemory(cp, m_dwNoticementDataSize + 2 + DEF_INDEX2_MSGTYPE + 2);
		memcpy((cp + DEF_INDEX2_MSGTYPE + 2), m_pNoticementData, m_dwNoticementDataSize);
		
		dwp  = (UINT32 *)(cp + DEF_INDEX4_MSGID);
		*dwp = MSGID_RESPONSE_NOTICEMENT;
		wp   = (UINT16 *)(cp + DEF_INDEX2_MSGTYPE);
		*wp  = DEF_MSGTYPE_REJECT;
		
		iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cp, m_dwNoticementDataSize + 2 + DEF_INDEX2_MSGTYPE + 2);
	
		delete[] cp;
	}
	else {
		ZeroMemory(cData, sizeof(cData));
		
		dwp  = (UINT32 *)(cData + DEF_INDEX4_MSGID);
		*dwp = MSGID_RESPONSE_NOTICEMENT;
		wp   = (UINT16 *)(cData + DEF_INDEX2_MSGTYPE);
		*wp  = DEF_MSGTYPE_CONFIRM;
		
		iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 6);
	}
	// Â¿Â¡Â·Â¯ Â¹ÃŸÂ»Ã½Ã‡Ã˜ÂµÂµ Â²Ã·ÃÃ¶ Â¾ÃŠÂ´Ã‚Â´Ã™.	
}

void CGame::_bDecodeNoticementFileContents(char *pData, UINT32 dwMsgSize)
{
 char * cp;	
	
	cp = (pData);
	
	if (m_pNoticementData != 0) {
		delete[] m_pNoticementData;
		m_pNoticementData = 0;
	}

	m_pNoticementData = new char[strlen(cp) + 2];
	ZeroMemory(m_pNoticementData, strlen(cp) + 2);

	memcpy(m_pNoticementData, cp, strlen(cp));
	m_dwNoticementDataSize = strlen(cp);

	wsprintf(G_cTxt, "(!) Noticement Data Size: %d", m_dwNoticementDataSize);
	PutLogList(G_cTxt);
}

void CGame::RequestCheckAccountPasswordHandler(char *pData, UINT32 dwMsgSize)
{
 int * ip, i, iLevel;
 char * cp, cAccountName[11], cAccountPassword[11];

	cp = (char *)(pData + 6);

	ZeroMemory(cAccountName, sizeof(cAccountName));
	ZeroMemory(cAccountPassword, sizeof(cAccountPassword));

	memcpy(cAccountName, cp, 10);
	cp += 10;

	memcpy(cAccountPassword, cp, 10);
	cp += 10;

	ip = (int *)cp;
	iLevel = *ip;
	cp += 4;

	for (i = 0; i < DEF_MAXCLIENTS; i++)
	if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cAccountName, cAccountName, 10) == 0)) {
		// Â°Â°Ã€Âº Â°Ã¨ÃÂ¤Ã€Â» ÃƒÂ£Â¾Ã’Â´Ã™. Â¸Â¸Â¾Ã  Ã†ÃÂ½ÂºÂ¿Ã¶ÂµÃ¥Â³Âª Â·Â¹ÂºÂ§Ã€ÃŒ Â´Ã™Â¸Â£Â¸Ã© ÂµÂ¥Ã€ÃŒÃ…Ã Ã€ÃºÃ€Ã¥Ã€Â» Ã‡ÃÃÃ¶ Â¾ÃŠÂ°Ã­ ÃÂ¢Â¼Ã“Ã€Â» Â²Ã·Â´Ã‚Â´Ã™. 
		if ((memcmp(m_pClientList[i]->m_cAccountPassword, cAccountPassword, 10) != 0) || (m_pClientList[i]->m_iLevel != iLevel)) {
			wsprintf(G_cTxt, "(TestLog) Error! Account(%s)-Level(%d) password(or level) mismatch! Disconnect.", cAccountName, iLevel);
			PutLogList(G_cTxt);
			// ÂµÂ¥Ã€ÃŒÃ…Ã Ã€ÃºÃ€Ã¥Ã€Â» Ã‡ÃÃÃ¶ Â¾ÃŠÂ°Ã­ Â²Ã·Â´Ã‚Â´Ã™.
			DeleteClient(i, false, true);
			return;
		}
	}
}

void CGame::AdminOrder_EnableAdminCommand(int iClientH, char *pData, UINT32 dwMsgSize)
{
 char   * token, cBuff[256];
 char   seps[] = "= \t\n";
 class  CStrTok * pStrTok;

	if (m_pClientList[iClientH] == 0) return;
	if ((dwMsgSize)	<= 0) return;

	if (m_pClientList[iClientH]->m_iAdminUserLevel < 1) return;

	ZeroMemory(cBuff, sizeof(cBuff));
	memcpy(cBuff, pData, dwMsgSize);
	pStrTok = new class CStrTok(cBuff, seps);
	token = pStrTok->pGet();
	token = pStrTok->pGet();

	if (token != 0) {
		if (strcmp(token, m_cSecurityNumber) == 0) {
			m_pClientList[iClientH]->m_bIsAdminCommandEnabled = true;
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_IPACCOUNTINFO, 0, 0, 0, "Admin commands ENABLED!");
		}
	}
}

void CGame::AdminOrder_CreateItem(int iClientH, char *pData, UINT32 dwMsgSize)
{
 char   seps[] = "= \t\n";
 char   * token, cBuff[256], cItemName[256], cData[256], cTemp[256], cAttribute[256], cValue[256];
 SYSTEMTIME SysTime;
 class  CStrTok * pStrTok;
 class  CItem * pItem;
 int    iTemp, iEraseReq, iValue;

 UINT16  wTemp;
 float dV1, dV2, dV3;
 
	// if the client doesnt exist than no effect.
	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsAdminCommandEnabled == false) return;

	// if the command is just "/createitem " than no effect.
	if ((dwMsgSize)	<= 0) return;

	// if client's admin level is less than 4 no effect.
	if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelCreateItem) {
		// if the admin user level is not 0 and less than 4 send message 
		if (m_pClientList[iClientH]->m_iAdminUserLevel !=  0)	
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
		return;
	} // close if 

	ZeroMemory(cBuff, sizeof(cBuff));
	memcpy(cBuff, pData, dwMsgSize);
	pStrTok = new class CStrTok(cBuff, seps);
	token = pStrTok->pGet();
	token = pStrTok->pGet();

	// if the token is something it is an item name
	if (token != 0) {
		ZeroMemory(cItemName, sizeof(cItemName));
		strcpy(cItemName, token);
	} // close if 

	// if the 2nd token is true it is the item attribute
	ZeroMemory(cAttribute, sizeof(cAttribute));
	token = pStrTok->pGet();
	if (token != 0) {
		ZeroMemory(cAttribute, sizeof(cAttribute));
		strcpy(cAttribute, token);
	} // close if 

	// if the 3rd token is true it is the item value
	ZeroMemory(cValue, sizeof(cValue));
	iValue = 0;
	token = pStrTok->pGet();
	if (token != 0) {
		ZeroMemory(cValue, sizeof(cValue));
		strcpy(cValue, token);
		iValue = atoi(cValue);
	} // close if 

	pItem = new class CItem;
	// if the given itemname doesnt exist delete item
	if (_bInitItemAttr(pItem, cItemName) == false) {
		delete pItem;
		return;	
	} //close if 

	if (strlen(cAttribute) != 0) {
		pItem->m_dwAttribute = atoi(cAttribute);
		if (pItem->m_dwAttribute == 1) {
			if ((iValue >= 1) && (iValue <= 200)) {
				pItem->m_cItemColor = 2;
				pItem->m_sItemSpecEffectValue2 = iValue - 100;
								
				dV2 = (float)pItem->m_sItemSpecEffectValue2;
				dV3 = (float)pItem->m_wMaxLifeSpan;
				dV1 = (dV2/100.0f)*dV3;

				iTemp  = (int)pItem->m_wMaxLifeSpan;
				iTemp += (int)dV1;

				if (iTemp <= 0) 
					 wTemp = 1;
				else wTemp = (UINT16)iTemp;

				if (wTemp <= pItem->m_wMaxLifeSpan*2) {
					pItem->m_wMaxLifeSpan = wTemp;
					pItem->m_sItemSpecEffectValue1 = (short)wTemp;
					pItem->m_wCurLifeSpan = pItem->m_wMaxLifeSpan;
				} // close if 
				else pItem->m_sItemSpecEffectValue1 = (short)pItem->m_wMaxLifeSpan;
			} // close if
			else pItem->m_dwAttribute = 0;
		} // close if
		else {
			if (pItem->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ATTACK || pItem->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ATTACK_ARROW) {
				switch ((pItem->m_dwAttribute & 0xF00000) >> 20) {
				case 6:	pItem->m_cItemColor = 2; break;
				case 8: pItem->m_cItemColor = 3; break;
				case 1:	pItem->m_cItemColor = 5; break;
				case 5:	pItem->m_cItemColor = 1; break;
				case 3:	pItem->m_cItemColor = 7; break;
				case 2:	pItem->m_cItemColor = 4; break;
				case 7:	pItem->m_cItemColor = 6; break;
				case 9:	pItem->m_cItemColor = 8; break;
				} // close switch
			} // close if 
			else if (pItem->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ATTACK_MANASAVE) {
				switch ((pItem->m_dwAttribute & 0xF00000) >> 20) {
				case 10: pItem->m_cItemColor = 5; break;
				} // close switch 
			} // close else if
		} // close else 
	} // close if 

	// if the item id is
	switch (pItem->m_sIDnum) {
	case 511: // ArenaTicket
	case 513: // ArenaTicket(2)
	case 515: // ArenaTicket(3)
	case 517: // ArenaTicket(4)
	case 530: // ArenaTicket(5)
	case 531: // ArenaTicket(6)
	case 532: // ArenaTicket(7)
	case 533: // ArenaTicket(8)
	case 534: // ArenaTicket(9)
		GetLocalTime(&SysTime);
		pItem->m_sTouchEffectType   = DEF_ITET_DATE;
		// v1.4311-3 º¯°æ ¿î¿µÀÚ°¡ ¹ß±ÞÇÑ ÀÔÀå±ÇÀº ±×³¯Àº Ç×»ó ÀÔÀå °¡´É ..
		pItem->m_sTouchEffectValue1 = (short)SysTime.wMonth;
		pItem->m_sTouchEffectValue2 = (short)SysTime.wDay;
		pItem->m_sTouchEffectValue3 = 24 ;
		break;

	default:
		GetLocalTime(&SysTime);
		pItem->m_sTouchEffectType   = DEF_ITET_ID;
		pItem->m_sTouchEffectValue1 = iDice(1,100000);
		pItem->m_sTouchEffectValue2 = iDice(1,100000);
		ZeroMemory(cTemp, sizeof(cTemp));
		wsprintf(cTemp, "%d%2d",  (short)SysTime.wMonth, (short)SysTime.wDay);
		pItem->m_sTouchEffectValue3 = atoi(cTemp);
		break;
	}
	
	ZeroMemory(cData, sizeof(cData));
	if (_bAddClientItemList(iClientH, pItem, &iEraseReq) == true) {
		SendItemNotifyMsg(iClientH, DEF_NOTIFY_ITEMOBTAINED, pItem, 0);		

		// v2.14 Admin Log
		wsprintf(G_cTxt, "(%s) GM Order(%s): Create ItemName(%s)",m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName, cItemName);
		bSendMsgToLS(MSGID_GAMEMASTERLOG, iClientH, 0,G_cTxt);
	}
	else {	
		delete pItem;
		
	}
}

//Magn0S: 19/04/2013 Coding for fun... ^^ (Create Fragile items)
void CGame::AdminOrder_CreateFragileItem(int iClientH, char* pData, UINT32 dwMsgSize)
{
	char   seps[] = "= \t\n";
	char* token, cBuff[256], cItemName[256];
	class  CStrTok* pStrTok;
	class  CItem* pItem;
	int  iEraseReq;
	short dFragile, dFDay, dFMonth, dFYear;
	dFragile, dFDay, dFMonth, dFYear = 0;

	if (m_pClientList[iClientH] == 0) return;
	if ((dwMsgSize) <= 0) return;

	if (m_pClientList[iClientH]->m_iAdminUserLevel <= 3) {
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
		return;
	}

	ZeroMemory(cBuff, sizeof(cBuff));
	memcpy(cBuff, pData, dwMsgSize);

	pStrTok = new class CStrTok(cBuff, seps);
	token = pStrTok->pGet();

	token = pStrTok->pGet();
	if (token != 0) {
		ZeroMemory(cItemName, sizeof(cItemName));
		strcpy(cItemName, token);
	}

	//-------------
	token = pStrTok->pGet();
	if (token != 0) {
		dFragile = atoi(token); //Set fragile type
	}

	token = pStrTok->pGet();
	if (token != 0) {
		dFDay = atoi(token); // Set fragile day
	}
	token = pStrTok->pGet();
	if (token != 0) {
		dFMonth = atoi(token); // Set fragile month
	}
	token = pStrTok->pGet();
	if (token != 0) {
		dFYear = atoi(token); // Set fragile year
	}
	//-----------------------

	pItem = new class CItem;
	if (_bInitItemAttr(pItem, cItemName) == false) {
		delete pItem;
		return;
	}

	if ((dFragile < 1) && (dFragile > 2)) {
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_IPACCOUNTINFO, 0, 0, 0, "Wrong Fragile item type data.");
		return;
	}

	pItem->m_sNewEffect1 = DEF_FRAGILEITEM;
	if (dFragile == 2) {
		SYSTEMTIME SysTime;
		GetLocalTime(&SysTime);

		if ((dFDay < 1) || (dFDay > 31) || (dFMonth < 1) || (dFMonth > 12) || (dFYear < SysTime.wYear) || (dFYear > SysTime.wYear + 1)) {
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_IPACCOUNTINFO, 0, 0, 0, "Wrong Fragile item dates data.");
			return;
		}
		pItem->m_sNewEffect2 = dFDay;
		pItem->m_sNewEffect3 = dFMonth;
		pItem->m_sNewEffect4 = dFYear;
	}


	if (_bAddClientItemList(iClientH, pItem, &iEraseReq) == true) {
		SendItemNotifyMsg(iClientH, DEF_NOTIFY_ITEMOBTAINED, pItem, 0);

		wsprintf(G_cTxt, "GM Order(%s): Created Fragile Item - Name:(%s)", m_pClientList[iClientH]->m_cCharName, cItemName);
		bSendMsgToLS(MSGID_GAMEMASTERLOG, iClientH, 0, G_cTxt);

		if (iEraseReq == 1)
			delete pItem;
	}
	else {
		delete pItem;
	}
}

void CGame::ActivateSpecialAbilityHandler(int iClientH)
{
 UINT32 dwTime = timeGetTime();
 short sTemp;

	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_iSpecialAbilityTime != 0) return;
	if (m_pClientList[iClientH]->m_iSpecialAbilityType == 0) return;
	if (m_pClientList[iClientH]->m_bIsSpecialAbilityEnabled == true) return;
	if (m_bIsCTFEvent && (m_pClientList[iClientH]->m_iStatus & 0x80000) != 0) return;

	m_pClientList[iClientH]->m_bIsSpecialAbilityEnabled  = true;
	m_pClientList[iClientH]->m_dwSpecialAbilityStartTime = dwTime;

	m_pClientList[iClientH]->m_iSpecialAbilityTime = DEF_SPECABLTYTIMESEC;

	sTemp = m_pClientList[iClientH]->m_sAppr4;
	sTemp = sTemp & 0xFF0F;
	switch (m_pClientList[iClientH]->m_iSpecialAbilityType) {
	case 1:
	case 2:
	case 3:
	case 5:
		sTemp = sTemp | 0x0010;
		break;
	case 50:
	case 51:
	case 52:
	case 53:
	case 54:
	case 60: // Magn0S:: Bloody Effect
		sTemp = sTemp | 0x0020;
		break;
	}
	m_pClientList[iClientH]->m_sAppr4 = sTemp;

	SendNotifyMsg(0, iClientH, DEF_NOTIFY_SPECIALABILITYSTATUS, 1, m_pClientList[iClientH]->m_iSpecialAbilityType, m_pClientList[iClientH]->m_iSpecialAbilityLastSec, 0);
	SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
}

void CGame::CheckConnectionHandler(int iClientH, char *pData)
{
 char * cp;
 UINT32 * dwp, dwTimeRcv, dwTime, dwTimeGapClient, dwTimeGapServer;
	
	dwTime = timeGetTime();
	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
	dwp = (UINT32 *)cp;
	dwTimeRcv = *dwp;

	if (m_bAntiHackMode) CheckDenialServiceAttack(iClientH, dwTimeRcv);

	if (m_pClientList[iClientH] == 0) return;

	if (m_pClientList[iClientH]->m_dwInitCCTimeRcv == 0) {
		m_pClientList[iClientH]->m_dwInitCCTimeRcv = dwTimeRcv;
		m_pClientList[iClientH]->m_dwInitCCTime = dwTime;
	}
	else {
		dwTimeGapClient = (dwTimeRcv - m_pClientList[iClientH]->m_dwInitCCTimeRcv);
		dwTimeGapServer = (dwTime - m_pClientList[iClientH]->m_dwInitCCTime);
		
		if (dwTimeGapClient < dwTimeGapServer) return;
		if ((abs(int(dwTimeGapClient - dwTimeGapServer))) >= (DEF_CLIENTTIMEOUT)) {
			DeleteClient(iClientH, true, true);
		}
	}
}

bool CGame::bStockMsgToGateServer(char *pData, UINT32 dwSize)
{
 char * cp;

	if ((m_iIndexGSS + dwSize) >= DEF_MAXGATESERVERSTOCKMSGSIZE-10) return false;

	cp = (char *)(m_cGateServerStockMsg + m_iIndexGSS);	
	memcpy(cp, pData, dwSize);

	m_iIndexGSS += dwSize;

	return true;
}


void CGame::SendStockMsgToGateServer()
{
 UINT32 * dwp;
 UINT16 * wp;
 char * cp;

	// Â±Ã—ÂµÂ¿Â¾Ãˆ Â¸Ã°Â¾Ã†Â³ÃµÂ¾Ã’Â´Ã¸ Â¸ÃžÂ½ÃƒÃÃ¶Â¸Â¦ Â°Ã”Ã€ÃŒÃ†Â® Â¼Â­Â¹Ã¶Â·ÃŽ Ã€Ã¼Â¼Ã›.
	if (m_iIndexGSS > 6) {
		SendMsgToGateServer(MSGID_SERVERSTOCKMSG, 0, m_cGateServerStockMsg);
	
		// Â¹Ã¶Ã†Ã› Ã…Â¬Â¸Â®Â¾Ã®
		ZeroMemory(m_cGateServerStockMsg, sizeof(m_cGateServerStockMsg));
		// Â¸Ã‡ Â¾Ã•ÂºÃŽÂºÃÂ¿Â¡ Â¸ÃžÂ½ÃƒÃÃ¶ Â»Ã°Ã€Ã”
		cp = (char *)m_cGateServerStockMsg;
		dwp = (UINT32 *)cp;
		*dwp = MSGID_SERVERSTOCKMSG;
		cp += 4;
		wp = (UINT16 *)cp;
		*wp = DEF_MSGTYPE_CONFIRM;
		cp += 2;

		m_iIndexGSS = 6;
	}
}

void CGame::ServerStockMsgHandler(char *pData)
{
 char * cp, * cp2, cTemp[120], cLocation[11], cGuildName[21], cName[11], cTemp2[120], cTemp3[120], cMapName[11], cBuffer[256]; short * sp;
 UINT16 * wp, wServerID, wClientH, wV1, wV2, wV3, wV4, wV5;
 UINT32 * dwp;
 bool bFlag = false;
 int * ip, i, iTotal, iV1, iV2, iV3, iRet;
 short sX, sY;

	iTotal = 0;
	cp = (char *)(pData + 6);
	while (bFlag == false) {
		iTotal++;
		switch (*cp) {
		
		case GSM_REQUEST_SUMMONGUILD:
			cp++;

			ZeroMemory(cGuildName, sizeof(cGuildName));
			memcpy(cGuildName, cp, 20);
			cp += 20;

			ZeroMemory(cTemp, sizeof(cMapName));
			memcpy(cMapName, cp, 10);
			cp += 10;

			cp2 = (char *) cTemp + 10;
			wp = (UINT16 *)cp;
			wV1 = *wp;
			cp += 2;

			wp = (UINT16 *)cp;
			wV2 = *wp;
			cp += 2;

			for (i = 0; i < DEF_MAXCLIENTS; i++)
			if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cGuildName, cGuildName, 20) == 0))
			{	wsprintf(G_cTxt,"PC(%s) summoned by Guild(%s) to %s.", m_pClientList[i]->m_cCharName, cGuildName, cMapName);
				PutLogList(G_cTxt);
				RequestTeleportHandler(i, "2   ", cMapName, wV1, wV2);
			}

			break;

		// v2.14 Â¼ÂºÃˆÃ„Â´Ã ÃƒÃŸÂ°Â¡ Ã€Â¯Ã€Ãº Â¼Ã’ÃˆÂ¯ 
		case GSM_REQUEST_SUMMONPLAYER:
			cp++;
			ZeroMemory(cName, sizeof(cName));
			memcpy(cName, cp, 10);
			cp += 10;

			ZeroMemory(cTemp, sizeof(cTemp));
			memcpy(cTemp, cp, 10);
			cp += 10;

			cp2 = (char *) cTemp + 10 ;
			
			wp = (UINT16 *)cp;
			wV1 = *wp;
			cp += 2;

			wp = (UINT16 *)cp;
			wV2 = *wp;
			cp += 2;
			
			for (i = 1; i < DEF_MAXCLIENTS; i++)
				if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cCharName, cName, 10) == 0)) {
					RequestTeleportHandler(i, "2   ", cTemp ,wV1, wV2);
					break;
				}
			break;
		
		case GSM_REQUEST_SUMMONALL:
			cp++;
			ZeroMemory(cLocation, sizeof(cLocation));
			memcpy(cLocation, cp, 10);
			cp += 10;

			ZeroMemory(cTemp, sizeof(cTemp));
			memcpy(cTemp, cp, 10);
			cp += 10;

			cp2 = (char *) cTemp + 10;
			
			wp = (UINT16 *)cp;
			wV1 = *wp;
			cp += 2;

			wp = (UINT16 *)cp;
			wV2 = *wp;
			cp += 2;
			
			for (i = 0; i < DEF_MAXCLIENTS; i++)
				if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cLocation, cLocation, 10) == 0)) {
					RequestTeleportHandler(i, "2   ", cTemp, wV1, wV2);
					break;
				}
			break;

		case GSM_CHATMSG:
			cp++;
			ZeroMemory(cTemp, sizeof(cTemp));
			ZeroMemory(cName, sizeof(cName));
			iV1 = *cp;
			cp++;
			ip = (int *)cp;
			iV2 = *ip;
			cp += 4;
			memcpy(cName, cp, 10);
			cp += 10;;
			sp = (short *)cp;
			wV1 = (UINT16)*sp;
			cp += 2;
			ChatMsgHandlerGSM(iV1, iV2, cName, cp, wV1);
			cp += wV1;
			break;
		
		case GSM_CONSTRUCTIONPOINT:
			cp++;
			ip = (int *)cp;
			iV1 = *ip;
			cp += 4;
			ip = (int *)cp;
			iV2 = *ip;
			cp += 4;
			GSM_ConstructionPoint(iV1, iV2);
			break;

		case GSM_SETGUILDTELEPORTLOC:
			cp++;
			ip = (int *)cp;
			iV1 = *ip;
			cp += 4;
			ip = (int *)cp;
			iV2 = *ip;
			cp += 4;
			ip = (int *)cp;
			iV3 = *ip;
			cp += 4;
			ZeroMemory(cTemp, sizeof(cTemp));
			memcpy(cTemp, cp, 10);
			cp += 10;
			GSM_SetGuildTeleportLoc(iV1, iV2, iV3, cTemp);
			break;

		case GSM_SETGUILDCONSTRUCTLOC:
			cp++;
			ip = (int *)cp;
			iV1 = *ip;
			cp += 4;
			ip = (int *)cp;
			iV2 = *ip;
			cp += 4;
			ip = (int *)cp;
			iV3 = *ip;
			cp += 4;
			ZeroMemory(cTemp, sizeof(cTemp));
			memcpy(cTemp, cp, 10);
			cp += 10;
			GSM_SetGuildConstructLoc(iV1, iV2, iV3, cTemp);
			break;

		case GSM_REQUEST_SETFORCERECALLTIME:
			cp++;
			wp = (UINT16 *)cp;
			m_sForceRecallTime = *wp;
			cp += 2;

			wsprintf(G_cTxt,"(!) Game Server Force Recall Time (%d)min",m_sForceRecallTime) ;
			PutLogList(G_cTxt) ;
			break;

		case GSM_MIDDLEMAPSTATUS:
			cp++;
			// Â±Â¸ÃÂ¶ÃƒÂ¼ Ã…Â¬Â¸Â®Â¾Ã®
			for (i = 0; i < DEF_MAXCRUSADESTRUCTURES; i++) {
				m_stMiddleCrusadeStructureInfo[i].cType = 0;
				m_stMiddleCrusadeStructureInfo[i].cSide = 0;
				m_stMiddleCrusadeStructureInfo[i].sX    = 0;
				m_stMiddleCrusadeStructureInfo[i].sY    = 0;
			}
			sp = (short *)cp;
			m_iTotalMiddleCrusadeStructures = *sp;
			cp += 2;
			// ÃÂ¤ÂºÂ¸ Ã€ÃÂ¾Ã®ÂµÃ©Ã€ÃŽÂ´Ã™.
			for (i = 0; i < m_iTotalMiddleCrusadeStructures; i++) {
				m_stMiddleCrusadeStructureInfo[i].cType = *cp;
				cp++;
				m_stMiddleCrusadeStructureInfo[i].cSide = *cp;
				cp++;
				sp = (short *)cp;
				m_stMiddleCrusadeStructureInfo[i].sX = *sp;
				cp += 2;
				sp = (short *)cp;
				m_stMiddleCrusadeStructureInfo[i].sY = *sp;
				cp += 2;	
			}
			break;
		
		case GSM_BEGINCRUSADE:
			cp++;
			dwp = (UINT32 *)cp;
			cp += 4;
			LocalStartCrusadeMode(*dwp);
			break;

		case GSM_BEGINAPOCALYPSE:
			cp++;
			dwp = (UINT32 *)cp;
			cp += 4;
			LocalStartApocalypse(*dwp);
			break;

		case GSM_ENDAPOCALYPSE:
			cp++;
			LocalEndApocalypse();
			break;

		case GSM_STARTHELDENIAN:
			cp++;
			wp  = (UINT16 *)cp;
			wV1 = *wp;
			cp += 2;
			wp  = (UINT16 *)cp;
			wV2 = *wp;
			cp += 2;
			dwp = (UINT32 *)cp;
			cp += 4;
			LocalStartHeldenianMode(wV1, wV2, *dwp);
			break;

		case GSM_ENDHELDENIAN:
			cp++;
			dwp = (UINT32 *)cp;
			cp += 4;
			ip = (int *)cp;
			iV1 = *ip;
			cp += 4;
			LocalEndHeldenianMode(*dwp ,iV1);
			break;
			
		case GSM_UPDATECONFIGS:
			cp++;
			LocalUpdateConfigs(*cp);
			cp += 16;
			break;

		case GSM_ENDCRUSADE:
			cp++;
			LocalEndCrusadeMode(*cp);
			cp += 16;
			break;

		case GSM_COLLECTEDMANA:
			cp++;
			wp  = (UINT16 *)cp;
			wV1 = *wp;
			cp += 2;

			wp  = (UINT16 *)cp;
			wV2 = *wp;
			cp += 2;
			CollectedManaHandler(wV1, wV2);
			break;

		case GSM_GRANDMAGICLAUNCH:
			cp++;
			wp  = (UINT16 *)cp;
			wV1 = *wp;
			cp += 2;

			wp  = (UINT16 *)cp;
			wV2 = *wp;
			cp += 2;

			switch (wV1) {
			case 1: // Â¸ÃžÃ…Ã—Â¿Ã€ Â½ÂºÃ†Â®Â¶Ã³Ã€ÃŒÃ…Â© 
				MeteorStrikeMsgHandler((char)wV2);
				break;
			}
			break;
		
		case  GSM_GRANDMAGICRESULT:
			cp++;
			ZeroMemory(cTemp, sizeof(cTemp));
			memcpy(cTemp, cp, 10);
			cp += 10;

			wp  = (UINT16 *)cp;
			wV1 = *wp;
			cp += 2;

			wp  = (UINT16 *)cp;
			wV2 = *wp;
			cp += 2;

			wp  = (UINT16 *)cp;
			wV3 = *wp;
			cp += 2;

			wp  = (UINT16 *)cp;
			wV4 = *wp;
			cp += 2;

			wp  = (UINT16 *)cp;
			wV5 = *wp;

			GrandMagicResultHandler(cTemp, wV1, wV2, wV3, wV4, wV5, cp);
			break;
		
			// v2.15 2002-5-21
		case GSM_REQUEST_SHUTUPPLAYER:
			cp++;
			wp = (UINT16 *)cp;
			wServerID = *wp;
			cp += 2;
			wp = (UINT16 *)cp;
			wClientH = *wp;
			cp += 2;

			ZeroMemory(cName, sizeof(cName));
			memcpy(cName, cp, 10);
			cp += 10;

			wp  = (UINT16 *)cp;
			wV1 = *wp;
			cp += 2;

			ZeroMemory(cTemp, sizeof(cTemp));
			memcpy(cTemp, cp, 10);
			cp += 10;

			GSM_RequestShutupPlayer(cTemp,wServerID,wClientH, wV1, cName);
			break ;

		case GSM_RESPONSE_SHUTUPPLAYER:
			cp++;
			wp = (UINT16 *)cp;
			wServerID = *wp;
			cp += 2;
			wp = (UINT16 *)cp;
			wClientH = *wp;
			cp += 2;

			ZeroMemory(cName, sizeof(cName));
			memcpy(cName, cp, 10);
			cp += 10;

			wp  = (UINT16 *)cp;
			wV1 = *wp;
			cp += 2;

			ZeroMemory(cTemp, sizeof(cTemp));
			memcpy(cTemp, cp, 10);
			cp += 10;


			if (wServerID == m_wServerID_GSS) {
				if ((m_pClientList[wClientH] != 0)&& (strcmp(m_pClientList[wClientH]->m_cCharName, cName) == 0)&&(m_pClientList[wClientH]->m_iAdminUserLevel > 0)) {
					SendNotifyMsg(0, wClientH, DEF_NOTIFY_PLAYERSHUTUP, wV1, 0, 0, cTemp);

				}
			}
			break;

		// New 16/05/2004
		case GSM_WHISFERMSG:
			ZeroMemory(cName, sizeof(cName));
			ZeroMemory(cBuffer, sizeof(cBuffer));
			cp++;
			memcpy(cName, cp, 10);
			cp += 10;
			wp = (UINT16 *)cp;
			wV1 = *wp;
			cp += 2;

			memcpy(cBuffer, cp, wV1);
			cp += wV1;

			for (i = 1; i < DEF_MAXCLIENTS; i++)
				if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cCharName, cName, 10) == 0)) {
					iRet = m_pClientList[i]->m_pXSock->iSendMsg(cBuffer, wV1);
					if(	m_pClientList[i]->m_iAdminUserLevel > 0) {
						char cTxt[200],cTmpName[12] ;
						ZeroMemory(cTxt,sizeof(cTxt)) ;
						ZeroMemory(cTmpName,sizeof(cTmpName)) ;

						memcpy(cTmpName,cBuffer+10,10) ;
						wsprintf(cTxt,"PC Chat(%s):\"%s\"\tto GM(%s)",cTmpName,cBuffer+21,m_pClientList[i]->m_cCharName) ;
						bSendMsgToLS(MSGID_GAMEMASTERLOG, i, 0, cTxt);
					}
					break;
				}
			break;

		// New 16/05/2004 Changed
		case GSM_REQUEST_FINDCHARACTER:
			cp++;

			wp = (UINT16 *)cp;
			wServerID = *wp;
			cp += 2;

			wp = (UINT16 *)cp;
			wClientH = *wp;
			cp += 2;

			ZeroMemory(cTemp, sizeof(cTemp));
			memcpy(cTemp, cp, 10);
			cp += 10;

			ZeroMemory(cTemp2, sizeof(cTemp2));
			memcpy(cTemp2, cp, 10);
			cp += 10;

			GSM_RequestFindCharacter(wServerID, wClientH, cTemp, cTemp2);
			break;

		case GSM_RESPONSE_FINDCHARACTER:
			sX = -1;
			sY = -1;
			cp++;
			wp = (UINT16 *)cp;
			wServerID = *wp;
			cp += 2;
			wp = (UINT16 *)cp;
			wClientH = *wp;
			cp += 2;
			ZeroMemory(cTemp, sizeof(cTemp));
			memcpy(cTemp, cp, 10);
			cp += 10;

			ZeroMemory(cTemp2, sizeof(cTemp2));
			memcpy(cTemp2, cp, 10);
			cp += 10;

			ZeroMemory(cTemp3, sizeof(cTemp3));
			memcpy(cTemp3, cp, 14);

			if ((wServerID == m_wServerID_GSS) && m_pClientList[wClientH] != 0) {
				if (m_pClientList[wClientH]->m_bIsAdminOrderGoto == true) {
					m_pClientList[wClientH]->m_bIsAdminOrderGoto = false;
					ZeroMemory(cMapName, sizeof(cMapName));
					memcpy(cMapName, cp, 10);
					cp += 10;

					wp = (UINT16 *)cp;
					sX = *wp;
					cp += 2;

					wp = (UINT16 *)cp;
					sY = *wp;
					cp += 2;

					if (m_pClientList[wClientH]->m_iAdminUserLevel > 0) {
						if(sX == -1 && sY == -1){
							wsprintf(G_cTxt,"GM Order(%s): GoTo MapName(%s)",m_pClientList[wClientH]->m_cCharName, cMapName);
						}
						else{
							wsprintf(G_cTxt,"GM Order(%s): GoTo MapName(%s)(%d %d)",m_pClientList[wClientH]->m_cCharName,
								cMapName, sX, sY);
						}
						bSendMsgToLS(MSGID_GAMEMASTERLOG, wClientH, false, G_cTxt);
						RequestTeleportHandler(wClientH,"2   ", cMapName, sX, sY);
					}
				}
				else{
					if ((m_pClientList[wClientH]->m_bIsCheckingWhisperPlayer == true) && (memcmp(m_pClientList[wClientH]->m_cWhisperPlayerName, cTemp, 10) == 0)) {
						m_pClientList[wClientH]->m_iWhisperPlayerIndex = 10000;
						SendNotifyMsg(0, wClientH, DEF_NOTIFY_WHISPERMODEON, 0, 0, 0, m_pClientList[wClientH]->m_cWhisperPlayerName);
					}
					else{
						if (m_pClientList[wClientH]->m_iAdminUserLevel == 0) {
							ZeroMemory(cTemp3, sizeof(cTemp3));
						}
						if (memcmp(m_pClientList[wClientH]->m_cCharName, cTemp2, 10) == 0) {
							SendNotifyMsg(0, wClientH, DEF_NOTIFY_PLAYERONGAME, 0, 0, 0, cTemp, 0, 0, 0, 0, 0, 0, cTemp3);
						}
					}
				}
			}
			break;

		default:
			bFlag = true;
			break;
		}
	}
}

void CGame::GSM_RequestFindCharacter(UINT16 wReqServerID, UINT16 wReqClientH, char *pName, char * pFinder)
{
 char * cp, cTemp[120];
 UINT16 * wp;
 int i;

	for (i = 1; i < DEF_MAXCLIENTS; i++)
	if ((m_pClientList[i] != 0)  && (strcmp(m_pClientList[i]->m_cCharName, pName) == 0)) {
		// ÃƒÂ£Â¾Ã’Â´Ã™.
		ZeroMemory(cTemp, sizeof(cTemp));
		cp = (char *)(cTemp);
		*cp = GSM_RESPONSE_FINDCHARACTER;
		cp++;

		wp = (UINT16 *)cp;
		*wp = wReqServerID;
		cp += 2;

		wp = (UINT16 *)cp;
		*wp = wReqClientH;
		cp += 2;
		
		memcpy(cp, pName, 10);
		cp += 10;

		memcpy(cp, pFinder, 10);
		cp += 10;

		memcpy(cp, m_pClientList[i]->m_cMapName, 10);
		cp += 10;

		wp = (UINT16 *)cp;
		*wp = m_pClientList[i]->m_sX;
		cp += 2;

		wp = (UINT16 *)cp;
		*wp = m_pClientList[i]->m_sY;
		cp += 2;

		bStockMsgToGateServer(cTemp, 39);
		break;	
	}
}

bool CGame::bAddClientShortCut(int iClientH)
{
 int i; 

	for (i = 0; i < DEF_MAXCLIENTS; i++)
	if (m_iClientShortCut[i] == iClientH) return false;

	for (i = 0; i < DEF_MAXCLIENTS; i++)
	if (m_iClientShortCut[i] == 0) {
		m_iClientShortCut[i] = iClientH;
		return true;
	}

	return false;
}

void CGame::RemoveClientShortCut(int iClientH)
{
 int i;

	 for (i = 0; i < DEF_MAXCLIENTS + 1; i++) {
		 if (m_iClientShortCut[i] == iClientH) {
			 m_iClientShortCut[i] = 0;
			 break;
		 }
	 }

	 for (i = 0; i < DEF_MAXCLIENTS; i++) {
		 if ((m_iClientShortCut[i] == 0) && (m_iClientShortCut[i + 1] != 0)) {
			 m_iClientShortCut[i] = m_iClientShortCut[i + 1];
			 m_iClientShortCut[i + 1] = 0;
		 }
	 }
}

void CGame::RequestChangePlayMode(int iClientH)
{

	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_iPKCount > 0) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;

	if (memcmp(m_pClientList[iClientH]->m_cMapName,"cityhall",8) != 0) return;

	if (m_pClientList[iClientH]->m_iLevel < 50) {
		if (m_pClientList[iClientH]->m_bIsPlayerCivil == true) {
			if (memcmp(m_pClientList[iClientH]->m_cLocation, "arehunter", 9) == 0) strcpy(m_pClientList[iClientH]->m_cLocation, "aresden");
			else if (memcmp(m_pClientList[iClientH]->m_cLocation, "elvhunter", 9) == 0) strcpy(m_pClientList[iClientH]->m_cLocation, "elvine");
			m_pClientList[iClientH]->m_bIsPlayerCivil = false;
		}
		else {
			if (memcmp(m_pClientList[iClientH]->m_cLocation, "aresden", 7) == 0) strcpy(m_pClientList[iClientH]->m_cLocation, "arehunter");
			else if (memcmp(m_pClientList[iClientH]->m_cLocation, "elvine", 6) == 0) strcpy(m_pClientList[iClientH]->m_cLocation, "elvhunter");
			m_pClientList[iClientH]->m_bIsPlayerCivil = true;
		}
		SendNotifyMsg(0,iClientH,DEF_NOTIFY_CHANGEPLAYMODE,0,0,0,m_pClientList[iClientH]->m_cLocation);
		SendEventToNearClient_TypeA(iClientH,DEF_OWNERTYPE_PLAYER,MSGID_EVENT_MOTION,100,0,0,0);
	}
}

void CGame::RequestRango(int iClientH, int iObjectID)
{
	if (m_pClientList[iClientH] == 0) return;
	if ((iObjectID <= 0) || (iObjectID >= DEF_MAXCLIENTS)) return;

	if (m_pClientList[iObjectID] != 0) {
		// Â¿Ã¤ÃƒÂ» Â¹ÃžÃ€Âº ObjectÂ°Â¡ Â¾Ã¸Â´Ã™.

		// centu
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_REQRANGO, m_pClientList[iObjectID]->m_iWantedLevel, 0, 0, 0);
	}
}

// thx to kazin
void CGame::RequestOnlines(int iClientH)
{
	char * cData = G_cData50000; //esta es el "buffer", onda, la memoria que neceisto para enviar
	
	UINT32 * dwp  = (UINT32 *)(cData + DEF_INDEX4_MSGID);
	//LifeX Fix User Refresh
	*dwp = MSGID_RESPONSE_ONLINE;//a la memoria esa le escribo este mensaje
	UINT16 * wp   = (UINT16 *)(cData + DEF_INDEX2_MSGTYPE);			   
	*wp  = 0;//este no se usa, le mando 0

	char * cp = (char *)(cData	+ DEF_INDEX2_MSGTYPE + 2); //eso se usa para decirle "desde aca empiezo a escribir"

	short * pTotal = (short *)cp;
	cp += 2;

	int count = 0;
	for (int i = 0; i < DEF_MAXCLIENTS; i++) //recorro todosl os clientes
	{
		if (!m_pClientList[i])//si son nulls
			continue;//no darles bola y seguir

		memcpy(cp, m_pClientList[i]->m_cCharName, 10); //copiar el nombre del char (de tamaño 10) a la data
		cp += 10;//decirle a la data que le sume 10 bites

		memcpy(cp, m_pClientList[i]->m_cGuildName, 20); //copiar el nombre del char (de tamaño 10) a la data
		cp += 20;//decirle a la data que le sume 10 bites

		count++;
	}

	*pTotal = count; //esto es medio enroscado, es para decirle cuantos onlines escribio
	m_pClientList[iClientH]->m_pXSock->iSendMsg(G_cData50000, cp - G_cData50000);//mandarle la data a p que es el cliente
	
}

void CGame::GetExp(int iClientH, UINT32 iExp, bool bIsAttackerOwn)
{
	float dV1, dV2, dV3;
	int i, iH;
	UINT32 dwTime = timeGetTime();
	int iTotalPartyMembers;
	UINT32 iUnitValue;

	if (m_pClientList[iClientH] == 0) return;
	if (iExp <= 0) return;

	if (m_pClientList[iClientH]->m_iLevel <= 80) 
	{
		dV1 = (float)(80 - m_pClientList[iClientH]->m_iLevel);
		dV2 = dV1 * 0.025f;
		dV3 = (float)iExp;
		dV1 = (dV2 + 1.025f)*dV3;
		iExp = (UINT32)dV1;
	}
		
	//Check for party status, else give exp to player
	if ((m_pClientList[iClientH]->m_iPartyID != 0) && (m_pClientList[iClientH]->m_iPartyStatus == DEF_PARTYSTATUS_CONFIRM))
	{
		//Only divide exp if > 1 person 
		if(m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iTotalMembers > 0){	
			iTotalPartyMembers = 0;
			for(i = 0; i < m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iTotalMembers; i++){
				iH = m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iIndex[i];
				if ((m_pClientList[iH] != 0) && (m_pClientList[iH]->m_iHP > 0)) {
					if ((strlen(m_pMapList[m_pClientList[iH]->m_cMapIndex]->m_cName)) == (strlen(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName))) {
						if(memcmp(m_pMapList[m_pClientList[iH]->m_cMapIndex]->m_cName, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, strlen(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName)) == 0){
							iTotalPartyMembers++;
						}
					}
				}
			}

			//Check for party bug
			if(iTotalPartyMembers > 8){
				wsprintf(G_cTxt, "(X) Party Bug !! partyMember %d", iTotalPartyMembers);
				PutLogFileList(G_cTxt);
				iTotalPartyMembers = 8;
			}

			//Figure out how much exp a player gets
			dV1 = (float)iExp;

			switch(iTotalPartyMembers){
			case 1:
				dV2 = dV1;
				break;
			case 2:
				dV2 = (dV1 + (dV1 * 0.02f)) / 2.0f;
				break;
			case 3:
				dV2 = (dV1 + (dV1 * 0.05f)) / 3.0f;
				break;
			case 4:
				dV2 = (dV1 + (dV1 * 0.07f)) / 4.0f;
				break;
			case 5:
				dV2 = (dV1 + (dV1 * 0.1f)) / 5.0f;
				break;
			case 6:
				dV2 = (dV1 + (dV1 * 0.14f)) / 6.0f;
				break;
			case 7:
				dV2 = (dV1 + (dV1 * 0.17f)) / 7.0f;
				break;
			case 8:
				dV2 = (dV1 + (dV1 * 0.2f)) / 8.0f;
				break;
			}

			dV3 = dV2 + 0.5f;
			iUnitValue = (UINT32)dV3;

			//Divide exp among party members
			for(i = 0; i < iTotalPartyMembers; i++){
				iH = m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iIndex[i];
				//Player who kills monster gets more exp
				if(iH == iClientH){
					if((m_pClientList[iH] != 0) && (m_pClientList[iH]->m_iHP > 0)) { // Is player alive ??
						//Exp gain based on lvl
						if((m_pClientList[iH]->m_iStatus & 0x10000) != 0) iUnitValue *= 3;
						MultiplicadorExp(iH, iUnitValue);
					}
				}
				else{
					//Other players get normal exp :P
					if((m_pClientList[iH] != 0) && (m_pClientList[iH]->m_iHP > 0))
						if(m_pClientList[iH]->m_bSkillUsingStatus[19] != 1)
							//Only players on same map get exp
							if(m_pClientList[iH]->m_cMapIndex == m_pClientList[iClientH]->m_cMapIndex)
								MultiplicadorExp(iH, iUnitValue);
				}
			}

			if (bIsAttackerOwn == true)
				if (iTotalPartyMembers > 1)
					MultiplicadorExp(iClientH, (iUnitValue/iTotalPartyMembers));
		}
		else
		{
			if ((m_pClientList[iClientH]->m_iStatus & 0x10000) != 0) iExp *= 3;	
			MultiplicadorExp(iClientH, iExp);
		}
	}
	else
	{
		if ((m_pClientList[iClientH]->m_iStatus & 0x10000) != 0) iExp *= 3;
		MultiplicadorExp(iClientH, iExp);
	}
}

void CGame::MultiplicadorExp(int Client, UINT32 Exp)
{
	if (m_pClientList[Client] == 0) return;

	if (m_pClientList[Client]->m_iAdminUserLevel > 0)
		m_pClientList[Client]->m_iExp = 0;
	else
		m_pClientList[Client]->m_iExp += Exp* m_iExpSetting;
	
	if (bCheckLimitedUser(Client) == false) {
		// ÃƒÂ¼Ã‡Ã¨Ã†Ã‡ Â»Ã§Â¿Ã«Ã€Ãš ÃÂ¦Ã‡Ã‘Â¿Â¡ Ã‡Ã˜Â´Ã§ÂµÃ‡ÃÃ¶ Â¾ÃŠÃ€Â¸Â¸Ã© Â°Ã¦Ã‡Ã¨Ã„Â¡Â°Â¡ Â¿ÃƒÂ¶ÃºÂ´Ã™Â´Ã‚ Ã…Ã«ÂºÂ¸Â¸Â¦ Ã‡Ã‘Â´Ã™.
		SendNotifyMsg(0, Client, DEF_NOTIFY_EXP, 0, 0, 0, 0);
	}
	bCheckLevelUp(Client);
}

void CGame::AdminOrder_GoTo(int iClientH, char* pData, UINT32 dwMsgSize)
{
	char   seps[] = "= \t\n";
	char   * token, cBuff[256], cPlayerName[11],cMapName[32];
	class  CStrTok * pStrTok;
	char *cp;
	UINT16 *wp;

	if (m_pClientList[iClientH] == 0) return;
	if ((dwMsgSize)	<= 0) return;

	if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelGoto) {
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
		return;
	}

	ZeroMemory(cPlayerName, sizeof(cPlayerName));
	ZeroMemory(cBuff, sizeof(cBuff));
	memcpy(cBuff, pData, dwMsgSize);

	pStrTok = new class CStrTok(cBuff, seps);
	token = pStrTok->pGet();

	token = pStrTok->pGet();
	if (token == 0) {
		delete pStrTok;
		return;
	}

	if (strlen(token) > 10) {
		memcpy(cPlayerName,token,10);
	}
	else{
		memcpy(cPlayerName,token,strlen(token));
	}

	for(int i = 1; i < DEF_MAXCLIENTS; i++){
		if (m_pClientList[i] != 0) {
			if (memcmp(cPlayerName,m_pClientList[i]->m_cCharName, 10) == 0) {
				if (m_pClientList[iClientH]->m_iAdminUserLevel > 0) {
					if(m_pClientList[i]->m_sX == -1 && m_pClientList[i]->m_sY == -1){
						wsprintf(cBuff,"GM Order(%s): GoTo MapName(%s)",m_pClientList[iClientH]->m_cCharName,m_pClientList[i]->m_cMapName);
					}
					else{
						wsprintf(cBuff,"GM Order(%s): GoTo MapName(%s)(%d %d)",m_pClientList[iClientH]->m_cCharName,
							     m_pClientList[i]->m_cMapName,m_pClientList[i]->m_sX,m_pClientList[i]->m_sY);
					}
					bSendMsgToLS(MSGID_GAMEMASTERLOG,iClientH,false,cBuff);
					ZeroMemory(cMapName,sizeof(cMapName));
					strcpy(cMapName, m_pClientList[i]->m_cMapName);
					RequestTeleportHandler(iClientH,"2   ", cMapName, m_pClientList[i]->m_sX,m_pClientList[i]->m_sY);
					

				}
				delete pStrTok;
				return;
			}
		}
	}
	m_pClientList[iClientH]->m_bIsAdminOrderGoto = true;

	ZeroMemory(cBuff,sizeof(cBuff));
	
	cp = (char *)cBuff;
	*cp = GSM_REQUEST_FINDCHARACTER;
	cp++;

	wp = (UINT16 *)cp;
	*wp = m_wServerID_GSS;
	cp += 2;

	wp = (UINT16 *)cp;
	*wp = iClientH;
	cp += 2;

	memcpy(cp,cPlayerName,10);
	cp += 10;

	memcpy(cp,m_pClientList[iClientH]->m_cCharName,10);
	cp += 10;

	bStockMsgToGateServer(cBuff,25);

	delete pStrTok;
}

void CGame::AdminOrder_MonsterCount(int iClientH, char* pData, UINT32 dwMsgSize)
{
	int iMonsterCount;

	if (m_pClientList[iClientH] == 0) return;
	if ((dwMsgSize)	<= 0) return;

	if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelMonsterCount) {
		// Admin user levelÃ€ÃŒ Â³Â·Â¾Ã†Â¼Â­ Ã€ÃŒ Â±Ã¢Â´Ã‰Ã€Â» Â»Ã§Â¿Ã«Ã‡Ã’ Â¼Ã¶ Â¾Ã¸Â´Ã™.
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
		return;
	}
	iMonsterCount = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_iTotalAliveObject;
	SendNotifyMsg(0,iClientH, DEF_NOTIFY_MONSTERCOUNT, iMonsterCount,0,0,0);
}

// v2.17 2002-7-15 ¿î¿µÀÚ ¸í·É¾î·Î °­ÄÝ½Ã°£À» ¼³Á¤ÇÑ´Ù.
void CGame::AdminOrder_SetForceRecallTime(int iClientH, char *pData, UINT32 dwMsgSize)
{
 class  CStrTok * pStrTok;
 char   seps[] = "= \t\n";
 char   * token, cBuff[256],* cp; 
 UINT16 * wp ;
 int    iTime;

	if (m_pClientList[iClientH] == 0) return;
	if ((dwMsgSize)	<= 0) return;


	if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelSetRecallTime) {
		// Admin user levelÀÌ ³·¾Æ¼­ ÀÌ ±â´ÉÀ» »ç¿ëÇÒ ¼ö ¾ø´Ù.
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
		return;
	}

	ZeroMemory(cBuff, sizeof(cBuff));
	memcpy(cBuff, pData, dwMsgSize);

	pStrTok = new class CStrTok(cBuff, seps);
	token = pStrTok->pGet();
	token = pStrTok->pGet();
   	
	if (token != 0) {

		if (token == 0) 
			 iTime = 0;
		else iTime = atoi(token);
	
		if (iTime < 0) iTime = 0;
	 	
		m_sForceRecallTime = iTime ;

		ZeroMemory(cBuff, sizeof(cBuff));
		cp = (char *)cBuff;
		*cp = GSM_REQUEST_SETFORCERECALLTIME;
		cp++;
	
		wp = (UINT16 *)cp;
		*wp = iTime ;
		cp += 2;

		bStockMsgToGateServer(cBuff, 3);

		wsprintf(G_cTxt,"(!) Game Server Force Recall Time (%d)min",m_sForceRecallTime) ;
		PutLogList(G_cTxt) ;
	}

	delete pStrTok;
}

void CGame::AdminOrder_UnsummonBoss(int iClientH)
{
	if (m_pClientList[iClientH] == 0) return;

	if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelUnsummonBoss) {
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
		return;
	}

	for(int i = 1; i < DEF_MAXNPCS; i++){
		if (m_pNpcList[i] != 0 && m_pNpcList[i]->m_bIsSummoned == false) {
			switch(m_pNpcList[i]->m_sType) {
			case 31: //Demon
			case 49: //Hellclaw
			case 45: //GHKABS
			case 47: //BG
			case 50: //Tigerworm
			case 52: //Gagoyle
			case 81: //Abaddon
			case 66: //Wyvern
			case 73: //Fire-Wyvern
			case 92: // Eternal Dragon
			case 93: // BlackDemon
			case 94: // BlackWyv
			case 95: // LighWyvern
			case 96: // PoisonWyvern
			case 97: // HeavenWyvern
			case 98: // IllusionWyvern
			case 99: //Ghost Abaadon
				m_pNpcList[i]->m_bIsUnsummoned = true;
				NpcKilledHandler(iClientH, DEF_OWNERTYPE_PLAYER, i, 0);
				break;
			}
		}
	}
}

void CGame::AdminOrder_ClearNpc(int iClientH)
{
	if (m_pClientList[iClientH] == 0) return;

	if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelClearNpc) {
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
		return;
	}

	for (int m = 0; m < DEF_MAXMAPS; m++) { //Enum all maps
		if (m_pMapList[m] != 0) {	//Is allocated map
			if (memcmp(m_pMapList[m]->m_cName, m_pClientList[iClientH]->m_cMapName, 11) == 0) {	//is map same name
				for (int i = 1; i < DEF_MAXNPCS; i++) {
					if (m_pNpcList[i] != 0 && m_pNpcList[i]->m_bIsSummoned == false) {
						switch (m_pNpcList[i]->m_sType) {
						case 15:
						case 19:
						case 20:
						case 24:
						case 25:
						case 26:
						case 67:
						case 68:
						case 69:
						case 90:
						case 91:
							break;

						default:
							m_pNpcList[i]->m_bIsUnsummoned = true;
							NpcKilledHandler(iClientH, DEF_OWNERTYPE_PLAYER, i, 0);
							break;
						}
					}
				}
			}
		}
	}
}

// Testcode, found reference to it in 3.51, InitPlayerData and RequestTeleportHandler
int CGame::iGetPlayerABSStatus(int iClientH)
{
	int iRet;

	if (m_pClientList[iClientH] == 0) return 0;

	iRet = 0;

		if (m_pClientList[iClientH]->m_iPKCount != 0) {
			iRet = 8;
		}

		if (m_pClientList[iClientH]->m_cSide != 0) {
			iRet = iRet | 4;
		}

		if (m_pClientList[iClientH]->m_cSide == 1) {
			iRet = iRet | 2;
		}

		if (m_pClientList[iClientH]->m_bIsPlayerCivil == true)
			iRet = iRet | 1;

		return iRet;
}

// New 18/05/2004
void CGame::SetPlayingStatus(int iClientH)
{
	char cMapName[11], cLocation[11];

	if (m_pClientList[iClientH] == 0) return;

	ZeroMemory(cMapName, sizeof(cMapName));
	ZeroMemory(cLocation, sizeof(cLocation));

	strcpy(cLocation, m_pClientList[iClientH]->m_cLocation);
	strcpy(cMapName, m_pMapList[ m_pClientList[iClientH]->m_cMapIndex]->m_cName);

	m_pClientList[iClientH]->m_cSide = 0;
	m_pClientList[iClientH]->m_bIsOwnLocation = false;
	m_pClientList[iClientH]->m_bIsPlayerCivil = false;

	if (memcmp(cLocation, cMapName, 3) == 0) {
		m_pClientList[iClientH]->m_bIsOwnLocation = true;
	}

	if (memcmp(cLocation, "are", 3) == 0)
		m_pClientList[iClientH]->m_cSide = 1;
	else if (memcmp(cLocation, "elv", 3) == 0)
		m_pClientList[iClientH]->m_cSide = 2;
	else{ 
		if (strcmp(cMapName, "elvine") == 0 || strcmp(cMapName, "aresden") == 0) {
			m_pClientList[iClientH]->m_bIsOwnLocation = true;
		}
		m_pClientList[iClientH]->m_bIsNeutral = true;
	}

	if (memcmp(cLocation, "arehunter", 9) == 0 || memcmp(cLocation, "elvhunter", 9) == 0) {
		m_pClientList[iClientH]->m_bIsPlayerCivil = true;
	}

	if (memcmp(m_pClientList[iClientH]->m_cMapName, "bisle", 5) == 0) {
		m_pClientList[iClientH]->m_bIsPlayerCivil = false;
	}

	// centu - all npc in cityhall
	if (memcmp(m_pClientList[iClientH]->m_cMapName, "cityhall", 8) == 0) {
		m_pClientList[iClientH]->m_pIsProcessingAllowed = true;
		m_pClientList[iClientH]->m_bIsInsideWarehouse = true;
		m_pClientList[iClientH]->m_bIsInsideWizardTower = true;
	}
	else {
		m_pClientList[iClientH]->m_pIsProcessingAllowed = false;
		m_pClientList[iClientH]->m_bIsInsideWarehouse = false;
		m_pClientList[iClientH]->m_bIsInsideWizardTower = false;
	}

	//Magn0S:: 04/2013 - Set apocalypse maps
	if ((memcmp(m_pClientList[iClientH]->m_cMapName, "procella", 8) == 0)
		|| (memcmp(m_pClientList[iClientH]->m_cMapName, "abaddon", 7) == 0))
		m_pClientList[iClientH]->m_bIsOnApocMap = true;
	else
		m_pClientList[iClientH]->m_bIsOnApocMap = false;
}

void CGame::ForceChangePlayMode(int iClientH)
{
	if(m_pClientList[iClientH] == 0) return;

	if (memcmp(m_pClientList[iClientH]->m_cLocation, "arehunter", 9) == 0)
		strcpy(m_pClientList[iClientH]->m_cLocation, "aresden");
	else if (memcmp(m_pClientList[iClientH]->m_cLocation, "elvhunter", 9) == 0)
		strcpy(m_pClientList[iClientH]->m_cLocation, "elvine");

	if (m_pClientList[iClientH]->m_bIsPlayerCivil == true)
		m_pClientList[iClientH]->m_bIsPlayerCivil = false;

	SendNotifyMsg(0, iClientH, DEF_NOTIFY_CHANGEPLAYMODE, 0, 0, 0, m_pClientList[iClientH]->m_cLocation);
	SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
	
}

// v2.15 2002-5-21
void CGame::GSM_RequestShutupPlayer(char * pGMName,UINT16 wReqServerID, UINT16 wReqClientH, UINT16 wTime,char * pPlayer )
{
	char * cp, cTemp[120];
	UINT16 * wp;
 int i;

	for (i = 1; i < DEF_MAXCLIENTS; i++)
		if ((m_pClientList[i] != 0) && (strcmp(m_pClientList[i]->m_cCharName, pPlayer) == 0)) {
			// ÃƒÂ£Â¾Ã’Â´Ã™.
			ZeroMemory(cTemp, sizeof(cTemp));
			cp = (char *)(cTemp);
			*cp = GSM_RESPONSE_SHUTUPPLAYER;
			cp++;

			wp = (UINT16 *)cp;
			*wp = wReqServerID;
			cp += 2;

			wp = (UINT16 *)cp;
			*wp = wReqClientH;
			cp += 2;

			memcpy(cp, pGMName, 10);
			cp += 10;

			wp = (UINT16 *)cp;
			*wp = (UINT16) wTime;
			cp += 2;

			memcpy(cp, pPlayer, 10);
			cp += 10;

			// v2.14 Â¼Ã‹Â¾Ã·Â½ÃƒÂ°Â£Ã€Â» ÂºÃÃ€Â¸Â·ÃŽ Â¼Ã¶ÃÂ¤
			m_pClientList[i]->m_iTimeLeft_ShutUp = wTime*20; // 1Ã€ÃŒ 3ÃƒÃŠÂ´Ã™. 20Ã€ÃŒÂ¸Ã© 1ÂºÃ 

			SendNotifyMsg(0, i, DEF_NOTIFY_PLAYERSHUTUP, wTime, 0, 0, pPlayer);

			bStockMsgToGateServer(cTemp, 27);
			break;	
		}
}

// v2.14 05/22/2004 - Hypnotoad - adds pk log
bool CGame::_bPKLog(int iAction,int iAttackerH , int iVictumH, char * pNPC)
{
	char  cTxt[1024], cTemp1[120], cTemp2[120];

	//  Â·ÃŽÂ±Ã— Â³Â²Â±Ã¤Â´Ã™. 
	ZeroMemory(cTxt, sizeof(cTxt));
	ZeroMemory(cTemp1, sizeof(cTemp1));
	ZeroMemory(cTemp2, sizeof(cTemp2));

	if ( m_pClientList[iVictumH] == 0) return false ;

	switch (iAction) {

		case DEF_PKLOG_REDUCECRIMINAL: 
			wsprintf(cTxt, "(%s) PC(%s)\tReduce\tCC(%d)\t%s(%d %d)\t", m_pClientList[iVictumH]->m_cIPaddress, m_pClientList[iVictumH]->m_cCharName, m_pClientList[iVictumH]->m_iPKCount,
				m_pClientList[iVictumH]->m_cMapName,m_pClientList[iVictumH]->m_sX,m_pClientList[iVictumH]->m_sY);
			break; 

		case DEF_PKLOG_BYPLAYER:
			if ( m_pClientList[iAttackerH] == 0) return false ;
			wsprintf(cTxt, "(%s) PC(%s)\tKilled by PC\t \t%s(%d %d)\t(%s) PC(%s)", m_pClientList[iVictumH]->m_cIPaddress, m_pClientList[iVictumH]->m_cCharName, 
				m_pClientList[iVictumH]->m_cMapName,m_pClientList[iVictumH]->m_sX,m_pClientList[iVictumH]->m_sY,m_pClientList[iAttackerH]->m_cIPaddress, m_pClientList[iAttackerH]->m_cCharName);
			break;
		case DEF_PKLOG_BYPK:
			if ( m_pClientList[iAttackerH] == 0) return false ;
			wsprintf(cTxt, "(%s) PC(%s)\tKilled by PK\tCC(%d)\t%s(%d %d)\t(%s) PC(%s)", m_pClientList[iVictumH]->m_cIPaddress, m_pClientList[iVictumH]->m_cCharName, m_pClientList[iAttackerH]->m_iPKCount,
				m_pClientList[iVictumH]->m_cMapName,m_pClientList[iVictumH]->m_sX,m_pClientList[iVictumH]->m_sY,m_pClientList[iAttackerH]->m_cIPaddress, m_pClientList[iAttackerH]->m_cCharName);
			break; 
		case DEF_PKLOG_BYENERMY:
			if ( m_pClientList[iAttackerH] == 0) return false ;
			wsprintf(cTxt, "(%s) PC(%s)\tKilled by Enemy\t \t%s(%d %d)\t(%s) PC(%s)", m_pClientList[iVictumH]->m_cIPaddress, m_pClientList[iVictumH]->m_cCharName, 
				m_pClientList[iVictumH]->m_cMapName,m_pClientList[iVictumH]->m_sX,m_pClientList[iVictumH]->m_sY,m_pClientList[iAttackerH]->m_cIPaddress, m_pClientList[iAttackerH]->m_cCharName);
			break; 
		case DEF_PKLOG_BYNPC:
			if(pNPC==0) return false ;
			wsprintf(cTxt, "(%s) PC(%s)\tKilled by NPC\t \t%s(%d %d)\tNPC(%s)", m_pClientList[iVictumH]->m_cIPaddress, m_pClientList[iVictumH]->m_cCharName, 
				m_pClientList[iVictumH]->m_cMapName,m_pClientList[iVictumH]->m_sX,m_pClientList[iVictumH]->m_sY,pNPC);
			break; 
		case DEF_PKLOG_BYOTHER:
			wsprintf(cTxt, "(%s) PC(%s)\tKilled by Other\t \t%s(%d %d)\tUnknown", m_pClientList[iVictumH]->m_cIPaddress, m_pClientList[iVictumH]->m_cCharName, 
				m_pClientList[iVictumH]->m_cMapName,m_pClientList[iVictumH]->m_sX,m_pClientList[iVictumH]->m_sY);
			break; 
		default:
			return false;
	}
	
	return true ;
}

void CGame::RequestResurrectPlayer(int iClientH, bool bResurrect)
{
short sX, sY;
char buff[100];

	if (m_pClientList[iClientH] == 0) return;

	sX = m_pClientList[iClientH]->m_sX;
	sY = m_pClientList[iClientH]->m_sY;

	if (bResurrect == false) {
		m_pClientList[iClientH]->m_bIsBeingResurrected = false;
		return;
	}

	if (m_pClientList[iClientH]->m_bIsBeingResurrected == false) {
		wsprintf(buff, "(!!!) Player(%s) Tried To Use Resurrection Hack", m_pClientList[iClientH]->m_cCharName);
		PutHackLogFileList(G_cTxt);

		return;
	}

	wsprintf(buff, "(*) Resurrect Player! %s", m_pClientList[iClientH]->m_cCharName);
	PutLogList(buff);


	m_pClientList[iClientH]->m_bIsKilled = false;
	// Player's HP becomes half of the Max HP. 
	m_pClientList[iClientH]->m_iHP = iGetMaxHP(iClientH)/2; 
	// Player's MP
	m_pClientList[iClientH]->m_iMP = iGetMaxMP(iClientH);
	// Player's SP
	m_pClientList[iClientH]->m_iSP = iGetMaxSP(iClientH);
	SendNotifyMsg(0, iClientH, DEF_NOTIFY_HP, 0, 0, 0, 0);
	SendNotifyMsg(0, iClientH, DEF_NOTIFY_MP, 0, 0, 0, 0);
	SendNotifyMsg(0, iClientH, DEF_NOTIFY_SP, 0, 0, 0, 0);
	// Player's Hunger
	m_pClientList[iClientH]->m_iHungerStatus = 100;

	m_pClientList[iClientH]->m_bIsBeingResurrected = false;

	// !!! RequestTeleportHandlerÂ³Â»Â¿Â¡Â¼Â­ m_cMapNameÃ€Â» Â¾Â²Â±Ã¢ Â¶Â§Â¹Â®Â¿Â¡ Â±Ã—Â´Ã«Â·ÃŽ Ã†Ã„Â¶Ã³Â¹ÃŒÃ…ÃÂ·ÃŽ Â³Ã‘Â°ÃœÃÃ–Â¸Ã© Â¿Ã€ÂµÂ¿Ã€Ã›
	RequestTeleportHandler(iClientH, "2   ", m_pClientList[iClientH]->m_cMapName, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY);
}

void CGame::bCheckClientAttackFrequency(int iClientH, UINT32 dwClientTime)
{
 UINT32 dwTimeGap;

	if (m_pClientList[iClientH] == 0) return ;
	if (m_pClientList[iClientH]->m_iAdminUserLevel > 0) return ;

	if (m_pClientList[iClientH]->m_dwAttackFreqTime == 0) 
		m_pClientList[iClientH]->m_dwAttackFreqTime = dwClientTime;
	else {
		dwTimeGap = dwClientTime - m_pClientList[iClientH]->m_dwAttackFreqTime;
		m_pClientList[iClientH]->m_dwAttackFreqTime = dwClientTime;

		if (dwTimeGap < 1000) {
			wsprintf(G_cTxt, "Swing Hack: (%s) Player: (%s) - attacking with weapon at irregular rates.", m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName);
			PutHackLogFileList(G_cTxt);


		}
	}

}

void CGame::bCheckClientMagicFrequency(int iClientH, UINT32 dwClientTime)
{
 UINT32 dwTimeGap;
	
	if (m_pClientList[iClientH] == 0) return ;

	if (m_pClientList[iClientH]->m_dwMagicFreqTime == 0) 
		m_pClientList[iClientH]->m_dwMagicFreqTime = dwClientTime;
	else {
		dwTimeGap = dwClientTime - m_pClientList[iClientH]->m_dwMagicFreqTime;
		m_pClientList[iClientH]->m_dwMagicFreqTime = dwClientTime;

		if ((dwTimeGap < 1500) && (m_pClientList[iClientH]->m_bMagicConfirm == true)) {
			wsprintf(G_cTxt, "Speed Cast: (%s) Player: (%s) - casting magic at irregular rates. ", m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName);
			PutHackLogFileList(G_cTxt);
			

		}
		
		m_pClientList[iClientH]->m_iSpellCount--;
		m_pClientList[iClientH]->m_bMagicConfirm = false;
		m_pClientList[iClientH]->m_bMagicPauseTime = false;
	}
}

void CGame::bCheckClientMoveFrequency(int iClientH, UINT32 dwClientTime)
{
 UINT32 dwTimeGap;
	
	if (m_pClientList[iClientH] == 0) return ;
	if (m_pClientList[iClientH]->m_iAdminUserLevel > 0) return ;

	if (m_pClientList[iClientH]->m_dwMoveFreqTime == 0) 
		m_pClientList[iClientH]->m_dwMoveFreqTime = dwClientTime;
	else {
		if (m_pClientList[iClientH]->m_bIsMoveBlocked == true) {
			m_pClientList[iClientH]->m_dwMoveFreqTime = 0;
			m_pClientList[iClientH]->m_bIsMoveBlocked = false;
			return ;
		}

		if (m_pClientList[iClientH]->m_bIsAttackModeChange == true) {
			m_pClientList[iClientH]->m_dwMoveFreqTime = 0;
			m_pClientList[iClientH]->m_bIsAttackModeChange = false;
			return ;
		}

		dwTimeGap = dwClientTime - m_pClientList[iClientH]->m_dwMoveFreqTime;
		m_pClientList[iClientH]->m_dwMoveFreqTime = dwClientTime;
		
		if ((dwTimeGap < 200) && (dwTimeGap >= 0)) {
			wsprintf(G_cTxt, "Speed Hack: (%s) Player: (%s) - running too fast.", m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName);
			PutHackLogFileList(G_cTxt);
			

		}

	}
}

void CGame::bReadScheduleConfigFile(char *pFn)
{
 FILE * pFile;
 HANDLE hFile;
 UINT32  dwFileSize;
 char * cp, * token, cReadModeA, cReadModeB;
 char seps[] = "= \t\n";
 class CStrTok * pStrTok;
 int   iIndex;

	cReadModeA = 0;
	cReadModeB = 0;
	iIndex = 0;

	hFile = CreateFile(pFn, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
	dwFileSize = GetFileSize(hFile, 0);
	if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);

	pFile = fopen(pFn, "rt");
	if (pFile == 0) {
		// °ÔÀÓ¼­¹öÀÇ ÃÊ±âÈ­ ÆÄÀÏÀ» ÀÐÀ» ¼ö ¾ø´Ù.
		PutLogList("(!) Cannot open Schedule file.");
		return;
	}
	else {
		PutLogList("(!) Reading Schedule file...");
		cp = new char[dwFileSize+2];
		ZeroMemory(cp, dwFileSize+2);
		fread(cp, dwFileSize, 1, pFile);

		pStrTok = new class CStrTok(cp, seps);
		token = pStrTok->pGet();

		while( token != 0 )   {
			
			if (cReadModeA != 0) {
				switch (cReadModeA) {
				case 1:
					if (strcmp(m_cServerName, token) == 0) {
						PutLogList("(!) Success: This server will schedule Crusade Total War.");
						m_bIsCrusadeWarStarter = true;
					}
					cReadModeA = 0;
					break;

				case 2:
					if (strcmp(m_cServerName, token) == 0) {
						PutLogList("(!) Success: This server will schedule Apocalypse.");
						m_bIsApocalypseStarter = true;
					}
					cReadModeA = 0;
					break;

				case 3:
					switch (cReadModeB) {
					case 1:
						if (iIndex >= DEF_MAXSCHEDULE) {
							PutLogList("(!) WARNING! Too many Crusade War schedule!"); 
							return;
						}
						m_stCrusadeWarSchedule[iIndex].iDay = atoi(token);
						cReadModeB = 2;
						break;
					case 2:
						m_stCrusadeWarSchedule[iIndex].iHour = atoi(token);
						cReadModeB = 3;
						break;
					case 3:
						m_stCrusadeWarSchedule[iIndex].iMinute = atoi(token);
						iIndex++;
						cReadModeA = 0;
						cReadModeB = 0;
						break;
					}
					break;

				case 4:
					switch (cReadModeB) {
					case 1:
						if (iIndex >= DEF_MAXSCHEDULE) {
							PutLogList("(!) WARNING! Too many Apocalypse schedule!"); 
							return;
						}
						m_stApocalypseScheduleStart[iIndex].iDay = atoi(token);
						cReadModeB = 2;
						break;
					case 2:
						m_stApocalypseScheduleStart[iIndex].iHour = atoi(token);
						cReadModeB = 3;
						break;
					case 3:
						m_stApocalypseScheduleStart[iIndex].iMinute = atoi(token);
						iIndex++;
						m_bIsApocalypseScheduleLoaded = true;
						cReadModeA = 0;
						cReadModeB = 0;
						break;
					}
					break;

				case 5:
					
					cReadModeA = 0;
					break;

				case 6:
					switch (cReadModeB) {
					case 1:
						if (iIndex >= DEF_MAXSCHEDULE) {
							PutLogList("(!) WARNING! Too many Heldenian schedule!"); 
							return;
						}
						m_stHeldenianSchedule[iIndex].iDay = atoi(token);
						cReadModeB = 2;
						break;
					case 2:
						m_stHeldenianSchedule[iIndex].StartiHour = atoi(token);
						cReadModeB = 3;
						break;
					case 3:
						m_stHeldenianSchedule[iIndex].StartiMinute = atoi(token);
						iIndex++;
						m_bIsHeldenianScheduleLoaded = true;
						cReadModeA = 0;
						cReadModeB = 4;
						break;
					}
					
					break;

				case 7: // by Centuu.-
					if (strcmp(m_cServerName, token) == 0) {
						PutLogList("(!) Success: This server will schedule Heldenian.");
						m_bIsHeldenianReady = true;
					}
					cReadModeA = 0;
					break;

				case 8:
					switch (cReadModeB) {
					case 1:
						if (iIndex >= DEF_MAXSCHEDULE) {
							PutLogList("(!) WARNING! Too many Gladiator Arena schedule!");
							return;
						}
						m_stGladArenaSchedule[iIndex].iDay = atoi(token);
						cReadModeB = 2;
						break;
					case 2:
						m_stGladArenaSchedule[iIndex].iHour = atoi(token);
						cReadModeB = 3;
						break;
					case 3:
						m_stGladArenaSchedule[iIndex].iMinute = atoi(token);
						iIndex++;
						cReadModeA = 0;
						cReadModeB = 0;
						break;
					}
					
					break;

				case 9:
					switch (cReadModeB) {
					case 1:
						if (iIndex >= DEF_MAXSCHEDULE) {
							PutLogList("(!) WARNING! Too many CTF schedule!");
							return;
						}
						m_stCTFSchedule[iIndex].iDay = atoi(token);
						cReadModeB = 2;
						break;
					case 2:
						m_stCTFSchedule[iIndex].iHour = atoi(token);
						cReadModeB = 3;
						break;
					case 3:
						m_stCTFSchedule[iIndex].iMinute = atoi(token);
						iIndex++;
						cReadModeA = 0;
						cReadModeB = 0;
						break;
					}
					
					break;

				case 10:
					switch (cReadModeB) {
					case 1:
						if (iIndex >= DEF_MAXSCHEDULE) {
							PutLogList("(!) WARNING! Too many Invasion schedule!");
							return;
						}
						m_stInvasionSchedule[iIndex].iDay = atoi(token);
						cReadModeB = 2;
						break;
					case 2:
						m_stInvasionSchedule[iIndex].iHour = atoi(token);
						cReadModeB = 3;
						break;
					case 3:
						m_stInvasionSchedule[iIndex].iMinute = atoi(token);
						iIndex++;
						cReadModeA = 0;
						cReadModeB = 0;
						break;
					}
					
					break;

				case 11:
					switch (cReadModeB) {
					case 1:
						if (iIndex >= DEF_MAXSCHEDULE) {
							PutLogList("(!) WARNING! Too many Candy Fury schedule!");
							return;
						}
						m_stCandyFurySchedule[iIndex].iDay = atoi(token);
						cReadModeB = 2;
						break;
					case 2:
						m_stCandyFurySchedule[iIndex].iHour = atoi(token);
						cReadModeB = 3;
						break;
					case 3:
						m_stCandyFurySchedule[iIndex].iMinute = atoi(token);
						iIndex++;
						cReadModeA = 0;
						cReadModeB = 0;
						break;
					}
					
					break;

				case 12:
					switch (cReadModeB) {
					case 1:
						if (iIndex >= DEF_MAXSCHEDULE) {
							PutLogList("(!) WARNING! Too many Beholder schedule!");
							return;
						}
						m_stBeholderSchedule[iIndex].iDay = atoi(token);
						cReadModeB = 2;
						break;
					case 2:
						m_stBeholderSchedule[iIndex].iHour = atoi(token);
						cReadModeB = 3;
						break;
					case 3:
						m_stBeholderSchedule[iIndex].iMinute = atoi(token);
						iIndex++;
						cReadModeA = 0;
						cReadModeB = 0;
						break;
					}
					
					break;

				case 13:
					switch (cReadModeB) {
					case 1:
						if (iIndex >= DEF_MAXSCHEDULE) {
							PutLogList("(!) WARNING! Too many Bag Protect schedule!");
							return;
						}
						m_stBagProtectSchedule[iIndex].iDay = atoi(token);
						cReadModeB = 2;
						break;
					case 2:
						m_stBagProtectSchedule[iIndex].iHour = atoi(token);
						cReadModeB = 3;
						break;
					case 3:
						m_stBagProtectSchedule[iIndex].iMinute = atoi(token);
						iIndex++;
						cReadModeA = 0;
						cReadModeB = 0;
						break;
					}
					
					break;

				case 14:
					switch (cReadModeB) {
					case 1:
						if (iIndex >= DEF_MAXSCHEDULE) {
							PutLogList("(!) WARNING! Too many Team Arena schedule!");
							return;
						}
						m_stTeamArenaSchedule[iIndex].iDay = atoi(token);
						cReadModeB = 2;
						break;
					case 2:
						m_stTeamArenaSchedule[iIndex].iHour = atoi(token);
						cReadModeB = 3;
						break;
					case 3:
						m_stTeamArenaSchedule[iIndex].iMinute = atoi(token);
						iIndex++;
						cReadModeA = 0;
						cReadModeB = 0;
						break;
					}
					
					break;

				case 15:
					switch (cReadModeB) {
					case 1:
						if (iIndex >= DEF_MAXSCHEDULE) {
							PutLogList("(!) WARNING! Too many Shinning schedule!");
							return;
						}
						m_stShinningSchedule[iIndex].iDay = atoi(token);
						cReadModeB = 2;
						break;
					case 2:
						m_stShinningSchedule[iIndex].iHour = atoi(token);
						cReadModeB = 3;
						break;
					case 3:
						m_stShinningSchedule[iIndex].iMinute = atoi(token);
						iIndex++;
						cReadModeA = 0;
						cReadModeB = 0;
						break;
					}
					
					break;

				case 16:
					switch (cReadModeB) {
					case 1:
						if (iIndex >= DEF_MAXSCHEDULE) {
							PutLogList("(!) WARNING! Too many Happy Hour schedule!");
							return;
						}
						m_stHappyHourSchedule[iIndex].iDay = atoi(token);
						cReadModeB = 2;
						break;
					case 2:
						m_stHappyHourSchedule[iIndex].iHour = atoi(token);
						cReadModeB = 3;
						break;
					case 3:
						m_stHappyHourSchedule[iIndex].iMinute = atoi(token);
						iIndex++;
						cReadModeA = 0;
						cReadModeB = 0;
						break;
					}
					
					break;
				case 17:
					switch (cReadModeB) {
					case 1:
						if (iIndex >= DEF_MAXSCHEDULE) {
							PutLogList("(!) WARNING! Too many Fury Hour schedule!");
							return;
						}
						m_stFuryHourSchedule[iIndex].iDay = atoi(token);
						cReadModeB = 2;
						break;
					case 2:
						m_stFuryHourSchedule[iIndex].iHour = atoi(token);
						cReadModeB = 3;
						break;
					case 3:
						m_stFuryHourSchedule[iIndex].iMinute = atoi(token);
						iIndex++;
						cReadModeA = 0;
						cReadModeB = 0;
						break;
					}
					break;
				}
			}
			else {
				if (memcmp(token, "crusade-server-name", 19) == 0) {
					cReadModeA = 1;
					cReadModeB = 1;
				}
				if (memcmp(token, "apocalypse-server-name", 22) == 0) {
					cReadModeA = 2;
					cReadModeB = 1;
				}
				if (memcmp(token, "crusade-schedule", 16) == 0) {
					cReadModeA = 3;
					cReadModeB = 1;
				}
				if (memcmp(token, "apocalypse-schedule", 19) == 0) {
					cReadModeA = 4;
					cReadModeB = 1;
				}
				if (memcmp(token, "heldenian-schedule", 18) == 0) {
					cReadModeA = 6;
					cReadModeB = 1;
				}
				if (memcmp(token, "heldenian-server-name", 21) == 0) {
					cReadModeA = 7;
					cReadModeB = 1;
				}
				if (memcmp(token, "glad-arena", 10) == 0) {
					cReadModeA = 8;
					cReadModeB = 1;
				}
				if (memcmp(token, "ctf-event", 9) == 0) {
					cReadModeA = 9;
					cReadModeB = 1;
				}
				if (memcmp(token, "invasion-schedule", 17) == 0) {
					cReadModeA = 10;
					cReadModeB = 1;
				}
				if (memcmp(token, "candy-schedule", 14) == 0) {
					cReadModeA = 11;
					cReadModeB = 1;
				}
				if (memcmp(token, "beholder-schedule", 17) == 0) {
					cReadModeA = 12;
					cReadModeB = 1;
				}
				if (memcmp(token, "bag-protection", 14) == 0) {
					cReadModeA = 13;
					cReadModeB = 1;
				}
				if (memcmp(token, "team-arena", 10) == 0) {
					cReadModeA = 14;
					cReadModeB = 1;
				}
				if (memcmp(token, "shinning-schedule", 17) == 0) {
					cReadModeA = 15;
					cReadModeB = 1;
				}
				if (memcmp(token, "happyhour-schedule", 18) == 0) {
					cReadModeA = 16;
					cReadModeB = 1;
				}
				if (memcmp(token, "furyhour-schedule", 17) == 0) {
					cReadModeA = 17;
					cReadModeB = 1;
				}
			}
			
			token = pStrTok->pGet();
		}
		delete pStrTok;
		delete[] cp;
		fclose(pFile);
	}
}

void CGame::OnStartGameSignal()
{
 int i;
	
	for (i = 0; i < DEF_MAXMAPS; i++)
	if (m_pMapList[i] != 0) 
		__bReadMapInfo(i);
	

	bReadCrusadeStructureConfigFile("GameConfigs\\Crusade.cfg");
	_LinkStrikePointMapIndex();
	bReadScheduleConfigFile("GameConfigs\\Schedule.cfg");

	bReadCrusadeGUIDFile("GameConfigs\\CrusadeGUID.txt");
	bReadApocalypseGUIDFile("GameConfigs\\ApocalypseGUID.txt");
	bReadHeldenianGUIDFile("GameConfigs\\HeldenianGUID.txt");

	PutLogList("");
	PutLogList("(!) Game Server Activated.");
}

void CGame::CheckForceRecallTime(int iClientH)
{
	SYSTEMTIME SysTime;
	int iTL_;

	if(m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_iAdminUserLevel > 0) return;
	if (m_pClientList[iClientH]->m_iTimeLeft_ForceRecall == 0) {
		// has admin set a recall time ??
		if (m_sForceRecallTime > 0) {
			m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = m_sForceRecallTime * 20;
		}
		// use standard recall time calculations
		else {
			GetLocalTime(&SysTime);
			switch (SysTime.wDayOfWeek) {
			case 1:	m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = m_sRaidTimeMonday *60*1000; break;  //¿ù¿äÀÏ  3ºÐ 2002-09-10 #1
			case 2:	m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = m_sRaidTimeTuesday * 60 * 1000; break;  //È­¿äÀÏ  3ºÐ 
			case 3:	m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = m_sRaidTimeWednesday * 60 * 1000; break;  //¼ö¿äÀÏ  3ºÐ 
			case 4:	m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = m_sRaidTimeThursday * 60 * 1000; break;  //¸ñ¿äÀÏ  3ºÐ 
			case 5:	m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = m_sRaidTimeFriday * 60 * 1000; break; //±Ý¿äÀÏ 15ºÐ
			case 6:	m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = m_sRaidTimeSaturday * 60 * 1000; break; //Åä¿äÀÏ 45ºÐ 
			case 0:	m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = m_sRaidTimeSunday * 60 * 1000; break; //ÀÏ¿äÀÏ 60ºÐ
			}
		}
	}
	else {
		// has admin set a recall time ??
		if (m_sForceRecallTime > 0) {
			iTL_ = m_sForceRecallTime * 20;
		}
		// use standard recall time calculations
		else {
			GetLocalTime(&SysTime);
			switch (SysTime.wDayOfWeek) {
			case 1:	m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = m_sRaidTimeMonday * 60 * 1000; break;  //¿ù¿äÀÏ  3ºÐ 2002-09-10 #1
			case 2:	m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = m_sRaidTimeTuesday * 60 * 1000; break;  //È­¿äÀÏ  3ºÐ 
			case 3:	m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = m_sRaidTimeWednesday * 60 * 1000; break;  //¼ö¿äÀÏ  3ºÐ 
			case 4:	m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = m_sRaidTimeThursday * 60 * 1000; break;  //¸ñ¿äÀÏ  3ºÐ 
			case 5:	m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = m_sRaidTimeFriday * 60 * 1000; break; //±Ý¿äÀÏ 15ºÐ
			case 6:	m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = m_sRaidTimeSaturday * 60 * 1000; break; //Åä¿äÀÏ 45ºÐ 
			case 0:	m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = m_sRaidTimeSunday * 60 * 1000; break; //ÀÏ¿äÀÏ 60ºÐ
			}
		}
		if (m_pClientList[iClientH]->m_iTimeLeft_ForceRecall > iTL_) {
			m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = iTL_;
			m_pClientList[iClientH]->m_bIsWarLocation = true;
		}
	}
		
}

bool CGame::bReadAdminSetConfigFile(char * cFn)
{
	FILE * pFile;
	HANDLE hFile;
	UINT32  dwFileSize;
	char * cp, * token, cReadMode, cGSMode[16] = "";
	char seps[] = "= \t\n";
	class CStrTok * pStrTok;

	cReadMode = 0;

	hFile = CreateFile(cFn, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
	dwFileSize = GetFileSize(hFile, 0);
	if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);

	pFile = fopen(cFn, "rt");
	if (pFile != 0) {
	
		 PutLogList("(!) Reading Admin Settings file...");
		 cp = new char[dwFileSize+2];
		 ZeroMemory(cp, dwFileSize+2);
		 fread(cp, dwFileSize, 1, pFile);

		 pStrTok = new class CStrTok(cp, seps);
		 token = pStrTok->pGet();
		 while( token != 0 )   {

		 if (cReadMode != 0) {
		   switch (cReadMode) {

		   case 1:
			if ((strlen(token) > 0) && (strlen(token) < 9))
			{
				m_iAdminLevelGMKill = atoi(token);
			}
			else{
				m_iAdminLevelGMKill = 3;
			}
			cReadMode = 0;
			break;
//----------------------------------------------------------------
		   case 2:
			if ((strlen(token) > 0) && (strlen(token) < 9))
			{
				m_iAdminLevelGMRevive = atoi(token);
			}
			else{
				m_iAdminLevelGMRevive = 3;
			}
			cReadMode = 0;
			break;
//----------------------------------------------------------------
		   case 3:
			if ((strlen(token) > 0) && (strlen(token) < 9))
			{
				m_iAdminLevelGMCloseconn = atoi(token);
			}
			else{
				m_iAdminLevelGMCloseconn = 3;
			}
			cReadMode = 0;
			break;
//----------------------------------------------------------------
		   case 4:
			if ((strlen(token) > 0) && (strlen(token) < 9))
			{
				m_iAdminLevelGMCheckRep = atoi(token);
			}
			else{
				m_iAdminLevelGMCheckRep = 1;
			}
			cReadMode = 0;
			break;
//----------------------------------------------------------------
		  case 5:
			if ((strlen(token) > 0) && (strlen(token) < 9))
			{
				m_iAdminLevelWho = atoi(token);
			}
			else{
				m_iAdminLevelWho = 1;
			}
			cReadMode = 0;
			break;
//----------------------------------------------------------------
		  case 6:
			if ((strlen(token) > 0) && (strlen(token) < 9))
			{
				m_iAdminLevelEnergySphere = atoi(token);
			}
			else{
				m_iAdminLevelEnergySphere = 2;
			}
			cReadMode = 0;
			break;
//----------------------------------------------------------------
		  case 7:
			if ((strlen(token) > 0) && (strlen(token) < 9))
			{
				m_iAdminLevelShutdown = atoi(token);
			}
			else{
				m_iAdminLevelShutdown = 3;
			}
			cReadMode = 0;
			break;
//----------------------------------------------------------------
		  case 8:
			if ((strlen(token) > 0) && (strlen(token) < 9))
			{
				m_iAdminLevelObserver = atoi(token);
			}
			else{
				m_iAdminLevelObserver = 3;
			}
			cReadMode = 0;
			break;
//----------------------------------------------------------------
		  case 9:
			if ((strlen(token) > 0) && (strlen(token) < 9))
			{
				m_iAdminLevelShutup = atoi(token);
			}
			else{
				m_iAdminLevelShutup = 2;
			}
			cReadMode = 0;
			break;
//----------------------------------------------------------------
		  case 10:
			if ((strlen(token) > 0) && (strlen(token) < 9))
			{
				m_iAdminLevelCallGaurd = atoi(token);
			}
			else{
				m_iAdminLevelCallGaurd = 2;
			}
			cReadMode = 0;
			break;
//----------------------------------------------------------------
		  case 11:
			if ((strlen(token) > 0) && (strlen(token) < 9))
			{
				m_iAdminLevelSummonDemon = atoi(token);
			}
			else{
				m_iAdminLevelSummonDemon = 3;
			}
			cReadMode = 0;
			break;
//----------------------------------------------------------------		   
		  case 12:
			if ((strlen(token) > 0) && (strlen(token) < 9))
			{
				m_iAdminLevelSummonDeath = atoi(token);
			}
			else{
				m_iAdminLevelSummonDeath = 3;
			}
			cReadMode = 0;
			break;
//----------------------------------------------------------------
		  case 13:
			if ((strlen(token) > 0) && (strlen(token) < 9))
			{
				m_iAdminLevelReserveFightzone = atoi(token);
			}
			else{
				m_iAdminLevelReserveFightzone = 2;
			}
			cReadMode = 0;
			break;
//----------------------------------------------------------------
		  case 14:
			if ((strlen(token) > 0) && (strlen(token) < 9))
			{
				m_iAdminLevelCreateFish = atoi(token);
			}
			else{
				m_iAdminLevelCreateFish = 2;
			}
			cReadMode = 0;
			break;
//----------------------------------------------------------------
		  case 15:
			if ((strlen(token) > 0) && (strlen(token) < 9))
			{
				m_iAdminLevelTeleport = atoi(token);
			}
			else{
				m_iAdminLevelTeleport = 2;
			}
			cReadMode = 0;
			break;
//----------------------------------------------------------------
		  case 16:
			if ((strlen(token) > 0) && (strlen(token) < 9))
			{
				m_iAdminLevelCheckIP = atoi(token);
			}
			else{
				m_iAdminLevelCheckIP = 2;
			}
			cReadMode = 0;
			break;
//----------------------------------------------------------------
		  case 17:
			if ((strlen(token) > 0) && (strlen(token) < 9))
			{
				m_iAdminLevelPolymorph = atoi(token);
			}
			else{
				m_iAdminLevelPolymorph = 2;
			}
			cReadMode = 0;
			break;
//----------------------------------------------------------------
		  case 18:
			if ((strlen(token) > 0) && (strlen(token) < 9))
			{
				m_iAdminLevelSetInvis = atoi(token);
			}
			else{
				m_iAdminLevelSetInvis = 2;
			}
			cReadMode = 0;
			break;
//----------------------------------------------------------------	
		  case 19:
			if ((strlen(token) > 0) && (strlen(token) < 9))
			{
				m_iAdminLevelSetZerk = atoi(token);
			}
			else{
				m_iAdminLevelSetZerk = 2;
			}
			cReadMode = 0;
			break;
//----------------------------------------------------------------	
		  case 20:
			if ((strlen(token) > 0) && (strlen(token) < 9))
			{
				m_iAdminLevelSetIce = atoi(token);
			}
			else{
				m_iAdminLevelSetIce = 2;
			}
			cReadMode = 0;
			break;
//----------------------------------------------------------------	
		  case 21:
			if ((strlen(token) > 0) && (strlen(token) < 9))
			{
				m_iAdminLevelGetNpcStatus = atoi(token);
			}
			else{
				m_iAdminLevelGetNpcStatus = 2;
			}
			cReadMode = 0;
			break;
//----------------------------------------------------------------				
		  case 22:
			if ((strlen(token) > 0) && (strlen(token) < 9))
			{
				m_iAdminLevelSetAttackMode = atoi(token);
			}
			else{
				m_iAdminLevelSetAttackMode = 2;
			}
			cReadMode = 0;
			break;
//----------------------------------------------------------------	
		  case 23:
			if ((strlen(token) > 0) && (strlen(token) < 9))
			{
				m_iAdminLevelUnsummonAll = atoi(token);
			}
			else{
				m_iAdminLevelUnsummonAll = 3;
			}
			cReadMode = 0;
			break;
//----------------------------------------------------------------	
		  case 24:
			if ((strlen(token) > 0) && (strlen(token) < 9))
			{
				m_iAdminLevelUnsummonDemon = atoi(token);
			}
			else{
				m_iAdminLevelUnsummonDemon = 3;
			}
			cReadMode = 0;
			break;
//----------------------------------------------------------------	
		  case 25:
			if ((strlen(token) > 0) && (strlen(token) < 9))
			{
				m_iAdminLevelSummon = atoi(token);
			}
			else{
				m_iAdminLevelSummon = 3;
			}
			cReadMode = 0;
			break;
//----------------------------------------------------------------			
		  case 26:
			if ((strlen(token) > 0) && (strlen(token) < 9))
			{
				m_iAdminLevelSummonAll = atoi(token);
			}
			else{
				m_iAdminLevelSummonAll = 4;
			}
			cReadMode = 0;
			break;
//----------------------------------------------------------------
		  case 27:
			if ((strlen(token) > 0) && (strlen(token) < 9))
			{
				m_iAdminLevelSummonPlayer = atoi(token);
			}
			else{
				m_iAdminLevelSummonPlayer = 1;
			}
			cReadMode = 0;
			break;
//----------------------------------------------------------------	
		  case 28:
			if ((strlen(token) > 0) && (strlen(token) < 9))
			{
				m_iAdminLevelDisconnectAll = atoi(token);
			}
			else{
				m_iAdminLevelDisconnectAll = 2;
			}
			cReadMode = 0;
			break;
//----------------------------------------------------------------	
		  case 29:
			if ((strlen(token) > 0) && (strlen(token) < 9))
			{
				m_iAdminLevelEnableCreateItem = atoi(token);
			}
			else{
				m_iAdminLevelEnableCreateItem = 3;
			}
			cReadMode = 0;
			break;
//----------------------------------------------------------------			
		  case 30:
			if ((strlen(token) > 0) && (strlen(token) < 9))
			{
				m_iAdminLevelCreateItem = atoi(token);
			}
			else{
				m_iAdminLevelCreateItem = 4;
			}
			cReadMode = 0;
			break;
//----------------------------------------------------------------
		  case 31:
			if ((strlen(token) > 0) && (strlen(token) < 9))
			{
				m_iAdminLevelStorm = atoi(token);
			}
			else{
				m_iAdminLevelStorm = 3;
			}
			cReadMode = 0;
			break;
//----------------------------------------------------------------
		  case 32:
			if ((strlen(token) > 0) && (strlen(token) < 9))
			{
				m_iAdminLevelWeather = atoi(token);
			}
			else{
				m_iAdminLevelWeather = 2;
			}
			cReadMode = 0;
			break;
//----------------------------------------------------------------
		  case 33:
			if ((strlen(token) > 0) && (strlen(token) < 9))
			{
				m_iAdminLevelSetStatus = atoi(token);
			}
			else{
				m_iAdminLevelSetStatus = 2;
			}
			cReadMode = 0;
			break;
//----------------------------------------------------------------
		  case 34:
			if ((strlen(token) > 0) && (strlen(token) < 9))
			{
				m_iAdminLevelGoto = atoi(token);
			}
			else{
				m_iAdminLevelGoto = 1;
			}
			cReadMode = 0;
			break;
//----------------------------------------------------------------	
		  case 35:
			if ((strlen(token) > 0) && (strlen(token) < 9))
			{
				m_iAdminLevelMonsterCount = atoi(token);
			}
			else{
				m_iAdminLevelMonsterCount = 1;
			}
			cReadMode = 0;
			break;
//----------------------------------------------------------------			
		  case 36:
			if ((strlen(token) > 0) && (strlen(token) < 9))
			{
				m_iAdminLevelSetRecallTime = atoi(token);
			}
			else{
				m_iAdminLevelSetRecallTime = 3;
			}
			cReadMode = 0;
			break;
//----------------------------------------------------------------	
		  case 37:
			if ((strlen(token) > 0) && (strlen(token) < 9))
			{
				m_iAdminLevelUnsummonBoss = atoi(token);
			}
			else{
				m_iAdminLevelUnsummonBoss = 3;
			}
			cReadMode = 0;
			break;
//----------------------------------------------------------------	
		  case 38:
			if ((strlen(token) > 0) && (strlen(token) < 9))
			{
				m_iAdminLevelClearNpc = atoi(token);
			}
			else{
				m_iAdminLevelClearNpc = 3;
			}
			cReadMode = 0;
			break;
//----------------------------------------------------------------				
		  case 39:
			if ((strlen(token) > 0) && (strlen(token) < 9))
			{
				m_iAdminLevelTime = atoi(token);
			}
			else{
				m_iAdminLevelTime = 2;
			}
			cReadMode = 0;
			break;
//----------------------------------------------------------------
		  case 40:
			if ((strlen(token) > 0) && (strlen(token) < 9))
			{
				m_iAdminLevelPushPlayer = atoi(token);
			}
			else{
				m_iAdminLevelPushPlayer = 2;
			}
			cReadMode = 0;
			break;
//-----------------------------------------------------------------
		  case 41:
			if ((strlen(token) > 0) && (strlen(token) < 9))
			{
				m_iAdminLevelSummonGuild = atoi(token);
			}
			else{
				m_iAdminLevelSummonGuild = 3;
			}
			cReadMode = 0;
			break;
//-----------------------------------------------------------------
		  case 42:
			if ((strlen(token) > 0) && (strlen(token) < 9))
			{
				m_iAdminLevelCheckStatus = atoi(token);
			}
			else{
				m_iAdminLevelCheckStatus = 1;
			}
			cReadMode = 0;
			break;
//-----------------------------------------------------------------
		  case 43:
			if ((strlen(token) > 0) && (strlen(token) < 9))
			{
				m_iAdminLevelCleanMap = atoi(token);
			}
			else{
				m_iAdminLevelCleanMap = 1;
			}
			cReadMode = 0;
			break;
//-----------------------------------------------------------------
			}
		  }
		  else {
			if (memcmp(token, "Admin-Level-/kill", 17) == 0)						cReadMode = 1;
			if (memcmp(token, "Admin-Level-/revive", 19) == 0)						cReadMode = 2;
			if (memcmp(token, "Admin-Level-/closecon", 21) == 0)					cReadMode = 3;
			if (memcmp(token, "Admin-Level-/checkrep", 21) == 0)					cReadMode = 4;
			if (memcmp(token, "Admin-Level-/who", 16) == 0)							cReadMode = 5;
			if (memcmp(token, "Admin-Level-/energysphere", 25) == 0)				cReadMode = 6;
			if (memcmp(token, "Admin-Level-/shutdownthisserverrightnow", 39) == 0)	cReadMode = 7;
			if (memcmp(token, "Admin-Level-/setobservermode", 28) == 0)				cReadMode = 8;
			if (memcmp(token, "Admin-Level-/shutup", 19) == 0)						cReadMode = 9;
			if (memcmp(token, "Admin-Level-/attack", 19) == 0)						cReadMode = 10;
			if (memcmp(token, "Admin-Level-/summondemon", 24) == 0)					cReadMode = 11;
			if (memcmp(token, "Admin-Level-/summondeath", 24) == 0)					cReadMode = 12;
			if (memcmp(token, "Admin-Level-/reservefightzone", 28) == 0)			cReadMode = 13;
			if (memcmp(token, "Admin-Level-/createfish", 23) == 0)					cReadMode = 14;
			if (memcmp(token, "Admin-Level-/teleport", 21) == 0)					cReadMode = 15;
			if (memcmp(token, "Admin-Level-/checkip", 20) == 0)						cReadMode = 16;
			if (memcmp(token, "Admin-Level-/polymorph", 22) == 0)					cReadMode = 17;
			if (memcmp(token, "Admin-Level-/setinvi", 20) == 0)						cReadMode = 18;
			if (memcmp(token, "Admin-Level-/setzerk", 20) == 0)						cReadMode = 19;
			if (memcmp(token, "Admin-Level-/setfreeze", 22) == 0)					cReadMode = 20;
			if (memcmp(token, "Admin-Level-/gns", 16) == 0)							cReadMode = 21;
			if (memcmp(token, "Admin-Level-/setattackmode", 26) == 0)				cReadMode = 22;
			if (memcmp(token, "Admin-Level-/unsummonall", 24) == 0)					cReadMode = 23;
			if (memcmp(token, "Admin-Level-/unsummondemon", 26) == 0)				cReadMode = 24;
			if (memcmp(token, "Admin-Level-/summonnpc", 22) == 0)					cReadMode = 25;
			if (memcmp(token, "Admin-Level-/summonall", 22) == 0)					cReadMode = 26;
			if (memcmp(token, "Admin-Level-/summonplayer", 25) == 0)				cReadMode = 27;
			if (memcmp(token, "Admin-Level-/disconnectall", 26) == 0)				cReadMode = 28;
			if (memcmp(token, "Admin-Level-/enableadmincreateitem", 34) == 0)		cReadMode = 29;
			if (memcmp(token, "Admin-Level-/createitem", 23) == 0)					cReadMode = 30;
			if (memcmp(token, "Admin-Level-/storm", 18) == 0)						cReadMode = 31;
			if (memcmp(token, "Admin-Level-/weather", 20) == 0)						cReadMode = 32;
			if (memcmp(token, "Admin-Level-/setstatus", 22) == 0)					cReadMode = 33;
			if (memcmp(token, "Admin-Level-/goto", 17) == 0)						cReadMode = 34;
			if (memcmp(token, "Admin-Level-/monstercount", 17) == 0)				cReadMode = 35;
			if (memcmp(token, "Admin-Level-/setforcerecalltime", 23) == 0)			cReadMode = 36;
			if (memcmp(token, "Admin-Level-/unsummonboss", 25) == 0)				cReadMode = 37;
			if (memcmp(token, "Admin-Level-/clearnpc", 21) == 0)					cReadMode = 38;
			if (memcmp(token, "Admin-Level-/time", 17) == 0)						cReadMode = 39;
			if (memcmp(token, "Admin-Level-/send", 17) == 0)						cReadMode = 40;
			if (memcmp(token, "Admin-Level-/summonguild", 24) == 0)					cReadMode = 41;
			if (memcmp(token, "Admin-Level-/checkstatus", 24) == 0)					cReadMode = 42;	
			if (memcmp(token, "Admin-Level-/clearmap", 21) == 0)					cReadMode = 43;	
		  }

		  token = pStrTok->pGet();
		 }
		 delete pStrTok;
		 delete[] cp;

		 fclose(pFile);

		 return true;
		}

		return false;
}

void CGame::AdminOrder_CheckRep(int iClientH, char *pData,UINT32 dwMsgSize)
{
 class  CStrTok * pStrTok;
 short m_iRating;
 char   * token, cName[11], cTargetName[11], cRepMessage[256], cTemp[256], seps[] = "= \t\n", cBuff[256];
 int i;

	if (m_pClientList[iClientH] == 0) return;
	ZeroMemory(cTemp, sizeof(cTemp));
	ZeroMemory(cRepMessage, sizeof(cRepMessage));
	if (m_pClientList[iClientH]->m_iAdminUserLevel < 1) {
		wsprintf(cRepMessage, " You have %d reputation points.", m_pClientList[iClientH]->m_iRating);
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, cRepMessage);
	} 
	else {
		if ((dwMsgSize)	<= 0) return;
		ZeroMemory(cTargetName, sizeof(cTargetName));
		ZeroMemory(cBuff, sizeof(cBuff));
		memcpy(cBuff, pData, dwMsgSize);

		pStrTok = new class CStrTok(cBuff, seps);
		token = pStrTok->pGet();

		token = pStrTok->pGet();
		
		if (token != 0) {
			ZeroMemory(cName, sizeof(cName));
			strcpy(cName, token);
		} 
		else {
			ZeroMemory(cName, sizeof(cName));
			strcpy(cName, "null");
		}

		token = pStrTok->pGet();
		
		if (token != 0) {
			m_iRating = atoi(token);
		} 
		
		if (token == 0) { 
			token = "null"; 
		}
		
			if (cName != 0) {
			token = cName;

				if (strlen(token) > 10) 
  					memcpy(cTargetName, token, 10);
				else memcpy(cTargetName, token, strlen(token));

				for (i = 1; i < DEF_MAXCLIENTS; i++) 
  					if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cCharName, cTargetName, 10) == 0)) {
						wsprintf(cRepMessage, " %s has %d reputation points.", m_pClientList[i]->m_cCharName, m_pClientList[i]->m_iRating);
						SendNotifyMsg(iClientH, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, cRepMessage);
					}
			}
	}
}

void CGame::AdminOrder_Pushplayer(int iClientH, char * pData, UINT32 dwMsgSize)
{
//Push Player- By:ACiDx  Monday , September 2004
	//Teleports a Defined Player to Defined Destination
	// /push playername mapname sX dX
 char   seps[] = "= \t\n";
 char   * token, cBuff[256], cMapName[11], cTargetName[11];
 class  CStrTok * pStrTok;
 int dX, dY;
  int i;
 bool   bFlag = false;
	ZeroMemory(cTargetName, sizeof(cTargetName));
	ZeroMemory(cBuff, sizeof(cBuff));
	ZeroMemory(cMapName, sizeof(cMapName));
	dX = dY = -1;

	if (m_pClientList[iClientH] == 0) return;
	if ((dwMsgSize)	<= 0) return;

	if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelPushPlayer) {
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
		return;
	}

	ZeroMemory(cBuff, sizeof(cBuff));
	memcpy(cBuff, pData, dwMsgSize);

	pStrTok = new class CStrTok(cBuff, seps);
	token = pStrTok->pGet();
	token = pStrTok->pGet();
	
	if (token != 0) {
		if (strlen(token) > 10) return;
		else strcpy(cTargetName, token);
	}
	else return;

	token = pStrTok->pGet();
	if (token != 0) {
		if (strlen(token) > 10) return;
		else strcpy(cMapName, token);
	}
	else return;

	token = pStrTok->pGet();
	if (token != 0) {
		dX = atoi(token);
	}
	else dX = -1;

	token = pStrTok->pGet();
	if (token != 0) {
		dY = atoi(token);
	}
	else dY = -1;

	// centuu
	for (int m = 0; m < DEF_MAXMAPS; m++) { //Enum all maps
		if (m_pMapList[m] != 0) {	//Is allocated map
			if (memcmp(m_pMapList[m]->m_cName, cMapName, 10) == 0) {
				bFlag = true;
				break;
			}
		}
	}

	
	for (i = 0; i < DEF_MAXCLIENTS; i++) {
		if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cCharName, cTargetName, 10) == 0)) {

			//Defines I as Max clients
			if (bFlag)
				RequestTeleportHandler(i, "2   ", cMapName, dX, dY);
			else ShowClientMsg(iClientH, "The map is currently offline.");
			
			break;
		}
	}
}


bool CGame::bReadAdminListConfigFile(char *pFn)
{
 FILE * pFile;
 HANDLE hFile;
 UINT32  dwFileSize;
 char * cp, * token, cReadModeA, cReadModeB;
 char seps[] = "= \t\n";
 class CStrTok * pStrTok;
 int   iIndex;
 short len;

	cReadModeA = 0;
	cReadModeB = 0;
	iIndex = 0;

	hFile = CreateFile(pFn, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
	dwFileSize = GetFileSize(hFile, 0);
	if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);

	pFile = fopen(pFn, "rt");
	if (pFile == 0) {
		// °ÔÀÓ¼­¹öÀÇ ÃÊ±âÈ­ ÆÄÀÏÀ» ÀÐÀ» ¼ö ¾ø´Ù.
		PutLogList("(!) Cannot open AdminList.cfg file.");
		return false;
	}
	else {
		PutLogList("(!) Reading AdminList.cfg...");
		cp = new char[dwFileSize+2];
		ZeroMemory(cp, dwFileSize+2);
		fread(cp, dwFileSize, 1, pFile);

		pStrTok = new class CStrTok(cp, seps);
		token = pStrTok->pGet();

		while( token != 0 )   {
			if (cReadModeA != 0) {
				switch (cReadModeA) {
				case 1:
						if (iIndex >= DEF_MAXADMINS) {
							PutLogList("(!) WARNING! Too many GMs on the AdminList.cfg!"); 
							return true;
						}
						len = (short)strlen(token);
						if(len > 10) len = 10;
						ZeroMemory(m_stAdminList[iIndex].m_cGMName, sizeof(m_stAdminList[iIndex].m_cGMName));
						memcpy(m_stAdminList[iIndex].m_cGMName, token, len);
						iIndex++;
						cReadModeA = 0;
						cReadModeB = 0;
						break;
				}
			}
			else {
				if (memcmp(token, "verified-admin", 14) == 0) {
					cReadModeA = 1;
					cReadModeB = 1;
				}
			}
			
			token = pStrTok->pGet();
		}
		delete pStrTok;
		delete[] cp;

		fclose(pFile);
	}

	return true;
}

bool CGame::bReadBannedListConfigFile(char *pFn)
{
 FILE * pFile;
 HANDLE hFile;
 UINT32  dwFileSize;
 char * cp, * token, cReadModeA, cReadModeB;
 char seps[] = "= \t\n";
 class CStrTok * pStrTok;
 int   iIndex;
 short len;

	cReadModeA = 0;
	cReadModeB = 0;
	iIndex = 0;

	hFile = CreateFile(pFn, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
	dwFileSize = GetFileSize(hFile, 0);
	if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);

	pFile = fopen(pFn, "rt");
	if (pFile == 0) {
		// °ÔÀÓ¼­¹öÀÇ ÃÊ±âÈ­ ÆÄÀÏÀ» ÀÐÀ» ¼ö ¾ø´Ù.
		PutLogList("(!) Cannot open BannedList.cfg file.");
		return false;
	}
	else {
		PutLogList("(!) Reading BannedList.cfg...");
		cp = new char[dwFileSize+2];
		ZeroMemory(cp, dwFileSize+2);
		fread(cp, dwFileSize, 1, pFile);

		pStrTok = new class CStrTok(cp, seps);
		token = pStrTok->pGet();

		while( token != 0 )   {
			if (cReadModeA != 0) {
				switch (cReadModeA) {
				case 1:
						if (iIndex >= DEF_MAXBANNED) {
							PutLogList("(!) WARNING! Too many banned on the BannedList.cfg!"); 
							return true;
						}
						len = (short)strlen(token);
						if(len > 21) len = 21;
						ZeroMemory(m_stBannedList[iIndex].m_cBannedIPaddress, sizeof(m_stBannedList[iIndex].m_cBannedIPaddress));
						memcpy(m_stBannedList[iIndex].m_cBannedIPaddress, token, len);
						iIndex++;
						cReadModeA = 0;
						cReadModeB = 0;
						break;
				}
			}
			else {
				if (memcmp(token, "banned-ip", 9) == 0) {
					cReadModeA = 1;
					cReadModeB = 1;
				}
			}
			
			token = pStrTok->pGet();
		}
		delete pStrTok;
		delete[] cp;

		fclose(pFile);
	}

	return true;
}

void CGame::AdminOrder_CleanMap(int iClientH, char * pData, UINT32 dwMsgSize)
{
	char   seps[] = "= \t\n";
	class  CStrTok * pStrTok;
	char   * token, cMapName[11], cBuff[256];
	bool bFlag = false;	//Used to check if we are on the map we wanna clear
	int i;
	CItem *pItem;
	short dX, dY, sRemainItemID, len;
	char cRemainItemColor;
	UINT32 dwRemainItemAttr;

	if (m_pClientList[iClientH] == 0) return;
	if ((dwMsgSize)	<= 0) return;

	if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelCleanMap) {
		// Admin user levelÀÌ ³·¾Æ¼­ ÀÌ ±â´ÉÀ» »ç¿ëÇÒ ¼ö ¾ø´Ù.
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
		return;
	}

	ZeroMemory(cBuff, sizeof(cBuff));
	memcpy(cBuff, pData, dwMsgSize);

	pStrTok = new class CStrTok(cBuff, seps);
	token = pStrTok->pGet();
	token = pStrTok->pGet();

	if (token != 0) {
		ZeroMemory(cMapName, sizeof(cMapName));
		strcpy(cMapName, token);

		for (i = 0; i < DEF_MAXMAPS; i++)	//Enum all maps
			if (m_pMapList[i] != 0) {	//Is allocated map
				if (memcmp(m_pMapList[i]->m_cName, cMapName, 10) == 0) {	//is map same name
					bFlag = true; //Set flag
					//Get X and Y coords
					int m_x = m_pMapList[i]->m_sSizeX;
					int m_y = m_pMapList[i]->m_sSizeY;
					for(int j = 1; j < m_x; j++)
						for(int k = 1; k < m_y; k++){
							do {	//Delete all items on current tile
								pItem = m_pMapList[i]->pGetItem(j, k, &sRemainItemID, &cRemainItemColor, &dwRemainItemAttr);

								if (pItem != 0) {
									delete pItem;
								}
							} while(pItem != 0);
						}
					break;	//Break outo f loop
				}
			}

		if (bFlag) 
		{	//Notify GM that all items have been cleared
			for(int i = 1; i < DEF_MAXCLIENTS; i++){
				if (m_pClientList[i] != 0) {
				len = (short)strlen(cMapName);
				if(len > 10) len = 10;
				if (memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, cMapName, len) != 0) return;
				dX = m_pClientList[i]->m_sX;
				dY = m_pClientList[i]->m_sY;
				ZeroMemory(cMapName,sizeof(cMapName));
				strcpy(cMapName, m_pClientList[i]->m_cMapName);
				RequestTeleportHandler(i,"2   ", cMapName, dX, dY);
				}
			}
		}
	}

	delete pStrTok;
}

void CGame::ShowClientMsg(int iClientH, char* pMsg)
{
	char * cp, cTemp[256];
	UINT32 * dwp, dwMsgSize;
	UINT16 * wp;
	short * sp;

	ZeroMemory(cTemp, sizeof(cTemp));

	dwp = (UINT32 *)cTemp;
	*dwp = MSGID_COMMAND_CHATMSG;

	wp  = (UINT16 *)(cTemp + DEF_INDEX2_MSGTYPE);
	*wp = 0;

	cp  = (char *)(cTemp + DEF_INDEX2_MSGTYPE + 2);
	sp  = (short *)cp;
	*sp = 0;
	cp += 2;

	sp  = (short *)cp;
	*sp = 0;
	cp += 2;

	memcpy(cp, "[BOT]", 5); // Player name :P
	cp += 10;

	*cp = 10; // chat type
	cp++;

	dwMsgSize = strlen(pMsg);
	if(dwMsgSize > 50) dwMsgSize = 50;
	memcpy(cp, pMsg, dwMsgSize);
	cp += dwMsgSize;

	m_pClientList[iClientH]->m_pXSock->iSendMsg(cTemp, dwMsgSize + 22);
}

void CGame::GlobalUpdateConfigs(char cConfigType)
{
 char * cp, cData[120];

	ZeroMemory(cData, sizeof(cData));
	cp = (char *)cData;
	*cp = GSM_UPDATECONFIGS;
	cp++;

	*cp = (char)cConfigType;
	cp++;			

	LocalUpdateConfigs(cConfigType);

	bStockMsgToGateServer(cData, 5);
}

void CGame::LocalUpdateConfigs(char cConfigType)
{
	if (cConfigType == 1) {
		bReadSettingsConfigFile("GameConfigs\\Settings.cfg");
		PutLogList("(!!!) Settings.cfg updated successfully!");
	}
	else if (cConfigType == 2) {
		bReadAdminListConfigFile("GameConfigs\\AdminList.cfg");
		PutLogList("(!!!) AdminList.cfg updated successfully!");
	}
	else if (cConfigType == 3) {
		bReadBannedListConfigFile("GameConfigs\\BannedList.cfg");
		PutLogList("(!!!) BannedList.cfg updated successfully!");
	}
	else if (cConfigType == 4) {
		bReadAdminSetConfigFile("GameConfigs\\AdminSettings.cfg");
		PutLogList("(!!!) AdminSettings.cfg updated successfully!");
	}
}

void CGame::TimeHitPointsUp(int iClientH)
{
 int iMaxHP, iTemp, iTotal;
 float dV1, dV2, dV3;

	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
	if (m_pClientList[iClientH]->m_iHungerStatus <= 0) return;
	if (m_pClientList[iClientH]->m_bIsKilled == true) return;
	if (m_pClientList[iClientH]->m_bSkillUsingStatus[19] == true) return;
	if ((m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bMapRegens == false) && (m_pClientList[iClientH]->m_iAdminUserLevel == 0)) {
		return;
	}
	
	iMaxHP = iGetMaxHP(iClientH);

	if (m_pClientList[iClientH]->m_iHP < iMaxHP) {
		
		iTemp = iDice(1, (m_pClientList[iClientH]->m_iVit));

		if (iTemp < (m_pClientList[iClientH]->m_iVit/2)) iTemp = (m_pClientList[iClientH]->m_iVit/2);

		if (m_pClientList[iClientH]->m_iSideEffect_MaxHPdown != 0)

			iTemp -= (iTemp / m_pClientList[iClientH]->m_iSideEffect_MaxHPdown);

		iTotal = iTemp + m_pClientList[iClientH]->m_iHPstock;

		if (m_pClientList[iClientH]->m_iAddHP != 0) {
			dV2 = (float)iTotal;
			dV3 = (float)m_pClientList[iClientH]->m_iAddHP;
			dV1 = (dV3 / 100.0f)*dV2;
			iTotal += (int)dV1;
		}

		m_pClientList[iClientH]->m_iHP += iTotal;

		if (m_pClientList[iClientH]->m_iHP > iMaxHP) m_pClientList[iClientH]->m_iHP = iMaxHP;

		if (m_pClientList[iClientH]->m_iHP <= 0) m_pClientList[iClientH]->m_iHP = 0;
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_HP, 0, 0, 0, 0);
	}

	m_pClientList[iClientH]->m_iHPstock = 0;
}

int CGame::iCalculateAttackEffect(short sTargetH, char cTargetType, short sAttackerH, char cAttackerType, int tdX, int tdY, int iAttackMode, bool bNearAttack, bool bIsDash, bool bArrowUse, bool bMainGaucheAttack)
{
 int    iAP_SM, iAP_L, iAttackerHitRatio, iTargetDefenseRatio, iDestHitRatio, iResult,iExtraDmg;
 char   cAttackerName[21], cAttackerDir, cAttackerSide, cTargetDir, cProtect, cCropSkill, cFarmingSkill, cDamageMod[256];
 short  sWeaponIndex, sAttackerWeapon, dX, dY, sX, sY, sAtkX, sAtkY, sTgtX, sTgtY;
 UINT32  dwTime;
 UINT16   wWeaponType;
 float dTmp1, dTmp2, dTmp3;
 bool   bKilled;
 bool   bNormalMissileAttack;
 bool   bIsAttackerBerserk;
 int    iKilledDice, iDamage, iWepLifeOff, iSideCondition, iMaxSuperAttack, iWeaponSkill, iComboBonus, iTemp;
 int    iAttackerHP, iMoveDamage, iRepDamage;
 char   cAttackerSA;
 int    iAttackerSAvalue, iHitPoint, iFarmingSSN;
 char   cDamageMoveDir;
 int    iPartyID, iConstructionPoint, iWarContribution, tX, tY, iDst1, iDst2;
 short	sItemIndex;
 short	sSkillUsed;
 UINT32 iExp;

	dwTime = timeGetTime();
	bKilled = false;
	iExp = 0;
	iPartyID = 0;
	bNormalMissileAttack = false;
	ZeroMemory(cAttackerName, sizeof(cAttackerName));
	cAttackerSA      = 0;
	iAttackerSAvalue = 0;
	wWeaponType      = 0;


	switch (cAttackerType) {
	case DEF_OWNERTYPE_PLAYER:

		if (m_pClientList[sAttackerH] == 0) return 0;
		//LifeX Fix Admin Hit NPC
		if ((m_bAdminSecurity == true) && (m_pClientList[sAttackerH]->m_iAdminUserLevel > 0 && m_pClientList[sAttackerH]->m_iAdminUserLevel < 7))
		{
			return 0;
		}
		if (m_pClientList[sAttackerH]->IsInsideTeam())
		{
			if (m_pClientList[sAttackerH]->iteam == m_pClientList[sTargetH]->iteam) return 0;
		}
		if ((m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_bIsAttackEnabled == false) && (m_pClientList[sAttackerH]->m_iAdminUserLevel == 0)) return 0;
		
		if (   (m_bIsHeldenianMode == true)				
			&& (m_bHeldenianWarInitiated == false)			// No attack work before war has begun
			&& (m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex] != 0) 
			&& (m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_bIsHeldenianMap == 1)) return 0; 

		if (cAttackerType == DEF_OWNERTYPE_NPC) 
		{	if (   ( m_pClientList[sTargetH]->m_bSkillUsingStatus[22] == true )
				&& (iDice(1,100) <= m_pClientList[sTargetH]->m_cSkillMastery[22]) )
				return 0;// SNOOPY, do not attack while Taming in progress
		}

		if ((m_bIsCrusadeMode == false) && (m_pClientList[sAttackerH]->m_bIsPlayerCivil == true) && (cTargetType == DEF_OWNERTYPE_PLAYER)) return 0;

		if ((m_pClientList[sAttackerH]->m_iStatus & 0x10) != 0) {
			SetInvisibilityFlag(sAttackerH, DEF_OWNERTYPE_PLAYER, false);
			bRemoveFromDelayEventList(sAttackerH, DEF_OWNERTYPE_PLAYER, DEF_MAGICTYPE_INVISIBILITY);
			m_pClientList[sAttackerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY] = 0;
		}
		
		if ((m_pClientList[sAttackerH]->m_sAppr2 & 0xF000) == 0) return 0;

		//Magn0S:: Battle Staffs coded by Magn0S
		if (m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND] != -1) {
			sWeaponIndex = m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND];
		}
		else if (m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND] != -1) {
			sWeaponIndex = m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND];
		}
		else sWeaponIndex = -1;

		if ((sWeaponIndex != -1) && (m_pClientList[sAttackerH]->m_pItemList[sWeaponIndex] != 0)) {
			if (m_pClientList[sAttackerH]->m_pItemList[sWeaponIndex]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ATTACK_MAGICITEM)
				return 0;
		}

		iAP_SM = 0;
		iAP_L  = 0;

		wWeaponType = ((m_pClientList[sAttackerH]->m_sAppr2 & 0x0FF0) >> 4);

		sSkillUsed = m_pClientList[sAttackerH]->m_sUsingWeaponSkill;
		if ((bIsDash == true) && (m_pClientList[sAttackerH]->m_cSkillMastery[sSkillUsed] != 100) && (wWeaponType != 25) && (wWeaponType != 27)) {
			wsprintf(G_cTxt, "TSearch Fullswing Hack: (%s) Player: (%s) - dashing with only (%d) weapon skill.", m_pClientList[sAttackerH]->m_cIPaddress, m_pClientList[sAttackerH]->m_cCharName, m_pClientList[sAttackerH]->m_cSkillMastery[sSkillUsed]);
			PutHackLogFileList(G_cTxt);

			return 0;
		}

		cAttackerSide = m_pClientList[sAttackerH]->m_cSide;

			if (wWeaponType == 0) {
				iAP_SM = iAP_L = iDice(1, ((m_pClientList[sAttackerH]->m_iStr) / 12));
				if (iAP_SM <= 0) iAP_SM = 1;
				if (iAP_L <= 0) iAP_L = 1;
				iAttackerHitRatio = m_pClientList[sAttackerH]->m_iHitRatio + m_pClientList[sAttackerH]->m_cSkillMastery[5];
				m_pClientList[sAttackerH]->m_sUsingWeaponSkill = 5;

			}
			else if (((wWeaponType >= 1) && (wWeaponType < 40)) || ((wWeaponType >= 55) && (wWeaponType < 60))) {
				iAP_SM = iDice(m_pClientList[sAttackerH]->m_cAttackDiceThrow_SM, m_pClientList[sAttackerH]->m_cAttackDiceRange_SM);
				iAP_L = iDice(m_pClientList[sAttackerH]->m_cAttackDiceThrow_L, m_pClientList[sAttackerH]->m_cAttackDiceRange_L);

				iAP_SM += m_pClientList[sAttackerH]->m_cAttackBonus_SM;
				iAP_L += m_pClientList[sAttackerH]->m_cAttackBonus_L;

				iAttackerHitRatio = m_pClientList[sAttackerH]->m_iHitRatio;

				dTmp1 = (float)iAP_SM;
				if ((m_pClientList[sAttackerH]->m_iStr) <= 0)
					dTmp2 = 1.0f;
				else dTmp2 = (float)(m_pClientList[sAttackerH]->m_iStr);

				dTmp2 = dTmp2 / 5.0f;
				dTmp3 = dTmp1 + (dTmp1 * (dTmp2 / 100.0f));
				iAP_SM = (int)(dTmp3 + 0.5f);

				dTmp1 = (float)iAP_L;
				if ((m_pClientList[sAttackerH]->m_iStr) <= 0)
					dTmp2 = 1.0f;
				else dTmp2 = (float)(m_pClientList[sAttackerH]->m_iStr);

				dTmp2 = dTmp2 / 5.0f;
				dTmp3 = dTmp1 + (dTmp1 * (dTmp2 / 100.0f));
				iAP_L = (int)(dTmp3 + 0.5f);
			}
			else if ((wWeaponType >= 40) && (wWeaponType < 55)) { // Centu : Archer
				iAP_SM = iDice(m_pClientList[sAttackerH]->m_cAttackDiceThrow_SM, m_pClientList[sAttackerH]->m_cAttackDiceRange_SM);
				iAP_L = iDice(m_pClientList[sAttackerH]->m_cAttackDiceThrow_L, m_pClientList[sAttackerH]->m_cAttackDiceRange_L);

				iAP_SM += m_pClientList[sAttackerH]->m_cAttackBonus_SM;
				iAP_L += m_pClientList[sAttackerH]->m_cAttackBonus_L;

				iAttackerHitRatio = m_pClientList[sAttackerH]->m_iHitRatio;
				bNormalMissileAttack = true;

				dTmp1 = (float)iAP_SM;
				if (m_pClientList[sAttackerH]->m_iCharisma <= 0)	dTmp2 = 1.0f;
				else dTmp2 = (float)m_pClientList[sAttackerH]->m_iCharisma;

				dTmp2 = dTmp2 / 5.0f;
				dTmp3 = dTmp1 + (dTmp1 * (dTmp2 / 100.0f));
				iAP_SM = (int)(dTmp3 + 0.5f);

				dTmp1 = (float)iAP_L;
				if (m_pClientList[sAttackerH]->m_iCharisma <= 0)	dTmp2 = 1.0f;
				else dTmp2 = (float)m_pClientList[sAttackerH]->m_iCharisma;

				dTmp2 = dTmp2 / 5.0f;
				dTmp3 = dTmp1 + (dTmp1 * (dTmp2 / 100.0f));
				iAP_L = (int)(dTmp3 + 0.5f);

				iAP_SM += iDice(1, (m_pClientList[sAttackerH]->m_iCharisma / 10));
				iAP_L += iDice(1, (m_pClientList[sAttackerH]->m_iCharisma / 10));

			}

			iAttackerHitRatio += 50;
			if (iAP_SM <= 0) iAP_SM = 1;
			if (iAP_L <= 0) iAP_L = 1;

			if (m_pClientList[sAttackerH]->m_iCustomItemValue_Attack != 0) {
				if ((m_pClientList[sAttackerH]->m_iMinAP_SM != 0) && (iAP_SM < m_pClientList[sAttackerH]->m_iMinAP_SM)) {
					iAP_SM = m_pClientList[sAttackerH]->m_iMinAP_SM;
				}
				if ((m_pClientList[sAttackerH]->m_iMinAP_L != 0) && (iAP_L < m_pClientList[sAttackerH]->m_iMinAP_L)) {
					iAP_L = m_pClientList[sAttackerH]->m_iMinAP_L;
				}
				if ((m_pClientList[sAttackerH]->m_iMaxAP_SM != 0) && (iAP_SM > m_pClientList[sAttackerH]->m_iMaxAP_SM)) {
					iAP_SM = m_pClientList[sAttackerH]->m_iMaxAP_SM;
				}
				if ((m_pClientList[sAttackerH]->m_iMaxAP_L != 0) && (iAP_L > m_pClientList[sAttackerH]->m_iMaxAP_L)) {
					iAP_L = m_pClientList[sAttackerH]->m_iMaxAP_L;
				}
			}

			if (m_pClientList[sAttackerH]->m_cHeroArmourBonus > 0) 
			{
				iAttackerHitRatio += 20;
				if (m_pClientList[sAttackerH]->m_cHeroArmourBonus > 8)
				{
					iAttackerHitRatio += 10;
				}
				iAP_SM += m_pClientList[sAttackerH]->m_cHeroArmourBonus;
				iAP_L += m_pClientList[sAttackerH]->m_cHeroArmourBonus;
			}

			sItemIndex = m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND];
			if ((sItemIndex != -1) && (m_pClientList[sAttackerH]->m_pItemList[sItemIndex] != 0)) {
				if ((m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 851) || // KlonessEsterk 
					(m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 863) || // KlonessWand(MS.20)
					(m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 864)) { // KlonessWand(MS.10) 
					if (m_pClientList[sAttackerH]->m_iRating > 0) {
						iRepDamage = m_pClientList[sAttackerH]->m_iRating / 100;
						iAP_SM += iRepDamage;
						iAP_L += iRepDamage;
					}
					if (cTargetType == DEF_OWNERTYPE_PLAYER) {
						if (m_pClientList[sTargetH] == 0) return 0;
						if (m_pClientList[sTargetH]->m_iRating < 0) {
							iRepDamage = (abs(m_pClientList[sTargetH]->m_iRating) / 100);
							iAP_SM += iRepDamage;
							iAP_L += iRepDamage;
						}
					}
				}
				if ((m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 861) || // BerserkWand(MS.20)
					(m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 862)) { // BerserkWand(MS.10)
					float damageTemp = (float)iAttackerHitRatio;
					damageTemp *= 1.5f; // O el valor con punto flotante que ustedes dispongan... JustThink
					iAttackerHitRatio = (int)damageTemp;
					iAP_SM++;
					iAP_L++;
				}
				if ((m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 865) || // BerserkWand(MS.20)
					(m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 866)) { // BerserkWand(MS.10)
					float damageTemp = (float)iAttackerHitRatio;
					damageTemp *= 0.5f; // O el valor con punto flotante que ustedes dispongan... JustThink
					iAttackerHitRatio = (int)damageTemp;
					iAP_SM++;
					iAP_L++;
				}
				if (m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 988) { // BerserkWand(MS.10)
					float damageTemp = (float)iAttackerHitRatio;
					damageTemp *= 2.0f; // O el valor con punto flotante que ustedes dispongan... JustThink
					iAttackerHitRatio = (int)damageTemp;
					iAP_SM++;
					iAP_L++;
				}
				if (m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 1005) { // BerserkWand(MS.10)
					float damageTemp = (float)iAttackerHitRatio;
					damageTemp *= 2.1f; // O el valor con punto flotante que ustedes dispongan... JustThink
					iAttackerHitRatio = (int)damageTemp;
					iAP_SM++;
					iAP_L++;
				}

			}

			sItemIndex = m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND];
			if ((sItemIndex != -1) && (m_pClientList[sAttackerH]->m_pItemList[sItemIndex] != 0)) {

				if ((m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 616) // DemonSlayer	
					&& (cTargetType == DEF_OWNERTYPE_NPC))
				{
					if (m_pNpcList[sTargetH]->m_sType == 31 || m_pNpcList[sTargetH]->m_sType == 93 || m_pNpcList[sTargetH]->m_sType == 52)
					{
						iAttackerHitRatio += 100;
						iAP_L += iDice(1, ((2 * iAP_L) / 3));
					}
				}

				else if ((m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 847) &&
					(m_cDayOrNight == 2)) {
					iAP_SM += 4;
					iAP_L += 4;
				}
				else if ((m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 848) &&
					(m_cDayOrNight == 1)) {
					iAP_SM += 4;
					iAP_L += 4;
				}

				//Magn0S:: Add Staminar & Mana Destruction Sword
				//SP Drain
				else if ((m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 295) &&
					(cTargetType == DEF_OWNERTYPE_PLAYER)) {
					if (m_pClientList[sTargetH]->m_iSP > 0) {
						m_pClientList[sTargetH]->m_iSP -= iAP_L / 20;
						SendNotifyMsg(0, sTargetH, DEF_NOTIFY_SP, 0, 0, 0, 0);
						if (m_pClientList[sTargetH]->m_iSP <= 0)
							m_pClientList[sTargetH]->m_iSP = 0;
					}
				}
				//MP Drain
				else if ((m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 296) &&
					(cTargetType == DEF_OWNERTYPE_PLAYER)) {
					if (m_pClientList[sTargetH]->m_iMP > 0) {
						m_pClientList[sTargetH]->m_iMP -= iAP_L / 20;
						SendNotifyMsg(0, sTargetH, DEF_NOTIFY_MP, 0, 0, 0, 0);
						if (m_pClientList[sTargetH]->m_iMP <= 0)
							m_pClientList[sTargetH]->m_iMP = 0;
					}
				}
				//--------------------------------------------------------------------------------------------
				else if ((m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 849) || // KlonessBlade 
					(m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 850) || // KlonessAxe
					(m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 854)) { // KlonessHammer
					if (m_pClientList[sAttackerH]->m_iRating > 0) {
						iRepDamage = m_pClientList[sAttackerH]->m_iRating / 100;
						iAP_SM += iRepDamage;
						iAP_L += iRepDamage;
					}
					if (cTargetType == DEF_OWNERTYPE_PLAYER) {
						if (m_pClientList[sTargetH] == 0) return 0;
						if (m_pClientList[sTargetH]->m_iRating < 0) {
							iRepDamage = (abs(m_pClientList[sTargetH]->m_iRating) / 100);
							iAP_SM += iRepDamage;
							iAP_L += iRepDamage;
						}
					}
				}
				
				else if (m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 983 || // HuntSword
					m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 1050) // HuntBow	
				{
					if (cTargetType == DEF_OWNERTYPE_NPC) {
						if (m_pNpcList[sTargetH]->m_sType == 49 || m_pNpcList[sTargetH]->m_sType == 50 || m_pNpcList[sTargetH]->m_sType == 66 ||
							m_pNpcList[sTargetH]->m_sType == 73 || m_pNpcList[sTargetH]->m_sType == 81 || m_pNpcList[sTargetH]->m_sType == 99 ||
							m_pNpcList[sTargetH]->m_sType == 92 || m_pNpcList[sTargetH]->m_sType == 21)
						{
							iAP_SM = 1;
							iAP_L = 1;
						}
						else
						{
							iAP_SM = 1000;
							iAP_L = 1000;
						}
					}
					else if (cTargetType == DEF_OWNERTYPE_PLAYER) {
						iAP_SM = 1;
						iAP_L = 1;
					}
				}
			}

			sItemIndex = m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_NECK];
			if ((sItemIndex != -1) && (m_pClientList[sAttackerH]->m_pItemList[sItemIndex] != 0)) {
				if (m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 859) { // NecklaceOfKloness  
					if (cTargetType == DEF_OWNERTYPE_PLAYER) {
						if (m_pClientList[sTargetH] == 0) return 0;
						if (m_pClientList[sTargetH]->m_iRating < 0) {
							iRepDamage = (abs(m_pClientList[sTargetH]->m_iRating) / 200);
							iAP_SM += iRepDamage;
							iAP_L += iRepDamage;
						}
					}
				}
			}

			cAttackerDir = m_pClientList[sAttackerH]->m_cDir;
			strcpy(cAttackerName, m_pClientList[sAttackerH]->m_cCharName);

			if (m_pClientList[sAttackerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_BERSERK] != 0)
				bIsAttackerBerserk = true;
			else bIsAttackerBerserk = false;

			if ((bArrowUse != true) && (m_pClientList[sAttackerH]->m_iSuperAttackLeft > 0) && (iAttackMode >= 20)) {

				dTmp1 = (float)iAP_SM;
				dTmp2 = (float)(m_pClientList[sAttackerH]->m_iLevel);
				dTmp3 = dTmp2 / 100.0f;
				dTmp2 = dTmp1 * dTmp3;
				iTemp = (int)(dTmp2 + 0.5f);
				iAP_SM += iTemp;

				dTmp1 = (float)iAP_L;
				dTmp2 = (float)(m_pClientList[sAttackerH]->m_iLevel);
				dTmp3 = dTmp2 / 100.0f;
				dTmp2 = dTmp1 * dTmp3;
				iTemp = (int)(dTmp2 + 0.5f);
				iAP_L += iTemp;

				switch (m_pClientList[sAttackerH]->m_sUsingWeaponSkill) {
				case 6:  iAP_SM += (iAP_SM / 10); iAP_L += (iAP_L / 10); iAttackerHitRatio += 30; break;
				case 7:  iAP_SM *= 2; iAP_L *= 2; break;
				case 8:  iAP_SM += (iAP_SM / 10); iAP_L += (iAP_L / 10); iAttackerHitRatio += 30; break;
				case 10:  iAP_SM += (iAP_SM / 5); iAP_L += (iAP_L / 5);                           break;
				case 14:  iAP_SM += (iAP_SM / 5); iAP_L += (iAP_L / 5); iAttackerHitRatio += 20; break;
				case 21:  iAP_SM += (iAP_SM / 5); iAP_L += (iAP_L / 5); iAttackerHitRatio += 50; break;
				default: break;

				}

				iAttackerHitRatio += 100 + m_pClientList[sAttackerH]->m_iCustomItemValue_Attack;
			}
			if (bIsDash == true) {
				iAttackerHitRatio += 20;
				switch (m_pClientList[sAttackerH]->m_sUsingWeaponSkill) {
				case 8:  iAP_SM += (iAP_SM / 10); iAP_L += (iAP_L / 10); break;
				case 10: iAP_SM += (iAP_SM / 5); iAP_L += (iAP_L / 5);break;
				case 14: iAP_SM += (iAP_SM / 5); iAP_L += (iAP_L / 5);break;
				default: break;
				}
			}
			iAttackerHP = m_pClientList[sAttackerH]->m_iHP;
			iAttackerHitRatio += m_pClientList[sAttackerH]->m_iAddAR;
			sAtkX = m_pClientList[sAttackerH]->m_sX;
			sAtkY = m_pClientList[sAttackerH]->m_sY;
			iPartyID = m_pClientList[sAttackerH]->m_iPartyID;
		
		break;

	case DEF_OWNERTYPE_NPC:

		if (m_pNpcList[sAttackerH] == 0) return 0;
		if (m_pMapList[m_pNpcList[sAttackerH]->m_cMapIndex]->m_bIsAttackEnabled == false) return 0;

		if ((m_pNpcList[sAttackerH]->m_iStatus & 0x10) != 0) {
			SetInvisibilityFlag(sAttackerH, DEF_OWNERTYPE_NPC, false);
			bRemoveFromDelayEventList(sAttackerH, DEF_OWNERTYPE_NPC, DEF_MAGICTYPE_INVISIBILITY);
			m_pNpcList[sAttackerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY] = 0;
		}

		cAttackerSide = m_pNpcList[sAttackerH]->m_cSide;
		iAP_SM = 0;
		iAP_L  = 0;

		if (m_pNpcList[sAttackerH]->m_cAttackDiceThrow != 0) 
			iAP_L = iAP_SM = iDice(m_pNpcList[sAttackerH]->m_cAttackDiceThrow, m_pNpcList[sAttackerH]->m_cAttackDiceRange);

		iAttackerHitRatio = m_pNpcList[sAttackerH]->m_iHitRatio;

		cAttackerDir = m_pNpcList[sAttackerH]->m_cDir;
		memcpy(cAttackerName, m_pNpcList[sAttackerH]->m_cNpcName, 20);

		if (m_pNpcList[sAttackerH]->m_cMagicEffectStatus[DEF_MAGICTYPE_BERSERK] != 0)
			bIsAttackerBerserk = true;
		else bIsAttackerBerserk = false;

		iAttackerHP = m_pNpcList[sAttackerH]->m_iHP;
		
		if (m_pNpcList[sAttackerH]->m_sType == 99)
			cAttackerSA = 2;
		else
			cAttackerSA = m_pNpcList[sAttackerH]->m_cSpecialAbility;

		sAtkX = m_pNpcList[sAttackerH]->m_sX;
		sAtkY = m_pNpcList[sAttackerH]->m_sY;

		// Check for arrow attack
		switch (m_pNpcList[sAttackerH]->m_sType) {
		case 54: // Dark-Elf				
			// No more "not arrow attack if close" (was buggy)
			bNormalMissileAttack = true; 	
			break;
		}
		break;
	}

	switch (cTargetType) {
	case DEF_OWNERTYPE_PLAYER:

		if (m_pClientList[sTargetH] == 0) return 0;
		if (m_pClientList[sTargetH]->m_bIsKilled == true) return 0;
		
		if ((m_pClientList[sTargetH]->m_iStatus & 0x400000) != 0) return 0;

		if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_bIsCrusadeMode == false) && 
			(m_pClientList[sTargetH]->m_iPKCount == 0) && (m_pClientList[sTargetH]->m_bIsPlayerCivil == true)) return 0;
				
		if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sTargetH]->m_bIsNeutral == true) && 
			(m_pClientList[sTargetH]->m_iPKCount == 0) && (m_pClientList[sTargetH]->m_bIsOwnLocation == true)) return 0;
		
		if ((m_pClientList[sTargetH]->m_sX != tdX) || (m_pClientList[sTargetH]->m_sY != tdY)) return 0;

		if (m_pClientList[sTargetH]->m_iAdminUserLevel > 0) return 0; 

		if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH]->m_bIsNeutral == true) 
			&& (m_pClientList[sTargetH]->m_iPKCount == 0)) return 0;

		if ((m_pClientList[sTargetH]->m_iPartyID != 0) && (iPartyID == m_pClientList[sTargetH]->m_iPartyID)) return 0;

		cTargetDir = m_pClientList[sTargetH]->m_cDir;
		iTargetDefenseRatio = m_pClientList[sTargetH]->m_iDefenseRatio;
		m_pClientList[sTargetH]->m_dwLogoutHackCheck = dwTime;
		if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH]->m_bIsSafeAttackMode == true)) { 
			iSideCondition = iGetPlayerRelationship(sAttackerH, sTargetH);
			if ((iSideCondition == 7) || (iSideCondition == 2) || (iSideCondition == 6)) {
				iAP_SM = iAP_SM / 2;
				iAP_L  = iAP_L / 2;
			}
			else {
				if (m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_bIsFightZone == true) {
					if (m_pClientList[sAttackerH]->m_iGuildGUID == m_pClientList[sTargetH]->m_iGuildGUID) return 0;
					else {
						iAP_SM = iAP_SM / 2;
						iAP_L  = iAP_L / 2;
					}
				}
				else return 0;
			}
			//Magn0S:: Put self safe for bmages.
			if ((m_pClientList[sAttackerH]->m_bIsSelfSafe == true) && (m_pClientList[sTargetH] == m_pClientList[sAttackerH]))
				return 0;
		} 

		iTargetDefenseRatio += m_pClientList[sTargetH]->m_iAddDR;

		sTgtX = m_pClientList[sTargetH]->m_sX;
		sTgtY = m_pClientList[sTargetH]->m_sY;
		break;

	case DEF_OWNERTYPE_NPC:

		if (m_pNpcList[sTargetH] == 0) return 0;
		if (m_pNpcList[sTargetH]->m_iHP <= 0) return 0;

		if ((m_pNpcList[sTargetH]->m_sX != tdX) || (m_pNpcList[sTargetH]->m_sY != tdY)) return 0;

		cTargetDir = m_pNpcList[sTargetH]->m_cDir;
		iTargetDefenseRatio = m_pNpcList[sTargetH]->m_iDefenseRatio;

		if (cAttackerType == DEF_OWNERTYPE_PLAYER) 
		{	switch (m_pNpcList[sTargetH]->m_sType) {
			case 40: // ESG
			case 41: // GMG
			case 36: // AGT
			case 37: // CGT
			case 38: // MS
			case 39: // DT		
				// Snoopy added Tile where you can't stay (behind crusade structures) 
				// If you try to stay behind and accept normal punition, damage is worse and at each attack...
				if (   (m_bIsCrusadeMode == true)
					&& (m_pClientList[sAttackerH]->m_iAdminUserLevel == 0)
					&& (m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->bGetIsStayAllowedTile(m_pClientList[sAttackerH]->m_sX, m_pClientList[sAttackerH]->m_sY) == false))
				{	int iDir = iDice(1,8);
					if (iDir == 5) iDir++;
					int iDamage2 = m_pClientList[sAttackerH]->m_iHP/8 + iDice(1,20) + iAP_SM;					
					if (iDamage2 < 40) iDamage2 = 30 + iDice(1,20);
					if (iDamage2 > m_pClientList[sAttackerH]->m_iHP) iDamage2 = m_pClientList[sAttackerH]->m_iHP -1;
					m_pClientList[sAttackerH]->m_iHP -= iDamage2;					
					m_pClientList[sAttackerH]->m_iLastDamage = iDamage2;
					m_pClientList[sAttackerH]->m_dwRecentAttackTime = dwTime;
					SendNotifyMsg(0, sAttackerH, DEF_NOTIFY_DAMAGEMOVE, iDir, iDamage2, 0, 0);					
					iAP_SM = iAP_SM/2; 
					iAP_L = iAP_L/2;
				}
				if ((m_pClientList[sAttackerH]->m_cSide == 0) || (m_pNpcList[sTargetH]->m_cSide == m_pClientList[sAttackerH]->m_cSide)) return 0;
				break;
			} 

			if (   (wWeaponType == 25) && (m_pNpcList[sTargetH]->m_cActionLimit == 5) 
				&& (m_pNpcList[sTargetH]->m_iBuildCount > 0)) 
			{	if ((m_pClientList[sAttackerH]->m_iCrusadeDuty != 2) && (m_pClientList[sAttackerH]->m_iAdminUserLevel == 0)) break;
				switch (m_pNpcList[sTargetH]->m_sType) {
				case 36: // AGT
				case 37: // CGT
				case 38: // MS
				case 39: // DT
					// administrators instantly build crusade structures
					// SNOOPY: was buugy because those structure woundn't Attack due to a m_iBuildCount = 0 value....
					if (m_pClientList[sAttackerH]->m_iAdminUserLevel > 0) 
					{	m_pNpcList[sTargetH]->m_sAppr2 = 0;
						m_pNpcList[sTargetH]->m_iBuildCount = 1;
						SendEventToNearClient_TypeA(sTargetH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
					}				
					switch (m_pNpcList[sTargetH]->m_iBuildCount) {
					case 1:
						m_pNpcList[sTargetH]->m_sAppr2 = 0;
						SendEventToNearClient_TypeA(sTargetH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
						// SNOOPY: Will not send those message outside crusades...
						if (m_bIsCrusadeMode == true) 
						{	switch (m_pNpcList[sTargetH]->m_sType) {
							case 36: iConstructionPoint = 700; iWarContribution = 700; break;
							case 37: iConstructionPoint = 700; iWarContribution = 700; break;
							case 38: iConstructionPoint = 500; iWarContribution = 500; break;
							case 39: iConstructionPoint = 500; iWarContribution = 500; break;
							}
							// Snoopy: Forbid Tile behind finished crusade structure
							m_pMapList[m_pNpcList[sTargetH]->m_cMapIndex]->SetStayAllowedFlag(m_pNpcList[sTargetH]->m_sX, m_pNpcList[sTargetH]->m_sY-1, false);
							m_pClientList[sAttackerH]->m_iWarContribution   += iWarContribution;
							

							wsprintf(G_cTxt, "Construction Complete: PC(%s) - WarContribution +%d"
								, m_pClientList[sAttackerH]->m_cCharName
								, iWarContribution);
							PutLogList(G_cTxt);
							PutLogEventFileList(G_cTxt);
							SendNotifyMsg(0, sAttackerH, DEF_NOTIFY_CONSTRUCTIONPOINT, m_pClientList[sAttackerH]->m_iConstructionPoint, m_pClientList[sAttackerH]->m_iWarContribution, 0, 0);
						}
						break;
					case 5:
						m_pNpcList[sTargetH]->m_sAppr2 = 1;
						SendEventToNearClient_TypeA(sTargetH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
						break;
					case 10:
						m_pNpcList[sTargetH]->m_sAppr2 = 2;
						SendEventToNearClient_TypeA(sTargetH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
						break;
					}
					break;
				}
				
				m_pNpcList[sTargetH]->m_iBuildCount--;
				if (m_pNpcList[sTargetH]->m_iBuildCount <= 0) {
					m_pNpcList[sTargetH]->m_iBuildCount = 0;
				}
				return 0;
			}
			if (   (wWeaponType == 27) && (m_pNpcList[sTargetH]->m_cCropType != 0) 
				&& (m_pNpcList[sTargetH]->m_cActionLimit == 5) && (m_pNpcList[sTargetH]->m_iBuildCount > 0)) 
			{	cFarmingSkill = m_pClientList[sAttackerH]->m_cSkillMastery[2];
				cCropSkill = m_pNpcList[sTargetH]->m_cCropSkill;
				if (cFarmingSkill < 20) return 0;
				if (m_pClientList[sAttackerH]->m_iLevel < 20) return 0;
				switch(m_pNpcList[sTargetH]->m_sType) {
				case 64:
					// There skill increase only if needed skill is less than 10% over the minimum skill for such crop
					// Only count once per seedbag if over mimi Skill +10%
					iFarmingSSN = 1;
					if (cFarmingSkill <= (cCropSkill + 10)) 
					{	iFarmingSSN = 1;
					}else 
					{	iFarmingSSN = 0;
					}
					switch(m_pNpcList[sTargetH]->m_iBuildCount) {
					case 1:	
						m_pNpcList[sTargetH]->m_sAppr2 = 3;
						SendEventToNearClient_TypeA(sTargetH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
						//sub_4B67E0
						CalculateSSN_SkillIndex(sAttackerH, 2, 1); 
						_CheckFarmingAction(sAttackerH, sTargetH, 1);
						DeleteNpc(sTargetH);
						return 0;
					case 8:	
						m_pNpcList[sTargetH]->m_sAppr2 = 3;
						SendEventToNearClient_TypeA(sTargetH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
						CalculateSSN_SkillIndex(sAttackerH, 2, iFarmingSSN);
						_CheckFarmingAction(sAttackerH, sTargetH, 0);
						m_pNpcList[sTargetH]->m_iBuildCount--;
						break;
					case 18:
						m_pNpcList[sTargetH]->m_sAppr2 = 2;
						SendEventToNearClient_TypeA(sTargetH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
						CalculateSSN_SkillIndex(sAttackerH, 2, iFarmingSSN);
						_CheckFarmingAction(sAttackerH, sTargetH, 0);
						// 1/20 chance to have 4 crops
						if (iDice(1,20) != 2)  m_pNpcList[sTargetH]->m_iBuildCount--;
						break;
					default:
						// SNOOPY Added a Skill Check to slow down a bit the process.
						// With minima Skill will cost twice the time
						if (   ((iDice(1,100) <= ((50+cFarmingSkill)-(cCropSkill)))// Chance off succes depends on skill
							|| (iDice(1,100)) <6 )) { // 1/20 chace of succes  ever skill you have
							m_pNpcList[sTargetH]->m_iBuildCount--;
						}
						break;
					}
					break;
				}
				// Useless? safety check !			
				if (m_pNpcList[sTargetH]->m_iBuildCount <= 0) 
				{	m_pNpcList[sTargetH]->m_iBuildCount = 0;
				}
				// They forgot to put Hoe deplession here..
				// Weapon's wep life off				
				sWeaponIndex = m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND];
				if ( sWeaponIndex != -1)
				{	m_pClientList[sAttackerH]->m_pItemList[sWeaponIndex]->m_wCurLifeSpan--;
					SendNotifyMsg(0, sAttackerH, DEF_NOTIFY_CURLIFESPAN, sWeaponIndex, m_pClientList[sAttackerH]->m_pItemList[sWeaponIndex]->m_wCurLifeSpan, 0, 0);
					if (m_pClientList[sAttackerH]->m_pItemList[sWeaponIndex]->m_wCurLifeSpan < 1)
						m_pClientList[sAttackerH]->m_pItemList[sWeaponIndex]->m_wCurLifeSpan = 0;
					// then notify the client...
					if (m_pClientList[sAttackerH]->m_pItemList[sWeaponIndex]->m_wCurLifeSpan <= 0)
					{	SendNotifyMsg(0, sAttackerH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[sAttackerH]->m_pItemList[sWeaponIndex]->m_cEquipPos, sWeaponIndex, 0, 0);
						ReleaseItemHandler(sAttackerH, sWeaponIndex, true);
				}	}
				return 0;
		}	}

		sTgtX = m_pNpcList[sTargetH]->m_sX;
		sTgtY = m_pNpcList[sTargetH]->m_sY;
		break;
	}

	if ( (cAttackerType == DEF_OWNERTYPE_PLAYER) && (cTargetType == DEF_OWNERTYPE_PLAYER) ) {

		sX = m_pClientList[sAttackerH]->m_sX;
		sY = m_pClientList[sAttackerH]->m_sY;

		dX = m_pClientList[sTargetH]->m_sX;
		dY = m_pClientList[sTargetH]->m_sY;

		if (m_pMapList[m_pClientList[sTargetH]->m_cMapIndex]->iGetAttribute(sX, sY, 0x00000006) != 0) return 0;
		if (m_pMapList[m_pClientList[sTargetH]->m_cMapIndex]->iGetAttribute(dX, dY, 0x00000006) != 0) return 0;
	}

	if (cAttackerType == DEF_OWNERTYPE_PLAYER) {
		if ((m_pClientList[sAttackerH]->m_iDex) > 50) {
			iAttackerHitRatio += ((m_pClientList[sAttackerH]->m_iDex) - 50); 	
		}
	}

	if ((wWeaponType >= 40) && (wWeaponType < 55)) {
		switch (m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_cWhetherStatus) {
		case 0:	break;
		case 1:	iAttackerHitRatio -= (iAttackerHitRatio / 20); break;
		case 2:	iAttackerHitRatio -= (iAttackerHitRatio / 10); break;
		case 3:	iAttackerHitRatio -= (iAttackerHitRatio / 4);  break;
		}
	}

	if (iAttackerHitRatio < 0)   iAttackerHitRatio = 0;    
	switch (cTargetType) {
	case DEF_OWNERTYPE_PLAYER:
		cProtect = m_pClientList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT];
		break;

	case DEF_OWNERTYPE_NPC:
		cProtect = m_pNpcList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT]; 
		break;
	}

	if (cAttackerType == DEF_OWNERTYPE_PLAYER) {
		if (m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND] != -1) {
			if (m_pClientList[sAttackerH]->m_pItemList[m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND]] == 0) {
				m_pClientList[sAttackerH]->m_bIsItemEquipped[m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND]] = false;
				DeleteClient(sAttackerH, true, true);
				return 0;
			}

			if (m_pClientList[sAttackerH]->m_pItemList[m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND]]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ATTACK_ARROW) {
				if (m_pClientList[sAttackerH]->m_cArrowIndex == -1) {
					return 0;
				}
				else {
					if (m_pClientList[sAttackerH]->m_pItemList[m_pClientList[sAttackerH]->m_cArrowIndex] == 0) 
						return 0;

					if (bArrowUse != true)
						m_pClientList[sAttackerH]->m_pItemList[m_pClientList[sAttackerH]->m_cArrowIndex]->m_dwCount--;
					if (m_pClientList[sAttackerH]->m_pItemList[ m_pClientList[sAttackerH]->m_cArrowIndex ]->m_dwCount <= 0) {

						ItemDepleteHandler(sAttackerH, m_pClientList[sAttackerH]->m_cArrowIndex, false, true);
						m_pClientList[sAttackerH]->m_cArrowIndex = _iGetArrowItemIndex(sAttackerH);
					}
					else {
						SendNotifyMsg(0, sAttackerH, DEF_NOTIFY_SETITEMCOUNT, m_pClientList[sAttackerH]->m_cArrowIndex, m_pClientList[sAttackerH]->m_pItemList[ m_pClientList[sAttackerH]->m_cArrowIndex ]->m_dwCount, false, 0);
						iCalcTotalWeight(sAttackerH);
					}
				}
				if (cProtect == 1)
				{
					iAP_SM = iAP_SM / 2;
					iAP_L = iAP_L / 2;

				}
			}
			else {
				switch (cProtect) {
				case 1:
					break;
				case 3: iTargetDefenseRatio += 40;  break;
				case 4: iTargetDefenseRatio += 100; break;
				}
				if (iTargetDefenseRatio < 0) iTargetDefenseRatio = 1;
			}
		}
	}
	else {
		switch (cProtect) {
		case 1: 
			if (bNormalMissileAttack) return 0;
			break;
		case 3: iTargetDefenseRatio += 40;  break;
		case 4: iTargetDefenseRatio += 100; break;
		}
		if (iTargetDefenseRatio < 0) iTargetDefenseRatio = 1;
	}

	if (cAttackerDir == cTargetDir) iTargetDefenseRatio = iTargetDefenseRatio / 2;
	if (iTargetDefenseRatio < 1)   iTargetDefenseRatio = 1;

	dTmp1 = (float)(iAttackerHitRatio);
	dTmp2 = (float)(iTargetDefenseRatio);
	dTmp3 = (dTmp1 / dTmp2) * 50.0f;
	iDestHitRatio = (int)(dTmp3); 

	if ((bIsAttackerBerserk == true) && (iAttackMode < 20)) {
		iAP_SM *= 2;
		iAP_L  *= 2;
	}

	if (cAttackerType == DEF_OWNERTYPE_PLAYER) {
		iAP_SM += m_pClientList[sAttackerH]->m_iAddPhysicalDamage;
		iAP_L  += m_pClientList[sAttackerH]->m_iAddPhysicalDamage;
	}

	if (bNearAttack == true) {
		iAP_SM = iAP_SM / 2;
		iAP_L  = iAP_L / 2;
	}

	if (cTargetType == DEF_OWNERTYPE_PLAYER) {
		iAP_SM -= (iDice(1, m_pClientList[sTargetH]->m_iVit/10) - 1);
		iAP_L  -= (iDice(1, m_pClientList[sTargetH]->m_iVit/10) - 1);
	}

	if (cAttackerType == DEF_OWNERTYPE_PLAYER) {
		if (iAP_SM <= 1) iAP_SM = 1;
		if (iAP_L  <= 1) iAP_L  = 1;
	}
	else {
		if (iAP_SM <= 0) iAP_SM = 0;
		if (iAP_L  <= 0) iAP_L  = 0;
	}

	// Centuu : Ranged Hit Fix - HB2
	bool bRangedWeapon = false;
	if (cAttackerType == DEF_OWNERTYPE_PLAYER) {
		if (m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND] != -1)
			 sWeaponIndex = m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND];
		else sWeaponIndex = m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND];
		if (sWeaponIndex != -1) {
			if (m_pClientList[sAttackerH]->m_pItemList[sWeaponIndex]->m_sRelatedSkill == 6 ||
				m_pClientList[sAttackerH]->m_pItemList[sWeaponIndex]->m_sIDnum == 845 ||
				m_pClientList[sAttackerH]->m_pItemList[sWeaponIndex]->m_sIDnum == 1037) {
				bRangedWeapon = true;
			}
		}
		switch (cTargetType) {
		case DEF_OWNERTYPE_PLAYER:
			if (m_pClientList[sAttackerH]->m_sX > m_pClientList[sTargetH]->m_sX) {
				if ((m_pClientList[sAttackerH]->m_sX - m_pClientList[sTargetH]->m_sX) > 1) {
					if ((bRangedWeapon != true) && (iAttackMode < 20)) return 0;
				}
			}
			if (m_pClientList[sTargetH]->m_sX > m_pClientList[sAttackerH]->m_sX) {
				if ((m_pClientList[sTargetH]->m_sX - m_pClientList[sAttackerH]->m_sX) > 1) {
					if ((bRangedWeapon != true) && (iAttackMode < 20)) return 0;
				}
			}
			if (m_pClientList[sAttackerH]->m_sY > m_pClientList[sTargetH]->m_sY) {
				if ((m_pClientList[sAttackerH]->m_sY - m_pClientList[sTargetH]->m_sY) > 1) {
					if ((bRangedWeapon != true) && (iAttackMode < 20)) return 0;
				}
			}
			if (m_pClientList[sTargetH]->m_sY > m_pClientList[sAttackerH]->m_sY) {
				if ((m_pClientList[sTargetH]->m_sY - m_pClientList[sAttackerH]->m_sY) > 1) {
					if ((bRangedWeapon != true) && (iAttackMode < 20)) return 0;
				}
			}
			break;
		case DEF_OWNERTYPE_NPC:
			if (m_pClientList[sAttackerH]->m_sX > m_pNpcList[sTargetH]->m_sX) {
				if ((m_pClientList[sAttackerH]->m_sX - m_pNpcList[sTargetH]->m_sX) > 1) {
					if ((bRangedWeapon != true) && (iAttackMode < 20)) return 0;
				}
			}
			if (m_pNpcList[sTargetH]->m_sX > m_pClientList[sAttackerH]->m_sX) {
				if ((m_pNpcList[sTargetH]->m_sX - m_pClientList[sAttackerH]->m_sX) > 1) {
					if ((bRangedWeapon != true) && (iAttackMode < 20)) return 0;
				}
			}
			if (m_pClientList[sAttackerH]->m_sY > m_pNpcList[sTargetH]->m_sY) {
				if ((m_pClientList[sAttackerH]->m_sY - m_pNpcList[sTargetH]->m_sY) > 1) {
					if ((bRangedWeapon != true) && (iAttackMode < 20)) return 0;
				}
			}
			if (m_pNpcList[sTargetH]->m_sY > m_pClientList[sAttackerH]->m_sY) {
				if ((m_pNpcList[sTargetH]->m_sY - m_pClientList[sAttackerH]->m_sY) > 1) {
					if ((bRangedWeapon != true) && (iAttackMode < 20)) return 0;
				}
			}
			break;
		}
	}

	iResult = iDice(1, 100);

	if (iResult <= iDestHitRatio) {
		if (   (m_bHeldenianWarInitiated == true)
			&& (bCheckHeldenianMap(sAttackerH, cAttackerType) == true))
		{	iAP_SM += iAP_SM/3;
			iAP_L  += iAP_L/3;
		}
		if (cAttackerType == DEF_OWNERTYPE_PLAYER) {

			if (((m_pClientList[sAttackerH]->m_iHungerStatus <= 10) || (m_pClientList[sAttackerH]->m_iSP <= 0)) && (iDice(1,10) == 5)) return false;	
			m_pClientList[sAttackerH]->m_iComboAttackCount++;
			if (m_pClientList[sAttackerH]->m_iComboAttackCount < 0) m_pClientList[sAttackerH]->m_iComboAttackCount = 0;
			if (m_pClientList[sAttackerH]->m_iComboAttackCount > 4) m_pClientList[sAttackerH]->m_iComboAttackCount = 1;
			iWeaponSkill = _iGetWeaponSkillType(sAttackerH);
			iComboBonus = iGetComboAttackBonus(iWeaponSkill, m_pClientList[sAttackerH]->m_iComboAttackCount);

			if ((m_pClientList[sAttackerH]->m_iComboAttackCount > 1) && (m_pClientList[sAttackerH]->m_iAddCD != 0))
				iComboBonus += m_pClientList[sAttackerH]->m_iAddCD;

			iAP_SM += iComboBonus;
			iAP_L  += iComboBonus;

			switch (m_pClientList[sAttackerH]->m_iSpecialWeaponEffectType) {
				case 0: break;
				case ITEMSTAT_CRITICAL:
					if ((m_pClientList[sAttackerH]->m_iSuperAttackLeft > 0) && (iAttackMode >= 20)) {
						iAP_SM += m_pClientList[sAttackerH]->m_iSpecialWeaponEffectValue;
						iAP_L  += m_pClientList[sAttackerH]->m_iSpecialWeaponEffectValue;
					}
					break;

				case ITEMSTAT_POISONING:
					cAttackerSA = 61;
					iAttackerSAvalue = m_pClientList[sAttackerH]->m_iSpecialWeaponEffectValue*5; 
					break;

				case ITEMSTAT_RIGHTEOUS:
					cAttackerSA = 62;
					break;
			}

			if (m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_bIsFightZone == true) {
				iAP_SM += iAP_SM/3;
				iAP_L  += iAP_L/3;
			}

			if (bCheckHeldenianMap(sAttackerH, DEF_OWNERTYPE_PLAYER) == 1) {
				iAP_SM += iAP_SM/3;
				iAP_L  += iAP_L/3;
			}
			
			//Magn0S:: Extra Dmg Map Restriction
			if (m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->bMapBonusDmg == true) {
				iAP_SM += iAP_SM / 2;
				iAP_L += iAP_L / 2;
			}

			//Magn0S:: Extra Dmg Map Restriction
			if (m_iServerPhyDmg > 0) {
				iExtraDmg = iAP_L / 20;
				iAP_SM += iExtraDmg * m_iServerPhyDmg;
				iAP_L += iExtraDmg * m_iServerPhyDmg;
			}

			if ((cTargetType == DEF_OWNERTYPE_PLAYER) && (m_bIsCrusadeMode == true) && (m_pClientList[sAttackerH]->m_iCrusadeDuty == 1)) {
				if (m_pClientList[sAttackerH]->m_iLevel <= 80) {
					iAP_SM += iAP_SM;
					iAP_L += iAP_L;
				}
				else if (m_pClientList[sAttackerH]->m_iLevel > 80 && m_pClientList[sAttackerH]->m_iLevel <= 100) {
					iAP_SM += (iAP_SM* 7)/10;
					iAP_L += (iAP_L* 7)/10;
				} else if (m_pClientList[sAttackerH]->m_iLevel > 100) {
					iAP_SM += iAP_SM/3;
					iAP_L += iAP_L/3;
				}
			}
		}

		char cEquipPos;
		switch (cTargetType) 
		{
		case DEF_OWNERTYPE_PLAYER:
			ClearSkillUsingStatus(sTargetH);
			if ((dwTime - m_pClientList[sTargetH]->m_dwTime) > DEF_RAGPROTECTIONTIME) {
				return 0;
			}
			else {
				switch (cAttackerSA) {
					case 62:
						if (m_pClientList[sTargetH]->m_iRating < 0) {
							iTemp = abs(m_pClientList[sTargetH]->m_iRating)/100;
							if (iTemp > 10) iTemp = 10;
							iAP_SM += iTemp;
						}
						break;
				}

				iTemp = iDice(1,10000);
				if ((iTemp >= 1) && (iTemp < 5000))           iHitPoint = 1;
				else if ((iTemp >= 5000) && (iTemp < 7500))   iHitPoint = 2;
				else if ((iTemp >= 7500) && (iTemp < 9000))   iHitPoint = 3;
				else if ((iTemp >= 9000) && (iTemp <= 10000)) iHitPoint = 4;

				// PA: Shields
				if (m_pClientList[sTargetH]->m_iDamageAbsorption_Shield > 0)
				{
					if (iDice(1, 100) <= (m_pClientList[sTargetH]->m_cSkillMastery[11]))
					{
						CalculateSSN_SkillIndex(sTargetH, 11, 1);
						cEquipPos = DEF_EQUIPPOS_LHAND;
						// Usure Shield
						iTemp = m_pClientList[sTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_LHAND];
						if ((iTemp != -1) && (m_pClientList[sTargetH]->m_pItemList[iTemp] != 0))
						{
							if ((m_pClientList[sTargetH]->m_cSide != 0) && (m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0)) {
								m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan--;
								SendNotifyMsg(0, sTargetH, DEF_NOTIFY_CURLIFESPAN, iTemp, m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan, 0, 0);
							}
							if (m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0)
							{
								SendNotifyMsg(0, sTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[sTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);
								ReleaseItemHandler(sTargetH, iTemp, true);
							}
						}
					}
				}

				// PA: Back (capes)  Added Damage absorbtion for enchanted capes  iAP_Abs_Cape
				if ((cAttackerDir == cTargetDir) // Of course only when attacked from behind !
					&& (m_pClientList[sTargetH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_BACK] > 0))
				{
					cEquipPos = DEF_EQUIPPOS_BACK;
					// Deplete Cape durability in this case	
					iTemp = m_pClientList[sTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_BACK];
					if ((iTemp != -1) && (m_pClientList[sTargetH]->m_pItemList[iTemp] != 0))
					{
						if ((m_pClientList[sTargetH]->m_cSide != 0) && (m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0)) {
							m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan--;
							SendNotifyMsg(0, sTargetH, DEF_NOTIFY_CURLIFESPAN, iTemp, m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan, 0, 0);
						}
						if (m_pClientList[sTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0)
						{
							SendNotifyMsg(0, sTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[sTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);
							ReleaseItemHandler(sTargetH, iTemp, true);
						}
					}
				}

				if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH] != 0) && (m_pClientList[sAttackerH]->m_bIsSpecialAbilityEnabled == true)) {
					switch (m_pClientList[sAttackerH]->m_iSpecialAbilityType) {
						case 0: break;
						case 1: // Xelima weapon
							iTemp = (m_pClientList[sTargetH]->m_iHP / 2);
							if (iAP_SM <= 0) iAP_SM = 1;
							if (iTemp > iAP_SM) iAP_SM = iTemp;
							break;
						case 2: // Ice weapon
							if (   (bCheckResistingIceSuccess(0, sTargetH, DEF_OWNERTYPE_PLAYER, 50) == false) 
								&& (m_pClientList[sTargetH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_ICE ] == 0)) 
							{	m_pClientList[sTargetH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_ICE ] = 1;
								SetIceFlag(sTargetH, DEF_OWNERTYPE_PLAYER, true);
								bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + 30000, 
									sTargetH, DEF_OWNERTYPE_PLAYER, 0, 0, 0, 1, 0, 0);
								SendNotifyMsg(0, sTargetH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_ICE, 1, 0, 0);
							}
							break;
						case 3: // Medusa sword
							if (m_pClientList[sTargetH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_HOLDOBJECT ] >= 5) break; 
							// Medusa neck or very high PR gives invulnerability, unless having a Medusa Blade...
							if (   ((m_pClientList[sTargetH]->m_cSkillMastery[23] + m_pClientList[sTargetH]->m_iAddPR + iDice(1,100)) >= 250) 
								&& (m_pClientList[sTargetH]->m_iSpecialAbilityType != 3)) break; 
							m_pClientList[sTargetH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_HOLDOBJECT ] = 5;
							// Nota: The client was fixed to show "paralysed" for any 2+ value
							bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_HOLDOBJECT, dwTime + 10000, 
								sTargetH, DEF_OWNERTYPE_PLAYER, 0, 0, 0, 5, 0, 0);								
							SendNotifyMsg(0, sTargetH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_HOLDOBJECT, 5, 0, 0);		
							break;
						case 5: // Vampiric weapon
							m_pClientList[sAttackerH]->m_iHP += iAP_SM;
							if (iGetMaxHP(sAttackerH, true) < m_pClientList[sAttackerH]->m_iHP)  // BloodEffect limite le regen
								m_pClientList[sAttackerH]->m_iHP = iGetMaxHP(sAttackerH, true);
							SendNotifyMsg(0, sAttackerH, DEF_NOTIFY_HP, 0, 0, 0, 0);
							break;
					}
				}

				if (m_pClientList[sTargetH]->m_bIsSpecialAbilityEnabled == true)
				{	if (   (cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH] != 0)) 
					{	switch (m_pClientList[sTargetH]->m_iSpecialAbilityType) {
						case 50: // Merien Plate
							if (m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND] != -1)
								sWeaponIndex = m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND];
							else sWeaponIndex = m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND];
							if ((sWeaponIndex != -1) && (iWeaponSkill !=6)) // Don't break bows
								m_pClientList[sAttackerH]->m_pItemList[sWeaponIndex]->m_wCurLifeSpan = 0;
							break;
						case 51: // Merien Shield effect on armor piece
							if (iHitPoint == m_pClientList[sTargetH]->m_iSpecialAbilityEquipPos)
								iAP_SM = 0;
							break;
						case 52: // Merien Shield
							iAP_SM = 0;
							break;
							//Magn0S:: Bloody Effect
						case 60: // Bloody Armors
							if (m_pClientList[sAttackerH]->m_iHP <= iAP_SM / 10) {
								iAP_SM = m_pClientList[sAttackerH]->m_iHP - 1;
							}
							m_pClientList[sAttackerH]->m_iHP -= iAP_SM / 10;
							SendNotifyMsg(0, sAttackerH, DEF_NOTIFY_HP, 0, 0, 0, 0);
							break;
						}
					}else if (cAttackerType == DEF_OWNERTYPE_NPC)
					{	switch (m_pClientList[sTargetH]->m_iSpecialAbilityType) {
						case 50: // Merien Plate							
							if (m_pNpcList[sAttackerH]->m_cActionLimit < 2)
							{	m_pNpcList[sAttackerH]->m_sBehaviorTurnCount = 0;
								m_pNpcList[sAttackerH]->m_cBehavior = DEF_BEHAVIOR_FLEE;
								m_pNpcList[sAttackerH]->m_bIsPermAttackMode = false;
								m_pNpcList[sAttackerH]->m_iTargetIndex = 0;
							}
							break;
						case 51: // Merien Shield effect on armor piece
							if (iHitPoint == m_pClientList[sTargetH]->m_iSpecialAbilityEquipPos)
								iAP_SM = 0;
							break;
						case 52: // Merien Shield
							iAP_SM = iAP_SM/2;
							break;				
				}	}	}

				switch (iHitPoint) {
				case 1:
					cEquipPos = DEF_EQUIPPOS_BODY;
					iTemp = m_pClientList[sTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_BODY];
					if ((iTemp != -1) && (m_pClientList[sTargetH]->m_pItemList[iTemp] != 0) && (cAttackerType != DEF_OWNERTYPE_NPC) )
					{
						bCalculateEnduranceDecrement(sTargetH, sAttackerH, cTargetType, iTemp);
					}
					break;

				case 2:
					cEquipPos = DEF_EQUIPPOS_LEGGINGS;
					iTemp = m_pClientList[sTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_LEGGINGS];
					if ((iTemp != -1) && (m_pClientList[sTargetH]->m_pItemList[iTemp] != 0) && (cAttackerType != DEF_OWNERTYPE_NPC)) {
						bCalculateEnduranceDecrement(sTargetH, sAttackerH, cTargetType, iTemp);
						
					}
					break;

				case 3:
					cEquipPos = DEF_EQUIPPOS_ARMS;
					iTemp = m_pClientList[sTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_ARMS];
					if ((iTemp != -1) && (m_pClientList[sTargetH]->m_pItemList[iTemp] != 0) && (cAttackerType != DEF_OWNERTYPE_NPC)) {
						bCalculateEnduranceDecrement(sTargetH, sAttackerH, cTargetType, iTemp);
					}
					break;

				case 4:
					cEquipPos = DEF_EQUIPPOS_HEAD;
					iTemp = m_pClientList[sTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_HEAD];
					if ((iTemp != -1) && (m_pClientList[sTargetH]->m_pItemList[iTemp] != 0) && (cAttackerType != DEF_OWNERTYPE_NPC)) {
						bCalculateEnduranceDecrement(sTargetH, sAttackerH, cTargetType, iTemp);
					}
					break;
				}
	
				if ((cAttackerSA == 2) && (m_pClientList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT] != 0)) {
					SendNotifyMsg(0, sTargetH, DEF_NOTIFY_MAGICEFFECTOFF, DEF_MAGICTYPE_PROTECT, m_pClientList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT ], 0, 0);
					switch(m_pClientList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT]) {
					case 1:
						SetProtectionFromArrowFlag(sTargetH, DEF_OWNERTYPE_PLAYER, false);
						break;
					case 2:
					case 5:
						SetMagicProtectionFlag(sTargetH, DEF_OWNERTYPE_PLAYER, false);
						break;
					case 3:
					case 4:
						SetDefenseShieldFlag(sTargetH, DEF_OWNERTYPE_PLAYER, false);
						break;
					}
					m_pClientList[sTargetH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_PROTECT] = 0;
					bRemoveFromDelayEventList(sTargetH, DEF_OWNERTYPE_PLAYER, DEF_MAGICTYPE_PROTECT);
				}

				if ( (m_pClientList[sTargetH]->m_bIsPoisoned == false) && 
					((cAttackerSA == 5) || (cAttackerSA == 6) || (cAttackerSA == 61)) ) {
					if (bCheckResistingPoisonSuccess(sTargetH, DEF_OWNERTYPE_PLAYER) == false) {
						m_pClientList[sTargetH]->m_bIsPoisoned = true;
						if (cAttackerSA == 5)		m_pClientList[sTargetH]->m_iPoisonLevel = 15;
						else if (cAttackerSA == 6)  m_pClientList[sTargetH]->m_iPoisonLevel = 40;
						else if (cAttackerSA == 61) m_pClientList[sTargetH]->m_iPoisonLevel = iAttackerSAvalue;

						m_pClientList[sTargetH]->m_dwPoisonTime = dwTime;
						SendNotifyMsg(0, sTargetH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_POISON, m_pClientList[sTargetH]->m_iPoisonLevel, 0, 0);
						SetPoisonFlag(sTargetH, DEF_OWNERTYPE_PLAYER, true);
					}
				}

				// Magic Weapons - Add Magical Damages
				if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH] != 0)) {
					float iInitial_AP_SM, iWeaponDamage, iTotalMagicDamage;
					int iMagDamage;
					short sManaCost;
					UINT32 dwType1, dwType2, dwValue1, dwValue2;

					if(m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND] != -1){
						sItemIndex = m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND];
					}
					else if(m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND] != -1){
						sItemIndex = m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND];
					}
					else sItemIndex = -1;

					if(sItemIndex != -1){
						if ((m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x00F00000) != 0) {
							dwType1  = (m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x00F00000) >> 20;  
							dwValue1 = (m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x000F0000) >> 16;
							dwType2  = (m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x0000F000) >> 12;  
							dwValue2 = (m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x00000F00) >> 8;
						}
						if(dwType1 == 15){
							sManaCost = (short)dwValue2*2;
							if(m_pClientList[sAttackerH]->m_iMP >= sManaCost){
								iWeaponDamage = (float)dwValue2*7;
								iInitial_AP_SM = (float)iAP_SM;
								
								iMagDamage = (m_pClientList[sAttackerH]->m_iMag)/20;
								iTotalMagicDamage = (float)((iInitial_AP_SM/100.0f)*(iWeaponDamage/100.0f))+iMagDamage;
								
								if (m_pClientList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT] == 2) 
									iAP_SM = (int)iTotalMagicDamage / 2;
								else if (m_pClientList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT] == 5)
									iAP_SM = (int)iWeaponDamage;
								else
									iAP_SM = (int)iTotalMagicDamage;

								m_pClientList[sAttackerH]->m_iMP -= sManaCost;
								SendNotifyMsg(0, sAttackerH, DEF_NOTIFY_MP, 0, 0, 0, 0);
							}
						}
					}
				}
				// End of Magic Weapons

				if (m_pClientList[sTargetH]->m_iDamageAbsorption_Armor[cEquipPos] != 0) { // PA
					dTmp1 = (float)iAP_SM;
					dTmp2 = (float)m_pClientList[sTargetH]->m_iDamageAbsorption_Armor[cEquipPos];
					dTmp3 = (dTmp2 / 100.0f) * dTmp1;
					iAP_SM = iAP_SM - (int)dTmp3;
				}

				m_pClientList[sTargetH]->m_iHP -= iAP_SM;

				//Magn0S:: Show Damage
				if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH] != 0)) {
				if (m_pClientList[sAttackerH]->bShowDmg) {
					ZeroMemory(cDamageMod, sizeof(cDamageMod));
					//Magn0S:: Change the showdmg style, now show where is ur hit location.
					switch (iHitPoint) {
						case 1:
							wsprintf(cDamageMod ,"You did %d Damage to %s (BODY)", iAP_SM, m_pClientList[sTargetH]->m_cCharName);
							SendNotifyMsg(0, sAttackerH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, cDamageMod);
							break;
						case 2:
							wsprintf(cDamageMod ,"You did %d Damage to %s (LEGS)", iAP_SM, m_pClientList[sTargetH]->m_cCharName);
							SendNotifyMsg(0, sAttackerH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, cDamageMod);
							break;
						case 3:
							wsprintf(cDamageMod ,"You did %d Damage to %s (ARMS)", iAP_SM, m_pClientList[sTargetH]->m_cCharName);
							SendNotifyMsg(0, sAttackerH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, cDamageMod);
							break;
						case 4:
							wsprintf(cDamageMod ,"You did %d Damage to %s (HEAD)", iAP_SM, m_pClientList[sTargetH]->m_cCharName);
							SendNotifyMsg(0, sAttackerH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, cDamageMod);
							break;
						default:
							wsprintf(cDamageMod ,"You did %d Damage to %s", iAP_SM, m_pClientList[sTargetH]->m_cCharName);
							SendNotifyMsg(0, sAttackerH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, cDamageMod);
							break;
						}
					}
				}

				if (m_pClientList[sTargetH]->m_iHP <= 0) {
					if (cAttackerType == DEF_OWNERTYPE_PLAYER) bAnalyzeCriminalAction(sAttackerH, m_pClientList[sTargetH]->m_sX, m_pClientList[sTargetH]->m_sY);
					ClientKilledHandler(sTargetH, sAttackerH, cAttackerType, iAP_SM);	
					bKilled     = true;
					iKilledDice = (m_pClientList[sTargetH]->m_iLevel);
				}
				else {
					if (iAP_SM > 0) {
						if (m_pClientList[sTargetH]->m_iAddTransMana > 0) {
							dTmp1 = (float)m_pClientList[sTargetH]->m_iAddTransMana;
							dTmp2 = (float)iAP_SM;
							dTmp3 = (dTmp1/100.0f)*dTmp2;
							iTemp = iGetMaxMP(sTargetH);
							m_pClientList[sTargetH]->m_iMP += (int)dTmp3;
							if (m_pClientList[sTargetH]->m_iMP > iTemp) m_pClientList[sTargetH]->m_iMP = iTemp;
						}
						if (m_pClientList[sTargetH]->m_iAddChargeCritical > 0) {
							// Crit increase items (bug < and not <= means CritIncrease 1% have no effect)
							if (iDice(1,100) <= (m_pClientList[sTargetH]->m_iAddChargeCritical)) {
								iMaxSuperAttack = ((m_pClientList[sTargetH]->m_iLevel) / 10);
								if (m_pClientList[sTargetH]->m_iSuperAttackLeft < iMaxSuperAttack) m_pClientList[sTargetH]->m_iSuperAttackLeft++;
									SendNotifyMsg(0, sTargetH, DEF_NOTIFY_SUPERATTACKLEFT, 0, 0, 0, 0);
							}
						}

					SendNotifyMsg(0, sTargetH, DEF_NOTIFY_HP, 0, 0, 0, 0);

					if (cAttackerType == DEF_OWNERTYPE_PLAYER) 
						sAttackerWeapon = ((m_pClientList[sAttackerH]->m_sAppr2 & 0x0FF0) >> 4);
					else sAttackerWeapon = 1;

					if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_bIsFightZone == true)) 
						iMoveDamage = 60;
					else iMoveDamage = 40;

					if (iAP_SM >= iMoveDamage) {
						if (sTgtX == sAtkX) {
							if (sTgtY == sAtkY)     goto CAE_SKIPDAMAGEMOVE;
								else if (sTgtY > sAtkY) cDamageMoveDir = 5;
								else if (sTgtY < sAtkY) cDamageMoveDir = 1;
							}
							else if (sTgtX > sAtkX) {
								if (sTgtY == sAtkY)     cDamageMoveDir = 3;
									else if (sTgtY > sAtkY) cDamageMoveDir = 4;
									else if (sTgtY < sAtkY) cDamageMoveDir = 2;
								}
								else if (sTgtX < sAtkX) {
									if (sTgtY == sAtkY)     cDamageMoveDir = 7;
										else if (sTgtY > sAtkY) cDamageMoveDir = 6;
										else if (sTgtY < sAtkY) cDamageMoveDir = 8;							
								}
							m_pClientList[sTargetH]->m_iLastDamage = iAP_SM;

							SendNotifyMsg(0, sTargetH, DEF_NOTIFY_DAMAGEMOVE, cDamageMoveDir, iAP_SM, sAttackerWeapon, 0);
						}
						else {
CAE_SKIPDAMAGEMOVE:;
							int iProb;
							if (cAttackerType == DEF_OWNERTYPE_PLAYER) {
								switch (m_pClientList[sAttackerH]->m_sUsingWeaponSkill) {
									case 6 : iProb = 3500; break;
									case 8 : iProb = 1000; break;
									case 9 : iProb = 2900; break;
									case 10: iProb = 2500; break;
									case 14: iProb = 2000; break;
									case 21: iProb = 2000; break;
									default: iProb = 1; break;
								}
							}
							else iProb = 1;

							if (iDice(1,10000) >= iProb) 
								SendEventToNearClient_TypeA(sTargetH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTDAMAGE, iAP_SM, sAttackerWeapon, 0);		
							}

							if (m_pClientList[sTargetH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_HOLDOBJECT ] == 1) {
								SendNotifyMsg(0, sTargetH, DEF_NOTIFY_MAGICEFFECTOFF, DEF_MAGICTYPE_HOLDOBJECT, m_pClientList[sTargetH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_HOLDOBJECT ], 0, 0);
								m_pClientList[sTargetH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_HOLDOBJECT ] = 0;
								bRemoveFromDelayEventList(sTargetH, DEF_OWNERTYPE_PLAYER, DEF_MAGICTYPE_HOLDOBJECT);
							}

							m_pClientList[sTargetH]->m_iSuperAttackCount++;
							if (m_pClientList[sTargetH]->m_iSuperAttackCount > 14) { 
								m_pClientList[sTargetH]->m_iSuperAttackCount = 0;
								iMaxSuperAttack = ((m_pClientList[sTargetH]->m_iLevel) / 10);
								if (m_pClientList[sTargetH]->m_iSuperAttackLeft < iMaxSuperAttack) m_pClientList[sTargetH]->m_iSuperAttackLeft++;
									SendNotifyMsg(0, sTargetH, DEF_NOTIFY_SUPERATTACKLEFT, 0, 0, 0, 0);
							}
						}
					}
				}
				break;				   

		case DEF_OWNERTYPE_NPC:
			if (m_pNpcList[sTargetH]->m_cBehavior == DEF_BEHAVIOR_DEAD) return 0;
			if (m_pNpcList[sTargetH]->m_bIsKilled == true) return 0;
			if (m_bIsCrusadeMode == true) {
				if (cAttackerSide == m_pNpcList[sTargetH]->m_cSide) {
					switch (m_pNpcList[sTargetH]->m_sType) {
						case 40:
						case 41:
						case 43:
						case 44:
						case 45:
						case 46:
						case 47: 
						case 51:
							return 0;

						default: 
							break;
					}
				}
				else {
					switch (m_pNpcList[sTargetH]->m_sType) {
						case 41:
							if (cAttackerSide != 0) {
								m_pNpcList[sTargetH]->m_iV1 += iAP_L;
								if (m_pNpcList[sTargetH]->m_iV1 > 500) {
									m_pNpcList[sTargetH]->m_iV1 = 0;
									m_pNpcList[sTargetH]->m_iManaStock--;
									if (m_pNpcList[sTargetH]->m_iManaStock <= 0) m_pNpcList[sTargetH]->m_iManaStock = 0;
									
								}
							}
							break;
					}
				}
			}
			switch (m_pNpcList[sTargetH]->m_cActionLimit) { // Invulnerable npcs...
			case 2: // merchants
			case 6: // moving merchants
				return 0;
				
			}
			// SNOOPY: Added possibility to poison npcs
			if (cTargetType == DEF_OWNERTYPE_NPC) {
				if ((cAttackerSA == 5) || (cAttackerSA == 6) || (cAttackerSA == 61))
				{
					if (bCheckResistingPoisonSuccess(sTargetH, DEF_OWNERTYPE_NPC) == false)
					{
						switch (cAttackerSA) {
						case 5:
							if (m_pNpcList[sTargetH]->m_iPoisonLevel < 15) m_pNpcList[sTargetH]->m_iPoisonLevel = 15; //15
							SetPoisonFlag(sTargetH, DEF_OWNERTYPE_NPC, true);
							break;
						case 6:
							if (m_pNpcList[sTargetH]->m_iPoisonLevel < 40) m_pNpcList[sTargetH]->m_iPoisonLevel = 40; //40
							SetPoisonFlag(sTargetH, DEF_OWNERTYPE_NPC, true);
							break;
						case 61:
							if (m_pNpcList[sTargetH]->m_iPoisonLevel < iAttackerSAvalue) m_pNpcList[sTargetH]->m_iPoisonLevel = iAttackerSAvalue;
							SetPoisonFlag(sTargetH, DEF_OWNERTYPE_NPC, true);
							break;
						}
					}
				}
			}

			// Snoopy: Added possibilty of activated weapons effect on npcs
			if (   (cAttackerType == DEF_OWNERTYPE_PLAYER) 
				&& (m_pClientList[sAttackerH] != 0) 
				&& (m_pClientList[sAttackerH]->m_bIsSpecialAbilityEnabled == true)) 
			{	switch (m_pClientList[sAttackerH]->m_iSpecialAbilityType) {
				case 0: break;
				case 1: // Xelima weapon, deals float damage 1 in 3 times
					if (iDice(1,3)==2) 
					{	iAP_SM *= 2;
						iAP_L *= 2;
					}
					break;
				case 2: // Ice weapon, will ice npcs						
					if (   (bCheckResistingIceSuccess(0, sTargetH, DEF_OWNERTYPE_NPC, 50) == false) 
							&& (m_pNpcList[sTargetH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_ICE ] == 0)) 
					{	m_pNpcList[sTargetH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_ICE ] = 2;
						SetIceFlag(sTargetH, DEF_OWNERTYPE_NPC, true);
						bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + 20000, 
							sTargetH, DEF_OWNERTYPE_NPC, 0, 0, 0, 1, 0, 0);
					}
					break;
				case 3: // Medusa sword - hold for 10 sec
					if (m_pNpcList[sTargetH]->m_cMagicLevel < 6)
					{	// Not on magic npcs,
					    // para for 10 sec
						if (m_pNpcList[sTargetH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_HOLDOBJECT ] == 0) 
						{	m_pNpcList[sTargetH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_HOLDOBJECT ] = 3;
							bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_HOLDOBJECT, dwTime + 10000, 
								sTargetH, DEF_OWNERTYPE_NPC, 0, 0, 0, 3, 0, 0);
							// May be canceled due to damage later...					
					}	}
					break;
				case 5: // Vampiric weapon 1/8 of given HP
					if (m_pNpcList[sTargetH]->m_cSize == 0)
						m_pClientList[sAttackerH]->m_iHP += iAP_SM / 8;
					else m_pClientList[sAttackerH]->m_iHP += iAP_L / 8;
					if (iGetMaxHP(sAttackerH, true) < m_pClientList[sAttackerH]->m_iHP) // BloodEffect limite le regen
						m_pClientList[sAttackerH]->m_iHP = iGetMaxHP(sAttackerH, true);
					SendNotifyMsg(0, sAttackerH, DEF_NOTIFY_HP, 0, 0, 0, 0);
					break;
			}	}

			if (m_pNpcList[sTargetH]->m_cSize == 0)	
				iDamage = iAP_SM;					
			else iDamage = iAP_L;

			// PA monsters
			if (m_pNpcList[sTargetH]->m_iAbsDamage < 0) // negative value: PA mobs
			{	dTmp1 = (float)iDamage;
				dTmp2 = (float)(abs(m_pNpcList[sTargetH]->m_iAbsDamage))/100.0f;
				dTmp3 = dTmp1 * dTmp2;										
				if (   (cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH] != 0) 
					&& (m_pNpcList[sTargetH]->m_cSpecialAbility == 3)
					&& (m_pClientList[sAttackerH]->m_sUsingWeaponSkill == 14)) 	// PA counts for 1/2 vs Hammers but not against natural PA
				{	dTmp2 = dTmp1 - (dTmp3/2);	
				}else dTmp2 = dTmp1 - dTmp3;	
				iDamage = (int)dTmp2;
				if (iDamage < 0) iDamage = 1;
			}

			if ((cAttackerSA == 2) && (m_pNpcList[sTargetH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_PROTECT ] != 0)) {
					switch(m_pNpcList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT]) {
					case 1:
						SetProtectionFromArrowFlag(sTargetH, DEF_OWNERTYPE_NPC, false);
						break;
					case 2:
					case 5:
						SetMagicProtectionFlag(sTargetH, DEF_OWNERTYPE_NPC, false);
						break;
					case 3:
					case 4:
						SetDefenseShieldFlag(sTargetH, DEF_OWNERTYPE_NPC, false);
						break;
					}
				m_pNpcList[sTargetH]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT] = 0;
				bRemoveFromDelayEventList(sTargetH, DEF_OWNERTYPE_NPC, DEF_MAGICTYPE_PROTECT);					  
			}

			// Magic Weapons - Add Magical Damages
			if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH] != 0)) {
				float iInitial_AP_SM, iWeaponDamage, iTotalMagicDamage;
				int iMagDamage;
				short sManaCost;
				UINT32 dwType1, dwType2, dwValue1, dwValue2;

				if(m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND] != -1){
					sItemIndex = m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND];
				}
				else if(m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND] != -1){
					sItemIndex = m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND];
				}
				else sItemIndex = -1;

				if(sItemIndex != -1){
					if ((m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x00F00000) != 0) {
						dwType1  = (m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x00F00000) >> 20;  
						dwValue1 = (m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x000F0000) >> 16;
						dwType2  = (m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x0000F000) >> 12;  
						dwValue2 = (m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x00000F00) >> 8;
					}
					if(dwType1 == 15){
						sManaCost = (short)dwValue2*2;
						if(m_pClientList[sAttackerH]->m_iMP >= sManaCost){
							iWeaponDamage = (float)dwValue2*7;
							iInitial_AP_SM = (float)iDamage;
							iMagDamage = (m_pClientList[sAttackerH]->m_iMag)/20;
							iTotalMagicDamage = ((iInitial_AP_SM/100.0f)*(iWeaponDamage+100.0f))+iMagDamage;
							iDamage = (int)iTotalMagicDamage;
							m_pClientList[sAttackerH]->m_iMP -= sManaCost;
							SendNotifyMsg(0, sAttackerH, DEF_NOTIFY_MP, 0, 0, 0, 0);
						}
					}
				}
			}
			// End of Magic Weapons 

			switch (m_pNpcList[sTargetH]->m_cActionLimit) {
			case 0:
			case 3:
			case 5:
				m_pNpcList[sTargetH]->m_iHP -= iDamage;
				//Magn0S:: Add the code showdmg
				if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH] != 0)) {
					if (m_pClientList[sAttackerH]->bShowDmg == true) {
						ZeroMemory(cDamageMod, sizeof(cDamageMod));
						wsprintf(cDamageMod, "You did %d Damage to %s", iDamage, m_pNpcList[sTargetH]->m_cNpcName);
						SendNotifyMsg(0, sAttackerH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, cDamageMod);
					}
				}
				break;
			}

			if (m_pNpcList[sTargetH]->m_iHP <= 0) {
				NpcKilledHandler(sAttackerH, cAttackerType, sTargetH, iDamage);
				bKilled = true;
				iKilledDice = m_pNpcList[sTargetH]->m_iHitDice;
			}
			else {
				if ((m_pNpcList[sTargetH]->m_sType != 21) && (m_pNpcList[sTargetH]->m_sType != 55) && (m_pNpcList[sTargetH]->m_sType != 56)
					&& (m_pNpcList[sTargetH]->m_cSide == cAttackerSide)) goto CAE_SKIPCOUNTERATTACK;

				// Tammed mobs
				if ((m_pNpcList[sTargetH]->m_dwTamingTime != 0) && (m_pNpcList[sTargetH]->m_iSummonControlMode == 1)) goto CAE_SKIPCOUNTERATTACK;

				if (m_pNpcList[sTargetH]->m_cActionLimit != 0) goto CAE_SKIPCOUNTERATTACK;
				if (m_pNpcList[sTargetH]->m_bIsPermAttackMode == true) goto CAE_SKIPCOUNTERATTACK;
				if ((m_pNpcList[sTargetH]->m_bIsSummoned == true) && (m_pNpcList[sTargetH]->m_iSummonControlMode == 1)) goto CAE_SKIPCOUNTERATTACK;
				if (m_pNpcList[sTargetH]->m_sType == 51) goto CAE_SKIPCOUNTERATTACK;

				if (iDice(1,3) == 2) {
					if (m_pNpcList[sTargetH]->m_cBehavior == DEF_BEHAVIOR_ATTACK) {
						tX = tY = 0;
						switch (m_pNpcList[sTargetH]->m_cTargetType) {
							case DEF_OWNERTYPE_PLAYER:
								if (m_pClientList[m_pNpcList[sTargetH]->m_iTargetIndex] != 0) {
									tX = m_pClientList[m_pNpcList[sTargetH]->m_iTargetIndex]->m_sX;
									tY = m_pClientList[m_pNpcList[sTargetH]->m_iTargetIndex]->m_sY;
								}
								break;

							case DEF_OWNERTYPE_NPC:
								if (m_pNpcList[m_pNpcList[sTargetH]->m_iTargetIndex] != 0) {
									tX = m_pNpcList[m_pNpcList[sTargetH]->m_iTargetIndex]->m_sX;
									tY = m_pNpcList[m_pNpcList[sTargetH]->m_iTargetIndex]->m_sY;
								}
								break;
							}

						iDst1 = (m_pNpcList[sTargetH]->m_sX - tX)*(m_pNpcList[sTargetH]->m_sX - tX) + (m_pNpcList[sTargetH]->m_sY - tY)*(m_pNpcList[sTargetH]->m_sY - tY);

						tX = tY = 0;
						switch (cAttackerType) {
							case DEF_OWNERTYPE_PLAYER:
								if (m_pClientList[sAttackerH] != 0) {
									tX = m_pClientList[sAttackerH]->m_sX;
									tY = m_pClientList[sAttackerH]->m_sY;
								}
								break;

							case DEF_OWNERTYPE_NPC:
								if (m_pNpcList[sAttackerH] != 0) {
									tX = m_pNpcList[sAttackerH]->m_sX;
									tY = m_pNpcList[sAttackerH]->m_sY;
								}
								break;
						}

						iDst2 = (m_pNpcList[sTargetH]->m_sX - tX)*(m_pNpcList[sTargetH]->m_sX - tX) + (m_pNpcList[sTargetH]->m_sY - tY)*(m_pNpcList[sTargetH]->m_sY - tY);

						if (iDst2 <= iDst1) {
							m_pNpcList[sTargetH]->m_cBehavior          = DEF_BEHAVIOR_ATTACK;
							m_pNpcList[sTargetH]->m_sBehaviorTurnCount = 0;		
							m_pNpcList[sTargetH]->m_iTargetIndex = sAttackerH;
							m_pNpcList[sTargetH]->m_cTargetType  = cAttackerType;
						}
					}
					else {
						m_pNpcList[sTargetH]->m_cBehavior          = DEF_BEHAVIOR_ATTACK;
						m_pNpcList[sTargetH]->m_sBehaviorTurnCount = 0;		
						m_pNpcList[sTargetH]->m_iTargetIndex = sAttackerH;
						m_pNpcList[sTargetH]->m_cTargetType  = cAttackerType;
					}
				}

CAE_SKIPCOUNTERATTACK:;

				if ((iDice(1,3) == 2) && (m_pNpcList[sTargetH]->m_cActionLimit == 0))
					m_pNpcList[sTargetH]->m_dwTime = dwTime;

				if (cAttackerType == DEF_OWNERTYPE_PLAYER) 
					sAttackerWeapon = ((m_pClientList[sAttackerH]->m_sAppr2 & 0x0FF0) >> 4);
				else sAttackerWeapon = 1;

				if (((wWeaponType < 40) || ((wWeaponType > 55) && (wWeaponType <= 60))) && (m_pNpcList[sTargetH]->m_cActionLimit == 4)) {
 					if (sTgtX == sAtkX) {
						if (sTgtY == sAtkY)     goto CAE_SKIPDAMAGEMOVE2;
						else if (sTgtY > sAtkY) cDamageMoveDir = 5;
						else if (sTgtY < sAtkY) cDamageMoveDir = 1;
					}
					else if (sTgtX > sAtkX) {
						if (sTgtY == sAtkY)     cDamageMoveDir = 3;
						else if (sTgtY > sAtkY) cDamageMoveDir = 4;
						else if (sTgtY < sAtkY) cDamageMoveDir = 2;
					}
					else if (sTgtX < sAtkX) {
						if (sTgtY == sAtkY)     cDamageMoveDir = 7;
						else if (sTgtY > sAtkY) cDamageMoveDir = 6;
						else if (sTgtY < sAtkY) cDamageMoveDir = 8;							
					}

					dX = m_pNpcList[sTargetH]->m_sX + _tmp_cTmpDirX[cDamageMoveDir];
					dY = m_pNpcList[sTargetH]->m_sY + _tmp_cTmpDirY[cDamageMoveDir];

					if (m_pMapList[m_pNpcList[sTargetH]->m_cMapIndex]->bGetMoveable(dX, dY, 0) == false) {
						cDamageMoveDir = iDice(1,8);
						dX = m_pNpcList[sTargetH]->m_sX + _tmp_cTmpDirX[cDamageMoveDir];
						dY = m_pNpcList[sTargetH]->m_sY + _tmp_cTmpDirY[cDamageMoveDir];

						if (m_pMapList[m_pNpcList[sTargetH]->m_cMapIndex]->bGetMoveable(dX, dY, 0) == false) goto CAE_SKIPDAMAGEMOVE2;
					}

					m_pMapList[m_pNpcList[sTargetH]->m_cMapIndex]->ClearOwner(5, sTargetH, DEF_OWNERTYPE_NPC, m_pNpcList[sTargetH]->m_sX, m_pNpcList[sTargetH]->m_sY);
					m_pMapList[m_pNpcList[sTargetH]->m_cMapIndex]->SetOwner(sTargetH, DEF_OWNERTYPE_NPC, dX, dY);
					m_pNpcList[sTargetH]->m_sX   = dX;
					m_pNpcList[sTargetH]->m_sY   = dY;
					m_pNpcList[sTargetH]->m_cDir = cDamageMoveDir;

					SendEventToNearClient_TypeA(sTargetH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTMOVE, 0, 0, 0);

					dX = m_pNpcList[sTargetH]->m_sX + _tmp_cTmpDirX[cDamageMoveDir];
					dY = m_pNpcList[sTargetH]->m_sY + _tmp_cTmpDirY[cDamageMoveDir];

					if (m_pMapList[m_pNpcList[sTargetH]->m_cMapIndex]->bGetMoveable(dX, dY, 0) == false) {
						cDamageMoveDir = iDice(1,8);
						dX = m_pNpcList[sTargetH]->m_sX + _tmp_cTmpDirX[cDamageMoveDir];
						dY = m_pNpcList[sTargetH]->m_sY + _tmp_cTmpDirY[cDamageMoveDir];
						if (m_pMapList[m_pNpcList[sTargetH]->m_cMapIndex]->bGetMoveable(dX, dY, 0) == false) goto CAE_SKIPDAMAGEMOVE2;
					}

					m_pMapList[m_pNpcList[sTargetH]->m_cMapIndex]->ClearOwner(5, sTargetH, DEF_OWNERTYPE_NPC, m_pNpcList[sTargetH]->m_sX, m_pNpcList[sTargetH]->m_sY);
					m_pMapList[m_pNpcList[sTargetH]->m_cMapIndex]->SetOwner(sTargetH, DEF_OWNERTYPE_NPC, dX, dY);
					m_pNpcList[sTargetH]->m_sX   = dX;
					m_pNpcList[sTargetH]->m_sY   = dY;
					m_pNpcList[sTargetH]->m_cDir = cDamageMoveDir;

					SendEventToNearClient_TypeA(sTargetH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTMOVE, 0, 0, 0);

				}
				else {
					SendEventToNearClient_TypeA(sTargetH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTDAMAGE, iDamage, sAttackerWeapon, 0);
				}
CAE_SKIPDAMAGEMOVE2:;
				if (m_pNpcList[sTargetH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_HOLDOBJECT ] == 1) {
					m_pNpcList[sTargetH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_HOLDOBJECT ] = 0;
					bRemoveFromDelayEventList(sTargetH, DEF_OWNERTYPE_NPC, DEF_MAGICTYPE_HOLDOBJECT);
				}
				else if (m_pNpcList[sTargetH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_HOLDOBJECT ] == 2) {
					if ((m_pNpcList[sTargetH]->m_iHitDice > 50) && (iDice(1, 10) == 5)) {
						m_pNpcList[sTargetH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_HOLDOBJECT ] = 0;
						bRemoveFromDelayEventList(sTargetH, DEF_OWNERTYPE_NPC, DEF_MAGICTYPE_HOLDOBJECT);
					}
				}

				if ( (m_pNpcList[sTargetH]->m_iNoDieRemainExp > 0) && (m_pNpcList[sTargetH]->m_bIsSummoned != true) && 
					(cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH] != 0) ) {
						if (m_pNpcList[sTargetH]->m_iNoDieRemainExp > iDamage) {
							iExp = iDamage;
							m_pNpcList[sTargetH]->m_iNoDieRemainExp -= iDamage;
						}
						else {
							iExp = m_pNpcList[sTargetH]->m_iNoDieRemainExp;
							m_pNpcList[sTargetH]->m_iNoDieRemainExp = 0;
						}

						if (m_pClientList[sAttackerH]->m_iAddExp != 0) {
							dTmp1 = (float)m_pClientList[sAttackerH]->m_iAddExp;
							dTmp2 = (float)iExp;
							dTmp3 = (dTmp1/100.0f)*dTmp2;
							iExp += (UINT32)dTmp3;
						}

						if (m_bIsCrusadeMode == true) iExp = iExp/3;

						if (m_pClientList[sAttackerH]->m_iLevel > 100 ) {
							switch (m_pNpcList[sTargetH]->m_sType) {
							case 55:
							case 56:
								iExp = 0;
								break;
							default: break;
							}
						}
					}
			}
			break;
		}

		if (cAttackerType == DEF_OWNERTYPE_PLAYER)
		{
			if (m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND] != -1)
				sWeaponIndex = m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND];
			else sWeaponIndex = m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND];
			// SNOOPY: ADDED support for Dual-Wielding on 4th Combo Attack
						// And add MainGauche Attack on succesfull 4th combo Attacks
			sItemIndex = m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_LHAND];
			if ((sItemIndex != -1)
				&& (m_pClientList[sAttackerH]->m_iComboAttackCount == 4)
				&& (m_pClientList[sAttackerH]->m_pItemList[sItemIndex] != 0)) {
				iWeaponSkill = _iGetWeaponSkillType(sAttackerH);
				if ((m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 10) // MainGauche
					&& ((iWeaponSkill == 7) || (iWeaponSkill == 9))) // Only SS or Fencers										 
				{
					iExp += iCalculateAttackEffect(sTargetH, cTargetType, sAttackerH, DEF_OWNERTYPE_PLAYER, tdX, tdY, 2, false, false, false, true);
				}
			}
				if ((sWeaponIndex != -1) && (bArrowUse != true)) {
					// No skill increase if bArrowUse
					if ((m_pClientList[sAttackerH]->m_pItemList[sWeaponIndex] != 0)
						&& (m_pClientList[sAttackerH]->m_pItemList[sWeaponIndex]->m_sIDnum != 231))  //  Not for PickAxe
					{
						if (bKilled == false)
							CalculateSSN_ItemIndex(sAttackerH, sWeaponIndex, 1);
						else
						{
							if (m_pClientList[sAttackerH]->m_iHP <= 3)
								CalculateSSN_ItemIndex(sAttackerH, sWeaponIndex, iDice(1, iKilledDice) * 2);
							else CalculateSSN_ItemIndex(sAttackerH, sWeaponIndex, iDice(1, iKilledDice));
						}
					}
					// Weapon's wep life off
					if ((m_pClientList[sAttackerH]->m_pItemList[sWeaponIndex] != 0)
						&& (m_pClientList[sAttackerH]->m_pItemList[sWeaponIndex]->m_wMaxLifeSpan != 0))
					{
						iWepLifeOff = 1;
						if (((wWeaponType >= 1) && (wWeaponType < 40)) || ((wWeaponType > 55) && (wWeaponType <= 60)))
						{
							switch (m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_cWhetherStatus) {
							case 0:	break;
							case 1:	if (iDice(1, 3) == 1) iWepLifeOff++; break;
							case 2:	if (iDice(1, 2) == 1) iWepLifeOff += iDice(1, 2); break;
							case 3:	if (iDice(1, 2) == 1) iWepLifeOff += iDice(1, 3); break;
							}
						}
						if (m_pClientList[sAttackerH]->m_cSide != 0) {
							if (m_pClientList[sAttackerH]->m_pItemList[sWeaponIndex]->m_wCurLifeSpan < iWepLifeOff)
								m_pClientList[sAttackerH]->m_pItemList[sWeaponIndex]->m_wCurLifeSpan = 0;
							else
							{
								m_pClientList[sAttackerH]->m_pItemList[sWeaponIndex]->m_wCurLifeSpan -= iWepLifeOff;
								SendNotifyMsg(0, sAttackerH, DEF_NOTIFY_CURLIFESPAN, sWeaponIndex, m_pClientList[sAttackerH]->m_pItemList[sWeaponIndex]->m_wCurLifeSpan, 0, 0);
							}
						}
						if (m_pClientList[sAttackerH]->m_pItemList[sWeaponIndex]->m_wCurLifeSpan <= 0) {
							SendNotifyMsg(0, sAttackerH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[sAttackerH]->m_pItemList[sWeaponIndex]->m_cEquipPos, sWeaponIndex, 0, 0);
							ReleaseItemHandler(sAttackerH, sWeaponIndex, true);
						}
					}
				}
				else
				{
					if (wWeaponType == 0)  // Open-Hand Skill increase
					{
						CalculateSSN_SkillIndex(sAttackerH, 5, 1);
					}
				}
			
		}
	}else  // Missing the target !
	{	if (cAttackerType == DEF_OWNERTYPE_PLAYER) {
			m_pClientList[sAttackerH]->m_iComboAttackCount = 0;
			//SNOOPY: Adding MainGauche Attack
			sItemIndex = m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_LHAND];
			if ((sItemIndex != -1)
				&& (m_pClientList[sAttackerH]->m_pItemList[sItemIndex] != 0)) {				
				iWeaponSkill = _iGetWeaponSkillType(sAttackerH);
				if ((m_pClientList[sAttackerH]->m_pItemList[sItemIndex]->m_sIDnum == 10) // MainGauche
					&& ((iWeaponSkill == 7)||(iWeaponSkill == 9))) // Only SS or Fencers									 
				{	iExp = iCalculateAttackEffect(sTargetH, cTargetType, sAttackerH, DEF_OWNERTYPE_PLAYER, tdX, tdY, 2, false, false, false, true);
	}	}	}	}
	return iExp;
}

bool CGame::_bCheckCharacterData(int iClientH)
{
 int i;
 int iTotalPoints;

	for (i = 0; i < DEF_MAXBANNED; i++) {
		 if (strlen(m_stBannedList[i].m_cBannedIPaddress) == 0) break; //No more GM's on list
		 if ((strlen(m_stBannedList[i].m_cBannedIPaddress)) == (strlen(m_pClientList[iClientH]->m_cIPaddress))) {
			 if (memcmp(m_stBannedList[i].m_cBannedIPaddress, m_pClientList[iClientH]->m_cIPaddress, strlen(m_pClientList[iClientH]->m_cIPaddress)) == 0) {
				 wsprintf(G_cTxt, "Client Rejected: Banned: (%s)", m_pClientList[iClientH]->m_cIPaddress);
				 PutLogList(G_cTxt);
				 return false;
			 }
		 }
	}

	if (m_pClientList[iClientH]->m_iAdminUserLevel > 0) return true;

	if (m_pClientList[iClientH]->m_iStr > m_sCharStatLimit) {
        m_pClientList[iClientH]->m_iStr = m_sCharStatLimit;
	}
	if (m_pClientList[iClientH]->m_iVit > m_sCharStatLimit) {
		m_pClientList[iClientH]->m_iVit = m_sCharStatLimit;
	}
	if (m_pClientList[iClientH]->m_iDex > m_sCharStatLimit) {
		m_pClientList[iClientH]->m_iDex = m_sCharStatLimit;
	}
	if (m_pClientList[iClientH]->m_iInt > m_sCharStatLimit) {
		m_pClientList[iClientH]->m_iInt = m_sCharStatLimit;
	}
	if (m_pClientList[iClientH]->m_iMag > m_sCharStatLimit) {
		m_pClientList[iClientH]->m_iMag = m_sCharStatLimit;
	}
	if (m_pClientList[iClientH]->m_iCharisma > m_sCharStatLimit) {
		m_pClientList[iClientH]->m_iCharisma = m_sCharStatLimit;
	}

	iTotalPoints = 0;
	for (i = 0; i <	DEF_MAXSKILLTYPE; i++) 
		iTotalPoints += m_pClientList[iClientH]->m_cSkillMastery[i];
		if (iTotalPoints-21 > m_sCharSkillLimit) {
		wsprintf(G_cTxt, "Packet Editing: (%s) Player: (%s) - has more than allowed skill points (%d).", m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName, iTotalPoints);
		PutHackLogFileList(G_cTxt);
		PutLogList(G_cTxt);
		return false;
	}

	if ((strcmp(m_pClientList[iClientH]->m_cLocation, "elvine") != 0) && 
		(strcmp(m_pClientList[iClientH]->m_cLocation, "elvhunter") != 0) && 
		(strcmp(m_pClientList[iClientH]->m_cLocation, "arehunter") != 0) && 
		(strcmp(m_pClientList[iClientH]->m_cLocation, "aresden") != 0) &&
		(m_pClientList[iClientH]->m_iLevel >= 20)) {
		wsprintf(G_cTxt, "Traveller Hack: (%s) Player: (%s) is a traveller and is greater than level 19.", m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName);
		PutHackLogFileList(G_cTxt);
		PutLogList(G_cTxt);
		return false;
	}

	if (m_pClientList[iClientH]->m_iLevel > m_iPlayerMaxLevel) {
		m_pClientList[iClientH]->m_iLevel = m_iPlayerMaxLevel;
	}

	if (m_pClientList[iClientH]->m_iExp < 0) {
		m_pClientList[iClientH]->m_iExp = m_iLevelExpTable[m_pClientList[iClientH]->m_iLevel];
	}

	if (m_pClientList[iClientH]->m_iHP > iGetMaxHP(iClientH)) {
		m_pClientList[iClientH]->m_iHP = iGetMaxHP(iClientH);
	}

	if (m_pClientList[iClientH]->m_iMP > iGetMaxMP(iClientH)) {
		m_pClientList[iClientH]->m_iMP = iGetMaxMP(iClientH);
	}

	if (m_pClientList[iClientH]->m_iSP > iGetMaxSP(iClientH)) {
		m_pClientList[iClientH]->m_iSP = iGetMaxSP(iClientH);
	}

	return true;
}

/*********************************************************************************************************************
**  int iGetPlayerStatus(int iClientH, short sOwnerH)																**
**  DESCRIPTION			:: returns client tile's mistatus															**
**  LAST_UPDATED		:: March 16, 2005; 12:31 PM; Hypnotoad														**
**	RETURN_VALUE		:: int																						**
**  NOTES				::	- not available in 2.03				 													**
**	MODIFICATION		:: N/A																						**
**********************************************************************************************************************/
int CGame::iGetPlayerStatus(int iClientH, short sOwnerH)
{
 int iRet;

	if (m_pClientList[iClientH]->m_cSide != m_pClientList[sOwnerH]->m_cSide) {
		if (iClientH != sOwnerH) {
			iRet = m_pClientList[sOwnerH]->m_iStatus & 0x0F0FFFF7F;
		}
		else iRet = m_pClientList[sOwnerH]->m_iStatus;
	}
	else iRet = m_pClientList[sOwnerH]->m_iStatus;
	return iRet;
}

bool CGame::bReadSettingsConfigFile(char * cFn) 
{ 
   FILE * pFile; 
   HANDLE hFile; 
   UINT32  dwFileSize; 
   char * cp, * token, cReadMode, cTxt[120], cGSMode[16] = ""; 
   char seps[] = "= \t\n"; 
   class CStrTok * pStrTok; 
   short len;

   cReadMode = 0; 

   hFile = CreateFile(cFn, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0); 
   dwFileSize = GetFileSize(hFile, 0); 
   if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile); 

   pFile = fopen(cFn, "rt"); 
   if (pFile == 0) { 
      // Â°Ã”Ã€Ã“Â¼Â­Â¹Ã¶Ã€Ã‡ ÃƒÃŠÂ±Ã¢ÃˆÂ­ Ã†Ã„Ã€ÃÃ€Â» Ã€ÃÃ€Â» Â¼Ã¶ Â¾Ã¸Â´Ã™. 
      PutLogList("(!) Cannot open configuration file."); 
      return false; 
   } 
   else { 
      PutLogList("(!) Reading settings file..."); 
      cp = new char[dwFileSize+2]; 
      ZeroMemory(cp, dwFileSize+2); 
      fread(cp, dwFileSize, 1, pFile); 

      pStrTok = new class CStrTok(cp, seps); 
      token = pStrTok->pGet(); 
      while( token != 0 )   { 

         if (cReadMode != 0) { 
            switch (cReadMode) { 

            case 1: 
               if ((strlen(token) != 0) && (strlen(token) <= 100) && (strlen(token) >= 1)) 
               { 
                  m_iPrimaryDropRate = atoi(token); 
               } 
               else 
               { 
                  wsprintf(cTxt, "(!!!) Primary drop rate(%s) invalid must be between 1 to 100", token); 
                  PutLogList(cTxt); 
				  return false;
               } 
               wsprintf(cTxt, "(*) Primary drop rate: (%d)", m_iPrimaryDropRate); 
               PutLogList(cTxt); 
               cReadMode = 0; 
               break; 

            case 2: 
               if ((strlen(token) != 0) && (strlen(token) <= 100) && (strlen(token) >= 1)) 
               { 
                  m_iSecondaryDropRate = atoi(token); 
               } 
               else 
               { 
                  wsprintf(cTxt, "(!!!) Secondary drop rate(%s) invalid must be between 1 to 100", token); 
                  PutLogList(cTxt);
				  return false;
               } 
               wsprintf(cTxt, "(*) Secondary drop rate: (%d)", m_iSecondaryDropRate); 
               PutLogList(cTxt); 
               cReadMode = 0; 
               break; 

            case 3: 
               if ((memcmp(token, "classic", 7) == 0) || (memcmp(token, "CLASSIC", 7) == 0)) 
               { 
                  m_bEnemyKillMode = false; 
                  PutLogList("(*) Enemy Kill Mode: CLASSIC");
               } 
               else if ((memcmp(token, "deathmatch", 10) == 0) || (memcmp(token, "DEATHMATCH", 10) == 0)) 
               { 
                  m_bEnemyKillMode = true; 
                  PutLogList("(*) Enemy Kill Mode: DEATHMATCH");
               } 
               else 
               { 
                  wsprintf(cTxt, "(!!!) ENEMY-KILL-MODE: (%s) must be either DEATHMATCH or CLASSIC", token); 
                  PutLogList(cTxt); 
                  return false; 
               } 
               cReadMode = 0; 
               break; 

            case 4: 
               m_iEnemyKillAdjust = atoi(token); 
               wsprintf(cTxt, "(*) Player gains (%d) EK.", m_iEnemyKillAdjust); 
               PutLogList(cTxt); 
               cReadMode = 0; 
               break; 

            case 5: 
               if ((memcmp(token, "on", 2) == 0) || (memcmp(token, "ON", 2) == 0)) 
               { 
                  m_bAdminSecurity = true; 
                  PutLogList("(*) Administator actions limited through security.");
               } 
               if ((memcmp(token, "off", 3) == 0) || (memcmp(token, "OFF", 3) == 0)) 
               { 
                  m_bAdminSecurity = false; 
                  PutLogList("(*) Administator vulnerability ignored.");
               } 

               cReadMode = 0; 
               break; 

            case 6: 
               m_sRaidTimeMonday = atoi(token); 
               if (m_sRaidTimeMonday == 0) m_sRaidTimeMonday = 3; 
               wsprintf(cTxt, "(*) Monday Raid Time : (%d) minutes", m_sRaidTimeMonday); 
               PutLogList(cTxt); 
               cReadMode = 0; 
               break; 

            case 7: 
               m_sRaidTimeTuesday = atoi(token); 
               if (m_sRaidTimeTuesday == 0) m_sRaidTimeTuesday = 3; 
               wsprintf(cTxt, "(*) Tuesday Raid Time : (%d) minutes", m_sRaidTimeTuesday); 
               PutLogList(cTxt); 
               cReadMode = 0; 
               break; 

            case 8: 
               m_sRaidTimeWednesday = atoi(token); 
               if (m_sRaidTimeWednesday == 0) m_sRaidTimeWednesday = 3; 
               wsprintf(cTxt, "(*) Wednesday Raid Time : (%d) minutes", m_sRaidTimeWednesday); 
               PutLogList(cTxt); 
               cReadMode = 0; 
               break; 

            case 9: 
               m_sRaidTimeThursday = atoi(token); 
               if (m_sRaidTimeThursday == 0) m_sRaidTimeThursday = 3; 
               wsprintf(cTxt, "(*) Thursday Raid Time : (%d) minutes", m_sRaidTimeThursday); 
               PutLogList(cTxt); 
               cReadMode = 0; 
               break; 

            case 10: 
               m_sRaidTimeFriday = atoi(token); 
               if (m_sRaidTimeFriday == 0) m_sRaidTimeFriday = 10; 
               wsprintf(cTxt, "(*) Friday Raid Time : (%d) minutes", m_sRaidTimeFriday); 
               PutLogList(cTxt); 
               cReadMode = 0; 
               break; 

            case 11: 
               m_sRaidTimeSaturday = atoi(token); 
               if (m_sRaidTimeSaturday == 0) m_sRaidTimeSaturday = 45; 
               wsprintf(cTxt, "(*) Saturday Raid Time : (%d) minutes", m_sRaidTimeSaturday); 
               PutLogList(cTxt); 
               cReadMode = 0; 
               break; 

            case 12: 
               m_sRaidTimeSunday = atoi(token); 
               if (m_sRaidTimeSunday == 0) m_sRaidTimeSunday = 60; 
               wsprintf(cTxt, "(*) Sunday Raid Time : (%d) minutes", m_sRaidTimeSunday); 
               PutLogList(cTxt); 
               cReadMode = 0; 
               break; 
		
			case 13:
				if ((memcmp(token, "player", 6) == 0) || (memcmp(token, "PLAYER", 6) == 0))
				{
					m_bLogChatOption = 1;
					PutLogList("(*) Logging Player Chat.");
				}
				if ((memcmp(token, "gm", 2) == 0) || (memcmp(token, "GM", 2) == 0))
				{
					m_bLogChatOption = 2;
					PutLogList("(*) Logging GM Chat.");
				}
				if ((memcmp(token, "all", 3) == 0) || (memcmp(token, "ALL", 3) == 0))
				{
					m_bLogChatOption = 3;
					PutLogList("(*) Logging All Chat.");
				}
				if ((memcmp(token, "none", 4) == 0) || (memcmp(token, "NONE", 4) == 0))
				{
					m_bLogChatOption = 4;
					PutLogList("(*) Not Logging Any Chat.");
				}
				cReadMode = 0;
				break;

            case 14: 
               m_iSummonGuildCost = atoi(token); 
               wsprintf(cTxt, "(*) Summoning guild costs (%d) gold", m_iSummonGuildCost); 
               PutLogList(cTxt); 
               cReadMode = 0; 
               break;
 
			case 15: 
               m_sSlateSuccessRate = atoi(token); 
               wsprintf(cTxt, "(*) Slate Success Rate (%d) percent", m_sSlateSuccessRate); 
               PutLogList(cTxt); 
			   if (m_sSlateSuccessRate == 0) m_sSlateSuccessRate = 14; 
               cReadMode = 0; 
               break;

			case 16: 
               m_sCharStatLimit = atoi(token); 
               if (m_sCharStatLimit == 0) m_sCharStatLimit = 200; 
               cReadMode = 0;
               break;

			case 17: 
               m_sCharSkillLimit = atoi(token); 
               if (m_sCharSkillLimit == 0) m_sCharSkillLimit = 700; 
               cReadMode = 0;
               break;

			case 18: 
				m_iMaxAbs = atoi(token);
               cReadMode = 0; 
               break;

			case 19: 
               ZeroMemory(m_cSecurityNumber, sizeof(m_cSecurityNumber));
			   len = (short)strlen(token);
			   //if(len > 10) len = 10;
			   memcpy(m_cSecurityNumber, token, len);
               PutLogList("(*) Security number memorized!");
               cReadMode = 0; 
               break;

			case 20: 
               m_iPlayerMaxLevel = atoi(token); 
               cReadMode = 0;
               break;

			case 21: 
				m_iMaxResist = atoi(token);
					cReadMode = 0;
					break;

			case 22: // Coded by Mkintosh
					m_iGoldRate = atoi(token);
					cReadMode = 0;
					break;

			case 23:
				// centu - added exp rate setting
					m_iExpSetting = atoi(token);
					wsprintf(cTxt, "(*) EXP Multiplier (x%d)", m_iExpSetting);
					PutLogList(cTxt);
					cReadMode = 0;
					break;

				case 24: 
					m_iGuildCost = atoi(token); 
					wsprintf(cTxt, "(*) Creating guild costs (%d) gold", m_iGuildCost); 
					PutLogList(cTxt); 
					cReadMode = 0; 
					break;

			case 25: 
				
					if ((strlen(token) < 1) || (strlen(token) > 100)) 
					{ 
						PutLogList("(!!!) Rare drop rate invalid must be between 1 to 100");
						return false; 
					} 
					else 
					{ 
						m_iRareDropRate = atoi(token);
					} 
					wsprintf(cTxt, "(*) Rare drop rate: (%d)", m_iRareDropRate); 
					PutLogList(cTxt); 
					cReadMode = 0; 
					break;

			case 26: 
               
				// centu - added clear map time to settings
				if (memcmp(token, "1", 1) == 0)
				{
					m_iClearMapTime = 3600;
					PutLogList("(*) Clear map timed to 1 hour."); 
				}
				else if (memcmp(token, "2", 1) == 0)
				{
					m_iClearMapTime = 7200;
					PutLogList("(*) Clear map timed to 2 hours."); 
				}
				else if (memcmp(token, "30", 2) == 0)
				{
					m_iClearMapTime = 1800;
					PutLogList("(*) Clear map timed to 30 minutes."); 
				}
				else if (memcmp(token, "15", 2) == 0)
				{
					m_iClearMapTime = 900;
					PutLogList("(*) Clear map timed to 15 minutes.");
				}
               
               cReadMode = 0; 
               break;

			case 27:
				m_iRatingAdjust = atoi(token);
				wsprintf(cTxt, "(*) Player gains (%d) REP.", m_iRatingAdjust);
				PutLogList(cTxt);
				cReadMode = 0;
				break;

			case 28:
				m_iDKCost = atoi(token);
				wsprintf(cTxt, "(*) DK Set costs (%d) gold", m_iDKCost);
				PutLogList(cTxt);
				cReadMode = 0;
				break;
				
			case 29:
				m_sHeldenianFinish = atoi(token);
				cReadMode = 0;
				break;
			case 30:
				m_sCrusadeFinish = atoi(token);
				cReadMode = 0;
				break;
			case 31:
				m_sApocalypseFinish = atoi(token);
				cReadMode = 0;
				break;
			case 32:
				m_sDeathmatchFinish = atoi(token);
				cReadMode = 0;
				break;
			case 33:
				m_sCTF_Finish = atoi(token);
				cReadMode = 0;
				break;
			case 34:
				m_sDropFinish = atoi(token);
				cReadMode = 0;
				break;
			case 35:
				m_sCityTeleportFinish = atoi(token);
				cReadMode = 0;
				break;
			case 36:
				m_sShinningFinish = atoi(token);
				cReadMode = 0;
				break;
			case 37:
				m_sCandyFinish = atoi(token);
				cReadMode = 0;
				break;
			case 38:
				m_sTeamArenaFinish = atoi(token);
				cReadMode = 0;
				break;
			case 39:
				m_sRevelationFinish = atoi(token);
				cReadMode = 0;
				break;

			case 40:
				m_iFragileDropRate = atoi(token);
				cReadMode = 0;
				break;
			case 41:
				m_iMaxRec = atoi(token);
				cReadMode = 0;
				break;
			case 42:
				m_iMaxHPCrit = atoi(token);
				cReadMode = 0;
				break;

			case 43:
				m_sJailTime = atoi(token);
				cReadMode = 0;
				break;

			case 44:
				if ((memcmp(token, "on", 2) == 0) || (memcmp(token, "ON", 2) == 0))
				{
					m_bSQLMode = true;
					PutLogList("(*) Server is connected to SQL database.");
				}
				if ((memcmp(token, "off", 3) == 0) || (memcmp(token, "OFF", 3) == 0))
				{
					m_bSQLMode = false;
					PutLogList("(*) Server is working with TXT files.");
				}

				cReadMode = 0;
				break;

			case 45:
				if ((memcmp(token, "on", 2) == 0) || (memcmp(token, "ON", 2) == 0))
				{
					m_bAntiHackMode = true;
					PutLogList("(*) Server have anti-hacks enabled.");
				}
				if ((memcmp(token, "off", 3) == 0) || (memcmp(token, "OFF", 3) == 0))
				{
					m_bAntiHackMode = false;
					PutLogList("(*) Server doesn't have anti-hacks.");
				}

				cReadMode = 0;
				break;
			case 46:
				if ((memcmp(token, "on", 2) == 0) || (memcmp(token, "ON", 2) == 0))
				{
					m_bBlockLocalConn = true;
					PutLogList("(*) Server blocks local connections.");
				}
				if ((memcmp(token, "off", 3) == 0) || (memcmp(token, "OFF", 3) == 0))
				{
					m_bBlockLocalConn = false;
					PutLogList("(*) Server allows local connections.");
				}

				cReadMode = 0;
				break;
			}
         } 
         else { 
            if (memcmp(token, "primary-drop-rate"      , 17) == 0)   cReadMode = 1; 
            if (memcmp(token, "secondary-drop-rate"      , 19) == 0) cReadMode = 2; 
            if (memcmp(token, "enemy-kill-mode"         , 15) == 0)  cReadMode = 3; 
            if (memcmp(token, "enemy-kill-adjust"      , 17) == 0)   cReadMode = 4; 
            if (memcmp(token, "admin-security"         , 14) == 0)   cReadMode = 5; 
            if (memcmp(token, "monday-raid-time"      , 16) == 0)    cReadMode = 6; 
            if (memcmp(token, "tuesday-raid-time"      , 17) == 0)   cReadMode = 7; 
            if (memcmp(token, "wednesday-raid-time"      , 19) == 0) cReadMode = 8; 
            if (memcmp(token, "thursday-raid-time"      , 18) == 0)  cReadMode = 9; 
            if (memcmp(token, "friday-raid-time"      , 16) == 0)    cReadMode = 10; 
            if (memcmp(token, "saturday-raid-time"      , 18) == 0)  cReadMode = 11; 
            if (memcmp(token, "sunday-raid-time"      , 16) == 0)    cReadMode = 12; 
			if (memcmp(token, "log-chat-settings", 17) == 0)		 cReadMode = 13;
			if (memcmp(token, "summonguild-cost", 16) == 0)			 cReadMode = 14;
			if (memcmp(token, "slate-success-rate", 18) == 0)		 cReadMode = 15;
			if (memcmp(token, "character-stat-limit", 20) == 0)		 cReadMode = 16;
			if (memcmp(token, "character-skill-limit", 21) == 0)	 cReadMode = 17;
			if (memcmp(token, "max-absorption", 14) == 0)			cReadMode = 18;
			if (memcmp(token, "admin-security-code", 19) == 0)		 cReadMode = 19;
			if (memcmp(token, "max-player-level", 16) == 0)			 cReadMode = 20;
			if (memcmp(token, "max-resistance", 14) == 0)			 cReadMode = 21;
			if (memcmp(token, "gold-drop-rate", 14) == 0)			 cReadMode = 22;
			if (memcmp(token, "exp-rate", 8) == 0)					 cReadMode = 23;
			if (memcmp(token, "guild-cost", 10) == 0)				 cReadMode = 24;
			if (memcmp(token, "rare-drop-rate"      , 14) == 0)		 cReadMode = 25;
			if (memcmp(token, "clear-map-time"      , 14) == 0)		 cReadMode = 26;
			if (memcmp(token, "rating-adjust", 13) == 0)		     cReadMode = 27;
			if (memcmp(token, "dk-set-cost", 11) == 0)				 cReadMode = 28;

			if (strcmp(token, "heldenian-finish") == 0)				 cReadMode = 29;
			if (strcmp(token, "crusade-finish") == 0)				 cReadMode = 30;
			if (strcmp(token, "apocalypse-finish") == 0)			 cReadMode = 31;
			if (strcmp(token, "deathmatch-finish") == 0)			 cReadMode = 32;
			if (strcmp(token, "ctf-finish") == 0)					 cReadMode = 33;
			if (strcmp(token, "bag-prot-finish") == 0)				 cReadMode = 34;
			if (strcmp(token, "city-teleport-finish") == 0)			 cReadMode = 35;
			if (strcmp(token, "shinning-finish") == 0)				 cReadMode = 36;
			if (strcmp(token, "candy-finish") == 0)					 cReadMode = 37;
			if (strcmp(token, "team-arena-finish") == 0)			 cReadMode = 38;
			if (strcmp(token, "revelation-finish") == 0)			 cReadMode = 39;

			if (strcmp(token, "fragile-drop-rate") == 0)			 cReadMode = 40;
			if (strcmp(token, "max-recovery") == 0)					cReadMode = 41;
			if (strcmp(token, "max-hpcrit") == 0)					cReadMode = 42;

			if (strcmp(token, "jail-time") == 0)					cReadMode = 43;

			if (strcmp(token, "sql-server-mode") == 0)					cReadMode = 44;
			if (strcmp(token, "anti-hack-mode") == 0)					cReadMode = 45;
			if (strcmp(token, "block-local-conn") == 0)					cReadMode = 46;
		 } 

         token = pStrTok->pGet(); 
      } 

      delete pStrTok; 
      delete[] cp;

	  fclose(pFile);
   } 

   return true; 
} 

void CGame::SendNotifyMsg(int iFromH, int iToH, UINT16 wMsgType, UINT32 sV1, UINT32 sV2, UINT32 sV3, char* pString, UINT32 sV4, UINT32 sV5, UINT32 sV6, UINT32 sV7, UINT32 sV8, UINT32 sV9, char* pString2, UINT32 sV10, UINT32 sV11, UINT32 sV12, UINT32 sV13, UINT32 sV14)
{
	char cData[1000];
	UINT32* dwp;
	UINT16* wp;
	char* cp;
	short* sp;
	int* ip, iRet, i;
	bool* bp;

	if (m_pClientList[iToH] == 0) return;

	ZeroMemory(cData, sizeof(cData));

	dwp = (UINT32*)(cData + DEF_INDEX4_MSGID);
	*dwp = MSGID_NOTIFY;

	wp = (UINT16*)(cData + DEF_INDEX2_MSGTYPE);
	*wp = wMsgType;

	cp = (char*)(cData + DEF_INDEX2_MSGTYPE + 2);

	// !!! sV1, sV2, sV3Â´Ã‚ DWORDÃ‡Ã¼Ã€Ã“Ã€Â» Â¸Ã­Â½Ã‰Ã‡ÃÂ¶Ã³.
	switch (wMsgType) {

	case DEF_NOTIFY_ARENASTATUS:
		ip = (int*)cp;
		*ip = sV1;
		cp += 4;

		ip = (int*)cp;
		*ip = sV2;
		cp += 4;

		ip = (int*)cp;
		*ip = sV3;
		cp += 4;

		memcpy(cp, pString, 10);
		cp += 10;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 28);
		break;

	case DEF_NOTIFY_EVENT: // kamal
		*cp = (char)sV1;
		cp++;
		*cp = (char)sV2;
		cp++;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 8);
		break;

	case DEF_NOTIFY_EVENTFISHMODE:
		wp = (UINT16*)cp;
		*wp = (UINT16)sV1;
		cp += 2;

		wp = (UINT16*)cp;
		*wp = (UINT16)sV2;
		cp += 2;

		wp = (UINT16*)cp;
		*wp = (UINT16)sV3;
		cp += 2;

		memcpy(cp, pString, 20);
		cp += 20;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 32);
		break;

		//LifeX Party Cords
	case DEF_SEND_PARTYCOORDS:
	case DEF_NOTIFY_DGKILL:
		ip = (int*)cp;
		*ip = (int)sV1;
		cp += 4;

		ip = (int*)cp;
		*ip = (int)sV2;
		cp += 4;

		ip = (int*)cp;
		*ip = (int)sV3;
		cp += 4;

		if (pString != 0) memcpy(cp, pString, 10);
		cp += 10;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 28);
		break;

	case DEF_SEND_PARTYHP: //New Party Status - ZeroEoyPnk
	case DEF_NOTIFY_HELDENIANCOUNT:
		ip = (int*)cp;
		*ip = (int)sV1;
		cp += 4;

		ip = (int*)cp;
		*ip = (int)sV2;
		cp += 4;

		ip = (int*)cp;
		*ip = (int)sV3;
		cp += 4;

		ip = (int*)cp;
		*ip = (int)sV4;
		cp += 4;

		ip = (int*)cp;
		*ip = (int)sV5;
		cp += 4;

		ip = (int*)cp;
		*ip = (int)sV6;
		cp += 4;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 30);
		break;

	case DEF_NOTIFY_SERVERTIME:
		SYSTEMTIME SysTime;
		GetLocalTime(&SysTime);

		sp = (short*)cp;
		*sp = (short)SysTime.wHour;
		cp += 2;

		ip = (int*)cp;
		*ip = (int)m_iPrimaryDropRate;
		cp += 4;

		ip = (int*)cp;
		*ip = (int)m_iSecondaryDropRate;
		cp += 4;

		ip = (int*)cp;
		*ip = (int)m_iRareDropRate;
		cp += 4;

		ip = (int*)cp;
		*ip = (int)m_iServerPhyDmg;
		cp += 4;

		ip = (int*)cp;
		*ip = (int)m_iServerMagDmg;
		cp += 4;

		bp = (bool*)cp;
		*bp = m_bNullDrop[DROP_MA];
		cp++;

		bp = (bool*)cp;
		*bp = m_bNullDrop[DROP_PA];
		cp++;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 30);
		break;

	
	case DEF_NOTIFY_TEAMARENA:
		sp = (short*)cp;
		*sp = (short)sV1;
		cp += 2;

		sp = (short*)cp;
		*sp = (short)sV2;
		cp += 2;

		sp = (short*)cp;
		*sp = (short)sV3;
		cp += 2;

		sp = (short*)cp;
		*sp = (short)sV4;
		cp += 2;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 14);
		break;

	case NOTIFYMSG:
		strcpy(cp, pString);
		cp += strlen(pString);

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 6 + strlen(pString) + 1);
		break;

	case NOTIFY_EQUIPITEM2:
		*cp = char(sV1);
		cp++;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 7);
		break;

		// New 07/05/2004
		// Party Notify Msg's
	case DEF_NOTIFY_PARTY:
		switch (sV1) {
		case 4:
		case 6:
			wp = (UINT16*)cp;
			*wp = (UINT16)sV1;
			cp += 2;

			wp = (UINT16*)cp;
			*wp = (UINT16)sV2;
			cp += 2;

			wp = (UINT16*)cp;
			*wp = (UINT16)sV3;
			cp += 2;

			memcpy(cp, pString, 10);
			cp += 10;

			iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 22);
			break;

		case 5:
			wp = (UINT16*)cp;
			*wp = (UINT16)sV1;
			cp += 2;

			wp = (UINT16*)cp;
			*wp = (UINT16)sV2;
			cp += 2;

			wp = (UINT16*)cp;
			*wp = (UINT16)sV3;
			cp += 2;

			memcpy(cp, pString, sV3 * 11);
			cp += sV3 * 11;

			iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 12 + (sV3 * 11));
			break;

		default:
			wp = (UINT16*)cp;
			*wp = (UINT16)sV1;
			cp += 2;

			wp = (UINT16*)cp;
			*wp = (UINT16)sV2;
			cp += 2;

			wp = (UINT16*)cp;
			*wp = (UINT16)sV3;
			cp += 2;

			wp = (UINT16*)cp;
			*wp = (UINT16)sV4;
			cp += 2;

			iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 14);
			break;
		}
		break;

	case DEF_NOTIFY_UPGRADEHEROCAPE:
	case DEF_NOTIFY_GIZONITEMCANGE:
		sp = (short*)cp;
		*sp = (short)sV1;
		cp += 2;

		*cp = (char)sV2;
		cp++;

		sp = (short*)cp;
		*sp = (short)sV3;
		cp += 2;

		sp = (short*)cp;
		*sp = (short)sV4;
		cp += 2;

		sp = (short*)cp;
		*sp = (short)sV5;
		cp += 2;

		*cp = (char)sV6;
		cp++;

		*cp = (char)sV7;
		cp++;

		dwp = (UINT32*)cp;
		*dwp = sV8;
		cp += 4;

		memcpy(cp, pString, 20);
		cp += 20;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 41);
		break;

	case DEF_NOTIFY_TCLOC:
		wp = (UINT16*)cp;
		*wp = (UINT16)sV1;
		cp += 2;

		wp = (UINT16*)cp;
		*wp = (UINT16)sV2;
		cp += 2;

		memcpy(cp, pString, 10);
		cp += 10;

		wp = (UINT16*)cp;
		*wp = (UINT16)sV4;
		cp += 2;

		wp = (UINT16*)cp;
		*wp = (UINT16)sV5;
		cp += 2;

		memcpy(cp, pString2, 10);
		cp += 10;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 34);
		break;

		//New 11/05/2004
	case DEF_NOTIFY_GRANDMAGICRESULT:
		wp = (UINT16*)cp;
		*wp = (UINT16)sV1;
		cp += 2;

		wp = (UINT16*)cp;
		*wp = (UINT16)sV2;
		cp += 2;

		wp = (UINT16*)cp;
		*wp = (UINT16)sV3;
		cp += 2;

		memcpy(cp, pString, 10);
		cp += 10;

		wp = (UINT16*)cp;
		*wp = (UINT16)sV4;
		cp += 2;

		if (sV9 > 0) {
			memcpy(cp, pString2, (sV9 * 2) + 2);
			cp += (sV9 * 2) + 2;

			iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, (sV9 * 2) + 26);
		}
		else {
			sp = (short*)cp;
			*sp = 0;
			cp += 2;

			iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 26);
		}
		break;

	case DEF_NOTIFY_MAPSTATUSNEXT:
	case DEF_NOTIFY_MAPSTATUSLAST:
		memcpy(cp, pString, sV1);
		cp += sV1;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 6 + sV1);
		break;

	case DEF_NOTIFY_BUILDITEMSUCCESS:
		if (sV1 >= 0) {
			sp = (short*)cp;
			*sp = (short)sV1;
			cp += 2;
		}
		else {
			sp = (short*)cp;
			*sp = (short)sV1 + 10000;
			cp += 2;
		}

		sp = (short*)cp;
		*sp = (short)sV2;
		cp += 2;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 10);
		break;

	case DEF_NOTIFY_QUESTCONTENTS:
		sp = (short*)cp;
		*sp = (short)sV1;
		cp += 2;

		sp = (short*)cp;
		*sp = (short)sV2;
		cp += 2;

		sp = (short*)cp;
		*sp = (short)sV3;
		cp += 2;

		sp = (short*)cp;
		*sp = (short)sV4;
		cp += 2;

		sp = (short*)cp;
		*sp = (short)sV5;
		cp += 2;

		sp = (short*)cp;
		*sp = (short)sV6;
		cp += 2;

		sp = (short*)cp;
		*sp = (short)sV7;
		cp += 2;

		sp = (short*)cp;
		*sp = (short)sV8;
		cp += 2;

		sp = (short*)cp;
		*sp = (short)sV9;
		cp += 2;

		sp = (short*)cp;
		*sp = (short)sV10; // Magn0S:: Multi Quest
		cp += 2;

		if (pString2 != 0) memcpy(cp, pString2, 20);
		cp += 20;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 48); // Original 44
		break;

	case DEF_NOTIFY_QUESTPRIZE:
		ip = (int*)cp;
		*ip = (int)sV1;
		cp += 4;

		ip = (int*)cp;
		*ip = (int)sV2;
		cp += 4;

		ip = (int*)cp;
		*ip = (int)sV3;
		cp += 4;

		memcpy(cp, pString, 20);
		cp += 20;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 38);
		break;


	case DEF_NOTIFY_SETEXCHANGEITEM:
	case DEF_NOTIFY_OPENEXCHANGEWINDOW:
		sp = (short*)cp;
		*sp = (short)sV1;
		cp += 2;

		sp = (short*)cp;
		*sp = (short)sV2;
		cp += 2;

		sp = (short*)cp;
		*sp = (short)sV3;
		cp += 2;

		ip = (int*)cp;
		*ip = (int)sV4;
		cp += 4;

		*cp = (char)sV5;
		cp++;

		sp = (short*)cp;
		*sp = (short)sV6;
		cp += 2;

		sp = (short*)cp;
		*sp = (short)sV7;
		cp += 2;

		sp = (short*)cp;
		*sp = (short)sV8;
		cp += 2;

		memcpy(cp, pString, 20);
		cp += 20;

		memcpy(cp, m_pClientList[iFromH]->m_cCharName, 10);
		cp += 10;

		// v1.42
		dwp = (UINT32*)cp;
		*dwp = (UINT32)sV9;
		cp += 4;

		//----------------------------
		sp = (short*)cp;
		*sp = (short)sV10;
		cp += 2;
		sp = (short*)cp;
		*sp = (short)sV11;
		cp += 2;
		sp = (short*)cp;
		*sp = (short)sV12;
		cp += 2;
		sp = (short*)cp;
		*sp = (short)sV13;
		cp += 2;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 65); //57
		break;

	case DEF_NOTIFY_ITEMPOSLIST:
		for (i = 0; i < DEF_MAXITEMS; i++) {
			sp = (short*)cp;
			*sp = (short)m_pClientList[iToH]->m_ItemPosList[i].x;
			cp += 2;

			sp = (short*)cp;
			*sp = (short)m_pClientList[iToH]->m_ItemPosList[i].y;
			cp += 2;
		}
		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 6 + (DEF_MAXITEMS * 4));
		break;

	case DEF_NOTIFY_ADMINIFO:
		switch (sV1) {
		case 1:
			// NPCÃ€Ã‡ ÃÂ¤ÂºÂ¸Â¸Â¦ Â¾Ã²Â¾Ã®Â¿Ã‚Â´Ã™.
			ip = (int*)cp;
			*ip = m_pNpcList[sV2]->m_iHP;
			cp += 4;

			ip = (int*)cp;
			*ip = m_pNpcList[sV2]->m_iDefenseRatio;
			cp += 4;

			ip = (int*)cp;
			*ip = m_pNpcList[sV2]->m_bIsSummoned;
			cp += 4;

			ip = (int*)cp;
			*ip = m_pNpcList[sV2]->m_cActionLimit;
			cp += 4;

			ip = (int*)cp;
			*ip = m_pNpcList[sV2]->m_iHitDice;
			cp += 4;

			dwp = (UINT32*)cp; // v1.4
			*dwp = m_pNpcList[sV2]->m_dwDeadTime;
			cp += 4;

			dwp = (UINT32*)cp;
			*dwp = m_pNpcList[sV2]->m_dwRegenTime;
			cp += 4;

			ip = (int*)cp;
			*ip = (int)m_pNpcList[sV2]->m_bIsKilled;
			cp += 4;

			iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 26 + 12);
			break;
		}
		break;

	case DEF_NOTIFY_NPCTALK:
		sp = (short*)cp;
		*sp = (short)sV1;
		cp += 2;

		sp = (short*)cp;
		*sp = (short)sV2;
		cp += 2;

		sp = (short*)cp;
		*sp = (short)sV3;
		cp += 2;

		sp = (short*)cp;
		*sp = (short)sV4;
		cp += 2;

		sp = (short*)cp;
		*sp = (short)sV5;
		cp += 2;

		sp = (short*)cp;
		*sp = (short)sV6;
		cp += 2;

		sp = (short*)cp;
		*sp = (short)sV7;
		cp += 2;

		sp = (short*)cp;
		*sp = (short)sV8;
		cp += 2;

		sp = (short*)cp;
		*sp = (short)sV9;
		cp += 2;

		if (pString != 0) memcpy(cp, pString, 20);
		cp += 20;

		if (pString2 != 0) memcpy(cp, pString2, 10);
		cp += 10;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 54);
		break;

	//Magn0S:: Update Event status
	case DEF_NOTIFY_EVENTUPDATER:
		sp = (short*)cp;
		*sp = (short)sV1;
		cp += 2;

		sp = (short*)cp;
		*sp = (short)sV2;
		cp += 2;

		sp = (short*)cp;
		*sp = (short)sV3;
		cp += 2;

		sp = (short*)cp;
		*sp = (short)sV4;
		cp += 2;

		sp = (short*)cp;
		*sp = (short)sV5;
		cp += 2;

		sp = (short*)cp;
		*sp = (short)sV6;
		cp += 2;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 18);
		break;

	case DEF_NOTIFY_SUPERATTACKLEFT:
		ip = (int*)cp;
		*ip = m_pClientList[iToH]->m_iSuperAttackLeft;
		cp += 4;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 10);
		break;

	case DEF_NOTIFY_SAFEATTACKMODE:
		*cp = m_pClientList[iToH]->m_bIsSafeAttackMode;
		cp++;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 7);
		break;

	case DEF_NOTIFY_QUERY_JOINPARTY:
	case DEF_NOTIFY_IPACCOUNTINFO:
		strcpy(cp, pString);
		cp += strlen(pString);

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 6 + strlen(pString) + 1);
		break;

	case DEF_NOTIFY_SLATE_CREATESUCCESS:
		dwp = (UINT32*)cp;
		*dwp = sV1;
		cp += 4;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 10);
		break;

	case DEF_NOTIFY_REWARDGOLD:
		dwp = (UINT32*)cp;
		*dwp = m_pClientList[iToH]->m_iRewardGold;
		cp += 4;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 10);
		break;

	case DEF_NOTIFY_SERVERSHUTDOWN:
	case DEF_NOTIFY_GLOBALATTACKMODE:
	case DEF_NOTIFY_WHETHERCHANGE:
	case DEF_NOTIFY_TIMECHANGE:
	case DEF_NOTIFY_HUNGER:
		*cp = (char)sV1;
		cp++;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 7);
		break;

	case DEF_NOTIFY_NOTICEMSG:
		memcpy(cp, pString, strlen(pString));
		cp += strlen(pString);

		*cp = 0;
		cp++;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, strlen(pString) + 7);
		break;

	case DEF_NOTIFY_RATINGPLAYER:
		*cp = (char)sV1;
		cp++;

		memcpy(cp, pString, 10);
		cp += 10;

		ip = (int*)cp;
		*ip = m_pClientList[iToH]->m_iRating;
		cp += 4;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 21);
		break;

	case DEF_NOTIFY_PLAYERPROFILE:
		if (strlen(pString) > 100) {
			memcpy(cp, pString, 100);
			cp += 100;
		}
		else {
			memcpy(cp, pString, strlen(pString));
			cp += strlen(pString);
		}
		*cp = 0;
		cp++;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 7 + strlen(pString));
		break;

		// New 10/05/2004 Changed
	case DEF_NOTIFY_WHISPERMODEON:
	case DEF_NOTIFY_WHISPERMODEOFF:
	case DEF_NOTIFY_PLAYERNOTONGAME:
		memcpy(cp, pString, 10);
		cp += 10;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 16);
		break;

		// New 15/05/2004 Changed
	case DEF_NOTIFY_PLAYERONGAME:
		memcpy(cp, pString, 10);
		cp += 10;

		if (pString[0] != 0) {
			memcpy(cp, pString2, 14);
			cp += 14;
		}

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 30);
		break;

	case DEF_NOTIFY_REPAIRALLPRICES:
		wp = (UINT16*)cp;
		*wp = (UINT16)m_pClientList[iToH]->totalItemRepair;
		cp += 2;

		for (i = 0; i < m_pClientList[iToH]->totalItemRepair; i++)
		{
			*cp = (char)m_pClientList[iToH]->m_stRepairAll[i].index;
			cp++;

			wp = (UINT16*)cp;
			*wp = (UINT16)m_pClientList[iToH]->m_stRepairAll[i].price;
			cp += 2;
		}

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 8 + (m_pClientList[iToH]->totalItemRepair * 3));
		break;

	case DEF_NOTIFY_QUESTREWARD:
		sp = (short*)cp;
		*sp = (short)sV1;
		cp += 2;

		sp = (short*)cp;
		*sp = (short)sV2;
		cp += 2;

		ip = (int*)cp;
		*ip = (int)sV3;
		cp += 4;

		memcpy(cp, pString, 20);
		cp += 20;

		ip = (int*)cp;
		*ip = (int)sV4;
		cp += 4;

		ip = (int*)cp; //Magn0S:: Multi Quest
		*ip = (int)sV5;
		cp += 4;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 42);
		break;

	case DEF_NOTIFY_HELP:
	case DEF_NOTIFY_REPAIRITEMPRICE:
	case DEF_NOTIFY_SELLITEMPRICE:
		dwp = (UINT32*)cp;
		*dwp = (UINT32)sV1;
		cp += 4;

		dwp = (UINT32*)cp;
		*dwp = (UINT32)sV2;
		cp += 4;

		dwp = (UINT32*)cp;
		*dwp = (UINT32)sV3;
		cp += 4;

		dwp = (UINT32*)cp;
		*dwp = (UINT32)sV4;
		cp += 4;

		memcpy(cp, pString, 20);
		cp += 20;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 42);
		break;

	case DEF_NOTIFY_TOTALUSERS:
		ip = (int*)cp;
		*ip = m_iTotalGameServerClients;
		cp += 4;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 10);
		break;

	case DEF_NOTIFY_SERVERCHANGE:
		memcpy(cp, m_pClientList[iToH]->m_cMapName, 10);
		cp += 10;

		// World Server connection across hgservers, behind router problem
		// ArchAngel fix
		if (m_iGameServerMode == 1) // Mode = 1 is LAN or ROUTER (behind router)
		{
			if (memcmp(m_cLogServerAddr, m_cGameServerAddr, 15) == 0)
			{
				memcpy(cp, m_cGameServerAddrExternal, 15);
			}
			else
			{
				memcpy(cp, m_cLogServerAddr, 15);
			}
		}
		else if (m_iGameServerMode == 2)// mode = 2 INTERNET
		{
			memcpy(cp, m_cLogServerAddr, 15);
		}
		cp += 15;

		ip = (int*)cp;
		*ip = m_iLogServerPort;
		cp += 4;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 16 + 19);
		break;

	case DEF_NOTIFY_ENEMYKILLREWARD:
		dwp = (UINT32*)cp;
		*dwp = (UINT32)m_pClientList[iToH]->m_iExp;
		cp += 4;

		dwp = (UINT32*)cp;
		*dwp = (UINT32)m_pClientList[iToH]->m_iEnemyKillCount;
		cp += 4;

		dwp = (UINT32*)cp;
		*dwp = (UINT32)m_pClientList[iToH]->m_iMaxEK;
		cp += 4;

		memcpy(cp, m_pClientList[sV1]->m_cCharName, 10);
		cp += 10;

		memcpy(cp, m_pClientList[sV1]->m_cGuildName, 20);
		cp += 20;

		sp = (short*)cp;
		*sp = (short)m_pClientList[sV1]->m_iGuildRank;
		cp += 2;

		sp = (short*)cp;
		*sp = (short)m_pClientList[iToH]->m_iWarContribution;
		cp += 2;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 48);
		break;

	case DEF_NOTIFY_USERJOIN:
		memcpy(cp, pString, 10);
		cp += 10;
		memcpy(cp, pString2, 20);
		cp += 20;
		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 36);
		break;

	case DEF_NOTIFY_PKCAPTURED:
		// PKÂ¸Â¦ Ã€Ã¢Â¾Ã’Â´Ã™.
		// PKÃ€Ã‡ PKcount
		wp = (UINT16*)cp;
		*wp = (UINT16)sV1;
		cp += 2;
		wp = (UINT16*)cp;
		*wp = (UINT16)sV2;
		cp += 2;
		memcpy(cp, pString, 10);
		cp += 10;
		dwp = (UINT32*)cp;
		*dwp = (UINT32)m_pClientList[iToH]->m_iRewardGold;
		cp += 4;

		dwp = (UINT32*)cp;
		*dwp = m_pClientList[iToH]->m_iExp;
		cp += 4;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 28);
		break;

	case DEF_NOTIFY_PKPENALTY:
		// PK Ã†Ã¤Â³ÃŽÃ†Â¼Â¸Â¦ Â¸Ã”Â¾ÃºÂ´Ã™.
		dwp = (UINT32*)cp;
		*dwp = m_pClientList[iToH]->m_iExp;
		cp += 4;
		dwp = (UINT32*)cp;
		*dwp = (UINT32)m_pClientList[iToH]->m_iStr;
		cp += 4;
		dwp = (UINT32*)cp;
		*dwp = (UINT32)m_pClientList[iToH]->m_iVit;
		cp += 4;
		dwp = (UINT32*)cp;
		*dwp = (UINT32)m_pClientList[iToH]->m_iDex;
		cp += 4;
		dwp = (UINT32*)cp;
		*dwp = (UINT32)m_pClientList[iToH]->m_iInt;
		cp += 4;
		dwp = (UINT32*)cp;
		*dwp = (UINT32)m_pClientList[iToH]->m_iMag;
		cp += 4;
		dwp = (UINT32*)cp;
		*dwp = (UINT32)m_pClientList[iToH]->m_iCharisma;
		cp += 4;
		dwp = (UINT32*)cp;
		*dwp = (UINT32)m_pClientList[iToH]->m_iPKCount;
		cp += 4;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 38);
		break;

	case DEF_NOTIFY_EXP:
		dwp = (UINT32*)cp;
		*dwp = m_pClientList[iToH]->m_iExp;
		cp += 4;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 10);
		break;

	case DEF_NOTIFY_HP:
		ip = (int*)cp;
		*ip = m_pClientList[iToH]->m_iHP;
		cp += 4;

		RefreshPartyStatus(iToH);

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 10);
		break;

	case DEF_NOTIFY_MP:
		ip = (int*)cp;
		*ip = m_pClientList[iToH]->m_iMP;
		cp += 4;

		RefreshPartyStatus(iToH);

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 10);
		break;

	case DEF_NOTIFY_SP:
		ip = (int*)cp;
		*ip = m_pClientList[iToH]->m_iSP;
		cp += 4;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 10);
		break;

	case DEF_NOTIFY_RESUR_ON:
	case DEF_NOTIFY_RESUR_OFF:
		
		for (i = 0; i < DEF_MAXMAGICTYPE; i++) {
			*cp = m_pClientList[iToH]->m_cMagicMastery[i];
			cp++;
		}

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 6 + DEF_MAXMAGICTYPE);
		break;

	/* Centuu: msgs agrupados */
	case DEF_NOTIFY_ITEMBLOOD:
	case DEF_NOTIFY_FRAGILE:
	case DEF_NOTIFY_SUMMONGUILD:
	case DEF_NOTIFY_TRAVELERLIMITEDLEVEL:
	case DEF_NOTIFY_LIMITEDLEVEL:
	case DEF_NOTIFY_BUILDITEMFAIL:
	case DEF_NOTIFY_HELDENIANSTART:
	case DEF_NOTIFY_ANGEL_RECEIVED:
	case DEF_NOTIFY_DEATHMATCHSTART: // MORLA 2.3 - DEATHMACH ON
	case DEF_NOTIFY_DEATHMATCHEND: // MORLA 2.3 - DEATHMACH OFF
	case DEF_NOTIFY_MORLEARPJ:
	case DEF_NOTIFY_NOMOREAGRICULTURE:
	case DEF_NOTIFY_AGRICULTURESKILLLIMIT:
	case DEF_NOTIFY_AGRICULTURENOAREA:
	case DEF_NOTIFY_APOCFORCERECALLPLAYERS:
	case DEF_NOTIFY_APOCGATESTARTMSG:
	case DEF_NOTIFY_APOCGATEENDMSG:
	case DEF_NOTIFY_NORECALL:
	case DEF_NOTIFY_ABADDONTHUNDER:
	case DEF_NOTIFY_0BE8:
	case DEF_NOTIFY_HELDENIANTELEPORT:
	case DEF_NOTIFY_HELDENIANEND:
	case DEF_NOTIFY_RESURRECTPLAYER:
	case DEF_NOTIFY_SLATE_EXP:
	case DEF_NOTIFY_SLATE_MANA:
	case DEF_NOTIFY_SLATE_INVINCIBLE:
	case DEF_NOTIFY_SLATE_CREATEFAIL:
	case DEF_NOTIFY_NOMORECRUSADESTRUCTURE:
	case DEF_NOTIFY_EXCHANGEITEMCOMPLETE:
	case DEF_NOTIFY_CANCELEXCHANGEITEM:
	case DEF_NOTIFY_NOTFLAGSPOT:
	case DEF_NOTIFY_CRAFTING_SUCCESS:
	case DEF_NOTIFY_POTIONSUCCESS:
	case DEF_NOTIFY_LOWPOTIONSKILL:
	case DEF_NOTIFY_POTIONFAIL:
	case DEF_NOTIFY_NOMATCHINGPOTION:
	case DEF_NOTIFY_ADMINUSERLEVELLOW:
	case DEF_NOTIFY_TOBERECALLED:
	case DEF_NOTIFY_CANNOTITEMTOBANK:
	case DEF_NOTIFY_STATECHANGE_FAILED:
	case DEF_NOTIFY_SETTING_FAILED:
	case DEF_NOTIFY_NOGUILDMASTERLEVEL:
	case DEF_NOTIFY_CANNOTBANGUILDMAN:
	case DEF_NOTIFY_METEORSTRIKEHIT:
	case DEF_NOTIFY_SPECIALABILITYENABLED:
	case DEF_NOTIFY_FISHSUCCESS:
	case DEF_NOTIFY_FISHFAIL:
	case DEF_NOTIFY_ITEMSOLD:
	case DEF_NOTIFY_APOCGATECLOSE:
	case DEF_NOTIFY_KILLED:
	case DEF_NOTIFY_ABBYAPPEAR:
		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 6);
		break;

	case DEF_NOTIFY_ITEMUPGRADEFAIL:
	case DEF_NOTIFY_CANNOTCONSTRUCT:
	case DEF_NOTIFY_OBSERVERMODE:
	case DEF_NOTIFY_DOWNSKILLINDEXSET:
	case DEF_NOTIFY_RESPONSE_CREATENEWPARTY:
		sp = (short*)cp;
		*sp = (short)sV1;
		cp += 2;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 8);
		break;

	case DEF_NOTIFY_METEORSTRIKECOMING:
	case DEF_NOTIFY_FORCEDISCONN:
	case DEF_NOTIFY_FISHCANCELED:
	case DEF_NOTIFY_CANNOTRATING:
	case DEF_NOTIFY_SKILLUSINGEND:
	case DEF_NOTIFY_SLATE_STATUS:
		wp = (UINT16*)cp;
		*wp = (UINT16)sV1;
		cp += 2;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 8);
		break;

	case DEF_SERVER_TIME:
	case DEF_NOTIFY_ENEMYKILLS:
		ip = (int*)cp;
		*ip = (int)sV1;
		cp += 4;

		ip = (int*)cp;
		*ip = (int)sV2;
		cp += 4;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 14);
		break;

	case DEF_NOTIFY_REQRANGO:
	case DEF_UPDATE_GUILDRANK:
	case DEF_NOTIFY_HEROBONUS:
	case DEF_MAX_STATS: //LifeX Fix Bytes Accuracy 01/01
	case DEF_MAX_LEVEL:
	case DEF_NOTIFY_HELPFAILED:
	case DEF_NOTIFY_CRAFTING_FAIL:	//reversed by Snoopy: 0x0BF1:
	case DEF_NOTIFY_ANGEL_FAILED:
	case DEF_NOTIFY_FIGHTZONERESERVE:
	case DEF_NOTIFY_FORCERECALLTIME:
	case DEF_NOTIFY_HELDENIANVICTORY:
	case DEF_NOTIFY_MONSTERCOUNT:
	case DEF_NOTIFY_DEBUGMSG:
	case DEF_NOTIFY_FISHCHANCE:
	case DEF_NOTIFY_COINPOINTS:
	case DEF_NOTIFY_CONTRIBPOINTS:
	case DEF_NOTIFY_MONSTERCOUNTAPOC:
		ip = (int*)cp;
		*ip = (int)sV1;
		cp += 4;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 10);
		break;

	case DEF_NOTIFY_ITEMDEPLETED_ERASEITEM:
		wp = (UINT16 *)cp;
		*wp = (UINT16)sV1;
		cp += 2;

		wp = (UINT16 *)cp;
		*wp = (UINT16)sV2;
		cp += 2;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 10);
		break;

	case DEF_NOTIFY_SKILL:
		sp = (short*)cp;
		*sp = (short)sV1;
		cp += 2;

		sp = (short*)cp;
		*sp = (short)sV2;
		cp += 2;

		sp = (short*)cp;
		*sp = (short)sV3;
		cp += 2;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 12);
		break;
	
	case DEF_NOTIFY_QUESTCOUNTER: // Magn0S:: Multi Quest - centu
	case DEF_NOTIFY_SKILLPOINT:
	case DEF_NOTIFY_CURLIFESPAN:
	case DEF_NOTIFY_ITEMCOLORCHANGE:
	case DEF_NOTIFY_ITEMRELEASED:
	case DEF_NOTIFY_ITEMLIFESPANEND:
	case DEF_NOTIFY_ATTUNEARMOUR:
		sp = (short *)cp;
		*sp = (short)sV1;
		cp += 2;

		sp = (short *)cp;
		*sp = (short)sV2;
		cp += 2;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 10);
		break;

	case DEF_NOTIFY_ARMOURVALUES:
		ip = (int*)cp;
		*ip = m_pClientList[iToH]->m_iAddAbsEarth;
		cp += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iToH]->m_iAddAbsLight;
		cp += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iToH]->m_iAddAbsFire;
		cp += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iToH]->m_iAddAbsWater;
		cp += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iToH]->m_iAddAbsHoly;
		cp += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iToH]->m_iAddAbsUnholy;
		cp += 4;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 30);
		break;

	case DEF_NOTIFY_DROPITEMFIN_COUNTCHANGED:
	case DEF_NOTIFY_DROPITEMFIN_ERASEITEM:
		wp = (UINT16 *)cp;
		*wp = (UINT16)sV1;
		cp += 2;

		ip = (int *)cp;
		*ip = (int)sV2;
		cp += 4;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 12);
		break;

	case DEF_NOTIFY_REPDGDEATHS:
	case DEF_NOTIFY_QUESTCOMPLETED:
	case DEF_NOTIFY_QUESTABORTED:
		ip = (int*)cp;
		*ip = (int)sV1;
		cp += 4;

		ip = (int*)cp;
		*ip = (int)sV2;
		cp += 4;

		ip = (int*)cp;
		*ip = (int)sV3;
		cp += 4;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 18);
		break;

	case DEF_NOTIFY_ITEMREPAIRED:
		dwp = (UINT32*)cp;
		*dwp = (UINT32)sV1;
		cp += 4;

		dwp = (UINT32*)cp;
		*dwp = (UINT32)sV2;
		cp += 4;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 14);
		break;

	case DEF_NOTIFY_SHOWMAP:
		wp = (UINT16*)cp;
		*wp = (UINT16)sV1;
		cp += 2;

		wp = (UINT16*)cp;
		*wp = (UINT16)sV2;
		cp += 2;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 10);
		break;

	case DEF_NOTIFY_GIZONITEMUPGRADELEFT:
	case DEF_SEND_NPCHP: //50Cent - HP Bar
	case DEF_NOTIFY_ENERGYSPHERECREATED:
		ip = (int*)cp;
		*ip = (int)sV1;
		cp += 4;

		ip = (int*)cp;
		*ip = (int)sV2;
		cp += 4;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 14);
		break;

	case DEF_NOTIFY_SETITEMCOUNT:
		wp = (UINT16 *)cp;
		*wp = (UINT16)sV1;
		cp += 2;

		dwp = (UINT32 *)cp;
		*dwp = (UINT32)sV2;
		cp += 4;

		*cp = (char)sV3;
		cp++;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 13);
		break;

	case DEF_NOTIFY_SPAWNEVENT:
		sp = (short*)cp;
		*sp = (short)sV1;
		cp += 2;

		sp = (short*)cp;
		*sp = (short)sV2;
		cp += 2;

		*cp = (char)sV3;   // Mob's type
		cp++;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 11);
		break;

	case DEF_NOTIFY_QUERY_DISMISSGUILDREQPERMISSION:
	case DEF_NOTIFY_QUERY_JOINGUILDREQPERMISSION:
	case DEF_NOTIFY_CANNOTJOINMOREGUILDSMAN:
	case DEF_NOTIFY_ABADDONKILLED:
		memcpy(cp, m_pClientList[iFromH]->m_cCharName, 10);
		cp += 10;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 16);
		break;

	case DEF_NOTIFY_CHANGEPLAYMODE: // 2.06 - by KLKS
		memcpy(cp, pString, 10);
		cp += 10;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 16);
		break;

	case DEF_NOTIFY_MAGICEFFECTOFF:
	case DEF_NOTIFY_MAGICEFFECTON:
	case DEF_NOTIFY_CONSTRUCTIONPOINT:
	// centu - fixed special ability bug
	case DEF_NOTIFY_SPECIALABILITYSTATUS:
		sp = (short*)cp;
		*sp = (short)sV1;
		cp += 2;

		sp = (short*)cp;
		*sp = (short)sV2;
		cp += 2;

		sp = (short*)cp;
		*sp = (short)sV3;
		cp += 2;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 12);
		break;

	//50Cent - No Critical Damage Limit
	case DEF_NOTIFY_DAMAGEMOVE:
		sp = (short*)cp;
		*sp = (short)sV1;
		cp += 2;

		ip = (int*)cp;
		*ip = (int)sV2;
		cp += 4;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 12);
		break;

	case DEF_NOTIFY_PLAYERSHUTUP:
		wp = (UINT16*)cp;
		*wp = (UINT16)sV1;
		cp += 2;

		memcpy(cp, pString, 10);
		cp += 10;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 18);
		break;
	case DEF_NOTIFY_LOCKEDMAP:
		sp = (short*)cp;
		*sp = (short)sV1;
		cp += 2;

		memcpy(cp, pString, 10);
		cp += 10;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 18);
		break;

	case DEF_NOTIFY_ITEMATTRIBUTECHANGE:
		sp = (short *)cp;
		*sp = (short)sV1;
		cp += 2;

		dwp = (UINT32 *)cp;
		*dwp = sV2;
		cp += 4;

		dwp = (UINT32 *)cp;
		*dwp = sV3;
		cp += 4;

		dwp = (UINT32 *)cp;
		*dwp = sV4;
		cp += 4;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 20);
		break;

	case DEF_NOTIFY_CITYHP:
		ip = (int*)cp;
		*ip = (int)sV1;
		cp += 4;

		ip = (int*)cp;
		*ip = (int)sV2;
		cp += 4;
		
		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 14);
		break;

	case DEF_NOTIFY_CRUSADE:
		ip = (int*)cp;
		*ip = (int)sV1;
		cp += 4;

		ip = (int*)cp;
		*ip = (int)sV2;
		cp += 4;

		ip = (int*)cp;
		*ip = (int)sV3;
		cp += 4;

		ip = (int*)cp;
		*ip = (int)sV4;
		cp += 4;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 22);
		break;

	case DEF_COMMONTYPE_JOINGUILDAPPROVE:
		if (m_pClientList[iFromH] != 0)
			memcpy(cp, m_pClientList[iFromH]->m_cGuildName, 20);
		else memcpy(cp, "?", 1);
		cp += 20;

		sp = (short*)cp;
		*sp = DEF_GUILDSTARTRANK;
		cp += 2;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 28);
		break;

	case DEF_NOTIFY_TOP15HB:
	case DEF_NOTIFY_APOCGATEOPEN:
		ip = (int*)cp;
		*ip = (int)sV1;
		cp += 4;

		ip = (int*)cp;
		*ip = (int)sV2;
		cp += 4;

		memcpy(cp, pString, 10);
		cp += 10;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 24);
		break;

	case DEF_NOTIFY_GOLD:
		dwp = (UINT32*)cp;
		*dwp = (UINT32)sV1;
		cp += 4;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 10);
		break;

	case DEF_NOTIFY_CANNOTREPAIRITEM:
	case DEF_NOTIFY_CANNOTSELLITEM:
		wp = (UINT16*)cp;
		*wp = (UINT16)sV1;
		cp += 2;

		wp = (UINT16*)cp;
		*wp = (UINT16)sV2;
		cp += 2;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 10);
		break;
	case DEF_NOTIFY_CANNOTGIVEITEM:
	case DEF_NOTIFY_GIVEITEMFIN_COUNTCHANGED:
	case DEF_NOTIFY_GIVEITEMFIN_ERASEITEM:
		wp = (UINT16*)cp;
		*wp = (UINT16)sV1;
		cp += 2;

		ip = (int*)cp;
		*ip = (int)sV2;
		cp += 4;

		memcpy(cp, pString, 20);
		cp += 20;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 32);
		break;

	case DEF_NOTIFY_REQGUILDRANKANSWER:
	case DEF_NOTIFY_REQGUILDNAMEANSWER: 
		ip = (int*)cp;
		*ip = (int)sV1;
		cp += 4;

		ip = (int*)cp;
		*ip = (int)sV2;
		cp += 4;

		memcpy(cp, pString, 20);
		cp += 20;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 34);
		break;

	case DEF_NOTIFY_STATECHANGE_SUCCESS2:
		ip = (int*)cp;
		*ip = m_pClientList[iToH]->m_iStr;
		cp += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iToH]->m_iVit;
		cp += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iToH]->m_iDex;
		cp += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iToH]->m_iInt;
		cp += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iToH]->m_iMag;
		cp += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iToH]->m_iCharisma;
		cp += 4;

		for (i = 0; i < DEF_MAXMAGICTYPE; i++) {
			*cp = m_pClientList[iToH]->m_cMagicMastery[i];
			cp++;
		}

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 30 + DEF_MAXMAGICTYPE);
		break;

	case DEF_NOTIFY_STATECHANGE_SUCCESS:
	case DEF_NOTIFY_SETTING_SUCCESS:
	case DEF_NOTIFY_LEVELUP:
		ip = (int*)cp;
		*ip = m_pClientList[iToH]->m_iLevel;
		cp += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iToH]->m_iStr;
		cp += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iToH]->m_iVit;
		cp += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iToH]->m_iDex;
		cp += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iToH]->m_iInt;
		cp += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iToH]->m_iMag;
		cp += 4;

		ip = (int*)cp;
		*ip = m_pClientList[iToH]->m_iCharisma;
		cp += 4;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 34);
		break;

	case DEF_NOTIFY_GUILDDISBANDED:
		memcpy(cp, pString, 20);
		cp += 20;

		memcpy(cp, m_pClientList[iToH]->m_cLocation, 10);
		cp += 10;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 36);
		break;

	case DEF_COMMONTYPE_JOINGUILDREJECT:
	case DEF_COMMONTYPE_DISMISSGUILDREJECT:
		if (m_pClientList[iFromH] != 0)
			memcpy(cp, m_pClientList[iFromH]->m_cGuildName, 20);
		else memcpy(cp, "?", 1);
		cp += 20;
		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 26);
		break;
	case DEF_COMMONTYPE_DISMISSGUILDAPPROVE:
		if (m_pClientList[iFromH] != 0)
			memcpy(cp, m_pClientList[iFromH]->m_cGuildName, 20);
		else memcpy(cp, "?", 1);
		cp += 20;

		sp = (short*)cp;
		*sp = DEF_GUILDSTARTRANK;
		cp += 2;

		memcpy(cp, m_pClientList[iToH]->m_cLocation, 10);
		cp += 10;

		iRet = m_pClientList[iToH]->m_pXSock->iSendMsg(cData, 38);
		break;
	}

	switch (iRet) {
	case DEF_XSOCKEVENT_QUENEFULL:
	case DEF_XSOCKEVENT_SOCKETERROR:
	case DEF_XSOCKEVENT_CRITICALERROR:
	case DEF_XSOCKEVENT_SOCKETCLOSED:
		break;
	}
}

/*****************************************************************
**---------------------------FUNCTION---------------------------**
**             void Game::CheckLevelUp(int iClientH)            **
**-------------------------DESCRIPTION--------------------------**
** Level-Up                                                     **
**  - Level +1                                                  **
**  - +3 Level Up Points                                        **
**  - Reset Next Level EXP                                      **
**  - Civilian Level Limit                                      **
**      Player mode switches to Combatant                       **
**      when the limit is reached                               **
**  - Majestic Points +1                                        **
**  - Reset Next Level EXP                                      **
**------------------------CREATION DATE-------------------------**
**                January 30, 2007; 3:06 PM; Dax                **
*****************************************************************/
void CGame::bCheckLevelUp(int iClientH)
{
	class CItem* pItem;
	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_iAdminUserLevel > 0) return;
	while (m_pClientList[iClientH]->m_iExp >= m_pClientList[iClientH]->m_iNextLevelExp)
	{ 
		if (m_pClientList[iClientH]->m_iLevel < m_iPlayerMaxLevel)
		{
			m_pClientList[iClientH]->m_iLevel++;
			m_pClientList[iClientH]->m_iLU_Pool += 3;
			if (m_pClientList[iClientH]->m_iLevel > 49 && m_pClientList[iClientH]->m_bIsPlayerCivil)
			{
				ForceChangePlayMode(iClientH);
			}
			
			//Magn0S:: Add Gold per lvl up to help new players
			if (m_pClientList[iClientH]->m_iLevel < 50)
			{
				pItem = new class CItem;
				if(_bInitItemAttr(pItem, 90) == false)
				{
					delete pItem;
				}
				else 
				{
					pItem->m_dwCount = (UINT32)1000;
					bAddItem(iClientH, pItem, 0);
				}
			}

			// centu - max hp,mp,sp when level up
			m_pClientList[iClientH]->m_iHP = iGetMaxHP(iClientH);
			m_pClientList[iClientH]->m_iMP = iGetMaxMP(iClientH);
			m_pClientList[iClientH]->m_iSP = iGetMaxSP(iClientH);
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_HP, 0, 0, 0, 0);
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_MP, 0, 0, 0, 0);
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_SP, 0, 0, 0, 0);

			SendNotifyMsg(0, iClientH, DEF_NOTIFY_LEVELUP, 0, 0, 0, 0);
			m_pClientList[iClientH]->m_iNextLevelExp = m_iLevelExpTable[m_pClientList[iClientH]->m_iLevel + 1];
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_EXP, 0, 0, 0, 0);
			CalcTotalItemEffect(iClientH, -1, false);

		}
		else 
		{
			m_pClientList[iClientH]->m_iExp = m_iLevelExpTable[m_iPlayerMaxLevel];
			m_pClientList[iClientH]->m_iNextLevelExp = m_iLevelExpTable[m_iPlayerMaxLevel + 1];
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_EXP, 0, 0, 0, 0);
			m_pClientList[iClientH]->m_iGizonItemUpgradeLeft++;		
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_GIZONITEMUPGRADELEFT, m_pClientList[iClientH]->m_iGizonItemUpgradeLeft, 1, 0, 0, 0);
		}
	}
}

void CGame::RequestTeleportHandler(int iClientH, char * pData, char * cMapName, int dX, int dY, bool deleteteam)
{
 char  * pBuffer, cTempMapName[21];
 UINT32 * dwp;
 UINT16  * wp;
 char  * cp, cDestMapName[11], cDir, cMapIndex, cPoints = 0;
 short * sp, sX, sY, sSummonPoints, sV1, aX, aY;
 int   * ip, i, iRet, iSize, iDestX, iDestY, iExH, iMapSide, iTemp, iTemp2, iQuestNumber, iQuestType, j;
 bool    bRet, bIsLockedMapNotify;
 SYSTEMTIME SysTime;

	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
	if (m_pClientList[iClientH]->m_bIsKilled == true) return;
	if (m_pClientList[iClientH]->m_bIsOnWaitingProcess == true) return;
	if (m_pClientList[iClientH]->m_bIsForceDisconnect == true) return;
	
	//Magn0S:: Arena
	if (memcmp(m_pClientList[iClientH]->m_cMapName, "fightzone1", 10) == 0) {
		RequestArenaStatus(iClientH, false); // remove player
	}

	if ((m_pClientList[iClientH]->m_cSide == 2) && 
		(memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, "aresden", 7) == 0) &&
		((pData[0] == '1')  || (pData[0] == '3'))) {
		if (m_pClientList[iClientH]->m_iAdminUserLevel == 0) return;
	}	
	if ((m_pClientList[iClientH]->m_cSide == 1) && 
		(memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, "elvine", 6) == 0) &&
		((pData[0] == '1')  || (pData[0] == '3'))) {
		if (m_pClientList[iClientH]->m_iAdminUserLevel == 0) return;
	} 

	if (deleteteam)
	{
		c_team->DeleteCape(iClientH);
		c_team->DeleteBoots(iClientH);
	}

	bIsLockedMapNotify = false;

	if (m_pClientList[iClientH]->m_cExchangeMode != 0) {
		iExH = m_pClientList[iClientH]->m_iExchangeH;
		_ClearExchangeStatus(iExH);
		_ClearExchangeStatus(iClientH);
	}
	m_pClientList[iClientH]->m_dwLastActionTime = timeGetTime();
	

	RemoveFromTarget(iClientH, DEF_OWNERTYPE_PLAYER);

	minimap_clear(iClientH);
	
	m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->ClearOwner(13, iClientH, DEF_OWNERTYPE_PLAYER, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY);
	SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_LOG, DEF_MSGTYPE_REJECT, 0, 0, 0);
	sX = m_pClientList[iClientH]->m_sX;
	sY = m_pClientList[iClientH]->m_sY;
	ZeroMemory(cDestMapName, sizeof(cDestMapName));
	bRet = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSearchTeleportDest(sX, sY, cDestMapName, &iDestX, &iDestY, &cDir);
	// Capture the Flag
	if (m_bIsCTFEvent && (m_pClientList[iClientH]->m_iStatus & 0x80000) != 0) {
		if (pData[0] == '0' || pData[0] == '1' ||
			(strlen(cDestMapName) > 0 &&
				(memcmp(cDestMapName, "elvine", 6) != 0 &&
					memcmp(cDestMapName, "aresden", 7) != 0 &&
					memcmp(cDestMapName, "2ndmiddle", 9) != 0 &&
					memcmp(cDestMapName, "middleland", 10) != 0 &&
					memcmp(cDestMapName, "middled1n", 9) != 0)) ||
			(cMapName != 0 &&
				(memcmp(cMapName, "elvine", 6) != 0 &&
					memcmp(cMapName, "aresden", 7) != 0 &&
					memcmp(cMapName, "2ndmiddle", 9) != 0 &&
					memcmp(cMapName, "middleland", 10) != 0 &&
					memcmp(cMapName, "middled1n", 9) != 0))) return;
	}
	if ((strcmp(m_pClientList[iClientH]->m_cLockedMapName, "NONE") != 0) && (m_pClientList[iClientH]->m_iLockedMapTime > 0)) {
		iMapSide = iGetMapLocationSide(cDestMapName);

		if (m_pClientList[iClientH]->m_cSide != iMapSide && iMapSide != 0) {
			iDestX = -1;
			iDestY = -1;
			bIsLockedMapNotify = true;
			ZeroMemory(cDestMapName, sizeof(cDestMapName));
			strcpy(cDestMapName, m_pClientList[iClientH]->m_cLockedMapName);
		}
	}
	if (strcmp(m_pClientList[iClientH]->m_cMapName, "arebrk11") == 0)
	{
		ZeroMemory(cDestMapName, sizeof(cDestMapName));
		if (m_pClientList[iClientH]->m_cSide == 1) 
		{
			strcpy(cDestMapName, "cityhall_1");
		}
		else
		{
			strcpy(cDestMapName, "cityhall_2");
		}
	}
	if ((bRet == true) && (cMapName == 0)) {
		for (i = 0; i < DEF_MAXMAPS; i++) {
			if ((m_pMapList[i] != 0) && (memcmp(m_pMapList[i]->m_cName, cDestMapName, 10) == 0)) {
				m_pClientList[iClientH]->m_sX   = iDestX;
				m_pClientList[iClientH]->m_sY   = iDestY;
				m_pClientList[iClientH]->m_cDir = cDir;
				m_pClientList[iClientH]->m_cMapIndex = i;
				ZeroMemory(m_pClientList[iClientH]->m_cMapName, sizeof(m_pClientList[iClientH]->m_cMapName));
				memcpy(m_pClientList[iClientH]->m_cMapName, m_pMapList[i]->m_cName, 10);
				goto RTH_NEXTSTEP;
			}
		}
		m_pClientList[iClientH]->m_sX   = iDestX;
		m_pClientList[iClientH]->m_sY   = iDestY;
		m_pClientList[iClientH]->m_cDir = cDir;
		ZeroMemory(m_pClientList[iClientH]->m_cMapName, sizeof(m_pClientList[iClientH]->m_cMapName));
		memcpy(m_pClientList[iClientH]->m_cMapName, cDestMapName, 10);
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_MAGICEFFECTOFF, DEF_MAGICTYPE_CONFUSE, m_pClientList[iClientH]->m_cMagicEffectStatus[ DEF_MAGICTYPE_CONFUSE ], 0, 0);
		SetSlateFlag(iClientH, DEF_NOTIFY_SLATECLEAR, false);
		bSendMsgToLS(MSGID_REQUEST_SAVEPLAYERDATA_REPLY, iClientH, false);
		m_pClientList[iClientH]->m_bIsOnServerChange = true;
		m_pClientList[iClientH]->m_bIsOnWaitingProcess = true;
		return;
	}
	else {
		switch (pData[0]) {
		case '0':
		case '1':
			ZeroMemory(cTempMapName, sizeof(cTempMapName));
			if (m_pClientList[iClientH]->m_cSide == 0) { 
				strcpy(cTempMapName, "default"); 
			}
			else {
				if (m_pClientList[iClientH]->m_cSide == 1) {
					strcpy(cTempMapName, "aresden");
				}
				else {
					strcpy(cTempMapName, "elvine");
				}
			}
			if ((strcmp(m_pClientList[iClientH]->m_cLockedMapName, "NONE") != 0) && (m_pClientList[iClientH]->m_iLockedMapTime > 0)) {
				bIsLockedMapNotify = true;
				ZeroMemory(cTempMapName, sizeof(cTempMapName));
				strcpy(cTempMapName, m_pClientList[iClientH]->m_cLockedMapName);
			}
			for (i = 0; i < DEF_MAXMAPS; i++) {
				if ((m_pMapList[i] != 0) && (memcmp(m_pMapList[i]->m_cName, cTempMapName, 10) == 0)) {
					
					// centu - recall fix
					GetMapInitialPoint(i, &aX, &aY, m_pClientList[iClientH]->m_cLocation);
					if ((m_pClientList[iClientH]->m_sX == aX) && (m_pClientList[iClientH]->m_sY == aY))
					{
						GetMapInitialPoint(i, &aX, &aY, m_pClientList[iClientH]->m_cLocation);
						if ((m_pClientList[iClientH]->m_sX == aX) && (m_pClientList[iClientH]->m_sY == aY))
						{
							GetMapInitialPoint(i, &aX, &aY, m_pClientList[iClientH]->m_cLocation);
						}
					}
					m_pClientList[iClientH]->m_sX = aX;
					m_pClientList[iClientH]->m_sY = aY;

					m_pClientList[iClientH]->m_cMapIndex = i;
					ZeroMemory(m_pClientList[iClientH]->m_cMapName, sizeof(m_pClientList[iClientH]->m_cMapName));
					memcpy(m_pClientList[iClientH]->m_cMapName, m_pMapList[i]->m_cName, 10);
					goto RTH_NEXTSTEP;
				}
			}
			m_pClientList[iClientH]->m_sX   = -1;
			m_pClientList[iClientH]->m_sY   = -1;
			ZeroMemory(m_pClientList[iClientH]->m_cMapName, sizeof(m_pClientList[iClientH]->m_cMapName));
			memcpy(m_pClientList[iClientH]->m_cMapName, cTempMapName, 10);
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_MAGICEFFECTOFF, DEF_MAGICTYPE_CONFUSE, m_pClientList[iClientH]->m_cMagicEffectStatus[DEF_MAGICTYPE_CONFUSE], 0, 0);
			SetSlateFlag(iClientH, DEF_NOTIFY_SLATECLEAR, false);	
			bSendMsgToLS(MSGID_REQUEST_SAVEPLAYERDATA_REPLY, iClientH, false, false);
			m_pClientList[iClientH]->m_bIsOnServerChange = true;
			m_pClientList[iClientH]->m_bIsOnWaitingProcess = true;

			m_pClientList[iClientH]->m_iDGKills = 0;
			m_pClientList[iClientH]->m_iDGDeaths = 0;

			return;

		case '2':
		case '3':
			if ((strcmp(m_pClientList[iClientH]->m_cLockedMapName, "NONE") != 0) && 
				(m_pClientList[iClientH]->m_iLockedMapTime > 0) &&
				(memcmp(cMapName, "cityhall", 8) != 0)) {
				dX = -1;
				dY = -1;
				bIsLockedMapNotify = true;
				ZeroMemory(cTempMapName, sizeof(cTempMapName));
				strcpy(cTempMapName, m_pClientList[iClientH]->m_cLockedMapName);
			}
			else {
				ZeroMemory(cTempMapName, sizeof(cTempMapName));
				strcpy(cTempMapName, cMapName);
			}
			cMapIndex = iGetMapIndex(cTempMapName);
			if (cMapIndex == -1) {
				m_pClientList[iClientH]->m_sX   = dX;
				m_pClientList[iClientH]->m_sY   = dY;
				ZeroMemory(m_pClientList[iClientH]->m_cMapName, sizeof(m_pClientList[iClientH]->m_cMapName));
				memcpy(m_pClientList[iClientH]->m_cMapName, cTempMapName, 10);
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_MAGICEFFECTOFF, DEF_MAGICTYPE_CONFUSE, m_pClientList[iClientH]->m_cMagicEffectStatus[DEF_MAGICTYPE_CONFUSE], 0, 0);
				SetSlateFlag(iClientH, DEF_NOTIFY_SLATECLEAR, false);
				bSendMsgToLS(MSGID_REQUEST_SAVEPLAYERDATA_REPLY, iClientH, false);
				m_pClientList[iClientH]->m_bIsOnServerChange   = true;
				m_pClientList[iClientH]->m_bIsOnWaitingProcess = true;
				return;
			}		
			m_pClientList[iClientH]->m_sX   = dX;
			m_pClientList[iClientH]->m_sY   = dY;
			m_pClientList[iClientH]->m_cMapIndex = cMapIndex;
			ZeroMemory(m_pClientList[iClientH]->m_cMapName, sizeof(m_pClientList[iClientH]->m_cMapName));
			memcpy(m_pClientList[iClientH]->m_cMapName, m_pMapList[cMapIndex]->m_cName, 10);  
			goto RTH_NEXTSTEP;
		}
	}

RTH_NEXTSTEP:;

	SetPlayingStatus(iClientH);
	iTemp = m_pClientList[iClientH]->m_iStatus;
	iTemp = 0x0FFFFFFF & iTemp;
	iTemp2 = iGetPlayerABSStatus(iClientH);
	iTemp = iTemp | (iTemp2 << 28);
	m_pClientList[iClientH]->m_iStatus = iTemp;
	
	if ((m_pClientList[iClientH]->m_pIsProcessingAllowed == true) &&
		(m_sLastHeldenianWinner != m_pClientList[iClientH]->m_cSide) &&
		(m_sLastHeldenianWinner != 0)) {
		cPoints = 100;
	}
	if (bIsLockedMapNotify == true) {
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_LOCKEDMAP, m_pClientList[iClientH]->m_iLockedMapTime, 0, 0, m_pClientList[iClientH]->m_cLockedMapName);
	}
	pBuffer = new char [DEF_MSGBUFFERSIZE+1];
	ZeroMemory(pBuffer, DEF_MSGBUFFERSIZE+1);

	dwp  = (UINT32 *)(pBuffer + DEF_INDEX4_MSGID);
	*dwp = MSGID_RESPONSE_INITDATA;
	wp   = (UINT16 *)(pBuffer + DEF_INDEX2_MSGTYPE);
	*wp  = DEF_MSGTYPE_CONFIRM;
	cp = (char *)(pBuffer + DEF_INDEX2_MSGTYPE + 2);
	if (m_pClientList[iClientH]->m_bIsObserverMode == false) {
		bGetEmptyPosition(&m_pClientList[iClientH]->m_sX, &m_pClientList[iClientH]->m_sY, m_pClientList[iClientH]->m_cMapIndex);
	}
	else {
		GetMapInitialPoint(m_pClientList[iClientH]->m_cMapIndex, &m_pClientList[iClientH]->m_sX, &m_pClientList[iClientH]->m_sY);
	}

	sp  = (short *)cp;
	*sp = iClientH;
	cp += 2;
//8
	sp  = (short *)cp;
	*sp = m_pClientList[iClientH]->m_sX - 14 - 5;
	cp += 2;
//10
	sp  = (short *)cp;
	*sp = m_pClientList[iClientH]->m_sY - 12 - 5;
	cp += 2;
//12
	sp  = (short *)cp;
	*sp = m_pClientList[iClientH]->m_sType;
	cp += 2;
//14
	sp  = (short *)cp;
	*sp = m_pClientList[iClientH]->m_sAppr1;
	cp += 2;
//16
	sp  = (short *)cp;
	*sp = m_pClientList[iClientH]->m_sAppr2;
	cp += 2;
//18
	sp  = (short *)cp;
	*sp = m_pClientList[iClientH]->m_sAppr3;
	cp += 2;
//20
	sp  = (short *)cp;
	*sp = m_pClientList[iClientH]->m_sAppr4;
	cp += 2;
//22
	ip  = (int *)cp;
	*ip = m_pClientList[iClientH]->m_iApprColor;
	cp += 4;
//26
	ip  = (int *)cp;
	*ip = m_pClientList[iClientH]->m_iStatus;
	cp += 4;
//30
	
	memcpy(cp, m_pClientList[iClientH]->m_cMapName, 10);
	cp += 10;
//40
	memcpy(cp, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, 10);
	cp += 10;
//50
	if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsFixedDayMode == true) {
		*cp = 1;
	}
	else {
		*cp = m_cDayOrNight;
	}
	cp++;
//51
	if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsFixedDayMode == true) {
		*cp = 0;
	}
	else {
		*cp = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cWhetherStatus;
	}
	cp++;
//52
	ip = (int *)cp;
	*ip = m_pClientList[iClientH]->m_iContribution;
	cp += 4;
//56
	if (m_pClientList[iClientH]->m_bIsObserverMode == false) {
		m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetOwner(iClientH, DEF_OWNERTYPE_PLAYER, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY);
	}

	*cp = (char)m_pClientList[iClientH]->m_bIsObserverMode;
	cp++;
//57
	ip = (int *)cp;
	*ip = m_pClientList[iClientH]->m_iRating;
	cp += 4;
//61
	ip = (int *)cp;
	*ip = m_pClientList[iClientH]->m_iHP;
	cp += 4;
//65
	*cp = cPoints;
	cp++;
	//66
	// centu - 800x600
	iSize = iComposeInitMapData(m_pClientList[iClientH]->m_sX - 12, m_pClientList[iClientH]->m_sY - 9, iClientH, cp);
	cp += iSize;

	iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(pBuffer, 66 + iSize); // SephirotH fix - 66 - Centuu: 85?
	switch (iRet) {
	case DEF_XSOCKEVENT_QUENEFULL:
	case DEF_XSOCKEVENT_SOCKETERROR:
	case DEF_XSOCKEVENT_CRITICALERROR:
	case DEF_XSOCKEVENT_SOCKETCLOSED:
		DeleteClient(iClientH, true, true);
		if(pBuffer != 0) delete[] pBuffer;
		return;
	}
	if(pBuffer != 0) delete[] pBuffer;
	SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_LOG, DEF_MSGTYPE_CONFIRM, 0, 0, 0);
	m_pClientList[iClientH]->m_bIsWarLocation = false;
	m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 0;
	if ((m_pClientList[iClientH]->m_cSide == 1) && 
		(memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, "elvine", 6) == 0) && 
		(m_pClientList[iClientH]->m_iAdminUserLevel < 1)) {
		m_pClientList[iClientH]->m_dwWarBeginTime = timeGetTime();
		m_pClientList[iClientH]->m_bIsWarLocation = true;
		CheckForceRecallTime(iClientH);
	}
	else if ((m_pClientList[iClientH]->m_cSide == 2) && 
		(memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, "aresden", 7) == 0) && 
		(m_pClientList[iClientH]->m_iAdminUserLevel == 0)) {
		m_pClientList[iClientH]->m_dwWarBeginTime = timeGetTime();
		m_pClientList[iClientH]->m_bIsWarLocation = true;
		CheckForceRecallTime(iClientH);	
	}
	else if ((m_pClientList[iClientH]->m_cSide == 0) &&
		(m_bIsCrusadeMode == true) &&
		((strcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, "elvine") == 0) ||
		(strcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, "aresden") == 0))) {
		if (m_pClientList[iClientH]->m_iAdminUserLevel == 0) {
			m_pClientList[iClientH]->m_dwWarBeginTime = timeGetTime();
			m_pClientList[iClientH]->m_bIsWarLocation = true;
			CheckForceRecallTime(iClientH);	
			m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 1;
		}
	}
	// Heldenian: TP to GodH go to appropriate place...
	if ((m_bIsHeldenianMode == true) && (bRet == true) && (cMapName == 0))
	{	if (memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, "HRampart", 8) == 0)
		{	if (m_pClientList[iClientH]->m_iAdminUserLevel == 0)
			{	if (memcmp(cDestMapName, "GodH", 4) == 0)
				{	if (m_pClientList[iClientH]->m_cSide == m_sLastHeldenianWinner)
					{	iDestX = 54;
						iDestY = 67;
						cDir = 4;
					}else
					{	iDestX = 192 + iDice(1,20);
						iDestY = 166 + iDice(1,10);
						cDir = 7;
	}	}	}	}	}
	iMapSide = iGetMapLocationSide(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName);

	m_pClientList[iClientH]->m_bIsInsideEnemyBuilding = false;
	if (m_pClientList[iClientH]->m_cSide != iMapSide && iMapSide != 0)  {
		if ((m_pClientList[iClientH]->m_iAdminUserLevel == 0) && 
			(m_pClientList[iClientH]->m_cSide != 0)) {
			m_pClientList[iClientH]->m_dwWarBeginTime = timeGetTime();
			m_pClientList[iClientH]->m_bIsWarLocation = true;
			m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 1;
			m_pClientList[iClientH]->m_bIsInsideEnemyBuilding = true;
		}
	}
	else{
		if ((m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsFightZone == true) &&
			(m_iFightzoneNoForceRecall == false) && 
			(m_pClientList[iClientH]->m_iAdminUserLevel == 0)) {
			m_pClientList[iClientH]->m_dwWarBeginTime = timeGetTime();
			m_pClientList[iClientH]->m_bIsWarLocation = true;
			GetLocalTime(&SysTime);
			m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 2*60*20 - ((SysTime.wHour%2)*20*60 + SysTime.wMinute*20) - 2*20;
		}
		else{
			if ((memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, "arejail", 7) == 0) ||
				(memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, "elvjail", 7) == 0)) {
				if (m_pClientList[iClientH]->m_iAdminUserLevel == 0) {
					m_pClientList[iClientH]->m_bIsWarLocation = true;
					m_pClientList[iClientH]->m_dwWarBeginTime = timeGetTime();
					if (m_pClientList[iClientH]->m_iTimeLeft_ForceRecall == 0) {
						m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 1;
					}
					else if(m_pClientList[iClientH]->m_iTimeLeft_ForceRecall > 100) {
						m_pClientList[iClientH]->m_iTimeLeft_ForceRecall = 100;
					}
				}
			}
		}
	}
	if (m_pClientList[iClientH]->m_iTimeLeft_ForceRecall > 0) {
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_FORCERECALLTIME, m_pClientList[iClientH]->m_iTimeLeft_ForceRecall, 0, 0, 0);
	}
	SendNotifyMsg(0, iClientH, DEF_NOTIFY_SAFEATTACKMODE, 0, 0, 0, 0);
	SendNotifyMsg(0, iClientH, DEF_NOTIFY_DOWNSKILLINDEXSET, m_pClientList[iClientH]->m_iDownSkillIndex, 0, 0, 0);
	SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMPOSLIST, 0, 0, 0, 0);
	_SendQuestContents(iClientH);
	_CheckQuestEnvironment(iClientH);
	if (m_pClientList[iClientH]->m_iSpecialAbilityTime == 0) {
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_SPECIALABILITYENABLED, 0, 0, 0, 0);
	}
	if (m_bIsCrusadeMode == true) {
		if (m_pClientList[iClientH]->m_dwCrusadeGUID == 0) {
			m_pClientList[iClientH]->m_iCrusadeDuty			= 0;
			m_pClientList[iClientH]->m_iConstructionPoint	= 0;
			m_pClientList[iClientH]->m_dwCrusadeGUID = m_dwCrusadeGUID;
		}
		else if (m_pClientList[iClientH]->m_dwCrusadeGUID != m_dwCrusadeGUID) {
			m_pClientList[iClientH]->m_iCrusadeDuty       = 0;
			m_pClientList[iClientH]->m_iConstructionPoint = 0;
			m_pClientList[iClientH]->m_iWarContribution   = 0;
			m_pClientList[iClientH]->m_dwCrusadeGUID = m_dwCrusadeGUID;
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_CRUSADE, (UINT32)m_bIsCrusadeMode, 0, 0, 0, -1);		
		}
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_CRUSADE, (UINT32)m_bIsCrusadeMode, m_pClientList[iClientH]->m_iCrusadeDuty, 0, 0);
	}
	// Heldenian mode on
	else if (m_bIsHeldenianMode == true) 
	{
		sSummonPoints = 10000;
		if (m_pClientList[iClientH]->m_dwHeldenianGUID == 0) 
		{	m_pClientList[iClientH]->m_dwHeldenianGUID    = m_dwHeldenianGUID;
			m_pClientList[iClientH]->m_iConstructionPoint = sSummonPoints;
			m_pClientList[iClientH]->m_iWarContribution   = 0;
		}else if (m_pClientList[iClientH]->m_dwHeldenianGUID != m_dwHeldenianGUID) 
		{	m_pClientList[iClientH]->m_dwHeldenianGUID    = m_dwHeldenianGUID;
			m_pClientList[iClientH]->m_iConstructionPoint = sSummonPoints;
			m_pClientList[iClientH]->m_iWarContribution   = 0;
		}
		if (m_cHeldenianWinner == -1) 
		{	if (m_bHeldenianWarInitiated == true) 
			{	SendNotifyMsg(0, iClientH, DEF_NOTIFY_HELDENIANSTART, 0, 0, 0, 0);
			}else
			{	SendNotifyMsg(0, iClientH, DEF_NOTIFY_HELDENIANTELEPORT, 0, 0, 0, 0);
			}
		}
		m_pClientList[iClientH]->m_cWarType = 2;// character have been informed of heldenian starting...
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_CONSTRUCTIONPOINT, m_pClientList[iClientH]->m_iConstructionPoint, m_pClientList[iClientH]->m_iWarContribution, 1, 0);
		UpdateHeldenianStatus(iClientH);
	}
	else if ((m_pClientList[iClientH]->m_dwCrusadeGUID != 0) && (m_pClientList[iClientH]->m_cWarType == 1)) {
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_CRUSADE, (UINT32)m_bIsCrusadeMode, 0, 0, 0, -1);
	}
	// The Heldenian he has participated is over...
	else if ( (m_pClientList[iClientH]->m_cWarType == 2) // Just finished Heldenian
			&& (m_pClientList[iClientH]->m_dwHeldenianGUID == m_dwHeldenianGUID)
			&& (m_pClientList[iClientH]->m_dwHeldenianGUID != 0)) 
	{	m_pClientList[iClientH]->m_iConstructionPoint = 0;
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_HELDENIANEND, 0, 0, 0, 0);
		m_pClientList[iClientH]->m_cWarType = 0; // Character is informed of Hedenian end... so he'll get XP later
	}
	else {
		m_pClientList[iClientH]->m_dwCrusadeGUID		= 0;
		m_pClientList[iClientH]->m_iWarContribution		= 0;
		m_pClientList[iClientH]->m_iConstructionPoint	= 0;
	}
	SendNotifyMsg(0, iClientH, DEF_NOTIFY_CONSTRUCTIONPOINT, m_pClientList[iClientH]->m_iConstructionPoint, m_pClientList[iClientH]->m_iWarContribution, 1, 0);
	SendNotifyMsg(0, iClientH, DEF_NOTIFY_GIZONITEMUPGRADELEFT, m_pClientList[iClientH]->m_iGizonItemUpgradeLeft, 0, 0, 0);
	
	//Magn0S:: Arena
	if (memcmp(m_pClientList[iClientH]->m_cMapName, "fightzone1", 10) == 0)
	{
		RequestArenaStatus(iClientH);
		RequestArenaStatus(iClientH, true);
	}

	//50Cent - Critical Count Login Fix
	SendNotifyMsg(0, iClientH, DEF_NOTIFY_SUPERATTACKLEFT, 0, 0, 0, 0);

	for (j = 0; j < DEF_MAXQUEST; j++) { // Magn0S:: Multi Quest
		if (m_pClientList[iClientH]->m_iQuest[j] != 0) {
			iQuestNumber = m_pClientList[iClientH]->m_iQuest[j];
			if ((iQuestNumber != 0) && (m_pQuestConfigList[iQuestNumber] != 0))
			{
				iQuestType = m_pQuestConfigList[iQuestNumber]->m_iType;
				if (iQuestType == 1) {
					if (_bCheckIsQuestCompleted(iClientH, j) == true)
					{
						sV1 = m_pClientList[iClientH]->m_iCurQuestCount[j];
						if (sV1 < 0) sV1 = 0;
						SendNotifyMsg(0, iClientH, DEF_NOTIFY_QUESTCOUNTER, j, sV1, 0, 0);
					}
				}
			}
		}
	}

	// SNOOPY: Send gate positions if applicable.
	Notify_ApocalypseGateState(iClientH);
	RefreshPartyCoords(iClientH);
	minimap_update(iClientH);
	minimap_update_mark(iClientH);
	NotifyMapRestrictions(iClientH, false);

	//Magn0S:: NPC Info on Apoc
	if ((m_pClientList[iClientH]->m_bIsOnApocMap == true) && (m_bIsApocalypseMode)) {
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_MONSTERCOUNTAPOC, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_iTotalAliveObject, 0, 0, 0);
	}
}

// notify player who just joined
void CGame::RequestArenaStatus(int iClientH)
{
	int i;
	if (m_pClientList[iClientH] == 0) return;

	for (i = 0; i < DEF_MAXCLIENTS; i++) {
		if ((m_pClientList[i] != 0) && (memcmp(m_pMapList[m_pClientList[i]->m_cMapIndex]->m_cName, "fightzone1", 10) == 0))
		{
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ARENASTATUS, m_pClientList[i]->m_iDGKills, m_pClientList[i]->m_iDGDeaths, 1, m_pClientList[i]->m_cCharName);
		}
	}
}

// update all players with score change
void CGame::RequestArenaStatus(int iSubjectH, bool bUpdate)
{
	int i;
	if (m_pClientList[iSubjectH] == 0) return;
	if (m_pClientList[iSubjectH]->m_bIsInitComplete == false) return;
	if (m_pClientList[iSubjectH]->m_bIsOnWaitingProcess == true) return;

	if (bUpdate)
	{
		for (i = 0; i < DEF_MAXCLIENTS; i++) {
			if ((m_pClientList[i] != 0) && (memcmp(m_pMapList[m_pClientList[i]->m_cMapIndex]->m_cName, "fightzone1", 10) == 0)) //crash line
			{
				SendNotifyMsg(0, i, DEF_NOTIFY_ARENASTATUS, m_pClientList[iSubjectH]->m_iDGKills, m_pClientList[iSubjectH]->m_iDGDeaths, 1, m_pClientList[iSubjectH]->m_cCharName);
			}
		}
	}
	else
	{
		for (i = 0; i < DEF_MAXCLIENTS; i++)
		{
			if ((m_pClientList[i] != 0) && (memcmp(m_pMapList[m_pClientList[i]->m_cMapIndex]->m_cName, "fightzone1", 10) == 0))
			{
				SendNotifyMsg(0, i, DEF_NOTIFY_ARENASTATUS, 0, 0, 0, m_pClientList[iSubjectH]->m_cCharName);
			}
		}
	}
}
//Magn0S:: Leave Arena
void CGame::RequestLeaveArena(int iClientH)
{
	short sX, sY;

	if (m_pClientList[iClientH] == 0) return;
	sX = m_pClientList[iClientH]->m_sX;
	sY = m_pClientList[iClientH]->m_sY;

	if (bDeathmatch) {
		RequestArenaStatus(iClientH, false);
		m_pClientList[iClientH]->m_iDGKills = 0;
		m_pClientList[iClientH]->m_iDGDeaths = 0;
	}

	m_pClientList[iClientH]->m_bIsKilled = false;
	m_pClientList[iClientH]->m_iHP = iGetMaxHP(iClientH);
	m_pClientList[iClientH]->m_iHungerStatus = 100;
	SendNotifyMsg(0, iClientH, DEF_NOTIFY_HP, 0, 0, 0, 0);
	m_pClientList[iClientH]->m_iMP = iGetMaxMP(iClientH);
	SendNotifyMsg(0, iClientH, DEF_NOTIFY_MP, 0, 0, 0, 0);
	m_pClientList[iClientH]->m_iSP = iGetMaxSP(iClientH);
	SendNotifyMsg(0, iClientH, DEF_NOTIFY_SP, 0, 0, 0, 0);

	if (c_team->bteam)
	{
		if (m_pClientList[iClientH]->m_cSide == 1)
			RequestTeleportHandler(iClientH, "2   ", "aresden", -1, -1, true);
		else
			RequestTeleportHandler(iClientH, "2   ", "elvine", -1, -1, true);
	}
	else {
		if (m_pClientList[iClientH]->m_cSide == 1)
			RequestTeleportHandler(iClientH, "2   ", "aresden", -1, -1);
		else
			RequestTeleportHandler(iClientH, "2   ", "elvine", -1, -1);
	}
}

void CGame::RequestRestartHandler(int iClientH)
{
 char  cTmpMap[32];

	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsKilled == true)
	{
		if (memcmp(m_pClientList[iClientH]->m_cMapName, "fightzone1", 10) == 0 || memcmp(m_pClientList[iClientH]->m_cMapName, "team", 4) == 0)
		{
			// MORLA2.2 - Revive en Deathmach Map
			m_pClientList[iClientH]->m_bIsKilled = false;
			m_pClientList[iClientH]->m_iHP = iGetMaxHP(iClientH);
			m_pClientList[iClientH]->m_iHungerStatus = 100;
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_HP, 0, 0, 0, 0); 
			m_pClientList[iClientH]->m_iMP = iGetMaxMP(iClientH);
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_MP, 0, 0, 0, 0); 
			m_pClientList[iClientH]->m_iSP = iGetMaxSP(iClientH);
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_SP, 0, 0, 0, 0); 

			RequestTeleportHandler(iClientH, "2   ", m_pClientList[iClientH]->m_cMapName, -1, -1);
		}
		else {
			strcpy(cTmpMap,m_pClientList[iClientH]->m_cMapName) ;
			ZeroMemory(m_pClientList[iClientH]->m_cMapName, sizeof(m_pClientList[iClientH]->m_cMapName));
			if (m_pClientList[iClientH]->m_cSide == 0) {
				strcpy(m_pClientList[iClientH]->m_cMapName, "default");
			}
			else if (m_pClientList[iClientH]->m_cSide == 1) {
				if (m_bIsCrusadeMode == true) {
					if (m_pClientList[iClientH]->m_iDeadPenaltyTime > 0) {
						ZeroMemory(m_pClientList[iClientH]->m_cLockedMapName, sizeof(m_pClientList[iClientH]->m_cLockedMapName));
						strcpy(m_pClientList[iClientH]->m_cLockedMapName, "aresden");
						m_pClientList[iClientH]->m_iLockedMapTime = 300;
						m_pClientList[iClientH]->m_iDeadPenaltyTime = 600;
					}
					else {
						memcpy(m_pClientList[iClientH]->m_cMapName, "cityhall_1", 10);
						m_pClientList[iClientH]->m_iDeadPenaltyTime = 600;
					}
				}
				else if (strcmp(cTmpMap, "elvine") == 0){
					memcpy(m_pClientList[iClientH]->m_cMapName, "elvjail", 7);
					strcpy(m_pClientList[iClientH]->m_cLockedMapName, "elvjail");
					m_pClientList[iClientH]->m_iLockedMapTime = m_sJailTime;
				}
				else  {
					memcpy(m_pClientList[iClientH]->m_cMapName, "cityhall_1", 10);
				}
			}
			else {
				if (m_bIsCrusadeMode == true) {
					if (m_pClientList[iClientH]->m_iDeadPenaltyTime > 0) {
						ZeroMemory(m_pClientList[iClientH]->m_cLockedMapName, sizeof(m_pClientList[iClientH]->m_cLockedMapName));
						strcpy(m_pClientList[iClientH]->m_cLockedMapName, "elvine");
						m_pClientList[iClientH]->m_iLockedMapTime = 300;
						m_pClientList[iClientH]->m_iDeadPenaltyTime = 600;
					}
					else {
						memcpy(m_pClientList[iClientH]->m_cMapName, "cityhall_2", 10);
						m_pClientList[iClientH]->m_iDeadPenaltyTime = 600;
					}
				}
				if (strcmp(cTmpMap, "aresden") == 0){
					memcpy(m_pClientList[iClientH]->m_cMapName, "arejail", 7);
					strcpy(m_pClientList[iClientH]->m_cLockedMapName, "arejail");
					m_pClientList[iClientH]->m_iLockedMapTime = m_sJailTime;
				}
				else {
					memcpy(m_pClientList[iClientH]->m_cMapName, "cityhall_2", 10);
				}
			}
			m_pClientList[iClientH]->m_bIsKilled = false;
			m_pClientList[iClientH]->m_iHP = iGetMaxHP(iClientH);
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_HP, 0, 0, 0, 0);
			m_pClientList[iClientH]->m_iHungerStatus = 100;
			ZeroMemory(cTmpMap, sizeof(cTmpMap));
			strcpy(cTmpMap, m_pClientList[iClientH]->m_cMapName);
			RequestTeleportHandler(iClientH, "2   ", cTmpMap, -1, -1);
		}
	}
}

void CGame::InitPlayerData(int iClientH, char * pData, UINT32 dwSize)
{
 char  * cp, cName[11], cData[256], cTxt[256], cGuildStatus, cPoints = 0;
 UINT32 * dwp;
 UINT16  * wp;
 int iRet, iTemp, iTemp2, i, iQuestType, iQuestNumber, j;
 bool bRet;
 short sV1;

	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == true) return;
	
	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
	ZeroMemory(cName, sizeof(cName));
	memcpy(cName, cp, 10);
	cp += 10;

	cp++;

	cGuildStatus = *cp;
	cp++;

	m_pClientList[iClientH]->m_iHitRatio     = 0;
	m_pClientList[iClientH]->m_iDefenseRatio = 0;
	m_pClientList[iClientH]->m_cSide         = 0;

	SendNotifyMsg(0, iClientH, DEF_NOTIFY_HUNGER, m_pClientList[iClientH]->m_iHungerStatus, 0, 0, 0); // MORLA2 - Muestra el hunger status
	SendNotifyMsg(0, iClientH, DEF_NOTIFY_REPDGDEATHS, 0, m_pClientList[iClientH]->m_iDeaths, m_pClientList[iClientH]->m_iRating, 0);

	SendNotifyMsg(0, iClientH, DEF_NOTIFY_SERVERTIME, 0, 0, 0, 0);

	bRet = _bDecodePlayerDatafileContents(iClientH, cp, dwSize - 19);
	if (bRet == false) {
		wsprintf(G_cTxt, "(HACK?) Character(%s) data error!", m_pClientList[iClientH]->m_cCharName);
		PutLogList(G_cTxt);
		DeleteClient(iClientH, false, true, true, false); 
		return;
	}
	
	if ((m_pClientList[iClientH]->m_sX == -1) && (m_pClientList[iClientH]->m_sY == -1)) {
		GetMapInitialPoint(m_pClientList[iClientH]->m_cMapIndex, &m_pClientList[iClientH]->m_sX, &m_pClientList[iClientH]->m_sY, m_pClientList[iClientH]->m_cLocation);
	}
	SetPlayingStatus(iClientH);	// New 17/05/2004
	iTemp = m_pClientList[iClientH]->m_iStatus;
	iTemp = 0x0FFFFFFF & iTemp;
	iTemp2 = iGetPlayerABSStatus(iClientH);
	iTemp = iTemp | (iTemp2 << 28);
	m_pClientList[iClientH]->m_iStatus = iTemp;

	if (m_pClientList[iClientH]->m_iLevel > 49 && m_pClientList[iClientH]->m_bIsPlayerCivil) {
		ForceChangePlayMode(iClientH);
	}
	
	CalcTotalItemEffect(iClientH, -1, true);
	iCalcTotalWeight(iClientH);
	
	bCheckMagicInt(iClientH);
	CheckDestroyFragileItem(iClientH);

	// Recover summons you may had before deco...
	iRecoverFollowers(iClientH, false);
	
	if (m_pClientList[iClientH]->m_iAdminUserLevel > 0) {
		SetInvisibilityFlag(iClientH, DEF_OWNERTYPE_PLAYER, true);
		m_pClientList[iClientH]->m_iNextLevelExp = 1;
	}		
	else m_pClientList[iClientH]->m_iNextLevelExp = m_iLevelExpTable[m_pClientList[iClientH]->m_iLevel + 1];

	SetClass(iClientH);

	if ((cGuildStatus == 0) && (memcmp(m_pClientList[iClientH]->m_cGuildName, "NONE", 4) != 0)) {
		ZeroMemory(m_pClientList[iClientH]->m_cGuildName, sizeof(m_pClientList[iClientH]->m_cGuildName));
		strcpy(m_pClientList[iClientH]->m_cGuildName, "NONE");
		m_pClientList[iClientH]->m_iGuildRank = -1;
		m_pClientList[iClientH]->m_iGuildGUID = -1;
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_GUILDDISBANDED, 0, 0, 0, m_pClientList[iClientH]->m_cGuildName);
	}
	// new
	for (j = 0; j < DEF_MAXQUEST; j++) { // Magn0S:: Multi Quest
		if (m_pClientList[iClientH]->m_iQuest[j] != 0) {
			iQuestNumber = m_pClientList[iClientH]->m_iQuest[j];
			if ((iQuestNumber != 0) && (m_pQuestConfigList[iQuestNumber] != 0))
			{
				iQuestType = m_pQuestConfigList[iQuestNumber]->m_iType;
				if (iQuestType == 1)
				{
					if (_bCheckIsQuestCompleted(iClientH, j) == true)
					{
						sV1 = m_pClientList[iClientH]->m_iCurQuestCount[j];
						if (sV1 > 0)
						{
							SendNotifyMsg(0, iClientH, DEF_NOTIFY_QUESTCOUNTER, j, sV1, 0, 0);
						}
					}
				}
			}
		}
	}
	// new - handles cPoints
	if (m_pClientList[iClientH]->m_pIsProcessingAllowed == true) {
		cPoints = 0;
		if ((m_pClientList[iClientH]->m_cSide != m_sLastHeldenianWinner) && (m_sLastHeldenianWinner != 0)) {
			cPoints = 100;
		}
	}
	if (m_pClientList[iClientH] == 0) {
		wsprintf(cTxt, "<%d> InitPlayerData error - Socket error! Disconnected.", iClientH);
		PutLogList(cTxt);
		return;
	}
	ZeroMemory(cData, sizeof(cData));
	dwp  = (UINT32 *)(cData + DEF_INDEX4_MSGID);
	*dwp = MSGID_RESPONSE_INITPLAYER;
	wp   = (UINT16 *)(cData + DEF_INDEX2_MSGTYPE);
	*wp  = DEF_MSGTYPE_CONFIRM;
	iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 6);
	switch (iRet) {
	case DEF_XSOCKEVENT_QUENEFULL:
	case DEF_XSOCKEVENT_SOCKETERROR:
	case DEF_XSOCKEVENT_CRITICALERROR:
	case DEF_XSOCKEVENT_SOCKETCLOSED:
		wsprintf(cTxt, "<%d> InitPlayerData - Socket error! Disconnected.", iClientH);
		PutLogList(cTxt);
		DeleteClient(iClientH, false, true, true, false); // 4 true
		return;
	}
	m_pClientList[iClientH]->m_bIsInitComplete = true;
	bSendMsgToLS(MSGID_ENTERGAMECONFIRM, iClientH, true, false); // 3 true
	if (m_pClientList[iClientH]->m_iPartyID != 0) {	
		ZeroMemory(cData, sizeof(cData));
		cp	 = (char *)cData;
		dwp	 = (UINT32 *)cp;
		*dwp = MSGID_PARTYOPERATION;
		cp	+= 4;

		wp	 = (UINT16*)cp;
		*wp  = 5; 
		cp  += 2;

		wp   = (UINT16 *)cp;
		*wp  = iClientH;
		cp  += 2;

		memcpy(cp, m_pClientList[iClientH]->m_cCharName, 10);
		cp  += 10;

		wp   = (UINT16 *) cp;
		*wp  = m_pClientList[iClientH]->m_iPartyID;
		cp  += 2;

		SendMsgToGateServer(MSGID_PARTYOPERATION, iClientH, cData);	
		for (i = 0; i < DEF_MAXPARTYMEMBERS; i++) {
			if (m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iIndex[i] == 0) {
				m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iIndex[i] = iClientH;
				m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iTotalMembers++;
				wsprintf(G_cTxt, "PartyID:%d member:%d In Total:%d", m_pClientList[iClientH]->m_iPartyID, iClientH, m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iTotalMembers);
				PutLogList(G_cTxt);
				break;
			}
		}
	}

	m_pClientList[iClientH]->m_dwLastActionTime = timeGetTime();

	if (m_iTotalClients > DEF_MAXONESERVERUSERS) {
		switch (iDice(1,2)) {
			case 1: 
				RequestTeleportHandler(iClientH, "2   ", "bisle", -1, -1); 
				break;
			case 2: 
				switch (m_pClientList[iClientH]->m_cSide) {
					case 0: RequestTeleportHandler(iClientH, "2   ", "default", -1, -1); break;
					case 1: RequestTeleportHandler(iClientH, "2   ", "cityhall_1", -1, -1); break;
					case 2: RequestTeleportHandler(iClientH, "2   ", "cityhall_2", -1, -1); break;
				}
				break;
		}
	}
	SendLoginData(iClientH);
}

void CGame::SendLoginData(int client)
{
	if (!m_pClientList[client]) return;

	if (m_pClientList[client]->IsInsideTeam())
	{
		c_team->NotPoints(client);
	}
}

void CGame::AdminOrder_BanIP(int iClientH, char *pData, UINT32 dwMsgSize)
{
 char seps[] = "= ";
 char * token, cBuff[256];
 class CStrTok * pStrTok;
 char * pk;
 char cNick[20], cIPtoBan[21];
 int i;
 FILE * pFile;

	if (m_pClientList[iClientH] == 0) return;
	if ((dwMsgSize) <= 0) return;
	if (m_pClientList[iClientH]->m_iAdminUserLevel <= 3) {
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
		return;
	}
	if (m_pClientList[iClientH]->m_bIsAdminCommandEnabled == false) return;

	ZeroMemory(cBuff, sizeof(cBuff));
	memcpy(cBuff, pData, dwMsgSize);
	pStrTok = new class CStrTok(cBuff, seps);
	token = pStrTok->pGet();
	token = pStrTok->pGet();
	if (token == 0) {
		delete pStrTok;
		return;
	}
	strcpy(cNick, token);
	pk = pStrTok->pGet();

	ZeroMemory(cIPtoBan, sizeof(cIPtoBan));

	for (i = 0; i < DEF_MAXCLIENTS; i++) {
		if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cCharName, cNick, strlen(cNick)) == 0)) {
			strcpy(cIPtoBan, m_pClientList[i]->m_cIPaddress);
			//opens cfg file
			pFile = fopen("GameConfigs\\BannedList.cfg", "a");
			//shows log
			wsprintf(G_cTxt,"<%d> Client IP Banned: (%s)", i, cIPtoBan);
			PutLogList(G_cTxt);
				//modifys cfg file
				fprintf(pFile, "banned-ip = %s", cIPtoBan);
				fprintf(pFile, "\n");
				fclose(pFile);
				for (auto x = 0; x < DEF_MAXBANNED; x++)
					if (strlen(m_stBannedList[x].m_cBannedIPaddress) == 0)
						strcpy(m_stBannedList[x].m_cBannedIPaddress, cIPtoBan);
			//disconnects player, and he can't connect again.. :)
			DeleteClient(i, true, true);
		}
	}
	delete pStrTok;
}


void CGame::AdminOrder_BanPj(int iClientH, char* pData, UINT32 dwMsgSize) // MORLA 2.12 - Morlear PJ
{
	char   seps[] = "= \t\n";
	char   * token, cBuff[256], cPlayerName[11];
	class  CStrTok * pStrTok;


	if (m_pClientList[iClientH] == 0) return;
	if ((dwMsgSize)	<= 0) return;

	if (m_pClientList[iClientH]->m_iAdminUserLevel < 7) return;

	ZeroMemory(cPlayerName, sizeof(cPlayerName));
	ZeroMemory(cBuff, sizeof(cBuff));
	memcpy(cBuff, pData, dwMsgSize);

	pStrTok = new class CStrTok(cBuff, seps);
	token = pStrTok->pGet();

	token = pStrTok->pGet();
	if (token == 0) {
		delete pStrTok;
		return;
	}

	if (strlen(token) > 10) {
		memcpy(cPlayerName, token, 10);
	}
	else{
		memcpy(cPlayerName, token, strlen(token));
	}

	for(int i = 1; i < DEF_MAXCLIENTS; i++) {
		if (m_pClientList[i] != 0) {
			if (memcmp(cPlayerName, m_pClientList[i]->m_cCharName, 10) == 0) {
				wsprintf(cBuff,"GM Order(%s): Ban Player (%s)", m_pClientList[iClientH]->m_cCharName, m_pClientList[i]->m_cCharName);					
				SendNotifyMsg(0, i, DEF_NOTIFY_MORLEARPJ, 0, 0, 0, 0); 
				
				break;
			}
		}
	}
	delete pStrTok;
}

void CGame::PlayerOrder_Criticals(int iClientH)
{
	if (m_pClientList[iClientH] == 0) return;
	UINT32 dwGoldCount = dwGetItemCount(iClientH, "Gold");
	if (m_pClientList[iClientH]->m_iSuperAttackLeft < 1000 && dwGoldCount >= 40000)
	{
		m_pClientList[iClientH]->m_iSuperAttackLeft += 500;
		if (m_pClientList[iClientH]->m_iSuperAttackLeft > 1000) m_pClientList[iClientH]->m_iSuperAttackLeft = 1000;
		SetItemCount(iClientH, "Gold", dwGoldCount - 40000);
		
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_SUPERATTACKLEFT, 0, 0, 0, 0);
		ShowClientMsg(iClientH, "You've received 500 Criticals!");
	}
	else ShowClientMsg(iClientH, "Max Crits 1000 - Req 40k Gold");
}

void CGame::PlayerOrder_ChangeCity(int iClientH, bool bChange) 
{
	if (m_pClientList[iClientH] == 0) return;  
	if (m_pClientList[iClientH]->m_iGizonItemUpgradeLeft >= 1000) {
		if (bChange == true) { 
			//Magn0S:: Adjusted to change city, not make Traveller
			switch (m_pClientList[iClientH]->m_cSide) {
			case 1:
				strcpy(m_pClientList[iClientH]->m_cLocation, "elvine");
				SendNotifyMsg(iClientH, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, "You've changed your nation to Elvine.");
				break;
			case 2:
				strcpy(m_pClientList[iClientH]->m_cLocation, "aresden");
				SendNotifyMsg(iClientH, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, "You've changed your nation to Aresden.");
				break;
			}

			if (memcmp(m_pClientList[iClientH]->m_cGuildName, "NONE", 4) != 0) {
				ZeroMemory(m_pClientList[iClientH]->m_cGuildName, sizeof(m_pClientList[iClientH]->m_cGuildName));
				strcpy(m_pClientList[iClientH]->m_cGuildName, "NONE");
				m_pClientList[iClientH]->m_iGuildRank = -1;
				m_pClientList[iClientH]->m_iGuildGUID = -1;
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_GUILDDISBANDED, 0, 0, 0, m_pClientList[iClientH]->m_cGuildName);
			}

			m_pClientList[iClientH]->m_iGizonItemUpgradeLeft -= 1000;
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_GIZONITEMUPGRADELEFT, m_pClientList[iClientH]->m_iGizonItemUpgradeLeft, 0, 0, 0);
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_CHANGEPLAYMODE, 0, 0, 0, m_pClientList[iClientH]->m_cLocation);
			SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, m_pClientList[iClientH]->m_iLevel, 0, 0, 0);
		}
		
	}
	else {
		SendNotifyMsg(iClientH, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, "You don't have 1000 Majestic Points.");
		
	}
}

void CGame::AFKChecker()
{}

void CGame::ParseCommand(char * pMsg) 
{ 
 char   seps[] = "= \t\n"; 
 char   * token, * token2; 
 class  CStrTok * pStrTok; 
 char buff[256]; 
 char ss[256];

	if (pMsg == 0) return; 

	pStrTok = new class CStrTok(pMsg, seps); 
	token = pStrTok->pGet(); 
	token2 = pStrTok->pGet(); 

	ZeroMemory(ss, sizeof(ss));
	wsprintf(ss, " %s", pMsg);
	for (int i = 1; i < DEF_MAXCLIENTS; i++) { 
		if (m_pClientList[i] != 0) { 
			ShowClientMsg(i, ss); 
		} 
	}  
	wsprintf(buff, "(!) '%s'.", ss); 
	PutLogList(buff);   
	delete pStrTok;
}


//LifeX Fix Revive 14/10/2019
void CGame::AdminOrder_Revive(int iClientH, char * pData, UINT32 dwMsgSize)
{
	char   seps[] = "= \t\n";
	char   * token, cTargetName[11], cBuff[256], cNpcName[21], cNpcWaypoint[11];
	class  CStrTok * pStrTok;
	int i;
	int sAttackerWeapon, sDamage, sHP;

	if (m_pClientList[iClientH] == 0) return;
	if ((dwMsgSize)	<= 0) return;

	if (m_pClientList[iClientH]->m_iAdminUserLevel < m_iAdminLevelGMRevive) {
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
		return;
	}

	ZeroMemory(cNpcWaypoint, sizeof(cNpcWaypoint));
	ZeroMemory(cTargetName, sizeof(cTargetName));
	ZeroMemory(cNpcName, sizeof(cNpcName));
	ZeroMemory(cBuff, sizeof(cBuff));
	memcpy(cBuff, pData, dwMsgSize);

	pStrTok = new class CStrTok(cBuff, seps);


	token = pStrTok->pGet();
	token = pStrTok->pGet();

	if (token != 0) {
		if (strlen(token) > 10) return;
		else strcpy(cTargetName, token);
	}
	else return;

	token = pStrTok->pGet();

	if (token != 0) {
		sDamage = atoi(token);
	} else {
		sDamage = 1;
	}
	token = pStrTok->pGet();
	if (token != 0) {
		sHP = atoi(token);
	} else {
		sHP = 1;
	}

		for (i = 1; i < DEF_MAXCLIENTS; i++) 
			if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cCharName, cTargetName, 10) == 0)) {
				// Â¸Ã±Ã‡Â¥ Ã„Â³Â¸Â¯Ã…ÃÂ¸Â¦ ÃƒÂ£Â¾Ã’Â´Ã™. 	

				m_pClientList[i]->m_iHP = sHP;
				if (iGetMaxHP(i) < m_pClientList[i]->m_iHP) m_pClientList[i]->m_iHP = iGetMaxHP(i);
				m_pClientList[i]->m_bIsKilled = false;
				m_pClientList[i]->m_iLastDamage = sDamage;
				SendNotifyMsg(0, i, DEF_NOTIFY_HP, 0, 0, 0, 0);
				sAttackerWeapon = 1;
				SendEventToNearClient_TypeA(i, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTDAMAGE, sDamage, sAttackerWeapon, 0);
				m_pMapList[m_pClientList[i]->m_cMapIndex]->ClearOwner(14, i, DEF_OWNERTYPE_PLAYER, m_pClientList[i]->m_sX, m_pClientList[i]->m_sY);
				m_pMapList[m_pClientList[i]->m_cMapIndex]->SetDeadOwner(i, DEF_OWNERTYPE_PLAYER, m_pClientList[i]->m_sX, m_pClientList[i]->m_sY);

				delete pStrTok;
				return;
			}

			// Ã‡Ã¶Ã€Ã§ ÃÂ¢Â¼Ã“ÃÃŸÃ€ÃŒ Â¾Ã†Â´ÃÂ´Ã™.
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_PLAYERNOTONGAME, 0, 0, 0, cTargetName);
}

bool CGame::bGetEmptyArea(short sOwnerH, char cMapIndex, short pX, short pY, char cArea)
{
	short sAreaSquared;
	int i;

	sAreaSquared = cArea * cArea;
	for (i = 0; i < sAreaSquared; i++) {
		if (m_pMapList[cMapIndex]->bGetMoveableArea(sOwnerH, pX + _tmp_cEmptyPosX[i], pY + _tmp_cEmptyPosY[i]) == false) return false;
		if (m_pMapList[cMapIndex]->bGetIsTeleport(pX + _tmp_cEmptyPosX[i], pY + _tmp_cEmptyPosY[i]) == true) return false;
	}
	return true;
}

bool CGame::bGetEmptyPosition(short* pX, short* pY, char cMapIndex)
{
	int i;
	short sX, sY;

	for (i = 0; i < 25; i++)
		if ((m_pMapList[cMapIndex]->bGetMoveable(*pX + _tmp_cEmptyPosX[i], *pY + _tmp_cEmptyPosY[i]) == true) &&
			(m_pMapList[cMapIndex]->bGetIsTeleport(*pX + _tmp_cEmptyPosX[i], *pY + _tmp_cEmptyPosY[i]) == false)) {
			sX = *pX + _tmp_cEmptyPosX[i];
			sY = *pY + _tmp_cEmptyPosY[i];
			*pX = sX;
			*pY = sY;
			return true;
		}
	GetMapInitialPoint(cMapIndex, &sX, &sY);
	*pX = sX;
	*pY = sY;

	return false;
}

void CGame::NpcBehavior_Attack(int iNpcH)
{
	int   iMagicType;
	short sX, sY, dX, dY;
	char  cDir;
	UINT32 dwTime = timeGetTime();
	bool bFly = false;

	if (m_pNpcList[iNpcH] == 0) return;
	if (m_pNpcList[iNpcH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] != 0) return;
	if (m_pNpcList[iNpcH]->m_bIsKilled == true) return;

	switch (m_pNpcList[iNpcH]->m_cActionLimit) {
	case 1:
	case 2:
	case 3:
	case 4:
	case 8: // Heldenian gate
		return;

	case 5:
		if (m_pNpcList[iNpcH]->m_iBuildCount > 0) return;
		break;
	}
	int iStX, iStY;
	if (m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex] != 0) {
		iStX = m_pNpcList[iNpcH]->m_sX / 20;
		iStY = m_pNpcList[iNpcH]->m_sY / 20;
		m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_stTempSectorInfo[iStX][iStY].iMonsterActivity++;
	}
	if (m_pNpcList[iNpcH]->m_sBehaviorTurnCount == 0)
		m_pNpcList[iNpcH]->m_iAttackCount = 0;
	m_pNpcList[iNpcH]->m_sBehaviorTurnCount++;
	if (m_pNpcList[iNpcH]->m_sBehaviorTurnCount > 20) {
		m_pNpcList[iNpcH]->m_sBehaviorTurnCount = 0;
		if ((m_pNpcList[iNpcH]->m_bIsPermAttackMode == false))
			m_pNpcList[iNpcH]->m_cBehavior = DEF_BEHAVIOR_MOVE;
		return;
	}
	sX = m_pNpcList[iNpcH]->m_sX;
	sY = m_pNpcList[iNpcH]->m_sY;
	switch (m_pNpcList[iNpcH]->m_cTargetType) {
	case DEF_OWNERTYPE_PLAYER:
		if (m_pClientList[m_pNpcList[iNpcH]->m_iTargetIndex] == 0) {
			m_pNpcList[iNpcH]->m_sBehaviorTurnCount = 0;
			m_pNpcList[iNpcH]->m_cBehavior = DEF_BEHAVIOR_MOVE;
			return;
		}
		dX = m_pClientList[m_pNpcList[iNpcH]->m_iTargetIndex]->m_sX;
		dY = m_pClientList[m_pNpcList[iNpcH]->m_iTargetIndex]->m_sY;
		break;

	case DEF_OWNERTYPE_NPC:
		if (m_pNpcList[m_pNpcList[iNpcH]->m_iTargetIndex] == 0) {
			m_pNpcList[iNpcH]->m_sBehaviorTurnCount = 0;
			m_pNpcList[iNpcH]->m_cBehavior = DEF_BEHAVIOR_MOVE;
			return;
		}
		dX = m_pNpcList[m_pNpcList[iNpcH]->m_iTargetIndex]->m_sX;
		dY = m_pNpcList[m_pNpcList[iNpcH]->m_iTargetIndex]->m_sY;
		break;
	}
	if ((iGetDangerValue(iNpcH, dX, dY) > m_pNpcList[iNpcH]->m_cBravery) &&
		(m_pNpcList[iNpcH]->m_bIsPermAttackMode == false) &&
		(m_pNpcList[iNpcH]->m_cActionLimit != 5)) {

		m_pNpcList[iNpcH]->m_sBehaviorTurnCount = 0;
		m_pNpcList[iNpcH]->m_cBehavior = DEF_BEHAVIOR_FLEE;
		return;
	}

	//   Bravery  (5) 
	if ((m_pNpcList[iNpcH]->m_iHP <= 2) && (iDice(1, m_pNpcList[iNpcH]->m_cBravery) <= 3) &&
		(m_pNpcList[iNpcH]->m_bIsPermAttackMode == false) &&
		(m_pNpcList[iNpcH]->m_cActionLimit != 5)) {

		m_pNpcList[iNpcH]->m_sBehaviorTurnCount = 0;
		m_pNpcList[iNpcH]->m_cBehavior = DEF_BEHAVIOR_FLEE;
		return;
	}
	cDir = m_Misc.cGetNextMoveDir(sX, sY, dX, dY);
	if (cDir == 0) return;
	if (m_pNpcList[iNpcH]->m_sAreaSize != 0) {
		bFly = m_pMapList[m_pClientList[iNpcH]->m_cMapIndex]->bCheckFlySpaceAvailable(dX, dY, cDir, iNpcH);
	}
	m_pNpcList[iNpcH]->m_cDir = cDir;
	if (((abs(sX - dX) <= 1) && (abs(sY - dY) <= 1)) && (m_pNpcList[iNpcH]->m_sAreaSize == 0)) 
	{
		if (m_pNpcList[iNpcH]->m_cActionLimit == 5) 
		{
			switch (m_pNpcList[iNpcH]->m_sType) {
			case 87: // CT
				SendEventToNearClient_TypeA(iNpcH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTATTACK, dX, dY, 2);
				iCalculateAttackEffect(m_pNpcList[iNpcH]->m_iTargetIndex, m_pNpcList[iNpcH]->m_cTargetType, iNpcH, DEF_OWNERTYPE_NPC, dX, dY, 2, false, false, false, false);
				break;
			case 36: // Crossbow Guard Tower: 
				SendEventToNearClient_TypeA(iNpcH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTATTACK, m_pNpcList[iNpcH]->m_sX + _tmp_cTmpDirX[cDir], m_pNpcList[iNpcH]->m_sY + _tmp_cTmpDirY[cDir], 2); // È°
				iCalculateAttackEffect(m_pNpcList[iNpcH]->m_iTargetIndex, m_pNpcList[iNpcH]->m_cTargetType, iNpcH, DEF_OWNERTYPE_NPC, dX, dY, 2, false, false, false, false);
				break;
			case 37: // Cannon Guard Tower: 
			case 89: // AGT
				SendEventToNearClient_TypeA(iNpcH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTATTACK, dX, dY, 1);
				m_pNpcList[iNpcH]->m_iMagicHitRatio = 1000;
				NpcMagicHandler(iNpcH, dX, dY, 61);
				break;
			}
		}
		else {
			SendEventToNearClient_TypeA(iNpcH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTATTACK, m_pNpcList[iNpcH]->m_sX + _tmp_cTmpDirX[cDir], m_pNpcList[iNpcH]->m_sY + _tmp_cTmpDirY[cDir], 1);
			iCalculateAttackEffect(m_pNpcList[iNpcH]->m_iTargetIndex, m_pNpcList[iNpcH]->m_cTargetType, iNpcH, DEF_OWNERTYPE_NPC, dX, dY, 1, false, false);
			
		}
		m_pNpcList[iNpcH]->m_iAttackCount++;
		if ((m_pNpcList[iNpcH]->m_bIsPermAttackMode == false) && (m_pNpcList[iNpcH]->m_cActionLimit == 0)) {
			switch (m_pNpcList[iNpcH]->m_iAttackStrategy) {
			case DEF_ATTACKAI_EXCHANGEATTACK:
				m_pNpcList[iNpcH]->m_sBehaviorTurnCount = 0;
				m_pNpcList[iNpcH]->m_cBehavior = DEF_BEHAVIOR_FLEE;
				break;

			case DEF_ATTACKAI_TWOBYONEATTACK:
				if (m_pNpcList[iNpcH]->m_iAttackCount >= 2) {
					m_pNpcList[iNpcH]->m_sBehaviorTurnCount = 0;
					m_pNpcList[iNpcH]->m_cBehavior = DEF_BEHAVIOR_FLEE;
				}
				break;
			}
		}
	}
	else {
		if ((m_pNpcList[iNpcH]->m_cMagicLevel > 0) && (iDice(1, 3) == 2) &&
			(abs(sX - dX) <= 11) && (abs(sY - dY) <= 9)) {
			iMagicType = -1;
			switch (m_pNpcList[iNpcH]->m_cMagicLevel) {
			case 1:
				if (m_pMagicConfigList[0]->m_sValue1 <= m_pNpcList[iNpcH]->m_iMana)
					iMagicType = 0;
				break;

			case 2:
				switch (iDice(1, 2)) {
				case 1: iMagicType = 0; break; // Ice-Strike
				case 2: iMagicType = 10; break; // Ice-Strike
				}
				break;

			case 3: // Orc-Mage
				switch (iDice(1, 2)) {
				case 1: iMagicType = 20; break; // Ice-Strike
				case 2: iMagicType = 10; break; // Ice-Strike
				}
				break;

			case 4:
				switch (iDice(1, 4)) {
				case 1: iMagicType = 30; break; // Ice-Strike
				case 2: iMagicType = 37; break; // Ice-Strike
				case 3: iMagicType = 20; break; // Ice-Strike
				case 4: iMagicType = 10; break; // Ice-Strike
				}
				break;

			case 5: // Rudolph, Cannibal-Plant, Cyclops
				switch (iDice(1, 5)) {
				case 1: iMagicType = 43; break; // Ice-Strike
				case 2: iMagicType = 30; break; // Ice-Strike
				case 3: iMagicType = 37; break; // Ice-Strike
				case 4: iMagicType = 20; break; // Ice-Strike
				case 5: iMagicType = 10; break; // Ice-Strike
				}
				break;

			case 6: // Tentocle, Liche
				switch (iDice(1, 6)) {
				case 1: iMagicType = 51; break; // Blizzard
				case 2: iMagicType = 43; break; // Ice-Strike
				case 3: iMagicType = 30; break; // Ice-Strike
				case 4: iMagicType = 37; break; // Ice-Strike
				case 5: iMagicType = 20; break; // Ice-Strike
				case 6: iMagicType = 10; break; // Ice-Strike
				}
				break;

			case 7: // Barlog, Fire-Wyvern, MasterMage-Orc , LightWarBeatle, GHK, GHKABS, TK, BG
				// Sor, Gagoyle, Demon
				switch (iDice(1, 5)) {
				case 1: iMagicType = 70; break; // Blizzard
				case 2: iMagicType = 61; break; // Ice-Strike
				case 3: iMagicType = 60; break; // Ice-Strike
				case 4: iMagicType = 51; break; // Ice-Strike
				case 5: iMagicType = 43; break; // Ice-Strike
				}
				break;

			case 8: // Unicorn, Centaurus
				switch (iDice(1, 4)) {
				case 1: iMagicType = 35; break; // Blizzard
				case 2: iMagicType = 60; break; // Ice-Strike
				case 3: iMagicType = 51; break; // Ice-Strike
				case 4: iMagicType = 43; break; // Ice-Strike
				}
				break;

			case 9: // Tigerworm
				if ((m_pMagicConfigList[74]->m_sValue1 <= m_pNpcList[iNpcH]->m_iMana))
					iMagicType = 74; // Lightning-Strike
				break;

			case 10: // Frost, Nizie
				if ((m_pMagicConfigList[57]->m_sValue1 <= m_pNpcList[iNpcH]->m_iMana))
					iMagicType = 57; // Ice-Strike
				break;

			case 12: // Wyvern
				switch (iDice(1, 2)) {
				case 1: iMagicType = 91; break; // Blizzard
				case 2: iMagicType = 57; break; // Ice-Strike
				}
				break;

			case 13: // Abaddon
				switch (iDice(1, 3)) {
				case 1: iMagicType = 96; break; // Earth Shock Wave
				case 2: iMagicType = 91; break; // Blizzard
				case 3: iMagicType = 81; break; // Metoer Strike
				}
				break;
			case 14: // Ghost-Abaddon
				switch (iDice(1,8)) {
				case 1: iMagicType = 96; break; // Earth Shock Wave
				case 2: iMagicType = 93; break; // Mass Blizzard
				case 3: iMagicType = 81; break; // Metoer Strike
				case 4: iMagicType = 98; break; // Strike of the Ghosts
				case 5: iMagicType = 99; break; // Hell-Fire
				case 6: iMagicType = 92; break; // Fiery Shock Wave
				case 7: iMagicType = 97; break; // Fury of Thor
				case 8:
					{
						char cName[21], cNpcName[21], cNpcWaypoint[11];
						int iNamingValue, tX, tY;
						iNamingValue = m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->iGetEmptyNamingValue();
						if (iNamingValue != -1) {
							ZeroMemory(cNpcName, sizeof(cNpcName));
							switch (iDice(1, 7)) {
							case 1: strcpy(cNpcName, "Demon"); break;
							case 2: strcpy(cNpcName, "Gagoyle"); break;
							case 3: strcpy(cNpcName, "Beholder"); break;
							case 4: strcpy(cNpcName, "Giant-Lizard"); break;
							case 5: strcpy(cNpcName, "MasterMage-Orc"); break;
							case 6: strcpy(cNpcName, "Nizie"); break;
							case 7: strcpy(cNpcName, "Barlog"); break;
							}
							ZeroMemory(cName, sizeof(cName));
							wsprintf(cName, "XX%d", iNamingValue);
							cName[0] = '_';
							cName[1] = m_pNpcList[iNpcH]->m_cMapIndex + 65;
							ZeroMemory(cNpcWaypoint, sizeof(cNpcWaypoint));
							tX = (int)m_pNpcList[iNpcH]->m_sX;
							tY = (int)m_pNpcList[iNpcH]->m_sY;
							if (bCreateNewNpc(cNpcName, cName, m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_cName, 0, (rand() % 9),
								DEF_MOVETYPE_RANDOM, &tX, &tY, cNpcWaypoint, 0, 0, -1, false, false) == false) {
								m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->SetNamingValueEmpty(iNamingValue);
							}
						}
					}
					break;
				}
				break;

			case 11:
				break;
			}

			if (iMagicType != -1) {
				if (m_pNpcList[iNpcH]->m_iAILevel >= 2) {
					switch (m_pNpcList[iNpcH]->m_cTargetType) {
					case DEF_OWNERTYPE_PLAYER:
						// npc dont waste magic on AMP targets
						if ((m_pClientList[m_pNpcList[iNpcH]->m_iTargetIndex]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT] == 5) &&
							(iMagicType > 90)) {
							if ((abs(sX - dX) > m_pNpcList[iNpcH]->m_iAttackRange) || (abs(sY - dY) > m_pNpcList[iNpcH]->m_iAttackRange)) {
								m_pNpcList[iNpcH]->m_sBehaviorTurnCount = 0;
								m_pNpcList[iNpcH]->m_cBehavior = DEF_BEHAVIOR_MOVE;
								return;
							}
							else goto NBA_CHASE;
						}
						if ((iMagicType == 35) && (m_pClientList[m_pNpcList[iNpcH]->m_iTargetIndex]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] != 0)) goto NBA_CHASE;
						break;
					case DEF_OWNERTYPE_NPC:
						if (m_pNpcList[m_pNpcList[iNpcH]->m_iTargetIndex]->m_cMagicEffectStatus[DEF_MAGICTYPE_PROTECT] == 2) {
							if ((abs(sX - dX) > m_pNpcList[iNpcH]->m_iAttackRange) || (abs(sY - dY) > m_pNpcList[iNpcH]->m_iAttackRange)) {
								m_pNpcList[iNpcH]->m_sBehaviorTurnCount = 0;
								m_pNpcList[iNpcH]->m_cBehavior = DEF_BEHAVIOR_MOVE;
								return;
							}
							else goto NBA_CHASE;
						}
						if ((iMagicType == 35) && (m_pNpcList[m_pNpcList[iNpcH]->m_iTargetIndex]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] != 0)) goto NBA_CHASE;
						break;
					}
				}
				SendEventToNearClient_TypeA(iNpcH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTATTACK, m_pNpcList[iNpcH]->m_sX + _tmp_cTmpDirX[cDir], m_pNpcList[iNpcH]->m_sY + _tmp_cTmpDirY[cDir], 1); // 1 : Ä®µîÀÇ ±ÙÁ¢¹«±â·Î °ø°ÝÇÏ´Â ÀÇ¹Ì 
				NpcMagicHandler(iNpcH, dX, dY, iMagicType);
				m_pNpcList[iNpcH]->m_dwTime = dwTime + 2000;
				return;
			}
		}
		if ((m_pNpcList[iNpcH]->m_cMagicLevel < 0) && (iDice(1, 2) == 1) &&
			(abs(sX - dX) <= 11) && (abs(sY - dY) <= 9)) {
			iMagicType = -1;
			if (m_pMagicConfigList[43]->m_sValue1 <= m_pNpcList[iNpcH]->m_iMana)
				iMagicType = 43;
			else if (m_pMagicConfigList[37]->m_sValue1 <= m_pNpcList[iNpcH]->m_iMana)
				iMagicType = 37;
			else if (m_pMagicConfigList[0]->m_sValue1 <= m_pNpcList[iNpcH]->m_iMana)
				iMagicType = 0;
			if (iMagicType != -1) {
				SendEventToNearClient_TypeA(iNpcH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTATTACK, m_pNpcList[iNpcH]->m_sX + _tmp_cTmpDirX[cDir], m_pNpcList[iNpcH]->m_sY + _tmp_cTmpDirY[cDir], 1); // 1 : Ä®µîÀÇ ±ÙÁ¢¹«±â·Î °ø°ÝÇÏ´Â ÀÇ¹Ì 
				NpcMagicHandler(iNpcH, dX, dY, iMagicType);
				m_pNpcList[iNpcH]->m_dwTime = dwTime + 2000;
				return;
			}
		}
		
		if ((m_pNpcList[iNpcH]->m_iAttackRange > 1) &&
			(abs(sX - dX) <= m_pNpcList[iNpcH]->m_iAttackRange) &&
			(abs(sY - dY) <= m_pNpcList[iNpcH]->m_iAttackRange)) {
			cDir = m_Misc.cGetNextMoveDir(sX, sY, dX, dY);
			if (cDir == 0) return;
			m_pNpcList[iNpcH]->m_cDir = cDir;
			if (m_pNpcList[iNpcH]->m_cActionLimit == 5) {
				switch (m_pNpcList[iNpcH]->m_sType) {
				case 87: // CT
					SendEventToNearClient_TypeA(iNpcH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTATTACK, dX, dY, 2);
					iCalculateAttackEffect(m_pNpcList[iNpcH]->m_iTargetIndex, m_pNpcList[iNpcH]->m_cTargetType, iNpcH, DEF_OWNERTYPE_NPC, dX, dY, 2, false, false, false, false);
					break;
				case 36: // Crossbow Guard Tower: 
					SendEventToNearClient_TypeA(iNpcH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTATTACK, m_pNpcList[iNpcH]->m_sX + _tmp_cTmpDirX[cDir], m_pNpcList[iNpcH]->m_sY + _tmp_cTmpDirY[cDir], 2); // È°
					iCalculateAttackEffect(m_pNpcList[iNpcH]->m_iTargetIndex, m_pNpcList[iNpcH]->m_cTargetType, iNpcH, DEF_OWNERTYPE_NPC, dX, dY, 2, false, false, false, false);
					break;
				case 37: // Cannon Guard Tower: 
				case 89: // AGT
					SendEventToNearClient_TypeA(iNpcH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTATTACK, dX, dY, 1);
					m_pNpcList[iNpcH]->m_iMagicHitRatio = 1000;
					NpcMagicHandler(iNpcH, dX, dY, 61);
					break;
				}
			}
			else {
				switch (m_pNpcList[iNpcH]->m_sType) {
				case 51: // v2.05 Catapult: ¸Å½º ÆÄÀÌ¾î ½ºÆ®¶óÀÌÅ© °ø°Ý 
					SendEventToNearClient_TypeA(iNpcH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTATTACK, dX, dY, 1);
					m_pNpcList[iNpcH]->m_iMagicHitRatio = 1000;
					NpcMagicHandler(iNpcH, dX, dY, 61);
					break;

				case 54: // Dark Elf: È° °ø°ÝÀ» ÇÑ´Ù.
					SendEventToNearClient_TypeA(iNpcH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTATTACK, dX, dY, 2); // 2: È°°ø°Ý 
					iCalculateAttackEffect(m_pNpcList[iNpcH]->m_iTargetIndex, m_pNpcList[iNpcH]->m_cTargetType, iNpcH, DEF_OWNERTYPE_NPC, dX, dY, 2);
					break;

				case 63: // Frost
				case 79: // Nizie
					switch (m_pNpcList[iNpcH]->m_cTargetType) {
					case DEF_OWNERTYPE_PLAYER:
						if (m_pClientList[m_pNpcList[iNpcH]->m_iTargetIndex] == 0) goto NBA_BREAK1;
						if ((m_pMagicConfigList[57]->m_sValue1 <= m_pNpcList[iNpcH]->m_iMana) && (iDice(1, 3) == 2))
							NpcMagicHandler(iNpcH, dX, dY, 57);
						// Å¸°ÙÀÌ »ì¾ÆÀÖ°í ¾óÀ½ ÀúÇ×¿¡ ½ÇÆÐÇß´Ù¸é ¾ó¾îºÙ´Â´Ù.
						if ((m_pClientList[m_pNpcList[iNpcH]->m_iTargetIndex]->m_iHP > 0) &&
							(bCheckResistingIceSuccess(m_pNpcList[iNpcH]->m_cDir, m_pNpcList[iNpcH]->m_iTargetIndex, DEF_OWNERTYPE_PLAYER, m_pNpcList[iNpcH]->m_iMagicHitRatio) == false)) {
							if (m_pClientList[m_pNpcList[iNpcH]->m_iTargetIndex]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0) {
								m_pClientList[m_pNpcList[iNpcH]->m_iTargetIndex]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
								SetIceFlag(m_pNpcList[iNpcH]->m_iTargetIndex, DEF_OWNERTYPE_PLAYER, true);
								// È¿°ú°¡ ÇØÁ¦µÉ ¶§ ¹ß»ýÇÒ µô·¹ÀÌ ÀÌº¥Æ®¸¦ µî·ÏÇÑ´Ù.
								bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (5 * 1000),
									m_pNpcList[iNpcH]->m_iTargetIndex, DEF_OWNERTYPE_PLAYER, 0, 0, 0, 1, 0, 0);
								// ´ë»óÀÌ ÇÃ·¹ÀÌ¾îÀÎ °æ¿ì ¾Ë·ÁÁØ´Ù.
								SendNotifyMsg(0, m_pNpcList[iNpcH]->m_iTargetIndex, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_ICE, 1, 0, 0);
							}
						}
						break;

					case DEF_OWNERTYPE_NPC:
						goto NBA_BREAK1;
						break;
					}
					break;


				case 65: // IceGolem
				case 53: //Beholder: ³Ãµ¿ °ø°ÝÀ» ÇÑ´Ù.
					switch (m_pNpcList[iNpcH]->m_cTargetType) {
					case DEF_OWNERTYPE_PLAYER:
						if (m_pClientList[m_pNpcList[iNpcH]->m_iTargetIndex] == 0) goto NBA_BREAK1;
						// Å¸°ÙÀÌ »ì¾ÆÀÖ°í ¾óÀ½ ÀúÇ×¿¡ ½ÇÆÐÇß´Ù¸é ¾ó¾îºÙ´Â´Ù.
						if ((m_pClientList[m_pNpcList[iNpcH]->m_iTargetIndex]->m_iHP > 0) &&
							(bCheckResistingIceSuccess(m_pNpcList[iNpcH]->m_cDir, m_pNpcList[iNpcH]->m_iTargetIndex, DEF_OWNERTYPE_PLAYER, m_pNpcList[iNpcH]->m_iMagicHitRatio) == false)) {
							if (m_pClientList[m_pNpcList[iNpcH]->m_iTargetIndex]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 0) {
								m_pClientList[m_pNpcList[iNpcH]->m_iTargetIndex]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 1;
								SetIceFlag(m_pNpcList[iNpcH]->m_iTargetIndex, DEF_OWNERTYPE_PLAYER, true);
								// È¿°ú°¡ ÇØÁ¦µÉ ¶§ ¹ß»ýÇÒ µô·¹ÀÌ ÀÌº¥Æ®¸¦ µî·ÏÇÑ´Ù.
								bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (5 * 1000),
									m_pNpcList[iNpcH]->m_iTargetIndex, DEF_OWNERTYPE_PLAYER, 0, 0, 0, 1, 0, 0);
								// ´ë»óÀÌ ÇÃ·¹ÀÌ¾îÀÎ °æ¿ì ¾Ë·ÁÁØ´Ù.
								SendNotifyMsg(0, m_pNpcList[iNpcH]->m_iTargetIndex, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_ICE, 1, 0, 0);
							}
						}
						break;

					case DEF_OWNERTYPE_NPC:
						goto NBA_BREAK1;
						break;
					}
					break;
				NBA_BREAK1:;
					SendEventToNearClient_TypeA(iNpcH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTATTACK, dX, dY, 20);
					iCalculateAttackEffect(m_pNpcList[iNpcH]->m_iTargetIndex, m_pNpcList[iNpcH]->m_cTargetType, iNpcH, DEF_OWNERTYPE_NPC, dX, dY, 20);
					break;

				default:
					SendEventToNearClient_TypeA(iNpcH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTATTACK, dX, dY, 20);
					iCalculateAttackEffect(m_pNpcList[iNpcH]->m_iTargetIndex, m_pNpcList[iNpcH]->m_cTargetType, iNpcH, DEF_OWNERTYPE_NPC, dX, dY, 20);
					break;
				}
			}
			m_pNpcList[iNpcH]->m_iAttackCount++;
			if ((m_pNpcList[iNpcH]->m_bIsPermAttackMode == false) && (m_pNpcList[iNpcH]->m_cActionLimit == 0)) {
				switch (m_pNpcList[iNpcH]->m_iAttackStrategy) {
				case DEF_ATTACKAI_EXCHANGEATTACK:
					m_pNpcList[iNpcH]->m_sBehaviorTurnCount = 0;
					m_pNpcList[iNpcH]->m_cBehavior = DEF_BEHAVIOR_FLEE;
					break;

				case DEF_ATTACKAI_TWOBYONEATTACK:
					if (m_pNpcList[iNpcH]->m_iAttackCount >= 2) {
						m_pNpcList[iNpcH]->m_sBehaviorTurnCount = 0;
						m_pNpcList[iNpcH]->m_cBehavior = DEF_BEHAVIOR_FLEE;
					}
					break;
				}
			}
		}

NBA_CHASE:;
		if (m_pNpcList[iNpcH]->m_cActionLimit != 0) return;
		m_pNpcList[iNpcH]->m_iAttackCount = 0;
		if ((m_pNpcList[iNpcH]->m_iAttackRange > 1) && // centu - fixed attack range
			(abs(sX - dX) <= m_pNpcList[iNpcH]->m_iAttackRange) &&
			(abs(sY - dY) <= m_pNpcList[iNpcH]->m_iAttackRange)) {
			SendEventToNearClient_TypeA(iNpcH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTATTACK, dX, dY, 20);
			iCalculateAttackEffect(m_pNpcList[iNpcH]->m_iTargetIndex, m_pNpcList[iNpcH]->m_cTargetType, iNpcH, DEF_OWNERTYPE_NPC, dX, dY, 20);
		}
		else {
			// new
			if (m_pNpcList[iNpcH]->m_sAreaSize == 0) {
				cDir = cGetNextMoveDir(sX, sY, dX, dY, m_pNpcList[iNpcH]->m_cMapIndex, m_pNpcList[iNpcH]->m_cTurn, &m_pNpcList[iNpcH]->m_tmp_iError);
			}
			else {
				cDir = cGetNextMoveArea(iNpcH, sX, sY, dX, dY, m_pNpcList[iNpcH]->m_cMapIndex, m_pNpcList[iNpcH]->m_cTurn, &m_pNpcList[iNpcH]->m_tmp_iError, m_pNpcList[iNpcH]->m_sAreaSize);
			}
			if (cDir == 0) return;
			dX = m_pNpcList[iNpcH]->m_sX + _tmp_cTmpDirX[cDir];
			dY = m_pNpcList[iNpcH]->m_sY + _tmp_cTmpDirY[cDir];
			// new 1x1, 2x2, 3x3 npc attack
			if (m_pNpcList[iNpcH]->m_sAreaSize == 0) {
				m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->ClearOwner(9, iNpcH, DEF_OWNERTYPE_NPC, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY);
				m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->SetOwner(iNpcH, DEF_OWNERTYPE_NPC, dX, dY);
			}
			else {
				m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->ClearBigOwner(iNpcH, DEF_OWNERTYPE_NPC, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY, m_pNpcList[iNpcH]->m_sAreaSize);
				m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->SetBigOwner(iNpcH, DEF_OWNERTYPE_NPC, dX, dY, m_pNpcList[iNpcH]->m_sAreaSize);
			}
			m_pNpcList[iNpcH]->m_sX = dX;
			m_pNpcList[iNpcH]->m_sY = dY;
			m_pNpcList[iNpcH]->m_cDir = cDir;
			SendEventToNearClient_TypeA(iNpcH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTMOVE, 0, 0, 0);
		}
	}
}

//Magn0S::  Generating Abby Ghost slaves to fight.
void CGame::GenerateMonsterSlaves(int MapIndex, int iX, int iY, int iOrder)
{
	char cName[21], cNpcName[21], cNpcWaypoint[11];
	int iNamingValue, iQtd, j;
	bool bZerk;
	bZerk = true; // false;
	iQtd = 0;
	ZeroMemory(cNpcWaypoint, sizeof(cNpcWaypoint));
	iNamingValue = m_pMapList[MapIndex]->iGetEmptyNamingValue();
	if (iNamingValue != -1)
	{
		switch (iOrder) {
		case 1: strcpy(cNpcName, "Demon"); iQtd = 5; break;
		case 2: strcpy(cNpcName, "Gagoyle"); iQtd = 3; break;
		case 3: strcpy(cNpcName, "Hellclaw"); iQtd = 2; break;
		case 4: strcpy(cNpcName, "Tigerworm"); iQtd = 2; break;
		}

		for (j = 0; j < iQtd; j++) {
			ZeroMemory(cName, sizeof(cName));
			wsprintf(cName, "XX%d", iNamingValue);
			cName[0] = '_';
			cName[1] = MapIndex + 65;

			if (bCreateNewNpc(cNpcName, cName, m_pMapList[MapIndex]->m_cName, 0, (rand() % 3)
				, DEF_MOVETYPE_RANDOM, &iX, &iY, cNpcWaypoint, 0, 0, 10, false, false, bZerk, true) == false) {
				m_pMapList[MapIndex]->SetNamingValueEmpty(iNamingValue);
			}
		}
	}
}

bool CGame::bGetMultipleItemNamesWhenDeleteNpc(short sNpcType, int iProbability, int iMin, int iMax, short sBaseX, short sBaseY, int iItemSpreadType, int iSpreadRange, int* iItemIDs, POINT* BasePos, int* iNumItem)
{
	
		class    CNpcItem	CTempNpcItem;
		
		int    iNpcIndex;
		int    iNumNpcitem;
		int    iIndex;
		
		int		iItemID = 0;
		int		iNum = 0;
		bool	bFirstDice = false, bSecondDice = false;

		for (iNpcIndex = 0; iNpcIndex < DEF_MAXNPCTYPES; iNpcIndex++) {
			if (m_pNpcConfigList[iNpcIndex] != 0) {
				if (m_pNpcConfigList[iNpcIndex]->m_sType == sNpcType) break;
			}
		}

		if (iNpcIndex == DEF_MAXNPCTYPES) return false;

		if (m_pNpcConfigList[iNpcIndex]->m_vNpcItem.size() <= 0)  return false;

		switch (m_pNpcConfigList[iNpcIndex]->m_iNpcItemType) {
		case 1:
			
			break;

		case 2:
			iNumNpcitem = m_pNpcConfigList[iNpcIndex]->m_vNpcItem.size() - 1;
			
			for (iIndex = 0; iIndex < iNumNpcitem; iIndex++) {
				
				if (iNum == iMax) break;
				
				CTempNpcItem = m_pNpcConfigList[iNpcIndex]->m_vNpcItem.at(iIndex);

				// centu - fixed que lea probabilidades
				if (iDice(1, 10000) >= CTempNpcItem.m_sFirstProbability) bFirstDice = true;
				if (iDice(1, 10000) <= CTempNpcItem.m_sSecondProbability) bSecondDice = true;

				if (bFirstDice && bSecondDice) {

					iItemID = CTempNpcItem.m_sItemID;

					wsprintf(G_cTxt, "NpcType 2 (%d) size(%d) %s(%d) (%d)", sNpcType, m_pNpcConfigList[iNpcIndex]->m_vNpcItem.size(), CTempNpcItem.m_cName, CTempNpcItem.m_sItemID, iItemID);
					PutLogList(G_cTxt);

				}
				else {
					switch (iDice(1, 4)) {
					case 1: iItemID = 650; break; // Zem
					case 2: // Stones
						switch (iDice(1, 2)) {
							case 1: iItemID = 656; break; // Xelima
							case 2: iItemID = 657; break; // Merien
						}
						break;
					case 3: // Ancient piece

						break;
					case 4: // Gold

						break;
					}
				}
				
				// item id
				iItemIDs[iNum] = iItemID;

				// item position
				switch (iItemSpreadType)
				{
					case DEF_ITEMSPREAD_RANDOM:
						BasePos[iNum].x = sBaseX + iSpreadRange - iDice(1, iSpreadRange * 2);
						BasePos[iNum].y = sBaseY + iSpreadRange - iDice(1, iSpreadRange * 2);
						break;

					case DEF_ITEMSPREAD_FIXED:
						BasePos[iNum].x = sBaseX + ITEMSPREAD_FIEXD_COORD[iNum][0];
						BasePos[iNum].y = sBaseY + ITEMSPREAD_FIEXD_COORD[iNum][1];
						break;
				}
				
				iNum++;
				
			} // for
			break;

		} // switch

		*iNumItem = iNum;

		return true;
	

}

/*********************************************************************************************************************
**  int CGame::iComposeMoveMapData(short sX, short sY, int iClientH, char cDir, char * pData)						**
**  DESCRIPTION			:: updates client screen on movement														**
**  LAST_UPDATED		:: March 16, 2005; 1:11 PM; Hypnotoad														**
**	RETURN_VALUE		:: void																						**
**  NOTES				::	- added iGetPlayerStatus on PLAYER														**
**	MODIFICATION		::  - invis hack 																			**
**********************************************************************************************************************/
int CGame::iComposeMoveMapData(short sX, short sY, int iClientH, char cDir, char* pData)
{
	int* ip, ix, iy, iSize, iTileExists;
	class CTile * pTile;
	unsigned char ucHeader;
	short* sp, * pTotal;
	int iTemp, iTemp2;
	UINT16* wp;
	char* cp;
	UINT32* dwp;

	if (m_pClientList[iClientH] == 0) return 0;
	pTotal = (short*)pData;
	cp = (char*)(pData + 2);
	iSize = 2;
	iTileExists = 0;

	for (iy = 0; iy < 19; iy++)
	for (ix = 0; ix < 25; ix++) {
		switch (cDir) {
			
			case 1: if (!(iy <= 0)) continue; break;
			case 2: if (!(iy <= 0 || ix >= 25 - 1)) continue; break;
			case 3: if (!(ix >= 25 - 1)) continue; break;
			case 4: if (!(ix >= 25 - 1 || iy >= 19 - 1)) continue; break;
			case 5: if (!(iy >= 19 - 1)) continue; break;
			case 6: if (!(ix <= 0 || iy >= 19 - 1)) continue; break;
			case 7: if (!(ix <= 0)) continue; break;
			case 8: if (!(ix <= 0 || iy <= 0)) continue; break;
		}
		
		pTile = (class CTile*)(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_pTile + (sX + ix) + (sY + iy) * m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_sSizeY);

		if ((pTile->m_sOwner != 0) || (pTile->m_sDeadOwner != 0) || (pTile->m_pItem[0] != 0) || (pTile->m_sDynamicObjectType != 0)) {
			iTileExists++;
			sp = (short*)cp;
			*sp = ix;
			cp += 2;
			sp = (short*)cp;
			*sp = iy;
			cp += 2;
			iSize += 4;
			ucHeader = 0;

			if (pTile->m_sOwner != 0) {
				if (pTile->m_cOwnerClass == DEF_OWNERTYPE_PLAYER) {
					if (m_pClientList[pTile->m_sOwner] != 0) ucHeader = ucHeader | 0x01;
					else pTile->m_sOwner = 0;
				}
				if (pTile->m_cOwnerClass == DEF_OWNERTYPE_NPC) {
					if (m_pNpcList[pTile->m_sOwner] != 0) ucHeader = ucHeader | 0x01;
					else pTile->m_sOwner = 0;
				}
			}
			if (pTile->m_sDeadOwner != 0) {
				if (pTile->m_cDeadOwnerClass == DEF_OWNERTYPE_PLAYER) {
					if (m_pClientList[pTile->m_sDeadOwner] != 0)	ucHeader = ucHeader | 0x02;
					else pTile->m_sDeadOwner = 0;
				}
				if (pTile->m_cDeadOwnerClass == DEF_OWNERTYPE_NPC) {
					if (m_pNpcList[pTile->m_sDeadOwner] != 0) ucHeader = ucHeader | 0x02;
					else pTile->m_sDeadOwner = 0;
				}
			}

			if (pTile->m_pItem[0] != 0)				ucHeader = ucHeader | 0x04;
			if (pTile->m_sDynamicObjectType != 0)    ucHeader = ucHeader | 0x08;

			*cp = ucHeader;
			cp++;
			iSize++;
			if ((ucHeader & 0x01) != 0) {
				switch (pTile->m_cOwnerClass) {
				case DEF_OWNERTYPE_PLAYER:
					sp = (short*)cp;
					*sp = pTile->m_sOwner;
					cp += 2;
					iSize += 2;

					sp = (short*)cp;
					*sp = m_pClientList[pTile->m_sOwner]->m_sType;
					cp += 2;
					iSize += 2;

					*cp = m_pClientList[pTile->m_sOwner]->m_cDir;
					cp++;
					iSize++;

					sp = (short*)cp;
					*sp = m_pClientList[pTile->m_sOwner]->m_sAppr1;
					cp += 2;
					iSize += 2;

					sp = (short*)cp;
					*sp = m_pClientList[pTile->m_sOwner]->m_sAppr2;
					cp += 2;
					iSize += 2;

					sp = (short*)cp;
					*sp = m_pClientList[pTile->m_sOwner]->m_sAppr3;
					cp += 2;
					iSize += 2;

					sp = (short*)cp;
					*sp = m_pClientList[pTile->m_sOwner]->m_sAppr4;
					cp += 2;
					iSize += 2;

					ip = (int*)cp;
					*ip = m_pClientList[pTile->m_sOwner]->m_iApprColor;
					cp += 4;
					iSize += 4;

					ip = (int*)cp;

					iTemp = m_pClientList[pTile->m_sOwner]->m_iStatus;

					iTemp = 0x0FFFFFFF & iTemp;
					iTemp2 = iGetPlayerABSStatus(pTile->m_sOwner, iClientH);
					iTemp = (iTemp | (iTemp2 << 28));
					*ip = iTemp;
					cp += 4;
					iSize += 4;

					memcpy(cp, m_pClientList[pTile->m_sOwner]->m_cCharName, 10);
					cp += 10;
					iSize += 10;
					break;

				case DEF_OWNERTYPE_NPC:
					sp = (short*)cp;
					*sp = pTile->m_sOwner + 10000;
					cp += 2;
					iSize += 2;

					sp = (short*)cp;
					*sp = m_pNpcList[pTile->m_sOwner]->m_sType;
					cp += 2;
					iSize += 2;

					*cp = m_pNpcList[pTile->m_sOwner]->m_cDir;
					cp++;
					iSize++;

					sp = (short*)cp;
					*sp = m_pNpcList[pTile->m_sOwner]->m_sAppr2;
					cp += 2;
					iSize += 2;

					ip = (int*)cp;
					iTemp = m_pNpcList[pTile->m_sOwner]->m_iStatus;
					iTemp = 0x0FFFFFFF & iTemp;
					iTemp2 = iGetNpcRelationship(pTile->m_sOwner, iClientH);
					iTemp = (iTemp | (iTemp2 << 28));
					*ip = iTemp;
					cp += 4;
					iSize += 4;

					memcpy(cp, m_pNpcList[pTile->m_sOwner]->m_cName, 5);
					cp += 5;
					iSize += 5;
				}
			}

			if ((ucHeader & 0x02) != 0) {
				switch (pTile->m_cDeadOwnerClass) {
				case DEF_OWNERTYPE_PLAYER:

					sp = (short*)cp;
					*sp = pTile->m_sDeadOwner;
					cp += 2;
					iSize += 2;

					sp = (short*)cp;
					*sp = m_pClientList[pTile->m_sDeadOwner]->m_sType;
					cp += 2;
					iSize += 2;

					*cp = m_pClientList[pTile->m_sDeadOwner]->m_cDir;
					cp++;
					iSize++;

					sp = (short*)cp;
					*sp = m_pClientList[pTile->m_sDeadOwner]->m_sAppr1;
					cp += 2;
					iSize += 2;

					sp = (short*)cp;
					*sp = m_pClientList[pTile->m_sDeadOwner]->m_sAppr2;
					cp += 2;
					iSize += 2;

					sp = (short*)cp;
					*sp = m_pClientList[pTile->m_sDeadOwner]->m_sAppr3;
					cp += 2;
					iSize += 2;

					sp = (short*)cp;
					*sp = m_pClientList[pTile->m_sDeadOwner]->m_sAppr4;
					cp += 2;
					iSize += 2;

					ip = (int*)cp;
					*ip = m_pClientList[pTile->m_sDeadOwner]->m_iApprColor;
					cp += 4;
					iSize += 4;

					ip = (int*)cp;

					iTemp = m_pClientList[pTile->m_sDeadOwner]->m_iStatus;

					iTemp = 0x0FFFFFFF & iTemp;
					iTemp2 = iGetPlayerABSStatus(pTile->m_sDeadOwner, iClientH);
					iTemp = (iTemp | (iTemp2 << 28));
					*ip = iTemp;
					cp += 4;
					iSize += 4;

					memcpy(cp, m_pClientList[pTile->m_sDeadOwner]->m_cCharName, 10);
					cp += 10;
					iSize += 10;
					break;

				case DEF_OWNERTYPE_NPC:
					sp = (short*)cp;
					*sp = pTile->m_sDeadOwner + 10000;
					cp += 2;
					iSize += 2;

					sp = (short*)cp;
					*sp = m_pNpcList[pTile->m_sDeadOwner]->m_sType;
					cp += 2;
					iSize += 2;

					*cp = m_pNpcList[pTile->m_sDeadOwner]->m_cDir;
					cp++;
					iSize++;

					sp = (short*)cp;
					*sp = m_pNpcList[pTile->m_sDeadOwner]->m_sAppr2;
					cp += 2;
					iSize += 2;

					ip = (int*)cp;

					iTemp = m_pNpcList[pTile->m_sDeadOwner]->m_iStatus;
					iTemp = 0x0FFFFFFF & iTemp;
					iTemp2 = iGetNpcRelationship(pTile->m_sDeadOwner, iClientH);
					iTemp = (iTemp | (iTemp2 << 28));
					*ip = iTemp;

					cp += 4;
					iSize += 4;

					memcpy(cp, m_pNpcList[pTile->m_sDeadOwner]->m_cName, 5);
					cp += 5;
					iSize += 5;
					break;
				}
			}
			if (pTile->m_pItem[0] != 0) {
				sp = (short*)cp;
				*sp = pTile->m_pItem[0]->m_sIDnum; // kamal
				cp += 2;
				iSize += 2;
				
				*cp = pTile->m_pItem[0]->m_cItemColor;
				cp++;
				iSize++;

				dwp = (UINT32*)cp;
				*dwp = pTile->m_pItem[0]->m_dwAttribute;
				cp += 4;
				iSize += 4;
			}
			if (pTile->m_sDynamicObjectType != 0) {

				wp = (UINT16*)cp;
				*wp = pTile->m_wDynamicObjectID;
				cp += 2;
				iSize += 2;

				sp = (short*)cp;
				*sp = pTile->m_sDynamicObjectType;
				cp += 2;
				iSize += 2;
			}
		}
	}
	*pTotal = iTileExists;
	return iSize;
}

char CGame::cGetNextMoveArea(short sOwnerH, short sX, short sY, short dstX, short dstY, char cMapIndex, char cTurn, int* pError, char cArea)
{
	char  cDir, cTmpDir;
	int   aX, aY, dX, dY;
	int   i, iResX, iResY;

	if ((sX == dstX) && (sY == dstY)) return 0;

	dX = sX;
	dY = sY;
	if ((abs(dX - dstX) <= 1) && (abs(dY - dstY) <= 1)) {
		iResX = dstX;
		iResY = dstY;
	}
	else m_Misc.GetPoint(dX, dY, dstX, dstY, &iResX, &iResY, pError);
	cDir = m_Misc.cGetNextMoveDir(dX, dY, iResX, iResY);

	if (cTurn == 0) {
		for (i = cDir; i <= cDir + 7; i++) {
			cTmpDir = i;
			if (cTmpDir != 0) {
				if (cTmpDir > 8) cTmpDir -= 8;
				aX = _tmp_cTmpDirX[cTmpDir];
				aY = _tmp_cTmpDirY[cTmpDir];
				if (bGetEmptyArea(sOwnerH, cMapIndex, dX + aX, dY + aY, cArea) != false) {
					return cTmpDir;
				}
			}
		}
	}
	else if (cTurn == 1) {
		for (i = cDir; i >= cDir - 7; i--) {
			cTmpDir = i;
			if (cTmpDir < 1) cTmpDir += 8;
			aX = _tmp_cTmpDirX[cTmpDir];
			aY = _tmp_cTmpDirY[cTmpDir];
			if (bGetEmptyArea(sOwnerH, cMapIndex, dX + aX, dY + aY, cArea) != false) {
				return cTmpDir;
			}
		}
	}
	return 0;
}

/*********************************************************************************************************************
**  char CGame::cGetNextMoveDir(short sX, short sY, short dstX, short dstY, char cMapIndex, char cTurn,				**
**		int * pError)																								**
**  DESCRIPTION			:: gets next direction																		**
**  LAST_UPDATED		:: March 19, 2005; 1:34 PM; Hypnotoad														**
**	RETURN_VALUE		:: char																						**
**  NOTES				::	n/a																						**
**	MODIFICATION		::	n/a																						**
**********************************************************************************************************************/
char CGame::cGetNextMoveDir(short sX, short sY, short dstX, short dstY, char cMapIndex, char cTurn, int* pError)
{
	char  cDir, cTmpDir;
	int   aX, aY, dX, dY;
	int   i, iResX, iResY;

	if ((sX == dstX) && (sY == dstY)) return 0;
	dX = sX;
	dY = sY;
	if ((abs(dX - dstX) <= 1) && (abs(dY - dstY) <= 1)) {
		iResX = dstX;
		iResY = dstY;
	}
	else m_Misc.GetPoint(dX, dY, dstX, dstY, &iResX, &iResY, pError);
	cDir = m_Misc.cGetNextMoveDir(dX, dY, iResX, iResY);
	
	if (cTurn == 0) {
		for (i = cDir; i <= cDir + 7; i++) {
			cTmpDir = i;
			if (cTmpDir > 8) cTmpDir -= 8;
			aX = _tmp_cTmpDirX[cTmpDir];
			aY = _tmp_cTmpDirY[cTmpDir];
			if (m_pMapList[cMapIndex]->bGetMoveable(dX + aX, dY + aY) == true) return cTmpDir;
		}
	}
	else if (cTurn == 1) {
		for (i = cDir; i >= cDir - 7; i--) {
			cTmpDir = i;
			if (cTmpDir < 1) cTmpDir += 8;
			aX = _tmp_cTmpDirX[cTmpDir];
			aY = _tmp_cTmpDirY[cTmpDir];
			if (m_pMapList[cMapIndex]->bGetMoveable(dX + aX, dY + aY) == true) return cTmpDir;
		}
	}
	return 0;
}

void CGame::NpcBehavior_Move(int iNpcH)
{
	char  cDir;
	short sX, sY, dX, dY, absX, absY;
	short sTarget, sDistance;
	char  cTargetType;

	if (m_pNpcList[iNpcH] == 0) return;
	if (m_pNpcList[iNpcH]->m_bIsKilled == true) return;
	if ((m_pNpcList[iNpcH]->m_bIsSummoned == true) && (m_pNpcList[iNpcH]->m_iSummonControlMode == 1)) return;
	if (m_pNpcList[iNpcH]->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] != 0) return;
	// Taming
	if ((m_pNpcList[iNpcH]->m_dwTamingTime != 0) && (m_pNpcList[iNpcH]->m_iSummonControlMode == 1)) return;
	switch (m_pNpcList[iNpcH]->m_cActionLimit) {
	case 2:
	case 3:
	case 5:
	case 8: // Heldenian gates
		m_pNpcList[iNpcH]->m_cBehavior = DEF_BEHAVIOR_STOP;
		m_pNpcList[iNpcH]->m_sBehaviorTurnCount = 0;
		return;
	}
	int iStX, iStY;
	if (m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex] != 0) {
		iStX = m_pNpcList[iNpcH]->m_sX / 20;
		iStY = m_pNpcList[iNpcH]->m_sY / 20;
		m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_stTempSectorInfo[iStX][iStY].iMonsterActivity++;
	}
	m_pNpcList[iNpcH]->m_sBehaviorTurnCount++;
	if (m_pNpcList[iNpcH]->m_sBehaviorTurnCount > 5) {
		m_pNpcList[iNpcH]->m_sBehaviorTurnCount = 0;
		absX = abs(m_pNpcList[iNpcH]->m_vX - m_pNpcList[iNpcH]->m_sX);
		absY = abs(m_pNpcList[iNpcH]->m_vY - m_pNpcList[iNpcH]->m_sY);
		if ((absX <= 2) && (absY <= 2)) {
			CalcNextWayPointDestination(iNpcH);
		}
		m_pNpcList[iNpcH]->m_vX = m_pNpcList[iNpcH]->m_sX;
		m_pNpcList[iNpcH]->m_vY = m_pNpcList[iNpcH]->m_sY;
	}
	TargetSearch(iNpcH, &sTarget, &cTargetType);
	minimap_update_apoc(iNpcH);
	if (sTarget != 0) {
		if (m_pNpcList[iNpcH]->m_dwActionTime < 1000) {
			if (iDice(1, 3) == 3) {
				m_pNpcList[iNpcH]->m_cBehavior = DEF_BEHAVIOR_ATTACK;
				m_pNpcList[iNpcH]->m_sBehaviorTurnCount = 0;
				m_pNpcList[iNpcH]->m_iTargetIndex = sTarget;
				m_pNpcList[iNpcH]->m_cTargetType = cTargetType;
				return;
			}
		}
		else {
			m_pNpcList[iNpcH]->m_cBehavior = DEF_BEHAVIOR_ATTACK;
			m_pNpcList[iNpcH]->m_sBehaviorTurnCount = 0;
			m_pNpcList[iNpcH]->m_iTargetIndex = sTarget;
			m_pNpcList[iNpcH]->m_cTargetType = cTargetType;
			return;
		}
	}
	if ((m_pNpcList[iNpcH]->m_bIsMaster == true) && (iDice(1, 3) == 2)) return;
	if (m_pNpcList[iNpcH]->m_cMoveType == DEF_MOVETYPE_FOLLOW) {
		sX = m_pNpcList[iNpcH]->m_sX;
		sY = m_pNpcList[iNpcH]->m_sY;
		switch (m_pNpcList[iNpcH]->m_cFollowOwnerType) {
		case DEF_OWNERTYPE_PLAYER_FREE: // Waiting for someone to control him
		case DEF_OWNERTYPE_PLAYER_WAITING: // Waiting for a character de reconnect, should be useless, here for safety
			m_pNpcList[iNpcH]->m_iFollowOwnerIndex = 0;
			m_pNpcList[iNpcH]->m_cMoveType = DEF_MOVETYPE_RANDOM;
			return;
		case DEF_OWNERTYPE_PLAYER:
			if (m_pClientList[m_pNpcList[iNpcH]->m_iFollowOwnerIndex] == 0) // Should not happen, because removeFromTarget should be called before
			{
				m_pNpcList[iNpcH]->m_cFollowOwnerType = DEF_OWNERTYPE_PLAYER_WAITING;
				m_pNpcList[iNpcH]->m_iFollowOwnerIndex = 0;
				m_pNpcList[iNpcH]->m_dwSummonWaitTime = timeGetTime();
				m_pNpcList[iNpcH]->m_cMoveType = DEF_MOVETYPE_RANDOM;
				return;
			}
			dX = m_pClientList[m_pNpcList[iNpcH]->m_iFollowOwnerIndex]->m_sX;
			dY = m_pClientList[m_pNpcList[iNpcH]->m_iFollowOwnerIndex]->m_sY;
			break;

		case DEF_OWNERTYPE_NPC:
			if (m_pNpcList[m_pNpcList[iNpcH]->m_iFollowOwnerIndex] == 0) {
				m_pNpcList[iNpcH]->m_cMoveType = DEF_MOVETYPE_RANDOM;
				m_pNpcList[iNpcH]->m_iFollowOwnerIndex = 0;
				return;
			}
			dX = m_pNpcList[m_pNpcList[iNpcH]->m_iFollowOwnerIndex]->m_sX;
			dY = m_pNpcList[m_pNpcList[iNpcH]->m_iFollowOwnerIndex]->m_sY;
			break;
		}

		if (abs(sX - dX) >= abs(sY - dY))
			sDistance = abs(sX - dX);
		else sDistance = abs(sY - dY);

		if (sDistance >= 3) {
			// new 1x1, 2x2, 3x3 monster move 
			if (m_pNpcList[iNpcH]->m_sAreaSize == 0) {
				cDir = cGetNextMoveDir(sX, sY, dX, dY, m_pNpcList[iNpcH]->m_cMapIndex, m_pNpcList[iNpcH]->m_cTurn, &m_pNpcList[iNpcH]->m_tmp_iError);
			}
			else {
				cDir = cGetNextMoveArea(iNpcH, sX, sY, dX, dY, m_pNpcList[iNpcH]->m_cMapIndex, m_pNpcList[iNpcH]->m_cTurn, &m_pNpcList[iNpcH]->m_tmp_iError, m_pNpcList[iNpcH]->m_sAreaSize);
			}
			if (cDir != 0) {

				dX = m_pNpcList[iNpcH]->m_sX + _tmp_cTmpDirX[cDir];
				dY = m_pNpcList[iNpcH]->m_sY + _tmp_cTmpDirY[cDir];
				// new 1x1, 2x2, 3x3 npc move
				if (m_pNpcList[iNpcH]->m_sAreaSize == 0) {
					m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->ClearOwner(3, iNpcH, DEF_OWNERTYPE_NPC, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY);
					m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->SetOwner(iNpcH, DEF_OWNERTYPE_NPC, dX, dY);
				}
				else {
					m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->ClearBigOwner(iNpcH, DEF_OWNERTYPE_NPC, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY, m_pNpcList[iNpcH]->m_sAreaSize);
					m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->SetBigOwner(iNpcH, DEF_OWNERTYPE_NPC, dX, dY, m_pNpcList[iNpcH]->m_sAreaSize);
				}
				m_pNpcList[iNpcH]->m_sX = dX;
				m_pNpcList[iNpcH]->m_sY = dY;
				m_pNpcList[iNpcH]->m_cDir = cDir;
				SendEventToNearClient_TypeA(iNpcH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTMOVE, 0, 0, 0);
			}
		}
	}
	else {
		// new 1x1, 2x2, 3x3 monster move 
		if (m_pNpcList[iNpcH]->m_sAreaSize == 0) {
			cDir = cGetNextMoveDir(m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY, m_pNpcList[iNpcH]->m_dX, m_pNpcList[iNpcH]->m_dY, m_pNpcList[iNpcH]->m_cMapIndex, m_pNpcList[iNpcH]->m_cTurn, &m_pNpcList[iNpcH]->m_tmp_iError);
		}
		else {
			cDir = cGetNextMoveArea(iNpcH, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY, m_pNpcList[iNpcH]->m_dX, m_pNpcList[iNpcH]->m_dY, m_pNpcList[iNpcH]->m_cMapIndex, m_pNpcList[iNpcH]->m_cTurn, &m_pNpcList[iNpcH]->m_tmp_iError, m_pNpcList[iNpcH]->m_sAreaSize);
		}
		if (cDir == 0) {
			if (iDice(1, 10) == 3) CalcNextWayPointDestination(iNpcH);
		}
		else {
			dX = m_pNpcList[iNpcH]->m_sX + _tmp_cTmpDirX[cDir];
			dY = m_pNpcList[iNpcH]->m_sY + _tmp_cTmpDirY[cDir];
			// new 1x1, 2x2, 3x3 npc move
			if (m_pNpcList[iNpcH]->m_sAreaSize == 0) {
				m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->ClearOwner(4, iNpcH, DEF_OWNERTYPE_NPC, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY);
				m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->SetOwner(iNpcH, DEF_OWNERTYPE_NPC, dX, dY);
			}
			else {
				m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->ClearBigOwner(iNpcH, DEF_OWNERTYPE_NPC, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY, m_pNpcList[iNpcH]->m_sAreaSize);
				m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->SetBigOwner(iNpcH, DEF_OWNERTYPE_NPC, dX, dY, m_pNpcList[iNpcH]->m_sAreaSize);
			}
			m_pNpcList[iNpcH]->m_sX = dX;
			m_pNpcList[iNpcH]->m_sY = dY;
			m_pNpcList[iNpcH]->m_cDir = cDir;
			SendEventToNearClient_TypeA(iNpcH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTMOVE, 0, 0, 0);
		}
	}
}

void CGame::NpcBehavior_Flee(int iNpcH)
{
	char cDir;
	short sX, sY, dX, dY;
	short sTarget;
	char  cTargetType;

	if (m_pNpcList[iNpcH] == 0) return;
	if (m_pNpcList[iNpcH]->m_bIsKilled == true) return;

	// ÅÏ Ä«¿îÆ® Áõ°¡. 
	m_pNpcList[iNpcH]->m_sBehaviorTurnCount++;

	// °ø°Ý ÀÎ°øÁö´É¿¡ ´ëÇÑ È¿°ú°è»ê.
	switch (m_pNpcList[iNpcH]->m_iAttackStrategy) {
	case DEF_ATTACKAI_EXCHANGEATTACK: // °ø°Ý -> ÈÄÅð -> ... 
	case DEF_ATTACKAI_TWOBYONEATTACK: // °ø°Ý -> °ø°Ý -> ÈÄÅð -> ...
		if (m_pNpcList[iNpcH]->m_sBehaviorTurnCount >= 2) {
			// ´Ù½Ã °ø°Ý ¸ðµå·Î ÀüÈ¯.
			m_pNpcList[iNpcH]->m_cBehavior = DEF_BEHAVIOR_ATTACK;
			m_pNpcList[iNpcH]->m_sBehaviorTurnCount = 0;
			return;
		}
		break;

	default:
		if (iDice(1, 2) == 1) NpcRequestAssistance(iNpcH);
		break;
	}

	if (m_pNpcList[iNpcH]->m_sBehaviorTurnCount > 10) {
		// 10¹øÀÇ ÅÏ¸¸ µµ¸ÁÇÑ ÈÄ ´Ù½Ã ÀÌµ¿¸ðµå·Î º¯È¯.
		m_pNpcList[iNpcH]->m_sBehaviorTurnCount = 0;
		m_pNpcList[iNpcH]->m_cBehavior = DEF_BEHAVIOR_MOVE;
		m_pNpcList[iNpcH]->m_tmp_iError = 0;
		if (m_pNpcList[iNpcH]->m_iHP <= 3) {
			m_pNpcList[iNpcH]->m_iHP += iDice(1, m_pNpcList[iNpcH]->m_iHitDice); // ! ¿¡³ÊÁö ºÎÁ·À¸·Î µµ¸ÁÃÆÀ»¶§ ¼º°øÀûÀ¸·Î µµ¸ÁÃÆÀ¸¹Ç·Î ¿¡³ÊÁö Áõ°¡.
			if (m_pNpcList[iNpcH]->m_iHP <= 0) m_pNpcList[iNpcH]->m_iHP = 1;
		}
		return;
	}

	// SNOOPY: Buggy here if TargetSearchfunction returns a faultly target
	// server will crash ! (added "else return")
	TargetSearch(iNpcH, &sTarget, &cTargetType);
	if (sTarget != 0) {
		m_pNpcList[iNpcH]->m_iTargetIndex = sTarget;
		m_pNpcList[iNpcH]->m_cTargetType = cTargetType;
	}
	else return;

	// ¸ñÇ¥¹°°ú ÀÚ½ÅÀÇ À§Ä¡¸¦ ±¸ÇÑ´Ù. ¹Ý´ë¹æÇâÀ¸·Î µµ¸ÁÄ¡±â À§ÇÔ.
	sX = m_pNpcList[iNpcH]->m_sX;
	sY = m_pNpcList[iNpcH]->m_sY;
	switch (m_pNpcList[iNpcH]->m_cTargetType) {
	case DEF_OWNERTYPE_PLAYER:
		dX = m_pClientList[m_pNpcList[iNpcH]->m_iTargetIndex]->m_sX;
		dY = m_pClientList[m_pNpcList[iNpcH]->m_iTargetIndex]->m_sY;
		break;
	case DEF_OWNERTYPE_NPC:
		dX = m_pNpcList[m_pNpcList[iNpcH]->m_iTargetIndex]->m_sX;
		dY = m_pNpcList[m_pNpcList[iNpcH]->m_iTargetIndex]->m_sY;
		break;
	}
	dX = sX - (dX - sX);
	dY = sY - (dY - sY);

	cDir = cGetNextMoveDir(sX, sY, dX, dY, m_pNpcList[iNpcH]->m_cMapIndex, m_pNpcList[iNpcH]->m_cTurn, &m_pNpcList[iNpcH]->m_tmp_iError);
	if (cDir != 0) {
		// ÀÌµ¿ÇÒ ¼ö ¾ø´Ù. 

		dX = m_pNpcList[iNpcH]->m_sX + _tmp_cTmpDirX[cDir];
		dY = m_pNpcList[iNpcH]->m_sY + _tmp_cTmpDirY[cDir];
		// ¿¹Àü À§Ä¡¿¡¼­ Áö¿î´Ù. 
		// »õ À§Ä¡¿¡ Ç¥½ÃÇÑ´Ù. 
		// new 1x1, 2x2, 3x3 npc flee
		if (m_pNpcList[iNpcH]->m_sAreaSize == 0) {
			m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->ClearOwner(11, iNpcH, DEF_OWNERTYPE_NPC, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY);
			m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->SetOwner(iNpcH, DEF_OWNERTYPE_NPC, dX, dY);
		}
		else {
			m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->ClearBigOwner(iNpcH, DEF_OWNERTYPE_NPC, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY, m_pNpcList[iNpcH]->m_sAreaSize);
			m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->SetBigOwner(iNpcH, DEF_OWNERTYPE_NPC, dX, dY, m_pNpcList[iNpcH]->m_sAreaSize);
		}
		m_pNpcList[iNpcH]->m_sX = dX;
		m_pNpcList[iNpcH]->m_sY = dY;
		m_pNpcList[iNpcH]->m_cDir = cDir;
		SendEventToNearClient_TypeA(iNpcH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTMOVE, 0, 0, 0);
	}
}

bool CGame::bGetNpcMovementArea(short sOwnerH, short pX, short pY, char cMapIndex, char cArea)
{
	short sAreaSquared, sPositionY, sPositionX, sX, sY;
	int i;

	sAreaSquared = cArea * cArea;
	if (m_pNpcList[sOwnerH]->m_sType == 91) return true;
	for (i = 1; i < sAreaSquared; i++) {
		if (_tmp_cEmptyPosX[i] > 0) {
			sPositionX = 2;
		}
		else if (_tmp_cEmptyPosX[i] < 0) {
			sPositionX = -2;
		}
		else {
			sPositionX = 0;
		}

		if (_tmp_cEmptyPosY[i] > 0) {
			sPositionY = 2;
		}
		else if (_tmp_cEmptyPosY[i] < 0) {
			sPositionY = -2;
		}
		else {
			sPositionY = 0;
		}
		if (bGetEmptyArea(sOwnerH, cMapIndex, pX + sPositionX, pY + sPositionY, cArea) != false) {
			sX = pX + sPositionX;
			sY = pY + sPositionY;
			pX = sX;
			pY = sY;
			return true;
		}
	}
	return false;
}

void CGame::bCalculateEnduranceDecrement(short sTargetH, short sAttackerH, char cTargetType, int iArmorType)
{
	int iDownValue, iHammerChance, iItemIndex;

	iDownValue = 1;
	if (m_pClientList[sTargetH] == 0) return;

	if (cTargetType == DEF_OWNERTYPE_PLAYER)
	{
		if (m_pClientList[sAttackerH] == 0) return;
		switch (m_pClientList[sAttackerH]->m_sUsingWeaponSkill) {
		case 14:
			if ((((m_pClientList[sAttackerH]->m_sAppr2 & 0x0FF0) >> 4) == 31) ||
				(((m_pClientList[sAttackerH]->m_sAppr2 & 0x0FF0) >> 4) == 32) ||
				(((m_pClientList[sAttackerH]->m_sAppr2 & 0x0FF0) >> 4) == 59)) {
				iItemIndex = m_pClientList[sAttackerH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND];
				if ((iItemIndex != -1) && (m_pClientList[sAttackerH]->m_pItemList[iItemIndex] != 0))
				{	//Nixu : aca se maneja la cantidad de endu que saca cada hammer
					if (m_pClientList[sAttackerH]->m_pItemList[iItemIndex]->m_sIDnum == 760) { // Hammer 
						iDownValue = 50;
					}
					else if (m_pClientList[sAttackerH]->m_pItemList[iItemIndex]->m_sIDnum == 761) { // BattleHammer 
						iDownValue = 100;
					}
					else if (m_pClientList[sAttackerH]->m_pItemList[iItemIndex]->m_sIDnum == 762) { // GiantBattleHammer
						iDownValue = 250;
					}
					else if (m_pClientList[sAttackerH]->m_pItemList[iItemIndex]->m_sIDnum == 843) { // BarbarianHammer
						iDownValue = 150;
					}
					else if (m_pClientList[sAttackerH]->m_pItemList[iItemIndex]->m_sIDnum == 854) { // KlonessHammer
						iDownValue = 350;
					}
					else if (m_pClientList[sAttackerH]->m_pItemList[iItemIndex]->m_sIDnum == 1038) { // StripBarbarian
						iDownValue = 500;
					}
				}
			}
			break;
		default:
			return;
		}
	}
	if (m_pClientList[sTargetH]->m_bIsSpecialAbilityEnabled == true) {
		switch (m_pClientList[sTargetH]->m_iSpecialAbilityType) {
		case 52:
			iDownValue = 0;
			iHammerChance = 0;
			break;
		}
	}
	if ((m_pClientList[sTargetH]->m_cSide != 0) && (m_pClientList[sTargetH]->m_pItemList[iArmorType]->m_wCurLifeSpan > 0)) {
		m_pClientList[sTargetH]->m_pItemList[iArmorType]->m_wCurLifeSpan -= iDownValue;
		SendNotifyMsg(0, sTargetH, DEF_NOTIFY_CURLIFESPAN, iArmorType, m_pClientList[sTargetH]->m_pItemList[iArmorType]->m_wCurLifeSpan, 0, 0);
		if (m_pClientList[sTargetH]->m_pItemList[iArmorType]->m_wCurLifeSpan <= 0) {
			m_pClientList[sTargetH]->m_pItemList[iArmorType]->m_wCurLifeSpan = 0;
			SendNotifyMsg(0, sTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[sTargetH]->m_pItemList[iArmorType]->m_cEquipPos, iArmorType, 0, 0);
			ReleaseItemHandler(sTargetH, iArmorType, true);
			return;
		}
		if (m_pClientList[sTargetH]->m_pItemList[iArmorType]->m_wCurLifeSpan <= 2000) {
			ReleaseItemHandler(sTargetH, iArmorType, true);
			SendNotifyMsg(0, sTargetH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[sTargetH]->m_pItemList[iArmorType]->m_cEquipPos, iArmorType, 0, 0);
		}
	}
	
}

// Wanted System
void CGame::SetWantedFlag(short sOwnerH, char cOwnerType, bool iStatus)
{
	if (cOwnerType != DEF_OWNERTYPE_PLAYER) return;
	if (m_pClientList[sOwnerH] == 0) return;

	switch (iStatus) {
	case 1: // Set
		m_pClientList[sOwnerH]->m_iStatus = m_pClientList[sOwnerH]->m_iStatus | 0x00040000;
		break;
	case 0: // Remove
		m_pClientList[sOwnerH]->m_iStatus = m_pClientList[sOwnerH]->m_iStatus & 0xFFFBFFFF;
		break;
	}

	SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
}

bool CGame::bReadTopPlayersFile(char* cFn) // MORLA 2.4 - Tops
{
	FILE* pFile;
	HANDLE hFile;
	UINT32  dwFileSize;
	char* cp, * token, cReadMode;
	char seps[] = "= \t\n";
	class CStrTok* pStrTok;

	cReadMode = 0;
	hFile = CreateFile(cFn, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
	dwFileSize = GetFileSize(hFile, 0);
	if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);
	pFile = fopen(cFn, "rt");
	if (pFile == 0)
	{
		PutLogList("(!) Cannot open Top Players file.");
		return false;
	}
	else
	{
		PutLogList("(!) Reading Top Players file...");
		cp = new char[dwFileSize + 2];
		ZeroMemory(cp, dwFileSize + 2);
		fread(cp, dwFileSize, 1, pFile);
		pStrTok = new class CStrTok(cp, seps);
		token = pStrTok->pGet();
		while (token != 0)
		{
			if (cReadMode != 0)
			{
				switch (cReadMode)
				{

				// #1
				case 1:
					ZeroMemory(aHBTopClientH[1], sizeof(aHBTopClientH[1]));
					memcpy(aHBTopClientH[1], token, 10);
					cReadMode = 0;
					break;
				case 2:
					aHBTopKills[1] = atoi(token);
					cReadMode = 0;
					break;

				// #2
				case 3:
					ZeroMemory(aHBTopClientH[2], sizeof(aHBTopClientH[2]));
					memcpy(aHBTopClientH[2], token, 10);
					cReadMode = 0;
					break;
				case 4:
					aHBTopKills[2] = atoi(token);
					cReadMode = 0;
					break;

				// #3
				case 5:
					ZeroMemory(aHBTopClientH[3], sizeof(aHBTopClientH[3]));
					memcpy(aHBTopClientH[3], token, 10);
					cReadMode = 0;
					break;
				case 6:
					aHBTopKills[3] = atoi(token);
					cReadMode = 0;
					break;

				// #4
				case 7:
					ZeroMemory(aHBTopClientH[4], sizeof(aHBTopClientH[4]));
					memcpy(aHBTopClientH[4], token, 10);
					cReadMode = 0;
					break;
				case 8:
					aHBTopKills[4] = atoi(token);
					cReadMode = 0;
					break;
				
				// #5
				case 9:
					ZeroMemory(aHBTopClientH[5], sizeof(aHBTopClientH[5]));
					memcpy(aHBTopClientH[5], token, 10);
					cReadMode = 0;
					break;
				case 10:
					aHBTopKills[5] = atoi(token);
					cReadMode = 0;
					break;

				// #6
				case 11:
					ZeroMemory(aHBTopClientH[6], sizeof(aHBTopClientH[6]));
					memcpy(aHBTopClientH[6], token, 10);
					cReadMode = 0;
					break;
				case 12:
					aHBTopKills[6] = atoi(token);
					cReadMode = 0;
					break;

				// #7
				case 13:
					ZeroMemory(aHBTopClientH[7], sizeof(aHBTopClientH[7]));
					memcpy(aHBTopClientH[7], token, 10);
					cReadMode = 0;
					break;
				case 14:
					aHBTopKills[7] = atoi(token);
					cReadMode = 0;
					break;

				// #8
				case 15:
					ZeroMemory(aHBTopClientH[8], sizeof(aHBTopClientH[8]));
					memcpy(aHBTopClientH[8], token, 10);
					cReadMode = 0;
					break;
				case 16:
					aHBTopKills[8] = atoi(token);
					cReadMode = 0;
					break;

				// #9
				case 17:
					ZeroMemory(aHBTopClientH[9], sizeof(aHBTopClientH[9]));
					memcpy(aHBTopClientH[9], token, 10);
					cReadMode = 0;
					break;
				case 18:
					aHBTopKills[9] = atoi(token);
					cReadMode = 0;
					break;

				// #10
				case 19:
					ZeroMemory(aHBTopClientH[10], sizeof(aHBTopClientH[10]));
					memcpy(aHBTopClientH[10], token, 10);
					cReadMode = 0;
					break;
				case 20:
					aHBTopKills[10] = atoi(token);
					cReadMode = 0;
					break;

				// #11
				case 21:
					ZeroMemory(aHBTopClientH[11], sizeof(aHBTopClientH[11]));
					memcpy(aHBTopClientH[11], token, 10);
					cReadMode = 0;
					break;
				case 22:
					aHBTopKills[11] = atoi(token);
					cReadMode = 0;
					break;

				// #12
				case 23:
					ZeroMemory(aHBTopClientH[12], sizeof(aHBTopClientH[12]));
					memcpy(aHBTopClientH[12], token, 10);
					cReadMode = 0;
					break;
				case 24:
					aHBTopKills[12] = atoi(token);
					cReadMode = 0;
					break;

				// #13
				case 25:
					ZeroMemory(aHBTopClientH[13], sizeof(aHBTopClientH[13]));
					memcpy(aHBTopClientH[13], token, 10);
					cReadMode = 0;
					break;
				case 26:
					aHBTopKills[13] = atoi(token);
					cReadMode = 0;
					break;

				// #14
				case 27:
					ZeroMemory(aHBTopClientH[14], sizeof(aHBTopClientH[14]));
					memcpy(aHBTopClientH[14], token, 10);
					cReadMode = 0;
					break;
				case 28:
					aHBTopKills[14] = atoi(token);
					cReadMode = 0;
					break;

				// #15
				case 29:
					ZeroMemory(aHBTopClientH[15], sizeof(aHBTopClientH[15]));
					memcpy(aHBTopClientH[15], token, 10);
					cReadMode = 0;
					break;
				case 30:
					aHBTopKills[15] = atoi(token);
					cReadMode = 0;
					break;
				}
			}
			else
			{
				// MORLA 2.10 - Top HB
				if (memcmp(token, "HBTop-1n", 8) == 0) cReadMode = 1;
				if (memcmp(token, "HBTop-1k", 8) == 0) cReadMode = 2;
				if (memcmp(token, "HBTop-2n", 8) == 0) cReadMode = 3;
				if (memcmp(token, "HBTop-2k", 8) == 0) cReadMode = 4;
				if (memcmp(token, "HBTop-3n", 8) == 0) cReadMode = 5;
				if (memcmp(token, "HBTop-3k", 8) == 0) cReadMode = 6;
				if (memcmp(token, "HBTop-4n", 8) == 0) cReadMode = 7;
				if (memcmp(token, "HBTop-4k", 8) == 0) cReadMode = 8;
				if (memcmp(token, "HBTop-5n", 8) == 0) cReadMode = 9;
				if (memcmp(token, "HBTop-5k", 8) == 0) cReadMode = 10;
				if (memcmp(token, "HBTop-6n", 8) == 0) cReadMode = 11;
				if (memcmp(token, "HBTop-6k", 8) == 0) cReadMode = 12;
				if (memcmp(token, "HBTop-7n", 8) == 0) cReadMode = 13;
				if (memcmp(token, "HBTop-7k", 8) == 0) cReadMode = 14;
				if (memcmp(token, "HBTop-8n", 8) == 0) cReadMode = 15;
				if (memcmp(token, "HBTop-8k", 8) == 0) cReadMode = 16;
				if (memcmp(token, "HBTop-9n", 8) == 0) cReadMode = 17;
				if (memcmp(token, "HBTop-9k", 8) == 0) cReadMode = 18;
				if (memcmp(token, "HBTop-10n", 9) == 0) cReadMode = 19;
				if (memcmp(token, "HBTop-10k", 9) == 0) cReadMode = 20;
				if (memcmp(token, "HBTop-11n", 9) == 0) cReadMode = 21;
				if (memcmp(token, "HBTop-11k", 9) == 0) cReadMode = 22;
				if (memcmp(token, "HBTop-12n", 9) == 0) cReadMode = 23;
				if (memcmp(token, "HBTop-12k", 9) == 0) cReadMode = 24;
				if (memcmp(token, "HBTop-13n", 9) == 0) cReadMode = 25;
				if (memcmp(token, "HBTop-13k", 9) == 0) cReadMode = 26;
				if (memcmp(token, "HBTop-14n", 9) == 0) cReadMode = 27;
				if (memcmp(token, "HBTop-14k", 9) == 0) cReadMode = 28;
				if (memcmp(token, "HBTop-15n", 9) == 0) cReadMode = 29;
				if (memcmp(token, "HBTop-15k", 9) == 0) cReadMode = 30;
			}
			token = pStrTok->pGet();
		}
		delete pStrTok;
		delete[] cp;
		fclose(pFile);
	}
	PutLogList("(!) Top Players file loaded.");
	return true;
}

void CGame::_CreateTopPlayers() // MORLA 2.4 - Guarda el archivo TopPlayers
{
	char* cp, cTxt[512], cFn[256], cTemp[512];
	FILE* pFile;

	ZeroMemory(cFn, sizeof(cFn));
	strcat(cFn, "GameConfigs\\TopPlayers.cfg");
	pFile = fopen(cFn, "wt");
	if (pFile == 0)
	{
		wsprintf(G_cTxt, "(!) Cannot create Top Players file.");
		PutLogList(G_cTxt);
	}
	else
	{
		ZeroMemory(cTemp, sizeof(cTemp));
		ZeroMemory(cTxt, sizeof(cTxt));
		wsprintf(cTxt, "HBTop-1n = %s\nHBTop-1k = %d\nHBTop-2n = %s\nHBTop-2k = %d\nHBTop-3n = %s\nHBTop-3k = %d\nHBTop-4n = %s\nHBTop-4k = %d\nHBTop-5n = %s\nHBTop-5k = %d\nHBTop-6n = %s\nHBTop-6k = %d\nHBTop-7n = %s\nHBTop-7k = %d\nHBTop-8n = %s\nHBTop-8k = %d\nHBTop-9n = %s\nHBTop-9k = %d\nHBTop-10n = %s\nHBTop-10k = %d\nHBTop-11n = %s\nHBTop-11k = %d\nHBTop-12n = %s\nHBTop-12k = %d\nHBTop-13n = %s\nHBTop-13k = %d\nHBTop-14n = %s\nHBTop-14k = %d\nHBTop-15n = %s\nHBTop-15k = %d\n",
			aHBTopClientH[1], aHBTopKills[1], 
			aHBTopClientH[2], aHBTopKills[2], 
			aHBTopClientH[3], aHBTopKills[3],
			aHBTopClientH[4], aHBTopKills[4],
			aHBTopClientH[5], aHBTopKills[5],
			aHBTopClientH[6], aHBTopKills[6],
			aHBTopClientH[7], aHBTopKills[7],
			aHBTopClientH[8], aHBTopKills[8],
			aHBTopClientH[9], aHBTopKills[9],
			aHBTopClientH[10], aHBTopKills[10],
			aHBTopClientH[11], aHBTopKills[11],
			aHBTopClientH[12], aHBTopKills[12],
			aHBTopClientH[13], aHBTopKills[13],
			aHBTopClientH[14], aHBTopKills[14],
			aHBTopClientH[15], aHBTopKills[15]);
		strcat(cTemp, cTxt);

		cp = (char*)cTemp;
		fwrite(cp, strlen(cp), 1, pFile);
		PutLogList("(!) Top Players file created.");
		fclose(pFile);
	}
}

void CGame::ordenarTop15HB(int iClientH) // MORLA 2.4 - Calcular si tiene mas Deaths
{
	int aux1, p, q;
	char aux3[11];

	for (p = 1; p <= 14; p++)
	{
		for (q = p + 1; q <= 15; q++)
		{
			if (aHBTopKills[p] < aHBTopKills[q])
			{
				aux1 = aHBTopKills[p];
				ZeroMemory(aux3, sizeof(aux3));
				strcpy(aux3, aHBTopClientH[p]);

				aHBTopKills[p] = aHBTopKills[q];
				ZeroMemory(aHBTopClientH[p], sizeof(aHBTopClientH[p]));
				strcpy(aHBTopClientH[p], aHBTopClientH[q]);

				aHBTopKills[q] = aux1;
				ZeroMemory(aHBTopClientH[q], sizeof(aHBTopClientH[q]));
				strcpy(aHBTopClientH[q], aux3);
			}
		}
	}
	_CreateTopPlayers();
}

void CGame::calcularTop15HB(int iClientH) // MORLA 2.4 - Calcular si tiene mas Deaths
{
	int p;
	bool bExiste = false;

	if (m_pClientList[iClientH]->m_iAdminUserLevel > 0) return;

	for (p = 1; p <= 15; p++)
	{
		if (strcmp(aHBTopClientH[p], m_pClientList[iClientH]->m_cCharName) == 0)
		{
			aHBTopKills[p] = m_pClientList[iClientH]->m_iEnemyKillCount;
			ZeroMemory(aHBTopClientH[p], sizeof(aHBTopClientH[p]));
			strcpy(aHBTopClientH[p], m_pClientList[iClientH]->m_cCharName);
			bExiste = true;
			ordenarTop15HB(iClientH);
		}
	}
	if (bExiste == false)
	{
		if (aHBTopKills[15] < m_pClientList[iClientH]->m_iEnemyKillCount)
		{
			aHBTopKills[15] = m_pClientList[iClientH]->m_iEnemyKillCount;
			ZeroMemory(aHBTopClientH[15], sizeof(aHBTopClientH[15]));
			strcpy(aHBTopClientH[15], m_pClientList[iClientH]->m_cCharName);
			ordenarTop15HB(iClientH);
		}
	}
}

void CGame::PlayerCommandCheckAdmins(int iClientH)
{//An Acidx Production - Last Updated:Aug.29.2006
	char  cNotifyMessage[256];
	int i, x;
	ZeroMemory(cNotifyMessage, sizeof(cNotifyMessage));
	x = 0;
	for (i = 0; i < DEF_MAXCLIENTS; i++) {
		if (m_pClientList[i] != 0) {
			if (m_pClientList[i]->m_iAdminUserLevel > 1) {
				wsprintf(cNotifyMessage, "Active Admin: %s", m_pClientList[i]->m_cCharName);
				SendNotifyMsg(iClientH, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, cNotifyMessage);
				x++;
			}
		}
	}
	wsprintf(cNotifyMessage, "%i Admins Are Online", x);
	SendNotifyMsg(iClientH, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, cNotifyMessage);
}

//Magn0S:: Added Teleport List
bool CGame::bDecodeTeleportList(char *pFn)
{
 FILE * pFile;
 HANDLE hFile;
 UINT32  dwFileSize;
 char * cp, * token, cTxt[120], cReadModeA = 0, cReadModeB = 0;
 char seps[] = "= \t\n";
 class CStrTok * pStrTok;
 int  iTeleportConfigListIndex = 0;

	for (int i = 0; i <  DEF_MAXTELEPORTLIST; i++)
		if (m_pTeleportConfigList[i] != 0) {
		delete m_pTeleportConfigList[i];
		m_pTeleportConfigList[i] = 0;
	}

	hFile = CreateFile(pFn, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
	dwFileSize = GetFileSize(hFile, 0);
	if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);
	pFile = fopen(pFn, "rt");
	if (pFile == 0) {
	PutLogList("(!) Cannot open Teleport.cfg file.");
	return false;
	}
	else {
	PutLogList("(!) Reading Teleport.cfg...");

	cp = new char[dwFileSize+2];
	ZeroMemory(cp, dwFileSize+2);
	fread(cp, dwFileSize, 1, pFile);

	pStrTok = new class CStrTok(cp, seps);
	token = pStrTok->pGet();
	while( token != 0 ) {
		if (cReadModeA != 0) {
			switch (cReadModeA) {
			case 1:
				switch (cReadModeB) {
				case 1: // Teleport
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! TELEPORT-LIST configuration file error - Wrong Data format.");
						delete pStrTok;
						delete[] cp;
						return false;
					}
					
					iTeleportConfigListIndex = atoi(token);

					if (m_pTeleportConfigList[iTeleportConfigListIndex] != 0) {
						PutLogList("(!!!) CRITICAL ERROR! TELEPORT-LIST configuration file error - Duplicate magic number.");
						delete pStrTok;
						delete[] cp;
						return false;
					}
					m_pTeleportConfigList[iTeleportConfigListIndex] = new class CTeleport;
					cReadModeB = 2;
					break;

				case 2: // NPC
					memcpy(m_pTeleportConfigList[iTeleportConfigListIndex]->m_cNpcname, token, strlen(token));
					cReadModeB = 3;
					break;

				case 3: // SOURCE MAP
					memcpy(m_pTeleportConfigList[iTeleportConfigListIndex]->m_cSourceMap, token, strlen(token));
					cReadModeB = 4;
					break;

				case 4: // TARGET MAP
					memcpy(m_pTeleportConfigList[iTeleportConfigListIndex]->m_cTargetMap, token, strlen(token));
					cReadModeB = 5;
					break;
				
				case 5: // TARGET MAP X
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! TELEPORT-LIST configuration file error - Wrong Data format.");
						delete pStrTok;
						delete[] cp;
						return false;
					}
					m_pTeleportConfigList[iTeleportConfigListIndex]->m_iX = atoi(token);
					cReadModeB = 6;
					break;

				case 6: // TARGET MAP Y
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! TELEPORT-LIST configuration file error - Wrong Data format.");
						delete pStrTok;
						delete[] cp;
						return false;
					}
					m_pTeleportConfigList[iTeleportConfigListIndex]->m_iY = atoi(token);
					cReadModeB = 7;
					break;

				case 7: // COST
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! TELEPORT-LIST configuration file error - Wrong Data format.");
						delete pStrTok;
						delete[] cp;
						return false;
					}

					m_pTeleportConfigList[iTeleportConfigListIndex]->m_iCost = atoi(token);
					cReadModeB = 8;
					break;

				case 8: // Minimum Level
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! TELEPORT-LIST configuration file error - Wrong Data format.");
						delete pStrTok;
						delete[] cp;
						return false;
					}
					m_pTeleportConfigList[iTeleportConfigListIndex]->m_iMinLvl = atoi(token);
					cReadModeB = 9;
					break;

				case 9: // Maximum Level
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! TELEPORT-LIST configuration file error - Wrong Data format.");
						delete pStrTok;
						delete[] cp;
						return false;
					}
					m_pTeleportConfigList[iTeleportConfigListIndex]->m_iMaxLvl = atoi(token);
					cReadModeB = 10;
					break;

				case 10: // Side (aresden, elvein)
					if( memcmp(token, "aresden", 7) == 0 )
					{
						m_pTeleportConfigList[iTeleportConfigListIndex]->m_iSide = 1;
					}
					else if( memcmp(token, "elvine", 6) == 0 )
					{
						m_pTeleportConfigList[iTeleportConfigListIndex]->m_iSide = 2;
					}
					else if( memcmp(token, "both", 4) == 0 )
					{
						m_pTeleportConfigList[iTeleportConfigListIndex]->m_iSide = DEF_BOTHSIDE;
					}
					else if( memcmp(token, "evil", 4) == 0 )
					{
						m_pTeleportConfigList[iTeleportConfigListIndex]->m_iSide = 4;
					}
					else if( memcmp(token, "all", 3) == 0 )
					{
						m_pTeleportConfigList[iTeleportConfigListIndex]->m_iSide = 124;
					}
					else if( memcmp(token, "heldenian", 9) == 0 )
					{
						m_pTeleportConfigList[iTeleportConfigListIndex]->m_iSide = 66;
                    }
					else
					{
						PutLogList("(!!!) CRITICAL ERROR! TELEPORT-LIST configuration file error - Wrong Data format.");
						delete pStrTok;
						delete[] cp;
						return false;
					}
					cReadModeB = 11;
					break;

				case 11: // HuntMode (0,1)
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! TELEPORT-LIST configuration file error - Wrong Data format.");
						delete pStrTok;
						delete[] cp;
						return false;
					}

					if( memcmp(token,"0",1) == 0 )
						m_pTeleportConfigList[iTeleportConfigListIndex]->m_bHunter = false;
					else if( memcmp(token,"1",1) == 0 )
						m_pTeleportConfigList[iTeleportConfigListIndex]->m_bHunter = true;
					else
					{
						PutLogList("(!!!) CRITICAL ERROR! TELEPORT-LIST configuration file error - Wrong Data format.");
						delete pStrTok;
						delete[] cp;
						return false;
					}
					cReadModeB = 12;
					break;

				case 12: // Neutral (0,1)
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! TELEPORT-LIST configuration file error - Wrong Data format.");
						delete pStrTok;
						delete[] cp;
						return false;
					}

					if( memcmp(token,"0",1) == 0 )
						m_pTeleportConfigList[iTeleportConfigListIndex]->m_bNetural = false;
					else if( memcmp(token,"1",1) == 0 )
						m_pTeleportConfigList[iTeleportConfigListIndex]->m_bNetural = true;
					else
					{
						PutLogList("(!!!) CRITICAL ERROR! TELEPORT-LIST configuration file error - Wrong Data format.");
						delete pStrTok;
						delete[] cp;
						return false;
					}
					cReadModeB = 13;
					break;

				case 13: // Criminal (0,1)
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! TELEPORT-LIST configuration file error - Wrong Data format.");
						delete pStrTok;
						delete[] cp;
						return false;
					}

					if( memcmp(token,"0",1) == 0 )
						m_pTeleportConfigList[iTeleportConfigListIndex]->m_bCriminal = false;
					else if( memcmp(token,"1",1) == 0 )
						m_pTeleportConfigList[iTeleportConfigListIndex]->m_bCriminal = true;
					else
					{
						PutLogList("(!!!) CRITICAL ERROR! TELEPORT-LIST configuration file error - Wrong Data format.");
						delete pStrTok;
						delete[] cp;
						return false;
					}
					cReadModeA = 0;
					cReadModeB = 0;
					break;
				}
				break;
			default:
				break;
			}
		}
		else
		{
			if (memcmp(token, "teleport", 8) == 0) {
				cReadModeA = 1;
				cReadModeB = 1;
			}
			else if (memcmp(token, "[END]", 5) == 0)
			{
				cReadModeA = 0;
				cReadModeB = 0;
				break; // Stop While Loop
			}
		}

		token = pStrTok->pGet();

	}

    delete[] cp;
	delete pStrTok;
	fclose(pFile);
	}
	if ((cReadModeA != 0) || (cReadModeB != 0) || (iTeleportConfigListIndex < 0) || (iTeleportConfigListIndex >= DEF_MAXTELEPORTLIST)) {
		PutLogList("(!!!) CRITICAL ERROR! TELEPORT-LIST configuration file contents error!");
		return false;
	}


	wsprintf(cTxt, "(!) TELEPORT-LIST(Total:%d) configuration - success!", iTeleportConfigListIndex);
	PutLogList(cTxt);

	return true;
}

void CGame::RequestTeleportListHandler(int iClientH, char* pData, UINT32 dwMsgSize)
{
	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
	if (m_pClientList[iClientH]->m_bIsKilled == true) return;
	if (m_pClientList[iClientH]->m_bIsOnWaitingProcess == true) return;

	char* cp, cData[512];
	int		iRet;
	UINT32* dwp;
	UINT16* wp;
	int		iMapSide = -1;
	int* listCount;
	char	cNpcName[21];
	int* ip;

	if (memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, "aresden", 7) == 0)
		iMapSide = DEF_ARESDEN;
	else if (memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, "elvine", 6) == 0)
		iMapSide = DEF_ELVINE;

	cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);
	ZeroMemory(cNpcName, sizeof(cNpcName));
	strncpy(cNpcName, cp, 20);
	cp += 20;

	ZeroMemory(cData, sizeof(cData));
	dwp = (UINT32*)(cData + DEF_INDEX4_MSGID);
	*dwp = MSGID_RESPONSE_TELEPORT_LIST;
	wp = (UINT16*)(cData + DEF_INDEX2_MSGTYPE);
	*wp = DEF_MSGTYPE_CONFIRM;

	cp = cData + 6;

	listCount = (int*)cp; *listCount = 0;
	cp += 4; 

	int		index;
	for (index = 0; index < DEF_MAXTELEPORTLIST; index++)
	{
		if (m_pTeleportConfigList[index] == 0) continue;
		//    Source Map 
		if (memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, m_pTeleportConfigList[index]->m_cSourceMap, 10) != 0)
			continue;
		//  NPC
		if (memcmp(m_pTeleportConfigList[index]->m_cNpcname, cNpcName, 20) != 0)
			continue;

		if (m_pTeleportConfigList[index]->m_iMinLvl > m_pClientList[iClientH]->m_iLevel ||
			m_pTeleportConfigList[index]->m_iMaxLvl < m_pClientList[iClientH]->m_iLevel)
			continue;

		if (m_pTeleportConfigList[index]->m_bHunter == false &&
			m_pClientList[iClientH]->m_bIsPlayerCivil == true)
			continue;

		if (m_pTeleportConfigList[index]->m_bNetural == false &&
			m_pClientList[iClientH]->m_cSide == DEF_NETURAL)
			continue;

		if (m_pTeleportConfigList[index]->m_bCriminal == false &&
			m_pClientList[iClientH]->m_iPKCount > 0)
			continue;

		// Side  (Check Point!!!!)
		if (m_pClientList[iClientH]->m_cSide == DEF_NETURAL &&
			(m_pTeleportConfigList[index]->m_iSide == DEF_BOTHSIDE ||
				m_pTeleportConfigList[index]->m_iSide == iMapSide))
		{
			ip = (int*)cp;
			*ip = index;
			cp += 4;

			memcpy(cp, m_pTeleportConfigList[index]->m_cTargetMap, 10);
			cp += 10;

			ip = (int*)cp;
			*ip = m_pTeleportConfigList[index]->m_iX;
			cp += 4;

			ip = (int*)cp;
			*ip = m_pTeleportConfigList[index]->m_iY;
			cp += 4;

			ip = (int*)cp;
			*ip = m_pTeleportConfigList[index]->m_iCost;
			cp += 4;
		}
		else if ((m_pTeleportConfigList[index]->m_iSide == DEF_BOTHSIDE) ||
			(m_pTeleportConfigList[index]->m_iSide == m_pClientList[iClientH]->m_cSide))
		{
			ip = (int*)cp;
			*ip = index;
			cp += 4;

			memcpy(cp, m_pTeleportConfigList[index]->m_cTargetMap, 10);
			cp += 10;

			ip = (int*)cp;
			*ip = m_pTeleportConfigList[index]->m_iX;
			cp += 4;

			ip = (int*)cp;
			*ip = m_pTeleportConfigList[index]->m_iY;
			cp += 4;

			ip = (int*)cp;
			*ip = m_pTeleportConfigList[index]->m_iCost;
			cp += 4;
		}
		else continue;

		(*listCount)++;
	}
	if ((*listCount) == 0)
		*wp = DEF_MSGTYPE_REJECT;

	iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 10 + (*listCount) * 26);

	switch (iRet) {
	case DEF_XSOCKEVENT_QUENEFULL:
	case DEF_XSOCKEVENT_SOCKETERROR:
	case DEF_XSOCKEVENT_CRITICALERROR:
	case DEF_XSOCKEVENT_SOCKETCLOSED:
		DeleteClient(iClientH, true, true);
		return;
	}
}

void CGame::RequestChargedTeleportHandler(int iClientH, char* pData, UINT32 dwMsgSize)
{
	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
	if (m_pClientList[iClientH]->m_bIsKilled == true) return;
	if (m_pClientList[iClientH]->m_bIsOnWaitingProcess == true) return;

	char* cp, cData[64];
	int		iRet;
	UINT32* dwp;
	UINT16* wp;
	int		iMapSide = -1;
	int		index;
	UINT16	wConfirm = DEF_MSGTYPE_CONFIRM;
	short	sError = 0;

	if (memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, "aresden", 7) == 0)
		iMapSide = DEF_ARESDEN;
	else if (memcmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, "elvine", 6) == 0)
		iMapSide = DEF_ELVINE;

	//   CHARGED_TELEPORT index
	cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);
	index = (int)(*cp);
	cp += 4;

	if (index < 0 || index >= DEF_MAXTELEPORTLIST)
		return;

	if (m_pTeleportConfigList[index] == 0)
		return;

	//    Source Map 
	if (strncmp(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName,
		m_pTeleportConfigList[index]->m_cSourceMap, 10) != 0)
		return;

	if (m_pTeleportConfigList[index]->m_iMinLvl > m_pClientList[iClientH]->m_iLevel ||
		m_pTeleportConfigList[index]->m_iMaxLvl < m_pClientList[iClientH]->m_iLevel)
	{
		wConfirm = DEF_MSGTYPE_REJECT;
		sError = 1;	//  
	}

	if (wConfirm == DEF_MSGTYPE_CONFIRM &&
		m_pTeleportConfigList[index]->m_bHunter == false &&
		m_pClientList[iClientH]->m_bIsPlayerCivil == true)
	{
		wConfirm = DEF_MSGTYPE_REJECT;
		sError = 2;
	}

	if (wConfirm == DEF_MSGTYPE_CONFIRM &&
		m_pTeleportConfigList[index]->m_bNetural == false &&
		m_pClientList[iClientH]->m_cSide == DEF_NETURAL)
	{
		wConfirm = DEF_MSGTYPE_REJECT;
		sError = 3;
	}

	if (wConfirm == DEF_MSGTYPE_CONFIRM &&
		m_pTeleportConfigList[index]->m_bCriminal == false &&
		m_pClientList[iClientH]->m_iPKCount > 0)
	{
		wConfirm = DEF_MSGTYPE_REJECT;
		sError = 4;
	}

	// Side
	if (wConfirm == DEF_MSGTYPE_CONFIRM &&
		m_pClientList[iClientH]->m_cSide == DEF_NETURAL &&
		(m_pTeleportConfigList[index]->m_iSide != DEF_BOTHSIDE && m_pTeleportConfigList[index]->m_iSide != iMapSide))
	{
		wConfirm = DEF_MSGTYPE_REJECT;
		sError = 5;
	}
	else if (
		wConfirm == DEF_MSGTYPE_CONFIRM &&
		m_pTeleportConfigList[index]->m_iSide != DEF_BOTHSIDE &&
		m_pTeleportConfigList[index]->m_iSide != m_pClientList[iClientH]->m_cSide)
	{
		wConfirm = DEF_MSGTYPE_REJECT;
		sError = 5;
	}
	//Magn0S::Cancel TP if arena is closed
	if (wConfirm == DEF_MSGTYPE_CONFIRM &&
		(memcmp(m_pTeleportConfigList[index]->m_cTargetMap, "fightzone1", 10) == 0) &&
		(!bDeathmatch))
	{
		wConfirm = DEF_MSGTYPE_REJECT;
		sError = 7;
	}

	if (wConfirm == DEF_MSGTYPE_CONFIRM &&
		(memcmp(m_pTeleportConfigList[index]->m_cTargetMap, "fightzone1", 10) == 0) &&
		(bDeathmatch))
	{
		RequestDismissPartyHandler(iClientH);
	}

	if (wConfirm == DEF_MSGTYPE_CONFIRM &&
		(memcmp(m_pTeleportConfigList[index]->m_cTargetMap, "team", 4) == 0) &&
		(!c_team->bteam))
	{
		wConfirm = DEF_MSGTYPE_REJECT;
		sError = 8;
	}

	if (wConfirm == DEF_MSGTYPE_CONFIRM &&
		(memcmp(m_pTeleportConfigList[index]->m_cTargetMap, "team", 4) == 0) &&
		(c_team->bteam))
	{
		RequestDismissPartyHandler(iClientH);
		c_team->Join(iClientH);
	}

	// Cost    
	if( wConfirm == DEF_MSGTYPE_CONFIRM )
	{
		UINT32 dwGoldCount = dwGetItemCount(iClientH, "Gold");

		if( dwGoldCount >= m_pTeleportConfigList[index]->m_iCost )
		{
			int iGoldWeight = SetItemCount(iClientH, "Gold", dwGoldCount - m_pTeleportConfigList[index]->m_iCost);

			iCalcTotalWeight(iClientH);
		} else {
			wConfirm = DEF_MSGTYPE_REJECT;
			sError = 6;	//
		}
	}
	// Invalid Charged-teleport
	if (wConfirm == DEF_MSGTYPE_REJECT)
	{
		ZeroMemory(cData, sizeof(cData));
		dwp = (UINT32*)(cData + DEF_INDEX4_MSGID);
		*dwp = MSGID_RESPONSE_CHARGED_TELEPORT;
		wp = (UINT16*)(cData + DEF_INDEX2_MSGTYPE);
		*wp = wConfirm;

		cp = cData + DEF_INDEX2_MSGTYPE + 2;

		short* sp;
		sp = (short*)cp;
		*sp = sError;
		cp += 2;
		iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 6 + 2);

		switch (iRet) {
		case DEF_XSOCKEVENT_QUENEFULL:
		case DEF_XSOCKEVENT_SOCKETERROR:
		case DEF_XSOCKEVENT_CRITICALERROR:
		case DEF_XSOCKEVENT_SOCKETCLOSED:
			DeleteClient(iClientH, true, true);
			return;
		}
		return;
	}

	// teleport .
	RequestTeleportHandler(iClientH, "2   ",
		m_pTeleportConfigList[index]->m_cTargetMap,
		m_pTeleportConfigList[index]->m_iX,
		m_pTeleportConfigList[index]->m_iY);

}

//Magn0S:: Function to receive orders from Client
void CGame::ReceivedClientOrder(int iClientH, int iOption1, int iOption2, int iOption3, char* cName)
{
	int i;
	FILE* pFile;
	char* token = 0, cMsg[100] = {}, cIPtoBan[15] = {};
	bool bExiste = false;

	if (m_pClientList[iClientH] == 0) return;

	switch (iOption1) {
	case 1:
		
		break;

	case 2: // GM Order = Enable Attacks
		if (m_pClientList[iClientH]->m_iAdminUserLevel < 2) {
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
			return;
		}
		
		switch (iOption2) {
		case 1:
			if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsAttackEnabled)
				m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsAttackEnabled = false;
			else m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsAttackEnabled = true;
			break;

		case 2:
			if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bMapTP)
				m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bMapTP = false;
			else m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bMapTP = true;
			break;

		case 3:
			if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bMapParty) {
				m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bMapParty = false;
			}
			else {
				m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bMapParty = true;

			}
			break;

		case 4:
			if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bMapIllusion)
				m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bMapIllusion = false;
			else m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bMapIllusion = true;
			break;

		case 5:
			if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bMapActivate)
				m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bMapActivate = false;
			else m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bMapActivate = true;
			break;

		case 6:
			if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bMapInvi)
				m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bMapInvi = false;
			else m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bMapInvi = true;
			break;

		case 7:
			if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bMapAMP)
				m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bMapAMP = false;
			else m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bMapAMP = true;
			break;

		case 8:
			if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bMapRegens)
				m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bMapRegens = false;
			else m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bMapRegens = true;
			break;

		case 9:
			if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bMapHideEnemy)
				m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bMapHideEnemy = false;
			else m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bMapHideEnemy = true;
			break;

		case 10:
			if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bMapBonusDmg)
				m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bMapBonusDmg = false;
			else m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bMapBonusDmg = true;
			break;

		case 11:
			if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bMapEquip)
				m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bMapEquip = false;
			else m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bMapEquip = true;
			break;

		default:
			return;
			break;
		}	
		
		for (i = 1; i < DEF_MAXCLIENTS; i++) {
			if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_bIsInitComplete == true) && (m_pClientList[i]->m_cMapIndex == m_pClientList[iClientH]->m_cMapIndex))
				NotifyMapRestrictions(iClientH, true);
		}
		break;

	case 3: // Revive
		if (m_pClientList[iClientH]->m_iAdminUserLevel < 2) {
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
			return;
		}
		if (cName != 0) {
			for (i = 1; i < DEF_MAXCLIENTS; i++)
				if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cCharName, cName, 10) == 0)) {
					if (m_pClientList[i]->m_iAdminUserLevel > 0) {
						SendNotifyMsg(0, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, "You are not allowed to do it with another GM.");
						return;
					}
					m_pClientList[i]->m_iHP = iGetMaxHP(i);
					m_pClientList[i]->m_bIsKilled = false;
					SendNotifyMsg(0, i, DEF_NOTIFY_HP, 0, 0, 0, 0);
					SendEventToNearClient_TypeA(i, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTDAMAGE, 1, 1, 0);
					m_pMapList[m_pClientList[i]->m_cMapIndex]->ClearOwner(14, i, DEF_OWNERTYPE_PLAYER, m_pClientList[i]->m_sX, m_pClientList[i]->m_sY);
					m_pMapList[m_pClientList[i]->m_cMapIndex]->SetDeadOwner(i, DEF_OWNERTYPE_PLAYER, m_pClientList[i]->m_sX, m_pClientList[i]->m_sY);
					return;
				}
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_PLAYERNOTONGAME, 0, 0, 0, cName);
		}
		break;

	case 4: // Kill
		if (m_pClientList[iClientH]->m_iAdminUserLevel < 2) {
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
			return;
		}
		if (cName != 0) {
			for (i = 1; i < DEF_MAXCLIENTS; i++)
				if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cCharName, cName, 10) == 0)) {
					if (m_pClientList[i]->m_iAdminUserLevel > 0) {
						SendNotifyMsg(0, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, "You are not allowed to do it with another GM.");
						return;
					}
					m_pClientList[i]->m_iHP = 0;
					m_pClientList[i]->m_bIsKilled = true;
					if (m_pClientList[i]->m_bIsExchangeMode == true) {
						_ClearExchangeStatus(m_pClientList[i]->m_iExchangeH);
						_ClearExchangeStatus(i);
					}
					RemoveFromTarget(i, DEF_OWNERTYPE_PLAYER);
					SendNotifyMsg(0, i, DEF_NOTIFY_KILLED, 0, 0, 0, m_pClientList[iClientH]->m_cCharName);
					SendEventToNearClient_TypeA(i, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTDYING, 1, 1, 0);
					m_pMapList[m_pClientList[i]->m_cMapIndex]->ClearOwner(12, i, DEF_OWNERTYPE_PLAYER, m_pClientList[i]->m_sX, m_pClientList[i]->m_sY);
					m_pMapList[m_pClientList[i]->m_cMapIndex]->SetDeadOwner(i, DEF_OWNERTYPE_PLAYER, m_pClientList[i]->m_sX, m_pClientList[i]->m_sY);
					return;
				}

		}
		break;

	case 5: // Shut up
		if (m_pClientList[iClientH]->m_iAdminUserLevel < 2) {
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
			return;
		} 
		if (cName != 0) {
			for (i = 1; i < DEF_MAXCLIENTS; i++)
				if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cCharName, cName, 10) == 0)) {
					if (m_pClientList[i]->m_iAdminUserLevel > 0) {
						SendNotifyMsg(0, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, "You are not allowed to do it with another GM.");
						return;
					}
					int iTime = 360;
					m_pClientList[i]->m_iTimeLeft_ShutUp = iTime * 20;
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_PLAYERSHUTUP, iTime, 0, 0, cName);
					SendNotifyMsg(0, i, DEF_NOTIFY_PLAYERSHUTUP, iTime, 0, 0, cName);
					wsprintf(G_cTxt, "GM Order(%s): Shutup PC(%s) (%d)Min", m_pClientList[iClientH]->m_cCharName, m_pClientList[i]->m_cCharName, iTime);
					PutLogList(G_cTxt);
				}
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_PLAYERNOTONGAME, 0, 0, 0, cName);
		}
		break;

	case 6: //Disconnect
		if (m_pClientList[iClientH]->m_iAdminUserLevel < 2) {
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
			return;
		}
		if (cName != 0) {
			for (i = 1; i < DEF_MAXCLIENTS; i++)
				if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cCharName, cName, 10) == 0)) {
					if (m_pClientList[i]->m_iAdminUserLevel > 0) {
						SendNotifyMsg(0, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, "You are not allowed to do it with another GM.");
						return;
					}

					// centu - block in bisle
					ZeroMemory(m_pClientList[i]->m_cLockedMapName, sizeof(m_pClientList[i]->m_cLockedMapName));
					strcpy(m_pClientList[i]->m_cLockedMapName, "bisle");
					m_pClientList[i]->m_iLockedMapTime = 10 * 60;
					RequestTeleportHandler(i, "2   ", "bisle", -1, -1);

					wsprintf(G_cTxt, "Admin Order(%s): Block char in Bisle - Char Name: (%s)", m_pClientList[iClientH]->m_cCharName, m_pClientList[i]->m_cCharName);
					PutLogList(G_cTxt);
					
					return;
				}
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_PLAYERNOTONGAME, 0, 0, 0, cName);
		}
		break;

	case 7: // players info
		if (m_pClientList[iClientH]->m_iAdminUserLevel < 4) {
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
			return;
		}
		break;

	case 8: // 1 semana em Bisle
		if (m_pClientList[iClientH]->m_iAdminUserLevel < 3) {
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
			return;
		}
		if (cName != 0) {
			for (i = 1; i < DEF_MAXCLIENTS; i++)
				if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cCharName, cName, 10) == 0)) {
					if (m_pClientList[i]->m_iAdminUserLevel > 0) {
						SendNotifyMsg(0, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, "You are not allowed to do it with another GM.");
						return;
					}
					
					// centu - block in bisle
					ZeroMemory(m_pClientList[i]->m_cLockedMapName, sizeof(m_pClientList[i]->m_cLockedMapName));
					strcpy(m_pClientList[i]->m_cLockedMapName, "bisle");
					m_pClientList[i]->m_iLockedMapTime = 24 * 60 * iOption2;
					RequestTeleportHandler(i, "2   ", "bisle", -1, -1);

					wsprintf(G_cTxt, "Admin Order(%s): Block char in Bisle - Char Name: (%s)", m_pClientList[iClientH]->m_cCharName, m_pClientList[i]->m_cCharName);
					PutLogList(G_cTxt);
					
					return;
				}
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_PLAYERNOTONGAME, 0, 0, 0, cName);
		}
		break;

	case 9: // Block Char (1 month)
		if (m_pClientList[iClientH]->m_iAdminUserLevel < 4) {
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
			return;
		}

		if (cName != 0) {
			for (i = 1; i < DEF_MAXCLIENTS; i++)
				if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cCharName, cName, strlen(cName)) == 0)) {
					if (m_pClientList[i]->m_iAdminUserLevel > 0) {
						SendNotifyMsg(0, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, "You are not allowed to do it with another GM.");
						return;
					}
					SYSTEMTIME SysTime;
					GetLocalTime(&SysTime);
					//iOption2 = Time of ban.
					if (SysTime.wDay + iOption2 > 30) {
						if ((SysTime.wMonth + 1) >= 13) {
							m_pClientList[i]->m_iPenaltyBlockYear = SysTime.wYear + 1;
							m_pClientList[i]->m_iPenaltyBlockMonth = 1;
							m_pClientList[i]->m_iPenaltyBlockDay = SysTime.wDay + iOption2;
						}
						else {
							m_pClientList[i]->m_iPenaltyBlockYear = SysTime.wYear;
							m_pClientList[i]->m_iPenaltyBlockMonth = SysTime.wMonth + 1;
							m_pClientList[i]->m_iPenaltyBlockDay = SysTime.wDay - iOption2;
						}
					}

					// centu - block in bisle
					ZeroMemory(m_pClientList[i]->m_cLockedMapName, sizeof(m_pClientList[i]->m_cLockedMapName));
					strcpy(m_pClientList[i]->m_cLockedMapName, "bisle");
					m_pClientList[i]->m_iLockedMapTime = (24 * 60 * iOption2) * 4;
					RequestTeleportHandler(i, "2   ", "bisle", -1, -1);

					wsprintf(cMsg, "Char %s were blocked for 1 month in BI.", m_pClientList[i]->m_cCharName);
					ShowClientMsg(iClientH, cMsg);

					PutLogList(G_cTxt);
					PutLogFileList(G_cTxt);

					return;
				}
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_PLAYERNOTONGAME, 0, 0, 0, cName);
		}
		break;

	case 10: // Ban IP
		if (m_pClientList[iClientH]->m_iAdminUserLevel < 4) {
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
			return;
		}

		if (cName != 0) {
			for (i = 1; i < DEF_MAXCLIENTS; i++)
				if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cCharName, cName, strlen(cName)) == 0)) {
					if (m_pClientList[i]->m_iAdminUserLevel > 0) {
						SendNotifyMsg(0, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, "You are not allowed to do it with another GM.");
						return;
					}
					strcpy(cIPtoBan, m_pClientList[i]->m_cIPaddress);
					pFile = fopen("GameConfigs\\BannedList.cfg", "a");

					wsprintf(cMsg, "The Ip Address: %s is Banned From Server.", cIPtoBan);
					ShowClientMsg(iClientH, cMsg);
					wsprintf(G_cTxt, "<%d> Client IP Banned: (%s)", i, cIPtoBan);
					PutLogList(G_cTxt);
					fprintf(pFile, "banned-ip = %s", cIPtoBan);
					fprintf(pFile, "\n");
					fclose(pFile);

					for (auto x = 0; x < DEF_MAXBANNED; x++)
						if (strlen(m_stBannedList[x].m_cBannedIPaddress) == 0)
							strcpy(m_stBannedList[x].m_cBannedIPaddress, cIPtoBan);
					DeleteClient(i, true, true);
					return;
				}
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_PLAYERNOTONGAME, 0, 0, 0, cName);
		}
		break;

	case 24:
		if (m_pClientList[iClientH]->m_iAdminUserLevel < 3) {
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
			return;
		}

		switch (iOption2) {
		case 1: // Primary Drop Manager
			if (iOption3 == 0)
				m_iPrimaryDropRate += 500;
			else m_iPrimaryDropRate -= 500;
			
			if (m_iPrimaryDropRate <= 0) m_iPrimaryDropRate = 0;
			if (m_iPrimaryDropRate >= 10000) m_iPrimaryDropRate = 10000;

			SendNotifyMsg(0, iClientH, DEF_NOTIFY_IPACCOUNTINFO, 0, 0, 0, "You have adjusted the Primary Drop Rate");
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_SERVERTIME, 0, 0, 0, 0);
			return;
			break;

		case 2: // Secondary Drop Manager
			if (iOption3 == 0)
				m_iSecondaryDropRate += 500;
			else m_iSecondaryDropRate -= 500;

			if (m_iSecondaryDropRate <= 0) m_iSecondaryDropRate = 0;
			if (m_iSecondaryDropRate >= 10000) m_iSecondaryDropRate = 10000;

			SendNotifyMsg(0, iClientH, DEF_NOTIFY_IPACCOUNTINFO, 0, 0, 0, "You have adjusted the Secondary Drop Rate");
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_SERVERTIME, 0, 0, 0, 0);
			return;
			break;

		case 3: // Stated Drop Manager
			if (iOption3 == 0)
				m_iRareDropRate += 500;
			else m_iRareDropRate -= 500;

			if (m_iRareDropRate <= 0) m_iRareDropRate = 0;
			if (m_iRareDropRate >= 10000) m_iRareDropRate = 10000;

			SendNotifyMsg(0, iClientH, DEF_NOTIFY_IPACCOUNTINFO, 0, 0, 0, "You have adjusted the Stated Drop Rate");
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_SERVERTIME, 0, 0, 0, 0);
			return;
			break;
			
		case 4: // Able / Unable stated drop by definition
			switch (iOption3) {
			case 1:
				if (m_bNullDrop[DROP_MA]) {
					m_bNullDrop[DROP_MA] = true;
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_IPACCOUNTINFO, 0, 0, 0, "You have enabled MA Stats to drop.");
				}
				else {
					m_bNullDrop[DROP_MA] = false;
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_IPACCOUNTINFO, 0, 0, 0, "You have disabled MA Stats to drop.");
				}
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_SERVERTIME, 0, 0, 0, 0);
				return;
				break;

			case 2:
				if (m_bNullDrop[DROP_PA]) {
					m_bNullDrop[DROP_PA] = true;
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_IPACCOUNTINFO, 0, 0, 0, "You have enabled PA Stats to drop.");
				}
				else {
					m_bNullDrop[DROP_PA] = false;
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_IPACCOUNTINFO, 0, 0, 0, "You have disabled PA Stats to drop.");
				}
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_SERVERTIME, 0, 0, 0, 0);
				return;
				break;

			default:
				return;
				break;
			}

			break;

		case 5: // Server Physical Damage
			if (iOption3 == 0)
				m_iServerPhyDmg += 5;
			else m_iServerPhyDmg -= 5;

			if (m_iServerPhyDmg <= 0) m_iServerPhyDmg = 0;
			if (m_iServerPhyDmg >= 100) m_iServerPhyDmg = 100;

			SendNotifyMsg(0, iClientH, DEF_NOTIFY_IPACCOUNTINFO, 0, 0, 0, "You have adjusted the Server Physical Damage");
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_SERVERTIME, 0, 0, 0, 0);
			return;
			break;

		case 6: // Server Magical Damage
			if (iOption3 == 0)
				m_iServerMagDmg += 5;
			else m_iServerMagDmg -= 5;

			if (m_iServerMagDmg <= 0) m_iServerMagDmg = 0;
			if (m_iServerMagDmg >= 100) m_iServerMagDmg = 100;

			SendNotifyMsg(0, iClientH, DEF_NOTIFY_IPACCOUNTINFO, 0, 0, 0, "You have adjusted the Server Magical Damage");
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_SERVERTIME, 0, 0, 0, 0);
			return;
			break;

		default:
			return;
			break;

			break;

		}
		break;

	case 27:
		NotifyPlayerAttributes(iClientH);
		break;

	case 28: // Teleport Request
		if (m_pClientList[iClientH]->m_iAdminUserLevel < 1) {
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
			return;
		}
		//Magn0S:: Add to prevent players to teleport to an Offline Map
		if (cName != 0) {
			for (i = 0; i < DEF_MAXMAPS; i++)
				if ((m_pMapList[i] != 0) && (memcmp(m_pMapList[i]->m_cName, cName, strlen(cName)) == 0)) {
					RequestTeleportHandler(iClientH, "2   ", cName, -1, -1);
					return;
			}
			ShowClientMsg(iClientH, "This map is offline, and you can't teleport into.");
		}
		
		break;

	case 29: // GM Auto TP by Mini Map Click
		if (m_pClientList[iClientH]->m_iAdminUserLevel < 1) {
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
			return;
		}
		RequestTeleportHandler(iClientH, "2   ", cName, iOption2, iOption3);
		break;


	//----------Admin Commands--------------------------------------------------------------------------------
	case 30: // Active GM Comnmand
		if (m_pClientList[iClientH]->m_iAdminUserLevel < 1) {
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
			return;
		}
		m_pClientList[iClientH]->m_bIsAdminCommandEnabled = true;
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, "GM Command actived.");
		break;

	case 31: // Observe Mode
		if (m_pClientList[iClientH]->m_iAdminUserLevel < 4) {
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
			return;
		}
		AdminOrder_SetObserverMode(iClientH);

		break;

	case 34: //DC All
		if (m_pClientList[iClientH]->m_iAdminUserLevel < 4) {
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
			return;
		}
		if (m_pClientList[iClientH]->m_bIsAdminCommandEnabled == false) return;

		_iForcePlayerDisconect(DEF_MAXCLIENTS);
		break;

	case 35: //Shut down server
		if (m_pClientList[iClientH]->m_iAdminUserLevel < 4) {
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
			return;
		}
		m_cShutDownCode = 4;
		m_bOnExitProcess = true;
		m_dwExitProcessTime = timeGetTime();
		PutLogList("(!) GAME SERVER SHUTDOWN PROCESS BEGIN(by Admin-Command)!!!");
		bSendMsgToLS(MSGID_GAMESERVERSHUTDOWNED, 0);
		break;

	case 36: //Destroy GM itens
		if (m_pClientList[iClientH]->m_iAdminUserLevel < 1) {
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
			return;
		}

		for (i = 0; i < DEF_MAXITEMS; i++) {
			ItemDepleteHandler(iClientH, i, false, true);
		}
		break;

	//----------Event Manager----------------------------------------------------------------------------------
	case 50: // Glad Arena
		if (m_pClientList[iClientH]->m_iAdminUserLevel < 3) {
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
			return;
		}

		if (!bDeathmatch)
			ManualStartGladiatorArena(iClientH, 0, 0);
		else ManualEndGladiatorArena(iClientH, 0, 0);

		SendNotifyMsg(0, iClientH, DEF_NOTIFY_EVENTUPDATER, (int)bDeathmatch, (int)m_bIsCrusadeMode, (int)m_bIsApocalypseMode, 0, (int)m_bIsHeldenianMode, (int)m_bIsCTFEvent);
		break;

	case 51: // Crusade Command
		if (m_pClientList[iClientH]->m_iAdminUserLevel < 3) {
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
			return;
		}
		if (!m_bIsCrusadeMode)
			GlobalStartCrusadeMode();
		else ManualEndCrusadeMode(0);
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_EVENTUPDATER, (int)bDeathmatch, (int)m_bIsCrusadeMode, (int)m_bIsApocalypseMode, 0, (int)m_bIsHeldenianMode, (int)m_bIsCTFEvent);
		break;

	case 52: // Apocalypse Command
		if (m_pClientList[iClientH]->m_iAdminUserLevel < 3) {
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
			return;
		}
		if (!m_bIsApocalypseMode)
			GlobalStartApocalypseMode(iClientH, 0);
		else GlobalEndApocalypseMode(iClientH);
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_EVENTUPDATER, (int)bDeathmatch, (int)m_bIsCrusadeMode, (int)m_bIsApocalypseMode, 0, (int)m_bIsHeldenianMode, (int)m_bIsCTFEvent);
		break;

	case 53: // Heldenian Command
		if (m_pClientList[iClientH]->m_iAdminUserLevel < 3) {
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
			return;
		}
		if (!m_bIsHeldenianMode) {
			GlobalStartHeldenianMode();
		}
		else {
			m_cHeldenianWinner = 0;
			GlobalEndHeldenianMode();
		}
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_EVENTUPDATER, (int)bDeathmatch, (int)m_bIsCrusadeMode, (int)m_bIsApocalypseMode, 0, (int)m_bIsHeldenianMode, (int)m_bIsCTFEvent);
		break;

	case 54: // CTF Command
		break;
		//---------------------------- PLAYERS COMMANDS---------------------------------------------------
	case 60: //who
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_TOTALUSERS, 0, 0, 0, 0);
		break;

	case 70: // Centuu - mark on minimap
		// X = iOption2
		// Y = iOption3
		// Map = cName

		for (i = 0; i < DEF_MAXGUILDS; i++)
		{
			if (string(m_stGuildInfo[i].cGuildName) == "NONE") continue;
			if (string(m_stGuildInfo[i].cGuildName) == m_pClientList[iClientH]->m_cGuildName)
			{
				m_stGuildInfo[i].markX = iOption2;
				m_stGuildInfo[i].markY = iOption3;
				strcpy(m_stGuildInfo[i].cMap, cName);
				m_stGuildInfo[i].dwMarkTime = timeGetTime() + 10 * 60 * 1000;
				bExiste = true;
				break;
			}
		}
		if (!bExiste)
		{
			for (i = 0; i < DEF_MAXGUILDS; i++)
			{
				if (string(m_stGuildInfo[i].cGuildName) == "NONE")
				{
					strcpy(m_stGuildInfo[i].cGuildName, m_pClientList[iClientH]->m_cGuildName);
					m_stGuildInfo[i].markX = iOption2;
					m_stGuildInfo[i].markY = iOption3;
					strcpy(m_stGuildInfo[i].cMap, cName);
					m_stGuildInfo[i].dwMarkTime = timeGetTime() + 10 * 60 * 1000;
					break;
				}
			}
		}

		minimap_clear_mark(iClientH);
		minimap_update_mark(iClientH);
		//
		
		break;

	default:
		break;
	}
}

void CGame::minimap_clear_mark(int client)
{
	char cData[56];
	UINT32* dwp;
	UINT16* wp;
	char* cp;
	int* ip;
	auto p = m_pClientList[client];
	if (!p) return;

	if (!bShinning && p->m_iGuildRank != -1)
	{
		if (!p->IsInMap("cityhall_1") && !p->IsInMap("cityhall_2") && !p->IsInMap("arejail") && !p->IsInMap("elvjail") && !p->IsInMap("default"))
		{
			dwp = (UINT32*)(cData + DEF_INDEX4_MSGID);
			*dwp = MSGID_NOTIFY;
			wp = (UINT16*)(cData + DEF_INDEX2_MSGTYPE);

			*wp = MINIMAPORANGE_CLEAR;

			cp = (char*)(cData + DEF_INDEX2_MSGTYPE + 2);
			ip = (int*)cp;
			*ip = client;
			cp += 4;

			for (int i = 0; i < DEF_MAXCLIENTS; i++)
			{
				auto pi = m_pClientList[i];

				if (!pi) continue;

				if (strcmp(pi->m_cGuildName, "NONE") == 0) continue;
				if (strcmp(pi->m_cGuildName, p->m_cGuildName) != 0) continue;

				pi->m_pXSock->iSendMsg(cData, 10);
			}
		}
	}
}

void CGame::minimap_update_mark(int client)
{
	char cData[56];
	UINT32* dwp;
	UINT16* wp;
	char* cp;
	int* ip;
	short* sp;
	auto p = m_pClientList[client];
	if (!p) return;

	if (strcmp(p->m_cMapName, m_stGuildInfo[ObtenerID(p->m_cGuildName)].cMap) != 0)
	{
		minimap_clear_mark(client);
		return;
	}

	if (!bShinning && p->m_iGuildRank != -1)
	{
		if (ObtenerX(p->m_cGuildName) == -1 && ObtenerY(p->m_cGuildName) == -1) return;
		if (!p->IsInMap("cityhall_1") && !p->IsInMap("cityhall_2") && !p->IsInMap("arejail") && !p->IsInMap("elvjail") && !p->IsInMap("default"))
		{
			dwp = (UINT32*)(cData + DEF_INDEX4_MSGID);
			*dwp = MSGID_NOTIFY;
			wp = (UINT16*)(cData + DEF_INDEX2_MSGTYPE);

			*wp = MINIMAPORANGE_UPDATE;

			cp = (char*)(cData + DEF_INDEX2_MSGTYPE + 2);
			ip = (int*)cp;
			*ip = client;
			cp += 4;

			sp = (short*)cp;
			*sp = ObtenerX(p->m_cGuildName);
			cp += 2;
			sp = (short*)cp;
			*sp = ObtenerY(p->m_cGuildName);
			cp += 2;

			for (int i = 0; i < DEF_MAXCLIENTS; i++)
			{
				auto pi = m_pClientList[i];

				if (!pi) continue;

				if (pi->m_cMapIndex == -1)	continue;

				if (strcmp(pi->m_cGuildName, "NONE") == 0) continue;
				if (strcmp(pi->m_cGuildName, p->m_cGuildName) != 0) continue;

				pi->m_pXSock->iSendMsg(cData, 14);
			}
		}
	}
}

//Magn0S:: Command to Add GM
void CGame::AdminOrder_AddGM(int iClientH, char* pData, UINT32 dwMsgSize)
{
	char seps[] = "= ";
	char* token, cBuff[256];
	class CStrTok* pStrTok;
	char* pk;
	char cNickName[21];
	int i;
	FILE* pFile;

	if (m_pClientList[iClientH] == 0) return;
	if ((dwMsgSize) <= 0) return;
	if (m_pClientList[iClientH]->m_iAdminUserLevel <= 3) {
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
		return;
	}

	ZeroMemory(cBuff, sizeof(cBuff));
	memcpy(cBuff, pData, dwMsgSize);
	pStrTok = new class CStrTok(cBuff, seps);
	token = pStrTok->pGet();
	token = pStrTok->pGet();
	if (token == 0) {
		delete pStrTok;
		return;
	}
	strcpy(cNickName, token);
	pk = pStrTok->pGet();


	if (pk == 0)
	{
		delete pStrTok;
		return;
	}
	char cPK[112];
	strcpy(cPK, pk);
	int level = atoi(cPK);
	if ((strlen(cNickName) < 3) || (strlen(cNickName) > 10)) return;

	for (i = 0; i < DEF_MAXCLIENTS; i++) {
		if (m_pClientList[i] != 0) {
			//opens cfg file
			pFile = fopen("GameConfigs\\AdminList.cfg", "a");
			wsprintf(G_cTxt, "<%d> Added new GM: (%s)", i, cNickName);
			PutLogList(G_cTxt);
			//modifys cfg file
			fprintf(pFile, "verified-admin = %s", cNickName);
			fprintf(pFile, "\n");
			fclose(pFile);

			ShowClientMsg(iClientH, "You have changed the Admin List.");
			//kazin
			for (auto x = 0; x < DEF_MAXADMINS; x++)
				if (strlen(m_stAdminList[x].m_cGMName) == 0)
					strcpy(m_stAdminList[x].m_cGMName, cNickName);

			m_pClientList[i]->m_iAdminUserLevel = level;
			m_pClientList[i]->m_iStatus = m_pClientList[i]->m_iStatus | 0x00040000;
		}
	}
	delete pStrTok;
}

void CGame::AdminOrder_SetEk(int iClientH, char* pData, UINT32 dwMsgSize)
{
	char seps[] = "= \t\n";
	char* token, cBuff[256];
	class CStrTok* pStrTok;
	char* pk;
	char cNick[20];
	char cPK[5];
	char notice[100];
	int oldpk;
	int i;
	if (m_pClientList[iClientH] == 0)
		return;
	if ((dwMsgSize) <= 0)
		return;
	if (m_pClientList[iClientH]->m_iAdminUserLevel < 2)
	{
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
		return;
	}
	ZeroMemory(cBuff, sizeof(cBuff));
	memcpy(cBuff, pData, dwMsgSize);
	pStrTok = new class CStrTok(cBuff, seps);
	token = pStrTok->pGet();
	token = pStrTok->pGet();
	if (token == 0)
	{
		delete pStrTok;
		return;
	}
	strcpy(cNick, token);
	pk = pStrTok->pGet();
	if (pk == 0)
	{
		delete pStrTok;
		return;
	}
	strcpy(cPK, pk);
	if (atoi(cPK) > 999999)
	{
		delete pStrTok;
		return;
	}
	for (i = 0; i < DEF_MAXCLIENTS; i++)
	{
		if ((m_pClientList[i] != 0)
			&& (memcmp(m_pClientList[i]->m_cCharName, cNick, strlen(cNick)) == 0))
		{
			oldpk = m_pClientList[i]->m_iEnemyKillCount;
			m_pClientList[i]->m_iEnemyKillCount += atoi(cPK);
			if (m_pClientList[i]->m_iEnemyKillCount > m_pClientList[i]->m_iMaxEK)
			{
				m_pClientList[i]->m_iMaxEK = m_pClientList[i]->m_iEnemyKillCount;
			}
			SendNotifyMsg(0, i, DEF_NOTIFY_ENEMYKILLS, m_pClientList[i]->m_iEnemyKillCount, m_pClientList[i]->m_iMaxEK, 0, 0);
			char cTxt[250];
			wsprintf(cTxt, "Congratulations, you have received %d Ek Points. (New Total: %d)", atoi(cPK), m_pClientList[i]->m_iEnemyKillCount);
			SendNotifyMsg(0, i, DEF_NOTIFY_IPACCOUNTINFO, 0, 0, 0, cTxt);
			calcularTop15HB(i); // centu - ordenar top ek
			wsprintf(notice, "%d Ek Points has been charged for player %s from %d to %d.", atoi(cPK), m_pClientList[i]->m_cCharName, oldpk, m_pClientList[i]->m_iEnemyKillCount);
			PutLogList(notice);
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, notice);
		}
	}
	delete pStrTok;
}

void CGame::AdminOrder_SetCoin(int iClientH, char* pData, UINT32 dwMsgSize)
{
	char seps[] = "= \t\n";
	char* token, cBuff[256];
	class CStrTok* pStrTok;
	char* pk;
	char cNick[20];
	char cPK[5];
	char notice[100];
	int oldpk;
	int i;
	if (m_pClientList[iClientH] == 0)
		return;
	if ((dwMsgSize) <= 0)
		return;
	if (m_pClientList[iClientH]->m_iAdminUserLevel < 2)
	{
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0, 0, 0);
		return;
	}
	ZeroMemory(cBuff, sizeof(cBuff));
	memcpy(cBuff, pData, dwMsgSize);
	pStrTok = new class CStrTok(cBuff, seps);
	token = pStrTok->pGet();
	token = pStrTok->pGet();
	if (token == 0)
	{
		delete pStrTok;
		return;
	}
	strcpy(cNick, token);
	pk = pStrTok->pGet();
	if (pk == 0)
	{
		delete pStrTok;
		return;
	}
	strcpy(cPK, pk);
	if (atoi(cPK) > 999999)
	{
		delete pStrTok;
		return;
	}
	for (i = 0; i < DEF_MAXCLIENTS; i++)
	{
		if ((m_pClientList[i] != 0)
			&& (memcmp(m_pClientList[i]->m_cCharName, cNick, strlen(cNick)) == 0))
		{
			oldpk = m_pClientList[i]->m_iCoinPoints;
			m_pClientList[i]->m_iCoinPoints += atoi(cPK);
			SendNotifyMsg(0, i, DEF_NOTIFY_COINPOINTS, m_pClientList[i]->m_iCoinPoints, 0, 0, 0);
			char cTxt[250];
			wsprintf(cTxt, "Congratulations, you have received %d Coin Points. (New Total: %d)", atoi(cPK), m_pClientList[i]->m_iCoinPoints);
			SendNotifyMsg(0, i, DEF_NOTIFY_IPACCOUNTINFO, 0, 0, 0, cTxt);

			wsprintf(notice, "%d Coin Points has been charged for player %s from %d to %d.", atoi(cPK), m_pClientList[i]->m_cCharName, oldpk, m_pClientList[i]->m_iCoinPoints);
			PutLogList(notice);
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, notice);
		}
	}
	delete pStrTok;
}

void CGame::AdminOrder_SetContrib(int iClientH, char* pData, UINT32 dwMsgSize)
{
	char seps[] = "= \t\n";
	char* token, cBuff[256];
	class CStrTok* pStrTok;
	char* pk;
	char cNick[20];
	char cPK[5];
	char notice[100];
	int oldpk;
	int i;
	if (m_pClientList[iClientH] == 0)
		return;
	if ((dwMsgSize) <= 0)
		return;
	if (m_pClientList[iClientH]->m_iAdminUserLevel < 2)
	{
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0,	0, 0);
		return;
	}
	ZeroMemory(cBuff, sizeof(cBuff));
	memcpy(cBuff, pData, dwMsgSize);
	pStrTok = new class CStrTok(cBuff, seps);
	token = pStrTok->pGet();
	token = pStrTok->pGet();
	if (token == 0)
	{
		delete pStrTok;
		return;
	}
	strcpy(cNick, token);
	pk = pStrTok->pGet();
	if (pk == 0)
	{
		delete pStrTok;
		return;
	}
	strcpy(cPK, pk);
	if (atoi(cPK) > 999999)
	{
		delete pStrTok;
		return;
	}
	for (i = 0; i < DEF_MAXCLIENTS; i++)
	{
		if ((m_pClientList[i] != 0)
			&& (memcmp(m_pClientList[i]->m_cCharName, cNick, strlen(cNick)) == 0))
		{
			oldpk = m_pClientList[i]->m_iContribution;
			m_pClientList[i]->m_iContribution += atoi(cPK);
			SendNotifyMsg(0, i, DEF_NOTIFY_CONTRIBPOINTS, m_pClientList[i]->m_iContribution, 0, 0, 0);
			char cTxt[250];
			wsprintf(cTxt, "Congratulations, you have received %d Contribution Points. (New Total: %d)", atoi(cPK), m_pClientList[i]->m_iContribution);
			SendNotifyMsg(0, i, DEF_NOTIFY_IPACCOUNTINFO, 0, 0, 0, cTxt);

			wsprintf(notice, "%d Contribution Points has been charged for player %s from %d to %d.", atoi(cPK), m_pClientList[i]->m_cCharName, oldpk, m_pClientList[i]->m_iContribution);
			PutLogList(notice);
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, notice);
		}
	}
	delete pStrTok;
}

void CGame::AdminOrder_SetMAJ(int iClientH, char* pData, UINT32 dwMsgSize)
{
	char seps[] = "= \t\n";
	char* token, cBuff[256];
	class CStrTok* pStrTok;
	char* pk;
	char cNick[20];
	char cPK[5];
	char notice[100];
	int oldpk;
	int i;
	if (m_pClientList[iClientH] == 0)
		return;
	if ((dwMsgSize) <= 0)
		return;
	if (m_pClientList[iClientH]->m_iAdminUserLevel < 2)
	{
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0,	0, 0);
		return;
	}
	ZeroMemory(cBuff, sizeof(cBuff));
	memcpy(cBuff, pData, dwMsgSize);
	pStrTok = new class CStrTok(cBuff, seps);
	token = pStrTok->pGet();
	token = pStrTok->pGet();
	if (token == 0)
	{
		delete pStrTok;
		return;
	}
	strcpy(cNick, token);
	pk = pStrTok->pGet();
	if (pk == 0)
	{
		delete pStrTok;
		return;
	}
	strcpy(cPK, pk);
	if (atoi(cPK) > 999999)
	{
		delete pStrTok;
		return;
	}
	for (i = 0; i < DEF_MAXCLIENTS; i++)
	{
		if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cCharName, cNick, strlen(cNick)) == 0))
		{
			oldpk = m_pClientList[i]->m_iGizonItemUpgradeLeft;
			m_pClientList[i]->m_iGizonItemUpgradeLeft += atoi(cPK);
			SendNotifyMsg(0, i, DEF_NOTIFY_GIZONITEMUPGRADELEFT, m_pClientList[i]->m_iGizonItemUpgradeLeft, 0, 0, 0);
			char cTxt[250];
			wsprintf(cTxt, "Congratulations, you have received %d Majestic Points. (New Total: %d)", atoi(cPK), m_pClientList[i]->m_iGizonItemUpgradeLeft);
			SendNotifyMsg(0, i, DEF_NOTIFY_IPACCOUNTINFO, 0, 0, 0, cTxt);

			wsprintf(notice, "%d Majestic Points has been charged for player %s from %d to %d.", atoi(cPK), m_pClientList[i]->m_cCharName, oldpk, m_pClientList[i]->m_iGizonItemUpgradeLeft);
			PutLogList(notice);
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, notice);
		}
	}
	delete pStrTok;
}

void CGame::AdminOrder_SetRep(int iClientH, char* pData, UINT32 dwMsgSize)
{
	char seps[] = "= \t\n";
	char* token, cBuff[256];
	class CStrTok* pStrTok;
	char* pk;
	char cNick[20];
	char cPK[5];
	char notice[100];
	int oldpk;
	int i;
	if (m_pClientList[iClientH] == 0)
		return;
	if ((dwMsgSize) <= 0)
		return;
	if (m_pClientList[iClientH]->m_iAdminUserLevel < 2)
	{
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0,	0, 0);
		return;
	}
	ZeroMemory(cBuff, sizeof(cBuff));
	memcpy(cBuff, pData, dwMsgSize);
	pStrTok = new class CStrTok(cBuff, seps);
	token = pStrTok->pGet();
	token = pStrTok->pGet();
	if (token == 0)
	{
		delete pStrTok;
		return;
	}
	strcpy(cNick, token);
	pk = pStrTok->pGet();
	if (pk == 0)
	{
		delete pStrTok;
		return;
	}
	strcpy(cPK, pk);
	if (atoi(cPK) > 999999)
	{
		delete pStrTok;
		return;
	}
	for (i = 0; i < DEF_MAXCLIENTS; i++)
	{
		if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cCharName, cNick, strlen(cNick)) == 0))
		{
			oldpk = m_pClientList[i]->m_iRating;
			m_pClientList[i]->m_iRating += atoi(cPK);
			SendNotifyMsg(0, i, DEF_NOTIFY_REPDGDEATHS, 0, 0, m_pClientList[i]->m_iRating, 0);
			char cTxt[250];
			wsprintf(cTxt, "Congratulations, you have received %d Reputation Points. (New Total: %d)", atoi(cPK), m_pClientList[i]->m_iRating);
			SendNotifyMsg(0, i, DEF_NOTIFY_IPACCOUNTINFO, 0, 0, 0, cTxt);

			wsprintf(notice, "%d Rep Points has been charged for player %s from %d to %d.", atoi(cPK), m_pClientList[i]->m_cCharName, oldpk, m_pClientList[i]->m_iRating);
			PutLogList(notice);
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, notice);
		}
	}
	delete pStrTok;
}

void CGame::AdminOrder_SetAdminLvl(int iClientH, char* pData, UINT32 dwMsgSize)
{
	char seps[] = "= \t\n";
	char* token, cBuff[256];
	class CStrTok* pStrTok;
	char* pk;
	char cNick[20];
	char cPK[5];
	char notice[100];
	int oldpk;
	int i;
	if (m_pClientList[iClientH] == 0)
		return;
	if ((dwMsgSize) <= 0)
		return;
	if (m_pClientList[iClientH]->m_iAdminUserLevel <= 3) {
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_ADMINUSERLEVELLOW, 0, 0,	0, 0);
		return;
	}
	ZeroMemory(cBuff, sizeof(cBuff));
	memcpy(cBuff, pData, dwMsgSize);
	pStrTok = new class CStrTok(cBuff, seps);
	token = pStrTok->pGet();
	token = pStrTok->pGet();
	if (token == 0)
	{
		delete pStrTok;
		return;
	}
	strcpy(cNick, token);
	pk = pStrTok->pGet();
	if (pk == 0)
	{
		delete pStrTok;
		return;
	}
	
	strcpy(cPK, pk);

	for (i = 0; i < DEF_MAXCLIENTS; i++)
	{
		if ((m_pClientList[i] != 0)
			&& (memcmp(m_pClientList[i]->m_cCharName, cNick, strlen(cNick)) == 0))
		{
			if (m_pClientList[i]->m_iAdminUserLevel == 0)
			{
				delete pStrTok;
				return;
			}

			oldpk = m_pClientList[i]->m_iAdminUserLevel;

			if (atoi(cPK) > 4)
				m_pClientList[i]->m_iAdminUserLevel = 4;
			else m_pClientList[i]->m_iAdminUserLevel = atoi(cPK);

			wsprintf(notice, "Admin User Level has been changed for player %s from %d to %d.", m_pClientList[i]->m_cCharName, oldpk, atoi(cPK));
			PutLogList(notice);
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, notice);
		}
	}
	delete pStrTok;
}

void CGame::RequestSetTrapHandler(int iClientH, char* pData)
{
	char* cp, cMagicID[3];
	int iItemID = -1;

	cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);
	cMagicID[0] = *cp;
	cp++;
	cMagicID[1] = *cp;
	cp++;
	cMagicID[2] = *cp;
	cp++;

	if (cMagicID[0] == 0 && cMagicID[1] == 0 && cMagicID[2] == 0) return;

	for (int i = 0; i < DEF_MAXITEMS; i++)
		if (m_pClientList[iClientH]->m_pItemList[i] != 0 && m_pClientList[iClientH]->m_pItemList[i]->m_sIDnum == 1032) {
			iItemID = i;
			break;
		}

	if (iItemID != -1 && (cMagicID[0] == 0 || m_pMagicConfigList[cMagicID[0] - 1] != 0) && (cMagicID[1] == 0 || m_pMagicConfigList[cMagicID[1] - 1] != 0) && (cMagicID[2] == 0 || m_pMagicConfigList[cMagicID[2] - 1] != 0)) {
		if (iAddDynamicObjectList(iClientH, DEF_OWNERTYPE_PLAYER, DEF_DYNAMICOBJECT_MAGICTRAP, m_pClientList[iClientH]->m_cMapIndex, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, 1000 * 60 * 10, cMagicID[0], cMagicID[1], cMagicID[2]) == 0) return;

		m_pClientList[iClientH]->m_pItemList[iItemID]->m_dwCount--;
		if (m_pClientList[iClientH]->m_pItemList[iItemID]->m_dwCount <= 0) ItemDepleteHandler(iClientH, iItemID, false, false);
	}
}

void CGame::NotifyEvents()
{
	UINT32* dwp;
	UINT16* wp;
	char* cp;
	bool* bp;

	auto pBuffer = new char[DEF_MSGBUFFERSIZE + 1];
	ZeroMemory(pBuffer, DEF_MSGBUFFERSIZE + 1);

	dwp = (UINT32*)(pBuffer + DEF_INDEX4_MSGID);
	*dwp = MSGID_EVENTS;
	wp = (UINT16*)(pBuffer + DEF_INDEX2_MSGTYPE);
	*wp = DEF_MSGTYPE_CONFIRM;

	cp = (char*)(pBuffer + DEF_INDEX2_MSGTYPE + 2);

	bp = (bool*)cp;
	*bp = _candy_boost;
	cp ++;

	bp = (bool*)cp;
	*bp = _revelation;
	cp ++;

	bp = (bool*)cp;
	*bp = _city_teleport;
	cp ++;

	bp = (bool*)cp;
	*bp = _drop_inhib;
	cp ++;

	bp = (bool*)cp;
	*bp = c_team->bteam;
	cp++;

	bp = (bool*)cp;
	*bp = bShinning;
	cp++;

	bp = (bool*)cp;
	*bp = m_bHappyHour;
	cp++;

	bp = (bool*)cp;
	*bp = m_bFuryHour;
	cp++;

	for (int iClientH = 0; iClientH < DEF_MAXCLIENTS; iClientH++)
	{
		if (!m_pClientList[iClientH])
			continue;

		auto iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(pBuffer, 14);
		switch (iRet) {
		case DEF_XSOCKEVENT_QUENEFULL:
		case DEF_XSOCKEVENT_SOCKETERROR:
		case DEF_XSOCKEVENT_CRITICALERROR:
		case DEF_XSOCKEVENT_SOCKETCLOSED:
			DeleteClient(iClientH, true, true, true, false); // added true (4)
			PutLogList("HERE 1");
			if (pBuffer != 0) delete[] pBuffer;
			return;
		}
	}


	if (pBuffer != 0) delete[] pBuffer;
}

void CGame::SendAlertMsg(int client, char* pMsg)
{
	auto p = m_pClientList[client];
	if (!p) return;
	auto cl = client;
	char message[200];
	ZeroMemory(message, sizeof(message));
	strcpy(message, pMsg);
	SendNotifyMsg(0, client, NOTIFYMSG, 0, 0, 0, message);
}

void CGame::ManageShinning()
{
	if (bShinning) bShinning = false;
	else bShinning = true;

	if (bShinning)
	{
		for (int i = 0; i < DEF_MAXCLIENTS; i++)
		{
			auto pi = m_pClientList[i];
			if (!pi) continue;
			SendAlertMsg(i, "Event Shinning Enabled");
		}
		dwEventFinishTime = timeGetTime() + m_sShinningFinish * 60 * 1000;
	}
	else
	{
		for (int i = 0; i < DEF_MAXCLIENTS; i++)
		{
			auto pi = m_pClientList[i];
			if (!pi) continue;
			SendAlertMsg(i, "Event Shinning Disabled");
		}
		dwEventFinishTime = 0;
	}
	NotifyEvents();
}

void CGame::ManageHappyHour()
{
	if (m_bFuryHour) m_bFuryHour = false;
	if (m_bHappyHour) m_bHappyHour = false;
	else m_bHappyHour = true;

	if (m_bHappyHour)
	{
		for (int i = 0; i < DEF_MAXCLIENTS; i++)
		{
			auto pi = m_pClientList[i];
			if (!pi) continue;
			SendAlertMsg(i, "Happy Hour Activated! Ek x40");
		}
		dwEventFinishTime = timeGetTime() + 60 * 60 * 1000;
	}
	else
	{
		for (int i = 0; i < DEF_MAXCLIENTS; i++)
		{
			auto pi = m_pClientList[i];
			if (!pi) continue;
			SendAlertMsg(i, "Happy Hour Ended");
		}
		dwEventFinishTime = 0;
	}
	NotifyEvents();
}

void CGame::ManageFuryHour()
{
	if (m_bHappyHour) m_bHappyHour = false;
	if (m_bFuryHour) m_bFuryHour = false;
	else m_bFuryHour = true;

	if (m_bFuryHour)
	{
		for (int i = 0; i < DEF_MAXCLIENTS; i++)
		{
			auto pi = m_pClientList[i];
			if (!pi) continue;
			SendAlertMsg(i, "Fury Hour Activated! Ek x100");
		}
		dwEventFinishTime = timeGetTime() + 60 * 60 * 1000;
	}
	else
	{
		for (int i = 0; i < DEF_MAXCLIENTS; i++)
		{
			auto pi = m_pClientList[i];
			if (!pi) continue;
			SendAlertMsg(i, "Fury Hour Ended");
		}
		dwEventFinishTime = 0;
	}
	NotifyEvents();
}

void CGame::minimap_clear(int client)
{
	char cData[56];
	UINT32* dwp;
	UINT16* wp;
	char* cp;
	int* ip;
	auto p = m_pClientList[client];
	if (!p) return;

	if (bShinning || (p->m_iStatus & 0x80000) != 0)
	{
		if (p->IsInMap("elvine") ||
			p->IsInMap("aresden") ||
			p->IsInMap("middleland") ||
			p->IsInMap("2ndmiddle") ||
			p->IsInMap("middled1n") ||
			p->IsInMap("icebound"))
		{
			dwp = (UINT32*)(cData + DEF_INDEX4_MSGID);
			*dwp = MSGID_NOTIFY;
			wp = (UINT16*)(cData + DEF_INDEX2_MSGTYPE);

			// centu - flag holder on minimap
			if (m_bIsCTFEvent)
			{
				if (p->IsLocation("aresden"))
				{
					*wp = MINIMAPBLUE_CLEAR;
				}
				else
				{
					*wp = MINIMAPRED_CLEAR;
				}
			}
			else
			{
				if (p->IsLocation("elvine"))
				{
					*wp = MINIMAPBLUE_CLEAR;
				}
				else
				{
					*wp = MINIMAPRED_CLEAR;
				}
			}

			cp = (char*)(cData + DEF_INDEX2_MSGTYPE + 2);
			ip = (int*)cp;
			*ip = client;
			cp += 4;

			for (int i = 0; i < DEF_MAXCLIENTS; i++)
			{
				auto pi = m_pClientList[i];

				if (!pi) continue;

				if (pi == p) continue;

				if (pi->m_cMapIndex == -1 || pi->m_cMapIndex != p->m_cMapIndex)	continue;

				pi->m_pXSock->iSendMsg(cData, 10);
			}
		}
	}

	if (!bShinning && p->m_iGuildRank != -1)
	{
		if (!p->IsInMap("cityhall_1") && !p->IsInMap("cityhall_2") && !p->IsInMap("arejail") && !p->IsInMap("elvjail") && !p->IsInMap("default"))
		{
			dwp = (UINT32*)(cData + DEF_INDEX4_MSGID);
			*dwp = MSGID_NOTIFY;
			wp = (UINT16*)(cData + DEF_INDEX2_MSGTYPE);

			*wp = MINIMAPGREEN_CLEAR;

			cp = (char*)(cData + DEF_INDEX2_MSGTYPE + 2);
			ip = (int*)cp;
			*ip = client;
			cp += 4;

			for (int i = 0; i < DEF_MAXCLIENTS; i++)
			{
				auto pi = m_pClientList[i];

				if (!pi) continue;

				if (pi == p) continue;

				if (pi->m_cMapIndex == -1 || pi->m_cMapIndex != p->m_cMapIndex)	continue;

				if (strcmp(pi->m_cGuildName, "NONE") == 0) continue;
				if (strcmp(pi->m_cGuildName, p->m_cGuildName) != 0) continue;

				pi->m_pXSock->iSendMsg(cData, 10);
			}
		}
	}
}

void CGame::minimap_update(int client)
{
	char cData[56];
	UINT32* dwp;
	UINT16* wp;
	char* cp;
	int* ip;
	short* sp;
	auto p = m_pClientList[client];
	if (!p) return;

	if (bShinning || (p->m_iStatus & 0x80000) != 0)
	{
		if (p->IsInMap("elvine") ||
			p->IsInMap("aresden") ||
			p->IsInMap("middleland") ||
			p->IsInMap("2ndmiddle") ||
			p->IsInMap("middled1n") ||
			p->IsInMap("icebound"))
		{
			dwp = (UINT32*)(cData + DEF_INDEX4_MSGID);
			*dwp = MSGID_NOTIFY;
			wp = (UINT16*)(cData + DEF_INDEX2_MSGTYPE);

			// centu - flag holder on minimap
			if (m_bIsCTFEvent)
			{
				if (p->IsLocation("aresden"))
				{
					*wp = MINIMAPBLUE_UPDATE;
				}
				else
				{
					*wp = MINIMAPRED_UPDATE;
				}
			}
			else
			{
				if (p->IsLocation("elvine"))
				{
					*wp = MINIMAPBLUE_UPDATE;
				}
				else
				{
					*wp = MINIMAPRED_UPDATE;
				}
			}

			cp = (char*)(cData + DEF_INDEX2_MSGTYPE + 2);
			ip = (int*)cp;
			*ip = client;
			cp += 4;

			sp = (short*)cp;
			*sp = p->m_sX;
			cp += 2;
			sp = (short*)cp;
			*sp = p->m_sY;
			cp += 2;

			for (int i = 0; i < DEF_MAXCLIENTS; i++)
			{
				auto pi = m_pClientList[i];

				if (!pi) continue;

				if (pi == p) continue;

				if (pi->m_cMapIndex == -1 || pi->m_cMapIndex != p->m_cMapIndex)	continue;

				pi->m_pXSock->iSendMsg(cData, 14);
			}
		}
	}

	if (!bShinning && p->m_iGuildRank != -1)
	{
		if (!p->IsInMap("cityhall_1") && !p->IsInMap("cityhall_2") && !p->IsInMap("arejail") && !p->IsInMap("elvjail") && !p->IsInMap("default"))
		{
			dwp = (UINT32*)(cData + DEF_INDEX4_MSGID);
			*dwp = MSGID_NOTIFY;
			wp = (UINT16*)(cData + DEF_INDEX2_MSGTYPE);

			*wp = MINIMAPGREEN_UPDATE;

			cp = (char*)(cData + DEF_INDEX2_MSGTYPE + 2);
			ip = (int*)cp;
			*ip = client;
			cp += 4;

			sp = (short*)cp;
			*sp = p->m_sX;
			cp += 2;
			sp = (short*)cp;
			*sp = p->m_sY;
			cp += 2;

			for (int i = 0; i < DEF_MAXCLIENTS; i++)
			{
				auto pi = m_pClientList[i];

				if (!pi) continue;

				if (pi == p) continue;

				if (pi->m_cMapIndex == -1 || pi->m_cMapIndex != p->m_cMapIndex)	continue;

				if (strcmp(pi->m_cGuildName, "NONE") == 0) continue;
				if (strcmp(pi->m_cGuildName, p->m_cGuildName) != 0) continue;
				
				pi->m_pXSock->iSendMsg(cData, 14);
			}
		}
	}
}

//Magn0S:: Set Map Restriction notification
void CGame::NotifyMapRestrictions(int iClientH, bool bNotify)
{
	UINT32* dwp;
	UINT16* wp;
	char* cp;
	bool* bp;

	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
	if (m_pClientList[iClientH]->m_bIsKilled == true) return;

	//Magn0S:: For specifics restrictions, update cliente.
	if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bMapIllusion == true) {
		bNotify = true;
	}

	auto pBuffer = new char[DEF_MSGBUFFERSIZE + 1];
	ZeroMemory(pBuffer, DEF_MSGBUFFERSIZE + 1);

	dwp = (UINT32*)(pBuffer + DEF_INDEX4_MSGID);
	*dwp = MSGID_MAP_RESTRI;
	wp = (UINT16*)(pBuffer + DEF_INDEX2_MSGTYPE);
	*wp = DEF_MSGTYPE_CONFIRM;

	cp = (char*)(pBuffer + DEF_INDEX2_MSGTYPE + 2);

	bp = (bool*)cp;
	*bp = bNotify;
	cp++;

	bp = (bool*)cp;
	*bp = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsAttackEnabled;
	cp++;

	bp = (bool*)cp;
	*bp = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bMapTP;
	cp++;

	bp = (bool*)cp;
	*bp = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bMapParty;
	cp++;

	bp = (bool*)cp;
	*bp = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bMapIllusion;
	cp++;

	bp = (bool*)cp;
	*bp = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bMapActivate;
	cp++;

	bp = (bool*)cp;
	*bp = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bMapInvi;
	cp++;

	bp = (bool*)cp;
	*bp = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bMapAMP;
	cp++;

	bp = (bool*)cp;
	*bp = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bMapRegens;
	cp++;

	bp = (bool*)cp;
	*bp = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bMapHideEnemy;
	cp++;

	bp = (bool*)cp;
	*bp = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bMapBonusDmg;
	cp++;

	bp = (bool*)cp;
	*bp = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bMapEquip;
	cp++;

	auto iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(pBuffer, 18);
	switch (iRet) {
	case DEF_XSOCKEVENT_QUENEFULL:
	case DEF_XSOCKEVENT_SOCKETERROR:
	case DEF_XSOCKEVENT_CRITICALERROR:
	case DEF_XSOCKEVENT_SOCKETCLOSED:
		DeleteClient(iClientH, true, true, true, false); // added true (4)
		PutLogList("HERE 2");
		if (pBuffer != 0) delete[] pBuffer;
		return;
	}

	if (pBuffer != 0) delete[] pBuffer;
}

// Capture the Flag
void CGame::UpdateEventStatus()
{
	char* cp, cData[255], cTotal = 0;
	int iSize = 0, i, iRet;
	
	UINT32* dwp;
	UINT16* wp;

	ZeroMemory(cData, sizeof(cData));

	dwp = (UINT32*)(cData + DEF_INDEX4_MSGID);
	*dwp = MSGID_NOTIFY;
	wp = (UINT16*)(cData + DEF_INDEX2_MSGTYPE);
	*wp = DEF_NOTIFY_EVENT_INFO;

	cp = (char*)(cData + DEF_INDEX2_MSGTYPE + 2);

	*cp = m_bIsCTFEvent;
	cp++;
	iSize++;

	*cp = m_cCFTEventCount[0];
	cp++;
	iSize++;

	*cp = m_cCFTEventCount[1];
	cp++;
	iSize++;

	for (i = 1; i < DEF_MAXCLIENTS; i++) {
		if (m_pClientList[i] != 0 && m_pClientList[i]->m_cSide != 0) {
			iRet = m_pClientList[i]->m_pXSock->iSendMsg(cData, 6 + iSize);
			switch (iRet) {
			case DEF_XSOCKEVENT_QUENEFULL:
			case DEF_XSOCKEVENT_SOCKETERROR:
			case DEF_XSOCKEVENT_CRITICALERROR:
			case DEF_XSOCKEVENT_SOCKETCLOSED:
				DeleteClient(i, true, true);
				break;
			}
		}
	}
}

void CGame::AdminOrder_SetEvent()
{
	int i;
	UINT32 dwTime = timeGetTime();

	if (m_bIsCTFEvent)
		m_bIsCTFEvent = false;
	else m_bIsCTFEvent = true;

	if (m_bIsCTFEvent) {
		for (i = 1; i < DEF_MAXCLIENTS; i++) {// Check all clients
			if (m_pClientList[i] != 0) {
				SendNotifyMsg(0, i, DEF_NOTIFY_EVENT, 3, m_bIsCTFEvent, 0, 0);
			}
		}

		m_iCTFEventFlagHolder[0] = m_iCTFEventFlagHolder[1] = -1;
		m_cCFTEventCount[0] = m_cCFTEventCount[1] = 0;

		m_iCTFEventFlag[0] = iAddDynamicObjectList(0, 0, DEF_DYNAMICOBJECT_ARESDENFLAG2, m_iAresdenMapIndex, 151, 128, 0, 0);
		m_iCTFEventFlag[1] = iAddDynamicObjectList(0, 0, DEF_DYNAMICOBJECT_ELVINEFLAG2, m_iElvineMapIndex, 151, 132, 0, 0);
		
		dwEventFinishTime = timeGetTime() + m_sCTF_Finish * 60 * 1000;

		// Centu - can't stay in flag position
		m_pMapList[m_iAresdenMapIndex]->SetStayAllowedFlag(151, 128, false);
		m_pMapList[m_iElvineMapIndex]->SetStayAllowedFlag(151, 132, false);
	}
	else {
		for (i = 0; i < 2; i++) {
			if (m_iCTFEventFlag[i] != 0 && m_pDynamicObjectList[m_iCTFEventFlag[i]] != 0) {
				SendEventToNearClient_TypeB(MSGID_DYNAMICOBJECT, DEF_MSGTYPE_REJECT, m_pDynamicObjectList[m_iCTFEventFlag[i]]->m_cMapIndex, m_pDynamicObjectList[m_iCTFEventFlag[i]]->m_sX, m_pDynamicObjectList[m_iCTFEventFlag[i]]->m_sY, m_pDynamicObjectList[m_iCTFEventFlag[i]]->m_sType, m_iCTFEventFlag[i], 0, (short)0);

				m_pMapList[m_pDynamicObjectList[m_iCTFEventFlag[i]]->m_cMapIndex]->SetDynamicObject(0, 0, m_pDynamicObjectList[m_iCTFEventFlag[i]]->m_sX, m_pDynamicObjectList[m_iCTFEventFlag[i]]->m_sY, dwTime);

				delete m_pDynamicObjectList[m_iCTFEventFlag[i]];
				m_pDynamicObjectList[m_iCTFEventFlag[i]] = 0;
				m_iCTFEventFlag[i] = 0;
			}
		}

		dwEventFinishTime = 0;

		for (i = 1; i < DEF_MAXCLIENTS; i++) {// Check all clients
			if (m_pClientList[i] != 0) {
				SendNotifyMsg(0, i, DEF_NOTIFY_EVENT, 0, 3, 0, 0);
			}
		}
	}

	UpdateEventStatus();
}

//Magn0S:: 04/2013 Code for fun, start add fragile items systems
void CGame::CheckDestroyFragileItem(int iClientH)
{
	CItem* item;
	SYSTEMTIME SysTime;
	GetLocalTime(&SysTime);
	bool bFragile = false;

	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;

	for (int i = 0; i < DEF_MAXITEMS; i++) {
		item = m_pClientList[iClientH]->m_pItemList[i];
		if (!item) continue;

		if ((item->m_sNewEffect1 == DEF_FRAGILEITEM) && (item->m_sNewEffect2 != 0)) { // Quebra somente itens de tempo.
			
			if ((item->m_sNewEffect3 < SysTime.wMonth) && (item->m_sNewEffect4 <= SysTime.wYear)) {
				wsprintf(G_cTxt, "Fragile Destroy - Char:(%s): (1) - Item:(%s)", m_pClientList[iClientH]->m_cCharName, item->m_cName);
				PutLogList(G_cTxt);
				ItemDepleteHandler(iClientH, i, false, true);
				bFragile = true;
			}
			else if ((item->m_sNewEffect2 < SysTime.wDay) && (item->m_sNewEffect3 <= SysTime.wMonth) && (item->m_sNewEffect4 <= SysTime.wYear)) {
				wsprintf(G_cTxt, "Fragile Destroy - Char:(%s): (2) - Item:(%s)", m_pClientList[iClientH]->m_cCharName, item->m_cName);
				PutLogList(G_cTxt);
				ItemDepleteHandler(iClientH, i, false, true);
				bFragile = true;
			}
		}
	}

	if (bFragile)
	{
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_IPACCOUNTINFO, 0, 0, 0, "Your Fragile item has vanished.");
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_FRAGILE, 0, 0, 0, 0);
	}
}

//Magn0S:: Set Map Restriction notification
void CGame::NotifyPlayerAttributes(int iClientH)
{
	UINT32* dwp;
	UINT16* wp;
	char* cp;
	int* ip, i;
	i = 0;

	auto player = m_pClientList[iClientH];

	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;

	auto pBuffer = new char[DEF_MSGBUFFERSIZE + 1];
	ZeroMemory(pBuffer, DEF_MSGBUFFERSIZE + 1);

	dwp = (UINT32*)(pBuffer + DEF_INDEX4_MSGID);
	*dwp = MSGID_PLAYER_ATTR;
	wp = (UINT16*)(pBuffer + DEF_INDEX2_MSGTYPE);
	*wp = DEF_MSGTYPE_CONFIRM;

	cp = (char*)(pBuffer + DEF_INDEX2_MSGTYPE + 2);

	ip = (int*)cp;
	*ip = player->m_iDefenseRatio + player->m_iAddDR;
	cp += 4;
	i += 4;

	ip = (int*)cp;
	*ip = player->m_iHitRatio + player->m_iAddAR + player->m_iMagicHitRatio_ItemEffect;
	cp += 4;
	i += 4;

	ip = (int*)cp;
	*ip = player->m_iAddResistMagic + player->m_iAddMR;
	cp += 4;
	i += 4;

	ip = (int*)cp;
	*ip = player->m_iAddHP;
	cp += 4;
	i += 4;

	ip = (int*)cp;
	*ip = player->m_iAddMP;
	cp += 4;
	i += 4;

	ip = (int*)cp;
	*ip = player->m_iManaSaveRatio;
	cp += 4;
	i += 4;
	//-------------------------------------------
	ip = (int*)cp;
	*ip = player->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_BODY] + player->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_LEGGINGS] + player->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_HEAD] + player->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_ARMS] + player->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_BACK] + player->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_BOOTS] + player->m_iDamageAbsorption_Shield; //player->m_iAddAbsPD;
	cp += 4;
	i += 4;

	ip = (int*)cp;
	*ip = player->m_iAddAbsMD;
	cp += 4;
	i += 4;

	ip = (int*)cp;
	*ip = player->m_iAddAbsLight;
	cp += 4;
	i += 4;

	ip = (int*)cp;
	*ip = player->m_iAddAbsEarth;
	cp += 4;
	i += 4;

	ip = (int*)cp;
	*ip = player->m_iAddAbsFire;
	cp += 4;
	i += 4;

	ip = (int*)cp;
	*ip = player->m_iAddAbsWater;
	cp += 4;
	i += 4;

	ip = (int*)cp;
	*ip = player->m_iAddAbsHoly;
	cp += 4;
	i += 4;

	ip = (int*)cp;
	*ip = player->m_iAddAbsUnholy;
	cp += 4;
	i += 4;
	//-------------------------------------------
	ip = (int*)cp;
	*ip = player->m_iAddPhysicalDamage;
	cp += 4;
	i += 4;

	ip = (int*)cp;
	*ip = player->m_iAddMagicalDamage;
	cp += 4;
	i += 4;
	//-------------------------------------------
	ip = (int*)cp;
	*ip = player->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_BODY];
	cp += 4;
	i += 4;

	ip = (int*)cp;
	*ip = player->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_BOOTS] + player->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_LEGGINGS];
	cp += 4;
	i += 4;

	ip = (int*)cp;
	*ip = player->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_ARMS];
	cp += 4;
	i += 4;

	ip = (int*)cp;
	*ip = player->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_HEAD];
	cp += 4;
	i += 4;

	ip = (int*)cp;
	*ip = player->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_BACK];
	cp += 4;
	i += 4;

	ip = (int*)cp;
	*ip = player->m_iDamageAbsorption_Shield;
	cp += 4;
	i += 4;

	ip = (int*)cp;
	*ip = player->m_iAddSP;
	cp += 4;
	i += 4;

	ip = (int*)cp;
	*ip = player->m_iAddPR;
	cp += 4;
	i += 4;
	//-------------------------------------------

	auto iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(pBuffer, 6 + i);
	switch (iRet) {
	case DEF_XSOCKEVENT_QUENEFULL:
	case DEF_XSOCKEVENT_SOCKETERROR:
	case DEF_XSOCKEVENT_CRITICALERROR:
	case DEF_XSOCKEVENT_SOCKETCLOSED:
		DeleteClient(iClientH, true, true, true, false); // added true (4)
		PutLogList("HERE 4A");
		if (pBuffer != 0) delete[] pBuffer;
		return;
	}

	if (pBuffer != 0) delete[] pBuffer;
}

void CGame::CityTeleport()
{
	_city_teleport = !_city_teleport;
	NotifyEvents();

	CMap* ares = nullptr;
	CMap* elv = nullptr;

	for (int i = 0; i < DEF_MAXMAPS; i++)
	{
		if (!m_pMapList[i])
			continue;

		if (std::string(m_pMapList[i]->m_cName) == "elvine")
			elv = m_pMapList[i];
		else if (std::string(m_pMapList[i]->m_cName) == "aresden")
			ares = m_pMapList[i];
	}

	if (_city_teleport)
	{
		dwEventFinishTime = timeGetTime() + m_sCityTeleportFinish * 60 * 1000;
	}
	else
	{
		dwEventFinishTime = 0;
	}

	for (int i = 0; i < DEF_MAXTELEPORTLOC; i++)
	{
		if (_city_teleport)
		{
			if (elv->m_pTeleportLoc[i] &&
				(std::string(elv->m_pTeleportLoc[i]->m_cDestMapName) == "middled1n"))
			{
				strcpy(elv->m_pTeleportLoc[i]->m_cDestMapName, "aresden");
				elv->m_pTeleportLoc[i]->m_sDestX = 77;
				elv->m_pTeleportLoc[i]->m_sDestY = 208;
			}

			if (ares->m_pTeleportLoc[i] &&
				(std::string(ares->m_pTeleportLoc[i]->m_cDestMapName) == "middled1n"))
			{
				strcpy(ares->m_pTeleportLoc[i]->m_cDestMapName, "elvine");
				ares->m_pTeleportLoc[i]->m_sDestX = 257;
				ares->m_pTeleportLoc[i]->m_sDestY = 80;
			}
			
		}
		else
		{
			if (elv->m_pTeleportLoc[i] &&
				(std::string(elv->m_pTeleportLoc[i]->m_cDestMapName) == "aresden"))
			{
				strcpy(elv->m_pTeleportLoc[i]->m_cDestMapName, "middled1n");
				elv->m_pTeleportLoc[i]->m_sDestX = 34;
				elv->m_pTeleportLoc[i]->m_sDestY = 36;
			}

			if (ares->m_pTeleportLoc[i] &&
				(std::string(ares->m_pTeleportLoc[i]->m_cDestMapName) == "elvine"))
			{
				strcpy(ares->m_pTeleportLoc[i]->m_cDestMapName, "middled1n");
				ares->m_pTeleportLoc[i]->m_sDestX = 181;
				ares->m_pTeleportLoc[i]->m_sDestY = 124;
			}
			
		}
	}
}

// Centuu - new classes
void CGame::SetClass(int iClientH)
{
	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_iStr == 14) 
	{
		m_pClientList[iClientH]->m_iClass = 1; // War
	}
	else if (m_pClientList[iClientH]->m_iMag == 14)
	{
		m_pClientList[iClientH]->m_iClass = 2; // Mage
	}
	else if (m_pClientList[iClientH]->m_iCharisma == 14)
	{
		m_pClientList[iClientH]->m_iClass = 3; // Archer
	}
	
}

void CGame::ChangeClassHandler(int iClientH, char* pData, UINT32 dwMsgSize)
{
	char* cp, cTmpName[5];
	int   i, iClass, * ip, iStr = 0, iDex = 0, iVit = 0, iMag = 0, iChar = 0, iInt = 0;
	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
	if (m_pClientList[iClientH]->m_iGizonItemUpgradeLeft < 500) return;
	cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);
	ZeroMemory(cTmpName, sizeof(cTmpName));
	strcpy(cTmpName, cp);
	cp += 5;
	ip = (int*)cp;
	iClass = *ip; // 0x00 l a i
	cp += 4;
	m_pClientList[iClientH]->m_iClass = iClass;
	m_pClientList[iClientH]->m_iGizonItemUpgradeLeft -= 500;
	SendNotifyMsg(0, iClientH, DEF_NOTIFY_GIZONITEMUPGRADELEFT, m_pClientList[iClientH]->m_iGizonItemUpgradeLeft, iClass + 10, 0, 0);
	
	switch (iClass) {
	case 1: 
		m_pClientList[iClientH]->m_iStr = 14;
		m_pClientList[iClientH]->m_iDex = 14;
		m_pClientList[iClientH]->m_iVit = 12;
		m_pClientList[iClientH]->m_iInt = 10;
		m_pClientList[iClientH]->m_iMag = 10;
		m_pClientList[iClientH]->m_iCharisma = 10;
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, "You're now a Warrior!"); 
		break;
	case 2: 
		m_pClientList[iClientH]->m_iStr = 10;
		m_pClientList[iClientH]->m_iDex = 10;
		m_pClientList[iClientH]->m_iVit = 12;
		m_pClientList[iClientH]->m_iInt = 14;
		m_pClientList[iClientH]->m_iMag = 14;
		m_pClientList[iClientH]->m_iCharisma = 10;
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, "You're now a Magician!"); 
		break;
	case 3: 
		m_pClientList[iClientH]->m_iStr = 10;
		m_pClientList[iClientH]->m_iDex = 14;
		m_pClientList[iClientH]->m_iVit = 12;
		m_pClientList[iClientH]->m_iInt = 10;
		m_pClientList[iClientH]->m_iMag = 10;
		m_pClientList[iClientH]->m_iCharisma = 14;
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, "You're now an Archer!"); 
		break;
	}

	int iStats = (m_pClientList[iClientH]->m_iStr + m_pClientList[iClientH]->m_iDex + m_pClientList[iClientH]->m_iVit +
		m_pClientList[iClientH]->m_iInt + m_pClientList[iClientH]->m_iMag + m_pClientList[iClientH]->m_iCharisma);

	m_pClientList[iClientH]->m_iLU_Pool = m_pClientList[iClientH]->m_iLevel * 3 - (iStats - 70) - 3;

	for (i = 0; i < DEF_MAXMAGICTYPE; i++)
	{
		m_pClientList[iClientH]->m_cMagicMastery[i] = 0;
	}

	if (m_pClientList[iClientH]->m_iHP > iGetMaxHP(iClientH)) m_pClientList[iClientH]->m_iHP = iGetMaxHP(iClientH);
	if (m_pClientList[iClientH]->m_iMP > iGetMaxMP(iClientH)) m_pClientList[iClientH]->m_iMP = iGetMaxMP(iClientH);
	if (m_pClientList[iClientH]->m_iSP > iGetMaxSP(iClientH)) m_pClientList[iClientH]->m_iSP = iGetMaxSP(iClientH);

	SendNotifyMsg(0, iClientH, DEF_NOTIFY_HP, 0, 0, 0, 0);
	SendNotifyMsg(0, iClientH, DEF_NOTIFY_MP, 0, 0, 0, 0);
	SendNotifyMsg(0, iClientH, DEF_NOTIFY_SP, 0, 0, 0, 0);

	for (i = 0; i < DEF_MAXITEMS; i++)
	{
		if (m_pClientList[iClientH]->m_bIsItemEquipped[i])
		{
			ReleaseItemHandler(iClientH, i, true);
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iClientH]->m_pItemList[i]->m_cEquipPos, i, 0, 0);
		}
	}

	SendNotifyMsg(0, iClientH, DEF_NOTIFY_STATECHANGE_SUCCESS2, 0, 0, 0, 0);
}

void CGame::ResetStatsHandler(int iClientH, char* pData, UINT32 dwMsgSize)
{
	char* cp, cTmpName[5];
	int   i, iClass, * ip, iStr = 0, iDex = 0, iVit = 0, iMag = 0, iChar = 0, iInt = 0;
	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
	if (m_pClientList[iClientH]->m_iGizonItemUpgradeLeft < 500) return;
	cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);
	ZeroMemory(cTmpName, sizeof(cTmpName));
	strcpy(cTmpName, cp);
	cp += 5;
	ip = (int*)cp;
	iClass = *ip; // 0x00 l a i
	cp += 4;
	
	m_pClientList[iClientH]->m_iGizonItemUpgradeLeft -= 300;
	SendNotifyMsg(0, iClientH, DEF_NOTIFY_GIZONITEMUPGRADELEFT, m_pClientList[iClientH]->m_iGizonItemUpgradeLeft, 0, 0, 0);

	switch (iClass) {
	case 1:
		m_pClientList[iClientH]->m_iStr = 14;
		m_pClientList[iClientH]->m_iDex = 14;
		m_pClientList[iClientH]->m_iVit = 12;
		m_pClientList[iClientH]->m_iInt = 10;
		m_pClientList[iClientH]->m_iMag = 10;
		m_pClientList[iClientH]->m_iCharisma = 10;
	
		break;
	case 2:
		m_pClientList[iClientH]->m_iStr = 10;
		m_pClientList[iClientH]->m_iDex = 10;
		m_pClientList[iClientH]->m_iVit = 12;
		m_pClientList[iClientH]->m_iInt = 14;
		m_pClientList[iClientH]->m_iMag = 14;
		m_pClientList[iClientH]->m_iCharisma = 10;
		
		break;
	case 3:
		m_pClientList[iClientH]->m_iStr = 10;
		m_pClientList[iClientH]->m_iDex = 14;
		m_pClientList[iClientH]->m_iVit = 12;
		m_pClientList[iClientH]->m_iInt = 10;
		m_pClientList[iClientH]->m_iMag = 10;
		m_pClientList[iClientH]->m_iCharisma = 14;
		
		break;
	}

	int iStats = (m_pClientList[iClientH]->m_iStr + m_pClientList[iClientH]->m_iDex + m_pClientList[iClientH]->m_iVit +
		m_pClientList[iClientH]->m_iInt + m_pClientList[iClientH]->m_iMag + m_pClientList[iClientH]->m_iCharisma);

	m_pClientList[iClientH]->m_iLU_Pool = m_pClientList[iClientH]->m_iLevel * 3 - (iStats - 70) - 3;

	for (i = 0; i < DEF_MAXMAGICTYPE; i++)
	{
		m_pClientList[iClientH]->m_cMagicMastery[i] = 0;
	}

	if (m_pClientList[iClientH]->m_iHP > iGetMaxHP(iClientH)) m_pClientList[iClientH]->m_iHP = iGetMaxHP(iClientH);
	if (m_pClientList[iClientH]->m_iMP > iGetMaxMP(iClientH)) m_pClientList[iClientH]->m_iMP = iGetMaxMP(iClientH);
	if (m_pClientList[iClientH]->m_iSP > iGetMaxSP(iClientH)) m_pClientList[iClientH]->m_iSP = iGetMaxSP(iClientH);

	SendNotifyMsg(0, iClientH, DEF_NOTIFY_HP, 0, 0, 0, 0);
	SendNotifyMsg(0, iClientH, DEF_NOTIFY_MP, 0, 0, 0, 0);
	SendNotifyMsg(0, iClientH, DEF_NOTIFY_SP, 0, 0, 0, 0);

	for (i = 0; i < DEF_MAXITEMS; i++)
	{
		if (m_pClientList[iClientH]->m_bIsItemEquipped[i])
		{
			ReleaseItemHandler(iClientH, i, true);
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iClientH]->m_pItemList[i]->m_cEquipPos, i, 0, 0);
		}
	}

	SendNotifyMsg(0, iClientH, DEF_NOTIFY_STATECHANGE_SUCCESS2, 0, 0, 0, 0);
}

void CGame::DeathmatchStarter()
{
	SYSTEMTIME SysTime;
	GetLocalTime(&SysTime);

	for (int i = 0; i < DEF_MAXSCHEDULE; i++)
		if ((m_stGladArenaSchedule[i].iDay == SysTime.wDayOfWeek) &&
			(m_stGladArenaSchedule[i].iHour == SysTime.wHour) &&
			(m_stGladArenaSchedule[i].iMinute == SysTime.wMinute)) {
			wsprintf(G_cTxt, "(!) Deathmatch Start : time(%d %d:%d), index(%d) schedule", m_stGladArenaSchedule[i].iDay, m_stGladArenaSchedule[i].iHour, m_stGladArenaSchedule[i].iMinute, i);
			PutLogList(G_cTxt);
			GlobalStartGladiatorArena();
			break;
		}
}

void CGame::CaptureTheFlagStarter()
{
	SYSTEMTIME SysTime;
	GetLocalTime(&SysTime);

	for (int i = 0; i < DEF_MAXSCHEDULE; i++)
		if ((m_stCTFSchedule[i].iDay == SysTime.wDayOfWeek) &&
			(m_stCTFSchedule[i].iHour == SysTime.wHour) &&
			(m_stCTFSchedule[i].iMinute == SysTime.wMinute)) {
			wsprintf(G_cTxt, "(!) Capture The Flag Start : time(%d %d:%d), index(%d) schedule", m_stCTFSchedule[i].iDay, m_stCTFSchedule[i].iHour, m_stCTFSchedule[i].iMinute, i);
			PutLogList(G_cTxt);
			AdminOrder_SetEvent();
			break;
		}
}

void CGame::InvasionStarter()
{
	SYSTEMTIME SysTime;
	GetLocalTime(&SysTime);

	for (int i = 0; i < DEF_MAXSCHEDULE; i++)
		if ((m_stInvasionSchedule[i].iDay == SysTime.wDayOfWeek) &&
			(m_stInvasionSchedule[i].iHour == SysTime.wHour) &&
			(m_stInvasionSchedule[i].iMinute == SysTime.wMinute)) {
			wsprintf(G_cTxt, "(!) Invasion Start : time(%d %d:%d), index(%d) schedule", m_stInvasionSchedule[i].iDay, m_stInvasionSchedule[i].iHour, m_stInvasionSchedule[i].iMinute, i);
			PutLogList(G_cTxt);
			CityTeleport();
			break;
		}
}

void CGame::CandyStarter()
{
	SYSTEMTIME SysTime;
	GetLocalTime(&SysTime);

	for (int i = 0; i < DEF_MAXSCHEDULE; i++)
		if ((m_stCandyFurySchedule[i].iDay == SysTime.wDayOfWeek) &&
			(m_stCandyFurySchedule[i].iHour == SysTime.wHour) &&
			(m_stCandyFurySchedule[i].iMinute == SysTime.wMinute)) {
			wsprintf(G_cTxt, "(!) Candy Fury Start : time(%d %d:%d), index(%d) schedule", m_stCandyFurySchedule[i].iDay, m_stCandyFurySchedule[i].iHour, m_stCandyFurySchedule[i].iMinute, i);
			PutLogList(G_cTxt);
			dwEventFinishTime = timeGetTime() + m_sCandyFinish * 60 * 1000;
			_candy_boost = true;
			NotifyEvents();
			break;
		}
}

void CGame::BeholderStarter()
{
	SYSTEMTIME SysTime;
	GetLocalTime(&SysTime);

	for (int i = 0; i < DEF_MAXSCHEDULE; i++)
		if ((m_stBeholderSchedule[i].iDay == SysTime.wDayOfWeek) &&
			(m_stBeholderSchedule[i].iHour == SysTime.wHour) &&
			(m_stBeholderSchedule[i].iMinute == SysTime.wMinute)) {
			wsprintf(G_cTxt, "(!) Beholder Start : time(%d %d:%d), index(%d) schedule", m_stBeholderSchedule[i].iDay, m_stBeholderSchedule[i].iHour, m_stBeholderSchedule[i].iMinute, i);
			PutLogList(G_cTxt);
			dwEventFinishTime = timeGetTime() + m_sRevelationFinish * 60 * 1000;
			_revelation = true;
			NotifyEvents();
			break;
		}
}

void CGame::BagProtectionStarter()
{
	SYSTEMTIME SysTime;
	GetLocalTime(&SysTime);

	for (int i = 0; i < DEF_MAXSCHEDULE; i++)
		if ((m_stBagProtectSchedule[i].iDay == SysTime.wDayOfWeek) &&
			(m_stBagProtectSchedule[i].iHour == SysTime.wHour) &&
			(m_stBagProtectSchedule[i].iMinute == SysTime.wMinute)) {
			wsprintf(G_cTxt, "(!) Bag Protection Start : time(%d %d:%d), index(%d) schedule", m_stBagProtectSchedule[i].iDay, m_stBagProtectSchedule[i].iHour, m_stBagProtectSchedule[i].iMinute, i);
			PutLogList(G_cTxt);
			dwEventFinishTime = timeGetTime() + m_sDropFinish * 60 * 1000;
			_drop_inhib = true;
			NotifyEvents();
			break;
		}
}

void CGame::TeamArenaStarter()
{
	SYSTEMTIME SysTime;
	GetLocalTime(&SysTime);

	for (int i = 0; i < DEF_MAXSCHEDULE; i++)
		if ((m_stTeamArenaSchedule[i].iDay == SysTime.wDayOfWeek) &&
			(m_stTeamArenaSchedule[i].iHour == SysTime.wHour) &&
			(m_stTeamArenaSchedule[i].iMinute == SysTime.wMinute)) {
			wsprintf(G_cTxt, "(!) Team Arena Start : time(%d %d:%d), index(%d) schedule", m_stTeamArenaSchedule[i].iDay, m_stTeamArenaSchedule[i].iHour, m_stTeamArenaSchedule[i].iMinute, i);
			PutLogList(G_cTxt);
			c_team->bteam = true;
			dwEventFinishTime = timeGetTime() + m_sTeamArenaFinish * 60 * 1000;
			c_team->EnableEvent();
			NotifyEvents();
			break;
		}
}

void CGame::ShinningStarter()
{
	SYSTEMTIME SysTime;
	GetLocalTime(&SysTime);

	for (int i = 0; i < DEF_MAXSCHEDULE; i++)
		if ((m_stShinningSchedule[i].iDay == SysTime.wDayOfWeek) &&
			(m_stShinningSchedule[i].iHour == SysTime.wHour) &&
			(m_stShinningSchedule[i].iMinute == SysTime.wMinute)) {
			wsprintf(G_cTxt, "(!) Shinning Start : time(%d %d:%d), index(%d) schedule", m_stShinningSchedule[i].iDay, m_stShinningSchedule[i].iHour, m_stShinningSchedule[i].iMinute, i);
			PutLogList(G_cTxt);
			ManageShinning();
			break;
		}
}

void CGame::HappyHourStarter()
{
	SYSTEMTIME SysTime;
	GetLocalTime(&SysTime);

	for (int i = 0; i < DEF_MAXSCHEDULE; i++)
		if ((m_stHappyHourSchedule[i].iDay == SysTime.wDayOfWeek) &&
			(m_stHappyHourSchedule[i].iHour == SysTime.wHour) &&
			(m_stHappyHourSchedule[i].iMinute == SysTime.wMinute)) {
			wsprintf(G_cTxt, "(!) Happy Hour Start : time(%d %d:%d), index(%d) schedule", m_stHappyHourSchedule[i].iDay, m_stHappyHourSchedule[i].iHour, m_stHappyHourSchedule[i].iMinute, i);
			PutLogList(G_cTxt);
			ManageHappyHour();
			break;
		}
}

void CGame::FuryHourStarter()
{
	SYSTEMTIME SysTime;
	GetLocalTime(&SysTime);

	for (int i = 0; i < DEF_MAXSCHEDULE; i++)
		if ((m_stFuryHourSchedule[i].iDay == SysTime.wDayOfWeek) &&
			(m_stFuryHourSchedule[i].iHour == SysTime.wHour) &&
			(m_stFuryHourSchedule[i].iMinute == SysTime.wMinute)) {
			wsprintf(G_cTxt, "(!) Fury Hour Start : time(%d %d:%d), index(%d) schedule", m_stFuryHourSchedule[i].iDay, m_stFuryHourSchedule[i].iHour, m_stFuryHourSchedule[i].iMinute, i);
			PutLogList(G_cTxt);
			ManageFuryHour();
			break;
		}
}