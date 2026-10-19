# Diagnostic firmware for the AT32UC3A0512 swap test

Firmware for the AT32UC3A0512 CPU (silicon revision L) on a board with an 8 MHz crystal. After power-on it performs 100 cycles of 480 check reads from the internal flash, compares every read word with the constant `0xFFFFFFFF` and prints the result on the UART. The result of a run is determined from the recorded output: PASS (0 errors) or FAIL.

| Item | Value |
|---|---|
| Version | `swaptest_final` version 2 (`FW_VER` in `src/fw_id.h`) |
| ELF | `bin/test_uc3a0512.elf`, SHA-256 `6675E8E3E4B4FF880ADD0A8B817B13226083ECCD2A64DC31F07DB1B353E08909` |
| CRC32 of the image (zlib) | `0xfb47ddb9`; the firmware computes it at run time and prints it |
| Compiler | WinAVR 20100110 (`avr32-gcc` 4.3.2), GNU Make 3.81 |
| Reproducibility | two independent builds are bit-identical (section 7.1) |
| Verification on hardware | version 2: regression run on PCB1 and PCB2 in the folders `REG_PCB1-X` and `REG_PCB2-Y` (section 9) |

Sections: 1 measurement principle · 2 failing values and run time · 3 identification block · 4 run result · 5 clock start-up and exceptions · 6 errata · 7 build and verification · 8 files and programming · 9 check on hardware.

---

## 1. Measurement principle and evaluation

### 1.1 Test core
The test core consists of 24 tables A–X at fixed flash addresses `0x8000A000–0x8007FC01`, placed by the linker script `src/placement.ld`. The table anchors are spaced `0x5000` apart from `0x8000A000` (A) to `0x8007D000` (X).

| Element (table X, anchor `K`) | Address | Content |
|---|---|---|
| `data_padding_X` | `K` | 0x200 B of padding |
| `dataX` | `K + 0x200` | 128 words; `dataX[0] = 0x00000000`, `dataX[0x77] = 0xFFFFFFFF` |
| `checkX0` … `checkX19` | `K + 0x5BC + n·0x200` | 20 check functions of 70 B, bit-identical within a table |
| `refX` | `0x80000060 + k·0x10` (outside the core) | reference function, 16 B |

- **Check function `checkXn`** reads `dataX[0]` and `dataX[0x77]` and returns `dataX[0] ^ dataX[0x77]`. It reads the word `dataX[0x77]` through an address computed at run time (instructions `bfextu`, `add`, `ld.w`). The critical read is the instruction `ld.w r10,r8[512]` at `checkXn + 0x30`. The sequence comes from the AES code in which the flash read failure appeared.
- **Reference function `refX`** returns the same value but reads `dataX[0x77]` at a fixed offset from the table address (`ld.w r12,r8[476]` at `refX + 0x04`, 476 = 4 · 0x77).
- The correct value of both is `0x00000000 ^ 0xFFFFFFFF = 0xFFFFFFFF` (`EXPECTED_VALUE` in `src/main.c`).

### 1.2 Measurement and evaluation (`src/main.c`)
- **One cycle:** `run_measurement()` calls `ref` 24 times and then `check` 480 times through tables of pointers and stores the results in an array. The measurement itself takes less than 1 ms.
- **Evaluation only after the measurement** (`evaluate_cycle()`, `noinline`):
  - every `ref_val[t]` and `check_val[t][c]` is compared with `EXPECTED_VALUE`;
  - failing `check` reads (`iter_errors`) and `ref` reads (`iter_ref_errors`) are counted separately, for the whole run as `errors` and `ref_errors`;
  - a cycle fails (`!!! ERROR !!!`) if it has at least one `check` **or** `ref` error;
  - the number of mismatches `check ≠ ref` is printed for information only as `vs_ref`. The comparison with the constant is decisive: it distinguishes a failure of the reference from a failure of the check read.
- **Output after every cycle:** a matrix A–X × 20 columns and a `ref:` line (24 symbols, tables A..X):
  - `*` = the value in this cycle ≠ `0xFFFFFFFF`;
  - `.` = correct now, wrong in some earlier cycle;
  - space = never failed.
- The `Summary iter=` line contains `errors=` (failing `check`), `ref_errors=` and `vs_ref=`.

