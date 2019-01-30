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
#define REMOTEXY_WIFI_SSID "RemoteXY" 
#define REMOTEXY_WIFI_PASSWORD "12345678" 
#define REMOTEXY_SERVER_PORT 6377 


// конфигурация интерфейса   
#pragma pack(push, 1) 
uint8_t RemoteXY_CONF[] =
{ 255,5,0,0,0,29,0,8,8,0,
5,42,5,27,30,30,2,26,31,3,
132,33,4,41,11,2,26,5,32,66,
28,30,30,2,26,31 };

// структура определяет все переменные вашего интерфейса управления  
struct {

	// input variable
	int8_t left_joy_x; // =-100..100 координата x положения джойстика 
	int8_t left_joy_y; // =-100..100 координата y положения джойстика 
	uint8_t siren_mode; // =0 если переключатель в положении A, =1 если в положении B, =2 если в положении C, ... 
	int8_t right_joy_x; // =-100..100 координата x положения джойстика 
	int8_t right_joy_y; // =-100..100 координата y положения джойстика 

	  // other variable
	uint8_t connect_flag;  // =1 if wire connected, else =0 

} RemoteXY;
#pragma pack(pop) 

///////////////////////////////////////////// 
//           END RemoteXY include          // 
///////////////////////////////////////////// 

RoboEffects motorEffect = RoboEffects();
RoboMotor motor = RoboMotor("motor", D5, D6, &motorEffect);

RoboEffects stearingEffect = RoboEffects();
RoboMotor stearing = RoboMotor("stearing", D7, D8, &stearingEffect);

void setup()
{
	Serial.begin(115200);
	RemoteXY_Init();
	// TODO you setup code 
	analogWriteRange(255);
	motor.responder = &Serial;
	stearing.responder = &Serial;
	Serial.println("Start");

	motor.setWeight(1);
	motor.reset();

	stearingEffect.halfProgress = 50;
	stearingEffect.fullProgress = 100;
	stearing.setWeight(1);
	stearing.reset();
}

void loop()
{
	RemoteXY_Handler();
	// TODO you loop code 
	// используйте структуру RemoteXY для передачи данных 
	if (RemoteXY.connect_flag) {
		switch (RemoteXY.siren_mode)
		{
		case 1: {//лівий джойстик швидкість, правий - повороти
			motor.setSpeed(RemoteXY.left_joy_y);
			stearing.setSpeed(RemoteXY.right_joy_x);
			break;
		}
		default://Все керування лівим джойстиком
			motor.setSpeed(RemoteXY.left_joy_y);
			stearing.setSpeed(RemoteXY.left_joy_x);
			break;
		}
		analogWrite(BUILTIN_LED, 200);
	}
	else {
		digitalWrite(BUILTIN_LED, 0);
		motor.reset();
		stearing.reset();
	}

	motor.loop();
	stearing.loop();
}