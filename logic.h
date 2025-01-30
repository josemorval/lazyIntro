#ifndef LOGIC_HEADER_H
#define LOGIC_HEADER_H

static int frame = 0;
static D3D11_BUFFER_DESC constant_buffer_desc = { 256, D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER, 0, 0, 0 }; // 256,0,4,0,0,0

ID3D11ComputeShader* cs_plain_color = NULL;
ID3D11VertexShader* vs_standard = NULL;
ID3D11PixelShader* ps_standard = NULL;

Mesh* pMeshCube;

void preparation()
{
    //Create vertex layout
    ID3D11VertexShader* vs = nullptr;
    ID3DBlob* inputlayoutsignature;
    D3DCompile(shader_input_layout_signature, sizeof(shader_input_layout_signature), 0, 0, 0, "vertex_input_layout_signature", "vs_5_0", D3D10_SHADER_DEBUG, 0, &inputlayoutsignature, nullptr);

    device->CreateVertexShader((void*)(((int*)inputlayoutsignature)[3]), ((int*)inputlayoutsignature)[2], NULL, &vs);

    D3D11_INPUT_ELEMENT_DESC layout[3] =
    {
         { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
         { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
         { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    device->CreateInputLayout(layout, ARRAYSIZE(layout), inputlayoutsignature->GetBufferPointer(), inputlayoutsignature->GetBufferSize(), &common_vertex_layout);
    UINT numElements = sizeof(layout) / sizeof(layout[0]);
    common_vertex_layout_stride = 8 * sizeof(float);

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

    cs_plain_color = CompileComputeShader("plain_color_cs");
    vs_standard = CompileVertexShader("vs_main");
    ps_standard = CompilePixelShader("ps_main");

    pMeshCube = allocation(pMeshCube, Mesh, cube_vertices, cube_indices, sizeof(cube_vertices), sizeof(cube_indices));

    // generic constant buffer
    device->CreateBuffer(&constant_buffer_desc, NULL, &constants_buffer);
    inmediate->VSSetConstantBuffers(0, 1, &constants_buffer);
    inmediate->PSSetConstantBuffers(0, 1, &constants_buffer);
    inmediate->GSSetConstantBuffers(0, 1, &constants_buffer);
    inmediate->CSSetConstantBuffers(0, 1, &constants_buffer);
}

static int buf[3] = { 0, 0, 0 };
float BACKGROUND_COL[4] = { 0.0,0.0,0.0,1.0 };

void loop()
{
    buf[0] = frame;
    buf[1] = width_window;
    buf[2] = height_window;
    
    inmediate->UpdateSubresource(constants_buffer, 0, 0, &buf[0], 0, 0);

    viewport->set(0.0,0.0, width_window, height_window);
    viewport->use();
    rasterizer->use();
    write_depthstencil->use();

    rendertarget_main->clear_rendertarget(BACKGROUND_COL);
    maindepth_texture->clear_depth();
    rendertarget_main->set_rendertarget_and_depth(maindepth_texture);

    inmediate->VSSetShader(vs_standard, nullptr, 0);
    inmediate->PSSetShader(ps_standard, nullptr, 0);
    pMeshCube->use();
    pMeshCube->draw_instanced(1);

    frame++;

}

#endif