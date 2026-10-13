param([Parameter(Mandatory = $true)][string]$Dir)
# Zapise $Dir\SHA256SUMS ve formatu sha256sum ("<hash> *<cesta>", LF) pro vsechny soubory ve slozce
# a podslozkach krome SHA256SUMS. Overeni: sha256sum -c SHA256SUMS (v Git Bash).
$root = (Resolve-Path $Dir).Path.TrimEnd('\')
$lines = Get-ChildItem -LiteralPath $root -File -Recurse | Where-Object { $_.Name -ne 'SHA256SUMS' } | Sort-Object FullName | ForEach-Object {
    '{0} *{1}' -f (Get-FileHash -Algorithm SHA256 -LiteralPath $_.FullName).Hash.ToLower(), $_.FullName.Substring($root.Length + 1).Replace('\', '/')
}
[IO.File]::WriteAllText((Join-Path $root 'SHA256SUMS'), (($lines -join "`n") + "`n"), (New-Object Text.UTF8Encoding $false))
$lines
