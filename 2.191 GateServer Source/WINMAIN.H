// winmain.h
#define	 _MT
#include <process.h>

bool InitApplication( HINSTANCE hInstance);
bool InitInstance( HINSTANCE hInstance, int nCmdShow );
LRESULT CALLBACK WndProc( HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam );
void OnAccept();
void PutLogList(char * cMsg);
void OnKeyUp(WPARAM wParam, LPARAM lParam);
void UpdateScreen();
int  EventLoop();
void OnPaint();
void OnDestroy();
void Initialize();
void CALLBACK _TimerFunc(UINT wID, UINT wUser, DWORD dwUSer, DWORD dw1, DWORD dw2);
MMRESULT _StartTimer(UINT32 dwTime);
void _StopTimer(MMRESULT timerid);
unsigned __stdcall  ThreadProc(void *ch);
void PutGameServerList(char * cStr) ;

void PutLogFileList(char * cStr);


