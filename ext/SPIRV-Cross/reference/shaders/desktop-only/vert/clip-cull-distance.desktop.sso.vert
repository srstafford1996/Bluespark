#version 450

out gl_PerVertex
{
    vec4 gl_Position;
    float gl_PointSize;
    float gl_ClipDistance[4];
    float gl_CullDistance[3];
};

void main()
{
    gl_Position = vec4(1.0);
    gl_ClipDistance[0] = 0.0;
    gl_ClipDistance[1] = 0.0;
    gl_ClipDistance[2] = 0.0;
    gl_ClipDistance[3] = 0.0;
    gl_CullDistance[1] = 4.0;
}

