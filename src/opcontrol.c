/** @file opcontrol.c
 * @brief File for operator control code
 *
 * This file should contain the user operatorControl() function and any functions related to it.
 *
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/
 *
 * PROS contains FreeRTOS (http://www.freertos.org) whose source code may be
 * obtained from http://sourceforge.net/projects/freertos/files/ or on request.
 */

#include "main.h"
#include "ports.h"
#include "controller.h"

/*
 * Runs the user operator control code. This function will be started in its own task with the
 * default priority and stack size whenever the robot is enabled via the Field Management System
 * or the VEX Competition Switch in the operator control mode. If the robot is disabled or
 * communications is lost, the operator control task will be stopped by the kernel. Re-enabling
 * the robot will restart the task, not resume it from where it left off.
 *
 * If no VEX Competition Switch or Field Management system is plugged in, the VEX Cortex will
 * run the operator control task. Be warned that this will also occur if the VEX Cortex is
 * tethered directly to a computer via the USB A to A cable without any VEX Joystick attached.
 *
 * Code running in this task can take almost any action, as the VEX Joystick is available and
 * the scheduler is operational. However, proper use of delay() or taskDelayUntil() is highly
 * recommended to give other tasks (including system tasks such as updating LCDs) time to run.
 *
 * This task should never exit; it should end with some kind of infinite loop, even if empty.
 */
void operatorControl() {

	int THRESHOLD = 15;
	bool intakeIsToggled = false;
	bool firingMode = false;

	while (1) {
		int stickLX = abs(joystickGetAnalog(MAIN_JOYSTICK, JOYSTICK_LEFT_X)) > THRESHOLD? joystickGetAnalog(MAIN_JOYSTICK, JOYSTICK_LEFT_X) : 0;
 		int stickLY = abs(joystickGetAnalog(MAIN_JOYSTICK, JOYSTICK_LEFT_Y)) > THRESHOLD? joystickGetAnalog(MAIN_JOYSTICK, JOYSTICK_LEFT_Y) : 0;
		int stickRX = abs(joystickGetAnalog(MAIN_JOYSTICK, JOYSTICK_RIGHT_X)) > THRESHOLD? joystickGetAnalog(MAIN_JOYSTICK, JOYSTICK_RIGHT_X) : 0;

		motorSet(MOTOR_FRONT_RIGHT, -(stickLY - stickRX - stickLX) / 2);
		motorSet(MOTOR_BACK_RIGHT, -(stickLY - stickRX + stickLX) / 2);
		motorSet(MOTOR_FRONT_LEFT, -(stickLY + stickRX + stickLX) / 2);
		motorSet(MOTOR_BACK_LEFT, (stickLY + stickRX - stickLX) / 2);

		if(joystickGetDigital(MAIN_JOYSTICK, 6, JOY_UP)) {
			intakeIsToggled = true;
		}
		if(joystickGetDigital(MAIN_JOYSTICK, 6, JOY_DOWN)) {
			intakeIsToggled = false;
		}

		if(intakeIsToggled) {
			motorSet(MOTOR_INTAKE, -127);
			motorSet(MOTOR_BELT, -127);
		} else {
			motorSet(MOTOR_INTAKE, 0);
			motorSet(MOTOR_BELT, 0);
		}

		if(joystickGetDigital(MAIN_JOYSTICK, 5, JOY_UP)) {
			firingMode = true;
		}
		if(joystickGetDigital(MAIN_JOYSTICK, 5, JOY_DOWN)) {
			firingMode = false;
		}

		if(firingMode) {
			motorSet(MOTOR_FLYWHEEL_A, 127);
			motorSet(MOTOR_FLYWHEEL_B, -127);
		} else {
			motorSet(MOTOR_FLYWHEEL_A, 0);
			motorSet(MOTOR_FLYWHEEL_B, 0);
		}

		if(joystickGetDigital(MAIN_JOYSTICK, 8, JOY_RIGHT)) {
			motorSet(MOTOR_FIRE_A, 90);
			motorSet(MOTOR_FIRE_B, -90);
		} else {
			motorSet(MOTOR_FIRE_A, 0);
			motorSet(MOTOR_FIRE_B, 0);
		}

		delay(20);
	}
}
