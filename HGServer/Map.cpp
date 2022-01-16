// Map.cpp: implementation of the CMap class.
//
//////////////////////////////////////////////////////////////////////

#include "Map.h"
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

#pragma warning (disable : 4996 4018 6031 6001)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMap::CMap(class CGame * pGame)
: m_bIsSnowEnabled(false)
{
 int i;

	for (i = 0; i < DEF_MAXTELEPORTLOC; i++)
		m_pTeleportLoc[i] = 0;

	for (i = 0; i < DEF_MAXWAYPOINTCFG; i++) {
		m_WaypointList[i].x = -1;
		m_WaypointList[i].y = -1;
	}

	for (i = 0; i < DEF_MAXMGAR; i++) {
		m_rcMobGenAvoidRect[i].top  = -1;
		m_rcMobGenAvoidRect[i].left = -1;
	}

	for (i = 0; i < DEF_MAXNMR; i++) {
		m_rcNoAttackRect[i].top  = -1;
		m_rcNoAttackRect[i].left = -1;
	}

	for (i = 0; i < DEF_MAXSPOTMOBGENERATOR; i++) {
		m_stSpotMobGenerator[i].bDefined = false;
		m_stSpotMobGenerator[i].iTotalActiveMob = 0;
	}

	for (i = 0; i < DEF_MAXFISHPOINT; i++) {
		m_FishPointList[i].x = -1;
		m_FishPointList[i].y = -1;
	}

	for (i = 0; i < DEF_MAXMINERALPOINT; i++) {
		m_MineralPointList[i].x = -1;
		m_MineralPointList[i].y = -1;
	}

	for (i = 0; i < DEF_MAXINITIALPOINT; i++) {
		m_pInitialPoint[i].x = -1;
		m_pInitialPoint[i].y = -1;
	}
 
	for (i = 0; i < 15000; i++)
		m_bNamingValueUsingStatus[i] = false; //LifeX Fix Mobs Respawn Limit 01/01

	for (i = 0; i < DEF_MAXOCCUPYFLAG; i++)
		m_pOccupyFlag[i] = 0;

	for (i = 0; i < DEF_MAXSTRATEGICPOINTS; i++)
		m_pStrategicPointList[i] = 0;

	for (i = 0; i < DEF_MAXENERGYSPHERES; i++) {
		m_stEnergySphereCreationList[i].cType = 0;
		m_stEnergySphereGoalList[i].cResult   = 0;
	}

	m_cThunder = 0;
	
	m_bIsHeldenianMap = false;
	m_iTotalActiveObject = 0;
	m_iTotalAliveObject  = 0;
	m_iTotalItemEvents = 0;
	sMobEventAmount = 15;

	m_bIsFixedDayMode = false;

	m_iTotalFishPoint = 0;
	m_iMaxFish = 0;
	m_iCurFish = 0;

	m_iTotalMineralPoint = 0;
	m_iMaxMineral = 0;
	m_iCurMineral = 0;

	m_pTile = 0;

	m_cWhetherStatus = 0;
	m_cType          = DEF_MAPTYPE_NORMAL;

	m_pGame = pGame;
	m_iApocalypseMobGenType = 0;
	m_iLevelLimit = 0;
	m_iUpperLevelLimit = 0; // v1.4
	m_bMineralGenerator = false;

	m_iTotalOccupyFlags = 0;
	
	m_bIsAttackEnabled = true;
	m_cRandomMobGeneratorLevel = 0;

	m_bIsFightZone = false;
	
	m_iTotalEnergySphereCreationPoint = 0;
	m_iTotalEnergySphereGoalPoint = 0;

	m_bIsEnergySphereGoalEnabled = false;
	m_iCurEnergySphereGoalPointIndex = -1;

	for (int ix = 0; ix < DEF_MAXSECTORS; ix++)
		for (int iy = 0; iy < DEF_MAXSECTORS; iy++) {
			m_stSectorInfo[ix][iy].iNeutralActivity = 0;
			m_stSectorInfo[ix][iy].iAresdenActivity = 0;
			m_stSectorInfo[ix][iy].iElvineActivity = 0;
			m_stSectorInfo[ix][iy].iMonsterActivity = 0;
			m_stSectorInfo[ix][iy].iPlayerActivity = 0;

			m_stTempSectorInfo[ix][iy].iNeutralActivity = 0;
			m_stTempSectorInfo[ix][iy].iAresdenActivity = 0;
			m_stTempSectorInfo[ix][iy].iElvineActivity = 0;
			m_stTempSectorInfo[ix][iy].iMonsterActivity = 0;
			m_stTempSectorInfo[ix][iy].iPlayerActivity = 0;
		}

	m_iMaxNx = m_iMaxNy = m_iMaxAx = m_iMaxAy = m_iMaxEx = m_iMaxEy = m_iMaxMx = m_iMaxMy = m_iMaxPx = m_iMaxPy = 0;


	for (i = 0; i < DEF_MAXHELDENIANDOOR; i++) {
		m_stHeldenianGateDoor[i].cDir = 0;
		m_stHeldenianGateDoor[i].dX = 0;
		m_stHeldenianGateDoor[i].dY = 0;
	}

	for (i = 0; i < DEF_MAXHELDENIANTOWER; i++) {
		m_stHeldenianTower[i].sTypeID = 0;
		m_stHeldenianTower[i].dX = 0;
		m_stHeldenianTower[i].dY = 0;
		m_stHeldenianTower[i].cSide = 0;
	}
	
	for (i = 0; i < DEF_MAXSTRIKEPOINTS; i++) {
		m_stStrikePoint[i].dX  = 0;
		m_stStrikePoint[i].dY  = 0;
		m_stStrikePoint[i].iHP = 0;
		m_stStrikePoint[i].iMapIndex = -1;
		ZeroMemory(m_stStrikePoint[i].cRelatedMapName, sizeof(m_stStrikePoint[i].cRelatedMapName));
	}
	m_iTotalStrikePoints = 0;
	m_bIsDisabled = false;

	for (i = 0; i < DEF_MAXCRUSADESTRUCTURES; i++) {
		m_stCrusadeStructureInfo[i].cType = 0;
		m_stCrusadeStructureInfo[i].cSide = 0;
		m_stCrusadeStructureInfo[i].sX = 0;
		m_stCrusadeStructureInfo[i].sY = 0;
	}
	m_iTotalCrusadeStructures = 0;
	m_iTotalAgriculture = 0;
	m_bIsGateAvailable = false;
	m_bIsApocalypseMobSpawn = false;
	m_iApocalypseMobGenType = 0;
	m_iApocalypseBossMobNpcID = 0;
	m_cDynamicGateType = 0;
	m_bIsCitizenLimit = 0;
}

CMap::~CMap()
{
	
 int i;
	
	if (m_pTile != 0) 
		delete[] m_pTile;	
 	
	for (i = 0; i < DEF_MAXTELEPORTLOC; i++)
		if (m_pTeleportLoc[i] != 0) delete m_pTeleportLoc[i];

	for (i = 0; i < DEF_MAXOCCUPYFLAG; i++)
		if (m_pOccupyFlag[i] != 0) delete m_pOccupyFlag[i];

	for (i = 0; i < DEF_MAXSTRATEGICPOINTS; i++)
		if (m_pStrategicPointList[i] != 0) delete m_pStrategicPointList[i];
}

void CMap::SetOwner(short sOwner, char cOwnerClass, short sX, short sY)
{
 class CTile * pTile;	
	
	if ((sX < 0) || (sX >= m_sSizeX) || (sY < 0) || (sY >= m_sSizeY)) return;
	
	pTile = (class CTile *)(m_pTile + sX + sY*m_sSizeY);
	pTile->m_sOwner      = sOwner;
	pTile->m_cOwnerClass = cOwnerClass;
}

char _tmp_cMoveDirX[9] = { 0,0,1,1,1,0,-1,-1,-1 };
char _tmp_cMoveDirY[9] = { 0,-1,-1,0,1,1,1,0,-1 };

bool CMap::bCheckFlySpaceAvailable(short sX, short sY, char cDir, short sOwner)
{
 class CTile * pTile;
 short dX, dY;

	if ((cDir <= 0) || (cDir > 8)) return 0;
	dX = _tmp_cMoveDirX[cDir] + sX;
	dY = _tmp_cMoveDirY[cDir] + sY;
	if ((dX < 20) || (dX >= m_sSizeX - 20) || (dY < 20) || (dY >= m_sSizeY - 20)) return 0;
	pTile = (class CTile *)(m_pTile + sX + sY*m_sSizeY);
	if (pTile->m_sOwner != 0) return 0;
	pTile->m_sOwner = sOwner;
	return 1;
}

void CMap::SetDeadOwner(short sOwner, char cOwnerClass, short sX, short sY)
{
 class CTile * pTile;	
	
	if ((sX < 0) || (sX >= m_sSizeX) || (sY < 0) || (sY >= m_sSizeY)) return;
 
	pTile = (class CTile *)(m_pTile + sX + sY*m_sSizeY);
	pTile->m_sDeadOwner      = sOwner;
	pTile->m_cDeadOwnerClass = cOwnerClass;
}


/*********************************************************************************************************************
**  void CMap::GetOwner(short * pOwner, char * pOwnerClass, short sX, short sY)										**
**  description			:: check if the tile contains a player														**
**  last updated		:: November 17, 2004; 10:48 PM; Hypnotoad													**
**	return value		:: void																						**
**  commentary			::	-	added check to see if owner is class 1 or if is greater than max clients 			**
**********************************************************************************************************************/
void CMap::GetOwner(short * pOwner, char * pOwnerClass, short sX, short sY)
{
 class CTile * pTile;	

	if ((sX < 0) || (sX >= m_sSizeX) || (sY < 0) || (sY >= m_sSizeY)) {
		*pOwner      = 0;	
		*pOwnerClass = 0;
		return;	
	}

	pTile = (class CTile *)(m_pTile + sX + sY*m_sSizeY);
	*pOwner      = pTile->m_sOwner;
	*pOwnerClass = pTile->m_cOwnerClass;
	
	if ((*pOwnerClass == 1) && (*pOwner > DEF_MAXCLIENTS)) {
		*pOwner      = 0;	
		*pOwnerClass = 0;
		return;	
	}
	
	if (pTile->m_sOwner == 0) *pOwnerClass = 0;
}

/*********************************************************************************************************************
**  void CMap::GetDeadOwner(short * pOwner, char * pOwnerClass, short sX, short sY)									**
**  description			:: check if the tile contains a dead player													**
**  last updated		:: November 20, 2004; 9:13 PM; Hypnotoad													**
**	return value		:: void																						**
**********************************************************************************************************************/
void CMap::GetDeadOwner(short * pOwner, char * pOwnerClass, short sX, short sY)
{
 class CTile * pTile;	

	if ((sX < 0) || (sX >= m_sSizeX) || (sY < 0) || (sY >= m_sSizeY)) {
		*pOwner      = 0;	
		*pOwnerClass = 0;
		return;	
	}

	pTile = (class CTile *)(m_pTile + sX + sY*m_sSizeY);
	*pOwner      = pTile->m_sDeadOwner;
	*pOwnerClass = pTile->m_cDeadOwnerClass;
}
 								  
bool CMap::bGetMoveable(short dX, short dY, short * pDOtype)
{
 class CTile * pTile;	
	
	if ((dX < 20) || (dX >= m_sSizeX - 20) || (dY < 20) || (dY >= m_sSizeY - 20)) return false;
	pTile = (class CTile *)(m_pTile + dX + dY*m_sSizeY);
	
	if (pDOtype != 0) *pDOtype = pTile->m_sDynamicObjectType;

	if (pTile->m_sOwner != 0) return false;
	if (pTile->m_bIsMoveAllowed == false) return false;
	if (pTile->m_bIsTempMoveAllowed == false) return false;
	
	return true;
}

bool CMap::bGetMoveableArea(short sOwnerH, short dX, short dY)
{
 class CTile * pTile;	
	
	if ((dX < 20) || (dX >= m_sSizeX - 20) || (dY < 20) || (dY >= m_sSizeY - 20)) return false;
	pTile = (class CTile *)(m_pTile + dX + dY*m_sSizeY);
	
	if ((pTile->m_sOwner == 0) && (pTile->m_sOwner == sOwnerH)) return false;
	if (pTile->m_bIsMoveAllowed == false) return false;
	if (pTile->m_bIsTempMoveAllowed == false) return false;
	return true;
}

bool CMap::bGetIsMoveAllowedTile(short dX, short dY)
{
 class CTile * pTile;	
	
	if ((dX < 20) || (dX >= m_sSizeX - 20) || (dY < 20) || (dY >= m_sSizeY - 20)) return false;

	pTile = (class CTile *)(m_pTile + dX + dY*m_sSizeY);
	
	if (pTile->m_bIsMoveAllowed == false) return false;
	if (pTile->m_bIsTempMoveAllowed == false) return false;
	
	return true;
}

bool CMap::bApocalypseGateTeleporter(short dX, short dY, char * cMapName, short * tX, short * tY)
{
 class CTile * pTile;	
	
	if (m_bIsGateAvailable == 0) return false;
	if ((dX < 14) || (dX >= m_sSizeX - 16) || (dY < 12) || (dY >= m_sSizeY - 14)) return false;

	pTile = (class CTile *)(m_pTile + dX + dY*m_sSizeY);
	if (pTile->m_bIsApocalypseGate == true) {
		if (cMapName != 0) {
			memcpy(cMapName, m_cDynamicGateDestMap, 10); 
		}
		if (tX != 0) {
			tX = (short *)m_sDynamicGateTgtX;
		}
		if (tY != 0) {
			tY = (short *)m_sDynamicGateTgtY;
		}
		return true;
	}
	return false;
}

bool CMap::bGetIsTeleport(short dX, short dY)
{
 class CTile * pTile;	
	
	if ((dX < 14) || (dX >= m_sSizeX - 16) || (dY < 12) || (dY >= m_sSizeY - 14)) return false;

	pTile = (class CTile *)(m_pTile + dX + dY*m_sSizeY);
	
	if (pTile->m_bIsTeleport == false) return false;
	
	return true;
}

void CMap::ClearOwner(int iDebugCode, short sOwnerH, char cOwnerType, short sX, short sY)
{
 class CTile * pTile;	
	
	if ((sX < 0) || (sX >= m_sSizeX) || (sY < 0) || (sY >= m_sSizeY)) return;
 
	pTile = (class CTile *)(m_pTile + sX + sY*m_sSizeY);

	// �� ��ġ�� �ڵ��� ��ġ�ϸ� ��� Ŭ�����Ѵ�. 
	if ((pTile->m_sOwner == sOwnerH) && (pTile->m_cOwnerClass == cOwnerType)) {
		pTile->m_sOwner      = 0;
		pTile->m_cOwnerClass = 0;
	}

	// 
	if ((pTile->m_sDeadOwner == sOwnerH) && (pTile->m_cDeadOwnerClass == cOwnerType)) {
		pTile->m_sDeadOwner      = 0;
		pTile->m_cDeadOwnerClass = 0;
	}
}

void CMap::ClearDeadOwner(short sX, short sY)
{
 class CTile * pTile;	

	if ((sX < 0) || (sX >= m_sSizeX) || (sY < 0) || (sY >= m_sSizeY)) return;	

	pTile = (class CTile *)(m_pTile + sX + sY*m_sSizeY);
	pTile->m_sDeadOwner      = 0;
	pTile->m_cDeadOwnerClass = 0;
}

bool CMap::bSetItem(short sX, short sY, class CItem * pItem)
{
 class CTile * pTile;	
 int i;
	
	if ((sX < 0) || (sX >= m_sSizeX) || (sY < 0) || (sY >= m_sSizeY)) return 0; 

	pTile = (class CTile *)(m_pTile + sX + sY*m_sSizeY);

	// v1.4 ������ Ÿ�Ͽ� �������� �ִٸ� ������Ű�� �̵��Ѵ�. 
	if (pTile->m_pItem[DEF_TILE_PER_ITEMS-1] != 0) 
		delete pTile->m_pItem[DEF_TILE_PER_ITEMS-1];
	else pTile->m_cTotalItem++;
	
	for (i = DEF_TILE_PER_ITEMS-2; i >= 0; i--) 
		pTile->m_pItem[i+1] = pTile->m_pItem[i];

	pTile->m_pItem[0] = pItem;
	return true;
}


