#version 150 core


//out vec4 fColor;
in vec4 color;
in vec2 texCoord;
in vec3 light;
in vec3 view;

uniform sampler2D tex1;
uniform sampler2D tex2;
uniform vec4 DiffuseProduct;

void main() 
{ 
    vec4 N = texture( tex1, texCoord ) * texture( tex2, texCoord );
    vec3 NN = normalize(2.0*N.xyz-1.0);
    vec3 LL = normalize(light);
    float Kd = max(dot(NN.xyz, LL), 0.0);
    gl_FragColor = color * Kd * DiffuseProduct;
} 

