@echo off
set PATH=c:\jtagice3\avrdbg;%PATH%
atprogram -t jtagice3 -i jtag -d at32uc3a0512 chiperase
atprogram -t jtagice3 -i jtag -d at32uc3a0512 program -c --format bin -o 0x80000000 -f bin/test_uc3a0512.bin
