#include <Arduino.h>

/*
  The circuit:
 * LCD RS pin to digital pin 8
 * LCD Enable pin to digital pin 9
 * LCD D4 pin to digital pin 4
 * LCD D5 pin to digital pin 5
 * LCD D6 pin to digital pin 6
 * LCD D7 pin to digital pin 7
 * LCD BL pin to digital pin 10
 * KEY pin to analogl pin 0
 */

#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 13, 9, 4, 5, 6, 7);

#define NUM_KEYS  5
int adc_key_val[NUM_KEYS] = {50, 200, 400, 600, 800 };

#define TEMP_PIN 1

#define NUM_READINGS 10
#define READING_DELAY 1000

float temperatures[NUM_READINGS];
float total = 0;
int temp_idx = 0;
bool temps_initialized = false;

float read_temperature() {
    // read the temperature sensor and convert the result to degrees Celsius
    float sensor = analogRead(TEMP_PIN);
    float voltage = (sensor * 5000) / 1024; // convert raw sensor value to millivolts
    voltage = voltage - 500; // remove voltage offset
    float celsius = voltage / 10; // convert millivolts to Celsius

    return celsius;
}

void setup()
{
    lcd.clear();
    lcd.begin(16, 2);
    lcd.setCursor(0, 0);

    // Initialize with a bunch of readings
    for (int idx = 0; idx < NUM_READINGS; idx++) {
        temperatures[idx] = read_temperature();
        total = total + temperatures[idx]; 
    }

}

// Convert ADC value to key number
int get_key(unsigned int input)
{
    int k;

    for (k = 0; k < NUM_KEYS; k++) {
        if (input < adc_key_val[k]) {
            return k;
        }
    }

    if (k >= NUM_KEYS) {
        k = -1;    // No valid key pressed
    }

    return k;
}

float average_temp(float curr_reading)
{
    // subtract the last reading:
    total = total - temperatures[temp_idx];

    // add new value
    temperatures[temp_idx] = curr_reading;

    // add the reading to the total:
    total = total + temperatures[temp_idx];

    // advance to the next position in the array:
    temp_idx++;

    // if we're at the end of the array...
    if (temp_idx >= NUM_READINGS)
        // ...wrap around to the beginning:
    {
        temp_idx = 0;
    }

    // calculate the average:
    float average = total / NUM_READINGS;

    return average;
}

void display_temps() {
    float celsius = average_temp(read_temperature());
    float fahrenheit = (celsius * 9.0) / 5.0 + 32.0;

    lcd.setCursor(0, 0);
    lcd.print(celsius, 1);
    lcd.print(" C ");

    lcd.setCursor(0, 1);
    lcd.print(fahrenheit, 1);
    lcd.print(" F ");
}

void loop()
{
    // Once temperatures are initialized keep displaying
    // them periodically
    display_temps();
    delay(READING_DELAY);
}
