#ifndef LOGIC_HEADER_H
#define LOGIC_HEADER_H

static int frame = 0;
static float BACKGROUND_COL[4] = { 0.0,0.0,0.0,1.0 };
static int MESHVERTEX_STRUCT_SIZE = 8;
static int INSTANCEPOINT_STRUCT_SIZE = 14;

VShader* vs_standard;
VShader* vs_instanced_standard;
VShader* vs_vertexid_standard;
VShader* vs_vertexid_instanced_standard;
PShader* ps_standard;
PShader* ps_ground_particles;

CShader* cs_generate_sphere;

Mesh* mesh_quad;
Mesh* mesh_cube;

Buffer* buffer_sphere_vertices;
Buffer* buffer_sphere_indices;

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
void set_cb_rotation(float x, float y, float z, float w)
{
    constants_buffer->map();
    float* cb = (float*)constants_buffer->get_data();
    cb[8] = x;
    cb[9] = y;
    cb[10] = z;
    cb[11] = w;
    constants_buffer->unmap();
}
void set_cb_scale(float x, float y, float z)
{
    constants_buffer->map();
    float* cb = (float*)constants_buffer->get_data();
    cb[12] = x;
    cb[13] = y;
    cb[14] = z;
    constants_buffer->unmap();
}
void set_cb_lightpass(float lightpass)
{
    constants_buffer->map();
    float* cb = (float*)constants_buffer->get_data();
    cb[15] = lightpass;
    constants_buffer->unmap();
}

void set_cb_campos(float x, float y, float z)
{
    constants_buffer->map();
    float* cb = (float*)constants_buffer->get_data();
    cb[16] = x;
    cb[17] = y;
    cb[18] = z;
    constants_buffer->unmap();
}
void set_cb_camview(float x, float y, float z)
{
    constants_buffer->map();
    float* cb = (float*)constants_buffer->get_data();
    cb[20] = x;
    cb[21] = y;
    cb[22] = z;
    constants_buffer->unmap();
}

void set_cb_nthreads(float nthreads)
{
    constants_buffer->map();
    float* cb = (float*)constants_buffer->get_data();
    cb[23] = nthreads;
    constants_buffer->unmap();
}

struct ProcMesh
{

};
struct Particles
{
    CShader* shader;
    Buffer* points;

    int npoints = 0;

    Particles(LPCSTR _lEntryPoint, int _npoints)
    {
        npoints = _npoints;
        allocation(shader, CShader, _lEntryPoint);
        allocation(points, Buffer, npoints, INSTANCEPOINT_STRUCT_SIZE * sizeof(float));
    }

    void update()
    {
        set_cb_nthreads((float)npoints);
        points->attach_uav(2);
        shader->use(npoints / 64 + 1, 1, 1);
        clean_uav(2);
    }

    void draw_mesh(VShader* _vshader, PShader* _pshader, Mesh* _mesh)
    {
        _vshader->use();
        _pshader->use();

        points->attach_srv(2);
        _mesh->use();
        _mesh->draw_instanced(npoints);
        clean_srv(2);
    }
};

Particles* particles0;
Particles* ground_particles;

void preparation()
{
    allocation(vs_standard, VShader, "vs_main");
    allocation(vs_instanced_standard, VShader, "vs_instanced_main");
    allocation(vs_vertexid_standard, VShader, "vs_vertexid_main");
    allocation(vs_vertexid_instanced_standard, VShader, "vs_vertexid_instanced_main");
    allocation(ps_standard, PShader, "ps_main");
    allocation(ps_ground_particles, PShader, "ps_ground_particles");

    allocation(mesh_cube, Mesh, cube_vertices, cube_indices, sizeof(cube_vertices), sizeof(cube_indices));
    allocation(mesh_quad, Mesh, quad_vertices, quad_indices, sizeof(quad_vertices), sizeof(quad_indices));

    allocation(cs_generate_sphere, CShader, "cs_generate_sphere");
    allocation(buffer_sphere_vertices, Buffer, 1000, MESHVERTEX_STRUCT_SIZE * sizeof(float));
    allocation(buffer_sphere_indices, Buffer, 1000 * 6, sizeof(float));

    buffer_sphere_vertices->attach_uav(0);
    buffer_sphere_indices->attach_uav(1);
    cs_generate_sphere->use(buffer_sphere_vertices->get_size(), 1, 1);
    clean_uav(0);
    clean_uav(1);

    allocation(particles0, Particles, "cs_rotating_cubes", 1);
    allocation(ground_particles, Particles, "cs_ground_particles", 2500);

    // generic constant buffer
    allocation(constants_buffer, ConstantBuffer, 256);
    constants_buffer->attach(0);

}

void compute_scene()
{

}

void render_scene()
{

}

void loop()
{
    //Initialize
    set_cb_framecount(frame);
    set_cb_rendersize(width_window, height_window);
    set_cb_position(0.0, 0.0, 0.0);
    set_cb_rotation(0.0, 0.0, 1.0, 0.0);
    set_cb_scale(1.0, 1.0, 1.0);
    set_cb_campos(10.0, 5.0, 10.0);
    set_cb_camview(0.0, 0.0, 0.0);

    viewport->set(0.0, 0.0, width_window, height_window);
    viewport->use();
    rasterizer->use();
    write_depthstencil->use();

    rendertarget_main->clear_rendertarget(BACKGROUND_COL);
    maindepth_texture->clear_depth();
    rendertarget_main->set_rendertarget_and_depth(maindepth_texture);


    particles0->update();
    particles0->draw_mesh(vs_instanced_standard,ps_standard, mesh_cube);

    ground_particles->update();
    ground_particles->draw_mesh(vs_instanced_standard, ps_ground_particles, mesh_quad);

    /*
    buffer_sphere_vertices->attach_srv(0);
    buffer_sphere_indices->attach_srv(1);
    buffer_instance_points->attach_srv(2);
    emit_vertex(6 * max_size_buffer_sphere, 10, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    clean_srv(0);
    clean_srv(1);
    clean_srv(2);
    */

    frame++;

#ifdef _DEBUG
    if (GetAsyncKeyState(VK_LSHIFT) & 0x8000) // Comprobar si la tecla 'R' está presionada
    {
        /*
        if (!iskeypressed) // Si la tecla 'R' no ha sido presionada anteriormente
        {
            printf(BG_BLUE YELLOW "RECOMPILING SHADERS" RESET "\n");

            vs_standard->compile();
            vs_instanced_standard->compile();
            vs_vertexid_standard->compile();
            vs_vertexid_instanced_standard->compile();
            ps_standard->compile();
            cs_generate_sphere->compile();
            cs_scatter_points->compile();

            iskeypressed = true; // Establecer como presionada
        }
        */
    }
    else
    {
        iskeypressed = false; // Si la tecla no está presionada, resetear el estado
    }
#endif

}

#endif