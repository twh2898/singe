#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTex;

out vec3 ourPos;
out vec2 ourTex;
out vec3 ourNorm;

uniform mat4 mvp;

void main()
{
    vec4 pos = mvp * vec4(aPos, 1.0);
    gl_Position = pos;
    ourPos = pos.xyz;
    ourTex = aTex;
    ourNorm = aNorm;
}