```
!!! ERROR !!! on iteration=1 (total failed iterations=1)
A: * * * * * * * * * * * * * * * * * * * *
...
X: * * * * * * * * * * * * * * * * * * * *
ref:
Summary iter=1 tests=480 errors=445 ref_errors=0 vs_ref=445
```
An empty `ref:` line and `ref_errors=0` mean that the references were read correctly and the check reads fail. (Format example.)

---

## 2. Failing values and run time

### 2.1 Detail of the 1st cycle
Printed only in the 1st cycle and only if it contains an error, right after the `Summary iter=1` line:
```
Detail iter=1 wrong=<n> (checks <n>, refs <n>): ERR <table> <cNN|ref> ld=<instr> adr=<data> val=<value>
ERR A c00 ld=8000a5ec adr=8000a3dc val=5f3fffff
...
```

| Field | Meaning |
|---|---|
| `<table>` | A..X |
| `cNN` / `ref` | column 00..19 (`checkXnn`), or the reference `refX` |
| `ld=` | address of the critical instruction: `checkXn + 0x30`, or `refX + 0x04` |
| `adr=` | address of the read word `&dataX[0x77]` (anchor + 0x3DC) |
| `val=` | returned value `dataX[0] ^ dataX[0x77]`. These are two reads; a failing bit may come from either of them. Expected value `ffffffff`. |

Values are hexadecimal with a fixed width, without `0x`.

### 2.2 Per-cell summary for the whole run
For each of the 504 cells (480 `check` + 24 `ref`) the firmware keeps the number of failing cycles, the first failing value and the OR of the failing bits (`value ^ 0xFFFFFFFF`). At the end of the run only cells with errors are printed:
```
Cells with errors: checks <n>/480, refs <n>/24: CELL <table> <cNN|ref> n=<cycles> first=<value> bits=<OR of value^ffffffff>
CELL A c00 n=100 first=5f3fffff bits=a0c00000
...
```
RAM: 504 × 12 B = 6,048 B; `.bss` ends at `0x30C7`, 4 KB stack at `0xF000–0xFFFF`.

### 2.3 Run time (N = 100)
- The output is sent on both USART0 and USART1 at 115,200 Bd (86.9 µs per character).
- **Cycle length** is ≈ 1.118 s: a pause of 1,000 ms plus the output of the matrix, the `ref:` line and `Summary`.
- **Detail of the 1st cycle** on a failing unit extends the 1st cycle by ≈ 8 ms per failing cell, at most by ≈ 4.3 s. It is printed after the measurement and therefore does not affect it.
- **Per-cell summary** at the end takes ≈ 0.3 s without errors, at most ≈ 4.4 s.
- Start-up (C runtime on the RC oscillator, image CRC, fCPU measurement, ID block) takes ≈ 0.7 s.

| Profile | Run time (START → RESULT) |
|---|---|
| no errors | ≈ 113 s (regression run PCB2 + CPU-Y: 112.8 s) |
| failing, ≈ 450 failing cells | ≈ 120 s (regression run PCB1 + CPU-X: 120.3 s) |
| worst case: all 504 cells | ≈ 121 s |

**Timeout for an INCOMPLETE run on the PC side:** `N · 1.12 s + 30 s` = **142 s** for N = 100.

---

## 3. Identification block

### 3.1 Output
The block is printed at the start (START) and at the end (FINISH) of the run, after the lines `Chip ID / Chip SN / Board ID`. `<…>` are the values of the particular chip or measurement.
```
=== START ===
Chip ID  = 0xb1edc03f (rev L, AT32UC3A0512, Atmel)
Chip SN  = <30 hex>
Board ID = <16 hex>
ID fw=swaptest_final ver=2 gcc=4.3.2 image_start=0x80000000 image_len=523268 image_crc32=0xfb47ddb9
ID did=0xb1edc03f rn=11(L) pn=0x1edc mid=0x01f
ID chip_sn=<30 hex>
ID board_id=<16 hex> jedec=<6 hex>
ID gpf_lo=0xffffffff gpf_hi=0xffffffff lock=0xffff epfl=1 bootprot=7 bodlevel=0x3f bodhyst=1 boden=3(off)
ID fsr=0x0000a001 security=0 fsz=5 locked=0x0000
ID fcr=0x00000040 fws=1 bit8=0
ID clk mcctrl=0x00000006 pll0=0x1007000d cksel=0x00800000 oscctrl0=0x00000307
ID clk fosc0_nom=8000000 fcpu=64000000 fhsb=64000000 fpba=32000000 fpbb=64000000 fcpu_meas=<Hz>
ID meas cycles=<n> rtc_ticks=5760 f_rtc=f_rc/2 f_rc_nom=115200 rtc_div_check=<≈2.0>
ID pwr rcause=<8 hex>(<bit names>) bod=0x0001037f
ID boot osc0_startup=2048rc osc0rdy_wait=<n> lock0_wait=<n> ckrdy_wait=<n> timeouts=0
ID plan cycles=100 checks_per_cycle=480 refs_per_cycle=24 expected=0xffffffff delay_ms=1000 ref_row=A..X
```

