#!/usr/bin/env python3
"""Kontrola sestaveneho firmwaru swaptest_final (jen cte soubory, nic nezapisuje).

Spousteni ze slozky 20261010_swaptest_final:
    python -I -B verify_build.py <bin_dir>
      bin_dir  slozka s test_uc3a0512.bin a test_uc3a0512.elf (napr. bin)
Potrebuje avr32-nm a avr32-objdump z WinAVR 20100110 (TOOLS) a src/placement.ld.

Kontroly (vypis je anglicky):
  1  SHA-256 a velikosti
  2  delka .bin == IMAGE_LEN (0x7FC04, z ni firmware za behu pocita CRC32)
  3  umisteni jadra testu podle src/placement.ld: 24 tabulek (data_padding_X na kotve,
     dataX = kotva + 0x200, 128 slov), 480 funkci checkXn na adresach z placement.ld
  4  obsah tabulek v obrazu: dataX[0] = 0x00000000, dataX[0x77] = 0xFFFFFFFF
  5  funkce: 20 funkci checkXn jedne tabulky bitove shodnych, refA..refX na 0x80000060 + k*0x10
  6  kriticka cteni: checkXn+0x30 'ld.w r10,r8[512]', refX+0x04 'ld.w r12,r8[476]' (adresy ld= ve vypisu)
  7  rozlozeni: kod a data mimo jadro konci pod prvni tabulkou (0x8000A000)
  8  zapisy do FLASHC: jen FCR (flashc_set_wait_state), zadny odkaz na FCMD
  9  CRC32 obrazu (zlib) = image_crc32 ve vypisu firmwaru, __VERSION__ v obrazu
 10  merici smycka v main(): posloupnost instrukci (informativne)
"""

import hashlib
import os
import re
import subprocess
import sys
import zlib

TOOLS = "C:/WinAVR-20100110/bin"
BASE = 0x80000000
IMAGE_LEN = 0x7FC04
REF0 = 0x80000060
FLASHC = 0xFFFE1400
FLASHC_REGS = {0x00: "FCR", 0x04: "FCMD", 0x08: "FSR", 0x0C: "FGPFRHI", 0x10: "FGPFRLO"}
LETTERS = "ABCDEFGHIJKLMNOPQRSTUVWX"
PLACEMENT = os.path.join(os.path.dirname(os.path.abspath(__file__)), "src", "placement.ld")

INSN_RE = re.compile(r"^\s*([0-9a-f]+):\s+((?:[0-9a-f]{2} )+)\s*\t(\S+)\s*(.*)$")
FUNC_RE = re.compile(r"^([0-9a-f]+) <([^>]+)>:$")

failures = []


def check(ok, text):
    print("  [%s] %s" % ("OK" if ok else "FAIL", text))
    if not ok:
        failures.append(text)


def run(*args):
    return subprocess.run(args, capture_output=True, text=True, check=True).stdout


def nm(elf):
    syms = {}
    for line in run(TOOLS + "/avr32-nm.exe", "-S", elf).splitlines():
        p = line.split()
        if len(p) == 4:
            syms[p[3]] = (int(p[0], 16), int(p[1], 16))
        elif len(p) == 3:
            syms[p[2]] = (int(p[0], 16), None)
    return syms


def sections(elf):
    sec = {}
    for line in run(TOOLS + "/avr32-objdump.exe", "-h", elf).splitlines():
        p = line.split()
        if len(p) >= 7 and p[0].isdigit():
            sec[p[1]] = (int(p[2], 16), int(p[3], 16), int(p[4], 16))  # size, vma, lma
    return sec


def disasm(elf):
    """{funkce: [(adresa, bajty, mnemonika, operandy)]}"""
    funcs, cur = {}, None
    for line in run(TOOLS + "/avr32-objdump.exe", "-d", elf).splitlines():
        m = FUNC_RE.match(line)
        if m:
            cur = m.group(2)
            funcs.setdefault(cur, [])
            continue
        m = INSN_RE.match(line)
        if m and cur:
            funcs[cur].append((int(m.group(1), 16), m.group(2).split(), m.group(3), m.group(4)))
    return funcs


def word(binary, addr):
    off = addr - BASE
    return int.from_bytes(binary[off:off + 4], "big") if 0 <= off <= len(binary) - 4 else None


def placement():
    """kotvy tabulek a adresy funkci check z src/placement.ld"""
    text = open(PLACEMENT, encoding="ascii").read()
    anchors = {m.group(1): int(m.group(2), 16)
               for m in re.finditer(r"\.text\.data_tables_anchor_([A-X])\s+0x([0-9a-fA-F]+)", text)}
    checks = {m.group(1): int(m.group(2), 16)
              for m in re.finditer(r"\.text\.(check[A-X]\d+)\s+0x([0-9a-fA-F]+)", text)}
    return anchors, checks


