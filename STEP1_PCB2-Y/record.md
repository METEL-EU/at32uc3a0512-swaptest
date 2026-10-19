# Configuration record: STEP1_PCB2-Y

Procedure according to the swap test protocol, section "Measurement procedure for one configuration".

| Item | Value |
| --- | --- |
| Step | STEP1 (step 1 of the swap test protocol) |
| Board + CPU | PCB2 + CPU-Y |
| Expected CPU SN | ba16905e4450334d342e3120ff0714 |
| Expected Board ID | 8a3203841518175d |
| Binary | 20261010_swaptest_final/bin/test_uc3a0512.bin, firmware version 2, SHA-256 2ECFE5317649FABD27797608F3F9924AAAC45B4DAA870A7546195E9975294440 |
| Protocol | version 1.0, approved in commit fa7ec7e |
| Tools | tools/ at commit 86bceee (last change of tools/ in d5c8c05) |
| Date | 2026-10-19 |
| Performed by | Pavel Mikšíček |
| Checked by | Milan Holinka |

## Checklist

- [x] Programming, only where the procedure requires it: not required in step 1; CPU-Y was programmed during the preparation (`REG_PCB2-Y`)
- [x] JTAG connected, readback `tools/readback.ps1` → `readback.txt` with `RESULT OK` (SHA-256 of flash = binary) (14:43:00)
- [x] JTAG disconnected from the board
- [x] Photo of the board with readable CPU marking and board label, with a time stamp in the image, taken during the measurement of the configuration (between the readback and the end of the UART capture): `photo_20261019-144420.JPG` (14:44:20)
- [x] UART capture started `tools/capture.ps1` → `uart.raw`, `uart.log` (14:45:01, port COM9)
- [x] Three runs: power off, wait ≥ 10 s, power on, record the output until the RESULT line; a fourth run was performed by mistake (see Notes)
- [x] VDDIN, VDDCORE (VDDIO is connected to VDDIN on the board) and ambient temperature measured during the 1st run (below); oscilloscope probes connected with the power off; screenshot `scope_pcb2_20261019-144644.png` saved at 14:46:44 (see Notes)
- [x] Capture stopped, evaluation `tools/evaluate.py` → `evaluate.txt`, and `evaluate_runs4.txt` for all four runs (see Notes)
- [x] Hashes `tools/sha256sums.ps1` → `SHA256SUMS`, commit and push

The SHA-256 of `SHA256SUMS` and the signatures follow the hashes and are therefore not part of this record. The SHA-256 of `SHA256SUMS` is entered in the table of records in the protocol; the signatures of the operator and the checker on the protocol cover that table.

## Measurements during the 1st run

| Quantity | Value | Limit |
| --- | --- | --- |
| VDDIO | same as VDDIN (connected on the board) | 3.0–3.6 V |
| VDDIN | 3.3098 V average, 200 mV peak-to-peak | 3.0–3.6 V |
| VDDCORE | 1.8107 V average, 320 mV peak-to-peak | 1.65–1.95 V |
| Ambient temperature | 24.6 °C | – |

Instrument: Keysight DSO-X 2024A (SN MY52160502), 10:1 probes, DC coupling, 1 V/div, 500 kSa/s, measurements Avg - FS and Pk-Pk; screenshot `scope_pcb2_20261019-144644.png`. At 1 V/div one step of the 8-bit converter is about 31 mV, so the peak-to-peak values are near the resolution limit of this setting and do not characterise the supply ripple. Thermometer: indicative instrument, manufacturer METEL.

## Runs

Power-on time = first `Break` event in `uart.log` (the `BOOT rc` marker is sent at 14 400 Bd).

| Run | Power-on (time) | Started within 5 s | Result (evaluate.py) | Note |
| --- | --- | --- | --- | --- |
| 1 | 14:45:39.508 | yes | PASS | 0 / 48 000 failing reads, 0/100 cycles failed, 112.8 s; supply voltages measured |
| 2 | 14:49:25.790 | yes | PASS | 0 / 48 000 failing reads, 0/100 cycles failed, 112.8 s |
| 3 | 14:53:21.462 | yes | PASS | 0 / 48 000 failing reads, 0/100 cycles failed, 112.8 s |
| 4 | 14:55:34.344 | yes | PASS | 0 / 48 000 failing reads, 0/100 cycles failed, 112.7 s; additional run performed by mistake |

Total number of power-ons, including repeated attempts (INCOMPLETE, DID NOT START or invalid runs): 4 (no repeated attempts; the 4th power-on is the additional run)

## Configuration result

PASS: runs 1–3 required by the procedure are 3 valid runs of 3, all PASS; the additional run 4 is also valid and PASS, so there is no mixed result.

`evaluate.txt` (default evaluation of three runs) ends with `CONFIGURATION RESULT: INCONSISTENT` because its rule requires exactly three valid runs and the record contains four; all four runs are listed there as valid and PASS. `evaluate_runs4.txt` is the same evaluation of the same `uart.log` with the option `--runs 4` and ends with `CONFIGURATION RESULT: PASS`.

## Notes

- Deviation: four runs were performed instead of three; the fourth run (power-on at 14:55:34, 20 s after the RESULT line of run 3) was performed by mistake. It was not repeated because of a failure; all four runs are valid and PASS.
- The photo was taken at 14:44:20 by the camera clock (Nikon D3400, time stamp in the image and in EXIF), after the readback at 14:43:00 and before the start of the UART capture at 14:45:01 by the PC clock. The laser label PCB2 is in the bottom right corner of the board; the CPU marking is readable.
- Oscilloscope: the screenshot was saved during run 1 at 14:46:44 by the oscilloscope clock, about 65 s after the start of run 1 at 14:45:39.5 by the PC clock. The procedure requires the screenshot to be saved within the first minute of the run; the difference is of the order of the offset between the unsynchronised oscilloscope and PC clocks.
- The USB–RS-485 converter is enumerated as COM9 on the measurement PC; the capture was run with `-Port COM9` (header of `uart.log`).
- `readback.txt` shows `FCR=0x00000000`, the reset value read while the CPU was halted by the debugger. At run time the firmware sets `FCR=0x00000040` (FWS=1), see the `ID fcr` lines in `uart.log`.
- `info.txt` shows `Target voltage: 2.47 V`. The VTref pin of the JTAG connector is not connected on the board, so this value is not the target supply voltage; the supply voltages are given above.
- The capture was ended by stopping the capture process at 14:57:50 after the fourth run; `uart.log` therefore has no `# capture end` line. The only line events in `uart.log` are 11 `Break` events at power-on; no framing or other error events.
