param(
    [Parameter(Mandatory = $true)][string]$Out,      # zaklad jmena, napr. REG_PCB1-X\uart -> uart.raw, uart.log
    [string]$Port = 'COM6',
    [int]$Baud = 115200,
    [double]$Seconds = 1800
)
# Pasivni zaznam UART: nic se neposila, DTR/RTS vypnute. Konci po $Seconds nebo ukoncenim procesu.
# $Out.raw = surova data beze zmeny, $Out.log = radky s absolutnim casem prijmu prvniho znaku
# "[HH:mm:ss.fff] text" (netisknutelne bajty jako <xx>) a "# EVT" radky udalosti linky
# (ErrorReceived/PinChanged, napr. break pri vypnuti napajeni).
$raw = [IO.File]::Open("$Out.raw", 'CreateNew', 'Write')
$log = New-Object IO.StreamWriter("$Out.log", $false, [Text.Encoding]::ASCII)
$log.AutoFlush = $true
$sp = New-Object IO.Ports.SerialPort $Port, $Baud, 'None', 8, 'One'
$sp.Handshake = 'None'; $sp.DtrEnable = $false; $sp.RtsEnable = $false
$sp.ReadBufferSize = 1048576
$sp.Open()
Register-ObjectEvent -InputObject $sp -EventName ErrorReceived -SourceIdentifier SpErr | Out-Null
Register-ObjectEvent -InputObject $sp -EventName PinChanged -SourceIdentifier SpPin | Out-Null
$t0 = Get-Date
$log.WriteLine('# capture start {0:yyyy-MM-dd HH:mm:ss.fff} port={1} baud={2} 8N1 pid={3}', $t0, $Port, $Baud, $PID)
$sw = [Diagnostics.Stopwatch]::StartNew()
$line = New-Object Text.StringBuilder
$lineStart = $null
$total = 0
function Stamp([double]$s) { ($t0.AddSeconds($s)).ToString('HH:mm:ss.fff') }
try {
    while ($sw.Elapsed.TotalSeconds -lt $Seconds) {
        foreach ($e in @(Get-Event -ErrorAction SilentlyContinue)) {
            $log.WriteLine('# EVT {0} {1} {2}', (Stamp $sw.Elapsed.TotalSeconds), $e.SourceIdentifier, $e.SourceEventArgs.EventType)
            Remove-Event -EventIdentifier $e.EventIdentifier
        }
        $n = $sp.BytesToRead
        if ($n -le 0) { Start-Sleep -Milliseconds 5; continue }
        $buf = New-Object byte[] $n
        $r = $sp.Read($buf, 0, $n)
        $now = $sw.Elapsed.TotalSeconds
        $raw.Write($buf, 0, $r); $raw.Flush()
        $total += $r
        for ($i = 0; $i -lt $r; $i++) {
            $b = $buf[$i]
            if ($null -eq $lineStart -and $b -ne 13 -and $b -ne 10) { $lineStart = $now }
            if ($b -eq 10) {
                if ($null -eq $lineStart) { $lineStart = $now }
                $log.WriteLine(('[{0}] {1}' -f (Stamp $lineStart), $line.ToString()))
                [void]$line.Clear(); $lineStart = $null
            } elseif ($b -ne 13) {
                if ($b -ge 32 -and $b -lt 127) { [void]$line.Append([char]$b) } else { [void]$line.AppendFormat('<{0:x2}>', $b) }
            }
        }
    }
} finally {
    if ($line.Length -gt 0) { $log.WriteLine(('[{0}] {1}' -f (Stamp $lineStart), $line.ToString())) }
    $log.WriteLine('# capture end {0:yyyy-MM-dd HH:mm:ss.fff} bytes={1}', (Get-Date), $total)
    Unregister-Event SpErr; Unregister-Event SpPin
    $sp.Close(); $raw.Close(); $log.Close()
}