class CItem* CMap::pGetItem(short sX, short sY, short* pRemainItemID, char* pRemainItemColor, UINT32* pRemainItemAttr) //v1.4 color
{
	class CTile* pTile;
	class CItem* pItem;
	int i;

	if ((sX < 0) || (sX >= m_sSizeX) || (sY < 0) || (sY >= m_sSizeY)) return 0;

	pTile = (class CTile*)(m_pTile + sX + sY * m_sSizeY);
	pItem = pTile->m_pItem[0];
	if (pTile->m_cTotalItem == 0) return 0;

	for (i = 0; i <= DEF_TILE_PER_ITEMS - 2; i++)
		pTile->m_pItem[i] = pTile->m_pItem[i + 1];
	pTile->m_cTotalItem--;
	pTile->m_pItem[pTile->m_cTotalItem] = 0;

	if (pTile->m_pItem[0] == 0) {
		*pRemainItemID = 0;
		*pRemainItemColor = 0;
		*pRemainItemAttr = 0;
	}
	else
	{
		*pRemainItemID = pTile->m_pItem[0]->m_sIDnum;
		*pRemainItemColor = pTile->m_pItem[0]->m_cItemColor;
		*pRemainItemAttr = pTile->m_pItem[0]->m_dwAttribute;
	}

	return pItem;
}

int CMap::iCheckItem(short sX, short sY)
{
 class CTile * pTile;	
 class CItem * pItem;
	
	if ((sX < 0) || (sX >= m_sSizeX) || (sY < 0) || (sY >= m_sSizeY)) return 0;

	pTile = (class CTile *)(m_pTile + sX + sY*m_sSizeY);
	pItem =  pTile->m_pItem[0];
	if (pTile->m_cTotalItem == 0) return 0;

	return pItem->m_sIDnum;
}

bool CMap::bIsValidLoc(short sX, short sY)
{
	if ((sX < 0) || (sX >= m_sSizeX) || (sY < 0) || (sY >= m_sSizeY)) return false;
	return true;
}

bool CMap::bInit(char * pName)
{
 int i;
	// �������� �ε��Ѵ�.
	ZeroMemory(m_cName, sizeof(m_cName));
	strcpy(m_cName, pName);

	ZeroMemory(m_cLocationName, sizeof(m_cLocationName));

	if (_bDecodeMapDataFileContents() == false) 
		return false;	

	for (i = 0; i < DEF_MAXTELEPORTLOC; i++)
		m_pTeleportLoc[i] = 0;

	return true;
}

