CC = g++
AR = ar
RM = rm

CDEBUG = -g

INC_DIR = ../include
OBJ_DIR = ../obj
LIB_DIR = ../lib

all: $(LIB_DIR)/libCWebGet.a

SRC = \
CWebGet.cpp \
CUrl.cpp \
CHttp.cpp \
CTcp.cpp \

OBJS = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC))

CPPFLAGS = \
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

clean:
	$(RM) -f $(OBJ_DIR)/*.o
	$(RM) -f $(LIB_DIR)/libCWebGet.a

$(OBJS): $(OBJ_DIR)/%.o: %.cpp
	$(CC) -c $< -o $(OBJ_DIR)/$*.o $(CPPFLAGS)

.SUFFIXES: .cpp

$(LIB_DIR)/libCWebGet.a: $(OBJS)
	$(AR) crv $(LIB_DIR)/libCWebGet.a $(OBJS)

