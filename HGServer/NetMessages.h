﻿#include "Version.h"

#ifndef DEF_VERSIONPLUSMODIFIER
#define DEF_VERSIONPLUSMODIFIER 0 // If the antihack was noy defined don't change any msgid
#endif

#define MINIMAPORANGE_CLEAR 0x1F43
#define MINIMAPORANGE_UPDATE 0x1F44

#define DEF_NOTIFY_SUMMONGUILD 0x1F45
#define MSGID_REQUEST_SUMMONGUILD 0x1F46

#define DEF_NOTIFY_FRAGILE 0x1F47

#define DEF_NOTIFY_CITYHP 0x1F33

#define DEF_NOTIFY_HEROBONUS 0x1F32

#define DEF_UPDATE_GUILDRANK 0x1F40

#define DEF_NOTIFY_GOLD 0x1F48

#define MINIMAPBLUE_CLEAR									0x1F34
#define MINIMAPBLUE_UPDATE									0x1F35
#define MINIMAPRED_CLEAR									0x1F36
#define MINIMAPRED_UPDATE									0x1F37

#define MINIMAPGREEN_CLEAR									0x1F38
#define MINIMAPGREEN_UPDATE									0x1F39

#define DEF_NOTIFY_ARENASTATUS			        0x0D27
#define DEF_COMMONTYPE_REQARENASTATUS	        0x0A70

#define DEF_NOTIFY_REQGUILDRANKANSWER 0x1F41
#define DEF_COMMONTYPE_REQGUILDRANK 0x1F42

//Vamp Arena
#define MSGID_REQUEST_LEAVEARENA				0x0EA03211

#define DEF_REQUEST_CHANGE_CLASS 0x0F28

// SQL
#define MSGID_UPDATECONFIGFILES					0x0FA40005
#define MSGID_ADMINSETTINGSCONFIGURATIONCONTENTS	0x0FA314D3
#define MSGID_SETTINGSCONFIGURATIONCONTENTS			0x0FA314D4
#define DEF_REJECTTYPE_GAMESERVERNOTONLINE             0x03

#define DEF_NOTIFY_EVENT				0x0BFC
#define DEF_NOTIFY_EVENT_INFO			0x0BFD

#define DEF_NOTIFY_CURLIFESPAN 0x0BF3

#define DEF_NOTIFY_ANGEL_FAILED					0x0BF4 // Failed receiving an Angel pandent
#define DEF_NOTIFY_ANGEL_RECEIVED				0x0BF5 // "You have received the Tutelary Angel"

#define MSGID_REQUEST_TOPEK 0x3B1890EC

#define MSGID_REQUEST_SETTRAP		0x0BFB

#define NOTIFYMSG										    0x0C56

#define MSGID_EVENTS			0x002071

#define DEF_NOTIFY_TEAMARENA			0x0BEE

//News Addons - ZeroEoyPnk
#define DEF_SEND_PARTYCOORDS				0x1F07
#define DEF_SEND_PARTYHP					0x1F08
#define DEF_RESQUEST_PARTYHP				0x1F09

// VAMP - online users list
#define MSGID_REQUEST_ONLINE						0x3B1890EB
#define DEF_NOTIFY_USERJOIN							0x0C04

//50Cent - Repair All
#define DEF_COMMONTYPE_REQ_REPAIRALL            0x0F10
#define DEF_NOTIFY_REPAIRALLPRICES                0x0F11
#define DEF_COMMONTYPE_REQ_REPAIRALLDELETE        0x0F12
#define DEF_COMMONTYPE_REQ_REPAIRALLCONFIRM        0x0F13

#define DEF_NOTIFY_RESUR_ON					0x0C49
#define DEF_NOTIFY_RESUR_OFF				        0x0C50

// Reversed by Snoopy from real 3.72 client
#define DEF_REQUEST_ANGEL    0x0FC9421E

#define DEF_NOTIFY_MORLEARPJ						0x0F24


//50Cent - HP Bar
#define DEF_COMMONTYPE_REQ_GETNPCHP					0x1F12
#define DEF_SEND_NPCHP								0x1F13

#define MSGID_REQUEST_PING                    0x0FFAACCA
#define MSGID_RESPONSE_PING                    0x0FFAACCB

#define DEF_NOTIFY_CRAFTING_SUCCESS				0x0BF0 // Crafting ok
#define DEF_NOTIFY_CRAFTING_FAIL					0x0BF1 // Crafting failed

#define DEF_COMMONTYPE_CRAFTITEM				0x0A28 // Snoopy: Crafting item

#define DEF_NOTIFY_HELDENIANVICTORY				0x0BEB // HELDENIAN FINISHED FOR Elvine/aresden

// Snoopy: Heldenian Teleports
#define MSGID_REQUEST_HELDENIAN_TP_LIST			0x0EA03206 // Teleport client <-> gserver
#define MSGID_RESPONSE_HELDENIAN_TP_LIST		0x0EA03207 // Teleport client <-> gserver
#define MSGID_REQUEST_HELDENIAN_TP				0x0EA03208 // Teleport client <-> gserver

#define DEF_NOTIFY_SPAWNEVENT				0x0BAA
#define DEF_NOTIFY_QUESTCOUNTER				0x0BE2	
#define DEF_NOTIFY_HELDENIANSTART			0x0BEA
#define	DEF_NOTIFY_HELDENIANCOUNT			0x0BEC
#define DEF_NOTIFY_HELDENIANEND				0x0BE7

#define NOTIFY_EQUIPITEM2								0x0B99D 

// NetMessages.h

#define DEF_MSGTYPE_CONFIRM					0x0F14
#define DEF_MSGTYPE_REJECT					0x0F15

