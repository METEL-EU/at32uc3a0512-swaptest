# Configuration record: <STEP>_<PCB>-<CPU>

Procedure according to the swap test protocol, section "Measurement procedure for one configuration". A copy of this template is saved in the configuration folder as `record.md`.

| Item | Value |
| --- | --- |
| Step | <REG / STEP1 / STEP6 / STEP7> |
| Board + CPU | <PCB1 + CPU-X …> |
| Expected CPU SN | <30 hex> |
| Expected Board ID | <16 hex> |
| Binary | 20261010_swaptest_final/bin/test_uc3a0512.bin, firmware version 2, SHA-256 2ECFE5317649FABD27797608F3F9924AAAC45B4DAA870A7546195E9975294440 |
| Tools | tools/ at commit <hash> |
| Date | <YYYY-MM-DD> |
| Performed by | <name> |
| Checked by | <name> |

## Checklist

- [ ] Programming, only where the procedure requires it: `tools/program.ps1` → `program.txt`
- [ ] JTAG connected, readback `tools/readback.ps1` → `readback.txt` with `RESULT OK` (SHA-256 of flash = binary)
- [ ] JTAG disconnected from the board
- [ ] Photo of the board with readable CPU marking, board label and step label, with a time stamp in the image, taken during the measurement of the configuration (between the readback and the end of the UART capture): `photo_*.jpg`
- [ ] UART capture started `tools/capture.ps1` → `uart.raw`, `uart.log`
- [ ] Three runs: power off, wait ≥ 10 s, power on, record the output until the RESULT line
- [ ] VDDIO, VDDIN, VDDCORE and ambient temperature measured during the 1st run (below)
- [ ] Capture stopped, evaluation `tools/evaluate.py` → `evaluate.txt`
- [ ] Hashes `tools/sha256sums.ps1` → `SHA256SUMS`, commit and push

The SHA-256 of `SHA256SUMS` and the signatures follow the hashes and are therefore not part of this record. The SHA-256 of `SHA256SUMS` is entered in the table of records in the protocol; the signatures of the operator and the checker on the protocol cover that table.

## Measurements during the 1st run

| Quantity | Value | Limit |
| --- | --- | --- |
| VDDIO | <V> | 3.0–3.6 V |
| VDDIN | <V> | <from datasheet> |
| VDDCORE | <V> | 1.65–1.95 V |
| Ambient temperature | <°C> | – |

## Runs

Power-on time = first `Break` event in `uart.log` (the `BOOT rc` marker is sent at 14 400 Bd).

| Run | Power-on (time) | Started within 5 s | Result (evaluate.py) | Note |
| --- | --- | --- | --- | --- |
| 1 | | | | |
| 2 | | | | |
| 3 | | | | |

Total number of power-ons, including repeated attempts: <n>

## Configuration result

<FAIL / PASS / INCONSISTENT> according to `evaluate.txt` (line `CONFIGURATION RESULT`).

## Notes

<deviations from the procedure, equipment changes>
