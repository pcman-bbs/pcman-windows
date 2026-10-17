# Script-level regression tests; only synthetic, non-sensitive fixture bytes.
$ErrorActionPreference = 'Stop'
$checks = 0
function Assert-Check([bool]$Condition, [string]$Description) {
    $script:checks++
    if (-not $Condition) { throw "FAIL: $Description" }
}

. (Join-Path $PSScriptRoot 'Sgr66FixtureHelpers.ps1')

$esc = [char]27
$middle = [string][char]0x4e2d
$cp950 = [Text.Encoding]::GetEncoding(950)
$valid = $cp950.GetBytes("${esc}[31;66;32m${middle}${esc}[mX")
Assert-Check (Get-Sgr66PasteReport $valid).Passed 'valid Big5 followed by reset and ASCII must pass'

$ctrlU = [char]21
foreach ($prefix in @([string]$esc, [string]$ctrlU)) {
    foreach ($suffix in @('X', '[', '@', '~', ' ', '', "`r`n")) {
        $bytes = $cp950.GetBytes("${prefix}[1;37;66;33m${middle}${prefix}[m${suffix}")
        $report = Get-Sgr66PasteReport $bytes
        Assert-Check $report.Passed 'valid ESC/Ctrl-U picture and reset suffix'
        Assert-Check (-not $report.HasSplitBig5) 'Big5 trail is not reinterpreted as a lead'
    }
    foreach ($lead in @(0x81,0xa4,0xfe)) {
        foreach ($trail in @(0x40,0x5b,0x5c,0x7e,0xa1,0xa4,0xfe)) {
            $pair = [string][char]$lead + [char]$trail
            $raw = [Text.Encoding]::GetEncoding(28591).GetBytes("${prefix}[66m${pair}${prefix}[mX")
            Assert-Check (-not (Get-Sgr66PasteReport $raw).HasSplitBig5) 'intact byte pair consumed once at known character boundary'
        }
    }
}
$legacy = [byte[]](@($cp950.GetBytes("${esc}[1;37m")) + @(0xa4) + @($cp950.GetBytes("${esc}[33m")) + @(0xa4))
$cases = @(
    @{ Bytes = [byte[]]@(); Status = 'NoData' },
    @{ Bytes = [byte[]]@(255,251,24,255,253,3); Status = 'NoData' },
    @{ Bytes = [byte[]]@(255,250,31,0,80,0,24,255,240); Status = 'NoData' },
    @{ Bytes = $cp950.GetBytes("`r`n"); Status = 'NoData' },
    @{ Bytes = $legacy; Status = 'SplitBig5' },
    @{ Bytes = $cp950.GetBytes($middle); Status = 'MissingSgr66' },
    @{ Bytes = $cp950.GetBytes("${esc}[166m${middle}"); Status = 'MissingSgr66' },
    @{ Bytes = $cp950.GetBytes("${esc}[?66m${middle}"); Status = 'MissingSgr66' },
    @{ Bytes = $cp950.GetBytes("${esc}[66mPICTURE"); Status = 'MissingPicture' },
    @{ Bytes = $cp950.GetBytes("${esc}[66;"); Status = 'Incomplete' },
    @{ Bytes = [byte[]](@($cp950.GetBytes("${esc}[66m")) + @(0xa4)); Status = 'Incomplete' },
    @{ Bytes = [byte[]](@($valid) + @(255)); Status = 'Incomplete' },
    @{ Bytes = [byte[]](@($valid) + @(0x80)); Status = 'InvalidBig5' }
)
foreach ($case in $cases) {
    $report = Get-Sgr66PasteReport $case.Bytes
    Assert-Check ($report.Status -eq $case.Status) ("diagnostic status: " + $case.Status)
    Assert-Check (-not $report.Passed) 'incomplete/invalid/absent paste is not a pass'
}
foreach ($introducer in @(']', 'P', '^', '_', 'X')) {
    $hidden = "${esc}${introducer}0;${esc}[66m${middle}${esc}\"
    Assert-Check ((Get-Sgr66PasteReport ($cp950.GetBytes($hidden + $middle))).Status -eq 'MissingSgr66') 'hidden SGR66 cannot satisfy visible paste test'
    Assert-Check ((Get-Sgr66PasteReport ($cp950.GetBytes($hidden + "${esc}[66mX"))).Status -eq 'MissingPicture') 'hidden picture cannot satisfy visible paste test'
}
Assert-Check (Get-Sgr66PasteReport ($cp950.GetBytes("${esc}[066m${middle}"))).Passed 'numeric SGR parameter with leading zeroes'
$withNegotiation = [byte[]](@(255,251,24) + @($valid) + @(255,250,31,0,80,0,24,255,240))
Assert-Check (Get-Sgr66PasteReport $withNegotiation).Passed 'Telnet negotiations separated from picture data'

