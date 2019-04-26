import json, socket

serverSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
serverSocket.bind(('localhost', 10000))

while 1:
    # Listen for data from UDP
    print("Waiting for packet...")
    data, addr = serverSocket.recvfrom(2048)
    print("Packet received!")
    txInfoJSON = json.loads(data.decode("utf-8"))
    txPowerLevel = txInfoJSON["txPowerLevel"]
    txVoltage = txInfoJSON["txVoltage"]
    txCurrent = txInfoJSON["txCurrent"]
    txPower = txInfoJSON["txPower"]

    print(txPowerLevel, txVoltage, txCurrent, txPower)