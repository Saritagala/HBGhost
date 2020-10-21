/****************************************************************
*		 This client was writen by Diuuude & Snoopy81.			*
*					Based on Cleroth work.						*
*																*
*			V3.51 compatibility by Cleroth						*
*			V3.51 dialogs by Diuuude							*
*			Effects, mobs, Apocalypse, Heldenian				*
*			& finalizing by Snoopy81							*
*			V3.82 Crafting & Angels by Snoopy81					*
*****************************************************************/

/****************************************************************
*	Find here all compilation options							*
*	I removed useless ones and tryed to add some explanations	*
*	( Snoopy81 06/2005 )										*
*****************************************************************/

/*** Put here global data for your server ***/
#define NAME_WORLDNAME			"WS1"											// Name for default (1st) server
#define MSG_WORLDNAME1			"Helbreath Ghost"								// Long name for 1st server...

#define MSG_HOMEPAGE			"http://www.helbreathghost.com"					// Web Server

#define DEF_UPPERVERSION		3
#define DEF_LOWERVERSION		51

#define DEF_UPPERVERSION2		1
#define DEF_LOWERVERSION2		0

//#define DEF_SERVER_IP			"192.168.0.28"
#define DEF_SERVER_IP			"192.168.100.2" // Host IP
							
#define DEF_SERVER_PORT			2848											// WorldLServer Port

#define RES_HIGH																// Change Resolution from 640x480 to 800x600	

#define SCREENX					80			//LifeX Fix DialogBox Cords
#define SCREENY					60			//LifeX Fix DialogBox Cords

#define MULTI_TRADE																// Enables Multi-Trade

//#define _DEBUG

//#define DEF_WINDOWED_MODE														// Windowed Mode


#define DEF_ANTI_HACK			// Snoopy: Mes antiHacks personels!
								// changes CCM routine to send "real checked time" instead of "sending time". 
								// CCM is reduced to 6150

//#define DEF_MULTI_CLIENT



//Magn0S:: Added
#define CHAT_GLOBAL		2
#define CHAT_PARTY		4
#define CHAT_CITY		3
#define CHAT_GUILD		1
#define CHAT_GM			14
#define CHAT_MARKET		15
#define CHAT_NORMAL		0
#define CHAT_WHISP		20
#define CHAT_SERVERMSG	10	
#define MAXCHATLINES    7

#define DROP_MA				0
#define DROP_PA				1

#define MAXDIALOGBOX	66

#ifdef DEF_ANTI_HACK
	#define DEF_HIDDEN_LOWERVERSION		95
	//that can be whatever number you wish, 1 byte-sized numbers will work fine
	#define DEF_VERSIONPLUSMODIFIER     3+(DEF_HIDDEN_LOWERVERSION % 9) 
#else
	#define DEF_VERSIONPLUSMODIFIER     0 
#endif