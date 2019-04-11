import socket
def Main(): #Servidor
    host = '127.0.0.1' #Ip del host
    port = 4000
    mySocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    mySocket.bind((host,port))
     
    mySocket.listen(2)
    conn, addr = mySocket.accept()
    print ("Connection from: " + str(addr))
    while True:
            data = conn.recv(1024).decode()
            if not data:
                    break
            print ("from connected  user: " + str(data))
            data = str(data).upper()
            print ("sending: " + str(data))
            conn.send(data.encode())
    conn.close()
     
if __name__ == '__main__':
    Main()