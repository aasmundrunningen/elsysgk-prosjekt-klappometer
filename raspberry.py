from pygame import mixer
mixer.init()
#from time import time

import time
import RPi.GPIO as GPIO
GPIO.setmode(GPIO.BCM)



# # pinsetup:
# # bra jobba: pin 12
# # litt hoyere: pin 20
# # introslutt: pin 16
# # introstart: pin 12
# sang spiller: pin 26

goodWorkPin = 23
higherPin = 7
introStoppPin = 8
introStartPin = 25
songFeedbackPin = 24

GPIO.setup(goodWorkPin, GPIO.IN)
GPIO.setup(higherPin, GPIO.IN)
GPIO.setup(introStoppPin, GPIO.IN)
GPIO.setup(introStartPin, GPIO.IN)
GPIO.setup(songFeedbackPin, GPIO.OUT)

#mixer.music.load("god work.wav")


secTimer = 0

while True:
    #print("pin 21: {}, pin 20: {}, pin 16: {}, pin 12: {}".format(GPIO.input(21), GPIO.input(20), GPIO.input(16), GPIO.input(12)))

    if mixer.music.get_busy():
        GPIO.output(songFeedbackPin, GPIO.HIGH)
        #print("playing")
        if time.time() > secTimer + 1:
            print("playing")
            secTimer = time.time()
    else:
        GPIO.output(songFeedbackPin, GPIO.LOW)
        if GPIO.input(goodWorkPin):
            mixer.music.load("god work.wav")
            mixer.music.play()
            print("playing good work")
        elif GPIO.input(higherPin):
            mixer.music.load("higher.wav")
            mixer.music.play()
            print("playing higher")
        elif GPIO.input(introStoppPin):
            mixer.music.load("introend.wav")
            mixer.music.play()
            print("playing introend")
        elif GPIO.input(introStartPin):
            mixer.music.load("introstart.wav")
            mixer.music.play()
            print("playing introstart")    
