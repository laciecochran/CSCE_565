#version 150 


in vec4 color;
in vec2 Texcoord;

uniform sampler2D tex;

void main() 
{ 
    gl_FragColor = texture(tex, Texcoord) * color;
} 

