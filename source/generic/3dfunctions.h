void set2dcamera(float top,float bottom,float left,float right){
	//disable depth test
	glDisable(GL_DEPTH_TEST);
	//setup the camera
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//apply the top, bottom, left and right settings	
	gluOrtho2D(left,right,bottom,top);
	//glOrtho(left,right,bottom,top,-1.f,1.f);

}

void draw2dquad(float posx,float posy,float sizex,float sizey,float *texturecoords,bool centerandrotate=0,float angle=0){
	if(centerandrotate==0){
		float verts[]={
			posx,		posy+sizey,
			posx+sizex,	posy+sizey,
			posx+sizex,	posy,
			posx,		posy,};
		glVertexPointer(2,GL_FLOAT,0,verts);
		glTexCoordPointer(2,GL_FLOAT,0,texturecoords);
		glDrawArrays(GL_QUADS,0,4);
	}else if(angle==0){
		sizex*=0.5f;
		sizey*=0.5f;
		float verts[]={
			posx-sizex,	posy+sizey,
			posx+sizex,	posy+sizey,
			posx+sizex,	posy-sizey,
			posx-sizex,	posy-sizey,};
		glVertexPointer(2,GL_FLOAT,0,verts);
		glTexCoordPointer(2,GL_FLOAT,0,texturecoords);
		glDrawArrays(GL_QUADS,0,4);
	}else{
		sizex*=0.5f;
		sizey*=0.5f;
		float verts[]={
			0-sizex,	0+sizey,
			0+sizex,	0+sizey,
			0+sizex,	0-sizey,
			0-sizex,	0-sizey,};
		glPushMatrix();
		glTranslatef(posx,posy,0);
		glRotatef((GLfloat)angle*(GLfloat)degreesinradian,(GLfloat)0.f,(GLfloat)0.f,(GLfloat)1.f);
		glVertexPointer(2,GL_FLOAT,0,verts);
		glTexCoordPointer(2,GL_FLOAT,0,texturecoords);
		glDrawArrays(GL_QUADS,0,4);
		glPopMatrix();
	}
}

void savecammatrix(){
	glPushMatrix();
}

void loadcammatrix(){
	glPopMatrix();
}

void translatecam(float x,float y,float z){
	glTranslatef(x,y,z);
}

void rotatecam(float angle,float x,float y,float z){
	glRotatef(angle,x,y,z);
}

void scalecam(float x,float y,float z){
	glScalef(x,y,z);
}

void set3dcamera(float posx,float posy,float posz,float xang,float yang,float zang,float fov,float aspect,float nearrange,float farrange){
	//enable depth test
	glEnable(GL_DEPTH_TEST);
	//setup the camera
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov,aspect,nearrange,farrange);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//rotate
	rotatecam(xang*(float)degreesinradian,1.f,0.f,0.f);
	rotatecam(yang*(float)degreesinradian,0.f,1.f,0.f);
	rotatecam(zang*(float)degreesinradian,0.f,0.f,1.f);
	//position
	translatecam(-posx,-posy,-posz);
}


void draw3dquadmesh(int vertexcount,float *verts,float *texturecoords,float *colors = NULL,float *normals = NULL){
	glVertexPointer(3,GL_FLOAT,0,verts);
	glTexCoordPointer(2,GL_FLOAT,0,texturecoords);
	if(colors!=NULL)glColorPointer(3,GL_FLOAT,0,colors);
	if(normals!=NULL)glNormalPointer(GL_FLOAT,0,normals);
	glDrawArrays(GL_QUADS,0,vertexcount);
}

void draw3dtrianglemesh(int vertexcount,float *verts,float *texturecoords,float *colors = NULL,float *normals = NULL){
	glVertexPointer(3,GL_FLOAT,0,verts);
	glTexCoordPointer(2,GL_FLOAT,0,texturecoords);
	if(colors!=NULL)glColorPointer(3,GL_FLOAT,0,colors);
	if(normals!=NULL)glNormalPointer(GL_FLOAT,0,normals);
	glDrawArrays(GL_TRIANGLES,0,vertexcount);
}
