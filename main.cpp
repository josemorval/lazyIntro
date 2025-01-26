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

static int width_window = 1000;
static int height_window = 800;
static int frame = 0;

static int SwapChainDesc[] = { width_window, height_window, 0, 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 0, 1, 0, DXGI_USAGE_RENDER_TARGET_OUTPUT, 1, 0, 1, 0, 0 };
static D3D11_BUFFER_DESC ConstBufferDesc = { 256, D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER, 0, 0, 0 }; // 256,0,4,0,0,0

ID3D11Device* pd3dDevice;
ID3D11DeviceContext* pImmediateContext;
IDXGISwapChain* pSwapChain;

HRESULT hr = NULL;
ID3D10Blob* pErrorBlob = NULL;
ID3D10Blob* pBlob = NULL;

//Main variables
ID3D11ShaderResourceView* null_srv;
ID3D11UnorderedAccessView* null_uav;
ID3D11RenderTargetView* null_rtv;
ID3D11Buffer* null_buffer;
ID3D11InputLayout* pCommonVertexLayout = NULL;
UINT uCommonVertexLayoutStride = 0;
ID3D11Buffer* pConstants;

static char static_memory[32 * 1024 * 1024];
static unsigned int static_memory_pointer = 0;

float quad_vertices[] = {
        -0.5f,  0.5f, 0.0f, 0.0f, 0.0f , 0.0f, 1.0f, 0.0f,
        0.5f,  0.5f, 0.0f, 1.0f, 0.0f , 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f , 0.0f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.0f, 1.0f, 1.0f , 0.0f, 1.0f, 0.0f
};
int quad_indices[] = {
    0, 1, 2,
    2, 1, 3
};
float cube_vertices[] = {
    -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
    0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
    0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f,

    -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
    0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,

    -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,

    0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
    0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
    0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f
};
int cube_indices[] = {
0, 2, 1, 1, 2, 3,
4, 5, 6, 6, 5, 7,
9, 8, 10, 9, 10, 11,
12, 13, 14, 14, 13, 15,
17, 16, 18, 17, 18, 19,
20, 21, 22, 22, 21, 23
};

struct Mesh {

    ID3D11Buffer* vertex_buffer = nullptr;
    ID3D11Buffer* index_buffer = nullptr;

    ID3D11ShaderResourceView* vertex_srv = nullptr;
    ID3D11ShaderResourceView* index_srv = nullptr;

    int vertices_count = 0;
    int indices_count = 0;

    unsigned int stride;
    unsigned int offset;

    Mesh(float* _vertices, int* _indices, int _vertices_size, int _indices_size)
    {
        D3D11_BUFFER_DESC vertexBufferDesc = {};
        vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        vertexBufferDesc.ByteWidth = _vertices_size;
        vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_SHADER_RESOURCE;

        D3D11_SUBRESOURCE_DATA initData = {};
        initData.pSysMem = _vertices;
        pd3dDevice->CreateBuffer(&vertexBufferDesc, &initData, &vertex_buffer);

        D3D11_BUFFER_DESC indexBufferDesc = {};
        indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        indexBufferDesc.ByteWidth = _indices_size;
        indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER | D3D11_BIND_SHADER_RESOURCE;
        indexBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;

        D3D11_SUBRESOURCE_DATA indexData = {};
        indexData.pSysMem = _indices;
        pd3dDevice->CreateBuffer(&indexBufferDesc, &indexData, &index_buffer);

        vertices_count = _vertices_size / sizeof(float);
        indices_count = _indices_size / sizeof(int);

        stride = 8 * sizeof(float);
        offset = 0;

        D3D11_SHADER_RESOURCE_VIEW_DESC vertexSRVDesc = {};
        vertexSRVDesc.Format = DXGI_FORMAT_R32_FLOAT; // Ajusta el formato según tus necesidades
        vertexSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
        vertexSRVDesc.Buffer.ElementOffset = 0;
        vertexSRVDesc.Buffer.ElementWidth = _vertices_size / sizeof(float);

        pd3dDevice->CreateShaderResourceView(vertex_buffer, &vertexSRVDesc, &vertex_srv);

        // Crear SRV para el búfer de índices
        D3D11_SHADER_RESOURCE_VIEW_DESC indexSRVDesc = {};
        indexSRVDesc.Format = DXGI_FORMAT_R32_UINT; // Ajusta el formato según tus necesidades
        indexSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
        indexSRVDesc.Buffer.ElementOffset = 0;
        indexSRVDesc.Buffer.ElementWidth = _indices_size / sizeof(int);
        pd3dDevice->CreateShaderResourceView(index_buffer, &indexSRVDesc, &index_srv);
    }

