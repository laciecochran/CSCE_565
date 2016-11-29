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
vector<GLushort> elements;
vector<vec3> normals;
vector<vec2> textCors;

GLfloat scale=1.0;

GLfloat tscale=0.3;

GLfloat dx=0;
GLfloat dy=0;
GLfloat dz=0;

GLfloat theta_x=0;
GLfloat theta_y=0;
GLfloat theta_z=0;

float l_amb_red = 0.2, l_amb_blu = 0.2, l_amb_green = 0.2;
float l_diff_red = 1.0, l_diff_blu = 1.0, l_diff_green = 1.0;
float l_spe_red = 1.0, l_spe_blu = 1.0, l_spe_green = 1.0;

float m_amb_red = 1.0, m_amb_blu = 0.0, m_amb_green = 1.0;
float m_diff_red = 1.0, m_diff_blu = 0.8, m_diff_green = 0.0;
float m_spe_red = 1.0, m_spe_blu = 0.8, m_spe_green = 0.0;

float light_position_x = 1.0, light_position_y = 1.0, light_position_z = 1.0;

int old_button;
int old_state;
int old_x;
int old_y;

int widthm;
int heightm;

int mouse = 0;
int keyboard = 0;

GLfloat radius = 1.0;
GLfloat theta = 0.0;
GLfloat phi = 0.0;

GLfloat  the_left = -1.0, the_right = 1.0;
GLfloat  bottom = -1.0, top = 1.0;
GLfloat zNear = 0.5, zFar = 3.0;

//black/white checkerboard
float pixels[] = { 0.0f, 0.0f, 0.0f,   1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,   0.0f, 0.0f, 0.0f};

//GLuint program;

GLuint  ModelView;  // model-view matrix uniform shader variable location

