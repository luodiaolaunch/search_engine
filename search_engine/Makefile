SRC_DIR := ./src
INC_DIR := ./include
PY_DIR := ./include/python2.7
#INC_DIR += $(PY_DIR)
EXE_DIR := ./bin
LIB_DIR := ./lib
LIB_SO := -ljson_linux-gcc-4.9.2_libmt \
		  -l_acl \
		  -l_protocol \
		  -l_acl_cpp
EXE := $(EXE_DIR)/searchEngine_server
CC := g++
CFLAGS :=   -std=c++11 -llog4cpp -lpthread  $(addprefix -I, $(INC_DIR))
SRC_OBJS := $(wildcard $(SRC_DIR)/*.cc)
#SRC_OBJ := $(SRC:.cc=.o)
$(EXE):$(SRC_OBJS) $(INC_OBJS)
	$(CC) -o $@  $(SRC_OBJS) $(CFLAGS) -L $(LIB_DIR) $(LIB_SO)
.PHONY:clean
clean:
	rm -f $(EXE)
