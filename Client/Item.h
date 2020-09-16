// Item.h: interface for the CItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITEM_H__211A1360_91B9_11D2_B143_00001C7030A6__INCLUDED_)
#define AFX_ITEM_H__211A1360_91B9_11D2_B143_00001C7030A6__INCLUDED_

#include <windows.h>

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define DEF_MAXITEMEQUIPPOS		15
#define DEF_EQUIPPOS_NONE		0
#define DEF_EQUIPPOS_HEAD		1
#define DEF_EQUIPPOS_BODY		2
#define DEF_EQUIPPOS_ARMS		3
#define DEF_EQUIPPOS_PANTS		4
#define DEF_EQUIPPOS_BOOTS		5
#define DEF_EQUIPPOS_NECK		6
#define DEF_EQUIPPOS_LHAND		7
#define DEF_EQUIPPOS_RHAND		8
#define DEF_EQUIPPOS_TWOHAND	9
#define DEF_EQUIPPOS_RFINGER	10
#define DEF_EQUIPPOS_LFINGER	11
#define DEF_EQUIPPOS_BACK		12
#define DEF_EQUIPPOS_FULLBODY	13

#define DEF_ITEMTYPE_NONE			0
#define DEF_ITEMTYPE_EQUIP			1
#define DEF_ITEMTYPE_APPLY			2
#define DEF_ITEMTYPE_USE_DEPLETE	3
#define DEF_ITEMTYPE_INSTALL		4
#define DEF_ITEMTYPE_CONSUME		5
#define DEF_ITEMTYPE_ARROW			6
#define DEF_ITEMTYPE_EAT			7
#define DEF_ITEMTYPE_USE_SKILL		8
#define DEF_ITEMTYPE_USE_PERM		9
#define DEF_ITEMTYPE_USE_SKILL_ENABLEDIALOGBOX	10
#define DEF_ITEMTYPE_USE_DEPLETE_DEST			11
#define DEF_ITEMTYPE_MATERIAL					12

//Magn0S:: Add same effect type of HG to update item description
#define DEF_ITEMEFFECTTYPE_NONE				0
#define DEF_ITEMEFFECTTYPE_ATTACK			1
#define DEF_ITEMEFFECTTYPE_DEFENSE			2
#define DEF_ITEMEFFECTTYPE_ATTACK_ARROW		3
#define DEF_ITEMEFFECTTYPE_HP   		    4
#define DEF_ITEMEFFECTTYPE_MP   		    5
#define DEF_ITEMEFFECTTYPE_SP   		    6
#define DEF_ITEMEFFECTTYPE_HPSTOCK 		    7
#define DEF_ITEMEFFECTTYPE_GET			    8
#define DEF_ITEMEFFECTTYPE_STUDYSKILL		9
#define DEF_ITEMEFFECTTYPE_SHOWLOCATION		10
#define DEF_ITEMEFFECTTYPE_MAGIC			11
#define DEF_ITEMEFFECTTYPE_CHANGEATTR		12
#define DEF_ITEMEFFECTTYPE_ATTACK_MANASAVE	13
#define DEF_ITEMEFFECTTYPE_ADDEFFECT	    14
#define DEF_ITEMEFFECTTYPE_MAGICDAMAGESAVE	15
#define DEF_ITEMEFFECTTYPE_OCCUPYFLAG		16
#define DEF_ITEMEFFECTTYPE_DYE				17
#define DEF_ITEMEFFECTTYPE_STUDYMAGIC		18
#define DEF_ITEMEFFECTTYPE_ATTACK_MAXHPDOWN	19
#define DEF_ITEMEFFECTTYPE_ATTACK_DEFENSE	20
#define DEF_ITEMEFFECTTYPE_MATERIAL_ATTR	21
#define DEF_ITEMEFFECTTYPE_FIRMSTAMINAR		22
#define DEF_ITEMEFFECTTYPE_LOTTERY			23
#define DEF_ITEMEFFECTTYPE_ATTACK_SPECABLTY		24
#define DEF_ITEMEFFECTTYPE_DEFENSE_SPECABLTY	25
#define DEF_ITEMEFFECTTYPE_ALTERITEMDROP		26
#define DEF_ITEMEFFECTTYPE_CONSTRUCTIONKIT		27
#define DEF_ITEMEFFECTTYPE_WARM				28	// Unfreeze pot ?
#define DEF_ITEMEFFECTTYPE_FARMING			30
#define DEF_ITEMEFFECTTYPE_SLATES			31
#define DEF_ITEMEFFECTTYPE_ARMORDYE			32

#define DEF_ITEMEFFECTTYPE_PANDORA			34
#define DEF_ITEMEFFECTTYPE_WEAPONDYE		41 // WeaponDye

//Magn0S:: Added---------------------------------------------------------------
#define DEF_ITEMEFFECTTYPE_REP				35
#define DEF_ITEMEFFECTTYPE_COINS			36
#define DEF_ITEMEFFECTTYPE_REPAIR			37

class CItem  
{
public:
	CItem();
	virtual ~CItem();
	char  m_cName[21];
	char  m_cItemType;
	char  m_cEquipPos;
	char  m_cItemColor;
	char  m_cSpeed;
	char  m_cGenderLimit;
	short m_sItemEffectType;
	short m_sLevelLimit;
	short m_sSprite;
	short m_sSpriteFrame;
	short m_sX, m_sY;
	short m_sItemSpecEffectValue1, m_sItemSpecEffectValue2, m_sItemSpecEffectValue3;
	short m_sItemEffectValue1, m_sItemEffectValue2, m_sItemEffectValue3, m_sItemEffectValue4, m_sItemEffectValue5, m_sItemEffectValue6; 
	WORD  m_wCurLifeSpan;
	WORD  m_wMaxLifeSpan;
	WORD  m_wPrice, m_wWeight;
	DWORD m_dwCount;
	DWORD m_dwAttribute;

	//Magn0S:: New Items Extra Atributes.
	short m_sNewEffect1, m_sNewEffect2, m_sNewEffect3, m_sNewEffect4;

	int m_iClass;
};

#endif // !defined(AFX_ITEM_H__211A1360_91B9_11D2_B143_00001C7030A6__INCLUDED_)
