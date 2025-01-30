#include <d3d11.h>
#include <d3dcompiler.h>

#include "vertex_input_layout_header.h"

#ifdef _DEBUG
	#include <stdio.h>
	#include <windows.h>
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
	freopen("CONOUT$", "w", stdout);
	window_handler = CreateWindowA("edit", 0, WS_POPUP | WS_VISIBLE, 0, 0, width_window, height_window, 0, 0, 0, 0);
#endif

    device_creation();
	preparation();

	do
	{
		PeekMessageA(0, 0, 0, 0, PM_REMOVE);
		if (GetAsyncKeyState('R') & 0x8000){}
		loop();
		swapchain->Present(1, 0);
	}

	while (!GetAsyncKeyState(VK_ESCAPE));
	ExitProcess(0);
}
