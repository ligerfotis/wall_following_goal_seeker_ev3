#include <ev3.h>
#include <time.h>

#define K_P 2
#define SPEED 5
#define MAX_ERROR 6
//#define DISTANCE_FROM_OBJECT 20
#define DISTANCE_FROM_OBJECT 31
#define WAIT_TIME 500
#define BLUE_COLOR 2
#define RED_COLOR 5
#define TARGET_ANGLE 6
#define TURN_THRESHOLD 360
#define TURN_THRESHOLD_RIGHT -90
#define REACH_DISTANCE 3
#define COUNTER_SINCE_GOAL 10
#define BIASED_PICK 1
#define MOVE_LEFT 0
#define MOVE_RIGHT 1
#define MOVE_FORWARD 2
#define TURN_OFFSET 0.5

int front_color = IN_1;
int left_color = IN_2;

bool isExitButtonPressed() {
    return ButtonIsDown(BTNEXIT);
}
void move_forward(int init_angle){
	int angle = ReadEV3GyroSensorAngle(IN_3, EV3GyroInterleavedRate) ;
	int error = angle -init_angle;
	int speed_l = K_P * error + SPEED;
	int speed_r = -K_P * error + SPEED;
	OnFwdReg(OUT_C, speed_l);
	OnFwdReg(OUT_A, speed_r);
}
void move_left(int init_angle){
	ResetEV3GyroSensor(IN_3, EV3GyroSoftwareOffset);
	int angle = ReadEV3GyroSensorAngle(IN_3, EV3GyroInterleavedRate) ;
	float error = ((float)angle - TURN_OFFSET) -(float)init_angle;
	int speed_l = K_P * error + SPEED;
	int speed_r = -K_P * error + SPEED;
	OnFwdReg(OUT_C, speed_l);
	OnFwdReg(OUT_A, speed_r);
}
void move_right(int init_angle){
	ResetEV3GyroSensor(IN_3, EV3GyroSoftwareOffset);
	int angle = ReadEV3GyroSensorAngle(IN_3, EV3GyroInterleavedRate) ;
	float error = ((float)angle + TURN_OFFSET) -(float)init_angle;
	int speed_l = K_P * error + SPEED;
	int speed_r = -K_P * error + SPEED;
	OnFwdReg(OUT_C, speed_l);
	OnFwdReg(OUT_A, speed_r);
}
void reach_object(int distance){
//	int distance = ReadEV3UltrasonicSensorDistance(IN_4, EV3_ULTRASONIC_CM);
	int init_angle = ReadEV3GyroSensorAngle(IN_3, EV3GyroInterleavedRate);
	while (distance > REACH_DISTANCE){
			move_forward(init_angle);
			distance = ReadEV3UltrasonicSensorDistance(IN_4, EV3_ULTRASONIC_CM);
	}
}
int rotate_anticlockwise() {
	ResetEV3GyroSensor(IN_3, EV3GyroSoftwareOffset);
	int init_angle = ReadEV3GyroSensorAngle(IN_3, EV3GyroInterleavedRate);
	PlaySound(SOUND_CLICK);
//	TermPrintln("HERE!");
//	ButtonWaitForPress(BUTTON_IDX_UP);
	int counter = 0;
	while (1){
		int angle = ReadEV3GyroSensorAngle(IN_3, EV3GyroInterleavedRate) ;
		angle -= init_angle;
		if (angle > TURN_THRESHOLD){
			Off(OUT_AC);
			PlaySound(SOUND_CLICK);
			break;
		}
		int distance = ReadEV3UltrasonicSensorDistance(IN_4, EV3_ULTRASONIC_CM);
		if (distance <= DISTANCE_FROM_OBJECT){
			if (counter > COUNTER_SINCE_GOAL){
				Off(OUT_AC);
				PlaySound(SOUND_FAST_UP);
				counter = 0;
				return distance;
			}
			PlaySound(SOUND_CLICK);
			counter++;

		}

		OnRevReg(OUT_C, SPEED);
		OnFwdReg(OUT_A, SPEED);

	}
	Off(OUT_AC);

}

void rotate_clockwise_90() {
	ResetEV3GyroSensor(IN_3, EV3GyroSoftwareOffset);
	int init_angle = ReadEV3GyroSensorAngle(IN_3, EV3GyroInterleavedRate);

	while (1) {

		int angle = ReadEV3GyroSensorAngle(IN_3, EV3GyroInterleavedRate);
		angle -= init_angle;
		if (angle < TURN_THRESHOLD_RIGHT){
			Off(OUT_AC);
			break;
		}

		OnRevReg(OUT_A, SPEED);
		OnFwdReg(OUT_C, SPEED);

	}
	Off(OUT_AC);

}

