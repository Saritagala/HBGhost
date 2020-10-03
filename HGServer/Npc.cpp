// Npc.cpp: implementation of the CNpc class.
//
//////////////////////////////////////////////////////////////////////

#include "Npc.h"
#include "Game.h"

extern void PutLogList(char* cMsg);
extern char G_cTxt[512];
extern char	G_cData50000[50000];
extern void PutLogFileList(char* cStr);
extern void PutAdminLogFileList(char* cStr);
extern void PutItemLogFileList(char* cStr);
extern void PutLogEventFileList(char* cStr);
extern void PutHackLogFileList(char* cStr);
extern void PutPvPLogFileList(char* cStr);
extern FILE* pLogFile;
extern HWND	G_hWnd;

#pragma warning (disable : 4996 6011 6001 4244 4018 6385 6386 26451 6054 4267 6053 6031)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNpc::CNpc(char * pName5)
{
 int i;
	ZeroMemory(m_cName, sizeof(m_cName));
	memcpy(m_cName, pName5, 5);
	
	for (i = 0; i < DEF_MAXWAYPOINTS; i++)			  
		m_iWayPointIndex[i] = -1;
	
	for (i = 0; i < DEF_MAXMAGICEFFECTS; i++) 
		m_cMagicEffectStatus[i]	= 0;

	m_bIsSummoned       = FALSE;
	m_bIsPermAttackMode = FALSE;

	m_dwRegenTime = NULL;
	m_bIsKilled   = FALSE;

	m_sOriginalType      = NULL;
	m_iSummonControlMode = NULL;
	
	m_cAttribute = NULL;
	m_iAbsDamage = NULL;
	m_iStatus    = NULL;
	m_sAppr2     = NULL;

	m_iAttackRange    = 1;
	m_cSpecialAbility = 0;
	m_sAreaSize = 0;

	m_iExp = 0;

	m_cPoisonResistance = 110;
	m_iPoisonLevel = 0;

	m_iBuildCount = 0;
	m_iManaStock  = 0;
	m_bIsUnsummoned = FALSE;
	m_cCropType = 0;
	m_cCropSkill = 0;

	m_bIsMaster  = FALSE;
	m_iGuildGUID = NULL;
	m_iV1 = 0;

	m_iNpcItemType = 0;
	m_iNpcItemMax = 0;
	m_bIsBossMob = FALSE;
}

CNpc::~CNpc()
{

}

// 2002-09-06 #1
CNpcItem::CNpcItem()
{
    ZeroMemory(m_cName, sizeof(m_cName));
    m_sItemID = 0;          // 2002-09-09 #1
    m_sFirstProbability = 0;
    m_cFirstTargetValue = NULL;
    m_sSecondProbability = 0;
    m_cSecondTargetValue = NULL;
}

void CGame::TargetSearch(int iNpcH, short* pTarget, char* pTargetType)
{
	int ix, iy, iPKCount;
	short sX, sY, rX, rY, dX, dY;
	short sOwner, sTargetOwner, sDistance, sTempDistance;
	char  cOwnerType, cTargetType, cTargetSide;
	int   iInv;

	sTargetOwner = NULL;
	cTargetType = NULL;
	sDistance = 100;

	sX = m_pNpcList[iNpcH]->m_sX;
	sY = m_pNpcList[iNpcH]->m_sY;

	// ÀÎµ¦½º iNpcH¿¡ ÇØ´çÇÏ´Â NPC°¡ °ø°Ý´ë»óÀ» Ã£´Â´Ù. 
	rX = m_pNpcList[iNpcH]->m_sX - m_pNpcList[iNpcH]->m_cTargetSearchRange;
	rY = m_pNpcList[iNpcH]->m_sY - m_pNpcList[iNpcH]->m_cTargetSearchRange;

	for (ix = rX; ix < rX + m_pNpcList[iNpcH]->m_cTargetSearchRange * 2 + 1; ix++)
		for (iy = rY; iy < rY + m_pNpcList[iNpcH]->m_cTargetSearchRange * 2 + 1; iy++) {

			m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetOwner(&sOwner, &cOwnerType, ix, iy);
			if (sOwner != NULL) {
				// ¹«¾ð°¡ ¹ß°ßµÇ¾ú´Ù. 
				if ((sOwner == iNpcH) && (cOwnerType == DEF_OWNERTYPE_NPC)) break; // ÀÚ±â ÀÚ½ÅÀÌ¸é ¹«½Ã 

				// ¸ñÇ¥ ¼³Á¤
				// À§Ä¡¸¦ ±¸ÇÑ´Ù.
				iPKCount = 0;
				switch (cOwnerType) {
				case DEF_OWNERTYPE_PLAYER:
					if (m_pClientList[sOwner] == NULL) {
						// Àß¸øµÈ ÀÎµ¦½º¸¦ °¡Áø °ªÀÌ´Ù. Å¬¸®¾îÇÑ´Ù.	
						m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->ClearOwner(5, sOwner, DEF_OWNERTYPE_PLAYER, ix, iy);
					}
					else {
						if (m_pClientList[sOwner]->m_iAdminUserLevel > 0) goto SKIP_SEARCH;

						dX = m_pClientList[sOwner]->m_sX;
						dY = m_pClientList[sOwner]->m_sY;
						cTargetSide = m_pClientList[sOwner]->m_cSide;
						iPKCount = m_pClientList[sOwner]->m_iPKCount;
						iInv = m_pClientList[sOwner]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY];
					}
					break;

				case DEF_OWNERTYPE_NPC:
					if (m_pNpcList[sOwner] == NULL) {
						// Àß¸øµÈ ÀÎµ¦½º¸¦ °¡Áø °ªÀÌ´Ù. Å¬¸®¾îÇÑ´Ù.	
						m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->ClearOwner(6, sOwner, DEF_OWNERTYPE_NPC, ix, iy);
					}
					else {
						dX = m_pNpcList[sOwner]->m_sX;
						dY = m_pNpcList[sOwner]->m_sY;
						cTargetSide = m_pNpcList[sOwner]->m_cSide;
						iPKCount = 0;
						iInv = m_pNpcList[sOwner]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY];

						if (m_pNpcList[iNpcH]->m_sType == 21) {
							// ¸¸¾à °æºñ°¡ NPC¸¦ ¸ñÇ¥¹°À» Ã£´Â °ÍÀÌ¶ó¸é NPCÁÖÀ§¿¡ ÇÃ·¹ÀÌ¾î°¡ ¸¹ÀÌ ÀÖ´Ù¸é °ø°ÝÇÏÁö ¾Ê´Â´Ù. 
							if (_iCalcPlayerNum(m_pNpcList[sOwner]->m_cMapIndex, dX, dY, 2) != 0) {
								sOwner = NULL;
								cOwnerType = NULL;
							}
						}
					}
					break;
				}

				if (m_pNpcList[iNpcH]->m_cSide < 10) {
					// NPCÀÇ Side°¡ 10º¸´Ù ÀÛ´Ù´Â °ÍÀº NPC°¡ ÇÑ ¸¶À»¿¡ ¼Ò¼ÓµÇ¾ú´Ù´Â ¶æ.  Ex: Guard È¤Àº ¼ÒÈ¯¸÷  
					if (cTargetSide == 0) {
						// ¸ñÇ¥¹°ÀÌ Áß¸³ÀÌ´Ù. PK°¡ ¾Æ´Ï¶ó¸é °ø°ÝÇÏÁö ¾Ê´Â´Ù. 
						if (iPKCount == 0) goto SKIP_SEARCH;
					}
					else {
						// ¸ñÇ¥¹°ÀÌ ¸¶À» ¼Ò¼ÓÀÌ´Ù. Side°¡ ´Ù¸£´Ù¸é °ø°ÝÇÑ´Ù. v1.12
						if ((iPKCount == 0) && (cTargetSide == m_pNpcList[iNpcH]->m_cSide)) goto SKIP_SEARCH;
						// Áß¸³¸÷Àº ¸¶À» ¼Ò¼Ó ¸ñÇ¥¹°À» °ø°ÝÇÏÁö ¾Ê´Â´Ù. v1.12
						if (m_pNpcList[iNpcH]->m_cSide == 0) goto SKIP_SEARCH;
					}
				}
				else {
					// ÀÏ¹Ý ¸ó½ºÅÍµéÀÌ´Ù. ÀÚ±â¿Í Side°¡ °°À¸¸é ¹«½Ã.
					if ((cOwnerType == DEF_OWNERTYPE_NPC) && (cTargetSide == 0)) goto SKIP_SEARCH;
					if (cTargetSide == m_pNpcList[iNpcH]->m_cSide) goto SKIP_SEARCH;
				}

				// ¸¸¾à INVISIBILITY°¡ µÇ¾î ÀÖ°í Åõ¸í Å½Áö ´É·ÂÀÌ ¾ø´Ù¸é 
				if ((iInv != 0) && (m_pNpcList[iNpcH]->m_cSpecialAbility != 1)) goto SKIP_SEARCH;

				if (abs(sX - dX) >= abs(sY - dY))
					sTempDistance = abs(sX - dX);
				else sTempDistance = abs(sY - dY);

				if (sTempDistance < sDistance) {
					sDistance = sTempDistance;
					sTargetOwner = sOwner;
					cTargetType = cOwnerType;
				}
			SKIP_SEARCH:;
			}
		}

	*pTarget = sTargetOwner;
	*pTargetType = cTargetType;
}

void CGame::RemoveFromTarget(short sTargetH, char cTargetType, int iCode, int iAttackerH, char cAttackerType)
// Used into deleteclient(), ClientKilled(), deleteNpc(), npcKilled(), invisibility, Teleport
{
	int i;
	//renew crusade summons duration	
	// Somebody from guild invied => renew summon duration !
	//  "   "     "     "  killed/teleported/deco 
	if ((m_bIsCrusadeMode == TRUE) && (cTargetType == DEF_OWNERTYPE_PLAYER))
	{
		DWORD dwTime = timeGetTime();
		for (i = 0; i < DEF_MAXNPCS; i++)
			if (m_pNpcList[i] != NULL)
			{
				if ((m_pNpcList[i]->m_iGuildGUID != NULL)
					&& (m_pClientList[sTargetH]->m_iGuildGUID == m_pNpcList[i]->m_iGuildGUID))
				{
					if (m_pNpcList[i]->m_cActionLimit < 2)
					{
						m_pNpcList[i]->m_bIsSummoned = TRUE;
						m_pNpcList[i]->m_dwSummonedTime = dwTime;
					}
				}
			}
	}

	// If master was killed or deco set follower mode
	if ((cTargetType == DEF_OWNERTYPE_PLAYER) && (iCode == NULL))// not DEF_MAGICTYPE_INVISIBILITY neither DEF_MAGICTYPE_TELEPORT
	{
		for (i = 0; i < DEF_MAXNPCS; i++)
			if (m_pNpcList[i] != NULL)
			{
				if ((m_pNpcList[i]->m_bIsSummoned == TRUE)
					&& (m_pNpcList[i]->m_bIsKilled == FALSE)
					&& (m_pNpcList[i]->m_cFollowOwnerType == DEF_OWNERTYPE_PLAYER)
					&& (m_pNpcList[i]->m_iFollowOwnerIndex == sTargetH))
				{	// Client suposed to be disconnected and not dead.
					if ((m_pClientList[sTargetH] != NULL)
						&& (m_pClientList[sTargetH]->m_bIsKilled == FALSE))
					{	// All Summons will live only 1 minute in ths case
						// other summons will live for standard time				
						m_pNpcList[i]->m_cFollowOwnerType = DEF_OWNERTYPE_PLAYER_WAITING;
						m_pNpcList[i]->m_iFollowOwnerIndex = NULL;
						m_pNpcList[i]->m_dwSummonWaitTime = timeGetTime();
						m_pNpcList[i]->m_cMoveType = DEF_MOVETYPE_RANDOM;
					}
					// Client is dead
					else if ((m_bIsHeldenianMode == TRUE)
						&& (m_pMapList[m_pNpcList[i]->m_cMapIndex] != NULL)
						&& (m_pMapList[m_pNpcList[i]->m_cMapIndex]->m_bIsHeldenianMap == 1))
					{
						switch (m_cHeldenianSummonMode) {
						case 0: // Mode 0, default, Kill the summon
							NpcKilledHandler(NULL, NULL, i, 0);
							break;
						case 1: // Mode 1: Summon will wait for his own master	
							m_pNpcList[i]->m_cFollowOwnerType = DEF_OWNERTYPE_PLAYER_WAITING;
							m_pNpcList[i]->m_iFollowOwnerIndex = NULL;
							m_pNpcList[i]->m_dwSummonWaitTime = timeGetTime();
							m_pNpcList[i]->m_cMoveType = DEF_MOVETYPE_RANDOM;
							// Summon tries to avenge his master
							if ((iAttackerH != NULL) && (cAttackerType != NULL))
							{
								m_pNpcList[i]->m_iSummonControlMode = 0;
								m_pNpcList[i]->m_cBehavior = DEF_BEHAVIOR_ATTACK;
								m_pNpcList[i]->m_sBehaviorTurnCount = 0;
								m_pNpcList[i]->m_iTargetIndex = iAttackerH;
								m_pNpcList[i]->m_cTargetType = cAttackerType;
								m_pNpcList[i]->m_bIsPermAttackMode = TRUE;
							}
							break;
						case 2: // Mode 3: Summon can be taken over by anybody else
							m_pNpcList[i]->m_cFollowOwnerType = DEF_OWNERTYPE_PLAYER_FREE;
							m_pNpcList[i]->m_iFollowOwnerIndex = NULL;
							ZeroMemory(m_pNpcList[i]->m_cFollowOwnerCharName, sizeof(m_pNpcList[i]->m_cFollowOwnerCharName));
							// Summon tries to avenge his master
							if ((iAttackerH != NULL) && (cAttackerType != NULL))
							{
								m_pNpcList[i]->m_iSummonControlMode = 0;
								m_pNpcList[i]->m_cBehavior = DEF_BEHAVIOR_ATTACK;
								m_pNpcList[i]->m_sBehaviorTurnCount = 0;
								m_pNpcList[i]->m_iTargetIndex = iAttackerH;
								m_pNpcList[i]->m_cTargetType = cAttackerType;
								m_pNpcList[i]->m_bIsPermAttackMode = TRUE;
							}
							break;
						}
					}
					else //Dead client, not Heldenian
					{
						m_pNpcList[i]->m_cFollowOwnerType = 0;	// No regain control possible
						m_pNpcList[i]->m_iFollowOwnerIndex = NULL;
						m_pNpcList[i]->m_cMoveType = DEF_MOVETYPE_RANDOM;
						m_pNpcList[i]->m_cBehavior = DEF_BEHAVIOR_MOVE;
						ZeroMemory(m_pNpcList[i]->m_cFollowOwnerCharName, sizeof(m_pNpcList[i]->m_cFollowOwnerCharName));
						// Summon tries to avenge his master
						if ((iAttackerH != NULL) && (cAttackerType != NULL))
						{
							m_pNpcList[i]->m_iSummonControlMode = 0;
							m_pNpcList[i]->m_cBehavior = DEF_BEHAVIOR_ATTACK;
							m_pNpcList[i]->m_sBehaviorTurnCount = 0;
							m_pNpcList[i]->m_iTargetIndex = iAttackerH;
							m_pNpcList[i]->m_cTargetType = cAttackerType;
							m_pNpcList[i]->m_bIsPermAttackMode = TRUE;
						}
					}
				}
				// Tamed mobs
				else if ((m_pNpcList[i]->m_dwTamingTime != 0) // For tamed mobs
					&& (m_pNpcList[i]->m_bIsSummoned == FALSE)
					&& (m_pNpcList[i]->m_bIsKilled == FALSE)
					&& (m_pNpcList[i]->m_cFollowOwnerType == DEF_OWNERTYPE_PLAYER)
					&& (m_pNpcList[i]->m_iFollowOwnerIndex == sTargetH))
				{	// Client suposed to be disconnected and not dead.
					if ((m_pClientList[sTargetH] != NULL)
						&& (m_pClientList[sTargetH]->m_bIsKilled == FALSE))
					{	// All Summons will live only 1 minute in ths case
						// other summons will live for standard time				
						m_pNpcList[i]->m_cFollowOwnerType = DEF_OWNERTYPE_PLAYER_WAITING;
						m_pNpcList[i]->m_iFollowOwnerIndex = NULL;
						m_pNpcList[i]->m_dwSummonWaitTime = timeGetTime();
						m_pNpcList[i]->m_cMoveType = DEF_MOVETYPE_RANDOM;
					}
					else// Client is dead
					{
						m_pNpcList[i]->m_cFollowOwnerType = 0;	// No regain control possible
						m_pNpcList[i]->m_iFollowOwnerIndex = NULL;
						m_pNpcList[i]->m_cMoveType = DEF_MOVETYPE_RANDOM;
						m_pNpcList[i]->m_cBehavior = DEF_BEHAVIOR_MOVE;
						ZeroMemory(m_pNpcList[i]->m_cFollowOwnerCharName, sizeof(m_pNpcList[i]->m_cFollowOwnerCharName));
						// Summon tries to avenge his master
						if ((iAttackerH != NULL) && (cAttackerType != NULL))
						{
							m_pNpcList[i]->m_iSummonControlMode = 0;
							m_pNpcList[i]->m_cBehavior = DEF_BEHAVIOR_ATTACK;
							m_pNpcList[i]->m_sBehaviorTurnCount = 0;
							m_pNpcList[i]->m_iTargetIndex = iAttackerH;
							m_pNpcList[i]->m_cTargetType = cAttackerType;
							m_pNpcList[i]->m_bIsPermAttackMode = TRUE;
						}
					}
				}
			}
	}

	// Stop attack the disapeared/dead/teleported/deco target
	for (i = 0; i < DEF_MAXNPCS; i++)
		if (m_pNpcList[i] != NULL)
		{
			if ((m_pNpcList[i]->m_iTargetIndex == sTargetH)
				&& (m_pNpcList[i]->m_cTargetType == cTargetType))
			{
				switch (iCode) {
				case DEF_MAGICTYPE_INVISIBILITY:
					if (m_pNpcList[i]->m_cSpecialAbility != 1)
					{
						m_pNpcList[i]->m_cBehavior = DEF_BEHAVIOR_MOVE;
						m_pNpcList[i]->m_iTargetIndex = NULL;
						m_pNpcList[i]->m_cTargetType = NULL;
					}
					break;
				case DEF_MAGICTYPE_TELEPORT:
				default:
					m_pNpcList[i]->m_cBehavior = DEF_BEHAVIOR_MOVE;
					m_pNpcList[i]->m_iTargetIndex = NULL;
					m_pNpcList[i]->m_cTargetType = NULL;
					break;
				}
			}
		}
}

void CGame::NpcKilledHandler(short sAttackerH, char cAttackerType, int iNpcH, short sDamage)
{
	short  sAttackerWeapon;
	int* ip, i, iQuestIndex, iConstructionPoint, iWarContribution, iMapIndex, j;
	double dTmp1, dTmp2, dTmp3;
	char* cp, cData[120], cQuestRemain, cTargetName[21];
	int iExp;

	if (m_pNpcList[iNpcH] == NULL) return;
	if (m_pNpcList[iNpcH]->m_bIsKilled == TRUE) return;

	m_pNpcList[iNpcH]->m_bIsKilled = TRUE;
	m_pNpcList[iNpcH]->m_iHP = 0;
	m_pNpcList[iNpcH]->m_iLastDamage = sDamage;

	m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_iTotalAliveObject--;

	RemoveFromTarget(iNpcH, DEF_OWNERTYPE_NPC);
	ReleaseFollowMode(iNpcH);

	m_pNpcList[iNpcH]->m_cFollowOwnerType = NULL; // Snoopy's add
	m_pNpcList[iNpcH]->m_iFollowOwnerIndex = NULL; // Snoopy's add

	m_pNpcList[iNpcH]->m_iTargetIndex = NULL;
	m_pNpcList[iNpcH]->m_cTargetType = NULL;
	if (cAttackerType == DEF_OWNERTYPE_PLAYER) {
		sAttackerWeapon = ((m_pClientList[sAttackerH]->m_sAppr2 & 0x0FF0) >> 4);
	}
	else sAttackerWeapon = 1;
	SendEventToNearClient_TypeA(iNpcH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTDYING, sDamage, sAttackerWeapon, NULL);
	m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->ClearOwner(10, iNpcH, DEF_OWNERTYPE_NPC, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY);
	m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->SetDeadOwner(iNpcH, DEF_OWNERTYPE_NPC, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY);
	m_pNpcList[iNpcH]->m_cBehavior = DEF_BEHAVIOR_DEAD;

	m_pNpcList[iNpcH]->m_sBehaviorTurnCount = 0;
	m_pNpcList[iNpcH]->m_dwDeadTime = timeGetTime();

	if (m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_cType == DEF_MAPTYPE_NOPENALTY_NOREWARD) return;

	NpcDeadItemGenerator(iNpcH, sAttackerH, cAttackerType);
	if ((m_pNpcList[iNpcH]->m_bIsSummoned != TRUE) && (cAttackerType == DEF_OWNERTYPE_PLAYER) &&
		(m_pClientList[sAttackerH] != NULL)) {
		iExp = (m_pNpcList[iNpcH]->m_iExp / 3);
		if (m_pNpcList[iNpcH]->m_iNoDieRemainExp > 0)
			iExp += m_pNpcList[iNpcH]->m_iNoDieRemainExp;

		if (m_pClientList[sAttackerH]->m_iAddExp != NULL) {
			dTmp1 = (double)m_pClientList[sAttackerH]->m_iAddExp;
			dTmp2 = (double)iExp;
			dTmp3 = (dTmp1 / 100.0f) * dTmp2;
			iExp += (int)dTmp3;
		}

		if (m_pNpcList[iNpcH]->m_sType == 99) {
			
			// Centu - end apocalypse
			DWORD dwTime = timeGetTime();
			dwTime += 1000 * 60 * 5; 
			bRegisterDelayEvent(DEF_DELAYEVENTTYPE_END_APOCALYPSE, 0, dwTime, 0
				, 0, iMapIndex, 0, 0, 0, 0, 0);

			/*for (i = 1; i < DEF_MAXCLIENTS; i++) {
				if (m_pClientList[i] != NULL) {
					SendNotifyMsg(sAttackerH, i, DEF_NOTIFY_ABADDONKILLED, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
				}
			}*/
		}

		if (m_bIsCrusadeMode == TRUE || m_bIsHeldenianMode == TRUE) {
			if (iExp > 10) iExp = iExp / 3;
		}

		// New 09/05/2004
		GetExp(sAttackerH, iExp, TRUE);

		for (j = 0; j < DEF_MAXQUEST; j++) { // Magn0S:: Multi Quest
			if (m_pClientList[sAttackerH]->m_iQuest[j] != NULL) {
				iQuestIndex = m_pClientList[sAttackerH]->m_iQuest[j];
				if (iQuestIndex != NULL) {
					if (m_pQuestConfigList[iQuestIndex] != NULL) {
						switch (m_pQuestConfigList[iQuestIndex]->m_iType) {
						case DEF_QUESTTYPE_MONSTERHUNT:
							//if ((m_pClientList[sAttackerH]->m_bQuestMatchFlag_Loc == TRUE) &&
							//	(m_pQuestConfigList[iQuestIndex]->m_iTargetType == m_pNpcList[iNpcH]->m_sType))
							//{
							ZeroMemory(cTargetName, sizeof(cTargetName));
							memcpy(cTargetName, m_pQuestConfigList[iQuestIndex]->m_cTargetName, 20);
							if ((memcmp(m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_cName, cTargetName, 10) == 0) && 
								(m_pQuestConfigList[iQuestIndex]->m_iTargetType == m_pNpcList[iNpcH]->m_sType)) {
								m_pClientList[sAttackerH]->m_iCurQuestCount[j]++;
								//cQuestRemain = (m_pQuestConfigList[m_pClientList[sAttackerH]->m_iQuest[j]]->m_iMaxCount - m_pClientList[sAttackerH]->m_iCurQuestCount[j]);
								//SendNotifyMsg(NULL, sAttackerH, DEF_NOTIFY_QUESTCOUNTER, j, cQuestRemain, NULL, NULL);
								SendNotifyMsg(NULL, sAttackerH, DEF_NOTIFY_QUESTCOUNTER, j, m_pClientList[sAttackerH]->m_iCurQuestCount[j], NULL, NULL);
								_bCheckIsQuestCompleted(sAttackerH, j);
							}
							break;
						}
					}
				}
			}
		}
	}

	//Magn0S:: Removed.
	/*if (cAttackerType == DEF_OWNERTYPE_PLAYER) {
		switch (m_pNpcList[iNpcH]->m_sType) {
		case 32:
			m_pClientList[sAttackerH]->m_iRating -= 5;
			// MORLA 2.4 - Actualizo la rep
			SendNotifyMsg(NULL, sAttackerH, DEF_NOTIFY_REPDGDEATHS, m_pClientList[sAttackerH]->m_iDGPoints, m_pClientList[sAttackerH]->m_iTotalDGKills, m_pClientList[sAttackerH]->m_iRating, NULL);
			break;

		case 33:
			break;
		}
	}*/

	if (m_pNpcList[iNpcH]->m_sType == 91) // Remove bloqued area
	{
		for (short sx1 = (m_pNpcList[iNpcH]->m_sX - 1); sx1 <= m_pNpcList[iNpcH]->m_sX + 1; sx1++)
			for (short sy1 = (m_pNpcList[iNpcH]->m_sY - 1); sy1 <= m_pNpcList[iNpcH]->m_sY + 1; sy1++)
			{
				m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->SetTempMoveAllowedFlag(sx1, sy1, TRUE);
			}
	}

	if (m_bIsCrusadeMode == TRUE) {
		// Crusade
		iConstructionPoint = 0;
		switch (m_pNpcList[iNpcH]->m_sType) {
		case 1:  iConstructionPoint = 50; iWarContribution = 100; break;
		case 2:  iConstructionPoint = 50; iWarContribution = 100; break;
		case 3:  iConstructionPoint = 50; iWarContribution = 100; break;
		case 4:  iConstructionPoint = 50; iWarContribution = 100; break;
		case 5:  iConstructionPoint = 50; iWarContribution = 100; break;
		case 6:  iConstructionPoint = 50; iWarContribution = 100; break;
		case 36: iConstructionPoint = 700; iWarContribution = 4000; break;
		case 37: iConstructionPoint = 700; iWarContribution = 4000; break;
		case 38: iConstructionPoint = 500; iWarContribution = 2000; break;
		case 39: iConstructionPoint = 500; iWarContribution = 2000; break;
		case 40: iConstructionPoint = 1500; iWarContribution = 5000; break;
		case 41: iConstructionPoint = 5000; iWarContribution = 10000; break;
		case 43: iConstructionPoint = 500; iWarContribution = 1000; break;
		case 44: iConstructionPoint = 1000; iWarContribution = 2000; break;
		case 45: iConstructionPoint = 1500; iWarContribution = 3000; break;
		case 46: iConstructionPoint = 1000; iWarContribution = 2000; break;
		case 47: iConstructionPoint = 1500; iWarContribution = 3000; break;
		case 64: m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->bRemoveCropsTotalSum(); break;

		}

		// Crusade
		if (iConstructionPoint != NULL) {
			switch (cAttackerType) {
			case DEF_OWNERTYPE_PLAYER:
				if (m_pClientList[sAttackerH]->m_cSide != m_pNpcList[iNpcH]->m_cSide) {
					m_pClientList[sAttackerH]->m_iConstructionPoint += iConstructionPoint;
					m_pClientList[sAttackerH]->m_iWarContribution += iWarContribution;
					//wsprintf(G_cTxt, "Enemy Npc Killed by player! Construction: +%d WarContribution: +%d", iConstructionPoint, iWarContribution);
					//PutLogList(G_cTxt);

					SendNotifyMsg(NULL, sAttackerH, DEF_NOTIFY_CONSTRUCTIONPOINT, m_pClientList[sAttackerH]->m_iConstructionPoint, m_pClientList[sAttackerH]->m_iWarContribution, NULL, NULL);
				}
				else {
					m_pClientList[sAttackerH]->m_iWarContribution -= (iWarContribution * 2);
					if (m_pClientList[sAttackerH]->m_iWarContribution < 0)
						m_pClientList[sAttackerH]->m_iWarContribution = 0;
					//wsprintf(G_cTxt, "Friendly Npc Killed by player! WarContribution: -%d", iWarContribution);
					//PutLogList(G_cTxt);

					SendNotifyMsg(NULL, sAttackerH, DEF_NOTIFY_CONSTRUCTIONPOINT, m_pClientList[sAttackerH]->m_iConstructionPoint, m_pClientList[sAttackerH]->m_iWarContribution, NULL, NULL);
				}
				break;

			case DEF_OWNERTYPE_NPC:
				if (m_pNpcList[sAttackerH]->m_iGuildGUID != NULL) {
					if (m_pNpcList[sAttackerH]->m_cSide != m_pNpcList[iNpcH]->m_cSide) {
						for (i = 1; i < DEF_MAXCLIENTS; i++)
							if ((m_pClientList[i] != NULL) && (m_pClientList[i]->m_iGuildGUID == m_pNpcList[sAttackerH]->m_iGuildGUID) &&
								(m_pClientList[i]->m_iCrusadeDuty == 3)) {

								m_pClientList[i]->m_iConstructionPoint += iConstructionPoint;
								//wsprintf(G_cTxt, "Enemy Npc Killed by Npc! Construct point +%d", iConstructionPoint);
								//PutLogList(G_cTxt);
								SendNotifyMsg(NULL, i, DEF_NOTIFY_CONSTRUCTIONPOINT, m_pClientList[i]->m_iConstructionPoint, m_pClientList[i]->m_iWarContribution, NULL, NULL);
								goto NKH_GOTOPOINT1;
							}

						ZeroMemory(cData, sizeof(cData));
						cp = (char*)cData;
						*cp = GSM_CONSTRUCTIONPOINT;
						cp++;
						ip = (int*)cp;
						*ip = m_pNpcList[sAttackerH]->m_iGuildGUID;
						cp += 4;
						ip = (int*)cp;
						*ip = iConstructionPoint;
						cp += 4;
						bStockMsgToGateServer(cData, 9);
					}
				}
				break;
			}
		}
	}

	// new - heldenian
	else if (m_bIsHeldenianMode == TRUE) {
		if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH]->m_cSide != m_pNpcList[iNpcH]->m_cSide)) {
			switch (m_pNpcList[iNpcH]->m_sType) {
			case 82:
			case 83:
			case 84:
			case 85:
			case 86:
			case 87:
			case 88:
			case 89:
				iWarContribution = 1000;
				break;
			}
			if (iWarContribution > 0) m_pClientList[sAttackerH]->m_iWarContribution += iWarContribution;
			SendNotifyMsg(NULL, sAttackerH, DEF_NOTIFY_CONSTRUCTIONPOINT, 0, m_pClientList[sAttackerH]->m_iWarContribution, 1, 0, 0, 0);
		}
	}

NKH_GOTOPOINT1:;

	// v1.411 ¸¸¾à ExplosiveÇÑ ¸ó½ºÅÍ°¡ Á×Àº °Å¶ó¸é Explosive
	if (m_pNpcList[iNpcH]->m_cSpecialAbility == 7) {
		m_pNpcList[iNpcH]->m_iMana = 100;
		m_pNpcList[iNpcH]->m_iMagicHitRatio = 100;
		NpcMagicHandler(iNpcH, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY, 30);
	}
	else if (m_pNpcList[iNpcH]->m_cSpecialAbility == 8) {
		m_pNpcList[iNpcH]->m_iMana = 100;
		m_pNpcList[iNpcH]->m_iMagicHitRatio = 100;
		NpcMagicHandler(iNpcH, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY, 61);
	}
	// new spawns boss when no monsters exist and apocalypse zone == 2
	if ((m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_iTotalAliveObject == 0) &&
		(m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_iApocalypseMobGenType == 2)) {
		MobBossGenerator(m_pNpcList[iNpcH]->m_cMapIndex);
	}
	// if boss mob is killed and zone is RecallImpossible, recall all people inside place
	if (m_pNpcList[iNpcH]->m_bIsBossMob == TRUE) {
		iMapIndex = NULL;
		for (i = 1; i < DEF_MAXCLIENTS; i++)
			iMapIndex = m_pNpcList[iNpcH]->m_cMapIndex;
		if ((m_pClientList[i] != NULL) && (m_pClientList[i]->m_bIsInitComplete == TRUE) && (iMapIndex >= 0) && (iMapIndex < DEF_MAXMAPS) && (m_pMapList[iMapIndex] != NULL) && (m_pMapList[iMapIndex]->m_bIsRecallImpossible == TRUE)) {
			m_pClientList[i]->m_iTimeLeft_ForceRecall = 200;
			m_pClientList[i]->m_bIsImpossibleZone = TRUE;
			SendNotifyMsg(NULL, i, DEF_NOTIFY_FORCERECALLTIME, m_pClientList[i]->m_iTimeLeft_ForceRecall, NULL, NULL, NULL);
		}
	}
	if ((m_bIsHeldenianMode == TRUE) && (m_cHeldenianType == 1)
		&& (m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_bIsHeldenianMap == TRUE))
	{
		if ((m_pNpcList[iNpcH]->m_sType == 87) || (m_pNpcList[iNpcH]->m_sType == 89))
		{
			if (m_pNpcList[iNpcH]->m_cSide == 1)
			{
				m_iHeldenianAresdenLeftTower--;
				if (m_cHeldenianFirstDestroyedTowerWinner == 0)  m_cHeldenianFirstDestroyedTowerWinner = 2;

				UpdateHeldenianStatus(-1);
				if (m_iHeldenianAresdenLeftTower == 0)	HeldenianVictoryNow(2);
			}
			else if (m_pNpcList[iNpcH]->m_cSide == 2)
			{
				m_iHeldenianElvineLeftTower--;
				if (m_cHeldenianFirstDestroyedTowerWinner == 0)  m_cHeldenianFirstDestroyedTowerWinner = 1;

				UpdateHeldenianStatus(-1);
				if (m_iHeldenianElvineLeftTower == 0)	HeldenianVictoryNow(1);
			}
		}
	}

	if (m_bIsApocalypseMode) {
		for (i = 0; i < DEF_MAXCLIENTS; i++)
			if ((m_pClientList[i] != NULL) && (m_pClientList[i]->m_bIsOnApocMap == TRUE) && (m_pClientList[i]->m_bIsInitComplete == TRUE)) {
				int iMonsterCount;
				iMonsterCount = m_pMapList[m_pClientList[i]->m_cMapIndex]->m_iTotalAliveObject;
				SendNotifyMsg(NULL, i, DEF_NOTIFY_MONSTERCOUNTAPOC, iMonsterCount, NULL, NULL, NULL);
			}
	}
}

