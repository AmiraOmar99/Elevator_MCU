# write the files without extension
FILES := main
mainFile := main
SiLabsPath = C:\SiLabs

space := $(null) #
comma := ,
SRCS := $(foreach file,$(FILES),$(file).c)
OBJSList := $(foreach file, $(FILES), $(file).OBJ)
OBJS := $(subst $(space), $(comma),$(strip $(OBJSList)))
CC      := $(SiLabsPath)/MCU/IDEfiles/C51/BIN/C51.exe
OH 		:= $(SiLabsPath)/MCU/IDEfiles/C51/BIN/OH51.exe
ld 		:= $(SiLabsPath)/MCU/IDEfiles/C51/BIN/BL51.exe
	
default: link
	$(OH) $(mainFile).ABS
	make clean
	
link:
	$(foreach var,$(SRCS), $(CC) $(var) DB OE BR;)
	$(ld) $(OBJS) TO $(mainFile).ABS

clean:
	rm *.OBJ *.LST *.M51 *.ABS