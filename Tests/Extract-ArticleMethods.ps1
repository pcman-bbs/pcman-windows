param([Parameter(Mandatory=$true)][string]$OutputPath)
$ErrorActionPreference = 'Stop'
$bytes = [IO.File]::ReadAllBytes((Join-Path $PSScriptRoot '../Lite/TelnetConn.cpp'))
try { $source = [Text.UTF8Encoding]::new($false, $true).GetString($bytes) }
catch { $source = [Text.Encoding]::GetEncoding(950).GetString($bytes) }
$parts = @()
foreach ($bounds in @(
    @('int CTelnetConn::IsEndOfArticleReached()', 'CString CTelnetConn::GetLineWithAnsi'),
    @('void CTelnetConn::CopyArticle(bool', 'void CTelnetConn::CopyArticleComplete(bool')
)) {
    $start = $source.IndexOf($bounds[0], [StringComparison]::Ordinal)
    if ($start -lt 0) { throw "Missing method: $($bounds[0])" }
    $end = $source.IndexOf($bounds[1], $start, [StringComparison]::Ordinal)
    if ($end -lt 0) { throw "Missing boundary: $($bounds[1])" }
    $parts += $source.Substring($start, $end - $start)
}
$fullPath = [IO.Path]::GetFullPath($OutputPath)
# Extract the real completion and decoding methods for the Unicode regression.
$start = $source.IndexOf('void CTelnetConn::CopyArticleComplete(bool', [StringComparison]::Ordinal)
if ($start -lt 0) { throw 'Missing CopyArticleComplete' }
$bodyStart = $source.IndexOf('{', $start)
$depth = 1
$end = $bodyStart + 1
while ($end -lt $source.Length -and $depth -gt 0) {
    if ($source[$end] -eq '{') { ++$depth }
    if ($source[$end] -eq '}') { --$depth }
    ++$end
}
if ($depth -ne 0) { throw 'Missing CopyArticleComplete end' }
$parts += $source.Substring($start, $end - $start).Replace('CTelnetConn::CopyArticleComplete(', 'CTelnetConn::ProductionCopyArticleComplete(')
$view = [Text.Encoding]::GetEncoding(950).GetString([IO.File]::ReadAllBytes((Join-Path $PSScriptRoot '../Lite/TermView.cpp')))
$start = $view.IndexOf('CStringW CTermView::DecodeText(', [StringComparison]::Ordinal)
if ($start -lt 0) { throw 'Missing DecodeText' }
$end = $view.IndexOf('LRESULT CTermView::WindowProc(', $start, [StringComparison]::Ordinal)
if ($end -le $start) { throw 'Missing DecodeText boundary' }
$parts += $view.Substring($start, $end - $start)
$conversion = [Text.Encoding]::GetEncoding(950).GetString([IO.File]::ReadAllBytes((Join-Path $PSScriptRoot '../Lite/Ucs2Conv.cpp')))
$start = $conversion.IndexOf('size_t CUcs2Conv::Big52Ucs2(', [StringComparison]::Ordinal)
if ($start -lt 0) { throw 'Missing Big52Ucs2' }
$end = $conversion.IndexOf('BOOL CUcs2Conv::InitBig52Ucs2Tab()', $start, [StringComparison]::Ordinal)
if ($end -le $start) { throw 'Missing Big52Ucs2 boundary' }
$parts += $conversion.Substring($start, $end - $start)
[void][IO.Directory]::CreateDirectory([IO.Path]::GetDirectoryName($fullPath))
[IO.File]::WriteAllText($fullPath, ($parts -join "`r`n"), [Text.UTF8Encoding]::new($false))
