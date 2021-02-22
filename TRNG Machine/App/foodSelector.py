import serial
import _thread
import googlemaps
from datetime import datetime
from time import sleep

googleAPIKey = ""
serialPort = serial.Serial()
seedStart = "SEEDBEGIN:"
seedEnd = ":SEEDEND"
magicNumbers = []

#Converts a binary string into a number
def b2d(binaryString):
    result = 0
    it = 0
    for bit in binaryString:
        if (bit == "1"):
            result += 1 * (pow(2, (len(binaryString) - (it + 1))))
        it += 1
    return result

#Serial Recieve Thread
def recvLoop():
    global serialPort, magicNumbers
    while True:
        raw = serialPort.readline()
        if (seedEnd in raw.decode()):
            print(raw)
            #strip number out
            raw = raw.decode().strip('\r').strip('\n')
            print(raw)
            raw.replace(seedStart, '').replace(seedEnd, '')
            print(raw)
            #add to magic numbers
            magicNumbers.append(str(b2d(raw)))

#Entry
def main():
    #load api key
    apiFile = open("key")
    googleAPIKey = apiFile.readline()
    apiFile.close()

    print("GMAPS")
    gmaps = googlemaps.Client(key=googleAPIKey)
    reverse_geocode_result = gmaps.reverse_geocode((40.714224, -73.961452))
    print(reverse_geocode_result)

    #serial
    serialPort.baudrate = 9600
    serialPort.port = "COM8"
    serialPort.open()
    sleep(3)
    #spawn recv thread
    _thread.start_new_thread(recvLoop)

    count = 0
    while True:
        if (count != len(magicNumbers)):
            print("======================")
            print("UPDATE")
            print(magicNumbers)
            count = len(magicNumbers)



if __name__ == "__main__":
    main()