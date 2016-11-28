//////////////////////////////////////////////////////////////////////////////
//
//  --- loadModels.cpp ---
//  --- The code is modified from
//https://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_Load_OBJ
//An obj file is structured in lines. 
// The lines starting with # are comments
// o introduces a new object
// For each following line,
// v introduces a vertex
// vn introduces a normal
// f introduces a face, using vertex indices, starting at 1
//
// loadModels() is used to load an existing model stored as an
// obj file and returns the vertices, normals, or faces.
//////////////////////////////////////////////////////////////////////////////

#include "loadModels.h"


void load_obj(const char* filename, vector<vec4>& vertices, vector<GLushort>& elements,vector<vec3>& normals)
{

    ifstream in(filename);
    if (!in)
    {
        cerr << "Cannot open " << filename << endl; exit(1);
    }

    string line;
    while (getline(in, line))
    {
        // load the vertices
    if (line.substr(0,2) == "v ") 
        {
            istringstream s(line.substr(2));
            vec4 v; s >> v.x; s >> v.y; s >> v.z; v.w = 1.0f;
            vertices.push_back(v);
        }
        //load the faces
        else if (line.substr(0,2) == "f ") // 
        {
            istringstream s(line.substr(2));
            GLushort a,b,c;
            s >> a; s >> b; s >> c;
            a--; b--; c--;
           elements.push_back(a); elements.push_back(b); elements.push_back(c);
        }
        else if (line[0] == '#')
        {
            /* ignoring this line */
        }
        else
        {
            /* ignoring this line */
        }
    }

 
    normals.resize(vertices.size(), vec3(0.0, 0.0, 0.0));


    for (unsigned i = 0; i < elements.size(); i+=3)
    {
        GLushort ia = elements[i];
        GLushort ib = elements[i+1];
        GLushort ic = elements[i+2];
    
        vec3 normal = normalize(cross(vertices[ib]- vertices[ia],vertices[ic]-vertices[ia]));
        normals[ia] = normals[ib] = normals[ic] = normal;
    }
    
}

void load_obj_tex(const char* filename, vector<vec4>& vertices, vector<vec2>& tex_coords,vector<vec3>& normals)
{
    vector<vec4> list_vertices;
    vector<vec2> list_tex_coords;
    vector<vec3> list_normals;
    vector<vec3> faces;

    ifstream in(filename);
    if (!in)
    {
        cerr << "Cannot open " << filename << endl; exit(1);
    }

    string line;
    while (getline(in, line))
    {
        // load the vertices
    if (line.substr(0,2) == "v ")  
        {
            istringstream s(line.substr(2));
            vec4 v; s >> v.x; s >> v.y; s >> v.z; v.w = 1.0f;
            list_vertices.push_back(v);
        }
        // load the normals
        else if (line.substr(0,2) == "vn") 
        {
            istringstream s(line.substr(2));
            vec3 v; s >> v.x; s >> v.y; s >> v.z;             			 
            list_normals.push_back(v);
        }
        // load the texture coordinates
        else if (line.substr(0,2) == "vt") 
        {
            istringstream s(line.substr(2));
            vec2 v; s >> v.x; s >> v.y;              			           		 
            list_tex_coords.push_back(v);
        }
        //load the faces
        else if (line.substr(0,2) == "f ") // 
        {
            istringstream s(line.substr(2));
            GLushort a,b,c;
		 vec3 v;
		 for(unsigned i=0;i<3;i++)
		 { 
              s >> a;
              s.ignore(1);
              s >> b; 
              s.ignore(1);
              s >> c;
              a--; b--; c--;              
              v.x = a;v.y = b;v.z = c;
 		   faces.push_back(v);
            }
        }
        else if (line[0] == '#')
        {
            /* ignoring this line */
        }
        else
        {
            /* ignoring this line */
        }
    }

	vertices.resize(faces.size(), vec4(0.0, 0.0, 0.0, 1.0));
	tex_coords.resize(faces.size(), vec2(0.0, 0.0)); 
	normals.resize(faces.size(), vec3(0.0, 0.0, 0.0));
      
	for (unsigned i = 0; i < faces.size(); i++)
     {
		GLushort indv= faces[i].x;
		vertices[i].x = list_vertices[indv].x;
		vertices[i].y = list_vertices[indv].y;
		vertices[i].z = list_vertices[indv].z;
		vertices[i].w = list_vertices[indv].w;

		GLushort indt= faces[i].y;
		tex_coords[i].x = list_tex_coords[indt].x;
		tex_coords[i].y = list_tex_coords[indt].y;

		GLushort indn= faces[i].z;
		normals[i].x = list_normals[indn].x;
		normals[i].y = list_normals[indn].y;
		normals[i].z = list_normals[indn].z;
     }
}

