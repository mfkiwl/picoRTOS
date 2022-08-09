# LAUNCHXL-F28069M board
OUT_RAM := launchxl-f28069m.out
OUT_FLASH := launchxl-f28069m_Flash.out

CC := cl2000

# directories
PICORTOS_DIR := ../..
BUILD_DIR := build

# SDK (you need TI C2000Ware SDK for this one)
SDK_DIR := /c/ti/C2000Ware_4_00_00_00
DEVICES_DIR := $(SDK_DIR)/device_support/f2806x/common

DEFINE := -D__TMS320C28XX__
INCLUDE := -I. -I$(PICORTOS_DIR) -I$(PICORTOS_DIR)/arch/include
INCLUDE += -I$(PICORTOS_DIR)/arch/c2000/c28x

TOOLCHAIN_DIR := $(dir $(shell which $(CC)))..
INCLUDE += -I$(TOOLCHAIN_DIR)/include

CFLAGS += -v28 -ml -mt -O0 $(DEFINE) $(INCLUDE)
CFLAGS += --float_support=fpu32 --preproc_with_compile 
CFLAGS += --preproc_dependency="$(basename $(<F)).d_raw"

SFLAGS := $(CFLAGS)

LDRAM := -l28069_RAM_lnk.cmd
LDFLASH := --define=_FLASH -lF28069M.cmd

LDFLAGS := -v28 -ml -mt -O0 -g -z --stack_size=0x200
LDFLAGS += $(INCLUDE) --warn_sections --reread_libs --priority
LDFLAGS += -i"$(DEVICES_DIR)/cmd" --disable_auto_rts --rom_model
LDFLAGS += --search_path="$(TOOLCHAIN_DIR)/lib" -lrts2800_fpu32.lib

# picoRTOS
SRC_C_FILES += $(PICORTOS_DIR)/picoRTOS.c
SRC_C_FILES += $(PICORTOS_DIR)/arch/c2000/c28x/picoRTOS_port.c
SRC_S_FILES += $(PICORTOS_DIR)/arch/c2000/c28x/picoRTOS_portasm.asm

# IPCs extension
SRC_C_FILES += $(PICORTOS_DIR)/ipc/picoRTOS_futex.c

# demo
SRC_C_FILES += blink.c

OBJ_FILES := $(SRC_C_FILES:%.c=%.o)
OBJ_FILES += $(SRC_S_FILES:%.asm=%.o)

all: $(OUT_RAM) $(OUT_FLASH)

$(OUT_RAM): $(OBJ_FILES)
	-mkdir -p $(@D)
	$(CC) $(LDFLAGS) $^ $(LDRAM) -o $@

$(OUT_FLASH): $(OBJ_FILES)
	-mkdir -p $(@D)
	$(CC) $(LDFLAGS) $^ $(LDFLASH) -o $@

%.o: %.c
	-mkdir -p $(@D)
	echo $(SRC_C_FILES)
	$(CC) $(CFLAGS) -c $< --output_file=$@

%.o: %.asm
	-mkdir -p $(@D)
	$(CC) $(SFLAGS) -c $< --output_file=$@

splint:
	splint $(SPLINTFLAGS) $(DEFINE) $(INCLUDE) $(SPLINT_C_FILES)

uncrustify:
	uncrustify -c $(PICORTOS_DIR)/uncrustify.cfg \
	  $(INDENT_FILES) --no-backup

clean:
	-rm -rf $(BUILD_DIR)
	-rm -f $(OUT_RAM) $(OUT_FLASH)
	-rm -f $(OBJ_FILES) *.d_raw

.PHONY: clean splint uncrustify
