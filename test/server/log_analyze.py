def load_log_file():
	lst = {}
	counts = {}
	with open('./parameter/list_agent.txt','r') as list_agent:
		for line in list_agent:
			if "#" in line:
				continue
			infos = line.split('|')
			lst[infos[1]] = infos[0]
			counts[infos[0]] = 0

	with open('./data/log_demo.txt', 'r') as log:
		for line in log:
			try:
				mac = line.split('|')[0]
				if mac in lst:
					agent = lst.get(mac)
					counts[agent] = counts.get(agent)+1
				elif mac not in counts:
					counts[mac] = 1
				else:
					counts[mac]+=1
			except Exception as e:
				print(e)
				continue
	return counts

def analyze():
	counts = load_log_file()
	for ct in sorted(counts):
		print("%s:   %s"%(ct, counts[ct]))

analyze()
