import serial
import _thread
import sys
import tkinter as tk
from selenium import webdriver
from selenium.webdriver.firefox.options import Options
from time import sleep
from datetime import timedelta

googleAPIKey = ""
serialPort = serial.Serial()
seedStart = "SEEDBEGIN:"
seedEnd = ":SEEDEND"
magicNumbers = []
prunedResults = []
serialConnected = False


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
    global serialPort, magicNumbers, serialConnected
    while serialConnected:
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

def magic(magicNumber):
    a = 48271
    c = 0
    modulus = sys.maxsize
    result = (a * int(magicNumber) + c) % modulus
    return result

#Pick a place to eat
def pickLocation():
    global prunedResults, magicNumbers
    if len(magicNumbers) <= 0:
        print("[-] No Magic Numbers Please Wait And Try Again")
        return

    #Perform the magic (Based on OpenBSD)
    result = 0
    range = len(prunedResults)
    fixBias = (-range) % range
    while True:
        magicDust = magic(magicNumbers[0])
        if (magicDust >= fixBias):
            result = magicDust % range
            break
    
    #remove magic number
    magicNumbers.remove(magicNumbers[0])
    print("-==========================-")
    print(result)
    print(prunedResults[result])
    print("-==========================-")
    return prunedResults[result]

def tkPick():
    locResult["text"] = pickLocation()


def logicLoop():
    global magicNumbers, seedStatus
    count = -1
    while True:
        if (len(magicNumbers) != count):
            if (len(magicNumbers) > 0):
                seedStatus["text"] = "Ready. <" + str(len(magicNumbers)) + ">"
            else:
                seedStatus["text"] = "Not Ready."
            count = len(magicNumbers)

        if locResult["text"] != "Chosen Lunch Spot: ":
            if (len(magicNumbers) > 0):
                seedStatus["text"] = "Ready. <" + str(len(magicNumbers)) + ">"
            else:
                seedStatus["text"] = "Not Ready."
            count = len(magicNumbers)
            sleep(10)
            locResult["text"] = "Chosen Lunch Spot: "
            

        
#Entry
def main():
    global prunedResults, magicNumbers, serialPort, serialConnected, window, seedStatus
    print("[+] Starting Lunch Bot...")
    
    queryFile = open("queries")
    queries = queryFile.readlines()
    queryFile.close()
    print("[+] Loaded Queries")

    #Use options to tell python where firefox is
    options = Options()
    options.binary_location = r"C:/Program Files/Mozilla Firefox/firefox.exe"
    browser = webdriver.Firefox(options=options)
    print("[+] Browser driver launched")

    #For all queries log results
    results = []
    count = 0
    for query in queries:

        #Format for web
        count += 1
        query = query.replace(' ', '+')
        query = "https://www.google.co.nz/maps/search/" + query

        #Go to Google Maps
        browser.get(query)

        browser.implicitly_wait(1)

        #Find all the elements with class name section-result
        tmpResults = browser.find_elements_by_class_name("section-result")
        for result in tmpResults:
            results.append(result.get_attribute("aria-label"))
            print(result.get_attribute("aria-label"))
        print("[+] Completed Query " + str(count) + "/" + str(len(queries)))
    
    print("[+] Completed All Queries")
    #close browser as it is no longer needed
    browser.close()

    #Get rid of dupilcate results
    print("[+] Pruning Results...")
    dupe = False
    for result in results:
        for prunedResult in prunedResults:
            if (result == prunedResult):
                print("[*] Pruned " + result)
                dupe = True
                break
        if dupe == False:
            prunedResults.append(result)
        dupe = False

    print("[+] Pruned " + str(len(results) - len(prunedResults)) + " Results")

    print("[+] Attempting To Connect On Serial")

    try:
        #Connect To Serial
        serialPort.baudrate = 9600
        serialPort.port = "COM8"
        serialPort.open()
        #Arduino needs to startup
        sleep(3)
        serialConnected = True
        print("[+] Serial Connected")
    except:
        print("[-] Serial Couldn't Connect, using debug list")
        serialConnected = False
        magicNumbers = [2118123519, 1983905735, 14580398] #random numbers

    #Spawn Serial Thread
    _thread.start_new_thread(recvLoop)
    _thread.start_new_thread(logicLoop)

    #Start TKinter
    window.title("TRNG Food Machine")
    window.geometry("250x100")
    seedStatus.grid(row = 0, column = 1, pady = 1)
    pickLocButton.grid(row = 3, column = 0, columnspan = 3, sticky=tk.W+tk.E)
    locResult.grid(row = 4, column = 0, columnspan = 3)
    window.mainloop()
    

#TKINTER
window = tk.Tk()

#Text
seedStatus = tk.Label(text="Error")
locResult = tk.Label(text="Chosen Lunch Spot: ")

#Buttons
pickLocButton = tk.Button(
    text="Choose Location",
    width=35,
    height=3,
    bg="white",
    fg="black",
    command=tkPick
)

if __name__ == "__main__":
    main()
