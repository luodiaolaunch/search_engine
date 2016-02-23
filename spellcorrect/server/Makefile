SRC_DIR := ./src
INC_DIR := ./inc
EXE_DIR := ./bin
EXE := $(EXE_DIR)/spellcheck_server
CC := g++
CFLAGS :=  -std=c++11 -llog4cpp -lpthread  $(addprefix -I, $(INC_DIR))
SRC_OBJS := $(wildcard $(SRC_DIR)/*.cc)
#SRC_OBJ := $(SRC:.cc=.o)
$(EXE):$(SRC_OBJS) $(INC_OBJS)
	$(CC) -o $@  $(SRC_OBJS) $(CFLAGS)

.PHONY:clean
clean:
	rm -f $(EXE)
