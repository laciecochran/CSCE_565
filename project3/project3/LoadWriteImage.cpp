/* Read and write PPM image file */

#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <ctime>
#include <GL/glut.h>
#include "Image565.h"
using namespace std;

const int MaxWidth=1024;
const int MaxHeight=1024;

char InputColorImage[64]="input.ppm";
char OutputColorImage[64]="input_warp.ppm";
static GLubyte InputColorData[MaxHeight][MaxWidth][4];
static GLubyte OutputColorData[MaxHeight][MaxWidth][4];

int ImageHeight;
int ImageWidth;
unsigned char Alpha=100;

/* This routine reads the data from a (color) PPM file */
int LoadPPMImage(char* filename, unsigned char alpha, unsigned char pdata[MaxHeight][MaxWidth][4])
{
    ByteImage texture;
	
    texture.ReadPnmFile(filename);
    ImageHeight=texture.NumRows();
    ImageWidth=texture.NumCols();
	
    for(int i=0; i<ImageHeight; ++i)
    {
        for(int j=0; j<ImageWidth; ++j)
        {
            pdata[i][j][0]=texture.GetPixel(0,i,j);
            pdata[i][j][1]=texture.GetPixel(1,i,j);
            pdata[i][j][2]=texture.GetPixel(2,i,j);
            pdata[i][j][3]=alpha;
        }
    }
    return 0;
}

/* This routine writes the data of pdata array into a (color) PPM file. */
int WritePPMImage(char* filename, unsigned char pdata[MaxHeight][MaxWidth][4])
{
    ByteImage texture(3, ImageHeight, ImageWidth);
	
    for(int i=0; i<ImageHeight; ++i)
    {
        for(int j=0; j<ImageWidth; ++j)
        {
            texture.SetPixel(0,i,j,pdata[i][j][0]);
            texture.SetPixel(1,i,j,pdata[i][j][1]);
            texture.SetPixel(2,i,j,pdata[i][j][2]);
        }
    }
    texture.WritePnmFile(filename, IO_PPM, 0);
    return 0;
}

int main(int argc, char** argv)
{
    int i, j;
	
    LoadPPMImage(InputColorImage, Alpha, InputColorData);
	
    cout << "Start image warping ..." << endl;
	
    srand(time(NULL)); //sets the random seed to be used with the rand() function
	
    for(i=0; i<ImageHeight; ++i)
    {
        for(j=0; j<ImageWidth; ++j)
        {
            OutputColorData[i][j][0]=InputColorData[i][j][0];
            OutputColorData[i][j][1]=InputColorData[i][j][1];
            OutputColorData[i][j][2]=InputColorData[i][j][2];
        }
    }
	
    WritePPMImage(OutputColorImage,OutputColorData);
    return 0;
}