### 3.2 Sources of the items
| Line | Source | Note |
|---|---|---|
| `fw` | `src/fw_id.h` (`FW_NAME`, `FW_VER`), `__VERSION__` | Fixed strings without `__DATE__`/`__TIME__`; the build is reproducible. `FW_VER` is increased with every code change. |
| `image_crc32` | CRC32 (zlib) of flash `0x80000000 + 523268 B` computed at run time (≈ 0.15 s) | Equals `zlib.crc32(bin/test_uc3a0512.bin)` = `0xfb47ddb9`. Computed at START and FINISH; reads the flash sequentially, without the `bfextu/add` pattern. |
| `did` | `mfdr AVR32_DID` | revision (RN), part number (PN), manufacturer (MID) |
| `chip_sn` | factory page `0x80800204–0x80800212` | serial number of the chip, read only |
| `board_id`, `jedec` | SPI flash on the board: JEDEC ID (9Fh) + UID (88h / 4Bh) | `spi_flash_read_ids()`; for an unsupported SPI flash `board_id=not_supported` |
| `gpf_*` | `FLASHC.FGPFRLO/HI` | Decoded according to `AT32UC3A0512.atdf` from UC3A_DFP 1.0.53 (Atmel Studio 7): LOCK 15:0, EPFL 16, BOOTPROT 19:17, BODLEVEL 25:20, BODHYST 26, BODEN 28:27 (0 and 3 = disabled, 1 = enabled with reset, 2 = enabled without reset) |
| `fsr`, `fcr` | `FLASHC.FSR`, `FLASHC.FCR` | `security`, `fsz`, `locked` (FSR 31:16), `fws`, `bit8` |
| `clk` | `PM.MCCTRL/PLL0/CKSEL/OSCCTRL0` read at run time | `oscctrl0 = 0x307`: crystal start-up time 2048 RC cycles |
| `fcpu_meas` | number of `COUNT` cycles in 5,760 RTC ticks from the RC oscillator (RTC = f_rc/2, 100 ms window) | The accuracy is set by the RC oscillator (nominal 115.2 kHz, tolerance of a few percent): it distinguishes 64 MHz from 48/80 MHz (crystal 6/10 MHz), not ±1 %. fCPU is documented more precisely by the cycle length measured with the PC clock (section 2.3). `rtc_div_check` checks the RTC divider while running on RC and should be ≈ 2.0. The RTC is restored to its original state after the measurement. |
| `pwr` | `PM.RCAUSE` (at start-up) + bit names, `PM.BOD` | reset cause: `0x1` = power-on (POR) |
| `boot` | start-up measurement (section 5) | `timeouts` = number of timeout reports |
| `plan` | constants in `src/main.c` | run length declared in advance |

---

## 4. Run length, summary and RESULT line

- `ITERATIONS = 100`, pause between cycles `cpu_delay_ms(1000)`. All 100 cycles are always run; the run does not stop at the first error. FINISH is followed by an endless loop without output.
- Order at the end: `=== FINISH ===`, `Chip ID/SN/Board ID`, ID block again, `CELL …`, `Summary`, `Result`, `RESULT`.

```
Summary  = cycles:100/100, checks/cycle:480, checks:48000, errors:<n>, failed cycles:<n>, first error cycle:<n>, min err/cycle:<n>, max err/cycle:<n>, ref reads:2400, ref errors:<n>, vs ref:<n>
Result   = !!! FAIL !!! (<n> errors in 48000 checks, <n> ref errors in 2400 ref reads)
RESULT;v=1;fw=swaptest_final-2;fw_crc=fb47ddb9;fw_crc_end=fb47ddb9;sn=<30 hex>;did=b1edc03f;board=<16 hex>;gpf=ffffffff;fcr=00000040;rcause=<8 hex>;boot_tmo=0;N=100;cycles=100;checks=48000;errors=<n>;failcyc=<n>;first=<n>;min=<n>;max=<n>;ref=<n>;verdict=FAIL;crc=<8 hex>
```
Without errors: `Result   = *** PASS *** (0 errors in 48000 checks, 0 ref errors in 2400 ref reads)` and in `RESULT` `errors=0;failcyc=0;first=0;min=0;max=0;ref=0;verdict=PASS`.

