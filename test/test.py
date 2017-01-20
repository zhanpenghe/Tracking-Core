import socket
import sys


def connect():
	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	server_addr = ('localhost', 9998)
	sock.connect(server_addr)
	print 'connected'
	try:
		message = 'okokok'

		sock.sendall(message)

		while True:
			try:
				data = sock.recv(1024)
				print(data)
				sock.sendall(message)
			except Exception as e:
				print(e)
				continue

	except Exception as e1:
		print(e1)
		return

connect()