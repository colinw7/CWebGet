#EXTRA_CFLAGS = `gtk-config --cflags`
#EXTRA_LFLAGS = `gtk-config --libs`

EXTRA_CFLAGS = \
-I/usr/include/gtk-2.0 \
-I/usr/include/glib-2.0 \
-I/usr/lib/gtk-2.0/include \
-I/usr/lib/glib-2.0/include \
-I/usr/include/cairo \
-I/usr/include/pango-1.0 \
-I/usr/include/atk-1.0

EXTRA_LFLAGS = \
-LI/usr/lib/gtk-2.0

CDEBUG = -g -Wall
LDEBUG = -g

LIBS = \
-lCWebGet -lCThread -lCHtml -lCFile -lCEnv -lCImageLib -lCFile \
-lCHttp -lCTcp -lCOS -lCGlob -lCUtil -lCStrUtil -lCFileUtil \
-lCUrl -lCRegExp -lCOS -lCStrNCase -lCPrintF \
-lgtk-x11-2.0 -lgdk-x11-2.0 -lglib-2.0 -lgobject-2.0 \
 $(X11_LIBS) -lXi \
-lpthread -lpng -ljpeg -lm -lc \

dummy:

PROG = GWebGet

CPP_SRC = GWebGet.cpp

include $(MAKE_DIR)/bin.mk
