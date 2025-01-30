#ifndef LIB3D_HEADER_H
#define LIB3D_HEADER_H

ID3D11ShaderResourceView* null_srv;
ID3D11UnorderedAccessView* null_uav;
ID3D11RenderTargetView* null_rtv;
ID3D11Buffer* null_buffer;
ID3D11InputLayout* common_vertex_layout = NULL;
UINT common_vertex_layout_stride = 0;
ID3D11Buffer* constants_buffer;

static float quad_vertices[] = {
    -0.5f,  0.5f, 0.0f, 0.0f, 0.0f , 0.0f, 1.0f, 0.0f,
    0.5f,  0.5f, 0.0f, 1.0f, 0.0f , 0.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, 0.0f, 0.0f, 1.0f , 0.0f, 1.0f, 0.0f,
    0.5f, -0.5f, 0.0f, 1.0f, 1.0f , 0.0f, 1.0f, 0.0f
};
static int quad_indices[] = {
    0, 1, 2,
    2, 1, 3
};
static float cube_vertices[] = {
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
static int cube_indices[] = {
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
        device->CreateBuffer(&vertexBufferDesc, &initData, &vertex_buffer);

        D3D11_BUFFER_DESC indexBufferDesc = {};
        indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        indexBufferDesc.ByteWidth = _indices_size;
        indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER | D3D11_BIND_SHADER_RESOURCE;
        indexBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;

        D3D11_SUBRESOURCE_DATA indexData = {};
        indexData.pSysMem = _indices;
        device->CreateBuffer(&indexBufferDesc, &indexData, &index_buffer);

        vertices_count = _vertices_size / sizeof(float);
        indices_count = _indices_size / sizeof(int);

        stride = 8 * sizeof(float);
        offset = 0;

        D3D11_SHADER_RESOURCE_VIEW_DESC vertexSRVDesc = {};
        vertexSRVDesc.Format = DXGI_FORMAT_R32_FLOAT; // Ajusta el formato según tus necesidades
        vertexSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
        vertexSRVDesc.Buffer.ElementOffset = 0;
        vertexSRVDesc.Buffer.ElementWidth = _vertices_size / sizeof(float);

        device->CreateShaderResourceView(vertex_buffer, &vertexSRVDesc, &vertex_srv);

        // Crear SRV para el búfer de índices
        D3D11_SHADER_RESOURCE_VIEW_DESC indexSRVDesc = {};
        indexSRVDesc.Format = DXGI_FORMAT_R32_UINT; // Ajusta el formato según tus necesidades
        indexSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
        indexSRVDesc.Buffer.ElementOffset = 0;
        indexSRVDesc.Buffer.ElementWidth = _indices_size / sizeof(int);
        device->CreateShaderResourceView(index_buffer, &indexSRVDesc, &index_srv);
    }

    void attach_vertices_srv(int _slot)
    {
        inmediate->VSSetShaderResources(_slot, 1, &vertex_srv);
        inmediate->GSSetShaderResources(_slot, 1, &vertex_srv);
        inmediate->PSSetShaderResources(_slot, 1, &vertex_srv);
        inmediate->CSSetShaderResources(_slot, 1, &vertex_srv);
    }

    void attach_indices_srv(int _slot)
    {
        inmediate->VSSetShaderResources(_slot, 1, &index_srv);
        inmediate->GSSetShaderResources(_slot, 1, &index_srv);
        inmediate->PSSetShaderResources(_slot, 1, &index_srv);
        inmediate->CSSetShaderResources(_slot, 1, &index_srv);
    }

    void use()
    {
        inmediate->IASetInputLayout(common_vertex_layout);
        inmediate->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);
        inmediate->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);
        inmediate->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    }

    void draw()
    {
        inmediate->DrawIndexed(indices_count, 0, 0);
    }

    void draw_instanced(int instances_count)
    {
        inmediate->DrawIndexedInstanced(indices_count, instances_count, 0, 0, 0);
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

        device->CreateTexture2D(&depthDesc, nullptr, &rendertarget);

        D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
        dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
        dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        device->CreateDepthStencilView(rendertarget, &dsvDesc, &dsv);

        D3D11_SHADER_RESOURCE_VIEW_DESC srvdesc = {};
        srvdesc.Format = DXGI_FORMAT_R32_FLOAT;
        srvdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvdesc.Texture2D.MipLevels = 1;
        device->CreateShaderResourceView(rendertarget, &srvdesc, &srv);

    }

    void attach_srv(int _slot)
    {
        inmediate->VSSetShaderResources(_slot, 1, &srv);
        inmediate->GSSetShaderResources(_slot, 1, &srv);
        inmediate->PSSetShaderResources(_slot, 1, &srv);
        inmediate->CSSetShaderResources(_slot, 1, &srv);
    }

    void set_depth()
    {
        inmediate->OMSetRenderTargets(0, nullptr, dsv);
    }

    void clear_depth()
    {
        inmediate->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH, 1.0, 0);
    }

    void clear_depth_zero()
    {
        inmediate->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH, 0.0, 0);
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


        device->CreateTexture2D(&textureDesc, nullptr, &rendertarget);
        device->CreateRenderTargetView(rendertarget, nullptr, &rtv);
        device->CreateShaderResourceView(rendertarget, nullptr, &srv);
        device->CreateUnorderedAccessView(rendertarget, nullptr, &uav);
    }

    void get_backbuffer()
    {
        swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&rendertarget);
        device->CreateRenderTargetView(rendertarget, nullptr, &rtv);
        device->CreateShaderResourceView(rendertarget, nullptr, &srv);
        device->CreateUnorderedAccessView(rendertarget, nullptr, &uav);
    }

    void attach_srv(int _slot)
    {
        inmediate->VSSetShaderResources(_slot, 1, &srv);
        inmediate->GSSetShaderResources(_slot, 1, &srv);
        inmediate->PSSetShaderResources(_slot, 1, &srv);
        inmediate->CSSetShaderResources(_slot, 1, &srv);
    }

    void attach_uav(int _slot)
    {
        inmediate->CSSetUnorderedAccessViews(_slot, 1, &uav, nullptr);
    }

    void set_rendertarget()
    {
        inmediate->OMSetRenderTargets(1, &rtv, nullptr);
    }

    void set_rendertarget_and_depth(RenderDepth2D* _depth2d)
    {
        inmediate->OMSetRenderTargets(1, &rtv, (*_depth2d).dsv);
    }

    void clear_rendertarget(float _clean_color[4])
    {
        inmediate->ClearRenderTargetView(rtv, _clean_color);
    }

    void copy_resource(RenderTarget2D* _rendertarget2d)
    {
        D3D11_TEXTURE2D_DESC textureDesc = {};
        (*_rendertarget2d).rendertarget->GetDesc(&textureDesc);
        device->CreateTexture2D(&textureDesc, nullptr, &rendertarget);
        device->CreateRenderTargetView(rendertarget, nullptr, &rtv);
        device->CreateShaderResourceView(rendertarget, nullptr, &srv);
        device->CreateUnorderedAccessView(rendertarget, nullptr, &uav);
        inmediate->CopyResource(rendertarget, (*_rendertarget2d).rendertarget);
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
        inmediate->GenerateMips(srv);
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


        device->CreateTexture3D(&textureDesc, nullptr, &rendertarget);
        device->CreateRenderTargetView(rendertarget, nullptr, &rtv);
        device->CreateShaderResourceView(rendertarget, nullptr, &srv);
        device->CreateUnorderedAccessView(rendertarget, nullptr, &uav);
    }

    void attach_srv(int _slot)
    {
        inmediate->VSSetShaderResources(_slot, 1, &srv);
        inmediate->GSSetShaderResources(_slot, 1, &srv);
        inmediate->PSSetShaderResources(_slot, 1, &srv);
        inmediate->CSSetShaderResources(_slot, 1, &srv);
    }

    void attach_uav(int _slot)
    {
        inmediate->CSSetUnorderedAccessViews(_slot, 1, &uav, nullptr);
    }

    void clear_rendertarget(float _clean_color[4])
    {
        inmediate->ClearRenderTargetView(rtv, _clean_color);
    }

    void copy_resource(RenderTarget3D* _rendertarget3d)
    {
        D3D11_TEXTURE3D_DESC textureDesc = {};
        (*_rendertarget3d).rendertarget->GetDesc(&textureDesc);
        device->CreateTexture3D(&textureDesc, nullptr, &rendertarget);
        device->CreateRenderTargetView(rendertarget, nullptr, &rtv);
        device->CreateShaderResourceView(rendertarget, nullptr, &srv);
        device->CreateUnorderedAccessView(rendertarget, nullptr, &uav);
        inmediate->CopyResource(rendertarget, (*_rendertarget3d).rendertarget);
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
        inmediate->GenerateMips(srv);
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
            device->CreateBuffer(&bufferDesc, nullptr, &buffer);


            D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
            uavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
            uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
            uavDesc.Buffer.NumElements = _size;
            uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
            device->CreateUnorderedAccessView(buffer, &uavDesc, &uav);

            D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
            srvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
            srvDesc.BufferEx.FirstElement = 0;
            srvDesc.BufferEx.NumElements = _size;
            srvDesc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
            device->CreateShaderResourceView(buffer, &srvDesc, &srv);
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

            device->CreateBuffer(&bufferDesc, nullptr, &buffer);

            D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
            uavDesc.Format = DXGI_FORMAT_UNKNOWN;
            uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
            uavDesc.Buffer.NumElements = _size;
            device->CreateUnorderedAccessView(buffer, &uavDesc, &uav);

            D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
            srvDesc.Format = DXGI_FORMAT_UNKNOWN;
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
            srvDesc.Buffer.NumElements = _size;
            device->CreateShaderResourceView(buffer, &srvDesc, &srv);
        }

    }

    void attach_srv(int _slot)
    {
        inmediate->VSSetShaderResources(_slot, 1, &srv);
        inmediate->GSSetShaderResources(_slot, 1, &srv);
        inmediate->PSSetShaderResources(_slot, 1, &srv);
        inmediate->CSSetShaderResources(_slot, 1, &srv);
    }

    void attach_uav(int _slot)
    {
        inmediate->CSSetUnorderedAccessViews(_slot, 1, &uav, nullptr);
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
        inmediate->RSSetViewports(1, &viewport);
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
        device->CreateRasterizerState(&sceneRasterizerDesc, &rasterizer);
    }

    Rasterizer(D3D11_FILL_MODE _fill_mode, D3D11_CULL_MODE _cull_mode)
    {
        D3D11_RASTERIZER_DESC sceneRasterizerDesc = {};
        {
            sceneRasterizerDesc.FillMode = _fill_mode;
            sceneRasterizerDesc.CullMode = _cull_mode;
        }
        device->CreateRasterizerState(&sceneRasterizerDesc, &rasterizer);
    }

    void use()
    {
        inmediate->RSSetState(rasterizer);
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


        device->CreateBlendState(&blend_desc, &blending);

    }
    void use()
    {
        float blend_factor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        UINT sample_mask = 0xFFFFFFFF;

        inmediate->OMSetBlendState(blending, blend_factor, sample_mask);
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


        device->CreateBlendState(&blend_desc, &blending);
    }

    void use()
    {
        float blend_factor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        UINT sample_mask = 0xFFFFFFFF;
        inmediate->OMSetBlendState(blending, blend_factor, sample_mask);
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

        device->CreateDepthStencilState(&depthstencil_desc, &depth_stencil);
    }

    void use()
    {
        inmediate->OMSetDepthStencilState(depth_stencil, 1);
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

        device->CreateSamplerState(&sampDesc, &sampler_state);
    }

    void set_sampler(int _slot)
    {
        inmediate->VSSetSamplers(_slot, 1, &sampler_state);
        inmediate->GSSetSamplers(_slot, 1, &sampler_state);
        inmediate->PSSetSamplers(_slot, 1, &sampler_state);
        inmediate->CSSetSamplers(_slot, 1, &sampler_state);
    }
};
void clean_srv(int _slot)
{
    inmediate->VSSetShaderResources(_slot, 1, &null_srv);
    inmediate->GSSetShaderResources(_slot, 1, &null_srv);
    inmediate->PSSetShaderResources(_slot, 1, &null_srv);
    inmediate->CSSetShaderResources(_slot, 1, &null_srv);
}
void clean_uav(int _slot)
{
    inmediate->CSSetUnorderedAccessViews(_slot, 1, &null_uav, nullptr);
}
void clear_blending()
{
    inmediate->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
}
void clear_depthstencil()
{
    inmediate->OMSetDepthStencilState(nullptr, 0);
}
void emit_vertex(int _vertexcount, int _instancecount)
{
    inmediate->IASetInputLayout(nullptr);
    inmediate->IASetVertexBuffers(0, 0, nullptr, 0, 0);
    inmediate->IASetIndexBuffer(0, DXGI_FORMAT_R32_UINT, 0);
    inmediate->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
    inmediate->DrawInstanced(_vertexcount, _instancecount, 0, 0);
}
void emit_vertex_indirect(Buffer* b)
{
    inmediate->IASetInputLayout(nullptr);
    inmediate->IASetVertexBuffers(0, 0, nullptr, 0, 0);
    inmediate->IASetIndexBuffer(0, DXGI_FORMAT_R32_UINT, 0);
    inmediate->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
    inmediate->DrawInstancedIndirect(b->buffer, 0);
}
void set_renders_and_uavs(ID3D11RenderTargetView* rtvs[], int numrtvs, ID3D11DepthStencilView* dsv, ID3D11UnorderedAccessView* uavs[], int offsetuavs, int numuavs)
{
    inmediate->OMSetRenderTargetsAndUnorderedAccessViews(numrtvs, rtvs, dsv, offsetuavs, numuavs, uavs, 0);
}

