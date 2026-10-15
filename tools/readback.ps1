param(
    [Parameter(Mandatory = $true)][string]$OutDir,   # configuration folder, e.g. REG_PCB1-X
    [Parameter(Mandatory = $true)][string]$Bin,      # binary the flash is compared with
    [string]$Atprogram = 'C:\Program Files (x86)\Atmel\Studio\7.0\atbackend\atprogram.exe'
)
# Readback over JTAG (read only, nothing is written): info, 512 KB flash, user page, FLASHC registers.
# Output in $OutDir: info.txt, flash.bin, userpage.bin, flashc.bin and readback.txt with the comparison:
#   SHA-256 of flash from 0x80000000 over the length of the binary == SHA-256 of the binary, rest of flash 0xFF.
# Every atprogram session resets the CPU at its end. Exit code 0 = RESULT OK, 1 = mismatch or read error.
$OutDir = $ExecutionContext.SessionState.Path.GetUnresolvedProviderPathFromPSPath($OutDir)   # absolute path, the script changes the current folder
$Bin = $ExecutionContext.SessionState.Path.GetUnresolvedProviderPathFromPSPath($Bin)   # absolute path, the script changes the current folder
$ErrorActionPreference = 'Continue'
$env:TEMP = 'C:\Windows\Temp'; $env:TMP = 'C:\Windows\Temp'   # atprogram fails with diacritics in the TEMP path
New-Item -ItemType Directory -Force -Path $OutDir | Out-Null
$base = @('-t', 'jtagice3', '-i', 'jtag', '-d', 'at32uc3a0512')
$reads = @(
    @{ name = 'flash.bin';    addr = '0x80000000'; size = '0x80000' },
    @{ name = 'userpage.bin'; addr = '0x80800000'; size = '0x200' },
    @{ name = 'flashc.bin';   addr = '0xFFFE1400'; size = '0x14' }
)
$report = New-Object System.Collections.Generic.List[string]
function Note([string]$m) { $report.Add($m); Write-Output $m }
function Hex([byte[]]$h) { -join ($h | ForEach-Object { $_.ToString('x2') }) }
function U32([byte[]]$b, [int]$o) { '0x{0:x2}{1:x2}{2:x2}{3:x2}' -f $b[$o], $b[$o + 1], $b[$o + 2], $b[$o + 3] }   # big-endian

Note ('readback {0:yyyy-MM-dd HH:mm:ss} bin={1}' -f (Get-Date), (Split-Path $Bin -Leaf))
$ok = $true
Push-Location (Split-Path $Atprogram)
try {
    $o = & $Atprogram @base info 2>&1 | ForEach-Object { "$_" }
    Note "info exit=$LASTEXITCODE"
    [IO.File]::WriteAllLines((Join-Path $OutDir 'info.txt'), [string[]]$o)
    foreach ($r in $reads) {
        $tmp = Join-Path $env:TEMP ('swaptest_' + $r.name)
        if (Test-Path $tmp) { Remove-Item $tmp -Force }
        $o = & $Atprogram @base read -o $r.addr -s $r.size --format bin -f $tmp 2>&1 | ForEach-Object { "$_" }
        Note ('read {0} {1} size {2} exit={3}' -f $r.name, $r.addr, $r.size, $LASTEXITCODE)
        if (-not (Test-Path $tmp)) { Note ('  READ FAILED: ' + ($o -join ' | ')); $ok = $false; continue }
        Move-Item $tmp (Join-Path $OutDir $r.name) -Force
    }
} finally { Pop-Location }

$sha = [Security.Cryptography.SHA256]::Create()
$b = [IO.File]::ReadAllBytes($Bin)
$hb = Hex $sha.ComputeHash($b)
Note ('bin    len={0} sha256={1}' -f $b.Length, $hb)
$fp = Join-Path $OutDir 'flash.bin'
if (Test-Path $fp) {
    $f = [IO.File]::ReadAllBytes($fp)
    $hf = Hex $sha.ComputeHash($f, 0, [Math]::Min($b.Length, $f.Length))
    $tail = 0; for ($i = $b.Length; $i -lt $f.Length; $i++) { if ($f[$i] -ne 0xFF) { $tail++ } }
    Note ('flash  0x80000000..0x{0:X8} sha256={1}' -f (0x80000000 + $b.Length - 1), $hf)
    Note ('flash  rest up to 0x8007FFFF: bytes other than 0xFF = {0}' -f $tail)
    if ($f.Length -ne 0x80000 -or $hf -ne $hb -or $tail -ne 0) { $ok = $false }
} else { $ok = $false }
$up = Join-Path $OutDir 'userpage.bin'
if (Test-Path $up) { Note ('userpage sha256={0}' -f (Hex $sha.ComputeHash([IO.File]::ReadAllBytes($up)))) }
$fc = Join-Path $OutDir 'flashc.bin'
if (Test-Path $fc) {
    $c = [IO.File]::ReadAllBytes($fc)
    Note ('FLASHC FCR={0} FSR={1} FGPFRHI={2} FGPFRLO={3}' -f (U32 $c 0), (U32 $c 8), (U32 $c 12), (U32 $c 16))
}
Note ('RESULT ' + $(if ($ok) { 'OK: flash matches the binary' } else { 'MISMATCH or read error -> STOP' }))
[IO.File]::WriteAllLines((Join-Path $OutDir 'readback.txt'), [string[]]$report)
exit $(if ($ok) { 0 } else { 1 })
