# Offline visual fixture for PCMan's ANSI editor. No network or URL requests.
$ErrorActionPreference = 'Stop'
$esc = [char]27
$bel = [char]7
$can = [char]24
$sub = [char]26
$st = "${esc}\"
$open = "${esc}]8;id=offline;https://example.com/?pcman=osc8-target${st}"
$close = "${esc}]8;;${st}"
$content = [Text.StringBuilder]::new()
[void]$content.Append("${esc}[0m${esc}[2J${esc}[4l${esc}[1;1HPCMan OSC compatibility - offline visual check")
[void]$content.Append("${esc}[3;1HST: BEFORE ${open}LABEL${close} AFTER")
[void]$content.Append("${esc}[4;1HBEL: ${esc}]8;;https://example.com${bel}LABEL${esc}]8;;${bel} END")
[void]$content.Append("${esc}[5;1HUNKNOWN: ${esc}]0;ignored title${st}OK")
[void]$content.Append("${esc}[6;1HINJECT: ${esc}]0;${esc}[2J${esc}[?2026h${esc}[31mHIDDEN${st}OK")
[void]$content.Append("${esc}[7;1HLONG: ${esc}]8;;" + ('x' * 4096) + "${st}OK")
[void]$content.Append("${esc}[8;1HCANCEL: ${esc}]0;hidden${can}CAN OK | ${esc}]0;hidden${sub}SUB OK")
[void]$content.Append("${esc}[9;1HSGR: ${open}${esc}[31mRED LABEL${close}${esc}[0m NORMAL")
[void]$content.Append("${esc}[10;1HBLOCKED: ${esc}]8;;javascript:alert(1)${st}https://example.com/${close}")
[void]$content.Append("${esc}[11;1HOVERRIDE: ${open}https://visible.example/${close}")
[void]$content.Append("${esc}[12;1HEDIT: ${open}ABCDE${close}${esc}[12;9HX")
[void]$content.Append("${esc}[13;1HERASE: ${open}ERASED${close}${esc}[13;1H${esc}[2KERASE: plain (not linked)")
[void]$content.Append("${esc}[14;1HINSERT: ${open}ABCDE${close}${esc}[14;10H${esc}[2@")
[void]$content.Append("${esc}[15;1HDELETE: ${open}ABCDE${close}${esc}[15;10H${esc}[2P")
$chinese = [string][char]0x4e2d + [char]0x6587 + [char]0x9023 + [char]0x7d50
[void]$content.Append("${esc}[16;1HCHINESE: ${open}${chinese}${close}")
[void]$content.Append("${esc}[17;1HMULTI: ${open}FIRST${close}${esc}]8;;http://example.com/?pcman=second${st}SECOND${close}")
[void]$content.Append("${esc}[19;1HRight-click LABEL: copy hidden target; BLOCKED must not open.")
[void]$content.Append("${esc}[20;1HINJECT must not clear screen/change colour/start sync.")
[void]$content.Append("${esc}[21;1HOpen locally. Do not paste/send this file to a BBS.")
[void]$content.Append("${esc}[23;1HEND${esc}[4h")
$directory = Join-Path $PSScriptRoot 'fixtures'
[void][IO.Directory]::CreateDirectory($directory)
$outputPath = Join-Path $directory 'OscCompatibility.ans'
[IO.File]::WriteAllBytes($outputPath, [Text.Encoding]::GetEncoding(950).GetBytes($content.ToString()))
Write-Output $outputPath