BOOL CGame::_bInitNpcAttr(class CNpc* pNpc, char* pNpcName, short sClass, char cSA)
{
	int i, iTemp;
	char cTmpName[21];
	int sTemp;
	double dV1, dV2, dV3;

	ZeroMemory(cTmpName, sizeof(cTmpName));
	strcpy(cTmpName, pNpcName);

	for (i = 0; i < DEF_MAXNPCTYPES; i++)
		if (m_pNpcConfigList[i] != NULL) {
			if (memcmp(cTmpName, m_pNpcConfigList[i]->m_cNpcName, 20) == 0) {
				// °°Àº ÀÌ¸§À» °¡Áø NPC ¼³Á¤À» Ã£¾Ò´Ù. ¼³Á¤°ªÀ¸·Î ÃÊ±âÈ­ÇÑ´Ù.
				ZeroMemory(pNpc->m_cNpcName, sizeof(pNpc->m_cNpcName));
				memcpy(pNpc->m_cNpcName, m_pNpcConfigList[i]->m_cNpcName, 20);

				pNpc->m_sType = m_pNpcConfigList[i]->m_sType;

				// HitDice¿¡ µû¶ó °ø½ÄÀÌ ´Ù¸£´Ù. ¸÷°£ÀÇ Â÷º°À» µÎ±â À§ÇÔ.
				if (m_pNpcConfigList[i]->m_iHitDice <= 5)
					pNpc->m_iHP = abs(iDice(m_pNpcConfigList[i]->m_iHitDice, 4) + m_pNpcConfigList[i]->m_iHitDice);
				else pNpc->m_iHP = abs((m_pNpcConfigList[i]->m_iHitDice * 4) + m_pNpcConfigList[i]->m_iHitDice + iDice(1, m_pNpcConfigList[i]->m_iHitDice));
				// v1.4 È®ÀÎÄÚµå
				if (pNpc->m_iHP == 0) pNpc->m_iHP = 1;

				//50Cent - HP Bar
				pNpc->m_iMaxHP = pNpc->m_iHP;

				pNpc->m_iExpDiceMin = m_pNpcConfigList[i]->m_iExpDiceMin;
				pNpc->m_iExpDiceMax = m_pNpcConfigList[i]->m_iExpDiceMax;
				pNpc->m_iGoldDiceMin = m_pNpcConfigList[i]->m_iGoldDiceMin;
				pNpc->m_iGoldDiceMax = m_pNpcConfigList[i]->m_iGoldDiceMax;
				pNpc->m_iExp = (iDice(1, (m_pNpcConfigList[i]->m_iExpDiceMax - m_pNpcConfigList[i]->m_iExpDiceMin)) + m_pNpcConfigList[i]->m_iExpDiceMin);

				pNpc->m_iHitDice = m_pNpcConfigList[i]->m_iHitDice;
				pNpc->m_iDefenseRatio = m_pNpcConfigList[i]->m_iDefenseRatio;
				pNpc->m_iHitRatio = m_pNpcConfigList[i]->m_iHitRatio;
				pNpc->m_iMinBravery = m_pNpcConfigList[i]->m_iMinBravery;
				pNpc->m_cAttackDiceThrow = m_pNpcConfigList[i]->m_cAttackDiceThrow;
				pNpc->m_cAttackDiceRange = m_pNpcConfigList[i]->m_cAttackDiceRange;
				pNpc->m_cSize = m_pNpcConfigList[i]->m_cSize;
				pNpc->m_cSide = m_pNpcConfigList[i]->m_cSide;

				pNpc->m_dwTamingTime = 0; // Taming
				pNpc->m_cOriginalSide = m_pNpcConfigList[i]->m_cSide;

				// Npc Poison Resistance
				pNpc->m_cPoisonResistance = m_pNpcConfigList[i]->m_cResistMagic;
				pNpc->m_iPoisonLevel = 0;

				pNpc->m_cActionLimit = m_pNpcConfigList[i]->m_cActionLimit;
				pNpc->m_dwActionTime = m_pNpcConfigList[i]->m_dwActionTime;
				pNpc->m_dwRegenTime = m_pNpcConfigList[i]->m_dwRegenTime;
				pNpc->m_cResistMagic = m_pNpcConfigList[i]->m_cResistMagic;
				pNpc->m_cMagicLevel = m_pNpcConfigList[i]->m_cMagicLevel;
				pNpc->m_iMaxMana = m_pNpcConfigList[i]->m_iMaxMana; // v1.4
				pNpc->m_iMana = m_pNpcConfigList[i]->m_iMaxMana;
				pNpc->m_cChatMsgPresence = m_pNpcConfigList[i]->m_cChatMsgPresence;
				pNpc->m_cDayOfWeekLimit = m_pNpcConfigList[i]->m_cDayOfWeekLimit;
				pNpc->m_cTargetSearchRange = m_pNpcConfigList[i]->m_cTargetSearchRange;
				pNpc->m_sAreaSize = m_pNpcConfigList[i]->m_sAreaSize;
				switch (sClass) {
				case 43:
				case 44:
				case 45:
				case 46:
				case 47: // ÀüÀï¿ë À¯´ÖÀÇ °æ¿ì ¹«Á¶°Ç °ø°Ý¸¸ ÇÏ´Â Àü·«À» ±¸»ç.
					pNpc->m_iAttackStrategy = DEF_ATTACKAI_NORMAL;
					break;

				default:
					pNpc->m_iAttackStrategy = iDice(1, 10);
					break;
				}

				pNpc->m_iAILevel = iDice(1, 3);
				pNpc->m_iAbsDamage = m_pNpcConfigList[i]->m_iAbsDamage;
				pNpc->m_iMagicHitRatio = m_pNpcConfigList[i]->m_iMagicHitRatio;
				pNpc->m_iAttackRange = m_pNpcConfigList[i]->m_iAttackRange;
				pNpc->m_cSpecialAbility = cSA;
				pNpc->m_iBuildCount = m_pNpcConfigList[i]->m_iMinBravery;

				// v1.411 NPCÀÇ Æ¯¼ö È¿°ú °è»ê. °æÇèÄ¡ °¡Áß 
				switch (pNpc->m_cSpecialAbility) {
				case 1:
					dV2 = (double)pNpc->m_iExp;
					dV3 = 25.0f / 100.0f;
					dV1 = dV2 * dV3;
					pNpc->m_iExp += (int)dV1;
					break;

				case 2:
					dV2 = (double)pNpc->m_iExp;
					dV3 = 30.0f / 100.0f;
					dV1 = dV2 * dV3;
					pNpc->m_iExp += (int)dV1;
					break;

				case 3: // Absorbing Physical Damage
					if (pNpc->m_iAbsDamage > 0) {
						// ±âº»ÀûÀ¸·Î ¸¶¹ý ´ë¹ÌÁö Àý°¨ ´É·ÂÀÌ ÀÖ´Â °æ¿ì ¹°¸® ´ë¹ÌÁö Àý°¨Àº Àû¿ëµÇÁö ¾Ê´Â´Ù. 
						pNpc->m_cSpecialAbility = NULL;
						cSA = NULL;
					}
					else {
						iTemp = 20 + iDice(1, 60);
						pNpc->m_iAbsDamage -= iTemp;
						if (pNpc->m_iAbsDamage < -90) pNpc->m_iAbsDamage = -90;
					}

					dV2 = (double)pNpc->m_iExp;
					dV3 = (double)abs(pNpc->m_iAbsDamage) / 100.0f;
					dV1 = dV2 * dV3;
					pNpc->m_iExp += (int)dV1;
					break;

				case 4: // Absorbing Magical Damage
					if (pNpc->m_iAbsDamage < 0) {
						// ±âº»ÀûÀ¸·Î ¹°¸® ´ë¹ÌÁö Àý°¨ ´É·ÂÀÌ ÀÖ´Â °æ¿ì ¸¶¹ý ´ë¹ÌÁö Àý°¨Àº Àû¿ëµÇÁö ¾Ê´Â´Ù. 
						pNpc->m_cSpecialAbility = NULL;
						cSA = NULL;
					}
					else {
						iTemp = 20 + iDice(1, 60);
						pNpc->m_iAbsDamage += iTemp;
						if (pNpc->m_iAbsDamage > 90) pNpc->m_iAbsDamage = 90;
					}

					dV2 = (double)pNpc->m_iExp;
					dV3 = (double)(pNpc->m_iAbsDamage) / 100.0f;
					dV1 = dV2 * dV3;
					pNpc->m_iExp += (int)dV1;
					break;

				case 5:
					dV2 = (double)pNpc->m_iExp;
					dV3 = 15.0f / 100.0f;
					dV1 = dV2 * dV3;
					pNpc->m_iExp += (int)dV1;
					break;

				case 6:
				case 7:
					dV2 = (double)pNpc->m_iExp;
					dV3 = 20.0f / 100.0f;
					dV1 = dV2 * dV3;
					pNpc->m_iExp += (int)dV1;
					break;

				case 8:
					dV2 = (double)pNpc->m_iExp;
					dV3 = 25.0f / 100.0f;
					dV1 = dV2 * dV3;
					pNpc->m_iExp += (int)dV1;
					break;
				}

				// v1.411 À§Ä¡ ¿Å±è 
				pNpc->m_iNoDieRemainExp = (pNpc->m_iExp) - (pNpc->m_iExp / 3);

				// v1.411 NPCÀÇ Æ¯¼ö ´É·Â »ðÀÔ 
				pNpc->m_iStatus = pNpc->m_iStatus & 0xFFFFF0FF;
				sTemp = cSA;
				sTemp = sTemp << 8;
				pNpc->m_iStatus = pNpc->m_iStatus | sTemp;

				// v1.41 NPCÀÇ Class»ðÀÔ.
				pNpc->m_iStatus = pNpc->m_iStatus & 0xFFFFFFF0;
				pNpc->m_iStatus = pNpc->m_iStatus | (sClass);

				return TRUE;
			}
		}

	// NPC¸®½ºÆ®¸¦ ¸ðµÎ °Ë»öÇßÀ½¿¡µµ ¹ß°ßµÇÁö ¾Ê¾Ò´Ù. 
	return FALSE;
}

void CGame::ReleaseFollowMode(short sOwnerH)
{
	int i;
	for (i = 0; i < DEF_MAXNPCS; i++)
		if ((i != sOwnerH) && (m_pNpcList[i] != NULL))
		{
			if ((m_pNpcList[i]->m_cMoveType == DEF_MOVETYPE_FOLLOW)
				&& (m_pNpcList[i]->m_cFollowOwnerType == DEF_OWNERTYPE_NPC)
				&& (m_pNpcList[i]->m_iFollowOwnerIndex == sOwnerH))
			{
				m_pNpcList[i]->m_cMoveType = DEF_MOVETYPE_RANDOMWAYPOINT;
				m_pNpcList[i]->m_iFollowOwnerIndex = 0;
				m_pNpcList[i]->m_cFollowOwnerType = 0;
			}
		}
}

void CGame::CalcNextWayPointDestination(int iNpcH)
{
	short sRange, sX, sY;
	int i, j, iMapIndex;
	BOOL bFlag;

	switch (m_pNpcList[iNpcH]->m_cMoveType) {
	case DEF_MOVETYPE_GUARD:
		// ÁöÁ¤µÈ À§Ä¡¸¦ ¹þ¾î³ªÁö ¾ÊÀ¸¸ç Ãß°ÝÀÌ ³¡³­ ÈÄ¿¡ µ¹¾Æ¿Â´Ù.
		break;

	case DEF_MOVETYPE_SEQWAYPOINT:

		m_pNpcList[iNpcH]->m_cCurWaypoint++;
		if (m_pNpcList[iNpcH]->m_cCurWaypoint >= m_pNpcList[iNpcH]->m_cTotalWaypoint)
			m_pNpcList[iNpcH]->m_cCurWaypoint = 1;
		m_pNpcList[iNpcH]->m_dX = (short)(m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_WaypointList[m_pNpcList[iNpcH]->m_iWayPointIndex[m_pNpcList[iNpcH]->m_cCurWaypoint]].x);
		m_pNpcList[iNpcH]->m_dY = (short)(m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_WaypointList[m_pNpcList[iNpcH]->m_iWayPointIndex[m_pNpcList[iNpcH]->m_cCurWaypoint]].y);
		break;

	case DEF_MOVETYPE_RANDOMWAYPOINT:

		m_pNpcList[iNpcH]->m_cCurWaypoint = (short)((rand() % (m_pNpcList[iNpcH]->m_cTotalWaypoint - 1)) + 1);
		m_pNpcList[iNpcH]->m_dX = (short)(m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_WaypointList[m_pNpcList[iNpcH]->m_iWayPointIndex[m_pNpcList[iNpcH]->m_cCurWaypoint]].x);
		m_pNpcList[iNpcH]->m_dY = (short)(m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_WaypointList[m_pNpcList[iNpcH]->m_iWayPointIndex[m_pNpcList[iNpcH]->m_cCurWaypoint]].y);
		break;

	case DEF_MOVETYPE_RANDOMAREA:

		sRange = (short)(m_pNpcList[iNpcH]->m_rcRandomArea.right - m_pNpcList[iNpcH]->m_rcRandomArea.left);
		m_pNpcList[iNpcH]->m_dX = (short)((rand() % sRange) + m_pNpcList[iNpcH]->m_rcRandomArea.left);
		sRange = (short)(m_pNpcList[iNpcH]->m_rcRandomArea.bottom - m_pNpcList[iNpcH]->m_rcRandomArea.top);
		m_pNpcList[iNpcH]->m_dY = (short)((rand() % sRange) + m_pNpcList[iNpcH]->m_rcRandomArea.top);
		break;

	case DEF_MOVETYPE_RANDOM:
		// ¿ÏÀü ·£´ý 
		iMapIndex = m_pNpcList[iNpcH]->m_cMapIndex;

		for (i = 0; i <= 30; i++) {
			// °¥ ¼ö ÀÖ´Â ÁÂÇ¥°¡ ³ª¿Ã¶§ ±îÁö 30¹ø ¹Ýº¹ 
			sX = (rand() % (m_pMapList[iMapIndex]->m_sSizeX - 50)) + 15;
			sY = (rand() % (m_pMapList[iMapIndex]->m_sSizeY - 50)) + 15;

			bFlag = TRUE;
			for (j = 0; j < DEF_MAXMGAR; j++)
				if (m_pMapList[iMapIndex]->m_rcMobGenAvoidRect[j].left != -1) {
					// ÇÇÇØ¾ß ÇÒ ÁÂÇ¥°¡ ÀÖ´Ù. 
					if ((sX >= m_pMapList[iMapIndex]->m_rcMobGenAvoidRect[j].left) &&
						(sX <= m_pMapList[iMapIndex]->m_rcMobGenAvoidRect[j].right) &&
						(sY >= m_pMapList[iMapIndex]->m_rcMobGenAvoidRect[j].top) &&
						(sY <= m_pMapList[iMapIndex]->m_rcMobGenAvoidRect[j].bottom)) {
						// Avoid Rect¾ÈÀÌ¹Ç·Î ÀÌ À§Ä¡¿¡´Â »ý¼º½ÃÅ³ ¼ö ¾ø´Ù.	
						bFlag = FALSE;
					}
				}
			if (bFlag == TRUE) goto CNW_GET_VALIDLOC_SUCCESS;
		}
		// Fail! 
		m_pNpcList[iNpcH]->m_tmp_iError = 0;
		return;

	CNW_GET_VALIDLOC_SUCCESS:;
		m_pNpcList[iNpcH]->m_dX = sX;
		m_pNpcList[iNpcH]->m_dY = sY;
		break;
	}

	m_pNpcList[iNpcH]->m_tmp_iError = 0; // @@@ !!! @@@
}

void CGame::DeleteNpc(int iNpcH)
{
	int  i, iNamingValue, iNumItem, iItemID, iItemIDs[MAX_NPCITEMDROP], iSlateID;
	char cTmp[21], cItemName[21];
	class CItem* pItem, * pItem2;
	DWORD dwCount, dwTime;
	POINT ItemPositions[MAX_NPCITEMDROP];
	char cTemp[256];
	SYSTEMTIME SysTime;
	if (m_pNpcList[iNpcH] == NULL) return;

	dwTime = timeGetTime();

	//Init number of items to 1 unless its a multidrop;
	iNumItem = 0;
	iItemID = 0; // No current item

	//ì£½ì–´ìžˆë˜ ìœ„ì¹˜ì—ì„œ ì‚­ì œ 
	SendEventToNearClient_TypeA(iNpcH, DEF_OWNERTYPE_NPC, MSGID_EVENT_LOG, DEF_MSGTYPE_REJECT, NULL, NULL, NULL);
	m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->ClearOwner(11, iNpcH, DEF_OWNERTYPE_NPC, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY);

	ZeroMemory(cTmp, sizeof(cTmp));
	strcpy(cTmp, (char*)(m_pNpcList[iNpcH]->m_cName + 2));
	// NPCì˜ NamigValueë¥¼ ì–»ì–´ì™€ ì‚¬ìš©ì¤‘ì¸ í‘œì‹œë¥¼ í•´ì§€í•œë‹¤.
	iNamingValue = atoi(cTmp);

	// NamingValueë¥¼ ë¹„ìš°ê³  ë™ìž‘ì¤‘ì¸ ê°œì²´ ìˆ˜ë¥¼ ê°ì†Œì‹œí‚¨ë‹¤.
	m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->SetNamingValueEmpty(iNamingValue);
	m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_iTotalActiveObject--;
	minimap_clear_apoc(iNpcH);
	if (m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_iTotalActiveObject == 0)
	{
		if (m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_iApocalypseMobGenType == 1)
		{	// See if we need to open a dynamic Gate, then tell clients
			Open_EmptyMap_Gate(m_pNpcList[iNpcH]->m_cMapIndex);
		}
		if (m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_iApocalypseMobGenType == 2)
		{	// generate Abaddon and open Dynamic Gate
			GenerateApocalypseBoss(m_pNpcList[iNpcH]->m_cMapIndex);
			// Then change m_iApocalypseMobGenType to 1 for Gate openning when this boss is dead
			m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_iApocalypseMobGenType = 1;
		}
	}

	// Spot-mob-generatorì •ë³´ 
	if (m_pNpcList[iNpcH]->m_iSpotMobIndex != NULL)
		m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_stSpotMobGenerator[m_pNpcList[iNpcH]->m_iSpotMobIndex].iCurMobs--;

	RemoveFromTarget(iNpcH, DEF_OWNERTYPE_NPC);

	// í¬ë£¨ì„¸ì´ë“œìš© ê±´ì¶•ë¬¼ì´ë¼ë©´ ì§€íœ˜ê´€ í†µë³´ìš© ê±´ì¶•ë¬¼ ë¦¬ìŠ¤íŠ¸ë¥¼ í•´ì œ
	switch (m_pNpcList[iNpcH]->m_sType) {
	case 36:
	case 37:
	case 38:
	case 39:
	case 42:
		m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->bRemoveCrusadeStructureInfo(m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY);
		// ê¸¸ë“œ ê±´ì¶•ë¬¼ ê°œìˆ˜ë„ ê°ì†Œì‹œí‚´ 
		for (i = 0; i < DEF_MAXGUILDS; i++) {
			if (m_pGuildTeleportLoc[i].m_iV1 == m_pNpcList[iNpcH]->m_iGuildGUID) {
				m_pGuildTeleportLoc[i].m_dwTime = dwTime;
				m_pGuildTeleportLoc[i].m_iV2--;
				if (m_pGuildTeleportLoc[i].m_iV2 < 0) m_pGuildTeleportLoc[i].m_iV2 = 0;
				break;
			}
		}
		break;
	case 64:
		m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->bRemoveCropsTotalSum();
		break;

	}

	// DelayEventë¥¼ ì‚­ì œ 
	bRemoveFromDelayEventList(iNpcH, DEF_OWNERTYPE_NPC, NULL);
	if ((m_pNpcList[iNpcH]->m_bIsSummoned == FALSE) && (m_pNpcList[iNpcH]->m_bIsUnsummoned == FALSE)) {
		// ì†Œí™˜ëª¹ì´ ì•„ë‹ˆë¼ë©´ ì—°ê¸ˆ ìž¬ë£Œê°€ ë‚˜ì˜¨ë‹¤.
		pItem = new class CItem;
		ZeroMemory(cItemName, sizeof(cItemName));
		switch (m_pNpcList[iNpcH]->m_sType) {

		case 10: // Slime
			if (iDice(1, 25) == 1) iItemID = 220;  // SlimeJelly			
			break;

		case 11: // Skeleton
			switch (iDice(1, 2)) {
			case 1:if (iDice(1, 20) == 1) iItemID = 219; break; // SkeletonBones
			case 2: bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType); break;

			}
			break;

		case 12: // Stone-Golem
			switch (iDice(1, 2)) {
			case 1:	if (iDice(1, 30) == 1) iItemID = 221; break; // StoneGolemPiece
			case 2: bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType); break;

			}
			break;

		case 13: // Cyclops
			switch (iDice(1, 6)) {
			case 1:	if (iDice(1, 36) == 1) iItemID = 194; break; // CyclopsEye
			case 2:	if (iDice(1, 40) == 1) iItemID = 195; break; // CyclopsHandEdge
			case 3:	if (iDice(1, 30) == 1) iItemID = 196; break; // CyclopsHeart
			case 4:	if (iDice(1, 22) == 1) iItemID = 197; break; // CyclopsMeat
			case 5:	if (iDice(1, 40) == 1) iItemID = 198; break; // CyclopsLeather
			case 6:	bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType); break;

			}
			break;

		case 14: // Orc, Orc-Mage
			switch (iDice(1, 4)) {
			case 1:	if (iDice(1, 11) == 1) iItemID = 206; break; // OrcMeat
			case 2:	if (iDice(1, 20) == 1) iItemID = 207; break; // OrcLeather
			case 3:	if (iDice(1, 21) == 1) iItemID = 208; break; // OrcTeeth
			case 4: bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType); break;

			}
			break;

		case 16: //Giant-Ant
			switch (iDice(1, 3)) {
			case 1:	if (iDice(1, 9) == 1) iItemID = 192; break; // AntLeg
			case 2:	if (iDice(1, 10) == 1) iItemID = 193; break; // AntFeeler
			case 3:	bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType); break;

			}
			break;

		case 17: //Scorpion
			switch (iDice(1, 5)) {
			case 1:	if (iDice(1, 50) == 1) iItemID = 215; break; // ScorpionPincers
			case 2:	if (iDice(1, 20) == 1) iItemID = 216; break; // ScorpionMeat
			case 3: if (iDice(1, 50) == 1) iItemID = 217; break; // ScorpionSting
			case 4: if (iDice(1, 40) == 1) iItemID = 218; break; // ScorpionSkin
			case 5: bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType); break;

			}
			break;

		case 22: //Amphis
			switch (iDice(1, 5)) {
			case 1: if (iDice(1, 15) == 1) iItemID = 188; break; // SnakeMeat
			case 2:	if (iDice(1, 16) == 1) iItemID = 189; break; // SnakeSkin
			case 3:	if (iDice(1, 16) == 1) iItemID = 190; break; // SnakeTeeth
			case 4:	if (iDice(1, 17) == 1) iItemID = 191; break; // SnakeTongue
			case 5: bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType); break;

			}
			break;

		case 23: //Clay-Golem
			switch (iDice(1, 2)) {
			case 1: if (iDice(1, 30) == 1) iItemID = 205; break; // LumpofClay
			case 2: bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType); break;

			}
			break;


		case 27: //Hellbound
			switch (iDice(1, 7)) {
			case 1:	if (iDice(1, 40) == 1) iItemID = 199; break; // HelboundHeart
			case 2:	if (iDice(1, 38) == 1) iItemID = 200; break; // HelboundLeather
			case 3:	if (iDice(1, 38) == 1) iItemID = 201; break; // HelboundTail
			case 4:	if (iDice(1, 36) == 1) iItemID = 202; break; // HelboundTeeth
			case 5:	if (iDice(1, 36) == 1) iItemID = 203; break; // HelboundClaw
			case 6:	if (iDice(1, 50) == 1) iItemID = 204; break; // HelboundTongue
			case 7: bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType); break;

			}
			break;

		case 28: //Troll
			switch (iDice(1, 5)) {
			case 1:	if (iDice(1, 35) == 1) iItemID = 222; break; // TrollHeart
			case 2:	if (iDice(1, 23) == 1) iItemID = 223; break; // TrollMeat
			case 3:	if (iDice(1, 25) == 1) iItemID = 224; break; // TrollLeather
			case 4:	if (iDice(1, 27) == 1) iItemID = 225; break; // TrollClaw
			case 5: bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType); break;

			}
			break;

		case 29: //Orge
			switch (iDice(1, 7)) {
			case 1:	if (iDice(1, 20) == 1) iItemID = 209; break; // OgreHair
			case 2:	if (iDice(1, 22) == 1) iItemID = 210; break; // OgreHeart
			case 3:	if (iDice(1, 25) == 1) iItemID = 211; break; // OgreMeat
			case 4:	if (iDice(1, 25) == 1) iItemID = 212; break; // OgreLeather
			case 5:	if (iDice(1, 28) == 1) iItemID = 213; break; // OgreTeeth
			case 6:	if (iDice(1, 28) == 1) iItemID = 214; break; // OgreClaw
			case 7: bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType); break;

			}
			break;

		case 31: //Demon
		case 93: //Black-Demon
			switch (iDice(1, 5)) {
			case 1:	if (iDice(1, 400) == 123) iItemID = 541; break; // DemonHeart
			case 2:	if (iDice(1, 1000) == 123) iItemID = 542; break; // DemonMeat
			case 3:	if (iDice(1, 200) == 123) iItemID = 543; break; // DemonLeather
			case 4:	if (iDice(1, 300) == 123) iItemID = 540; break; // DemonEye
			case 5: bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType); break;

			}
			break;

		case 32: //Unicorn
			switch (iDice(1, 5)) {
			case 1:	if (iDice(1, 3000) == 396) iItemID = 544; break; // UnicornHeart
			case 2:	if (iDice(1, 500) == 3) iItemID = 545; break; // UnicornHorn
			case 3:	if (iDice(1, 100) == 3) iItemID = 546; break; // UnicornMeat
			case 4:	if (iDice(1, 200) == 3) iItemID = 547; break; // UnicornLeather
			case 5: bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType); break;

			}
			break;

		case 33: //WereWolf
			switch (iDice(1, 8)) {
			case 1: if (iDice(1, 30) == 3) iItemID = 551; break; // WerewolfTail
			case 2:	if (iDice(1, 28) == 3) iItemID = 548; break; // WerewolfHeart
			case 3:	if (iDice(1, 25) == 3) iItemID = 550; break; // WerewolfMeat
			case 4:	if (iDice(1, 35) == 3) iItemID = 553; break; // WerewolfLeather
			case 5:	if (iDice(1, 28) == 3) iItemID = 552; break; // WerewolfTeeth
			case 6:	if (iDice(1, 28) == 3) iItemID = 554; break; // WerewolfClaw
			case 7:	if (iDice(1, 38) == 3) iItemID = 549; break; // WerewolfNail
			case 8: bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType); break;

			}
			break;

		case 48: //Stalker
		case 49: //Hellclaw
		case 50: //Tigerworm
		case 52: //Gagoyle
		case 53: //Beholder
		case 55: //Rabbit
		case 56: //Cat
		case 57: //Giant-Frog
		case 58: //Mountain-Giant
		case 59: //Ettin
		case 60: //Cannibal-Plant
		case 61: //Rudolph
		case 62: //DireBoar
		case 63: //Frost
		case 65: //Ice
		case 54: //Dark-Elf
		case 30: //Liche
		case 18: //Zombie
			bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType); 
			
			break;

		// new 05/10/2004
		case 66: // Wyvern
		case 73: // Fire-Wyvern
		case 92: // Eternal Dragon
		case 94: // BlackWyv
		case 95: // LighWyvern
		case 96: // PoisonWyvern
		case 97: // HeavenWyvern
		case 98: // IllusionWyvern
			bGetMultipleItemNamesWhenDeleteNpc(m_pNpcList[iNpcH]->m_sType,	// NPC Type
				50,							// È®·ü( ÃÖ¼Ò ~ ÃÖ´ë »çÀÌÀÇ ¾ÆÀÌÅÛÀÌ ³ª¿Ã È®·ü , 100 ÀÌ¸é ÃÖ´ë °¹¼ö ¸¸Å­ ³ª¿Â´Ù.)
				5,							// ³ª¿Í¾ß ÇÏ´Â Item ÃÖ¼Ò °³¼ö
				15,							// ³ª¿Ã¼ö ÀÖ´Â Item ÃÖ´ë °³¼ö
				m_pNpcList[iNpcH]->m_sX,	// ±âÁØ ÁÂÇ¥ X
				m_pNpcList[iNpcH]->m_sY,	// ±âÁØ ÁÂÇ¥ Y
				DEF_ITEMSPREAD_FIXED,		// ¾ÆÀÌÅÛ »Ñ¸®´Â ¹æ¹ý (RANDOM, FIXED)
				4,							// ¾ÆÀÌÅÛ »Ñ¸± ¹üÀ§, DEF_ITEMSPREAD_RANDOM ÀÏ¶§¸¸ »ç¿ë
				iItemIDs,					// ¹Þ¾Æ¿Ã ¾ÆÀÌÅÛ Idµé
				ItemPositions,				// ¾ÆÀÌÅÛµéÀÇ À§Ä¡
				&iNumItem);					// ¾ÆÀÌÅÛÀÇ ÃÑ °¹¼ö
			break;

		case 81: // Abaddon
		case 99: // Ghost-Abaddon
			bGetMultipleItemNamesWhenDeleteNpc(m_pNpcList[iNpcH]->m_sType,	// NPC Type
				50,							// È®·ü( ÃÖ¼Ò ~ ÃÖ´ë »çÀÌÀÇ ¾ÆÀÌÅÛÀÌ ³ª¿Ã È®·ü , 100 ÀÌ¸é ÃÖ´ë °¹¼ö ¸¸Å­ ³ª¿Â´Ù.)
				12,							// ³ª¿Í¾ß ÇÏ´Â Item ÃÖ¼Ò °³¼ö
				20,							// ³ª¿Ã¼ö ÀÖ´Â Item ÃÖ´ë °³¼ö
				m_pNpcList[iNpcH]->m_sX,	// ±âÁØ ÁÂÇ¥ X
				m_pNpcList[iNpcH]->m_sY,	// ±âÁØ ÁÂÇ¥ Y
				DEF_ITEMSPREAD_FIXED,		// ¾ÆÀÌÅÛ »Ñ¸®´Â ¹æ¹ý (RANDOM, FIXED)
				65,							// ¾ÆÀÌÅÛ »Ñ¸± ¹üÀ§, DEF_ITEMSPREAD_RANDOM ÀÏ¶§¸¸ »ç¿ë
				iItemIDs,					// ¹Þ¾Æ¿Ã ¾ÆÀÌÅÛ Idµé
				ItemPositions,				// ¾ÆÀÌÅÛµéÀÇ À§Ä¡
				&iNumItem);
			break;

		}

		dwCount = 1;
		if (iNumItem > 0) {
			GetLocalTime(&SysTime);
			wsprintf(cTemp, "%d%02d%", SysTime.wMonth, SysTime.wDay);
			for (int j = 0; j < iNumItem; j++) {
				if (pItem == NULL) {
					pItem = new class CItem;
				}
				if (_bInitItemAttr(pItem, iItemIDs[j]) == FALSE ||
					m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->bGetIsMoveAllowedTile(ItemPositions[j].x, ItemPositions[j].y) == FALSE) {
					delete pItem;
					pItem = NULL;
				}
				else {
					if (iItemIDs[j] == 90 || iItemIDs[j] == 77) // Gold or Arrow
						pItem->m_dwCount = iDice(10, 15000);
					else
						pItem->m_dwCount = dwCount;

					pItem->m_sTouchEffectType = DEF_ITET_ID;
					pItem->m_sTouchEffectValue1 = iDice(1, 100000);
					pItem->m_sTouchEffectValue2 = iDice(1, 100000);
					pItem->m_sTouchEffectValue3 = (short)timeGetTime();
					m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->bSetItem(ItemPositions[j].x, ItemPositions[j].y, pItem);
					
					//SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pNpcList[iNpcH]->m_cMapIndex,
					//	ItemPositions[j].x, ItemPositions[j].y, pItem->m_sSprite, pItem->m_sSpriteFrame, pItem->m_cItemColor);
					SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pNpcList[iNpcH]->m_cMapIndex,
						(short)ItemPositions[j].x, (short)ItemPositions[j].y, pItem->m_sIDnum, pItem->m_sSpriteFrame, pItem->m_cItemColor, pItem->m_dwAttribute);

					_bItemLog(DEF_ITEMLOG_NEWGENDROP, NULL, m_pNpcList[iNpcH]->m_cNpcName, pItem);
					pItem = NULL;
				}
			}
		}
		else {
			if (_bInitItemAttr(pItem, iItemID) == FALSE) {
				delete pItem;
				pItem = NULL;
			}
			else {
				pItem->m_dwCount = dwCount;

				pItem->m_sTouchEffectType = DEF_ITET_ID;
				pItem->m_sTouchEffectValue1 = iDice(1, 100000);
				pItem->m_sTouchEffectValue2 = iDice(1, 100000);
				pItem->m_sTouchEffectValue3 = (short)timeGetTime();
				m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->bSetItem(m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY, pItem);
				
				//SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pNpcList[iNpcH]->m_cMapIndex,
				//	m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY, pItem->m_sSprite, pItem->m_sSpriteFrame, pItem->m_cItemColor);
				SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pNpcList[iNpcH]->m_cMapIndex,
					m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY,
					pItem->m_sIDnum, pItem->m_sSpriteFrame, pItem->m_cItemColor, pItem->m_dwAttribute);

				_bItemLog(DEF_ITEMLOG_NEWGENDROP, NULL, m_pNpcList[iNpcH]->m_cNpcName, pItem);
			}
		}

		if (iDice(1, 100000) < 10) {
			pItem2 = new class CItem;
			switch (iDice(1, 4)) {
			case 1:	iSlateID = 868; break;
			case 2: iSlateID = 869; break;
			case 3: iSlateID = 870; break;
			case 4: iSlateID = 871; break;
			}
			if (_bInitItemAttr(pItem2, iSlateID) == FALSE) {
				delete pItem2;
				pItem2 = NULL;
			}
			else {
				pItem2->m_dwCount = 1;

				pItem2->m_sTouchEffectType = DEF_ITET_ID;
				pItem2->m_sTouchEffectValue1 = iDice(1, 100000);
				pItem2->m_sTouchEffectValue2 = iDice(1, 100000);
				pItem2->m_sTouchEffectValue3 = (short)timeGetTime();

				m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->bSetItem(m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY, pItem2);
				//SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pNpcList[iNpcH]->m_cMapIndex,
				//	m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY, pItem2->m_sSprite, pItem2->m_sSpriteFrame, pItem2->m_cItemColor);
				SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pNpcList[iNpcH]->m_cMapIndex,
					m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY,
					pItem2->m_sIDnum, pItem2->m_sSpriteFrame, pItem2->m_cItemColor, pItem2->m_dwAttribute);
				_bItemLog(DEF_ITEMLOG_NEWGENDROP, NULL, m_pNpcList[iNpcH]->m_cNpcName, pItem2);
			}
		}
	}

	delete m_pNpcList[iNpcH];
	m_pNpcList[iNpcH] = NULL;
}

