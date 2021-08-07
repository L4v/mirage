TARGET=mxbx_mirage
LIB=/mnt/c/Prj/Altera/C/moxiebox/modules/lib

FILES = $(TARGET) $(LIB)/mouse $(LIB)/sprintf $(LIB)/string $(LIB)/stdio $(LIB)/graphics 

ASM_LIST := $(foreach file,$(FILES), $(file).s) $(LIB)/keyboard.s $(LIB)/files.s $(LIB)/graphics320.s $(LIB)/graphics640.s $(LIB)/fonts.s $(LIB)/consts.s

all: compile assemble

compile:
	@for file in $(FILES); do \
    moxiebox-gcc -DKERNEL -fsigned-char -S $${file}.c -o $${file}.s -fverbose-asm; \
	done
	
# NOTE(Jovan): Maybe an -o flag for outputting .bin files to a specific dir
assemble:
	java -jar ../FPGA_Assembler.jar -s -f $(ASM_LIST)

