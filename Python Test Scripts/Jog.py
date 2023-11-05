### Commands all <AXES_NUM> motors to repeat following sequence:             ###
### make a number of steps in one direction then the opposite way            ###

# Python version check (designed to work with 3.x)
import sys   
#print(sys.version_info)
if (sys.version_info.major < 3):
    print("Your version of Python is %d.%d"%(sys.version_info.major,sys.version_info.minor))
    print("Use version 3.x instead")
    exit()

import serial # pip install pyserial
import random
import time
import atexit

# sets resolution of Sleep() closer to actual 1ms
import ctypes
winmm = ctypes.WinDLL('winmm')
winmm.timeBeginPeriod(1)

#---------------------------------- Settings ----------------------------------#
AXES_NUM      = 3       # number of used axes
FRAME_PERIOD  = 0.999   # will be slightly bigger due to OS inner workings
MAX_VAL       = 40      # MAX_VAL / FRAME_PERIOD gives you speed
FRAMES_IN_JOG = 5	#
JOG_STEPS     = (MAX_VAL * FRAMES_IN_JOG) # overall number of steps in one
                                          # direction, not used
COM_NUM       = "COM4"
BAUD          = 115200  # speed doesn't change delays

#------------------------------------ Vars ------------------------------------#
Port = serial.Serial()
values = [0] * AXES_NUM # list, not (super)efficient array
cnt = 0
dir = 1

#------------------------------------ Code ------------------------------------#
def SendData(prt):
    global cnt, dir

    string = "%03d;%1d;" % (FRAME_PERIOD*1000, AXES_NUM)
    
    if (cnt >= FRAMES_IN_JOG):
        cnt = 0
        dir *= -1 # switch between 1 and -1
    cnt += 1   
    #print(cnt * MAX_VAL) # dbg

    for i in range(AXES_NUM):
        values[i] = dir * MAX_VAL
        #values[i] = random.randint(-MAX_VAL, MAX_VAL) # dbg
        #values[i] = -30                               # dbg
        string += "%+04d;" % values[i]
    string += "\n" 
    buff = bytearray(string.encode('ascii'))
    #print(values) # dbg
    print(string[:-1])   # dbg
    prt.write(buff)


def atexit_handler():
    print("Exiting...")
    try:
        if (Port.isOpen()):
            Port.close()
            print(COM_NUM + " closed...")
    except:
        print("All ports were closed earlier...")



if __name__ == "__main__":
    # open the serial port
    Port.port = COM_NUM
    Port.baudrate = BAUD
    Port.timeout = 60000
    Port.stopbits = 2
    atexit.register(atexit_handler)
    try:
        #Port = serial.Serial(COM_NUM, BAUD, timeout=60000)
        Port.open()
        if (Port.isOpen()):
            print(Port.name + " is opened...")
        else:
            print("Error: can't open " + COM_NUM)
            exit()
    except:
        print("Error: can't open " + COM_NUM)
        exit()

    while True:
        ns0 = time.time_ns() # os timing dbg
        SendData(Port)
        ns1 = time.time_ns() # os timing dbg

        time.sleep(FRAME_PERIOD)
        ns2 = time.time_ns() # os timing dbg

        ms1 = (ns1 - ns0) // 1_000_000  # os timing dbg
        ms2 = (ns2 - ns1) // 1_000_000  # os timing dbg
        
        #print(ms1, ms2) # os timing dbg
        
    Port.close()
    print(COM_NUM + " closed")


