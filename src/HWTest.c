#include <ev3.h>

#define K_P 2
#define SPEED 5
#define MAX_ERROR 6
//#define DISTANCE_FROM_OBJECT 20
#define DISTANCE_FROM_OBJECT 31
#define WAIT_TIME 500
#define RED_COLOR 5
#define TARGET_ANGLE 6
#define TURN_THRESHOLD 360
#define REACH_DISTANCE 3

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
void reach_object(){
	int distance = ReadEV3UltrasonicSensorDistance(IN_4, EV3_ULTRASONIC_CM);
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
			if (counter > 18){
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

int main () {
    /**
     * Initialize EV3Color sensor connected at port 1
     */
    InitEV3();
    SetAllSensors(EV3Color, NULL, EV3Gyro, EV3Ultrasonic);

    int distance = rotate_anticlockwise();
    reach_object();

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
