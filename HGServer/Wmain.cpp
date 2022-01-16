// -------------------------------------------------------------- 
//                      New Game Server  						  
//
//                      1998.11 by Soph
//
// --------------------------------------------------------------

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include <winbase.h>
#include <mmsystem.h>
#include <time.h>				   
#include "winmain.h"
#include "Game.h"
#include "UserMessages.h"
#include "resource.h"

void PutAdminLogFileList(char * cStr);
void PutHackLogFileList(char * cStr);
void PutPvPLogFileList(char * cStr);

#pragma warning (disable : 4996)

// --------------------------------------------------------------

#define WM_USER_TIMERSIGNAL		WM_USER + 500

#define ID_BUTTON 501

HWND G_hEdt = 0; 
HWND G_hBut = 0;


char			szAppClass[32];
HWND			G_hWnd = 0;
char			G_cMsgList[120*50];
bool            G_cMsgUpdated =	false;
char            G_cTxt[512];
char			G_cData50000[50000];
MMRESULT        G_mmTimer = 0;

class XSocket * G_pListenSock = 0;
class XSocket * G_pPingSock = 0;
class XSocket * G_pLogSock    = 0;
class CGame *   G_pGame       = 0;

int             G_iQuitProgramCount = 0;
bool			G_bIsThread = true;

FILE * pLogFile;

//
char			G_cCrashTxt[50000];
// --------------------------------------------------------------

//MAJORHG - CLEROTH - 26/03/05
static HWND    List1;
static HWND    Edit1;
static HWND    Button;
static HWND    Button2;
static HINSTANCE BCX_hInstance;
static int     BCX_ScaleX;
static int     BCX_ScaleY;
static char    BCX_ClassName[2048];
HWND    BCX_Listbox(char*,HWND,int,int,int,int,int,int=0,int=-1);
HWND    BCX_Editbox(char*,HWND,int,int,int,int,int,int=0,int=-1);
HWND    BCX_Button(char*,HWND,int,int,int,int,int,int=0,int=-1);
HWND    BCX_Button2(char*,HWND,int,int,int,int,int,int=0,int=-1);
int ItemCount = 0;

// 2020
unsigned __stdcall ThreadProc(void* ch)
{
	class CTile* pTile;
	while (G_bIsThread)
	{
		Sleep(1000); // centu - 10000 -> 1000

		for (int a = 0; a < DEF_MAXMAPS; a++)
		{
			if (G_pGame->m_pMapList[a] != 0)
			{
				for (int iy = 0; iy < G_pGame->m_pMapList[a]->m_sSizeY; iy++)
				{
					for (int ix = 0; ix < G_pGame->m_pMapList[a]->m_sSizeX; ix++)
					{
						pTile = (class CTile*)(G_pGame->m_pMapList[a]->m_pTile + ix + iy * G_pGame->m_pMapList[a]->m_sSizeY);
						if ((pTile != 0) && (pTile->m_sOwner != 0) && (pTile->m_cOwnerClass == 0))
						{
							pTile->m_sOwner = 0;
						}
					}
				}
			}
		}
	}

	_endthread();
	return 0;
}

