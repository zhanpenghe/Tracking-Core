#Usage for automated ssh/scp

from pexpect import pxssh
import pexpect
import getpass
import sys
import thread

def scp(path, username, hostname, password, targetpath):
	try:
		cmd = 'scp -r '+path+' '+username+'@'+hostname+':'+targetpath;
		child = pexpect.spawn(cmd)

		i = child.expect(["Password:", pexpect.EOF])

		if i==0:
			child.sendline(password)
			child.expect(pexpect.EOF)
		else:
			print 'errrrror'
	except Exception as e:
		print(e)

def ssh(hostname,username,password,commands):
	try:
		s = pxssh.pxssh()
		s.force_password = True
		s.login(hostname, username, password,auto_prompt_reset = False)
		print '[INFO] Login %s successfully, now run commands.'%hostname
		
		for cmd in commands:
			s.sendline(cmd)
			#print(s.before)
		s.logout()
		print '[INFO] Finish all the jobs and logged out'
	except pxssh.ExceptionPxssh as e:
		print("pxssh failed on login %s."%hostname)
		print(e)


def compile():
	print 'Starting to connect agents'
	lst = {}
	with open('../parameter/list_agent.txt','r') as list_agent:
		for line in list_agent:
			if "#" in line:
				continue
			infos = line.split('|')
			lst[infos[0]] = infos[1]
	print(lst)

	for agent in lst:
		host = 'ioteye-%s'%(agent)
		scp('./btagent','ioteye',host,'ioteye1234','~')

	cmds = ['cd ~/btagent','make agent','rm *.c','rm *.h','rm Makefile']
	for agent in lst:
		host = 'ioteye-%s'%(agent)
		try:
			ssh(host,'ioteye','ioteye1234',cmds)
		except Exception as e:
			print 'Failed to compile at %s'%(agent)
			print(e)
			continue
compile()