| Field | Meaning |
|---|---|
| `errors`, `min`, `max` | failing `check` reads in total, or min./max. per cycle |
| `ref` | failing `ref` reads in total (max. 2,400) |
| `failcyc` | cycles with ≥ 1 `check` or `ref` error |
| `first` | first failing cycle, `0` = none |
| `fw_crc` / `fw_crc_end` | image CRC32 from START / FINISH |
| `boot_tmo` | number of timeout reports during start-up |
| `crc` | CRC32 (zlib) of the whole text of the line before `;crc=` |

The line is longer than the `__debug` buffer (256 B), so `main.c` assembles it in its own buffer (384 B) and sends it in parts.

**Verdict:**
- firmware: `FAIL` = `errors + ref ≥ 1`, `PASS` = `cycles == N` and 0 errors;
- PC (`tools/evaluate.py`): INCOMPLETE = no `RESULT` within 142 s of START. Validity of a run: CPU SN and Board ID in START, FINISH and `RESULT` match the expected pair, image CRC32 = `fb47ddb9` at START and FINISH and a valid `crc` of the `RESULT` line.

Check of the line on the PC: `zlib.crc32(line.rsplit(";crc=", 1)[0].encode())` = `crc`. A mismatch of the image CRC between START and FINISH would indicate inconsistent flash reads; the firmware does not include it in the verdict, it is evaluated on the PC.

---

## 5. Clock start-up and exceptions

### 5.1 Start-up sequence
The clocks are set by `boot_clock_init()` (`src/lowleveinit.c`), called from `main()`. Early in the start-up the firmware can therefore write to the UART and store results in RAM; the C runtime initialisation (`.data`/`.bss`, ≈ 12 kB) thus still runs on the RC oscillator.

| # | Clock | Step | Output | Time from reset (estimate) |
|---|---|---|---|---|
| 0 | RC 115 kHz | crt0: SP, EVBA, exceptions; `_init_startup`: WDT off, INTC; `.data`, `.bss` | – | 0 → ≈ 70 ms |
| 1 | RC | UART0+1 at **14,400 Bd** (no faster rate is possible from RC) | `BOOT rc rcause=0x……` | ≈ 70–90 ms |
| 2 | RC | check of the RTC divider (1,024 ticks) | – | +18 ms |
| 3 | RC | OSC0 crystal 8 MHz (G3), **start-up time 2048 RC cycles (≈ 18 ms; doc32058K Table 13-7, the value used by Atmel ASF for the EVK1100)**, wait for OSC0RDY with a timeout | on timeout `BOOT TIMEOUT OSC0RDY …` | +18 ms |
| 4 | **switch to OSC0** | UART at 115,200 Bd from 8 MHz (114,286 Bd, −0.8 %) | `BOOT osc0 osc0rdy_wait=<n> rc_cycles` | ≈ 130 ms |
| 5 | OSC0 | PLL0 = 8 × OSC0 (VCO 128 MHz, /2), wait for LOCK0 with a timeout | on timeout `BOOT TIMEOUT LOCK0 …` | +≈ 0.15 ms |
| 6 | OSC0 | CKSEL = `0x00800000` (PBA = main/2), wait for CKRDY with a timeout | on timeout `BOOT TIMEOUT CKRDY …` | – |
| 7 | OSC0 | `FCR.FWS = 1` (the only write to FLASHC) | – | – |
| 8 | **PLL0 64 MHz** | UART for PBA 32 MHz | `=== START ===` | ≈ 0.13–0.16 s |
| 9 | PLL0 | ID block: image CRC ≈ 0.15 s, fCPU 0.1 s, output ≈ 0.2 s | `ID …` | 1st cycle ≈ 0.6–0.7 s |

