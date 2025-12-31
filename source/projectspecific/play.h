void play(){

	//if(dash_framerate)laptime+=1.f/dash_framerate;
	laptime+=1.f/60.f;

	//Joystick on off
		if(keystate[SDLK_j]==2){
			if(usejoystick==1){usejoystick=0;}else{usejoystick=1;}
		}

	//get input
	float inputspeed=0,inputangle=0;
	bool up=0,down=0,left=0,right=0;

	if(usejoystick){											//Joystick control	
#ifdef _WIN32
		if(joyGetPos(0,&joystick))	usejoystick=0;				//obtain joystick value and check the presence
		else{
			
			if(32768-(float)joystick.wYpos>6500)
				up=(32768-(float)joystick.wYpos)/16000;
			
			if(32768-(float)joystick.wYpos<-6500)
				down=((float)joystick.wYpos-32768)/16000;
 
			if(32768-(float)joystick.wXpos>6500)
				left=(32768-(float)joystick.wYpos)/100000;
			
			if(32768-(float)joystick.wXpos<-6500)
				right=((float)joystick.wYpos-32768)/100000;
 
			}
#else
		usejoystick=0;
#endif
		}

	if(keystate[SDLK_w]|keystate[SDLK_UP])up=1;
	if(keystate[SDLK_s]|keystate[SDLK_DOWN])down=1;
	if(keystate[SDLK_a]|keystate[SDLK_LEFT])left=1;
	if(keystate[SDLK_d]|keystate[SDLK_RIGHT])right=1;
	inputspeed=dist2d((float)right-(float)left,(float)up-(float)down,0,0);
	if(inputspeed>0.2f){
		if(inputspeed>2.f)inputspeed=2.f;
		inputangle=atan2((float)right-(float)left,(float)up-(float)down)-90.f*radiansindegree;
	}else{
		inputspeed=0.f;
	}
	//clarafy that input
	float turn=inputspeed*cos(inputangle);
	float accellerate=inputspeed*sin(inputangle);
	
	//turn
	playerangmovy+=turn*0.00475f;
	playerangy+=playerangmovy;
	playerangmovy*=0.9f;

	//tilt
	playerangz=playerangmovy*15.f;

	//keep track of the starting position
	float oldx=playerposx;
	float oldy=playerposy;
	float oldz=playerposz;
	int oldtrackposition=trackprogress[(int)floor(playerposx+0.5f)][(int)floor(playerposz+0.5f)];
	//gain movement speed from input
	playerposmovx+=accellerate*cos(playerangy)*0.00125f;
	playerposmovz+=accellerate*sin(playerangy)*0.00125f;
	//motion takes effect
	playerposx+=playerposmovx;
	playerposy+=playerposmovy;
	playerposz+=playerposmovz;
	//bump into the world
	/*for(int x=0;x<maxtracksizex;x++)
	for(int z=0;z<maxtracksizez;z++)
	if(track[x][z]>0){
		float trackangle=0;
		polygoncollisionmesh *collisionmesh=NULL;
		if(track[x][z]<5){
			if(track[x][z]==1)trackangle=90;
			if(track[x][z]==2)trackangle=180;
			if(track[x][z]==3)trackangle=270;
			if(track[x][z]==4)trackangle=0;
			collisionmesh=trackcollisionmesh;
		}else if(track[x][z]>4 && track[x][z]<9){
			if(track[x][z]==5)trackangle=180;
			if(track[x][z]==6)trackangle=270;
			if(track[x][z]==7)trackangle=0;
			if(track[x][z]==8)trackangle=90;
			collisionmesh=trackturnlcollisionmesh;
		}else if(track[x][z]>8 && track[x][z]<13){
			if(track[x][z]==9)trackangle=180;
			if(track[x][z]==10)trackangle=270;
			if(track[x][z]==11)trackangle=0;
			if(track[x][z]==12)trackangle=90;
			collisionmesh=trackturnrcollisionmesh;
		}
		spherebumppolygoncollisionmesh_advanced(
			playerposx,playerposy,playerposz,0.05f,1.f,
			x,0,z,0,(0.f+trackangle)*radiansindegree,0,
			collisionmesh,1,1);
	}*/
	bumpintoworld(&playerposx,&playerposz,0.05f);
	//correct movement speed
	playerposmovx=playerposx-oldx;
	playerposmovy=playerposy-oldy;
	playerposmovz=playerposz-oldz;
	//lose speed over time
	playerposmovx*=0.965f;
	playerposmovy*=0.965f;
	playerposmovz*=0.965f;
	//dont move
	playerposy=0.1;
	//check if the player finished a lap or went back
	int newtrackposition=trackprogress[(int)floor(playerposx+0.5f)][(int)floor(playerposz+0.5f)];
	if(oldtrackposition==tracklength && newtrackposition==0){
		lapcount++;
		if(laptime<bestlaptime)bestlaptime=laptime;
		laptime=0;
	}
	if(oldtrackposition==0 && newtrackposition==tracklength){
		lapcount--;
		laptime=bestlaptime;
	}

	//turn the camera
	camangz+=(playerangz*0.85f-camangz)*0.1f;
	camangy+=(playerangy-(camangy+90.f*radiansindegree))*0.1f;

	//position the camera
	camposx=playerposx-cos(camangy-90.f*radiansindegree)*0.15f;
	camposy=playerposy+0.04f;
	camposz=playerposz-sin(camangy-90.f*radiansindegree)*0.15f;

}