def flashc_access(funcs, binary):
    """registr s bazi FLASHC (mov imm / lddpc z cpool) -> nasledne ld/st s offsetem"""
    loads, stores = {}, {}
    for name, insns in funcs.items():
        regs = {}
        for addr, raw, mn, ops in insns:
            o = [x.strip() for x in ops.split(",")]
            val = None
            if mn == "mov" and len(o) == 2 and re.fullmatch(r"-?\d+", o[1]):
                val = int(o[1]) & 0xFFFFFFFF
            elif mn == "lddpc" and len(o) == 2:
                t = re.match(r"([0-9a-f]+)", o[1])
                val = word(binary, int(t.group(1), 16)) if t else None
            m = re.match(r"(?:--)?(r\d+|sp|lr)(?:\[(-?(?:0x[0-9a-f]+|\d+))\])?", o[0]) if o and o[0] else None
            if mn.startswith("st.") and m and m.group(1) in regs:
                off = int(m.group(2) or "0", 0)
                stores.setdefault(regs[m.group(1)] + off, set()).add(name)
            if mn.startswith("ld.") and len(o) == 2:
                mm = re.match(r"(r\d+)\[(-?(?:0x[0-9a-f]+|\d+))\]", o[1])
                if mm and mm.group(1) in regs:
                    loads.setdefault(regs[mm.group(1)] + int(mm.group(2), 0), set()).add(name)
            # cilovy registr instrukce prepise sledovanou bazi
            if o and re.fullmatch(r"r\d+", o[0]) and not mn.startswith("st.") and not mn.startswith("cp"):
                regs.pop(o[0], None)
            if val is not None and FLASHC <= val < FLASHC + 0x20:
                regs[o[0]] = val
            if "fffe1404" in "".join(raw):
                stores.setdefault(0xFFFE1404, set()).add(name + " (constant)")
    return loads, stores


def loop_parts(insns):
    """casti merici smycky jako [(mnemonika, delka)]:
       ref    = telo smycky 24x ref (od ld.w r12 pred 1. icall po brne)
       check  = telo vnitrni smycky 20x check (od ld.w r12 pred 2. icall po brne)
       outer  = zbytek vnejsi smycky pres 24 tabulek (po rjmp zpet na vnitrni smycku)"""
    icalls = [i for i, x in enumerate(insns) if x[2] == "icall"]
    if len(icalls) < 2:
        return None

    def until(start, mnem):
        return next(i for i in range(start, len(insns)) if insns[i][2] == mnem)

    r_end = until(icalls[0], "brne")
    c_end = until(icalls[1], "brne")
    o_end = until(c_end + 1, "rjmp")
    parts = {"ref": insns[icalls[0] - 1:r_end + 1],
             "check": insns[icalls[1] - 1:c_end + 1],
             "outer": insns[c_end + 1:o_end + 1]}
    return {k: ([x[2] for x in v], v[0][0], v[-1][0]) for k, v in parts.items()}


