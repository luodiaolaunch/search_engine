SRC_DIR := ./
INC_DIR := ./
EXE_DIR := ./
JSON_DIR := ../lib
EXE := $(EXE_DIR)/searchEngine_client
CC := g++
CFLAGS :=  -std=c++11 -llog4cpp -lpthread  $(addprefix -I, $(INC_DIR))
SRC_OBJS := $(wildcard $(SRC_DIR)/*.cc)
#SRC_OBJ := $(SRC:.cc=.o)
$(EXE):$(SRC_OBJS) $(INC_OBJS)
	$(CC) -g -o $@  $(SRC_OBJS) $(CFLAGS) -L $(JSON_DIR) -ljson_linux-gcc-4.9.2_libmt

.PHONY:clean
clean:
	rm -f $(EXE)
