void dashloop(){

	//adjust the speed of the game based on the frame rate
	float dashspeed=60.f/dash_framerate;
	//if(dashspeed<0.2f)dashspeed=0.2f;
	//if(dashspeed>2.0f)dashspeed=2.0f;
	dashtime();
	dashonoff(dashspeed);
	if(dashshowframerate==1){
		char temptext[256];
		sprintf(temptext,"%s%2.0f","FPS: ",dash_framerate);
		dashbutton(25,25,300,10,0,0,(char*)temptext);
	}
	if(dashonpercent!=0){
		dashbars();
		dashmenu();
		dashcursors();
	}
	
}