//#define MSGID_REQUEST_INITPLAYER				0x05040205 // client -> gserver 
#define MSGID_REQUEST_INITPLAYER				0x05040205 - DEF_VERSIONPLUSMODIFIER  // client -> gserver 
#define MSGID_RESPONSE_INITPLAYER				0x05040206 // reponse

//#define MSGID_REQUEST_INITDATA					0x05080404 // client -> gserver 
#define MSGID_REQUEST_INITDATA					0x05080404 // client -> gserver 
#define MSGID_RESPONSE_INITDATA					0x05080405 - DEF_VERSIONPLUSMODIFIER   // response

#define MSGID_COMMAND_MOTION				0x0FA314D5
#define MSGID_RESPONSE_MOTION				0x0FA314D6
#define MSGID_EVENT_MOTION					0x0FA314D7
#define MSGID_EVENT_LOG						0x0FA314D8
#define MSGID_EVENT_COMMON					0x0FA314DB
#define MSGID_COMMAND_COMMON				0x0FA314DC

#define DEF_COMMONTYPE_ITEMDROP				0x0A01
#define DEF_COMMONTYPE_EQUIPITEM				0x0A02
#define DEF_COMMONTYPE_REQ_LISTCONTENTS		0x0A03
#define DEF_COMMONTYPE_REQ_PURCHASEITEM		0x0A04
#define DEF_COMMONTYPE_GIVEITEMTOCHAR			0x0A05
#define DEF_COMMONTYPE_JOINGUILDAPPROVE		0x0A06
#define DEF_COMMONTYPE_JOINGUILDREJECT			0x0A07
#define DEF_COMMONTYPE_DISMISSGUILDAPPROVE		0x0A08
#define DEF_COMMONTYPE_DISMISSGUILDREJECT		0x0A09
#define DEF_COMMONTYPE_RELEASEITEM				0x0A0A
#define DEF_COMMONTYPE_TOGGLECOMBATMODE		0x0A0B
#define DEF_COMMONTYPE_SETITEM					0x0A0C
#define DEF_COMMONTYPE_MAGIC					0x0A0D
#define DEF_COMMONTYPE_REQ_STUDYMAGIC			0x0A0E
#define DEF_COMMONTYPE_REQ_TRAINSKILL			0x0A0F 
#define DEF_COMMONTYPE_REQ_GETREWARDMONEY		0x0A10
#define DEF_COMMONTYPE_REQ_USEITEM				0x0A11
#define DEF_COMMONTYPE_REQ_USESKILL			0x0A12
#define DEF_COMMONTYPE_REQ_SELLITEM			0x0A13
#define DEF_COMMONTYPE_REQ_REPAIRITEM			0x0A14
#define DEF_COMMONTYPE_REQ_SELLITEMCONFIRM		0x0A15
#define DEF_COMMONTYPE_REQ_REPAIRITEMCONFIRM	0x0A16
#define DEF_COMMONTYPE_REQ_GETFISHTHISTIME		0x0A17
#define DEF_COMMONTYPE_TOGGLESAFEATTACKMODE    0x0A18
#define DEF_COMMONTYPE_REQ_CREATEPOTION		0x0A19
#define DEF_COMMONTYPE_TALKTONPC				0x0A1A
#define DEF_COMMONTYPE_REQ_SETDOWNSKILLINDEX	0x0A1B
#define DEF_COMMONTYPE_REQ_GETOCCUPYFLAG       0x0A1C
#define DEF_COMMONTYPE_REQ_GETHEROMANTLE		0x0A1D
#define DEF_COMMONTYPE_EXCHANGEITEMTOCHAR		0x0A1E
#define DEF_COMMONTYPE_SETEXCHANGEITEM			0x0A1F
#define DEF_COMMONTYPE_CONFIRMEXCHANGEITEM		0x0A20
#define DEF_COMMONTYPE_CANCELEXCHANGEITEM		0x0A21
#define DEF_COMMONTYPE_QUESTACCEPTED			0x0A22
#define DEF_COMMONTYPE_BUILDITEM				0x0A23
#define DEF_COMMONTYPE_GETMAGICABILITY			0x0A24

// v1.4311-3 추가 메세지 선언 입장권 받기위한 메세지  길드에서 강퇴됨  DEF_COMMONTYPE_REQ_GETOCCUPYFIGHTZONETICKET DEF_COMMONTYPE_BANGUILD
#define DEF_COMMONTYPE_REQ_GETOCCUPYFIGHTZONETICKET	0x0A25
#define DEF_COMMONTYPE_BANGUILD						0x0A26

#define DEF_COMMONTYPE_REQUEST_ACCEPTJOINPARTY			0x0A30 // Original DEF_COMMONTYPE_REQUEST_CREATENEWPARTY
#define DEF_COMMONTYPE_REQUEST_JOINPARTY				0x0A31
#define DEF_COMMONTYPE_RESPONSE_JOINPARTY				0x0A32
#define DEF_COMMONTYPE_REQUEST_ACTIVATESPECABLTY		0x0A40
#define DEF_COMMONTYPE_REQUEST_CANCELQUEST				0x0A50
#define DEF_COMMONTYPE_REQUEST_SELECTCRUSADEDUTY		0x0A51
#define DEF_COMMONTYPE_REQUEST_MAPSTATUS				0x0A52
#define DEF_COMMONTYPE_REQUEST_HELP					0x0A53

#define DEF_COMMONTYPE_SETGUILDTELEPORTLOC				0x0A54
#define DEF_COMMONTYPE_GUILDTELEPORT					0x0A55
#define DEF_COMMONTYPE_SUMMONWARUNIT					0x0A56
#define DEF_COMMONTYPE_SETGUILDCONSTRUCTLOC			0x0A57