    void attach_vertices_srv(int _slot)
    {
        pImmediateContext->VSSetShaderResources(_slot, 1, &vertex_srv);
        pImmediateContext->GSSetShaderResources(_slot, 1, &vertex_srv);
        pImmediateContext->PSSetShaderResources(_slot, 1, &vertex_srv);
        pImmediateContext->CSSetShaderResources(_slot, 1, &vertex_srv);
    }

    void attach_indices_srv(int _slot)
    {
        pImmediateContext->VSSetShaderResources(_slot, 1, &index_srv);
        pImmediateContext->GSSetShaderResources(_slot, 1, &index_srv);
        pImmediateContext->PSSetShaderResources(_slot, 1, &index_srv);
        pImmediateContext->CSSetShaderResources(_slot, 1, &index_srv);
    }

    void use()
    {
        pImmediateContext->IASetInputLayout(pCommonVertexLayout);
        pImmediateContext->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);
        pImmediateContext->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);
        pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    }

    void draw()
    {
        pImmediateContext->DrawIndexed(indices_count, 0, 0);
    }

    void draw_instanced(int instances_count)
    {
        pImmediateContext->DrawIndexedInstanced(indices_count, instances_count, 0, 0, 0);
    }

    void release()
    {
        vertex_buffer->Release();
        vertex_buffer = nullptr;
        index_buffer->Release();
        index_buffer = nullptr;
        vertex_srv->Release();
        vertex_srv = nullptr;
        index_srv->Release();
        index_srv = nullptr;
    }
};
struct RenderDepth2D
{
    ID3D11Texture2D* rendertarget;
    ID3D11DepthStencilView* dsv;
    ID3D11ShaderResourceView* srv;

    RenderDepth2D(int _width, int _height)
    {
        D3D11_TEXTURE2D_DESC depthDesc = {};
        depthDesc.Width = _width;
        depthDesc.Height = _height;
        depthDesc.MipLevels = 0;
        depthDesc.ArraySize = 1;
        depthDesc.Format = DXGI_FORMAT_R32_TYPELESS;
        depthDesc.SampleDesc.Count = 1;
        depthDesc.Usage = D3D11_USAGE_DEFAULT;
        depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

        pd3dDevice->CreateTexture2D(&depthDesc, nullptr, &rendertarget);

        D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
        dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
        dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        pd3dDevice->CreateDepthStencilView(rendertarget, &dsvDesc, &dsv);

        D3D11_SHADER_RESOURCE_VIEW_DESC srvdesc = {};
        srvdesc.Format = DXGI_FORMAT_R32_FLOAT;
        srvdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvdesc.Texture2D.MipLevels = 1;
        pd3dDevice->CreateShaderResourceView(rendertarget, &srvdesc, &srv);

    }

    void attach_srv(int _slot)
    {
        pImmediateContext->VSSetShaderResources(_slot, 1, &srv);
        pImmediateContext->GSSetShaderResources(_slot, 1, &srv);
        pImmediateContext->PSSetShaderResources(_slot, 1, &srv);
        pImmediateContext->CSSetShaderResources(_slot, 1, &srv);
    }

    void set_depth()
    {
        pImmediateContext->OMSetRenderTargets(0, nullptr, dsv);
    }

    void clear_depth()
    {
        pImmediateContext->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH, 1.0, 0);
    }

    void clear_depth_zero()
    {
        pImmediateContext->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH, 0.0, 0);
    }

    void release()
    {
        dsv->Release();
        dsv = nullptr;
        srv->Release();
        srv = nullptr;
        rendertarget->Release();
        rendertarget = nullptr;
    }

};
struct RenderTarget2D
{
    ID3D11Texture2D* rendertarget;
    ID3D11RenderTargetView* rtv;
    ID3D11ShaderResourceView* srv;
    ID3D11UnorderedAccessView* uav;

    RenderTarget2D() {}

