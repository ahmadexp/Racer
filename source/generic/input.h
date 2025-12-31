//define some things
#define keycount                    (500)
#define buttoncount                 (20)
#define mousecount                  (10)

//controller input variables
int cursorx[4];
int cursory[4];
int cursorxmov[4];
int cursorymov[4];
bool centercursor=0;

int keystate[keycount];
int buttonstate[buttoncount];
int mousestate[mousecount];

//recieve input
void updateinput(){
	SDL_Event event;
	for(int a=0;a<keycount;a++)if(keystate[a]==2)keystate[a]=1;
	while(SDL_PeepEvents(&event,1,SDL_GETEVENT,SDL_EVENTMASK(SDL_KEYDOWN)))
		keystate[event.key.keysym.sym]=2;
	while(SDL_PeepEvents(&event,1,SDL_GETEVENT,SDL_EVENTMASK(SDL_KEYUP)))
		keystate[event.key.keysym.sym]=0;
	for(int a=0;a<buttoncount;a++)if(buttonstate[a]==2)buttonstate[a]=1;
	while(SDL_PeepEvents(&event,1,SDL_GETEVENT,SDL_EVENTMASK(SDL_JOYBUTTONDOWN)))
		buttonstate[event.jbutton.button]=2;
	while(SDL_PeepEvents(&event,1,SDL_GETEVENT,SDL_EVENTMASK(SDL_JOYBUTTONUP)))
		buttonstate[event.jbutton.button]=0;
	for(int a=0;a<mousecount;a++)if(mousestate[a]==2)mousestate[a]=1;
	while(SDL_PeepEvents(&event,1,SDL_GETEVENT,SDL_EVENTMASK(SDL_MOUSEBUTTONDOWN)))
		mousestate[event.button.button]=2;
	while(SDL_PeepEvents(&event,1,SDL_GETEVENT,SDL_EVENTMASK(SDL_MOUSEBUTTONUP)))
		mousestate[event.button.button]=0;
	if(centercursor){
		//SDL_WM_GrabInput(SDL_GRAB_ON);
		SDL_GetMouseState(&cursorx[0],&cursory[0]);
		//SDL_GetRelativeMouseState(&cursorxmov[0],&cursorymov[0]);
		SDL_WarpMouse(screen->w/2,screen->h/2);
		cursorxmov[0]=cursorx[0]-screen->w/2;
		cursorymov[0]=cursory[0]-screen->h/2;
	}else{
		int oldx=cursorx[0];
		int oldy=cursory[0];
		SDL_GetMouseState(&cursorx[0],&cursory[0]);
		cursorxmov[0]=cursorx[0]-oldx;
		cursorymov[0]=cursory[0]-oldy;
	}
}
