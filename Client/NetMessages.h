#include "GlobalDef.h"

#ifndef DEF_VERSIONPLUSMODIFIER
#define DEF_VERSIONPLUSMODIFIER 0 // If the antihack was noy defined don't change any msgid
#endif
// NetMessages.h

 #define DEF_COMMONTYPE_REQ_PURCHASEITEM2			0x0F14
 #define DEF_NOTIFY_ITEMTRADE						0x0F15

#define DEF_NOTIFY_TOP15HB                                                     0x0F26

#define DEF_NOTIFY_UPGRADEHEROCAPE 0x0BC0

#define DEF_NOTIFY_EVENT				0x0BFC
#define DEF_NOTIFY_EVENT_INFO			0x0BFD

#define DEF_NOTIFY_CHANGECITY 0x0CA1
#define DEF_REQUEST_CHANGECITY_YES 0x0FC9422E

#define DEF_NOTIFY_TEAMARENA			0x0BEE

#define MSGID_REQUEST_SETTRAP		0x0BFB
#define MSGID_EVENTS			0x002071

#define NOTIFY_EQUIPITEM2								0x0B99D
#define NOTIFYMSG										    0x0C56

#define MINIMAPBLUE_CLEAR									0x1F34
#define MINIMAPBLUE_UPDATE									0x1F35
#define MINIMAPRED_CLEAR									0x1F36
#define MINIMAPRED_UPDATE									0x1F37

 #define DEF_COMMONTYPE_REQDKSET					0x0F18

#define MSGID_REQUEST_TOPEK 0x3B1890EC

//News Addons - ZeroEoyPnk
#define DEF_SEND_PARTYCOORDS				0x1F07
#define DEF_SEND_PARTYHP					0x1F08
#define DEF_RESQUEST_PARTYHP				0x1F09

// VAMP - online users list
#define MSGID_REQUEST_ONLINE						0x3B1890EB
#define DEF_NOTIFY_USERJOIN							0x0C04


// Centuu : EVENT CODE
#define DEF_NOTIFY_EVENTTPOFF 0x0AF3
#define DEF_NOTIFY_EVENTSTART 0x0AF6
#define DEF_NOTIFY_EVENTEND      0x0AF9

#define DEF_NOTIFY_DEATHS	0x0F37

//50Cent - HP Bar
#define DEF_COMMONTYPE_REQ_GETNPCHP					0x1F12
#define DEF_SEND_NPCHP								0x1F13

#define MSGID_REQUEST_PING                    0x0FFAACCA
#define MSGID_RESPONSE_PING                    0x0FFAACCB

//50Cent - Repair All
#define DEF_COMMONTYPE_REQ_REPAIRALL            0x0F10
#define DEF_NOTIFY_REPAIRALLPRICES                0x0F11
#define DEF_COMMONTYPE_REQ_REPAIRALLDELETE        0x0F12
#define DEF_COMMONTYPE_REQ_REPAIRALLCONFIRM        0x0F13

#define DEF_NOTIFY_RESUR_ON					0x0C49
#define DEF_NOTIFY_RESUR_OFF				        0x0C50

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

#define DEF_COMMONTYPE_ITEMDROP					0x0A01
#define DEF_COMMONTYPE_EQUIPITEM				0x0A02
#define DEF_COMMONTYPE_REQ_LISTCONTENTS			0x0A03
#define DEF_COMMONTYPE_REQ_PURCHASEITEM			0x0A04
#define DEF_COMMONTYPE_GIVEITEMTOCHAR			0x0A05
#define DEF_COMMONTYPE_JOINGUILDAPPROVE			0x0A06
#define DEF_COMMONTYPE_JOINGUILDREJECT			0x0A07
#define DEF_COMMONTYPE_DISMISSGUILDAPPROVE		0x0A08
#define DEF_COMMONTYPE_DISMISSGUILDREJECT		0x0A09
#define DEF_COMMONTYPE_RELEASEITEM				0x0A0A
#define DEF_COMMONTYPE_TOGGLECOMBATMODE			0x0A0B
#define DEF_COMMONTYPE_SETITEM					0x0A0C
#define DEF_COMMONTYPE_MAGIC					0x0A0D
#define DEF_COMMONTYPE_REQ_STUDYMAGIC			0x0A0E
#define DEF_COMMONTYPE_REQ_TRAINSKILL			0x0A0F

