#version 150 

//used vertex lighting shader example from lecture

in vec4 vPosition;
in vec3 vNormal;
in vec2 texcoord;

out vec4 color;//vertex shade
out vec2 Texcoord;

//light and material properties
uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform vec4 LightPosition;
uniform float Shininess;

uniform mat4 ModelView;
uniform mat4 Projection;

void main() 
{
    //Transform vertex position into eye coordinates
    vec3 pos = (ModelView * vPosition).xyz;
    vec3 L;

    //compute the 4 vectors
    if(LightPosition.w != 0)//point light
        L = normalize((ModelView*LightPosition).xyz - pos);
    else //distant light
        L = normalize((ModelView*LightPosition).xyz);
    vec3 E = normalize( -pos );
    vec3 H = normalize( L+E );

    //transform vertex normal into eye coordinates
    vec3 N = normalize(ModelView*vec4(vNormal, 0.0)).xyz;

    //compute terms in the illuminatin equation
    //ambient term
    vec4 ambient = AmbientProduct;
    //diffuse term
    float Kd = max(dot(L,N), 0.0);
    vec4 diffuse = Kd*DiffuseProduct;
    //specular term
    float Ks = pow(max(dot(N,H), 0.0), Shininess);
    vec4 specular = Ks*SpecularProduct;
    //discard the specular highlight if the light's behind the vertex
    if(dot(L,N) < 0.0)
        specular = vec4(0.0,0.0,0.0,1.0);

    color = ambient + diffuse + specular;
    color.a = 1.0;

    gl_Position = Projection*ModelView*vPosition;
    Texcoord = texcoord;
} 
