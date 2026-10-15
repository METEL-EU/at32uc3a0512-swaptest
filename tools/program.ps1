param(
    [Parameter(Mandatory = $true)][string]$OutDir,   # configuration folder, e.g. REG_PCB2-Y
    [Parameter(Mandatory = $true)][string]$Bin,      # binary to program
    [string]$Atprogram = 'C:\Program Files (x86)\Atmel\Studio\7.0\atbackend\atprogram.exe'
)
# Programs the binary over JTAG: chiperase + program from 0x80000000 (chiperase does not erase the user page).
# Output in $OutDir\program.txt: time, SHA-256 of the binary, command, exit code and output of atprogram.
# The content is verified separately by readback (readback.ps1). Exit code 0 = programmed, 1 = error.
$OutDir = $ExecutionContext.SessionState.Path.GetUnresolvedProviderPathFromPSPath($OutDir)   # absolute path, the script changes the current folder
$Bin = $ExecutionContext.SessionState.Path.GetUnresolvedProviderPathFromPSPath($Bin)   # absolute path, the script changes the current folder
$ErrorActionPreference = 'Continue'
$env:TEMP = 'C:\Windows\Temp'; $env:TMP = 'C:\Windows\Temp'   # atprogram fails with diacritics in the TEMP path and in -f
New-Item -ItemType Directory -Force -Path $OutDir | Out-Null
$tmp = Join-Path $env:TEMP 'swaptest_program.bin'
Copy-Item -LiteralPath $Bin -Destination $tmp -Force
$h1 = (Get-FileHash -Algorithm SHA256 -LiteralPath $Bin).Hash
$h2 = (Get-FileHash -Algorithm SHA256 -LiteralPath $tmp).Hash
$report = New-Object System.Collections.Generic.List[string]
$report.Add(('program {0:yyyy-MM-dd HH:mm:ss} bin={1} sha256={2}' -f (Get-Date), (Split-Path $Bin -Leaf), $h1.ToLower()))
$ok = $false
if ($h1 -ne $h2) {
    $report.Add('RESULT ERROR: copy of the binary differs -> nothing programmed')
} else {
    $cmd = @('-t', 'jtagice3', '-i', 'jtag', '-d', 'at32uc3a0512', 'chiperase', 'program', '--format', 'bin', '-o', '0x80000000', '-f', $tmp)
    $report.Add('atprogram ' + ($cmd -join ' '))
    $rc = $null
    Push-Location (Split-Path $Atprogram)
    try { $o = & $Atprogram @cmd 2>&1 | ForEach-Object { "$_" }; $rc = $LASTEXITCODE }
    catch { $o = @("$_") }
    finally { Pop-Location }
    foreach ($l in $o) { $report.Add('  ' + $l) }
    $report.Add('exit=' + $rc)
    $ok = ($rc -eq 0)
    $report.Add('RESULT ' + $(if ($ok) { 'programmed, verify by readback (readback.ps1)' } else { 'programming ERROR' }))
}
Remove-Item $tmp -Force -ErrorAction SilentlyContinue
[IO.File]::WriteAllLines((Join-Path $OutDir 'program.txt'), [string[]]$report)
$report
exit $(if ($ok) { 0 } else { 1 })
