param(
    [Parameter(Mandatory = $true)][string]$OutDir,   # slozka konfigurace, napr. REG_PCB2-Y
    [Parameter(Mandatory = $true)][string]$Bin,      # binarka k nahrani
    [string]$Atprogram = 'C:\Program Files (x86)\Atmel\Studio\7.0\atbackend\atprogram.exe'
)
# Nahrani binarky pres JTAG: chiperase + program od 0x80000000 (user page se chiperase nemaze).
# Vystup v $OutDir\program.txt: cas, SHA-256 binarky, prikaz, navratovy kod a vystup atprogram.
# Overeni obsahu se dela zvlast zpetnym ctenim (readback.ps1).
$OutDir = $ExecutionContext.SessionState.Path.GetUnresolvedProviderPathFromPSPath($OutDir)   # absolutni cesta, skript meni aktualni slozku
$Bin = $ExecutionContext.SessionState.Path.GetUnresolvedProviderPathFromPSPath($Bin)   # absolutni cesta, skript meni aktualni slozku
$ErrorActionPreference = 'Continue'
$env:TEMP = 'C:\Windows\Temp'; $env:TMP = 'C:\Windows\Temp'   # atprogram nesnese diakritiku v ceste TEMP ani -f
New-Item -ItemType Directory -Force -Path $OutDir | Out-Null
$tmp = Join-Path $env:TEMP 'swaptest_program.bin'
Copy-Item -LiteralPath $Bin -Destination $tmp -Force
$h1 = (Get-FileHash -Algorithm SHA256 -LiteralPath $Bin).Hash
$h2 = (Get-FileHash -Algorithm SHA256 -LiteralPath $tmp).Hash
$report = New-Object System.Collections.Generic.List[string]
$report.Add(('program {0:yyyy-MM-dd HH:mm:ss} bin={1} sha256={2}' -f (Get-Date), (Split-Path $Bin -Leaf), $h1.ToLower()))
if ($h1 -ne $h2) {
    $report.Add('RESULT ERROR: copy of the binary differs -> nothing programmed')
} else {
    $cmd = @('-t', 'jtagice3', '-i', 'jtag', '-d', 'at32uc3a0512', 'chiperase', 'program', '--format', 'bin', '-o', '0x80000000', '-f', $tmp)
    $report.Add('atprogram ' + ($cmd -join ' '))
    Push-Location (Split-Path $Atprogram)
    try { $o = & $Atprogram @cmd 2>&1 | ForEach-Object { "$_" }; $rc = $LASTEXITCODE } finally { Pop-Location }
    foreach ($l in $o) { $report.Add('  ' + $l) }
    $report.Add('exit=' + $rc)
    $report.Add('RESULT ' + $(if ($rc -eq 0) { 'programmed, verify by readback (readback.ps1)' } else { 'programming ERROR' }))
}
Remove-Item $tmp -Force -ErrorAction SilentlyContinue
[IO.File]::WriteAllLines((Join-Path $OutDir 'program.txt'), [string[]]$report)
$report
