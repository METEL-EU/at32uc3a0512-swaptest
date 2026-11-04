# Configuration record: STEP6_PCB2-X

Procedure according to the swap test protocol, section "Measurement procedure for one configuration".

| Item | Value |
| --- | --- |
| Step | STEP6 (step 6 of the swap test protocol, after resoldering 1, see `RESOLDER1`) |
| Board + CPU | PCB2 + CPU-X |
| Expected CPU SN | 141916cb4450334d342e3120ff060b |
| Expected Board ID | 8a3203841518175d |
| Binary | 20261010_swaptest_final/bin/test_uc3a0512.bin, firmware version 2, SHA-256 2ECFE5317649FABD27797608F3F9924AAAC45B4DAA870A7546195E9975294440 |
| Protocol | version 1.0, approved in commit fa7ec7e |
| Tools | tools/ at commit d1c821e (last change of tools/ in d5c8c05) |
| Date | 2026-11-04 |
| Performed by | Pavel Mikšíček |
| Checked by | Milan Holinka |

## Checklist

- [x] Programming, only where the procedure requires it: not required in step 6; the firmware moved with CPU-X and is verified by the readback
- [x] JTAG connected, readback `tools/readback.ps1` → `readback.txt` with `RESULT OK` (SHA-256 of flash = binary) (16:36:47)
- [x] JTAG disconnected from the board
- [x] Photo of the board with readable CPU marking and board label, with a time stamp in the image, taken during the measurement of the configuration (between the readback and the end of the UART capture): `photo_20261104-163812.jpg` (16:38:12)
- [x] UART capture started `tools/capture.ps1` → `uart.raw`, `uart.log` (16:38:56, port COM9)
- [x] Three runs: power off, wait ≥ 10 s, power on, record the output until the RESULT line; the first power-on was interrupted and repeated (see Runs)
- [x] VDDIN, VDDCORE (VDDIO is connected to VDDIN on the board) and ambient temperature measured during the 1st valid run (below); oscilloscope probes connected with the power off; screenshot `scope_pcb_20261104-164100.png` saved at 16:41:00, not within the first minute of the run (see Notes)
- [x] Capture stopped, evaluation `tools/evaluate.py` → `evaluate.txt`
- [x] Hashes `tools/sha256sums.ps1` → `SHA256SUMS`, commit and push

The SHA-256 of `SHA256SUMS` and the signatures follow the hashes and are therefore not part of this record. The SHA-256 of `SHA256SUMS` is entered in the table of records in the protocol; the signatures of the operator and the checker on the protocol cover that table.

## Measurements during the 1st valid run (run 2 below)

| Quantity | Value | Limit |
| --- | --- | --- |
| VDDIO | same as VDDIN (connected on the board) | 3.0–3.6 V |
| VDDIN | 3.3120 V average, 240 mV peak-to-peak | 3.0–3.6 V |
| VDDCORE | 1.7712 V average, 320 mV peak-to-peak | 1.65–1.95 V |
| Ambient temperature | 24.4 °C | – |

Instrument: Keysight DSO-X 2024A (SN MY52160502), 10:1 probes, DC coupling, 1 V/div, 500 kSa/s, measurements Avg - FS and Pk-Pk; screenshot `scope_pcb_20261104-164100.png`. At 1 V/div one step of the 8-bit converter is about 31 mV, so the peak-to-peak values are near the resolution limit of this setting and do not characterise the supply ripple. Thermometer: indicative instrument, manufacturer METEL.

## Runs

Power-on time = first `Break` event in `uart.log` (the `BOOT rc` marker is sent at 14 400 Bd). The run numbers are those of `evaluate.txt`.

| Run | Power-on (time) | Started within 5 s | Result (evaluate.py) | Note |
| --- | --- | --- | --- | --- |
| 1 | 16:39:12.922 | yes | INCOMPLETE | the power connector was not fully inserted and came out; output ends at 16:39:13.643 during the cycle matrix of cycle 1; repeated |
| 2 | 16:39:34.259 | yes | FAIL | 46 311 / 48 000 failing reads, 100/100 cycles failed, 120.6 s; supply voltages measured |
| 3 | 16:41:56.011 | yes | FAIL | 45 929 / 48 000 failing reads, 100/100 cycles failed, 120.5 s |
| 4 | 16:44:19.329 | yes | FAIL | 45 790 / 48 000 failing reads, 100/100 cycles failed, 120.6 s |

Total number of power-ons, including repeated attempts (INCOMPLETE, DID NOT START or invalid runs): 4 (one INCOMPLETE attempt, three valid runs)

## Configuration result

FAIL according to `evaluate.txt` (line `CONFIGURATION RESULT: FAIL`): 3 valid runs of 3, all FAIL; the INCOMPLETE attempt is not counted, as the protocol requires. The output of every run contains the SN of CPU-X and the Board ID of PCB2, so CPU-X is mounted on PCB2 as intended by resoldering 1.

## Notes

- Run 1: the operator had not fully inserted the power connector before the first power-on; it came out about 0.6 s after START. The power was off for about 20 s (from 16:39:13.6 to 16:39:34.3) before run 2. The partial output of run 1 is kept unchanged in `uart.log`.
- The photo was taken at 16:38:12 by the camera clock (Nikon D3400, time stamp in the image and in EXIF), after the readback at 16:36:47 and before the start of the UART capture at 16:38:56 by the PC clock. The laser label PCB2 is in the bottom right corner of the board.
- Oscilloscope: the acquisition was stopped during run 2 (the screen of the stopped acquisition shows 16:40 by the oscilloscope clock); the screenshot was saved at 16:41:00, about 86 s after the start of run 2 at 16:39:34 by the PC clock. Run 2 ended at 16:41:34.9. The procedure requires the screenshot to be saved within the first minute of the run; this is a deviation from the procedure. The oscilloscope and PC clocks are not synchronised.
- VDDCORE is 1.77 V, as on PCB1 with CPU-X in steps REG and 1 (1.77 V), and different from PCB2 with CPU-Y (1.81 V).
- `uart.log` contains one `SpErr Overrun` event at 16:38:56.216, when the port was opened, before the first power-on. The board was still sending its output after the reset at the end of the readback while the port was closed; the event is attributed to the USB–RS-485 converter reporting that data when the port was opened. No other error events; 13 `Break` events at power-on.
- The USB–RS-485 converter is enumerated as COM9 on the measurement PC; the capture was run with `-Port COM9` (header of `uart.log`).
- `readback.txt` shows `FCR=0x00000000`, the reset value read while the CPU was halted by the debugger. At run time the firmware sets `FCR=0x00000040` (FWS=1), see the `ID fcr` lines in `uart.log`.
- `info.txt` shows `Target voltage: 2.47 V`. The VTref pin of the JTAG connector is not connected on the board, so this value is not the target supply voltage; the supply voltages are given above.
- The capture was ended by stopping the capture process at 16:46:48 after the fourth power-on; `uart.log` therefore has no `# capture end` line.
