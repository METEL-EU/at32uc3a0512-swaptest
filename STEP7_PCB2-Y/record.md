# Configuration record: STEP7_PCB2-Y

Procedure according to the swap test protocol, section "Measurement procedure for one configuration".

| Item | Value |
| --- | --- |
| Step | STEP7 (step 7 of the swap test protocol, after resoldering 2, see `RESOLDER2`) |
| Board + CPU | PCB2 + CPU-Y |
| Expected CPU SN | ba16905e4450334d342e3120ff0714 |
| Expected Board ID | 8a3203841518175d |
| Binary | 20261010_swaptest_final/bin/test_uc3a0512.bin, firmware version 2, SHA-256 2ECFE5317649FABD27797608F3F9924AAAC45B4DAA870A7546195E9975294440 |
| Protocol | version 1.0, approved in commit fa7ec7e |
| Tools | tools/ at commit 348e514 (last change of tools/ in d5c8c05) |
| Date | 2026-11-16 |
| Performed by | Pavel Mikšíček |
| Checked by | Milan Holinka |

## Checklist

- [x] Programming, only where the procedure requires it: not required in step 7; the firmware moved with CPU-Y and is verified by the readback
- [x] JTAG connected, readback `tools/readback.ps1` → `readback.txt` with `RESULT OK` (SHA-256 of flash = binary) (11:15:26)
- [x] JTAG disconnected from the board
- [x] Photo of the board with readable CPU marking and board label, with a time stamp in the image, taken during the measurement of the configuration (between the readback and the end of the UART capture): `photo_20261116-114411.jpg` (11:44:11)
- [x] UART capture started `tools/capture.ps1` → `uart.raw`, `uart.log` (11:45:24, port COM9)
- [x] Three runs: power off, wait ≥ 10 s, power on, record the output until the RESULT line
- [x] VDDIN, VDDCORE (VDDIO is connected to VDDIN on the board) and ambient temperature measured during the 1st run (below); oscilloscope probes connected with the power off; screenshot `scope_pcb2_20261116-114712.png`, screen time 11:46 (first minute of run 1), file written at 11:47:12 (see Notes)
- [x] Capture stopped, evaluation `tools/evaluate.py` → `evaluate.txt`
- [x] Hashes `tools/sha256sums.ps1` → `SHA256SUMS`, commit and push

The SHA-256 of `SHA256SUMS` and the signatures follow the hashes and are therefore not part of this record. The SHA-256 of `SHA256SUMS` is entered in the table of records in the protocol; the signatures of the operator and the checker on the protocol cover that table.

## Measurements during the 1st run

| Quantity | Value | Limit |
| --- | --- | --- |
| VDDIO | same as VDDIN (connected on the board) | 3.0–3.6 V |
| VDDIN | 3.3101 V average, 440 mV peak-to-peak | 3.0–3.6 V |
| VDDCORE | 1.8131 V average, 200 mV peak-to-peak | 1.65–1.95 V |
| Ambient temperature | 23.8 °C | – |

Instrument: Keysight DSO-X 2024A (SN MY52160502), 10:1 probes, DC coupling, 1 V/div, 500 kSa/s, measurements Avg - FS and Pk-Pk; screenshot `scope_pcb2_20261116-114712.png`. At 1 V/div one step of the 8-bit converter is about 31 mV, so the peak-to-peak values are near the resolution limit of this setting and do not characterise the supply ripple. Thermometer: indicative instrument, manufacturer METEL.

## Runs

Power-on time = first `Break` event in `uart.log` (the `BOOT rc` marker is sent at 14 400 Bd).

| Run | Power-on (time) | Started within 5 s | Result (evaluate.py) | Note |
| --- | --- | --- | --- | --- |
| 1 | 11:46:00.204 | yes | PASS | 0 / 48 000 failing reads, 0/100 cycles failed, 112.8 s; supply voltages measured |
| 2 | 11:48:29.737 | yes | PASS | 0 / 48 000 failing reads, 0/100 cycles failed, 112.8 s |
| 3 | 11:51:21.735 | yes | PASS | 0 / 48 000 failing reads, 0/100 cycles failed, 112.8 s |

Total number of power-ons, including repeated attempts (INCOMPLETE, DID NOT START or invalid runs): 3

## Configuration result

PASS according to `evaluate.txt` (line `CONFIGURATION RESULT: PASS`): 3 valid runs of 3, all PASS. The output of every run contains the SN of CPU-Y and the Board ID of PCB2, so CPU-Y is mounted on PCB2 as intended by resoldering 2.

## Notes

- The photo was taken at 11:44:11 by the camera clock (Nikon D3400, time stamp in the image and in EXIF), after the readback at 11:15:26 and before the start of the UART capture at 11:45:24 by the PC clock. The laser label PCB2 is in the bottom right corner of the board.
- Oscilloscope: the acquisition was stopped during run 1. The time at the bottom right of the screen is the time of the screen content (11:46, minute resolution); the time in the header of the image is the time the file was written to the USB flash drive (11:47:12). The oscilloscope clock is synchronised with the PC clock by the operator. Run 1 started at 11:46:00.2 and ended at 11:47:53.0 by the PC clock, so the screen content is from the first minute of run 1; the file was written about 72 s after the start of run 1. The procedure requires the screenshot to be saved within the first minute of the run.
- VDDCORE is 1.81 V, as with CPU-Y in steps REG and 1 (on PCB2) and 6 (on PCB1), and different from CPU-X (1.77 V on either board).
- `uart.log` contains no error events; 8 `Break` events at power-on (three at the 1st and 3rd power-on, two at the 2nd). There is no `SpErr Overrun` event when the port was opened: the run started by the reset at the end of the readback (11:15:50) ended at about 11:17:43, before the capture started.
- The USB–RS-485 converter is enumerated as COM9 on the measurement PC; the capture was run with `-Port COM9` (header of `uart.log`).
- `readback.txt` shows `FCR=0x00000000`, the reset value read while the CPU was halted by the debugger. At run time the firmware sets `FCR=0x00000040` (FWS=1), see the `ID fcr` lines in `uart.log`.
- `info.txt` shows `Target voltage: 2.46 V`. The VTref pin of the JTAG connector is not connected on the board, so this value is not the target supply voltage; the supply voltages are given above.
- The capture was ended by stopping the capture process at 11:53:28 after the third run; `uart.log` therefore has no `# capture end` line.
