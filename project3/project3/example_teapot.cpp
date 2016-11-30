//
// Example of drawing a teapot 
// Part of the code was modified from examples in the textbooks
//

#include "Angel.h"
#include "loadModels.h"
#include "math.h"
 
typedef Angel::vec4 color4;
typedef Angel::vec4 point4;

// the model
vector<vec4> vertices;
vector<vec2> tex_coords;
vector<vec3> normals;

GLfloat scale=1.0;
GLfloat scale_factor = 0.05;
const int  TextureSize  = 64;

GLfloat dx=0;
GLfloat dy=0;
GLfloat dz=0;

GLfloat theta_x=0;
GLfloat theta_y=0;
GLfloat theta_z=0;

color4 ambient_product;
color4 diffuse_product;
color4 specular_product;

float l_amb_red = 0.2, l_amb_blu = 0.2, l_amb_green = 0.2;
float l_diff_red = 1.0, l_diff_blu = 1.0, l_diff_green = 1.0;
float l_spe_red = 1.0, l_spe_blu = 1.0, l_spe_green = 1.0;

float m_amb_red = 1.0, m_amb_blu = 0.0, m_amb_green = 1.0;
float m_diff_red = 1.0, m_diff_blu = 0.8, m_diff_green = 0.0;
float m_spe_red = 1.0, m_spe_blu = 0.8, m_spe_green = 0.0;

float light_position_x = 1.0, light_position_y = 1.0, light_position_z = 1.0;

point4 light_position( light_position_x, light_position_y, light_position_z, 0.0 );
color4 light_ambient( l_amb_red, l_amb_blu, l_amb_green, 1.0 );
color4 light_diffuse( l_diff_red, l_diff_blu, l_diff_green, 1.0 );
color4 light_specular( l_spe_red, l_spe_blu, l_spe_green, 1.0 );
	
color4 material_ambient( m_amb_red, m_amb_blu, m_amb_green, 1.0 );
color4 material_diffuse( m_diff_red, m_diff_blu, m_diff_green, 1.0 );
color4 material_specular( m_spe_red, m_spe_blu, m_spe_green, 1.0 );
float material_shininess = 100.0;

int old_button;
int old_state;
int old_x;
int old_y;

int widthm;
int heightm;

int mouse = 0;
int keyboard = 0;


GLuint program;

GLuint  model_view;  // model-view matrix uniform shader variable location

GLuint  projection; // projection matrix uniform shader variable location

// Texture objects and storage for texture image
GLuint textures[2];

GLubyte image[TextureSize][TextureSize][3];
GLubyte image2[TextureSize][TextureSize][3];


enum { ArrayBuffer, ElementBuffer, NumVertexBuffers };

//
//----------------------------------------------------------------------------

// OpenGL initialization
void
init()
{

    // Create a checkerboard pattern
    for ( int i = 0; i < 64; i++ ) {
        for ( int j = 0; j < 64; j++ ) {
            GLubyte c = (((i & 0x8) == 0) ^ ((j & 0x8)  == 0)) * 255;
            image[i][j][0]  = c;
            image[i][j][1]  = c;
            image[i][j][2]  = c;
            image2[i][j][0] = c;
            image2[i][j][1] = 0;
            image2[i][j][2] = c;
        }
    }

    // Initialize texture objects
    glGenTextures( 2, textures );
    
    glBindTexture( GL_TEXTURE_2D, textures[0] );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, TextureSize, TextureSize, 0, GL_RGB, GL_UNSIGNED_BYTE, image );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

    glBindTexture( GL_TEXTURE_2D, textures[1] );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, TextureSize, TextureSize, 0, GL_RGB, GL_UNSIGNED_BYTE, image2 );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, textures[0] );
    
    // Create a vertex array object
    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );


    // Load a model
    load_obj_tex("../models/teapot_texCor.obj", vertices, tex_coords, normals); // tex_coords = elements (faces)
if ((vertices.size()==0) || (tex_coords.size()==0)|| (normals.size()==0))
    cout << "Failed to read the model"<<endl;
