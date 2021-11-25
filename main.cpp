#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <Arduino.h>
#include "MCP3XXX.h"
#include <FastLED.h>

#define buttonPin 22
bool buttonPressed;

//values for the ledstrip
#define ledPin 15
#define numLeds 45
CRGB leds[numLeds];

//pins for raspberrypi
#define introStartPin 33
#define introSluttPin 32
#define higherPin 25
#define goodWorkPin 27
#define playingMusicPin 26
bool stoppedPlaying = false;

MCP3001 adc;
TaskHandle_t task1;

class mic{
    private:
        //values for rawData from ADC
        int volatile rawData[4][1000]; //stores the rawData
        int volatile rawDataIndex[2] = {0, 0};
        bool volatile dataFlags[4]; //the list which is currently being writen two is high
        int volatile dataTime[4] = {0, 0, 0, 0}; //time used to fill the list

        
        float tenSecAverage[400]; //stores the averages of 1000 rawDatapoints
        int tenSecAverageInd = 0;

        int clappTarget = 10; //number of detected clapps in 10 seconds to let code continue  

    public:   
        //reads and stores data from the ADC     
        void sensorReading(){
            adc.begin();

            for(;;){
                rawData[rawDataIndex[0]][rawDataIndex[1]] = adc.analogRead(0);
                rawDataIndex[1] ++;
                
                //runs when list is full. Changes which list too write data too
                //and calculates time used too fill the list
                if (rawDataIndex[1] >= 1000){
                    dataTime[rawDataIndex[0]] = millis() - dataTime[rawDataIndex[0]];
                    rawDataIndex[1] = 0;
                    for (int i = 0; i < 4; i++){dataFlags[i] = false;}
                    dataFlags[rawDataIndex[0]] = true;
                    rawDataIndex[0] = (rawDataIndex[0] + 1)%4;
                    delay(10); //gives the core have time too run other tasks
                    dataTime[rawDataIndex[0]] = millis();
                }
            }
        }

        //runs through the rawData and calculates the average of the filled lists of data.
        void prossesSensorReadings(){
            for (int i = 0; i < 4; i++){
                //checks if this is the last updated datalist and findes the average of the list
                if (dataFlags[i] == true){
                    float sum = 0;
                    for (int a = 0; a < 1000; a++){
                        sum += pow(rawData[i][a]/100, 2);
                    }
                    sum /= 1000;

                    tenSecAverage[tenSecAverageInd] = sum;
                    tenSecAverageInd = (tenSecAverageInd + 1)%400;
                    dataFlags[i] = false;
                }
            }
        }

        float getClappLevel(){
            float clappLevel = 0;
            for (int i = 0; i < 400; i++){
                if (tenSecAverage[i] > 1){clappLevel ++;}
            
            }
            return clappLevel / clappTarget;
        }

};

mic microphone;

//code too run at core 0
void core0(void * pvParameters){
    microphone.sensorReading();
}

void ledSetup(){
    FastLED.addLeds<WS2812B, ledPin, GRB>(leds, numLeds);  // GRB ordering is typical
}

//lights up the leds depending on how hugh the value v is between 0 and 1
void ledControll(int pin, int nLeds, float v){
    if (v > 1.0){v = 1.0;}
    else if (v < 0){v = 0;}
    for (int i = 0; i<nLeds; i++){leds[i] = CRGB(0, 0, 0);}

    int l = round(nLeds/3 * v); //numbers of leds to light up
    for (int i = 0; i < l; i++){
        leds[i] = CRGB(255, 0, 0);
        leds[2*nLeds/3 - i - 1] = CRGB(255, 0, 0);
        leds[2*nLeds/3 + i] = CRGB(255, 0, 0);
    }
    //leds[10] = CRGB(255, 0, 0);
    FastLED.show();
}

//detects buttonpress
void IRAM_ATTR button(){
  buttonPressed = true;
}

void IRAM_ATTR raspberry_fedback(){
    stoppedPlaying = true;
}

