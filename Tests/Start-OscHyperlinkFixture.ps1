# Loopback-only manual test for OSC 8 links and DEC2026 presentation.
param([int]$Port = 22328)
$ErrorActionPreference = 'Stop'
$listener = [Net.Sockets.TcpListener]::new([Net.IPAddress]::Loopback, $Port)
$client = $null
$esc = [char]27
$st = "${esc}\"
$close = "${esc}]8;;${st}"
function Send-Bytes([string]$Text) {
    $bytes = [Text.Encoding]::ASCII.GetBytes($Text)
    $script:stream.Write($bytes, 0, $bytes.Length)
    $script:stream.Flush()
}
function Link([string]$Url, [string]$Label) { return "${esc}]8;;${Url}${st}${Label}${close}" }
try {
    $listener.Start()
    Write-Output "Within 60 seconds, open a NEW PCMan Telnet connection: telnet://127.0.0.1:$Port"
    Write-Output 'Do not reuse a saved site with credentials/macros. This fixture has no login.'
    $deadline = [DateTime]::UtcNow.AddSeconds(60)
    while (-not $listener.Pending()) {
        if ([DateTime]::UtcNow -ge $deadline) { throw 'No test client connected within 60 seconds.' }
        Start-Sleep -Milliseconds 100
    }
    $client = $listener.AcceptTcpClient()
    $client.NoDelay = $true
    $script:stream = $client.GetStream()
    Send-Bytes "${esc}[0m${esc}[2J${esc}[1;1HOSC 8 loopback test - no login"
    # Deliberately send every OSC/ST byte separately.
    $example = "${esc}[3;1H" + (Link 'https://example.com/?pcman=osc8' 'LABEL - right-click to copy target')
    foreach ($c in $example.ToCharArray()) { Send-Bytes ([string]$c); Start-Sleep -Milliseconds 10 }
    Send-Bytes ("${esc}[5;1H" + (Link 'javascript:alert(1)' 'https://visible.example/ (blocked target)'))
    Send-Bytes ("${esc}[7;1H" + (Link 'https://example.com/?pcman=override' 'https://visible.example/ (explicit target wins)'))
    Write-Output 'A: Check cursor/underline, copied targets, and blocked URL. Script never opens any URL.'
    $null = Read-Host 'Press Enter HERE to start the OLD/NEW synchronized target test'
    Send-Bytes ("${esc}[9;1H${esc}[2K" + (Link 'https://example.com/?frame=old' 'OLD LABEL'))
    $null = Read-Host 'Copy the OLD target; press Enter HERE to begin a 1-second sync block'
    Send-Bytes ("${esc}[?2026h${esc}[9;1H${esc}[2K" + (Link 'https://example.com/?frame=new' 'NEW LABEL'))
    Write-Output 'B: During sync, OLD remains visible and hit testing is disabled; after end, NEW target becomes clickable.'
    Start-Sleep -Milliseconds 1000
    Send-Bytes "${esc}[?2026l"
    $null = Read-Host 'Verify NEW copied target; press Enter HERE to scroll the linked rows'
    Send-Bytes ("${esc}[24;1H" + ("`r`n" * 5))
    Write-Output 'C: Visible links must move with their labels. Use local scrollback to inspect earlier rows.'
    $null = Read-Host 'Press Enter HERE to disconnect with an unfinished OSC, then rerun the script to test reconnect'
    Send-Bytes "${esc}]8;;unfinished"
}
finally {
    if ($client) { $client.Dispose() }
    $listener.Stop()
}
