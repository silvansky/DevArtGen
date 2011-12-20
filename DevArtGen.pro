QT       += core gui

TARGET = DevArtGen
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

macx: {
	ICON = icon.icns
	target.path = /Applications
	plist.path = /Applications/$${TARGET}.app/Contents/
	plist.files = ./Info.plist
	INSTALLS = target plist
}

win32: {
	win32:RC_FILE      = icon.rc
}

OTHER_FILES += \
	icon.rc \
	icon.icns \
	icon.ico \
	Info.plist
