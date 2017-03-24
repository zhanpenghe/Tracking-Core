

class Filter:

	def __init__(self):
		self.Q = 1.0
		self.R = 1.0
		self.A = 1.0
		self.B = 1.0
		self.H = 1.0
		self.x = 0.0
		self.cov = 0.0
		self.isInit = True

	def set_noise(self, Q, R):
		self.Q = Q
		self.R = R

	def filter(self,z):
		u = 1.0
		if self.isInit:
			self.x = (1/self.H)*z
			self.cov = (1/self.H)*self.R*(1/self.H);
			self.isInit = False
		else:
			predX = (self.A*self.x)+(self.B*u)
			predCov = (self.A*self.cov)*self.A+self.Q

			K = predCov*self.H*(1/((self.H*predCov*self.H)+self.R))
		
			self.x = predX+K*(z-(self.H*predX))
			self.cov = predCov-(K*self.H*predCov)

		return self.x

	def __str__(self):
		return '(Q: %f, R: %f)'%(self.Q, self.R)

def process_file(filename, resultfilename):

	filters = {}

	agent_mac = ''
	beacon_mac = ''
	with open(resultfilename,"w") as resultFile:
		with open(filename) as file:
			for line in file:
				try:
					line = line.rstrip('\n')
					infos = line.split('|')
					if int(infos[1]) > 0: 
						#header
						agent_mac = infos[0]
						resultFile.write(line+'\n')

						if agent_mac not in filters:
							filters[agent_mac] = {}

					else:
						beacon_mac = infos[0]
						if agent_mac not in filters:
							filters[agent_mac] = {}	
						if beacon_mac not in filters[agent_mac]:
							temp = Filter()
							temp.set_noise(0.03, 0.3)
							filters[agent_mac][beacon_mac] = temp
						#apply kalman filter, resemble the string and log
						rssi_kf = filters[agent_mac][beacon_mac].filter(int(infos[1]))
						result = '%s|%f\n'%(line, rssi_kf)
						resultFile.write(result)

				except Exception as e:
					print(e)
					print 'empty line'
					continue


def main():

	process_file('A.txt', "A_kf.txt")
	return

if __name__ == "__main__":
	main()