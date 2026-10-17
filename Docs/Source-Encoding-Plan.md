# 原始碼編碼標準化：檢查結果與調整計畫

日期：2026-10-17。狀態：已完成編碼盤點，尚未執行轉碼或修改編譯設定。
本項插入 OSC 8 驗收完成之後、SGR 66 彩圖匯出／貼上之前，建議獨立提交。

## 確認的問題

目前是歷史編碼與 UTF-8 混用，不能把「不是 UTF-8」直接等同於檔案損壞。
以 `git ls-files` 列出的 `.cpp`、`.h`、`.hpp`、`.c`、`.inl`、`.rc`、`.rc2` 共 174 個檔案進行位元組檢查：

| 分類 | 數量 | 判斷方式／代表檔案 |
| --- | --- | --- |
| 純 ASCII | 124 | 不含高位元組，亦為有效 UTF-8 |
| CP950 相容且可無損往返 | 38 | 嚴格 UTF-8 解碼失敗；嚴格 CP950 解碼後重編碼，位元組完全相同。例如 `Lite/TelnetConn.h`、`Lite/TermView.cpp`、`Resource/Common.rc` |
| UTF-8，有 BOM | 1 | `Lite/TelnetConn.cpp` |
| UTF-8，無 BOM、含非 ASCII 內容 | 11 | 例如 `Tests/ArticleProgressTests.cpp`、`Migrate/AppConfig.cpp` |

沒有遇到同時無法以嚴格 UTF-8 或 CP950 解碼的檔案；但可解碼／往返不代表已逐字證明所有文字從未損壞。
以 CP950 讀取 `TermView.cpp`，可正常顯示「複製」「搜尋」「開啟」等中文註解。

可重現的最小檢查，在專案根目錄執行：

```powershell
$strictUtf8 = [System.Text.UTF8Encoding]::new($false, $true)
foreach ($path in @('Lite/TelnetConn.h', 'Lite/TermView.cpp', 'Lite/TelnetConn.cpp')) {
    try {
        $null = $strictUtf8.GetString([System.IO.File]::ReadAllBytes((Join-Path (Get-Location).Path $path)))
        "$path : strict UTF-8 PASS"
    } catch [System.Text.DecoderFallbackException] {
        "$path : strict UTF-8 FAIL"
    }
}
```

本輪實測前兩個檔案為 FAIL，`TelnetConn.cpp` 為 PASS。這重現的是 UTF-8 工具無法讀取既有檔案，
不是 PTT 連線資料解析失敗，也不是本輪發現主程式執行錯誤。

## 設定與風險

- [.editorconfig](../.editorconfig) 的 `charset` 仍註解停用，沒有統一編輯器存檔編碼。
- [Lite.vcxproj](../Lite/Lite.vcxproj) 的 Debug／Release 都是 `MultiByte`，未在專案內明訂原始碼與執行字元集。
  BOM 可提供來源編碼線索；無 BOM 且沒有明確來源編碼選項時，MSVC 預設依使用者碼頁讀取。
  因此混用檔案可能讓編輯工具解碼失敗，或在不同語系環境編譯出不同結果。
  此預設行為見 [Microsoft 的來源／執行字元集說明](https://learn.microsoft.com/en-us/cpp/build/reference/utf-8-set-source-and-executable-character-sets-to-utf-8?view=msvc-170)。
- `Resource/Common.rc`、`Resource/Lite.rc` 與 `CustomTabCtrl/CustomTabCtrl.rc` 明訂 `#pragma code_page(950)`；
  部分資源也在 `TEXTINCLUDE` 內保留此設定，不可只轉檔而忘記資源編譯／編輯器重建規則。
- `/utf-8` 同時指定來源與執行字元集，不只是「以 UTF-8 讀檔」。目前程式仍有 ANSI／MultiByte 流程，
  直接套用可能改變一般窄字串的位元組內容，不能據此宣稱既有中文字串行為不變。
  見 [Microsoft 的執行字元集說明](https://learn.microsoft.com/en-us/cpp/build/reference/execution-charset-set-execution-character-set?view=msvc-170)。

## 待實作範圍

1. 優先處理 `PCMan.sln` 目前建置的 Lite、其共用標頭與資源，以及 Tests；`Migrate`、`UAOTabGen` 等工具另列範圍，
   不把整個儲存庫一律轉碼。這是檔案編碼維護，不更動 PTT 的 Big5／UAO／UTF-8 連線模式。
2. 將選定 C/C++ 檔案統一為 UTF-8（建議 UTF-8 BOM），以原始 CP950 嚴格解碼後轉換；
   比對轉換前後 Unicode 內容並確認 CP950 往返一致，不使用替代字元默默跳過錯誤。
3. 分開指定來源與執行字元集，評估 `/source-charset:utf-8` 與保留 CP950 執行字元集的方案；
   驗證既有窄字串、明確的 UTF-8 字面值及測試專案設定，避免全域 `/utf-8` 帶來行為改變。
4. 資源檔獨立處理：確認資源編譯器及 Visual Studio 資源編輯器可讀回，再決定 UTF-8 對應碼頁或 Unicode 儲存格式。
   更新所有生效與內嵌碼頁設定，不改變對話框、選單及字串資源內容。
5. 完成轉換後才啟用適用路徑的 `.editorconfig` 編碼規則，加入可自動執行的嚴格編碼檢查與例外清單。
   明確排除需保留原始位元組的 ANSI fixture、使用者設定、圖片與其他二進位檔；不得全庫強制 UTF-8。
6. 轉碼與功能修改分開，不順便重排格式、行尾或改寫文字，讓差異可核對。

## 完成條件

- 選定來源檔通過嚴格 UTF-8／BOM 規則；轉換前後文字相同，資源可由編輯器開啟並儲存。
- Debug／Release 主程式重新建置，五組既有測試重新建置並執行；不能沿用轉碼前的通過數量宣稱本項完成。
- 補足一般中文字串執行位元組與資源文字的回歸檢查；人工確認中文選單、對話框、訊息與網址功能。
- 重跑兩個 OSC 人工驗收流程，確認 PTT 瀏覽、選取／整篇複製及 ANSI 編輯／存檔未退化。
- 檢查工具應可攔下誤存為 CP950／混合編碼的來源檔，而不誤擋木刻意保留 Big5 的測試資料。

本輪只完成唯讀診斷及計畫文件；尚未轉換原始碼、加入編碼檢查器，或重新編譯／執行功能測試。
