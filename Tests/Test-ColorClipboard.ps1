# Check command wiring only. ArticleProgressTests exercises both clipboard
# representations with the shipped UAO table. Neither touches the clipboard.
$ErrorActionPreference = 'Stop'
$lite = Join-Path $PSScriptRoot '../Lite'
$view = [IO.File]::ReadAllText((Join-Path $lite 'TermView.cpp'))
$article = [IO.File]::ReadAllText((Join-Path $lite 'TelnetConn.cpp'))
$frame = [IO.File]::ReadAllText((Join-Path $lite 'MainFrm.cpp'))
$errorsFound = @()
$freq = [IO.File]::ReadAllText((Join-Path $lite 'CtrlEdit.cpp'))
if ($freq -notmatch 'CClipboard::GetStationText' -or $freq -notmatch 'CClipboard::GetTextW') { $errorsFound += 'frequent strings bypass station/Unicode clipboard formats' }
if ($view -notmatch '(?s)void CTermView::OnAnsiCopy\(\).*?GetSelAnsi\(\);\s*if \(CopyColorText\(data\)\)') { $errorsFound += 'selection does not publish Unicode + station bytes' }
if ($article -notmatch 'view->CopyColorText\(downloaded_article\)') { $errorsFound += 'colored article does not share selection output' }
if ($frame -notmatch 'CClipboard::GetStationText\(text, view.GetCodePage\(\)\)') { $errorsFound += 'paste does not preserve same-codepage station bytes' }
if ($errorsFound) { throw ($errorsFound -join '; ') }
Write-Output 'PASS: both color copy commands and station-aware paste routing'