LRESULT CALLBACK WndProc( HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam )
{	
	char cWhat[256];
	switch (message) {

	case WM_COMMAND: 
		switch(wParam) { 
		case ID_BUTTON: 
			
			break; 

		case 1012:
            GetWindowText(Edit1,&cWhat[0],256);
            if (cWhat[0] != 0) {
                SetWindowText(Edit1, &cWhat[0]);
                SetFocus(Edit1);
                G_pGame->ParseCommand(cWhat);
				SetWindowText(Edit1, "");
			}
            break;

        case 1010:
			G_cMsgUpdated = true;
            SendMessage(List1,(UINT)LB_RESETCONTENT, 0, 0);
            ItemCount = 0;
            PutLogList(" ");
            break;
		} 
		break;

	case WM_CREATE:
		break;
	
	case WM_KEYDOWN:
		G_pGame->OnKeyDown(wParam, lParam);
		return (DefWindowProc(hWnd, message, wParam, lParam));
		break;

	case WM_KEYUP:
		G_pGame->OnKeyUp(wParam, lParam);
		return (DefWindowProc(hWnd, message, wParam, lParam));
		break;
	
	case WM_USER_STARTGAMESIGNAL:
		G_pGame->OnStartGameSignal();
		break;
	
	case WM_USER_TIMERSIGNAL:
		G_pGame->OnTimer(0);
		break;

	case WM_USER_ACCEPT:
		OnAccept();
		break;

	case WM_PAINT:
		OnPaint();
		break;

	case WM_DESTROY:
		OnDestroy();
		break;

	case WM_CLOSE:
		if (G_pGame->bOnClose() == true) return (DefWindowProc(hWnd, message, wParam, lParam));
		break;

	case WM_ONGATESOCKETEVENT:
		G_pGame->OnGateSocketEvent(message, wParam, lParam);
		break;

	case WM_ONLOGSOCKETEVENT:
		G_pGame->OnMainLogSocketEvent(message, wParam, lParam);
		break;
	
	default: 
		if ((message >= WM_ONLOGSOCKETEVENT + 1) && (message <= WM_ONLOGSOCKETEVENT + DEF_MAXSUBLOGSOCK))
			G_pGame->OnSubLogSocketEvent(message, wParam, lParam);
		
		if ((message >= WM_ONCLIENTSOCKETEVENT) && (message < WM_ONCLIENTSOCKETEVENT + DEF_MAXCLIENTS)) 
			G_pGame->OnClientSocketEvent(message, wParam, lParam);

		return (DefWindowProc(hWnd, message, wParam, lParam));
	}
	
	return 0;
}