**What a terminal at 115,200 Bd shows:**
- The `BOOT rc` marker (14,400 Bd) appears as a short sequence of non-ASCII bytes, mostly `0x00` and `0x80`; the converter may report it as a *break*. It shows that the CPU runs on RC. It is readable on a terminal set to 14,400 Bd.
- From step 4 on, everything is readable at 115,200 Bd.
- **Locating a start-up that did not complete:**
  - no bytes = the CPU printed nothing, not even on RC;
  - only a sequence of bytes without `BOOT osc0` = stuck during or after the switch to the crystal;
  - `BOOT osc0` without START = PLL/CKSEL/switch to PLL0;
  - `BOOT TIMEOUT …` = the particular wait loop.
- **Restart over JTAG:** when a JTAG session ends, the programmer briefly releases the CPU and resets it again; the `BOOT rc` marker may then appear twice and only the second start reaches START.

**Timeouts:**
- OSC0RDY 16,384 RC cycles (≈ 142 ms, 8× the start-up time), LOCK0 80,000 cycles at 8 MHz (10 ms), CKRDY 8,000 cycles (1 ms).
- When a timeout expires, a report is printed and the firmware **keeps waiting**; the report is repeated at twice the interval. When the flag is finally set, `BOOT <flag> ok after <n> cycles` is printed. The number of reports is in `ID boot … timeouts=` and `RESULT … boot_tmo=`.
- The RTC measurement has timeouts (on failure `fcpu_meas=0`); the output in `rawuart` has a timeout on TXRDY/TXEMPTY.

**What the start-up does not cover:**
- If the CPU stops after switching to OSC0 (the crystal does not oscillate after 18 ms), no software timeout catches it; it shows as a missing `BOOT osc0`.
- The firmware does not use the watchdog.
- `LOCK0` is not taken as proof that the crystal oscillates (it was not verified whether it indicates PLL lock or only the expiry of `PLLCOUNT`).
- The wait loops in the drivers `spi_flash.c` (SPI) and `uart0.c` (TXRDY) have no timeout; they run only on the PLL and a hang would show as truncated output.

### 5.2 Exception handler
- `exception.S` (`isr_exception`) passes to C the vector number, the SP on entry (where the CPU stored SR at `[SP]` and PC at `[SP+4]` according to the AVR32A architecture) and LR. It then sets up a new stack from `_estack`; the handler does not return.
- `exception_stub.c` derives the current clock from `MCCTRL/CKSEL`, sets up the UART (RC → 14,400 Bd, OSC0/PLL0 → 115,200 Bd) and prints every ≈ 2 s:
```
!!! EXCEPTION !!! #1 vec=3(BUS_ERROR_DATA) ecr=0x<8 hex> pc=0x<8 hex> sr=0x<8 hex> lr=0x<8 hex> sp=0x<8 hex> bear=0x<8 hex> mcsel=2
```
- `vec` is the vector number and name (1–22), `ecr` is `ECR`, `pc`/`sr` are the values stored by the CPU, `bear` is `BEAR` (address of a bus error). If SP is not a valid RAM address, `pc=? sr=?` is printed.
- The handler does not use `printf`, `.data` or `.bss`; it also works for an exception during start-up.

---

## 6. Errata doc32058K for revision L
Datasheet doc32058K (revision K, 01/12), section 41.1 "Rev. K, L, M":
- The only flash-related item is FLASHC 1: reading from the on-chip flash may fail after a flash fuse write operation (FLASHC commands LP, UP, WGPB, EGPB, SSB, PGPFB, EAGPF). The firmware issues none of these commands and the runs start at power-on.
- The errata state nothing about prefetch or flash wait states.
- Power Manager 1 (the part is constantly under reset if the BOD level is higher than VDDCORE) does not apply: the BOD is disabled (`boden=3`).

Crystal start-up: the datasheet gives a typical start-up time of 4 ms for an 8 MHz crystal (Table 38-15); the firmware waits 2048 RC cycles ≈ 18 ms (`OSCCTRL0.STARTUP = 3`, Table 13-7).

---

## 7. Build and verification

### 7.1 Build and reproducibility
In the folder `20261010_swaptest_final`, from the `cmd` command line:
```
make COMSPEC=%ComSpec% clean all
```
The Makefile runs the target commands through `$(COMSPEC)`; Windows passes the variable as `ComSpec`, so it is given on the command line, otherwise the `clean` target deletes nothing. The output is in `bin/` (`.bin`, `.elf`, `.map`), intermediate files in `obj/` (not in the repository).

