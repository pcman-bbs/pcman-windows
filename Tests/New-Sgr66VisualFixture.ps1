# Offline CP950 fixtures. No network, clipboard access or browser requests.
$ErrorActionPreference = 'Stop'
$esc = [char]27
$middle = [string][char]0x4e2d
$letter = [string][char]0x6587
$cp950 = [Text.Encoding]::GetEncoding(950)
$directory = Join-Path $PSScriptRoot 'fixtures'
[void][IO.Directory]::CreateDirectory($directory)
$stream = [IO.MemoryStream]::new()
function Add-Text([string]$Value) {
    $bytes = $cp950.GetBytes($Value)
    $stream.Write($bytes, 0, $bytes.Length)
}
try {
    Add-Text "${esc}[0m${esc}[2J${esc}[4l${esc}[1;1HPCMan SGR 66 - offline CP950 visual check"
    Add-Text "${esc}[3;1HWHITE/YELLOW: ${esc}[1;37;66;33m${middle}${esc}[m"
    Add-Text "${esc}[4;1HRIGHT RESET: ${esc}[1;37;66;0;31m${middle}${esc}[m"
    Add-Text "${esc}[5;1HONE SHOT: ${esc}[31;66;32m${middle}${middle}${esc}[m"
    Add-Text "${esc}[6;1HBG/BLINK: ${esc}[1;44;5;37;66;0;43;31m${letter}${esc}[m"
    Add-Text "${esc}[7;1HASCII NEXT: ${esc}[31;66;32mXY${esc}[m"
    Add-Text "${esc}[8;1HLEGACY PAIR: ${esc}[1;37m"
    $pair = $cp950.GetBytes($middle)
    $stream.WriteByte($pair[0]); Add-Text "${esc}[33m"; $stream.WriteByte($pair[1])
    Add-Text "${esc}[m${esc}[9;1HBLOCK SPACES: ${esc}[41m  ${esc}[m"
    Add-Text "${esc}[10;1HNORMAL: plain text after reset"
    Add-Text "${esc}[12;1HSelect/copy/save in SGR66 mode, reopen and compare the halves."
    Add-Text "${esc}[13;1HSwitch to legacy mode and save a DIFFERENT file for old BBS."
    Add-Text "${esc}[15;1HDo not paste this whole screen fixture to a public BBS."
    Add-Text "${esc}[17;1HEND${esc}[4h"
    $visual = Join-Path $directory 'Sgr66Compatibility.ans'
    [IO.File]::WriteAllBytes($visual, $stream.ToArray())
    Write-Output $visual

    $stream.SetLength(0)
    $stream.Position = 0
    Add-Text "${esc}[1;37m"
    $stream.WriteByte($pair[0]); Add-Text "${esc}[33m"; $stream.WriteByte($pair[1])
    Add-Text "${esc}[m PICTURE`r`n"
    $legacy = Join-Path $directory 'LegacySplitPicture.ans'
    [IO.File]::WriteAllBytes($legacy, $stream.ToArray())
    Write-Output $legacy
    $compatible = Join-Path $directory 'Sgr66Picture.ans'
    [IO.File]::WriteAllBytes($compatible, $cp950.GetBytes("${esc}[1;37;66;33m${middle}${esc}[m PICTURE`r`n"))
    Write-Output $compatible
}
finally { $stream.Dispose() }