#define MSGID_NOTIFY						0x0FA314D0

#define DEF_NOTIFY_TOP15HB  0x0F26

#define DEF_NOTIFY_ITEMOBTAINED					0x0B01
#define DEF_NOTIFY_QUERY_JOINGUILDREQPERMISSION	0x0B02
#define DEF_NOTIFY_QUERY_DISMISSGUILDREQPERMISSION	0x0B03 
#define DEF_NOTIFY_WAITFORGUILDOPERATION			0x0B04
#define DEF_NOTIFY_CANNOTCARRYMOREITEM				0x0B05
#define DEF_NOTIFY_ITEMPURCHASED					0x0B06
#define DEF_NOTIFY_HP								0x0B07
#define DEF_NOTIFY_NOTENOUGHGOLD					0x0B08
#define DEF_NOTIFY_KILLED							0x0B09
#define DEF_NOTIFY_EXP								0x0B0A
#define DEF_NOTIFY_GUILDDISBANDED					0x0B0B
#define DEF_NOTIFY_EVENTMSGSTRING					0x0B0C
#define DEF_NOTIFY_CANNOTJOINMOREGUILDSMAN			0x0B0D
#define DEF_NOTIFY_NEWGUILDSMAN					0x0B0E
#define DEF_NOTIFY_DISMISSGUILDSMAN				0x0B0F
#define DEF_NOTIFY_MAGICSTUDYSUCCESS				0x0B10
#define DEF_NOTIFY_MAGICSTUDYFAIL					0x0B11
#define DEF_NOTIFY_SKILLTRAINSUCCESS				0x0B12
#define DEF_NOTIFY_SKILLTRAINFAIL					0x0B13
#define DEF_NOTIFY_MP								0x0B14
#define DEF_NOTIFY_SP								0x0B15
#define DEF_NOTIFY_LEVELUP							0x0B16
#define DEF_NOTIFY_ITEMLIFESPANEND					0x0B17
#define DEF_NOTIFY_LIMITEDLEVEL					0x0B18
#define DEF_NOTIFY_ITEMTOBANK						0x0B19
#define DEF_NOTIFY_PKPENALTY						0x0B1A
#define DEF_NOTIFY_PKCAPTURED						0x0B1B
#define DEF_NOTIFY_ENEMYKILLREWARD					0x0B1C
#define DEF_NOTIFY_GIVEITEMFIN_ERASEITEM           0x0B1D
#define DEF_NOTIFY_DROPITEMFIN_ERASEITEM			0x0B1F
#define DEF_NOTIFY_ITEMDEPLETED_ERASEITEM			0x0B20
#define DEF_NOTIFY_NEWDYNAMICOBJECT				0x0B21
#define DEF_NOTIFY_DELDYNAMICOBJECT				0x0B22
#define DEF_NOTIFY_SKILL							0x0B23
#define DEF_NOTIFY_SERVERCHANGE					0x0B24
#define DEF_NOTIFY_SETITEMCOUNT					0x0B25
#define DEF_NOTIFY_CANNOTITEMTOBANK				0x0B26
#define DEF_NOTIFY_MAGICEFFECTON					0x0B27
#define DEF_NOTIFY_MAGICEFFECTOFF					0x0B28
#define DEF_NOTIFY_TOTALUSERS						0x0B29
#define DEF_NOTIFY_SKILLUSINGEND					0x0B2A
#define DEF_NOTIFY_SHOWMAP							0x0B2B
#define DEF_NOTIFY_CANNOTSELLITEM					0x0B2C
#define DEF_NOTIFY_SELLITEMPRICE					0x0B2D
#define DEF_NOTIFY_CANNOTREPAIRITEM				0x0B2E
#define DEF_NOTIFY_REPAIRITEMPRICE					0x0B2F
#define DEF_NOTIFY_ITEMREPAIRED					0x0B30
#define DEF_NOTIFY_ITEMSOLD						0x0B31

#define DEF_MAX_STATS								0x0B32 // centu

#define DEF_NOTIFY_PLAYERONGAME					0x0B33
#define DEF_NOTIFY_PLAYERNOTONGAME					0x0B34
#define DEF_NOTIFY_WHISPERMODEON					0x0B35
#define DEF_NOTIFY_WHISPERMODEOFF					0x0B36
#define DEF_NOTIFY_PLAYERPROFILE					0x0B37
#define DEF_NOTIFY_TRAVELERLIMITEDLEVEL			0x0B38
#define DEF_NOTIFY_HUNGER							0x0B39

