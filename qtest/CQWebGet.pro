TEMPLATE = app

QT += widgets

TARGET = CQWebGet

DEPENDPATH += .

MOC_DIR = .moc

QMAKE_CXXFLAGS += -std=c++14

CONFIG += debug

# Input
SOURCES += \
main.cpp \
CQWebGet.cpp \

HEADERS += \
CQWebGet.h \

FORMS += \
CQWebGet.ui

DESTDIR     = ../bin
OBJECTS_DIR = ../obj
LIB_DIR     = ../lib

INCLUDEPATH += \
. \
../include \
../../CWebGet/include \
../../CHtml/include \
../../CQUtil/include \
../../CImageLib/include \
../../CFont/include \
../../CCeil/include \
../../CArgs/include \
../../CFile/include \
../../CFileUtil/include \
../../COS/include \
../../CStrUtil/include \
../../CRegExp/include \
../../CUtil/include \
../../CMath/include \
../../CGlob/include \
../../CReadLine/include \
../../CRGBName/include \

unix:LIBS += \
-L$$LIB_DIR \
-L../../CWebGet/lib \
-L../../CHtml/lib \
-L../../CQUtil/lib \
-L../../CImageLib/lib \
-L../../CFont/lib \
-L../../CCeil/lib \
-L../../CArgs/lib \
-L../../CConfig/lib \
-L../../CReadLine/lib \
-L../../CFile/lib \
-L../../CFileUtil/lib \
-L../../CStrUtil/lib \
-L../../CRegExp/lib \
-L../../CGlob/lib \
-L../../CThread/lib \
-L../../CUtil/lib \
-L../../COS/lib \
-L../../CRGBName/lib \
-lCWebGet -lCHtml \
-lCQUtil -lCImageLib -lCFont -lCCeil -lCArgs -lCConfig -lCReadLine \
-lCFile -lCFileUtil -lCStrUtil -lCGlob -lCRegExp -lCRGBName -lCUtil -lCOS \
-lCThread -ljpeg -lpng -lcurses -ltre