def main():
    bin_dir = sys.argv[1]
    fb, fe = os.path.join(bin_dir, "test_uc3a0512.bin"), os.path.join(bin_dir, "test_uc3a0512.elf")
    image = open(fb, "rb").read()

    print("1 SHA-256")
    for f in (fb, fe):
        d = open(f, "rb").read()
        print("  %-22s %7d B  %s" % (os.path.basename(f), len(d), hashlib.sha256(d).hexdigest().upper()))

    print("2 image length")
    check(len(image) == IMAGE_LEN, ".bin %d B (0x%X), IMAGE_LEN 0x%X" % (len(image), len(image), IMAGE_LEN))

    print("3 placement of the test core (src/placement.ld)")
    syms = nm(fe)
    anchors, checks = placement()
    check(sorted(anchors) == list(LETTERS) and len(checks) == 480,
          "placement.ld: %d table anchors, %d check functions" % (len(anchors), len(checks)))
    bad = [L for L in LETTERS if syms.get("data_padding_" + L) != (anchors[L], 0x200)
           or syms.get("data" + L) != (anchors[L] + 0x200, 0x200)]
    check(not bad, "data_padding_X at anchor (0x200 B), dataX = anchor + 0x200 (128 words)%s"
          % ((" wrong: %s" % bad) if bad else ""))
    bad = [c for c, a in checks.items() if syms.get(c, (None,))[0] != a]
    check(not bad, "480 functions checkXn at the addresses from placement.ld%s" % ((" wrong: %s" % bad[:5]) if bad else ""))
    sizes = {syms[c][1] for c in checks}
    core_end = max(a + syms[c][1] for c, a in checks.items())
    print("  test core: 0x%08X..0x%08X (tables + check functions), check function size: %s B"
          % (min(anchors.values()), core_end - 1, ",".join(str(s) for s in sorted(sizes))))
    print("  table anchors: A..K 0x%08X..0x%08X, L..X 0x%08X..0x%08X, spacing 0x5000"
          % (anchors["A"], anchors["K"], anchors["L"], anchors["X"]))
    print("  checkXn = anchor + 0x5BC + n*0x200; read address adr= is &dataX[0x77] = anchor + 0x3DC")

    print("4 table contents in the image")
    vals = {(word(image, syms["data" + L][0]), word(image, syms["data" + L][0] + 4 * 0x77)) for L in LETTERS}
    check(vals == {(0x00000000, 0xFFFFFFFF)},
          "all 24 tables: data[0] = 0x00000000, data[0x77] = 0xFFFFFFFF (expected value data[0]^data[0x77] = 0xFFFFFFFF)")

    print("5 check and ref functions")
    body = lambda s: image[syms[s][0] - BASE:syms[s][0] - BASE + syms[s][1]]
    bad = [L for L in LETTERS if len({body("check%s%d" % (L, n)) for n in range(20)}) != 1]
    check(not bad, "20 functions checkXn of each table bit-identical%s" % ((" differ: %s" % bad) if bad else ""))
    ok = all(syms.get("ref" + L) == (REF0 + 16 * k, 16) for k, L in enumerate(LETTERS))
    check(ok, "refA..refX at 0x%08X + k*0x10, size 0x10 (0x%08X..0x%08X)" % (REF0, REF0, REF0 + 24 * 16 - 1))

    print("6 critical reads")
    funcs = disasm(fe)
    crit = []
    for L in LETTERS:
        crit.append(("ref" + L, 0x04, "ld.w", "r12,r8[476]"))
        crit += [("check%s%d" % (L, n), 0x30, "ld.w", "r10,r8[512]") for n in range(20)]
    # objdump deli checkXn na podbloky (checkXn_LCFI1 ...), proto globalni mapa adres
    at = {a: (m, o) for insns in funcs.values() for a, _, m, o in insns}
    bad = [name for name, ofs, mn, ops in crit if at.get(syms[name][0] + ofs) != (mn, ops)]
    check(not bad, "refX+0x04 'ld.w r12,r8[476]', checkXn+0x30 'ld.w r10,r8[512]' (%d functions)%s"
          % (len(crit), (" missing: %s" % bad[:5]) if bad else ""))

    print("7 layout below 0x8000A000")
    sec = sections(fe)
    first = min(anchors.values())
    data_end = sec[".data"][2] + sec[".data"][0]
    for n in (".text", ".exception", ".rodata", ".data"):
        size, vma, lma = sec[n]
        print("  %-11s LMA 0x%08X..0x%08X (%d B)" % (n, lma, lma + size - 1, size))
    check(data_end <= first, ".data LMA ends 0x%08X, reserve up to 0x%08X: %d B" % (data_end - 1, first, first - data_end))
    print("  RAM: .bss up to 0x%08X, stack 0x0000F000..0x0000FFFF" % (sec[".bss"][1] + sec[".bss"][0] - 1))

    print("8 FLASHC")
    loads, stores = flashc_access(funcs, image)
    for adr in sorted(set(loads) | set(stores)):
        reg = FLASHC_REGS.get(adr - FLASHC, "+0x%X" % (adr - FLASHC))
        print("  %-8s read: %-30s write: %s" % (reg, ",".join(sorted(loads.get(adr, ()))) or "-",
                                                ",".join(sorted(stores.get(adr, ()))) or "-"))
    check(set(stores) == {FLASHC} and stores[FLASHC] == {"flashc_set_wait_state"},
          "the only write to FLASHC is FCR in flashc_set_wait_state")
    flashc_syms = sorted(s for s in syms if s.startswith("flashc_"))
    check(flashc_syms == ["flashc_set_wait_state"], "linked from flashc.c only: %s" % ", ".join(flashc_syms))

    print("9 image CRC32")
    print("  zlib.crc32(.bin) = 0x%08x (must match 'ID ... image_crc32=')" % zlib.crc32(image))
    gcc = re.search(rb"4\.3\.2[^\x00]*", image)
    print("  __VERSION__ in the image: %s" % (gcc.group(0).decode("ascii", "replace") if gcc else "not found"))

    print("10 measurement loop in main() (informative)")
    parts = loop_parts(funcs["main"])
    for k in ("ref", "check", "outer"):
        mns, a0, a1 = parts[k]
        print("  [INFO] %-5s 0x%08X..0x%08X %2d instr.: %s" % (k, a0, a1, len(mns), " ".join(mns)))

    print()
    print("RESULT: %s" % ("all OK" if not failures else "ERROR: " + "; ".join(failures)))
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main())
