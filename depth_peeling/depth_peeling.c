//gcc -DDEPTH_PEELING -g -Wall -o depth_peeling ../x11_platform.c -lGL -lcairo -lX11-xcb -lX11 -lxcb -lxcb-sync -lxcb-randr -lm
/*
 * Copiright (C) 2018 Santiago León O.
 */

#ifdef DEPTH_PEELING

struct camera_t {
    float width_m;
    float height_m;
    float near_plane;
    float far_plane;
    float pitch;
    float yaw;
    float distance;
};

dvec3 camera_compute_pos (struct camera_t *camera)
{
    camera->pitch = CLAMP (camera->pitch, -M_PI/2 + 0.0001, M_PI/2 - 0.0001);
    camera->yaw = WRAP (camera->yaw, -M_PI, M_PI);
    camera->distance = LOW_CLAMP (camera->distance, camera->near_plane);

    return DVEC3 (cos(camera->pitch)*sin(camera->yaw)*camera->distance,
                  sin(camera->pitch)*camera->distance,
                  cos(camera->pitch)*cos(camera->yaw)*camera->distance);
}

struct cuboid_t {
    fvec3 v[8];
};

#define UNIT_CUBE (struct cuboid_t){{\
                    FVEC3(-1,-1,-1), \
                    FVEC3(-1,-1, 1), \
                    FVEC3(-1, 1,-1), \
                    FVEC3(-1, 1, 1), \
                    FVEC3( 1,-1,-1), \
                    FVEC3( 1,-1, 1), \
                    FVEC3( 1, 1,-1), \
                    FVEC3( 1, 1, 1), \
                   }}

void cuboid_init (fvec3 dim, struct cuboid_t *res)
{
    *res = UNIT_CUBE;

    int i;
    for (i=0; i<8; i++) {
        res->v[i].x = res->v[i].x * dim.x/2;
        res->v[i].y = res->v[i].y * dim.y/2;
        res->v[i].z = res->v[i].z * dim.z/2;
    }
}

#define VA_CUBOID_SIZE (36*6*sizeof(float))

float* put_cuboid_in_vertex_array (struct cuboid_t *cuboid, float *dest)
{
    fvec3 ldb = cuboid->v[0];
    fvec3 ldf = cuboid->v[1];
    fvec3 lub = cuboid->v[2];
    fvec3 luf = cuboid->v[3];
    fvec3 rdb = cuboid->v[4];
    fvec3 rdf = cuboid->v[5];
    fvec3 rub = cuboid->v[6];
    fvec3 ruf = cuboid->v[7];

    float vertex_array[] = {
     // Coords                Normals
        rdb.x, rdb.y, rdb.z,  0.0f,  0.0f, -1.0f,
        ldb.x, ldb.y, ldb.z,  0.0f,  0.0f, -1.0f,
        rub.x, rub.y, rub.z,  0.0f,  0.0f, -1.0f,
        lub.x, lub.y, lub.z,  0.0f,  0.0f, -1.0f,
        rub.x, rub.y, rub.z,  0.0f,  0.0f, -1.0f,
        ldb.x, ldb.y, ldb.z,  0.0f,  0.0f, -1.0f,

        ldf.x, ldf.y, ldf.z,  0.0f,  0.0f,  1.0f,
        rdf.x, rdf.y, rdf.z,  0.0f,  0.0f,  1.0f,
        ruf.x, ruf.y, ruf.z,  0.0f,  0.0f,  1.0f,
        ruf.x, ruf.y, ruf.z,  0.0f,  0.0f,  1.0f,
        luf.x, luf.y, luf.z,  0.0f,  0.0f,  1.0f,
        ldf.x, ldf.y, ldf.z,  0.0f,  0.0f,  1.0f,

        luf.x, luf.y, luf.z, -1.0f,  0.0f,  0.0f,
        lub.x, lub.y, lub.z, -1.0f,  0.0f,  0.0f,
        ldb.x, ldb.y, ldb.z, -1.0f,  0.0f,  0.0f,
        ldb.x, ldb.y, ldb.z, -1.0f,  0.0f,  0.0f,
        ldf.x, ldf.y, ldf.z, -1.0f,  0.0f,  0.0f,
        luf.x, luf.y, luf.z, -1.0f,  0.0f,  0.0f,

        rub.x, rub.y, rub.z,  1.0f,  0.0f,  0.0f,
        ruf.x, ruf.y, ruf.z,  1.0f,  0.0f,  0.0f,
        rdb.x, rdb.y, rdb.z,  1.0f,  0.0f,  0.0f,
        rdf.x, rdf.y, rdf.z,  1.0f,  0.0f,  0.0f,
        rdb.x, rdb.y, rdb.z,  1.0f,  0.0f,  0.0f,
        ruf.x, ruf.y, ruf.z,  1.0f,  0.0f,  0.0f,

        ldb.x, ldb.y, ldb.z,  0.0f, -1.0f,  0.0f,
        rdb.x, rdb.y, rdb.z,  0.0f, -1.0f,  0.0f,
        rdf.x, rdf.y, rdf.z,  0.0f, -1.0f,  0.0f,
        rdf.x, rdf.y, rdf.z,  0.0f, -1.0f,  0.0f,
        ldf.x, ldf.y, ldf.z,  0.0f, -1.0f,  0.0f,
        ldb.x, ldb.y, ldb.z,  0.0f, -1.0f,  0.0f,

        lub.x, lub.y, lub.z,  0.0f,  1.0f,  0.0f,
        ruf.x, ruf.y, ruf.z,  0.0f,  1.0f,  0.0f,
        rub.x, rub.y, rub.z,  0.0f,  1.0f,  0.0f,
        luf.x, luf.y, luf.z,  0.0f,  1.0f,  0.0f,
        ruf.x, ruf.y, ruf.z,  0.0f,  1.0f,  0.0f,
        lub.x, lub.y, lub.z,  0.0f,  1.0f,  0.0f,
    };

    memcpy (dest, vertex_array, sizeof(vertex_array));
    return (float*)((uint8_t*)dest + sizeof (vertex_array));
}

