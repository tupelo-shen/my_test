import logging

logging.basicConfig(level=logging.DEBUG,
                    format='%(asctime)s - %(filename)s[line:%(lineno)d] - %(levelname)s: %(message)s')
logging.debug(u"1")
logging.info(u"2")
logging.warning(u"3")
logging.error(u"4")
logging.critical(u"5")