bool CALLBACK lpCrashDialogFunc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam){
HANDLE outHand;
char cCrashFileName[MAX_PATH];
char cLF[]={0x0d,0x0a};
char cDash ='-';
SYSTEMTIME sysTime;
DWORD written;

	switch(uMsg) {
	case WM_CLOSE:
		EndDialog(hDlg, true);
		break;

	case WM_COMMAND:
		switch(LOWORD(wParam)) {
		case IDC_CLOSE:
			EndDialog(hDlg, true);
			break;
		}
		break;

	case WM_INITDIALOG:
		//Show Crash Data
		SetWindowText(GetDlgItem(hDlg, IDC_EDIT1), G_cCrashTxt);
		GetLocalTime(&sysTime);
		wsprintf(cCrashFileName,"CrashData - %d-%d-%d.txt", sysTime.wDay, sysTime.wMonth, sysTime.wYear);
		SetWindowText(GetDlgItem(hDlg, IDC_EDITPATH), cCrashFileName);
		//Open File For Writing
		outHand = CreateFile(cCrashFileName,GENERIC_READ+GENERIC_WRITE,FILE_SHARE_READ+FILE_SHARE_WRITE,0,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
		SetFilePointer(outHand, 0, 0, FILE_END);
		WriteFile(outHand, G_cCrashTxt, strlen(G_cCrashTxt), &written, 0);
		for (int i = 0; i < 80; i++)
			WriteFile(outHand, &cDash, 1, &written, 0);
		WriteFile(outHand, cLF, 2, &written, 0);
		WriteFile(outHand, cLF, 2, &written, 0);
		CloseHandle(outHand);
		break;

	}
	return 0;
}

UINT32 lpTopLevelExceptionFilter(struct _EXCEPTION_POINTERS *ExceptionInfo){

	//Shutdown everything
	G_bIsThread = false;
	_StopTimer(G_mmTimer);

	try{
		delete G_pGame;
		G_pGame = 0;
	}
	catch (...) {
	}

	ZeroMemory(G_cCrashTxt, sizeof(G_cCrashTxt));

	//Format a nice output

	//Reason for crash
	strcpy(G_cCrashTxt, "HGServer Exception Information\r\n");
	strcat(G_cCrashTxt, "Code : ");
	wsprintf(&G_cCrashTxt[strlen(G_cCrashTxt)], "0x%.8X\r\n", ExceptionInfo->ExceptionRecord->ExceptionCode);
	strcat(G_cCrashTxt, "Flags : ");
	wsprintf(&G_cCrashTxt[strlen(G_cCrashTxt)], "0x%.8X\r\n", ExceptionInfo->ExceptionRecord->ExceptionFlags);
	strcat(G_cCrashTxt, "Address : ");
	wsprintf(&G_cCrashTxt[strlen(G_cCrashTxt)], "0x%.8X\r\n", ExceptionInfo->ExceptionRecord->ExceptionAddress);
	strcat(G_cCrashTxt, "Parameters : ");
	wsprintf(&G_cCrashTxt[strlen(G_cCrashTxt)], "0x%.8X\r\n\r\n", ExceptionInfo->ExceptionRecord->NumberParameters);

	
	strcat(G_cCrashTxt, "\r\n");

	//Crash Details
	strcat(G_cCrashTxt, "Context :\r\n");
	wsprintf(&G_cCrashTxt[strlen(G_cCrashTxt)], "EDI: 0x%.8X\t\tESI: 0x%.8X\t\tEAX: 0x%.8X\r\n",ExceptionInfo->ContextRecord->Edi,
																						ExceptionInfo->ContextRecord->Esi,
																						ExceptionInfo->ContextRecord->Eax);
	wsprintf(&G_cCrashTxt[strlen(G_cCrashTxt)], "EBX: 0x%.8X\t\tECX: 0x%.8X\t\tEDX: 0x%.8X\r\n",ExceptionInfo->ContextRecord->Ebx,
																						ExceptionInfo->ContextRecord->Ecx,
																						ExceptionInfo->ContextRecord->Edx);
	wsprintf(&G_cCrashTxt[strlen(G_cCrashTxt)], "EIP: 0x%.8X\t\tEBP: 0x%.8X\t\tSegCs: 0x%.8X\r\n",ExceptionInfo->ContextRecord->Eip,
																						ExceptionInfo->ContextRecord->Ebp,
																						ExceptionInfo->ContextRecord->SegCs);
	wsprintf(&G_cCrashTxt[strlen(G_cCrashTxt)], "EFlags: 0x%.8X\tESP: 0x%.8X\t\tSegSs: 0x%.8X\r\n",ExceptionInfo->ContextRecord->EFlags,
																						ExceptionInfo->ContextRecord->Esp,
																						ExceptionInfo->ContextRecord->SegSs);


		// SNOOPY: Tweacked Exeption handler to suit my needs
	HANDLE outHand;
	char cCrashFileName[MAX_PATH];
	char cLF[]={0x0d,0x0a};
	char cDash ='-';
	char cCrashTime[200];
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	ZeroMemory(cCrashTime, sizeof(cCrashTime));	
	wsprintf(cCrashTime, "%2d %2d - %2d:%2d:%2d",  (short)sysTime.wMonth, (short)sysTime.wDay, (short)sysTime.wHour, (short)sysTime.wMinute, (short)sysTime.wSecond);
	wsprintf(cCrashFileName,"CrashData-%2d-%2d-%2dh%2dm.txt", sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute);

	DWORD written;
	//Open File For Writing
	outHand = CreateFile(cCrashFileName,GENERIC_READ+GENERIC_WRITE,FILE_SHARE_READ+FILE_SHARE_WRITE,0,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
	SetFilePointer(outHand, 0, 0, FILE_END);
	// Put Time
	WriteFile(outHand, cCrashTime, strlen(cCrashTime), &written, 0);	
	// Put LF
	WriteFile(outHand, cLF, 2, &written, 0);	
	// Put CrashText
	WriteFile(outHand, G_cCrashTxt, strlen(G_cCrashTxt), &written, 0);
	// Put ----------------------------------
	for (int i = 0; i < 80; i++) WriteFile(outHand, &cDash, 1, &written, 0);
	WriteFile(outHand, cLF, 2, &written, 0);
	CloseHandle(outHand);

	
	return EXCEPTION_EXECUTE_HANDLER;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	// Install SEH
	SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)lpTopLevelExceptionFilter);
	wsprintf( szAppClass, "Game-Server%d", (int)hInstance);
	if (!InitApplication( hInstance))		return (false);
    if (!InitInstance(hInstance, nCmdShow)) return (false);
	
	Initialize();
	EventLoop();
    return 0;
}

