#include <d3d11.h>
#include <d3dcompiler.h>

#include "shader_helper.h"

#ifdef _DEBUG
	#include <stdio.h>
	#include <windows.h>
	
	//Some definitions for console
	#define RESET   "\033[0m"
	#define BLACK   "\033[1;30m"
	#define RED     "\033[1;31m"
	#define GREEN   "\033[1;32m"
	#define YELLOW  "\033[1;33m"
	#define BLUE    "\033[1;34m"
	#define MAGENTA "\033[1;35m"
	#define CYAN    "\033[1;36m"
	#define WHITE   "\033[1;37m"
	#define BOLD    "\033[1m"

	// Colores de fondo básicos (ANSI 16 colores)
	#define BG_BLACK   "\033[40m"
	#define BG_RED     "\033[41m"
	#define BG_GREEN   "\033[42m"
	#define BG_YELLOW  "\033[43m"
	#define BG_BLUE    "\033[44m"
	#define BG_MAGENTA "\033[45m"
	#define BG_CYAN    "\033[46m"
	#define BG_WHITE   "\033[47m"
#endif

#ifndef _DEBUG
    #include "generated_shader.h"
#endif

#include "libmem.h"
#include "libdevice.h"
#include "lib3d.h"
#include "logic.h"

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
int WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nFunsterStil)
{
	AllocConsole();
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD dwMode = 0;
	GetConsoleMode(hOut, &dwMode);
	SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
	freopen("CONOUT$", "w", stdout);

	window_handler = CreateWindowA("edit", 0, WS_POPUP | WS_VISIBLE, 0, 0, width_window, height_window, 0, 0, 0, 0);
	printf(BOLD WHITE "===" BG_BLUE RED "DEBUG lazyIntro" WHITE BG_BLACK "=== \n" RESET);
#endif

    device_creation();
	preparation();

	do
	{
		PeekMessageA(0, 0, 0, 0, PM_REMOVE);
		loop();
		swapchain->Present(1, 0);
	}

	while (!GetAsyncKeyState(VK_ESCAPE));
	ExitProcess(0);
}