function Test-TcpCapture([byte[]]$Data, [int]$Split) {
    $listener = [Net.Sockets.TcpListener]::new([Net.IPAddress]::Loopback, 0)
    $peer = $null; $accepted = $null; $writer = $null
    try {
        $listener.Start()
        $peer = [Net.Sockets.TcpClient]::new()
        $peer.Connect([Net.IPAddress]::Loopback, $listener.LocalEndpoint.Port)
        $accepted = $listener.AcceptTcpClient()
        $writer = [PowerShell]::Create()
        [void]$writer.AddScript({
            param($Stream, [byte[]]$Data, [int]$Split)
            $Stream.Write($Data, 0, $Split); $Stream.Flush()
            Start-Sleep -Milliseconds 100
            $Stream.Write($Data, $Split, $Data.Length - $Split); $Stream.Flush()
        }).AddArgument($peer.GetStream()).AddArgument($Data).AddArgument($Split)
        $pending = $writer.BeginInvoke()
        $report = Read-Sgr66PasteReport $accepted.GetStream() -TimeoutMilliseconds 350
        $null = $writer.EndInvoke($pending)
        if ($writer.HadErrors) { throw ($writer.Streams.Error | Out-String) }
        Assert-Check $report.Passed 'real TCP capture accepts delayed segmented paste'
        Assert-Check ($report.ByteCount -eq $Data.Length) 'bounded receive collects every segment, including suffix'
    }
    finally {
        if ($writer) { $writer.Dispose() }
        if ($accepted) { $accepted.Dispose() }
        if ($peer) { $peer.Dispose() }
        $listener.Stop()
    }
}
for ($split = 0; $split -le $valid.Length; $split++) { Test-TcpCapture $valid $split }
Test-TcpCapture $withNegotiation 1

$listener = [Net.Sockets.TcpListener]::new([Net.IPAddress]::Loopback, 0)
$peer = $null; $accepted = $null
try {
    $listener.Start()
    $peer = [Net.Sockets.TcpClient]::new()
    $peer.Connect([Net.IPAddress]::Loopback, $listener.LocalEndpoint.Port)
    $accepted = $listener.AcceptTcpClient()
    Assert-Check ((Read-Sgr66PasteReport $accepted.GetStream() -TimeoutMilliseconds 100).Status -eq 'NoData') 'real TCP no paste is an explicit diagnostic'
    $tooMuch = [byte[]](1..64 | ForEach-Object { 65 })
    $peer.GetStream().Write($tooMuch,0,$tooMuch.Length)
    $limited = $false
    try { $null = Read-Sgr66PasteReport $accepted.GetStream() -TimeoutMilliseconds 200 -MaxBytes 32 }
    catch { if ($_.Exception.Message -notlike '*receive byte limit*') { throw }; $limited = $true }
    Assert-Check $limited 'receive byte limit retained'
}
finally {
    if ($accepted) { $accepted.Dispose() }
    if ($peer) { $peer.Dispose() }
    $listener.Stop()
}