bool InitApplication( HINSTANCE hInstance)
{     
 WNDCLASS  wc;

	
	wc.style = (CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS);
	wc.lpfnWndProc   = (WNDPROC)WndProc;             
	wc.cbClsExtra    = 0;                            
	wc.cbWndExtra    = sizeof (int);             
	wc.hInstance     = hInstance;                    
	wc.hIcon         = 0;
	wc.hCursor       = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName  = 0;                    		 
	wc.lpszClassName = szAppClass; 


        
	return (RegisterClass(&wc));
}


bool InitInstance( HINSTANCE hInstance, int nCmdShow )
{
 char cTitle[100];
 SYSTEMTIME SysTime;
 
	// 서버 부팅시간 기록 
	
	

	GetLocalTime(&SysTime);
	wsprintf(cTitle, "Helbreath Game-Server V%s.%s (Executed at: %d/%d/%d - %d:%d)", DEF_UPPERVERSION, DEF_LOWERVERSION, SysTime.wDay, SysTime.wMonth, SysTime.wYear, SysTime.wHour, SysTime.wMinute);

	G_hWnd = CreateWindowEx(0,  
        szAppClass,
        cTitle,
        WS_VISIBLE | // so we don't have to call ShowWindow
        WS_POPUP |   // non-app window
        WS_CAPTION | // so our menu doesn't look ultra-goofy
        WS_SYSMENU |  // so we get an icon in the tray
        WS_MINIMIZEBOX, 
		CW_USEDEFAULT,
        CW_USEDEFAULT,
        800, 
        600, 
        0,
        0,
        hInstance,
        0);

	
//MAJORHG - CLEROTH - 26/03/05
    WNDCLASS  Wc;
	strcpy(BCX_ClassName,"ListBox1");
    BCX_ScaleX       = 1;
    BCX_ScaleY       = 1;
    BCX_hInstance    =  hInstance;
    Wc.style         =  CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    Wc.lpfnWndProc   =  WndProc;
    Wc.cbClsExtra    =  0;
    Wc.cbWndExtra    =  0;
    Wc.hInstance     =  hInstance;
    Wc.hIcon         =  LoadIcon(0,IDI_WINLOGO);
    Wc.hCursor       =  LoadCursor(0,IDC_ARROW);
    Wc.hbrBackground =  (HBRUSH)(COLOR_BTNFACE+4);
    Wc.lpszMenuName  =  0;
    Wc.lpszClassName =  BCX_ClassName;
    RegisterClass(&Wc);

	List1 = BCX_Listbox("", G_hWnd, 1009, 8, 45, 585, 300);
	Edit1 = BCX_Editbox("", G_hWnd, 1011, 8, 345, 585, 20);
	Button2 = BCX_Button2("", G_hWnd, 1012, 8, 369, 300, 20);
	//    Button Clear
	Button = BCX_Button("", G_hWnd, 1010, 310, 369, 285, 20);

    if (!G_hWnd) return (false);
    
	ShowWindow(G_hWnd, nCmdShow);    
	UpdateWindow(G_hWnd);            

	return (true);                 
}



int EventLoop()
{
 MSG msg;

	while( 1 ) {
		if( PeekMessage( &msg, 0, 0, 0, PM_NOREMOVE ) ) {
			if( !GetMessage( &msg, 0, 0, 0 ) ) {
				return msg.wParam;
			}
            TranslateMessage(&msg);
            DispatchMessage(&msg);
			UpdateScreen();
		}
		else WaitMessage();
	}
}

void Initialize()
{
	if (_InitWinsock() == false) {
		MessageBox(G_hWnd, "Socket 1.1 not found! Cannot execute program.","ERROR", MB_ICONEXCLAMATION | MB_OK);
		PostQuitMessage(0);
		return;
	}
	G_pGame = new class CGame(G_hWnd);
	if (G_pGame->bInit() == false) {
		PutLogList("(!!!) STOPPED!");
		return;
	}
	G_mmTimer = _StartTimer(50); // centu - 300 -> 50
	G_pListenSock = new class XSocket(G_hWnd, DEF_SERVERSOCKETBLOCKLIMIT);
	if (G_pGame->m_iGameServerMode == 1)
	{
		G_pListenSock->bListen(G_pGame->m_cGameServerAddrInternal, G_pGame->m_iGameServerPort, WM_USER_ACCEPT);
	}
	else if (G_pGame->m_iGameServerMode == 2)
	{
		G_pListenSock->bListen(G_pGame->m_cGameServerAddr, G_pGame->m_iGameServerPort, WM_USER_ACCEPT);
	}
	pLogFile = 0;
}

