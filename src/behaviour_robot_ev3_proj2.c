/*
 ============================================================================
 Name        : behaviour_robot_ev3_proj2.c
 Authors     : Fotios Lygerakis, Mehul Vishal Sadh
 Version     : 1.0
 Description : Wall Following / Wandering / Object Clearing Robot in C
 ============================================================================
*/

#include <ev3.h>
#include <time.h>
#include <math.h>

// Hyperparameters
#define K_P 1
#define SPEED 20
#define MAX_ERROR 6
#define PI 3.14
#define DISTANCE_FROM_OBJECT 31
#define WAIT_TIME 500
#define BLUE_COLOR 2
#define RED_COLOR 5
#define TARGET_ANGLE 6
#define TURN_THRESHOLD 360
#define TURN_THRESHOLD_RIGHT -30
#define REACH_DISTANCE 3
#define COUNTER_SINCE_GOAL 6
#define BIASED_PICK 1
#define MOVE_LEFT 0
#define MOVE_RIGHT 1
#define MOVE_FORWARD 2
#define TURN_OFFSET 0.5
#define PUSH_ITERATIONS 300
#define GO_BACK_DISTANCE 1
#define TIME_THRESHOLD 30

int left_color = IN_1;
int right_color = IN_2;

bool isExitButtonPressed() {
    return ButtonIsDown(BTNEXIT);
}

void move_forward(int init_angle){
	/*
	 * PD controlled forward movement using the error from the gyroscope
	 */
	int angle = ReadEV3GyroSensorAngle(IN_3, EV3GyroInterleavedRate) ;
	int error = angle -init_angle;
	int speed_l = K_P * error + SPEED;
	int speed_r = -K_P * error + SPEED;
	OnFwdReg(OUT_C, speed_l);
	OnFwdReg(OUT_A, speed_r);
}
void move_left(int init_angle){
	/*
	 * PD controlled left movement using the error from the gyroscope.
	 * The turn is being performed smoothly by moving the PD controller's target.
	 */
	ResetEV3GyroSensor(IN_3, EV3GyroSoftwareOffset);
	int angle = ReadEV3GyroSensorAngle(IN_3, EV3GyroInterleavedRate) ;
	float error = ((float)angle - TURN_OFFSET) -(float)init_angle;
	int speed_l = K_P * error + SPEED;
	int speed_r = -K_P * error + SPEED;
	OnFwdReg(OUT_C, speed_l);
	OnFwdReg(OUT_A, speed_r);
}
void move_right(int init_angle){
	/*
	 * PD controlled right movement using the error from the gyroscope.
	 * The turn is being performed smoothly by moving the PD controller's target.
	 */
	ResetEV3GyroSensor(IN_3, EV3GyroSoftwareOffset);
	int angle = ReadEV3GyroSensorAngle(IN_3, EV3GyroInterleavedRate) ;
	float error = ((float)angle + TURN_OFFSET) -(float)init_angle;
	int speed_l = K_P * error + SPEED;
	int speed_r = -K_P * error + SPEED;
	OnFwdReg(OUT_C, speed_l);
	OnFwdReg(OUT_A, speed_r);
}
void reach_object(int distance){
	/*
	 * Reaches the object found within a REACH_DISTANCE threshold
	 */
	int init_angle = ReadEV3GyroSensorAngle(IN_3, EV3GyroInterleavedRate);
	while (distance > REACH_DISTANCE){
			move_forward(init_angle);
			distance = ReadEV3UltrasonicSensorDistance(IN_4, EV3_ULTRASONIC_CM);
	}
}

int rotate_counterclockwise() {
	/*
	 * Rotates the robot on the spot counterclockwise
	 */
	ResetEV3GyroSensor(IN_3, EV3GyroSoftwareOffset);
	int init_angle = ReadEV3GyroSensorAngle(IN_3, EV3GyroInterleavedRate);
	int distance;
	PlaySound(SOUND_CLICK);
	int counter = 0;
	while (1){
		int angle = ReadEV3GyroSensorAngle(IN_3, EV3GyroInterleavedRate) ;
//		angle -= init_angle;
		if (angle >= TURN_THRESHOLD){
			Off(OUT_AC);
			PlaySound(SOUND_CLICK);
			break;
		}
		distance = ReadEV3UltrasonicSensorDistance(IN_4, EV3_ULTRASONIC_CM);
		if (distance <= DISTANCE_FROM_OBJECT){
			if (counter > COUNTER_SINCE_GOAL){
				Off(OUT_AC);
				PlaySound(SOUND_FAST_UP);
				counter = 0;
				break;
			}
			PlaySound(SOUND_CLICK);
			counter++;

		}

		OnRevReg(OUT_C, SPEED);
		OnFwdReg(OUT_A, SPEED);

	}
	Off(OUT_AC);
	return distance;

}

