/*
   -- NYDP --

   This source code of graphical user interface
   has been generated automatically by RemoteXY editor.
   To compile this code using RemoteXY library 2.3.3 or later version
   download by link http://remotexy.com/en/library/
   To connect using RemoteXY mobile app by link http://remotexy.com/en/download/
	 - for ANDROID 4.1.1 or later version;
	 - for iOS 1.2.1 or later version;

   This source code is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.
*/

////////////////////////////////////////////// 
//        RemoteXY include library          // 
////////////////////////////////////////////// 

// определение режима соединения и подключение библиотеки RemoteXY  
#define REMOTEXY_MODE__ESP8266WIFI_LIB_POINT
#include <ESP8266WiFi.h> 
#include "RoboconMotor.h"
#include "Json.h"

#include <RemoteXY.h> 

// настройки соединения  
#define REMOTEXY_WIFI_SSID "NYPD" 
#define REMOTEXY_WIFI_PASSWORD "12345678" 
#define REMOTEXY_SERVER_PORT 6377 


// конфигурация интерфейса   
#pragma pack(push, 1) 
uint8_t RemoteXY_CONF[] =
{ 255,7,0,0,0,54,0,8,8,0,
5,42,5,27,30,30,2,26,31,3,
132,2,2,27,7,2,26,5,47,66,
28,30,30,2,26,31,3,131,32,17,
39,14,2,26,2,0,77,4,22,11,
2,26,31,31,79,78,0,79,70,70,
0 };

// структура определяет все переменные вашего интерфейса управления  
struct {

	// input variable
	int8_t left_joy_x; // =-100..100 координата x положения джойстика 
	int8_t left_joy_y; // =-100..100 координата y положения джойстика 
	uint8_t drive_mode; // =0 если переключатель в положении A, =1 если в положении B, =2 если в положении C, ... 
	int8_t right_joy_x; // =-100..100 координата x положения джойстика 
	int8_t right_joy_y; // =-100..100 координата y положения джойстика 
	uint8_t turnLight; // =0 если переключатель в положении A, =1 если в положении B, =2 если в положении C, ... 
	uint8_t Siren; // =1 если переключатель включен и =0 если отключен 

	  // other variable
	uint8_t connect_flag;  // =1 if wire connected, else =0 

} RemoteXY;
#pragma pack(pop) 

///////////////////////////////////////////// 
//           END RemoteXY include          // 
///////////////////////////////////////////// 

RoboEffects motorEffect = RoboEffects();
RoboMotor motor = RoboMotor("motor", D7, D8, &motorEffect);

RoboEffects stearingEffect = RoboEffects();
RoboMotor stearing = RoboMotor("stearing", D5, D6, &stearingEffect);

bool turnOffTurnLights = false;

void handleTurnLight(int stearing) {
	if (RemoteXY.turnLight == 1) return;
	if (RemoteXY.turnLight == 2) { //Включений правий поворот
		if (stearing > 50) turnOffTurnLights = true;//ставимо флажок, щоб вимкнути поворот після того як руль вернеться в прямк положенн
	}
	if (RemoteXY.turnLight == 0) { //Включений правий поворот
		if (stearing < -50) turnOffTurnLights = true;//ставимо флажок, щоб вимкнути поворот після того як руль вернеться в прямк положенн
	}
	if (stearing > -10 && stearing < 10 && turnOffTurnLights) {
		RemoteXY.turnLight = 1;
		turnOffTurnLights = false;
		Serial.println("Поворот вимкнено.");
	}
}

void setup()
{
	Serial.begin(115200);
	RemoteXY_Init();
	// TODO you setup code 
	analogWriteRange(255);
	motor.responder = &Serial;
	stearing.responder = &Serial;
	Serial.println("Start");

	motor.setWeight(800);
	motor.reset();

	stearingEffect.halfProgress = 50;
	stearingEffect.fullProgress = 100;
	stearing.setWeight(20);
	stearing.reset();
	RemoteXY.turnLight = 1;
}

void loop()
{
	RemoteXY_Handler();
	// TODO you loop code 
	// используйте структуру RemoteXY для передачи данных 
	if (RemoteXY.connect_flag) {
		switch (RemoteXY.drive_mode)
		{
		case 1: {//лівий джойстик швидкість, правий - повороти
			motor.setSpeed(RemoteXY.left_joy_y);
			stearing.setSpeed(RemoteXY.right_joy_x);
			handleTurnLight(RemoteXY.right_joy_x);
			break;
		}
		case 2: {//лівий джойстик повороти, правий - швидкість
			motor.setSpeed(RemoteXY.right_joy_y);
			stearing.setSpeed(RemoteXY.left_joy_x);
			handleTurnLight(RemoteXY.left_joy_x);
			break;
		}
		case 3: {//Все керування правим джойстиком
			motor.setSpeed(RemoteXY.right_joy_y);
			stearing.setSpeed(RemoteXY.right_joy_x);
			handleTurnLight(RemoteXY.right_joy_x);
			break;
		}
		default://Все керування лівим джойстиком
			motor.setSpeed(RemoteXY.left_joy_y);
			stearing.setSpeed(RemoteXY.left_joy_x);
			handleTurnLight(RemoteXY.left_joy_x);
			break;
		}
		analogWrite(BUILTIN_LED, 220);
	}
	else {
		digitalWrite(BUILTIN_LED, 0);
		motor.reset();
		stearing.reset();
	}

	motor.loop();
	stearing.loop();
}