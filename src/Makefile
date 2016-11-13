CC = g++

RM = rm

CDEBUG = -g
LDEBUG = -g

INC_DIR = ../include
OBJ_DIR = ../obj
LIB_DIR = ../lib
BIN_DIR = ../bin

all: $(BIN_DIR)/CWebGet

clean:
	$(RM) -f $(OBJ_DIR)/*.o
	$(RM) -f $(BIN_DIR)/CWebGet

SRC = \
CWebGetMain.cpp \
CWebGet.cpp \
CUrl.cpp \
CHttp.cpp \
CTcp.cpp \

OBJS = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC))

CFLAGS = \
-std=c++11 \
-I$(INC_DIR) \
-I. \
-I../../CHtml/include \
-I../../CArgs/include \
-I../../CImageLib/include \
-I../../CFileUtil/include \
-I../../CFile/include \
-I../../CThread/include \
-I../../COS/include \
-I../../CRegExp/include \
-I../../CMath/include \
-I../../CStrUtil/include \
-I../../CUtil/include \

LFLAGS = \
$(LEBUG) \
-L$(LIB_DIR) \
-L../../CHtml/lib \
-L../../CArgs/lib \
-L../../CImageLib/lib \
-L../../CFileUtil/lib \
-L../../CFile/lib \
-L../../CThread/lib \
-L../../COS/lib \
-L../../CRegExp/lib \
-L../../CStrUtil/lib \
-lCHtml \
-lCArgs \
-lCImageLib \
-lCThread \
-lCFileUtil \
-lCFile \
-lCOS \
-lCRegExp \
-lCStrUtil \
-pthread -ltre

.SUFFIXES: .cpp

$(OBJS): $(OBJ_DIR)/%.o: %.cpp
	$(CC) -c $< -o $(OBJ_DIR)/$*.o $(CFLAGS)

$(BIN_DIR)/CWebGet: $(OBJS)
	$(CC) -o $(BIN_DIR)/CWebGet $(OBJS) $(LFLAGS)
