import serial
import _thread
from selenium import webdriver
from selenium.webdriver.firefox.options import Options
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

        #Find all the elements with a section-result class
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
    prunedResults = []
    dupe = False
    for result in results:
        for prunedResult in prunedResults:
            if (result == prunedResult):
                print("FOUND DUPE! " + result + " == " + prunedResult)
                dupe = True
                break
        if dupe == False:
            prunedResults.append(result)
        dupe = False

    print("[+] Pruned " + str(len(results) - len(prunedResults)) + " Results")

    print("[+] Attempting To Connect On Serial")
    #serial
    serialPort.baudrate = 9600
    serialPort.port = "COM8"
    serialPort.open()
    #Arduino needs to startup
    sleep(3)
    print("[+] Serial Connected")
    
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