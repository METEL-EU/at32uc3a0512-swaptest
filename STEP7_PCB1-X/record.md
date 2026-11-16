# Configuration record: STEP7_PCB1-X

Procedure according to the swap test protocol, section "Measurement procedure for one configuration".

| Item | Value |
| --- | --- |
| Step | STEP7 (step 7 of the swap test protocol, after resoldering 2, see `RESOLDER2`) |
| Board + CPU | PCB1 + CPU-X |
| Expected CPU SN | 141916cb4450334d342e3120ff060b |
| Expected Board ID | 8a32128415213248 |
| Binary | 20261010_swaptest_final/bin/test_uc3a0512.bin, firmware version 2, SHA-256 2ECFE5317649FABD27797608F3F9924AAAC45B4DAA870A7546195E9975294440 |
| Protocol | version 1.0, approved in commit fa7ec7e |
| Tools | tools/ at commit 4850241 (last change of tools/ in d5c8c05) |
| Date | 2026-11-16 |
| Performed by | Pavel Mikšíček |
| Checked by | Milan Holinka |

## Checklist

- [x] Programming, only where the procedure requires it: not required in step 7; the firmware moved with CPU-X and is verified by the readback
- [x] JTAG connected, readback `tools/readback.ps1` → `readback.txt` with `RESULT OK` (SHA-256 of flash = binary) (10:55:23)
- [x] JTAG disconnected from the board
- [x] Photo of the board with readable CPU marking and board label, with a time stamp in the image, taken during the measurement of the configuration (between the readback and the end of the UART capture): `photo_20261116-105732.jpg` (10:57:32)
- [x] UART capture started `tools/capture.ps1` → `uart.raw`, `uart.log` (10:58:15, port COM9)
- [x] Three runs: power off, wait ≥ 10 s, power on, record the output until the RESULT line
- [x] VDDIN, VDDCORE (VDDIO is connected to VDDIN on the board) and ambient temperature measured during the 1st run (below); oscilloscope probes connected with the power off; screenshot `scope_pcb1_20261116-110114.png` saved at 11:01:14, not within the first minute of the run (see Notes)
- [x] Capture stopped, evaluation `tools/evaluate.py` → `evaluate.txt`
- [x] Hashes `tools/sha256sums.ps1` → `SHA256SUMS`, commit and push

The SHA-256 of `SHA256SUMS` and the signatures follow the hashes and are therefore not part of this record. The SHA-256 of `SHA256SUMS` is entered in the table of records in the protocol; the signatures of the operator and the checker on the protocol cover that table.

## Measurements during the 1st run

| Quantity | Value | Limit |
| --- | --- | --- |
| VDDIO | same as VDDIN (connected on the board) | 3.0–3.6 V |
| VDDIN | 3.3114 V average, 360 mV peak-to-peak | 3.0–3.6 V |
| VDDCORE | 1.7738 V average, 200 mV peak-to-peak | 1.65–1.95 V |
| Ambient temperature | 23.6 °C | – |

Instrument: Keysight DSO-X 2024A (SN MY52160502), 10:1 probes, DC coupling, 1 V/div, 500 kSa/s, measurements Avg - FS and Pk-Pk; screenshot `scope_pcb1_20261116-110114.png`. At 1 V/div one step of the 8-bit converter is about 31 mV, so the peak-to-peak values are near the resolution limit of this setting and do not characterise the supply ripple. Thermometer: indicative instrument, manufacturer METEL.

## Runs

Power-on time = first `Break` event in `uart.log` (the `BOOT rc` marker is sent at 14 400 Bd).

| Run | Power-on (time) | Started within 5 s | Result (evaluate.py) | Note |
| --- | --- | --- | --- | --- |
| 1 | 10:59:04.483 | yes | FAIL | 45 937 / 48 000 failing reads, 100/100 cycles failed, 120.5 s; supply voltages measured |
| 2 | 11:01:43.554 | yes | FAIL | 45 499 / 48 000 failing reads, 100/100 cycles failed, 120.5 s |
| 3 | 11:04:24.060 | yes | FAIL | 45 441 / 48 000 failing reads, 100/100 cycles failed, 120.5 s |

Total number of power-ons, including repeated attempts (INCOMPLETE, DID NOT START or invalid runs): 3

## Configuration result

FAIL according to `evaluate.txt` (line `CONFIGURATION RESULT: FAIL`): 3 valid runs of 3, all FAIL. The output of every run contains the SN of CPU-X and the Board ID of PCB1, so CPU-X is mounted on PCB1 as intended by resoldering 2.

## Notes

- The photo was taken at 10:57:32 by the camera clock (Nikon D3400, time stamp in the image and in EXIF), after the readback at 10:55:23 and before the start of the UART capture at 10:58:15 by the PC clock. The laser label PCB1 is in the bottom right corner of the board.
- Oscilloscope: the acquisition was stopped during run 1 (the stopped screen shows the supply voltages of the powered board and the time 11:00 by the oscilloscope clock); the screenshot was saved at 11:01:14 by the oscilloscope clock, about 130 s after the start of run 1 at 10:59:04.5 by the PC clock. Run 1 ended at 11:01:05.1. The procedure requires the screenshot to be saved within the first minute of the run; this is a deviation from the procedure. The oscilloscope and PC clocks are not synchronised.
- VDDCORE is 1.77 V, as with CPU-X in steps REG and 1 (on PCB1) and 6 (on PCB2), and different from CPU-Y (1.81 V on either board).
- `uart.log` contains no error events; 9 `Break` events at power-on (three per power-on). Unlike the step 6 records, there is no `SpErr Overrun` event when the port was opened: the run started by the reset at the end of the readback (10:55:45) ended at about 10:57:46, before the capture started.
- Before the readback, the JTAG connection was checked with `atprogram info` and the UART reception on COM9 with a separate test capture outside this record (10:44); the reset at the end of that atprogram session started the firmware.
- The USB–RS-485 converter is enumerated as COM9 on the measurement PC; the capture was run with `-Port COM9` (header of `uart.log`).
- `readback.txt` shows `FCR=0x00000000`, the reset value read while the CPU was halted by the debugger. At run time the firmware sets `FCR=0x00000040` (FWS=1), see the `ID fcr` lines in `uart.log`.
- `info.txt` shows `Target voltage: 2.45 V`. The VTref pin of the JTAG connector is not connected on the board, so this value is not the target supply voltage; the supply voltages are given above.
- The capture was ended by stopping the capture process at 11:08:38 after the third run; `uart.log` therefore has no `# capture end` line.