void OnDestroy()
{
	if (G_pListenSock != 0) delete G_pListenSock;
	if (G_pLogSock != 0) delete G_pLogSock;

	if (G_pGame != 0) {
		G_pGame->Quit();
		delete G_pGame;
	}

	if (G_mmTimer != 0) _StopTimer(G_mmTimer);
	
	WSACleanup();
	if (pLogFile != 0) fclose(pLogFile);

	PostQuitMessage(0);
}


void PutLogList(char * cMsg)
{
	
	G_cMsgUpdated = true;
	

	SendMessage(List1,(UINT)LB_ADDSTRING,(WPARAM)0,(LPARAM)cMsg);
	SendMessage(List1,(UINT)LB_SETCURSEL,ItemCount,0);
	ItemCount++;
}

void PutXSocketLogList(char * cMsg)
{

	FILE* pFile;
	char cBuffer[512];
	SYSTEMTIME SysTime;

	pFile = fopen("GameLogs\\XSocket.log", "at");
	if (pFile == 0) return;

	ZeroMemory(cBuffer, sizeof(cBuffer));

	GetLocalTime(&SysTime);
	wsprintf(cBuffer, "(%2d/%2d/%4d - %2d:%2d:%2d) - ", SysTime.wDay, SysTime.wMonth, SysTime.wYear, SysTime.wHour, SysTime.wMinute, SysTime.wSecond);
	strcat(cBuffer, cMsg);
	strcat(cBuffer, "\n");

	fwrite(cBuffer, 1, strlen(cBuffer), pFile);
	fclose(pFile);

}

void UpdateScreen()
{
	if (G_cMsgUpdated == true) {
		InvalidateRect(G_hWnd, 0, true);
		G_cMsgUpdated = false;
	}
}


void OnPaint()
{
 HDC hdc;
 PAINTSTRUCT ps;
short i;
 char * cMsg;

	hdc = BeginPaint(G_hWnd, &ps);
	
	SetBkMode(hdc,TRANSPARENT);

	for (i = 0; i < 20; i++) {
		cMsg = (char *)(G_cMsgList + i*120);
		TextOut(hdc, 5, 5 + 350 - i*16, cMsg, strlen(cMsg));
	}
	
	if (G_pGame	!= 0)
		G_pGame->DisplayInfo(hdc);

	EndPaint(G_hWnd, &ps);
}



void OnAccept()
{
	G_pGame->bAccept(G_pListenSock);
}

void CALLBACK _TimerFunc(UINT wID, UINT wUser, DWORD dwUSer, DWORD dw1, DWORD dw2)
{
	PostMessage(G_hWnd, WM_USER_TIMERSIGNAL, wID, 0);
}



MMRESULT _StartTimer(UINT32 dwTime)
{
 TIMECAPS caps;
 MMRESULT timerid;

	timeGetDevCaps(&caps, sizeof(caps));
	timeBeginPeriod(caps.wPeriodMin);
	timerid = timeSetEvent(dwTime,0,_TimerFunc,0, (UINT)TIME_PERIODIC);

	return timerid;
}



void _StopTimer(MMRESULT timerid)
{
 TIMECAPS caps;

	if (timerid != 0) {
		timeKillEvent(timerid);
		timerid = 0;
		timeGetDevCaps(&caps, sizeof(caps));
		timeEndPeriod(caps.wPeriodMin);
	}
}

