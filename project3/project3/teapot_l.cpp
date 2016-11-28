//
// Example of drawing a teapot 
// Part of the code was modified from examples in the textbooks
//

#include "Angel.h"
#include "loadModels.h"
#include <GL/glut.h> 


// the model
vector<vec4> vertices;
vector<GLushort> elements;
vector<vec3> normals;

//initialize shader lighting parameters
vec4 light_position = vec4(0.0, 0.0, 1.0, 0.0);
vec4 light_ambient = vec4(0.1, 0.1, 0.1, 1.0);
vec4 light_diffuse = vec4(1.0, 1.0, 1.0, 1.0);
vec4 light_specular = vec4(1.0, 1.0, 1.0, 1.0);

vec4 material_ambient = vec4(0.5, 0.0, 0.0, 1.0);
vec4 material_diffuse = vec4(0.5, 0.0, 0.0, 1.0);
vec4 material_specular = vec4(0.5, 0.0, 0.0, 1.0);
float material_shininess = 100.0;

//lighting products
vec4 ambient_product = vec4(0.0, 0.0, 0.0, 0.0);
vec4 diffuse_product = vec4(0.0, 0.0, 0.0, 0.0);
vec4 specular_product = vec4(0.0, 0.0, 0.0, 0.0);

//choose a,d,s; 0=inactive, 1=active
int ambient = 0;
int diffuse = 0;
int specular = 0;

//change material or light properties
int material, light = 0; //0=inactive, 1=active

//mouse
int old_x = 0;
int old_y = 0;

//window width and height
int height = 0;
int width = 0;

GLuint  ModelView;  // model-view matrix uniform shader variable location

GLuint  Projection; // projection matrix uniform shader variable location

GLuint program;


enum { ArrayBuffer, ElementBuffer, NumVertexBuffers };

//
//----------------------------------------------------------------------------

// OpenGL initialization
void
init()
{
    // Create a vertex array object
    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    // Load a model
    load_obj("../models/teapot.obj", vertices,elements,normals);
    if ((vertices.size()==0) || (elements.size()==0)|| (normals.size()==0))
        cout << "Failed to read the model"<<endl;
    else
        cout<< vertices.size()<<" "<<elements.size()<<endl;

    GLuint buffers[NumVertexBuffers];
	
	glGenBuffers( NumVertexBuffers, buffers );
	
	glBindBuffer( GL_ARRAY_BUFFER, buffers[ArrayBuffer]  );
	glBufferData( GL_ARRAY_BUFFER, vertices.size()*sizeof(vec4) + normals.size()*sizeof(vec3), NULL, GL_STATIC_DRAW );
	
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, buffers[ElementBuffer] );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, elements.size()*sizeof(GLushort), &elements[0], GL_STATIC_DRAW );
	
	glBufferSubData(GL_ARRAY_BUFFER,0,vertices.size()*sizeof(vec4), &vertices[0]);
	glBufferSubData(GL_ARRAY_BUFFER,vertices.size()*sizeof(vec4),sizeof(vec3)*normals.size(), &normals[0] );

    // Load shaders and use the resulting shader program
    program = InitShader( "vshader_teapot.glsl", "fshader_teapot.glsl" );
    glUseProgram( program );

    // set up vertex arrays
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,
			   BUFFER_OFFSET(0) );

    GLuint vNormal = glGetAttribLocation( program, "vNormal" ); 
    glEnableVertexAttribArray( vNormal );
    glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vertices)) );

    ModelView = glGetUniformLocation( program, "ModelView" );
    Projection = glGetUniformLocation( program, "Projection" );
    
    glEnable( GL_DEPTH_TEST );
    glShadeModel(GL_FLAT);
    glClearColor( 1.0, 1.0, 1.0, 1.0 ); 
}

//----------------------------------------------------------------------------

