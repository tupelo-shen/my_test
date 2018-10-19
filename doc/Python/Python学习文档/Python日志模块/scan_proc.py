#!/usr/bin/env python
# -*- coding: utf8 -*-

import time
import datetime
import signal
import threading
import Queue
import logging
import logging.config
import ConfigParser
import json
from socket import *

continue_working = True

POWER_ON_WAIT = 2

logger = []
CONFIG_FILE = 'config.ini'

# Device infomation
ID = []
pre_uid = []
rd_count = 0

# network infomation
host = ""
port = 0
tctimeClient = []
reConCounter = 50

# Card SN
SN_PAGE_ST = 4
SN_PAGE_COUNT = 8

# Timeout
ReadCycleTime = 100
ReadInvalidCycleTime = 5
SendCycleTime = 100
ScanCycleTime = 100

# HeartBeat
SendFlag = False
HeartBeatCycleTime = 1000
HeartBeatInfo = "HeartBeat"

threads = []
thread_count = 0
workQueue = Queue.Queue(10)
queueLock = threading.Lock()

#--------------Global Function------------
def end_read():
    global continue_working
    logger.info("Ctrl+C captured, terminate app.")
    continue_working = False
    # GPIO.cleanup() # clean all GPIO

def LoadConfig(cfgfile):
    config = ConfigParser.ConfigParser()
    config.readfp(open(cfgfile))
    
    #---- Version -----
    ver = config.get("Version", "ver")
    logger.info("version info:%s"%ver)
    
    #---- Device ------
    global ID
    ID = config.get("Device", "ID")
    
    #---- NetWork -----
    global host
    host = config.get("Network", "host")
    global port
    port = int(config.get("Network", "port"))
    global reConCounter
    reConCounter = int(config.get("Network", "reConCounter"))
    logger.info("host:%s,port:%d,reConCounter:%d"%(host, port, reConCounter))
    
    #---- Cycle Time ----
    global ReadCycleTime
    global ReadInvalidCycleTime
    global ScanCycleTime
    global SendCycleTime
    ReadCycleTime = int(config.get("RFID", "GetRFIDDataCylceTime"))
    ReadInvalidCycleTime = int(config.get("RFID", "ReadInvalidCycleTime"))
    SendCycleTime = int(config.get("RFID", "SendDataCyleTime"))
    ScanCycleTime = int(config.get("RFID", "ScanCycleTime"))
    logger.info("RFID read interval=%dms,\
        RFID Invalid interval=%ds, \
        scan interval=%dms,\
        send interval=%dms"\
        %(ReadCycleTime,\
        ReadInvalidCycleTime,\
        ScanCycleTime,\
        SendCycleTime))

def ConfigLog():
    # debug, info, warning, error, critical
    global logger
    #log
    logging.config.fileConfig('logging.conf')
    logger = logging.getLogger('root')
    logger.debug('test root logger...')

def JsonPack(info):
    UTCTimeString = datetime.datetime.utcnow().isoformat() + 'Z'
    CardInfo = {"did":ID,"pld":{"dts":UTCTimeString,"tid":info}}
    JsonString = json.dumps(CardInfo, ensure_ascii=False)
    queueLock.acquire()
    workQueue.put(JsonString)
    queueLock.release()

# 
class ReadRFIDThread (threading.Thread):
    
    def __init__(self, threadID, name, q, CycleTime):
        threading.Thread.__init__(self)
        self.threadID = threadID
        self.name = name
        self.daemon = True
        self.q = q
        self.CycleTime = CycleTime
        self.start()
        logger.info("ReadRFIDThread:__init__")

    def run(self):
        logger.debug("Starting %s"%self.name)
        self.read_RFID_data(self.name, self.q)
        logger.debug("Exiting %s"%self.name)

    def read_RFID_data(self, threadName, q):
        global ReadCycleTime
        global ReadInvalidCycleTime
        global rd_count
        global pre_uid
        while (True == continue_working):
            sn_info = [SN_PAGE_ST, SN_PAGE_COUNT]
            # PageOperation.data_operation(read_ultralight_sn, sn_info)
            logger.debug("ReadRFIDThread is runging")
            time.sleep(float(self.CycleTime)/1000.0)

            rd_count += 1
            if(rd_count > (((ReadInvalidCycleTime*1000)/ReadCycleTime))):
                pre_uid = []
                rd_count = 0
            logger.debug("Scanning Card!")
        logger.info("receive a signal to exit ReadRFIDThread")

class ReadDimensionCodeThread (threading.Thread):
    
    def __init__(self, threadID, name, q, CycleTime):
        threading.Thread.__init__(self)
        self.threadID = threadID
        self.name = name
        self.q = q
        self.CycleTime = CycleTime
        self.start()
        logger.info("ReadDimensionCodeThread:__init__")
    
    def run(self):
        logger.info("Starting %s"%self.name)
        self.read_DimensionCode_data(self.name)
        logger.info("Exiting %s"%self.name)
    
    def read_DimensionCode_data(self, threadName):
        while (True == continue_working):
            barcodestr = raw_input("")
            logger.debug("barcodestr = %s"%barcodestr)
            JsonPack(barcodestr)
            time.sleep(float(self.CycleTime)/1000.0)
        logger.debug("receive a signal to exit ReadDimensionCodeThread")