    RenderTarget2D(int _width, int _height)
    {
        D3D11_TEXTURE2D_DESC textureDesc = {};
        textureDesc.Width = _width;
        textureDesc.Height = _height;
        textureDesc.MipLevels = 0;
        textureDesc.ArraySize = 1;
        textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; // Formato de píxeles
        textureDesc.SampleDesc.Count = 1;
        textureDesc.SampleDesc.Quality = 0;
        textureDesc.Usage = D3D11_USAGE_DEFAULT;
        textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
        textureDesc.CPUAccessFlags = 0;
        textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;


        pd3dDevice->CreateTexture2D(&textureDesc, nullptr, &rendertarget);
        pd3dDevice->CreateRenderTargetView(rendertarget, nullptr, &rtv);
        pd3dDevice->CreateShaderResourceView(rendertarget, nullptr, &srv);
        pd3dDevice->CreateUnorderedAccessView(rendertarget, nullptr, &uav);
    }

    void get_backbuffer()
    {
        pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&rendertarget);
        pd3dDevice->CreateRenderTargetView(rendertarget, nullptr, &rtv);
        pd3dDevice->CreateShaderResourceView(rendertarget, nullptr, &srv);
        pd3dDevice->CreateUnorderedAccessView(rendertarget, nullptr, &uav);
    }

    void attach_srv(int _slot)
    {
        pImmediateContext->VSSetShaderResources(_slot, 1, &srv);
        pImmediateContext->GSSetShaderResources(_slot, 1, &srv);
        pImmediateContext->PSSetShaderResources(_slot, 1, &srv);
        pImmediateContext->CSSetShaderResources(_slot, 1, &srv);
    }

    void attach_uav(int _slot)
    {
        pImmediateContext->CSSetUnorderedAccessViews(_slot, 1, &uav, nullptr);
    }

    void set_rendertarget()
    {
        pImmediateContext->OMSetRenderTargets(1, &rtv, nullptr);
    }

    void set_rendertarget_and_depth(RenderDepth2D _depth2d)
    {
        pImmediateContext->OMSetRenderTargets(1, &rtv, _depth2d.dsv);
    }

    void clear_rendertarget(float _clean_color[4])
    {
        pImmediateContext->ClearRenderTargetView(rtv, _clean_color);
    }

    void copy_resource(RenderTarget2D _rendertarget2d)
    {
        D3D11_TEXTURE2D_DESC textureDesc = {};
        _rendertarget2d.rendertarget->GetDesc(&textureDesc);
        pd3dDevice->CreateTexture2D(&textureDesc, nullptr, &rendertarget);
        pd3dDevice->CreateRenderTargetView(rendertarget, nullptr, &rtv);
        pd3dDevice->CreateShaderResourceView(rendertarget, nullptr, &srv);
        pd3dDevice->CreateUnorderedAccessView(rendertarget, nullptr, &uav);
        pImmediateContext->CopyResource(rendertarget, _rendertarget2d.rendertarget);
    }

    void release()
    {
        rtv->Release();
        rtv = nullptr;
        srv->Release();
        srv = nullptr;
        uav->Release();
        uav = nullptr;
        rendertarget->Release();
        rendertarget = nullptr;
    }

    void generate_mipmaps()
    {
        pImmediateContext->GenerateMips(srv);
    }
};
struct RenderTarget3D
{
    ID3D11Texture3D* rendertarget;
    ID3D11RenderTargetView* rtv;
    ID3D11ShaderResourceView* srv;
    ID3D11UnorderedAccessView* uav;

    RenderTarget3D() {}
    RenderTarget3D(int _width, int _height, int _depth)
    {
        D3D11_TEXTURE3D_DESC textureDesc = {};
        textureDesc.Width = _width;
        textureDesc.Height = _height;
        textureDesc.Height = _height;
        textureDesc.Depth = _depth;
        textureDesc.MipLevels = 0;
        textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        textureDesc.Usage = D3D11_USAGE_DEFAULT;
        textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
        textureDesc.CPUAccessFlags = 0;
        textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;


        pd3dDevice->CreateTexture3D(&textureDesc, nullptr, &rendertarget);
        pd3dDevice->CreateRenderTargetView(rendertarget, nullptr, &rtv);
        pd3dDevice->CreateShaderResourceView(rendertarget, nullptr, &srv);
        pd3dDevice->CreateUnorderedAccessView(rendertarget, nullptr, &uav);
    }

    void attach_srv(int _slot)
    {
        pImmediateContext->VSSetShaderResources(_slot, 1, &srv);
        pImmediateContext->GSSetShaderResources(_slot, 1, &srv);
        pImmediateContext->PSSetShaderResources(_slot, 1, &srv);
        pImmediateContext->CSSetShaderResources(_slot, 1, &srv);
    }

    void attach_uav(int _slot)
    {
        pImmediateContext->CSSetUnorderedAccessViews(_slot, 1, &uav, nullptr);
    }

