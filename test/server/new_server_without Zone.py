#!/usr/bin/python
#******************************************************************************#
### Read agent uploaded data
### Using MAX RSSI as input

## Libraries
from __future__ import print_function
import numpy as np                          # Math functions


try:
    import simplejson as json               # Json read and write
except ImportError:
    import json                             # Json read and write
import os.path                              # Directory management
import sys                                  # Compatibility
import time                                 # Timestamp
import math                                 # Operands
import threading                            # Multithread
import getopt                               # Read Options
from collections import deque               # FIFO queue
from bottle import auth_basic, route, run, error               # For API

#import ssh                   # for automatic agent implementation
import os
import time
import traceback
import agent_control				    #shutdown agents when closing the server

if sys.version_info.major == 2:             # Check Python version
    import ConfigParser                     # Read Configurations
else:
    import configparser as ConfigParser     # Read Configurations

import socket                               # Socket
import traceback                            # For debug
from operator import itemgetter             # improve efficiency

if sys.version_info.major == 2:             # Check Python version
    import SocketServer                     # Server Socket
    import Queue                            # Message sharing
else:
    import socketserver as SockerServer     # Server Socket
    import queue as Queue                   # Message sharing

# Local modules
#import configure as conf    # Configurations, parameters

# Import Algorithm
import alg.simple as simple
import boundary as bound
#import alg.ls as ls

#******************************************************************************#
# Global Variables
enable_local = 0
enable_log = 1
port_num = 9999
api_port_num = 9998
beacon_position = {}
beacon_position_kf = {}
agent_ip_to_mac = {}
agent_status = {}

#******************************************************************************#

## Data Classes
# Agent Measurement Class
class AgentMsClass:
    def __init__(self):
        self.value = None
        self.room_num = None
        self.v_list = deque(maxlen = MAL)

    # Use Average
    def add(self,rssi_value,room_num,MA,if_int = 1):
        self.v_list.append(int(rssi_value))

        if self.value == None:
            self.value = max(self.v_list)
        else:
            self.value = (self.value*(MA-1)+max(self.v_list))/MA
        self.room_num = room_num

# Beacon Class
class BeaconClass:
    # Constructor
    def __init__(self):
        self.dic = {}
        self.pos = None
        self.truepos =[0,0]
        self.cur_room = None
        self.pos_list = []

    # Return Position
    def position(self,a_pos):
        a=0
        pos_list = []
        cur_pos_list = []
        # Check measurement count
        room_list = [[],[],[],[],[],[]]
        new_room = None
        for item in sorted(self.dic,key=lambda item: self.dic[item].value, reverse = True):
            temp_room = self.dic[item].room_num
            room_list[temp_room].append([item,self.dic[item].value])

            if new_room == None:
                for room_item in range(6):
                    if len(room_list[room_item])>2:
                        new_room = room_item
                        pos_list = room_list[new_room]
                        break
                else:
                    continue
            else:
                if len(room_list[self.cur_room])>2:
                    cur_pos_list = room_list[self.cur_room]
                    # Calculate the RSSI measurement difference
                    room_diff = sum([x[1] for x in pos_list]) - sum([y[1] for y in cur_pos_list])
                    if room_diff < DMR:
                        pos_list = cur_pos_list
                    else:
                        self.cur_room  = new_room
                    break
            if self.cur_room == None:
                self.cur_room = new_room
            if new_room == self.cur_room:
                break
        else:
            a = 1

        if a==0:
	    print (pos_list)
            current_pos = simple.calc_position(pos_list,a_pos,PN,PA,0,0)
            current_pos = self.adjust_pos(current_pos,*r_param[self.cur_room])

            #self.pos = [int(x) for x in current_pos]
            current_pos = self.apply_bound(current_pos,self.cur_room)

            self.pos_list.append(current_pos)

            #self.truepos = self.pos_avg(self.truepos,current_pos)

            if len(self.pos_list) > PMA:
                self.truepos = self.pos_avg()
                self.pos = self.truepos
            else:
                self.pos = None

            #self.turepos = self.pos
            #self.pos = [self.pos[0],self.pos[1]+300]
            # return [(self.pos[0]-350)*2.5,self.pos[1]]
            # return self.pos

    # Adjust position by (x_n,y_n) = [(x-a)*b,(y-c)*d]
    def adjust_pos(self, pos, a, b, c, d):
        #return [pos[0]*a+b,pos[1]*c+d]     #southbrunswick
	return [(pos[0]-a)*b,(pos[1]-c)*d]   #piscatway

    # Need future modification
    def apply_bound(self,pos_in,room_in):
        c_pos = bound.add_boundary(pos_in, room_in)
        return [int(c_pos[0]),int(c_pos[1])]

    # Calculate Average
    def pos_avg(self):
        temp_list = np.array(self.pos_list)
        self.pos_list = []
        avg_pos = temp_list.mean(0)
        return [int(avg_pos[0]),int(avg_pos[1])]
    '''
    def pos_avg(self, pos, current):
        x = (pos[0]*(conf.PMA-1)+current[0])/conf.PMA
        y = (pos[1]*(conf.PMA-1)+current[1])/conf.PMA
        return [int(x),int(y)]
    '''

    # Add new incoming message from agents
    def add_msg(self,msg_list,a_pos,MA):
        #print(msg_list)
        if msg_list[0] not in self.dic:
            self.dic[msg_list[0]]=AgentMsClass()

        self.dic[msg_list[0]].add(int(msg_list[2]),a_room[msg_list[0]],MA)
        self.position(a_pos)
