void clearscreen(){
	if(useopengl)glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void drawscreen(){
	SDL_GL_SwapBuffers();
}
