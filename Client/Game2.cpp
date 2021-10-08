// Game.cpp: implementation of the CGame class.
//
//////////////////////////////////////////////////////////////////////

#include "Game.h"
#include "lan_eng.h"

extern HWND	G_hWnd;

// This msg is sent by server when lvl-up
void CGame::NotifyMsg_LevelUp(char * pData)
{
	char * cp;
	int  * ip;
	int i, iPrevLevel;
	char cTxt[120];

	iPrevLevel = m_iLevel;

	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

	ip = (int *)cp;
	m_iLevel = *ip;
	cp += 4;

	ip = (int *)cp;
	m_iStr = *ip;
	cp += 4;

	ip = (int *)cp;
	m_iVit = *ip;
	cp += 4;

	ip = (int *)cp;
	m_iDex = *ip;
	cp += 4;

	ip = (int *)cp;
	m_iInt = *ip;
	cp += 4;

	ip = (int *)cp;
	m_iMag = *ip;
	cp += 4;

	ip = (int *)cp;
	m_iCharisma = *ip;
	cp += 4;

	// CLEROTH - LU
	m_iLU_Point = m_iLevel * 3 - ((m_iStr + m_iVit + m_iDex + m_iInt + m_iMag + m_iCharisma) - 70) - 3;
	m_cLU_Str = m_cLU_Vit = m_cLU_Dex = m_cLU_Int = m_cLU_Mag = m_cLU_Char = 0;

	wsprintf(cTxt, NOTIFYMSG_LEVELUP1, m_iLevel);// "Level up!!! Level %d!"
	AddEventList(cTxt, 10);

	switch (m_sPlayerType) {
	case 1:
	case 2:
	case 3:
		PlaySound('C', 21, 0);
		break;

	case 4:
	case 5:
	case 6:
		PlaySound('C', 22, 0);
		break;
	}

	_RemoveChatMsgListByObjectID(m_sPlayerObjectID);

	for (i = 1; i < DEF_MAXCHATMSGS; i++)
		if (m_pChatMsgList[i] == 0) {
			ZeroMemory(cTxt, sizeof(cTxt));
			strcpy(cTxt, "Level up!");
			m_pChatMsgList[i] = new class CMsg(23, cTxt, m_dwCurTime);
			m_pChatMsgList[i]->m_iObjectID = m_sPlayerObjectID;

			if (m_pMapData->bSetChatMsgOwner(m_sPlayerObjectID, -10, -10, i) == false) {
				delete m_pChatMsgList[i];
				m_pChatMsgList[i] = 0;
			}
			return;
		}
}

// CLEROTH - LU
void CGame::NotifyMsg_SettingSuccess(char * pData)
{
	char * cp;
	int  * ip;
	int iPrevLevel;
	char cTxt[120];
	iPrevLevel = m_iLevel;
	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
	ip = (int *)cp;
	m_iLevel = *ip;
	cp += 4;
	ip = (int *)cp;
	m_iStr = *ip;
	cp += 4;
	ip = (int *)cp;
	m_iVit = *ip;
	cp += 4;
	ip = (int *)cp;
	m_iDex = *ip;
	cp += 4;
	ip = (int *)cp;
	m_iInt = *ip;
	cp += 4;
	ip = (int *)cp;
	m_iMag = *ip;
	cp += 4;
	ip = (int *)cp;
	m_iCharisma = *ip;
	cp += 4;
	wsprintf(cTxt, "Your stat has been changed.");
	AddEventList(cTxt, 10);
	// CLEROTH - LU
	m_iLU_Point = m_iLevel * 3 - ((m_iStr + m_iVit + m_iDex + m_iInt + m_iMag + m_iCharisma) - 70) - 3;
	m_cLU_Str = m_cLU_Vit = m_cLU_Dex = m_cLU_Int = m_cLU_Mag = m_cLU_Char = 0;
}

void CGame::NotifyMsg_MagicEffectOff(char * pData) // MORLA - Aca efectua para uras
{
	char * cp;
	UINT16 * wp;
	short  sMagicType, sMagicEffect;
	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
	wp = (UINT16 *)cp;
	sMagicType = (short)*wp;
	cp += 2;
	wp = (UINT16 *)cp;
	sMagicEffect = (short)*wp;
	cp += 2;
	switch (sMagicType) {
	case DEF_MAGICTYPE_PROTECT:
		switch (sMagicEffect) {
		case 1: // "Protection from arrows has vanished."
			AddEventList(NOTIFYMSG_MAGICEFFECT_OFF1, 10);
			bPfa = false;
			break;
		case 2:	// "Protection from magic has vanished."
			AddEventList(NOTIFYMSG_MAGICEFFECT_OFF2, 10);
			bPfm = false;
			break;
		case 3:	// "Defense shield effect has vanished."
		case 4:	// "Defense shield effect has vanished."
			AddEventList(NOTIFYMSG_MAGICEFFECT_OFF3, 10);
			bShield = false;
			break;
		case 5:	// "Absolute Magic Protection has been vanished."
			AddEventList(NOTIFYMSG_MAGICEFFECT_OFF14, 10);
			bPfm = false;
			break;
		}
		break;

	case DEF_MAGICTYPE_HOLDOBJECT:
		switch (sMagicEffect) {
		case 1:	// "Hold person magic effect has vanished."
			AddEventList(NOTIFYMSG_MAGICEFFECT_OFF4, 10);
			break;

		case 2:	// "Paralysis magic effect has vanished."
			AddEventList(NOTIFYMSG_MAGICEFFECT_OFF5, 10);
			break;
		}
		m_bParalyze = false;
		break;

	case DEF_MAGICTYPE_INVISIBILITY:
		switch (sMagicEffect) {
		case 1:	// "Invisibility magic effect has vanished."
			AddEventList(NOTIFYMSG_MAGICEFFECT_OFF6, 10);
			bInv = false;
			break;
		}
		break;

	case DEF_MAGICTYPE_CONFUSE:
		switch (sMagicEffect) {
		case 1:	// "Language confuse magic effect has vanished."
			AddEventList(NOTIFYMSG_MAGICEFFECT_OFF7, 10);
			break;
		case 2:	// "Confusion magic has vanished."
			AddEventList(NOTIFYMSG_MAGICEFFECT_OFF8, 10);
			m_bIsConfusion = false;
			break;
		case 3:	// "Illusion magic has vanished."
			AddEventList(NOTIFYMSG_MAGICEFFECT_OFF9, 10);
			m_iIlusionOwnerH = 0;
			break;
		case 4:	// "At last, you gather your senses." // snoopy
			AddEventList(NOTIFYMSG_MAGICEFFECT_OFF15, 10);
			m_bIllusionMVT = false;
			break;
		}
		break;

	case DEF_MAGICTYPE_POISON:
		if (m_bIsPoisoned) AddEventList(NOTIFYMSG_MAGICEFFECT_OFF10, 10);
		m_bIsPoisoned = false;
		break;

	case DEF_MAGICTYPE_BERSERK:
		switch (sMagicEffect) {
		case 1:
			AddEventList(NOTIFYMSG_MAGICEFFECT_OFF11, 10);
			bZerk = false;
			break;
		}
		break;

	case DEF_MAGICTYPE_POLYMORPH:
		switch (sMagicEffect) {
		case 1:
			AddEventList(NOTIFYMSG_MAGICEFFECT_OFF12, 10);
			m_bIsPolymorph = false;
			break;
		}
		break;

	case DEF_MAGICTYPE_ICE:
		AddEventList(NOTIFYMSG_MAGICEFFECT_OFF13, 10);
		bFreeze = false;
		break;
	}
}

void CGame::NotifyMsg_MagicEffectOn(char * pData)
{
	char * cp;
	UINT32 * dwp;
	UINT16 * wp;
	short  sMagicType, sMagicEffect, sOwnerH;
	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

	wp = (UINT16 *)cp;
	sMagicType = (short)*wp;
	cp += 2;

	dwp = (UINT32 *)cp;
	sMagicEffect = (short)*dwp;
	cp += 4;

	dwp = (UINT32 *)cp;
	sOwnerH = (short)*dwp;
	cp += 4;

	switch (sMagicType) {
	case DEF_MAGICTYPE_PROTECT:
		switch (sMagicEffect) {
		case 1: // "You are completely protected from arrows!"
			AddEventList(NOTIFYMSG_MAGICEFFECT_ON1, 10);
			bPfa = true;
			m_sPfa = 60;
			break;
		case 2: // "You are protected from magic!"
			AddEventList(NOTIFYMSG_MAGICEFFECT_ON2, 10);
			bPfm = true;
			m_sPfm = 60;
			break;
		case 3: // "Defense ratio increased by a magic shield!"
		case 4: // "Defense ratio increased by a magic shield!"
			AddEventList(NOTIFYMSG_MAGICEFFECT_ON3, 10);
			bShield = true;
			m_sShield = 60;
			break;
		case 5: // "You are completely protected from magic!"
			AddEventList(NOTIFYMSG_MAGICEFFECT_ON14, 10);
			bPfm = true;
			m_sPfm = 60;
			break;
		}
		break;

	case DEF_MAGICTYPE_HOLDOBJECT:
		switch (sMagicEffect) {
		case 1: // "You were bounded by a Hold Person spell! Unable to move!"
			AddEventList(NOTIFYMSG_MAGICEFFECT_ON4, 10);
			break;
		case 2: // "You were bounded by a Paralysis spell! Unable to move!"
			AddEventList(NOTIFYMSG_MAGICEFFECT_ON5, 10);
			break;
		}
		m_bParalyze = true;
		break;

	case DEF_MAGICTYPE_INVISIBILITY:
		switch (sMagicEffect) {
		case 1: // "You are now invisible, no one can see you!"
			AddEventList(NOTIFYMSG_MAGICEFFECT_ON6, 10);
			bInv = true;
			m_sInv = 60;
			break;
		}
		break;

	case DEF_MAGICTYPE_CONFUSE:
		switch (sMagicEffect) {
		case 1:	// Confuse Language "No one understands you because of language confusion magic!"
			AddEventList(NOTIFYMSG_MAGICEFFECT_ON7, 10);
			break;

		case 2: // Confusion "Confusion magic casted, impossible to determine player allegience."
			AddEventList(NOTIFYMSG_MAGICEFFECT_ON8, 10);
			m_bIsConfusion = true;
			break;

		case 3:	// Illusion "Illusion magic casted, impossible to tell who is who!"
			AddEventList(NOTIFYMSG_MAGICEFFECT_ON9, 10);
			_SetIlusionEffect(sOwnerH);
			break;

		case 4:	// IllusionMouvement "You are thrown into confusion, and you are flustered yourself." // snoopy
			AddEventList(NOTIFYMSG_MAGICEFFECT_ON15, 10);
			m_bIllusionMVT = true;
			break;
		}
		break;

	case DEF_MAGICTYPE_POISON:
		AddEventList(NOTIFYMSG_MAGICEFFECT_ON10, 10);
		m_bIsPoisoned = true;
		break;

	case DEF_MAGICTYPE_BERSERK:
		switch (sMagicEffect) {
		case 1:
			AddEventList(NOTIFYMSG_MAGICEFFECT_ON11, 10);
			bZerk = true;
			m_sZerk = 40;
			break;
		}
		break;

	case DEF_MAGICTYPE_POLYMORPH:
		switch (sMagicEffect) {
		case 1:
			AddEventList(NOTIFYMSG_MAGICEFFECT_ON12, 10);
			m_bIsPolymorph = true;
			break;
		}
		break;

	case DEF_MAGICTYPE_ICE:
		AddEventList(NOTIFYMSG_MAGICEFFECT_ON13, 10);
		bFreeze = true;
		break;
	}
}

void CGame::NotifyMsg_MagicStudyFail(char * pData)
{
	char * cp, cMagicNum, cName[31], cFailCode;
	char cTxt[120];
	int  * ip, iCost, iReqInt, iReqStr;
	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
	cFailCode = *cp;
	cp++;
	cMagicNum = *cp;
	cp++;
	ZeroMemory(cName, sizeof(cName));
	memcpy(cName, cp, 30);
	cp += 30;
	ip = (int *)cp;
	iCost = *ip;
	cp += 4;
	ip = (int *)cp;
	iReqInt = *ip;
	cp += 4;

	if (iCost > 0)
	{
		wsprintf(cTxt, NOTIFYMSG_MAGICSTUDY_FAIL1, cName);
		AddEventList(cTxt, 10);
	}
	else
	{
		wsprintf(cTxt, NOTIFYMSG_MAGICSTUDY_FAIL2, cName);
		AddEventList(cTxt, 10);
		wsprintf(cTxt, NOTIFYMSG_MAGICSTUDY_FAIL3, iReqInt);
		AddEventList(cTxt, 10);
	}
}

void CGame::NotifyMsg_MagicStudySuccess(char * pData)
{
	char * cp, cMagicNum, cName[31];
	char cTxt[120];
	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
	cMagicNum = *cp;
	cp++;
	m_cMagicMastery[cMagicNum] = 1;
	ZeroMemory(cName, sizeof(cName));
	memcpy(cName, cp, 30);
	wsprintf(cTxt, NOTIFYMSG_MAGICSTUDY_SUCCESS1, cName);
	AddEventList(cTxt, 10);
	PlaySound('E', 23, 0);
}

void CGame::NotifyMsg_MP(char * pData)
{
	int iPrevMP, *ip;
	char cTxt[120];
	iPrevMP = m_iMP;
	ip = (int *)(pData + DEF_INDEX2_MSGTYPE + 2);
	m_iMP = *ip;
	if (abs(m_iMP - iPrevMP) < 10) return;
	if (m_iMP > iPrevMP)
	{
		wsprintf(cTxt, NOTIFYMSG_MP_UP, m_iMP - iPrevMP);//"MP�
		AddEnergyList(cTxt, 3);
		PlaySound('E', 21, 0);
	}
	else
	{
		wsprintf(cTxt, NOTIFYMSG_MP_DOWN, iPrevMP - m_iMP);//"MP�
		AddEnergyList(cTxt, 3);
	}
}

void CGame::NotifyMsg_NewGuildsMan(char * pData)
{
	char * cp, cName[12], cTxt[120];
	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
	ZeroMemory(cName, sizeof(cName));
	memcpy(cName, cp, 10);
	wsprintf(cTxt, NOTIFYMSG_NEW_GUILDMAN1, cName);
	AddEventList(cTxt, 10);
	ClearGuildNameList();
}

void CGame::NotifyMsg_PKcaptured(char *pData)
{
	char  * cp;
	UINT32 * dwp;
	UINT16  * wp;
	int     iPKcount, iLevel, iRewardGold;
	char cTxt[120], cName[12];
	UINT32 iExp;
	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
	wp = (UINT16 *)cp;
	iPKcount = *wp;
	cp += 2;
	wp = (UINT16 *)cp;
	iLevel = *wp;
	cp += 2;
	ZeroMemory(cName, sizeof(cName));
	memcpy(cName, cp, 10);
	cp += 10;
	dwp = (UINT32 *)cp;
	iRewardGold = *dwp;
	cp += 4;
	dwp = (UINT32 *)cp;
	iExp = *dwp;
	cp += 4;
	wsprintf(cTxt, NOTIFYMSG_PK_CAPTURED1, iLevel, cName, iPKcount);
	AddEventList(cTxt, 10);
	
	wsprintf(cTxt, NOTIFYMSG_PK_CAPTURED3, iExp - m_iExp);
	AddEventList(cTxt, 10);
}

void CGame::NotifyMsg_PKpenalty(char *pData)
{
	char  * cp;
	UINT32 * dwp;
	int     iPKcount, iStr, iVit, iDex, iInt, iMag, iChr;
	UINT32 iExp;
	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
	dwp = (UINT32 *)cp;
	iExp = *dwp;
	cp += 4;
	dwp = (UINT32 *)cp;
	iStr = *dwp;
	cp += 4;
	dwp = (UINT32 *)cp;
	iVit = *dwp;
	cp += 4;
	dwp = (UINT32 *)cp;
	iDex = *dwp;
	cp += 4;
	dwp = (UINT32 *)cp;
	iInt = *dwp;
	cp += 4;
	dwp = (UINT32 *)cp;
	iMag = *dwp;
	cp += 4;
	dwp = (UINT32 *)cp;
	iChr = *dwp;
	cp += 4;
	dwp = (UINT32 *)cp;
	iPKcount = *dwp;
	cp += 4;
	wsprintf(G_cTxt, NOTIFYMSG_PK_PENALTY1, iPKcount - m_iPKCount);
	AddEventList(G_cTxt, 10);
	
	m_iExp = iExp;
	m_iStr = iStr;
	m_iVit = iVit;
	m_iDex = iDex;
	m_iInt = iInt;
	m_iMag = iMag;
	m_iCharisma = iChr;
	m_iPKCount = iPKcount;
}

void CGame::NotifyMsg_PlayerShutUp(char * pData)
{
	char * cp, cName[12];
	UINT16 * wp, wTime;
	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
	wp = (UINT16 *)cp;
	wTime = *wp;
	cp += 2;
	ZeroMemory(cName, sizeof(cName));
	memcpy(cName, cp, 10);
	cp += 10;
	if (memcmp(m_cPlayerName, cName, 10) == 0)
		wsprintf(G_cTxt, NOTIFYMSG_PLAYER_SHUTUP1, wTime);
	else wsprintf(G_cTxt, NOTIFYMSG_PLAYER_SHUTUP2, cName, wTime);

	AddEventList(G_cTxt, 10);
}

void CGame::NotifyMsg_PlayerStatus(bool bOnGame, char * pData)
{
	char cName[12], cMapName[12], *cp;
	UINT16 * wp;
	UINT16  dx = 1, dy = 1;
	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
	ZeroMemory(cName, sizeof(cName));
	memcpy(cName, cp, 10);
	cp += 10;
	ZeroMemory(cMapName, sizeof(cMapName));
	memcpy(cMapName, cp, 10);
	cp += 10;
	wp = (UINT16 *)cp;
	dx = (UINT16)*wp;
	cp += 2;
	wp = (UINT16 *)cp;
	dy = (UINT16)*wp;
	cp += 2;
	ZeroMemory(G_cTxt, sizeof(G_cTxt));
	if (bOnGame == true) {
		if (strlen(cMapName) == 0)
			wsprintf(G_cTxt, NOTIFYMSG_PLAYER_STATUS1, cName);
		else wsprintf(G_cTxt, NOTIFYMSG_PLAYER_STATUS2, cName, cMapName, dx, dy);
	}
	else wsprintf(G_cTxt, NOTIFYMSG_PLAYER_STATUS3, cName);
	AddEventList(G_cTxt, 10);
}


void CGame::NotifyMsg_QuestReward(char *pData)
{
	short * sp, sWho, sFlag;
	char  * cp, cRewardName[21], cTxt[120];
	int   * ip, iAmount, iIndex, iPreCon, iV5;
	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
	sp = (short *)cp;
	sWho = *sp;
	cp += 2;
	sp = (short *)cp;
	sFlag = *sp;
	cp += 2;
	ip = (int *)cp;
	iAmount = *ip;
	cp += 4;
	ZeroMemory(cRewardName, sizeof(cRewardName));
	memcpy(cRewardName, cp, 20);
	cp += 20;
	iPreCon = m_iContribution;
	ip = (int *)cp;
	m_iContribution = *ip;
	cp += 4;

	ip = (int*)cp;
	iV5 = *ip; // Magn0S:: Multi Quest
	cp += 4;

	if (sFlag == 1)
	{
		m_stQuest[iV5].sWho = 0;
		m_stQuest[iV5].sQuestType = 0;
		m_stQuest[iV5].sContribution = 0;
		m_stQuest[iV5].sTargetType = 0;
		m_stQuest[iV5].sTargetCount = 0;
		m_stQuest[iV5].sX = 0;
		m_stQuest[iV5].sY = 0;
		m_stQuest[iV5].sRange = 0;
		m_stQuest[iV5].sCurrentCount = 0;
		m_stQuest[iV5].bIsQuestCompleted = false;
		ZeroMemory(m_stQuest[iV5].cTargetName, sizeof(m_stQuest[iV5].cTargetName));
		EnableDialogBox(21, 0, sWho + 110, 0);
		iIndex = m_stDialogBoxInfo[21].sV1;
		m_pMsgTextList2[iIndex] = new class CMsg(0, "  ", 0);
		iIndex++;
		ZeroMemory(cTxt, sizeof(cTxt));
		if (memcmp(cRewardName, "Exp", 6) == 0)
		{
			if (iAmount > 0) wsprintf(cTxt, NOTIFYMSG_QUEST_REWARD1, iAmount);
		}
		else
		{
			wsprintf(cTxt, NOTIFYMSG_QUEST_REWARD2, iAmount, cRewardName);
		}
		m_pMsgTextList2[iIndex] = new class CMsg(0, cTxt, 0);
		iIndex++;
		m_pMsgTextList2[iIndex] = new class CMsg(0, "  ", 0);
		iIndex++;
		ZeroMemory(cTxt, sizeof(cTxt));
		if (iPreCon < m_iContribution)
			wsprintf(cTxt, NOTIFYMSG_QUEST_REWARD3, m_iContribution - iPreCon);
		else wsprintf(cTxt, NOTIFYMSG_QUEST_REWARD4, iPreCon - m_iContribution);

		m_pMsgTextList2[iIndex] = new class CMsg(0, "  ", 0);
		iIndex++;
	}
	else EnableDialogBox(21, 0, sWho + 120, 0);
}

void CGame::NotifyMsg_RatingPlayer(char * pData)
{
	int * ip;
	char * cp, cName[12];
	UINT16  cValue;
	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
	cValue = *cp;
	cp++;
	ZeroMemory(cName, sizeof(cName));
	memcpy(cName, cp, 10);
	cp += 10;
	ip = (int *)cp;
	m_iRating = *ip;
	cp += 4;
	ZeroMemory(G_cTxt, sizeof(G_cTxt));
	if (memcmp(m_cPlayerName, cName, 10) == 0)
	{
		if (cValue == 1)
		{
			wsprintf(G_cTxt, NOTIFYMSG_RATING_PLAYER1, m_iRating);
			PlaySound('E', 23, 0);
		}
	}
	else
	{
		if (cValue == 1)
			wsprintf(G_cTxt, NOTIFYMSG_RATING_PLAYER2, cName);
		else wsprintf(G_cTxt, NOTIFYMSG_RATING_PLAYER3, cName);
	}
	AddEventList(G_cTxt, 10);
}


void CGame::NotifyMsg_ServerChange(char * pData)
{
	char * cp, cWorldServerAddr[16];	//Snoopy: change names for better readability
	int * ip, iWorldServerPort;		//Snoopy: change names for better readability

	ZeroMemory(m_cMapName, sizeof(m_cMapName));
	ZeroMemory(m_cMapMessage, sizeof(m_cMapMessage));
	ZeroMemory(cWorldServerAddr, sizeof(cWorldServerAddr));

	// v1.43
	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
	memcpy(m_cMapName, cp, 10);

	cp += 10;

	memcpy(cWorldServerAddr, cp, 15);
	cp += 15;
	ip = (int *)cp;
	iWorldServerPort = *ip;
	cp += 4;
	if (m_pGSock != 0)
	{
		delete m_pGSock;
		m_pGSock = 0;
	}
	if (m_pLSock != 0)
	{
		delete m_pLSock;
		m_pLSock = 0;
	}
	m_pLSock = new class XSocket(m_hWnd, DEF_SOCKETBLOCKLIMIT);
	//GetIPByDNS();
	m_pLSock->bConnect(m_cLogServerAddr, iWorldServerPort, WM_USER_LOGSOCKETEVENT);
	m_pLSock->bInitBufferSize(30000);

	m_bIsPoisoned = false;
	ResetValues();

	ChangeGameMode(DEF_GAMEMODE_ONCONNECTING);
	m_dwConnectMode = MSGID_REQUEST_ENTERGAME;
	m_wEnterGameType = DEF_ENTERGAMEMSGTYPE_NEW_TOWLSBUTMLS;
	ZeroMemory(m_cMsg, sizeof(m_cMsg));
	strcpy(m_cMsg, "55");
}

void CGame::NotifyMsg_SetItemCount(char * pData)
{
	char  * cp;
	UINT16  * wp;
	UINT32 * dwp;
	short  sItemIndex;
	UINT32  dwCount;
	bool   bIsItemUseResponse;

	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

	wp = (UINT16 *)cp;
	sItemIndex = *wp;
	cp += 2;

	dwp = (UINT32 *)cp;
	dwCount = *dwp;
	cp += 4;

	bIsItemUseResponse = (bool)*cp;
	cp++;

	if (m_pItemList[sItemIndex] != 0)
	{
		m_pItemList[sItemIndex]->m_dwCount = dwCount;
		if (bIsItemUseResponse == true) m_bIsItemDisabled[sItemIndex] = false;
	}
}

void CGame::NotifyMsg_ShowMap(char * pData)
{
	char * cp;
	UINT16 * wp, w1, w2;
	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
	wp = (UINT16 *)cp;
	w1 = *wp;
	cp += 2;
	wp = (UINT16 *)cp;
	w2 = *wp;
	cp += 2;
	if (w2 == 0) AddEventList(NOTIFYMSG_SHOW_MAP1, 10);
	else EnableDialogBox(22, 0, w1, w2 - 1);
}

void CGame::NotifyMsg_Skill(char *pData)
{
	//UINT16 * wp; // centu - fix negative skills
	short sSkillIndex, sValue, *sp, sValue2;
	char * cp;
	char cTxt[120];
	int i;

	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
	
	sp = (short *)cp;
	sSkillIndex = *sp;
	cp += 2;

	sp = (short *)cp;
	sValue = *sp;
	cp += 2;

	sp = (short*)cp;
	sValue2 = *sp;
	cp += 2;
	
	_RemoveChatMsgListByObjectID(m_sPlayerObjectID);
	if (m_pSkillCfgList[sSkillIndex]->m_iLevel < sValue)
	{
		wsprintf(cTxt, NOTIFYMSG_SKILL1, m_pSkillCfgList[sSkillIndex]->m_cName, sValue - m_pSkillCfgList[sSkillIndex]->m_iLevel);
		AddEventList(cTxt, 10);
		PlaySound('E', 23, 0);
		for (i = 1; i < DEF_MAXCHATMSGS; i++)
			if (m_pChatMsgList[i] == 0)
			{
				ZeroMemory(cTxt, sizeof(cTxt));
				wsprintf(cTxt, "%s Level +%d", m_pSkillCfgList[sSkillIndex]->m_cName, sValue - m_pSkillCfgList[sSkillIndex]->m_iLevel);
				m_pChatMsgList[i] = new class CMsg(20, cTxt, m_dwCurTime);
				m_pChatMsgList[i]->m_iObjectID = m_sPlayerObjectID;
				if (m_pMapData->bSetChatMsgOwner(m_sPlayerObjectID, -10, -10, i) == false)
				{
					delete m_pChatMsgList[i];
					m_pChatMsgList[i] = 0;
				}
				break;
			}
	}
	else if (m_pSkillCfgList[sSkillIndex]->m_iLevel > sValue) {
		wsprintf(cTxt, NOTIFYMSG_SKILL2, m_pSkillCfgList[sSkillIndex]->m_cName, m_pSkillCfgList[sSkillIndex]->m_iLevel - sValue);
		AddEventList(cTxt, 10);
		PlaySound('E', 24, 0);
		for (i = 1; i < DEF_MAXCHATMSGS; i++)
			if (m_pChatMsgList[i] == 0)
			{
				ZeroMemory(cTxt, sizeof(cTxt));
				wsprintf(cTxt, "%s Level -%d", m_pSkillCfgList[sSkillIndex]->m_cName, sValue - m_pSkillCfgList[sSkillIndex]->m_iLevel);
				m_pChatMsgList[i] = new class CMsg(20, cTxt, m_dwCurTime);
				m_pChatMsgList[i]->m_iObjectID = m_sPlayerObjectID;
				if (m_pMapData->bSetChatMsgOwner(m_sPlayerObjectID, -10, -10, i) == false)
				{
					delete m_pChatMsgList[i];
					m_pChatMsgList[i] = 0;
				}
				break;
			}
	}
	
	m_pSkillCfgList[sSkillIndex]->m_iLevel = sValue;
	m_cSkillMastery[sSkillIndex] = (unsigned char)sValue;
	m_iSkillSSN[sSkillIndex] = sValue2;
}

void CGame::NotifyMsg_SkillPoint(char* pData)
{
	short sSkillIndex, * sp;
	char* cp;

	cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);

	sp = (short*)cp;
	sSkillIndex = *sp;
	cp += 2;

	sp = (short*)cp;
	m_iSkillSSN[sSkillIndex] = *sp;
	cp += 2;

}

void CGame::NotifyMsg_SkillTrainSuccess(char * pData)
{
	char * cp, cSkillNum, cSkillLevel;
	char cTemp[120];
	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
	cSkillNum = *cp;
	cp++;
	cSkillLevel = *cp;
	cp++;
	ZeroMemory(cTemp, sizeof(cTemp));
	wsprintf(cTemp, NOTIFYMSG_SKILL_TRAIN_SUCCESS1, m_pSkillCfgList[cSkillNum]->m_cName, cSkillLevel);
	AddEventList(cTemp, 10);
	m_pSkillCfgList[cSkillNum]->m_iLevel = cSkillLevel;
	m_cSkillMastery[cSkillNum] = (unsigned char)cSkillLevel;
	PlaySound('E', 23, 0);
}

void CGame::NotifyMsg_SkillUsingEnd(char * pData)
{
	char * cp;
	UINT16 * wp, wResult;
	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
	wp = (UINT16 *)cp;
	wResult = *wp;
	switch (wResult) {
	case 0:
		AddEventList(NOTIFYMSG_SKILL_USINGEND1, 10);
		break;
	case 1:
		AddEventList(NOTIFYMSG_SKILL_USINGEND2, 10);
		break;
	}
	m_bSkillUsingStatus = false;
}

void CGame::NotifyMsg_SP(char * pData)
{
	int iPrevSP, *ip;
	iPrevSP = m_iSP;
	ip = (int *)(pData + DEF_INDEX2_MSGTYPE + 2);
	m_iSP = *ip;
	if (abs(m_iSP - iPrevSP) < 10) return;
	if (m_iSP > iPrevSP)
	{
		wsprintf(G_cTxt, NOTIFYMSG_SP_UP, m_iSP - iPrevSP);
		AddEnergyList(G_cTxt, 1);
		PlaySound('E', 21, 0);
	}
	else
	{
		wsprintf(G_cTxt, NOTIFYMSG_SP_DOWN, iPrevSP - m_iSP);
		AddEnergyList(G_cTxt, 1);
	}
}

void CGame::NotifyMsg_TotalUsers(char * pData)
{
	int* ip;

	ip = (int *)(pData + DEF_INDEX2_MSGTYPE + 2);
	m_iTotalUsers = *ip;

	wsprintf(G_cTxt, NOTIFYMSG_TOTAL_USER1, m_iTotalUsers);
	AddEventList(G_cTxt, 10);
}

void CGame::NotifyMsg_WhisperMode(bool bActive, char * pData)
{
	char cName[12], *cp;
	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
	ZeroMemory(cName, sizeof(cName));
	memcpy(cName, cp, 10);
	if (bActive == true)
	{
		wsprintf(G_cTxt, NOTIFYMSG_WHISPERMODE1, cName);
		if (m_pWhisperMsg[DEF_MAXWHISPERMSG - 1] != 0) {
			delete m_pWhisperMsg[DEF_MAXWHISPERMSG - 1];
			m_pWhisperMsg[DEF_MAXWHISPERMSG - 1] = 0;
		}
		for (int i = DEF_MAXWHISPERMSG - 2; i >= 0; i--) {
			m_pWhisperMsg[i + 1] = m_pWhisperMsg[i];
			m_pWhisperMsg[i] = 0;
		}
		m_pWhisperMsg[0] = new class CMsg(0, cName, 0);
		m_cWhisperIndex = 0;
	}
	else wsprintf(G_cTxt, NOTIFYMSG_WHISPERMODE2);

	AddEventList(G_cTxt, 10);
}

void CGame::DrawDialogBox_CrusadeJob(short msX, short msY)
{
	short sX, sY;

	sX = m_stDialogBoxInfo[33].sX;
	sY = m_stDialogBoxInfo[33].sY;
	DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME6, sX, sY, 0);

	switch (m_stDialogBoxInfo[33].cMode) {
	case 1:
		PutAlignedString(sX + 24, sX + 246, sY + 45 + 20, DRAWDIALOGBOX_CRUSADEJOB1);
		PutAlignedString(sX + 24, sX + 246, sY + 60 + 20, DRAWDIALOGBOX_CRUSADEJOB2);
		PutAlignedString(sX + 24, sX + 246, sY + 75 + 20, DRAWDIALOGBOX_CRUSADEJOB3);
		PutAlignedString(sX + 24, sX + 246, sY + 90 + 20, DRAWDIALOGBOX_CRUSADEJOB4);

		if (m_bCitizen == true)
		{
			if (m_bAresden)
			{
				if (m_iGuildRank == 0 || m_iGuildRank == 3)
				{
					if ((msX > sX + 24) && (msX < sX + 246) && (msY > sY + 150) && (msY < sY + 165))
						PutAlignedString(sX + 24, sX + 246, sY + 150, DRAWDIALOGBOX_CRUSADEJOB5, 255, 255, 255);//(Commander)"
					else PutAlignedString(sX + 24, sX + 246, sY + 150, DRAWDIALOGBOX_CRUSADEJOB5, 4, 0, 50);//(Commander)"
				}
				else
				{
					if ((msX > sX + 24) && (msX < sX + 246) && (msY > sY + 150) && (msY < sY + 165))
						PutAlignedString(sX + 24, sX + 246, sY + 150, DRAWDIALOGBOX_CRUSADEJOB7, 255, 255, 255);//(Soldier)"
					else PutAlignedString(sX + 24, sX + 246, sY + 150, DRAWDIALOGBOX_CRUSADEJOB7, 4, 0, 50);//(Soldier)"
					if (m_iGuildRank != -1)
					{
						if ((msX > sX + 24) && (msX < sX + 246) && (msY > sY + 175) && (msY < sY + 190))
							PutAlignedString(sX + 24, sX + 246, sY + 175, DRAWDIALOGBOX_CRUSADEJOB9, 255, 255, 255);//(Constructor)"
						else PutAlignedString(sX + 24, sX + 246, sY + 175, DRAWDIALOGBOX_CRUSADEJOB9, 4, 0, 50);//(Constructor)"

					}
				}
			}
			else if (m_bAresden == false)
			{
				if (m_iGuildRank == 0 || m_iGuildRank == 3)
				{
					if ((msX > sX + 24) && (msX < sX + 246) && (msY > sY + 150) && (msY < sY + 165))
						PutAlignedString(sX + 24, sX + 246, sY + 150, DRAWDIALOGBOX_CRUSADEJOB5, 255, 255, 255);//(Commander)"
					else PutAlignedString(sX + 24, sX + 246, sY + 150, DRAWDIALOGBOX_CRUSADEJOB5, 4, 0, 50);//(Commander)"
				}
				else
				{
					if ((msX > sX + 24) && (msX < sX + 246) && (msY > sY + 150) && (msY < sY + 165))
						PutAlignedString(sX + 24, sX + 246, sY + 150, DRAWDIALOGBOX_CRUSADEJOB7, 255, 255, 255);//(Soldier)"
					else PutAlignedString(sX + 24, sX + 246, sY + 150, DRAWDIALOGBOX_CRUSADEJOB7, 4, 0, 50);//(Soldier)"
					if (m_iGuildRank != -1)
					{
						if ((msX > sX + 24) && (msX < sX + 246) && (msY > sY + 175) && (msY < sY + 190))
							PutAlignedString(sX + 24, sX + 246, sY + 175, DRAWDIALOGBOX_CRUSADEJOB9, 255, 255, 255);//(Constructor)"
						else PutAlignedString(sX + 24, sX + 246, sY + 175, DRAWDIALOGBOX_CRUSADEJOB9, 4, 0, 50);//(Constructor)"
					}
				}
			}
		}

		PutAlignedString(sX + 24, sX + 246, sY + 290 - 40, DRAWDIALOGBOX_CRUSADEJOB10);
		PutAlignedString(sX + 24, sX + 246, sY + 305 - 40, DRAWDIALOGBOX_CRUSADEJOB17);

		if ((msX > sX + 210) && (msX < sX + 260) && (msY >= sY + 296) && (msY <= sY + 316))
			PutString_SprFont(sX + 50 + 160, sY + 296, "Help", 6, 6, 20);
		else PutString_SprFont(sX + 50 + 160, sY + 296, "Help", 0, 0, 7);
		break;

	case 2:
		PutAlignedString(sX + 24, sX + 246, sY + 90 + 20, DRAWDIALOGBOX_CRUSADEJOB18);
		switch (m_iCrusadeDuty) {
		case 1: PutAlignedString(sX + 24, sX + 246, sY + 125, DRAWDIALOGBOX_CRUSADEJOB19); break;//"(Soldier)
		case 2: PutAlignedString(sX + 24, sX + 246, sY + 125, DRAWDIALOGBOX_CRUSADEJOB20); break;//"(Constructor)
		case 3: PutAlignedString(sX + 24, sX + 246, sY + 125, DRAWDIALOGBOX_CRUSADEJOB21); break;//"(Commander)
		}

		PutAlignedString(sX + 24, sX + 246, sY + 145, DRAWDIALOGBOX_CRUSADEJOB22);
		if ((msX > sX + 24) && (msX < sX + 246) && (msY > sY + 160) && (msY < sY + 175))
			PutAlignedString(sX + 24, sX + 246, sY + 160, DRAWDIALOGBOX_CRUSADEJOB23, 255, 255, 255);
		else PutAlignedString(sX + 24, sX + 246, sY + 160, DRAWDIALOGBOX_CRUSADEJOB23, 4, 0, 50);

		PutAlignedString(sX + 24, sX + 246, sY + 175, DRAWDIALOGBOX_CRUSADEJOB25);
		PutAlignedString(sX + 24, sX + 246, sY + 190, DRAWDIALOGBOX_CRUSADEJOB26);

		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
		break;
	}
}

void CGame::_Draw_OnLogin(char *pAccount, char *pPassword, int msX, int msY, int iFrame)
{
	bool bFlag = true;
	UINT32 dwTime = timeGetTime();

	m_DDraw.ClearBackB4();

	DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_LOGIN, 80, 60, 0, true); // centu - new login
	DrawVersion();

	if ((iFrame >= 15) && (iFrame <= 20)) 
		m_pSprite[DEF_SPRID_INTERFACE_ND_LOGIN]->PutTransSprite25(43+75, 121+61, 2, true);
	else if (iFrame > 20) 
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_LOGIN, 43+75, 121+61, 2, true);

	if (m_cCurFocus != 1) 
	{
		if (m_Misc.bCheckValidName(pAccount) != false)
			PutString2(350-90, 285-65, pAccount, 200, 200, 200);
		else PutString2(350-90, 285-65, pAccount, 200, 100, 100);
	}

	if ((m_Misc.bCheckValidName(pAccount) == false) || (strlen(pAccount) == 0)) bFlag = false;

	if (m_cCurFocus != 2) 
	{
		if ((m_Misc.bCheckValidString(pPassword) != false))
			PutString(350-90, 310-65, pPassword, RGB(200, 200, 200), true, 1);
		else PutString(350-90, 310-65, pPassword, RGB(200, 100, 100), true, 1);
	}

	if ((m_Misc.bCheckValidString(pPassword) == false) || (strlen(pPassword) == 0)) bFlag = false;

	if (m_cCurFocus == 1)
		ShowReceivedString();
	else
		if (m_cCurFocus == 2)
			ShowReceivedString(true);

	if (bFlag == true)
	{
		if (m_cCurFocus == 3) DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_LOGIN, 80+80, 282+60, 3, true);
	}
	if (m_cCurFocus == 4) DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_LOGIN, 256+80, 282+60, 4, true);
	if ((m_bIsHideLocalCursor != true) && (msX != 0) && (msY != 0)) {
		m_pSprite[DEF_SPRID_MOUSECURSOR]->PutSpriteFast(msX, msY, 0, dwTime);
	}
}

void CGame::ShowEventList(UINT32 dwTime)
{
	int i;
	int resi;
#ifdef RES_HIGH
	resi = 120;
#else
	resi = 0;
#endif

	m_DDraw._GetBackBufferDC();
	for (i = 0; i < 6; i++)
		if ((dwTime - m_stEventHistory[i].dwTime) < 5000)
		{
			switch (m_stEventHistory[i].cColor) {
			case 0:
				PutString(10, 50 + i * 15, m_stEventHistory[i].cTxt, RGB(225, 225, 225), false, 1, true);
				break;
			case 1:
				PutString(10, 50 + i * 15, m_stEventHistory[i].cTxt, RGB(130, 255, 130), false, 1, true);
				break;
			case 2:
				PutString(10, 50 + i * 15, m_stEventHistory[i].cTxt, RGB(255, 130, 130), false, 1, true);
				break;
			case 3:
				PutString(10, 50 + i * 15, m_stEventHistory[i].cTxt, RGB(130, 130, 255), false, 1, true);
				break;
			case 4:
				PutString(10, 50 + i * 15, m_stEventHistory[i].cTxt, RGB(230, 230, 130), false, 1, true);
				break;
			case 10:
				PutString(10, 50 + i * 15, m_stEventHistory[i].cTxt, RGB(180, 255, 180), false, 1, true);
				break;
			case CHAT_GM:
				PutString(10, 50 + i * 15, m_stEventHistory[i].cTxt, RGB(255, 184, 0), false, 1, true);
				break;
			case CHAT_MARKET:
				PutString(10, 50 + i * 15, m_stEventHistory[i].cTxt, RGB(50, 255, 255), false, 1, true);
				break;
			case 20:
				PutString(10, 50 + i * 15, m_stEventHistory[i].cTxt, RGB(150, 150, 170), false, 1, true);
				break;
			}
		}

	for (i = 0; i < 6; i++)
		if ((dwTime - m_stEventHistory2[i].dwTime) < 5000)
		{
			switch (m_stEventHistory2[i].cColor) {
			case 0:
				PutString(10, 322 + resi + i * 15, m_stEventHistory2[i].cTxt, RGB(225, 225, 225), false, 1, true);
				break;
			case 1:
				PutString(10, 322 + resi + i * 15, m_stEventHistory2[i].cTxt, RGB(130, 255, 130), false, 1, true);
				break;
			case 2:
				PutString(10, 322 + resi + i * 15, m_stEventHistory2[i].cTxt, RGB(255, 130, 130), false, 1, true);
				break;
			case 3:
				PutString(10, 322 + resi + i * 15, m_stEventHistory2[i].cTxt, RGB(130, 130, 255), false, 1, true);
				break;
			case 4:
				PutString(10, 322 + resi + i * 15, m_stEventHistory2[i].cTxt, RGB(230, 230, 130), false, 1, true);
				break;
			case 10:
				PutString(10, 322 + resi + i * 15, m_stEventHistory2[i].cTxt, RGB(180, 255, 180), false, 1, true);
				break;
			case CHAT_GM:
				PutString(10, 322 + resi + i * 15, m_stEventHistory2[i].cTxt, RGB(255, 184, 0), false, 1, true);
				break;
			case CHAT_MARKET:
				PutString(10, 322 + resi + i * 15, m_stEventHistory2[i].cTxt, RGB(50, 255, 255), false, 1, true);
				break;
			case 20:
				PutString(10, 322 + resi + i * 15, m_stEventHistory2[i].cTxt, RGB(150, 150, 170), false, 1, true);
				break;
			}
		}
	if (m_bSkillUsingStatus == true)
	{
#ifdef RES_HIGH
		PutString(440 - 29, 440 - 52, SHOW_EVENT_LIST1, RGB(235, 235, 235), false, 1, true);
#else
		PutString(280 - 29, 280 - 52, SHOW_EVENT_LIST1, RGB(235, 235, 235), false, 1, true);
#endif
	}
	m_DDraw._ReleaseBackBufferDC();
}

void CGame::RequestTeleportAndWaitData()
{	// Snoopy: removed that, Noob Dungeon is now at farm...

	bSendCommand(MSGID_REQUEST_TELEPORT, 0, 0, 0, 0, 0, 0);
	ChangeGameMode(DEF_GAMEMODE_ONWAITINGINITDATA);
}

void CGame::MotionEventHandler(char* pData)
{
	UINT16* wp, wEventType, wObjectID;
	short* sp, sX, sY, sType, sAppr1, sAppr2, sAppr3, sAppr4, sV1, sV2, sV3, sPrevAppr2;
	int iStatus, iStatus2;
	char* cp, cDir, cName[12];
	int* ip, iApprColor, iLoc;
	char    cTxt[120];
	int i;
	//50Cent - No Critical Damage Limit
	int iDamage = 0;
	ZeroMemory(cName, sizeof(cName));
	sV1 = sV2 = sV3 = 0;
	wp = (UINT16*)(pData + DEF_INDEX2_MSGTYPE);
	wEventType = *wp;
	cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);
	wp = (UINT16*)cp;
	wObjectID = *wp;
	cp += 2;
	iLoc = 0;
	if (wObjectID < 30000)
	{
		if (wObjectID < 10000) 	// Player
		{
			sp = (short*)cp;
			sX = *sp;
			cp += 2;
			sp = (short*)cp;
			sY = *sp;
			cp += 2;
			sp = (short*)cp;
			sType = *sp;
			cp += 2;
			cDir = *cp;
			cp++;
			memcpy(cName, cp, 10);
			cp += 10;
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
			ip = (int*)cp; // v1.4
			iApprColor = *ip;
			cp += 4;

			// CLEROTH - CRASH BUG ( STATUS )
			ip = (int*)cp;
			iStatus = *ip;
			cp += 4;

			iLoc = *cp;
			cp++;
		}
		else 	// Npc or mob
		{
			sp = (short*)cp;
			sX = *sp;
			cp += 2;
			sp = (short*)cp;
			sY = *sp;
			cp += 2;
			sp = (short*)cp;
			sType = *sp;
			cp += 2;
			cDir = *cp;
			cp++;
			memcpy(cName, cp, 5);
			cp += 5;
			sAppr1 = sAppr3 = sAppr4 = 0;
			iStatus2 = 0;
			sp = (short*)cp;
			sAppr2 = *sp;
			cp += 2;
			ip = (int*)cp;
			iStatus = *ip;
			cp += 4;
			iLoc = *cp;
			cp++;
		}
	}
	else
	{
		switch (wEventType) {

			//50Cent - No Critical Damage Limit
		case DEF_OBJECTMAGIC:
			cDir = *cp;
			cp++;
			sV1 = (short)*cp; //Damage
			cp++;
			sV2 = (short)*cp; //
			cp++;
			break;
		case DEF_OBJECTDAMAGEMOVE:
		case DEF_OBJECTDAMAGE:
			cDir = *cp;
			cp++;

			ip = (int*)cp;
			iDamage = *ip;
			cp += 4;

			sV2 = (short)*cp; //
			cp++;
			break;

		case DEF_OBJECTDYING:
			cDir = *cp;
			cp++;

			ip = (int*)cp;
			iDamage = *ip;
			cp += 4;

			sV2 = (short)*cp; //
			cp++;
			sp = (short*)cp;
			sX = *sp;
			cp += 2;
			sp = (short*)cp;
			sY = *sp;
			cp += 2;
			break;

		case DEF_OBJECTATTACK:
			cDir = *cp;
			cp++;
			sV1 = *cp;
			cp++;
			sV2 = *cp;
			cp++;
			sp = (short*)cp;
			sV3 = *sp;
			cp += 2;
			break;

		default:
			cDir = *cp;
			cp++;
			break;
		}
	}

	if ((wEventType == DEF_OBJECTNULLACTION) && (memcmp(cName, m_cPlayerName, 10) == 0))
	{
		m_sPlayerType = sType;
		m_sPlayerAppr1 = sAppr1;
		sPrevAppr2 = m_sPlayerAppr2;
		m_sPlayerAppr2 = sAppr2;
		m_sPlayerAppr3 = sAppr3;
		m_sPlayerAppr4 = sAppr4;
		m_iPlayerApprColor = iApprColor;
		m_iPlayerStatus = iStatus;
		if ((sPrevAppr2 & 0xF000) == 0)
		{
			if ((sAppr2 & 0xF000) != 0)
			{
				AddEventList(MOTION_EVENT_HANDLER1, 10);
				m_bIsCombatMode = true;
			}
		}
		else
		{
			if ((sAppr2 & 0xF000) == 0)
			{
				AddEventList(MOTION_EVENT_HANDLER2, 10);
				m_bIsCombatMode = false;
			}
		}
		if (m_cCommand != DEF_OBJECTRUN) m_pMapData->bSetOwner(wObjectID, sX, sY, sType, cDir, sAppr1, sAppr2, sAppr3, sAppr4, iApprColor, iStatus, cName, (char)wEventType, sV1, sV2, sV3, iLoc);
	}
	else m_pMapData->bSetOwner(wObjectID, sX, sY, sType, cDir, sAppr1, sAppr2, sAppr3, sAppr4, iApprColor, iStatus, cName, (char)wEventType, sV1, sV2, sV3, iLoc);

	switch (wEventType) {
	case DEF_OBJECTMAGIC: // Casting
		_RemoveChatMsgListByObjectID(wObjectID - 30000);

		for (i = 1; i < DEF_MAXCHATMSGS; i++)
			if (m_pChatMsgList[i] == 0)
			{
				ZeroMemory(cTxt, sizeof(cTxt));
				wsprintf(cTxt, "%s!", m_pMagicCfgList[sV1]->m_cName);
				m_pChatMsgList[i] = new class CMsg(41, cTxt, m_dwCurTime);
				m_pChatMsgList[i]->m_iObjectID = wObjectID - 30000;
				if (m_pMapData->bSetChatMsgOwner(wObjectID - 30000, -10, -10, i) == false)
				{
					delete m_pChatMsgList[i];
					m_pChatMsgList[i] = 0;
				}
				return;
			}
		break;

	case DEF_OBJECTDYING:
		_RemoveChatMsgListByObjectID(wObjectID - 30000);
		for (i = 1; i < DEF_MAXCHATMSGS; i++) {
			if (m_pChatMsgList[i] == 0)
			{	/* Showing real damage done to NPCs instead of Critical!
				Credit goes to: 50Cent, SleeQ, Matt */
				ZeroMemory(cTxt, sizeof(cTxt));
				wsprintf(cTxt, "-%dHp!", iDamage);
				int iFontType;
				if ((iDamage >= 0) && (iDamage < 20))        iFontType = 21;
				else if ((iDamage >= 20) && (iDamage < 50)) iFontType = 22;
				else if (iDamage >= 50 || iDamage < 0)    iFontType = 23;
				m_pChatMsgList[i] = new class CMsg(iFontType, cTxt, m_dwCurTime);
				m_pChatMsgList[i]->m_iObjectID = wObjectID - 30000;
				if (m_pMapData->bSetChatMsgOwner(wObjectID - 30000, -10, -10, i) == false)
				{
					delete m_pChatMsgList[i];
					m_pChatMsgList[i] = 0;
				}
				return;
			}
		}
		break;

	case DEF_OBJECTDAMAGEMOVE:
	case DEF_OBJECTDAMAGE:
		if (memcmp(cName, m_cPlayerName, 10) == 0)
		{
			m_bIsGetPointingMode = false;
			m_iPointCommandType = -1;
			m_stMCursor.sCursorFrame = 0;
			ClearSkillUsingStatus();
		}
		_RemoveChatMsgListByObjectID(wObjectID - 30000);

		for (i = 1; i < DEF_MAXCHATMSGS; i++)
			if (m_pChatMsgList[i] == 0)
			{	/* Showing real damage done to NPCs instead of Critical!
				Credit goes to: 50Cent, SleeQ, Matt */
				ZeroMemory(cTxt, sizeof(cTxt));
				wsprintf(cTxt, "-%dHp", iDamage);
				int iFontType;
				if ((iDamage >= 0) && (iDamage < 20))        iFontType = 21;
				else if ((iDamage >= 20) && (iDamage < 50)) iFontType = 22;
				else if (iDamage >= 50 || iDamage < 0)    iFontType = 23;
				m_pChatMsgList[i] = new class CMsg(iFontType, cTxt, m_dwCurTime);
				m_pChatMsgList[i]->m_iObjectID = wObjectID - 30000;
				if (m_pMapData->bSetChatMsgOwner(wObjectID - 30000, -10, -10, i) == false)
				{
					delete m_pChatMsgList[i];
					m_pChatMsgList[i] = 0;
				}
				return;
			}
		break;
	}
}

void CGame::DrawDialogBox_Commander(int msX, int msY) // Snoopy: Fixed for 351
{
	short sX, sY, szX, szY, MapSzX, MapSzY;
	UINT32 dwTime = G_dwGlobalTime;
	float dV1, dV2, dV3;
	int i, tX, tY;
	sX = m_stDialogBoxInfo[36].sX;
	sY = m_stDialogBoxInfo[36].sY;
	szX = m_stDialogBoxInfo[36].sSizeX;
	if ((dwTime - m_dwCommanderCommandRequestedTime) > 1000 * 10)
	{
		_RequestMapStatus("middleland", 3);
		_RequestMapStatus("middleland", 1);
		m_dwCommanderCommandRequestedTime = dwTime;
	}

	DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_CRUSADE, sX, sY - 5, 0, false, m_bDialogTrans); // Main image
	DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_TEXT, sX, sY, 15, false, m_bDialogTrans);

	switch (m_stDialogBoxInfo[36].cMode) {
	case 0: // Main dlg
		m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20, sY + 340, 3, dwTime); // btn
		m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 50, sY + 340, 1, dwTime); // btn
		m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 100, sY + 340, 2, dwTime); // btn
		m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 150, sY + 340, 30, dwTime); // Btn TP
		m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 150 + 74, sY + 340, 4, dwTime); // btn ?
		PutAlignedString(sX, sX + szX, sY + 37, DRAW_DIALOGBOX_COMMANDER1);//"Middleland warfare building structure situation"

		if ((msX >= sX + 20) && (msX <= sX + 20 + 46)
			&& (msY >= sY + 340) && (msY <= sY + 340 + 52))
		{
			m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20, sY + 340, 17, dwTime);
			PutString2(msX + 20, msY + 35, DRAW_DIALOGBOX_COMMANDER2, 255, 255, 255);//"Middleland teleport position set"
		}
		else if ((msX >= sX + 20 + 50) && (msX <= sX + 20 + 46 + 50)
			&& (msY >= sY + 340) && (msY <= sY + 340 + 52))
		{
			m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 50, sY + 340, 15, dwTime);
			PutString2(msX + 20, msY + 35, DRAW_DIALOGBOX_COMMANDER3, 255, 255, 255);//"Teleport to the selected area"
		}
		else if ((msX >= sX + 20 + 100) && (msX <= sX + 20 + 46 + 100)
			&& (msY >= sY + 340) && (msY <= sY + 340 + 52)) {
			m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 100, sY + 340, 16, dwTime);
			PutString2(msX + 20, msY + 35, DRAW_DIALOGBOX_COMMANDER4, 255, 255, 255);//"Recall warfare units"
		}
		else if ((msX >= sX + 20 + 150) && (msX <= sX + 20 + 46 + 150)
			&& (msY >= sY + 340) && (msY <= sY + 340 + 52))
		{
			m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 150, sY + 340, 24, dwTime);
			PutString2(msX + 20, msY + 35, DRAW_DIALOGBOX_COMMANDER5, 255, 255, 255);//"Middleland construction building set"
		}
		else if ((msX >= sX + 20 + 150 + 74) && (msX <= sX + 20 + 46 + 150 + 74)
			&& (msY >= sY + 340) && (msY <= sY + 340 + 52))
		{
			m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 150 + 74, sY + 340, 18, dwTime);
			PutString2(msX + 20, msY + 35, DRAW_DIALOGBOX_COMMANDER6, 255, 255, 255);//"Commander's duty help"
		}
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_CRUSADE, sX, sY, 21, false, m_bDialogTrans);// Map
		break;

	case 1: // Set TP
		m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 100 + 74, sY + 340, 20, dwTime); //btn <-
		m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 150 + 74, sY + 340, 4, dwTime);  //btn ?
		PutAlignedString(sX, sX + szX, sY + 40, DRAW_DIALOGBOX_COMMANDER7); // "Click and select the teleport location."

		if ((msX >= sX + 20 + 150 + 74 - 50) && (msX <= sX + 20 + 46 + 150 + 74 - 50)
			&& (msY >= sY + 340) && (msY <= sY + 340 + 52))
		{
			m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 100 + 74, sY + 340, 19, dwTime);
			PutString2(msX + 20, msY + 35, DRAW_DIALOGBOX_COMMANDER8, 255, 255, 255);
		}
		else if ((msX >= sX + 20 + 150 + 74) && (msX <= sX + 20 + 46 + 150 + 74)
			&& (msY >= sY + 340) && (msY <= sY + 340 + 52))
		{
			m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 150 + 74, sY + 340, 18, dwTime);
			PutString2(msX + 20, msY + 35, DRAW_DIALOGBOX_COMMANDER9, 255, 255, 255);
		}
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_CRUSADE, sX, sY, 21, false, m_bDialogTrans);

		if ((msX >= sX + 15) && (msX <= sX + 15 + 278)
			&& (msY >= sY + 60) && (msY <= sY + 60 + 272)) // shows TP posit on map following the mouse
		{
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_CRUSADE, msX, msY, 42, false, true);
		}
		break;

	case 2: // Use TP
		m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 50, sY + 340, 1, dwTime);       // Btn TP
		m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 100 + 74, sY + 340, 20, dwTime); // Btn <-
		m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 150 + 74, sY + 340, 4, dwTime);  // Btn ?
		PutAlignedString(sX, sX + szX, sY + 40, DRAW_DIALOGBOX_COMMANDER10); // "Teleport to the selected area"

		if ((msX >= sX + 20 + 50) && (msX <= sX + 20 + 46 + 50)
			&& (msY >= sY + 340) && (msY <= sY + 340 + 52))
		{
			m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 50, sY + 340, 15, dwTime);
			PutString2(msX + 20, msY + 35, DRAW_DIALOGBOX_COMMANDER11, 255, 255, 255);
		}
		else if ((msX >= sX + 20 + 150 + 74 - 50) && (msX <= sX + 20 + 46 + 150 + 74 - 50)
			&& (msY >= sY + 340) && (msY <= sY + 340 + 52))
		{
			m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 100 + 74, sY + 340, 19, dwTime);
			PutString2(msX + 20, msY + 35, DRAW_DIALOGBOX_COMMANDER12, 255, 255, 255);
		}
		else if ((msX >= sX + 20 + 150 + 74) && (msX <= sX + 20 + 46 + 150 + 74)
			&& (msY >= sY + 340) && (msY <= sY + 340 + 52))
		{
			m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 150 + 74, sY + 340, 18, dwTime);
			PutString2(msX + 20, msY + 35, DRAW_DIALOGBOX_COMMANDER13, 255, 255, 255);
		}
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_CRUSADE, sX, sY, 21, false, m_bDialogTrans);
		break;

	case 3: // Choose summon
		if ((m_bCitizen == true) && (m_bAresden == true))
		{
			m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20, sY + 220, 6, dwTime);
			m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 50, sY + 220, 5, dwTime);
			m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 100, sY + 220, 7, dwTime);
			m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 150, sY + 220, 35, dwTime);
		}
		else if ((m_bCitizen == true) && (m_bAresden == false))
		{
			m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20, sY + 220, 9, dwTime);
			m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 50, sY + 220, 8, dwTime);
			m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 100, sY + 220, 7, dwTime);
			m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 150, sY + 220, 35, dwTime);
		}
		m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 100 + 74, sY + 340, 20, dwTime);
		m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 150 + 74, sY + 340, 4, dwTime);

		PutAlignedString(sX, sX + szX, sY + 40, DRAW_DIALOGBOX_COMMANDER14); // "Recall warfare units"

		wsprintf(G_cTxt, "%s %d", DRAW_DIALOGBOX_COMMANDER15, m_iConstructionPoint);
		PutAlignedString(sX, sX + 323, sY + 190, G_cTxt);

		if ((m_bCitizen == true) && (m_bAresden == true))
		{
			if ((msX >= sX + 20) && (msX <= sX + 20 + 46) && (msY >= sY + 220) && (msY <= sY + 220 + 50))
			{
				if (m_iConstructionPoint >= 3000)
				{
					m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20, sY + 220, 11, dwTime);
				}
				PutString2(msX + 20, msY + 35, DRAW_DIALOGBOX_COMMANDER16, 255, 255, 255);//"�(Battle Golem)"
				PutString2(msX + 20, msY + 50, DRAW_DIALOGBOX_COMMANDER17, 255, 255, 255);//"3000"
			}
			else if ((msX >= sX + 20 + 50) && (msX <= sX + 20 + 50 + 45) && (msY >= sY + 220) && (msY <= sY + 220 + 50))
			{
				if (m_iConstructionPoint >= 2000)
				{
					m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 50, sY + 220, 10, dwTime);
				}
				PutString2(msX + 20, msY + 35, DRAW_DIALOGBOX_COMMANDER18, 255, 255, 255);//"(Temple Knight)"
				PutString2(msX + 20, msY + 50, DRAW_DIALOGBOX_COMMANDER19, 255, 255, 255);//"2000"
			}
			else if ((msX >= sX + 20 + 100) && (msX <= sX + 20 + 100 + 45) && (msY >= sY + 220) && (msY <= sY + 220 + 50))
			{
				if (m_iConstructionPoint >= 1000)
				{
					m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 100, sY + 220, 12, dwTime);
				}
				PutString2(msX + 20, msY + 35, DRAW_DIALOGBOX_COMMANDER20, 255, 255, 255);//"(Light War Beetle)"
				PutString2(msX + 20, msY + 50, DRAW_DIALOGBOX_COMMANDER21, 255, 255, 255);//" 1000"
			}
			else if ((msX >= sX + 20 + 150) && (msX <= sX + 20 + 150 + 45) && (msY >= sY + 220) && (msY <= sY + 220 + 50))
			{
				if (m_iConstructionPoint >= 5000)
				{
					m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 150, sY + 220, 29, dwTime);
				}
				PutString2(msX + 20, msY + 35, DRAW_DIALOGBOX_COMMANDER22, 255, 255, 255);//"(Catapult)"
				PutString2(msX + 20, msY + 50, DRAW_DIALOGBOX_COMMANDER23, 255, 255, 255);// 1500"
			}
			else if ((msX >= sX + 20) && (msX <= sX + 380) && (msY > sY + 140) && (msY < sY + 160))
			{
				PutString2(msX + 20, msY + 35, DRAW_DIALOGBOX_COMMANDER24, 255, 255, 255);
			}
			else if ((msX >= sX + 20) && (msX <= sX + 380) && (msY > sY + 160) && (msY < sY + 175))
			{
				PutString2(msX + 20, msY + 35, DRAW_DIALOGBOX_COMMANDER25, 255, 255, 255);
			}
			else if ((msX >= sX + 20 + 150 + 74 - 50) && (msX <= sX + 20 + 46 + 150 + 74 - 50)
				&& (msY >= sY + 340) && (msY <= sY + 340 + 52))
			{
				m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 100 + 74, sY + 340, 19, dwTime);
				PutString2(msX + 20, msY + 35, DRAW_DIALOGBOX_COMMANDER26, 255, 255, 255);
			}
			else if ((msX >= sX + 20 + 150 + 74) && (msX <= sX + 20 + 46 + 150 + 74)
				&& (msY >= sY + 340) && (msY <= sY + 340 + 52))
			{
				m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 150 + 74, sY + 340, 18, dwTime);
				PutString2(msX + 20, msY + 35, DRAW_DIALOGBOX_COMMANDER27, 255, 255, 255); //"Warfare units recall related help"
			}
		}
		else if ((m_bCitizen == true) && (m_bAresden == false))
		{
			if ((msX >= sX + 20) && (msX <= sX + 20 + 46) && (msY >= sY + 220) && (msY <= sY + 220 + 50))
			{
				if (m_iConstructionPoint >= 3000)
				{
					m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20, sY + 220, 14, dwTime);
				}
				PutString2(msX + 20, msY + 35, DRAW_DIALOGBOX_COMMANDER28, 255, 255, 255);//"(God's Hand Knight Cavalry)"
				PutString2(msX + 20, msY + 50, DRAW_DIALOGBOX_COMMANDER29, 255, 255, 255);//"3000"
			}
			else if ((msX >= sX + 20 + 50) && (msX <= sX + 20 + 50 + 45)
				&& (msY >= sY + 220) && (msY <= sY + 220 + 50))
			{
				if (m_iConstructionPoint >= 2000)
				{
					m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 50, sY + 220, 13, dwTime);
				}
				PutString2(msX + 20, msY + 35, DRAW_DIALOGBOX_COMMANDER30, 255, 255, 255);//"�(God's Hand Knight)"
				PutString2(msX + 20, msY + 50, DRAW_DIALOGBOX_COMMANDER31, 255, 255, 255);//"2000"
			}
			else if ((msX >= sX + 20 + 100) && (msX <= sX + 20 + 100 + 45) && (msY >= sY + 220) && (msY <= sY + 220 + 50))
			{
				if (m_iConstructionPoint >= 1000)
				{
					m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 100, sY + 220, 12, dwTime);
				}
				PutString2(msX + 20, msY + 35, DRAW_DIALOGBOX_COMMANDER32, 255, 255, 255);//"(Light War Beetle)"
				PutString2(msX + 20, msY + 50, DRAW_DIALOGBOX_COMMANDER33, 255, 255, 255);//" 1000"
			}
			else if ((msX >= sX + 20 + 150) && (msX <= sX + 20 + 150 + 45) && (msY >= sY + 220) && (msY <= sY + 220 + 50))
			{
				if (m_iConstructionPoint >= 5000)
				{
					m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 150, sY + 220, 29, dwTime);
				}
				PutString2(msX + 20, msY + 35, DRAW_DIALOGBOX_COMMANDER34, 255, 255, 255);//"(Catapult)"
				PutString2(msX + 20, msY + 50, DRAW_DIALOGBOX_COMMANDER35, 255, 255, 255);//" 1500"
			}
			else if ((msX >= sX + 20) && (msX <= sX + 380) && (msY > sY + 140) && (msY < sY + 160))
			{
				PutString2(msX + 20, msY + 35, DRAW_DIALOGBOX_COMMANDER36, 255, 255, 255);
			}
			else if ((msX >= sX + 20) && (msX <= sX + 380) && (msY > sY + 160) && (msY < sY + 175))
			{
				PutString2(msX + 20, msY + 35, DRAW_DIALOGBOX_COMMANDER37, 255, 255, 255);
			}
			else if ((msX >= sX + 20 + 150 + 74 - 50) && (msX <= sX + 20 + 46 + 150 + 74 - 50)
				&& (msY >= sY + 340) && (msY <= sY + 340 + 52))
			{
				m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 100 + 74, sY + 340, 19, dwTime);
				PutString2(msX + 20, msY + 35, DRAW_DIALOGBOX_COMMANDER38, 255, 255, 255);

			}
			else if ((msX >= sX + 20 + 150 + 74) && (msX <= sX + 20 + 46 + 150 + 74)
				&& (msY >= sY + 340) && (msY <= sY + 340 + 52))
			{
				m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 150 + 74, sY + 340, 18, dwTime);
				PutString2(msX + 20, msY + 35, DRAW_DIALOGBOX_COMMANDER39, 255, 255, 255);
			}
		}
		PutAlignedString(sX, sX + 323, sY + 80, DRAW_DIALOGBOX_COMMANDER40);
		PutAlignedString(sX, sX + 323, sY + 95, DRAW_DIALOGBOX_COMMANDER41);
		PutAlignedString(sX, sX + 323, sY + 110, DRAW_DIALOGBOX_COMMANDER42);

		switch (m_stDialogBoxInfo[36].sV1) {
		case 0:
			PutAlignedString(sX, sX + 323, sY + 140, DRAW_DIALOGBOX_COMMANDER43, 255, 255, 255);//"Guard Mode"
			PutAlignedString(sX, sX + 323, sY + 160, DRAW_DIALOGBOX_COMMANDER44, 4, 0, 50);//"Hold Mode"
			break;
		case 1:
			PutAlignedString(sX, sX + 323, sY + 140, DRAW_DIALOGBOX_COMMANDER43, 4, 0, 50);//"Guard Mode"
			PutAlignedString(sX, sX + 323, sY + 160, DRAW_DIALOGBOX_COMMANDER44, 255, 255, 255);//"Hold Mode"
			break;
		}
		break;

	case 4: // Set Construction point
		m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 100 + 74, sY + 340, 20, dwTime);
		m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 150 + 74, sY + 340, 4, dwTime);
		PutAlignedString(sX, sX + szX, sY + 40, DRAW_DIALOGBOX_COMMANDER47); // "Select the construction area and click."

		if ((msX >= sX + 20 + 150 + 74 - 50) && (msX <= sX + 20 + 46 + 150 + 74 - 50)
			&& (msY >= sY + 340) && (msY <= sY + 340 + 52))
		{
			m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 100 + 74, sY + 340, 19, dwTime);
			PutString2(msX + 20, msY + 35, DRAW_DIALOGBOX_COMMANDER48, 255, 255, 255); // "Returns to commander's main menu"
		}
		else if ((msX >= sX + 20 + 150 + 74) && (msX <= sX + 20 + 46 + 150 + 74)
			&& (msY >= sY + 340) && (msY <= sY + 340 + 52))
		{
			m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 150 + 74, sY + 340, 18, dwTime);
			PutString2(msX + 20, msY + 35, DRAW_DIALOGBOX_COMMANDER49, 255, 255, 255); // "Construction area set related help"
		}
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_CRUSADE, sX, sY, 21);
		if ((msX >= sX + 15) && (msX <= sX + 15 + 278)
			&& (msY >= sY + 60) && (msY <= sY + 60 + 272)) // shows posit on map following the mouse
		{
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_CRUSADE, msX, msY, 41, false, true);
		}
		break;
	}

	switch (m_stDialogBoxInfo[36].cMode) {
	case 0: // Main
	case 1: // Set TP
	case 2: // Use TP
	case 4: // Set construc
		szX = 0;
		szY = 0;
		MapSzX = 0;
		MapSzY = 0;
		if (strcmp(m_cStatusMapName, "aresden") == 0)
		{
			szX = 250;
			szY = 250;
		}
		else if (strcmp(m_cStatusMapName, "elvine") == 0)
		{
			szX = 250;
			szY = 250;
		}
		else if (strcmp(m_cStatusMapName, "middleland") == 0)
		{
			szX = 279;
			szY = 280;
			MapSzX = 524;
			MapSzY = 524;
		}
		if (szX != 0)
		{
			for (i = 0; i < DEF_MAXCRUSADESTRUCTURES; i++)
				if (m_stCrusadeStructureInfo[i].cType != 0)
				{
					dV1 = (float)MapSzX;
					dV2 = (float)m_stCrusadeStructureInfo[i].sX;
					dV3 = (dV2*(float)szX) / dV1;
					tX = (int)dV3;
					dV1 = (float)MapSzY;
					dV2 = (float)m_stCrusadeStructureInfo[i].sY;
					dV3 = (dV2*(float)szY) / dV1;
					tY = (int)dV3;
					switch (m_stCrusadeStructureInfo[i].cType) {
					case 38:
						if (m_stCrusadeStructureInfo[i].cSide == 1)
							DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_CRUSADE, sX + tX + 15, sY + tY + 60, 39, false, true); // Base
						else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_CRUSADE, sX + tX + 15, sY + tY + 60, 37, false, true);
						break;
					case 36:
					case 37:
					case 39:
						if (m_stCrusadeStructureInfo[i].cSide == 1)
							DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_CRUSADE, sX + tX + 15, sY + tY + 60, 38, false, true); // Base
						else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_CRUSADE, sX + tX + 15, sY + tY + 60, 36, false, true);
						break;
					case 42:
						DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_CRUSADE, sX + tX + 15, sY + tY + 60, 40); // ManaStone
						break;
					}
				}
			if (m_iTeleportLocX != -1)
			{
				dV1 = (float)MapSzX;
				dV2 = (float)m_iTeleportLocX;
				dV3 = (dV2*(float)szX) / dV1;
				tX = (int)dV3;
				dV1 = (float)MapSzY;
				dV2 = (float)m_iTeleportLocY;
				dV3 = (dV2*(float)szY) / dV1;
				tY = (int)dV3;
				if ((m_stDialogBoxInfo[36].cMode == 1) && (tY >= 30) && (tY <= 494)) // Place the TP
				{
					DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_CRUSADE, sX + tX + 15, sY + tY + 60, 42, false, true); //TP
				}
				else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_CRUSADE, sX + tX + 15, sY + tY + 60, 42, false, true); //TP
			}
			if ((m_stDialogBoxInfo[36].cMode != 2) && (m_iConstructLocX != -1))
			{
				dV1 = (float)MapSzX;
				dV2 = (float)m_iConstructLocX;
				dV3 = (dV2*(float)szX) / dV1;
				tX = (int)dV3;
				dV1 = (float)MapSzY;
				dV2 = (float)m_iConstructLocY;
				dV3 = (dV2*(float)szY) / dV1;
				tY = (int)dV3;
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_CRUSADE, sX + tX + 15, sY + tY + 60, 41, false, true); // Contr pt
			}
			if (strcmp(m_cMapName, "middleland") == 0)
			{
				dV1 = (float)MapSzX;
				dV2 = (float)m_sPlayerX;
				dV3 = (dV2*(float)szX) / dV1;
				tX = (int)dV3;
				dV1 = (float)MapSzY;
				dV2 = (float)m_sPlayerY;
				dV3 = (dV2*(float)szY) / dV1;
				tY = (int)dV3;
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_CRUSADE, sX + tX + 15, sY + tY + 60, 43); // Player place
			}
		}
		if (m_stDialogBoxInfo[36].cMode != 3)
		{
			if ((msX >= sX + 15) && (msX <= sX + 15 + 278)
				&& (msY >= sY + 60) && (msY <= sY + 60 + 272)) // Draw mouse coordinates
			{
				dV1 = (float)(msX - (sX + 15));
				dV2 = (float)MapSzX;
				dV3 = (dV2*dV1) / szX;
				tX = (int)dV3;
				dV1 = (float)(msY - (sY + 60));
				dV2 = (float)MapSzX;
				dV3 = (dV2*dV1) / szY;
				tY = (int)dV3;
				if (tX < 30) tX = 30;
				if (tY < 30) tY = 30;
				if (tX > MapSzX - 30) tX = MapSzX - 30;
				if (tY > MapSzY - 30) tY = MapSzY - 30;
				wsprintf(G_cTxt, "%d,%d", tX, tY);
				PutString_SprFont3(msX + 10, msY - 10, G_cTxt, m_wR[13] * 4, m_wG[13] * 4, m_wB[13] * 4, false, 2);
			}
		}
		break;
	}
}

void CGame::DrawDialogBox_Constructor(int msX, int msY) // Snoopy: Fixed for 351
{
	short sX, sY, szX, szY, MapSzX, MapSzY;
	UINT32 dwTime = G_dwGlobalTime;
	float dV1, dV2, dV3;
	int tX, tY;
	char cMapName[12];
	sX = m_stDialogBoxInfo[37].sX;
	sY = m_stDialogBoxInfo[37].sY;
	szX = m_stDialogBoxInfo[37].sSizeX;
	if ((dwTime - m_dwCommanderCommandRequestedTime) > 1000 * 10)
	{
		_RequestMapStatus("middleland", 1);
		m_dwCommanderCommandRequestedTime = dwTime;
	}

	DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_CRUSADE, sX, sY - 5, 0, false, m_bDialogTrans); // Main image
	DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_TEXT, sX, sY, 16, false, m_bDialogTrans);

	switch (m_stDialogBoxInfo[37].cMode) {
	case 0: // Main dlg
		if (m_iConstructLocX != -1)
		{
			ZeroMemory(cMapName, sizeof(cMapName));
			GetOfficialMapName(m_cConstructMapName, cMapName);
			wsprintf(G_cTxt, DRAW_DIALOGBOX_CONSTRUCTOR1, cMapName, m_iConstructLocX, m_iConstructLocY);
			PutAlignedString(sX, sX + szX, sY + 40, G_cTxt);
		}
		else PutAlignedString(sX, sX + szX, sY + 40, DRAW_DIALOGBOX_CONSTRUCTOR2); //"Construction unavailable: Unable to construct"
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_CRUSADE, sX, sY, 21, false, m_bDialogTrans); // Map

		if ((msX >= sX + 20) && (msX <= sX + 20 + 46)
			&& (msY >= sY + 340) && (msY <= sY + 340 + 52))
		{
			m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20, sY + 340, 24, dwTime);
		}
		else m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20, sY + 340, 30, dwTime);

		if ((msX >= sX + 20 + 50) && (msX <= sX + 20 + 46 + 50)
			&& (msY >= sY + 340) && (msY <= sY + 340 + 52))
		{
			m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 50, sY + 340, 15, dwTime);
		}
		else m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 50, sY + 340, 1, dwTime);

		if ((msX >= sX + 20 + 150 + 74) && (msX <= sX + 20 + 46 + 150 + 74)
			&& (msY >= sY + 340) && (msY <= sY + 340 + 52))
		{
			m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 150 + 74, sY + 340, 18, dwTime);
		}
		else m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 150 + 74, sY + 340, 4, dwTime);

		if ((msX >= sX + 20) && (msX <= sX + 20 + 46)
			&& (msY >= sY + 340) && (msY <= sY + 340 + 52))
		{
			PutString2(msX + 20, msY + 35, DRAW_DIALOGBOX_CONSTRUCTOR3, 255, 255, 255);
		}
		else if ((msX >= sX + 20 + 50) && (msX <= sX + 20 + 46 + 50)
			&& (msY >= sY + 322) && (msY <= sY + 322 + 52))
		{
			PutString2(msX + 20, msY + 35, DRAW_DIALOGBOX_CONSTRUCTOR4, 255, 255, 255);
		}
		else if ((msX >= sX + 20 + 150 + 74) && (msX <= sX + 20 + 46 + 150 + 74)
			&& (msY >= sY + 322) && (msY <= sY + 322 + 52))
		{
			PutString2(msX + 20, msY + 35, DRAW_DIALOGBOX_CONSTRUCTOR5, 255, 255, 255);
		}
		break;

	case 1: // Select building
		PutAlignedString(sX, sX + szX, sY + 40, DRAW_DIALOGBOX_CONSTRUCTOR6); //"Select war building"
		PutAlignedString(sX, sX + 323, sY + 80, DRAW_DIALOGBOX_CONSTRUCTOR7); //"Constructs the war building here."
		PutAlignedString(sX, sX + 323, sY + 95, DRAW_DIALOGBOX_CONSTRUCTOR8); //"Press the building icon and"
		PutAlignedString(sX, sX + 323, sY + 110, DRAW_DIALOGBOX_CONSTRUCTOR9); //"the base will be formed and slowly finished"
		PutAlignedString(sX, sX + 323, sY + 125, DRAW_DIALOGBOX_CONSTRUCTOR10);//"Attack the building with Pickaxe."
		PutAlignedString(sX, sX + 323, sY + 140, DRAW_DIALOGBOX_CONSTRUCTOR11);//"Building is not activated"
		PutAlignedString(sX, sX + 323, sY + 155, DRAW_DIALOGBOX_CONSTRUCTOR12);//"while constructing."

		if ((msX >= sX + 20) && (msX <= sX + 20 + 46) && (msY >= sY + 220) && (msY <= sY + 220 + 50))
		{
			m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20, sY + 220, 27, dwTime);
		}
		else m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20, sY + 220, 33, dwTime);

		if ((msX >= sX + 20 + 50) && (msX <= sX + 20 + 50 + 45) && (msY >= sY + 220) && (msY <= sY + 220 + 50))
		{
			m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 50, sY + 220, 28, dwTime);
		}
		else m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 50, sY + 220, 34, dwTime);

		if ((msX >= sX + 20 + 100) && (msX <= sX + 20 + 100 + 45) && (msY >= sY + 220) && (msY <= sY + 220 + 50))
		{
			m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 100, sY + 220, 26, dwTime);
		}
		else m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 100, sY + 220, 32, dwTime);

		if ((msX >= sX + 20 + 150) && (msX <= sX + 20 + 150 + 45) && (msY >= sY + 220) && (msY <= sY + 220 + 50))
		{
			m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 150, sY + 220, 25, dwTime);
		}
		else m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 150, sY + 220, 31, dwTime);

		if ((msX >= sX + 20 + 150 + 74 - 50) && (msX <= sX + 20 + 46 + 150 + 74 - 50) && (msY >= sY + 322) && (msY <= sY + 322 + 52))
		{
			m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 100 + 74, sY + 322, 19, dwTime);
		}
		else m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 100 + 74, sY + 322, 20, dwTime);

		if ((msX >= sX + 20 + 150 + 74) && (msX <= sX + 20 + 46 + 150 + 74) && (msY >= sY + 322) && (msY <= sY + 322 + 52))
		{
			m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 150 + 74, sY + 322, 18, dwTime);
		}
		else m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 150 + 74, sY + 322, 4, dwTime);

		if ((msX >= sX + 20) && (msX <= sX + 20 + 46) && (msY >= sY + 220) && (msY <= sY + 220 + 50))
		{
			PutString2(msX + 20, msY + 35, DRAW_DIALOGBOX_CONSTRUCTOR13, 255, 255, 255);//"(Mana Collector)"
		}
		else if ((msX >= sX + 20 + 50) && (msX <= sX + 20 + 50 + 45) && (msY >= sY + 220) && (msY <= sY + 220 + 50))
		{
			PutString2(msX + 20, msY + 35, DRAW_DIALOGBOX_CONSTRUCTOR14, 255, 255, 255);//"(Detector)"
		}
		else if ((msX >= sX + 20 + 100) && (msX <= sX + 20 + 100 + 45) && (msY >= sY + 220) && (msY <= sY + 220 + 50))
		{
			PutString2(msX + 20, msY + 35, DRAW_DIALOGBOX_CONSTRUCTOR15, 255, 255, 255);//"(Arrow Guard Tower)"
		}
		else if ((msX >= sX + 20 + 150) && (msX <= sX + 20 + 150 + 45) && (msY >= sY + 220) && (msY <= sY + 220 + 50))
		{
			PutString2(msX + 20, msY + 35, DRAW_DIALOGBOX_CONSTRUCTOR16, 255, 255, 255);//(Cannon Guard Tower)"
		}
		else if ((msX >= sX + 20 + 150 + 74 - 50) && (msX <= sX + 20 + 46 + 150 + 74 - 50) && (msY >= sY + 322) && (msY <= sY + 322 + 52))
		{
			PutString2(msX + 20, msY + 35, DRAW_DIALOGBOX_CONSTRUCTOR17, 255, 255, 255);
		}
		else if ((msX >= sX + 20 + 150 + 74) && (msX <= sX + 20 + 46 + 150 + 74) && (msY >= sY + 322) && (msY <= sY + 322 + 52))
		{
			PutString2(msX + 20, msY + 35, DRAW_DIALOGBOX_CONSTRUCTOR18, 255, 255, 255);
		}
		break;

	case 2: // Teleport
		PutAlignedString(sX, sX + szX, sY + 40, DRAW_DIALOGBOX_CONSTRUCTOR19); //"Teleport to position that commander pointed"
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_CRUSADE, sX, sY, 21, false, m_bDialogTrans); // map

		if ((msX >= sX + 20 + 50) && (msX <= sX + 20 + 46 + 50)
			&& (msY >= sY + 340) && (msY <= sY + 340 + 52))
		{
			m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 50, sY + 340, 15, dwTime);
		}
		else m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 50, sY + 340, 1, dwTime);

		if ((msX >= sX + 20 + 150 + 74 - 50) && (msX <= sX + 20 + 46 + 150 + 74 - 50)
			&& (msY >= sY + 340) && (msY <= sY + 340 + 52))
		{
			m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 100 + 74, sY + 340, 19, dwTime);
		}
		else m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 100 + 74, sY + 340, 20, dwTime);

		if ((msX >= sX + 20 + 150 + 74) && (msX <= sX + 20 + 46 + 150 + 74)
			&& (msY >= sY + 340) && (msY <= sY + 340 + 52))
		{
			m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 150 + 74, sY + 340, 18, dwTime);
		}
		else m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 150 + 74, sY + 340, 4, dwTime);

		if ((msX >= sX + 20 + 50) && (msX <= sX + 20 + 46 + 50)
			&& (msY >= sY + 340) && (msY <= sY + 340 + 52))
		{
			PutString2(msX + 20, msY + 35, DRAW_DIALOGBOX_CONSTRUCTOR20, 255, 255, 255);
		}
		else if ((msX >= sX + 20 + 150 + 74 - 50) && (msX <= sX + 20 + 46 + 150 + 74 - 50)
			&& (msY >= sY + 340) && (msY <= sY + 340 + 52))
		{
			PutString2(msX + 20, msY + 35, DRAW_DIALOGBOX_CONSTRUCTOR21, 255, 255, 255);
		}
		else if ((msX >= sX + 20 + 150 + 74) && (msX <= sX + 20 + 46 + 150 + 74)
			&& (msY >= sY + 340) && (msY <= sY + 340 + 52))
		{
			PutString2(msX + 20, msY + 35, DRAW_DIALOGBOX_CONSTRUCTOR22, 255, 255, 255);
		}
		break;
	}

	switch (m_stDialogBoxInfo[37].cMode) {
	case 0: // Main dlg
	case 2: // TP
		szX = 0;
		szY = 0;
		MapSzX = 0;
		MapSzY = 0;
		if (strcmp(m_cStatusMapName, "aresden") == 0)
		{
			szX = 250;
			szY = 250;
		}
		else if (strcmp(m_cStatusMapName, "elvine") == 0)
		{
			szX = 250;
			szY = 250;
		}
		else if (strcmp(m_cStatusMapName, "middleland") == 0)
		{
			szX = 279;
			szY = 280;
			MapSzX = 524;
			MapSzY = 524;
		}
		if (szX != 0)
		{
			for (int i = 0; i < DEF_MAXCRUSADESTRUCTURES; i++)
				if (m_stCrusadeStructureInfo[i].cType == 42)
				{
					dV1 = (float)MapSzX;
					dV2 = (float)m_stCrusadeStructureInfo[i].sX;
					dV3 = (dV2*(float)szX) / dV1;
					tX = (int)dV3;
					dV1 = (float)MapSzY;
					dV2 = (float)m_stCrusadeStructureInfo[i].sY;
					dV3 = (dV2*(float)szY) / dV1;
					tY = (int)dV3;
					switch (m_stCrusadeStructureInfo[i].cType) {
					case 42:
						DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_CRUSADE, sX + tX + 15, sY + tY + 60, 40); // ManaStone
						break;
					}
				}
			if (m_iTeleportLocX != -1)
			{
				dV1 = (float)MapSzX;
				dV2 = (float)m_iTeleportLocX;
				dV3 = (dV2*(float)szX) / dV1;
				tX = (int)dV3;
				dV1 = (float)MapSzY;
				dV2 = (float)m_iTeleportLocY;
				dV3 = (dV2*(float)szY) / dV1;
				tY = (int)dV3;
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_CRUSADE, sX + tX + 15, sY + tY + 60, 42, false, true); //TP
			}
			if ((m_stDialogBoxInfo[37].cMode != 2) && (m_iConstructLocX != -1))
			{
				dV1 = (float)MapSzX;
				dV2 = (float)m_iConstructLocX;
				dV3 = (dV2*(float)szX) / dV1;
				tX = (int)dV3;
				dV1 = (float)MapSzY;
				dV2 = (float)m_iConstructLocY;
				dV3 = (dV2*(float)szY) / dV1;
				tY = (int)dV3;
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_CRUSADE, sX + tX + 15, sY + tY + 60, 41, false, true); // Contr pt
			}
			if (strcmp(m_cMapName, "middleland") == 0)
			{
				dV1 = (float)MapSzX;
				dV2 = (float)m_sPlayerX;
				dV3 = (dV2*(float)szX) / dV1;
				tX = (int)dV3;
				dV1 = (float)MapSzY;
				dV2 = (float)m_sPlayerY;
				dV3 = (dV2*(float)szY) / dV1;
				tY = (int)dV3;
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_CRUSADE, sX + tX + 15, sY + tY + 60, 43); // Player place
			}
		}
		if ((msX >= sX + 15) && (msX <= sX + 15 + 278)
			&& (msY >= sY + 60) && (msY <= sY + 60 + 272))  // Draw mouse coordinates
		{
			dV1 = (float)(msX - (sX + 15));
			dV2 = (float)MapSzX;
			dV3 = (dV2*dV1) / szX;
			tX = (int)dV3;
			dV1 = (float)(msY - (sY + 60));
			dV2 = (float)MapSzX;
			dV3 = (dV2*dV1) / szY;
			tY = (int)dV3;
			if (tX < 30) tX = 30;
			if (tY < 30) tY = 30;
			if (tX > MapSzX - 30) tX = MapSzX - 30;
			if (tY > MapSzY - 30) tY = MapSzY - 30;
			wsprintf(G_cTxt, "%d,%d", tX, tY);
			PutString_SprFont3(msX + 10, msY - 10, G_cTxt, m_wR[13] * 4, m_wG[13] * 4, m_wB[13] * 4, false, 2);
		}
		break;
	}
}

void CGame::DrawDialogBox_Soldier(int msX, int msY) // Snoopy: Fixed for 351
{
	short sX, sY, szX, szY, MapSzX, MapSzY;
	UINT32 dwTime = G_dwGlobalTime;
	char cMapName[120];
	float dV1, dV2, dV3;
	int tX, tY;
	sX = m_stDialogBoxInfo[38].sX;
	sY = m_stDialogBoxInfo[38].sY;
	szX = m_stDialogBoxInfo[38].sSizeX;
	if ((dwTime - m_dwCommanderCommandRequestedTime) > 1000 * 10)
	{
		_RequestMapStatus("middleland", 1);
		m_dwCommanderCommandRequestedTime = dwTime;
	}

	DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_CRUSADE, sX, sY - 5, 0, false, m_bDialogTrans);
	DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_CRUSADE, sX, sY, 21, false, m_bDialogTrans);
	DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_TEXT, sX, sY, 17, false, m_bDialogTrans); // Crusade Soldier Menu Text

	switch (m_stDialogBoxInfo[38].cMode) {
	case 0: // Main dlg, Map
		if (m_iTeleportLocX != -1)
		{
			ZeroMemory(cMapName, sizeof(cMapName));
			GetOfficialMapName(m_cTeleportMapName, cMapName);
			wsprintf(G_cTxt, DRAW_DIALOGBOX_SOLDIER1, cMapName, m_iTeleportLocX, m_iTeleportLocY);
			PutAlignedString(sX, sX + szX, sY + 40, G_cTxt);					// "Teleport position: %s near %d, %d"
		}
		else PutAlignedString(sX, sX + szX, sY + 40, DRAW_DIALOGBOX_SOLDIER2); //"Teleport position is not set."

		if ((msX >= sX + 20) && (msX <= sX + 20 + 46)
			&& (msY >= sY + 340) && (msY <= sY + 340 + 52))
		{
			m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20, sY + 340, 15, dwTime);
		}
		else m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20, sY + 340, 1, dwTime);

		if ((msX >= sX + 20 + 150 + 74) && (msX <= sX + 20 + 46 + 150 + 74)
			&& (msY >= sY + 340) && (msY <= sY + 340 + 52))
		{
			m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 150 + 74, sY + 340, 18, dwTime);
		}
		else m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 150 + 74, sY + 340, 4, dwTime);

		if ((msX >= sX + 20) && (msX <= sX + 20 + 46)
			&& (msY >= sY + 340) && (msY <= sY + 340 + 52))
		{
			PutString2(msX + 20, msY + 35, DRAW_DIALOGBOX_SOLDIER3, 255, 255, 255); // "Teleport to position that commander pointed"
		}
		else if ((msX >= sX + 20 + 150 + 74) && (msX <= sX + 20 + 46 + 150 + 74)
			&& (msY >= sY + 340) && (msY <= sY + 340 + 52))
		{
			PutString2(msX + 20, msY + 35, DRAW_DIALOGBOX_SOLDIER4, 255, 255, 255); // "Help for soldier"
		}
		break;

	case 1: // TP now
		PutAlignedString(sX, sX + szX, sY + 40, DRAW_DIALOGBOX_SOLDIER5); // "Teleport to position that commander pointed"
		if ((msX >= sX + 20) && (msX <= sX + 20 + 46)
			&& (msY >= sY + 340) && (msY <= sY + 340 + 52))
		{
			m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20, sY + 340, 15, dwTime);
		}
		else m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20, sY + 340, 1, dwTime);

		if ((msX >= sX + 20 + 150 + 74 - 50) && (msX <= sX + 20 + 46 + 150 + 74 - 50)
			&& (msY >= sY + 340) && (msY <= sY + 340 + 52))
		{
			m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 150 + 74 - 50, sY + 340, 19, dwTime);
		}
		else m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 150 + 74 - 50, sY + 340, 20, dwTime);

		if ((msX >= sX + 20 + 150 + 74) && (msX <= sX + 20 + 46 + 150 + 74)
			&& (msY >= sY + 340) && (msY <= sY + 340 + 52))
		{
			m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 150 + 74, sY + 340, 18, dwTime);
		}
		else m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + 20 + 150 + 74, sY + 340, 4, dwTime);

		if ((msX >= sX + 20) && (msX <= sX + 20 + 46)
			&& (msY >= sY + 340) && (msY <= sY + 340 + 52))
		{
			PutString2(msX + 20, msY + 35, DRAW_DIALOGBOX_SOLDIER6, 255, 255, 255);
		}
		else if ((msX >= sX + 20 + 150 + 74 - 50) && (msX <= sX + 20 + 46 + 150 + 74 - 50)
			&& (msY >= sY + 340) && (msY <= sY + 340 + 52))
		{
			PutString2(msX + 20, msY + 35, DRAW_DIALOGBOX_SOLDIER7, 255, 255, 255);
		}
		else if ((msX >= sX + 20 + 150 + 74) && (msX <= sX + 20 + 46 + 150 + 74)
			&& (msY >= sY + 340) && (msY <= sY + 340 + 52))
		{
			PutString2(msX + 20, msY + 35, DRAW_DIALOGBOX_SOLDIER8, 255, 255, 255);//"Help for teleport"
		}
		break;
	}

	switch (m_stDialogBoxInfo[38].cMode) {
	case 0: // Main
	case 1: // TP
		szX = 0;
		szY = 0;
		MapSzX = 0;
		MapSzY = 0;
		if (strcmp(m_cStatusMapName, "aresden") == 0)
		{
			szX = 250;
			szY = 250;
		}
		else if (strcmp(m_cStatusMapName, "elvine") == 0)
		{
			szX = 250;
			szY = 250;
		}
		else if (strcmp(m_cStatusMapName, "middleland") == 0)
		{
			szX = 279;
			szY = 280;	  // Fix for ML
			MapSzX = 524; // Fix for ML
			MapSzY = 524; // Fix for ML
		}
		if (szX != 0)
		{
			for (int i = 0; i < DEF_MAXCRUSADESTRUCTURES; i++)
				if (m_stCrusadeStructureInfo[i].cType == 42)
				{
					dV1 = (float)MapSzX;
					dV2 = (float)m_stCrusadeStructureInfo[i].sX;
					dV3 = (dV2*(float)szX) / dV1;
					tX = (int)dV3;
					dV1 = (float)MapSzY;
					dV2 = (float)m_stCrusadeStructureInfo[i].sY;
					dV3 = (dV2*(float)szY) / dV1;
					tY = (int)dV3;
					switch (m_stCrusadeStructureInfo[i].cType) {
					case 42:
						DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_CRUSADE, sX + tX + 15, sY + tY + 60, 40); // ManaStone
						break;
					}
				}
			if (m_iTeleportLocX != -1)
			{
				dV1 = (float)MapSzX;
				dV2 = (float)m_iTeleportLocX;
				dV3 = (dV2*(float)szX) / dV1;
				tX = (int)dV3;
				dV1 = (float)MapSzY;
				dV2 = (float)m_iTeleportLocY;
				dV3 = (dV2*(float)szY) / dV1;
				tY = (int)dV3;
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_CRUSADE, sX + tX + 15, sY + tY + 60, 42, false, true); //TP
			}
			if (strcmp(m_cMapName, "middleland") == 0)
			{
				dV1 = (float)MapSzX;
				dV2 = (float)m_sPlayerX;
				dV3 = (dV2*(float)szX) / dV1;
				tX = (int)dV3;
				dV1 = (float)MapSzY;
				dV2 = (float)m_sPlayerY;
				dV3 = (dV2*(float)szY) / dV1;
				tY = (int)dV3;
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_CRUSADE, sX + tX + 15, sY + tY + 60, 43); // Player place
			}
		}
		if ((msX >= sX + 15) && (msX <= sX + 15 + 278)
			&& (msY >= sY + 60) && (msY <= sY + 60 + 272))  // Draw mouse coordinates
		{
			dV1 = (float)(msX - (sX + 15));
			dV2 = (float)MapSzX;
			dV3 = (dV2*dV1) / szX;
			tX = (int)dV3;
			dV1 = (float)(msY - (sY + 60));
			dV2 = (float)MapSzX;
			dV3 = (dV2*dV1) / szY;
			tY = (int)dV3;
			if (tX < 30) tX = 30;
			if (tY < 30) tY = 30;
			if (tX > MapSzX - 30) tX = MapSzX - 30;
			if (tY > MapSzY - 30) tY = MapSzY - 30;
			wsprintf(G_cTxt, "%d,%d", tX, tY);
			PutString_SprFont3(msX + 10, msY - 10, G_cTxt, m_wR[13] * 4, m_wG[13] * 4, m_wB[13] * 4, false, 2);
		}
		break;
	}
}

void CGame::GrandMagicResult(char *pMapName, int iV1, int iV2, int iV3, int iV4, int iHP1, int iHP2, int iHP3, int iHP4)
{
	int i, iTxtIdx = 0;
	char cTemp[120];

	for (i = 0; i < DEF_TEXTDLGMAXLINES; i++)
	{
		if (m_pMsgTextList[i] != 0)
			delete m_pMsgTextList[i];
		m_pMsgTextList[i] = 0;
	}

	for (i = 0; i < 92; i++)
		if (m_pGameMsgList[i] == 0) return;

	if (strcmp(pMapName, "aresden") == 0)
	{
		m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[2]->m_pMsg, 0);
		m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[3]->m_pMsg, 0);
		m_pMsgTextList[iTxtIdx++] = new class CMsg(0, " ", 0);

		ZeroMemory(cTemp, sizeof(cTemp));
		wsprintf(cTemp, "%s %d", m_pGameMsgList[4]->m_pMsg, iV1);
		m_pMsgTextList[iTxtIdx++] = new class CMsg(0, cTemp, 0);

		ZeroMemory(cTemp, sizeof(cTemp));
		wsprintf(cTemp, "%s %d", m_pGameMsgList[5]->m_pMsg, iV2);
		m_pMsgTextList[iTxtIdx++] = new class CMsg(0, cTemp, 0);

		ZeroMemory(cTemp, sizeof(cTemp));
		wsprintf(cTemp, "%s %d", m_pGameMsgList[6]->m_pMsg, iV3);
		m_pMsgTextList[iTxtIdx++] = new class CMsg(0, cTemp, 0);

		ZeroMemory(cTemp, sizeof(cTemp));
		wsprintf(cTemp, "%s %d", m_pGameMsgList[58]->m_pMsg, iV4);
		m_pMsgTextList[iTxtIdx++] = new class CMsg(0, cTemp, 0);
		m_pMsgTextList[iTxtIdx++] = new class CMsg(0, " ", 0);

		ZeroMemory(cTemp, sizeof(cTemp));
		wsprintf(cTemp, "%s %d %d %d %d", NOTIFY_MSG_STRUCTURE_HP, iHP1, iHP2, iHP3, iHP4);
		m_pMsgTextList[iTxtIdx++] = new class CMsg(0, cTemp, 0);
		m_pMsgTextList[iTxtIdx++] = new class CMsg(0, " ", 0);

		if (iV2 == 0) {
			if ((m_bCitizen == true) && (m_bAresden == false))
			{
				PlaySound('E', 25, 0, 0);
				m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[59]->m_pMsg, 0);
				m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[60]->m_pMsg, 0);
				m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[61]->m_pMsg, 0);
				m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[62]->m_pMsg, 0);
				for (i = iTxtIdx; i < 18; i++) m_pMsgTextList[i] = new class CMsg(0, " ", 0);
			}
			else if ((m_bCitizen == true) && (m_bAresden == true))
			{
				PlaySound('E', 25, 0, 0);
				m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[69]->m_pMsg, 0);
				m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[70]->m_pMsg, 0);
				m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[71]->m_pMsg, 0);
				m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[72]->m_pMsg, 0);
				m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[73]->m_pMsg, 0);
				m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[74]->m_pMsg, 0);
				for (i = iTxtIdx; i < 18; i++) m_pMsgTextList[i] = new class CMsg(0, " ", 0);
			}
			else PlaySound('E', 25, 0, 0);
		}
		else
		{
			if (iV1 != 0)
			{
				if ((m_bCitizen == true) && (m_bAresden == false))
				{
					PlaySound('E', 23, 0, 0);
					PlaySound('C', 21, 0, 0);
					PlaySound('C', 22, 0, 0);
					m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[63]->m_pMsg, 0);
					m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[64]->m_pMsg, 0);
					m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[65]->m_pMsg, 0);
					for (i = iTxtIdx; i < 18; i++) m_pMsgTextList[i] = new class CMsg(0, " ", 0);
				}
				else if ((m_bCitizen == true) && (m_bAresden == true))
				{
					PlaySound('E', 24, 0, 0);
					PlaySound('C', 12, 0, 0);
					PlaySound('C', 13, 0, 0);
					m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[75]->m_pMsg, 0);
					m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[76]->m_pMsg, 0);
					m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[77]->m_pMsg, 0);
					m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[78]->m_pMsg, 0);
					m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[79]->m_pMsg, 0);
					m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[80]->m_pMsg, 0);
					m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[81]->m_pMsg, 0);
					m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[82]->m_pMsg, 0);
					for (i = iTxtIdx; i < 18; i++) m_pMsgTextList[i] = new class CMsg(0, " ", 0);
				}
				else PlaySound('E', 25, 0, 0);
			}
			else
			{
				if ((m_bCitizen == true) && (m_bAresden == false))
				{
					PlaySound('E', 23, 0, 0);
					m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[66]->m_pMsg, 0);
					m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[67]->m_pMsg, 0);
					m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[68]->m_pMsg, 0);
					for (i = iTxtIdx; i < 18; i++) m_pMsgTextList[i] = new class CMsg(0, " ", 0);
				}
				else if ((m_bCitizen == true) && (m_bAresden == true))
				{
					PlaySound('E', 24, 0, 0);
					m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[83]->m_pMsg, 0);
					m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[84]->m_pMsg, 0);
					m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[85]->m_pMsg, 0);
					m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[86]->m_pMsg, 0);
					m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[87]->m_pMsg, 0);
					m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[88]->m_pMsg, 0);
					m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[89]->m_pMsg, 0);
					m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[90]->m_pMsg, 0);
					for (i = iTxtIdx; i < 18; i++) m_pMsgTextList[i] = new class CMsg(0, " ", 0);
				}
				else PlaySound('E', 25, 0, 0);
			}
		}
	}
	else if (strcmp(pMapName, "elvine") == 0)
	{
		m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[7]->m_pMsg, 0);
		m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[8]->m_pMsg, 0);
		m_pMsgTextList[iTxtIdx++] = new class CMsg(0, " ", 0);

		ZeroMemory(cTemp, sizeof(cTemp));
		wsprintf(cTemp, "%s %d", m_pGameMsgList[4]->m_pMsg, iV1);
		m_pMsgTextList[iTxtIdx++] = new class CMsg(0, cTemp, 0);

		ZeroMemory(cTemp, sizeof(cTemp));
		wsprintf(cTemp, "%s %d", m_pGameMsgList[5]->m_pMsg, iV2);
		m_pMsgTextList[iTxtIdx++] = new class CMsg(0, cTemp, 0);

		ZeroMemory(cTemp, sizeof(cTemp));
		wsprintf(cTemp, "%s %d", m_pGameMsgList[6]->m_pMsg, iV3);
		m_pMsgTextList[iTxtIdx++] = new class CMsg(0, cTemp, 0);

		ZeroMemory(cTemp, sizeof(cTemp));
		wsprintf(cTemp, "%s %d", m_pGameMsgList[58]->m_pMsg, iV4);
		m_pMsgTextList[iTxtIdx++] = new class CMsg(0, cTemp, 0);
		m_pMsgTextList[iTxtIdx++] = new class CMsg(0, " ", 0);

		ZeroMemory(cTemp, sizeof(cTemp));
		wsprintf(cTemp, "%s %d %d %d %d", NOTIFY_MSG_STRUCTURE_HP, iHP1, iHP2, iHP3, iHP4);
		m_pMsgTextList[iTxtIdx++] = new class CMsg(0, cTemp, 0);
		m_pMsgTextList[iTxtIdx++] = new class CMsg(0, " ", 0);

		if (iV2 == 0) {
			if ((m_bCitizen == true) && (m_bAresden == true))
			{
				PlaySound('E', 25, 0, 0);
				m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[59]->m_pMsg, 0);
				m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[60]->m_pMsg, 0);
				m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[61]->m_pMsg, 0);
				m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[62]->m_pMsg, 0);
				for (i = iTxtIdx; i < 18; i++) m_pMsgTextList[i] = new class CMsg(0, " ", 0);
			}
			else if ((m_bCitizen == true) && (m_bAresden == false))
			{
				PlaySound('E', 25, 0, 0);
				m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[69]->m_pMsg, 0);
				m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[70]->m_pMsg, 0);
				m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[71]->m_pMsg, 0);
				m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[72]->m_pMsg, 0);
				m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[73]->m_pMsg, 0);
				m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[74]->m_pMsg, 0);
				for (i = iTxtIdx; i < 18; i++) m_pMsgTextList[i] = new class CMsg(0, " ", 0);
			}
			else PlaySound('E', 25, 0, 0);
		}
		else
		{
			if (iV1 != 0) {
				if ((m_bCitizen == true) && (m_bAresden == true))
				{
					PlaySound('E', 23, 0, 0);
					PlaySound('C', 21, 0, 0);
					PlaySound('C', 22, 0, 0);
					m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[63]->m_pMsg, 0);
					m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[64]->m_pMsg, 0);
					m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[65]->m_pMsg, 0);
					for (i = iTxtIdx; i < 18; i++) m_pMsgTextList[i] = new class CMsg(0, " ", 0);
				}
				else if ((m_bCitizen == true) && (m_bAresden == false))
				{
					PlaySound('E', 24, 0, 0);
					PlaySound('C', 12, 0, 0);
					PlaySound('C', 13, 0, 0);
					m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[75]->m_pMsg, 0);
					m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[76]->m_pMsg, 0);
					m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[77]->m_pMsg, 0);
					m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[78]->m_pMsg, 0);
					m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[79]->m_pMsg, 0);
					m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[80]->m_pMsg, 0);
					m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[81]->m_pMsg, 0);
					m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[82]->m_pMsg, 0);
					for (i = iTxtIdx; i < 18; i++) m_pMsgTextList[i] = new class CMsg(0, " ", 0);
				}
				else PlaySound('E', 25, 0, 0);
			}
			else
			{
				if ((m_bCitizen == true) && (m_bAresden == true))
				{
					PlaySound('E', 23, 0, 0);
					m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[66]->m_pMsg, 0);
					m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[67]->m_pMsg, 0);
					m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[68]->m_pMsg, 0);
					for (i = iTxtIdx; i < 18; i++) m_pMsgTextList[i] = new class CMsg(0, " ", 0);
				}
				else if ((m_bCitizen == true) && (m_bAresden == false))
				{
					PlaySound('E', 24, 0, 0);
					m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[83]->m_pMsg, 0);
					m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[84]->m_pMsg, 0);
					m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[85]->m_pMsg, 0);
					m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[86]->m_pMsg, 0);
					m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[87]->m_pMsg, 0);
					m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[88]->m_pMsg, 0);
					m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[89]->m_pMsg, 0);
					m_pMsgTextList[iTxtIdx++] = new class CMsg(0, m_pGameMsgList[90]->m_pMsg, 0);
					for (i = iTxtIdx; i < 18; i++) m_pMsgTextList[i] = new class CMsg(0, " ", 0);
				}
				else PlaySound('E', 25, 0, 0);
			}
		}
	}

	EnableDialogBox(18, 0, 0, 0);
}

void CGame::DrawDialogBox_Help(int msX, int msY)
{
	short sX, sY, szX;

	sX = m_stDialogBoxInfo[35].sX;
	sY = m_stDialogBoxInfo[35].sY;
	szX = m_stDialogBoxInfo[35].sSizeX;

	if (m_bUseOldPanels) {
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME2, sX, sY, 2);
	}
	else {
		short limitX, limitY;
		limitX = sX + m_stDialogBoxInfo[35].sSizeX;
		limitY = sY + m_stDialogBoxInfo[35].sSizeY;

		m_DDraw.DrawShadowBox(sX, sY, limitX, limitY, 0, true);
		if (!m_bDialogTrans) m_DDraw.DrawShadowBox(sX, sY, limitX, limitY, 0, true);

		m_DDraw.DrawShadowBox(sX, sY, limitX, sY + 25, 0, true);
		if (!m_bDialogTrans) m_DDraw.DrawShadowBox(sX, sY, limitX, sY + 25, 0, true);
		PutString_SprFont2(sX + 85, sY + 5, "Information", 240, 240, 240);
	}

	if ((msX >= sX + 25) && (msX <= sX + 248) && (msY >= sY + 50 + 15 * 0) && (msY < sY + 50 + 15 * 1))
		PutAlignedString(sX, sX + szX, sY + 50 + 15 * 0, DRAW_DIALOGBOX_HELP1, 255, 255, 255);
	else PutAlignedString(sX, sX + szX, sY + 50 + 15 * 0, DRAW_DIALOGBOX_HELP1, 19, 104, 169);
	if ((msX >= sX + 25) && (msX <= sX + 248) && (msY >= sY + 50 + 15 * 1) && (msY < sY + 50 + 15 * 2))
		PutAlignedString(sX, sX + szX, sY + 50 + 15 * 1, DRAW_DIALOGBOX_HELP2, 255, 255, 255);
	else PutAlignedString(sX, sX + szX, sY + 50 + 15 * 1, DRAW_DIALOGBOX_HELP2, 19, 104, 169);
	if ((msX >= sX + 25) && (msX <= sX + 248) && (msY >= sY + 50 + 15 * 2) && (msY < sY + 50 + 15 * 3))
		PutAlignedString(sX, sX + szX, sY + 50 + 15 * 2, DRAW_DIALOGBOX_HELP3, 255, 255, 255);
	else PutAlignedString(sX, sX + szX, sY + 50 + 15 * 2, DRAW_DIALOGBOX_HELP3, 19, 104, 169);
	if ((msX >= sX + 25) && (msX <= sX + 248) && (msY >= sY + 50 + 15 * 3) && (msY < sY + 50 + 15 * 4))
		PutAlignedString(sX, sX + szX, sY + 50 + 15 * 3, DRAW_DIALOGBOX_HELP4, 255, 255, 255);
	else PutAlignedString(sX, sX + szX, sY + 50 + 15 * 3, DRAW_DIALOGBOX_HELP4, 19, 104, 169);
	if ((msX >= sX + 25) && (msX <= sX + 248) && (msY >= sY + 50 + 15 * 4) && (msY < sY + 50 + 15 * 5))
		PutAlignedString(sX, sX + szX, sY + 50 + 15 * 4, DRAW_DIALOGBOX_HELP5, 255, 255, 255);
	else PutAlignedString(sX, sX + szX, sY + 50 + 15 * 4, DRAW_DIALOGBOX_HELP5, 19, 104, 169);
	if ((msX >= sX + 25) && (msX <= sX + 248) && (msY >= sY + 50 + 15 * 5) && (msY < sY + 50 + 15 * 6))
		PutAlignedString(sX, sX + szX, sY + 50 + 15 * 5, DRAW_DIALOGBOX_HELP6, 255, 255, 255);
	else PutAlignedString(sX, sX + szX, sY + 50 + 15 * 5, DRAW_DIALOGBOX_HELP6, 19, 104, 169);
	if ((msX >= sX + 25) && (msX <= sX + 248) && (msY >= sY + 50 + 15 * 6) && (msY < sY + 50 + 15 * 7))
		PutAlignedString(sX, sX + szX, sY + 50 + 15 * 6, DRAW_DIALOGBOX_HELP7, 255, 255, 255);
	else PutAlignedString(sX, sX + szX, sY + 50 + 15 * 6, DRAW_DIALOGBOX_HELP7, 19, 104, 169);
	if ((msX >= sX + 25) && (msX <= sX + 248) && (msY >= sY + 50 + 15 * 7) && (msY < sY + 50 + 15 * 8))
		PutAlignedString(sX, sX + szX, sY + 50 + 15 * 7, DRAW_DIALOGBOX_HELP8, 255, 255, 255);
	else PutAlignedString(sX, sX + szX, sY + 50 + 15 * 7, DRAW_DIALOGBOX_HELP8, 19, 104, 169);
	if ((msX >= sX + 25) && (msX <= sX + 248) && (msY >= sY + 50 + 15 * 8) && (msY < sY + 50 + 15 * 9))
		PutAlignedString(sX, sX + szX, sY + 50 + 15 * 8, DRAW_DIALOGBOX_HELP9, 255, 255, 255);
	else PutAlignedString(sX, sX + szX, sY + 50 + 15 * 8, DRAW_DIALOGBOX_HELP9, 19, 104, 169);
	if ((msX >= sX + 25) && (msX <= sX + 248) && (msY >= sY + 50 + 15 * 9) && (msY < sY + 50 + 15 * 10))
		PutAlignedString(sX, sX + szX, sY + 50 + 15 * 9, DRAW_DIALOGBOX_HELP10, 255, 255, 255);
	else PutAlignedString(sX, sX + szX, sY + 50 + 15 * 9, DRAW_DIALOGBOX_HELP10, 19, 104, 169);
	if ((msX >= sX + 25) && (msX <= sX + 248) && (msY >= sY + 50 + 15 * 10) && (msY < sY + 50 + 15 * 11))
		PutAlignedString(sX, sX + szX, sY + 50 + 15 * 10, DRAW_DIALOGBOX_HELP11, 255, 255, 255);
	else PutAlignedString(sX, sX + szX, sY + 50 + 15 * 10, DRAW_DIALOGBOX_HELP11, 19, 104, 169);
	if ((msX >= sX + 25) && (msX <= sX + 248) && (msY >= sY + 50 + 15 * 11) && (msY < sY + 50 + 15 * 12))
		PutAlignedString(sX, sX + szX, sY + 50 + 15 * 11, DRAW_DIALOGBOX_HELP12, 255, 255, 255);
	else PutAlignedString(sX, sX + szX, sY + 50 + 15 * 11, DRAW_DIALOGBOX_HELP12, 19, 104, 169);
	if ((msX >= sX + 25) && (msX <= sX + 248) && (msY >= sY + 50 + 15 * 12) && (msY < sY + 50 + 15 * 13))
		PutAlignedString(sX, sX + szX, sY + 50 + 15 * 12, "F.A.Q.", 255, 255, 255);
	else PutAlignedString(sX, sX + szX, sY + 50 + 15 * 12, "F.A.Q.", 19, 104, 169);
	if ((msX >= sX + 25) && (msX <= sX + 248) && (msY >= sY + 50 + 15 * 13) && (msY < sY + 50 + 15 * 14))
		PutAlignedString(sX, sX + szX, sY + 50 + 15 * 13, DRAW_DIALOGBOX_HELP13, 255, 255, 255);
	else PutAlignedString(sX, sX + szX, sY + 50 + 15 * 13, DRAW_DIALOGBOX_HELP13, 19, 104, 169);

	if (m_bUseOldPanels) {
		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
	}
}

void CGame::DrawDialogBox_ItemUpgrade(int msX, int msY)
{
	int i, sX, sY, iValue;
	char cItemColor, cStr1[120], cStr2[120], cStr3[120], cStr4[120], cStr5[120], cStr6[120];
	UINT32 dwTime = timeGetTime();

	sX = m_stDialogBoxInfo[34].sX;
	sY = m_stDialogBoxInfo[34].sY;
	
	if (m_bUseOldPanels) {
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME6, sX, sY, 0);
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_TEXT2, sX, sY, 5); //Item Upgrade Text
		if (m_stDialogBoxInfo[34].cMode != 5 && m_stDialogBoxInfo[34].cMode < 7) {
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME7, sX, sY, 3); // recuadro x1 ok y1 ok x2 ok y2 ok
			//m_DDraw.DrawShadowBox(sX + 20 - 5 + 90 - 2 - 5 - 4, sY + 90 + 10 + 24 + 10 - 1, sX + 50 + 5 + 80 + 40, sY + 145 + 102, 1, true);
		}
	}
	else {
		short limitX, limitY;
		limitX = sX + m_stDialogBoxInfo[34].sSizeX;
		limitY = sY + m_stDialogBoxInfo[34].sSizeY;

		m_DDraw.DrawShadowBox(sX, sY, limitX, limitY, 0, true);
		if (!m_bDialogTrans) m_DDraw.DrawShadowBox(sX, sY, limitX, limitY, 0, true);

		m_DDraw.DrawShadowBox(sX, sY, limitX, sY + 25, 0, true);
		if (!m_bDialogTrans) m_DDraw.DrawShadowBox(sX, sY, limitX, sY + 25, 0, true);
		PutString_SprFont2(sX + 85, sY + 5, "Item Upgrade", 240, 240, 240);

		if (m_stDialogBoxInfo[34].cMode != 5 && m_stDialogBoxInfo[34].cMode < 7) {
			//DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME3, sX, sY, 3); // recuadro x1 ok y1 ok x2 ok y2 ok
			m_DDraw.DrawShadowBox(sX + 20 - 5 + 90 - 2 - 5 - 4, sY + 90 + 10 + 24 + 10 - 1, sX + 50 + 5 + 80 + 40, sY + 145 + 102, 1, true);
		}
	}

	switch (m_stDialogBoxInfo[34].cMode) {
	case 1://Gizon box Drag item needed to be upgraded"
		
		PutAlignedString(sX + 24, sX + 248, sY + 20 + 30, DRAW_DIALOGBOX_ITEMUPGRADE1,255,255,255); // Drag item needed to be upgraded"
		PutAlignedString(sX + 24, sX + 248, sY + 20 + 45, DRAW_DIALOGBOX_ITEMUPGRADE2, 255, 255, 255); // "from the inventory. Then press"
		PutAlignedString(sX + 24, sX + 248, sY + 20 + 60, DRAW_DIALOGBOX_ITEMUPGRADE3, 255, 255, 255); // 'Upgrade' button."
		//DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 46);
		wsprintf(G_cTxt, DRAW_DIALOGBOX_ITEMUPGRADE11, m_iGizonItemUpgradeLeft);		// "Item upgrade point : %d"
		PutAlignedString(sX + 24, sX + 248, sY + 100, G_cTxt, 255, 255, 255);
		if (m_stDialogBoxInfo[34].sV1 != -1)
		{
			//DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME3, sX, sY, 3);
			iValue = (m_pItemList[m_stDialogBoxInfo[34].sV1]->m_dwAttribute & 0xF0000000) >> 28;
			if ((m_pItemList[m_stDialogBoxInfo[34].sV1]->m_cEquipPos >= 11)
				&& (m_pItemList[m_stDialogBoxInfo[34].sV1]->m_cItemType == 1))
			{
				if ((memcmp(m_pItemList[m_stDialogBoxInfo[34].sV1]->m_cName, "AngelicPandent(STR)", 19) == 0)
					|| (memcmp(m_pItemList[m_stDialogBoxInfo[34].sV1]->m_cName, "AngelicPandent(DEX)", 19) == 0)
					|| (memcmp(m_pItemList[m_stDialogBoxInfo[34].sV1]->m_cName, "AngelicPandent(INT)", 19) == 0)
					|| (memcmp(m_pItemList[m_stDialogBoxInfo[34].sV1]->m_cName, "AngelicPandent(MAG)", 19) == 0))
				{
					switch (iValue) {
					case 0:	iValue = 10*2; break;
					case 1: iValue = 11*2; break;
					case 2: iValue = 13*2; break;
					case 3: iValue = 16*2; break;
					case 4: iValue = 20*2; break;
					case 5: iValue = 25*2; break;
					case 6: iValue = 31*2; break;
					case 7: iValue = 38*2; break;
					case 8: iValue = 46*2; break;
					case 9: iValue = 55*2; break;
					}
				}
			}
			else iValue = iValue * (iValue + 6) / 8 + 2;
			wsprintf(G_cTxt, DRAW_DIALOGBOX_ITEMUPGRADE12, iValue); //"Needed upgrade point : %d"
			if (m_iGizonItemUpgradeLeft < iValue)
				PutAlignedString(sX + 24, sX + 248, sY + 115, G_cTxt, 195, 25, 25);
			else PutAlignedString(sX + 24, sX + 248, sY + 115, G_cTxt, 255, 255, 255);
			i = m_stDialogBoxInfo[34].sV1;
			cItemColor = m_pItemList[m_stDialogBoxInfo[34].sV1]->m_cItemColor;
			if ((m_pItemList[m_stDialogBoxInfo[34].sV1]->m_cEquipPos == DEF_EQUIPPOS_LHAND)
				|| (m_pItemList[m_stDialogBoxInfo[34].sV1]->m_cEquipPos == DEF_EQUIPPOS_RHAND)
				|| (m_pItemList[m_stDialogBoxInfo[34].sV1]->m_cEquipPos == DEF_EQUIPPOS_TWOHAND))
			{
				m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_pItemList[m_stDialogBoxInfo[34].sV1]->m_sSprite]->PutSpriteRGB(sX + 134, sY + 182, m_pItemList[m_stDialogBoxInfo[34].sV1]->m_sSpriteFrame, m_wWR[cItemColor] - m_wR[0], m_wWG[cItemColor] - m_wG[0], m_wWB[cItemColor] - m_wB[0], dwTime);
			}
			else
			{
				m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_pItemList[m_stDialogBoxInfo[34].sV1]->m_sSprite]->PutSpriteRGB(sX + 134, sY + 182, m_pItemList[m_stDialogBoxInfo[34].sV1]->m_sSpriteFrame, m_wR[cItemColor] - m_wR[0], m_wG[cItemColor] - m_wG[0], m_wB[cItemColor] - m_wB[0], dwTime);
			}
			ZeroMemory(cStr1, sizeof(cStr1));
			ZeroMemory(cStr2, sizeof(cStr2));
			ZeroMemory(cStr3, sizeof(cStr3));
			ZeroMemory(cStr3, sizeof(cStr4));
			ZeroMemory(cStr3, sizeof(cStr5));
			ZeroMemory(cStr3, sizeof(cStr6));
			GetItemName(m_pItemList[m_stDialogBoxInfo[34].sV1], cStr1, cStr2, cStr3, cStr4, cStr5, cStr6);
			PutAlignedString(sX + 24, sX + 248, sY + 230 + 20, cStr1, 255, 255, 255);
			PutAlignedString(sX + 24, sX + 248, sY + 245 + 20, cStr2, 255, 255, 255);
			PutAlignedString(sX + 24, sX + 248, sY + 260 + 20, cStr3, 255, 255, 255);
			
			if (m_bUseOldPanels) {
				if (m_iGizonItemUpgradeLeft < iValue)
					DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 46);
				else
				{
					if ((msX >= sX + DEF_LBTNPOSX) && (msX <= sX + DEF_LBTNPOSX + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
						DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 47);
					else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 46);
				}
			}
			else {
				if (m_iGizonItemUpgradeLeft >= iValue)
				{
					if ((msX >= sX + DEF_LBTNPOSX) && (msX <= sX + DEF_LBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
					{
						DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 0);
						PutAlignedString2(sX + DEF_LBTNPOSX + 10, sX + DEF_LBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Upgrade", 255, 255, 100);
					}
					else
					{
						DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 1);
						PutAlignedString2(sX + DEF_LBTNPOSX + 10, sX + DEF_LBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Upgrade", 180, 188, 180);
					}
				}
				else
				{
					DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 1);
					PutAlignedString2(sX + DEF_LBTNPOSX + 10, sX + DEF_LBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Upgrade", 180, 188, 180);
				}
			}
		}
		//else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 46);
		
		// Cancel
		if (m_bUseOldPanels) {
			if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 17);
			else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 16); 
		}
		else {
			if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
			{
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
				PutAlignedString2(sX + DEF_RBTNPOSX + 10, sX + DEF_RBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Cancel", 255, 255, 100);
			}
			else
			{
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
				PutAlignedString2(sX + DEF_RBTNPOSX + 10, sX + DEF_RBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Cancel", 180, 188, 180);
			}
		}
		break;

	case 2: // in progress
		PutAlignedString(sX + 24, sX + 248, sY + 55 + 30 + 282 - 117 - 170, DRAW_DIALOGBOX_ITEMUPGRADE5, 255, 255, 255);
		PutAlignedString(sX + 24, sX + 248, sY + 55 + 45 + 282 - 117 - 170, DRAW_DIALOGBOX_ITEMUPGRADE6, 255, 255, 255);
		if (m_stDialogBoxInfo[34].sV1 != -1)
		{
			//DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME3, sX, sY, 3);
			i = m_stDialogBoxInfo[34].sV1;
			cItemColor = m_pItemList[m_stDialogBoxInfo[34].sV1]->m_cItemColor;
			if ((m_pItemList[m_stDialogBoxInfo[34].sV1]->m_cEquipPos == DEF_EQUIPPOS_LHAND)
				|| (m_pItemList[m_stDialogBoxInfo[34].sV1]->m_cEquipPos == DEF_EQUIPPOS_RHAND)
				|| (m_pItemList[m_stDialogBoxInfo[34].sV1]->m_cEquipPos == DEF_EQUIPPOS_TWOHAND))
			{
				m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_pItemList[m_stDialogBoxInfo[34].sV1]->m_sSprite]->PutSpriteRGB(sX + 134, sY + 182
					, m_pItemList[m_stDialogBoxInfo[34].sV1]->m_sSpriteFrame, m_wWR[cItemColor] - m_wR[0], m_wWG[cItemColor] - m_wG[0], m_wWB[cItemColor] - m_wB[0], dwTime);
			}
			else
			{
				m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_pItemList[m_stDialogBoxInfo[34].sV1]->m_sSprite]->PutSpriteRGB(sX + 134, sY + 182
					, m_pItemList[m_stDialogBoxInfo[34].sV1]->m_sSpriteFrame, m_wR[cItemColor] - m_wR[0], m_wG[cItemColor] - m_wG[0], m_wB[cItemColor] - m_wB[0], dwTime);
			}
			if ((rand() % 5) == 0) m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_pItemList[m_stDialogBoxInfo[34].sV1]->m_sSprite]->PutTransSprite25(sX + 134, sY + 182, m_pItemList[m_stDialogBoxInfo[34].sV1]->m_sSpriteFrame, dwTime);
			ZeroMemory(cStr1, sizeof(cStr1));
			ZeroMemory(cStr2, sizeof(cStr2));
			ZeroMemory(cStr3, sizeof(cStr3));
			ZeroMemory(cStr3, sizeof(cStr4));
			ZeroMemory(cStr3, sizeof(cStr5));
			ZeroMemory(cStr3, sizeof(cStr6));
			GetItemName(m_pItemList[m_stDialogBoxInfo[34].sV1], cStr1, cStr2, cStr3, cStr4, cStr5, cStr6);
			PutAlignedString(sX + 24, sX + 248, sY + 230 + 20, cStr1, 255, 255, 255);
			PutAlignedString(sX + 24, sX + 248, sY + 245 + 20, cStr2, 255, 255, 255);
			PutAlignedString(sX + 24, sX + 248, sY + 260 + 20, cStr3, 255, 255, 255);
		}
		if (((dwTime - m_stDialogBoxInfo[34].dwV1) / 1000 > 4) && (m_stDialogBoxInfo[34].dwV1 != 0))
		{
			m_stDialogBoxInfo[34].dwV1 = 0;
			bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_UPGRADEITEM, 0, m_stDialogBoxInfo[34].sV1, 0, 0, 0);
		}
		break;

	case 3: // Congratulations
		PutAlignedString(sX + 24, sX + 248, sY + 55 + 30 + 282 - 117 - 170, DRAW_DIALOGBOX_ITEMUPGRADE7, 255, 255, 255);//"Congratulation! item upgrade"
		PutAlignedString(sX + 24, sX + 248, sY + 55 + 45 + 282 - 117 - 170, DRAW_DIALOGBOX_ITEMUPGRADE8, 255, 255, 255);//"has been succeeded."
		if (m_stDialogBoxInfo[34].sV1 != -1)
		{
			//DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME3, sX, sY, 3);
			i = m_stDialogBoxInfo[34].sV1;
			cItemColor = m_pItemList[m_stDialogBoxInfo[34].sV1]->m_cItemColor;
			if ((m_pItemList[m_stDialogBoxInfo[34].sV1]->m_cEquipPos == DEF_EQUIPPOS_LHAND)
				|| (m_pItemList[m_stDialogBoxInfo[34].sV1]->m_cEquipPos == DEF_EQUIPPOS_RHAND)
				|| (m_pItemList[m_stDialogBoxInfo[34].sV1]->m_cEquipPos == DEF_EQUIPPOS_TWOHAND))
			{
				m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_pItemList[m_stDialogBoxInfo[34].sV1]->m_sSprite]->PutSpriteRGB(sX + 134, sY + 182
					, m_pItemList[m_stDialogBoxInfo[34].sV1]->m_sSpriteFrame, m_wWR[cItemColor] - m_wR[0], m_wWG[cItemColor] - m_wG[0], m_wWB[cItemColor] - m_wB[0], dwTime);
			}
			else
			{
				m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_pItemList[m_stDialogBoxInfo[34].sV1]->m_sSprite]->PutSpriteRGB(sX + 134, sY + 182
					, m_pItemList[m_stDialogBoxInfo[34].sV1]->m_sSpriteFrame, m_wR[cItemColor] - m_wR[0], m_wG[cItemColor] - m_wG[0], m_wB[cItemColor] - m_wB[0], dwTime);
			}
			ZeroMemory(cStr1, sizeof(cStr1));
			ZeroMemory(cStr2, sizeof(cStr2));
			ZeroMemory(cStr3, sizeof(cStr3));
			ZeroMemory(cStr3, sizeof(cStr4));
			ZeroMemory(cStr3, sizeof(cStr5));
			ZeroMemory(cStr3, sizeof(cStr6));
			GetItemName(m_pItemList[m_stDialogBoxInfo[34].sV1], cStr1, cStr2, cStr3, cStr4, cStr5, cStr6);
			PutAlignedString(sX + 24, sX + 248, sY + 230 + 20, cStr1, 255, 255, 255);
			PutAlignedString(sX + 24, sX + 248, sY + 245 + 20, cStr2, 255, 255, 255);
			PutAlignedString(sX + 24, sX + 248, sY + 260 + 20, cStr3, 255, 255, 255);
		}
		// OK
		if (m_bUseOldPanels) {
			if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
			else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0); 
		}
		else {
			if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
			{
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
				PutAlignedString2(sX + DEF_RBTNPOSX + 10, sX + DEF_RBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Ok", 255, 255, 100);
			}
			else
			{
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
				PutAlignedString2(sX + DEF_RBTNPOSX + 10, sX + DEF_RBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Ok", 180, 188, 180);
			}
		}
		break;

	case 4: // Failed
		PutAlignedString(sX + 24, sX + 248, sY + 55 + 30 + 282 - 117 - 170, DRAW_DIALOGBOX_ITEMUPGRADE9, 255, 255, 255);//"Item upgrade has failed."
		if ((m_stDialogBoxInfo[34].sV1 != -1) && (m_pItemList[m_stDialogBoxInfo[34].sV1] == 0))
		{
			PlaySound('E', 24, 0, 0);
			m_stDialogBoxInfo[34].cMode = 7;
			return;
		}
		if (m_stDialogBoxInfo[34].sV1 != -1)
		{
			//DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME3, sX, sY, 3);
			i = m_stDialogBoxInfo[34].sV1;
			cItemColor = m_pItemList[m_stDialogBoxInfo[34].sV1]->m_cItemColor;
			if ((m_pItemList[m_stDialogBoxInfo[34].sV1]->m_cEquipPos == DEF_EQUIPPOS_LHAND)
				|| (m_pItemList[m_stDialogBoxInfo[34].sV1]->m_cEquipPos == DEF_EQUIPPOS_RHAND)
				|| (m_pItemList[m_stDialogBoxInfo[34].sV1]->m_cEquipPos == DEF_EQUIPPOS_TWOHAND))
			{
				m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_pItemList[m_stDialogBoxInfo[34].sV1]->m_sSprite]->PutSpriteRGB(sX + 134, sY + 182
					, m_pItemList[m_stDialogBoxInfo[34].sV1]->m_sSpriteFrame, m_wWR[cItemColor] - m_wR[0], m_wWG[cItemColor] - m_wG[0], m_wWB[cItemColor] - m_wB[0], dwTime);
			}
			else
			{
				m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_pItemList[m_stDialogBoxInfo[34].sV1]->m_sSprite]->PutSpriteRGB(sX + 134, sY + 182
					, m_pItemList[m_stDialogBoxInfo[34].sV1]->m_sSpriteFrame, m_wR[cItemColor] - m_wR[0], m_wG[cItemColor] - m_wG[0], m_wB[cItemColor] - m_wB[0], dwTime);
			}
			ZeroMemory(cStr1, sizeof(cStr1));
			ZeroMemory(cStr2, sizeof(cStr2));
			ZeroMemory(cStr3, sizeof(cStr3));
			ZeroMemory(cStr3, sizeof(cStr4));
			ZeroMemory(cStr3, sizeof(cStr5));
			ZeroMemory(cStr3, sizeof(cStr6));
			GetItemName(m_pItemList[m_stDialogBoxInfo[34].sV1], cStr1, cStr2, cStr3, cStr4, cStr5, cStr6);
			PutAlignedString(sX + 24, sX + 248, sY + 230 + 20, cStr1, 255, 255, 255);
			PutAlignedString(sX + 24, sX + 248, sY + 245 + 20, cStr2, 255, 255, 255);
			PutAlignedString(sX + 24, sX + 248, sY + 260 + 20, cStr3, 255, 255, 255);
		}

		// OK
		if (m_bUseOldPanels) {
			if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
			else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0); 
		}
		else {
			if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
			{
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
				PutAlignedString2(sX + DEF_RBTNPOSX + 10, sX + DEF_RBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Ok", 255, 255, 100);
			}
			else
			{
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
				PutAlignedString2(sX + DEF_RBTNPOSX + 10, sX + DEF_RBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Ok", 180, 188, 180);
			}
		}
		break;

	case 5: // select Stone/ Gizon
		PutAlignedString(sX + 24, sX + 248, sY + 20 + 45, DRAW_DIALOGBOX_ITEMUPGRADE13, 255, 255, 255); // "Select which kind of items to upgrade."
		if ((msX > sX + 24) && (msX < sX + 248) && (msY > sY + 100) && (msY < sY + 115))
		{
			PutAlignedString(sX + 24, sX + 248, sY + 100, DRAW_DIALOGBOX_ITEMUPGRADE14, 255, 255, 0); // "Normal item upgrade"
			PutAlignedString(sX + 24, sX + 248, sY + 150, DRAW_DIALOGBOX_ITEMUPGRADE16, 255, 255, 255); // "Upgrading a normal item."
			PutAlignedString(sX + 24, sX + 248, sY + 165, DRAW_DIALOGBOX_ITEMUPGRADE17, 255, 255, 255); // "With upgrading weapon"
			PutAlignedString(sX + 24, sX + 248, sY + 180, DRAW_DIALOGBOX_ITEMUPGRADE18, 255, 255, 255); // "items, Stone of Xelima"
			PutAlignedString(sX + 24, sX + 248, sY + 195, DRAW_DIALOGBOX_ITEMUPGRADE19, 255, 255, 255); // "is needed. With upgrading"
			PutAlignedString(sX + 24, sX + 248, sY + 210, DRAW_DIALOGBOX_ITEMUPGRADE20, 255, 255, 255); // "armor items, Stone of"
			PutAlignedString(sX + 24, sX + 248, sY + 225, DRAW_DIALOGBOX_ITEMUPGRADE21, 255, 255, 255); // "Merien is needed."
			PutAlignedString(sX + 24, sX + 248, sY + 255, DRAW_DIALOGBOX_ITEMUPGRADE26, 255, 255, 255); // "* Majestic items cannot be"
			PutAlignedString(sX + 24, sX + 248, sY + 270, DRAW_DIALOGBOX_ITEMUPGRADE27, 255, 255, 255); // "upgraded with this performance."
		}
		else PutAlignedString(sX + 24, sX + 248, sY + 100, DRAW_DIALOGBOX_ITEMUPGRADE14, 70, 130, 180); // "Normal item upgrade"
		if ((msX > sX + 24) && (msX < sX + 248) && (msY > sY + 120) && (msY < sY + 135))
		{
			PutAlignedString(sX + 24, sX + 248, sY + 120, DRAW_DIALOGBOX_ITEMUPGRADE15, 255, 255, 0); // "Majestic item upgrade"
			PutAlignedString(sX + 24, sX + 248, sY + 150, DRAW_DIALOGBOX_ITEMUPGRADE22, 255, 255, 255); // "Upgrading majestic items."
			PutAlignedString(sX + 24, sX + 248, sY + 165, DRAW_DIALOGBOX_ITEMUPGRADE23, 255, 255, 255); // "In order to upgrade majestic items,"
			PutAlignedString(sX + 24, sX + 248, sY + 180, DRAW_DIALOGBOX_ITEMUPGRADE24, 255, 255, 255); // "enough upgrade points for"
			PutAlignedString(sX + 24, sX + 248, sY + 195, DRAW_DIALOGBOX_ITEMUPGRADE25, 255, 255, 255); // "majestic items are required."
			PutAlignedString(sX + 24, sX + 248, sY + 225, DRAW_DIALOGBOX_ITEMUPGRADE28, 255, 255, 255); // "* Normal items cannot be"
			PutAlignedString(sX + 24, sX + 248, sY + 240, DRAW_DIALOGBOX_ITEMUPGRADE29, 255, 255, 255); // "upgraded with this performance."
		}
		else PutAlignedString(sX + 24, sX + 248, sY + 120, DRAW_DIALOGBOX_ITEMUPGRADE15, 70, 130, 180); // "Majestic item upgrade"
		// Cancel
		if (m_bUseOldPanels) {
			if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 17);
			else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 16); 
		}
		else {
			if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
			{
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
				PutAlignedString2(sX + DEF_RBTNPOSX + 10, sX + DEF_RBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Cancel", 255, 255, 100);
			}
			else
			{
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
				PutAlignedString2(sX + DEF_RBTNPOSX + 10, sX + DEF_RBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Cancel", 180, 188, 180);
			}
		}
		break;

	case 6: // Stone upgrade
		//DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME3, sX, sY, 3);
		PutAlignedString(sX + 24, sX + 248, sY + 20 + 30, DRAW_DIALOGBOX_ITEMUPGRADE31, 255, 255, 255); // "After bring a normal item to"
		PutAlignedString(sX + 24, sX + 248, sY + 20 + 45, DRAW_DIALOGBOX_ITEMUPGRADE32, 255, 255, 255); //
		PutAlignedString(sX + 24, sX + 248, sY + 20 + 60, DRAW_DIALOGBOX_ITEMUPGRADE33, 255, 255, 255); //
		if (m_stDialogBoxInfo[34].sV2 == 0)
		{
			PutAlignedString(sX + 24, sX + 248, sY + 20 + 80, DRAW_DIALOGBOX_ITEMUPGRADE41, 195, 25, 25);//"You can upgrade only armor."
		}
		else
		{
			wsprintf(G_cTxt, DRAW_DIALOGBOX_ITEMUPGRADE34, m_stDialogBoxInfo[34].sV2);//"The number of upgrade(weapon): %d"
			PutAlignedString(sX + 24, sX + 248, sY + 20 + 80, G_cTxt, 255, 255, 255);
		}
		if (m_stDialogBoxInfo[34].sV3 == 0)
		{
			PutAlignedString(sX + 24, sX + 248, sY + 20 + 95, DRAW_DIALOGBOX_ITEMUPGRADE42, 195, 25, 25);//"You can upgrade only weapon."
		}
		else
		{
			wsprintf(G_cTxt, DRAW_DIALOGBOX_ITEMUPGRADE35, m_stDialogBoxInfo[34].sV3);//"The number of upgrade(armor): %d"
			PutAlignedString(sX + 24, sX + 248, sY + 20 + 95, G_cTxt, 255, 255, 255);
		}

		//DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 46);

		if (m_stDialogBoxInfo[34].sV1 != -1)
		{
			//DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME3, sX, sY, 3);
			i = m_stDialogBoxInfo[34].sV1;
			cItemColor = m_pItemList[m_stDialogBoxInfo[34].sV1]->m_cItemColor;
			if ((m_pItemList[m_stDialogBoxInfo[34].sV1]->m_cEquipPos == DEF_EQUIPPOS_LHAND)
				|| (m_pItemList[m_stDialogBoxInfo[34].sV1]->m_cEquipPos == DEF_EQUIPPOS_RHAND)
				|| (m_pItemList[m_stDialogBoxInfo[34].sV1]->m_cEquipPos == DEF_EQUIPPOS_TWOHAND))
			{
				m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_pItemList[m_stDialogBoxInfo[34].sV1]->m_sSprite]->PutSpriteRGB(sX + 134, sY + 182
					, m_pItemList[m_stDialogBoxInfo[34].sV1]->m_sSpriteFrame, m_wWR[cItemColor] - m_wR[0], m_wWG[cItemColor] - m_wG[0], m_wWB[cItemColor] - m_wB[0], dwTime);
			}
			else
			{
				m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_pItemList[m_stDialogBoxInfo[34].sV1]->m_sSprite]->PutSpriteRGB(sX + 134, sY + 182
					, m_pItemList[m_stDialogBoxInfo[34].sV1]->m_sSpriteFrame, m_wR[cItemColor] - m_wR[0], m_wG[cItemColor] - m_wG[0], m_wB[cItemColor] - m_wB[0], dwTime);
			}

			ZeroMemory(cStr1, sizeof(cStr1));
			ZeroMemory(cStr2, sizeof(cStr2));
			ZeroMemory(cStr3, sizeof(cStr3));
			ZeroMemory(cStr3, sizeof(cStr4));
			ZeroMemory(cStr3, sizeof(cStr5));
			ZeroMemory(cStr3, sizeof(cStr6));
			GetItemName(m_pItemList[m_stDialogBoxInfo[34].sV1], cStr1, cStr2, cStr3, cStr4, cStr5, cStr6);
			PutAlignedString(sX + 24, sX + 248, sY + 230 + 20, cStr1, 255, 255, 255);
			PutAlignedString(sX + 24, sX + 248, sY + 245 + 20, cStr2, 255, 255, 255);
			PutAlignedString(sX + 24, sX + 248, sY + 260 + 20, cStr3, 255, 255, 255);

			string st1 = m_pItemList[m_stDialogBoxInfo[34].sV1]->m_cName;
			if (memcmp(m_pItemList[m_stDialogBoxInfo[34].sV1]->m_cName, "HeroSword", 9) == 0 ||
				memcmp(m_pItemList[m_stDialogBoxInfo[34].sV1]->m_cName, "HeroWand", 8) == 0 ||
				memcmp(m_pItemList[m_stDialogBoxInfo[34].sV1]->m_cName, "HeroBow", 7) == 0)
			{
				PutAlignedString(sX + 24, sX + 248, sY + 245 + 20, "Required: 20.000 EKs", 255, 255, 255);
			}
			else if (st1.find("aHero") != string::npos || st1.find("eHero") != string::npos)
			{
				PutAlignedString(sX + 24, sX + 248, sY + 245 + 20, "Required: 30.000 EKs", 255, 255, 255);
			}
			else if (memcmp(m_pItemList[m_stDialogBoxInfo[34].sV1]->m_cName, "AresdenHeroCape", 15) == 0 ||
				memcmp(m_pItemList[m_stDialogBoxInfo[34].sV1]->m_cName, "ElvineHeroCape", 14) == 0)
			{
				PutAlignedString(sX + 24, sX + 248, sY + 245 + 20, "Required: 30 EKs & 50 Contrib.", 255, 255, 255);
			}
			else if (memcmp(m_pItemList[m_stDialogBoxInfo[34].sV1]->m_cName, "AresdenHeroCape+1", 17) == 0 ||
				memcmp(m_pItemList[m_stDialogBoxInfo[34].sV1]->m_cName, "ElvineHeroCape+1", 16) == 0)
			{
				PutAlignedString(sX + 24, sX + 248, sY + 245 + 20, "Required: 3.000 EKs & 200 Contrib.", 255, 255, 255);
			}

			if (m_bUseOldPanels) {
				if ((msX >= sX + DEF_LBTNPOSX) && (msX <= sX + DEF_LBTNPOSX + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
					DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 47);
				else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 46);
			}
			else {
				if ((msX >= sX + DEF_LBTNPOSX) && (msX <= sX + DEF_LBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
				{
					DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 0);
					PutAlignedString2(sX + DEF_LBTNPOSX + 10, sX + DEF_LBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Upgrade", 255, 255, 100);
				}
				else
				{
					DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 1);
					PutAlignedString2(sX + DEF_LBTNPOSX + 10, sX + DEF_LBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Upgrade", 180, 188, 180);
				}
			}
		}
		// Cancel
		if (m_bUseOldPanels) {
			if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 17);
			else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 16); 
		}
		else {
			if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
			{
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
				PutAlignedString2(sX + DEF_RBTNPOSX + 10, sX + DEF_RBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Cancel", 255, 255, 100);
			}
			else
			{
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
				PutAlignedString2(sX + DEF_RBTNPOSX + 10, sX + DEF_RBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Cancel", 180, 188, 180);
			}
		}
		break;

	case 7: // lost item
		PutAlignedString(sX + 24, sX + 248, sY + 20 + 130, DRAW_DIALOGBOX_ITEMUPGRADE36, 255, 255, 255);//"Due to the failure of upgrade, the"
		PutAlignedString(sX + 24, sX + 248, sY + 20 + 145, DRAW_DIALOGBOX_ITEMUPGRADE37, 255, 255, 255);//"item is gone. Try next time!"
		// OK
		if (m_bUseOldPanels) {
			if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
			else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0); 
		}
		else {
			if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
			{
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
				PutAlignedString2(sX + DEF_RBTNPOSX + 10, sX + DEF_RBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Ok", 255, 255, 100);
			}
			else
			{
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
				PutAlignedString2(sX + DEF_RBTNPOSX + 10, sX + DEF_RBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Ok", 180, 188, 180);
			}
		}
		break;

	case 8: // impossible deja an max
		PutAlignedString(sX + 24, sX + 248, sY + 20 + 130, DRAW_DIALOGBOX_ITEMUPGRADE38, 255, 255, 255);//"Upgrading the item is not possible any more."
		// OK
		if (m_bUseOldPanels) {
			if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
			else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0); 
		}
		else {
			if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
			{
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
				PutAlignedString2(sX + DEF_RBTNPOSX + 10, sX + DEF_RBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Ok", 255, 255, 100);
			}
			else
			{
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
				PutAlignedString2(sX + DEF_RBTNPOSX + 10, sX + DEF_RBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Ok", 180, 188, 180);
			}
		}
		break;

	case 9: // impossible
		PutAlignedString(sX + 24, sX + 248, sY + 20 + 130, DRAW_DIALOGBOX_ITEMUPGRADE39, 255, 255, 255);//"Upgrading the item is not possible."
		// OK
		if (m_bUseOldPanels) {
			if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
			else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
		}
		else {
			if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
			{
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
				PutAlignedString2(sX + DEF_RBTNPOSX + 10, sX + DEF_RBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Ok", 255, 255, 100);
			}
			else
			{
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
				PutAlignedString2(sX + DEF_RBTNPOSX + 10, sX + DEF_RBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Ok", 180, 188, 180);
			}
		}
		break;

	case 10: // plus de gizons
		PutAlignedString(sX + 24, sX + 248, sY + 20 + 130, DRAW_DIALOGBOX_ITEMUPGRADE40, 255, 255, 255);//"A point to upgrade items is not present"
		// OK
		if (m_bUseOldPanels) {
			if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
			else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0); 
		}
		else {
			if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
			{
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
				PutAlignedString2(sX + DEF_RBTNPOSX + 10, sX + DEF_RBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Ok", 255, 255, 100);
			}
			else
			{
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
				PutAlignedString2(sX + DEF_RBTNPOSX + 10, sX + DEF_RBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Ok", 180, 188, 180);
			}
		}
		break;
	}
}

void CGame::DrawDialogBox_Enchanting(int msX, int msY)
{
	int i, sX, sY;
	char cItemColor, cStr1[120], cStr2[120], cStr3[120], cStr4[120], cStr5[120], cStr6[120];
	UINT32 dwTime = timeGetTime();
	int iLoc, iLenSize, iEntry = 0;

	sX = m_stDialogBoxInfo[44].sX;
	sY = m_stDialogBoxInfo[44].sY;
	
	//DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME2, sX, sY, 0);
	//DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_TEXT, sX, sY, 5);

	short limitX, limitY, addx;

	if (m_stDialogBoxInfo[44].sV1 != -1 && m_stDialogBoxInfo[44].cMode == 1)
	{
		m_stDialogBoxInfo[44].sSizeX = (short)(258*2.3f);
		//m_stDialogBoxInfo[44].sSizeY = 339*2;
		addx = 3;
	}
	else {
		m_stDialogBoxInfo[44].sSizeX = 258;
		//m_stDialogBoxInfo[44].sSizeY = 339;
		addx = 1;
	}
	limitX = sX + m_stDialogBoxInfo[44].sSizeX;
	limitY = sY + m_stDialogBoxInfo[44].sSizeY;
	m_DDraw.DrawShadowBox(sX, sY, limitX, limitY, 0, true);
	m_DDraw.DrawShadowBox(sX, sY, limitX, limitY, 0, true);

	m_DDraw.DrawShadowBox(sX, sY, limitX, sY + 25, 0, true);
	m_DDraw.DrawShadowBox(sX, sY, limitX, sY + 25, 0, true);
	PutString_SprFont2(sX + 85* addx, sY + 5, "Item Enchant", 240, 240, 240);

	if (m_stDialogBoxInfo[44].cMode != 9) {
		//DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME3, sX, sY, 3); // recuadro x1 ok y1 ok x2 ok y2 ok
		m_DDraw.DrawShadowBox(sX + 20 - 5 + 90 - 2 - 5 - 4, sY + 90 + 10 + 24 + 10 - 1, sX + 50 + 5 + 80 + 40, sY + 145 + 102, 1, true);
	}

	switch (m_stDialogBoxInfo[44].cMode) {
	case 1://Gizon box Drag item needed to be upgraded"
		//DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME3, sX, sY, 3);
		PutAlignedString(sX + 24, sX + 248, sY + 20 + 30, "Drag item needed to be enchanted", 255, 255, 255); // Drag item needed to be upgraded"
		PutAlignedString(sX + 24, sX + 248, sY + 20 + 45, "from the inventory. ", 255, 255, 255); // "from the inventory. Then press"
		PutAlignedString(sX + 24, sX + 248, sY + 20 + 60, "", 255, 255, 255); // 'Upgrade' button."
		//DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 46);
		if (m_stDialogBoxInfo[44].sV1 != -1)
		{
			DisableDialogBox(2); // centu : close bag

			PutAlignedString(sX + 24 + 300, sX + 248 + 300, sY + 20 + 30, "Now select which attribute", 255, 255, 255); // Drag item needed to be upgraded"
			PutAlignedString(sX + 24 + 300, sX + 248 + 300, sY + 20 + 45, "you want to change. Then press", 255, 255, 255); // "from the inventory. Then press"
			PutAlignedString(sX + 24 + 300, sX + 248 + 300, sY + 20 + 60, "'Enchant' button.", 255, 255, 255); // 'Upgrade' button."
			//DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME3, sX, sY, 3);
			PutAlignedString(sX + 24 + 300, sX + 248 + 320, sY + 20 + 30 + 205, "* The attribute will change randomly to", 255, 255, 255); // Drag item needed to be upgraded"
			PutAlignedString(sX + 24 + 300, sX + 248 + 320, sY + 20 + 45 + 205, "one that is shown in the list. The ", 255, 255, 255); // "from the inventory. Then press"
			PutAlignedString(sX + 24 + 300, sX + 248 + 320, sY + 20 + 60 + 205, "percentage may vary depending on your skill level.", 255, 255, 255); // 'Upgrade' button."

			i = m_stDialogBoxInfo[44].sV1;
			cItemColor = m_pItemList[m_stDialogBoxInfo[44].sV1]->m_cItemColor;
			if ((m_pItemList[m_stDialogBoxInfo[44].sV1]->m_cEquipPos == DEF_EQUIPPOS_LHAND)
				|| (m_pItemList[m_stDialogBoxInfo[44].sV1]->m_cEquipPos == DEF_EQUIPPOS_RHAND)
				|| (m_pItemList[m_stDialogBoxInfo[44].sV1]->m_cEquipPos == DEF_EQUIPPOS_TWOHAND))
			{
				m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_pItemList[m_stDialogBoxInfo[44].sV1]->m_sSprite]->PutSpriteRGB(sX + 134, sY + 182, m_pItemList[m_stDialogBoxInfo[44].sV1]->m_sSpriteFrame, m_wWR[cItemColor] - m_wR[0], m_wWG[cItemColor] - m_wG[0], m_wWB[cItemColor] - m_wB[0], dwTime);
			}
			else
			{
				m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_pItemList[m_stDialogBoxInfo[44].sV1]->m_sSprite]->PutSpriteRGB(sX + 134, sY + 182, m_pItemList[m_stDialogBoxInfo[44].sV1]->m_sSpriteFrame, m_wR[cItemColor] - m_wR[0], m_wG[cItemColor] - m_wG[0], m_wB[cItemColor] - m_wB[0], dwTime);
			}

			char cSkillPerc[51];
			wsprintf(cSkillPerc, "Enchanting Probability: %d%%", m_cSkillMastery[20]);
			PutAlignedString(sX + 24, sX + 248, sY + 230 + 20, cSkillPerc, 255, 255, 255);
			
			ZeroMemory(cStr1, sizeof(cStr1));
			ZeroMemory(cStr2, sizeof(cStr2));
			ZeroMemory(cStr3, sizeof(cStr3));
			ZeroMemory(cStr3, sizeof(cStr4));
			ZeroMemory(cStr3, sizeof(cStr5));
			ZeroMemory(cStr3, sizeof(cStr6));
			GetItemName(m_pItemList[m_stDialogBoxInfo[44].sV1], cStr1, cStr2, cStr3, cStr4, cStr5, cStr6);
			PutAlignedString(sX + 24+300, sX + 248 + 300, sY + 230 + 20 - 120, cStr1, 255, 255, 0);
			
			if ((msX >= sX + 24 + 300) && (msX <= sX + 24 + 300 + 200) && (msY >= sY + 245 + 20 - 120) && (msY <= sY + 245 + 20 - 120 + 14) && strlen(cStr2) > 0)
			{
				PutAlignedString(sX + 24 + 300, sX + 248 + 300, sY + 245 + 20 - 120, cStr2, 255, 255, 255);
				// centu : recuadro 1er stat
				
				iLenSize = (int)strlen(cStr1);
				if (iLenSize < (int)strlen(cStr2)) iLenSize = (int)strlen(cStr2);
				if (iLenSize < (int)strlen(cStr3)) iLenSize = (int)strlen(cStr3);
				if (iLenSize < (int)strlen(cStr4)) iLenSize = (int)strlen(cStr4);
				if (iLenSize < (int)strlen(cStr5)) iLenSize = (int)strlen(cStr5);
				if (iLenSize < (int)strlen(cStr6)) iLenSize = (int)strlen(cStr6);

				ZeroMemory(G_cTxt, sizeof(G_cTxt));
				if ((m_pItemList[m_stDialogBoxInfo[44].sV1]->m_cEquipPos == DEF_EQUIPPOS_RHAND)
					|| (m_pItemList[m_stDialogBoxInfo[44].sV1]->m_cEquipPos == DEF_EQUIPPOS_TWOHAND)) 
				{
					if (m_pItemList[m_stDialogBoxInfo[44].sV1]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ATTACK_MANASAVE)
					{
						strcpy(G_cTxt, "Casting Prob. (7~105%)");
						iEntry++;
						if (iLenSize < (int)strlen(G_cTxt)) iLenSize = (int)strlen(G_cTxt);
					}
					else
					{
						strcpy(G_cTxt, "Light (7~105%)");
						iEntry++;
						if (iLenSize < (int)strlen(G_cTxt)) iLenSize = (int)strlen(G_cTxt);
						strcpy(G_cTxt, "Strong (7~105%)");
						iEntry++;
						if (iLenSize < (int)strlen(G_cTxt)) iLenSize = (int)strlen(G_cTxt);
						strcpy(G_cTxt, "Critical Damage (7~105%)");
						iEntry++;
						if (iLenSize < (int)strlen(G_cTxt)) iLenSize = (int)strlen(G_cTxt);
						strcpy(G_cTxt, "Agile (7~105%)");
						iEntry++;
						if (iLenSize < (int)strlen(G_cTxt)) iLenSize = (int)strlen(G_cTxt);
						strcpy(G_cTxt, "Righteous (7~105%)");
						iEntry++;
						if (iLenSize < (int)strlen(G_cTxt)) iLenSize = (int)strlen(G_cTxt);
						strcpy(G_cTxt, "Poison (7~105%)");
						iEntry++;
						if (iLenSize < (int)strlen(G_cTxt)) iLenSize = (int)strlen(G_cTxt);
						strcpy(G_cTxt, "Sharp (7~105%)");
						iEntry++;
						if (iLenSize < (int)strlen(G_cTxt)) iLenSize = (int)strlen(G_cTxt);
						strcpy(G_cTxt, "Ancient (7~105%)");
						iEntry++;
						if (iLenSize < (int)strlen(G_cTxt)) iLenSize = (int)strlen(G_cTxt);
						//strcpy(G_cTxt, "Magic (7~105%)");
						//iEntry++;
						//if (iLenSize < (int)strlen(G_cTxt)) iLenSize = (int)strlen(G_cTxt);
					}
				}
				else
				{
					if (m_pItemList[m_stDialogBoxInfo[44].sV1]->m_sItemEffectType == 45)
					{
						strcpy(G_cTxt, "Mana Conv. (7~105%)");
						iEntry++;
						if (iLenSize < (int)strlen(G_cTxt)) iLenSize = (int)strlen(G_cTxt);
						strcpy(G_cTxt, "Critical Chance (7~105%)");
						iEntry++;
						if (iLenSize < (int)strlen(G_cTxt)) iLenSize = (int)strlen(G_cTxt);
						strcpy(G_cTxt, "Casting Prob. (7~105%)");
						iEntry++;
						if (iLenSize < (int)strlen(G_cTxt)) iLenSize = (int)strlen(G_cTxt);
						strcpy(G_cTxt, "Critical Damage (7~105%)");
						iEntry++;
						if (iLenSize < (int)strlen(G_cTxt)) iLenSize = (int)strlen(G_cTxt);
					}
					else {
						strcpy(G_cTxt, "Light (7~105%)");
						iEntry++;
						if (iLenSize < (int)strlen(G_cTxt)) iLenSize = (int)strlen(G_cTxt);
						strcpy(G_cTxt, "Strong (7~105%)");
						iEntry++;
						if (iLenSize < (int)strlen(G_cTxt)) iLenSize = (int)strlen(G_cTxt);
						strcpy(G_cTxt, "Mana Conv. (7~105%)");
						iEntry++;
						if (iLenSize < (int)strlen(G_cTxt)) iLenSize = (int)strlen(G_cTxt);
						strcpy(G_cTxt, "Critical Chance (7~105%)");
						iEntry++;
						if (iLenSize < (int)strlen(G_cTxt)) iLenSize = (int)strlen(G_cTxt);

					}
				}

				if (iLenSize <= 15)
					iLenSize = iLenSize * 7.0f;
				else if (iLenSize <= 28)
					iLenSize = iLenSize * 5.5f;
				else iLenSize = iLenSize * 6.2f;

				m_DDraw.DrawShadowBox(msX - 3, msY + 25 - 1, msX + iLenSize+22, msY + 26 + 15 * iEntry, 0, true);
				//-----------------------------------------------------------------------------------------------------------------------------------

				iLoc = 0;
				ZeroMemory(G_cTxt, sizeof(G_cTxt));
				if ((m_pItemList[m_stDialogBoxInfo[44].sV1]->m_cEquipPos == DEF_EQUIPPOS_RHAND)
					|| (m_pItemList[m_stDialogBoxInfo[44].sV1]->m_cEquipPos == DEF_EQUIPPOS_TWOHAND)) 
				{
					if (m_pItemList[m_stDialogBoxInfo[44].sV1]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ATTACK_MANASAVE)
					{
						strcpy(G_cTxt, "Casting Prob. (7~105%)");
						PutString(msX, msY + 25 + iLoc, G_cTxt, RGB(150, 150, 150), false, 1);
						iLoc += 15;
					}
					else
					{
						strcpy(G_cTxt, "Light (7~105%)");
						PutString(msX, msY + 25 + iLoc, G_cTxt, RGB(150, 150, 150), false, 1);
						iLoc += 15;
						strcpy(G_cTxt, "Strong (7~105%)");
						PutString(msX, msY + 25 + iLoc, G_cTxt, RGB(150, 150, 150), false, 1);
						iLoc += 15;
						strcpy(G_cTxt, "Critical Damage (7~105%)");
						PutString(msX, msY + 25 + iLoc, G_cTxt, RGB(150, 150, 150), false, 1);
						iLoc += 15;
						strcpy(G_cTxt, "Agile (7~105%)");
						PutString(msX, msY + 25 + iLoc, G_cTxt, RGB(150, 150, 150), false, 1);
						iLoc += 15;
						strcpy(G_cTxt, "Righteous (7~105%)");
						PutString(msX, msY + 25 + iLoc, G_cTxt, RGB(150, 150, 150), false, 1);
						iLoc += 15;
						strcpy(G_cTxt, "Poison (7~105%)");
						PutString(msX, msY + 25 + iLoc, G_cTxt, RGB(150, 150, 150), false, 1);
						iLoc += 15;
						strcpy(G_cTxt, "Sharp (7~105%)");
						PutString(msX, msY + 25 + iLoc, G_cTxt, RGB(150, 150, 150), false, 1);
						iLoc += 15;
						strcpy(G_cTxt, "Ancient (7~105%)");
						PutString(msX, msY + 25 + iLoc, G_cTxt, RGB(150, 150, 150), false, 1);
						iLoc += 15;
						//strcpy(G_cTxt, "Magic (7~105%)");
						//PutString(msX, msY + 25 + iLoc, G_cTxt, RGB(150, 150, 150), false, 1);
						//iLoc += 15;
					}
				}
				else
				{
					if (m_pItemList[m_stDialogBoxInfo[44].sV1]->m_sItemEffectType == 45)
					{
						strcpy(G_cTxt, "Mana Conv. (7~105%)");
						PutString(msX, msY + 25 + iLoc, G_cTxt, RGB(150, 150, 150), false, 1);
						iLoc += 15;
						strcpy(G_cTxt, "Critical Chance (7~105%)");
						PutString(msX, msY + 25 + iLoc, G_cTxt, RGB(150, 150, 150), false, 1);
						iLoc += 15;
						strcpy(G_cTxt, "Casting Prob. (7~105%)");
						PutString(msX, msY + 25 + iLoc, G_cTxt, RGB(150, 150, 150), false, 1);
						iLoc += 15;
						strcpy(G_cTxt, "Critical Damage (7~105%)");
						PutString(msX, msY + 25 + iLoc, G_cTxt, RGB(150, 150, 150), false, 1);
						iLoc += 15;
					}
					else {
						strcpy(G_cTxt, "Light (7~105%)");
						PutString(msX, msY + 25 + iLoc, G_cTxt, RGB(150, 150, 150), false, 1);
						iLoc += 15;
						strcpy(G_cTxt, "Strong (7~105%)");
						PutString(msX, msY + 25 + iLoc, G_cTxt, RGB(150, 150, 150), false, 1);
						iLoc += 15;
						strcpy(G_cTxt, "Mana Conv. (7~105%)");
						PutString(msX, msY + 25 + iLoc, G_cTxt, RGB(150, 150, 150), false, 1);
						iLoc += 15;
						strcpy(G_cTxt, "Critical Chance (7~105%)");
						PutString(msX, msY + 25 + iLoc, G_cTxt, RGB(150, 150, 150), false, 1);
						iLoc += 15;
						
					}
				}
			}
			else
			{
				if (m_bFirstStatSelected) PutAlignedString(sX + 24 + 300, sX + 248 + 300, sY + 245 + 20 - 120, cStr2, 0, 255, 0);
				else PutAlignedString(sX + 24 + 300, sX + 248 + 300, sY + 245 + 20 - 120, cStr2, 19, 104, 169);
			}
			if ((msX >= sX + 24 + 300) && (msX <= sX + 24 + 300 + 200) && (msY >= sY + 260 + 20 - 120) && (msY <= sY + 260 + 20 - 120 + 14) && strlen(cStr3) > 0)
			{
				PutAlignedString(sX + 24 + 300, sX + 248 + 300, sY + 260 + 20 - 120, cStr3, 255, 255, 255);
				// centu : recuadro 2do stat
				iLenSize = (int)strlen(cStr1);
				if (iLenSize < (int)strlen(cStr2)) iLenSize = (int)strlen(cStr2);
				if (iLenSize < (int)strlen(cStr3)) iLenSize = (int)strlen(cStr3);
				if (iLenSize < (int)strlen(cStr4)) iLenSize = (int)strlen(cStr4);
				if (iLenSize < (int)strlen(cStr5)) iLenSize = (int)strlen(cStr5);
				if (iLenSize < (int)strlen(cStr6)) iLenSize = (int)strlen(cStr6);

				ZeroMemory(G_cTxt, sizeof(G_cTxt));
				if ((m_pItemList[m_stDialogBoxInfo[44].sV1]->m_cEquipPos == DEF_EQUIPPOS_RHAND)
					|| (m_pItemList[m_stDialogBoxInfo[44].sV1]->m_cEquipPos == DEF_EQUIPPOS_TWOHAND))
				{
					strcpy(G_cTxt, "Hit Prob. (7~105%)");
					iEntry++;
					if (iLenSize < (int)strlen(G_cTxt)) iLenSize = (int)strlen(G_cTxt);
					ZeroMemory(G_cTxt, sizeof(G_cTxt));
					strcpy(G_cTxt, "Consecutive Attack (7~105%)");
					iEntry++;
					if (iLenSize < (int)strlen(G_cTxt)) iLenSize = (int)strlen(G_cTxt);
					ZeroMemory(G_cTxt, sizeof(G_cTxt));
					strcpy(G_cTxt, "Experience (7~105%)");
					iEntry++;
					if (iLenSize < (int)strlen(G_cTxt)) iLenSize = (int)strlen(G_cTxt);
					ZeroMemory(G_cTxt, sizeof(G_cTxt));
					strcpy(G_cTxt, "Gold (7~105%)");
					iEntry++;
					if (iLenSize < (int)strlen(G_cTxt)) iLenSize = (int)strlen(G_cTxt);
				}
				else
				{
					if (m_pItemList[m_stDialogBoxInfo[44].sV1]->m_sItemEffectType == 45)
					{
						strcpy(G_cTxt, "Hit Prob. (7~105%)");
						iEntry++;
						if (iLenSize < (int)strlen(G_cTxt)) iLenSize = (int)strlen(G_cTxt);
						ZeroMemory(G_cTxt, sizeof(G_cTxt));
						strcpy(G_cTxt, "Consecutive Attack (7~105%)");
						iEntry++;
						if (iLenSize < (int)strlen(G_cTxt)) iLenSize = (int)strlen(G_cTxt);
						ZeroMemory(G_cTxt, sizeof(G_cTxt));
						strcpy(G_cTxt, "Experience (7~105%)");
						iEntry++;
						if (iLenSize < (int)strlen(G_cTxt)) iLenSize = (int)strlen(G_cTxt);
						ZeroMemory(G_cTxt, sizeof(G_cTxt));
						strcpy(G_cTxt, "Gold (7~105%)");
						iEntry++;
						if (iLenSize < (int)strlen(G_cTxt)) iLenSize = (int)strlen(G_cTxt);
						ZeroMemory(G_cTxt, sizeof(G_cTxt));
						strcpy(G_cTxt, "HP Rec. (7~105%)");
						iEntry++;
						if (iLenSize < (int)strlen(G_cTxt)) iLenSize = (int)strlen(G_cTxt);
						ZeroMemory(G_cTxt, sizeof(G_cTxt));
						strcpy(G_cTxt, "SP Rec. (7~105%)");
						iEntry++;
						if (iLenSize < (int)strlen(G_cTxt)) iLenSize = (int)strlen(G_cTxt);
						ZeroMemory(G_cTxt, sizeof(G_cTxt));
						strcpy(G_cTxt, "MP Rec. (7~105%)");
						iEntry++;
						if (iLenSize < (int)strlen(G_cTxt)) iLenSize = (int)strlen(G_cTxt);
					}
					else
					{
						strcpy(G_cTxt, "Poison Resist. (7~105%)");
						iEntry++;
						//if (iLenSize < (int)strlen(G_cTxt)) iLenSize = (int)strlen(G_cTxt);
						iLenSize = (int)strlen(G_cTxt);
						ZeroMemory(G_cTxt, sizeof(G_cTxt));
						strcpy(G_cTxt, "Defence Ratio (7~105%)");
						iEntry++;
						if (iLenSize < (int)strlen(G_cTxt)) iLenSize = (int)strlen(G_cTxt);
						ZeroMemory(G_cTxt, sizeof(G_cTxt));
						strcpy(G_cTxt, "HP Rec. (7~105%)");
						iEntry++;
						if (iLenSize < (int)strlen(G_cTxt)) iLenSize = (int)strlen(G_cTxt);
						ZeroMemory(G_cTxt, sizeof(G_cTxt));
						strcpy(G_cTxt, "SP Rec. (7~105%)");
						iEntry++;
						if (iLenSize < (int)strlen(G_cTxt)) iLenSize = (int)strlen(G_cTxt);
						ZeroMemory(G_cTxt, sizeof(G_cTxt));
						strcpy(G_cTxt, "MP Rec. (7~105%)");
						iEntry++;
						if (iLenSize < (int)strlen(G_cTxt)) iLenSize = (int)strlen(G_cTxt);
						ZeroMemory(G_cTxt, sizeof(G_cTxt));
						strcpy(G_cTxt, "Magic Resist. (7~105%)");
						iEntry++;
						if (iLenSize < (int)strlen(G_cTxt)) iLenSize = (int)strlen(G_cTxt);
						ZeroMemory(G_cTxt, sizeof(G_cTxt));
						strcpy(G_cTxt, "Physical Abs. (7~105%)");
						iEntry++;
						if (iLenSize < (int)strlen(G_cTxt)) iLenSize = (int)strlen(G_cTxt);
						ZeroMemory(G_cTxt, sizeof(G_cTxt));
						strcpy(G_cTxt, "Magic Abs. (7~105%)");
						iEntry++;
						if (iLenSize < (int)strlen(G_cTxt)) iLenSize = (int)strlen(G_cTxt);
					}
				}

				if (iLenSize <= 15)
					iLenSize = iLenSize * 7.0f;
				else if (iLenSize <= 28)
					iLenSize = iLenSize * 5.5f;
				else iLenSize = iLenSize * 6.2f;

				m_DDraw.DrawShadowBox(msX - 3, msY + 25 - 1, msX + iLenSize + 22, msY + 26 + 15 * iEntry, 0, true);
				//-----------------------------------------------------------------------------------------------------------------------------------

				iLoc = 0;
				ZeroMemory(G_cTxt, sizeof(G_cTxt));
				if ((m_pItemList[m_stDialogBoxInfo[44].sV1]->m_cEquipPos == DEF_EQUIPPOS_RHAND)
					|| (m_pItemList[m_stDialogBoxInfo[44].sV1]->m_cEquipPos == DEF_EQUIPPOS_TWOHAND))
				{
					strcpy(G_cTxt, "Hit Prob. (7~105%)");
					PutString(msX, msY + 25 + iLoc, G_cTxt, RGB(150, 150, 150), false, 1);
					iLoc += 15;
					ZeroMemory(G_cTxt, sizeof(G_cTxt));
					strcpy(G_cTxt, "Consecutive Attack (7~105%)");
					PutString(msX, msY + 25 + iLoc, G_cTxt, RGB(150, 150, 150), false, 1);
					iLoc += 15;
					ZeroMemory(G_cTxt, sizeof(G_cTxt));
					strcpy(G_cTxt, "Experience (7~105%)");
					PutString(msX, msY + 25 + iLoc, G_cTxt, RGB(150, 150, 150), false, 1);
					iLoc += 15;
					ZeroMemory(G_cTxt, sizeof(G_cTxt));
					strcpy(G_cTxt, "Gold (7~105%)");
					PutString(msX, msY + 25 + iLoc, G_cTxt, RGB(150, 150, 150), false, 1);
					iLoc += 15;
				}
				else
				{
					strcpy(G_cTxt, "Poison Resist. (7~105%)");
					PutString(msX, msY + 25 + iLoc, G_cTxt, RGB(150, 150, 150), false, 1);
					iLoc += 15;
					ZeroMemory(G_cTxt, sizeof(G_cTxt));
					strcpy(G_cTxt, "Defence Ratio (7~105%)");
					PutString(msX, msY + 25 + iLoc, G_cTxt, RGB(150, 150, 150), false, 1);
					iLoc += 15;
					ZeroMemory(G_cTxt, sizeof(G_cTxt));
					strcpy(G_cTxt, "HP Rec. (7~105%)");
					PutString(msX, msY + 25 + iLoc, G_cTxt, RGB(150, 150, 150), false, 1);
					iLoc += 15;
					ZeroMemory(G_cTxt, sizeof(G_cTxt));
					strcpy(G_cTxt, "SP Rec. (7~105%)");
					PutString(msX, msY + 25 + iLoc, G_cTxt, RGB(150, 150, 150), false, 1);
					iLoc += 15;
					ZeroMemory(G_cTxt, sizeof(G_cTxt));
					strcpy(G_cTxt, "MP Rec. (7~105%)");
					PutString(msX, msY + 25 + iLoc, G_cTxt, RGB(150, 150, 150), false, 1);
					iLoc += 15;
					ZeroMemory(G_cTxt, sizeof(G_cTxt));
					strcpy(G_cTxt, "Magic Resist. (7~105%)");
					PutString(msX, msY + 25 + iLoc, G_cTxt, RGB(150, 150, 150), false, 1);
					iLoc += 15;
					ZeroMemory(G_cTxt, sizeof(G_cTxt));
					strcpy(G_cTxt, "Physical Abs. (7~105%)");
					PutString(msX, msY + 25 + iLoc, G_cTxt, RGB(150, 150, 150), false, 1);
					iLoc += 15;
					ZeroMemory(G_cTxt, sizeof(G_cTxt));
					strcpy(G_cTxt, "Magic Abs. (7~105%)");
					PutString(msX, msY + 25 + iLoc, G_cTxt, RGB(150, 150, 150), false, 1);
					iLoc += 15;
				}
			}
			else
			{
				if (m_bSecondStatSelected) PutAlignedString(sX + 24 + 300, sX + 248 + 300, sY + 260 + 20 - 120, cStr3, 0, 255, 0);
				else PutAlignedString(sX + 24 + 300, sX + 248 + 300, sY + 260 + 20 - 120, cStr3, 19, 104, 169);
			}
			/*if ((msX >= sX + DEF_LBTNPOSX) && (msX <= sX + DEF_LBTNPOSX + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 47);
			else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 46);*/

			if (m_bFirstStatSelected || m_bSecondStatSelected) 
			{
				if ((msX >= sX + DEF_LBTNPOSX) && (msX <= sX + DEF_LBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
				{
					DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 0);
					PutAlignedString2(sX + DEF_LBTNPOSX + 10, sX + DEF_LBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Enchant", 255, 255, 100);
				}
				else
				{
					DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 1);
					PutAlignedString2(sX + DEF_LBTNPOSX + 10, sX + DEF_LBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Enchant", 180, 188, 180);
				}
			}
		}
		//else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 46);
		// Cancel
		/*if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 17);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 16);*/

		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
		{
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
			PutAlignedString2(sX + DEF_RBTNPOSX + 10, sX + DEF_RBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Cancel", 255, 255, 100);
		}
		else
		{
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
			PutAlignedString2(sX + DEF_RBTNPOSX + 10, sX + DEF_RBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Cancel", 180, 188, 180);
		}
		break;

	case 2: // in progress
		PutAlignedString(sX + 24, sX + 248, sY + 55 + 30 + 282 - 117 - 170, "Item enchanting is in process now.", 255, 255, 255);
		PutAlignedString(sX + 24, sX + 248, sY + 55 + 45 + 282 - 117 - 170, DRAW_DIALOGBOX_ITEMUPGRADE6, 255, 255, 255);
		if (m_stDialogBoxInfo[44].sV1 != -1)
		{
			//DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME3, sX, sY, 3);
			i = m_stDialogBoxInfo[44].sV1;
			cItemColor = m_pItemList[m_stDialogBoxInfo[44].sV1]->m_cItemColor;
			if ((m_pItemList[m_stDialogBoxInfo[44].sV1]->m_cEquipPos == DEF_EQUIPPOS_LHAND)
				|| (m_pItemList[m_stDialogBoxInfo[44].sV1]->m_cEquipPos == DEF_EQUIPPOS_RHAND)
				|| (m_pItemList[m_stDialogBoxInfo[44].sV1]->m_cEquipPos == DEF_EQUIPPOS_TWOHAND))
			{
				m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_pItemList[m_stDialogBoxInfo[44].sV1]->m_sSprite]->PutSpriteRGB(sX + 134, sY + 182
					, m_pItemList[m_stDialogBoxInfo[44].sV1]->m_sSpriteFrame, m_wWR[cItemColor] - m_wR[0], m_wWG[cItemColor] - m_wG[0], m_wWB[cItemColor] - m_wB[0], dwTime);
			}
			else
			{
				m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_pItemList[m_stDialogBoxInfo[44].sV1]->m_sSprite]->PutSpriteRGB(sX + 134, sY + 182
					, m_pItemList[m_stDialogBoxInfo[44].sV1]->m_sSpriteFrame, m_wR[cItemColor] - m_wR[0], m_wG[cItemColor] - m_wG[0], m_wB[cItemColor] - m_wB[0], dwTime);
			}
			if ((rand() % 5) == 0) m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_pItemList[m_stDialogBoxInfo[44].sV1]->m_sSprite]->PutTransSprite25(sX + 134, sY + 182, m_pItemList[m_stDialogBoxInfo[44].sV1]->m_sSpriteFrame, dwTime);
			/*ZeroMemory(cStr1, sizeof(cStr1));
			ZeroMemory(cStr2, sizeof(cStr2));
			ZeroMemory(cStr3, sizeof(cStr3));
			ZeroMemory(cStr3, sizeof(cStr4));
			ZeroMemory(cStr3, sizeof(cStr5));
			ZeroMemory(cStr3, sizeof(cStr6));
			GetItemName(m_pItemList[m_stDialogBoxInfo[44].sV1], cStr1, cStr2, cStr3, cStr4, cStr5, cStr6);
			PutAlignedString(sX + 24, sX + 248, sY + 230 + 20, cStr1, 255, 255, 255);
			PutAlignedString(sX + 24, sX + 248, sY + 245 + 20, cStr2, 255, 255, 255);
			PutAlignedString(sX + 24, sX + 248, sY + 260 + 20, cStr3, 255, 255, 255);*/
		}
		if (((dwTime - m_stDialogBoxInfo[44].dwV1) / 1000 > 4) && (m_stDialogBoxInfo[44].dwV1 != 0))
		{
			m_stDialogBoxInfo[44].dwV1 = 0;
			bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_ENCHANTITEM, 0, m_stDialogBoxInfo[44].sV1, m_stDialogBoxInfo[44].sV2, 0, 0);
		}
		break;

	case 3: // Congratulations
		PutAlignedString(sX + 24, sX + 248, sY + 55 + 30 + 282 - 117 - 170, "Congratulation! item enchant", 255, 255, 255);//"Congratulation! item upgrade"
		PutAlignedString(sX + 24, sX + 248, sY + 55 + 45 + 282 - 117 - 170, "has been succeeded.", 255, 255, 255);//"has been succeeded."
		if (m_stDialogBoxInfo[44].sV1 != -1)
		{
			//DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME3, sX, sY, 3);
			i = m_stDialogBoxInfo[44].sV1;
			cItemColor = m_pItemList[m_stDialogBoxInfo[44].sV1]->m_cItemColor;
			if ((m_pItemList[m_stDialogBoxInfo[44].sV1]->m_cEquipPos == DEF_EQUIPPOS_LHAND)
				|| (m_pItemList[m_stDialogBoxInfo[44].sV1]->m_cEquipPos == DEF_EQUIPPOS_RHAND)
				|| (m_pItemList[m_stDialogBoxInfo[44].sV1]->m_cEquipPos == DEF_EQUIPPOS_TWOHAND))
			{
				m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_pItemList[m_stDialogBoxInfo[44].sV1]->m_sSprite]->PutSpriteRGB(sX + 134, sY + 182
					, m_pItemList[m_stDialogBoxInfo[44].sV1]->m_sSpriteFrame, m_wWR[cItemColor] - m_wR[0], m_wWG[cItemColor] - m_wG[0], m_wWB[cItemColor] - m_wB[0], dwTime);
			}
			else
			{
				m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_pItemList[m_stDialogBoxInfo[44].sV1]->m_sSprite]->PutSpriteRGB(sX + 134, sY + 182
					, m_pItemList[m_stDialogBoxInfo[44].sV1]->m_sSpriteFrame, m_wR[cItemColor] - m_wR[0], m_wG[cItemColor] - m_wG[0], m_wB[cItemColor] - m_wB[0], dwTime);
			}
			ZeroMemory(cStr1, sizeof(cStr1));
			ZeroMemory(cStr2, sizeof(cStr2));
			ZeroMemory(cStr3, sizeof(cStr3));
			ZeroMemory(cStr3, sizeof(cStr4));
			ZeroMemory(cStr3, sizeof(cStr5));
			ZeroMemory(cStr3, sizeof(cStr6));
			GetItemName(m_pItemList[m_stDialogBoxInfo[44].sV1], cStr1, cStr2, cStr3, cStr4, cStr5, cStr6);
			PutAlignedString(sX + 24, sX + 248, sY + 230 + 20, cStr1, 255, 255, 255);
			PutAlignedString(sX + 24, sX + 248, sY + 245 + 20, cStr2, 255, 255, 255);
			PutAlignedString(sX + 24, sX + 248, sY + 260 + 20, cStr3, 255, 255, 255);
		}
		// OK
		/*if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);*/

		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
		{
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
			PutAlignedString2(sX + DEF_RBTNPOSX + 10, sX + DEF_RBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Ok", 255, 255, 100);
		}
		else
		{
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
			PutAlignedString2(sX + DEF_RBTNPOSX + 10, sX + DEF_RBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Ok", 180, 188, 180);
		}
		break;

	case 4: // Failed
		PutAlignedString(sX + 24, sX + 248, sY + 55 + 30 + 282 - 117 - 170, "Item enchant has failed.", 255, 255, 255);//"Item upgrade has failed."
		if ((m_stDialogBoxInfo[44].sV1 != -1) && (m_pItemList[m_stDialogBoxInfo[44].sV1] == 0))
		{
			PlaySound('E', 24, 0, 0);
			m_stDialogBoxInfo[44].cMode = 7;
			return;
		}
		if (m_stDialogBoxInfo[44].sV1 != -1)
		{
			//DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME3, sX, sY, 3);
			i = m_stDialogBoxInfo[44].sV1;
			cItemColor = m_pItemList[m_stDialogBoxInfo[44].sV1]->m_cItemColor;
			if ((m_pItemList[m_stDialogBoxInfo[44].sV1]->m_cEquipPos == DEF_EQUIPPOS_LHAND)
				|| (m_pItemList[m_stDialogBoxInfo[44].sV1]->m_cEquipPos == DEF_EQUIPPOS_RHAND)
				|| (m_pItemList[m_stDialogBoxInfo[44].sV1]->m_cEquipPos == DEF_EQUIPPOS_TWOHAND))
			{
				m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_pItemList[m_stDialogBoxInfo[44].sV1]->m_sSprite]->PutSpriteRGB(sX + 134, sY + 182
					, m_pItemList[m_stDialogBoxInfo[44].sV1]->m_sSpriteFrame, m_wWR[cItemColor] - m_wR[0], m_wWG[cItemColor] - m_wG[0], m_wWB[cItemColor] - m_wB[0], dwTime);
			}
			else
			{
				m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_pItemList[m_stDialogBoxInfo[44].sV1]->m_sSprite]->PutSpriteRGB(sX + 134, sY + 182
					, m_pItemList[m_stDialogBoxInfo[44].sV1]->m_sSpriteFrame, m_wR[cItemColor] - m_wR[0], m_wG[cItemColor] - m_wG[0], m_wB[cItemColor] - m_wB[0], dwTime);
			}
			ZeroMemory(cStr1, sizeof(cStr1));
			ZeroMemory(cStr2, sizeof(cStr2));
			ZeroMemory(cStr3, sizeof(cStr3));
			ZeroMemory(cStr3, sizeof(cStr4));
			ZeroMemory(cStr3, sizeof(cStr5));
			ZeroMemory(cStr3, sizeof(cStr6));
			GetItemName(m_pItemList[m_stDialogBoxInfo[44].sV1], cStr1, cStr2, cStr3, cStr4, cStr5, cStr6);
			PutAlignedString(sX + 24, sX + 248, sY + 230 + 20, cStr1, 255, 255, 255);
			PutAlignedString(sX + 24, sX + 248, sY + 245 + 20, cStr2, 255, 255, 255);
			PutAlignedString(sX + 24, sX + 248, sY + 260 + 20, cStr3, 255, 255, 255);
		}

		// OK
		/*if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);*/

		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
		{
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
			PutAlignedString2(sX + DEF_RBTNPOSX + 10, sX + DEF_RBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Ok", 255, 255, 100);
		}
		else
		{
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
			PutAlignedString2(sX + DEF_RBTNPOSX + 10, sX + DEF_RBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Ok", 180, 188, 180);
		}
		break;

	case 9: // impossible
		PutAlignedString(sX + 24, sX + 248, sY + 20 + 130, "Enchanting the item is not possible.", 255, 255, 255);//"Upgrading the item is not possible."
		// OK
		/*if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);*/

		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
		{
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
			PutAlignedString2(sX + DEF_RBTNPOSX + 10, sX + DEF_RBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Ok", 255, 255, 100);
		}
		else
		{
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
			PutAlignedString2(sX + DEF_RBTNPOSX + 10, sX + DEF_RBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Ok", 180, 188, 180);
		}
		break;
	}
}

// num : 1 - F1, 2 - F2, 3 - F3
void CGame::UseShortCut(int num)
{
	int index;
	if (num < 4) index = num;
	else index = num + 7;
	if (m_cGameMode != DEF_GAMEMODE_ONMAINGAME) return;
	if (m_bCtrlPressed == true)
	{
		if (m_sRecentShortCut == -1)
		{
			AddEventList(MSG_SHORTCUT1, 10);
			wsprintf(G_cTxt, MSG_SHORTCUT2, index);// [F%d]
			AddEventList(G_cTxt, 10);
			wsprintf(G_cTxt, MSG_SHORTCUT3, index);// [Control]-[F%d]
			AddEventList(G_cTxt, 10);
		}
		else
		{
			m_sShortCut[num] = m_sRecentShortCut;
			if (m_sShortCut[num] < 100)
			{
				if (m_pItemList[m_sShortCut[num]] == 0)
				{
					m_sShortCut[num] = -1;
					m_sRecentShortCut = -1;
					return;
				}
				char cStr1[64], cStr2[64], cStr3[64], cStr4[64], cStr5[64], cStr6[64];
				ZeroMemory(cStr1, sizeof(cStr1));
				ZeroMemory(cStr2, sizeof(cStr2));
				ZeroMemory(cStr3, sizeof(cStr3));
				ZeroMemory(cStr3, sizeof(cStr4));
				ZeroMemory(cStr3, sizeof(cStr5));
				ZeroMemory(cStr3, sizeof(cStr6));

				GetItemName(m_pItemList[m_sShortCut[num]], cStr1, cStr2, cStr3, cStr4, cStr5, cStr6);
				wsprintf(G_cTxt, MSG_SHORTCUT4, cStr1, cStr2, cStr3, index);// (%s %s %s) [F%d]
				AddEventList(G_cTxt, 10);
			}
			else if (m_sShortCut[num] >= 100)
			{
				if (m_pMagicCfgList[m_sShortCut[num] - 100] == 0)
				{
					m_sShortCut[num] = -1;
					m_sRecentShortCut = -1;
					return;
				}
				wsprintf(G_cTxt, MSG_SHORTCUT5, m_pMagicCfgList[m_sShortCut[num] - 100]->m_cName, index);// %s) [F%d]
				AddEventList(G_cTxt, 10);
			}
		}
	}
	else
	{
		if (m_sShortCut[num] == -1)
		{
			AddEventList(MSG_SHORTCUT1, 10);
			wsprintf(G_cTxt, MSG_SHORTCUT2, index);// [F%d]
			AddEventList(G_cTxt, 10);
			wsprintf(G_cTxt, MSG_SHORTCUT3, index);// [Control]-[F%d]
			AddEventList(G_cTxt, 10);
		}
		else if (m_sShortCut[num] < 100)
		{
			ItemEquipHandler((char)m_sShortCut[num]);
		}
		else if (m_sShortCut[num] >= 100) UseMagic(m_sShortCut[num] - 100);
	}
}

int CGame::iGetManaCost(int iMagicNo)
{
	int i, iManaSave, iManaCost;
	iManaSave = 0;
	if (iMagicNo < 0 || iMagicNo >= 100) return 1;
	for (i = 0; i<DEF_MAXITEMS; i++)
	{
		if (m_pItemList[i] == 0) continue;
		if (m_bIsItemEquipped[i] == true)
		{
			if (strcmp(m_pItemList[i]->m_cName, "MagicWand(MS10)") == 0)		iManaSave += 10;
			else if (strcmp(m_pItemList[i]->m_cName, "MagicWand(MS20)") == 0)		iManaSave += 20;
			else if (strcmp(m_pItemList[i]->m_cName, "MagicWand(MS30-LLF)") == 0) iManaSave += 30;
			else if (strcmp(m_pItemList[i]->m_cName, "WizMagicWand(MS10)") == 0)	iManaSave += 10;
			else if (strcmp(m_pItemList[i]->m_cName, "WizMagicWand(MS20)") == 0)	iManaSave += 20;
			else if (strcmp(m_pItemList[i]->m_cName, "MagicNecklace(MS10)") == 0) iManaSave += 10;
			else if (strcmp(m_pItemList[i]->m_cName, "DarkMageMagicStaff") == 0)	iManaSave += 10;
			else if (strcmp(m_pItemList[i]->m_cName, "DarkMageMagicStaffW") == 0) iManaSave += 10;
			else if (strcmp(m_pItemList[i]->m_cName, "DarkMageMagicWand") == 0)	iManaSave += 20;
			else if (strcmp(m_pItemList[i]->m_cName, "NecklaceOfLiche") == 0)   	iManaSave += 15;
			// Snoopy: v351 wands
			else if (strcmp(m_pItemList[i]->m_cName, "BlackMageTemple") == 0)		iManaSave += 25;
			else if (strcmp(m_pItemList[i]->m_cName, "BerserkWand(MS.20)") == 0)  iManaSave += 20;
			else if (strcmp(m_pItemList[i]->m_cName, "BerserkWand(MS.10)") == 0)  iManaSave += 10;
			else if (strcmp(m_pItemList[i]->m_cName, "KlonessWand(MS.20)") == 0)	iManaSave += 20;
			else if (strcmp(m_pItemList[i]->m_cName, "KlonessWand(MS.10)") == 0)	iManaSave += 10;
			else if (strcmp(m_pItemList[i]->m_cName, "ResurWand(MS.20)") == 0)	iManaSave += 20;
			else if (strcmp(m_pItemList[i]->m_cName, "ResurWand(MS.10)") == 0)	iManaSave += 10;
			// centuu - crafting necklaces
			else if (strcmp(m_pItemList[i]->m_cName, "MagicNecklace(MS12)") == 0) iManaSave += 12;
			else if (strcmp(m_pItemList[i]->m_cName, "MagicNecklace(MS14)") == 0) iManaSave += 14;
			else if (strcmp(m_pItemList[i]->m_cName, "MagicNecklace(MS16)") == 0) iManaSave += 16;
			else if (strcmp(m_pItemList[i]->m_cName, "MagicNecklace(MS18)") == 0) iManaSave += 18;
			// Ghost items
			else if (strcmp(m_pItemList[i]->m_cName, "HeroWand") == 0) iManaSave += 35;
			else if (strcmp(m_pItemList[i]->m_cName, "SupremoHeroWand") == 0) iManaSave += 40;
			else if (strcmp(m_pItemList[i]->m_cName, "GhostWand") == 0) iManaSave += 50;
			else if (strcmp(m_pItemList[i]->m_cName, "MagicWand(MS60-LLF)") == 0) iManaSave += 60;
		}
	}
	// Snoopy: MS max = 80%
	if (iManaSave > 80) iManaSave = 80;
	iManaCost = m_pMagicCfgList[iMagicNo]->m_sValue1;
	if (m_bIsSafeAttackMode) iManaCost += (iManaCost / 2) - (iManaCost / 10);
	if (iManaSave > 0)
	{
		float dV1 = (float)iManaSave;
		float dV2 = (float)(dV1 / 100.0f);
		float dV3 = (float)iManaCost;
		dV1 = dV2 * dV3;
		dV2 = dV3 - dV1;
		iManaCost = (int)dV2;
	}

	if (iManaCost < 1) iManaCost = 1;
	if (((m_sPlayerAppr2 & 0x0FF0) >> 4) == 34)
		iManaCost += 20;
	return iManaCost;
}

void CGame::UseMagic(int iMagicNo)
{
	if (iMagicNo < 0 || iMagicNo >= 100) return;
	if ((m_cMagicMastery[iMagicNo] == 0) || (m_pMagicCfgList[iMagicNo] == 0)) return;

	// Centu - can't use Inv or AMP in Arena
	if (strcmp(m_cMapName, "team") == 0 || strcmp(m_cMapName, "fightzone1") == 0)
	{
		if (iMagicNo == 32 || iMagicNo == 65)
		{
			AddEventList("You can't use this magic in this map.", 10);
			return;
		}
	}

	// Casting
	if (m_iHP <= 0) return;
	if (m_bIsGetPointingMode == true) return;
	if (iGetManaCost(iMagicNo) > m_iMP) return;
	
	if (_bIsItemOnHand() == true)
	{
		AddEventList(DLGBOX_CLICK_MAGIC1, 10);
		return;
	}

	// centu: release shield when cast
	for (int i = 0; i < DEF_MAXITEMS; i++)
	if ((m_pItemList[i] != 0) && (m_bIsItemEquipped[i] == true))
	{
		if (m_pItemList[i]->m_cEquipPos == DEF_EQUIPPOS_LHAND) 
		{
			bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_RELEASEITEM, 0, i, 0, 0, 0); //ReleaseEquipHandler(DEF_EQUIPPOS_LHAND);
			m_bIsItemEquipped[i] = false;
			m_sItemEquipmentStatus[m_pItemList[i]->m_cEquipPos] = -1;
		}
	}
	
	if (m_bSkillUsingStatus == true)
	{
		AddEventList(DLGBOX_CLICK_MAGIC2, 10);
		return;
	}
	if ((m_sPlayerAppr2 & 0xF000) == 0) bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_TOGGLECOMBATMODE, 0, 0, 0, 0, 0);
	m_cCommand = DEF_OBJECTMAGIC;
	m_iCastingMagicType = iMagicNo;
	m_sMagicShortCut = iMagicNo;
	m_sRecentShortCut = iMagicNo + 100;
	m_iPointCommandType = iMagicNo + 100;
	DisableDialogBox(3);
}


void CGame::ReleaseEquipHandler(char cEquipPos)
{
	char cStr1[64], cStr2[64], cStr3[64], cStr4[64], cStr5[64], cStr6[64];
	if (m_sItemEquipmentStatus[cEquipPos] < 0) return;

	GetItemName(m_pItemList[m_sItemEquipmentStatus[cEquipPos]], cStr1, cStr2, cStr3, cStr4, cStr5, cStr6);
	wsprintf(G_cTxt, ITEM_EQUIPMENT_RELEASED, cStr1);
	AddEventList(G_cTxt, 10);
	m_bIsItemEquipped[m_sItemEquipmentStatus[cEquipPos]] = false;
	m_sItemEquipmentStatus[cEquipPos] = -1;
}

void CGame::ItemEquipHandler(char cItemID)
{
	if (strcmp(m_cMapName, "team") == 0)
	{
		if ((m_pItemList[cItemID]->m_cEquipPos == DEF_EQUIPPOS_BACK) || (m_pItemList[cItemID]->m_cEquipPos == DEF_EQUIPPOS_BOOTS))
		{
			return;
		}
	}

	//Magn0S::
	if (!bMapEquip) {
		AddEventList("Equip Items in this map as been disabled.", 10);
		return;
	}

	if (bCheckItemOperationEnabled(cItemID) == false) return;
	if (m_bIsItemEquipped[cItemID] == true) return;
	
	if (m_pItemList[cItemID]->m_cEquipPos == DEF_EQUIPPOS_NONE)
	{
		AddEventList(BITEMDROP_CHARACTER3, 10);//"The item is not available."
		return;
	}
	if (m_pItemList[cItemID]->m_wCurLifeSpan <= 0)
	{
		AddEventList(BITEMDROP_CHARACTER1, 10); //"The item is exhausted. Fix it to use it."
		return;
	}

	if (m_pItemList[cItemID]->m_iClass != 0)
	{
		if (m_pItemList[cItemID]->m_iClass != m_iClass)
		{
			AddEventList("This item is not for your Class.", 10); //"The item is exhausted. Fix it to use it."
			return;
		}
	}

	if (m_pItemList[cItemID]->m_cEquipPos != DEF_EQUIPPOS_NECK &&
		m_pItemList[cItemID]->m_cEquipPos != DEF_EQUIPPOS_LFINGER && m_pItemList[cItemID]->m_cEquipPos != DEF_EQUIPPOS_BOOTS &&
		m_pItemList[cItemID]->m_cEquipPos != DEF_EQUIPPOS_BACK && m_pItemList[cItemID]->m_cEquipPos != DEF_EQUIPPOS_RFINGER)
	{
		switch (m_pItemList[cItemID]->m_iReqStat) {
		case 1://"Available for above Str %d"
			//LifeX Fix Light
			// If not Class base will show the light required stats STR
			if (m_pItemList[cItemID]->m_iReqStat != 1)
			{
				if (m_iStr < (m_pItemList[cItemID]->m_wWeight % 100))
				{
					AddEventList("You need more STR to equip this item.", 10);
					return;
				}

			}
			else
			{
				if (m_iStr < m_pItemList[cItemID]->m_iQuantStat)
				{
					AddEventList("You need more STR to equip this item.", 10);
					return;
				}
			}
			break;
		case 2: // "Available for above Dex %d"
			if (m_iDex < m_pItemList[cItemID]->m_iQuantStat)
			{
				AddEventList("You need more DEX to equip this item.", 10);
				return;
			}
			break;
		case 3: // "Available for above Vit %d"
			if (m_iVit < m_pItemList[cItemID]->m_iQuantStat)
			{
				AddEventList("You need more VIT to equip this item.", 10);
				return;
			}
			break;
		case 4: // "Available for above Int %d"
			if (m_iInt < m_pItemList[cItemID]->m_iQuantStat)
			{
				AddEventList("You need more INT to equip this item.", 10);
				return;
			}
			break;
		case 5: // "Available for above Mag %d"
			if (m_iMag < m_pItemList[cItemID]->m_iQuantStat)
			{
				AddEventList("You need more MAG to equip this item.", 10);
				return;
			}
			break;
		case 6: // "Available for above Chr %d"
			if (m_iCharisma < m_pItemList[cItemID]->m_iQuantStat)
			{
				AddEventList("You need more AGI to equip this item.", 10);
				return;
			}
			break;
		}
	}

	if (((m_pItemList[cItemID]->m_dwAttribute & 0x00000001) == 0) && (m_pItemList[cItemID]->m_sLevelLimit > m_iLevel))
	{
		AddEventList(BITEMDROP_CHARACTER4, 10);
		return;
	}
	if (m_bSkillUsingStatus == true)
	{
		AddEventList(BITEMDROP_CHARACTER5, 10);
		return;
	}
	if (m_pItemList[cItemID]->m_cGenderLimit != 0)
	{
		switch (m_sPlayerType) {
		case 1:
		case 2:
		case 3:
			if (m_pItemList[cItemID]->m_cGenderLimit != 1)
			{
				AddEventList(BITEMDROP_CHARACTER6, 10);
				return;
			}
			break;
		case 4:
		case 5:
		case 6:
			if (m_pItemList[cItemID]->m_cGenderLimit != 2)
			{
				AddEventList(BITEMDROP_CHARACTER7, 10);
				return;
			}
			break;
		}
	}

	bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_EQUIPITEM, 0, cItemID, 0, 0, 0);
	m_sRecentShortCut = cItemID;
	ReleaseEquipHandler(m_pItemList[cItemID]->m_cEquipPos);
	switch (m_pItemList[cItemID]->m_cEquipPos)	{
	case DEF_EQUIPPOS_HEAD:
	case DEF_EQUIPPOS_BODY:
	case DEF_EQUIPPOS_ARMS:
	case DEF_EQUIPPOS_PANTS:
	case DEF_EQUIPPOS_BOOTS:
	case DEF_EQUIPPOS_BACK:
		ReleaseEquipHandler(DEF_EQUIPPOS_FULLBODY);
		break;
	case DEF_EQUIPPOS_FULLBODY:
		ReleaseEquipHandler(DEF_EQUIPPOS_HEAD);
		ReleaseEquipHandler(DEF_EQUIPPOS_BODY);
		ReleaseEquipHandler(DEF_EQUIPPOS_ARMS);
		ReleaseEquipHandler(DEF_EQUIPPOS_PANTS);
		ReleaseEquipHandler(DEF_EQUIPPOS_BOOTS);
		ReleaseEquipHandler(DEF_EQUIPPOS_BACK);
		break;
	case DEF_EQUIPPOS_LHAND:
	case DEF_EQUIPPOS_RHAND:
		ReleaseEquipHandler(DEF_EQUIPPOS_TWOHAND);
		break;
	case DEF_EQUIPPOS_TWOHAND:
		ReleaseEquipHandler(DEF_EQUIPPOS_RHAND);
		ReleaseEquipHandler(DEF_EQUIPPOS_LHAND);
		break;
	}

	m_sItemEquipmentStatus[m_pItemList[cItemID]->m_cEquipPos] = cItemID;
	m_bIsItemEquipped[cItemID] = true;

	char cStr1[64], cStr2[64], cStr3[64], cStr4[64], cStr5[64], cStr6[64];
	GetItemName(m_pItemList[cItemID], cStr1, cStr2, cStr3, cStr4, cStr5, cStr6);
	wsprintf(G_cTxt, BITEMDROP_CHARACTER9, cStr1);
	AddEventList(G_cTxt, 10);
	
	if (memcmp(m_pItemList[cItemID]->m_cName, "AngelicPandent(STR)", 19) == 0) PlaySound('E', 52, 0);
	else if (memcmp(m_pItemList[cItemID]->m_cName, "AngelicPandent(DEX)", 19) == 0) PlaySound('E', 52, 0);
	else if (memcmp(m_pItemList[cItemID]->m_cName, "AngelicPandent(INT)", 19) == 0) PlaySound('E', 52, 0);
	else if (memcmp(m_pItemList[cItemID]->m_cName, "AngelicPandent(MAG)", 19) == 0) PlaySound('E', 52, 0);
	else PlaySound('E', 28, 0);
}

// 3.51 Level Up Dialog - Changes stats Using Majestic points - Diuuude
void CGame::DrawDialogBox_ChangeStatsMajestic(short msX, short msY)
{
	short sX, sY, szX;
	UINT32 dwTime = m_dwCurTime;
	char cTxt[120];
	int iStats;
	sX = m_stDialogBoxInfo[42].sX;
	sY = m_stDialogBoxInfo[42].sY;
	szX = m_stDialogBoxInfo[42].sSizeX;

	if (m_bUseOldPanels) {
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME6, sX, sY, 0);
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_TEXT2, sX, sY, 2);
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME8, sX + 16, sY + 100, 4);
	}
	else {
		short limitX, limitY;
		limitX = sX + m_stDialogBoxInfo[12].sSizeX;
		limitY = sY + m_stDialogBoxInfo[12].sSizeY;

		int addx = 50;
		int addy = 0;

		m_DDraw.DrawShadowBox(sX, sY, limitX, limitY, 0, true);
		if (!m_bDialogTrans) m_DDraw.DrawShadowBox(sX, sY, limitX, limitY, 0, true);

		m_DDraw.DrawShadowBox(sX, sY, limitX, sY + 25, 0, true);
		if (!m_bDialogTrans) m_DDraw.DrawShadowBox(sX, sY, limitX, sY + 25, 0, true);
		PutString_SprFont2(sX + 70, sY + 5, "Level Up Settings", 240, 240, 240);

		m_DDraw.DrawShadowBox(sX + 20 - 5 + 90 - 2, sY + 90 + 10 + 24, sX + 50 + 5 + 82, sY + 145 - 5, 1, true);
		m_DDraw.DrawShadowBox(sX + 20 - 5 + 90 - 1 + addx, sY + 90 + 10 + 24, sX + 50 + 5 + 83 + addx, sY + 145 - 5, 1, true);

		addy += 20;

		m_DDraw.DrawShadowBox(sX + 20 - 5 + 90 - 2, sY + 90 + 10 + 24 + addy, sX + 50 + 5 + 82, sY + 145 - 5 + addy, 1, true);
		m_DDraw.DrawShadowBox(sX + 20 - 5 + 90 - 1 + addx, sY + 90 + 10 + 24 + addy, sX + 50 + 5 + 83 + addx, sY + 145 - 5 + addy, 1, true);

		addy += 20;

		m_DDraw.DrawShadowBox(sX + 20 - 5 + 90 - 2, sY + 90 + 10 + 24 + addy, sX + 50 + 5 + 82, sY + 145 - 5 + addy, 1, true);
		m_DDraw.DrawShadowBox(sX + 20 - 5 + 90 - 1 + addx, sY + 90 + 10 + 24 + addy, sX + 50 + 5 + 83 + addx, sY + 145 - 5 + addy, 1, true);

		addy += 20;

		m_DDraw.DrawShadowBox(sX + 20 - 5 + 90 - 2, sY + 90 + 10 + 24 + addy, sX + 50 + 5 + 82, sY + 145 - 5 + addy, 1, true);
		m_DDraw.DrawShadowBox(sX + 20 - 5 + 90 - 1 + addx, sY + 90 + 10 + 24 + addy, sX + 50 + 5 + 83 + addx, sY + 145 - 5 + addy, 1, true);

		addy += 20;

		m_DDraw.DrawShadowBox(sX + 20 - 5 + 90 - 2, sY + 90 + 10 + 24 + addy, sX + 50 + 5 + 82, sY + 145 - 5 + addy, 1, true);
		m_DDraw.DrawShadowBox(sX + 20 - 5 + 90 - 1 + addx, sY + 90 + 10 + 24 + addy, sX + 50 + 5 + 83 + addx, sY + 145 - 5 + addy, 1, true);

		addy += 20;

		m_DDraw.DrawShadowBox(sX + 20 - 5 + 90 - 2, sY + 90 + 10 + 24 + addy, sX + 50 + 5 + 82, sY + 145 - 5 + addy, 1, true);
		m_DDraw.DrawShadowBox(sX + 20 - 5 + 90 - 1 + addx, sY + 90 + 10 + 24 + addy, sX + 50 + 5 + 83 + addx, sY + 145 - 5 + addy, 1, true);
	}

	PutAlignedString(sX, sX + szX, sY + 50, DRAW_DIALOGBOX_LEVELUP_SETTING14, 255, 255, 255);
	PutAlignedString(sX, sX + szX, sY + 65, DRAW_DIALOGBOX_LEVELUP_SETTING15, 255, 255, 255);

	// Majestic Points Left - Display in green if > 0
	PutString(sX + 20, sY + 85, DRAW_DIALOGBOX_LEVELUP_SETTING16, RGB(255, 255, 255));
	wsprintf(cTxt, "%d", m_iGizonItemUpgradeLeft);
	if (m_iGizonItemUpgradeLeft > 0)
	{
		PutString(sX + 95+55, sY + 85, cTxt, RGB(0, 255, 0));
	}
	else
	{
		PutString(sX + 95+55, sY + 85, cTxt, RGB(255, 255, 255));
	}
	// Strength
	PutString(sX + 24, sY + 125, DRAW_DIALOGBOX_LEVELUP_SETTING4, RGB(255, 255, 255));
	wsprintf(cTxt, "%d", m_iStr);
	PutString(sX + 109, sY + 125, cTxt, RGB(255, 255, 0));
	iStats = m_iStr + m_cLU_Str;
	wsprintf(cTxt, "%d", iStats);
	if (iStats != m_iStr)
	{
		PutString(sX + 162, sY + 125, cTxt, RGB(255, 0, 0));
	}
	else
	{
		PutString(sX + 162, sY + 125, cTxt, RGB(255, 255, 0));
	}
	//if ((msX >= sX + 195) && (msX <= sX + 205) && (msY >= sY + 127) && (msY <= sY + 133) && (m_iStr < DEF_STATS_LIMIT))
		m_pSprite[DEF_SPRID_INTERFACE_ND_GAME4]->PutSpriteFast(sX + 195, sY + 127, 5, dwTime);
	//if ((msX >= sX + 210) && (msX <= sX + 220) && (msY >= sY + 127) && (msY <= sY + 133) && (m_cLU_Str > 0))
		m_pSprite[DEF_SPRID_INTERFACE_ND_GAME4]->PutSpriteFast(sX + 210, sY + 127, 6, dwTime);

	// Vitality
	PutString(sX + 24, sY + 144, DRAW_DIALOGBOX_LEVELUP_SETTING5, RGB(255, 255, 255));
	wsprintf(cTxt, "%d", m_iVit);
	PutString(sX + 109, sY + 144, cTxt, RGB(255, 255, 0));
	iStats = m_iVit + m_cLU_Vit;
	wsprintf(cTxt, "%d", iStats);
	if (iStats != m_iVit)
	{
		PutString(sX + 162, sY + 144, cTxt, RGB(255, 0, 0));
	}
	else
	{
		PutString(sX + 162, sY + 144, cTxt, RGB(255, 255, 0));
	}
	//if ((msX >= sX + 195) && (msX <= sX + 205) && (msY >= sY + 146) && (msY <= sY + 152) && (m_iVit < DEF_STATS_LIMIT))
		m_pSprite[DEF_SPRID_INTERFACE_ND_GAME4]->PutSpriteFast(sX + 195, sY + 146, 5, dwTime);
	//if ((msX >= sX + 210) && (msX <= sX + 220) && (msY >= sY + 146) && (msY <= sY + 152) && (m_cLU_Vit > 0))
		m_pSprite[DEF_SPRID_INTERFACE_ND_GAME4]->PutSpriteFast(sX + 210, sY + 146, 6, dwTime);

	// Dexterity
	PutString(sX + 24, sY + 163, DRAW_DIALOGBOX_LEVELUP_SETTING6, RGB(255, 255, 255));
	wsprintf(cTxt, "%d", m_iDex);
	PutString(sX + 109, sY + 163, cTxt, RGB(255, 255, 0));
	iStats = m_iDex + m_cLU_Dex;
	wsprintf(cTxt, "%d", iStats);
	if (iStats != m_iDex)
	{
		PutString(sX + 162, sY + 163, cTxt, RGB(255, 0, 0));
	}
	else
	{
		PutString(sX + 162, sY + 163, cTxt, RGB(255, 255, 0));
	}
	//if ((msX >= sX + 195) && (msX <= sX + 205) && (msY >= sY + 165) && (msY <= sY + 171) && (m_iDex < DEF_STATS_LIMIT))
		m_pSprite[DEF_SPRID_INTERFACE_ND_GAME4]->PutSpriteFast(sX + 195, sY + 165, 5, dwTime);
	//if ((msX >= sX + 210) && (msX <= sX + 220) && (msY >= sY + 165) && (msY <= sY + 171) && (m_cLU_Dex > 0))
		m_pSprite[DEF_SPRID_INTERFACE_ND_GAME4]->PutSpriteFast(sX + 210, sY + 165, 6, dwTime);

	// Intelligence
	PutString(sX + 24, sY + 182+2, DRAW_DIALOGBOX_LEVELUP_SETTING7, RGB(255, 255, 255));
	wsprintf(cTxt, "%d", m_iInt);
	PutString(sX + 109, sY + 182+2, cTxt, RGB(255, 255, 0));
	iStats = m_iInt + m_cLU_Int;
	wsprintf(cTxt, "%d", iStats);
	if (iStats != m_iInt)
	{
		PutString(sX + 162, sY + 182+2, cTxt, RGB(255, 0, 0));
	}
	else
	{
		PutString(sX + 162, sY + 182+2, cTxt, RGB(255, 255, 0));
	}
	//if ((msX >= sX + 195) && (msX <= sX + 205) && (msY >= sY + 184) && (msY <= sY + 190) && (m_iInt < DEF_STATS_LIMIT))
		m_pSprite[DEF_SPRID_INTERFACE_ND_GAME4]->PutSpriteFast(sX + 195, sY + 184 + 2, 5, dwTime);
	//if ((msX >= sX + 210) && (msX <= sX + 220) && (msY >= sY + 184) && (msY <= sY + 190) && (m_cLU_Int > 0))
		m_pSprite[DEF_SPRID_INTERFACE_ND_GAME4]->PutSpriteFast(sX + 210, sY + 184 + 2, 6, dwTime);

	// Magic
	PutString(sX + 24, sY + 201+3, DRAW_DIALOGBOX_LEVELUP_SETTING8, RGB(255, 255, 255));
	wsprintf(cTxt, "%d", m_iMag);
	PutString(sX + 109, sY + 201+3, cTxt, RGB(255, 255, 0));
	iStats = m_iMag + m_cLU_Mag;
	wsprintf(cTxt, "%d", iStats);
	if (iStats != m_iMag)
	{
		PutString(sX + 162, sY + 201+3, cTxt, RGB(255, 0, 0));
	}
	else
	{
		PutString(sX + 162, sY + 201+3, cTxt, RGB(255, 255, 0));
	}
	//if ((msX >= sX + 195) && (msX <= sX + 205) && (msY >= sY + 203) && (msY <= sY + 209) && (m_iMag < DEF_STATS_LIMIT))
		m_pSprite[DEF_SPRID_INTERFACE_ND_GAME4]->PutSpriteFast(sX + 195, sY + 203 + 3, 5, dwTime);
	//if ((msX >= sX + 210) && (msX <= sX + 220) && (msY >= sY + 203) && (msY <= sY + 209) && (m_cLU_Mag > 0))
		m_pSprite[DEF_SPRID_INTERFACE_ND_GAME4]->PutSpriteFast(sX + 210, sY + 203 + 3, 6, dwTime);

	// Charisma
	//PutString(sX + 24, sY + 220, DRAW_DIALOGBOX_LEVELUP_SETTING9, RGB(5, 5, 5));
	PutString(sX + 24, sY + 220+3, "Agility", RGB(255, 255, 255));
	wsprintf(cTxt, "%d", m_iCharisma);
	PutString(sX + 109, sY + 220+3, cTxt, RGB(255, 255, 0));
	iStats = m_iCharisma + m_cLU_Char;
	wsprintf(cTxt, "%d", iStats);
	if (iStats != m_iCharisma)
	{
		PutString(sX + 162, sY + 220+3, cTxt, RGB(255, 0, 0));
	}
	else
	{
		PutString(sX + 162, sY + 220+3, cTxt, RGB(255, 255, 0));
	}
	//if ((msX >= sX + 195) && (msX <= sX + 205) && (msY >= sY + 222) && (msY <= sY + 228) && (m_iCharisma < DEF_STATS_LIMIT))
		m_pSprite[DEF_SPRID_INTERFACE_ND_GAME4]->PutSpriteFast(sX + 195, sY + 222 + 3, 5, dwTime);
	//if ((msX >= sX + 210) && (msX <= sX + 220) && (msY >= sY + 222) && (msY <= sY + 228) && (m_cLU_Char > 0))
		m_pSprite[DEF_SPRID_INTERFACE_ND_GAME4]->PutSpriteFast(sX + 210, sY + 222 + 3, 6, dwTime);
	
	if (!m_bUseOldPanels)
	{
		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
		{
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
			PutAlignedString2(sX + DEF_RBTNPOSX + 10, sX + DEF_RBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Cancel", 255, 255, 100);
		}
		else
		{
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
			PutAlignedString2(sX + DEF_RBTNPOSX + 10, sX + DEF_RBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Cancel", 180, 188, 180);
		}

		if ((msX >= sX + DEF_LBTNPOSX) && (msX <= sX + DEF_LBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
		{
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 0);
			PutAlignedString2(sX + DEF_LBTNPOSX + 10, sX + DEF_LBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Change", 255, 255, 100);
		}
		else
		{
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 1);
			PutAlignedString2(sX + DEF_LBTNPOSX + 10, sX + DEF_LBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Change", 180, 188, 180);
		}
	}
	else 
	{
		if ((msX >= sX + DEF_LBTNPOSX) && (msX <= sX + DEF_LBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 17);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 16);

		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
	}
}


// 3.51 LevelUp Box - Change stats using Majestic - Diuuude
void CGame::DlgBoxClick_ChangeStatsMajestic(short msX, short msY)
{
	short sX, sY;

	sX = m_stDialogBoxInfo[42].sX;
	sY = m_stDialogBoxInfo[42].sY;

	if ((cStateChange1 != 0) || (cStateChange2 != 0) || (cStateChange3 != 0)) {
		// Strength UP - Diuuude
		if ((msX >= sX + 195) && (msX <= sX + 205) && (msY >= sY + 127) && (msY <= sY + 133) && (m_cLU_Str < 0))
		{
			if (cStateChange1 == DEF_STR)
			{
				cStateChange1 = 0;
				m_cLU_Str++;
				m_iLU_Point--;
			}
			else if (cStateChange2 == DEF_STR)
			{
				cStateChange2 = 0;
				m_cLU_Str++;
				m_iLU_Point--;
			}
			else if (cStateChange3 == DEF_STR)
			{
				cStateChange3 = 0;
				m_cLU_Str++;
				m_iLU_Point--;
			}
			PlaySound('E', 14, 5);
		}

		// Vitality UP - Diuuude
		if ((msX >= sX + 195) && (msX <= sX + 205) && (msY >= sY + 146) && (msY <= sY + 152) && (m_cLU_Vit < 0)) {
			if (cStateChange1 == DEF_VIT)
			{
				cStateChange1 = 0;
				m_cLU_Vit++;
				m_iLU_Point--;
			}
			else if (cStateChange2 == DEF_VIT)
			{
				cStateChange2 = 0;
				m_cLU_Vit++;
				m_iLU_Point--;
			}
			else if (cStateChange3 == DEF_VIT)
			{
				cStateChange3 = 0;
				m_cLU_Vit++;
				m_iLU_Point--;
			}
			PlaySound('E', 14, 5);
		}

		// Dexterity UP - Diuuude
		if ((msX >= sX + 195) && (msX <= sX + 205) && (msY >= sY + 165) && (msY <= sY + 171) && (m_cLU_Dex < 0)) {
			if (cStateChange1 == DEF_DEX) {
				cStateChange1 = 0;
				m_cLU_Dex++;
				m_iLU_Point--;
			}
			else if (cStateChange2 == DEF_DEX) {
				cStateChange2 = 0;
				m_cLU_Dex++;
				m_iLU_Point--;
			}
			else if (cStateChange3 == DEF_DEX) {
				cStateChange3 = 0;
				m_cLU_Dex++;
				m_iLU_Point--;
			}
			PlaySound('E', 14, 5);
		}

		// Intelligence UP - Diuuude
		if ((msX >= sX + 195) && (msX <= sX + 205) && (msY >= sY + 184) && (msY <= sY + 190) && (m_cLU_Int < 0)) {
			if (cStateChange1 == DEF_INT) {
				cStateChange1 = 0;
				m_cLU_Int++;
				m_iLU_Point--;
			}
			else if (cStateChange2 == DEF_INT) {
				cStateChange2 = 0;
				m_cLU_Int++;
				m_iLU_Point--;
			}
			else if (cStateChange3 == DEF_INT) {
				cStateChange3 = 0;
				m_cLU_Int++;
				m_iLU_Point--;
			}
			PlaySound('E', 14, 5);
		}

		// Magic UP - Diuuude
		if ((msX >= sX + 195) && (msX <= sX + 205) && (msY >= sY + 203) && (msY <= sY + 209) && (m_cLU_Mag < 0)) {
			if (cStateChange1 == DEF_MAG) {
				cStateChange1 = 0;
				m_cLU_Mag++;
				m_iLU_Point--;
			}
			else if (cStateChange2 == DEF_MAG) {
				cStateChange2 = 0;
				m_cLU_Mag++;
				m_iLU_Point--;
			}
			else if (cStateChange3 == DEF_MAG) {
				cStateChange3 = 0;
				m_cLU_Mag++;
				m_iLU_Point--;
			}
			PlaySound('E', 14, 5);
		}

		// Charisma UP - Diuuude
		if ((msX >= sX + 195) && (msX <= sX + 205) && (msY >= sY + 222) && (msY <= sY + 228) && (m_cLU_Char < 0)) {
			if (cStateChange1 == DEF_CHR) {
				cStateChange1 = 0;
				m_cLU_Char++;
				m_iLU_Point--;
			}
			else if (cStateChange2 == DEF_CHR) {
				cStateChange2 = 0;
				m_cLU_Char++;
				m_iLU_Point--;
			}
			else if (cStateChange3 == DEF_CHR) {
				cStateChange3 = 0;
				m_cLU_Char++;
				m_iLU_Point--;
			}
			PlaySound('E', 14, 5);
		}
	}

	if ((cStateChange1 == 0) || (cStateChange2 == 0) || (cStateChange3 == 0) && (m_iGizonItemUpgradeLeft > 0))
	{	// Strength DOWN - Diuuude
		if ((msX >= sX + 210) && (msX <= sX + 220) && (msY >= sY + 127) && (msY <= sY + 133) && (m_iStr > 10))
		{
			if (cStateChange1 == 0)
			{
				cStateChange1 = DEF_STR;
				m_cLU_Str--;
				m_iLU_Point++;
			}
			else if (cStateChange2 == 0)
			{
				cStateChange2 = DEF_STR;
				m_cLU_Str--;
				m_iLU_Point++;
			}
			else
			{
				cStateChange3 = DEF_STR;
				m_cLU_Str--;
				m_iLU_Point++;
			}
			PlaySound('E', 14, 5);
		}

		// Vitality DOWN - Diuuude
		if ((msX >= sX + 210) && (msX <= sX + 220) && (msY >= sY + 146) && (msY <= sY + 152) && (m_iVit > 10)) {
			if (cStateChange1 == 0) {
				cStateChange1 = DEF_VIT;
				m_cLU_Vit--;
				m_iLU_Point++;
			}
			else if (cStateChange2 == 0) {
				cStateChange2 = DEF_VIT;
				m_cLU_Vit--;
				m_iLU_Point++;
			}
			else {
				cStateChange3 = DEF_VIT;
				m_cLU_Vit--;
				m_iLU_Point++;
			}
			PlaySound('E', 14, 5);
		}

		// Dexterity DOWN - Diuuude
		if ((msX >= sX + 210) && (msX <= sX + 220) && (msY >= sY + 165) && (msY <= sY + 171) && (m_iDex > 10)) {
			if (cStateChange1 == 0) {
				cStateChange1 = DEF_DEX;
				m_cLU_Dex--;
				m_iLU_Point++;
			}
			else if (cStateChange2 == 0) {
				cStateChange2 = DEF_DEX;
				m_cLU_Dex--;
				m_iLU_Point++;
			}
			else {
				cStateChange3 = DEF_DEX;
				m_cLU_Dex--;
				m_iLU_Point++;
			}
			PlaySound('E', 14, 5);
		}

		// Intelligence DOWN - Diuuude
		if ((msX >= sX + 210) && (msX <= sX + 220) && (msY >= sY + 184) && (msY <= sY + 190) && (m_iInt > 10))
		{
			if (cStateChange1 == 0)
			{
				cStateChange1 = DEF_INT;
				m_cLU_Int--;
				m_iLU_Point++;
			}
			else if (cStateChange2 == 0)
			{
				cStateChange2 = DEF_INT;
				m_cLU_Int--;
				m_iLU_Point++;
			}
			else
			{
				cStateChange3 = DEF_INT;
				m_cLU_Int--;
				m_iLU_Point++;
			}
			PlaySound('E', 14, 5);
		}

		// Magic DOWN - Diuuude
		if ((msX >= sX + 210) && (msX <= sX + 220) && (msY >= sY + 203) && (msY <= sY + 209) && (m_iMag > 10)) {
			if (cStateChange1 == 0) {
				cStateChange1 = DEF_MAG;
				m_cLU_Mag--;
				m_iLU_Point++;
			}
			else if (cStateChange2 == 0) {
				cStateChange2 = DEF_MAG;
				m_cLU_Mag--;
				m_iLU_Point++;
			}
			else {
				cStateChange3 = DEF_MAG;
				m_cLU_Mag--;
				m_iLU_Point++;
			}
			PlaySound('E', 14, 5);
		}

		// Charisma DOWN - Diuuude
		if ((msX >= sX + 210) && (msX <= sX + 220) && (msY >= sY + 222) && (msY <= sY + 228) && (m_iCharisma > 10)) {
			if (cStateChange1 == 0) {
				cStateChange1 = DEF_CHR;
				m_cLU_Char--;
				m_iLU_Point++;
			}
			else if (cStateChange2 == 0) {
				cStateChange2 = DEF_CHR;
				m_cLU_Char--;
				m_iLU_Point++;
			}
			else {
				cStateChange3 = DEF_CHR;
				m_cLU_Char--;
				m_iLU_Point++;
			}
			PlaySound('E', 14, 5);
		}
	}
	else
	{
		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
		{	// Send command to HG - Diuuude
			bSendCommand(MSGID_STATECHANGEPOINT, 0, 0, 0, 0, 0, 0);
			DisableDialogBox(42);
			PlaySound('E', 14, 5);
		}
	}
	if ((msX >= sX + DEF_LBTNPOSX) && (msX <= sX + DEF_LBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY)) {
		DisableDialogBox(42);
		PlaySound('E', 14, 5);
	}
}

/*********************************************************************************************************************
**  int CGame::bHasHeroSet( short m_sAppr3, short m_sAppr3, char OwnerType)		( Snoopy )							**
**  description			:: check weather the object (is character) is using a hero set (1:war, 2:mage)				**
**********************************************************************************************************************/
//Magn0S> Fixed Ultiamte hero set.
int CGame::bHasHeroSet(short Appr3, short Appr4, char OwnerType)
{
	char cArmor, cLeg, cBerk, cHat;
	cArmor = (Appr3 & 0xF000) >> 12;
	cLeg = (Appr3 & 0x0F00) >> 8;
	cHat = (Appr3 & 0x00F0) >> 4;
	cBerk = Appr3 & 0x000F;
	switch (OwnerType) {
	case 1:
	case 2:
	case 3:
		if ((cArmor == 8) && (cLeg == 5) && (cHat == 9) && (cBerk == 3)) return (1); // Warr elv M
		if ((cArmor == 9) && (cLeg == 6) && (cHat == 10) && (cBerk == 4)) return (1); // Warr ares M
		if ((cArmor == 13) && (cLeg == 7) && (cHat == 14) && (cBerk == 5)) return (1); // Warr Ghost M
		
		if ((cArmor == 10) && (cLeg == 5) && (cHat == 11) && (cBerk == 3)) return (2); // Mage elv M
		if ((cArmor == 11) && (cLeg == 6) && (cHat == 12) && (cBerk == 4)) return (2); // Mage ares M
		if ((cArmor == 12) && (cLeg == 7) && (cHat == 13) && (cBerk == 5)) return (2); // Mage Ghost M

		if ((cArmor == 8) && (cLeg == 5) && (cHat == 11) && (cBerk == 3)) return (1); // Archer elv M
		if ((cArmor == 9) && (cLeg == 6) && (cHat == 12) && (cBerk == 4)) return (1); // Archer ares M
		if ((cArmor == 13) && (cLeg == 7) && (cHat == 15) && (cBerk == 5)) return (1); // Archer God M
		break;
	case 4:
	case 5:
	case 6: // fixed
		if ((cArmor == 9) && (cLeg == 6) && (cHat == 9) && (cBerk == 4)) return (1); //War Elvine W
		if ((cArmor == 10) && (cLeg == 7) && (cHat == 10) && (cBerk == 5)) return (1); //War Aresden W
		if ((cArmor == 14) && (cLeg == 8) && (cHat == 14) && (cBerk == 6)) return (1); //War Ghost W
		
		if ((cArmor == 11) && (cLeg == 6) && (cHat == 11) && (cBerk == 4)) return (2); //Mage Elvine W
		if ((cArmor == 12) && (cLeg == 7) && (cHat == 12) && (cBerk == 5)) return (2); //Mage Aresden W
		if ((cArmor == 13) && (cLeg == 8) && (cHat == 13) && (cBerk == 6)) return (2); //Mage Ghost W

		if ((cArmor == 9) && (cLeg == 6) && (cHat == 11) && (cBerk == 4)) return (1); //War Elvine W
		if ((cArmor == 10) && (cLeg == 7) && (cHat == 12) && (cBerk == 5)) return (1); //War Aresden W
		if ((cArmor == 14) && (cLeg == 8) && (cHat == 15) && (cBerk == 6)) return (1); //War Ghost W
		break;
	}
	return 0;
}
/*int CGame::bHasHeroSet(short Appr3, short Appr4, char OwnerType)
{
	char cArmor, cLeg, cBerk, cHat;
	cArmor = (Appr3 & 0xF000) >> 12;
	cLeg = (Appr3 & 0x0F00) >> 8;
	cHat = (Appr3 & 0x00F0) >> 4;
	cBerk = Appr3 & 0x000F;
	switch (OwnerType){
	case 1:
	case 2:
	case 3:
		if ((cArmor == 8) && (cLeg == 5) && (cHat == 9) && (cBerk == 3)) return (1); // Warr elv M
		if ((cArmor == 9) && (cLeg == 6) && (cHat == 10) && (cBerk == 4)) return (1); // Warr ares M
		if ((cArmor == 10) && (cLeg == 5) && (cHat == 11) && (cBerk == 3)) return (2); // Mage elv M
		if ((cArmor == 11) && (cLeg == 6) && (cHat == 12) && (cBerk == 4)) return (2); // Mage ares M
		break;
	case 4:
	case 5:
	case 6: // fixed
		if ((cArmor == 9) && (cLeg == 6) && (cHat == 9) && (cBerk == 4)) return (1); //warr elv W
		if ((cArmor == 10) && (cLeg == 7) && (cHat == 10) && (cBerk == 5)) return (1); //warr ares W
		if ((cArmor == 11) && (cLeg == 6) && (cHat == 11) && (cBerk == 4)) return (2); //mage elv W
		if ((cArmor == 12) && (cLeg == 7) && (cHat == 12) && (cBerk == 5)) return (2); //mage ares W
		break;
	}
	return 0;
}*/
/*********************************************************************************************************************
**  void ShowHeldenianVictory( short sSide)				( Snoopy )													**
**  description			: Shows the Heldenian's End window															**
**********************************************************************************************************************/
void CGame::ShowHeldenianVictory(short sSide)
{
	int i, iPlayerSide;
	DisableDialogBox(18);
	for (i = 0; i < DEF_TEXTDLGMAXLINES; i++)
	{
		if (m_pMsgTextList[i] != 0)
			delete m_pMsgTextList[i];
		m_pMsgTextList[i] = 0;
	}
	if (m_bCitizen == false) iPlayerSide = 0;
	else if (m_bAresden == true) iPlayerSide = 1;
	else if (m_bAresden == false) iPlayerSide = 2;
	switch (sSide) {
	case 0:
		PlaySound('E', 25, 0, 0);
		m_pMsgTextList[0] = new class CMsg(0, "Heldenian holy war has been closed!", 0);
		m_pMsgTextList[1] = new class CMsg(0, " ", 0);
		m_pMsgTextList[2] = new class CMsg(0, "Heldenian Holy war ended", 0);
		m_pMsgTextList[3] = new class CMsg(0, "in a tie.", 0);
		break;
	case 1:
		PlaySound('E', 25, 0, 0);
		m_pMsgTextList[0] = new class CMsg(0, "Heldenian holy war has been closed!", 0);
		m_pMsgTextList[1] = new class CMsg(0, " ", 0);
		m_pMsgTextList[2] = new class CMsg(0, "Heldenian Holy war ended", 0);
		m_pMsgTextList[3] = new class CMsg(0, "in favor of Aresden.", 0);
		break;
	case 2:
		PlaySound('E', 25, 0, 0);
		m_pMsgTextList[0] = new class CMsg(0, "Heldenian holy war has been closed!", 0);
		m_pMsgTextList[1] = new class CMsg(0, " ", 0);
		m_pMsgTextList[2] = new class CMsg(0, "Heldenian Holy war ended", 0);
		m_pMsgTextList[3] = new class CMsg(0, "in favor of Elvine.", 0);
		break;
	}
	m_pMsgTextList[4] = new class CMsg(0, " ", 0);

	if (((iPlayerSide != 1) && (iPlayerSide != 2))   // Player not a normal citizen
		|| (sSide == 0))								// or no winner
	{
		PlaySound('E', 25, 0, 0);
		m_pMsgTextList[5] = new class CMsg(0, " ", 0);
		m_pMsgTextList[6] = new class CMsg(0, " ", 0);
		m_pMsgTextList[7] = new class CMsg(0, " ", 0);
		m_pMsgTextList[8] = new class CMsg(0, " ", 0);
	}
	else
	{
		if (sSide == iPlayerSide)
		{
			PlaySound('E', 23, 0, 0);
			PlaySound('C', 21, 0, 0);
			PlaySound('C', 22, 0, 0);
			m_pMsgTextList[5] = new class CMsg(0, "Congratulation.", 0);
			m_pMsgTextList[6] = new class CMsg(0, "As cityzen of victory,", 0);
			m_pMsgTextList[7] = new class CMsg(0, "You will recieve a reward.", 0);
			m_pMsgTextList[8] = new class CMsg(0, "      ", 0);
		}
		else
		{
			PlaySound('E', 24, 0, 0);
			PlaySound('C', 12, 0, 0);
			PlaySound('C', 13, 0, 0);
			m_pMsgTextList[5] = new class CMsg(0, "To our regret", 0);
			m_pMsgTextList[6] = new class CMsg(0, "As cityzen of defeat,", 0);
			m_pMsgTextList[7] = new class CMsg(0, "You cannot recieve any reward.", 0);
			m_pMsgTextList[8] = new class CMsg(0, "     ", 0);
		}
	}
	for (i = 9; i < 18; i++)
		m_pMsgTextList[i] = new class CMsg(0, " ", 0);
	EnableDialogBox(18, 0, 0, 0);
	DisableDialogBox(36);
	DisableDialogBox(37);
	DisableDialogBox(38);
}

/*********************************************************************************************************************
**  void DrawDialogBox_Resurect(short msX, short msY) 				( Snoopy )										**
**  description			: Asks the player if he accepts resurection													**
**********************************************************************************************************************/
void CGame::DrawDialogBox_Resurect(short msX, short msY)
{
	short sX, sY;
	//00476D02  |. 68 A4474C00    PUSH 004C47A4                    ;  ASCII "Someone intend to resurrect you."
	//00476D41  |. 68 8C474C00    PUSH 004C478C                    ;  ASCII "Will you revive here?"
	sX = m_stDialogBoxInfo[50].sX;
	sY = m_stDialogBoxInfo[50].sY;
	DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME6, sX, sY, 2);

	PutString(sX + 50, sY + 20, "Someone intend to resurrect you.", RGB(4, 0, 50));
	PutString(sX + 80, sY + 35, "Will you revive here?", RGB(4, 0, 50));

	if ((msX >= sX + 30) && (msX <= sX + 30 + DEF_BTNSZX) && (msY >= sY + 55) && (msY <= sY + 55 + DEF_BTNSZY))
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + 30, sY + 55, 19);
	else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + 30, sY + 55, 18);

	if ((msX >= sX + 170) && (msX <= sX + 170 + DEF_BTNSZX) && (msY >= sY + 55) && (msY <= sY + 55 + DEF_BTNSZY))
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + 170, sY + 55, 3);
	else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + 170, sY + 55, 2);
}

/*********************************************************************************************************************
**  void DlgBoxClick_Resurect(short msX, short msY)			( Snoopy )												**
**  description			: Response of the player if he accepts resurection or not									**
**********************************************************************************************************************/
void CGame::DlgBoxClick_Resurect(short msX, short msY)
{
	short sX, sY;
	sX = m_stDialogBoxInfo[50].sX;
	sY = m_stDialogBoxInfo[50].sY;
	if ((msX >= sX + 30) && (msX <= sX + 30 + DEF_BTNSZX) && (msY >= sY + 55) && (msY <= sY + 55 + DEF_BTNSZY))
	{   // yes
		bSendCommand(DEF_REQUEST_RESURRECTPLAYER_YES, 0, 0, 0, 0, 0, 0, 0);
		DisableDialogBox(50);
	}
	else if ((msX >= sX + 170) && (msX <= sX + 170 + DEF_BTNSZX) && (msY >= sY + 55) && (msY <= sY + 55 + DEF_BTNSZY))
	{	// no
		bSendCommand(DEF_REQUEST_RESURRECTPLAYER_NO, 0, 0, 0, 0, 0, 0, 0);
		DisableDialogBox(50);
	}
}

/*********************************************************************************************************************
**  void 	ResponseHeldenianTeleportList(char *pData)									(  Snoopy )					**
**  description			: Gail's TP																					**
**********************************************************************************************************************/
void CGame::ResponseHeldenianTeleportList(char *pData)
{
	char *cp;
	int  *ip, i;

	cp = pData + 6;
	ip = (int*)cp;
	m_iTeleportMapCount = *ip;
	cp += 4;
	for (i = 0; i < m_iTeleportMapCount; i++)
	{
		ip = (int*)cp;
		m_stTeleportList[i].iIndex = *ip;
		cp += 4;
		ZeroMemory(m_stTeleportList[i].mapname, sizeof(m_stTeleportList[i].mapname));
		memcpy(m_stTeleportList[i].mapname, cp, 10);
		cp += 10;
		ip = (int*)cp;
		m_stTeleportList[i].iX = *ip;
		cp += 4;
		ip = (int*)cp;
		m_stTeleportList[i].iY = *ip;
		cp += 4;
		ip = (int*)cp;
		m_stTeleportList[i].iCost = *ip;
		cp += 4;
	}
}

/*********************************************************************************************************************
**  void DrawDialogBox_CMDHallMenu(short msX, short msY)			( Snoopy )										**
**  description			: Draw the diag																				**
**********************************************************************************************************************/
void CGame::DrawDialogBox_CMDHallMenu(short msX, short msY)
{
	short sX, sY, szX;
	char cTxt[120];
	sX = m_stDialogBoxInfo[51].sX;
	sY = m_stDialogBoxInfo[51].sY;
	szX = m_stDialogBoxInfo[51].sSizeX;
	DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME6, sX, sY, 2);

	switch (m_stDialogBoxInfo[51].cMode) {
	case 0: // initial diag
		if ((msX > sX + 35) && (msX < sX + 220) && (msY > sY + 70) && (msY < sY + 95))
			PutAlignedString(sX, sX + szX, sY + 70, "Teleport to Battle Field", 255, 255, 255);
		else PutAlignedString(sX, sX + szX, sY + 70, "Teleport to Battle Field", 4, 0, 50);

		if ((msX > sX + 35) && (msX < sX + 220) && (msY > sY + 95) && (msY < sY + 120))
			PutAlignedString(sX, sX + szX, sY + 95, "Hire a soldier", 255, 255, 255);
		else PutAlignedString(sX, sX + szX, sY + 95, "Hire a soldier", 4, 0, 50);

		if ((msX > sX + 35) && (msX < sX + 220) && (msY > sY + 120) && (msY < sY + 145))
			PutAlignedString(sX, sX + szX, sY + 120, "Take a Flag", 255, 255, 255);
		else PutAlignedString(sX, sX + szX, sY + 120, "Take a Flag", 4, 0, 50);
		// Angels
		if ((msX > sX + 35) && (msX < sX + 220) && (msY > sY + 145) && (msY < sY + 170))
			PutAlignedString(sX, sX + szX, sY + 145, "Receive a Tutelary Angel", 255, 255, 255);
		else PutAlignedString(sX, sX + szX, sY + 145, "Receive a Tutelary Angel", 4, 0, 50);

		if ((msX > sX + 35) && (msX < sX + 220) && (msY > sY + 170) && (msY < sY + 195))
			PutAlignedString(sX, sX + szX, sY + 170, "Change Class", 255, 255, 255);
		else PutAlignedString(sX, sX + szX, sY + 170, "Change Class", 4, 0, 50);

		if ((msX > sX + 35) && (msX < sX + 220) && (msY > sY + 195) && (msY < sY + 195+25))
			PutAlignedString(sX, sX + szX, sY + 195, "Reset Stats", 255, 255, 255);
		else PutAlignedString(sX, sX + szX, sY + 195, "Reset Stats", 4, 0, 50);
		break;

	case 1: // TP diag
		if (m_iTeleportMapCount > 0)
		{
			PutString2(sX + 35, sY + 250, DRAW_DIALOGBOX_CITYHALL_MENU72_1, 55, 25, 25);//"Civilians cannot go some area."
			for (int i = 0; i<m_iTeleportMapCount; i++)
			{
				ZeroMemory(cTxt, sizeof(cTxt));
				GetOfficialMapName(m_stTeleportList[i].mapname, cTxt);
				wsprintf(G_cTxt, DRAW_DIALOGBOX_CITYHALL_MENU77, cTxt, m_stTeleportList[i].iCost);
				if ((msX >= sX + DEF_LBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY >= sY + 130 + i * 15) && (msY <= sY + 144 + i * 15))
					PutAlignedString(sX, sX + szX, sY + 130 + i * 15, G_cTxt, 255, 255, 255);
				else PutAlignedString(sX, sX + szX, sY + 130 + i * 15, G_cTxt, 250, 250, 0);
			}
		}
		else if (m_iTeleportMapCount == -1)
		{
			PutAlignedString(sX, sX + szX, sY + 125, DRAW_DIALOGBOX_CITYHALL_MENU73, 55, 25, 25);//"Now it's searching for possible area"
			PutAlignedString(sX, sX + szX, sY + 150, DRAW_DIALOGBOX_CITYHALL_MENU74, 55, 25, 25);//"to teleport."
			PutAlignedString(sX, sX + szX, sY + 175, DRAW_DIALOGBOX_CITYHALL_MENU75, 55, 25, 25);//"Please wait for a moment."
		}
		else
		{
			PutAlignedString(sX, sX + szX, sY + 175, DRAW_DIALOGBOX_CITYHALL_MENU76, 55, 25, 25);//"There is no area that you can teleport."
		}
		break;

	case 2: // Soldier diag
		PutAlignedString(sX, sX + szX, sY + 45, "You will hire a soldier by summon points", 255, 255, 255);
		if ((m_iConstructionPoint >= 2000) && (m_bIsCrusadeMode == false))
		{
			if ((msX > sX + 35) && (msX < sX + 220) && (msY > sY + 70) && (msY < sY + 95))
				PutAlignedString(sX, sX + szX, sY + 70, "Sorceress             2000 Point", 255, 255, 255);
			else PutAlignedString(sX, sX + szX, sY + 70, "Sorceress             2000 Point", 4, 0, 50);
		}
		else PutAlignedString(sX, sX + szX, sY + 70, "Sorceress             2000 Point", 65, 65, 65);

		if ((m_iConstructionPoint >= 3000) && (m_bIsCrusadeMode == false))
		{
			if ((msX > sX + 35) && (msX < sX + 220) && (msY > sY + 95) && (msY < sY + 120))
				PutAlignedString(sX, sX + szX, sY + 95, "Ancient Temple Knight 3000 Point", 255, 255, 255);
			else PutAlignedString(sX, sX + szX, sY + 95, "Ancient Temple Knight 3000 Point", 4, 0, 50);
		}
		else PutAlignedString(sX, sX + szX, sY + 95, "Ancient Temple Knight 3000 Point", 65, 65, 65);

		if ((m_iConstructionPoint >= 1500) && (m_bIsCrusadeMode == false))
		{
			if ((msX > sX + 35) && (msX < sX + 220) && (msY > sY + 120) && (msY < sY + 145))
				PutAlignedString(sX, sX + szX, sY + 120, "Elf Master            1500 Point", 255, 255, 255);
			else PutAlignedString(sX, sX + szX, sY + 120, "Elf Master            1500 Point", 4, 0, 50);
		}
		else PutAlignedString(sX, sX + szX, sY + 120, "Elf Master            1500 Point", 65, 65, 65);

		if ((m_iConstructionPoint >= 3000) && (m_bIsCrusadeMode == false))
		{
			if ((msX > sX + 35) && (msX < sX + 220) && (msY > sY + 145) && (msY < sY + 171))
				PutAlignedString(sX, sX + szX, sY + 145, "Dark Shadow Knight    3000 Point", 255, 255, 255);
			else PutAlignedString(sX, sX + szX, sY + 145, "Dark Shadow Knight    3000 Point", 4, 0, 50);
		}
		else PutAlignedString(sX, sX + szX, sY + 145, "Dark Shadow Knight    3000 Point", 65, 65, 65);

		if ((m_iConstructionPoint >= 4000) && (m_bIsCrusadeMode == false))
		{
			if ((msX > sX + 35) && (msX < sX + 220) && (msY > sY + 170) && (msY < sY + 195))
				PutAlignedString(sX, sX + szX, sY + 170, "Heavy Battle Tank     4000 Point", 255, 255, 255);
			else PutAlignedString(sX, sX + szX, sY + 170, "Heavy Battle Tank     4000 Point", 4, 0, 50);
		}
		else PutAlignedString(sX, sX + szX, sY + 170, "Heavy Battle Tank     4000 Point", 65, 65, 65);

		if ((m_iConstructionPoint >= 3000) && (m_bIsCrusadeMode == false))
		{
			if ((msX > sX + 35) && (msX < sX + 220) && (msY > sY + 195) && (msY < sY + 220))
				PutAlignedString(sX, sX + szX, sY + 195, "Barbarian             3000 Point", 255, 255, 255);
			else PutAlignedString(sX, sX + szX, sY + 195, "Barbarian             3000 Point", 4, 0, 50);
		}
		else PutAlignedString(sX, sX + szX, sY + 195, "Barbarian             3000 Point", 65, 65, 65);

		PutAlignedString(sX, sX + szX, sY + 220, "You should join a guild to hire soldiers.", 4, 0, 50);
		wsprintf(G_cTxt, "Summon points : %d", m_iConstructionPoint);
		PutAlignedString(sX, sX + szX, sY + 250, G_cTxt, 4, 0, 50);
		PutAlignedString(sX, sX + szX, sY + 280, "Maximum summon points : 12000 points.", 4, 0, 50);
		PutAlignedString(sX, sX + szX, sY + 300, "Maximum hiring number : 5 ", 4, 0, 50);
		break;

	case 3: // Hire a Flag Diag
		PutAlignedString(sX, sX + szX, sY + 45, "You may acquire Flags with EK points.", 4, 0, 50);
		PutAlignedString(sX, sX + szX, sY + 70, "Price is 10 EK per Flag.", 4, 0, 50);
		if ((msX >= sX + 35) && (msX <= sX + 220) && (msY >= sY + 140) && (msY <= sY + 165))
			PutAlignedString(sX, sX + szX, sY + 140, "Take a Flag", 255, 255, 255);
		else
			PutAlignedString(sX, sX + szX, sY + 140, "Take a Flag", 250, 250, 0);
		break;

	case 4: // Tutelar Angel Diag
		PutAlignedString(sX, sX + szX, sY + 45, "5 majestic points will be deducted", 4, 0, 50);
		PutAlignedString(sX, sX + szX, sY + 80, "upon receiving the Pendant of Tutelary Angel.", 4, 0, 50);
		PutAlignedString(sX, sX + szX, sY + 105, "Would you like to receive the Tutelary Angel?", 4, 0, 50);
		wsprintf(G_cTxt, DRAW_DIALOGBOX_ITEMUPGRADE11, m_iGizonItemUpgradeLeft);// "Item upgrade point : %d"
		PutAlignedString(sX, sX + szX, sY + 140, G_cTxt, 0, 0, 0);

		if ((msX > sX + 35) && (msX < sX + 220) && (msY > sY + 175) && (msY < sY + 200)
			&& (m_iGizonItemUpgradeLeft >4))
			PutAlignedString(sX, sX + szX, sY + 175, "Tutelary Angel (STR) will be handed out.", 255, 255, 255);
		else PutAlignedString(sX, sX + szX, sY + 175, "Tutelary Angel (STR) will be handed out.", 250, 250, 0);

		if ((msX > sX + 35) && (msX < sX + 220) && (msY > sY + 200) && (msY < sY + 225)
			&& (m_iGizonItemUpgradeLeft >4))
			PutAlignedString(sX, sX + szX, sY + 200, "Tutelary Angel (DEX) will be handed out.", 255, 255, 255);
		else PutAlignedString(sX, sX + szX, sY + 200, "Tutelary Angel (DEX) will be handed out.", 250, 250, 0);

		if ((msX > sX + 35) && (msX < sX + 220) && (msY > sY + 225) && (msY < sY + 250)
			&& (m_iGizonItemUpgradeLeft >4))
			PutAlignedString(sX, sX + szX, sY + 225, "Tutelary Angel (INT) will be handed out.", 255, 255, 255);
		else PutAlignedString(sX, sX + szX, sY + 225, "Tutelary Angel (INT) will be handed out.", 250, 250, 0);

		if ((msX > sX + 35) && (msX < sX + 220) && (msY > sY + 250) && (msY < sY + 275)
			&& (m_iGizonItemUpgradeLeft >4))
			PutAlignedString(sX, sX + szX, sY + 250, "Tutelary Angel (MAG) will be handed out.", 255, 255, 255);
		else PutAlignedString(sX, sX + szX, sY + 250, "Tutelary Angel (MAG) will be handed out.", 250, 250, 0);

		break;

	case 5:
		PutAlignedString(sX, sX + szX, sY + 45, "500 majestic points will be deducted", 4, 0, 50);
		PutAlignedString(sX, sX + szX, sY + 80, "upon changing class.", 4, 0, 50);
		PutAlignedString(sX, sX + szX, sY + 105, "Would you like to continue?", 4, 0, 50);
		wsprintf(G_cTxt, DRAW_DIALOGBOX_ITEMUPGRADE11, m_iGizonItemUpgradeLeft);// "Item upgrade point : %d"
		PutAlignedString(sX, sX + szX, sY + 140, G_cTxt, 0, 0, 0);

		if ((msX > sX + 35) && (msX < sX + 220) && (msY > sY + 175) && (msY < sY + 200)
			&& (m_iGizonItemUpgradeLeft > 499) && (m_iClass != 1))
			PutAlignedString(sX, sX + szX, sY + 175, "Warrior", 255, 255, 255);
		else
		{
			if (m_iClass == 1) PutAlignedString(sX, sX + szX, sY + 175, "Warrior", 250, 00, 0);
			else PutAlignedString(sX, sX + szX, sY + 175, "Warrior", 250, 250, 0);
		}

		if ((msX > sX + 35) && (msX < sX + 220) && (msY > sY + 200) && (msY < sY + 225)
			&& (m_iGizonItemUpgradeLeft > 499) && (m_iClass != 2))
			PutAlignedString(sX, sX + szX, sY + 200, "Magician", 255, 255, 255);
		else
		{
			if (m_iClass == 2) PutAlignedString(sX, sX + szX, sY + 200, "Magician", 250, 0, 0);
			else PutAlignedString(sX, sX + szX, sY + 200, "Magician", 250, 250, 0);
		}

		if ((msX > sX + 35) && (msX < sX + 220) && (msY > sY + 225) && (msY < sY + 250)
			&& (m_iGizonItemUpgradeLeft > 499) && (m_iClass != 3))
			PutAlignedString(sX, sX + szX, sY + 225, "Archer", 255, 255, 255);
		else
		{
			if (m_iClass == 3) PutAlignedString(sX, sX + szX, sY + 225, "Archer", 250, 0, 0);
			else PutAlignedString(sX, sX + szX, sY + 225, "Archer", 250, 250, 0);
		}
		break;

	case 6: //drajwer - ask for tooking hero mantle
		//PutAlignedString(sX, sX + szX - 1, sY + 125, m_cTakeHeroItemName, 55, 25, 25);
		//PutAlignedString(sX + 1, sX + szX, sY + 125, m_cTakeHeroItemName, 55, 25, 25);
		PutAlignedString(sX, sX + szX, sY + 260, "Would you like to receive this angel?", 55, 25, 25); // would you like..
		if ((msX >= sX + DEF_LBTNPOSX) && (msX <= sX + DEF_LBTNPOSX + DEF_BTNSZX)
			&& (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 19);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 18);
		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX)
			&& (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 3);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 2);
		break;

	case 7: //drajwer - ask for tooking hero mantle
		//PutAlignedString(sX, sX + szX - 1, sY + 125, m_cTakeHeroItemName, 55, 25, 25);
		//PutAlignedString(sX + 1, sX + szX, sY + 125, m_cTakeHeroItemName, 55, 25, 25);
		PutAlignedString(sX, sX + szX, sY + 260, "Would you like to change class?", 55, 25, 25); // would you like..
		if ((msX >= sX + DEF_LBTNPOSX) && (msX <= sX + DEF_LBTNPOSX + DEF_BTNSZX)
			&& (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 19);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 18);
		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX)
			&& (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 3);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 2);
		break;

	case 8: //drajwer - ask for tooking hero mantle
	//PutAlignedString(sX, sX + szX - 1, sY + 125, m_cTakeHeroItemName, 55, 25, 25);
	//PutAlignedString(sX + 1, sX + szX, sY + 125, m_cTakeHeroItemName, 55, 25, 25);
		PutAlignedString(sX, sX + szX, sY + 245, "This will cost 300 majestics,", 55, 25, 25);
		PutAlignedString(sX, sX + szX, sY + 260, "Would you like to reset stats?", 55, 25, 25); // would you like..
		if ((msX >= sX + DEF_LBTNPOSX) && (msX <= sX + DEF_LBTNPOSX + DEF_BTNSZX)
			&& (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 19);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 18);
		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX)
			&& (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 3);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 2);
		break;
	}
}

/*********************************************************************************************************************
**  void DlgBoxClick_CMDHallMenu(short msX, short msY)			( Snoopy )											**
**  description			: Response of the player if he accepts resurection or not									**
**********************************************************************************************************************/
void CGame::DlgBoxClick_CMDHallMenu(short msX, short msY)
{
	short sX, sY;
	sX = m_stDialogBoxInfo[51].sX;
	sY = m_stDialogBoxInfo[51].sY;
	switch (m_stDialogBoxInfo[51].cMode) {
	case 0: // initial diag
		if ((msX > sX + 35) && (msX < sX + 220) && (msY > sY + 70) && (msY < sY + 95))
		{
			m_stDialogBoxInfo[51].cMode = 1; // TP diag
			m_iTeleportMapCount = -1;
			bSendCommand(MSGID_REQUEST_HELDENIAN_TP_LIST, 0, 0, 0, 0, 0, 0);
			PlaySound('E', 14, 5);
		}
		if ((msX > sX + 35) && (msX < sX + 220) && (msY > sY + 95) && (msY < sY + 120))
		{
			m_stDialogBoxInfo[51].cMode = 2; // Soldier Diag
			PlaySound('E', 14, 5);
		}
		if ((msX > sX + 35) && (msX < sX + 220) && (msY > sY + 120) && (msY < sY + 145))
		{
			m_stDialogBoxInfo[51].cMode = 3; // Flag Diag
			PlaySound('E', 14, 5);
		}
		if ((msX > sX + 35) && (msX < sX + 220) && (msY > sY + 120) && (msY < sY + 145))
		{
			m_stDialogBoxInfo[51].cMode = 3; // Flag Diag
			PlaySound('E', 14, 5);
		}

		if ((msX > sX + 35) && (msX < sX + 220) && (msY > sY + 145) && (msY < sY + 170))
		{
			m_stDialogBoxInfo[51].cMode = 4; // Flag Diag
			PlaySound('E', 14, 5);
		}

		if ((msX > sX + 35) && (msX < sX + 220) && (msY > sY + 170) && (msY < sY + 195))
		{
			m_stDialogBoxInfo[51].cMode = 5; // Flag Diag
			PlaySound('E', 14, 5);
		}

		if ((msX > sX + 35) && (msX < sX + 220) && (msY > sY + 195) && (msY < sY + 195+25))
		{
			m_stDialogBoxInfo[51].cMode = 8; // Flag Diag
			PlaySound('E', 14, 5);
		}
		break;

	case 1: // TP now
		if (m_iTeleportMapCount > 0)
		{
			for (int i = 0; i<m_iTeleportMapCount; i++)
			{
				if ((msX >= sX + DEF_LBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY >= sY + 130 + i * 15) && (msY <= sY + 144 + i * 15))
				{
					bSendCommand(MSGID_REQUEST_HELDENIAN_TP, 0, 0, m_stTeleportList[i].iIndex, 0, 0, 0);
					DisableDialogBox(51);
					return;
				}
			}
		}
		break;

	case 2: // Buy a soldier scroll
		if ((msX >= sX + 35) && (msX <= sX + 220) && (msY > sY + 70) && (msY < sY + 95)
			&& (m_iConstructionPoint >= 2000) && (m_bIsCrusadeMode == false)) // Sorceress
		{
			bSendCommand(MSGID_REQUEST_HELDENIAN_SCROLL, 875, 1, 2, 3, 4, "Gail", 5);
			PlaySound('E', 14, 5);
		}
		if ((msX >= sX + 35) && (msX <= sX + 220) && (msY > sY + 95) && (msY < sY + 120)
			&& (m_iConstructionPoint >= 3000) && (m_bIsCrusadeMode == false)) // ATK
		{
			bSendCommand(MSGID_REQUEST_HELDENIAN_SCROLL, 876, 0, 0, 0, 0, "Gail", 0);
			PlaySound('E', 14, 5);
		}
		if ((msX >= sX + 35) && (msX <= sX + 220) && (msY > sY + 120) && (msY < sY + 145)
			&& (m_iConstructionPoint >= 1500) && (m_bIsCrusadeMode == false)) // Elf
		{
			bSendCommand(MSGID_REQUEST_HELDENIAN_SCROLL, 877, 0, 0, 0, 0, "Gail", 0);
			PlaySound('E', 14, 5);
		}
		if ((msX >= sX + 35) && (msX <= sX + 220) && (msY > sY + 145) && (msY < sY + 170)
			&& (m_iConstructionPoint >= 3000) && (m_bIsCrusadeMode == false)) // DSK
		{
			bSendCommand(MSGID_REQUEST_HELDENIAN_SCROLL, 878, 0, 0, 0, 0, "Gail", 0);
			PlaySound('E', 14, 5);
		}
		if ((msX >= sX + 35) && (msX <= sX + 220) && (msY > sY + 170) && (msY < sY + 195)
			&& (m_iConstructionPoint >= 4000) && (m_bIsCrusadeMode == false)) // HBT
		{
			bSendCommand(MSGID_REQUEST_HELDENIAN_SCROLL, 879, 0, 0, 0, 0, "Gail", 0);
			PlaySound('E', 14, 5);
		}
		if ((msX >= sX + 35) && (msX <= sX + 220) && (msY > sY + 195) && (msY < sY + 220)
			&& (m_iConstructionPoint >= 3000) && (m_bIsCrusadeMode == false)) // Barbarian
		{
			bSendCommand(MSGID_REQUEST_HELDENIAN_SCROLL, 880, 0, 0, 0, 0, "Gail", 0);
			PlaySound('E', 14, 5);
		}
		break;

	case 3: // Buy a Flag
		if ((msX >= sX + 35) && (msX <= sX + 220) && (msY >= sY + 140) && (msY <= sY + 165)
			&& (m_iEnemyKillCount >= 3))
		{
			bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_REQ_GETOCCUPYFLAG, 0, 0, 0, 0, 0, 0);
			PlaySound('E', 14, 5);
		}
		break;

	case 4: // Buy an Angel
		if ((msX >= sX + 35) && (msX <= sX + 220) && (msY >= sY + 175) && (msY <= sY + 200)
			&& (m_iGizonItemUpgradeLeft >= 5))
		{
			//bSendCommand(DEF_REQUEST_ANGEL, 0, 0, 1, 0, 0, "Gail", 0);
			m_stDialogBoxInfo[51].cMode = 6;
			m_stDialogBoxInfo[51].sV1 = 1;
			PlaySound('E', 14, 5);
		}
		if ((msX >= sX + 35) && (msX <= sX + 220) && (msY >= sY + 200) && (msY <= sY + 225)
			&& (m_iGizonItemUpgradeLeft >= 5))
		{
			//bSendCommand(DEF_REQUEST_ANGEL, 0, 0, 2, 0, 0, "Gail", 0);
			m_stDialogBoxInfo[51].cMode = 6;
			m_stDialogBoxInfo[51].sV1 = 2;
			PlaySound('E', 14, 5);
		}
		if ((msX >= sX + 35) && (msX <= sX + 220) && (msY >= sY + 225) && (msY <= sY + 250)
			&& (m_iGizonItemUpgradeLeft >= 5))
		{
			//bSendCommand(DEF_REQUEST_ANGEL, 0, 0, 3, 0, 0, "Gail", 0);
			m_stDialogBoxInfo[51].cMode = 6;
			m_stDialogBoxInfo[51].sV1 = 3;
			PlaySound('E', 14, 5);
		}
		if ((msX >= sX + 35) && (msX <= sX + 220) && (msY >= sY + 250) && (msY <= sY + 275)
			&& (m_iGizonItemUpgradeLeft >= 5))
		{
			//bSendCommand(DEF_REQUEST_ANGEL, 0, 0, 4, 0, 0, "Gail", 0);
			m_stDialogBoxInfo[51].cMode = 6;
			m_stDialogBoxInfo[51].sV1 = 4;
			PlaySound('E', 14, 5);
		}

		break;

	case 5:

		if ((msX >= sX + 35) && (msX <= sX + 220) && (msY >= sY + 175) && (msY <= sY + 200)
			&& (m_iGizonItemUpgradeLeft >= 500) && (m_iClass != 1))
		{
			//bSendCommand(DEF_REQUEST_CHANGE_CLASS, 0, 0, 1, 0, 0, "Gail", 0);
			m_stDialogBoxInfo[51].cMode = 7;
			m_stDialogBoxInfo[51].sV1 = 1;
			PlaySound('E', 14, 5);
		}
		if ((msX >= sX + 35) && (msX <= sX + 220) && (msY >= sY + 200) && (msY <= sY + 225)
			&& (m_iGizonItemUpgradeLeft >= 500) && (m_iClass != 2))
		{
			//bSendCommand(DEF_REQUEST_CHANGE_CLASS, 0, 0, 2, 0, 0, "Gail", 0);
			m_stDialogBoxInfo[51].cMode = 7;
			m_stDialogBoxInfo[51].sV1 = 2;
			PlaySound('E', 14, 5);
		}
		if ((msX >= sX + 35) && (msX <= sX + 220) && (msY >= sY + 225) && (msY <= sY + 250)
			&& (m_iGizonItemUpgradeLeft >= 500) && (m_iClass != 3))
		{
			//bSendCommand(DEF_REQUEST_CHANGE_CLASS, 0, 0, 3, 0, 0, "Gail", 0);
			m_stDialogBoxInfo[51].cMode = 7;
			m_stDialogBoxInfo[51].sV1 = 3;
			PlaySound('E', 14, 5);
		}

		break;

	case 6: 
		if ((msX >= sX + DEF_LBTNPOSX) && (msX <= sX + DEF_LBTNPOSX + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
		{
			bSendCommand(DEF_REQUEST_ANGEL, 0, 0, m_stDialogBoxInfo[51].sV1, 0, 0, "Gail", 0);
			m_stDialogBoxInfo[51].cMode = 0;
			PlaySound('E', 14, 5);
		}
		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
		{
			m_stDialogBoxInfo[51].cMode = 4;
			PlaySound('E', 14, 5);
		}
		break;

	case 7:
		if ((msX >= sX + DEF_LBTNPOSX) && (msX <= sX + DEF_LBTNPOSX + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
		{
			bSendCommand(DEF_REQUEST_CHANGE_CLASS, 0, 0, m_stDialogBoxInfo[51].sV1, 0, 0, "Gail", 0);
			m_stDialogBoxInfo[51].cMode = 0;
			PlaySound('E', 14, 5);
		}
		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
		{
			m_stDialogBoxInfo[51].cMode = 5;
			PlaySound('E', 14, 5);
		}
		break;
	case 8:
		if ((msX >= sX + DEF_LBTNPOSX) && (msX <= sX + DEF_LBTNPOSX + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
		{
			bSendCommand(DEF_REQUEST_RESET_STATS, 0, 0, m_iClass, 0, 0, "Gail", 0);
			m_stDialogBoxInfo[51].cMode = 0;
			PlaySound('E', 14, 5);
		}
		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
		{
			m_stDialogBoxInfo[51].cMode = 0;
			PlaySound('E', 14, 5);
		}
		break;
	}
}
/*********************************************************************************************************************
**  bool DKGlare(int iWeaponIndex, int iWeaponIndex, int *iWeaponGlare)	( Snoopy )									**
**  description			: test glowing condition for DK set															**
**********************************************************************************************************************/
void CGame::DKGlare(int iWeaponColor, int iWeaponIndex, int *iWeaponGlare)
{
	if (iWeaponColor != 9) return;
	if (((iWeaponIndex >= DEF_SPRID_WEAPON_M + 64 * 14) && (iWeaponIndex < DEF_SPRID_WEAPON_M + 64 * 14 + 56)) //msw3
		|| ((iWeaponIndex >= DEF_SPRID_WEAPON_W + 64 * 14) && (iWeaponIndex < DEF_SPRID_WEAPON_W + 64 * 14 + 56))) //wsw3
	{
		*iWeaponGlare = 3;
	}
	else if (((iWeaponIndex >= DEF_SPRID_WEAPON_M + 64 * 37) && (iWeaponIndex < DEF_SPRID_WEAPON_M + 64 * 37 + 56)) //MStaff3
		|| ((iWeaponIndex >= DEF_SPRID_WEAPON_W + 64 * 37) && (iWeaponIndex < DEF_SPRID_WEAPON_W + 64 * 37 + 56)))//WStaff3
	{
		*iWeaponGlare = 2;
	}
	else if (((iWeaponIndex >= DEF_SPRID_WEAPON_M + 64 * 32) && (iWeaponIndex < DEF_SPRID_WEAPON_M + 64 * 32 + 56)) //BBH
		|| ((iWeaponIndex >= DEF_SPRID_WEAPON_W + 64 * 32) && (iWeaponIndex < DEF_SPRID_WEAPON_W + 64 * 32 + 56)))//BBH
	{
		*iWeaponGlare = 1;
	} // MORLA 2.6 - Agregado el efecto al Dark Knight Barbarian Hammer +15
}

/*********************************************************************************************************************
**  void CGame::Abaddon_corpse(int sX, int sY);		( Snoopy )														**
**  description			: Placeholder for abaddon's death lightnings												**
**********************************************************************************************************************/
void CGame::Abaddon_corpse(int sX, int sY)
{
	int ir = (rand() % 20) - 10;
	_DrawThunderEffect(sX + 30, 0, sX + 30, sY - 10, ir, ir, 1);
	_DrawThunderEffect(sX + 30, 0, sX + 30, sY - 10, ir + 2, ir, 2);
	_DrawThunderEffect(sX + 30, 0, sX + 30, sY - 10, ir - 2, ir, 2);
	ir = (rand() % 20) - 10;
	_DrawThunderEffect(sX - 20, 0, sX - 20, sY - 35, ir, ir, 1);
	_DrawThunderEffect(sX - 20, 0, sX - 20, sY - 35, ir + 2, ir, 2);
	_DrawThunderEffect(sX - 20, 0, sX - 20, sY - 35, ir - 2, ir, 2);
	ir = (rand() % 20) - 10;
	_DrawThunderEffect(sX - 10, 0, sX - 10, sY + 30, ir, ir, 1);
	_DrawThunderEffect(sX - 10, 0, sX - 10, sY + 30, ir + 2, ir + 2, 2);
	_DrawThunderEffect(sX - 10, 0, sX - 10, sY + 30, ir - 2, ir + 2, 2);
	ir = (rand() % 20) - 10;
	_DrawThunderEffect(sX + 50, 0, sX + 50, sY + 35, ir, ir, 1);
	_DrawThunderEffect(sX + 50, 0, sX + 50, sY + 35, ir + 2, ir + 2, 2);
	_DrawThunderEffect(sX + 50, 0, sX + 50, sY + 35, ir - 2, ir + 2, 2);
	ir = (rand() % 20) - 10;
	_DrawThunderEffect(sX + 65, 0, sX + 65, sY - 5, ir, ir, 1);
	_DrawThunderEffect(sX + 65, 0, sX + 65, sY - 5, ir + 2, ir + 2, 2);
	_DrawThunderEffect(sX + 65, 0, sX + 65, sY - 5, ir - 2, ir + 2, 2);
	ir = (rand() % 20) - 10;
	_DrawThunderEffect(sX + 45, 0, sX + 45, sY - 50, ir, ir, 1);
	_DrawThunderEffect(sX + 45, 0, sX + 45, sY - 50, ir + 2, ir + 2, 2);
	_DrawThunderEffect(sX + 45, 0, sX + 45, sY - 50, ir - 2, ir + 2, 2);

	for (int x = sX - 50; x <= sX + 100; x += rand() % 35)
		for (int y = sY - 30; y <= sY + 50; y += rand() % 45)
		{
			ir = (rand() % 20) - 10;
			_DrawThunderEffect(x, 0, x, y, ir, ir, 2);
		}
}

/*********************************************************************************************************************
**  void DebugLog(char * cStr)			( Snoopy )																	**
**  description			: writes data into "Debug.txt"																**
**********************************************************************************************************************/
void CGame::DebugLog(char * cStr)
{
	FILE * pFile;
	char cBuffer[512];
	SYSTEMTIME SysTime;
	pFile = fopen("Debug.txt", "at");
	if (pFile == 0) return;
	ZeroMemory(cBuffer, sizeof(cBuffer));
	GetLocalTime(&SysTime);
	wsprintf(cBuffer, "(%4d:%2d:%2d_%2d:%2d:%2d) - ", SysTime.wYear, SysTime.wMonth, SysTime.wDay, SysTime.wHour, SysTime.wMinute, SysTime.wSecond);
	strcat(cBuffer, cStr);
	strcat(cBuffer, "\n");
	fwrite(cBuffer, 1, strlen(cBuffer), pFile);
	fclose(pFile);
}

void CGame::GetIPByDNS()
{
	ZeroMemory(m_cLogServerAddr, sizeof(m_cLogServerAddr));
	if (m_cLogServerAddrBuffer[0] >= 65 && m_cLogServerAddrBuffer[0] <= 122) {
		char cDnsResult[40];
		struct hostent *host_entry;

		host_entry = gethostbyname(m_cLogServerAddrBuffer);
		if (host_entry == 0){
			MessageBox(m_hWnd, "Failed to get DNS entry for the logserver!", "ERROR1", MB_ICONEXCLAMATION | MB_OK);
			_exit(0);
		}
		ZeroMemory(cDnsResult, sizeof(cDnsResult));
		wsprintf(cDnsResult, "%d.%d.%d.%d",
			(host_entry->h_addr_list[0][0] & 0x00ff),
			(host_entry->h_addr_list[0][1] & 0x00ff),
			(host_entry->h_addr_list[0][2] & 0x00ff),
			(host_entry->h_addr_list[0][3] & 0x00ff));

		strcpy(m_cLogServerAddr, cDnsResult);
	}
	else strcpy(m_cLogServerAddr, m_cLogServerAddrBuffer);
}

void CGame::DrawDialogBox_FriendList(short msX, short msY)//43
{
	short sX, sY, szX, szY;
	int j;
	char cTxt[120];
	char cNicks[13];
	COLORREF tmpColor;

	sX = m_stDialogBoxInfo[43].sX;
	sY = m_stDialogBoxInfo[43].sY;
	szX = m_stDialogBoxInfo[43].sSizeX;
	szY = m_stDialogBoxInfo[43].sSizeY;

	if (m_bUseOldPanels) {
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME2, sX, sY, 2);
	}
	else {
		short limitX, limitY;
		limitX = sX + m_stDialogBoxInfo[43].sSizeX;
		limitY = sY + m_stDialogBoxInfo[43].sSizeY;

		m_DDraw.DrawShadowBox(sX, sY, limitX, limitY, 0, true);
		if (!m_bDialogTrans) m_DDraw.DrawShadowBox(sX, sY, limitX, limitY, 0, true);

		m_DDraw.DrawShadowBox(sX, sY, limitX, sY + 25, 0, true);
		if (!m_bDialogTrans) m_DDraw.DrawShadowBox(sX, sY, limitX, sY + 25, 0, true);
		//PutString_SprFont2(sX + 100, sY + 5, "Magics", 240, 240, 240);
	}

	PutString_SprFont2(sX + 90, sY + 5, "Friends List", 240, 240, 240);

	switch (m_stDialogBoxInfo[43].cMode) {
	case 0:
		for (int i = 0; i < m_iTotalFriends; i++) {
			if (m_cFriends[i] != 0) {
				PutString(sX + 25, (sY + 30 + ((i + 1) * 16)), m_cFriends[i], RGB(70, 130, 180));
			}
		}

		if ((msX > sX + 25) && (msX < sX + 25 + 25) && (msY > (sY + 45 + (14 * 16)) + 20) && (msY < (sY + 45 + (15 * 16)) + 20)) {
			PutString_SprFont2(sX + 25, (sY + 45 + (14 * 16)) + 20, "Add", 255, 255, 255);
		}
		else
		{
			PutString_SprFont2(sX + 25, (sY + 45 + (14 * 16)) + 20, "Add", 19, 104, 169);
		}
		if (m_iFriendIndex != -1) {
			PutString(sX + 25, sY + 30 + (m_iFriendIndex * 16), m_cFriends[m_iFriendIndex - 1], RGB(255, 255, 255));
			m_stDialogBoxInfo[43].sV1 = m_iFriendIndex - 1;
			//wsprintf(cTxt, "%s", m_cFriends[m_iFriendIndex - 1]);
			m_iFriendIndex2 = m_iFriendIndex - 1;
			//PutString(sX + 20, sY + 45 + (13 * 16), "Player name:", RGB(0, 255, 0));
			//PutString(sX + 110, sY + 45 + (13 * 16), cTxt, RGB(255, 255, 0));

			if ((msX > sX + 80+5) && (msX < sX + 80 + 30 + 5) && (msY > (sY + 45 + (14 * 16)) + 20) && (msY < (sY + 45 + (15 * 16)) + 20)) {
				PutString_SprFont2(sX + 80 + 5, (sY + 45 + (14 * 16)) + 20, "Delete", 255, 255, 255);
			}
			else
			{
				PutString_SprFont2(sX + 80 + 5, (sY + 45 + (14 * 16)) + 20, "Delete", 19, 104, 169);
			}

			if ((msX > sX + 140+20) && (msX < sX + 140 + 45 + 20) && (msY > (sY + 45 + (14 * 16)) + 20) && (msY < (sY + 45 + (15 * 16)) + 20)) {
				PutString_SprFont2(sX + 140 + 20, (sY + 45 + (14 * 16)) + 20, "Whisper", 255, 255, 255);
			}
			else
			{
				PutString_SprFont2(sX + 140 + 20, (sY + 45 + (14 * 16)) + 20, "Whisper", 19, 104, 169);
			}
		}
		break;
	}
}

void CGame::DlgBoxClick_FriendList(short msX, short msY) // 43 - drajwer
{
	short sX, sY, szX, szY;
	int tmp, i;
	char cTxt[100];

	sX = m_stDialogBoxInfo[43].sX;
	sY = m_stDialogBoxInfo[43].sY;
	szX = m_stDialogBoxInfo[43].sSizeX;
	szY = m_stDialogBoxInfo[43].sSizeY;

	if ((msX > sX + 25) && (msX < sX + szX - 25) && (msY > sY + 30 + 16) && (msY < sY + 30 + (13 * 16))) {
		tmp = (msY - sY - 30 - 16) / 16;
		tmp++;
		if (strcmp(m_cFriends[tmp - 1], " ") != 0) { //izzit not empty?
			m_iFriendIndex = tmp;
		}
		else {
			m_iFriendIndex = -1;
		}
		return;
	}
	if ((msX > sX + 25) && (msX < sX + 25 + 25) && (msY > (sY + 45 + (14 * 16)) + 20) && (msY < (sY + 45 + (15 * 16)) + 20)) {
		if (m_iTotalFriends == 12) {
			AddEventList("You can not have more than 12 friends in list.", 10);
			return;
		}
		m_bIsGetPointingMode = true;
		m_iPointCommandType = 250;
		PlaySound('E', 14, 5);
		AddEventList("Click the character which you want to add.", 10); //pointing mode 
		return;
	}
	if ((msX > sX + 80 + 5) && (msX < sX + 80 + 30 + 5) && (msY > (sY + 45 + (14 * 16)) + 20) && (msY < (sY + 45 + (15 * 16)) + 20)) {
		for (i = m_iFriendIndex2 + 1; i < 13; i++) { //simple delete
			ZeroMemory(cTxt, sizeof(cTxt)); //just move down elements
			memcpy(cTxt, m_cFriends[i], 10);
			ZeroMemory(m_cFriends[i - 1], sizeof(m_cFriends[i - 1]));
			memcpy(m_cFriends[i - 1], cTxt, 10);
		}
		m_iTotalFriends--;
		SaveFriendList();
	}

	if ((msX > sX + 140 + 20) && (msX < sX + 140 + 45 + 20) && (msY > (sY + 45 + (14 * 16)) + 20) && (msY < (sY + 45 + (15 * 16)) + 20)) {
		ZeroMemory(cTxt, sizeof(cTxt));
		wsprintf(cTxt, "/to %s", m_cFriends[m_iFriendIndex2]);
		bSendCommand(MSGID_COMMAND_CHATMSG, 0, 0, 0, 0, 0, cTxt);
	}
}
void CGame::SaveFriendList()
{
	FILE* f = fopen("contents\\FriendList.txt", "w");
	char wr[16];

	if (f == 0) return;
	for (int i = 0; i < m_iTotalFriends; i++) {
		ZeroMemory(wr, sizeof(wr));
		wsprintf(wr, "%s\n", m_cFriends[i]);
		fputs(wr, f);
	}
	fclose(f);
}
void CGame::LoadFriendList()
{
	char buf[1024]; int konieclinii; unsigned long linie = 0;
	FILE* f = fopen("contents\\FriendList.txt", "rt");
	if (f == 0) return; // Centuu: si el archivo no existe, crash
	m_iTotalFriends = 0;
	while (fgets(buf, 1024, f)) {
		konieclinii = 0;
		int i = strlen(buf);
		if (i > 0 && buf[--i] == '\n') {
			buf[i] = 0; // kasujemy znak konca linii
			konieclinii = 1;
			linie++;
		}
		if (linie - konieclinii < 13) {
			strcpy(m_cFriends[linie - konieclinii], buf);
			m_iTotalFriends++;
		}
	}
	if (m_iTotalFriends > 12) m_iTotalFriends = 12;
	fclose(f);
}

/*********************************************************************************************************************
**  void CGame::CheckProcesses()	( Snoopy )																		**
**  description			: 	Check if forbiden precesses running (1st, evident function)								**
**********************************************************************************************************************/
bool CGame::CheckProcesses()
{
	HANDLE hSnapShot;
	PROCESSENTRY32 uProcess;
	bool r;
	bool bFound = false;
	char *Ufoundprocess;
	hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
	uProcess.dwSize = (UINT32)sizeof(PROCESSENTRY32);
	r = Process32First(hSnapShot, &uProcess);
	do
	{
		Ufoundprocess = _strupr(uProcess.szExeFile);
		if (strcmp(Ufoundprocess, "SPEEDERXP.EXE") == 0)	 bFound = true;
		if (strcmp(Ufoundprocess, "WpeSpy.dll") == 0)	 bFound = true;
		if (strcmp(Ufoundprocess, "WpeSpy.DLL") == 0)	 bFound = true;
		if (strcmp(Ufoundprocess, "SetPriv.DLL") == 0)	 bFound = true;
		if (strcmp(Ufoundprocess, "TSpeech.DLL") == 0)	 bFound = true;
		if (strcmp(Ufoundprocess, "THotkeys.DLL") == 0)	 bFound = true;
		if (strcmp(Ufoundprocess, "imagehlp.DLL") == 0)	 bFound = true;
		if (strcmp(Ufoundprocess, "DBGHELP.DLL") == 0)	 bFound = true;
		if (strcmp(Ufoundprocess, "TSearch.exe") == 0)			 bFound = true;
		if (strcmp(Ufoundprocess, "TSearch.EXE") == 0)			 bFound = true;
		if (strcmp(Ufoundprocess, "WPE PRO.EXE") == 0)	 bFound = true;
		if (strcmp(Ufoundprocess, "TEHELGAME.EXE") == 0)	 bFound = true;
		if (strcmp(Ufoundprocess, "CMD.EXE") == 0)		 bFound = true;
		if (strcmp(Ufoundprocess, "WPE PRO - modified.exe") == 0)		 bFound = true;
		if (strcmp(Ufoundprocess, "SPEED HACK.EXE") == 0)  bFound = true;
		if (strcmp(Ufoundprocess, "SPEEDHACKNT.EXE") == 0)  bFound = true;
		if (strcmp(Ufoundprocess, "ASPEEDER.EXE") == 0)	 bFound = true;
		if (strcmp(Ufoundprocess, "WPEPRO.EXE") == 0)		 bFound = true;
		if (strcmp(Ufoundprocess, "351HAX.EXE") == 0)		 bFound = true;
		if (strcmp(Ufoundprocess, "CATASTROPHE.EXE") == 0) bFound = true;
		if (memcmp(Ufoundprocess, "WPE.EXE", 3) == 0) bFound = true;
		if (memcmp(Ufoundprocess, "TSearchDll.DLL", 10) == 0) bFound = true;
		if (memcmp(Ufoundprocess, "TSearchDll.DLL", 3) == 0) bFound = true;
		if (memcmp(Ufoundprocess, "TSe.exe", 3) == 0) bFound = true;
		if (memcmp(Ufoundprocess, "TSearch.EXE", 7) == 0) bFound = true;
		if (memcmp(Ufoundprocess, "TSearch.exe", 7) == 0) bFound = true;

		if (strcmp(Ufoundprocess, "Catastrophe v0.1.EXE") == 0) bFound = true;
		if (strcmp(Ufoundprocess, "Catastrophe v1.2.EXE") == 0) bFound = true;
		if (strcmp(Ufoundprocess, "WPePro.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "WPePro 0.9x.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "WPePro 1.3.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "WPe.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Speed Gear.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Speed Gear 5.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "UoPilot.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Cheat Engine.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Cheat Engine 5.3.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Cheat Master 1.0.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "PINACCLE.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "WildProxy.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "WildProxy v0.3.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "WildProxy v0.2.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "WildProxy v0.1.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "WildProxy v1.0 Public.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Speed Hack.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Speed Hack Simplifier 1.3 Test.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "99.62t Speed Hack.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "SpotHack 1.1.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "MJB Perfect DL Bot.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Speed Hack Simplifier 1.1.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "HahaMu 1.16.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Cheat Engine.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Cheat Engine 5.0.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Cheat Engine 5.2.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Cheat Engine 5.1.1.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Cheat Engine 5.4.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Game Speed Changer.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Xelerator 1.4.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Capotecheat.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Cheat4Fun v0.9 Beta.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "!xSpeed.net3.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "AutoBuff D-C.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Cheat Happens.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Cheat Happens v3.9b1.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Cheat Happens v3.95b2.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Cheat Happens v3.95b3.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Cheat Happens v3.96b2.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "HastyMu.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "HastyMu v0.2.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "FunnyZhyper.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "FunnyZhyper v5.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Lipsum.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Lipsum v2.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "MuPie.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "MuPie v2 Beta.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "MuPie HG v2.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "MuPie HG v3.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "DKAEMultiStrike_MU10e.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "DKAEMultiStrike_MU-97.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Cheat Engine.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Cheat Engine 5.5.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "rPE rEdoX Packet Editor.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Gold & Dupe.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Catastrophe.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Catastrophe v0.1.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Catastrophe v1.2.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "WPePro.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "WPePro 0.9a.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "WPePro 1.3.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "WPE PRO - modified.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "WPE PRO.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "asd.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "hack.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Permit.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "T Search.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "TSearch.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Speed Gear.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Speed Gear 5.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Speed Gear 6.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "WildProxy.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "WildProxy v1.0 Public.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "WildProxy v1.0.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "WildProxy v0.1.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "WildProxy v0.2.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "WildProxy v0.3.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Speed Hack.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Speed HackSimplifier 1.1.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Speed HackSimplifier 1.2.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Speed HackSimplifier 1.3.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Speed HackSimplifier 1.0.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Speed HackSimplifier 1.0 Test.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "!xSpeed.net 2.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "!xSpeed.net 3.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "!xSpeed.net 6.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "AUTO Clicker.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Nsauditor 1.9.1.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Super Bot.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "!xSpeed 6.0.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Nsauditor 1.9.1.EXE") == 0)      bFound = true;

		if (strcmp(Ufoundprocess, "SPEEDERXP.EXE") == 0)	bFound = true;
		if (strcmp(Ufoundprocess, "MM.EXE") == 0)			bFound = true;
		if (strcmp(Ufoundprocess, "WPE PRO.EXE") == 0)	bFound = true;
		if (strcmp(Ufoundprocess, "TEHELGAME.EXE") == 0)	bFound = true;
		if (strcmp(Ufoundprocess, "HELGAME.EXE") == 0)	bFound = true;
		if (strcmp(Ufoundprocess, "SPEED HACK.EXE") == 0) bFound = true;
		if (strcmp(Ufoundprocess, "SPEEDHACKNT.EXE") == 0) bFound = true;
		if (strcmp(Ufoundprocess, "ASPEEDER.EXE") == 0)	bFound = true;
		if (strcmp(Ufoundprocess, "WPEPRO.EXE") == 0)		bFound = true;
		if (strcmp(Ufoundprocess, "SOFTSNOOP.EXE") == 0)	bFound = true;
		if (strcmp(Ufoundprocess, "TS.EXE") == 0)			bFound = true;
		if (strcmp(Ufoundprocess, "IP_TOOLS.EXE") == 0)	bFound = true;
		if (strcmp(Ufoundprocess, "TS.EXE") == 0)			bFound = true;
		if (strcmp(Ufoundprocess, "HBMAGIC190.EXE") == 0)	bFound = true;
		if (strcmp(Ufoundprocess, "FRHED.EXE") == 0)		bFound = true;
		if (strcmp(Ufoundprocess, "HWORKS32.EXE") == 0)	bFound = true;
		if (strcmp(Ufoundprocess, "MSCHED.EXE") == 0)		bFound = true;
		if (strcmp(Ufoundprocess, "HBSERVERKILLER-0.2.EXE") == 0)		bFound = true;
		if (strcmp(Ufoundprocess, "EIDTZBIIEIDTZEIEX_v1-1.EXE") == 0)	bFound = true;
		if (strcmp(Ufoundprocess, "EIDTZBIIEIDTZEIEX_v2-2.EXE") == 0)	bFound = true;
		if (strcmp(Ufoundprocess, "PSNIFF.EXE") == 0)		bFound = true;
		if (strcmp(Ufoundprocess, "SPEEDGEAR.EXE") == 0)	bFound = true;
		if (strcmp(Ufoundprocess, "351HAX.EXE") == 0)		 bFound = true;
		if (strcmp(Ufoundprocess, "CATASTROPHE.EXE") == 0) bFound = true;
		if (strcmp(Ufoundprocess, "Catastrophe v0.1.EXE") == 0) bFound = true;
		if (strcmp(Ufoundprocess, "Catastrophe v1.2.EXE") == 0) bFound = true;
		if (strcmp(Ufoundprocess, "WPePro.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "WPePro 0.9x.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "WPePro 1.3.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Speed Gear.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Speed Gear 5.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "UoPilot.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Cheat Engine.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Cheat Engine 5.3.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Cheat Master 1.0.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "PINACCLE.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "WildProxy.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "WildProxy v0.3.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "WildProxy v0.2.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "WildProxy v0.1.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "WildProxy v1.0 Public.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Speed Hack.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Speed Hack Simplifier 1.3 Test.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "99.62t Speed Hack.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "SpotHack 1.1.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "MJB Perfect DL Bot.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Speed Hack Simplifier 1.1.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "HahaMu 1.16.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Cheat Engine.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Cheat Engine 5.0.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Cheat Engine 5.2.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Cheat Engine 5.1.1.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Cheat Engine 5.4.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Game Speed Changer.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Xelerator 1.4.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Capotecheat.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Cheat4Fun v0.9 Beta.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "!xSpeed.net3.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "AutoBuff D-C.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Cheat Happens.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Cheat Happens v3.9b1.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Cheat Happens v3.95b2.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Cheat Happens v3.95b3.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Cheat Happens v3.96b2.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "HastyMu.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "HastyMu v0.2.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "FunnyZhyper.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "FunnyZhyper v5.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Lipsum.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Lipsum v2.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "MuPie.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "MuPie v2 Beta.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "MuPie HG v2.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "MuPie HG v3.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "DKAEMultiStrike_MU10e.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "DKAEMultiStrike_MU-97.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Cheat Engine.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Cheat Engine 5.5.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "rPE rEdoX Packet Editor.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Gold & Dupe.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Catastrophe.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Catastrophe v0.1.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Catastrophe v1.2.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "WPePro.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "WPePro 0.9a.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "WPePro 1.3.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Permit.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "TSEARCH.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Speed Gear.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Speed Gear 5.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Speed Gear 6.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "WildProxy.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "WildProxy v1.0 Public.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "WildProxy v1.0.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "WildProxy v0.1.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "WildProxy v0.2.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "WildProxy v0.3.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Speed Hack.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Speed HackSimplifier 1.1.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Speed HackSimplifier 1.2.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Speed HackSimplifier 1.3.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Speed HackSimplifier 1.0.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Speed HackSimplifier 1.0 Test.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "!xSpeed.net 2.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "!xSpeed.net 3.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "!xSpeed.net 6.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "AUTO Clicker.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Nsauditor 1.9.1.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "Super Bot.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "!xSpeed 6.0.EXE") == 0)      bFound = true;
		if (strcmp(Ufoundprocess, "CHEATENGINE-X86_64.EXE") == 0)	bFound = true;
		if (strcmp(Ufoundprocess, "WPE PRO - MODIFIED.EXE") == 0)	bFound = true;
		if (strcmp(Ufoundprocess, "CMD.EXE") == 0)	bFound = true;
		if (strcmp(Ufoundprocess, "powershell.EXE") == 0)	bFound = true;
		if (strcmp(Ufoundprocess, "powershell_ise.EXE") == 0)	bFound = true;
		r = Process32Next(hSnapShot, &uProcess);
	} while (r);
	CloseHandle(hSnapShot);
	if (bFound)
	{
		if (m_cGameMode == DEF_GAMEMODE_ONMAINGAME)
		{
			ChangeGameMode(DEF_GAMEMODE_ONQUIT);
		}
	}
	//m_bHackMoveBlocked = bFound;
	return bFound;
}


bool CGame::bCheckItemEquiped(char itemName[])
{
	for (int i = 0; i < DEF_MAXITEMS; i++)
	{
		if (m_pItemList[i] != 0)
		{
			if (strcmp(m_pItemList[i]->m_cName, itemName) == 0)
			{
				for (int x = 0; x < DEF_MAXITEMEQUIPPOS; x++)
				{
					if (m_sItemEquipmentStatus[x] == i) return true;
				}
			}
		}
	}
	return false;
}

//50Cent - Repair All
void CGame::NotifyMsg_RepairAllPrices(char * pData)
{
	short * sp;
	char  * cp;
	int i;

	totalPrice = 0;
	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
	sp = (short *)cp;
	totalItemRepair = *sp;
	cp += 2;

	for (i = 0; i < totalItemRepair; i++)
	{
		m_stRepairAll[i].index = *cp;
		cp++;

		sp = (short *)cp;
		m_stRepairAll[i].price = *sp;
		cp += 2;

		totalPrice += m_stRepairAll[i].price;
	}
	if (totalItemRepair == 0)
		EnableDialogBox(52, 1, 0, 0);
	else
		EnableDialogBox(52, 0, 0, 0);
}

void CGame::DrawDialogBox_RepairAll(short msX, short msY, short msZ) //4LifeX Modified - 50Cent - Repair All
{
	short sX, sY, szX;
	char cTxt[120];
	int i, iTotalLines, iPointerLoc;
	float d1, d2, d3;

	sX = m_stDialogBoxInfo[52].sX;
	sY = m_stDialogBoxInfo[52].sY;
	szX = m_stDialogBoxInfo[52].sSizeX;

	DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME6, sX, sY, 2);
	DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_TEXT2, sX, sY, 10);

	for (i = 0; i < 15; i++)
		if ((i + m_stDialogBoxInfo[52].sView) < totalItemRepair)
		{
			ZeroMemory(cTxt, sizeof(cTxt));
			wsprintf(cTxt, "%s - Cost: %d", m_pItemList[m_stRepairAll[i + m_stDialogBoxInfo[52].sView].index]->m_cName, m_stRepairAll[i + m_stDialogBoxInfo[52].sView].price);

			PutString(sX + 30, sY + 45 + i * 15, cTxt, RGB(5, 5, 5));
			m_bIsItemDisabled[m_stRepairAll[i + m_stDialogBoxInfo[52].sView].index] = true;
		}


	iTotalLines = totalItemRepair;
	if (iTotalLines > 15)
	{
		d1 = (float)m_stDialogBoxInfo[52].sView;
		d2 = (float)(iTotalLines - 15);
		d3 = (274.0f * d1) / d2;
		iPointerLoc = (int)d3;
	}
	else iPointerLoc = 0;

	if (iTotalLines > 15)
	{
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME6, sX, sY, 1);
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME6, sX + 242, sY + iPointerLoc + 35, 7);
	}

	if (iTotalLines > 15)
	{
		if (iGetTopDialogBoxIndex() == 56 && msZ != 0)
		{
			if (msZ > 0) m_stDialogBoxInfo[52].sView--;
			if (msZ < 0) m_stDialogBoxInfo[52].sView++;
			m_DInput.m_sZ = 0;
		}

		if (m_stDialogBoxInfo[52].sView < 0)
			m_stDialogBoxInfo[52].sView = 0;

		if (iTotalLines > 15 && m_stDialogBoxInfo[52].sView > iTotalLines - 15)
			m_stDialogBoxInfo[52].sView = iTotalLines - 15;
	}




	if (totalItemRepair > 0)
	{
		if ((msX >= sX + DEF_LBTNPOSX) && (msX <= sX + DEF_LBTNPOSX + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
		{
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 43);
		}
		else
		{
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 42);
		}

		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
		{
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 17);
		}
		else
		{
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 16);
		}
		ZeroMemory(cTxt, sizeof(cTxt));
		wsprintf(cTxt, "Total cost : %d", totalPrice);
		PutString(sX + 30, sY + 270, cTxt, RGB(5, 5, 5));
	}
	else
	{
		PutAlignedString(sX, sX + szX, sY + 140, "There are no items to repair.", 5, 5, 5);
		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
		{
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 17);
		}
		else
		{
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 16);
		}
	}
}

//4LifeX Modified - 50Cent - Repair All
void CGame::DlgBoxClick_RepairAll(short msX, short msY)
{
	short sX, sY;
	int i;

	sX = m_stDialogBoxInfo[52].sX;
	sY = m_stDialogBoxInfo[52].sY;

	for (i = 0; i < 15; i++)
		if ((i + m_stDialogBoxInfo[52].sView) < totalItemRepair)
		{
			if ((msX >= sX + 30) && (msX <= sX + 30 + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
			{
				bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_REQ_REPAIRALLCONFIRM, 0, 0, 0, 0, 0);
				DisableDialogBox(52);
			}

			if ((msX >= sX + 154) && (msX <= sX + 154 + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
				DisableDialogBox(52);
		}
		else
		{
			if ((msX >= sX + 154) && (msX <= sX + 154 + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
				DisableDialogBox(52);
		}
}

//New GM Panel by Magn0S :D
void CGame::DlgBoxClick_GMPanel(short msX, short msY)
{
	short sX, sY;
	int tmp, i, iaddx, iaddy;
	char cTxt[120], cMapTP[30], cSummon[100], cMsg[100], cTemp[21];
	int iNext = 0;
	int iNextB = 0;
	int iNextC = 0;
	int FixChat;
	sX = m_stDialogBoxInfo[56].sX;
	sY = m_stDialogBoxInfo[56].sY;
	bool bFlag = false;

	ZeroMemory(cMapTP, sizeof(cMapTP));
	ZeroMemory(cSummon, sizeof(cSummon));
	ZeroMemory(cMsg, sizeof(cMsg));

#ifdef RES_HIGH
	FixChat = 532;
#else
	FixChat = 414;
#endif

	switch (m_stDialogBoxInfo[56].cMode) {
	case 0:
		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			m_stDialogBoxInfo[56].cMode = 21; // Admin Commands
			PlaySound('E', 14, 5);
		}
		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			m_stDialogBoxInfo[56].cMode = 2; // GameServer Administration
			PlaySound('E', 14, 5);
		}
		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			if (m_iAdminUserLevel > 2) {
				m_stDialogBoxInfo[56].cMode = 22; // Event Manager
				PlaySound('E', 14, 5);
			} else { AddEventList("Admin User Level is too low for this action.", 10); }
		}
		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			m_stDialogBoxInfo[56].cMode = 23; // Player Manipulation
			PlaySound('E', 14, 5);
		}
		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			//m_stDialogBoxInfo[56].cMode = 22; // Check Online
			//EnableDialogBox(60, 0, 0, 0); 
			m_stDialogBoxInfo[56].cMode = 40; // Player Manipulation
			//AddEventList("Online Users List Requested...", 10);
			PlaySound('E', 14, 5);
		}
		break;

	case 2:
		iNext += 2;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			if (m_iAdminUserLevel > 2) {
				m_stDialogBoxInfo[56].cMode = 20; // Map Restrictions
				PlaySound('E', 14, 5);
			}
			else { AddEventList("Admin User Level is too low for this action.", 10); }
		}

		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			if (m_iAdminUserLevel > 3) {
				m_stDialogBoxInfo[56].cMode = 24; // Drops / Damage server manipulation
				PlaySound('E', 14, 5);
			}
			else { AddEventList("Admin User Level is too low for this action.", 10); }
		}

		//Back
		if ((msX > sX + 220) && (msX < sX + 240) && (msY > sY + 315) && (msY < sY + 330)) {
			m_stDialogBoxInfo[56].cMode = 0; // Return to Game Adm
			PlaySound('E', 14, 5);
		}
		break;

	case 10: // Lock Player in BI
		iaddx = 43;
		iaddy = -140;
		//-------------------------------------------------------------------------------------------------------------------------------
		if ((msX >= sX + 127 + iaddx) && (msX <= sX + 150 + iaddx) && (msY >= sY + 209 + iaddy) && (msY <= sY + 230 + iaddy))
		{
			m_stDialogBoxInfo[56].sV3 += 100;
			if (m_stDialogBoxInfo[56].sV3 >= 365)
				m_stDialogBoxInfo[56].sV3 = 365;
		}

		if ((msX >= sX + 127 + iaddx) && (msX <= sX + 150 + iaddx) && (msY >= sY + 234 + iaddy) && (msY <= sY + 251 + iaddy))
		{
			m_stDialogBoxInfo[56].sV3 -= 100;
			if (m_stDialogBoxInfo[56].sV3 <= 0)
				m_stDialogBoxInfo[56].sV3 = 0;
		}
		//-------------------------------------------------------------------------------------------------------------------------------
		if ((msX >= sX + 145 + iaddx) && (msX <= sX + 162 + iaddx) && (msY >= sY + 209 + iaddy) && (msY <= sY + 230 + iaddy))
		{
			m_stDialogBoxInfo[56].sV3 += 10;
			if (m_stDialogBoxInfo[56].sV3 >= 365)
				m_stDialogBoxInfo[56].sV3 = 365;
		}

		if ((msX >= sX + 145 + iaddx) && (msX <= sX + 162 + iaddx) && (msY >= sY + 234 + iaddy) && (msY <= sY + 251 + iaddy))
		{
			m_stDialogBoxInfo[56].sV3 -= 10;
			if (m_stDialogBoxInfo[56].sV3 <= 0)
				m_stDialogBoxInfo[56].sV3 = 0;
		}
		//-------------------------------------------------------------------------------------------------------------------------------
		if ((msX >= sX + 163 + iaddx) && (msX <= sX + 180 + iaddx) && (msY >= sY + 209 + iaddy) && (msY <= sY + 230 + iaddy))
		{
			m_stDialogBoxInfo[56].sV3++;
			if (m_stDialogBoxInfo[56].sV3 >= 365)
				m_stDialogBoxInfo[56].sV3 = 365;
		}

		if ((msX >= sX + 163 + iaddx) && (msX <= sX + 180 + iaddx) && (msY >= sY + 234 + iaddy) && (msY <= sY + 251 + iaddy))
		{
			m_stDialogBoxInfo[56].sV3--;
			if (m_stDialogBoxInfo[56].sV3 <= 0)
				m_stDialogBoxInfo[56].sV3 = 0;
		}

		iNext += 5; // Lock Char
		if ((msX > sX + 165) && (msX < sX + 255) && (msY > sY + iNext * 17 + 50) && (msY < sY + iNext * 17 + 70)) {
			if ((bListSelected) && strlen(cCharSelection) > 2) {
				if ((strcmp(cCharSelection, "YoKo[GM]") == 0) || (strcmp(cCharSelection, "Centuu[GM]") == 0) || (strcmp(cCharSelection, "NiXuGM") == 0)) return;
				PlaySound('E', 14, 5);
				bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_CLIENTMSG, 0, 8, m_stDialogBoxInfo[56].sV3, 0, cCharSelection);
			}
			else {
				ZeroMemory(m_cChatMsg, sizeof(m_cChatMsg));
				strcpy(m_cChatMsg, "/lock (not working) ");
				StartInputString(10, FixChat, sizeof(m_cChatMsg), m_cChatMsg);
				PlaySound('E', 14, 5);
			}
		}

		iNext += 3; // Block Char
		if ((msX > sX + 165) && (msX < sX + 255) && (msY > sY + iNext * 17 + 55) && (msY < sY + iNext * 17 + 75)) {
			if ((bListSelected) && strlen(cCharSelection) > 2) {
				if ((strcmp(cCharSelection, "YoKo[GM]") == 0) || (strcmp(cCharSelection, "Centuu[GM]") == 0) || (strcmp(cCharSelection, "NiXuGM") == 0)) return;
				bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_CLIENTMSG, 0, 9, m_stDialogBoxInfo[56].sV3, 0, cCharSelection);
				PlaySound('E', 14, 5);
			}
			else {
				ZeroMemory(m_cChatMsg, sizeof(m_cChatMsg));
				strcpy(m_cChatMsg, "/block (not working) ");
				StartInputString(10, FixChat, sizeof(m_cChatMsg), m_cChatMsg);
				PlaySound('E', 14, 5);
			}
		}

		iNext += 3; // Ban IP
		if ((msX > sX + 173) && (msX < sX + 240) && (msY > sY + iNext * 17 + 60) && (msY < sY + iNext * 17 + 80)) {
			if ((bListSelected) && strlen(cCharSelection) > 2) {
				if ((strcmp(cCharSelection, "YoKo[GM]") == 0) || (strcmp(cCharSelection, "Centuu[GM]") == 0) || (strcmp(cCharSelection, "NiXuGM") == 0)) return;
				ZeroMemory(cTemp, sizeof(cTemp));
				wsprintf(cTemp, "/banip %s", cCharSelection);
				bSendCommand(MSGID_COMMAND_CHATMSG, 0, 0, 0, 0, 0, cTemp);
				PlaySound('E', 14, 5);
			}
			else {
				ZeroMemory(m_cChatMsg, sizeof(m_cChatMsg));
				strcpy(m_cChatMsg, "/banip Name ");
				StartInputString(10, FixChat, sizeof(m_cChatMsg), m_cChatMsg);
				PlaySound('E', 14, 5);
			}
		}

		//Back
		if ((msX > sX + 220) && (msX < sX + 240) && (msY > sY + 315) && (msY < sY + 330)) {
			m_stDialogBoxInfo[56].cMode = 0; // Return to Game Adm
			PlaySound('E', 14, 5);
		}
		break;

	case 19: // Set Player Statistics
		iaddx = 43;
		iaddy = -105;
		//-------------------------------------------------------------------------------------------------------------------------------
		if ((msX >= sX + 127 + iaddx) && (msX <= sX + 150 + iaddx) && (msY >= sY + 209 + iaddy) && (msY <= sY + 230 + iaddy))
		{
			m_stDialogBoxInfo[56].sV3 += 100;
			if (m_stDialogBoxInfo[56].sV3 >= 999)
				m_stDialogBoxInfo[56].sV3 = 999;
		}

		if ((msX >= sX + 127 + iaddx) && (msX <= sX + 150 + iaddx) && (msY >= sY + 234 + iaddy) && (msY <= sY + 251 + iaddy))
		{
			m_stDialogBoxInfo[56].sV3 -= 100;
			if (m_stDialogBoxInfo[56].sV3 <= 0)
				m_stDialogBoxInfo[56].sV3 = 0;
		}
		//-------------------------------------------------------------------------------------------------------------------------------
		if ((msX >= sX + 145 + iaddx) && (msX <= sX + 162 + iaddx) && (msY >= sY + 209 + iaddy) && (msY <= sY + 230 + iaddy))
		{
			m_stDialogBoxInfo[56].sV3 += 10;
			if (m_stDialogBoxInfo[56].sV3 >= 999)
				m_stDialogBoxInfo[56].sV3 = 999;
		}

		if ((msX >= sX + 145 + iaddx) && (msX <= sX + 162 + iaddx) && (msY >= sY + 234 + iaddy) && (msY <= sY + 251 + iaddy))
		{
			m_stDialogBoxInfo[56].sV3 -= 10;
			if (m_stDialogBoxInfo[56].sV3 <= 0)
				m_stDialogBoxInfo[56].sV3 = 0;
		}
		//-------------------------------------------------------------------------------------------------------------------------------
		if ((msX >= sX + 163 + iaddx) && (msX <= sX + 180 + iaddx) && (msY >= sY + 209 + iaddy) && (msY <= sY + 230 + iaddy))
		{
			m_stDialogBoxInfo[56].sV3++;
			if (m_stDialogBoxInfo[56].sV3 >= 999)
				m_stDialogBoxInfo[56].sV3 = 999;
		}

		if ((msX >= sX + 163 + iaddx) && (msX <= sX + 180 + iaddx) && (msY >= sY + 234 + iaddy) && (msY <= sY + 251 + iaddy))
		{
			m_stDialogBoxInfo[56].sV3--;
			if (m_stDialogBoxInfo[56].sV3 <= 0)
				m_stDialogBoxInfo[56].sV3 = 0;
		}

		iNext += 7;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			if ((bListSelected) && strlen(cCharSelection) > 2 && m_stDialogBoxInfo[56].sV3 > 0) {
				ZeroMemory(cTemp, sizeof(cTemp));
				wsprintf(cTemp, "/setek %s %d", cCharSelection, m_stDialogBoxInfo[56].sV3);
				bSendCommand(MSGID_COMMAND_CHATMSG, 0, 0, 0, 0, 0, cTemp);
				m_stDialogBoxInfo[56].sV3 = 0;
				PlaySound('E', 14, 5);
			}
			else {
				ZeroMemory(m_cChatMsg, sizeof(m_cChatMsg));
				strcpy(m_cChatMsg, "/setek ");
				StartInputString(10, FixChat, sizeof(m_cChatMsg), m_cChatMsg);
				PlaySound('E', 14, 5);
			}
		}
		iNext += 2;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			if ((bListSelected) && strlen(cCharSelection) > 2 && m_stDialogBoxInfo[56].sV3 > 0) {
				ZeroMemory(cTemp, sizeof(cTemp));
				wsprintf(cTemp, "/setcontrib %s %d", cCharSelection, m_stDialogBoxInfo[56].sV3);
				bSendCommand(MSGID_COMMAND_CHATMSG, 0, 0, 0, 0, 0, cTemp);
				m_stDialogBoxInfo[56].sV3 = 0;
				PlaySound('E', 14, 5);
			}
			else {
				ZeroMemory(m_cChatMsg, sizeof(m_cChatMsg));
				strcpy(m_cChatMsg, "/setcontrib ");
				StartInputString(10, FixChat, sizeof(m_cChatMsg), m_cChatMsg);
				PlaySound('E', 14, 5);
			}
		}
		iNext += 2;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			if ((bListSelected) && strlen(cCharSelection) > 2 && m_stDialogBoxInfo[56].sV3 > 0) {
				ZeroMemory(cTemp, sizeof(cTemp));
				wsprintf(cTemp, "/setmp %s %d", cCharSelection, m_stDialogBoxInfo[56].sV3);
				bSendCommand(MSGID_COMMAND_CHATMSG, 0, 0, 0, 0, 0, cTemp);
				m_stDialogBoxInfo[56].sV3 = 0;
				PlaySound('E', 14, 5);
			}
			else {
				ZeroMemory(m_cChatMsg, sizeof(m_cChatMsg));
				strcpy(m_cChatMsg, "/setmp ");
				StartInputString(10, FixChat, sizeof(m_cChatMsg), m_cChatMsg);
				PlaySound('E', 14, 5);
			}
		}
		iNext += 2;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			if ((bListSelected) && strlen(cCharSelection) > 2 && m_stDialogBoxInfo[56].sV3 > 0) {
				ZeroMemory(cTemp, sizeof(cTemp));
				wsprintf(cTemp, "/setrep %s %d", cCharSelection, m_stDialogBoxInfo[56].sV3);
				bSendCommand(MSGID_COMMAND_CHATMSG, 0, 0, 0, 0, 0, cTemp);
				m_stDialogBoxInfo[56].sV3 = 0;
				PlaySound('E', 14, 5);
			}
			else {
				ZeroMemory(m_cChatMsg, sizeof(m_cChatMsg));
				strcpy(m_cChatMsg, "/setrep ");
				StartInputString(10, FixChat, sizeof(m_cChatMsg), m_cChatMsg);
				PlaySound('E', 14, 5);
			}
		}
		iNext += 2;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			if ((bListSelected) && strlen(cCharSelection) > 2 && m_stDialogBoxInfo[56].sV3 > 0) {
				ZeroMemory(cTemp, sizeof(cTemp));
				wsprintf(cTemp, "/setcoin %s %d", cCharSelection, m_stDialogBoxInfo[56].sV3);
				bSendCommand(MSGID_COMMAND_CHATMSG, 0, 0, 0, 0, 0, cTemp);
				m_stDialogBoxInfo[56].sV3 = 0;
				PlaySound('E', 14, 5);
			}
			else {
				ZeroMemory(m_cChatMsg, sizeof(m_cChatMsg));
				strcpy(m_cChatMsg, "/setcoin ");
				StartInputString(10, FixChat, sizeof(m_cChatMsg), m_cChatMsg);
				PlaySound('E', 14, 5);
			}
		}

		//Back
		if ((msX > sX + 220) && (msX < sX + 240) && (msY > sY + 315) && (msY < sY + 330)) {
			m_stDialogBoxInfo[56].cMode = 23; // Return to Game Adm
			PlaySound('E', 14, 5);
		}
		break;

	case 20:
		//Map Restriction
		iNext += 2; //Turn on/off Damage
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_CLIENTMSG, 0, 2, 1, 0, 0);
			PlaySound('E', 14, 5);
		}
		
		iNext += 1; //Teleports
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_CLIENTMSG, 0, 2, 2, 0, 0);
			PlaySound('E', 14, 5);
		}

		iNext += 1; //Party
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_CLIENTMSG, 0, 2, 3, 0, 0);
			PlaySound('E', 14, 5);
		}

		iNext += 1; //Illusion
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			//bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_CLIENTMSG, 0, 2, 4, 0, 0); // centu - bug by magn0s
			PlaySound('E', 14, 5);
		}

		iNext += 1; //Actives
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_CLIENTMSG, 0, 2, 5, 0, 0);
			PlaySound('E', 14, 5);
		}

		iNext += 1; //Invisibility
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_CLIENTMSG, 0, 2, 6, 0, 0);
			PlaySound('E', 14, 5);
		}

		iNext += 1; //AMP
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_CLIENTMSG, 0, 2, 7, 0, 0);
			PlaySound('E', 14, 5);
		}

		iNext += 1; //Regens
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_CLIENTMSG, 0, 2, 8, 0, 0);
			PlaySound('E', 14, 5);
		}

		iNext += 1; //Hide Enemy
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_CLIENTMSG, 0, 2, 9, 0, 0);
			PlaySound('E', 14, 5);
		}

		iNext += 1; //Bonus Dmg
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_CLIENTMSG, 0, 2, 10, 0, 0);
			PlaySound('E', 14, 5);
		}

		iNext += 1; //Equip
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_CLIENTMSG, 0, 2, 11, 0, 0);
			PlaySound('E', 14, 5);
		}

		/*iNext += 1; //AMP
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_CLIENTMSG, 0, 2, 12, 0, 0);
			PlaySound('E', 14, 5);
		}*/

		//Back
		if ((msX > sX + 220) && (msX < sX + 240) && (msY > sY + 315) && (msY < sY + 330)) {
			m_stDialogBoxInfo[56].cMode = 0; // Return to Game Adm
			PlaySound('E', 14, 5);
		}
		break;

	case 21: // GM Commands
		iNext += 2;//create item
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			ZeroMemory(m_cChatMsg, sizeof(m_cChatMsg));
			strcpy(m_cChatMsg, "/enableadmincommand 2257516364627831 ");
			//StartInputString(10, FixChat, sizeof(m_cChatMsg), m_cChatMsg);
			bSendCommand(MSGID_COMMAND_CHATMSG, 0, 0, 0, 0, 0, m_cChatMsg); // centu
			PlaySound('E', 14, 5);
		}
		iNext += 1;//create item
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			ZeroMemory(m_cChatMsg, sizeof(m_cChatMsg));
			strcpy(m_cChatMsg, "/createitem ");
			StartInputString(10, FixChat, sizeof(m_cChatMsg), m_cChatMsg);
			PlaySound('E', 14, 5);
		}
		iNext += 1;//create item
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			ZeroMemory(m_cChatMsg, sizeof(m_cChatMsg));
			strcpy(m_cChatMsg, "/fragile itemname type day month year ");
			StartInputString(10, FixChat, sizeof(m_cChatMsg), m_cChatMsg);
			PlaySound('E', 14, 5);
		}
		iNext += 1; //Teleport
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			m_stDialogBoxInfo[56].cMode = 25; // Return to Game Adm
			PlaySound('E', 14, 5);
		}
		iNext += 1; //Add Game Master
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			if (m_iAdminUserLevel > 3) {
				ZeroMemory(m_cChatMsg, sizeof(m_cChatMsg));
				strcpy(m_cChatMsg, "/addgm [Nick]");
				StartInputString(10, FixChat, sizeof(m_cChatMsg), m_cChatMsg);
				PlaySound('E', 14, 5);
			} else { AddEventList("Admin User Level is too low for this action.", 10); }
		}
		iNext += 1; // Upgrade Admin Level
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			if (m_iAdminUserLevel > 3) {
				ZeroMemory(m_cChatMsg, sizeof(m_cChatMsg));
				strcpy(m_cChatMsg, "/gmlevel [Nick] [Level]"); 
				StartInputString(10, FixChat, sizeof(m_cChatMsg), m_cChatMsg);
				PlaySound('E', 14, 5);
			} else { AddEventList("Admin User Level is too low for this action.", 10); }
		}

		iNext += 1; //Admin goto
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			//bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_CLIENTMSG, 0, 31, 0, 0, 0);
			m_stDialogBoxInfo[56].cMode = 42; // Admin Goto
			PlaySound('E', 14, 5);
		}
		iNext += 1; //Set inv
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			//bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_CLIENTMSG, 0, 32, 0, 0, 0);
			ZeroMemory(m_cChatMsg, sizeof(m_cChatMsg));
			if ((_tmp_iStatus & 0x10) != 0) {
				strcpy(m_cChatMsg, "/setinvi 0");
				bSendCommand(MSGID_COMMAND_CHATMSG, 0, 0, 0, 0, 0, m_cChatMsg);
				PlaySound('E', 14, 5);
			}
			else {
				strcpy(m_cChatMsg, "/setinvi 1");
				bSendCommand(MSGID_COMMAND_CHATMSG, 0, 0, 0, 0, 0, m_cChatMsg);
				PlaySound('E', 14, 5);
			}
		}

		iNext += 1; //Summon
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			m_stDialogBoxInfo[56].cMode = 26; // Return to Game Adm
			PlaySound('E', 14, 5);
		}
		iNext += 1; //Clear Map
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			ZeroMemory(cTemp, sizeof(cTemp));
			wsprintf(cTemp, "/clearmap %s", m_cCurLocation);
			bSendCommand(MSGID_COMMAND_CHATMSG, 0, 0, 0, 0, 0, cTemp);
			PlaySound('E', 14, 5);
		}
		iNext += 1; //Clear NPC
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			ZeroMemory(m_cChatMsg, sizeof(m_cChatMsg));
			strcpy(m_cChatMsg, "/clearnpc");
			bSendCommand(MSGID_COMMAND_CHATMSG, 0, 0, 0, 0, 0, m_cChatMsg);
			//StartInputString(10, FixChat, sizeof(m_cChatMsg), m_cChatMsg);
			PlaySound('E', 14, 5);
		}
		iNext += 1; //Disconnect all
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			if (m_iAdminUserLevel > 3) {
					bSendCommand(MSGID_COMMAND_CHATMSG, 0, 0, 0, 0, 0, "/disconnectall");
					PlaySound('E', 14, 5);
				/*bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_CLIENTMSG, 0, 34, 0, 0, 0);
				PlaySound('E', 14, 5);*/
			} else { AddEventList("Admin User Level is too low for this action.", 10); }
		}
		iNext += 1; //Shut down server
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			if (m_iAdminUserLevel > 3) {
				bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_CLIENTMSG, 0, 35, 0, 0, 0);
				PlaySound('E', 14, 5);
			} else { AddEventList("Admin User Level is too low for this action.", 10); }
		}
		iNext += 1; //Destroy Itens
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_CLIENTMSG, 0, 36, 0, 0, 0);
			PlaySound('E', 14, 5);
		}

		/*if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_CLIENTMSG, 0, 2, 0, 0, 0);
			PlaySound('E', 14, 5);
		}*/

		//Back
		if ((msX > sX + 220) && (msX < sX + 240) && (msY > sY + 315) && (msY < sY + 330)) {
			m_stDialogBoxInfo[56].cMode = 0; // Return to Game Adm
			PlaySound('E', 14, 5);
		}
		break;

	case 22: // Event Manage
		if (m_iAdminUserLevel > 2) {
			iNext += 2; // Open / Close Arena
			if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
				bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_CLIENTMSG, 0, 50, 0, 0, 0);
				PlaySound('E', 14, 5);
			}
			iNext += 1; // Crusade
			if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
				bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_CLIENTMSG, 0, 51, 0, 0, 0);
				PlaySound('E', 14, 5);
			}
			iNext += 1; // Apocalypse
			if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
				bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_CLIENTMSG, 0, 52, 0, 0, 0);
				PlaySound('E', 14, 5);
			}
			iNext += 1; // heldenian
			if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
				bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_CLIENTMSG, 0, 53, 0, 0, 0);			
				PlaySound('E', 14, 5);
			}
			iNext += 1; // CTF
			if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
				//bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_CLIENTMSG, 0, 54, 0, 0, 0);
				bSendCommand(MSGID_COMMAND_CHATMSG, 0, 0, 0, 0, 0, "/ctf");
				PlaySound('E', 14, 5);
			}

			// kazin
			iNext += 1;
			if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {

				bSendCommand(MSGID_COMMAND_CHATMSG, 0, 0, 0, 0, 0, "/candyboost");
				PlaySound('E', 14, 5);
			}
			iNext += 1;
			if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
				bSendCommand(MSGID_COMMAND_CHATMSG, 0, 0, 0, 0, 0, "/revelation");
				PlaySound('E', 14, 5);
			}
			iNext += 1;
			if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
				bSendCommand(MSGID_COMMAND_CHATMSG, 0, 0, 0, 0, 0, "/cityteleport");
				PlaySound('E', 14, 5);
			}
			iNext += 1;
			if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
				bSendCommand(MSGID_COMMAND_CHATMSG, 0, 0, 0, 0, 0, "/dropinhib");
				PlaySound('E', 14, 5);
			}

			iNext += 1;
			if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
				bSendCommand(MSGID_COMMAND_CHATMSG, 0, 0, 0, 0, 0, "/team");
				PlaySound('E', 14, 5);
			}

			iNext += 1;
			if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
				bSendCommand(MSGID_COMMAND_CHATMSG, 0, 0, 0, 0, 0, "/shinning");
				PlaySound('E', 14, 5);
			}

			iNext += 1;
			if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
				bSendCommand(MSGID_COMMAND_CHATMSG, 0, 0, 0, 0, 0, "/happy");
				PlaySound('E', 14, 5);
			}

			iNext += 1;
			if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
				bSendCommand(MSGID_COMMAND_CHATMSG, 0, 0, 0, 0, 0, "/fury");
				PlaySound('E', 14, 5);
			}
		} else { AddEventList("Admin User Level is too low for this action.", 10); }

		//Back
		if ((msX > sX + 220) && (msX < sX + 240) && (msY > sY + 315) && (msY < sY + 330)) {
			m_stDialogBoxInfo[56].cMode = 0; // Return to Game Adm
			PlaySound('E', 14, 5);
		}
		break;

	case 23: // Player Manipulation
		iNext += 2; //Revive
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			if (m_iAdminUserLevel > 1) {
				if ((bListSelected) && strlen(cCharSelection) > 2) {
					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "/revive %s 3000 3000", cCharSelection);
					bSendCommand(MSGID_COMMAND_CHATMSG, 0, 0, 0, 0, 0, cTemp);
					PlaySound('E', 14, 5);
				}
				else {
					m_bIsGetPointingMode = true;
					m_iPointCommandType = 260;
					PlaySound('E', 14, 5);
					AddEventList("Click the character which you want to revive.", 10); //pointing mode*/
				}
			} else { AddEventList("Admin User Level is too low for this action.", 10); }
		}
		iNext += 1; //Kill
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			if (m_iAdminUserLevel > 1) {
				if ((bListSelected) && strlen(cCharSelection) > 2) {
					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "/kill %s", cCharSelection);
					bSendCommand(MSGID_COMMAND_CHATMSG, 0, 0, 0, 0, 0, cTemp);
					PlaySound('E', 14, 5);
				}
				else {
					m_bIsGetPointingMode = true;
					m_iPointCommandType = 261;
					PlaySound('E', 14, 5);
					AddEventList("Click the character which you want to kill.", 10); //pointing mode
				}
			}	else { AddEventList("Admin User Level is too low for this action.", 10); }
		}
		iNext += 1; // Set Ek, MP, REP, Contrib, Coins
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			if (m_iAdminUserLevel > 2) {
				m_stDialogBoxInfo[56].cMode = 19;
				PlaySound('E', 14, 5);
			} else { AddEventList("Admin User Level is too low for this action.", 10); }
		}

		iNext += 1; //Shutup
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			if (m_iAdminUserLevel > 1) {
				if ((bListSelected) && strlen(cCharSelection) > 2) {
					ZeroMemory(cTemp, sizeof(cTemp));
					//wsprintf(cTemp, "/shutup %s %d", cCharSelection, m_stDialogBoxInfo[56].sV3);
					//bSendCommand(MSGID_COMMAND_CHATMSG, 0, 0, 0, 0, 0, cTemp);
					wsprintf(cTemp, "/shutup %s 9999", cCharSelection);
					StartInputString(10, FixChat, sizeof(cTemp), cTemp);
					PlaySound('E', 14, 5);
				}
				else {
					m_bIsGetPointingMode = true;
					m_iPointCommandType = 262;
					PlaySound('E', 14, 5);
					AddEventList("Click the character which you want to shutup.", 10); //pointing mode
				}
			} else { AddEventList("Admin User Level is too low for this action.", 10); }
		}
		iNext += 1; //Summon player
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			if (m_iAdminUserLevel > 1) {
				if ((bListSelected) && strlen(cCharSelection) > 2) {
					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "/summonplayer %s", cCharSelection);
					bSendCommand(MSGID_COMMAND_CHATMSG, 0, 0, 0, 0, 0, cTemp);
					PlaySound('E', 14, 5);
				}
				else {
					ZeroMemory(m_cChatMsg, sizeof(m_cChatMsg));
					strcpy(m_cChatMsg, "/summonplayer ");
					StartInputString(10, FixChat, sizeof(m_cChatMsg), m_cChatMsg);
					PlaySound('E', 14, 5);
				}
			} else { AddEventList("Admin User Level is too low for this action.", 10); }
		}
		iNext += 1; //Send player
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			ZeroMemory(m_cChatMsg, sizeof(m_cChatMsg));
			strcpy(m_cChatMsg, "/sendplayer ");
			StartInputString(10, FixChat, sizeof(m_cChatMsg), m_cChatMsg);
			PlaySound('E', 14, 5);
		}
		iNext += 1; // Close Connection
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			if (m_iAdminUserLevel > 1) {
				if ((bListSelected) && strlen(cCharSelection) > 2) {
					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "/closeconn %s", cCharSelection);
					bSendCommand(MSGID_COMMAND_CHATMSG, 0, 0, 0, 0, 0, cTemp);
					PlaySound('E', 14, 5);
				}
				else {
					m_bIsGetPointingMode = true;
					m_iPointCommandType = 263;
					PlaySound('E', 14, 5);
					AddEventList("Click the character which you want to disconnect.", 10); //pointing mode
				}
			}	else { AddEventList("Admin User Level is too low for this action.", 10); }
		}
		iNext += 1; // Palyer Info
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			m_bIsGetPointingMode = true;
			m_iPointCommandType = 264;
			PlaySound('E', 14, 5);
			AddEventList("Click the character which you want to have full information.", 10); //pointing mode
		}
		iNext += 1; // Apply Player Penalty
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			if (m_iAdminUserLevel > 2) {
				if ((bListSelected) && strlen(cCharSelection) > 2) {
					m_stDialogBoxInfo[56].cMode = 10; // Lock Player BI
					PlaySound('E', 14, 5);
				}
				else {
					m_bIsGetPointingMode = true;
					m_iPointCommandType = 265;
					PlaySound('E', 14, 5);
					AddEventList("Click the character which you want to give a penalty", 10); //pointing mode
				}
			} else { AddEventList("Admin User Level is too low for this action.", 10); }
		}

		//Back
		if ((msX > sX + 220) && (msX < sX + 240) && (msY > sY + 315) && (msY < sY + 330)) {
			m_stDialogBoxInfo[56].cMode = 0; // Return to Game Adm
			PlaySound('E', 14, 5);
		}
		break;

	case 24:
		iNext += 2; // Primary Drop
		if ((msX >= sX + 180) && (msX <= sX + 195) && (msY >= sY + iNext * 17 + 20) && (msY <= sY + iNext * 17 + 34)) {
			if (m_iAdminUserLevel > 3) {
				bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_CLIENTMSG, 0, 24, 1, 0, 0);
				PlaySound('E', 14, 5);
			}	else { AddEventList("Admin User Level is too low for this action.", 10); }
		}

		if ((msX >= sX + 230) && (msX <= sX + 245) && (msY >= sY + iNext * 17 + 20) && (msY <= sY + iNext * 17 + 34)) {
			if (m_iAdminUserLevel > 3) {
				bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_CLIENTMSG, 0, 24, 1, 1, 0);
				PlaySound('E', 14, 5);
			}
			else { AddEventList("Admin User Level is too low for this action.", 10); }
		}

		iNext += 3; // Secondary Drop
		if ((msX >= sX + 180) && (msX <= sX + 195) && (msY >= sY + iNext * 17 + 20) && (msY <= sY + iNext * 17 + 34)) {
			if (m_iAdminUserLevel > 3) {
				bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_CLIENTMSG, 0, 24, 2, 0, 0);
				PlaySound('E', 14, 5);
			}
			else { AddEventList("Admin User Level is too low for this action.", 10); }
		}

		if ((msX >= sX + 230) && (msX <= sX + 245) && (msY >= sY + iNext * 17 + 20) && (msY <= sY + iNext * 17 + 34)) {
			if (m_iAdminUserLevel > 3) {
				bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_CLIENTMSG, 0, 24, 2, 1, 0);
				PlaySound('E', 14, 5);
			}
			else { AddEventList("Admin User Level is too low for this action.", 10); }
		}

		iNext += 3; // Stated Drop
		if ((msX >= sX + 180) && (msX <= sX + 195) && (msY >= sY + iNext * 17 + 20) && (msY <= sY + iNext * 17 + 34)) {
			if (m_iAdminUserLevel > 3) {
				bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_CLIENTMSG, 0, 24, 3, 0, 0);
				PlaySound('E', 14, 5);
			}
			else { AddEventList("Admin User Level is too low for this action.", 10); }
		}

		if ((msX >= sX + 230) && (msX <= sX + 245) && (msY >= sY + iNext * 17 + 20) && (msY <= sY + iNext * 17 + 34)) {
			if (m_iAdminUserLevel > 3) {
				bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_CLIENTMSG, 0, 24, 3, 1, 0);
				PlaySound('E', 14, 5);
			}
			else { AddEventList("Admin User Level is too low for this action.", 10); }
		}
		//-------------------------------
		iNext += 3;
		//Magic abs drops
		if ((msX >= sX + 180) && (msX <= sX + 195) && (msY >= sY + iNext * 17 + 20) && (msY <= sY + iNext * 17 + 34)) {
			if (m_iAdminUserLevel > 3) {
				bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_CLIENTMSG, 0, 24, 4, 1, 0);
				PlaySound('E', 14, 5);
			}
			else { AddEventList("Admin User Level is too low for this action.", 10); }
		}

		if ((msX >= sX + 230) && (msX <= sX + 245) && (msY >= sY + iNext * 17 + 20) && (msY <= sY + iNext * 17 + 34)) {
			if (m_iAdminUserLevel > 3) {
				bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_CLIENTMSG, 0, 24, 4, 1, 0);
				PlaySound('E', 14, 5);
			}
			else { AddEventList("Admin User Level is too low for this action.", 10); }
		}
		
		iNext += 1;
		//Physical abs drops
		if ((msX >= sX + 180) && (msX <= sX + 195) && (msY >= sY + iNext * 17 + 20) && (msY <= sY + iNext * 17 + 34)) {
			if (m_iAdminUserLevel > 3) {
				bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_CLIENTMSG, 0, 24, 4, 2, 0);
				PlaySound('E', 14, 5);
			}
			else { AddEventList("Admin User Level is too low for this action.", 10); }
		}

		if ((msX >= sX + 230) && (msX <= sX + 245) && (msY >= sY + iNext * 17 + 20) && (msY <= sY + iNext * 17 + 34)) {
			if (m_iAdminUserLevel > 3) {
				bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_CLIENTMSG, 0, 24, 4, 2, 0);
				PlaySound('E', 14, 5);
			}
			else { AddEventList("Admin User Level is too low for this action.", 10); }
		}

		iNext += 2;
		//Server Physical Damage
		if ((msX >= sX + 180) && (msX <= sX + 195) && (msY >= sY + iNext * 17 + 20) && (msY <= sY + iNext * 17 + 34)) {
			if (m_iAdminUserLevel > 3) {
				bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_CLIENTMSG, 0, 24, 5, 0, 0);
				PlaySound('E', 14, 5);
			}
			else { AddEventList("Admin User Level is too low for this action.", 10); }
		}

		if ((msX >= sX + 230) && (msX <= sX + 245) && (msY >= sY + iNext * 17 + 20) && (msY <= sY + iNext * 17 + 34)) {
			if (m_iAdminUserLevel > 3) {
				bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_CLIENTMSG, 0, 24, 5, 1, 0);
				PlaySound('E', 14, 5);
			}
			else { AddEventList("Admin User Level is too low for this action.", 10); }
		}

		iNext += 1;
		//Server Magical Damage
		if ((msX >= sX + 180) && (msX <= sX + 195) && (msY >= sY + iNext * 17 + 20) && (msY <= sY + iNext * 17 + 34)) {
			if (m_iAdminUserLevel > 3) {
				bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_CLIENTMSG, 0, 24, 6, 0, 0);
				PlaySound('E', 14, 5);
			}
			else { AddEventList("Admin User Level is too low for this action.", 10); }
		}

		if ((msX >= sX + 230) && (msX <= sX + 245) && (msY >= sY + iNext * 17 + 20) && (msY <= sY + iNext * 17 + 34)) {
			if (m_iAdminUserLevel > 3) {
				bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_CLIENTMSG, 0, 24, 6, 1, 0);
				PlaySound('E', 14, 5);
			}
			else { AddEventList("Admin User Level is too low for this action.", 10); }
		}

		break;

	case 25: //cMapTP
		bFlag = true;
		iNext += 2;
		if ((msX >= sX + 20) && (msX <= sX + 80) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			strcpy(cMapTP, "aresden");
		}

		if ((msX >= sX + 100) && (msX <= sX + 160) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			strcpy(cMapTP, "elvine");
		}

		if ((msX >= sX + 180) && (msX <= sX + 230) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			strcpy(cMapTP, "fightzone1");
		}
		//---------------------------------------------------------------------------------------------------------
		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 80) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			strcpy(cMapTP, "gshop_1");
		}
		if ((msX >= sX + 100) && (msX <= sX + 160) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			strcpy(cMapTP, "gshop_2");
		}
		if ((msX >= sX + 180) && (msX <= sX + 230) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			strcpy(cMapTP, "fightzone2");
		}
		//---------------------------------------------------------------------------------------------------------
		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 80) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			strcpy(cMapTP, "cityhall_1");
		}

		if ((msX >= sX + 100) && (msX <= sX + 160) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			strcpy(cMapTP, "cityhall_2");
		}

		if ((msX >= sX + 180) && (msX <= sX + 230) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			strcpy(cMapTP, "fightzone3");
		}
		//---------------------------------------------------------------------------------------------------------
		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 80) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			strcpy(cMapTP, "wrhus_1");
		}

		if ((msX >= sX + 100) && (msX <= sX + 160) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			strcpy(cMapTP, "wrhus_2");
		}

		if ((msX >= sX + 180) && (msX <= sX + 230) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			strcpy(cMapTP, "fightzone4");
		}
		//---------------------------------------------------------------------------------------------------------
		//---------------------------------------------------------------------------------------------------------
		iNext += 2;
		if ((msX >= sX + 20) && (msX <= sX + 80) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			strcpy(cMapTP, "default");
		}

		if ((msX >= sX + 100) && (msX <= sX + 160) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			strcpy(cMapTP, "icebound");
		}

		if ((msX >= sX + 180) && (msX <= sX + 230) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			strcpy(cMapTP, "fightzone5");
		}
		//---------------------------------------------------------------------------------------------------------
		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 80) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			strcpy(cMapTP, "middleland");
		}

		if ((msX >= sX + 100) && (msX <= sX + 160) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			strcpy(cMapTP, "druncncity");
		}

		if ((msX >= sX + 180) && (msX <= sX + 230) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			strcpy(cMapTP, "fightzone6");
		}
		//---------------------------------------------------------------------------------------------------------
		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 80) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			strcpy(cMapTP, "2ndmiddle");
		}

		if ((msX >= sX + 100) && (msX <= sX + 160) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			strcpy(cMapTP, "inferniaA");
		}

		if ((msX >= sX + 180) && (msX <= sX + 230) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			strcpy(cMapTP, "fightzone7");
		}
		//---------------------------------------------------------------------------------------------------------
		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 80) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			strcpy(cMapTP, "BtField");
		}

		if ((msX >= sX + 100) && (msX <= sX + 160) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			strcpy(cMapTP, "inferniaB");
		}

		if ((msX >= sX + 180) && (msX <= sX + 230) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			strcpy(cMapTP, "team");
		}
		//---------------------------------------------------------------------------------------------------------
		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 80) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			strcpy(cMapTP, "GodH");
		}

		if ((msX >= sX + 100) && (msX <= sX + 160) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			strcpy(cMapTP, "maze");
		}

		if ((msX >= sX + 180) && (msX <= sX + 230) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			strcpy(cMapTP, "bisle");
		}
		//---------------------------------------------------------------------------------------------------------
		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 80) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			strcpy(cMapTP, "HRampart");
		}

		if ((msX >= sX + 100) && (msX <= sX + 160) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			strcpy(cMapTP, "procella");
		}

		if ((msX >= sX + 180) && (msX <= sX + 230) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			strcpy(cMapTP, "middled1x");
		}
		//---------------------------------------------------------------------------------------------------------
		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 80) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			strcpy(cMapTP, "arebrk11");
		}

		if ((msX >= sX + 100) && (msX <= sX + 160) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			strcpy(cMapTP, "abaddon");
		}

		if ((msX >= sX + 180) && (msX <= sX + 230) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			strcpy(cMapTP, "middled1n");
		}
		//---------------------------------------------------------------------------------------------------------

		//Magn0S:: Set command to TP
		if ((strlen(cMapTP) > 3) && (bFlag))
		bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_CLIENTMSG, 0, 28, 0, 0, cMapTP);

		//Back
		if ((msX > sX + 220) && (msX < sX + 240) && (msY > sY + 315) && (msY < sY + 330)) {
			m_stDialogBoxInfo[56].cMode = 0; // Return to Game Adm
			PlaySound('E', 14, 5);
		}
		break;

	case 26:
		int iaddx, iaddy;
		iaddx = 15;
		iaddy = 75;
		bFlag = true;
		if ((msX >= sX + 145 + iaddx) && (msX <= sX + 162 + iaddx) && (msY >= sY + 209 + iaddy) && (msY <= sY + 230 + iaddy))
		{
			m_stDialogBoxInfo[56].sV3 += 10;
			if (m_stDialogBoxInfo[56].sV3 >= 50)
				m_stDialogBoxInfo[56].sV3 = 50;
		}

		if ((msX >= sX + 145 + iaddx) && (msX <= sX + 162 + iaddx) && (msY >= sY + 234 + iaddy) && (msY <= sY + 251 + iaddy))
		{
			m_stDialogBoxInfo[56].sV3 -= 10;
			if (m_stDialogBoxInfo[56].sV3 <= 0)
				m_stDialogBoxInfo[56].sV3 = 0;
		}

		if ((msX >= sX + 163 + iaddx) && (msX <= sX + 180 + iaddx) && (msY >= sY + 209 + iaddy) && (msY <= sY + 230 + iaddy))
		{
			m_stDialogBoxInfo[56].sV3++;
			if (m_stDialogBoxInfo[56].sV3 >= 50)
				m_stDialogBoxInfo[56].sV3 = 50;
		}

		if ((msX >= sX + 163 + iaddx) && (msX <= sX + 180 + iaddx) && (msY >= sY + 234 + iaddy) && (msY <= sY + 251 + iaddy))
		{
			m_stDialogBoxInfo[56].sV3--;
			if (m_stDialogBoxInfo[56].sV3 <= 0)
				m_stDialogBoxInfo[56].sV3 = 0;
		}

		iNext += 1;
		if ((msX >= sX + 10) && (msX <= sX + 80) && (msY >= sY + iNext * 15 + 30) && (msY <= sY + iNext * 15 + 45)) {
			strcpy(cSummon, "/summon Dummy ");
			PlaySound('E', 14, 5);
		}
		iNext += 1;
		if ((msX >= sX + 10) && (msX <= sX + 80) && (msY >= sY + iNext * 15 + 30) && (msY <= sY + iNext * 15 + 45)) {
			strcpy(cSummon, "/summon Slime ");
			PlaySound('E', 14, 5);
		}
		iNext += 1;
		if ((msX >= sX + 10) && (msX <= sX + 80) && (msY >= sY + iNext * 15 + 30) && (msY <= sY + iNext * 15 + 45)) {
			strcpy(cSummon, "/summon Giant-Ant ");
			PlaySound('E', 14, 5);
		}
		iNext += 1;
		if ((msX >= sX + 10) && (msX <= sX + 80) && (msY >= sY + iNext * 15 + 30) && (msY <= sY + iNext * 15 + 45)) {
			strcpy(cSummon, "/summon Orc ");
			PlaySound('E', 14, 5);
		}
		iNext += 1;
		if ((msX >= sX + 10) && (msX <= sX + 80) && (msY >= sY + iNext * 15 + 30) && (msY <= sY + iNext * 15 + 45)) {
			strcpy(cSummon, "/summon Skeleton ");
			PlaySound('E', 14, 5);
		}
		iNext += 1;
		if ((msX >= sX + 10) && (msX <= sX + 80) && (msY >= sY + iNext * 15 + 30) && (msY <= sY + iNext * 15 + 45)) {
			strcpy(cSummon, "/summon Cyclops ");
			PlaySound('E', 14, 5);
		}
		iNext += 1;
		if ((msX >= sX + 10) && (msX <= sX + 80) && (msY >= sY + iNext * 15 + 30) && (msY <= sY + iNext * 15 + 45)) {
			strcpy(cSummon, "/summon Zombie ");
			PlaySound('E', 14, 5);
		}
		iNext += 1;
		if ((msX >= sX + 10) && (msX <= sX + 80) && (msY >= sY + iNext * 15 + 30) && (msY <= sY + iNext * 15 + 45)) {
			strcpy(cSummon, "/summon Scorpion ");
			PlaySound('E', 14, 5);
		}
		iNext += 1;
		if ((msX >= sX + 10) && (msX <= sX + 80) && (msY >= sY + iNext * 15 + 30) && (msY <= sY + iNext * 15 + 45)) {
			strcpy(cSummon, "/summon Amphis ");
			PlaySound('E', 14, 5);
		}
		iNext += 1;
		if ((msX >= sX + 10) && (msX <= sX + 80) && (msY >= sY + iNext * 15 + 30) && (msY <= sY + iNext * 15 + 45)) {
			strcpy(cSummon, "/summon Clay-Golem ");
			PlaySound('E', 14, 5);
		}
		iNext += 1;
		if ((msX >= sX + 10) && (msX <= sX + 80) && (msY >= sY + iNext * 15 + 30) && (msY <= sY + iNext * 15 + 45)) {
			strcpy(cSummon, "/summon Stone-Golem ");
			PlaySound('E', 14, 5);
		}
		iNext += 1;
		if ((msX >= sX + 10) && (msX <= sX + 80) && (msY >= sY + iNext * 15 + 30) && (msY <= sY + iNext * 15 + 45)){
			strcpy(cSummon, "/summon Hellbound ");
			PlaySound('E', 14, 5);
		}
		iNext += 1;
		if ((msX >= sX + 10) && (msX <= sX + 80) && (msY >= sY + iNext * 15 + 30) && (msY <= sY + iNext * 15 + 45)) {
			strcpy(cSummon, "/summon Troll ");
			PlaySound('E', 14, 5);
		}
		iNext += 1;
		if ((msX >= sX + 10) && (msX <= sX + 80) && (msY >= sY + iNext * 15 + 30) && (msY <= sY + iNext * 15 + 45)) {
			strcpy(cSummon, "/summon Orge ");
			PlaySound('E', 14, 5);
		}
		iNext += 1;
		if ((msX >= sX + 10) && (msX <= sX + 80) && (msY >= sY + iNext * 15 + 30) && (msY <= sY + iNext * 15 + 45)) {
			strcpy(cSummon, "/summon Giant-Plant ");
			PlaySound('E', 14, 5);
		}
		iNext += 1;
		if ((msX >= sX + 10) && (msX <= sX + 80) && (msY >= sY + iNext * 15 + 30) && (msY <= sY + iNext * 15 + 45)) {
			strcpy(cSummon, "/summon Giant-Crayfish ");
			PlaySound('E', 14, 5);
		}
		iNext += 1;
		if ((msX >= sX + 10) && (msX <= sX + 80) && (msY >= sY + iNext * 15 + 30) && (msY <= sY + iNext * 15 + 45)) {
			strcpy(cSummon, "/summon Tentocle ");
			PlaySound('E', 14, 5);
		}
		//-------------------------------------------------------------------------------------------------------------------------

		iNextB += 1;
		if ((msX >= sX + 100) && (msX <= sX + 160) && (msY >= sY + iNextB * 15 + 30) && (msY <= sY + iNextB * 15 + 45)) {
			strcpy(cSummon, "/summon Liche ");
			PlaySound('E', 14, 5);
		}
		iNextB += 1;
		if ((msX >= sX + 100) && (msX <= sX + 160) && (msY >= sY + iNextB * 15 + 30) && (msY <= sY + iNextB * 15 + 45)) {
			strcpy(cSummon, "/summon WereWolf ");
			PlaySound('E', 14, 5);
		}
		iNextB += 1;
		if ((msX >= sX + 100) && (msX <= sX + 160) && (msY >= sY + iNextB * 15 + 30) && (msY <= sY + iNextB * 15 + 45)) {
			strcpy(cSummon, "/summon Stalker ");
			PlaySound('E', 14, 5);
		}
		iNextB += 1;
		if ((msX >= sX + 100) && (msX <= sX + 160) && (msY >= sY + iNextB * 15 + 30) && (msY <= sY + iNextB * 15 + 45)) {
			strcpy(cSummon, "/summon Dark-Elf ");
			PlaySound('E', 14, 5);
		}
		iNextB += 1;
		if ((msX >= sX + 100) && (msX <= sX + 160) && (msY >= sY + iNextB * 15 + 30) && (msY <= sY + iNextB * 15 + 45)) {
			strcpy(cSummon, "/summon Giant-Frog ");
			PlaySound('E', 14, 5);
		}
		iNextB += 1;
		if ((msX >= sX + 100) && (msX <= sX + 160) && (msY >= sY + iNextB * 15 + 30) && (msY <= sY + iNextB * 15 + 45)) {
			strcpy(cSummon, "/summon Mountain-Giant ");
			PlaySound('E', 14, 5);
		}
		iNextB += 1;
		if ((msX >= sX + 100) && (msX <= sX + 160) && (msY >= sY + iNextB * 15 + 30) && (msY <= sY + iNextB * 15 + 45)) {
			strcpy(cSummon, "/summon Ettin ");
			PlaySound('E', 14, 5);
		}
		iNextB += 1;
		if ((msX >= sX + 100) && (msX <= sX + 160) && (msY >= sY + iNextB * 15 + 30) && (msY <= sY + iNextB * 15 + 45)) {
			strcpy(cSummon, "/summon Cannibal-Plant ");
			PlaySound('E', 14, 5);
		}
		iNextB += 1;
		if ((msX >= sX + 100) && (msX <= sX + 160) && (msY >= sY + iNextB * 15 + 30) && (msY <= sY + iNextB * 15 + 45)) {
			strcpy(cSummon, "/summon Rudolph ");
			PlaySound('E', 14, 5);
		}
		iNextB += 1;
		if ((msX >= sX + 100) && (msX <= sX + 160) && (msY >= sY + iNextB * 15 + 30) && (msY <= sY + iNextB * 15 + 45)) {
			strcpy(cSummon, "/summon Beholder ");
			PlaySound('E', 14, 5);
		}
		iNextB += 1;
		if ((msX >= sX + 100) && (msX <= sX + 160) && (msY >= sY + iNextB * 15 + 30) && (msY <= sY + iNextB * 15 + 45)) {
			strcpy(cSummon, "/summon DireBoar ");
			PlaySound('E', 14, 5);
		}
		iNextB += 1;
		if ((msX >= sX + 100) && (msX <= sX + 160) && (msY >= sY + iNextB * 15 + 30) && (msY <= sY + iNextB * 15 + 45)) {
			strcpy(cSummon, "/summon Ice-Golem ");
			PlaySound('E', 14, 5);
		}
		iNextB += 1;
		if ((msX >= sX + 100) && (msX <= sX + 160) && (msY >= sY + iNextB * 15 + 30) && (msY <= sY + iNextB * 15 + 45)) {
			strcpy(cSummon, "/summon Frost ");
			PlaySound('E', 14, 5);
		}
		iNextB += 1;
		if ((msX >= sX + 100) && (msX <= sX + 160) && (msY >= sY + iNextB * 15 + 30) && (msY <= sY + iNextB * 15 + 45)) {
			strcpy(cSummon, "/summon Nizie ");
			PlaySound('E', 14, 5);
		}
		iNextB += 1;
		if ((msX >= sX + 100) && (msX <= sX + 160) && (msY >= sY + iNextB * 15 + 30) && (msY <= sY + iNextB * 15 + 45)) {
			strcpy(cSummon, "/summon Claw-Turtle ");
			PlaySound('E', 14, 5);
		}
		iNextB += 1;
		if ((msX >= sX + 100) && (msX <= sX + 160) && (msY >= sY + iNextB * 15 + 30) && (msY <= sY + iNextB * 15 + 45)) {
			strcpy(cSummon, "/summon Giant-Lizard ");
			PlaySound('E', 14, 5);
		}
		//------------------------------------------------------------------------------------------------------------------------

		iNextC += 1;
		if ((msX >= sX + 180) && (msX <= sX + 230) && (msY >= sY + iNextC * 15 + 30) && (msY <= sY + iNextC * 15 + 45)) {
			strcpy(cSummon, "/summon MasterMage-Orc ");
			PlaySound('E', 14, 5);
		}
		iNextC += 1;
		if ((msX >= sX + 180) && (msX <= sX + 230) && (msY >= sY + iNextC * 15 + 30) && (msY <= sY + iNextC * 15 + 45)) {
			strcpy(cSummon, "/summon Minotaurs ");
			PlaySound('E', 14, 5);
		}
		iNextC += 1;
		if ((msX >= sX + 180) && (msX <= sX + 230) && (msY >= sY + iNextC * 15 + 30) && (msY <= sY + iNextC * 15 + 45)) {
			strcpy(cSummon, "/summon Barlog ");
			PlaySound('E', 14, 5);
		}
		iNextC += 1;
		if ((msX >= sX + 180) && (msX <= sX + 230) && (msY >= sY + iNextC * 15 + 30) && (msY <= sY + iNextC * 15 + 45)) {
			strcpy(cSummon, "/summon Centaurus ");
			PlaySound('E', 14, 5);
		}
		iNextC += 1;
		if ((msX >= sX + 180) && (msX <= sX + 230) && (msY >= sY + iNextC * 15 + 30) && (msY <= sY + iNextC * 15 + 45)) {
			strcpy(cSummon, "/summon Demon ");
			PlaySound('E', 14, 5);
		}
		iNextC += 1;
		if ((msX >= sX + 180) && (msX <= sX + 230) && (msY >= sY + iNextC * 15 + 30) && (msY <= sY + iNextC * 15 + 45)) {
			strcpy(cSummon, "/summon Unicorn ");
			PlaySound('E', 14, 5);
		}
		iNextC += 1;
		if ((msX >= sX + 180) && (msX <= sX + 230) && (msY >= sY + iNextC * 15 + 30) && (msY <= sY + iNextC * 15 + 45)) {
			strcpy(cSummon, "/summon Gagoyle ");
			PlaySound('E', 14, 5);
		}
		iNextC += 1;
		if ((msX >= sX + 180) && (msX <= sX + 230) && (msY >= sY + iNextC * 15 + 30) && (msY <= sY + iNextC * 15 + 45)) {
			strcpy(cSummon, "/summon Hellclaw ");
			PlaySound('E', 14, 5);
		}
		iNextC += 1;
		if ((msX >= sX + 180) && (msX <= sX + 230) && (msY >= sY + iNextC * 15 + 30) && (msY <= sY + iNextC * 15 + 45)) {
			strcpy(cSummon, "/summon Tigerworm ");
			PlaySound('E', 14, 5);
		}
		iNextC += 1;
		if ((msX >= sX + 180) && (msX <= sX + 230) && (msY >= sY + iNextC * 15 + 30) && (msY <= sY + iNextC * 15 + 45)) {
			strcpy(cSummon, "/summon Wyvern ");
			PlaySound('E', 14, 5);
		}
		iNextC += 1;
		if ((msX >= sX + 180) && (msX <= sX + 230) && (msY >= sY + iNextC * 15 + 30) && (msY <= sY + iNextC * 15 + 45)) {
			strcpy(cSummon, "/summon Fire-Wyvern ");
			PlaySound('E', 14, 5);
		}
		iNextC += 1;
		if ((msX >= sX + 180) && (msX <= sX + 230) && (msY >= sY + iNextC * 15 + 30) && (msY <= sY + iNextC * 15 + 45)) {
			strcpy(cSummon, "/summon Abaddon ");
			PlaySound('E', 14, 5);
		}

		wsprintf(cMsg, "%s %d", cSummon, m_stDialogBoxInfo[56].sV3);
		strcpy(cSummon, cMsg);

		if ((strlen(cSummon) > 10) && (bFlag) && (m_stDialogBoxInfo[56].sV3 > 0)) {
			bSendCommand(MSGID_COMMAND_CHATMSG, 0, 0, 0, 0, 0, cSummon);
			m_stDialogBoxInfo[56].sV3 = 0;
		}

		//Back
		if ((msX > sX + 220) && (msX < sX + 240) && (msY > sY + 315) && (msY < sY + 330)) {
			m_stDialogBoxInfo[56].cMode = 21; // Return to Game Adm
			PlaySound('E', 14, 5);
		}
		break;

		case 40:
			for (i = 0; i < 17; i++)
			{
				if (((i + m_stDialogBoxInfo[56].sView) < DEF_MAXCLIENTS) && (m_pOnlineUsersList[i + m_stDialogBoxInfo[56].sView] != 0))
				{
					if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + i * 18 + 65) && (msY <= sY + i * 18 + 79))
					{
						ZeroMemory(cTemp, sizeof(cTemp));
						ZeroMemory(cCharSelection, sizeof(cCharSelection));
						strcpy(cCharSelection, m_pOnlineUsersList[i + m_stDialogBoxInfo[56].sView]->m_cName);
						m_stDialogBoxInfo[56].cMode = 23; // Manipulate Char
						bListSelected = true;
						PlaySound('E', 14, 5);
						break;
						//wsprintf(cTemp, "/to %s", m_pOnlineUsersList[i + m_stDialogBoxInfo[60].sView]->m_cName);
						//bSendCommand(MSGID_COMMAND_CHATMSG, 0, 0, 0, 0, 0, cTemp);
					}
				}
			}

			if ((msX > sX + 180) && (msX < sX + 180 + 40) && (msY > sY + 45 + 14 * 18) && (msY < sY + 45 + 15 * 18))
			{
				if ((m_dwCurTime - m_dwReqUsersTime) > 30000)
				{
					for (i = 0; i < DEF_MAXCLIENTS; i++)
						if (m_pOnlineUsersList[i] != 0) m_pOnlineUsersList[i] = 0;

					for (i = 0; i < DEF_MAXCLIENTS; i++)
						if (strlen(m_stOnlineGuild[i].cCharName) != 0) ZeroMemory(m_stOnlineGuild[i].cCharName, sizeof(m_stOnlineGuild[i].cCharName));

					bSendCommand(MSGID_REQUEST_ONLINE, 0, 0, 0, 0, 0, 0);

					m_dwReqUsersTime = m_dwCurTime;
					m_cCommandCount--;
				}
			}
			break;

		case 42: // centu
			for (i = 0; i < 17; i++)
			{
				if (((i + m_stDialogBoxInfo[56].sView) < DEF_MAXCLIENTS) && (m_pOnlineUsersList[i + m_stDialogBoxInfo[56].sView] != 0))
				{
					if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + i * 18 + 65) && (msY <= sY + i * 18 + 79))
					{
						ZeroMemory(cTemp, sizeof(cTemp));
						//ZeroMemory(cCharSelection, sizeof(cCharSelection));
						//strcpy(cCharSelection, m_pOnlineUsersList[i + m_stDialogBoxInfo[56].sView]->m_cName);
						//m_stDialogBoxInfo[56].cMode = 23; // Manipulate Char
						//bListSelected = true;
						wsprintf(cTemp, "/goto %s", m_pOnlineUsersList[i + m_stDialogBoxInfo[56].sView]->m_cName);
						bSendCommand(MSGID_COMMAND_CHATMSG, 0, 0, 0, 0, 0, cTemp);
						PlaySound('E', 14, 5);
						break;
					}
				}
			}

			if ((msX > sX + 180) && (msX < sX + 180 + 40) && (msY > sY + 45 + 14 * 18) && (msY < sY + 45 + 15 * 18))
			{
				if ((m_dwCurTime - m_dwReqUsersTime) > 30000)
				{
					for (i = 0; i < DEF_MAXCLIENTS; i++)
						if (m_pOnlineUsersList[i] != 0) m_pOnlineUsersList[i] = 0;

					for (i = 0; i < DEF_MAXCLIENTS; i++)
						if (strlen(m_stOnlineGuild[i].cCharName) != 0) ZeroMemory(m_stOnlineGuild[i].cCharName, sizeof(m_stOnlineGuild[i].cCharName));

					bSendCommand(MSGID_REQUEST_ONLINE, 0, 0, 0, 0, 0, 0);

					m_dwReqUsersTime = m_dwCurTime;
					m_cCommandCount--;
				}
			}
			break;

		/*case 5: // Guild Menu
			if ((msX > sX + 180) && (msX < sX + 380) && (msY > sY + 85) && (msY < sY + 100)) {
				m_stDialogBoxInfo[56].cMode = 50; // Members online
			//	bSendCommand(DEF_REQUEST_GUILDMEMBERON, 0, 0, 0, 0, 0, 0);
				bSendCommand(MSGID_COMMAND_COMMON, DEF_REQUEST_GUILDMEMBERON, 0, 0, 0, 0, 0);
				PlaySound('E', 14, 5);
			}
			if ((msX > sX + 180) && (msX < sX + 280) && (msY > sY + 100) && (msY < sY + 115)) {
				// teleport members
				bSendCommand(MSGID_REQUEST_TELEPORTMSG, 0, 0, 10, 0, 0, 0);
				PlaySound('E', 14, 5);
			}
	break;*/
	}
}

//New GM Panel by Magn0S ;D
void CGame::DrawDialogBox_GMPanel(short msX, short msY, short msZ, char cLB)
{
	short sX, sY, szX;
	char cTxt[120], cTemp[255], cDrops[120];
	int y, iaddx, iaddy;
	int nickheight = 16;
	short toX, toY, limitX, limitY;
	UINT32 dwTime = m_dwCurTime;
	int  i, iTemp;
	char cTemp2[255], cStr2[255], cStr3[255];

	float dTmp1, dTmp2, dTmp3;
	int  iTotalLines, iPointerLoc;
	bool bFlagStatLow = false;
	bool bFlagRedShown = false;
	float d1, d2, d3;
	int iR, iG, iB;

	char cTmpCName[11];
	char cTmpGName[21];

	for (int j = 0; j < DEF_MAXCLIENTS; j++)
	{
		for (int k = j; k < DEF_MAXCLIENTS - 1; k++)
		{
			if (m_pOnlineUsersList[j] != 0 && m_pOnlineUsersList[k] != 0)
			{
				ZeroMemory(cTmpCName, sizeof(cTmpCName));
				ZeroMemory(cTmpGName, sizeof(cTmpGName));

				if (m_pOnlineUsersList[j]->m_cName[0] > m_pOnlineUsersList[k]->m_cName[0])
				{
					memcpy(cTmpCName, m_pOnlineUsersList[j]->m_cName, 10);
					memcpy(cTmpGName, m_pOnlineUsersList[j]->m_cGuildName, 20);

					memcpy(m_pOnlineUsersList[j]->m_cName, m_pOnlineUsersList[k]->m_cName, 10);
					memcpy(m_pOnlineUsersList[j]->m_cGuildName, m_pOnlineUsersList[k]->m_cGuildName, 20);

					memcpy(m_pOnlineUsersList[k]->m_cName, cTmpCName, 10);
					memcpy(m_pOnlineUsersList[k]->m_cGuildName, cTmpGName, 20);
				}
			}
		}
	}

	sX = m_stDialogBoxInfo[56].sX;
	sY = m_stDialogBoxInfo[56].sY;
	szX = m_stDialogBoxInfo[56].sSizeX;

	//if ((strcmp(m_cPlayerName, "Magn0S[GM]") != 0) || (strcmp(m_cPlayerName, "Centuu[GM]") != 0) || (strcmp(m_cPlayerName, "Nixu[GM]") != 0)) return;

	DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME6, sX, sY, 0, m_bDialogTrans);

	toX = sX;
	toY = sY;
	limitX = sX + 258;
	limitY = sY + 339;
	int iNext = 0;
	int iNextB = 0;
	int iNextC = 0;
	int iEntry = 0;
	
	//m_DDraw.DrawShadowBox(toX, toY, limitX, limitY, 0, true);
	//m_DDraw.DrawShadowBox(toX, toY, limitX, limitY, 0, true);

	switch (m_stDialogBoxInfo[56].cMode) {
	case 0:
		//PutString_SprFont3(sX + 50, sY + 10, "Administration Panel", 2, 150, 0);
		PutString_SprFont3(sX + 125 - ((strlen("Administration Panel") * 7) / 2), sY + 10, "Administration Panel", 2, 150, 0);
		wsprintf(G_cTxt, "Hi %s", m_cPlayerName);
		PutAlignedString(sX + 30, sX + szX - 30, sY + 30, G_cTxt, 255, 255, 255);
		PutAlignedString(sX + 32, sX + szX - 30, sY + 30, G_cTxt, 255, 255, 255);

		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59))
		{
			PutString2(sX + 25, sY + iNext * 17 + 45, "Administrative Commands", 255, 255, 255);
		}
		else PutString2(sX + 25, sY + iNext * 17 + 45, "Administrative Commands", 255, 255, 100);

		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59))
		{
			PutString2(sX + 25, sY + iNext * 17 + 45, "GameServer Administration", 255, 255, 255);
		}
		else PutString2(sX + 25, sY + iNext * 17 + 45, "GameServer Administration", 255, 255, 100);

		iNext += 1;
		if (m_iAdminUserLevel > 2) {
			if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59))
			{
				PutString2(sX + 25, sY + iNext * 17 + 45, "Event Manager", 255, 255, 255);
			}
			else PutString2(sX + 25, sY + iNext * 17 + 45, "Event Manager", 255, 255, 100);
		}
		else { PutString2(sX + 25, sY + iNext * 17 + 45, "Event Manager", 188, 188, 188); }

		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59))
		{
			PutString2(sX + 25, sY + iNext * 17 + 45, "Player Manipulation", 255, 255, 255);
		}
		else PutString2(sX + 25, sY + iNext * 17 + 45, "Player Manipulation", 255, 255, 100);

		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59))
		{
			PutString2(sX + 25, sY + iNext * 17 + 45, "Manipulate Online Player", 255, 255, 255);
		}
		else PutString2(sX + 25, sY + iNext * 17 + 45, "Manipulate Online Player", 255, 255, 100);

		/*iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 60) && (msY <= sY + iNext * 17 + 84))
		{	DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX+20, sY + iNext * 17 + 60, 0);
		}	else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX+20, sY + iNext * 17 + 60, 1);*/
		
		break;

	case 2: // Game Administration
		//PutString_SprFont3(sX + 50, sY + 10, "Administration Panel", 2, 150, 0);
		PutString_SprFont3(sX + 125 - ((strlen("Administration Panel") * 7) / 2), sY + 10, "Administration Panel", 2, 150, 0);
		wsprintf(G_cTxt, "Hi %s", m_cPlayerName);
		PutAlignedString(sX + 30, sX + szX - 30, sY + 30, G_cTxt, 255, 255, 255);
		PutAlignedString(sX + 32, sX + szX - 30, sY + 30, G_cTxt, 255, 255, 255);

		//PutString_SprFont3(sX + 35, sY + 50, "GameServer Administration", 200, 250, 2);
		PutString_SprFont3(sX + 115 - ((strlen("GameServer Administration") * 7) / 2), sY + 50, "GameServer Administration", 200, 250, 2);
		iNext += 2;
		if (m_iAdminUserLevel > 2) {
			if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59))
			{
				PutString2(sX + 25, sY + iNext * 17 + 45, "Map Restrictions", 255, 255, 255);
			}
			else PutString2(sX + 25, sY + iNext * 17 + 45, "Map Restrictions", 255, 255, 100);
		} else { PutString2(sX + 25, sY + iNext * 17 + 45, "Map Restrictions", 188, 188, 188); }

		iNext += 1;
		if (m_iAdminUserLevel > 2) {
			if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59))
			{
				PutString2(sX + 25, sY + iNext * 17 + 45, "Manage Server Rates (Drops, Exp)", 255, 255, 255);
			}
			else PutString2(sX + 25, sY + iNext * 17 + 45, "Manage Server Rates (Drops, Exp)", 255, 255, 100);
		} else { PutString2(sX + 25, sY + iNext * 17 + 45, "Manage Server Rates (Drops, Exp)", 188, 188, 188); }

		if ((msX > sX + 210) && (msX < sX + 240) && (msY > sY + 315) && (msY < sY + 330))
			PutString_SprFont3(sX + 210, sY + 315, "Back", 200, 250, 2);
		else PutString_SprFont3(sX + 210, sY + 315, "Back", 200, 250, 200);

		break;

	case 10: // Lock Player in BI
		//PutString_SprFont3(sX + 50, sY + 10, "Administration Panel", 2, 150, 0);
		PutString_SprFont3(sX + 125 - ((strlen("Administration Panel") * 7) / 2), sY + 10, "Administration Panel", 2, 150, 0);
		wsprintf(G_cTxt, "Hi %s", m_cPlayerName);
		PutAlignedString(sX + 30, sX + szX - 30, sY + 30, G_cTxt, 255, 255, 255);
		PutAlignedString(sX + 32, sX + szX - 30, sY + 30, G_cTxt, 255, 255, 255);

		PutString_SprFont3(sX + 65, sY + 50, "Give Player Penalty", 200, 250, 2);

		iNext += 2;
		PutString2(sX + 25, sY + iNext * 17 + 45, "Player:", 255, 255, 255);
		PutString2(sX + 65, sY + iNext * 17 + 45, cCharSelection, 255, 255, 255);
		PutString(sX + 60, sY + iNext * 17 + 47, "_____________", RGB(255, 255, 255));
		PutString(sX + 80, sY + iNext * 17 + 62, "Selecion in Days:", RGB(255, 255, 255));

		/*if (iGetTopDialogBoxIndex() != 56)
			PutString(sX + 105, sY + iNext * 17 + 45, cCharSelection, RGB(255, 255, 255), 16, false, 2);*/

		iNext += 3;
		PutAlignedString(sX + 10, sX + szX - 100, sY + iNext * 17 + 55, "Total time of player locked in Bisle: (Max: 30 days)", 255, 255, 255);
		if ((msX > sX + 165) && (msX < sX + 255) && (msY > sY + iNext * 17 + 50) && (msY < sY + iNext * 17 + 70))
			PutString_SprFont3(sX + 170, sY + iNext * 17 + 55, "Lock Char", 200, 250, 200);
		else PutString_SprFont3(sX + 170, sY + iNext * 17 + 55, "Lock Char", 200, 250, 2);
		PutAlignedString(sX + 10, sX + szX - 10, sY + iNext * 17 + 35, "___________________________________", 128, 128, 128);

		iNext += 3;
		PutAlignedString(sX + 10, sX + szX - 100, sY + iNext * 17 + 55, "Total time of player blocked in Server: (Max: 1 year)", 255, 255, 255);
		if ((msX > sX + 165) && (msX < sX + 255) && (msY > sY + iNext * 17 + 55) && (msY < sY + iNext * 17 + 75))
			PutString_SprFont3(sX + 165, sY + iNext * 17 + 60, "Block Char", 200, 250, 200);
		else PutString_SprFont3(sX + 165, sY + iNext * 17 + 60, "Block Char", 200, 250, 2);
		PutAlignedString(sX + 10, sX + szX - 10, sY + iNext * 17 + 35, "___________________________________", 128, 128, 128);

		iNext += 3;
		//PutString(sX + 10, sY + iNext * 17 + 55, "Selecting Perma IP Ban, you will include the char's IP on BannedList.cfg", RGB(50, 170, 255), false, 1);
		PutAlignedString(sX + 10, sX + szX - 100, sY + iNext * 17 + 55, "Selecting Perma IP Ban, you will include the char's IP on BannedList.cfg", 255, 255, 255);
		if ((msX > sX + 173) && (msX < sX + 240) && (msY > sY + iNext * 17 + 60) && (msY < sY + iNext * 17 + 80))
			PutString_SprFont3(sX + 180, sY + iNext * 17 + 65, "IP BAN", 200, 250, 200);
		else PutString_SprFont3(sX + 180, sY + iNext * 17 + 65, "IP BAN", 200, 250, 2);
		PutAlignedString(sX + 10, sX + szX - 10, sY + iNext * 17 + 35, "___________________________________", 128, 128, 128);

		//PutString2(sX + 25, sY + iNext * 17 + 55, "Select time (in days) of Ban:", 255, 255, 255);

		iaddx = 43;
		iaddy = -140;
		m_pSprite[DEF_SPRID_INTERFACE_ND_GAME6]->PutSpriteFast(sX + 142 + iaddx, sY + 219 + iaddy, 19, dwTime);
		m_pSprite[DEF_SPRID_INTERFACE_ND_GAME6]->PutSpriteFast(sX + 156 + iaddx, sY + 219 + iaddy, 19, dwTime);
		m_pSprite[DEF_SPRID_INTERFACE_ND_GAME6]->PutSpriteFast(sX + 170 + iaddx, sY + 219 + iaddy, 19, dwTime);

		if (iGetTopDialogBoxIndex() == 56 && msZ != 0)
		{
			m_stDialogBoxInfo[56].sV3 = m_stDialogBoxInfo[56].sV3 + msZ / 60;
			m_DInput.m_sZ = 0;
		}


		if (m_stDialogBoxInfo[56].sV3 > 365) m_stDialogBoxInfo[57].sV3 = 365;
		if (m_stDialogBoxInfo[56].sV3 < 0) m_stDialogBoxInfo[56].sV3 = 0;

		if (m_stDialogBoxInfo[56].sV3 >= 100)
		{
			ZeroMemory(cTemp, sizeof(cTemp));
			_itoa(m_stDialogBoxInfo[56].sV3, cTemp, 10);
			cTemp[1] = 0;
			PutString(sX - 35 + 172 + iaddx, sY - 10 + 237 + iaddy, cTemp, RGB(255, 255, 100));
			PutString(sX - 35 + 173 + iaddx, sY - 10 + 237 + iaddy, cTemp, RGB(255, 255, 100));

			ZeroMemory(cTemp, sizeof(cTemp));
			_itoa(m_stDialogBoxInfo[56].sV3, cTemp, 10);
			cTemp[2] = 0;
			PutString(sX - 35 + 186 + iaddx, sY - 10 + 237 + iaddy, (cTemp + 1), RGB(255, 255, 100));
			PutString(sX - 35 + 187 + iaddx, sY - 10 + 237 + iaddy, (cTemp + 1), RGB(255, 255, 100));

			ZeroMemory(cTemp, sizeof(cTemp));
			_itoa(m_stDialogBoxInfo[56].sV3, cTemp, 10);
			PutString(sX - 35 + 200 + iaddx, sY - 10 + 237 + iaddy, (cTemp + 2), RGB(255, 255, 100));
			PutString(sX - 35 + 201 + iaddx, sY - 10 + 237 + iaddy, (cTemp + 2), RGB(255, 255, 100));
		} else if (m_stDialogBoxInfo[56].sV3 >= 10)
		{
			PutString(sX - 35 + 172 + iaddx, sY - 10 + 237 + iaddy, "0", RGB(255, 255, 100));
			PutString(sX - 35 + 173 + iaddx, sY - 10 + 237 + iaddy, "0", RGB(255, 255, 100));
			ZeroMemory(cTemp, sizeof(cTemp));
			_itoa(m_stDialogBoxInfo[56].sV3, cTemp, 10);
			cTemp[1] = 0;
			PutString(sX - 35 + 186 + iaddx, sY - 10 + 237 + iaddy, cTemp, RGB(255, 255, 100));
			PutString(sX - 35 + 187 + iaddx, sY - 10 + 237 + iaddy, cTemp, RGB(255, 255, 100));
			ZeroMemory(cTemp, sizeof(cTemp));
			_itoa(m_stDialogBoxInfo[56].sV3, cTemp, 10);
			PutString(sX - 35 + 200 + iaddx, sY - 10 + 237 + iaddy, (cTemp + 1), RGB(255, 255, 100));
			PutString(sX - 35 + 201 + iaddx, sY - 10 + 237 + iaddy, (cTemp + 1), RGB(255, 255, 100));
		}
		else
		{
			PutString(sX - 35 + 172 + iaddx, sY - 10 + 237 + iaddy, "0", RGB(255, 255, 100));
			PutString(sX - 35 + 173 + iaddx, sY - 10 + 237 + iaddy, "0", RGB(255, 255, 100));

			PutString(sX - 35 + 186 + iaddx, sY - 10 + 237 + iaddy, "0", RGB(255, 255, 100));
			PutString(sX - 35 + 187 + iaddx, sY - 10 + 237 + iaddy, "0", RGB(255, 255, 100));
			ZeroMemory(cTemp, sizeof(cTemp));
			_itoa(m_stDialogBoxInfo[56].sV3, cTemp, 10);
			PutString(sX - 35 + 200 + iaddx, sY - 10 + 237 + iaddy, (cTemp), RGB(255, 255, 100));
			PutString(sX - 35 + 201 + iaddx, sY - 10 + 237 + iaddy, (cTemp), RGB(255, 255, 100));
		}
		m_pSprite[DEF_SPRID_INTERFACE_ND_GAME6]->PutSpriteFast(sX + 142 + iaddx, sY + 246 + iaddy, 20, dwTime);
		m_pSprite[DEF_SPRID_INTERFACE_ND_GAME6]->PutSpriteFast(sX + 156 + iaddx, sY + 246 + iaddy, 20, dwTime);
		m_pSprite[DEF_SPRID_INTERFACE_ND_GAME6]->PutSpriteFast(sX + 170 + iaddx, sY + 246 + iaddy, 20, dwTime);

		/*if ((msX > sX + 60) && (msX < sX + 190) && (msY > sY + 160) && (msY < sY + 185))
			PutString_SprFont3(sX + 70, sY + 170, "Apply Action", 200, 250, 2);
		else PutString_SprFont3(sX + 70, sY + 170, "Apply Action", 200, 250, 200);*/

		if ((msX > sX + 210) && (msX < sX + 260) && (msY > sY + 315) && (msY < sY + 330))
			PutString_SprFont3(sX + 210, sY + 315, "Back", 200, 250, 2);
		else PutString_SprFont3(sX + 210, sY + 315, "Back", 200, 250, 200);

		break;


	case 19: // Set Player Statistics
		//PutString_SprFont3(sX + 50, sY + 10, "Administration Panel", 2, 150, 0);
		PutString_SprFont3(sX + 125 - ((strlen("Administration Panel") * 7) / 2), sY + 10, "Administration Panel", 2, 150, 0);
		wsprintf(G_cTxt, "Hi %s", m_cPlayerName);
		PutAlignedString(sX + 30, sX + szX - 30, sY + 30, G_cTxt, 255, 255, 255);
		PutAlignedString(sX + 32, sX + szX - 30, sY + 30, G_cTxt, 255, 255, 255);

		//PutString_SprFont3(sX + 65, sY + 50, "Set Player Statistics", 200, 250, 2);
		PutString_SprFont3(sX + 125 - ((strlen("Set Player Statistics") * 7) / 2), sY + 50, "Set Player Statistics", 200, 250, 2);

		iNext += 2;
		PutString2(sX + 25, sY + iNext * 17 + 45, "Player Name:", 255, 255, 255);
		PutString2(sX + 105, sY + iNext * 17 + 45, cCharSelection, 255, 255, 255);
		PutString(sX + 100, sY + iNext * 17 + 47, "_____________", RGB(255, 255, 255));

		/*if (iGetTopDialogBoxIndex() != 56)
			PutString(sX + 105, sY + iNext * 17 + 45, cCharSelection, RGB(255, 255, 255), 16, false, 2);*/

		iNext += 2;
		PutString2(sX + 25, sY + iNext * 17 + 55, "Select Amount of change:", 255, 255, 255);

		iaddx = 43;
		iaddy = -105;
		m_pSprite[DEF_SPRID_INTERFACE_ND_GAME6]->PutSpriteFast(sX + 142 + iaddx, sY + 219 + iaddy, 19, dwTime);
		m_pSprite[DEF_SPRID_INTERFACE_ND_GAME6]->PutSpriteFast(sX + 156 + iaddx, sY + 219 + iaddy, 19, dwTime);
		m_pSprite[DEF_SPRID_INTERFACE_ND_GAME6]->PutSpriteFast(sX + 170 + iaddx, sY + 219 + iaddy, 19, dwTime);

		if (iGetTopDialogBoxIndex() == 56 && msZ != 0)
		{
			m_stDialogBoxInfo[56].sV3 = m_stDialogBoxInfo[56].sV3 + msZ / 60;
			m_DInput.m_sZ = 0;
		}

		if (m_stDialogBoxInfo[56].sV3 > 999) m_stDialogBoxInfo[57].sV3 = 999;
		if (m_stDialogBoxInfo[56].sV3 < 0) m_stDialogBoxInfo[56].sV3 = 0;

		if (m_stDialogBoxInfo[56].sV3 >= 100)
		{
			ZeroMemory(cTemp, sizeof(cTemp));
			_itoa(m_stDialogBoxInfo[56].sV3, cTemp, 10);
			cTemp[1] = 0;
			PutString(sX - 35 + 172 + iaddx, sY - 10 + 237 + iaddy, cTemp, RGB(255, 255, 100));
			PutString(sX - 35 + 173 + iaddx, sY - 10 + 237 + iaddy, cTemp, RGB(255, 255, 100));

			ZeroMemory(cTemp, sizeof(cTemp));
			_itoa(m_stDialogBoxInfo[56].sV3, cTemp, 10);
			cTemp[2] = 0;
			PutString(sX - 35 + 186 + iaddx, sY - 10 + 237 + iaddy, (cTemp + 1), RGB(255, 255, 100));
			PutString(sX - 35 + 187 + iaddx, sY - 10 + 237 + iaddy, (cTemp + 1), RGB(255, 255, 100));

			ZeroMemory(cTemp, sizeof(cTemp));
			_itoa(m_stDialogBoxInfo[56].sV3, cTemp, 10);
			PutString(sX - 35 + 200 + iaddx, sY - 10 + 237 + iaddy, (cTemp + 2), RGB(255, 255, 100));
			PutString(sX - 35 + 201 + iaddx, sY - 10 + 237 + iaddy, (cTemp + 2), RGB(255, 255, 100));
		}
		else if (m_stDialogBoxInfo[56].sV3 >= 10)
		{
			PutString(sX - 35 + 172 + iaddx, sY - 10 + 237 + iaddy, "0", RGB(255, 255, 100));
			PutString(sX - 35 + 173 + iaddx, sY - 10 + 237 + iaddy, "0", RGB(255, 255, 100));
			ZeroMemory(cTemp, sizeof(cTemp));
			_itoa(m_stDialogBoxInfo[56].sV3, cTemp, 10);
			cTemp[1] = 0;
			PutString(sX - 35 + 186 + iaddx, sY - 10 + 237 + iaddy, cTemp, RGB(255, 255, 100));
			PutString(sX - 35 + 187 + iaddx, sY - 10 + 237 + iaddy, cTemp, RGB(255, 255, 100));
			ZeroMemory(cTemp, sizeof(cTemp));
			_itoa(m_stDialogBoxInfo[56].sV3, cTemp, 10);
			PutString(sX - 35 + 200 + iaddx, sY - 10 + 237 + iaddy, (cTemp + 1), RGB(255, 255, 100));
			PutString(sX - 35 + 201 + iaddx, sY - 10 + 237 + iaddy, (cTemp + 1), RGB(255, 255, 100));
		}
		else
		{
			PutString(sX - 35 + 172 + iaddx, sY - 10 + 237 + iaddy, "0", RGB(255, 255, 100));
			PutString(sX - 35 + 173 + iaddx, sY - 10 + 237 + iaddy, "0", RGB(255, 255, 100));

			PutString(sX - 35 + 186 + iaddx, sY - 10 + 237 + iaddy, "0", RGB(255, 255, 100));
			PutString(sX - 35 + 187 + iaddx, sY - 10 + 237 + iaddy, "0", RGB(255, 255, 100));
			ZeroMemory(cTemp, sizeof(cTemp));
			_itoa(m_stDialogBoxInfo[56].sV3, cTemp, 10);
			PutString(sX - 35 + 200 + iaddx, sY - 10 + 237 + iaddy, (cTemp), RGB(255, 255, 100));
			PutString(sX - 35 + 201 + iaddx, sY - 10 + 237 + iaddy, (cTemp), RGB(255, 255, 100));
		}
		m_pSprite[DEF_SPRID_INTERFACE_ND_GAME6]->PutSpriteFast(sX + 142 + iaddx, sY + 246 + iaddy, 20, dwTime);
		m_pSprite[DEF_SPRID_INTERFACE_ND_GAME6]->PutSpriteFast(sX + 156 + iaddx, sY + 246 + iaddy, 20, dwTime);
		m_pSprite[DEF_SPRID_INTERFACE_ND_GAME6]->PutSpriteFast(sX + 170 + iaddx, sY + 246 + iaddy, 20, dwTime);


		iNext += 3;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59))
			PutAlignedString(sX + 10, sX + szX - 10, sY + iNext * 17 + 45, "Apply Ek Points", 255, 255, 255);
		else PutAlignedString(sX + 10, sX + szX - 10, sY + iNext * 17 + 45, "Apply Ek Points", 255, 255, 100);
		iNext += 2;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59))
			PutAlignedString(sX + 10, sX + szX - 10, sY + iNext * 17 + 45, "Apply Contribution Points", 255, 255, 255);
		else PutAlignedString(sX + 10, sX + szX - 10, sY + iNext * 17 + 45, "Apply Contribution Points", 255, 255, 100);
		iNext += 2;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59))
			PutAlignedString(sX + 10, sX + szX - 10, sY + iNext * 17 + 45, "Apply Majestics Points", 255, 255, 255);
		else PutAlignedString(sX + 10, sX + szX - 10, sY + iNext * 17 + 45, "Apply Majestics Points", 255, 255, 100);
		iNext += 2;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59))
			PutAlignedString(sX + 10, sX + szX - 10, sY + iNext * 17 + 45, "Apply Reputation Points", 255, 255, 255);
		else PutAlignedString(sX + 10, sX + szX - 10, sY + iNext * 17 + 45, "Apply Reputation Points", 255, 255, 100);
		iNext += 2;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59))
			PutAlignedString(sX + 10, sX + szX - 10, sY + iNext * 17 + 45, "Apply Coin Points", 255, 255, 255);
		else PutAlignedString(sX + 10, sX + szX - 10, sY + iNext * 17 + 45, "Apply Coin Points", 255, 255, 100);


		
		/*iNext += 3;
		if ((msX > sX + 60) && (msX < sX + 190) && (msY > sY + iNext * 17 + 45) && (msY < sY + iNext * 17 + 59))
			PutString_SprFont3(sX + 70, sY + iNext * 17 + 45, "Apply EK Points", 200, 250, 2);
		else PutString_SprFont3(sX + 70, sY + iNext * 17 + 45, "Apply EK Points", 200, 250, 200);
		iNext += 2;
		if ((msX > sX + 30) && (msX < sX + 190) && (msY > sY + iNext * 17 + 45) && (msY < sY + iNext * 17 + 59))
			PutString_SprFont3(sX + 30, sY + iNext * 17 + 45, "Apply Contribution Points", 200, 250, 2);
		else PutString_SprFont3(sX + 30, sY + iNext * 17 + 45, "Apply Contribution Points", 200, 250, 200);
		iNext += 2;
		if ((msX > sX + 40) && (msX < sX + 190) && (msY > sY + iNext * 17 + 45) && (msY < sY + iNext * 17 + 59))
			PutString_SprFont3(sX + 45, sY + iNext * 17 + 45, "Apply Majestics Points", 200, 250, 2);
		else PutString_SprFont3(sX + 45, sY + iNext * 17 + 45, "Apply Majestics Points", 200, 250, 200);
		iNext += 2;
		if ((msX > sX + 40) && (msX < sX + 190) && (msY > sY + iNext * 17 + 45) && (msY < sY + iNext * 17 + 59))
			PutString_SprFont3(sX + 45, sY + iNext * 17 + 45, "Apply Reputation Points", 200, 250, 2);
		else PutString_SprFont3(sX + 45, sY + iNext * 17 + 45, "Apply Reputation Points", 200, 250, 200);
		iNext += 2;
		if ((msX > sX + 55) && (msX < sX + 190) && (msY > sY + iNext * 17 + 45) && (msY < sY + iNext * 17 + 59))
			PutString_SprFont3(sX + 63, sY + iNext * 17 + 45, "Apply Coin Points", 200, 250, 2);
		else PutString_SprFont3(sX + 63, sY + iNext * 17 + 45, "Apply Coin Points", 200, 250, 200);*/

		if ((msX > sX + 210) && (msX < sX + 260) && (msY > sY + 315) && (msY < sY + 340))
			PutString_SprFont3(sX + 210, sY + 315, "Back", 200, 250, 2);
		else PutString_SprFont3(sX + 210, sY + 315, "Back", 200, 250, 200);

		break;

	case 20: // Map Restrictions
		//PutString_SprFont3(sX + 50, sY + 10, "Administration Panel", 2, 150, 0);
		PutString_SprFont3(sX + 125 - ((strlen("Administration Panel") * 7) / 2), sY + 10, "Administration Panel", 2, 150, 0);
		wsprintf(G_cTxt, "Hi %s", m_cPlayerName);
		PutAlignedString(sX + 30, sX + szX - 30, sY + 30, G_cTxt, 255, 255, 255);
		PutAlignedString(sX + 32, sX + szX - 30, sY + 30, G_cTxt, 255, 255, 255);

		//PutString_SprFont3(sX + 65, sY + 50, "Map Restrictions", 200, 250, 2);
		PutString_SprFont3(sX + 125 - ((strlen("Map Restrictions") * 7) / 2), sY + 10, "Map Restrictions", 200, 250, 2);
		iNext += 2;
		PutString2(sX + 15, sY + iNext * 17 + 45, "Attack Map Restriction:", 255, 255, 255);
		if (m_bAttackMode) {
			PutString2(sX + 200, sY + iNext * 17 + 45, "OFF", 255, 0, 0);
		}
		else {
			PutString2(sX + 200, sY + iNext * 17 + 45, "ON", 0, 255, 0);
		}

		iNext += 1;
		PutString2(sX + 15, sY + iNext * 17 + 45, "Teleport", 255, 255, 255);
		if (!bMapTP) {
			PutString2(sX + 200, sY + iNext * 17 + 45, "OFF", 255, 0, 0);
		}
		else {	PutString2(sX + 200, sY + iNext * 17 + 45, "ON", 0, 255, 0);
		}

		iNext += 1;
		PutString2(sX + 15, sY + iNext * 17 + 45, "Party Mode", 255, 255, 255);
		if (!bMapParty) {
			PutString2(sX + 200, sY + iNext * 17 + 45, "OFF", 255, 0, 0);
		}
		else {	PutString2(sX + 200, sY + iNext * 17 + 45, "ON", 0, 255, 0); }

		iNext += 1;
		PutString2(sX + 15, sY + iNext * 17 + 45, "Illusion Flag", 255, 255, 255);
		if (!bMapIllusion) {
			PutString2(sX + 200, sY + iNext * 17 + 45, "OFF", 255, 0, 0);
		}
		else {
			PutString2(sX + 200, sY + iNext * 17 + 45, "ON", 0, 255, 0);
		}

		iNext += 1;
		PutString2(sX + 15, sY + iNext * 17 + 45, "Activations", 255, 255, 255);
		if (!bMapActivate) {
			PutString2(sX + 200, sY + iNext * 17 + 45, "OFF", 255, 0, 0);
		}
		else {
			PutString2(sX + 200, sY + iNext * 17 + 45, "ON", 0, 255, 0);
		}

		iNext += 1;
		PutString2(sX + 15, sY + iNext * 17 + 45, "Invisibility", 255, 255, 255);
		if (!bMapInvi) {
			PutString2(sX + 200, sY + iNext * 17 + 45, "OFF", 255, 0, 0);
		}
		else {
			PutString2(sX + 200, sY + iNext * 17 + 45, "ON", 0, 255, 0);
		}

		iNext += 1;
		PutString2(sX + 15, sY + iNext * 17 + 45, "Abs. Magic Protection (AMP)", 255, 255, 255);
		if (!bMapAMP) {
			PutString2(sX + 200, sY + iNext * 17 + 45, "OFF", 255, 0, 0);
		}
		else {
			PutString2(sX + 200, sY + iNext * 17 + 45, "ON", 0, 255, 0);
		}

		iNext += 1;
		PutString2(sX + 15, sY + iNext * 17 + 45, "Regens (HP, MP, SP)", 255, 255, 255);
		if (!bMapRegens) {
			PutString2(sX + 200, sY + iNext * 17 + 45, "OFF", 255, 0, 0);
		}
		else { PutString2(sX + 200, sY + iNext * 17 + 45, "ON", 0, 255, 0); }

		iNext += 1;
		PutString2(sX + 15, sY + iNext * 17 + 45, "Hide Enemy Name", 255, 255, 255);
		if (!bMapHideEnemy) {
			PutString2(sX + 200, sY + iNext * 17 + 45, "OFF", 255, 0, 0);
		}
		else { PutString2(sX + 200, sY + iNext * 17 + 45, "ON", 0, 255, 0); }

		iNext += 1;
		PutString2(sX + 15, sY + iNext * 17 + 45, "Bonus Damage (50%)", 255, 255, 255);
		if (!bMapBonusDmg) {
			PutString2(sX + 200, sY + iNext * 17 + 45, "OFF", 255, 0, 0);
		}
		else { PutString2(sX + 200, sY + iNext * 17 + 45, "ON", 0, 255, 0); }

		iNext += 1;
		PutString2(sX + 15, sY + iNext * 17 + 45, "Equip Items", 255, 255, 255);
		if (!bMapEquip) {
			PutString2(sX + 200, sY + iNext * 17 + 45, "OFF", 255, 0, 0);
		}
		else { PutString2(sX + 200, sY + iNext * 17 + 45, "ON", 0, 255, 0); }

		if ((msX > sX + 210) && (msX < sX + 240) && (msY > sY + 315) && (msY < sY + 330))
			PutString_SprFont3(sX + 210, sY + 315, "Back", 200, 250, 2);
		else PutString_SprFont3(sX + 210, sY + 315, "Back", 200, 250, 200);

		break;

	case 21: // GM Commands
		//PutString_SprFont3(sX + 50, sY + 10, "Administration Panel", 2, 150, 0);
		PutString_SprFont3(sX + 125 - ((strlen("Administration Panel") * 7) / 2), sY + 10, "Administration Panel", 2, 150, 0);
		wsprintf(G_cTxt, "Hi %s", m_cPlayerName);
		PutAlignedString(sX + 30, sX + szX - 30, sY + 30, G_cTxt, 255, 255, 255);
		PutAlignedString(sX + 32, sX + szX - 30, sY + 30, G_cTxt, 255, 255, 255);

		//PutString_SprFont3(sX + 70, sY + 50, "Admin Commands", 200, 250, 2);
		PutString_SprFont3(sX + 120 - ((strlen("Admin Commands") * 7) / 2), sY + 50, "Admin Commands", 200, 250, 2);
		iNext += 2;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			PutString2(sX + 25, sY + iNext * 17 + 45, "Enable Commands", 255, 255, 255);
		}
		else PutString2(sX + 25, sY + iNext * 17 + 45, "Enable Commands", 255, 255, 100);
		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			PutString2(sX + 25, sY + iNext * 17 + 45, "Create Item", 255, 255, 255);
		}
		else PutString2(sX + 25, sY + iNext * 17 + 45, "Create Item", 255, 255, 100);
		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			PutString2(sX + 25, sY + iNext * 17 + 45, "Create Fragile Item", 255, 255, 255);
		}
		else PutString2(sX + 25, sY + iNext * 17 + 45, "Create Fragile Item", 255, 255, 100);

		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			PutString2(sX + 25, sY + iNext * 17 + 45, "Teleport", 255, 255, 255);
		}
		else PutString2(sX + 25, sY + iNext * 17 + 45, "Teleport", 255, 255, 100);

		iNext += 1;
		if (m_iAdminUserLevel > 3) {
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			PutString2(sX + 25, sY + iNext * 17 + 45, "Add Game Master", 255, 255, 255);
			}	else PutString2(sX + 25, sY + iNext * 17 + 45, "Add Game Master", 255, 255, 100);
		} else { PutString2(sX + 25, sY + iNext * 17 + 45, "Add Game Master", 188, 188, 188); }

		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			PutString2(sX + 25, sY + iNext * 17 + 45, "Upgrade Admin Level", 255, 255, 255);
		}
		else PutString2(sX + 25, sY + iNext * 17 + 45, "Upgrade Admin Level", 255, 255, 100);

		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			PutString2(sX + 25, sY + iNext * 17 + 45, "Admin GoTo", 255, 255, 255);
		}
		else PutString2(sX + 25, sY + iNext * 17 + 45, "Admin GoTo", 255, 255, 100);

		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			PutString2(sX + 25, sY + iNext * 17 + 45, "Set Inv ON/OFF", 255, 255, 255);
		}
		else PutString2(sX + 25, sY + iNext * 17 + 45, "Set Inv ON/OFF", 255, 255, 100);

		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			PutString2(sX + 25, sY + iNext * 17 + 45, "Summon Creatures", 255, 255, 255);
		}
		else PutString2(sX + 25, sY + iNext * 17 + 45, "Summon Creatures", 255, 255, 100);

		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			PutString2(sX + 25, sY + iNext * 17 + 45, "Clear Map", 255, 255, 255);
		}
		else PutString2(sX + 25, sY + iNext * 17 + 45, "Clear Map", 255, 255, 100);

		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			PutString2(sX + 25, sY + iNext * 17 + 45, "Clear Npc", 255, 255, 255);
		}
		else PutString2(sX + 25, sY + iNext * 17 + 45, "Clear Npc", 255, 255, 100);

		iNext += 1;
		if (m_iAdminUserLevel > 3) {
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			PutString2(sX + 25, sY + iNext * 17 + 45, "Disconnect All", 255, 255, 255);
			}	else PutString2(sX + 25, sY + iNext * 17 + 45, "Disconnect All", 255, 255, 100);
		} else { PutString2(sX + 25, sY + iNext * 17 + 45, "Disconnect All", 188, 188, 188); }

		iNext += 1;
		if (m_iAdminUserLevel > 3) {
			if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
				PutString2(sX + 25, sY + iNext * 17 + 45, "Shut down server", 255, 255, 255);
			}
			else PutString2(sX + 25, sY + iNext * 17 + 45, "Shut down server", 255, 255, 100);
		} else { PutString2(sX + 25, sY + iNext * 17 + 45, "Shut down server", 188, 188, 188); }

		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			PutString2(sX + 25, sY + iNext * 17 + 45, "Destroy all GM Items", 255, 255, 255);
		}
		else PutString2(sX + 25, sY + iNext * 17 + 45, "Destroy all GM Items", 255, 255, 100);

		if ((msX > sX + 210) && (msX < sX + 240) && (msY > sY + 315) && (msY < sY + 330))
			PutString_SprFont3(sX + 210, sY + 315, "Back", 200, 250, 2);
		else PutString_SprFont3(sX + 210, sY + 315, "Back", 200, 250, 200);

		break;

	case 22: // Event Manager
		//PutString_SprFont3(sX + 50, sY + 10, "Administration Panel", 2, 150, 0);
		PutString_SprFont3(sX + 125 - ((strlen("Administration Panel") * 7) / 2), sY + 10, "Administration Panel", 2, 150, 0);
		wsprintf(G_cTxt, "Hi %s", m_cPlayerName);
		PutAlignedString(sX + 30, sX + szX - 30, sY + 30, G_cTxt, 255, 255, 255);
		PutAlignedString(sX + 32, sX + szX - 30, sY + 30, G_cTxt, 255, 255, 255);

		PutString_SprFont3(sX + 65, sY + 50, "Event Manager", 200, 250, 2);
		iNext += 2;
		PutString2(sX + 25, sY + iNext * 17 + 45, "Deathmatch Arena", 255, 255, 255);
		if (!bDeathmatch) {
			PutString2(sX + 200, sY + iNext * 17 + 45, "OFF", 255, 0, 0);
		}
		else {
			PutString2(sX + 200, sY + iNext * 17 + 45, "ON", 0, 255, 0);
		}
		iNext += 1;
		PutString2(sX + 25, sY + iNext * 17 + 45, "The Crusade", 255, 255, 255);
		if (!m_bIsCrusadeMode) {
			PutString2(sX + 200, sY + iNext * 17 + 45, "OFF", 255, 0, 0);
		}
		else {
			PutString2(sX + 200, sY + iNext * 17 + 45, "ON", 0, 255, 0);
		}
		iNext += 1;
		PutString2(sX + 25, sY + iNext * 17 + 45, "The Apocalypse", 255, 255, 255);
		if (!m_bApocalypse) {
			PutString2(sX + 200, sY + iNext * 17 + 45, "OFF", 255, 0, 0);
		}
		else {
			PutString2(sX + 200, sY + iNext * 17 + 45, "ON", 0, 255, 0);
		}
		iNext += 1;
		PutString2(sX + 25, sY + iNext * 17 + 45, "The Heldenian", 255, 255, 255);
		if (!m_bIsHeldenian) {
			PutString2(sX + 200, sY + iNext * 17 + 45, "OFF", 255, 0, 0);
		}
		else {
			PutString2(sX + 200, sY + iNext * 17 + 45, "ON", 0, 255, 0);
		}
		iNext += 1;
		PutString2(sX + 25, sY + iNext * 17 + 45, "Capture the Flag", 255, 255, 255);
		if (!m_bIsCTFMode) {
			PutString2(sX + 200, sY + iNext * 17 + 45, "OFF", 255, 0, 0);
		}
		else {
			PutString2(sX + 200, sY + iNext * 17 + 45, "ON", 0, 255, 0);
		}

		// kazin
		iNext += 1;
		PutString2(sX + 25, sY + iNext * 17 + 45, "Candy Fury", 255, 255, 255);
		if (!_candy_boost) {
			PutString2(sX + 200, sY + iNext * 17 + 45, "OFF", 255, 0, 0);
		}
		else {
			PutString2(sX + 200, sY + iNext * 17 + 45, "ON", 0, 255, 0);
		}

		iNext += 1;
		PutString2(sX + 25, sY + iNext * 17 + 45, "Beholder Event", 255, 255, 255);
		if (!_revelation) {
			PutString2(sX + 200, sY + iNext * 17 + 45, "OFF", 255, 0, 0);
		}
		else {
			PutString2(sX + 200, sY + iNext * 17 + 45, "ON", 0, 255, 0);
		}

		iNext += 1;
		PutString2(sX + 25, sY + iNext * 17 + 45, "Invasion Time", 255, 255, 255);
		if (!_city_teleport) {
			PutString2(sX + 200, sY + iNext * 17 + 45, "OFF", 255, 0, 0);
		}
		else {
			PutString2(sX + 200, sY + iNext * 17 + 45, "ON", 0, 255, 0);
		}

		iNext += 1;
		PutString2(sX + 25, sY + iNext * 17 + 45, "Bag Protection", 255, 255, 255);
		if (!_drop_inhib) {
			PutString2(sX + 200, sY + iNext * 17 + 45, "OFF", 255, 0, 0);
		}
		else {
			PutString2(sX + 200, sY + iNext * 17 + 45, "ON", 0, 255, 0);
		}

		iNext += 1;
		PutString2(sX + 25, sY + iNext * 17 + 45, "Team Arena", 255, 255, 255);
		if (!_team_arena) {
			PutString2(sX + 200, sY + iNext * 17 + 45, "OFF", 255, 0, 0);
		}
		else {
			PutString2(sX + 200, sY + iNext * 17 + 45, "ON", 0, 255, 0);
		}

		iNext += 1;
		PutString2(sX + 25, sY + iNext * 17 + 45, "Shinning Event", 255, 255, 255);
		if (!bShinning) {
			PutString2(sX + 200, sY + iNext * 17 + 45, "OFF", 255, 0, 0);
		}
		else {
			PutString2(sX + 200, sY + iNext * 17 + 45, "ON", 0, 255, 0);
		}

		iNext += 1;
		PutString2(sX + 25, sY + iNext * 17 + 45, "Happy Hour", 255, 255, 255);
		if (!m_bHappyHour) {
			PutString2(sX + 200, sY + iNext * 17 + 45, "OFF", 255, 0, 0);
		}
		else {
			PutString2(sX + 200, sY + iNext * 17 + 45, "ON", 0, 255, 0);
		}

		iNext += 1;
		PutString2(sX + 25, sY + iNext * 17 + 45, "Fury Hour", 255, 255, 255);
		if (!m_bFuryHour) {
			PutString2(sX + 200, sY + iNext * 17 + 45, "OFF", 255, 0, 0);
		}
		else {
			PutString2(sX + 200, sY + iNext * 17 + 45, "ON", 0, 255, 0);
		}

		if ((msX > sX + 210) && (msX < sX + 240) && (msY > sY + 315) && (msY < sY + 330))
			PutString_SprFont3(sX + 210, sY + 315, "Back", 200, 250, 2);
		else PutString_SprFont3(sX + 210, sY + 315, "Back", 200, 250, 200);

		break;

	case 23: // Player Manipulation
		//PutString_SprFont3(sX + 51, sY + 10, "Administration Panel", 2, 150, 0);
		PutString_SprFont3(sX + 125 - ((strlen("Administration Panel") * 7) / 2), sY + 10, "Administration Panel", 2, 150, 0);
		/*wsprintf(G_cTxt, "Hi %s", m_cPlayerName);
		PutAlignedString(sX + 30, sX + szX - 30, sY + 30, G_cTxt, 255, 255, 255);
		PutAlignedString(sX + 32, sX + szX - 30, sY + 30, G_cTxt, 255, 255, 255);*/

		//PutString_SprFont3(sX + 56, sY + 30, "Player Manipulation", 200, 250, 2);
		PutString_SprFont3(sX + 125 - ((strlen("Player Manipulation") * 7) / 2), sY + 30, "Player Manipulation", 200, 250, 2);

		wsprintf(G_cTxt, "Player to Manipulate: %s", cCharSelection);
		PutAlignedString(sX + 30, sX + szX - 30, sY + 55, G_cTxt, 255, 255, 255);
		PutAlignedString(sX + 32, sX + szX - 30, sY + 55, G_cTxt, 255, 255, 255);

		iNext += 2;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			PutString2(sX + 25, sY + iNext * 17 + 45, "Revive player", 255, 255, 255);
		}
		else PutString2(sX + 25, sY + iNext * 17 + 45, "Revive player", 255, 255, 100);

		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			PutString2(sX + 25, sY + iNext * 17 + 45, "Kill player", 255, 255, 255);
		}
		else PutString2(sX + 25, sY + iNext * 17 + 45, "Kill player", 255, 255, 100);

		iNext += 1;
		if (m_iAdminUserLevel > 1) {
			if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
				PutString2(sX + 25, sY + iNext * 17 + 45, "Set Player Attributes", 255, 255, 255);
			}
			else PutString2(sX + 25, sY + iNext * 17 + 45, "Set Player Attributes", 255, 255, 100);
		} else { PutString2(sX + 25, sY + iNext * 17 + 45, "Set Player Attributes", 188, 188, 188); }

		iNext += 1;
		if (m_iAdminUserLevel > 1) {
			if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
				PutString2(sX + 25, sY + iNext * 17 + 45, "Mute player", 255, 255, 255);
			}
			else PutString2(sX + 25, sY + iNext * 17 + 45, "Mute player", 255, 255, 100);
		}
		else { PutString2(sX + 25, sY + iNext * 17 + 45, "Mute player", 188, 188, 188); }

		iNext += 1;
		if (m_iAdminUserLevel > 1) {
			if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
				PutString2(sX + 25, sY + iNext * 17 + 45, "Summon player", 255, 255, 255);
			}
			else PutString2(sX + 25, sY + iNext * 17 + 45, "Summon player", 255, 255, 100);
		}
		else { PutString2(sX + 25, sY + iNext * 17 + 45, "Summon player", 188, 188, 188); }

		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			PutString2(sX + 25, sY + iNext * 17 + 45, "Teleport player", 255, 255, 255);
		}
		else PutString2(sX + 25, sY + iNext * 17 + 45, "Teleport player", 255, 255, 100);

		iNext += 1;
		if (m_iAdminUserLevel > 1) {
			if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
				PutString2(sX + 25, sY + iNext * 17 + 45, "Close player connection", 255, 255, 255);
			}
			else PutString2(sX + 25, sY + iNext * 17 + 45, "Close player connection", 255, 255, 100);
		}
		else { PutString2(sX + 25, sY + iNext * 17 + 45, "Close player connection", 188, 188, 188); }

		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			PutString2(sX + 25, sY + iNext * 17 + 45, "Check player information", 255, 255, 255);
		}
		else PutString2(sX + 25, sY + iNext * 17 + 45, "Check player information", 255, 255, 100);

		iNext += 1;
		if (m_iAdminUserLevel > 1) {
			if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
				PutString2(sX + 25, sY + iNext * 17 + 45, "Apply Player Penalty", 255, 255, 255);
			}
			else PutString2(sX + 25, sY + iNext * 17 + 45, "Apply Player Penalty", 255, 255, 100);
		}
		else { PutString2(sX + 25, sY + iNext * 17 + 45, "Apply Player Penalty", 188, 188, 188); }

		if ((msX > sX + 210) && (msX < sX + 240) && (msY > sY + 315) && (msY < sY + 330))
			PutString_SprFont3(sX + 210, sY + 315, "Back", 200, 250, 2);
		else PutString_SprFont3(sX + 210, sY + 315, "Back", 200, 250, 200);

		break;

	case 24:
		PutString_SprFont2(sX + 125 - ((strlen("Drop and Damage Management") * 7) / 2), sY + 10, "Drop and Damage Management", 240, 240, 240);

		wsprintf(G_cTxt, "Hi %s", m_cPlayerName);
		PutAlignedString(sX + 30, sX + szX - 30, sY + 30, G_cTxt, 255, 255, 255);
		PutAlignedString(sX + 32, sX + szX - 30, sY + 30, G_cTxt, 255, 255, 255);

		iNext += 2;
		PutString2(sX + 25, sY + iNext * 17 + 20, "Primary Drop rate", 19, 104, 169);
		if ((msX >= sX + 180) && (msX <= sX + 195) && (msY >= sY + iNext * 17 + 20) && (msY <= sY + iNext * 17 + 34))
			m_pSprite[DEF_SPRID_INTERFACE_ND_GAME6]->PutSpriteFast(sX + 190, sY + iNext * 17 + 28, 17, dwTime);
		else m_pSprite[DEF_SPRID_INTERFACE_ND_GAME6]->PutSpriteFast(sX + 190, sY + iNext * 17 + 28, 19, dwTime); // +
		
		ZeroMemory(cDrops, sizeof(cDrops));
		wsprintf(cDrops, "%d%%", iFirstDropProb/100);
		PutAlignedString2(sX + 200, sX + 230, sY + iNext * 17 + 20, cDrops, 255, 255, 255);

		if ((msX >= sX + 230) && (msX <= sX + 245) && (msY >= sY + iNext * 17 + 20) && (msY <= sY + iNext * 17 + 34))
			m_pSprite[DEF_SPRID_INTERFACE_ND_GAME6]->PutSpriteFast(sX + 240, sY + iNext * 17 + 28, 18, dwTime);
		else m_pSprite[DEF_SPRID_INTERFACE_ND_GAME6]->PutSpriteFast(sX + 240, sY + iNext * 17 + 28, 20, dwTime); // -
		
		iNext += 1;
		PutAlignedString2(sX + 10, sX + 250, sY + iNext * 17 + 20, "Primary Drop manage will decide if a item will be droped, or not.", 210, 255, 0);

		iNext += 2;
		PutString2(sX + 25, sY + iNext * 17 + 20, "Secondary Drop rate", 19, 104, 169);
		if ((msX >= sX + 180) && (msX <= sX + 195) && (msY >= sY + iNext * 17 + 20) && (msY <= sY + iNext * 17 + 34))
			m_pSprite[DEF_SPRID_INTERFACE_ND_GAME6]->PutSpriteFast(sX + 190, sY + iNext * 17 + 28, 17, dwTime);
		else m_pSprite[DEF_SPRID_INTERFACE_ND_GAME6]->PutSpriteFast(sX + 190, sY + iNext * 17 + 28, 19, dwTime); // +

		ZeroMemory(cDrops, sizeof(cDrops));
		wsprintf(cDrops, "%d%%", iSecDropPro/100);
		PutAlignedString2(sX + 200, sX + 230, sY + iNext * 17 + 20, cDrops, 255, 255, 255);

		if ((msX >= sX + 230) && (msX <= sX + 245) && (msY >= sY + iNext * 17 + 20) && (msY <= sY + iNext * 17 + 34))
			m_pSprite[DEF_SPRID_INTERFACE_ND_GAME6]->PutSpriteFast(sX + 240, sY + iNext * 17 + 28, 18, dwTime);
		else m_pSprite[DEF_SPRID_INTERFACE_ND_GAME6]->PutSpriteFast(sX + 240, sY + iNext * 17 + 28, 20, dwTime); // -
		iNext += 1;
		ZeroMemory(cDrops, sizeof(cDrops));
		wsprintf(cDrops, "Chance to drop Pots, Zems, Stones: %d%%", iSecDropPro / 100);
		PutAlignedString2(sX + 10, sX + 250, sY + iNext * 17 + 20, cDrops, 210, 255, 0);
		iNext += 1;
		ZeroMemory(cDrops, sizeof(cDrops));
		wsprintf(cDrops, "Chance to Armors & Weapons: %d%%", 100 - (iSecDropPro / 100));
		PutAlignedString2(sX + 10, sX + 250, sY + iNext * 17 + 20, cDrops, 210, 255, 0);

		iNext += 1;
		PutString2(sX + 25, sY + iNext * 17 + 20, "Stated Itens Drop rate", 19, 104, 169);
		if ((msX >= sX + 180) && (msX <= sX + 195) && (msY >= sY + iNext * 17 + 20) && (msY <= sY + iNext * 17 + 34))
			m_pSprite[DEF_SPRID_INTERFACE_ND_GAME6]->PutSpriteFast(sX + 190, sY + iNext * 17 + 28, 17, dwTime);
		else m_pSprite[DEF_SPRID_INTERFACE_ND_GAME6]->PutSpriteFast(sX + 190, sY + iNext * 17 + 28, 19, dwTime); // +

		ZeroMemory(cDrops, sizeof(cDrops));
		wsprintf(cDrops, "%d%%", 100 - (iStatedDropProb/100));
		PutAlignedString2(sX + 200, sX + 230, sY + iNext * 17 + 20, cDrops, 255, 255, 255);

		if ((msX >= sX + 230) && (msX <= sX + 245) && (msY >= sY + iNext * 17 + 20) && (msY <= sY + iNext * 17 + 34))
			m_pSprite[DEF_SPRID_INTERFACE_ND_GAME6]->PutSpriteFast(sX + 240, sY + iNext * 17 + 28, 18, dwTime);
		else m_pSprite[DEF_SPRID_INTERFACE_ND_GAME6]->PutSpriteFast(sX + 240, sY + iNext * 17 + 28, 20, dwTime); // -
		iNext += 1;
		PutAlignedString2(sX + 10, sX + 250, sY + iNext * 17 + 20, "Stated Drop manage the probability if the Armor/Weapon will have any special stats.", 210, 255, 0);

		iNext += 2;
		PutString2(sX + 25, sY + iNext * 17 + 20, "Magic Abs. Drop is", 19, 104, 169);
		if (m_bNullDrop[DROP_MA])
		PutString2(sX + 210, sY + iNext * 17 + 20, "ON", 0, 255, 0);
		else PutString2(sX + 210, sY + iNext * 17 + 20, "OFF", 255, 0, 0);

		iNext += 1;
		PutString2(sX + 25, sY + iNext * 17 + 20, "Physical Abs. Drop is", 19, 104, 169);
		if (m_bNullDrop[DROP_PA])
			PutString2(sX + 210, sY + iNext * 17 + 20, "ON", 0, 255, 0);
		else PutString2(sX + 210, sY + iNext * 17 + 20, "OFF", 255, 0, 0);

		iNext += 2;
		PutString2(sX + 25, sY + iNext * 17 + 20, "Server Physical Damage", 19, 104, 169);
		if ((msX >= sX + 180) && (msX <= sX + 195) && (msY >= sY + iNext * 17 + 20) && (msY <= sY + iNext * 17 + 34))
			m_pSprite[DEF_SPRID_INTERFACE_ND_GAME6]->PutSpriteFast(sX + 190, sY + iNext * 17 + 28, 17, dwTime);
		else m_pSprite[DEF_SPRID_INTERFACE_ND_GAME6]->PutSpriteFast(sX + 190, sY + iNext * 17 + 28, 19, dwTime); // +
		ZeroMemory(cDrops, sizeof(cDrops));
		wsprintf(cDrops, "%d%", iServerPDamage*5);
		PutAlignedString2(sX + 200, sX + 230, sY + iNext * 17 + 20, cDrops, 255, 255, 255);
		if ((msX >= sX + 230) && (msX <= sX + 245) && (msY >= sY + iNext * 17 + 20) && (msY <= sY + iNext * 17 + 34))
			m_pSprite[DEF_SPRID_INTERFACE_ND_GAME6]->PutSpriteFast(sX + 240, sY + iNext * 17 + 28, 18, dwTime);
		else m_pSprite[DEF_SPRID_INTERFACE_ND_GAME6]->PutSpriteFast(sX + 240, sY + iNext * 17 + 28, 20, dwTime); // -
		iNext += 1;
		PutString2(sX + 25, sY + iNext * 17 + 20, "Server Magical Damage", 19, 104, 169);
		if ((msX >= sX + 180) && (msX <= sX + 195) && (msY >= sY + iNext * 17 + 20) && (msY <= sY + iNext * 17 + 34))
			m_pSprite[DEF_SPRID_INTERFACE_ND_GAME6]->PutSpriteFast(sX + 190, sY + iNext * 17 + 28, 17, dwTime);
		else m_pSprite[DEF_SPRID_INTERFACE_ND_GAME6]->PutSpriteFast(sX + 190, sY + iNext * 17 + 28, 19, dwTime); // +
		ZeroMemory(cDrops, sizeof(cDrops));
		wsprintf(cDrops, "%d%", iServerMDamage*5);
		PutAlignedString2(sX + 200, sX + 230, sY + iNext * 17 + 20, cDrops, 255, 255, 255);
		if ((msX >= sX + 230) && (msX <= sX + 245) && (msY >= sY + iNext * 17 + 20) && (msY <= sY + iNext * 17 + 34))
			m_pSprite[DEF_SPRID_INTERFACE_ND_GAME6]->PutSpriteFast(sX + 240, sY + iNext * 17 + 28, 18, dwTime);
		else m_pSprite[DEF_SPRID_INTERFACE_ND_GAME6]->PutSpriteFast(sX + 240, sY + iNext * 17 + 28, 20, dwTime); // -

		break;

	case 25: // GM Teleport List
		//PutString_SprFont3(sX + 50, sY + 10, "Administration Panel", 2, 150, 0);
		PutString_SprFont3(sX + 125 - ((strlen("Administration Panel") * 7) / 2), sY + 10, "Administration Panel", 2, 150, 0);
		wsprintf(G_cTxt, "Hi %s", m_cPlayerName);
		PutAlignedString(sX + 30, sX + szX - 30, sY + 30, G_cTxt, 255, 255, 255);
		PutAlignedString(sX + 32, sX + szX - 30, sY + 30, G_cTxt, 255, 255, 255);

		PutString_SprFont3(sX + 57, sY + 50, "Map List (Teleport)", 200, 250, 2);
		iNext += 2;
		if ((msX >= sX + 20) && (msX <= sX + 80) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			PutString2(sX + 25, sY + iNext * 17 + 45, "Aresden", 255, 255, 255);
		}
		else PutString2(sX + 25, sY + iNext * 17 + 45, "Aresden", 255, 255, 100);

		if ((msX >= sX + 100) && (msX <= sX + 160) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			PutString2(sX + 110, sY + iNext * 17 + 45, "Elvine", 255, 255, 255);
		}
		else PutString2(sX + 110, sY + iNext * 17 + 45, "Elvine", 255, 255, 100);

		if ((msX >= sX + 180) && (msX <= sX + 230) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			PutString2(sX + 190, sY + iNext * 17 + 45, "Arena 1", 255, 255, 255);
		}
		else PutString2(sX + 190, sY + iNext * 17 + 45, "Arena 1", 255, 255, 100);
		//---------------------------------------------------------------------------------------------------------
		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 80) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			PutString2(sX + 25, sY + iNext * 17 + 45, "Ares Shop", 255, 255, 255);
		}
		else PutString2(sX + 25, sY + iNext * 17 + 45, "Ares Shop", 255, 255, 100);

		if ((msX >= sX + 100) && (msX <= sX + 160) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			PutString2(sX + 110, sY + iNext * 17 + 45, "Elv Shop", 255, 255, 255);
		}
		else PutString2(sX + 110, sY + iNext * 17 + 45, "Elv Shop", 255, 255, 100);

		if ((msX >= sX + 180) && (msX <= sX + 230) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			PutString2(sX + 190, sY + iNext * 17 + 45, "Arena 2", 255, 255, 255);
		}
		else PutString2(sX + 190, sY + iNext * 17 + 45, "Arena 2", 255, 255, 100);
		//---------------------------------------------------------------------------------------------------------
		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 80) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			PutString2(sX + 25, sY + iNext * 17 + 45, "Ares CH", 255, 255, 255);
		}
		else PutString2(sX + 25, sY + iNext * 17 + 45, "Ares CH", 255, 255, 100);

		if ((msX >= sX + 100) && (msX <= sX + 160) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			PutString2(sX + 110, sY + iNext * 17 + 45, "Elv CH", 255, 255, 255);
		}
		else PutString2(sX + 110, sY + iNext * 17 + 45, "Elv CH", 255, 255, 100);

		if ((msX >= sX + 180) && (msX <= sX + 230) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			PutString2(sX + 190, sY + iNext * 17 + 45, "Arena 3", 255, 255, 255);
		}
		else PutString2(sX + 190, sY + iNext * 17 + 45, "Arena 3", 255, 255, 100);
		//---------------------------------------------------------------------------------------------------------
		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 80) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			PutString2(sX + 25, sY + iNext * 17 + 45, "Ares WH", 255, 255, 255);
		}
		else PutString2(sX + 25, sY + iNext * 17 + 45, "Ares WH", 255, 255, 100);

		if ((msX >= sX + 100) && (msX <= sX + 160) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			PutString2(sX + 110, sY + iNext * 17 + 45, "Elv WH", 255, 255, 255);
		}
		else PutString2(sX + 110, sY + iNext * 17 + 45, "Elv WH", 255, 255, 100);

		if ((msX >= sX + 180) && (msX <= sX + 230) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			PutString2(sX + 190, sY + iNext * 17 + 45, "Arena 4", 255, 255, 255);
		}
		else PutString2(sX + 190, sY + iNext * 17 + 45, "Arena 4", 255, 255, 100);
		//---------------------------------------------------------------------------------------------------------
		iNext += 2;
		if ((msX >= sX + 20) && (msX <= sX + 80) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			PutString2(sX + 25, sY + iNext * 17 + 45, "Beginner Zone", 255, 255, 255);
		}
		else PutString2(sX + 25, sY + iNext * 17 + 45, "Beginner Zone", 255, 255, 100);

		if ((msX >= sX + 100) && (msX <= sX + 160) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			PutString2(sX + 110, sY + iNext * 17 + 45, "Ice Bound", 255, 255, 255);
		}
		else PutString2(sX + 110, sY + iNext * 17 + 45, "Ice Bound", 255, 255, 100);

		if ((msX >= sX + 180) && (msX <= sX + 230) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			PutString2(sX + 190, sY + iNext * 17 + 45, "Arena 5", 255, 255, 255);
		}
		else PutString2(sX + 190, sY + iNext * 17 + 45, "Arena 5", 255, 255, 100);
		//======================================================================================================================
		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 80) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			PutString2(sX + 25, sY + iNext * 17 + 45, "Middleland", 255, 255, 255);
		}
		else PutString2(sX + 25, sY + iNext * 17 + 45, "Middleland", 255, 255, 100);

		if ((msX >= sX + 100) && (msX <= sX + 160) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			PutString2(sX + 110, sY + iNext * 17 + 45, "Druncn City", 255, 255, 255);
		}
		else PutString2(sX + 110, sY + iNext * 17 + 45, "Druncn City", 255, 255, 100);

		if ((msX >= sX + 180) && (msX <= sX + 230) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			PutString2(sX + 190, sY + iNext * 17 + 45, "Arena 6", 255, 255, 255);
		}
		else PutString2(sX + 190, sY + iNext * 17 + 45, "Arena 6", 255, 255, 100);
		//===================================================================================================

		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 80) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			PutString2(sX + 25, sY + iNext * 17 + 45, "Promiseland", 255, 255, 255);
		}
		else PutString2(sX + 25, sY + iNext * 17 + 45, "Promiseland", 255, 255, 100);

		if ((msX >= sX + 100) && (msX <= sX + 160) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			PutString2(sX + 110, sY + iNext * 17 + 45, "Internia A", 255, 255, 255);
		}
		else PutString2(sX + 110, sY + iNext * 17 + 45, "Internia A", 255, 255, 100);

		if ((msX >= sX + 180) && (msX <= sX + 230) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			PutString2(sX + 190, sY + iNext * 17 + 45, "Arena 7", 255, 255, 255);
		}
		else PutString2(sX + 190, sY + iNext * 17 + 45, "Arena 7", 255, 255, 100);
//==============================================================================================================================
		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 80) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			PutString2(sX + 25, sY + iNext * 17 + 45, "BtField", 255, 255, 255);
		}
		else PutString2(sX + 25, sY + iNext * 17 + 45, "BtField", 255, 255, 100);

		if ((msX >= sX + 100) && (msX <= sX + 160) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			PutString2(sX + 110, sY + iNext * 17 + 45, "Infernia B", 255, 255, 255);
		}
		else PutString2(sX + 110, sY + iNext * 17 + 45, "Infernia B", 255, 255, 100);

		if ((msX >= sX + 180) && (msX <= sX + 230) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			PutString2(sX + 190, sY + iNext * 17 + 45, "Team Arena", 255, 255, 255);
		}
		else PutString2(sX + 190, sY + iNext * 17 + 45, "Team Arena", 255, 255, 100);
		//======================================================================================================================
		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 80) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			PutString2(sX + 25, sY + iNext * 17 + 45, "GodH", 255, 255, 255);
		}
		else PutString2(sX + 25, sY + iNext * 17 + 45, "GodH", 255, 255, 100);

		if ((msX >= sX + 100) && (msX <= sX + 160) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			PutString2(sX + 110, sY + iNext * 17 + 45, "Maze", 255, 255, 255);
		}
		else PutString2(sX + 110, sY + iNext * 17 + 45, "Maze", 255, 255, 100);

		if ((msX >= sX + 180) && (msX <= sX + 230) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			PutString2(sX + 190, sY + iNext * 17 + 45, "Bisle", 255, 255, 255);
		}
		else PutString2(sX + 190, sY + iNext * 17 + 45, "Bisle", 255, 255, 100);
		//===================================================================================================

		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 80) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			PutString2(sX + 25, sY + iNext * 17 + 45, "HRampart", 255, 255, 255);
		}
		else PutString2(sX + 25, sY + iNext * 17 + 45, "HRampart", 255, 255, 100);

		if ((msX >= sX + 100) && (msX <= sX + 160) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			PutString2(sX + 110, sY + iNext * 17 + 45, "Procella", 255, 255, 255);
		}
		else PutString2(sX + 110, sY + iNext * 17 + 45, "Procella", 255, 255, 100);

		if ((msX >= sX + 180) && (msX <= sX + 230) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			PutString2(sX + 190, sY + iNext * 17 + 45, "ML Mine", 255, 255, 255);
		}
		else PutString2(sX + 190, sY + iNext * 17 + 45, "ML Mine", 255, 255, 100);

		//--------------------------------------
		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 80) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			PutString2(sX + 25, sY + iNext * 17 + 45, "Barracks", 255, 255, 255);
		}
		else PutString2(sX + 25, sY + iNext * 17 + 45, "Barracks", 255, 255, 100);

		if ((msX >= sX + 100) && (msX <= sX + 160) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			PutString2(sX + 110, sY + iNext * 17 + 45, "Abaddon", 255, 255, 255);
		}
		else PutString2(sX + 110, sY + iNext * 17 + 45, "Abaddon", 255, 255, 100);

		if ((msX >= sX + 180) && (msX <= sX + 230) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			PutString2(sX + 190, sY + iNext * 17 + 45, "ML Dungeons", 255, 255, 255);
		}
		else PutString2(sX + 190, sY + iNext * 17 + 45, "ML Dungeons", 255, 255, 100);

		if ((msX > sX + 210) && (msX < sX + 240) && (msY > sY + 315) && (msY < sY + 330))
			PutString_SprFont3(sX + 210, sY + 315, "Back", 200, 250, 2);
		else PutString_SprFont3(sX + 210, sY + 315, "Back", 200, 250, 200);

		break;

	case 26: // Summons Panel
		iaddx = 15;
		iaddy = 75;
		PutString_SprFont3(sX + 75, sY + 20, "Summon List", 200, 250, 2);
		iNext += 1;
		if ((msX >= sX + 10) && (msX <= sX + 80) && (msY >= sY + iNext * 15 + 30) && (msY <= sY + iNext * 15 + 45))
			PutString2(sX + 15, sY + iNext * 15 + 30, NPC_NAME_DUMMY, 255, 255, 255);
		else PutString2(sX + 15, sY + iNext * 15 + 30, NPC_NAME_DUMMY, 255, 255, 100);
		iNext += 1;
		if ((msX >= sX + 10) && (msX <= sX + 80) && (msY >= sY + iNext * 15 + 30) && (msY <= sY + iNext * 15 + 45))
			PutString2(sX + 15, sY + iNext * 15 + 30, NPC_NAME_SLIME, 255, 255, 255);
		else PutString2(sX + 15, sY + iNext * 15 + 30, NPC_NAME_SLIME, 255, 255, 100);
		iNext += 1;
		if ((msX >= sX + 10) && (msX <= sX + 80) && (msY >= sY + iNext * 15 + 30) && (msY <= sY + iNext * 15 + 45))
			PutString2(sX + 15, sY + iNext * 15 + 30, NPC_NAME_GIANTANT, 255, 255, 255);
		else PutString2(sX + 15, sY + iNext * 15 + 30, NPC_NAME_GIANTANT, 255, 255, 100);
		iNext += 1;
		if ((msX >= sX + 10) && (msX <= sX + 80) && (msY >= sY + iNext * 15 + 30) && (msY <= sY + iNext * 15 + 45))
			PutString2(sX + 15, sY + iNext * 15 + 30, NPC_NAME_ORC, 255, 255, 255);
		else PutString2(sX + 15, sY + iNext * 15 + 30, NPC_NAME_ORC, 255, 255, 100);
		iNext += 1;
		if ((msX >= sX + 10) && (msX <= sX + 80) && (msY >= sY + iNext * 15 + 30) && (msY <= sY + iNext * 15 + 45))
			PutString2(sX + 15, sY + iNext * 15 + 30, NPC_NAME_SKELETON, 255, 255, 255);
		else PutString2(sX + 15, sY + iNext * 15 + 30, NPC_NAME_SKELETON, 255, 255, 100);
		iNext += 1;
		if ((msX >= sX + 10) && (msX <= sX + 80) && (msY >= sY + iNext * 15 + 30) && (msY <= sY + iNext * 15 + 45))
			PutString2(sX + 15, sY + iNext * 15 + 30, NPC_NAME_CYCLOPS, 255, 255, 255);
		else PutString2(sX + 15, sY + iNext * 15 + 30, NPC_NAME_CYCLOPS, 255, 255, 100);
		iNext += 1;
		if ((msX >= sX + 10) && (msX <= sX + 80) && (msY >= sY + iNext * 15 + 30) && (msY <= sY + iNext * 15 + 45))
			PutString2(sX + 15, sY + iNext * 15 + 30, NPC_NAME_ZOMBIE, 255, 255, 255);
		else PutString2(sX + 15, sY + iNext * 15 + 30, NPC_NAME_ZOMBIE, 255, 255, 100);
		iNext += 1;
		if ((msX >= sX + 10) && (msX <= sX + 80) && (msY >= sY + iNext * 15 + 30) && (msY <= sY + iNext * 15 + 45))
			PutString2(sX + 15, sY + iNext * 15 + 30, "G. Scorpion", 255, 255, 255);
		else PutString2(sX + 15, sY + iNext * 15 + 30, "G. Scorpion", 255, 255, 100);
		iNext += 1;
		if ((msX >= sX + 10) && (msX <= sX + 80) && (msY >= sY + iNext * 15 + 30) && (msY <= sY + iNext * 15 + 45))
			PutString2(sX + 15, sY + iNext * 15 + 30, NPC_NAME_SNAKE, 255, 255, 255);
		else PutString2(sX + 15, sY + iNext * 15 + 30, NPC_NAME_SNAKE, 255, 255, 100);
		iNext += 1;
		if ((msX >= sX + 10) && (msX <= sX + 80) && (msY >= sY + iNext * 15 + 30) && (msY <= sY + iNext * 15 + 45))
			PutString2(sX + 15, sY + iNext * 15 + 30, NPC_NAME_CLAYGOLEM, 255, 255, 255);
		else PutString2(sX + 15, sY + iNext * 15 + 30, NPC_NAME_CLAYGOLEM, 255, 255, 100);
		iNext += 1;
		if ((msX >= sX + 10) && (msX <= sX + 80) && (msY >= sY + iNext * 15 + 30) && (msY <= sY + iNext * 15 + 45))
			PutString2(sX + 15, sY + iNext * 15 + 30, NPC_NAME_STONEGOLEM, 255, 255, 255);
		else PutString2(sX + 15, sY + iNext * 15 + 30, NPC_NAME_STONEGOLEM, 255, 255, 100);
		iNext += 1;
		if ((msX >= sX + 10) && (msX <= sX + 80) && (msY >= sY + iNext * 15 + 30) && (msY <= sY + iNext * 15 + 45))
			PutString2(sX + 15, sY + iNext * 15 + 30, NPC_NAME_HELHOUND, 255, 255, 255);
		else PutString2(sX + 15, sY + iNext * 15 + 30, NPC_NAME_HELHOUND, 255, 255, 100);
		iNext += 1;
		if ((msX >= sX + 10) && (msX <= sX + 80) && (msY >= sY + iNext * 15 + 30) && (msY <= sY + iNext * 15 + 45))
			PutString2(sX + 15, sY + iNext * 15 + 30, NPC_NAME_TROLL, 255, 255, 255);
		else PutString2(sX + 15, sY + iNext * 15 + 30, NPC_NAME_TROLL, 255, 255, 100);
		iNext += 1;
		if ((msX >= sX + 10) && (msX <= sX + 80) && (msY >= sY + iNext * 15 + 30) && (msY <= sY + iNext * 15 + 45))
			PutString2(sX + 15, sY + iNext * 15 + 30, NPC_NAME_OGRE, 255, 255, 255);
		else PutString2(sX + 15, sY + iNext * 15 + 30, NPC_NAME_OGRE, 255, 255, 100);
		iNext += 1;
		if ((msX >= sX + 10) && (msX <= sX + 80) && (msY >= sY + iNext * 15 + 30) && (msY <= sY + iNext * 15 + 45))
			PutString2(sX + 15, sY + iNext * 15 + 30, "Giant Tree", 255, 255, 255);
		else PutString2(sX + 15, sY + iNext * 15 + 30, "Giant Tree", 255, 255, 100);
		iNext += 1;
		if ((msX >= sX + 10) && (msX <= sX + 80) && (msY >= sY + iNext * 15 + 30) && (msY <= sY + iNext * 15 + 45))
			PutString2(sX + 15, sY + iNext * 15 + 30, NPC_NAME_GICRAYF, 255, 255, 255);
		else PutString2(sX + 15, sY + iNext * 15 + 30, NPC_NAME_GICRAYF, 255, 255, 100);
		iNext += 1;
		if ((msX >= sX + 10) && (msX <= sX + 80) && (msY >= sY + iNext * 15 + 30) && (msY <= sY + iNext * 15 + 45))
			PutString2(sX + 15, sY + iNext * 15 + 30, NPC_NAME_TENTOCL, 255, 255, 255);
		else PutString2(sX + 15, sY + iNext * 15 + 30, NPC_NAME_TENTOCL, 255, 255, 100);
		
		//--Second Column--------------------------------------------------------------------------------------------------
		iNextB += 1;
		if ((msX >= sX + 100) && (msX <= sX + 160) && (msY >= sY + iNextB * 15 + 30) && (msY <= sY + iNextB * 15 + 45))
			PutString2(sX + 110, sY + iNextB * 15 + 30, NPC_NAME_LICHE, 255, 255, 255);
		else PutString2(sX + 110, sY + iNextB * 15 + 30, NPC_NAME_LICHE, 255, 255, 100);
		iNextB += 1;
		if ((msX >= sX + 100) && (msX <= sX + 160) && (msY >= sY + iNextB * 15 + 30) && (msY <= sY + iNextB * 15 + 45))
			PutString2(sX + 110, sY + iNextB * 15 + 30, NPC_NAME_WEREWOLF, 255, 255, 255);
		else PutString2(sX + 110, sY + iNextB * 15 + 30, NPC_NAME_WEREWOLF, 255, 255, 100);
		
		iNextB += 1;
		if ((msX >= sX + 100) && (msX <= sX + 160) && (msY >= sY + iNextB * 15 + 30) && (msY <= sY + iNextB * 15 + 45))
			PutString2(sX + 110, sY + iNextB * 15 + 30, NPC_NAME_STALKER, 255, 255, 255);
		else PutString2(sX + 110, sY + iNextB * 15 + 30, NPC_NAME_STALKER, 255, 255, 100);
		iNextB += 1;
		if ((msX >= sX + 100) && (msX <= sX + 160) && (msY >= sY + iNextB * 15 + 30) && (msY <= sY + iNextB * 15 + 45))
			PutString2(sX + 110, sY + iNextB * 15 + 30, NPC_NAME_DARKELF, 255, 255, 255);
		else PutString2(sX + 110, sY + iNextB * 15 + 30, NPC_NAME_DARKELF, 255, 255, 100);
		iNextB += 1;
		if ((msX >= sX + 100) && (msX <= sX + 160) && (msY >= sY + iNextB * 15 + 30) && (msY <= sY + iNextB * 15 + 45))
			PutString2(sX + 110, sY + iNextB * 15 + 30, NPC_NAME_FROG, 255, 255, 255);
		else PutString2(sX + 110, sY + iNextB * 15 + 30, NPC_NAME_FROG, 255, 255, 100);
		iNextB += 1;
		if ((msX >= sX + 100) && (msX <= sX + 160) && (msY >= sY + iNextB * 15 + 30) && (msY <= sY + iNextB * 15 + 45))
			PutString2(sX + 110, sY + iNextB * 15 + 30, "Mountain G.", 255, 255, 255);
		else PutString2(sX + 110, sY + iNextB * 15 + 30, "Mountain G.", 255, 255, 100);
		iNextB += 1;
		if ((msX >= sX + 100) && (msX <= sX + 160) && (msY >= sY + iNextB * 15 + 30) && (msY <= sY + iNextB * 15 + 45))
			PutString2(sX + 110, sY + iNextB * 15 + 30, NPC_NAME_ETTIN, 255, 255, 255);
		else PutString2(sX + 110, sY + iNextB * 15 + 30, NPC_NAME_ETTIN, 255, 255, 100);
		iNextB += 1;
		if ((msX >= sX + 100) && (msX <= sX + 160) && (msY >= sY + iNextB * 15 + 30) && (msY <= sY + iNextB * 15 + 45))
			PutString2(sX + 110, sY + iNextB * 15 + 30, NPC_NAME_CANNIBAL, 255, 255, 255);
		else PutString2(sX + 110, sY + iNextB * 15 + 30, NPC_NAME_CANNIBAL, 255, 255, 100);
		iNextB += 1;
		if ((msX >= sX + 100) && (msX <= sX + 160) && (msY >= sY + iNextB * 15 + 30) && (msY <= sY + iNextB * 15 + 45))
			PutString2(sX + 110, sY + iNextB * 15 + 30, NPC_NAME_RUDOLPH, 255, 255, 255);
		else PutString2(sX + 110, sY + iNextB * 15 + 30, NPC_NAME_RUDOLPH, 255, 255, 100);
		iNextB += 1;
		if ((msX >= sX + 100) && (msX <= sX + 160) && (msY >= sY + iNextB * 15 + 30) && (msY <= sY + iNextB * 15 + 45))
			PutString2(sX + 110, sY + iNextB * 15 + 30, NPC_NAME_BEHOLDER, 255, 255, 255);
		else PutString2(sX + 110, sY + iNextB * 15 + 30, NPC_NAME_BEHOLDER, 255, 255, 100);
		iNextB += 1;
		if ((msX >= sX + 100) && (msX <= sX + 160) && (msY >= sY + iNextB * 15 + 30) && (msY <= sY + iNextB * 15 + 45))
			PutString2(sX + 110, sY + iNextB * 15 + 30, NPC_NAME_DIREBOAR, 255, 255, 255);
		else PutString2(sX + 110, sY + iNextB * 15 + 30, NPC_NAME_DIREBOAR, 255, 255, 100);
		iNextB += 1;
		if ((msX >= sX + 100) && (msX <= sX + 160) && (msY >= sY + iNextB * 15 + 30) && (msY <= sY + iNextB * 15 + 45))
			PutString2(sX + 110, sY + iNextB * 15 + 30, NPC_NAME_ICEGOLEM, 255, 255, 255);
		else PutString2(sX + 110, sY + iNextB * 15 + 30, NPC_NAME_ICEGOLEM, 255, 255, 100);
		iNextB += 1;
		if ((msX >= sX + 100) && (msX <= sX + 160) && (msY >= sY + iNextB * 15 + 30) && (msY <= sY + iNextB * 15 + 45))
			PutString2(sX + 110, sY + iNextB * 15 + 30, NPC_NAME_FROST, 255, 255, 255);
		else PutString2(sX + 110, sY + iNextB * 15 + 30, NPC_NAME_FROST, 255, 255, 100);
		iNextB += 1;
		if ((msX >= sX + 100) && (msX <= sX + 160) && (msY >= sY + iNextB * 15 + 30) && (msY <= sY + iNextB * 15 + 45))
			PutString2(sX + 110, sY + iNextB * 15 + 30, NPC_NAME_NIZIE, 255, 255, 255);
		else PutString2(sX + 110, sY + iNextB * 15 + 30, NPC_NAME_NIZIE, 255, 255, 100);
		iNextB += 1;
		if ((msX >= sX + 100) && (msX <= sX + 160) && (msY >= sY + iNextB * 15 + 30) && (msY <= sY + iNextB * 15 + 45))
			PutString2(sX + 110, sY + iNextB * 15 + 30, NPC_NAME_CLAWTUR, 255, 255, 255);
		else PutString2(sX + 110, sY + iNextB * 15 + 30, NPC_NAME_CLAWTUR, 255, 255, 100);
		iNextB += 1;
		if ((msX >= sX + 100) && (msX <= sX + 160) && (msY >= sY + iNextB * 15 + 30) && (msY <= sY + iNextB * 15 + 45))
			PutString2(sX + 110, sY + iNextB * 15 + 30, NPC_NAME_GILIZAR, 255, 255, 255);
		else PutString2(sX + 110, sY + iNextB * 15 + 30, NPC_NAME_GILIZAR, 255, 255, 100);

		//--3� Column---------------------------------------------------------------------------------------------------------
		iNextC += 1;
		if ((msX >= sX + 180) && (msX <= sX + 230) && (msY >= sY + iNextC * 15 + 30) && (msY <= sY + iNextC * 15 + 45))
			PutString2(sX + 190, sY + iNextC * 15 + 30, "M. Mage Orc", 255, 255, 255);
		else PutString2(sX + 190, sY + iNextC * 15 + 30, "M. Mage Orc", 255, 255, 100);
		iNextC += 1;
		if ((msX >= sX + 180) && (msX <= sX + 230) && (msY >= sY + iNextC * 15 + 30) && (msY <= sY + iNextC * 15 + 45))
			PutString2(sX + 190, sY + iNextC * 15 + 30, NPC_NAME_MINAUS, 255, 255, 255);
		else PutString2(sX + 190, sY + iNextC * 15 + 30, NPC_NAME_MINAUS, 255, 255, 100);
		iNextC += 1;
		if ((msX >= sX + 180) && (msX <= sX + 230) && (msY >= sY + iNextC * 15 + 30) && (msY <= sY + iNextC * 15 + 45))
			PutString2(sX + 190, sY + iNextC * 15 + 30, NPC_NAME_DRAGON, 255, 255, 255);
		else PutString2(sX + 190, sY + iNextC * 15 + 30, NPC_NAME_DRAGON, 255, 255, 100);
		iNextC += 1;
		if ((msX >= sX + 180) && (msX <= sX + 230) && (msY >= sY + iNextC * 15 + 30) && (msY <= sY + iNextC * 15 + 45))
			PutString2(sX + 190, sY + iNextC * 15 + 30, NPC_NAME_CENTAUR, 255, 255, 255);
		else PutString2(sX + 190, sY + iNextC * 15 + 30, NPC_NAME_CENTAUR, 255, 255, 100);
		iNextC += 1;
		if ((msX >= sX + 180) && (msX <= sX + 230) && (msY >= sY + iNextC * 15 + 30) && (msY <= sY + iNextC * 15 + 45))
			PutString2(sX + 190, sY + iNextC * 15 + 30, NPC_NAME_DEMON, 255, 255, 255);
		else PutString2(sX + 190, sY + iNextC * 15 + 30, NPC_NAME_DEMON, 255, 255, 100);
		iNextC += 1;
		if ((msX >= sX + 180) && (msX <= sX + 230) && (msY >= sY + iNextC * 15 + 30) && (msY <= sY + iNextC * 15 + 45))
			PutString2(sX + 190, sY + iNextC * 15 + 30, NPC_NAME_UNICORN, 255, 255, 255);
		else PutString2(sX + 190, sY + iNextC * 15 + 30, NPC_NAME_UNICORN, 255, 255, 100);
		iNextC += 1;
		if ((msX >= sX + 180) && (msX <= sX + 230) && (msY >= sY + iNextC * 15 + 30) && (msY <= sY + iNextC * 15 + 45))
			PutString2(sX + 190, sY + iNextC * 15 + 30, NPC_NAME_GARGOYLE, 255, 255, 255);
		else PutString2(sX + 190, sY + iNextC * 15 + 30, NPC_NAME_GARGOYLE, 255, 255, 100);
		iNextC += 1;
		if ((msX >= sX + 180) && (msX <= sX + 230) && (msY >= sY + iNextC * 15 + 30) && (msY <= sY + iNextC * 15 + 45))
			PutString2(sX + 190, sY + iNextC * 15 + 30, NPC_NAME_HELLCLAW, 255, 255, 255);
		else PutString2(sX + 190, sY + iNextC * 15 + 30, NPC_NAME_HELLCLAW, 255, 255, 100);
		iNextC += 1;
		if ((msX >= sX + 180) && (msX <= sX + 230) && (msY >= sY + iNextC * 15 + 30) && (msY <= sY + iNextC * 15 + 45))
			PutString2(sX + 190, sY + iNextC * 15 + 30, NPC_NAME_TIGERWORM, 255, 255, 255);
		else PutString2(sX + 190, sY + iNextC * 15 + 30, NPC_NAME_TIGERWORM, 255, 255, 100);
		iNextC += 1;
		if ((msX >= sX + 180) && (msX <= sX + 230) && (msY >= sY + iNextC * 15 + 30) && (msY <= sY + iNextC * 15 + 45))
			PutString2(sX + 190, sY + iNextC * 15 + 30, NPC_NAME_WYVERN, 255, 255, 255);
		else PutString2(sX + 190, sY + iNextC * 15 + 30, NPC_NAME_WYVERN, 255, 255, 100);
		iNextC += 1;
		if ((msX >= sX + 180) && (msX <= sX + 230) && (msY >= sY + iNextC * 15 + 30) && (msY <= sY + iNextC * 15 + 45))
			PutString2(sX + 190, sY + iNextC * 15 + 30, NPC_NAME_FIREWYV, 255, 255, 255);
		else PutString2(sX + 190, sY + iNextC * 15 + 30, NPC_NAME_FIREWYV, 255, 255, 100);
		iNextC += 1;
		if ((msX >= sX + 180) && (msX <= sX + 230) && (msY >= sY + iNextC * 15 + 30) && (msY <= sY + iNextC * 15 + 45))
			PutString2(sX + 190, sY + iNextC * 15 + 30, NPC_NAME_ABADDON, 255, 255, 255);
		else PutString2(sX + 190, sY + iNextC * 15 + 30, NPC_NAME_ABADDON, 255, 255, 100);

/*	
ase 80: strcpy(pName, NPC_NAME_TENTOCL); break;
*/

		m_pSprite[DEF_SPRID_INTERFACE_ND_GAME6]->PutSpriteFast(sX + 156 + iaddx, sY + 219 + iaddy, 19, dwTime);
		m_pSprite[DEF_SPRID_INTERFACE_ND_GAME6]->PutSpriteFast(sX + 170 + iaddx, sY + 219 + iaddy, 19, dwTime);
		PutString(sX + 123 - 30 + iaddx, sY + 237 - 10 + iaddy, DRAW_DIALOGBOX_SHOP27, RGB(255, 255, 255)); // "Quantity:"
		PutString(sX + 124 - 30 + iaddx, sY + 237 - 10 + iaddy, DRAW_DIALOGBOX_SHOP27, RGB(255, 255, 255));

		if (iGetTopDialogBoxIndex() == 56 && msZ != 0)
		{
			m_stDialogBoxInfo[56].sV3 = m_stDialogBoxInfo[56].sV3 + msZ / 60;
			m_DInput.m_sZ = 0;
		}

		if (m_stDialogBoxInfo[56].sV3 > 50) m_stDialogBoxInfo[57].sV3 = 50;
		if (m_stDialogBoxInfo[56].sV3 < 0) m_stDialogBoxInfo[56].sV3 = 0;

		if (m_stDialogBoxInfo[56].sV3 >= 10)
		{
			ZeroMemory(cTemp, sizeof(cTemp));
			_itoa(m_stDialogBoxInfo[56].sV3, cTemp, 10);
			cTemp[1] = 0;
			PutString(sX - 35 + 186 + iaddx, sY - 10 + 237 + iaddy, cTemp, RGB(255, 255, 100));
			PutString(sX - 35 + 187 + iaddx, sY - 10 + 237 + iaddy, cTemp, RGB(255, 255, 100));
			ZeroMemory(cTemp, sizeof(cTemp));
			_itoa(m_stDialogBoxInfo[56].sV3, cTemp, 10);
			PutString(sX - 35 + 200 + iaddx, sY - 10 + 237 + iaddy, (cTemp + 1), RGB(255, 255, 100));
			PutString(sX - 35 + 201 + iaddx, sY - 10 + 237 + iaddy, (cTemp + 1), RGB(255, 255, 100));
		}
		else
		{
			PutString(sX - 35 + 186 + iaddx, sY - 10 + 237 + iaddy, "0", RGB(255, 255, 100));
			PutString(sX - 35 + 187 + iaddx, sY - 10 + 237 + iaddy, "0", RGB(255, 255, 100));
			ZeroMemory(cTemp, sizeof(cTemp));
			_itoa(m_stDialogBoxInfo[56].sV3, cTemp, 10);
			PutString(sX - 35 + 200 + iaddx, sY - 10 + 237 + iaddy, (cTemp), RGB(255, 255, 100));
			PutString(sX - 35 + 201 + iaddx, sY - 10 + 237 + iaddy, (cTemp), RGB(255, 255, 100));
		}
		m_pSprite[DEF_SPRID_INTERFACE_ND_GAME6]->PutSpriteFast(sX + 156 + iaddx, sY + 246 + iaddy, 20, dwTime);
		m_pSprite[DEF_SPRID_INTERFACE_ND_GAME6]->PutSpriteFast(sX + 170 + iaddx, sY + 246 + iaddy, 20, dwTime);

		if ((msX > sX + 210) && (msX < sX + 240) && (msY > sY + 315) && (msY < sY + 330))
			PutString_SprFont3(sX + 210, sY + 315, "Back", 200, 250, 2);
		else PutString_SprFont3(sX + 210, sY + 315, "Back", 200, 250, 200);

		break;

	case 40: // GM Online List
		PutString_SprFont3(sX + 65, sY + 10, "Online List for GM's", 2, 150, 0);
		PutAlignedString(sX + 30, sX + szX - 30, sY + 30, "Select a player to manipulate this character.", 255, 255, 255);

		iTotalLines = 0;
		for (i = 0; i < DEF_MAXCLIENTS; i++)
			if (m_pOnlineUsersList[i] != 0) iTotalLines++;
		if (iTotalLines > 17) {
			d1 = (float)m_stDialogBoxInfo[56].sView;
			d2 = (float)(iTotalLines - 17);
			d3 = (274.0f * d1) / d2;
			iPointerLoc = (int)(d3);
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME6, sX, sY, 3);
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME6, sX + 242, sY + iPointerLoc + 35, 7);
		}
		else iPointerLoc = 0;

		if (cLB != 0 && iTotalLines > 17)
		{
			if ((iGetTopDialogBoxIndex() == 56))
			{
				if ((msX >= sX + 235) && (msX <= sX + 260) && (msY >= sY + 10) && (msY <= sY + 330))
				{
					d1 = (float)(msY - (sY + 35));
					d2 = (float)(iTotalLines - 17);
					d3 = (d1 * d2) / 274.0f;
					m_stDialogBoxInfo[56].sView = (int)(d3 + 0.5f);
				}
			}
		}
		else m_stDialogBoxInfo[56].bIsScrollSelected = false;
		if (iGetTopDialogBoxIndex() == 56 && msZ != 0)
		{
			m_stDialogBoxInfo[56].sView = m_stDialogBoxInfo[56].sView - msZ / 60;
			m_DInput.m_sZ = 0;
		}
		if (iTotalLines > 17 && m_stDialogBoxInfo[56].sView > iTotalLines - 17) m_stDialogBoxInfo[56].sView = iTotalLines - 17;
		if (m_stDialogBoxInfo[56].sView < 0 || iTotalLines < 17) m_stDialogBoxInfo[56].sView = 0;

		for (i = 0; i < 17; i++)
			if (((i + m_stDialogBoxInfo[56].sView) < DEF_MAXCLIENTS) && (m_pOnlineUsersList[i + m_stDialogBoxInfo[56].sView] != 0))
			{
				ZeroMemory(cTemp, sizeof(cTemp));
				wsprintf(cTemp, m_pOnlineUsersList[i + m_stDialogBoxInfo[56].sView]->m_cName, cStr2, cStr3);

				ZeroMemory(cTemp2, sizeof(cTemp2));
				wsprintf(cTemp2, m_pOnlineUsersList[i + m_stDialogBoxInfo[56].sView]->m_cGuildName, cStr2, cStr3);
				m_Misc.ReplaceString(cTemp2, '_', ' ');

				if (memcmp(cTemp2, m_cGuildName, sizeof(m_cGuildName)) == 0)
				{
					iR = 0;	iG = 255; iB = 0;
				}
				else
				{
					iR = 255;	iG = 255; iB = 255;
				}


				if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + i * 18 + 65) && (msY <= sY + i * 18 + 79))
				{
					PutString2(sX + 25, sY + i * 18 + 65, cTemp, iR, iG, iB);
				}
				else
				{
					PutString2(sX + 25, sY + i * 18 + 65, cTemp, iR / 2, iG / 2, iB / 2);
				}
			}

		for (i = 0; i < 17; i++)
			if (((i + m_stDialogBoxInfo[56].sView) < DEF_MAXCLIENTS) && (m_pOnlineUsersList[i + m_stDialogBoxInfo[56].sView] != 0))
			{
				ZeroMemory(cTemp, sizeof(cTemp));
				wsprintf(cTemp, m_pOnlineUsersList[i + m_stDialogBoxInfo[56].sView]->m_cGuildName, cStr2, cStr3);
				m_Misc.ReplaceString(cTemp, '_', ' ');

				ZeroMemory(cTemp2, sizeof(cTemp2));
				wsprintf(cTemp2, m_pOnlineUsersList[i + m_stDialogBoxInfo[56].sView]->m_cName, cStr2, cStr3);

				// display blank space rather than NONE if char has no guild
				if (memcmp(cTemp, "NONE", 4) == 0)
				{
					memcpy(cTemp, "    ", 4);
				}

				if (memcmp(cTemp, m_cGuildName, sizeof(m_cGuildName)) == 0)
				{
					iR = 0;	iG = 255; iB = 0;
				}
				else
				{
					iR = 255;	iG = 255; iB = 255;
				}

				if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + i * 18 + 65) && (msY <= sY + i * 18 + 79))
				{
					PutString2(sX + 125, sY + i * 18 + 65, cTemp, iR, iG, iB);
				}
				else
				{
					PutString2(sX + 125, sY + i * 18 + 65, cTemp, iR / 2, iG / 2, iB / 2);
				}
			}

		if ((m_dwCurTime - m_dwReqUsersTime) > 30000)
		{
			if ((msX > sX + 180) && (msX < sX + 180 + 40) && (msY > sY + 45 + 14 * 18) && (msY < sY + 45 + 15 * 18)) {
				PutString2(sX + 180, sY + 45 + (14 * 18), "Refresh", 255, 255, 255);
			}
			else {
				PutString2(sX + 180, sY + 45 + (14 * 18), "Refresh", 255, 255, 100);
			}
		}
		else
		{
			PutString2(sX + 180, sY + 45 + (14 * 18), "Refresh", 65, 65, 65);
		}
		break;

	case 42: // GM Online List
		PutString_SprFont3(sX + 65, sY + 10, "Online List for GM's", 2, 150, 0);
		PutAlignedString(sX + 30, sX + szX - 30, sY + 30, "Select a player to teleport.", 255, 255, 255);

		iTotalLines = 0;
		for (i = 0; i < DEF_MAXCLIENTS; i++)
			if (m_pOnlineUsersList[i] != 0) iTotalLines++;
		if (iTotalLines > 17) {
			d1 = (float)m_stDialogBoxInfo[56].sView;
			d2 = (float)(iTotalLines - 17);
			d3 = (274.0f * d1) / d2;
			iPointerLoc = (int)(d3);
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME6, sX, sY, 3);
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME6, sX + 242, sY + iPointerLoc + 35, 7);
		}
		else iPointerLoc = 0;

		if (cLB != 0 && iTotalLines > 17)
		{
			if ((iGetTopDialogBoxIndex() == 56))
			{
				if ((msX >= sX + 235) && (msX <= sX + 260) && (msY >= sY + 10) && (msY <= sY + 330))
				{
					d1 = (float)(msY - (sY + 35));
					d2 = (float)(iTotalLines - 17);
					d3 = (d1 * d2) / 274.0f;
					m_stDialogBoxInfo[56].sView = (int)(d3 + 0.5f);
				}
			}
		}
		else m_stDialogBoxInfo[56].bIsScrollSelected = false;
		if (iGetTopDialogBoxIndex() == 56 && msZ != 0)
		{
			m_stDialogBoxInfo[56].sView = m_stDialogBoxInfo[56].sView - msZ / 60;
			m_DInput.m_sZ = 0;
		}
		if (iTotalLines > 17 && m_stDialogBoxInfo[56].sView > iTotalLines - 17) m_stDialogBoxInfo[56].sView = iTotalLines - 17;
		if (m_stDialogBoxInfo[56].sView < 0 || iTotalLines < 17) m_stDialogBoxInfo[56].sView = 0;

		for (i = 0; i < 17; i++)
			if (((i + m_stDialogBoxInfo[56].sView) < DEF_MAXCLIENTS) && (m_pOnlineUsersList[i + m_stDialogBoxInfo[56].sView] != 0))
			{
				ZeroMemory(cTemp, sizeof(cTemp));
				wsprintf(cTemp, m_pOnlineUsersList[i + m_stDialogBoxInfo[56].sView]->m_cName, cStr2, cStr3);

				ZeroMemory(cTemp2, sizeof(cTemp2));
				wsprintf(cTemp2, m_pOnlineUsersList[i + m_stDialogBoxInfo[56].sView]->m_cGuildName, cStr2, cStr3);
				m_Misc.ReplaceString(cTemp2, '_', ' ');

				if (memcmp(cTemp2, m_cGuildName, sizeof(m_cGuildName)) == 0)
				{
					iR = 0;	iG = 255; iB = 0;
				}
				else
				{
					iR = 255;	iG = 255; iB = 255;
				}


				if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + i * 18 + 65) && (msY <= sY + i * 18 + 79))
				{
					PutString2(sX + 25, sY + i * 18 + 65, cTemp, iR, iG, iB);
				}
				else
				{
					PutString2(sX + 25, sY + i * 18 + 65, cTemp, iR / 2, iG / 2, iB / 2);
				}
			}

		for (i = 0; i < 17; i++)
			if (((i + m_stDialogBoxInfo[56].sView) < DEF_MAXCLIENTS) && (m_pOnlineUsersList[i + m_stDialogBoxInfo[56].sView] != 0))
			{
				ZeroMemory(cTemp, sizeof(cTemp));
				wsprintf(cTemp, m_pOnlineUsersList[i + m_stDialogBoxInfo[56].sView]->m_cGuildName, cStr2, cStr3);
				m_Misc.ReplaceString(cTemp, '_', ' ');

				ZeroMemory(cTemp2, sizeof(cTemp2));
				wsprintf(cTemp2, m_pOnlineUsersList[i + m_stDialogBoxInfo[56].sView]->m_cName, cStr2, cStr3);

				// display blank space rather than NONE if char has no guild
				if (memcmp(cTemp, "NONE", 4) == 0)
				{
					memcpy(cTemp, "    ", 4);
				}

				if (memcmp(cTemp, m_cGuildName, sizeof(m_cGuildName)) == 0)
				{
					iR = 0;	iG = 255; iB = 0;
				}
				else
				{
					iR = 255;	iG = 255; iB = 255;
				}

				if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + i * 18 + 65) && (msY <= sY + i * 18 + 79))
				{
					PutString2(sX + 125, sY + i * 18 + 65, cTemp, iR, iG, iB);
				}
				else
				{
					PutString2(sX + 125, sY + i * 18 + 65, cTemp, iR / 2, iG / 2, iB / 2);
				}
			}

		if ((m_dwCurTime - m_dwReqUsersTime) > 30000)
		{
			if ((msX > sX + 180) && (msX < sX + 180 + 40) && (msY > sY + 45 + 14 * 18) && (msY < sY + 45 + 15 * 18)) {
				PutString2(sX + 180, sY + 45 + (14 * 18), "Refresh", 255, 255, 255);
			}
			else {
				PutString2(sX + 180, sY + 45 + (14 * 18), "Refresh", 255, 255, 100);
			}
		}
		else
		{
			PutString2(sX + 180, sY + 45 + (14 * 18), "Refresh", 65, 65, 65);
		}
		break;

	case 30:
		PutString_SprFont(sX + 180 + 70, sY + 45, "List of Commands", 7, 0, 0);
		PutString2(sX + 210, sY + 70, "Command", 255, 255, 190);
		PutString2(sX + 350, sY + 70, "Cost", 255, 255, 190);
		{	if ((msX > sX + 180) && (msX < sX + 280) && (msY > sY + 85) && (msY < sY + 100))
			PutString2(sX + 190, sY + 85, "/showframe", 255, 255, 255);
		else PutString2(sX + 190, sY + 85, "/showframe", 150, 150, 150);
		}
		{	if ((msX > sX + 190) && (msX < sX + 280) && (msY > sY + 100) && (msY < sY + 115))
			PutString2(sX + 190, sY + 100, "/showdmg", 255, 255, 255);
		else PutString2(sX + 190, sY + 100, "/showdmg", 150, 150, 150);
		}
		{	if ((msX > sX + 190) && (msX < sX + 280) && (msY > sY + 115) && (msY < sY + 130))
			PutString2(sX + 190, sY + 115, "/showdef", 255, 255, 255);
		else PutString2(sX + 190, sY + 115, "/showdef", 150, 150, 150);
		}
		{	if ((msX > sX + 190) && (msX < sX + 280) && (msY > sY + 130) && (msY < sY + 145))
			PutString2(sX + 190, sY + 130, "/getskills", 255, 255, 255);
		else PutString2(sX + 190, sY + 130, "/getskills", 150, 150, 150);
		}
		{	if ((msX > sX + 190) && (msX < sX + 280) && (msY > sY + 145) && (msY < sY + 160))
			PutString2(sX + 190, sY + 145, "/checkrep", 255, 255, 255);
		else PutString2(sX + 190, sY + 145, "/checkrep", 150, 150, 150);
		}
		{	if ((msX > sX + 190) && (msX < sX + 280) && (msY > sY + 160) && (msY < sY + 175))
			PutString2(sX + 190, sY + 160, "/tgt", 255, 255, 255);
		else PutString2(sX + 190, sY + 160, "/tgt", 150, 150, 150);
		}
		{	if ((msX > sX + 190) && (msX < sX + 280) && (msY > sY + 175) && (msY < sY + 190))
			PutString2(sX + 190, sY + 175, "/raid", 255, 255, 255);
		else PutString2(sX + 190, sY + 175, "/raid", 150, 150, 150);
		}
		{	if ((msX > sX + 190) && (msX < sX + 280) && (msY > sY + 190) && (msY < sY + 205))
			PutString2(sX + 190, sY + 190, "/getcrits", 255, 255, 255);
		else PutString2(sX + 190, sY + 190, "/getcrits", 150, 150, 150);
		}

		if ((msX > sX + 210) && (msX < sX + 240) && (msY > sY + 315) && (msY < sY + 330))
			PutString_SprFont3(sX + 210, sY + 315, "Back", 200, 250, 2);
		else PutString_SprFont3(sX + 210, sY + 315, "Back", 200, 250, 200);

		break;
	}
}


void CGame::DrawDialogBox_Shop2(short msX, short msY, short msZ, char cLB) // MORLA 2.4 - Shop2
{
	short sX, sY;
	UINT32 dwTime = m_dwCurTime;
	int  i, iTemp;
	char cTemp[255], cStr2[255], cStr3[255], cTxt[250], cStr4[250], cStr5[250], cStr6[250];

	int  iCost, iDiscountCost, iDiscountRatio;

	float dTmp1, dTmp2, dTmp3;
	int  iTotalLines, iPointerLoc;
	bool bFlagStatLow = false;
	bool bFlagRedShown = false;
	float d1, d2, d3;

	sX = m_stDialogBoxInfo[57].sX;
	sY = m_stDialogBoxInfo[57].sY;

	DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME6, sX, sY, 2);
	DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_TEXT2, sX, sY, 11);

	switch (m_stDialogBoxInfo[57].cMode) {
	case 0:
		iTotalLines = 0;
		for (i = 0; i < DEF_MAXMENUITEMS; i++)
			if (m_pItemForSaleList[i] != 0) iTotalLines++;
		if (iTotalLines > 13) {
			d1 = (float)m_stDialogBoxInfo[57].sView;
			d2 = (float)(iTotalLines - 13);
			d3 = (274.0f * d1) / d2;
			iPointerLoc = (int)(d3);
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME6, sX, sY, 3);
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME6, sX + 242, sY + iPointerLoc + 35, 7);
		}
		else iPointerLoc = 0;

		if (cLB != 0 && iTotalLines > 13)
		{
			if ((iGetTopDialogBoxIndex() == 57))
			{
				if ((msX >= sX + 235) && (msX <= sX + 260) && (msY >= sY + 10) && (msY <= sY + 330))
				{
					d1 = (float)(msY - (sY + 35));
					d2 = (float)(iTotalLines - 13);
					d3 = (d1 * d2) / 274.0f;
					m_stDialogBoxInfo[57].sView = (int)(d3 + 0.5f);
				}
			}
		}
		else m_stDialogBoxInfo[57].bIsScrollSelected = false;
		if (iGetTopDialogBoxIndex() == 57 && msZ != 0)
		{
			m_stDialogBoxInfo[57].sView = m_stDialogBoxInfo[57].sView - msZ / 60;
			m_DInput.m_sZ = 0;
		}
		if (iTotalLines > 13 && m_stDialogBoxInfo[57].sView > iTotalLines - 13) m_stDialogBoxInfo[57].sView = iTotalLines - 13;
		if (m_stDialogBoxInfo[57].sView < 0 || iTotalLines < 13) m_stDialogBoxInfo[57].sView = 0;
		PutAlignedString(sX + 22, sX + 165, sY + 45, DRAW_DIALOGBOX_SHOP1); // "ITEM"
		PutAlignedString(sX + 23, sX + 166, sY + 45, DRAW_DIALOGBOX_SHOP1);
		PutAlignedString(sX + 153, sX + 250, sY + 45, "POINTS");
		PutAlignedString(sX + 154, sX + 251, sY + 45, "POINTS"); // POINTS

		//Magn0S:: Add to show ammount of points in each market list
		switch (iNewShop) {
		case 1: //Trade Ek
			wsprintf(cTxt, "Total Eks: %d", m_iEnemyKillCount);
			PutAlignedString(sX + 10, sX + 150, sY + 30, cTxt, 255, 255, 255);
			break;
		case 2: //Trade Ek
			wsprintf(cTxt, "Contrib. Points: %d", m_iContribution);
			PutAlignedString(sX + 10, sX + 150, sY + 30, cTxt, 255, 255, 255);
			break;
		case 3: //Trade Ek
			wsprintf(cTxt, "Coin Points: %d", m_iCoinPoints);
			PutAlignedString(sX + 10, sX + 150, sY + 30, cTxt, 255, 255, 255);
			break;
		default: 
			PutAlignedString(sX + 10, sX + 150, sY + 30, "Empty", 255, 255, 255); 
			break;
		}


		for (i = 0; i < 13; i++) {
			if (((i + m_stDialogBoxInfo[57].sView) < DEF_MAXMENUITEMS) && (m_pItemForSaleList[i + m_stDialogBoxInfo[57].sView] != 0))
			{
				/*iDiscountRatio = 0;
				dTmp1 = (float)iDiscountRatio;
				dTmp2 = dTmp1 / 100.0f;
				dTmp1 = (float)m_pItemForSaleList[i + m_stDialogBoxInfo[57].sView]->m_wPrice;
				dTmp3 = dTmp1 * dTmp2;
				iDiscountCost = (int)dTmp3;*/
				iCost = m_pItemForSaleList[i + m_stDialogBoxInfo[57].sView]->m_wPrice;

				/*if (iCost < (m_pItemForSaleList[i + m_stDialogBoxInfo[57].sView]->m_wPrice / 2))
					iCost = (m_pItemForSaleList[i + m_stDialogBoxInfo[57].sView]->m_wPrice / 2) - 1;*/

				ZeroMemory(cTemp, sizeof(cTemp));
				wsprintf(cTemp, "%d", iCost);
				if (iCost != 0) {
					if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + i * 18 + 65) && (msY <= sY + i * 18 + 79))
						PutAlignedString(sX + 148, sX + 260, sY + i * 18 + 65, cTemp, 255, 255, 255);
					else	PutAlignedString(sX + 148, sX + 260, sY + i * 18 + 65, cTemp, 4, 0, 50);
				}

				if (((i + m_stDialogBoxInfo[57].sView) < DEF_MAXMENUITEMS) && (m_pItemForSaleList[i + m_stDialogBoxInfo[57].sView] != 0))
				{
					ZeroMemory(cTemp, sizeof(cTemp));
					GetItemName(m_pItemForSaleList[i + m_stDialogBoxInfo[57].sView], cTemp, cStr2, cStr3, cStr4, cStr5, cStr6);
					m_Misc.ReplaceString(cTemp, '-', ' ');
					if (iCost == 0) {
						PutAlignedString(sX + 10, sX + 260, sY + i * 18 + 65, cTemp, 250, 240, 200);
					}
					else
					{
						if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + i * 18 + 65) && (msY <= sY + i * 18 + 79))
						{
							PutAlignedString(sX + 10, sX + 190, sY + i * 18 + 65, cTemp, 255, 255, 255);
						}
						else {
							PutAlignedString(sX + 10, sX + 190, sY + i * 18 + 65, cTemp, 4, 0, 50);
						}

					}
				}
			}
		}
		break;

	default:
		m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_pItemForSaleList[m_stDialogBoxInfo[57].cMode - 1]->m_sSprite]->PutSpriteFast(sX + 62 + 30 - 35, sY + 84 + 30 - 10,
			m_pItemForSaleList[m_stDialogBoxInfo[57].cMode - 1]->m_sSpriteFrame, dwTime);

		ZeroMemory(cTemp, sizeof(cTemp));
		GetItemName(m_pItemForSaleList[m_stDialogBoxInfo[57].cMode - 1], cTemp, cStr2, cStr3, cStr4, cStr5, cStr6);

		PutAlignedString(sX + 25, sX + 240, sY + 50, cTemp, 255, 255, 255);
		PutAlignedString(sX + 26, sX + 241, sY + 50, cTemp, 255, 255, 255);

		strcpy(cTemp, "POINTS"); //"POINTS"
		PutString(sX + 90, sY + 78 + 30 - 10, cTemp, RGB(40, 10, 10));
		PutString(sX + 91, sY + 78 + 30 - 10, cTemp, RGB(40, 10, 10));
		strcpy(cTemp, DRAW_DIALOGBOX_SHOP6); // "Weight"
		PutString(sX + 90, sY + 93 + 30 - 10, cTemp, RGB(40, 10, 10));
		PutString(sX + 91, sY + 93 + 30 - 10, cTemp, RGB(40, 10, 10));

		/*iDiscountRatio = 0;
		dTmp1 = (float)iDiscountRatio;
		dTmp2 = dTmp1 / 100.0f;
		dTmp1 = (float)m_pItemForSaleList[m_stDialogBoxInfo[57].cMode - 1]->m_wPrice;
		dTmp3 = dTmp1 * dTmp2;
		iDiscountCost = (int)dTmp3;*/
		//iCost = (int)(m_pItemForSaleList[m_stDialogBoxInfo[57].cMode - 1]->m_wPrice * ((100 + m_cDiscount) / 100.0f));
		//iCost = iCost - iDiscountCost;

		iCost = m_pItemForSaleList[m_stDialogBoxInfo[57].cMode - 1]->m_wPrice;

		/*if (iCost < (m_pItemForSaleList[m_stDialogBoxInfo[57].cMode - 1]->m_wPrice / 2))
			iCost = (m_pItemForSaleList[m_stDialogBoxInfo[57].cMode - 1]->m_wPrice / 2) - 1;*/

		wsprintf(cTemp, "%d Points", iCost);
		//": %d Points"
		PutString(sX + 140, sY + 98, cTemp, RGB(45, 25, 25));

		iTemp = m_pItemForSaleList[m_stDialogBoxInfo[57].cMode - 1]->m_wWeight / 100;
		//wsprintf(cTemp, DRAW_DIALOGBOX_SHOP8, iTemp);
		//": %d Stone"
		//PutString(sX + 140, sY + 113, cTemp, RGB(45, 25, 25));

		switch (m_pItemForSaleList[m_stDialogBoxInfo[57].cMode - 1]->m_cEquipPos) {
		case DEF_EQUIPPOS_RHAND:
		case DEF_EQUIPPOS_TWOHAND:
			strcpy(cTemp, DRAW_DIALOGBOX_SHOP9);  // Damage
			PutString(sX + 90, sY + 145, cTemp, RGB(40, 10, 10));
			PutString(sX + 91, sY + 145, cTemp, RGB(40, 10, 10));
			strcpy(cTemp, DRAW_DIALOGBOX_SHOP10); //"Speed(Min.~Max.)"
			PutString(sX + 40, sY + 175, cTemp, RGB(40, 10, 10));
			PutString(sX + 41, sY + 175, cTemp, RGB(40, 10, 10));

			if (m_pItemForSaleList[m_stDialogBoxInfo[57].cMode - 1]->m_sItemEffectValue3 != 0)
			{
				wsprintf(cTemp, ": %dD%d+%d (S-M)", m_pItemForSaleList[m_stDialogBoxInfo[57].cMode - 1]->m_sItemEffectValue1,
					m_pItemForSaleList[m_stDialogBoxInfo[57].cMode - 1]->m_sItemEffectValue2,
					m_pItemForSaleList[m_stDialogBoxInfo[57].cMode - 1]->m_sItemEffectValue3);
			}
			else
			{
				wsprintf(cTemp, ": %dD%d (S-M)", m_pItemForSaleList[m_stDialogBoxInfo[57].cMode - 1]->m_sItemEffectValue1,
					m_pItemForSaleList[m_stDialogBoxInfo[57].cMode - 1]->m_sItemEffectValue2);
			}
			PutString(sX + 140, sY + 145, cTemp, RGB(45, 25, 25));
			if (m_pItemForSaleList[m_stDialogBoxInfo[57].cMode - 1]->m_sItemEffectValue6 != 0)
			{
				wsprintf(cTemp, ": %dD%d+%d (L)", m_pItemForSaleList[m_stDialogBoxInfo[57].cMode - 1]->m_sItemEffectValue4,
					m_pItemForSaleList[m_stDialogBoxInfo[57].cMode - 1]->m_sItemEffectValue5,
					m_pItemForSaleList[m_stDialogBoxInfo[57].cMode - 1]->m_sItemEffectValue6);
			}
			else
			{
				wsprintf(cTemp, ": %dD%d (L)", m_pItemForSaleList[m_stDialogBoxInfo[57].cMode - 1]->m_sItemEffectValue4,
					m_pItemForSaleList[m_stDialogBoxInfo[57].cMode - 1]->m_sItemEffectValue5);
			}
			PutString(sX + 140, sY + 160, cTemp, RGB(45, 25, 25));
			if (m_pItemForSaleList[m_stDialogBoxInfo[57].cMode - 1]->m_cSpeed == 0)   wsprintf(cTemp, ": 0(10~10)");
			else wsprintf(cTemp, ": %d(%d ~ %d)", m_pItemForSaleList[m_stDialogBoxInfo[57].cMode - 1]->m_cSpeed, iTemp, m_pItemForSaleList[m_stDialogBoxInfo[57].cMode - 1]->m_cSpeed * 13);
			PutString(sX + 140, sY + 175, cTemp, RGB(45, 25, 25));

			//if ((m_pItemForSaleList[m_stDialogBoxInfo[57].cMode - 1]->m_wWeight / 100) > m_iStr)
			//{
			//	wsprintf(cTemp, DRAW_DIALOGBOX_SHOP11, (m_pItemForSaleList[m_stDialogBoxInfo[57].cMode - 1]->m_wWeight / 100));
			//	PutAlignedString(sX + 25, sX + 240, sY + 258, cTemp, 195, 25, 25);
			//	PutAlignedString(sX + 26, sX + 241, sY + 258, cTemp, 195, 25, 25); // *Your STR should be at least %d to use this item."
			//	bFlagRedShown = true;
			//}
			
			break;

		case DEF_EQUIPPOS_LHAND:
			strcpy(cTemp, DRAW_DIALOGBOX_SHOP12); // "Defence"
			PutString(sX + 90, sY + 145, cTemp, RGB(40, 10, 10));
			PutString(sX + 91, sY + 145, cTemp, RGB(40, 10, 10));
			wsprintf(cTemp, ": +%d%", m_pItemForSaleList[m_stDialogBoxInfo[57].cMode - 1]->m_sItemEffectValue1);
			PutString(sX + 140, sY + 145, cTemp, RGB(45, 25, 25));
			//if ((m_pItemForSaleList[m_stDialogBoxInfo[57].cMode - 1]->m_wWeight / 100) > m_iStr)
			//{
			//	wsprintf(cTemp, DRAW_DIALOGBOX_SHOP11, (m_pItemForSaleList[m_stDialogBoxInfo[57].cMode - 1]->m_wWeight / 100));
			//	PutAlignedString(sX + 25, sX + 240, sY + 258, cTemp, 195, 25, 25); // "*Your STR should be at least %d to use this item."
			//	PutAlignedString(sX + 26, sX + 241, sY + 258, cTemp, 195, 25, 25);
			//	bFlagRedShown = true;
			//}
			
			break;

		case DEF_EQUIPPOS_HEAD:
		case DEF_EQUIPPOS_BODY:
		case DEF_EQUIPPOS_BOOTS:
		case DEF_EQUIPPOS_ARMS:
		case DEF_EQUIPPOS_PANTS:
			strcpy(cTemp, DRAW_DIALOGBOX_SHOP12); // "Defence"
			PutString(sX + 90, sY + 145, cTemp, RGB(40, 10, 10));
			PutString(sX + 91, sY + 145, cTemp, RGB(40, 10, 10));
			wsprintf(cTemp, ": +%d%", m_pItemForSaleList[m_stDialogBoxInfo[57].cMode - 1]->m_sItemEffectValue1);
			PutString(sX + 140, sY + 145, cTemp, RGB(45, 25, 25));
			bFlagStatLow = false;
			
			//if ((m_pItemForSaleList[m_stDialogBoxInfo[57].cMode - 1]->m_wWeight / 100) > m_iStr)
			//{
			//	wsprintf(cTemp, DRAW_DIALOGBOX_SHOP11, (m_pItemForSaleList[m_stDialogBoxInfo[57].cMode - 1]->m_wWeight / 100));
			//	PutAlignedString(sX + 25, sX + 240, sY + 258, cTemp, 195, 25, 25);
			//	PutAlignedString(sX + 26, sX + 241, sY + 258, cTemp, 195, 25, 25); // "*Your STR should be at least %d to use this item."
			//	bFlagRedShown = true;
			//}
			if (bFlagStatLow == true) // Means some stat is too low
			{
				//strcpy(cTemp, DRAW_DIALOGBOX_SHOP21); // "(Warning!) Your stat is too low for this item."
				//PutAlignedString(sX + 25, sX + 240, sY + 258, cTemp, 195, 25, 25);
				//PutAlignedString(sX + 26, sX + 241, sY + 258, cTemp, 195, 25, 25);
				//bFlagRedShown = true;
			}
			else if ((strstr(m_pItemForSaleList[m_stDialogBoxInfo[57].cMode - 1]->m_cName, "(M)") != 0)
				&& (m_sPlayerType > 3))
			{
				strcpy(cTemp, DRAW_DIALOGBOX_SHOP22); // "(Warning!) only for male."
				PutAlignedString(sX + 25, sX + 240, sY + 258, cTemp, 195, 25, 25);
				PutAlignedString(sX + 26, sX + 241, sY + 258, cTemp, 195, 25, 25);
				bFlagRedShown = true;
			}
			else if ((strstr(m_pItemForSaleList[m_stDialogBoxInfo[57].cMode - 1]->m_cName, "(W)") != 0)
				&& (m_sPlayerType <= 3))
			{
				strcpy(cTemp, DRAW_DIALOGBOX_SHOP23); // "(Warning!) only for female."
				PutAlignedString(sX + 25, sX + 240, sY + 258, cTemp, 195, 25, 25);
				PutAlignedString(sX + 26, sX + 241, sY + 258, cTemp, 195, 25, 25);
				bFlagRedShown = true;
			}
			break;

		case DEF_EQUIPPOS_NONE:
			break;
		}

		if (m_pItemForSaleList[m_stDialogBoxInfo[57].cMode - 1]->m_sLevelLimit != 0)
		{
			strcpy(cTemp, DRAW_DIALOGBOX_SHOP24); // "Level"
			if (m_iLevel >= m_pItemForSaleList[m_stDialogBoxInfo[57].cMode - 1]->m_sLevelLimit)
			{
				PutString(sX + 90, sY + 190, cTemp, RGB(40, 10, 10));
				PutString(sX + 91, sY + 190, cTemp, RGB(40, 10, 10));
				wsprintf(cTemp, DRAW_DIALOGBOX_SHOP25, m_pItemForSaleList[m_stDialogBoxInfo[57].cMode - 1]->m_sLevelLimit);
				PutString(sX + 140, sY + 190, cTemp, RGB(45, 25, 25));// ": above %d"
			}
			else
			{
				PutString(sX + 90, sY + 190, cTemp, RGB(40, 10, 10));
				PutString(sX + 91, sY + 190, cTemp, RGB(40, 10, 10));
				wsprintf(cTemp, DRAW_DIALOGBOX_SHOP25, m_pItemForSaleList[m_stDialogBoxInfo[57].cMode - 1]->m_sLevelLimit);
				PutString(sX + 140, sY + 190, cTemp, RGB(125, 25, 25));// ": above %d"
				if (bFlagRedShown == false)
				{
					strcpy(cTemp, DRAW_DIALOGBOX_SHOP26); // "(Warning!) Your level is too low for this item."
					PutAlignedString(sX + 25, sX + 240, sY + 258, cTemp, 195, 25, 25);
					PutAlignedString(sX + 25 + 1, sX + 240 + 1, sY + 258, cTemp, 195, 25, 25);
					bFlagRedShown = true;
				}
			}
		}

		m_pSprite[DEF_SPRID_INTERFACE_ND_GAME6]->PutSpriteFast(sX + 156, sY + 219, 19, dwTime);
		m_pSprite[DEF_SPRID_INTERFACE_ND_GAME6]->PutSpriteFast(sX + 170, sY + 219, 19, dwTime);
		PutString(sX + 123 - 35, sY + 237 - 10, DRAW_DIALOGBOX_SHOP27, RGB(40, 10, 10)); // "Quantity:"
		PutString(sX + 124 - 35, sY + 237 - 10, DRAW_DIALOGBOX_SHOP27, RGB(40, 10, 10));
		if (iGetTopDialogBoxIndex() == 57 && msZ != 0)
		{
			m_stDialogBoxInfo[57].sV3 = m_stDialogBoxInfo[57].sV3 + msZ / 60;
			m_DInput.m_sZ = 0;
		}
		if (m_stDialogBoxInfo[57].sV3 > (50 - _iGetTotalItemNum())) m_stDialogBoxInfo[57].sV3 = (50 - _iGetTotalItemNum());
		if (m_stDialogBoxInfo[57].sV3 < 1) m_stDialogBoxInfo[57].sV3 = 1;

		if (m_stDialogBoxInfo[57].sV3 >= 10)
		{
			ZeroMemory(cTemp, sizeof(cTemp));
			_itoa(m_stDialogBoxInfo[57].sV3, cTemp, 10);
			cTemp[1] = 0;
			PutString(sX - 35 + 186, sY - 10 + 237, cTemp, RGB(40, 10, 10));
			PutString(sX - 35 + 187, sY - 10 + 237, cTemp, RGB(40, 10, 10));
			ZeroMemory(cTemp, sizeof(cTemp));
			_itoa(m_stDialogBoxInfo[57].sV3, cTemp, 10);
			PutString(sX - 35 + 200, sY - 10 + 237, (cTemp + 1), RGB(40, 10, 10));
			PutString(sX - 35 + 201, sY - 10 + 237, (cTemp + 1), RGB(40, 10, 10));
		}
		else
		{
			PutString(sX - 35 + 186, sY - 10 + 237, "0", RGB(40, 10, 10));
			PutString(sX - 35 + 187, sY - 10 + 237, "0", RGB(40, 10, 10));
			ZeroMemory(cTemp, sizeof(cTemp));
			_itoa(m_stDialogBoxInfo[57].sV3, cTemp, 10);
			PutString(sX - 35 + 200, sY - 10 + 237, (cTemp), RGB(40, 10, 10));
			PutString(sX - 35 + 201, sY - 10 + 237, (cTemp), RGB(40, 10, 10));
		}
		m_pSprite[DEF_SPRID_INTERFACE_ND_GAME6]->PutSpriteFast(sX + 156, sY + 244, 20, dwTime);
		m_pSprite[DEF_SPRID_INTERFACE_ND_GAME6]->PutSpriteFast(sX + 170, sY + 244, 20, dwTime);

		if ((msX >= sX + DEF_LBTNPOSX) && (msX <= sX + DEF_LBTNPOSX + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 31);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 30);

		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 17);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 16);
		break;
	}
}

void CGame::DlgBoxClick_Shop2(short msX, short msY)
{
	int i;
	short sX, sY;
	char cTemp[21];
	sX = m_stDialogBoxInfo[57].sX;
	sY = m_stDialogBoxInfo[57].sY;

	switch (m_stDialogBoxInfo[57].cMode) {
	case 0:
		for (i = 0; i < 13; i++) {
			if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + i * 18 + 65) && (msY <= sY + i * 18 + 79)) {
				if (_iGetTotalItemNum() >= 50) {
					AddEventList(DLGBOX_CLICK_SHOP1, 10);//"You cannot buy anything because your bag is full."
					return;
				}

				PlaySound('E', 14, 5);
				if (m_pItemForSaleList[m_stDialogBoxInfo[57].sView + i] != 0)
					m_stDialogBoxInfo[57].cMode = m_stDialogBoxInfo[57].sView + i + 1;
				return;
			}
		}
		break;

	default:
		if ((msX >= sX + 145) && (msX <= sX + 162) && (msY >= sY + 209) && (msY <= sY + 230))
		{
			m_stDialogBoxInfo[57].sV3 += 10;
			if (m_stDialogBoxInfo[57].sV3 >= (50 - _iGetTotalItemNum()))
				m_stDialogBoxInfo[57].sV3 = (50 - _iGetTotalItemNum());
		}

		if ((msX >= sX + 145) && (msX <= sX + 162) && (msY >= sY + 234) && (msY <= sY + 251))
		{
			m_stDialogBoxInfo[57].sV3 -= 10;
			if (m_stDialogBoxInfo[57].sV3 <= 1)
				m_stDialogBoxInfo[57].sV3 = 1;
		}

		if ((msX >= sX + 163) && (msX <= sX + 180) && (msY >= sY + 209) && (msY <= sY + 230))
		{
			m_stDialogBoxInfo[57].sV3++;
			if (m_stDialogBoxInfo[57].sV3 >= (50 - _iGetTotalItemNum()))
				m_stDialogBoxInfo[57].sV3 = (50 - _iGetTotalItemNum());
		}

		if ((msX >= sX + 163) && (msX <= sX + 180) && (msY >= sY + 234) && (msY <= sY + 251))
		{
			m_stDialogBoxInfo[57].sV3--;
			if (m_stDialogBoxInfo[57].sV3 <= 1)
				m_stDialogBoxInfo[57].sV3 = 1;
		}

		if ((msX >= sX + 30) && (msX <= sX + 30 + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
		{	// Purchase Click
			if ((50 - _iGetTotalItemNum()) < m_stDialogBoxInfo[57].sV3)
			{
				AddEventList(DLGBOX_CLICK_SHOP1, 10);//"You cannot buy anything because your bag is full."
			}
			else
			{
				ZeroMemory(cTemp, sizeof(cTemp));
				strcpy(cTemp, m_pItemForSaleList[m_stDialogBoxInfo[57].cMode - 1]->m_cName);
				bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_REQ_PURCHASEITEM2, 0, m_stDialogBoxInfo[57].sV3, iNewShop, 0, cTemp);
			}
			m_stDialogBoxInfo[57].cMode = 0;
			m_stDialogBoxInfo[57].sV3 = 1;
			PlaySound('E', 14, 5);
		}

		if ((msX >= sX + 154) && (msX <= sX + 154 + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
		{	// Cancel Click
			m_stDialogBoxInfo[57].cMode = 0;
			m_stDialogBoxInfo[57].sV3 = 1;
			PlaySound('E', 14, 5);
		}
		break;
	}
}


void CGame::NotifyMsg_ItemTrade(char * pData) // MORLA 2.4 - Cuando realiza el trade item x eks o dgs
{
	char  * cp;
	short * sp;
	UINT32 * dwp;
	UINT16  * wp;
	int i, j;

	UINT32 dwCount;
	char  cName[21], cItemType, cEquipPos, cGenderLimit;
	bool  bIsEquipped;
	short sSprite, sSpriteFrame, sLevelLimit;
	UINT16  wCost, wWeight, wCurLifeSpan;
	char  cTxt[120], cItemColor;

	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

	cp++;

	ZeroMemory(cName, sizeof(cName));
	memcpy(cName, cp, 20);
	cp += 20;

	dwp = (UINT32 *)cp;
	dwCount = *dwp;
	cp += 4;

	cItemType = *cp;
	cp++;

	cEquipPos = *cp;
	cp++;

	bIsEquipped = (bool)*cp;
	cp++;

	sp = (short *)cp;
	sLevelLimit = *sp;
	cp += 2;

	cGenderLimit = *cp;
	cp++;

	wp = (UINT16 *)cp;
	wCurLifeSpan = *wp;
	cp += 2;

	wp = (UINT16 *)cp;
	wWeight = *wp;
	cp += 2;

	sp = (short *)cp;
	sSprite = *sp;
	cp += 2;

	sp = (short *)cp;
	sSpriteFrame = *sp;
	cp += 2;

	cItemColor = *cp; // v1.4
	cp++;

	wp = (UINT16 *)cp;
	wCost = *wp;
	ZeroMemory(cTxt, sizeof(cTxt));
	char cStr1[64], cStr2[64], cStr3[64];
	GetItemName(cName, 0, cStr1, cStr2, cStr3);
	wsprintf(cTxt, NOTIFYMSG_ITEMTRADE, cStr1, wCost);
	AddEventList(cTxt, 10);

	if ((cItemType == DEF_ITEMTYPE_CONSUME) || (cItemType == DEF_ITEMTYPE_ARROW))
	{
		for (i = 0; i < DEF_MAXITEMS; i++)
			if ((m_pItemList[i] != 0) && (memcmp(m_pItemList[i]->m_cName, cName, 20) == 0))
			{
				m_pItemList[i]->m_dwCount += dwCount;
				return;
			}
	}

	short nX, nY;
	for (i = 0; i < DEF_MAXITEMS; i++)
	{
		if ((m_pItemList[i] != 0) && (memcmp(m_pItemList[i]->m_cName, cName, 20) == 0))
		{
			nX = m_pItemList[i]->m_sX;
			nY = m_pItemList[i]->m_sY;
			break;
		}
		else
		{
			nX = 40;
			nY = 30;
		}
	}

	for (i = 0; i < DEF_MAXITEMS; i++)
		if (m_pItemList[i] == 0)
		{
			m_pItemList[i] = new class CItem;
			memcpy(m_pItemList[i]->m_cName, cName, 20);
			m_pItemList[i]->m_dwCount = dwCount;
			m_pItemList[i]->m_sX = nX;
			m_pItemList[i]->m_sY = nY;
			bSendCommand(MSGID_REQUEST_SETITEMPOS, 0, i, nX, nY, 0, 0);
			m_pItemList[i]->m_cItemType = cItemType;
			m_pItemList[i]->m_cEquipPos = cEquipPos;
			m_bIsItemDisabled[i] = false;
			m_bIsItemEquipped[i] = false;
			m_pItemList[i]->m_sLevelLimit = sLevelLimit;
			m_pItemList[i]->m_cGenderLimit = cGenderLimit;
			m_pItemList[i]->m_wCurLifeSpan = wCurLifeSpan;
			m_pItemList[i]->m_wWeight = wWeight;
			m_pItemList[i]->m_sSprite = sSprite;
			m_pItemList[i]->m_sSpriteFrame = sSpriteFrame;
			m_pItemList[i]->m_cItemColor = cItemColor;    // v1.4

			// fixed v1.11
			for (j = 0; j < DEF_MAXITEMS; j++)
				if (m_cItemOrder[j] == -1) {
					m_cItemOrder[j] = i;
					return;
				}

			return;
		}
}

// VAMP - online users list display the list of users from new OnlineUser class.
void CGame::DrawDialogBox_OnlineUsers(short msX, short msY, short msZ, char cLB)
{
	short sX, sY;
	UINT32 dwTime = m_dwCurTime;
	int  i, iTemp;
	char cTemp[255], cTemp2[255], cStr2[255], cStr3[255];

	float dTmp1, dTmp2, dTmp3;
	int  iTotalLines, iPointerLoc;
	bool bFlagStatLow = false;
	bool bFlagRedShown = false;
	float d1, d2, d3;
	int iR, iG, iB;

	char cTmpCName[11];
	char cTmpGName[21];

	for (int j = 0; j< DEF_MAXCLIENTS; j++)
	{
		for (int k = j; k< DEF_MAXCLIENTS - 1; k++)
		{
			if (m_pOnlineUsersList[j] != 0 && m_pOnlineUsersList[k] != 0)
			{
				ZeroMemory(cTmpCName, sizeof(cTmpCName));
				ZeroMemory(cTmpGName, sizeof(cTmpGName));

				if (m_pOnlineUsersList[j]->m_cName[0] > m_pOnlineUsersList[k]->m_cName[0])
				{
					memcpy(cTmpCName, m_pOnlineUsersList[j]->m_cName, 10);
					memcpy(cTmpGName, m_pOnlineUsersList[j]->m_cGuildName, 20);

					memcpy(m_pOnlineUsersList[j]->m_cName, m_pOnlineUsersList[k]->m_cName, 10);
					memcpy(m_pOnlineUsersList[j]->m_cGuildName, m_pOnlineUsersList[k]->m_cGuildName, 20);

					memcpy(m_pOnlineUsersList[k]->m_cName, cTmpCName, 10);
					memcpy(m_pOnlineUsersList[k]->m_cGuildName, cTmpGName, 20);
				}
			}
		}
	}

	sX = m_stDialogBoxInfo[60].sX;
	sY = m_stDialogBoxInfo[60].sY;

	if (m_bUseOldPanels) {
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME6, sX, sY, 2);
		//PutString_SprFont(sX + 65, sY + 35, "Online Users List", 1, 1, 8);
	}
	else {
		short limitX, limitY;
		limitX = sX + m_stDialogBoxInfo[60].sSizeX;
		limitY = sY + m_stDialogBoxInfo[60].sSizeY;

		m_DDraw.DrawShadowBox(sX, sY, limitX, limitY, 0, true);
		if (!m_bDialogTrans) m_DDraw.DrawShadowBox(sX, sY, limitX, limitY, 0, true);

		m_DDraw.DrawShadowBox(sX, sY, limitX, sY + 25, 0, true);
		if (!m_bDialogTrans) m_DDraw.DrawShadowBox(sX, sY, limitX, sY + 25, 0, true);
		//PutString_SprFont2(sX + 100, sY + 5, "Magics", 240, 240, 240);
	}

	PutString_SprFont2(sX + 65, sY + 5, "Online Users List", 240, 240, 240);

	switch (m_stDialogBoxInfo[60].cMode) {
	case 0:
		iTotalLines = 0;
		for (i = 0; i < DEF_MAXCLIENTS; i++)
			if (m_pOnlineUsersList[i] != 0) iTotalLines++;
		if (iTotalLines > 17) {
			d1 = (float)m_stDialogBoxInfo[60].sView;
			d2 = (float)(iTotalLines - 17);
			d3 = (274.0f * d1) / d2;
			iPointerLoc = (int)(d3);
			if (m_bUseOldPanels)
			{
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME6, sX, sY, 3);
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME6, sX + 242, sY + iPointerLoc + 35, 7);
			}
			else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME2, sX + 242, sY + iPointerLoc + 35, 7);
		}
		else iPointerLoc = 0;

		if (cLB != 0 && iTotalLines > 17)
		{
			if ((iGetTopDialogBoxIndex() == 60))
			{
				if ((msX >= sX + 235) && (msX <= sX + 260) && (msY >= sY + 10) && (msY <= sY + 330))
				{
					d1 = (float)(msY - (sY + 35));
					d2 = (float)(iTotalLines - 17);
					d3 = (d1 * d2) / 274.0f;
					m_stDialogBoxInfo[60].sView = (int)(d3 + 0.5f);
				}
			}
		}
		else m_stDialogBoxInfo[60].bIsScrollSelected = false;
		if (iGetTopDialogBoxIndex() == 60 && msZ != 0)
		{
			m_stDialogBoxInfo[60].sView = m_stDialogBoxInfo[60].sView - msZ / 60;
			m_DInput.m_sZ = 0;
		}
		if (iTotalLines > 17 && m_stDialogBoxInfo[60].sView > iTotalLines - 17) m_stDialogBoxInfo[60].sView = iTotalLines - 17;
		if (m_stDialogBoxInfo[60].sView < 0 || iTotalLines < 17) m_stDialogBoxInfo[60].sView = 0;

		for (i = 0; i < 17; i++)
			if (((i + m_stDialogBoxInfo[60].sView) < DEF_MAXCLIENTS) && (m_pOnlineUsersList[i + m_stDialogBoxInfo[60].sView] != 0))
			{
				ZeroMemory(cTemp, sizeof(cTemp));
				wsprintf(cTemp, m_pOnlineUsersList[i + m_stDialogBoxInfo[60].sView]->m_cName, cStr2, cStr3);

				ZeroMemory(cTemp2, sizeof(cTemp2));
				wsprintf(cTemp2, m_pOnlineUsersList[i + m_stDialogBoxInfo[60].sView]->m_cGuildName, cStr2, cStr3);
				m_Misc.ReplaceString(cTemp2, '_', ' ');

				if (memcmp(cTemp2, m_cGuildName, sizeof(m_cGuildName)) == 0)
				{
					iR = 0;	iG = 255; iB = 0;
				}
				else
				{
					iR = 255;	iG = 255; iB = 255;
				}


				if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + i * 15 + 35) && (msY <= sY + i * 15 + 49))
				{
					PutString2(sX + 25, sY + i * 15 + 35, cTemp, iR, iG, iB);
				}
				else
				{
					PutString2(sX + 25, sY + i * 15 + 35, cTemp, iR / 2, iG / 2, iB / 2);
				}
			}

		for (i = 0; i < 17; i++)
			if (((i + m_stDialogBoxInfo[60].sView) < DEF_MAXCLIENTS) && (m_pOnlineUsersList[i + m_stDialogBoxInfo[60].sView] != 0))
			{
				ZeroMemory(cTemp, sizeof(cTemp));
				wsprintf(cTemp, m_pOnlineUsersList[i + m_stDialogBoxInfo[60].sView]->m_cGuildName, cStr2, cStr3);
				m_Misc.ReplaceString(cTemp, '_', ' ');

				ZeroMemory(cTemp2, sizeof(cTemp2));
				wsprintf(cTemp2, m_pOnlineUsersList[i + m_stDialogBoxInfo[60].sView]->m_cName, cStr2, cStr3);

				// display blank space rather than NONE if char has no guild
				if (memcmp(cTemp, "NONE", 4) == 0)
				{
					memcpy(cTemp, "    ", 4);
				}

				if (memcmp(cTemp, m_cGuildName, sizeof(m_cGuildName)) == 0)
				{
					iR = 0;	iG = 255; iB = 0;
				}
				else
				{
					iR = 255;	iG = 255; iB = 255;
				}

				if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + i * 15 + 35) && (msY <= sY + i * 15 + 49))
				{
					PutString2(sX + 125, sY + i * 15 + 35, cTemp, iR, iG, iB);
				}
				else
				{
					PutString2(sX + 125, sY + i * 15 + 35, cTemp, iR / 2, iG / 2, iB / 2);
				}
			}

		if ((m_dwCurTime - m_dwReqUsersTime) > 30000)
		{
			if ((msX > sX + 180) && (msX < sX + 180 + 40) && (msY > sY + 45 + 14 * 18) && (msY < sY + 45 + 15 * 18)) {
				PutString2(sX + 180, sY + 45 + (14 * 18), "Refresh", 255, 255, 255);
			}
			else {
				PutString2(sX + 180, sY + 45 + (14 * 18), "Refresh", 255, 255, 100);
			}
		}
		else
		{
			PutString2(sX + 180, sY + 45 + (14 * 18), "Refresh", 65, 65, 65);
		}

		break;

	}
}


void CGame::DlgBoxClick_OnlineUsers(int msX, int msY)
{
	int i;
	short sX, sY;
	char cTemp[21];

	sX = m_stDialogBoxInfo[60].sX;
	sY = m_stDialogBoxInfo[60].sY;

		for (i = 0; i < 17; i++)
		{
			if (((i + m_stDialogBoxInfo[60].sView) < DEF_MAXCLIENTS) && (m_pOnlineUsersList[i + m_stDialogBoxInfo[60].sView] != 0))
			{
				if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + i * 15 + 35) && (msY <= sY + i * 15 + 49))
				{
					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "/to %s", m_pOnlineUsersList[i + m_stDialogBoxInfo[60].sView]->m_cName);
					bSendCommand(MSGID_COMMAND_CHATMSG, 0, 0, 0, 0, 0, cTemp);
				}
			}
		}

	if ((msX > sX + 180) && (msX < sX + 180 + 40) && (msY > sY + 45 + 14 * 18) && (msY < sY + 45 + 15 * 18))
	{
		if ((m_dwCurTime - m_dwReqUsersTime) > 30000)
		{
			for (i = 0; i < DEF_MAXCLIENTS; i++)
				if (m_pOnlineUsersList[i] != 0) m_pOnlineUsersList[i] = 0;

			for (i = 0; i < DEF_MAXCLIENTS; i++)
				if (strlen(m_stOnlineGuild[i].cCharName) != 0) ZeroMemory(m_stOnlineGuild[i].cCharName, sizeof(m_stOnlineGuild[i].cCharName));

			bSendCommand(MSGID_REQUEST_ONLINE, 0, 0, 0, 0, 0, 0);

			m_dwReqUsersTime = m_dwCurTime;
			m_cCommandCount--;
		}
	}
}

// thx to kazin
void CGame::ResponseOnlines(char * pData)
{
	char * cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

	int j;
	for (j = 0; j < DEF_MAXCLIENTS; j++)
	{
		if (m_pOnlineUsersList[j] != 0)
		{
			delete m_pOnlineUsersList[j];
			m_pOnlineUsersList[j] = 0;
		}
	}

	short total;
	total = (short)*cp;
	cp += 2;

	for (j = 0; j < total; j++)
	{
		m_pOnlineUsersList[j] = new OnlineUser;
		memcpy(m_pOnlineUsersList[j]->m_cName, cp, 10);
		cp += 10;
		memcpy(m_pOnlineUsersList[j]->m_cGuildName, cp, 20);
		cp += 20;
	}
}

void CGame::NpcTalkHandler(char *pData)
{
	char  * cp, cRewardName[21], cTargetName[21], cTemp[21], cTxt[250], cStr1[255], cStr2[255], cStr3[255];
	short * sp, sType, sResponse;
	int     iAmount, iIndex, iContribution, iX, iY, iRange;
	int     iTargetType, iTargetCount, iQuestionType;

	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
	sp = (short *)cp;
	sType = *sp;
	cp += 2;
	sp = (short *)cp;
	sResponse = *sp;
	cp += 2;
	sp = (short *)cp;
	iAmount = *sp;
	cp += 2;
	sp = (short *)cp;
	iContribution = *sp;
	cp += 2;
	sp = (short *)cp;
	iTargetType = *sp;
	cp += 2;
	sp = (short *)cp;
	iTargetCount = *sp;
	cp += 2;
	sp = (short *)cp;
	iX = *sp;
	cp += 2;
	sp = (short *)cp;
	iY = *sp;
	cp += 2;
	sp = (short *)cp;
	iRange = *sp;
	cp += 2;
	ZeroMemory(cRewardName, sizeof(cRewardName));
	memcpy(cRewardName, cp, 20);
	cp += 20;
	ZeroMemory(cTargetName, sizeof(cTargetName));
	memcpy(cTargetName, cp, 20);
	cp += 20;
	EnableDialogBox(21, sResponse, sType, 0);

	if ((sType >= 1) && (sType <= 100))
	{
		iIndex = m_stDialogBoxInfo[21].sV1;
		m_pMsgTextList2[iIndex] = new class CMsg(0, "  ", 0);
		iIndex++;
		iQuestionType = 0;
		switch (sType) {
		case 1: //Monster Hunt
			ZeroMemory(cTemp, sizeof(cTemp));
			GetNpcName(iTargetType, cTemp);
			ZeroMemory(cTxt, sizeof(cTxt));
			wsprintf(cTxt, NPC_TALK_HANDLER16, iTargetCount, cTemp);
			m_pMsgTextList2[iIndex] = new class CMsg(0, cTxt, 0);
			iIndex++;

			//Magn0S:: Add to show Item Name
			ZeroMemory(cStr1, sizeof(cStr1));
			GetItemName(cRewardName, 0, cStr1, cStr2, cStr3);
			wsprintf(cTxt, "Item Reward: %s", cStr1);
			m_pMsgTextList2[iIndex] = new class CMsg(0, cTxt, 0);
			iIndex += 2;

			ZeroMemory(cTxt, sizeof(cTxt));
			if (memcmp(cTargetName, "NONE", 4) == 0) {
				strcpy(cTxt, NPC_TALK_HANDLER17);//"
				m_pMsgTextList2[iIndex] = new class CMsg(0, cTxt, 0);
				iIndex++;
			}
			else {
				ZeroMemory(cTemp, sizeof(cTemp));
				GetOfficialMapName(cTargetName, cTemp);
				wsprintf(cTxt, NPC_TALK_HANDLER18, cTemp);//"Map : %s"
				m_pMsgTextList2[iIndex] = new class CMsg(0, cTxt, 0);
				iIndex++;

				if (iX != 0) {
					ZeroMemory(cTxt, sizeof(cTxt));
					wsprintf(cTxt, NPC_TALK_HANDLER19, iX, iY, iRange);//"Position: %d,%d within %d blocks"
					m_pMsgTextList2[iIndex] = new class CMsg(0, cTxt, 0);
					iIndex++;
				}

				ZeroMemory(cTxt, sizeof(cTxt));
				wsprintf(cTxt, NPC_TALK_HANDLER20, iContribution);//"
				m_pMsgTextList2[iIndex] = new class CMsg(0, cTxt, 0);
				iIndex++;
			}
			iQuestionType = 1;
			break;

		case 7: //
			ZeroMemory(cTxt, sizeof(cTxt));
			m_pMsgTextList2[iIndex] = new class CMsg(0, NPC_TALK_HANDLER21, 0);
			iIndex++;

			ZeroMemory(cTxt, sizeof(cTxt));
			if (memcmp(cTargetName, "NONE", 4) == 0) {
				strcpy(cTxt, NPC_TALK_HANDLER22);
				m_pMsgTextList2[iIndex] = new class CMsg(0, cTxt, 0);
				iIndex++;
			}
			else {
				ZeroMemory(cTemp, sizeof(cTemp));
				GetOfficialMapName(cTargetName, cTemp);
				wsprintf(cTxt, NPC_TALK_HANDLER23, cTemp);
				m_pMsgTextList2[iIndex] = new class CMsg(0, cTxt, 0);
				iIndex++;

				if (iX != 0) {
					ZeroMemory(cTxt, sizeof(cTxt));
					wsprintf(cTxt, NPC_TALK_HANDLER24, iX, iY, iRange);
					m_pMsgTextList2[iIndex] = new class CMsg(0, cTxt, 0);
					iIndex++;
				}

				ZeroMemory(cTxt, sizeof(cTxt));
				wsprintf(cTxt, NPC_TALK_HANDLER25, iContribution);
				m_pMsgTextList2[iIndex] = new class CMsg(0, cTxt, 0);
				iIndex++;
			}
			iQuestionType = 1;
			break;

		case 10: // Crusade
			ZeroMemory(cTxt, sizeof(cTxt));
			m_pMsgTextList2[iIndex] = new class CMsg(0, NPC_TALK_HANDLER26, 0);
			iIndex++;

			ZeroMemory(cTxt, sizeof(cTxt));
			strcpy(cTxt, NPC_TALK_HANDLER27);//"
			m_pMsgTextList2[iIndex] = new class CMsg(0, cTxt, 0);
			iIndex++;

			ZeroMemory(cTxt, sizeof(cTxt));
			strcpy(cTxt, NPC_TALK_HANDLER28);//"
			m_pMsgTextList2[iIndex] = new class CMsg(0, cTxt, 0);
			iIndex++;

			ZeroMemory(cTxt, sizeof(cTxt));
			strcpy(cTxt, NPC_TALK_HANDLER29);//"
			m_pMsgTextList2[iIndex] = new class CMsg(0, cTxt, 0);
			iIndex++;

			ZeroMemory(cTxt, sizeof(cTxt));
			strcpy(cTxt, NPC_TALK_HANDLER30);//"
			m_pMsgTextList2[iIndex] = new class CMsg(0, cTxt, 0);
			iIndex++;

			ZeroMemory(cTxt, sizeof(cTxt));
			strcpy(cTxt, " ");
			m_pMsgTextList2[iIndex] = new class CMsg(0, cTxt, 0);
			iIndex++;

			ZeroMemory(cTxt, sizeof(cTxt));
			if (memcmp(cTargetName, "NONE", 4) == 0) {
				strcpy(cTxt, NPC_TALK_HANDLER31);//"
				m_pMsgTextList2[iIndex] = new class CMsg(0, cTxt, 0);
				iIndex++;
			}
			else {
				ZeroMemory(cTemp, sizeof(cTemp));
				GetOfficialMapName(cTargetName, cTemp);
				wsprintf(cTxt, NPC_TALK_HANDLER32, cTemp);//"
				m_pMsgTextList2[iIndex] = new class CMsg(0, cTxt, 0);
				iIndex++;
			}
			iQuestionType = 2;
			break;
		}

		switch (iQuestionType) {
		case 1:
			m_pMsgTextList2[iIndex] = new class CMsg(0, "  ", 0);
			iIndex++;
			m_pMsgTextList2[iIndex] = new class CMsg(0, NPC_TALK_HANDLER33, 0);//"
			iIndex++;
			m_pMsgTextList2[iIndex] = new class CMsg(0, NPC_TALK_HANDLER34, 0);//"
			iIndex++;
			m_pMsgTextList2[iIndex] = new class CMsg(0, "  ", 0);
			iIndex++;
			break;

		case 2:
			m_pMsgTextList2[iIndex] = new class CMsg(0, "  ", 0);
			iIndex++;
			m_pMsgTextList2[iIndex] = new class CMsg(0, NPC_TALK_HANDLER35, 0);//"
			iIndex++;
			m_pMsgTextList2[iIndex] = new class CMsg(0, "  ", 0);
			iIndex++;
			break;

		default:
			break;
		}
	}
}

//
//void CGame::GetItemName(CItem* pItem, char* pStr1, char* pStr2, char* pStr3)
void CGame::GetItemName(CItem* pItem, char* pStr1, char* pStr2, char* pStr3, char* pStr4, char* pStr5, char* pStr6)
{
	int i;
	char cTxt[256], cTxt2[256], cName[51];
	UINT32 dwType1, dwType2, dwValue1, dwValue2, dwValue3;
	char cDescri1[256], cDescri2[256], cDescri3[256];

	m_bIsSpecial = false;
	m_bIsRare = false;
	ZeroMemory(cName, sizeof(cName));
	ZeroMemory(pStr1, sizeof(pStr1));
	ZeroMemory(pStr2, sizeof(pStr2));
	ZeroMemory(pStr3, sizeof(pStr3));
	ZeroMemory(pStr4, sizeof(pStr4));
	ZeroMemory(pStr5, sizeof(pStr5));
	ZeroMemory(pStr6, sizeof(pStr6));

	ZeroMemory(cDescri1, sizeof(cDescri1));
	ZeroMemory(cDescri2, sizeof(cDescri2));
	ZeroMemory(cDescri3, sizeof(cDescri3));

	strcpy(cName, pItem->m_cName);
	for (i = 0; i < DEF_MAXITEMNAMES; i++)
		if ((m_pItemNameList[i] != 0) && (strcmp(m_pItemNameList[i]->m_cOriginName, pItem->m_cName) == 0))
		{
			strcpy(cName, m_pItemNameList[i]->m_cName);
			break;
		}


	if (0 == memcmp(pItem->m_cName, "AcientTablet", 12)) m_bIsSpecial = true;
	else if (0 == memcmp(pItem->m_cName, "NecklaceOf", 10)) { m_bIsSpecial = true; m_bIsRare = true; }
	else if (0 == memcmp(pItem->m_cName, "DarkElfBow", 10)) { m_bIsSpecial = true; m_bIsRare = true; }
	else if (0 == memcmp(pItem->m_cName, "DarkExecutor", 12)) { m_bIsSpecial = true; m_bIsRare = true; }
	else if (0 == memcmp(pItem->m_cName, "The_Devastator", 14)) { m_bIsSpecial = true; m_bIsRare = true; }
	else if (0 == memcmp(pItem->m_cName, "DemonSlayer", 10)) { m_bIsSpecial = true; m_bIsRare = true; }
	else if (0 == memcmp(pItem->m_cName, "LightingBlade", 12)) { m_bIsSpecial = true; m_bIsRare = true; }
	else if (0 == memcmp(pItem->m_cName, "5thAnniversary", 13)) m_bIsSpecial = true;
	else if (0 == memcmp(pItem->m_cName, "RubyRing", 8)) m_bIsSpecial = true;
	else if (0 == memcmp(pItem->m_cName, "SapphireRing", 12)) m_bIsSpecial = true;
	else if (0 == memcmp(pItem->m_cName, "Ringof", 6)) m_bIsSpecial = true;
	else if (0 == memcmp(pItem->m_cName, "MagicNecklace", 13)) m_bIsSpecial = true;
	else if (0 == memcmp(pItem->m_cName, "MagicWand(M.Shield)", 19)) { m_bIsSpecial = true; m_bIsRare = true; }
	else if (0 == memcmp(pItem->m_cName, "MagicWand(MS30-LLF)", 19)) { m_bIsSpecial = true; m_bIsRare = true; }
	else if (0 == memcmp(pItem->m_cName, "Merien", 6)) { m_bIsSpecial = true; m_bIsRare = true; }
	else if (0 == memcmp(pItem->m_cName, "BerserkWand", 11)) { m_bIsSpecial = true; m_bIsRare = true; }
	else if (0 == memcmp(pItem->m_cName, "ResurWand", 9)) { m_bIsSpecial = true; m_bIsRare = true; }
	else if (0 == memcmp(pItem->m_cName, "Blood", 5)) { m_bIsSpecial = true; m_bIsRare = true; }
	else if (0 == memcmp(pItem->m_cName, "Swordof", 7)) m_bIsSpecial = true;
	else if (0 == memcmp(pItem->m_cName, "StoneOf", 7)) m_bIsSpecial = true;
	else if (0 == memcmp(pItem->m_cName, "ZemstoneofSacrifice", 19)) m_bIsSpecial = true;
	else if (0 == memcmp(pItem->m_cName, "StormBringer", 12)) { m_bIsSpecial = true; m_bIsRare = true; }
	else if (0 == memcmp(pItem->m_cName, "Aresden", 7)) m_bIsSpecial = true;
	else if (0 == memcmp(pItem->m_cName, "Elvine", 6)) m_bIsSpecial = true;
	else if (0 == memcmp(pItem->m_cName, "EmeraldRing", 11)) m_bIsSpecial = true;
	else if (0 == memcmp(pItem->m_cName, "Excaliber", 9)) m_bIsSpecial = true;
	else if (0 == memcmp(pItem->m_cName, "Xelima", 6)) m_bIsSpecial = true;
	else if (0 == memcmp(pItem->m_cName, "Ghost", 5)) { m_bIsSpecial = true; m_bIsRare = true; }
	else if (0 == memcmp(pItem->m_cName, "aHeroOf", 7)) m_bIsSpecial = true; 
	else if (0 == memcmp(pItem->m_cName, "eHeroOf", 7)) m_bIsSpecial = true; 
	else if (0 == memcmp(pItem->m_cName, "Kloness", 7)) { m_bIsSpecial = true; m_bIsRare = true; }

	if ((pItem->m_dwAttribute & 0x00000001) != 0)
	{
		m_bIsSpecial = true;
		strcpy(pStr1, cName);
		if (pItem->m_cItemType == DEF_ITEMTYPE_MATERIAL)
			wsprintf(pStr2, GET_ITEM_NAME1, pItem->m_sItemSpecEffectValue2);		//"Purity: %d%%"
		else
		{	// Crafting Magins completion fix
			if (pItem->m_cEquipPos == DEF_EQUIPPOS_LFINGER)
			{
				wsprintf(pStr2, GET_ITEM_NAME2, pItem->m_sItemSpecEffectValue2);	//"Completion:
			}
			else
			{
				wsprintf(pStr2, GET_ITEM_NAME2, pItem->m_sItemSpecEffectValue2 + 100);	//"Completion: +100
			}
		}
	}
	else
	{
		if (pItem->m_dwCount == 1)
			wsprintf(G_cTxt, "%s", cName);
		else wsprintf(G_cTxt, DRAW_DIALOGBOX_SELLOR_REPAIR_ITEM1, pItem->m_dwCount, cName);//"%d %s"
		strcpy(pStr1, G_cTxt);
	}

	if ((pItem->m_dwAttribute & 0x00F0F000) != 0)
	{
		m_bIsSpecial = true;
		dwType1 = (pItem->m_dwAttribute & 0x00F00000) >> 20;
		dwValue1 = (pItem->m_dwAttribute & 0x000F0000) >> 16;
		dwType2 = (pItem->m_dwAttribute & 0x0000F000) >> 12;
		dwValue2 = (pItem->m_dwAttribute & 0x00000F00) >> 8;
		if (dwType1 != 0)
		{
			ZeroMemory(cTxt, sizeof(cTxt));
			switch (dwType1) {
			case 1: strcpy(cTxt, GET_ITEM_NAME3);   break;
			case 2: strcpy(cTxt, GET_ITEM_NAME4);   break; // "Poisoning "
			case 3: strcpy(cTxt, GET_ITEM_NAME5);   break; // "Righteous "
			case 4: break;
			case 5: strcpy(cTxt, GET_ITEM_NAME6);   break; // "Agile "
			case 6: strcpy(cTxt, GET_ITEM_NAME7);   break;
			case 7: strcpy(cTxt, GET_ITEM_NAME8);   break;
			case 8: strcpy(cTxt, GET_ITEM_NAME9);   break;
			case 9: strcpy(cTxt, GET_ITEM_NAME10);  break;
			case 10: strcpy(cTxt, GET_ITEM_NAME11); break;
			case 11: strcpy(cTxt, GET_ITEM_NAME12); break;
			case 12: strcpy(cTxt, GET_ITEM_NAME13); break;

			case 15: strcpy(cTxt, GET_ITEM_NAME50); break;
			}
			strcat(cTxt, pStr1);
			ZeroMemory(pStr1, sizeof(pStr1));
			strcpy(pStr1, cTxt);

			ZeroMemory(cTxt, sizeof(cTxt));
			switch (dwType1) {
			case 1: wsprintf(cTxt, GET_ITEM_NAME14, dwValue1); break; // "Critical Hit Damage+%d"
			case 2: wsprintf(cTxt, GET_ITEM_NAME15, dwValue1 * 5); break; // "Poison Damage+%d"
			case 3: break;
			case 4: break;
			case 5: strcpy(cTxt, GET_ITEM_NAME16); break; // "Attack Speed -1"
			case 6: wsprintf(cTxt, GET_ITEM_NAME17, dwValue1 * 4); break;
			case 7: strcpy(cTxt, GET_ITEM_NAME18); break;
			case 8: wsprintf(cTxt, GET_ITEM_NAME19, dwValue1 * 7); break;
			case 9: strcpy(cTxt, GET_ITEM_NAME20); break;
			case 10: wsprintf(cTxt, GET_ITEM_NAME21, dwValue1 * 3); break;
			case 11: wsprintf(cTxt, GET_ITEM_NAME22, dwValue1); break;
			case 12: wsprintf(cTxt, GET_ITEM_NAME23, dwValue1); break;

			case 15:
				switch (dwValue1) {
				case 1:
					wsprintf(cTxt, GET_ITEM_NAME51, dwValue2 * 7); break;
					break;
				case 2:
					wsprintf(cTxt, GET_ITEM_NAME52, dwValue2 * 7); break;
					break;
				case 3:
					wsprintf(cTxt, GET_ITEM_NAME53, dwValue2 * 7); break;
					break;
				case 4:
					wsprintf(cTxt, GET_ITEM_NAME54, dwValue2 * 7); break;
					break;
				case 5:
					wsprintf(cTxt, GET_ITEM_NAME55, dwValue2 * 7); break;
					break;
				case 6:
					wsprintf(cTxt, GET_ITEM_NAME56, dwValue2 * 7); break;
					break;
				case 7:
					wsprintf(cTxt, GET_ITEM_NAME57, dwValue2 * 7); break;
					break;
				case 8:
					wsprintf(cTxt, GET_ITEM_NAME58, dwValue2 * 7); break;
					break;
				case 9:
					wsprintf(cTxt, GET_ITEM_NAME59, dwValue2 * 7); break;
					break;
				case 10:
					wsprintf(cTxt, GET_ITEM_NAME60, dwValue2 * 7); break;
					break;
				case 11:
					wsprintf(cTxt, GET_ITEM_NAME61, dwValue2 * 7); break;
					break;
				case 12:
					wsprintf(cTxt, GET_ITEM_NAME62, dwValue2 * 7); break;
					break;
				case 13:
					wsprintf(cTxt, GET_ITEM_NAME63, dwValue2 * 7); break;
					break;
				case 14:
					wsprintf(cTxt, GET_ITEM_NAME64, dwValue2 * 7); break;
					break;
				case 15:
					wsprintf(cTxt, GET_ITEM_NAME65, dwValue2 * 7); break;
					break;
				case 16:
					wsprintf(cTxt, GET_ITEM_NAME66, dwValue2 * 7); break;
					break;
				}
				break;

			}
			strcat(pStr2, cTxt);

			if (dwType2 != 0) {
				ZeroMemory(cTxt, sizeof(cTxt));
				switch (dwType2) {
				case 1:  wsprintf(cTxt, GET_ITEM_NAME24, dwValue2 * 7); break;
				case 2:  wsprintf(cTxt, GET_ITEM_NAME25, dwValue2 * 7); if (dwValue2 > 11) m_bIsRare = true; break;
				case 3:  wsprintf(cTxt, GET_ITEM_NAME26, dwValue2 * 7); if (dwValue2 > 11) m_bIsRare = true; break;
				case 4:  wsprintf(cTxt, GET_ITEM_NAME27, dwValue2 * 7); if (dwValue2 > 11) m_bIsRare = true; break;
				case 5:  wsprintf(cTxt, GET_ITEM_NAME28, dwValue2 * 7); break;//"SPrec
				case 6:  wsprintf(cTxt, GET_ITEM_NAME29, dwValue2 * 7); if (dwValue2 > 11) m_bIsRare = true; break;//"MPrec
				case 7:  wsprintf(cTxt, GET_ITEM_NAME30, dwValue2 * 7); if (dwValue2 > 11) m_bIsRare = true; break;

				case 8:  wsprintf(cTxt, GET_ITEM_NAME31, dwValue2 * 7); if (dwValue2 > 11) m_bIsRare = true; break;

				case 9:  //wsprintf(cTxt, GET_ITEM_NAME32, dwValue2*7);  if(dwValue2 > 11) m_bIsRare = true;  break;
					//Magn0S:: Added magic abs by elements
					switch (pItem->m_sNewEffect1)
					{
						//wsprintf(cTxt, GET_ITEM_NAME32, dwValue2*3); if(dwValue2 > 11) m_bIsRare = true;  break;
					case 1: wsprintf(cTxt, "Earth Absorption+%d%%", dwValue2 * 7); if (dwValue2 > 11) m_bIsRare = true;  break;
					case 2: wsprintf(cTxt, "Air Absorption+%d%%", dwValue2 * 7); if (dwValue2 > 11) m_bIsRare = true;  break;
					case 3: wsprintf(cTxt, "Fire Absorption+%d%%", dwValue2 * 7); if (dwValue2 > 11) m_bIsRare = true;  break;
					case 4: wsprintf(cTxt, "Water Absorption+%d%%", dwValue2 * 7); if (dwValue2 > 11) m_bIsRare = true;  break;
					case 5: wsprintf(cTxt, "Holy Absorption+%d%%", dwValue2 * 7); if (dwValue2 > 11) m_bIsRare = true;  break;
					case 6: wsprintf(cTxt, "Unholy Absorption+%d%%", dwValue2 * 7); if (dwValue2 > 11) m_bIsRare = true;  break;
					default:
						wsprintf(cTxt, "Magic Absorption+%d%%", dwValue2 * 7); if (dwValue2 > 11) m_bIsRare = true;  break;
					}
					break;

				case 10: wsprintf(cTxt, GET_ITEM_NAME33, dwValue2 * 4);   if (dwValue2 > 11) m_bIsRare = true; break;
				case 11: wsprintf(cTxt, GET_ITEM_NAME34, dwValue2 * 10); break;
				case 12: wsprintf(cTxt, GET_ITEM_NAME35, dwValue2 * 10); break;//"Gold +%
				}
				strcpy(pStr3, cTxt);

				// VAMP - elemental armours - add to end of item name
				if (pItem->m_sNewEffect1 > 0)
				{
					ZeroMemory(cTxt, sizeof(cTxt));
					switch (pItem->m_sNewEffect1)
					{
					case 1: strcpy(cTxt, " of Nature"); break;
					case 2: strcpy(cTxt, " of the Thunder"); break;
					case 3: strcpy(cTxt, " of the Flames"); break;
					case 4: strcpy(cTxt, " of the Ocean"); break;
					case 5: strcpy(cTxt, " of the Gods"); break;
					case 6: strcpy(cTxt, " of Corruption"); break;
					}
					strcat(pStr1, cTxt);
				}

			}
		}
	}

	//Magn0S:: Added for fragiles
	if (pItem->m_sNewEffect1 == 10) {
		m_bIsFragile = true;
		strcpy(cTxt, "Fragile ");
		strcat(cTxt, pStr1);
		ZeroMemory(pStr1, sizeof(pStr1));
		strcpy(pStr1, cTxt);
	}

	//Magn0S:: Add item descriptions
	ZeroMemory(cDescri1, sizeof(cDescri1));
	switch(pItem->m_sItemEffectType) {
		case DEF_ITEMEFFECTTYPE_ATTACK_MANASAVE:
			wsprintf(cDescri1, GET_ITEM_NAME44, pItem->m_sItemEffectValue4);	
			break;

		case DEF_ITEMEFFECTTYPE_ADDEFFECT:
			switch (pItem->m_sItemEffectValue1) {
			case 1:	wsprintf(cDescri1, GET_ITEM_NAME30, pItem->m_sItemEffectValue2); break;
			case 2:	wsprintf(cDescri1, GET_ITEM_NAME44, pItem->m_sItemEffectValue2); break;
			case 3:	wsprintf(cDescri1, GET_ITEM_NAME45, pItem->m_sItemEffectValue2); break;
			case 4:	wsprintf(cDescri1, GET_ITEM_NAME80, pItem->m_sItemEffectValue2); break;
			case 5:	wsprintf(cDescri1, "Luck Effect +%d", pItem->m_sItemEffectValue2); break;
			case 6:	wsprintf(cDescri1, GET_ITEM_NAME46, pItem->m_sItemEffectValue2); break;
			case 7: wsprintf(cDescri1, GET_ITEM_NAME49, pItem->m_sItemEffectValue2); break;
			case 8: wsprintf(cDescri1, GET_ITEM_NAME48, pItem->m_sItemEffectValue2); break;
			case 9: wsprintf(cDescri1, GET_ITEM_NAME70, pItem->m_sItemEffectValue2); break;
			case 10: wsprintf(cDescri1, GET_ITEM_NAME71, pItem->m_sItemEffectValue2); break;
			case 11: wsprintf(cDescri1, GET_ITEM_NAME24, pItem->m_sItemEffectValue2); break;
			case 12: wsprintf(cDescri1, GET_ITEM_NAME25, pItem->m_sItemEffectValue2); break;
			case 13: wsprintf(cDescri1, GET_ITEM_NAME27, pItem->m_sItemEffectValue2); break;
			case 14: wsprintf(cDescri1, GET_ITEM_NAME25, pItem->m_sItemEffectValue2); break;
			case 15: wsprintf(cDescri1, GET_ITEM_NAME46, pItem->m_sItemEffectValue2); break;
			}
			break;

		//default:
		//	break;

			
	}
	strcat(pStr4, cDescri1);
	//--------------------------------------------------------------------------------------------------

	dwValue3 = (pItem->m_dwAttribute & 0xF0000000) >> 28;
	if (dwValue3 > 0)
	{
		if (memcmp(pItem->m_cName, "AngelicPandent(STR)", 19) == 0)
		{
			wsprintf(cTxt2, "+%d Physical Damage", dwValue3);
			ZeroMemory(pStr4, sizeof(pStr4));
			strcpy(pStr4, cTxt2);
		}
		else if (memcmp(pItem->m_cName, "AngelicPandent(DEX)", 19) == 0)
		{
			wsprintf(cTxt2, "+%d Defense Ratio", dwValue3);
			ZeroMemory(pStr4, sizeof(pStr4));
			strcpy(pStr4, cTxt2);
		}
		else if (memcmp(pItem->m_cName, "AngelicPandent(INT)", 19) == 0)
		{
			wsprintf(cTxt2, "+%d Magic Resistance", dwValue3);
			ZeroMemory(pStr4, sizeof(pStr4));
			strcpy(pStr4, cTxt2);
		}
		else if (memcmp(pItem->m_cName, "AngelicPandent(MAG)", 19) == 0)
		{
			wsprintf(cTxt2, "+%d Magical Damage", dwValue3);
			ZeroMemory(pStr4, sizeof(pStr4));
			strcpy(pStr4, cTxt2);
		}
		
			if (pStr1[strlen(pStr1) - 2] == '+')
			{
				dwValue3 = atoi((char*)(pStr1 + strlen(pStr1) - 1)) + dwValue3;
				ZeroMemory(cTxt, sizeof(cTxt));
				memcpy(cTxt, pStr1, strlen(pStr1) - 2);
				ZeroMemory(cTxt2, sizeof(cTxt2));
				wsprintf(cTxt2, "%s+%d", cTxt, dwValue3);
				ZeroMemory(pStr1, sizeof(pStr1));
				strcpy(pStr1, cTxt2);
			}
			else
			{
				ZeroMemory(cTxt, sizeof(cTxt));
				wsprintf(cTxt, "+%d", dwValue3);
				strcat(pStr1, cTxt);
			}
		
	}
}

//Magn0S:: Changed
void CGame::GetItemName(char* cItemName, UINT32 dwAttribute, char* pStr1, char* pStr2, char* pStr3, short sEffect1, short sEffect2, short sEffect3, short sEffect4)
{
	int i;
	char cTxt[256], cTxt2[256], cName[51];
	UINT32 dwType1, dwType2, dwValue1, dwValue2, dwValue3;


	m_bIsSpecial = false;
	m_bIsRare = false;
	m_bIsFragile = false;
	ZeroMemory(cName, sizeof(cName));
	ZeroMemory(pStr1, sizeof(pStr1));
	ZeroMemory(pStr2, sizeof(pStr2));
	ZeroMemory(pStr3, sizeof(pStr3));

	strcpy(cName, cItemName);
	for (i = 0; i < DEF_MAXITEMNAMES; i++) {
		if ((m_pItemNameList[i] != 0) && (strcmp(m_pItemNameList[i]->m_cOriginName, cItemName) == 0)) {
			strcpy(cName, m_pItemNameList[i]->m_cName);
			break;
		}
	}

	if (0 == memcmp(cItemName, "AcientTablet", 12)) m_bIsSpecial = true;
	else if (0 == memcmp(cItemName, "NecklaceOf", 10)) { m_bIsSpecial = true; m_bIsRare = true; }
	else if (0 == memcmp(cItemName, "DarkElfBow", 10)) { m_bIsSpecial = true; m_bIsRare = true; }
	else if (0 == memcmp(cItemName, "DarkExecutor", 12)) { m_bIsSpecial = true; m_bIsRare = true; }
	else if (0 == memcmp(cItemName, "The_Devastator", 14)) { m_bIsSpecial = true; m_bIsRare = true; }
	else if (0 == memcmp(cItemName, "DemonSlayer", 10)) { m_bIsSpecial = true; m_bIsRare = true; }
	else if (0 == memcmp(cItemName, "LightingBlade", 12)) { m_bIsSpecial = true; m_bIsRare = true; }
	else if (0 == memcmp(cItemName, "Ghost", 5)) { m_bIsSpecial = true; m_bIsRare = true; }
	else if (0 == memcmp(cItemName, "RubyRing", 8)) m_bIsSpecial = true;
	else if (0 == memcmp(cItemName, "SapphireRing", 12)) m_bIsSpecial = true;
	else if (0 == memcmp(cItemName, "Ringof", 6)) m_bIsSpecial = true;
	else if (0 == memcmp(cItemName, "MagicNecklace", 13)) m_bIsSpecial = true;
	else if (0 == memcmp(cItemName, "MagicWand(M.Shield)", 19)) { m_bIsSpecial = true; m_bIsRare = true; }
	else if (0 == memcmp(cItemName, "MagicWand(MS30-LLF)", 19)) { m_bIsSpecial = true; m_bIsRare = true; }
	else if (0 == memcmp(cItemName, "Merien", 6)) { m_bIsSpecial = true; m_bIsRare = true; }
	else if (0 == memcmp(cItemName, "BerserkWand", 11)) { m_bIsSpecial = true; m_bIsRare = true; }
	else if (0 == memcmp(cItemName, "ResurWand", 9)) { m_bIsSpecial = true; m_bIsRare = true; }
	else if (0 == memcmp(cItemName, "Blood", 5)) { m_bIsSpecial = true; m_bIsRare = true; }
	else if (0 == memcmp(cItemName, "Swordof", 7)) m_bIsSpecial = true;
	else if (0 == memcmp(cItemName, "StoneOf", 7)) m_bIsSpecial = true;
	else if (0 == memcmp(cItemName, "ZemstoneofSacrifice", 19)) m_bIsSpecial = true;
	else if (0 == memcmp(cItemName, "StormBringer", 12)) { m_bIsSpecial = true; m_bIsRare = true; }
	else if (0 == memcmp(cItemName, "Aresden", 7)) m_bIsSpecial = true;
	else if (0 == memcmp(cItemName, "Elvine", 6)) m_bIsSpecial = true;
	else if (0 == memcmp(cItemName, "EmeraldRing", 11)) m_bIsSpecial = true;
	else if (0 == memcmp(cItemName, "Excaliber", 9)) m_bIsSpecial = true;
	else if (0 == memcmp(cItemName, "Xelima", 6)) m_bIsSpecial = true;
	else if (0 == memcmp(cItemName, "Kloness", 7)) { m_bIsSpecial = true; m_bIsRare = true; }
	else if (0 == memcmp(cItemName, "aHeroOf", 7)) m_bIsSpecial = true;
	else if (0 == memcmp(cItemName, "eHeroOf", 7)) m_bIsSpecial = true;
	
	strcpy(pStr1, cName);

	if ((dwAttribute & 0x00F0F000) != 0)
	{
		m_bIsSpecial = true;
		dwType1 = (dwAttribute & 0x00F00000) >> 20;
		dwValue1 = (dwAttribute & 0x000F0000) >> 16;
		dwType2 = (dwAttribute & 0x0000F000) >> 12;
		dwValue2 = (dwAttribute & 0x00000F00) >> 8;
		if (dwType1 != 0)
		{
			ZeroMemory(cTxt, sizeof(cTxt));
			switch (dwType1) {
			case 1: strcpy(cTxt, GET_ITEM_NAME3); break;
			case 2: strcpy(cTxt, GET_ITEM_NAME4); break;
			case 3: strcpy(cTxt, GET_ITEM_NAME5); break;
			case 4: strcpy(cTxt, "Fragile "); break;
			case 5: strcpy(cTxt, GET_ITEM_NAME6); break;
			case 6: strcpy(cTxt, GET_ITEM_NAME7); break;
			case 7: strcpy(cTxt, GET_ITEM_NAME8); break;
			case 8: strcpy(cTxt, GET_ITEM_NAME9); break;
			case 9: strcpy(cTxt, GET_ITEM_NAME10); break;
			case 10: strcpy(cTxt, GET_ITEM_NAME11); break;
			case 11: strcpy(cTxt, GET_ITEM_NAME12); break;
			case 12: strcpy(cTxt, GET_ITEM_NAME13); break;

			case 15: strcpy(cTxt, GET_ITEM_NAME50); break;
			}
			strcat(cTxt, pStr1);
			ZeroMemory(pStr1, sizeof(pStr1));
			strcpy(pStr1, cTxt);

			ZeroMemory(cTxt, sizeof(cTxt));
			switch (dwType1) {
			case 1: wsprintf(cTxt, GET_ITEM_NAME14, dwValue1); break;
			case 2: wsprintf(cTxt, GET_ITEM_NAME15, dwValue1 * 5); break;
			case 3: break;
			case 4: break;
			case 5: strcpy(cTxt, GET_ITEM_NAME16); break;
			case 6: wsprintf(cTxt, GET_ITEM_NAME17, dwValue1 * 4); break;
			case 7: strcpy(cTxt, GET_ITEM_NAME18); break;
			case 8: wsprintf(cTxt, GET_ITEM_NAME19, dwValue1 * 7); break;
			case 9: strcpy(cTxt, GET_ITEM_NAME20); break;
			case 10: wsprintf(cTxt, GET_ITEM_NAME21, dwValue1 * 3); break;
			case 11: wsprintf(cTxt, GET_ITEM_NAME22, dwValue1); break;
			case 12: wsprintf(cTxt, GET_ITEM_NAME23, dwValue1); break;

			case 15:
				switch (dwValue1){
				case 1:
					wsprintf(cTxt, GET_ITEM_NAME51, dwValue2 * 7); break;
					break;
				case 2:
					wsprintf(cTxt, GET_ITEM_NAME52, dwValue2 * 7); break;
					break;
				case 3:
					wsprintf(cTxt, GET_ITEM_NAME53, dwValue2 * 7); break;
					break;
				case 4:
					wsprintf(cTxt, GET_ITEM_NAME54, dwValue2 * 7); break;
					break;
				case 5:
					wsprintf(cTxt, GET_ITEM_NAME55, dwValue2 * 7); break;
					break;
				case 6:
					wsprintf(cTxt, GET_ITEM_NAME56, dwValue2 * 7); break;
					break;
				case 7:
					wsprintf(cTxt, GET_ITEM_NAME57, dwValue2 * 7); break;
					break;
				case 8:
					wsprintf(cTxt, GET_ITEM_NAME58, dwValue2 * 7); break;
					break;
				case 9:
					wsprintf(cTxt, GET_ITEM_NAME59, dwValue2 * 7); break;
					break;
				case 10:
					wsprintf(cTxt, GET_ITEM_NAME60, dwValue2 * 7); break;
					break;
				case 11:
					wsprintf(cTxt, GET_ITEM_NAME61, dwValue2 * 7); break;
					break;
				case 12:
					wsprintf(cTxt, GET_ITEM_NAME62, dwValue2 * 7); break;
					break;
				case 13:
					wsprintf(cTxt, GET_ITEM_NAME63, dwValue2 * 7); break;
					break;
				case 14:
					wsprintf(cTxt, GET_ITEM_NAME64, dwValue2 * 7); break;
					break;
				case 15:
					wsprintf(cTxt, GET_ITEM_NAME65, dwValue2 * 7); break;
					break;
				case 16:
					wsprintf(cTxt, GET_ITEM_NAME66, dwValue2 * 7); break;
					break;
				}
				break;
			}
			strcat(pStr2, cTxt);

			if (dwType2 != 0)
			{
				ZeroMemory(cTxt, sizeof(cTxt));
				switch (dwType2) {
				case 1:  wsprintf(cTxt, GET_ITEM_NAME24, dwValue2 * 7);  break;
				case 2:  wsprintf(cTxt, GET_ITEM_NAME25, dwValue2 * 7);  if (dwValue2 > 11) m_bIsRare = true; break;
				case 3:  wsprintf(cTxt, GET_ITEM_NAME26, dwValue2 * 7);  if (dwValue2 > 11) m_bIsRare = true; break;
				case 4:  wsprintf(cTxt, GET_ITEM_NAME27, dwValue2 * 7);  if (dwValue2 > 11) m_bIsRare = true; break;
				case 5:  wsprintf(cTxt, GET_ITEM_NAME28, dwValue2 * 7);  break;
				case 6:  wsprintf(cTxt, GET_ITEM_NAME29, dwValue2 * 7);  if (dwValue2 > 11) m_bIsRare = true; break;
				case 7:  wsprintf(cTxt, GET_ITEM_NAME30, dwValue2 * 7);  if (dwValue2 > 11) m_bIsRare = true; break;

				case 8:  wsprintf(cTxt, GET_ITEM_NAME31, dwValue2 * 7);  if (dwValue2 > 11) m_bIsRare = true; break;

				case 9:  //wsprintf(cTxt, GET_ITEM_NAME32, dwValue2*7);   if(dwValue2 > 11) m_bIsRare = true;  break;
					//Magn0S:: Added magic abs by elements
					switch (sEffect1) //m_sItemSpecEffectValue2
					{
						//wsprintf(cTxt, GET_ITEM_NAME32, dwValue2*3); if(dwValue2 > 11) m_bIsRare = true;  break;
					case 1: wsprintf(cTxt, "Earth Absorption+%d%%", dwValue2 * 7); if (dwValue2 > 11) m_bIsRare = true;  break;
					case 2: wsprintf(cTxt, "Air Absorption+%d%%", dwValue2 * 7); if (dwValue2 > 11) m_bIsRare = true;  break;
					case 3: wsprintf(cTxt, "Fire Absorption+%d%%", dwValue2 * 7); if (dwValue2 > 11) m_bIsRare = true;  break;
					case 4: wsprintf(cTxt, "Water Absorption+%d%%", dwValue2 * 7); if (dwValue2 > 11) m_bIsRare = true;  break;
					case 5: wsprintf(cTxt, "Holy Absorption+%d%%", dwValue2 * 7); if (dwValue2 > 11) m_bIsRare = true;  break;
					case 6: wsprintf(cTxt, "Unholy Absorption+%d%%", dwValue2 * 7); if (dwValue2 > 11) m_bIsRare = true;  break;
					default:
						wsprintf(cTxt, "Magic Absorption+%d%%", dwValue2 * 7); if (dwValue2 > 11) m_bIsRare = true;  break;
					}
					break;

				case 10: wsprintf(cTxt, GET_ITEM_NAME33, dwValue2 * 4);   if (dwValue2 > 11) m_bIsRare = true; break;
				case 11: wsprintf(cTxt, GET_ITEM_NAME34, dwValue2 * 10); break;
				case 12: wsprintf(cTxt, GET_ITEM_NAME35, dwValue2 * 10); break;
				}
				strcpy(pStr3, cTxt);
				// VAMP - elemental armours - add to end of item name
				if (dwType2 == 9)
				{
					ZeroMemory(cTxt, sizeof(cTxt));
					switch (sEffect1)
					{
					case 1: strcpy(cTxt, " of Nature"); break;
					case 2: strcpy(cTxt, " of the Thunder"); break;
					case 3: strcpy(cTxt, " of the Flames"); break;
					case 4: strcpy(cTxt, " of the Ocean"); break;
					case 5: strcpy(cTxt, " of the Gods"); break;
					case 6: strcpy(cTxt, " of Corruption"); break;
					}
					strcat(pStr1, cTxt);
				}
			}
		}
	}

	dwValue3 = (dwAttribute & 0xF0000000) >> 28;
	if (dwValue3 > 0)
	{
		if (pStr1[strlen(pStr1) - 2] == '+')
			{
				dwValue3 = atoi((char*)(pStr1 + strlen(pStr1) - 1)) + dwValue3;
				ZeroMemory(cTxt, sizeof(cTxt));
				memcpy(cTxt, pStr1, strlen(pStr1) - 2);
				ZeroMemory(cTxt2, sizeof(cTxt2));
				wsprintf(cTxt2, "%s+%d", cTxt, dwValue3);
				ZeroMemory(pStr1, sizeof(pStr1));
				strcpy(pStr1, cTxt2);
			}
			else
			{
				ZeroMemory(cTxt, sizeof(cTxt));
				wsprintf(cTxt, "+%d", dwValue3);
				strcat(pStr1, cTxt);
		}
		
	}
}

void CGame::_CalcSocketClosed()
{
	if (m_cGameMode == DEF_GAMEMODE_ONMAINGAME)
	{
		delete m_pGSock;
		m_pGSock = 0;
		m_bEscPressed = false;
		PlaySound('E', 14, 5);
		if (m_bSoundFlag) m_pESound[38]->bStop();
		if ((m_bSoundFlag) && (m_bMusicStat == true))
		{
			StopBGM();	// Snoopy: mp3 support
			if (m_pBGM != 0) m_pBGM->bStop();
		}
		ChangeGameMode(DEF_GAMEMODE_ONQUIT);
	}
}

void CGame::PointCommandHandler(int indexX, int indexY, char cItemID)
{
	char cTemp[31], cTxt[150];
	if ((m_iPointCommandType >= 100) && (m_iPointCommandType < 200))
	{
		bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_MAGIC, 0, indexX, indexY, m_iPointCommandType, 0);
	}
	else if ((m_iPointCommandType >= 0) && (m_iPointCommandType < 50))
	{
		bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_REQ_USEITEM, 0, m_iPointCommandType, indexX, indexY, cTemp, cItemID); // v1.4

		if (m_pItemList[m_iPointCommandType]->m_cItemType == DEF_ITEMTYPE_USE_SKILL)
			m_bSkillUsingStatus = true;
	}
	else if (m_iPointCommandType == 200) // Normal Hand
	{
		if ((strlen(m_cMCName) == 0) || (strcmp(m_cMCName, m_cPlayerName) == 0) || (m_cMCName[0] == '_'))
		{
			m_stDialogBoxInfo[32].cMode = 0;
			PlaySound('E', 14, 5);
			AddEventList(POINT_COMMAND_HANDLER1, 10);
		}
		else
		{
			m_stDialogBoxInfo[32].cMode = 3;
			PlaySound('E', 14, 5);
			ZeroMemory(m_stDialogBoxInfo[32].cStr, sizeof(m_stDialogBoxInfo[32].cStr));
			strcpy(m_stDialogBoxInfo[32].cStr, m_cMCName);
			bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_REQUEST_JOINPARTY, 0, 1, 0, 0, m_cMCName);
			return;
		}
	}
	else if (m_iPointCommandType == 250)
	{
		if ((strlen(m_cMCName) == 0) || (strcmp(m_cMCName, m_cPlayerName) == 0) || (m_cMCName[0] == '_'))
		{
			AddEventList("Only players can be added.", 10);
			PlaySound('E', 14, 5);
		}
		else
		{
			PlaySound('E', 14, 5);
			for (int i = 0; i < m_iTotalFriends; i++) {
				if (strcmp(m_cMCName, m_cFriends[i]) == 0) {
					AddEventList("This friend is already added.", 10);
					return;
				}
			}
			memcpy(m_cFriends[m_iTotalFriends], m_cMCName, 10);
			m_iTotalFriends++;
			m_stDialogBoxInfo[43].sV1 = 0;
			SaveFriendList();
			return;
		}
	}
	//Mang0S:: Begin code GM Pannel for HB-Fun - by Magn0S
	else if (m_iPointCommandType == 260) {
			if ((strlen(m_cMCName) == 0) || (strcmp(m_cMCName, m_cPlayerName) == 0) || (m_cMCName[0] == '_')) {
			AddEventList("Please, select a valid player.", 10);
			PlaySound('E', 24, 5);
		} else {	
			bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_CLIENTMSG, 0, 3, 0, 0, m_cMCName);
			PlaySound('E', 14, 5);
		}
	}
	else if (m_iPointCommandType == 261) { // Kill
		if ((strlen(m_cMCName) == 0) || (strcmp(m_cMCName, m_cPlayerName) == 0) || (m_cMCName[0] == '_')) {
			AddEventList("Please, select a valid player.", 10);
			PlaySound('E', 24, 5);
		}
		else {
			bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_CLIENTMSG, 0, 4, 0, 0, m_cMCName);
			PlaySound('E', 14, 5);
		}
	}
	else if (m_iPointCommandType == 262) { // Shutup 
		if ((strlen(m_cMCName) == 0) || (strcmp(m_cMCName, m_cPlayerName) == 0) || (m_cMCName[0] == '_')) {
			AddEventList("Please, select a valid player.", 10);
			PlaySound('E', 24, 5);
		}
		else {
			bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_CLIENTMSG, 0, 5, 0, 0, m_cMCName);
			PlaySound('E', 14, 5);
		}
	}
	else if (m_iPointCommandType == 263) { // Close connection
		if ((strlen(m_cMCName) == 0) || (strcmp(m_cMCName, m_cPlayerName) == 0) || (m_cMCName[0] == '_')) {
			AddEventList("Please, select a valid player.", 10);
			PlaySound('E', 24, 5);
		}
		else {
			ZeroMemory(cTxt, sizeof(cTxt));
			wsprintf(cTxt, "/closeconn %s", m_cMCName);
			bSendCommand(MSGID_COMMAND_CHATMSG, 0, 0, 0, 0, 0, cTxt);
			//bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_CLIENTMSG, 0, 6, 0, 0, m_cMCName);
			PlaySound('E', 14, 5);
		}
	}
	else if (m_iPointCommandType == 264) { // Check information
		if ((strlen(m_cMCName) == 0) || (strcmp(m_cMCName, m_cPlayerName) == 0) || (m_cMCName[0] == '_')) {
			AddEventList("Please, select a valid player.", 10);
			PlaySound('E', 24, 5);
		}
		else {
			bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_CLIENTMSG, 0, 7, 0, 0, m_cMCName);
			PlaySound('E', 14, 5);
		}
	}
	else if (m_iPointCommandType == 265) { // Ban char in nBI
		if ((strlen(m_cMCName) == 0) || (strcmp(m_cMCName, m_cPlayerName) == 0) || (m_cMCName[0] == '_')) {
			AddEventList("Please, select a valid player.", 10);
			PlaySound('E', 24, 5);
		}
		else {
			ZeroMemory(cCharSelection, sizeof(cCharSelection));
			strcpy(cCharSelection, m_cMCName);
			m_stDialogBoxInfo[56].cMode = 10; // Block Player in BI
			//bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_CLIENTMSG, 0, 8, 0, 0, m_cMCName);
			PlaySound('E', 14, 5);
		}
	}
	else if (m_iPointCommandType == 266) { // Block player
		if ((strlen(m_cMCName) == 0) || (strcmp(m_cMCName, m_cPlayerName) == 0) || (m_cMCName[0] == '_')) {
			AddEventList("Please, select a valid player.", 10);
			PlaySound('E', 24, 5);
		}
		else {
			ZeroMemory(cCharSelection, sizeof(cCharSelection));
			strcpy(cCharSelection, m_cMCName);
			m_stDialogBoxInfo[56].cMode = 10; // Block Char Connection
			//bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_CLIENTMSG, 0, 9, 0, 0, m_cMCName);
			PlaySound('E', 14, 5);
		}
	}
	else if (m_iPointCommandType == 267) { // Ban player
		if ((strlen(m_cMCName) == 0) || (strcmp(m_cMCName, m_cPlayerName) == 0) || (m_cMCName[0] == '_')) {
			AddEventList("Please, select a valid player.", 10);
			PlaySound('E', 24, 5);
		}
		else {
			ZeroMemory(cCharSelection, sizeof(cCharSelection));
			strcpy(cCharSelection, m_cMCName);
			m_stDialogBoxInfo[56].cMode = 10; // Ban player
			//bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_CLIENTMSG, 0, 10, 0, 0, m_cMCName);
			PlaySound('E', 14, 5);
		}
	}
}

void CGame::DrawDialogBox_Bank(short msX, short msY, short msZ, char cLB)
{
	short sX, sY, szX;
	int  iMaxPage, j, i, iLoc;
	char cTotalItems, cItemColor, cStr1[64], cStr2[64], cStr3[64], cStr4[64], cStr5[64], cStr6[64];
	bool bFlag;
	UINT32 dwTime = timeGetTime();
	sX = m_stDialogBoxInfo[14].sX;
	sY = m_stDialogBoxInfo[14].sY;
	szX = m_stDialogBoxInfo[14].sSizeX - 5;
	DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME6, sX, sY, 2);
	DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_TEXT2, sX, sY, 21);
	cTotalItems = 0;
	iMaxPage = 0;
	for (i = 0; i < DEF_MAXBANKITEMS; i++)
		if (m_pBankList[i] != 0)
			cTotalItems++;

	iMaxPage = (int)(cTotalItems / 12);
	if (iGetTopDialogBoxIndex() == 14 && msZ != 0)
	{
		if (msZ > 0) m_stDialogBoxInfo[14].sV14--;
		if (msZ < 0) m_stDialogBoxInfo[14].sV14++;
		m_DInput.m_sZ = 0;
	}
	if (m_stDialogBoxInfo[14].sV14 < 0)
		m_stDialogBoxInfo[14].sV14 = iMaxPage;
	if (m_stDialogBoxInfo[14].sV14 > 9)
		m_stDialogBoxInfo[14].sV14 = iMaxPage;
	if (m_stDialogBoxInfo[14].sV14 > iMaxPage)
		m_stDialogBoxInfo[14].sV14 = 0;


	m_pSprite[DEF_SPRID_INTERFACE_SPRFONTS]->PutSpriteFast(sX + 30, sY + 305, 19, dwTime);
	switch (m_stDialogBoxInfo[14].sV14)
	{
	case 0: m_pSprite[DEF_SPRID_INTERFACE_SPRFONTS]->PutSpriteFast(sX + 30, sY + 305, 20, dwTime); break;
	case 1: m_pSprite[DEF_SPRID_INTERFACE_SPRFONTS]->PutSpriteFast(sX + 43, sY + 305, 21, dwTime); break;
	case 2: m_pSprite[DEF_SPRID_INTERFACE_SPRFONTS]->PutSpriteFast(sX + 61, sY + 305, 22, dwTime); break;
	case 3: m_pSprite[DEF_SPRID_INTERFACE_SPRFONTS]->PutSpriteFast(sX + 86, sY + 305, 23, dwTime); break;
	case 4: m_pSprite[DEF_SPRID_INTERFACE_SPRFONTS]->PutSpriteFast(sX + 106, sY + 305, 24, dwTime); break;
	case 5: m_pSprite[DEF_SPRID_INTERFACE_SPRFONTS]->PutSpriteFast(sX + 121, sY + 305, 25, dwTime); break;
	case 6: m_pSprite[DEF_SPRID_INTERFACE_SPRFONTS]->PutSpriteFast(sX + 142, sY + 305, 26, dwTime); break;
	case 7: m_pSprite[DEF_SPRID_INTERFACE_SPRFONTS]->PutSpriteFast(sX + 169, sY + 305, 27, dwTime); break;
	case 8: m_pSprite[DEF_SPRID_INTERFACE_SPRFONTS]->PutSpriteFast(sX + 202, sY + 305, 28, dwTime); break;
	case 9: m_pSprite[DEF_SPRID_INTERFACE_SPRFONTS]->PutSpriteFast(sX + 222, sY + 305, 29, dwTime); break;
	}
	ZeroMemory(cStr1, sizeof(cStr1));
	ZeroMemory(cStr2, sizeof(cStr2));
	ZeroMemory(cStr3, sizeof(cStr3));
	ZeroMemory(cStr4, sizeof(cStr4));
	ZeroMemory(cStr5, sizeof(cStr5));
	ZeroMemory(cStr6, sizeof(cStr6));
	iLoc = 45;
	j = 0;
	switch (m_stDialogBoxInfo[14].cMode)
	{
	case -1:
		PutString(sX + 30 + 15, sY + 70, DRAW_DIALOGBOX_BANK1, RGB(0, 0, 0));//"
		PutString(sX + 30 + 15, sY + 85, DRAW_DIALOGBOX_BANK2, RGB(0, 0, 0));//"
		break;
	case 0:
		bFlag = false;
		for (i = (m_stDialogBoxInfo[14].sV14 * 12); i < (12 + (12 * m_stDialogBoxInfo[14].sV14)); i++)
		{
			if ((m_pBankList[i] != 0) && (i < DEF_MAXBANKITEMS))
			{
				GetItemName(m_pBankList[i], cStr1, cStr2, cStr3, cStr4, cStr5, cStr6);
				if ((msX > sX + 30) && (msX < sX + 210) && (msY >= sY + 110 + j * 15) && (msY <= sY + 124 + j * 15))
				{
					bFlag = true;
					PutAlignedString(sX, sX + szX, sY + 110 + j * 15, cStr1, 255, 255, 255);
					if (m_bIsSpecial)
						if (m_bIsRare)
						{
							PutAlignedString(sX + 70, sX + szX, sY + iLoc, cStr1, 255, 208, 60);
						}
						else PutAlignedString(sX + 70, sX + szX, sY + iLoc, cStr1, 0, 255, 50);
					else PutAlignedString(sX + 70, sX + szX, sY + iLoc, cStr1, 255, 255, 255);
					if (strlen(cStr2) > 0)
					{
						iLoc += 15;
						PutAlignedString(sX + 70, sX + szX, sY + iLoc, cStr2, 150, 150, 150);
					}
					if (strlen(cStr3) > 0)
					{
						iLoc += 15;
						PutAlignedString(sX + 70, sX + szX, sY + iLoc, cStr3, 150, 150, 150);
					}
					if (m_pBankList[i]->m_sLevelLimit != 0 && m_pBankList[i]->m_dwAttribute & 0x00000001)
					{
						iLoc += 15;
						wsprintf(G_cTxt, "%s: %d", DRAW_DIALOGBOX_SHOP24, m_pBankList[i]->m_sLevelLimit);
						PutAlignedString(sX + 70, sX + szX, sY + iLoc, G_cTxt, 150, 150, 150);
					}
					/*if ((m_pBankList[i]->m_cEquipPos != DEF_EQUIPPOS_NONE) && (m_pBankList[i]->m_wWeight >= 1100))
					{
						iLoc += 15;
						int _wWeight = 0;
						if (m_pBankList[i]->m_wWeight % 100)
							_wWeight = 1;
						wsprintf(G_cTxt, DRAW_DIALOGBOX_SHOP15, (m_pBankList[i]->m_wWeight / 100) + _wWeight);
						PutAlignedString(sX + 70, sX + szX, sY + iLoc, G_cTxt, 150, 150, 150);
					}*/
					cItemColor = m_pBankList[i]->m_cItemColor;
					if (cItemColor == 0)
					{
						m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_pBankList[i]->m_sSprite]->PutSpriteFast(sX + 60, sY + 68, m_pBankList[i]->m_sSpriteFrame, m_dwCurTime);
					}
					else
					{
						if ((m_pBankList[i]->m_cEquipPos == DEF_EQUIPPOS_LHAND) || (m_pBankList[i]->m_cEquipPos == DEF_EQUIPPOS_RHAND) || (m_pBankList[i]->m_cEquipPos == DEF_EQUIPPOS_TWOHAND))
						{
							m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_pBankList[i]->m_sSprite]->PutSpriteRGB(sX + 60, sY + 68, m_pBankList[i]->m_sSpriteFrame, m_wWR[cItemColor] - m_wR[0], m_wWG[cItemColor] - m_wG[0], m_wWB[cItemColor] - m_wB[0], m_dwCurTime);
						}
						else m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_pBankList[i]->m_sSprite]->PutSpriteRGB(sX + 60, sY + 68, m_pBankList[i]->m_sSpriteFrame, m_wR[cItemColor] - m_wR[0], m_wG[cItemColor] - m_wG[0], m_wB[cItemColor] - m_wB[0], m_dwCurTime);
					}
				}
				else
				{
					if (m_bIsSpecial)
						if (m_bIsRare)
						{
							PutAlignedString(sX, sX + szX, sY + 110 + i * 15, cStr1, 255, 208, 60);
						}
						else PutAlignedString(sX, sX + szX, sY + 110 + j * 15, cStr1, 0, 255, 50);
					else PutAlignedString(sX, sX + szX, sY + 110 + j * 15, cStr1, 0, 0, 0);
				}
				j++;
			}
		}
		if (bFlag == false)
		{
			PutAlignedString(sX, sX + szX, sY + 45, DRAW_DIALOGBOX_BANK3);
			PutAlignedString(sX, sX + szX, sY + 60, DRAW_DIALOGBOX_BANK4);
			PutAlignedString(sX, sX + szX, sY + 75, DRAW_DIALOGBOX_BANK5);
		}
		break;
	}
}


// Centu - Guild Warehouse
void CGame::DrawDialogBox_GuildBank(short msX, short msY, short msZ, char cLB)
{
	short sX, sY, szX;
	int  i, iTotalLines, iPointerLoc, iLoc;
	float d1, d2, d3;
	char cItemColor, cStr1[64], cStr2[64], cStr3[64], cStr4[64], cStr5[64], cStr6[64];
	bool bFlag = false;

	sX = m_stDialogBoxInfo[58].sX;
	sY = m_stDialogBoxInfo[58].sY;
	szX = m_stDialogBoxInfo[58].sSizeX - 5;

	DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME6, sX, sY, 2);
	DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_TEXT2, sX, sY, 21);
	ZeroMemory(cStr1, sizeof(cStr1));
	ZeroMemory(cStr2, sizeof(cStr2));
	ZeroMemory(cStr3, sizeof(cStr3));
	ZeroMemory(cStr4, sizeof(cStr4));
	ZeroMemory(cStr5, sizeof(cStr5));
	ZeroMemory(cStr6, sizeof(cStr6));
	iLoc = 45;

	switch (m_stDialogBoxInfo[58].cMode) {
	case -1:
		PutString(sX + 30 + 15, sY + 70, DRAW_DIALOGBOX_BANK1, RGB(0, 0, 0));//"
		PutString(sX + 30 + 15, sY + 85, DRAW_DIALOGBOX_BANK2, RGB(0, 0, 0));//"
		break;

	case 0:
		for (i = 0; i < m_stDialogBoxInfo[58].sV1; i++)
			if ((m_pBankList[i + m_stDialogBoxInfo[58].sView] != 0) && ((i + m_stDialogBoxInfo[58].sView) < DEF_MAXBANKITEMS)) {
				GetItemName(m_pBankList[i + m_stDialogBoxInfo[58].sView], cStr1, cStr2, cStr3, cStr4, cStr5, cStr6);
				if ((msX > sX + 30) && (msX < sX + 210) && (msY >= sY + 110 + i * 15) && (msY <= sY + 124 + i * 15))
				{
					bFlag = true;

					PutAlignedString(sX, sX + szX, sY + 110 + i * 15, cStr1, 255, 255, 255);
					if (m_bIsSpecial)
						if (m_bIsRare)
						{
							PutAlignedString(sX + 70, sX + szX, sY + iLoc, cStr1, 255, 208, 60);
						}
						else PutAlignedString(sX + 70, sX + szX, sY + iLoc, cStr1, 0, 255, 50);
					else
						PutAlignedString(sX + 70, sX + szX, sY + iLoc, cStr1, 255, 255, 255);

					if (strlen(cStr2) > 0)
					{
						iLoc += 15;
						PutAlignedString(sX + 70, sX + szX, sY + iLoc, cStr2, 150, 150, 150);
					}
					if (strlen(cStr3) > 0)
					{
						iLoc += 15;
						PutAlignedString(sX + 70, sX + szX, sY + iLoc, cStr3, 150, 150, 150);
					}
					if (m_pBankList[i + m_stDialogBoxInfo[58].sView]->m_sLevelLimit != 0 && m_pBankList[i + m_stDialogBoxInfo[58].sView]->m_dwAttribute & 0x00000001)
					{
						iLoc += 15;
						wsprintf(G_cTxt, "%s: %d", DRAW_DIALOGBOX_SHOP24, m_pBankList[i + m_stDialogBoxInfo[58].sView]->m_sLevelLimit);
						PutAlignedString(sX + 70, sX + szX, sY + iLoc, G_cTxt, 150, 150, 150);
					}

					/*if ((m_pBankList[i + m_stDialogBoxInfo[58].sView]->m_cEquipPos != DEF_EQUIPPOS_NONE) && (m_pBankList[i + m_stDialogBoxInfo[58].sView]->m_wWeight >= 1100))
					{
						iLoc += 15;
						int		_wWeight = 0;
						if (m_pBankList[i + m_stDialogBoxInfo[58].sView]->m_wWeight % 100)				_wWeight = 1;
						wsprintf(G_cTxt, DRAW_DIALOGBOX_SHOP15, m_pBankList[i + m_stDialogBoxInfo[58].sView]->m_wWeight / 100 + _wWeight);

						PutAlignedString(sX + 70, sX + szX, sY + iLoc, G_cTxt, 150, 150, 150);
					}*/

					cItemColor = m_pBankList[i + m_stDialogBoxInfo[58].sView]->m_cItemColor;
					if (cItemColor == 0)
					{
						m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_pBankList[i + m_stDialogBoxInfo[58].sView]->m_sSprite]->PutSpriteFast(sX + 60, sY + 68, m_pBankList[i + m_stDialogBoxInfo[58].sView]->m_sSpriteFrame, m_dwCurTime);
					}
					else
					{
						if ((m_pBankList[i + m_stDialogBoxInfo[58].sView]->m_cEquipPos == DEF_EQUIPPOS_LHAND)
							|| (m_pBankList[i + m_stDialogBoxInfo[58].sView]->m_cEquipPos == DEF_EQUIPPOS_RHAND)
							|| (m_pBankList[i + m_stDialogBoxInfo[58].sView]->m_cEquipPos == DEF_EQUIPPOS_TWOHAND))
						{
							m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_pBankList[i + m_stDialogBoxInfo[58].sView]->m_sSprite]->PutSpriteRGB(sX + 60, sY + 68, m_pBankList[i + m_stDialogBoxInfo[58].sView]->m_sSpriteFrame,
								m_wWR[cItemColor] - m_wR[0], m_wWG[cItemColor] - m_wG[0], m_wWB[cItemColor] - m_wB[0], m_dwCurTime);
						}
						else m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_pBankList[i + m_stDialogBoxInfo[58].sView]->m_sSprite]->PutSpriteRGB(sX + 60, sY + 68, m_pBankList[i + m_stDialogBoxInfo[58].sView]->m_sSpriteFrame,
							m_wR[cItemColor] - m_wR[0], m_wG[cItemColor] - m_wG[0], m_wB[cItemColor] - m_wB[0], m_dwCurTime);
					}
				}
				else
				{
					if (m_bIsSpecial)
						PutAlignedString(sX, sX + szX, sY + 110 + i * 15, cStr1, 0, 255, 50);
					else
						PutAlignedString(sX, sX + szX, sY + 110 + i * 15, cStr1, 0, 0, 0);
				}
			}

		iTotalLines = 0;
		for (i = 0; i < 1000; i++)
			if (m_pBankList[i] != 0) iTotalLines++;
		if (iTotalLines > m_stDialogBoxInfo[58].sV1)
		{
			d1 = (float)m_stDialogBoxInfo[58].sView;
			d2 = (float)(iTotalLines - m_stDialogBoxInfo[58].sV1);
			d3 = (274.0f * d1) / d2;
			iPointerLoc = (int)d3;
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME6, sX, sY, 3);
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME6, sX + 242, sY + iPointerLoc + 35, 7);
		}
		else iPointerLoc = 0;
		if (cLB != 0 && (iGetTopDialogBoxIndex() == 58) && iTotalLines > m_stDialogBoxInfo[58].sV1) {
			if ((msX >= sX + 230) && (msX <= sX + 260) && (msY >= sY + 40) && (msY <= sY + 320)) {
				d1 = (float)(msY - (sY + 35));
				d2 = (float)(iTotalLines - m_stDialogBoxInfo[58].sV1);
				d3 = (d1 * d2) / 274.0f;
				m_stDialogBoxInfo[58].sView = (int)(d3 + 0.5f);
			}
			else if ((msX >= sX + 230) && (msX <= sX + 260) && (msY > sY + 10) && (msY < sY + 40)) m_stDialogBoxInfo[58].sView = 0;
		}
		else m_stDialogBoxInfo[58].bIsScrollSelected = false;
		if (iGetTopDialogBoxIndex() == 58 && msZ != 0)
		{
			if (iTotalLines > 50) m_stDialogBoxInfo[58].sView = m_stDialogBoxInfo[58].sView - msZ / 30;
			else {//
				if (msZ > 0) m_stDialogBoxInfo[58].sView--;
				if (msZ < 0) m_stDialogBoxInfo[58].sView++;
			}
			m_DInput.m_sZ = 0;
		}
		if (iTotalLines > m_stDialogBoxInfo[58].sV1&& m_stDialogBoxInfo[58].sView > iTotalLines - m_stDialogBoxInfo[58].sV1) m_stDialogBoxInfo[58].sView = iTotalLines - m_stDialogBoxInfo[58].sV1;
		if (iTotalLines <= m_stDialogBoxInfo[58].sV1) m_stDialogBoxInfo[58].sView = 0;
		if (m_stDialogBoxInfo[58].sView < 0) m_stDialogBoxInfo[58].sView = 0;
		if (bFlag == false)
		{
			PutAlignedString(sX, sX + szX, sY + 45, DRAW_DIALOGBOX_BANK3);
			PutAlignedString(sX, sX + szX, sY + 60, DRAW_DIALOGBOX_BANK4);
			PutAlignedString(sX, sX + szX, sY + 75, DRAW_DIALOGBOX_BANK5);
		}
		break;
	}
}

bool CGame::bDlgBoxPress_Inventory(short msX, short msY)
{
	int i;
	char  cItemID;
	short sX, sY, x1, x2, y1, y2;



	if (m_bIsDialogEnabled[2] == false) return false;
	if (m_bIsDialogEnabled[17] == true) return false;
	if (m_bIsDialogEnabled[4] == true) return false;

	sX = m_stDialogBoxInfo[2].sX;
	sY = m_stDialogBoxInfo[2].sY;

	for (i = 0; i < DEF_MAXITEMS; i++)
		if (m_cItemOrder[DEF_MAXITEMS - 1 - i] != -1) {
			cItemID = m_cItemOrder[DEF_MAXITEMS - 1 - i];

			if (m_pItemList[cItemID] != 0)
			{
				m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_pItemList[cItemID]->m_sSprite]->_GetSpriteRect(sX + 32 + m_pItemList[cItemID]->m_sX,
					sY + 44 + m_pItemList[cItemID]->m_sY, m_pItemList[cItemID]->m_sSpriteFrame);
				x1 = (short)m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_pItemList[cItemID]->m_sSprite]->m_rcBound.left;
				y1 = (short)m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_pItemList[cItemID]->m_sSprite]->m_rcBound.top;
				x2 = (short)m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_pItemList[cItemID]->m_sSprite]->m_rcBound.right;
				y2 = (short)m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_pItemList[cItemID]->m_sSprite]->m_rcBound.bottom;
				if ((m_bIsItemDisabled[cItemID] == false) && (m_bIsItemEquipped[cItemID] == false) && (msX > x1) && (msX < x2) && (msY > y1) && (msY < y2)) {

					if (m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_pItemList[cItemID]->m_sSprite]->_bCheckCollison(sX + 32 + m_pItemList[cItemID]->m_sX, sY + 44 + m_pItemList[cItemID]->m_sY, m_pItemList[cItemID]->m_sSpriteFrame, msX, msY) == true)
					{
						_SetItemOrder(0, cItemID);
						if ((m_bIsGetPointingMode == true) && (m_iPointCommandType < 100) && (m_iPointCommandType >= 0)
							&& (m_pItemList[m_iPointCommandType] != 0)
							&& (m_pItemList[m_iPointCommandType]->m_cItemType == DEF_ITEMTYPE_USE_DEPLETE_DEST)
							&& (m_iPointCommandType != cItemID))
						{
							PointCommandHandler(0, 0, cItemID);
							m_bIsGetPointingMode = false;
						}
						else
						{
							m_stMCursor.cSelectedObjectType = DEF_SELECTEDOBJTYPE_ITEM;
							m_stMCursor.sSelectedObjectID = cItemID;
							m_stMCursor.sDistX = msX - x1 + (short)m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_pItemList[cItemID]->m_sSprite]->m_sPivotX;
							m_stMCursor.sDistY = msY - y1 + (short)m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_pItemList[cItemID]->m_sSprite]->m_sPivotY;
						}
						return true;
					}
				}
			}
		}
	return false;
}

void CGame::_SetItemOrder(char cWhere, char cItemID)
{
	int i;

	switch (cWhere) {
	case 0:
		for (i = 0; i < DEF_MAXITEMS; i++)
			if (m_cItemOrder[i] == cItemID)
				m_cItemOrder[i] = -1;

		for (i = 1; i < DEF_MAXITEMS; i++)
			if ((m_cItemOrder[i - 1] == -1) && (m_cItemOrder[i] != -1)) {
				m_cItemOrder[i - 1] = m_cItemOrder[i];
				m_cItemOrder[i] = -1;
			}

		for (i = 0; i < DEF_MAXITEMS; i++)
			if (m_cItemOrder[i] == -1) {
				m_cItemOrder[i] = cItemID;
				return;
			}
		break;
	}
}

void CGame::ClearGuildNameList()
{
	for (int i = 0; i < DEF_MAXGUILDNAMES; i++) {
		m_stGuildName[i].dwRefTime = 0;
		m_stGuildName[i].iGuildRank = -1;
		ZeroMemory(m_stGuildName[i].cCharName, sizeof(m_stGuildName[i].cCharName));
		ZeroMemory(m_stGuildName[i].cGuildName, sizeof(m_stGuildName[i].cGuildName));
	}
}

void CGame::InitGameSettings()
{
	int i;

	m_sZerk = m_sInv = m_sPfm = m_sPfa = m_sShield = 0;
	bZerk = bInv = bPfm = bPfa = bShield = bFreeze = bBlood = false;

	m_bForceAttack = false;

	m_bShowParty = true; // vamp - party info
	m_bShowEmblems = true;

	m_dwCommandTime = 0;

	m_bInputStatus = false;
	m_pInputBuffer = 0;

	m_iPDBGSdivX = 0;
	m_iPDBGSdivY = 0;
	m_bIsRedrawPDBGS = true;

	m_iCameraShakingDegree = 0;

	m_cCommand = DEF_OBJECTSTOP;
	m_cCommandCount = 0;

	//m_bHackMoveBlocked = false;
	m_bIsPolymorph = false;

	m_bIsGetPointingMode = false;
	m_iPointCommandType = -1; //v2.15 0 -> -1

	m_bIsCombatMode = false;
	m_bRunningMode = true;

	m_stMCursor.cPrevStatus = DEF_CURSORSTATUS_NULL;
	m_stMCursor.dwSelectClickTime = 0;

	m_bSkillUsingStatus = false;
	m_bItemUsingStatus = false;
	m_bUsingSlate = false;

	m_bIsWhetherEffect = false;
	m_cWhetherEffectType = 0;

	m_iDownSkillIndex = -1;
	m_stDialogBoxInfo[15].bFlag = false;

	m_bIsConfusion = false;

	m_iIlusionOwnerH = 0;
	m_cIlusionOwnerType = 0;

	m_iDrawFlag = 0;
	m_bDrawFlagDir = false;
	m_bIsCrusadeMode = false;
	m_iCrusadeDuty = 0;

	m_bIsCTFMode = false;
	m_bIsHeldenian = false;

	//Magn0S:: New Variables
	m_bApocalypse = false;
	m_bAttackMode = false;

	m_iNetLagCount = 0;

	m_dwEnvEffectTime = timeGetTime();

	for (i = 0; i < DEF_MAXGUILDNAMES; i++) {
		m_stGuildName[i].dwRefTime = 0;
		m_stGuildName[i].iGuildRank = -1;
		ZeroMemory(m_stGuildName[i].cCharName, sizeof(m_stGuildName[i].cCharName));
		ZeroMemory(m_stGuildName[i].cGuildName, sizeof(m_stGuildName[i].cGuildName));
	}
	//Snoopy: 61
	for (i = 0; i < 61; i++)
		m_bIsDialogEnabled[i] = false;

	//Snoopy: 58 because 2 last ones alreaddy defined / Magn0S Increased dialog boxes
	for (i = 0; i < 58; i++)
		m_cDialogBoxOrder[i] = 0;

	for (i = 0; i < DEF_MAXEFFECTS; i++) {
		if (m_pEffectList[i] != 0) delete m_pEffectList[i];
		m_pEffectList[i] = 0;
	}

	for (i = 0; i < DEF_MAXCHATMSGS; i++) {
		if (m_pChatMsgList[i] != 0) delete m_pChatMsgList[i];
		m_pChatMsgList[i] = 0;
	}

	for (i = 0; i < DEF_MAXCHATSCROLLMSGS; i++) {
		if (m_pChatScrollList[i] != 0) delete m_pChatScrollList[i];
		m_pChatScrollList[i] = 0;
	}

	for (i = 0; i < DEF_MAXWHISPERMSG; i++) {
		if (m_pWhisperMsg[i] != 0) delete m_pWhisperMsg[i];
		m_pWhisperMsg[i] = 0;
	}

	ZeroMemory(m_cLocation, sizeof(m_cLocation));

	ZeroMemory(m_cGuildName, sizeof(m_cGuildName));
	m_iGuildRank = -1;
	m_iTotalGuildsMan = 0;


	for (i = 0; i < 100; i++) {
		m_stGuildOpList[i].cOpMode = 0;
		ZeroMemory(m_stGuildOpList[i].cName, sizeof(m_stGuildOpList[i].cName));
	}

	for (i = 0; i < 6; i++) {
		ZeroMemory(m_stEventHistory[i].cTxt, sizeof(m_stEventHistory[i].cTxt));
		m_stEventHistory[i].dwTime = G_dwGlobalTime;

		ZeroMemory(m_stEventHistory2[i].cTxt, sizeof(m_stEventHistory2[i].cTxt));
		m_stEventHistory2[i].dwTime = G_dwGlobalTime;
	}

	for (i = 0; i < DEF_MAXMENUITEMS; i++) {
		if (m_pItemForSaleList[i] != 0) delete m_pItemForSaleList[i];
		m_pItemForSaleList[i] = 0;
	}

	// VAMP - online users list
	for (i = 0; i < DEF_MAXCLIENTS; i++) {
		if (m_pOnlineUsersList[i] != 0) delete m_pOnlineUsersList[i];
		m_pOnlineUsersList[i] = 0;
	}

	for (i = 0; i < 41; i++) {
		m_stDialogBoxInfo[i].bFlag = false;
		m_stDialogBoxInfo[i].sView = 0;
		m_stDialogBoxInfo[i].bIsScrollSelected = false;
	}

	for (i = 0; i < DEF_MAXITEMS; i++)
		if (m_pItemList[i] != 0) {
			delete m_pItemList[i];
			m_pItemList[i] = 0;
		}

	for (i = 0; i < DEF_MAXSELLLIST; i++) {
		m_stSellItemList[i].iIndex = -1;
		m_stSellItemList[i].iAmount = 0;
	}

	for (i = 0; i < DEF_MAXBANKITEMS; i++)
		if (m_pBankList[i] != 0) {
			delete m_pBankList[i];
			m_pBankList[i] = 0;
		}

	for (i = 0; i < DEF_MAXMAGICTYPE; i++)
		m_cMagicMastery[i] = 0;

	for (i = 0; i < DEF_MAXSKILLTYPE; i++)
		m_cSkillMastery[i] = 0;

	for (i = 0; i < DEF_TEXTDLGMAXLINES; i++) {
		if (m_pMsgTextList[i] != 0)
			delete m_pMsgTextList[i];
		m_pMsgTextList[i] = 0;

		if (m_pMsgTextList2[i] != 0)
			delete m_pMsgTextList2[i];
		m_pMsgTextList2[i] = 0;

		if (m_pAgreeMsgTextList[i] != 0)
			delete m_pAgreeMsgTextList[i];
		m_pAgreeMsgTextList[i] = 0;
	}

	for (i = 0; i < DEF_MAXPARTYMEMBERS; i++) {
		m_stPartyMember[i].cStatus = 0;
		ZeroMemory(m_stPartyMember[i].cName, sizeof(m_stPartyMember[i].cName));
	}

	m_iLU_Point = 0;
	m_cLU_Str = m_cLU_Vit = m_cLU_Dex = m_cLU_Int = m_cLU_Mag = m_cLU_Char = 0;
	m_bDialogTrans   = false;
	m_cWhetherStatus = 0;
	m_cLogOutCount = -1;
	m_dwLogOutCountTime = 0;
	m_iSuperAttackLeft = 0;
	m_bSuperAttackMode = false;
	m_iFightzoneNumber = 0;
	ZeroMemory(m_cBGMmapName, sizeof(m_cBGMmapName));
	m_dwWOFtime = 0;

/*	m_stQuest.sWho = 0;
	m_stQuest.sQuestType = 0;
	m_stQuest.sContribution = 0;
	m_stQuest.sTargetType = 0;
	m_stQuest.sTargetCount = 0;
	m_stQuest.sCurrentCount = 0;
	m_stQuest.sX = 0;
	m_stQuest.sY = 0;
	m_stQuest.sRange = 0;
	m_stQuest.bIsQuestCompleted = false;
	ZeroMemory(m_stQuest.cTargetName, sizeof(m_stQuest.cTargetName));*/

	m_bIsObserverMode = false;
	m_bIsObserverCommanded = false;
	m_bIsPoisoned = false;
	m_bIsPrevMoveBlocked = false;
	m_iPrevMoveX = m_iPrevMoveY = -1;
	m_sDamageMove = 0;
	m_sDamageMoveAmount = 0;
	m_bForceDisconn = false;
	m_bIsSpecialAbilityEnabled = false;
	m_iSpecialAbilityType = 0;
	m_dwSpecialAbilitySettingTime = 0;
	m_iSpecialAbilityTimeLeftSec = 0;
	m_stMCursor.cSelectedObjectType = 0;
	m_bIsF1HelpWindowEnabled = false;
	m_bIsTeleportRequested = false;
	for (i = 0; i < DEF_MAXCRUSADESTRUCTURES; i++)
	{
		m_stCrusadeStructureInfo[i].cType = 0;
		m_stCrusadeStructureInfo[i].cSide = 0;
		m_stCrusadeStructureInfo[i].sX = 0;
		m_stCrusadeStructureInfo[i].sY = 0;
	}
	ZeroMemory(m_cStatusMapName, sizeof(m_cStatusMapName));
	m_dwCommanderCommandRequestedTime = 0;
	ZeroMemory(m_cTopMsg, sizeof(m_cTopMsg));
	m_iTopMsgLastSec = 0;
	m_dwTopMsgTime = 0;
	m_iConstructionPoint = 0;
	m_iWarContribution = 0;
	ZeroMemory(m_cTeleportMapName, sizeof(m_cTeleportMapName));
	m_iTeleportLocX = m_iTeleportLocY = -1;
	ZeroMemory(m_cConstructMapName, sizeof(m_cConstructMapName));
	m_iConstructLocX = m_iConstructLocY = -1;

	//Snoopy: Apocalypse Gate
	ZeroMemory(m_cGateMapName, sizeof(m_cGateMapName));
	m_iGatePositX = m_iGatePositY = -1;
	m_iHeldenianAresdenLeftTower = -1;
	m_iHeldenianElvineLeftTower = -1;
	m_iHeldenianAresdenFlags = -1;
	m_iHeldenianElvineFlags = -1;
	m_iHeldenianAresdenDead = -1;
	m_iHeldenianElvineDead = -1;

	m_bIsXmas = false;

	m_iTotalPartyMember = 0;
	m_iPartyStatus = 0;
	//New Party Status - ZeroEoyPnk - 06/09/2010
	ActualizarParty = false;
	for (i = 0; i < DEF_MAXPARTYMEMBERS; i++)
	{

		ZeroMemory(m_stPartyMemberNameList[i].cName, sizeof(m_stPartyMemberNameList[i].cName));
		iPartyHp[i] = 0;
		iMaxPoint2[i] = 0;
		iBarWidth2[i] = 0;
		iParty_sX[i] = 0;
		iParty_sY[i] = 0;
		iPartyMp[i] = 0;
		iMaxPoint0[i] = 0;
		iBarWidth0[i] = 0;


	}
	m_iGizonItemUpgradeLeft = 0;
	cStateChange1 = 0;
	cStateChange2 = 0;
	cStateChange3 = 0;

	EnableDialogBox(9, 0, 0, 0);
	
	//Magn0S:: Quest List
	for (i = 0; i < 50; i++)
	{
		//if (strlen(m_stQuestList[i].cMapName) > 0)
		//{
			m_stQuestList[i].iQuestID = 0;
			m_stQuestList[i].iContribution = 0;
			m_stQuestList[i].iMaxCount = 0;
			m_stQuestList[i].iNpcID = 0;

			ZeroMemory(m_stQuestList[i].cPrizeName, sizeof(m_stQuestList[i].cPrizeName));
			ZeroMemory(m_stQuestList[i].cMapName, sizeof(m_stQuestList[i].cMapName));
		//}
	}

	//Magn0S:: Multi Quest
	for (i = 0; i < DEF_MAXQUEST; i++)
	{
		m_stQuest[i].iQuestID = 0;
		m_stQuest[i].sWho = 0;
		m_stQuest[i].sQuestType = 0;
		m_stQuest[i].sContribution = 0;
		m_stQuest[i].sTargetType = 0;
		m_stQuest[i].sTargetCount = 0;
		m_stQuest[i].sX = 0;
		m_stQuest[i].sY = 0;
		m_stQuest[i].sRange = 0;
		m_stQuest[i].sCurrentCount = 0;
		m_stQuest[i].bIsQuestCompleted = false;
		m_stQuest[i].iAmount = 0;
		m_stQuest[i].iRewardType = 0;

		ZeroMemory(m_stQuest[i].cTargetName, sizeof(m_stQuest[i].cTargetName));
		ZeroMemory(m_stQuest[i].cPrizeName, sizeof(m_stQuest[i].cPrizeName));
	}

	for (i = 0; i < 10; i++) m_bNullDrop[i] = true;
}

void CGame::_GetHairColorRGB(int iColorType, int * pR, int * pG, int * pB)
{
	switch (iColorType) {
	case 0: // rouge fonc�
		*pR = 14; *pG = -5; *pB = -5; break;
	case 1: // Orange
		*pR = 20; *pG = 0; *pB = 0; break;
	case 2: // marron tres clair
		*pR = 22; *pG = 13; *pB = -10; break;
	case 3: // vert
		*pR = 0; *pG = 10; *pB = 0; break;
	case 4: // Bleu flashy
		*pR = 0; *pG = 0; *pB = 22; break;
	case 5: // Bleu fonc�
		*pR = -5; *pG = -5; *pB = 15; break;
	case 6: //Mauve
		*pR = 15; *pG = -5; *pB = 16; break;
	case 7: // Noir
		*pR = -6; *pG = -6; *pB = -6; break;
	case 8:
		*pR = 10; *pG = 3; *pB = 10; break;
	case 9:
		*pR = 10; *pG = 3; *pB = -10; break;
	case 10:
		*pR = -10; *pG = 3; *pB = 10; break;
	case 11:
		*pR = 10; *pG = 3; *pB = 20; break;
	case 12:
		*pR = 21; *pG = 3; *pB = 3; break;
	case 13:
		*pR = 3; *pG = 3; *pB = 25; break;
	case 14:
		*pR = 3; *pG = 11; *pB = 3; break;
	case 15:
		*pR = 6; *pG = 8; *pB = 0; break;
	}
}

void CGame::DlgBoxClick_GuildMenu(short msX, short msY)
{
	short sX, sY;
	char cTemp[21];
	int iAdjX, iAdjY;
	sX = m_stDialogBoxInfo[7].sX;
	sY = m_stDialogBoxInfo[7].sY;

	iAdjX = -13;
	iAdjY = 30;

	switch (m_stDialogBoxInfo[7].cMode) {
	case 0:
		if ((msX > sX + iAdjX + 80) && (msX < sX + iAdjX + 210) && (msY > sY + iAdjY + 63) && (msY < sY + iAdjY + 78))
		{
			if (m_iGuildRank != -1) return;
			//if (m_iCharisma < 20) return;
			if (m_iLevel < 100) return;
			if (m_bIsCrusadeMode) return;
			if (m_bIsHeldenian) return;
			EndInputString();
			StartInputString(sX + 75, sY + 140, 21, m_cGuildName);
			m_stDialogBoxInfo[7].cMode = 1;
			PlaySound('E', 14, 5);
		}
		if ((msX > sX + iAdjX + 72) && (msX < sX + iAdjX + 222) && (msY > sY + iAdjY + 82) && (msY < sY + iAdjY + 99))
		{
			if (m_iGuildRank != 0) return;
			if (m_bIsCrusadeMode) return;
			if (m_bIsHeldenian) return;
			m_stDialogBoxInfo[7].cMode = 5;
			PlaySound('E', 14, 5);
		}
		if ((msX > sX + iAdjX + 61) && (msX < sX + iAdjX + 226) && (msY > sY + iAdjY + 103) && (msY < sY + iAdjY + 120))
		{
			m_stDialogBoxInfo[7].cMode = 9;
			PlaySound('E', 14, 5);
		}
		if ((msX > sX + iAdjX + 60) && (msX < sX + iAdjX + 227) && (msY > sY + iAdjY + 123) && (msY < sY + iAdjY + 139))
		{
			m_stDialogBoxInfo[7].cMode = 11;
			PlaySound('E', 14, 5);
		}
		/*if (m_iFightzoneNumber < 0) break;
		if ((msX > sX + iAdjX + 72) && (msX < sX + iAdjX + 228) && (msY > sY + iAdjY + 143) && (msY < sY + iAdjY + 169))
		{
			if (m_iGuildRank != 0) return;
			if (m_iFightzoneNumber == 0)	m_stDialogBoxInfo[7].cMode = 13;
			else  m_stDialogBoxInfo[7].cMode = 19;
			PlaySound('E', 14, 5);
		}*/
		break;
	case 1:
		if ((msX >= sX + 30) && (msX <= sX + 30 + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
		{	// Submit
			if (strcmp(m_cGuildName, "NONE") == 0) return;
			if (strlen(m_cGuildName) == 0) return;
			bSendCommand(MSGID_REQUEST_CREATENEWGUILD, DEF_MSGTYPE_CONFIRM, 0, 0, 0, 0, 0);
			m_stDialogBoxInfo[7].cMode = 2;
			EndInputString();
			PlaySound('E', 14, 5);
		}
		if ((msX >= sX + 154) && (msX <= sX + 154 + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
		{	// Cancel
			m_stDialogBoxInfo[7].cMode = 0;
			EndInputString();
			PlaySound('E', 14, 5);
		}
		break;

	case 3:
	case 4:
	case 7:
	case 8:
	case 10:
	case 12:
		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY)) {
			m_stDialogBoxInfo[7].cMode = 0;
			PlaySound('E', 14, 5);
		}
		break;

	case 9:
		if ((msX >= sX + 30) && (msX <= sX + 30 + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
		{	// Purchase�
			ZeroMemory(cTemp, sizeof(cTemp));
			strcpy(cTemp, "GuildAdmissionTicket");
			bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_REQ_PURCHASEITEM, 0, 1, 0, 0, cTemp);
			m_stDialogBoxInfo[7].cMode = 0;
			PlaySound('E', 14, 5);
		}
		if ((msX >= sX + 154) && (msX <= sX + 154 + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
		{	// Cancel
			m_stDialogBoxInfo[7].cMode = 0;
			PlaySound('E', 14, 5);
		}
		break;

	case 11:
		if ((msX >= sX + 30) && (msX <= sX + 30 + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
		{	// Purchase
			ZeroMemory(cTemp, sizeof(cTemp));
			strcpy(cTemp, "GuildSecessionTicket");
			bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_REQ_PURCHASEITEM, 0, 1, 0, 0, cTemp);
			m_stDialogBoxInfo[7].cMode = 0;
			PlaySound('E', 14, 5);
		}
		if ((msX >= sX + 154) && (msX <= sX + 154 + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
		{	// Cancel
			m_stDialogBoxInfo[7].cMode = 0;
			PlaySound('E', 14, 5);
		}
		break;

	case 5:
		if ((msX >= sX + 30) && (msX <= sX + 30 + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY)) {
			// Confirm
			bSendCommand(MSGID_REQUEST_DISBANDGUILD, DEF_MSGTYPE_CONFIRM, 0, 0, 0, 0, 0);
			m_stDialogBoxInfo[7].cMode = 6;
			PlaySound('E', 14, 5);
		}
		if ((msX >= sX + 154) && (msX <= sX + 154 + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY)) {
			// Cancel
			m_stDialogBoxInfo[7].cMode = 0;
			PlaySound('E', 14, 5);
		}
		break;

	case 13:
		if ((msX > sX + iAdjX + 65) && (msX < sX + iAdjX + 137) && (msY > sY + iAdjY + 168) && (msY < sY + iAdjY + 185))
		{
			bSendCommand(MSGID_REQUEST_FIGHTZONE_RESERVE, 0, 0, 1, 0, 0, 0);
			m_stDialogBoxInfo[7].cMode = 18;
			m_iFightzoneNumberTemp = 1;
			PlaySound('E', 14, 5);
		}
		if ((msX > sX + iAdjX + 150) && (msX < sX + iAdjX + 222) && (msY > sY + iAdjY + 168) && (msY < sY + iAdjY + 185))
		{
			bSendCommand(MSGID_REQUEST_FIGHTZONE_RESERVE, 0, 0, 2, 0, 0, 0);
			m_stDialogBoxInfo[7].cMode = 18;
			m_iFightzoneNumberTemp = 2;
			PlaySound('E', 14, 5);
		}
		if ((msX > sX + iAdjX + 65) && (msX < sX + iAdjX + 137) && (msY > sY + iAdjY + 188) && (msY < sY + iAdjY + 205))
		{
			bSendCommand(MSGID_REQUEST_FIGHTZONE_RESERVE, 0, 0, 3, 0, 0, 0);
			m_stDialogBoxInfo[7].cMode = 18;
			m_iFightzoneNumberTemp = 3;
			PlaySound('E', 14, 5);
		}
		if ((msX > sX + iAdjX + 150) && (msX < sX + iAdjX + 222) && (msY > sY + iAdjY + 188) && (msY < sY + iAdjY + 205))
		{
			bSendCommand(MSGID_REQUEST_FIGHTZONE_RESERVE, 0, 0, 4, 0, 0, 0);
			m_stDialogBoxInfo[7].cMode = 18;
			m_iFightzoneNumberTemp = 4;
			PlaySound('E', 14, 5);
		}
		if ((msX > sX + iAdjX + 65) && (msX < sX + iAdjX + 137) && (msY > sY + iAdjY + 208) && (msY < sY + iAdjY + 225))
		{
			bSendCommand(MSGID_REQUEST_FIGHTZONE_RESERVE, 0, 0, 5, 0, 0, 0);
			m_stDialogBoxInfo[7].cMode = 18;
			m_iFightzoneNumberTemp = 5;
			PlaySound('E', 14, 5);
		}
		if ((msX > sX + iAdjX + 150) && (msX < sX + iAdjX + 222) && (msY > sY + iAdjY + 208) && (msY < sY + iAdjY + 225))
		{
			bSendCommand(MSGID_REQUEST_FIGHTZONE_RESERVE, 0, 0, 6, 0, 0, 0);
			m_stDialogBoxInfo[7].cMode = 18;
			m_iFightzoneNumberTemp = 6;
			PlaySound('E', 14, 5);
		}
		if ((msX > sX + iAdjX + 65) && (msX < sX + iAdjX + 137) && (msY > sY + iAdjY + 228) && (msY < sY + iAdjY + 245))
		{
			bSendCommand(MSGID_REQUEST_FIGHTZONE_RESERVE, 0, 0, 7, 0, 0, 0);
			m_stDialogBoxInfo[7].cMode = 18;
			m_iFightzoneNumberTemp = 7;
			PlaySound('E', 14, 5);
		}
		if ((msX > sX + iAdjX + 150) && (msX < sX + iAdjX + 222) && (msY > sY + iAdjY + 228) && (msY < sY + iAdjY + 245))
		{
			bSendCommand(MSGID_REQUEST_FIGHTZONE_RESERVE, 0, 0, 8, 0, 0, 0);
			m_stDialogBoxInfo[7].cMode = 18;
			m_iFightzoneNumberTemp = 8;
			PlaySound('E', 14, 5);
		}
		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
		{	// Cancel
			m_stDialogBoxInfo[7].cMode = 0;
			PlaySound('E', 14, 5);
		}
		break;

	case 14://
	case 15://
	case 16://
	case 17://
	case 21://
	case 22://
		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY)) {
			// OK
			m_stDialogBoxInfo[7].cMode = 0;
			PlaySound('E', 14, 5);
		}
		break;
	}
}

void CGame::CreateNewGuildResponseHandler(char * pData)
{
	UINT16 * wpResult;
	wpResult = (UINT16 *)(pData + DEF_INDEX2_MSGTYPE);
	switch (*wpResult) {
	case DEF_MSGTYPE_CONFIRM:
		m_iGuildRank = 0;
		m_stDialogBoxInfo[7].cMode = 3;
		break;
	case DEF_MSGTYPE_REJECT:
		m_iGuildRank = -1;
		m_stDialogBoxInfo[7].cMode = 4;
		break;
	}
}

// 3.51 Level Up Dialog - Diuuude
void CGame::DrawDialogBox_LevelUpSetting(short msX, short msY)
{
	short sX, sY, szX;
	UINT32 dwTime = m_dwCurTime;
	char cTxt[120];
	int iStats;
	sX = m_stDialogBoxInfo[12].sX;
	sY = m_stDialogBoxInfo[12].sY;
	szX = m_stDialogBoxInfo[12].sSizeX;

	if (m_bUseOldPanels) {
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME6, sX, sY, 0);
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_TEXT2, sX, sY, 2);
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME8, sX + 16, sY + 100, 4);
	}
	else {
		short limitX, limitY;
		limitX = sX + m_stDialogBoxInfo[12].sSizeX;
		limitY = sY + m_stDialogBoxInfo[12].sSizeY;

		int addx = 50;
		int addy = 0;

		m_DDraw.DrawShadowBox(sX, sY, limitX, limitY, 0, true);
		if (!m_bDialogTrans) m_DDraw.DrawShadowBox(sX, sY, limitX, limitY, 0, true);

		m_DDraw.DrawShadowBox(sX, sY, limitX, sY + 25, 0, true);
		if (!m_bDialogTrans) m_DDraw.DrawShadowBox(sX, sY, limitX, sY + 25, 0, true);
		PutString_SprFont2(sX + 70, sY + 5, "Level Up Settings", 240, 240, 240);

		m_DDraw.DrawShadowBox(sX + 20 - 5 + 90 - 2, sY + 90 + 10 + 24, sX + 50 + 5 + 82, sY + 145 - 5, 1, true);
		m_DDraw.DrawShadowBox(sX + 20 - 5 + 90 - 1 + addx, sY + 90 + 10 + 24, sX + 50 + 5 + 83 + addx, sY + 145 - 5, 1, true);

		addy += 20;

		m_DDraw.DrawShadowBox(sX + 20 - 5 + 90 - 2, sY + 90 + 10 + 24 + addy, sX + 50 + 5 + 82, sY + 145 - 5 + addy, 1, true);
		m_DDraw.DrawShadowBox(sX + 20 - 5 + 90 - 1 + addx, sY + 90 + 10 + 24 + addy, sX + 50 + 5 + 83 + addx, sY + 145 - 5 + addy, 1, true);

		addy += 20;

		m_DDraw.DrawShadowBox(sX + 20 - 5 + 90 - 2, sY + 90 + 10 + 24 + addy, sX + 50 + 5 + 82, sY + 145 - 5 + addy, 1, true);
		m_DDraw.DrawShadowBox(sX + 20 - 5 + 90 - 1 + addx, sY + 90 + 10 + 24 + addy, sX + 50 + 5 + 83 + addx, sY + 145 - 5 + addy, 1, true);

		addy += 20;

		m_DDraw.DrawShadowBox(sX + 20 - 5 + 90 - 2, sY + 90 + 10 + 24 + addy, sX + 50 + 5 + 82, sY + 145 - 5 + addy, 1, true);
		m_DDraw.DrawShadowBox(sX + 20 - 5 + 90 - 1 + addx, sY + 90 + 10 + 24 + addy, sX + 50 + 5 + 83 + addx, sY + 145 - 5 + addy, 1, true);

		addy += 20;

		m_DDraw.DrawShadowBox(sX + 20 - 5 + 90 - 2, sY + 90 + 10 + 24 + addy, sX + 50 + 5 + 82, sY + 145 - 5 + addy, 1, true);
		m_DDraw.DrawShadowBox(sX + 20 - 5 + 90 - 1 + addx, sY + 90 + 10 + 24 + addy, sX + 50 + 5 + 83 + addx, sY + 145 - 5 + addy, 1, true);

		addy += 20;

		m_DDraw.DrawShadowBox(sX + 20 - 5 + 90 - 2, sY + 90 + 10 + 24 + addy, sX + 50 + 5 + 82, sY + 145 - 5 + addy, 1, true);
		m_DDraw.DrawShadowBox(sX + 20 - 5 + 90 - 1 + addx, sY + 90 + 10 + 24 + addy, sX + 50 + 5 + 83 + addx, sY + 145 - 5 + addy, 1, true);
	}

	PutAlignedString(sX, sX + szX, sY + 50, DRAW_DIALOGBOX_LEVELUP_SETTING1, 255, 255, 255);
	PutAlignedString(sX, sX + szX, sY + 65, DRAW_DIALOGBOX_LEVELUP_SETTING2, 255, 255, 255);

	// Points Left - Display in green if > 0
	PutString(sX + 20, sY + 85, DRAW_DIALOGBOX_LEVELUP_SETTING3, RGB(255, 255, 255));
	wsprintf(cTxt, "%d", m_iLU_Point);
	if (m_iLU_Point > 0)
	{
		PutString(sX + 95, sY + 85, cTxt, RGB(0, 255, 0));
	}
	else
	{
		PutString(sX + 95, sY + 85, cTxt, RGB(255, 255, 255));
	}
	// Strength
	PutString(sX + 24, sY + 125, DRAW_DIALOGBOX_LEVELUP_SETTING4, RGB(255, 255, 255));
	wsprintf(cTxt, "%d", m_iStr);
	PutString(sX + 109, sY + 125, cTxt, RGB(255, 255, 0));
	iStats = m_iStr + m_cLU_Str;
	wsprintf(cTxt, "%d", iStats);
	if (iStats != m_iStr)
	{
		PutString(sX + 162, sY + 125, cTxt, RGB(255, 0, 0));
	}
	else
	{
		PutString(sX + 162, sY + 125, cTxt, RGB(255, 255, 0));
	}
	//if ((msX >= sX + 195) && (msX <= sX + 205) && (msY >= sY + 127) && (msY <= sY + 133) && (m_iStr < DEF_STATS_LIMIT))
		m_pSprite[DEF_SPRID_INTERFACE_ND_GAME4]->PutSpriteFast(sX + 195, sY + 127, 5, dwTime);
	//if ((msX >= sX + 210) && (msX <= sX + 220) && (msY >= sY + 127) && (msY <= sY + 133) && (m_cLU_Str > 0))
		m_pSprite[DEF_SPRID_INTERFACE_ND_GAME4]->PutSpriteFast(sX + 210, sY + 127, 6, dwTime);

	// Vitality
	PutString(sX + 24, sY + 144, DRAW_DIALOGBOX_LEVELUP_SETTING5, RGB(255, 255, 255));
	wsprintf(cTxt, "%d", m_iVit);
	PutString(sX + 109, sY + 144, cTxt, RGB(255, 255, 0));
	iStats = m_iVit + m_cLU_Vit;
	wsprintf(cTxt, "%d", iStats);
	if (iStats != m_iVit)
	{
		PutString(sX + 162, sY + 144, cTxt, RGB(255, 0, 0));
	}
	else
	{
		PutString(sX + 162, sY + 144, cTxt, RGB(255, 255, 0));
	}
	//if ((msX >= sX + 195) && (msX <= sX + 205) && (msY >= sY + 146) && (msY <= sY + 152) && (m_iVit < DEF_STATS_LIMIT))
		m_pSprite[DEF_SPRID_INTERFACE_ND_GAME4]->PutSpriteFast(sX + 195, sY + 146, 5, dwTime);
	//if ((msX >= sX + 210) && (msX <= sX + 220) && (msY >= sY + 146) && (msY <= sY + 152) && (m_cLU_Vit > 0))
		m_pSprite[DEF_SPRID_INTERFACE_ND_GAME4]->PutSpriteFast(sX + 210, sY + 146, 6, dwTime);

	// Dexterity
	PutString(sX + 24, sY + 163, DRAW_DIALOGBOX_LEVELUP_SETTING6, RGB(255, 255, 255));
	wsprintf(cTxt, "%d", m_iDex);
	PutString(sX + 109, sY + 163, cTxt, RGB(255, 255, 0));
	iStats = m_iDex + m_cLU_Dex;
	wsprintf(cTxt, "%d", iStats);
	if (iStats != m_iDex)
	{
		PutString(sX + 162, sY + 163, cTxt, RGB(255, 0, 0));
	}
	else
	{
		PutString(sX + 162, sY + 163, cTxt, RGB(255, 255, 0));
	}
	//if ((msX >= sX + 195) && (msX <= sX + 205) && (msY >= sY + 165) && (msY <= sY + 171) && (m_iDex < DEF_STATS_LIMIT))
		m_pSprite[DEF_SPRID_INTERFACE_ND_GAME4]->PutSpriteFast(sX + 195, sY + 165, 5, dwTime);
	//if ((msX >= sX + 210) && (msX <= sX + 220) && (msY >= sY + 165) && (msY <= sY + 171) && (m_cLU_Dex > 0))
		m_pSprite[DEF_SPRID_INTERFACE_ND_GAME4]->PutSpriteFast(sX + 210, sY + 165, 6, dwTime);

	// Intelligence
	PutString(sX + 24, sY + 182+2, DRAW_DIALOGBOX_LEVELUP_SETTING7, RGB(255, 255, 255));
	wsprintf(cTxt, "%d", m_iInt);
	PutString(sX + 109, sY + 182+2, cTxt, RGB(255, 255, 0));
	iStats = m_iInt + m_cLU_Int;
	wsprintf(cTxt, "%d", iStats);
	if (iStats != m_iInt)
	{
		PutString(sX + 162, sY + 182+2, cTxt, RGB(255, 0, 0));
	}
	else
	{
		PutString(sX + 162, sY + 182+2, cTxt, RGB(255, 255, 0));
	}
	//if ((msX >= sX + 195) && (msX <= sX + 205) && (msY >= sY + 184) && (msY <= sY + 190) && (m_iInt < DEF_STATS_LIMIT))
		m_pSprite[DEF_SPRID_INTERFACE_ND_GAME4]->PutSpriteFast(sX + 195, sY + 184 + 2, 5, dwTime);
	//if ((msX >= sX + 210) && (msX <= sX + 220) && (msY >= sY + 184) && (msY <= sY + 190) && (m_cLU_Int > 0))
		m_pSprite[DEF_SPRID_INTERFACE_ND_GAME4]->PutSpriteFast(sX + 210, sY + 184 + 2, 6, dwTime);

	// Magic
	PutString(sX + 24, sY + 201+3, DRAW_DIALOGBOX_LEVELUP_SETTING8, RGB(255, 255, 255));
	wsprintf(cTxt, "%d", m_iMag);
	PutString(sX + 109, sY + 201+3, cTxt, RGB(255, 255, 0));
	iStats = m_iMag + m_cLU_Mag;
	wsprintf(cTxt, "%d", iStats);
	if (iStats != m_iMag)
	{
		PutString(sX + 162, sY + 201+3, cTxt, RGB(255, 0, 0));
	}
	else
	{
		PutString(sX + 162, sY + 201+3, cTxt, RGB(255, 255, 0));
	}
	//if ((msX >= sX + 195) && (msX <= sX + 205) && (msY >= sY + 203) && (msY <= sY + 209) && (m_iMag < DEF_STATS_LIMIT))
		m_pSprite[DEF_SPRID_INTERFACE_ND_GAME4]->PutSpriteFast(sX + 195, sY + 203 + 3, 5, dwTime);
	//if ((msX >= sX + 210) && (msX <= sX + 220) && (msY >= sY + 203) && (msY <= sY + 209) && (m_cLU_Mag > 0))
		m_pSprite[DEF_SPRID_INTERFACE_ND_GAME4]->PutSpriteFast(sX + 210, sY + 203 + 3, 6, dwTime);

	// Charisma
	//PutString(sX + 24, sY + 220, DRAW_DIALOGBOX_LEVELUP_SETTING9, RGB(5, 5, 5));
	PutString(sX + 24, sY + 220+3, "Agility", RGB(255, 255, 255));
	wsprintf(cTxt, "%d", m_iCharisma);
	PutString(sX + 109, sY + 220+3, cTxt, RGB(255, 255, 0));
	iStats = m_iCharisma + m_cLU_Char;
	wsprintf(cTxt, "%d", iStats);
	if (iStats != m_iCharisma)
	{
		PutString(sX + 162, sY + 220+3, cTxt, RGB(255, 0, 0));
	}
	else
	{
		PutString(sX + 162, sY + 220+3, cTxt, RGB(255, 255, 0));
	}
	//if ((msX >= sX + 195) && (msX <= sX + 205) && (msY >= sY + 222) && (msY <= sY + 228) && (m_iCharisma < DEF_STATS_LIMIT))
		m_pSprite[DEF_SPRID_INTERFACE_ND_GAME4]->PutSpriteFast(sX + 195, sY + 222 + 3, 5, dwTime);
	//if ((msX >= sX + 210) && (msX <= sX + 220) && (msY >= sY + 222) && (msY <= sY + 228) && (m_cLU_Char > 0))
		m_pSprite[DEF_SPRID_INTERFACE_ND_GAME4]->PutSpriteFast(sX + 210, sY + 222 + 3, 6, dwTime);

		if (m_bUseOldPanels) {
			if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
			else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);

			if ((m_cLU_Str == 0) && (m_cLU_Vit == 0) && (m_cLU_Dex == 0) && (m_cLU_Int == 0) && (m_cLU_Mag == 0) && (m_cLU_Char == 0))
			{
				if ((msX >= sX + DEF_LBTNPOSX) && (msX <= sX + DEF_LBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
				{
					if (m_iLU_Point <= 0) DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 21);
				}
				else
				{
					if (m_iLU_Point <= 0) DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 20);
				}
			}
		}
		else {

			if ((m_cLU_Str == 0) && (m_cLU_Vit == 0) && (m_cLU_Dex == 0) && (m_cLU_Int == 0) && (m_cLU_Mag == 0) && (m_cLU_Char == 0))
			{
				if ((msX >= sX + DEF_LBTNPOSX) && (msX <= sX + DEF_LBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
				{
					if (m_iLU_Point <= 0)
					{
						DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 0);
						PutAlignedString2(sX + DEF_LBTNPOSX + 10, sX + DEF_LBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Change", 255, 255, 100);
					}
				}
				else
				{
					if (m_iLU_Point <= 0)
					{
						DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 1);
						PutAlignedString2(sX + DEF_LBTNPOSX + 10, sX + DEF_LBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Change", 180, 188, 180);
					}
				}
			}
			if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
			{
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
				PutAlignedString2(sX + DEF_RBTNPOSX + 10, sX + DEF_RBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Ok", 255, 255, 100);
			}
			else
			{
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
				PutAlignedString2(sX + DEF_RBTNPOSX + 10, sX + DEF_RBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Ok", 180, 188, 180);
			}
			
		}

	// Centuu : Prevenci�n para que no se pase de los stats m�ximos - reintroducido del cliente original
	if (m_iLU_Point != 0)
	{
		PutAlignedString(sX, sX + szX, sY + 235+15, DRAW_DIALOGBOX_LEVELUP_SETTING18, 255, 0, 0);
	}
	else if (((m_iStr + m_cLU_Str) > DEF_STATS_LIMIT) || ((m_iDex + m_cLU_Dex) > DEF_STATS_LIMIT) ||
		((m_iVit + m_cLU_Vit) > DEF_STATS_LIMIT) || ((m_iInt + m_cLU_Int) > DEF_STATS_LIMIT) ||
		((m_iMag + m_cLU_Mag) > DEF_STATS_LIMIT) || ((m_iCharisma + m_cLU_Char) > DEF_STATS_LIMIT))
	{
		PutAlignedString(sX, sX + szX, sY + 235+15, DRAW_DIALOGBOX_LEVELUP_SETTING19, 255, 0, 0);
		PutAlignedString(sX, sX + szX, sY + 250+15, DRAW_DIALOGBOX_LEVELUP_SETTING20, 255, 0, 0);
		wsprintf(G_cTxt, DRAW_DIALOGBOX_LEVELUP_SETTING21, DEF_STATS_LIMIT);
		PutAlignedString(sX, sX + szX, sY + 265+15, G_cTxt, 255, 0, 0);
	}
}

void CGame::DrawDialogBox_CityHallMenu(short msX, short msY)
{
	short sX, sY, szX;
	char cTxt[120];
	int i;

	sX = m_stDialogBoxInfo[13].sX;
	sY = m_stDialogBoxInfo[13].sY;
	szX = m_stDialogBoxInfo[13].sSizeX;

	DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME6, sX, sY, 2);
	DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_TEXT2, sX, sY, 18);//CityHall Menu Text

	switch (m_stDialogBoxInfo[13].cMode) {
	case 0:
		// citizenship req
		if (m_bCitizen == false)
		{
			if ((msX > sX + 35) && (msX < sX + 220) && (msY > sY + 70) && (msY < sY + 95))
				PutAlignedString(sX, sX + szX, sY + 70, DRAW_DIALOGBOX_CITYHALL_MENU1, 255, 255, 255);	//"
			else PutAlignedString(sX, sX + szX, sY + 70, DRAW_DIALOGBOX_CITYHALL_MENU1, 4, 0, 50);		//"
		}
		else //"
		{	// Centuu: Deny citizenship from CityHall
			//Magn0S:: Change Words.
			if ((msX > sX + 35) && (msX < sX + 220) && (msY > sY + 70) && (msY < sY + 95))
				PutAlignedString(sX, sX + szX, sY + 70, "Change City Request.", 255, 255, 255);
			else PutAlignedString(sX, sX + szX, sY + 70, "Change City Request.", 4, 0, 50);
		}

		if (m_iRewardGold > 0)
		{
			if ((msX > sX + 35) && (msX < sX + 220) && (msY > sY + 95) && (msY < sY + 120))
				PutAlignedString(sX, sX + szX, sY + 95, DRAW_DIALOGBOX_CITYHALL_MENU4, 255, 255, 255);	//"
			else PutAlignedString(sX, sX + szX, sY + 95, DRAW_DIALOGBOX_CITYHALL_MENU4, 4, 0, 50);		//"
		}
		else    PutAlignedString(sX, sX + szX, sY + 95, DRAW_DIALOGBOX_CITYHALL_MENU4, 65, 65, 65);			//"

		// 3.51 Cityhall Menu - Request Hero's Items - Diuuude
		if ((m_iEnemyKillCount >= 0) && (m_iContribution >= 0))
		{
			if ((msX > sX + 35) && (msX < sX + 220) && (msY > sY + 120) && (msY < sY + 145))
				PutAlignedString(sX, sX + szX, sY + 120, DRAW_DIALOGBOX_CITYHALL_MENU8, 255, 255, 255);
			else PutAlignedString(sX, sX + szX, sY + 120, DRAW_DIALOGBOX_CITYHALL_MENU8, 4, 0, 50);
		}
		else    PutAlignedString(sX, sX + szX, sY + 120, DRAW_DIALOGBOX_CITYHALL_MENU8, 65, 65, 65);

		// Cancel quest
		//for (i = 0; i < DEF_MAXQUEST; i++) {
		//	if (m_stQuest[i].sQuestType == 0)
		//	{
				if ((msX > sX + 35) && (msX < sX + 220) && (msY > sY + 145) && (msY < sY + 170))
					PutAlignedString(sX, sX + szX, sY + 145, "Quest List", 255, 255, 255);//"
				else PutAlignedString(sX, sX + szX, sY + 145, "Quest List", 4, 0, 50);//"
		//	}
		//	else    PutAlignedString(sX, sX + szX, sY + 145, "Quest List", 65, 65, 65);//"
		//}

		// change playmode
		if ((m_bIsCrusadeMode == false) && m_bCitizen && (m_iPKCount == 0))
		{
			if (m_bHunter == true)
			{
				if ((msX > sX + 35) && (msX < sX + 220) && (msY > sY + 170) && (msY < sY + 195))
					PutAlignedString(sX, sX + szX, sY + 170, DRAW_DIALOGBOX_CITYHALL_MENU56, 255, 255, 255);
				else
					PutAlignedString(sX, sX + szX, sY + 170, DRAW_DIALOGBOX_CITYHALL_MENU56, 4, 0, 50);
			}
			else if (m_iLevel < 100)
			{
				if ((msX > sX + 35) && (msX < sX + 220) && (msY > sY + 170) && (msY < sY + 195))
					PutAlignedString(sX, sX + szX, sY + 170, DRAW_DIALOGBOX_CITYHALL_MENU56, 255, 255, 255);
				else
					PutAlignedString(sX, sX + szX, sY + 170, DRAW_DIALOGBOX_CITYHALL_MENU56, 4, 0, 50);
			}
			else // Disable...
				PutAlignedString(sX, sX + szX, sY + 170, DRAW_DIALOGBOX_CITYHALL_MENU56, 65, 65, 65);
		}
		else       PutAlignedString(sX, sX + szX, sY + 170, DRAW_DIALOGBOX_CITYHALL_MENU56, 65, 65, 65);

		// Teleport menu
		if ((m_bIsCrusadeMode == false) && m_bCitizen && (m_iPKCount == 0))
		{
			if ((msX > sX + 35) && (msX < sX + 220) && (msY > sY + 195) && (msY < sY + 220))
				PutAlignedString(sX, sX + szX, sY + 195, "Teleport List", 255, 255, 255);//"Teleporting to dungeon level 2."
			else PutAlignedString(sX, sX + szX, sY + 195, "Teleport List", 4, 0, 50);
		}
		else    PutAlignedString(sX, sX + szX, sY + 195, "Teleport List", 65, 65, 65);

		//Change crusade role
		if (m_bIsCrusadeMode && m_bCitizen)
		{
			if ((msX > sX + 35) && (msX < sX + 220) && (msY > sY + 220) && (msY < sY + 220))
				PutAlignedString(sX, sX + szX, sY + 220, DRAW_DIALOGBOX_CITYHALL_MENU14, 255, 255, 255);//"Change the crusade assignment."
			else PutAlignedString(sX, sX + szX, sY + 220, DRAW_DIALOGBOX_CITYHALL_MENU14, 4, 0, 50);//"
		}
		else    PutAlignedString(sX, sX + szX, sY + 220, DRAW_DIALOGBOX_CITYHALL_MENU14, 65, 65, 65);//"

		//MORLA 2.4 - Trade Items
		if ((msX > sX + 35) && (msX < sX + 220) && (msY > sY + 240) && (msY < sY + 257))
			PutAlignedString(sX, sX + szX, sY + 240, "Trade Market", 255, 255, 255);//"Change the crusade assignment."
		else PutAlignedString(sX, sX + szX, sY + 240, "Trade Market", 4, 0, 50);//"

		//MORLA 2.4 - DK Set
		if ((msX > sX + 35) && (msX < sX + 220) && (msY > sY + 260) && (msY < sY + 273))
			PutAlignedString(sX, sX + szX, sY + 260, "Take the Dark Knight's items.", 255, 255, 255);//"Change the crusade assignment."
		else PutAlignedString(sX, sX + szX, sY + 260, "Take the Dark Knight's items.", 4, 0, 50);//"

		//MORLA 2.4 - TP DeathMach Game
		/*if ((msX > sX + 35) && (msX < sX + 220) && (msY > sY + 280) && (msY < sY + 293))
			PutAlignedString(sX, sX + szX, sY + 280, "Go to DeathMatch Game", 255, 255, 255);//"Change the crusade assignment."
		else PutAlignedString(sX, sX + szX, sY + 280, "Go to DeathMatch Game", 4, 0, 50);//"*/

		break;

	case 1: // become citizen warning
		if (m_bCitizen == false)
		{

			PutAlignedString(sX, sX + szX, sY + 80, DRAW_DIALOGBOX_CITYHALL_MENU18, 55, 25, 25);//"
			PutAlignedString(sX, sX + szX, sY + 95, DRAW_DIALOGBOX_CITYHALL_MENU19, 55, 25, 25);//"
			PutAlignedString(sX, sX + szX, sY + 110, DRAW_DIALOGBOX_CITYHALL_MENU20, 55, 25, 25);//"
			PutAlignedString(sX, sX + szX, sY + 125, DRAW_DIALOGBOX_CITYHALL_MENU21, 55, 25, 25);//"
			PutAlignedString(sX, sX + szX, sY + 140, DRAW_DIALOGBOX_CITYHALL_MENU22, 55, 25, 25);//"
			PutAlignedString(sX, sX + szX, sY + 155, DRAW_DIALOGBOX_CITYHALL_MENU23, 55, 25, 25);//"
			PutAlignedString(sX, sX + szX, sY + 170, DRAW_DIALOGBOX_CITYHALL_MENU24, 55, 25, 25);//"
			PutAlignedString(sX, sX + szX, sY + 200, DRAW_DIALOGBOX_CITYHALL_MENU25, 55, 25, 25);//"
			PutAlignedString(sX, sX + szX, sY + 215, DRAW_DIALOGBOX_CITYHALL_MENU26, 55, 25, 25);//"
			PutAlignedString(sX, sX + szX, sY + 230, DRAW_DIALOGBOX_CITYHALL_MENU27, 55, 25, 25);//"
		}
		else	// Centuu: Deny citizenship from CityHall
		{
			PutAlignedString(sX, sX + szX, sY + 80, "If you become a traveller, the level", 55, 25, 25);//"
			PutAlignedString(sX, sX + szX, sY + 95, "restriction as a traveller is enabled.", 55, 25, 25);//"
			PutAlignedString(sX, sX + szX, sY + 110, "You can't buy and sell almost all items", 55, 25, 25);//"
			PutAlignedString(sX, sX + szX, sY + 125, "which are produced in town.", 55, 25, 25);//"
			PutAlignedString(sX, sX + szX, sY + 155, "You can't also join any guilds.", 55, 25, 25);//"
			PutAlignedString(sX, sX + szX, sY + 170, " ", 55, 25, 25);//"
			PutAlignedString(sX, sX + szX, sY + 200, "Do you really wanna leave", 55, 25, 25);//"
			PutAlignedString(sX, sX + szX, sY + 215, "your nation??", 55, 25, 25);//"
		}

		if ((msX >= sX + DEF_LBTNPOSX) && (msX <= sX + DEF_LBTNPOSX + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 19);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 18);

		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 3);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 2);

		break;

	case 2: // Offering Citizenship.
		if (m_bCitizen == false) PutAlignedString(sX, sX + szX, sY + 140, DRAW_DIALOGBOX_CITYHALL_MENU28, 55, 25, 25);//"
		else					 PutAlignedString(sX, sX + szX, sY + 140, "Removing Citizenship...", 55, 25, 25);
		break;

	case 3: //Congratulations!! You acquired Citizenship"
		if (m_bCitizen == true) PutAlignedString(sX, sX + szX, sY + 140, DRAW_DIALOGBOX_CITYHALL_MENU29, 55, 25, 25);//"
		else					PutAlignedString(sX, sX + szX, sY + 140, "We are sorry that you have to leave the city", 55, 25, 25);

		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
		break;

	case 4: //Failed on acquiring citizenship!"
		PutAlignedString(sX, sX + szX, sY + 80, DRAW_DIALOGBOX_CITYHALL_MENU30, 55, 25, 25);//"
		PutAlignedString(sX, sX + szX, sY + 100, DRAW_DIALOGBOX_CITYHALL_MENU31, 55, 25, 25);//"
		PutAlignedString(sX, sX + szX, sY + 115, DRAW_DIALOGBOX_CITYHALL_MENU32, 55, 25, 25);//"

		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
		break;

	case 5: //The prize gold for your"
		PutAlignedString(sX, sX + szX, sY + 125, DRAW_DIALOGBOX_CITYHALL_MENU33, 55, 25, 25);//"
		wsprintf(cTxt, DRAW_DIALOGBOX_CITYHALL_MENU34, m_iRewardGold);//"
		PutAlignedString(sX, sX + szX, sY + 140, cTxt, 55, 25, 25);
		PutAlignedString(sX, sX + szX, sY + 155, DRAW_DIALOGBOX_CITYHALL_MENU35, 55, 25, 25);//"

		if ((msX >= sX + DEF_LBTNPOSX) && (msX <= sX + DEF_LBTNPOSX + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 19);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 18);

		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 3);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 2);
		break;

	case 7:// 3.51 Cityhall Menu - Request Hero's Items - Diuuude
		PutAlignedString(sX, sX + szX, sY + 60, DRAW_DIALOGBOX_CITYHALL_MENU46, 255, 255, 255);// Here are the Hero's Item aivable :
		// Hero's Cape (EK 300)
		if (m_iEnemyKillCount >= 300)
		{
			if ((msX > sX + 35) && (msX < sX + 220) && (msY > sY + 95) && (msY < sY + 110))
				PutAlignedString(sX, sX + szX, sY + 95, DRAW_DIALOGBOX_CITYHALL_MENU47, 255, 255, 255);// On mouse over Mode
			else PutAlignedString(sX, sX + szX, sY + 95, DRAW_DIALOGBOX_CITYHALL_MENU47, 4, 0, 50);// Normal Mode
		}
		else PutAlignedString(sX, sX + szX, sY + 95, DRAW_DIALOGBOX_CITYHALL_MENU47, 65, 65, 65);// Disabled Mode
		// Hero's Helm (EK 150 - Contrib 20)
		if ((m_iEnemyKillCount >= 150) && (m_iContribution >= 20))
		{
			if ((msX > sX + 35) && (msX < sX + 220) && (msY > sY + 110) && (msY < sY + 125))
				PutAlignedString(sX, sX + szX, sY + 110, DRAW_DIALOGBOX_CITYHALL_MENU48, 255, 255, 255);// On mouse over Mode
			else PutAlignedString(sX, sX + szX, sY + 110, DRAW_DIALOGBOX_CITYHALL_MENU48, 4, 0, 50);// Normal Mode
		}
		else PutAlignedString(sX, sX + szX, sY + 110, DRAW_DIALOGBOX_CITYHALL_MENU48, 65, 65, 65);// Disabled Mode
		// Hero's Cap (EK 100 - Contrib 20)
		if ((m_iEnemyKillCount >= 100) && (m_iContribution >= 20))
		{
			if ((msX > sX + 35) && (msX < sX + 220) && (msY > sY + 125) && (msY < sY + 140))
				PutAlignedString(sX, sX + szX, sY + 125, DRAW_DIALOGBOX_CITYHALL_MENU49, 255, 255, 255);// On mouse over Mode
			else PutAlignedString(sX, sX + szX, sY + 125, DRAW_DIALOGBOX_CITYHALL_MENU49, 4, 0, 50);// Normal Mode
		}
		else PutAlignedString(sX, sX + szX, sY + 125, DRAW_DIALOGBOX_CITYHALL_MENU49, 65, 65, 65);// Disabled Mode
		// Hero's Armor (EK 300 - Contrib 30)
		if ((m_iEnemyKillCount >= 300) && (m_iContribution >= 30))
		{
			if ((msX > sX + 35) && (msX < sX + 220) && (msY > sY + 140) && (msY < sY + 155))
				PutAlignedString(sX, sX + szX, sY + 140, DRAW_DIALOGBOX_CITYHALL_MENU50, 255, 255, 255);// On mouse over Mode
			else PutAlignedString(sX, sX + szX, sY + 140, DRAW_DIALOGBOX_CITYHALL_MENU50, 4, 0, 50);// Normal Mode
		}
		else PutAlignedString(sX, sX + szX, sY + 140, DRAW_DIALOGBOX_CITYHALL_MENU50, 65, 65, 65);// Disabled Mode
		// Hero's Robe (EK 200 - Contrib 20)
		if ((m_iEnemyKillCount >= 200) && (m_iContribution >= 20))
		{
			if ((msX > sX + 35) && (msX < sX + 220) && (msY > sY + 155) && (msY < sY + 170))
				PutAlignedString(sX, sX + szX, sY + 155, DRAW_DIALOGBOX_CITYHALL_MENU51, 255, 255, 255);// On mouse over Mode
			else PutAlignedString(sX, sX + szX, sY + 155, DRAW_DIALOGBOX_CITYHALL_MENU51, 4, 0, 50);// Normal Mode
		}
		else PutAlignedString(sX, sX + szX, sY + 155, DRAW_DIALOGBOX_CITYHALL_MENU51, 65, 65, 65);// Disabled Mode
		// Hero's Hauberk (EK 100 - Contrib 10)
		if ((m_iEnemyKillCount >= 100) && (m_iContribution >= 10))
		{
			if ((msX > sX + 35) && (msX < sX + 220) && (msY > sY + 170) && (msY < sY + 185))
				PutAlignedString(sX, sX + szX, sY + 170, DRAW_DIALOGBOX_CITYHALL_MENU52, 255, 255, 255);// On mouse over Mode
			else PutAlignedString(sX, sX + szX, sY + 170, DRAW_DIALOGBOX_CITYHALL_MENU52, 4, 0, 50);// Normal Mode
		}
		else PutAlignedString(sX, sX + szX, sY + 170, DRAW_DIALOGBOX_CITYHALL_MENU52, 65, 65, 65);// Disabled Mode
		// Hero's Leggings (EK 150 - Contrib 15)
		if ((m_iEnemyKillCount >= 150) && (m_iContribution >= 15))
		{
			if ((msX > sX + 35) && (msX < sX + 220) && (msY > sY + 185) && (msY < sY + 200))
				PutAlignedString(sX, sX + szX, sY + 185, DRAW_DIALOGBOX_CITYHALL_MENU53, 255, 255, 255);// On mouse over Mode
			else PutAlignedString(sX, sX + szX, sY + 185, DRAW_DIALOGBOX_CITYHALL_MENU53, 4, 0, 50);// Normal Mode
		}
		else PutAlignedString(sX, sX + szX, sY + 185, DRAW_DIALOGBOX_CITYHALL_MENU53, 65, 65, 65);// Disabled Mode
		// Hero's Hood (EK 150 - Contrib 20)
		if ((m_iEnemyKillCount >= 150) && (m_iContribution >= 20))
		{
			if ((msX > sX + 35) && (msX < sX + 220) && (msY > sY + 200) && (msY < sY + 215))
				PutAlignedString(sX, sX + szX, sY + 200, "Hero's Hood (EK 150 - Contrib 20)", 255, 255, 255);// On mouse over Mode
			else PutAlignedString(sX, sX + szX, sY + 200, "Hero's Hood (EK 150 - Contrib 20)", 4, 0, 50);// Normal Mode
		}
		else PutAlignedString(sX, sX + szX, sY + 200, "Hero's Hood (EK 150 - Contrib 20)", 65, 65, 65);// Disabled Mode
		break;

	case 8: // cancel current quest?
		PutAlignedString(sX, sX + szX, sY + 125, DRAW_DIALOGBOX_CITYHALL_MENU54, 55, 25, 25);//"
		PutAlignedString(sX, sX + szX, sY + 140, DRAW_DIALOGBOX_CITYHALL_MENU55, 55, 25, 25);//"

		if ((msX >= sX + DEF_LBTNPOSX) && (msX <= sX + DEF_LBTNPOSX + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 19);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 18);

		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 3);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 2);
		break;

	case 9: // You are civilian/ combatant now
		if (m_bHunter)
		{
			PutAlignedString(sX, sX + szX, sY + 53, DRAW_DIALOGBOX_CITYHALL_MENU57, 200, 200, 25);//"
		}
		else
		{
			PutAlignedString(sX, sX + szX, sY + 53, DRAW_DIALOGBOX_CITYHALL_MENU58, 200, 200, 25);//"
		}
		PutAlignedString(sX, sX + szX, sY + 78, DRAW_DIALOGBOX_CITYHALL_MENU59, 55, 25, 25);//"

		PutString(sX + 35, sY + 108, DRAW_DIALOGBOX_CITYHALL_MENU60, RGB(220, 130, 45));//"
		PutAlignedString(sX, sX + szX, sY + 125, DRAW_DIALOGBOX_CITYHALL_MENU61, 55, 25, 25);//"
		PutAlignedString(sX, sX + szX, sY + 140, DRAW_DIALOGBOX_CITYHALL_MENU62, 55, 25, 25);//"
		PutAlignedString(sX, sX + szX, sY + 155, DRAW_DIALOGBOX_CITYHALL_MENU63, 55, 25, 25);//"
		PutString(sX + 35, sY + 177, DRAW_DIALOGBOX_CITYHALL_MENU64, RGB(220, 130, 45));//"
		PutAlignedString(sX, sX + szX, sY + 194, DRAW_DIALOGBOX_CITYHALL_MENU65, 55, 25, 25);//"
		PutAlignedString(sX, sX + szX, sY + 209, DRAW_DIALOGBOX_CITYHALL_MENU66, 55, 25, 25);//"
		PutAlignedString(sX, sX + szX, sY + 224, DRAW_DIALOGBOX_CITYHALL_MENU67, 55, 25, 25);//"

		PutAlignedString(sX, sX + szX, sY + 252, DRAW_DIALOGBOX_CITYHALL_MENU68, 55, 25, 25);//"
		if ((msX >= sX + DEF_LBTNPOSX) && (msX <= sX + DEF_LBTNPOSX + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 19);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 18);

		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 3);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 2);
		break;

	case 10: // TP 2nd screen
		if (m_iTeleportMapCount > 0)
		{
			PutString_SprFont(sX + 80, sY + 50, "Teleport List", 1, 1, 8);//"Teleport System with Balls"
			PutAlignedString(sX, sX + szX, sY + 80, "Teleports requires an ammount of Gold.", 55, 25, 25);//"Teleporting to dungeon level 2."
			PutAlignedString(sX, sX + szX, sY + 95, "Event Maps will be possible to teleport", 55, 25, 25);//"5000Gold is required"
			PutAlignedString(sX, sX + szX, sY + 110, "only if the Event is Open.", 55, 25, 25);//"to teleport to dungeon level 2."
			//PutAlignedString(sX, sX + szX, sY + 125, DRAW_DIALOGBOX_CITYHALL_MENU72, 55, 25, 25);//"Would you like to teleport?"
			//PutString2(sX + 35, sY + 250, DRAW_DIALOGBOX_CITYHALL_MENU72_1, 55, 25, 25);//"Civilians cannot go some area."
			for (int i = 0; i<m_iTeleportMapCount; i++)
			{
				ZeroMemory(cTxt, sizeof(cTxt));
				GetOfficialMapName(m_stTeleportList[i].mapname, cTxt);
				wsprintf(G_cTxt, DRAW_DIALOGBOX_CITYHALL_MENU77, cTxt, m_stTeleportList[i].iCost);
				
				if (string(m_stTeleportList[i].mapname) == "fightzone1")
				{
					if (!bDeathmatch)
					{
						PutAlignedString(sX, sX + szX, sY + 130 + i * 15, G_cTxt, 255, 0, 0);
					}
					else
					{
						if ((msX >= sX + DEF_LBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY >= sY + 130 + i * 15) && (msY <= sY + 144 + i * 15))
							PutAlignedString(sX, sX + szX, sY + 130 + i * 15, G_cTxt, 255, 255, 255);
						else PutAlignedString(sX, sX + szX, sY + 130 + i * 15, G_cTxt, 250, 250, 0);
					}
				}
				else if (string(m_stTeleportList[i].mapname) == "team")
				{
					if (!_team_arena)
					{
						PutAlignedString(sX, sX + szX, sY + 130 + i * 15, G_cTxt, 255, 0, 0);
					}
					else
					{
						if ((msX >= sX + DEF_LBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY >= sY + 130 + i * 15) && (msY <= sY + 144 + i * 15))
							PutAlignedString(sX, sX + szX, sY + 130 + i * 15, G_cTxt, 255, 255, 255);
						else PutAlignedString(sX, sX + szX, sY + 130 + i * 15, G_cTxt, 250, 250, 0);
					}
				}
				else {
					if ((msX >= sX + DEF_LBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY >= sY + 130 + i * 15) && (msY <= sY + 144 + i * 15))
						PutAlignedString(sX, sX + szX, sY + 130 + i * 15, G_cTxt, 255, 255, 255);
					else PutAlignedString(sX, sX + szX, sY + 130 + i * 15, G_cTxt, 250, 250, 0);
				}
			}
		}
		else if (m_iTeleportMapCount == -1)
		{
			PutAlignedString(sX, sX + szX, sY + 125, DRAW_DIALOGBOX_CITYHALL_MENU73, 55, 25, 25);//"Now it's searching for possible area"
			PutAlignedString(sX, sX + szX, sY + 150, DRAW_DIALOGBOX_CITYHALL_MENU74, 55, 25, 25);//"to teleport."
			PutAlignedString(sX, sX + szX, sY + 175, DRAW_DIALOGBOX_CITYHALL_MENU75, 55, 25, 25);//"Please wait for a moment."
		}
		else
		{
			PutAlignedString(sX, sX + szX, sY + 175, DRAW_DIALOGBOX_CITYHALL_MENU76, 55, 25, 25);//"There is no area that you can teleport."
		}
		break;

	case 11: //drajwer - ask for tooking hero mantle
		//PutAlignedString(sX, sX + szX - 1, sY + 125, m_cTakeHeroItemName, 55, 25, 25);
		//PutAlignedString(sX + 1, sX + szX, sY + 125, m_cTakeHeroItemName, 55, 25, 25);
		PutAlignedString(sX, sX + szX, sY + 260, DRAW_DIALOGBOX_CITYHALL_MENU46A, 55, 25, 25); // would you like..
		if ((msX >= sX + DEF_LBTNPOSX) && (msX <= sX + DEF_LBTNPOSX + DEF_BTNSZX)
			&& (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 19);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 18);
		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX)
			&& (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 3);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 2);
		break;

	//Magn0S:: Split Trades
	case 12:
		PutString_SprFont(sX + 40, sY + 50, "Trade Market Selection", 1, 1, 8);//"Teleport System with Balls"

		if ((msX > sX + 35) && (msX < sX + 220) && (msY > sY + 80) && (msY < sY + 100))
			PutAlignedString(sX, sX + szX, sY + 80, "Trade Ek", 255, 255, 255);//"Change the crusade assignment."
		else PutAlignedString(sX, sX + szX, sY + 80, "Trade Ek", 4, 0, 50);//"

		if ((msX > sX + 35) && (msX < sX + 220) && (msY > sY + 120) && (msY < sY + 140))
			PutAlignedString(sX, sX + szX, sY + 120, "Trade Contribution", 255, 255, 255);//"Change the crusade assignment."
		else PutAlignedString(sX, sX + szX, sY + 120, "Trade Contribution", 4, 0, 50);//"

		if ((msX > sX + 35) && (msX < sX + 220) && (msY > sY + 160) && (msY < sY + 180))
			PutAlignedString(sX, sX + szX, sY + 160, "Trade Coins", 255, 255, 255);//"Change the crusade assignment."
		else PutAlignedString(sX, sX + szX, sY + 160, "Trade Coins", 4, 0, 50);//"
		break;

	case 13: //Magn0S:: Inform and Confirm to get dk set
		if (m_iLevel < DEF_LEVEL_LIMIT)
		{
			PutAlignedString(sX, sX + szX, sY + 80, "Your level is too low to request DK Set.", 55, 25, 25);//"
			//PutAlignedString(sX, sX + szX, sY + 95, DRAW_DIALOGBOX_CITYHALL_MENU19, 55, 25, 25);//"
			PutAlignedString(sX, sX + szX, sY + 110, "To request DK Set, you must be", 55, 25, 25);//"
			PutAlignedString(sX, sX + szX, sY + 125, "Level Max, and have 50.000 Gold", 55, 25, 25);//"
			PutAlignedString(sX, sX + szX, sY + 140, "After it, you can come back here.", 55, 25, 25);//"
			//PutAlignedString(sX, sX + szX, sY + 155, DRAW_DIALOGBOX_CITYHALL_MENU23, 55, 25, 25);//"
			//PutAlignedString(sX, sX + szX, sY + 170, DRAW_DIALOGBOX_CITYHALL_MENU24, 55, 25, 25);//"
			//PutAlignedString(sX, sX + szX, sY + 200, DRAW_DIALOGBOX_CITYHALL_MENU25, 55, 25, 25);//"
			//PutAlignedString(sX, sX + szX, sY + 215, DRAW_DIALOGBOX_CITYHALL_MENU26, 55, 25, 25);//"
			//PutAlignedString(sX, sX + szX, sY + 230, DRAW_DIALOGBOX_CITYHALL_MENU27, 55, 25, 25);//"
		}
		else	// Max Lvl
		{
			PutAlignedString(sX, sX + szX, sY + 80, "Congratulations, you become Level Max!", 55, 25, 25);//"

			PutAlignedString(sX, sX + szX, sY + 110, "To request DK Set, you must have", 55, 25, 25);//"
			PutAlignedString(sX, sX + szX, sY + 125, "50.000 Gold.", 55, 25, 25);//"
			
			PutAlignedString(sX, sX + szX, sY + 170, "By Clicking YES, you will receive", 55, 25, 25);//"
			PutAlignedString(sX, sX + szX, sY + 200, "the default Armors of your class", 55, 25, 25);//"
			PutAlignedString(sX, sX + szX, sY + 215, "(War / Mage / Archer)", 55, 25, 25);//"

			if ((msX >= sX + DEF_LBTNPOSX) && (msX <= sX + DEF_LBTNPOSX + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 19);
			else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 18);

			if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 3);
			else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 2);
		}
		break;
	}
}
/*********************************************************************************************************************
**  void CGame::DrawDialogBox_ConfirmExchange(short msX, short msY)	(snoopy)										**
**  description			:: paints the trade windows	Added confirmation window in v3.51								**
**********************************************************************************************************************/
void CGame::DrawDialogBox_ConfirmExchange(short msX, short msY)
{
	short sX, sY;
	sX = m_stDialogBoxInfo[41].sX;
	sY = m_stDialogBoxInfo[41].sY;

	DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME5, sX, sY, 2);
	switch (m_stDialogBoxInfo[41].cMode) {
	case 1: // Question
		PutString(sX + 35, sY + 30, "Do you really want to exchange?", RGB(4, 0, 50));
		PutString(sX + 36, sY + 30, "Do you really want to exchange?", RGB(4, 0, 50));

		if ((msX >= sX + 30) && (msX <= sX + 30 + DEF_BTNSZX) && (msY >= sY + 55) && (msY <= sY + 55 + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + 30, sY + 55, 19);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + 30, sY + 55, 18);

		if ((msX >= sX + 170) && (msX <= sX + 170 + DEF_BTNSZX) && (msY >= sY + 55) && (msY <= sY + 55 + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + 170, sY + 55, 3);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + 170, sY + 55, 2);
		break;

	case 2: // Waiting for response
		PutString(sX + 45, sY + 36, "Waiting for response...", RGB(4, 0, 50));
		PutString(sX + 46, sY + 36, "Waiting for response...", RGB(4, 0, 50));
		break;
	}
}

/*********************************************************************************************************************
**  void CGame::DrawDialogBox_Exchange(short msX, short msY)	(snoopy)											**
**  description			:: paints the trade windows																	**
**********************************************************************************************************************/
void CGame::DrawDialogBox_Exchange(short msX, short msY)
{
#ifdef MULTI_TRADE
	short sX, sY, szX, sXadd;
	//50Cent - MultiTrade
	short sYadd, sAux = 0;
	UINT32 dwTime = m_dwCurTime;
	char cItemColor, cTxt[120], cTxt2[128];
	char cNameStr[120], cSubStr1[120], cSubStr2[120];
	int iLoc, i;

	sX = m_stDialogBoxInfo[27].sX;
	sY = m_stDialogBoxInfo[27].sY;
	szX = m_stDialogBoxInfo[27].sSizeX;

	DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_NEWEXCHANGE2, sX, sY, 0);
	switch (m_stDialogBoxInfo[27].cMode) {
	case 1: // Not yet confirmed exchange
		PutAlignedString(sX + 80, sX + 180, sY + 38, m_cPlayerName, 35, 55, 35); // my name
		if (m_stDialogBoxExchangeInfo[8].sV1 != -1) // Other name if applicable//50Cent - MultiTrade
			PutAlignedString(sX + 250, sX + 540, sY + 38, m_stDialogBoxExchangeInfo[8].cStr2, 35, 55, 35);//50Cent - MultiTrade
		for (i = 0; i < 16; i++) // draw items//50Cent - MultiTrade
		{

			//50Cent - MultiTrade
			switch (i)
			{
			case 0: sXadd = 48;  sYadd = 95; break;
			case 1: sXadd = 106; sYadd = 95; break;
			case 2: sXadd = 164; sYadd = 95; break;
			case 3: sXadd = 222; sYadd = 95; break;
			case 4: sXadd = 48;  sYadd = 160; break;
			case 5: sXadd = 106; sYadd = 160; break;
			case 6: sXadd = 164; sYadd = 160; break;
			case 7: sXadd = 222; sYadd = 160; break;

			case 8: sXadd = 300; sYadd = 95; break;
			case 9: sXadd = 358; sYadd = 95; break;
			case 10: sXadd = 416; sYadd = 95; break;
			case 11: sXadd = 474; sYadd = 95; break;
			case 12: sXadd = 300; sYadd = 175; break;
			case 13: sXadd = 358; sYadd = 175; break;
			case 14: sXadd = 416; sYadd = 175; break;
			case 15: sXadd = 474; sYadd = 175; break;
			}
			if (m_stDialogBoxExchangeInfo[i].sV1 != -1)
			{
				cItemColor = m_stDialogBoxExchangeInfo[i].sV4;
				if (cItemColor == 0)
				{
					//50Cent - MultiTrade
					m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_stDialogBoxExchangeInfo[i].sV1]->PutSpriteFast(sX + sXadd, sY + sYadd, m_stDialogBoxExchangeInfo[i].sV2, dwTime);
				}
				else
				{
					switch (m_stDialogBoxExchangeInfo[i].sV1) {//sV1 : Sprite
					case 1: //  Swds
					case 2: //  Bows
					case 3: //  Shields
					case 15: // Axes hammers
					case 17: // Wands

						//50Cent - MultiTrade
						m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_stDialogBoxExchangeInfo[i].sV1]->PutSpriteRGB(sX + sXadd, sY + sYadd
							, m_stDialogBoxExchangeInfo[i].sV2, m_wWR[cItemColor] - m_wR[0], m_wWG[cItemColor] - m_wG[0], m_wWB[cItemColor] - m_wB[0], dwTime);
						break;
					default:
						//50Cent - MultiTrade
						m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_stDialogBoxExchangeInfo[i].sV1]->PutSpriteRGB(sX + sXadd, sY + sYadd
							, m_stDialogBoxExchangeInfo[i].sV2, m_wR[cItemColor] - m_wR[0], m_wG[cItemColor] - m_wG[0], m_wB[cItemColor] - m_wB[0], dwTime);
						break;
					}
				}
				GetItemName(m_stDialogBoxExchangeInfo[i].cStr1, m_stDialogBoxExchangeInfo[i].dwV1, cNameStr, cSubStr1, cSubStr2);
				// If pointer over item then show this item data

				//50Cent - MultiTrade
				if ((msX >= sX + sXadd - 6) && (msX <= sX + sXadd + 42)
					&& (msY >= sY + (sYadd - 34)) && (msY <= sY + (sYadd + 36)))
				{
					wsprintf(cTxt, "%s", cNameStr);
					if (m_bIsSpecial)
					{
						// VAMP - gold items
						if (m_bIsRare)
						{
							PutAlignedString(sX + 15, sX + 255, sY + 215, cTxt, 255, 208, 60);
							PutAlignedString(sX + 16, sX + 256, sY + 215, cTxt, 255, 208, 60);
						}
						else if (m_bIsFragile)
						{
							PutAlignedString(sX + 15, sX + 255, sY + 215, cTxt, 50, 255, 255);
							PutAlignedString(sX + 16, sX + 256, sY + 215, cTxt, 50, 255, 255);
						}
						else
						{
							PutAlignedString(sX + 15, sX + 255, sY + 215, cTxt, 0, 255, 50);
							PutAlignedString(sX + 16, sX + 256, sY + 215, cTxt, 0, 255, 50);

						}
					}
					else
					{
						PutAlignedString(sX + 15, sX + 255, sY + 215, cTxt, 35, 35, 35);
						PutAlignedString(sX + 16, sX + 256, sY + 215, cTxt, 35, 35, 35);
					}
					iLoc = 0;
					if (strlen(cSubStr1) != 0)
					{
						PutAlignedString(sX + 16, sX + 155, sY + 245 + iLoc, cSubStr1, 0, 0, 0);
						iLoc += 15;
					}
					if (strlen(cSubStr2) != 0)
					{
						PutAlignedString(sX + 16, sX + 155, sY + 245 + iLoc, cSubStr2, 0, 0, 0);
						iLoc += 15;
					}
					if (m_stDialogBoxExchangeInfo[i].sV3 != 1) // Item avec Quantity
					{
						if (m_stDialogBoxExchangeInfo[i].sV3 > 1)
						{
							DisplayCommaNumber_G_cTxt(m_stDialogBoxExchangeInfo[i].sV3);
							strcpy(cTxt2, G_cTxt);
						}
						else wsprintf(cTxt2, DRAW_DIALOGBOX_EXCHANGE2, m_stDialogBoxExchangeInfo[i].sV3);
						PutAlignedString(sX + 16, sX + 155, sY + 245 + iLoc, cTxt2, 35, 35, 35);
						iLoc += 15;
					}
					if (m_stDialogBoxExchangeInfo[i].sV5 != -1) // completion
					{    // Crafting Magins completion fix
						if (m_stDialogBoxExchangeInfo[i].sV1 == 22)
						{
							if ((m_stDialogBoxExchangeInfo[i].sV2 > 5)
								&& (m_stDialogBoxExchangeInfo[i].sV2 < 10))
							{
								wsprintf(cTxt, GET_ITEM_NAME2, (m_stDialogBoxExchangeInfo[i].sV7 - 100)); //Completion - 100
							}
						}
						else if (m_stDialogBoxExchangeInfo[i].sV1 == 6)
						{
							wsprintf(cTxt, GET_ITEM_NAME1, (m_stDialogBoxExchangeInfo[i].sV7 - 100)); //Purity
						}
						else
						{
							wsprintf(cTxt, GET_ITEM_NAME2, m_stDialogBoxExchangeInfo[i].sV7); //Completion
						}
						PutAlignedString(sX + 16, sX + 155, sY + 245 + iLoc, cTxt, 35, 35, 35);
						iLoc += 15;
					}
					if (iLoc < 45) // Endurance
					{
						wsprintf(cTxt, DRAW_DIALOGBOX_EXCHANGE3, m_stDialogBoxExchangeInfo[i].sV5, m_stDialogBoxExchangeInfo[i].sV6);
						PutAlignedString(sX + 16, sX + 155, sY + 245 + iLoc, cTxt, 35, 35, 35);
						iLoc += 15;
					}
				}
			}
		}
		if ((m_stDialogBoxExchangeInfo[0].sV1 != -1) && (m_stDialogBoxExchangeInfo[8].sV1 == -1))//50Cent - MultiTrade
		{
			PutAlignedString(sX, sX + szX, sY + 235 + 10, DRAW_DIALOGBOX_EXCHANGE9, 55, 25, 25); // Please wait until other player  decides
			PutAlignedString(sX, sX + szX, sY + 250 + 10, DRAW_DIALOGBOX_EXCHANGE10, 55, 25, 25);// to exchange. If you want to  cancel the
			PutAlignedString(sX, sX + szX, sY + 265 + 10, DRAW_DIALOGBOX_EXCHANGE11, 55, 25, 25);// exchange press the CANCEL button  now.
			PutString_SprFont(sX + 220, sY + 310, "Exchange", 15, 15, 15);
		}
		else if ((m_stDialogBoxExchangeInfo[0].sV1 == -1) && (m_stDialogBoxExchangeInfo[8].sV1 != -1))//50Cent - MultiTrade
		{
			PutAlignedString(sX, sX + szX, sY + 205 + 10, DRAW_DIALOGBOX_EXCHANGE12, 55, 25, 25);// Other player offered an item  exchange
			PutAlignedString(sX, sX + szX, sY + 220 + 10, DRAW_DIALOGBOX_EXCHANGE13, 55, 25, 25);// Select an item which you want to  exc-
			PutAlignedString(sX, sX + szX, sY + 235 + 10, DRAW_DIALOGBOX_EXCHANGE14, 55, 25, 25);// hange with above item, drag it  to the
			PutAlignedString(sX, sX + szX, sY + 250 + 10, DRAW_DIALOGBOX_EXCHANGE15, 55, 25, 25);// blank and press the EXCHANGE  button.
			PutAlignedString(sX, sX + szX, sY + 265 + 10, DRAW_DIALOGBOX_EXCHANGE16, 55, 25, 25);// And you can also reject an offer  by
			PutAlignedString(sX, sX + szX, sY + 280 + 10, DRAW_DIALOGBOX_EXCHANGE17, 55, 25, 25);// pressing the CANCEL button.
			PutString_SprFont(sX + 220, sY + 310, "Exchange", 15, 15, 15);
		}
		else if ((m_stDialogBoxExchangeInfo[0].sV1 != -1) && (m_stDialogBoxExchangeInfo[8].sV1 != -1))//50Cent - MultiTrade
		{
			PutAlignedString(sX, sX + szX, sY + 205 + 10, DRAW_DIALOGBOX_EXCHANGE18, 55, 25, 25);// The preparation for item  exchange
			PutAlignedString(sX, sX + szX, sY + 220 + 10, DRAW_DIALOGBOX_EXCHANGE19, 55, 25, 25);// has been finished. Press the  EXCHANGE
			PutAlignedString(sX, sX + szX, sY + 235 + 10, DRAW_DIALOGBOX_EXCHANGE20, 55, 25, 25);// button to exchange as above.  Press the
			PutAlignedString(sX, sX + szX, sY + 250 + 10, DRAW_DIALOGBOX_EXCHANGE21, 55, 25, 25);// CANCEL button to cancel.  Occasionally
			PutAlignedString(sX, sX + szX, sY + 265 + 10, DRAW_DIALOGBOX_EXCHANGE22, 55, 25, 25);// when you press the EXCHANGE  button, you
			PutAlignedString(sX, sX + szX, sY + 280 + 10, DRAW_DIALOGBOX_EXCHANGE23, 55, 25, 25);// will not be able to cancel the  exchange.
			if ((msX >= sX + 200) && (msX <= sX + 200 + DEF_BTNSZX) && (msY >= sY + 310) && (msY <= sY + 310 + DEF_BTNSZY))
				PutString_SprFont(sX + 220, sY + 310, "Exchange", 6, 6, 20);
			else PutString_SprFont(sX + 220, sY + 310, "Exchange", 0, 0, 7);
		}
		if ((msX >= sX + 450) && (msX <= sX + 450 + DEF_BTNSZX) && (msY >= sY + 310) && (msY <= sY + 310 + DEF_BTNSZY)
			&& (m_bIsDialogEnabled[41] == false))
			PutString_SprFont(sX + 450, sY + 310, "Cancel", 6, 6, 20);
		else PutString_SprFont(sX + 450, sY + 310, "Cancel", 0, 0, 7);
		break;

	case 2: // You have confirmed the exchange
		PutAlignedString(sX + 80, sX + 180, sY + 38, m_cPlayerName, 35, 55, 35); // my name
		if (m_stDialogBoxExchangeInfo[8].sV1 != -1) // Other name if applicable//50Cent - MultiTrade
			PutAlignedString(sX + 250, sX + 540, sY + 38, m_stDialogBoxExchangeInfo[8].cStr2, 35, 55, 35);//50Cent - MultiTrade
		for (i = 0; i < 16; i++) // draw items//50Cent - MultiTrade
		{
			//50Cent - MultiTrade
			switch (i)
			{
			case 0: sXadd = 48;  sYadd = 95; break;
			case 1: sXadd = 106; sYadd = 95; break;
			case 2: sXadd = 164; sYadd = 95; break;
			case 3: sXadd = 222; sYadd = 95; break;
			case 4: sXadd = 48;  sYadd = 160; break;
			case 5: sXadd = 106; sYadd = 160; break;
			case 6: sXadd = 164; sYadd = 160; break;
			case 7: sXadd = 222; sYadd = 160; break;

			case 8: sXadd = 300; sYadd = 95; break;
			case 9: sXadd = 358; sYadd = 95; break;
			case 10: sXadd = 416; sYadd = 95; break;
			case 11: sXadd = 474; sYadd = 95; break;
			case 12: sXadd = 300; sYadd = 175; break;
			case 13: sXadd = 358; sYadd = 175; break;
			case 14: sXadd = 416; sYadd = 175; break;
			case 15: sXadd = 474; sYadd = 175; break;
			}
			if (m_stDialogBoxExchangeInfo[i].sV1 != -1)
			{
				cItemColor = m_stDialogBoxExchangeInfo[i].sV4;
				if (cItemColor == 0)
				{
					//50Cent - MultiTrade
					m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_stDialogBoxExchangeInfo[i].sV1]->PutSpriteFast(sX + sXadd, sY + sYadd, m_stDialogBoxExchangeInfo[i].sV2, dwTime);
				}
				else
				{
					switch (m_stDialogBoxExchangeInfo[i].sV1) {
					case 1: // Swds
					case 2: // Bows
					case 3: // Shields
					case 15: // Axes hammers
					case 17: // Wands

						//50Cent - MultiTrade
						m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_stDialogBoxExchangeInfo[i].sV1]->PutSpriteRGB(sX + sXadd, sY + sYadd
							, m_stDialogBoxExchangeInfo[i].sV2, m_wWR[cItemColor] - m_wR[0], m_wWG[cItemColor] - m_wG[0], m_wWB[cItemColor] - m_wB[0], dwTime);
						break;
					default:
						//50Cent - MultiTrade
						m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_stDialogBoxExchangeInfo[i].sV1]->PutSpriteRGB(sX + sXadd, sY + sYadd
							, m_stDialogBoxExchangeInfo[i].sV2, m_wR[cItemColor] - m_wR[0], m_wG[cItemColor] - m_wG[0], m_wB[cItemColor] - m_wB[0], dwTime);
						break;
					}
				}
				GetItemName(m_stDialogBoxExchangeInfo[i].cStr1, m_stDialogBoxExchangeInfo[i].dwV1, cNameStr, cSubStr1, cSubStr2);
				// If pointer over item then show this item data

				//50Cent - MultiTrade
				if ((msX >= sX + sXadd - 6) && (msX <= sX + sXadd + 42)
					&& (msY >= sY + (sYadd - 34)) && (msY <= sY + (sYadd + 36)))
				{
					wsprintf(cTxt, "%s", cNameStr);
					if (m_bIsSpecial)
					{
						// VAMP - gold items
						if (m_bIsRare)
						{
							PutAlignedString(sX + 15, sX + 155, sY + 215, cTxt, 255, 208, 60);
							PutAlignedString(sX + 16, sX + 156, sY + 215, cTxt, 255, 208, 60);
						}
						else if (m_bIsFragile)
						{
							PutAlignedString(sX + 15, sX + 155, sY + 215, cTxt, 50, 255, 255);
							PutAlignedString(sX + 16, sX + 156, sY + 215, cTxt, 50, 255, 255);
						}
						else
						{
							PutAlignedString(sX + 15, sX + 155, sY + 215, cTxt, 0, 255, 50);
							PutAlignedString(sX + 16, sX + 156, sY + 215, cTxt, 0, 255, 50);

						}
					}
					else
					{
						PutAlignedString(sX + 15, sX + 155, sY + 215, cTxt, 35, 35, 35);
						PutAlignedString(sX + 16, sX + 156, sY + 215, cTxt, 35, 35, 35);
					}
					iLoc = 0;
					if (strlen(cSubStr1) != 0)
					{
						PutAlignedString(sX + 16, sX + 155, sY + 245 + iLoc, cSubStr1, 0, 0, 0);
						iLoc += 15;
					}
					if (strlen(cSubStr2) != 0)
					{
						PutAlignedString(sX + 16, sX + 155, sY + 245 + iLoc, cSubStr2, 0, 0, 0);
						iLoc += 15;
					}
					if (m_stDialogBoxExchangeInfo[i].sV3 != 1) // Item avec Quantity
					{
						if (m_stDialogBoxExchangeInfo[i].sV3 > 1)
						{
							DisplayCommaNumber_G_cTxt(m_stDialogBoxExchangeInfo[i].sV3);
							strcpy(cTxt2, G_cTxt);
						}
						else wsprintf(cTxt2, DRAW_DIALOGBOX_EXCHANGE2, m_stDialogBoxExchangeInfo[i].sV3);
						PutAlignedString(sX + 16, sX + 155, sY + 245 + iLoc, cTxt2, 35, 35, 35);
						iLoc += 15;
					}
					if (m_stDialogBoxExchangeInfo[i].sV5 != -1) // completion
					{    // Crafting Magins completion fix
						if (m_stDialogBoxExchangeInfo[i].sV1 == 22)
						{
							if ((m_stDialogBoxExchangeInfo[i].sV2 > 5)
								&& (m_stDialogBoxExchangeInfo[i].sV2 < 10))
							{
								wsprintf(cTxt, GET_ITEM_NAME2, (m_stDialogBoxExchangeInfo[i].sV7 - 100)); //Completion - 100
							}
						}
						else if (m_stDialogBoxExchangeInfo[i].sV1 == 6)
						{
							wsprintf(cTxt, GET_ITEM_NAME1, (m_stDialogBoxExchangeInfo[i].sV7 - 100)); //Purity
						}
						else
						{
							wsprintf(cTxt, GET_ITEM_NAME2, m_stDialogBoxExchangeInfo[i].sV7); //Completion
						}
						PutAlignedString(sX + 16, sX + 155, sY + 245 + iLoc, cTxt, 35, 35, 35);
						iLoc += 15;
					}
					if (iLoc < 45) // Endurance
					{
						wsprintf(cTxt, DRAW_DIALOGBOX_EXCHANGE3, m_stDialogBoxExchangeInfo[i].sV5, m_stDialogBoxExchangeInfo[i].sV6);
						PutAlignedString(sX + 16, sX + 155, sY + 245 + iLoc, cTxt, 35, 35, 35);
						iLoc += 15;
					}
				}
			}
		}
		wsprintf(cTxt, DRAW_DIALOGBOX_EXCHANGE33, m_stDialogBoxExchangeInfo[8].cStr2);//50Cent - MultiTrade
		PutAlignedString(sX, sX + szX, sY + 205 + 10, cTxt, 55, 25, 25);                     // Please wait until %s agrees to
		PutAlignedString(sX, sX + szX, sY + 220 + 10, DRAW_DIALOGBOX_EXCHANGE34, 55, 25, 25);// exchange. The exchange can't be  achieved
		PutAlignedString(sX, sX + szX, sY + 235 + 10, DRAW_DIALOGBOX_EXCHANGE35, 55, 25, 25);// unless both people agree.
		PutAlignedString(sX, sX + szX, sY + 250 + 10, DRAW_DIALOGBOX_EXCHANGE36, 55, 25, 25);//  If other player does not decide  to exchange
		PutAlignedString(sX, sX + szX, sY + 265 + 10, DRAW_DIALOGBOX_EXCHANGE37, 55, 25, 25);// you can cancel the exchange by  pressing the
		PutAlignedString(sX, sX + szX, sY + 280 + 10, DRAW_DIALOGBOX_EXCHANGE38, 55, 25, 25);// CANCEL button. But if other  player already
		PutAlignedString(sX, sX + szX, sY + 295 + 10, DRAW_DIALOGBOX_EXCHANGE39, 55, 25, 25);// decided to exchange, you can't  cancel anymore
		break;
	}
#else
	short sX, sY, szX, sXadd;
	UINT32 dwTime = m_dwCurTime;
	char cItemColor, cTxt[120], cTxt2[128];
	char cNameStr[120], cSubStr1[120], cSubStr2[120];
	int iLoc, i;

	sX = m_stDialogBoxInfo[27].sX;
	sY = m_stDialogBoxInfo[27].sY;
	szX = m_stDialogBoxInfo[27].sSizeX;

	DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_NEWEXCHANGE, sX, sY, 0);
	switch (m_stDialogBoxInfo[27].cMode) {
	case 1: // Not yet confirmed exchange
		PutAlignedString(sX + 80, sX + 180, sY + 38, m_cPlayerName, 35, 55, 35); // my name
		if (m_stDialogBoxExchangeInfo[4].sV1 != -1) // Other name if applicable
			PutAlignedString(sX + 250, sX + 540, sY + 38, m_stDialogBoxExchangeInfo[4].cStr2, 35, 55, 35);
		for (i = 0; i<8; i++) // draw items
		{
			sXadd = (58 * i) + 48; if (i>3) sXadd += 20;
			if (m_stDialogBoxExchangeInfo[i].sV1 != -1)
			{
				cItemColor = m_stDialogBoxExchangeInfo[i].sV4;
				if (cItemColor == 0)
				{
					m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_stDialogBoxExchangeInfo[i].sV1]->PutSpriteFast(sX + sXadd, sY + 130, m_stDialogBoxExchangeInfo[i].sV2, dwTime);
				}
				else
				{
					switch (m_stDialogBoxExchangeInfo[i].sV1) {//sV1 : Sprite
					case 1: //  Swds
					case 2: //  Bows
					case 3: //  Shields
					case 15: // Axes hammers
					case 17: // Wands
						m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_stDialogBoxExchangeInfo[i].sV1]->PutSpriteRGB(sX + sXadd, sY + 130
							, m_stDialogBoxExchangeInfo[i].sV2, m_wWR[cItemColor] - m_wR[0], m_wWG[cItemColor] - m_wG[0], m_wWB[cItemColor] - m_wB[0], dwTime);
						break;
					default: m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_stDialogBoxExchangeInfo[i].sV1]->PutSpriteRGB(sX + sXadd, sY + 130
						, m_stDialogBoxExchangeInfo[i].sV2, m_wR[cItemColor] - m_wR[0], m_wG[cItemColor] - m_wG[0], m_wB[cItemColor] - m_wB[0], dwTime);
						break;
					}
				}
				GetItemName(m_stDialogBoxExchangeInfo[i].cStr1, m_stDialogBoxExchangeInfo[i].dwV1, cNameStr, cSubStr1, cSubStr2);
				// If pointer over item then show this item data
				if ((msX >= sX + sXadd - 6) && (msX <= sX + sXadd + 42)
					&& (msY >= sY + 61) && (msY <= sY + 200))
				{
					wsprintf(cTxt, "%s", cNameStr);
					if (m_bIsSpecial)
					{
						if (m_bIsRare)
						{
							PutAlignedString(sX + 15, sX + 255, sY + 215, cTxt, 255, 208, 60);
							PutAlignedString(sX + 16, sX + 256, sY + 215, cTxt, 255, 208, 60);
						}
						else {
							PutAlignedString(sX + 15, sX + 155, sY + 215, cTxt, 0, 255, 50);
							PutAlignedString(sX + 16, sX + 156, sY + 215, cTxt, 0, 255, 50);
						}
					}
					else
					{
						PutAlignedString(sX + 15, sX + 155, sY + 215, cTxt, 35, 35, 35);
						PutAlignedString(sX + 16, sX + 156, sY + 215, cTxt, 35, 35, 35);
					}
					iLoc = 0;
					if (strlen(cSubStr1) != 0)
					{
						PutAlignedString(sX + 16, sX + 155, sY + 235 + iLoc, cSubStr1, 0, 0, 0);
						iLoc += 15;
					}
					if (strlen(cSubStr2) != 0)
					{
						PutAlignedString(sX + 16, sX + 155, sY + 235 + iLoc, cSubStr2, 0, 0, 0);
						iLoc += 15;
					}
					if (m_stDialogBoxExchangeInfo[i].sV3 != 1) // Item avec Quantity
					{
						if (m_stDialogBoxExchangeInfo[i].sV3 > 1)
						{
							DisplayCommaNumber_G_cTxt(m_stDialogBoxExchangeInfo[i].sV3);
							strcpy(cTxt2, G_cTxt);
						}
						else wsprintf(cTxt2, DRAW_DIALOGBOX_EXCHANGE2, m_stDialogBoxExchangeInfo[i].sV3);
						PutAlignedString(sX + 16, sX + 155, sY + 235 + iLoc, cTxt2, 35, 35, 35);
						iLoc += 15;
					}
					if (m_stDialogBoxExchangeInfo[i].sV5 != -1) // completion
					{	// Crafting Magins completion fix
						if (m_stDialogBoxExchangeInfo[i].sV1 == 22)
						{
							if ((m_stDialogBoxExchangeInfo[i].sV2 > 5)
								&& (m_stDialogBoxExchangeInfo[i].sV2 < 10))
							{
								wsprintf(cTxt, GET_ITEM_NAME2, (m_stDialogBoxExchangeInfo[i].sV7 - 100)); //Completion - 100
							}
						}
						else if (m_stDialogBoxExchangeInfo[i].sV1 == 6)
						{
							wsprintf(cTxt, GET_ITEM_NAME1, (m_stDialogBoxExchangeInfo[i].sV7 - 100)); //Purity
						}
						else
						{
							wsprintf(cTxt, GET_ITEM_NAME2, m_stDialogBoxExchangeInfo[i].sV7); //Completion
						}
						PutAlignedString(sX + 16, sX + 155, sY + 235 + iLoc, cTxt, 35, 35, 35);
						iLoc += 15;
					}
					if (iLoc < 45) // Endurance
					{
						wsprintf(cTxt, DRAW_DIALOGBOX_EXCHANGE3, m_stDialogBoxExchangeInfo[i].sV5, m_stDialogBoxExchangeInfo[i].sV6);
						PutAlignedString(sX + 16, sX + 155, sY + 235 + iLoc, cTxt, 35, 35, 35);
						iLoc += 15;
					}
				}
			}
		}
		if ((m_stDialogBoxExchangeInfo[0].sV1 != -1) && (m_stDialogBoxExchangeInfo[4].sV1 == -1))
		{
			PutAlignedString(sX, sX + szX, sY + 235 + 10, DRAW_DIALOGBOX_EXCHANGE9, 55, 25, 25); // Please wait until other player decides
			PutAlignedString(sX, sX + szX, sY + 250 + 10, DRAW_DIALOGBOX_EXCHANGE10, 55, 25, 25);// to exchange. If you want to cancel the
			PutAlignedString(sX, sX + szX, sY + 265 + 10, DRAW_DIALOGBOX_EXCHANGE11, 55, 25, 25);// exchange press the CANCEL button now.
			PutString_SprFont(sX + 220, sY + 310, "Exchange", 15, 15, 15);
		}
		else if ((m_stDialogBoxExchangeInfo[0].sV1 == -1) && (m_stDialogBoxExchangeInfo[4].sV1 != -1))
		{
			PutAlignedString(sX, sX + szX, sY + 205 + 10, DRAW_DIALOGBOX_EXCHANGE12, 55, 25, 25);// Other player offered an item exchange
			PutAlignedString(sX, sX + szX, sY + 220 + 10, DRAW_DIALOGBOX_EXCHANGE13, 55, 25, 25);// Select an item which you want to exc-
			PutAlignedString(sX, sX + szX, sY + 235 + 10, DRAW_DIALOGBOX_EXCHANGE14, 55, 25, 25);// hange with above item, drag it to the
			PutAlignedString(sX, sX + szX, sY + 250 + 10, DRAW_DIALOGBOX_EXCHANGE15, 55, 25, 25);// blank and press the EXCHANGE button.
			PutAlignedString(sX, sX + szX, sY + 265 + 10, DRAW_DIALOGBOX_EXCHANGE16, 55, 25, 25);// And you can also reject an offer by
			PutAlignedString(sX, sX + szX, sY + 280 + 10, DRAW_DIALOGBOX_EXCHANGE17, 55, 25, 25);// pressing the CANCEL button.
			PutString_SprFont(sX + 220, sY + 310, "Exchange", 15, 15, 15);
		}
		else if ((m_stDialogBoxExchangeInfo[0].sV1 != -1) && (m_stDialogBoxExchangeInfo[4].sV1 != -1))
		{
			PutAlignedString(sX, sX + szX, sY + 205 + 10, DRAW_DIALOGBOX_EXCHANGE18, 55, 25, 25);// The preparation for item exchange
			PutAlignedString(sX, sX + szX, sY + 220 + 10, DRAW_DIALOGBOX_EXCHANGE19, 55, 25, 25);// has been finished. Press the EXCHANGE
			PutAlignedString(sX, sX + szX, sY + 235 + 10, DRAW_DIALOGBOX_EXCHANGE20, 55, 25, 25);// button to exchange as above. Press the
			PutAlignedString(sX, sX + szX, sY + 250 + 10, DRAW_DIALOGBOX_EXCHANGE21, 55, 25, 25);// CANCEL button to cancel. Occasionally
			PutAlignedString(sX, sX + szX, sY + 265 + 10, DRAW_DIALOGBOX_EXCHANGE22, 55, 25, 25);// when you press the EXCHANGE button, you
			PutAlignedString(sX, sX + szX, sY + 280 + 10, DRAW_DIALOGBOX_EXCHANGE23, 55, 25, 25);// will not be able to cancel the exchange.
			if ((msX >= sX + 200) && (msX <= sX + 200 + DEF_BTNSZX) && (msY >= sY + 310) && (msY <= sY + 310 + DEF_BTNSZY))
				PutString_SprFont(sX + 220, sY + 310, "Exchange", 6, 6, 20);
			else PutString_SprFont(sX + 220, sY + 310, "Exchange", 0, 0, 7);
		}
		if ((msX >= sX + 450) && (msX <= sX + 450 + DEF_BTNSZX) && (msY >= sY + 310) && (msY <= sY + 310 + DEF_BTNSZY)
			&& (m_bIsDialogEnabled[41] == false))
			PutString_SprFont(sX + 450, sY + 310, "Cancel", 6, 6, 20);
		else PutString_SprFont(sX + 450, sY + 310, "Cancel", 0, 0, 7);
		break;

	case 2: // You have confirmed the exchange
		PutAlignedString(sX + 80, sX + 180, sY + 38, m_cPlayerName, 35, 55, 35); // my name
		if (m_stDialogBoxExchangeInfo[4].sV1 != -1) // Other name if applicable
			PutAlignedString(sX + 250, sX + 540, sY + 38, m_stDialogBoxExchangeInfo[4].cStr2, 35, 55, 35);
		for (i = 0; i<8; i++) // draw items
		{
			sXadd = (58 * i) + 48; if (i>3) sXadd += 20;
			if (m_stDialogBoxExchangeInfo[i].sV1 != -1)
			{
				cItemColor = m_stDialogBoxExchangeInfo[i].sV4;
				if (cItemColor == 0)
				{
					m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_stDialogBoxExchangeInfo[i].sV1]->PutSpriteFast(sX + sXadd, sY + 130, m_stDialogBoxExchangeInfo[i].sV2, dwTime);
				}
				else
				{
					switch (m_stDialogBoxExchangeInfo[i].sV1) {
					case 1: // Swds
					case 2: // Bows
					case 3: // Shields
					case 15: // Axes hammers
					case 17: // Wands
						m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_stDialogBoxExchangeInfo[i].sV1]->PutSpriteRGB(sX + sXadd, sY + 130
							, m_stDialogBoxExchangeInfo[i].sV2, m_wWR[cItemColor] - m_wR[0], m_wWG[cItemColor] - m_wG[0], m_wWB[cItemColor] - m_wB[0], dwTime);
						break;
					default: m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_stDialogBoxExchangeInfo[i].sV1]->PutSpriteRGB(sX + sXadd, sY + 130
						, m_stDialogBoxExchangeInfo[i].sV2, m_wR[cItemColor] - m_wR[0], m_wG[cItemColor] - m_wG[0], m_wB[cItemColor] - m_wB[0], dwTime);
						break;
					}
				}
				GetItemName(m_stDialogBoxExchangeInfo[i].cStr1, m_stDialogBoxExchangeInfo[i].dwV1, cNameStr, cSubStr1, cSubStr2);
				// If pointer over item then show this item data
				if ((msX >= sX + sXadd - 6) && (msX <= sX + sXadd + 42)
					&& (msY >= sY + 61) && (msY <= sY + 200))
				{
					wsprintf(cTxt, "%s", cNameStr);
					if (m_bIsSpecial)
					{
						PutAlignedString(sX + 15, sX + 155, sY + 215, cTxt, 0, 255, 50);
						PutAlignedString(sX + 16, sX + 156, sY + 215, cTxt, 0, 255, 50);
					}
					else
					{
						PutAlignedString(sX + 15, sX + 155, sY + 215, cTxt, 35, 35, 35);
						PutAlignedString(sX + 16, sX + 156, sY + 215, cTxt, 35, 35, 35);
					}
					iLoc = 0;
					if (strlen(cSubStr1) != 0)
					{
						PutAlignedString(sX + 16, sX + 155, sY + 235 + iLoc, cSubStr1, 0, 0, 0);
						iLoc += 15;
					}
					if (strlen(cSubStr2) != 0)
					{
						PutAlignedString(sX + 16, sX + 155, sY + 235 + iLoc, cSubStr2, 0, 0, 0);
						iLoc += 15;
					}
					if (m_stDialogBoxExchangeInfo[i].sV3 != 1) // Item avec Quantity
					{
						if (m_stDialogBoxExchangeInfo[i].sV3 > 1)
						{
							DisplayCommaNumber_G_cTxt(m_stDialogBoxExchangeInfo[i].sV3);
							strcpy(cTxt2, G_cTxt);
						}
						else wsprintf(cTxt2, DRAW_DIALOGBOX_EXCHANGE2, m_stDialogBoxExchangeInfo[i].sV3);
						PutAlignedString(sX + 16, sX + 155, sY + 235 + iLoc, cTxt2, 35, 35, 35);
						iLoc += 15;
					}
					if (m_stDialogBoxExchangeInfo[i].sV5 != -1) // completion
					{	// Crafting Magins completion fix
						if (m_stDialogBoxExchangeInfo[i].sV1 == 22)
						{
							if ((m_stDialogBoxExchangeInfo[i].sV2 > 5)
								&& (m_stDialogBoxExchangeInfo[i].sV2 < 10))
							{
								wsprintf(cTxt, GET_ITEM_NAME2, (m_stDialogBoxExchangeInfo[i].sV7 - 100)); //Completion - 100
							}
						}
						else if (m_stDialogBoxExchangeInfo[i].sV1 == 6)
						{
							wsprintf(cTxt, GET_ITEM_NAME1, (m_stDialogBoxExchangeInfo[i].sV7 - 100)); //Purity
						}
						else
						{
							wsprintf(cTxt, GET_ITEM_NAME2, m_stDialogBoxExchangeInfo[i].sV7); //Completion
						}
						PutAlignedString(sX + 16, sX + 155, sY + 235 + iLoc, cTxt, 35, 35, 35);
						iLoc += 15;
					}
					if (iLoc < 45) // Endurance
					{
						wsprintf(cTxt, DRAW_DIALOGBOX_EXCHANGE3, m_stDialogBoxExchangeInfo[i].sV5, m_stDialogBoxExchangeInfo[i].sV6);
						PutAlignedString(sX + 16, sX + 155, sY + 235 + iLoc, cTxt, 35, 35, 35);
						iLoc += 15;
					}
				}
			}
		}
		wsprintf(cTxt, DRAW_DIALOGBOX_EXCHANGE33, m_stDialogBoxExchangeInfo[4].cStr2);
		PutAlignedString(sX, sX + szX, sY + 205 + 10, cTxt, 55, 25, 25);                     // Please wait until %s agrees to
		PutAlignedString(sX, sX + szX, sY + 220 + 10, DRAW_DIALOGBOX_EXCHANGE34, 55, 25, 25);// exchange. The exchange can't be achieved
		PutAlignedString(sX, sX + szX, sY + 235 + 10, DRAW_DIALOGBOX_EXCHANGE35, 55, 25, 25);// unless both people agree.
		PutAlignedString(sX, sX + szX, sY + 250 + 10, DRAW_DIALOGBOX_EXCHANGE36, 55, 25, 25);//  If other player does not decide to exchange
		PutAlignedString(sX, sX + szX, sY + 265 + 10, DRAW_DIALOGBOX_EXCHANGE37, 55, 25, 25);// you can cancel the exchange by pressing the
		PutAlignedString(sX, sX + szX, sY + 280 + 10, DRAW_DIALOGBOX_EXCHANGE38, 55, 25, 25);// CANCEL button. But if other player already
		PutAlignedString(sX, sX + szX, sY + 295 + 10, DRAW_DIALOGBOX_EXCHANGE39, 55, 25, 25);// decided to exchange, you can't cancel anymore
		break;
	}
#endif
}

void CGame::DrawDialogBox_Fishing(short msX, short msY)
{

	short sX, sY;
	UINT32 dwTime = m_dwCurTime;
	char  cTxt[120];

	sX = m_stDialogBoxInfo[24].sX;
	sY = m_stDialogBoxInfo[24].sY;

	DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME5, sX, sY, 2);

	char cStr1[64], cStr2[64], cStr3[64];
	GetItemName(m_stDialogBoxInfo[24].cStr, 0, cStr1, cStr2, cStr3);

	switch (m_stDialogBoxInfo[24].cMode) {
	case 0:
		m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_stDialogBoxInfo[24].sV3]->PutSpriteFast(sX + 18 + 35, sY + 18 + 17, m_stDialogBoxInfo[24].sV4, dwTime);
		wsprintf(cTxt, "%s", cStr1);
		PutString(sX + 98, sY + 14, cTxt, RGB(255, 255, 255));
		wsprintf(cTxt, DRAW_DIALOGBOX_FISHING1, m_stDialogBoxInfo[24].sV2);
		PutString(sX + 98, sY + 28, cTxt, RGB(0, 0, 0));
		PutString(sX + 97, sY + 43, DRAW_DIALOGBOX_FISHING2, RGB(0, 0, 0));
		wsprintf(cTxt, "%d %%", m_stDialogBoxInfo[24].sV1);
		PutString_SprFont(sX + 157, sY + 40, cTxt, 10, 0, 0);
		if ((msX >= sX + 160) && (msX <= sX + 253) && (msY >= sY + 70) && (msY <= sY + 90))
			PutString_SprFont(sX + 160, sY + 70, "Try Now!", 6, 6, 20);
		else PutString_SprFont(sX + 160, sY + 70, "Try Now!", 0, 0, 7);
		break;
	}

}

void CGame::DrawDialogBox_GuildMenu(short msX, short msY)
{
	short sX, sY, szX;
	int iAdjX, iAdjY;

	sX = m_stDialogBoxInfo[7].sX;
	sY = m_stDialogBoxInfo[7].sY;
	szX = m_stDialogBoxInfo[7].sSizeX;

	iAdjX = -13;
	iAdjY = 30;
	DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME6, sX, sY, 2);
	DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_TEXT2, sX, sY, 19);

	switch (m_stDialogBoxInfo[7].cMode) {
	case 0:
		if ((m_iGuildRank == -1) && /*(m_iCharisma >= 20) &&*/ (m_iLevel >= 100)) {
			if ((msX > sX + iAdjX + 80) && (msX < sX + iAdjX + 210) && (msY > sY + iAdjY + 63) && (msY < sY + iAdjY + 78))
				PutAlignedString(sX, sX + szX, sY + iAdjY + 65, DRAW_DIALOGBOX_GUILDMENU1, 255, 255, 255);//"
			else PutAlignedString(sX, sX + szX, sY + iAdjY + 65, DRAW_DIALOGBOX_GUILDMENU1, 4, 0, 50);//"
		}
		else PutAlignedString(sX, sX + szX, sY + iAdjY + 65, DRAW_DIALOGBOX_GUILDMENU1, 65, 65, 65);//"

		if (m_iGuildRank == 0) {
			if ((msX > sX + iAdjX + 72) && (msX < sX + iAdjX + 222) && (msY > sY + iAdjY + 82) && (msY < sY + iAdjY + 99))
				PutAlignedString(sX, sX + szX, sY + iAdjY + 85, DRAW_DIALOGBOX_GUILDMENU4, 255, 255, 255);//"
			else PutAlignedString(sX, sX + szX, sY + iAdjY + 85, DRAW_DIALOGBOX_GUILDMENU4, 4, 0, 50);//"
		}
		else PutAlignedString(sX, sX + szX, sY + iAdjY + 85, DRAW_DIALOGBOX_GUILDMENU4, 65, 65, 65);//"

		if ((msX > sX + iAdjX + 61) && (msX < sX + iAdjX + 226) && (msY > sY + iAdjY + 103) && (msY < sY + iAdjY + 120))
			PutAlignedString(sX, sX + szX, sY + iAdjY + 105, DRAW_DIALOGBOX_GUILDMENU7, 255, 255, 255);//"
		else PutAlignedString(sX, sX + szX, sY + iAdjY + 105, DRAW_DIALOGBOX_GUILDMENU7, 4, 0, 50);//"

		if ((msX > sX + iAdjX + 60) && (msX < sX + iAdjX + 227) && (msY > sY + iAdjY + 123) && (msY < sY + iAdjY + 139))
			PutAlignedString(sX, sX + szX, sY + iAdjY + 125, DRAW_DIALOGBOX_GUILDMENU9, 255, 255, 255);//"
		else PutAlignedString(sX, sX + szX, sY + iAdjY + 125, DRAW_DIALOGBOX_GUILDMENU9, 4, 0, 50);//"

		/*if (m_iGuildRank == 0 && m_iFightzoneNumber == 0) {
			if ((msX > sX + iAdjX + 72) && (msX < sX + iAdjX + 228) && (msY > sY + iAdjY + 143) && (msY < sY + iAdjY + 169))
				PutAlignedString(sX, sX + szX, sY + iAdjY + 145, DRAW_DIALOGBOX_GUILDMENU11, 255, 255, 255);//"
			else PutAlignedString(sX, sX + szX, sY + iAdjY + 145, DRAW_DIALOGBOX_GUILDMENU11, 4, 0, 50);//"

		}
		else if (m_iGuildRank == 0 && m_iFightzoneNumber > 0) {
			if ((msX > sX + iAdjX + 72) && (msX < sX + iAdjX + 216) && (msY > sY + iAdjY + 143) && (msY < sY + iAdjY + 169))
				PutAlignedString(sX, sX + szX, sY + iAdjY + 145, DRAW_DIALOGBOX_GUILDMENU13, 255, 255, 255);//"
			else PutAlignedString(sX, sX + szX, sY + iAdjY + 145, DRAW_DIALOGBOX_GUILDMENU13, 4, 0, 50);//"

		}
		else if (m_iFightzoneNumber < 0) {
			PutAlignedString(sX, sX + szX, sY + iAdjY + 145, DRAW_DIALOGBOX_GUILDMENU13, 65, 65, 65);//"
		}
		else PutAlignedString(sX, sX + szX, sY + iAdjY + 145, DRAW_DIALOGBOX_GUILDMENU11, 65, 65, 65);*/
		
		PutAlignedString(sX, sX + szX, sY + iAdjY + 205, DRAW_DIALOGBOX_GUILDMENU17);//"
		break;

	case 1:
		PutAlignedString(sX + 24, sX + 239, sY + 125, DRAW_DIALOGBOX_GUILDMENU18, 4, 0, 50);//"
		PutString(sX + 75, sY + 144, "____________________", RGB(25, 35, 25));

		if (iGetTopDialogBoxIndex() != 7)
		{
			//PutString(sX + 75, sY + 140, m_cGuildName, RGB(255, 255, 255), 16, false, 2);
			PutString(sX + 75, sY + 140, m_cGuildName, RGB(255, 255, 255));
		}
		PutAlignedString(sX + 24, sX + 239, sY + 175, "Making a Guild costs 100.000 Gold.", 4, 0, 50);//"

		if ((msX >= sX + DEF_LBTNPOSX) && (msX <= sX + DEF_LBTNPOSX + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY)) {
			if ((strcmp(m_cGuildName, "NONE") == 0) || (strlen(m_cGuildName) == 0)) {
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 24);//Create Gray Button
			}
			else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 25);//Create Highlight Button
		}
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 24);//Create Gray Button

		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 17);//Red Cancel Button
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 16);//Gray Cancel Button
		break;

	case 2:
		PutAlignedString(sX, sX + szX, sY + 140, DRAW_DIALOGBOX_GUILDMENU19, 4, 0, 50);//"
		break;
	case 3:
		PutAlignedString(sX, sX + szX, sY + 125, DRAW_DIALOGBOX_GUILDMENU20, 4, 0, 50);//"
		PutAlignedString(sX, sX + szX, sY + 140, m_cGuildName, 35, 35, 35);
		PutAlignedString(sX, sX + szX, sY + 144, "____________________", 25, 35, 25);
		PutAlignedString(sX, sX + szX, sY + 160, DRAW_DIALOGBOX_GUILDMENU21, 4, 0, 50);//"

		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
		break;
	case 4:
		PutAlignedString(sX, sX + szX, sY + 135, DRAW_DIALOGBOX_GUILDMENU22, 4, 0, 50);//"
		PutAlignedString(sX, sX + szX, sY + 150, DRAW_DIALOGBOX_GUILDMENU23, 4, 0, 50);//"

		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
		break;
	case 5:
		PutAlignedString(sX, sX + szX, sY + 90, DRAW_DIALOGBOX_GUILDMENU24);
		PutAlignedString(sX, sX + szX, sY + 105, m_cGuildName, 35, 35, 35);
		PutAlignedString(sX, sX + szX, sY + 109, "____________________", 0, 0, 0);
		PutAlignedString(sX, sX + szX, sY + 130, DRAW_DIALOGBOX_GUILDMENU25, 4, 0, 50);//"
		PutAlignedString(sX, sX + szX, sY + 145, DRAW_DIALOGBOX_GUILDMENU26, 4, 0, 50);//"
		PutAlignedString(sX, sX + szX, sY + 160, DRAW_DIALOGBOX_GUILDMENU27, 4, 0, 50);//"
		PutAlignedString(sX, sX + szX, sY + 185, DRAW_DIALOGBOX_GUILDMENU28, 4, 0, 50);//"

		if ((msX >= sX + DEF_LBTNPOSX) && (msX <= sX + DEF_LBTNPOSX + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 19);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 18);

		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 3);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 2);
		break;
	case 6:
		PutAlignedString(sX, sX + szX, sY + 140, DRAW_DIALOGBOX_GUILDMENU29, 4, 0, 50);//"
		break;
	case 7:
		PutAlignedString(sX, sX + szX, sY + 140, DRAW_DIALOGBOX_GUILDMENU30, 4, 0, 50);//"
		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
		break;
	case 8:
		PutAlignedString(sX, sX + szX, sY + 140, DRAW_DIALOGBOX_GUILDMENU31, 4, 0, 50);//"
		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
		break;
	case 9:
		PutAlignedString(sX, sX + szX, sY + iAdjY + 60, DRAW_DIALOGBOX_GUILDMENU32, 4, 0, 50);//"
		PutAlignedString(sX, sX + szX, sY + iAdjY + 75, DRAW_DIALOGBOX_GUILDMENU33, 4, 0, 50);//"
		PutAlignedString(sX, sX + szX, sY + iAdjY + 90, DRAW_DIALOGBOX_GUILDMENU34, 4, 0, 50);//"
		PutAlignedString(sX, sX + szX, sY + iAdjY + 105, DRAW_DIALOGBOX_GUILDMENU35, 4, 0, 50);//"
		PutAlignedString(sX, sX + szX, sY + iAdjY + 130, DRAW_DIALOGBOX_GUILDMENU36, 4, 0, 50);//"
		if ((msX >= sX + DEF_LBTNPOSX) && (msX <= sX + DEF_LBTNPOSX + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 31);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 30);
		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 17);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 16);
		break;
	case 10:
		PutAlignedString(sX, sX + szX, sY + 140, DRAW_DIALOGBOX_GUILDMENU37, 4, 0, 50);//"
		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
		break;
	case 11:
		PutAlignedString(sX, sX + szX, sY + iAdjY + 60, DRAW_DIALOGBOX_GUILDMENU38, 4, 0, 50);//"
		PutAlignedString(sX, sX + szX, sY + iAdjY + 75, DRAW_DIALOGBOX_GUILDMENU39, 4, 0, 50);//"
		PutAlignedString(sX, sX + szX, sY + iAdjY + 90, DRAW_DIALOGBOX_GUILDMENU40, 4, 0, 50);//"
		PutAlignedString(sX, sX + szX, sY + iAdjY + 105, DRAW_DIALOGBOX_GUILDMENU41, 4, 0, 50);//
		PutAlignedString(sX, sX + szX, sY + iAdjY + 130, DRAW_DIALOGBOX_GUILDMENU42, 4, 0, 50);//"
		if ((msX >= sX + DEF_LBTNPOSX) && (msX <= sX + DEF_LBTNPOSX + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 31);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 30);
		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 17);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 16);
		break;
	case 12:
		PutAlignedString(sX, sX + szX, sY + 140, DRAW_DIALOGBOX_GUILDMENU43, 4, 0, 50);//"
		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
		break;

	case 13:
		PutAlignedString(sX, sX + szX, sY + iAdjY + 40, DRAW_DIALOGBOX_GUILDMENU44, 4, 0, 50);//"
		PutAlignedString(sX, sX + szX, sY + iAdjY + 55, DRAW_DIALOGBOX_GUILDMENU45, 4, 0, 50);//"
		PutAlignedString(sX, sX + szX, sY + iAdjY + 70, DRAW_DIALOGBOX_GUILDMENU46, 4, 0, 50);//"
		PutAlignedString(sX, sX + szX, sY + iAdjY + 85, DRAW_DIALOGBOX_GUILDMENU47, 4, 0, 50);//"
		PutAlignedString(sX, sX + szX, sY + iAdjY + 100, DRAW_DIALOGBOX_GUILDMENU48, 4, 0, 50);//"
		PutAlignedString(sX, sX + szX, sY + iAdjY + 115, DRAW_DIALOGBOX_GUILDMENU49, 4, 0, 50);//"
		PutAlignedString(sX, sX + szX, sY + iAdjY + 130, DRAW_DIALOGBOX_GUILDMENU50, 4, 0, 50);//"

		if ((msX > sX + iAdjX + 65) && (msX < sX + iAdjX + 137) && (msY > sY + iAdjY + 168) && (msY < sY + iAdjY + 185))
			PutString(sX + iAdjX + 65 + 25 - 23, sY + iAdjY + 170, DRAW_DIALOGBOX_GUILDMENU51, RGB(255, 255, 255));
		else PutString(sX + iAdjX + 65 + 25 - 23, sY + iAdjY + 170, DRAW_DIALOGBOX_GUILDMENU51, RGB(4, 0, 50));

		if ((msX > sX + iAdjX + 150) && (msX < sX + iAdjX + 222) && (msY > sY + iAdjY + 168) && (msY < sY + iAdjY + 185))
			PutString(sX + iAdjX + 150 + 25 - 23, sY + iAdjY + 170, DRAW_DIALOGBOX_GUILDMENU53, RGB(255, 255, 255));
		else PutString(sX + iAdjX + 150 + 25 - 23, sY + iAdjY + 170, DRAW_DIALOGBOX_GUILDMENU53, RGB(4, 0, 50));

		if ((msX > sX + iAdjX + 65) && (msX < sX + iAdjX + 137) && (msY > sY + iAdjY + 188) && (msY < sY + iAdjY + 205))
			PutString(sX + iAdjX + 65 + 25 - 23, sY + iAdjY + 190, DRAW_DIALOGBOX_GUILDMENU55, RGB(255, 255, 255));
		else PutString(sX + iAdjX + 65 + 25 - 23, sY + iAdjY + 190, DRAW_DIALOGBOX_GUILDMENU55, RGB(4, 0, 50));

		if ((msX > sX + iAdjX + 150) && (msX < sX + iAdjX + 222) && (msY > sY + iAdjY + 188) && (msY < sY + iAdjY + 205))
			PutString(sX + iAdjX + 150 + 25 - 23, sY + iAdjY + 190, DRAW_DIALOGBOX_GUILDMENU57, RGB(255, 255, 255));
		else PutString(sX + iAdjX + 150 + 25 - 23, sY + iAdjY + 190, DRAW_DIALOGBOX_GUILDMENU57, RGB(4, 0, 50));

		if ((msX > sX + iAdjX + 65) && (msX < sX + iAdjX + 137) && (msY > sY + iAdjY + 208) && (msY < sY + iAdjY + 225))
			PutString(sX + iAdjX + 65 + 25 - 23, sY + iAdjY + 210, DRAW_DIALOGBOX_GUILDMENU59, RGB(255, 255, 255));
		else PutString(sX + iAdjX + 65 + 25 - 23, sY + iAdjY + 210, DRAW_DIALOGBOX_GUILDMENU59, RGB(4, 0, 50));

		if ((msX > sX + iAdjX + 150) && (msX < sX + iAdjX + 222) && (msY > sY + iAdjY + 208) && (msY < sY + iAdjY + 225))
			PutString(sX + iAdjX + 150 + 25 - 23, sY + iAdjY + 210, DRAW_DIALOGBOX_GUILDMENU61, RGB(255, 255, 255));
		else PutString(sX + iAdjX + 150 + 25 - 23, sY + iAdjY + 210, DRAW_DIALOGBOX_GUILDMENU61, RGB(4, 0, 50));

		if ((msX > sX + iAdjX + 65) && (msX < sX + iAdjX + 137) && (msY > sY + iAdjY + 228) && (msY < sY + iAdjY + 245))
			PutString(sX + iAdjX + 65 + 25 - 23, sY + iAdjY + 230, DRAW_DIALOGBOX_GUILDMENU63, RGB(255, 255, 255));
		else PutString(sX + iAdjX + 65 + 25 - 23, sY + iAdjY + 230, DRAW_DIALOGBOX_GUILDMENU63, RGB(4, 0, 50));

		if ((msX > sX + iAdjX + 150) && (msX < sX + iAdjX + 222) && (msY > sY + iAdjY + 228) && (msY < sY + iAdjY + 245))
			PutString(sX + iAdjX + 150 + 25 - 23, sY + iAdjY + 230, DRAW_DIALOGBOX_GUILDMENU65, RGB(255, 255, 255));
		else PutString(sX + iAdjX + 150 + 25 - 23, sY + iAdjY + 230, DRAW_DIALOGBOX_GUILDMENU65, RGB(4, 0, 50));

		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 17);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 16);
		break;

	case 14:
		PutAlignedString(sX, sX + szX, sY + 130, DRAW_DIALOGBOX_GUILDMENU66, 4, 0, 50);//"
		PutAlignedString(sX, sX + szX, sY + 145, DRAW_DIALOGBOX_GUILDMENU67, 4, 0, 50);//"
		PutAlignedString(sX, sX + szX, sY + 160, DRAW_DIALOGBOX_GUILDMENU68, 4, 0, 50);//
		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
		break;

	case 15:
		PutAlignedString(sX, sX + szX, sY + 135, DRAW_DIALOGBOX_GUILDMENU69, 4, 0, 50);//"
		PutAlignedString(sX, sX + szX, sY + 150, DRAW_DIALOGBOX_GUILDMENU70, 4, 0, 50);//"
		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
		break;

	case 16:
		PutAlignedString(sX, sX + szX, sY + 135, DRAW_DIALOGBOX_GUILDMENU71, 4, 0, 50);//"
		PutAlignedString(sX, sX + szX, sY + 150, DRAW_DIALOGBOX_GUILDMENU72, 4, 0, 50);//"
		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
		break;

	case 17:
		PutAlignedString(sX, sX + szX, sY + 140, DRAW_DIALOGBOX_GUILDMENU73, 4, 0, 50);//"
		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
		break;

	case 18:
		PutAlignedString(sX, sX + szX, sY + 140, DRAW_DIALOGBOX_GUILDMENU74, 4, 0, 50);//"
		break;

	case 19:
		if (m_iFightzoneNumber >0)
			bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_REQ_GETOCCUPYFIGHTZONETICKET, 0, 0, 0, 0, 0);
		m_stDialogBoxInfo[7].cMode = 0;
		break;

	case 20:
		PutAlignedString(sX, sX + szX, sY + 125, DRAW_DIALOGBOX_GUILDMENU75, 4, 0, 50);//"
		PutString(sX + 75, sY + 144, "____________________", RGB(25, 35, 25));
		PutString(sX + 75, sY + 140, m_cGuildName, RGB(255, 255, 255), false, 2);
		if ((msX >= sX + DEF_LBTNPOSX) && (msX <= sX + DEF_LBTNPOSX + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 25);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 24);
		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 17);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 16);
		break;

	case 21:
		PutAlignedString(sX, sX + szX, sY + iAdjY + 95, DRAW_DIALOGBOX_GUILDMENU76, 4, 0, 50);//"
		PutAlignedString(sX, sX + szX, sY + iAdjY + 110, DRAW_DIALOGBOX_GUILDMENU77, 4, 0, 50);//"
		PutAlignedString(sX, sX + szX, sY + iAdjY + 135, DRAW_DIALOGBOX_GUILDMENU78, 4, 0, 50);//"
		PutAlignedString(sX, sX + szX, sY + iAdjY + 150, DRAW_DIALOGBOX_GUILDMENU79, 4, 0, 50);//"
		PutAlignedString(sX, sX + szX, sY + iAdjY + 165, DRAW_DIALOGBOX_GUILDMENU80, 4, 0, 50);//"
		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
		break;

	case 22:
		PutAlignedString(sX, sX + szX, sY + 140, DRAW_DIALOGBOX_GUILDMENU81, 4, 0, 50);//"
		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
		break;
	}
}

void CGame::DrawDialogBox_GuildOperation(short msX, short msY)
{
	short sX, sY;

	sX = m_stDialogBoxInfo[8].sX;
	sY = m_stDialogBoxInfo[8].sY;

	DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME6, sX, sY, 0);
	DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_TEXT2, sX, sY, 19);

	switch (m_stGuildOpList[0].cOpMode) {
	case 1:
		PutAlignedString(sX + 24, sX + 248, sY + 50, DRAW_DIALOGBOX_GUILD_OPERATION1);
		PutAlignedString(sX + 24, sX + 248, sY + 65, m_stGuildOpList[0].cName, 35, 35, 35);
		PutAlignedString(sX + 24, sX + 248, sY + 69, "____________________", 0, 0, 0);
		PutAlignedString(sX + 24, sX + 248, sY + 90, DRAW_DIALOGBOX_GUILD_OPERATION2);
		PutAlignedString(sX + 24, sX + 248, sY + 105, DRAW_DIALOGBOX_GUILD_OPERATION3);
		PutAlignedString(sX + 24, sX + 248, sY + 120, DRAW_DIALOGBOX_GUILD_OPERATION4);
		PutAlignedString(sX + 24, sX + 248, sY + 160, DRAW_DIALOGBOX_GUILD_OPERATION5, 55, 25, 25);

		if ((msX >= sX + DEF_LBTNPOSX) && (msX <= sX + DEF_LBTNPOSX + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 33);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 32);

		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 35);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 34);
		break;

	case 2:
		PutAlignedString(sX + 24, sX + 248, sY + 50, DRAW_DIALOGBOX_GUILD_OPERATION6);
		PutAlignedString(sX + 24, sX + 248, sY + 65, m_stGuildOpList[0].cName, 35, 35, 35);
		PutAlignedString(sX + 24, sX + 248, sY + 69, "____________________", 0, 0, 0);
		PutAlignedString(sX + 24, sX + 248, sY + 90, DRAW_DIALOGBOX_GUILD_OPERATION7);
		PutAlignedString(sX + 24, sX + 248, sY + 105, DRAW_DIALOGBOX_GUILD_OPERATION8);
		PutAlignedString(sX + 24, sX + 248, sY + 120, DRAW_DIALOGBOX_GUILD_OPERATION9);
		PutAlignedString(sX + 24, sX + 248, sY + 160, DRAW_DIALOGBOX_GUILD_OPERATION10, 55, 25, 25);

		if ((msX >= sX + DEF_LBTNPOSX) && (msX <= sX + DEF_LBTNPOSX + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 33);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 32);

		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 35);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 34);
		break;

	case 3:
		PutAlignedString(sX + 24, sX + 248, sY + 50, DRAW_DIALOGBOX_GUILD_OPERATION11);
		PutAlignedString(sX + 24, sX + 248, sY + 65, m_stGuildOpList[0].cName, 35, 35, 35);
		PutAlignedString(sX + 24, sX + 248, sY + 69, "____________________", 0, 0, 0);
		PutAlignedString(sX + 24, sX + 248, sY + 90, DRAW_DIALOGBOX_GUILD_OPERATION12);
		PutAlignedString(sX + 24, sX + 248, sY + 105, DRAW_DIALOGBOX_GUILD_OPERATION13);

		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
		break;

	case 4:
		PutAlignedString(sX + 24, sX + 248, sY + 50, DRAW_DIALOGBOX_GUILD_OPERATION14);
		PutAlignedString(sX + 24, sX + 248, sY + 65, m_stGuildOpList[0].cName, 35, 35, 35);
		PutAlignedString(sX + 24, sX + 248, sY + 69, "____________________", 0, 0, 0);
		PutAlignedString(sX + 24, sX + 248, sY + 90, DRAW_DIALOGBOX_GUILD_OPERATION15);
		PutAlignedString(sX + 24, sX + 248, sY + 105, DRAW_DIALOGBOX_GUILD_OPERATION16);

		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
		break;

	case 5:
		PutAlignedString(sX + 24, sX + 248, sY + 50, DRAW_DIALOGBOX_GUILD_OPERATION17);
		PutAlignedString(sX + 24, sX + 248, sY + 65, m_stGuildOpList[0].cName, 35, 35, 35);
		PutAlignedString(sX + 24, sX + 248, sY + 69, "____________________", 0, 0, 0);
		PutAlignedString(sX + 24, sX + 248, sY + 90, DRAW_DIALOGBOX_GUILD_OPERATION18);
		PutAlignedString(sX + 24, sX + 248, sY + 105, DRAW_DIALOGBOX_GUILD_OPERATION19);
		PutAlignedString(sX + 24, sX + 248, sY + 120, DRAW_DIALOGBOX_GUILD_OPERATION20);

		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
		break;

	case 6:
		PutAlignedString(sX + 24, sX + 248, sY + 50, DRAW_DIALOGBOX_GUILD_OPERATION21);
		PutAlignedString(sX + 24, sX + 248, sY + 65, m_stGuildOpList[0].cName, 35, 35, 35);
		PutAlignedString(sX + 24, sX + 248, sY + 69, "____________________", 0, 0, 0);
		PutAlignedString(sX + 24, sX + 248, sY + 90, DRAW_DIALOGBOX_GUILD_OPERATION22);
		PutAlignedString(sX + 24, sX + 248, sY + 105, DRAW_DIALOGBOX_GUILD_OPERATION23);

		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
		break;

	case 7:
		PutAlignedString(sX + 24, sX + 248, sY + 50, DRAW_DIALOGBOX_GUILD_OPERATION24);
		PutAlignedString(sX + 24, sX + 248, sY + 90, DRAW_DIALOGBOX_GUILD_OPERATION25);
		PutAlignedString(sX + 24, sX + 248, sY + 105, DRAW_DIALOGBOX_GUILD_OPERATION26);
		PutAlignedString(sX + 24, sX + 248, sY + 120, DRAW_DIALOGBOX_GUILD_OPERATION27);

		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
		break;
	}
}

void CGame::DrawDialogBox_MagicShop(short msX, short msY, short msZ)
{
	short sX, sY;
	UINT32 dwTime = m_dwCurTime;
	int  i;

	int  iCPivot, iYloc;
	char cTxt[120], cMana[10];

	sX = m_stDialogBoxInfo[16].sX;
	sY = m_stDialogBoxInfo[16].sY;


	DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME8, sX, sY, 1);
	DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_TEXT2, sX, sY, 14);

	if (iGetTopDialogBoxIndex() == 16 && msZ != 0)
	{
		if (msZ > 0) m_stDialogBoxInfo[16].sView--;
		if (msZ < 0) m_stDialogBoxInfo[16].sView++;
		m_DInput.m_sZ = 0;
	}
	if (m_stDialogBoxInfo[16].sView < 0) m_stDialogBoxInfo[16].sView = 9;
	if (m_stDialogBoxInfo[16].sView > 9) m_stDialogBoxInfo[16].sView = 0;

	PutString(sX - 20 + 60 - 17, sY - 35 + 90, DRAW_DIALOGBOX_MAGICSHOP11, RGB(0, 0, 0));//"Spell Name"
	PutString(sX - 20 + 232 - 20, sY - 35 + 90, DRAW_DIALOGBOX_MAGICSHOP12, RGB(0, 0, 0));//"Req.Int"
	PutString(sX - 20 + 270, sY - 35 + 90, DRAW_DIALOGBOX_MAGICSHOP13, RGB(0, 0, 0));//"Cost"

	iCPivot = m_stDialogBoxInfo[16].sView * 10;

	iYloc = 0;
	for (i = 0; i < 9; i++) {
		if ((m_pMagicCfgList[iCPivot + i] != 0) && (m_pMagicCfgList[iCPivot + i]->m_bIsVisible)) {
			wsprintf(cTxt, "%s", m_pMagicCfgList[iCPivot + i]->m_cName);

			m_Misc.ReplaceString(cTxt, '-', ' ');
			if (m_cMagicMastery[iCPivot + i] != 0) {
				if (m_Misc.bCheckIMEString(cTxt) == false)
				{
					PutString(sX + 24, sY + 73 + iYloc, cTxt, RGB(41, 16, 41));
					PutString(sX + 25, sY + 73 + iYloc, cTxt, RGB(41, 16, 41));
				}
				else PutString_SprFont(sX + 24, sY + 70 + iYloc, cTxt, 5, 5, 5);
				wsprintf(cMana, "%3d", m_pMagicCfgList[iCPivot + i]->m_sValue2);
				PutString_SprFont(sX + 200, sY + 70 + iYloc, cMana, 5, 5, 5);
				wsprintf(cMana, "%3d", m_pMagicCfgList[iCPivot + i]->m_sValue3);
				PutString_SprFont(sX + 241, sY + 70 + iYloc, cMana, 5, 5, 5);
			}
			else
				if ((msX >= sX + 24) && (msX <= sX + 24 + 135) && (msY >= sY + 70 + iYloc) && (msY <= sY + 70 + 14 + iYloc)) {
					if (m_Misc.bCheckIMEString(cTxt) == false)
					{
						PutString(sX + 24, sY + 73 + iYloc, cTxt, RGB(255, 255, 255));
						PutString(sX + 25, sY + 73 + iYloc, cTxt, RGB(255, 255, 255));
					}
					else PutString_SprFont2(sX - 20 + 44, sY + 70 + iYloc, cTxt, 255, 255, 255);
					wsprintf(cMana, "%3d", m_pMagicCfgList[iCPivot + i]->m_sValue2);
					PutString_SprFont2(sX - 20 + 220, sY + 70 + iYloc, cMana, 255, 255, 255);
					wsprintf(cMana, "%3d", m_pMagicCfgList[iCPivot + i]->m_sValue3);
					PutString_SprFont2(sX - 20 + 261, sY + 70 + iYloc, cMana, 255, 255, 255);
				}
				else {
					if (m_Misc.bCheckIMEString(cTxt) == false)
					{
						PutString(sX + 24, sY + 73 + iYloc, cTxt, RGB(8, 0, 66));
						PutString(sX + 25, sY + 73 + iYloc, cTxt, RGB(8, 0, 66));
					}
					else PutString_SprFont2(sX - 20 + 44, sY + 70 + iYloc, cTxt, 19, 104, 169);
					wsprintf(cMana, "%3d", m_pMagicCfgList[iCPivot + i]->m_sValue2);
					PutString_SprFont2(sX - 20 + 220, sY + 70 + iYloc, cMana, 19, 104, 169);
					wsprintf(cMana, "%3d", m_pMagicCfgList[iCPivot + i]->m_sValue3);
					PutString_SprFont2(sX - 20 + 261, sY + 70 + iYloc, cMana, 19, 104, 169);
				}
				iYloc += 18;
		}
	}

	m_pSprite[DEF_SPRID_INTERFACE_SPRFONTS]->PutSpriteFast(sX + 55, sY + 250, 19, dwTime);

	switch (m_stDialogBoxInfo[16].sView) {
	case 0: m_pSprite[DEF_SPRID_INTERFACE_SPRFONTS]->PutSpriteFast(sX - 20 + 44 + 31, sY + 250, 20, dwTime); break;
	case 1: m_pSprite[DEF_SPRID_INTERFACE_SPRFONTS]->PutSpriteFast(sX - 20 + 57 + 31, sY + 250, 21, dwTime); break;
	case 2: m_pSprite[DEF_SPRID_INTERFACE_SPRFONTS]->PutSpriteFast(sX - 20 + 75 + 31, sY + 250, 22, dwTime); break;
	case 3: m_pSprite[DEF_SPRID_INTERFACE_SPRFONTS]->PutSpriteFast(sX - 20 + 100 + 31, sY + 250, 23, dwTime); break;
	case 4: m_pSprite[DEF_SPRID_INTERFACE_SPRFONTS]->PutSpriteFast(sX - 20 + 120 + 31, sY + 250, 24, dwTime); break;
	case 5: m_pSprite[DEF_SPRID_INTERFACE_SPRFONTS]->PutSpriteFast(sX - 20 + 135 + 31, sY + 250, 25, dwTime); break;
	case 6: m_pSprite[DEF_SPRID_INTERFACE_SPRFONTS]->PutSpriteFast(sX - 20 + 156 + 31, sY + 250, 26, dwTime); break;
	case 7: m_pSprite[DEF_SPRID_INTERFACE_SPRFONTS]->PutSpriteFast(sX - 20 + 183 + 31, sY + 250, 27, dwTime); break;
	case 8: m_pSprite[DEF_SPRID_INTERFACE_SPRFONTS]->PutSpriteFast(sX - 20 + 216 + 31, sY + 250, 28, dwTime); break;
	case 9: m_pSprite[DEF_SPRID_INTERFACE_SPRFONTS]->PutSpriteFast(sX - 20 + 236 + 31, sY + 250, 29, dwTime); break;
	}

	PutAlignedString(sX, sX + m_stDialogBoxInfo[16].sSizeX, sY + 275, DRAW_DIALOGBOX_MAGICSHOP14);
}

void CGame::DrawDialogBox_ShutDownMsg(short msX, short msY)
{
	short sX, sY, szX;

	sX = m_stDialogBoxInfo[25].sX;
	sY = m_stDialogBoxInfo[25].sY;
	szX = m_stDialogBoxInfo[25].sSizeX;

	DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME8, sX, sY, 2);

	switch (m_stDialogBoxInfo[25].cMode) {
	case 1:
		ZeroMemory(G_cTxt, sizeof(G_cTxt));
		if (m_stDialogBoxInfo[25].sV1 != 0) wsprintf(G_cTxt, DRAW_DIALOGBOX_NOTICEMSG1, m_stDialogBoxInfo[25].sV1);
		else strcpy(G_cTxt, DRAW_DIALOGBOX_NOTICEMSG2);
		PutAlignedString(sX, sX + szX, sY + 31, G_cTxt, 100, 10, 10);
		PutAlignedString(sX, sX + szX, sY + 48, DRAW_DIALOGBOX_NOTICEMSG3);
		PutAlignedString(sX, sX + szX, sY + 65, DRAW_DIALOGBOX_NOTICEMSG4);
		PutAlignedString(sX, sX + szX, sY + 82, DRAW_DIALOGBOX_NOTICEMSG5);
		PutAlignedString(sX, sX + szX, sY + 99, DRAW_DIALOGBOX_NOTICEMSG6);
		if ((msX >= sX + 210) && (msX <= sX + 210 + DEF_BTNSZX) && (msY > sY + 127) && (msY < sY + 127 + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + 210, sY + 127, 1);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + 210, sY + 127, 0);
		break;

	case 2:
		PutAlignedString(sX, sX + szX, sY + 31, DRAW_DIALOGBOX_NOTICEMSG7, 100, 10, 10);
		PutAlignedString(sX, sX + szX, sY + 48, DRAW_DIALOGBOX_NOTICEMSG8);
		PutAlignedString(sX, sX + szX, sY + 65, DRAW_DIALOGBOX_NOTICEMSG9);
		PutAlignedString(sX, sX + szX, sY + 82, DRAW_DIALOGBOX_NOTICEMSG10);
		PutAlignedString(sX, sX + szX, sY + 99, DRAW_DIALOGBOX_NOTICEMSG11);
		if ((msX >= sX + 210) && (msX <= sX + 210 + DEF_BTNSZX) && (msY > sY + 127) && (msY < sY + 127 + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + 210, sY + 127, 1);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + 210, sY + 127, 0);
		break;
	}
}

void CGame::DrawDialogBox_NpcActionQuery(short msX, short msY)
{
	short sX, sY, szX;

	char cTxt[120], cTxt2[120], cStr1[64], cStr2[64], cStr3[64];

	ZeroMemory(cStr1, sizeof(cStr1));
	ZeroMemory(cStr2, sizeof(cStr2));
	ZeroMemory(cStr3, sizeof(cStr3));

	sX = m_stDialogBoxInfo[20].sX;
	sY = m_stDialogBoxInfo[20].sY;
	szX = m_stDialogBoxInfo[20].sSizeX;

	switch (m_stDialogBoxInfo[20].cMode) {
	case 0: // npc
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME6, sX, sY, 5);
		switch (m_stDialogBoxInfo[20].sV3) {
		case 15:
			PutString(sX + 33, sY + 23, NPC_NAME_SHOP_KEEPER, RGB(45, 25, 25));
			PutString(sX + 33 - 1, sY + 23 - 1, NPC_NAME_SHOP_KEEPER, RGB(255, 255, 255));
			break;
		case 19:
			PutString(sX + 33, sY + 23, NPC_NAME_MAGICIAN, RGB(45, 25, 25));
			PutString(sX + 33 - 1, sY + 23 - 1, NPC_NAME_MAGICIAN, RGB(255, 255, 255));
			break;
		case 20:
			PutString(sX + 33, sY + 23, NPC_NAME_WAREHOUSE_KEEPER, RGB(45, 25, 25));
			PutString(sX + 33 - 1, sY + 23 - 1, NPC_NAME_WAREHOUSE_KEEPER, RGB(255, 255, 255));
			break;
		case 24:
			PutString(sX + 33, sY + 23, NPC_NAME_BLACKSMITH_KEEPER, RGB(45, 25, 25));
			PutString(sX + 33 - 1, sY + 23 - 1, NPC_NAME_BLACKSMITH_KEEPER, RGB(255, 255, 255));
			break;
		case 25:
			PutString(sX + 33, sY + 23, NPC_NAME_CITYHALL_OFFICER, RGB(45, 25, 25));
			PutString(sX + 33 - 1, sY + 23 - 1, NPC_NAME_CITYHALL_OFFICER, RGB(255, 255, 255));
			break;
		case 26: // "Guildhall Officer"
			PutString(sX + 33, sY + 23, NPC_NAME_GUILDHALL_OFFICER, RGB(45, 25, 25));
			PutString(sX + 33 - 1, sY + 23 - 1, NPC_NAME_GUILDHALL_OFFICER, RGB(255, 255, 255));
			break;
		case 90: //Gail
			PutString(sX + 33, sY + 23, "Heldenian staff officer", RGB(45, 25, 25));
			PutString(sX + 33 - 1, sY + 23 - 1, "Heldenian staff officer", RGB(255, 255, 255));
			break;
		}

		if (m_stDialogBoxInfo[20].sV3 == 25)
		{   // OFFER
			if ((msX > sX + 25) && (msX < sX + 100) && (msY > sY + 55) && (msY < sY + 70)) {
				PutString(sX + 28, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY13, RGB(255, 255, 255)); // "Offer"
				PutString(sX + 29, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY13, RGB(255, 255, 255));
			}
			else {
				PutString(sX + 28, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY13, RGB(4, 0, 50));
				PutString(sX + 29, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY13, RGB(4, 0, 50));
			}
		}
		else if (m_stDialogBoxInfo[20].sV3 == 20) // WAREHOUSE
		{   // WITHDRAW
			if ((msX > sX + 25) && (msX < sX + 100) && (msY > sY + 55) && (msY < sY + 70)) {
				PutString(sX + 28, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY17, RGB(255, 255, 255)); // "Withdraw"
				PutString(sX + 29, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY17, RGB(255, 255, 255));
			}
			else {
				PutString(sX + 28, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY17, RGB(4, 0, 50));
				PutString(sX + 29, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY17, RGB(4, 0, 50));
			}
			// Centu - Guild Warehouse
			/*if ((msX > sX + 152) && (msX < sX + 200) && (msY > sY + 23) && (msY < sY + 38)) {
				PutString(sX + 155, sY + 23, "Guild", RGB(255, 255, 255));
				PutString(sX + 156, sY + 23, "Guild", RGB(255, 255, 255));
			}
			else {
				PutString(sX + 155, sY + 23, "Guild", RGB(4, 0, 50));
				PutString(sX + 156, sY + 23, "Guild", RGB(4, 0, 50));
			}*/
		}
		else if (m_stDialogBoxInfo[20].sV3 == 19) // CLEROTH: MAGICIAN
		{   // LEARN
			if ((msX > sX + 25) && (msX < sX + 100) && (msY > sY + 55) && (msY < sY + 70)) {
				PutString(sX + 28, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY19, RGB(255, 255, 255)); // "Learn"
				PutString(sX + 29, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY19, RGB(255, 255, 255));
			}
			else {
				PutString(sX + 28, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY19, RGB(4, 0, 50));
				PutString(sX + 29, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY19, RGB(4, 0, 50));
			}
		}
		else
		{   // TRADE
			if ((msX > sX + 25) && (msX < sX + 100) && (msY > sY + 55) && (msY < sY + 70)) {
				PutString(sX + 28, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY21, RGB(255, 255, 255)); // "Trade"
				PutString(sX + 29, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY21, RGB(255, 255, 255));
			}
			else {
				PutString(sX + 28, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY21, RGB(4, 0, 50));
				PutString(sX + 29, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY21, RGB(4, 0, 50));
			}
		}

		if (m_bIsDialogEnabled[21] == false)
		{
			if ((msX > sX + 125) && (msX < sX + 180) && (msY > sY + 55) && (msY < sY + 70)) {
				PutString(sX + 125, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY25, RGB(255, 255, 255)); // "Talk"
				PutString(sX + 126, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY25, RGB(255, 255, 255));
			}
			else {
				PutString(sX + 125, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY25, RGB(4, 0, 50));
				PutString(sX + 126, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY25, RGB(4, 0, 50));
			}
		}
		break;

	case 1: // Other char
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME6, sX, sY, 6);
		GetItemName(m_pItemList[m_stDialogBoxInfo[20].sV1]->m_cName, m_pItemList[m_stDialogBoxInfo[20].sV1]->m_dwAttribute, cStr1, cStr2, cStr3);
		wsprintf(cTxt, DRAW_DIALOGBOX_NPCACTION_QUERY29, m_stDialogBoxInfo[20].sV3, cStr1); //"%d %s to"
		wsprintf(cTxt2, DRAW_DIALOGBOX_NPCACTION_QUERY29_1, m_stDialogBoxInfo[20].cStr); // "%s"

		PutString(sX + 24, sY + 25, cTxt, RGB(45, 25, 25));
		PutString(sX + 24, sY + 40, cTxt2, RGB(45, 25, 25));

		if ((msX > sX + 25) && (msX < sX + 100) && (msY > sY + 55) && (msY < sY + 70)) {
			PutString(sX + 28, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY30, RGB(255, 255, 255));//"Give"
			PutString(sX + 29, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY30, RGB(255, 255, 255));
		}
		else {
			PutString(sX + 28, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY30, RGB(4, 0, 50));
			PutString(sX + 29, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY30, RGB(4, 0, 50));
		}

		if ((msX > sX + 155) && (msX < sX + 210) && (msY > sY + 55) && (msY < sY + 70)) {
			PutString(sX + 155, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY34, RGB(255, 255, 255));//"Exchange"
			PutString(sX + 156, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY34, RGB(255, 255, 255));
		}
		else {
			PutString(sX + 155, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY34, RGB(4, 0, 50));
			PutString(sX + 156, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY34, RGB(4, 0, 50));
		}
		break;

	case 2: // Shop / BS
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME6, sX, sY, 5);
		GetItemName(m_pItemList[m_stDialogBoxInfo[20].sV1]->m_cName, m_pItemList[m_stDialogBoxInfo[20].sV1]->m_dwAttribute, cStr1, cStr2, cStr3);

		wsprintf(cTxt, DRAW_DIALOGBOX_NPCACTION_QUERY29, m_stDialogBoxInfo[20].sV3, cStr1);//"%d %s to"
		wsprintf(cTxt2, DRAW_DIALOGBOX_NPCACTION_QUERY29_1, m_stDialogBoxInfo[20].cStr);//"%s"

		PutString(sX + 24, sY + 20, cTxt, RGB(45, 25, 25));
		PutString(sX + 24, sY + 35, cTxt2, RGB(45, 25, 25));

		if ((msX > sX + 25) && (msX < sX + 100) && (msY > sY + 55) && (msY < sY + 70))
		{
			PutString(sX + 28, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY39, RGB(255, 255, 255));//"Sell"
			PutString(sX + 29, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY39, RGB(255, 255, 255));
		}
		else
		{
			PutString(sX + 28, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY39, RGB(4, 0, 50));
			PutString(sX + 29, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY39, RGB(4, 0, 50));
		}

		if ((m_pItemList[m_stDialogBoxInfo[20].sV1]->m_cItemType == DEF_ITEMTYPE_CONSUME) ||
			(m_pItemList[m_stDialogBoxInfo[20].sV1]->m_cItemType == DEF_ITEMTYPE_ARROW))
		{
		}
		else
		{
			if ((msX > sX + 125) && (msX < sX + 180) && (msY > sY + 55) && (msY < sY + 70))
			{
				PutString(sX + 125, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY43, RGB(255, 255, 255));//"Repair"
				PutString(sX + 126, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY43, RGB(255, 255, 255));
			}
			else
			{
				PutString(sX + 125, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY43, RGB(4, 0, 50));
				PutString(sX + 126, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY43, RGB(4, 0, 50));
			}
		}
		break;

	case 3: // WH
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME6, sX, sY, 6);
		GetItemName(m_pItemList[m_stDialogBoxInfo[20].sV1]->m_cName, m_pItemList[m_stDialogBoxInfo[20].sV1]->m_dwAttribute, cStr1, cStr2, cStr3);

		wsprintf(cTxt, DRAW_DIALOGBOX_NPCACTION_QUERY29, m_stDialogBoxInfo[20].sV3, cStr1);//"%d %s to"
		wsprintf(cTxt2, DRAW_DIALOGBOX_NPCACTION_QUERY29_1, m_stDialogBoxInfo[20].cStr);//"%s"

		PutAlignedString(sX, sX + 240, sY + 20, cTxt, 45, 25, 25);
		PutAlignedString(sX, sX + 240, sY + 35, cTxt2, 45, 25, 25);

		if ((msX > sX + 25) && (msX < sX + 100) && (msY > sY + 55) && (msY < sY + 70)) {
			PutString(sX + 28, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY48, RGB(255, 255, 255));//"Deposit"
			PutString(sX + 29, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY48, RGB(255, 255, 255));
		}
		else
		{
			PutString(sX + 28, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY48, RGB(4, 0, 50));
			PutString(sX + 29, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY48, RGB(4, 0, 50));
		}
		break;

	case 4: // talk to npc or unicorn
		/*DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME2, sX, sY, 5);
		switch (m_stDialogBoxInfo[20].sV3) {
		case 21:
			PutString(sX + 35, sY + 25, NPC_NAME_GUARD, RGB(45, 25, 25));//
			PutString(sX + 35 - 1, sY + 25 - 1, NPC_NAME_GUARD, RGB(255, 255, 255));
			break;
		case 32:
			PutString(sX + 35, sY + 25, NPC_NAME_UNICORN, RGB(45, 25, 25));//"
			PutString(sX + 35 - 1, sY + 25 - 1, NPC_NAME_UNICORN, RGB(255, 255, 255));//"
			break;
		case 67:
			PutString(sX + 35, sY + 25, NPC_NAME_MCGAFFIN, RGB(45, 25, 25));
			PutString(sX + 35 - 1, sY + 25 - 1, NPC_NAME_MCGAFFIN, RGB(255, 255, 255));
			break;
		case 68:
			PutString(sX + 35, sY + 25, NPC_NAME_PERRY, RGB(45, 25, 25));
			PutString(sX + 35 - 1, sY + 25 - 1, NPC_NAME_PERRY, RGB(255, 255, 255));
			break;
		case 69:
			PutString(sX + 35, sY + 25, NPC_NAME_DEVLIN, RGB(45, 25, 25));
			PutString(sX + 35 - 1, sY + 25 - 1, NPC_NAME_DEVLIN, RGB(255, 255, 255));
			break;

		}

		if (m_bIsDialogEnabled[21] == false) {
			if ((msX > sX + 125) && (msX < sX + 180) && (msY > sY + 55) && (msY < sY + 70)) {
				PutString(sX + 125, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY25, RGB(255, 255, 255));
				PutString(sX + 126, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY25, RGB(255, 255, 255));
			}
			else {
				PutString(sX + 125, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY25, RGB(4, 0, 50));
				PutString(sX + 126, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY25, RGB(4, 0, 50));
			}
		}*/
		break;

	case 5: // Shop / BS
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME6, sX, sY, 6);
		switch (m_stDialogBoxInfo[20].sV3) {
		case 15:
			PutString(sX + 33, sY + 23, NPC_NAME_SHOP_KEEPER, RGB(45, 25, 25));//Shop Keeper"
			PutString(sX + 33 - 1, sY + 23 - 1, NPC_NAME_SHOP_KEEPER, RGB(255, 255, 255));
			break;
		case 24:
			PutString(sX + 33, sY + 23, NPC_NAME_BLACKSMITH_KEEPER, RGB(45, 25, 25));//"BlackSmith Keeper"
			PutString(sX + 33 - 1, sY + 23 - 1, NPC_NAME_BLACKSMITH_KEEPER, RGB(255, 255, 255));//"
			//50Cent - Repair All
			if ((msX > sX + 155) && (msX < sX + 210) && (msY > sY + 22) && (msY < sY + 37))
			{
				PutString(sX + 155, sY + 22, DRAW_DIALOGBOX_NPCACTION_QUERY49, RGB(255, 255, 255));//"Repair All"
				PutString(sX + 156, sY + 22, DRAW_DIALOGBOX_NPCACTION_QUERY49, RGB(255, 255, 255));
			}
			else
			{
				PutString(sX + 155, sY + 22, DRAW_DIALOGBOX_NPCACTION_QUERY49, RGB(4, 0, 50));
				PutString(sX + 156, sY + 22, DRAW_DIALOGBOX_NPCACTION_QUERY49, RGB(4, 0, 50));
			}
			break;
		}

		if ((msX > sX + 25) && (msX < sX + 100) && (msY > sY + 55) && (msY < sY + 70))
		{
			PutString(sX + 28, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY21, RGB(255, 255, 255));//"Trade"
			PutString(sX + 29, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY21, RGB(255, 255, 255));
		}
		else
		{
			PutString(sX + 28, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY21, RGB(4, 0, 50));
			PutString(sX + 29, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY21, RGB(4, 0, 50));
		}

		if ((msX > sX + 25 + 79) && (msX < sX + 80 + 75) && (msY > sY + 55) && (msY < sY + 70))
		{
			PutString(sX + 28 + 75, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY39, RGB(255, 255, 255));//"Sell"
			PutString(sX + 29 + 75, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY39, RGB(255, 255, 255));
		}
		else
		{
			PutString(sX + 28 + 75, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY39, RGB(4, 0, 50));
			PutString(sX + 29 + 75, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY39, RGB(4, 0, 50));
		}
		if (m_bIsDialogEnabled[21] == false)
		{
			if ((msX > sX + 155) && (msX < sX + 210) && (msY > sY + 55) && (msY < sY + 70))
			{
				PutString(sX + 155, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY25, RGB(255, 255, 255));//"Talk"
				PutString(sX + 156, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY25, RGB(255, 255, 255));
			}
			else
			{
				PutString(sX + 155, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY25, RGB(4, 0, 50));
				PutString(sX + 156, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY25, RGB(4, 0, 50));
			}
		}
		break;

	case 6: //Gail
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME6, sX, sY, 5);
		if ((msX > sX + 25) && (msX < sX + 100) && (msY > sY + 55) && (msY < sY + 70))
		{
			PutString(sX + 28, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY21, RGB(255, 255, 255)); // "Trade"
			PutString(sX + 29, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY21, RGB(255, 255, 255));
		}
		else
		{
			PutString(sX + 28, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY21, RGB(4, 0, 50));
			PutString(sX + 29, sY + 55, DRAW_DIALOGBOX_NPCACTION_QUERY21, RGB(4, 0, 50));
		}
		PutString(sX + 33, sY + 23, "Heldenian staff officer", RGB(45, 25, 25));
		PutString(sX + 33 - 1, sY + 23 - 1, "Heldenian staff officer", RGB(255, 255, 255));
		break;

	//Magn0S:: Add Learn All Magics
	case 7: // Learn all magics
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME6, sX, sY, 5);
		
		PutString(sX + 28, sY + 25, "Wizard", RGB(45, 25, 25));
		PutString(sX + 27, sY + 24, "Wizard", RGB(255, 255, 255));

		if ((msX > sX + 25) && (msX < sX + 100) && (msY > sY + 55) && (msY < sY + 70))
		{
			PutString(sX + 28, sY + 55, "Buy Magic", RGB(255, 255, 255));//"Talk"
			PutString(sX + 29, sY + 55, "Buy Magic", RGB(255, 255, 255));
		}
		else
		{
			PutString(sX + 28, sY + 55, "Buy Magic", RGB(4, 0, 50));
			PutString(sX + 29, sY + 55, "Buy Magic", RGB(4, 0, 50));
		}

		if ((msX > sX + 125) && (msX < sX + 200) && (msY > sY + 55) && (msY < sY + 70))
		{
			PutString(sX + 128, sY + 55, "Enchant", RGB(255, 255, 255));//"Talk"
			PutString(sX + 129, sY + 55, "Enchant", RGB(255, 255, 255));
		}
		else
		{
			PutString(sX + 128, sY + 55, "Enchant", RGB(4, 0, 50));
			PutString(sX + 129, sY + 55, "Enchant", RGB(4, 0, 50));
		}
		break;
	}
}

void CGame::DrawDialogBox_Party(short msX, short msY)
{
	short sX, sY, szX;
	int  i, iNth;
	sX = m_stDialogBoxInfo[32].sX;
	sY = m_stDialogBoxInfo[32].sY;
	szX = m_stDialogBoxInfo[32].sSizeX;

	if (m_bUseOldPanels) {
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME6, sX, sY, 0);
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_TEXT2, sX, sY, 3);
	}
	else {
		short limitX, limitY;
		limitX = sX + m_stDialogBoxInfo[32].sSizeX;
		limitY = sY + m_stDialogBoxInfo[32].sSizeY;

		m_DDraw.DrawShadowBox(sX, sY, limitX, limitY, 0, true);
		if (!m_bDialogTrans) m_DDraw.DrawShadowBox(sX, sY, limitX, limitY, 0, true);

		m_DDraw.DrawShadowBox(sX, sY, limitX, sY + 25, 0, true);
		if (!m_bDialogTrans) m_DDraw.DrawShadowBox(sX, sY, limitX, sY + 25, 0, true);
		PutString_SprFont2(sX + 100, sY + 5, "Party", 240, 240, 240);
	}

	switch (m_stDialogBoxInfo[32].cMode) {
	case 0:
		if (m_iPartyStatus == 0) {
			if ((msX > sX + 80) && (msX < sX + 195) && (msY > sY + 80) && (msY < sY + 100))
				PutAlignedString(sX, sX + szX, sY + 85, DRAW_DIALOGBOX_PARTY1, 255, 255, 255);//"
			else PutAlignedString(sX, sX + szX, sY + 85, DRAW_DIALOGBOX_PARTY1, 19, 104, 169);//"
		}
		else PutAlignedString(sX, sX + szX, sY + 85, DRAW_DIALOGBOX_PARTY1, 65, 65, 65);//"

		if (m_iPartyStatus != 0) {
			if ((msX > sX + 80) && (msX < sX + 195) && (msY > sY + 100) && (msY < sY + 120))
				PutAlignedString(sX, sX + szX, sY + 105, DRAW_DIALOGBOX_PARTY4, 255, 255, 255);//"
			else PutAlignedString(sX, sX + szX, sY + 105, DRAW_DIALOGBOX_PARTY4, 19, 104, 169);//"
		}
		else PutAlignedString(sX, sX + szX, sY + 105, DRAW_DIALOGBOX_PARTY4, 65, 65, 65);//"

		if (m_iPartyStatus != 0) {
			if ((msX > sX + 80) && (msX < sX + 195) && (msY > sY + 120) && (msY < sY + 140))
				PutAlignedString(sX, sX + szX, sY + 125, DRAW_DIALOGBOX_PARTY7, 255, 255, 255);//"
			else PutAlignedString(sX, sX + szX, sY + 125, DRAW_DIALOGBOX_PARTY7, 19, 104, 169);//"
		}
		else PutAlignedString(sX, sX + szX, sY + 125, DRAW_DIALOGBOX_PARTY7, 65, 65, 65);//"		

		switch (m_iPartyStatus) {
		case 0:
			PutAlignedString(sX, sX + szX, sY + 155, DRAW_DIALOGBOX_PARTY10, 255, 255, 255);//"
			PutAlignedString(sX, sX + szX, sY + 170, DRAW_DIALOGBOX_PARTY11, 255, 255, 255);//"
			PutAlignedString(sX, sX + szX, sY + 185, DRAW_DIALOGBOX_PARTY12, 255, 255, 255);//"
			break;

		case 1:
		case 2:
			PutAlignedString(sX, sX + szX, sY + 155, DRAW_DIALOGBOX_PARTY13, 255, 255, 255);//"
			PutAlignedString(sX, sX + szX, sY + 170, DRAW_DIALOGBOX_PARTY14, 255, 255, 255);//"
			PutAlignedString(sX, sX + szX, sY + 185, DRAW_DIALOGBOX_PARTY15, 255, 255, 255);//"
			break;
		}

		if (m_bUseOldPanels) {
			if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
			else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
		}
		else {
			if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
			{
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
				PutAlignedString2(sX + DEF_RBTNPOSX + 10, sX + DEF_RBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Ok", 255, 255, 100);
			}
			else
			{
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
				PutAlignedString2(sX + DEF_RBTNPOSX + 10, sX + DEF_RBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Ok", 180, 188, 180);
			}
		}
		break;

	case 1: //
		wsprintf(G_cTxt, DRAW_DIALOGBOX_PARTY16, m_stDialogBoxInfo[32].cStr);
		PutAlignedString(sX, sX + szX, sY + 95, G_cTxt,255, 255, 255);
		PutAlignedString(sX, sX + szX, sY + 110, DRAW_DIALOGBOX_PARTY17, 255, 255, 255);//"join the party. You can"
		PutAlignedString(sX, sX + szX, sY + 125, DRAW_DIALOGBOX_PARTY18, 255, 255, 255);//"share the experience by"
		PutAlignedString(sX, sX + szX, sY + 140, DRAW_DIALOGBOX_PARTY19, 255, 255, 255);//"the ratio of level if"
		PutAlignedString(sX, sX + szX, sY + 155, DRAW_DIALOGBOX_PARTY20, 255, 255, 255);//"you join the party."
		PutAlignedString(sX, sX + szX, sY + 175, DRAW_DIALOGBOX_PARTY21, 255, 255, 255);//"Would you like to join party?"

		if (m_bUseOldPanels) {
			/*if ((msX >= sX + DEF_LBTNPOSX) && (msX <= sX + DEF_LBTNPOSX + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 19);
			else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 18);*/

			if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
			else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
		}
		else {
			if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
			{
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
				PutAlignedString2(sX + DEF_RBTNPOSX + 10, sX + DEF_RBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Cancel", 255, 255, 100);
			}
			else
			{
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
				PutAlignedString2(sX + DEF_RBTNPOSX + 10, sX + DEF_RBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Cancel", 180, 188, 180);
			}
		}
		break;

	case 2:
		PutAlignedString(sX, sX + szX, sY + 95, DRAW_DIALOGBOX_PARTY22, 255, 255, 255);//"Click the character which"
		PutAlignedString(sX, sX + szX, sY + 110, DRAW_DIALOGBOX_PARTY23, 255, 255, 255);//"
		PutAlignedString(sX, sX + szX, sY + 125, DRAW_DIALOGBOX_PARTY24, 255, 255, 255);//"
		PutAlignedString(sX, sX + szX, sY + 140, DRAW_DIALOGBOX_PARTY25, 255, 255, 255);//"

		if (m_bUseOldPanels) {
			if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
			else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
		}
		else {
			if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
			{
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
				PutAlignedString2(sX + DEF_RBTNPOSX + 10, sX + DEF_RBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Ok", 255, 255, 100);
			}
			else
			{
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
				PutAlignedString2(sX + DEF_RBTNPOSX + 10, sX + DEF_RBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Ok", 180, 188, 180);
			}
		}
		break;

	case 3:
		wsprintf(G_cTxt, DRAW_DIALOGBOX_PARTY26, m_stDialogBoxInfo[32].cStr);//"
		PutAlignedString(sX, sX + szX, sY + 95, G_cTxt, 255, 255, 255);
		PutAlignedString(sX, sX + szX, sY + 110, DRAW_DIALOGBOX_PARTY27, 255, 255, 255);//"
		PutAlignedString(sX, sX + szX, sY + 125, DRAW_DIALOGBOX_PARTY28, 255, 255, 255);//"
		PutAlignedString(sX, sX + szX, sY + 140, DRAW_DIALOGBOX_PARTY29, 255, 255, 255);//"
		PutAlignedString(sX, sX + szX, sY + 155, DRAW_DIALOGBOX_PARTY30, 255, 255, 255);//"

		if (m_bUseOldPanels) {
			if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
			else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
		}
		else {
			if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
			{
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
				PutAlignedString2(sX + DEF_RBTNPOSX + 10, sX + DEF_RBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Ok", 255, 255, 100);
			}
			else
			{
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
				PutAlignedString2(sX + DEF_RBTNPOSX + 10, sX + DEF_RBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Ok", 180, 188, 180);
			}
		}
		break;

	case 4:
		char CoordX[5], CoordY[5], name[8], map[5], coord[8];
		wsprintf(name, "Name");
		wsprintf(map, "Map");
		wsprintf(coord, "Coords");
		PutString(sX + 30, sY + 80, name, RGB(255, 255, 255));
		PutString(sX + 115, sY + 80, map, RGB(255, 255, 255));
		PutString(sX + 180, sY + 80, coord, RGB(255, 255, 255));

		PutString(sX + 25, sY + 90, "-----------------------------------------", RGB(255, 255, 255));

		iNth = 0;
		for (i = 0; i <= DEF_MAXPARTYMEMBERS; i++)
			if (strlen(m_stPartyMemberNameList[i].cName) != 0) {
				wsprintf(G_cTxt, "%s", m_stPartyMemberNameList[i].cName);
				wsprintf(CoordX, "%d", iParty_sX[i]);
				wsprintf(CoordY, "%d", iParty_sY[i]);
				PutString(sX + 30, sY + 105 + 15 * (iNth), G_cTxt, RGB(255, 255, 255));
				PutString(sX + 115, sY + 105 + 15 * (iNth), PartyMapName[i], RGB(255, 255, 255));
				PutString(sX + 180, sY + 105 + 15 * (iNth), CoordX, RGB(255, 255, 255));
				PutString(sX + 210, sY + 105 + 15 * (iNth), CoordY, RGB(255, 255, 255));
				iNth++;
			}

		//Show Party Bonus! - ZeroEoyPnk
		/*switch (m_iTotalPartyMember)
		{
		case 1:
			PutAlignedString(sX, sX + szX, sY + 250, "Party Exp 100% | Party Ek x 10");
			break;
		case 2:
			PutAlignedString(sX, sX + szX, sY + 250, "Party Exp 60% | Party Ek x 5");
			break;
		case 3:
			PutAlignedString(sX, sX + szX, sY + 250, "Party Exp 70% | Party Ek x 5");
			break;
		case 4:
			PutAlignedString(sX, sX + szX, sY + 250, "Party Exp 80% | Party Ek x 5");
			break;
		case 5:
			PutAlignedString(sX, sX + szX, sY + 250, "Party Exp 85% | Party Ek x 5");
			break;
		case 6:
			PutAlignedString(sX, sX + szX, sY + 250, "Party Exp 90% | Party Ek x 5");
			break;
		case 7:
			PutAlignedString(sX, sX + szX, sY + 250, "Party Exp 95% | Party Ek x 5");
			break;
		case 8:
			PutAlignedString(sX, sX + szX, sY + 250, "Party Exp 100% | Party Ek x 5");
			break;
		}*/

		if (m_bUseOldPanels) {
			if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
			else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
		}
		else {
			if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
			{
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
				PutAlignedString2(sX + DEF_RBTNPOSX + 10, sX + DEF_RBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Ok", 255, 255, 100);
			}
			else
			{
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
				PutAlignedString2(sX + DEF_RBTNPOSX + 10, sX + DEF_RBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Ok", 180, 188, 180);
			}
		}
		break;

	case 5:
		PutAlignedString(sX, sX + szX, sY + 95, DRAW_DIALOGBOX_PARTY33, 255, 255, 255);//"
		PutAlignedString(sX, sX + szX, sY + 110, DRAW_DIALOGBOX_PARTY34, 255, 255, 255);//"
		break;

	case 6:
		PutAlignedString(sX, sX + szX, sY + 95, DRAW_DIALOGBOX_PARTY35, 255, 255, 255);//"
		if (m_bUseOldPanels) {
			if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
			else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
		}
		else {
			if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
			{
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
				PutAlignedString2(sX + DEF_RBTNPOSX + 10, sX + DEF_RBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Ok", 255, 255, 100);
			}
			else
			{
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
				PutAlignedString2(sX + DEF_RBTNPOSX + 10, sX + DEF_RBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Ok", 180, 188, 180);
			}
		}
		break;

	case 7:
		PutAlignedString(sX, sX + szX, sY + 95, DRAW_DIALOGBOX_PARTY36, 255, 255, 255);//"
		PutAlignedString(sX, sX + szX, sY + 110, DRAW_DIALOGBOX_PARTY37, 255, 255, 255);//"
		PutAlignedString(sX, sX + szX, sY + 125, DRAW_DIALOGBOX_PARTY38, 255, 255, 255);//"
		PutAlignedString(sX, sX + szX, sY + 140, DRAW_DIALOGBOX_PARTY39, 255, 255, 255);//"
		if (m_bUseOldPanels) {
			if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
			else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
		}
		else {
			if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
			{
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
				PutAlignedString2(sX + DEF_RBTNPOSX + 10, sX + DEF_RBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Ok", 255, 255, 100);
			}
			else
			{
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
				PutAlignedString2(sX + DEF_RBTNPOSX + 10, sX + DEF_RBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Ok", 180, 188, 180);
			}
		}
		break;

	case 8:
		PutAlignedString(sX, sX + szX, sY + 95, DRAW_DIALOGBOX_PARTY40, 255, 255, 255);//"
		PutAlignedString(sX, sX + szX, sY + 110, DRAW_DIALOGBOX_PARTY41, 255, 255, 255);//"
		PutAlignedString(sX, sX + szX, sY + 125, DRAW_DIALOGBOX_PARTY42, 255, 255, 255);//"
		PutAlignedString(sX, sX + szX, sY + 140, DRAW_DIALOGBOX_PARTY43, 255, 255, 255);//"
		PutAlignedString(sX, sX + szX, sY + 155, DRAW_DIALOGBOX_PARTY44, 255, 255, 255);//"
		PutAlignedString(sX, sX + szX, sY + 170, DRAW_DIALOGBOX_PARTY45, 255, 255, 255);//"

		if (m_bUseOldPanels) {
			if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
			else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
		}
		else {
			if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
			{
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
				PutAlignedString2(sX + DEF_RBTNPOSX + 10, sX + DEF_RBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Ok", 255, 255, 100);
			}
			else
			{
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
				PutAlignedString2(sX + DEF_RBTNPOSX + 10, sX + DEF_RBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Ok", 180, 188, 180);
			}
		}
		break;

	case 9:
		PutAlignedString(sX, sX + szX, sY + 95, DRAW_DIALOGBOX_PARTY46, 255, 255, 255);//"
		PutAlignedString(sX, sX + szX, sY + 110, DRAW_DIALOGBOX_PARTY47, 255, 255, 255);//"
		PutAlignedString(sX, sX + szX, sY + 125, DRAW_DIALOGBOX_PARTY48, 255, 255, 255);//"
		PutAlignedString(sX, sX + szX, sY + 140, DRAW_DIALOGBOX_PARTY49, 255, 255, 255);//"
		PutAlignedString(sX, sX + szX, sY + 155, DRAW_DIALOGBOX_PARTY50, 255, 255, 255);//"
		PutAlignedString(sX, sX + szX, sY + 170, DRAW_DIALOGBOX_PARTY51, 255, 255, 255);//"
		if (m_bUseOldPanels) {
			if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
			else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
		}
		else {
			if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
			{
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
				PutAlignedString2(sX + DEF_RBTNPOSX + 10, sX + DEF_RBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Ok", 255, 255, 100);
			}
			else
			{
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
				PutAlignedString2(sX + DEF_RBTNPOSX + 10, sX + DEF_RBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Ok", 180, 188, 180);
			}
		}
		break;

	case 10:
		PutAlignedString(sX, sX + szX, sY + 95, DRAW_DIALOGBOX_PARTY52, 255, 255, 255);//"
		PutAlignedString(sX, sX + szX, sY + 110, DRAW_DIALOGBOX_PARTY53, 255, 255, 255);//"
		PutAlignedString(sX, sX + szX, sY + 125, DRAW_DIALOGBOX_PARTY54, 255, 255, 255);//"
		if (m_bUseOldPanels) {
			if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
			else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
		}
		else {
			if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
			{
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
				PutAlignedString2(sX + DEF_RBTNPOSX + 10, sX + DEF_RBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Ok", 255, 255, 100);
			}
			else
			{
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
				PutAlignedString2(sX + DEF_RBTNPOSX + 10, sX + DEF_RBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Ok", 180, 188, 180);
			}
		}
		break;

	case 11: //
		PutAlignedString(sX, sX + szX, sY + 95, DRAW_DIALOGBOX_PARTY55, 255, 255, 255);//"
		/*if ((msX >= sX + DEF_LBTNPOSX) && (msX <= sX + DEF_LBTNPOSX + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 19);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 18);*/
		if (m_bUseOldPanels) {
			if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
			else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
		}
		else {
			if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY > sY + DEF_BTNPOSY) && (msY < sY + DEF_BTNPOSY + DEF_BTNSZY))
			{
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 0);
				PutAlignedString2(sX + DEF_RBTNPOSX + 10, sX + DEF_RBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Ok", 255, 255, 100);
			}
			else
			{
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 1);
				PutAlignedString2(sX + DEF_RBTNPOSX + 10, sX + DEF_RBTNPOSX + DEF_BTNSZX + 10, sY + DEF_BTNPOSY + 1, "Ok", 180, 188, 180);
			}
		}
		break;
	}
}

void CGame::DrawDialogBox_QueryDropItemAmount()
{
	short sX, sY;
	char cTxt[120], cStr1[64], cStr2[64], cStr3[64];

	sX = m_stDialogBoxInfo[17].sX;
	sY = m_stDialogBoxInfo[17].sY;

	DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME6, sX, sY, 5);

	switch (m_stDialogBoxInfo[17].cMode) {
	case 1:
		GetItemName(m_pItemList[m_stDialogBoxInfo[17].sView]->m_cName, m_pItemList[m_stDialogBoxInfo[17].sView]->m_dwAttribute, cStr1, cStr2, cStr3);
		if (strlen(m_stDialogBoxInfo[17].cStr) == 0)
			wsprintf(cTxt, DRAW_DIALOGBOX_QUERY_DROP_ITEM_AMOUNT1, cStr1);
		else wsprintf(cTxt, DRAW_DIALOGBOX_QUERY_DROP_ITEM_AMOUNT2, cStr1, m_stDialogBoxInfo[17].cStr);//"%s:

		if (m_stDialogBoxInfo[17].sV3 < 1000)
			PutString(sX + 30, sY + 20, cTxt, RGB(55, 25, 25));

		PutString(sX + 30, sY + 35, DRAW_DIALOGBOX_QUERY_DROP_ITEM_AMOUNT3, RGB(55, 25, 25));//"
		if (iGetTopDialogBoxIndex() != 17)
			PutString(sX + 40, sY + 57, m_cAmountString, RGB(255, 255, 255), false, 2);
		wsprintf(cTxt, "__________ (0 ~ %d)", m_pItemList[m_stDialogBoxInfo[17].sView]->m_dwCount);
		PutString(sX + 38, sY + 62, cTxt, RGB(25, 35, 25));
		break;

	case 20:
		GetItemName(m_pItemList[m_stDialogBoxInfo[17].sView]->m_cName, m_pItemList[m_stDialogBoxInfo[17].sView]->m_dwAttribute, cStr1, cStr2, cStr3);
		if (strlen(m_stDialogBoxInfo[17].cStr) == 0) // v1.4
			wsprintf(cTxt, DRAW_DIALOGBOX_QUERY_DROP_ITEM_AMOUNT1, cStr1);//"%s:
		else wsprintf(cTxt, DRAW_DIALOGBOX_QUERY_DROP_ITEM_AMOUNT2, cStr1, m_stDialogBoxInfo[17].cStr);//"%s:

		if (m_stDialogBoxInfo[17].sV3 < 1000) // v1.4
			PutString(sX + 30, sY + 20, cTxt, RGB(55, 25, 25));

		PutString(sX + 30, sY + 35, DRAW_DIALOGBOX_QUERY_DROP_ITEM_AMOUNT3, RGB(55, 25, 25));//"
		PutString(sX + 40, sY + 57, m_cAmountString, RGB(255, 255, 255), false, 2);
		wsprintf(cTxt, "__________ (0 ~ %d)", m_pItemList[m_stDialogBoxInfo[17].sView]->m_dwCount);
		PutString(sX + 38, sY + 62, cTxt, RGB(25, 35, 25));
		break;
	}
}


void CGame::DrawDialogBox_Quest(int msX, int msY)
{
	short sX, sY, szX;
	char cTxt[120], cTemp[21], cTxt2[100], cTxt3[100], cTxt4[50], cTxt5[250], cStr1[255], cStr2[255], cStr3[255];
	int i, iNext;
	short toX, toY, limitX, limitY;

	sX = m_stDialogBoxInfo[28].sX;
	sY = m_stDialogBoxInfo[28].sY;
	szX = m_stDialogBoxInfo[28].sSizeX;

	toX = sX;
	toY = sY;
	limitX = sX + 258;
	limitY = sY + 339;
	int iEntry = 0;
	int iEntry2 = 0;

	iNext = 0;

	if (m_bUseOldPanels) {
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME6, sX, sY, 2);
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_TEXT2, sX, sY, 4);
	}
	else {
		m_DDraw.DrawShadowBox(toX, toY, limitX, limitY, 0, true);
		if (!m_bDialogTrans) m_DDraw.DrawShadowBox(toX, toY, limitX, limitY, 0, true);

		m_DDraw.DrawShadowBox(toX, toY, limitX, toY + 25, 0, true);
		if (!m_bDialogTrans) m_DDraw.DrawShadowBox(toX, toY, limitX, toY + 25, 0, true);
		PutString_SprFont2(sX + 105, sY + 5, "Quests", 240, 240, 240);
	}
	switch (m_stDialogBoxInfo[28].cMode) {
	case 0:	//Magn0S:: Multi Quest
		for (i = 0; i < DEF_MAXQUEST; i++)
		{
			if (m_stQuest[i].sQuestType != 0)
			{
				if (m_stQuest[i].sX != 0)
				{
					ZeroMemory(cTxt, sizeof(cTxt));
					wsprintf(cTxt, "Observe: %d,%d /%d", m_stQuest[i].sX, m_stQuest[i].sY, m_stQuest[i].sRange);
					PutString2(sX + 20, sY + 30+ iNext, cTxt, 55, 255, 255);
					if (m_stQuest[i].bIsQuestCompleted) PutString2(sX + 20, sY + 45 + iNext, "Quest completed.", 255, 0, 0);
				}
				else
				{
					ZeroMemory(cTxt2, sizeof(cTxt2));
					GetOfficialMapName(m_stQuest[i].cTargetName, cTxt2);
					ZeroMemory(cTemp, sizeof(cTemp));
					GetNpcName(m_stQuest[i].sTargetType, cTemp);
					ZeroMemory(cTxt, sizeof(cTxt));
					wsprintf(cTxt, "Kill %d %s on %s", m_stQuest[i].sTargetCount, cTemp, cTxt2);
					PutString(sX + 20, sY + 30 + iNext, cTxt, RGB(50, 170, 255), false, 1);
					if (m_stQuest[i].iRewardType > 0)
					{
						ZeroMemory(cStr1, sizeof(cStr1));
						GetItemName(m_stQuest[i].cPrizeName, 0, cStr1, cStr2, cStr3);
						ZeroMemory(cTxt5, sizeof(cTxt5));
						wsprintf(cTxt5, "Prize: Contrib: %d  | Item: %s", m_stQuest[i].sContribution, cStr1);
						PutString(sX + 20, sY + 45 + iNext, cTxt5, RGB(255, 255, 255), false, 1);
					}
					else
					{
						ZeroMemory(cTxt5, sizeof(cTxt5));
						wsprintf(cTxt5, "Prize: Contribution: %d", m_stQuest[i].sContribution);
						PutString(sX + 20, sY + 45 + iNext, cTxt5, RGB(255, 255, 255), false, 1);
					}
				}
				if (m_stQuest[i].bIsQuestCompleted)
				{
					if ((msX > sX + 20) && (msX < sX + 90) && (msY > sY + 60 + iNext) && (msY < sY + 60 + iNext + 15))
						PutString2(sX + 20, sY + 60 + iNext, "Claim Prize", 210, 255, 0);
					else PutString2(sX + 20, sY + 60 + iNext, "Claim Prize", 0, 255, 0);
				}
				else
				{
					if ((msX > sX + 20) && (msX < sX + 90) && (msY > sY + 60 + iNext) && (msY < sY + 60 + iNext + 15))
						PutString2(sX + 20, sY + 60 + iNext, "Cancel Quest", 255, 0, 0);
					else PutString2(sX + 20, sY + 60 + iNext, "Cancel Quest", 255, 255, 255);
				}
				iNext += 50;
			}
		}
		/*if ((msX > sX + 135) && (msX < sX + 235) && (msY > sY + 305) && (msY < sY + 320))
		{
			if (!m_bQuestHelper)
				PutString2(sX + 135, sY + 305, "Quest Helper: ON", 0, 255, 0);
			else PutString2(sX + 135, sY + 305, "Quest Helper: OFF", 255, 0, 0);
		}
		else 
		{
			if (!m_bQuestHelper)
				PutString2(sX + 135, sY + 305, "Quest Helper: OFF", 255, 0, 0);
			else PutString2(sX + 135, sY + 305, "Quest Helper: ON", 0, 255, 0);
		}*/
		break;

	case 99: //Magn0S:: Changed to 99 cause we never will use it now.
		switch (m_stQuest[1].sQuestType) {
		case 0:
			PutAlignedString(sX, sX + szX, sY + 50 + 115 - 30, DRAW_DIALOGBOX_QUEST1, 55, 25, 25); // " You are not on a quest."
			break;

		case 1:	//  Hunt
			
			break;

		case 7: //
			//if (m_stQuest[2].bIsQuestCompleted == false)
			//	PutAlignedString(sX, sX + szX, sY + 50, DRAW_DIALOGBOX_QUEST26, 55, 25, 25);
			//else PutAlignedString(sX, sX + szX, sY + 50, DRAW_DIALOGBOX_QUEST27, 55, 25, 25);

			//ZeroMemory(cTemp, sizeof(cTemp));
			//switch (m_stQuest[2].sWho) {
			//case 1:
			//case 2:
			//case 3: break;
			//case 4: strcpy(cTemp, NPC_NAME_CITYHALL_OFFICER); break;
			//case 5:
			//case 6:
			//case 7: break;
			//}
			//ZeroMemory(cTxt, sizeof(cTxt));
			//wsprintf(cTxt, DRAW_DIALOGBOX_QUEST29, cTemp);
			//PutAlignedString(sX, sX + szX, sY + 50 + 45, cTxt, 55, 25, 25);

			//PutAlignedString(sX, sX + szX, sY + 50 + 60, DRAW_DIALOGBOX_QUEST30, 55, 25, 25);

			//ZeroMemory(cTxt, sizeof(cTxt));
			//if (memcmp(m_stQuest[2].cTargetName, "NONE", 4) == 0) {
			//	strcpy(cTxt, DRAW_DIALOGBOX_QUEST31);
			//	PutAlignedString(sX, sX + szX, sY + 50 + 75, cTxt, 55, 25, 25);
			//}
			//else {
			//	ZeroMemory(cTemp, sizeof(cTemp));
			//	GetOfficialMapName(m_stQuest[2].cTargetName, cTemp);
			//	wsprintf(cTxt, DRAW_DIALOGBOX_QUEST32, cTemp);
			//	PutAlignedString(sX, sX + szX, sY + 50 + 75, cTxt, 55, 25, 25);

			//	if (m_stQuest[2].sX != 0) {
			//		ZeroMemory(cTxt, sizeof(cTxt));
			//		wsprintf(cTxt, DRAW_DIALOGBOX_QUEST33, m_stQuest[2].sX, m_stQuest[2].sY, m_stQuest[2].sRange);
			//		PutAlignedString(sX, sX + szX, sY + 50 + 90, cTxt, 55, 25, 25);
			//	}
			//}

			//ZeroMemory(cTxt, sizeof(cTxt));
			//wsprintf(cTxt, DRAW_DIALOGBOX_QUEST34, m_stQuest[2].sContribution);//" %dPoint"
			//PutAlignedString(sX, sX + szX, sY + 50 + 105, cTxt, 55, 25, 25);
			break;
		}
		break;

	case 2: // Cancel Quest
		PutString(sX + 60, sY + 50 + 115 -30, DRAW_DIALOGBOX_QUEST35, RGB(210, 255, 0), false, 1);

		if ((msX > sX + 210) && (msX < sX + 245) && (msY > sY + 305) && (msY < sY + 320))
			PutString_SprFont3(sX + 210, sY + 305, "Back", 200, 250, 2);
		else PutString_SprFont3(sX + 210, sY + 305, "Back", 200, 250, 200);
		break;

	case 3: // No Quests
		PutString(sX + 60, sY + 50 + 115 - 30, DRAW_DIALOGBOX_QUEST1, RGB(210, 255, 0), false, 1);
		break;
	}
}

//50Cent - Quest Helper
//Magn0S:: Adjusted for Multi quest
/*************************************************************************************************
**  void CGame::DrawQuestHelper()                                                                **
*************************************************************************************************/
void CGame::DrawQuestHelper()
{
	char cTemp[21], cTxt[120], cTxt2[50];
	int i, iX, iY, iEntry;

	iEntry = 0;

#ifdef RES_HIGH
	iX = 640;
	iY = 150;
#else
	iX = 500;
	iY = 130;
#endif

	for (i = 0; i < DEF_MAXQUEST; i++) {
		if (m_stQuest[i].sQuestType != 0) {
			PutString2(iX, iY, DEF_QUESTHELPER, 255, 200, 0);
			if (m_stQuest[i].bIsQuestCompleted) {
				//PutString2(iX, iY + 14 + (iEntry *30), DEF_QUESTHELPERCOMPLETE, 255, 0, 0);
				ZeroMemory(cTxt2, sizeof(cTxt2));
				GetOfficialMapName(m_stQuest[i].cTargetName, cTxt2);
				PutString2(iX, iY + 14 + (iEntry * 32), cTxt2, 255, 255, 255);

				ZeroMemory(cTemp, sizeof(cTemp));
				GetNpcName(m_stQuest[i].sTargetType, cTemp);
				ZeroMemory(cTxt, sizeof(cTxt));
				wsprintf(cTxt, "%s: %d/%d", cTemp, m_stQuest[i].sTargetCount, m_stQuest[i].sTargetCount);
				PutString2(iX, iY + 28 + (iEntry * 32), cTxt, 70, 70, 70);
			}
			else {
				/*if (m_stQuest[i].sX != 0) {
					ZeroMemory(cTxt, sizeof(cTxt));
					wsprintf(cTxt, "%d,%d /%d", m_stQuest[1].sX, m_stQuest[i].sY, m_stQuest[i].sRange);
					PutString2(iX, iY + 14 + (iEntry *15), cTxt, 55, 255, 255);
				}
				else {*/
					ZeroMemory(cTxt2, sizeof(cTxt2));
					GetOfficialMapName(m_stQuest[i].cTargetName, cTxt2);
					PutString2(iX, iY + 14 + (iEntry * 32), cTxt2, 255, 255, 255);

					ZeroMemory(cTemp, sizeof(cTemp));
					GetNpcName(m_stQuest[i].sTargetType, cTemp);
					ZeroMemory(cTxt, sizeof(cTxt));
					//wsprintf(cTxt, "%s: %d/%d", cTemp, (m_stQuest[i].sTargetCount - m_stQuest[i].sCurrentCount), m_stQuest[i].sTargetCount);
					wsprintf(cTxt, "%s: %d/%d", cTemp, m_stQuest[i].sCurrentCount, m_stQuest[i].sTargetCount);
					PutString2(iX, iY + 28 + (iEntry *32), cTxt, 55, 255, 255);
				//}
			}
			iEntry++;
		}
	}
}

void CGame::DrawDialogBox_SellList(short msX, short msY)
{
	short sX, sY, szX;
	int  i, iItem;
	char cTemp[255], cStr1[64], cStr2[64], cStr3[64];

	sX = m_stDialogBoxInfo[31].sX;
	sY = m_stDialogBoxInfo[31].sY;
	szX = m_stDialogBoxInfo[31].sSizeX;

	DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME6, sX, sY, 2);
	DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_TEXT2, sX, sY, 11);

	iItem = 0;
	for (i = 0; i < DEF_MAXSELLLIST; i++)
		if (m_stSellItemList[i].iIndex != -1)
		{
			ZeroMemory(cTemp, sizeof(cTemp));
			GetItemName(m_pItemList[m_stSellItemList[i].iIndex]->m_cName, m_pItemList[m_stSellItemList[i].iIndex]->m_dwAttribute, cStr1, cStr2, cStr3);
			if (m_stSellItemList[i].iAmount > 1)
			{
				wsprintf(cTemp, DRAW_DIALOGBOX_SELL_LIST1, m_stSellItemList[i].iAmount, cStr1);
				if ((msX > sX + 25) && (msX < sX + 250) && (msY >= sY + 55 + i * 15) && (msY <= sY + 55 + 14 + i * 15))
					PutAlignedString(sX, sX + szX, sY + 55 + i * 15, cTemp, 255, 255, 255);
				else
				{
					if (m_bIsSpecial)
					{
						// VAMP - gold items
						if (m_bIsRare)
						{
							PutAlignedString(sX, sX + szX, sY + 55 + i * 15, cTemp, 255, 208, 60);
						}
						else if (m_bIsFragile)
						{
							PutAlignedString(sX, sX + szX, sY + 55 + i * 15, cTemp, 50, 255, 255);
						}
						else
						{
							PutAlignedString(sX, sX + szX, sY + 55 + i * 15, cTemp, 0, 255, 50);
						}
					}
					else
					{
						PutAlignedString(sX, sX + szX, sY + 55 + i * 15, cTemp, 255, 255, 255);
					}
				}
			}
			else
			{
				if ((msX > sX + 25) && (msX < sX + 250) && (msY >= sY + 55 + i * 15) && (msY <= sY + 55 + 14 + i * 15))
				{
					if ((strlen(cStr2) == 0) && (strlen(cStr3) == 0)) PutAlignedString(sX, sX + szX, sY + 55 + i * 15, cStr1, 255, 255, 255);
					else
					{
						ZeroMemory(G_cTxt, sizeof(G_cTxt));
						if ((strlen(cStr1) + strlen(cStr2) + strlen(cStr3)) < 36)
						{
							if ((strlen(cStr2)>0) && (strlen(cStr3)>0)) wsprintf(G_cTxt, "%s(%s, %s)", cStr1, cStr2, cStr3);
							else wsprintf(G_cTxt, "%s(%s%s)", cStr1, cStr2, cStr3);
							PutAlignedString(sX, sX + szX, sY + 55 + i * 15, G_cTxt, 255, 255, 255);

						}
						else
						{
							if ((strlen(cStr2)>0) && (strlen(cStr3)>0)) wsprintf(G_cTxt, "(%s, %s)", cStr2, cStr3);
							else wsprintf(G_cTxt, "(%s%s)", cStr2, cStr3);
							PutAlignedString(sX, sX + szX, sY + 55 + i * 15, cStr1, 255, 255, 255);
							PutAlignedString(sX, sX + szX, sY + 55 + i * 15 + 15, G_cTxt, 200, 200, 200);
							i++;
						}
					}
				}
				else
				{
					if ((strlen(cStr2) == 0) && (strlen(cStr3) == 0))
					{
						if (m_bIsSpecial)
						{
							// VAMP - gold items
							if (m_bIsRare)
							{
								PutAlignedString(sX, sX + szX, sY + 55 + i * 15, cStr1, 255, 208, 60);
							}
							else if (m_bIsFragile)
							{
								PutAlignedString(sX, sX + szX, sY + 55 + i * 15, cStr1, 50, 255, 255);
							}
							else
							{
								PutAlignedString(sX, sX + szX, sY + 55 + i * 15, cStr1, 0, 255, 50);
							}
						}
						else
						{
							PutAlignedString(sX, sX + szX, sY + 55 + i * 15, cStr1, 255, 255, 255);
						}
					}
					else
					{
						ZeroMemory(G_cTxt, sizeof(G_cTxt));
						if ((strlen(cStr1) + strlen(cStr2) + strlen(cStr3)) < 36)
						{
							if ((strlen(cStr2)>0) && (strlen(cStr3)>0)) wsprintf(G_cTxt, "%s(%s, %s)", cStr1, cStr2, cStr3);
							else wsprintf(G_cTxt, "%s(%s%s)", cStr1, cStr2, cStr3);

							if (m_bIsSpecial)
							{
								// VAMP - gold items
								if (m_bIsRare)
								{
									PutAlignedString(sX, sX + szX, sY + 55 + i * 15, G_cTxt, 255, 208, 60);
								}
								else if (m_bIsFragile)
								{
									PutAlignedString(sX, sX + szX, sY + 55 + i * 15, G_cTxt, 50, 255, 255);
								}
								else
								{
									PutAlignedString(sX, sX + szX, sY + 55 + i * 15, G_cTxt, 0, 255, 50);
								}
							}
							else
							{
								PutAlignedString(sX, sX + szX, sY + 55 + i * 15, G_cTxt, 255, 255, 255);
							}

						}
						else
						{
							if (m_bIsSpecial)
							{
								// VAMP - gold items
								if (m_bIsRare)
								{
									PutAlignedString(sX, sX + szX, sY + 55 + i * 15, cStr1, 255, 208, 60);
								}
								else if (m_bIsFragile)
								{
									PutAlignedString(sX, sX + szX, sY + 55 + i * 15, cStr1, 50, 255, 255);
								}
								else
								{
									PutAlignedString(sX, sX + szX, sY + 55 + i * 15, cStr1, 0, 255, 50);
								}
							}
							else
							{
								PutAlignedString(sX, sX + szX, sY + 55 + i * 15, cStr1, 255, 255, 255);
							}
						}
					}
				}
			}
		}
		else iItem++;

		if (iItem == DEF_MAXSELLLIST) {
			PutAlignedString(sX, sX + szX, sY + 55 + 30 + 282 - 117 - 170, DRAW_DIALOGBOX_SELL_LIST2);//"
			PutAlignedString(sX, sX + szX, sY + 55 + 45 + 282 - 117 - 170, DRAW_DIALOGBOX_SELL_LIST3);//"
			PutAlignedString(sX, sX + szX, sY + 55 + 60 + 282 - 117 - 170, DRAW_DIALOGBOX_SELL_LIST4);//"
			PutAlignedString(sX, sX + szX, sY + 55 + 75 + 282 - 117 - 170, DRAW_DIALOGBOX_SELL_LIST5);//"
			PutAlignedString(sX, sX + szX, sY + 55 + 95 + 282 - 117 - 170, DRAW_DIALOGBOX_SELL_LIST6);//"
			PutAlignedString(sX, sX + szX, sY + 55 + 110 + 282 - 117 - 170, DRAW_DIALOGBOX_SELL_LIST7);//"
			PutAlignedString(sX, sX + szX, sY + 55 + 125 + 282 - 117 - 170, DRAW_DIALOGBOX_SELL_LIST8);//"
			PutAlignedString(sX, sX + szX, sY + 55 + 155 + 282 - 117 - 170, DRAW_DIALOGBOX_SELL_LIST9);//"

		}

		if ((msX >= sX + DEF_LBTNPOSX) && (msX <= sX + DEF_LBTNPOSX + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY) && (iItem < DEF_MAXSELLLIST))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 39);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 38);

		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 17);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 16);
}

void CGame::DrawDialogBox_SellorRepairItem(short msX, short msY)
{
	short sX, sY;
	UINT32 dwTime = m_dwCurTime;
	char cItemID, cItemColor, cTxt[120], cTemp[120], cStr2[120], cStr3[120], cStr4[120], cStr5[120], cStr6[120];

	sX = m_stDialogBoxInfo[23].sX;
	sY = m_stDialogBoxInfo[23].sY;

	switch (m_stDialogBoxInfo[23].cMode) {
	case 1:
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME6, sX, sY, 2);
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_TEXT2, sX, sY, 11);

		cItemID = m_stDialogBoxInfo[23].sV1;

		cItemColor = m_pItemList[cItemID]->m_cItemColor;
		if (cItemColor == 0)
			m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_pItemList[cItemID]->m_sSprite]->PutSpriteFast(sX + 62 + 15, sY + 84 + 30,
			m_pItemList[cItemID]->m_sSpriteFrame, dwTime);
		else
		{
			switch (m_pItemList[cItemID]->m_sSprite) {
			case 1: // Swds
			case 2: // Bows
			case 3: // Shields
			case 15: // Axes hammers
				m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_pItemList[cItemID]->m_sSprite]->PutSpriteRGB(sX + 62 + 15, sY + 84 + 30
					, m_pItemList[cItemID]->m_sSpriteFrame, m_wWR[cItemColor] - m_wR[0], m_wWG[cItemColor] - m_wG[0], m_wWB[cItemColor] - m_wB[0], dwTime);
				break;
			default: m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_pItemList[cItemID]->m_sSprite]->PutSpriteRGB(sX + 62 + 15, sY + 84 + 30
				, m_pItemList[cItemID]->m_sSpriteFrame, m_wR[cItemColor] - m_wR[0], m_wG[cItemColor] - m_wG[0], m_wB[cItemColor] - m_wB[0], dwTime);
				break;
			}
		}
		ZeroMemory(cTemp, sizeof(cTemp));
		ZeroMemory(cStr2, sizeof(cStr2));
		ZeroMemory(cStr3, sizeof(cStr3));
		
		GetItemName(m_pItemList[cItemID]->m_cName, m_pItemList[cItemID]->m_dwAttribute, cTemp, cStr2, cStr3);
		if (m_stDialogBoxInfo[23].sV4 == 1) strcpy(cTxt, cTemp);
		else wsprintf(cTxt, DRAW_DIALOGBOX_SELLOR_REPAIR_ITEM1, m_stDialogBoxInfo[23].sV4, cTemp);

		if (m_bIsSpecial)
		{
			PutAlignedString(sX + 25, sX + 240, sY + 60, cTxt, 0, 255, 50);
			PutAlignedString(sX + 25 + 1, sX + 240 + 1, sY + 60, cTxt, 0, 255, 50);
		}
		else
		{
			PutAlignedString(sX + 25, sX + 240, sY + 60, cTxt, 45, 25, 25);
			PutAlignedString(sX + 25 + 1, sX + 240 + 1, sY + 60, cTxt, 45, 25, 25);
		}

		wsprintf(cTxt, DRAW_DIALOGBOX_SELLOR_REPAIR_ITEM2, m_stDialogBoxInfo[23].sV2);
		PutString(sX + 95 + 15, sY + 53 + 60, cTxt, RGB(45, 25, 25));
		wsprintf(cTxt, DRAW_DIALOGBOX_SELLOR_REPAIR_ITEM3, m_stDialogBoxInfo[23].sV3);
		PutString(sX + 95 + 15, sY + 53 + 75, cTxt, RGB(45, 25, 25));
		PutString(sX + 55, sY + 190, DRAW_DIALOGBOX_SELLOR_REPAIR_ITEM4, RGB(45, 25, 25));

		if ((msX >= sX + DEF_LBTNPOSX) && (msX <= sX + DEF_LBTNPOSX + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 39);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 38);

		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 17);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 16);
		break;

	case 2:
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME6, sX, sY, 2);
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_TEXT2, sX, sY, 10);
		cItemID = m_stDialogBoxInfo[23].sV1;
		cItemColor = m_pItemList[cItemID]->m_cItemColor; // v1.4
		if (cItemColor == 0)
			m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_pItemList[cItemID]->m_sSprite]->PutSpriteFast(sX + 62 + 15, sY + 84 + 30,
			m_pItemList[cItemID]->m_sSpriteFrame, dwTime);
		else
		{
			switch (m_pItemList[cItemID]->m_sSprite) {
			case 1: // Swds
			case 2: // Bows
			case 3: // Shields
			case 15: // Axes hammers
				m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_pItemList[cItemID]->m_sSprite]->PutSpriteRGB(sX + 62 + 15, sY + 84 + 30
					, m_pItemList[cItemID]->m_sSpriteFrame, m_wWR[cItemColor] - m_wR[0], m_wWG[cItemColor] - m_wG[0], m_wWB[cItemColor] - m_wB[0], dwTime);
				break;

			default: m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_pItemList[cItemID]->m_sSprite]->PutSpriteRGB(sX + 62 + 15, sY + 84 + 30
				, m_pItemList[cItemID]->m_sSpriteFrame, m_wR[cItemColor] - m_wR[0], m_wG[cItemColor] - m_wG[0], m_wB[cItemColor] - m_wB[0], dwTime);
				break;
			}
		}
		ZeroMemory(cTemp, sizeof(cTemp));
		ZeroMemory(cStr2, sizeof(cStr2));
		ZeroMemory(cStr3, sizeof(cStr3));
		ZeroMemory(cStr4, sizeof(cStr4));
		ZeroMemory(cStr5, sizeof(cStr5));
		ZeroMemory(cStr6, sizeof(cStr6));
		GetItemName(m_pItemList[cItemID], cTemp, cStr2, cStr3, cStr4, cStr5, cStr6);
		wsprintf(cTxt, "%s", cTemp);
		if (m_bIsSpecial)
		{
			PutAlignedString(sX + 25, sX + 240, sY + 60, cTxt, 0, 255, 50);
			PutAlignedString(sX + 25 + 1, sX + 240 + 1, sY + 60, cTxt, 0, 255, 50);
		}
		else
		{
			PutAlignedString(sX + 25, sX + 240, sY + 60, cTxt, 45, 25, 25);
			PutAlignedString(sX + 25 + 1, sX + 240 + 1, sY + 60, cTxt, 45, 25, 25);
		}
		wsprintf(cTxt, DRAW_DIALOGBOX_SELLOR_REPAIR_ITEM2, m_stDialogBoxInfo[23].sV2);
		PutString(sX + 95 + 15, sY + 53 + 60, cTxt, RGB(45, 25, 25));
		wsprintf(cTxt, DRAW_DIALOGBOX_SELLOR_REPAIR_ITEM6, m_stDialogBoxInfo[23].sV3);
		PutString(sX + 95 + 15, sY + 53 + 75, cTxt, RGB(45, 25, 25));
		PutString(sX + 55, sY + 190, DRAW_DIALOGBOX_SELLOR_REPAIR_ITEM7, RGB(45, 25, 25));

		if ((msX >= sX + DEF_LBTNPOSX) && (msX <= sX + DEF_LBTNPOSX + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 43);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 42);

		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 17);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 16);
		break;

	case 3:
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME6, sX, sY, 2);
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_TEXT2, sX, sY, 11);

		PutString(sX + 55, sY + 100, DRAW_DIALOGBOX_SELLOR_REPAIR_ITEM8, RGB(45, 25, 25));//"
		PutString(sX + 55, sY + 120, DRAW_DIALOGBOX_SELLOR_REPAIR_ITEM9, RGB(45, 25, 25));//"
		PutString(sX + 55, sY + 135, DRAW_DIALOGBOX_SELLOR_REPAIR_ITEM10, RGB(45, 25, 25));//"
		break;

	case 4:
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME6, sX, sY, 2);
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_TEXT2, sX, sY, 10);

		PutString(sX + 55, sY + 100, DRAW_DIALOGBOX_SELLOR_REPAIR_ITEM11, RGB(45, 25, 25));//"
		PutString(sX + 55, sY + 120, DRAW_DIALOGBOX_SELLOR_REPAIR_ITEM9, RGB(45, 25, 25));//"
		PutString(sX + 55, sY + 135, DRAW_DIALOGBOX_SELLOR_REPAIR_ITEM10, RGB(45, 25, 25));//"
		break;
	}
}

void CGame::DrawDialogBox_Shop(short msX, short msY, short msZ, char cLB)
{
	short sX, sY;
	UINT32 dwTime = m_dwCurTime;
	int  i, iTemp;
	char cTemp[255], cStr2[255], cStr3[255], cStr4[255], cStr5[255], cStr6[255];

	int  iCost, iDiscountCost, iDiscountRatio=0;

	float dTmp1, dTmp2, dTmp3;
	int  iTotalLines, iPointerLoc;
	bool bFlagStatLow = false;
	bool bFlagRedShown = false;
	float d1, d2, d3;

	sX = m_stDialogBoxInfo[11].sX;
	sY = m_stDialogBoxInfo[11].sY;

	DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME6, sX, sY, 2);
	DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_TEXT2, sX, sY, 11);

	switch (m_stDialogBoxInfo[11].cMode) {
	case 0:
		iTotalLines = 0;
		for (i = 0; i < DEF_MAXMENUITEMS; i++)
			if (m_pItemForSaleList[i] != 0) iTotalLines++;
		if (iTotalLines > 13) {
			d1 = (float)m_stDialogBoxInfo[11].sView;
			d2 = (float)(iTotalLines - 13);
			d3 = (274.0f * d1) / d2;
			iPointerLoc = (int)(d3);
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME6, sX, sY, 3);
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME6, sX + 242, sY + iPointerLoc + 35, 7);
		}
		else iPointerLoc = 0;

		if (cLB != 0 && iTotalLines > 13)
		{
			if ((iGetTopDialogBoxIndex() == 11))
			{
				if ((msX >= sX + 235) && (msX <= sX + 260) && (msY >= sY + 10) && (msY <= sY + 330))
				{
					d1 = (float)(msY - (sY + 35));
					d2 = (float)(iTotalLines - 13);
					d3 = (d1 * d2) / 274.0f;
					m_stDialogBoxInfo[11].sView = (int)(d3 + 0.5f);
				}
			}
		}
		else m_stDialogBoxInfo[11].bIsScrollSelected = false;
		if (iGetTopDialogBoxIndex() == 11 && msZ != 0)
		{
			m_stDialogBoxInfo[11].sView = m_stDialogBoxInfo[11].sView - msZ / 60;
			m_DInput.m_sZ = 0;
		}
		if (iTotalLines > 13 && m_stDialogBoxInfo[11].sView > iTotalLines - 13) m_stDialogBoxInfo[11].sView = iTotalLines - 13;
		if (m_stDialogBoxInfo[11].sView < 0 || iTotalLines < 13) m_stDialogBoxInfo[11].sView = 0;
		PutAlignedString(sX + 22, sX + 165, sY + 45, DRAW_DIALOGBOX_SHOP1); // "ITEM"
		PutAlignedString(sX + 23, sX + 166, sY + 45, DRAW_DIALOGBOX_SHOP1);
		PutAlignedString(sX + 153, sX + 250, sY + 45, DRAW_DIALOGBOX_SHOP3);
		PutAlignedString(sX + 154, sX + 251, sY + 45, DRAW_DIALOGBOX_SHOP3);

		for (i = 0; i < 13; i++)
			if (((i + m_stDialogBoxInfo[11].sView) < DEF_MAXMENUITEMS) && (m_pItemForSaleList[i + m_stDialogBoxInfo[11].sView] != 0))
			{
				ZeroMemory(cTemp, sizeof(cTemp));
				GetItemName(m_pItemForSaleList[i + m_stDialogBoxInfo[11].sView], cTemp, cStr2, cStr3, cStr4, cStr5, cStr6);
				if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + i * 18 + 65) && (msY <= sY + i * 18 + 79))
				{
					PutAlignedString(sX + 10, sX + 190, sY + i * 18 + 65, cTemp, 255, 255, 255);
				}
				else PutAlignedString(sX + 10, sX + 190, sY + i * 18 + 65, cTemp, 4, 0, 50);
			}

		for (i = 0; i < 13; i++)
			if (((i + m_stDialogBoxInfo[11].sView) < DEF_MAXMENUITEMS) && (m_pItemForSaleList[i + m_stDialogBoxInfo[11].sView] != 0))
			{
				//iDiscountRatio = ((m_iCharisma - 10) / 4);
				dTmp1 = (float)iDiscountRatio;
				dTmp2 = dTmp1 / 100.0f;
				dTmp1 = (float)m_pItemForSaleList[i + m_stDialogBoxInfo[11].sView]->m_wPrice;
				dTmp3 = dTmp1 * dTmp2;
				iDiscountCost = (int)dTmp3;
				iCost = (int)(m_pItemForSaleList[i + m_stDialogBoxInfo[11].sView]->m_wPrice * ((100 + m_cDiscount) / 100.0f));
				iCost = iCost - iDiscountCost;

				if (iCost < (m_pItemForSaleList[i + m_stDialogBoxInfo[11].sView]->m_wPrice / 2))
					iCost = (m_pItemForSaleList[i + m_stDialogBoxInfo[11].sView]->m_wPrice / 2) - 1;
				ZeroMemory(cTemp, sizeof(cTemp));
				wsprintf(cTemp, "%6d", iCost);
				if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + i * 18 + 65) && (msY <= sY + i * 18 + 79))
					PutAlignedString(sX + 148, sX + 260, sY + i * 18 + 65, cTemp, 255, 255, 255);
				else	PutAlignedString(sX + 148, sX + 260, sY + i * 18 + 65, cTemp, 4, 0, 50);
			}
		break;

	default:
		m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_pItemForSaleList[m_stDialogBoxInfo[11].cMode - 1]->m_sSprite]->PutSpriteFast(sX + 62 + 30 - 35, sY + 84 + 30 - 10,
			m_pItemForSaleList[m_stDialogBoxInfo[11].cMode - 1]->m_sSpriteFrame, dwTime);

		ZeroMemory(cTemp, sizeof(cTemp));
		GetItemName(m_pItemForSaleList[m_stDialogBoxInfo[11].cMode - 1], cTemp, cStr2, cStr3, cStr4, cStr5, cStr6);

		PutAlignedString(sX + 25, sX + 240, sY + 50, cTemp, 255, 255, 255);
		PutAlignedString(sX + 26, sX + 241, sY + 50, cTemp, 255, 255, 255);

		strcpy(cTemp, DRAW_DIALOGBOX_SHOP3); //"PRICE"
		PutString(sX + 90, sY + 78 + 30 - 10, cTemp, RGB(40, 10, 10));
		PutString(sX + 91, sY + 78 + 30 - 10, cTemp, RGB(40, 10, 10));
		strcpy(cTemp, DRAW_DIALOGBOX_SHOP6); // "Weight"
		PutString(sX + 90, sY + 93 + 30 - 10, cTemp, RGB(40, 10, 10));
		PutString(sX + 91, sY + 93 + 30 - 10, cTemp, RGB(40, 10, 10));

		//iDiscountRatio = ((m_iCharisma - 10) / 4);
		dTmp1 = (float)iDiscountRatio;
		dTmp2 = dTmp1 / 100.0f;
		dTmp1 = (float)m_pItemForSaleList[m_stDialogBoxInfo[11].cMode - 1]->m_wPrice;
		dTmp3 = dTmp1 * dTmp2;
		iDiscountCost = (int)dTmp3;
		iCost = (int)(m_pItemForSaleList[m_stDialogBoxInfo[11].cMode - 1]->m_wPrice * ((100 + m_cDiscount) / 100.0f));
		iCost = iCost - iDiscountCost;

		if (iCost < (m_pItemForSaleList[m_stDialogBoxInfo[11].cMode - 1]->m_wPrice / 2))
			iCost = (m_pItemForSaleList[m_stDialogBoxInfo[11].cMode - 1]->m_wPrice / 2) - 1;

		wsprintf(cTemp, DRAW_DIALOGBOX_SHOP7, iCost);
		//": %d Gold"
		PutString(sX + 140, sY + 98, cTemp, RGB(45, 25, 25));

		iTemp = m_pItemForSaleList[m_stDialogBoxInfo[11].cMode - 1]->m_wWeight / 100;
		//wsprintf(cTemp, DRAW_DIALOGBOX_SHOP8, iTemp);
		//": %d Stone"
		//PutString(sX + 140, sY + 113, cTemp, RGB(45, 25, 25));

		switch (m_pItemForSaleList[m_stDialogBoxInfo[11].cMode - 1]->m_cEquipPos) {
		case DEF_EQUIPPOS_RHAND:
		case DEF_EQUIPPOS_TWOHAND:
			strcpy(cTemp, DRAW_DIALOGBOX_SHOP9);  // Damage
			PutString(sX + 90, sY + 145, cTemp, RGB(40, 10, 10));
			PutString(sX + 91, sY + 145, cTemp, RGB(40, 10, 10));
			strcpy(cTemp, DRAW_DIALOGBOX_SHOP10); //"Speed(Min.~Max.)"
			PutString(sX + 40, sY + 175, cTemp, RGB(40, 10, 10));
			PutString(sX + 41, sY + 175, cTemp, RGB(40, 10, 10));

			if (m_pItemForSaleList[m_stDialogBoxInfo[11].cMode - 1]->m_sItemEffectValue3 != 0)
			{
				wsprintf(cTemp, ": %dD%d+%d (S-M)", m_pItemForSaleList[m_stDialogBoxInfo[11].cMode - 1]->m_sItemEffectValue1,
					m_pItemForSaleList[m_stDialogBoxInfo[11].cMode - 1]->m_sItemEffectValue2,
					m_pItemForSaleList[m_stDialogBoxInfo[11].cMode - 1]->m_sItemEffectValue3);
			}
			else
			{
				wsprintf(cTemp, ": %dD%d (S-M)", m_pItemForSaleList[m_stDialogBoxInfo[11].cMode - 1]->m_sItemEffectValue1,
					m_pItemForSaleList[m_stDialogBoxInfo[11].cMode - 1]->m_sItemEffectValue2);
			}
			PutString(sX + 140, sY + 145, cTemp, RGB(45, 25, 25));
			if (m_pItemForSaleList[m_stDialogBoxInfo[11].cMode - 1]->m_sItemEffectValue6 != 0)
			{
				wsprintf(cTemp, ": %dD%d+%d (L)", m_pItemForSaleList[m_stDialogBoxInfo[11].cMode - 1]->m_sItemEffectValue4,
					m_pItemForSaleList[m_stDialogBoxInfo[11].cMode - 1]->m_sItemEffectValue5,
					m_pItemForSaleList[m_stDialogBoxInfo[11].cMode - 1]->m_sItemEffectValue6);
			}
			else
			{
				wsprintf(cTemp, ": %dD%d (L)", m_pItemForSaleList[m_stDialogBoxInfo[11].cMode - 1]->m_sItemEffectValue4,
					m_pItemForSaleList[m_stDialogBoxInfo[11].cMode - 1]->m_sItemEffectValue5);
			}
			PutString(sX + 140, sY + 160, cTemp, RGB(45, 25, 25));
			if (m_pItemForSaleList[m_stDialogBoxInfo[11].cMode - 1]->m_cSpeed == 0)   wsprintf(cTemp, ": 0(10~10)");
			else wsprintf(cTemp, ": %d(%d ~ %d)", m_pItemForSaleList[m_stDialogBoxInfo[11].cMode - 1]->m_cSpeed, iTemp, m_pItemForSaleList[m_stDialogBoxInfo[11].cMode - 1]->m_cSpeed * 13);
			PutString(sX + 140, sY + 175, cTemp, RGB(45, 25, 25));

			//if ((m_pItemForSaleList[m_stDialogBoxInfo[11].cMode - 1]->m_wWeight / 100) > m_iStr)
			//{
			//	wsprintf(cTemp, DRAW_DIALOGBOX_SHOP11, (m_pItemForSaleList[m_stDialogBoxInfo[11].cMode - 1]->m_wWeight / 100));
			//	PutAlignedString(sX + 25, sX + 240, sY + 258, cTemp, 195, 25, 25);
			//	PutAlignedString(sX + 26, sX + 241, sY + 258, cTemp, 195, 25, 25); // *Your STR should be at least %d to use this item."
			//	bFlagRedShown = true;
			//}
			
			break;

		case DEF_EQUIPPOS_LHAND:
			strcpy(cTemp, DRAW_DIALOGBOX_SHOP12); // "Defence"
			PutString(sX + 90, sY + 145, cTemp, RGB(40, 10, 10));
			PutString(sX + 91, sY + 145, cTemp, RGB(40, 10, 10));
			wsprintf(cTemp, ": +%d%", m_pItemForSaleList[m_stDialogBoxInfo[11].cMode - 1]->m_sItemEffectValue1);
			PutString(sX + 140, sY + 145, cTemp, RGB(45, 25, 25));
			//if ((m_pItemForSaleList[m_stDialogBoxInfo[11].cMode - 1]->m_wWeight / 100) > m_iStr)
			//{
			//	wsprintf(cTemp, DRAW_DIALOGBOX_SHOP11, (m_pItemForSaleList[m_stDialogBoxInfo[11].cMode - 1]->m_wWeight / 100));
			//	PutAlignedString(sX + 25, sX + 240, sY + 258, cTemp, 195, 25, 25); // "*Your STR should be at least %d to use this item."
			//	PutAlignedString(sX + 26, sX + 241, sY + 258, cTemp, 195, 25, 25);
			//	bFlagRedShown = true;
			//}
			
			break;

		case DEF_EQUIPPOS_HEAD:
		case DEF_EQUIPPOS_BODY:
		case DEF_EQUIPPOS_BOOTS:
		case DEF_EQUIPPOS_ARMS:
		case DEF_EQUIPPOS_PANTS:
			strcpy(cTemp, DRAW_DIALOGBOX_SHOP12); // "Defence"
			PutString(sX + 90, sY + 145, cTemp, RGB(40, 10, 10));
			PutString(sX + 91, sY + 145, cTemp, RGB(40, 10, 10));
			wsprintf(cTemp, ": +%d%", m_pItemForSaleList[m_stDialogBoxInfo[11].cMode - 1]->m_sItemEffectValue1);
			PutString(sX + 140, sY + 145, cTemp, RGB(45, 25, 25));
			bFlagStatLow = false;
			
			//if ((m_pItemForSaleList[m_stDialogBoxInfo[11].cMode - 1]->m_wWeight / 100) > m_iStr)
			//{
			//	wsprintf(cTemp, DRAW_DIALOGBOX_SHOP11, (m_pItemForSaleList[m_stDialogBoxInfo[11].cMode - 1]->m_wWeight / 100));
			//	PutAlignedString(sX + 25, sX + 240, sY + 258, cTemp, 195, 25, 25);
			//	PutAlignedString(sX + 26, sX + 241, sY + 258, cTemp, 195, 25, 25); // "*Your STR should be at least %d to use this item."
			//	bFlagRedShown = true;
			//}
			if (bFlagStatLow == true) // Means some stat is too low
			{
				//strcpy(cTemp, DRAW_DIALOGBOX_SHOP21); // "(Warning!) Your stat is too low for this item."
				//PutAlignedString(sX + 25, sX + 240, sY + 258, cTemp, 195, 25, 25);
				//PutAlignedString(sX + 26, sX + 241, sY + 258, cTemp, 195, 25, 25);
				//bFlagRedShown = true;
			}
			else if ((strstr(m_pItemForSaleList[m_stDialogBoxInfo[11].cMode - 1]->m_cName, "(M)") != 0)
				&& (m_sPlayerType > 3))
			{
				strcpy(cTemp, DRAW_DIALOGBOX_SHOP22); // "(Warning!) only for male."
				PutAlignedString(sX + 25, sX + 240, sY + 258, cTemp, 195, 25, 25);
				PutAlignedString(sX + 26, sX + 241, sY + 258, cTemp, 195, 25, 25);
				bFlagRedShown = true;
			}
			else if ((strstr(m_pItemForSaleList[m_stDialogBoxInfo[11].cMode - 1]->m_cName, "(W)") != 0)
				&& (m_sPlayerType <= 3))
			{
				strcpy(cTemp, DRAW_DIALOGBOX_SHOP23); // "(Warning!) only for female."
				PutAlignedString(sX + 25, sX + 240, sY + 258, cTemp, 195, 25, 25);
				PutAlignedString(sX + 26, sX + 241, sY + 258, cTemp, 195, 25, 25);
				bFlagRedShown = true;
			}
			break;

		case DEF_EQUIPPOS_NONE:
			break;
		}

		if (m_pItemForSaleList[m_stDialogBoxInfo[11].cMode - 1]->m_sLevelLimit != 0)
		{
			strcpy(cTemp, DRAW_DIALOGBOX_SHOP24); // "Level"
			if (m_iLevel >= m_pItemForSaleList[m_stDialogBoxInfo[11].cMode - 1]->m_sLevelLimit)
			{
				PutString(sX + 90, sY + 190, cTemp, RGB(40, 10, 10));
				PutString(sX + 91, sY + 190, cTemp, RGB(40, 10, 10));
				wsprintf(cTemp, DRAW_DIALOGBOX_SHOP25, m_pItemForSaleList[m_stDialogBoxInfo[11].cMode - 1]->m_sLevelLimit);
				PutString(sX + 140, sY + 190, cTemp, RGB(45, 25, 25));// ": above %d"
			}
			else
			{
				PutString(sX + 90, sY + 190, cTemp, RGB(40, 10, 10));
				PutString(sX + 91, sY + 190, cTemp, RGB(40, 10, 10));
				wsprintf(cTemp, DRAW_DIALOGBOX_SHOP25, m_pItemForSaleList[m_stDialogBoxInfo[11].cMode - 1]->m_sLevelLimit);
				PutString(sX + 140, sY + 190, cTemp, RGB(125, 25, 25));// ": above %d"
				if (bFlagRedShown == false)
				{
					strcpy(cTemp, DRAW_DIALOGBOX_SHOP26); // "(Warning!) Your level is too low for this item."
					PutAlignedString(sX + 25, sX + 240, sY + 258, cTemp, 195, 25, 25);
					PutAlignedString(sX + 25 + 1, sX + 240 + 1, sY + 258, cTemp, 195, 25, 25);
					bFlagRedShown = true;
				}
			}
		}

		m_pSprite[DEF_SPRID_INTERFACE_ND_GAME6]->PutSpriteFast(sX + 156, sY + 219, 19, dwTime);
		m_pSprite[DEF_SPRID_INTERFACE_ND_GAME6]->PutSpriteFast(sX + 170, sY + 219, 19, dwTime);
		PutString(sX + 123 - 35, sY + 237 - 10, DRAW_DIALOGBOX_SHOP27, RGB(40, 10, 10)); // "Quantity:"
		PutString(sX + 124 - 35, sY + 237 - 10, DRAW_DIALOGBOX_SHOP27, RGB(40, 10, 10));
		if (iGetTopDialogBoxIndex() == 11 && msZ != 0)
		{
			m_stDialogBoxInfo[11].sV3 = m_stDialogBoxInfo[11].sV3 + msZ / 60;
			m_DInput.m_sZ = 0;
		}
		if (m_stDialogBoxInfo[11].sV3 > (50 - _iGetTotalItemNum())) m_stDialogBoxInfo[11].sV3 = (50 - _iGetTotalItemNum());
		if (m_stDialogBoxInfo[11].sV3 < 1) m_stDialogBoxInfo[11].sV3 = 1;

		if (m_stDialogBoxInfo[11].sV3 >= 10)
		{
			ZeroMemory(cTemp, sizeof(cTemp));
			_itoa(m_stDialogBoxInfo[11].sV3, cTemp, 10);
			cTemp[1] = 0;
			PutString(sX - 35 + 186, sY - 10 + 237, cTemp, RGB(40, 10, 10));
			PutString(sX - 35 + 187, sY - 10 + 237, cTemp, RGB(40, 10, 10));
			ZeroMemory(cTemp, sizeof(cTemp));
			_itoa(m_stDialogBoxInfo[11].sV3, cTemp, 10);
			PutString(sX - 35 + 200, sY - 10 + 237, (cTemp + 1), RGB(40, 10, 10));
			PutString(sX - 35 + 201, sY - 10 + 237, (cTemp + 1), RGB(40, 10, 10));
		}
		else
		{
			PutString(sX - 35 + 186, sY - 10 + 237, "0", RGB(40, 10, 10));
			PutString(sX - 35 + 187, sY - 10 + 237, "0", RGB(40, 10, 10));
			ZeroMemory(cTemp, sizeof(cTemp));
			_itoa(m_stDialogBoxInfo[11].sV3, cTemp, 10);
			PutString(sX - 35 + 200, sY - 10 + 237, (cTemp), RGB(40, 10, 10));
			PutString(sX - 35 + 201, sY - 10 + 237, (cTemp), RGB(40, 10, 10));
		}
		m_pSprite[DEF_SPRID_INTERFACE_ND_GAME6]->PutSpriteFast(sX + 156, sY + 244, 20, dwTime);
		m_pSprite[DEF_SPRID_INTERFACE_ND_GAME6]->PutSpriteFast(sX + 170, sY + 244, 20, dwTime);

		if ((msX >= sX + DEF_LBTNPOSX) && (msX <= sX + DEF_LBTNPOSX + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 31);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + DEF_BTNPOSY, 30);

		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 17);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + DEF_BTNPOSY, 16);
		break;
	}
}

int CGame::_iCalcSkillSSNpoint(int iLevel)
{
	int iRet;

	if (iLevel < 1) return 1;

	if (iLevel <= 50)
		iRet = iLevel;
	else if (iLevel > 50) {
		iRet = (iLevel * iLevel) / 10;
	}

	//iRet = iLevel; // centu

	return iRet;
}

void CGame::DrawDialogBox_Skill(short msX, short msY, short msZ, char cLB)
{
	short sX, sY;
	int  i, iTotalLines, iPointerLoc;
	char cTemp[255], cTemp2[255], cTemp3[255];
	float d1, d2, d3;
	
	if (m_bUseOldPanels)
	{

		sX = m_stDialogBoxInfo[15].sX;
		sY = m_stDialogBoxInfo[15].sY;

		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME6, sX, sY, 0); // Normal Dialog
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_TEXT2, sX, sY, 1); // Skill Dialog Title Bar

		switch (m_stDialogBoxInfo[15].cMode) {
		case 0:
			for (i = 0; i < 17; i++)
				if ((i < DEF_MAXSKILLTYPE) && (m_pSkillCfgList[i + m_stDialogBoxInfo[15].sView] != 0))
				{
					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "%s", m_pSkillCfgList[i + m_stDialogBoxInfo[15].sView]->m_cName);
					m_Misc.ReplaceString(cTemp, '-', ' ');
					ZeroMemory(cTemp2, sizeof(cTemp2));
					wsprintf(cTemp2, "%3d%%", m_pSkillCfgList[i + m_stDialogBoxInfo[15].sView]->m_iLevel);
					if ((msX >= sX + 25) && (msX <= sX + 166) && (msY >= sY + 45 + i * 15) && (msY <= sY + 59 + i * 15))
					{
						if ((m_pSkillCfgList[i + m_stDialogBoxInfo[15].sView]->m_bIsUseable == true)
							&& (m_pSkillCfgList[i + m_stDialogBoxInfo[15].sView]->m_iLevel != 0))
						{
							PutString(sX + 30, sY + 45 + i * 15, cTemp, RGB(255, 255, 255));
							PutString(sX + 183, sY + 45 + i * 15, cTemp2, RGB(255, 255, 255));
						}
						else
						{
							PutString(sX + 30, sY + 45 + i * 15, cTemp, RGB(5, 5, 5));
							PutString(sX + 183, sY + 45 + i * 15, cTemp2, RGB(5, 5, 5));
						}
					}
					else
					{
						if ((m_pSkillCfgList[i + m_stDialogBoxInfo[15].sView]->m_bIsUseable == true)
							&& (m_pSkillCfgList[i + m_stDialogBoxInfo[15].sView]->m_iLevel != 0))
						{
							PutString(sX + 30, sY + 45 + i * 15, cTemp, RGB(34, 30, 120));
							PutString(sX + 183, sY + 45 + i * 15, cTemp2, RGB(34, 30, 120));
						}
						else
						{
							PutString(sX + 30, sY + 45 + i * 15, cTemp, RGB(5, 5, 5));
							PutString(sX + 183, sY + 45 + i * 15, cTemp2, RGB(5, 5, 5));
						}
					}

					if (m_iDownSkillIndex == (i + m_stDialogBoxInfo[15].sView))
						m_pSprite[DEF_SPRID_INTERFACE_ADDINTERFACE]->PutTransSpriteRGB(sX + 215, sY + 47 + i * 15, 21, 50, 50, 50, m_dwTime);
					else m_pSprite[DEF_SPRID_INTERFACE_ADDINTERFACE]->PutSpriteFast(sX + 215, sY + 47 + i * 15, 20, m_dwTime);
				}

			iTotalLines = 0;
			for (i = 0; i < DEF_MAXSKILLTYPE; i++)
				if (m_pSkillCfgList[i] != 0) iTotalLines++;

			if (iTotalLines > 17)
			{
				d1 = (float)m_stDialogBoxInfo[15].sView;
				d2 = (float)(iTotalLines - 17);
				d3 = (274.0f * d1) / d2;
				iPointerLoc = (int)d3;
			}
			else iPointerLoc = 0;
			if (iTotalLines > 17)
			{
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME6, sX, sY, 1);
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME6, sX + 242, sY + iPointerLoc + 35, 7);
			}

			if (cLB != 0 && iTotalLines > 17)
			{
				if ((iGetTopDialogBoxIndex() == 15))
				{
					if ((msX >= sX + 240) && (msX <= sX + 260) && (msY >= sY + 30) && (msY <= sY + 320))
					{
						d1 = (float)(msY - (sY + 35));
						d2 = (float)(iTotalLines - 17);
						d3 = (d1 * d2) / 274.0f;
						iPointerLoc = (int)(d3 + 0.5f);
						if (iPointerLoc > iTotalLines - 17) iPointerLoc = iTotalLines - 17;
						m_stDialogBoxInfo[15].sView = iPointerLoc;
					}
				}
			}
			else m_stDialogBoxInfo[15].bIsScrollSelected = false;
			if (iGetTopDialogBoxIndex() == 15 && msZ != 0)
			{
				if (msZ > 0) m_stDialogBoxInfo[15].sView--;
				if (msZ < 0) m_stDialogBoxInfo[15].sView++;
				m_DInput.m_sZ = 0;
			}
			if (m_stDialogBoxInfo[15].sView < 0) m_stDialogBoxInfo[15].sView = 0;
			if (iTotalLines > 17 && m_stDialogBoxInfo[15].sView > iTotalLines - 17) m_stDialogBoxInfo[15].sView = iTotalLines - 17;
			break;
		}
	}
	else {

		sX = m_stDialogBoxInfo[15].sX;
		sY = m_stDialogBoxInfo[15].sY;

		//DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME2, sX, sY, 0); // Normal Dialog
		//DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_TEXT, sX, sY, 1); // Skill Dialog Title Bar

		short limitX, limitY;
		limitX = sX + m_stDialogBoxInfo[15].sSizeX + 20;
		limitY = sY + m_stDialogBoxInfo[15].sSizeY + 25;

		m_DDraw.DrawShadowBox(sX, sY, limitX, limitY, 0, true);
		if (!m_bDialogTrans) m_DDraw.DrawShadowBox(sX, sY, limitX, limitY, 0, true);

		m_DDraw.DrawShadowBox(sX, sY, limitX, sY + 25, 0, true);
		if (!m_bDialogTrans) m_DDraw.DrawShadowBox(sX, sY, limitX, sY + 25, 0, true);
		PutString_SprFont2(sX + 115, sY + 5, "Skills", 240, 240, 240);

		PutString(sX + 30, sY + 15 + 15, "Name", RGB(0, 255, 0));
		PutString(sX + 150, sY + 15 + 15, "Progress", RGB(0, 255, 0));
		PutString(sX + 183 + 30 + 10, sY + 15 + 15, "Level", RGB(0, 255, 0));

		switch (m_stDialogBoxInfo[15].cMode) {
		case 0:
			int x = 0;
			for (i = 0; i < 24; i++)
				if ((i < DEF_MAXSKILLTYPE) && (m_pSkillCfgList[i + m_stDialogBoxInfo[15].sView] != 0))
				{
					if (strcmp(m_pSkillCfgList[i + m_stDialogBoxInfo[15].sView]->m_cName, "????") == 0) continue;

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "%s", m_pSkillCfgList[i + m_stDialogBoxInfo[15].sView]->m_cName);
					m_Misc.ReplaceString(cTemp, '-', ' ');
					ZeroMemory(cTemp2, sizeof(cTemp2));
					wsprintf(cTemp2, "%3d", m_pSkillCfgList[i + m_stDialogBoxInfo[15].sView]->m_iLevel);

					if (m_pSkillCfgList[i + m_stDialogBoxInfo[15].sView]->m_iLevel < 100) {
						ZeroMemory(cTemp3, sizeof(cTemp3));
						wsprintf(cTemp3, "%d/%d", m_iSkillSSN[i + m_stDialogBoxInfo[15].sView], _iCalcSkillSSNpoint(m_pSkillCfgList[i + m_stDialogBoxInfo[15].sView]->m_iLevel + 1));
					}
					else
					{
						ZeroMemory(cTemp3, sizeof(cTemp3));
						wsprintf(cTemp3, "%d/----", m_iSkillSSN[i + m_stDialogBoxInfo[15].sView]);
					}
					if ((msX >= sX + 25) && (msX <= sX + 180) && (msY >= sY + 30 + (x * 15) + 15) && (msY <= sY + 44 + (x * 15) + 15))
					{
						if ((m_pSkillCfgList[i + m_stDialogBoxInfo[15].sView]->m_bIsUseable == true)
							&& (m_pSkillCfgList[i + m_stDialogBoxInfo[15].sView]->m_iLevel != 0))
						{
							PutString(sX + 30, sY + 30 + (x * 15) + 15, cTemp, RGB(255, 255, 0));
							PutAlignedString(sX + 153, sX + 153 + 40, sY + 30 + (x * 15) + 15, cTemp3, 255, 255, 0);
							PutString(sX + 183 + 30 + 15, sY + 30 + (x * 15) + 15, cTemp2, RGB(255, 255, 0));
						}
						else
						{
							PutString(sX + 30, sY + 30 + (x * 15) + 15, cTemp, RGB(255, 255, 255));
							PutAlignedString(sX + 153, sX + 153 + 40, sY + 30 + (x * 15) + 15, cTemp3, 255, 255, 255);
							PutString(sX + 183 + 30 + 15, sY + 30 + (x * 15) + 15, cTemp2, RGB(255, 255, 255));
						}
					}
					else
					{
						if ((m_pSkillCfgList[i + m_stDialogBoxInfo[15].sView]->m_bIsUseable == true)
							&& (m_pSkillCfgList[i + m_stDialogBoxInfo[15].sView]->m_iLevel != 0))
						{
							PutString(sX + 30, sY + 30 + (x * 15) + 15, cTemp, RGB(70, 130, 180));
							PutAlignedString(sX + 153, sX + 153 + 40, sY + 30 + (x * 15) + 15, cTemp3, 70, 130, 180);
							PutString(sX + 183 + 30 + 15, sY + 30 + (x * 15) + 15, cTemp2, RGB(70, 130, 180));
						}
						else
						{
							PutString(sX + 30, sY + 30 + (x * 15) + 15, cTemp, RGB(255, 255, 255));
							PutAlignedString(sX + 153, sX + 153 + 40, sY + 30 + (x * 15) + 15, cTemp3, 255, 255, 255);
							PutString(sX + 183 + 30 + 15, sY + 30 + (x * 15) + 15, cTemp2, RGB(255, 255, 255));
						}
					}

					x++;
				}

			
			break;
		}
	}
}
void CGame::DrawDialogBox_SkillDlg(short msX, short msY, short msZ, char cLB)
{
	int i, iLoc, iAdjX, iAdjY;
	char cTemp[120], cTemp2[120];
	short sX, sY, szX;
	char cStr1[64], cStr2[64], cStr3[64];
	UINT32 dwTime = m_dwCurTime;

	iAdjX = 5;
	iAdjY = 8;

	switch (m_stDialogBoxInfo[26].cMode) {
	case 1: // Alchemy waiting incredients
		if (m_stDialogBoxInfo[26].cStr[0] != 0)
		{
			sX = m_stDialogBoxInfo[26].sX + iAdjX + (m_stDialogBoxInfo[26].cStr[0] - (rand() % (m_stDialogBoxInfo[26].cStr[0] * 2)));
			sY = m_stDialogBoxInfo[26].sY + iAdjY + (m_stDialogBoxInfo[26].cStr[0] - (rand() % (m_stDialogBoxInfo[26].cStr[0] * 2)));
		}
		else
		{
			sX = m_stDialogBoxInfo[26].sX;
			sY = m_stDialogBoxInfo[26].sY;
		}

		m_pSprite[DEF_SPRID_INTERFACE_ADDINTERFACE]->PutSpriteFast(sX, sY, 1, dwTime);

		if (m_stDialogBoxInfo[26].sV1 != -1)
			m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT +
			m_pItemList[m_stDialogBoxInfo[26].sV1]->m_sSprite]->PutTransSprite50(sX + iAdjX + 55 + (1 - (rand() % 3)), sY + iAdjY + 55 + (1 - (rand() % 3)),
			m_pItemList[m_stDialogBoxInfo[26].sV1]->m_sSpriteFrame, dwTime);

		if (m_stDialogBoxInfo[26].sV2 != -1)
			m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT +
			m_pItemList[m_stDialogBoxInfo[26].sV2]->m_sSprite]->PutTransSprite50(sX + iAdjX + 55 + 45 * 1 + (1 - (rand() % 3)), sY + iAdjY + 55 + (1 - (rand() % 3)),
			m_pItemList[m_stDialogBoxInfo[26].sV2]->m_sSpriteFrame, dwTime);

		if (m_stDialogBoxInfo[26].sV3 != -1)
			m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT +
			m_pItemList[m_stDialogBoxInfo[26].sV3]->m_sSprite]->PutTransSprite50(sX + iAdjX + 55 + 45 * 2 + (1 - (rand() % 3)), sY + iAdjY + 55 + (1 - (rand() % 3)),
			m_pItemList[m_stDialogBoxInfo[26].sV3]->m_sSpriteFrame, dwTime);

		if (m_stDialogBoxInfo[26].sV4 != -1)
			m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT +
			m_pItemList[m_stDialogBoxInfo[26].sV4]->m_sSprite]->PutTransSprite50(sX + iAdjX + 55 + (1 - (rand() % 3)), sY + iAdjY + 100 + (1 - (rand() % 3)),
			m_pItemList[m_stDialogBoxInfo[26].sV4]->m_sSpriteFrame, dwTime);

		if (m_stDialogBoxInfo[26].sV5 != -1)
			m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT +
			m_pItemList[m_stDialogBoxInfo[26].sV5]->m_sSprite]->PutTransSprite50(sX + iAdjX + 55 + 45 * 1 + (1 - (rand() % 3)), sY + iAdjY + 100 + (1 - (rand() % 3)),
			m_pItemList[m_stDialogBoxInfo[26].sV5]->m_sSpriteFrame, dwTime);

		if (m_stDialogBoxInfo[26].sV6 != -1)
			m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT +
			m_pItemList[m_stDialogBoxInfo[26].sV6]->m_sSprite]->PutTransSprite50(sX + iAdjX + 55 + 45 * 2 + (1 - (rand() % 3)), sY + iAdjY + 100 + (1 - (rand() % 3)),
			m_pItemList[m_stDialogBoxInfo[26].sV6]->m_sSpriteFrame, dwTime);

		if ((msX >= sX + iAdjX + 60) && (msX <= sX + iAdjX + 153) && (msY >= sY + iAdjY + 175) && (msY <= sY + iAdjY + 195))
			PutString_SprFont(sX + iAdjX + 60, sY + iAdjY + 175, "Try Now!", 16, 16, 30);
		else PutString_SprFont(sX + iAdjX + 60, sY + iAdjY + 175, "Try Now!", 6, 6, 20);

		break;

	case 2: // Alchemy, creating a potion
		if (m_stDialogBoxInfo[26].cStr[0] != 0) {
			sX = m_stDialogBoxInfo[26].sX + iAdjX + (m_stDialogBoxInfo[26].cStr[0] - (rand() % (m_stDialogBoxInfo[26].cStr[0] * 2)));
			sY = m_stDialogBoxInfo[26].sY + iAdjY + (m_stDialogBoxInfo[26].cStr[0] - (rand() % (m_stDialogBoxInfo[26].cStr[0] * 2)));
		}
		else
		{
			sX = m_stDialogBoxInfo[26].sX;
			sY = m_stDialogBoxInfo[26].sY;
		}
		m_pSprite[DEF_SPRID_INTERFACE_ADDINTERFACE]->PutSpriteFast(sX, sY, 1, dwTime);

		if (m_stDialogBoxInfo[26].sV1 != -1)
			m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT +
			m_pItemList[m_stDialogBoxInfo[26].sV1]->m_sSprite]->PutTransSprite50(sX + iAdjX + 55 + (1 - (rand() % 3)), sY + iAdjY + 55 + (1 - (rand() % 3)),
			m_pItemList[m_stDialogBoxInfo[26].sV1]->m_sSpriteFrame, dwTime);

		if (m_stDialogBoxInfo[26].sV2 != -1)
			m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT +
			m_pItemList[m_stDialogBoxInfo[26].sV2]->m_sSprite]->PutTransSprite50(sX + iAdjX + 55 + 45 * 1 + (1 - (rand() % 3)), sY + iAdjY + 55 + (1 - (rand() % 3)),
			m_pItemList[m_stDialogBoxInfo[26].sV2]->m_sSpriteFrame, dwTime);

		if (m_stDialogBoxInfo[26].sV3 != -1)
			m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT +
			m_pItemList[m_stDialogBoxInfo[26].sV3]->m_sSprite]->PutTransSprite50(sX + iAdjX + 55 + 45 * 2 + (1 - (rand() % 3)), sY + iAdjY + 55 + (1 - (rand() % 3)),
			m_pItemList[m_stDialogBoxInfo[26].sV3]->m_sSpriteFrame, dwTime);

		if (m_stDialogBoxInfo[26].sV4 != -1)
			m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT +
			m_pItemList[m_stDialogBoxInfo[26].sV4]->m_sSprite]->PutTransSprite50(sX + iAdjX + 55 + (1 - (rand() % 3)), sY + iAdjY + 100 + (1 - (rand() % 3)),
			m_pItemList[m_stDialogBoxInfo[26].sV4]->m_sSpriteFrame, dwTime);

		if (m_stDialogBoxInfo[26].sV5 != -1)
			m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT +
			m_pItemList[m_stDialogBoxInfo[26].sV5]->m_sSprite]->PutTransSprite50(sX + iAdjX + 55 + 45 * 1 + (1 - (rand() % 3)), sY + iAdjY + 100 + (1 - (rand() % 3)),
			m_pItemList[m_stDialogBoxInfo[26].sV5]->m_sSpriteFrame, dwTime);

		if (m_stDialogBoxInfo[26].sV6 != -1)
			m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT +
			m_pItemList[m_stDialogBoxInfo[26].sV6]->m_sSprite]->PutTransSprite50(sX + iAdjX + 55 + 45 * 2 + (1 - (rand() % 3)), sY + iAdjY + 100 + (1 - (rand() % 3)),
			m_pItemList[m_stDialogBoxInfo[26].sV6]->m_sSpriteFrame, dwTime);

		PutString_SprFont(sX + iAdjX + 60, sY + iAdjY + 175, "Creating...", 20, 6, 6);


		if ((dwTime - m_stDialogBoxInfo[26].dwT1) > 1000)
		{
			m_stDialogBoxInfo[26].dwT1 = dwTime;
			m_stDialogBoxInfo[26].cStr[0]++;
		}

		if (m_stDialogBoxInfo[26].cStr[0] >= 5)
		{
			bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_REQ_CREATEPORTION, 0, 0, 0, 0, 0);
			DisableDialogBox(26);
			PlaySound('E', 42, 0);
		}
		break;

	case 3: // Manuf: Choose what you want to create
		sX = m_stDialogBoxInfo[26].sX;
		sY = m_stDialogBoxInfo[26].sY;
		szX = m_stDialogBoxInfo[26].sSizeX;
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME7, sX, sY, 0);
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_TEXT2, sX, sY, 8);
		PutString(sX + iAdjX + 44, sY + iAdjY + 38, "Name", RGB(0, 0, 0));
		PutString(sX + iAdjX + 171, sY + iAdjY + 38, "Max.Skill", RGB(0, 0, 0));

		iLoc = 0;
		for (i = 0; i < 13; i++)
			if (m_pDispBuildItemList[i + m_stDialogBoxInfo[26].sView] != 0) {

				ZeroMemory(cTemp, sizeof(cTemp));
				GetItemName(m_pDispBuildItemList[i + m_stDialogBoxInfo[26].sView]->m_cName, 0, cStr1, cStr2, cStr3);
				wsprintf(cTemp, "%s", cStr1);
				ZeroMemory(cTemp2, sizeof(cTemp2));
				wsprintf(cTemp2, "%d%%", m_pDispBuildItemList[i + m_stDialogBoxInfo[26].sView]->m_iMaxSkill);

				if ((msX >= sX + 30) && (msX <= sX + 180) && (msY >= sY + iAdjY + 55 + iLoc * 15) && (msY <= sY + iAdjY + 69 + iLoc * 15))
				{
					PutString(sX + 30, sY + iAdjY + 55 + iLoc * 15, cTemp, RGB(255, 255, 255));
					PutString(sX + 190, sY + iAdjY + 55 + iLoc * 15, cTemp2, RGB(255, 255, 255));
				}
				else
				{
					if (m_pDispBuildItemList[i + m_stDialogBoxInfo[26].sView]->m_bBuildEnabled == true)
					{
						PutString(sX + 30, sY + iAdjY + 55 + iLoc * 15, cTemp, RGB(34, 30, 120));
						PutString(sX + 190, sY + iAdjY + 55 + iLoc * 15, cTemp2, RGB(34, 30, 120));
					}
					else
					{
						PutString(sX + 30, sY + iAdjY + 55 + iLoc * 15, cTemp, RGB(45, 25, 25));
						PutString(sX + 190, sY + iAdjY + 55 + iLoc * 15, cTemp2, RGB(45, 25, 25));
					}
				}

				iLoc++;
			}
		if ((m_stDialogBoxInfo[26].sView >= 1) && (m_pDispBuildItemList[m_stDialogBoxInfo[26].sView - 1] != 0))
			m_pSprite[DEF_SPRID_INTERFACE_ND_GAME6]->PutSpriteFast(sX + iAdjX + 225, sY + iAdjY + 210, 23, dwTime);
		else m_pSprite[DEF_SPRID_INTERFACE_ND_GAME6]->PutTransSpriteRGB(sX + iAdjX + 225, sY + iAdjY + 210, 23, 5, 5, 5, dwTime);

		if (m_pDispBuildItemList[m_stDialogBoxInfo[26].sView + 13] != 0)
			m_pSprite[DEF_SPRID_INTERFACE_ND_GAME6]->PutSpriteFast(sX + iAdjX + 225, sY + iAdjY + 230, 24, dwTime);
		else m_pSprite[DEF_SPRID_INTERFACE_ND_GAME6]->PutTransSpriteRGB(sX + iAdjX + 225, sY + iAdjY + 230, 24, 5, 5, 5, dwTime);

		if ((cLB != 0) && (iGetTopDialogBoxIndex() == 26)) {
			if ((msX >= sX + iAdjX + 225) && (msX <= sX + iAdjX + 245) && (msY >= sY + iAdjY + 210) && (msY <= sY + iAdjY + 230)) {
				m_stDialogBoxInfo[26].sView--;
			}

			if ((msX >= sX + iAdjX + 225) && (msX <= sX + iAdjX + 245) && (msY >= sY + iAdjY + 230) && (msY <= sY + iAdjY + 250)) {
				if (m_pDispBuildItemList[m_stDialogBoxInfo[26].sView + 13] != 0)
					m_stDialogBoxInfo[26].sView++;
			}
		}
		if ((msZ != 0) && (iGetTopDialogBoxIndex() == 26)) {
			m_stDialogBoxInfo[26].sView = m_stDialogBoxInfo[26].sView - msZ / 60;
			m_DInput.m_sZ = 0;
		}
		if (m_pDispBuildItemList[m_stDialogBoxInfo[26].sView + 12] == 0)
		{
			while (1)
			{
				m_stDialogBoxInfo[26].sView--;
				if (m_stDialogBoxInfo[26].sView < 1) break;
				if (m_pDispBuildItemList[m_stDialogBoxInfo[26].sView + 12] != 0) break;
			}
		}
		if (m_stDialogBoxInfo[26].sView < 0) m_stDialogBoxInfo[26].sView = 0;

		PutAlignedString(sX, sX + m_stDialogBoxInfo[26].sSizeX, sY + 265, DRAW_DIALOGBOX_SKILLDLG2, 55, 25, 25);//" List of items which you can make with"
		PutAlignedString(sX, sX + m_stDialogBoxInfo[26].sSizeX, sY + 280, DRAW_DIALOGBOX_SKILLDLG3, 55, 25, 25);//"your current skill. The items you can"
		PutAlignedString(sX, sX + m_stDialogBoxInfo[26].sSizeX, sY + 295, DRAW_DIALOGBOX_SKILLDLG4, 55, 25, 25);//"make now with your current stuff will"
		PutAlignedString(sX, sX + m_stDialogBoxInfo[26].sSizeX, sY + 310, DRAW_DIALOGBOX_SKILLDLG5, 55, 25, 25);//"be displayed in blue. "
		PutAlignedString(sX, sX + m_stDialogBoxInfo[26].sSizeX, sY + 340, DRAW_DIALOGBOX_SKILLDLG6, 55, 25, 25);//"Select an item you want to manufacture."
		break;

	case 4: // Manuf: Waiting for incredients
		sX = m_stDialogBoxInfo[26].sX;
		sY = m_stDialogBoxInfo[26].sY;
		szX = m_stDialogBoxInfo[26].sSizeX;
		iAdjX = -1;
		iAdjY = -7;
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME7, sX, sY, 0);
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_TEXT2, sX, sY, 8);
		m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_pDispBuildItemList[m_stDialogBoxInfo[26].cStr[0]]->m_iSprH]->PutSpriteFast(sX + iAdjX + 62 + 5, sY + iAdjY + 84 + 17,
			m_pDispBuildItemList[m_stDialogBoxInfo[26].cStr[0]]->m_iSprFrame, dwTime);

		ZeroMemory(cTemp, sizeof(cTemp));
		GetItemName(m_pDispBuildItemList[m_stDialogBoxInfo[26].cStr[0]]->m_cName, 0, cStr1, cStr2, cStr3);
		wsprintf(cTemp, "%s", cStr1);
		PutString(sX + iAdjX + 44 + 10 + 60, sY + iAdjY + 55, cTemp, RGB(255, 255, 255));

		wsprintf(cTemp, DRAW_DIALOGBOX_SKILLDLG7 // "Skill level: %d/%d"
			, m_pDispBuildItemList[m_stDialogBoxInfo[26].cStr[0]]->m_iSkillLimit
			, m_pDispBuildItemList[m_stDialogBoxInfo[26].cStr[0]]->m_iMaxSkill);
		PutString(sX + iAdjX + 44 + 10 + 60, sY + iAdjY + 55 + 2 * 15, cTemp, RGB(45, 25, 25));
		PutString(sX + iAdjX + 44 + 10 + 60, sY + iAdjY + 55 + 3 * 15 + 5, DRAW_DIALOGBOX_SKILLDLG8, RGB(45, 25, 25));//"Ingredients Needed:"

		iLoc = 4;
		if (m_pDispBuildItemList[m_stDialogBoxInfo[26].cStr[0]]->m_iElementCount[1] != 0) {
			GetItemName(m_pDispBuildItemList[m_stDialogBoxInfo[26].cStr[0]]->m_cElementName1, 0, cStr1, cStr2, cStr3);
			wsprintf(cTemp, "%s", cStr1);
			if (m_pDispBuildItemList[m_stDialogBoxInfo[26].cStr[0]]->m_bElementFlag[1] == true)
				PutString(sX + iAdjX + 44 + 20 + 60, sY + iAdjY + 55 + iLoc * 15 + 5, cTemp, RGB(45, 25, 25));
			else PutString(sX + iAdjX + 44 + 20 + 60, sY + iAdjY + 55 + iLoc * 15 + 5, cTemp, RGB(150, 150, 150));
			iLoc++;
		}

		if (m_pDispBuildItemList[m_stDialogBoxInfo[26].cStr[0]]->m_iElementCount[2] != 0) {
			GetItemName(m_pDispBuildItemList[m_stDialogBoxInfo[26].cStr[0]]->m_cElementName2, 0, cStr1, cStr2, cStr3);
			wsprintf(cTemp, "%s", cStr1);
			if (m_pDispBuildItemList[m_stDialogBoxInfo[26].cStr[0]]->m_bElementFlag[2] == true)
				PutString(sX + iAdjX + 44 + 20 + 60, sY + iAdjY + 55 + iLoc * 15 + 5, cTemp, RGB(45, 25, 25));
			else PutString(sX + iAdjX + 44 + 20 + 60, sY + iAdjY + 55 + iLoc * 15 + 5, cTemp, RGB(150, 150, 150));
			iLoc++;
		}

		if (m_pDispBuildItemList[m_stDialogBoxInfo[26].cStr[0]]->m_iElementCount[3] != 0) {
			GetItemName(m_pDispBuildItemList[m_stDialogBoxInfo[26].cStr[0]]->m_cElementName3, 0, cStr1, cStr2, cStr3);
			wsprintf(cTemp, "%s", cStr1);
			if (m_pDispBuildItemList[m_stDialogBoxInfo[26].cStr[0]]->m_bElementFlag[3] == true)
				PutString(sX + iAdjX + 44 + 20 + 60, sY + iAdjY + 55 + iLoc * 15 + 5, cTemp, RGB(45, 25, 25));
			else PutString(sX + iAdjX + 44 + 20 + 60, sY + iAdjY + 55 + iLoc * 15 + 5, cTemp, RGB(150, 150, 150));
			iLoc++;
		}

		if (m_pDispBuildItemList[m_stDialogBoxInfo[26].cStr[0]]->m_iElementCount[4] != 0) {
			GetItemName(m_pDispBuildItemList[m_stDialogBoxInfo[26].cStr[0]]->m_cElementName4, 0, cStr1, cStr2, cStr3);
			wsprintf(cTemp, "%s", cStr1);
			if (m_pDispBuildItemList[m_stDialogBoxInfo[26].cStr[0]]->m_bElementFlag[4] == true)
				PutString(sX + iAdjX + 44 + 20 + 60, sY + iAdjY + 55 + iLoc * 15 + 5, cTemp, RGB(45, 25, 25));
			else PutString(sX + iAdjX + 44 + 20 + 60, sY + iAdjY + 55 + iLoc * 15 + 5, cTemp, RGB(150, 150, 150));
			iLoc++;
		}

		if (m_pDispBuildItemList[m_stDialogBoxInfo[26].cStr[0]]->m_iElementCount[5] != 0) {
			GetItemName(m_pDispBuildItemList[m_stDialogBoxInfo[26].cStr[0]]->m_cElementName5, 0, cStr1, cStr2, cStr3);
			wsprintf(cTemp, "%s", cStr1);
			if (m_pDispBuildItemList[m_stDialogBoxInfo[26].cStr[0]]->m_bElementFlag[5] == true)
				PutString(sX + iAdjX + 44 + 20 + 60, sY + iAdjY + 55 + iLoc * 15 + 5, cTemp, RGB(45, 25, 25));
			else PutString(sX + iAdjX + 44 + 20 + 60, sY + iAdjY + 55 + iLoc * 15 + 5, cTemp, RGB(150, 150, 150));
			iLoc++;
		}

		if (m_pDispBuildItemList[m_stDialogBoxInfo[26].cStr[0]]->m_iElementCount[6] != 0) {
			GetItemName(m_pDispBuildItemList[m_stDialogBoxInfo[26].cStr[0]]->m_cElementName6, 0, cStr1, cStr2, cStr3);
			wsprintf(cTemp, "%s", cStr1);
			if (m_pDispBuildItemList[m_stDialogBoxInfo[26].cStr[0]]->m_bElementFlag[6] == true)
				PutString(sX + iAdjX + 44 + 20 + 60, sY + iAdjY + 55 + iLoc * 15 + 5, cTemp, RGB(45, 25, 25));
			else PutString(sX + iAdjX + 44 + 20 + 60, sY + iAdjY + 55 + iLoc * 15 + 5, cTemp, RGB(150, 150, 150));
			iLoc++;
		}

		if (m_pDispBuildItemList[m_stDialogBoxInfo[26].cStr[0]]->m_bBuildEnabled == true)
		{
			m_pSprite[DEF_SPRID_INTERFACE_ADDINTERFACE]->PutSpriteFast(sX + iAdjX + 55 + 30 + 13, sY + iAdjY + 55 + 180, 2, dwTime);
			m_pSprite[DEF_SPRID_INTERFACE_ADDINTERFACE]->PutSpriteFast(sX + iAdjX + 55 + 30 + 45 * 1 + 13, sY + iAdjY + 55 + 180, 2, dwTime);
			m_pSprite[DEF_SPRID_INTERFACE_ADDINTERFACE]->PutSpriteFast(sX + iAdjX + 55 + 30 + 45 * 2 + 13, sY + iAdjY + 55 + 180, 2, dwTime);
			m_pSprite[DEF_SPRID_INTERFACE_ADDINTERFACE]->PutSpriteFast(sX + iAdjX + 55 + 30 + 13, sY + iAdjY + 100 + 180, 2, dwTime);
			m_pSprite[DEF_SPRID_INTERFACE_ADDINTERFACE]->PutSpriteFast(sX + iAdjX + 55 + 30 + 45 * 1 + 13, sY + iAdjY + 100 + 180, 2, dwTime);
			m_pSprite[DEF_SPRID_INTERFACE_ADDINTERFACE]->PutSpriteFast(sX + iAdjX + 55 + 30 + 45 * 2 + 13, sY + iAdjY + 100 + 180, 2, dwTime);

			if (m_stDialogBoxInfo[26].sV1 != -1)
				m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT +
				m_pItemList[m_stDialogBoxInfo[26].sV1]->m_sSprite]->PutSpriteFast(sX + iAdjX + 55 + 30 + 13, sY + iAdjY + 55 + 180,
				m_pItemList[m_stDialogBoxInfo[26].sV1]->m_sSpriteFrame, dwTime);

			if (m_stDialogBoxInfo[26].sV2 != -1)
				m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT +
				m_pItemList[m_stDialogBoxInfo[26].sV2]->m_sSprite]->PutSpriteFast(sX + iAdjX + 55 + 45 * 1 + 30 + 13, sY + iAdjY + 55 + 180,
				m_pItemList[m_stDialogBoxInfo[26].sV2]->m_sSpriteFrame, dwTime);

			if (m_stDialogBoxInfo[26].sV3 != -1)
				m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT +
				m_pItemList[m_stDialogBoxInfo[26].sV3]->m_sSprite]->PutSpriteFast(sX + iAdjX + 55 + 45 * 2 + 30 + 13, sY + iAdjY + 55 + 180,
				m_pItemList[m_stDialogBoxInfo[26].sV3]->m_sSpriteFrame, dwTime);

			if (m_stDialogBoxInfo[26].sV4 != -1)
				m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT +
				m_pItemList[m_stDialogBoxInfo[26].sV4]->m_sSprite]->PutSpriteFast(sX + iAdjX + 55 + 30 + 13, sY + iAdjY + 100 + 180,
				m_pItemList[m_stDialogBoxInfo[26].sV4]->m_sSpriteFrame, dwTime);

			if (m_stDialogBoxInfo[26].sV5 != -1)
				m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT +
				m_pItemList[m_stDialogBoxInfo[26].sV5]->m_sSprite]->PutSpriteFast(sX + iAdjX + 55 + 45 * 1 + 30 + 13, sY + iAdjY + 100 + 180,
				m_pItemList[m_stDialogBoxInfo[26].sV5]->m_sSpriteFrame, dwTime);

			if (m_stDialogBoxInfo[26].sV6 != -1)
				m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT +
				m_pItemList[m_stDialogBoxInfo[26].sV6]->m_sSprite]->PutSpriteFast(sX + iAdjX + 55 + 45 * 2 + 30 + 13, sY + iAdjY + 100 + 180,
				m_pItemList[m_stDialogBoxInfo[26].sV6]->m_sSpriteFrame, dwTime);

			PutAlignedString(sX, sX + szX, sY + iAdjY + 230 + 75, DRAW_DIALOGBOX_SKILLDLG15, 55, 25, 25);//" Click MANUFACTURE button after"
			PutAlignedString(sX, sX + szX, sY + iAdjY + 245 + 75, DRAW_DIALOGBOX_SKILLDLG16, 55, 25, 25);//"dragging ingredients in the blanks"
			PutAlignedString(sX, sX + szX, sY + iAdjY + 260 + 75, DRAW_DIALOGBOX_SKILLDLG17, 55, 25, 25);//"to manufacture above item."

			if ((msX >= sX + iAdjX + 32) && (msX <= sX + iAdjX + 95) && (msY >= sY + iAdjY + 353) && (msY <= sY + iAdjY + 372))
				PutString_SprFont(sX + iAdjX + 25, sY + iAdjY + 330 + 23, "Back", 6, 6, 20);
			else PutString_SprFont(sX + iAdjX + 25, sY + iAdjY + 330 + 23, "Back", 0, 0, 7);


			if ((msX >= sX + iAdjX + 160) && (msX <= sX + iAdjX + 255) && (msY >= sY + iAdjY + 353) && (msY <= sY + iAdjY + 372)) {
				if (m_stDialogBoxInfo[26].cStr[4] == 1)
					PutString_SprFont(sX + iAdjX + 153, sY + iAdjY + 330 + 23, "Manufacture", 6, 6, 20);
				else PutString_SprFont(sX + iAdjX + 153, sY + iAdjY + 330 + 23, "Manufacture", 10, 10, 10);
			}
			else {
				if (m_stDialogBoxInfo[26].cStr[4] == 1)
					PutString_SprFont(sX + iAdjX + 153, sY + iAdjY + 330 + 23, "Manufacture", 0, 0, 7);
				else PutString_SprFont(sX + iAdjX + 153, sY + iAdjY + 330 + 23, "Manufacture", 10, 10, 10);
			}
		}
		else {
			PutAlignedString(sX, sX + szX, sY + iAdjY + 200 + 75, DRAW_DIALOGBOX_SKILLDLG18, 55, 25, 25);//"There are not enough ingredients to"
			PutAlignedString(sX, sX + szX, sY + iAdjY + 215 + 75, DRAW_DIALOGBOX_SKILLDLG19, 55, 25, 25);//"manufacture. Needed materials are"
			PutAlignedString(sX, sX + szX, sY + iAdjY + 230 + 75, DRAW_DIALOGBOX_SKILLDLG20, 55, 25, 25);//"displayed in grey."
			if ((msX >= sX + iAdjX + 32) && (msX <= sX + iAdjX + 95) && (msY >= sY + iAdjY + 353) && (msY <= sY + iAdjY + 372))
				PutString_SprFont(sX + iAdjX + 25, sY + iAdjY + 330 + 23, "Back", 6, 6, 20);
			else PutString_SprFont(sX + iAdjX + 25, sY + iAdjY + 330 + 23, "Back", 0, 0, 7);
		}
		break;

	case 5: // Manuf: in progress
		sX = m_stDialogBoxInfo[26].sX;
		sY = m_stDialogBoxInfo[26].sY;
		iAdjX = -1;
		iAdjY = -7;

		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME7, sX, sY, 0);
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_TEXT2, sX, sY, 8);
		m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_pDispBuildItemList[m_stDialogBoxInfo[26].cStr[0]]->m_iSprH]->PutSpriteFast(sX + iAdjX + 62 + 5, sY + iAdjY + 84 + 17,
			m_pDispBuildItemList[m_stDialogBoxInfo[26].cStr[0]]->m_iSprFrame, dwTime);

		ZeroMemory(cTemp, sizeof(cTemp));
		GetItemName(m_pDispBuildItemList[m_stDialogBoxInfo[26].cStr[0]]->m_cName, 0, cStr1, cStr2, cStr3);
		wsprintf(cTemp, "%s", cStr1);
		PutString(sX + iAdjX + 44 + 10 + 60, sY + iAdjY + 55, cTemp, RGB(255, 255, 255));

		wsprintf(cTemp, DRAW_DIALOGBOX_SKILLDLG7 // "Skill level: %d/%d"
			, m_pDispBuildItemList[m_stDialogBoxInfo[26].cStr[0]]->m_iSkillLimit, m_pDispBuildItemList[m_stDialogBoxInfo[26].cStr[0]]->m_iMaxSkill);//"��ų ����: %d/%d"
		PutString(sX + iAdjX + 44 + 10 + 60, sY + iAdjY + 55 + 2 * 15, cTemp, RGB(45, 25, 25));
		PutString(sX + iAdjX + 44 + 10 + 60, sY + iAdjY + 55 + 3 * 15 + 5, DRAW_DIALOGBOX_SKILLDLG8, RGB(45, 25, 25));//"Ingredients Needed:"

		iLoc = 4;
		if (m_pDispBuildItemList[m_stDialogBoxInfo[26].cStr[0]]->m_iElementCount[1] != 0)
		{
			GetItemName(m_pDispBuildItemList[m_stDialogBoxInfo[26].cStr[0]]->m_cElementName1, 0, cStr1, cStr2, cStr3);
			wsprintf(cTemp, "%s", cStr1);
			if (m_pDispBuildItemList[m_stDialogBoxInfo[26].cStr[0]]->m_bElementFlag[1] == true)
				PutString(sX + iAdjX + 44 + 20 + 60, sY + iAdjY + 55 + iLoc * 15 + 5, cTemp, RGB(45, 25, 25));
			else PutString(sX + iAdjX + 44 + 20 + 60, sY + iAdjY + 55 + iLoc * 15 + 5, cTemp, RGB(120, 120, 120));
			iLoc++;
		}

		if (m_pDispBuildItemList[m_stDialogBoxInfo[26].cStr[0]]->m_iElementCount[2] != 0)
		{
			GetItemName(m_pDispBuildItemList[m_stDialogBoxInfo[26].cStr[0]]->m_cElementName2, 0, cStr1, cStr2, cStr3);
			wsprintf(cTemp, "%s", cStr1);
			if (m_pDispBuildItemList[m_stDialogBoxInfo[26].cStr[0]]->m_bElementFlag[2] == true)
				PutString(sX + iAdjX + 44 + 20 + 60, sY + iAdjY + 55 + iLoc * 15 + 5, cTemp, RGB(45, 25, 25));
			else PutString(sX + iAdjX + 44 + 20 + 60, sY + iAdjY + 55 + iLoc * 15 + 5, cTemp, RGB(120, 120, 120));
			iLoc++;
		}

		if (m_pDispBuildItemList[m_stDialogBoxInfo[26].cStr[0]]->m_iElementCount[3] != 0)
		{
			GetItemName(m_pDispBuildItemList[m_stDialogBoxInfo[26].cStr[0]]->m_cElementName3, 0, cStr1, cStr2, cStr3);
			wsprintf(cTemp, "%s", cStr1);
			if (m_pDispBuildItemList[m_stDialogBoxInfo[26].cStr[0]]->m_bElementFlag[3] == true)
				PutString(sX + iAdjX + 44 + 20 + 60, sY + iAdjY + 55 + iLoc * 15 + 5, cTemp, RGB(45, 25, 25));
			else PutString(sX + iAdjX + 44 + 20 + 60, sY + iAdjY + 55 + iLoc * 15 + 5, cTemp, RGB(120, 120, 120));
			iLoc++;
		}

		if (m_pDispBuildItemList[m_stDialogBoxInfo[26].cStr[0]]->m_iElementCount[4] != 0)
		{
			GetItemName(m_pDispBuildItemList[m_stDialogBoxInfo[26].cStr[0]]->m_cElementName4, 0, cStr1, cStr2, cStr3);
			wsprintf(cTemp, "%s", cStr1);
			if (m_pDispBuildItemList[m_stDialogBoxInfo[26].cStr[0]]->m_bElementFlag[4] == true)
				PutString(sX + iAdjX + 44 + 20 + 60, sY + iAdjY + 55 + iLoc * 15 + 5, cTemp, RGB(45, 25, 25));
			else PutString(sX + iAdjX + 44 + 20 + 60, sY + iAdjY + 55 + iLoc * 15 + 5, cTemp, RGB(120, 120, 120));
			iLoc++;
		}

		if (m_pDispBuildItemList[m_stDialogBoxInfo[26].cStr[0]]->m_iElementCount[5] != 0)
		{
			GetItemName(m_pDispBuildItemList[m_stDialogBoxInfo[26].cStr[0]]->m_cElementName5, 0, cStr1, cStr2, cStr3);
			wsprintf(cTemp, "%s", cStr1);
			if (m_pDispBuildItemList[m_stDialogBoxInfo[26].cStr[0]]->m_bElementFlag[5] == true)
				PutString(sX + iAdjX + 44 + 20 + 60, sY + iAdjY + 55 + iLoc * 15 + 5, cTemp, RGB(45, 25, 25));
			else PutString(sX + iAdjX + 44 + 20 + 60, sY + iAdjY + 55 + iLoc * 15 + 5, cTemp, RGB(120, 120, 120));
			iLoc++;
		}

		if (m_pDispBuildItemList[m_stDialogBoxInfo[26].cStr[0]]->m_iElementCount[6] != 0)
		{
			GetItemName(m_pDispBuildItemList[m_stDialogBoxInfo[26].cStr[0]]->m_cElementName6, 0, cStr1, cStr2, cStr3);
			wsprintf(cTemp, "%s", cStr1);
			if (m_pDispBuildItemList[m_stDialogBoxInfo[26].cStr[0]]->m_bElementFlag[6] == true)
				PutString(sX + iAdjX + 44 + 20 + 60, sY + iAdjY + 55 + iLoc * 15 + 5, cTemp, RGB(45, 25, 25));
			else PutString(sX + iAdjX + 44 + 20 + 60, sY + iAdjY + 55 + iLoc * 15 + 5, cTemp, RGB(120, 120, 120));
			iLoc++;
		}

		m_pSprite[DEF_SPRID_INTERFACE_ADDINTERFACE]->PutSpriteFast(sX + iAdjX + 55 + 30 + 13, sY + iAdjY + 55 + 180, 2, dwTime);
		m_pSprite[DEF_SPRID_INTERFACE_ADDINTERFACE]->PutSpriteFast(sX + iAdjX + 55 + 30 + 45 * 1 + 13, sY + iAdjY + 55 + 180, 2, dwTime);
		m_pSprite[DEF_SPRID_INTERFACE_ADDINTERFACE]->PutSpriteFast(sX + iAdjX + 55 + 30 + 45 * 2 + 13, sY + iAdjY + 55 + 180, 2, dwTime);
		m_pSprite[DEF_SPRID_INTERFACE_ADDINTERFACE]->PutSpriteFast(sX + iAdjX + 55 + 30 + 13, sY + iAdjY + 100 + 180, 2, dwTime);
		m_pSprite[DEF_SPRID_INTERFACE_ADDINTERFACE]->PutSpriteFast(sX + iAdjX + 55 + 30 + 45 * 1 + 13, sY + iAdjY + 100 + 180, 2, dwTime);
		m_pSprite[DEF_SPRID_INTERFACE_ADDINTERFACE]->PutSpriteFast(sX + iAdjX + 55 + 30 + 45 * 2 + 13, sY + iAdjY + 100 + 180, 2, dwTime);

		if (m_stDialogBoxInfo[26].sV1 != -1)
			m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT +
			m_pItemList[m_stDialogBoxInfo[26].sV1]->m_sSprite]->PutSpriteFast(sX + iAdjX + 55 + 30 + 13, sY + iAdjY + 55 + 180,
			m_pItemList[m_stDialogBoxInfo[26].sV1]->m_sSpriteFrame, dwTime);

		if (m_stDialogBoxInfo[26].sV2 != -1)
			m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT +
			m_pItemList[m_stDialogBoxInfo[26].sV2]->m_sSprite]->PutSpriteFast(sX + iAdjX + 55 + 45 * 1 + 30 + 13, sY + iAdjY + 55 + 180,
			m_pItemList[m_stDialogBoxInfo[26].sV2]->m_sSpriteFrame, dwTime);

		if (m_stDialogBoxInfo[26].sV3 != -1)
			m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT +
			m_pItemList[m_stDialogBoxInfo[26].sV3]->m_sSprite]->PutSpriteFast(sX + iAdjX + 55 + 45 * 2 + 30 + 13, sY + iAdjY + 55 + 180,
			m_pItemList[m_stDialogBoxInfo[26].sV3]->m_sSpriteFrame, dwTime);

		if (m_stDialogBoxInfo[26].sV4 != -1)
			m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT +
			m_pItemList[m_stDialogBoxInfo[26].sV4]->m_sSprite]->PutSpriteFast(sX + iAdjX + 55 + 30 + 13, sY + iAdjY + 100 + 180,
			m_pItemList[m_stDialogBoxInfo[26].sV4]->m_sSpriteFrame, dwTime);

		if (m_stDialogBoxInfo[26].sV5 != -1)
			m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT +
			m_pItemList[m_stDialogBoxInfo[26].sV5]->m_sSprite]->PutSpriteFast(sX + iAdjX + 55 + 45 * 1 + 30 + 13, sY + iAdjY + 100 + 180,
			m_pItemList[m_stDialogBoxInfo[26].sV5]->m_sSpriteFrame, dwTime);

		if (m_stDialogBoxInfo[26].sV6 != -1)
			m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT +
			m_pItemList[m_stDialogBoxInfo[26].sV6]->m_sSprite]->PutSpriteFast(sX + iAdjX + 55 + 45 * 2 + 30 + 13, sY + iAdjY + 100 + 180,
			m_pItemList[m_stDialogBoxInfo[26].sV6]->m_sSpriteFrame, dwTime);

		PutString(sX + iAdjX + 33, sY + iAdjY + 230 + 75, DRAW_DIALOGBOX_SKILLDLG29, RGB(55, 25, 25));//" Manufacturing the items...."
		PutString(sX + iAdjX + 33, sY + iAdjY + 245 + 75, DRAW_DIALOGBOX_SKILLDLG30, RGB(55, 25, 25));//"Please wait until manufacture finishes."

		if ((dwTime - m_stDialogBoxInfo[26].dwT1) > 1000)
		{
			m_stDialogBoxInfo[26].dwT1 = dwTime;
			m_stDialogBoxInfo[26].cStr[1]++;
			if (m_stDialogBoxInfo[26].cStr[1] >= 7) m_stDialogBoxInfo[26].cStr[1] = 7;
		}

		if (m_stDialogBoxInfo[26].cStr[1] == 4)
		{
			bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_BUILDITEM, 0, 0, 0, 0, m_pDispBuildItemList[m_stDialogBoxInfo[26].cStr[0]]->m_cName);
			m_stDialogBoxInfo[26].cStr[1]++;
		}
		break;

	case 6: // Manuf: Done
		sX = m_stDialogBoxInfo[26].sX;
		sY = m_stDialogBoxInfo[26].sY;
		iAdjX = -1;
		iAdjY = -7;

		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME7, sX, sY, 0);
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_TEXT2, sX, sY, 8);
		m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT + m_pDispBuildItemList[m_stDialogBoxInfo[26].cStr[0]]->m_iSprH]->PutSpriteFast(sX + iAdjX + 62 + 5, sY + iAdjY + 84 + 17,
			m_pDispBuildItemList[m_stDialogBoxInfo[26].cStr[0]]->m_iSprFrame, dwTime);

		ZeroMemory(cTemp, sizeof(cTemp));
		GetItemName(m_pDispBuildItemList[m_stDialogBoxInfo[26].cStr[0]]->m_cName, 0, cStr1, cStr2, cStr3);

		wsprintf(cTemp, "%s", cStr1);
		PutString(sX + iAdjX + 44 + 10 + 60, sY + iAdjY + 55, cTemp, RGB(255, 255, 255));

		if (m_stDialogBoxInfo[26].cStr[2] == 1) {
			PutString(sX + iAdjX + 33 + 11, sY + iAdjY + 200 - 45, DRAW_DIALOGBOX_SKILLDLG31, RGB(55, 25, 25));//" Success in manufacture!"

			if (m_stDialogBoxInfo[26].sV1 == DEF_ITEMTYPE_MATERIAL) {
				wsprintf(G_cTxt, DRAW_DIALOGBOX_SKILLDLG32, m_stDialogBoxInfo[26].cStr[3]);//"The purity of product is %d%%."
				PutString(sX + iAdjX + 33 + 11, sY + iAdjY + 215 - 45, G_cTxt, RGB(55, 25, 25));
			}
			else {
				wsprintf(G_cTxt, DRAW_DIALOGBOX_SKILLDLG33, (int)m_stDialogBoxInfo[26].cStr[3] + 100);//"The completion of product is %d%%."
				PutString(sX + iAdjX + 33, sY + iAdjY + 215 - 45, G_cTxt, RGB(55, 25, 25));
			}
		}
		else {
			PutString(sX + iAdjX + 33 + 11, sY + iAdjY + 200, DRAW_DIALOGBOX_SKILLDLG34, RGB(55, 25, 25));//"Failed on manufacture."
		}

		if ((msX >= sX + iAdjX + 32) && (msX <= sX + iAdjX + 95) && (msY >= sY + iAdjY + 353) && (msY <= sY + iAdjY + 372))
			PutString_SprFont(sX + iAdjX + 35, sY + iAdjY + 330 + 23, "Back", 6, 6, 20);
		else PutString_SprFont(sX + iAdjX + 35, sY + iAdjY + 330 + 23, "Back", 0, 0, 7);
		break;


	case 7: // Crafting, wait for incredients
		if (m_stDialogBoxInfo[26].cStr[0] != 0)
		{
			sX = m_stDialogBoxInfo[26].sX + iAdjX + (m_stDialogBoxInfo[26].cStr[0] - (rand() % (m_stDialogBoxInfo[26].cStr[0] * 2)));
			sY = m_stDialogBoxInfo[26].sY + iAdjY + (m_stDialogBoxInfo[26].cStr[0] - (rand() % (m_stDialogBoxInfo[26].cStr[0] * 2)));
		}
		else
		{
			sX = m_stDialogBoxInfo[26].sX;
			sY = m_stDialogBoxInfo[26].sY;
		}
		m_pSprite[DEF_SPRID_INTERFACE_CRAFTING]->PutSpriteFast(sX, sY, 0, dwTime);

		if (m_stDialogBoxInfo[26].sV1 != -1)
			m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT +
			m_pItemList[m_stDialogBoxInfo[26].sV1]->m_sSprite]->PutSpriteFast(sX + iAdjX + 55 + (1 - (rand() % 3)), sY + iAdjY + 55 + (1 - (rand() % 3)),
			m_pItemList[m_stDialogBoxInfo[26].sV1]->m_sSpriteFrame, dwTime);

		if (m_stDialogBoxInfo[26].sV2 != -1)
			m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT +
			m_pItemList[m_stDialogBoxInfo[26].sV2]->m_sSprite]->PutSpriteFast(sX + iAdjX + 65 + 45 * 1 + (1 - (rand() % 3)), sY + iAdjY + 40 + (1 - (rand() % 3)),
			m_pItemList[m_stDialogBoxInfo[26].sV2]->m_sSpriteFrame, dwTime);

		if (m_stDialogBoxInfo[26].sV3 != -1)
			m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT +
			m_pItemList[m_stDialogBoxInfo[26].sV3]->m_sSprite]->PutSpriteFast(sX + iAdjX + 65 + 45 * 2 + (1 - (rand() % 3)), sY + iAdjY + 55 + (1 - (rand() % 3)),
			m_pItemList[m_stDialogBoxInfo[26].sV3]->m_sSpriteFrame, dwTime);

		if (m_stDialogBoxInfo[26].sV4 != -1)
			m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT +
			m_pItemList[m_stDialogBoxInfo[26].sV4]->m_sSprite]->PutSpriteFast(sX + iAdjX + 65 + (1 - (rand() % 3)), sY + iAdjY + 100 + (1 - (rand() % 3)),
			m_pItemList[m_stDialogBoxInfo[26].sV4]->m_sSpriteFrame, dwTime);

		if (m_stDialogBoxInfo[26].sV5 != -1)
			m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT +
			m_pItemList[m_stDialogBoxInfo[26].sV5]->m_sSprite]->PutSpriteFast(sX + iAdjX + 65 + 45 * 1 + (1 - (rand() % 3)), sY + iAdjY + 115 + (1 - (rand() % 3)),
			m_pItemList[m_stDialogBoxInfo[26].sV5]->m_sSpriteFrame, dwTime);

		if (m_stDialogBoxInfo[26].sV6 != -1)
			m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT +
			m_pItemList[m_stDialogBoxInfo[26].sV6]->m_sSprite]->PutSpriteFast(sX + iAdjX + 75 + 45 * 2 + (1 - (rand() % 3)), sY + iAdjY + 100 + (1 - (rand() % 3)),
			m_pItemList[m_stDialogBoxInfo[26].sV6]->m_sSpriteFrame, dwTime);
		if ((msX >= sX + iAdjX + 60) && (msX <= sX + iAdjX + 153) && (msY >= sY + iAdjY + 175) && (msY <= sY + iAdjY + 195))
			PutString_SprFont(sX + iAdjX + 60, sY + iAdjY + 175, "Try Now!", 16, 16, 30);
		else PutString_SprFont(sX + iAdjX + 60, sY + iAdjY + 175, "Try Now!", 6, 6, 20);
		break;

	case 8: // Crafting in progress
		if (m_stDialogBoxInfo[26].cStr[0] != 0)
		{
			sX = m_stDialogBoxInfo[26].sX + iAdjX + (m_stDialogBoxInfo[26].cStr[0] - (rand() % (m_stDialogBoxInfo[26].cStr[0] * 2)));
			sY = m_stDialogBoxInfo[26].sY + iAdjY + (m_stDialogBoxInfo[26].cStr[0] - (rand() % (m_stDialogBoxInfo[26].cStr[0] * 2)));
		}
		else
		{
			sX = m_stDialogBoxInfo[26].sX;
			sY = m_stDialogBoxInfo[26].sY;
		}
		m_pSprite[DEF_SPRID_INTERFACE_CRAFTING]->PutSpriteFast(sX, sY, 0, dwTime);

		if (m_stDialogBoxInfo[26].sV1 != -1)
		{
			m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT +
				m_pItemList[m_stDialogBoxInfo[26].sV1]->m_sSprite]->PutSpriteFast(sX + iAdjX + 55 + (1 - (rand() % 3)) + (1 - (rand() % 3)), sY + iAdjY + 55 + (1 - (rand() % 3)),
				m_pItemList[m_stDialogBoxInfo[26].sV1]->m_sSpriteFrame, dwTime);
			if ((m_pItemList[m_stDialogBoxInfo[26].sV1]->m_cItemType == DEF_ITEMTYPE_EQUIP)
				&& (m_pItemList[m_stDialogBoxInfo[26].sV1]->m_cEquipPos == DEF_EQUIPPOS_NECK))
				m_iContributionPrice = 10;
		}
		if (m_stDialogBoxInfo[26].sV2 != -1)
		{
			m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT +
				m_pItemList[m_stDialogBoxInfo[26].sV2]->m_sSprite]->PutSpriteFast(sX + iAdjX + 65 + 45 * 1 + (1 - (rand() % 3)), sY + iAdjY + 40 + (1 - (rand() % 3)),
				m_pItemList[m_stDialogBoxInfo[26].sV2]->m_sSpriteFrame, dwTime);
			if ((m_pItemList[m_stDialogBoxInfo[26].sV2]->m_cItemType == DEF_ITEMTYPE_EQUIP)
				&& (m_pItemList[m_stDialogBoxInfo[26].sV2]->m_cEquipPos == DEF_EQUIPPOS_NECK))
				m_iContributionPrice = 10;
		}
		if (m_stDialogBoxInfo[26].sV3 != -1)
		{
			m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT +
				m_pItemList[m_stDialogBoxInfo[26].sV3]->m_sSprite]->PutSpriteFast(sX + iAdjX + 65 + 45 * 2 + (1 - (rand() % 3)), sY + iAdjY + 55 + (1 - (rand() % 3)),
				m_pItemList[m_stDialogBoxInfo[26].sV3]->m_sSpriteFrame, dwTime);
			if ((m_pItemList[m_stDialogBoxInfo[26].sV3]->m_cItemType == DEF_ITEMTYPE_EQUIP)
				&& (m_pItemList[m_stDialogBoxInfo[26].sV3]->m_cEquipPos == DEF_EQUIPPOS_NECK))
				m_iContributionPrice = 10;
		}
		if (m_stDialogBoxInfo[26].sV4 != -1)
		{
			m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT +
				m_pItemList[m_stDialogBoxInfo[26].sV4]->m_sSprite]->PutSpriteFast(sX + iAdjX + 65 + (1 - (rand() % 3)), sY + iAdjY + 100 + (1 - (rand() % 3)),
				m_pItemList[m_stDialogBoxInfo[26].sV4]->m_sSpriteFrame, dwTime);
			if ((m_pItemList[m_stDialogBoxInfo[26].sV4]->m_cItemType == DEF_ITEMTYPE_EQUIP)
				&& (m_pItemList[m_stDialogBoxInfo[26].sV4]->m_cEquipPos == DEF_EQUIPPOS_NECK))
				m_iContributionPrice = 10;
		}
		if (m_stDialogBoxInfo[26].sV5 != -1)
		{
			m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT +
				m_pItemList[m_stDialogBoxInfo[26].sV5]->m_sSprite]->PutSpriteFast(sX + iAdjX + 65 + 45 * 1 + (1 - (rand() % 3)), sY + iAdjY + 115 + (1 - (rand() % 3)),
				m_pItemList[m_stDialogBoxInfo[26].sV5]->m_sSpriteFrame, dwTime);
			if ((m_pItemList[m_stDialogBoxInfo[26].sV5]->m_cItemType == DEF_ITEMTYPE_EQUIP)
				&& (m_pItemList[m_stDialogBoxInfo[26].sV5]->m_cEquipPos == DEF_EQUIPPOS_NECK))
				m_iContributionPrice = 10;
		}
		if (m_stDialogBoxInfo[26].sV6 != -1)
		{
			m_pSprite[DEF_SPRID_ITEMPACK_PIVOTPOINT +
				m_pItemList[m_stDialogBoxInfo[26].sV6]->m_sSprite]->PutSpriteFast(sX + iAdjX + 75 + 45 * 2 + (1 - (rand() % 3)), sY + iAdjY + 100 + (1 - (rand() % 3)),
				m_pItemList[m_stDialogBoxInfo[26].sV6]->m_sSpriteFrame, dwTime);
			if ((m_pItemList[m_stDialogBoxInfo[26].sV6]->m_cItemType == DEF_ITEMTYPE_EQUIP)
				&& (m_pItemList[m_stDialogBoxInfo[26].sV6]->m_cEquipPos == DEF_EQUIPPOS_NECK))
				m_iContributionPrice = 10;
		}
		PutString_SprFont(sX + iAdjX + 60, sY + iAdjY + 175, "Creating...", 20, 6, 6);

		if ((dwTime - m_stDialogBoxInfo[26].dwT1) > 1000)
		{
			m_stDialogBoxInfo[26].dwT1 = dwTime;
			m_stDialogBoxInfo[26].cStr[1]++;
		}
		if (m_stDialogBoxInfo[26].cStr[1] >= 5)//m_pDispCraftItemList
		{
			bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_CRAFTITEM, 0, 0, 0, 0, 0);
			DisableDialogBox(26);
			PlaySound('E', 42, 0);
		}
		break;
	}
}

void CGame::DrawDialogBox_SysMenu(short msX, short msY, char cLB)
{
	short sX, sY;

	sX = m_stDialogBoxInfo[19].sX;
	sY = m_stDialogBoxInfo[19].sY;
	DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME5, sX, sY, 0);
	DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_TEXT2, sX, sY, 6);

	PutString(sX + 23, sY + 63, DRAW_DIALOGBOX_SYSMENU_DETAILLEVEL, RGB(255, 255, 255));
	if (m_cDetailLevel == 0)
		PutString(sX + 121, sY + 63, DRAW_DIALOGBOX_SYSMENU_LOW, RGB(255, 255, 0));
	else PutString(sX + 121, sY + 63, DRAW_DIALOGBOX_SYSMENU_LOW, RGB(45, 25, 25));
	if (m_cDetailLevel == 1)
		PutString(sX + 153, sY + 63, DRAW_DIALOGBOX_SYSMENU_NORMAL, RGB(255, 255, 0));
	else PutString(sX + 153, sY + 63, DRAW_DIALOGBOX_SYSMENU_NORMAL, RGB(45, 25, 25));
	if (m_cDetailLevel == 2)
		PutString(sX + 205, sY + 63, DRAW_DIALOGBOX_SYSMENU_HIGH, RGB(255, 255, 0));
	else PutString(sX + 205, sY + 63, DRAW_DIALOGBOX_SYSMENU_HIGH, RGB(45, 25, 25));

	PutString(sX + 23, sY + 84, DRAW_DIALOGBOX_SYSMENU_SOUND, RGB(255, 255, 255));
	
	if (m_bSoundFlag) {
		if (m_bSoundStat) PutString(sX + 85, sY + 85, DRAW_DIALOGBOX_SYSMENU_ON, RGB(0, 255, 0));
		else PutString(sX + 83, sY + 85, DRAW_DIALOGBOX_SYSMENU_OFF, RGB(255, 0, 0));
	}
	else PutString(sX + 68, sY + 85, DRAW_DIALOGBOX_SYSMENU_DISABLED, RGB(100, 100, 100));

	PutString(sX + 123, sY + 84, DRAW_DIALOGBOX_SYSMENU_MUSIC, RGB(255, 255, 255));
	
	if (m_bSoundFlag) {
		if (m_bMusicStat) PutString(sX + 180, sY + 85, DRAW_DIALOGBOX_SYSMENU_ON, RGB(0, 255, 0));
		else PutString(sX + 178, sY + 85, DRAW_DIALOGBOX_SYSMENU_OFF, RGB(255, 0, 0));
	}
	else PutString(sX + 163, sY + 85, DRAW_DIALOGBOX_SYSMENU_DISABLED, RGB(100, 100, 100));

	PutString(sX + 23, sY + 106, DRAW_DIALOGBOX_SYSMENU_WHISPER, RGB(255, 255, 255));
	
	if (m_bWhisper) PutString(sX + 85, sY + 106, DRAW_DIALOGBOX_SYSMENU_ON, RGB(0, 255, 0));
	else PutString(sX + 82, sY + 106, DRAW_DIALOGBOX_SYSMENU_OFF, RGB(255, 0, 0));

	PutString(sX + 123, sY + 106, DRAW_DIALOGBOX_SYSMENU_SHOUT, RGB(255, 255, 255));

	if (m_bShout) PutString(sX + 180, sY + 106, DRAW_DIALOGBOX_SYSMENU_ON, RGB(0, 255, 0));
	else PutString(sX + 177, sY + 106, DRAW_DIALOGBOX_SYSMENU_OFF, RGB(255, 0, 0));

	PutString(sX + 23, sY + 124, DRAW_DIALOGBOX_SYSMENU_SOUNDVOLUME, RGB(255, 255, 255));
	
	DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME6, sX + 130 + m_cSoundVolume, sY + 129, 8);

	PutString(sX + 23, sY + 141, DRAW_DIALOGBOX_SYSMENU_MUSICVOLUME, RGB(255, 255, 255));
	
	DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME6, sX + 130 + m_cMusicVolume, sY + 145, 8);

	PutString(sX + 23, sY + 158, DRAW_DIALOGBOX_SYSMENU_TRANSPARENCY, RGB(255, 255, 255));
	
	if (m_bDialogTrans) PutString(sX + 208, sY + 158, DRAW_DIALOGBOX_SYSMENU_ON, RGB(0, 255, 0));
	else PutString(sX + 207, sY + 158, DRAW_DIALOGBOX_SYSMENU_OFF, RGB(255, 0, 0));

	PutString(sX + 23, sY + 180, DRAW_DIALOGBOX_SYSMENU_GUIDEMAP, RGB(255, 255, 255));
	
	if (m_bIsDialogEnabled[9]) PutString(sX + 99, sY + 180, DRAW_DIALOGBOX_SYSMENU_ON, RGB(0, 255, 0));
	else PutString(sX + 98, sY + 180, DRAW_DIALOGBOX_SYSMENU_OFF, RGB(255, 0, 0));

	/*SYSTEMTIME SysTime;
	GetLocalTime(&SysTime);
	ZeroMemory(G_cTxt, sizeof(G_cTxt));
	wsprintf(G_cTxt, "Local Time: %.2d:%.2d:%.2d", SysTime.wHour, SysTime.wMinute, SysTime.wSecond);
	PutString(sX + 23, sY + 204, G_cTxt, RGB(4, 0, 50));*/
	
	ZeroMemory(G_cTxt, sizeof(G_cTxt));
	wsprintf(G_cTxt, "Time: %.2d:%.2d", srvHour, srvMinute);
	PutString(sX + 23, sY + 204, G_cTxt, RGB(4, 0, 50));

	ZeroMemory(G_cTxt, sizeof(G_cTxt));
	wsprintf(G_cTxt, "Ping: %.3d ms", m_iPing);
	PutString(sX + 153, sY + 204, G_cTxt, RGB(255, 255, 255));

	//Grid - by luqah
	PutString(sX + 136, sY + 180, "Grid", RGB(255, 255, 255));
	
	if (m_bGrid) PutString(sX + 3 + 120 + 88, sY + 180, DRAW_DIALOGBOX_SYSMENU_ON, RGB(0, 255, 0));
	else PutString(sX + 3 + 120 + 87, sY + 180, DRAW_DIALOGBOX_SYSMENU_OFF, RGB(255, 0, 0));


	if ((cLB != 0) && (iGetTopDialogBoxIndex() == 19))
	{
		if ((msX >= sX + 127) && (msX <= sX + 238) && (msY >= sY + 122) && (msY <= sY + 138))
		{
			m_cSoundVolume = msX - (sX + 127);
			if (m_cSoundVolume > 100) m_cSoundVolume = 100;
			if (m_cSoundVolume < 0) m_cSoundVolume = 0;
		}
		if ((msX >= sX + 127) && (msX <= sX + 238) && (msY >= sY + 139) && (msY <= sY + 155))
		{
			m_cMusicVolume = msX - (sX + 127);
			if (m_cMusicVolume > 100) m_cMusicVolume = 100;
			if (m_cMusicVolume < 0) m_cMusicVolume = 0;
			if (m_bSoundFlag)
			{
				SetBGMVolume();// Snoopy: mp3 support
				int iVol;
				iVol = (m_cMusicVolume - 100) * 20;
				if (iVol > 0) iVol = 0;
				if (iVol < -10000) iVol = -10000;
				if (m_pBGM != 0)
				{
					m_pBGM->bStop(true);
					m_pBGM->Play(false, 0, iVol);
				}
			}
		}
	}
	else m_stDialogBoxInfo[19].bIsScrollSelected = false;

	// Log-Out
	if (m_cLogOutCount == -1)
	{
		if ((msX >= sX + DEF_LBTNPOSX) && (msX <= sX + DEF_LBTNPOSX + DEF_BTNSZX) && (msY >= sY + 225) && (msY <= sY + 225 + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + 225, 9);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + 225, 8);
	}
	else  //Continue
	{
		if ((msX >= sX + DEF_LBTNPOSX) && (msX <= sX + DEF_LBTNPOSX + DEF_BTNSZX) && (msY >= sY + 225) && (msY <= sY + 225 + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + 225, 7);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_LBTNPOSX, sY + 225, 6);
	}
	// Restart
	if ((m_iHP <= 0) && (m_cRestartCount == -1))
	{
		if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY >= sY + 225) && (msY <= sY + 225 + DEF_BTNSZY))
			DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + 225, 37);
		else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + DEF_RBTNPOSX, sY + 225, 36);
	}
}

//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

void CGame::bItemDrop_Character()
{
	ItemEquipHandler((char)m_stMCursor.sSelectedObjectID);
}

void CGame::bItemDrop_Inventory(short msX, short msY)
{
	short sX, sY, dX, dY;
	char  cTxt[120];
	if (m_cCommand < 0) return;
	if (m_pItemList[m_stMCursor.sSelectedObjectID] == 0) return;
	if ((m_bSkillUsingStatus == true) && (m_bIsItemEquipped[m_stMCursor.sSelectedObjectID] == true))
	{
		AddEventList(BITEMDROP_INVENTORY1, 10);
		return;
	}
	if (m_bIsItemDisabled[m_stMCursor.sSelectedObjectID] == true) return;

	if (strcmp(m_cMapName, "team") == 0)
	{
		if (m_bIsItemEquipped[m_stMCursor.sSelectedObjectID] == true && (m_pItemList[m_stMCursor.sSelectedObjectID]->m_cEquipPos == DEF_EQUIPPOS_BACK || m_pItemList[m_stMCursor.sSelectedObjectID]->m_cEquipPos == DEF_EQUIPPOS_BOOTS))
		{
			return;
		}
	}

	sY = m_stDialogBoxInfo[2].sY;
	sX = m_stDialogBoxInfo[2].sX;
	dX = msX - sX - 32 - m_stMCursor.sDistX;
	dY = msY - sY - 44 - m_stMCursor.sDistY;
	if (dY < -10) dY = -10;
	if (dX < 0)   dX = 0;
	if (dX > 170) dX = 170;
	if (dY > 95) dY = 95;

	m_pItemList[m_stMCursor.sSelectedObjectID]->m_sX = dX;
	m_pItemList[m_stMCursor.sSelectedObjectID]->m_sY = dY;

	short sTmpSpr, sTmpSprFrm;
	sTmpSpr = m_pItemList[m_stMCursor.sSelectedObjectID]->m_sSprite;
	sTmpSprFrm = m_pItemList[m_stMCursor.sSelectedObjectID]->m_sSpriteFrame;

	char cItemID;
	if (m_bShiftPressed)
	{
		for (int i = 0; i < DEF_MAXITEMS; i++)
		{
			if (m_cItemOrder[DEF_MAXITEMS - 1 - i] != -1)
			{
				cItemID = m_cItemOrder[DEF_MAXITEMS - 1 - i];
				if (m_pItemList[cItemID] != 0 && memcmp(m_pItemList[cItemID]->m_cName, m_pItemList[m_stMCursor.sSelectedObjectID]->m_cName, 20) == 0)
				{
					m_pItemList[cItemID]->m_sX = dX;
					m_pItemList[cItemID]->m_sY = dY;
					bSendCommand(MSGID_REQUEST_SETITEMPOS, 0, cItemID, dX, dY, 0, 0);
				}
			}
		}
	}
	else bSendCommand(MSGID_REQUEST_SETITEMPOS, 0, (char)(m_stMCursor.sSelectedObjectID), dX, dY, 0, 0);

	if (m_bIsItemEquipped[m_stMCursor.sSelectedObjectID] == true)
	{
		char cStr1[64], cStr2[64], cStr3[64], cStr4[64], cStr5[64], cStr6[64];
		GetItemName(m_pItemList[m_stMCursor.sSelectedObjectID], cStr1, cStr2, cStr3, cStr4, cStr5, cStr6);
		wsprintf(cTxt, ITEM_EQUIPMENT_RELEASED, cStr1);
		AddEventList(cTxt, 10);

		if (memcmp(m_pItemList[m_stMCursor.sSelectedObjectID]->m_cName, "AngelicPandent(STR)", 19) == 0) PlaySound('E', 53, 0);
		else if (memcmp(m_pItemList[m_stMCursor.sSelectedObjectID]->m_cName, "AngelicPandent(DEX)", 19) == 0) PlaySound('E', 53, 0);
		else if (memcmp(m_pItemList[m_stMCursor.sSelectedObjectID]->m_cName, "AngelicPandent(INT)", 19) == 0) PlaySound('E', 53, 0);
		else if (memcmp(m_pItemList[m_stMCursor.sSelectedObjectID]->m_cName, "AngelicPandent(MAG)", 19) == 0) PlaySound('E', 53, 0);
		else PlaySound('E', 29, 0);
		
		bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_RELEASEITEM, 0, m_stMCursor.sSelectedObjectID, 0, 0, 0);
		m_bIsItemEquipped[m_stMCursor.sSelectedObjectID] = false;
		m_sItemEquipmentStatus[m_pItemList[m_stMCursor.sSelectedObjectID]->m_cEquipPos] = -1;
	}
}

void CGame::bItemDrop_SellList(short msX, short msY)
{
	int i;
	char cItemID;

	cItemID = (char)m_stMCursor.sSelectedObjectID;

	if (m_pItemList[cItemID] == 0) return;
	if (m_bIsItemDisabled[cItemID] == true) return;
	if (m_cCommand < 0) return;
	for (i = 0; i < DEF_MAXSELLLIST; i++)
		if (m_stSellItemList[i].iIndex == cItemID)
		{
			AddEventList(BITEMDROP_SELLLIST1, 10);
			return;
		}
	if (strcmp(m_pItemList[cItemID]->m_cName, "Gold") == 0) {
		AddEventList(BITEMDROP_SELLLIST2, 10);
		return;
	}
	if (m_pItemList[cItemID]->m_wCurLifeSpan == 0)
	{
		ZeroMemory(G_cTxt, sizeof(G_cTxt));
		char cStr1[64], cStr2[64], cStr3[64], cStr4[64], cStr5[64], cStr6[64];
		GetItemName(m_pItemList[cItemID], cStr1, cStr2, cStr3, cStr4, cStr5, cStr6);
		wsprintf(G_cTxt, NOTIFYMSG_CANNOT_SELL_ITEM2, cStr1);
		AddEventList(G_cTxt, 10);
		return;
	}

	if (((m_pItemList[cItemID]->m_cItemType == DEF_ITEMTYPE_CONSUME) || (m_pItemList[cItemID]->m_cItemType == DEF_ITEMTYPE_ARROW)) &&
		(m_pItemList[cItemID]->m_dwCount > 1))
	{
		m_stDialogBoxInfo[17].sX = msX - 140;
		m_stDialogBoxInfo[17].sY = msY - 70;
		if (m_stDialogBoxInfo[17].sY < 0) m_stDialogBoxInfo[17].sY = 0;
		m_stDialogBoxInfo[17].sV1 = m_sPlayerX + 1;
		m_stDialogBoxInfo[17].sV2 = m_sPlayerY + 1;
		m_stDialogBoxInfo[17].sV3 = 1001;
		m_stDialogBoxInfo[17].sV4 = cItemID;
		ZeroMemory(m_stDialogBoxInfo[17].cStr, sizeof(m_stDialogBoxInfo[17].cStr));
		EnableDialogBox(17, cItemID, m_pItemList[cItemID]->m_dwCount, 0);
		m_bIsItemDisabled[cItemID] = true;
	}
	else
	{
		for (i = 0; i < DEF_MAXSELLLIST; i++)
			if (m_stSellItemList[i].iIndex == -1)
			{
				m_stSellItemList[i].iIndex = cItemID;
				m_stSellItemList[i].iAmount = 1;
				m_bIsItemDisabled[cItemID] = true;
				return;
			}
		AddEventList(BITEMDROP_SELLLIST3, 10);
	}
}

void CGame::bItemDrop_ItemUpgrade()
{
	char cItemID;
	cItemID = (char)m_stMCursor.sSelectedObjectID;
	if (m_bIsItemDisabled[cItemID] == true) return;
	if (m_cCommand < 0) return;
	if (m_pItemList[cItemID]->m_cEquipPos == DEF_EQUIPPOS_NONE) return;

	switch (m_stDialogBoxInfo[34].cMode) {
	case 1:
		m_bIsItemDisabled[m_stDialogBoxInfo[34].sV1] = false;
		m_stDialogBoxInfo[34].sV1 = cItemID;
		m_bIsItemDisabled[cItemID] = true;
		PlaySound('E', 29, 0);
		break;

	case 6:
		m_bIsItemDisabled[m_stDialogBoxInfo[34].sV1] = false;
		m_stDialogBoxInfo[34].sV1 = cItemID;
		m_bIsItemDisabled[cItemID] = true;
		PlaySound('E', 29, 0);
		break;
	}
}

void CGame::bItemDrop_Enchanting()
{
	char cItemID;
	cItemID = (char)m_stMCursor.sSelectedObjectID;
	if (m_bIsItemDisabled[cItemID] == true) return;
	if (m_cCommand < 0) return;
	if (m_pItemList[cItemID]->m_cEquipPos == DEF_EQUIPPOS_NONE) return;

	switch (m_stDialogBoxInfo[44].cMode) {
	case 1:
		m_bIsItemDisabled[m_stDialogBoxInfo[44].sV1] = false;
		m_stDialogBoxInfo[44].sV1 = cItemID;
		m_bIsItemDisabled[cItemID] = true;
		PlaySound('E', 29, 0);
		break;
	}
}

void CGame::bItemDrop_Bank(short msX, short msY)
{
	m_stDialogBoxInfo[39].sV1 = m_stMCursor.sSelectedObjectID;
	if (m_cCommand < 0) return;
	if (m_pItemList[m_stDialogBoxInfo[39].sV1] == 0) return;
	if (m_bIsItemDisabled[m_stDialogBoxInfo[39].sV1] == true) return;
	if (m_bIsDialogEnabled[17] == true)
	{
		AddEventList(BITEMDROP_SKILLDIALOG1, 10);
		return;
	}
	if ((m_bIsDialogEnabled[20] == true) && ((m_stDialogBoxInfo[20].cMode == 1) || (m_stDialogBoxInfo[20].cMode == 2)))
	{
		AddEventList(BITEMDROP_SKILLDIALOG1, 10);
		return;
	}
	if (m_bIsDialogEnabled[23] == true)
	{
		AddEventList(BITEMDROP_SKILLDIALOG1, 10);
		return;
	}
	if (m_bIsDialogEnabled[4] == true)
	{
		AddEventList(BITEMDROP_SKILLDIALOG1, 10);
		return;
	}
	if (((m_pItemList[m_stDialogBoxInfo[39].sV1]->m_cItemType == DEF_ITEMTYPE_CONSUME) || (m_pItemList[m_stDialogBoxInfo[39].sV1]->m_cItemType == DEF_ITEMTYPE_ARROW)) && (m_pItemList[m_stDialogBoxInfo[39].sV1]->m_dwCount > 1))
	{
		m_stDialogBoxInfo[17].sX = msX - 140;
		m_stDialogBoxInfo[17].sY = msY - 70;
		if (m_stDialogBoxInfo[17].sY < 0) m_stDialogBoxInfo[17].sY = 0;

		m_stDialogBoxInfo[17].sV1 = m_sPlayerX + 1;
		m_stDialogBoxInfo[17].sV2 = m_sPlayerY + 1;
		m_stDialogBoxInfo[17].sV3 = 1002;// NPC
		m_stDialogBoxInfo[17].sV4 = m_stDialogBoxInfo[39].sV1;

		ZeroMemory(m_stDialogBoxInfo[17].cStr, sizeof(m_stDialogBoxInfo[17].cStr));
		EnableDialogBox(17, m_stDialogBoxInfo[39].sV1, m_pItemList[m_stDialogBoxInfo[39].sV1]->m_dwCount, 0);
	}
	else
	{
		if (_iGetBankItemCount() >= (DEF_MAXBANKITEMS)) AddEventList(DLGBOX_CLICK_NPCACTION_QUERY9, 10);
		else bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_GIVEITEMTOCHAR, m_stDialogBoxInfo[39].sV1, 1, m_stDialogBoxInfo[39].sV5, m_stDialogBoxInfo[39].sV6, m_pItemList[m_stDialogBoxInfo[39].sV1]->m_cName, m_stDialogBoxInfo[39].sV4); //v1.4
	}
}

void CGame::bItemDrop_SkillDialog()
{
	int iConsumeNum;
	char cItemID;

	if (m_cCommand < 0) return;
	cItemID = (char)m_stMCursor.sSelectedObjectID;
	if (m_pItemList[cItemID] == 0) return;
	if (m_bIsItemDisabled[cItemID] == true) return;

	if (m_bIsDialogEnabled[17] == true)
	{
		AddEventList(BITEMDROP_SKILLDIALOG1, 10);
		return;
	}

	if ((m_bIsDialogEnabled[20] == true)
		&& ((m_stDialogBoxInfo[20].cMode == 1) || (m_stDialogBoxInfo[20].cMode == 2)))
	{
		AddEventList(BITEMDROP_SKILLDIALOG1, 10);
		return;
	}

	if (m_bIsDialogEnabled[23] == true)
	{
		AddEventList(BITEMDROP_SKILLDIALOG1, 10);
		return;
	}

	switch (m_stDialogBoxInfo[26].cMode) {
	case 1:
		if (m_pItemList[cItemID]->m_cItemType == DEF_ITEMTYPE_CONSUME)
		{
			iConsumeNum = 0;
			if (m_stDialogBoxInfo[26].sV1 == cItemID) iConsumeNum++;
			if (m_stDialogBoxInfo[26].sV2 == cItemID) iConsumeNum++;
			if (m_stDialogBoxInfo[26].sV3 == cItemID) iConsumeNum++;
			if (m_stDialogBoxInfo[26].sV4 == cItemID) iConsumeNum++;
			if (m_stDialogBoxInfo[26].sV5 == cItemID) iConsumeNum++;
			if (m_stDialogBoxInfo[26].sV6 == cItemID) iConsumeNum++;
			if (iConsumeNum >= (int)(m_pItemList[cItemID]->m_dwCount)) return;
		}
		if ((m_pItemList[cItemID]->m_cItemType == DEF_ITEMTYPE_EAT) ||
			(m_pItemList[cItemID]->m_cItemType == DEF_ITEMTYPE_CONSUME) ||
			(m_pItemList[cItemID]->m_cItemType == DEF_ITEMTYPE_NONE))
		{
		}
		else return;

		if (m_stDialogBoxInfo[26].sV1 == -1)
		{
			m_stDialogBoxInfo[26].sV1 = cItemID;
			if ((m_pItemList[cItemID]->m_cItemType == DEF_ITEMTYPE_CONSUME) && (m_pItemList[cItemID]->m_dwCount > 1))
			{
			}
			else m_bIsItemDisabled[cItemID] = true;
			return;
		}
		else if (m_stDialogBoxInfo[26].sV2 == -1)
		{
			m_stDialogBoxInfo[26].sV2 = cItemID;
			if ((m_pItemList[cItemID]->m_cItemType == DEF_ITEMTYPE_CONSUME) && (m_pItemList[cItemID]->m_dwCount > 1))
			{
			}
			else m_bIsItemDisabled[cItemID] = true;
			return;
		}
		else if (m_stDialogBoxInfo[26].sV3 == -1)
		{
			m_stDialogBoxInfo[26].sV3 = cItemID;
			if ((m_pItemList[cItemID]->m_cItemType == DEF_ITEMTYPE_CONSUME) && (m_pItemList[cItemID]->m_dwCount > 1))
			{
			}
			else m_bIsItemDisabled[cItemID] = true;
			return;
		}
		else if (m_stDialogBoxInfo[26].sV4 == -1)
		{
			m_stDialogBoxInfo[26].sV4 = cItemID;
			if ((m_pItemList[cItemID]->m_cItemType == DEF_ITEMTYPE_CONSUME) && (m_pItemList[cItemID]->m_dwCount > 1))
			{
			}
			else m_bIsItemDisabled[cItemID] = true;
			return;
		}
		else if (m_stDialogBoxInfo[26].sV5 == -1)
		{
			m_stDialogBoxInfo[26].sV5 = cItemID;
			if ((m_pItemList[cItemID]->m_cItemType == DEF_ITEMTYPE_CONSUME) && (m_pItemList[cItemID]->m_dwCount > 1))
			{
			}
			else m_bIsItemDisabled[cItemID] = true;
			return;
		}
		else if (m_stDialogBoxInfo[26].sV6 == -1)
		{
			m_stDialogBoxInfo[26].sV6 = cItemID;
			if ((m_pItemList[cItemID]->m_cItemType == DEF_ITEMTYPE_CONSUME) && (m_pItemList[cItemID]->m_dwCount > 1))
			{
			}
			else m_bIsItemDisabled[cItemID] = true;
			return;
		}
		AddEventList(BITEMDROP_SKILLDIALOG4, 10);
		break;

	case 4:
		if (m_pItemList[cItemID]->m_cItemType == DEF_ITEMTYPE_CONSUME)
		{
			iConsumeNum = 0;
			if (m_stDialogBoxInfo[26].sV1 == cItemID) iConsumeNum++;
			if (m_stDialogBoxInfo[26].sV2 == cItemID) iConsumeNum++;
			if (m_stDialogBoxInfo[26].sV3 == cItemID) iConsumeNum++;
			if (m_stDialogBoxInfo[26].sV4 == cItemID) iConsumeNum++;
			if (m_stDialogBoxInfo[26].sV5 == cItemID) iConsumeNum++;
			if (m_stDialogBoxInfo[26].sV6 == cItemID) iConsumeNum++;
			if (iConsumeNum >= (int)(m_pItemList[cItemID]->m_dwCount)) return;
		}

		if (m_stDialogBoxInfo[26].sV1 == -1)
		{
			m_stDialogBoxInfo[26].sV1 = cItemID;
			m_stDialogBoxInfo[26].cStr[4] = (char)_bCheckCurrentBuildItemStatus();
			if ((m_pItemList[cItemID]->m_cItemType == DEF_ITEMTYPE_CONSUME) && (m_pItemList[cItemID]->m_dwCount > 1))
			{
			}
			else m_bIsItemDisabled[cItemID] = true;
			return;
		}
		else if (m_stDialogBoxInfo[26].sV2 == -1)
		{
			m_stDialogBoxInfo[26].sV2 = cItemID;
			m_stDialogBoxInfo[26].cStr[4] = (char)_bCheckCurrentBuildItemStatus();
			if ((m_pItemList[cItemID]->m_cItemType == DEF_ITEMTYPE_CONSUME) && (m_pItemList[cItemID]->m_dwCount > 1))
			{
			}
			else m_bIsItemDisabled[cItemID] = true;
			return;
		}
		else if (m_stDialogBoxInfo[26].sV3 == -1)
		{
			m_stDialogBoxInfo[26].sV3 = cItemID;
			m_stDialogBoxInfo[26].cStr[4] = (char)_bCheckCurrentBuildItemStatus();
			if ((m_pItemList[cItemID]->m_cItemType == DEF_ITEMTYPE_CONSUME) && (m_pItemList[cItemID]->m_dwCount > 1))
			{
			}
			else m_bIsItemDisabled[cItemID] = true;
			return;
		}
		else if (m_stDialogBoxInfo[26].sV4 == -1)
		{
			m_stDialogBoxInfo[26].sV4 = cItemID;
			m_stDialogBoxInfo[26].cStr[4] = (char)_bCheckCurrentBuildItemStatus();
			if ((m_pItemList[cItemID]->m_cItemType == DEF_ITEMTYPE_CONSUME) && (m_pItemList[cItemID]->m_dwCount > 1))
			{
			}
			else m_bIsItemDisabled[cItemID] = true;
			return;
		}
		else if (m_stDialogBoxInfo[26].sV5 == -1)
		{
			m_stDialogBoxInfo[26].sV5 = cItemID;
			m_stDialogBoxInfo[26].cStr[4] = (char)_bCheckCurrentBuildItemStatus();
			if ((m_pItemList[cItemID]->m_cItemType == DEF_ITEMTYPE_CONSUME) && (m_pItemList[cItemID]->m_dwCount > 1))
			{
			}
			else m_bIsItemDisabled[cItemID] = true;
			return;
		}
		else if (m_stDialogBoxInfo[26].sV6 == -1)
		{
			m_stDialogBoxInfo[26].sV6 = cItemID;
			m_stDialogBoxInfo[26].cStr[4] = (char)_bCheckCurrentBuildItemStatus();
			if ((m_pItemList[cItemID]->m_cItemType == DEF_ITEMTYPE_CONSUME) && (m_pItemList[cItemID]->m_dwCount > 1))
			{
			}
			else m_bIsItemDisabled[cItemID] = true;
			return;
		}
		AddEventList(BITEMDROP_SKILLDIALOG4, 10); // "There is no more space for ingredients."
		break;

		// Crafting
	case 7:
		if ((m_pItemList[cItemID]->m_cItemType == DEF_ITEMTYPE_NONE)			// Merien Stone
			|| (m_pItemList[cItemID]->m_cItemType == DEF_ITEMTYPE_EQUIP)		// Necks XXXMagins
			|| (m_pItemList[cItemID]->m_cItemType == DEF_ITEMTYPE_CONSUME)		// stones
			|| (m_pItemList[cItemID]->m_cItemType == DEF_ITEMTYPE_MATERIAL))	// XXXwares
		{
		}
		else return;

		if (m_stDialogBoxInfo[26].sV1 == -1)
		{
			m_stDialogBoxInfo[26].sV1 = cItemID;
			if ((m_pItemList[cItemID]->m_cItemType == DEF_ITEMTYPE_CONSUME) && (m_pItemList[cItemID]->m_dwCount > 1))
			{
			}
			else m_bIsItemDisabled[cItemID] = true;
			return;
		}
		else if (m_stDialogBoxInfo[26].sV2 == -1)
		{
			m_stDialogBoxInfo[26].sV2 = cItemID;
			if ((m_pItemList[cItemID]->m_cItemType == DEF_ITEMTYPE_CONSUME) && (m_pItemList[cItemID]->m_dwCount > 1))
			{
			}
			else m_bIsItemDisabled[cItemID] = true;
			return;
		}
		else if (m_stDialogBoxInfo[26].sV3 == -1)
		{
			m_stDialogBoxInfo[26].sV3 = cItemID;
			if ((m_pItemList[cItemID]->m_cItemType == DEF_ITEMTYPE_CONSUME) && (m_pItemList[cItemID]->m_dwCount > 1))
			{
			}
			else m_bIsItemDisabled[cItemID] = true;
			return;
		}
		else if (m_stDialogBoxInfo[26].sV4 == -1)
		{
			m_stDialogBoxInfo[26].sV4 = cItemID;
			if ((m_pItemList[cItemID]->m_cItemType == DEF_ITEMTYPE_CONSUME) && (m_pItemList[cItemID]->m_dwCount > 1))
			{
			}
			else m_bIsItemDisabled[cItemID] = true;
			return;
		}
		else if (m_stDialogBoxInfo[26].sV5 == -1)
		{
			m_stDialogBoxInfo[26].sV5 = cItemID;
			if ((m_pItemList[cItemID]->m_cItemType == DEF_ITEMTYPE_CONSUME) && (m_pItemList[cItemID]->m_dwCount > 1))
			{
			}
			else m_bIsItemDisabled[cItemID] = true;
			return;
		}
		else if (m_stDialogBoxInfo[26].sV6 == -1)
		{
			m_stDialogBoxInfo[26].sV6 = cItemID;
			if ((m_pItemList[cItemID]->m_cItemType == DEF_ITEMTYPE_CONSUME) && (m_pItemList[cItemID]->m_dwCount > 1))
			{
			}
			else m_bIsItemDisabled[cItemID] = true;
			return;
		}
		AddEventList(BITEMDROP_SKILLDIALOG4, 10); // "There is no more space for ingredients."
		break;

	default:
		break;
	}
}

// Slates Item Drag&Drop - Diuuude
void CGame::bItemDrop_Slates()
{
	char cItemID;
	if (m_cCommand < 0) return;
	cItemID = (char)m_stMCursor.sSelectedObjectID;
	if (m_pItemList[cItemID] == 0) return;
	if (m_bIsItemDisabled[cItemID] == true) return;
	if (m_bIsDialogEnabled[17] == true) {
		AddEventList(BITEMDROP_SKILLDIALOG1, 10);
		return;
	}

	if ((m_bIsDialogEnabled[20] == true) &&
		((m_stDialogBoxInfo[20].cMode == 1) || (m_stDialogBoxInfo[20].cMode == 2))) {
		AddEventList(BITEMDROP_SKILLDIALOG1, 10);
		return;
	}

	if (m_bIsDialogEnabled[23] == true) {
		AddEventList(BITEMDROP_SKILLDIALOG1, 10);
		return;
	}

	switch (m_stDialogBoxInfo[40].cMode) {
	case 1:
		if ((m_pItemList[cItemID]->m_cItemType == DEF_ITEMTYPE_USE_SKILL_ENABLEDIALOGBOX) && (m_pItemList[cItemID]->m_sSpriteFrame >= 151) && (m_pItemList[cItemID]->m_sSpriteFrame <= 154)) {
			char cItemIDText[20];
			switch (m_pItemList[cItemID]->m_sSpriteFrame){
			case 151:
				if (m_stDialogBoxInfo[40].sV1 == -1){
					m_bIsItemDisabled[cItemID] = true;
					m_stDialogBoxInfo[40].sV1 = cItemID;
					wsprintf(cItemIDText, "Item ID : %d", cItemID);
					AddEventList(cItemIDText, 10);
				}
				break;
			case 152:
				if (m_stDialogBoxInfo[40].sV2 == -1){
					m_bIsItemDisabled[cItemID] = true;
					m_stDialogBoxInfo[40].sV2 = cItemID;
					wsprintf(cItemIDText, "Item ID : %d", cItemID);
					AddEventList(cItemIDText, 10);
				}
				break;
			case 153:
				if (m_stDialogBoxInfo[40].sV3 == -1){
					m_bIsItemDisabled[cItemID] = true;
					m_stDialogBoxInfo[40].sV3 = cItemID;
					wsprintf(cItemIDText, "Item ID : %d", cItemID);
					AddEventList(cItemIDText, 10);
				}
				break;
			case 154:
				if (m_stDialogBoxInfo[40].sV4 == -1){
					m_bIsItemDisabled[cItemID] = true;
					m_stDialogBoxInfo[40].sV4 = cItemID;
					wsprintf(cItemIDText, "Item ID : %d", cItemID);
					AddEventList(cItemIDText, 10);
				}
				break;
			}
		}
		break;

	default:
		break;
	}
}

void CGame::DlgBoxClick_Bank(short msX, short msY)
{
	int i;
	short sX, sY;
	sX = m_stDialogBoxInfo[14].sX;
	sY = m_stDialogBoxInfo[14].sY;
	switch (m_stDialogBoxInfo[14].cMode)
	{
	case -1:
		break;
	case 0:
		for (i = 0; i < 12; i++)
		{
			if ((msX > sX + 30) && (msX < sX + 210) && (msY >= sY + 110 + i * 15) && (msY <= sY + 124 + i * 15))
			{
				if ((m_pBankList[i + (m_stDialogBoxInfo[14].sV14 * 12)] != 0) && (i < DEF_MAXBANKITEMS))
				{
					if (_iGetTotalItemNum() >= DEF_MAXITEMS)
					{
						AddEventList(DLGBOX_CLICK_BANK1, 10);
						return;
					}
					bSendCommand(MSGID_REQUEST_RETRIEVEITEM, 0, 0, (i + (m_stDialogBoxInfo[14].sV14 * 12)), 0, 0, 0);
					m_stDialogBoxInfo[14].cMode = -1;
					PlaySound('E', 14, 5);
				}
				return;
			}
		}
		break;
	}
	if ((msX >= sX + 16) && (msX <= sX + 38) && (msY >= sY + 300) && (msY <= sY + 300 + 15))
		m_stDialogBoxInfo[14].sV14 = 0;
	if ((msX >= sX + 39) && (msX <= sX + 56) && (msY >= sY + 300) && (msY <= sY + 300 + 15))
		m_stDialogBoxInfo[14].sV14 = 1;
	if ((msX >= sX + 57) && (msX <= sX + 81) && (msY >= sY + 300) && (msY <= sY + 300 + 15))
		m_stDialogBoxInfo[14].sV14 = 2;
	if ((msX >= sX + 82) && (msX <= sX + 101) && (msY >= sY + 300) && (msY <= sY + 300 + 15))
		m_stDialogBoxInfo[14].sV14 = 3;
	if ((msX >= sX + 102) && (msX <= sX + 116) && (msY >= sY + 300) && (msY <= sY + 300 + 15))
		m_stDialogBoxInfo[14].sV14 = 4;
	if ((msX >= sX + 117) && (msX <= sX + 137) && (msY >= sY + 300) && (msY <= sY + 300 + 15))
		m_stDialogBoxInfo[14].sV14 = 5;
	if ((msX >= sX + 138) && (msX <= sX + 165) && (msY >= sY + 300) && (msY <= sY + 300 + 15))
		m_stDialogBoxInfo[14].sV14 = 6;
	if ((msX >= sX + 166) && (msX <= sX + 197) && (msY >= sY + 300) && (msY <= sY + 300 + 15))
		m_stDialogBoxInfo[14].sV14 = 7;
	if ((msX >= sX + 198) && (msX <= sX + 217) && (msY >= sY + 300) && (msY <= sY + 300 + 15))
		m_stDialogBoxInfo[14].sV14 = 8;
	if ((msX >= sX + 218) && (msX <= sX + 239) && (msY >= sY + 300) && (msY <= sY + 300 + 15))
		m_stDialogBoxInfo[14].sV14 = 9;
}
// centu
void CGame::DlgBoxClick_GuildBank(short msX, short msY)
{
	int i;
	short sX, sY;

	sX = m_stDialogBoxInfo[58].sX;
	sY = m_stDialogBoxInfo[58].sY;

	switch (m_stDialogBoxInfo[58].cMode) {
	case -1:
		break;

	case 0:
		for (i = 0; i < m_stDialogBoxInfo[58].sV1; i++)
			if ((msX > sX + 30) && (msX < sX + 210) && (msY >= sY + 110 + i * 15) && (msY <= sY + 124 + i * 15)) {
				if ((m_pBankList[m_stDialogBoxInfo[58].sView + i] != 0) && ((m_stDialogBoxInfo[58].sView + i) < 1000)) {
					if (_iGetTotalItemNum() >= 50) {
						AddEventList(DLGBOX_CLICK_BANK1, 10);
						return;
					}
					bSendCommand(MSGID_REQUEST_RETRIEVEITEM, 0, 0, (m_stDialogBoxInfo[58].sView + i), 0, 0, 0);
					m_stDialogBoxInfo[58].cMode = -1;
					PlaySound('E', 14, 5);
				}
				return;
			}

		break;
	}
}

void CGame::DlgBoxClick_Fish(short msX, short msY)
{
	short sX, sY;

	sX = m_stDialogBoxInfo[24].sX;
	sY = m_stDialogBoxInfo[24].sY;

	switch (m_stDialogBoxInfo[24].cMode) {
	case 0:
		if ((msX >= sX + 160) && (msX <= sX + 253) && (msY >= sY + 70) && (msY <= sY + 90)) {
			bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_REQ_GETFISHTHISTIME, 0, 0, 0, 0, 0);
			AddEventList(DLGBOX_CLICK_FISH1, 10);
			DisableDialogBox(24);

			PlaySound('E', 14, 5);
		}
		break;
	}
}

void CGame::DlgBoxClick_Magic(short msX, short msY)
{
	int i, iCPivot, iYloc;
	short sX, sY;

	sX = m_stDialogBoxInfo[3].sX;
	sY = m_stDialogBoxInfo[3].sY;
	iCPivot = m_stDialogBoxInfo[3].sView * 10;
	iYloc = 0;
	for (i = 0; i < 9; i++)
	{
		if ((m_cMagicMastery[iCPivot + i] != 0) && (m_pMagicCfgList[iCPivot + i] != 0))
		{
			if ((msX >= sX + 30) && (msX <= sX + 240) && (msY >= sY + 70 + iYloc) && (msY <= sY + 70 + 18 + iYloc))
			{
				UseMagic(iCPivot + i);
				PlaySound('E', 14, 5);
				return;
			}
			iYloc += 18;
		}
	}
	if ((msX >= sX + 16) && (msX <= sX + 38) && (msY >= sY + 240) && (msY <= sY + 268))
		m_stDialogBoxInfo[3].sView = 0;
	if ((msX >= sX + 39) && (msX <= sX + 56) && (msY >= sY + 240) && (msY <= sY + 268))
		m_stDialogBoxInfo[3].sView = 1;
	if ((msX >= sX + 57) && (msX <= sX + 81) && (msY >= sY + 240) && (msY <= sY + 268))
		m_stDialogBoxInfo[3].sView = 2;
	if ((msX >= sX + 82) && (msX <= sX + 101) && (msY >= sY + 240) && (msY <= sY + 268))
		m_stDialogBoxInfo[3].sView = 3;
	if ((msX >= sX + 102) && (msX <= sX + 116) && (msY >= sY + 240) && (msY <= sY + 268))
		m_stDialogBoxInfo[3].sView = 4;
	if ((msX >= sX + 117) && (msX <= sX + 137) && (msY >= sY + 240) && (msY <= sY + 268))
		m_stDialogBoxInfo[3].sView = 5;
	if ((msX >= sX + 138) && (msX <= sX + 165) && (msY >= sY + 240) && (msY <= sY + 268))
		m_stDialogBoxInfo[3].sView = 6;
	if ((msX >= sX + 166) && (msX <= sX + 197) && (msY >= sY + 240) && (msY <= sY + 268))
		m_stDialogBoxInfo[3].sView = 7;
	if ((msX >= sX + 198) && (msX <= sX + 217) && (msY >= sY + 240) && (msY <= sY + 268))
		m_stDialogBoxInfo[3].sView = 8;
	if ((msX >= sX + 218) && (msX <= sX + 239) && (msY >= sY + 240) && (msY <= sY + 268))
		m_stDialogBoxInfo[3].sView = 9;

	if ((msX >= sX + DEF_RBTNPOSX) && (msX <= sX + DEF_RBTNPOSX + DEF_BTNSZX) && (msY >= sY + 285) && (msY <= sY + 285 + DEF_BTNSZY))
	{
		if (m_cSkillMastery[12] == 0) AddEventList(BDLBBOX_DOUBLE_CLICK_INVENTORY16, 10);
		else
		{
			for (i = 0; i < DEF_MAXITEMS; i++)
				if ((m_pItemList[i] != 0) && (m_pItemList[i]->m_cItemType == DEF_ITEMTYPE_USE_SKILL_ENABLEDIALOGBOX) &&
					(m_pItemList[i]->m_sSpriteFrame == 55))
				{
					EnableDialogBox(26, 1, 0, 0, 0);
					AddEventList(BDLBBOX_DOUBLE_CLICK_INVENTORY10, 10);
					PlaySound('E', 14, 5);
					return;
				}
			AddEventList(BDLBBOX_DOUBLE_CLICK_INVENTORY15, 10);
		}
		PlaySound('E', 14, 5);
	}
}

void CGame::DlgBoxClick_NpcActionQuery(short msX, short msY)
{
	short sX, sY;
	int   absX, absY;
	if (m_bIsDialogEnabled[27] == true)
	{
		AddEventList(BITEMDROP_SKILLDIALOG1, 10);
		return;
	}
	sX = m_stDialogBoxInfo[20].sX;
	sY = m_stDialogBoxInfo[20].sY;

	switch (m_stDialogBoxInfo[20].cMode) {
	case 0: // Talk to npc
		if ((msX > sX + 25) && (msX < sX + 100) && (msY > sY + 55) && (msY < sY + 70))
		{
			EnableDialogBox(m_stDialogBoxInfo[20].sV1, m_stDialogBoxInfo[20].sV2, 0, 0);
			DisableDialogBox(20);
		}
		if ((m_bIsDialogEnabled[21] == false) && (msX > sX + 125) && (msX < sX + 180) && (msY > sY + 55) && (msY < sY + 70))
		{
			switch (m_stDialogBoxInfo[20].sV1) {
			case 7:	// Guild
				bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_TALKTONPC, 0, 1, 0, 0, 0);
				AddEventList(TALKING_TO_GUILDHALL_OFFICER, 10);
				break;
			case 11: // BS or Shop
				switch (m_stDialogBoxInfo[20].sV2) {
				case 1:
					bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_TALKTONPC, 0, 2, 0, 0, 0);
					AddEventList(TALKING_TO_SHOP_KEEPER, 10);
					break;
				case 2:
					bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_TALKTONPC, 0, 3, 0, 0, 0);
					AddEventList(TALKING_TO_BLACKSMITH_KEEPER, 10);
					break;
				}
				break;
			case 13: // CityHall officer
				//bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_TALKTONPC, 0, 4, 0, 0, 0);
				//Magn0S:: Quset List
				bSendCommand(DEF_MSGID_REQUEST_QUEST_LIST, DEF_MSGTYPE_CONFIRM, 0, 0, 0, 0, 0);
				AddEventList(TALKING_TO_CITYHALL_OFFICER, 10);
				break;
			case 14: // WH keeper
				bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_TALKTONPC, 0, 5, 0, 0, 0);
				AddEventList(TALKING_TO_WAREHOUSE_KEEPER, 10);
				break;
			case 16: // Magicmerchant
				bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_TALKTONPC, 0, 6, 0, 0, 0);
				AddEventList(TALKING_TO_MAGICIAN, 10);
				break;
			}
			DisableDialogBox(20);
		}
		break;

	case 1: // On other player
		if ((msX > sX + 25) && (msX < sX + 100) && (msY > sY + 55) && (msY < sY + 70))
		{
			absX = abs(m_stDialogBoxInfo[20].sV5 - m_sPlayerX);
			absY = abs(m_stDialogBoxInfo[20].sV6 - m_sPlayerY);
			if ((absX <= 4) && (absY <= 4))
				bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_GIVEITEMTOCHAR, m_stDialogBoxInfo[20].sV1, m_stDialogBoxInfo[20].sV3, m_stDialogBoxInfo[20].sV5, m_stDialogBoxInfo[20].sV6, m_pItemList[m_stDialogBoxInfo[20].sV1]->m_cName, m_stDialogBoxInfo[20].sV4); //v1.4
			else AddEventList(DLGBOX_CLICK_NPCACTION_QUERY7, 10); //"Too far to give the item."
			DisableDialogBox(20);
		}
		else if ((msX > sX + 155) && (msX < sX + 210) && (msY > sY + 55) && (msY < sY + 70))
		{
			absX = abs(m_stDialogBoxInfo[20].sV5 - m_sPlayerX);
			absY = abs(m_stDialogBoxInfo[20].sV6 - m_sPlayerY);
			if ((absX <= 4) && (absY <= 4))
				bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_EXCHANGEITEMTOCHAR, m_stDialogBoxInfo[20].sV1, m_stDialogBoxInfo[20].sV3, m_stDialogBoxInfo[20].sV5, m_stDialogBoxInfo[20].sV6, m_pItemList[m_stDialogBoxInfo[20].sV1]->m_cName, m_stDialogBoxInfo[20].sV4); //v1.4
			else AddEventList(DLGBOX_CLICK_NPCACTION_QUERY8, 10); //"Too far to exchange item."
			DisableDialogBox(20);
		}
		break;

	case 2: // Item on Shop/BS
		if ((msX > sX + 25) && (msX < sX + 100) && (msY > sY + 55) && (msY < sY + 70))
		{
			bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_REQ_SELLITEM, 0, m_stDialogBoxInfo[20].sV1, m_stDialogBoxInfo[20].sV2, m_stDialogBoxInfo[20].sV3, m_pItemList[m_stDialogBoxInfo[20].sV1]->m_cName, m_stDialogBoxInfo[20].sV4); // v1.4
			DisableDialogBox(20);
		}
		else if ((msX > sX + 125) && (msX < sX + 180) && (msY > sY + 55) && (msY < sY + 70))
		{
			if (m_stDialogBoxInfo[20].sV3 == 1)
			{
				bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_REQ_REPAIRITEM, 0, m_stDialogBoxInfo[20].sV1, m_stDialogBoxInfo[20].sV2, 0, m_pItemList[m_stDialogBoxInfo[20].sV1]->m_cName, m_stDialogBoxInfo[20].sV4); // v1.4
				DisableDialogBox(20);
			}
		}
		break;

	case 3: // Put item in the WH
		if ((msX > sX + 25) && (msX < sX + 105) && (msY > sY + 55) && (msY < sY + 70))
		{
			absX = abs(m_stDialogBoxInfo[20].sV5 - m_sPlayerX);
			absY = abs(m_stDialogBoxInfo[20].sV6 - m_sPlayerY);
			if ((absX <= 8) && (absY <= 8))
			{
				if (_iGetBankItemCount() >= (DEF_MAXBANKITEMS))
				{
					AddEventList(DLGBOX_CLICK_NPCACTION_QUERY9, 10);//"here is no empty space left in warehouse."
				}
				else bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_GIVEITEMTOCHAR, m_stDialogBoxInfo[20].sV1, m_stDialogBoxInfo[20].sV3, m_stDialogBoxInfo[20].sV5, m_stDialogBoxInfo[20].sV6, m_pItemList[m_stDialogBoxInfo[20].sV1]->m_cName, m_stDialogBoxInfo[20].sV4); //v1.4
			}
			else AddEventList(DLGBOX_CLICK_NPCACTION_QUERY7, 10);//"Too far to give the item."

			DisableDialogBox(20);
		}
		break;

	case 4: // talk to npc or Unicorn
		/*if ((m_bIsDialogEnabled[21] == false) && (msX > sX + 125) && (msX < sX + 180) && (msY > sY + 55) && (msY < sY + 70))
		{
			switch (m_stDialogBoxInfo[20].sV3) {
			case 21: // Guard
				bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_TALKTONPC, 0, 21, 0, 0, 0);
				AddEventList(TALKING_TO_GUARD, 10);//"Talking to Guard..."
				break;

			case 32: // Unicorn
				bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_TALKTONPC, 0, 32, 0, 0, 0);
				AddEventList(TALKING_TO_UNICORN, 10);//"Talking to Unicorn..."
				break;
			case 67:
				bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_TALKTONPC, 0, 67, 0, 0, 0);
				AddEventList(TALKING_TO_MCGAFFIN, 10);//"Talking to a town man..."
				break;
			case 68:
				bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_TALKTONPC, 0, 68, 0, 0, 0);
				AddEventList(TALKING_TO_PERRY, 10);//"Talking to a town maiden..."
				break;
			case 69:
				bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_TALKTONPC, 0, 69, 0, 0, 0);
				AddEventList(TALKING_TO_DEVLIN, 10);//"Talking to a town magician..."
				break;
			}
		}
		DisableDialogBox(20);*/
		break;


	case 5: // Talk
		if ((msX > sX + 25) && (msX < sX + 100) && (msY > sY + 55) && (msY < sY + 70))
		{
			EnableDialogBox(m_stDialogBoxInfo[20].sV1, m_stDialogBoxInfo[20].sV2, 0, 0);
			DisableDialogBox(20);
		}
		if ((msX > sX + 25 + 75) && (msX < sX + 80 + 75) && (msY > sY + 55) && (msY < sY + 70))
		{
			EnableDialogBox(31, 0, 0, 0);
			DisableDialogBox(20);
		}

		if ((m_bIsDialogEnabled[21] == false) && (msX > sX + 155) && (msX < sX + 210) && (msY > sY + 55) && (msY < sY + 70))
		{
			switch (m_stDialogBoxInfo[20].sV1) {
			case 7:	// Guild
				bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_TALKTONPC, 0, 1, 0, 0, 0);
				AddEventList(TALKING_TO_GUILDHALL_OFFICER, 10);//"Talking to Guildhall Officer..."
				break;

			case 11: //
				switch (m_stDialogBoxInfo[20].sV2) {
				case 1:
					bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_TALKTONPC, 0, 2, 0, 0, 0);
					AddEventList(TALKING_TO_SHOP_KEEPER, 10);//"Talking to Shop Keeper..."
					break;
				case 2:
					bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_TALKTONPC, 0, 3, 0, 0, 0);
					AddEventList(TALKING_TO_BLACKSMITH_KEEPER, 10);//"
					break;
				}
				break;

			case 13: //
				bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_TALKTONPC, 0, 4, 0, 0, 0);
				AddEventList(TALKING_TO_CITYHALL_OFFICER, 10);//"
				break;

			case 14: //
				bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_TALKTONPC, 0, 5, 0, 0, 0);
				AddEventList(TALKING_TO_WAREHOUSE_KEEPER, 10);//
				break;

			case 16: //
				bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_TALKTONPC, 0, 6, 0, 0, 0);
				AddEventList(TALKING_TO_MAGICIAN, 10);//"
				break;
			}
			DisableDialogBox(20);
		}
		//50Cent - Repair All
		if ((msX > sX + 155) && (msX < sX + 210) && (msY > sY + 22) && (msY < sY + 37))
			if (m_stDialogBoxInfo[20].sV3 == 24)
			{
				bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_REQ_REPAIRALL, 0, 0, 0, 0, 0, 0); // v1.4
				DisableDialogBox(20);
			}
		break;

	case 6: // Snoopy: Added Gail
		if ((msX > sX + 25) && (msX < sX + 100) && (msY > sY + 55) && (msY < sY + 70))
		{
			EnableDialogBox(51, 0, 0, 0);
			DisableDialogBox(20);
		}
		break;

	//Magn0S:: Add Learn All Magic button
	case 7:
		/*if ((msX > sX + 25) && (msX < sX + 100) && (msY > sY + 25) && (msY < sY + 40))
		{
			bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_CLIENTMSG, 0, 1, 0, 0, 0);
			PlaySound('E', 14, 5);
		}*/

		if ((msX > sX + 25) && (msX < sX + 100) && (msY > sY + 55) && (msY < sY + 70))
		{
			EnableDialogBox(16, 0, 0, 0);
			DisableDialogBox(20);
			PlaySound('E', 14, 5);
		}

		if ((msX > sX + 125) && (msX < sX + 200) && (msY > sY + 55) && (msY < sY + 70))
		{
			// Centuu : Enchanting
			EnableDialogBox(44, 0, 0, 0);
			DisableDialogBox(20);
			PlaySound('E', 14, 5);
		}
		break;
	}
}


void CGame::DlgBoxClick_Shop(short msX, short msY)
{
	int i;
	short sX, sY;
	char cTemp[21];
	sX = m_stDialogBoxInfo[11].sX;
	sY = m_stDialogBoxInfo[11].sY;

	switch (m_stDialogBoxInfo[11].cMode) {
	case 0:
		for (i = 0; i < 13; i++)
			if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + i * 18 + 65) && (msY <= sY + i * 18 + 79)) {
				if (_iGetTotalItemNum() >= 50) {
					AddEventList(DLGBOX_CLICK_SHOP1, 10);//"You cannot buy anything because your bag is full."
					return;
				}

				PlaySound('E', 14, 5);
				if (m_pItemForSaleList[m_stDialogBoxInfo[11].sView + i] != 0)
					m_stDialogBoxInfo[11].cMode = m_stDialogBoxInfo[11].sView + i + 1;
				return;
			}
		break;

	default:
		if ((msX >= sX + 145) && (msX <= sX + 162) && (msY >= sY + 209) && (msY <= sY + 230))
		{
			m_stDialogBoxInfo[11].sV3 += 10;
			if (m_stDialogBoxInfo[11].sV3 >= (50 - _iGetTotalItemNum()))
				m_stDialogBoxInfo[11].sV3 = (50 - _iGetTotalItemNum());
		}

		if ((msX >= sX + 145) && (msX <= sX + 162) && (msY >= sY + 234) && (msY <= sY + 251))
		{
			m_stDialogBoxInfo[11].sV3 -= 10;
			if (m_stDialogBoxInfo[11].sV3 <= 1)
				m_stDialogBoxInfo[11].sV3 = 1;
		}

		if ((msX >= sX + 163) && (msX <= sX + 180) && (msY >= sY + 209) && (msY <= sY + 230))
		{
			m_stDialogBoxInfo[11].sV3++;
			if (m_stDialogBoxInfo[11].sV3 >= (50 - _iGetTotalItemNum()))
				m_stDialogBoxInfo[11].sV3 = (50 - _iGetTotalItemNum());
		}

		if ((msX >= sX + 163) && (msX <= sX + 180) && (msY >= sY + 234) && (msY <= sY + 251))
		{
			m_stDialogBoxInfo[11].sV3--;
			if (m_stDialogBoxInfo[11].sV3 <= 1)
				m_stDialogBoxInfo[11].sV3 = 1;
		}

		if ((msX >= sX + 30) && (msX <= sX + 30 + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
		{	// Purchase Click
			if ((50 - _iGetTotalItemNum()) < m_stDialogBoxInfo[11].sV3)
			{
				AddEventList(DLGBOX_CLICK_SHOP1, 10);//"ou cannot buy anything because your bag is full."
			}
			else
			{
				ZeroMemory(cTemp, sizeof(cTemp));
				strcpy(cTemp, m_pItemForSaleList[m_stDialogBoxInfo[11].cMode - 1]->m_cName);
				bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_REQ_PURCHASEITEM, 0, m_stDialogBoxInfo[11].sV3, 0, 0, cTemp);
			}
			m_stDialogBoxInfo[11].cMode = 0;
			m_stDialogBoxInfo[11].sV3 = 1;
			PlaySound('E', 14, 5);
		}

		if ((msX >= sX + 154) && (msX <= sX + 154 + DEF_BTNSZX) && (msY >= sY + DEF_BTNPOSY) && (msY <= sY + DEF_BTNPOSY + DEF_BTNSZY))
		{	// Cancel Click
			m_stDialogBoxInfo[11].cMode = 0;
			m_stDialogBoxInfo[11].sV3 = 1;
			PlaySound('E', 14, 5);
		}
		break;
	}
}

void CGame::DlgBoxClick_Skill(short msX, short msY)
{
	int i;
	short sX, sY;
	int x = 0;

	sX = m_stDialogBoxInfo[15].sX;
	sY = m_stDialogBoxInfo[15].sY;
	
	switch (m_stDialogBoxInfo[15].cMode) {
	case -1:
		break;
	
	case 0:	
		if (m_bUseOldPanels)
		{
			for (i = 0; i < 17; i++)
				if ((i < DEF_MAXSKILLTYPE) && (m_pSkillCfgList[i + m_stDialogBoxInfo[15].sView] != 0))
				{
					if ((msX >= sX + 23) && (msX <= sX + 135 + 44) && (msY >= sY + 45 + i * 15) && (msY <= sY + 59 + i * 15))
					{
						if ((m_pSkillCfgList[i + m_stDialogBoxInfo[15].sView]->m_bIsUseable == true)
							&& (m_pSkillCfgList[i + m_stDialogBoxInfo[15].sView]->m_iLevel != 0))
						{
							if (m_bSkillUsingStatus == true)
							{
								AddEventList(DLGBOX_CLICK_SKILL1, 10); // "You are already using other skill."
								return;
							}
							if ((m_bCommandAvailable == false) || (m_iHP <= 0))
							{
								AddEventList(DLGBOX_CLICK_SKILL2, 10); // "You can't use a skill while you are moving."
								return;
							}
							if (m_bIsGetPointingMode == true)
							{
								return;
							}
							switch (m_pSkillCfgList[i + m_stDialogBoxInfo[15].sView]->m_cUseMethod) {
							case 0:
							case 2:
								bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_REQ_USESKILL, 0, (i + m_stDialogBoxInfo[15].sView), 0, 0, 0);
								m_bSkillUsingStatus = true;
								DisableDialogBox(15);
								PlaySound('E', 14, 5);
								break;
							}
						}
					}
					else if ((msX >= sX + 215) && (msX <= sX + 240) && (msY >= sY + 45 + i * 15) && (msY <= sY + 59 + i * 15))
					{
						if (m_stDialogBoxInfo[15].bFlag == false)
						{
							bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_REQ_SETDOWNSKILLINDEX, 0, i + m_stDialogBoxInfo[15].sView, 0, 0, 0);
							PlaySound('E', 14, 5);
							m_stDialogBoxInfo[15].bFlag = true;
						}
					}
				}
		}
		else {
			for (i = 0; i < 24; i++)
				if ((i < DEF_MAXSKILLTYPE) && (m_pSkillCfgList[i + m_stDialogBoxInfo[15].sView] != 0))
				{
					if (strcmp(m_pSkillCfgList[i + m_stDialogBoxInfo[15].sView]->m_cName, "????") == 0) continue;
					if ((msX >= sX + 23) && (msX <= sX + 135 + 44) && (msY >= sY + 30 + (x * 15) + 15) && (msY <= sY + 44 + (x * 15) + 15))
					{
						if ((m_pSkillCfgList[i + m_stDialogBoxInfo[15].sView]->m_bIsUseable == true)
							&& (m_pSkillCfgList[i + m_stDialogBoxInfo[15].sView]->m_iLevel != 0))
						{
							if (m_bSkillUsingStatus == true)
							{
								AddEventList(DLGBOX_CLICK_SKILL1, 10); // "You are already using other skill."
								return;
							}
							if ((m_bCommandAvailable == false) || (m_iHP <= 0))
							{
								AddEventList(DLGBOX_CLICK_SKILL2, 10); // "You can't use a skill while you are moving."
								return;
							}
							if (m_bIsGetPointingMode == true)
							{
								return;
							}
							switch (m_pSkillCfgList[i + m_stDialogBoxInfo[15].sView]->m_cUseMethod) {
							case 0:
							case 2:
								bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_REQ_USESKILL, 0, (i + m_stDialogBoxInfo[15].sView), 0, 0, 0);
								m_bSkillUsingStatus = true;
								DisableDialogBox(15);
								PlaySound('E', 14, 5);
								break;
							}
						}
					}
					/*else if ((msX >= sX + 215) && (msX <= sX + 240) && (msY >= sY + 45 + i * 15) && (msY <= sY + 59 + i * 15))
					{
						if (m_stDialogBoxInfo[15].bFlag == false)
						{
							bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_REQ_SETDOWNSKILLINDEX, 0, i + m_stDialogBoxInfo[15].sView, 0, 0, 0);
							PlaySound('E', 14, 5);
							m_stDialogBoxInfo[15].bFlag = true;
						}
					}*/
					x++;
				}
		}
		break;
	}
}

void CGame::DlgBoxClick_SkillDlg(short msX, short msY)
{
	int i, iAdjX, iAdjY;
	short sX, sY;

	sX = m_stDialogBoxInfo[26].sX;
	sY = m_stDialogBoxInfo[26].sY;

	iAdjX = 5;
	iAdjY = 8;


	switch (m_stDialogBoxInfo[26].cMode) {
	case 1:
		if ((msX >= sX + iAdjX + 60) && (msX <= sX + iAdjX + 153) && (msY >= sY + iAdjY + 175) && (msY <= sY + iAdjY + 195))
		{
			m_stDialogBoxInfo[26].cMode = 2;
			m_stDialogBoxInfo[26].cStr[0] = 1;
			m_stDialogBoxInfo[26].dwT1 = m_dwCurTime;
			PlaySound('E', 14, 5);
			AddEventList(DLGBOX_CLICK_SKILLDLG1, 10); // "Mixing a potion..."
			PlaySound('E', 41, 0);
		}
		break;
		// Crafting
	case 7:
		if ((msX >= sX + iAdjX + 60) && (msX <= sX + iAdjX + 153) && (msY >= sY + iAdjY + 175) && (msY <= sY + iAdjY + 195))
		{
			//DebugLog("Tag1 DlgBoxClick_SkillDlg");
			if (m_stDialogBoxInfo[26].sV1 == -1)
			{
				AddEventList(DLGBOX_CLICK_SKILLDLG2, 10); // "There is not enough crafting materials. Please put in more materials."
				PlaySound('E', 14, 5);
			}
			else
			{
				m_stDialogBoxInfo[26].cMode = 8;
				m_stDialogBoxInfo[26].dwT1 = m_dwCurTime;
				m_stDialogBoxInfo[26].cStr[1] = 1;
				PlaySound('E', 14, 5);
				AddEventList(DLGBOX_CLICK_SKILLDLG3, 10); // "Crafting the item in progress"
				PlaySound('E', 51, 0);
			}
		}
		break;

	case 3:
		for (i = 0; i < 13; i++)
			if (m_pDispBuildItemList[i + m_stDialogBoxInfo[26].sView] != 0)
			{
				if ((msX >= sX + iAdjX + 44) && (msX <= sX + iAdjX + 135 + 44) && (msY >= sY + iAdjY + 55 + i * 15) && (msY <= sY + iAdjY + 55 + 14 + i * 15)) {
					m_stDialogBoxInfo[26].cMode = 4;
					m_stDialogBoxInfo[26].cStr[0] = i + m_stDialogBoxInfo[26].sView;
					PlaySound('E', 14, 5);
				}
			}
		break;

	case 4:
		iAdjX = -1;
		iAdjY = -7;
		if (m_pDispBuildItemList[m_stDialogBoxInfo[26].cStr[0]]->m_bBuildEnabled == true)
		{
			if ((msX >= sX + iAdjX + 32) && (msX <= sX + iAdjX + 95) && (msY >= sY + iAdjY + 353) && (msY <= sY + iAdjY + 372)) {
				// Back
				m_stDialogBoxInfo[26].cMode = 3;
				PlaySound('E', 14, 5);

				if ((m_stDialogBoxInfo[26].sV1 != -1) && (m_pItemList[m_stDialogBoxInfo[26].sV1] != 0))
					m_bIsItemDisabled[m_stDialogBoxInfo[26].sV1] = false;
				if ((m_stDialogBoxInfo[26].sV2 != -1) && (m_pItemList[m_stDialogBoxInfo[26].sV2] != 0))
					m_bIsItemDisabled[m_stDialogBoxInfo[26].sV2] = false;
				if ((m_stDialogBoxInfo[26].sV3 != -1) && (m_pItemList[m_stDialogBoxInfo[26].sV3] != 0))
					m_bIsItemDisabled[m_stDialogBoxInfo[26].sV3] = false;
				if ((m_stDialogBoxInfo[26].sV4 != -1) && (m_pItemList[m_stDialogBoxInfo[26].sV4] != 0))
					m_bIsItemDisabled[m_stDialogBoxInfo[26].sV4] = false;
				if ((m_stDialogBoxInfo[26].sV5 != -1) && (m_pItemList[m_stDialogBoxInfo[26].sV5] != 0))
					m_bIsItemDisabled[m_stDialogBoxInfo[26].sV5] = false;
				if ((m_stDialogBoxInfo[26].sV6 != -1) && (m_pItemList[m_stDialogBoxInfo[26].sV6] != 0))
					m_bIsItemDisabled[m_stDialogBoxInfo[26].sV6] = false;

				m_stDialogBoxInfo[26].sV1 = -1;
				m_stDialogBoxInfo[26].sV2 = -1;
				m_stDialogBoxInfo[26].sV3 = -1;
				m_stDialogBoxInfo[26].sV4 = -1;
				m_stDialogBoxInfo[26].sV5 = -1;
				m_stDialogBoxInfo[26].sV6 = -1;
				m_stDialogBoxInfo[26].cStr[0] = 0;
				m_stDialogBoxInfo[26].cStr[1] = 0;
				m_stDialogBoxInfo[26].cStr[4] = 0;
			}

			if ((msX >= sX + iAdjX + 160) && (msX <= sX + iAdjX + 255) && (msY >= sY + iAdjY + 353) && (msY <= sY + iAdjY + 372))
			{	// Manufacture
				if (m_stDialogBoxInfo[26].cStr[4] == 1)
				{
					m_stDialogBoxInfo[26].cMode = 5;
					m_stDialogBoxInfo[26].cStr[1] = 0;
					m_stDialogBoxInfo[26].dwT1 = m_dwCurTime;
					PlaySound('E', 14, 5);
					PlaySound('E', 44, 0);
				}
			}
		}
		else
		{
			if ((msX >= sX + iAdjX + 32) && (msX <= sX + iAdjX + 95) && (msY >= sY + iAdjY + 353) && (msY <= sY + iAdjY + 372)) 	// Back
			{
				m_stDialogBoxInfo[26].cMode = 3;
				PlaySound('E', 14, 5);

				if ((m_stDialogBoxInfo[26].sV1 != -1) && (m_pItemList[m_stDialogBoxInfo[26].sV1] != 0))
					m_bIsItemDisabled[m_stDialogBoxInfo[26].sV1] = false;
				if ((m_stDialogBoxInfo[26].sV2 != -1) && (m_pItemList[m_stDialogBoxInfo[26].sV2] != 0))
					m_bIsItemDisabled[m_stDialogBoxInfo[26].sV2] = false;
				if ((m_stDialogBoxInfo[26].sV3 != -1) && (m_pItemList[m_stDialogBoxInfo[26].sV3] != 0))
					m_bIsItemDisabled[m_stDialogBoxInfo[26].sV3] = false;
				if ((m_stDialogBoxInfo[26].sV4 != -1) && (m_pItemList[m_stDialogBoxInfo[26].sV4] != 0))
					m_bIsItemDisabled[m_stDialogBoxInfo[26].sV4] = false;
				if ((m_stDialogBoxInfo[26].sV5 != -1) && (m_pItemList[m_stDialogBoxInfo[26].sV5] != 0))
					m_bIsItemDisabled[m_stDialogBoxInfo[26].sV5] = false;
				if ((m_stDialogBoxInfo[26].sV6 != -1) && (m_pItemList[m_stDialogBoxInfo[26].sV6] != 0))
					m_bIsItemDisabled[m_stDialogBoxInfo[26].sV6] = false;

				m_stDialogBoxInfo[26].sV1 = -1;
				m_stDialogBoxInfo[26].sV2 = -1;
				m_stDialogBoxInfo[26].sV3 = -1;
				m_stDialogBoxInfo[26].sV4 = -1;
				m_stDialogBoxInfo[26].sV5 = -1;
				m_stDialogBoxInfo[26].sV6 = -1;
				m_stDialogBoxInfo[26].cStr[0] = 0;
				m_stDialogBoxInfo[26].cStr[1] = 0;
				m_stDialogBoxInfo[26].cStr[4] = 0;
			}
		}
		break;

	case 6:
		iAdjX = -1;
		iAdjY = -7;
		if ((msX >= sX + iAdjX + 32) && (msX <= sX + iAdjX + 95) && (msY >= sY + iAdjY + 353) && (msY <= sY + iAdjY + 372)) {
			// Back
			m_stDialogBoxInfo[26].cMode = 3;
			PlaySound('E', 14, 5);

			if ((m_stDialogBoxInfo[26].sV1 != -1) && (m_pItemList[m_stDialogBoxInfo[26].sV1] != 0))
				m_bIsItemDisabled[m_stDialogBoxInfo[26].sV1] = false;
			if ((m_stDialogBoxInfo[26].sV2 != -1) && (m_pItemList[m_stDialogBoxInfo[26].sV2] != 0))
				m_bIsItemDisabled[m_stDialogBoxInfo[26].sV2] = false;
			if ((m_stDialogBoxInfo[26].sV3 != -1) && (m_pItemList[m_stDialogBoxInfo[26].sV3] != 0))
				m_bIsItemDisabled[m_stDialogBoxInfo[26].sV3] = false;
			if ((m_stDialogBoxInfo[26].sV4 != -1) && (m_pItemList[m_stDialogBoxInfo[26].sV4] != 0))
				m_bIsItemDisabled[m_stDialogBoxInfo[26].sV4] = false;
			if ((m_stDialogBoxInfo[26].sV5 != -1) && (m_pItemList[m_stDialogBoxInfo[26].sV5] != 0))
				m_bIsItemDisabled[m_stDialogBoxInfo[26].sV5] = false;
			if ((m_stDialogBoxInfo[26].sV6 != -1) && (m_pItemList[m_stDialogBoxInfo[26].sV6] != 0))
				m_bIsItemDisabled[m_stDialogBoxInfo[26].sV6] = false;

			m_stDialogBoxInfo[26].sV1 = -1;
			m_stDialogBoxInfo[26].sV2 = -1;
			m_stDialogBoxInfo[26].sV3 = -1;
			m_stDialogBoxInfo[26].sV4 = -1;
			m_stDialogBoxInfo[26].sV5 = -1;
			m_stDialogBoxInfo[26].sV6 = -1;
			m_stDialogBoxInfo[26].cStr[0] = 0;
			m_stDialogBoxInfo[26].cStr[1] = 0;
			m_stDialogBoxInfo[26].cStr[4] = 0;
		}
		break;
	}
}

void CGame::ResponseTeleportList(char *pData)
{
	char *cp;
	int  *ip, i;

	cp = pData + 6;
	ip = (int*)cp;
	m_iTeleportMapCount = *ip;
	cp += 4;
	for (i = 0; i < m_iTeleportMapCount; i++)
	{
		ip = (int*)cp;
		m_stTeleportList[i].iIndex = *ip;
		cp += 4;
		ZeroMemory(m_stTeleportList[i].mapname, sizeof(m_stTeleportList[i].mapname));
		memcpy(m_stTeleportList[i].mapname, cp, 10);
		cp += 10;
		ip = (int*)cp;
		m_stTeleportList[i].iX = *ip;
		cp += 4;
		ip = (int*)cp;
		m_stTeleportList[i].iY = *ip;
		cp += 4;
		ip = (int*)cp;
		m_stTeleportList[i].iCost = *ip;
		cp += 4;
	}
}

void CGame::ResponseChargedTeleport(char *pData)
{
	short *sp;
	char *cp;
	short sRejectReason = 0;
	cp = (char*)pData + DEF_INDEX2_MSGTYPE + 2;
	sp = (short*)cp;
	sRejectReason = *sp;



	switch (sRejectReason)	{
	case 1:
		AddEventList(RESPONSE_CHARGED_TELEPORT1, 10);
		break;
	case 2:
		AddEventList(RESPONSE_CHARGED_TELEPORT2, 10);
		break;
	case 3:
		AddEventList(RESPONSE_CHARGED_TELEPORT3, 10);
		break;
	case 4:
		AddEventList(RESPONSE_CHARGED_TELEPORT4, 10);
		break;
	case 5:
		AddEventList(RESPONSE_CHARGED_TELEPORT5, 10);
		break;
	case 6:
		AddEventList(RESPONSE_CHARGED_TELEPORT6, 10);
		break;
	case 7: // Magn0S:: Added
		AddEventList("Deathmatch Arena is Closed. You can't teleport.", 10);
		break;
	case 8: // Centuu:: Added
		AddEventList("Team Arena is Closed. You can't teleport.", 10);
		break;
	default:
		AddEventList(RESPONSE_CHARGED_TELEPORT7, 10);
		break;
	}
}

void CGame::NotifyMsg_CannotGiveItem(char *pData)
{
	char * cp, cName[21], cTxt[256];
	UINT16 * wp, wItemIndex;
	int  * ip, iAmount;

	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
	wp = (UINT16 *)cp;
	wItemIndex = *wp;
	cp += 2;

	ip = (int *)cp;
	iAmount = *ip;
	cp += 4;

	ZeroMemory(cName, sizeof(cName));
	memcpy(cName, cp, 20);
	cp += 20;

	char cStr1[64], cStr2[64], cStr3[64], cStr4[64], cStr5[64], cStr6[64];
	GetItemName(m_pItemList[wItemIndex], cStr1, cStr2, cStr3, cStr4, cStr5, cStr6);
	if (iAmount == 1) wsprintf(cTxt, NOTIFYMSG_CANNOT_GIVE_ITEM2, cStr1, cName);
	else wsprintf(cTxt, NOTIFYMSG_CANNOT_GIVE_ITEM1, iAmount, cStr1, cName);


	AddEventList(cTxt, 10);
}

void CGame::NotifyMsg_DropItemFin_CountChanged(char *pData)
{
	char * cp, cTxt[256];
	UINT16 * wp, wItemIndex;
	int  * ip, iAmount;

	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
	wp = (UINT16 *)cp;
	wItemIndex = *wp;
	cp += 2;

	ip = (int *)cp;
	iAmount = *ip;
	cp += 4;

	char cStr1[64], cStr2[64], cStr3[64];
	//GetItemName(m_pItemList[wItemIndex]->m_cName, m_pItemList[wItemIndex]->m_dwAttribute, cStr1, cStr2, cStr3);
	GetItemName(m_pItemList[wItemIndex]->m_cName, m_pItemList[wItemIndex]->m_dwAttribute, cStr1, cStr2, cStr3, m_pItemList[wItemIndex]->m_sNewEffect1);

	wsprintf(cTxt, NOTIFYMSG_THROW_ITEM1, iAmount, cStr1);

	AddEventList(cTxt, 10);
}

void CGame::NotifyMsg_CannotJoinMoreGuildsMan(char * pData)
{
	char * cp, cName[12], cTxt[120];

	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
	ZeroMemory(cName, sizeof(cName));
	memcpy(cName, cp, 10);

	wsprintf(cTxt, NOTIFYMSG_CANNOT_JOIN_MOREGUILDMAN1, cName);
	AddEventList(cTxt, 10);
	AddEventList(NOTIFYMSG_CANNOT_JOIN_MOREGUILDMAN2, 10);
}



void CGame::NotifyMsg_DismissGuildsMan(char * pData)
{
	char * cp, cName[12], cTxt[120];
	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
	ZeroMemory(cName, sizeof(cName));
	memcpy(cName, cp, 10);

	if (memcmp(m_cPlayerName, cName, 10) != 0) {
		wsprintf(cTxt, NOTIFYMSG_DISMISS_GUILDMAN1, cName);
		AddEventList(cTxt, 10);
	}
	ClearGuildNameList();
}

void CGame::NotifyMsg_CannotRating(char * pData)
{
	char * cp, cTxt[120];
	UINT16 * wp, wTime;

	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
	wp = (UINT16 *)cp;
	wTime = *wp;
	cp += 2;

	if (wTime == 0) wsprintf(cTxt, NOTIFYMSG_CANNOT_RATING1);
	else wsprintf(cTxt, NOTIFYMSG_CANNOT_RATING2, wTime * 3);
	AddEventList(cTxt, 10);
}

void CGame::NotifyMsg_CannotRepairItem(char * pData)
{
	char * cp, cTxt[120], cStr1[64], cStr2[64], cStr3[64], cStr4[64], cStr5[64], cStr6[64];
	UINT16 * wp, wV1, wV2;

	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
	wp = (UINT16 *)cp;
	wV1 = *wp;
	cp += 2;
	wp = (UINT16 *)cp;
	wV2 = *wp;
	cp += 2;
	ZeroMemory(cStr1, sizeof(cStr1));
	ZeroMemory(cStr2, sizeof(cStr2));
	ZeroMemory(cStr3, sizeof(cStr3));
	ZeroMemory(cStr4, sizeof(cStr4));
	ZeroMemory(cStr5, sizeof(cStr5));
	ZeroMemory(cStr6, sizeof(cStr6));
	GetItemName(m_pItemList[wV1], cStr1, cStr2, cStr3, cStr4, cStr5, cStr6);

	switch (wV2) {
	case 1:
		wsprintf(cTxt, NOTIFYMSG_CANNOT_REPAIR_ITEM1, cStr1);
		AddEventList(cTxt, 10);
		break;
	case 2:
		wsprintf(cTxt, NOTIFYMSG_CANNOT_REPAIR_ITEM2, cStr1);
		AddEventList(cTxt, 10);
		break;
	}
	m_bIsItemDisabled[wV1] = false;
}

void CGame::NotifyMsg_CannotSellItem(char * pData)
{
	char * cp, cTxt[120], cStr1[64], cStr2[64], cStr3[64], cStr4[64], cStr5[64], cStr6[64];
	UINT16 * wp, wV1, wV2;

	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

	wp = (UINT16 *)cp;
	wV1 = *wp;
	cp += 2;

	wp = (UINT16 *)cp;
	wV2 = *wp;
	cp += 2;

	ZeroMemory(cStr1, sizeof(cStr1));
	ZeroMemory(cStr2, sizeof(cStr2));
	ZeroMemory(cStr3, sizeof(cStr3));
	ZeroMemory(cStr4, sizeof(cStr4));
	ZeroMemory(cStr5, sizeof(cStr5));
	ZeroMemory(cStr6, sizeof(cStr6));
	GetItemName(m_pItemList[wV1], cStr1, cStr2, cStr3, cStr4, cStr5, cStr6);

	switch (wV2) {
	case 1:
		wsprintf(cTxt, NOTIFYMSG_CANNOT_SELL_ITEM1, cStr1);//"
		AddEventList(cTxt, 10);
		break;

	case 2:
		wsprintf(cTxt, NOTIFYMSG_CANNOT_SELL_ITEM2, cStr1);//"
		AddEventList(cTxt, 10);
		break;

	case 3:
		wsprintf(cTxt, NOTIFYMSG_CANNOT_SELL_ITEM3, cStr1);//"
		AddEventList(cTxt, 10);
		AddEventList(NOTIFYMSG_CANNOT_SELL_ITEM4, 10);//"
		break;

	case 4:
		AddEventList(NOTIFYMSG_CANNOT_SELL_ITEM5, 10); // "
		AddEventList(NOTIFYMSG_CANNOT_SELL_ITEM6, 10); // "
		break;
	}
	m_bIsItemDisabled[wV1] = false;
}



void CGame::NotifyMsg_DropItemFin_EraseItem(char *pData)
{
	char * cp;
	UINT16 * wp;
	int * ip, iAmount;
	short  sItemIndex;
	char   cTxt[120];

	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

	wp = (UINT16 *)cp;
	sItemIndex = *wp;
	cp += 2;

	ip = (int *)cp;
	iAmount = *ip;
	cp += 4;

	char cStr1[64], cStr2[64], cStr3[64], cStr4[64], cStr5[64], cStr6[64];
	GetItemName(m_pItemList[sItemIndex], cStr1, cStr2, cStr3, cStr4, cStr5, cStr6);

	ZeroMemory(cTxt, sizeof(cTxt));
	if (m_bIsItemEquipped[sItemIndex] == true)
	{
		wsprintf(cTxt, ITEM_EQUIPMENT_RELEASED, cStr1);
		AddEventList(cTxt, 10);
		m_sItemEquipmentStatus[m_pItemList[sItemIndex]->m_cEquipPos] = -1;
		m_bIsItemEquipped[sItemIndex] = false;
	}
	if (m_iHP > 0)
	{
		wsprintf(cTxt, NOTIFYMSG_THROW_ITEM2, cStr1);
	}
	else
	{
		if (iAmount < 2)
			wsprintf(cTxt, NOTIFYMSG_DROPITEMFIN_ERASEITEM3, cStr1); // "You dropped %s."
		else // Snoopy fix - centu fix
		{
			wsprintf(cTxt, NOTIFYMSG_DROPITEMFIN_ERASEITEM4, iAmount, cStr1); // "You dropped %d %s."
		}
	}
	AddEventList(cTxt, 10);
	EraseItem((char)sItemIndex);
	_bCheckBuildItemStatus();
}

void CGame::NotifyMsg_EventFishMode(char * pData)
{
	short sSprite, sSpriteFrame;
	char * cp, cName[21];
	UINT16 * wp, wPrice;
	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

	wp = (UINT16 *)cp;
	wPrice = *wp;
	cp += 2;

	wp = (UINT16 *)cp;
	sSprite = (short)*wp;
	cp += 2;

	wp = (UINT16 *)cp;
	sSpriteFrame = (short)*wp;
	cp += 2;

	ZeroMemory(cName, sizeof(cName));
	memcpy(cName, cp, 20);
	cp += 20;

	EnableDialogBox(24, 0, 0, wPrice, cName);
	m_stDialogBoxInfo[24].sV3 = sSprite;
	m_stDialogBoxInfo[24].sV4 = sSpriteFrame;

	AddEventList(NOTIFYMSG_EVENTFISHMODE1, 10);
}

void CGame::NotifyMsg_Exp(char * pData)
{
	UINT32 * dwp;
	int  * ip;
	char * cp, cTxt[120];
	UINT32 iPrevExp;

	iPrevExp = m_iExp;
	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
	dwp = (UINT32*)cp;
	m_iExp = *dwp;
	cp += 4;
}

void CGame::NotifyMsg_GiveItemFin_CountChanged(char *pData)
{
	char * cp, cName[21], cTxt[256];
	UINT16 * wp, wItemIndex;
	int  * ip, iAmount;

	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
	wp = (UINT16 *)cp;
	wItemIndex = *wp;
	cp += 2;

	ip = (int *)cp;
	iAmount = *ip;
	cp += 4;

	ZeroMemory(cName, sizeof(cName));
	memcpy(cName, cp, 20);
	cp += 20;

	char cStr1[64], cStr2[64], cStr3[64];
	//GetItemName(m_pItemList[wItemIndex]->m_cName, m_pItemList[wItemIndex]->m_dwAttribute, cStr1, cStr2, cStr3);
	GetItemName(m_pItemList[wItemIndex]->m_cName, m_pItemList[wItemIndex]->m_dwAttribute, cStr1, cStr2, cStr3, m_pItemList[wItemIndex]->m_sNewEffect1);
	if (iAmount < 2) wsprintf(cTxt, NOTIFYMSG_GIVEITEMFIN_COUNTCHANGED1, cStr1, cName);
	else wsprintf(cTxt, NOTIFYMSG_GIVEITEMFIN_COUNTCHANGED2, iAmount, cStr1, cName); // centu - fix
	AddEventList(cTxt, 10);
}

void CGame::NotifyMsg_GiveItemFin_EraseItem(char *pData)
{
	char * cp;
	UINT16 * wp;
	int  * ip, iAmount;
	short  sItemIndex;
	char cName[21], cTxt[250];


	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

	wp = (UINT16 *)cp;
	sItemIndex = *wp;
	cp += 2;

	ip = (int *)cp;
	iAmount = *ip;
	cp += 4;

	ZeroMemory(cName, sizeof(cName));
	memcpy(cName, cp, 20);
	cp += 20;

	char cStr1[64], cStr2[64], cStr3[64];
	//GetItemName(m_pItemList[sItemIndex]->m_cName, m_pItemList[sItemIndex]->m_dwAttribute, cStr1, cStr2, cStr3);
	GetItemName(m_pItemList[sItemIndex]->m_cName, m_pItemList[sItemIndex]->m_dwAttribute, cStr1, cStr2, cStr3, m_pItemList[sItemIndex]->m_sNewEffect1);

	if (m_bIsItemEquipped[sItemIndex] == true) {
		wsprintf(cTxt, ITEM_EQUIPMENT_RELEASED, cStr1);
		AddEventList(cTxt, 10);

		// v1.42
		m_sItemEquipmentStatus[m_pItemList[sItemIndex]->m_cEquipPos] = -1;
		m_bIsItemEquipped[sItemIndex] = false;
	}
	if (strlen(cName) == 0) wsprintf(cTxt, NOTIFYMSG_GIVEITEMFIN_ERASEITEM2, iAmount, cStr1);
	else {
		if (strcmp(cName, "Howard") == 0)
			wsprintf(cTxt, NOTIFYMSG_GIVEITEMFIN_ERASEITEM3, iAmount, cStr1);
		else if (strcmp(cName, "William") == 0)
			wsprintf(cTxt, NOTIFYMSG_GIVEITEMFIN_ERASEITEM4, iAmount, cStr1);
		else if (strcmp(cName, "Kennedy") == 0)
			wsprintf(cTxt, NOTIFYMSG_GIVEITEMFIN_ERASEITEM5, iAmount, cStr1);
		else if (strcmp(cName, "Tom") == 0)
			wsprintf(cTxt, NOTIFYMSG_GIVEITEMFIN_ERASEITEM7, iAmount, cStr1);
		else wsprintf(cTxt, NOTIFYMSG_GIVEITEMFIN_COUNTCHANGED2, iAmount, cStr1, cName);
	}
	AddEventList(cTxt, 10);
	EraseItem((char)sItemIndex);
	_bCheckBuildItemStatus();
}

void CGame::NotifyMsg_GlobalAttackMode(char *pData)
{
	char * cp;

	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

	switch (*cp) {
	case 0:
		AddEventList(NOTIFYMSG_GLOBAL_ATTACK_MODE1, 10);
		AddEventList(NOTIFYMSG_GLOBAL_ATTACK_MODE2, 10);
		break;

	case 1:
		AddEventList(NOTIFYMSG_GLOBAL_ATTACK_MODE3, 10);
		break;
	}
	cp++;
}


void CGame::NotifyMsg_HP(char * pData) //LifeX Fix HP Regen Bugs MP 01/01
{
	int iPrevHP, *ip;
	char cTxt[120];

	iPrevHP = m_iHP;
	ip = (int *)(pData + DEF_INDEX2_MSGTYPE + 2);
	m_iHP = *ip;

	if (m_iHP > iPrevHP)
	{
		if ((m_iHP - iPrevHP) < 10) return;
		wsprintf(cTxt, NOTIFYMSG_HP_UP, m_iHP - iPrevHP);
		AddEnergyList(cTxt, 2);
		PlaySound('E', 21, 0);
	}
	else
	{
		if ((m_cLogOutCount > 0) && (m_bForceDisconn == false))
		{
			m_cLogOutCount = -1;
			AddEnergyList(NOTIFYMSG_HP2, 2);
		}
		m_dwDamagedTime = timeGetTime();
		if (m_iHP < 20) AddEnergyList(NOTIFYMSG_HP3, 2);
		if ((iPrevHP - m_iHP) < 10) return;
		wsprintf(cTxt, NOTIFYMSG_HP_DOWN, iPrevHP - m_iHP);
		AddEnergyList(cTxt, 2);
	}
}

void CGame::NotifyMsg_ItemColorChange(char *pData)
{
	short * sp, sItemIndex, sItemColor;
	char * cp;
	char cTxt[120];

	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

	sp = (short *)cp;
	sItemIndex = *sp;
	cp += 2;

	sp = (short *)cp;
	sItemColor = (short)*sp;
	cp += 2;

	if (m_pItemList[sItemIndex] != 0) {
		char cStr1[64], cStr2[64], cStr3[64], cStr4[64], cStr5[64], cStr6[64];
		GetItemName(m_pItemList[sItemIndex], cStr1, cStr2, cStr3, cStr4, cStr5, cStr6);
		if (sItemColor != -1) {
			m_pItemList[sItemIndex]->m_cItemColor = (char)sItemColor;
			wsprintf(cTxt, NOTIFYMSG_ITEMCOLOR_CHANGE1, cStr1);
			AddEventList(cTxt, 10);
		}
		else {
			wsprintf(cTxt, NOTIFYMSG_ITEMCOLOR_CHANGE2, cStr1);
			AddEventList(cTxt, 10);
		}
	}
}

void CGame::NotifyMsg_ItemDepleted_EraseItem(char * pData)
{
	char * cp;
	UINT16 * wp;
	short  sItemIndex;
	bool   bIsUseItemResult;
	char   cTxt[120];

	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

	wp = (UINT16 *)cp;
	sItemIndex = *wp;
	cp += 2;

	bIsUseItemResult = (bool)*cp;
	cp += 2;

	ZeroMemory(cTxt, sizeof(cTxt));

	char cStr1[64], cStr2[64], cStr3[64], cStr4[64], cStr5[64], cStr6[64];
	GetItemName(m_pItemList[sItemIndex], cStr1, cStr2, cStr3, cStr4, cStr5, cStr6);

	if (m_bIsItemEquipped[sItemIndex] == true) 
	{
		wsprintf(cTxt, ITEM_EQUIPMENT_RELEASED, cStr1);
		AddEventList(cTxt, 10);

		// v1.42
		m_sItemEquipmentStatus[m_pItemList[sItemIndex]->m_cEquipPos] = -1;
		m_bIsItemEquipped[sItemIndex] = false;
	}

	ZeroMemory(cTxt, sizeof(cTxt));
	if ((m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_CONSUME) ||
		(m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_ARROW)) 
	{
		wsprintf(cTxt, NOTIFYMSG_ITEMDEPlETED_ERASEITEM2, cStr1);
	}
	else 
	{
		if (m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_USE_DEPLETE)
		{
			if (bIsUseItemResult == true)
			{
				wsprintf(cTxt, NOTIFYMSG_ITEMDEPlETED_ERASEITEM3, cStr1);
			}
		}
		else if (m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_EAT)
		{
			if (bIsUseItemResult == true)
			{
				wsprintf(cTxt, NOTIFYMSG_ITEMDEPlETED_ERASEITEM4, cStr1);
				if ((m_sPlayerType >= 1) && (m_sPlayerType <= 3))
					PlaySound('C', 19, 0);
				if ((m_sPlayerType >= 4) && (m_sPlayerType <= 6))
					PlaySound('C', 20, 0);
			}
		}
		else if (m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_USE_DEPLETE_DEST)
		{
			if (bIsUseItemResult == true)
			{
				wsprintf(cTxt, NOTIFYMSG_ITEMDEPlETED_ERASEITEM3, cStr1);
			}
		}
		else {
			if (bIsUseItemResult == true)
			{
				wsprintf(cTxt, NOTIFYMSG_ITEMDEPlETED_ERASEITEM6, cStr1);
				PlaySound('E', 10, 0);
			}
		}
	}

	AddEventList(cTxt, 10);

	if (bIsUseItemResult == true) m_bItemUsingStatus = false;
	EraseItem((char)sItemIndex);
	_bCheckBuildItemStatus();
}

void CGame::NotifyMsg_ItemLifeSpanEnd(char * pData)
{
	char * cp;
	short * sp, sEquipPos, sItemIndex;
	char cTxt[120];

	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
	sp = (short *)cp;
	sEquipPos = *sp;
	cp += 2;
	sp = (short *)cp;
	sItemIndex = *sp;
	cp += 2;

	char cStr1[64], cStr2[64], cStr3[64], cStr4[64], cStr5[64], cStr6[64];
	GetItemName(m_pItemList[sItemIndex], cStr1, cStr2, cStr3, cStr4, cStr5, cStr6);
	wsprintf(cTxt, NOTIFYMSG_ITEMLIFE_SPANEND1, cStr1);
	AddEventList(cTxt, 10);
	m_sItemEquipmentStatus[m_pItemList[sItemIndex]->m_cEquipPos] = -1;
	m_bIsItemEquipped[sItemIndex] = false;
	m_pItemList[sItemIndex]->m_wCurLifeSpan = 0;

	PlaySound('E', 10, 0);
}

void CGame::NotifyMsg_CurLifeSpan(char* pData)
{
	char* cp;
	short* sp, sItemIndex;

	cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);

	sp = (short*)cp;
	sItemIndex = *sp;
	cp += 2;

	sp = (short*)cp;
	m_pItemList[sItemIndex]->m_wCurLifeSpan = *sp;
	cp += 2;
}

void CGame::NotifyMsg_ItemObtained(char * pData)
{
	char * cp;
	short * sp;
	UINT32 * dwp;
	int i, j, *ip, iClass;

	UINT32 dwCount, dwAttribute;
	char  cName[21], cItemType, cEquipPos;
	bool  bIsEquipped;
	short sSprite, sSpriteFrame, sLevelLimit, sSpecialEV2;
	char  cTxt[120], cGenderLimit, cItemColor;
	UINT16  * wp, wWeight, wCurLifeSpan, wMaxLifeSpan;
	//Magn0S::
	short sNewAtt1, sNewAtt2, sNewAtt3, sNewAtt4;
	short sItemEffectValue6, sItemEffectValue5, sItemEffectValue4, sItemEffectValue3, sItemEffectValue2, sItemEffectValue1, sItemEffectType;

	int iReqStat, iQuantStat;

	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

	cp++;

	ZeroMemory(cName, sizeof(cName));
	memcpy(cName, cp, 20);
	cp += 20;

	dwp = (UINT32 *)cp;
	dwCount = *dwp;
	cp += 4;

	cItemType = *cp;
	cp++;

	cEquipPos = *cp;
	cp++;

	bIsEquipped = (bool)*cp;
	cp++;

	sp = (short *)cp;
	sLevelLimit = *sp;
	cp += 2;

	cGenderLimit = *cp;
	cp++;

	wp = (UINT16 *)cp;
	wCurLifeSpan = *wp;
	cp += 2;

	wp = (UINT16 *)cp;
	wWeight = *wp;
	cp += 2;

	sp = (short *)cp;
	sSprite = *sp;
	cp += 2;

	sp = (short *)cp;
	sSpriteFrame = *sp;
	cp += 2;

	cItemColor = *cp;
	cp++;

	sSpecialEV2 = (short)*cp; // v1.41
	cp++;

	dwp = (UINT32 *)cp;
	dwAttribute = *dwp;
	cp += 4;

	sp = (short*)cp;
	sNewAtt1 = *sp;
	cp += 2;

	sp = (short*)cp;
	sNewAtt2 = *sp;
	cp += 2;

	sp = (short*)cp;
	sNewAtt3 = *sp;
	cp += 2;

	sp = (short*)cp;
	sNewAtt4 = *sp;
	cp += 2;

	sp = (short*)cp;
	sItemEffectValue1 = *sp;
	cp += 2;

	sp = (short*)cp;
	sItemEffectValue2 = *sp;
	cp += 2;

	sp = (short*)cp;
	sItemEffectValue3 = *sp;
	cp += 2;

	sp = (short*)cp;
	sItemEffectValue4 = *sp;
	cp += 2;

	sp = (short*)cp;
	sItemEffectValue5 = *sp;
	cp += 2;

	sp = (short*)cp;
	sItemEffectValue6 = *sp;
	cp += 2;

	sp = (short*)cp;
	sItemEffectType = *sp;
	cp += 2;
	
	wp = (UINT16*)cp;
	wMaxLifeSpan = *wp;
	cp += 2;

	// Centuu - class
	ip = (int*)cp;
	iClass = *ip;
	cp += 4;

	ip = (int*)cp;
	iReqStat = *ip;
	cp += 4;
	ip = (int*)cp;
	iQuantStat = *ip;
	cp += 4;

	char cStr1[64], cStr2[64], cStr3[64];
	//GetItemName(cName, dwAttribute, cStr1, cStr2, cStr3);
	GetItemName(cName, dwAttribute, cStr1, cStr2, cStr3, sNewAtt1, sNewAtt2, sNewAtt3, sNewAtt4);

	ZeroMemory(cTxt, sizeof(cTxt));
	if (dwCount == 1) wsprintf(cTxt, NOTIFYMSG_ITEMOBTAINED2, cStr1);
	else wsprintf(cTxt, NOTIFYMSG_ITEMOBTAINED1, dwCount, cStr1);

	AddEventList(cTxt, 10);

	PlaySound('E', 20, 0);

	if ((cItemType == DEF_ITEMTYPE_CONSUME) || (cItemType == DEF_ITEMTYPE_ARROW))
	{
		for (i = 0; i < DEF_MAXITEMS; i++)
			if ((m_pItemList[i] != 0) && (memcmp(m_pItemList[i]->m_cName, cName, 20) == 0))
			{
				m_pItemList[i]->m_dwCount += dwCount;
				m_bIsItemDisabled[i] = false;
				return;
			}
	}

	short nX, nY;
	for (i = 0; i < DEF_MAXITEMS; i++)
	{
		if ((m_pItemList[i] != 0) && (memcmp(m_pItemList[i]->m_cName, cName, 20) == 0))
		{
			nX = m_pItemList[i]->m_sX;
			nY = m_pItemList[i]->m_sY;
			break;
		}
		else
		{
			nX = 40;
			nY = 30;
		}
	}


	for (i = 0; i < DEF_MAXITEMS; i++)
		if (m_pItemList[i] == 0)
		{
			m_pItemList[i] = new class CItem;
			memcpy(m_pItemList[i]->m_cName, cName, 20);
			m_pItemList[i]->m_dwCount = dwCount;
			m_pItemList[i]->m_sX = nX;
			m_pItemList[i]->m_sY = nY;
			bSendCommand(MSGID_REQUEST_SETITEMPOS, 0, i, nX, nY, 0, 0);
			m_pItemList[i]->m_cItemType = cItemType;
			m_pItemList[i]->m_cEquipPos = cEquipPos;
			m_bIsItemDisabled[i] = false;

			m_bIsItemEquipped[i] = false;
			m_pItemList[i]->m_sLevelLimit = sLevelLimit;
			m_pItemList[i]->m_cGenderLimit = cGenderLimit;
			m_pItemList[i]->m_wCurLifeSpan = wCurLifeSpan;
			m_pItemList[i]->m_wWeight = wWeight;
			m_pItemList[i]->m_sSprite = sSprite;
			m_pItemList[i]->m_sSpriteFrame = sSpriteFrame;
			m_pItemList[i]->m_cItemColor = cItemColor;
			m_pItemList[i]->m_sItemSpecEffectValue2 = sSpecialEV2; // v1.41
			m_pItemList[i]->m_dwAttribute = dwAttribute;

			//Magn0S:: Add new variabled received from HG
			m_pItemList[i]->m_sNewEffect1 = sNewAtt1;
			m_pItemList[i]->m_sNewEffect2 = sNewAtt2;
			m_pItemList[i]->m_sNewEffect3 = sNewAtt3;
			m_pItemList[i]->m_sNewEffect4 = sNewAtt4;

			m_pItemList[i]->m_sItemEffectValue1 = sItemEffectValue1;
			m_pItemList[i]->m_sItemEffectValue2 = sItemEffectValue2;
			m_pItemList[i]->m_sItemEffectValue3 = sItemEffectValue3;
			m_pItemList[i]->m_sItemEffectValue4 = sItemEffectValue4;
			m_pItemList[i]->m_sItemEffectValue5 = sItemEffectValue5;
			m_pItemList[i]->m_sItemEffectValue6 = sItemEffectValue6;

			m_pItemList[i]->m_sItemEffectType = sItemEffectType;
			m_pItemList[i]->m_wMaxLifeSpan = wMaxLifeSpan;

			m_pItemList[i]->m_iClass = iClass; // Centuu - class

			m_pItemList[i]->m_iReqStat = iReqStat; // Centuu - class
			m_pItemList[i]->m_iQuantStat = iQuantStat; // Centuu - class

			_bCheckBuildItemStatus();

			for (j = 0; j < DEF_MAXITEMS; j++)
				if (m_cItemOrder[j] == -1) {
					m_cItemOrder[j] = i;
					return;
				}
			return;
		}
}

void CGame::NotifyMsg_ItemPurchased(char * pData)
{
	char  * cp;
	short * sp;
	UINT32 * dwp;
	UINT16  * wp;
	int i, j, *ip, iClass, wCost;

	UINT32 dwCount;
	char  cName[21], cItemType, cEquipPos, cGenderLimit;
	bool  bIsEquipped;
	short sSprite, sSpriteFrame, sLevelLimit;
	UINT16  wWeight, wCurLifeSpan, wMaxLifeSpan;
	char  cTxt[120], cItemColor;
	//Magn0S:: Add
	short sNewAtt1, sNewAtt2, sNewAtt3, sNewAtt4;
	short sItemEffectValue6, sItemEffectValue5, sItemEffectValue4, sItemEffectValue3, sItemEffectValue2, sItemEffectValue1, sItemEffectType;

	int iReqStat, iQuantStat;

	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

	cp++;

	ZeroMemory(cName, sizeof(cName));
	memcpy(cName, cp, 20);
	cp += 20;

	dwp = (UINT32 *)cp;
	dwCount = *dwp;
	cp += 4;

	cItemType = *cp;
	cp++;

	cEquipPos = *cp;
	cp++;

	bIsEquipped = (bool)*cp;
	cp++;

	sp = (short *)cp;
	sLevelLimit = *sp;
	cp += 2;

	cGenderLimit = *cp;
	cp++;

	wp = (UINT16 *)cp;
	wCurLifeSpan = *wp;
	cp += 2;

	wp = (UINT16 *)cp;
	wWeight = *wp;
	cp += 2;

	sp = (short *)cp;
	sSprite = *sp;
	cp += 2;

	sp = (short *)cp;
	sSpriteFrame = *sp;
	cp += 2;

	cItemColor = *cp; // v1.4
	cp++;

	ip = (int *)cp;
	wCost = *ip;
	cp += 4;

	sp = (short*)cp;
	sNewAtt1 = *sp;
	cp += 2;

	sp = (short*)cp;
	sNewAtt2 = *sp;
	cp += 2;

	sp = (short*)cp;
	sNewAtt3 = *sp;
	cp += 2;

	sp = (short*)cp;
	sNewAtt4 = *sp;
	cp += 2;

	sp = (short*)cp;
	sItemEffectValue1 = *sp;
	cp += 2;

	sp = (short*)cp;
	sItemEffectValue2 = *sp;
	cp += 2;

	sp = (short*)cp;
	sItemEffectValue3 = *sp;
	cp += 2;

	sp = (short*)cp;
	sItemEffectValue4 = *sp;
	cp += 2;

	sp = (short*)cp;
	sItemEffectValue5 = *sp;
	cp += 2;

	sp = (short*)cp;
	sItemEffectValue6 = *sp;
	cp += 2;

	sp = (short*)cp;
	sItemEffectType = *sp;
	cp += 2;

	wp = (UINT16*)cp;
	wMaxLifeSpan = *wp;
	cp += 2;
	
	// Centuu - class
	ip = (int*)cp;
	iClass = *ip;
	cp += 4;

	ip = (int*)cp;
	iReqStat = *ip;
	cp += 4;
	ip = (int*)cp;
	iQuantStat = *ip;
	cp += 4;
	
	ZeroMemory(cTxt, sizeof(cTxt));
	char cStr1[64], cStr2[64], cStr3[64];
	GetItemName(cName, 0, cStr1, cStr2, cStr3);

	switch (iNewShop) {
	case 1:
		wsprintf(cTxt, NOTIFYMSG_ITEMPURCHASED_EK, cStr1, wCost);
		break;
	case 2:
		wsprintf(cTxt, NOTIFYMSG_ITEMPURCHASED_CONTRIB, cStr1, wCost);
		break;
	case 3:
		wsprintf(cTxt, NOTIFYMSG_ITEMPURCHASED_COIN, cStr1, wCost);
		break;
	default:
		wsprintf(cTxt, NOTIFYMSG_ITEMPURCHASED, cStr1, wCost);
		break;
	}
	
	AddEventList(cTxt, 10);

	if ((cItemType == DEF_ITEMTYPE_CONSUME) || (cItemType == DEF_ITEMTYPE_ARROW))
	{
		for (i = 0; i < DEF_MAXITEMS; i++)
			if ((m_pItemList[i] != 0) && (memcmp(m_pItemList[i]->m_cName, cName, 20) == 0))
			{
				m_pItemList[i]->m_dwCount += dwCount;
				return;
			}
	}

	short nX, nY;
	for (i = 0; i < DEF_MAXITEMS; i++)
	{
		if ((m_pItemList[i] != 0) && (memcmp(m_pItemList[i]->m_cName, cName, 20) == 0))
		{
			nX = m_pItemList[i]->m_sX;
			nY = m_pItemList[i]->m_sY;
			break;
		}
		else
		{
			nX = 40;
			nY = 30;
		}
	}

	for (i = 0; i < DEF_MAXITEMS; i++)
		if (m_pItemList[i] == 0)
		{
			m_pItemList[i] = new class CItem;
			memcpy(m_pItemList[i]->m_cName, cName, 20);
			m_pItemList[i]->m_dwCount = dwCount;
			m_pItemList[i]->m_sX = nX;
			m_pItemList[i]->m_sY = nY;
			bSendCommand(MSGID_REQUEST_SETITEMPOS, 0, i, nX, nY, 0, 0);
			m_pItemList[i]->m_cItemType = cItemType;
			m_pItemList[i]->m_cEquipPos = cEquipPos;
			m_bIsItemDisabled[i] = false;
			m_bIsItemEquipped[i] = false;
			m_pItemList[i]->m_sLevelLimit = sLevelLimit;
			m_pItemList[i]->m_cGenderLimit = cGenderLimit;
			m_pItemList[i]->m_wCurLifeSpan = wCurLifeSpan;
			m_pItemList[i]->m_wWeight = wWeight;
			m_pItemList[i]->m_sSprite = sSprite;
			m_pItemList[i]->m_sSpriteFrame = sSpriteFrame;
			m_pItemList[i]->m_cItemColor = cItemColor;    // v1.4

			m_pItemList[i]->m_sNewEffect1 = sNewAtt1;
			m_pItemList[i]->m_sNewEffect2 = sNewAtt2;
			m_pItemList[i]->m_sNewEffect3 = sNewAtt3;
			m_pItemList[i]->m_sNewEffect4 = sNewAtt4;

			m_pItemList[i]->m_sItemEffectValue1 = sItemEffectValue1;
			m_pItemList[i]->m_sItemEffectValue2 = sItemEffectValue2;
			m_pItemList[i]->m_sItemEffectValue3 = sItemEffectValue3;
			m_pItemList[i]->m_sItemEffectValue4 = sItemEffectValue4;
			m_pItemList[i]->m_sItemEffectValue5 = sItemEffectValue5;
			m_pItemList[i]->m_sItemEffectValue6 = sItemEffectValue6;

			m_pItemList[i]->m_sItemEffectType = sItemEffectType;
			m_pItemList[i]->m_wMaxLifeSpan = wMaxLifeSpan;

			m_pItemList[i]->m_iClass = iClass;

			m_pItemList[i]->m_iReqStat = iReqStat;
			m_pItemList[i]->m_iQuantStat = iQuantStat;

			// fixed v1.11
			for (j = 0; j < DEF_MAXITEMS; j++)
				if (m_cItemOrder[j] == -1) {
					m_cItemOrder[j] = i;
					return;
				}

			return;
		}
}

void CGame::NotifyMsg_ItemReleased(char * pData)
{
	char * cp;
	short * sp, sEquipPos, sItemIndex;
	char cTxt[120];

	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
	sp = (short *)cp;
	sEquipPos = *sp;
	cp += 2;
	sp = (short *)cp;
	sItemIndex = *sp;
	cp += 2;

	char cStr1[64], cStr2[64], cStr3[64], cStr4[64], cStr5[64], cStr6[64];
	GetItemName(m_pItemList[sItemIndex], cStr1, cStr2, cStr3, cStr4, cStr5, cStr6);
	wsprintf(cTxt, ITEM_EQUIPMENT_RELEASED, cStr1);
	AddEventList(cTxt, 10);
	m_bIsItemEquipped[sItemIndex] = false;
	m_sItemEquipmentStatus[m_pItemList[sItemIndex]->m_cEquipPos] = -1;

	if (memcmp(m_pItemList[sItemIndex]->m_cName, "AngelicPandent(STR)", 19) == 0) PlaySound('E', 53, 0);
	else if (memcmp(m_pItemList[sItemIndex]->m_cName, "AngelicPandent(DEX)", 19) == 0) PlaySound('E', 53, 0);
	else if (memcmp(m_pItemList[sItemIndex]->m_cName, "AngelicPandent(INT)", 19) == 0) PlaySound('E', 53, 0);
	else if (memcmp(m_pItemList[sItemIndex]->m_cName, "AngelicPandent(MAG)", 19) == 0) PlaySound('E', 53, 0);
	else PlaySound('E', 29, 0);
}

void CGame::NotifyMsg_ItemRepaired(char * pData)
{
	char * cp, cTxt[120];
	UINT32 * dwp, dwItemID, dwLife;

	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

	dwp = (UINT32 *)cp;
	dwItemID = *dwp;
	cp += 4;

	dwp = (UINT32 *)cp;
	dwLife = *dwp;
	cp += 4;

	m_pItemList[dwItemID]->m_wCurLifeSpan = (UINT16)dwLife;
	m_bIsItemDisabled[dwItemID] = false;
	char cStr1[64], cStr2[64], cStr3[64], cStr4[64], cStr5[64], cStr6[64];
	GetItemName(m_pItemList[dwItemID], cStr1, cStr2, cStr3, cStr4, cStr5, cStr6);

	wsprintf(cTxt, NOTIFYMSG_ITEMREPAIRED1, cStr1);

	AddEventList(cTxt, 10);
}

void CGame::NotifyMsg_ItemToBank(char *pData)
{
	char * cp, cIndex;
	UINT32 * dwp, dwCount, dwAttribute;
	char  cName[21], cItemType, cEquipPos, cGenderLimit, cItemColor;
	bool  bIsEquipped;
	short * sp, sSprite, sSpriteFrame, sLevelLimit, sItemSpecEffectValue2;
	UINT16* wp, wWeight, wCurLifeSpan, wMaxLifeSpan;
	char  cTxt[120];
	//Magn0S::
	short sNewAtt1, sNewAtt2, sNewAtt3, sNewAtt4;
	short sItemEffectValue6, sItemEffectValue5, sItemEffectValue4, sItemEffectValue3, sItemEffectValue2, sItemEffectValue1, sItemEffectType;
	int* ip, iClass, iReqStat, iQuantStat;

	//cp = (pData + DEF_INDEX2_MSGTYPE + 2);
	cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);

	cIndex = *cp;
	cp++;

	cp++;

	ZeroMemory(cName, sizeof(cName));
	memcpy(cName, cp, 20);
	cp += 20;

	dwp = (UINT32 *)cp;
	dwCount = *dwp;
	cp += 4;

	cItemType = *cp;
	cp++;

	cEquipPos = *cp;
	cp++;

	bIsEquipped = (bool)*cp;
	cp++;

	sp = (short *)cp;
	sLevelLimit = *sp;
	cp += 2;

	cGenderLimit = *cp;
	cp++;

	wp = (UINT16 *)cp;
	wCurLifeSpan = *wp;
	cp += 2;

	wp = (UINT16 *)cp;
	wWeight = *wp;
	cp += 2;

	sp = (short *)cp;
	sSprite = *sp;
	cp += 2;

	sp = (short *)cp;
	sSpriteFrame = *sp;
	cp += 2;

	cItemColor = *cp;
	cp++;

	// v1.432
	sp = (short *)cp;
	sItemEffectValue2 = *sp;
	cp += 2;

	dwp = (UINT32 *)cp;
	dwAttribute = *dwp;
	cp += 4;

	sp = (short*)cp;
	sItemSpecEffectValue2 = (short)*cp;
	//cp++;
	cp += 2;

	sp = (short*)cp;
	sNewAtt1 = *sp;
	cp += 2;

	sp = (short*)cp;
	sNewAtt2 = *sp;
	cp += 2;

	sp = (short*)cp;
	sNewAtt3 = *sp;
	cp += 2;

	sp = (short*)cp;
	sNewAtt4 = *sp;
	cp += 2;

	sp = (short*)cp;
	sItemEffectValue1 = *sp;
	cp += 2;

	sp = (short*)cp;
	sItemEffectValue2 = *sp;
	cp += 2;

	sp = (short*)cp;
	sItemEffectValue3 = *sp;
	cp += 2;

	sp = (short*)cp;
	sItemEffectValue4 = *sp;
	cp += 2;

	sp = (short*)cp;
	sItemEffectValue5 = *sp;
	cp += 2;

	sp = (short*)cp;
	sItemEffectValue6 = *sp;
	cp += 2;

	sp = (short*)cp;
	sItemEffectType = *sp;
	cp += 2;

	wp = (UINT16*)cp;
	wMaxLifeSpan = *wp;
	cp += 2;

	ip = (int*)cp;
	iClass = *ip;
	cp += 4;

	ip = (int*)cp;
	iReqStat = *ip;
	cp += 4;
	ip = (int*)cp;
	iQuantStat = *ip;
	cp += 4;

	char cStr1[64], cStr2[64], cStr3[64];
	//GetItemName(cName, dwAttribute, cStr1, cStr2, cStr3);
	GetItemName(cName, dwAttribute, cStr1, cStr2, cStr3, sNewAtt1, sNewAtt2, sNewAtt3, sNewAtt4);

	if (m_pBankList[cIndex] == 0) {
		m_pBankList[cIndex] = new class CItem;

		memcpy(m_pBankList[cIndex]->m_cName, cName, 20);
		m_pBankList[cIndex]->m_dwCount = dwCount;

		m_pBankList[cIndex]->m_cItemType = cItemType;
		m_pBankList[cIndex]->m_cEquipPos = cEquipPos;

		m_pBankList[cIndex]->m_sLevelLimit = sLevelLimit;
		m_pBankList[cIndex]->m_cGenderLimit = cGenderLimit;
		m_pBankList[cIndex]->m_wCurLifeSpan = wCurLifeSpan;
		m_pBankList[cIndex]->m_wWeight = wWeight;
		m_pBankList[cIndex]->m_sSprite = sSprite;
		m_pBankList[cIndex]->m_sSpriteFrame = sSpriteFrame;
		m_pBankList[cIndex]->m_cItemColor = cItemColor;
		m_pBankList[cIndex]->m_sItemEffectValue2 = sItemEffectValue2;
		m_pBankList[cIndex]->m_dwAttribute = dwAttribute;
		m_pBankList[cIndex]->m_sItemSpecEffectValue2 = sItemSpecEffectValue2;

		m_pBankList[cIndex]->m_sNewEffect1 = sNewAtt1;
		m_pBankList[cIndex]->m_sNewEffect2 = sNewAtt2;
		m_pBankList[cIndex]->m_sNewEffect3 = sNewAtt3;
		m_pBankList[cIndex]->m_sNewEffect4 = sNewAtt4;

		m_pBankList[cIndex]->m_sItemEffectValue1 = sItemEffectValue1;
		m_pBankList[cIndex]->m_sItemEffectValue2 = sItemEffectValue2;
		m_pBankList[cIndex]->m_sItemEffectValue3 = sItemEffectValue3;
		m_pBankList[cIndex]->m_sItemEffectValue4 = sItemEffectValue4;
		m_pBankList[cIndex]->m_sItemEffectValue5 = sItemEffectValue5;
		m_pBankList[cIndex]->m_sItemEffectValue6 = sItemEffectValue6;

		m_pBankList[cIndex]->m_sItemEffectType = sItemEffectType;
		m_pBankList[cIndex]->m_wMaxLifeSpan = wMaxLifeSpan;
		m_pBankList[cIndex]->m_iClass = iClass;

		m_pBankList[cIndex]->m_iReqStat = iReqStat;
		m_pBankList[cIndex]->m_iQuantStat = iQuantStat;

		ZeroMemory(cTxt, sizeof(cTxt));
		if (dwCount == 1) wsprintf(cTxt, NOTIFYMSG_ITEMTOBANK3, cStr1);
		else wsprintf(cTxt, NOTIFYMSG_ITEMTOBANK2, dwCount, cStr1);

		if (m_bIsDialogEnabled[14] == true) m_stDialogBoxInfo[14].sView = DEF_MAXBANKITEMS - 11;
		AddEventList(cTxt, 10);
	}
}

void CGame::NotifyMsg_Killed(char * pData)
{
	char * cp, cAttackerName[21];
	m_bCommandAvailable = false;
	m_cCommand = DEF_OBJECTSTOP;
	m_iHP = 0;
	m_cCommand = -1;
	// Restart
	m_bItemUsingStatus = false;
	ClearSkillUsingStatus();
	ZeroMemory(cAttackerName, sizeof(cAttackerName));
	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
	memcpy(cAttackerName, cp, 20);
	cp += 20;

	// Snoopy: reduced 3 lines -> 2 lines
	AddEventList(NOTIFYMSG_KILLED1, 10);
	AddEventList(NOTIFYMSG_KILLED3, 10);
}

bool CGame::bInitMagicCfgList()
{
	char cFn[255], cTemp[255];
	char * pContents, *token;
	char seps[] = "= ,\t\n";
	char cReadModeA = 0;
	char cReadModeB = 0;
	int  iMagicCfgListIndex = 0;
	class CStrTok * pStrTok;
	HANDLE hFile;
	FILE * pFile;
	UINT32  dwFileSize;

	ZeroMemory(cTemp, sizeof(cTemp));
	ZeroMemory(cFn, sizeof(cFn));

	// CLEROTH - MAGIC CFG
	strcpy(cTemp, "magiccfg.txt");

	strcat(cFn, "contents");
	strcat(cFn, "\\");
	strcat(cFn, "\\");
	strcat(cFn, cTemp);

	hFile = CreateFile(cFn, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
	dwFileSize = GetFileSize(hFile, 0);
	if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);

	pFile = fopen(cFn, "rt");
	if (pFile == 0) return false;
	else {
		pContents = new char[dwFileSize + 1];
		ZeroMemory(pContents, dwFileSize + 1);
		fread(pContents, dwFileSize, 1, pFile);
		fclose(pFile);
	}

	pStrTok = new class CStrTok(pContents, seps);
	token = pStrTok->pGet();
	while (token != 0) {
		if (cReadModeA != 0) {
			switch (cReadModeA) {
			case 1:
				switch (cReadModeB) {
				case 1:
					if (_bGetIsStringIsNumber(token) == false)
					{
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					if (m_pMagicCfgList[atoi(token)] != 0)
					{
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMagicCfgList[atoi(token)] = new class CMagic;
					iMagicCfgListIndex = atoi(token);

					cReadModeB = 2;
					break;

				case 2:
					ZeroMemory(m_pMagicCfgList[iMagicCfgListIndex]->m_cName, sizeof(m_pMagicCfgList[iMagicCfgListIndex]->m_cName));
					memcpy(m_pMagicCfgList[iMagicCfgListIndex]->m_cName, token, strlen(token));
					cReadModeB = 3;
					break;

				case 3: // m_sValue1
					if (_bGetIsStringIsNumber(token) == false) {
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMagicCfgList[iMagicCfgListIndex]->m_sValue1 = atoi(token);
					cReadModeB = 4;
					break;

				case 4: // m_sValue2	// INT
					if (_bGetIsStringIsNumber(token) == false) {
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMagicCfgList[iMagicCfgListIndex]->m_sValue2 = atoi(token);
					cReadModeB = 5;
					break;

				case 5: // m_sValue3	// COST
					if (_bGetIsStringIsNumber(token) == false) {
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMagicCfgList[iMagicCfgListIndex]->m_sValue3 = atoi(token);
					cReadModeB = 6;
					break;

					// CLEROTH MAGIC CFG
				case 6: // m_sValue4	// STR
					if (_bGetIsStringIsNumber(token) == false)
					{
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMagicCfgList[iMagicCfgListIndex]->m_sValue4 = atoi(token);
					cReadModeB = 7;
					break;

				case 7: // m_sValue5
					cReadModeB = 8;
					break;

				case 8: // m_sValue6
					cReadModeB = 9;
					break;

				case 9: // m_bIsVisible
					if (_bGetIsStringIsNumber(token) == false)
					{
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					if (atoi(token) == 0) m_pMagicCfgList[iMagicCfgListIndex]->m_bIsVisible = false;
					else m_pMagicCfgList[iMagicCfgListIndex]->m_bIsVisible = true;
					cReadModeA = 0;
					cReadModeB = 0;
					break;
				}
				break;

			default:
				break;
			}
		}
		else {
			if (memcmp(token, "magic", 5) == 0) {
				cReadModeA = 1;
				cReadModeB = 1;
			}
		}
		token = pStrTok->pGet();
	}

	delete pStrTok;
	delete[] pContents;

	if ((cReadModeA != 0) || (cReadModeB != 0)) {
		return false;
	}

	return true;
}

bool CGame::bCheckImportantFile()
{
	HANDLE hFile;


	hFile = CreateFile("SPRITES\\TREES1.PAK", GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
	if (hFile == INVALID_HANDLE_VALUE) return false;

	CloseHandle(hFile);
	return true;
}

bool CGame::bInitSkillCfgList()
{
	char cFn[255], cTemp[255];
	char * pContents, *token;
	char seps[] = "= ,\t\n";
	char cReadModeA = 0;
	char cReadModeB = 0;
	int  iSkillCfgListIndex = 0;
	class CStrTok * pStrTok;
	HANDLE hFile;
	FILE * pFile;
	UINT32  dwFileSize;

	ZeroMemory(cTemp, sizeof(cTemp));
	ZeroMemory(cFn, sizeof(cFn));

	strcpy(cTemp, "Skillcfg.txt");
	strcat(cFn, "contents");
	strcat(cFn, "\\");
	strcat(cFn, "\\");
	strcat(cFn, cTemp);

	hFile = CreateFile(cFn, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
	dwFileSize = GetFileSize(hFile, 0);
	if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);

	pFile = fopen(cFn, "rt");
	if (pFile == 0) return false;
	else {
		pContents = new char[dwFileSize + 1];
		ZeroMemory(pContents, dwFileSize + 1);
		fread(pContents, dwFileSize, 1, pFile);
		fclose(pFile);
	}

	pStrTok = new class CStrTok(pContents, seps);
	token = pStrTok->pGet();
	while (token != 0) {
		if (cReadModeA != 0) {
			switch (cReadModeA) {
			case 1:
				switch (cReadModeB) {
				case 1:
					if (_bGetIsStringIsNumber(token) == false)
					{
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					if (m_pSkillCfgList[atoi(token)] != 0)
					{
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pSkillCfgList[atoi(token)] = new class CSkill;
					iSkillCfgListIndex = atoi(token);
					cReadModeB = 2;
					break;

				case 2:
					ZeroMemory(m_pSkillCfgList[iSkillCfgListIndex]->m_cName, sizeof(m_pSkillCfgList[iSkillCfgListIndex]->m_cName));
					memcpy(m_pSkillCfgList[iSkillCfgListIndex]->m_cName, token, strlen(token));
					cReadModeB = 3;
					break;

				case 3: // m_bIsUseable
					if (_bGetIsStringIsNumber(token) == false) {
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pSkillCfgList[iSkillCfgListIndex]->m_bIsUseable = (bool)atoi(token);
					cReadModeB = 4;
					break;

				case 4: // m_cUseMethod
					if (_bGetIsStringIsNumber(token) == false) {
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pSkillCfgList[iSkillCfgListIndex]->m_cUseMethod = atoi(token);
					cReadModeA = 0;
					cReadModeB = 0;
					break;
				}
				break;

			default:
				break;
			}
		}
		else {
			if (memcmp(token, "skill", 5) == 0) {
				cReadModeA = 1;
				cReadModeB = 1;
			}
		}
		token = pStrTok->pGet();
	}

	delete pStrTok;
	delete[] pContents;

	if ((cReadModeA != 0) || (cReadModeB != 0)) {
		return false;
	}

	return true;
}

bool CGame::_bGetIsStringIsNumber(char * pStr)
{
	int i;
	for (i = 0; i < (int)strlen(pStr); i++)
		if ((pStr[i] != '-') && ((pStr[i] < (char)'0') || (pStr[i] > (char)'9'))) return false;

	return true;
}

void CGame::ReadSettings()
{
	m_sMagicShortCut = -1;
	m_sRecentShortCut = -1;
	for (int i = 0; i<6; i++) m_sShortCut[i] = -1;

	HKEY key;
	DWORD dwDisp;
	UINT Result;
	DWORD Size = sizeof(LONG);
	if (RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\Siementech\\Helbreath\\Settings", 0, 0, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, 0, &key, &dwDisp) != ERROR_SUCCESS) return;

	if (RegQueryValueEx(key, "Magic", 0, 0, (LPBYTE)&Result, &Size) != ERROR_SUCCESS)
	{
		RegCloseKey(key);
		return;
	}
	if (Result > 0 && Result < 101) m_sMagicShortCut = Result - 1;
	else m_sMagicShortCut = -1;

	if (RegQueryValueEx(key, "ShortCut0", 0, 0, (LPBYTE)&Result, &Size) != ERROR_SUCCESS)
	{
		RegCloseKey(key);
		return;
	}
	if (Result > 0 && Result < 201) m_sShortCut[0] = Result - 1;
	else m_sShortCut[0] = -1;

	if (RegQueryValueEx(key, "ShortCut1", 0, 0, (LPBYTE)&Result, &Size) != ERROR_SUCCESS)
	{
		RegCloseKey(key);
		return;
	}
	if (Result > 0 && Result < 201) m_sShortCut[1] = Result - 1;
	else m_sShortCut[1] = -1;

	if (RegQueryValueEx(key, "ShortCut2", 0, 0, (LPBYTE)&Result, &Size) != ERROR_SUCCESS)
	{
		RegCloseKey(key);
		return;
	}
	if (Result > 0 && Result < 201) m_sShortCut[2] = Result - 1;
	else m_sShortCut[2] = -1;

	if (RegQueryValueEx(key, "ShortCut3", 0, 0, (LPBYTE)&Result, &Size) != ERROR_SUCCESS)
	{
		RegCloseKey(key);
		return;
	}
	if (Result > 0 && Result < 201) m_sShortCut[3] = Result - 1;
	else m_sShortCut[3] = -1;

	if (RegQueryValueEx(key, "ShortCut4", 0, 0, (LPBYTE)&Result, &Size) != ERROR_SUCCESS)
	{
		RegCloseKey(key);
		return;
	}
	if (Result > 0 && Result < 201) m_sShortCut[4] = Result - 1;
	else m_sShortCut[4] = -1;

	RegCloseKey(key);
}

void CGame::WriteSettings()
{
	HKEY key;
	DWORD dwDisp;
	UINT nData;
	if (RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\Siementech\\Helbreath\\Settings", 0, 0, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, 0, &key, &dwDisp) != ERROR_SUCCESS) return;

	if (m_sMagicShortCut >= 0 && m_sMagicShortCut < 100) nData = m_sMagicShortCut + 1;
	else nData = 0;
	if (RegSetValueEx(key, "Magic", 0, REG_DWORD, (LPBYTE)&nData, sizeof(UINT)) != ERROR_SUCCESS)
	{
		RegCloseKey(key);
		return;
	}

	if (m_sShortCut[0] >= 0 && m_sShortCut[0] < 200) nData = m_sShortCut[0] + 1;
	else nData = 0;
	if (RegSetValueEx(key, "ShortCut0", 0, REG_DWORD, (LPBYTE)&nData, sizeof(UINT)) != ERROR_SUCCESS)
	{
		RegCloseKey(key);
		return;
	}

	if (m_sShortCut[1] >= 0 && m_sShortCut[1] < 200) nData = m_sShortCut[1] + 1;
	else nData = 0;
	if (RegSetValueEx(key, "ShortCut1", 0, REG_DWORD, (LPBYTE)&nData, sizeof(UINT)) != ERROR_SUCCESS)
	{
		RegCloseKey(key);
		return;
	}

	if (m_sShortCut[2] >= 0 && m_sShortCut[2] < 200) nData = m_sShortCut[2] + 1;
	else nData = 0;
	if (RegSetValueEx(key, "ShortCut2", 0, REG_DWORD, (LPBYTE)&nData, sizeof(UINT)) != ERROR_SUCCESS)
	{
		RegCloseKey(key);
		return;
	}

	if (m_sShortCut[3] >= 0 && m_sShortCut[3] < 200) nData = m_sShortCut[3] + 1;
	else nData = 0;
	if (RegSetValueEx(key, "ShortCut3", 0, REG_DWORD, (LPBYTE)&nData, sizeof(UINT)) != ERROR_SUCCESS)
	{
		RegCloseKey(key);
		return;
	}

	if (m_sShortCut[4] >= 0 && m_sShortCut[4] < 200) nData = m_sShortCut[4] + 1;
	else nData = 0;
	if (RegSetValueEx(key, "ShortCut4", 0, REG_DWORD, (LPBYTE)&nData, sizeof(UINT)) != ERROR_SUCCESS)
	{
		RegCloseKey(key);
		return;
	}
	RegCloseKey(key);
}


bool CGame::bEffectFrameCounter()
{
	int i, x;
	UINT32 dwTime;
	bool bRet = false;
	short sAbsX, sAbsY, sDist;
	char  cDir;
	long lPan;
	dwTime = m_dwCurTime;
	dwTime += m_pMapData->m_dwFrameAdjustTime;
	for (i = 0; i < DEF_MAXEFFECTS; i++)
		if (m_pEffectList[i] != 0) {
			if ((dwTime - m_pEffectList[i]->m_dwTime) > m_pEffectList[i]->m_dwFrameTime)
			{
				m_pEffectList[i]->m_dwTime = dwTime;
				m_pEffectList[i]->m_cFrame++;
				bRet = true;
				m_pEffectList[i]->m_mX2 = m_pEffectList[i]->m_mX;
				m_pEffectList[i]->m_mY2 = m_pEffectList[i]->m_mY;
				switch (m_pEffectList[i]->m_sType) {
				case 1: // coup normal
					if (m_pEffectList[i]->m_cFrame == 1)
					{
						for (int j = 1; j <= m_pEffectList[i]->m_iV1; j++) bAddNewEffect(11, m_pEffectList[i]->m_mX + 15 - (rand() % 30), m_pEffectList[i]->m_mY + 15 - (rand() % 30), 0, 0, -1 * (rand() % 2));
					}
					if (m_pEffectList[i]->m_cFrame >= m_pEffectList[i]->m_cMaxFrame)
					{
						delete m_pEffectList[i];
						m_pEffectList[i] = 0;
					}
					break;

				case 2:	// (Arrow missing target ?)
					m_Misc.GetPoint(m_pEffectList[i]->m_mX, m_pEffectList[i]->m_mY,
						m_pEffectList[i]->m_dX * 32, m_pEffectList[i]->m_dY * 32 - 40,
						&m_pEffectList[i]->m_mX, &m_pEffectList[i]->m_mY,
						&m_pEffectList[i]->m_iErr, 70);
					if ((abs(m_pEffectList[i]->m_mX - m_pEffectList[i]->m_dX * 32) <= 2)
						&& (abs(m_pEffectList[i]->m_mY - (m_pEffectList[i]->m_dY * 32 - 40)) <= 2))
					{
						delete m_pEffectList[i];
						m_pEffectList[i] = 0;
					}
					break;

					//HellFire Fix by NomaD
				case 85:
				case 86:
				case 87:
					if (m_pEffectList[i]->m_cFrame > m_pEffectList[i]->m_cMaxFrame)
					{
						delete m_pEffectList[i];
						m_pEffectList[i] = 0;
					}
					break;
					//HellFire Fix by NomaD
				case 190:
					if (m_pEffectList[i]->m_cFrame > m_pEffectList[i]->m_cMaxFrame)
					{
						delete m_pEffectList[i];
						m_pEffectList[i] = 0;
					}
					else 
					{
						sAbsX = abs(((m_sViewPointX / 32) + 12) - m_pEffectList[i]->m_dX);
						sAbsY = abs(((m_sViewPointY / 32) + 9) - m_pEffectList[i]->m_dY);
						if (sAbsX > sAbsY) sDist = sAbsX;
						else sDist = sAbsY;
						lPan = -(((m_sViewPointX / 32) + 12) - m_pEffectList[i]->m_dX);
						PlaySound('E', 4, sDist, lPan);
					}
					break;

				case 4: // Gold Drop ,33,69,70
				case 33: //
				case 69:
				case 70:
					if (m_pEffectList[i]->m_cFrame > m_pEffectList[i]->m_cMaxFrame)
					{
						delete m_pEffectList[i];
						m_pEffectList[i] = 0;
					}
					break;

				case 5:
				case 30:
				case 31: // Fire Explosion
				case 252:
					if (m_pEffectList[i]->m_cFrame == 1)
					{
						bAddNewEffect(12, m_pEffectList[i]->m_mX + 5 - (rand() % 10), m_pEffectList[i]->m_mY + 5 - (rand() % 10), 0, 0, -1 * (rand() % 2));
						bAddNewEffect(12, m_pEffectList[i]->m_mX + 5 - (rand() % 10), m_pEffectList[i]->m_mY + 5 - (rand() % 10), 0, 0, -1 * (rand() % 2));
						bAddNewEffect(12, m_pEffectList[i]->m_mX + 5 - (rand() % 10), m_pEffectList[i]->m_mY + 5 - (rand() % 10), 0, 0, -1 * (rand() % 2));
						bAddNewEffect(12, m_pEffectList[i]->m_mX + 5 - (rand() % 10), m_pEffectList[i]->m_mY + 5 - (rand() % 10), 0, 0, -1 * (rand() % 2));
						bAddNewEffect(12, m_pEffectList[i]->m_mX + 5 - (rand() % 10), m_pEffectList[i]->m_mY + 5 - (rand() % 10), 0, 0, -1 * (rand() % 2));
					}
					if (m_pEffectList[i]->m_cFrame == 7)
					{
						bAddNewEffect(15, m_pEffectList[i]->m_mX + 5 - (rand() % 10), m_pEffectList[i]->m_mY + 5 - (rand() % 10), 0, 0, 0);
						bAddNewEffect(15, m_pEffectList[i]->m_mX + 5 - (rand() % 10), m_pEffectList[i]->m_mY + 5 - (rand() % 10), 0, 0, 0);
						bAddNewEffect(15, m_pEffectList[i]->m_mX + 5 - (rand() % 10), m_pEffectList[i]->m_mY + 5 - (rand() % 10), 0, 0, 0);
					}
					if (m_pEffectList[i]->m_cFrame > m_pEffectList[i]->m_cMaxFrame)
					{
						delete m_pEffectList[i];
						m_pEffectList[i] = 0;
					}
					break;

				case 193: // Mass-Blizzard
					if (m_pEffectList[i]->m_cFrame > m_pEffectList[i]->m_cMaxFrame)
					{
						delete m_pEffectList[i];
						m_pEffectList[i] = 0;
					}
					else /*if (m_pEffectList[i]->m_cFrame == 1)*/
					{
						bAddNewEffect(71, m_pEffectList[i]->m_sX, m_pEffectList[i]->m_sY,
							m_pEffectList[i]->m_dX * 32 + (rand() % 120) - 60, m_pEffectList[i]->m_dY * 32 + (rand() % 120) - 60, 0);
						sAbsX = abs(((m_sViewPointX / 32) + 12) - m_pEffectList[i]->m_dX);
						sAbsY = abs(((m_sViewPointY / 32) + 9) - m_pEffectList[i]->m_dY);
						if (sAbsX > sAbsY) sDist = sAbsX;
						else sDist = sAbsY;
						lPan = -(((m_sViewPointX / 32) + 12) - m_pEffectList[i]->m_dX);
						PlaySound('E', 1, sDist, lPan);
					}
					break;
				case 192: // Fiery-Shock-Wave
					m_Misc.GetPoint(m_pEffectList[i]->m_mX
						, m_pEffectList[i]->m_mY
						, m_pEffectList[i]->m_dX * 32
						, m_pEffectList[i]->m_dY * 32
						, &m_pEffectList[i]->m_mX
						, &m_pEffectList[i]->m_mY
						, &m_pEffectList[i]->m_iErr
						, 40);
					// 30 - looks ok, good fps, bit amaturish
					// 31 - looks good, good fps
					// 67 - looks pants, best fps tho
					// 66 - very FPS intensive
					// 5 - looks ok, bit gappy
					// 18 - ENERGY SHOCK WAVE-looking sprite :P
					bAddNewEffect(66, m_pEffectList[i]->m_mX + (rand() % 20) - 10, m_pEffectList[i]->m_mY + (rand() % 20) - 10, 0, 0, 0, 0);
					if (m_pEffectList[i]->m_cFrame >= m_pEffectList[i]->m_cMaxFrame)
					{
						delete m_pEffectList[i];
						m_pEffectList[i] = 0;
					}
					else
					{
						sAbsX = abs(((m_sViewPointX / 32) + 12) - m_pEffectList[i]->m_dX);
						sAbsY = abs(((m_sViewPointY / 32) + 9) - m_pEffectList[i]->m_dY);
						if (sAbsX > sAbsY) sDist = sAbsX - 10;
						else sDist = sAbsY - 10;
						lPan = -(((m_sViewPointX / 32) + 12) - m_pEffectList[i]->m_dX);
						PlaySound('E', 1, sDist, lPan);
					}
					break;
				case 199: // VAMP - Call-Of-The-Gods
					/*m_Misc.GetPoint(m_pEffectList[i]->m_mX
						, m_pEffectList[i]->m_mY
						, m_pEffectList[i]->m_dX * 32
						, m_pEffectList[i]->m_dY * 32
						, &m_pEffectList[i]->m_mX
						, &m_pEffectList[i]->m_mY
						, &m_pEffectList[i]->m_iErr
						, 40);
					if (m_pEffectList[i]->m_cFrame % 2 == 0)
						bAddNewEffect(68, m_pEffectList[i]->m_mX + (rand() % 30) - 15, m_pEffectList[i]->m_mY + (rand() % 30) - 15, 0, 0, 0, 1);

					bAddNewEffect(71, m_pEffectList[i]->m_sX, m_pEffectList[i]->m_sY,
						m_pEffectList[i]->m_dX * 32 + (rand() % 120) - 60, m_pEffectList[i]->m_dY * 32 + (rand() % 120) - 60, 0);
					if (m_pEffectList[i]->m_cFrame >= m_pEffectList[i]->m_cMaxFrame)
					{
						delete m_pEffectList[i];
						m_pEffectList[i] = 0;
					}
					else
					{
						sAbsX = abs(((m_sViewPointX / 32) + 12) - m_pEffectList[i]->m_dX);
						sAbsY = abs(((m_sViewPointY / 32) + 9) - m_pEffectList[i]->m_dY);
						if (sAbsX > sAbsY) sDist = sAbsX - 10;
						else sDist = sAbsY - 10;
						lPan = -(((m_sViewPointX / 32) + 12) - m_pEffectList[i]->m_dX);
						PlaySound('E', 1, sDist, lPan);
					}*/

					break;

				case 6: // Lightning Bolt Burst
					if (m_pEffectList[i]->m_cFrame == 1)
					{
						bAddNewEffect(9, m_pEffectList[i]->m_mX + 5 - (rand() % 10), m_pEffectList[i]->m_mY + 5 - (rand() % 10), 0, 0, -1 * (rand() % 2));
						bAddNewEffect(9, m_pEffectList[i]->m_mX + 5 - (rand() % 10), m_pEffectList[i]->m_mY + 5 - (rand() % 10), 0, 0, -1 * (rand() % 2));
						bAddNewEffect(9, m_pEffectList[i]->m_mX + 5 - (rand() % 10), m_pEffectList[i]->m_mY + 5 - (rand() % 10), 0, 0, -1 * (rand() % 2));
						bAddNewEffect(9, m_pEffectList[i]->m_mX + 5 - (rand() % 10), m_pEffectList[i]->m_mY + 5 - (rand() % 10), 0, 0, -1 * (rand() % 2));
						bAddNewEffect(9, m_pEffectList[i]->m_mX + 5 - (rand() % 10), m_pEffectList[i]->m_mY + 5 - (rand() % 10), 0, 0, -1 * (rand() % 2));
					}
					if (m_pEffectList[i]->m_cFrame >= m_pEffectList[i]->m_cMaxFrame)
					{
						delete m_pEffectList[i];
						m_pEffectList[i] = 0;
					}
					break;

				case 7: // Magic Missile Burst
					if (m_pEffectList[i]->m_cFrame == 1)
					{
						bAddNewEffect(9, m_pEffectList[i]->m_mX + 5 - (rand() % 10), m_pEffectList[i]->m_mY + 5 - (rand() % 10), 0, 0, -1 * (rand() % 2));
						bAddNewEffect(9, m_pEffectList[i]->m_mX + 5 - (rand() % 10), m_pEffectList[i]->m_mY + 5 - (rand() % 10), 0, 0, -1 * (rand() % 2));
						bAddNewEffect(9, m_pEffectList[i]->m_mX + 5 - (rand() % 10), m_pEffectList[i]->m_mY + 5 - (rand() % 10), 0, 0, -1 * (rand() % 2));
					}
					if (m_pEffectList[i]->m_cFrame >= m_pEffectList[i]->m_cMaxFrame)
					{
						delete m_pEffectList[i];
						m_pEffectList[i] = 0;
					}
					break;

				case 9:  // Burst Type 2
				case 11: // Burst Type 3
					m_pEffectList[i]->m_mX += m_pEffectList[i]->m_rX;
					m_pEffectList[i]->m_mY += m_pEffectList[i]->m_rY;
					m_pEffectList[i]->m_rY++;
					if (m_pEffectList[i]->m_cFrame > m_pEffectList[i]->m_cMaxFrame)
					{
						delete m_pEffectList[i];
						m_pEffectList[i] = 0;
					}
					break;

				case 10: // Lightning Arrow Burst
					if (m_pEffectList[i]->m_cFrame == 1)
					{
						bAddNewEffect(9, m_pEffectList[i]->m_mX + 20 - (rand() % 40), m_pEffectList[i]->m_mY + 20 - (rand() % 40), 0, 0, -1 * (rand() % 2));
						bAddNewEffect(9, m_pEffectList[i]->m_mX + 20 - (rand() % 40), m_pEffectList[i]->m_mY + 20 - (rand() % 40), 0, 0, -1 * (rand() % 2));
						bAddNewEffect(9, m_pEffectList[i]->m_mX + 20 - (rand() % 40), m_pEffectList[i]->m_mY + 20 - (rand() % 40), 0, 0, -1*(rand() % 2));
						bAddNewEffect(9, m_pEffectList[i]->m_mX + 20 - (rand() % 40), m_pEffectList[i]->m_mY + 20 - (rand() % 40), 0, 0, -1*(rand() % 2));
						bAddNewEffect(9, m_pEffectList[i]->m_mX + 20 - (rand() % 40), m_pEffectList[i]->m_mY + 20 - (rand() % 40), 0, 0, -1*(rand() % 2));
						bAddNewEffect(9, m_pEffectList[i]->m_mX + 20 - (rand() % 40), m_pEffectList[i]->m_mY + 20 - (rand() % 40), 0, 0, -1*(rand() % 2));
						bAddNewEffect(9, m_pEffectList[i]->m_mX + 20 - (rand() % 40), m_pEffectList[i]->m_mY + 20 - (rand() % 40), 0, 0, -1 * (rand() % 2));
						bAddNewEffect(9, m_pEffectList[i]->m_mX + 20 - (rand() % 40), m_pEffectList[i]->m_mY + 20 - (rand() % 40), 0, 0, -1 * (rand() % 2));
						bAddNewEffect(9, m_pEffectList[i]->m_mX + 20 - (rand() % 40), m_pEffectList[i]->m_mY + 20 - (rand() % 40), 0, 0, -1 * (rand() % 2));
						bAddNewEffect(9, m_pEffectList[i]->m_mX + 20 - (rand() % 40), m_pEffectList[i]->m_mY + 20 - (rand() % 40), 0, 0, -1 * (rand() % 2));
						bAddNewEffect(9, m_pEffectList[i]->m_mX + 20 - (rand() % 40), m_pEffectList[i]->m_mY + 20 - (rand() % 40), 0, 0, -1 * (rand() % 2));
					}
					if (m_pEffectList[i]->m_cFrame >= m_pEffectList[i]->m_cMaxFrame)
					{
						delete m_pEffectList[i];
						m_pEffectList[i] = 0;
					}
					break;

				case 12: // Burst Type 4
					m_pEffectList[i]->m_mX += m_pEffectList[i]->m_rX;
					m_pEffectList[i]->m_mY += m_pEffectList[i]->m_rY;
					if (m_pEffectList[i]->m_cFrame > m_pEffectList[i]->m_cMaxFrame)
					{
						delete m_pEffectList[i];
						m_pEffectList[i] = 0;
					}
					break;

				case 13: // Bulles druncncity
					if (m_pEffectList[i]->m_cFrame < 15)
					{	if ((rand() % 2) == 0)
					m_pEffectList[i]->m_mX++;
					else m_pEffectList[i]->m_mX--;
					m_pEffectList[i]->m_mY--;
			}
					if (m_pEffectList[i]->m_cFrame > m_pEffectList[i]->m_cMaxFrame)
					{
						delete m_pEffectList[i];
						m_pEffectList[i] = 0;
					}
					break;

				case 16: //
					m_Misc.GetPoint(m_pEffectList[i]->m_mX, m_pEffectList[i]->m_mY,
						m_pEffectList[i]->m_dX, m_pEffectList[i]->m_dY,
						&m_pEffectList[i]->m_mX, &m_pEffectList[i]->m_mY,
						&m_pEffectList[i]->m_iErr, 40);
					bAddNewEffect(8, m_pEffectList[i]->m_mX + (rand() % 20) - 10, m_pEffectList[i]->m_mY + (rand() % 20) - 10, 0, 0, -1 * (rand() % 4));
					if ((abs(m_pEffectList[i]->m_mX - m_pEffectList[i]->m_dX) <= 2)
						&& (abs(m_pEffectList[i]->m_mY - (m_pEffectList[i]->m_dY)) <= 2))
					{
						bAddNewEffect(18, m_pEffectList[i]->m_dX, m_pEffectList[i]->m_dY, 0, 0, 0); // testcode 0111 18
						bAddNewEffect(9, m_pEffectList[i]->m_mX + 20 - (rand() % 40), m_pEffectList[i]->m_mY + 20 - (rand() % 40), 0, 0, -1 * (rand() % 2));
						bAddNewEffect(9, m_pEffectList[i]->m_mX + 20 - (rand() % 40), m_pEffectList[i]->m_mY + 20 - (rand() % 40), 0, 0, -1 * (rand() % 2));
						bAddNewEffect(9, m_pEffectList[i]->m_mX + 20 - (rand() % 40), m_pEffectList[i]->m_mY + 20 - (rand() % 40), 0, 0, -1 * (rand() % 2));
						bAddNewEffect(9, m_pEffectList[i]->m_mX + 20 - (rand() % 40), m_pEffectList[i]->m_mY + 20 - (rand() % 40), 0, 0, -1 * (rand() % 2));
						bAddNewEffect(9, m_pEffectList[i]->m_mX + 20 - (rand() % 40), m_pEffectList[i]->m_mY + 20 - (rand() % 40), 0, 0, -1 * (rand() % 2));
						delete m_pEffectList[i];
						m_pEffectList[i] = 0;
					}
					break;

				case 197: // Magn0S:: Fury-of-Thor
					m_Misc.GetPoint(m_pEffectList[i]->m_mX
						, m_pEffectList[i]->m_mY
						, m_pEffectList[i]->m_dX * 32
						, m_pEffectList[i]->m_dY * 32
						, &m_pEffectList[i]->m_mX
						, &m_pEffectList[i]->m_mY
						, &m_pEffectList[i]->m_iErr
						, 40);
					int dX, dY;
					dX = (m_pEffectList[i]->m_mX) - m_sViewPointX;
					dY = (m_pEffectList[i]->m_mY) - m_sViewPointY;

					_DrawThunderEffect(m_pEffectList[i]->m_dX * 32 - m_sViewPointX - 400, m_pEffectList[i]->m_dY * 32 - m_sViewPointY - 800,
						dX, dY,
						m_pEffectList[i]->m_mX + (rand() % 20) - 10, m_pEffectList[i]->m_mY + (rand() % 20) - 10, 1);

					bAddNewEffect(10, m_pEffectList[i]->m_mX + (rand() % 20) - 10, m_pEffectList[i]->m_mY + (rand() % 20) - 10, 0, 0, 0, 0);
					if (m_pEffectList[i]->m_cFrame >= m_pEffectList[i]->m_cMaxFrame)
					{
						delete m_pEffectList[i];
						m_pEffectList[i] = 0;
					}
					else
					{
						sAbsX = abs(((m_sViewPointX / 32) + 12) - m_pEffectList[i]->m_dX);
						sAbsY = abs(((m_sViewPointY / 32) + 9) - m_pEffectList[i]->m_dY);
						if (sAbsX > sAbsY) sDist = sAbsX - 10;
						else sDist = sAbsY - 10;
						lPan = -(((m_sViewPointX / 32) + 12) - m_pEffectList[i]->m_dX);
						PlaySound('E', 1, sDist, lPan);
					}
					break;

				case 17: // Ice-Storm
					cDir = m_Misc.cGetNextMoveDir(m_pEffectList[i]->m_mX, m_pEffectList[i]->m_mY, m_pEffectList[i]->m_mX3, m_pEffectList[i]->m_mY3);
					switch (cDir) {
					case 1:
						m_pEffectList[i]->m_rY -= 2;
						break;
					case 2:
						m_pEffectList[i]->m_rY -= 2;
						m_pEffectList[i]->m_rX += 2;
						break;
					case 3:
						m_pEffectList[i]->m_rX += 2;
						break;
					case 4:
						m_pEffectList[i]->m_rX += 2;
						m_pEffectList[i]->m_rY += 2;
						break;
					case 5:
						m_pEffectList[i]->m_rY += 2;
						break;
					case 6:
						m_pEffectList[i]->m_rX -= 2;
						m_pEffectList[i]->m_rY += 2;
						break;
					case 7:
						m_pEffectList[i]->m_rX -= 2;
						break;
					case 8:
						m_pEffectList[i]->m_rX -= 2;
						m_pEffectList[i]->m_rY -= 2;
						break;
					}
					if (m_pEffectList[i]->m_rX < -10) m_pEffectList[i]->m_rX = -10;
					if (m_pEffectList[i]->m_rX >  10) m_pEffectList[i]->m_rX = 10;
					if (m_pEffectList[i]->m_rY < -10) m_pEffectList[i]->m_rY = -10;
					if (m_pEffectList[i]->m_rY >  10) m_pEffectList[i]->m_rY = 10;
					m_pEffectList[i]->m_mX += m_pEffectList[i]->m_rX;
					m_pEffectList[i]->m_mY += m_pEffectList[i]->m_rY;
					m_pEffectList[i]->m_mY3--;
					if (m_pEffectList[i]->m_cFrame > 10)
					{
						m_pEffectList[i]->m_cFrame = 0;
						if (abs(m_pEffectList[i]->m_sY - m_pEffectList[i]->m_mY3) > 100)
						{
							delete m_pEffectList[i];
							m_pEffectList[i] = 0;
						}
					}
					break;

				case 20: // Critical strike with a weapon
				case 21:
				case 22:
				case 23:
				case 24:
				case 25:
				case 26:
				case 27: // Critical strike with a weapon
					m_Misc.GetPoint(m_pEffectList[i]->m_mX, m_pEffectList[i]->m_mY,
						m_pEffectList[i]->m_dX * 32, m_pEffectList[i]->m_dY * 32 - 40,
						&m_pEffectList[i]->m_mX, &m_pEffectList[i]->m_mY,
						&m_pEffectList[i]->m_iErr, 50);
					bAddNewEffect(8, m_pEffectList[i]->m_mX + 10 - (rand() % 20), m_pEffectList[i]->m_mY + 10 - (rand() % 20), 0, 0, 0);//-1*(rand() % 4));
					bAddNewEffect(8, m_pEffectList[i]->m_mX + 10 - (rand() % 20), m_pEffectList[i]->m_mY + 10 - (rand() % 20), 0, 0, 0);//-1*(rand() % 4));
					bAddNewEffect(8, m_pEffectList[i]->m_mX + 10 - (rand() % 20), m_pEffectList[i]->m_mY + 10 - (rand() % 20), 0, 0, 0);//-1*(rand() % 4));
					bAddNewEffect(8, m_pEffectList[i]->m_mX + 10 - (rand() % 20), m_pEffectList[i]->m_mY + 10 - (rand() % 20), 0, 0, 0);//-1*(rand() % 4));
					bAddNewEffect(8, m_pEffectList[i]->m_mX + 10 - (rand() % 20), m_pEffectList[i]->m_mY + 10 - (rand() % 20), 0, 0, 0);//-1*(rand() % 4));
					if ((abs(m_pEffectList[i]->m_mX - m_pEffectList[i]->m_dX * 32) <= 2) &&
						(abs(m_pEffectList[i]->m_mY - (m_pEffectList[i]->m_dY * 32 - 40)) <= 2))
					{
						delete m_pEffectList[i];
						m_pEffectList[i] = 0;
					}
					break;

				case 34: //
					m_Misc.GetPoint(m_pEffectList[i]->m_mX, m_pEffectList[i]->m_mY,
						m_pEffectList[i]->m_dX, m_pEffectList[i]->m_dY,
						&m_pEffectList[i]->m_mX, &m_pEffectList[i]->m_mY,
						&m_pEffectList[i]->m_iErr, 50);
					bAddNewEffect(33, m_pEffectList[i]->m_mX + (rand() % 30) - 15, m_pEffectList[i]->m_mY + (rand() % 30) - 15, 0, 0, -1 * (rand() % 4));
					if ((abs(m_pEffectList[i]->m_mX - m_pEffectList[i]->m_dX) <= 2) &&
						(abs(m_pEffectList[i]->m_mY - (m_pEffectList[i]->m_dY)) <= 2))
					{
						bAddNewEffect(33, m_pEffectList[i]->m_dX, m_pEffectList[i]->m_dY, 0, 0, 0); //7
						delete m_pEffectList[i];
						m_pEffectList[i] = 0;
					}
					break;


				case 40:
				case 56:
					if (m_pEffectList[i]->m_cFrame == 9)
					{
						bAddNewEffect(51, m_pEffectList[i]->m_mX + ((rand() % 100) - 50), m_pEffectList[i]->m_mY + ((rand() % 70) - 35), 0, 0, 0, 0);
						bAddNewEffect(51, m_pEffectList[i]->m_mX + ((rand() % 100) - 50), m_pEffectList[i]->m_mY + ((rand() % 70) - 35), 0, 0, 0, 0);
						bAddNewEffect(51, m_pEffectList[i]->m_mX + ((rand() % 100) - 50), m_pEffectList[i]->m_mY + ((rand() % 70) - 35), 0, 0, 0, 0);
						bAddNewEffect(51, m_pEffectList[i]->m_mX +((rand() % 100)-50), m_pEffectList[i]->m_mY +((rand() % 70)-35), 0, 0, 0, 0);
						bAddNewEffect(51, m_pEffectList[i]->m_mX +((rand() % 100)-50), m_pEffectList[i]->m_mY +((rand() % 70)-35), 0, 0, 0, 0);
					}
					if (m_pEffectList[i]->m_cFrame > m_pEffectList[i]->m_cMaxFrame)
					{
						delete m_pEffectList[i];
						m_pEffectList[i] = 0;
					}
					break;

				case 41: //Large Type 1, 2, 3, 4
				case 42:
				case 43:
				case 44:
				case 45: // Small Type 1, 2
				case 46:
					if (m_pEffectList[i]->m_cFrame >= 7)
					{
						m_pEffectList[i]->m_mX--;
						m_pEffectList[i]->m_mY += m_pEffectList[i]->m_iV1;
						m_pEffectList[i]->m_iV1++;
					}

					if (m_pEffectList[i]->m_cFrame > m_pEffectList[i]->m_cMaxFrame)
					{
						if ((m_pEffectList[i]->m_sType != 45) && (m_pEffectList[i]->m_sType != 46))
						{
							bAddNewEffect(50, m_pEffectList[i]->m_mX, m_pEffectList[i]->m_mY, 0, 0, 0);
							bAddNewEffect(14, m_pEffectList[i]->m_mX + ((rand() % 20) - 10), m_pEffectList[i]->m_mY + ((rand() % 20) - 10), 0, 0, 0, 0);
							bAddNewEffect(14, m_pEffectList[i]->m_mX + ((rand() % 20) - 10), m_pEffectList[i]->m_mY + ((rand() % 20) - 10), 0, 0, 0, 0);
							bAddNewEffect(14, m_pEffectList[i]->m_mX + ((rand() % 20) - 10), m_pEffectList[i]->m_mY + ((rand() % 20) - 10), 0, 0, 0, 0);
							bAddNewEffect(51, m_pEffectList[i]->m_mX + ((rand() % 20) - 10), m_pEffectList[i]->m_mY + ((rand() % 20) - 10), 0, 0, 0, 0);
							bAddNewEffect(51, m_pEffectList[i]->m_mX + ((rand() % 20) - 10), m_pEffectList[i]->m_mY + ((rand() % 20) - 10), 0, 0, 0, 0);
						}
						delete m_pEffectList[i];
						m_pEffectList[i] = 0;
					}
					break;

				case 47: // Blizzard
				case 48:
				case 49:
					if (m_pEffectList[i]->m_cFrame >= 7)
					{
						m_pEffectList[i]->m_mX--;
						m_pEffectList[i]->m_mY += m_pEffectList[i]->m_iV1;
						m_pEffectList[i]->m_iV1 += 4;
					}
					if (m_pEffectList[i]->m_cFrame > m_pEffectList[i]->m_cMaxFrame)
					{
						if (m_pEffectList[i]->m_sType == 49)
							bAddNewEffect(72, m_pEffectList[i]->m_mX, m_pEffectList[i]->m_mY, 0, 0, 0);
						else bAddNewEffect(50, m_pEffectList[i]->m_mX, m_pEffectList[i]->m_mY, 0, 0, 0);
						bAddNewEffect(14, m_pEffectList[i]->m_mX + ((rand() % 20) - 10), m_pEffectList[i]->m_mY + ((rand() % 20) - 10), 0, 0, 0, 0);
						bAddNewEffect(14, m_pEffectList[i]->m_mX + ((rand() % 20) - 10), m_pEffectList[i]->m_mY + ((rand() % 20) - 10), 0, 0, 0, 0);
						bAddNewEffect(14, m_pEffectList[i]->m_mX + ((rand() % 20) - 10), m_pEffectList[i]->m_mY + ((rand() % 20) - 10), 0, 0, 0, 0);

						bAddNewEffect(51, m_pEffectList[i]->m_mX + ((rand() % 20) - 10), m_pEffectList[i]->m_mY + ((rand() % 20) - 10), 0, 0, 0, 0);
						bAddNewEffect(51, m_pEffectList[i]->m_mX + ((rand() % 20) - 10), m_pEffectList[i]->m_mY + ((rand() % 20) - 10), 0, 0, 0, 0);
						delete m_pEffectList[i];
						m_pEffectList[i] = 0;
					}
					break;

				case 60: //
				case 181: // Meteor-Strike
					if (m_pEffectList[i]->m_cFrame > m_pEffectList[i]->m_cMaxFrame)
					{
						bAddNewEffect(61, m_pEffectList[i]->m_mX, m_pEffectList[i]->m_mY, 0, 0, 0, 0);
						bAddNewEffect(63, m_pEffectList[i]->m_mX, m_pEffectList[i]->m_mY, 0, 0, 0, 0);
						bAddNewEffect(12, m_pEffectList[i]->m_mX + 5 - (rand() % 10), m_pEffectList[i]->m_mY + 5 - (rand() % 10), 0, 0, -1 * (rand() % 2));
						bAddNewEffect(12, m_pEffectList[i]->m_mX + 5 - (rand() % 10), m_pEffectList[i]->m_mY + 5 - (rand() % 10), 0, 0, -1 * (rand() % 2));
						bAddNewEffect(12, m_pEffectList[i]->m_mX + 5 - (rand() % 10), m_pEffectList[i]->m_mY + 5 - (rand() % 10), 0, 0, -1 * (rand() % 2));
						bAddNewEffect(12, m_pEffectList[i]->m_mX + 5 - (rand() % 10), m_pEffectList[i]->m_mY + 5 - (rand() % 10), 0, 0, -1 * (rand() % 2));
						bAddNewEffect(12, m_pEffectList[i]->m_mX + 5 - (rand() % 10), m_pEffectList[i]->m_mY + 5 - (rand() % 10), 0, 0, -1 * (rand() % 2));
						delete m_pEffectList[i];
						m_pEffectList[i] = 0;
					}
					else if (m_pEffectList[i]->m_cFrame >= 0)
					{
						m_pEffectList[i]->m_mX -= 30;
						m_pEffectList[i]->m_mY += 46;
						bAddNewEffect(62, m_pEffectList[i]->m_mX, m_pEffectList[i]->m_mY, 0, 0, 0, 0);
					}
					break;

				case 62:
					if (m_pEffectList[i]->m_cFrame > m_pEffectList[i]->m_cMaxFrame)
					{
						delete m_pEffectList[i];
						m_pEffectList[i] = 0;
					}
					else if (m_pEffectList[i]->m_cFrame >= 0)
					{
						m_pEffectList[i]->m_mX += (rand() % 3) - 1;
						m_pEffectList[i]->m_mY += (rand() % 3) - 1;
					}
					break;

				case 65: // Building fire after MS (crusade) 65 & 67
					if (m_pEffectList[i]->m_cFrame > m_pEffectList[i]->m_cMaxFrame)
					{
						delete m_pEffectList[i];
						m_pEffectList[i] = 0;
					}else if (m_pEffectList[i]->m_cFrame >= 0)
					{
						m_pEffectList[i]->m_mX += (rand() % 3) - 1;
						m_pEffectList[i]->m_mY -= 4 + (rand() % 2);
					}
					break;

				case 66:
				case 203:
				case 204:
				case 205:
				case 206:
					if (m_pEffectList[i]->m_cFrame > m_pEffectList[i]->m_cMaxFrame)
					{
						delete m_pEffectList[i];
						m_pEffectList[i] = 0;
					}
					break;

				case 68:
					if (m_pEffectList[i]->m_cFrame > m_pEffectList[i]->m_cMaxFrame)
					{
						delete m_pEffectList[i];
						m_pEffectList[i] = 0;
					}
					else if (m_pEffectList[i]->m_cFrame == 11)
					{
						SetCameraShakingEffect(m_pEffectList[i]->m_iV1, 2);
					}
					break;

				case 71:
					m_Misc.GetPoint(m_pEffectList[i]->m_mX, m_pEffectList[i]->m_mY,
						m_pEffectList[i]->m_dX, m_pEffectList[i]->m_dY,
						&m_pEffectList[i]->m_mX, &m_pEffectList[i]->m_mY,
						&m_pEffectList[i]->m_iErr, 50);
					bAddNewEffect(48, m_pEffectList[i]->m_mX + (rand() % 30) - 15, m_pEffectList[i]->m_mY + (rand() % 30) - 15, 0, 0, 0);
					bAddNewEffect(51, m_pEffectList[i]->m_mX + ((rand() % 20) - 10), m_pEffectList[i]->m_mY + ((rand() % 20) - 10), 0, 0, 0, 0);
					if ((abs(m_pEffectList[i]->m_mX - m_pEffectList[i]->m_dX) <= 2) &&
						(abs(m_pEffectList[i]->m_mY - (m_pEffectList[i]->m_dY)) <= 2))
					{
						bAddNewEffect(49, m_pEffectList[i]->m_mX, m_pEffectList[i]->m_mY, 0, 0, 0);
						delete m_pEffectList[i];
						m_pEffectList[i] = 0;
					}
					break;

				case 81: // Snoopy: Ajout StromBlade
					m_Misc.GetPoint(m_pEffectList[i]->m_mX
						, m_pEffectList[i]->m_mY
						, m_pEffectList[i]->m_dX * 32
						, m_pEffectList[i]->m_dY * 32
						, &m_pEffectList[i]->m_mX
						, &m_pEffectList[i]->m_mY
						, &m_pEffectList[i]->m_iErr
						, 10);
					if (m_pEffectList[i]->m_cFrame > m_pEffectList[i]->m_cMaxFrame)
					{
						delete m_pEffectList[i];
						m_pEffectList[i] = 0;
					}
					break;

				case 100: // Magic Missile
					m_Misc.GetPoint(m_pEffectList[i]->m_mX, m_pEffectList[i]->m_mY,
						m_pEffectList[i]->m_dX * 32, m_pEffectList[i]->m_dY * 32,
						&m_pEffectList[i]->m_mX, &m_pEffectList[i]->m_mY,
						&m_pEffectList[i]->m_iErr, 50);
					bAddNewEffect(8, m_pEffectList[i]->m_mX + (rand() % 20) - 10, m_pEffectList[i]->m_mY + (rand() % 20) - 10, 0, 0, -1 * (rand() % 4));

					if ((abs(m_pEffectList[i]->m_mX - m_pEffectList[i]->m_dX * 32) <= 2) &&
						(abs(m_pEffectList[i]->m_mY - (m_pEffectList[i]->m_dY * 32)) <= 2))
					{
						bAddNewEffect(7, m_pEffectList[i]->m_dX * 32, m_pEffectList[i]->m_dY * 32, 0, 0, 0);
						delete m_pEffectList[i];
						m_pEffectList[i] = 0;
					}
					break;

				case 110: // Enegy-Bolt
					m_Misc.GetPoint(m_pEffectList[i]->m_mX, m_pEffectList[i]->m_mY,
						m_pEffectList[i]->m_dX * 32, m_pEffectList[i]->m_dY * 32,
						&m_pEffectList[i]->m_mX, &m_pEffectList[i]->m_mY,
						&m_pEffectList[i]->m_iErr, 50);
					bAddNewEffect(8, m_pEffectList[i]->m_mX + (rand() % 20) - 10, m_pEffectList[i]->m_mY + (rand() % 20) - 10, 0, 0, -1 * (rand() % 4));
					bAddNewEffect(8, m_pEffectList[i]->m_mX + (rand() % 20) - 10, m_pEffectList[i]->m_mY + (rand() % 20) - 10, 0, 0, -1 * (rand() % 4));
					if ((abs(m_pEffectList[i]->m_mX - m_pEffectList[i]->m_dX * 32) <= 2)
						&& (abs(m_pEffectList[i]->m_mY - m_pEffectList[i]->m_dY * 32) <= 2))
					{
						bAddNewEffect(6, m_pEffectList[i]->m_dX * 32, m_pEffectList[i]->m_dY * 32, 0, 0, 0); // 6 testcode 0111
						delete m_pEffectList[i];
						m_pEffectList[i] = 0;
					}
					break;

				case 120: // Fire Ball
					m_Misc.GetPoint(m_pEffectList[i]->m_mX, m_pEffectList[i]->m_mY,
						m_pEffectList[i]->m_dX * 32, m_pEffectList[i]->m_dY * 32,
						&m_pEffectList[i]->m_mX, &m_pEffectList[i]->m_mY,
						&m_pEffectList[i]->m_iErr, 50);
					if ((abs(m_pEffectList[i]->m_mX - m_pEffectList[i]->m_dX * 32) <= 2)
						&& (abs(m_pEffectList[i]->m_mY - m_pEffectList[i]->m_dY * 32) <= 2))
					{
						bAddNewEffect(5, m_pEffectList[i]->m_dX * 32, m_pEffectList[i]->m_dY * 32, 0, 0, 0);
						delete m_pEffectList[i];
						m_pEffectList[i] = 0;
					}
					break;

				case 130: // Fire Strike
					m_Misc.GetPoint(m_pEffectList[i]->m_mX, m_pEffectList[i]->m_mY,
						m_pEffectList[i]->m_dX * 32, m_pEffectList[i]->m_dY * 32,
						&m_pEffectList[i]->m_mX, &m_pEffectList[i]->m_mY,
						&m_pEffectList[i]->m_iErr, 50);
					if ((abs(m_pEffectList[i]->m_mX - m_pEffectList[i]->m_dX * 32) <= 2)
						&& (abs(m_pEffectList[i]->m_mY - m_pEffectList[i]->m_dY * 32) <= 2))
					{
						bAddNewEffect(5, m_pEffectList[i]->m_dX * 32, m_pEffectList[i]->m_dY * 32, 0, 0, 0);
						bAddNewEffect(5, m_pEffectList[i]->m_dX * 32 - 30, m_pEffectList[i]->m_dY * 32 - 15, 0, 0, -7);
						bAddNewEffect(5, m_pEffectList[i]->m_dX * 32 + 35, m_pEffectList[i]->m_dY * 32 - 30, 0, 0, -5);
						bAddNewEffect(5, m_pEffectList[i]->m_dX * 32 + 20, m_pEffectList[i]->m_dY * 32 + 30, 0, 0, -3);
						delete m_pEffectList[i];
						m_pEffectList[i] = 0;
					}
					break;

				case 137: // Lightning Arrow
					m_Misc.GetPoint(m_pEffectList[i]->m_mX, m_pEffectList[i]->m_mY,
						m_pEffectList[i]->m_dX * 32, m_pEffectList[i]->m_dY * 32,
						&m_pEffectList[i]->m_mX, &m_pEffectList[i]->m_mY,
						&m_pEffectList[i]->m_iErr, 50);
					bAddNewEffect(8, m_pEffectList[i]->m_mX + (rand() % 20) - 10, m_pEffectList[i]->m_mY + (rand() % 20) - 10, 0, 0, -1 * (rand() % 4));
					bAddNewEffect(8, m_pEffectList[i]->m_mX + (rand() % 20) - 10, m_pEffectList[i]->m_mY + (rand() % 20) - 10, 0, 0, -1 * (rand() % 4));
					bAddNewEffect(8, m_pEffectList[i]->m_mX + (rand() % 20) - 10, m_pEffectList[i]->m_mY + (rand() % 20) - 10, 0, 0, -1 * (rand() % 4));
					if ((abs(m_pEffectList[i]->m_mX - m_pEffectList[i]->m_dX * 32) <= 2)
						&& (abs(m_pEffectList[i]->m_mY - m_pEffectList[i]->m_dY * 32) <= 2))
					{
						bAddNewEffect(10, m_pEffectList[i]->m_dX * 32, m_pEffectList[i]->m_dY * 32, 0, 0, 0);
						delete m_pEffectList[i];
						m_pEffectList[i] = 0;
					}
					break;

				case 143: // Lightning
				case 151: // Lightning-Bolt
					if (m_pEffectList[i]->m_cFrame > m_pEffectList[i]->m_cMaxFrame)
					{
						bAddNewEffect(10, m_pEffectList[i]->m_dX * 32, m_pEffectList[i]->m_dY * 32, 0, 0, 0);
						delete m_pEffectList[i];
						m_pEffectList[i] = 0;
					}
					else
					{
						m_pEffectList[i]->m_rX = 5 - (rand() % 10);
						m_pEffectList[i]->m_rY = 5 - (rand() % 10);
					}
					break;

				case 145: // Chill-Wind
					bAddNewEffect(40, m_pEffectList[i]->m_dX * 32, m_pEffectList[i]->m_dY * 32, 0, 0, 0);
					bAddNewEffect(40, m_pEffectList[i]->m_dX * 32 - 30, m_pEffectList[i]->m_dY * 32 - 15, 0, 0, -10);
					bAddNewEffect(40, m_pEffectList[i]->m_dX * 32 + 35, m_pEffectList[i]->m_dY * 32 - 30, 0, 0, -6);
					bAddNewEffect(40, m_pEffectList[i]->m_dX * 32 + 20, m_pEffectList[i]->m_dY * 32 + 30, 0, 0, -3);
					delete m_pEffectList[i];
					m_pEffectList[i] = 0;
					break;

				case 147:  // Triple-Energy-Bolt
					bAddNewEffect(110, m_pEffectList[i]->m_sX, m_pEffectList[i]->m_sY, m_pEffectList[i]->m_dX - 1, m_pEffectList[i]->m_dY - 1, 0);
					bAddNewEffect(110, m_pEffectList[i]->m_sX, m_pEffectList[i]->m_sY, m_pEffectList[i]->m_dX + 1, m_pEffectList[i]->m_dY - 1, 0);
					bAddNewEffect(110, m_pEffectList[i]->m_sX, m_pEffectList[i]->m_sY, m_pEffectList[i]->m_dX + 1, m_pEffectList[i]->m_dY + 1, 0);
					bAddNewEffect(8, m_pEffectList[i]->m_mX + (rand() % 20) - 10, m_pEffectList[i]->m_mY + (rand() % 20) - 10, 0, 0, -1 * (rand() % 4));
					lPan = -(((m_sViewPointX / 32) + 12) - m_pEffectList[i]->m_dX) * 1000;
					PlaySound('E', 1, sDist, lPan);
					bAddNewEffect(7, m_pEffectList[i]->m_dX * 32, m_pEffectList[i]->m_dY * 32, 0, 0, 0);
					delete m_pEffectList[i];
					m_pEffectList[i] = 0;
					break;

				case 156: // Mass-Lightning-Arrow
					if (m_pEffectList[i]->m_cFrame > m_pEffectList[i]->m_cMaxFrame)
					{
						delete m_pEffectList[i];
						m_pEffectList[i] = 0;
					}
					else
					{
						bAddNewEffect(137, m_pEffectList[i]->m_sX, m_pEffectList[i]->m_sY,
							m_pEffectList[i]->m_dX, m_pEffectList[i]->m_dY, 0);
#ifdef RES_HIGH
						sAbsX = abs(((m_sViewPointX / 32) + 12) - m_pEffectList[i]->m_dX);
						sAbsY = abs(((m_sViewPointY / 32) + 9) - m_pEffectList[i]->m_dY);
#else
						sAbsX = abs(((m_sViewPointX / 32) + 10) - m_pEffectList[i]->m_dX);
						sAbsY = abs(((m_sViewPointY / 32) + 7) - m_pEffectList[i]->m_dY);
#endif
						if (sAbsX > sAbsY) sDist = sAbsX;
						else sDist = sAbsY;
						lPan = -(((m_sViewPointX / 32) + 12) - m_pEffectList[i]->m_dX) * 1000;
						PlaySound('E', 1, sDist, lPan);
					}
					break;

				case 157: // Ice-Strike
					bAddNewEffect(41, m_pEffectList[i]->m_dX * 32, m_pEffectList[i]->m_dY * 32, 0, 0, 0);
					for (x = 0; x < 14; x++)
					{
						bAddNewEffect(41 + (rand() % 3), m_pEffectList[i]->m_dX * 32 + (rand() % 100) - 50 + 10, m_pEffectList[i]->m_dY * 32 + (rand() % 90) - 45, 0, 0, -1 * x - 1);
					}
					for (x = 0; x < 6; x++)
					{
						bAddNewEffect(45 + (rand() % 2), m_pEffectList[i]->m_dX * 32 + (rand() % 100) - 50 + 10, m_pEffectList[i]->m_dY * 32 + (rand() % 90) - 45, 0, 0, -1 * x - 1 - 10);
					}
					delete m_pEffectList[i];
					m_pEffectList[i] = 0;
					break;

				case 160: // Energy-Strike
					if (m_pEffectList[i]->m_cFrame > m_pEffectList[i]->m_cMaxFrame)
					{
						delete m_pEffectList[i];
						m_pEffectList[i] = 0;
					}
					else
					{
						bAddNewEffect(16, m_pEffectList[i]->m_sX, m_pEffectList[i]->m_sY,
							m_pEffectList[i]->m_dX * 32 + 50 - (rand() % 100), m_pEffectList[i]->m_dY * 32 + 50 - (rand() % 100), 0);
#ifdef RES_HIGH
						sAbsX = abs(((m_sViewPointX / 32) + 12) - m_pEffectList[i]->m_dX);
						sAbsY = abs(((m_sViewPointY / 32) + 9) - m_pEffectList[i]->m_dY);
#else
						sAbsX = abs(((m_sViewPointX / 32) + 10) - m_pEffectList[i]->m_dX);
						sAbsY = abs(((m_sViewPointY / 32) + 7) - m_pEffectList[i]->m_dY);
#endif
						if (sAbsX > sAbsY) sDist = sAbsX;
						else sDist = sAbsY;
						lPan = -(((m_sViewPointX / 32) + 12) - m_pEffectList[i]->m_dX);
						PlaySound('E', 1, sDist, lPan);
					}
					break;

				case 161: // Mass-Fire-Strike
					m_Misc.GetPoint(m_pEffectList[i]->m_mX, m_pEffectList[i]->m_mY,
						m_pEffectList[i]->m_dX * 32, m_pEffectList[i]->m_dY * 32,
						&m_pEffectList[i]->m_mX, &m_pEffectList[i]->m_mY,
						&m_pEffectList[i]->m_iErr, 50);
					if ((abs(m_pEffectList[i]->m_mX - m_pEffectList[i]->m_dX * 32) <= 2)
						&& (abs(m_pEffectList[i]->m_mY - m_pEffectList[i]->m_dY * 32) <= 2))
					{
						bAddNewEffect(30, m_pEffectList[i]->m_dX * 32, m_pEffectList[i]->m_dY * 32, 0, 0, 0);
						bAddNewEffect(31, m_pEffectList[i]->m_dX * 32 - 30, m_pEffectList[i]->m_dY * 32 - 15, 0, 0, -7);
						bAddNewEffect(31, m_pEffectList[i]->m_dX * 32 + 35, m_pEffectList[i]->m_dY * 32 - 30, 0, 0, -5);
						bAddNewEffect(31, m_pEffectList[i]->m_dX * 32 + 20, m_pEffectList[i]->m_dY * 32 + 30, 0, 0, -3);
						delete m_pEffectList[i];
						m_pEffectList[i] = 0;
					}
					break;

				case 163: // Mass-Chill-Wind Chill-Wind
					bAddNewEffect(56, m_pEffectList[i]->m_dX * 32, m_pEffectList[i]->m_dY * 32, 0, 0, 0);
					bAddNewEffect(56, m_pEffectList[i]->m_dX * 32 - 30, m_pEffectList[i]->m_dY * 32 - 15, 0, 0, -10);
					bAddNewEffect(56, m_pEffectList[i]->m_dX * 32 + 35, m_pEffectList[i]->m_dY * 32 - 30, 0, 0, -6);
					bAddNewEffect(56, m_pEffectList[i]->m_dX * 32 + 20, m_pEffectList[i]->m_dY * 32 + 30, 0, 0, -3);
					bAddNewEffect(56, m_pEffectList[i]->m_dX * 32 + (rand() % 100) - 50, m_pEffectList[i]->m_dY * 32 + (rand() % 70) - 35, 0, 0, -1 * (rand() % 10));
					bAddNewEffect(56, m_pEffectList[i]->m_dX * 32 + (rand() % 100) - 50, m_pEffectList[i]->m_dY * 32 + (rand() % 70) - 35, 0, 0, -1 * (rand() % 10));
					bAddNewEffect(56, m_pEffectList[i]->m_dX * 32 + (rand() % 100) - 50, m_pEffectList[i]->m_dY * 32 + (rand() % 70) - 35, 0, 0, -1 * (rand() % 10));
					bAddNewEffect(56, m_pEffectList[i]->m_dX * 32 + (rand() % 100) - 50, m_pEffectList[i]->m_dY * 32 + (rand() % 70) - 35, 0, 0, -1 * (rand() % 10));
					delete m_pEffectList[i];
					m_pEffectList[i] = 0;
					break;

				case 164: // worm-bite
					if (m_pEffectList[i]->m_cFrame > m_pEffectList[i]->m_cMaxFrame)
					{
						bAddNewEffect(68, m_pEffectList[i]->m_dX * 32, m_pEffectList[i]->m_dY * 32, 0, 0, 0); // testcode 0111 18
						delete m_pEffectList[i];
						m_pEffectList[i] = 0;
					}
					break;

				case 170: // Bloody-Shock-Wave
					if (m_pEffectList[i]->m_cFrame > m_pEffectList[i]->m_cMaxFrame)
					{
						delete m_pEffectList[i];
						m_pEffectList[i] = 0;
					}
					else if ((m_pEffectList[i]->m_cFrame % 2) == 0)
					{
						bAddNewEffect(34, m_pEffectList[i]->m_sX, m_pEffectList[i]->m_sY,
							m_pEffectList[i]->m_dX * 32 + 30 - (rand() % 60), m_pEffectList[i]->m_dY * 32 + 30 - (rand() % 60), 0);
#ifdef RES_HIGH
						sAbsX = abs(((m_sViewPointX / 32) + 12) - m_pEffectList[i]->m_dX);
						sAbsY = abs(((m_sViewPointY / 32) + 9) - m_pEffectList[i]->m_dY);
#else
						sAbsX = abs(((m_sViewPointX / 32) + 10) - m_pEffectList[i]->m_dX);
						sAbsY = abs(((m_sViewPointY / 32) + 7) - m_pEffectList[i]->m_dY);
#endif
						if (sAbsX > sAbsY) sDist = sAbsX;
						else sDist = sAbsY;
						lPan = -(((m_sViewPointX / 32) + 12) - m_pEffectList[i]->m_dX);
						PlaySound('E', 1, sDist, lPan);
					}
					break;

				case 172: // Mass-Ice-Strike
					bAddNewEffect(44, m_pEffectList[i]->m_dX * 32, m_pEffectList[i]->m_dY * 32, 0, 0, 0);
					bAddNewEffect(44, m_pEffectList[i]->m_dX * 32 + (rand() % 110) - 55 + 10, m_pEffectList[i]->m_dY * 32 + (rand() % 100) - 50, 0, 0, -1 * (rand() % 3));
					bAddNewEffect(44, m_pEffectList[i]->m_dX * 32 + (rand() % 110) - 55 + 10, m_pEffectList[i]->m_dY * 32 + (rand() % 100) - 50, 0, 0, -1 * (rand() % 3));
					bAddNewEffect(44, m_pEffectList[i]->m_dX * 32 + (rand() % 110) - 55 + 10, m_pEffectList[i]->m_dY * 32 + (rand() % 100) - 50, 0, 0, -1 * (rand() % 3));
					bAddNewEffect(44, m_pEffectList[i]->m_dX * 32 + (rand() % 110) - 55 + 10, m_pEffectList[i]->m_dY * 32 + (rand() % 100) - 50, 0, 0, -1 * (rand() % 3));
					bAddNewEffect(44, m_pEffectList[i]->m_dX * 32 + (rand() % 110) - 55 + 10, m_pEffectList[i]->m_dY * 32 + (rand() % 100) - 50, 0, 0, -1 * (rand() % 3));
					bAddNewEffect(44, m_pEffectList[i]->m_dX * 32 + (rand() % 110) - 55 + 10, m_pEffectList[i]->m_dY * 32 + (rand() % 100) - 50, 0, 0, -1 * (rand() % 3));
					bAddNewEffect(44, m_pEffectList[i]->m_dX * 32 + (rand() % 110) - 55 + 10, m_pEffectList[i]->m_dY * 32 + (rand() % 100) - 50, 0, 0, -1 * (rand() % 3));
					bAddNewEffect(44, m_pEffectList[i]->m_dX * 32 + (rand() % 110) - 55 + 10, m_pEffectList[i]->m_dY * 32 + (rand() % 100) - 50, 0, 0, -1 * (rand() % 3));
					for (x = 0; x < 16; x++)
					{
						bAddNewEffect(44, m_pEffectList[i]->m_dX * 32 + (rand() % 110) - 55 + 10, m_pEffectList[i]->m_dY * 32 + (rand() % 100) - 50, 0, 0, -1 * x - 1);
					}
					for (x = 0; x < 8; x++)
					{
						bAddNewEffect(45 + (rand() % 2), m_pEffectList[i]->m_dX * 32 + (rand() % 100) - 50 + 10, m_pEffectList[i]->m_dY * 32 + (rand() % 90) - 45, 0, 0, -1 * x - 1 - 10);
					}
					delete m_pEffectList[i];
					m_pEffectList[i] = 0;
					break;

				case 174: // Lightning-Strike
					if (m_pEffectList[i]->m_cFrame > m_pEffectList[i]->m_cMaxFrame)
					{
						delete m_pEffectList[i];
						m_pEffectList[i] = 0;
					}
					else
					{
						bAddNewEffect(151, m_pEffectList[i]->m_sX, m_pEffectList[i]->m_sY,
							m_pEffectList[i]->m_dX + (rand() % 3) - 1, m_pEffectList[i]->m_dY + (rand() % 3) - 1, 0);
#ifdef RES_HIGH
						sAbsX = abs(((m_sViewPointX / 32) + 12) - m_pEffectList[i]->m_dX);
						sAbsY = abs(((m_sViewPointY / 32) + 9) - m_pEffectList[i]->m_dY);
#else
						sAbsX = abs(((m_sViewPointX / 32) + 10) - m_pEffectList[i]->m_dX);
						sAbsY = abs(((m_sViewPointY / 32) + 7) - m_pEffectList[i]->m_dY);
#endif
						if (sAbsX > sAbsY) sDist = sAbsX;
						else sDist = sAbsY;
						lPan = -(((m_sViewPointX / 32) + 12) - m_pEffectList[i]->m_dX);
						PlaySound('E', 1, sDist, lPan);
					}
					break;

				case 182: // Mass-Magic-Missile
					m_Misc.GetPoint(m_pEffectList[i]->m_mX
						, m_pEffectList[i]->m_mY
						, m_pEffectList[i]->m_dX * 32
						, m_pEffectList[i]->m_dY * 32
						, &m_pEffectList[i]->m_mX
						, &m_pEffectList[i]->m_mY
						, &m_pEffectList[i]->m_iErr
						, 50);
					bAddNewEffect(8, m_pEffectList[i]->m_mX + (rand() % 20) - 10, m_pEffectList[i]->m_mY + (rand() % 20) - 10, 0, 0, -1 * (rand() % 4));
					if ((abs(m_pEffectList[i]->m_mX - m_pEffectList[i]->m_dX * 32) <= 2)
						&& (abs(m_pEffectList[i]->m_mY - m_pEffectList[i]->m_dY * 32) <= 2))
					{	// JLE 0043132A
						delete m_pEffectList[i];
						m_pEffectList[i] = 0;
					}
					else
					{
						bAddNewEffect(35, m_pEffectList[i]->m_dX * 32 + 22, m_pEffectList[i]->m_dY * 32 - 15, 0, 0, -7, 1);
						bAddNewEffect(36, m_pEffectList[i]->m_dX * 32 - 22, m_pEffectList[i]->m_dY * 32 - 7, 0, 0, -7, 1);
						bAddNewEffect(36, m_pEffectList[i]->m_dX * 32 + 30, m_pEffectList[i]->m_dY * 32 - 22, 0, 0, -5, 1);
						bAddNewEffect(36, m_pEffectList[i]->m_dX * 32 + 12, m_pEffectList[i]->m_dY * 32 + 22, 0, 0, -3, 1);
					}
					break;

				case 191: // Blizzard
					if (m_pEffectList[i]->m_cFrame > m_pEffectList[i]->m_cMaxFrame)
					{
						delete m_pEffectList[i];
						m_pEffectList[i] = 0;
					}
					else
					{
						bAddNewEffect(71, m_pEffectList[i]->m_sX, m_pEffectList[i]->m_sY,
							m_pEffectList[i]->m_dX * 32 + (rand() % 120) - 60, m_pEffectList[i]->m_dY * 32 + (rand() % 120) - 60, 0);
#ifdef RES_HIGH
						sAbsX = abs(((m_sViewPointX / 32) + 12) - m_pEffectList[i]->m_dX);
						sAbsY = abs(((m_sViewPointY / 32) + 9) - m_pEffectList[i]->m_dY);
#else
						sAbsX = abs(((m_sViewPointX / 32) + 10) - m_pEffectList[i]->m_dX);
						sAbsY = abs(((m_sViewPointY / 32) + 7) - m_pEffectList[i]->m_dY);
#endif
						if (sAbsX > sAbsY) sDist = sAbsX;
						else sDist = sAbsY;
						lPan = -(((m_sViewPointX / 32) + 12) - m_pEffectList[i]->m_dX);
						PlaySound('E', 1, sDist, lPan);
					}
					break;

				case 198: // VAMP - Strike-Of-The-Ghosts
					m_Misc.GetPoint(m_pEffectList[i]->m_mX
						, m_pEffectList[i]->m_mY
						, m_pEffectList[i]->m_dX * 32
						, m_pEffectList[i]->m_dY * 32
						, &m_pEffectList[i]->m_mX
						, &m_pEffectList[i]->m_mY
						, &m_pEffectList[i]->m_iErr
						, 40);

					bAddNewEffect(245, m_pEffectList[i]->m_mX + (rand() % 30) - 15, m_pEffectList[i]->m_mY + (rand() % 30) - 15, 0, 0, 0, 1);

					if (m_pEffectList[i]->m_cFrame >= m_pEffectList[i]->m_cMaxFrame)
					{
						delete m_pEffectList[i];
						m_pEffectList[i] = 0;
					}
					else
					{
#ifdef RES_HIGH
						sAbsX = abs(((m_sViewPointX / 32) + 12) - m_pEffectList[i]->m_dX);
						sAbsY = abs(((m_sViewPointY / 32) + 9) - m_pEffectList[i]->m_dY);
#else
						sAbsX = abs(((m_sViewPointX / 32) + 10) - m_pEffectList[i]->m_dX);
						sAbsY = abs(((m_sViewPointY / 32) + 7) - m_pEffectList[i]->m_dY);
#endif
						if (sAbsX > sAbsY) sDist = sAbsX - 10;
						else sDist = sAbsY - 10;
						lPan = -(((m_sViewPointX / 32) + 12) - m_pEffectList[i]->m_dX);
						PlaySound('E', 1, sDist, lPan);
					}

					break;

				case 196: // Earth-Shock-Wave
					m_Misc.GetPoint(m_pEffectList[i]->m_mX
						, m_pEffectList[i]->m_mY
						, m_pEffectList[i]->m_dX * 32
						, m_pEffectList[i]->m_dY * 32
						, &m_pEffectList[i]->m_mX
						, &m_pEffectList[i]->m_mY
						, &m_pEffectList[i]->m_iErr
						, 40);
					bAddNewEffect(80, m_pEffectList[i]->m_mX + (rand() % 30) - 15, m_pEffectList[i]->m_mY + (rand() % 30) - 15, 0, 0, 0, 1);
					bAddNewEffect(80, m_pEffectList[i]->m_mX + (rand() % 20) - 10, m_pEffectList[i]->m_mY + (rand() % 20) - 10, 0, 0, 0, 0);
					if (m_pEffectList[i]->m_cFrame >= m_pEffectList[i]->m_cMaxFrame)
					{
						delete m_pEffectList[i];
						m_pEffectList[i] = 0;
					}
					else
					{
#ifdef RES_HIGH
						sAbsX = abs(((m_sViewPointX / 32) + 12) - m_pEffectList[i]->m_dX);
						sAbsY = abs(((m_sViewPointY / 32) + 9) - m_pEffectList[i]->m_dY);
#else
						sAbsX = abs(((m_sViewPointX / 32) + 10) - m_pEffectList[i]->m_dX);
						sAbsY = abs(((m_sViewPointY / 32) + 7) - m_pEffectList[i]->m_dY);
#endif
						if (sAbsX > sAbsY) sDist = sAbsX - 10;
						else sDist = sAbsY - 10;
						lPan = -(((m_sViewPointX / 32) + 12) - m_pEffectList[i]->m_dX);
						PlaySound('E', 1, sDist, lPan);
					}
					break;

				case 200:
					if (m_pEffectList[i]->m_cFrame >= m_pEffectList[i]->m_cMaxFrame)
					{
						delete m_pEffectList[i];
						m_pEffectList[i] = 0;
					}
					else
					{
						bAddNewEffect(203, m_pEffectList[i]->m_sX + 40, m_pEffectList[i]->m_sY + 120, 0, 0, 0, 0);
						bAddNewEffect(204, m_pEffectList[i]->m_sX - 10, m_pEffectList[i]->m_sY + 70, 0, 0, 0, 0);
						bAddNewEffect(205, m_pEffectList[i]->m_sX - 10, m_pEffectList[i]->m_sY + 75, 0, 0, 0, 0);
						bAddNewEffect(206, m_pEffectList[i]->m_sX - 7, m_pEffectList[i]->m_sY + 27, 0, 0, 0, 0);

						bAddNewEffect(201, (rand() % 160) + 400, (rand() % 120) + 300, 0, 0, 0, 1);
						bAddNewEffect(202, (rand() % 160) + 400, (rand() % 120) + 300, 0, 0, 0, 1);

						delete m_pEffectList[i];
						m_pEffectList[i] = 0;
					}
					break;

				case 201:
					if (m_pEffectList[i]->m_cFrame >= m_pEffectList[i]->m_cMaxFrame)
					{
						delete m_pEffectList[i];
						m_pEffectList[i] = 0;
					}
					else
					{
						bAddNewEffect(203, m_pEffectList[i]->m_sX + 110, m_pEffectList[i]->m_sY + 120, 0, 0, 0, 0);
						bAddNewEffect(204, m_pEffectList[i]->m_sX - 10, m_pEffectList[i]->m_sY + 70, 0, 0, 0, 0);
						bAddNewEffect(205, m_pEffectList[i]->m_sX - 10, m_pEffectList[i]->m_sY + 75, 0, 0, 0, 0);

						bAddNewEffect(202, (rand() % 160) + 400, (rand() % 120) + 300, 0, 0, 0, 1);

						delete m_pEffectList[i];
						m_pEffectList[i] = 0;
					}
					break;

				case 202:
					if (m_pEffectList[i]->m_cFrame >= m_pEffectList[i]->m_cMaxFrame)
					{
						delete m_pEffectList[i];
						m_pEffectList[i] = 0;
					}
					else
					{
						bAddNewEffect(203, m_pEffectList[i]->m_sX + 65, m_pEffectList[i]->m_sY + 120, 0, 0, 0, 0);
						bAddNewEffect(204, m_pEffectList[i]->m_sX - 10, m_pEffectList[i]->m_sY + 70, 0, 0, 0, 0);
						bAddNewEffect(205, m_pEffectList[i]->m_sX - 10, m_pEffectList[i]->m_sY + 75, 0, 0, 0, 0);
						bAddNewEffect(206, m_pEffectList[i]->m_sX - 7, m_pEffectList[i]->m_sY + 27, 0, 0, 0, 0);
						delete m_pEffectList[i];
						m_pEffectList[i] = 0;
					}
					break;

				case 250: // Gate round
					m_Misc.GetPoint(m_pEffectList[i]->m_mX
						, m_pEffectList[i]->m_mY
						, m_pEffectList[i]->m_dX * 32
						, m_pEffectList[i]->m_dY * 32 - 40
						, &m_pEffectList[i]->m_mX
						, &m_pEffectList[i]->m_mY
						, &m_pEffectList[i]->m_iErr
						, 10);
					if ((abs(m_pEffectList[i]->m_mX - m_pEffectList[i]->m_dX * 32) <= 2)
						&& (abs(m_pEffectList[i]->m_mY - (m_pEffectList[i]->m_dY * 32 - 40)) <= 2))
					{
						delete m_pEffectList[i];
						m_pEffectList[i] = 0;
					}
					break;

				case 251: // Salmon burst (effect11s)
					m_Misc.GetPoint(m_pEffectList[i]->m_mX
						, m_pEffectList[i]->m_mY
						, m_pEffectList[i]->m_dX * 32
						, m_pEffectList[i]->m_dY * 32
						, &m_pEffectList[i]->m_mX
						, &m_pEffectList[i]->m_mY
						, &m_pEffectList[i]->m_iErr
						, 50);
					if ((abs(m_pEffectList[i]->m_mX - m_pEffectList[i]->m_dX * 32) <= 2)
						&& (abs(m_pEffectList[i]->m_mY - (m_pEffectList[i]->m_dY * 32 - 40)) <= 2))
					{
						delete m_pEffectList[i];
						m_pEffectList[i] = 0;
					}
					else
					{
						bAddNewEffect(252, m_pEffectList[i]->m_dX * 32, m_pEffectList[i]->m_dY * 32, 0, 0, 0, 1);
						bAddNewEffect(252, m_pEffectList[i]->m_dX * 32 - 30, m_pEffectList[i]->m_dY * 32 - 15, 0, 0, -7, 1);
						bAddNewEffect(252, m_pEffectList[i]->m_dX * 32 - 35, m_pEffectList[i]->m_dY * 32 - 30, 0, 0, -5, 1);
						bAddNewEffect(252, m_pEffectList[i]->m_dX * 32 + 20, m_pEffectList[i]->m_dY * 32 + 30, 0, 0, -3, 1);
						delete m_pEffectList[i];
						m_pEffectList[i] = 0;
					}
					break;

				case 8:
				case 14:
				case 15:
				case 18:
				case 32:
				case 35: //
				case 36: //
				case 50:
				case 51:
				case 52:
				case 53:
				case 54:
				case 55:
				case 57:
				case 61:
				case 63:
				case 64:
				case 67:
				case 72:
				case 73:
				case 74:
				case 75:
				case 76:
				case 77:
				case 80: //
				case 82: //

				case 101:
				case 102:
				case 104: // Create Potion
				case 111:
				case 112:
				case 113:
				case 121:
				case 122:
				case 123: // Stamina Rec
				case 124:
				case 125:
				case 126:
				case 127:
				case 128: // Gr Stamina Rec
				case 131:
				case 132:
				case 133:
				case 134:
				case 135:
				case 136:
				case 142:
				case 144:

				case 245: // VAMP - Ghosts 1
				case 149: // <--- este en caso de que no este

				case 150: // Berserk : Cirlcle 6 magic
				case 152: // Polymorph
				case 153:
				case 162:
				case 165:
				case 166:
				case 171:
				case 176: //
				case 177: //

				case 180:
				case 183: //
				case 184: // EP's Magic Drain
				
				//case 192:
				//case 193:
				case 194:
				case 195:
				case 242: // Mage hero effect
				case 243: // War hero effect
				case 244: // Snoopy: d�plac� pour nvx sorts: Aura du casteur de Mass MagicMissile
					if (m_pEffectList[i]->m_cFrame > m_pEffectList[i]->m_cMaxFrame)
					{
						delete m_pEffectList[i];
						m_pEffectList[i] = 0;
					}
					break;
				}
			}
		}
	return bRet;
}

void CGame::DrawDialogBox_GuideMap(short msX, short msY, char cLB)
{
	int  m_iMaxMapIndex = DEF_SPRID_INTERFACE_GUIDEMAP + m_cMapIndex + 1;
	int  m_iMinMapIndex = DEF_SPRID_INTERFACE_GUIDEMAP;
	int  m_iMinMapSquare = m_cMapIndex;
	int i;
	//telescope.pak:  frames 1..31 <=> DEF_SPRID_INTERFACE_GUIDEMAP +1...+31 <=> m_cMapIndex 0..30
	//telescope1.pak: frames 1..3  <=> DEF_SPRID_INTERFACE_GUIDEMAP +36..+38 <=> m_cMapIndex 35..37 et +
	if (m_cMapIndex < 0) return;
	if (m_cMapIndex >= 35)
	{
		m_iMaxMapIndex = DEF_SPRID_INTERFACE_GUIDEMAP + m_cMapIndex + 1;
		m_iMinMapIndex = DEF_SPRID_INTERFACE_GUIDEMAP + 35;
		m_iMinMapSquare = m_cMapIndex - 35;
	}
	short sX, sY, shX, shY, szX, szY;
	sX = m_stDialogBoxInfo[9].sX;
	sY = m_stDialogBoxInfo[9].sY;
	szX = m_stDialogBoxInfo[9].sSizeX;
	szY = m_stDialogBoxInfo[9].sSizeY;
	if (sX < 20) sX = 0;
	if (sY < 20) sY = 0;

	//LifeX Fix Map
	if (sX > 400) sX = 799 - 128;
	if (sY > 273) sY = 547 - 128;

	for (shX = -2; shX < 130; shX++)
	{
		m_DDraw.PutPixel(sX + shX, sY - 2, 50, 50, 50);
		m_DDraw.PutPixel(sX + shX, sY - 1, 50, 50, 50);
		m_DDraw.PutPixel(sX + shX, sY + 128, 50, 50, 50);
		m_DDraw.PutPixel(sX + shX, sY + 129, 50, 50, 50);
	}
	for (shY = -2; shY < 130; shY++)
	{
		m_DDraw.PutPixel(sX - 2, sY + shY, 50, 50, 50);
		m_DDraw.PutPixel(sX - 1, sY + shY, 50, 50, 50);
		m_DDraw.PutPixel(sX + 128, sY + shY, 50, 50, 50);
		m_DDraw.PutPixel(sX + 129, sY + shY, 50, 50, 50);
	}

	auto pixblue = [&](int x, int y, int r, int g, int b)
	{
		m_DDraw.PutPixel(x - 1, y, 0, 0, 255);
		m_DDraw.PutPixel(x, y - 1, 0, 0, 255);
		m_DDraw.PutPixel(x - 2, y, 0, 0, 255);
		m_DDraw.PutPixel(x, y - 2, 0, 0, 255);
		m_DDraw.PutPixel(x, y, 0, 0, 255);
		m_DDraw.PutPixel(x, y + 1, 0, 0, 255);
		m_DDraw.PutPixel(x + 1, y, 0, 0, 255);
		m_DDraw.PutPixel(x, y + 2, 0, 0, 255);
		m_DDraw.PutPixel(x + 2, y, 0, 0, 255);
	};

	auto pixred = [&](int x, int y, int r, int g, int b)
	{
		m_DDraw.PutPixel(x - 1, y, 255, 0, 0);
		m_DDraw.PutPixel(x, y - 1, 255, 0, 0);
		m_DDraw.PutPixel(x - 2, y, 255, 0, 0);
		m_DDraw.PutPixel(x, y - 2, 255, 0, 0);
		m_DDraw.PutPixel(x, y, 255, 0, 0);
		m_DDraw.PutPixel(x, y + 1, 255, 0, 0);
		m_DDraw.PutPixel(x + 1, y, 255, 0, 0);
		m_DDraw.PutPixel(x, y + 2, 255, 0, 0);
		m_DDraw.PutPixel(x + 2, y, 255, 0, 0);
	};

	auto pixgreen = [&](int x, int y, int r, int g, int b)
	{
		m_DDraw.PutPixel(x - 1, y, 0, 255, 0);
		m_DDraw.PutPixel(x, y - 1, 0, 255, 0);
		m_DDraw.PutPixel(x - 2, y, 0, 255, 0);
		m_DDraw.PutPixel(x, y - 2, 0, 255, 0);
		m_DDraw.PutPixel(x, y, 0, 255, 0);
		m_DDraw.PutPixel(x, y + 1, 0, 255, 0);
		m_DDraw.PutPixel(x + 1, y, 0, 255, 0);
		m_DDraw.PutPixel(x, y + 2, 0, 255, 0);
		m_DDraw.PutPixel(x + 2, y, 0, 255, 0);
	};

	auto pixyellow = [&](int x, int y, int r, int g, int b)
	{
		m_DDraw.PutPixel(x - 1, y, 255, 255, 0);
		m_DDraw.PutPixel(x, y - 1, 255, 255, 0);
		m_DDraw.PutPixel(x - 2, y, 255, 255, 0);
		m_DDraw.PutPixel(x, y - 2, 255, 255, 0);
		m_DDraw.PutPixel(x, y, 255, 255, 0);
		m_DDraw.PutPixel(x, y + 1, 255, 255, 0);
		m_DDraw.PutPixel(x + 1, y, 255, 255, 0);
		m_DDraw.PutPixel(x, y + 2, 255, 255, 0);
		m_DDraw.PutPixel(x + 2, y, 255, 255, 0);
	};

	auto pixorange = [&](int x, int y, int r, int g, int b)
	{
		m_DDraw.PutPixel(x - 1, y, 255, 165, 0);
		m_DDraw.PutPixel(x, y - 1, 255, 165, 0);
		m_DDraw.PutPixel(x - 2, y, 255, 165, 0);
		m_DDraw.PutPixel(x, y - 2, 255, 165, 0);
		m_DDraw.PutPixel(x, y, 255, 165, 0);
		m_DDraw.PutPixel(x, y + 1, 255, 165, 0);
		m_DDraw.PutPixel(x + 1, y, 255, 165, 0);
		m_DDraw.PutPixel(x, y + 2, 255, 165, 0);
		m_DDraw.PutPixel(x + 2, y, 255, 165, 0);
	};

	if (m_bZoomMap)
	{
		shX = m_sPlayerX - 64;
		shY = m_sPlayerY - 64;
		if (shX < 0) shX = 0;
		if (shY < 0) shY = 0;
		if (shX > m_pMapData->m_sMapSizeX - 128) shX = m_pMapData->m_sMapSizeX - 128;
		if (shY > m_pMapData->m_sMapSizeY - 128) shY = m_pMapData->m_sMapSizeY - 128;
		if (m_bDialogTrans) m_pSprite[m_iMaxMapIndex]->PutShiftTransSprite2(sX, sY, shX, shY, 0, m_dwCurTime);
		else m_pSprite[m_iMaxMapIndex]->PutShiftSpriteFast(sX, sY, shX, shY, 0, m_dwCurTime);

		m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX - shX + m_sPlayerX, sY - shY + m_sPlayerY, 37, m_dwCurTime);

		if (bShinning || m_bIsCTFMode || m_bApocalypse)
		{
			for (auto& u : m_minimapblue.list)
			{
				auto* p = &u;
				if (p->x < shX || p->x > shX + 128 || p->y < shY || p->y > shY + 128)
					continue;

				if (m_dwTime - p->time > 25000)
					continue;

				pixblue(sX + p->x - shX, sY + p->y - shY, 0, 0, 255);
			}

			for (auto& u : m_minimapred.list)
			{
				auto* p = &u;
				if (p->x < shX || p->x > shX + 128 || p->y < shY || p->y > shY + 128)
					continue;

				if (m_dwTime - p->time > 25000)
					continue;

				pixred(sX + p->x - shX, sY + p->y - shY, 255, 0, 0);
			}
		}
		if (!bShinning)
		{
			for (auto& u : m_minimapgreen.list)
			{
				auto* p = &u;
				if (p->x < shX || p->x > shX + 128 || p->y < shY || p->y > shY + 128)
					continue;

				if (m_dwTime - p->time > 25000)
					continue;

				pixgreen(sX + p->x - shX, sY + p->y - shY, 0, 255, 0);
			}

			for (auto& u : m_minimaporange.list)
			{
				auto* p = &u;
				if (p->x < shX || p->x > shX + 128 || p->y < shY || p->y > shY + 128)
					continue;

				/*if (m_dwTime - p->time > 25000)
					continue;*/

				pixorange(sX + p->x - shX, sY + p->y - shY, 255, 165, 0);
			}

			if (m_iPartyStatus != 0)
			{
				for (i = 0; i <= DEF_MAXPARTYMEMBERS; i++)
				{
					if (strlen(m_stPartyMemberNameList[i].cName) != 0)
						if (memcmp(PartyMapName[i], m_cMapName, 10) == 0)
							if (memcmp(m_stPartyMemberNameList[i].cName, m_cPlayerName, 10) != 0)
								if (iParty_sX[i] >= shX && iParty_sX[i] <= shX + 128 && iParty_sY[i] >= shY && iParty_sY[i] <= shY + 128)
									//m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX - shX + iParty_sX[i], sY - shY + iParty_sY[i], 38, m_dwCurTime);
									pixyellow(sX + iParty_sX[i] - shX, sY + iParty_sY[i] - shY, 255, 255, 0);
				}
			}
		}

		if ((m_dwCurTime - m_dwMonsterEventTime) < 30000)
		{
			if ((m_dwCurTime % 500) < 370)
			{
				if (m_sEventX >= shX && m_sEventX <= shX + 128 && m_sEventY >= shY && m_sEventY <= shY + 128)
					m_pSprite[DEF_SPRID_INTERFACE_MONSTER]->PutSpriteFast(sX + m_sEventX - shX, sY + m_sEventY - shY, m_sMonsterID, m_dwCurTime);
			}
		}
		else
		{
			m_dwMonsterEventTime = 0;
			m_sMonsterID = 0;
		}

		if (m_bNpcMap) {//MAPA GRANDE
			if (m_cMapIndex == 11) { // Aresden
	 // Slime
				if (88 >= shX && 88 <= shX + 128 && 31 >= shY && 31 <= shY + 128)
					m_pSprite[DEF_SPRID_INTERFACE_MONSTER]->PutSpriteFast(sX + 88 - shX, sY + 31 - shY, 17, m_dwCurTime);
				if (30 >= shX && 30 <= shX + 128 && 170 >= shY && 170 <= shY + 128)
					m_pSprite[DEF_SPRID_INTERFACE_MONSTER]->PutSpriteFast(sX + 30 - shX, sY + 170 - shY, 12, m_dwCurTime);
				if (134 >= shX && 134 <= shX + 128 && 234 >= shY && 234 <= shY + 128)
					m_pSprite[DEF_SPRID_INTERFACE_MONSTER]->PutSpriteFast(sX + 134 - shX, sY + 234 - shY, 10, m_dwCurTime);
				if (186 >= shX && 186 <= shX + 128 && 227 >= shY && 227 <= shY + 128)
					m_pSprite[DEF_SPRID_INTERFACE_MONSTER]->PutSpriteFast(sX + 186 - shX, sY + 227 - shY, 54, m_dwCurTime);
				if (253 >= shX && 253 <= shX + 128 && 189 >= shY && 189 <= shY + 128)
					m_pSprite[DEF_SPRID_INTERFACE_MONSTER]->PutSpriteFast(sX + 253 - shX, sY + 189 - shY, 11, m_dwCurTime);
				if (109 >= shX && 109 <= shX + 128 && 49 >= shY && 49 <= shY + 128)
					m_pSprite[DEF_SPRID_INTERFACE_MONSTER]->PutSpriteFast(sX + 109 - shX, sY + 49 - shY, 18, m_dwCurTime);
			}
			else if (m_cMapIndex == 3) { // Elvine
			// Slime
				if (148 >= shX && 148 <= shX + 128 && 29 >= shY && 29 <= shY + 128)
					m_pSprite[DEF_SPRID_INTERFACE_MONSTER]->PutSpriteFast(sX + 148 - shX, sY + 29 - shY, 17, m_dwCurTime);
				if (123 >= shX && 123 <= shX + 128 && 28 >= shY && 28 <= shY + 128)
					m_pSprite[DEF_SPRID_INTERFACE_MONSTER]->PutSpriteFast(sX + 123 - shX, sY + 28 - shY, 11, m_dwCurTime);
				if (46 >= shX && 46 <= shX + 128 && 56 >= shY && 56 <= shY + 128)
					m_pSprite[DEF_SPRID_INTERFACE_MONSTER]->PutSpriteFast(sX + 46 - shX, sY + 56 - shY, 18, m_dwCurTime);
				if (31 >= shX && 31 <= shX + 128 && 176 >= shY && 176 <= shY + 128)
					m_pSprite[DEF_SPRID_INTERFACE_MONSTER]->PutSpriteFast(sX + 31 - shX, sY + 176 - shY, 12, m_dwCurTime);
				if (122 >= shX && 122 <= shX + 128 && 234 >= shY && 234 <= shY + 128)
					m_pSprite[DEF_SPRID_INTERFACE_MONSTER]->PutSpriteFast(sX + 122 - shX, sY + 234 - shY, 10, m_dwCurTime);
				if (234 >= shX && 234 <= shX + 128 && 201 >= shY && 201 <= shY + 128)
					m_pSprite[DEF_SPRID_INTERFACE_MONSTER]->PutSpriteFast(sX + 234 - shX, sY + 201 - shY, 54, m_dwCurTime);

			}
			else if (m_cMapIndex == 4) { // Middleland
			// Slime
				if (227 >= shX && 227 <= shX + 128 && 419 >= shY && 419 <= shY + 128)
					m_pSprite[DEF_SPRID_INTERFACE_MONSTER]->PutSpriteFast(sX + 227 - shX, sY + 419 - shY, 33, m_dwCurTime);
				if (251 >= shX && 251 <= shX + 128 && 274 >= shY && 274 <= shY + 128)
					m_pSprite[DEF_SPRID_INTERFACE_MONSTER]->PutSpriteFast(sX + 251 - shX, sY + 274 - shY, 29, m_dwCurTime);
				if (240 >= shX && 240 <= shX + 128 && 197 >= shY && 197 <= shY + 128)
					m_pSprite[DEF_SPRID_INTERFACE_MONSTER]->PutSpriteFast(sX + 240 - shX, sY + 197 - shY, 29, m_dwCurTime);
				if (187 >= shX && 187 <= shX + 128 && 93 >= shY && 93 <= shY + 128)
					m_pSprite[DEF_SPRID_INTERFACE_MONSTER]->PutSpriteFast(sX + 187 - shX, sY + 93 - shY, 33, m_dwCurTime);
				if (435 >= shX && 435 <= shX + 128 && 378 >= shY && 378 <= shY + 128)
					m_pSprite[DEF_SPRID_INTERFACE_MONSTER]->PutSpriteFast(sX + 435 - shX, sY + 378 - shY, 59, m_dwCurTime);
				if (420 >= shX && 420 <= shX + 128 && 150 >= shY && 150 <= shY + 128)
					m_pSprite[DEF_SPRID_INTERFACE_MONSTER]->PutSpriteFast(sX + 420 - shX, sY + 150 - shY, 59, m_dwCurTime);
				if (466 >= shX && 466 <= shX + 128 && 314 >= shY && 314 <= shY + 128)
					m_pSprite[DEF_SPRID_INTERFACE_MONSTER]->PutSpriteFast(sX + 466 - shX, sY + 314 - shY, 31, m_dwCurTime);
				if (50 >= shX && 50 <= shX + 128 && 213 >= shY && 213 <= shY + 128)
					m_pSprite[DEF_SPRID_INTERFACE_MONSTER]->PutSpriteFast(sX + 50 - shX, sY + 213 - shY, 31, m_dwCurTime);

			}
			else if (m_cMapIndex == 24) { // Icebound
			// Slime
				if (190 >= shX && 190 <= shX + 128 && 224 >= shY && 224 <= shY + 128)
					m_pSprite[DEF_SPRID_INTERFACE_MONSTER]->PutSpriteFast(sX + 190 - shX, sY + 224 - shY, 65, m_dwCurTime);
				if (220 >= shX && 220 <= shX + 128 && 251 >= shY && 251 <= shY + 128)
					m_pSprite[DEF_SPRID_INTERFACE_MONSTER]->PutSpriteFast(sX + 220 - shX, sY + 251 - shY, 65, m_dwCurTime);
				if (199 >= shX && 199 <= shX + 128 && 37 >= shY && 37 <= shY + 128)
					m_pSprite[DEF_SPRID_INTERFACE_MONSTER]->PutSpriteFast(sX + 199 - shX, sY + 37 - shY, 63, m_dwCurTime);
				if (137 >= shX && 137 <= shX + 128 && 57 >= shY && 57 <= shY + 128)
					m_pSprite[DEF_SPRID_INTERFACE_MONSTER]->PutSpriteFast(sX + 137 - shX, sY + 57 - shY, 79, m_dwCurTime);
				if (43 >= shX && 43 <= shX + 128 && 35 >= shY && 35 <= shY + 128)
					m_pSprite[DEF_SPRID_INTERFACE_MONSTER]->PutSpriteFast(sX + 43 - shX, sY + 35 - shY, 66, m_dwCurTime);
				if (227 >= shX && 227 <= shX + 128 && 139 >= shY && 139 <= shY + 128)
					m_pSprite[DEF_SPRID_INTERFACE_MONSTER]->PutSpriteFast(sX + 227 - shX, sY + 139 - shY, 65, m_dwCurTime);


			}
		}
	}
	else // Sans zoom
	{
		if (m_bDialogTrans) m_pSprite[m_iMinMapIndex]->PutTransSprite2(sX, sY, m_iMinMapSquare, m_dwCurTime);
		else m_pSprite[m_iMinMapIndex]->PutSpriteFastNoColorKey(sX, sY, m_iMinMapSquare, m_dwCurTime);
		shX = (m_sPlayerX * 128) / (m_pMapData->m_sMapSizeX);
		shY = (m_sPlayerY * 128) / (m_pMapData->m_sMapSizeX);

		m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + shX, sY + shY, 37, m_dwCurTime);

		if (bShinning || m_bIsCTFMode || m_bApocalypse)
		{
			for (auto& u : m_minimapblue.list) {
				auto* p = &u;
				shX = (p->x * 128) / (m_pMapData->m_sMapSizeX);
				shY = (p->y * 128) / (m_pMapData->m_sMapSizeY);

				if (m_dwCurTime - p->time > 25000)
					continue;

				m_DDraw.PutPixel(sX + shX, sY + shY - 1, 0, 0, 255);
				m_DDraw.PutPixel(sX + shX - 1, sY + shY, 0, 0, 255);
				m_DDraw.PutPixel(sX + shX, sY + shY - 2, 0, 0, 255);
				m_DDraw.PutPixel(sX + shX - 2, sY + shY, 0, 0, 255);
				m_DDraw.PutPixel(sX + shX, sY + shY, 0, 0, 255);
				m_DDraw.PutPixel(sX + shX + 1, sY + shY, 0, 0, 255);
				m_DDraw.PutPixel(sX + shX, sY + shY + 1, 0, 0, 255);
				m_DDraw.PutPixel(sX + shX + 2, sY + shY, 0, 0, 255);
				m_DDraw.PutPixel(sX + shX, sY + shY + 2, 0, 0, 255);
			}

			for (auto& u : m_minimapred.list) {
				auto* p = &u;
				shX = (p->x * 128) / (m_pMapData->m_sMapSizeX);
				shY = (p->y * 128) / (m_pMapData->m_sMapSizeY);

				if (m_dwCurTime - p->time > 25000)
					continue;

				m_DDraw.PutPixel(sX + shX, sY + shY - 1, 255, 0, 0);
				m_DDraw.PutPixel(sX + shX - 1, sY + shY, 255, 0, 0);
				m_DDraw.PutPixel(sX + shX, sY + shY - 2, 255, 0, 0);
				m_DDraw.PutPixel(sX + shX - 2, sY + shY, 255, 0, 0);
				m_DDraw.PutPixel(sX + shX, sY + shY, 255, 0, 0);
				m_DDraw.PutPixel(sX + shX + 1, sY + shY, 255, 0, 0);
				m_DDraw.PutPixel(sX + shX, sY + shY + 1, 255, 0, 0);
				m_DDraw.PutPixel(sX + shX + 2, sY + shY, 255, 0, 0);
				m_DDraw.PutPixel(sX + shX, sY + shY + 2, 255, 0, 0);
			}
		}

		if (!bShinning)
		{
			for (auto& u : m_minimapgreen.list) {
				auto* p = &u;
				shX = (p->x * 128) / (m_pMapData->m_sMapSizeX);
				shY = (p->y * 128) / (m_pMapData->m_sMapSizeY);

				if (m_dwCurTime - p->time > 25000)
					continue;

				m_DDraw.PutPixel(sX + shX, sY + shY - 1, 0, 255, 0);
				m_DDraw.PutPixel(sX + shX - 1, sY + shY, 0, 255, 0);
				m_DDraw.PutPixel(sX + shX, sY + shY - 2, 0, 255, 0);
				m_DDraw.PutPixel(sX + shX - 2, sY + shY, 0, 255, 0);
				m_DDraw.PutPixel(sX + shX, sY + shY, 0, 255, 0);
				m_DDraw.PutPixel(sX + shX + 1, sY + shY, 0, 255, 0);
				m_DDraw.PutPixel(sX + shX, sY + shY + 1, 0, 255, 0);
				m_DDraw.PutPixel(sX + shX + 2, sY + shY, 0, 255, 0);
				m_DDraw.PutPixel(sX + shX, sY + shY + 2, 0, 255, 0);
			}

			for (auto& u : m_minimaporange.list) {
				auto* p = &u;
				shX = (p->x * 128) / (m_pMapData->m_sMapSizeX);
				shY = (p->y * 128) / (m_pMapData->m_sMapSizeY);

				/*if (m_dwCurTime - p->time > 25000)
					continue;*/

				m_DDraw.PutPixel(sX + shX, sY + shY - 1, 255, 165, 0);
				m_DDraw.PutPixel(sX + shX - 1, sY + shY, 255, 165, 0);
				m_DDraw.PutPixel(sX + shX, sY + shY - 2, 255, 165, 0);
				m_DDraw.PutPixel(sX + shX - 2, sY + shY, 255, 165, 0);
				m_DDraw.PutPixel(sX + shX, sY + shY, 255, 165, 0);
				m_DDraw.PutPixel(sX + shX + 1, sY + shY, 255, 165, 0);
				m_DDraw.PutPixel(sX + shX, sY + shY + 1, 255, 165, 0);
				m_DDraw.PutPixel(sX + shX + 2, sY + shY, 255, 165, 0);
				m_DDraw.PutPixel(sX + shX, sY + shY + 2, 255, 165, 0);
			}

			if (m_iPartyStatus != 0)
			{
				for (i = 0; i <= DEF_MAXPARTYMEMBERS; i++)
				{
					if (strlen(m_stPartyMemberNameList[i].cName) != 0)
					{
						if (memcmp(PartyMapName[i], m_cMapName, 10) == 0)
						{
							shX = (iParty_sX[i] * 128) / (m_pMapData->m_sMapSizeX);
							shY = (iParty_sY[i] * 128) / (m_pMapData->m_sMapSizeX);
							if (memcmp(m_stPartyMemberNameList[i].cName, m_cPlayerName, 10) != 0) {
								//m_pSprite[DEF_SPRID_INTERFACE_ND_CRUSADE]->PutSpriteFast(sX + shX, sY + shY, 38, m_dwCurTime);

								m_DDraw.PutPixel(sX + shX, sY + shY - 1, 255, 255, 0);
								m_DDraw.PutPixel(sX + shX - 1, sY + shY, 255, 255, 0);
								m_DDraw.PutPixel(sX + shX, sY + shY - 2, 255, 255, 0);
								m_DDraw.PutPixel(sX + shX - 2, sY + shY, 255, 255, 0);
								m_DDraw.PutPixel(sX + shX, sY + shY, 255, 255, 0);
								m_DDraw.PutPixel(sX + shX + 1, sY + shY, 255, 255, 0);
								m_DDraw.PutPixel(sX + shX, sY + shY + 1, 255, 255, 0);
								m_DDraw.PutPixel(sX + shX + 2, sY + shY, 255, 255, 0);
								m_DDraw.PutPixel(sX + shX, sY + shY + 2, 255, 255, 0);
							}

						}
					}
				}
			}
		}

		if ((G_dwGlobalTime - m_dwMonsterEventTime) < 30000)
		{
			if ((m_dwCurTime % 500) < 370)
			{
				shX = (m_sEventX * 128) / (m_pMapData->m_sMapSizeX);
				shY = (m_sEventY * 128) / (m_pMapData->m_sMapSizeX);
				m_pSprite[DEF_SPRID_INTERFACE_MONSTER]->PutSpriteFast(sX + shX, sY + shY, m_sMonsterID, m_dwCurTime);
			}
		}
		else
		{
			m_dwMonsterEventTime = 0;
			m_sMonsterID = 0;
		}

		if (m_bNpcMap) {//MAPA CHICO
			if (m_cMapIndex == 11) { // Aresden

				m_pSprite[DEF_SPRID_INTERFACE_MONSTER]->PutSpriteFast(sX + ((83 * 128) / m_pMapData->m_sMapSizeX), sY + ((31 * 128) / m_pMapData->m_sMapSizeX), 17, m_dwCurTime);
				m_pSprite[DEF_SPRID_INTERFACE_MONSTER]->PutSpriteFast(sX + ((30 * 128) / m_pMapData->m_sMapSizeX), sY + ((170 * 128) / m_pMapData->m_sMapSizeX), 12, m_dwCurTime);
				m_pSprite[DEF_SPRID_INTERFACE_MONSTER]->PutSpriteFast(sX + ((133 * 128) / m_pMapData->m_sMapSizeX), sY + ((234 * 128) / m_pMapData->m_sMapSizeX), 10, m_dwCurTime);
				m_pSprite[DEF_SPRID_INTERFACE_MONSTER]->PutSpriteFast(sX + ((186 * 128) / m_pMapData->m_sMapSizeX), sY + ((227 * 128) / m_pMapData->m_sMapSizeX), 54, m_dwCurTime);
				m_pSprite[DEF_SPRID_INTERFACE_MONSTER]->PutSpriteFast(sX + ((253 * 128) / m_pMapData->m_sMapSizeX), sY + ((189 * 128) / m_pMapData->m_sMapSizeX), 11, m_dwCurTime);
				m_pSprite[DEF_SPRID_INTERFACE_MONSTER]->PutSpriteFast(sX + ((109 * 128) / m_pMapData->m_sMapSizeX), sY + ((49 * 128) / m_pMapData->m_sMapSizeX), 18, m_dwCurTime);

			}
			else if (m_cMapIndex == 3) { // Elvine

				m_pSprite[DEF_SPRID_INTERFACE_MONSTER]->PutSpriteFast(sX + ((148 * 128) / m_pMapData->m_sMapSizeX), sY + ((29 * 128) / m_pMapData->m_sMapSizeX), 17, m_dwCurTime);
				m_pSprite[DEF_SPRID_INTERFACE_MONSTER]->PutSpriteFast(sX + ((123 * 128) / m_pMapData->m_sMapSizeX), sY + ((28 * 128) / m_pMapData->m_sMapSizeX), 11, m_dwCurTime);
				m_pSprite[DEF_SPRID_INTERFACE_MONSTER]->PutSpriteFast(sX + ((46 * 128) / m_pMapData->m_sMapSizeX), sY + ((56 * 128) / m_pMapData->m_sMapSizeX), 18, m_dwCurTime);
				m_pSprite[DEF_SPRID_INTERFACE_MONSTER]->PutSpriteFast(sX + ((31 * 128) / m_pMapData->m_sMapSizeX), sY + ((176 * 128) / m_pMapData->m_sMapSizeX), 12, m_dwCurTime);
				m_pSprite[DEF_SPRID_INTERFACE_MONSTER]->PutSpriteFast(sX + ((122 * 128) / m_pMapData->m_sMapSizeX), sY + ((234 * 128) / m_pMapData->m_sMapSizeX), 10, m_dwCurTime);
				m_pSprite[DEF_SPRID_INTERFACE_MONSTER]->PutSpriteFast(sX + ((234 * 128) / m_pMapData->m_sMapSizeX), sY + ((201 * 128) / m_pMapData->m_sMapSizeX), 54, m_dwCurTime);

			}
			else if (m_cMapIndex == 4) { // Middleland

				m_pSprite[DEF_SPRID_INTERFACE_MONSTER]->PutSpriteFast(sX + ((227 * 128) / m_pMapData->m_sMapSizeX), sY + ((419 * 128) / m_pMapData->m_sMapSizeX), 33, m_dwCurTime);
				m_pSprite[DEF_SPRID_INTERFACE_MONSTER]->PutSpriteFast(sX + ((251 * 128) / m_pMapData->m_sMapSizeX), sY + ((274 * 128) / m_pMapData->m_sMapSizeX), 29, m_dwCurTime);
				m_pSprite[DEF_SPRID_INTERFACE_MONSTER]->PutSpriteFast(sX + ((240 * 128) / m_pMapData->m_sMapSizeX), sY + ((197 * 128) / m_pMapData->m_sMapSizeX), 29, m_dwCurTime);
				m_pSprite[DEF_SPRID_INTERFACE_MONSTER]->PutSpriteFast(sX + ((187 * 128) / m_pMapData->m_sMapSizeX), sY + ((93 * 128) / m_pMapData->m_sMapSizeX), 33, m_dwCurTime);
				m_pSprite[DEF_SPRID_INTERFACE_MONSTER]->PutSpriteFast(sX + ((435 * 128) / m_pMapData->m_sMapSizeX), sY + ((378 * 128) / m_pMapData->m_sMapSizeX), 59, m_dwCurTime);
				m_pSprite[DEF_SPRID_INTERFACE_MONSTER]->PutSpriteFast(sX + ((420 * 128) / m_pMapData->m_sMapSizeX), sY + ((150 * 128) / m_pMapData->m_sMapSizeX), 59, m_dwCurTime);
				m_pSprite[DEF_SPRID_INTERFACE_MONSTER]->PutSpriteFast(sX + ((466 * 128) / m_pMapData->m_sMapSizeX), sY + ((314 * 128) / m_pMapData->m_sMapSizeX), 31, m_dwCurTime);
				m_pSprite[DEF_SPRID_INTERFACE_MONSTER]->PutSpriteFast(sX + ((50 * 128) / m_pMapData->m_sMapSizeX), sY + ((213 * 128) / m_pMapData->m_sMapSizeX), 31, m_dwCurTime);


			}
			else if (m_cMapIndex == 24) { // Icebound

				m_pSprite[DEF_SPRID_INTERFACE_MONSTER]->PutSpriteFast(sX + ((190 * 128) / m_pMapData->m_sMapSizeX), sY + ((224 * 128) / m_pMapData->m_sMapSizeX), 65, m_dwCurTime);
				m_pSprite[DEF_SPRID_INTERFACE_MONSTER]->PutSpriteFast(sX + ((220 * 128) / m_pMapData->m_sMapSizeX), sY + ((251 * 128) / m_pMapData->m_sMapSizeX), 65, m_dwCurTime);
				m_pSprite[DEF_SPRID_INTERFACE_MONSTER]->PutSpriteFast(sX + ((199 * 128) / m_pMapData->m_sMapSizeX), sY + ((37 * 128) / m_pMapData->m_sMapSizeX), 63, m_dwCurTime);
				m_pSprite[DEF_SPRID_INTERFACE_MONSTER]->PutSpriteFast(sX + ((137 * 128) / m_pMapData->m_sMapSizeX), sY + ((57 * 128) / m_pMapData->m_sMapSizeX), 79, m_dwCurTime);
				m_pSprite[DEF_SPRID_INTERFACE_MONSTER]->PutSpriteFast(sX + ((43 * 128) / m_pMapData->m_sMapSizeX), sY + ((35 * 128) / m_pMapData->m_sMapSizeX), 66, m_dwCurTime);
				m_pSprite[DEF_SPRID_INTERFACE_MONSTER]->PutSpriteFast(sX + ((227 * 128) / m_pMapData->m_sMapSizeX), sY + ((139 * 128) / m_pMapData->m_sMapSizeX), 65, m_dwCurTime);



			}
		}
	}
	if (cLB != 0) return;
	if (msX >= sX && msX < sX + szY && msY >= sY && msY < sY + szY)
	{
		if (sY > 273) shY = sY - 17;
		else shY = sY + szY + 4;
		if (m_bZoomMap) PutString(sX, shY, "(-) Zoom out", RGB(200, 200, 120));//"(-)
		else PutString(sX, shY, "(+) Zoom in", RGB(200, 200, 120));//"(+)

		if (m_bZoomMap)
		{
			shX = m_sPlayerX - 64;
			shY = m_sPlayerY - 64;
			if (shX < 0) shX = 0;
			if (shY < 0) shY = 0;
			if (shX > m_pMapData->m_sMapSizeX - 128) shX = m_pMapData->m_sMapSizeX - 128;
			if (shY > m_pMapData->m_sMapSizeY - 128) shY = m_pMapData->m_sMapSizeY - 128;
			shX += msX - sX;
			shY += msY - sY;
		}
		else
		{
			shX = (msX - sX) * m_pMapData->m_sMapSizeX / 128;
			shY = (msY - sY) * m_pMapData->m_sMapSizeX / 128;
		}
		wsprintf(G_cTxt, "(%d, %d)", shX, shY);
		if (m_cMapIndex == 11) // Aresden: Fixed by Snoopy for v3.51 maps
		{
			if (shX > 46 && shX < 66 && shY > 107 && shY < 127) strcpy(G_cTxt, DEF_MSG_MAPNAME_MAGICTOWER);
			else if (shX > 103 && shX < 123 && shY > 86 && shY < 116)  strcpy(G_cTxt, DEF_MSG_MAPNAME_GUILDHALL);
			else if (shX > 176 && shX < 196 && shY >  62 && shY < 82)  strcpy(G_cTxt, DEF_MSG_MAPNAME_CATH);
			else if (shX > 135 && shX < 155 && shY > 113 && shY < 133) strcpy(G_cTxt, DEF_MSG_MAPNAME_CITYHALL);
			else if (shX > 97 && shX < 117 && shY > 175 && shY < 195) strcpy(G_cTxt, DEF_MSG_MAPNAME_WAREHOUSE);
			else if (shX > 223 && shX < 243 && shY > 124 && shY < 144) strcpy(G_cTxt, DEF_MSG_MAPNAME_WAREHOUSE);
			else if (shX > 118 && shX < 138 && shY > 157 && shY < 177) strcpy(G_cTxt, DEF_MSG_MAPNAME_SHOP);
			else if (shX > 148 && shX < 178 && shY > 188 && shY < 208) strcpy(G_cTxt, DEF_MSG_MAPNAME_BLACKSMITH);
			else if (shX > 69 && shX < 89 && shY > 199 && shY < 219) strcpy(G_cTxt, DEF_MSG_MAPNAME_DUNGEON);
			else if (shX > 21 && shX < 41 && shY > 266 && shY < 286) strcpy(G_cTxt, DEF_MSG_MAPNAME_HUNT);
			else if (shX > 20 && shX < 40 && shY > 13 && shY < 33)  strcpy(G_cTxt, DEF_MSG_MAPNAME_ML);
			else if (shX > 246 && shX < 266 && shY > 16 && shY < 36)  strcpy(G_cTxt, DEF_MSG_MAPNAME_ML);
			else if (shX > 265 && shX < 285 && shY > 195 && shY < 215) strcpy(G_cTxt, DEF_MSG_MAPNAME_FARM);
			else if (shX > 88 && shX < 108 && shY > 150 && shY < 170) strcpy(G_cTxt, DEF_MSG_MAPNAME_CMDHALL);
		}
		else if (m_cMapIndex == 3) // Elvine: Fixed by Snoopy for v3.51 maps
		{
			if (shX > 170 && shX < 190 && shY >  65 && shY < 85)      strcpy(G_cTxt, DEF_MSG_MAPNAME_MAGICTOWER);
			else if (shX > 67 && shX < 87 && shY > 130 && shY < 150)  strcpy(G_cTxt, DEF_MSG_MAPNAME_GUILDHALL);
			else if (shX > 121 && shX < 141 && shY >  66 && shY < 86)  strcpy(G_cTxt, DEF_MSG_MAPNAME_CATH);
			else if (shX > 135 && shX < 155 && shY > 117 && shY < 137) strcpy(G_cTxt, DEF_MSG_MAPNAME_CITYHALL);
			else if (shX > 190 && shX < 213 && shY > 118 && shY < 138) strcpy(G_cTxt, DEF_MSG_MAPNAME_WAREHOUSE);
			else if (shX > 73 && shX < 103 && shY > 165 && shY < 185) strcpy(G_cTxt, DEF_MSG_MAPNAME_WAREHOUSE);
			else if (shX > 217 && shX < 237 && shY > 142 && shY < 162) strcpy(G_cTxt, DEF_MSG_MAPNAME_SHOP);
			else if (shX > 216 && shX < 256 && shY > 99 && shY < 119) strcpy(G_cTxt, DEF_MSG_MAPNAME_BLACKSMITH);
			else if (shX > 251 && shX < 271 && shY >  73 && shY < 93)  strcpy(G_cTxt, DEF_MSG_MAPNAME_DUNGEON);
			else if (shX > 212 && shX < 232 && shY > 13 && shY < 33)  strcpy(G_cTxt, DEF_MSG_MAPNAME_HUNT);
			else if (shX > 16 && shX < 36 && shY > 262 && shY < 282) strcpy(G_cTxt, DEF_MSG_MAPNAME_ML);
			else if (shX > 244 && shX < 264 && shY > 248 && shY < 268) strcpy(G_cTxt, DEF_MSG_MAPNAME_ML);
			else if (shX > 264 && shX < 284 && shY > 177 && shY < 207) strcpy(G_cTxt, DEF_MSG_MAPNAME_FARM);
			else if (shX > 207 && shX < 227 && shY > 79 && shY < 99)  strcpy(G_cTxt, DEF_MSG_MAPNAME_CMDHALL);
		}
		else if (m_cMapIndex == 5) // Elvine Farm: Fixed by Snoopy for v3.51 maps
		{
			if (shX > 62 && shX < 82 && shY >  187 && shY < 207) strcpy(G_cTxt, DEF_MSG_MAPNAME_WAREHOUSE);
			else if (shX > 81 && shX < 101 && shY > 169 && shY < 189) strcpy(G_cTxt, DEF_MSG_MAPNAME_SHOP);
			else if (shX > 101 && shX < 131 && shY > 180 && shY < 200) strcpy(G_cTxt, DEF_MSG_MAPNAME_BLACKSMITH);
			else if (shX > 130 && shX < 150 && shY > 195 && shY < 215) strcpy(G_cTxt, DEF_MSG_MAPNAME_DUNGEON);
			else if (shX > 86 && shX < 106 && shY > 139 && shY < 159) strcpy(G_cTxt, DEF_MSG_MAPNAME_BARRACK);
		}
		else if (m_cMapIndex == 6) // Areden's Farm: Fixed by Snoopy for v3.51 maps
		{
			if (shX > 30 && shX < 50 && shY >  80 && shY < 100) strcpy(G_cTxt, DEF_MSG_MAPNAME_WAREHOUSE);
			else if (shX > 55 && shX < 85 && shY > 80 && shY < 100)  strcpy(G_cTxt, DEF_MSG_MAPNAME_BLACKSMITH);
			else if (shX > 52 && shX < 72 && shY > 80 && shY < 100)  strcpy(G_cTxt, DEF_MSG_MAPNAME_SHOP);
			else if (shX > 70 && shX < 90 && shY > 60 && shY < 80)   strcpy(G_cTxt, DEF_MSG_MAPNAME_DUNGEON);
			else if (shX > 45 && shX < 65 && shY > 123 && shY < 143) strcpy(G_cTxt, DEF_MSG_MAPNAME_BARRACK);
		}

		if (m_iPartyStatus != 0)
		{
			for (i = 0; i <= DEF_MAXPARTYMEMBERS; i++)
			{
				if (strlen(m_stPartyMemberNameList[i].cName) != 0)
					if (memcmp(PartyMapName[i], m_cMapName, 10) == 0)
						if (shX > iParty_sX[i] - 5 && shX < iParty_sX[i] + 5 && shY >  iParty_sY[i] - 5 && shY < iParty_sY[i] + 5)
							strcpy(G_cTxt, m_stPartyMemberNameList[i].cName);

			}
		}

		PutString(msX - 10, msY - 13, G_cTxt, RGB(200, 200, 120));
	}
}

void CGame::DrawDialogBoxs(short msX, short msY, short msZ, char cLB)
{
	int i;
	if (m_bIsObserverMode == true) return;
	m_DInput.m_sZ = 0;
	//Snoopy: 41->61
	for (i = 0; i < 61; i++)
		if (m_cDialogBoxOrder[i] != 0)
		{
			switch (m_cDialogBoxOrder[i]) {
			case 1:
				DrawDialogBox_Character(msX, msY);
				break;
			case 2:
				DrawDialogBox_Inventory(msX, msY); //@@@
				break;
			case 3:
				DrawDialogBox_Magic(msX, msY, msZ); //@@@
				break;
			case 4:
				DrawDialogBox_ItemDrop(msX, msY); //@@@
				break;
			case 5:

				break;
			case 6:
				DrawDialogBox_WarningMsg(msX, msY); //@@@
				break;
			case 7:
				DrawDialogBox_GuildMenu(msX, msY);
				break;
			case 8:
				DrawDialogBox_GuildOperation(msX, msY);
				break;
			case 9:
				DrawDialogBox_GuideMap(msX, msY, cLB);
				break;
			case 10:
				DrawDialogBox_Chat(msX, msY, msZ, cLB); //@@@
				break;
			case 11:
				DrawDialogBox_Shop(msX, msY, msZ, cLB); //@@@
				break;
			case 12:
				DrawDialogBox_LevelUpSetting(msX, msY); //@@@
				break;
			case 13:
				DrawDialogBox_CityHallMenu(msX, msY);
				break;
			case 14:
				DrawDialogBox_Bank(msX, msY, msZ, cLB); //@@@
				break;
			case 15:
				DrawDialogBox_Skill(msX, msY, msZ, cLB); //@@@
				break;
			case 16:
				DrawDialogBox_MagicShop(msX, msY, msZ); //@@@
				break;
			case 17:
				DrawDialogBox_QueryDropItemAmount();
				break;
			case 18:
				DrawDialogBox_Text(msX, msY, msZ, cLB); //@@@
				break;
			case 19:
				DrawDialogBox_SysMenu(msX, msY, cLB); //@@@
				break;
			case 20:
				DrawDialogBox_NpcActionQuery(msX, msY); //@@@
				break;
			case 21:
				DrawDialogBox_NpcTalk(msX, msY, cLB); //@@@
				break;
			case 22:
				DrawDialogBox_Map();
				break;
			case 23:
				DrawDialogBox_SellorRepairItem(msX, msY); //@@@
				break;
			case 24:
				DrawDialogBox_Fishing(msX, msY);
				break;
			case 25:
				DrawDialogBox_ShutDownMsg(msX, msY); //@@@
				break;
			case 26: // Manuf
				DrawDialogBox_SkillDlg(msX, msY, msZ, cLB);
				break;
			case 27:
				DrawDialogBox_Exchange(msX, msY); //@@@
				break;
			case 28:
				DrawDialogBox_Quest(msX, msY); //@@@
				break;
			case 29:
				DrawDialogBox_GaugePannel(); //@@@
				break;
			case 30:
				DrawDialogBox_IconPannel(msX, msY); //@@@
				break;
			case 31:
				DrawDialogBox_SellList(msX, msY); //@@@
				break;
			case 32:
				DrawDialogBox_Party(msX, msY); //@@@
				break;
			case 33:
				DrawDialogBox_CrusadeJob(msX, msY); //@@@
				break;
			case 34:
				DrawDialogBox_ItemUpgrade(msX, msY);
				break;
			case 35:
				DrawDialogBox_Help(msX, msY); //@@@
				break;
			case 36:
				DrawDialogBox_Commander(msX, msY); //@@@
				break;
			case 37:
				DrawDialogBox_Constructor(msX, msY); //@@@
				break;
			case 38:
				DrawDialogBox_Soldier(msX, msY); //@@@
				break;
			case 40:
				DrawDialogBox_Slates(msX, msY, msZ, cLB);
				break;
			case 41:	//Snoopy: Confirmation Exchange
				DrawDialogBox_ConfirmExchange(msX, msY);
				break;
			case 42:
				DrawDialogBox_ChangeStatsMajestic(msX, msY);
				break;
			case 43:
				DrawDialogBox_FriendList(msX, msY); //drajwer: friendlist
				break;
			case 44:
				DrawDialogBox_Enchanting(msX, msY);
				break;
			case 45: // kamal
				DrawDialogBox_SetTrap(msX, msY);
				break;
			case 46: // VAMP - arena restart
				DrawDialogBox_ArenaRestart(msX, msY, msZ, cLB);
				break;
			case 47:
				DrawDialogBox_SummonGuild(msX, msY);
				break;
			case 50: // Snoopy: Resurection?
				DrawDialogBox_Resurect(msX, msY);
				break;
			case 51: // Gail
				DrawDialogBox_CMDHallMenu(msX, msY);
				break;
			case 52: //50Cent - Repair All
				DrawDialogBox_RepairAll(msX, msY, msZ); //@@@
				break;
			case 53: //Magn0S:: General Panel
				DrawDialogBox_GeneralPanel(msX, msY, msZ, cLB);
				break;
			case 56: //Magn0S:: GMPanel
				DrawDialogBox_GMPanel(msX, msY, msZ, cLB); //@@@
				break;
			case 57: // MORLA 2.4 - SHOP 2
				DrawDialogBox_Shop2(msX, msY, msZ, cLB); //@@@
				break;

			case 58: // Centu - Guild Warehouse
				//DrawDialogBox_GuildBank(msX, msY, msZ, cLB); //@@@
				break;

			case 59:
				DrawDialogBox_QuestList(msX, msY, msZ, cLB); //@@@
				break;

			case 60:
				DrawDialogBox_OnlineUsers(msX, msY, msZ, cLB);
				break;
			}
		}
	int resi, resx;
#ifdef RES_HIGH
	resi = 120;
	resx = 80;
#else
	resi = 0;
#endif
	if (m_cSkillMastery[_iGetWeaponSkillType()] == 100)
	{
		if (m_iSuperAttackLeft > 0)
		{
			if (GetAsyncKeyState(VK_MENU) >> 15)
				m_pSprite[DEF_SPRID_INTERFACE_ND_ICONPANNEL2]->PutTransSprite(368 + resx + 7, 440 + resi, 3, m_dwCurTime);
			wsprintf(G_cTxt, "%d", m_iSuperAttackLeft);
			PutString_SprFont2(380 + resx + 10, 454 + resi, G_cTxt, 220, 200, 200);
		}
	}
	else
	{
		if (m_iSuperAttackLeft > 0)
		{
			wsprintf(G_cTxt, "%d", m_iSuperAttackLeft);
			PutString_SprFont2(380 + resx + 5, 454 + resi, G_cTxt, 10, 10, 10);
		}
	}

}

void CGame::_Draw_CharacterBody(short sX, short sY, short sType)
{
	UINT32 dwTime = m_dwCurTime;
	int  iR, iG, iB;

	if (sType <= 3)
	{
		m_pSprite[DEF_SPRID_ITEMEQUIP_PIVOTPOINT + 0]->PutSpriteFast(sX, sY, sType - 1, dwTime);
		_GetHairColorRGB(((_tmp_sAppr1 & 0x00F0) >> 4), &iR, &iG, &iB);
		m_pSprite[DEF_SPRID_ITEMEQUIP_PIVOTPOINT + 18]->PutSpriteRGB(sX, sY, (_tmp_sAppr1 & 0x0F00) >> 8, iR, iG, iB, dwTime);

		m_pSprite[DEF_SPRID_ITEMEQUIP_PIVOTPOINT + 19]->PutSpriteFast(sX, sY, (_tmp_sAppr1 & 0x000F), dwTime);
	}
	else
	{
		m_pSprite[DEF_SPRID_ITEMEQUIP_PIVOTPOINT + 40]->PutSpriteFast(sX, sY, sType - 4, dwTime);
		_GetHairColorRGB(((_tmp_sAppr1 & 0x00F0) >> 4), &iR, &iG, &iB);
		m_pSprite[DEF_SPRID_ITEMEQUIP_PIVOTPOINT + 18 + 40]->PutSpriteRGB(sX, sY, (_tmp_sAppr1 & 0x0F00) >> 8, iR, iG, iB, dwTime);
		m_pSprite[DEF_SPRID_ITEMEQUIP_PIVOTPOINT + 19 + 40]->PutSpriteFast(sX, sY, (_tmp_sAppr1 & 0x000F), dwTime);
	}

}

void CGame::GetPlayerTurn()
{
	char cDir;
	short sX, sY, sCnt1, sCnt2;
	int   iError;

	sX = m_sPlayerX;
	sY = m_sPlayerY;
	sCnt1 = 0;
	m_cPlayerTurn = 0;
	iError = 0;
	while (1) {
		cDir = cGetNextMoveDir(sX, sY, m_sCommX, m_sCommY);
		if (cDir == 0) break;
		switch (cDir) {
		case 1: sY--;       break;
		case 2: sX++; sY--; break;
		case 3: sX++;       break;
		case 4: sX++; sY++; break;
		case 5: sY++;       break;
		case 6: sX--; sY++; break;
		case 7: sX--;       break;
		case 8: sX--; sY--; break;
		}
		sCnt1++;
		if (sCnt1 > 30) break;
	}

	sX = m_sPlayerX;
	sY = m_sPlayerY;
	sCnt2 = 0;
	m_cPlayerTurn = 1;
	iError = 0;
	while (1) {
		cDir = cGetNextMoveDir(sX, sY, m_sCommX, m_sCommY);
		if (cDir == 0) break;
		switch (cDir) {
		case 1: sY--;       break;
		case 2: sX++; sY--; break;
		case 3: sX++;       break;
		case 4: sX++; sY++; break;
		case 5: sY++;       break;
		case 6: sX--; sY++; break;
		case 7: sX--;       break;
		case 8: sX--; sY--; break;
		}
		sCnt2++;
		if (sCnt2 > 30) break;
	}

	if (sCnt1 > sCnt2)
		m_cPlayerTurn = 0;
	else m_cPlayerTurn = 1;
}

int CGame::_iCheckDlgBoxFocus(short msX, short msY, char cButtonSide)
{
	int i;
	char         cDlgID;
	short        sX, sY;
	UINT32		  dwTime = m_dwCurTime;
	if (cButtonSide == 1) {
		// Snoopy: 41->61
		for (i = 0; i < 61; i++)
			// Snoopy: 40->60
			if (m_cDialogBoxOrder[60 - i] != 0) 	// Snoopy: 40->60
			{
				cDlgID = m_cDialogBoxOrder[60 - i];
				if ((m_stDialogBoxInfo[cDlgID].sX <= msX) && ((m_stDialogBoxInfo[cDlgID].sX + m_stDialogBoxInfo[cDlgID].sSizeX) >= msX) &&
					(m_stDialogBoxInfo[cDlgID].sY <= msY) && ((m_stDialogBoxInfo[cDlgID].sY + m_stDialogBoxInfo[cDlgID].sSizeY) >= msY))
				{
					EnableDialogBox(cDlgID, 0, 0, 0);

					m_stMCursor.sPrevX = msX;
					m_stMCursor.sPrevY = msY;
					m_stMCursor.sDistX = msX - m_stDialogBoxInfo[cDlgID].sX;
					m_stMCursor.sDistY = msY - m_stDialogBoxInfo[cDlgID].sY;

					switch (cDlgID) {
					case 1:
						if (bDlgBoxPress_Character(msX, msY) == false) {
							m_stMCursor.cSelectedObjectType = DEF_SELECTEDOBJTYPE_DLGBOX;
							m_stMCursor.sSelectedObjectID = cDlgID;
						}
						break;

					case 3:
					case 4:
					case 5:
					case 6:
					case 7:
					case 8:
					case 9:

					case 12:
					case 13:
					case 16:
					case 17:
					case 20:
					case 22:
					case 23:
					case 24:
					case 25:
					case 28:
					case 29:
					case 30:
					case 31:
					case 32:
					case 33:
					case 34:
					case 35:
					case 36:
					case 37:
					case 38:
					case 40:
					case 50: // resur
					case 44:
						// NPC
					case 45: // kamal
					case 67:
					case 68:
					case 69:
					case 27:
					case 41: //Snoopy: Drag exchange confirmation dialog
					case 42:  // Snoopy: Drag majestic stats
					case 60:
					case 43: // Drajwer - friendlist s
					case 51:  // Snoopy: Drag Gail menu
					case 56: //Magn0S:: Add focus on GM Panel
					case 53: // General Panel

						m_stMCursor.cSelectedObjectType = DEF_SELECTEDOBJTYPE_DLGBOX;
						m_stMCursor.sSelectedObjectID = cDlgID;
						break;

					case 2:	// (Sell Item)
						if (bDlgBoxPress_Inventory(msX, msY) == false)
						{
							m_stMCursor.cSelectedObjectType = DEF_SELECTEDOBJTYPE_DLGBOX;
							m_stMCursor.sSelectedObjectID = cDlgID;
						}
						break;

					case 10:
						sX = m_stDialogBoxInfo[10].sX;
						sY = m_stDialogBoxInfo[10].sY;
						if ((msX >= sX + 340) && (msX <= sX + 360) && (msY >= sY + 22) && (msY <= sY + 138)) {
							m_stDialogBoxInfo[10].bIsScrollSelected = true;
							return -1;
						}

						if (m_stDialogBoxInfo[10].bIsScrollSelected == false) {
							m_stMCursor.cSelectedObjectType = DEF_SELECTEDOBJTYPE_DLGBOX;
							m_stMCursor.sSelectedObjectID = cDlgID;
						}
						else return -1;
						break;

					case 11:
						sX = m_stDialogBoxInfo[11].sX;
						sY = m_stDialogBoxInfo[11].sY;
						if ((m_stDialogBoxInfo[11].cMode == 0) && (msX >= sX + 240) && (msX <= sX + 260) && (msY >= sY + 20) && (msY <= sY + 330)) {
							m_stDialogBoxInfo[11].bIsScrollSelected = true;
							return -1;
						}

						if ((m_stDialogBoxInfo[11].bIsScrollSelected == false)) {
							m_stMCursor.cSelectedObjectType = DEF_SELECTEDOBJTYPE_DLGBOX;
							m_stMCursor.sSelectedObjectID = cDlgID;
						}
						else return -1;
						break;

					case 14:
						sX = m_stDialogBoxInfo[14].sX;
						sY = m_stDialogBoxInfo[14].sY;
						if ((msX >= sX + 240) && (msX <= sX + 260) && (msY >= sY + 40) && (msY <= sY + 320)) {
							m_stDialogBoxInfo[14].bIsScrollSelected = true;
							return -1;
						}

						if (m_stDialogBoxInfo[14].bIsScrollSelected == false) {
							m_stMCursor.cSelectedObjectType = DEF_SELECTEDOBJTYPE_DLGBOX;
							m_stMCursor.sSelectedObjectID = cDlgID;
						}
						else return -1;
						break;

					case 15:
						sX = m_stDialogBoxInfo[15].sX;
						sY = m_stDialogBoxInfo[15].sY;
						if ((msX >= sX + 240) && (msX <= sX + 260) && (msY >= sY + 40) && (msY <= sY + 320))
						{
							m_stDialogBoxInfo[15].bIsScrollSelected = true;
							return -1;
						}
						if (m_stDialogBoxInfo[15].bIsScrollSelected == false)
						{
							m_stMCursor.cSelectedObjectType = DEF_SELECTEDOBJTYPE_DLGBOX;
							m_stMCursor.sSelectedObjectID = cDlgID;
						}
						else return -1;
						break;

					case 18:
						sX = m_stDialogBoxInfo[18].sX;
						sY = m_stDialogBoxInfo[18].sY;
						if ((msX >= sX + 240) && (msX <= sX + 260) && (msY >= sY + 40) && (msY <= sY + 320)) {
							m_stDialogBoxInfo[18].bIsScrollSelected = true;
							return -1;
						}

						if (m_stDialogBoxInfo[18].bIsScrollSelected == false) {
							m_stMCursor.cSelectedObjectType = DEF_SELECTEDOBJTYPE_DLGBOX;
							m_stMCursor.sSelectedObjectID = cDlgID;
						}
						else return -1;
						break;

					case 19:
						sX = m_stDialogBoxInfo[19].sX;
						sY = m_stDialogBoxInfo[19].sY;
						if ((msX >= sX + 126) && (msX <= sX + 238) && (msY >= sY + 122) && (msY <= sY + 138)) {
							m_stDialogBoxInfo[19].bIsScrollSelected = true;
							return -1;
						}
						if ((msX >= sX + 126) && (msX <= sX + 238) && (msY >= sY + 139) && (msY <= sY + 155)) {
							m_stDialogBoxInfo[19].bIsScrollSelected = true;
							return -1;
						}

						if (m_stDialogBoxInfo[19].bIsScrollSelected == false) {
							m_stMCursor.cSelectedObjectType = DEF_SELECTEDOBJTYPE_DLGBOX;
							m_stMCursor.sSelectedObjectID = cDlgID;
						}
						else return -1;
						break;

					case 21:
						sX = m_stDialogBoxInfo[21].sX;
						sY = m_stDialogBoxInfo[21].sY;
						if ((msX >= sX + 240) && (msX <= sX + 260) && (msY >= sY + 40) && (msY <= sY + 320)) {
							m_stDialogBoxInfo[21].bIsScrollSelected = true;
							return -1;
						}

						if (m_stDialogBoxInfo[21].bIsScrollSelected == false) {
							m_stMCursor.cSelectedObjectType = DEF_SELECTEDOBJTYPE_DLGBOX;
							m_stMCursor.sSelectedObjectID = cDlgID;
						}
						else return -1;
						break;

					case 26:
						if (bDlgBoxPress_SkillDlg(msX, msY) == false)
						{
							m_stMCursor.cSelectedObjectType = DEF_SELECTEDOBJTYPE_DLGBOX;
							m_stMCursor.sSelectedObjectID = cDlgID;
						}
						break;

					
					case 57: // MORLA 2.4 - shop 2
						sX = m_stDialogBoxInfo[57].sX;
						sY = m_stDialogBoxInfo[57].sY;
						if ((m_stDialogBoxInfo[57].cMode == 0) && (msX >= sX + 240) && (msX <= sX + 260) && (msY >= sY + 20) && (msY <= sY + 330)) {
							m_stDialogBoxInfo[57].bIsScrollSelected = true;
							return -1;
						}

						if ((m_stDialogBoxInfo[57].bIsScrollSelected == false)) {
							m_stMCursor.cSelectedObjectType = DEF_SELECTEDOBJTYPE_DLGBOX;
							m_stMCursor.sSelectedObjectID = cDlgID;
						}
						else return -1;
						break;

					case 59: // Magn0S - Quest List
						sX = m_stDialogBoxInfo[59].sX;
						sY = m_stDialogBoxInfo[59].sY;
						if ((m_stDialogBoxInfo[59].cMode == 0) && (msX >= sX + 405) && (msX <= sX + 420) && (msY >= sY + 20) && (msY <= sY + 330)) {
							m_stDialogBoxInfo[59].bIsScrollSelected = true;
							return -1;
						}

						if ((m_stDialogBoxInfo[59].bIsScrollSelected == false)) {
							m_stMCursor.cSelectedObjectType = DEF_SELECTEDOBJTYPE_DLGBOX;
							m_stMCursor.sSelectedObjectID = cDlgID;
						}
						else return -1;
						break;
					}
					return 1;
				}
			}
		return 0;
	}
	else if (cButtonSide == 2)
	{
		if ((dwTime - m_dwDialogCloseTime) < 300) return 0;
		// Snoopy: 40->60
		for (i = 0; i < 61; i++)
			// Snoopy: 40->60
			if (m_cDialogBoxOrder[60 - i] != 0) {
				// Snoopy: 40->60
				cDlgID = m_cDialogBoxOrder[60 - i];
				if ((m_stDialogBoxInfo[cDlgID].sX < msX) && ((m_stDialogBoxInfo[cDlgID].sX + m_stDialogBoxInfo[cDlgID].sSizeX) > msX) &&
					(m_stDialogBoxInfo[cDlgID].sY < msY) && ((m_stDialogBoxInfo[cDlgID].sY + m_stDialogBoxInfo[cDlgID].sSizeY) > msY))
				{
					if ((cDlgID != 5) && (cDlgID != 6) && (cDlgID != 8) && (cDlgID != 12) && ((cDlgID != 23) || (m_stDialogBoxInfo[23].cMode < 3)) && (cDlgID != 24) && (cDlgID != 27) && (cDlgID != 34) &&
						(cDlgID != 33) && !((cDlgID == 32) && ((m_stDialogBoxInfo[cDlgID].cMode == 1) || (m_stDialogBoxInfo[cDlgID].cMode == 3))))
						DisableDialogBox(cDlgID);
					m_dwDialogCloseTime = dwTime;
					return 1;
				}
			}
	}
	return 0;
}

void CGame::InitItemList(char * pData)
{
	char    cTotalItems;
	int     i, iAngelValue, *ip;
	short * sp;
	UINT32 * dwp;
	UINT16  * wp;
	char  * cp;

	for (i = 0; i < DEF_MAXITEMS; i++)
		m_cItemOrder[i] = -1;

	for (i = 0; i < DEF_MAXITEMEQUIPPOS; i++)
		m_sItemEquipmentStatus[i] = -1;

	for (i = 0; i < DEF_MAXITEMS; i++)
		m_bIsItemDisabled[i] = false;

	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

	cTotalItems = *cp;
	cp++;

	for (i = 0; i < DEF_MAXITEMS; i++)
		if (m_pItemList[i] != 0)
		{
			delete m_pItemList[i];
			m_pItemList[i] = 0;
		}

	for (i = 0; i < DEF_MAXBANKITEMS; i++)
		if (m_pBankList[i] != 0)
		{
			delete m_pBankList[i];
			m_pBankList[i] = 0;
		}

	for (i = 0; i < cTotalItems; i++)
	{
		m_pItemList[i] = new class CItem;
		memcpy(m_pItemList[i]->m_cName, cp, 20);
		cp += 20;
		dwp = (UINT32 *)cp;
		m_pItemList[i]->m_dwCount = *dwp;
		m_pItemList[i]->m_sX = 40;
		m_pItemList[i]->m_sY = 30;
		cp += 4;
		m_pItemList[i]->m_cItemType = *cp;
		cp++;
		m_pItemList[i]->m_cEquipPos = *cp;
		cp++;
		if (*cp == 0) m_bIsItemEquipped[i] = false;
		else m_bIsItemEquipped[i] = true;
		cp++;
		if (m_bIsItemEquipped[i] == true)
		{
			m_sItemEquipmentStatus[m_pItemList[i]->m_cEquipPos] = i;
		}
		sp = (short *)cp;
		m_pItemList[i]->m_sLevelLimit = *sp;
		cp += 2;
		m_pItemList[i]->m_cGenderLimit = *cp;
		cp++;
		wp = (UINT16 *)cp;
		m_pItemList[i]->m_wCurLifeSpan = *wp;
		cp += 2;
		wp = (UINT16 *)cp;
		m_pItemList[i]->m_wWeight = *wp;
		cp += 2;
		sp = (short *)cp;
		m_pItemList[i]->m_sSprite = *sp;
		cp += 2;
		sp = (short *)cp;
		m_pItemList[i]->m_sSpriteFrame = *sp;
		cp += 2;
		m_pItemList[i]->m_cItemColor = *cp;
		cp++;
		m_pItemList[i]->m_sItemSpecEffectValue2 = (short)*cp; // v1.41
		cp++;
		dwp = (UINT32 *)cp;
		m_pItemList[i]->m_dwAttribute = *dwp;
		cp += 4;

		sp = (short*)cp;
		m_pItemList[i]->m_sNewEffect1 = *sp;
		cp += 2;

		sp = (short*)cp;
		m_pItemList[i]->m_sNewEffect2 = *sp;
		cp += 2;

		sp = (short*)cp;
		m_pItemList[i]->m_sNewEffect3 = *sp;
		cp += 2;

		sp = (short*)cp;
		m_pItemList[i]->m_sNewEffect4 = *sp;
		cp += 2;

		sp = (short*)cp;
		m_pItemList[i]->m_sItemEffectValue1 = *sp;
		cp += 2;

		sp = (short*)cp;
		m_pItemList[i]->m_sItemEffectValue2 = *sp;
		cp += 2;

		sp = (short*)cp;
		m_pItemList[i]->m_sItemEffectValue3 = *sp;
		cp += 2;

		sp = (short*)cp;
		m_pItemList[i]->m_sItemEffectValue4 = *sp;
		cp += 2;

		sp = (short*)cp;
		m_pItemList[i]->m_sItemEffectValue5 = *sp;
		cp += 2;

		sp = (short*)cp;
		m_pItemList[i]->m_sItemEffectValue6 = *sp;
		cp += 2;

		sp = (short*)cp;
		m_pItemList[i]->m_sItemEffectType = *sp;
		cp += 2;

		wp = (UINT16*)cp;
		m_pItemList[i]->m_wMaxLifeSpan = *wp;
		cp += 2;

		ip = (int*)cp;
		m_pItemList[i]->m_iClass = *ip;
		cp += 4;

		ip = (int*)cp;
		m_pItemList[i]->m_iReqStat = *ip;
		cp += 4;
		ip = (int*)cp;
		m_pItemList[i]->m_iQuantStat = *ip;
		cp += 4;

		m_cItemOrder[i] = i;
		
	}

	cTotalItems = *cp;
	cp++;

	for (i = 0; i < DEF_MAXBANKITEMS; i++)
		if (m_pBankList[i] != 0)
		{
			delete m_pBankList[i];
			m_pBankList[i] = 0;
		}

	for (i = 0; i < cTotalItems; i++)
	{
		m_pBankList[i] = new class CItem;
		memcpy(m_pBankList[i]->m_cName, cp, 20);
		cp += 20;

		dwp = (UINT32 *)cp;
		m_pBankList[i]->m_dwCount = *dwp;
		cp += 4;

		m_pBankList[i]->m_sX = 40;
		m_pBankList[i]->m_sY = 30;

		m_pBankList[i]->m_cItemType = *cp;
		cp++;

		m_pBankList[i]->m_cEquipPos = *cp;
		cp++;

		sp = (short *)cp;
		m_pBankList[i]->m_sLevelLimit = *sp;
		cp += 2;

		m_pBankList[i]->m_cGenderLimit = *cp;
		cp++;

		wp = (UINT16 *)cp;
		m_pBankList[i]->m_wCurLifeSpan = *wp;
		cp += 2;

		wp = (UINT16 *)cp;
		m_pBankList[i]->m_wWeight = *wp;
		cp += 2;

		sp = (short *)cp;
		m_pBankList[i]->m_sSprite = *sp;
		cp += 2;

		sp = (short *)cp;
		m_pBankList[i]->m_sSpriteFrame = *sp;
		cp += 2;

		m_pBankList[i]->m_cItemColor = *cp;
		cp++;

		m_pBankList[i]->m_sItemSpecEffectValue2 = (short)*cp; // v1.41
		cp++;

		dwp = (UINT32 *)cp;
		m_pBankList[i]->m_dwAttribute = *dwp;
		cp += 4;
		//Magn0S::
		sp = (short*)cp;
		m_pBankList[i]->m_sNewEffect1 = *sp;
		cp += 2;

		sp = (short*)cp;
		m_pBankList[i]->m_sNewEffect2 = *sp;
		cp += 2;

		sp = (short*)cp;
		m_pBankList[i]->m_sNewEffect3 = *sp;
		cp += 2;

		sp = (short*)cp;
		m_pBankList[i]->m_sNewEffect4 = *sp;
		cp += 2;

		sp = (short*)cp;
		m_pBankList[i]->m_sItemEffectValue1 = *sp;
		cp += 2;

		sp = (short*)cp;
		m_pBankList[i]->m_sItemEffectValue2 = *sp;
		cp += 2;

		sp = (short*)cp;
		m_pBankList[i]->m_sItemEffectValue3 = *sp;
		cp += 2;

		sp = (short*)cp;
		m_pBankList[i]->m_sItemEffectValue4 = *sp;
		cp += 2;

		sp = (short*)cp;
		m_pBankList[i]->m_sItemEffectValue5 = *sp;
		cp += 2;

		sp = (short*)cp;
		m_pBankList[i]->m_sItemEffectValue6 = *sp;
		cp += 2;

		sp = (short*)cp;
		m_pBankList[i]->m_sItemEffectType = *sp;
		cp += 2;

		wp = (UINT16*)cp;
		m_pBankList[i]->m_wMaxLifeSpan = *wp;
		cp += 2;

		ip = (int*)cp;
		m_pBankList[i]->m_iClass = *ip;
		cp += 4;

		ip = (int*)cp;
		m_pBankList[i]->m_iReqStat = *ip;
		cp += 4;
		ip = (int*)cp;
		m_pBankList[i]->m_iQuantStat = *ip;
		cp += 4;

	}

	// Magic, Skill Mastery
	for (i = 0; i < DEF_MAXMAGICTYPE; i++)
	{
		m_cMagicMastery[i] = *cp;
		cp++;
	}

	for (i = 0; i < DEF_MAXSKILLTYPE; i++)
	{
		m_cSkillMastery[i] = (unsigned char)*cp;
		if (m_pSkillCfgList[i] != 0)
			m_pSkillCfgList[i]->m_iLevel = (int)*cp;
		cp++;
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//	Snoopy: mp3 functions
//
////////////////////////////////////////////////////////////////////////////////////////
void CGame::LoopBGM()
{
	Mp3Event();
}
void CGame::PauseBGM()
{
	Mp3Pause();
}
void CGame::ResumeBGM()
{
	Mp3Resume();
}
void CGame::StopBGM()
{
	Mp3Stop();
}
void CGame::SetBGMVolume()
{
	int iVolume = (m_cMusicVolume - 100) * 20;
	if (iVolume > 0) iVolume = 0;
	if (iVolume < -10000) iVolume = -10000; //iVolume = Volume
	Mp3SetVolume(iVolume);
}
void CGame::StartBGM()
{
	if ((m_bSoundFlag == false) && (bPlayGhostMusic == false))
	{
		StopBGM();	// Snoopy: mp3 support
		if (m_pBGM != 0)
		{
			m_pBGM->bStop();
			delete m_pBGM;
			m_pBGM = 0;
		}
		return;
	}
	char cWavFileName[32];
	ZeroMemory(cWavFileName, sizeof(cWavFileName));
	if (m_cGameMode == DEF_GAMEMODE_ONMAINMENU || m_cGameMode == DEF_GAMEMODE_ONCREATENEWACCOUNT ||
		m_cGameMode == DEF_GAMEMODE_ONLOGIN || m_cGameMode == DEF_GAMEMODE_ONSELECTCHARACTER ||
		m_cGameMode == DEF_GAMEMODE_ONCREATENEWCHARACTER) {
		strcpy(cWavFileName, "music\\MainMenu.mp3");
	}
	else if (m_cGameMode == DEF_GAMEMODE_ONMAINGAME) {
		if (m_bIsXmas == true) strcpy(cWavFileName, "music\\Carol.wav");
		else
		{
			if (memcmp(m_cCurLocation, "aresden", 7) == 0) strcpy(cWavFileName, "music\\aresden.wav");
			else if (memcmp(m_cCurLocation, "elvine", 6) == 0) strcpy(cWavFileName, "music\\elvine.wav");
			else if (memcmp(m_cCurLocation, "dglv", 4) == 0) strcpy(cWavFileName, "music\\dungeon.wav");
			else if (memcmp(m_cCurLocation, "middled1", 8) == 0) strcpy(cWavFileName, "music\\dungeon.wav");
			else if (memcmp(m_cCurLocation, "middleland", 10) == 0) strcpy(cWavFileName, "music\\middleland.wav");
			// Snoopy: new musics
			else if (memcmp(m_cCurLocation, "druncncity", 10) == 0) strcpy(cWavFileName, "music\\druncncity.wav");
			else if (memcmp(m_cCurLocation, "inferniaA", 9) == 0) strcpy(cWavFileName, "music\\middleland.wav");
			else if (memcmp(m_cCurLocation, "inferniaB", 9) == 0) strcpy(cWavFileName, "music\\middleland.wav");
			else if (memcmp(m_cCurLocation, "maze", 4) == 0) strcpy(cWavFileName, "music\\dungeon.wav");
			else if (memcmp(m_cCurLocation, "abaddon", 7) == 0) {
				if (bPlayGhostMusic) {
					m_bSoundFlag = true;
					m_cMusicVolume = 100; //Magn0S:: Set max volume
					strcpy(cWavFileName, "music\\Ghost.wav");
				}
				else {
					strcpy(cWavFileName, "music\\abaddon.wav");
				}
			}
			else if (memcmp(m_cCurLocation, "stadium", 7) == 0) strcpy(cWavFileName, "music\\stadium.mp3");
			else if (memcmp(m_cCurLocation, "lost", 4) == 0) strcpy(cWavFileName, "music\\lost.mp3");
			else if (memcmp(m_cCurLocation, "catacombs", 9) == 0) strcpy(cWavFileName, "music\\lost1.mp3");
			else if (memcmp(m_cCurLocation, "qusmarsh", 8) == 0) strcpy(cWavFileName, "music\\lost2.mp3");
			else if (memcmp(m_cCurLocation, "asgarde", 7) == 0) strcpy(cWavFileName, "music\\lost3.mp3");

			else strcpy(cWavFileName, "music\\MainTm.wav");

		}
	}

	//Snoopy: mp3 support
	if (Mp3Playing(cWavFileName) == false)
	{
		Mp3Stop();
		Mp3Load(cWavFileName);

		int iVolume = (m_cMusicVolume - 100) * 20;
		if (iVolume > 0) iVolume = 0;
		if (iVolume < -10000) iVolume = -10000; //iVolume = Volume
		Mp3SetVolume(iVolume);
		if (m_bIsProgramActive == true)
		{
			Mp3Play();
		}
		else
		{
			Mp3Play();
			PauseBGM();
		}
	}
}

//Magn0S:: Add to start Special Music on Ghost Abaddon fight
/*void CGame::StartBGMGhost()
{
	char cWavFileName[32];
	ZeroMemory(cWavFileName, sizeof(cWavFileName));

	StopBGM();
	if (m_bSoundFlag == false) m_bSoundFlag = true;

	if (m_cGameMode == DEF_GAMEMODE_ONMAINGAME) {
		if ((memcmp(m_cCurLocation, "abaddon", 7) == 0)) strcpy(cWavFileName, "music\\Ghost.wav");

	}
	//Snoopy: mp3 support
	if (Mp3Playing(cWavFileName) == false)
	{
		Mp3Stop();
		Mp3Load(cWavFileName);

		m_cMusicVolume = 100; //Magn0S:: Set max volume

		int iVolume = (m_cMusicVolume - 100) * 20;
		if (iVolume > 0) iVolume = 0;
		if (iVolume < -10000) iVolume = -10000; //iVolume = Volume
		Mp3SetVolume(iVolume);
		if (m_bIsProgramActive == true)
		{
			Mp3Play();
		}
		else
		{
			Mp3Play();
			PauseBGM();
		}
	}
}*/

void CGame::MotionResponseHandler(char * pData)
{
	UINT16  * wp, wResponse;
	short * sp, sX, sY;
	char  * cp, cDir;
	int   * ip, iPreHP;
	//						          0 3        4 5						 6 7		8 9		   10	    11
	// Confirm Code(4) | MsgSize(4) | MsgID(4) | DEF_OBJECTMOVE_CONFIRM(2) | Loc-X(2) | Loc-Y(2) | Dir(1) | MapData ...
	// Confirm Code(4) | MsgSize(4) | MsgID(4) | DEF_OBJECTMOVE_REJECT(2)  | Loc-X(2) | Loc-Y(2)
	wp = (UINT16 *)(pData + DEF_INDEX2_MSGTYPE);
	wResponse = *wp;

	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

	switch (wResponse) {
	case DEF_OBJECTMOTION_CONFIRM:
	case DEF_OBJECTMOTION_ATTACK_CONFIRM:
		m_cCommandCount--;
		break;

	case DEF_OBJECTMOTION_REJECT:

		if (m_iHP <= 0) return;

		sp = (short *)cp;
		m_sPlayerX = *sp;
		cp += 2;

		sp = (short *)cp;
		m_sPlayerY = *sp;
		cp += 2;

		m_cCommand = DEF_OBJECTSTOP;
		m_sCommX = m_sPlayerX;
		m_sCommY = m_sPlayerY;

		m_pMapData->bSetOwner(m_sPlayerObjectID, m_sPlayerX, m_sPlayerY, m_sPlayerType, m_cPlayerDir,
			m_sPlayerAppr1, m_sPlayerAppr2, m_sPlayerAppr3, m_sPlayerAppr4, m_iPlayerApprColor,
			m_iPlayerStatus, m_cPlayerName,
			DEF_OBJECTSTOP, 0, 0, 0);
		m_cCommandCount = 0;
		m_bIsGetPointingMode = false;
#ifdef RES_HIGH
		m_sViewDstX = m_sViewPointX = (m_sPlayerX - 12) * 32 - 16;
		m_sViewDstY = m_sViewPointY = (m_sPlayerY - 9) * 32 - 16;
#else
		m_sViewDstX = m_sViewPointX = (m_sPlayerX - 10) * 32;
		m_sViewDstY = m_sViewPointY = (m_sPlayerY - 7) * 32;
#endif
		m_bIsRedrawPDBGS = true;
		break;

	case DEF_OBJECTMOVE_CONFIRM:
		sp = (short *)cp;
		sX = *sp;
		cp += 2;
		sp = (short *)cp;
		sY = *sp;
		cp += 2;
		cDir = *cp;
		cp++;
		m_iSP = m_iSP - *cp;
		cp++;
		if (m_iSP < 0) m_iSP = 0;
		// v1.3
		cp++;
		// v1.4
		iPreHP = m_iHP;
		ip = (int *)cp;
		m_iHP = *ip;
		cp += 4;

		if (m_iHP != iPreHP)
		{
			if (m_iHP < iPreHP)
			{
				wsprintf(G_cTxt, NOTIFYMSG_HP_DOWN, iPreHP - m_iHP);
				AddEnergyList(G_cTxt, 2);
				m_dwDamagedTime = timeGetTime();
				if ((m_cLogOutCount>0) && (m_bForceDisconn == false))
				{
					m_cLogOutCount = -1;
					AddEventList(MOTION_RESPONSE_HANDLER2, 10);
				}
			}
			else
			{
				wsprintf(G_cTxt, NOTIFYMSG_HP_UP, m_iHP - iPreHP);
				AddEnergyList(G_cTxt, 2);
			}
		}
		m_pMapData->ShiftMapData(cDir);
		_ReadMapData(sX, sY, cp);
		m_bIsRedrawPDBGS = true;
		m_cCommandCount--;
		break;

	case DEF_OBJECTMOVE_REJECT:
		if (m_iHP <= 0) return;
		wp = (UINT16 *)cp;
		if (m_sPlayerObjectID != *wp) return;
		cp += 2;
		sp = (short *)cp;
		m_sPlayerX = *sp;
		cp += 2;
		sp = (short *)cp;
		m_sPlayerY = *sp;
		cp += 2;
		sp = (short *)cp;
		m_sPlayerType = *sp;
		cp += 2;
		m_cPlayerDir = *cp;
		cp++;
		cp += 10;
		sp = (short *)cp;
		m_sPlayerAppr1 = *sp;
		cp += 2;
		sp = (short *)cp;
		m_sPlayerAppr2 = *sp;
		cp += 2;
		sp = (short *)cp;
		m_sPlayerAppr3 = *sp;
		cp += 2;
		sp = (short *)cp;
		m_sPlayerAppr4 = *sp;
		cp += 2;
		ip = (int *)cp;
		m_iPlayerApprColor = *ip;
		cp += 4;
		ip = (int *)cp;
		m_iPlayerStatus = *ip;
		cp += 4;
		m_cCommand = DEF_OBJECTSTOP;
		m_sCommX = m_sPlayerX;
		m_sCommY = m_sPlayerY;
		m_pMapData->bSetOwner(m_sPlayerObjectID, m_sPlayerX, m_sPlayerY, m_sPlayerType, m_cPlayerDir,
			m_sPlayerAppr1, m_sPlayerAppr2, m_sPlayerAppr3, m_sPlayerAppr4, m_iPlayerApprColor, // v1.4
			m_iPlayerStatus, m_cPlayerName,
			DEF_OBJECTSTOP, 0, 0, 0,
			0, 7);
		m_cCommandCount = 0;
		m_bIsGetPointingMode = false;
#ifdef RES_HIGH
		m_sViewDstX = m_sViewPointX = (m_sPlayerX - 12) * 32 - 16;
		m_sViewDstY = m_sViewPointY = (m_sPlayerY - 9) * 32 - 16;
#else
		m_sViewDstX = m_sViewPointX = (m_sPlayerX - 10) * 32;
		m_sViewDstY = m_sViewPointY = (m_sPlayerY - 7) * 32;
#endif
		m_bIsPrevMoveBlocked = true;
		switch (m_sPlayerType) {
		case 1:
		case 2:
		case 3:
			PlaySound('C', 12, 0);
			break;
		case 4:
		case 5:
		case 6:
			PlaySound('C', 13, 0);
			break;
		}
		break;
	}
}

void CGame::DrawAngel(int iSprite, short sX, short sY, char cFrame, UINT32 dwTime)
{
	switch (_tmp_cDir)
	{
	case 1:
	case 2:
	case 7:
	case 8:
		sX -= 30;
		break;
	}
	if ((_tmp_iStatus & 0x10) != 0)
	{
		if ((_tmp_iStatus & 0x1000) != 0)
			m_pSprite[DEF_SPRID_TUTELARYANGELS_PIVOTPOINT + iSprite]->PutTransSprite(sX, sY, cFrame, dwTime);  //AngelicPendant(STR)
		else if ((_tmp_iStatus & 0x2000) != 0)
			m_pSprite[DEF_SPRID_TUTELARYANGELS_PIVOTPOINT + (50 * 1) + iSprite]->PutTransSprite(sX, sY, cFrame, dwTime); //AngelicPendant(DEX)
		else if ((_tmp_iStatus & 0x4000) != 0)
			m_pSprite[DEF_SPRID_TUTELARYANGELS_PIVOTPOINT + (50 * 2) + iSprite]->PutTransSprite(sX, sY - 15, cFrame, dwTime);//AngelicPendant(INT)
		else if ((_tmp_iStatus & 0x8000) != 0)
			m_pSprite[DEF_SPRID_TUTELARYANGELS_PIVOTPOINT + (50 * 3) + iSprite]->PutTransSprite(sX, sY - 15, cFrame, dwTime);//AngelicPendant(MAG)
	}
	else
	{
		if ((_tmp_iStatus & 0x1000) != 0)
			m_pSprite[DEF_SPRID_TUTELARYANGELS_PIVOTPOINT + iSprite]->PutSpriteFast(sX, sY, cFrame, dwTime);  //AngelicPendant(STR)
		else if ((_tmp_iStatus & 0x2000) != 0)
			m_pSprite[DEF_SPRID_TUTELARYANGELS_PIVOTPOINT + (50 * 1) + iSprite]->PutSpriteFast(sX, sY, cFrame, dwTime); //AngelicPendant(DEX)
		else if ((_tmp_iStatus & 0x4000) != 0)
			m_pSprite[DEF_SPRID_TUTELARYANGELS_PIVOTPOINT + (50 * 2) + iSprite]->PutSpriteFast(sX, sY - 15, cFrame, dwTime);//AngelicPendant(INT)
		else if ((_tmp_iStatus & 0x8000) != 0)
			m_pSprite[DEF_SPRID_TUTELARYANGELS_PIVOTPOINT + (50 * 3) + iSprite]->PutSpriteFast(sX, sY - 15, cFrame, dwTime);//AngelicPendant(MAG)
	}
}

void CGame::EnableDialogBox(int iBoxID, int cType, int sV1, int sV2, char * pString)
{
	int i;
	short sX, sY;

	switch (iBoxID) {
	case 10: // Magn0S:: Update chat mode
		if (m_bIsDialogEnabled[10] == false) {
			m_stDialogBoxInfo[10].cMode = cType;
			m_stDialogBoxInfo[10].sV1 = sV1;
			m_stDialogBoxInfo[10].sV2 = sV2;
		}
		break;

	case 11:
		if (m_bIsDialogEnabled[11] == false)
		{
			switch (cType) {
			case 0:
				break;
			default:
				iNewShop = 0;
				_LoadShopMenuContents(cType);
				m_stDialogBoxInfo[11].sV1 = cType;
				m_stDialogBoxInfo[11].cMode = 0;
				m_stDialogBoxInfo[11].sView = 0;
				m_stDialogBoxInfo[11].bFlag = true;
				m_stDialogBoxInfo[11].sV3 = 1;
				break;
			}
		}
		break;

	case 12: // levelup diag
		if (m_bIsDialogEnabled[12] == false)
		{
			m_stDialogBoxInfo[12].sX = m_stDialogBoxInfo[1].sX + 20;
			m_stDialogBoxInfo[12].sY = m_stDialogBoxInfo[1].sY + 20;
			m_stDialogBoxInfo[12].sV1 = m_iLU_Point;
		}
		break;

	case 3: // Magic Dialog
		break;

	case 4:
		if (m_bIsDialogEnabled[4] == false) {
			m_stDialogBoxInfo[4].sView = cType;
		}
		break;

	case 5:
		if (m_bIsDialogEnabled[5] == false) {
			m_stDialogBoxInfo[5].sView = cType;
		}
		break;

	case 6:
		if (m_bIsDialogEnabled[6] == false) {
			m_stDialogBoxInfo[6].sView = cType;
		}
		break;

	case 7:
		if (m_stDialogBoxInfo[7].cMode == 1) {
			sX = m_stDialogBoxInfo[7].sX;
			sY = m_stDialogBoxInfo[7].sY;
			EndInputString();
			StartInputString(sX + 75, sY + 140, 21, m_cGuildName);
		}
		break;



	case 17: // demande quantit�
		if (m_bIsDialogEnabled[17] == false)
		{
			m_stDialogBoxInfo[iBoxID].cMode = 1;
			m_stDialogBoxInfo[17].sView = cType;
			EndInputString();
			ZeroMemory(m_cAmountString, sizeof(m_cAmountString));
			wsprintf(m_cAmountString, "%d", sV1);
			sX = m_stDialogBoxInfo[17].sX;
			sY = m_stDialogBoxInfo[17].sY;
			StartInputString(sX + 40, sY + 57, 11, m_cAmountString, false);
		}
		else
		{
			if (m_stDialogBoxInfo[17].cMode == 1)
			{
				sX = m_stDialogBoxInfo[17].sX;
				sY = m_stDialogBoxInfo[17].sY;
				EndInputString();
				StartInputString(sX + 40, sY + 57, 11, m_cAmountString, false);
			}
		}
		break;

	case 18:
		if (m_bIsDialogEnabled[18] == false)
		{
			switch (cType) {
			case 0:
				m_stDialogBoxInfo[18].cMode = 0;
				m_stDialogBoxInfo[18].sView = 0;
				break;
			default:
				_LoadTextDlgContents(cType);
				m_stDialogBoxInfo[18].cMode = 0;
				m_stDialogBoxInfo[18].sView = 0;
				break;
			}
		}
		break;

	case 19:
		break;

	case 20: // Talk to npc or unicorn
		m_bIsItemDisabled[m_stDialogBoxInfo[20].sV1] = false;
		if (m_bIsDialogEnabled[20] == false)
		{
			m_stDialogBoxInfo[11].sV1 = m_stDialogBoxInfo[11].sV2 = m_stDialogBoxInfo[11].sV3 =
				m_stDialogBoxInfo[11].sV4 = m_stDialogBoxInfo[11].sV5 = m_stDialogBoxInfo[11].sV6 = 0;
			m_stDialogBoxInfo[20].cMode = cType;
			m_stDialogBoxInfo[20].sView = 0;
			m_stDialogBoxInfo[20].sV1 = sV1;
			m_stDialogBoxInfo[20].sV2 = sV2;
		}
		break;

	case 21:
		if (m_bIsDialogEnabled[21] == false)
		{
			m_stDialogBoxInfo[21].cMode = cType;
			m_stDialogBoxInfo[21].sView = 0;
			m_stDialogBoxInfo[21].sV1 = _iLoadTextDlgContents2(sV1 + 20);
			m_stDialogBoxInfo[21].sV2 = sV1 + 20;
		}
		break;

	case 22:
		if (m_bIsDialogEnabled[22] == false) {
			m_stDialogBoxInfo[22].sV1 = sV1;
			m_stDialogBoxInfo[22].sV2 = sV2;

			m_stDialogBoxInfo[22].sSizeX = 290;
			m_stDialogBoxInfo[22].sSizeY = 290;
		}
		break;

	case 23:
		if (m_bIsDialogEnabled[23] == false) {
			m_stDialogBoxInfo[23].cMode = cType;
			m_stDialogBoxInfo[23].sV1 = sV1;		// ItemID
			m_stDialogBoxInfo[23].sV2 = sV2;
			if (cType == 2)
			{
				m_stDialogBoxInfo[23].sX = m_stDialogBoxInfo[11].sX;
				m_stDialogBoxInfo[23].sY = m_stDialogBoxInfo[11].sY;
			}
		}
		break;

	case 15:
		break;

	case 24:
		if (m_bIsDialogEnabled[24] == false)
		{
			m_stDialogBoxInfo[24].cMode = cType;
			m_stDialogBoxInfo[24].sV1 = sV1;
			m_stDialogBoxInfo[24].sV2 = sV2;
			m_bSkillUsingStatus = true;
		}
		break;

	case 25:
		if (m_bIsDialogEnabled[25] == false) {
			m_stDialogBoxInfo[25].cMode = cType;
			m_stDialogBoxInfo[25].sV1 = sV1;
			m_stDialogBoxInfo[25].sV2 = sV2;
		}
		break;

	case 26:
		switch (cType) {
		case 1:
		case 2: //
			if (m_bIsDialogEnabled[26] == false)
			{
				m_stDialogBoxInfo[26].cMode = cType;
				m_stDialogBoxInfo[26].sV1 = -1;
				m_stDialogBoxInfo[26].sV2 = -1;
				m_stDialogBoxInfo[26].sV3 = -1;
				m_stDialogBoxInfo[26].sV4 = -1;
				m_stDialogBoxInfo[26].sV5 = -1;
				m_stDialogBoxInfo[26].sV6 = -1;
				m_stDialogBoxInfo[26].cStr[0] = 0;
				m_bSkillUsingStatus = true;
				m_stDialogBoxInfo[26].sSizeX = 195;
				m_stDialogBoxInfo[26].sSizeY = 215;
				DisableDialogBox(17);
				DisableDialogBox(20);
				DisableDialogBox(23);
			}
			break;

		case 3:	//
			if (m_bIsDialogEnabled[26] == false)
			{
				m_stDialogBoxInfo[26].sView = 0;
				m_stDialogBoxInfo[26].cMode = cType;
				m_stDialogBoxInfo[26].sV1 = -1;
				m_stDialogBoxInfo[26].sV2 = -1;
				m_stDialogBoxInfo[26].sV3 = -1;
				m_stDialogBoxInfo[26].sV4 = -1;
				m_stDialogBoxInfo[26].sV5 = -1;
				m_stDialogBoxInfo[26].sV6 = -1;
				m_stDialogBoxInfo[26].cStr[0] = 0;
				m_stDialogBoxInfo[26].cStr[1] = 0;
				m_stDialogBoxInfo[26].cStr[4] = 0;
				m_bSkillUsingStatus = true;
				_bCheckBuildItemStatus();
				m_stDialogBoxInfo[26].sSizeX = 270;
				m_stDialogBoxInfo[26].sSizeY = 381;
				DisableDialogBox(17);
				DisableDialogBox(20);
				DisableDialogBox(23);
			}
			break;

		case 6:
			if (m_bIsDialogEnabled[26] == false)
			{
				m_stDialogBoxInfo[26].cMode = cType;
				m_stDialogBoxInfo[26].cStr[2] = sV1;
				m_stDialogBoxInfo[26].cStr[3] = sV2;
				m_stDialogBoxInfo[26].sSizeX = 270;
				m_stDialogBoxInfo[26].sSizeY = 381;
				m_bSkillUsingStatus = true;
				_bCheckBuildItemStatus();
				DisableDialogBox(17);
				DisableDialogBox(20);
				DisableDialogBox(23);
			}
			break;
			// Crafting
		case 7:
		case 8:
			if (m_bIsDialogEnabled[26] == false)
			{
				m_stDialogBoxInfo[26].cMode = cType;
				m_stDialogBoxInfo[26].sV1 = -1;
				m_stDialogBoxInfo[26].sV2 = -1;
				m_stDialogBoxInfo[26].sV3 = -1;
				m_stDialogBoxInfo[26].sV4 = -1;
				m_stDialogBoxInfo[26].sV5 = -1;
				m_stDialogBoxInfo[26].sV6 = -1;
				m_stDialogBoxInfo[26].cStr[0] = 0;
				m_stDialogBoxInfo[26].cStr[1] = 0;
				m_bSkillUsingStatus = true;
				m_stDialogBoxInfo[26].sSizeX = 195;
				m_stDialogBoxInfo[26].sSizeY = 215;
				DisableDialogBox(17);
				DisableDialogBox(20);
				DisableDialogBox(23);
			}
			break;
		}
		break;

	case 27: // Snoopy: 7 mar 06 (multitrade) case rewriten
		if (m_bIsDialogEnabled[27] == false)
		{
			m_stDialogBoxInfo[27].cMode = cType;
#ifdef MULTI_TRADE
			for (i = 0; i < 16; i++)//50Cent - MultiTrade
#else
			for (i = 0; i<8; i++)
#endif
			{
				ZeroMemory(m_stDialogBoxExchangeInfo[i].cStr1, sizeof(m_stDialogBoxExchangeInfo[i].cStr1));
				ZeroMemory(m_stDialogBoxExchangeInfo[i].cStr2, sizeof(m_stDialogBoxExchangeInfo[i].cStr2));
				m_stDialogBoxExchangeInfo[i].sV1 = -1;
				m_stDialogBoxExchangeInfo[i].sV2 = -1;
				m_stDialogBoxExchangeInfo[i].sV3 = -1;
				m_stDialogBoxExchangeInfo[i].sV4 = -1;
				m_stDialogBoxExchangeInfo[i].sV5 = -1;
				m_stDialogBoxExchangeInfo[i].sV6 = -1;
				m_stDialogBoxExchangeInfo[i].sV7 = -1;
				m_stDialogBoxExchangeInfo[i].dwV1 = 0;
			}
			DisableDialogBox(17);
			DisableDialogBox(20);
			DisableDialogBox(23);
			DisableDialogBox(26);
		}
		break;

	case 41: // Snoopy: 7 mar 06 (MultiTrade) Confirmation dialog
		break;

	case 28:
		if (m_bIsDialogEnabled[28] == false) {
			m_stDialogBoxInfo[28].cMode = cType;
			m_stDialogBoxInfo[28].sX = m_stDialogBoxInfo[1].sX + 20;
			m_stDialogBoxInfo[28].sY = m_stDialogBoxInfo[1].sY + 20;
		}
		break;

	case 32:
		if (m_bIsDialogEnabled[32] == false) {
			m_stDialogBoxInfo[32].cMode = cType;
			m_stDialogBoxInfo[32].sX = m_stDialogBoxInfo[1].sX + 20;
			m_stDialogBoxInfo[32].sY = m_stDialogBoxInfo[1].sY + 20;
		}
		break;

	case 33:
		if ((m_iHP <= 0) || (m_bCitizen == false)) return;
		if (m_bIsDialogEnabled[33] == false)
		{
			m_stDialogBoxInfo[33].cMode = cType;
			m_stDialogBoxInfo[33].sX = 360 + SCREENX;
			m_stDialogBoxInfo[33].sY = 65 + SCREENY;
			m_stDialogBoxInfo[33].sV1 = sV1;
		}
		break;

	case 44:
		if (m_bIsDialogEnabled[44] == false)
		{
			m_stDialogBoxInfo[44].cMode = 1;
			m_stDialogBoxInfo[44].sV2 = -1;
			m_stDialogBoxInfo[44].sV3 = -1;
			m_stDialogBoxInfo[44].sV1 = -1;
			m_stDialogBoxInfo[44].dwV1 = 0;
		}
		break;

	case 34:
		if (m_bIsDialogEnabled[34] == false)
		{
			m_stDialogBoxInfo[34].cMode = cType;
			m_stDialogBoxInfo[34].sV1 = -1;
			m_stDialogBoxInfo[34].dwV1 = 0;
		}
		else if (m_bIsDialogEnabled[34] == false)
		{
			int iSoX, iSoM;
			iSoX = iSoM = 0;
			for (i = 0; i < DEF_MAXITEMS; i++)
				if (m_pItemList[i] != 0)
				{
					if ((m_pItemList[i]->m_sSprite == 6) && (m_pItemList[i]->m_sSpriteFrame == 128)) iSoX++;
					if ((m_pItemList[i]->m_sSprite == 6) && (m_pItemList[i]->m_sSpriteFrame == 129)) iSoM++;
				}
			if ((iSoX > 0) || (iSoM > 0))
			{
				m_stDialogBoxInfo[34].cMode = 6; // Stone upgrade
				m_stDialogBoxInfo[34].sV2 = iSoX;
				m_stDialogBoxInfo[34].sV3 = iSoM;
				m_stDialogBoxInfo[34].sV1 = -1;
				m_stDialogBoxInfo[34].dwV1 = 0;
			}
			else if (m_iGizonItemUpgradeLeft > 0)
			{
				m_stDialogBoxInfo[34].cMode = 1;
				m_stDialogBoxInfo[34].sV2 = -1;
				m_stDialogBoxInfo[34].sV3 = -1;
				m_stDialogBoxInfo[34].sV1 = -1;
				m_stDialogBoxInfo[34].dwV1 = 0;
			}
			else
			{
				AddEventList(DRAW_DIALOGBOX_ITEMUPGRADE30, 10); // "Stone of Xelima or Merien is not present."
				return;
			}
		}
		break;

	case 16:
		if (m_bIsDialogEnabled[iBoxID] == false) {
			if (m_cSkillMastery[4] == 0) {
				DisableDialogBox(16);
				EnableDialogBox(21, 0, 480, 0);
				return;
			}
			else {
				m_stDialogBoxInfo[iBoxID].cMode = 0;
				m_stDialogBoxInfo[iBoxID].sView = 0;
			}
		}
		break;

	case 14:
		EndInputString();
		if (m_bIsDialogEnabled[iBoxID] == false) {
			m_stDialogBoxInfo[iBoxID].cMode = 0;
			m_stDialogBoxInfo[iBoxID].sView = 0;
			EnableDialogBox(2, 0, 0, 0);
		}
		break;

	case 40: // Slates
		if (m_bIsDialogEnabled[40] == false) {
			m_stDialogBoxInfo[40].sView = 0;
			m_stDialogBoxInfo[40].cMode = cType;
			m_stDialogBoxInfo[40].sV1 = -1;
			m_stDialogBoxInfo[40].sV2 = -1;
			m_stDialogBoxInfo[40].sV3 = -1;
			m_stDialogBoxInfo[40].sV4 = -1;
			m_stDialogBoxInfo[40].sV5 = -1;
			m_stDialogBoxInfo[40].sV6 = -1;
			m_stDialogBoxInfo[40].cStr[0] = 0;
			m_stDialogBoxInfo[40].cStr[1] = 0;
			m_stDialogBoxInfo[40].cStr[4] = 0;

			m_stDialogBoxInfo[40].sSizeX = 180;
			m_stDialogBoxInfo[40].sSizeY = 183;

			DisableDialogBox(17);
			DisableDialogBox(20);
			DisableDialogBox(23);
			DisableDialogBox(26);
		}
		break;
	case 42: // Diuuude: Change stats window
		if (m_bIsDialogEnabled[42] == false) {
			m_stDialogBoxInfo[42].sX = m_stDialogBoxInfo[12].sX + 10;
			m_stDialogBoxInfo[42].sY = m_stDialogBoxInfo[12].sY + 10;
			m_stDialogBoxInfo[42].cMode = 0;
			m_stDialogBoxInfo[42].sView = 0;
			m_bSkillUsingStatus = false;
		}
		break;

	case 43:
		if (m_bIsDialogEnabled[43] == false) {
			m_stDialogBoxInfo[43].sX = 337;
			m_stDialogBoxInfo[43].sY = 57;
			m_iFriendIndex = -1;
			m_stDialogBoxInfo[43].sV1 = 0;
			m_stDialogBoxInfo[43].cMode = 0;
		}
		break;

	case 46: // VAMP - arena restart
		if (m_bIsDialogEnabled[46] == false)
		{
			m_stDialogBoxInfo[46].sX = 185;
			m_stDialogBoxInfo[46].sY = 200;
			m_stDialogBoxInfo[46].cMode = 0;
			m_stDialogBoxInfo[46].sView = 0;
			m_bSkillUsingStatus = false;
		}
		break;

	case 47: 
		if (m_bIsDialogEnabled[47] == false)
		{
			m_stDialogBoxInfo[47].sX = 185;
			m_stDialogBoxInfo[47].sY = 200;
			m_stDialogBoxInfo[47].cMode = 0;
			m_stDialogBoxInfo[47].sView = 0;
			m_bSkillUsingStatus = false;
		}
		break;

	case 50: // Snoopy: Resurection
		if (m_bIsDialogEnabled[50] == false)
		{
			m_stDialogBoxInfo[50].sX = 185;
			m_stDialogBoxInfo[50].sY = 100;
			m_stDialogBoxInfo[50].cMode = 0;
			m_stDialogBoxInfo[50].sView = 0;
			m_bSkillUsingStatus = false;
		}
		break;

	case 53: // Magn0S:: General Panel
		if (m_bIsDialogEnabled[53] == false)
		{
			m_stDialogBoxInfo[53].cMode = cType;
		}
		break;

	case 52: //50Cent - Repair all
		m_stDialogBoxInfo[52].cMode = cType;
		break;

	case 56: //Magn0S:: Update GM Panel
		if (m_bIsDialogEnabled[56] == false)
		{
			m_stDialogBoxInfo[56].cMode = cType;
			m_stDialogBoxInfo[56].sV3 = 0;
			ZeroMemory(cCharSelection, sizeof(cCharSelection));
			bListSelected = false;
		}
		break;

	case 57: // MORLA 2.4 - Shop 2
		if (m_bIsDialogEnabled[57] == false)
		{
			switch (cType) {
			case 0:
				break;
			default:
				//Magn0S:: Split Trades
				switch (iNewShop) {
				case 1:
					_LoadShopMenuContents(0); // Eks Trade
					break;
				case 2:
					_LoadShopMenuContents(3); // Contrib Trade
					break;
				case 3:
					_LoadShopMenuContents(4); // Coins Trade
					break;
				default: 
					_LoadShopMenuContents(0); // Eks Trade 
					break;
				}
				m_stDialogBoxInfo[57].sV1 = 1;
				m_stDialogBoxInfo[57].cMode = 0;
				m_stDialogBoxInfo[57].sView = 0;
				m_stDialogBoxInfo[57].bFlag = true;
				m_stDialogBoxInfo[57].sV3 = 1;
				break;
			}
		}
		break;

	case 58: // Centuu : Guild Warehouse
		/*EndInputString();
		if (m_bIsDialogEnabled[iBoxID] == false) {
			m_stDialogBoxInfo[iBoxID].cMode = 0;
			m_stDialogBoxInfo[iBoxID].sView = 0;
			EnableDialogBox(2, 0, 0, 0);
		}*/
		break;

	case 59: // Magn0S - Quest List
		if (m_bIsDialogEnabled[59] == false)
		{
			m_stDialogBoxInfo[59].sV1 = cType;
			m_stDialogBoxInfo[59].cMode = 0;
			m_stDialogBoxInfo[59].sView = 0;
			m_stDialogBoxInfo[59].bFlag = true;
			m_stDialogBoxInfo[59].sV3 = 1;

			break;
		}
		break;

	case 67:
		break;

	default:
		EndInputString();
		if (m_bIsDialogEnabled[iBoxID] == false) {
			m_stDialogBoxInfo[iBoxID].cMode = 0;
			m_stDialogBoxInfo[iBoxID].sView = 0;
		}
		break;
	}
	if (iBoxID != 30)
	{
		if (m_bIsDialogEnabled[iBoxID] == false)
		{
			if (m_stDialogBoxInfo[iBoxID].sY > 400) m_stDialogBoxInfo[iBoxID].sY = 410;
			if (m_stDialogBoxInfo[iBoxID].sX > 620) m_stDialogBoxInfo[iBoxID].sX = 620;
			if ((m_stDialogBoxInfo[iBoxID].sX + m_stDialogBoxInfo[iBoxID].sSizeX) < 10) m_stDialogBoxInfo[iBoxID].sX += 20;
			if ((m_stDialogBoxInfo[iBoxID].sY + m_stDialogBoxInfo[iBoxID].sSizeY) < 10) m_stDialogBoxInfo[iBoxID].sY += 20;
		}
	}
	m_bIsDialogEnabled[iBoxID] = true;
	if (pString != 0) strcpy(m_stDialogBoxInfo[iBoxID].cStr, pString);
	//Snoopy: 39->59
	for (i = 0; i < 59; i++)
		if (m_cDialogBoxOrder[i] == iBoxID) m_cDialogBoxOrder[i] = 0;
	//Snoopy: 39->59
	for (i = 0; i < 59; i++)
		if ((m_cDialogBoxOrder[i - 1] == 0) && (m_cDialogBoxOrder[i] != 0)) {
			m_cDialogBoxOrder[i - 1] = m_cDialogBoxOrder[i];
			m_cDialogBoxOrder[i] = 0;
		}
	//Snoopy: 39->59
	for (i = 0; i < 59; i++)
		if (m_cDialogBoxOrder[i] == 0) {
			m_cDialogBoxOrder[i] = iBoxID;
			return;
		}
}

// VAMP - arena restart
void CGame::DrawDialogBox_ArenaRestart(short msX, short msY, short msZ, char cLB)
{
	short sX, sY;

	sX = m_stDialogBoxInfo[46].sX;
	sY = m_stDialogBoxInfo[46].sY;
	DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME5, sX, sY, 2);

	PutString_SprFont(sX + 90, sY + 25, "Continue?", 7, 0, 0); // previously 1,1,8

	//PutString(sX + 50, sY + 20, "You have died in the Gladiator Arena", RGB(4,0,50));
	//PutString(sX + 80, sY + 35, "Do you want to continue?", RGB(4,0,50));

	if ((msX >= sX + 30) && (msX <= sX + 30 + DEF_BTNSZX) && (msY >= sY + 55) && (msY <= sY + 55 + DEF_BTNSZY))
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + 30, sY + 55, 19);
	else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + 30, sY + 55, 18);

	if ((msX >= sX + 170) && (msX <= sX + 170 + DEF_BTNSZX) && (msY >= sY + 55) && (msY <= sY + 55 + DEF_BTNSZY))
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + 170, sY + 55, 3);
	else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + 170, sY + 55, 2);
}


void CGame::DlgBoxClick_ArenaRestart(short msX, short msY)
{
	short sX, sY;

	char G_cTxt[64];

	sX = m_stDialogBoxInfo[46].sX;
	sY = m_stDialogBoxInfo[46].sY;
	if ((msX >= sX + 30) && (msX <= sX + 30 + DEF_BTNSZX) && (msY >= sY + 55) && (msY <= sY + 55 + DEF_BTNSZY))
	{   // yes
		m_cRestartCount = 5;
		m_dwRestartCountTime = timeGetTime();
		DisableDialogBox(46);
		wsprintf(G_cTxt, DLGBOX_CLICK_SYSMENU1, m_cRestartCount); // "Restarting game....%d"
		AddEventList(G_cTxt, 10);
		PlaySound('E', 14, 5);
	}
	else if ((msX >= sX + 170) && (msX <= sX + 170 + DEF_BTNSZX) && (msY >= sY + 55) && (msY <= sY + 55 + DEF_BTNSZY))
	{	// no
		bSendCommand(MSGID_REQUEST_LEAVEARENA, 0, 0, 0, 0, 0, 0);
		DisableDialogBox(46);
		PlaySound('E', 14, 5);
	}
}

void CGame::DrawDialogBox_SummonGuild(short msX, short msY)
{
	short sX, sY;
	char cTemp[51];

	sX = m_stDialogBoxInfo[47].sX;
	sY = m_stDialogBoxInfo[47].sY;
	DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME5, sX, sY, 2);

	PutString(sX + 10, sY + 20, "Your guild has requested your presence,", RGB(4,0,50));
	wsprintf(cTemp,	"Do you want to teleport? Respond in %d sec..", sSummonGuild);
	PutString(sX + 10, sY + 35, cTemp, RGB(4,0,50));

	if ((msX >= sX + 30) && (msX <= sX + 30 + DEF_BTNSZX) && (msY >= sY + 55) && (msY <= sY + 55 + DEF_BTNSZY))
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + 30, sY + 55, 19);
	else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + 30, sY + 55, 18);

	if ((msX >= sX + 170) && (msX <= sX + 170 + DEF_BTNSZX) && (msY >= sY + 55) && (msY <= sY + 55 + DEF_BTNSZY))
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + 170, sY + 55, 3);
	else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTON2, sX + 170, sY + 55, 2);
}


void CGame::DlgBoxClick_SummonGuild(short msX, short msY)
{
	short sX, sY;

	sX = m_stDialogBoxInfo[47].sX;
	sY = m_stDialogBoxInfo[47].sY;
	if ((msX >= sX + 30) && (msX <= sX + 30 + DEF_BTNSZX) && (msY >= sY + 55) && (msY <= sY + 55 + DEF_BTNSZY))
	{   // yes
		bSendCommand(MSGID_REQUEST_SUMMONGUILD);
		DisableDialogBox(47);
		PlaySound('E', 14, 5);
	}
	else if ((msX >= sX + 170) && (msX <= sX + 170 + DEF_BTNSZX) && (msY >= sY + 55) && (msY <= sY + 55 + DEF_BTNSZY))
	{	// no
		DisableDialogBox(47);
		PlaySound('E', 14, 5);
	}
}

void CGame::DisableDialogBox(int iBoxID)
{
	int i;

	switch (iBoxID) {
	case 4:
		m_bIsItemDisabled[m_stDialogBoxInfo[4].sView] = false;
		break;

	case 5:
		m_bIsItemDisabled[m_stDialogBoxInfo[5].sView] = false;
		break;

	case 6:
		m_bIsItemDisabled[m_stDialogBoxInfo[6].sView] = false;
		break;

	case 7:
		if (m_stDialogBoxInfo[7].cMode == 1)
			EndInputString();
		m_stDialogBoxInfo[7].cMode = 0;
		break;



	case 11:
		for (i = 0; i < DEF_MAXMENUITEMS; i++)
			if (m_pItemForSaleList[i] != 0) {
				delete m_pItemForSaleList[i];
				m_pItemForSaleList[i] = 0;
			}
		m_stDialogBoxInfo[39].sV3 = 0;
		m_stDialogBoxInfo[39].sV4 = 0; // v1.4
		m_stDialogBoxInfo[39].sV5 = 0;
		m_stDialogBoxInfo[39].sV6 = 0;
		break;

	case 14:
		if (m_stDialogBoxInfo[14].cMode < 0) return;
		break;

	case 17:
		if (m_stDialogBoxInfo[17].cMode == 1) {
			EndInputString();
			m_bIsItemDisabled[m_stDialogBoxInfo[17].sView] = false;
		}
		break;

	case 20: // v1.4
		m_bIsItemDisabled[m_stDialogBoxInfo[20].sV1] = false;
		break;

	case 21:
		if (m_stDialogBoxInfo[21].sV2 == 500)
		{
			bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_GETMAGICABILITY, 0, 0, 0, 0, 0);
		}
		break;

	case 24:
		m_bSkillUsingStatus = false;
		break;

	case 26:
		if (m_stDialogBoxInfo[26].sV1 != -1) m_bIsItemDisabled[m_stDialogBoxInfo[26].sV1] = false;
		if (m_stDialogBoxInfo[26].sV2 != -1) m_bIsItemDisabled[m_stDialogBoxInfo[26].sV2] = false;
		if (m_stDialogBoxInfo[26].sV3 != -1) m_bIsItemDisabled[m_stDialogBoxInfo[26].sV3] = false;
		if (m_stDialogBoxInfo[26].sV4 != -1) m_bIsItemDisabled[m_stDialogBoxInfo[26].sV4] = false;
		if (m_stDialogBoxInfo[26].sV5 != -1) m_bIsItemDisabled[m_stDialogBoxInfo[26].sV5] = false;
		if (m_stDialogBoxInfo[26].sV6 != -1) m_bIsItemDisabled[m_stDialogBoxInfo[26].sV6] = false;
		m_bSkillUsingStatus = false;
		break;

	case 27: //Snoopy: 7 mar 06 (multiTrade) case rewriten
#ifdef MULTI_TRADE
		for (i = 0; i < 16; i++)//50Cent - MultiTrade
#else
		for (i = 0; i<8; i++)
#endif
		{
			ZeroMemory(m_stDialogBoxExchangeInfo[i].cStr1, sizeof(m_stDialogBoxExchangeInfo[i].cStr1));
			ZeroMemory(m_stDialogBoxExchangeInfo[i].cStr2, sizeof(m_stDialogBoxExchangeInfo[i].cStr2));
			m_stDialogBoxExchangeInfo[i].sV1 = -1;
			m_stDialogBoxExchangeInfo[i].sV2 = -1;
			m_stDialogBoxExchangeInfo[i].sV3 = -1;
			m_stDialogBoxExchangeInfo[i].sV4 = -1;
			m_stDialogBoxExchangeInfo[i].sV5 = -1;
			m_stDialogBoxExchangeInfo[i].sV6 = -1;
			m_stDialogBoxExchangeInfo[i].sV7 = -1;
			m_stDialogBoxExchangeInfo[i].dwV1 = 0;
			if (m_bIsItemDisabled[m_stDialogBoxExchangeInfo[i].sItemID] == true)
				m_bIsItemDisabled[m_stDialogBoxExchangeInfo[i].sItemID] = false;
		}
		break;


	case 31:
		for (i = 0; i < DEF_MAXSELLLIST; i++)
		{
			if (m_stSellItemList[i].iIndex != -1) m_bIsItemDisabled[m_stSellItemList[i].iIndex] = false;
			m_stSellItemList[i].iIndex = -1;
			m_stSellItemList[i].iAmount = 0;
		}
		break;

	case 44:
		if (m_stDialogBoxInfo[44].sV1 != -1)
			m_bIsItemDisabled[m_stDialogBoxInfo[44].sV1] = false;
		break;

	case 34:
		if (m_stDialogBoxInfo[34].sV1 != -1)
			m_bIsItemDisabled[m_stDialogBoxInfo[34].sV1] = false;
		break;

	case 40:
		m_bIsItemDisabled[m_stDialogBoxInfo[40].sV1] = false;
		m_bIsItemDisabled[m_stDialogBoxInfo[40].sV2] = false;
		m_bIsItemDisabled[m_stDialogBoxInfo[40].sV3] = false;
		m_bIsItemDisabled[m_stDialogBoxInfo[40].sV4] = false;

		ZeroMemory(m_stDialogBoxInfo[40].cStr, sizeof(m_stDialogBoxInfo[40].cStr));
		ZeroMemory(m_stDialogBoxInfo[40].cStr2, sizeof(m_stDialogBoxInfo[40].cStr2));
		ZeroMemory(m_stDialogBoxInfo[40].cStr3, sizeof(m_stDialogBoxInfo[40].cStr3));
		ZeroMemory(m_stDialogBoxInfo[40].cStr4, sizeof(m_stDialogBoxInfo[40].cStr4));
		m_stDialogBoxInfo[40].sV1 = -1;
		m_stDialogBoxInfo[40].sV2 = -1;
		m_stDialogBoxInfo[40].sV3 = -1;
		m_stDialogBoxInfo[40].sV4 = -1;
		m_stDialogBoxInfo[40].sV5 = -1;
		m_stDialogBoxInfo[40].sV6 = -1;
		m_stDialogBoxInfo[40].sV9 = -1;
		m_stDialogBoxInfo[40].sV10 = -1;
		m_stDialogBoxInfo[40].sV11 = -1;
		m_stDialogBoxInfo[40].sV12 = -1;
		m_stDialogBoxInfo[40].sV13 = -1;
		m_stDialogBoxInfo[40].sV14 = -1;
		m_stDialogBoxInfo[40].dwV1 = 0;
		m_stDialogBoxInfo[40].dwV2 = 0;
		break;

	case 42:
		cStateChange1 = 0;
		cStateChange2 = 0;
		cStateChange3 = 0;

		break;

	case 47:
		sSummonGuild = 0;
		bSummonGuild = false;
		break;

	}

	m_bIsDialogEnabled[iBoxID] = false;
	// Snoopy: 39->59
	for (i = 0; i < 59; i++)
		if (m_cDialogBoxOrder[i] == iBoxID)
			m_cDialogBoxOrder[i] = 0;

	// Snoopy: 39->59
	for (i = 1; i < 59; i++)
		if ((m_cDialogBoxOrder[i - 1] == 0) && (m_cDialogBoxOrder[i] != 0))
		{
			m_cDialogBoxOrder[i - 1] = m_cDialogBoxOrder[i];
			m_cDialogBoxOrder[i] = 0;
		}
}

int CGame::iGetTopDialogBoxIndex()
{
	int i;
	//Snoopy: 38->58
	for (i = 58; i >= 0; i--)
		if (m_cDialogBoxOrder[i] != 0)
			return m_cDialogBoxOrder[i];

	return 0;
}

void CGame::DlgBoxClick_IconPannel(short msX, short msY)
{

	short sX, sY;
	int resi, resx;
#ifdef RES_HIGH
	resi = 120;
	resx = 80;
	int addx = 10;
#else
	resi = 0;
#endif
	sX = m_stDialogBoxInfo[30].sX;
	sY = m_stDialogBoxInfo[30].sY;

	if ((msX > 322 + resx + addx) && (msX < 355 + resx + addx) && (434 + resi < msY) && (475 + resi > msY)) {
		// Crusade
		if (m_bIsCrusadeMode == false) return;
		switch (m_iCrusadeDuty) {
		case 1: // Fighter
			EnableDialogBox(38, 0, 0, 0);
			break;

		case 2: // Constructor
			EnableDialogBox(37, 0, 0, 0);
			break;

		case 3: // Commander
			EnableDialogBox(36, 0, 0, 0);
			break;

		default:
			break;
		}
		PlaySound('E', 14, 5);
	}

	if ((362 + resx + addx < msX) && (404 + resx + addx > msX) && (434 + resi < msY) && (475 + resi > msY)) {
		// Combat Mode Toggle
		bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_TOGGLECOMBATMODE, 0, 0, 0, 0, 0);
		PlaySound('E', 14, 5);
	}

	// Character
	if ((413 + resx + addx < msX) && (447 + resx + addx > msX) && (434 + resi < msY) && (475 + resi > msY)) {
		if (m_bIsDialogEnabled[1] == true)
			DisableDialogBox(1);
		else EnableDialogBox(1, 0, 0, 0);
		PlaySound('E', 14, 5);
	}

	// Inventory
	if ((447 + resx + addx < msX) && (484 + resx + addx > msX) && (434 + resi < msY) && (475 + resi > msY)) {
		if (m_bIsDialogEnabled[2] == true)
			DisableDialogBox(2);
		else EnableDialogBox(2, 0, 0, 0);
		PlaySound('E', 14, 5);
	}

	// Magic
	if ((484 + resx + addx < msX) && (521 + resx + addx > msX) && (434 + resi < msY) && (475 + resi > msY)) {
		if (m_bIsDialogEnabled[3] == true)
			DisableDialogBox(3);
		else EnableDialogBox(3, 0, 0, 0);
		PlaySound('E', 14, 5);
	}

	// Skill
	if ((521 + resx + addx < msX) && (558 + resx + addx > msX) && (434 + resi < msY) && (475 + resi > msY)) {
		if (m_bIsDialogEnabled[15] == true)
			DisableDialogBox(15);
		else {
			bSendCommand(MSGID_REQUEST_SKILLPOINT);
			EnableDialogBox(15, 0, 0, 0);
		}
		PlaySound('E', 14, 5);
	}

	// Chat
	if ((558 + resx + addx < msX) && (595 + resx + addx > msX) && (434 + resi < msY) && (475 + resi > msY)) {
		if (m_bIsDialogEnabled[10] == true)
			DisableDialogBox(10);
		else EnableDialogBox(10, 0, 0, 0);
		PlaySound('E', 14, 5);
	}

	// System Menu
	if ((595 + resx + addx < msX) && (631 + resx + addx > msX) && (434 + resi < msY) && (475 + resi > msY)) {
		if (m_bIsDialogEnabled[19] == true)
			DisableDialogBox(19);
		else EnableDialogBox(19, 0, 0, 0);
		PlaySound('E', 14, 5);
	}
}

//New GM Panel by Magn0S :D
void CGame::DlgBoxClick_GeneralPanel(short msX, short msY)
{
	short sX, sY;
	int tmp, i;
	char cTxt[120], cMapTP[30];
	int iNext = 0;
	int FixChat;
	sX = m_stDialogBoxInfo[53].sX;
	sY = m_stDialogBoxInfo[53].sY;
	bool bFlag = false;

	char cTemp[255], cTemp2[255], cStr2[255], cStr3[255];
	int x = 0;

	ZeroMemory(cMapTP, sizeof(cMapTP));

#ifdef RES_HIGH
	FixChat = 532;
#else
	FixChat = 414;
#endif

	switch (m_stDialogBoxInfo[53].cMode) {
	case 0:
		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			bSendCommand(MSGID_REQUEST_TOPEK, 0, 0, 0, 0, 0, 0);
			m_stDialogBoxInfo[53].cMode = 1; // Top Ek
			PlaySound('E', 14, 5);
		}
		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			m_stDialogBoxInfo[53].cMode = 2; // Event Schedule
			PlaySound('E', 14, 5);
		}
		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			m_stDialogBoxInfo[53].cMode = 3; // Game Config
			PlaySound('E', 14, 5);
		}
		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			bSendCommand(MSGID_REQUEST_ONLINE);	 //ahora es lo mismo poner eso que todos esos 0, 0 etc
			EnableDialogBox(60, 0, 0, 0);
			PlaySound('E', 14, 5);
		}
		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			//Friend List
			if (m_bIsDialogEnabled[43] == false)
			{
				EnableDialogBox(43, 0, 0, 0);
				LoadFriendList();
			}
			else DisableDialogBox(43);
			PlaySound('E', 14, 5);
		}
		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59)) {
			if (m_iGuildRank == -1) return;
			m_stDialogBoxInfo[53].cMode = 40; // Guild Menu
			PlaySound('E', 14, 5);
		}

		break;

	case 1:
		//Back
		if ((msX > sX + 220) && (msX < sX + 240) && (msY > sY + 315) && (msY < sY + 330)) {
			m_stDialogBoxInfo[53].cMode = 0; // Return
			PlaySound('E', 14, 5);
		}
		break;

	case 2:
		iNext += 14;
		if ((msX >= sX + 20) && (msX <= sX + 240) && (msY >= sY + iNext * 17 + 35) && (msY <= sY + iNext * 17 + 49)) {
			//m_DDraw.DrawShadowBox(sX + 20, sY + iNext * 17 + 35, sX + 240, sY + iNext * 17 + 55, 1, true);
			if (bServerTime)
				bServerTime = false;
			else bServerTime = true;
		}

		//LifeX Fix BACK button for Event Schedule
		if ((msX > sX + 210) && (msX < sX + 240) && (msY > sY + 315) && (msY < sY + 330)) {
			m_stDialogBoxInfo[53].cMode = 0;
			PlaySound('E', 14, 5);
		}
		break;

	case 3:
		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59))
			if (m_bShowParty) m_bShowParty = false; else m_bShowParty = true;

		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59))
			if (bShowEventInfo) bShowEventInfo = false; else bShowEventInfo = true;

		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59))
			if (m_bShowEmblems) m_bShowEmblems = false; else m_bShowEmblems = true;

		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59))
			if (m_bShowTrees) m_bShowTrees = false; else m_bShowTrees = true;

		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59))
			ChangeBigItems();

		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59))
			if (m_bNpcMap) m_bNpcMap = false; else m_bNpcMap = true;

		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59))
			if (m_bQuestHelper) m_bQuestHelper = false; else m_bQuestHelper = true;

		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59))
			if (m_bUseOldPanels) m_bUseOldPanels = false; else m_bUseOldPanels = true;

		

		//Back
		if ((msX > sX + 220) && (msX < sX + 240) && (msY > sY + 315) && (msY < sY + 330)) {
			m_stDialogBoxInfo[53].cMode = 0; // Return
			PlaySound('E', 14, 5);
		}

		break;
	case 40:
		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59))
		{
			if (m_iGuildRank != 0 && m_iGuildRank != 3)
			{
				AddEventList("Only available for GuildMaster or Captain.", 10);
				return;
			}
			bSendCommand(MSGID_REQUEST_ONLINE);
			m_stDialogBoxInfo[53].cMode = 41; // Return
			PlaySound('E', 14, 5);
		}
		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59))
		{
			if (m_iGuildRank != 0 && m_iGuildRank != 2)
			{
				AddEventList("Only available for GuildMaster or Summoner.", 10);
				return;
			}
			bSendCommand(MSGID_COMMAND_CHATMSG, 0, 0, 0, 0, 0, "/summonguild");

		}
		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59))
		{
			bSendCommand(MSGID_REQUEST_ONLINE);
			m_stDialogBoxInfo[53].cMode = 43; // Return
			PlaySound('E', 14, 5);
		}

		//Back
		if ((msX > sX + 220) && (msX < sX + 240) && (msY > sY + 315) && (msY < sY + 330)) {
			m_stDialogBoxInfo[53].cMode = 0; // Return
			PlaySound('E', 14, 5);
		}
		break;
	case 41:
		x = 0;
		for (i = 0; i < 17; i++)
		{
			if (((i + m_stDialogBoxInfo[53].sView) < DEF_MAXCLIENTS) && (m_pOnlineUsersList[i + m_stDialogBoxInfo[53].sView] != 0))
			{
				if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + x * 18 + 65) && (msY <= sY + x * 18 + 79))
				{
					ZeroMemory(cTemp2, sizeof(cTemp2));
					wsprintf(cTemp2, m_pOnlineUsersList[i + m_stDialogBoxInfo[53].sView]->m_cGuildName, cStr2, cStr3);
					m_Misc.ReplaceString(cTemp2, '_', ' ');

					if (memcmp(cTemp2, m_cGuildName, sizeof(m_cGuildName)) != 0)
					{
						continue;
					}

					if (strcmp(m_cPlayerName, m_pOnlineUsersList[i + m_stDialogBoxInfo[53].sView]->m_cName) == 0)
					{
						AddEventList("You can't manipulate to yourself.", 10);
						break;
					}
					ZeroMemory(cCharSelection, sizeof(cCharSelection));
					strcpy(cCharSelection, m_pOnlineUsersList[i + m_stDialogBoxInfo[53].sView]->m_cName);
					m_stDialogBoxInfo[53].cMode = 42; // Manipulate Char
					
					bListSelected = true;
					PlaySound('E', 14, 5);
					break;
				}
				x++;
			}
		}

		//Back
		if ((msX > sX + 220) && (msX < sX + 240) && (msY > sY + 315) && (msY < sY + 330)) {
			m_stDialogBoxInfo[53].cMode = 40; // Return
			PlaySound('E', 14, 5);
		}
		break;
	case 42:
		iNext += 5;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59))
		{
			ZeroMemory(G_cTxt, sizeof(G_cTxt));
			wsprintf(G_cTxt, "/addcaptain %s", cCharSelection);
			bSendCommand(MSGID_COMMAND_CHATMSG, 0, 0, 0, 0, 0, G_cTxt);

		}
		iNext += 2;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59))
		{
			ZeroMemory(G_cTxt, sizeof(G_cTxt));
			wsprintf(G_cTxt, "/addsummoner %s", cCharSelection);
			bSendCommand(MSGID_COMMAND_CHATMSG, 0, 0, 0, 0, 0, G_cTxt);

		}
		iNext += 2;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59))
		{
			ZeroMemory(G_cTxt, sizeof(G_cTxt));
			wsprintf(G_cTxt, "/addrecluiter %s", cCharSelection);
			bSendCommand(MSGID_COMMAND_CHATMSG, 0, 0, 0, 0, 0, G_cTxt);

		}
		iNext += 2;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59))
		{
			ZeroMemory(G_cTxt, sizeof(G_cTxt));
			wsprintf(G_cTxt, "/delrank %s", cCharSelection);
			bSendCommand(MSGID_COMMAND_CHATMSG, 0, 0, 0, 0, 0, G_cTxt);

		}
		iNext += 2;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59))
		{
			ZeroMemory(G_cTxt, sizeof(G_cTxt));
			wsprintf(G_cTxt, "/ban %s", cCharSelection);
			bSendCommand(MSGID_COMMAND_CHATMSG, 0, 0, 0, 0, 0, G_cTxt);

		}

		//Back
		if ((msX > sX + 220) && (msX < sX + 240) && (msY > sY + 315) && (msY < sY + 330)) {
			m_stDialogBoxInfo[53].cMode = 41; // Return
			PlaySound('E', 14, 5);
		}
		break;
	case 43:
		x = 0;
		for (i = 0; i < 17; i++)
		{
			if (((i + m_stDialogBoxInfo[53].sView) < DEF_MAXCLIENTS) && (m_pOnlineUsersList[i + m_stDialogBoxInfo[53].sView] != 0))
			{
				if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + x * 18 + 65) && (msY <= sY + x * 18 + 79))
				{
					ZeroMemory(cTemp2, sizeof(cTemp2));
					wsprintf(cTemp2, m_pOnlineUsersList[i + m_stDialogBoxInfo[53].sView]->m_cGuildName, cStr2, cStr3);
					m_Misc.ReplaceString(cTemp2, '_', ' ');

					if (memcmp(cTemp2, m_cGuildName, sizeof(m_cGuildName)) != 0)
					{
						continue;
					}

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "/to %s", m_pOnlineUsersList[i + m_stDialogBoxInfo[53].sView]->m_cName);
					bSendCommand(MSGID_COMMAND_CHATMSG, 0, 0, 0, 0, 0, cTemp);
					
					bListSelected = true;
					PlaySound('E', 14, 5);
					break;
				}
				x++;
			}
		}

		//Back
		if ((msX > sX + 220) && (msX < sX + 240) && (msY > sY + 315) && (msY < sY + 330)) {
			m_stDialogBoxInfo[53].cMode = 42; // Return
			PlaySound('E', 14, 5);
		}
		break;
	}
}

//New GM Panel by Magn0S ;D
void CGame::DrawDialogBox_GeneralPanel(short msX, short msY, short msZ, char cLB)
{
	short sX, sY, szX, sServerHour;
	char cTxt[120];
	int y;
	int nickheight = 16;
	short toX, toY, limitX, limitY;
	int iNext = 0;
	int iEntry = 0;

	int iGuildIndex;

	sX = m_stDialogBoxInfo[53].sX;
	sY = m_stDialogBoxInfo[53].sY;
	szX = m_stDialogBoxInfo[53].sSizeX;

	//if ((strcmp(m_cPlayerName, "Magn0S[GM]") != 0) || (strcmp(m_cPlayerName, "Centuu[GM]") != 0) || (strcmp(m_cPlayerName, "Nixu[GM]") != 0)) return;

	
	if (m_bUseOldPanels) {
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME6, sX, sY, 2);
	}
	else {
		//short limitX, limitY;
		limitX = sX + m_stDialogBoxInfo[53].sSizeX;
		limitY = sY + m_stDialogBoxInfo[53].sSizeY;

		m_DDraw.DrawShadowBox(sX, sY, limitX, limitY, 0, true);
		if (!m_bDialogTrans) m_DDraw.DrawShadowBox(sX, sY, limitX, limitY, 0, true);

		m_DDraw.DrawShadowBox(sX, sY, limitX, sY + 25, 0, true);
		if (!m_bDialogTrans) m_DDraw.DrawShadowBox(sX, sY, limitX, sY + 25, 0, true);
		//PutString_SprFont2(sX + 100, sY + 5, "Magics", 240, 240, 240);
	}

	int  i, iTemp;
	char cTemp[255], cTemp2[255], cStr2[255], cStr3[255];

	float dTmp1, dTmp2, dTmp3;
	int  iTotalLines, iPointerLoc;
	float d1, d2, d3;
	int iR, iG, iB;

	int x;
	
	switch (m_stDialogBoxInfo[53].cMode) {
	case 0:
		//PutString_SprFont3(sX + 80, sY + 10, "Player Panel", 2, 150, 0);

		PutString_SprFont2(sX + 80, sY + 5, "Player Panel", 240, 240, 240);
		//wsprintf(G_cTxt, "Hi %s", m_cPlayerName);
		//PutAlignedString(sX + 30, sX + szX - 30, sY + 30, G_cTxt, 255, 255, 255);
		//PutAlignedString(sX + 32, sX + szX - 30, sY + 30, G_cTxt, 255, 255, 255);

		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59))
		{
			//PutString2(sX + 25, sY + iNext * 17 + 45, "Top Ek List", 255, 255, 255);
			PutAlignedString(sX + 30, sX + szX - 30, sY + iNext * 17 + 45, "Server Top Ek", 255, 255, 255);
		}
		else PutAlignedString(sX + 30, sX + szX - 30, sY + iNext * 17 + 45, "Server Top Ek", 19, 104, 169);
		//PutString2(sX + 25, sY + iNext * 17 + 45, "Top Ek List", 255, 255, 100);

		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59))
		{
			//PutString2(sX + 25, sY + iNext * 17 + 45, "Top Ek List", 255, 255, 255);
			PutAlignedString(sX + 30, sX + szX - 30, sY + iNext * 17 + 45, "Event Schedule", 255, 255, 255);
		}
		else PutAlignedString(sX + 30, sX + szX - 30, sY + iNext * 17 + 45, "Event Schedule", 19, 104, 169);
		//PutString2(sX + 25, sY + iNext * 17 + 45, "Top Ek List", 255, 255, 100);

		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59))
		{
			PutAlignedString(sX + 30, sX + szX - 30, sY + iNext * 17 + 45, "Game Options", 255, 255, 255);
		}
		else PutAlignedString(sX + 30, sX + szX - 30, sY + iNext * 17 + 45, "Game Options", 19, 104, 169);

		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59))
		{
			PutAlignedString(sX + 30, sX + szX - 30, sY + iNext * 17 + 45, "Online User List", 255, 255, 255);
		}
		else PutAlignedString(sX + 30, sX + szX - 30, sY + iNext * 17 + 45, "Online User List", 19, 104, 169);

		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59))
		{
			PutAlignedString(sX + 30, sX + szX - 30, sY + iNext * 17 + 45, "Friend's List", 255, 255, 255);
		}
		else PutAlignedString(sX + 30, sX + szX - 30, sY + iNext * 17 + 45, "Friend's List", 19, 104, 169);

		if (m_iGuildRank != -1) 
		{
			iNext += 1;
			if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59))
			{
				PutAlignedString(sX + 30, sX + szX - 30, sY + iNext * 17 + 45, "Guild Menu", 255, 255, 255);
			}
			else PutAlignedString(sX + 30, sX + szX - 30, sY + iNext * 17 + 45, "Guild Menu", 19, 104, 169);
		}
		break;

	case 1:
		//PutString_SprFont3(sX + 70, sY + 10, "Server Top Ek", 2, 150, 0);
		//PutString_SprFont(sX + 70, sY + 35, "Server Top Ek", 7, 0, 0);

		PutString_SprFont2(sX + 70, sY + 5, "Server Top Ek", 240, 240, 240);

		PutString2(sX + 65, sY + 30, "Name", 0, 255, 0);
		PutString2(sX + 165, sY + 30, "EKs", 0, 255, 0);
		for (i = 1; i <= 15; i++)
		{
			//if (strcmp(iHBTOPn[i], "HBTop-") == 0) continue;
			string st1 = iHBTOPn[i];
			if (st1.find("HBTop-") != string::npos) break;
			wsprintf(G_cTxt, "%s", iHBTOPn[i]);
			if (i == 1)
			{
				PutString2(sX + 55, sY + 30 + 17 * i, G_cTxt, 255, 255, 0); // 170,170,170
			}
			else
			{
				PutString2(sX + 55, sY + 30 + 17 * i, G_cTxt, 255, 255, 255);
			}

			wsprintf(G_cTxt, "%d", iHBTOPk[i]);
			if (i == 1)
			{
				//PutString2(sX + 155, sY + 30 + 17 * i, G_cTxt, 255, 255, 0); // 170,170,170
				PutAlignedString2(sX + 155, sX + 195, sY + 30 + 17 * i, G_cTxt, 255, 255, 0);
			}
			else
			{
				//PutString2(sX + 155, sY + 30 + 17 * i, G_cTxt, 255, 255, 255);
				PutAlignedString2(sX + 155, sX + 195, sY + 30 + 17 * i, G_cTxt, 255, 255, 255);
			}
			wsprintf(G_cTxt, "%d)", i);
			PutString2(sX + 26, sY + 30 + 17 * i, G_cTxt, 0, 255, 0);
		}

		if ((msX > sX + 210) && (msX < sX + 240) && (msY > sY + 315) && (msY < sY + 330))
			PutString_SprFont3(sX + 210, sY + 315, "Back", 200, 250, 2);
		else PutString_SprFont3(sX + 210, sY + 315, "Back", 200, 250, 200);

		break;

	case 2: // Game Administration
		//PutString_SprFont3(sX + 70, sY + 10, "Event Schedule", 200, 250, 2);
		//PutString_SprFont(sX + 70, sY + 35, "Event Schedule", 7, 0, 0);
		PutString_SprFont2(sX + 70, sY + 5, "Event Schedule", 240, 240, 240);
		SYSTEMTIME SysTime;
		GetLocalTime(&SysTime);
		ZeroMemory(G_cTxt, sizeof(G_cTxt));
		//sServerHour = 0;
		sServerHour = SysTime.wHour - sServerTime;
		if (bServerTime)
			wsprintf(G_cTxt, "Server time: %d:%d:%d", sServerHour, SysTime.wMinute, SysTime.wSecond);
		else wsprintf(G_cTxt, "Local time: %d:%d:%d", SysTime.wHour, SysTime.wMinute, SysTime.wSecond);
		PutString(sX + 130, sY + 35, G_cTxt, RGB(255, 255, 255));
		PutString(sX + 130, sY + 35, G_cTxt, RGB(255, 255, 255));

		iNext += 1;
		PutString2(sX + 35, sY + iNext * 15 + 35, "Event name:", 255, 255, 0);
		PutString2(sX + 190, sY + iNext * 15 + 35, "Status:", 255, 255, 0);
		//Green color: RGB(0, 255, 0)); 
		//------------------------------------------------------------------------------------------------------------
		iNext += 1;
		/*if ((msX >= sX + 20) && (msX <= sX + 180) && (msY >= sY + iNext * 17 + 35) && (msY <= sY + iNext * 17 + 49)) {
			PutString2(sX + 25, sY + iNext * 17 + 35, "Death Match Game", 255, 255, 255);
		} else {*/
		PutString2(sX + 25, sY + iNext * 17 + 35, "Death Match Game", 19, 104, 169);
		//}
		if (!bDeathmatch) {
			PutString2(sX + 200, sY + iNext * 17 + 35, "OFF", 255, 0, 0);
		}
		else { PutString2(sX + 200, sY + iNext * 17 + 35, "ON", 0, 255, 0); }
		//------------------------------------------------------------------------------------------------------------
		iNext += 1;
		/*if ((msX >= sX + 20) && (msX <= sX + 180) && (msY >= sY + iNext * 17 + 35) && (msY <= sY + iNext * 17 + 49)) {
			PutString2(sX + 25, sY + iNext * 17 + 35, "Crusade", 255, 255, 255);
		} else { */
		PutString2(sX + 25, sY + iNext * 17 + 35, "Crusade", 19, 104, 169);
		//}
		if (!m_bIsCrusadeMode) {
			PutString2(sX + 200, sY + iNext * 17 + 35, "OFF", 255, 0, 0);
		}
		else { PutString2(sX + 200, sY + iNext * 17 + 35, "ON", 0, 255, 0); }

		//------------------------------------------------------------------------------------------------------------
		iNext += 1;
		/*if ((msX >= sX + 20) && (msX <= sX + 180) && (msY >= sY + iNext * 17 + 35) && (msY <= sY + iNext * 17 + 49)) {
			PutString2(sX + 25, sY + iNext * 17 + 35, "Apocalypse", 255, 255, 255);
		} else {*/
		PutString2(sX + 25, sY + iNext * 17 + 35, "Apocalypse", 19, 104, 169);
		//}
		if (!m_bApocalypse) {
			PutString2(sX + 200, sY + iNext * 17 + 35, "OFF", 255, 0, 0);
		}
		else { PutString2(sX + 200, sY + iNext * 17 + 35, "ON", 0, 255, 0); }
		//------------------------------------------------------------------------------------------------------------
		iNext += 1;
		/*if ((msX >= sX + 20) && (msX <= sX + 180) && (msY >= sY + iNext * 17 + 35) && (msY <= sY + iNext * 17 + 49)) {
			PutString2(sX + 25, sY + iNext * 17 + 35, "Heldenian", 255, 255, 255);
		} else {*/
		PutString2(sX + 25, sY + iNext * 17 + 35, "Heldenian", 19, 104, 169);
		//}
		if (!m_bIsHeldenian) {
			PutString2(sX + 200, sY + iNext * 17 + 35, "OFF", 255, 0, 0);
		}
		else { PutString2(sX + 200, sY + iNext * 17 + 35, "ON", 0, 255, 0); }
		//------------------------------------------------------------------------------------------------------------
		iNext += 1;
		/*if ((msX >= sX + 20) && (msX <= sX + 180) && (msY >= sY + iNext * 17 + 35) && (msY <= sY + iNext * 17 + 49)) {
			PutString2(sX + 25, sY + iNext * 17 + 35, "Beholder Event", 255, 255, 255);
		} else {*/
		PutString2(sX + 25, sY + iNext * 17 + 35, "Beholder Event", 19, 104, 169);
		//}
		if (!_revelation) {
			PutString2(sX + 200, sY + iNext * 17 + 35, "OFF", 255, 0, 0);
		}
		else { PutString2(sX + 200, sY + iNext * 17 + 35, "ON", 0, 255, 0); }
		//------------------------------------------------------------------------------------------------------------
		iNext += 1;
		/*if ((msX >= sX + 20) && (msX <= sX + 180) && (msY >= sY + iNext * 17 + 35) && (msY <= sY + iNext * 17 + 49)) {
			PutString2(sX + 25, sY + iNext * 17 + 35, "Shinning", 255, 255, 255);
		} else {*/
		PutString2(sX + 25, sY + iNext * 17 + 35, "Shinning", 19, 104, 169);
		//}
		if (!bShinning) {
			PutString2(sX + 200, sY + iNext * 17 + 35, "OFF", 255, 0, 0);
		}
		else { PutString2(sX + 200, sY + iNext * 17 + 35, "ON", 0, 255, 0); }
		//------------------------------------------------------------------------------------------------------------
		iNext += 1;
		/*if ((msX >= sX + 20) && (msX <= sX + 180) && (msY >= sY + iNext * 17 + 35) && (msY <= sY + iNext * 17 + 49)) {
			PutString2(sX + 25, sY + iNext * 17 + 35, "Candy Fury", 255, 255, 255);
		} else {*/
		PutString2(sX + 25, sY + iNext * 17 + 35, "Candy Fury", 19, 104, 169);
		//}
		if (!_candy_boost) {
			PutString2(sX + 200, sY + iNext * 17 + 35, "OFF", 255, 0, 0);
		}
		else { PutString2(sX + 200, sY + iNext * 17 + 35, "ON", 0, 255, 0); }
		//------------------------------------------------------------------------------------------------------------
		iNext += 1;
		/*if ((msX >= sX + 20) && (msX <= sX + 180) && (msY >= sY + iNext * 17 + 35) && (msY <= sY + iNext * 17 + 49)) {
			PutString2(sX + 25, sY + iNext * 17 + 35, "Capture The Flag", 255, 255, 255);
		} else {*/
		PutString2(sX + 25, sY + iNext * 17 + 35, "Capture The Flag", 19, 104, 169);
		//}
		if (!_drop_inhib) {
			PutString2(sX + 200, sY + iNext * 17 + 35, "OFF", 255, 0, 0);
		}
		else { PutString2(sX + 200, sY + iNext * 17 + 35, "ON", 0, 255, 0); }
		//------------------------------------------------------------------------------------------------------------
		iNext += 1;
		/*if ((msX >= sX + 20) && (msX <= sX + 180) && (msY >= sY + iNext * 17 + 35) && (msY <= sY + iNext * 17 + 49)) {
			PutString2(sX + 25, sY + iNext * 17 + 35, "Bag Protection", 255, 255, 255);
		} else {*/
		PutString2(sX + 25, sY + iNext * 17 + 35, "Bag Protection", 19, 104, 169);
		//}
		if (!_drop_inhib) {
			PutString2(sX + 200, sY + iNext * 17 + 35, "OFF", 255, 0, 0);
		}
		else { PutString2(sX + 200, sY + iNext * 17 + 35, "ON", 0, 255, 0); }
		//------------------------------------------------------------------------------------------------------------
		iNext += 1;
		/*if ((msX >= sX + 20) && (msX <= sX + 180) && (msY >= sY + iNext * 17 + 35) && (msY <= sY + iNext * 17 + 49)) {
			PutString2(sX + 25, sY + iNext * 17 + 35, "Team Arena", 255, 255, 255);
		} else {*/
		PutString2(sX + 25, sY + iNext * 17 + 35, "Team Arena", 19, 104, 169);
		//}
		if (!_team_arena) {
			PutString2(sX + 200, sY + iNext * 17 + 35, "OFF", 255, 0, 0);
		}
		else { PutString2(sX + 200, sY + iNext * 17 + 35, "ON", 0, 255, 0); }
		//------------------------------------------------------------------------------------------------------------
		iNext += 1;
		/*if ((msX >= sX + 20) && (msX <= sX + 180) && (msY >= sY + iNext * 17 + 35) && (msY <= sY + iNext * 17 + 49)) {
			PutString2(sX + 25, sY + iNext * 17 + 35, "Dungeon Attack", 255, 255, 255);
		} else {*/
		PutString2(sX + 25, sY + iNext * 17 + 35, "Dungeon Attack", 19, 104, 169);
		//}

		if (!_city_teleport) {
			PutString2(sX + 200, sY + iNext * 17 + 35, "OFF", 255, 0, 0);
		}
		else { PutString2(sX + 200, sY + iNext * 17 + 35, "ON", 0, 255, 0); }
		//------------------------------------------------------------------------------------------------------------


		iNext += 2;
		if ((msX >= sX + 20) && (msX <= sX + 240) && (msY >= sY + iNext * 17 + 35) && (msY <= sY + iNext * 17 + 49)) {
			//m_DDraw.DrawShadowBox(sX + 20, sY + iNext * 17 + 35, sX + 240, sY + iNext * 17 + 55, 1, true);
			if (bServerTime)
				PutAlignedString2(sX + 20, sX + 240, sY + iNext * 17 + 37, "Change to Local Time", 255, 255, 100);
			else PutAlignedString2(sX + 20, sX + 240, sY + iNext * 17 + 37, "Change to Server Time", 255, 255, 100);
		}
		else {
			//m_DDraw.DrawShadowBox(sX + 20, sY + iNext * 17 + 35, sX + 240, sY + iNext * 17 + 55, 2, true);
			if (bServerTime)
				PutAlignedString2(sX + 20, sX + 240, sY + iNext * 17 + 37, "Change to Local Time", 180, 188, 180);
			else PutAlignedString2(sX + 20, sX + 240, sY + iNext * 17 + 37, "Change to Server Time", 180, 188, 180);
		}

		if ((msX > sX + 210) && (msX < sX + 240) && (msY > sY + 315) && (msY < sY + 330))
			PutString_SprFont3(sX + 210, sY + 315, "Back", 200, 250, 2);
		else PutString_SprFont3(sX + 210, sY + 315, "Back", 200, 250, 200);

		break;
		
	case 3:
		PutString_SprFont2(sX + 125 - ((strlen("Game Options") * 7) / 2), sY + 5, "Game Options", 240, 240, 240);

		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59))
		{
			PutString2(sX + 25, sY + iNext * 17 + 45, "Show Party Info", 255, 255, 255);
		}
		else PutString2(sX + 25, sY + iNext * 17 + 45, "Show Party Info", 19, 104, 169);

		if (m_bShowParty) PutString2(sX + 200, sY + iNext * 17 + 45, "ON", 0, 255, 0);
		else PutString2(sX + 200, sY + iNext * 17 + 45, "OFF", 255, 0, 0);

		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59))
		{
			PutString2(sX + 25, sY + iNext * 17 + 45, "Show Events Info", 255, 255, 255);
		}
		else PutString2(sX + 25, sY + iNext * 17 + 45, "Show Events Info", 19, 104, 169);

		if (bShowEventInfo) PutString2(sX + 200, sY + iNext * 17 + 45, "ON", 0, 255, 0);
		else PutString2(sX + 200, sY + iNext * 17 + 45, "OFF", 255, 0, 0);

		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59))
		{
			PutString2(sX + 25, sY + iNext * 17 + 45, "Show Emblems", 255, 255, 255);
		}
		else PutString2(sX + 25, sY + iNext * 17 + 45, "Show Emblems", 19, 104, 169);

		if (m_bShowEmblems) PutString2(sX + 200, sY + iNext * 17 + 45, "ON", 0, 255, 0);
		else PutString2(sX + 200, sY + iNext * 17 + 45, "OFF", 255, 0, 0);

		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59))
		{
			PutString2(sX + 25, sY + iNext * 17 + 45, "Show Trees", 255, 255, 255);
		}
		else PutString2(sX + 25, sY + iNext * 17 + 45, "Show Trees", 19, 104, 169);

		if (m_bShowTrees) PutString2(sX + 200, sY + iNext * 17 + 45, "ON", 0, 255, 0);
		else PutString2(sX + 200, sY + iNext * 17 + 45, "OFF", 255, 0, 0);

		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59))
		{
			PutString2(sX + 25, sY + iNext * 17 + 45, "Show Big Items", 255, 255, 255);
		}
		else PutString2(sX + 25, sY + iNext * 17 + 45, "Show Big Items", 19, 104, 169);

		if (bChangeBigItems) PutString2(sX + 200, sY + iNext * 17 + 45, "ON", 0, 255, 0);
		else PutString2(sX + 200, sY + iNext * 17 + 45, "OFF", 255, 0, 0);

		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59))
		{
			PutString2(sX + 25, sY + iNext * 17 + 45, "Show NPC", 255, 255, 255);
		}
		else PutString2(sX + 25, sY + iNext * 17 + 45, "Show NPC", 19, 104, 169);

		if (m_bNpcMap) PutString2(sX + 200, sY + iNext * 17 + 45, "ON", 0, 255, 0);
		else PutString2(sX + 200, sY + iNext * 17 + 45, "OFF", 255, 0, 0);

		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59))
		{
			PutString2(sX + 25, sY + iNext * 17 + 45, "Show Quest Helper", 255, 255, 255);
		}
		else PutString2(sX + 25, sY + iNext * 17 + 45, "Show Quest Helper", 19, 104, 169);

		if (m_bQuestHelper) PutString2(sX + 200, sY + iNext * 17 + 45, "ON", 0, 255, 0);
		else PutString2(sX + 200, sY + iNext * 17 + 45, "OFF", 255, 0, 0);

		

		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59))
		{
			PutString2(sX + 25, sY + iNext * 17 + 45, "Use Old-Style Panels", 255, 255, 255);
		}
		else PutString2(sX + 25, sY + iNext * 17 + 45, "Use Old-Style Panels", 19, 104, 169);

		if (m_bUseOldPanels) PutString2(sX + 200, sY + iNext * 17 + 45, "ON", 0, 255, 0);
		else PutString2(sX + 200, sY + iNext * 17 + 45, "OFF", 255, 0, 0);

		if ((msX > sX + 210) && (msX < sX + 240) && (msY > sY + 315) && (msY < sY + 330))
			PutString_SprFont3(sX + 210, sY + 315, "Back", 200, 250, 2);
		else PutString_SprFont3(sX + 210, sY + 315, "Back", 200, 250, 200);

		break;

	case 40:
		PutString_SprFont2(sX + 125 - ((strlen("Guild Menu") * 7) / 2), sY + 5, "Guild Menu", 240, 240, 240);
		
		wsprintf(G_cTxt, "%s", m_cGuildName);
		PutAlignedString(sX + 30, sX + szX - 30, sY + 30+5, G_cTxt, 255, 255, 255);

		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59))
		{
			PutString2(sX + 25, sY + iNext * 17 + 45, "Change Member Rank", 255, 255, 255);
		}
		else PutString2(sX + 25, sY + iNext * 17 + 45, "Change Member Rank", 19, 104, 169);

		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59))
		{
			PutString2(sX + 25, sY + iNext * 17 + 45, "Summon Guild", 255, 255, 255);
		}
		else PutString2(sX + 25, sY + iNext * 17 + 45, "Summon Guild", 19, 104, 169);

		iNext += 1;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59))
		{
			PutString2(sX + 25, sY + iNext * 17 + 45, "Guild Member List", 255, 255, 255);
		}
		else PutString2(sX + 25, sY + iNext * 17 + 45, "Guild Member List", 19, 104, 169);

		if ((msX > sX + 210) && (msX < sX + 240) && (msY > sY + 315) && (msY < sY + 330))
			PutString_SprFont3(sX + 210, sY + 315, "Back", 200, 250, 2);
		else PutString_SprFont3(sX + 210, sY + 315, "Back", 200, 250, 200);

		break;
	case 41:
		PutString_SprFont3(sX + 65, sY + 10, "Guild Member List", 2, 150, 0);

		PutAlignedString(sX + 30, sX + szX - 30, sY + 30, "Select a member to change rank or ban.", 255, 255, 255);
		
		iTotalLines = 0;
		for (i = 0; i < DEF_MAXMENUITEMS; i++)
			if (m_pOnlineUsersList[i] != 0) iTotalLines++;
		if (iTotalLines > 13) {
			d1 = (float)m_stDialogBoxInfo[53].sView;
			d2 = (float)(iTotalLines - 13);
			d3 = (274.0f * d1) / d2;
			iPointerLoc = (int)(d3);
			if (m_bUseOldPanels) {
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME6, sX, sY, 3);
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME6, sX + 242, sY + iPointerLoc + 35, 7);
			} 
			else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME2, sX + 242, sY + iPointerLoc + 35, 7);
		}
		else iPointerLoc = 0;

		if (cLB != 0 && iTotalLines > 13)
		{
			if ((iGetTopDialogBoxIndex() == 53))
			{
				if ((msX >= sX + 235) && (msX <= sX + 260) && (msY >= sY + 10) && (msY <= sY + 330))
				{
					d1 = (float)(msY - (sY + 35));
					d2 = (float)(iTotalLines - 13);
					d3 = (d1 * d2) / 274.0f;
					m_stDialogBoxInfo[53].sView = (int)(d3 + 0.5f);
				}
			}
		}
		else m_stDialogBoxInfo[53].bIsScrollSelected = false;
		if (iGetTopDialogBoxIndex() == 53 && msZ != 0)
		{
			m_stDialogBoxInfo[53].sView = m_stDialogBoxInfo[53].sView - msZ / 60;
			m_DInput.m_sZ = 0;
		}
		if (iTotalLines > 13 && m_stDialogBoxInfo[53].sView > iTotalLines - 13) m_stDialogBoxInfo[53].sView = iTotalLines - 13;
		if (m_stDialogBoxInfo[53].sView < 0 || iTotalLines < 13) m_stDialogBoxInfo[53].sView = 0;

		x = 0;
		for (i = 0; i < 13; i++) {
			if (((i + m_stDialogBoxInfo[53].sView) < DEF_MAXMENUITEMS) && (m_pOnlineUsersList[i + m_stDialogBoxInfo[53].sView] != 0))
			{
				ZeroMemory(cTemp, sizeof(cTemp));
				wsprintf(cTemp, m_pOnlineUsersList[i + m_stDialogBoxInfo[53].sView]->m_cName, cStr2, cStr3);

				ZeroMemory(cTemp2, sizeof(cTemp2));
				wsprintf(cTemp2, m_pOnlineUsersList[i + m_stDialogBoxInfo[53].sView]->m_cGuildName, cStr2, cStr3);
				m_Misc.ReplaceString(cTemp2, '_', ' ');

				if (memcmp(cTemp2, m_cGuildName, sizeof(m_cGuildName)) == 0)
				{
					iR = 255;	iG = 255; iB = 255;
				}
				else
				{
					//iR = 255;	iG = 255; iB = 255;
					continue;
				}

				if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + x * 18 + 65) && (msY <= sY + x * 18 + 79))
				{
					PutString2(sX + 25, sY + x * 18 + 65, cTemp, iR, iG, iB);
				}
				else
				{
					PutString2(sX + 25, sY + x * 18 + 65, cTemp, iR / 2, iG / 2, iB / 2);
				}
				x++;
			}
		}

		x = 0;
		for (i = 0; i < 13; i++) {
			if (((i + m_stDialogBoxInfo[53].sView) < DEF_MAXMENUITEMS) && (m_pOnlineUsersList[i + m_stDialogBoxInfo[53].sView] != 0))
			{
				ZeroMemory(cTemp, sizeof(cTemp));
				wsprintf(cTemp, m_pOnlineUsersList[i + m_stDialogBoxInfo[53].sView]->m_cGuildName, cStr2, cStr3);
				m_Misc.ReplaceString(cTemp, '_', ' ');

				ZeroMemory(cTemp2, sizeof(cTemp2));
				wsprintf(cTemp2, m_pOnlineUsersList[i + m_stDialogBoxInfo[53].sView]->m_cName, cStr2, cStr3);

				// display blank space rather than NONE if char has no guild
				if (memcmp(cTemp, "NONE", 4) == 0)
				{
					memcpy(cTemp, "    ", 4);
				}

				if (memcmp(cTemp, m_cGuildName, sizeof(m_cGuildName)) == 0)
				{
					iR = 255;	iG = 255; iB = 255;
				}
				else
				{
					//iR = 255;	iG = 255; iB = 255;
					continue;
				}

				if (FindGuildName(m_pOnlineUsersList[i + m_stDialogBoxInfo[53].sView]->m_cName, &iGuildIndex) == true)
				{
					if (m_stGuildName[iGuildIndex].cGuildName[0] != 0)
					{
						if (strcmp(m_stGuildName[iGuildIndex].cGuildName, "NONE") != 0)
						{
							if (m_stGuildName[iGuildIndex].iGuildRank == 0)
							{
								wsprintf(G_cTxt, "Guildmaster");//

							}
							else if (m_stGuildName[iGuildIndex].iGuildRank == 12)
							{
								wsprintf(G_cTxt, "Guildsman");//"

							}
							else if (m_stGuildName[iGuildIndex].iGuildRank == 1)
							{
								wsprintf(G_cTxt, "Recluiter");//" Guildsman)"

							}
							else if (m_stGuildName[iGuildIndex].iGuildRank == 2)
							{
								wsprintf(G_cTxt, "Summoner");//" Guildsman)"

							}
							else if (m_stGuildName[iGuildIndex].iGuildRank == 3)
							{
								wsprintf(G_cTxt, "Captain");//" Guildsman)"

							}
							m_stGuildName[iGuildIndex].dwRefTime = m_dwCurTime;
						}
						else
						{
							m_stGuildName[iGuildIndex].dwRefTime = 0;
						}
					}
				}
				else bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_REQGUILDRANK, 0, 0, iGuildIndex, 0, m_pOnlineUsersList[i + m_stDialogBoxInfo[53].sView]->m_cName);

				if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + x * 18 + 65) && (msY <= sY + x * 18 + 79))
				{
					PutString2(sX + 125, sY + x * 18 + 65, G_cTxt, iR, iG, iB);
				}
				else
				{
					PutString2(sX + 125, sY + x * 18 + 65, G_cTxt, iR / 2, iG / 2, iB / 2);
				}
				x++;
			}
		}

		if ((msX > sX + 210) && (msX < sX + 240) && (msY > sY + 315) && (msY < sY + 330))
			PutString_SprFont3(sX + 210, sY + 315, "Back", 200, 250, 2);
		else PutString_SprFont3(sX + 210, sY + 315, "Back", 200, 250, 200);

		break;

	case 42:
		//PutString_SprFont3(sX + 50, sY + 10, "Administration Panel", 2, 150, 0);
		PutString_SprFont3(sX + 125 - ((strlen("Guild Menu") * 7) / 2), sY + 10, "Guild Menu", 2, 150, 0);

		iNext += 2;
		PutString2(sX + 25, sY + iNext * 17 + 45, "Guild Member:", 255, 255, 255);
		PutString2(sX + 105, sY + iNext * 17 + 45, cCharSelection, 255, 255, 255);
		PutString(sX + 100, sY + iNext * 17 + 47, "_____________", RGB(255, 255, 255));

		if (iGetTopDialogBoxIndex() == 53 && msZ != 0)
		{
			m_stDialogBoxInfo[53].sV3 = m_stDialogBoxInfo[53].sV3 + msZ / 60;
			m_DInput.m_sZ = 0;
		}

		iNext += 3;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59))
			PutAlignedString(sX + 10, sX + szX - 10, sY + iNext * 17 + 45, "Set as Captain", 255, 255, 255);
		else PutAlignedString(sX + 10, sX + szX - 10, sY + iNext * 17 + 45, "Set as Captain", 255, 255, 100);
		iNext += 2;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59))
			PutAlignedString(sX + 10, sX + szX - 10, sY + iNext * 17 + 45, "Set as Summoner", 255, 255, 255);
		else PutAlignedString(sX + 10, sX + szX - 10, sY + iNext * 17 + 45, "Set as Summoner", 255, 255, 100);
		iNext += 2;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59))
			PutAlignedString(sX + 10, sX + szX - 10, sY + iNext * 17 + 45, "Set as Recluiter", 255, 255, 255);
		else PutAlignedString(sX + 10, sX + szX - 10, sY + iNext * 17 + 45, "Set as Recluiter", 255, 255, 100);
		iNext += 2;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59))
			PutAlignedString(sX + 10, sX + szX - 10, sY + iNext * 17 + 45, "Remove Rank", 255, 255, 255);
		else PutAlignedString(sX + 10, sX + szX - 10, sY + iNext * 17 + 45, "Remove Rank", 255, 255, 100);
		iNext += 2;
		if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + iNext * 17 + 45) && (msY <= sY + iNext * 17 + 59))
			PutAlignedString(sX + 10, sX + szX - 10, sY + iNext * 17 + 45, "Ban Member", 255, 255, 255);
		else PutAlignedString(sX + 10, sX + szX - 10, sY + iNext * 17 + 45, "Ban Member", 255, 255, 100);

		if ((msX > sX + 210) && (msX < sX + 240) && (msY > sY + 315) && (msY < sY + 330))
			PutString_SprFont3(sX + 210, sY + 315, "Back", 200, 250, 2);
		else PutString_SprFont3(sX + 210, sY + 315, "Back", 200, 250, 200);

		break;
	case 43:
		PutString_SprFont3(sX + 65, sY + 10, "Guild Member List", 2, 150, 0);

		PutString2(sX + 25, sY + 50, "Name", 0, 255, 0);
		PutString2(sX + 135, sY + 50, "Rank", 0, 255, 0);
		
		iTotalLines = 0;
		for (i = 0; i < DEF_MAXMENUITEMS; i++)
			if (m_pOnlineUsersList[i] != 0) iTotalLines++;
		if (iTotalLines > 13) {
			d1 = (float)m_stDialogBoxInfo[53].sView;
			d2 = (float)(iTotalLines - 13);
			d3 = (274.0f * d1) / d2;
			iPointerLoc = (int)(d3);
			if (m_bUseOldPanels) {
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME6, sX, sY, 3);
				DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME6, sX + 242, sY + iPointerLoc + 35, 7);
			}
			else DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME2, sX + 242, sY + iPointerLoc + 35, 7);
		}
		else iPointerLoc = 0;

		if (cLB != 0 && iTotalLines > 13)
		{
			if ((iGetTopDialogBoxIndex() == 53))
			{
				if ((msX >= sX + 235) && (msX <= sX + 260) && (msY >= sY + 10) && (msY <= sY + 330))
				{
					d1 = (float)(msY - (sY + 35));
					d2 = (float)(iTotalLines - 13);
					d3 = (d1 * d2) / 274.0f;
					m_stDialogBoxInfo[53].sView = (int)(d3 + 0.5f);
				}
			}
		}
		else m_stDialogBoxInfo[53].bIsScrollSelected = false;
		if (iGetTopDialogBoxIndex() == 53 && msZ != 0)
		{
			m_stDialogBoxInfo[53].sView = m_stDialogBoxInfo[53].sView - msZ / 60;
			m_DInput.m_sZ = 0;
		}
		if (iTotalLines > 13 && m_stDialogBoxInfo[53].sView > iTotalLines - 13) m_stDialogBoxInfo[53].sView = iTotalLines - 13;
		if (m_stDialogBoxInfo[53].sView < 0 || iTotalLines < 13) m_stDialogBoxInfo[53].sView = 0;

		x = 0;
		for (i = 0; i < 13; i++) {
			if (((i + m_stDialogBoxInfo[53].sView) < DEF_MAXMENUITEMS) && (m_pOnlineUsersList[i + m_stDialogBoxInfo[53].sView] != 0))
			{
				ZeroMemory(cTemp, sizeof(cTemp));
				wsprintf(cTemp, m_pOnlineUsersList[i + m_stDialogBoxInfo[53].sView]->m_cName, cStr2, cStr3);

				ZeroMemory(cTemp2, sizeof(cTemp2));
				wsprintf(cTemp2, m_pOnlineUsersList[i + m_stDialogBoxInfo[53].sView]->m_cGuildName, cStr2, cStr3);
				m_Misc.ReplaceString(cTemp2, '_', ' ');

				if (memcmp(cTemp2, m_cGuildName, sizeof(m_cGuildName)) == 0)
				{
					iR = 255;	iG = 255; iB = 255;
				}
				else
				{
					//iR = 255;	iG = 255; iB = 255;
					continue;
				}

				if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + x * 18 + 65) && (msY <= sY + x * 18 + 79))
				{
					PutString2(sX + 25, sY + x * 18 + 65, cTemp, iR, iG, iB);
				}
				else
				{
					PutString2(sX + 25, sY + x * 18 + 65, cTemp, iR / 2, iG / 2, iB / 2);
				}
				x++;
			}
		}

		x = 0;
		for (i = 0; i < 13; i++) {
			if (((i + m_stDialogBoxInfo[53].sView) < DEF_MAXMENUITEMS) && (m_pOnlineUsersList[i + m_stDialogBoxInfo[53].sView] != 0))
			{
				ZeroMemory(cTemp, sizeof(cTemp));
				wsprintf(cTemp, m_pOnlineUsersList[i + m_stDialogBoxInfo[53].sView]->m_cGuildName, cStr2, cStr3);
				m_Misc.ReplaceString(cTemp, '_', ' ');

				ZeroMemory(cTemp2, sizeof(cTemp2));
				wsprintf(cTemp2, m_pOnlineUsersList[i + m_stDialogBoxInfo[53].sView]->m_cName, cStr2, cStr3);

				// display blank space rather than NONE if char has no guild
				if (memcmp(cTemp, "NONE", 4) == 0)
				{
					memcpy(cTemp, "    ", 4);
				}

				if (memcmp(cTemp, m_cGuildName, sizeof(m_cGuildName)) == 0)
				{
					iR = 255;	iG = 255; iB = 255;
				}
				else
				{
					//iR = 255;	iG = 255; iB = 255;
					continue;
				}

				if (FindGuildName(m_pOnlineUsersList[i + m_stDialogBoxInfo[53].sView]->m_cName, &iGuildIndex) == true)
				{
					if (m_stGuildName[iGuildIndex].cGuildName[0] != 0)
					{
						if (strcmp(m_stGuildName[iGuildIndex].cGuildName, "NONE") != 0)
						{
							if (m_stGuildName[iGuildIndex].iGuildRank == 0)
							{
								wsprintf(G_cTxt, "Guildmaster");//

							}
							else if (m_stGuildName[iGuildIndex].iGuildRank == 12)
							{
								wsprintf(G_cTxt, "Guildsman");//"

							}
							else if (m_stGuildName[iGuildIndex].iGuildRank == 1)
							{
								wsprintf(G_cTxt, "Recluiter");//" Guildsman)"

							}
							else if (m_stGuildName[iGuildIndex].iGuildRank == 2)
							{
								wsprintf(G_cTxt, "Summoner");//" Guildsman)"

							}
							else if (m_stGuildName[iGuildIndex].iGuildRank == 3)
							{
								wsprintf(G_cTxt, "Captain");//" Guildsman)"

							}
							m_stGuildName[iGuildIndex].dwRefTime = m_dwCurTime;
						}
						else
						{
							m_stGuildName[iGuildIndex].dwRefTime = 0;
						}
					}
				}
				else bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_REQGUILDRANK, 0, 0, iGuildIndex, 0, m_pOnlineUsersList[i + m_stDialogBoxInfo[53].sView]->m_cName);

				if ((msX >= sX + 20) && (msX <= sX + 220) && (msY >= sY + x * 18 + 65) && (msY <= sY + x * 18 + 79))
				{
					PutString2(sX + 125, sY + x * 18 + 65, G_cTxt, iR, iG, iB);
				}
				else
				{
					PutString2(sX + 125, sY + x * 18 + 65, G_cTxt, iR / 2, iG / 2, iB / 2);
				}
				x++;
			}
		}

		if ((msX > sX + 210) && (msX < sX + 240) && (msY > sY + 315) && (msY < sY + 330))
			PutString_SprFont3(sX + 210, sY + 315, "Back", 200, 250, 2);
		else PutString_SprFont3(sX + 210, sY + 315, "Back", 200, 250, 200);

		break;
	}
}

void CGame::DlgBoxClick_SetTrap(short msX, short msY)
{
	short sX, sY, szx, szy;
	sX = m_stDialogBoxInfo[45].sX;
	sY = m_stDialogBoxInfo[45].sY;
	szx = m_stDialogBoxInfo[45].sSizeX;
	szy = m_stDialogBoxInfo[45].sSizeY;

	if (m_pMagicCfgList[m_iCastingMagicType] != 0) {
		// Magic Slot
		if ((msX >= sX + 20) && (msX <= sX + 20 + szx - 20) && (msY >= sY + 60 + 15 * 1) && (msY <= sY + 60 + 15 * 1 + 15))
		{
			if (m_stDialogBoxInfo[45].sV1 == m_iCastingMagicType + 1) m_stDialogBoxInfo[45].sV1 = 0;
			else m_stDialogBoxInfo[45].sV1 = m_iCastingMagicType + 1;
		}
	}

	// Create
	if ((msX >= sX + (szx / 2 - DEF_BTNSZX) / 2) && (msX <= sX + (szx / 2 - DEF_BTNSZX) / 2 + DEF_BTNSZX) && (msY >= sY + szy - 20 - DEF_BTNSZY) && (msY <= sY + szy - 20 - DEF_BTNSZY + DEF_BTNSZY))
	{
		bSendCommand(MSGID_REQUEST_SETTRAP, DEF_MSGTYPE_CONFIRM, 0, 0, 0, 0, 0);
		DisableDialogBox(45);
		PlaySound('E', 14, 5);
	}

	// Cancel
	if ((msX >= sX + (szx / 2 - DEF_BTNSZX) / 2 + szx / 2) && (msX <= sX + (szx / 2 - DEF_BTNSZX) / 2 + szx / 2 + DEF_BTNSZX) && (msY >= sY + szy - 20 - DEF_BTNSZY) && (msY <= sY + szy - 20 - DEF_BTNSZY + DEF_BTNSZY))
	{
		DisableDialogBox(45);
		PlaySound('E', 14, 5);
	}
}

void CGame::DrawDialogBox_SetTrap(short msX, short msY)
{
	short sX, sY, szx, szy;
	sX = m_stDialogBoxInfo[45].sX;
	sY = m_stDialogBoxInfo[45].sY;
	szx = m_stDialogBoxInfo[45].sSizeX;
	szy = m_stDialogBoxInfo[45].sSizeY;

	//DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_GAME4, sX, sY, 2);

	short limitX, limitY;
	limitX = sX + m_stDialogBoxInfo[45].sSizeX;
	limitY = sY + m_stDialogBoxInfo[45].sSizeY;

	m_DDraw.DrawShadowBox(sX, sY, limitX, limitY, 0, true);
	m_DDraw.DrawShadowBox(sX, sY, limitX, limitY, 0, true);

	PutAlignedString(sX, sX + szx, sY + 30, "Setup Your Trap Configuration", 0, 255, 0);

	PutString(sX + 15 + 40, sY + 60 + 15, "Magic Slot:", RGB(0, 255, 0));
	if (m_stDialogBoxInfo[45].sV1 == NULL) strcpy(G_cTxt, "(Not set)");
	else sprintf(G_cTxt, "%s", m_pMagicCfgList[m_stDialogBoxInfo[45].sV1 - 1]->m_cName);
	PutString(sX + 90 + 40, sY + 60 + 15, G_cTxt, RGB(255, 255, 0));

	if ((msX >= sX + (szx / 2 - DEF_BTNSZX) / 2) && (msX <= sX + (szx / 2 - DEF_BTNSZX) / 2 + DEF_BTNSZX) && (msY >= sY + szy - 20 - DEF_BTNSZY) && (msY <= sY + szy - 20 - DEF_BTNSZY + DEF_BTNSZY))
	{
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + (szx / 2 - DEF_BTNSZX) / 2, sY + szy - 20 - DEF_BTNSZY, 0);
		PutAlignedString2(sX + (szx / 2 - DEF_BTNSZX) / 2, (sX + (szx / 2 - DEF_BTNSZX) / 2) + 90, (sY + szy - 20 - DEF_BTNSZY) + 2, "Create", 255, 255, 100);
	}
	else {
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + (szx / 2 - DEF_BTNSZX) / 2, sY + szy - 20 - DEF_BTNSZY, 1);
		PutAlignedString2(sX + (szx / 2 - DEF_BTNSZX) / 2, (sX + (szx / 2 - DEF_BTNSZX) / 2) + 90, (sY + szy - 20 - DEF_BTNSZY) + 2, "Create", 180, 188, 180);
	}

	if ((msX >= sX + (szx / 2 - DEF_BTNSZX) / 2 + szx / 2) && (msX <= sX + (szx / 2 - DEF_BTNSZX) / 2 + szx / 2 + DEF_BTNSZX) && (msY >= sY + szy - 20 - DEF_BTNSZY) && (msY <= sY + szy - 20 - DEF_BTNSZY + DEF_BTNSZY)) {
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + (szx / 2 - DEF_BTNSZX) / 2 + szx / 2, sY + szy - 20 - DEF_BTNSZY, 0);
		PutAlignedString2(sX + (szx / 2 - DEF_BTNSZX) / 2 + szx / 2, (sX + (szx / 2 - DEF_BTNSZX) / 2 + szx / 2) + 90, (sY + szy - 20 - DEF_BTNSZY) + 2, "Cancel", 255, 255, 100);
	}
	else {
		DrawNewDialogBox(DEF_SPRID_INTERFACE_ND_BUTTONS, sX + (szx / 2 - DEF_BTNSZX) / 2 + szx / 2, sY + szy - 20 - DEF_BTNSZY, 1);
		PutAlignedString2(sX + (szx / 2 - DEF_BTNSZX) / 2 + szx / 2, (sX + (szx / 2 - DEF_BTNSZX) / 2 + szx / 2) + 90, (sY + szy - 20 - DEF_BTNSZY) + 2, "Cancel", 180, 188, 180);
	}
}

// kazin
void CGame::NotifyEvents(char* pData)
{
	char* cp;
	bool* bp;

	bool candy = _candy_boost;
	bool rev = _revelation;
	bool tp = _city_teleport;
	bool drop = _drop_inhib;

	cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);
	bp = (bool*)cp;
	_candy_boost = *bp;
	cp ++;

	bp = (bool*)cp;
	_revelation = *bp;
	cp ++;

	bp = (bool*)cp;
	_city_teleport = *bp;
	cp ++;

	bp = (bool*)cp;
	_drop_inhib = *bp;
	cp ++;

	bp = (bool*)cp;
	_team_arena = *bp;
	cp++;

	bp = (bool*)cp;
	bShinning = *bp;
	cp++;

	bp = (bool*)cp;
	m_bHappyHour = *bp;
	cp++;

	bp = (bool*)cp;
	m_bFuryHour = *bp;
	cp++;

	// ON
	if (_candy_boost && !candy)
	{
		SetTopMsg("Candy Fury started", 10);
	}

	if (_revelation && !rev)
	{
		SetTopMsg("Beholder Event started", 10);
	}

	if (_city_teleport && !tp)
	{
		SetTopMsg("Dungeon Attack Time started", 10);
	}

	if (_drop_inhib && !drop)
	{
		SetTopMsg("Bag Protection started", 10);
	}

	// OFF
	if (!_candy_boost && candy)
	{
		SetTopMsg("Candy Fury finished", 10);
	}

	if (!_revelation && rev)
	{
		SetTopMsg("Beholder Event finished", 10);
	}

	if (!_city_teleport && tp)
	{
		SetTopMsg("Dungeon Attack Time finished", 10);
	}

	if (!_drop_inhib && drop)
	{
		SetTopMsg("Bag Protection event finished", 10);
	}
}

void CGame::minimapblue_update(char* cp)
{
	int* ip = (int*)cp;
	int h = *ip;
	cp += 4;

	short* sp = (short*)cp;
	short x = *sp;
	cp += 2;

	sp = (short*)cp;
	short y = *sp;
	cp += 2;

	MinimapBlue::Unit u;
	u.id = h;
	u.x = x;
	u.y = y;
	auto t = timeGetTime();
	u.time = t;
	m_minimapblue.Remove(h);
	m_minimapblue.list.push_back(u);
}

void CGame::minimapblue_clear(char* cp)
{
	int h;

	int* ip = (int*)cp;
	h = *ip;
	cp += 4;

	m_minimapblue.Remove(h);
}

void CGame::minimapred_update(char* cp)
{
	int* ip = (int*)cp;
	int h = *ip;
	cp += 4;

	short* sp = (short*)cp;
	short x = *sp;
	cp += 2;

	sp = (short*)cp;
	short y = *sp;
	cp += 2;

	MinimapRed::Unit u;
	u.id = h;
	u.x = x;
	u.y = y;
	auto t = timeGetTime();
	u.time = t;
	m_minimapred.Remove(h);


	m_minimapred.list.push_back(u);
}

void CGame::minimapred_clear(char* cp)
{
	int h;

	int* ip = (int*)cp;
	h = *ip;
	cp += 4;

	m_minimapred.Remove(h);
}

void CGame::minimapgreen_update(char* cp)
{
	int* ip = (int*)cp;
	int h = *ip;
	cp += 4;

	short* sp = (short*)cp;
	short x = *sp;
	cp += 2;

	sp = (short*)cp;
	short y = *sp;
	cp += 2;

	MinimapGreen::Unit u;
	u.id = h;
	u.x = x;
	u.y = y;
	auto t = timeGetTime();
	u.time = t;
	m_minimapgreen.Remove(h);
	m_minimapgreen.list.push_back(u);
}

void CGame::minimapgreen_clear(char* cp)
{
	int h;

	int* ip = (int*)cp;
	h = *ip;
	cp += 4;

	m_minimapgreen.Remove(h);
}

void CGame::minimapyellow_update(char* cp)
{
	int* ip = (int*)cp;
	int h = *ip;
	cp += 4;

	short* sp = (short*)cp;
	short x = *sp;
	cp += 2;

	sp = (short*)cp;
	short y = *sp;
	cp += 2;

	MinimapYellow::Unit u;
	u.id = h;
	u.x = x;
	u.y = y;
	auto t = timeGetTime();
	u.time = t;
	m_minimapyellow.Remove(h);
	m_minimapyellow.list.push_back(u);
}

void CGame::minimapyellow_clear(char* cp)
{
	int h;

	int* ip = (int*)cp;
	h = *ip;
	cp += 4;

	m_minimapyellow.Remove(h);
}

void CGame::minimaporange_update(char* cp)
{
	int* ip = (int*)cp;
	int h = *ip;
	cp += 4;

	short* sp = (short*)cp;
	short x = *sp;
	cp += 2;

	sp = (short*)cp;
	short y = *sp;
	cp += 2;

	MinimapOrange::Unit u;
	u.id = h;
	u.x = x;
	u.y = y;
	auto t = timeGetTime();
	u.time = t;
	m_minimaporange.Remove(h);
	m_minimaporange.list.push_back(u);
}

void CGame::minimaporange_clear(char* cp)
{
	int h;

	int* ip = (int*)cp;
	h = *ip;
	cp += 4;

	m_minimaporange.Remove(h);
}

void CGame::MinimapRed::Clear()
{
	list.clear();
}

void CGame::MinimapBlue::Clear()
{
	list.clear();
}

void CGame::MinimapGreen::Clear()
{
	list.clear();
}

void CGame::MinimapYellow::Clear()
{
	list.clear();
}

void CGame::MinimapOrange::Clear()
{
	list.clear();
}

#define stdremove(list, func) list.erase(remove_if(list.begin(), list.end(), func), list.end());

void CGame::MinimapBlue::Remove(int handle) {
	stdremove(list, [&](MinimapBlue::Unit& p)
		{
			return p.id == handle;
		});
}

void CGame::MinimapRed::Remove(int handle) {
	stdremove(list, [&](MinimapRed::Unit& p)
		{
			return p.id == handle;
		});
}

void CGame::MinimapGreen::Remove(int handle) {
	stdremove(list, [&](MinimapGreen::Unit& p)
		{
			return p.id == handle;
		});
}

void CGame::MinimapYellow::Remove(int handle) {
	stdremove(list, [&](MinimapYellow::Unit& p)
		{
			return p.id == handle;
		});
}

void CGame::MinimapOrange::Remove(int handle) {
	stdremove(list, [&](MinimapOrange::Unit& p)
		{
			return p.id == handle;
		});
}

// Magn0S
void CGame::NotifyMapRestrictions(char* pData)
{
	char* cp;
	bool* bp;

	bool attck = m_bAttackMode;
	bool teleport = bMapTP;
	bool party = bMapParty;
	bool illusion = bMapIllusion;
	bool actives = bMapActivate;
	bool invi = bMapInvi;
	bool amp = bMapAMP;
	bool regens = bMapRegens;
	bool hideenemy = bMapHideEnemy;
	bool dmgbonus = bMapBonusDmg;
	bool equip = bMapEquip;

	bool notify = false;

	cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);
	bp = (bool*)cp;
	notify = *bp;
	cp ++;

	bp = (bool*)cp;
	m_bAttackMode = *bp;
	cp ++;

	bp = (bool*)cp;
	bMapTP = *bp;
	cp++;

	bp = (bool*)cp;
	bMapParty = *bp;
	cp++;

	bp = (bool*)cp;
	bMapIllusion = *bp;
	cp ++;

	bp = (bool*)cp;
	bMapActivate = *bp;
	cp ++;

	bp = (bool*)cp;
	bMapInvi = *bp;
	cp++;

	bp = (bool*)cp;
	bMapAMP = *bp;
	cp++;

	bp = (bool*)cp;
	bMapRegens = *bp;
	cp++;

	bp = (bool*)cp;
	bMapHideEnemy = *bp;
	cp++;

	bp = (bool*)cp;
	bMapBonusDmg = *bp;
	cp++;

	bp = (bool*)cp;
	bMapEquip = *bp;
	cp++;

	if (notify) {
	// ON
		if (m_bAttackMode && !attck)
		{	AddEventList(NOTIFYMSG_GLOBAL_ATTACK_MODE3, 10);
		}
		if (bMapTP && !teleport)
		{	AddEventList("Teleport Actions in this map has been enabled!", 10);
		}
		if (bMapParty && !party)
		{	AddEventList("Party Mode in this map has been enabled!", 10);
		}
		if (bMapIllusion && !illusion)
		{	AddEventList("Illusion Mode in this map has been enabled!", 10);
		//	m_iIlusionOwnerH = _tmp_iStatus;
			//_SetIlusionEffect(_tmp_iStatus);
		}
		if (bMapActivate && !actives)
		{	AddEventList("Activations in this map has been enabled!", 10);
		}
		if (bMapInvi && !invi)
		{	AddEventList("Invisibility in this map has been enabled!", 10);
		}
		if (bMapAMP && !amp)
		{	AddEventList("Abs. Magic Protection (AMP) in this map has been enabled!", 10);
		}
		if (bMapRegens && !regens)
		{	AddEventList("Regenerate in this map has been enabled!", 10);
		}
		if (bMapHideEnemy && !hideenemy)
		{	AddEventList("Enemy Names in this map has been enabled!", 10);
		}
		if (bMapBonusDmg && !dmgbonus)
		{	AddEventList("Bonus Damage (50%) in this map has been enabled!", 10);
		}
		if (bMapEquip && !equip)
		{	AddEventList("Equip Items in this map has been enabled!", 10);
		}

	// OFF
		if (!m_bAttackMode && attck)
		{	AddEventList(NOTIFYMSG_GLOBAL_ATTACK_MODE2, 10);
		}
		if (!bMapTP && teleport)
		{	AddEventList("Teleport Actions in this map has been enabled!", 10);
		}
		if (!bMapParty && party)
		{	AddEventList("Party Mode in this map has been disabled!", 10);
		}
		if (!bMapIllusion && illusion)
		{	AddEventList("Illusion Mode in this map has been disabled!", 10);
			//m_iIlusionOwnerH = 0;
		}
		if (!bMapActivate && actives)
		{	AddEventList("Activations in this map has been disabled!", 10);
		}
		if (!bMapInvi && invi)
		{	AddEventList("Invisibility in this map has been disabled!", 10);
		}
		if (!bMapAMP && amp)
		{	AddEventList("Abs. Magic Protection (AMP) in this map has been disabled!", 10);
		}
		if (!bMapRegens && regens)
		{	AddEventList("Regenerate in this map has been disabled!", 10);
		}
		if (!bMapHideEnemy && hideenemy)
		{	AddEventList("Enemy Names in this map has been disabled!", 10);
		}
		if (!bMapBonusDmg && dmgbonus)
		{	AddEventList("Bonus Damage (50%) in this map has been disabled!", 10);
		}
		if (!bMapEquip && equip)
		{	AddEventList("Equip Items in this map has been disabled!", 10);
		}
	}
}

void CGame::NotifyMsg_EventInfo(char* pData)
{
	char* cp, cTotal, i;
	short* sp;

	cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);

	m_bIsCTFMode = *cp;
	cp++;

	m_cCFTEventCount[0] = *cp;
	cp++;

	m_cCFTEventCount[1] = *cp;
	cp++;
	
}

// if you log out or get DC, we run this to reset any timers etc. it also saves the game preferences
void CGame::ResetValues()
{
	if (m_bFinishInit)
		SaveGameConfigFile();

	m_iTotalDR = 0;
	m_iHitRatio = 0;
	m_iTotalMR = 0;
	m_iAddHP = 0;
	m_iAddMP = 0;
	m_iAddSP = 0;
	m_iAddPR = 0;
	m_iManaSaveRatio = 0;
	m_iAddAbsPD = 0;
	m_iAddAbsMD = 0;
	m_iAddAbsAir = 0;
	m_iAddAbsEarth = 0;
	m_iAddAbsFire = 0;
	m_iAddAbsWater = 0;
	m_iAddPhysicalDamage = 0;
	m_iAddMagicalDamage = 0;
	m_iArmorPA = 0;
	m_iLeggsPA = 0;
	m_iBerkPA = 0;
	m_iHelmPA = 0;
	m_iCapePA = 0;
	m_iShieldPA = 0;

	// VAMP - arena
	for (int i = 0; i < DEF_MAXCLIENTS; i++)
	{
		//m_stArenaPlayers[i].iTeam = 0;
		m_stArenaPlayers[i].iKills = 0;
		m_stArenaPlayers[i].iDeaths = 0;
		ZeroMemory(m_stArenaPlayers[i].cCharName, sizeof(m_stArenaPlayers[i].cCharName));
	}

	bReadGameConfigFile("CONTENTS\\GameConfig.cfg");
}

// Magn0S
void CGame::NotifyPlayerAttributes(char* pData)
{
	char* cp;
	int  * ip;

	m_iTotalDR = 0;
	m_iHitRatio = 0;
	m_iTotalMR = 0;
	m_iAddHP = 0;
	m_iAddMP = 0;
	m_iAddSP = 0;
	m_iAddPR = 0;
	m_iManaSaveRatio = 0;
	m_iAddAbsPD = 0;
	m_iAddAbsMD = 0;
	m_iAddAbsAir = 0;
	m_iAddAbsEarth = 0;
	m_iAddAbsFire = 0;
	m_iAddAbsWater = 0;
	m_iAddPhysicalDamage = 0;
	m_iAddMagicalDamage = 0;
	m_iArmorPA = 0;
	m_iLeggsPA = 0;
	m_iBerkPA = 0;
	m_iHelmPA = 0;
	m_iCapePA = 0;
	m_iShieldPA = 0;

	cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);

	ip = (int*)cp;
	m_iTotalDR = *ip;
	cp += 4;

	ip = (int*)cp;
	m_iHitRatio = *ip;
	cp += 4;

	ip = (int*)cp;
	m_iTotalMR = *ip;
	cp += 4;

	ip = (int*)cp;
	m_iAddHP = *ip;
	cp += 4;

	ip = (int*)cp;
	m_iAddMP = *ip;
	cp += 4;

	ip = (int*)cp;
	m_iManaSaveRatio = *ip;
	cp += 4;
	//-------------------------------------------
	ip = (int*)cp;
	m_iAddAbsPD = *ip;
	cp += 4;

	ip = (int*)cp;
	m_iAddAbsMD = *ip;
	cp += 4;

	ip = (int*)cp;
	m_iAddAbsAir = *ip;
	cp += 4;

	ip = (int*)cp;
	m_iAddAbsEarth = *ip;
	cp += 4;

	ip = (int*)cp;
	m_iAddAbsFire = *ip;
	cp += 4;

	ip = (int*)cp;
	m_iAddAbsWater = *ip;
	cp += 4;

	ip = (int*)cp;
	m_iAddAbsHoly = *ip;
	cp += 4;

	ip = (int*)cp;
	m_iAddAbsUnholy = *ip;
	cp += 4;
	//-------------------------------------------
	ip = (int*)cp;
	m_iAddPhysicalDamage = *ip;
	cp += 4;

	ip = (int*)cp;
	m_iAddMagicalDamage = *ip;
	cp += 4;
	//-------------------------------------------
	ip = (int*)cp;
	m_iArmorPA = *ip;
	cp += 4;

	ip = (int*)cp;
	m_iLeggsPA = *ip;
	cp += 4;

	ip = (int*)cp;
	m_iBerkPA = *ip;
	cp += 4;

	ip = (int*)cp;
	m_iHelmPA = *ip;
	cp += 4;

	ip = (int*)cp;
	m_iCapePA = *ip;
	cp += 4;

	ip = (int*)cp;
	m_iShieldPA = *ip;
	cp += 4;
	//-------------------------------------------
	ip = (int*)cp;
	m_iAddSP = *ip;
	cp += 4;

	ip = (int*)cp;
	m_iAddPR = *ip;
	cp += 4;
}

LONG CGame::GetRegKey(HKEY key, LPCTSTR subkey, LPTSTR retdata)
{
	HKEY hkey;
	LONG retval = RegOpenKeyEx(key, subkey, 0, KEY_QUERY_VALUE, &hkey);
	if (retval == ERROR_SUCCESS)
	{
		long datasize = MAX_PATH;
		TCHAR data[MAX_PATH];
		RegQueryValue(hkey, 0, data, &datasize);
		lstrcpy(retdata, data);
		RegCloseKey(hkey);
	}
	return retval;
}

void CGame::GoHomepage()
{
	LPCTSTR	url;
	switch (sWebID) {
	case 1: // Update List
		url = "http://www.helbreathghost.com/monsters.html";
		break;
	case 2: // Drop List
		url = "http://www.helbreathghost.com/monsters.html";
		break;
	case 3: // Weapons
		url = "http://www.helbreathghost.com/weapons.html";
		break;
	case 4: // Armors
		url = "http://www.helbreathghost.com/armors.html";
		break;
	case 5: // Ball Points
		url = "https://www.helbreathghost.com/helpdesk";
		break;
	case 6: // Maps Online
		url = "http://www.helbreathghost.com/maps.html";
		break;
	case 7: // Jemels
		url = "http://www.helbreathghost.com/jewels.html";
		break;
	case 8: // Rings
		url = "http://www.helbreathghost.com/rings.html";
		break;
	case 9: // Necks
		url = "http://www.helbreathghost.com/necks.html";
		break;
	case 10: // Magics
		url = "http://www.helbreathghost.com/magics.html";
		break;
	case 11: // Discord
		url = "https://discord.gg/mvF5f25";
		break;
	case 12: // The Crusade
		url = "http://www.helbreathghost.com/crusade.html";
		break;
	case 13: // Staff
		url = "http://www.helbreathghost.com/staff.html";
		break;

	default:
		url = "http://www.helbreath.fun";
		break;
	}

	int		showcmd = SW_SHOW;
	char	key[MAX_PATH + MAX_PATH];
	SendMessage(G_hWnd, WM_ACTIVATEAPP, 0, 0);
	// First try ShellExecute()
	HINSTANCE result = ShellExecute(0, "open", url, 0, 0, showcmd);

	// If it failed, get the .htm regkey and lookup the program
	if ((UINT)result <= HINSTANCE_ERROR)
	{
		if (GetRegKey(HKEY_CLASSES_ROOT, ".htm", key) == ERROR_SUCCESS)
		{
			lstrcat(key, "\\shell\\open\\command");

			if (GetRegKey(HKEY_CLASSES_ROOT, key, key) == ERROR_SUCCESS)
			{
				char* pos;
				pos = strstr(key, "\"%1\"");
				if (pos == 0)					// No quotes found
				{
					pos = strstr(key, "%1");			// Check for %1, without quotes
					if (pos == 0)				// No parameter at all...
						pos = key + lstrlen(key) - 1;
					else *pos = '\0';				// Remove the parameter
				}
				else    *pos = '\0';				// Remove the parameter
				lstrcat(pos, " ");
				lstrcat(pos, url);
				result = (HINSTANCE)WinExec(key, showcmd);
			}
		}
	}
}

int CGame::dice(int _throw, int range)
{
	int i, ret;

	if (range <= 0 || _throw <= 0) return 0;

	ret = 0;
	for (i = 1; i <= _throw; i++) {

		ret += (rand() % range) + 1;
	}

	return ret;
}