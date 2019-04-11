import socket
import threading
import queue
import sys
import configparser


class Data:
    def __init__(self, oracion, numPalabras):
        self.oracion = oracion
        self.numPalabras = numPalabras


class socketManager(threading.Thread):
	def __init__(self):
		threading.Thread.__init__(self)
		self.continueWorking = True

	def run(self): #Cliente
		global startAsking
		global normal_exit
		global stopAll

		config = configparser.ConfigParser()
		config.sections()
		config.read('client.ini')
		host = config['DEFAULT']['IP']
		port = int(config['DEFAULT']['Port'])

		# Trying to make a connection with the server
		try:
			mySocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
			mySocket.connect((host,port))
			print('Connection Sucessful')
		except:
			print("Server is down")
			startAsking = False
			self.continueWorking = False
			normal_exit = True

		resultsIndex = 0  # index to the last result received from the server
		while self.continueWorking and not stopAll:

			startAsking = True

			if not inputQueue.empty():
				outboundMessage = inputQueue.get()
				if outboundMessage != 'exit':
					mySocket.send(outboundMessage.encode())
					response = mySocket.recv(1024).decode()
					
					if response == "":
						print("Server down press enter ...")
						mySocket.close()
						self.continueWorking = False
						normal_exit = True
					else:
						results[resultsIndex].numPalabras = int(response)
						resultsIndex += 1
				else:
					mySocket.close()
					self.continueWorking = False
			
        
class readingManager(threading.Thread):
	def __init__(self):
		threading.Thread.__init__(self)

	def printResults(self):
		print("\n")
		for data in results:
			print("%s \t %d" % (data.oracion, data.numPalabras))

	def run(self):
		global normal_exit
		global stopAll
		while not normal_exit:
			if startAsking:
				message = ".."
				while message != 'exit' and not normal_exit:
					message = input()
					if message == "exit":
						normal_exit = True
					elif message == "print":
						self.printResults()

					else:
						results.append(Data(message, 0))
						inputQueue.put(message)
		stopAll = True

					
inputQueue = queue.Queue()
results = []

normal_exit = False
startAsking = False
stopAll = False

thread1 = readingManager()
thread2 = socketManager()

thread1.start()
thread2.start()

thread1.join()
thread2.join()