/*********************************************************************************************************************
**  void PutLogFileList(char * cStr)																				**
**  description			:: writes data into "Events.log"															**
**  last updated		:: November 22, 2004; 5:40 PM; Hypnotoad													**
**	return value		:: void																						**
**********************************************************************************************************************/
void PutLogFileList(char * cStr)
{
 FILE * pFile;
 char cBuffer[512];
 SYSTEMTIME SysTime;
	
	
	pFile = fopen("GameLogs\\Events.log", "at");
	if (pFile == 0) return;
	ZeroMemory(cBuffer, sizeof(cBuffer));
	GetLocalTime(&SysTime);
	wsprintf(cBuffer, "(%2d/%2d/%4d - %2d:%2d:%2d) - ", SysTime.wDay, SysTime.wMonth, SysTime.wYear, SysTime.wHour, SysTime.wMinute, SysTime.wSecond);
	strcat(cBuffer, cStr);
	strcat(cBuffer, "\n");
	fwrite(cBuffer, 1, strlen(cBuffer), pFile);
	fclose(pFile);
}

void PutAdminLogFileList(char * cStr)
{
 FILE * pFile;
 char cBuffer[512];
 SYSTEMTIME SysTime;
	
	pFile = fopen("GameLogs\\AdminEvents.log", "at");
	if (pFile == 0) return;

	ZeroMemory(cBuffer, sizeof(cBuffer));
	
	GetLocalTime(&SysTime);
	wsprintf(cBuffer, "(%2d/%2d/%4d - %2d:%2d:%2d) - ", SysTime.wDay, SysTime.wMonth, SysTime.wYear, SysTime.wHour, SysTime.wMinute, SysTime.wSecond);
	strcat(cBuffer, cStr);
	strcat(cBuffer, "\n");

	fwrite(cBuffer, 1, strlen(cBuffer), pFile);
	fclose(pFile);
}

void PutHackLogFileList(char * cStr)
{
 FILE * pFile;
 char cBuffer[512];
 SYSTEMTIME SysTime;
	
	pFile = fopen("GameLogs\\HackEvents.log", "at");
	if (pFile == 0) return;

	ZeroMemory(cBuffer, sizeof(cBuffer));
	
	GetLocalTime(&SysTime);
	wsprintf(cBuffer, "(%2d/%2d/%4d - %2d:%2d:%2d) - ", SysTime.wDay, SysTime.wMonth, SysTime.wYear, SysTime.wHour, SysTime.wMinute, SysTime.wSecond);
	strcat(cBuffer, cStr);
	strcat(cBuffer, "\n");

	fwrite(cBuffer, 1, strlen(cBuffer), pFile);
	fclose(pFile);
}

void PutPvPLogFileList(char * cStr)
{
 FILE * pFile;
 char cBuffer[512];
 SYSTEMTIME SysTime;
	
	pFile = fopen("GameLogs\\PvPEvents.log", "at");
	if (pFile == 0) return;

	ZeroMemory(cBuffer, sizeof(cBuffer));
	
	GetLocalTime(&SysTime);
	wsprintf(cBuffer, "(%2d/%2d/%4d - %2d:%2d:%2d) - ", SysTime.wDay, SysTime.wMonth, SysTime.wYear, SysTime.wHour, SysTime.wMinute, SysTime.wSecond);
	strcat(cBuffer, cStr);
	strcat(cBuffer, "\n");

	fwrite(cBuffer, 1, strlen(cBuffer), pFile);
	fclose(pFile);
}


void PutItemLogFileList(char * cStr)
{
 FILE * pFile;
 char cBuffer[512];
 SYSTEMTIME SysTime;
	
	pFile = fopen("GameLogs\\ItemEvents.log", "at");
	if (pFile == 0) return;

	ZeroMemory(cBuffer, sizeof(cBuffer));
	
	GetLocalTime(&SysTime);
	wsprintf(cBuffer, "(%2d/%2d/%4d - %2d:%2d:%2d) - ", SysTime.wDay, SysTime.wMonth, SysTime.wYear, SysTime.wHour, SysTime.wMinute, SysTime.wSecond);
	strcat(cBuffer, cStr);
	strcat(cBuffer, "\n");

	fwrite(cBuffer, 1, strlen(cBuffer), pFile);
	fclose(pFile);
}