| Build | Folder | `.bin` SHA-256 | `.elf` SHA-256 |
|---|---|---|---|
| 1 | `20261010_swaptest_final` | `2ECFE5317649FABD27797608F3F9924AAAC45B4DAA870A7546195E9975294440` | `6675E8E3E4B4FF880ADD0A8B817B13226083ECCD2A64DC31F07DB1B353E08909` |
| 2 | copy of `Makefile` + `src` in another folder | identical | identical |

- Both builds are **bit-identical**, including `.map` and all 43 `obj/*.o`; rc = 0, 0 warnings.
- Records: `build_check/build1.log`, `build2.log` (make output), `build_check/sha256.txt` (hashes).

### 7.2 Build check (`verify_build.py`)
```
python -I -B verify_build.py bin
```
The script only reads `bin/test_uc3a0512.bin/.elf` and `src/placement.ld` and uses `avr32-nm` and `avr32-objdump` from WinAVR. The outputs for both builds are in `build_check/verify_build1.txt` and `verify_build2.txt`, both **"RESULT: all OK"**.

| Check | Result |
|---|---|
| length of `.bin` = `IMAGE_LEN` | 523,268 B = 0x7FC04 (the firmware computes the CRC32 over it) |
| placement of the core according to `placement.ld` | 24 table anchors, `data_padding_X` at the anchor, `dataX` = anchor + 0x200; 480 `checkXn` functions at the addresses from `placement.ld` |
| table content in the image | in all 24 tables `data[0] = 0x00000000`, `data[0x77] = 0xFFFFFFFF` |
| functions | the 20 `checkXn` functions of each table are bit-identical; `refA..refX` at `0x80000060 + k·0x10`, 16 B |
| critical reads (`ld=` in the output) | `refX+0x04 = ld.w r12,r8[476]`, `checkXn+0x30 = ld.w r10,r8[512]` (504 functions) |
| layout below the core | `.text` `0x80000008–0x800084CF`, `.exception` `0x80008600`, `.rodata` up to `0x80009937`, `.data` LMA up to `0x80009E53`, reserve up to `0x8000A000` 428 B |
| writes to FLASHC | the only write is FCR in `flashc_set_wait_state`; FSR, FGPFRHI/LO and FCR are only read in `id_collect`; nothing refers to FCMD (`0xFFFE1404`); of `flashc.c` only `flashc_set_wait_state` is linked |
| image CRC32 | `0xfb47ddb9`, `__VERSION__` 4.3.2 |

### 7.3 Measurement loop
`run_measurement()` is inlined into `main()` (addresses according to `verify_build.py`, section 10 of its output):
- loop of 24× `ref`: 5 instructions (`ld.w icall st.w cp.w brne`), `0x80000DA0–0x80000DA8`;
- inner loop of 20× `check`: 17 instructions, `0x80000DC0–0x80000DE6`;
- outer loop over 24 tables: 11 instructions, `0x80000DE8–0x80000E02`.

Between the calls the results are only stored in RAM; evaluation and output follow after the whole measurement of the cycle.

---

## 8. Files and programming the CPU

| File | Content |
|---|---|
| `src/check.S` | 480 check functions `checkXn` |
| `src/ref.c` | 24 reference functions `refX` |
| `src/data[A-X].c`, `src/data_table.h` | 24 data tables and their padding |
| `src/placement.ld` | fixed placement of the tables and `check` functions (test core) |
| `src/crt0.S` | start-up code (reset vector, stack, `.data`/`.bss`) |
| `src/main.c` | measurement, evaluation, cycle output, detail, per-cell summary, `RESULT` (sections 1, 2, 4) |
| `src/lowleveinit.c` | `boot_clock_init()` (clock start-up with markers and timeouts), `rtc_measure_cycles()` (section 5) |
| `src/idblock.c/.h` | identification block, image CRC32, decoding of RCAUSE and fuses (section 3) |
| `src/fw_id.h` | `FW_NAME`, `FW_VER` |
| `src/boot.h` | `boot_info_t`, start-up prototypes |
| `src/rawuart.c/.h` | output without `printf`, `.data` and `.bss` on both USARTs with timeouts (start-up, exception) |
| `src/exception_stub.c`, `src/SOFTWARE_FRAMEWORK/DRIVERS/INTC/exception.S` | exception handler (section 5.2) |
| `src/spi_flash.c/.h` | SPI flash on the board: `Board ID`, JEDEC ID and UID |
| `src/uart0.c/.h`, `src/uartdebug.c`, `src/debug.h` | UART output at run time on the PLL |
| `src/user_board.h` | 8 MHz crystal (`FOSC0`), OSC0 start-up time 2048 RC (`OSC0_STARTUP`), bus frequencies |
| `src/SOFTWARE_FRAMEWORK/` | Atmel ASF drivers and headers (PM, FLASHC, GPIO, USART, SPI, INTC, WDT, TC, PDCA, cycle counter, utils) |
| `Makefile` | build (section 7.1) |
| `verify_build.py` | build check (section 7.2) |
| `build_check/` | logs of both builds, outputs of `verify_build.py`, `sha256.txt` |
| `bin/` | `test_uc3a0512.bin` (to be programmed), `.elf`, `.map` |
| `prog3_0512.bat` | older programming script; needs `atprogram` with UC3 device descriptions and does not work on the measurement PC |