#define DEF_COMMONTYPE_REQ_GETREWARDMONEY		0x0A10
#define DEF_COMMONTYPE_REQ_USEITEM				0x0A11
#define DEF_COMMONTYPE_REQ_USESKILL				0x0A12
#define DEF_COMMONTYPE_REQ_SELLITEM				0x0A13
#define DEF_COMMONTYPE_REQ_REPAIRITEM			0x0A14
#define DEF_COMMONTYPE_REQ_SELLITEMCONFIRM		0x0A15
#define DEF_COMMONTYPE_REQ_REPAIRITEMCONFIRM	0x0A16
#define DEF_COMMONTYPE_REQ_GETFISHTHISTIME		0x0A17
#define DEF_COMMONTYPE_TOGGLESAFEATTACKMODE		0x0A18
#define DEF_COMMONTYPE_REQ_CREATEPORTION		0x0A19 // alchemy now
#define DEF_COMMONTYPE_TALKTONPC				0x0A1A
#define DEF_COMMONTYPE_REQ_SETDOWNSKILLINDEX	0x0A1B
#define DEF_COMMONTYPE_REQ_GETOCCUPYFLAG		0x0A1C
#define DEF_COMMONTYPE_REQ_GETHEROMANTLE		0x0A1D
#define DEF_COMMONTYPE_EXCHANGEITEMTOCHAR		0x0A1E
#define DEF_COMMONTYPE_SETEXCHANGEITEM			0x0A1F

#define DEF_COMMONTYPE_CONFIRMEXCHANGEITEM		0x0A20
#define DEF_COMMONTYPE_CANCELEXCHANGEITEM		0x0A21
#define DEF_COMMONTYPE_QUESTACCEPTED			0x0A22
#define DEF_COMMONTYPE_BUILDITEM				0x0A23 // Manuf
#define DEF_COMMONTYPE_GETMAGICABILITY			0x0A24
#define DEF_COMMONTYPE_CLEARGUILDNAME			0x0A25 


// _CLEARGUILDNAME is used hen received from server, not implemented in HBx gserver
#define DEF_COMMONTYPE_REQ_GETOCCUPYFIGHTZONETICKET	0x0A25
#define DEF_COMMONTYPE_BANGUILD					0x0A26

#define DEF_COMMONTYPE_CRAFTITEM				0x0A28 // Craft item


#define DEF_COMMONTYPE_REQUEST_ACCEPTJOINPARTY			0x0A30
#define DEF_COMMONTYPE_REQUEST_JOINPARTY				0x0A31
#define DEF_COMMONTYPE_RESPONSE_JOINPARTY				0x0A32

#define DEF_COMMONTYPE_REQUEST_ACTIVATESPECABLTY		0x0A40

#define DEF_COMMONTYPE_REQUEST_CANCELQUEST				0x0A50
#define DEF_COMMONTYPE_REQUEST_SELECTCRUSADEDUTY		0x0A51
#define DEF_COMMONTYPE_REQUEST_MAPSTATUS				0x0A52
#define DEF_COMMONTYPE_REQUEST_HELP						0x0A53
#define DEF_COMMONTYPE_SETGUILDTELEPORTLOC				0x0A54
#define DEF_COMMONTYPE_GUILDTELEPORT					0x0A55
#define DEF_COMMONTYPE_SUMMONWARUNIT					0x0A56
#define DEF_COMMONTYPE_SETGUILDCONSTRUCTLOC				0x0A57
#define DEF_COMMONTYPE_UPGRADEITEM						0x0A58
#define DEF_COMMONTYPE_REQGUILDNAME						0x0A59

