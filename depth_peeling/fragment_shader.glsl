#version 400 core
flat in vec3 normal;

out vec4 out_color;

uniform sampler2DMS peel_depth_map;
vec4 apply_depth_peeling (const in vec4 color)
{
    float peel_depth = texelFetch (peel_depth_map, ivec2(gl_FragCoord.xy), gl_SampleID).r;

    if (gl_FragCoord.z <= peel_depth) {
        discard;
    } else {
        return color;
    }
}

void main()
{
    vec3 res = vec3(0,0,0);
    float alpha = 0.7;
#if 1
    if (normal.x != 0) {
        res = vec3(1,0,0);
    } else if (normal.y != 0) {
        res = vec3(0,1,0);
    } else if (normal.z != 0) {
        res = vec3(0,0,1);
    }

#else
    if (normal.x != 0) {
        if (normal.x == 1) {
            res = vec3(1,0,0);
        } else {
            res = vec3(1,1,0);
        }
    } else if (normal.y != 0) {
        if (normal.y == 1) {
            res = vec3(0,1,0);
        } else {
            res = vec3(0,1,1);
        }
    } else if (normal.z != 0) {
        if (normal.z == 1) {
            res = vec3(0,0,1);
        } else {
            res = vec3(1,0,1);
        }
    }
#endif

    out_color = apply_depth_peeling (vec4(res * alpha, alpha));
}
