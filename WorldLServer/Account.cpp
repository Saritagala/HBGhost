//////////////////////////////////////////////////////////////////////
//
// Account.cpp: implementation of the Account class
//
//////////////////////////////////////////////////////////////////////

#include "Account.h"

#pragma warning (disable : 4996)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAccount::CAccount(char * pData, DWORD dwTime)
{

	ZeroMemory(cAccountName, sizeof(cAccountName));			
	strcpy(cAccountName, pData);
	ZeroMemory(cPassword,sizeof(cPassword));
	cAccountType = 0;										
	dwLogInTime = dwTime;									
	m_iLevel = 0;											
	ZeroMemory(m_cOnGameServer, sizeof(m_cOnGameServer));
	Timeout = -1;

}

CAccount::~CAccount()
{

}