    void clear_rendertarget(float _clean_color[4])
    {
        pImmediateContext->ClearRenderTargetView(rtv, _clean_color);
    }

    void copy_resource(RenderTarget3D _rendertarget3d)
    {
        D3D11_TEXTURE3D_DESC textureDesc = {};
        _rendertarget3d.rendertarget->GetDesc(&textureDesc);
        pd3dDevice->CreateTexture3D(&textureDesc, nullptr, &rendertarget);
        pd3dDevice->CreateRenderTargetView(rendertarget, nullptr, &rtv);
        pd3dDevice->CreateShaderResourceView(rendertarget, nullptr, &srv);
        pd3dDevice->CreateUnorderedAccessView(rendertarget, nullptr, &uav);
        pImmediateContext->CopyResource(rendertarget, _rendertarget3d.rendertarget);
    }

    void release()
    {
        rtv->Release();
        rtv = nullptr;
        srv->Release();
        srv = nullptr;
        uav->Release();
        uav = nullptr;
        rendertarget->Release();
        rendertarget = nullptr;

    }

    void generate_mipmaps()
    {
        pImmediateContext->GenerateMips(srv);
    }
};
struct Buffer {

    ID3D11Buffer* buffer;
    ID3D11UnorderedAccessView* uav;
    ID3D11ShaderResourceView* srv;

    Buffer(int _size, int _size_per_element, bool _cpu_read_access = false, bool _indirect_buffer = false)
    {

        if (_indirect_buffer)
        {

            D3D11_BUFFER_DESC bufferDesc = {};
            bufferDesc.Usage = D3D11_USAGE_DEFAULT;
            bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
            bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS | D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
            bufferDesc.ByteWidth = _size_per_element * _size;
            bufferDesc.StructureByteStride = 0;
            bufferDesc.CPUAccessFlags = 0;
            pd3dDevice->CreateBuffer(&bufferDesc, nullptr, &buffer);


            D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
            uavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
            uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
            uavDesc.Buffer.NumElements = _size;
            uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
            pd3dDevice->CreateUnorderedAccessView(buffer, &uavDesc, &uav);

            D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
            srvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
            srvDesc.BufferEx.FirstElement = 0;
            srvDesc.BufferEx.NumElements = _size;
            srvDesc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
            pd3dDevice->CreateShaderResourceView(buffer, &srvDesc, &srv);
        }
        else
        {
            D3D11_BUFFER_DESC bufferDesc = {};
            bufferDesc.Usage = D3D11_USAGE_DEFAULT;
            bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;

            if (_cpu_read_access) bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

            bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
            bufferDesc.ByteWidth = _size_per_element * _size;
            bufferDesc.StructureByteStride = _size_per_element;

            pd3dDevice->CreateBuffer(&bufferDesc, nullptr, &buffer);

            D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
            uavDesc.Format = DXGI_FORMAT_UNKNOWN;
            uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
            uavDesc.Buffer.NumElements = _size;
            pd3dDevice->CreateUnorderedAccessView(buffer, &uavDesc, &uav);

            D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
            srvDesc.Format = DXGI_FORMAT_UNKNOWN;
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
            srvDesc.Buffer.NumElements = _size;
            pd3dDevice->CreateShaderResourceView(buffer, &srvDesc, &srv);
        }

    }

    void attach_srv(int _slot)
    {
        pImmediateContext->VSSetShaderResources(_slot, 1, &srv);
        pImmediateContext->GSSetShaderResources(_slot, 1, &srv);
        pImmediateContext->PSSetShaderResources(_slot, 1, &srv);
        pImmediateContext->CSSetShaderResources(_slot, 1, &srv);
    }

    void attach_uav(int _slot)
    {
        pImmediateContext->CSSetUnorderedAccessViews(_slot, 1, &uav, nullptr);
    }

    void release()
    {
        srv->Release();
        srv = nullptr;
        uav->Release();
        uav = nullptr;
        buffer->Release();
        buffer = nullptr;
    }
};
struct Viewport
{
    D3D11_VIEWPORT viewport;
    Viewport(float _width, float _height)
    {
        viewport = { 0.0f, 0.0f, _width, _height, 0.0f, 1.0f };
    }

    void set(float _offsetx0, float _offsety0, float _width, float _height)
    {
        viewport.TopLeftX = _offsetx0;
        viewport.TopLeftY = _offsety0;
        viewport.Width = _width;
        viewport.Height = _height;
        viewport.MinDepth = 0.0;
        viewport.MaxDepth = 1.0f;
    }

