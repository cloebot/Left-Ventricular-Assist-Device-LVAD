#!/usr/bin/env python3
# coding=utf-8

import asyncio
import websockets
import packettools
import sys, select, os
import transmitterGUIClass
import time

# define variables
STATUS = 0
INITIALIZE_STATUS = 1
STAGE = 2
POWER_LEVEL = 60000

class NetApi:
	def __init__(self, websocket_url):
		self.websocket_url = websocket_url
		pass

	async def connect(self):
		async with websockets.connect(self.websocket_url, subprotocols=['wibotic']) as websocket:
			consumer_task = asyncio.ensure_future(self.consumer_handler(websocket))
			producer_task = asyncio.ensure_future(self.producer_handler(websocket))
			done, pending = await asyncio.wait(
				[consumer_task, producer_task],
				return_when=asyncio.FIRST_COMPLETED,
			)
			for task in pending:
				task.cancel()

	async def consumer_handler(self, websocket):
		while True:
			message = await websocket.recv()
			await self.consumer(message)

	async def producer_handler(self, websocket):
		# set to the current command that is entered
		status = "off"
		# keep track whether initialization is completed or not
		initializeStatus = 0
		# keep track of the stage of the current status
		stage = 0
		# pass into produce() function to determine what set of commands will be executed
		statusWrapper = [status, initializeStatus, stage]

		while True:
			# Listen to user input to control the transmitter
			if sys.stdin in select.select([sys.stdin], [], [], 0)[0]:
				userInput = input()
				if userInput != "":
					if userInput.lower() == "off" or userInput.lower() == "on" or userInput.lower() == "quit":
						statusWrapper[STATUS] = userInput.lower()
						statusWrapper[STAGE] = 0

			# Send command to the transmitter
			if statusWrapper[STAGE] == "quit":
				print("Quiting...")
				break
			elif statusWrapper[STAGE] != -1:
				message = await self.producer(statusWrapper)
				await websocket.send(message)
			else:
				statusWrapper[STATUS] = "empty"
				message = await self.producer(statusWrapper)
				await websocket.send(message)

	async def consumer(self, data):
		# x = packettools.process_data(data)
		# if type(x) == packettools.ADCUpdate:
		# 	if (x.device == packettools.DeviceID.RX_1):
		# 		print(x.values[packettools.AdcID.VMonBatt])
		# else:
		# 	pass
		print(packettools.process_data(data))

	async def producer(self, statusWrapper):
		await asyncio.sleep(1)

		'''--------------------------------------------------------------------
				 _       _ _   _       _ _          _   _
				(_)     (_) | (_)     | (_)        | | (_)
				 _ _ __  _| |_ _  __ _| |_ ______ _| |_ _  ___  _ __
				| | '_ \| | __| |/ _` | | |_  / _` | __| |/ _ \| '_ \
				| | | | | | |_| | (_| | | |/ / (_| | |_| | (_) | | | |
				|_|_| |_|_|\__|_|\__,_|_|_/___\__,_|\__|_|\___/|_| |_|

		--------------------------------------------------------------------'''
		# Initializing with highest access level in manual mode
		if statusWrapper[INITIALIZE_STATUS] == 0:
			print("Initializing...")
			print("Change access level to 15")
			request = packettools.ParamWriteRequest(
				packettools.DeviceID.TX,
				packettools.ParamID.AccessLevel,
				15 #max level
			);
			statusWrapper[INITIALIZE_STATUS] = 1
		elif statusWrapper[INITIALIZE_STATUS] == 1:
			print("Change to Manual Mode...")
			request = packettools.ParamWriteRequest(
				packettools.DeviceID.TX,
				packettools.ParamID.ManualMode,
				True
			);
			statusWrapper[INITIALIZE_STATUS] = 2
			statusWrapper[STAGE] = -1
		else:
			if statusWrapper[STATUS] == "empty":
				request = packettools.ParamWriteRequest(
						packettools.DeviceID.TX,
						packettools.ParamID.ChargeEnable,
						False
					);

				'''----------------------------------------------------------------
								_
								| |
								| |_ _   _ _ __ _ __     ___  _ __
								| __| | | | '__| '_ \   / _ \| '_ \
								| |_| |_| | |  | | | | | (_) | | | |
								\__|\__,_|_|  |_| |_|  \___/|_| |_|

				----------------------------------------------------------------'''

			# Send commands to the transmitter
			elif statusWrapper[STATUS] == "on":
				if statusWrapper[STAGE] == 0:
					print("Turning on fan 1...")
					request = packettools.ParamWriteRequest(
						packettools.DeviceID.TX,
						packettools.ParamID.TxFan1Enable,
						255
					);
					statusWrapper[STAGE] = 1
				elif statusWrapper[STAGE] == 1:
					print("Turning on fan 2...")
					request = packettools.ParamWriteRequest(
						packettools.DeviceID.TX,
						packettools.ParamID.TxFan2Enable,
						255
					);
					statusWrapper[STAGE] = 2
				elif statusWrapper[STAGE] == 2:
					print("Turning on transmitter...")
					request = packettools.ParamWriteRequest(
						packettools.DeviceID.TX,
						packettools.ParamID.TxPowerEnable,
						1
					);
					statusWrapper[STAGE] = 3
				elif statusWrapper[STAGE] == 3:
					print("Set power level to %d..." %(POWER_LEVEL))
					request = packettools.ParamWriteRequest(
						packettools.DeviceID.TX,
						packettools.ParamID.TxPowerLevel,
						POWER_LEVEL
					);
					statusWrapper[STAGE] = 4
				elif statusWrapper[STAGE] == 4:
					print("Transmitter ON")
					request = packettools.ParamWriteRequest(
						packettools.DeviceID.TX,
						packettools.ParamID.ChargeEnable,
						True
					);
					statusWrapper[STAGE] = -1

				'''------------------------------------------------------------
							 _                             __  __
							| |                           / _|/ _|
							| |_ _   _ _ __ _ __     ___ | |_| |_
							| __| | | | '__| '_ \   / _ \|  _|  _|
							| |_| |_| | |  | | | | | (_) | | | |
							 \__|\__,_|_|  |_| |_|  \___/|_| |_|


				------------------------------------------------------------'''

			elif statusWrapper[STATUS] == "off" or "quit":
				if statusWrapper[STAGE] == 0:
					print("Turning off the transmitter...")
					request = packettools.ParamWriteRequest(
						packettools.DeviceID.TX,
						packettools.ParamID.TxPowerEnable,
						0
					);
					statusWrapper[STAGE] = 1
				elif statusWrapper[STAGE] == 1:
					print("Turning off fan 1...")
					request = packettools.ParamWriteRequest(
						packettools.DeviceID.TX,
						packettools.ParamID.TxFan1Enable,
						0
					);
					statusWrapper[STAGE] = 2
				elif statusWrapper[STAGE] == 2:
					print("Turning off fan 2...")
					request = packettools.ParamWriteRequest(
						packettools.DeviceID.TX,
						packettools.ParamID.TxFan2Enable,
						0
					);
					statusWrapper[STAGE] = 3
				elif statusWrapper[STAGE] == 3:
					print("Transmitter OFF")
					if statusWrapper[STATUS] == "off":
						statusWrapper[STAGE] = -1
					else:
						statusWrapper[STAGE] = "quit"

					request = packettools.ParamWriteRequest(
						packettools.DeviceID.TX,
						packettools.ParamID.ChargeEnable,
						False
					);

		# Return the request as bytes to get sent out
		return bytes(request.as_packet())

if __name__ == "__main__":
	WIBOTIC_CHARGER_WS_URL = "ws://192.168.2.20/ws"
	test = NetApi(WIBOTIC_CHARGER_WS_URL)
	asyncio.get_event_loop().run_until_complete(
		test.connect()
	)
	print("Disconnected")