## End of Data Classes
#******************************************************************************#

# Beacon Class
class BeaconKFClass:
    # Constructor
    def __init__(self):
        self.dic = {}
        self.pos = None
        self.truepos =[0,0]
        self.cur_room = None
        self.pos_list = []
        #kalman filter init
        self.Q = 1.0
        self.R = 1.0
        self.A = 1.0
        self.B = 1.0
        self.H = 1.0
        self.x = 0.0
        self.cov = 0.0
        self.isInit = True

    # Return Position
    def position(self,a_pos):
        a=0
        pos_list = []
        cur_pos_list = []
        # Check measurement count
        room_list = [[],[],[],[],[],[]]
        new_room = None
        for item in sorted(self.dic,key=lambda item: self.dic[item].value, reverse = True):
            temp_room = self.dic[item].room_num
            room_list[temp_room].append([item,self.dic[item].value])

            if new_room == None:
                for room_item in range(6):
                    if len(room_list[room_item])>2:
                        new_room = room_item
                        pos_list = room_list[new_room]
                        break
                else:
                    continue
            else:
                if len(room_list[self.cur_room])>2:
                    cur_pos_list = room_list[self.cur_room]
                    # Calculate the RSSI measurement difference
                    room_diff = sum([x[1] for x in pos_list]) - sum([y[1] for y in cur_pos_list])
                    if room_diff < DMR:
                        pos_list = cur_pos_list
                    else:
                        self.cur_room  = new_room
                    break
            if self.cur_room == None:
                self.cur_room = new_room
            if new_room == self.cur_room:
                break
        else:
            a = 1

        if a==0:
            current_pos = simple.calc_position(pos_list,a_pos,PN,PA,0,0)
            current_pos = self.adjust_pos(current_pos,*r_param[self.cur_room])

            #self.pos = [int(x) for x in current_pos]
            current_pos = self.apply_bound(current_pos,self.cur_room)

            self.pos_list.append(current_pos)

            #self.truepos = self.pos_avg(self.truepos,current_pos)

            if len(self.pos_list) > PMA:
                self.truepos = self.pos_avg()
                self.pos = self.truepos
            else:
                self.pos = None

            #self.turepos = self.pos
            #self.pos = [self.pos[0],self.pos[1]+300]
            # return [(self.pos[0]-350)*2.5,self.pos[1]]
            # return self.pos

    def filter(self, z):
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

    # Adjust position by (x_n,y_n) = [(x-a)*b,(y-c)*d]
    def adjust_pos(self, pos, a, b, c, d):
        #return [pos[0]*a+b,pos[1]*c+d]     #southbrunswick
        return [(pos[0]-a)*b,(pos[1]-c)*d]   #piscatway

    # Need future modification
    def apply_bound(self,pos_in,room_in):
        c_pos = bound.add_boundary(pos_in, room_in)
        return [int(c_pos[0]),int(c_pos[1])]

    # Calculate Average
    def pos_avg(self):
        temp_list = np.array(self.pos_list)
        self.pos_list = []
        avg_pos = temp_list.mean(0)
        return [int(avg_pos[0]),int(avg_pos[1])]
    '''
    def pos_avg(self, pos, current):
        x = (pos[0]*(conf.PMA-1)+current[0])/conf.PMA
        y = (pos[1]*(conf.PMA-1)+current[1])/conf.PMA
        return [int(x),int(y)]
    '''

    # Add new incoming message from agents
    def add_msg(self,msg_list,a_pos,MA):
        #print(msg_list)
        if msg_list[0] not in self.dic:
            self.dic[msg_list[0]]=AgentMsClass()

        rssi = int(msg_list[2])
        rssi = self.filter(rssi)
        self.dic[msg_list[0]].add(rssi,a_room[msg_list[0]],MA)
        self.position(a_pos)