else
    cout<< vertices.size()<<" "<<tex_coords.size()<<endl;

    // Create and initialize a buffer object
	GLuint buffers[NumVertexBuffers];
	
	glGenBuffers( NumVertexBuffers, buffers );

	glBindBuffer( GL_ARRAY_BUFFER, buffers[ArrayBuffer]  );
	glBufferData( GL_ARRAY_BUFFER, vertices.size()*sizeof(vec4) + normals.size()*sizeof(vec3) + tex_coords.size()*sizeof(vec2), NULL, GL_STATIC_DRAW );

	GLintptr offset = 0;

	glBufferSubData(GL_ARRAY_BUFFER,0,vertices.size()*sizeof(vec4), &vertices[0]);
	glBufferSubData(GL_ARRAY_BUFFER,vertices.size()*sizeof(vec4),sizeof(vec3)*normals.size(), &normals[0] );
	glBufferSubData(GL_ARRAY_BUFFER,vertices.size()*sizeof(vec4)+sizeof(vec3)*normals.size(), tex_coords.size()*sizeof(vec2), &tex_coords[0] );

        // Load shaders and use the resulting shader program
        program = InitShader( "vshader_teapot.glsl", "fshader_teapot.glsl" );
        glUseProgram( program );

        // set up vertex arrays
	offset = 0;
	
	GLuint vPosition = glGetAttribLocation( program, "vPosition" );
	glEnableVertexAttribArray( vPosition );
	glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE,0,BUFFER_OFFSET(offset) );

	GLuint vNormal = glGetAttribLocation( program, "vNormal" );
	glEnableVertexAttribArray( vNormal );
	glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vertices.size()*sizeof(vec4)) );

	offset += vertices.size()*sizeof(vec4) +sizeof(vec3)*normals.size() ;
	
	GLuint vTexCoord = glGetAttribLocation( program, "vTexCoord" );
	glEnableVertexAttribArray( vTexCoord );
	glVertexAttribPointer( vTexCoord, 2,GL_FLOAT,GL_FALSE, 0,BUFFER_OFFSET(offset) );
	
	ambient_product  = light_ambient * material_ambient;
	diffuse_product  = light_diffuse * material_diffuse;
	specular_product = light_specular * material_specular;
	
	glUniform4fv( glGetUniformLocation(program, "AmbientProduct"),1, ambient_product );
	glUniform4fv( glGetUniformLocation(program, "DiffuseProduct"),1, diffuse_product );
	glUniform4fv( glGetUniformLocation(program, "SpecularProduct"),1, specular_product );
	glUniform4fv( glGetUniformLocation(program, "LightPosition"),1, light_position );
	glUniform1f( glGetUniformLocation(program, "Shininess"), material_shininess );
	
        // Set the value of the fragment shader texture sampler variable
        //   ("tex") to the the appropriate texture unit. In this case,
        //   zero, for GL_TEXTURE0 which was previously set by calling
        //   glActiveTexture().
        glUniform1i( glGetUniformLocation(program, "tex"), 0 );
	
	// Retrieve transformation uniform variable locations
	model_view = glGetUniformLocation( program, "model_view" );
	projection = glGetUniformLocation( program, "projection" );
	glEnable( GL_DEPTH_TEST );
	
	glShadeModel(GL_FLAT);
	glClearColor( 1.0, 1.0, 1.0, 1.0 ); 
	
}

//----------------------------------------------------------------------------

void
display( void )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
	ambient_product  = light_ambient * material_ambient;
	diffuse_product  = light_diffuse * material_diffuse;
	specular_product = light_specular * material_specular;
	
	glUniform4fv( glGetUniformLocation(program, "AmbientProduct"),1, ambient_product );
	glUniform4fv( glGetUniformLocation(program, "DiffuseProduct"),1, diffuse_product );
	glUniform4fv( glGetUniformLocation(program, "SpecularProduct"),1, specular_product );
	glUniform4fv( glGetUniformLocation(program, "LightPosition"),1, light_position );
	glUniform1f( glGetUniformLocation(program, "Shininess"), material_shininess );
   
    point4  eye( 60, 0.5, 50, 1.0 );
    point4  at( 0.0, 0.0, 0.0, 1.0 );
    vec4    up( 0.0, 0.0, 1.0, 0.0 );

    mat4  mv = LookAt( eye, at, up );

    mat4  mv_scale = mat4(scale, 0.0, 0.0, 0.0,
        0.0, scale, 0.0, 0.0,
        0.0, 0.0, scale, 0.0,
        0.0, 0.0, 0.0, 1.0);

    mat4  rotate_z = mat4(cos(theta_z), -sin(theta_z), 0.0, 0.0,
        sin(theta_z), cos(theta_z), 0.0, 0.0,
        0.0, 0.0, 1, 0.0,
        0.0, 0.0, 0.0, 1);

    mat4  rotate_y = mat4(cos(theta_y), 0.0, sin(theta_y), 0.0,
        0.0, 1, 0.0, 0.0,
        -sin(theta_y), 0.0, cos(theta_y), 0.0,
        0.0, 0.0, 0.0, 1);
        
    mat4  rotate_x = mat4(1, 0.0, 0.0, 0.0,
        0.0, cos(theta_x), -sin(theta_x), 0.0,
        0.0, sin(theta_x), cos(theta_x), 0.0,
        0.0, 0.0, 0.0, 1);
    
    glUniformMatrix4fv( model_view, 1, GL_TRUE, mv*rotate_z*rotate_y*rotate_x*mv_scale*Scale(scale_factor));
       
    mat4  p = Perspective( 90, 1, 0.5, 100 );
    glUniformMatrix4fv( projection, 1, GL_TRUE, p );

    //glDrawElements( GL_TRIANGLES, elements.size()*sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
    glDrawArrays( GL_TRIANGLES, 0, vertices.size());

    glutSwapBuffers();
}


