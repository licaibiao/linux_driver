TARGET := test

COMPILE_CROSS:= arm-hisiv500-linux-
CC := $(COMPILE_CROSS)gcc
GG := $(COMPILE_CROSS)g++

 
INC     := -I ./
SRC_C	:= $(wildcard ./*.c)
SRC_OBJ	:= $(patsubst %.c,%.o,$(SRC_C))

CFLAGS += -g 

$(TARGET): $(SRC_OBJ)
	$(CC) -o $(TARGET) $(SRC_OBJ) $(LDLIBS)
	
$(SRC_OBJ):%.o:%.c
	echo $(SRC_OBJ)
	$(CC) $(CFLAGS) -c $< -o $@ $(INC)	
 
clean:
	$(RM) ./*.o 
	$(RM) $(TARGET)
