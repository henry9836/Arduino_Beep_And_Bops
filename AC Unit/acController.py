import serial
import tkinter as tk
from time import sleep

override = False
tmpTarget = 21
roomTemp = 25
airTemp = 23
targetSensor = "ROOM"
threshold = 1

serInterface = serial.Serial('COM12', 9600, timeout=5)
sleep(5)

def toggleOverride():
    global override, tmpTarget, roomTemp, airTemp, targetSensor, threshold
    if override is False:
        serInterface.write(("#1:1############").encode())
        overrideButton["text"] = "Turn On"
        overrideButton["fg"] = "green"
    elif override is True:
        serInterface.write(("#1:0############").encode())
        overrideButton["text"] = "Turn Off"
        overrideButton["fg"] = "red"
    getStats()

def getStats():
    global override, tmpTarget, roomTemp, airTemp, targetSensor, threshold
    serInterface.write(("#2:#############").encode())
    data = (serInterface.read_until(expected='&')).decode("utf-8")
    data = data.replace('\r', '')
    data = data.split('\n')
    print(data)
    try:
        print("BOOL IS:" + (data[1][data[1].find(":")+1:]))
        if ((data[1][data[1].find(":")+1:]) == "1"):
            override = True
        else:
            override = False
        tmpTarget = data[2][data[2].find(":")+1:]
        roomTemp = data[3][data[3].find(":")+1:]
        airTemp = data[4][data[4].find(":")+1:]
        targetSensor = data[5][data[5].find(":")+1:]
        threshold = data[6][data[6].find(":")+1:]
        infomation["text"] = "OVERRIDE:" + str(override) + "\nTEMPERTURE TARGET:" + tmpTarget + "\nROOM TEMPERTURE:" + roomTemp + "\nAIR TEMPERTURE:" + airTemp + "\nTARGET SENSOR:" + targetSensor + "\nTHRESHOLD:" + threshold
    except:
        print("Error Parsing...")
def setNewTemp():
    global override, tmpTarget, roomTemp, airTemp, targetSensor, threshold, tmpEntry
    serInterface.write(("#3:"+tmpEntry.get()+"###########").encode())
    getStats()

def setNewThres():
    global override, tmpTarget, roomTemp, airTemp, targetSensor, threshold, thresholdEntry
    if (len(thresholdEntry.get()) > 1):
        serInterface.write(("#5:"+thresholdEntry.get()+"###########").encode())
    else:
        serInterface.write(("#5:"+thresholdEntry.get()+"############").encode())
    getStats()
def toggleSensor():
    serInterface.write(("#4:#############").encode())
    getStats()

window = tk.Tk()
window.title("AC Control")
window.geometry("300x500")
infomation = tk.Label(text="Loading...")
tmpLabel = tk.Label(text="Temperture Target: ")
thresLabel = tk.Label(text="Threshold Target: ")
ChangeSensor = tk.Label(text="Change Sensor Target")
overrideButton = tk.Button(
    text="Turn Off",
    width=35,
    height=3,
    bg="white",
    fg="red",
    command=toggleOverride
)
updateButton = tk.Button(
    text="Update Stats",
    width=35,
    height=3,
    bg="white",
    fg="black",
    command=getStats
)
setNewTmp = tk.Button(
    text="Set",
    width=5,
    height=1,
    bg="white",
    fg="black",
    command=setNewTemp
)
setNewThrs = tk.Button(
    text="Set",
    width=5,
    height=1,
    bg="white",
    fg="black",
    command=setNewThres
)
toggleSensorButton = tk.Button(
    text="Toggle Target Sensor",
    width=35,
    height=2,
    bg="white",
    fg="black",
    command=toggleSensor
)

tmpEntry = tk.Entry(fg="black", bg="white", width=10)
thresholdEntry = tk.Entry(fg="black", bg="white", width=10)

infomation.grid(row = 0, column = 0, pady = 1)
overrideButton.grid(row = 1, column = 0, pady =1)
updateButton.grid(row = 2, column = 0, pady =1)
tmpLabel.grid(row = 3, column = 0, pady =1)
tmpEntry.grid(row = 4, column = 0, pady =2)
setNewTmp.grid(row = 4, column = 1, pady =2)
thresLabel.grid(row = 5, column = 0, pady =1)
thresholdEntry.grid(row = 6, column = 0, pady =1)
setNewThrs.grid(row = 6, column = 1, pady =1)
toggleSensorButton.grid(row = 7, column = 0, pady =1)

getStats()

window.mainloop()