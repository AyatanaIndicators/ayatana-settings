#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import psutil
import os
from .appdata import APPNAME

def isRunning():

    for pProc in psutil.process_iter():

        sName = pProc.name

        if not isinstance(sName, str):

           sName = pProc.name()

        if sName == 'python3' or sName == 'python':

            lCmdLine = pProc.cmdline

            if not isinstance(lCmdLine, list):

               lCmdLine = pProc.cmdline()

            for sCmd in lCmdLine:

                if sCmd.endswith(APPNAME) and pProc.pid != os.getpid():

                    return True

        elif sName.endswith(APPNAME) and pProc.pid != os.getpid():

            return True

    return False

def isSystemd():

    for pProc in psutil.process_iter():

        sName = pProc.name

        if not isinstance(sName, str):

           sName = pProc.name()

        if sName == 'systemd':

            lCmdLine = pProc.cmdline

            if not isinstance(lCmdLine, list):

               lCmdLine = pProc.cmdline()

            for sCmd in lCmdLine:

                if sCmd == '--user':

                    return True

    return False
