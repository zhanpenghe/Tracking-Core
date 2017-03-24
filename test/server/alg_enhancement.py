import math
r=60
def add_enhancement(pos_r,r_num):
    pos = list(pos_r)
    if r_num == 1:
        if pos[0]<=370 and pos[1]<220:
            pos[0]=200
            pos[1]=160

        elif pos[0]<=370 and pos[1]>=220:
            pos[0]=200
            pos[1]=260

        elif pos[0]>350:
            pos[0]=450
            pos[1]=229

        else:
        	pos[0]=247
        	pos[1]=184

    if r_num == 2:
        if pos[0]<=900:
            pos[0]=800
            pos[1]=200
        else:
            pos[0]=1050
            pos[1]=200

    if r_num == 3:
        if pos[0]<=1100:
            pos[0]=950
            pos[1]=430
        else:
            pos[0]=1253
            pos[1]=430

    if r_num == 4:
        if pos[0]<=1350:
            pos[0]=1250
            pos[1]=650
        elif pos[0]>1359 and pos[1]<= 620:
            pos[0]=1500
            pos[1]=490
        else:
            pos[0]=1500
            pos[1]=730

    if r_num == 5:
        if pos[0]<=1950 and pos[1]<= 630:
            pos[0]=1760
            pos[1]=490
        elif pos[0]>1950 and pos[1]<= 620:
            pos[0]=2000
            pos[1]=490
        elif pos[0]<=1950 and pos[1]> 630:
            pos[0]=1760
            pos[1]=730
        else:
            pos[0]=2000
            pos[1]=730
    return pos

def calculate_distance(x,y,a,b):
    m=x-a
    n=y-b
    distance = math.sqrt(math.pow(m,2)+math.pow(n,2))
    return distance
