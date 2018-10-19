import logging
import os.path
import time

# First, create the logger.
logger = logging.getLogger()
logger.setLevel(logging.INFO)  # switch

# Second, create a handler for the logger
rq = time.strftime('%Y%m%d%H%M', time.localtime(time.time()))
log_path = os.path.dirname(os.getcwd()) + '/logs/'
log_name = log_path + rq + '.log'
logfile = log_name

fh = logging.FileHandler(logfile, mode='w')
fh.setLevel(logging.DEBUG)  # logtofile switch

ch = logging.StreamHandler()
ch.setLevel(logging.WARNING)

# Third, setup the format of the logger 
formatter = logging.Formatter("%(asctime)s - %(filename)s[line:%(lineno)d] - %(levelname)s: %(message)s")
fh.setFormatter(formatter)
ch.setFormatter(formatter)

# Forth, add to handler qeueue
logger.addHandler(fh)
logger.addHandler(ch)

# logging
logger.debug('this is a logger debug message')
logger.info('this is a logger info message')
logger.warning('this is a logger warning message')
logger.error('this is a logger error message')
logger.critical('this is a logger critical message')