#define DEF_NOTIFY_TOBERECALLED					0x0B40
#define DEF_NOTIFY_TIMECHANGE						0x0B41
#define DEF_NOTIFY_PLAYERSHUTUP					0x0B42
#define DEF_NOTIFY_ADMINUSERLEVELLOW				0x0B43
#define DEF_NOTIFY_CANNOTRATING					0x0B44
#define DEF_NOTIFY_RATINGPLAYER					0x0B45
#define DEF_NOTIFY_NOTICEMSG						0x0B46
#define DEF_NOTIFY_EVENTFISHMODE					0x0B47
#define DEF_NOTIFY_FISHCHANCE						0x0B48
#define DEF_NOTIFY_DEBUGMSG						0x0B49
#define DEF_NOTIFY_FISHSUCCESS						0x0B4A
#define DEF_NOTIFY_FISHFAIL						0x0B4B
#define DEF_NOTIFY_FISHCANCELED					0x0B4C
#define DEF_NOTIFY_WHETHERCHANGE					0x0B4D
#define DEF_NOTIFY_SERVERSHUTDOWN					0x0B4E
#define DEF_NOTIFY_REWARDGOLD						0x0B4F
#define DEF_NOTIFY_IPACCOUNTINFO					0x0B50
#define DEF_NOTIFY_SAFEATTACKMODE					0x0B51
#define DEF_NOTIFY_SUPERATTACKLEFT					0x0B52
#define DEF_NOTIFY_NOMATCHINGPOTION				0x0B53
#define DEF_NOTIFY_LOWPOTIONSKILL					0x0B54
#define DEF_NOTIFY_POTIONFAIL						0x0B55
#define DEF_NOTIFY_POTIONSUCCESS					0x0B56
#define DEF_NOTIFY_NPCTALK							0x0B57
#define DEF_NOTIFY_ADMINIFO						0x0B58
#define DEF_NOTIFY_DOWNSKILLINDEXSET				0x0B59
#define DEF_NOTIFY_ENEMYKILLS					    0x0B5A
#define DEF_NOTIFY_ITEMPOSLIST						0x0B5B
#define DEF_NOTIFY_ITEMRELEASED					0x0B5C
#define DEF_NOTIFY_NOTFLAGSPOT						0x0B5D
#define DEF_NOTIFY_OPENEXCHANGEWINDOW				0x0B5E
#define DEF_NOTIFY_SETEXCHANGEITEM					0x0B5F
#define DEF_NOTIFY_CANCELEXCHANGEITEM				0x0B60
#define DEF_NOTIFY_EXCHANGEITEMCOMPLETE			0x0B61
#define DEF_NOTIFY_CANNOTGIVEITEM					0x0B62
#define DEF_NOTIFY_GIVEITEMFIN_COUNTCHANGED        0x0B63
#define DEF_NOTIFY_DROPITEMFIN_COUNTCHANGED		0x0B64
#define DEF_NOTIFY_ITEMCOLORCHANGE					0x0B65
#define DEF_NOTIFY_QUESTCONTENTS					0x0B66
#define DEF_NOTIFY_QUESTABORTED					0x0B67
#define DEF_NOTIFY_QUESTCOMPLETED					0x0B68
#define DEF_NOTIFY_QUESTREWARD						0x0B69

#define DEF_NOTIFY_BUILDITEMSUCCESS				0x0B70
#define DEF_NOTIFY_BUILDITEMFAIL					0x0B71
#define DEF_NOTIFY_OBSERVERMODE					0x0B72
#define DEF_NOTIFY_GLOBALATTACKMODE				0x0B73
#define DEF_NOTIFY_DAMAGEMOVE						0x0B74
#define DEF_NOTIFY_FORCEDISCONN					0x0B75
#define DEF_NOTIFY_FIGHTZONERESERVE				0x0B76
#define DEF_NOTIFY_NOGUILDMASTERLEVEL				0x0B77
#define DEF_NOTIFY_SUCCESSBANGUILDMAN				0x0B78
#define DEF_NOTIFY_CANNOTBANGUILDMAN				0x0B79

#define DEF_NOTIFY_RESPONSE_CREATENEWPARTY			0x0B80
#define DEF_NOTIFY_QUERY_JOINPARTY					0x0B81

// v1.4311-3 메세지 선언 선언 사투장 예약이 취소 및 길드 마스터 명령 사용권한이 없음을 알림
// #define DEF_NOTIFY_SUCCESSBANGUILDMAN			0x0B82

#define DEF_NOTIFY_SKILLPOINT 0x0B82
#define MSGID_REQUEST_SKILLPOINT 0x002072

#define DEF_NOTIFY_ENERGYSPHERECREATED				0x0B90
#define DEF_NOTIFY_ENERGYSPHEREGOALIN				0x0B91
#define DEF_NOTIFY_SPECIALABILITYENABLED			0x0B92
#define DEF_NOTIFY_SPECIALABILITYSTATUS			0x0B93
#define DEF_NOTIFY_CRUSADE							0x0B94
#define DEF_NOTIFY_LOCKEDMAP						0x0B95
#define DEF_NOTIFY_DUTYSELECTED					0x0B96
#define DEF_NOTIFY_MAPSTATUSNEXT					0x0B97
#define DEF_NOTIFY_MAPSTATUSLAST					0x0B98
#define DEF_NOTIFY_HELP							0x0B99
#define DEF_NOTIFY_HELPFAILED						0x0B9A
#define DEF_NOTIFY_METEORSTRIKECOMING				0x0B9B
#define DEF_NOTIFY_METEORSTRIKEHIT					0x0B9C
#define DEF_NOTIFY_GRANDMAGICRESULT				0x0B9D
#define DEF_NOTIFY_NOMORECRUSADESTRUCTURE			0x0B9E
#define DEF_NOTIFY_CONSTRUCTIONPOINT				0x0B9F

#define DEF_NOTIFY_TCLOC							0x0BA0
#define DEF_NOTIFY_CANNOTCONSTRUCT					0x0BA1
#define DEF_NOTIFY_PARTY							0x0BA2
#define DEF_NOTIFY_ITEMATTRIBUTECHANGE				0x0BA3
#define DEF_NOTIFY_GIZONITEMUPGRADELEFT			0x0BA4
#define DEF_NOTIFY_GIZONITEMCANGE					0x0BA5
#define DEF_NOTIFY_REQGUILDNAMEANSWER				0x0BA6
#define DEF_NOTIFY_FORCERECALLTIME					0x0BA7
#define DEF_NOTIFY_ITEMUPGRADEFAIL					0x0BA8

