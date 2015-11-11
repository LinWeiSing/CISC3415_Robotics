/*
 *  bumper.cc
 *  
 */

#include <iostream>
#include <cstdlib>
#include <libplayerc++/playerc++.h>

int main(int argc, char *argv[]){  
  using namespace PlayerCc;  
  PlayerClient    robot("localhost");  
  BumperProxy     bp(&robot,0);
  Position2dProxy pp(&robot,0);
  pp.SetMotorEnable(true);
  double turnrate, speed;
  
  // Control loop
  while(true){    
    robot.Read();//reads from proxies 
	//PlayerClient::ReadIfWaiting? {if peek(0) then read()}
	  
	//output odometry data =(x,y,Ө) [L_bp,R_bp]=
    std::cout << "(" << pp.GetXPos() << ", "
	  			     << pp.GetYPos() << ", "
					 << pp.GetYaw()  << ")" 
			 << " [" << bp[0] << ", " << bp[1] << "]"
					 << std::endl;
      
    if(bp[0] || bp[1]){//if either bp
	  speed=-0.5;
	  // dtor converts from degrees to radians.
	  if (bp[0] && !bp[1]){ //if L_bp
		turnrate=dtor(-10);  
	  }
	  if (!bp[0] && bp[1]){ //if R_bp
		turnrate=dtor(10);
	  }
	  if (bp[0] && bp[1]){//if both bp
		if(rand()%2 > 0){
		  turnrate=dtor(-10);
		}else{
	      turnrate=dtor(10);
		}
	  }//end if both
    }else{
	  turnrate = 0;   
	  speed=0.5;
	}//end if either_bp else

      // What did we decide to do?
      std::cout << "Speed: " << speed << std::endl;      
      std::cout << "Turn rate: " << turnrate << std::endl << std::endl;

      // Send the motion commands that we decided on to the robot.
      pp.SetSpeed(speed, turnrate);  
    }//end while
}//end main