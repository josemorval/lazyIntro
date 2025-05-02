#include "common_includes.h"

#ifndef _DEBUG
void WinMainCRTStartup()
{
	ShowCursor(0);
	window_handler = CreateWindowExA(0, (LPCSTR)0xC018, 0, WS_POPUP | WS_VISIBLE | WS_MAXIMIZE, 0, 0, 0, 0, 0, 0, 0, 0);
    RECT rect;
    if (GetWindowRect(window_handler, &rect)) {
        width_window = rect.right - rect.left;
        height_window = rect.bottom - rect.top;
    }
#else

LARGE_INTEGER frequency; 
LARGE_INTEGER prevTime; 
int frameCount = 0; 
double elapsedTime = 0.0; 

void IntToString(int value, char* buffer) {
	char* p = buffer;
	int temp = value;
	int digits = 0;

	do {
		digits++;
		temp /= 10;
	} while (temp);

	p += digits;
	*p = '\0'; 

	do {
		*--p = '0' + (value % 10); 
		value /= 10;
	} while (value);
}

void InitTimer() {
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&prevTime); 
}

void CalculateFPS() {
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);

	double deltaTime = static_cast<double>(currentTime.QuadPart - prevTime.QuadPart) / frequency.QuadPart;
	prevTime = currentTime; 

	if (elapsedTime >= 1.0) { 
		char title[32] = "lazy[I]ntro [ ";
		IntToString(frameCount, title + 14);
		title[16] = ' ';
		title[17] = 'F';
		title[18] = 'P';
		title[19] = 'S';
		title[20] = ' ';
		title[21] = ']';

		SetWindowText(window_handler, title);

		frameCount = 0;    // Reinicia el contador
		elapsedTime = 0.0; // Reinicia el tiempo acumulado
	}
	else 
	{
		frameCount++;           // Incrementa el contador de frames
		elapsedTime += deltaTime;  // Acumula el tiempo transcurrido
	}
}

int WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nFunsterStil)
{
	AllocConsole();
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD dwMode = 0;
	GetConsoleMode(hOut, &dwMode);
	SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
	freopen("CONOUT$", "w", stdout);

	HICON hIcon = (HICON)LoadImage(NULL, "app.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	WNDCLASS wc = {};
	wc.hInstance = hThisInstance;
	wc.lpszClassName = "custom_class";
	wc.lpfnWndProc = DefWindowProc;
	wc.hIcon = hIcon;
	RegisterClass(&wc);

	window_handler = CreateWindowA("custom_class", "lazy[I]ntro", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 200, 200, width_window, height_window, 0, 0, hThisInstance, 0);

	SetWindowLong(window_handler, GWL_STYLE, GetWindowLong(window_handler, GWL_STYLE) & ~WS_THICKFRAME);
	SetWindowPos(window_handler, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

	ShowCursor(true);

	printf(BOLD WHITE "===" BG_BLUE RED "DEBUG lazyIntro" WHITE BG_BLACK "=== \n" RESET);
#endif

    device_creation();
	ini_3d();
	preparation();

#ifdef _DEBUG

	InitTimer();
	MSG msg = { 0 };
	while (true) {
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			// Detectar si se presiona la tecla ESCAPE
			if (msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE) {
				int result = system("devenv LazyIntro.sln /Build Debug");

				ExitProcess(0);
			}
		}

		CalculateFPS();
		loop();
		swapchain->Present(1, 0);
	}


	ExitProcess(0);

#else
	while (!GetAsyncKeyState(VK_ESCAPE))
	{
		PeekMessageA(0, 0, 0, 0, PM_REMOVE);
		loop();
		swapchain->Present(1, 0);
	};

	ExitProcess(0);
#endif

}