#define DEF_NOTIFY_NOMOREAGRICULTURE				0x0BB0  //? 농작물 제한
#define DEF_NOTIFY_AGRICULTURESKILLLIMIT			0x0BB1  //? 스킬 제한
#define DEF_NOTIFY_AGRICULTURENOAREA				0x0BB2  //? 농작물을 지을수 있는 공간이 아니다.
#define DEF_NOTIFY_SETTING_SUCCESS					0x0BB3
#define DEF_NOTIFY_SETTING_FAILED					0x0BB4
#define DEF_NOTIFY_STATECHANGE_SUCCESS				0x0BB5
#define DEF_NOTIFY_STATECHANGE_FAILED				0x0BB6

#define DEF_NOTIFY_STATECHANGE_SUCCESS2				0x0BB7

#define DEF_NOTIFY_UPGRADEHEROCAPE					0x0BC0
#define DEF_NOTIFY_SLATE_CREATESUCCESS				0x0BC1
#define DEF_NOTIFY_SLATE_CREATEFAIL				0x0BC2

#define DEF_NOTIFY_NORECALL						0x0BD1 // "You cannot recall on this map"
#define DEF_NOTIFY_APOCGATESTARTMSG				0x0BD2 // "The portal to the appoclypse gate is open."
#define DEF_NOTIFY_APOCGATEENDMSG					0x0BD3
#define DEF_NOTIFY_APOCGATEOPEN					0x0BD4
#define DEF_NOTIFY_APOCGATECLOSE					0x0BD5
#define DEF_NOTIFY_ABADDONKILLED					0x0BD6 // "Abaddon destroyed by (playername)"
#define DEF_NOTIFY_APOCFORCERECALLPLAYERS			0x0BD7 // "You are forced to recall, because the apoclypse has started"
#define DEF_NOTIFY_SLATE_INVINCIBLE				0x0BD8
#define DEF_NOTIFY_SLATE_MANA						0x0BD9

#define DEF_REQUEST_RESET_STATS		0x0BB8

#define DEF_NOTIFY_SLATE_EXP 						0x0BE0
#define DEF_NOTIFY_SLATE_STATUS					0x0BE1
#define DEF_NOTIFY_MONSTERCOUNT					0x0BE3
#define DEF_NOTIFY_RESURRECTPLAYER					0x0BE9
#define DEF_NOTIFY_HELDENIANTELEPORT				0x0BE6
#define DEF_NOTIFY_0BE8							0x0BE8
#define DEF_NOTIFY_ABADDONTHUNDER					0x0BE5

#define MSGID_ITEMCONFIGURATIONCONTENTS			0x0FA314D9
#define MSGID_NPCCONFIGURATIONCONTENTS			0x0FA314DA
#define MSGID_MAGICCONFIGURATIONCONTENTS		0x0FA314DB
#define MSGID_SKILLCONFIGURATIONCONTENTS		0x0FA314DC
#define MSGID_PLAYERITEMLISTCONTENTS			0x0FA314DD
#define MSGID_POTIONCONFIGURATIONCONTENTS		0x0FA314DE
#define MSGID_PLAYERCHARACTERCONTENTS			0x0FA40000
#define MSGID_QUESTCONFIGURATIONCONTENTS		0x0FA40001
#define MSGID_BUILDITEMCONFIGURATIONCONTENTS	0x0FA40002
#define MSGID_DUPITEMIDFILECONTENTS				0x0FA40003
#define MSGID_NOTICEMENTFILECONTENTS			0x0FA40004
//#define WorldCfg								0x0FA40005
#define MSGID_NPCITEMCONFIGCONTENTS				0x0FA40006
#define MSGID_WLSERVERCONFIGURATIONCONTENTS		0x0FA40007
//#define Teleport								0x0FA40008
//#define ApocalypeSchedule						0x0FA40009
//#define HeldinianSchedule						0x0FA40010

#define DEF_NOTIFY_REQRANGO						0x0F02
#define DEF_COMMONTYPE_REQRANGO					0x0F03
#define DEF_NOTIFY_DGKILL							0x0F04
#define DEF_NOTIFY_DOUBLEKILL						0x0F05
#define DEF_NOTIFY_KILLSPRING						0x0F06
#define DEF_NOTIFY_HOLYSHIT						0x0F07
#define DEF_NOTIFY_MONSTERKILL						0x0F12
#define DEF_NOTIFY_REPDGDEATHS						0x0F16
#define DEF_COMMONTYPE_REQTPDG						0x0F17

#define DEF_NOTIFY_DEATHMATCHSTART					0x0F25
#define DEF_NOTIFY_DEATHMATCHEND					0x0F27

// #define MSGID_COMMAND_CHECKCONNECTION			0x03203203 // client -> gserver (ANTI-HACK)
#define MSGID_COMMAND_CHECKCONNECTION			0x03203203 + DEF_VERSIONPLUSMODIFIER // client -> gserver (ANTI-HACK)
#define MSGID_COMMAND_CHATMSG					0x03203204 // client -> gserver 

#define MSGID_REQUEST_REGISTERGAMESERVER	0x0512A3F4	
#define MSGID_RESPONSE_REGISTERGAMESERVER	0x0512A3F5
#define MSGID_REQUEST_REGISTERDBSERVER		0x0512A3F6
#define MSGID_RESPONSE_REGISTERDBSERVER		0x0512A3F7
#define MSGID_REQUEST_REGISTERGAMESERVERSOCKET	0x0512A3F8
#define MSGID_RESPONSE_REGISTERGAMESERVERSOCKET	0x0512A3F9

