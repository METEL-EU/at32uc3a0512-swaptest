# AT32UC3A0512 CPU swap test

Measurement repository for the swap test of the AT32UC3A0512 CPU (silicon revision L) using the A-B-A method: diagnostic firmware, measurement tools and the records of each step. The procedure, criteria and evaluation are defined by the swap test protocol; its approved, filled-in version will be stored here.

## Structure

```
.
├── README.md                     this description
├── .gitignore, .gitattributes    files are stored byte-exact (* -text, no line-ending conversion)
├── 20261010_swaptest_final/      diagnostic firmware
│   ├── Makefile                  build: make COMSPEC=%ComSpec% clean all (WinAVR 20100110)
│   ├── src/                      sources; test core = check.S, ref.c, data*.c, placement.ld
│   ├── bin/                      test_uc3a0512.bin (to be programmed), .elf, .map
│   ├── build_check/              logs of two independent builds, verify_build.py outputs, sha256.txt
│   ├── verify_build.py           build check: test-core placement, critical reads, FLASHC writes
│   └── prog3_0512.bat            legacy programming script; needs atprogram with UC3 device files
├── tools/                        tools for measuring one configuration
│   ├── program.ps1               programming the binary over JTAG (chiperase + program), log in program.txt
│   ├── readback.ps1              readback over JTAG (info, flash, user page, FLASHC), comparison with the binary
│   ├── capture.ps1               passive UART capture (COM6, 115200 8N1) into uart.raw and uart.log
│   ├── evaluate.py               evaluation of uart.log: validity and result of runs, result of the configuration
│   ├── sha256sums.ps1            SHA256SUMS with the hashes of all files in a folder
│   └── record_template.md        template of the configuration record (checklist, voltages, runs)
└── <STEP>_<PCB>-<CPU>/           records of one configuration, e.g. REG_PCB1-X, STEP1_PCB2-Y
    ├── record.md                 filled-in template
    ├── program.txt               programming log (only where the binary was programmed in this configuration)
    ├── readback.txt              readback result (RESULT OK = flash matches the binary)
    ├── flash.bin, userpage.bin, flashc.bin, info.txt   raw readbacks
    ├── uart.raw, uart.log        raw UART output and the same output with receive time
    ├── evaluate.txt              output of evaluate.py
    ├── photo_*.jpg               photos of the board with the CPU
    └── SHA256SUMS                hashes of all files in the folder
```

Record folders: `REG` = regression run of the final firmware during preparation; `STEP1`, `STEP6`, `STEP7` = measurement steps of the protocol (numbering of the Microchip SWAP TEST article). The step is followed by the board and the CPU mounted in that configuration.

## Units under test

| Label | Identifier |
| --- | --- |
| PCB1 (originally failing) | Board ID `8a32128415213248` |
| CPU-X (originally on PCB1) | SN `141916cb4450334d342e3120ff060b` |
| PCB2 (originally passing) | Board ID `8a3203841518175d` |
| CPU-Y (originally on PCB2) | SN `ba16905e4450334d342e3120ff0714` |

The Board ID is the UID of the SPI flash on the board; the CPU SN is the serial number from the factory page of the chip (`0x80800204–0x80800212`). The firmware prints both in every run.

## Firmware

After power-on the firmware runs 100 cycles of 480 check reads from the internal flash, compares every read word with the constant `0xFFFFFFFF` and prints the result on USART0 and USART1 (115200 Bd, 8N1). The output contains an identification block (CPU SN, Board ID, fuses, FLASHC, clocks, reset cause, CRC32 of the flash image computed at run time), a matrix of failing reads per cycle, details of every failing read in the 1st cycle, a per-cell summary and a machine-readable `RESULT;…;crc=` line. A run takes about 113 s on a passing board and about 120 s on a failing one.

| File | SHA-256 |
| --- | --- |
| `20261010_swaptest_final/bin/test_uc3a0512.bin` | `2ECFE5317649FABD27797608F3F9924AAAC45B4DAA870A7546195E9975294440` |
| `20261010_swaptest_final/bin/test_uc3a0512.elf` | `6675E8E3E4B4FF880ADD0A8B817B13226083ECCD2A64DC31F07DB1B353E08909` |

Firmware version 2 (`ID fw=swaptest_final ver=2`, `RESULT … fw=swaptest_final-2`). CRC32 of the flash image computed and printed by the firmware: `0xfb47ddb9`. Two independent builds are bit-identical (`build_check/`).

## Measuring one configuration

Requirements: JTAGICE3 and `atprogram` from Atmel Studio 7, USB–UART converter on COM6, Windows PowerShell 5.1, Python 3. Example for `REG_PCB1-X`, run from the `cmd` command line in the repository root:

```
mkdir REG_PCB1-X
copy tools\record_template.md REG_PCB1-X\record.md
powershell -ExecutionPolicy Bypass -File tools\program.ps1 -OutDir REG_PCB1-X -Bin 20261010_swaptest_final\bin\test_uc3a0512.bin
powershell -ExecutionPolicy Bypass -File tools\readback.ps1 -OutDir REG_PCB1-X -Bin 20261010_swaptest_final\bin\test_uc3a0512.bin
(disconnect JTAG)
powershell -ExecutionPolicy Bypass -File tools\capture.ps1 -Out REG_PCB1-X\uart
(three runs: power off, wait >= 10 s, power on, wait for RESULT; then stop the capture)
python -I -B tools\evaluate.py REG_PCB1-X\uart.log --sn 141916cb4450334d342e3120ff060b --board 8a32128415213248 > REG_PCB1-X\evaluate.txt
powershell -ExecutionPolicy Bypass -File tools\sha256sums.ps1 -Dir REG_PCB1-X
git add REG_PCB1-X && git commit && git push
```

`program.ps1` is used only where the procedure requires programming; after a CPU swap the firmware moves with the chip and is only verified by readback.

## Integrity of the records

After every step the SHA-256 of the folder's `SHA256SUMS` file is entered in the table of records in the protocol. After the test the completed protocol, signed by the operator and the checker, is sent by e-mail to the recipient of the results; the signatures cover the table of records. The repository itself does not prevent rewriting of the history; the evidential value of the records rests on the hashes and the copy of the protocol held by the recipient. Raw records are never edited; a correction is stored as a new file with a note.
