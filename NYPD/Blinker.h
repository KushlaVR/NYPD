#pragma once

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

class BlinkerItem
{
public:
	int iteration;
	BlinkerItem();
	~BlinkerItem() {};
	BlinkerItem * next;
	int pin;
	unsigned long offset;
	uint8_t value;
};

class Blinker
{
	BlinkerItem * first;
	BlinkerItem * last;
	BlinkerItem * current;

	unsigned long startTime = 0;

public:
	Blinker();
	~Blinker();
	void loop();
	Blinker * Add(int pin, unsigned long offset, uint8_t value);
	Blinker * begin() { current = first; startTime = millis(); return this; };
	Blinker * end() { current = nullptr; return this; };
};