void rotate_anticlockwise_90() {
	ResetEV3GyroSensor(IN_3, EV3GyroSoftwareOffset);
	int init_angle = ReadEV3GyroSensorAngle(IN_3, EV3GyroInterleavedRate);

	while (1) {

		int angle = ReadEV3GyroSensorAngle(IN_3, EV3GyroInterleavedRate);
		angle -= init_angle;
		if (angle > -TURN_THRESHOLD_RIGHT){
			Off(OUT_AC);
			break;
		}

		OnRevReg(OUT_C, SPEED);
		OnFwdReg(OUT_A, SPEED);

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

void go_back(int init_angle) {

	while (ReadEV3ColorSensorColor(front_color) == BLUE_COLOR) {
		init_angle = ReadEV3GyroSensorAngle(IN_3, EV3GyroInterleavedRate);
		int angle = ReadEV3GyroSensorAngle(IN_3, EV3GyroInterleavedRate) ;
		int error = angle -init_angle;
		int speed_l = K_P * error + SPEED;
		int speed_r = -K_P * error + SPEED;
		OnRevReg(OUT_C, speed_l);
		OnRevReg(OUT_A, speed_r);
	}

}

void wall_following() {

	PlaySound(SOUND_DOUBLE_BEEP);

	int init_angle = ReadEV3GyroSensorAngle(IN_3, EV3GyroInterleavedRate);

	go_back(init_angle);

	Off(OUT_AC);

	rotate_clockwise_90();

	Off(OUT_AC);

	while (ReadEV3ColorSensorColor(front_color) != BLUE_COLOR) {

		if (ReadEV3ColorSensorColor(left_color) != BLUE_COLOR)
			break;

		init_angle = ReadEV3GyroSensorAngle(IN_3, EV3GyroInterleavedRate);

		move_forward(init_angle);

	}

	Off(OUT_AC);

	PlaySound(SOUND_DOWN);

}
int pick_random_move(){
	int lower = 1, upper = 10;
	int random_pick = (rand() % (upper - lower + 1)) + lower;
	if (BIASED_PICK){
		if(random_pick <= 4)
			return MOVE_LEFT;		//left
		else if ( random_pick <= 8)
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
    if (move == MOVE_FORWARD)
		move_forward(init_angle);
//    	TermPrintln("MOVE_FORWARD");
    else if (move == MOVE_LEFT)
    	move_left(init_angle);
//    	TermPrintln("MOVE_LEFT");
    else if (move == MOVE_RIGHT)
		move_right(init_angle);
//    	TermPrintln("MOVE_RIGHT");
}

int main () {
    /**
     * Initialize EV3Color sensor connected at port 1
     */

    InitEV3();
    SetAllSensors(EV3Color, EV3Color, EV3Gyro, EV3Ultrasonic);
//    ResetEV3GyroSensor(IN_3, EV3GyroSoftwareOffset);
//
//	// Start Motion
//	int init_angle = ReadEV3GyroSensorAngle(IN_3, EV3GyroInterleavedRate);
//	while(ReadEV3ColorSensorColor(front_color) != BLUE_COLOR)
//		move_forward(init_angle);
//
//	Off(OUT_AC);
//
//	// Start Wall Following
//	if (ReadEV3ColorSensorColor(front_color) == BLUE_COLOR)
//		wall_following();

/*
 * Uncomment bellow to check wander function
 * */
//    while(1){
//        wander();
//
//    }

/*
 * Uncomment bellow to check wander function
 * */
    int distance = rotate_anticlockwise();
    reach_object(distance);

/*
 * Uncomment bellow to check sensors
 * */
//    while (!isExitButtonPressed()) {
//    	int angle = ReadEV3GyroSensorAngle(IN_1, EV3GyroInterleavedAngle);
//        Color color = ReadEV3ColorSensorColor(IN_1);
//
//        LcdClean();
//        int distance = ReadEV3UltrasonicSensorDistance(IN_4, EV3_ULTRASONIC_CM);
//
//        if (color == RED_COLOR)
//        	LcdTextf(1, 0, LcdRowToY(1), "Color: red!");
//        if (distance <= DISTANCE_FROM_OBJECT){
//        	LcdTextf(LCD_COLOR_BLACK, 0, LcdRowToY(2), "Object in %d cm", distance);
//        	PlaySound(SOUND_FAST_UP);
//        }
//        LcdTextf(1, 0, LcdRowToY(3), "Angle: %d", angle);
//        Wait(WAIT_TIME);
//    }

    FreeEV3();
    return 0;
}