## End of Data Classes
#******************************************************************************#



## Functions
# Update Json
def update_map(beaconNum,pos):
    with open("GUI/1.json") as dataFile:
        data = json.load(dataFile)

    data['tag'][int(beaconNum[-1])-1]['loc']=[pos[0],pos[1]]

    with open("GUI/1.json","w") as dataFile:
        json.dump(data, dataFile)

def usage():
    print("IoT Eye inc. Host server program.")
    print("Usage:")
    print("\tstart_server.py -l -p [Port]")
    print("Options:\n"
          "\t-r                Read Local Files\n"
          "\t-p [Port_Num]     Host server port. [Default 9999].\n"
          "\t-q [Port_Num]     API port. [Default 9998].\n"
          "\t-m                Enable local map update.\n"
          "\t-l                Enable log\n"
          "\t-d                Demo mode.\n"
          "\t-s                Packet size. [Default 10]\n"
          "\t-h                Help.\n")

## End of Functions
#******************************************************************************#
# Main Function
def main_server():
    # Allocate memory
    message_buffer = Queue.Queue()      # Buffer for sharing message
    dc_flag = 0
    current_time = "%02d:%02d:%02d"%(time.localtime().tm_hour,time.localtime().tm_min,time.localtime().tm_sec)
    # TCP request handler
    class ThreadedTCPRequestHandler(SocketServer.BaseRequestHandler):
        def handle(self):
            #self.request.settimeout(1)
            print("INFO: New connection in. From {}".format(self.client_address[0]))
            while 1:
                if dc_flag:
                    break
                data = self.request.recv(1024)
                if not data:
                    break

                MAC_addr=data[0:17]
                agent_ip_to_mac[self.client_address[0]] = MAC_addr
                print(self.client_address[0])
                agent_status[MAC_addr] = True
                message_buffer.put(data)
                self.request.sendall('z')
            print("INFO: Connection finished. From {}".format(self.client_address[0]))
            agent_ip_to_mac.pop(self.client_address[0],None)
            agent_status[MAC_addr] = False

    # TCP server class
    class ThreadedTCPServer(SocketServer.ThreadingMixIn, SocketServer.TCPServer):
        pass

    # Allocate connection memory
    agent_list = []
    agentInfo_list = []

    # Main Program
    with open("data/log_result.txt","w") as resultLogFile:
        with open("data/log_demo.txt","w") as demoLogFile:
            b_pos={}    # Beacon Positions
            b_pos_kf = {}
            print(HT,PT)
            server = ThreadedTCPServer((HT,PT), ThreadedTCPRequestHandler)

            server_thread = threading.Thread(target = server.serve_forever)
            server_thread.deamon = True

            # Convert parameters to local variables
            LMA = MA

            try:
                server_thread.start()
                while 1:
                    temp_data = message_buffer.get(0.1)     # Get Queue item with 0.1s time out
                    if enable_log:
                        demoLogFile.write(temp_data.strip()+"\n")

                    msg_info_prelist = temp_data.strip().split("\n")
                    currentAgentMac = msg_info_prelist[0].split("|")[0]
                    msg_info = [[currentAgentMac]+x.split("|") for x in msg_info_prelist[1:]]

                    for agentList in msg_info:
                        b_mac = agentList[1]
                        if b_mac in b_list:
                            try:
                                b_pos[b_mac].add_msg(agentList,a_pos,LMA)
                                b_pos_kf[b_mac].add_msg(agentList, a_pos, LMA)
                            except KeyError:
                                b_pos[b_mac]=BeaconClass()
                                b_pos_kf[b_mac]=BeaconKFClass()
                                b_pos[b_mac].add_msg(agentList,a_pos,LMA)
                                b_pos_kf[b_mac].add_msg(agentList, a_pos, LMA)

                            log_pos = b_pos[b_mac].pos
                            log_pos_kf = b_pos_kf[b_mac].pos
                            if log_pos != None:
                                cur_rn = b_pos[b_mac].cur_room
                                log_truepos = b_pos[b_mac].truepos
                                current_time = "%02d:%02d:%02d"%(time.localtime().tm_hour,
                                                                 time.localtime().tm_min,
                                                                 time.localtime().tm_sec)
                                #print("INFO: {} at ({},{}), in room {}, {}".format(b_list[b_mac]
                                #       ,log_truepos[0],log_truepos[1],cur_rn, current_time))
                                #update_map(b_list[b_mac],log_truepos)
                                beacon_position[b_list[b_mac]]=log_truepos#add

                            if log_pos_kf != None:
                                log_truepos_kf = b_pos_kf[b_mac].truepos
                                beacon_position_kf[b_list[b_mac]] = log_truepos_kf



				log_pos = [(log_pos[0]-50)/100.0,(log_pos[1]-50)/100.0]
                                if enable_log:
                                    resultLogFile.write("{}|{}|{}|{}|{}|{}\n".format(b_list[b_mac]
                                            ,log_pos[0], log_pos[1],log_truepos[0],log_truepos[1], current_time))
            except KeyboardInterrupt:
                dc_flag = 1
                agent_control.rebootAllAgent()
                server.shutdown()
                server.server_close()
                print("WARN: Program stopped")
            except Exception as e:
                dc_flag = 1
                server.shutdown()
                server.server_close()
                exc_type,exc_value,exc_traceback = sys.exc_info()
                for line in traceback.format_exception(exc_type,exc_value,exc_traceback):
                    print(line, end="")
                print(str(e))
                print("WARN: Program stopped")

