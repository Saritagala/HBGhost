// PartyManager.h: interface for the PartyManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARTYMANAGER_H__ED26235F_8BA2_493A_AF52_397C25D4CF5C__INCLUDED_)
#define AFX_PARTYMANAGER_H__ED26235F_8BA2_493A_AF52_397C25D4CF5C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include "GateCore.h"

#define DEF_MAXPARTY		5000
#define DEF_MAXPARTYMEMBER	8


class PartyManager  
{
public:
	void CheckMemberActivity();
	void SetServerChangeStatus(char * pName, int iPartyID);
	void GameServerDown(int iClientH);
	bool bGetPartyInfo(int iClientH, int iGSCH, char * pName, int iPartyID);
	bool bCheckPartyMember(int iClientH, int iGSCH, int iPartyID, char * pName);
	bool bRemoveMember(int iPartyID, char *pMemberName);
	bool bAddMember(int iClientH, int iPartyID, char * pMemberName);
	bool bDeleteParty(int iPartyID);
	int iCreateNewParty(int iClientH, char * pMasterName);
	PartyManager(class CGateCore * pGateCore);
	virtual ~PartyManager();

	int m_iMemberNumList[DEF_MAXPARTY];

	struct {
		int  m_iPartyID, m_iIndex;
		char m_cName[12];
		UINT32 m_dwServerChangeTime;
	} m_stMemberNameList[DEF_MAXPARTY];

	class CGateCore * m_pGateCore;
	UINT32 m_dwCheckMemberActTime;
};

#endif // !defined(AFX_PARTYMANAGER_H__ED26235F_8BA2_493A_AF52_397C25D4CF5C__INCLUDED_)
