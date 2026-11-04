# Resoldering record: RESOLDER1 (steps 4 and 5)

Resoldering 1 according to the swap test protocol, section "Test sequence by phase": CPU-X removed from PCB1 and CPU-Y removed from PCB2, CPU-Y soldered onto PCB1 and CPU-X onto PCB2. The work was done by an external company under a separate work order.

| Item | Value |
| --- | --- |
| Boards handed over to the company | 2026-10-21 |
| Work performed (according to the company's record) | 2026-11-02 |
| Boards returned | 2026-11-03 |
| Acceptance check by the operator | 2026-11-04 |
| State after the work | PCB1 + CPU-Y, PCB2 + CPU-X |
| Performed by (acceptance check) | Pavel Mikšíček |
| Checked by | Milan Holinka |

The company's signed record (work order with the filled-in record and the handover table) is kept by the operator. It identifies the company and is therefore not part of this repository; its content is transcribed below.

## Company's record (transcription)

| Item | Company's entry |
| --- | --- |
| Equipment | hot-gas rework station ZEVAC Onyx24; no oven |
| Baking before removal | performed: 60 °C, 168 h, relative humidity 1 %; marked by the company as not according to J-STD-033 |
| Temperature profile, removal | measured with a thermocouple on the board; screenshot provided, see `profile_removal.png`; numeric fields not filled in |
| Temperature profile, soldering | measured with a thermocouple on the board; screenshot provided, see `profile_soldering.png`; numeric fields not filled in |
| Solder | not filled in |
| Flux | FLUX-P26 |
| Cleaning after soldering | Vigon N680; flux residues removed: yes |
| ESD protection | ESD-protected workplace: yes |
| Visual inspection after soldering | orientation of both CPUs: not filled in; leads without bridges and shorts: yes |
| Number of removals and solderings in this order | not filled in (the work order specifies one removal and one soldering per CPU) |
| Board labels PCB1 and PCB2 undamaged and not covered | yes |
| Boards connected to power | no |
| Deviations from the work order | not filled in |
| Confirmation | dated 2026-11-02, stamped and signed by the company |

The items not filled in were not requested from the company afterwards; the order is closed.

## Temperature profiles

`profile_removal.png` and `profile_soldering.png` are crops of the screenshots of the rework station software provided by the company (window "Manual Interactive Soldering"); only the area outside that window was removed. Settings shown in both: hot-gas (top) 260 °C, flow 55 SLM, bottom heater 350 °C.

| Curve | Removal | Soldering |
| --- | --- | --- |
| Hot gas (red) | preheat plateau 180 °C to ≈ 131 s, peak ≈ 265 °C at ≈ 145 s, then 260 °C to ≈ 172 s | preheat plateau 180 °C to ≈ 133 s, peak ≈ 264 °C at ≈ 144 s, then 260 °C to ≈ 171 s |
| Thermocouple 1 (blue) | from ≈ 137 °C, maximum ≈ 193 °C at ≈ 168 s | from ≈ 96 °C, maximum ≈ 192 °C at ≈ 170 s |

The position of thermocouple 1 on the board is not stated by the company. Thermocouple 1 stays below 217 °C in both profiles; the hot-gas temperature is above 217 °C from ≈ 137 s to ≈ 175 s. Values read from the screenshots.

## Thermal cycles

Each CPU had one reflow from the production of its board. This order adds one removal and one soldering per CPU according to the work order, i.e. three thermal cycles per CPU after step 5. The company did not fill in the number of removals and solderings.

## Acceptance check (2026-11-04)

- Return bags labelled as specified in the work order: "PCB1 s CPU-Y" and "PCB2 s CPU-X".
- Photos of the top side: `photo_PCB1_top_20261104-160058.jpg`, `photo_PCB2_top_20261104-160137.jpg`. Laser labels PCB1 and PCB2 and the CPU marking are readable. The camera clock was about one hour ahead (not changed after the end of daylight saving time on 2026-10-25): the camera stamped 16:00:58 and 16:01:37, the files were saved on the PC at 15:04:59 and 15:05:00.
- Resistance of the supply rails to GND, power off, Fluke 115 (EUR), measured at the oscilloscope probing points, + on the rail:

| Board | 3V3–GND | VDDCORE–GND |
| --- | --- | --- |
| PCB1 (with CPU-Y) | 5.894 kΩ | 45.40 kΩ |
| PCB2 (with CPU-X) | 6.006 kΩ | 45.79 kΩ |

No short circuit; the two boards differ by 1.9 % (3V3) and 0.9 % (VDDCORE). No reference values were measured before the handover. This check is not part of the protocol; it was done before the first power-on after resoldering.