ID3D11VertexShader* CompileVertexShader(LPCSTR lEntryPoint)
{
    ID3D11VertexShader* pVS;

#ifndef _DEBUG
    hr = D3DCompile(shader_content, sizeof(shader_content), 0, 0, 0, lEntryPoint, "vs_5_0", D3D10_SHADER_DEBUG, 0, &blob, &error_blob);
#else
    hr = D3DCompileFromFile(L"source.shader", 0, 0, lEntryPoint, "vs_5_0", D3D10_SHADER_DEBUG, 0, &blob, &error_blob);
#endif	

    hr = device->CreateVertexShader((DWORD*)blob->GetBufferPointer(), blob->GetBufferSize(), NULL, &pVS);
    void* pp = blob->GetBufferPointer(); int ps = blob->GetBufferSize();
    if (error_blob != 0) error_blob->Release();
    if (blob != 0) blob->Release();
    return pVS;
}
ID3D11GeometryShader* CompileGeometryShader(LPCSTR lEntryPoint)
{
    ID3D11GeometryShader* pGS;

#ifndef _DEBUG
    hr = D3DCompile(shader_content, sizeof(shader_content), 0, 0, 0, lEntryPoint, "gs_5_0", D3D10_SHADER_DEBUG, 0, &blob, &error_blob);
#else
    hr = D3DCompileFromFile(L"source.shader", 0, 0, lEntryPoint, "gs_5_0", D3D10_SHADER_DEBUG, 0, &blob, &error_blob);
#endif

    hr = device->CreateGeometryShader((DWORD*)blob->GetBufferPointer(), blob->GetBufferSize(), NULL, &pGS);
    void* pp = blob->GetBufferPointer(); int ps = blob->GetBufferSize();
    if (error_blob != 0) error_blob->Release();
    if (blob != 0) blob->Release();
    return pGS;
}
ID3D11PixelShader* CompilePixelShader(LPCSTR lEntryPoint)
{
    ID3D11PixelShader* pPS;

#ifndef _DEBUG
    hr = D3DCompile(shader_content, sizeof(shader_content), 0, 0, 0, lEntryPoint, "ps_5_0", D3D10_SHADER_DEBUG, 0, &blob, &error_blob);
#else
    hr = D3DCompileFromFile(L"source.shader", 0, 0, lEntryPoint, "ps_5_0", D3D10_SHADER_DEBUG, 0, &blob, &error_blob);
#endif	

    hr = device->CreatePixelShader((DWORD*)blob->GetBufferPointer(), blob->GetBufferSize(), NULL, &pPS);
    void* pp = blob->GetBufferPointer(); int ps = blob->GetBufferSize();
    if (error_blob != 0) error_blob->Release();
    if (blob != 0) blob->Release();
    return pPS;
}
ID3D11ComputeShader* CompileComputeShader(LPCSTR lEntryPoint)
{
    ID3D11ComputeShader* pCS;

#ifndef _DEBUG
    hr = D3DCompile(shader_content, sizeof(shader_content), 0, 0, 0, lEntryPoint, "cs_5_0", D3D10_SHADER_DEBUG, 0, &blob, &error_blob);
#else
    hr = D3DCompileFromFile(L"source.shader", 0, 0, lEntryPoint, "cs_5_0", D3D10_SHADER_DEBUG, 0, &blob, &error_blob);
#endif	

    hr = device->CreateComputeShader((DWORD*)blob->GetBufferPointer(), blob->GetBufferSize(), NULL, &pCS);
    void* pp = blob->GetBufferPointer(); int ps = blob->GetBufferSize();
    if (error_blob != 0) error_blob->Release();
    if (blob != 0) blob->Release();
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


#endif