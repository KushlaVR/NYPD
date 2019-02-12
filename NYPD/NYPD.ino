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
//  
//  
//                ------|USB|------
//                |     -----     |
//               3V3             VIN
//               GND             GND
//               TX              RST
//               RX               EN
//     MOTOR A <-D8              3V3
//     MOTOR B <-D7              GND
//  STEARING A <-D6               SK
//  STEARING B <-D5               SO
//               GND              SC
//               3V3              S1
// Front Light <-D4               S2
//   siren red <-D3               S3
//    LEFT LED <-D2               VU
//   RIGHT LED <-D1              GND
//  siren blue <-D0     LOLIN     A0
//                |               |
//                -----------------
//   
//  
////////////////////////////////////////////// 




////////////////////////////////////////////// 
//        RemoteXY include library          // 
////////////////////////////////////////////// 

// определение режима соединения и подключение библиотеки RemoteXY  
#define REMOTEXY_MODE__ESP8266WIFI_LIB_POINT
#include <ESP8266WiFi.h> 
#include "RoboconMotor.h"
#include "Json.h"
#include "SerialController.h"
#include "Blinker.h"

#include <RemoteXY.h> 

// настройки соединения  
#define REMOTEXY_WIFI_SSID "NYPD" 
#define REMOTEXY_WIFI_PASSWORD "12345678" 
#define REMOTEXY_SERVER_PORT 6377 


// конфигурация интерфейса   
// RemoteXY configurate   
#pragma pack(push, 1) 
uint8_t RemoteXY_CONF[] =
{ 255,7,0,0,0,54,0,8,8,0,
5,43,1,21,36,36,2,26,31,3,
132,2,2,27,7,2,26,5,51,63,
21,36,36,2,26,31,3,131,33,5,
34,12,2,26,2,0,76,2,22,11,
2,26,31,31,79,78,0,79,70,70,
0 };

// this structure defines all the variables of your control interface  
struct {

	// input variable
	int8_t left_joy_x; // =-100..100 x-coordinate joystick position 
	int8_t left_joy_y; // =-100..100 y-coordinate joystick position 
	uint8_t drive_mode; // =0 if select position A, =1 if position B, =2 if position C, ... 
	int8_t right_joy_x; // =-100..100 x-coordinate joystick position 
	int8_t right_joy_y; // =-100..100 y-coordinate joystick position 
	uint8_t turnLight; // =0 if select position A, =1 if position B, =2 if position C, ... 
	uint8_t Siren; // =1 if switch ON and =0 if OFF 

	  // other variable
	uint8_t connect_flag;  // =1 if wire connected, else =0 

} RemoteXY;
#pragma pack(pop) 

///////////////////////////////////////////// 
//           END RemoteXY include          // 
///////////////////////////////////////////// 

char SSID[20];
bool connected = false;

RoboEffects motorEffect = RoboEffects();
RoboMotor motor = RoboMotor("motor", D7, D8, &motorEffect);

RoboEffects stearingEffect = RoboEffects();
RoboMotor stearing = RoboMotor("stearing", D5, D6, &stearingEffect);

SerialController serialController = SerialController();

Blinker leftLight = Blinker("Left light");
Blinker rightLight = Blinker("Right light");
Blinker siren1 = Blinker("Siren");
Blinker buildinLed = Blinker("Build in led");

int FrontLightPin = D4;

bool turnOffTurnLights = false;

void handleTurnLight(int stearing) {
	if (RemoteXY.turnLight == 0) { //Включений лівий поворот
		if (!leftLight.isRunning()) leftLight.begin();
		rightLight.end();
	}
	if (RemoteXY.turnLight == 2) { //Включений правий поворот
		if (!rightLight.isRunning()) rightLight.begin();
		leftLight.end();
	}
	if (leftLight.isRunning()) {
		if (stearing < -50) turnOffTurnLights = true;//ставимо флажок, щоб вимкнути поворот після того як руль вернеться в прямк положенн
	}
	else if (rightLight.isRunning()) {
		if (stearing > 50) turnOffTurnLights = true;//ставимо флажок, щоб вимкнути поворот після того як руль вернеться в прямк положенн
	}
	if (turnOffTurnLights && stearing > -50 && stearing < 50) {
		if (leftLight.isRunning() && !rightLight.isRunning()) {//блимає лівий поворот
			leftLight.end();
			turnOffTurnLights = false;
			Serial.println("Лівий поворот вимкнено.");
		}
		else if (!leftLight.isRunning() && rightLight.isRunning()) {//блимає правий поворот
			rightLight.end();
			turnOffTurnLights = false;
			Serial.println("Правий поворот вимкнено.");
		}
	}
}

