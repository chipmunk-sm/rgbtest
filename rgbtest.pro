#/* This file is part of rgbtest - LCD Dead Pixel Test
# * Copyright (C) 2017 chipmunk-sm <dannico@linuxmail.org>
# * https://github.com/chipmunk-sm/rgbtest
# *
# * This program is free software: you can redistribute it and/or modify
# * it under the terms of the GNU General Public License as published by
# * the Free Software Foundation, either version 3 of the License, or
# * (at your option) any later version.
# *
# * This program is distributed in the hope that it will be useful,
# * but WITHOUT ANY WARRANTY; without even the implied warranty of
# * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# * GNU General Public License for more details.
# *
# * You should have received a copy of the GNU General Public License
# * along with this program.  If not, see <http://www.gnu.org/licenses/>.
# */

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = rgbtest
TEMPLATE = app

CONFIG += c++11

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

DISTFILES += \
    README.md \
    LICENSE \
    debian/source/format \
    debian/compat \
    debian/control \
    debian/copyright \
    debian/rgbtest.install \
    debian/changelog \
    debian/rules \
    data/rgbtest.desktop \
    data/rgbtest.svg.png \
    data/rgbtest.svg

RESOURCES += \
    rgbtest.qrc

win32 {
  RC_FILE     += rgbtest.rc
  OTHER_FILES += rgbtest.rc
}
