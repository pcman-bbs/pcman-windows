# Wire-format checks for synthetic Big5 fixture data; never log payloads.
function Test-Sgr66Big5Trail([int]$Value) {
    return ($Value -ge 0x40 -and $Value -le 0x7e) -or ($Value -ge 0xa1 -and $Value -le 0xfe)
}

function Get-Sgr66ControlToken([byte[]]$Bytes, [int]$Start) {
    $count = $Bytes.Length
    if ($Start -ge $count -or $Bytes[$Start] -notin @(0x1b, 0x15)) { return $null }
    if ($Start + 1 -ge $count) {
        return [pscustomobject]@{ Next = $count; Kind = 'Incomplete'; Has66 = $false }
    }
    $type = $Bytes[$Start + 1]
    if ($Bytes[$Start] -eq 0x15 -and $type -ne 0x5b) { return $null }
    if ($type -eq 0x5b) {
        $numeric = $true
        for ($i = $Start + 2; $i -lt $count; $i++) {
            $value = $Bytes[$i]
            if ($value -ge 0x40 -and $value -le 0x7e) {
                $kind = 'Control'
                $has66 = $false
                if ($numeric -and $value -eq 0x6d) {
                    $kind = 'Sgr'
                    $parameters = [Text.Encoding]::ASCII.GetString($Bytes, $Start + 2, $i - $Start - 2)
                    foreach ($parameter in $parameters.Split(';')) {
                        $number = 0
                        if ($parameter -ne '' -and (-not [int]::TryParse($parameter, [ref]$number) -or $number -gt 32767)) {
                            $kind = 'Control'; $has66 = $false; break
                        }
                        if ($number -eq 66) { $has66 = $true }
                    }
                }
                return [pscustomobject]@{ Next = $i + 1; Kind = $kind; Has66 = $has66 }
            }
            if (($value -lt 0x30 -or $value -gt 0x39) -and $value -ne 0x3b) { $numeric = $false }
        }
        return [pscustomobject]@{ Next = $count; Kind = 'Incomplete'; Has66 = $false }
    }
    if ($type -in @(0x5d, 0x50, 0x5e, 0x5f, 0x58)) {
        for ($i = $Start + 2; $i -lt $count; $i++) {
            if (($type -eq 0x5d -and $Bytes[$i] -eq 7) -or $Bytes[$i] -in @(0x18, 0x1a)) {
                return [pscustomobject]@{ Next = $i + 1; Kind = 'Control'; Has66 = $false }
            }
            if ($Bytes[$i] -eq 0x1b -and $i + 1 -lt $count -and $Bytes[$i + 1] -eq 0x5c) {
                return [pscustomobject]@{ Next = $i + 2; Kind = 'Control'; Has66 = $false }
            }
        }
        return [pscustomobject]@{ Next = $count; Kind = 'Incomplete'; Has66 = $false }
    }
    # Other ESC commands can contain intermediate bytes before their final.
    $i = $Start + 1
    while ($i -lt $count -and $Bytes[$i] -ge 0x20 -and $Bytes[$i] -le 0x2f) { $i++ }
    if ($i -ge $count) { return [pscustomobject]@{ Next = $count; Kind = 'Incomplete'; Has66 = $false } }
    return [pscustomobject]@{ Next = $i + 1; Kind = 'Control'; Has66 = $false }
}