#define DEF_COMMONTYPE_REQUEST_HUNTMODE					0x0A60
#define DEF_COMMONTYPE_REQ_CREATESLATE					0x0A61



 #define MSGID_NOTIFY							0x0FA314D0
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
 #define DEF_NOTIFY_LEVELUP							0x0B16 // levelup
 #define DEF_NOTIFY_ITEMLIFESPANEND					0x0B17
 #define DEF_NOTIFY_LIMITEDLEVEL					0x0B18 //  "Trial version user can not rise anymore the experience!"
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
 #define DEF_NOTIFY_TOTALUSERS						0x0B29 // "Total players connected: %d."
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
 #define DEF_NOTIFY_TRAVELERLIMITEDLEVEL			0x0B38 // "Travelers cannot level up to 20. Become a citizen."
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
 #define DEF_NOTIFY_NOMATCHINGPORTION				0x0B53
 #define DEF_NOTIFY_LOWPORTIONSKILL					0x0B54
 #define DEF_NOTIFY_PORTIONFAIL						0x0B55
 #define DEF_NOTIFY_PORTIONSUCCESS					0x0B56
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
 #define DEF_NOTIFY_EXCHANGEITEMCOMPLETE			0x0B61 // Item exchanged!
 #define DEF_NOTIFY_CANNOTGIVEITEM					0x0B62
 #define DEF_NOTIFY_GIVEITEMFIN_COUNTCHANGED        0x0B63
 #define DEF_NOTIFY_DROPITEMFIN_COUNTCHANGED		0x0B64 // You threw %d %s away on the ground.
 #define DEF_NOTIFY_ITEMCOLORCHANGE					0x0B65
 #define DEF_NOTIFY_QUESTCONTENTS					0x0B66
 #define DEF_NOTIFY_QUESTABORTED					0x0B67
 #define DEF_NOTIFY_QUESTCOMPLETED					0x0B68 // Congratulations! You completed your quest!
 #define DEF_NOTIFY_QUESTREWARD						0x0B69

 #define DEF_NOTIFY_BUILDITEMSUCCESS				0x0B70 // Item manufacture success!
 #define DEF_NOTIFY_BUILDITEMFAIL					0x0B71 // Failed on manufacturing item.
 #define DEF_NOTIFY_OBSERVERMODE					0x0B72 // Observer Mode On. Press 'SHIFT + ESC' to Log Out...
 #define DEF_NOTIFY_GLOBALATTACKMODE				0x0B73 // Attack prohibition...
 #define DEF_NOTIFY_DAMAGEMOVE						0x0B74
 #define DEF_NOTIFY_FORCEDISCONN					0x0B75
 #define DEF_NOTIFY_FIGHTZONERESERVE				0x0B76
 #define DEF_NOTIFY_NOGUILDMASTERLEVEL				0x0B77
 #define DEF_NOTIFY_SUCCESSBANGUILDMAN				0x0B78
 #define DEF_NOTIFY_CANNOTBANGUILDMAN				0x0B79

 #define DEF_NOTIFY_RESPONSE_CREATENEWPARTY			0x0B80
 #define DEF_NOTIFY_QUERY_JOINPARTY					0x0B81
 
 #define DEF_NOTIFY_ENERGYSPHERECREATED				0x0B90 // Energy sphere was dropped in (%d, %d) of middleland!
 #define DEF_NOTIFY_ENERGYSPHEREGOALIN				0x0B91 // You pushed energy sphere to enemy's energy portal! Contribution point will be decreased by 10 points.
 #define DEF_NOTIFY_SPECIALABILITYENABLED			0x0B92 // You can now use a special ability!
 #define DEF_NOTIFY_SPECIALABILITYSTATUS			0x0B93 // Use special ability!
 #define DEF_NOTIFY_CRUSADE							0x0B94 // Beginners cannot go to the town during all-out battle.
 #define DEF_NOTIFY_LOCKEDMAP						0x0B95 // You can't move to other position for %d seconds except %s
 #define DEF_NOTIFY_DUTYSELECTED					0x0B96
 #define DEF_NOTIFY_MAPSTATUSNEXT					0x0B97
 #define DEF_NOTIFY_MAPSTATUSLAST					0x0B98
 #define DEF_NOTIFY_HELP							0x0B99
 #define DEF_NOTIFY_HELPFAILED						0x0B9A
 #define DEF_NOTIFY_METEORSTRIKECOMING				0x0B9B
 #define DEF_NOTIFY_METEORSTRIKEHIT					0x0B9C
 #define DEF_NOTIFY_GRANDMAGICRESULT				0x0B9D
 #define DEF_NOTIFY_NOMORECRUSADESTRUCTURE			0x0B9E
 #define DEF_NOTIFY_CONSTRUCTIONPOINT				0x0B9F // %s +%d, %s +%d

 #define DEF_NOTIFY_TCLOC							0x0BA0
 #define DEF_NOTIFY_CANNOTCONSTRUCT					0X0BA1 // %s XY(%d, %d)
 #define DEF_NOTIFY_PARTY							0x0BA2
 #define DEF_NOTIFY_ITEMATTRIBUTECHANGE				0x0BA3
 #define DEF_NOTIFY_GIZONITEMUPGRADELEFT			0x0BA4 // Item upgrade is possible.
 #define DEF_NOTIFY_GIZONEITEMCHANGE				0x0BA5
 #define DEF_NOTIFY_REQGUILDNAMEANSWER			    0x0BA6
 #define DEF_NOTIFY_FORCERECALLTIME					0x0BA7
 #define DEF_NOTIFY_ITEMUPGRADEFAIL					0x0BA8
 #define DEF_NOTIFY_CHANGEPLAYMODE					0x0BA9 // Snoopy: renomm� comme sur serveur // Play mode has been changed.
 #define DEF_NOTIFY_SPAWNEVENT						0x0BAA // Snoopy: renomm� comme sur serveur

 #define DEF_NOTIFY_NOMOREAGRICULTURE				0x0BB0  //You cannot farm any more.
 #define DEF_NOTIFY_AGRICULTURESKILLLIMIT			0x0BB1  //Farming is not possible with your current farming skill.
 #define DEF_NOTIFY_AGRICULTURENOAREA				0x0BB2  //This is not an area to farm.
 #define DEF_NOTIFY_SETTING_SUCCESS					0x0BB3 // Your stat has been changed. // CLEROTH - LU
 #define DEF_NOTIFY_SETTING_FAILED					0x0BB4 // Your stat has been changed. // Inutilis� ???
 #define DEF_NOTIFY_STATECHANGE_SUCCESS				0x0BB5 // Your stat has been changed. // MJ Stats Change - Diuuude
 #define DEF_NOTIFY_STATECHANGE_FAILED				0x0BB6 // MJ Stats Change - Diuuude

 #define DEF_NOTIFY_SLATE_CREATESUCCESS				0x0BC1 // Slates - Diuuude
 #define DEF_NOTIFY_SLATE_CREATEFAIL				0x0BC2

  #define DEF_NOTIFY_NORECALL						0x0BD1 // "You cannot recall on this map"
  #define DEF_NOTIFY_APOCGATESTARTMSG				0x0BD2 // "The portal to the appoclypse gate is open."
  #define DEF_NOTIFY_APOCGATEENDMSG					0x0BD3 // "The portal to the Apocalypse is closed."
  #define DEF_NOTIFY_APOCGATEOPEN					0x0BD4
  #define DEF_NOTIFY_APOCGATECLOSE					0x0BD5 
  #define DEF_NOTIFY_ABADDONKILLED					0x0BD6 // "Abaddon destroyed by (playername)"
  #define DEF_NOTIFY_APOCFORCERECALLPLAYERS			0x0BD7 // "You are forced to recall, because the apoclypse has started"
  #define DEF_NOTIFY_SLATE_INVINCIBLE				0x0BD8 // "You are invincible by divine protection of Kloness, the GOD OF JUSTICE."
  #define DEF_NOTIFY_SLATE_MANA						0x0BD9 // "Your mana will be refilled by great Wizard."

  #define DEF_NOTIFY_SLATE_EXP 						0x0BE0
  #define DEF_NOTIFY_SLATE_STATUS					0x0BE1 // "The effect of the prophecy-slate is disappeared."
  #define DEF_NOTIFY_QUESTCOUNTER					0x0BE2 // by Snoopy	
  #define DEF_NOTIFY_MONSTERCOUNT					0x0BE3 // Rest Monsters %d ... Used only by Admin order
  #define DEF_NOTIFY_0BE5							0x0BE5 // Abbadon's related, thunder? 
  #define DEF_NOTIFY_HELDENIANTELEPORT				0x0BE6 // Teleport to Heldinian field is available from now on as Heldenian holy
														   // war begins. Magic casting is forbidden until real battle.
  #define DEF_NOTIFY_HELDENIANEND					0x0BE7 // Heldenian holy war has been closed.
  #define DEF_NOTIFY_0BE8							0x0BE8 // Characters will be recalled by force as Heldenian begins.
  #define DEF_NOTIFY_RESURRECTPLAYER				0x0BE9
  #define DEF_NOTIFY_HELDENIANSTART					0x0BEA // Heldenian real battle has been started form now on.
  #define DEF_NOTIFY_HELDENIANVICTORY				0x0BEB // HELDENIAN FINISHED FOR Elvine/aresden
  #define DEF_NOTIFY_HELDENIANCOUNT					0x0BEC // Sends client nbe of destroyed towers, deaths....

