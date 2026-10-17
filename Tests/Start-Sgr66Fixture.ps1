# Loopback-only manual receive/paste test. No login, credentials or payload log.
param([ValidateRange(1024,65535)][int]$Port = 22366)
$ErrorActionPreference = 'Stop'
. (Join-Path $PSScriptRoot 'Sgr66FixtureHelpers.ps1')
$listener = [Net.Sockets.TcpListener]::new([Net.IPAddress]::Loopback, $Port)
$client = $null
$esc = [char]27
$middle = [string][char]0x4e2d
$cp950 = [Text.Encoding]::GetEncoding(950)
function Send-Text([string]$Text, [bool]$Bytewise = $false) {
    $bytes = $cp950.GetBytes($Text)
    if ($Bytewise) {
        foreach ($byte in $bytes) { $script:stream.WriteByte($byte); $script:stream.Flush(); Start-Sleep -Milliseconds 10 }
    } else { $script:stream.Write($bytes,0,$bytes.Length); $script:stream.Flush() }
}
try {
    $listener.Start()
    Write-Output "Within 60 seconds, open a NEW PCMan connection: telnet://127.0.0.1:$Port"
    Write-Output 'Use Big5. Do not reuse saved credentials, auto-login, macros or triggers. No login is required.'
    $deadline = [DateTime]::UtcNow.AddSeconds(60)
    while (-not $listener.Pending()) {
        if ([DateTime]::UtcNow -ge $deadline) { throw 'No client connected within 60 seconds.' }
        Start-Sleep -Milliseconds 100
    }
    $client = $listener.AcceptTcpClient(); $client.NoDelay = $true
    $script:stream = $client.GetStream()
    Write-Output 'A：PCMan 接收顯示檢查。這一階段不需要複製或貼上，傳統輸出格式也可使用。'
    Send-Text "${esc}[0m${esc}[2J${esc}[1;1HSGR 66 loopback - no login"
    Send-Text "${esc}[3;1HBYTEWISE: ${esc}[1;37;66;33m${middle}${esc}[m" $true
    Send-Text "${esc}[5;1HONE SHOT: ${esc}[31;66;32m${middle}${middle}${esc}[m"
    $null = Read-Host '確認雙色及第二個字全綠後，在此按 Enter，進入 B 同步重畫檢查'
    Send-Text "${esc}[7;1HOLD: ${esc}[31;66;32m${middle}${esc}[m"
    $null = Read-Host 'B：確認 OLD 紅／綠雙色後，在此按 Enter，開始 1 秒同步變更'
    Send-Text "${esc}[?2026h${esc}[7;1HNEW: ${esc}[34;66;33m${middle}${esc}[m"
    Start-Sleep -Milliseconds 1000
    Send-Text "${esc}[?2026l"
    Write-Output 'B：同步期間維持 OLD，結束後才顯示 NEW 藍／黃雙色，請人工確認畫面。'
    Write-Output 'C：以下是另一項「手動貼上」檢查；只看畫面或按 Enter，不會自動送回測試圖。'
    Write-Output '1. 在 PCMan「選項 > 彩色內容輸出格式（全域）」選擇「SGR 66 相容格式」（本機連線的自動模式仍用傳統格式；各模式可按「說明」查看）。'
    Write-Output '2. 用 ANSI 編輯器開啟 Tests/fixtures/LegacySplitPicture.ans，選取完整的「中 PICTURE」。'
    Write-Output '3. 使用「複製（包含 ANSI 彩色）」，切回本機測試連線，執行貼上；畫面不會回顯貼上內容。'
    Write-Output '不做貼上測試可輸入 S 略過，會記錄 SKIP，不會算通過。接收內容不會輸出或儲存。'
    while ($true) {
        $choice = (Read-Host 'C：已貼上請按 Enter 檢查（等待最多 2 秒）；S 略過；Q 結束').Trim().ToUpperInvariant()
        if ($choice -eq 'S') { Write-Output 'SKIP C: 手動貼上未驗收；A／B 的人工畫面判讀不受影響。'; break }
        if ($choice -eq 'Q') { Write-Output 'STOP：已結束本機測試，未完成步驟不記為通過。'; return }
        if ($choice -ne '') { Write-Warning '請按 Enter，或輸入 S／Q。'; continue }
        $report = Read-Sgr66PasteReport $script:stream
        if ($report.Passed) {
            Write-Output 'PASS C: received SGR66, intact Big5 picture bytes, and no split SGR.'
            break
        }
        $reason = switch ($report.Status) {
            'NoData' { '未收到測試圖（可能只有 Telnet 協商）；請先依上方步驟彩色複製並貼上，不是只按 Enter。' }
            'SplitBig5' { '收到舊式半字色碼，控制碼仍位於 Big5 字元中間；請切換 SGR 66 相容格式後重新完整貼上。' }
            'Incomplete' { '收到的控制碼或 Big5 字元尚未完整；請待貼上完成，再重新完整貼上測試圖。' }
            'InvalidBig5' { '收到不符合本測試的 Big5 位元組；請確認 PCMan 使用 Big5／UAO，且來源為指定 CP950 測試檔。' }
            'MissingSgr66' { '收到文字但沒有 SGR 66；請確認輸出格式，並使用包含 ANSI 彩色的複製，不是純文字複製。' }
            'MissingPicture' { '收到 SGR 66，但缺少指定測試圖的完整中文字；請選取 LegacySplitPicture.ans 的完整「中 PICTURE」。' }
            default { '資料未符合指定貼上測試，請重新完整貼上，或輸入 S 略過。' }
        }
        Write-Warning ("C 尚未通過：" + $reason)
        Write-Output ("診斷：收到 {0} bytes；文字 {1} bytes；SGR66={2}；完整測試字={3}；半字切斷={4}。" -f
            $report.ByteCount, $report.TextByteCount, $report.HasSgr66, $report.HasIntactPicture, $report.HasSplitBig5)
    }
    $null = Read-Host 'D：在此按 Enter，以未消耗的 SGR 66 狀態斷線；重跑腳本並重連以驗證重設'
    Send-Text "${esc}[31;66;32m"
}
finally {
    if ($client) { $client.Dispose() }
    $listener.Stop()
}
