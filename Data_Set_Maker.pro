#-------------------------------------------------
#
# Project created by QtCreator 2016-10-04T22:54:23
#
#-------------------------------------------------

QT       += core
QT       += widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Data_Set_Maker
TEMPLATE = app


SOURCES += main.cpp\
        data_set_maker.cpp \
    CSaveImg/CSaveImg.cpp

HEADERS  += data_set_maker.h \
    CSaveImg/CSaveImg.h

INCLUDEPATH += /usr/local/include
INCLUDEPATH += /usr/lib/x86_64-linux-gnu/hdf5/serial/include
INCLUDEPATH += /usr/local/cuda/include

LIBS += -L/usr/local/lib
LIBS += -lopencv_core
LIBS += -lopencv_videoio
LIBS += -lopencv_highgui
LIBS += -lopencv_imgproc
LIBS += -lopencv_imgcodecs

FORMS    += data_set_maker.ui
