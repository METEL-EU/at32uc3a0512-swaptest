# Configuration record: REG_PCB1-X

Procedure according to the swap test protocol, section "Measurement procedure for one configuration".

| Item | Value |
| --- | --- |
| Step | REG (regression run of firmware version 2) |
| Board + CPU | PCB1 + CPU-X |
| Expected CPU SN | 141916cb4450334d342e3120ff060b |
| Expected Board ID | 8a32128415213248 |
| Binary | 20261010_swaptest_final/bin/test_uc3a0512.bin, firmware version 2, SHA-256 2ECFE5317649FABD27797608F3F9924AAAC45B4DAA870A7546195E9975294440 |
| Tools | tools/ at commit 6a9e94b (programming with 20a70e9, see Notes) |
| Date | 2026-10-14 |
| Performed by | Pavel Mikšíček |
| Checked by | Milan Holinka |

## Checklist

- [x] Programming, only where the procedure requires it: `tools/program.ps1` → `program.txt` (11:21:36, exit 0)
- [x] Photo of the board with readable CPU marking, board label and step label: `photo_20261014-124440.jpg`
- [x] JTAG connected, readback `tools/readback.ps1` → `readback.txt` with `RESULT OK` (11:25:01; first attempt failed, see Notes)
- [x] JTAG disconnected from the board
- [x] UART capture started `tools/capture.ps1` → `uart.raw`, `uart.log` (15:02:47; first capture `uart_attempt1.*`, see Notes)
- [x] Three runs: power off, wait ≥ 10 s, power on, record the output until the RESULT line
- [x] VDDIO, VDDIN, VDDCORE and ambient temperature measured during the 2nd run, not the 1st (below, see Notes)
- [x] Capture stopped, evaluation `tools/evaluate.py` → `evaluate.txt`
- [x] Hashes `tools/sha256sums.ps1` → `SHA256SUMS`, commit and push
- [ ] SHA-256 of `SHA256SUMS` sent by e-mail to the recipient of the results
- [ ] Signatures of the operator and the checker

## Measurements during the 2nd run

| Quantity | Value | Limit |
| --- | --- | --- |
| VDDIO | same as VDDIN (VDDIO is connected to VDDIN on the board, not measured separately) | 3.0–3.6 V |
| VDDIN | 3.2989 V average, 200 mV peak-to-peak | 3.0–3.6 V (not yet verified against the datasheet) |
| VDDCORE | 1.7718 V average, 240 mV peak-to-peak | 1.65–1.95 V |
| Ambient temperature | 24.7 °C | – |

Instrument: Keysight DSO-X 2024A (SN MY52160502), 10:1 probes, DC coupling, 1 V/div, 500 kSa/s, measurements Avg - FS and Pk-Pk; screenshot `scope_20261014-151405.png`. At 1 V/div one step of the 8-bit converter is about 31 mV, so the peak-to-peak values are near the resolution limit of this setting and do not characterise the supply ripple.

## Runs

Power-on time = first `Break` event in `uart.log` (the `BOOT rc` marker is sent at 14 400 Bd).

| Run | Power-on (time) | Started within 5 s | Result (evaluate.py) | Note |
| --- | --- | --- | --- | --- |
| 1 | 15:04:37.829 | yes | FAIL | 44 066 / 48 000 failing reads, 100/100 cycles failed, 120.3 s |
| 2 | 15:11:44.839 | yes | FAIL | 44 133 / 48 000 failing reads, 100/100 cycles failed, 120.3 s; supply voltages measured |
| 3 | 15:15:45.697 | yes | FAIL | 43 926 / 48 000 failing reads, 100/100 cycles failed, 120.3 s |

Total number of power-ons, including repeated attempts: 3 in the evaluated capture `uart.log`; 2 more in the incomplete first capture `uart_attempt1.log` (see Notes).

## Configuration result

FAIL according to `evaluate.txt` (line `CONFIGURATION RESULT: FAIL`): 3 valid runs of 3, all FAIL.

## Notes

- Readback: the first attempt at 11:22:31 ended with `RESULT MISMATCH or read error` because `readback.ps1` changes its working directory and the relative paths given on the command line no longer pointed to the record folder. Its outputs are kept unchanged as `readback_attempt1.txt` and `info_attempt1.txt`. The tools were fixed in commit 6a9e94b (relative paths resolved to absolute) and the readback was repeated at 11:25:01 with `RESULT OK`.
- Programming was run at 11:21:36 with tools at commit 20a70e9. Commit 6a9e94b only adds the same path resolution to `program.ps1`; `program.txt` shows a successful chip erase and programming, and the programmed content is confirmed by the readback.
- `readback.txt` shows `FCR=0x00000000`, the reset value read while the CPU was halted by the debugger. At run time the firmware sets `FCR=0x00000040` (FWS=1), see the `ID fcr` lines in `uart.log`.
- `info.txt` shows `Target voltage: 2.48 V`. The VTref pin of the JTAG connector is not connected on the board, so this value is not the target supply voltage; the supply voltages are given above.
- First UART capture: `uart_attempt1.raw` and `uart_attempt1.log` (13:16:09–14:01:09, ended by its 2700 s limit) contain two complete runs, power-on 13:16:46 and 13:20:41, both valid and FAIL (44 403 and 44 698 of 48 000 failing reads). The third run was not performed within the capture time because the supply voltage measurement took longer than planned. Evaluation in `evaluate_attempt1.txt`: `NOT COMPLETE (2 of 3 valid runs, invalid 0)`. The files were renamed, not modified, and the capture was repeated from 15:02:47 into `uart.raw` and `uart.log`.
- The second capture was ended by stopping the capture process at 15:18:29 after the third run; `uart.log` therefore has no `# capture end` line. No `Overrun` events in `uart.log`.
- Supply voltages were measured during the 2nd run instead of the 1st. The oscilloscope acquisition was stopped during run 2; the screenshot was saved at 15:14:05 by the oscilloscope clock, after run 2 had ended at 15:13:45 by the PC clock.
