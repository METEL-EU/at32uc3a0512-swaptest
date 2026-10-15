# Configuration record: REG_PCB2-Y

Procedure according to the swap test protocol, section "Measurement procedure for one configuration".

| Item | Value |
| --- | --- |
| Step | REG (regression run of firmware version 2) |
| Board + CPU | PCB2 + CPU-Y |
| Expected CPU SN | ba16905e4450334d342e3120ff0714 |
| Expected Board ID | 8a3203841518175d |
| Binary | 20261010_swaptest_final/bin/test_uc3a0512.bin, firmware version 2, SHA-256 2ECFE5317649FABD27797608F3F9924AAAC45B4DAA870A7546195E9975294440 |
| Tools | tools/ at commit dbcb354 |
| Date | 2026-10-15 |
| Performed by | Pavel Mikšíček |
| Checked by | Milan Holinka |

## Checklist

- [x] Programming, only where the procedure requires it: `tools/program.ps1` → `program.txt` (14:58:23, exit 0)
- [x] JTAG connected, readback `tools/readback.ps1` → `readback.txt` with `RESULT OK` (SHA-256 of flash = binary) (14:59:14)
- [x] JTAG disconnected from the board
- [x] Photo of the board with readable CPU marking and board label, with a time stamp in the image, taken during the measurement of the configuration (between the readback and the end of the UART capture): `photo_20261015-150057.jpg` (15:00:57)
- [x] UART capture started `tools/capture.ps1` → `uart.raw`, `uart.log` (15:04:23)
- [x] Three runs: power off, wait ≥ 10 s, power on, record the output until the RESULT line
- [x] VDDIO, VDDIN, VDDCORE and ambient temperature measured during the 1st run (below)
- [x] Capture stopped, evaluation `tools/evaluate.py` → `evaluate.txt`
- [x] Hashes `tools/sha256sums.ps1` → `SHA256SUMS`, commit and push

The SHA-256 of `SHA256SUMS` and the signatures follow the hashes and are therefore not part of this record. The SHA-256 of `SHA256SUMS` is entered in the table of records in the protocol; the signatures of the operator and the checker on the protocol cover that table.

## Measurements during the 1st run

| Quantity | Value | Limit |
| --- | --- | --- |
| VDDIO | same as VDDIN (VDDIO is connected to VDDIN on the board, not measured separately) | 3.0–3.6 V |
| VDDIN | 3.3129 V average, 200 mV peak-to-peak | 3.0–3.6 V |
| VDDCORE | 1.8119 V average, 280 mV peak-to-peak | 1.65–1.95 V |
| Ambient temperature | 25.1 °C | – |

Instrument: Keysight DSO-X 2024A (SN MY52160502), 10:1 probes, DC coupling, 1 V/div, 500 kSa/s, measurements Avg - FS and Pk-Pk; screenshot `scope_pcb2_20261015-150634.png`. At 1 V/div one step of the 8-bit converter is about 31 mV, so the peak-to-peak values are near the resolution limit of this setting and do not characterise the supply ripple.

## Runs

Power-on time = first `Break` event in `uart.log` (the `BOOT rc` marker is sent at 14 400 Bd).

| Run | Power-on (time) | Started within 5 s | Result (evaluate.py) | Note |
| --- | --- | --- | --- | --- |
| 1 | 15:04:45.364 | yes | PASS | 0 / 48 000 failing reads, 0/100 cycles failed, 112.8 s; supply voltages measured; UART framing errors, see Notes |
| 2 | 15:07:17.495 | yes | PASS | 0 / 48 000 failing reads, 0/100 cycles failed, 112.8 s |
| 3 | 15:10:15.612 | yes | PASS | 0 / 48 000 failing reads, 0/100 cycles failed, 112.8 s |

Total number of power-ons, including repeated attempts: 3

## Configuration result

PASS according to `evaluate.txt` (line `CONFIGURATION RESULT: PASS`): 3 valid runs of 3, all PASS.

## Notes

- The photo was taken at 15:00:57 by the camera clock (Nikon D3400, time stamp in the image and in EXIF), after the readback at 14:59:14 and before the start of the UART capture at 15:04:23 by the PC clock. The laser label PCB2 is in the bottom right corner of the board.
- During the 1st run, while the supply voltages were being measured, `uart.log` shows 18 `SpErr Frame` events between 15:04:52 and 15:05:25. They corrupted the cell table output of cycles 7, 31, 33 and 36 (non-printable bytes shown as `<xx>`, some table lines merged or lost). All `ID` lines, all 100 `Summary iter=` lines and the `RESULT` line of the run are intact, and the `RESULT` line carries a valid crc, so `evaluate.py` counts the run as valid. Runs 2 and 3 have no framing errors. No other error events in `uart.log`.
- `readback.txt` shows `FCR=0x00000000`, the reset value read while the CPU was halted by the debugger. At run time the firmware sets `FCR=0x00000040` (FWS=1), see the `ID fcr` lines in `uart.log`.
- `info.txt` shows `Target voltage: 2.47 V`. The VTref pin of the JTAG connector is not connected on the board, so this value is not the target supply voltage; the supply voltages are given above.
- The capture was ended by stopping the capture process at 15:12:38 after the third run; `uart.log` therefore has no `# capture end` line.
- The oscilloscope screenshot was saved at 15:06:34 by the oscilloscope clock; run 1 ended with its RESULT line at 15:06:38.180 by the PC clock. The two clocks are not synchronised.
