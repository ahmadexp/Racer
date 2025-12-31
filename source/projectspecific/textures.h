GLuint tracktexture = NULL;
GLuint fonttexture = NULL;

void setuptextures(){
	//load textures
	tracktexture=loadtexture((char*)"Media/tracktexture.png",mipmapping);
	fonttexture=loadtexture((char*)"Media/fonttexture.png",mipmapping);
}