class SendDataThread (threading.Thread):
    
    def __init__(self, threadID, name, q, CycleTime):
        threading.Thread.__init__(self)
        self.threadID = threadID
        self.name = name
        self.daemon = True
        self.q = q
        self.CycleTime = CycleTime
        self.start()
        logger.debug("SendDataThread:__init__")
        global tctimeClient
    
    def connect(self):
        global tctimeClient
        ret = True
        try:
            tctimeClient = socket(AF_INET,SOCK_STREAM)
            tctimeClient.connect((host, port))
            logger.info("connecting to %s "%host)
        except:
            logger.error("connect to %s failed"%host)
            ret = False
            pass
        return ret
    
    def run(self):
        while (True == continue_working):
            ConCounter = 0
            retConn = self.connect()
            while ((True == continue_working) and (retConn==False)):
                if 0 < reConCounter:
                    if ConCounter < reConCounter:
                        retConn = self.connect()
                    else:
                        logger.error("connect to %s failed\nPlease check Network, Restart Raspberry! "%host)
                else:
                    retConn = self.connect()
                time.sleep(5)
                ConCounter = ConCounter + 1
                logger.info("ConCounter=%d"%ConCounter)
            if (True == retConn):
                logger.info("connecting to %s success"%host)

                #send Device ID
                try:
                    self.send_data(tctimeClient, 'CONN', self.q)
                except:
                    logger.error("Device ID send failed")

                time.sleep(0.1)

                logger.info("continue_working=%d"%continue_working)
                # send data
                counter = 0
                while (True == continue_working):
                    # send RFID data
                    if (0 == counter % SendCycleTime):
                        ret = self.send_data(tctimeClient, 'RFID', self.q)
                    # send HeartBeat data
                    if ((False == SendFlag) and (0 == counter % HeartBeatCycleTime)):
                        ret = self.send_data(tctimeClient, 'HEARTBEAT', self.q)
                    if (ret == False):
                        logger.error("data send to %s failed"%host)
                        break
                    counter = counter + 1
                    time.sleep(0.001)
        logger.debug("receive a signal to exit SendDataThread")
    
    def send_data(self, sockclient, type, q):
        if 'RFID' == type:
            global SendFlag
            SendFlag = False
            queueLock.acquire()
            while not workQueue.empty():
                logger.debug("queue size: %d", workQueue.qsize())
                TagInfo = q.get()
                try:
                    sockclient.send("type2:"+ TagInfo)
                    recv = sockclient.recv(1024)
                    logger.info("%s:%s\n"%(type, recv))
                    SendFlag = True
                except:
                    logger.error("RFIDInfo send error")
                    queueLock.release()
                    return False
                logger.debug("TagInfo:%s"%TagInfo)
            queueLock.release()
        elif 'CONN' == type:
            try:
                sockclient.send("type1:"+ ID)
                recv = sockclient.recv(1024)
                logger.info("%s:%s"%(type, recv))
            except:
                logger.error("conn error")
                return False
        elif 'HEARTBEAT' == type:
            try:
                sockclient.send("type3:"+ ID)
                recv = sockclient.recv(1024)
                logger.debug("%s:%s"%(type, recv))
            except:
                logger.error("heartbeat error")
                return False
        else:
            logger.error("send_data/else error")
            return False
        return True

def Init():
    # Hook the SIGINT
    signal.signal(signal.SIGINT, end_read)

    # Welcome message
    print("Welcome to the RFIDReadingApp")
    print("Press Ctrl-C to stop.")
    print("Please Wait on %d seconds..."%POWER_ON_WAIT)

    time.sleep(POWER_ON_WAIT)

    #configure log
    ConfigLog()
    #load config info
    LoadConfig(CONFIG_FILE)

    global thread_count
    threadID = 1
    readthread = ReadRFIDThread(threadID, "ReadRFIDThread", workQueue, ReadCycleTime)
    threads.append(readthread)
    thread_count += 1

    threadID += 1
    sendthread = SendDataThread(threadID, "SendDataThread", workQueue, SendCycleTime)
    threads.append(sendthread)
    thread_count += 1
    
    threadID += 1
    dimensionReadthread = ReadDimensionCodeThread(threadID, "ReadDimensionCodeThread", workQueue, ScanCycleTime)
    threads.append(dimensionReadthread)
    thread_count += 1

if __name__ == "__main__":
    #RASPBERYY POWER ON
    Init()
    while True:
        alive = False
        for i in range(thread_count):
            alive = alive or threads[i].isAlive()
        if not alive:
            break
        time.sleep(1)
    #GPIO.cleanup() # clean up all GPIO
    logger.info("Main progaram is terminating!")