void setup()
{
	//SSID = ;
	String s = String(REMOTEXY_WIFI_SSID) + "_" + WiFi.macAddress();
	s.replace(":", "");
	strcpy(&SSID[0], s.c_str());

	Serial.begin(115200);
	Serial.println("");
	Serial.println("");
	Serial.println("     _______ ___ ___ ______ _____");
	Serial.println("    |    |  |   |   |   __ \\     \\");
	Serial.println("    |       |\\     /|    __/  --  |");
	Serial.println("    |__|____| |___| |___|  |_____/");
	Serial.println("               WI-FI remote control");
	Serial.println("                 kushlavr@gmail.com");
	Serial.println(SSID);

	remotexy = new CRemoteXY(RemoteXY_CONF_PROGMEM, &RemoteXY, REMOTEXY_ACCESS_PASSWORD, SSID, REMOTEXY_WIFI_PASSWORD, REMOTEXY_SERVER_PORT);//RemoteXY_Init();
	// TODO you setup code 
	analogWriteRange(120);
	//analogWriteFreq(500);
	motor.responder = &Serial;
	stearing.responder = &Serial;
	Serial.println("Start");

	motor.setWeight(800);
	motor.reset();

	stearingEffect.halfProgress = 50;
	stearingEffect.fullProgress = 100;
	stearing.setWeight(20);
	stearing.reset();
	RemoteXY.drive_mode = 1;
	RemoteXY.turnLight = 1;

	serialController.motor = &motor;
	serialController.stearing = &stearing;
	//Налаштування поворотників
	leftLight
		.Add(D2, 0, 255)
		->Add(D2, 500, 0)
		->Add(D2, 1000, 0);
	serialController.leftLight = &leftLight;
	rightLight
		.Add(D1, 0, 255)
		->Add(D1, 500, 0)
		->Add(D1, 1000, 0);
	serialController.rightLight = &rightLight;
	//Налаштування сирени
	siren1
		.Add(D3, 0, 0)
		
		->Add(D0, 0, 255)
		->Add(D0, 100, 0)
		->Add(D0, 200, 255)
		->Add(D0, 300, 0)
		->Add(D0, 400, 255)
		->Add(D0, 600, 0)

		->Add(D3, 700, 255)
		->Add(D3, 800, 0)
		->Add(D3, 900, 255)
		->Add(D3, 1000, 0)
		->Add(D3, 1100, 255)
		->Add(D3, 1200, 0);
	serialController.siren1 = &siren1;
	//Налаштування фар
	pinMode(FrontLightPin, OUTPUT);
	digitalWrite(FrontLightPin, LOW);
	//Блимак встроїного світлодіода
	buildinLed.Add(BUILTIN_LED, 0, 0)
		->Add(BUILTIN_LED, 500, 255)
		->Add(BUILTIN_LED, 1000, 0);
	buildinLed.begin();
	leftLight.begin();
	rightLight.begin();
}

int mapSpeed(int speed) {
	int corectedSpeed = (speed * speed) / 100;
	//30...100
	if (speed > 0)
		return map(corectedSpeed, 0, 100, 20, 100);
	if (speed < 0)
		return -map(corectedSpeed, 0, 100, 20, 100);
	return 0;
}

int mapStearing(int direction) {
	//50..100
	int corectedDirection = (direction * direction) / 100;

	if (direction >= -10 && direction <= 10) return 0;
	if (direction > 10)
		return map(corectedDirection, 0, 100, 50, 100);
	if (direction < 10)
		return -map(corectedDirection, 0, 100, 50, 100);
}


void loop()
{
	RemoteXY_Handler();
	if (!serialController.isRunning) {
		// используйте структуру RemoteXY для передачи данных 
		if (RemoteXY.connect_flag) {
			if (!connected) {
				Serial.println("Connected!");
				digitalWrite(FrontLightPin , HIGH);
				buildinLed.end();
				leftLight.end();
				rightLight.end();
				connected = true;
			}
			switch (RemoteXY.drive_mode)
			{
			case 1: {//лівий джойстик швидкість, правий - повороти
				motor.setSpeed(mapSpeed(RemoteXY.left_joy_y));
				stearing.setSpeed(mapStearing(RemoteXY.right_joy_x));
				handleTurnLight(RemoteXY.right_joy_x);
				break;
			}
			case 2: {//лівий джойстик повороти, правий - швидкість
				motor.setSpeed(mapSpeed(RemoteXY.right_joy_y));
				stearing.setSpeed(mapStearing(RemoteXY.left_joy_x));
				handleTurnLight(RemoteXY.left_joy_x);
				break;
			}
			case 3: {//Все керування правим джойстиком
				motor.setSpeed(mapSpeed(RemoteXY.right_joy_y));
				stearing.setSpeed(mapStearing(RemoteXY.right_joy_x));
				handleTurnLight(RemoteXY.right_joy_x);
				break;
			}
			default://Все керування лівим джойстиком
				motor.setSpeed(mapSpeed(RemoteXY.left_joy_y));
				stearing.setSpeed(mapStearing(RemoteXY.left_joy_x));
				handleTurnLight(RemoteXY.left_joy_x);
				break;
			}
			if (RemoteXY.Siren == 1) {
				if (!siren1.isRunning()) siren1.begin();
			}
			else {
				if (siren1.isRunning()) siren1.end();
			}
		}
		else {
			if (connected) {
				Serial.println("Disconnected!");
				digitalWrite(FrontLightPin, LOW);
				connected = false;
				buildinLed.begin();
				leftLight.begin();
				rightLight.begin();
			}
			motor.reset();
			stearing.reset();
		}
	}
	serialController.loop();
	motor.loop();
	stearing.loop();
	leftLight.loop();
	rightLight.loop();
	siren1.loop();
	buildinLed.loop();
}