function Get-Sgr66PasteReport([AllowEmptyCollection()][byte[]]$Bytes) {
    # Remove wire-level Telnet negotiations before interpreting picture bytes.
    $payload = [Collections.Generic.List[byte]]::new()
    $incomplete = $false
    for ($i = 0; $i -lt $Bytes.Length;) {
        if ($Bytes[$i] -ne 255) { $payload.Add($Bytes[$i]); $i++; continue }
        if ($i + 1 -ge $Bytes.Length) { $incomplete = $true; break }
        $command = $Bytes[$i + 1]
        if ($command -eq 255) { $payload.Add(255); $i += 2; continue }
        if ($command -in @(251, 252, 253, 254)) {
            if ($i + 2 -ge $Bytes.Length) { $incomplete = $true; break }
            $i += 3; continue
        }
        if ($command -eq 250) {
            $i += 2; $ended = $false
            while ($i -lt $Bytes.Length) {
                if ($Bytes[$i] -eq 255) {
                    if ($i + 1 -ge $Bytes.Length) { break }
                    if ($Bytes[$i + 1] -eq 240) { $i += 2; $ended = $true; break }
                    if ($Bytes[$i + 1] -eq 255) { $i += 2; continue }
                }
                $i++
            }
            if (-not $ended) { $incomplete = $true; break }
            continue
        }
        $i += 2
    }

    $picture = $payload.ToArray()
    $has66 = $false; $hasPicture = $false; $split = $false; $invalidBig5 = $false
    $textBytes = 0; $sgrCount = 0
    for ($i = 0; $i -lt $picture.Length;) {
        $token = Get-Sgr66ControlToken $picture $i
        if ($null -ne $token) {
            if ($token.Kind -eq 'Incomplete') { $incomplete = $true }
            if ($token.Kind -eq 'Sgr') { $sgrCount++ }
            if ($token.Has66) { $has66 = $true }
            $i = $token.Next; continue
        }
        $lead = $picture[$i]
        if ($lead -ge 0x81 -and $lead -le 0xfe) {
            $j = $i + 1
            $between = 0
            while ($j -lt $picture.Length) {
                $middleToken = Get-Sgr66ControlToken $picture $j
                if ($null -eq $middleToken -or $middleToken.Kind -ne 'Sgr') { break }
                if ($middleToken.Has66) { $has66 = $true }
                $sgrCount++; $between++; $j = $middleToken.Next
            }
            if ($j -lt $picture.Length -and (Test-Sgr66Big5Trail $picture[$j])) {
                if ($between -gt 0) { $split = $true }
                elseif ($lead -eq 0xa4 -and $picture[$j] -eq 0xa4) { $hasPicture = $true }
                # Consume BOTH bytes. The trail is never reconsidered as a lead.
                $textBytes += 2; $i = $j + 1; continue
            }
            if ($j -ge $picture.Length) { $incomplete = $true }
            else { $invalidBig5 = $true }
        }
        elseif ($lead -ge 0x80) { $invalidBig5 = $true }
        if ($lead -ge 0x20 -and $lead -ne 0x7f) { $textBytes++ }
        $i++
    }
    $status = 'Passed'
    if ($textBytes -eq 0 -and $sgrCount -eq 0 -and $picture.Length -eq 0) { $status = 'NoData' }
    elseif ($split) { $status = 'SplitBig5' }
    elseif ($incomplete) { $status = 'Incomplete' }
    elseif ($invalidBig5) { $status = 'InvalidBig5' }
    elseif ($textBytes -eq 0 -and $sgrCount -eq 0) { $status = 'NoData' }
    elseif (-not $has66) { $status = 'MissingSgr66' }
    elseif (-not $hasPicture) { $status = 'MissingPicture' }
    return [pscustomobject]@{
        Passed = $status -eq 'Passed'; Status = $status
        ByteCount = $Bytes.Length; TextByteCount = $textBytes
        HasSgr66 = $has66; HasIntactPicture = $hasPicture; HasSplitBig5 = $split
    }
}

function Read-Sgr66PasteReport(
    $Stream,
    [ValidateRange(100,10000)][int]$TimeoutMilliseconds = 2000,
    [ValidateRange(1,65536)][int]$MaxBytes = 65536
) {
    $received = [IO.MemoryStream]::new()
    $clock = [Diagnostics.Stopwatch]::StartNew()
    $buffer = [byte[]]::new(4096)
    try {
        while ($clock.ElapsedMilliseconds -lt $TimeoutMilliseconds) {
            if ($Stream.DataAvailable) {
                $count = $Stream.Read($buffer, 0, $buffer.Length)
                if ($count -le 0) { break }
                if ($received.Length + $count -gt $MaxBytes) { throw 'Paste capture exceeded the receive byte limit.' }
                $received.Write($buffer, 0, $count)
            }
            else {
                # TCP has no paste-message boundary. Keep the bounded receive
                # window open instead of treating a temporary pause as EOF.
                Start-Sleep -Milliseconds 20
            }
        }
        return Get-Sgr66PasteReport ($received.ToArray())
    }
    finally { $received.Dispose(); $clock.Stop() }
}
