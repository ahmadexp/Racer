void drawhud(){

	float vieww=500*screena, viewh=500;
	set2dcamera(0,viewh,0,vieww);

	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);

	glBindTexture(GL_TEXTURE_2D,fonttexture);

	//hud text
	char temptext[256];
	int x=25;
	int y=25;

	//sprintf(temptext,"%s%f","frames per second: ",dash_framerate);
	//drawtext(x,y,(char*)temptext,1);y+=20;

	if(dashshowframerate){
		sprintf(temptext,"%s%i%s%i","track position: ",
			trackprogress[(int)floor(playerposx+0.5f)][(int)floor(playerposz+0.5f)],
			"/",tracklength);
		drawtext(x,y,(char*)temptext,1);y+=20;

		sprintf(temptext,"%s%i","laps completed: ",lapcount);
		drawtext(x,y,(char*)temptext,1);y+=20;

		sprintf(temptext,"%s%.2f%s","time:      ",laptime," seconds");
		drawtext(x,y,(char*)temptext,1);y+=20;

		sprintf(temptext,"%s%f%s","best time: ",bestlaptime," seconds");
		drawtext(x,y,(char*)temptext,1);y+=20;
	}

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);

}
