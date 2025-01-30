#ifndef LIBDEVICE_HEADER_H
#define LIBDEVICE_HEADER_H

static int width_window = 1000;
static int height_window = 800;
static int swapchain_desc[] = { width_window, height_window, 0, 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 0, 1, 0, DXGI_USAGE_RENDER_TARGET_OUTPUT, 1, 0, 1, 0, 0 };

HWND window_handler;
ID3D11Device* device;
ID3D11DeviceContext* inmediate;
IDXGISwapChain* swapchain;

HRESULT hr = NULL;
ID3D10Blob* error_blob = NULL;
ID3D10Blob* blob = NULL;

void device_creation()
{
	swapchain_desc[0] = width_window;
	swapchain_desc[1] = height_window;
	swapchain_desc[11] = (int)window_handler;
	D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, 0, 0, D3D11_SDK_VERSION, (DXGI_SWAP_CHAIN_DESC*)&swapchain_desc[0], &swapchain, &device, NULL, &inmediate);
}

#endif