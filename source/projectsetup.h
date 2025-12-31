#include "projectspecific/meshes/track.h"
#include "projectspecific/meshes/trackturnl.h"
#include "projectspecific/meshes/trackturnr.h"

float camposx,camposy,camposz;
float camangx,camangy,camangz;

bool usejoystick=1;
#ifdef _WIN32
JOYINFO joystick;
#endif

float playerposx,playerposy,playerposz,playerposmovx,playerposmovy,playerposmovz;
float playerangx,playerangy,playerangz,playerangmovx,playerangmovy,playerangmovz;

#define maxtracksizex (10)
#define maxtracksizez (10)
int track[maxtracksizex][maxtracksizez]={
	{9, 2, 2, 2, 2, 2, 2, 10,0, 0, },
	{1, 0, 0, 0, 0, 0, 0, 3, 0, 0, },
	{1, 0, 0, 0, 0, 0, 0, 3, 0, 0, },
	{1, 0, 6, 4, 4, 4, 4, 11,0, 0, },
	{1, 0, 3, 0, 0, 0, 0, 0, 0, 0, },
	{1, 0, 5, 2, 2, 2, 2, 2, 2, 10,},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 3, },
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 3, },
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 3, },
	{12,4, 4, 4, 4, 4, 4, 4, 4, 11, },
};
int trackprogress[maxtracksizex][maxtracksizez];
int tracklength=0;

int lapcount;

float laptime;
float bestlaptime;

#include "projectspecific/textures.h"
#include "projectspecific/text.h"
#include "projectspecific/bumpintoworld.h"
#include "projectspecific/play.h"
#include "projectspecific/drawworld.h"
#include "projectspecific/drawhud.h"

void resetgame(){

	playerposx=0;
	playerposy=0.1f;
	playerposz=0;
	playerposmovx=0;
	playerposmovy=0;
	playerposmovz=0;
	playerangx=0;
	playerangy=180.f*radiansindegree;
	playerangz=0;
	playerangmovx=0;
	playerangmovy=0;
	playerangmovz=0;

	camposx=playerposx;
	camposy=playerposy;
	camposz=playerposz;
	camangx=playerangx;
	camangy=playerangy-90.f*radiansindegree;
	camangz=playerangz;

	for(int x=0;x<maxtracksizex;x++)
	for(int z=0;z<maxtracksizez;z++)trackprogress[x][z]=0;

	int progress=0;
	int startx=0;
	int startz=0;
	int x=startx;
	int z=startz;
	while(progress<maxtracksizex*maxtracksizez){
		trackprogress[x][z]=progress;
		tracklength=progress;
		progress++;
		if(x==startx && z==startz && progress>1){
			progress=maxtracksizex*maxtracksizez;
			trackprogress[x][z]=0;
			tracklength-=1;
		}
		else if(track[x][z]==1)x++;
		else if(track[x][z]==2)z--;
		else if(track[x][z]==3)x--;
		else if(track[x][z]==4)z++;
		else if(track[x][z]==5)x--;
		else if(track[x][z]==6)z++;
		else if(track[x][z]==9)x++;
		else if(track[x][z]==10)z--;
		else if(track[x][z]==11)x--;
		else if(track[x][z]==12)z++;
		if(x<0 || x>=maxtracksizex || z<0 || z>=maxtracksizez)
			progress=maxtracksizex*maxtracksizez;
	}

	lapcount=0;

	laptime=0;
	bestlaptime=60*60;

}

void projectsetup(){
    printf("projectsetup: start\n");

	//feed info to the dashboard
	sprintf(dash_bars_gamename,"%s","Racer");
	dash_bars_gameversion=0.1;

	//load the textures
    printf("projectsetup: loading textures\n");
	setuptextures();
    printf("projectsetup: loading text\n");
	setuptext();

	//adjust the track meshes and generate collision meshes
    printf("projectsetup: adjusting track meshes\n");
	for(int a=0;a<trackvertexcount;a++){
		tracktexturearray[a*2+1]=1-tracktexturearray[a*2+1];
		trackvertexarray[a*3+0]/=8;
		trackvertexarray[a*3+1]/=8;
		trackvertexarray[a*3+2]/=8;
	}
	for(int a=0;a<trackturnlvertexcount;a++){
		trackturnltexturearray[a*2+1]=1-trackturnltexturearray[a*2+1];
		trackturnlvertexarray[a*3+0]/=8;
		trackturnlvertexarray[a*3+1]/=8;
		trackturnlvertexarray[a*3+2]/=8;
	}
	for(int a=0;a<trackturnrvertexcount;a++){
		trackturnrtexturearray[a*2+1]=1-trackturnrtexturearray[a*2+1];
		trackturnrvertexarray[a*3+0]/=8;
		trackturnrvertexarray[a*3+1]/=8;
		trackturnrvertexarray[a*3+2]/=8;
	}

	//new game
	resetgame();

}
