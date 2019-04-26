# Example of interaction with a BLE UART device using a UART service
# implementation.
# Author: Tony DiCola
import Adafruit_BluefruitLE
from Adafruit_BluefruitLE.services import UART
from time import sleep
import json, socket
import xlwt

# convert char to int
ASCII_A = ord('a')

# Get the BLE provider for the current platform.
ble = Adafruit_BluefruitLE.get_provider()

# Main function implements the program logic so it can run in a background
# thread.  Most platforms require the main thread to handle GUI events and other
# asyncronous events like BLE actions.  All of the threading logic is taken care
# of automatically though and you just need to provide a main function that uses
# the BLE provider.
def main():

	# create excel spreads
	# book = xlwt.Workbook(encoding="utf-8")
	# sheet1 = book.add_sheet("Sheet 1")
	# for n in range(0,12) :
	# 	sheet1.write(2, n, n + 1)

	serverSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	serverSocket.bind(('localhost', 10000))

	# Clear any cached data because both bluez and CoreBluetooth have issues with
	# caching data and it going stale.
	ble.clear_cached_data()

	# Get the first available BLE network adapter and make sure it's powered on.
	adapter = ble.get_default_adapter()
	adapter.power_on()
	print('Using adapter: {0}'.format(adapter.name))

	# Disconnect any currently connected UART devices.  Good for cleaning up and
	# starting from a fresh state.
	print('Disconnecting any connected UART devices...')
	UART.disconnect_devices()

	# Scan for UART devices.
	print('Searching for UART device...')
	try:
		adapter.start_scan()
		# Search for the first UART device found (will time out after 60 seconds
		# but you can specify an optional timeout_sec parameter to change it).
		device = UART.find_device()
		if device is None:
			raise RuntimeError('Failed to find UART device!')
	finally:
		# Make sure scanning is stopped before exiting.
		adapter.stop_scan()

	print('Connecting to device...')
	device.connect()  # Will time out after 60 seconds, specify timeout_sec parameter
					  # to change the timeout.

	# Once connected do everything else in a try/finally to make sure the device
	# is disconnected when done.
	try:
		# Wait for service discovery to complete for the UART service.  Will
		# time out after 60 seconds (specify timeout_sec parameter to override).
		print('Discovering services...')
		UART.discover(device)

		# Once service discovery is complete create an instance of the service
		# and start interacting with it.
		uart = UART(device)

		print('PC should decide speed mode first.')
		print('you can type 'r' whenever you want to reset.')
		print("    type '1': regular mode")
		print("    type '2': slow mode")
		print("    type '3': fast mode")
		speed = raw_input('-> ')
		# print "you type ",speed

		if speed == '1' or speed == '2' or speed == '3' :
			uart.write(speed)

		sleep(1)
		while speed != '1' :
			if speed == '2' :
				print "enter slow mode"
			elif speed == '3' :
				print "enter fast mode"
			else :
				print "Error: you type wrong value"
			speed = raw_input('-> ')
			if speed == '1' or speed == '2' or speed == '3' :
				uart.write(speed)

		print "enter regular mode"

		 # continue sweeping
		uart.write('y')

		state = 0
		currVal = [0] * 12
		maxCurrPath = 0
		i = 3

		while true :
		# for n in range(0, 400) :
			current_path = uart.read(timeout_sec = 60)
			currPath = ord(current_path) - ASCII_A


			# stdin = sys.stdin.read()
			# if ("\n" in stdin or "\r" in stdin):
			# 	idx = 0
			# 	firstSweep = 0 # flag after the first sweep
			# 	pathChange = 0 # flag when found the best path
			# 	initial_value = [0] * 12
			# 	reset = 0 # flag after the first
        	# 	break

			# file.read(2) read the first two characters and return it as a string
			# if (idx == len(lines)):
			# 	idx = 0

            # Listen for data from UDP
			# print("Waiting for packet...")
			data, addr = serverSocket.recvfrom(2048)
			print("Packet received!")
			txInfoJSON = json.loads(data.decode("utf-8"))
			txPowerLevel = txInfoJSON["txPowerLevel"]
			txVoltage = txInfoJSON["txVoltage"]
			txCurrent = txInfoJSON["txCurrent"]
			txPower = txInfoJSON["txPower"]

			print "		current_path : ", currPath
			print "		txCurrent	 : ", txCurrent # currVal #test
			print " "

			# swipe and store state
			if state == 0 :
				currVal[currPath - 1] = txCurrent

				# sheet1.write(i, currPath - 1, txCurrent) #saving in excel file

				# if (currPath == 11):
				#print currVal #test
				# if currPath == 1 and currVal[currPath - 1] >= 0.53 and currVal[currPath - 1] <= 0.56 :
				# 	maxCurrPath = currPath
				#  	print "path 1 detected!"
				# 	state = 1
				# elif currPath == 2 and currVal[currPath - 1] >= 0.53 and currVal[currPath - 1] <= 0.58 :
				# 	print "path 2 detected"
				# 	maxCurrPath = currPath
				# 	state = 1
				# # elif currPath == 3 and currVal[currPath - 1] <= 0.52 and currVal[currPath - 1] >= 0.48 :
				# # 	print "path 3 detected"
				# # 	maxCurrPath = currPath
				# # 	state = 1
				# elif currPath == 4 and currVal[currPath - 1] >= 0.42 and currVal[currPath - 1] <= 0.47 :
				# 	print "path 4 detected"
				# 	maxCurrPath = currPath
				# 	state = 1
				# elif currPath == 5 and currVal[currPath - 1] >= 0.5 and currVal[currPath - 1] <= 0.56 :
				# 	print "path 5 detected"
				# 	maxCurrPath = currPath
				# 	state = 1
				# elif currPath == 6 and currVal[currPath - 1] >= 0.45 and currVal[currPath - 1] <= 0.51 :
				# 	print "path 6 detected"
				# 	maxCurrPath = currPath
				# 	state = 1
				# elif currPath == 7 and currVal[currPath - 1] >= 0.51 and currVal[currPath - 1] <= 0.56 :
				# 	print "path 7 detected"
				# 	maxCurrPath = currPath
				# 	state = 1
				if currPath == 12 :
					i = i + 1

			elif state == 1 :
				uart.write('x')
				sleep(1)
				# chr(ord('a') + 5)
				uart.write(chr(ASCII_A + maxCurrPath))
				print ("enter x state, max path : ", maxCurrPath) #test
				state = 2


			# pause state
			elif state == 2 :
				# print ("old current : ", currVal[currPath - 1], "new current : ", txCurrent) #test
				# if currVal[currPath - 1] > txCurrent + 0.2 :
				# 	print "value got changed!"
				# 	state = 3
				print "max path : ", maxCurrPath, "txCurrent : ", txCurrent #test
				if maxCurrPath == 1 and txCurrent <= 0.45 :
				 	print "path 1 detected!"
					state = 3
				elif maxCurrPath == 2 and txCurrent <= 0.43 :
					print "path 2 detected"
					state = 3
				elif maxCurrPath == 3 and txCurrent >= 0.57 :
					print "path 3 detected"
					state = 3
				elif maxCurrPath == 4 and txCurrent <= 0.4 :
					print "path 4 detected"
					state = 3
				elif maxCurrPath == 5 and txCurrent <= 0.44 :
					print "path 5 detected"
					state = 3
				elif maxCurrPath == 6 and txCurrent <= 0.43 :
					print "path 6 detected"
					state = 3
				elif maxCurrPath == 7 and txCurrent <= 0.48 :
					print "path 7 detected"
					state = 3


			elif state == 3 :
				print "value got changed!"
				uart.write('y')
				sleep(2)
				print ("enter y state, current : ", txCurrent, "current path : ", currPath) #test
				state = 0


			# idx += 1
			# F.truncate()
			# F.close()

		# Now wait up to one minute to receive data from the device.

		# print('Waiting up to 60 seconds to receive data from the device...')
		# received = uart.read(timeout_sec=60)
		# if received is not None:
		#     # Received data, print it out.
		#     print('Received: {0}'.format(received))
		# else:
		#     # Timeout waiting for data, None is returned.
		#     print('Received no data!')
	finally:
		# book.save("path1-twopath.xls") # saving in excel
		# Make sure device is disconnected on exit.
		device.disconnect()


# Initialize the BLE system.  MUST be called before other BLE calls!
ble.initialize()

# Start the mainloop to process BLE events, and run the provided function in
# a background thread.  When the provided main function stops running, returns
# an integer status code, or throws an error the program will exit.
ble.run_mainloop_with(main)
