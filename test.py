import socket
import sys

# Create a UDP socket
serverSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Bind the socket to the port
server_address = ('localhost', 10000)
serverSocket.bind(server_address)

while True:
    print('\nwaiting to receive message')
    data, address = serverSocket.recvfrom(4096)


    print(data.decode())