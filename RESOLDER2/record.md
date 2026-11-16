# Resoldering record: RESOLDER2 (resoldering 2, before step 7)

Resoldering 2 according to the swap test protocol, section "Test sequence by phase": CPU-Y removed from PCB1 and CPU-X removed from PCB2, CPU-X soldered back onto PCB1 and CPU-Y back onto PCB2. The work was done by an external company under a separate work order.

| Item | Value |
| --- | --- |
| Boards handed over to the company | 2026-11-05 |
| Work performed (according to the company's record) | 2026-11-13 |
| Boards returned | 2026-11-13 |
| Acceptance check by the operator | 2026-11-16 |
| State after the work | PCB1 + CPU-X, PCB2 + CPU-Y |
| Performed by (acceptance check) | Pavel Mikšíček |
| Checked by | Milan Holinka |

The company's signed record (work order with the filled-in record and the handover table) is kept by the operator. It identifies the company and is therefore not part of this repository; its content is transcribed below.

## Company's record (transcription)

| Item | Company's entry |
| --- | --- |
| Equipment | hot-gas rework station ZEVAC Onyx24; no oven |
| Baking before removal | performed: 60 °C, 168 h, relative humidity 1 %; marked by the company as not according to J-STD-033 |
| Temperature profile, removal | screenshot provided, see `profile_removal.png`; numeric fields and the question "measured with a thermocouple on the board" not filled in |
| Temperature profile, soldering | screenshot provided, see `profile_soldering.png`; numeric fields and the question "measured with a thermocouple on the board" not filled in |
| Solder | a dash entered |
| Flux | Flux P26 |
| Cleaning after soldering | Vigon N680; flux residues removed: yes |
| ESD protection | ESD-protected workplace: yes |
| Visual inspection after soldering | orientation of both CPUs correct: yes; leads without bridges and shorts: yes |
| Number of removals and solderings in this order | not filled in (the work order specifies one removal and one soldering per CPU) |
| Board labels PCB1 and PCB2 undamaged and not covered | yes |
| Boards connected to power | no |
| Deviations from the work order | not filled in (neither "none" nor "yes" ticked) |
| Confirmation | dated 2026-11-13, stamped and signed by the company |

The items not filled in were not requested from the company afterwards; the order is closed.

## Temperature profiles

`profile_removal.png` and `profile_soldering.png` are crops of the screenshots of the rework station software provided by the company (window "Manual Interactive Soldering"); only the area outside that window was removed. Settings shown in both: hot-gas (top) 260 °C, flow 55 SLM, bottom heater 350 °C. The time axis of both charts is 0–210 s.

| Curve | Removal | Soldering |
| --- | --- | --- |
| Hot gas (red) | preheat plateau 180 °C to ≈ 137 s, peak ≈ 265 °C at ≈ 149 s, then 260 °C to ≈ 178 s | preheat plateau 180 °C to ≈ 142 s, peak ≈ 265 °C at ≈ 154 s, then 260 °C to ≈ 181 s |
| Thermocouple 1 (blue) | from ≈ 92 °C, maximum ≈ 190 °C at ≈ 175 s | ≈ 25 °C up to ≈ 34 s, then a rise to ≈ 120 °C within ≈ 6 s; maximum ≈ 194 °C at ≈ 180 s |

The position of thermocouple 1 on the board is not stated by the company. Thermocouple 1 stays below 217 °C in both profiles; the hot-gas temperature is above 217 °C from ≈ 141 s to ≈ 180 s (removal) and from ≈ 146 s to ≈ 183 s (soldering). Values read from the screenshots.

## Thermal cycles

Each CPU had one reflow from the production of its board and one removal and one soldering in resoldering 1 (`RESOLDER1`). This order adds one removal and one soldering per CPU according to the work order, i.e. five thermal cycles per CPU after resoldering 2. The company did not fill in the number of removals and solderings. As stated in the protocol, step 7 is above the limit of three reflows (doc32058K, section 39.3).

## Acceptance check (2026-11-16)

- Return bags labelled as specified in the work order: "PCB1 s CPU-X" and "PCB2 s CPU-Y".
- Photos of the top side: `photo_PCB1_top_20261116-100605.jpg`, `photo_PCB2_top_20261116-100638.jpg`; close-ups of the CPU: `photo_PCB1_cpu_20261116-103305.jpg`, `photo_PCB2_cpu_20261116-103342.jpg`. Time stamps by the camera clock (Nikon D3400, in the image and in EXIF). Laser labels PCB1 and PCB2 and the CPU marking are readable; the orientation of both CPUs is the same as in the photos in `RESOLDER1`.
- The package of CPU-X (on PCB1) has a dark mark to the right of the lot code that is not in the `RESOLDER1` photos. On inspection with a magnifier it is a felt-tip marker mark; no mechanical damage was found. The work order asked the company to keep the removed CPUs separate and marked so that they cannot be mixed up. The identity of the CPU on each board is verified by the CPU serial number in the output of step 7.
- Resistance of the supply rails to GND, power off, Fluke 115 (EUR), measured at the oscilloscope probing points, + on the rail:

| Board | 3V3–GND | VDDCORE–GND |
| --- | --- | --- |
| PCB1 (with CPU-X) | 5.895 kΩ | 46.33 kΩ |
| PCB2 (with CPU-Y) | 6.052 kΩ | 46.13 kΩ |

No short circuit; the two boards differ by 2.7 % (3V3) and 0.4 % (VDDCORE). Compared with the acceptance check in `RESOLDER1` (other CPU on each board: PCB1 5.894 kΩ / 45.40 kΩ, PCB2 6.006 kΩ / 45.79 kΩ), 3V3–GND changed by +0.02 % (PCB1) and +0.8 % (PCB2), VDDCORE–GND by +2.0 % (PCB1) and +0.7 % (PCB2). This check is not part of the protocol; it was done before the first power-on after resoldering.