void rotate_clockwise_threshold() {
	/*
	 * Rotates the robot clockwise until a threshold angle
	 */
	ResetEV3GyroSensor(IN_3, EV3GyroSoftwareOffset);

	while (1) {

		int angle = ReadEV3GyroSensorAngle(IN_3, EV3GyroInterleavedRate);

		if (angle < TURN_THRESHOLD_RIGHT){
			Off(OUT_AC);
			break;
		}

		OnRevReg(OUT_A, SPEED);
		OnFwdReg(OUT_C, SPEED);
		Wait(10);

	}
	Off(OUT_AC);

}

void rotate_anticlockwise_threshold() {
	/*
	 * Rotates the robot anticlockwise until a threshold angle
	 */
	ResetEV3GyroSensor(IN_3, EV3GyroSoftwareOffset);

	while (1) {

		int angle = ReadEV3GyroSensorAngle(IN_3, EV3GyroInterleavedRate);

		if (angle > -TURN_THRESHOLD_RIGHT){
			Off(OUT_AC);
			break;
		}

		OnRevReg(OUT_C, SPEED);
		OnFwdReg(OUT_A, SPEED);
		Wait(10);

	}
	Off(OUT_AC);

}

void rotate_anticlockwise_continuous() {

	OnRevReg(OUT_C, SPEED);
	OnFwdReg(OUT_A, SPEED);

}

void rotate_clockwise_continuous() {

	OnRevReg(OUT_A, SPEED);
	OnFwdReg(OUT_C, SPEED);

}

void go_back(int BACK_DISTANCE) {
	// distance is in centimeters
	ResetRotationCount(OUT_AC);
	int angle_rotated = (int)ceil((360 * BACK_DISTANCE / (PI * 5.6)));
	while(MotorRotationCount(OUT_A) > -angle_rotated) {
		OnRevSync(OUT_AC, SPEED);
		Wait(10);
	}
	Off(OUT_AC);
}

void wall_following_left_color(int start_time) {
	/*
	 * Makes the robot follow the wall when blue color is first detected on left color sensor
	 * Terminates in less than 30 seconds or until wall found on right color sensor
	 */
	int count = 1;
	ResetEV3GyroSensor(IN_3, EV3GyroSoftwareOffset);

	while (ReadEV3ColorSensorColor(right_color) != BLUE_COLOR) {

		if (count == 1) {
			go_back(GO_BACK_DISTANCE);
			ResetEV3GyroSensor(IN_3, EV3GyroSoftwareOffset);
			while(ReadEV3ColorSensorColor(left_color) == BLUE_COLOR) {
				rotate_clockwise_continuous();
				if (ReadEV3GyroSensorAngle(IN_3, EV3GyroInterleavedRate) < -360) {
					ResetEV3GyroSensor(IN_3, EV3GyroSoftwareOffset);
					break;
				}
				Wait(10);
			}
			Off(OUT_AC);
			count++;

		}

		while(ReadEV3ColorSensorColor(left_color) == BLUE_COLOR) {
			if (ReadEV3ColorSensorColor(right_color) == BLUE_COLOR) {
				Off(OUT_AC);
				break;
			}
			OnFwdReg(OUT_C, SPEED);
			Wait(10);
			if ((time(NULL) - start_time) % TIME_THRESHOLD == 0)
				break;
		}
		Off(OUT_AC);

		while(ReadEV3ColorSensorColor(left_color) != BLUE_COLOR) {
			if (ReadEV3ColorSensorColor(right_color) == BLUE_COLOR) {
				Off(OUT_AC);
				break;
			}
			OnFwdReg(OUT_A, SPEED);
			Wait(10);
			if ((time(NULL) - start_time) % TIME_THRESHOLD == 0)
				break;
		}
		Off(OUT_AC);

		if ((time(NULL) - start_time) % TIME_THRESHOLD == 0)
			break;


	}

	if (ReadEV3ColorSensorColor(right_color) == BLUE_COLOR) {
		ResetEV3GyroSensor(IN_3, EV3GyroSoftwareOffset);
		go_back(GO_BACK_DISTANCE + 1);
		rotate_clockwise_threshold();
		Wait(10);
		count = 1;
	}

	Off(OUT_AC);

}