int CGame::iGetFollowerNumber(short sOwnerH, char cOwnerType)
{
	int i, iTotal;

	iTotal = 0;

	for (i = 1; i < DEF_MAXNPCS; i++)
		if ((m_pNpcList[i] != NULL) && (m_pNpcList[i]->m_cMoveType == DEF_MOVETYPE_FOLLOW)) {

			if ((m_pNpcList[i]->m_iFollowOwnerIndex == sOwnerH) && (m_pNpcList[i]->m_cFollowOwnerType == cOwnerType))
				iTotal++;
		}

	return iTotal;
}

void CGame::bSetNpcAttackMode(char* cName, int iTargetH, char cTargetType, BOOL bIsPermAttack)
{
	int i, iIndex;

	for (i = 1; i < DEF_MAXNPCS; i++)
		if ((m_pNpcList[i] != NULL) && (memcmp(m_pNpcList[i]->m_cName, cName, 5) == 0)) {
			iIndex = i;
			goto NEXT_STEP_SNAM1;

		}
	// °°Àº ÀÌ¸§À» °¡Áø NPC°¡ ¾ø´Ù.
	return;

NEXT_STEP_SNAM1:;

	switch (cTargetType) {
	case DEF_OWNERTYPE_PLAYER:
		if (m_pClientList[iTargetH] == NULL) return;
		break;

	case DEF_OWNERTYPE_NPC:
		if (m_pNpcList[iTargetH] == NULL) return;
		break;
	}

	// ¸ñÇ¥¹°À» ÇÒ´çÇÑ´Ù. 
	m_pNpcList[iIndex]->m_cBehavior = DEF_BEHAVIOR_ATTACK;
	m_pNpcList[iIndex]->m_sBehaviorTurnCount = 0;
	m_pNpcList[iIndex]->m_iTargetIndex = iTargetH;
	m_pNpcList[iIndex]->m_cTargetType = cTargetType;

	// ¿µ±¸ °ø°Ý ¸ðµå ¿©ºÎ ¼³Á¤ 
	m_pNpcList[iIndex]->m_bIsPermAttackMode = bIsPermAttack;

}

int CGame::iGetNpcRelationship(int iWhatH, int iRecvH)
{
	int iRet;

	if (m_pClientList[iRecvH] == NULL) return 0;
	if (m_pNpcList[iWhatH] == NULL) return 0;

	iRet = 0;
	switch (m_pNpcList[iWhatH]->m_cSide) {
	case 10: iRet |= 8; break;//Fixed, Original 15 
	case 1:  iRet = (iRet | 4) | 2; break;//Fixed, Original 1 
	case 2:  iRet |= 4; break;//Fixed, Orignal 8 
	}

	return iRet;
}



int CGame::iGetNpcRelationship_SendEvent(int iNpcH, int iOpponentH)
{
	int iRet;

	// ¿©±â¼­ »çÀÌµå °è»êÇÏ´Â ¹æ¹ýÀº °üÁ¡ÀÌ ´Ù¸£¹Ç·Î ÁÖÀÇÇÑ´Ù. 
	// ³»(NPC) µ¥ÀÌÅÍ¸¦ ÀÛ¼ºÇÏ´Â °ÍÀÌ ¾Æ´Ï¶ó ´Ù¸¥ ÀÌ¿¡°Ô º¸³»´Â µ¥ÀÌÅÍ¸¦ ÀÛ¼ºÇÏ´Â °ÍÀÓÀ» ÁÖÀÇ.
	if (m_pClientList[iOpponentH] == NULL) return 0;
	if (m_pClientList[iOpponentH]->m_bIsInitComplete == FALSE) return 0;

	if (m_pNpcList[iNpcH] == NULL) return 0;

	iRet = 0;

	if (m_pClientList[iOpponentH]->m_iPKCount != 0) {
		// »ó´ë¹æÀÌ ¹üÁËÀÚÀÌÇÏ¸é ÆíÀÌ °°À¸¸é À§ÇùÀ¸·Î ¾Æ´Ï¸é NPC ÀûÀ¸·Î ÀÎ½ÄµÈ´Ù.
		if (m_pNpcList[iNpcH]->m_cSide == m_pClientList[iOpponentH]->m_cSide)
			iRet = 7;
		else iRet = 2;
	}
	else {
		if (m_pNpcList[iNpcH]->m_cSide != m_pClientList[iOpponentH]->m_cSide) {
			// ¸÷ÀÇ Æí°ú »ó´ë¹æÀÇ ÆíÀÌ ´Ù¸£´Ù.
			if (m_pNpcList[iNpcH]->m_cSide == 10)
				iRet = 2; // ¸÷ÀÌ ÀÏ¹Ý ¸÷ÀÌ´Ù. »ó´ë¹æ¿¡°Ô´Â ÀûÀ¸·Î ÀÎ½ÄµÈ´Ù.
			else
				if (m_pNpcList[iNpcH]->m_cSide == 0) iRet = 0; // ¸÷ÀÌ Áß¸³ÀÌ¸é ¹«Á¶°Ç Áß¸³ 	
				else
					if (m_pClientList[iOpponentH]->m_cSide == 0)
						iRet = 0; // »ó´ë¹æÀÌ Áß¸³ÀÌ´Ù. ¼ÒÈ¯¸÷°ú Áß¸³°úÀÇ °ü°è´Â Áß¸³. 
					else iRet = 2; // ¼ÒÈ¯¸÷°ú ½Ã¹Î ÀÌ»óÀÎµ¥ ÆíÀÌ ´Ù¸£¹Ç·Î Àû 

		}
		else iRet = 1; // °°Àº Æí 
	}

	return iRet;
}

void CGame::NpcRequestAssistance(int iNpcH)
{
	int ix, iy, sX, sY;
	short sOwnerH;
	char  cOwnerType;

	// iNpc ±Ù¹æ¿¡ ÀÖ´Â ÀüÅõÁßÀÌ ¾Æ´Ñ NPC¿¡°Ô µµ¿òÀ» ¿äÃ»ÇÑ´Ù. 
	if (m_pNpcList[iNpcH] == NULL) return;

	sX = m_pNpcList[iNpcH]->m_sX;
	sY = m_pNpcList[iNpcH]->m_sY;

	for (ix = sX - 8; ix <= sX + 8; ix++)
		for (iy = sY - 8; iy <= sY + 8; iy++) {
			m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
			if ((sOwnerH != NULL) && (m_pNpcList[sOwnerH] != NULL) && (cOwnerType == DEF_OWNERTYPE_NPC) &&
				(iNpcH != sOwnerH) && (m_pNpcList[sOwnerH]->m_cSide == m_pNpcList[iNpcH]->m_cSide) &&
				(m_pNpcList[sOwnerH]->m_bIsPermAttackMode == FALSE) && (m_pNpcList[sOwnerH]->m_cBehavior == DEF_BEHAVIOR_MOVE)) {

				// Á¶°Ç¿¡ ºÎÇÕÇÏ´Â NPC¸¦ Ã£¾Ò´Ù. 
				m_pNpcList[sOwnerH]->m_cBehavior = DEF_BEHAVIOR_ATTACK;
				m_pNpcList[sOwnerH]->m_sBehaviorTurnCount = 0;
				m_pNpcList[sOwnerH]->m_iTargetIndex = m_pNpcList[iNpcH]->m_iTargetIndex;
				m_pNpcList[sOwnerH]->m_cTargetType = m_pNpcList[iNpcH]->m_cTargetType;

				return;
			}
		}
}

void CGame::NpcTalkHandler(int iClientH, int iWho, int iQuest)
{
	char cRewardName[21], cTargetName[21];
	int iResMode, iQuestNum, iQuestType, iRewardType, iRewardAmount, iContribution, iX, iY, iRange, iTargetType, iTargetCount, iReward, i;

	iQuestNum = 0;
	/*ZeroMemory(cTargetName, sizeof(cTargetName));
	if (m_pClientList[iClientH] == NULL) return;
	switch (iWho) {
	case 1: break;
	case 2:	break;
	case 3:	break;
	case 4:
		iQuestNum = _iTalkToNpcResult_Cityhall(iClientH, &iQuestType, &iResMode, &iRewardType, &iRewardAmount, &iContribution, cTargetName, &iTargetType, &iTargetCount, &iX, &iY, &iRange);
		break;
	case 5: break;
	case 6:	break;
	case 32: break;
	case 21:
		iQuestNum = _iTalkToNpcResult_Guard(iClientH, &iQuestType, &iResMode, &iRewardType, &iRewardAmount, &iContribution, cTargetName, &iTargetType, &iTargetCount, &iX, &iY, &iRange);
		if (iQuestNum >= 1000) return;
		break;
	}*/

	//Magn0S:: Quest List
	for (i = 0; i < DEF_MAXQUEST; i++) { // Magn0S:: Multi Quest
		if (m_pClientList[iClientH]->m_iQuest[i] == 0) {
			if (iWho == 4 && iQuest > 0)
			{
				iQuestNum = iQuest;

				iReward = iDice(1, 3);
				iResMode = m_pQuestConfigList[iQuestNum]->m_iResponseMode;
				iRewardType = m_pQuestConfigList[iQuestNum]->m_iRewardType[iReward];
				iRewardAmount = m_pQuestConfigList[iQuestNum]->m_iRewardAmount[iReward];
				iContribution = m_pQuestConfigList[iQuestNum]->m_iContribution;

				ZeroMemory(cTargetName, sizeof(cTargetName));
				strcpy(cTargetName, m_pQuestConfigList[iQuestNum]->m_cTargetName); // <<, Crash line?

				iX = m_pQuestConfigList[iQuestNum]->m_sX;
				iY = m_pQuestConfigList[iQuestNum]->m_sY;
				iRange = m_pQuestConfigList[iQuestNum]->m_iRange;

				iTargetType = m_pQuestConfigList[iQuestNum]->m_iTargetType;
				iTargetCount = m_pQuestConfigList[iQuestNum]->m_iMaxCount;
				iQuestType = m_pQuestConfigList[iQuestNum]->m_iType;
			}

			ZeroMemory(cRewardName, sizeof(cRewardName));
			if (iQuestNum > 0) {
				if (iRewardType > 1) {
					strcpy(cRewardName, m_pItemConfigList[iRewardType]->m_cName);
				}
				else {
					switch (iRewardType) {
						// SNOOPY Strange here !

					case -1: strcpy(cRewardName, "XP Points"); break;
					}
				}

				m_pClientList[iClientH]->m_iAskedQuest = iQuestNum;
				//m_pClientList[iClientH]->m_iQuestRewardType[i] = iRewardType;
				//m_pClientList[iClientH]->m_iQuestRewardAmount[i] = iRewardAmount;
				//SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_IPACCOUNTINFO, NULL, NULL, NULL, "Send Quest Data");

				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_NPCTALK, iQuestType, iResMode, iRewardAmount, cRewardName, iContribution,
					iTargetType, iTargetCount, iX, iY, iRange, cTargetName);
			}
			else {
				switch (iQuestNum) {
				case  0: SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_NPCTALK, (iWho + 130), NULL, NULL, NULL, NULL); break;
				case -1:
				case -2:
				case -3:
				case -4: SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_NPCTALK, abs(iQuestNum) + 100, NULL, NULL, NULL, NULL); break;
				case -5: break;
				}
			}
		}
	}
}

void CGame::SetSummonMobAction(int iClientH, int iMode, DWORD dwMsgSize, char* pData)
{
	int i, iTargetIndex;
	char   seps[] = "= \t\n";
	char* token, cTargetName[11], cBuff[256];
	class  CStrTok* pStrTok;
	if (m_pClientList[iClientH] == NULL) return;
	if (m_pClientList[iClientH]->m_cSide == 0) return;
	switch (iMode) {
	case 8: // Mobilize
		if ((m_bIsHeldenianMode == TRUE)
			&& (m_pMapList[m_pClientList[iClientH]->m_cMapIndex] != NULL)
			&& (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsHeldenianMap == 1))
		{
			iRecoverFollowers(iClientH, TRUE);
		}
		break;
	case 0: // Free �
	case 1: // Hold �
		for (i = 0; i < DEF_MAXNPCS; i++)
			if (m_pNpcList[i] != NULL)
			{
				if ((m_pNpcList[i]->m_iFollowOwnerIndex == iClientH)
					&& ((m_pNpcList[i]->m_bIsSummoned == TRUE) || (m_pNpcList[i]->m_dwTamingTime != 0))
					&& (m_pNpcList[i]->m_cFollowOwnerType == DEF_OWNERTYPE_PLAYER))
				{
					m_pNpcList[i]->m_iSummonControlMode = iMode;
					m_pNpcList[i]->m_bIsPermAttackMode = FALSE;
					m_pNpcList[i]->m_cBehavior = DEF_BEHAVIOR_MOVE;
					m_pNpcList[i]->m_sBehaviorTurnCount = 0;
					m_pNpcList[i]->m_iTargetIndex = NULL;
				}
			}
		break;
	case 2:	// target
		if ((dwMsgSize) <= 0) return;
		memcpy(cBuff, pData, dwMsgSize);
		pStrTok = new class CStrTok(cBuff, seps);
		token = pStrTok->pGet();
		token = pStrTok->pGet();

		iTargetIndex = 0;
		if (token != NULL) {
			// token�� �� ������ ����� �̸� 
			if (strlen(token) > 10)
				memcpy(cTargetName, token, 10);
			else memcpy(cTargetName, token, strlen(token));

			// 2002.8.17 ����ȣ ����
			for (i = 1; i < DEF_MAXCLIENTS; i++)
			{
				if ((m_pClientList[i] != NULL) &&
					(memcmp(m_pClientList[i]->m_cCharName, cTargetName, 10) == 0) &&
					(strcmp(m_pClientList[iClientH]->m_cMapName, m_pClientList[i]->m_cMapName) == 0)) // adamas(map�� ���ƾ� �Ѵ�.)
				{
					// ��ǥ ĳ���͸� ã�Ҵ�. �ε��� ����
					iTargetIndex = i;
					goto SSMA_SKIPSEARCH;
				}
			}
		}
		break;

	SSMA_SKIPSEARCH:;
		if ((iTargetIndex != 0) && (m_pClientList[iTargetIndex]->m_cSide != 0)
			&& (m_pClientList[iTargetIndex]->m_cSide != m_pClientList[iClientH]->m_cSide))
		{
			for (i = 0; i < DEF_MAXNPCS; i++)
				if (m_pNpcList[i] != NULL)
				{
					if ((m_pNpcList[i]->m_cFollowOwnerType == DEF_OWNERTYPE_PLAYER)
						&& ((m_pNpcList[i]->m_bIsSummoned == TRUE) || (m_pNpcList[i]->m_dwTamingTime != 0))
						&& (m_pNpcList[i]->m_iFollowOwnerIndex == iClientH))
					{
						m_pNpcList[i]->m_iSummonControlMode = iMode;
						m_pNpcList[i]->m_cBehavior = DEF_BEHAVIOR_ATTACK;
						m_pNpcList[i]->m_sBehaviorTurnCount = 0;
						m_pNpcList[i]->m_iTargetIndex = iTargetIndex;
						m_pNpcList[i]->m_cTargetType = DEF_OWNERTYPE_PLAYER;
						m_pNpcList[i]->m_bIsPermAttackMode = TRUE;
					}
				}
		}
		delete pStrTok;
		break;
	}
}

int CGame::_iTalkToNpcResult_Guard(int iClientH, int* pQuestType, int* pMode, int* pRewardType, int* pRewardAmount, int* pContribution, char* pTargetName, int* pTargetType, int* pTargetCount, int* pX, int* pY, int* pRange)
{
	// °æºñº´°úÀÇ ´ëÈ­. ÇÃ·¹ÀÌ¾îÀÇ À§Ä¡¿¡ ÀûÀýÇÑ ´ëÈ­ ¸Þ½ÃÁö¸¦ µð½ºÇÃ·¹ÀÌÇÑ´Ù.
	if (m_pClientList[iClientH] == NULL) return 0;

	if (memcmp(m_pClientList[iClientH]->m_cLocation, "are", 3) == 0) {
		// ¸» °Ç »ç¶÷ÀÌ ¾Æ·¹½ºµ§ ¼Ò¼Ó 
		if (memcmp(m_pClientList[iClientH]->m_cMapName, "aresden", 7) == 0) {
			// ÀÚ±â ¸¶À» °æºñ¿¡°Ô ¸»À» °É¾úÀ½.
			SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_NPCTALK, (200), NULL, NULL, NULL, NULL);
			return 1000;
		}
		else
			if (memcmp(m_pClientList[iClientH]->m_cMapName, "elv", 3) == 0) {
				// ´Ù¸¥ ¸¶À» °æºñ¿¡°Ô ¸»À» °É¾úÀ½.	
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_NPCTALK, (201), NULL, NULL, NULL, NULL);
				return 1001;
			}
	}
	else
		if (memcmp(m_pClientList[iClientH]->m_cLocation, "elv", 3) == 0) {
			if (memcmp(m_pClientList[iClientH]->m_cMapName, "aresden", 7) == 0) {
				// ´Ù¸¥ ¸¶À» °æºñ¿¡°Ô ¸»À» °É¾úÀ½.	
				SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_NPCTALK, (202), NULL, NULL, NULL, NULL);
				return 1002;
			}
			else
				if (memcmp(m_pClientList[iClientH]->m_cMapName, "elv", 3) == 0) {
					// ÀÚ±â ¸¶À» °æºñ¿¡°Ô ¸»À» °É¾úÀ½.
					SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_NPCTALK, (203), NULL, NULL, NULL, NULL);
					return 1003;
				}
		}
		else
			if (memcmp(m_pClientList[iClientH]->m_cLocation, "NONE", 4) == 0) {
				if (memcmp(m_pClientList[iClientH]->m_cMapName, "aresden", 7) == 0) {
					// Áß¸³ÀÌ ¾Æ·¹½ºµ§ °æºñ¿¡°Ô ¸»À» °É¾úÀ½
					SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_NPCTALK, (204), NULL, NULL, NULL, NULL);
					return 1004;
				}
				else
					if (memcmp(m_pClientList[iClientH]->m_cMapName, "elvine", 6) == 0) {
						// Áß¸³ÀÌ ¿¤¹ÙÀÎ °æºñ¿¡°Ô ¸»À» °É¾úÀ½.
						SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_NPCTALK, (205), NULL, NULL, NULL, NULL);
						return 1005;
					}
					else
						if (memcmp(m_pClientList[iClientH]->m_cMapName, "default", 7) == 0) {
							SendNotifyMsg(NULL, iClientH, DEF_NOTIFY_NPCTALK, (206), NULL, NULL, NULL, NULL);
							return 1006;
						}
			}

	return 0;
}

int CGame::_iTalkToNpcResult_GuildHall(int iClientH, int* pQuestType, int* pMode, int* pRewardType, int* pRewardAmount, int* pContribution, char* pTargetName, int* pTargetType, int* pTargetCount, int* pX, int* pY, int* pRange)
{
	return -4;
}

int CGame::_iTalkToNpcResult_GShop(int iClientH, int* pQuestType, int* pMode, int* pRewardType, int* pRewardAmount, int* pContribution, char* pTargetName, int* pTargetType, int* pTargetCount, int* pX, int* pY, int* pRange)
{
	return -4;
}

int CGame::_iTalkToNpcResult_BSmith(int iClientH, int* pQuestType, int* pMode, int* pRewardType, int* pRewardAmount, int* pContribution, char* pTargetName, int* pTargetType, int* pTargetCount, int* pX, int* pY, int* pRange)
{
	return -4;
}

int CGame::_iTalkToNpcResult_WHouse(int iClientH, int* pQuestType, int* pMode, int* pRewardType, int* pRewardAmount, int* pContribution, char* pTargetName, int* pTargetType, int* pTargetCount, int* pX, int* pY, int* pRange)
{
	return -4;
}

int CGame::_iTalkToNpcResult_WTower(int iClientH, int* pQuestType, int* pMode, int* pRewardType, int* pRewardAmount, int* pContribution, char* pTargetName, int* pTargetType, int* pTargetCount, int* pX, int* pY, int* pRange)
{
	return -4;
}