def main_local(time_interval=0.0015):
    # Allocate connection memory
    agent_list = []
    agentInfo_list = []
    cur_time = "00:00:00"
    start_time = time.time()
    # Main Program
    with open("log_demo.txt") as inputLogFile:
        with open("data/local_result.txt","w") as resultLogFile:
            b_pos={}    # Beacon Positions
            LMA = MA

            for line in inputLogFile:
                time.sleep(time_interval)
                temp_data = line.strip("\n").split("|")
                # Line that have agent_info
                if len(temp_data) == 2:
                    cur_agent = temp_data[0]
                    continue

                # Line that have time infor
                if len(temp_data) == 1:
                    cur_time = temp_data[0][5:]
                    continue
                # line that contain sensor info
                if len(temp_data) == 3:
                    agentList = [cur_agent]+temp_data

                    b_mac = agentList[1]
                    if b_mac in b_list:
                        try:
                            b_pos[b_mac].add_msg(agentList,a_pos,LMA)
                        except KeyError:
                            b_pos[b_mac]=BeaconClass()
                            b_pos[b_mac].add_msg(agentList,a_pos,LMA)
                        log_pos = b_pos[b_mac].pos
                            #log_pos = [log_pos[0],log_pos[1]]
                        if log_pos != None:
                            cur_rn = b_pos[b_mac].cur_room
                            log_truepos = b_pos[b_mac].truepos
                            print("INFO: {} at ({},{}), true({},{}) at room {}, {}".format(b_list[b_mac]
                                    ,log_truepos[0],log_truepos[1],log_pos[0], log_pos[1], cur_rn, cur_time))
                            #update_map(b_list[b_mac],log_truepos)
                            beacon_position[b_list[b_mac]]=log_truepos
                            #log_pos = [(log_pos[0]-100)/100.0,log_pos[1]/100.0]
                            resultLogFile.write("{}|{}|{}|{}|{}|{}|{}\n".format(b_list[b_mac]
                                    ,log_truepos[0],log_truepos[1],log_pos[0], log_pos[1], cur_rn, cur_time))

    print("Program finished, spend %.2f s"%(time.time()-start_time))
