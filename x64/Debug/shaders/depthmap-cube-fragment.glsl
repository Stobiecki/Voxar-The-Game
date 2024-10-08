#version 330 core

uniform vec3  lightPos;
uniform float farPlane;

in vec3 fragPos;
out vec4 outColor;

void main()
{
    // odleglosc fragmentu od zrodla swiatla
    float lightDistance = length(fragPos - lightPos);

    // znormalizowanie odleglosci do [0,1]
    lightDistance = lightDistance / farPlane;

    // ustawienie glebokosci na ta odleglosc
    gl_FragDepth = lightDistance;

}
