param([Parameter(Mandatory=$true)][string]$OutputPath)
$ErrorActionPreference = 'Stop'
$source = [Text.UTF8Encoding]::new($false, $true).GetString(
    [IO.File]::ReadAllBytes((Join-Path $PSScriptRoot '../Lite/TelnetConn.cpp')))
$parts = @()
foreach ($signature in @('void CTelnetConn::OnText(', 'inline void CTelnetConn::ProcessAnsiEscapeSequence(',
    'bool CTelnetConn::UseSgr66(', 'inline void CTelnetConn::SetCurrentAttributes(',
    'CString CTelnetConn::GetLineWithAnsi(', 'CString AttrToStr(')) {
    $start = $source.IndexOf($signature, [StringComparison]::Ordinal)
    if ($start -lt 0) { throw "Missing production method: $signature" }
    $body = $source.IndexOf('{', $start)
    $depth = 1
    $end = $body + 1
    while ($end -lt $source.Length -and $depth -gt 0) {
        if ($source[$end] -eq '{') { ++$depth }
        if ($source[$end] -eq '}') { --$depth }
        ++$end
    }
    if ($depth -ne 0) { throw "Unclosed production method: $signature" }
    $parts += $source.Substring($start, $end - $start)
}
$source = [Text.UTF8Encoding]::new($false, $true).GetString(
    [IO.File]::ReadAllBytes((Join-Path $PSScriptRoot '../Lite/TermView.cpp')))
foreach ($signature in @('CString CTermView::GetSelAnsi(', 'void CTermView::SendAnsiString(')) {
    $start = $source.IndexOf($signature, [StringComparison]::Ordinal)
    if ($start -lt 0) { throw "Missing production method: $signature" }
    $body = $source.IndexOf('{', $start)
    $depth = 1
    $end = $body + 1
    while ($end -lt $source.Length -and $depth -gt 0) {
        if ($source[$end] -eq '{') { ++$depth }
        if ($source[$end] -eq '}') { --$depth }
        ++$end
    }
    if ($depth -ne 0) { throw "Unclosed production method: $signature" }
    $parts += $source.Substring($start, $end - $start)
}
$source = [Text.UTF8Encoding]::new($false, $true).GetString(
    [IO.File]::ReadAllBytes((Join-Path $PSScriptRoot '../Lite/MainFrm.cpp')))
foreach ($signature in @('static void AppendAnsiOutputMenu(', 'void CMainFrame::OnAnsiOutputFormatHelp(')) {
    $start = $source.IndexOf($signature, [StringComparison]::Ordinal)
    if ($start -lt 0) { throw "Missing production method: $signature" }
    $body = $source.IndexOf('{', $start)
    $depth = 1
    $end = $body + 1
    while ($end -lt $source.Length -and $depth -gt 0) {
        if ($source[$end] -eq '{') { ++$depth }
        if ($source[$end] -eq '}') { --$depth }
        ++$end
    }
    if ($depth -ne 0) { throw "Unclosed production method: $signature" }
    $parts += $source.Substring($start, $end - $start)
}
$fullPath = [IO.Path]::GetFullPath($OutputPath)
[void][IO.Directory]::CreateDirectory([IO.Path]::GetDirectoryName($fullPath))
[IO.File]::WriteAllText($fullPath, ($parts -join "`r`n"), [Text.UTF8Encoding]::new($false))