// reversed 3.60+ MSGIDs by Snoopy
  #define DEF_NOTIFY_SLATE_BERSERK					0x0BED // Berserk magic casted!
  //#define DEF_NOTIFY_LOTERY_LOST					0x0BEE // You draw a blank. Please try again next time..
  #define DEF_NOTIFY_0BEF							0x0BEF // Strange behaviour, exits client?
  #define DEF_NOTIFY_CRAFTING_SUCCESS				0x0BF0 // Crafting ok
  #define DEF_NOTIFY_CRAFTING_FAIL					0x0BF1 // Crafting failed


 #define DEF_NOTIFY_MORLEARPJ						0x0F24
#define DEF_NOTIFY_CURLIFESPAN 0x0BF3

  #define DEF_NOTIFY_ANGEL_FAILED					0x0BF4 // Failed receiving an Angel pandent
  #define DEF_NOTIFY_ANGEL_RECEIVED					0x0BF5 // "You have received the Tutelary Angel"

  #define DEF_NOTIFY_SPELL_SKILL					0x0BF6 // Loads Speell list skill list silentlly

// MJ Stats Change - Diuuude
 #define MSGID_STATECHANGEPOINT				0x11A01001

 #define DEF_STR							0x01
 #define DEF_DEX							0x02
 #define DEF_INT							0x03
 #define DEF_VIT							0x04
 #define DEF_MAG							0x05
 #define DEF_CHR							0x06

 #define DEF_NOTIFY_REQRANGO						0x0F02
 #define DEF_COMMONTYPE_REQRANGO					0x0F03
