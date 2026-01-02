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

	if(showgps){
		glDisable(GL_TEXTURE_2D);
		
		// Draw map
		int mapsize = 15;
		float startx = vieww - (maxtracksizez * mapsize) - 20;
		float starty = viewh - (maxtracksizex * mapsize) - 60; 

		for(int gx=0;gx<maxtracksizex;gx++)
		for(int gz=0;gz<maxtracksizez;gz++){
			if(track[gx][gz]>0){
				if(gx==(int)floor(playerposx+0.5f) && gz==(int)floor(playerposz+0.5f))
					glColor4f(1.f,0.f,0.f,1.f);
				else
					glColor4f(1.f,1.f,1.f,0.5f);
				
				// Map Orientation:
				// Screen X (Right) = World Z. (Normal)
				// Screen Y (Up) = World X. (Inverted Y coordinate to map Up)
				
				float qx = startx + gz*mapsize;
				float qy = starty + (maxtracksizex - 1 - gx)*mapsize;
				float v[] = {qx,qy, qx+mapsize,qy, qx+mapsize,qy+mapsize, qx,qy+mapsize};
				glVertexPointer(2, GL_FLOAT, 0, v);
				glDrawArrays(GL_QUADS, 0, 4);
			}
		}
		
		// Draw player arrow
		glColor4f(0.f,1.f,0.f,1.f);
		float px = startx + playerposz*mapsize + mapsize/2.0f;
		float py = starty + (maxtracksizex - 1 - playerposx)*mapsize + mapsize/2.0f;
		float size = 5.0f;
		
		// Angle Logic:
		// We want Screen X = +Z = Sin(ang).
		// Screen Y = +X = Cos(ang).
		// We want X=Sin(ang), Y=Cos(ang).
		// This implies A = ang - 90 (or similar).
		// Testing: 0 (World +X, Screen Down) -> -90 (Down). OK.
		// Testing: 90 (World +Z, Screen Right) -> 0 (Right). OK.
		// Update: Previous was A - 90, result inverted.
		// Try A + 90.
		
		float ang = playerangy + 1.57079632679f; 
		
		float p1x = px + size * cos(ang);
		float p1y = py + size * sin(ang);
		float p2x = px + size * cos(ang + 2.6f); 
		float p2y = py + size * sin(ang + 2.6f);
		float p3x = px + size * cos(ang - 2.6f);
		float p3y = py + size * sin(ang - 2.6f);

		float v[] = {p1x,p1y, p2x,p2y, p3x,p3y};
		glVertexPointer(2, GL_FLOAT, 0, v);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glEnable(GL_TEXTURE_2D);
		glColor4f(1.f,1.f,1.f,1.f);
	}

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
