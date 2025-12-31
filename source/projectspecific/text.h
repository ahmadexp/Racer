float fonttexturecoords[32*3][8];

float textxsize=0.f;
float textysize=0.f;

void setuptext(){
	textxsize=16.f;
	textysize=32.f;
	float xoffset=0.f;
	float yoffset=0.f;
	float textxborder=0.f;
	float textyborder=5.f;
	for(int y=0; y<3; y++)for(int x=0; x<32; x++){//setup the pixel positions
		fonttexturecoords[32*y+x][0]=xoffset+x*textxsize+textxborder;
		fonttexturecoords[32*y+x][1]=yoffset+y*textysize-textyborder+textysize;
		fonttexturecoords[32*y+x][2]=xoffset+x*textxsize-textxborder+textxsize;
		fonttexturecoords[32*y+x][3]=yoffset+y*textysize-textyborder+textysize;
		fonttexturecoords[32*y+x][4]=xoffset+x*textxsize-textxborder+textxsize;
		fonttexturecoords[32*y+x][5]=yoffset+y*textysize+textyborder;
		fonttexturecoords[32*y+x][6]=xoffset+x*textxsize+textxborder;
		fonttexturecoords[32*y+x][7]=yoffset+y*textysize+textyborder;
	}
	for(int a=0; a<32*3; a++){//scale it for the texture
		for(int b=0; b<4; b++){
			fonttexturecoords[a][b*2+0]/=512.f;
			fonttexturecoords[a][b*2+1]/=128.f;
		}
	}
	textxsize-=textxborder*2.f;
	textysize-=textyborder*2.f;
}

void drawtext(float posx,float posy,char* text,float size){
	float xsize=textxsize*size;
	float ysize=textysize*size;
	int length=strlen(text);
	for(int a=0;a<length;a++){
		draw2dquad(
			posx,posy,
			xsize,ysize,
			fonttexturecoords[text[a]-' '],
			0,0.f);
		posx+=xsize*0.65f;
	}
}

void drawtextrightaligned(float posx,float posy,char* text,float size){
	posx-=textxsize*size*0.65f*(float)strlen(text);
	drawtext(posx,posy,text,size);
}

void drawtextcenteraligned(float posx,float posy,char* text,float size){
	posx-=textxsize*size*0.65f*((float)strlen(text)+0.5f)*0.5f;
	drawtext(posx,posy,text,size);
}