// 05/21/2004 - Hypnotoad - Balances to drop status
void CGame::NpcDeadItemGenerator(int iNpcH, short sAttackerH, char cAttackerType)
{
	class CItem* pItem;
	char  cColor, cItemName[21];
	BOOL  bIsGold;
	int   iGenLevel, iResult, iItemID;
	DWORD dwType, dwValue;
	double dTmp1, dTmp2, dTmp3;
	short sMagicLevel = 0;
	short sElement;

	if (m_pNpcList[iNpcH] == NULL) return;
	if ((cAttackerType != DEF_OWNERTYPE_PLAYER) || (m_pNpcList[iNpcH]->m_bIsSummoned == TRUE)) return;
	if (m_pNpcList[iNpcH]->m_bIsUnsummoned == TRUE) return;

	ZeroMemory(cItemName, sizeof(cItemName));
	bIsGold = FALSE;

	switch (m_pNpcList[iNpcH]->m_sType) {
	// NPC not dropping Gold
	case 21: // Guard
	case 64: // Crop
		return;
	}

	// 6500 default; the lower the greater the Weapon/Armor/Wand Drop
	if (iDice(1, 10000) >= m_iPrimaryDropRate) {
		// 35% Drop 60% of that is gold
		// 35% Chance of drop (35/100)
		if (iDice(1, 10000) <= m_iGoldRate) { // Centuu : Agregado para controlar el drop de oro.
			switch (iDice(1, 2)) {
			case 1: iItemID = 90; break; // Gold: (35/100) * (60/100) = 21%
			case 2: iItemID = 77; break; // Arrow
			}
			// If a non-existing itemID is given create no item 
			pItem = new class CItem;
			if (_bInitItemAttr(pItem, iItemID) == FALSE) {
				delete pItem;
				return;
			}

			pItem->m_dwCount = (DWORD)(iDice(1, (m_pNpcList[iNpcH]->m_iGoldDiceMax - m_pNpcList[iNpcH]->m_iGoldDiceMin)) + m_pNpcList[iNpcH]->m_iGoldDiceMin);

			// v1.42 Gold 
			if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH]->m_iAddGold != NULL)) {
				dTmp1 = (double)m_pClientList[sAttackerH]->m_iAddGold;
				dTmp2 = (double)pItem->m_dwCount;
				dTmp3 = (dTmp1 / 100.0f) * dTmp2;
				pItem->m_dwCount += (int)dTmp3;
			}

			//Magn0S:: Multiplicador de Gold
			pItem->m_dwCount *= 10;
		}
		else {

			switch (m_pNpcList[iNpcH]->m_sType) {
			case 34: // Dummy
			case 55: // Rabbit
			case 56: //	Cat
				return;
			}
			// 9000 default; the lower the greater the Weapon/Armor/Wand Drop
			// 35% Drop 40% of that is an Item 
			if (iDice(1, 10000) < m_iSecondaryDropRate) {
				// 40% Drop 90% of that is a standard drop
				// Standard Drop Calculation: (35/100) * (40/100) * (90/100) = 12.6%
				iResult = iDice(1, 10000);
				if ((iResult >= 1) && (iResult <= 3000))          dwValue = 1;
				else if ((iResult >= 3001) && (iResult <= 4000))  dwValue = 2;
				else if ((iResult >= 4001) && (iResult <= 5500))  dwValue = 3;
				else if ((iResult >= 5501) && (iResult <= 7000))  dwValue = 4;
				else if ((iResult >= 7001) && (iResult <= 8500))  dwValue = 5;
				else if ((iResult >= 8501) && (iResult <= 9200))  dwValue = 6;
				else if ((iResult >= 9201) && (iResult <= 9800))  dwValue = 7;
				else if ((iResult >= 9801) && (iResult <= 10000)) dwValue = 8;
				
				switch (dwValue) {
				case 1: // Green Potion
				case 4: iItemID = 96; break; // Big Green Potion
				case 2: // Red Potion
				case 5: iItemID = 92; break; // Big Red Potion
				case 3: // Blue Potion
				case 6: iItemID = 94; break; // Big Blue Potion
				case 7: iItemID = 1000; break; // Kloness Potion
				case 8: 
					switch (iDice(1, 3)) {
						case 1: iItemID = 650; break; // Zemstone of Sacrifice
						case 2: iItemID = 656; break; // Xelima Stone
						case 3: iItemID = 657; break; // Merien Stone
						
					}
					break;

				}
				// If a non-existing item is created then delete the item
				pItem = new class CItem;
				if (_bInitItemAttr(pItem, iItemID) == FALSE) {
					delete pItem;
					return;
				}
			}
			else {
				// Valuable Drop Calculation: (35/100) * (40/100) * (10/100) = 1.4%
				// Define iGenLevel using Npc.cfg#
				switch (m_pNpcList[iNpcH]->m_sType) {

				case 10: // Slime
				case 16: // Giant-Ant
				case 22: // Amphis
				//case 55: // Rabbit
				//case 56: //	Cat
					iGenLevel = 1;
					break;

				case 11: // Skeleton
				case 14: // Orc, Orc-Mage
				case 17: // Scorpion
				case 18: // Zombie
					iGenLevel = 2;
					break;

				case 12: // Stone-Golem
				case 23: // Clay-Golem
					iGenLevel = 3;
					break;

				case 27: // Hellbound
				case 61: // Rudolph
					iGenLevel = 4;
					break;

				case 72: // Claw-Turtle
				case 76: // Giant-Plant
				case 74: // Giant-Crayfish
				case 13: // Cyclops
				case 28: // Troll
				case 53: // Beholder
				case 60: // Cannibal-Plant
				case 62: // DireBoar
					iGenLevel = 5;
					break;

				case 29: // Orge
				case 33: // WereWolf
				case 48: // Stalker
				case 54: // Dark-Elf
				case 65: // Ice-Golem
				case 78: // Minotaurus
					iGenLevel = 6;
					break;

				case 70: // Balrogs
				case 71: // Centaurus
				case 30: // Liche
				case 63: // Frost
				case 79: // Nizie
					iGenLevel = 7;
					break;

				case 31: // Demon
				case 32: // Unicorn
				case 49: // Hellclaw
				case 50: // Tigerworm
				case 52: // Gagoyle
					iGenLevel = 8;
					break;

				case 58: // MountainGiant
					iGenLevel = 9;
					break;

				case 77: // MasterMage-Orc
				case 59: // Ettin
				case 75: // Lizards
					iGenLevel = 10;
					break;

				default:
					iGenLevel = 0;
					break;
				}

				if (iGenLevel == 0) return;

				// Weapon Drop: 
				// 1.4% chance Valuable Drop 60% that it is a Weapon
				if (iDice(1, 10000) <= m_iPrimaryDropRate) {
					if (iDice(1, 10000) >= m_iSecondaryDropRate) {
						// 70% the Weapon is Melee
						switch (iGenLevel) {

						//Nixu : aca se manejan los Drop..

						case 1: // Slime, Giant-Ant, Amphis
							switch (iDice(1, 3)) {
								case 1: iItemID = 1;  break; // Dagger
								case 2: iItemID = 8;  break; // ShortSword
								case 3: iItemID = 59; break; // LightAxe

							}
							break;

						case 2: // Skeleton, Orc, Orc-Mage, Scorpion, Zombie
							switch (iDice(1, 6)) {
								case 1: iItemID = 12;  break; // MainGauche
								case 2: iItemID = 15;  break; // Gradius
								case 3: iItemID = 65;  break; // SexonAxe
								case 4: iItemID = 62;  break; // Tomahoc
								case 5: iItemID = 23;  break; // Sabre
								case 6: iItemID = 31;  break; // Esterk
							}
							break;

						case 3: // Stone-Golem, Clay-Golem
							switch (iDice(1, 4)) {
								case 1: iItemID = 17;  break; // LongSword
								case 2: iItemID = 68;  break; // DoubleAxe
								case 3: iItemID = 23;  break; // Sabre
								case 4: iItemID = 31;  break; // Esterk
							}
							break;

						case 4: // Hellbound, Rudolph
							switch (iDice(1, 5)) {
								case 1: iItemID = 23;  break; // Sabre
								case 2: iItemID = 25;  break; // Scimitar
								case 3: iItemID = 28;  break; // Falchion
								case 4: iItemID = 31;  break; // Esterk
								case 5: iItemID = 34;  break; // Rapier
							}
							break;

						case 5: // Cyclops, Troll, Beholder, Cannibal-Plant, DireBoar
							switch (iDice(1, 3)) {
								case 1: iItemID = 31;  break; // Esterk
								case 2: iItemID = 34;  break; // Rapier
								case 3: iItemID = 71;  break; // WarAxe
							}
							break;

						case 6: // Orge, WereWolf, Stalker, Dark-Elf, Ice-Golem
							switch (iDice(1, 9)) {
								case 1: iItemID = 50;  break; // GreatSword
								case 2: iItemID = 54;  break; // Flameberge
								case 3: iItemID = 46;  break; // Claymore
								case 4: iItemID = 31;  break; // Esterk
								case 5: iItemID = 34;  break; // Rapier
								case 6: iItemID = 617; break; // CompositeBow
								case 7: iItemID = 1044; break; // PlateBow
								case 8: iItemID = 1045; break; // KnightBow
								case 9: iItemID = 1046; break; // GiantBow
							}
							break;

						case 7: // Liche, Frost
							switch (iDice(1, 4)) {
								case 1: iItemID = 50;  break; // GreatSword
								case 2: iItemID = 54;  break; // Flameberge
								case 3: iItemID = 31;  break; // Esterk
								case 4: iItemID = 34;  break; // Rapier
							}
							break;

						case 8: // Demon, Unicorn, Hellclaw, Tigerworm, Gagoyle
							switch (iDice(1, 8)) {
								case 1: iItemID = 50;  break; // GreatSword
								case 2: iItemID = 54;  break; // Flameberge
								case 3: iItemID = 560; break; // BattleAxe
								case 4: iItemID = 31;  break; // Esterk
								case 5: iItemID = 34;  break; // Rapier
								case 6: iItemID = 55;  break; // Flameberge+1
								case 7: iItemID = 615; break; // GiantSword
								case 8: iItemID = 1045; break; // KnightBow
							}
							break;

						case 9: // MountainGiant
							switch (iDice(1, 6)) {
								case 1: iItemID = 23;  break; // Sabre
								case 2: iItemID = 25;  break; // Scimitar
								case 3: iItemID = 28;  break; // Falchion
								case 4: iItemID = 31;  break; // Esterk
								case 5: iItemID = 34;  break; // Rapier
								case 6: iItemID = 760; break; // Hammer
							}
							break;

						case 10: // Ettin
							switch (iDice(1, 5)) {
								case 1: iItemID = 46;  break; // Claymore
								case 2: iItemID = 31;  break; // Esterk
								case 3: iItemID = 34;  break; // Rapier
								case 4: iItemID = 760; break; // Hammer
								case 5: iItemID = 761; break; // BattleHammer
							}
							break;

						}
					}
					else {
						// 30% the weapon is a Wand
						switch (iGenLevel) {

						case 2:
						case 3:
						case 9:
						case 10:
							iItemID = 258; // MagicWand(MS0) 
							break; 
						case 4:
						case 5:
						case 6:
							iItemID = 257; // MagicWand(MS10) 
							break; 
						case 7:
						case 8:
							iItemID = 256; // MagicWand(MS20) 
							break; 
						}
					}
				}
				else {
					// 1.4% chance Valuable Drop 40% that drop is an Armor/Shield
					switch (iGenLevel) {

					case 1: // Slime, Giant-Ant, Amphis
					case 2: // Skeleton, Orc, Orc-Mage, Scorpion, Zombie
						switch (iDice(1, 4)) {
							case 1: iItemID = 79;  break; // WoodShield
							case 2: iItemID = 80;  break; // LeatherShield
							case 3: iItemID = 450; break; // Shoes
							case 4: iItemID = 451; break; // LongBoots
						}
						break;

					case 3: // Stone-Golem, Clay-Golem
						iItemID = 81; // TargeShield
						break;

					case 4: // Hellbound, Rudolph
						switch (iDice(1, 5)) {
							case 1: iItemID = 454; break; // Hauberk(M)
							case 2: iItemID = 472; break; // Hauberk(W)
							case 3: iItemID = 461; break; // ChainHose(M)
							case 4: iItemID = 482; break; // ChainHose(W)
							case 5: iItemID = 83;  break; // BlondeShield
						}
						break;

					case 5: // Cyclops, Troll, Beholder, Cannibal-Plant, DireBoar
						switch (iDice(1, 9)) {
							case 1: iItemID = 455; break; // LeatherArmor(M)
							case 2: iItemID = 475; break; // LeatherArmor(W)
							case 3: iItemID = 84;  break; // IronShield
							case 4: iItemID = 454; break; // Hauberk(M)
							case 5: iItemID = 472; break; // Hauberk(W)
							case 6: iItemID = 461; break; // ChainHose(M)
							case 7: iItemID = 482; break; // ChainHose(W)
							case 8: iItemID = 623; break; // Hood(M)
							case 9: iItemID = 624; break; // Hood(W)
						}
						break;

					case 6: // Orge, WereWolf, Stalker, Dark-Elf, Ice-Golem
						switch (iDice(1, 21)) {
							case 1: iItemID = 456; break; // ChainMail(M)
							case 2: iItemID = 476; break; // ChainMail(W)
							case 3: iItemID = 458; break; // PlateMail(M)
							case 4: iItemID = 478; break; // PlateMail(W)
							case 5: iItemID = 85; break; // LagiShield
							case 6: iItemID = 750; break; // Horned-Helm(M)
							case 7: iItemID = 751; break; // Wings-Helm(M)
							case 8: iItemID = 754; break; // Horned-Helm(W)
							case 9: iItemID = 755; break; // Wings-Helm(W)
							case 10: iItemID = 752; break; // Wizard-Cap(M) 
							case 11: iItemID = 753; break; // Wizard-Hat(M)
							case 12: iItemID = 756; break; // Wizard-Cap(W) 
							case 13: iItemID = 757; break; // Wizard-Hat(W) 
							case 14: iItemID = 454; break; // Hauberk(M)
							case 15: iItemID = 472; break; // Hauberk(W)
							case 16: iItemID = 461; break; // ChainHose(M)
							case 17: iItemID = 482; break; // ChainHose(W)
							case 18: iItemID = 623; break; // Hood(M)
							case 19: iItemID = 624; break; // Hood(W)
							case 20: iItemID = 455; break; // LeatherArmor(M)
							case 21: iItemID = 475; break; // LeatherArmor(W)

						}
						break;

					case 7: // Liche, Frost
						switch (iDice(1, 12)) {
							case 1: iItemID = 457; break; // ScaleMail(M)
							case 2: iItemID = 477; break; // ScaleMail(W)
							case 3: iItemID = 458; break; // PlateMail(M)
							case 4: iItemID = 478; break; // PlateMail(W)
							case 5: iItemID = 86; break; // KnightShield
							case 6: iItemID = 87; break; // TowerShield
							case 7: iItemID = 600; break; // Helm(M)
							case 8: iItemID = 602; break; // Helm(W)
							case 9: iItemID = 601; break; // Full-Helm(M)
							case 10: iItemID = 603; break; // Full-Helm(W)
							case 11: iItemID = 623; break; // Hood(M)
							case 12: iItemID = 624; break; // Hood(W)

						}
						break;

					case 8: // Demon, Unicorn, Hellclaw, Tigerworm, Gagoyle
						iItemID = 402; // Cape
						break;

					// Centu - Mountain-Giant, Ettin, MasterMage-Orc, Giant-Lizard
					case 9:
					case 10:
						switch (iDice(1, 21)) {
						case 1: iItemID = 456; break; // ChainMail(M)
						case 2: iItemID = 476; break; // ChainMail(W)
						case 3: iItemID = 458; break; // PlateMail(M)
						case 4: iItemID = 478; break; // PlateMail(W)
						case 5: iItemID = 85; break; // LagiShield
						case 6: iItemID = 750; break; // Horned-Helm(M)
						case 7: iItemID = 751; break; // Wings-Helm(M)
						case 8: iItemID = 754; break; // Horned-Helm(W)
						case 9: iItemID = 755; break; // Wings-Helm(W)
						case 10: iItemID = 752; break; // Wizard-Cap(M) 
						case 11: iItemID = 753; break; // Wizard-Hat(M)
						case 12: iItemID = 756; break; // Wizard-Cap(W) 
						case 13: iItemID = 757; break; // Wizard-Hat(W) 
						case 14: iItemID = 454; break; // Hauberk(M)
						case 15: iItemID = 472; break; // Hauberk(W)
						case 16: iItemID = 461; break; // ChainHose(M)
						case 17: iItemID = 482; break; // ChainHose(W)
						case 18: iItemID = 623; break; // Hood(M)
						case 19: iItemID = 624; break; // Hood(W)
						case 20: iItemID = 455; break; // LeatherArmor(M)
						case 21: iItemID = 475; break; // LeatherArmor(W)

						}
						break;
					}
				}

				pItem = new class CItem;
				if (_bInitItemAttr(pItem, iItemID) == FALSE) {
					delete pItem;
					return;
				}

				//Magn0S:: Begin to update Drop Management from .cfg file
				if (pItem->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ATTACK || pItem->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ATTACK_ARROW) {
					iResult = iDice(1, m_iMaxAttrWeaponDrop); //11500
					if ((iResult >= 1) && (iResult <= m_iAttrWeaponDrop[0])) {
						dwType = ITEMSTAT_LIGHT;
						cColor = 2;
					}
					else if ((iResult > m_iAttrWeaponDrop[0]) && (iResult <= m_iAttrWeaponDrop[1])) {
						dwType = ITEMSTAT_STRONG;
						cColor = 3;
					}
					else if ((iResult > m_iAttrWeaponDrop[1]) && (iResult <= m_iAttrWeaponDrop[2])) {
						dwType = ITEMSTAT_CRITICAL;
						cColor = 5;
					}
					else if ((iResult > m_iAttrWeaponDrop[2]) && (iResult <= m_iAttrWeaponDrop[3])) {
						dwType = ITEMSTAT_AGILE;
						cColor = 1;
					}
					else if ((iResult > m_iAttrWeaponDrop[3]) && (iResult <= m_iAttrWeaponDrop[4])) {
						dwType = ITEMSTAT_RIGHTEOUS;
						cColor = 7;
					}
					else if ((iResult > m_iAttrWeaponDrop[4]) && (iResult <= m_iAttrWeaponDrop[5])) {
						dwType = ITEMSTAT_POISONING;
						cColor = 4;
					}
					else if ((iResult > m_iAttrWeaponDrop[5]) && (iResult <= m_iAttrWeaponDrop[6])) {
						dwType = ITEMSTAT_SHARP;
						cColor = 6;
					}
					else if ((iResult > m_iAttrWeaponDrop[6]) && (iResult <= m_iAttrWeaponDrop[7])) {
						dwType = ITEMSTAT_ANCIENT;
						cColor = 8;
					}
					// Magic Weapons - Drops Improvement
					else if ((iResult >= m_iAttrWeaponDrop[7]) && (iResult <= m_iAttrWeaponDrop[8])) {
						dwType = ITEMSTAT_MAGIC; // Magic
						cColor = 10; // Black Color
					}
					// End of Magic Weapons

					pItem->m_cItemColor = cColor;

					// Magic Weapons - Set the Spell Effect on Dropped Item
					if (dwType == 15) {
						iResult = iDice(1, 30060);
						if ((iResult >= 1) && (iResult < 10000))           dwValue = 1;  // Fire-Strike
						else if ((iResult >= 10000) && (iResult < 17400))  dwValue = 2;  // Lightning
						else if ((iResult >= 17400) && (iResult < 22400))  dwValue = 3;  // Chill-Wind
						else if ((iResult >= 22400) && (iResult < 25400))  dwValue = 4;  // Ice-Strike
						else if ((iResult >= 25400) && (iResult < 27400))  dwValue = 5;  // Energy-Strike
						else if ((iResult >= 27400) && (iResult < 28400))  dwValue = 6;  // Mass-Fire-Strike
						else if ((iResult >= 28400) && (iResult < 28900))  dwValue = 7;  // Mass-Chill-Wind
						else if ((iResult >= 28900) && (iResult < 29300))  dwValue = 8;  // Earthworm-Strike
						else if ((iResult >= 29300) && (iResult < 29600))  dwValue = 9;  // Bloody-Shock-Wave
						else if ((iResult >= 29600) && (iResult < 29800))  dwValue = 10; // Mass-Ice-Strike
						else if ((iResult >= 29800) && (iResult < 29900))  dwValue = 11; // Lightning-Strike
						else if ((iResult >= 29900) && (iResult < 29970))  dwValue = 12; // Ice-Whirlwind
						else if ((iResult >= 29970) && (iResult < 30000))  dwValue = 13; // Meteor-Strike
						else if ((iResult >= 30000) && (iResult < 30025))  dwValue = 14; // Mass-Magic-Missile
						else if ((iResult >= 30025) && (iResult < 30045))  dwValue = 15; // Blizzard
						else if ((iResult >= 30045) && (iResult <= 30060))  dwValue = 16; // Earth-Shock-Wave
						else dwValue = 1;
						sMagicLevel = 1;
					}
					else {
						iResult = iDice(1, m_iMaxWeaponDrop);
						if ((iResult >= 1) && (iResult < m_iWeaponDrop[0]))           dwValue = 1;  // 10000/29348 = 34%
						else if ((iResult >= m_iWeaponDrop[0]) && (iResult < m_iWeaponDrop[1]))  dwValue = 2;  // 6600/29348 = 22.4%
						else if ((iResult >= m_iWeaponDrop[1]) && (iResult < m_iWeaponDrop[2]))  dwValue = 3;  // 4356/29348 = 14.8%
						else if ((iResult >= m_iWeaponDrop[2]) && (iResult < m_iWeaponDrop[3]))  dwValue = 4;  // 2874/29348 = 9.7%
						else if ((iResult >= m_iWeaponDrop[3]) && (iResult < m_iWeaponDrop[4]))  dwValue = 5;  // 1897/29348 = 6.4%
						else if ((iResult >= m_iWeaponDrop[4]) && (iResult < m_iWeaponDrop[5]))  dwValue = 6;  // 1252/29348 = 4.2%
						else if ((iResult >= m_iWeaponDrop[5]) && (iResult < m_iWeaponDrop[6]))  dwValue = 7;  // 826/29348 = 2.8%
						else if ((iResult >= m_iWeaponDrop[6]) && (iResult < m_iWeaponDrop[7]))  dwValue = 8;  // 545/29348 = 1.85%
						else if ((iResult >= m_iWeaponDrop[7]) && (iResult < m_iWeaponDrop[8]))  dwValue = 9;  // 360/29348 = 1.2%
						else if ((iResult >= m_iWeaponDrop[8]) && (iResult < m_iWeaponDrop[9]))  dwValue = 10; // 237/29348 = 0.8%
						else if ((iResult >= m_iWeaponDrop[9]) && (iResult < m_iWeaponDrop[10]))  dwValue = 11; // 156/29348 = 0.5%
						else if ((iResult >= m_iWeaponDrop[10]) && (iResult < m_iWeaponDrop[11]))  dwValue = 12; // 103/29348 = 0.3%
						else if ((iResult >= m_iWeaponDrop[12]) && (iResult <= m_iWeaponDrop[12]))  dwValue = 13; // 68/29348 = 0.1%
						else dwValue = 1; // v2.03 906
						sMagicLevel = 0;
					}
					// End of Magic Weapons 

					switch (dwType) {
					case ITEMSTAT_CRITICAL: // 
						if (dwValue <= 5) dwValue = 5;
						break;
					case ITEMSTAT_POISONING: // Min +20
						if (dwValue <= 4) dwValue = 4;
						break;
					case ITEMSTAT_LIGHT: // Min +16%
						if (dwValue <= 4) dwValue = 4;
						break;
					case ITEMSTAT_STRONG: // Min +14%					
						if (dwValue <= 2) dwValue = 2;
						break;
						// Magic Weapons - Better Stats for Strong NPC killed
					case ITEMSTAT_MAGIC:
						switch (iGenLevel) {
						case 1: // Slime, Giant-Ant, Amphis, Rabbit, Cat
							if (dwValue > 4) dwValue = 4;
							if (dwValue < 1) dwValue = 1;
							break;
						case 2: // Skeleton, Orc, Orc-Mage, Scorpion, Zombie
							if (dwValue > 6) dwValue = 6;
							if (dwValue < 3) dwValue = 3;
							break;
						case 3: // Stone-Golem, Clay-Golem
							if (dwValue > 7) dwValue = 7;
							if (dwValue < 4) dwValue = 4;
							break;
						case 4: // Hellbound, Rudolph
							if (dwValue > 8) dwValue = 8;
							if (dwValue < 5) dwValue = 5;
							break;
						case 5: // Cyclops, Troll, Beholder, Cannibal-Plant, DireBoar
							if (dwValue > 10) dwValue = 10;
							if (dwValue < 7) dwValue = 7;
							break;
						case 6: // Orge, WereWolf, Stalker, Dark-Elf, Ice-Golem
							if (dwValue > 12) dwValue = 12;
							if (dwValue < 9) dwValue = 9;
							break;
						case 7: // Liche, Frost
							if (dwValue > 13) dwValue = 13;
							if (dwValue < 10) dwValue = 10;
							break;
						case 8: // Demon, Unicorn, Hellclaw, Tigerworm, Gagoyle
							if (dwValue > 16) dwValue = 16;
							if (dwValue < 13) dwValue = 13;
							break;
						}
						break;
						// End of Magic Weapons
					}
					// Max 7
					if ((iGenLevel <= 2) && (dwValue > 7)) dwValue = 7;

					// Main
					pItem->m_dwAttribute = NULL;
					dwType = dwType << 20;
					dwValue = dwValue << 16;
					pItem->m_dwAttribute = pItem->m_dwAttribute | dwType | dwValue;

					// Rare Drop 40%
					if (iDice(1, 10000) <= m_iRareDropRate) {
						//  Hit Prob(50%),  CAD(35%),  Gold(10%), Exp(5%)
						iResult = iDice(1, m_iMaxStatedWeapon);
						if (iResult <= m_iStatedWeaponDrop[0])       dwType = ITEMSTAT2_HITPROB;
						else if (iResult <= m_iStatedWeaponDrop[1]) dwType = ITEMSTAT2_CAD;
						else if (iResult <= m_iStatedWeaponDrop[2]) dwType = ITEMSTAT2_GOLD;
						else if (iResult <= m_iStatedWeaponDrop[3]) dwType = ITEMSTAT2_EXP;

						// Magic Weapons - Set the Spell Effect on Dropped Item
						if (dwType == 15) {
							iResult = iDice(1, 30060);
							if ((iResult >= 1) && (iResult < 10000))           dwValue = 1;  // Fire-Strike
							else if ((iResult >= 10000) && (iResult < 17400))  dwValue = 2;  // Lightning
							else if ((iResult >= 17400) && (iResult < 22400))  dwValue = 3;  // Chill-Wind
							else if ((iResult >= 22400) && (iResult < 25400))  dwValue = 4;  // Ice-Strike
							else if ((iResult >= 25400) && (iResult < 27400))  dwValue = 5;  // Energy-Strike
							else if ((iResult >= 27400) && (iResult < 28400))  dwValue = 6;  // Mass-Fire-Strike
							else if ((iResult >= 28400) && (iResult < 28900))  dwValue = 7;  // Mass-Chill-Wind
							else if ((iResult >= 28900) && (iResult < 29300))  dwValue = 8;  // Earthworm-Strike
							else if ((iResult >= 29300) && (iResult < 29600))  dwValue = 9;  // Bloody-Shock-Wave
							else if ((iResult >= 29600) && (iResult < 29800))  dwValue = 10; // Mass-Ice-Strike
							else if ((iResult >= 29800) && (iResult < 29900))  dwValue = 11; // Lightning-Strike
							else if ((iResult >= 29900) && (iResult < 29970))  dwValue = 12; // Ice-Whirlwind
							else if ((iResult >= 29970) && (iResult < 30000))  dwValue = 13; // Meteor-Strike
							else if ((iResult >= 30000) && (iResult < 30025))  dwValue = 14; // Mass-Magic-Missile
							else if ((iResult >= 30025) && (iResult < 30045))  dwValue = 15; // Blizzard
							else if ((iResult >= 30045) && (iResult <= 30060))  dwValue = 16; // Earth-Shock-Wave
							else dwValue = 1;
							sMagicLevel = 1;
						}
						else {
							iResult = iDice(1, m_iMaxWeaponDrop);
							if ((iResult >= 1) && (iResult < m_iWeaponDrop[0]))           dwValue = 1;  // 10000/29348 = 34%
							else if ((iResult >= m_iWeaponDrop[0]) && (iResult < m_iWeaponDrop[1]))  dwValue = 2;  // 6600/29348 = 22.4%
							else if ((iResult >= m_iWeaponDrop[1]) && (iResult < m_iWeaponDrop[2]))  dwValue = 3;  // 4356/29348 = 14.8%
							else if ((iResult >= m_iWeaponDrop[2]) && (iResult < m_iWeaponDrop[3]))  dwValue = 4;  // 2874/29348 = 9.7%
							else if ((iResult >= m_iWeaponDrop[3]) && (iResult < m_iWeaponDrop[4]))  dwValue = 5;  // 1897/29348 = 6.4%
							else if ((iResult >= m_iWeaponDrop[4]) && (iResult < m_iWeaponDrop[5]))  dwValue = 6;  // 1252/29348 = 4.2%
							else if ((iResult >= m_iWeaponDrop[5]) && (iResult < m_iWeaponDrop[6]))  dwValue = 7;  // 826/29348 = 2.8%
							else if ((iResult >= m_iWeaponDrop[6]) && (iResult < m_iWeaponDrop[7]))  dwValue = 8;  // 545/29348 = 1.85%
							else if ((iResult >= m_iWeaponDrop[7]) && (iResult < m_iWeaponDrop[8]))  dwValue = 9;  // 360/29348 = 1.2%
							else if ((iResult >= m_iWeaponDrop[8]) && (iResult < m_iWeaponDrop[9]))  dwValue = 10; // 237/29348 = 0.8%
							else if ((iResult >= m_iWeaponDrop[9]) && (iResult < m_iWeaponDrop[10]))  dwValue = 11; // 156/29348 = 0.5%
							else if ((iResult >= m_iWeaponDrop[10]) && (iResult < m_iWeaponDrop[11]))  dwValue = 12; // 103/29348 = 0.3%
							else if ((iResult >= m_iWeaponDrop[12]) && (iResult <= m_iWeaponDrop[12]))  dwValue = 13; // 68/29348 = 0.1%
							else dwValue = 1; // v2.03 906
							sMagicLevel = 0;
						}
						// End of Magic Weapons 

						switch (dwType) {
						case ITEMSTAT2_HITPROB: // Min Hip Prob +21%
							if (dwValue <= 3) dwValue = 3;
							break;
						case ITEMSTAT2_CAD: // CAD Vai do +1  at� +7 s�
							if (dwValue > 7) dwValue = 7;
							break;
						case ITEMSTAT2_EXP: // Exp  +20%
							dwValue = 2;
							break;
						case ITEMSTAT2_GOLD: // Gold +50%
							dwValue = 5;
							break;
						}

						// Magic Weapons - Better Stats for Strong NPC Killed
						if (sMagicLevel > 0) {
							switch (iGenLevel) {
							case 1: // Slime, Giant-Ant, Amphis, Rabbit, Cat
								if (dwValue > 2) dwValue = 2;
								if (dwValue < 1) dwValue = 1;
								break;
							case 2: // Skeleton, Orc, Orc-Mage, Scorpion, Zombie
								if (dwValue > 3) dwValue = 3;
								if (dwValue < 1) dwValue = 1;
								break;
							case 3: // Stone-Golem, Clay-Golem
								if (dwValue > 4) dwValue = 4;
								if (dwValue < 2) dwValue = 2;
								break;
							case 4: // Hellbound, Rudolph
								if (dwValue > 5) dwValue = 5;
								if (dwValue < 2) dwValue = 2;
								break;
							case 5: // Cyclops, Troll, Beholder, Cannibal-Plant, DireBoar
								if (dwValue > 7) dwValue = 7;
								if (dwValue < 3) dwValue = 3;
								break;
							case 6: // Orge, WereWolf, Stalker, Dark-Elf, Ice-Golem
								if (dwValue > 9) dwValue = 9;
								if (dwValue < 5) dwValue = 5;
								break;
							case 7: // Liche, Frost
								if (dwValue > 10) dwValue = 10;
								if (dwValue < 6) dwValue = 6;
								break;
							case 8: // Demon, Unicorn, Hellclaw, Tigerworm, Gagoyle
								if (dwValue > 15) dwValue = 15;
								if (dwValue < 9) dwValue = 9;
								break;
							}
						}
						// End of Magic Weapons

						// Demais stats max � 7 (para mobs fracos iGenLevel)
						if ((iGenLevel <= 2) && (dwValue > 7)) dwValue = 7;

						dwType = dwType << 12;
						dwValue = dwValue << 8;

						pItem->m_dwAttribute = pItem->m_dwAttribute | dwType | dwValue;
					}
				}
				else if (pItem->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ATTACK_MANASAVE) {

					dwType = ITEMSTAT_CASTPROB;
					cColor = 5;

					pItem->m_cItemColor = cColor;

					// Magic Weapons - Set the Spell Effect on Dropped Item
					iResult = iDice(1, m_iMaxWeaponDrop);
					if ((iResult >= 1) && (iResult < m_iWeaponDrop[0])) dwValue = 1;
					else if ((iResult >= m_iWeaponDrop[0]) && (iResult < m_iWeaponDrop[1]))  dwValue = 2;
					else if ((iResult >= m_iWeaponDrop[1]) && (iResult < m_iWeaponDrop[2]))  dwValue = 3;
					else if ((iResult >= m_iWeaponDrop[2]) && (iResult < m_iWeaponDrop[3]))  dwValue = 4;
					else if ((iResult >= m_iWeaponDrop[3]) && (iResult < m_iWeaponDrop[4]))  dwValue = 5;
					else if ((iResult >= m_iWeaponDrop[4]) && (iResult < m_iWeaponDrop[5]))  dwValue = 6;
					else if ((iResult >= m_iWeaponDrop[5]) && (iResult < m_iWeaponDrop[6]))  dwValue = 7; 
					else if ((iResult >= m_iWeaponDrop[6]) && (iResult < m_iWeaponDrop[7]))  dwValue = 8;
					else if ((iResult >= m_iWeaponDrop[7]) && (iResult < m_iWeaponDrop[8]))  dwValue = 9; 
					else if ((iResult >= m_iWeaponDrop[8]) && (iResult < m_iWeaponDrop[9]))  dwValue = 10;
					else if ((iResult >= m_iWeaponDrop[9]) && (iResult < m_iWeaponDrop[10]))  dwValue = 11;
					else if ((iResult >= m_iWeaponDrop[10]) && (iResult < m_iWeaponDrop[11]))  dwValue = 12;
					else if ((iResult >= m_iWeaponDrop[12]) && (iResult <= m_iWeaponDrop[12]))  dwValue = 13;
					else dwValue = 1;

					if ((iGenLevel <= 2) && (dwValue > 7)) dwValue = 7;

					pItem->m_dwAttribute = NULL;
					dwType = dwType << 20;
					dwValue = dwValue << 16;
					pItem->m_dwAttribute = pItem->m_dwAttribute | dwType | dwValue;

					if (iDice(1, 10000) <= m_iRareDropRate) {
						iResult = iDice(1, m_iMaxStatedWeapon);
						if (iResult <= m_iStatedWeaponDrop[0])      dwType = ITEMSTAT2_HITPROB;
						else if (iResult <= m_iStatedWeaponDrop[1]) dwType = ITEMSTAT2_CAD;
						else if (iResult <= m_iStatedWeaponDrop[2]) dwType = ITEMSTAT2_GOLD;
						else if (iResult <= m_iStatedWeaponDrop[3]) dwType = ITEMSTAT2_EXP;

						iResult = iDice(1, m_iMaxWeaponDrop);
						if ((iResult >= 1) && (iResult < m_iWeaponDrop[0])) dwValue = 1;
						else if ((iResult >= m_iWeaponDrop[0]) && (iResult < m_iWeaponDrop[1]))  dwValue = 2;
						else if ((iResult >= m_iWeaponDrop[1]) && (iResult < m_iWeaponDrop[2]))  dwValue = 3;
						else if ((iResult >= m_iWeaponDrop[2]) && (iResult < m_iWeaponDrop[3]))  dwValue = 4;
						else if ((iResult >= m_iWeaponDrop[3]) && (iResult < m_iWeaponDrop[4]))  dwValue = 5;
						else if ((iResult >= m_iWeaponDrop[4]) && (iResult < m_iWeaponDrop[5]))  dwValue = 6;
						else if ((iResult >= m_iWeaponDrop[5]) && (iResult < m_iWeaponDrop[6]))  dwValue = 7;
						else if ((iResult >= m_iWeaponDrop[6]) && (iResult < m_iWeaponDrop[7]))  dwValue = 8;
						else if ((iResult >= m_iWeaponDrop[7]) && (iResult < m_iWeaponDrop[8]))  dwValue = 9;
						else if ((iResult >= m_iWeaponDrop[8]) && (iResult < m_iWeaponDrop[9]))  dwValue = 10;
						else if ((iResult >= m_iWeaponDrop[9]) && (iResult < m_iWeaponDrop[10]))  dwValue = 11;
						else if ((iResult >= m_iWeaponDrop[10]) && (iResult < m_iWeaponDrop[11]))  dwValue = 12;
						else if ((iResult >= m_iWeaponDrop[12]) && (iResult <= m_iWeaponDrop[12]))  dwValue = 13;
						else dwValue = 1;

						if ((iGenLevel <= 2) && (dwValue > 7)) dwValue = 7;

						switch (dwType) {
						case ITEMSTAT2_HITPROB: // Hit Prob Min +21%
							if (dwValue <= 3) dwValue = 3;
							break;
						case ITEMSTAT2_CAD: // Cad vai de +1 at� +7
							if (dwValue > 7) dwValue = 7;
							break;
						case ITEMSTAT2_EXP: // Exp set +20%
							dwValue = 2;
							break;
						case ITEMSTAT2_GOLD: // Gold Set +50%
							dwValue = 5;
							break;
						}

						dwType = dwType << 12;
						dwValue = dwValue << 8;
						pItem->m_dwAttribute = pItem->m_dwAttribute | dwType | dwValue;
					}
				}
				else if (pItem->m_sItemEffectType == DEF_ITEMEFFECTTYPE_DEFENSE) {

					//  Strong (60%), Light (30%), Mana Conv.(5%), Crit Increased(5%) (Drops prob.)
					iResult = iDice(1, 10000);
					if ((iResult >= 1) && (iResult <= 5999))          dwType = ITEMSTAT_STRONG;
					else if ((iResult >= 6000) && (iResult <= 8999))  dwType = ITEMSTAT_LIGHT;
					else if ((iResult >= 9000) && (iResult <= 9554))  dwType = ITEMSTAT_MANACONV; //dwType = 11;
					else if ((iResult >= 9555) && (iResult <= 10000)) dwType = ITEMSTAT_CRITICAL2; //dwType = 12;

					iResult = iDice(1, m_iMaxArmorDrop);
					if ((iResult >= 1) && (iResult < m_iArmorDrop[0]))           dwValue = 1;
					else if ((iResult >= m_iArmorDrop[0]) && (iResult < m_iArmorDrop[1]))  dwValue = 2; 
					else if ((iResult >= m_iArmorDrop[1]) && (iResult < m_iArmorDrop[2]))  dwValue = 3; 
					else if ((iResult >= m_iArmorDrop[2]) && (iResult < m_iArmorDrop[3]))  dwValue = 4;  
					else if ((iResult >= m_iArmorDrop[3]) && (iResult < m_iArmorDrop[4]))  dwValue = 5;
					else if ((iResult >= m_iArmorDrop[4]) && (iResult < m_iArmorDrop[5]))  dwValue = 6;  
					else if ((iResult >= m_iArmorDrop[5]) && (iResult < m_iArmorDrop[6]))  dwValue = 7; 
					else if ((iResult >= m_iArmorDrop[6]) && (iResult < m_iArmorDrop[7]))  dwValue = 8; 
					else if ((iResult >= m_iArmorDrop[7]) && (iResult < m_iArmorDrop[8]))  dwValue = 9; 
					else if ((iResult >= m_iArmorDrop[8]) && (iResult < m_iArmorDrop[9]))  dwValue = 10;
					else if ((iResult >= m_iArmorDrop[9]) && (iResult < m_iArmorDrop[10]))  dwValue = 11;
					else if ((iResult >= m_iArmorDrop[10]) && (iResult < m_iArmorDrop[11]))  dwValue = 12; 
					else if ((iResult >= m_iArmorDrop[11]) && (iResult <= m_iArmorDrop[12]))  dwValue = 13; 
					else dwValue = 1;

					switch (dwType) {
					case ITEMSTAT_LIGHT: // Min +16%
						if (dwValue <= 4) dwValue = 4;
						break;
					case ITEMSTAT_STRONG: // Min +14%
						if (dwValue <= 2) dwValue = 2;
						break;

					case ITEMSTAT_MANACONV:
					case ITEMSTAT_CRITICAL2:
						dwValue = (dwValue + 1) / 2;
						if (dwValue < 1) dwValue = 1;
						if ((iGenLevel <= 3) && (dwValue > 2)) dwValue = 2;
						break;
					}
					// Max = 7
					if ((iGenLevel <= 2) && (dwValue > 7)) dwValue = 7;

					pItem->m_dwAttribute = NULL;
					dwType = dwType << 20;
					dwValue = dwValue << 16;
					pItem->m_dwAttribute = pItem->m_dwAttribute | dwType | dwValue;

					// 40%
					if (iDice(1, 10000) <= m_iRareDropRate) {

						// Poison R.(1),  Hit Prob(2), DR(3), HP(4), SP(5), MP(6),  MR(7),  PA(8), MA(9), CAD(10),  Exp(11), Gold(12)
						iResult = iDice(1, m_iMaxStatedArmor);
						if (iResult <= m_iStatedArmorDrop[0])       dwType = ITEMSTAT2_PSNRES;	// 10
						else if (iResult <= m_iStatedArmorDrop[1])  dwType = ITEMSTAT2_DEF;		// 12
						else if (iResult <= m_iStatedArmorDrop[2])  dwType = ITEMSTAT2_SPREC;	// 16
						else if (iResult <= m_iStatedArmorDrop[3])  dwType = ITEMSTAT2_HPREC;	// 23
						else if (iResult <= m_iStatedArmorDrop[4])  dwType = ITEMSTAT2_MPREC;	// 23 
						else if (iResult <= m_iStatedArmorDrop[5])  dwType = ITEMSTAT2_MR;		// 12
						else if (iResult <= m_iStatedArmorDrop[6])  dwType = ITEMSTAT2_PA;		// 3
						else if (iResult <= m_iStatedArmorDrop[7]) dwType = ITEMSTAT2_MA;		// 1

						//Magn0S:: Added to ability some stats drops
						if ((m_bNullDrop[DROP_MA] == false) && (dwType == ITEMSTAT2_MA))
							dwType = ITEMSTAT2_NONE;

						if ((m_bNullDrop[DROP_PA] == false) && (dwType == ITEMSTAT2_PA))
							dwType = ITEMSTAT2_NONE;

						iResult = iDice(1, m_iMaxArmorDrop);
						if ((iResult >= 1) && (iResult < m_iArmorDrop[0]))           dwValue = 1;
						else if ((iResult >= m_iArmorDrop[0]) && (iResult < m_iArmorDrop[1]))  dwValue = 2;
						else if ((iResult >= m_iArmorDrop[1]) && (iResult < m_iArmorDrop[2]))  dwValue = 3;
						else if ((iResult >= m_iArmorDrop[2]) && (iResult < m_iArmorDrop[3]))  dwValue = 4;
						else if ((iResult >= m_iArmorDrop[3]) && (iResult < m_iArmorDrop[4]))  dwValue = 5;
						else if ((iResult >= m_iArmorDrop[4]) && (iResult < m_iArmorDrop[5]))  dwValue = 6;
						else if ((iResult >= m_iArmorDrop[5]) && (iResult < m_iArmorDrop[6]))  dwValue = 7;
						else if ((iResult >= m_iArmorDrop[6]) && (iResult < m_iArmorDrop[7]))  dwValue = 8; 
						else if ((iResult >= m_iArmorDrop[7]) && (iResult < m_iArmorDrop[8]))  dwValue = 9;
						else if ((iResult >= m_iArmorDrop[8]) && (iResult < m_iArmorDrop[9]))  dwValue = 10;
						else if ((iResult >= m_iArmorDrop[9]) && (iResult < m_iArmorDrop[10]))  dwValue = 11;
						else if ((iResult >= m_iArmorDrop[10]) && (iResult < m_iArmorDrop[11]))  dwValue = 12;
						else if ((iResult >= m_iArmorDrop[11]) && (iResult <= m_iArmorDrop[12]))  dwValue = 13;
						else dwValue = 1;

						switch (dwType) {
						case 1: // Poison min +21%
						case 3: // DR Min +21%
						case 7: // MR Min +21%
						case 8: // PA Min +9%
							if (dwValue <= 3) dwValue = 3;
							break;
						case 9: // MA Min +9%
							if (dwValue <= 3) dwValue = 3;
							//Magn0S:: Add Elements
							iResult = iDice(1, 10200);
							if ((iResult >= 1) && (iResult < 2750)) sElement = 1;  //Earth 27%
							else if ((iResult >= 2750) && (iResult < 5500)) sElement = 2;  //Light 27%
							else if ((iResult >= 5500) && (iResult < 6500)) sElement = 3;  //Fire 10%
							else if ((iResult >= 6500) && (iResult < 9250)) sElement = 4;  //Water 27%
							else if ((iResult >= 9250) && (iResult < 9625)) sElement = 5;  //Holy 04%
							else if ((iResult >= 9625) && (iResult < 10000)) sElement = 6;  //Unnholy 04%
							else if ((iResult >= 10000) && (iResult < 10200)) sElement = 7;  //Untunned 02%
							else sElement = 0;

							pItem->m_sNewEffect1 = sElement;
							break;
							
						}
						// Max = 7
						if ((iGenLevel <= 2) && (dwValue > 7)) dwValue = 7;

						dwType = dwType << 12;
						dwValue = dwValue << 8;
						pItem->m_dwAttribute = pItem->m_dwAttribute | dwType | dwValue;
					}
				}

				_AdjustRareItemValue(pItem);
			}
		}

		pItem->m_sTouchEffectType = DEF_ITET_ID;
		pItem->m_sTouchEffectValue1 = iDice(1, 100000);
		pItem->m_sTouchEffectValue2 = iDice(1, 100000);

		SYSTEMTIME SysTime;
		char cTemp[256];
		GetLocalTime(&SysTime);
		ZeroMemory(cTemp, sizeof(cTemp));
		wsprintf(cTemp, "%d%2d", (short)SysTime.wMonth, (short)SysTime.wDay);
		pItem->m_sTouchEffectValue3 = atoi(cTemp);

		m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->bSetItem(m_pNpcList[iNpcH]->m_sX,
			m_pNpcList[iNpcH]->m_sY,
			pItem);

		/*SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pNpcList[iNpcH]->m_cMapIndex,
			m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY,
			pItem->m_sSprite, pItem->m_sSpriteFrame, pItem->m_cItemColor);*/ //v1.4 color

		SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pNpcList[iNpcH]->m_cMapIndex,
			m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY,
			pItem->m_sIDnum, pItem->m_sSpriteFrame, pItem->m_cItemColor, pItem->m_dwAttribute);

		_bItemLog(DEF_ITEMLOG_NEWGENDROP, NULL, NULL, pItem);
	}
}

