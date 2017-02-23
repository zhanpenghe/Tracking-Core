import socket
import sys
import numpy as np

def start_server():
	try:
		sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		server_addr = ('localhost', 9999)
		print('[INFO] Starting up on %s port %s' %server_addr)
		sock.bind(server_addr)
		sock.listen(1)
		print '[INFO] Waiting for an incoming connection'

		connection, client_addr = sock.accept()
		return (connection, client_addr)
	except Exception as e:
		print(e)
		return

#
class Filter:

	def __init__(self):
		#initial state(PV Mode)
		self.X = np.matrix([0,0,0,0])
		#initial uncertainty
		self.P = np.random.rand(4,4)
		#4D->2D
		self.H = np.matrix([
			[1, 0, 0, 0],
			[0, 1, 0, 0]
			])


x = Filter()