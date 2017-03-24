# Least Squares
from __future__ import print_function

# Calculate porsition
def calc_position(pos_list,agentPos_dic,n,A,scale=400,if_int=1):
	d = []
	#print(pos_list[0][1],pos_list[1][1],pos_list[2][1])
	for idx,pos in enumerate(pos_list):
		d.append(calc_distance(int(pos[1]),n,A,scale))
	#Sprint(d)
	pos_loc = [agentPos_dic[x[0]] for x in pos_list]

	[x,y] = least_square(pos_loc,d)
	return [int(x),int(y)]

# Rssi to Distance
def calc_distance(rssi,n,A,scale):
	d = 10**((rssi-A)/(-10.0*n))
	return int(d*scale)

# Least square
def least_square(p,d):
	a = 2*(p[1][0]-p[0][0])
	b = 2*(p[1][1]-p[0][1])
	c = d[0]**2 - d[1]**2 - p[0][0]**2-p[0][1]**2+p[1][1]**2+p[1][0]**2
	e = 2*(p[2][0]-p[0][0])
	f = 2*(p[2][1]-p[0][1])
	g = d[0]**2 - d[2]**2 - p[0][0]**2-p[0][1]**2+p[2][1]**2+p[2][0]**2
	x=(g*b-f*c)/(e*b-f*a)
	y=(a*g-e*c)/(a*f-e*b)
	return [x,y]