// October 19,2004 - 3.51 translated
void CGame::NpcBehavior_Dead(int iNpcH)
{
	DWORD dwTime;

	if (m_pNpcList[iNpcH] == NULL) return;

	dwTime = timeGetTime();
	m_pNpcList[iNpcH]->m_sBehaviorTurnCount++;
	if (m_pNpcList[iNpcH]->m_sBehaviorTurnCount > 5) {
		m_pNpcList[iNpcH]->m_sBehaviorTurnCount = 0;
	}

	if ((dwTime - m_pNpcList[iNpcH]->m_dwDeadTime) > m_pNpcList[iNpcH]->m_dwRegenTime)
		DeleteNpc(iNpcH);
}


BOOL CGame::_bDecodeNpcItemConfigFileContents(char* cFn)
{
	FILE* pFile;
	HANDLE hFile;
	DWORD  dwFileSize;
	char* cp, * token, cGSMode[16] = "";
	char seps[] = "= \t\n";
	class CStrTok* pStrTok;
	char cReadModeA = 0;
	char cReadModeB = 0;
	int  iNpcConfigListIndex = 0, k = 0;
	class CNpcItem* pTempNpcItem = NULL;

	hFile = CreateFile(cFn, GENERIC_READ, NULL, NULL, OPEN_EXISTING, NULL, NULL);
	dwFileSize = GetFileSize(hFile, NULL);
	if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);

	pFile = fopen(cFn, "rt");
	if (pFile != NULL) {

		PutLogList("(!) NpcItemConfig file contents received. Now decoding...");
		cp = new char[dwFileSize + 2];
		ZeroMemory(cp, dwFileSize + 2);
		fread(cp, dwFileSize, 1, pFile);

		pStrTok = new class CStrTok(cp, seps);
		token = pStrTok->pGet();
		while (token != NULL) {
			if (cReadModeA != 0) {
				switch (cReadModeA) {
				case 1:

					switch (cReadModeB) {

					case 1:
						// NPC �̸�
						if (strlen(token) > 20) {
							PutLogList("(!!!) CRITICAL ERROR! NPCITEM configuration file error - Too long Npc name.");
							delete[] cp;
							delete pStrTok;
							return FALSE;
						}
						for (iNpcConfigListIndex = 0; iNpcConfigListIndex < DEF_MAXNPCTYPES; iNpcConfigListIndex++)
							if (m_pNpcConfigList[iNpcConfigListIndex] != NULL) {
								if (strcmp(m_pNpcConfigList[iNpcConfigListIndex]->m_cNpcName, token) == 0) {
									ZeroMemory(G_cTxt, sizeof(G_cTxt));
									wsprintf(G_cTxt, "%s... Ok!", token);
									PutLogList(G_cTxt);
									break;
								}
							}
						if (iNpcConfigListIndex == DEF_MAXNPCTYPES) {
							PutLogList("(!!!) CRITICAL ERROR! NPCITEM configuration file error - No exist Npc Name");
							delete[] cp;
							delete pStrTok;
							return FALSE;
						}
						cReadModeB = 2;
						break;

					case 2:
						if (strlen(token) > 2) {
							PutLogList("(!!!) CRITICAL ERROR! NPCITEM configuration file error - Type Error.");
							delete[] cp;
							delete pStrTok;
							return FALSE;
						}
						m_pNpcConfigList[iNpcConfigListIndex]->m_iNpcItemType = atoi(token);
						cReadModeB = 3;
						break;

					case 3:
						if (strlen(token) > 20) {
							PutLogList("(!!!) CRITICAL ERROR! NPCITEM configuration file error - Too long Item name.");
							delete[] cp;
							delete pStrTok;
							return FALSE;
						}
						if (pTempNpcItem == NULL)
							pTempNpcItem = new class CNpcItem();

						if (memcmp(token, "[ENDITEM]", 9) == 0) {
							cReadModeA = 0;
							cReadModeB = 0;

							if (pTempNpcItem != NULL) {
								delete pTempNpcItem;
								pTempNpcItem = NULL;
							}
							break;
						}

						strcpy(pTempNpcItem->m_cName, token);
						if (!m_bReceivedItemList)	// ITEM List�� ���� �ʾҴ�.
						{
							PutLogList("(!!!) CRITICAL ERROR! NPCITEM configuration error - Before Item List receiving.");
							delete[] cp;
							delete pStrTok;
							return FALSE;
						}

						for (k = 0; k < DEF_MAXITEMTYPES; k++)
						{
							if (m_pItemConfigList[k] == NULL)
								continue;

							if (strcmp(token, m_pItemConfigList[k]->m_cName) == 0) {
								pTempNpcItem->m_sItemID = m_pItemConfigList[k]->m_sIDnum;
								break;
							}
						}

						if (k == DEF_MAXITEMTYPES) {
							PutLogList("(!!!) CRITICAL ERROR! NPCITEM configuration error - Do Not exist in ITEM LIST");
							delete[] cp;
							delete pStrTok;
							return FALSE;
						}

						cReadModeB = 4;
						break;

					case 4:
						// ù��° Ȯ��
						if (_bGetIsStringIsNumber(token) == FALSE) {
							PutLogList("(!!!) CRITICAL ERROR! NPCITEM configuration file error - Wrong Data format.");
							delete[] cp;
							delete pStrTok;
							return FALSE;
						}
						pTempNpcItem->m_sFirstProbability = atoi(token);

						if (pTempNpcItem->m_sFirstProbability <= 0) {
							PutLogList("(!!!) CRITICAL ERROR! NPCITEM configuration file error - First probability have wrong value");
							delete[] cp;
							delete pStrTok;
							return FALSE;
						}

						// ù��° Ȯ���� ���� Ÿ�ٰ��� ���Ƿ� �ش�. 

						cReadModeB = 5;
						break;

					case 5:
						// �ι�° Ȯ��
						if (_bGetIsStringIsNumber(token) == FALSE) {
							PutLogList("(!!!) CRITICAL ERROR! NPCITEM configuration file error - Wrong Data format.");
							delete[] cp;
							delete pStrTok;
							return FALSE;
						}

						pTempNpcItem->m_sSecondProbability = atoi(token);

						if (pTempNpcItem->m_sSecondProbability <= 0) {
							PutLogList("(!!!) CRITICAL ERROR! NPCITEM configuration file error - Second probability have wrong value");
							delete[] cp;
							delete pStrTok;
							return FALSE;
						}

						// �ι�° Ȯ���� ���� Ÿ�ٰ��� ���Ƿ� �ش�. 

						cReadModeB = 3;

						// vector�� ���� ���� �о� �帰 ���� �ִ´�. 
						m_pNpcConfigList[iNpcConfigListIndex]->m_vNpcItem.push_back(*pTempNpcItem);

						// 2002-09-17 #1 NPCITEM Type 2�� ���
						/*if (m_pNpcConfigList[iNpcConfigListIndex]->m_iNpcItemMax < pTempNpcItem->m_sSecondProbability)
							m_pNpcConfigList[iNpcConfigListIndex]->m_iNpcItemMax = pTempNpcItem->m_sSecondProbability;*/

						break;

					} // switch #2
					break;

				default:
					break;

				} // switch #1
			} // if
			else {

				if (memcmp(token, "NpcItem", 7) == 0) {
					cReadModeA = 1;
					cReadModeB = 1;

				}
			}
			token = pStrTok->pGet();
		}

		delete pStrTok;
		delete[] cp;
	}
	if ((cReadModeA != 0) || (cReadModeB != 0)) {
		PutLogList("(!!!) CRITICAL ERROR! NPCITEM configuration file contents error!");
		return FALSE;
	}
	ZeroMemory(G_cTxt, sizeof(G_cTxt));
	wsprintf(G_cTxt, "(!) NPC-Item(Total:%d) configuration - success!", iNpcConfigListIndex);
	PutLogList(G_cTxt);
	return TRUE;

}
BOOL CGame::bGetItemNameWhenDeleteNpc(int& iItemID, short sNpcType)
{
	if (m_bNpcItemConfig == TRUE) {

		class    CNpcItem	CTempNpcItem;
		int    iResult;
		int    iNpcIndex;
		int    iNumNpcitem;
		int    iIndex;
		int    iDiceValue;

		int iProb;
		BOOL	bFirstDice = FALSE, bSecondDice = FALSE;

		for (iNpcIndex = 0; iNpcIndex < DEF_MAXNPCTYPES; iNpcIndex++) {
			if (m_pNpcConfigList[iNpcIndex] != NULL) {
				if (m_pNpcConfigList[iNpcIndex]->m_sType == sNpcType) break;
			}
		}

		if (iNpcIndex == DEF_MAXNPCTYPES) return FALSE;

		if (m_pNpcConfigList[iNpcIndex]->m_vNpcItem.size() <= 0)  return FALSE;

		switch (m_pNpcConfigList[iNpcIndex]->m_iNpcItemType) {
		case 1:
			// ���� ������ ���� ���� ������ �� �ϳ��� ������.
			iResult = iDice(1, m_pNpcConfigList[iNpcIndex]->m_vNpcItem.size()) - 1;

			CTempNpcItem = m_pNpcConfigList[iNpcIndex]->m_vNpcItem.at(iResult);

			// centu - fixed que lea probabilidades
			if (iDice(1, 1000) == CTempNpcItem.m_sFirstProbability) bFirstDice = TRUE;
			if (iDice(1, 1000) == CTempNpcItem.m_sSecondProbability) bSecondDice = TRUE;
		
			if (bFirstDice && bSecondDice) {
				iItemID = CTempNpcItem.m_sItemID;

				wsprintf(G_cTxt, "NpcType 1 (%d) size(%d) %s(%d) (%d)", sNpcType, m_pNpcConfigList[iNpcIndex]->m_vNpcItem.size(), CTempNpcItem.m_cName, CTempNpcItem.m_sItemID, iItemID);
				PutLogList(G_cTxt);

			}
			else {
				switch (iDice(1, 2))
				{
					case 1: iItemID = 90; break;
					case 2: iItemID = 77; break;
				}
			}
			break;

		case 2:
			
			break;

		} // switch

		if (iItemID <= 0)
			return FALSE;

		return TRUE;

	}
	else {
		int iResult;
		// NPC를 삭제할때 특수 아이템을 발생시킬 것인지의 여부를 계산한다. 

		switch (sNpcType) {
		case 49: // Hellclaw 
			iResult = iDice(1, 20100);
			if ((iResult >= 1) && (iResult <= 5000)) iItemID = 308;		    // MagicNecklace(MS10)
			else if ((iResult > 5000) && (iResult <= 10000)) iItemID = 259;	// MagicWand(M.Shield)
			else if ((iResult > 10000) && (iResult <= 13000))  iItemID = 337;  // RubyRing
			else if ((iResult > 13000) && (iResult <= 15000))  iItemID = 335;  // EmeraldRing
			else if ((iResult > 15000) && (iResult <= 17500))  iItemID = 300;  // MagicNecklace(RM10)
			else if ((iResult > 17500) && (iResult <= 18750))  iItemID = 311;  // MagicNecklace(DF+10)
			else if ((iResult > 18750) && (iResult <= 19000))  iItemID = 305;  // MagicNecklace(DM+1)
			else if ((iResult > 19000) && (iResult <= 19700))  iItemID = 634;  // RingofWizard
			else if ((iResult > 19700) && (iResult <= 19844))  iItemID = 635;  // RingofMage
			else if ((iResult > 19844) && (iResult <= 19922))  iItemID = 643;  // KnecklaceOfIceEle	
			else if ((iResult > 19922) && (iResult <= 19961))  iItemID = 640;  // KnecklaceOfSufferent
			else if ((iResult > 19961) && (iResult <= 19981))  iItemID = 637;  // KnecklaceOfLightPro
			else if ((iResult > 19981) && (iResult <= 19991))  iItemID = 620;  // MerienShield	
			else if ((iResult > 19991) && (iResult <= 19996))  iItemID = 644;  // KnecklaceOfAirEle	
			else if ((iResult > 19996) && (iResult <= 19999))  iItemID = 614;  // SwordofIceElemental	
			else if ((iResult > 19999) && (iResult <= 20000))  iItemID = 636;	// RingofGrandMage
			else if ((iResult > 20001) && (iResult <= 20100))  iItemID = 280; // Pandora's Box
			return TRUE;

		case 50: // Tigerworm
			iResult = iDice(1, 10050);
			if ((iResult >= 1) && (iResult <= 4999)) {
				if (iDice(1, 2) == 1)
					iItemID = 311;  // MagicNecklace(DF+10)
				else iItemID = 305;  // MagicNecklace(DM+1)
			}
			else if ((iResult > 5000) && (iResult <= 7499))  iItemID = 614;  // SwordofIceElemental	
			else if ((iResult > 7500) && (iResult <= 8749))  iItemID = 290;  // Flameberge+3(LLF)
			else if ((iResult > 8750) && (iResult <= 9374))  iItemID = 633;  // RingofDemonpower
			else if ((iResult > 9375) && (iResult <= 9687))  iItemID = 492;  // BloodRapier		
			else if ((iResult > 9688) && (iResult <= 9843))  iItemID = 490;  // BloodSword		
			else if ((iResult > 9844) && (iResult <= 9921))  iItemID = 491;  // BloodAxe		
			else if ((iResult > 9922) && (iResult <= 9960))  iItemID = 291;  // MagicWand(MS30-LLF)	
			else if ((iResult > 9961) && (iResult <= 9980))  iItemID = 630;  // RingoftheXelima	
			else if ((iResult > 9981) && (iResult <= 9990))  iItemID = 612;  // XelimaRapier	
			else if ((iResult > 9991) && (iResult <= 9996))  iItemID = 610;  // XelimaBlade	
			else if ((iResult > 9996) && (iResult <= 9998))  iItemID = 611;  // XelimaAxe	
			else if ((iResult > 9999) && (iResult <= 10000)) iItemID = 631;  // RingoftheAbaddon
			else if ((iResult > 10001) && (iResult <= 10050)) iItemID = 631;  // Pandora's Box
			return TRUE;

		default:
			break;
		}

		if (iDice(1, 45) == 13) {
			switch (sNpcType) {
			case 11: if (iDice(1, 550) != 11) return FALSE; 	  // Skeleton   2 * 100	
			case 12: if (iDice(1, 400) != 11) return FALSE; 	  // Stone-Golem 2 * 100	
			case 13: if (iDice(1, 100) != 11) return FALSE; 	  // Cyclops  6 * 100	
			case 14: if (iDice(1, 700) != 11) return FALSE; 	  // Orc 4 * 100	
			case 17: if (iDice(1, 600) != 11) return FALSE; 	  // Scorpoin 5 * 100	
			case 18: if (iDice(1, 850) != 11) return FALSE; 	  // Zombie 1 * 100	
			case 22: if (iDice(1, 600) != 11) return FALSE; 	  // Amphis 5 * 100	
			case 23: if (iDice(1, 400) != 11) return FALSE; 	  // Clay-Golem 2 * 100	
			case 27: if (iDice(1, 100) != 11) return FALSE; 	  // Hellhound 7 * 100	
			case 28: if (iDice(1, 100) != 11) return FALSE; 	  // Troll 5 * 100	
			case 29: if (iDice(1, 150) != 11) return FALSE; 	  // Orge  7 * 100	
			case 30: if (iDice(1, 120) != 11) return FALSE; 	  // Liche 1 * 100   
															  // Demon 5 * 100	
			case 32: if (iDice(1, 200) != 11) return FALSE; 	  // Unicorn 5 * 100	
			case 33: if (iDice(1, 300) != 11) return FALSE; 	  // WereWolf 7 * 100
			case 48: if (iDice(1, 100) != 11) return FALSE; 	  // Stalker 
			case 52: if (iDice(1, 300) != 11) return FALSE;    // Gagoyle
			case 53: if (iDice(1, 500) != 11) return FALSE; 	  // Beholder
			case 54: if (iDice(1, 200) != 11) return FALSE; 	  // Dark-Elf
			case 57: if (iDice(1, 400) != 11) return FALSE; 	  // Giant-Frog
			case 63: if (iDice(1, 300) != 11) return FALSE; 	  // Frost
			case 79: if (iDice(1, 200) != 11) return FALSE; 	  // Nizie
			case 70: if (iDice(1, 200) != 11) return FALSE; 	  // Barlog
			case 71: if (iDice(1, 200) != 11) return FALSE; 	  // Centaurus
			default: break;
			}
		}
		else return FALSE;


		switch (sNpcType) {
		case 11: // Skeleton
		case 17: // Scorpoin
		case 14: // Orc
		case 28: // Troll
		case 57: // Giant-Frog
			switch (iDice(1, 7)) {
			case 1: iItemID = 334; break; // LuckyGoldRing
			case 2: iItemID = 336; break; // SapphireRing
			case 3: if (iDice(1, 15) == 3) iItemID = 335; break; // EmeraldRing
			case 4: iItemID = 337; break; // RubyRing
			case 5: iItemID = 333; break; // PlatinumRing
			case 6: if (iDice(1, 15) == 3) iItemID = 634; break; // RingofWizard
			case 7: if (iDice(1, 25) == 3) iItemID = 635; break; // RingofMage
			}
			break;

		case 13: // Cyclops
		case 27: // Hellhound
		case 29: // Orge
			switch (iDice(1, 7)) {
			case 1: iItemID = 311; break; // MagicNecklace(DF+10)
			case 2: if (iDice(1, 20) == 13) iItemID = 308; break; // MagicNecklace(MS10)
			case 3: if (iDice(1, 10) == 13) iItemID = 305; break; // MagicNecklace(DM+1)
			case 4: iItemID = 300; break; // MagicNecklace(RM10)
			case 5: if (iDice(1, 30) == 13) iItemID = 632; break; // RingofOgrepower
			case 6: if (iDice(1, 30) == 13) iItemID = 637; break; // KnecklaceOfLightPro
			case 7: if (iDice(1, 30) == 13) iItemID = 638; break; // KnecklaceOfFirePro
			}
			break;

		case 18: // Zombie
		case 22: // Amphis
			switch (iDice(1, 4)) {
			case 1: if (iDice(1, 75) == 13) iItemID = 613; break; // SwordofMedusa
			case 2: if (iDice(1, 20) == 13) iItemID = 639; break; // KnecklaceOfPoisonPro
			case 3: if (iDice(1, 40) == 13) iItemID = 641; break; // KnecklaceOfMedusa
			case 4: if (iDice(1, 30) == 13) iItemID = 640; break; // KnecklaceOfSufferent
			}
			break;

		case 12: // Stone-Golem
			switch (iDice(1, 5)) {
			case 1: if (iDice(1, 40) == 13) iItemID = 620; break; // MerienShield
			case 2: if (iDice(1, 40) == 13) iItemID = 621; break; // MerienPlateMail(M)
			case 3: if (iDice(1, 40) == 13) iItemID = 622; break; // MerienPlateMail(W)
			case 4: if (iDice(1, 20) == 11) iItemID = 644; break; // KnecklaceOfAirEle
			case 5: if (iDice(1, 20) == 11) iItemID = 647; break; // KnecklaceOfStoneGolem
			}
			break;

		case 23: // Clay-Golem
			switch (iDice(1, 4)) {
			case 1: if (iDice(1, 40) == 13) iItemID = 620; break; // MerienShield	
			case 2: if (iDice(1, 40) == 13) iItemID = 621; break; // MerienPlateMail(M)
			case 3: if (iDice(1, 40) == 13) iItemID = 622; break; // MerienPlateMail(W)
			case 4: if (iDice(1, 20) == 11) iItemID = 644; break; // KnecklaceOfAirEle
			}
			break;

		case 32: // Unicorn
			switch (iDice(1, 6)) {
			case 1: if (iDice(1, 40) == 13) iItemID = 620; break; // MerienShield	
			case 2: if (iDice(1, 40) == 13) iItemID = 621; break; // MerienPlateMail(M)
			case 3: if (iDice(1, 40) == 13) iItemID = 622; break; // MerienPlateMail(W)
			case 4: if (iDice(1, 20) == 11) iItemID = 644; break; // KnecklaceOfAirEle
			case 5: if (iDice(1, 20) == 11) iItemID = 848; break; // Lighting Blade
			case 6: iItemID = 280; break; // Pandora's Box
			}
			break;

		case 33: // WereWolf
		case 48: // Stalker
			switch (iDice(1, 2)) {
			case 1: if (iDice(1, 30) == 3) iItemID = 290; break; // Flameberge+3(LLF)
			case 2: iItemID = 292; break; // GoldenAxe(LLF)
			}
			break;

		case 30: // Liche
			switch (iDice(1, 8)) {
			case 1: if (iDice(1, 10) == 3) iItemID = 380; break; // IceStormManual
			case 2: iItemID = 259; break; // MagicWand(M.Shield)
			case 3: if (iDice(1, 30) == 3) iItemID = 291; break; // MagicWand(MS30-LLF)
			case 4: if (iDice(1, 10) == 3) iItemID = 614; break; // SwordofIceElemental	
			case 5: if (iDice(1, 10) == 3) iItemID = 642; break; // KnecklaceOfIcePro
			case 6: if (iDice(1, 15) == 3) iItemID = 643; break; // KnecklaceOfIceEle	
			case 7: if (iDice(1, 30) == 3) iItemID = 636; break; // RingofGrandMage
			case 8: if (iDice(1, 30) == 3) iItemID = 734; break; // RingOfArcmage
			}
			break;

		case 31: // Demon 
			switch (iDice(1, 9)) {
			case 1: if (iDice(1, 30) == 3) iItemID = 382; break; // BloodyShockW.Manual
			case 2: iItemID = 491; break; // BloodAxe
			case 3: if (iDice(1, 10) == 3) iItemID = 490; break; // BloodSword
			case 4: iItemID = 492; break; // BloodRapier
			case 5: if (iDice(1, 10) == 3) iItemID = 381; break; // MassFireStrikeManual
			case 6: if (iDice(1, 30) == 3) iItemID = 633; break; // RingofDemonpower
			case 7: if (iDice(1, 10) == 3) iItemID = 645; break; // KnecklaceOfEfreet
			case 8: if (iDice(1, 20) == 3) iItemID = 616; break; // DemonSlayer
			case 9: iItemID = 280; break; // Pandora's Box
			}
			break;

		case 52: // Gagoyle
			switch (iDice(1, 13)) {
			case 1: if (iDice(1, 30) == 3) iItemID = 382; break; // BloodyShockW.Manual	
			case 2: if (iDice(1, 20) == 3) iItemID = 610; break; // XelimaBlade	
			case 3: if (iDice(1, 20) == 3) iItemID = 611; break; // XelimaAxe	
			case 4: if (iDice(1, 20) == 3) iItemID = 612; break; // XelimaRapier
			case 5: if (iDice(1, 10) == 3) iItemID = 381; break; // MassFireStrikeManual
			case 6: break; // RingofDemonpower
			case 7: if (iDice(1, 10) == 3) iItemID = 645; break; // KnecklaceOfEfreet
			case 8: if (iDice(1, 40) == 3) iItemID = 630; break; // RingoftheXelima	
			case 9: break; // RingoftheAbaddon
			case 10: break; // RingOfDragonpower
			case 11: if (iDice(1, 40) == 3) iItemID = 20; break; // Excalibur
			case 12: break; // The_Devastator
			case 13: iItemID = 280; break; // Pandora's Box
			}
			break;

		case 53: // Beholder
			if (iDice(1, 20) == 11) iItemID = 646;  // KnecklaceOfBeholder
			break;


		case 54: // Dark-Elf
			if (iDice(1, 20) == 11) iItemID = 618;  // DarkElfBow
			break;


		case 63: // Frost
			if (iDice(1, 40) == 11) iItemID = 845;  // StormBringer
			break;


		case 79: // Nizie
			if (iDice(1, 20) == 11) iItemID = 845;  // StormBringer	
			break;


		case 70: // Barlog
			if (iDice(1, 40) == 11) iItemID = 846;  // The_Devastator
			break;


		case 71: // Centaurus
			if (iDice(1, 20) == 11) iItemID = 848;  // Lighting Blade
			break;


		}

		if (iItemID == 0)
			return FALSE;
		else return TRUE;
	}
}

void CGame::RemoveEventNpc(int iNpcH)
{
	if (m_pNpcList[iNpcH] == NULL) return;
	if (m_pNpcList[iNpcH]->m_bIsKilled == TRUE) return;

	m_pNpcList[iNpcH]->m_bIsKilled = TRUE;
	m_pNpcList[iNpcH]->m_iHP = 0;
	m_pNpcList[iNpcH]->m_iLastDamage = 0;
	m_pNpcList[iNpcH]->m_dwRegenTime = 0;
	m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_iTotalAliveObject--;

	ReleaseFollowMode(iNpcH);
	m_pNpcList[iNpcH]->m_iTargetIndex = 0;
	m_pNpcList[iNpcH]->m_cTargetType = 0;

	SendEventToNearClient_TypeA(iNpcH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTDYING, NULL, 1, NULL);
	if (m_pNpcList[iNpcH]->m_sAreaSize == 0) {
		m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->ClearOwner(10, iNpcH, DEF_OWNERTYPE_NPC, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY);
	}
	else {
		m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->ClearBigOwner(iNpcH, DEF_OWNERTYPE_NPC, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY, m_pNpcList[iNpcH]->m_sAreaSize);
	}
	m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->SetDeadOwner(iNpcH, DEF_OWNERTYPE_NPC, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY);
	m_pNpcList[iNpcH]->m_cBehavior = 4;
	m_pNpcList[iNpcH]->m_sBehaviorTurnCount = 0;
	m_pNpcList[iNpcH]->m_dwDeadTime = timeGetTime();

}

