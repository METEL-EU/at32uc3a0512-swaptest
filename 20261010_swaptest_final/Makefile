
TOOLDIR  ?= C:/WinAVR-20100110/bin

CC        = $(TOOLDIR)/avr32-gcc.exe
SIZE      = $(TOOLDIR)/avr32-size.exe
OBJCOPY   = $(TOOLDIR)/avr32-objcopy.exe
OBJDUMP   = $(TOOLDIR)/avr32-objdump.exe

PART      = uc3a0512

SRCDIR    = src
BUILDDIR  = build
OBJDIR    = obj
BINDIR    = bin

CFLAGS  = -Wall \
          -Wcast-align \
          -Os \
          -mpart=$(PART) \
          -ffunction-sections \
          -masm-addr-pseudos

ASF          = $(SRCDIR)/SOFTWARE_FRAMEWORK
ASF_INCS     = -I$(SRCDIR) -I. \
               -I$(ASF)/DRIVERS/GPIO -I$(ASF)/DRIVERS/USART -I$(ASF)/DRIVERS/INTC \
               -I$(ASF)/DRIVERS/PDCA -I$(ASF)/DRIVERS/TC -I$(ASF)/BOARDS \
               -I$(ASF)/UTILS -I$(ASF)/UTILS/PREPROCESSOR \
               -I$(ASF)/DRIVERS/CPU/CYCLE_COUNTER -I$(ASF)/DRIVERS/WDT \
               -I$(ASF)/DRIVERS/PM -I$(ASF)/DRIVERS/FLASHC -I$(ASF)/DRIVERS/SPI

CFLAGS_BOARD = $(CFLAGS) -DBOARD=USER_BOARD -DDEBUG -DUSE_UARTDEBUG=1 $(ASF_INCS)

MAIN_OBJ      = $(OBJDIR)/main.o
UARTDEBUG_OBJ = $(OBJDIR)/uartdebug.o
ASF_OBJS      = $(OBJDIR)/uart0.o $(OBJDIR)/gpio.o $(OBJDIR)/usart.o \
                $(OBJDIR)/intc.o $(OBJDIR)/exception.o \
                $(OBJDIR)/spi_flash.o $(OBJDIR)/spi.o
EXC_STUB_OBJ  = $(OBJDIR)/exception_stub.o
CRT0_OBJ      = $(OBJDIR)/crt0.o

INIT_STARTUP_OBJS = $(OBJDIR)/lowleveinit.o $(OBJDIR)/pm.o \
                     $(OBJDIR)/wdt.o $(OBJDIR)/flashc.o

# swaptest_final: nove moduly se linkuji az za puvodni objekty, aby se nezmenilo
# umisteni ref* (0x80000060..0x800001DF, hned za crt0)
FINAL_OBJS  = $(OBJDIR)/rawuart.o $(OBJDIR)/idblock.o

DATA_TABLES = A B C D E F G H I J K L M N O P Q R S T U V W X
DATA_OBJS   = $(foreach t,$(DATA_TABLES),$(OBJDIR)/data$(t).o)

OBJS        = $(CRT0_OBJ) $(DATA_OBJS) $(OBJDIR)/check.o $(OBJDIR)/ref.o $(MAIN_OBJ) \
              $(UARTDEBUG_OBJ) $(ASF_OBJS) $(EXC_STUB_OBJ) $(INIT_STARTUP_OBJS) $(FINAL_OBJS)
MAPFILE     = $(BINDIR)/test_uc3a0512.map
ELF_TARGET  = $(BINDIR)/test_uc3a0512.elf
BIN_TARGET  = $(BINDIR)/test_uc3a0512.bin

LDFLAGS = -mpart=$(PART) -nostartfiles -Wl,--gc-sections -Wl,-Map=$(MAPFILE),--cref -Wl,-T,$(SRCDIR)/placement.ld


.PHONY: clean size all

.DEFAULT_GOAL := all

$(OBJDIR) $(BINDIR) \
$(DATA_OBJS) $(OBJDIR)/check.o $(OBJDIR)/util.o \
$(OBJDIR)/main.o $(OBJDIR)/uartdebug.o $(OBJDIR)/uart0.o \
$(OBJDIR)/gpio.o $(OBJDIR)/usart.o $(OBJDIR)/intc.o \
$(OBJDIR)/exception.o $(OBJDIR)/exception_stub.o $(OBJDIR)/lowleveinit.o \
$(OBJDIR)/spi_flash.o $(OBJDIR)/spi.o \
$(OBJDIR)/pm.o $(OBJDIR)/wdt.o $(OBJDIR)/flashc.o $(OBJDIR)/crt0.o \
$(OBJDIR)/rawuart.o $(OBJDIR)/idblock.o \
$(ELF_TARGET) $(BIN_TARGET) all size: SHELL := $(COMSPEC)

$(OBJDIR) $(BINDIR) $(BUILDDIR):
	mkdir $@

$(OBJDIR)/data%.o: $(SRCDIR)/data%.c $(SRCDIR)/data_table.h | $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/check.o: $(SRCDIR)/check.S | $(OBJDIR)
	$(CC) -mpart=$(PART) -c -o $@ $<