# Run the UNMODIFIED interactive script in an isolated runspace, providing
# only answers to Read-Host. Real loopback sockets still carry the test bytes.
function Test-FixtureFlow([string]$Name, [object[]]$Actions, [bool]$ExpectPass, [bool]$ExpectSkip, [bool]$ExpectWarning) {
    $reservation = [Net.Sockets.TcpListener]::new([Net.IPAddress]::Loopback, 0)
    $reservation.Start(); $port = $reservation.LocalEndpoint.Port; $reservation.Stop()
    $prompts = [Collections.Concurrent.BlockingCollection[string]]::new()
    $answers = [Collections.Concurrent.BlockingCollection[string]]::new()
    $runner = [PowerShell]::Create()
    $peer = $null
    try {
        [void]$runner.AddScript({
            param($Path, $Port, $Prompts, $Answers)
            $global:fixturePrompts = $Prompts; $global:fixtureAnswers = $Answers
            function Read-Host([string]$Prompt) {
                $global:fixturePrompts.Add($Prompt)
                $reply = ''
                if (-not $global:fixtureAnswers.TryTake([ref]$reply,10000)) { throw 'Timed out waiting for synthetic test answer' }
                return $reply
            }
            & $Path -Port $Port
        }).AddArgument((Join-Path $PSScriptRoot 'Start-Sgr66Fixture.ps1')).AddArgument($port).AddArgument($prompts).AddArgument($answers)
        $pending = $runner.BeginInvoke()
        for ($attempt=0; $attempt -lt 100; $attempt++) {
            $candidate = [Net.Sockets.TcpClient]::new()
            try { $candidate.Connect([Net.IPAddress]::Loopback,$port); $peer = $candidate; break }
            catch { $candidate.Dispose(); Start-Sleep -Milliseconds 20 }
        }
        if (-not $peer) { throw 'Fixture test listener did not start' }
        foreach ($action in $Actions) {
            $prompt = ''
            if (-not $prompts.TryTake([ref]$prompt,10000)) { throw "Missing fixture prompt: $Name" }
            if ($action.ContainsKey('Data')) { $peer.GetStream().Write($action.Data,0,$action.Data.Length); $peer.GetStream().Flush() }
            $answers.Add([string]$action.Reply)
        }
        $output = @($runner.EndInvoke($pending)) -join "`n"
        if ($runner.HadErrors) { throw ($runner.Streams.Error | Out-String) }
        Assert-Check ($output.Contains('PASS C:') -eq $ExpectPass) ("flow pass result: " + $Name)
        Assert-Check ($output.Contains('SKIP C:') -eq $ExpectSkip) ("flow skip result: " + $Name)
        Assert-Check (($runner.Streams.Warning.Count -gt 0) -eq $ExpectWarning) ("flow actionable warning: " + $Name)
        Assert-Check (-not $output.Contains('FIXTURE_PRIVATE_SENTINEL')) 'interactive script never prints synthetic payload'
        Assert-Check (-not (($runner.Streams.Warning | Out-String).Contains('FIXTURE_PRIVATE_SENTINEL'))) 'warnings never print synthetic payload'
        $probe = [Net.Sockets.TcpListener]::new([Net.IPAddress]::Loopback,$port)
        try { $probe.Start(); Assert-Check $true 'script releases its listener' } finally { $probe.Stop() }
        Write-Output ("Flow PASS: " + $Name)
    }
    finally {
        $runner.Stop(); $runner.Dispose()
        if ($peer) { $peer.Dispose() }
        $prompts.Dispose(); $answers.Dispose()
    }
}
$privateFixture = $cp950.GetBytes("${esc}[31;66;32m${middle}${esc}[mXFIXTURE_PRIVATE_SENTINEL")
Test-FixtureFlow 'explicit skip' @(@{Reply=''},@{Reply=''},@{Reply='S'},@{Reply=''}) $false $true $false
Test-FixtureFlow 'no paste then skip' @(@{Reply=''},@{Reply=''},@{Reply=''},@{Reply='S'},@{Reply=''}) $false $true $true
Test-FixtureFlow 'valid paste' @(@{Reply=''},@{Reply=''},@{Reply='';Data=$privateFixture},@{Reply=''}) $true $false $false
Test-FixtureFlow 'legacy paste then successful retry' @(@{Reply=''},@{Reply=''},@{Reply='';Data=$legacy},@{Reply='';Data=$valid},@{Reply=''}) $true $false $true
Test-FixtureFlow 'explicit quit' @(@{Reply=''},@{Reply=''},@{Reply='Q'}) $false $false $false
Write-Output "PASS: $checks SGR66 fixture checks"