BOOL CGame::_bDecodeNpcConfigFileContents(char* pData, DWORD dwMsgSize)
{
	char* pContents, * token, cTxt[120];
	char seps[] = "= \t\n";
	char cReadModeA = 0;
	char cReadModeB = 0;
	int  iNpcConfigListIndex = 0;
	class CStrTok* pStrTok;

	pContents = new char[dwMsgSize + 1];
	ZeroMemory(pContents, dwMsgSize + 1);
	memcpy(pContents, pData, dwMsgSize);

	pStrTok = new class CStrTok(pContents, seps);
	token = pStrTok->pGet();
	while (token != NULL) {
		if (cReadModeA != 0) {
			switch (cReadModeA) {
			case 1:
				switch (cReadModeB) {
				case 1:
					// NPC
					if (strlen(token) > 20) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Too long Npc name.");
						delete[] pContents;
						delete pStrTok;
						return FALSE;
					}
					ZeroMemory(m_pNpcConfigList[iNpcConfigListIndex]->m_cNpcName, sizeof(m_pNpcConfigList[iNpcConfigListIndex]->m_cNpcName));
					memcpy(m_pNpcConfigList[iNpcConfigListIndex]->m_cNpcName, token, strlen(token));
					cReadModeB = 2;
					break;
				case 2:
					// m_sType
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return FALSE;
					}
					m_pNpcConfigList[iNpcConfigListIndex]->m_sType = atoi(token);
					cReadModeB = 3;
					break;
				case 3:
					// m_iHitDice
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return FALSE;
					}
					m_pNpcConfigList[iNpcConfigListIndex]->m_iHitDice = atoi(token);
					cReadModeB = 4;
					break;
				case 4:
					// m_iDefenseRatio
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return FALSE;
					}
					m_pNpcConfigList[iNpcConfigListIndex]->m_iDefenseRatio = atoi(token);
					cReadModeB = 5;
					break;
				case 5:
					// m_iHitRatio
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return FALSE;
					}
					m_pNpcConfigList[iNpcConfigListIndex]->m_iHitRatio = atoi(token);
					cReadModeB = 6;
					break;
				case 6:
					// m_iMinBravery
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return FALSE;
					}
					m_pNpcConfigList[iNpcConfigListIndex]->m_iMinBravery = atoi(token);
					cReadModeB = 7;
					break;
				case 7:
					// m_iExpDiceMin
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return FALSE;
					}
					m_pNpcConfigList[iNpcConfigListIndex]->m_iExpDiceMin = atoi(token);
					cReadModeB = 8;
					break;

				case 8:
					// m_iExpDiceMax
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return FALSE;
					}
					m_pNpcConfigList[iNpcConfigListIndex]->m_iExpDiceMax = atoi(token);
					cReadModeB = 9;
					break;

				case 9:
					// m_iGoldDiceMin
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return FALSE;
					}
					m_pNpcConfigList[iNpcConfigListIndex]->m_iGoldDiceMin = atoi(token);
					cReadModeB = 10;
					break;

				case 10:
					// m_iGoldDiceMax
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return FALSE;
					}
					m_pNpcConfigList[iNpcConfigListIndex]->m_iGoldDiceMax = atoi(token);
					cReadModeB = 11;
					break;

				case 11:
					// m_cAttackDiceThrow
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return FALSE;
					}
					m_pNpcConfigList[iNpcConfigListIndex]->m_cAttackDiceThrow = atoi(token);
					cReadModeB = 12;
					break;

				case 12:
					// m_cAttackDiceRange
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return FALSE;
					}
					m_pNpcConfigList[iNpcConfigListIndex]->m_cAttackDiceRange = atoi(token);
					cReadModeB = 13;
					break;

				case 13:
					// m_cSize
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return FALSE;
					}
					m_pNpcConfigList[iNpcConfigListIndex]->m_cSize = atoi(token);
					cReadModeB = 14;
					break;

				case 14:
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return FALSE;
					}
					m_pNpcConfigList[iNpcConfigListIndex]->m_cSide = atoi(token);
					cReadModeB = 15;
					break;

				case 15:
					// ActionLimit 
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return FALSE;
					}
					m_pNpcConfigList[iNpcConfigListIndex]->m_cActionLimit = atoi(token);
					cReadModeB = 16;
					break;

				case 16:
					// Action Time
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return FALSE;
					}
					m_pNpcConfigList[iNpcConfigListIndex]->m_dwActionTime = atoi(token);
					cReadModeB = 17;
					break;

				case 17:
					// ResistMagic
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return FALSE;
					}
					m_pNpcConfigList[iNpcConfigListIndex]->m_cResistMagic = atoi(token);
					cReadModeB = 18;
					break;

				case 18:
					// cMagicLevel
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return FALSE;
					}
					m_pNpcConfigList[iNpcConfigListIndex]->m_cMagicLevel = atoi(token);
					cReadModeB = 19;
					break;

				case 19:
					// cGenDayWeekLimit  // Ư�� ���Ͽ��� �����Ǵ� ���Ϳ��� 
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return FALSE;
					}
					m_pNpcConfigList[iNpcConfigListIndex]->m_cDayOfWeekLimit = atoi(token);
					cReadModeB = 20;
					break;

				case 20:
					// cChatMsgPresence
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return FALSE;
					}
					m_pNpcConfigList[iNpcConfigListIndex]->m_cChatMsgPresence = atoi(token);
					cReadModeB = 21;
					break;

				case 21:
					// m_cTargetSearchRange
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return FALSE;
					}
					m_pNpcConfigList[iNpcConfigListIndex]->m_cTargetSearchRange = atoi(token);

					cReadModeB = 22;
					break;

				case 22:
					// Npc �� ���������� �ð�
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return FALSE;
					}
					m_pNpcConfigList[iNpcConfigListIndex]->m_dwRegenTime = atoi(token);

					cReadModeB = 23;
					break;

				case 23:
					// Attribute
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return FALSE;
					}
					m_pNpcConfigList[iNpcConfigListIndex]->m_cAttribute = atoi(token);

					cReadModeB = 24;
					break;

				case 24:
					// Absorb Magic Damage
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return FALSE;
					}
					m_pNpcConfigList[iNpcConfigListIndex]->m_iAbsDamage = atoi(token);

					cReadModeB = 25;
					break;

				case 25:
					// Maximum Mana Point
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return FALSE;
					}
					m_pNpcConfigList[iNpcConfigListIndex]->m_iMaxMana = atoi(token);

					cReadModeB = 26;
					break;

				case 26:
					// MagicHitRatio
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return FALSE;
					}
					m_pNpcConfigList[iNpcConfigListIndex]->m_iMagicHitRatio = atoi(token);

					cReadModeB = 27;
					break;

				case 27:
					// AttackRange
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return FALSE;
					}
					m_pNpcConfigList[iNpcConfigListIndex]->m_iAttackRange = atoi(token);
					cReadModeB = 28;
					break;

				
				case 28:
					// AreaSize
					if (_bGetIsStringIsNumber(token) == FALSE) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return FALSE;
					}
					m_pNpcConfigList[iNpcConfigListIndex]->m_sAreaSize = atoi(token);
					cReadModeA = 0;
					cReadModeB = 0;
					iNpcConfigListIndex++;
					break;

				}
				break;

			default:
				break;
			}
		}
		else {
			if (memcmp(token, "Npc", 3) == 0) {
				cReadModeA = 1;
				cReadModeB = 1;
				m_pNpcConfigList[iNpcConfigListIndex] = new class CNpc(" ");
			}


		}
		token = pStrTok->pGet();
	}

	delete pStrTok;
	delete[] pContents;

	if ((cReadModeA != 0) || (cReadModeB != 0)) {
		PutLogList("(!!!) CRITICAL ERROR! NPC configuration file contents error!");
		return FALSE;
	}

	wsprintf(cTxt, "(!) NPC(Total:%d) configuration - success!", iNpcConfigListIndex);
	PutLogList(cTxt);

	return TRUE;
}

void CGame::MobGenerator()
{
	int i, x, j, iNamingValue, iResult, iTotalMob;
	char cNpcName[21], cName_Master[11], cName_Slave[11], cWaypoint[11];
	char cSA;
	int  pX, pY, iMapLevel, iProbSA, iKindSA, iResultNum, iNpcID;
	BOOL bFirmBerserk, bIsSpecialEvent, bMaster = FALSE;
	bool bIsGuard;

	if (m_bOnExitProcess == TRUE) return;

	for (i = 0; i < DEF_MAXMAPS; i++) {

		if (m_pMapList[i] != NULL) {
			iResultNum = (m_pMapList[i]->m_iMaximumObject - 30);
		}

		if ((m_pMapList[i] != NULL) && (m_pMapList[i]->m_bRandomMobGenerator == TRUE) && (iResultNum > m_pMapList[i]->m_iTotalActiveObject)) {
			if ((m_iMiddlelandMapIndex != -1) && (m_iMiddlelandMapIndex == i) && (m_bIsCrusadeMode == TRUE)) continue;//break;

			iNamingValue = m_pMapList[i]->iGetEmptyNamingValue();
			if (iNamingValue != -1) {
				// Master Mob????????.
				ZeroMemory(cName_Master, sizeof(cName_Master));
				wsprintf(cName_Master, "XX%d", iNamingValue);
				cName_Master[0] = '_';
				cName_Master[1] = i + 65;

				ZeroMemory(cNpcName, sizeof(cNpcName));

				bFirmBerserk = FALSE;
				iResult = iDice(1, 100);
				switch (m_pMapList[i]->m_cRandomMobGeneratorLevel) {

				case 1: // arefarm, elvfarm, aresden, elvine
					if ((iResult >= 1) && (iResult < 20)) {
						iResult = 1; // Slime
					}
					else if ((iResult >= 20) && (iResult < 40)) {
						iResult = 2; // Giant-Ant
					}
					else if ((iResult >= 40) && (iResult < 85)) {
						iResult = 24; // Rabbit
					}
					else if ((iResult >= 85) && (iResult < 95)) {
						iResult = 25; // Cat
					}
					else if ((iResult >= 95) && (iResult <= 100)) {
						iResult = 3; // Orc
					}
					iMapLevel = 1;
					break;

				case 2:
					if ((iResult >= 1) && (iResult < 40)) {
						iResult = 1;
					}
					else if ((iResult >= 40) && (iResult < 80)) {
						iResult = 2;
					}
					else iResult = 10;
					iMapLevel = 1;
					break;

				case 3:     // v1.4334 ????????? ????
					if ((iResult >= 1) && (iResult < 20)) {
						switch (iDice(1, 2)) {
						case 1: iResult = 3;  break;
						case 2: iResult = 4;  break;
						}
					}
					else if ((iResult >= 20) && (iResult < 25)) {
						iResult = 30;
					}
					else if ((iResult >= 25) && (iResult < 50)) {
						switch (iDice(1, 3)) {
						case 1: iResult = 5;  break;
						case 2: iResult = 6;  break;
						case 3:	iResult = 7;  break;
						}
					}
					else if ((iResult >= 50) && (iResult < 75)) {
						switch (iDice(1, 7)) {
						case 1:
						case 2:	iResult = 8;  break;
						case 3:	iResult = 11; break;
						case 4: iResult = 12; break;
						case 5:	iResult = 18; break;
						case 6:	iResult = 26; break;
						case 7: iResult = 28; break;
						}
					}
					else if ((iResult >= 75) && (iResult <= 100)) {
						switch (iDice(1, 5)) {
						case 1:
						case 2: iResult = 9;  break;
						case 3:	iResult = 13; break;
						case 4: iResult = 14; break;
						case 5:	iResult = 27; break;
						}
					}
					iMapLevel = 3;
					break;

				case 4:
					if ((iResult >= 1) && (iResult < 50)) {
						switch (iDice(1, 2)) {
						case 1:	iResult = 2;  break;
						case 2: iResult = 10; break;
						}
					}
					else if ((iResult >= 50) && (iResult < 80)) {
						switch (iDice(1, 2)) {
						case 1: iResult = 8;  break;
						case 2: iResult = 11; break;
						}
					}
					else if ((iResult >= 80) && (iResult <= 100)) {
						switch (iDice(1, 2)) {
						case 1: iResult = 14; break;
						case 2:	iResult = 9;  break;
						}
					}
					iMapLevel = 2;
					break;

				case 5:
					if ((iResult >= 1) && (iResult < 30)) {
						switch (iDice(1, 5)) {
						case 1:
						case 2:
						case 3:
						case 4:
						case 5: iResult = 2;  break;
						}
					}
					else if ((iResult >= 30) && (iResult < 60)) {
						switch (iDice(1, 2)) {
						case 1: iResult = 3;  break;
						case 2: iResult = 4;  break;
						}
					}
					else if ((iResult >= 60) && (iResult < 80)) {
						switch (iDice(1, 2)) {
						case 1: iResult = 5;  break;
						case 2: iResult = 7;  break;
						}
					}
					else if ((iResult >= 80) && (iResult < 95)) {
						switch (iDice(1, 3)) {
						case 1:
						case 2: iResult = 8;  break;
						case 3:	iResult = 11; break;
						}
					}
					else if ((iResult >= 95) && (iResult <= 100)) {
						switch (iDice(1, 3)) {
						case 1: iResult = 11; break;
						case 2: iResult = 14; break;
						case 3: iResult = 9;  break;
						}
					}
					iMapLevel = 3;
					break;

				case 6: // huntzone3, huntzone4
					if ((iResult >= 1) && (iResult < 60)) {
						switch (iDice(1, 4)) {
						case 1: iResult = 5;  break; // Skeleton
						case 2:	iResult = 6;  break; // Orc-Mage
						case 3: iResult = 12; break; // Cyclops
						case 4: iResult = 11; break; // Troll
						}
					}
					else if ((iResult >= 60) && (iResult < 90)) {
						switch (iDice(1, 5)) {
						case 1:
						case 2: iResult = 8;  break; // Stone-Golem
						case 3:	iResult = 11; break; // Troll
						case 4:	iResult = 12; break; // Cyclops 
						case 5:	iResult = 43; break; // Tentocle
						}
					}
					else if ((iResult >= 90) && (iResult <= 100)) {
						switch (iDice(1, 9)) {
						case 1:	iResult = 26; break;
						case 2:	iResult = 9;  break;
						case 3: iResult = 13; break;
						case 4: iResult = 14; break;
						case 5:	iResult = 18; break;
						case 6:	iResult = 28; break;
						case 7: iResult = 27; break;
						case 8: iResult = 29; break;
						}
					}
					iMapLevel = 4;
					break;

				case 7: // areuni, elvuni
					if ((iResult >= 1) && (iResult < 50)) {
						switch (iDice(1, 5)) {
						case 1: iResult = 3;  break; // Orc
						case 2: iResult = 6;  break; // Orc-Mage
						case 3: iResult = 10; break; // Amphis
						case 4: iResult = 3;  break; // Orc
						case 5: iResult = 50; break; // Giant-Tree
						}
					}
					else if ((iResult >= 50) && (iResult < 60)) {
						iResult = 29; // Rudolph
					}
					else if ((iResult >= 60) && (iResult < 85)) {
						switch (iDice(1, 4)) {
						case 1: iResult = 50; break; // Giant-Tree
						case 2:
						case 3: iResult = 6;  break; // Orc-Mage
						case 4: iResult = 12; break; // Troll
						}
					}
					else if ((iResult >= 85) && (iResult <= 100)) {
						switch (iDice(1, 4)) {
						case 1: iResult = 12;  break; // Troll
						case 2:
						case 3:
							if (iDice(1, 100) < 3)
								iResult = 17; // Unicorn
							else iResult = 12; // Troll
							break;
						case 4: iResult = 29;  break; // Cannibal-Plant
						}
					}
					iMapLevel = 4;
					break;

				case 8:
					if ((iResult >= 1) && (iResult < 70)) {
						switch (iDice(1, 2)) {
						case 1:	iResult = 4;  break;
						case 2: iResult = 5;  break;
						}
					}
					else if ((iResult >= 70) && (iResult < 95)) {
						switch (iDice(1, 2)) {
						case 1: iResult = 8;  break;
						case 2: iResult = 11; break;
						}
					}
					else if ((iResult >= 95) && (iResult <= 100)) {
						iResult = 14; break;
					}
					iMapLevel = 4;
					break;

				case 9:
					if ((iResult >= 1) && (iResult < 70)) {
						switch (iDice(1, 2)) {
						case 1:	iResult = 4;  break;
						case 2: iResult = 5;  break;
						}
					}
					else if ((iResult >= 70) && (iResult < 95)) {
						switch (iDice(1, 3)) {
						case 1: iResult = 8;  break;
						case 2: iResult = 9;  break;
						case 3: iResult = 13; break;
						}
					}
					else if ((iResult >= 95) && (iResult <= 100)) {
						switch (iDice(1, 6)) {
						case 1:
						case 2:
						case 3: iResult = 9;  break;
						case 4:
						case 5: iResult = 14; break;
						case 6: iResult = 15; break;
						}
					}

					if ((iDice(1, 3) == 1) && (iResult != 16)) bFirmBerserk = TRUE;
					iMapLevel = 5;
					break;

				case 10:
					if ((iResult >= 1) && (iResult < 70)) {
						switch (iDice(1, 3)) {
						case 1:	iResult = 9; break;
						case 2: iResult = 5; break;
						case 3: iResult = 8; break;
						}
					}
					else if ((iResult >= 70) && (iResult < 95)) {
						switch (iDice(1, 3)) {
						case 1:
						case 2:	iResult = 13; break;
						case 3: iResult = 14; break;
						}
					}
					else if ((iResult >= 95) && (iResult <= 100)) {
						switch (iDice(1, 3)) {
						case 1:
						case 2: iResult = 14; break;
						case 3: iResult = 15; break;
						}
					}
					// Demon?? Berserk ????? ??? ?????
					if ((iDice(1, 3) == 1) && (iResult != 16)) bFirmBerserk = TRUE;
					iMapLevel = 5;
					break;

				case 11:
					if ((iResult >= 1) && (iResult < 30)) {
						switch (iDice(1, 5)) {
						case 1:
						case 2:
						case 3:
						case 4:
						case 5: iResult = 2; break;
						}
					}
					else if ((iResult >= 30) && (iResult < 60)) {
						switch (iDice(1, 2)) {
						case 1: iResult = 3; break;
						case 2: iResult = 4; break;
						}
					}
					else if ((iResult >= 60) && (iResult < 80)) {
						switch (iDice(1, 2)) {
						case 1: iResult = 5; break;
						case 2: iResult = 7; break;
						}
					}
					else if ((iResult >= 80) && (iResult < 95)) {
						switch (iDice(1, 3)) {
						case 1:
						case 2: iResult = 10;  break;
						case 3:	iResult = 11; break;
						}
					}
					else if ((iResult >= 95) && (iResult <= 100)) {
						switch (iDice(1, 3)) {
						case 1: iResult = 11; break;
						case 2: iResult = 7; break;
						case 3: iResult = 8; break;
						}
					}
					iMapLevel = 4;
					break;

				case 12:
					if ((iResult >= 1) && (iResult < 50)) {
						switch (iDice(1, 3)) {
						case 1:	iResult = 1; break;
						case 2: iResult = 2; break;
						case 3: iResult = 10; break;
						}
					}
					else if ((iResult >= 50) && (iResult < 85)) {
						switch (iDice(1, 2)) {
						case 1: iResult = 5; break;
						case 2: iResult = 4; break;
						}
					}
					else if ((iResult >= 85) && (iResult <= 100)) {
						switch (iDice(1, 3)) {
						case 1: iResult = 8; break;
						case 2: iResult = 11; break;
						case 3: iResult = 26; break;
						}
					}
					iMapLevel = 4;
					break;

				case 13:
					if ((iResult >= 1) && (iResult < 15)) {
						iResult = 4;
						bFirmBerserk = TRUE;
						iTotalMob = 4 - (iDice(1, 2) - 1);
						break;
					}
					else if ((iResult >= 15) && (iResult < 40)) {
						iResult = 14;
						bFirmBerserk = TRUE;
						iTotalMob = 4 - (iDice(1, 2) - 1);
						break;
					}
					else if ((iResult >= 40) && (iResult < 60)) {
						iResult = 9;
						bFirmBerserk = TRUE;
						iTotalMob = 4 - (iDice(1, 2) - 1);
						break;
					}
					else if ((iResult >= 60) && (iResult < 75)) {
						iResult = 13;
						bFirmBerserk = TRUE;
						iTotalMob = 4 - (iDice(1, 2) - 1);
						break;
					}
					else if ((iResult >= 75) && (iResult < 95)) {
						iResult = 23;
					}
					else if ((iResult >= 95) && (iResult <= 100)) {
						iResult = 22;
					}
					iMapLevel = 5;
					break;

				case 14: // icebound
					if ((iResult >= 1) && (iResult < 30)) {
						iResult = 23; // Dark-Elf
					}
					else if ((iResult >= 30) && (iResult < 50)) {
						iResult = 31; // Ice-Golem
					}
					else if ((iResult >= 50) && (iResult < 70)) {
						iResult = 22; // Beholder
						bFirmBerserk = TRUE;
						iTotalMob = 4 - (iDice(1, 2) - 1);
					}
					else if ((iResult >= 70) && (iResult < 90)) {
						iResult = 32; // DireBoar
					}
					else if ((iResult >= 90) && (iResult <= 100)) {
						iResult = 33; // Frost
					}
					iMapLevel = 5;
					break;

				case 15:
					if ((iResult >= 1) && (iResult < 35)) {
						iResult = 23;
						bFirmBerserk = TRUE;
					}
					else if ((iResult >= 35) && (iResult < 50)) {
						iResult = 22;
						bFirmBerserk = TRUE;
					}
					else if ((iResult >= 50) && (iResult < 80)) {
						iResult = 15;
					}
					else if ((iResult >= 80) && (iResult <= 100)) {
						iResult = 21;
					}
					iMapLevel = 4;
					break;

				case 16: // 2ndmiddle, huntzone1, huntzone2, 
					if ((iResult >= 1) && (iResult < 40)) {
						switch (iDice(1, 3)) {
						case 1:	iResult = 7;  break; // Scorpion
						case 2: iResult = 2;  break; // Giant-Ant
						case 3: iResult = 10; break; // Amphis
						}
					}
					else if ((iResult >= 40) && (iResult < 50)) {
						iResult = 30; // Rudolph
					}
					else if ((iResult >= 50) && (iResult < 85)) {
						switch (iDice(1, 2)) {
						case 1: iResult = 5;  break; // Skeleton
						case 2: iResult = 4;  break; // Zombie
						}
					}
					else if ((iResult >= 85) && (iResult <= 100)) {
						switch (iDice(1, 3)) {
						case 1: iResult = 8;  break; // Stone-Golem
						case 2: iResult = 11; break; // Clay-Golem
						case 3: iResult = 7;  break; // Scorpion
						}
					}
					iMapLevel = 1;
					break;

				case 17:
					if ((iResult >= 1) && (iResult < 30)) {
						switch (iDice(1, 4)) {
						case 1:	iResult = 22;  break; // Giant-Frog
						case 2: iResult = 8;   break; // Stone-Golem
						case 3: iResult = 24;  break; // Rabbit
						case 4: iResult = 5;   break;
						}
					}
					else if ((iResult >= 30) && (iResult < 40)) {
						iResult = 30;
					}
					else if ((iResult >= 40) && (iResult < 70)) {
						iResult = 32;
					}
					else if ((iResult >= 70) && (iResult < 90)) {
						iResult = 31;
						if (iDice(1, 5) == 1) {
							bFirmBerserk = TRUE;
						}
					}
					else if ((iResult >= 90) && (iResult <= 100)) {
						iResult = 33;
					}
					iMapLevel = 1;
					break;

				case 18: // druncncity
					if ((iResult >= 1) && (iResult < 2)) {
						iResult = 39; // Tentocle
					}
					else if ((iResult >= 2) && (iResult < 12)) {
						iResult = 44; // ClawTurtle
					}
					else if ((iResult >= 12) && (iResult < 50)) {
						iResult = 48; // Nizie
					}
					else if ((iResult >= 50) && (iResult < 80)) {
						iResult = 45; // Giant-Crayfish
					}
					else if ((iResult >= 80) && (iResult < 90)) {
						iResult = 34; // Stalker
					}
					else if ((iResult >= 90) && (iResult <= 100)) {
						iResult = 26; // Giant-Frog
					}
					iMapLevel = 4;
					break;

				case 19:
					if ((iResult >= 1) && (iResult < 15)) {
						iResult = 44;
					}
					else if ((iResult >= 15) && (iResult < 25)) {
						iResult = 46;
					}
					else if ((iResult >= 25) && (iResult < 35)) {
						iResult = 21;
					}
					else if ((iResult >= 35) && (iResult < 60)) {
						iResult = 43;
					}
					else if ((iResult >= 60) && (iResult < 85)) {
						iResult = 23;
					}
					else if ((iResult >= 85) && (iResult <= 100)) {
						iResult = 22;
					}
					iMapLevel = 4;
					break;

				case 20:
					if ((iResult >= 1) && (iResult < 2)) {
						iResult = 41;
					}
					else if ((iResult >= 2) && (iResult < 3)) {
						iResult = 40;
					}
					else if ((iResult >= 3) && (iResult < 8)) {
						iResult = 53;
					}
					else if ((iResult >= 8) && (iResult < 9)) {
						iResult = 39;
					}
					else if ((iResult >= 9) && (iResult < 20)) {
						iResult = 21;
					}
					else if ((iResult >= 20) && (iResult < 35)) {
						iResult = 16;
					}
					else if ((iResult >= 35) && (iResult < 45)) {
						iResult = 44;
					}
					else if ((iResult >= 45) && (iResult < 55)) {
						iResult = 45;
					}
					else if ((iResult >= 55) && (iResult < 75)) {
						iResult = 28;
					}
					else if ((iResult >= 75) && (iResult < 95)) {
						iResult = 43;
					}
					else if ((iResult >= 95) && (iResult < 100)) {
						iResult = 22;
					}
					iMapLevel = 4;
					break;

				case 21:
					if ((iResult >= 1) && (iResult < 94)) {
						iResult = 17; // Unicorn
						bFirmBerserk = TRUE;
					}
					else if ((iResult >= 94) && (iResult < 95)) {
						iResult = 36; // Wyvern
					}
					else if ((iResult >= 95) && (iResult < 96)) {
						iResult = 37; // Fire-Wyvern
					}
					else if ((iResult >= 96) && (iResult < 97)) {
						iResult = 47; // MasterMage-Orc
					}
					else if ((iResult >= 97) && (iResult < 98)) {
						iResult = 35; // Hellclaw
					}
					else if ((iResult >= 98) && (iResult < 99)) {
						iResult = 49; // Tigerworm
					}
					else if ((iResult >= 99) && (iResult <= 100)) {
						iResult = 51; // Abaddon
					}
					iMapLevel = 4;
					break;

				}

				pX = NULL;
				pY = NULL;

				if ((m_bIsSpecialEventTime == TRUE) && (iDice(1, 10) == 3)) bIsSpecialEvent = TRUE;

				if (bIsSpecialEvent == TRUE) {
					switch (m_cSpecialEventType) {
					case 1:
						if (m_pMapList[i]->m_iMaxPx != 0) {
							pX = m_pMapList[i]->m_iMaxPx * 20 + 10;
							pY = m_pMapList[i]->m_iMaxPy * 20 + 10;

							if (pX < 0) pX = 0;
							if (pY < 0) pY = 0;

							if (m_bIsCrusadeMode == TRUE) {
								if (strcmp(m_pMapList[i]->m_cName, "aresden") == 0) {
									switch (iDice(1, 6)) {
									case 1: iResult = 20; break;
									case 2: iResult = 53; break;
									case 3: iResult = 55; break;
									case 4: iResult = 57; break;
									case 5: iResult = 59; break;
									case 6: iResult = 61; break;
									}
								}
								else if (strcmp(m_pMapList[i]->m_cName, "elvine") == 0) {
									switch (iDice(1, 6)) {
									case 1: iResult = 19; break;
									case 2: iResult = 52; break;
									case 3: iResult = 54; break;
									case 4: iResult = 56; break;
									case 5: iResult = 58; break;
									case 6: iResult = 60; break;
									}
								}
							}
							wsprintf(G_cTxt, "(!) Mob-Event Map(%s)[%d (%d,%d)]", m_pMapList[i]->m_cName, iResult, pX, pY);
						}
						break;

					case 2:
						if (iDice(1, 3) == 2) {
							if ((memcmp(m_pMapList[i]->m_cLocationName, "aresden", 7) == 0) ||
								(memcmp(m_pMapList[i]->m_cLocationName, "middled1n", 9) == 0) ||
								(memcmp(m_pMapList[i]->m_cLocationName, "arefarm", 7) == 0) ||
								(memcmp(m_pMapList[i]->m_cLocationName, "elvfarm", 7) == 0) ||
								(memcmp(m_pMapList[i]->m_cLocationName, "elvine", 6) == 0)) {
								if (iDice(1, 30) == 5)
									iResult = 16;
								else iResult = 5;
							}
							else iResult = 16;
						}
						else iResult = 17;

						m_bIsSpecialEventTime = FALSE;
						break;
					}
				}

				ZeroMemory(cNpcName, sizeof(cNpcName));
				//Random Monster Spawns
				switch (iResult) {
				case 1:  strcpy(cNpcName, "Slime");				iNpcID = 10; iProbSA = 5;  iKindSA = 1; break;
				case 2:  strcpy(cNpcName, "Giant-Ant");			iNpcID = 16; iProbSA = 10; iKindSA = 2; break;
				case 3:  strcpy(cNpcName, "Orc");				iNpcID = 14; iProbSA = 15; iKindSA = 1; break;
				case 4:  strcpy(cNpcName, "Zombie");			iNpcID = 18; iProbSA = 15; iKindSA = 3; break;
				case 5:  strcpy(cNpcName, "Skeleton");			iNpcID = 11; iProbSA = 35; iKindSA = 8; break;
				case 6:  strcpy(cNpcName, "Orc-Mage");			iNpcID = 14; iProbSA = 30; iKindSA = 7; break;
				case 7:  strcpy(cNpcName, "Scorpion");			iNpcID = 17; iProbSA = 15; iKindSA = 3; break;
				case 8:  strcpy(cNpcName, "Stone-Golem");		iNpcID = 12; iProbSA = 25; iKindSA = 5; break;
				case 9:  strcpy(cNpcName, "Cyclops");			iNpcID = 13; iProbSA = 35; iKindSA = 8; break;
				case 10: strcpy(cNpcName, "Amphis");			iNpcID = 22; iProbSA = 20; iKindSA = 3; break;
				case 11: strcpy(cNpcName, "Clay-Golem");		iNpcID = 23; iProbSA = 20; iKindSA = 5; break;
				case 12: strcpy(cNpcName, "Troll");				iNpcID = 28; iProbSA = 25; iKindSA = 3; break;
				case 13: strcpy(cNpcName, "Orge");				iNpcID = 29; iProbSA = 25; iKindSA = 1; break;
				case 14: strcpy(cNpcName, "Hellbound");			iNpcID = 27; iProbSA = 25; iKindSA = 8; break;
				case 15: strcpy(cNpcName, "Liche");				iNpcID = 30; iProbSA = 30; iKindSA = 8; break;
				case 16: strcpy(cNpcName, "Demon");				iNpcID = 31; iProbSA = 20; iKindSA = 8; break;
				case 17: strcpy(cNpcName, "Unicorn");			iNpcID = 32; iProbSA = 35; iKindSA = 7; break;
				case 18: strcpy(cNpcName, "WereWolf");			iNpcID = 33; iProbSA = 25; iKindSA = 1; break;
				case 19: strcpy(cNpcName, "YB-Aresden");		iNpcID = -1;  iProbSA = 15; iKindSA = 1; break;
				case 20: strcpy(cNpcName, "YB-Elvine");			iNpcID = -1;  iProbSA = 15; iKindSA = 1; break;
				case 21: strcpy(cNpcName, "Gagoyle");			iNpcID = 52; iProbSA = 20; iKindSA = 8; break;
				case 22: strcpy(cNpcName, "Beholder");			iNpcID = 53; iProbSA = 20; iKindSA = 5; break;
				case 23: strcpy(cNpcName, "Dark-Elf");			iNpcID = 54; iProbSA = 20; iKindSA = 3; break;
				case 24: strcpy(cNpcName, "Rabbit");			iNpcID = -1; iProbSA = 5;  iKindSA = 1; break;
				case 25: strcpy(cNpcName, "Cat");				iNpcID = -1; iProbSA = 10; iKindSA = 2; break;
				case 26: strcpy(cNpcName, "Giant-Frog");		iNpcID = 57; iProbSA = 10; iKindSA = 2; break;
				case 27: strcpy(cNpcName, "Mountain-Giant");	iNpcID = 58; iProbSA = 25; iKindSA = 1; break;
				case 28: strcpy(cNpcName, "Ettin");				iNpcID = 59; iProbSA = 20; iKindSA = 8; break;
				case 29: strcpy(cNpcName, "Cannibal-Plant");	iNpcID = 60; iProbSA = 20; iKindSA = 5; break;
				case 30: strcpy(cNpcName, "Rudolph");			iNpcID = -1; iProbSA = 20; iKindSA = 5; break;
				case 31: strcpy(cNpcName, "Ice-Golem");			iNpcID = 65; iProbSA = 35; iKindSA = 8; break;
				case 32: strcpy(cNpcName, "DireBoar");			iNpcID = 62; iProbSA = 20; iKindSA = 5; break;
				case 33: strcpy(cNpcName, "Frost");				iNpcID = 63; iProbSA = 30; iKindSA = 8; break;
				case 34: strcpy(cNpcName, "Stalker");           iNpcID = 48; iProbSA = 20; iKindSA = 1; break;
				case 35: strcpy(cNpcName, "Hellclaw");			iNpcID = 49; iProbSA = 20; iKindSA = 1; break;
				case 36: strcpy(cNpcName, "Wyvern");			iNpcID = 66; iProbSA = 20; iKindSA = 1; break;
				case 37: strcpy(cNpcName, "Fire-Wyvern");		iNpcID = -1; iProbSA = 20; iKindSA = 1; break;
				case 38: strcpy(cNpcName, "Barlog");			iNpcID = -1; iProbSA = 20; iKindSA = 1; break;
				case 39: strcpy(cNpcName, "Tentocle");			iNpcID = -1; iProbSA = 20; iKindSA = 1; break;
				case 40: strcpy(cNpcName, "Centaurus");			iNpcID = -1; iProbSA = 20; iKindSA = 1; break;
				case 41: strcpy(cNpcName, "Giant-Lizard");		iNpcID = -1; iProbSA = 20; iKindSA = 1; break;
				case 42: strcpy(cNpcName, "Minotaurs");			iNpcID = -1; iProbSA = 20; iKindSA = 1; break;
				case 43: strcpy(cNpcName, "Tentocle");			iNpcID = -1; iProbSA = 20; iKindSA = 1; break;
				case 44: strcpy(cNpcName, "Claw-Turtle");		iNpcID = -1; iProbSA = 20; iKindSA = 1; break;
				case 45: strcpy(cNpcName, "Giant-Crayfish");	iNpcID = -1; iProbSA = 20; iKindSA = 1; break;
				case 46: strcpy(cNpcName, "Giant-Plant");		iNpcID = -1; iProbSA = 20; iKindSA = 1; break;
				case 47: strcpy(cNpcName, "MasterMage-Orc");	iNpcID = -1; iProbSA = 20; iKindSA = 1; break;
				case 48: strcpy(cNpcName, "Nizie");				iNpcID = -1; iProbSA = 20; iKindSA = 1; break;
				case 49: strcpy(cNpcName, "Tigerworm");			iNpcID = 50; iProbSA = 20; iKindSA = 1; break;
				case 50: strcpy(cNpcName, "Giant-Plant");		iNpcID = -1; iProbSA = 20; iKindSA = 1; break;
				case 51: strcpy(cNpcName, "Abaddon");			iNpcID = -1; iProbSA = 20; iKindSA = 9; break;
				case 52: strcpy(cNpcName, "YW-Aresden");		iNpcID = -1; iProbSA = 15; iKindSA = 1; break;
				case 53: strcpy(cNpcName, "YW-Elvine");			iNpcID = -1; iProbSA = 15; iKindSA = 1; break;
				case 54: strcpy(cNpcName, "YY-Aresden");		iNpcID = -1; iProbSA = 15; iKindSA = 1; break;
				case 55: strcpy(cNpcName, "YY-Elvine");			iNpcID = -1; iProbSA = 15; iKindSA = 1; break;
				case 56: strcpy(cNpcName, "XB-Aresden");		iNpcID = -1; iProbSA = 15; iKindSA = 1; break;
				case 57: strcpy(cNpcName, "XB-Elvine");			iNpcID = -1; iProbSA = 15; iKindSA = 1; break;
				case 58: strcpy(cNpcName, "XW-Aresden");		iNpcID = -1; iProbSA = 15; iKindSA = 1; break;
				case 59: strcpy(cNpcName, "XW-Elvine");			iNpcID = -1; iProbSA = 15; iKindSA = 1; break;
				case 60: strcpy(cNpcName, "XY-Aresden");		iNpcID = -1; iProbSA = 15; iKindSA = 1; break;
				case 61: strcpy(cNpcName, "XY-Elvine");			iNpcID = -1; iProbSA = 15; iKindSA = 1; break;
				default: strcpy(cNpcName, "Orc");				iNpcID = 14; iProbSA = 15; iKindSA = 1; break;
				}

				cSA = 0;
				if (iDice(1, 100) <= iProbSA) {
					cSA = _cGetSpecialAbility(iKindSA);
				}

				bMaster = bCreateNewNpc(cNpcName, cName_Master, m_pMapList[i]->m_cName, (rand() % 3), cSA, DEF_MOVETYPE_RANDOM, &pX, &pY, cWaypoint, NULL, NULL, -1, FALSE, FALSE, bFirmBerserk, TRUE);

				if (!bMaster)
				{
					m_pMapList[i]->SetNamingValueEmpty(iNamingValue);
					iTotalMob = 0;
				}

			}

			switch (iResult) {

			case 1:
			case 2:
			case 3:
			case 10:
			case 12:
			case 18:
			case 21:
			case 29:
				iTotalMob = iDice(1, 5) - 1;
				break;

			case 4:
			case 5:
			case 6:
			case 7:
			case 11:
			case 13:
			case 15:
			case 26:
			case 27:
			case 28:
			case 30:
			case 31:
			case 42:
			case 44:
				iTotalMob = iDice(1, 3) - 1;
				break;

			case 8:
			case 9:
			case 14:
			case 16:
			case 17:
			case 19:
			case 20:
			case 22:
			case 23:
			case 25:
				iTotalMob = iDice(1, 2) - 1;
				break;

			case 24:
				iTotalMob = iDice(1, 4) - 1;
				break;

			case 32:
			case 33:
			case 34:
			case 35:
			case 36:
			case 37:
			case 38:
			case 39:
			case 40:
			case 41:
			case 43:
			case 45:
				iTotalMob = 1;
				break;

			default:
				iTotalMob = 0;
				break;
			}


			if (iTotalMob > 2) {
				switch (iResult) {
				case 1:  // Slime 
				case 2:  // Giant-Ant
				case 3:  // Orc
				case 4:  // Zombie
				case 5:  // Skeleton
				case 6:  // Orc-Mage
				case 7:  // Scorpion
				case 8:  // Stone-Golem
				case 9:  // Cyclops
				case 10: // Amphis
				case 11: // Clay-Golem
				case 12: // Troll
				case 13: // Orge
				case 14: // Hellbound
				case 15: // Liche
				case 16: // Demon
				case 17: // Unicorn
				case 18: // WereWolf
				case 19:
				case 20:
				case 21:
				case 22:
				case 23:
				case 24:
				case 25:
				case 26:
				case 27:
				case 28:
				case 29:
				case 30:
				case 31:
				case 32:
					if (iDice(1, 5) == 1) iTotalMob = 0;  // 75% ???????? ???.
					break;

				case 33:
				case 34:
				case 35:
				case 36:
				case 37:
				case 38:
				case 39:
				case 40:
				case 41:
				case 42:
				case 44:
				case 45:
				case 46:
				case 47:
				case 48:
				case 49:
					if (iDice(1, 5) != 1) iTotalMob = 0;  // 75% ???????????????
					break;
				}
			}

			if (bIsSpecialEvent == TRUE) {
				switch (m_cSpecialEventType) {
				case 1:
					if ((iResult != 35) && (iResult != 36) && (iResult != 37) && (iResult != 49)
						&& (iResult != 51) && (iResult != 15) && (iResult != 16) && (iResult != 21)) iTotalMob = 12;
					for (x = 1; x < DEF_MAXCLIENTS; x++)
						if ((iNpcID != -1) && (m_pClientList[x] != NULL) && (m_pClientList[x]->m_bIsInitComplete == TRUE)) {
							SendNotifyMsg(NULL, x, DEF_NOTIFY_SPAWNEVENT, pX, pY, iNpcID, NULL, NULL, NULL);
						}
					break;

				case 2:
					if ((memcmp(m_pMapList[i]->m_cLocationName, "aresden", 7) == 0) ||
						(memcmp(m_pMapList[i]->m_cLocationName, "elvine", 6) == 0) ||
						(memcmp(m_pMapList[i]->m_cLocationName, "elvfarm", 7) == 0) ||
						(memcmp(m_pMapList[i]->m_cLocationName, "arefarm", 7) == 0)) {
						iTotalMob = 0;
					}
					break;
				}
				m_bIsSpecialEventTime = FALSE;
			}

			for (j = 0; j < iTotalMob; j++) {
				iNamingValue = m_pMapList[i]->iGetEmptyNamingValue();
				if (iNamingValue != -1) {
					ZeroMemory(cName_Slave, sizeof(cName_Slave));
					wsprintf(cName_Slave, "XX%d", iNamingValue);
					cName_Slave[0] = 95; // original '_';
					cName_Slave[1] = i + 65;

					cSA = 0;

					if (iDice(1, 100) <= iProbSA) {
						cSA = _cGetSpecialAbility(iKindSA);
					}

					if (bCreateNewNpc(cNpcName, cName_Slave, m_pMapList[i]->m_cName, (rand() % 3), cSA, DEF_MOVETYPE_RANDOM, &pX, &pY, cWaypoint, NULL, NULL, -1, FALSE, FALSE, bFirmBerserk) == FALSE) {
						m_pMapList[i]->SetNamingValueEmpty(iNamingValue);
					}
					else {
						bSetNpcFollowMode(cName_Slave, cName_Master, DEF_OWNERTYPE_NPC);
					}
				}
			}
		}

		if ((m_pMapList[i] != NULL) && ((m_pMapList[i]->m_iMaximumObject) > m_pMapList[i]->m_iTotalActiveObject))
		{
			for (j = 1; j < DEF_MAXSPOTMOBGENERATOR; j++)
				if ((iDice(1, 3) == 2) && (m_pMapList[i]->m_stSpotMobGenerator[j].bDefined == TRUE) &&
					(m_pMapList[i]->m_stSpotMobGenerator[j].iMaxMobs > m_pMapList[i]->m_stSpotMobGenerator[j].iCurMobs))
				{
					iNamingValue = m_pMapList[i]->iGetEmptyNamingValue();
					if (iNamingValue != -1)
					{
							bIsGuard = FALSE;
							ZeroMemory(cNpcName, sizeof(cNpcName));
							switch (m_pMapList[i]->m_stSpotMobGenerator[j].iMobType) {
								// spot-mob-generator
							case 10:  strcpy(cNpcName, "Slime");				iProbSA = 5;  iKindSA = 1;  break;
							case 16:  strcpy(cNpcName, "Giant-Ant");			iProbSA = 10; iKindSA = 2;  break;
							case 14:  strcpy(cNpcName, "Orc");				iProbSA = 15; iKindSA = 1;  break;
							case 18:  strcpy(cNpcName, "Zombie");			iProbSA = 15; iKindSA = 3;  break;
							case 11:  strcpy(cNpcName, "Skeleton");			iProbSA = 35; iKindSA = 8;  break;
							case 6:   strcpy(cNpcName, "Orc-Mage");			iProbSA = 30; iKindSA = 7;  break;
							case 17:  strcpy(cNpcName, "Scorpion");			iProbSA = 15; iKindSA = 3;  break;
							case 12:  strcpy(cNpcName, "Stone-Golem");		iProbSA = 25; iKindSA = 5;  break;
							case 13:  strcpy(cNpcName, "Cyclops");			iProbSA = 35; iKindSA = 8;  break;
							case 22:  strcpy(cNpcName, "Amphis");			iProbSA = 20; iKindSA = 3;  break;
							case 23:  strcpy(cNpcName, "Clay-Golem");		iProbSA = 20; iKindSA = 5;  break;

								// centu - guards
							case 24:  strcpy(cNpcName, "Guard-Aresden");		bIsGuard = TRUE; break;
							case 25:  strcpy(cNpcName, "Guard-Elvine");		bIsGuard = TRUE; break;
							case 26:  strcpy(cNpcName, "Guard-Neutral");		bIsGuard = TRUE; break;

							case 27:  strcpy(cNpcName, "Hellbound");			iProbSA = 20; iKindSA = 1;  break;
							case 29:  strcpy(cNpcName, "Orge");              iProbSA = 20; iKindSA = 1;  break;
							case 30:  strcpy(cNpcName, "Liche");				iProbSA = 30; iKindSA = 8;  break;
							case 31:  strcpy(cNpcName, "Demon");				iProbSA = 20; iKindSA = 8;  break;
							case 32:  strcpy(cNpcName, "Unicorn");			iProbSA = 35; iKindSA = 7;  break;
							case 33:  strcpy(cNpcName, "WereWolf");			iProbSA = 25; iKindSA = 1;  break;
							case 34:  strcpy(cNpcName, "Dummy");				iProbSA = 5;  iKindSA = 1;  break;
							case 35:  strcpy(cNpcName, "Attack-Dummy");		iProbSA = 5;  iKindSA = 1;  break;
							case 48:  strcpy(cNpcName, "Stalker");			iProbSA = 20; iKindSA = 3;  break;

							case 49:  strcpy(cNpcName, "Hellclaw");			iProbSA = 20; iKindSA = 8;  break;
							case 50:  strcpy(cNpcName, "Tigerworm");			iProbSA = 20; iKindSA = 8;  break;
							case 54:  strcpy(cNpcName, "Dark-Elf");			iProbSA = 20; iKindSA = 8;  break;
							case 53:  strcpy(cNpcName, "Beholder");			iProbSA = 20; iKindSA = 8;  break;
							case 52:  strcpy(cNpcName, "Gagoyle");			iProbSA = 20; iKindSA = 8;  break;

							case 57:  strcpy(cNpcName, "Giant-Frog");		iProbSA = 10; iKindSA = 2;  break;
							case 58:  strcpy(cNpcName, "Mountain-Giant");	iProbSA = 25; iKindSA = 1;  break;
							case 59:  strcpy(cNpcName, "Ettin");				iProbSA = 20; iKindSA = 8;  break;
							case 60:  strcpy(cNpcName, "Cannibal-Plant");	iProbSA = 20; iKindSA = 5;  break;
							case 61:  strcpy(cNpcName, "Rudolph");			iProbSA = 20; iKindSA = 1;  break;
							case 62:  strcpy(cNpcName, "DireBoar");			iProbSA = 20; iKindSA = 1;  break;
							case 63:  strcpy(cNpcName, "Frost");				iProbSA = 20; iKindSA = 8;  break;
							case 65:  strcpy(cNpcName, "Ice-Golem");			iProbSA = 20; iKindSA = 8;  break;
							case 66:  strcpy(cNpcName, "Wyvern");			iProbSA = 20; iKindSA = 1;  break;
							case 55:  strcpy(cNpcName, "Rabbit");			iProbSA = 20; iKindSA = 1;  break;
							case 67:  strcpy(cNpcName, "McGaffin");			iProbSA = 20; iKindSA = 1;  break;
							case 68:  strcpy(cNpcName, "Perry");				iProbSA = 20; iKindSA = 1;  break;
							case 69:  strcpy(cNpcName, "Devlin");			iProbSA = 20; iKindSA = 1;  break;
							case 73:  strcpy(cNpcName, "Fire-Wyvern");		iProbSA = 20; iKindSA = 1;  break;
							case 70:  strcpy(cNpcName, "Barlog");			iProbSA = 20; iKindSA = 1;  break;
							case 80:  strcpy(cNpcName, "Tentocle");			iProbSA = 20; iKindSA = 1;  break;
							case 71:  strcpy(cNpcName, "Centaurus");			iProbSA = 20; iKindSA = 1;  break;
							case 75:  strcpy(cNpcName, "Giant-Lizard");		iProbSA = 20; iKindSA = 1;  break;
							case 78:  strcpy(cNpcName, "Minotaurs");			iProbSA = 20; iKindSA = 1;  break;
							case 81:  strcpy(cNpcName, "Abaddon");			iProbSA = 20; iKindSA = 9;  break;
							case 72:  strcpy(cNpcName, "Claw-Turtle");		iProbSA = 20; iKindSA = 1;  break;
							case 74:  strcpy(cNpcName, "Giant-Crayfish");	iProbSA = 20; iKindSA = 1;  break;
							case 76:  strcpy(cNpcName, "Giant-Plant");		iProbSA = 20; iKindSA = 1;  break;
							case 77:  strcpy(cNpcName, "MasterMage-Orc");	iProbSA = 20; iKindSA = 1;  break;
							case 79:  strcpy(cNpcName, "Nizie");				iProbSA = 20; iKindSA = 1; break;
							//Magn0S:: New Npc's
							case 92:  strcpy(cNpcName, "Eternal-Dragon");			iProbSA = 20; iKindSA = 9;  break;
							case 93:  strcpy(cNpcName, "Black-Demon");			iProbSA = 20; iKindSA = 9;  break;
							case 94:  strcpy(cNpcName, "Black-Wyvern");			iProbSA = 20; iKindSA = 9;  break;
							case 95:  strcpy(cNpcName, "Light-Wyvern");			iProbSA = 20; iKindSA = 9;  break;
							case 96:  strcpy(cNpcName, "Poison-Wyvern");			iProbSA = 20; iKindSA = 9;  break;
							case 97:  strcpy(cNpcName, "Heaven-Wyvern");			iProbSA = 20; iKindSA = 9;  break;
							case 98:  strcpy(cNpcName, "Illusion-Wyvern");			iProbSA = 20; iKindSA = 9;  break;
							case 99:  strcpy(cNpcName, "Ghost-Abaddon");			iProbSA = 20; iKindSA = 9;  break;


							default:
								strcpy(cNpcName, "Orc");
								iProbSA = 15;
								iKindSA = 1;
								break;
							}
							/* NPCs not spawning in pits:

							// 15 ShopKeeper-W
							// 19 Gandlf
							// 20 Howard
							// 36 Arrow Gaurd Tower Kit - Aresden, Elvine
							// 37 Cannon Gaurd Tower Kit - Aresden, Elvine
							// 38 Mana Collector Kit - Aresden, Elvine
							// 39 Detector Constructor Kit - Aresden, Elvine
							// 40 Energy Shield Generator - Aresden, Elvine
							// 41 Grand Master Generator - Aresden Elvine
							// 43 Light War Beetle - Aresden, Elvine
							// 44 God's Hand Knight
							// 45 Mounted God's Hand Knight
							// 46 Temple Knight
							// 47 Battle Golem
							// 51 Catapult
							// 64 Crops
							*/
							bFirmBerserk = FALSE;
							if ((iMapLevel == 5) && (iDice(1, 3) == 1)) bFirmBerserk = TRUE;

							ZeroMemory(cName_Master, sizeof(cName_Master));
							wsprintf(cName_Master, "XX%d", iNamingValue);
							cName_Master[0] = 95; // original '_';
							cName_Master[1] = i + 65;

							cSA = 0;

							// centu - guards only Magic Destruction & Clairvoyrant
							if (bIsGuard)
							{
								cSA = _cGetSpecialAbility(10);
							}
							else
							{
								if ((m_pMapList[i]->m_stSpotMobGenerator[j].iMobType != 34) && (iDice(1, 100) <= iProbSA)) {
									cSA = _cGetSpecialAbility(iKindSA);
								}
							}

							switch (m_pMapList[i]->m_stSpotMobGenerator[j].cType) {
							case 1:
								if (bCreateNewNpc(cNpcName, cName_Master, m_pMapList[i]->m_cName, (rand() % 3), cSA, DEF_MOVETYPE_RANDOMAREA, &pX, &pY, cWaypoint, &m_pMapList[i]->m_stSpotMobGenerator[j].rcRect, j, -1, FALSE, FALSE, bFirmBerserk) == FALSE) {
									m_pMapList[i]->SetNamingValueEmpty(iNamingValue);
								}
								else {
									m_pMapList[i]->m_stSpotMobGenerator[j].iCurMobs++;
								}
								break;

							case 2:
								if (bCreateNewNpc(cNpcName, cName_Master, m_pMapList[i]->m_cName, (rand() % 3), cSA, DEF_MOVETYPE_RANDOMWAYPOINT, NULL, NULL, m_pMapList[i]->m_stSpotMobGenerator[j].cWaypoint, NULL, j, -1, FALSE, FALSE, bFirmBerserk) == FALSE) {
									m_pMapList[i]->SetNamingValueEmpty(iNamingValue);
								}
								else {
									m_pMapList[i]->m_stSpotMobGenerator[j].iCurMobs++;
								}
								break;
							}
						}
					}
		}
	}
}

