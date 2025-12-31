//define maxpathlen for a file
#define MAXPATHLEN 1024

//super ultra mega generic includes
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <malloc.h>
#include <windows.h>
#pragma comment(lib,"Winmm.lib")
#else
#include <unistd.h>
#endif

#include <SDL.h>
//#include <SDL_image.h>
//#include <SDL_mixer.h>
#include <SDL_net.h>
#include <SDL_opengl.h>
#include <SDL_video.h>
#include <vector>
#include <iostream>
using std::cerr;
using std::cout;
using std::endl;
#include <fstream>
using std::ofstream;
using std::ifstream;
#include <cstdlib>
#include <string>

//super generic
#include "genericmath.h"
#include "normals.h"
#include "filesystem.h"
#include "readinifiles.h"
#include "genericsettings.h"
#include "initvideo.h"
#include "input.h"
#include "bresenham.h"
#include "3dfunctions.h"
#include "refreshscreen.h"

#include "dirlist.h"
#include "loadobj.h"
#include "saveobj.h"
#include "3DMath.h"
#include "meshbump.h"
#include "frustum.h"
#include "lodepng.h"
#include "msviewer.h"

//specific to SDL surfaces
#include "sdldraw/loadanddrawsurfaces.h"
#include "sdldraw/getputpixel.h"
#include "sdldraw/drawcircle.h"
#include "sdldraw/drawline.h"
#include "sdldraw/drawpolygon.h"
#include "sdldraw/drawhexagon.h"
#include "sdldraw/filtersurface.h"
#include "sdldraw/rotoscale.h"
#include "sdldraw/savesurface.h"

//specific to OpenGL
#include "opengl/loadtexture.h"

//the dashboard and related functions
#include "dashboard/dashsetup.h"
#include "dashboard/dashloop.h"

void genericsetup(){
	//seed the random number generator
	srand((int)time(NULL));
	//setup the screen or window
	initvideo();
}
