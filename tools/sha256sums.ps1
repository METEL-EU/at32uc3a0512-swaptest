param([Parameter(Mandatory = $true)][string]$Dir)
# Writes $Dir\SHA256SUMS in sha256sum format ("<hash> *<path>", LF) for all files in the folder
# and its subfolders except SHA256SUMS. Verification: sha256sum -c SHA256SUMS (in Git Bash).
$root = (Resolve-Path $Dir).Path.TrimEnd('\')
$lines = Get-ChildItem -LiteralPath $root -File -Recurse | Where-Object { $_.Name -ne 'SHA256SUMS' } | Sort-Object FullName | ForEach-Object {
    '{0} *{1}' -f (Get-FileHash -Algorithm SHA256 -LiteralPath $_.FullName).Hash.ToLower(), $_.FullName.Substring($root.Length + 1).Replace('\', '/')
}
[IO.File]::WriteAllText((Join-Path $root 'SHA256SUMS'), (($lines -join "`n") + "`n"), (New-Object Text.UTF8Encoding $false))
$lines