struct scene_t {
    GLuint program_id;
    uint32_t vao_size;
    GLuint vao;
};

struct scene_t scene_init ()
{
    struct scene_t scene = {0};

    scene.program_id = gl_program ("vertex_shader.glsl", "fragment_shader.glsl");
    if (!scene.program_id) {
        return scene;
    }

    glGenVertexArrays (1, &scene.vao);

    float vertices[VA_CUBOID_SIZE];
    struct cuboid_t cube;
    cuboid_init (FVEC3 (1,1,1), &cube);
    put_cuboid_in_vertex_array (&cube, vertices);

    glBindVertexArray (scene.vao);

      GLuint vbo;
      glGenBuffers (1, &vbo);
      glBindBuffer (GL_ARRAY_BUFFER, vbo);
      glBufferData (GL_ARRAY_BUFFER, VA_CUBOID_SIZE, vertices, GL_STATIC_DRAW);

      GLuint pos_attr = glGetAttribLocation (scene.program_id, "position");
      glEnableVertexAttribArray (pos_attr);
      glVertexAttribPointer (pos_attr, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), 0);

      GLuint normal_attr = glGetAttribLocation (scene.program_id, "in_normal");
      glEnableVertexAttribArray (normal_attr);
      glVertexAttribPointer (normal_attr, 3, GL_FLOAT, GL_FALSE,
                             6*sizeof(float), (void*)(3*sizeof(float)));

    return scene;
}

void scene_update_camera (struct scene_t *scene, struct camera_t *camera)
{
    glUseProgram (scene->program_id);

    mat4f model = rotation_y (0);
    glUniformMatrix4fv (glGetUniformLocation (scene->program_id, "model"), 1, GL_TRUE, model.E);

    dvec3 camera_pos = camera_compute_pos (camera);
    mat4f view = look_at (camera_pos,
                          DVEC3(0,0,0),
                          DVEC3(0,1,0));
    glUniformMatrix4fv (glGetUniformLocation (scene->program_id, "view"), 1, GL_TRUE, view.E);

    mat4f projection = perspective_projection (-camera->width_m/2, camera->width_m/2,
                                               -camera->height_m/2, camera->height_m/2,
                                               camera->near_plane, camera->far_plane);
    glUniformMatrix4fv (glGetUniformLocation (scene->program_id, "proj"), 1, GL_TRUE, projection.E);
}

void scene_render (struct scene_t *scene)
{
    glBindVertexArray (scene->vao);
    glDrawArrays (GL_TRIANGLES, 0, 36);
}

void depth_peel_set_shader_slots (GLuint program_id,
                                  GLuint color_texture, GLuint depth_texture,
                                  GLuint peel_depth_map)
{
    glFramebufferTexture2D (
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D_MULTISAMPLE, color_texture, 0
    );
    glFramebufferTexture2D (
        GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
        GL_TEXTURE_2D_MULTISAMPLE, depth_texture, 0
    );

    glActiveTexture (GL_TEXTURE0);
    glBindTexture (GL_TEXTURE_2D_MULTISAMPLE, peel_depth_map);
    glUniform1i (glGetUniformLocation (program_id, "peel_depth_map"), 0);
}

