# Configuration record: STEP1_PCB1-X

Procedure according to the swap test protocol, section "Measurement procedure for one configuration".

| Item | Value |
| --- | --- |
| Step | STEP1 (step 1 of the swap test protocol) |
| Board + CPU | PCB1 + CPU-X |
| Expected CPU SN | 141916cb4450334d342e3120ff060b |
| Expected Board ID | 8a32128415213248 |
| Binary | 20261010_swaptest_final/bin/test_uc3a0512.bin, firmware version 2, SHA-256 2ECFE5317649FABD27797608F3F9924AAAC45B4DAA870A7546195E9975294440 |
| Protocol | version 1.0, approved in commit fa7ec7e |
| Tools | tools/ at commit fa7ec7e (last change of tools/ in d5c8c05) |
| Date | 2026-10-19 |
| Performed by | Pavel Mikšíček |
| Checked by | Milan Holinka |

## Checklist

- [x] Programming, only where the procedure requires it: not required in step 1; CPU-X was programmed during the preparation (`REG_PCB1-X`)
- [x] JTAG connected, readback `tools/readback.ps1` → `readback.txt` with `RESULT OK` (SHA-256 of flash = binary) (13:56:18)
- [x] JTAG disconnected from the board
- [x] Photo of the board with readable CPU marking and board label, with a time stamp in the image, taken during the measurement of the configuration (between the readback and the end of the UART capture): `photo_20261019-140154.JPG` (14:01:54)
- [x] UART capture started `tools/capture.ps1` → `uart.raw`, `uart.log` (14:12:02, port COM9)
- [x] Three runs: power off, wait ≥ 10 s, power on, record the output until the RESULT line
- [x] VDDIN, VDDCORE (VDDIO is connected to VDDIN on the board) and ambient temperature measured during the 1st run (below); oscilloscope probes connected with the power off; screenshot `scope_pcb1_20261019-141519.png` saved at 14:15:19, not within the first minute of the run (see Notes)
- [x] Capture stopped, evaluation `tools/evaluate.py` → `evaluate.txt`
- [x] Hashes `tools/sha256sums.ps1` → `SHA256SUMS`, commit and push

The SHA-256 of `SHA256SUMS` and the signatures follow the hashes and are therefore not part of this record. The SHA-256 of `SHA256SUMS` is entered in the table of records in the protocol; the signatures of the operator and the checker on the protocol cover that table.

## Measurements during the 1st run

| Quantity | Value | Limit |
| --- | --- | --- |
| VDDIO | same as VDDIN (connected on the board) | 3.0–3.6 V |
| VDDIN | 3.3152 V average, 200 mV peak-to-peak | 3.0–3.6 V |
| VDDCORE | 1.7735 V average, 320 mV peak-to-peak | 1.65–1.95 V |
| Ambient temperature | 24.7 °C | – |

Instrument: Keysight DSO-X 2024A (SN MY52160502), 10:1 probes, DC coupling, 1 V/div, 500 kSa/s, measurements Avg - FS and Pk-Pk; screenshot `scope_pcb1_20261019-141519.png`. At 1 V/div one step of the 8-bit converter is about 31 mV, so the peak-to-peak values are near the resolution limit of this setting and do not characterise the supply ripple. Thermometer: indicative instrument, manufacturer METEL.

## Runs

Power-on time = first `Break` event in `uart.log` (the `BOOT rc` marker is sent at 14 400 Bd).

| Run | Power-on (time) | Started within 5 s | Result (evaluate.py) | Note |
| --- | --- | --- | --- | --- |
| 1 | 14:12:27.042 | yes | FAIL | 44 906 / 48 000 failing reads, 100/100 cycles failed, 120.6 s; supply voltages measured |
| 2 | 14:15:02.941 | yes | FAIL | 44 648 / 48 000 failing reads, 100/100 cycles failed, 120.4 s |
| 3 | 14:18:29.227 | yes | FAIL | 45 176 / 48 000 failing reads, 100/100 cycles failed, 120.4 s |

Total number of power-ons, including repeated attempts (INCOMPLETE, DID NOT START or invalid runs): 3

## Configuration result

FAIL according to `evaluate.txt` (line `CONFIGURATION RESULT: FAIL`): 3 valid runs of 3, all FAIL.

## Notes

- The photo was taken at 14:01:54 by the camera clock (Nikon D3400, time stamp in the image and in EXIF), after the readback at 13:56:18 and before the start of the UART capture at 14:12:02 by the PC clock. The laser label PCB1 is in the bottom right corner of the board. The type line of the CPU marking (32UC3A0512-U) is readable; the lower lines (date code, lot) are less legible because of a light reflection. The CPU is identified by its SN.
- Oscilloscope: the acquisition was stopped during run 1; the screen of the stopped acquisition shows 14:14 by the oscilloscope clock (run 1 ran from 14:12:27 to 14:14:27.7 by the PC clock), so the measured values are from run 1. The screenshot of the stopped acquisition was saved at 14:15:19 by the oscilloscope clock, i.e. during run 2. The procedure requires the screenshot to be saved within the first minute of the run; this is a deviation from the procedure. The oscilloscope and PC clocks are not synchronised.
- The USB–RS-485 converter is enumerated as COM9 on the measurement PC; the capture was run with `-Port COM9` (header of `uart.log`).
- `readback.txt` shows `FCR=0x00000000`, the reset value read while the CPU was halted by the debugger. At run time the firmware sets `FCR=0x00000040` (FWS=1), see the `ID fcr` lines in `uart.log`.
- `info.txt` shows `Target voltage: 2.46 V`. The VTref pin of the JTAG connector is not connected on the board, so this value is not the target supply voltage; the supply voltages are given above.
- The capture was ended by stopping the capture process at 14:24:39 after the third run; `uart.log` therefore has no `# capture end` line. The only line events in `uart.log` are 10 `Break` events at power-on; no framing or other error events.