#define MSGID_REQUEST_LOGIN					0x0FC94201
#define MSGID_REQUEST_CREATENEWACCOUNT		0x0FC94202
#define MSGID_RESPONSE_LOG					0x0FC94203	// Login, createnewaccount, createnewcharacter, deletecharacter 등등에 대한 응답 
#define MSGID_REQUEST_CREATENEWCHARACTER	0x0FC94204
#define MSGID_REQUEST_ENTERGAME				0x0FC94205
#define MSGID_RESPONSE_ENTERGAME			0x0FC94206
#define MSGID_REQUEST_DELETECHARACTER		0x0FC94207
#define MSGID_REQUEST_CREATENEWGUILD		0x0FC94208
#define MSGID_RESPONSE_CREATENEWGUILD		0x0FC94209
#define MSGID_REQUEST_DISBANDGUILD			0x0FC9420A
#define MSGID_RESPONSE_DISBANDGUILD			0x0FC9420B
#define MSGID_REQUEST_HELDENIAN_WINNER		0x3D001240
#define MSGID_3D001242h						0x3D001242
#define MSGID_3D00123Eh						0x3D00123E

#define MSGID_REQUEST_UPDATEGUILDINFO_NEWGUILDSMAN		0x0FC9420C
#define MSGID_REQUEST_UPDATEGUILDINFO_DELGUILDSMAN		0x0FC9420D

#define MSGID_REQUEST_CIVILRIGHT			0x0FC9420E
#define MSGID_RESPONSE_CIVILRIGHT			0x0FC9420F

#define MSGID_REQUEST_CHANGEPASSWORD		0x0FC94210
#define MSGID_RESPONSE_CHANGEPASSWORD		0x0FC94211

#define DEF_LOGRESMSGTYPE_CONFIRM						0x0F14
#define DEF_LOGRESMSGTYPE_REJECT						0x0F15
#define DEF_LOGRESMSGTYPE_PASSWORDMISMATCH				0x0F16
#define DEF_LOGRESMSGTYPE_NOTEXISTINGACCOUNT			0x0F17
#define DEF_LOGRESMSGTYPE_NEWACCOUNTCREATED			0x0F18
#define DEF_LOGRESMSGTYPE_NEWACCOUNTFAILED				0x0F19
#define DEF_LOGRESMSGTYPE_ALREADYEXISTINGACCOUNT		0x0F1A
#define DEF_LOGRESMSGTYPE_NOTEXISTINGCHARACTER			0x0F1B
#define DEF_LOGRESMSGTYPE_NEWCHARACTERCREATED			0x0F1C
#define DEF_LOGRESMSGTYPE_NEWCHARACTERFAILED			0x0F1D
#define DEF_LOGRESMSGTYPE_ALREADYEXISTINGCHARACTER		0x0F1E
#define DEF_LOGRESMSGTYPE_CHARACTERDELETED				0x0F1F
#define DEF_LOGRESMSGTYPE_PASSWORDCHANGESUCCESS   	    0x0A00			
#define DEF_LOGRESMSGTYPE_PASSWORDCHANGEFAIL			0x0A01
#define DEF_LOGRESMSGTYPE_NOTEXISTINGWORLDSERVER		0x0A02			

// 게임 접근시 옵션 
#define DEF_ENTERGAMEMSGTYPE_NEW						0x0F1C
#define DEF_ENTERGAMEMSGTYPE_NOENTER_FORCEDISCONN		0x0F1D
#define DEF_ENTERGAMEMSGTYPE_CHANGINGSERVER			0x0F1E

// 게임 접근 결과 
#define DEF_ENTERGAMERESTYPE_PLAYING			0x0F20
#define DEF_ENTERGAMERESTYPE_REJECT			0x0F21
#define DEF_ENTERGAMERESTYPE_CONFIRM			0x0F22
#define DEF_ENTERGAMERESTYPE_FORCEDISCONN		0x0F23

#define MSGID_REQUEST_PLAYERDATA			0x0C152210
#define MSGID_RESPONSE_PLAYERDATA			0x0C152211
#define MSGID_RESPONSE_SAVEPLAYERDATA_REPLY	0x0C152212
#define MSGID_REQUEST_SAVEPLAYERDATA		0x0DF3076F
#define MSGID_REQUEST_SAVEPLAYERDATA_REPLY	0x0DF30770	// 로그서버로부터 세이브됨의 응답을 받아야 하는 경우 
#define MSGID_REQUEST_SAVEPLAYERDATALOGOUT	0x0DF3074F
#define MSGID_REQUEST_NOSAVELOGOUT			0x0DF30750


#define MSGID_REQUEST_RETRIEVEITEM			0x0DF30751
#define MSGID_RESPONSE_RETRIEVEITEM			0x0DF30752

#define MSGID_REQUEST_FULLOBJECTDATA		0x0DF40000

// 로그서버에서 게임서버로 통보하는 메시지 
#define MSGID_GUILDNOTIFY					0x0DF30760
#define DEF_GUILDNOTIFY_NEWGUILDSMAN		0x1F00

// 텔레포트 
#define MSGID_REQUEST_TELEPORT				0x0EA03201
//#define MSGID_REQUEST_CITYHALLTELEPORT		0x0EA03202
//#define MSGID_RESPONSE_CITYHALLTELEPORT		0x0EA03203
//	0x0EA03204
#define MSGID_RESPONSE_CHARGED_TELEPORT			0x0EA03205
//#define MSGID_REQUEST_HELDENIANTELEPORT		0x0EA03206
//#define	MSGID_REQUEST_GAILTELEPORT			0x0EA03207
//#define MSGID_REQUEST_NPCSPEAKTELEPORT		0x0EA03208



#define MSGID_REQUEST_TELEPORT_LIST			0x0EA03202
#define MSGID_RESPONSE_TELEPORT_LIST		0x0EA03203
#define MSGID_REQUEST_CHARGED_TELEPORT		0x0EA03204



