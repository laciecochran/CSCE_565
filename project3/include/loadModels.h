//////////////////////////////////////////////////////////////////////////////
//
//  --- loadModels.h ---
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

#include <fstream>
#include <sstream>
#include <vector>
#include "Angel.h"
#include <GL/gl.h>

using namespace std;

void load_obj(const char* filename,vector<vec4>& vertices, vector<GLushort>& elements, vector<vec3>& normals);
