#version 150 core


//out vec4 fColor;
in vec4 color;
in vec2 texCoord;
in vec3 R;

uniform samplerCube tex1;
uniform sampler2D tex2;
uniform sampler2D tex3;
uniform vec4 DiffuseProduct;

void main() 
{ 
    gl_FragColor = color * texture( tex1, R ) * texture(tex2, texCoord) * texture(tex3, texCoord);
} 