// Centuu : Agregado cartel YES-NO para ChangeCity
#define DEF_NOTIFY_CHANGECITY 0x0CA1
#define DEF_REQUEST_CHANGECITY_YES 0x0FC9422E
#define DEF_REQUEST_CHANGECITY_NO  0x0FC9423E


#define DEF_COMMONTYPE_REQ_PURCHASEITEM2			0x0F14
#define DEF_NOTIFY_ITEMTRADE						0x0F15

#define DEF_COMMONTYPE_REQDKSET					0x0F18


// Centuu : EVENT CODE
#define DEF_NOTIFY_EVENTTPOFF 0x0AF3
#define DEF_NOTIFY_EVENTSTART 0x0AF6
#define DEF_NOTIFY_EVENTEND      0x0AF9


#define MSGID_REQUEST_HELDENIAN_SCROLL		0x3D001244

//#define MSGID_LEVELUPSETTINGS				0x11A01000
// MOG Definitions - 3.51
// Level up MSG
#define MSGID_LEVELUPSETTINGS				0x11A01000
// 2003-04-14 지존 포인트를 레벨 수정에 쓸수 있다...
// Stat Point Change MSG
#define MSGID_STATECHANGEPOINT				0x11A01001

#define MSGID_DYNAMICOBJECT					0x12A01001

#define MSGID_GAMESERVERALIVE				0x12A01002
#define MSGID_ADMINUSER						0x12A01003

#define MSGID_GAMESERVERDOWN				0x12A01004
#define MSGID_TOTALGAMESERVERCLIENTS		0x12A01005

#define MSGID_ENTERGAMECONFIRM				0x12A01006

// v1.4311-3 선언 사투장 예약 관련 메세지 
#define MSGID_REQUEST_FIGHTZONE_RESERVE		0x12A01007
#define MSGID_RESPONSE_FIGHTZONE_RESERVE	0x12A01008

#define DEF_MSGID_ANNOUNCEACCOUNT			0x13000000

#define MSGID_ACCOUNTINFOCHANGE				0x13000001
#define MSGID_IPINFOCHANGE					0x13000002

#define MSGID_GAMESERVERSHUTDOWNED			0x14000000
#define MSGID_ANNOUNCEACCOUNTNEWPASSWORD	0x14000010

#define MSGID_REQUEST_IPIDSTATUS				0x14E91200
#define MSGID_RESPONSE_IPIDSTATUS				0x14E91201
#define MSGID_REQUEST_ACCOUNTCONNSTATUS			0x14E91202
#define MSGID_RESPONSE_ACCOUNTCONNSTATUS		0x14E91203
#define MSGID_REQUEST_CLEARACCOUNTCONNSTATUS	0x14E91204
#define MSGID_RESPONSE_CLEARACCOUNTCONNSTATUS	0x14E91205

#define MSGID_REQUEST_FORCEDISCONECTACCOUNT		0x15000000
#define MSGID_REQUEST_NOCOUNTINGSAVELOGOUT	    0x15000001

#define MSGID_OCCUPYFLAGDATA					0x167C0A30
#define MSGID_REQUEST_SAVEARESDENOCCUPYFLAGDATA	0x167C0A31
#define MSGID_REQUEST_SAVEELVINEOCCUPYFLAGDATA	0x167C0A32

#define MSGID_ARESDENOCCUPYFLAGSAVEFILECONTENTS 0x17081034
#define MSGID_ELVINEOCCUPYFLAGSAVEFILECONTENTS  0x17081035

#define MSGID_REQUEST_SETITEMPOS				0x180ACE0A

#define MSGID_BWM_INIT							0x19CC0F82
#define MSGID_BWM_COMMAND_SHUTUP				0x19CC0F84

#define MSGID_SENDSERVERSHUTDOWNMSG				0x20000000
#define MSGID_ITEMLOG							0x210A914D
#define MSGID_GAMEMASTERLOG						0x210A914E // New 08/05/2004

#define MSGID_REQUEST_NOTICEMENT				0x220B2F00
#define MSGID_RESPONSE_NOTICEMENT				0x220B2F01

#define MSGID_REGISTER_WORLDSERVER					0x23AA210E
#define MSGID_REGISTER_WORLDSERVERSOCKET			0x23AA210F
#define MSGID_REGISTER_WORLDSERVER_GAMESERVER		0x23AB211F

#define MSGID_REQUEST_CHARINFOLIST				0x23AB2200
#define MSGID_RESPONSE_CHARINFOLIST				0x23AB2201

#define MSGID_REQUEST_REMOVEGAMESERVER			0x2400000A
#define MSGID_REQUEST_CLEARACCOUNTSTATUS		0x24021EE0

#define MSGID_REQUEST_SETACCOUNTINITSTATUS		0x25000198
#define MSGID_REQUEST_SETACCOUNTWAITSTATUS		0x25000199

#define MSGID_REQUEST_CHECKACCOUNTPASSWORD		0x2654203A
#define MSGID_WORLDSERVERACTIVATED				0x27049D0C

#define MSGID_REQUEST_PANNING					0x27B314D0
#define MSGID_RESPONSE_PANNING					0x27B314D1 // 0x27B314D1
// 0,1,2,3, 
// 210A9150
#define MSGID_REQUEST_RESTART						0x28010EEE
#define MSGID_RESPONSE_REGISTER_WORLDSERVERSOCKET	0x280120A0

#define MSGID_REQUEST_BLOCKACCOUNT					0x2900AD10
#define MSGID_IPTIMECHANGE							0x2900AD20
#define MSGID_ACCOUNTTIMECHANGE						0x2900AD22
#define MSGID_REQUEST_IPTIME						0x2900AD30
#define MSGID_RESPONSE_IPTIME						0x2900AD31

