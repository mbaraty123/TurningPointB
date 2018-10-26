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
/*
 *	Due to the joystics having a margin of error, the THRESHOLD variable allows for a pre-defined
 *	sensitivity so that the motors do not over-exert themselves by applying a force too little to
 *	move the motor.
 */
	int THRESHOLD = 15;

	/*
	*	Determines whether the intake system is activated.
	*/
	bool intakeIsToggled = false;

	/*
	* Determines whether the flywheel is activated.
	*/
	bool firingMode = false;

	/*
	* Determines the speed at which the flywheel will spin
	* True = fast
	* False = slow
	*/
	bool firingSpeed = false;

	while (1) {

		/*
		* Sets the value of the left joystick's x-axis on the contoller to a variable.
		*/
		int stickLX = abs(joystickGetAnalog(MAIN_JOYSTICK, JOYSTICK_LEFT_X)) > THRESHOLD?
			joystickGetAnalog(MAIN_JOYSTICK, JOYSTICK_LEFT_X) : 0;

			/*
			* Sets the value of the left joystick's y-axis on the contoller to a variable.
			*/
 		int stickLY = abs(joystickGetAnalog(MAIN_JOYSTICK, JOYSTICK_LEFT_Y)) > THRESHOLD?
			joystickGetAnalog(MAIN_JOYSTICK, JOYSTICK_LEFT_Y) : 0;

			/*
			* Sets the value of the right joystick's x-axis on the contoller to a variable.
			*/
		int stickRX = abs(joystickGetAnalog(MAIN_JOYSTICK, JOYSTICK_RIGHT_X)) > THRESHOLD?
			joystickGetAnalog(MAIN_JOYSTICK, JOYSTICK_RIGHT_X) : 0;

		/*
		* Sets the motors to the correct power for mechanum wheels
		*/
		motorSet(MOTOR_FRONT_RIGHT, -(stickLY - stickRX - stickLX) / 2);
		motorSet(MOTOR_BACK_RIGHT, -(stickLY - stickRX + stickLX) / 2);
		motorSet(MOTOR_FRONT_LEFT, -(stickLY + stickRX + stickLX) / 2);
		motorSet(MOTOR_BACK_LEFT, (stickLY + stickRX - stickLX) / 2);

		/*
		*	Toggles the intakeIsToggled boolean to activate the intake system
		*/
		if(joystickGetDigital(MAIN_JOYSTICK, 6, JOY_UP)) {
			intakeIsToggled = true;
		}
		if(joystickGetDigital(MAIN_JOYSTICK, 6, JOY_DOWN)) {
			intakeIsToggled = false;
		}

		/*
		*	Determines whether the intake system is triggered
		*/
		if(intakeIsToggled) {
			motorSet(MOTOR_INTAKE, -127);
			motorSet(MOTOR_BELT, 127);
		} else {
			motorSet(MOTOR_INTAKE, 0);
			motorSet(MOTOR_BELT, 0);
		}

		/*
		*	Toggles the firingMode variable to activate the flywheels
		*/
		if(joystickGetDigital(MAIN_JOYSTICK, 5, JOY_UP)) {
			firingMode = true;
		}
		if(joystickGetDigital(MAIN_JOYSTICK, 5, JOY_DOWN)) {
			firingMode = false;
		}

		/*
		* Changes the boolean that determines the firing speed
		*/
		if(firingMode && joystickGetDigital(MAIN_JOYSTICK, 7, JOY_LEFT)) {
			firingSpeed = true;
		} else if(firingMode && joystickGetDigital(MAIN_JOYSTICK, 7, JOY_DOWN)) {
			firingSpeed = false;
		}

		/*
		*	Determines whether the firing system is triggered
		*/
		if(firingMode && firingSpeed) {
			motorSet(MOTOR_FLYWHEEL_A, -127);
			motorSet(MOTOR_FLYWHEEL_B, -127);
		} else if(firingMode && !firingSpeed) {
			motorSet(MOTOR_FLYWHEEL_A, -60);
			motorSet(MOTOR_FLYWHEEL_B, -60);
		} else {
			motorSet(MOTOR_FLYWHEEL_A, 0);
			motorSet(MOTOR_FLYWHEEL_B, 0);
		}

		/*
		*	Activates the cap flipper
		*/
		if(joystickGetDigital(MAIN_JOYSTICK, 8, JOY_DOWN)) {
			motorSet(MOTOR_FLIPPER, 127);
		} else if(joystickGetDigital(MAIN_JOYSTICK, 8, JOY_RIGHT)) {
			motorSet(MOTOR_FLIPPER, -127);
		} else {
			motorStop(MOTOR_FLIPPER);
		}

		delay(20);
	}
}
