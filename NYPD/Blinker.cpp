#include "Blinker.h"



Blinker::Blinker()
{
	first = nullptr;
	current = nullptr;
}


Blinker::~Blinker()
{
}

void Blinker::loop()
{
	if (startTime == 0) return;
	unsigned long offset = millis() - startTime;//—к≥льки часу пройшло в≥д початку
	BlinkerItem * item = current;
	if (item == nullptr) return;//якщо немаЇ елемента на черз≥ - н≥чого не робимо
	if (offset < item->offset) return;//якщо час ще не настав - виходимо
	analogWrite(item->pin, item->value);//„ас настав
	Serial.printf("blink %i->%i\n", item->pin, item->value);
	current = item->next;//ѕереходимо до наступного елемента
	if (current == nullptr) {//к≥нець списку, починаЇмо з початку
		current = first;
		startTime = millis();
	}
}

Blinker * Blinker::Add(int pin, unsigned long offset, uint8_t value)
{
	pinMode(pin, OUTPUT);
	BlinkerItem * item = new BlinkerItem();
	item->pin = pin;
	item->offset = offset;
	item->value = value;
	if (first == nullptr) {
		first = item;
		last = item;
	}
	else {
		last->next = item;
		last = item;
	}
	return this;
}

BlinkerItem::BlinkerItem()
{
	next = nullptr;
}