int stepStartTime = 0;
int step = 0;



void setup(){
    Serial.begin(115200);
    ledSetup();
    pinMode(buttonPin, INPUT_PULLUP);
    
    pinMode(introStartPin,  OUTPUT);
    pinMode(introSluttPin, OUTPUT);
    pinMode(higherPin, OUTPUT);
    pinMode(goodWorkPin, OUTPUT);
    pinMode(playingMusicPin, INPUT_PULLUP);

    //sets microphone.sensorReading too run on core 0
    xTaskCreatePinnedToCore(core0, "task1", 1000, NULL, 10, &task1, 0);

    attachInterrupt(buttonPin,button,FALLING);
    attachInterrupt(playingMusicPin, raspberry_fedback, FALLING);

    stepStartTime = millis();
    ledControll(ledPin, numLeds,0.0);
}

void loop(){
    microphone.prossesSensorReadings();

    /*
    Step 0 -> waiting for buttonpress
        - clapplevel activated?
    Step 1 -> playing "introstart"
        - clapplevel activated?
    Step 2 -> waiting for high enough clapping
        - if time > 10s -> Step 3
        - if clapplevel > minimumlevel -> Step 4
    Step 3 -> playing "litt hoyere"
        - if song finished -> Step 2
    Step 4 -> playing introslutt
        - if song finished -> step 5
    Step 5 -> playing bra jobba
        - if song finished -> step 0
    */  

    
    switch (step){
        case 0: //waiting for buttonpress
            stoppedPlaying = false;
            
            if (buttonPressed){
                Serial.println("0 -> 1");
                buttonPressed = false;
                step = 1;
                stepStartTime = millis();
            }
            break;
        case 1: //playing start off intromusic
            if (millis() < stepStartTime + 1000){digitalWrite(introStartPin, HIGH);}
            else {digitalWrite(introStartPin, LOW);}
            
            if (stoppedPlaying == true){
                Serial.println("1 -> 2");
                digitalWrite(introStartPin, LOW);
                step = 2;
                stoppedPlaying = false;
                stepStartTime = millis();
            }
            break;
        case 2: //waiting for clapping too cross treshold
            stoppedPlaying = false;
            ledControll(ledPin, numLeds, microphone.getClappLevel());
            
            if (stepStartTime + 10000 < millis()){
                Serial.println("2 -> 3");
                step = 3;
                stepStartTime = millis();
            }
            else if (microphone.getClappLevel() >= 1.0){
                Serial.println("2 -> 4");
                step = 4;
                stepStartTime = millis();
            }
            break;
        case 3: //playing "clapp higher"
            if (millis() < stepStartTime + 1000){digitalWrite(higherPin, HIGH);}
            else {digitalWrite(higherPin, LOW);}
            
            ledControll(ledPin, numLeds,0.0);
            if (stoppedPlaying == true){
                Serial.println("3 -> 2");
                digitalWrite(higherPin, LOW);
                step = 2;
                stoppedPlaying = false;
                stepStartTime = millis();
            }
            break;
        case 4: //playing end of intromusic
            if (millis() < stepStartTime + 1000){digitalWrite(introSluttPin, HIGH);}
            else {digitalWrite(introSluttPin, LOW);}
            ledControll(ledPin, numLeds,0.0);
            if (stoppedPlaying == true){
                Serial.println("4 -> 5");
                digitalWrite(introSluttPin, LOW);
                step = 5;
                stoppedPlaying = false; 
                stepStartTime = millis();
            }
            break;
        case 5: //playing "good work"
            if (millis() < stepStartTime + 1000){digitalWrite(goodWorkPin, HIGH);}
            else {digitalWrite(goodWorkPin, LOW);}
            
            if (stoppedPlaying == true){
                Serial.println("5 -> 0");
                digitalWrite(goodWorkPin, LOW);
                step = 0;
                stoppedPlaying = false;
                stepStartTime = millis();
                buttonPressed = false;
            }
            break;
        }
}