void
display( void )
{
    //lighting
    ambient_product = light_ambient*material_ambient;
    diffuse_product = light_diffuse*material_diffuse;
    specular_product = light_specular*material_specular;

    glUniform4fv(glGetUniformLocation(program, "AmbientProduct"),1,ambient_product);
    glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"),1,diffuse_product);
    glUniform4fv(glGetUniformLocation(program, "SpecularProduct"),1,specular_product);
    glUniform4fv(glGetUniformLocation(program, "LightPosition"),1,light_position);
    glUniform1f(glGetUniformLocation(program, "Shininess"),material_shininess);

    //model view and projection
    vec4 at(0.0, 0.0, 0.0, 1.0);
    vec4 eye(0.0, 0.0, 1.0, 1.0);
    vec4 up(0.0, 1.0, 0.0, 0.0);

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
   
    mat4 mv = LookAt(eye, at, up);
    glUniformMatrix4fv( ModelView, 1, GL_TRUE, mv );

    mat4 p = Frustum(-5, 5, -5, 5, 1, 10);
    glUniformMatrix4fv( Projection, 1, GL_TRUE, p );

    glDrawElements( GL_TRIANGLES, elements.size()*sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

    glutSwapBuffers();
}

//----------------------------------------------------------------------------

void myMouse(int button, int state, int x, int y)
{
    width = glutGet(GLUT_WINDOW_WIDTH);
    height = glutGet(GLUT_WINDOW_HEIGHT);
    if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
        //modify light properties
        light = 0;
        material = 1;
    }
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        //modify material properties
        light = 1;
        material = 0;
    }
    if(button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)
    {
        //move light position
        int dx = old_x - x;
        int dy = old_y - y;
        light_position.x += dx;
        light_position.y += dy;
    }
    old_x = x;
    old_y = y;
    glutPostRedisplay();
}

//----------------------------------------------------------------------------

