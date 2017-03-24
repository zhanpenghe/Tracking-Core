from __future__ import print_function
import json
i=0
while 1:
	try:
		with open("1.json") as jsonFile:
			temp_json = json.load(jsonFile)
			#print(temp_json["tag"])
			if temp_jason["tag"]!=temp_json_part or i==0:
				for item in temp_json["tag"]:
					print(item["reader"],item["loc"], end=" ")
				print("\n")
				i=1;
			temp_json_part = temp_json["tag"]
	except KeyboardInterrupt:
		raise
	except:
		pass