    void use()
    {
        pImmediateContext->RSSetViewports(1, &viewport);
    }
};
struct Rasterizer
{
    ID3D11RasterizerState* rasterizer = nullptr;
    Rasterizer()
    {
        D3D11_RASTERIZER_DESC sceneRasterizerDesc = {};
        {
            sceneRasterizerDesc.FillMode = D3D11_FILL_SOLID;
            sceneRasterizerDesc.CullMode = D3D11_CULL_BACK;
        }
        pd3dDevice->CreateRasterizerState(&sceneRasterizerDesc, &rasterizer);
    }

    Rasterizer(D3D11_FILL_MODE _fill_mode, D3D11_CULL_MODE _cull_mode)
    {
        D3D11_RASTERIZER_DESC sceneRasterizerDesc = {};
        {
            sceneRasterizerDesc.FillMode = _fill_mode;
            sceneRasterizerDesc.CullMode = _cull_mode;
        }
        pd3dDevice->CreateRasterizerState(&sceneRasterizerDesc, &rasterizer);
    }

    void use()
    {
        pImmediateContext->RSSetState(rasterizer);
    }

};
struct AlphaBlending
{
    ID3D11BlendState* blending = nullptr;
    AlphaBlending()
    {
        D3D11_BLEND_DESC blend_desc;
        blend_desc.RenderTarget[0].BlendEnable = TRUE;
        blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
        blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        

        pd3dDevice->CreateBlendState(&blend_desc, &blending);

    }
    void use()
    {
        float blend_factor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        UINT sample_mask = 0xFFFFFFFF;

        pImmediateContext->OMSetBlendState(blending, blend_factor, sample_mask);
    }
};
struct AdditiveBlending
{
    ID3D11BlendState* blending = nullptr;

    AdditiveBlending()
    {
        D3D11_BLEND_DESC blend_desc;        
        blend_desc.RenderTarget[0].BlendEnable = TRUE;
        blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
        blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
        blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
        blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        

        pd3dDevice->CreateBlendState(&blend_desc, &blending);
    }

    void use()
    {
        float blend_factor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        UINT sample_mask = 0xFFFFFFFF;
        pImmediateContext->OMSetBlendState(blending, blend_factor, sample_mask);
    }
};
struct DepthStencil
{
    ID3D11DepthStencilState* depth_stencil = nullptr;
    DepthStencil(bool _depth_enable, D3D11_DEPTH_WRITE_MASK _write_mask, D3D11_COMPARISON_FUNC _comparison_func)
    {
        D3D11_DEPTH_STENCIL_DESC depthstencil_desc;
        depthstencil_desc.DepthEnable = _depth_enable;
        depthstencil_desc.DepthWriteMask = _write_mask;
        depthstencil_desc.DepthFunc = _comparison_func;        

        pd3dDevice->CreateDepthStencilState(&depthstencil_desc, &depth_stencil);
    }

    void use()
    {
        pImmediateContext->OMSetDepthStencilState(depth_stencil, 1);
    }
};
struct SamplerState
{
    ID3D11SamplerState* sampler_state = nullptr;

    SamplerState(D3D11_FILTER _filter, D3D11_TEXTURE_ADDRESS_MODE _adress)
    {
        D3D11_SAMPLER_DESC sampDesc;
        sampDesc.Filter = _filter;
        sampDesc.AddressU = _adress;
        sampDesc.AddressV = _adress;
        sampDesc.AddressW = _adress;
        sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        sampDesc.MinLOD = 0;
        sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

        pd3dDevice->CreateSamplerState(&sampDesc, &sampler_state);
    }