**Programming the CPU and verification:** the scripts `tools/program.ps1` and `tools/readback.ps1` in the repository root (see `README.md`). They use `atprogram` from Atmel Studio 7 with a `TEMP` variable without diacritics (`C:\Windows\Temp`):
```
atprogram -t jtagice3 -i jtag -d at32uc3a0512 chiperase program --format bin -o 0x80000000 -f test_uc3a0512.bin
atprogram -t jtagice3 -i jtag -d at32uc3a0512 read -o 0x80000000 -s 0x80000 --format bin -f flash.bin
```
The first 523,268 B of `flash.bin` must be bit-identical with `bin/test_uc3a0512.bin` and the rest `0xFF`. The command `verify --format bin` fails in this version of `atprogram`, so the content is verified by readback. `chiperase` does not erase the user page.

**Writes by the firmware:** the firmware configures only the peripherals needed to run: PM (crystal, PLL0, dividers, clock switching), GPIO (USART and SPI pins), USART0/1 (different rates during start-up and in the exception handler), SPI (reading the SPI flash IDs), INTC, WDT (disabled), RTC (only during the fCPU measurement, then restored) and `FLASHC.FCR.FWS = 1`. It writes nothing to the flash, the user page or the fuses.

---

## 9. Check on hardware
The following is checked in the output of a run:
- `BOOT osc0` readable at 115,200 Bd;
- `image_crc32 = 0xfb47ddb9` at START and FINISH, `fw_crc = fw_crc_end = fb47ddb9` in `RESULT`;
- `chip_sn` and `board_id` identical at START, FINISH and in `RESULT`;
- `oscctrl0 = 0x307`, `cksel = 0x00800000`, `fcr = 0x00000040` (FWS = 1);
- `osc0rdy_wait` ≈ 2,000–2,100 RC cycles, `timeouts = 0`, no `!!! EXCEPTION !!!`;
- `fcpu_meas` within the RC oscillator tolerance around 64 MHz, `rtc_div_check` ≈ 2.0; cycle length ≈ 1.118 s by the PC clock;
- 100 cycles, a valid `crc` of the `RESULT` line and a run time according to section 2.3.

The validity and result of the runs are evaluated by `tools/evaluate.py`. Regression run of version 2 (3 runs after power-on with the JTAG disconnected on each board, 2026-10-15):

| Folder | Board + CPU | Result |
|---|---|---|
| `REG_PCB1-X` | PCB1 + CPU-X | FAIL, 3 of 3 valid runs (44,384–44,508 errors of 48,000, `ref` 0, `osc0rdy_wait` 2034, `timeouts` 0) |
| `REG_PCB2-Y` | PCB2 + CPU-Y | PASS, 3 of 3 valid runs (0 errors, `ref` 0, `osc0rdy_wait` 2034, `timeouts` 0) |
| `REG_PCB1-X_attempt1` | PCB1 + CPU-X | superseded attempt of 2026-10-14 (the board did not yet carry the laser label): FAIL, 3 of 3 valid runs (43,926–44,133 errors of 48,000) |

Version 2 (commit `ab71773`) differs from version 1 (commit `ecc3fec`) only in the removed `base=` field of the `ID fw` line and in the version number (`ID fw … ver=`, `RESULT … fw=`); the test core `0x8000A000–0x8007FC01` is bit-identical.

The measured `fcpu_meas` of 65.81–65.89 MHz (+2.8 to +2.9 %) is the deviation of the RC oscillator: the cycle length by the PC clock (1.118 s on PCB1, 1.115 s on PCB2) corresponds to 64 MHz; at 65.8 MHz the 1,000 ms pause would last only 972 ms.