void PutLogEventFileList(char * cStr)
{
 FILE * pFile;
 char cBuffer[512];
 SYSTEMTIME SysTime;
	
	pFile = fopen("GameLogs\\LogEvents.log", "at");
	if (pFile == 0) return;

	ZeroMemory(cBuffer, sizeof(cBuffer));
	
	GetLocalTime(&SysTime);
	wsprintf(cBuffer, "(%2d/%2d/%4d - %2d:%2d:%2d) - ", SysTime.wDay, SysTime.wMonth, SysTime.wYear, SysTime.wHour, SysTime.wMinute, SysTime.wSecond);
	strcat(cBuffer, cStr);
	strcat(cBuffer, "\n");

	fwrite(cBuffer, 1, strlen(cBuffer), pFile);
	fclose(pFile);
}


//MAJOR - CLEROTH - 26/03/05
HWND BCX_Listbox(char* Text,HWND hWnd,int id,int X,int Y,int W,int H,int Style,int Exstyle)
{
        HWND  A;
        if (!Style)
        {
			Style = WS_VSCROLL | WS_VISIBLE | WS_CHILD | LBS_HASSTRINGS | LBS_NOINTEGRALHEIGHT | LBS_NOTIFY | WS_TABSTOP;
        }
        if (Exstyle == -1)
        {
			Exstyle=WS_EX_CLIENTEDGE;
        }
        A = CreateWindowEx(Exstyle,"Listbox",0,Style,X*BCX_ScaleX, Y*BCX_ScaleY, W*BCX_ScaleX, H*BCX_ScaleY,hWnd,(HMENU)id,BCX_hInstance,0);
        SendMessage(A,(UINT)WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),(LPARAM)MAKELPARAM(false,0));
        return A;
}
//MAJOR - CLEROTH - 26/03/05
HWND BCX_Editbox(char* Text,HWND hWnd,int id,int X,int Y,int W,int H,int Style,int Exstyle)
{
        HWND  A;
        if (!Style)
        {
			Style = WS_VISIBLE | WS_CHILD | LBS_HASSTRINGS | LBS_NOINTEGRALHEIGHT | WS_TABSTOP;
        }
        if (Exstyle == -1)
        {
			Exstyle=WS_EX_CLIENTEDGE;
        }
        A = CreateWindowEx(Exstyle,"EDIT",0,Style,X*BCX_ScaleX, Y*BCX_ScaleY, W*BCX_ScaleX, H*BCX_ScaleY,hWnd,(HMENU)id,BCX_hInstance,0);
        SendMessage(A,(UINT)WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),(LPARAM)MAKELPARAM(false,0));
        return A;
}

//MAJOR - CLEROTH - 26/03/05
HWND BCX_Button(char* Text,HWND hWnd,int id,int X,int Y,int W,int H,int Style,int Exstyle)
{
        HWND  A;
        if (!Style)
        {
			Style = WS_CHILD | WS_VISIBLE | WS_BORDER;
        }
        A = CreateWindowEx(0,"Button","Clear",Style,X*BCX_ScaleX, Y*BCX_ScaleY, W*BCX_ScaleX, H*BCX_ScaleY,hWnd,(HMENU)id,BCX_hInstance,0);
        SendMessage(A,(UINT)WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),(LPARAM)MAKELPARAM(false,0));
        return A;
}
HWND BCX_Button2(char* Text,HWND hWnd,int id,int X,int Y,int W,int H,int Style,int Exstyle)
{
        HWND  A;
        if (!Style)
        {
			Style = WS_CHILD | WS_VISIBLE | WS_BORDER;
        }
        A = CreateWindowEx(0,"Button","Send",Style,X*BCX_ScaleX, Y*BCX_ScaleY, W*BCX_ScaleX, H*BCX_ScaleY,hWnd,(HMENU)id,BCX_hInstance,0);
        SendMessage(A,(UINT)WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),(LPARAM)MAKELPARAM(false,0));
        return A;
}//Button CLear

void TextOut2()
{}

void PutLogOnline(char* cStr)
{
	FILE* pFile;

	pFile = fopen("GameLogs\\LogOnline.log", "wt");
	if (pFile == 0) return;

	fwrite(cStr, 1, strlen(cStr), pFile);
	fclose(pFile);
}