// SNOOPY: reconnecting clients will recover summons 
void CGame::iRecoverFollowers(int  iClientH, BOOL bControlAll)
{
	if (m_pClientList[iClientH] == NULL) return;
	int i, iTotal;
	char  cKillerMsg[128];
	iTotal = 0;
	for (i = 1; i < DEF_MAXNPCS; i++)
		if ((m_pNpcList[i] != NULL)
			&& (m_pNpcList[i]->m_cFollowOwnerType > 7)	// Reduce cases to process
			&& ((m_pNpcList[i]->m_bIsSummoned == TRUE) || (m_pNpcList[i]->m_dwTamingTime != 0))
			&& (m_pNpcList[i]->m_bIsKilled == FALSE))
		{
			switch (m_pNpcList[i]->m_cFollowOwnerType) {
			case DEF_OWNERTYPE_PLAYER_FREE: // Anybody can take control
				if ((bControlAll == TRUE)
					&& (m_pNpcList[i]->m_cSide == m_pClientList[iClientH]->m_cSide))
				{
					m_pNpcList[i]->m_iFollowOwnerIndex = iClientH;
					m_pNpcList[i]->m_cFollowOwnerType = DEF_OWNERTYPE_PLAYER;
					m_pNpcList[i]->m_cMoveType = DEF_MOVETYPE_FOLLOW;
					strcpy(m_pNpcList[i]->m_cFollowOwnerCharName, m_pClientList[iClientH]->m_cCharName);
					//Tell client				
					ZeroMemory(cKillerMsg, sizeof(cKillerMsg));
					wsprintf(cKillerMsg, "A %s soldier accepted to follow you.", m_pNpcList[i]->m_cNpcName);
					SendNotifyMsg(iClientH, iClientH, DEF_NOTIFY_NOTICEMSG, NULL, NULL, NULL, cKillerMsg);
				}
				break;
			case DEF_OWNERTYPE_PLAYER_WAITING:
				if (strcmp(m_pNpcList[i]->m_cFollowOwnerCharName, m_pClientList[iClientH]->m_cCharName) == 0)
				{
					m_pNpcList[i]->m_iFollowOwnerIndex = iClientH;
					m_pNpcList[i]->m_cFollowOwnerType = DEF_OWNERTYPE_PLAYER;
					m_pNpcList[i]->m_cMoveType = DEF_MOVETYPE_FOLLOW;
					//Tell client				
					ZeroMemory(cKillerMsg, sizeof(cKillerMsg));
					if ((m_bIsHeldenianMode == TRUE)
						&& (m_pMapList[m_pNpcList[i]->m_cMapIndex] != NULL)
						&& (m_pMapList[m_pNpcList[i]->m_cMapIndex]->m_bIsHeldenianMap == 1))
					{
						wsprintf(cKillerMsg, "Recovered control of a %s soldier.", m_pNpcList[i]->m_cNpcName);
						SendNotifyMsg(iClientH, iClientH, DEF_NOTIFY_NOTICEMSG, NULL, NULL, NULL, cKillerMsg);
					}
					else
					{
						wsprintf(cKillerMsg, "Recovered control of a %s summon.", m_pNpcList[i]->m_cNpcName);
						SendNotifyMsg(iClientH, iClientH, DEF_NOTIFY_NOTICEMSG, NULL, NULL, NULL, cKillerMsg);
					}
				}
				break;
			}
		}
}

/*********************************************************************************************************************
**  BOOL CGame::bSetNpcFollowMode(char * pName, char * pFollowName, char cFollowOwnerType)							**
**  DESCRIPTION			:: set npc follow mode																		**
**  LAST_UPDATED		:: March 18, 2005; 9:43 AM; Hypnotoad														**
**	RETURN_VALUE		:: BOOL																						**
**  NOTES				::	n/a																						**
**	MODIFICATION		::	n/a																						**
**********************************************************************************************************************/
/*****************************************************************************************************
**							  Follower mode functions:												**
**	bSetNpcFollowMode:	After creating a mob, gives him a master...									**
**  RemoveFromTarget:   If target or master Killed/TP/deco/Invi, check follow mode					**
**  ReleaseFollowMode:  Check if a killed npc had followers, and free them							**
**  iRecoverFollowers:  Restore follow mode after a client has disconnected the connected again		**
**  iGetFollowerNumber: Counts the follower numbers to prevent too much summons						**
*****************************************************************************************************/
BOOL CGame::bSetNpcFollowMode(char* pName, char* pFollowName, char cFollowOwnerType)
{
	int i, iIndex, iMapIndex, iFollowIndex;
	char cTmpName[11], cFollowSide;
	ZeroMemory(cTmpName, sizeof(cTmpName));// Snoopy: added m_cFollowOwnerCharName
	iIndex = -1;
	iMapIndex = -1;
	iFollowIndex = -1;
	for (i = 1; i < DEF_MAXNPCS; i++)
		if ((m_pNpcList[i] != NULL) && (memcmp(m_pNpcList[i]->m_cName, pName, 5) == 0))
		{
			iIndex = i;
			iMapIndex = m_pNpcList[i]->m_cMapIndex;
			break;
		}
	if (iIndex == -1) return FALSE;

	switch (cFollowOwnerType) {
	case DEF_OWNERTYPE_NPC:
		for (i = 1; i < DEF_MAXNPCS; i++)
			if ((m_pNpcList[i] != NULL) && (memcmp(m_pNpcList[i]->m_cName, pFollowName, 5) == 0))
			{
				if (m_pNpcList[i]->m_cMapIndex != iMapIndex) return FALSE;
				m_pNpcList[iIndex]->m_cMoveType = DEF_MOVETYPE_FOLLOW;
				m_pNpcList[iIndex]->m_cFollowOwnerType = DEF_OWNERTYPE_NPC;
				m_pNpcList[iIndex]->m_iFollowOwnerIndex = i;
				ZeroMemory(m_pNpcList[i]->m_cFollowOwnerCharName, sizeof(m_pNpcList[i]->m_cFollowOwnerCharName));
				m_pNpcList[iIndex]->m_cSide = m_pNpcList[i]->m_cSide;
				return TRUE;
			}
		break;

	case DEF_OWNERTYPE_PLAYER:
		for (i = 1; i < DEF_MAXCLIENTS; i++)
			if ((m_pClientList[i] != NULL) && (memcmp(m_pClientList[i]->m_cCharName, pFollowName, 10) == 0))
			{
				if (m_pClientList[i]->m_cMapIndex != iMapIndex) return FALSE;
				iFollowIndex = i;
				cFollowSide = m_pClientList[i]->m_cSide;
				m_pNpcList[iIndex]->m_cMoveType = DEF_MOVETYPE_FOLLOW;
				m_pNpcList[iIndex]->m_cFollowOwnerType = DEF_OWNERTYPE_PLAYER;
				m_pNpcList[iIndex]->m_iFollowOwnerIndex = i;
				strcpy(m_pNpcList[iIndex]->m_cFollowOwnerCharName, m_pClientList[i]->m_cCharName);
				m_pNpcList[iIndex]->m_cSide = m_pClientList[i]->m_cSide;
				return TRUE;
			}
		break;
	}
	return FALSE;
}