bool update_and_render (struct app_state_t *st, app_graphics_t *graphics, app_input_t input)
{
    bool blit_needed = false;
    st->gui_st.gr = *graphics;
    if (!st->is_initialized) {
        st->end_execution = false;
        st->is_initialized = true;
    }

    update_input (&st->gui_st, input);

    switch (st->gui_st.input.keycode) {
        case 24: //KEY_Q
            st->end_execution = true;
            break;
        default:
            //if (input.keycode >= 8) {
            //    printf ("%" PRIu8 "\n", input.keycode);
            //    //printf ("%" PRIu16 "\n", input.modifiers);
            //}
            break;
    }

    static struct quad_renderer_t quad_renderer;
    static struct scene_t scene;
    static bool run_once = false;
    static struct camera_t main_camera;

    static GLuint fb;
    static GLuint color_texture;
    static GLuint depth_texture;
    static GLuint peel_depth_map;

    if (!run_once) {
        run_once = true;

        scene = scene_init ();
        if (scene.program_id == 0) {
            st->end_execution = true;
            return blit_needed;
        }

        float width = graphics->screen_width;
        float height = graphics->screen_height;
        // Framebuffer creation
        glGenFramebuffers (1, &fb);
        glBindFramebuffer (GL_FRAMEBUFFER, fb);

        create_color_texture (&color_texture, width, height, 4);
        create_depth_texture (&peel_depth_map, width, height, 4);
        create_depth_texture (&depth_texture, width, height, 4);

        quad_renderer = init_quad_renderer ();

        main_camera.near_plane = 0.1;
        main_camera.far_plane = 100;
        main_camera.pitch = M_PI/4;
        main_camera.yaw = M_PI/4;
        main_camera.distance = 4.5;
    }

    if (st->gui_st.dragging[0]) {
        dvec2 change = st->gui_st.ptr_delta;
        main_camera.pitch += 0.01 * change.y;
        main_camera.yaw -= 0.01 * change.x;
    }

    if (input.wheel != 1) {
        main_camera.distance -= (input.wheel - 1)*main_camera.distance*0.7;
    }

    main_camera.width_m = px_to_m_x (graphics, graphics->width);
    main_camera.height_m = px_to_m_y (graphics, graphics->height);

    scene_update_camera (&scene, &main_camera);

    glEnable (GL_DEPTH_TEST);
    glEnable (GL_SAMPLE_SHADING);
    glMinSampleShading (1.0);

    glBindFramebuffer (GL_FRAMEBUFFER, fb);
    glViewport (0, 0, graphics->width, graphics->height);
    glScissor (0, 0, graphics->width, graphics->height);

    // Initial texture contents
    //
    // color_texture -> (0,0,0,0)
    // depth_texture -> 1
    // opaque_color_texture -> (0,0,0,0)
    // opaque_depth_map -> 1
    // peel_depth_map -> 0

    // Init color_texture and depth_texture
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glFramebufferTexture2D (
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D_MULTISAMPLE, color_texture, 0
    );
    glFramebufferTexture2D (
        GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
        GL_TEXTURE_2D_MULTISAMPLE, depth_texture, 0
    );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Init peel_depth_map
    glFramebufferTexture2D (
        GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
        GL_TEXTURE_2D_MULTISAMPLE, peel_depth_map, 0
    );
    glClearDepth (0);
    glClear (GL_DEPTH_BUFFER_BIT);
    glClearDepth (1);

    int num_pass = 2;

    // Fragment shader slot content:
    //
    // COLOR BUFFER: color_texture
    // DEPTH BUFFER: depth_texture
    // uniform peel_depth_map: peel_depth_map
    depth_peel_set_shader_slots (scene.program_id,
                                 color_texture, depth_texture,
                                 peel_depth_map);

    glDisable (GL_BLEND);
    scene_render (&scene);

    glEnable (GL_BLEND);
    int i;
    for (i = 0; i < num_pass-1; i++) {
        // Swap the depth buffer with peel_depth_map shader slot
        GLuint tmp = peel_depth_map;
        peel_depth_map = depth_texture;
        depth_texture = tmp;

        glFramebufferTexture2D (
            GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
            GL_TEXTURE_2D_MULTISAMPLE, depth_texture, 0
        );

        glActiveTexture (GL_TEXTURE0);
        glBindTexture (GL_TEXTURE_2D_MULTISAMPLE, peel_depth_map);
        glUniform1i (glGetUniformLocation (scene.program_id, "peel_depth_map"), 0);

        glClear(GL_DEPTH_BUFFER_BIT);

        // Render scene using UNDER blending operator
        glBlendFunc (GL_ONE_MINUS_SRC_ALPHA, GL_ONE);
        scene_render (&scene);
    }

    // Blend resulting color buffer into the window using the OVER operator
    glBlendFunc (GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    draw_into_window (graphics);
    glClearColor(0.164f, 0.203f, 0.223f, 1.0f);
    glClear (GL_COLOR_BUFFER_BIT);
    set_texture_clip (&quad_renderer, graphics->screen_width, graphics->screen_height,
                      0, 0, graphics->width, graphics->height);
    blend_premul_quad (&quad_renderer, color_texture, true, graphics,
                        0, 0, graphics->width, graphics->height);

    return true;
}
#endif // DEPTH_PEELING