#define DEF_NOTIFY_DGKILL							0x0F04
 #define DEF_NOTIFY_DOUBLEKILL						0x0F05
 #define DEF_NOTIFY_KILLSPRING						0x0F06
 #define DEF_NOTIFY_HOLYSHIT						0x0F07
 #define DEF_NOTIFY_MONSTERKILL						0x0F12
 #define DEF_NOTIFY_REPDGDEATHS						0x0F16
 #define DEF_COMMONTYPE_REQTPDG						0x0F17
 #define DEF_NOTIFY_DEATHMATCHSTART					0x0F20
 #define DEF_NOTIFY_DEATHMATCHEND					0x0F21



// CLEROTH
// #define MSGID_COMMAND_CHECKCONNECTION			0x03203203 // client -> gserver (ANTI-HACK)
#define MSGID_COMMAND_CHECKCONNECTION			0x03203203 + DEF_VERSIONPLUSMODIFIER // client -> gserver (ANTI-HACK)
#define MSGID_COMMAND_CHATMSG					0x03203204 // client -> gserver 


//Teleports
#define MSGID_REQUEST_TELEPORT				0x0EA03201
#define MSGID_REQUEST_TELEPORT_LIST			0x0EA03202
#define MSGID_RESPONSE_TELEPORT_LIST		0x0EA03203
#define MSGID_REQUEST_CHARGED_TELEPORT		0x0EA03204
#define MSGID_RESPONSE_CHARGED_TELEPORT		0x0EA03205
// Snoopy: Heldenian Teleports
#define MSGID_REQUEST_HELDENIAN_TP_LIST		0x0EA03206
#define MSGID_RESPONSE_HELDENIAN_TP_LIST	0x0EA03207
#define MSGID_REQUEST_HELDENIAN_TP			0x0EA03208

#define MSGID_REQUEST_HELDENIAN_SCROLL				0x3D001244 // Add by Snoopy for client buying Heldenian scrolls

// Ressurection
#define DEF_REQUEST_RESURRECTPLAYER_YES				0x0FC94214
#define DEF_REQUEST_RESURRECTPLAYER_NO				0x0FC94215

// Reversed by Snoopy
#define DEF_REQUEST_ANGEL				0x0FC9421E


#define MSGID_REQUEST_REGISTERGAMESERVER	0x0512A3F4 // unused by client	
#define MSGID_RESPONSE_REGISTERGAMESERVER	0x0512A3F5 // unused by client
#define MSGID_REQUEST_REGISTERDBSERVER		0x0512A3F6 // unused by client
#define MSGID_RESPONSE_REGISTERDBSERVER		0x0512A3F7 // unused by client

