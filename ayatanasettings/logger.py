#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import logging
from .appdata import APPNAME

class Formatter(logging.Formatter):

    def __init__(self):

        logging.Formatter.__init__(self, '[%(asctime)s] %(levelname)s: %(message)s')
        self.default_msec_format = '%s.%03d'

    def format(self, record):

        dColours = {'WARNING': '33', 'INFO': '32', 'DEBUG': '37', 'CRITICAL': '35', 'ERROR': '31'}
        record.levelname = '\033[1;' + dColours[record.levelname] + 'm' + record.levelname + '\033[0m'

        return logging.Formatter.format(self, record)

logger = logging.getLogger(APPNAME)
logger.setLevel(logging.DEBUG)
pStreamHandler = logging.StreamHandler()
pStreamHandler.setFormatter(Formatter())
logger.addHandler(pStreamHandler)