# End of Main Functions
#******************************************************************************#
### API
### Authentication
def check_user(user,pw):
    if user == "ioteye" and pw == "ioteye1234":
        return True
    else:
        return False

### Error handler
@error(401)
def error_handler_unauth(error):
    return "Unauthorized"
    # Can also use abort(<ErrorNum>.<ErrorMsg>) in function

### API Method
@route('/agent/list-all-name')
def api_list_active_agents_name():
    return a_name

@route('/agent/list-all-room')
def api_list_active_agents_room():
    return a_room

@route('/agent/list-all-pos')
def api_list_active_agents_pos():
    return a_pos

@route('/agent/check_status_all')
def api_check_status_all():
    return agent_status

@route('/agent/restart_all_agent')
def api_restart_all_agent():
    try:
        agent_control.restartAllAgent(HT)
    except Exception as e:
        print(e)
        print('[ERROR] Failed to restart all agent')

@route('/agent/check_status/<agent_mac>')
def api_check_status(agent_ = None):
    try:
        result = agent_status[agent_mac]
        if result:
            return {"Agent_ID":a_name[agent_mac],"Agent_MAC":agent_mac,"Status":"On","Success":True}
        else:
            return {"Agent_ID":a_name[agent_mac],"Agent_MAC":agent_mac,"Status":"Off","Success":True}
    except KeyError:
        return {"Message":"MAC not Found","Success":False}
    return

@route('/agent/shutdown/<agent_name>')
def api_reboot(agent_name = None):
    try:
        cmds=['sudo shutdown -P now']
        agent_control.try_ssh('ioteye-'+agent_name, 'ioteye', 'ioteye1234',cmds,3)
    except:
        print ('Not successful shut down '+agent_name)
@route('/agent/reboot/<agent_name>')
def api_reboot(agent_name = None):
    try:
        cmds=['sudo killall btagent','sudo shutdown -r now']
        agent_control.try_ssh('ioteye-'+agent_name, 'ioteye', 'ioteye1234',cmds,3)
    except:
        print ('Not successful reboot '+agent_name)


@route('/agent/restart/<agent_name>')
def api_restart(agent_name = None):
    try:
        cmds=['sudo killall btagent','cd agent','sudo hciconfig hci0 down','sudo hciconfig hci0 up','sudo nohup ./btagent -a '+ HT + ' -w >/dev/null &']
        agent_control.try_ssh('ioteye-'+agent_name, 'ioteye', 'ioteye1234',cmds,3)
    except:
        print ('Not successful restart '+agent_name)


