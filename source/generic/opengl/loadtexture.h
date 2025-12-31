GLuint maketexturefromsurface(SDL_Surface *surface,bool mipmap){

	GLuint texture;
	GLenum texture_format;
	GLint  nOfColors;
	
	nOfColors = surface->format->BytesPerPixel;
	if(nOfColors == 4){//contains an alpha channel
		if(surface->format->Rmask == 0x000000ff)
			texture_format = GL_RGBA;
		else
			texture_format = GL_BGRA;
	}else if(nOfColors == 3){//no alpha channel
		if(surface->format->Rmask == 0x000000ff)
			texture_format = GL_RGB;
		else
			texture_format = GL_BGR;
	}
	glGenTextures( 1, &texture );
	glBindTexture( GL_TEXTURE_2D, texture );
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	if(mipmap){
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR);
		gluBuild2DMipmaps(GL_TEXTURE_2D,nOfColors,surface->w,surface->h,
			texture_format,GL_UNSIGNED_BYTE,surface->pixels);
	}else{
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D,0,nOfColors,surface->w,surface->h,0,
			texture_format,GL_UNSIGNED_BYTE,surface->pixels);
	}
	
	return texture;

}

GLuint loadtexture(char* filename,bool mipmap){

	GLuint texture = 0;
	SDL_Surface *surface = NULL;
	GLenum texture_format;
	GLint  nOfColors;
	
	printf("loadtexture: Loading %s\n", filename);
	if((surface = LoadImageFromFile(filename))){ 
		printf("loadtexture: Successfully loaded surface for %s, w=%d h=%d bpp=%d\n", filename, surface->w, surface->h, surface->format->BytesPerPixel); 
		nOfColors = surface->format->BytesPerPixel;
		if(nOfColors == 4){//contains an alpha channel
			if(surface->format->Rmask == 0x000000ff)
				texture_format = GL_RGBA;
			else
				texture_format = GL_BGRA;
		}else if(nOfColors == 3){//no alpha channel
			if(surface->format->Rmask == 0x000000ff)
				texture_format = GL_RGB;
			else
				texture_format = GL_BGR;
		}
		glGenTextures( 1, &texture );
		glBindTexture( GL_TEXTURE_2D, texture );
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
		if(mipmap){
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR);
            // Use GL_GENERATE_MIPMAP instead of gluBuild2DMipmaps which hangs
            glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
			glTexImage2D(GL_TEXTURE_2D,0,nOfColors,surface->w,surface->h,0,
				texture_format,GL_UNSIGNED_BYTE,surface->pixels);
		}else{
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D,0,nOfColors,surface->w,surface->h,0,
				texture_format,GL_UNSIGNED_BYTE,surface->pixels);
		}
	}
	
	if(surface) SDL_FreeSurface( surface);

	printf("loadtexture: returning texture ID %d for %s\n", texture, filename);
	return texture;

}
