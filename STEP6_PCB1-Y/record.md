# Configuration record: STEP6_PCB1-Y

Procedure according to the swap test protocol, section "Measurement procedure for one configuration".

| Item | Value |
| --- | --- |
| Step | STEP6 (step 6 of the swap test protocol, after resoldering 1, see `RESOLDER1`) |
| Board + CPU | PCB1 + CPU-Y |
| Expected CPU SN | ba16905e4450334d342e3120ff0714 |
| Expected Board ID | 8a32128415213248 |
| Binary | 20261010_swaptest_final/bin/test_uc3a0512.bin, firmware version 2, SHA-256 2ECFE5317649FABD27797608F3F9924AAAC45B4DAA870A7546195E9975294440 |
| Protocol | version 1.0, approved in commit fa7ec7e |
| Tools | tools/ at commit 8a7c6a1 (last change of tools/ in d5c8c05) |
| Date | 2026-11-04 |
| Performed by | Pavel Mikšíček |
| Checked by | Milan Holinka |

## Checklist

- [x] Programming, only where the procedure requires it: not required in step 6; the firmware moved with CPU-Y and is verified by the readback
- [x] JTAG connected, readback `tools/readback.ps1` → `readback.txt` with `RESULT OK` (SHA-256 of flash = binary) (16:07:46)
- [x] JTAG disconnected from the board
- [x] Photo of the board with readable CPU marking and board label, with a time stamp in the image, taken during the measurement of the configuration (between the readback and the end of the UART capture): `photo_20261104-161114.jpg` (16:11:14)
- [x] UART capture started `tools/capture.ps1` → `uart.raw`, `uart.log` (16:22:46, port COM9)
- [x] Three runs: power off, wait ≥ 10 s, power on, record the output until the RESULT line
- [x] VDDIN, VDDCORE (VDDIO is connected to VDDIN on the board) and ambient temperature measured during the 1st run (below); oscilloscope probes connected with the power off; screenshot `scope_pcb1_20261104-162539.png` saved at 16:25:39, not within the first minute of the run (see Notes)
- [x] Capture stopped, evaluation `tools/evaluate.py` → `evaluate.txt`
- [x] Hashes `tools/sha256sums.ps1` → `SHA256SUMS`, commit and push

The SHA-256 of `SHA256SUMS` and the signatures follow the hashes and are therefore not part of this record. The SHA-256 of `SHA256SUMS` is entered in the table of records in the protocol; the signatures of the operator and the checker on the protocol cover that table.

## Measurements during the 1st run

| Quantity | Value | Limit |
| --- | --- | --- |
| VDDIO | same as VDDIN (connected on the board) | 3.0–3.6 V |
| VDDIN | 3.3175 V average, 280 mV peak-to-peak | 3.0–3.6 V |
| VDDCORE | 1.8103 V average, 320 mV peak-to-peak | 1.65–1.95 V |
| Ambient temperature | 24.4 °C | – |

Instrument: Keysight DSO-X 2024A (SN MY52160502), 10:1 probes, DC coupling, 1 V/div, 500 kSa/s, measurements Avg - FS and Pk-Pk; screenshot `scope_pcb1_20261104-162539.png`. At 1 V/div one step of the 8-bit converter is about 31 mV, so the peak-to-peak values are near the resolution limit of this setting and do not characterise the supply ripple. Thermometer: indicative instrument, manufacturer METEL.

## Runs

Power-on time = first `Break` event in `uart.log` (the `BOOT rc` marker is sent at 14 400 Bd).

| Run | Power-on (time) | Started within 5 s | Result (evaluate.py) | Note |
| --- | --- | --- | --- | --- |
| 1 | 16:24:04.906 | yes | PASS | 0 / 48 000 failing reads, 0/100 cycles failed, 112.8 s; supply voltages measured |
| 2 | 16:26:48.767 | yes | PASS | 0 / 48 000 failing reads, 0/100 cycles failed, 112.8 s |
| 3 | 16:29:07.041 | yes | PASS | 0 / 48 000 failing reads, 0/100 cycles failed, 112.8 s |

Total number of power-ons, including repeated attempts (INCOMPLETE, DID NOT START or invalid runs): 3

## Configuration result

PASS according to `evaluate.txt` (line `CONFIGURATION RESULT: PASS`): 3 valid runs of 3, all PASS. The output of every run contains the SN of CPU-Y and the Board ID of PCB1, so CPU-Y is mounted on PCB1 as intended by resoldering 1.

## Notes

- The photo was taken at 16:11:14 by the camera clock (Nikon D3400, time stamp in the image and in EXIF), after the readback at 16:07:46 and before the start of the UART capture at 16:22:46 by the PC clock. The camera clock had been corrected after the acceptance photos in `RESOLDER1`. The laser label PCB1 is in the bottom right corner of the board.
- Oscilloscope: the screenshot was saved during run 1 at 16:25:39 by the oscilloscope clock, about 94 s after the start of run 1 at 16:24:05 by the PC clock; run 1 ended at 16:25:57.8. The procedure requires the screenshot to be saved within the first minute of the run; this is a deviation from the procedure. The oscilloscope and PC clocks are not synchronised.
- VDDCORE is 1.81 V, as on PCB2 with CPU-Y in steps REG and 1 (1.81 V), and different from PCB1 with CPU-X (1.77 V).
- `uart.log` contains one `SpErr Overrun` event at 16:22:46.177, when the port was opened, before the first power-on. Before the measurement the UART reception on COM9 was checked with a separate test capture outside this record, during which the board ran; the overrun is attributed to data left in the converter from that check. No other error events; 8 `Break` events at power-on.
- The USB–RS-485 converter is enumerated as COM9 on the measurement PC; the capture was run with `-Port COM9` (header of `uart.log`).
- `readback.txt` shows `FCR=0x00000000`, the reset value read while the CPU was halted by the debugger. At run time the firmware sets `FCR=0x00000040` (FWS=1), see the `ID fcr` lines in `uart.log`.
- `info.txt` shows `Target voltage: 2.48 V`. The VTref pin of the JTAG connector is not connected on the board, so this value is not the target supply voltage; the supply voltages are given above.
- The capture was ended by stopping the capture process at 16:31:55 after the third run; `uart.log` therefore has no `# capture end` line.
