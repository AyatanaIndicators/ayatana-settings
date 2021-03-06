#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from setuptools import setup
from ayatanasettings.appdata import *
import os, polib, configparser

m_lstDataFiles = []

iFile = open('data/applications/{}.desktop.in'.format(APPNAME), 'r')
oConfigParser = configparser.ConfigParser()
oConfigParser.optionxform = str
oConfigParser.read_file(iFile)
iFile.close()

for strRoot, lstDirnames, lstFilenames in os.walk('po'):

    for strFilename in lstFilenames:

        if strFilename.endswith('.po'):

            strLocale = os.path.splitext(strFilename)[0]

            for oEntry in polib.pofile('po/' + strFilename).translated_entries():

                if oEntry.msgid == oConfigParser['Desktop Entry']['Name']:

                    oConfigParser['Desktop Entry']['Name[' + strLocale + ']'] = oEntry.msgstr

                elif oEntry.msgid == oConfigParser['Desktop Entry']['Comment']:

                    oConfigParser['Desktop Entry']['Comment[' + strLocale + ']'] = oEntry.msgstr

for sSection in oConfigParser.sections():

    oConfigParser[sSection] = dict(sorted(oConfigParser[sSection].items(), key=lambda lParams: lParams[0]))

oFile = open('data/applications/{}.desktop'.format(APPNAME), 'w')
oConfigParser.write(oFile, False)
oFile.close()

for strRoot, lstDirnames, lstFilenames in os.walk('po'):

    for strFilename in lstFilenames:

        if strFilename.endswith('.po'):

            strLocale = os.path.splitext(strFilename)[0]

            if strLocale != APPNAME:

                strLocaleDir = 'data/locale/' + strLocale + '/LC_MESSAGES/'

                if not os.path.isdir(strLocaleDir):

                    os.makedirs(strLocaleDir)

                polib.pofile('po/' + strFilename).save_as_mofile(strLocaleDir + APPNAME + '.mo')

for strRoot, lstDirnames, lstFilenames in os.walk('data'):

    for strFilename in lstFilenames:

        if strFilename == '.gitkeep':

            continue

        elif strFilename == 'ayatana-settings.desktop.in':

            continue

        strPath = os.path.join(strRoot, strFilename)
        m_lstDataFiles.append(("share/{data}".format(data=os.path.dirname(strPath).replace('data/', '')), [strPath]))

m_lstDataFiles.append(('bin/', ['ayatana-settings']))

setup(
    name = APPNAME,
    version = APPVERSION,
    description = APPDESCRIPTION,
    long_description = APPLONGDESCRIPTION,
    url = APPURL,
    author = APPAUTHOR,
    author_email = APPMAIL,
    maintainer = APPAUTHOR,
    maintainer_email = APPMAIL,
    license = 'GPL-3',
    classifiers = [
        'Development Status :: 5 - Production/Stable',
        'Environment :: X11 Applications :: GTK',
        'Intended Audience :: End Users/Desktop',
        'License :: OSI Approved :: GNU General Public License v3 or later (GPLv3+)',
        'Natural Language :: English',
        'Operating System :: POSIX :: Linux',
        'Programming Language :: Python :: 3 :: Only',
        'Topic :: Desktop Environment'
        ],
    keywords = APPKEYWORDS,
    packages = [APPNAME.replace('-', '')],
    data_files = m_lstDataFiles,
    install_requires = [ 'setuptools', ],
    platforms = 'UNIX'
    )