void myMouse (int button, int state, int x, int y)
{

	old_state  = state;
	old_button = button;
    
	if(button == GLUT_LEFT_BUTTON) {
		mouse = 1;
	} 
	if(button == GLUT_RIGHT_BUTTON) {
		mouse = 2;
	}
	if(button == GLUT_MIDDLE_BUTTON) {
		if (x < 256)
			light_position_x = light_position_x - 1;
		if (x >= 256)
			light_position_x = light_position_x + 1;
		if (y < 256)
			light_position_y = light_position_y - 1;
		if (y >= 256)
			light_position_y = light_position_y + 1;
			
		glutPostRedisplay();
		cout<< "X: "<< x << " Y: "<< y <<endl;	
	}
}

void myMouseMotion(int x, int y){
	int distance;
    widthm = glutGet(GLUT_WINDOW_WIDTH);
    heightm = glutGet(GLUT_WINDOW_HEIGHT);
    
    distance = sqrt((x-old_x)*(x-old_x) + (x-old_y)*(x-old_y));
    
	if(old_button == GLUT_LEFT_BUTTON && old_state == GLUT_DOWN) {
		theta_x = (360 * distance / sqrt(widthm*widthm + heightm*heightm)) * M_PI / 180;
	} 
	if(old_button == GLUT_RIGHT_BUTTON && old_state == GLUT_DOWN) {
		theta_y = (360 * distance / sqrt(widthm*widthm + heightm*heightm)) * M_PI / 180; 
	} 
	if(old_button == GLUT_MIDDLE_BUTTON && old_state == GLUT_DOWN) {
		theta_z = (360 * distance / sqrt(widthm*widthm + heightm*heightm)) * M_PI / 180;
	} 
	//glClearColor(0.0f, 0.0f, 0.0f, 0.0f); 
	glutPostRedisplay();
}


void myKey(unsigned char key, int x, int y )
{

    switch (key)
    {
	case 'Q':
	{
	    exit(0);            
	    break; 
	}
	
	case 'q':
	{
	    exit(0);            
	    break; 
	}
        case '1':
            glBindTexture( GL_TEXTURE_2D, textures[0] );
	    glutPostRedisplay();
            break;

        case '2':
            glBindTexture( GL_TEXTURE_2D, textures[1] );
	    glutPostRedisplay();
            break;
	case 'S':
	{
	    //Scale up by 10%
	    if(scale >= -0.1 && scale < 1.0)
	    	scale += 0.1;
	    glutPostRedisplay();
	    break;
	}
	case 's':
	{
	     //scale down by 10%
	     if(scale <= 1.0 && scale > 0.1)
	    	scale -= 0.1;
	     glutPostRedisplay();
	     break;
	}
    }
    
    cout<< "mouse: "<< mouse <<endl;
    cout<< "scale: "<< scale <<endl;
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
    glutCreateWindow( "Teapot" );

    if (glewInit()) {
        cerr << "Unable to initialize GLEW ... exiting" << endl;
        exit(EXIT_FAILURE);
    }

    init();

    glutDisplayFunc( display );
    glutMouseFunc( myMouse );
    glutMotionFunc(myMouseMotion);
	glutKeyboardFunc(myKey);
	//glClearColor(0.0f, 0.0f, 0.0f, 0.0f); 
  
    glutMainLoop();
    return 0;
}