    void set_sampler(int _slot)
    {
        pImmediateContext->VSSetSamplers(_slot, 1, &sampler_state);
        pImmediateContext->GSSetSamplers(_slot, 1, &sampler_state);
        pImmediateContext->PSSetSamplers(_slot, 1, &sampler_state);
        pImmediateContext->CSSetSamplers(_slot, 1, &sampler_state);
    }
};
void clean_srv(int _slot)
{
    pImmediateContext->VSSetShaderResources(_slot, 1, &null_srv);
    pImmediateContext->GSSetShaderResources(_slot, 1, &null_srv);
    pImmediateContext->PSSetShaderResources(_slot, 1, &null_srv);
    pImmediateContext->CSSetShaderResources(_slot, 1, &null_srv);
}
void clean_uav(int _slot)
{
    pImmediateContext->CSSetUnorderedAccessViews(_slot, 1, &null_uav, nullptr);
}
void clear_blending()
{
    pImmediateContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
}
void clear_depthstencil()
{
    pImmediateContext->OMSetDepthStencilState(nullptr, 0);
}
void emit_vertex(int _vertexcount, int _instancecount)
{
    pImmediateContext->IASetInputLayout(nullptr);
    pImmediateContext->IASetVertexBuffers(0, 0, nullptr, 0, 0);
    pImmediateContext->IASetIndexBuffer(0, DXGI_FORMAT_R32_UINT, 0);
    pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
    pImmediateContext->DrawInstanced(_vertexcount, _instancecount, 0, 0);
}
void emit_vertex_indirect(Buffer* b)
{
    pImmediateContext->IASetInputLayout(nullptr);
    pImmediateContext->IASetVertexBuffers(0, 0, nullptr, 0, 0);
    pImmediateContext->IASetIndexBuffer(0, DXGI_FORMAT_R32_UINT, 0);
    pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
    pImmediateContext->DrawInstancedIndirect(b->buffer, 0);
}
void set_renders_and_uavs(ID3D11RenderTargetView* rtvs[], int numrtvs, ID3D11DepthStencilView* dsv, ID3D11UnorderedAccessView* uavs[], int offsetuavs, int numuavs)
{
    pImmediateContext->OMSetRenderTargetsAndUnorderedAccessViews(numrtvs, rtvs, dsv, offsetuavs, numuavs, uavs, 0);
}

ID3D11VertexShader* CompileVertexShader(LPCSTR lEntryPoint)
{
	ID3D11VertexShader* pVS;

#ifndef _DEBUG
    hr = D3DCompile(shader_content, sizeof(shader_content), 0, 0, 0, lEntryPoint, "vs_5_0", D3D10_SHADER_DEBUG, 0, &pBlob, &pErrorBlob);
#else
    hr = D3DCompileFromFile(L"source.shader", 0, 0, lEntryPoint, "vs_5_0", D3D10_SHADER_DEBUG, 0, &pBlob, &pErrorBlob);
#endif	
    
    hr = pd3dDevice->CreateVertexShader((DWORD*)pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &pVS);
	void* pp = pBlob->GetBufferPointer(); int ps = pBlob->GetBufferSize();
	if (pErrorBlob != 0) pErrorBlob->Release();
	if (pBlob != 0) pBlob->Release();
	return pVS;
}
ID3D11GeometryShader* CompileGeometryShader(LPCSTR lEntryPoint)
{
	ID3D11GeometryShader* pGS;

#ifndef _DEBUG
    hr = D3DCompile(shader_content, sizeof(shader_content), 0, 0, 0, lEntryPoint, "gs_5_0", D3D10_SHADER_DEBUG, 0, &pBlob, &pErrorBlob);
#else
    hr = D3DCompileFromFile(L"source.shader", 0, 0, lEntryPoint, "gs_5_0", D3D10_SHADER_DEBUG, 0, &pBlob, &pErrorBlob);
#endif

	hr = pd3dDevice->CreateGeometryShader((DWORD*)pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &pGS);
	void* pp = pBlob->GetBufferPointer(); int ps = pBlob->GetBufferSize();
	if (pErrorBlob != 0) pErrorBlob->Release();
	if (pBlob != 0) pBlob->Release();
	return pGS;
}
ID3D11PixelShader* CompilePixelShader(LPCSTR lEntryPoint)
{
	ID3D11PixelShader* pPS;

#ifndef _DEBUG
    hr = D3DCompile(shader_content, sizeof(shader_content), 0, 0, 0, lEntryPoint, "ps_5_0", D3D10_SHADER_DEBUG, 0, &pBlob, &pErrorBlob);
#else
    hr = D3DCompileFromFile(L"source.shader", 0, 0, lEntryPoint, "ps_5_0", D3D10_SHADER_DEBUG, 0, &pBlob, &pErrorBlob);
#endif	

    hr = pd3dDevice->CreatePixelShader((DWORD*)pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &pPS);
	void* pp = pBlob->GetBufferPointer(); int ps = pBlob->GetBufferSize();
	if (pErrorBlob != 0) pErrorBlob->Release();
	if (pBlob != 0) pBlob->Release();
	return pPS;
}
ID3D11ComputeShader* CompileComputeShader(LPCSTR lEntryPoint)
{
	ID3D11ComputeShader* pCS;

#ifndef _DEBUG
    hr = D3DCompile(shader_content, sizeof(shader_content), 0, 0, 0, lEntryPoint, "cs_5_0", D3D10_SHADER_DEBUG, 0, &pBlob, &pErrorBlob);
#else
    hr = D3DCompileFromFile(L"source.shader", 0, 0, lEntryPoint, "cs_5_0", D3D10_SHADER_DEBUG, 0, &pBlob, &pErrorBlob);
#endif	
    
    hr = pd3dDevice->CreateComputeShader((DWORD*)pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &pCS);
	void* pp = pBlob->GetBufferPointer(); int ps = pBlob->GetBufferSize();
	if (pErrorBlob != 0) pErrorBlob->Release();
	if (pBlob != 0) pBlob->Release();
	return pCS;
}

