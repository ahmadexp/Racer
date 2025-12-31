//these variables should always exist
float screenw;
float screenh;
float screena;
float screenfov;
int screenbpp;

bool fullscreen;
bool showcursor;
bool antialiasing;
bool mipmapping;
bool usesdl;
bool useopengl;
bool usegx;
bool usersx;



//set the settings
void loadgenericsettings(){

	//load the settings ini
	loadinifile((char*)"settings.ini");

	//load read the setttings
	screenw=readfloatfromini((char*)"screenw");
	screenh=readfloatfromini((char*)"screenh");
	screenbpp=(int)readfloatfromini((char*)"screenbpp");

	fullscreen=(bool)readfloatfromini((char*)"fullscreen");
	showcursor=(bool)readfloatfromini((char*)"showcursor");
	antialiasing=(bool)readfloatfromini((char*)"antialiasing");
	mipmapping=(bool)readfloatfromini((char*)"mipmapping");

	//calculate the screens aspect ratio
	screena = screenw/screenh;

}
