void drawworld(){

	//cam stats
	//float camfov=80.f;
	float camnear=0.01f;
	float camfar=100.f;

	//prep for drawing
	if(texmode)glEnable(GL_TEXTURE_2D);
	else glDisable(GL_TEXTURE_2D);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	//set the camera
	set3dcamera(0.f,0.f,0.f,0.f,0.f,0.f,camfov,screena,camnear,camfar);
	//	camposx,camposy,camposz,camangx,camangy,camangz,
	//	camfov,screena,camnear,camfar);
	rotatecam(camangz*degreesinradian,0.f,0.f,1.f);
	rotatecam(camangx*degreesinradian,1.f,0.f,0.f);
	rotatecam(camangy*degreesinradian,0.f,1.f,0.f);
	translatecam(-camposx,-camposy,-camposz);

	//disable culling to ensure track is visible even if we are inside
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	glColor4f(1.f,1.f,1.f,1.f);

	//draw the track
	glBindTexture(GL_TEXTURE_2D,tracktexture);
	for(int x=0;x<maxtracksizex;x++)
	for(int z=0;z<maxtracksizez;z++){
		if(track[x][z]>0 && track[x][z]<5){
			savecammatrix();
			translatecam(x,0.f,z);
			if(track[x][z]==1)rotatecam(90.f,0.f,1.f,0.f);
			if(track[x][z]==2)rotatecam(180.f,0.f,1.f,0.f);
			if(track[x][z]==3)rotatecam(270.f,0.f,1.f,0.f);
			if(track[x][z]==4)rotatecam(0.f,0.f,1.f,0.f);
			draw3dtrianglemesh(
				trackvertexcount,
				trackvertexarray,
				tracktexturearray,NULL,NULL);
			loadcammatrix();
		}
		if(track[x][z]>4 && track[x][z]<9){
			savecammatrix();
			translatecam(x,0.f,z);
			if(track[x][z]==5)rotatecam(180,0,1,0);
			if(track[x][z]==6)rotatecam(270,0,1,0);
			if(track[x][z]==7)rotatecam(0,0,1,0);
			if(track[x][z]==8)rotatecam(90,0,1,0);
			draw3dtrianglemesh(
				trackturnlvertexcount,
				trackturnlvertexarray,
				trackturnltexturearray,NULL,NULL);
			loadcammatrix();
		}
		if(track[x][z]>8 && track[x][z]<13){
			savecammatrix();
			translatecam(x,0.f,z);
			if(track[x][z]==9)rotatecam(180,0,1,0);
			if(track[x][z]==10)rotatecam(270,0,1,0);
			if(track[x][z]==11)rotatecam(0,0,1,0);
			if(track[x][z]==12)rotatecam(90,0,1,0);
			draw3dtrianglemesh(
				trackturnrvertexcount,
				trackturnrvertexarray,
				trackturnrtexturearray,NULL,NULL);
			loadcammatrix();
		}
	}

	//done drawing
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_TEXTURE_2D);

}
