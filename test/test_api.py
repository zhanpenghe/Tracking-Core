'''
This is a test api server that can connect to the tracking-core server.

@Author Adam Ho
'''
import socket
import sys
import threading
from bottle import route, run, template

pos_dict = {} #dict to store the positions
pos_dict['test'] = 123

@route('/hello/<name>')
def index(name):
	return template('<b>Hello {{name}}</b>!', name=name)

@route('/pos/<name>')
def find_pos(name):
	if name in pos_dict:
		print 'okok'
		return ''+name+','+str(pos_dict[name])
	else:
		return ''


#Connection with core server..
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
				#print(data)
				infos = data.split('|')
				if infos[0] == 'okokok':
					#print 'no pos info'
					pos_dict['abc'] = 10
				sock.sendall(message)
			except Exception as e:
				print(e)
				continue

	except Exception as e1:
		print(e1)
		return

def start_api():
	run(host='localhost', port=9090)
	print 'API ESTABLISHED'


def run_core_con():
	api_thread = threading.Thread(target = start_api)
	api_thread.daemon = True
	api_thread.start()
	connect();

run_core_con()


