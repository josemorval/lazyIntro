#ifndef LOGIC_HEADER_H
#define LOGIC_HEADER_H

static int frame = 0;
static D3D11_BUFFER_DESC constant_buffer_desc = { 256, D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER, 0, 0, 0 }; // 256,0,4,0,0,0

CShader* cs_plain_color = NULL;
VShader* vs_standard = NULL;
PShader* ps_standard = NULL;

Mesh* mesh_cube;
Buffer* other_buffer;
RenderTarget2D* other_render;

//Constants buffer helper functions
void set_cb_framecount(float frame)
{
    constants_buffer->map();
    float* cb = (float*)constants_buffer->get_data();
    cb[0] = frame;
    constants_buffer->unmap();
}

void set_cb_rendersize(float x, float y)
{
    constants_buffer->map();
    float* cb = (float*)constants_buffer->get_data();
    cb[1] = x;
    cb[2] = y;
    constants_buffer->unmap();
}


void set_cb_position(float x, float y, float z)
{
    constants_buffer->map();
    float* cb = (float*)constants_buffer->get_data();
    cb[4] = x;
    cb[5] = y;
    cb[6] = z;
    constants_buffer->unmap();
}

void preparation()
{
    //Create vertex layout
    ID3D11VertexShader* vs = nullptr;
    ID3DBlob* inputlayoutsignature;
    D3DCompile(shader_vertex_input_layout_signature, sizeof(shader_vertex_input_layout_signature), 0, 0, 0, "vertex_input_layout_signature", "vs_5_0", D3D10_SHADER_DEBUG, 0, &inputlayoutsignature, nullptr);

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

    allocation(other_buffer, Buffer, 100, 16);
    allocation(other_render, RenderTarget2D, 1000, 1500 );

    allocation(cs_plain_color, CShader, "plain_color_cs");
    allocation(vs_standard, VShader, "vs_main");
    allocation(ps_standard, PShader, "ps_main");

    mesh_cube = allocation(mesh_cube, Mesh, cube_vertices, cube_indices, sizeof(cube_vertices), sizeof(cube_indices));

    // generic constant buffer
    allocation(constants_buffer, ConstantBuffer, 64);
    constants_buffer->attach(0);
}

float BACKGROUND_COL[4] = { 0.0,0.0,0.0,1.0 };

void loop()
{

    set_cb_framecount( frame );
    set_cb_rendersize( width_window, height_window );

    viewport->set(0.0,0.0, width_window, height_window);
    viewport->use();
    rasterizer->use();
    write_depthstencil->use();

    rendertarget_main->clear_rendertarget(BACKGROUND_COL);
    maindepth_texture->clear_depth();
    rendertarget_main->set_rendertarget_and_depth(maindepth_texture);

    vs_standard->use();
    ps_standard->use();

    set_cb_position(0.0, 0.0, 0.0);
    mesh_cube->use();
    mesh_cube->draw();

    frame++;

#ifdef _DEBUG
    if (GetAsyncKeyState('R') & 0x8000) // Comprobar si la tecla 'R' está presionada
    {
        if (!iskeypressed) // Si la tecla 'R' no ha sido presionada anteriormente
        {
            printf(BG_BLUE YELLOW "RECOMPILING SHADERS" RESET "\n");

            cs_plain_color->compile();
            vs_standard->compile();
            ps_standard->compile();

            iskeypressed = true; // Establecer como presionada
        }
    }
    else
    {
        iskeypressed = false; // Si la tecla no está presionada, resetear el estado
    }
#endif

}

#endif