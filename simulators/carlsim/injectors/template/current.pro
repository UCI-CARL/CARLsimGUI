SPIKESTREAM_ROOT_DIR = ../../../..

include( $${SPIKESTREAM_ROOT_DIR}/spikestream.pri )

TEMPLATE = lib

TARGET = oat

DESTDIR = $${SPIKESTREAM_ROOT_DIR}/plugins/simulation/carlsimmonitors

QT += xml opengl sql


#----------------------------------------------#
#---              INCLUDE PATH              ---#
#----------------------------------------------#
INCLUDEPATH += src/gui src/managers \
				$${SPIKESTREAM_ROOT_DIR}/simulators/carlsim/include \
				$${SPIKESTREAM_ROOT_DIR}/library/include \
				$${SPIKESTREAM_ROOT_DIR}/applicationlibrary/include
unix {
	INCLUDEPATH += /usr/local/include
}
win32 {
	INCLUDEPATH += $${SPIKESTREAM_ROOT_DIR}/../GitHub/CARLsim4/carlsim/interface/inc
}


#----------------------------------------------#
#---               LIBRARIES                ---#
#----------------------------------------------#
unix:!macx {
	LIBS += -lnemo -L$${SPIKESTREAM_ROOT_DIR}/lib  -lspikestreamapplication -lspikestream
}
win32{
	LIBS += -lcarlsimwrapper0 -L$${SPIKESTREAM_ROOT_DIR}/plugins/simulation/
	LIBS += -L$${SPIKESTREAM_ROOT_DIR}/lib -lspikestreamapplication0 -lspikestream0
	LIBS += -lcarlsim -L$${SPIKESTREAM_ROOT_DIR}/../GitHub/CARLsim4/x64/Debug
}
macx {
        LIBS += -lnemowrapper -L$${SPIKESTREAM_ROOT_DIR}/plugins/simulation/
        LIBS += -lnemo -lspikestreamapplication -lspikestream -L$${SPIKESTREAM_ROOT_DIR}/lib
}


#----------------------------------------------#
#-----               GUI                  -----#
#----------------------------------------------#
HEADERS += src/gui/CarlsimOatWidget.h
SOURCES += src/gui/CarlsimOatWidget.cpp


#----------------------------------------------#
#-----              Managers              -----#
#----------------------------------------------#
HEADERS += src/managers/CarlsimOatManager.h
SOURCES += src/managers/CarlsimOatManager.cpp

