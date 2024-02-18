#include <Arduino.h>
#include <Wire.h>
#include <DS3231.h>
#include <time.h>

DS3231 cl;
RTClib rtc;

// Режим 1 принудительного включения или выключения
int r1 = 5;
bool regime1_On = false;
// Режим 2 работы по фотоэлементу + таймер
int r2 = 6;
bool regime2_On = true;
// Режим 3 работы по фотоэлементу
int r3 = 7;
bool regime3_On = false;

// Таймер 1
int hour1_On = -1;
int hour1_Off = -1;
int minute1_On = -1;
int minute1_Off = -1;

// Таймер 2
int hour2_On = -1;
int hour2_Off = -1;
int minute2_On = -1;
int minute2_Off = -1;

// Таймер 3
int hour3_On = -1;
int hour3_Off = -1;
int minute3_On = -1;
int minute3_Off = -1;

void printCurrentTime();
void setCurrentTime();
void observeRegimeChanges();
void checkAlarmsAndTurnRelaysIfNeeded();
void turnOnRegime1Relay();
void turnOnRegime2Relay();
void turnOnRegime3Relay();
void observeGsmCommands();
void turnOffAllRelays();

void setup() {

    Serial.begin(9600);
    Wire.begin();

    pinMode(r1, OUTPUT);
    pinMode(r2, OUTPUT);
    pinMode(r3, OUTPUT);

    Serial.println("Board Ready!");
}

void loop() {

    // setCurrentTime(); // тут можно установить время для RTC модуля
    delay(500);
    printCurrentTime();
    delay(500);
    observeRegimeChanges();
}

void observeGsmCommands() {

}

void observeRegimeChanges() {
    if (regime1_On) {
        Serial.println("Режим работы принудительного включения/выключения АКТИВЕН");
        turnOnRegime1Relay();
    } else if (regime2_On) {
        Serial.println("Режим работы по таймеру АКТИВЕН");
        checkAlarmsAndTurnRelaysIfNeeded();
    } else if (regime3_On) {
        Serial.println("Режим работы по фотоэлементу АКТИВЕН");
        turnOnRegime3Relay();
    }
}

void checkAlarmsAndTurnRelaysIfNeeded() {
        
    DateTime now = rtc.now();
    int currentHour = now.hour();
    int currentMin = now.minute();

    if (
        (currentHour == hour1_On && currentMin == minute1_On)
        || (currentHour == hour2_On && currentMin == minute1_On)
        || (currentHour == hour3_On && currentMin == minute1_On)
    ) {
        Serial.println("---");
        Serial.println("Наступило время ВКЛЮЧИТЬ реле в режиме работы по таймеру");
        Serial.println("");
        turnOnRegime2Relay();
    }

    if (
        (currentHour == hour1_Off && currentMin == minute1_Off)
        || (currentHour == hour2_Off && currentMin == minute2_Off)
        || (currentHour == hour3_Off && currentMin == minute3_Off)
    ) {
        Serial.println("---");
        Serial.println("Наступило время ВЫКЛЮЧИТЬ реле в режиме работы по таймеру");
        Serial.println("");
        digitalWrite(r2, LOW);
    }
}

void turnOnRegime1Relay() {
    Serial.println("---");
    Serial.println("ВКЛЮЧАЕМ РЕЛЕ принудительного режима работы");
    Serial.println("");
    digitalWrite(r1, HIGH);
    digitalWrite(r2, LOW);
    digitalWrite(r3, LOW);
}

void turnOnRegime2Relay() {
    Serial.println("---");
    Serial.println("ВКЛЮЧАЕМ РЕЛЕ режима работы по таймеру");
    Serial.println("");
    digitalWrite(r1, LOW);
    digitalWrite(r2, HIGH);
    digitalWrite(r3, LOW);
}

void turnOnRegime3Relay() {
    Serial.println("---");
    Serial.println("ВКЛЮЧАЕМ РЕЛЕ режима работы по таймеру");
    Serial.println("");
    digitalWrite(r1, LOW);
    digitalWrite(r2, LOW);
    digitalWrite(r3, HIGH);
}

void turnOffAllRelays() {
    Serial.println("---");
    Serial.println("ВЫКЛЮЧАЕМ все реле");
    Serial.println("");
    digitalWrite(r1, LOW);
    digitalWrite(r2, LOW);
    digitalWrite(r3, LOW);
}

void printCurrentTime() {
    DateTime now = rtc.now();
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
}

void setCurrentTime() {
    cl.setDoW(SATURDAY);
    cl.setYear(24);
    cl.setMonth(2);
    cl.setDate(18);
    cl.setHour(11);
    cl.setMinute(38);
    cl.setSecond(0);
}