# Divide room to 9 grid

from __future__ import print_function
import numpy as np
import math

# Positioning function
def calc_position(pos_list,agentPos_dic,n,A,center_pos,radius):
	temp_list=[]


	ratio_ab = calc_ratio(pos_list[0][1],pos_list[1][1],n,A)
	ratio_ac = calc_ratio(pos_list[0][1],pos_list[2][1],n,A)
	ratio_bc = calc_ratio(pos_list[1][1],pos_list[2][1],n,A)

	line_ab = calc_orth(agentPos_dic[pos_list[0][0]],agentPos_dic[pos_list[1][0]],ratio_ab)
	line_ac = calc_orth(agentPos_dic[pos_list[0][0]],agentPos_dic[pos_list[2][0]],ratio_ac)
	line_bc = calc_orth(agentPos_dic[pos_list[1][0]],agentPos_dic[pos_list[2][0]],ratio_bc)

	temp_list.append(solve_matrix(line_ab,line_ac))
	temp_list.append(solve_matrix(line_ab,line_bc))
	temp_list.append(solve_matrix(line_ac,line_bc))

	position_temp = [0,0]
	position_temp[0] = (temp_list[0][0][0]+temp_list[1][0][0]+temp_list[2][0][0])/3
	position_temp[1] = (temp_list[0][1][0]+temp_list[1][1][0]+temp_list[2][1][0])/3

	#print(temp_list[0][0],temp_list[1][0])
	return [int(position_temp[0]),int(position_temp[1])]

def solve_matrix(a,b):
	matrix_a = np.array([a[1],b[1]])
	matrix_b = np.array([[a[0]],[b[0]]])
	return np.linalg.inv(matrix_a).dot(matrix_b).tolist()

# Calculate distance ratio
def calc_ratio(a,b,n,A):
	# Ratio
	ratio = math.pow(10,(b-a)/(10.0*n))
	return ratio

# Calculate Orthogonal: Return [c,[a,b]] for ax+by=c
def calc_orth(start,end,ratio):
    r_start = math.pow(start[0],2)+math.pow(start[1],2)
    r_end = math.pow(end[0],2)+math.pow(end[1],2)

    a = (end[0]-start[0])
    b = (end[1]-start[1])

    if r_start == r_end:
        return [0,[a,b]]
    else:
        mid = np.divide(np.array([start[0]+ratio*end[0],start[1]+ratio*end[1]]),ratio+1)
        mid_scalar = mid[0]*a+mid[1]*b
        if mid_scalar == 0:
            return [0,[a,b]]
        else:
            return [1,[a/float(mid_scalar),b/float(mid_scalar)]]


if __name__ == "__main__":
	# Execute only if run as a script
	print("This a sub-module")