@route('/agent/upgrade/<agent_name>')
def api_upgrade(agent_name = None):
    try:
        source_path = "/home/ioteye/agent/btagent"
        destination_path = '/home/ioteye/agent/btagent'
        agent_control.upgrade('ioteye-'+agent_name, 'ioteye', 'ioteye1234',source_path,destination_path)
    except Exception as e:
        print(e)
        print ('Not successful upgrade_whitelist '+agent_name)


@route('/agent/upgrade_whitelist/<agent_name>')
def api_upgrade_whitelist(agent_name = None):
    try:
        source_path = "/home/ioteye/agent/beacon.txt"
        destination_path = '/home/ioteye/agent/beacon.txt'
        agent_control.upgrade('ioteye-'+agent_name, 'ioteye', 'ioteye1234',source_path,destination_path)
    except Exception as e:
        print(e)
        print ('Not successful upgrade_whitelist '+agent_name)


@route('/tag/<beacon_id>', method = "GET")
@auth_basic(check_user)
def api_get_beacon_pos(beacon_id=None):
    if beacon_id in beacon_position:
        pos = beacon_position[beacon_id]
        return {"BeaconID":beacon_id, "X":pos[0], "Y":pos[1], "Success":True}
    else:
        return {"BeaconID":beacon_id, "Message":"Tag Not Found","Success":False}
	#return {"beacon_position":beacon_position}

@route('/algo_comp/<beacon_id>', method = "GET")
@auth_basic(check_user)
def api_algo_compare(beacon_id = None):
    if beacon_id in beacon_position:
        pos = beacon_position[beacon_id]
        pos1 = beacon_position_kf[beacon_id]
        return {"BeaconID":beacon_id, "X":pos[0], "Y":pos[1], "X1":pos1[0], "Y1":pos1[1], "Success":True}
    else:
        return {"BeaconID":beacon_id, "Message":"Tag Not Found","Success":False}

@route('/tag/<beacon_id>/<beacon_mac>/<new_beacon_id>', method = "PUT")
@auth_basic(check_user)
def api_get_beacon_pos(beacon_id=None,beacon_mac=None,new_beacon_id=None):
    if beacon_id in beacon_position:
        if new_beacon_id:
            if beacon_mac in b_list:
                if new_beacon_id in beacon_position:
                    return {"BeaconID":beacon_id, "Message":"Tag ID Already Exist", "Success":False}
                else:
                    if beacon_id == b_list[beacon_mac]:
                        b_list[beacon_mac] = new_beacon_id
                        beacon_position[new_beacon_id] = beacon_position[beacon_id]
                        beacon_position.pop(beacon_id, None)
                        return {"BeaconID":beacon_id, "NewBeaconID":new_beacon_id,"Success":True}
                    else:
                        return {"BeaconID":beacon_id, "Message":"Tag ID Not Match MAC", "Success":False}
            else:
                return {"BeaconID":beacon_id, "Message":"MAC Not Found", "Success":False}
        else:
            return {"BeaconID":beacon_id, "Message":"No Tag ID Specified", "Success":False}
    else:
        return {"BeaconID":beacon_id, "Message":"Tag Not Found", "Success":False}
# Action Recorder
def write_log(m):
        today = time.strftime('%Y%m%d',time.localtime(time.time()))
        rsync_log = "data/%s_rsync.log" % today

        with open(rsync_log,'a') as fh:
            fh.write('%s\n' % m)
# Error Code:

