bool shutdownprogram = 0;

#include "generic.h"
#include "settings.h"
#include "projectsetup.h"
#include "projectloop.h"

int main(int argc,char **argv){

	//game loop

	printf("Loading generic settings...\n");
	loadgenericsettings();									//load the generic settings

	printf("Loading settings...\n");
	loadsettings();											//load the project specific settings

	printf("Generic setup...\n");
	genericsetup();											//generic setup

	printf("Dash setup...\n");
	dashsetup();											//setup the dashboard

	printf("Project setup...\n");
	projectsetup();											//setup for the project

	//game loop
    int frame_count = 0;
	while(!shutdownprogram){
        frame_count++;
        if (frame_count % 60 == 0) printf("Main loop running, frame %d\n", frame_count);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	//clear the screen
		
		SDL_PumpEvents();									//get what events have occured
		
		updateinput();										//get controller input

		if(!dashonpercent)projectloop();					//update the game

		dashloop();											//update the dashboard

        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            printf("OpenGL error in loop: 0x%x\n", err);
        }

		//show the frame
		SDL_GL_SwapBuffers();
		
		//clear out left over events and shut down when appropriate
		SDL_Event event;
		while(SDL_PeepEvents(&event,1,SDL_GETEVENT,SDL_ALLEVENTS)>0)
			if(event.type==SDL_QUIT)shutdownprogram=1;
    }
    // Clean up
    SDL_Quit();
    return 0;
}

