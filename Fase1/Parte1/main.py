import time
import threading
import datetime
import sys

class Data:
    oracion = ""
    numPalabaras = 0
    def __init__(self,oracion,numPalabaras):
        self.oracion = oracion
        self.numPalabaras = numPalabaras


class Lector (threading.Thread):
    list = []
    def __init__(self):
        threading.Thread.__init__(self)
        print("Im lector")

    def printResults(self):
        for data in self.list:
            if data.numPalabaras != -1:
                print("%s \t Palabras: %d" % (data.oracion,data.numPalabaras))
            else:
                print("%s \t Palabras: no procesada" % (data.oracion))

    def run(self):
       index = 0
       lastIdexSend = 0
       while True:

        oracion= input("inserte la oracion: ")

        if working[0] == 2:
                temp = self.list[lastIdexSend]
                temp.numPalabaras = resultadoContador[0]
                ##sem.release()
                lastIdexSend += 1
                working[0] = 0 

        if len(oracion) == 1:
         if oracion[0] == '1':
             workDone[0]=True
             sem.release()
             self.printResults()
             break

        else:
            self.list.append(Data(oracion, -1))

            
            if working[0] == 0: ##Si esta libre
                data = self.list[index]
                buffer[0] = data.oracion
                sem.release()
                index += 1

           


class Contador (threading.Thread):
    delay = 0
    def __init__(self, final_delay):
        print("Im counter")
        self.delay = final_delay
        threading.Thread.__init__(self)

    def run(self):
        while True:
            ##print("waiting on semaphore!")
            sem.acquire()

            if workDone[0]:
                ##print("workDone set to TRUE")
                break

            working[0] = 1
            ##print("counting %s..." % (buffer[0]))

            resultado = len(buffer[0].split())##Cuenta las palabras

            time.sleep(int(delay))
            resultadoContador[0] = resultado
            working[0] = 2
            ##print("done counting, got %s in the shared and %d in the result" % (resultadoContador[0],resultado))
            

            ##print("im out")



if __name__ == '__main__':

    sem = threading.Semaphore(0)
    buffer = []
    resultadoContador = []
    working = []
    workDone = []

    delay = sys.argv[1]
    working.append(0) ## 0 = not working ## 1 = working ## 2 finished counting the word
    buffer.append("")
    resultadoContador.append(0)
    workDone.append(False)
    # Create new threads
    thread1 = Lector()
    thread2 = Contador(delay)

    # Start new Threads
    thread1.start()
    thread2.start()

    thread1.join()
    ##print("thread1 is done")
    thread2.join()
    ##print("thread2 is done")
    print ("Exiting the Program!!!")
