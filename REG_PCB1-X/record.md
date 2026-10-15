# Configuration record: REG_PCB1-X

Procedure according to the swap test protocol, section "Measurement procedure for one configuration".

| Item | Value |
| --- | --- |
| Step | REG (regression run of firmware version 2) |
| Board + CPU | PCB1 + CPU-X |
| Expected CPU SN | 141916cb4450334d342e3120ff060b |
| Expected Board ID | 8a32128415213248 |
| Binary | 20261010_swaptest_final/bin/test_uc3a0512.bin, firmware version 2, SHA-256 2ECFE5317649FABD27797608F3F9924AAAC45B4DAA870A7546195E9975294440 |
| Tools | tools/ at commit 396780b (record template from dbcb354, see Notes) |
| Date | 2026-10-15 |
| Performed by | Pavel Mikšíček |
| Checked by | Milan Holinka |

## Checklist

- [x] Programming, only where the procedure requires it: `tools/program.ps1` → `program.txt` (09:26:18, exit 0)
- [x] JTAG connected, readback `tools/readback.ps1` → `readback.txt` with `RESULT OK` (SHA-256 of flash = binary) (09:27:03)
- [x] JTAG disconnected from the board
- [x] Photo of the board with readable CPU marking and board label, with a time stamp in the image, taken during the measurement of the configuration (between the readback and the end of the UART capture): `photo_20261015-101959.jpg` (10:19:59)
- [x] UART capture started `tools/capture.ps1` → `uart.raw`, `uart.log` (10:22:26)
- [x] Three runs: power off, wait ≥ 10 s, power on, record the output until the RESULT line
- [x] VDDIO, VDDIN, VDDCORE and ambient temperature measured during the 1st run (below)
- [x] Capture stopped, evaluation `tools/evaluate.py` → `evaluate.txt`
- [x] Hashes `tools/sha256sums.ps1` → `SHA256SUMS`, commit and push

The SHA-256 of `SHA256SUMS` and the signatures follow the hashes and are therefore not part of this record. The SHA-256 of `SHA256SUMS` is entered in the table of records in the protocol; the signatures of the operator and the checker on the protocol cover that table.

## Measurements during the 1st run

| Quantity | Value | Limit |
| --- | --- | --- |
| VDDIO | same as VDDIN (VDDIO is connected to VDDIN on the board, not measured separately) | 3.0–3.6 V |
| VDDIN | 3.3138 V average, 200 mV peak-to-peak | 3.0–3.6 V |
| VDDCORE | 1.7733 V average, 200 mV peak-to-peak | 1.65–1.95 V |
| Ambient temperature | 24.6 °C | – |

Instrument: Keysight DSO-X 2024A (SN MY52160502), 10:1 probes, DC coupling, 1 V/div, 500 kSa/s, measurements Avg - FS and Pk-Pk; screenshot `scope_pcb1_20261015-102444.png`. At 1 V/div one step of the 8-bit converter is about 31 mV, so the peak-to-peak values are near the resolution limit of this setting and do not characterise the supply ripple.

## Runs

Power-on time = first `Break` event in `uart.log` (the `BOOT rc` marker is sent at 14 400 Bd).

| Run | Power-on (time) | Started within 5 s | Result (evaluate.py) | Note |
| --- | --- | --- | --- | --- |
| 1 | 10:22:45.786 | yes | FAIL | 44 508 / 48 000 failing reads, 100/100 cycles failed, 120.3 s; supply voltages measured |
| 2 | 10:25:29.934 | yes | FAIL | 44 384 / 48 000 failing reads, 100/100 cycles failed, 120.3 s |
| 3 | 10:28:20.646 | yes | FAIL | 44 387 / 48 000 failing reads, 100/100 cycles failed, 120.3 s |

Total number of power-ons, including repeated attempts: 3

## Configuration result

FAIL according to `evaluate.txt` (line `CONFIGURATION RESULT: FAIL`): 3 valid runs of 3, all FAIL.

## Notes

- This record repeats `REG_PCB1-X_attempt1` (2026-10-14), which was superseded because the board did not yet carry the laser label PCB1. That record is kept unchanged.
- The photo was taken at 10:19:59 by the camera clock (Nikon D3400, time stamp in the image and in EXIF), after the readback at 09:27:03 and before the start of the UART capture at 10:22:26 by the PC clock. The laser label PCB1 is in the bottom right corner of the board.
- The record template was changed in commit dbcb354 after the measurement and before this record was filled in: a step label in the photo is no longer required, because the time stamp in the image and the laser board label identify the configuration and the step. The photo has no step label. The scripts are unchanged between 396780b and dbcb354.
- `readback.txt` shows `FCR=0x00000000`, the reset value read while the CPU was halted by the debugger. At run time the firmware sets `FCR=0x00000040` (FWS=1), see the `ID fcr` lines in `uart.log`.
- `info.txt` shows `Target voltage: 2.47 V`. The VTref pin of the JTAG connector is not connected on the board, so this value is not the target supply voltage; the supply voltages are given above.
- The capture was ended by stopping the capture process at 10:32:39 after the third run; `uart.log` therefore has no `# capture end` line. No `Overrun` or other error events in `uart.log`.
- The oscilloscope screenshot was saved at 10:24:44 by the oscilloscope clock; run 1 ended with its RESULT line at 10:24:46.135 by the PC clock. The two clocks are not synchronised.
