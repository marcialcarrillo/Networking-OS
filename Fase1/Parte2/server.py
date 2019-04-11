import socket
import threading
import queue
import os
import configparser
import time

class Data:
    def __init__(self, oracion, numPalabras, addr):
        self.oracion = oracion
        self.numPalabras = numPalabras
        self.ip = addr[0]
        self.port = addr[1]

class UserInputManager(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)

    def printResults(self, ip, port):
        print("\n")
        for data in resultData:
            counter = 0
            if data.ip == ip and int(data.port) == port:
                print("%s \t %d \t %s %s" % (data.oracion, data.numPalabras, data.ip, data.port))
                counter += 1
        if counter == 0:
            print("No matches for that ip and port %s %s %s %s" % (ip, port, data.ip, data.port))

    def run(self):
        while True:
            userInput = input()
            if userInput == "print":
                ip = input("Type the ip: ")
                port = input("Type the port: ")
                self.printResults(str(ip), int(port))
            elif userInput == "exit":
                os._exit(1)


            
class BufferManager(threading.Thread):
    def __init__(self, host, port):
        threading.Thread.__init__(self)
        self.host = host
        self.port = port

    def run(self):

        print("waiting connection")
        mySocket = socket.socket()
        mySocket.bind((host, port))
        mySocket.listen()
        conn, addr = mySocket.accept()
        print("Connection from: ", addr)

        while True:
            string = conn.recv(1024).decode()
            if not string:
                conn.close()
                print("connection lost, waiting for connection...")
                conn, addr = mySocket.accept()
                print("Connection from -: " + str(addr))
            else:
                cola.put(Data(string, 0, addr))
                response = str((outQueue.get()).numPalabras)
                conn.send(response.encode())                
        conn.close()    


class CountManager(threading.Thread):
    def __init__(self, delay):
        threading.Thread.__init__(self)
        self.delay = delay

    def run(self):
        while workDone[0] == False:
            data = cola.get(True,None)
            if data != -1:
                result = len(data.oracion.split())
                time.sleep(delay)
                data.numPalabras = result
                resultData.append(data)
                outQueue.put(data)



workDone = []

printCommand = []

cola = queue.Queue()
outQueue = queue.Queue()


resultData = []

workDone.append(False)

config = configparser.ConfigParser()
config.sections()
config.read('server.ini')
host = config['DEFAULT']['IP']
port = int(config['DEFAULT']['Port'])
delay = int(config['DEFAULT']['Delay'])

# Create new threads
thread1 = BufferManager(host, port)
thread2 = CountManager(delay)
thread3 = UserInputManager()

# Start new Threads
thread1.start()
thread2.start()
thread3.start()

thread1.join()
thread2.join()
thread3.join()