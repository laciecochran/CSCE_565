#version 150 


//out vec4 fColor;
in vec4 color;
in vec2 texCoord;

uniform sampler2D tex1;
uniform sampler2D tex2; 

void main() 
{ 
    gl_FragColor = color * texture( tex1, texCoord ) * texture( tex2, texCoord );
} 

