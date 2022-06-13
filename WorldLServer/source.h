#include "XSocket.h"
#include "NetMessages.h"

LRESULT CALLBACK WindowProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
bool InitApplication( HINSTANCE hInstance);
bool InitInstance( HINSTANCE hInstance, int nCmdShow );
int EventLoop();
void Initialize();
void UpdateScreen();
void OnDestroy();
void OnPaint();
void PutLogList(char * cMsg);
void PutLogServ(char * cMsg);
void CALLBACK _TimerFunc(UINT wID, UINT wUser, DWORD dwUSer, DWORD dw1, DWORD dw2);
MMRESULT _StartTimer(DWORD dwTime);
void _StopTimer(MMRESULT timerid);
void OnAccept();
void PutGameLogData(char * cMsg);
void PutEventLog(char * cMsg);
void OnKeyUp(WPARAM wParam, LPARAM lParam);

//Global Defs'
bool G_bIsMainLogActive;
char G_cData50000[50000];

//Socket to listen for clients!
class XSocket * G_pXWorldLogSock=NULL;