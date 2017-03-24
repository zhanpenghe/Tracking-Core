#Usage for automated ssh/scp
import paramiko
import sys
import thread
import time

def try_ssh(hostname, user, passwd, cmds, counter):
	try:
		ssh = paramiko.SSHClient()
		ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
		ssh.connect(hostname, username = user, password = passwd)
	except Exception as e:
		print('[ERROR]Failed to log in to host: %s'%(hostname))
		print(e)
		if counter > 0:
			print('[INFO]Attempt #%d'%(counter))
			time.sleep(3)
			try_ssh(hostname, user, passwd, cmds, counter-1)
			return

	print('[INFO]Succeed to log in to host: %s, now run commands.' % (hostname))
	stdin, stdout, stderr = ssh.exec_command('/bin/sh')
	for cmd in cmds:
		try:
			print(cmd)
			stdin.write(cmd+'\n')
		except Exception as e:
			print('[ERROR]Failed to run cmd: %s'%(cmd))
			print(e)
	
	print '[INFO]All jobs are done. Now log out.'
	if ssh:
		ssh.close()
		print '[INFO]Logged out successfully'
		return
	print('[ERROR]Failed to logout, ssh client is None.')

def ssh(hostname, user, passwd, cmds):	#ssh with only one attempt
	try_ssh(hostname, user, passwd, cmds, 0)


def rebootAllAgent():
	print 'Starting shut down agents'
	lst = {}
	with open('./parameter/list_agent.txt','r') as list_agent:
		for line in list_agent:
			if "#" in line:
				continue
			infos = line.split('|')
			lst[infos[0]] = infos[1]
	print(lst)

	cmds = ['sudo killall btagent','sudo shutdown -r now']
	for agent in lst:
		host = 'ioteye-%s'%(agent)
		try:
			try_ssh(host,'ioteye','ioteye1234',cmds,3)
		except Exception as e:
			print 'Failed to shut down %s'%(agent)
			print(e)
			continue

def restartAllAgent(ip):
	print '[INFO]Starting All Agent Programs'
	lst = {}
	with open('./parameter/list_agent.txt','r') as list_agent:
		for line in list_agent:
			if "#" in line:
				continue
			infos = line.split('|')
			lst[infos[0]] = infos[1]
	print(lst)

	#cmds = ['cd ~/agent','sudo hciconfig hci0 down','sudo hciconfig hci0 up','sudo ./btagent -a 10.237.43.108 -w > log.txt &', 'exit'] #add  log on agent
	cmds = ['cd ~/agent','sudo hciconfig hci0 down','sudo hciconfig hci0 up','rm log.txt','sudo ./btagent -a '+ip+' -w >log.txt 2>&1 &']
	for agent in lst:
		host = 'ioteye-%s'%(agent)
		try:
			try_ssh(host,'ioteye','ioteye1234',cmds,3)
		except Exception as e:
			print 'Failed to start %s'%(agent)
			continue;
			print(e)

def upgrade(hostname, user, passwd, localpath, remotepath):
	try:
		ssh = paramiko.SSHClient()
		ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
		ssh.connect(hostname, username = user, password = passwd)
	except Exception as e:
		print('[ERROR]Failed to log in to host: %s'%(hostname))
		print(e)
		if counter > 0:
			print('[INFO]Attempt #%d'%(counter))
			time.sleep(3)
			try_ssh(hostname, user, passwd, cmds, counter-1)
			return

	print('[INFO]Succeed to log in to host: %s, now transfer files.' % (hostname))
	try:
		sftp = ssh.open_sftp()
		sftp.put(localpath, remotepath)
		sftp.close()
	except Exception as e:
		print(e)