bool CMap::_bDecodeMapDataFileContents()
{
 HANDLE hFile;
 char  cMapFileName[256], cHeader[260], cTemp[100];
 DWORD dwFileSize, nRead;
 int i, ix, iy;
 char * token, cReadMode;
 char seps[] = "= \t\n";
 class CStrTok * pStrTok = 0;
 class CTile * pTile;
 short * sp;

	ZeroMemory(cMapFileName, sizeof(cMapFileName));
	strcat(cMapFileName, "mapdata\\");
	strcat(cMapFileName, m_cName);
	strcat(cMapFileName, ".amd");
	
	hFile = CreateFile(cMapFileName, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
	if (hFile == INVALID_HANDLE_VALUE) return false;
	dwFileSize = GetFileSize(hFile, 0);
	
	ZeroMemory(cHeader, sizeof(cHeader));
	ReadFile(hFile, (char *)cHeader, 256, &nRead, 0);

	// ��������� �м��Ѵ�.

	// ���� �ι��ڸ� ���ش�.
	for (i = 0; i < 256; i++) 
		if (cHeader[i] == 0) cHeader[i] = ' ';

	cReadMode = 0;

	pStrTok = new class CStrTok(cHeader, seps);
	token = pStrTok->pGet(); 
	while( token != 0 )   {
			
		if (cReadMode != 0) {
			switch (cReadMode) {
			case 1:
				m_sSizeX = atoi(token);
				cReadMode = 0;
				break;
			case 2:
				m_sSizeY = atoi(token);
				cReadMode = 0;
				break;
			case 3:
				m_sTileDataSize = atoi(token);
				cReadMode = 0;
				break;
			}
		}	
		else {
			if (memcmp(token, "MAPSIZEX",8) == 0) cReadMode = 1;
			if (memcmp(token, "MAPSIZEY",8) == 0) cReadMode = 2;
			if (memcmp(token, "TILESIZE",8) == 0) cReadMode = 3; 
		}
			
		token = pStrTok->pGet();
	}

	// ũ�⿡ �°� Ÿ���� �����Ѵ�.
	m_pTile = (class CTile *)new class CTile[m_sSizeX * m_sSizeY];

	// Ÿ���� ������ �о���δ�.
	for (iy = 0; iy < m_sSizeY; iy++)
	for (ix = 0; ix < m_sSizeX; ix++) {
		ReadFile(hFile, (char *)cTemp, m_sTileDataSize, &nRead, 0);	
		pTile = (class CTile *)(m_pTile + ix + iy*m_sSizeY);
		if ((cTemp[8] & 0x80) != 0) {
			// �������� �Ӽ��� ��Ʈ�Ǿ� �ִ�.
			 pTile->m_bIsMoveAllowed = false;
		}
		else pTile->m_bIsMoveAllowed = true;

		if ((cTemp[8] & 0x40) != 0) {
			// �ڷ���Ʈ �Ӽ��� ��Ʈ�Ǿ� �ִ�.
			 pTile->m_bIsTeleport = true;
		}
		else pTile->m_bIsTeleport = false;

		if ((cTemp[8] & 0x20) != 0) {
			 pTile->m_bIsFarm = true;
		}
		else pTile->m_bIsFarm = false;

		sp = (short *)&cTemp[0];
		if (*sp == 19) {
			// �� Ÿ���̴�. 
			 pTile->m_bIsWater = true;
		}
		else pTile->m_bIsWater = false;


	}

	CloseHandle(hFile);

	if (pStrTok != 0) delete pStrTok;
	return true;
}

bool CMap::bSearchTeleportDest(int sX, int sY, char * pMapName, int * pDx, int * pDy, char * pDir)
{
 int i;

	for (i = 0; i < DEF_MAXTELEPORTLOC; i++)
	if ((m_pTeleportLoc[i] != 0) && (m_pTeleportLoc[i]->m_sSrcX == sX) && (m_pTeleportLoc[i]->m_sSrcY == sY)) {
		// ���� ��ǥ�� ���� �ڷ���Ʈ ��ġ�� ã�Ҵ�. 	
		memcpy(pMapName, m_pTeleportLoc[i]->m_cDestMapName, 10);
		*pDx  = m_pTeleportLoc[i]->m_sDestX;
		*pDy  = m_pTeleportLoc[i]->m_sDestY;
		*pDir = m_pTeleportLoc[i]->m_cDir;
		return true;
	}

	return false;
}

void CMap::SetDynamicObject(UINT16 wID, short sType, short sX, short sY, UINT32 dwRegisterTime)
{
 class CTile * pTile;	

	
	if ((sX < 0) || (sX >= m_sSizeX) || (sY < 0) || (sY >= m_sSizeY)) return; 

	pTile = (class CTile *)(m_pTile + sX + sY*m_sSizeY);

	pTile->m_wDynamicObjectID   = wID;
	pTile->m_sDynamicObjectType = sType;
	pTile->m_dwDynamicObjectRegisterTime = dwRegisterTime;
}

bool CMap::bGetDynamicObject(short sX, short sY, short *pType, UINT32 *pRegisterTime, int * pIndex)
{
 class CTile * pTile;	

	
	if ((sX < 0) || (sX >= m_sSizeX) || (sY < 0) || (sY >= m_sSizeY)) return false; 

	pTile = (class CTile *)(m_pTile + sX + sY*m_sSizeY);

	*pType = pTile->m_sDynamicObjectType;
	*pRegisterTime = pTile->m_dwDynamicObjectRegisterTime;
	if (pIndex != 0) *pIndex = pTile->m_wDynamicObjectID;

	return true;
}

int CMap::iGetEmptyNamingValue()
{
  int i;

	for (i = 0; i < 15000; i++) //LifeX Fix Mobs Respawn Limit 01/01
	if (m_bNamingValueUsingStatus[i] == false) 
	{
		m_bNamingValueUsingStatus[i] = true;
		return i;
	}

	return -1;
}

void CMap::SetNamingValueEmpty(int iValue)
{
	m_bNamingValueUsingStatus[iValue] = false;
}

bool CMap::bGetIsWater(short dX, short dY)
{
 class CTile * pTile;	
	
	if ((dX < 14) || (dX >= m_sSizeX - 16) || (dY < 12) || (dY >= m_sSizeY - 14)) return false;

	pTile = (class CTile *)(m_pTile + dX + dY*m_sSizeY);
	
	if (pTile->m_bIsWater == false) return false;
	
	return true;
}

//v2.19 2002-12-16 ��� ��ų ����
bool CMap::bRemoveCropsTotalSum()
{
	if(m_iTotalAgriculture < DEF_MAXAGRICULTURE)
	{
		m_iTotalAgriculture--;
		if(m_iTotalAgriculture < 0)
		{
			m_iTotalAgriculture = 0;
		}
		return true;
	}
	return false;
}

//v2.19 2002-12-16 ��� ��ų ����
bool CMap::bAddCropsTotalSum()
{
	if(m_iTotalAgriculture < DEF_MAXAGRICULTURE)
	{
		m_iTotalAgriculture++;
		return true;
	}
	return false;
}

bool CMap::bGetIsFarm(short tX, short tY)
{
 class CTile * pTile;	
	
	if ((tX < 14) || (tX >= m_sSizeX - 16) || (tY < 12) || (tY >= m_sSizeY - 14)) return false;

	pTile = (class CTile *)(m_pTile + tX + tY*m_sSizeY);
	
	if (pTile->m_bIsFarm == false) return false;
	
	return true;
}

int CMap::iAnalyze(char cType, int * pX, int * pY, int * pV1, int * pV2, int * pV3)
{

	return 0;
}

void CMap::SetTempMoveAllowedFlag(int dX, int dY, bool bFlag)
{
 class CTile * pTile;	
	
	if ((dX < 20) || (dX >= m_sSizeX - 20) || (dY < 20) || (dY >= m_sSizeY - 20)) return;

	pTile = (class CTile *)(m_pTile + dX + dY*m_sSizeY);
	pTile->m_bIsTempMoveAllowed = bFlag;
}

int CMap::iRegisterOccupyFlag(int dX, int dY, int iSide, int iEKNum, int iDOI)
{
 int i;

	if ((dX < 20) || (dX >= m_sSizeX - 20) || (dY < 20) || (dY >= m_sSizeY - 20)) return -1;
	
	for (i = 1; i < DEF_MAXOCCUPYFLAG; i++) 
	if (m_pOccupyFlag[i] == 0) {
		// ���� �Ҵ��ϰ� ��ȯ.
		m_pOccupyFlag[i] = new class COccupyFlag(dX, dY, iSide, iEKNum, iDOI);
		if (m_pOccupyFlag == 0) return -1;
		else return i;
	}
	
	return -1;
}




void CMap::_SetupNoAttackArea()
{
 int i, ix, iy;
 class CTile * pTile;	

	for (i = 0; i < DEF_MAXNMR; i++) {
		if ((m_rcNoAttackRect[i].top > 0)) {
			// No-Magic-Area�� ���ǵǾ� �ִ�.
			for (ix = m_rcNoAttackRect[i].left; ix <= m_rcNoAttackRect[i].right; ix++)
			for (iy = m_rcNoAttackRect[i].top; iy <= m_rcNoAttackRect[i].bottom; iy++) {
				pTile = (class CTile *)(m_pTile + ix + iy*m_sSizeY);
				pTile->m_iAttribute = pTile->m_iAttribute | 0x00000004;		// 3��° ��Ʈ�� �����Ѵ�. �������� 
			}
		}
		else if (m_rcNoAttackRect[i].top == -10) {
			// �� ���� ���� ������.	
			for (ix = 0; ix < m_sSizeX; ix++)
			for (iy = 0; iy < m_sSizeY; iy++) {
				pTile = (class CTile *)(m_pTile + ix + iy*m_sSizeY);
				pTile->m_iAttribute = pTile->m_iAttribute | 0x00000004;		// 3��° ��Ʈ�� �����Ѵ�. ��������
			}
		}
	}
}

/*********************************************************************************************************************
**  int CMap::iGetAttribute(int dX, int dY, int iBitMask)															**
**  description			:: check if the tile contains a dead player													**
**  last updated		:: November 20, 2004; 9:55 PM; Hypnotoad													**
**	return value		:: int																						**
**********************************************************************************************************************/
int CMap::iGetAttribute(int dX, int dY, int iBitMask)
{
 class CTile * pTile;

	if ((dX < 20) || (dX >= m_sSizeX - 20) || (dY < 20) || (dY >= m_sSizeY - 20)) return -1;

	pTile = (class CTile *)(m_pTile + dX + dY*m_sSizeY);
	return (pTile->m_iAttribute & iBitMask);
}

bool CMap::bAddCrusadeStructureInfo(char cType, short sX, short sY, char cSide)
{
 int i;

	for (i = 0; i < DEF_MAXCRUSADESTRUCTURES; i++)
	if (m_stCrusadeStructureInfo[i].cType == 0) {
		m_stCrusadeStructureInfo[i].cType = cType;
		m_stCrusadeStructureInfo[i].cSide = cSide;
		m_stCrusadeStructureInfo[i].sX = sX;
		m_stCrusadeStructureInfo[i].sY = sY;

		m_iTotalCrusadeStructures++;
		return true;
	}

	return false;
}

bool CMap::bRemoveCrusadeStructureInfo(short sX, short sY)
{
 int i;

	for (i = 0; i < DEF_MAXCRUSADESTRUCTURES; i++)
	if ((m_stCrusadeStructureInfo[i].sX == sX) && (m_stCrusadeStructureInfo[i].sY == sY)) {
		m_stCrusadeStructureInfo[i].cType = 0;
		m_stCrusadeStructureInfo[i].cSide = 0;
		m_stCrusadeStructureInfo[i].sX = 0;
		m_stCrusadeStructureInfo[i].sY = 0;
		goto RCSI_REARRANGE;
	}

	return false;

RCSI_REARRANGE:;

	for (i = 0; i < DEF_MAXCRUSADESTRUCTURES-1; i++)
	if ((m_stCrusadeStructureInfo[i].cType == 0) && (m_stCrusadeStructureInfo[i+1].cType != 0)) {
		m_stCrusadeStructureInfo[i].cType = m_stCrusadeStructureInfo[i+1].cType;
		m_stCrusadeStructureInfo[i].cSide = m_stCrusadeStructureInfo[i+1].cSide;
		m_stCrusadeStructureInfo[i].sX = m_stCrusadeStructureInfo[i+1].sX;
		m_stCrusadeStructureInfo[i].sY = m_stCrusadeStructureInfo[i+1].sY;

		m_stCrusadeStructureInfo[i+1].cType = 0;
		m_stCrusadeStructureInfo[i+1].cSide = 0;
		m_stCrusadeStructureInfo[i+1].sX = 0;
		m_stCrusadeStructureInfo[i+1].sY = 0;
	}

	m_iTotalCrusadeStructures--;
	return true;
}



void CMap::RestoreStrikePoints()
{
 int i;

	for (i = 0; i < DEF_MAXSTRIKEPOINTS; i++) {
		m_stStrikePoint[i].iInitHP = m_stStrikePoint[i].iHP;
	}
}

char _tmp_cEmptyAreaX[] = { 0, 1, 1, 0, -1, -1, -1, 0 ,1, 2, 2, 2, 2, 1, 0, -1, -2, -2, -2, -2, -2, -1, 0, 1, 2 };
char _tmp_cEmptyAreaY[] = { 0, 0, 1, 1, 1, 0, -1, -1, -1, -1, 0, 1, 2, 2, 2, 2, 2, 1, 0, -1, -2, -2, -2, -2, -2 };
void CMap::SetBigOwner(short sOwner, char cOwnerClass, short pX, short pY, char cArea)
{
 short sX, sY, sAreaSquared;
 class CTile * pTile;
 int i;

	sX = 0;
	sY = 0;
	sAreaSquared = cArea*cArea;
	if ((cArea > 5) || (cArea < 2)) return;
	if ((pX < 0) || (pX >= m_sSizeX) || (pY < 0) || (pY >= m_sSizeY)) return; 
	for (i = 0; i < sAreaSquared; i++) {
		sX = pX + _tmp_cEmptyAreaX[i];
		sY = pY + _tmp_cEmptyAreaY[i];	
		pTile = (class CTile *)(m_pTile + sX + sY*m_sSizeY);
		pTile->m_sOwner = sOwner;
		pTile->m_cOwnerClass = cOwnerClass;
	}
	return;
}

void CMap::ClearBigOwner(short sOwnerH, char cOwnerType, short pX, short pY, char cArea)
{
 short sX, sY, sAreaSquared;
 class CTile * pTile;
 int i;

	sX = 0;
	sY = 0;
	sAreaSquared = cArea*cArea;
	if ((pX < 0) || (pX >= m_sSizeX) || (pY < 0) || (pY >= m_sSizeY)) return; 
	pTile = (class CTile *)(m_pTile + pX + pY*m_sSizeY);
	if ((pTile->m_sDeadOwner == sOwnerH) && (pTile->m_cDeadOwnerClass == cOwnerType)) {
		pTile->m_sDeadOwner      = 0;
		pTile->m_cDeadOwnerClass = 0;
	}
	for (i = 0; i < sAreaSquared; i++) {
		sX = pX + _tmp_cEmptyAreaX[i];
		sY = pY + _tmp_cEmptyAreaY[i];	
		pTile = (class CTile *)(m_pTile + sX + sY*m_sSizeY);
		if ((pTile->m_sOwner == sOwnerH) && (pTile->m_cOwnerClass == cOwnerType)) {
			pTile->m_sOwner      = 0;
			pTile->m_cOwnerClass = 0;
		}
	}
}

bool CMap::bApocalypseGate()
{
 class CTile * pTile;
 int i, LR, TB;
 UINT32 cLeft, cTop, cRight, cBottom;

	for (i = 0; i < m_iTotalDynamicGate; i++) {
		cLeft	= m_rcDynamicGateRect[i].left;
		cTop	= m_rcDynamicGateRect[i].top;  
		cRight	= m_rcDynamicGateRect[i].right;
		cBottom = m_rcDynamicGateRect[i].bottom;
		for (LR = cLeft; LR <= cRight; LR++) {
			for (TB = cTop; TB <= cBottom; TB++) {
				pTile = (class CTile *)(m_pTile + TB + LR*m_sSizeY);
				pTile->m_bIsApocalypseGate = true;
				pTile->m_cApocalypseGateMap = i;
			}
		}
		return true;
	}
	return false;
}

// SNoopy: Prevent characters to stay behind crusade structures
void CMap::SetStayAllowedFlag(int dX, int dY, bool bFlag)
{ class CTile * pTile;		
	if ((dX < 20) || (dX >= m_sSizeX - 20) || (dY < 20) || (dY >= m_sSizeY - 20)) return;
	pTile = (class CTile *)(m_pTile + dX + dY*m_sSizeY);
	pTile->m_bCanStayHere = bFlag;
}
bool CMap::bGetIsStayAllowedTile(short dX, short dY) // Staying there, and suffer damage....
{ class CTile * pTile;		
	if ((dX < 20) || (dX >= m_sSizeX - 20) || (dY < 20) || (dY >= m_sSizeY - 20)) return false;
	pTile = (class CTile *)(m_pTile + dX + dY*m_sSizeY);	
	if (pTile->m_bIsMoveAllowed == false) return false;
	if (pTile->m_bIsTempMoveAllowed == false) return false;
	if (pTile->m_bCanStayHere == false) return false;
	return true;
}

bool CGame::__bReadMapInfo(int iMapIndex)
{
	char* pContents, * token, cTxt[250], cFn[255];
	char seps[] = "= \t\n";
	char cReadModeA = 0;
	char cReadModeB = 0;
	int  iTeleportLocIndex = 0;
	int  iWayPointCfgIndex = 0;
	int  iTotalNpcSetting = 0;
	int  iMGARCfgIndex = 0;
	int  iSMGRCfgIndex = 0;
	int  iNMRCfgIndex = 0;
	int  iFishPointIndex = 0;
	int  iMineralPointIndex = 0;
	int  iStrategicPointIndex = 0;
	int  iIndex = 0, iIndexGD = 0, iIndexHT = 0;;
	int	i;

	int  iNamingValue;
	class CStrTok* pStrTok;
	HANDLE hFile;
	UINT32  dwFileSize, dwReadSize;
	FILE* pFile;

	char cName[6], cNpcName[21], cNpcMoveType, cNpcWaypointIndex[10], cNamePrefix;
	short sIPindex;

	m_pMapList[iMapIndex]->m_iApocalypseMobGenType = 0;		// 26 ApocalypseMobGenType
	m_pMapList[iMapIndex]->m_iApocalypseBossMobNpcID = 0;	// 27 ApocalypseBossMob
	m_pMapList[iMapIndex]->m_sApocalypseBossMobRect.left = 0;
	m_pMapList[iMapIndex]->m_sApocalypseBossMobRect.top = 0;
	m_pMapList[iMapIndex]->m_sApocalypseBossMobRect.right = 0;
	m_pMapList[iMapIndex]->m_sApocalypseBossMobRect.bottom = 0;
	m_pMapList[iMapIndex]->m_cDynamicGateType = 0;			// 28 DynamicGateType
	m_pMapList[iMapIndex]->m_sDynamicGateCoordRectX1 = 0;	// 29 DynamicGateCoord 
	m_pMapList[iMapIndex]->m_sDynamicGateCoordRectX2 = 0;
	m_pMapList[iMapIndex]->m_sDynamicGateCoordRectY1 = 0;
	m_pMapList[iMapIndex]->m_sDynamicGateCoordRectY2 = 0;
	ZeroMemory(m_pMapList[iMapIndex]->m_cDynamicGateCoordDestMap, sizeof(m_pMapList[iMapIndex]->m_cDynamicGateCoordDestMap));
	m_pMapList[iMapIndex]->m_sDynamicGateCoordTgtX = 0;
	m_pMapList[iMapIndex]->m_sDynamicGateCoordTgtY = 0;
	m_pMapList[iMapIndex]->m_bIsRecallImpossible = false;	// 30 Recal impossible
	m_pMapList[iMapIndex]->m_bIsApocalypseMap = false;		// 31 ApocalypseMap 

	m_pMapList[iMapIndex]->m_bIsHeldenianMap = false;		// 33 HeldenianMap	
	for (i = 0; i < DEF_MAXHELDENIANTOWER; i++)				// 34 HeldenianTower
	{
		m_pMapList[iMapIndex]->m_stHeldenianTower[i].sTypeID = 0;
		m_pMapList[iMapIndex]->m_stHeldenianTower[i].dX = 0;
		m_pMapList[iMapIndex]->m_stHeldenianTower[i].dY = 0;
		m_pMapList[iMapIndex]->m_stHeldenianTower[i].cSide = 0;
	}
	m_pMapList[iMapIndex]->m_cHeldenianModeMap = 0;			// 35 HeldenianModeMap
	m_pMapList[iMapIndex]->m_sHeldenianWinningZoneX = -1;	// 36 HeldenianWinningZone
	m_pMapList[iMapIndex]->m_sHeldenianWinningZoneY = -1;	// 36 HeldenianWinningZone
	for (i = 0; i < DEF_MAXHELDENIANDOOR; i++) 				// 37 HeldenianGateDoor  
	{
		m_pMapList[iMapIndex]->m_stHeldenianGateDoor[i].cDir = 0;
		m_pMapList[iMapIndex]->m_stHeldenianGateDoor[i].dX = 0;
		m_pMapList[iMapIndex]->m_stHeldenianGateDoor[i].dY = 0;
	}

	// »çÅõÀåÀÎÁö¸¦ ÆÇ´Ü.
	if (memcmp(m_pMapList[iMapIndex]->m_cName, "fightzone", 9) == 0)
		m_pMapList[iMapIndex]->m_bIsFightZone = true;

	if (memcmp(m_pMapList[iMapIndex]->m_cName, "icebound", 8) == 0)
		m_pMapList[iMapIndex]->m_bIsSnowEnabled = true;

	ZeroMemory(cFn, sizeof(cFn));
	strcat(cFn, "mapdata\\");
	strcat(cFn, m_pMapList[iMapIndex]->m_cName);
	strcat(cFn, ".txt");

	hFile = CreateFile(cFn, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
	if (hFile == INVALID_HANDLE_VALUE) return false;
	dwFileSize = GetFileSize(hFile, 0);
	CloseHandle(hFile);


	pContents = new char[dwFileSize + 1];
	ZeroMemory(pContents, dwFileSize + 1);

	pFile = fopen(cFn, "rt");
	if (pFile == 0) {
		// ¸Ê Á¤º¸ÆÄÀÏÀ»  ÀÐÀ» ¼ö ¾ø´Ù.
		wsprintf(cTxt, "(!) Cannot open file : %s", cFn);
		PutLogList(cTxt);
		return false;
	}
	else {
		wsprintf(cTxt, "(!) Reading Map info file : %s", cFn);
		PutLogList(cTxt);
		dwReadSize = fread(pContents, dwFileSize, 1, pFile);
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
					// ÅÚ·¹Æ÷Æ® ¼Ò½º ÁÂÇ¥ X  
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 1 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}

					m_pMapList[iMapIndex]->m_pTeleportLoc[iTeleportLocIndex]->m_sSrcX = atoi(token);
					cReadModeB = 2;
					break;

				case 2:
					// ÅÚ·¹Æ÷Æ® ¼Ò½º ÁÂÇ¥ Y 
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 2 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}

					m_pMapList[iMapIndex]->m_pTeleportLoc[iTeleportLocIndex]->m_sSrcY = atoi(token);
					cReadModeB = 3;
					break;

				case 3:
					// ÅÚ·¹Æ÷Æ® ¸ñÀûÁö ¸Ê ÀÌ¸§ 
					ZeroMemory(m_pMapList[iMapIndex]->m_pTeleportLoc[iTeleportLocIndex]->m_cDestMapName, sizeof(m_pMapList[iMapIndex]->m_pTeleportLoc[iTeleportLocIndex]->m_cDestMapName));
					strcpy(m_pMapList[iMapIndex]->m_pTeleportLoc[iTeleportLocIndex]->m_cDestMapName, token);
					cReadModeB = 4;
					break;

				case 4:
					// ÅÚ·¹Æ÷Æ® ¸ñÀûÁö À§Ä¡ X 
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 3 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}

					m_pMapList[iMapIndex]->m_pTeleportLoc[iTeleportLocIndex]->m_sDestX = atoi(token);
					cReadModeB = 5;
					break;

				case 5:
					// ÅÚ·¹Æ÷Æ® ¸ñÀûÁö À§Ä¡ Y 
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 4 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}

					m_pMapList[iMapIndex]->m_pTeleportLoc[iTeleportLocIndex]->m_sDestY = atoi(token);
					cReadModeB = 6;
					break;

				case 6:
					// ÅÚ·¹Æ÷Æ® ÈÄ ¹æÇâ  
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 5 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}

					m_pMapList[iMapIndex]->m_pTeleportLoc[iTeleportLocIndex]->m_cDir = atoi(token);
					iTeleportLocIndex++;
					cReadModeA = 0;
					cReadModeB = 0;
					break;
				}
				break;

			case 2:
				switch (cReadModeB) {
				case 1:
					// waypoint ¹øÈ£   
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 6 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					iWayPointCfgIndex = atoi(token);

					if (m_pMapList[iMapIndex]->m_WaypointList[iWayPointCfgIndex].x != -1) {
						// ÀÌ¹Ì ÇÒ´çµÇ¾îÀÖ´Â Waypoint ¹øÈ£ÀÌ´Ù.
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 7 - Duplicated waypoint");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					cReadModeB = 2;
					break;

				case 2:
					// waypoint ÁýÇÕ Á¤ÀÇ X  
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 8 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_WaypointList[iWayPointCfgIndex].x = atoi(token);
					cReadModeB = 3;
					break;

				case 3:
					// waypoint ÁýÇÕ Á¤ÀÇ Y  
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 9 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_WaypointList[iWayPointCfgIndex].y = atoi(token);
					cReadModeA = 0;
					cReadModeB = 0;
					break;
				}
				break;

			case 3:
				// Npc¸¦ Æ¯Á¤À§Ä¡¿¡ À§Ä¡½ÃÅ²´Ù.
				switch (cReadModeB) {
				case 1:
					// NPCÀÇ ÀÌ¸§. 
					ZeroMemory(cNpcName, sizeof(cNpcName));
					strcpy(cNpcName, token);
					cReadModeB = 2;
					break;
				case 2:
					// NpcMoveType
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 10 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					cNpcMoveType = atoi(token);
					cReadModeB = 3;
					break;
				default:
					// WayPoint0~waypoint9
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 11 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					cNpcWaypointIndex[cReadModeB - 3] = atoi(token);
					cReadModeB++;
					break;
				case 13:
					// cNamePrefix
					cNamePrefix = token[0];
					// ÀÌÁ¦ ÀÌ°÷¿¡¼­ NPC¸¦ »ý¼ºÇÑ´Ù. 

					iNamingValue = m_pMapList[iMapIndex]->iGetEmptyNamingValue();
					if (iNamingValue != -1) {
						// ´õÀÌ»ó ÀÌ ¸Ê¿¡ NPC¸¦ ¸¸µé¼ö ¾ø´Ù. ÀÌ¸§À» ÇÒ´çÇÒ ¼ö ¾ø±â ¶§¹®.

						// NPC¸¦ »ý¼ºÇÑ´Ù.
						ZeroMemory(cName, sizeof(cName));
						wsprintf(cName, "XX%d", iNamingValue);
						cName[0] = cNamePrefix;
						cName[1] = iMapIndex + 65;

						if (bCreateNewNpc(cNpcName, cName, m_pMapList[iMapIndex]->m_cName, 0, 0, cNpcMoveType, 0, 0, cNpcWaypointIndex, 0, 0, -1, false) == false) {
							// ½ÇÆÐÇßÀ¸¹Ç·Î ¿¹¾àµÈ NameValue¸¦ ÇØÁ¦½ÃÅ²´Ù.
							m_pMapList[iMapIndex]->SetNamingValueEmpty(iNamingValue);
						}
					}
					cReadModeA = 0;
					cReadModeB = 0;
					iTotalNpcSetting++;
					break;
				}
				break;

			case 4:
				switch (cReadModeB) {
				case 1:
					// Random-Mob-Generator »ç¿ë ¿©ºÎ 
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 12 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_bRandomMobGenerator = (bool)atoi(token);
					cReadModeB = 2;
					break;

				case 2:
					// Mob- Level
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 13 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_cRandomMobGeneratorLevel = atoi(token);
					cReadModeA = 0;
					cReadModeB = 0;
					break;
				}
				break;

			case 5:
				// Maximum object
				if (_bGetIsStringIsNumber(token) == false) {
					PutLogList("(!!!) CRITICAL ERROR! Map Info file error 14 - Wrong Data format.");
					delete[] pContents;
					delete pStrTok;
					return false;
				}

				m_pMapList[iMapIndex]->m_iMaximumObject = atoi(token);
				cReadModeA = 0;
				cReadModeB = 0;
				break;

			case 6:
				// Ä³¸¯ÅÍ ·£´ý »ý¼º ±ÝÁö ±¸¿ª : ¸¶À» Áß½ÉºÎ °°Àºµ¥¼­ ¸÷ÀÌ ¹ß»ýµÇ¸é °ï¶õÇÏ¹Ç·Î 
				switch (cReadModeB) {
				case 1:
					// Rect ¹øÈ£ 
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 15 - Wrong Data format(MGAR num).");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					iMGARCfgIndex = atoi(token);

					if (m_pMapList[iMapIndex]->m_rcMobGenAvoidRect[iMGARCfgIndex].left != -1) {
						// ÀÌ¹Ì ÇÒ´çµÇ¾îÀÖ´Â Waypoint ¹øÈ£ÀÌ´Ù.
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 16 - Duplicated Mob Gen Rect Number!");
						delete[] pContents;
						delete pStrTok;
						return false;
					}

					cReadModeB = 2;
					break;

				case 2:
					// left
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 17 - Wrong Data format(MGAR num).");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_rcMobGenAvoidRect[iMGARCfgIndex].left = atoi(token);
					cReadModeB = 3;
					break;

				case 3:
					// top
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 18 - Wrong Data format(MGAR num).");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_rcMobGenAvoidRect[iMGARCfgIndex].top = atoi(token);
					cReadModeB = 4;
					break;

				case 4:
					// right
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 19 - Wrong Data format(MGAR num).");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_rcMobGenAvoidRect[iMGARCfgIndex].right = atoi(token);
					cReadModeB = 5;
					break;

				case 5:
					// bottom
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 20 - Wrong Data format(MGAR num).");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_rcMobGenAvoidRect[iMGARCfgIndex].bottom = atoi(token);
					cReadModeA = 0;
					cReadModeB = 0;
					break;
				}
				break;

			case 7:
				// Æ¯Á¤Áö¿ª ¸÷ »ý¼º 
				switch (cReadModeB) {
				case 1:
					// Rect ¹øÈ£ m_stSpotMobGenerator[]
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 21 - Wrong Data format(MGAR num).");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					iSMGRCfgIndex = atoi(token);

					if (m_pMapList[iMapIndex]->m_stSpotMobGenerator[iSMGRCfgIndex].bDefined == true) {
						// ÀÌ¹Ì ÇÒ´çµÇ¾îÀÖ´Â ¸÷ Á¦³Ê·¹ÀÌÅÍ ¹øÈ£ÀÌ´Ù.
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error - ");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					cReadModeB = 2;
					m_pMapList[iMapIndex]->m_stSpotMobGenerator[iSMGRCfgIndex].bDefined = true;
					break;

				case 2:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 22 - Wrong Data format(SMGAR num).");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_stSpotMobGenerator[iSMGRCfgIndex].cType = atoi(token);

					if (m_pMapList[iMapIndex]->m_stSpotMobGenerator[iSMGRCfgIndex].cType == 1)
						cReadModeB = 3;
					else if (m_pMapList[iMapIndex]->m_stSpotMobGenerator[iSMGRCfgIndex].cType == 2)
						cReadModeB = 9;  // RECT°¡ ¾Æ´Ï¶ó WaypointÁýÇÕÀ» ÀÐ¾î¾ß ÇÑ´Ù.
					break;

				case 3:
					// left
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 23 - Wrong Data format(SMGAR num).");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_stSpotMobGenerator[iSMGRCfgIndex].rcRect.left = atoi(token);
					cReadModeB = 4;
					break;

				case 4:
					// top
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 24 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_stSpotMobGenerator[iSMGRCfgIndex].rcRect.top = atoi(token);
					cReadModeB = 5;
					break;

				case 5:
					// right
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 25 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_stSpotMobGenerator[iSMGRCfgIndex].rcRect.right = atoi(token);
					cReadModeB = 6;
					break;

				case 6:
					// bottom
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 26 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_stSpotMobGenerator[iSMGRCfgIndex].rcRect.bottom = atoi(token);
					cReadModeB = 7;
					break;

				case 7:
					// spot mob type
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 27 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_stSpotMobGenerator[iSMGRCfgIndex].iMobType = atoi(token);
					cReadModeB = 8;
					break;

				case 8:
					// Max Mobs
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 28 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_stSpotMobGenerator[iSMGRCfgIndex].iMaxMobs = atoi(token);
					m_pMapList[iMapIndex]->m_stSpotMobGenerator[iSMGRCfgIndex].iCurMobs = 0;
					cReadModeA = 0;
					cReadModeB = 0;
					break;

				default:
					// WayPoint0~waypoint9
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 29 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_stSpotMobGenerator[iSMGRCfgIndex].cWaypoint[cReadModeB - 9] = atoi(token);
					cReadModeB++;
					break;

				case 19:
					// spot mob type
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 30 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_stSpotMobGenerator[iSMGRCfgIndex].iMobType = atoi(token);
					cReadModeB = 20;
					break;

				case 20:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 31 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_stSpotMobGenerator[iSMGRCfgIndex].iMaxMobs = atoi(token);
					m_pMapList[iMapIndex]->m_stSpotMobGenerator[iSMGRCfgIndex].iCurMobs = 0;
					cReadModeA = 0;
					cReadModeB = 0;
					break;
				}
				break;

			case 8:
				// ¸ÊÀÌ ¼ÓÇÑ Àå¼Ò ÀÌ¸§ 
				ZeroMemory(m_pMapList[iMapIndex]->m_cLocationName, sizeof(m_pMapList[iMapIndex]->m_cLocationName));
				memcpy(m_pMapList[iMapIndex]->m_cLocationName, token, 10);
				cReadModeA = 0;
				cReadModeB = 0;
				break;

			case 9:
				switch (cReadModeB) {
				case 1:
					// Initial-Point Index
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 32:1 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					sIPindex = atoi(token);
					if (m_pMapList[iMapIndex]->m_pInitialPoint[sIPindex].x != -1) {
						PutLogList("(!!!) CRITICAL ERROR! Duplicate Initial Point Index!");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					cReadModeB = 2;
					break;

				case 2:
					// Initial-Point X
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 32 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_pInitialPoint[sIPindex].x = atoi(token);
					cReadModeB = 3;
					break;

				case 3:
					// Initial-Point Y
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 33 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_pInitialPoint[sIPindex].y = atoi(token);
					cReadModeA = 0;
					cReadModeB = 0;
					break;
				}
				break;

			case 10:
				// °ø°Ý ¹«È¿È­ ¿µ¿ª RECT
				switch (cReadModeB) {
				case 1:
					// 
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 34 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					iNMRCfgIndex = atoi(token);

					if (m_pMapList[iMapIndex]->m_rcNoAttackRect[iNMRCfgIndex].top != -1) {
						// ÀÌ¹Ì ÇÒ´çµÇ¾îÀÖ´Â No-Magic-Rect ¹øÈ£ÀÌ´Ù.
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 35 - Duplicate No-Magic-Rect number");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					cReadModeB = 2;
					break;

				case 2:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 36 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_rcNoAttackRect[iNMRCfgIndex].left = atoi(token);
					cReadModeB = 3;
					break;

				case 3:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 37 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_rcNoAttackRect[iNMRCfgIndex].top = atoi(token);
					cReadModeB = 4;
					break;

				case 4:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 38 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_rcNoAttackRect[iNMRCfgIndex].right = atoi(token);
					cReadModeB = 5;
					break;

				case 5:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 39 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_rcNoAttackRect[iNMRCfgIndex].bottom = atoi(token);
					cReadModeA = 0;
					cReadModeB = 0;
					break;
				}
				break;

			case 11:
				m_pMapList[iMapIndex]->m_bIsFixedDayMode = (bool)atoi(token);
				if (m_pMapList[iMapIndex]->m_bIsFixedDayMode == true)
					m_pMapList[iMapIndex]->m_bIsSnowEnabled = false;
				cReadModeA = 0;
				break;

			case 12:
				switch (cReadModeB) {
				case 1:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 40 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					iFishPointIndex = atoi(token);

					if (m_pMapList[iMapIndex]->m_FishPointList[iFishPointIndex].x != -1) {
						// ÀÌ¹Ì ÇÒ´çµÇ¾îÀÖ´Â Fish Point ¹øÈ£ÀÌ´Ù.
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 41 - Duplicate FishPoint number");
						delete[] pContents;
						delete pStrTok;
						return false;
					}

					m_pMapList[iMapIndex]->m_iTotalFishPoint++;
					cReadModeB = 2;
					break;

				case 2:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 42 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_FishPointList[iFishPointIndex].x = atoi(token);

					cReadModeB = 3;
					break;

				case 3:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 43 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_FishPointList[iFishPointIndex].y = atoi(token);

					cReadModeA = 0;
					cReadModeB = 0;
					break;
				}
				break;

			case 13:
				if (_bGetIsStringIsNumber(token) == false) {
					PutLogList("(!!!) CRITICAL ERROR! Map Info file error 44 - Wrong Data format.");
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pMapList[iMapIndex]->m_iMaxFish = atoi(token);

				cReadModeA = 0;
				break;

			case 14:
				if (_bGetIsStringIsNumber(token) == false) {
					PutLogList("(!!!) CRITICAL ERROR! Map Info file error 45 - Wrong Data format.");
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pMapList[iMapIndex]->m_cType = atoi(token);

				cReadModeA = 0;
				break;

			case 15:
				if (_bGetIsStringIsNumber(token) == false) {
					PutLogList("(!!!) CRITICAL ERROR! Map Info file error 46 - Wrong Data format.");
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pMapList[iMapIndex]->m_iLevelLimit = atoi(token);

				cReadModeA = 0;
				break;

			case 16:
				// ±¤¹° Á¦³×·¹ÀÌÅÍÀÇ Á¸ÀçÀ¯¹«¿Í µî±Þ 
				switch (cReadModeB) {
				case 1:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 47 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_bMineralGenerator = (bool)atoi(token);
					cReadModeB = 2;
					break;

				case 2:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 48 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_cMineralGeneratorLevel = atoi(token);
					cReadModeA = 0;
					cReadModeB = 0;
					break;
				}
				break;

			case 17:
				switch (cReadModeB) {
				case 1:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 49 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					iMineralPointIndex = atoi(token);

					if (m_pMapList[iMapIndex]->m_MineralPointList[iMineralPointIndex].x != -1) {
						// ÀÌ¹Ì ÇÒ´çµÇ¾îÀÖ´Â Mineral Point ¹øÈ£ÀÌ´Ù.
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 50 - Duplicate MineralPoint number");
						delete[] pContents;
						delete pStrTok;
						return false;
					}

					m_pMapList[iMapIndex]->m_iTotalMineralPoint++;
					cReadModeB = 2;
					break;

				case 2:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 51 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_MineralPointList[iMineralPointIndex].x = atoi(token);

					cReadModeB = 3;
					break;

				case 3:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 52 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_MineralPointList[iMineralPointIndex].y = atoi(token);

					cReadModeA = 0;
					cReadModeB = 0;
					break;
				}
				break;

			case 18:
				if (_bGetIsStringIsNumber(token) == false) {
					PutLogList("(!!!) CRITICAL ERROR! Map Info file error 53 - Wrong Data format.");
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pMapList[iMapIndex]->m_iMaxMineral = atoi(token);

				cReadModeA = 0;
				break;

			case 19:
				if (_bGetIsStringIsNumber(token) == false) {
					PutLogList("(!!!) CRITICAL ERROR! Map Info file error 54 - Wrong Data format.");
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pMapList[iMapIndex]->m_iUpperLevelLimit = atoi(token);

				cReadModeA = 0;
				break;

			case 20:
				switch (cReadModeB) {
				case 1:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 55 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					iStrategicPointIndex = atoi(token);

					if (m_pMapList[iMapIndex]->m_pStrategicPointList[iStrategicPointIndex] != 0) {
						// ÀÌ¹Ì ÇÒ´çµÇ¾îÀÖ´Â Strategic Point ¹øÈ£ÀÌ´Ù.
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 56 - Duplicate Strategic Point number");
						delete[] pContents;
						delete pStrTok;
						return false;
					}

					m_pMapList[iMapIndex]->m_pStrategicPointList[iStrategicPointIndex] = new class CStrategicPoint;
					cReadModeB = 2;
					break;

				case 2:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 57 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_pStrategicPointList[iStrategicPointIndex]->m_iSide = atoi(token);

					cReadModeB = 3;
					break;

				case 3:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 58 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_pStrategicPointList[iStrategicPointIndex]->m_iValue = atoi(token);

					cReadModeB = 4;
					break;

				case 4:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 59 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_pStrategicPointList[iStrategicPointIndex]->m_iX = atoi(token);

					cReadModeB = 5;
					break;

				case 5:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 60 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_pStrategicPointList[iStrategicPointIndex]->m_iY = atoi(token);

					cReadModeA = 0;
					cReadModeB = 0;
					break;
				}
				break;

			case 21:
				switch (cReadModeB) {
				case 1:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 61 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					iIndex = atoi(token);

					if (m_pMapList[iMapIndex]->m_stEnergySphereCreationList[iIndex].cType != 0) {
						// ÀÌ¹Ì ÇÒ´çµÇ¾îÀÖ´Â Energy-Sphere-Creation Point ¹øÈ£ÀÌ´Ù.
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 62 - Duplicate EnergySphereCreation number");
						delete[] pContents;
						delete pStrTok;
						return false;
					}

					m_pMapList[iMapIndex]->m_iTotalEnergySphereCreationPoint++;
					cReadModeB = 2;
					break;

				case 2:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 63 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_stEnergySphereCreationList[iIndex].cType = atoi(token);

					cReadModeB = 3;
					break;

				case 3:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 64 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_stEnergySphereCreationList[iIndex].sX = atoi(token);

					cReadModeB = 4;
					break;

				case 4:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 65 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_stEnergySphereCreationList[iIndex].sY = atoi(token);

					cReadModeA = 0;
					cReadModeB = 0;
					break;
				}
				break;

			case 22:
				switch (cReadModeB) {
				case 1:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 66 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					iIndex = atoi(token);

					if (m_pMapList[iMapIndex]->m_stEnergySphereGoalList[iIndex].cResult != 0) {
						// ÀÌ¹Ì ÇÒ´çµÇ¾îÀÖ´Â Energy-Sphere-Goal Point ¹øÈ£ÀÌ´Ù.
						wsprintf(G_cTxt, "(!!!) CRITICAL ERROR! Map Info file error 67 - Duplicate EnergySphereGoal number(%d:%d)", iIndex, m_pMapList[iMapIndex]->m_stEnergySphereGoalList[iIndex].cResult);
						PutLogList(G_cTxt);
						delete[] pContents;
						delete pStrTok;
						return false;
					}

					m_pMapList[iMapIndex]->m_iTotalEnergySphereGoalPoint++;
					cReadModeB = 2;
					break;

				case 2:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 68 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_stEnergySphereGoalList[iIndex].cResult = atoi(token);

					cReadModeB = 3;
					break;

				case 3:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 69 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_stEnergySphereGoalList[iIndex].aresdenX = atoi(token);

					cReadModeB = 4;
					break;

				case 4:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 70 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_stEnergySphereGoalList[iIndex].aresdenY = atoi(token);

					cReadModeB = 5;
					break;

				case 5:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 71 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_stEnergySphereGoalList[iIndex].elvineX = atoi(token);

					cReadModeB = 6;
					break;

				case 6:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 72 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_stEnergySphereGoalList[iIndex].elvineY = atoi(token);

					cReadModeA = 0;
					cReadModeB = 0;
					break;
				}
				break;

			case 23:
				switch (cReadModeB) {
				case 1:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 73 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					iIndex = atoi(token);

					if (strlen(m_pMapList[iMapIndex]->m_stStrikePoint[iIndex].cRelatedMapName) != 0) {
						// ÀÌ¹Ì ÇÒ´çµÇ¾îÀÖ´Â  Point ¹øÈ£ÀÌ´Ù.
						wsprintf(G_cTxt, "(!!!) CRITICAL ERROR! Map Info file error 74 - Duplicate Strike Point number(%d)", iIndex);
						PutLogList(G_cTxt);
						delete[] pContents;
						delete pStrTok;
						return false;
					}

					cReadModeB = 2;
					break;

				case 2:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 75 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_stStrikePoint[iIndex].dX = atoi(token);

					cReadModeB = 3;
					break;

				case 3:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 76 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_stStrikePoint[iIndex].dY = atoi(token);

					cReadModeB = 4;
					break;

				case 4:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 77 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_stStrikePoint[iIndex].iHP = atoi(token);
					m_pMapList[iMapIndex]->m_stStrikePoint[iIndex].iInitHP = atoi(token);
					cReadModeB = 5;
					break;


				case 5:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 77 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_stStrikePoint[iIndex].iEffectX[0] = atoi(token);

					cReadModeB = 6;
					break;

				case 6:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 77 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_stStrikePoint[iIndex].iEffectY[0] = atoi(token);

					cReadModeB = 7;
					break;

				case 7:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 77 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_stStrikePoint[iIndex].iEffectX[1] = atoi(token);

					cReadModeB = 8;
					break;

				case 8:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 77 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_stStrikePoint[iIndex].iEffectY[1] = atoi(token);

					cReadModeB = 9;
					break;

				case 9:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 77 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_stStrikePoint[iIndex].iEffectX[2] = atoi(token);

					cReadModeB = 10;
					break;

				case 10:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 77 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_stStrikePoint[iIndex].iEffectY[2] = atoi(token);

					cReadModeB = 11;
					break;

				case 11:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 77 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_stStrikePoint[iIndex].iEffectX[3] = atoi(token);

					cReadModeB = 12;
					break;

				case 12:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 77 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_stStrikePoint[iIndex].iEffectY[3] = atoi(token);

					cReadModeB = 13;
					break;

				case 13:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 77 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_stStrikePoint[iIndex].iEffectX[4] = atoi(token);

					cReadModeB = 14;
					break;

				case 14:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 77 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_stStrikePoint[iIndex].iEffectY[4] = atoi(token);

					cReadModeB = 15;
					break;

				case 15:
					ZeroMemory(m_pMapList[iMapIndex]->m_stStrikePoint[iIndex].cRelatedMapName, sizeof(m_pMapList[iMapIndex]->m_stStrikePoint[iIndex].cRelatedMapName));
					strcpy(m_pMapList[iMapIndex]->m_stStrikePoint[iIndex].cRelatedMapName, token);

					m_pMapList[iMapIndex]->m_iTotalStrikePoints++;
					cReadModeA = 0;
					cReadModeB = 0;
					break;

				}
				break; // end 23

			case 24:
				switch (cReadModeB) {
				case 1:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 78 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					iIndex = atoi(token);

					if (strlen(m_pMapList[iMapIndex]->m_stItemEventList[iIndex].cItemName) != 0) {
						// �̹� �Ҵ�Ǿ��ִ� Item-Event ��ȣ�̴�.
						wsprintf(G_cTxt, "(!!!) CRITICAL ERROR! Map Info file error 79 - Duplicate Item-Event number(%d:%s)", iIndex, m_pMapList[iMapIndex]->m_stItemEventList[iIndex].cItemName);
						PutLogList(G_cTxt);
						delete[] pContents;
						delete pStrTok;
						return false;
					}

					m_pMapList[iMapIndex]->m_iTotalItemEvents++;
					cReadModeB = 2;
					break;

				case 2:
					strcpy(m_pMapList[iMapIndex]->m_stItemEventList[iIndex].cItemName, token);
					cReadModeB = 3;
					break;

				case 3:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 81 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_stItemEventList[iIndex].iAmount = atoi(token);

					cReadModeB = 4;
					break;

				case 4:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 82 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_stItemEventList[iIndex].iTotalNum = atoi(token);

					cReadModeB = 5;
					break;

				case 5:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 83 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_stItemEventList[iIndex].iMonth = atoi(token);

					cReadModeB = 6;
					break;

				case 6:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 83 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_stItemEventList[iIndex].iDay = atoi(token);

					cReadModeA = 0;
					cReadModeB = 0;
					break;
				}
				break;

			case 25: //mobevent-amount
				switch (cReadModeB) {
				case 1:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error 78 - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->sMobEventAmount = atoi(token);
					cReadModeB = 0;
					break;
				}
				break;

			case 26: // ApocalypseMobGenType
				if (_bGetIsStringIsNumber(token) == false) {
					PutLogList("(!!!) CRITICAL ERROR! Map Info file error ApocalypseMobGenType - Wrong Data format.");
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pMapList[iMapIndex]->m_iApocalypseMobGenType = atoi(token);
				cReadModeA = 0;
				cReadModeB = 0;
				break;

			case 27: // ApocalypseBossMob //SNOOPY replaced coord by RECT structure.
				switch (cReadModeB) {
				case 1: // 3CB6Ch m_pMapList[]->m_ApocalypseBossMobNpcID
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error ApocalypseBossMob - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_iApocalypseBossMobNpcID = atoi(token);
					cReadModeB = 2;
					break;
				case 2: // 3CB70h m_pMapList[]->ApocalypseBossMobRectX1
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error ApocalypseBossMob - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_sApocalypseBossMobRect.left = atoi(token);
					cReadModeB = 3;
					break;
				case 3: // 3CB74h m_pMapList[]->ApocalypseBossMobRectY1
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error ApocalypseBossMob - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_sApocalypseBossMobRect.top = atoi(token);
					cReadModeB = 4;
					break;
				case 4: // 3CB78h m_pMapList[]->ApocalypseBossMobRectX2
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error ApocalypseBossMob - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_sApocalypseBossMobRect.right = atoi(token);
					cReadModeB = 5;
					break;
				case 5: // 3CB7Ch m_pMapList[]->ApocalypseBossMobRectY2
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error ApocalypseBossMob - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_sApocalypseBossMobRect.bottom = atoi(token);
					cReadModeA = 0;
					cReadModeB = 0;
					break;
				}
				break;

			case 28: //DynamicGateType // 28
				if (_bGetIsStringIsNumber(token) == false) {
					PutLogList("(!!!) CRITICAL ERROR! Map Info file error DynamicGateType - Wrong Data format.");
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pMapList[iMapIndex]->m_cDynamicGateType = atoi(token);
				// DynamicGateType = 1: Opens Gate when Apoc begins
				// DynamicGateType = 2: Open the gate when map is empty
				// DynamicGateType = 3: Gate stays closed will change to 4 value to open it, along with a boss spawn.
				// DynamicGateType = 4: Gate is openned. (set by server when Abaddon is spawning)
				// DynamicGateType = 5: Gate is used by GM command

				cReadModeA = 0;
				cReadModeB = 0;
				break;

			case 29: //DynamicGateCoord // 29
				// DynamicGateCoord	= 59 196 60 197        abaddon	   -1  -1
				switch (cReadModeB) {
				case 1: // 3CA20h
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error ApocalypseBossMob - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_sDynamicGateCoordRectX1 = atoi(token);
					cReadModeB = 2;
					break;

				case 2: // 3CA24h
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error ApocalypseBossMob - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_sDynamicGateCoordRectY1 = atoi(token);
					cReadModeB = 3;
					break;

				case 3: // 3CA28h
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error ApocalypseBossMob - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_sDynamicGateCoordRectX2 = atoi(token);
					cReadModeB = 4;
					break;

				case 4: // 3CA2Ch
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error ApocalypseBossMob - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_sDynamicGateCoordRectY2 = atoi(token);
					cReadModeB = 5;
					break;

				case 5: // 3CA30h
					ZeroMemory(m_pMapList[iMapIndex]->m_cDynamicGateCoordDestMap, sizeof(m_pMapList[iMapIndex]->m_cDynamicGateCoordDestMap));
					memcpy(m_pMapList[iMapIndex]->m_cDynamicGateCoordDestMap, token, strlen(token));
					cReadModeB = 6;
					break;

				case 6: // 3CA3Ch
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error ApocalypseBossMob - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_sDynamicGateCoordTgtX = atoi(token);
					cReadModeB = 7;
					break;

				case 7: // (ty = 3CB60h) unknown (3CA3Eh)
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error ApocalypseBossMob - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_sDynamicGateCoordTgtY = atoi(token);
					cReadModeA = 0;
					cReadModeB = 0;
					break;
				}
				break;

			case 30: // RecallImpossible // 30
				if (_bGetIsStringIsNumber(token) == false) {
					PutLogList("(!!!) CRITICAL ERROR! Map Info file error RecallImpossible -  Wrong Data format.");
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pMapList[iMapIndex]->m_bIsRecallImpossible = (bool)atoi(token);
				cReadModeA = 0;
				cReadModeB = 0;
				break;

			case 31: // ApocalypseMap // 31
				if (_bGetIsStringIsNumber(token) == false) {
					PutLogList("(!!!) CRITICAL ERROR! Map Info file error ApocalypseMap -  Wrong Data format.");
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pMapList[iMapIndex]->m_bIsApocalypseMap = (bool)atoi(token);
				cReadModeA = 0;
				cReadModeB = 0;
				break;

			case 32: // CitizenLimit // 32
				if (_bGetIsStringIsNumber(token) == false) {
					PutLogList("(!!!) CRITICAL ERROR! Map Info file error CitizenLimit -  Wrong Data format.");
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pMapList[iMapIndex]->m_bIsCitizenLimit = atoi(token);
				cReadModeA = 0;
				cReadModeB = 0;
				break;

			case 33: // HeldenianMap
				if (_bGetIsStringIsNumber(token) == false)
				{
					PutLogList("(!!!) CRITICAL ERROR! Map Info file error CitizenLimit -  Wrong Data format.");
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pMapList[iMapIndex]->m_bIsHeldenianMap = atoi(token);
				cReadModeA = 0;
				cReadModeB = 0;
				break;

			case 34: // HeldenianTower
				switch (cReadModeB) {
				case 1: // NpcID
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error Hedenian tower type id - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_stHeldenianTower[iIndexHT].sTypeID = atoi(token);
					cReadModeB = 2;
					break;
				case 2: // side 
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error Hedenian Tower Side - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_stHeldenianTower[iIndexHT].cSide = atoi(token);
					cReadModeB = 3;
					break;
				case 3: // sX
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error Hedenian Tower X pos - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_stHeldenianTower[iIndexHT].dX = atoi(token);
					cReadModeB = 4;
					break;
				case 4: // sY
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error Hedenian Tower Y pos - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_stHeldenianTower[iIndexHT].dY = atoi(token);
					iIndexHT++;
					cReadModeA = 0;
					cReadModeB = 0;
					break;
				}
				break;

			case 35: // HeldenianModeMap
				if (_bGetIsStringIsNumber(token) == false) {
					PutLogList("(!!!) CRITICAL ERROR! Map Info file error Hedenian Map Mode - Wrong Data format.");
					delete[] pContents;
					delete pStrTok;
					return false;
				}
				m_pMapList[iMapIndex]->m_cHeldenianModeMap = atoi(token);
				cReadModeA = 0;
				cReadModeB = 0;
				break;

			case 36: // HeldenianWinningZone
				switch (cReadModeB) {
				case 1:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error Hedenian Map Mode - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_sHeldenianWinningZoneX = atoi(token);
					cReadModeB = 2;
					break;
				case 2:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error Hedenian Map Mode - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_sHeldenianWinningZoneY = atoi(token);
					cReadModeA = 0;
					cReadModeB = 0;
					break;
				}
				break;

			case 37: // HeldenianGateDoor // 37
				switch (cReadModeB) {
				case 1:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error Heldenian Door Direction - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_stHeldenianGateDoor[iIndexGD].cDir = atoi(token);
					cReadModeB = 2;
					break;
				case 2:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error Heldenian Door X pos - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_stHeldenianGateDoor[iIndexGD].dX = atoi(token);
					cReadModeB = 3;
					break;
				case 3:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! Map Info file error Heldenian Door Y pos - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pMapList[iMapIndex]->m_stHeldenianGateDoor[iIndexGD].dY = atoi(token);
					iIndexGD++;
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
			if (memcmp(token, "teleport-loc", 12) == 0) {
				m_pMapList[iMapIndex]->m_pTeleportLoc[iTeleportLocIndex] = new class CTeleportLoc;
				cReadModeA = 1;
				cReadModeB = 1;
			}

			if (memcmp(token, "waypoint", 8) == 0) {
				cReadModeA = 2;
				cReadModeB = 1;
			}

			if (memcmp(token, "npc", 3) == 0) {
				cReadModeA = 3;
				cReadModeB = 1;
			}

			if (memcmp(token, "random-mob-generator", 20) == 0) {
				cReadModeA = 4;
				cReadModeB = 1;
			}

			if (memcmp(token, "maximum-object", 14) == 0)
				cReadModeA = 5;

			if (memcmp(token, "npc-avoidrect", 13) == 0) {
				cReadModeA = 6;
				cReadModeB = 1;
			}

			if (memcmp(token, "spot-mob-generator", 18) == 0) {
				cReadModeA = 7;
				cReadModeB = 1;
			}

			if (memcmp(token, "map-location", 12) == 0)
				cReadModeA = 8;

			if (memcmp(token, "initial-point", 13) == 0) {
				cReadModeA = 9;
				cReadModeB = 1;
			}

			if (memcmp(token, "no-attack-area", 14) == 0) {
				cReadModeA = 10;
				cReadModeB = 1;
			}

			if (memcmp(token, "fixed-dayornight-mode", 21) == 0) cReadModeA = 11;

			if (memcmp(token, "fish-point", 10) == 0) {
				cReadModeA = 12;
				cReadModeB = 1;
			}

			if (memcmp(token, "max-fish", 8) == 0)		cReadModeA = 13;
			if (memcmp(token, "type", 4) == 0)			cReadModeA = 14;
			if (memcmp(token, "level-limit", 11) == 0)	cReadModeA = 15;

			if (memcmp(token, "mineral-generator", 17) == 0) {
				cReadModeA = 16;
				cReadModeB = 1;
			}

			if (memcmp(token, "mineral-point", 13) == 0) {
				cReadModeA = 17;
				cReadModeB = 1;
			}

			if (memcmp(token, "max-mineral", 11) == 0)			cReadModeA = 18;
			if (memcmp(token, "upper-level-limit", 17) == 0)	cReadModeA = 19;	// v1.4
			if (memcmp(token, "strategic-point", 15) == 0) {	// v1.41
				cReadModeA = 20;
				cReadModeB = 1;
			}

			if (memcmp(token, "energy-sphere-creation-point", 28) == 0) {
				cReadModeA = 21;
				cReadModeB = 1;
			}

			if (memcmp(token, "energy-sphere-goal-point", 24) == 0) {
				cReadModeA = 22;
				cReadModeB = 1;
			}

			if (memcmp(token, "strike-point", 12) == 0) {
				cReadModeA = 23;
				cReadModeB = 1;
			}

			if (memcmp(token, "item-event", 10) == 0) {
				cReadModeA = 24;
				cReadModeB = 1;
			}

			if (memcmp(token, "energy-sphere-auto-creation", 27) == 0) {
				cReadModeA = 0;
				cReadModeB = 0;
				m_pMapList[iMapIndex]->m_bIsEnergySphereAutoCreation = true;
			}

			if (memcmp(token, "mobevent-amount", 15) == 0) {
				cReadModeA = 25;
				cReadModeB = 1;
			}

			if (memcmp(token, "ApocalypseMobGenType", 20) == 0) {
				cReadModeA = 26;
				cReadModeB = 1;
			}

			if (memcmp(token, "ApocalypseBossMob", 17) == 0) {
				cReadModeA = 27;
				cReadModeB = 1;
			}

			if (memcmp(token, "DynamicGateType", 15) == 0) {
				cReadModeA = 28;
				cReadModeB = 1;
			}

			if (memcmp(token, "DynamicGateCoord", 16) == 0) {
				cReadModeA = 29;
				cReadModeB = 1;
			}

			if (memcmp(token, "RecallImpossible", 16) == 0) {
				cReadModeA = 30;
				cReadModeB = 1;
			}

			if (memcmp(token, "ApocalypseMap", 13) == 0) {
				cReadModeA = 31;
				cReadModeB = 1;
			}

			if (memcmp(token, "CitizenLimit", 12) == 0) {
				cReadModeA = 32;
				cReadModeB = 1;
			}

			if (memcmp(token, "HeldenianMap", 12) == 0) {
				cReadModeA = 33;
				cReadModeB = 1;
			}

			if (memcmp(token, "HeldenianTower", 14) == 0) {
				cReadModeA = 34;
				cReadModeB = 1;
			}

			if (memcmp(token, "HeldenianModeMap", 16) == 0) {
				cReadModeA = 35;
				cReadModeB = 1;
			}

			if (memcmp(token, "HeldenianWinningZone", 20) == 0) {
				cReadModeA = 36;
				cReadModeB = 1;
			}

			if (memcmp(token, "HeldenianGateDoor", 17) == 0) {
				cReadModeA = 37;
				cReadModeB = 1;
			}

			if (memcmp(token, "[END-MAP-INFO]", 14) == 0) {
				cReadModeA = 0;
				cReadModeB = 0;
				goto RMI_SKIPDECODING;
			}
		}
		token = pStrTok->pGet();
	}

RMI_SKIPDECODING:;

	delete pStrTok;
	delete[] pContents;

	if ((cReadModeA != 0) || (cReadModeB != 0)) {
		PutLogList("(!!!) CRITICAL ERROR! map info file contents error!");
		return false;
	}

	wsprintf(cTxt, "(!) Map info file decoding(%s) - success! TL(%d) WP(%d) LNPC(%d) MXO(%d) RMG(%d / %d)", cFn, iTeleportLocIndex, iWayPointCfgIndex, iTotalNpcSetting, m_pMapList[iMapIndex]->m_iMaximumObject, m_pMapList[iMapIndex]->m_bRandomMobGenerator, m_pMapList[iMapIndex]->m_cRandomMobGeneratorLevel);
	PutLogList(cTxt);
	m_pMapList[iMapIndex]->_SetupNoAttackArea();
	m_pMapList[iMapIndex]->bApocalypseGate();

	return true;
}

int CGame::iRequestPanningMapDataRequest(int iClientH, char* pData)
{
	char* cp, cDir, cData[3000];
	UINT32* dwp;
	UINT16* wp;
	short* sp, dX, dY;
	int   iRet, iSize;

	if (m_pClientList[iClientH] == 0) return 0;
	if (m_pClientList[iClientH]->m_bIsObserverMode == false) return 0;
	if (m_pClientList[iClientH]->m_bIsKilled == true) return 0;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return 0;

	dX = m_pClientList[iClientH]->m_sX;
	dY = m_pClientList[iClientH]->m_sY;

	cDir = *(pData + DEF_INDEX2_MSGTYPE + 2);
	if ((cDir <= 0) || (cDir > 8)) return 0;

	switch (cDir) {
	case 1:	dY--; break; // responding when mouse is placed north
	case 2:	dX++; dY--;	break;
	case 3:	dX++; break;
	case 4:	dX++; dY++;	break;
	case 5: dY++; break;
	case 6:	dX--; dY++;	break;
	case 7:	dX--; break; // responding when mouse placed at west side of screen
	case 8:	dX--; dY--;	break; // responding when mouse is placed north west

	/*
	player is in the center, and is trying to pan,
	directions not responding or causing a break will be kept as X,
	others are the cDir case...
			8	1	X

			7	_	X

			X	X	X
	*/
	}

	m_pClientList[iClientH]->m_sX = dX;
	m_pClientList[iClientH]->m_sY = dY;
	m_pClientList[iClientH]->m_cDir = cDir;

	dwp = (UINT32*)(cData + DEF_INDEX4_MSGID);
	*dwp = MSGID_RESPONSE_PANNING;
	wp = (UINT16*)(cData + DEF_INDEX2_MSGTYPE);
	*wp = DEF_OBJECTMOVE_CONFIRM;

	cp = (char*)(cData + DEF_INDEX2_MSGTYPE + 2);

	// centu - 800x600
	sp = (short*)cp;
	*sp = (short)(dX - 12);
	cp += 2;

	sp = (short*)cp;
	*sp = (short)(dY - 9);
	cp += 2;

	*cp = cDir;
	cp++;

	// centu - 800x600
	iSize = iComposeMoveMapData((short)(dX - 12), (short)(dY - 9), iClientH, cDir, cp);
	iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, iSize + 12 + 1 + 4);
	switch (iRet) {
	case DEF_XSOCKEVENT_QUENEFULL:
	case DEF_XSOCKEVENT_SOCKETERROR:
	case DEF_XSOCKEVENT_CRITICALERROR:
	case DEF_XSOCKEVENT_SOCKETCLOSED:
		// ¸Þ½ÃÁö¸¦ º¸³¾¶§ ¿¡·¯°¡ ¹ß»ýÇß´Ù¸é Á¦°ÅÇÑ´Ù.
		DeleteClient(iClientH, true, true);
		return 0;
	}

	return 1;
}

int CGame::iGetMapIndex(char* pMapName)
{
	int i;
	char cTmpName[256];

	ZeroMemory(cTmpName, sizeof(cTmpName));
	strcpy(cTmpName, pMapName);

	for (i = 0; i < DEF_MAXMAPS; i++)
		if (m_pMapList[i] != 0) {
			if (memcmp(m_pMapList[i]->m_cName, pMapName, 10) == 0)
				return i;
		}

	return -1;
}

void CGame::ClearMap()
{
	int i, k, j, m;
	class CItem* pItem;
	short sRemainItemID;
	char cRemainItemColor;
	UINT32 dwRemainItemAttr;
	for (m = 0; m < DEF_MAXMAPS; m++)
	{
		if (m_pMapList[m] != 0)
		{
			for (j = 1; j < m_pMapList[m]->m_sSizeX; j++)
			{
				for (k = 1; k < m_pMapList[m]->m_sSizeY; k++)
				{
					do
					{
						pItem = m_pMapList[m]->pGetItem(j, k, &sRemainItemID, &cRemainItemColor, &dwRemainItemAttr);
						if (pItem != 0)
						{
							delete pItem;
						}
					} while (pItem != 0);
				}
			}
		}
	}
	for (i = 1; i < DEF_MAXCLIENTS; i++)
	{
		if (m_pClientList[i] != 0)
		{
			RequestTeleportHandler(i, "2 ", m_pClientList[i]->m_cMapName, m_pClientList[i]->m_sX, m_pClientList[i]->m_sY);
		}
	}
	m_iNotifyCleanMap = true;
}

/*********************************************************************************************************************
**  CGame::iComposeInitMapData(short sX, short sY, int iClientH, char * pData)										**
**  DESCRIPTION			::																							**
**  LAST_UPDATED		:: March 16, 2005; 11:59 AM; Hypnotoad														**
**	RETURN_VALUE		:: void																						**
**  NOTES				::	- added iGetPlayerStatus()																**
**	MODIFICATION		::  - added check for invis hack															**
**********************************************************************************************************************/
int CGame::iComposeInitMapData(short sX, short sY, int iClientH, char* pData)
{
	int* ip, ix, iy, iSize, iTileExists;
	class CTile* pTile;
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
	
	// centu - 800x600
	for (iy = 0; iy < 19; iy++)
		for (ix = 0; ix < 25; ix++) {
			
			pTile = (class CTile*)(m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_pTile + (sX + ix) + (sY + iy) * m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_sSizeY);

			if ((pTile->m_sOwner != 0) || (pTile->m_sDeadOwner != 0) ||
				(pTile->m_pItem[0] != 0) || (pTile->m_sDynamicObjectType != 0)) {
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
						else {
							wsprintf(G_cTxt, "Empty player handle: %d", pTile->m_sOwner);
							PutLogList(G_cTxt);
							pTile->m_sOwner = 0;
						}
					}

					if (pTile->m_cOwnerClass == DEF_OWNERTYPE_NPC) {
						if (m_pNpcList[pTile->m_sOwner] != 0) ucHeader = ucHeader | 0x01;
						else pTile->m_sOwner = 0;
					}
				}
				if (pTile->m_sDeadOwner != 0) {
					if (pTile->m_cDeadOwnerClass == DEF_OWNERTYPE_PLAYER) {
						if (m_pClientList[pTile->m_sDeadOwner] != 0) ucHeader = ucHeader | 0x02;
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
						// Object ID number(Player) : 1~10000
						sp = (short*)cp;
						*sp = pTile->m_sOwner;
						cp += 2;
						iSize += 2;
						// object type
						sp = (short*)cp;
						*sp = m_pClientList[pTile->m_sOwner]->m_sType;
						cp += 2;
						iSize += 2;
						// dir
						*cp = m_pClientList[pTile->m_sOwner]->m_cDir;
						cp++;
						iSize++;
						// Appearance1
						sp = (short*)cp;
						*sp = m_pClientList[pTile->m_sOwner]->m_sAppr1;
						cp += 2;
						iSize += 2;
						// Appearance2
						sp = (short*)cp;
						*sp = m_pClientList[pTile->m_sOwner]->m_sAppr2;
						cp += 2;
						iSize += 2;
						// Appearance3
						sp = (short*)cp;
						*sp = m_pClientList[pTile->m_sOwner]->m_sAppr3;
						cp += 2;
						iSize += 2;
						// Appearance4
						sp = (short*)cp;
						*sp = m_pClientList[pTile->m_sOwner]->m_sAppr4;
						cp += 2;
						iSize += 2;
						// v1.4 ApprColor
						ip = (int*)cp;
						*ip = m_pClientList[pTile->m_sOwner]->m_iApprColor;
						cp += 4;
						iSize += 4;
						// Status
						ip = (int*)cp;


						iTemp = m_pClientList[pTile->m_sOwner]->m_iStatus;
						
						iTemp = 0x0FFFFFFF & iTemp;
						iTemp2 = iGetPlayerABSStatus(pTile->m_sOwner, iClientH); 
						iTemp = (iTemp | (iTemp2 << 28));
						*ip = iTemp;
						cp += 4;
						iSize += 4;
						// Name
						memcpy(cp, m_pClientList[pTile->m_sOwner]->m_cCharName, 10);
						cp += 10;
						iSize += 10;
						break;

					case DEF_OWNERTYPE_NPC:
						// Object ID number(NPC) : 10000~
						sp = (short*)cp;
						*sp = pTile->m_sOwner + 10000;
						cp += 2;
						iSize += 2;
						// object type
						sp = (short*)cp;
						*sp = m_pNpcList[pTile->m_sOwner]->m_sType;
						cp += 2;
						iSize += 2;
						// dir
						*cp = m_pNpcList[pTile->m_sOwner]->m_cDir;
						cp++;
						iSize++;
						// Appr2
						sp = (short*)cp;
						*sp = m_pNpcList[pTile->m_sOwner]->m_sAppr2;
						cp += 2;
						iSize += 2;
						// Status
						ip = (int*)cp;
						iTemp = m_pNpcList[pTile->m_sOwner]->m_iStatus;
						iTemp = 0x0FFFFFFF & iTemp;// »óÀ§ 4ºñÆ® Å¬¸®¾î
						iTemp2 = iGetNpcRelationship(pTile->m_sOwner, iClientH);
						iTemp = (iTemp | (iTemp2 << 28));//Original : 12
						*ip = iTemp;
						cp += 4;
						iSize += 4;
						// Name
						memcpy(cp, m_pNpcList[pTile->m_sOwner]->m_cName, 5);
						cp += 5;
						iSize += 5;
						break;
					}
				}
				if ((ucHeader & 0x02) != 0) {
					switch (pTile->m_cDeadOwnerClass) {
					case DEF_OWNERTYPE_PLAYER:
						// Object ID number : 1~10000
						sp = (short*)cp;
						*sp = pTile->m_sDeadOwner;
						cp += 2;
						iSize += 2;
						// object type
						sp = (short*)cp;
						*sp = m_pClientList[pTile->m_sDeadOwner]->m_sType;
						cp += 2;
						iSize += 2;
						// dir
						*cp = m_pClientList[pTile->m_sDeadOwner]->m_cDir;
						cp++;
						iSize++;
						// Appearance1
						sp = (short*)cp;
						*sp = m_pClientList[pTile->m_sDeadOwner]->m_sAppr1;
						cp += 2;
						iSize += 2;
						// Appearance2
						sp = (short*)cp;
						*sp = m_pClientList[pTile->m_sDeadOwner]->m_sAppr2;
						cp += 2;
						iSize += 2;
						// Appearance3
						sp = (short*)cp;
						*sp = m_pClientList[pTile->m_sDeadOwner]->m_sAppr3;
						cp += 2;
						iSize += 2;
						// Appearance4
						sp = (short*)cp;
						*sp = m_pClientList[pTile->m_sDeadOwner]->m_sAppr4;
						cp += 2;
						iSize += 2;
						// v1.4 ApprColor
						ip = (int*)cp;
						*ip = m_pClientList[pTile->m_sDeadOwner]->m_iApprColor;
						cp += 4;
						iSize += 4;
						// Status
						ip = (int*)cp;
						

						iTemp = m_pClientList[pTile->m_sDeadOwner]->m_iStatus;
						
						iTemp = 0x0FFFFFFF & iTemp;
						iTemp2 = iGetPlayerABSStatus(pTile->m_sDeadOwner, iClientH); 
						iTemp = (iTemp | (iTemp2 << 28));//Original : 12
						*ip = iTemp;
						cp += 4;
						iSize += 4;
						// Name
						memcpy(cp, m_pClientList[pTile->m_sDeadOwner]->m_cCharName, 10);
						cp += 10;
						iSize += 10;
						break;

					case DEF_OWNERTYPE_NPC:
						// Object ID number : 10000	~
						sp = (short*)cp;
						*sp = pTile->m_sDeadOwner + 10000;
						cp += 2;
						iSize += 2;
						// object type
						sp = (short*)cp;
						*sp = m_pNpcList[pTile->m_sDeadOwner]->m_sType;
						cp += 2;
						iSize += 2;
						// dir
						*cp = m_pNpcList[pTile->m_sDeadOwner]->m_cDir;
						cp++;
						iSize++;
						// Appr2
						sp = (short*)cp;
						*sp = m_pNpcList[pTile->m_sDeadOwner]->m_sAppr2;
						cp += 2;
						iSize += 2;
						// Status
						ip = (int*)cp;
						iTemp = m_pNpcList[pTile->m_sDeadOwner]->m_iStatus;
						iTemp = 0x0FFFFFFF & iTemp;
						iTemp2 = iGetNpcRelationship(pTile->m_sDeadOwner, iClientH);
						iTemp = (iTemp | (iTemp2 << 28));//Original : 12
						*ip = iTemp;
						cp += 4;//Original 2
						iSize += 4;//Original 2
						// Name
						memcpy(cp, m_pNpcList[pTile->m_sDeadOwner]->m_cName, 5);
						cp += 5;
						iSize += 5;
						break;
					}
				}

				if (pTile->m_pItem[0] != 0) {

					// Centu - id num
					sp = (short*)cp;
					*sp = pTile->m_pItem[0]->m_sIDnum;
					cp += 2;
					iSize += 2;

					*cp = pTile->m_pItem[0]->m_cItemColor;
					cp++;
					iSize++;

					// Centu - attribute
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

/*********************************************************************************************************************
**  void CGame::SendEventToNearClient_TypeA(short sOwnerH, char cOwnerType, UINT32 dwMsgID, UINT16 wMsgType,			**
**		short sV1, short sV2, short sV3)																			**
**  DESCRIPTION			:: updates client screen																	**
**  LAST_UPDATED		:: March 16, 2005; 12:26 AM; Hypnotoad														**
**	RETURN_VALUE		:: void																						**
**  NOTES				::	- updated m_iStatus calculation															**
**							- changed npc psock sendmsg to 29 from 27												**
**	MODIFICATION		::  - should have a invis hack check (commented)											**
**********************************************************************************************************************/
void CGame::SendEventToNearClient_TypeA(short sOwnerH, char cOwnerType, UINT32 dwMsgID, UINT16 wMsgType, short sV1, short sV2, short sV3)
{
	int* ip, i, iRet, iShortCutIndex;
	char* cp_a, * cp_s, * cp_sv, cData_All[200], cData_Srt[200], cData_Srt_Av[200];
	UINT32* dwp;
	UINT16* wp;
	int* ipStatus, iDumm;
	short* sp, sRange, sX, sY;
	bool  bFlag, cOwnerSend;
	char cKey;
	int iTemp3, iTemp, iTemp2;

	ZeroMemory(cData_All, sizeof(cData_All));
	ZeroMemory(cData_Srt, sizeof(cData_Srt));
	ZeroMemory(cData_Srt_Av, sizeof(cData_Srt_Av));
	ipStatus = (int*)&iDumm;
	cKey = (rand() % 245) + 1;

	dwp = (UINT32*)(cData_All + DEF_INDEX4_MSGID);
	*dwp = dwMsgID;
	wp = (UINT16*)(cData_All + DEF_INDEX2_MSGTYPE);
	*wp = wMsgType;

	dwp = (UINT32*)(cData_Srt + DEF_INDEX4_MSGID);
	*dwp = dwMsgID;
	wp = (UINT16*)(cData_Srt + DEF_INDEX2_MSGTYPE);
	*wp = wMsgType;

	dwp = (UINT32*)(cData_Srt_Av + DEF_INDEX4_MSGID);
	*dwp = dwMsgID;
	wp = (UINT16*)(cData_Srt_Av + DEF_INDEX2_MSGTYPE);
	*wp = wMsgType;

	cp_a = (char*)(cData_All + DEF_INDEX2_MSGTYPE + 2);
	cp_s = (char*)(cData_Srt + DEF_INDEX2_MSGTYPE + 2);
	cp_sv = (char*)(cData_Srt_Av + DEF_INDEX2_MSGTYPE + 2);


	if ((dwMsgID == MSGID_EVENT_LOG) || (wMsgType == DEF_OBJECTMOVE) || (wMsgType == DEF_OBJECTRUN) ||
		(wMsgType == DEF_OBJECTATTACKMOVE) || (wMsgType == DEF_OBJECTDAMAGEMOVE) || (wMsgType == DEF_OBJECTDYING))
		sRange = 1;
	else sRange = 0;


	if (cOwnerType == DEF_OWNERTYPE_PLAYER) {
		if (m_pClientList[sOwnerH] == 0) return;
		switch (wMsgType) {
		case DEF_OBJECTNULLACTION:
		case DEF_OBJECTDAMAGE:
		case DEF_OBJECTDYING:
		case DEF_MSGTYPE_CONFIRM:
			cOwnerSend = true;
			break;
		default:
			cOwnerSend = false;
			break;
		}

		wp = (UINT16*)cp_a;
		*wp = sOwnerH;
		cp_a += 2;

		sp = (short*)cp_a;
		sX = m_pClientList[sOwnerH]->m_sX;
		*sp = sX;
		cp_a += 2;

		sp = (short*)cp_a;
		sY = m_pClientList[sOwnerH]->m_sY;
		*sp = sY;
		cp_a += 2;

		sp = (short*)cp_a;
		*sp = m_pClientList[sOwnerH]->m_sType;
		cp_a += 2;

		*cp_a = m_pClientList[sOwnerH]->m_cDir;
		cp_a++;

		memcpy(cp_a, m_pClientList[sOwnerH]->m_cCharName, 10);
		cp_a += 10;

		sp = (short*)cp_a;
		*sp = m_pClientList[sOwnerH]->m_sAppr1;
		cp_a += 2;

		sp = (short*)cp_a;
		*sp = m_pClientList[sOwnerH]->m_sAppr2;
		cp_a += 2;

		sp = (short*)cp_a;
		*sp = m_pClientList[sOwnerH]->m_sAppr3;
		cp_a += 2;

		sp = (short*)cp_a;
		*sp = m_pClientList[sOwnerH]->m_sAppr4;
		cp_a += 2;

		ip = (int*)cp_a;
		*ip = m_pClientList[sOwnerH]->m_iApprColor;
		cp_a += 4;

		ip = (int*)cp_a;
		ipStatus = ip;
		*ip = m_pClientList[sOwnerH]->m_iStatus;
		cp_a += 4;

		iTemp = m_pClientList[sOwnerH]->m_iStatus;
		iTemp = 0x0F0 & iTemp;
		iTemp3 = m_pClientList[sOwnerH]->m_iStatus & 0x0F0FFFF7F;

		if (wMsgType == DEF_OBJECTNULLACTION) {
			if (m_pClientList[sOwnerH]->m_bIsKilled == true)
				*cp_a = 1;
			else *cp_a = 0;
		}
		else *cp_a = 0;
		cp_a++;

		wp = (UINT16*)cp_s;
		*wp = sOwnerH + 30000;
		cp_s += 2;

		*cp_s = m_pClientList[sOwnerH]->m_cDir;
		cp_s++;

		//50Cent - No Critical Damage Limit
		ip = (int*)cp_s;
		*ip = (int)sV1;
		cp_s += 4;

		*cp_s = (unsigned char)sV2;
		cp_s++;

		sp = (short*)cp_s;
		sX = m_pClientList[sOwnerH]->m_sX;
		*sp = sX;
		cp_s += 2;

		sp = (short*)cp_s;
		sY = m_pClientList[sOwnerH]->m_sY;
		*sp = sY;
		cp_s += 2;

		wp = (UINT16*)cp_sv;
		*wp = sOwnerH + 30000;
		cp_sv += 2;

		*cp_sv = m_pClientList[sOwnerH]->m_cDir;
		cp_sv++;

		*cp_sv = sV1 - sX;
		cp_sv++;

		*cp_sv = sV2 - sY;
		cp_sv++;

		sp = (short*)cp_sv;
		*sp = sV3;
		cp_sv += 2;

		bFlag = true;
		iShortCutIndex = 0;
		while (bFlag) {
			i = m_iClientShortCut[iShortCutIndex];
			iShortCutIndex++;
			if (i == 0) bFlag = false;
			if ((bFlag == true) && (m_pClientList[i] != 0) && (m_pClientList[i]->m_bIsInitComplete == true))

				if ((m_pClientList[i]->m_cMapIndex == m_pClientList[sOwnerH]->m_cMapIndex) &&
					(m_pClientList[i]->m_sX >= m_pClientList[sOwnerH]->m_sX - 12 - sRange) &&
					(m_pClientList[i]->m_sX <= m_pClientList[sOwnerH]->m_sX + 12 + sRange) &&
					(m_pClientList[i]->m_sY >= m_pClientList[sOwnerH]->m_sY - 10 - sRange) &&
					(m_pClientList[i]->m_sY <= m_pClientList[sOwnerH]->m_sY + 10 + sRange)) {
					if (m_pClientList[sOwnerH]->m_cSide != m_pClientList[i]->m_cSide) {
						if (m_pClientList[i]->m_iAdminUserLevel > 0) {
							iTemp = m_pClientList[sOwnerH]->m_iStatus;
						}
						else if (i != sOwnerH) {
							iTemp = iTemp3;
						}
						else {
							iTemp = m_pClientList[sOwnerH]->m_iStatus;
						}
					}
					else {
						iTemp = m_pClientList[sOwnerH]->m_iStatus;
					}

					iTemp = 0x0FFFFFFF & iTemp;
					iTemp2 = iGetPlayerABSStatus(sOwnerH, i);
					iTemp = (iTemp | (iTemp2 << 28));
					*ipStatus = iTemp;

					if ((m_pClientList[i]->m_sX >= m_pClientList[sOwnerH]->m_sX - 11) &&
						(m_pClientList[i]->m_sX <= m_pClientList[sOwnerH]->m_sX + 11) &&
						(m_pClientList[i]->m_sY >= m_pClientList[sOwnerH]->m_sY - 9) &&
						(m_pClientList[i]->m_sY <= m_pClientList[sOwnerH]->m_sY + 9)) {

						switch (wMsgType) {
						case DEF_MSGTYPE_CONFIRM:
						case DEF_MSGTYPE_REJECT:
						case DEF_OBJECTNULLACTION:
							if (cOwnerSend == true)
								iRet = m_pClientList[i]->m_pXSock->iSendMsg(cData_All, 43, cKey);
							else
								if (i != sOwnerH)
									iRet = m_pClientList[i]->m_pXSock->iSendMsg(cData_All, 43, cKey);
							break;

						case DEF_OBJECTATTACK:
						case DEF_OBJECTATTACKMOVE:
							if (cOwnerSend == true)
								iRet = m_pClientList[i]->m_pXSock->iSendMsg(cData_Srt_Av, 13, cKey);
							else
								if (i != sOwnerH)
									iRet = m_pClientList[i]->m_pXSock->iSendMsg(cData_Srt_Av, 13, cKey);
							break;

						//50Cent - No Critical Damage Limit
						case DEF_OBJECTMAGIC:
							if (cOwnerSend == true)
								iRet = m_pClientList[i]->m_pXSock->iSendMsg(cData_Srt, 11, cKey);
							else
								if (i != sOwnerH)
									iRet = m_pClientList[i]->m_pXSock->iSendMsg(cData_Srt, 11, cKey);
							break;
						case DEF_OBJECTDAMAGE:
						case DEF_OBJECTDAMAGEMOVE:
							if (cOwnerSend == true)
								iRet = m_pClientList[i]->m_pXSock->iSendMsg(cData_Srt, 14, cKey);
							else
								if (i != sOwnerH)
									iRet = m_pClientList[i]->m_pXSock->iSendMsg(cData_Srt, 14, cKey);
							break;

						case DEF_OBJECTDYING:
							if (cOwnerSend == true)
								iRet = m_pClientList[i]->m_pXSock->iSendMsg(cData_Srt, 18, cKey);//50Cent - No Critical Damage Limit 15
							else
								if (i != sOwnerH)
									iRet = m_pClientList[i]->m_pXSock->iSendMsg(cData_Srt, 18, cKey);//50Cent - No Critical Damage Limit 15
							break;

						default:
							if (cOwnerSend == true)
								iRet = m_pClientList[i]->m_pXSock->iSendMsg(cData_Srt, 9, cKey);
							else
								if (i != sOwnerH)
									iRet = m_pClientList[i]->m_pXSock->iSendMsg(cData_Srt, 9, cKey);
							break;
						}
					}
					else {
						switch (wMsgType) {
						case DEF_MSGTYPE_CONFIRM:
						case DEF_MSGTYPE_REJECT:
						case DEF_OBJECTNULLACTION:
							if (cOwnerSend == true)
								iRet = m_pClientList[i]->m_pXSock->iSendMsg(cData_All, 43, cKey);
							else
								if (i != sOwnerH)
									iRet = m_pClientList[i]->m_pXSock->iSendMsg(cData_All, 43, cKey);
							break;

						case DEF_OBJECTATTACK:
						case DEF_OBJECTATTACKMOVE:
							if (cOwnerSend == true)
								iRet = m_pClientList[i]->m_pXSock->iSendMsg(cData_Srt_Av, 13, cKey);
							else
								if (i != sOwnerH)
									iRet = m_pClientList[i]->m_pXSock->iSendMsg(cData_Srt_Av, 13, cKey);
							break;

						//50Cent - No Critical Damage Limit
						case DEF_OBJECTMAGIC:
							if (cOwnerSend == true)
								iRet = m_pClientList[i]->m_pXSock->iSendMsg(cData_Srt, 11, cKey);
							else
								if (i != sOwnerH)
									iRet = m_pClientList[i]->m_pXSock->iSendMsg(cData_Srt, 11, cKey);
							break;
						case DEF_OBJECTDAMAGE:
						case DEF_OBJECTDAMAGEMOVE:
							if (cOwnerSend == true)
								iRet = m_pClientList[i]->m_pXSock->iSendMsg(cData_Srt, 14, cKey);
							else
								if (i != sOwnerH)
									iRet = m_pClientList[i]->m_pXSock->iSendMsg(cData_Srt, 14, cKey);
							break;

						case DEF_OBJECTDYING:
							if (cOwnerSend == true)
								iRet = m_pClientList[i]->m_pXSock->iSendMsg(cData_Srt, 18, cKey);//50Cent - No Critical Damage Limit 15
							else
								if (i != sOwnerH)
									iRet = m_pClientList[i]->m_pXSock->iSendMsg(cData_Srt, 18, cKey);//50Cent - No Critical Damage Limit 15
							break;

						default:
							if (cOwnerSend == true)
								iRet = m_pClientList[i]->m_pXSock->iSendMsg(cData_All, 43, cKey);
							else
								if (i != sOwnerH)
									iRet = m_pClientList[i]->m_pXSock->iSendMsg(cData_All, 43, cKey);
							break;
						}
					}
				}
		}
	}
	else {
		if (m_pNpcList[sOwnerH] == 0) return;

		wp = (UINT16*)cp_a;
		*wp = sOwnerH + 10000;
		cp_a += 2;

		sp = (short*)cp_a;
		sX = m_pNpcList[sOwnerH]->m_sX;
		*sp = sX;
		cp_a += 2;

		sp = (short*)cp_a;
		sY = m_pNpcList[sOwnerH]->m_sY;
		*sp = sY;
		cp_a += 2;

		sp = (short*)cp_a;
		*sp = m_pNpcList[sOwnerH]->m_sType;
		cp_a += 2;

		*cp_a = m_pNpcList[sOwnerH]->m_cDir;
		cp_a++;

		memcpy(cp_a, m_pNpcList[sOwnerH]->m_cName, 5);
		cp_a += 5;

		sp = (short*)cp_a;
		*sp = m_pNpcList[sOwnerH]->m_sAppr2;
		cp_a += 2;

		ip = (int*)cp_a;
		ipStatus = ip;
		*ip = m_pNpcList[sOwnerH]->m_iStatus;
		cp_a += 4;

		if (wMsgType == DEF_OBJECTNULLACTION) {
			if (m_pNpcList[sOwnerH]->m_bIsKilled == true)
				*cp_a = 1;
			else *cp_a = 0;
		}
		else *cp_a = 0;
		cp_a++;

		wp = (UINT16*)cp_s;
		*wp = sOwnerH + 40000;
		cp_s += 2;

		*cp_s = m_pNpcList[sOwnerH]->m_cDir;
		cp_s++;

		//50Cent - No Critical Damage Limit
		ip = (int*)cp_s;
		*ip = (int)sV1;
		cp_s += 4;

		*cp_s = (unsigned char)sV2;
		cp_s++;

		sp = (short*)cp_s;
		sX = m_pNpcList[sOwnerH]->m_sX;
		*sp = sX;
		cp_s += 2;
		sp = (short*)cp_s;
		sY = m_pNpcList[sOwnerH]->m_sY;
		*sp = sY;
		cp_s += 2;

		wp = (UINT16*)cp_sv;
		*wp = sOwnerH + 40000;
		cp_sv += 2;
		*cp_sv = m_pNpcList[sOwnerH]->m_cDir;
		cp_sv++;
		*cp_sv = sV1 - sX;
		cp_sv++;
		*cp_sv = sV2 - sY;
		cp_sv++;
		sp = (short*)cp_sv;
		*sp = sV3;
		cp_sv += 2;

		bFlag = true;
		iShortCutIndex = 0;

		while (bFlag) {
			i = m_iClientShortCut[iShortCutIndex];
			iShortCutIndex++;
			if (i == 0) bFlag = false;
			if ((bFlag == true) && (m_pClientList[i] != 0))
				if ((m_pClientList[i]->m_cMapIndex == m_pNpcList[sOwnerH]->m_cMapIndex) &&
					(m_pClientList[i]->m_sX >= m_pNpcList[sOwnerH]->m_sX - 12 - sRange) &&
					(m_pClientList[i]->m_sX <= m_pNpcList[sOwnerH]->m_sX + 12 + sRange) &&
					(m_pClientList[i]->m_sY >= m_pNpcList[sOwnerH]->m_sY - 10 - sRange) &&
					(m_pClientList[i]->m_sY <= m_pNpcList[sOwnerH]->m_sY + 10 + sRange)) {

					iTemp = *ipStatus;
					iTemp = 0x0FFFFFFF & iTemp;
					iTemp2 = iGetNpcRelationship(sOwnerH, i);
					iTemp = (iTemp | (iTemp2 << 28));
					*ipStatus = iTemp;

					if ((m_pClientList[i]->m_sX >= m_pNpcList[sOwnerH]->m_sX - 11) &&
						(m_pClientList[i]->m_sX <= m_pNpcList[sOwnerH]->m_sX + 11) &&
						(m_pClientList[i]->m_sY >= m_pNpcList[sOwnerH]->m_sY - 9) &&
						(m_pClientList[i]->m_sY <= m_pNpcList[sOwnerH]->m_sY + 9)) {
						switch (wMsgType) {
						case DEF_MSGTYPE_CONFIRM:
						case DEF_MSGTYPE_REJECT:
						case DEF_OBJECTNULLACTION:
							iRet = m_pClientList[i]->m_pXSock->iSendMsg(cData_All, 29, cKey); // changed from 27
							break;

						case DEF_OBJECTDYING:

							iRet = m_pClientList[i]->m_pXSock->iSendMsg(cData_Srt, 18, cKey);//50Cent - No Critical Damage Limit 15
							break;

						case DEF_OBJECTDAMAGE:
						case DEF_OBJECTDAMAGEMOVE:

							iRet = m_pClientList[i]->m_pXSock->iSendMsg(cData_Srt, 14, cKey);//50Cent - No Critical Damage Limit 11
							break;

						case DEF_OBJECTATTACK:
						case DEF_OBJECTATTACKMOVE:
							iRet = m_pClientList[i]->m_pXSock->iSendMsg(cData_Srt_Av, 13, cKey);
							break;

						default:
							iRet = m_pClientList[i]->m_pXSock->iSendMsg(cData_Srt, 9, cKey);
							break;

						}
					}
					else {
						switch (wMsgType) {
						case DEF_MSGTYPE_CONFIRM:
						case DEF_MSGTYPE_REJECT:
						case DEF_OBJECTNULLACTION:
							iRet = m_pClientList[i]->m_pXSock->iSendMsg(cData_All, 29, cKey); // changed from 27
							break;

						case DEF_OBJECTDYING:

							iRet = m_pClientList[i]->m_pXSock->iSendMsg(cData_Srt, 18, cKey);//50Cent - No Critical Damage Limit 15
							break;

						case DEF_OBJECTDAMAGE:
						case DEF_OBJECTDAMAGEMOVE:

							iRet = m_pClientList[i]->m_pXSock->iSendMsg(cData_Srt, 14, cKey);//50Cent - No Critical Damage Limit 11
							break;

						case DEF_OBJECTATTACK:
						case DEF_OBJECTATTACKMOVE:
							iRet = m_pClientList[i]->m_pXSock->iSendMsg(cData_Srt_Av, 13, cKey);
							break;

						default:
							iRet = m_pClientList[i]->m_pXSock->iSendMsg(cData_All, 29, cKey); // changed from 27
							break;

						}
					}
				}
		}
	}
}

void CGame::SendEventToNearClient_TypeB(UINT32 dwMsgID, UINT16 wMsgType, char cMapIndex, short sX, short sY, short sV1, short sV2, short sV3, UINT32 sV4)
{
	int i, iRet, iShortCutIndex;
	char* cp, cData[100];
	UINT32* dwp, dwTime;
	UINT16* wp;
	short* sp;
	bool bFlag;
	char  cKey;

	cKey = (char)(rand() % 245) + 1; // v1.4

	ZeroMemory(cData, sizeof(cData));

	dwp = (UINT32*)(cData + DEF_INDEX4_MSGID);
	*dwp = dwMsgID;
	wp = (UINT16*)(cData + DEF_INDEX2_MSGTYPE);
	*wp = wMsgType;

	cp = (char*)(cData + DEF_INDEX2_MSGTYPE + 2);

	sp = (short*)cp;
	*sp = sX;
	cp += 2;

	sp = (short*)cp;
	*sp = sY;
	cp += 2;

	sp = (short*)cp;
	*sp = sV1;
	cp += 2;

	sp = (short*)cp;
	*sp = sV2;
	cp += 2;

	sp = (short*)cp;
	*sp = sV3;
	cp += 2;

	dwp = (UINT32*)cp;
	*dwp = sV4;
	cp += 4;

	dwTime = timeGetTime();


	bFlag = true;
	iShortCutIndex = 0;
	while (bFlag == true) {
		i = m_iClientShortCut[iShortCutIndex];
		iShortCutIndex++;
		if (i == 0) bFlag = false;

		if ((bFlag == true) && (m_pClientList[i] != 0)) {
			if ((m_pClientList[i]->m_cMapIndex == cMapIndex) &&
				(m_pClientList[i]->m_sX >= sX - 12) &&
				(m_pClientList[i]->m_sX <= sX + 12) &&
				(m_pClientList[i]->m_sY >= sY - 10) &&
				(m_pClientList[i]->m_sY <= sY + 10)) {

				iRet = m_pClientList[i]->m_pXSock->iSendMsg(cData, 20, cKey);
			}
		}
	}
}

void CGame::SendEventToNearClient_TypeB(UINT32 dwMsgID, UINT16 wMsgType, char cMapIndex, short sX, short sY, short sV1, short sV2, short sV3, short sV4)
{
	int i, iRet, iShortCutIndex;
	char* cp, cData[100];
	UINT32* dwp, dwTime;
	UINT16* wp;
	short* sp;
	bool bFlag;
	char  cKey;

	cKey = (char)(rand() % 245) + 1; // v1.4

	ZeroMemory(cData, sizeof(cData));

	dwp = (UINT32*)(cData + DEF_INDEX4_MSGID);
	*dwp = dwMsgID;
	wp = (UINT16*)(cData + DEF_INDEX2_MSGTYPE);
	*wp = wMsgType;

	cp = (char*)(cData + DEF_INDEX2_MSGTYPE + 2);

	sp = (short*)cp;
	*sp = sX;
	cp += 2;

	sp = (short*)cp;
	*sp = sY;
	cp += 2;

	sp = (short*)cp;
	*sp = sV1;
	cp += 2;

	sp = (short*)cp;
	*sp = sV2;
	cp += 2;

	sp = (short*)cp;
	*sp = sV3;
	cp += 2;

	sp = (short*)cp;
	*sp = sV4;
	cp += 2;

	dwTime = timeGetTime();


	bFlag = true;
	iShortCutIndex = 0;
	while (bFlag == true) {
		i = m_iClientShortCut[iShortCutIndex];
		iShortCutIndex++;
		if (i == 0) bFlag = false;

		if ((bFlag == true) && (m_pClientList[i] != 0)) {
			if ((m_pClientList[i]->m_cMapIndex == cMapIndex) &&
				(m_pClientList[i]->m_sX >= sX - 12) &&
				(m_pClientList[i]->m_sX <= sX + 12) &&
				(m_pClientList[i]->m_sY >= sY - 10) &&
				(m_pClientList[i]->m_sY <= sY + 10) &&
				!(dwMsgID == MSGID_DYNAMICOBJECT && sV1 == DEF_DYNAMICOBJECT_MAGICTRAP && m_pDynamicObjectList[sV2] != 0 && m_pClientList[m_pDynamicObjectList[sV2]->m_sOwner]->m_cSide != m_pClientList[i]->m_cSide)) {

				iRet = m_pClientList[i]->m_pXSock->iSendMsg(cData, 18, cKey);
			}
		}
	}
}

bool CGame::_bRegisterMap(char* pName)
{
	int i;
	char cTmpName[11], cTxt[120];

	ZeroMemory(cTmpName, sizeof(cTmpName));
	strcpy(cTmpName, pName);
	for (i = 0; i < DEF_MAXMAPS; i++) {
		if ((m_pMapList[i] != 0) && (memcmp(m_pMapList[i]->m_cName, cTmpName, 10) == 0)) {
			wsprintf(cTxt, "(!!!) CRITICAL ERROR! Map (%s) is already installed! cannot add.", cTmpName);
			PutLogList(cTxt);
			return false;
		}
	}
	for (i = 0; i < DEF_MAXMAPS; i++) {
		if (m_pMapList[i] == 0) {
			m_pMapList[i] = (class CMap*)new class CMap(this);
			wsprintf(cTxt, "(*) Add map (%s) - Loading map info files...", pName);
			PutLogList(cTxt);
			if (m_pMapList[i]->bInit(pName) == false) {
				PutLogList("(!!!) Data file loading fail!");
				return false;
			}
			PutLogList("(*) Data file loading success.");
			if ((m_iMiddlelandMapIndex == -1) && (strcmp("middleland", pName) == 0)) m_iMiddlelandMapIndex = i;
			if ((m_iAresdenMapIndex == -1) && (strcmp("aresden", pName) == 0)) m_iAresdenMapIndex = i;
			if ((m_iElvineMapIndex == -1) && (strcmp("elvine", pName) == 0)) m_iElvineMapIndex = i;
			if ((m_iBTFieldMapIndex == -1) && (strcmp("BtField", pName) == 0)) m_iBTFieldMapIndex = i; // new
			if ((m_iGodHMapIndex == -1) && (strcmp("GodH", pName) == 0)) m_iGodHMapIndex = i; // new
			if ((m_iRampartMapIndex == -1) && (strcmp("HRampart", pName) == 0)) m_iRampartMapIndex = i;
			
			m_iTotalMaps++;
			return true;
		}
	}
	wsprintf(cTxt, "(!!!) CRITICAL ERROR! Map (%s) cannot be added - no more map space.", pName);
	PutLogList(cTxt);
	return false;
}

void CGame::GetMapInitialPoint(int iMapIndex, short* pX, short* pY, char* pPlayerLocation)
{
	int i, iTotalPoint;
	POINT  pList[DEF_MAXINITIALPOINT];

	if (m_pMapList[iMapIndex] == 0) return;

	// ¸®½ºÆ®¸¦ ÀÛ¼ºÇÑ´Ù.
	iTotalPoint = 0;
	for (i = 0; i < DEF_MAXINITIALPOINT; i++)
		if (m_pMapList[iMapIndex]->m_pInitialPoint[i].x != -1) {
			pList[iTotalPoint].x = m_pMapList[iMapIndex]->m_pInitialPoint[i].x;
			pList[iTotalPoint].y = m_pMapList[iMapIndex]->m_pInitialPoint[i].y;
			iTotalPoint++;
		}

	if (iTotalPoint == 0) return;

	// v1.42 ¼Ò¼ÓÀÌ Áß¸³ÀÌ¸é ¹«Á¶°Ç Initial PointÀÇ Ã³À½À¸·Î °£´Ù.
	if ((pPlayerLocation != 0) && (memcmp(pPlayerLocation, "NONE", 4) == 0))
		i = 0;
	else i = iDice(1, iTotalPoint) - 1;

	*pX = (short)pList[i].x;
	*pY = (short)pList[i].y;
}

int CGame::iGetMapLocationSide(char* pMapName)
{

	if (strcmp(pMapName, "cityhall_1") == 0) return 1;
	if (strcmp(pMapName, "cityhall_2") == 0) return 2;

	return 0;
}

void CGame::UpdateMapSectorInfo()
{
	int i, ix, iy;
	int iMaxNeutralActivity, iMaxAresdenActivity, iMaxElvineActivity, iMaxMonsterActivity, iMaxPlayerActivity;

	for (i = 0; i < DEF_MAXMAPS; i++)
		if (m_pMapList[i] != 0) {

			iMaxNeutralActivity = iMaxAresdenActivity = iMaxElvineActivity = iMaxMonsterActivity = iMaxPlayerActivity = 0;
			m_pMapList[i]->m_iMaxNx = m_pMapList[i]->m_iMaxNy = m_pMapList[i]->m_iMaxAx = m_pMapList[i]->m_iMaxAy = 0;
			m_pMapList[i]->m_iMaxEx = m_pMapList[i]->m_iMaxEy = m_pMapList[i]->m_iMaxMx = m_pMapList[i]->m_iMaxMy = 0;
			m_pMapList[i]->m_iMaxPx = m_pMapList[i]->m_iMaxPy = 0;

			// ±×µ¿¾È ÀúÀåÇß´ø TempSectorInfo¿¡¼­ Á¤º¸¸¦ ¾ò¾î SectorInfo¿¡ ÀúÀåÇÑ ´ÙÀ½ TempSectorInfo´Â Áö¿î´Ù.
			for (ix = 0; ix < DEF_MAXSECTORS; ix++)
				for (iy = 0; iy < DEF_MAXSECTORS; iy++) {
					if (m_pMapList[i]->m_stTempSectorInfo[ix][iy].iNeutralActivity > iMaxNeutralActivity) {
						iMaxNeutralActivity = m_pMapList[i]->m_stTempSectorInfo[ix][iy].iNeutralActivity;
						m_pMapList[i]->m_iMaxNx = ix;
						m_pMapList[i]->m_iMaxNy = iy;
					}

					if (m_pMapList[i]->m_stTempSectorInfo[ix][iy].iAresdenActivity > iMaxAresdenActivity) {
						iMaxAresdenActivity = m_pMapList[i]->m_stTempSectorInfo[ix][iy].iAresdenActivity;
						m_pMapList[i]->m_iMaxAx = ix;
						m_pMapList[i]->m_iMaxAy = iy;
					}

					if (m_pMapList[i]->m_stTempSectorInfo[ix][iy].iElvineActivity > iMaxElvineActivity) {
						iMaxElvineActivity = m_pMapList[i]->m_stTempSectorInfo[ix][iy].iElvineActivity;
						m_pMapList[i]->m_iMaxEx = ix;
						m_pMapList[i]->m_iMaxEy = iy;
					}

					if (m_pMapList[i]->m_stTempSectorInfo[ix][iy].iMonsterActivity > iMaxMonsterActivity) {
						iMaxMonsterActivity = m_pMapList[i]->m_stTempSectorInfo[ix][iy].iMonsterActivity;
						m_pMapList[i]->m_iMaxMx = ix;
						m_pMapList[i]->m_iMaxMy = iy;
					}

					if (m_pMapList[i]->m_stTempSectorInfo[ix][iy].iPlayerActivity > iMaxPlayerActivity) {
						iMaxPlayerActivity = m_pMapList[i]->m_stTempSectorInfo[ix][iy].iPlayerActivity;
						m_pMapList[i]->m_iMaxPx = ix;
						m_pMapList[i]->m_iMaxPy = iy;
					}
				}

			// TempSectorInfo ³¯¸°´Ù.
			m_pMapList[i]->ClearTempSectorInfo();

			// Sector Info¿¡ ÀúÀå
			if (m_pMapList[i]->m_iMaxNx > 0) m_pMapList[i]->m_stSectorInfo[m_pMapList[i]->m_iMaxNx][m_pMapList[i]->m_iMaxNy].iNeutralActivity++;
			if (m_pMapList[i]->m_iMaxAx > 0) m_pMapList[i]->m_stSectorInfo[m_pMapList[i]->m_iMaxAx][m_pMapList[i]->m_iMaxAy].iAresdenActivity++;
			if (m_pMapList[i]->m_iMaxEx > 0) m_pMapList[i]->m_stSectorInfo[m_pMapList[i]->m_iMaxEx][m_pMapList[i]->m_iMaxEy].iElvineActivity++;
			if (m_pMapList[i]->m_iMaxMx > 0) m_pMapList[i]->m_stSectorInfo[m_pMapList[i]->m_iMaxMx][m_pMapList[i]->m_iMaxMy].iMonsterActivity++;
			if (m_pMapList[i]->m_iMaxPx > 0) m_pMapList[i]->m_stSectorInfo[m_pMapList[i]->m_iMaxPx][m_pMapList[i]->m_iMaxPy].iPlayerActivity++;
		}
}


void CGame::AgingMapSectorInfo()
{
	int i, ix, iy;

	for (i = 0; i < DEF_MAXMAPS; i++)
		if (m_pMapList[i] != 0) {
			for (ix = 0; ix < DEF_MAXSECTORS; ix++)
				for (iy = 0; iy < DEF_MAXSECTORS; iy++) {
					m_pMapList[i]->m_stSectorInfo[ix][iy].iNeutralActivity--;
					m_pMapList[i]->m_stSectorInfo[ix][iy].iAresdenActivity--;
					m_pMapList[i]->m_stSectorInfo[ix][iy].iElvineActivity--;
					m_pMapList[i]->m_stSectorInfo[ix][iy].iMonsterActivity--;
					m_pMapList[i]->m_stSectorInfo[ix][iy].iPlayerActivity--;

					if (m_pMapList[i]->m_stSectorInfo[ix][iy].iNeutralActivity < 0) m_pMapList[i]->m_stSectorInfo[ix][iy].iNeutralActivity = 0;
					if (m_pMapList[i]->m_stSectorInfo[ix][iy].iAresdenActivity < 0) m_pMapList[i]->m_stSectorInfo[ix][iy].iAresdenActivity = 0;
					if (m_pMapList[i]->m_stSectorInfo[ix][iy].iElvineActivity < 0) m_pMapList[i]->m_stSectorInfo[ix][iy].iElvineActivity = 0;
					if (m_pMapList[i]->m_stSectorInfo[ix][iy].iMonsterActivity < 0) m_pMapList[i]->m_stSectorInfo[ix][iy].iMonsterActivity = 0;
					if (m_pMapList[i]->m_stSectorInfo[ix][iy].iPlayerActivity < 0) m_pMapList[i]->m_stSectorInfo[ix][iy].iPlayerActivity = 0;
				}
		}
}

void CMap::ClearSectorInfo()
{
	int ix, iy;

	for (ix = 0; ix < DEF_MAXSECTORS; ix++)
		for (iy = 0; iy < DEF_MAXSECTORS; iy++) {
			m_stSectorInfo[ix][iy].iNeutralActivity = 0;
			m_stSectorInfo[ix][iy].iAresdenActivity = 0;
			m_stSectorInfo[ix][iy].iElvineActivity = 0;
			m_stSectorInfo[ix][iy].iMonsterActivity = 0;
			m_stSectorInfo[ix][iy].iPlayerActivity = 0;
		}
}

void CMap::ClearTempSectorInfo()
{
	int ix, iy;

	for (ix = 0; ix < DEF_MAXSECTORS; ix++)
		for (iy = 0; iy < DEF_MAXSECTORS; iy++) {
			m_stTempSectorInfo[ix][iy].iNeutralActivity = 0;
			m_stTempSectorInfo[ix][iy].iAresdenActivity = 0;
			m_stTempSectorInfo[ix][iy].iElvineActivity = 0;
			m_stTempSectorInfo[ix][iy].iMonsterActivity = 0;
			m_stTempSectorInfo[ix][iy].iPlayerActivity = 0;
		}
}