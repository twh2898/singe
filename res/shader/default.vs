#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec4 ourColor;

uniform mat4 mvp;
uniform mat4 model;

void main()
{
    gl_Position = mvp * vec4(aPos, 1.0);
    // gl_Position = vec4(aPos, 1.0);
    ourColor = vec4(aColor, 1.0);
}