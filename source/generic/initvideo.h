SDL_Surface *screen;

void initsdl(){
	SDL_Init(SDL_INIT_EVERYTHING);
	if(fullscreen==1)
		screen = SDL_SetVideoMode((int)screenw,(int)screenh,screenbpp,SDL_FULLSCREEN);
	if(fullscreen==0)
		screen = SDL_SetVideoMode((int)screenw,(int)screenh,screenbpp,SDL_DOUBLEBUF);
	SDL_WM_SetCaption( "Kinetica Engine", NULL );
}

void initopengl(){
    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) exit(1);
    //Window Settings
	SDL_WM_SetCaption( "Kinetica Engine", NULL );
	//enable anti aliasing
	if(antialiasing){
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,4);
		// glEnable calls moved after SetVideoMode
	}
	//Set Pixel Format
	// SDL_GL_SetAttribute(SDL_GL_RED_SIZE,5);
	// SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,5);
	// SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,5);
	// SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);

	//Create Video Surface
	//there seems to be a problem with opengl plus a screen width or height of zero
	if(screenw==0 || screenh==0){
		screen = SDL_SetVideoMode((int)screenw,(int)screenh,screenbpp,SDL_DOUBLEBUF);
		screenw=(float)screen->w;
		screenh=(float)screen->h;
		screena = screenw/screenh;
		SDL_FreeSurface(screen);
	}

	if(fullscreen==1)
		screen = SDL_SetVideoMode((int)screenw,(int)screenh,screenbpp,SDL_OPENGL|SDL_FULLSCREEN);
	if(fullscreen==0)
		screen = SDL_SetVideoMode((int)screenw,(int)screenh,screenbpp,SDL_OPENGL);
	if (!screen) { SDL_Quit(); exit(3); }

	//Size OpenGL to Video Surface
	glViewport(0, 0, (GLsizei)screenw, (GLsizei)screenh);
    
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(screenfov,screena,1.0,1000.0);
	glMatrixMode(GL_MODELVIEW);
    if(antialiasing){
		glEnable(GL_MULTISAMPLE);
		glHint(GL_LINE_SMOOTH_HINT,GL_FASTEST);
		glHint(GL_POLYGON_SMOOTH_HINT,GL_FASTEST);
		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_POLYGON_SMOOTH);
	}
	//OpenGL Render Settings
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glClearDepth(1000.0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glShadeModel(GL_SMOOTH);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	glDisable(GL_CULL_FACE); // Debug: disable culling
	glFrontFace(GL_CCW);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glAlphaFunc(GL_GREATER,0.0f);
}


void initvideo(){
	//fov is 45 by default
	screenfov=45;
	//the setup
	initopengl();

	screenw = (float)screen->w;
	screenh = (float)screen->h;
	screena = screenw/screenh;
	screenbpp = screen->format->BitsPerPixel;

	SDL_ShowCursor(showcursor);
	
}
