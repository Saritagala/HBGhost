// winmain.h

bool InitApplication( HINSTANCE hInstance);
bool InitInstance( HINSTANCE hInstance, int nCmdShow );
LRESULT CALLBACK WndProc( HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam );
void EventLoop();
void OnDestroy();
void Initialize(char * pCmdLine);
void CALLBACK _TimerFunc(UINT wID, UINT wUser, DWORD dwUSer, DWORD dw1, DWORD dw2);
MMRESULT _StartTimer(DWORD dwTime);

void _StopTimer(MMRESULT timerid);
void CPSocketConnet() ;

bool IsWin();