$(OBJDIR)/ref.o: $(SRCDIR)/ref.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

size: $(ELF_TARGET)
	$(SIZE) $(ELF_TARGET)

$(OBJDIR)/main.o: $(SRCDIR)/main.c $(SRCDIR)/boot.h $(SRCDIR)/idblock.h $(SRCDIR)/fw_id.h | $(OBJDIR)
	$(CC) $(CFLAGS_BOARD) -c -o $@ $<

$(OBJDIR)/uartdebug.o: $(SRCDIR)/uartdebug.c | $(OBJDIR)
	$(CC) $(CFLAGS_BOARD) -c -o $@ $<

$(OBJDIR)/uart0.o: $(SRCDIR)/uart0.c | $(OBJDIR)
	$(CC) $(CFLAGS_BOARD) -c -o $@ $<

$(OBJDIR)/gpio.o: $(ASF)/DRIVERS/GPIO/gpio.c | $(OBJDIR)
	$(CC) $(CFLAGS_BOARD) -c -o $@ $<

$(OBJDIR)/usart.o: $(ASF)/DRIVERS/USART/usart.c | $(OBJDIR)
	$(CC) $(CFLAGS_BOARD) -c -o $@ $<

$(OBJDIR)/intc.o: $(ASF)/DRIVERS/INTC/intc.c | $(OBJDIR)
	$(CC) $(CFLAGS_BOARD) -c -o $@ $<

$(OBJDIR)/spi_flash.o: $(SRCDIR)/spi_flash.c $(SRCDIR)/spi_flash.h | $(OBJDIR)
	$(CC) $(CFLAGS_BOARD) -c -o $@ $<

$(OBJDIR)/spi.o: $(ASF)/DRIVERS/SPI/spi.c | $(OBJDIR)
	$(CC) $(CFLAGS_BOARD) -c -o $@ $<

$(OBJDIR)/exception.o: $(ASF)/DRIVERS/INTC/exception.S | $(OBJDIR)
	$(CC) -mpart=$(PART) -I$(ASF)/DRIVERS/INTC -c -o $@ $<

$(OBJDIR)/exception_stub.o: $(SRCDIR)/exception_stub.c $(SRCDIR)/rawuart.h $(SRCDIR)/user_board.h | $(OBJDIR)
	$(CC) $(CFLAGS_BOARD) -c -o $@ $<

$(OBJDIR)/lowleveinit.o: $(SRCDIR)/lowleveinit.c $(SRCDIR)/boot.h $(SRCDIR)/rawuart.h $(SRCDIR)/user_board.h | $(OBJDIR)
	$(CC) $(CFLAGS_BOARD) -c -o $@ $<

$(OBJDIR)/rawuart.o: $(SRCDIR)/rawuart.c $(SRCDIR)/rawuart.h | $(OBJDIR)
	$(CC) $(CFLAGS_BOARD) -c -o $@ $<

$(OBJDIR)/idblock.o: $(SRCDIR)/idblock.c $(SRCDIR)/idblock.h $(SRCDIR)/boot.h $(SRCDIR)/fw_id.h $(SRCDIR)/user_board.h | $(OBJDIR)
	$(CC) $(CFLAGS_BOARD) -c -o $@ $<

$(OBJDIR)/pm.o: $(ASF)/DRIVERS/PM/pm.c | $(OBJDIR)
	$(CC) $(CFLAGS_BOARD) -c -o $@ $<

$(OBJDIR)/wdt.o: $(ASF)/DRIVERS/WDT/wdt.c | $(OBJDIR)
	$(CC) $(CFLAGS_BOARD) -c -o $@ $<

$(OBJDIR)/flashc.o: $(ASF)/DRIVERS/FLASHC/flashc.c | $(OBJDIR)
	$(CC) $(CFLAGS_BOARD) -c -o $@ $<

$(OBJDIR)/crt0.o: $(SRCDIR)/crt0.S | $(OBJDIR)
	$(CC) -mpart=$(PART) -I$(SRCDIR) -c -o $@ $<

$(ELF_TARGET): $(OBJS) | $(BINDIR)
	$(CC) $(LDFLAGS) -o $(ELF_TARGET) $(OBJS)

$(BIN_TARGET): $(ELF_TARGET) | $(BINDIR)
	$(OBJCOPY) -O binary $< $@

all: $(ELF_TARGET) $(BIN_TARGET)

clean: SHELL := $(COMSPEC)
clean:
	-if exist $(OBJDIR) rmdir /s /q $(OBJDIR)
	-if exist $(BINDIR) rmdir /s /q $(BINDIR)
	-if exist $(BUILDDIR)\ECB_ENC.asm del /q $(BUILDDIR)\ECB_ENC.asm
	-if exist $(BUILDDIR)\mh_ECB_ENC.asm del /q $(BUILDDIR)\mh_ECB_ENC.asm
	-if exist $(BUILDDIR)\rijndael_ecb_encrypt_ours.instr del /q $(BUILDDIR)\rijndael_ecb_encrypt_ours.instr
	-if exist $(BUILDDIR)\rijndael_ecb_encrypt_real.instr del /q $(BUILDDIR)\rijndael_ecb_encrypt_real.instr
