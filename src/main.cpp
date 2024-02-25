#include <Arduino.h>
#include <Wire.h>
#include <DS3231.h>
#include <time.h>
#include <EEPROM.h>
#include "models.cpp"

DS3231 cl;
RTClib rtc;

// Пины управления реле
int regime1_relay = 5;
int regime2_relay = 6;
int regime3_relay = 7;

// Таймеры
Timer timer1 = Timer();
Timer timer2 = Timer();
Timer timer3 = Timer();

/** Текущий режим 
* 1 - Режим принудительного включения\выключения
* 2 - Режим работы по таймеру
* 3 - Режим работу по фотоэлементу
*/
int current_regime = -1;

// EEPROM адресы
int timer_1_addr = 1;
int timer_2_addr = 101;
int timer_3_addr = 202;
int current_regime_addr = 303;

void printCurrentTime();
void setCurrentTime();
void observeRegimeChanges();
void checkRegime2Alarms();
void turnOnRegime1Relay();
void turnOnRegime2Relay();
void turnOnRegime3Relay();
void observeGsmCommands();
void turnOffAllRelays();
void restoreRegime();
void restoreTimers();
void printTimersAndRegime();
void setTimer(int num, Timer t);
void setRegime(int regime);
void setTestRegimeAndTimers();

void setup() {

    Serial.begin(9600);
    Wire.begin();

    // setCurrentTime(); // тут можно установить время для RTC модуля
    setTestRegimeAndTimers();

    restoreRegime();
    restoreTimers();

    pinMode(regime1_relay, OUTPUT);
    pinMode(regime2_relay, OUTPUT);
    pinMode(regime3_relay, OUTPUT);

    Serial.println("Board Ready!");
}

void loop() {

    delay(1000);
    printCurrentTime();
    printTimersAndRegime();
    observeGsmCommands();
    observeRegimeChanges();
}

void restoreRegime() {
    EEPROM.get(current_regime_addr, current_regime);
}

void restoreTimers() {
    EEPROM.get(timer_1_addr, timer1);
    EEPROM.get(timer_2_addr, timer2);
    EEPROM.get(timer_3_addr, timer3);
}

void observeGsmCommands() {
    // Устанавливаем режим в зависимости от команды
}

void setRegime(int regime) {
    Serial.println("");
    Serial.println("Устанавливаем режим ");
    Serial.print(regime);
    Serial.println("");
    current_regime = regime;
    EEPROM.put(current_regime_addr, regime);
}

void setTimer(int num, Timer t) {
    if (num == 1) {
        Serial.println("---");
        Serial.println("Устанавливаем таймер 1 ");
        Serial.println("");
        timer1 = t;
        EEPROM.put(timer_1_addr, t);
    } else if (num == 2) {
        Serial.println("---");
        Serial.println("Устанавливаем таймер 2 ");
        Serial.println("");
        timer2 = t;
        EEPROM.put(timer_2_addr, t);
    } else if (num == 3) {
        Serial.println("---");
        Serial.println("Устанавливаем таймер 3 ");
        Serial.println("");
        timer3 = t;
        EEPROM.put(timer_3_addr, t);
    }
    Serial.print("ВКЛЮЧАЕМ в: ");
    Serial.print(t.hour_On);
    Serial.print(":");
    Serial.print(t.min_On);
    Serial.print(" ВЫКЛЮЧАЕМ в: ");
    Serial.print(t.hour_Off);
    Serial.print(":");
    Serial.print(t.min_Off);
}

void observeRegimeChanges() {
    if (current_regime == 1) {
        Serial.println("Режим работы принудительного включения/выключения АКТИВЕН");
        turnOnRegime1Relay();
    } else if (current_regime == 2) {
        Serial.println("Режим работы по таймеру АКТИВЕН");
        checkRegime2Alarms();
    } else if (current_regime == 3) {
        Serial.println("Режим работы по фотоэлементу АКТИВЕН");
        turnOnRegime3Relay();
    } else {
        Serial.println("Режим работы НЕ УСТАНОВЛЕН");
    }
}

void checkRegime2Alarms() {
        
    DateTime now = rtc.now();
    int currentHour = now.hour();
    int currentMin = now.minute();

    if (
        (currentHour == timer1.hour_On && currentMin == timer1.min_On)
        || (currentHour == timer2.hour_On && currentMin == timer2.min_On)
        || (currentHour == timer3.hour_On && currentMin == timer3.min_On)
    ) {
        Serial.println("---");
        Serial.println("Наступило время ВКЛЮЧИТЬ реле в режиме работы по таймеру");
        Serial.println("");
        turnOnRegime2Relay();
    }

    if (
        (currentHour == timer1.hour_Off && currentMin == timer1.min_Off)
        || (currentHour == timer2.hour_Off && currentMin == timer2.min_Off)
        || (currentHour == timer3.hour_Off && currentMin == timer3.min_Off)
    ) {
        Serial.println("---");
        Serial.println("Наступило время ВЫКЛЮЧИТЬ реле в режиме работы по таймеру");
        Serial.println("");
        digitalWrite(regime2_relay, LOW);
    }
}

