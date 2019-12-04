
CROSS=arm-hisiv500-linux-
CC  = $(CROSS)g++
AR  = $(CROSS)ar
ARFLAGS = rcv
OUTPUT_EXECUT := hisi_feeddog

CFLAGS  += -O2 -lpthread -lm -ldl 
CFLAGS  += $(INCLUDE)


PROJECT_SRC+=./hisi_feeddog.c
PROJECT_SRC+=./MsgInterface.c

INCLUDE += -I./

OBJS = $(addprefix ./obj/, $(addsuffix .o,$(basename $(notdir $(PROJECT_SRC)))))

all:
	@mkdir -p obj
	@for file in $(PROJECT_SRC); do \
        OBJ=`basename $$file|sed -e 's/\.cpp/\.o/' -e 's/\.c/\.o/'`; \
        echo -e "$(CC) -c -o $$OBJ\n$$file"; \
		$(CC) $(CFLAGS) -c -o obj/$$OBJ $$file; \
    done; \
	echo $(OBJS)
	$(CC) $(CFLAGS) -o $(OUTPUT_EXECUT) $(OBJS) -L$(LIB_DIR) -l$(LIBA)
	@echo "Compile complete"
	
.PHONY: clean, copy
clean:
	@echo "DEL OBJ files"
	@rm -fR ./obj/*.o
	@rm -fR $(OUTPUT_EXECUT)
	@echo "Clean complete"    
