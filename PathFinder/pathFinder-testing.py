# Example of interaction with a BLE UART device using a UART service
# implementation.
# Author: Tony DiCola
import Adafruit_BluefruitLE
from Adafruit_BluefruitLE.services import UART
from time import sleep
import json, socket

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

		# Write a string to the TX characteristic.
		# uart.write('3')
		# print("Sent '3' to the device (high speed).")
		#
		# print("delay for 5 second...")
		# sleep(5)   # Delay for 1 minute (60 seconds).
		#
		# uart.write('2')
		# print("Sent '2' to the device (low speed).")
		#
		# print("delay for 5 second...")
		# sleep(5)   # Delay for 1 minute (60 seconds).
		#
		# uart.write('1')
		# print("Sent '1' to the device (pause).")
		# print("delay for 5 second...")
		# sleep(5)
		# print("done")

		print('PC should decide speed mode first.')
		print('you can type 'r' whenever you want to reset.')
		print("    type '1': regular mode")
		print("    type '2': slow mode")
		print("    type '3': fast mode")
		speed = raw_input('-> ')
		print('you type ',speed)
		uart.write(speed)
		sleep(1)

		 # send "y" when have not yet found the best path
		uart.write('y')

		# # # open the file
		# # try:
		# # 	F = open("/Users/cloelee/Documents/Capstone-LVAD/PathFinder/dummyFile.txt", "r+")
		# # except IOError:
		# # 	print "Could not read file:"
		# # 	sys.exit()
		# #
		# # fl = fcntl.fcntl(sys.stdin.fileno(), fcntl.F_GETFL)
		# # fcntl.fcntl(sys.stdin.fileno(), fcntl.F_SETFL, fl | os.O_NONBLOCK)
		# #
		# # lines = F.readlines()
		# idx = 0

		firstSweep = 0 # flag after the first sweep
		pathChange = 0 # flag when found the best path
		initial_value = [0] * 12
		reset = 0 # flag after the first

		while True:
			current_path = uart.read(timeout_sec = 60)
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
			print("Waiting for packet...")
			data, addr = serverSocket.recvfrom(2048)
			print("Packet received!")
			txInfoJSON = json.loads(data.decode("utf-8"))
			txPowerLevel = txInfoJSON["txPowerLevel"]
			txVoltage = txInfoJSON["txVoltage"]
			txCurrent = txInfoJSON["txCurrent"]
			txPower = txInfoJSON["txPower"]

			# receivedVoltage = int(lines[idx])

			print(" current_path : ", current_path, ord(current_path) - ASCII_A)

			# First time running the program to get the initial values
			if ((ord(current_path) <= (ASCII_A + 12)) & (firstSweep == 0)):
				print("enter first loop")
				initial_value[ord(current_path) - 1 - ASCII_A] = txCurrent

				if (ord(current_path) == (ASCII_A + 12)):
					firstSweep = 1
					reset = 1
					print(initial_value)


			if (firstSweep == 1 and reset == 0):
				if (txCurrent > initial_value[ord(current_path) - 1 - ASCII_A] + 0.3):
					#(receivedVoltage > initial_value[ord(current_path) - 1 - ASCII_A] - 2)):
					#initial_value[current_path - 1 - 'a'] = receiveVoltage
					if (pathChange != 1):
						pathChange = 1;
						uart.write('x')
						print("enter x step, receive V : ", txCurrent, "current path : ", ord(current_path) - ASCII_A)
						#sleep(5)
						uart.write('a' + current_path)
						sleep(5)

				elif (pathChange == 1):
					print("enter y step, receive V : ", txCurrent, "current path : ", ord(current_path) - ASCII_A)
					uart.write('y')
					pathChange = 0
					firstSweep = 0
			else:
				reset = 0

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
		# Make sure device is disconnected on exit.
		device.disconnect()


# Initialize the BLE system.  MUST be called before other BLE calls!
ble.initialize()

# Start the mainloop to process BLE events, and run the provided function in
# a background thread.  When the provided main function stops running, returns
# an integer status code, or throws an error the program will exit.
ble.run_mainloop_with(main)