#******************************************************************************#
# Main Program
if __name__ == "__main__":
    # Read Options
    argv = sys.argv[1:]

    try:
        opts, args = getopt.getopt(argv,"hlp:",["help","local","port="])
    except getopt.GetoptError:
        usage()
        sys.exit(2)

    for opt, arg in opts:
        if opt in ("-h","--help"):
            s
        elif opt in ("-r","--local"):
            enable_local = 1
        elif opt in ("-l","--log"):
            enable_log = 1
        elif opt in ("-d","--demo"):
            demo_mode = 1
        elif opt in ("-p","--port"):
            port_num = arg
            print("Not support yet, please change in configuration file.")
            usage()
            sys.exit(2)
        elif opt in ("-q","--api_port"):
            api_port_num = arg
            print("Not support yet, please change in configuration file.")
            usage()
            sys.exit(2)

    print("Current Python Version is %d.%d.%d"%(sys.version_info.major,
                                                sys.version_info.minor,
                                                sys.version_info.micro))

#******************************************************************************#
    # Read Configurations
    CP = ConfigParser.ConfigParser()
    CP.read("parameter/config.ini")
    EAC = int(CP.get('Parameters','EAC'))
    MA = int(CP.get('Parameters','MA'))
    PMA = int(CP.get('Parameters','PMA'))
    PN = float(CP.get('Parameters','PN'))
    PA = int(CP.get('Parameters','PA'))
    RN = int(CP.get('Parameters','RN'))
    DMR = int(CP.get('Parameters','DMR'))
    ACP = float(CP.get('Parameters','ACP'))
    MAL = int(CP.get('Parameters','MAL'))
    DF = float(CP.get('Parameters','DF'))

    HT = str(CP.get('Network','HT'))
    PT = int(CP.get('Network','PT'))
    APT = int(CP.get('Network','APT'))

#******************************************************************************#

    print("Loading Tag Information", end='')
    # Read Beacon Info
    b_list = {}     # Beacon list
    with open("parameter/list_beacon.txt") as beaconFile:
        for line in beaconFile:
            if line[0]=="#":
                continue
            temp_info = line.strip("\n").split("|")
            if len(temp_info)>1:
                b_list[temp_info[1]]=temp_info[0]
                print(".",end='')
    print("Success")

#******************************************************************************#

    print("Loading Agent Information",end='')
    # Read Agent Position
    a_name={}       # MAC/Name Dictionary
    a_pos={}        # MAC/Position Dictionary
    a_room={}       # MAC/Room Dictionary
    with open("parameter/list_agent.txt") as agentFile:
        with open("GUI/1.json") as dataFile:
            data = json.load(dataFile)
        i=0
        data['reader']=[]
        for line in agentFile:
            if line[0]=="#":
                continue
            temp_info = line.strip("\n").split("|")
            a_name[temp_info[1]]=temp_info[0]
            a_pos[temp_info[1]]=[int(temp_info[2]),int(temp_info[3])]
            a_room[temp_info[1]]=int(temp_info[-1])
            data['reader'].append({"loc":a_pos[temp_info[1]], "id":temp_info[0]})
            i += 1
            print(".",end='')
        data['tag']=[]
        for i in range(len(b_list)):
            data['tag'].append({"loc":[0,0],"id":i+1})
        with open("GUI/1.json","w") as dataFile:
            json.dump(data, dataFile)
    print("Success")

#******************************************************************************#

    print("Loading Room Info",end='')
    r_param = {}
    with open("parameter/list_room_max.txt") as roomFile:
        for line in roomFile:
            if line[0]=="#":
                continue
            temp_info = line.strip("\n").split("|")
            if len(temp_info)>1:
                r_param[int(temp_info[0])] =[float(x) for x in temp_info[1].split(",")]
                print(".",end='')

                #r_bound[int(temp_info[0])] = [x.split(",") for x in temp_info[2:]]
    print("Success")

#******************************************************************************#

    print("Establishing API...........",end='')
    api_thread = threading.Thread(target = run, kwargs=dict(host='0.0.0.0',port = api_port_num))
    api_thread.daemon = True
    api_thread.start()
    print("Success")


    if enable_local == 0:
        main_server()
    else:
        main_local()
