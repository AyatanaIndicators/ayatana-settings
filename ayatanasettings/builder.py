#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import gi

gi.require_version('Gtk', '3.0')

from gi.repository import Gtk
from .appdata import APPNAME, APPTITLE, APPDEBUG, APPEXECUTABLE
import os

def getClassVar(sId):

    return 'p' + sId[0:1].upper() + sId[1:]

def getDataPath(sPath):

    try:

        sExecPath = os.path.split(APPEXECUTABLE)[0]
        sDataPath = os.getcwd().replace(sExecPath, '')
        sRelativePath = os.path.join(sDataPath, sPath.lstrip('/'))

        if os.path.exists(sRelativePath):

            return sRelativePath

    except:

        pass

    return sPath

def buildApp(pApp):

    pBuilder = Gtk.Builder()
    pBuilder.set_translation_domain(APPNAME)
    pBuilder.add_from_file(getDataPath('/usr/share/' + APPNAME + '/' + APPNAME + '.glade'))
    pBuilder.connect_signals(pApp)

    for pObject in pBuilder.get_objects():

        try:

            pApp.__dict__[getClassVar(Gtk.Buildable.get_name(pObject))] = pObject

            if isinstance(pObject, Gtk.Notebook):

                def onNotebookPageSelected(pWidget, pEvent):

                    nMouseX, nMouseY = pEvent.get_coords()

                    for nPage in range(0, pWidget.get_n_pages()):

                        pLabel = pWidget.get_tab_label(pWidget.get_nth_page(nPage))
                        nX, nY = pLabel.translate_coordinates(pWidget, 0, 0)
                        rcSize = pLabel.get_allocation()

                        if nMouseX >= nX and nMouseY >= nY and nMouseX <= nX + rcSize.width and nMouseY <= nY + rcSize.height and pWidget.get_tab_label(pWidget.get_nth_page(nPage)).get_sensitive():

                            return False

                    return True

                pObject.connect('button-press-event', onNotebookPageSelected)

        except:

            pass

    pApp.pWindow.set_icon_from_file(getDataPath('/usr/share/icons/hicolor/scalable/apps/' + APPNAME + '.svg'))

    sTitle = APPTITLE

    if APPDEBUG:

        sTitle += ' - DEBUGGING MODE'

    pApp.pWindow.set_title(sTitle)
    pApp.pWindow.show_all()
