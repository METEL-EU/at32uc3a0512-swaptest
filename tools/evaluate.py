"""Evaluation of the UART record of the final firmware for one configuration (board + CPU).

Usage:
    python -I -B evaluate.py <uart.log> --sn <CPU SN> --board <Board ID> [--crc fb47ddb9] [--timeout 142]

<uart.log> is the output of capture.ps1 ("[HH:mm:ss.fff] text"). A run = from "=== START ===" to the "RESULT;..." line.
Every run is checked for:
  - validity: CPU SN and Board ID in START, FINISH and RESULT match the expected pair,
    CRC32 of the flash image (ID block in START and FINISH, fw_crc, fw_crc_end) == --crc, valid crc of the RESULT line;
  - result: FAIL = RESULT with at least 1 error, PASS = RESULT, 100 cycles, 0 errors,
    INCOMPLETE = no RESULT within --timeout s from START (or another START came first).
A run that did not start (no output after power-on) cannot be recognised in the record; the operator records it.
Configuration result: FAIL = 3 of 3 valid runs FAIL, PASS = 3 of 3 PASS, NOT COMPLETE = fewer than
3 valid runs with the same result, otherwise INCONSISTENT.
Read only; the output goes to stdout.
"""
import argparse
import re
import zlib

ap = argparse.ArgumentParser()
ap.add_argument("log")
ap.add_argument("--sn", required=True)
ap.add_argument("--board", required=True)
ap.add_argument("--crc", default="fb47ddb9")
ap.add_argument("--timeout", type=float, default=142.0)
ap.add_argument("--runs", type=int, default=3)
a = ap.parse_args()

LINE = re.compile(r"^\[(\d\d):(\d\d):(\d\d\.\d{3})\] ?(.*)$")


def secs(m):
    return int(m.group(1)) * 3600 + int(m.group(2)) * 60 + float(m.group(3))


runs, run, prev = [], None, None
for raw in open(a.log, encoding="ascii", errors="replace"):
    m = LINE.match(raw.rstrip("\n"))
    if not m:
        continue
    t, text = secs(m), m.group(4)
    if prev is not None and t < prev - 43200:  # crossing midnight
        t += 86400
    prev = t
    if text.startswith("=== START ==="):
        run = {"t0": t, "clock": "%s:%s:%s" % (m.group(1), m.group(2), m.group(3)),
               "start": {}, "finish": {}, "in_finish": False, "iters": 0, "result": None, "t_res": None, "exc": 0}
        runs.append(run)
        continue
    if run is None:
        continue
    if text.startswith("=== FINISH ==="):
        run["in_finish"] = True
    elif text.startswith("ID "):
        for k, v in re.findall(r"(\w+)=(\S+)", text):
            run["finish" if run["in_finish"] else "start"].setdefault(k, v)
    elif text.startswith("Summary iter="):
        run["iters"] += 1
    elif "!!! EXCEPTION !!!" in text:
        run["exc"] += 1
    elif text.startswith("RESULT;") and run["result"] is None:
        run["result"], run["t_res"] = text, t

print(f"record: {a.log}")
print(f"expected: sn={a.sn} board={a.board} crc={a.crc}")
results = []
for k, r in enumerate(runs, 1):
    head = f"run {k}: START {r['clock']}"
    if r["result"] is None or r["t_res"] - r["t0"] > a.timeout:
        print(f"{head} -> INCOMPLETE (cycles {r['iters']}, exceptions {r['exc']})")
        results.append(("INCOMPLETE", False))
        continue
    body, crc = r["result"].rsplit(";crc=", 1)
    f = dict(x.split("=", 1) for x in body.split(";")[1:] if "=" in x)
    s, e = r["start"], r["finish"]
    checks = {
        "crc RESULT": f"{zlib.crc32(body.encode()) & 0xffffffff:08x}" == crc.strip(),
        "SN": s.get("chip_sn") == e.get("chip_sn") == f.get("sn") == a.sn,
        "Board ID": s.get("board_id") == e.get("board_id") == f.get("board") == a.board,
        "image CRC32": s.get("image_crc32") == e.get("image_crc32") == "0x" + a.crc
                        and f.get("fw_crc") == f.get("fw_crc_end") == a.crc,
    }
    valid = all(checks.values())
    n, cyc, errs, refs = int(f.get("N", -1)), int(f.get("cycles", -2)), int(f.get("errors", -1)), int(f.get("ref", -1))
    if errs + refs >= 1:
        verdict = "FAIL"
    elif cyc == n == r["iters"] and errs == 0 and refs == 0:
        verdict = "PASS"
    else:
        verdict = "INCOMPLETE"
    if f.get("verdict") != verdict and verdict != "INCOMPLETE":
        verdict = "MISMATCH(" + str(f.get("verdict")) + ")"
    bad = [name for name, ok in checks.items() if not ok]
    print(f"{head} -> {verdict}  {'valid' if valid else 'INVALID: ' + ', '.join(bad)}")
    print(f"    duration {r['t_res'] - r['t0']:.1f} s, cycles {cyc}/{n}, errors {errs}/{f.get('checks')} "
          f"(min {f.get('min')}, max {f.get('max')}, failed cycles {f.get('failcyc')}, first {f.get('first')}), "
          f"ref {refs}, rcause {f.get('rcause')}, boot_tmo {f.get('boot_tmo')}, exceptions {r['exc']}")
    print(f"    fcpu_meas {s.get('fcpu_meas')}, osc0rdy_wait {s.get('osc0rdy_wait')}, lock0_wait {s.get('lock0_wait')}, "
          f"oscctrl0 {s.get('oscctrl0')}, fcr {s.get('fcr')}")
    results.append((verdict, valid))

valid = [v for v, ok in results if ok]
print()
print(f"runs with START: {len(runs)}, valid: {len(valid)}, results: {[v for v, _ in results]}")
if len(valid) == a.runs and all(v == "FAIL" for v in valid):
    conf = "FAIL"
elif len(valid) == a.runs and all(v == "PASS" for v in valid):
    conf = "PASS"
elif len(valid) < a.runs and all(v in ("FAIL", "PASS") for v in valid) and len(set(valid)) <= 1:
    conf = f"NOT COMPLETE ({len(valid)} of {a.runs} valid runs, invalid {len(results) - len(valid)})"
else:
    conf = "INCONSISTENT"
print(f"CONFIGURATION RESULT: {conf}")
