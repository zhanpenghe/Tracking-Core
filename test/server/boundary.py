

def add_boundary(pos_r,r_num):
    pos = list(pos_r)
    pos[0]=pos[0]+50
    if r_num == 1:
        if pos[0]<110:
            pos[0]=110
        if pos[0]>681:
            pos[0]=681

        if pos[1]<60:
            pos[1]=60

        if pos[0]<526:
            if pos[1]>373:
                pos[1]=373
        else:
            if pos[1]>328:
                pos[1]=328

    if r_num == 2:
        if pos[0]<723:
            pos[0]=723
        if pos[0]>1293:
            pos[0]=1293

        if pos[1]<60:
            pos[1]=60

        if pos[0]<859:
            if pos[1]>328:
                pos[1]=328
        else:
            if pos[1]>302:
                pos[1]=302

    if r_num == 3:
        if pos[0]<879:
            pos[0]=879
        if pos[0]>1450:
            pos[0]=1450

        if pos[1]>524:
            pos[1]=524

        if pos[0]<1303:
            if pos[1]<332:
                pos[1]=332
        else:
            if pos[1]<168:
                pos[1]=168


    if r_num == 4:
        if pos[0]<1167:
            pos[0]=1167
        if pos[0]>2175:
            pos[0]=2175

        if pos[1]<385:
            pos[1]=385

        if pos[1]>845:
            pos[1]=845
    pos[0]=pos[0]-50
    return pos

# Ture Boundary
'''
def add_boundary(pos_r,r_num):
    pos = list(pos_r)
    pos[0]=pos[0]+50
    if r_num == 1:
        if pos[0]<100:
            pos[0]=100
        if pos[0]>691:
            pos[0]=691

        if pos[1]<50:
            pos[1]=50

        if pos[0]<526:
            if pos[1]>383:
                pos[1]=383
        else:
            if pos[1]>338:
                pos[1]=338

    if r_num == 2:
        if pos[0]<713:
            pos[0]=713
        if pos[0]>1303:
            pos[0]=1303

        if pos[1]<50:
            pos[1]=50

        if pos[0]<859:
            if pos[1]>338:
                pos[1]=338
        else:
            if pos[1]>312:
                pos[1]=312

    if r_num == 3:
        if pos[0]<869:
            pos[0]=869
        if pos[0]>1460:
            pos[0]=1460

        if pos[1]>534:
            pos[1]=534

        if pos[0]<1303:
            if pos[1]<322:
                pos[1]=322
        else:
            if pos[1]<158:
                pos[1]=158


    if r_num == 4:
        if pos[0]<1157:
            pos[0]=1157
        if pos[0]>2185:
            pos[0]=2185

        if pos[1]<375:
            pos[1]=375

        if pos[1]>855:
            pos[1]=855
    pos[0]=pos[0]-50
    return pos
'''