void wall_following_right_color(int start_time) {
	/*
	 * Makes the robot follow the wall when blue color is first detected on right color sensor
	 * Terminates in less than 30 seconds or until wall found on left color sensor
	 */
	int count = 1;
	ResetEV3GyroSensor(IN_3, EV3GyroSoftwareOffset);

	while (ReadEV3ColorSensorColor(left_color) != BLUE_COLOR) {

		if (count == 1) {
			go_back(GO_BACK_DISTANCE);
			ResetEV3GyroSensor(IN_3, EV3GyroSoftwareOffset);
			while(ReadEV3ColorSensorColor(right_color) == BLUE_COLOR) {
				rotate_anticlockwise_continuous();
				if (ReadEV3GyroSensorAngle(IN_3, EV3GyroInterleavedRate) > 360) {
					ResetEV3GyroSensor(IN_3, EV3GyroSoftwareOffset);
					break;
				}
				Wait(10);
			}
			Off(OUT_AC);
			count++;

		}

		while(ReadEV3ColorSensorColor(right_color) == BLUE_COLOR) {
			if (ReadEV3ColorSensorColor(left_color) == BLUE_COLOR) {
				Off(OUT_AC);
				break;
			}
			OnFwdReg(OUT_A, SPEED);
			Wait(10);
			if ((time(NULL) - start_time) % TIME_THRESHOLD == 0)
				break;
		}
		Off(OUT_AC);

		while(ReadEV3ColorSensorColor(right_color) != BLUE_COLOR) {
			if (ReadEV3ColorSensorColor(left_color) == BLUE_COLOR) {
				Off(OUT_AC);
				break;
			}
			OnFwdReg(OUT_C, SPEED);
			Wait(10);
			if ((time(NULL) - start_time) % TIME_THRESHOLD == 0)
				break;
		}
		Off(OUT_AC);
		if ((time(NULL) - start_time) % TIME_THRESHOLD == 0)
			break;
	}

	if (ReadEV3ColorSensorColor(right_color) == BLUE_COLOR) {
		ResetEV3GyroSensor(IN_3, EV3GyroSoftwareOffset);
		go_back(GO_BACK_DISTANCE + 1);
		rotate_anticlockwise_threshold();
		Wait(10);
		count = 1;
	}


	Off(OUT_AC);



}

int pick_random_move(){
	/*
	 * returns a random move to perform during wander phase
	 * There is a biased option to favor turning left and forward
	 */
	int lower = 1, upper = 10;
	int random_pick = (rand() % (upper - lower + 1)) + lower;
	if (BIASED_PICK){
		if(random_pick <= 3)
			return MOVE_LEFT;		//left
		else if ( random_pick <= 6)
			return MOVE_RIGHT;		//right
		else
			return MOVE_FORWARD;	//Forward
	}
	else{
		lower = 0, upper = 2;
		random_pick = (rand() % (upper - lower + 1)) + lower;
		if(random_pick == 0)
			return MOVE_LEFT;		//left
		else if ( random_pick == 1)
			return MOVE_RIGHT;		//right
		else
			return MOVE_FORWARD;	//Forward
	}
}

void wander(){
	int init_angle = ReadEV3GyroSensorAngle(IN_3, EV3GyroInterleavedRate);
    int move = pick_random_move();
    if (move == MOVE_FORWARD) {
		move_forward(init_angle);
    	TermPrintln("MOVE_FORWARD");
    }
    else if (move == MOVE_LEFT) {
    	move_left(init_angle);
    	TermPrintln("MOVE_LEFT");
    }
    else if (move == MOVE_RIGHT) {
		move_right(init_angle);
    	TermPrintln("MOVE_RIGHT");
    }
}
void find_goal(){

	int init_angle = ReadEV3GyroSensorAngle(IN_3, EV3GyroInterleavedRate);
	while (ReadEV3ColorSensorColor(right_color) == RED_COLOR || ReadEV3ColorSensorColor(left_color) == RED_COLOR){
		move_forward(init_angle);
	}

}

int main () {

    InitEV3();
    SetAllSensors(EV3Color, EV3Color, EV3Gyro, EV3Ultrasonic);
    ResetEV3GyroSensor(IN_3, EV3GyroSoftwareOffset);
    int goal_reached = 0;
    time_t start_time = time(NULL);

	while(1) {

		if (goal_reached == 1)
			break;

		switch(ReadEV3ColorSensorColor(left_color)) {

			case BLUE_COLOR:
				Off(OUT_AC);
				PlaySound(SOUND_DOUBLE_BEEP);
				TermPrintln("Entering Left Wall Following");
				wall_following_left_color(start_time);
				PlaySound(SOUND_DOWN);
				break;

			case RED_COLOR:
				PlaySound(SOUND_CLICK);
				TermPrintln("Entering Goal Finding");
				find_goal();
				goal_reached++;
				PlaySound(SOUND_DOWN);
				break;

			default:
				TermPrintln("Wandering");
				wander();
				break;
		}

		if (goal_reached == 1)
			break;

		switch(ReadEV3ColorSensorColor(right_color)) {
			case BLUE_COLOR:
				Off(OUT_AC);
				PlaySound(SOUND_DOUBLE_BEEP);
				TermPrintln("Entering Right Wall Following");
				wall_following_right_color(start_time);
				PlaySound(SOUND_DOWN);
				break;

			case RED_COLOR:
				PlaySound(SOUND_CLICK);
				TermPrintln("Entering Goal Finding");
				find_goal();
				goal_reached++;
				PlaySound(SOUND_DOWN);
				break;

			default:
				TermPrintln("Wandering");
				wander();
				break;
		}

	}

    FreeEV3();
    return 0;
}