#define MSGID_ITEMCONFIGURATIONCONTENTS			0x0FA314D9 // unused by client
#define MSGID_NPCCONFIGURATIONCONTENTS			0x0FA314DA // unused by client
#define MSGID_MAGICCONFIGURATIONCONTENTS		0x0FA314DB // unused by client
#define MSGID_SKILLCONFIGURATIONCONTENTS		0x0FA314DC // unused by client (used by client v3.62)
#define MSGID_PLAYERITEMLISTCONTENTS			0x0FA314DD
#define MSGID_PORTIONCONFIGURATIONCONTENTS		0x0FA314DE // unused by client
#define MSGID_PLAYERCHARACTERCONTENTS			0x0FA40000
#define MSGID_QUESTCONFIGURATIONCONTENTS		0x0FA40001 // unused by client
#define MSGID_BUILDITEMCONFIGURATIONCONTENTS	0x0FA40002 // unused by client	
#define MSGID_DUPITEMIDFILECONTENTS				0x0FA40003 // unused by client
#define MSGID_NOTICEMENTFILECONTENTS			0x0FA40004 // unused by client


#define MSGID_REQUEST_LOGIN					0x0FC94201
#define MSGID_REQUEST_CREATENEWACCOUNT		0x0FC94202
#define MSGID_RESPONSE_LOG					0x0FC94203	// Login, createnewaccount, createnewcharacter, deletecharacter 
#define MSGID_REQUEST_CREATENEWCHARACTER	0x0FC94204
#define MSGID_REQUEST_ENTERGAME				0x0FC94205
#define MSGID_RESPONSE_ENTERGAME			0x0FC94206
#define MSGID_REQUEST_DELETECHARACTER		0x0FC94207
#define MSGID_REQUEST_CREATENEWGUILD		0x0FC94208
#define MSGID_RESPONSE_CREATENEWGUILD		0x0FC94209
#define MSGID_REQUEST_DISBANDGUILD			0x0FC9420A
#define MSGID_RESPONSE_DISBANDGUILD			0x0FC9420B

#define MSGID_REQUEST_UPDATEGUILDINFO_NEWGUILDSMAN		0x0FC9420C
#define MSGID_REQUEST_UPDATEGUILDINFO_DELGUILDSMAN		0x0FC9420D

#define MSGID_REQUEST_CIVILRIGHT			0x0FC9420E
#define MSGID_RESPONSE_CIVILRIGHT			0x0FC9420F

#define MSGID_REQUEST_CHANGEPASSWORD		0x0FC94210
#define MSGID_RESPONSE_CHANGEPASSWORD		0x0FC94211

#define MSGID_REQUEST_INPUTKEYCODE			0x0FC94212
#define MSGID_RESPONSE_INPUTKEYCODE			0x0FC94213



//LogServer msgs
#define DEF_LOGRESMSGTYPE_CONFIRM						0x0F14
#define DEF_LOGRESMSGTYPE_REJECT						0x0F15
#define DEF_LOGRESMSGTYPE_PASSWORDMISMATCH				0x0F16
#define DEF_LOGRESMSGTYPE_NOTEXISTINGACCOUNT			0x0F17
#define DEF_LOGRESMSGTYPE_NEWACCOUNTCREATED				0x0F18
#define DEF_LOGRESMSGTYPE_NEWACCOUNTFAILED				0x0F19
#define DEF_LOGRESMSGTYPE_ALREADYEXISTINGACCOUNT		0x0F1A
#define DEF_LOGRESMSGTYPE_NOTEXISTINGCHARACTER			0x0F1B
#define DEF_LOGRESMSGTYPE_NEWCHARACTERCREATED			0x0F1C
#define DEF_LOGRESMSGTYPE_NEWCHARACTERFAILED			0x0F1D
#define DEF_LOGRESMSGTYPE_ALREADYEXISTINGCHARACTER		0x0F1E
#define DEF_LOGRESMSGTYPE_CHARACTERDELETED				0x0F1F
#define DEF_LOGRESMSGTYPE_NOTENOUGHPOINT				0x0F30
#define DEF_LOGRESMSGTYPE_ACCOUNTLOCKED					0x0F31
#define DEF_LOGRESMSGTYPE_SERVICENOTAVAILABLE			0x0F32
#define DEF_LOGRESMSGTYPE_PASSWORDCHANGESUCCESS   	    0x0A00			
#define DEF_LOGRESMSGTYPE_PASSWORDCHANGEFAIL			0x0A01
#define DEF_LOGRESMSGTYPE_NOTEXISTINGWORLDSERVER		0x0A02			
#define DEF_LOGRESMSGTYPE_INPUTKEYCODE					0x0A03
#define DEF_LOGRESMSGTYPE_REALACCOUNT					0x0A04
#define DEF_LOGRESMSGTYPE_FORCECHANGEPASSWORD			0x0A05
#define DEF_LOGRESMSGTYPE_INVALIDKOREANSSN				0x0A06
#define DEF_LOGRESMSGTYPE_LESSTHENFIFTEEN				0x0A07

 
#define DEF_ENTERGAMEMSGTYPE_NEW						0x0F1C
#define DEF_ENTERGAMEMSGTYPE_NOENTER_FORCEDISCONN		0x0F1D
#define DEF_ENTERGAMEMSGTYPE_CHANGINGSERVER				0x0F1E
#define DEF_ENTERGAMEMSGTYPE_NEW_TOWLSBUTMLS			0x0F1F
#define DEF_ENTERGAMERESTYPE_PLAYING					0x0F20
#define DEF_ENTERGAMERESTYPE_REJECT						0x0F21
#define DEF_ENTERGAMERESTYPE_CONFIRM					0x0F22
#define DEF_ENTERGAMERESTYPE_FORCEDISCONN				0x0F23