void turnOnRegime1Relay() {
    Serial.println("---");
    Serial.println("ВКЛЮЧАЕМ РЕЛЕ принудительного режима работы");
    Serial.println("");
    digitalWrite(regime1_relay, HIGH);
    digitalWrite(regime2_relay, LOW);
    digitalWrite(regime3_relay, LOW);
}

void turnOnRegime2Relay() {
    Serial.println("---");
    Serial.println("ВКЛЮЧАЕМ РЕЛЕ режима работы по таймеру");
    Serial.println("");
    digitalWrite(regime1_relay, LOW);
    digitalWrite(regime2_relay, HIGH);
    digitalWrite(regime3_relay, LOW);
}

void turnOnRegime3Relay() {
    Serial.println("---");
    Serial.println("ВКЛЮЧАЕМ РЕЛЕ режима работы по таймеру");
    Serial.println("");
    digitalWrite(regime2_relay, LOW);
    digitalWrite(regime2_relay, LOW);
    digitalWrite(regime2_relay, HIGH);
}

void turnOffAllRelays() {
    Serial.println("---");
    Serial.println("ВЫКЛЮЧАЕМ все реле");
    Serial.println("");
    digitalWrite(regime1_relay, LOW);
    digitalWrite(regime2_relay, LOW);
    digitalWrite(regime3_relay, LOW);
}

void printCurrentTime() {
    DateTime now = rtc.now();
    Serial.println();
    Serial.println();
    Serial.println("---");
    Serial.println("Текущее время: ");
    Serial.println();
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
    Serial.println();
}

void printTimersAndRegime() {
    Serial.println();
    Serial.println("---");
    Serial.print("Текущий режим: ");
    Serial.print(current_regime);
    Serial.println();
    Serial.println("Таймер 1: ");
    Serial.print("ВКЛЮЧЕНИЕ в: ");
    Serial.print(timer1.hour_On);
    Serial.print(":");
    Serial.print(timer1.min_On);
    Serial.print("  ");
    Serial.print("ВЫКЛЮЧЕНИЕ в: ");
    Serial.print(timer1.hour_Off);
    Serial.print(":");
    Serial.print(timer1.min_Off);
    Serial.println("");
    Serial.println("Таймер 2: ");
    Serial.print("ВКЛЮЧЕНИЕ в: ");
    Serial.print(timer2.hour_On);
    Serial.print(":");
    Serial.print(timer2.min_On);
    Serial.print("  ");
    Serial.print("ВЫКЛЮЧЕНИЕ в: ");
    Serial.print(timer2.hour_Off);
    Serial.print(":");
    Serial.print(timer2.min_Off);
    Serial.println("");
    Serial.println("Таймер 3: ");
    Serial.print("ВКЛЮЧЕНИЕ в: ");
    Serial.print(timer3.hour_On);
    Serial.print(":");
    Serial.print(timer3.min_On);
    Serial.print("  ");
    Serial.print("ВЫКЛЮЧЕНИЕ в: ");
    Serial.print(timer3.hour_Off);
    Serial.print(":");
    Serial.print(timer3.min_Off);
    Serial.println();
    Serial.println();
}

void setCurrentTime() {
    cl.setDoW(SATURDAY);
    cl.setYear(24);
    cl.setMonth(2);
    cl.setDate(25);
    cl.setHour(16);
    cl.setMinute(15);
    cl.setSecond(30);
}

void setTestRegimeAndTimers() {
    Timer t1_test = Timer();
    t1_test.hour_On = 16;
    t1_test.hour_Off = 16;
    t1_test.min_On = 29;
    t1_test.min_Off = 31;
    setTimer(1, t1_test);

    Timer t2_test = Timer();
    t2_test.hour_On = 19;
    t2_test.hour_Off = 19;
    t2_test.min_On = 22;
    t2_test.min_Off = 30;
    setTimer(2, t2_test);

    Timer t3_test = Timer();
    t3_test.hour_On = 21;
    t3_test.hour_Off = 21;
    t3_test.min_On = 20;
    t3_test.min_Off = 30;
    setTimer(3, t3_test);

    setRegime(2);
}