void myKey(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 'Q':
        case 'q':
        {
             //exit
             exit (0);
             break;
        }
        case 'a':
        {
             //change ambient
             ambient = 1;
             diffuse = 0;
             specular = 0;
             break;
        }
        case 'd':
        {
             //change diffuse
             ambient = 0;
             diffuse = 1;
             specular = 0;
             break;
        }
        case 's':
        {
             //change specular
             ambient = 0;
             diffuse = 0;
             specular = 1;
             break;
        }
        case 'R':
        {
             //increase red component
             if(ambient == 1 && material == 1){
                if(material_ambient.x < 1.0)
                    material_ambient.x += 0.1;
             }
             else if(diffuse == 1 && material == 1){
                if(material_diffuse.x < 1.0)
                    material_diffuse.x += 0.1;
             }
             else if(specular == 1 && material == 1){
                if(material_specular.x < 1.0)
                    material_specular.x += 0.1;
             }
             else if(ambient == 1 && light == 1){
                if(light_ambient.x < 1.0)
                    light_ambient.x += 0.1;
             }
             else if(diffuse == 1 && light == 1){
                if(light_diffuse.x < 1.0)
                    light_diffuse.x += 0.1;
             }
             else if(specular == 1 && light == 1){
                if(light_specular.x < 1.0)
                    light_specular.x += 0.1;
             }
             glutPostRedisplay();
             break;
        }
        case 'r':
        {
             //decrease red component
             if(ambient == 1 && material == 1){
                if(material_ambient.x > 0.0)
                    material_ambient.x -= 0.1;
             }
             else if(diffuse == 1 && material == 1){
                if(material_diffuse.x > 0.0)
                    material_diffuse.x -= 0.1;
             }
             else if(specular == 1 && material == 1){
                if(material_specular.x > 0.0)
                    material_specular.x -= 0.1;
             }
             else if(ambient == 1 && light == 1){
                if(light_ambient.x > 0.0)
                    light_ambient.x -= 0.1;
             }
             else if(diffuse == 1 && light == 1){
                if(light_diffuse.x > 0.0)
                    light_diffuse.x -= 0.1;
             }
             else if(specular == 1 && light == 1){
                if(light_specular.x > 0.0)
                    light_specular.x -= 0.1;
             }
             glutPostRedisplay();
             break;
        }
        case 'G':
        {
             //increase green component
             if(ambient == 1 && material == 1){
                if(material_ambient.y < 1.0)
                    material_ambient.y += 0.1;
             }
             else if(diffuse == 1 && material == 1){
                if(material_diffuse.y < 1.0)
                    material_diffuse.y += 0.1;
             }
             else if(specular == 1 && material == 1){
                if(material_specular.y < 1.0)
                    material_specular.y += 0.1;
             }
             else if(ambient == 1 && light == 1){
                if(light_ambient.y < 1.0)
                    light_ambient.y += 0.1;
             }
             else if(diffuse == 1 && light == 1){
                if(light_diffuse.y < 1.0)
                    light_diffuse.y += 0.1;
             }
             else if(specular == 1 && light == 1){
                if(light_specular.y < 1.0)
                    light_specular.y += 0.1;
             }
             glutPostRedisplay();
             break;
        }
        case 'g':
        {
             //decrease green component
             if(ambient == 1 && material == 1){
                if(material_ambient.y > 0.0)
                    material_ambient.y -= 0.1;
             }
             else if(diffuse == 1 && material == 1){
                if(material_diffuse.y > 0.0)
                    material_diffuse.y -= 0.1;
             }
             else if(specular == 1 && material == 1){
                if(material_specular.y > 0.0)
                    material_specular.y -= 0.1;
             }
             else if(ambient == 1 && light == 1){
                if(light_ambient.y > 0.0)
                    light_ambient.y -= 0.1;
             }
             else if(diffuse == 1 && light == 1){
                if(light_diffuse.y > 0.0)
                    light_diffuse.y -= 0.1;
             }
             else if(specular == 1 && light == 1){
                if(light_specular.y > 0.0)
                    light_specular.y -= 0.1;
             }
             glutPostRedisplay();
             break;
        }
        case 'B':
        {
             //increase blue component
             if(ambient == 1 && material == 1){
                if(material_ambient.z < 1.0)
                    material_ambient.z += 0.1;
             }
             else if(diffuse == 1 && material == 1){
                if(material_diffuse.z < 1.0)
                    material_diffuse.z += 0.1;
             }
             else if(specular == 1 && material == 1){
                if(material_specular.z < 1.0)
                    material_specular.z += 0.1;
             }
             else if(ambient == 1 && light == 1){
                if(light_ambient.z < 1.0)
                    light_ambient.z += 0.1;
             }
             else if(diffuse == 1 && light == 1){
                if(light_diffuse.z < 1.0)
                    light_diffuse.z += 0.1;
             }
             else if(specular == 1 && light == 1){
                if(light_specular.z < 1.0)
                    light_specular.z += 0.1;
             }
             glutPostRedisplay();
             break;
        }
        case 'b':
        {
             //decrease blue component
             if(ambient == 1 && material == 1){
                if(material_ambient.z > 0.0)
                    material_ambient.z -= 0.1;
             }
             else if(diffuse == 1 && material == 1){
                if(material_diffuse.z > 0.0)
                    material_diffuse.z -= 0.1;
             }
             else if(specular == 1 && material == 1){
                if(material_specular.z > 0.0)
                    material_specular.z -= 0.1;
             }
             else if(ambient == 1 && light == 1){
                if(light_ambient.z > 0.0)
                    light_ambient.z -= 0.1;
             }
             else if(diffuse == 1 && light == 1){
                if(light_diffuse.z > 0.0)
                    light_diffuse.z -= 0.1;
             }
             else if(specular == 1 && light == 1){
                if(light_specular.z > 0.0)
                    light_specular.z -= 0.1;
             }
             glutPostRedisplay();
             break;
        }
    }
}

//----------------------------------------------------------------------------
int
main( int argc, char **argv )
{
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowSize( 512, 512 );
    glutInitContextVersion( 3, 2 );
    glutInitContextProfile( GLUT_CORE_PROFILE );
    //First window
    glutCreateWindow( "Teapot" );
    glewInit();
    init();
    glutDisplayFunc( display );
    //mouse functionality
    glutMouseFunc(myMouse);
    //keyboard functionality
    glutKeyboardFunc(myKey);
  
    glutMainLoop();
    return 0;
}