#define MSGID_REQUEST_PLAYERDATA			0x0C152210
#define MSGID_RESPONSE_PLAYERDATA			0x0C152211
#define MSGID_RESPONSE_SAVEPLAYERDATA_REPLY	0x0C152212
#define MSGID_REQUEST_SAVEPLAYERDATA		0x0DF3076F
#define MSGID_REQUEST_SAVEPLAYERDATA_REPLY	0x0DF30770	// 
#define MSGID_REQUEST_SAVEPLAYERDATALOGOUT	0x0DF3074F
#define MSGID_REQUEST_NOSAVELOGOUT			0x0DF30750


#define MSGID_REQUEST_RETRIEVEITEM			0x0DF30751
#define MSGID_RESPONSE_RETRIEVEITEM			0x0DF30752

#define MSGID_REQUEST_FULLOBJECTDATA		0x0DF40000


#define MSGID_GUILDNOTIFY					0x0DF30760
#define DEF_GUILDNOTIFY_NEWGUILDSMAN		0x1F00


#define MSGID_LEVELUPSETTINGS				0x11A01000
#define MSGID_DYNAMICOBJECT					0x12A01001
#define MSGID_GAMESERVERALIVE				0x12A01002
#define MSGID_ADMINUSER						0x12A01003
#define MSGID_GAMESERVERDOWN				0x12A01004
#define MSGID_TOTALGAMESERVERCLIENTS		0x12A01005

#define MSGID_ENTERGAMECONFIRM				0x12A01006

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
#define MSGID_REQUEST_NOTICEMENT				0x220B2F00
#define MSGID_RESPONSE_NOTICEMENT				0x220B2F01
#define MSGID_REGISTER_WORLDSERVER				0x23AA210E
#define MSGID_REGISTER_WORLDSERVERSOCKET		0x23AA210F
#define MSGID_REGISTER_WORLDSERVER_GAMESERVER	0x23AB211F
#define MSGID_REQUEST_CHARINFOLIST				0x23AB2200
#define MSGID_RESPONSE_CHARINFOLIST				0x23AB2201
#define MSGID_REQUEST_REMOVEGAMESERVER			0x2400000A
#define MSGID_REQUEST_CLEARACCOUNTSTATUS		0x24021EE0
#define MSGID_REQUEST_SETACCOUNTINITSTATUS		0x25000198
#define MSGID_REQUEST_SETACCOUNTWAITSTATUS		0x25000199
#define MSGID_REQUEST_CHECKACCOUNTPASSWORD		0x2654203A
#define MSGID_WORLDSERVERACTIVATED				0x27049D0C
#define MSGID_REQUEST_PANNING					0x27B314D0
#define MSGID_RESPONSE_PANNING					0x27B314D1

#define MSGID_REQUEST_RESTART					0x28010EEE
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

#define MSGID_COLLECTEDMANA							0x3AE90000
#define MSGID_METEORSTRIKE							0x3AE90001

#define MSGID_SERVERSTOCKMSG						0x3AE90013
#define MSGID_GATEWAY_CURRENTCONN					0x3B000000
#define MSGID_GETMINIMUMLOADGATEWAY					0x3B1890EA

// Gate-Server
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

#define MSGID_PARTYOPERATION						0x3C00123A

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