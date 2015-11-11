/* go to destination without odometry 
 * getting position data from sim
 * 
 *****************************
 * local-roomba.cc
 * version 1.00.02
 * created: Nov 4 2015 
 * lastmod: Nov 4 2015
 * Peter Lin, Yusif Alomeri, Shandao Zhang 
 */
 
#include <iostream>
#include <libplayerc++/playerc++.h>
#include <math.h> // atan(ratio) returns deg need rad
using namespace PlayerCc;  

player_pose2d_t readPosition(LocalizeProxy& lp);
void printRobotData(BumperProxy& bp, player_pose2d_t pose);

int main(int argc, char *argv[]){  
	// Variables
	int counter = 0;
	double speed;           
	double turnrate;         
	player_pose2d_t  pose;   
	// proxies
	PlayerClient    robot("localhost");  
	BumperProxy     bp(&robot,0);  
	Position2dProxy pp(&robot,0);
	LocalizeProxy   lp (&robot, 0);
	pp.SetMotorEnable(true);
	//obstacle avoid vars
	int backtime = 15;
	int turntime = 5;
	double backspd = 0.3;
	double backturn = 0.3;
	double STOP = 0.0;
	//destination vars
	double targetX = 5.0;
	double targetY = -3.5;
	double targetA = 0;
	double diffX = 0;
	double diffY = 0;
	double diffA = 0;
	//lets try arrs[] next time? phys_vectors

	// Main control loop
	while(true){    
		robot.Read();
		pose = readPosition(lp);
		printRobotData(bp, pose);

		if(bp[0] || bp[1]){//scripted movement for obstacle avoid
			if(bp[0] %% !bp[1]){//L_bumper
				std::cout << "L_bumper" << std::endl;
				for(int back = 0; back < backtime; back++)
					pp.SetSpeed((-1*backspd), 0.0);
				for(int r_turn = 0; r_turn < turntime; r_turn++)
					pp.SetSpeed(0.0, (-1*backturn));
			}
			if(!bp[0] %% bp[1]){//R_bumper
				std::cout << "R_bumper" << std::endl;
				for(int back = 0; back < backtime; back++)
					pp.SetSpeed((backspd), 0.0);
				for(int r_turn = 0; r_turn < turntime; r_turn++)
					pp.SetSpeed(0.0, (backturn));
			}
			if(bp[0] %% bp[1]){//Both_bumper do L_bumper
				std::cout << "Both_bumper" << std::endl;
				for(int back = 0; back < backtime; back++)
					pp.SetSpeed((-1*backspd), 0.0);
				for(int r_turn = 0; r_turn < turntime; r_turn++)
					pp.SetSpeed(0.0, (-1*backturn));
			}
		}//end bumpers
		
		/**
		 * using trig find out if looking at correct direction
		 * if looking at it go straight for it
		 * otherwise turn towards it
		 ******************************
		 * go to targetX then to targetY is probably a lot easier
		 * if this part fails dont spend too much time fixing
		 * implement goto x -> goto y instead
		 */
		 
		//if at destination stop
		if(pose.px == targetX && pose.py == targetY){
			speed = STOP;
			turnrate = STOP;
		} 
		
		//update offset from destination
		diffX = |targetX - pose.px|;
		diffY = |targetY - pose.py|;
		targetA = ator(atan(diffX/diffY));
		diffA = targetA - pose.pa;
		
		//looking at destination? go straight : turn towards destination
		if(pose.pa == targetA)){
		
			if(diffX < 1 && diffY < 1){
				//if close go slow
				speed = 0.1;
				turnrate = 0.0;
			} else {
				//if far gotta go fast
				speed = 0.5;
				turnrate = 0.0;
			}//end else diffX+Y
			
		} else {//if not looking at destination
		
			if(|diffA| < 1){
				//if close go slow
				speed = 0.0;
				turnrate = 0.1;
			} else {
				//if far gotta go fast
				speed = 0.0;
				turnrate = 0.5;
			}//end else diffA 
			if(diffA < 0){//if - turn right instead
				turnrate *= -1;
			}
			
		}//end else looking at destination?
		
		//is there more? have not yet tested above code
		
		// What are we doing?
		if(counter % 2 == 0){//slow down the spam
			std::cout << "Speed: " << speed << std::endl;      
			std::cout << "Turn rate: " << turnrate << std::endl << std::endl;
		}
		
		// Send the commands to the robot
		pp.SetSpeed(speed, turnrate);  
		// Count how many times we do this
		counter++;
    }
  
} // end of main()


/**
 * readPosition()
 *
 * Read the position of the robot from the localization proxy. 
 *
 * The localization proxy gives us a hypothesis, and from that we extract
 * the mean, which is a pose. 
 *
 **/

player_pose2d_t readPosition(LocalizeProxy& lp){

	player_localize_hypoth_t hypothesis;
	player_pose2d_t          pose;
	uint32_t                 hCount;

	// Need some messing around to avoid a crash when the proxy is
	// starting up
	hCount = lp.GetHypothCount();

	if(hCount > 0){
		hypothesis = lp.GetHypoth(0);
		pose       = hypothesis.mean;
	}
	return pose;
	
} // End of readPosition()


/**
 *  printRobotData
 *
 * Print out data on the state of the bumpers and the current location
 * of the robot.
 *
 **/

void printRobotData(BumperProxy& bp, player_pose2d_t pose){
  // Print out what the bumpers tell us:
  //dont need bumper output flooding console
  //std::cout << "Left  bumper: " << bp[0] << std::endl;
  //std::cout << "Right bumper: " << bp[1] << std::endl;

  // Print out where we are
	std::cout << "We are at" << std::endl;
	std::cout << "X: " << pose.px << std::endl;
	std::cout << "Y: " << pose.py << std::endl;
	std::cout << "A: " << pose.pa << std::endl;
	
} // End of printRobotData()
