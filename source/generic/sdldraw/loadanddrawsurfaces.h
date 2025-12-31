#include <algorithm>
#include <string>

// Helper to check extension
bool hasEnding(std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        std::string sub = fullString.substr(fullString.length() - ending.length());
        std::transform(sub.begin(), sub.end(), sub.begin(), ::tolower);
        std::string endingLow = ending; 
        std::transform(endingLow.begin(), endingLow.end(), endingLow.begin(), ::tolower);
        return (0 == sub.compare(endingLow));
    } else {
        return false;
    }
}

// Replacement for IMG_Load using lodepng for PNGs and SDL_LoadBMP for BMPs
SDL_Surface* LoadImageFromFile(const char* file) {
    std::string filename = file;
    if (hasEnding(filename, ".png")) {
        std::vector<unsigned char> image;
        unsigned width, height;
        unsigned error = LodePNG::decode(image, width, height, filename);
        if(error) {
            printf("LodePNG error %u: %s for file %s\n", error, LodePNG_error_text(error), file);
            return NULL;
        }

        // LodePNG decodes to RGBA (4 bytes).
        // Create an SDL Surface.
        // SDL 1.2 masks for RGBA (Big Endian vs Little Endian usually handled, but lodepng is always RGBA order in byte array)
        // In memory: R, G, B, A.
        // For Little Endian machine (x86), 0xABGR (A is MSB).
        
        Uint32 rmask, gmask, bmask, amask;
        #if SDL_BYTEORDER == SDL_BIG_ENDIAN
            rmask = 0xff000000;
            gmask = 0x00ff0000;
            bmask = 0x0000ff00;
            amask = 0x000000ff;
        #else
            rmask = 0x000000ff;
            gmask = 0x0000ff00;
            bmask = 0x00ff0000;
            amask = 0xff000000;
        #endif

        SDL_Surface* surface = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32, rmask, gmask, bmask, amask);
        if(!surface) return NULL;

        // Copy pixels
        // SDL Surface pixels might have padding (pitch), lodepng is tight.
        // Copy row by row.
        if (SDL_MUSTLOCK(surface)) SDL_LockSurface(surface);
        
        if (image.empty()) {
            printf("LodePNG decoded empty image: %s\n", file);
            SDL_FreeSurface(surface);
            return NULL;
        }

        unsigned char* src = &image[0];
        unsigned char* dst = (unsigned char*)surface->pixels;
        for(unsigned y = 0; y < height; y++) {
             memcpy(dst + y * surface->pitch, src + y * width * 4, width * 4);
        }
        
        if (SDL_MUSTLOCK(surface)) SDL_UnlockSurface(surface);
        
        return surface;
    } else {
        // Fallback to BMP
        return SDL_LoadBMP(file);
    }
}

SDL_Surface *IMG_LoadOptimize( std::string filename ){
	SDL_Surface* loadedImage = NULL;
	SDL_Surface* optimizedImage = NULL; 
	loadedImage = LoadImageFromFile( filename.c_str() ); 
	if( loadedImage != NULL ){
		optimizedImage = SDL_DisplayFormat( loadedImage );
		SDL_FreeSurface( loadedImage );
	} else {
        printf("Failed to load image: %s\n", filename.c_str());
    }
	return optimizedImage;
}

SDL_Surface *IMG_LoadOptimizeWithColorKey( std::string filename ){
	SDL_Surface* loadedImage = NULL;
	SDL_Surface* optimizedImage = NULL; 
	loadedImage = LoadImageFromFile( filename.c_str() ); 
	if( loadedImage != NULL ){
		optimizedImage = SDL_DisplayFormat( loadedImage );
		if (optimizedImage) {
            Uint32 colorkey = SDL_MapRGB( optimizedImage->format, 0, 0xFF, 0xFF );
            SDL_SetColorKey( optimizedImage, SDL_SRCCOLORKEY, colorkey );
        }
		SDL_FreeSurface( loadedImage );
	} else {
        printf("Failed to load image: %s\n", filename.c_str());
    }
	return optimizedImage;
}

SDL_Surface *IMG_LoadOptimizeAlpha( std::string filename ){
	SDL_Surface* loadedImage = NULL;
	SDL_Surface* optimizedImage = NULL; 
	loadedImage = LoadImageFromFile( filename.c_str() ); 
	if( loadedImage != NULL ){
        // SDL 1.2 way
		optimizedImage = SDL_DisplayFormatAlpha( loadedImage );
		SDL_FreeSurface( loadedImage );
	} else {
        printf("Failed to load image: %s\n", filename.c_str());
    }
	return optimizedImage;
}

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip = NULL ){
	if(!source || !destination) return; // Add safety check
    SDL_Rect offset;
	offset.x = x;
	offset.y = y;
	SDL_BlitSurface( source, clip, destination, &offset );
}
