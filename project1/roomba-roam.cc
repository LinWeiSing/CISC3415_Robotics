/* roomba roams in a left turning square
 * right turning square available through cmdln arg
 * ./roomba-roam [ANY] for right turning
 *****************************
 * roomba-roam.cc
 * version 1.01.02
 * Sept 3 2015
 * Sept 8 2015
 * Peter Lin, Yusif Alomeri, Shandao Zhang 
 */
#include <iostream>
#include <cstdlib>
#include <libplayerc++/playerc++.h>

int main(int argc, char *argv[]){  
  using namespace PlayerCc;  

  // Set up proxy. Proxies are the datastructures that Player uses to
  // talk to the simulator and the real robot.

  PlayerClient    robot("localhost");  
  Position2dProxy pp(&robot,0);       // The 2D proxy is used to send 
                                      // motion commands
  int timer = 0;                      

  // Allow the program to take charge of the motors (take care now)
  pp.SetMotorEnable(true);
  
  //new control variables
  //=====================================================
  //default 30, 30 || 0.1, 0.0 || 0.1, 0.0 || 0.0, 0.0
  //final 3ft = TIME = 15 SPEED = 0.3 
  //90deg turn = TIME = 12 TURN = 0.3
  //Error: LeftSQR: 11 inches North East from init pos
  //Error: RightSQR: 7 inches North West from init pos
  int straight_time = 15;
  int turn_time = 12;
  int sec_time = straight_time + turn_time + 1;
  double speed, turnrate;
  //straight
  double S_speed = 0.3;
  double S_turnrate = 0.0;
  //turn
  double T_speed = 0.0;
  double T_turnrate = 0.3;
  //stop
  double STOP = 0;
  //in configure mode the control loop will only run once
  bool configure = false;
  bool left = true;
  //====================================================
  
  //allows for command line argument to switch between 
  //left and right turning square default() = left 
  if (argc > 0)
  	left = false;
  
  // Control loop
  //sqr = straight, turn(90) repeat x4
  for (int x = 0; x < 4; x++){ 
	//section loop
	while(true) {    
      timer++;
      // Read from the proxies.
      robot.Read();
	  
	  //timer % section_time to repeat section 
	  if((timer%sec_time) < straight_time){
		//straight
	    speed = S_speed;
	    turnrate = S_turnrate;
      }
      else
      if(((timer%sec_time) >= straight_time) && 
		 ((timer%sec_time) < (straight_time + turn_time))){
		//turn
		speed = T_speed;
		//turns left
		if(left)
			turnrate = T_turnrate;
		else
		//turns right
		if(!left)
			turnrate = T_turnrate * -1;
	  }
	  else
	  //breaks after section complete
	  break;
      // Print out what we decided to do?
	  //std::cout << x << std::endl; //troubleshoot outer for loop
      std::cout << "Speed: " << speed << std::endl;      
      std::cout << "Turn rate: " << turnrate << std::endl << std::endl;

      // Send the motion commands that we decided on to the robot.
      pp.SetSpeed(speed, turnrate);  
    }//end while
	//in configure only runs 1 section
	if (configure)
	  break;
  }//end for
  
  //stop
  std::cout << "STOPPED(0)" << std::endl;  
  while(true){
	pp.SetSpeed(STOP, STOP);  
  }//end stop while
  
}//end main

