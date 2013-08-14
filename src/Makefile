CPP_CC = g++
C_CC   = gcc

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

CPP_SRC = \
CWebGetMain.cpp \
CWebGet.cpp \
CUrl.cpp \
CHtmlComment.cpp \
CHtml.cpp \
CHtmlNamedChar.cpp \
CHtmlParser.cpp \
CHtmlTag.cpp \
CHtmlTagDef.cpp \
CHtmlText.cpp \
CHtmlToken.cpp \
CHtmlUtil.cpp \
CHttp.cpp \
CTcp.cpp \
CThread.cpp \
CThreadPool.cpp

C_SRC = \
pthread_rdwr.c \

CPP_OBJS = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(CPP_SRC))

C_OBJS = $(patsubst %.c,$(OBJ_DIR)/%.o,$(C_SRC))

CPPFLAGS = \
-I$(INC_DIR) \
-I. \
-I../../CArgs/include \
-I../../CImageLib/include \
-I../../CFile/include \
-I../../COS/include \
-I../../CRegExp/include \
-I../../CMath/include \
-I../../CStrUtil/include \
-I../../CUtil/include \

LFLAGS = \
$(LEBUG) \
-L$(LIB_DIR) \
-L../../CArgs/lib \
-L../../CImageLib/lib \
-L../../CFile/lib \
-L../../COS/lib \
-L../../CRegExp/lib \
-L../../CStrUtil/lib \
-lCArgs \
-lCImageLib \
-lCFile \
-lCOS \
-lCRegExp \
-lCStrUtil \
-pthread

.SUFFIXES: .cpp

$(CPP_OBJS): $(OBJ_DIR)/%.o: %.cpp
	$(CPP_CC) -c $< -o $(OBJ_DIR)/$*.o $(CPPFLAGS)

$(C_OBJS): $(OBJ_DIR)/%.o: %.c
	$(C_CC) -c $< -o $(OBJ_DIR)/$*.o $(CPPFLAGS)

$(BIN_DIR)/CWebGet: $(CPP_OBJS) $(C_OBJS)
	$(CPP_CC) -o $(BIN_DIR)/CWebGet $(CPP_OBJS) $(C_OBJS) $(LFLAGS) -ltre