#define MSGID_REQUEST_SELLITEMLIST					0x2900AD30

#define MSGID_REQUEST_ALL_SERVER_REBOOT				0x3AE8270A
#define MSGID_REQUEST_EXEC_1DOTBAT					0x3AE8370A
#define MSGID_REQUEST_EXEC_2DOTBAT					0x3AE8470A
#define MSGID_MONITORALIVE							0x3AE8570A
#define MSGID_UNK1									0x3AE8670A
#define MSGID_UNK2									0x3AE8770A

#define MSGID_COLLECTEDMANA							0x3AE90000
#define MSGID_METEORSTRIKE							0x3AE90001

#define MSGID_SERVERSTOCKMSG						0x3AE90013

// Gate-Server간 전달 내용
#define GSM_REQUEST_FINDCHARACTER					0x01
#define GSM_RESPONSE_FINDCHARACTER					0x02
#define GSM_GRANDMAGICRESULT						0x03
#define GSM_GRANDMAGICLAUNCH						0x04
#define GSM_COLLECTEDMANA							0x05
#define GSM_BEGINCRUSADE							0x06
#define GSM_ENDCRUSADE								0x07
#define GSM_MIDDLEMAPSTATUS							0x08
#define GSM_SETGUILDTELEPORTLOC						0x09
#define GSM_CONSTRUCTIONPOINT						0x0A
#define GSM_SETGUILDCONSTRUCTLOC					0x0B
#define GSM_CHATMSG									0x0C
#define GSM_WHISFERMSG								0x0D
#define GSM_DISCONNECT								0x0E
#define GSM_REQUEST_SUMMONPLAYER					0x0F
#define GSM_REQUEST_SHUTUPPLAYER					0x10
#define GSM_RESPONSE_SHUTUPPLAYER					0x11
#define GSM_REQUEST_SETFORCERECALLTIME				0x12
#define GSM_BEGINAPOCALYPSE							0x13
#define GSM_ENDAPOCALYPSE							0x14
#define GSM_REQUEST_SUMMONGUILD						0x15
#define GSM_REQUEST_SUMMONALL						0x16
#define GSM_ENDHELDENIAN							0x17
#define GSM_UPDATECONFIGS							0x18
#define GSM_STARTHELDENIAN							0x19
// 2.06
#define DEF_COMMONTYPE_REQ_CHANGEPLAYMODE			0x0A60
#define DEF_NOTIFY_CHANGEPLAYMODE					0x0BA9

// Party Code
#define MSGID_PARTYOPERATION		0x3C00123A
#define DEF_PARTYSTATUS_PROCESSING	1
#define DEF_PARTYSTATUS_NULL		0
#define DEF_PARTYSTATUS_CONFIRM		2

// Upgrade code
#define DEF_COMMONTYPE_UPGRADEITEM					0x0A58
#define DEF_ITEMLOG_UPGRADESUCCESS					30
#define DEF_ITEMLOG_UPGRADEFAIL						29
#define DEF_COMMONTYPE_REQGUILDNAME					0x0A59

#define DEF_COMMONTYPE_ENCHANTITEM					0x0A65

// Log Msg
#define MSGID_GAMEITEMLOG							0x210A914F

// Crusade
#define DEF_CRUSADELOG_ENDCRUSADE		1
#define DEF_CRUSADELOG_STARTCRUSADE		2
#define DEF_CRUSADELOG_SELECTDUTY		3
#define DEF_CRUSADELOG_GETEXP			4
//#define MSGID_GAMECRUSADELOG			0x210A914F

#define MSGID_GAMECRUSADELOG			0x210A914C

// NPC drops
#define DEF_ITEMSPREAD_RANDOM	1
#define DEF_ITEMSPREAD_FIXED	2
#define MAX_NPCITEMDROP			25


// Slates
#define DEF_COMMONTYPE_REQ_CREATESLATE		0x0A61
#define DEF_NOTIFY_SLATECLEAR				99

//PK
#define  DEF_PKLOG_REDUCECRIMINAL					1
#define  DEF_PKLOG_BYPLAYER							2
#define  DEF_PKLOG_BYPK								3
#define  DEF_PKLOG_BYENERMY							4
#define  DEF_PKLOG_BYNPC							5
#define  DEF_PKLOG_BYOTHER							6

// Ressurection
#define DEF_REQUEST_RESURRECTPLAYER_NO				0x0FC94215
#define DEF_REQUEST_RESURRECTPLAYER_YES				0x0FC94214

//Magn0S:: Quest List
#define DEF_MSGID_RESPONSE_QUEST_LIST			0x1565
#define DEF_MSGID_REQUEST_QUEST_LIST			0x1566

#define DEF_NOTIFY_COINPOINTS                   0x1570
#define DEF_NOTIFY_CONTRIBPOINTS                0x1572
#define DEF_COMMONTYPE_CLIENTMSG    			0x0F5B
#define DEF_NOTIFY_EVENTUPDATER					0x002070
#define DEF_NOTIFY_QUESTPRIZE                   0x0F5C
#define DEF_COMMONTYPE_REQUEST_QUESTPRIZE       0x0F5D

#define MSGID_MAP_RESTRI						0x002075
#define MSGID_PLAYER_ATTR						0x002085
#define DEF_NOTIFY_SERVERTIME					0x002095
#define DEF_NOTIFY_ABBYAPPEAR					0x003010
#define DEF_NOTIFY_MONSTERCOUNTAPOC				0x003020

// VAMP - elemental armours attunement
#define DEF_NOTIFY_ATTUNEARMOUR					0x0D19
// VAMP - notifiers
#define DEF_NOTIFY_ARMOURVALUES					0x0D20