Viewport* viewport;
Rasterizer* rasterizer;
Rasterizer* rasterizer_back;
Rasterizer* rasterizer_nocull;
Rasterizer* rasterizer_wireframe;
SamplerState* linearwrap;
SamplerState* pointwrap;
SamplerState* linearclamp;
SamplerState* pointclamp;
AlphaBlending* alpha_blending;
AdditiveBlending* additive_blending;
DepthStencil* nowrite_depthstencil;
DepthStencil* write_depthstencil;
DepthStencil* nowrite_greater_depthstencil;
DepthStencil* write_greater_depthstencil;
RenderTarget2D* rendertarget_main;
RenderDepth2D* maindepth_texture;

ID3D11ComputeShader* pCSPlainColor = NULL;
ID3D11VertexShader* pVSStandard = NULL;
ID3D11PixelShader* pPSStandard = NULL;

Mesh* pMeshCube;

#define allocation(var,type, ...) var=(type*)allocate_object(sizeof(type)); *var = type(__VA_ARGS__);
void* allocate_object(unsigned int size)
{
    void* ptr = &(static_memory[static_memory_pointer]);
    static_memory_pointer += size;
    return ptr;
}

void preparation()
{	
    //Create vertex layout
    ID3D11VertexShader* vs = nullptr;
    ID3DBlob* inputlayoutsignature;
    D3DCompile(shader_input_layout_signature, sizeof(shader_input_layout_signature), 0, 0, 0, "vertex_input_layout_signature", "vs_5_0", D3D10_SHADER_DEBUG, 0, &inputlayoutsignature, nullptr);

    pd3dDevice->CreateVertexShader((void*)(((int*)inputlayoutsignature)[3]), ((int*)inputlayoutsignature)[2], NULL, &vs);

    D3D11_INPUT_ELEMENT_DESC layout[3] =
    {
         { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
         { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
         { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    pd3dDevice->CreateInputLayout(layout, ARRAYSIZE(layout), inputlayoutsignature->GetBufferPointer(), inputlayoutsignature->GetBufferSize(), &pCommonVertexLayout);
    UINT numElements = sizeof(layout) / sizeof(layout[0]);
    uCommonVertexLayoutStride = 8 * sizeof(float);

    //Create a viewport
    allocation(viewport, Viewport, width_window, height_window);

    //Create several rasterizers (one of them for wireframe)
    allocation(rasterizer, Rasterizer);
    allocation(rasterizer_back, Rasterizer, D3D11_FILL_SOLID, D3D11_CULL_FRONT);
    allocation(rasterizer_nocull, Rasterizer, D3D11_FILL_SOLID, D3D11_CULL_NONE);
    allocation(rasterizer_wireframe, Rasterizer, D3D11_FILL_WIREFRAME, D3D11_CULL_NONE);

    //Create samplers and set
    allocation(linearwrap, SamplerState, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP);
    allocation(pointwrap, SamplerState, D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP);
    allocation(linearclamp, SamplerState, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP);
    allocation(pointclamp, SamplerState, D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_CLAMP);

    linearwrap->set_sampler(0);
    pointwrap->set_sampler(1);
    linearclamp->set_sampler(2);
    pointclamp->set_sampler(3);

    //Create a additive and alpha blending mode 
    allocation(alpha_blending, AlphaBlending);
    allocation(additive_blending, AdditiveBlending);

    //Several depth stencil setups (write or not write to depth buffer)
    allocation(nowrite_depthstencil, DepthStencil, true, D3D11_DEPTH_WRITE_MASK_ZERO, D3D11_COMPARISON_LESS_EQUAL);
    allocation(write_depthstencil, DepthStencil, true, D3D11_DEPTH_WRITE_MASK_ALL, D3D11_COMPARISON_LESS_EQUAL);
    allocation(nowrite_greater_depthstencil, DepthStencil, true, D3D11_DEPTH_WRITE_MASK_ZERO, D3D11_COMPARISON_GREATER_EQUAL);
    allocation(write_greater_depthstencil, DepthStencil, true, D3D11_DEPTH_WRITE_MASK_ALL, D3D11_COMPARISON_GREATER_EQUAL);

    //Here we take the reference for the backbuffer (to draw the things) and create a depth map
    allocation(rendertarget_main, RenderTarget2D);
    rendertarget_main->get_backbuffer();

    allocation(maindepth_texture, RenderDepth2D, width_window, height_window);

    pCSPlainColor = CompileComputeShader("plain_color_cs");
    pVSStandard = CompileVertexShader("vs_main");
    pPSStandard = CompilePixelShader("ps_main");

    pMeshCube = allocation(pMeshCube, Mesh, cube_vertices, cube_indices, sizeof(cube_vertices), sizeof(cube_indices));

    // generic constant buffer
    pd3dDevice->CreateBuffer(&ConstBufferDesc, NULL, &pConstants);
    pImmediateContext->VSSetConstantBuffers(0, 1, &pConstants);
    pImmediateContext->PSSetConstantBuffers(0, 1, &pConstants);
    pImmediateContext->GSSetConstantBuffers(0, 1, &pConstants);
    pImmediateContext->CSSetConstantBuffers(0, 1, &pConstants);
}

static int buf[3] = { frame,width_window,height_window };
float BACKGROUND_COL[4] = { 0.0,0.0,0.0,1.0 };

void loop()
{
    buf[0] = frame;
    buf[1] = width_window;
    buf[2] = height_window;

    pImmediateContext->UpdateSubresource(pConstants, 0, 0, &buf[0], 0, 0);
    //pImmediateContext->CSSetShader(pCSPlainColor, NULL, 0);
    //rendertarget_main->attach_uav(0);
    //pImmediateContext->Dispatch(width_window / 8, height_window / 8, 1);

    viewport->set(0.0, 0.0, width_window, height_window);
    viewport->use();
    rasterizer_nocull->use();
    nowrite_depthstencil->use();

    rendertarget_main->clear_rendertarget(BACKGROUND_COL);
    maindepth_texture->clear_depth();
    rendertarget_main->set_rendertarget_and_depth(*maindepth_texture);

    pImmediateContext->VSSetShader(pVSStandard, nullptr, 0);
    pImmediateContext->PSSetShader(pPSStandard, nullptr, 0);
    pMeshCube->use();
    pMeshCube->draw_instanced(1);

    frame++;

}

#ifndef _DEBUG
void WinMainCRTStartup()
{
	ShowCursor(0);
	HWND  hWnd = CreateWindowExA(0, (LPCSTR)0xC018, 0, WS_POPUP | WS_VISIBLE | WS_MAXIMIZE, 0, 0, 0, 0, 0, 0, 0, 0); 
    RECT rect;
    if (GetWindowRect(hWnd, &rect)) {
        width_window = rect.right - rect.left;
        height_window = rect.bottom - rect.top;
    }
#else
int WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nFunsterStil)
{
	AllocConsole();
	freopen("CONOUT$", "w", stdout); // Redirige stdout a la consola
	HWND hWnd = CreateWindowA("edit", 0, WS_POPUP | WS_VISIBLE, 0, 0, width_window, height_window, 0, 0, 0, 0);
	//HWND hWnd = CreateWindowA("edit", 0, WS_POPUP | WS_VISIBLE | WS_MAXIMIZE, 0, 0, 0, 0, 0, 0, 0, 0);
#endif

    SwapChainDesc[0] = width_window;
    SwapChainDesc[1] = height_window;
	SwapChainDesc[11] = (int)hWnd;
	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, 0, 0, D3D11_SDK_VERSION, (DXGI_SWAP_CHAIN_DESC*)&SwapChainDesc[0], &pSwapChain, &pd3dDevice, NULL, &pImmediateContext);
	preparation();

	do
	{
		PeekMessageA(0, 0, 0, 0, PM_REMOVE);
		if (GetAsyncKeyState('R') & 0x8000){}
        loop();
		pSwapChain->Present(0, 0);
	}

	while (!GetAsyncKeyState(VK_ESCAPE));
	ExitProcess(0);  // actually not needed in this setup, but may be smaller..
}