/*********************************************************************************************************************
**  int CGame::bCreateNewNpc(char * pNpcName, char * pName, char * pMapName, short sClass, char cSA, char cMoveType,**
** int * poX, int * poY, char * pWaypointList, RECT * pArea, int iSpotMobIndex, char cChangeSide, BOOL bHideGenMode,**
** BOOL bIsSummoned, BOOL bFirmBerserk, BOOL bIsMaster, int iGuildGUID)												**
**  DESCRIPTION			:: creates a npc																			**
**  LAST_UPDATED		:: March 18, 2005; 6:01 PM; Hypnotoad														**
**	RETURN_VALUE		:: int																						**
**  NOTES				::	- sends 1x1 monsters to a different position verifier than 2x2/3x3						**
**							- new - expmin and expmax x 2 if npc is bFirmBerserk									**
**							- creates blocked spaces below 1x1, 2x2, and 3x3 monsters								**
**	MODIFICATION		::	- ExpDice MIN/MAX and bFirmBerserk requires testing										**
**********************************************************************************************************************/
int CGame::bCreateNewNpc(char* pNpcName, char* pName, char* pMapName, short sClass, char cSA, char cMoveType, int* poX, int* poY, char* pWaypointList, RECT* pArea, int iSpotMobIndex, char cChangeSide, BOOL bHideGenMode, BOOL bIsSummoned, BOOL bFirmBerserk, BOOL bIsMaster, int iGuildGUID)
{
	int i, t, j, k, iMapIndex;
	char  cTmpName[11], cTxt[120];
	short sX, sY, sRange;
	BOOL  bFlag;
	SYSTEMTIME SysTime;

	if (strlen(pName) == 0)   return FALSE;
	if (strlen(pNpcName) == 0) return FALSE;
	GetLocalTime(&SysTime);
	ZeroMemory(cTmpName, sizeof(cTmpName));
	strcpy(cTmpName, pMapName);
	iMapIndex = -1;

	for (i = 0; i < DEF_MAXMAPS; i++)
		if (m_pMapList[i] != NULL) {
			if (memcmp(m_pMapList[i]->m_cName, cTmpName, 10) == 0)
				iMapIndex = i;
		}

	if (iMapIndex == -1) return FALSE;

	for (i = 1; i < DEF_MAXNPCS; i++)
		if (m_pNpcList[i] == NULL) {
			m_pNpcList[i] = new class CNpc(pName);
			if (_bInitNpcAttr(m_pNpcList[i], pNpcName, sClass, cSA) == FALSE) {
				wsprintf(cTxt, "(!) Not existing NPC creation request! (%s) Ignored.", pNpcName);
				PutLogList(cTxt);

				delete m_pNpcList[i];
				m_pNpcList[i] = NULL;
				break;
			}
			if (m_pNpcList[i]->m_cDayOfWeekLimit < 10) {
				if (m_pNpcList[i]->m_cDayOfWeekLimit != SysTime.wDayOfWeek) {
					delete m_pNpcList[i];
					m_pNpcList[i] = NULL;
					break;
				}
			}
			switch (cMoveType) {
			case DEF_MOVETYPE_GUARD:
			case DEF_MOVETYPE_RANDOM:
				if ((poX != NULL) && (poY != NULL) && (*poX != NULL) && (*poY != NULL)) {
					sX = *poX;
					sY = *poY;
				}
				else {
					for (j = 0; j <= 30; j++) {
						// SNOOPY: Why -15 ?
						sX = (rand() % (m_pMapList[iMapIndex]->m_sSizeX - 50)) + 25;//15
						sY = (rand() % (m_pMapList[iMapIndex]->m_sSizeY - 50)) + 25;//15
						bFlag = TRUE;
						for (k = 0; k < DEF_MAXMGAR; k++)
							if (m_pMapList[iMapIndex]->m_rcMobGenAvoidRect[k].left != -1) {
								if ((sX >= m_pMapList[iMapIndex]->m_rcMobGenAvoidRect[k].left) &&
									(sX <= m_pMapList[iMapIndex]->m_rcMobGenAvoidRect[k].right) &&
									(sY >= m_pMapList[iMapIndex]->m_rcMobGenAvoidRect[k].top) &&
									(sY <= m_pMapList[iMapIndex]->m_rcMobGenAvoidRect[k].bottom)) {
									bFlag = FALSE;
								}
							}
						// SNOOPY: Added condition for accessible map place.
						// Secondary possible effect: Less spawns in small corridors ?
						if ((m_pMapList[iMapIndex]->bGetMoveable(sX, sY) == FALSE) ||
							(m_pMapList[iMapIndex]->bGetIsTeleport(sX, sY) == TRUE))
						{
							bFlag = FALSE; // Not accessible place
						}
						if (bFlag == TRUE) goto GET_VALIDLOC_SUCCESS;
					}
					delete m_pNpcList[i];
					m_pNpcList[i] = NULL;
					return FALSE;

				GET_VALIDLOC_SUCCESS:;
				}
				break;

			case DEF_MOVETYPE_RANDOMAREA:
				sRange = (short)(pArea->right - pArea->left);
				if (sRange <= 0) sRange = 1; // SNOOPY antibug
				sX = (short)((rand() % sRange) + pArea->left);
				sRange = (short)(pArea->bottom - pArea->top);
				if (sRange <= 0) sRange = 1; // SNOOPY antibug
				sY = (short)((rand() % sRange) + pArea->top);
				break;

			case DEF_MOVETYPE_RANDOMWAYPOINT:
				sX = (short)m_pMapList[iMapIndex]->m_WaypointList[pWaypointList[iDice(1, 10) - 1]].x;
				sY = (short)m_pMapList[iMapIndex]->m_WaypointList[pWaypointList[iDice(1, 10) - 1]].y;
				break;

			default:
				if ((poX != NULL) && (poY != NULL) && (*poX != NULL) && (*poY != NULL)) {
					sX = *poX;
					sY = *poY;
				}
				else {
					sX = (short)m_pMapList[iMapIndex]->m_WaypointList[pWaypointList[0]].x;
					sY = (short)m_pMapList[iMapIndex]->m_WaypointList[pWaypointList[0]].y;
				}
				break;
			}
			// new - sends 1x1 monsters to a different position checker than 2x2/3x3
			if (m_pNpcList[i]->m_sAreaSize == 0) {
				if (bGetEmptyPosition(&sX, &sY, iMapIndex) == FALSE) {
					delete m_pNpcList[i];
					m_pNpcList[i] = NULL;
					break;
				}
			}
			else {
				// new - checks if walk location is free for larger than 1x1 monsters
				if (bGetNpcMovementArea(i, sX, sY, iMapIndex, m_pNpcList[i]->m_sAreaSize) == FALSE) {
					delete m_pNpcList[i];
					m_pNpcList[i] = NULL;
					break;
				}
			}

			if ((bHideGenMode == TRUE) && (_iGetPlayerNumberOnSpot(sX, sY, iMapIndex, 7) != 0)) {
				delete m_pNpcList[i];
				m_pNpcList[i] = NULL;
				break;
			}

			if ((poX != NULL) && (poY != NULL)) {
				*poX = sX;
				*poY = sY;
			}

			m_pNpcList[i]->m_sX = sX;
			m_pNpcList[i]->m_sY = sY;
			m_pNpcList[i]->m_vX = sX;
			m_pNpcList[i]->m_vY = sY;
			for (t = 0; t < 10; t++) m_pNpcList[i]->m_iWayPointIndex[t] = pWaypointList[t];

			m_pNpcList[i]->m_cTotalWaypoint = 0;

			for (t = 0; t < 10; t++)
				if (m_pNpcList[i]->m_iWayPointIndex[t] != -1) m_pNpcList[i]->m_cTotalWaypoint++;

			if (pArea != NULL) {
				SetRect(&m_pNpcList[i]->m_rcRandomArea, pArea->left, pArea->top, pArea->right, pArea->bottom);
			}

			switch (cMoveType) {
			case DEF_MOVETYPE_GUARD:
				m_pNpcList[i]->m_dX = m_pNpcList[i]->m_sX;
				m_pNpcList[i]->m_dY = m_pNpcList[i]->m_sY;
				break;

			case DEF_MOVETYPE_SEQWAYPOINT:
				m_pNpcList[i]->m_cCurWaypoint = 1;
				m_pNpcList[i]->m_dX = (short)m_pMapList[iMapIndex]->m_WaypointList[m_pNpcList[i]->m_iWayPointIndex[m_pNpcList[i]->m_cCurWaypoint]].x;
				m_pNpcList[i]->m_dY = (short)m_pMapList[iMapIndex]->m_WaypointList[m_pNpcList[i]->m_iWayPointIndex[m_pNpcList[i]->m_cCurWaypoint]].y;
				break;

			case DEF_MOVETYPE_RANDOMWAYPOINT:
				m_pNpcList[i]->m_cCurWaypoint = (rand() % (m_pNpcList[i]->m_cTotalWaypoint - 1)) + 1;
				m_pNpcList[i]->m_dX = (short)m_pMapList[iMapIndex]->m_WaypointList[m_pNpcList[i]->m_iWayPointIndex[m_pNpcList[i]->m_cCurWaypoint]].x;
				m_pNpcList[i]->m_dY = (short)m_pMapList[iMapIndex]->m_WaypointList[m_pNpcList[i]->m_iWayPointIndex[m_pNpcList[i]->m_cCurWaypoint]].y;
				break;

			case DEF_MOVETYPE_RANDOMAREA:
				m_pNpcList[i]->m_cCurWaypoint = 0;
				sRange = (short)(m_pNpcList[i]->m_rcRandomArea.right - m_pNpcList[i]->m_rcRandomArea.left);
				m_pNpcList[i]->m_dX = (short)((rand() % sRange) + m_pNpcList[i]->m_rcRandomArea.left);
				sRange = (short)(m_pNpcList[i]->m_rcRandomArea.bottom - m_pNpcList[i]->m_rcRandomArea.top);
				m_pNpcList[i]->m_dY = (short)((rand() % sRange) + m_pNpcList[i]->m_rcRandomArea.top);
				break;

			case DEF_MOVETYPE_RANDOM:
				m_pNpcList[i]->m_dX = (short)((rand() % (m_pMapList[iMapIndex]->m_sSizeX - 50)) + 15);
				m_pNpcList[i]->m_dY = (short)((rand() % (m_pMapList[iMapIndex]->m_sSizeY - 50)) + 15);
				break;
			}
			m_pNpcList[i]->m_tmp_iError = 0;
			m_pNpcList[i]->m_cMoveType = cMoveType;

			switch (m_pNpcList[i]->m_cActionLimit) {
			case 2:
			case 3:
			case 5:
				m_pNpcList[i]->m_cBehavior = DEF_BEHAVIOR_STOP;
				switch (m_pNpcList[i]->m_sType) {
				case 15: // ShopKeeper-W
				case 19: // Gandlf
				case 20: // Howard
				case 24: // Tom
				case 25: // William
				case 26: // Kennedy
					m_pNpcList[i]->m_cDir = 6; //iDice(1, 3) + 3;
					break;

				default:
					m_pNpcList[i]->m_cDir = iDice(1, 8);
					break;
				}
				break;

			case 8: // Heldenian gate
				m_pNpcList[i]->m_cDir = 3;
				m_pNpcList[i]->m_cBehavior = DEF_BEHAVIOR_STOP;
				if (m_pNpcList[i]->m_cArea > 0)
				{
					for (short sx1 = (sX - 1); sx1 <= sX + 1; sx1++)
						for (short sy1 = (sY - 1); sy1 <= sY + 1; sy1++)
						{
							m_pMapList[iMapIndex]->SetTempMoveAllowedFlag(sx1, sy1, FALSE);
						}
				}
				break;

			default:
				m_pNpcList[i]->m_cBehavior = DEF_BEHAVIOR_MOVE;
				m_pNpcList[i]->m_cDir = 5;
				break;
			}

			m_pNpcList[i]->m_iFollowOwnerIndex = NULL;
			ZeroMemory(m_pNpcList[i]->m_cFollowOwnerCharName, sizeof(m_pNpcList[i]->m_cFollowOwnerCharName));
			m_pNpcList[i]->m_iTargetIndex = NULL;
			m_pNpcList[i]->m_cTurn = (rand() % 2);

			switch (m_pNpcList[i]->m_sType) {
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
				m_pNpcList[i]->m_sAppr2 = 0xF000;
				m_pNpcList[i]->m_sAppr2 = m_pNpcList[i]->m_sAppr2 | ((rand() % 13) << 4);
				m_pNpcList[i]->m_sAppr2 = m_pNpcList[i]->m_sAppr2 | (rand() % 9);
				break;

			case 36: // AGT-Aresden/AGT-Elvine
			case 37: // CGT-Aresden/CGT-Elvine
			case 38: // MS-Aresden/MS-Elvine
			case 39: // DT-Aresden/DT-Elvine
				m_pNpcList[i]->m_sAppr2 = 3;
				break;

			case 64: // Crop
				m_pNpcList[i]->m_sAppr2 = 1; // 1 bud; 2 grown; 3 large 
				break;

			default:
				m_pNpcList[i]->m_sAppr2 = 0;
				break;
			}
			m_pNpcList[i]->m_cMapIndex = (char)iMapIndex;
			m_pNpcList[i]->m_dwTime = timeGetTime() + (rand() % 10000);
			m_pNpcList[i]->m_dwActionTime += (rand() % 300);
			m_pNpcList[i]->m_dwMPupTime = timeGetTime();
			m_pNpcList[i]->m_dwHPupTime = m_pNpcList[i]->m_dwMPupTime;
			m_pNpcList[i]->m_sBehaviorTurnCount = 0;
			m_pNpcList[i]->m_bIsSummoned = bIsSummoned;
			m_pNpcList[i]->m_bIsMaster = bIsMaster;
			if (bIsSummoned == TRUE)
			{
				m_pNpcList[i]->m_dwSummonedTime = timeGetTime();
				m_pNpcList[i]->m_dwSummonWaitTime = 0;
			}
			if (bFirmBerserk == TRUE) {
				m_pNpcList[i]->m_cMagicEffectStatus[DEF_MAGICTYPE_BERSERK] = 1;
				m_pNpcList[i]->m_iStatus = m_pNpcList[i]->m_iStatus | 0x20;
				m_pNpcList[i]->m_iExpDiceMin = (m_pNpcList[i]->m_iExpDiceMin * 2); // new - expmin x 2 if npc is bFirmBerserk
				m_pNpcList[i]->m_iExpDiceMax = (m_pNpcList[i]->m_iExpDiceMax * 2); // new - expmax x 10 if npc is bFirmBerserk

			}
			if (cChangeSide != -1) m_pNpcList[i]->m_cSide = cChangeSide;
			m_pNpcList[i]->m_cBravery = (rand() % 3) + m_pNpcList[i]->m_iMinBravery;
			m_pNpcList[i]->m_iSpotMobIndex = iSpotMobIndex;
			m_pNpcList[i]->m_iGuildGUID = iGuildGUID;
			if (iGuildGUID != 0) {
				wsprintf(G_cTxt, "Summon War Unit(%d) GUID(%d)", m_pNpcList[i]->m_sType, iGuildGUID);
				PutLogList(G_cTxt);
			}
			// new - creates blocked spaces below 1x1, 2x2, and 3x3 monsters
			if (m_pNpcList[i]->m_sAreaSize == 0) {
				m_pMapList[iMapIndex]->SetOwner(i, DEF_OWNERTYPE_NPC, sX, sY);
			}
			else {
				m_pMapList[iMapIndex]->SetBigOwner(i, DEF_OWNERTYPE_NPC, sX, sY, m_pNpcList[i]->m_sAreaSize);
			}
			if ((m_bIsHeldenianMode == TRUE) // If any mob created as Helnian war not started,, ensure it's Neutral...
				&& (m_bHeldenianWarInitiated == FALSE)
				&& (m_pMapList[iMapIndex]->m_bIsHeldenianMap == TRUE)
				&& (m_pNpcList[i]->m_cSide > 2))
			{
				m_pNpcList[i]->m_cOriginalSide = m_pNpcList[i]->m_cSide;
				m_pNpcList[i]->m_cSide = 0;
			}
			m_pMapList[iMapIndex]->m_iTotalActiveObject++;
			m_pMapList[iMapIndex]->m_iTotalAliveObject++;
			switch (m_pNpcList[i]->m_sType) {
			case 36: // AGT-Aresden/AGT-Elvine
			case 37: // CGT-Aresden/CGT-Elvine
			case 38: // MS-Aresden/MS-Elvine
			case 39: // DT-Aresden/DT-Elvine
			case 42: // ManaStone
				m_pMapList[iMapIndex]->bAddCrusadeStructureInfo(m_pNpcList[i]->m_sType, sX, sY, m_pNpcList[i]->m_cSide);
				break;

			case 64:
				m_pMapList[iMapIndex]->bAddCropsTotalSum();
				break;
			}
			SendEventToNearClient_TypeA(i, DEF_OWNERTYPE_NPC, MSGID_EVENT_LOG, DEF_MSGTYPE_CONFIRM, NULL, NULL, NULL);
			return TRUE;
		}
	return FALSE;
}

/*********************************************************************************************************************
**  void CGame::NpcProcess()																						**
**  DESCRIPTION			:: manages Npc variables																	**
**  LAST_UPDATED		:: March 18, 2005; 6:30 PM; Hypnotoad														**
**	RETURN_VALUE		:: void																						**
**  NOTES				::	- new Crop DeleteNpc after specific amount of time										**
**	MODIFICATION		::	n/a																						**
**********************************************************************************************************************/
void CGame::NpcProcess()
{
	int i, iMaxHP;
	DWORD dwTime, dwActionTime;
	// SNOOPY: Poisonned npcs
	int iHPup = 0;

	dwTime = timeGetTime();
	for (i = 1; i < DEF_MAXNPCS; i++) {
		if (m_pNpcList[i] != NULL) {
			if (m_pNpcList[i]->m_cBehavior == DEF_BEHAVIOR_ATTACK) {
				switch (iDice(1, 7)) {
				case 1: dwActionTime = m_pNpcList[i]->m_dwActionTime; break;
				case 2: dwActionTime = m_pNpcList[i]->m_dwActionTime - 100; break;
				case 3: dwActionTime = m_pNpcList[i]->m_dwActionTime - 200; break;
				case 4: dwActionTime = m_pNpcList[i]->m_dwActionTime - 300; break;
				case 5: dwActionTime = m_pNpcList[i]->m_dwActionTime - 400; break;
				case 6: dwActionTime = m_pNpcList[i]->m_dwActionTime - 600; break;
				case 7: dwActionTime = m_pNpcList[i]->m_dwActionTime - 700; break;
				}
				if (dwActionTime < 600) dwActionTime = 600;
			}
			else dwActionTime = m_pNpcList[i]->m_dwActionTime;
			if (m_pNpcList[i]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] != 0)
				dwActionTime += (dwActionTime / 2);

			// SNOOPY: Added safety here
			if (m_pNpcList[i]->m_iHP <= 0) m_pNpcList[i]->m_cBehavior = DEF_BEHAVIOR_DEAD;
		}
		//SNOOPY Added here for ending Taming
		if ((m_pNpcList[i] != NULL)
			&& (m_pNpcList[i]->m_dwTamingTime != 0)
			&& (m_pNpcList[i]->m_cOriginalSide != m_pNpcList[i]->m_cSide)
			&& (dwTime > m_pNpcList[i]->m_dwTamingTime))
		{
			m_pNpcList[i]->m_cSide = m_pNpcList[i]->m_cOriginalSide;
			m_pNpcList[i]->m_dwTamingTime = 0;
			m_pNpcList[i]->m_cMoveType = DEF_MOVETYPE_RANDOMWAYPOINT;
			SendEventToNearClient_TypeA(i, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, NULL, NULL, NULL);
		}
		if ((m_pNpcList[i] != NULL) && ((dwTime - m_pNpcList[i]->m_dwTime) > dwActionTime)) {
			m_pNpcList[i]->m_dwTime = dwTime;
			if (abs(m_pNpcList[i]->m_cMagicLevel) > 0) {
				if ((dwTime - m_pNpcList[i]->m_dwMPupTime) > DEF_MPUPTIME) {
					m_pNpcList[i]->m_dwMPupTime = dwTime;
					m_pNpcList[i]->m_iMana += iDice(1, (m_pNpcList[i]->m_iMaxMana / 5));

					if (m_pNpcList[i]->m_iMana > m_pNpcList[i]->m_iMaxMana)
						m_pNpcList[i]->m_iMana = m_pNpcList[i]->m_iMaxMana;
				}
			}
			// HP up
			if (((dwTime - m_pNpcList[i]->m_dwHPupTime) > DEF_HPUPTIME) && (m_pNpcList[i]->m_bIsKilled == FALSE))
			{
				m_pNpcList[i]->m_dwHPupTime = dwTime;
				iMaxHP = iDice(m_pNpcList[i]->m_iHitDice, 8) + m_pNpcList[i]->m_iHitDice;
				if ((m_pNpcList[i]->m_iHP < iMaxHP)
					&& (m_pNpcList[i]->m_cActionLimit != 5) && (m_pNpcList[i]->m_cActionLimit != 8)) // Building do not regenerate.					
				{
					if (m_pNpcList[i]->m_bIsSummoned == FALSE)
					{
						iHPup = iDice(1, m_pNpcList[i]->m_iHitDice);
					}
					// SNOOPY: Added poison support for npcs...
					if (m_pNpcList[i]->m_iPoisonLevel != 0)
					{
						iHPup -= m_pNpcList[i]->m_iPoisonLevel;
						if (iHPup < 0) iHPup = 0;
						if (bCheckResistingPoisonSuccess(i, DEF_OWNERTYPE_NPC) == TRUE)
						{
							m_pNpcList[i]->m_iPoisonLevel = 0;
							SetPoisonFlag(i, DEF_OWNERTYPE_NPC, FALSE);
						}
					}
					m_pNpcList[i]->m_iHP += iHPup;
					if (m_pNpcList[i]->m_iHP > iMaxHP) m_pNpcList[i]->m_iHP = iMaxHP;
					if (m_pNpcList[i]->m_iHP <= 0)     m_pNpcList[i]->m_iHP = 1;
				}
			}
			switch (m_pNpcList[i]->m_cBehavior) {
			case DEF_BEHAVIOR_DEAD:
				NpcBehavior_Dead(i);
				break;
			case DEF_BEHAVIOR_STOP:
				NpcBehavior_Stop(i);
				break;
			case DEF_BEHAVIOR_MOVE:
				NpcBehavior_Move(i);
				break;
			case DEF_BEHAVIOR_ATTACK:
				NpcBehavior_Attack(i);
				break;
			case DEF_BEHAVIOR_FLEE:
				NpcBehavior_Flee(i);
				break;
			}
			//SNOOPY Added here for ending waiting for Tamed mobs
			if ((m_pNpcList[i] != NULL) && (m_pNpcList[i]->m_iHP > 0)
				&& (m_pNpcList[i]->m_dwTamingTime != 0)
				&& (m_pNpcList[i]->m_cFollowOwnerType == DEF_OWNERTYPE_PLAYER_WAITING)
				&& ((dwTime - m_pNpcList[i]->m_dwSummonWaitTime) > 1000 * 60)) // Wait 60sec for the character to connect again
			{
				m_pNpcList[i]->m_cFollowOwnerType = 0;
				m_pNpcList[i]->m_iSummonControlMode = 0;
			}
			// !!! m_pNpcList
			if ((m_pNpcList[i] != NULL) && (m_pNpcList[i]->m_iHP > 0) && (m_pNpcList[i]->m_bIsSummoned == TRUE))
			{
				switch (m_pNpcList[i]->m_sType) {
				case 29: // Ogre summon popo:10 min
				case 33: // WW summon popo		
				case 31: // Demon summon popo
					if ((dwTime - m_pNpcList[i]->m_dwSummonedTime) > 1000 * 60 * 10)
						NpcKilledHandler(NULL, NULL, i, 0);
					else if ((m_pNpcList[i]->m_cFollowOwnerType == DEF_OWNERTYPE_PLAYER_WAITING)
						&& ((dwTime - m_pNpcList[i]->m_dwSummonWaitTime) > 1000 * 60)) // Wait 60sec for the character to connect again
					{
						m_pNpcList[i]->m_cFollowOwnerType = 0;
						m_pNpcList[i]->m_iSummonControlMode = 0;
					}
					break;
				case 43: // LWB
				case 44: // GHK
				case 45: // GHKABS
				case 46: // TK
				case 47: // BG Crusade summons 10 min
					if ((dwTime - m_pNpcList[i]->m_dwSummonedTime) > 1000 * 60 * 10)
						NpcKilledHandler(NULL, NULL, i, 0);
					else if ((m_pNpcList[i]->m_cFollowOwnerType == DEF_OWNERTYPE_PLAYER_WAITING)
						&& ((dwTime - m_pNpcList[i]->m_dwSummonWaitTime) > 1000 * 60)) // Wait 60sec for the character to connect again
					{
						m_pNpcList[i]->m_cFollowOwnerType = 0;
						m_pNpcList[i]->m_iSummonControlMode = 0;
					}
					break;
				case 82: // Sorceress	
				case 83: // ATK	
				case 84: // MasterElf	
				case 85: // DSK	
				case 86: // HBT		
				case 88: // Barbarian
					if ((m_bIsHeldenianMode == TRUE)
						&& (m_pMapList[m_pNpcList[i]->m_cMapIndex] != NULL)
						&& (m_pMapList[m_pNpcList[i]->m_cMapIndex]->m_bIsHeldenianMap == 1))
					{
						if ((m_pNpcList[i]->m_cFollowOwnerType == DEF_OWNERTYPE_PLAYER_WAITING)
							&& ((dwTime - m_pNpcList[i]->m_dwSummonWaitTime) > 1000 * 60)) // Wait 60sec for the character to connect again
						{
							switch (m_cHeldenianSummonMode) {
							case 0: // Master has not connected again, remove summon
								NpcKilledHandler(NULL, NULL, i, 0);
								break;
							case 1: // Master has not connected again, wait forever			
								m_pNpcList[i]->m_iSummonControlMode = 0; // Un"/hold"
								break;
							case 2: // Master has not connected again, available for others to take over	
								m_pNpcList[i]->m_cFollowOwnerType = DEF_OWNERTYPE_PLAYER_FREE;
								m_pNpcList[i]->m_iSummonControlMode = 0; // Un"/hold"
								break;
							}
						}
						else break; // Keep them alive					
					}
					else if ((dwTime - m_pNpcList[i]->m_dwSummonedTime) > 1000 * 60 * 15)
						NpcKilledHandler(NULL, NULL, i, 0);		// Heldenian soldiars limited to 15min if not Heldenian
					break;
				case 36: // AGT         Those will last forever when summonned
				case 37: // AGC
				case 38: // MS
				case 39: // DT
				case 40: // ESG
				case 41: // ManaStone
				case 87: // CT
				case 89: // AGC
				case 91: // gates
					if ((m_pNpcList[i]->m_cFollowOwnerType == DEF_OWNERTYPE_PLAYER_WAITING)
						&& ((dwTime - m_pNpcList[i]->m_dwSummonWaitTime) > 1000 * 60)) // Wait 60sec for the character to connect again
					{
						m_pNpcList[i]->m_cFollowOwnerType = 0;
						m_pNpcList[i]->m_iSummonControlMode = 0;
					}
					break;
				default: //other summons: 5 minutes
					if ((dwTime - m_pNpcList[i]->m_dwSummonedTime) > DEF_SUMMONTIME)
						NpcKilledHandler(NULL, NULL, i, 0);
					else if ((m_pNpcList[i]->m_cFollowOwnerType == DEF_OWNERTYPE_PLAYER_WAITING)
						&& ((dwTime - m_pNpcList[i]->m_dwSummonWaitTime) > 1000 * 60)) // Wait 60sec for the character to connect again
					{
						m_pNpcList[i]->m_cFollowOwnerType = 0;
						m_pNpcList[i]->m_iSummonControlMode = 0;
					}
					break;
				}
			}
		}
	}
}

int CGame::iGetDangerValue(int iNpcH, short dX, short dY)
{
	int ix, iy, iDangerValue;
	short sOwner, sDOType;
	char  cOwnerType;
	DWORD dwRegisterTime;

	if (m_pNpcList[iNpcH] == NULL) return FALSE;

	iDangerValue = 0;

	for (ix = dX - 2; ix <= dX + 2; ix++)
		for (iy = dY - 2; iy <= dY + 2; iy++) {
			m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetOwner(&sOwner, &cOwnerType, ix, iy);
			m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->bGetDynamicObject(ix, iy, &sDOType, &dwRegisterTime);

			if (sDOType == 1) iDangerValue++;

			switch (cOwnerType) {
			case NULL:
				break;
			case DEF_OWNERTYPE_PLAYER:
				if (m_pClientList[sOwner] == NULL) break;
				if (m_pNpcList[iNpcH]->m_cSide != m_pClientList[sOwner]->m_cSide)
					iDangerValue++;
				else iDangerValue--;
				break;
			case DEF_OWNERTYPE_NPC:
				if (m_pNpcList[sOwner] == NULL) break;
				if (m_pNpcList[iNpcH]->m_cSide != m_pNpcList[sOwner]->m_cSide)
					iDangerValue++;
				else iDangerValue--;
				break;
			}
		}

	return iDangerValue;
}

// 3.51 - 05/17/2004 - Hypnotoad/[KLKS] - Monster Special Abilities
char CGame::_cGetSpecialAbility(int iKindSA)
{
	char cSA;

	switch (iKindSA) {
	case 1:
		// Slime, Orc, Orge, WereWolf, YB-, Rabbit, Mountain-Giant, Stalker, Hellclaw, 
		// Wyvern, Fire-Wyvern, Barlog, Tentocle, Centaurus, Giant-Lizard, Minotaurus,
		// Abaddon, Claw-Turtle, Giant-Cray-Fish, Giant-Plant, MasterMage-Orc, Nizie,
		// Tigerworm
		switch (iDice(1, 2)) {
		case 1: cSA = 3; break; // Anti-Physical Damage
		case 2: cSA = 4; break; // Anti-Magic Damage
		}
		break;

	case 2:
		// Giant-Ant, Cat, Giant-Frog, 
		switch (iDice(1, 3)) {
		case 1: cSA = 3; break; // Anti-Physical Damage
		case 2: cSA = 4; break; // Anti-Magic Damage
		case 3: cSA = 5; break; // Poisonous
		}
		break;

	case 3:
		// Zombie, Scorpion, Amphis, Troll, Dark-Elf
		switch (iDice(1, 4)) {
		case 1: cSA = 3; break; // Anti-Physical Damage
		case 2: cSA = 4; break; // Anti-Magic Damage
		case 3: cSA = 5; break; // Poisonous
		case 4: cSA = 6; break; // Critical Poisonous
		}
		break;

	case 4:
		// no linked Npc
		switch (iDice(1, 3)) {
		case 1: cSA = 3; break; // Anti-Physical Damage
		case 2: cSA = 4; break; // Anti-Magic Damage
		case 3: cSA = 7; break; // Explosive
		}
		break;

	case 5:
		// Stone-Golem, Clay-Golem, Beholder, Cannibal-Plant, Rudolph, DireBoar
		switch (iDice(1, 4)) {
		case 1: cSA = 3; break; // Anti-Physical Damage
		case 2: cSA = 4; break; // Anti-Magic Damage
		case 3: cSA = 7; break; // Explosive
		case 4: cSA = 8; break; // Critical-Explosive
		}
		break;

	case 6:
		// no linked Npc
		switch (iDice(1, 3)) {
		case 1: cSA = 3; break; // Anti-Physical Damage
		case 2: cSA = 4; break; // Anti-Magic Damage
		case 3: cSA = 5; break; // Poisonous
		}
		break;

	case 7:
		// Orc-Mage, Unicorn
		switch (iDice(1, 3)) {
		case 1: cSA = 1; break; // Clairvoyant
		case 2: cSA = 2; break; // Distruction of Magic Protection
		case 3: cSA = 4; break; // Anti-Magic Damage
		}
		break;

	case 8:
		// Frost, Ice-Golem, Ettin, Gagoyle, Demon, Liche, Hellbound, Cyclops, 
		// Skeleton
		switch (iDice(1, 5)) {
		case 1: cSA = 1; break; // Clairvoyant
		case 2: cSA = 2; break; // Distruction of Magic Protection
		case 3: cSA = 4; break; // Anti-Magic Damage
		case 4: cSA = 3; break; // Anti-Physical Damage
		case 5: cSA = 8; break; // Critical-Explosive
		}
		break;

	case 9:
		// no linked Npc
		cSA = iDice(1, 8); // All abilities available
		break;

	case 10: // centu - guards
		cSA = iDice(1, 2);
		break;
	}

	return cSA;
}

//Magn0S:: Coded Drop Manager from .cfg
bool CGame::bDecodeDropManagerFile(char* pFn)
{
	FILE* pFile;
	HANDLE hFile;
	char* cp, * token, cReadModeA;
	char seps[] = "= \t\n";
	int i = 0, j = 0, k = 0, f = 0, o = 0;
	char cTxt[250];

	DWORD  dwFileSize;
	class CStrTok* pStrTok;

	cReadModeA = 0;

	hFile = CreateFile(pFn, GENERIC_READ, NULL, NULL, OPEN_EXISTING, NULL, NULL);
	dwFileSize = GetFileSize(hFile, NULL);
	if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);

	pFile = fopen(pFn, "rt");
	if (pFile == NULL) {

		PutLogList("(!) Cannot open Drop Manager file.");
		return false;
	}
	else {
		PutLogList("(!) Reading DropManager file...");
		cp = new char[dwFileSize + 2];
		ZeroMemory(cp, dwFileSize + 2);
		fread(cp, dwFileSize, 1, pFile);

		pStrTok = new class CStrTok(cp, seps);
		token = pStrTok->pGet();

		while (token) {
			if (cReadModeA != 0) {
				switch (cReadModeA) {
				case 1:
					if (_bGetIsStringIsNumber(token) == false)
					{	PutLogList("(!!!) CRITICAL ERROR! Max Armor Drop Prob. config. file error - Wrong Data format.");
						delete[] cp; return false;
					}

					m_iMaxArmorDrop = atoi(token);
					wsprintf(cTxt, "(*) iMaxArmorProbability drop rate: (%d)", m_iMaxArmorDrop);
					PutLogList(cTxt);
					cReadModeA = 0;
					break;

				case 2:
					if (_bGetIsStringIsNumber(token) == false)
					{	PutLogList("(!!!) CRITICAL ERROR! ArmorDrop configuration file error - Wrong Data format.");
						delete[] cp; return false;
					}
					if (i > 12) {
						PutLogList("(!) WARNING (A)! Too many armor probability added!");
						return true;
					}
					m_iArmorDrop[i] = atoi(token);
					wsprintf(cTxt, "(*) iArmorProb[%d] drop rate: (%d)", i, m_iArmorDrop[i]);
					PutLogList(cTxt);
					i++;
					cReadModeA = 0;
					break;
				//-------------------------------------------------------------------------------------------------------------
				case 3:
					if (_bGetIsStringIsNumber(token) == false)
					{	PutLogList("(!!!) CRITICAL ERROR (B)! Max Weapon Drop Prob. config. file error - Wrong Data format.");
						delete[] cp; return false;
					}
					m_iMaxWeaponDrop = atoi(token);
					wsprintf(cTxt, "(*) iMaxWeaponProbability drop rate: (%d)", m_iMaxWeaponDrop);
					PutLogList(cTxt);
					cReadModeA = 0;
					break;

				case 4:
					if (_bGetIsStringIsNumber(token) == false)
					{	PutLogList("(!!!) CRITICAL ERROR (B)! WeaponDrop configuration file error - Wrong Data format.");
						delete[] cp; return false;
					}

					if (j > 12) {
						PutLogList("(!) WARNING (B)! Too many weapon probability added!");
						return true;
					}
					m_iWeaponDrop[j] = atoi(token);
					wsprintf(cTxt, "(*) iWeaponProb[%d] drop rate: (%d)", j, m_iWeaponDrop[j]);
					PutLogList(cTxt);
					j++;
					cReadModeA = 0;
					break;
				//===========================================================================================================================
				case 5:
					if (_bGetIsStringIsNumber(token) == false)
					{	PutLogList("(!!!) CRITICAL ERROR (C)! Max Drop Stated Armor Prob. config. file error - Wrong Data format.");
						delete[] cp; return false;
					}
					m_iMaxStatedArmor = atoi(token);
					wsprintf(cTxt, "(*) m_iMaxStatedArmor drop rate: (%d)", m_iMaxStatedArmor);
					PutLogList(cTxt);
					cReadModeA = 0;
					break;

				case 6:
					if (_bGetIsStringIsNumber(token) == false)
					{	PutLogList("(!!!) CRITICAL ERROR (C)! Armor Stated Drop configuration file error - Wrong Data format.");
						delete[] cp; return false;
					}

					if (k > 8) {
						PutLogList("(!) WARNING (C)! Too many stated armor probability added!");
						return true;
					}
					m_iStatedArmorDrop[k] = atoi(token);
					wsprintf(cTxt, "(*) iStatedArmor[%d] drop rate: (%d)", k, m_iStatedArmorDrop[k]);
					PutLogList(cTxt);
					k++;
					cReadModeA = 0;
					break;
				//===========================================================================================================================
				case 7:
					if (_bGetIsStringIsNumber(token) == false)
					{	PutLogList("(!!!) CRITICAL ERROR (D)! Attr Weapon Drop Prob. config. file error - Wrong Data format.");
						delete[] cp; return false;
					}
					m_iMaxAttrWeaponDrop = atoi(token);
					wsprintf(cTxt, "(*) m_iMaxAttrWeaponDrop rate: (%d)", m_iMaxAttrWeaponDrop);
					PutLogList(cTxt);
					cReadModeA = 0;
					break;

				case 8:
					if (_bGetIsStringIsNumber(token) == false)
					{	PutLogList("(!!!) CRITICAL ERROR (D)! Attr Weapon Drop configuration file error - Wrong Data format.");
						delete[] cp; return false;
					}

					if (f > 9) {
						PutLogList("(!) WARNING (D)! Too many attr weapon probability added!");
						return true;
					}
					m_iAttrWeaponDrop[f] = atoi(token);
					wsprintf(cTxt, "(*) iAttrWeaponDrop[%d] drop rate: (%d)", f, m_iAttrWeaponDrop[f]);
					PutLogList(cTxt);
					f++;
					cReadModeA = 0;
					break;
				//===========================================================================================================================
				case 9:
					if (_bGetIsStringIsNumber(token) == false)
					{	PutLogList("(!!!) CRITICAL ERROR (E)! Max Drop Stated Weapon Prob. config. file error - Wrong Data format.");
						delete[] cp; return false;
					}
					m_iMaxStatedWeapon = atoi(token);
					wsprintf(cTxt, "(*) m_iMaxStatedWeapon rate: (%d)", m_iMaxStatedWeapon);
					PutLogList(cTxt);
					cReadModeA = 0;
					break;

				case 10:
					if (_bGetIsStringIsNumber(token) == false)
					{	PutLogList("(!!!) CRITICAL ERROR (E)! Armor Stated Drop configuration file error - Wrong Data format.");
						delete[] cp; return false;
					}

					if (o > 3) {
						PutLogList("(!) WARNING (E)! Too many stated weapon probability added!");
						return true;
					}
					m_iStatedWeaponDrop[o] = atoi(token);
					wsprintf(cTxt, "(*) iStatedWeaponDrop[%d] drop rate: (%d)", o, m_iStatedWeaponDrop[o]);
					PutLogList(cTxt);
					o++;
					cReadModeA = 0;
					break;
				}
			}
			else {
				if (memcmp(token, "max-armor-prob", 14) == 0) {
					cReadModeA = 1;
				}
				if (memcmp(token, "p_armor", 7) == 0) {
					cReadModeA = 2;
				}
				if (memcmp(token, "max-weapon-prob", 15) == 0) {
					cReadModeA = 3;
				}
				if (memcmp(token, "p_weapon", 8) == 0) {
					cReadModeA = 4;
				}
				if (memcmp(token, "max-stated-armor", 16) == 0) {
					cReadModeA = 5;
				}
				if (memcmp(token, "stated-armor", 12) == 0) {
					cReadModeA = 6;
				}
				if (memcmp(token, "max-attr-weapon", 15) == 0) {
					cReadModeA = 7;
				}
				if (memcmp(token, "attr-weapon", 11) == 0) {
					cReadModeA = 8;
				}
				if (memcmp(token, "max-stated-weapon", 17) == 0) {
					cReadModeA = 9;
				}
				if (memcmp(token, "stated-weapon", 13) == 0) {
					cReadModeA = 10;
				}
			}

			token = pStrTok->pGet();
		}
		delete[] cp;
	}
	if (pFile) fclose(pFile);

	return true;
}