GLuint  Projection; // projection matrix uniform shader variable location


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
    //texture generation
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_FLOAT, pixels);


    // Load a model
    //load_obj("../models/teapot.obj", vertices,elements,normals);
    load_obj_tex("../models/teapot_texCor.obj", vertices, textCors, normals);
    //if ((vertices.size()==0) || (elements.size()==0)|| (normals.size()==0))
    if ((vertices.size()==0) || (textCors.size()==0)|| (normals.size()==0))
        cout << "Failed to read the model"<<endl;
    else
        cout<< vertices.size()<<" "<<elements.size()<<endl;

    // Create and initialize a buffer object

    GLuint buffers[NumVertexBuffers];

    glGenBuffers( NumVertexBuffers, buffers );

    glBindBuffer( GL_ARRAY_BUFFER, buffers[ArrayBuffer]  );
    glBufferData( GL_ARRAY_BUFFER, vertices.size()*sizeof(vec4) + normals.size()*sizeof(vec3), NULL, GL_STATIC_DRAW );

    //glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, buffers[ElementBuffer] );
    //glBufferData( GL_ELEMENT_ARRAY_BUFFER, elements.size()*sizeof(GLushort), &elements[0], GL_STATIC_DRAW );


    glBufferSubData(GL_ARRAY_BUFFER,0,vertices.size()*sizeof(vec4), &vertices[0]);
    glBufferSubData(GL_ARRAY_BUFFER,vertices.size()*sizeof(vec4),sizeof(vec3)*normals.size(), &normals[0] );

   // Load shaders and use the resulting shader program
   GLuint program = InitShader( "vshader_teapot.glsl", "fshader_teapot.glsl" );
   glUseProgram( program );

   // set up vertex arrays
   GLuint vPosition = glGetAttribLocation( program, "vPosition" );
   glEnableVertexAttribArray( vPosition );
   glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

	GLuint vNormal = glGetAttribLocation( program, "vNormal" );
	glEnableVertexAttribArray( vNormal );
	glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vertices.size()*sizeof(vec4)) );

	point4 light_position( light_position_x, light_position_y, light_position_y, 0.0 );
	color4 light_ambient( l_amb_red, l_amb_blu, l_amb_green, 1.0 );
	color4 light_diffuse( l_diff_red, l_diff_blu, l_diff_green, 1.0 );
	color4 light_specular( l_spe_red, l_spe_blu, l_spe_green, 1.0 );

	color4 material_ambient( m_amb_red, m_amb_blu, m_amb_green, 1.0 );
	color4 material_diffuse( m_diff_red, m_diff_blu, m_diff_green, 1.0 );
	color4 material_specular( m_spe_red, m_spe_blu, m_spe_green, 1.0 );
	float material_shininess = 100.0;

	color4 ambient_product  = light_ambient * material_ambient;
	color4 diffuse_product  = light_diffuse * material_diffuse;
	color4 specular_product = light_specular * material_specular;

	glUniform4fv( glGetUniformLocation(program, "AmbientProduct"),1, ambient_product );
	glUniform4fv( glGetUniformLocation(program, "DiffuseProduct"),1, diffuse_product );
	glUniform4fv( glGetUniformLocation(program, "SpecularProduct"),1, specular_product );
	glUniform4fv( glGetUniformLocation(program, "LightPosition"),1, light_position );
	glUniform1f( glGetUniformLocation(program, "Shininess"), material_shininess );

	// Retrieve transformation uniform variable locations
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
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    point4  eye( 1.0, 5.0, 4.0, 1.0 );
    point4  at( 0.0, 0.0, 0.0, 1.0 );
    vec4    up( 0.0, 1.0, 0.0, 0.0 );

    mat4  mv_scale = mat4(scale, 0.0, 0.0, 0.0,
	0.0, scale, 0.0, 0.0,
	0.0, 0.0, scale, 0.0,
	0.0, 0.0, 0.0, 1.0);
     mat4  mv_rx = mat4(1.0, 0.0, 0.0, 0.0,
	0.0, cos(theta_x), -sin(theta_x), 0.0,
	0.0, sin(theta_x), cos(theta_x), 0.0,
	0.0, 0.0, 0.0, 1.0);
     mat4  mv_ry = mat4(cos(theta_y), 0.0, sin(theta_y), 0.0,
	0.0, 1.0, 0.0, 0.0,
	-sin(theta_y), 0.0, cos(theta_y), 0.0,
	0.0, 0.0, 0.0, 1.0);
     mat4  mv_rz = mat4(cos(theta_z), -sin(theta_z), 0.0, 0.0,
	sin(theta_z), cos(theta_z), 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0);

    mat4  mv = LookAt( eye, at, up );
    
    glUniformMatrix4fv( ModelView, 1, GL_TRUE, mv*mv_rz*mv_ry*mv_rx*mv_scale );
       
    mat4  p = Perspective( 90, 1, 0.5, 8 );
    glUniformMatrix4fv( Projection, 1, GL_TRUE, p );

    glDrawElements( GL_TRIANGLES, elements.size()*sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
    glutSwapBuffers();
}


void myMouse (int button, int state, int x, int y)
{
    
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
		
		init();
                glutPostRedisplay();
		//cout<< "X: "<< x << "Y: "<< y <<endl;
	}
        old_button = button;
        old_state = state;
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
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); 
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
    cout<< "keyboard: "<< keyboard <<endl;
    
/*    cout<< "l_amb_red: "<< l_amb_red << "  l_amb_blu: "<< l_amb_blu << "  l_spe_red: "<< l_amb_green <<endl;
    cout<< "l_diff_red: "<< l_diff_red << "  l_diff_blu: "<< l_diff_blu << "  l_diff_green: "<< l_diff_green <<endl;
    cout<< "l_spe_red: "<< l_spe_red << "  l_spe_blu: "<< l_spe_blu << "  l_spe_green: "<< l_spe_green <<endl;
    cout<< endl;
    cout<< "m_amb_red: "<< m_amb_red << "  m_amb_blu: "<< m_amb_blu << "  m_spe_red: "<< m_amb_green <<endl;
    cout<< "m_diff_red: "<< m_diff_red << "  m_diff_blu: "<< m_diff_blu << "  m_diff_green: "<< m_diff_green <<endl;
    cout<< "m_spe_red: "<< m_spe_red << "  m_spe_blu: "<< m_spe_blu << "  m_spe_green: "<< m_spe_green <<endl;*/
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
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); 
  
    glutMainLoop();
    return 0;
}
