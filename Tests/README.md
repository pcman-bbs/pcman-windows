# CSI／OSC 解析測試

PTT 新介面（主選單、列表、空結果、看板資訊及自訂觸發器）的程式碼檢查與人工驗收，
見 [新介面回歸驗證](../Docs/PTT-Interface-Regression.md)。此項不由工具自動登入操作。

解析器補強 7-bit `ESC [`（CSI）解析，保留現有基本指令處理。
DEC2026 現已加入專用同步處理，見 [同步更新驗收](../Docs/DEC2026-Validation.md)；
SGR Mouse 仍忽略，CPR 仍由既有空函式處理，不回覆。
2026-10-17 已加入 OSC 字串解析與 OSC 8 目標點擊，見 [OSC 實作與驗收](../Docs/OSC-Validation.md)。
同日後續實作 SGR 66 彩圖匯出／貼上與 Big5 讀回，新增第六組測試及人工工具，見 [SGR 66 實作與驗收](../Docs/SGR66-Validation.md)。
PowerShell 驗收工具另有 `powershell -NoProfile -File Tests/Test-Sgr66Fixture.ps1`：兩種 PowerShell 環境各通過 179 項檢查，包含真正的本機 TCP 與原腳本重試／略過流程，不執行 PCMan GUI。
SGR 66 本輪 Debug／Release 主程式均完整重建成功；六組測試每組態通過 SGR 66 197,558、解析器 11,451、Keep-Alive 2,318、同步 100、文章 810、OSC 目標 1,343 項，人工驗收待執行。
這不是完整的 ECMA-48 終端機實作，DCS 字串協定尚未實作。
不將 Big5／UAO／UTF-8 中的高位元組當成 8-bit C1 控制碼。

2026-10-17 六則公告的程式碼評估與待補驗收，見 [近期公告評估](../Docs/PTT-Announcements-2026-10.md)。
OSC 8 的 payload 不流入文字；完整命令以有界 payload 傳給目標模組，標籤可點擊、右鍵複製網址並優先於文字自動辨識。
SGR 66 已加入 Big5 匯出／貼上與雙色讀回（UTF-8 接收仍降級），人工驗收見專用文件；
DECAWM 折行、DECSTBM 邊界及 VCOL 新畫面仍需對應驗證。
N5 的解析／接收路徑已有自動測試；2026-10-17 使用者確認兩個 OSC 人工驗收腳本及流程皆正確，
實際瀏覽 PTT 無特殊異常。本輪 OSC 人工驗收完成，其他 N1～N6 範圍仍依各項狀態記錄。

## 自動測試

使用 Visual Studio Developer PowerShell，在專案根目錄執行：

```powershell
MSBuild Tests/AnsiSequenceParserTests.vcxproj /t:Rebuild /p:Configuration=Debug /p:Platform=Win32
./Tests/bin/Debug/AnsiSequenceParserTests.exe
MSBuild Tests/AnsiSequenceParserTests.vcxproj /t:Rebuild /p:Configuration=Release /p:Platform=Win32
./Tests/bin/Release/AnsiSequenceParserTests.exe
```

測試直接包含正式程式使用的 `Lite/AnsiSequenceParser.h`，不是另外模擬一份解析器。
檢查既有指令分派、所有私有前綴與 final byte 組合、中間字元、未知指令、
63-byte 容量界線、數字超出 32767、ESC 重啟、CAN／SUB 取消、內嵌 C0、
中文編碼位元組、每個可能的兩段切割位置及逐位元組輸入，以及不同連線的狀態隔離。
過長、超出數字上限或不支援的序列會整段忽略，不執行截斷後的內容。

OSC 測試涵蓋 ST 所有切點、BEL、CAN／SUB、未知 OSC、內嵌 CSI／DEC2026、高位元組、
未終止與 1 MiB 輸入，以及每連線隔離和 Reset。最多保留 4096-byte 命令；超長仍吸收到結尾，不建立截斷網址。
2026-10-17 最終版：Debug／Release 各通過 OSC 目標 1,343、解析器 11,451、Keep-Alive 2,318、同步 100、文章 810 項。

OSC 目標與畫面範圍的獨立測試：

```powershell
MSBuild Tests/OscHyperlinkTests.vcxproj /t:Rebuild /p:Configuration=Debug /p:Platform=Win32
./Tests/bin/Debug/OscHyperlinkTests.exe
MSBuild Tests/OscHyperlinkTests.vcxproj /t:Rebuild /p:Configuration=Release /p:Platform=Win32
./Tests/bin/Release/OscHyperlinkTests.exe
```

使用正式 `AnsiSequenceParser.h`／`OscHyperlinks.h`，不開啟瀏覽器；驗證 HTTP／HTTPS、非法協定、
顯示網址與真正目標不同、相鄰／多位元組標籤、覆寫與搬移、快照、重設與 URL 額度。
這些不直接操作 MFC 滑鼠／繪圖，實際點擊與右鍵選單使用下方人工驗收。

此測試驗證解析結果，**不等於驗證 MFC 畫面繪製或實際 Telnet／WSS 連線**。
完整程式另以 `PCMan.sln` 的 Debug／Release Win32 組態編譯。

## 人工離線測試

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File Tests/New-CsiVisualFixture.ps1
```

1. 開啟剛編譯的 PCMan。
2. 使用「ANSI編輯 → 開啟舊檔」，開啟 `Tests/fixtures/CsiCompatibility.ans`。
3. 第 3 行 `RED BEFORE` 與 `RED AFTER` 應同樣為紅色。
4. 第 4 行的綠色 LEFT／RIGHT 應在同一行，第 5 行青色亦同。
5. DEC2026、MOUSE、LONG CSI、CPR 各行不應出現多餘控制碼文字。
6. RESTART 行應顯示綠色 `GREEN OK`，最後可見 `END`。

其中第 3 行可重現舊版把 `ESC[>4;2m` 誤當 SGR 重設顏色的問題；
第 4／5 行檢查未知指令不能誤移游標。畫面測試不驗證網路分段，
分段處理由上述自動測試涵蓋。不要將測試檔貼上／傳送至 BBS。

OSC 離線畫面另外使用 `Tests/New-OscVisualFixture.ps1` 產生 `Tests/fixtures/OscCompatibility.ans`，
各行預期結果與人工驗收紀錄見 [OSC 實作與驗收](../Docs/OSC-Validation.md)。
`Tests/Start-OscHyperlinkFixture.ps1` 提供只監聽 127.0.0.1:22328 的逐位元組／同步／捲動與重連測試；
不需帳密，不會自動開啟 URL，Enter 步驟在 PowerShell 視窗操作。執行方式見同一份 OSC 驗收文件。
2026-10-17 使用者確認上述兩個腳本及其驗收流程皆正確，實際瀏覽 PTT 也未發現特殊異常。

## 人工連線回歸測試

Debug 與 Release 分別登入 PTT／PTT2（不要同時執行兩個版本）：

- 登入前後無控制碼殘字、閃退或停滯。
- 看板清單、文章、上下頁切換及返回選單的顏色、游標位置正常。
- 中文／日文字顯示與選取複製沒有新增異常。
- 既有滾輪與右鍵功能保持原狀。

正常瀏覽預期與原先相同；不能只因畫面正常，就宣稱所有未知指令均已相容。
CSI 調整本身不處理文章複製；後續安全修正與測試見下方章節。

## Keep-Alive 自動測試

在 Visual Studio Developer PowerShell 執行：

```powershell
MSBuild Tests/KeepAliveTests.vcxproj /t:Rebuild /p:Configuration=Debug /p:Platform=Win32
./Tests/bin/Debug/KeepAliveTests.exe
MSBuild Tests/KeepAliveTests.vcxproj /t:Rebuild /p:Configuration=Release /p:Platform=Win32
./Tests/bin/Release/KeepAliveTests.exe
```

測試包含正式 KeepAlive／設定值解析 helper，建置前從正式 `TelnetConn.cpp` 擷取
`ProcessData`、`OnIAC`、`Send` 方法，在替代傳輸與畫面接收端中驗證。
涵蓋原始 `FF FD 06`、WILL／WONT 回覆的所有兩段切割與逐位元組接收、
不回 DONT、不產生文字、既有協商、計時重置、主機辨識、非法設定值及儲存往返。
此測試不是實際 WSS／Telnet 連線，也不取代 MFC 設定頁的儲存／繼承驗收。
2026-09-28：Debug／Release 各通過 884 項；CSI 各 8,896 項仍通過。

## Keep-Alive 人工驗收

由使用者操作；不要同時開啟安裝版與測試版，以免單一執行個體機制切回舊程式。
先備份平常使用的設定。直接從下列建置目錄執行，保留原目錄附屬檔案，勿只複製 EXE：

- Debug：`Lite/Debug/PCMan/PCMan.exe`
- Release：`Lite/Release/PCMan/PCMan.exe`

獨立 `Tests/bin/UiSmoke` 副本曾出現 Runtime Library 提示，原因未確認，請勿用它驗收。
若上述正常輸出目錄也出現提示，先停止測試並提供完整截圖（含檔名／行號），不要略過錯誤。

### 1. 設定畫面與驗證

開啟全域設定中的「站台選項」，檢查：

1. 「連線保持（防閒置）」、秒數、方式、字串、提示文字及原有下方設定均可見，無重疊／截斷。
2. 選擇「自動判斷」：字串可編輯；提示 PTT 用協定、其他站台沿用字串。
3. 選擇「TIMING-MARK」：字串停用，但原內容保留。
4. 選擇「自訂字串」：字串可編輯，提示可能干擾操作；說明指出留空不傳送資料。
5. 取消啟用：秒數／方式／字串停用；重新啟用後依所選方式恢復。
6. 啟用功能時，依次輸入空白、0、29、86401，再按確定或切換頁籤：應阻止離開並提示有效範圍。
7. 30、180、300、86400 應可接受；最終恢復 180 秒與自動判斷。無須實際等待 86400 秒。
8. 儲存並重新開啟設定／程式：秒數、方式與原字串應保持。

### 2. 個別站台與舊設定

1. 在個別站台設定勾選「使用全域設定」：相應欄位停用，值依全域設定顯示。
2. 取消繼承、改為 TIMING-MARK／300 秒，儲存後重開：個別值保留，全域值不被修改。
3. 恢復繼承，確認跟隨全域；完成後恢復原本需要的配置。
4. 使用尚未由新版儲存的舊設定備份時，缺少新模式應顯示自動判斷，原字串保留。
   自訂站台缺少模式亦採自動；設定為繼承者以全域模式為準。

### 3. PTT／PTT2 連線

先以平常使用的 WSS、180 秒、自動模式驗證；設定修改後重新連線，確保套用到新連線。
使用 `ptt.cc`／`ptt2.cc` 或其子網域可自動辨識；若用 IP／其他別名，明確選 TIMING-MARK。

1. 登入後停在看板列表，記住選取列，完全不操作至少 200 秒：選取列不應自行移動，也不應出現控制碼殘字。
2. 在不送出的搜尋／輸入框輸入少量測試文字，停留至少 200 秒：文字、游標不應被插入／改動；完成後取消。
3. 若要測編輯器，使用可安全取消的草稿，等待至少 200 秒：不應移動游標、改動內容或送出文章；勿發表測試內容。
4. 停在文章畫面超過兩個週期（約 6～7 分鐘）：不應自動翻頁、卡住或新增控制碼文字，之後按鍵仍正常。
5. 關閉保持連線、重新連線後，正常瀏覽仍應可用。單靠「沒斷線」無法證明功能已關閉或封包正確。
6. Debug 與 Release 分別執行；如平常也用 Telnet 或 PTT2，再對應重複連線測試。

正常畫面預期沒有明顯變化，改善是閒置時不再模擬方向鍵。
畫面正常不能單獨證明每個封包或每種網路環境正確；精確位元組由自動測試驗證，
實際網路傳輸／站台回覆仍需另外追蹤才可確證，也不保證永不斷線。

回報時請列出：Debug／Release、PTT／PTT2、WSS／Telnet、選擇模式、間隔、
通過的測試編號，以及異常截圖（遮蔽帳號、私人內容）。

## 文章複製自動測試

```powershell
MSBuild Tests/ArticleProgressTests.vcxproj /t:Rebuild /p:Configuration=Debug /p:Platform=Win32
./Tests/bin/Debug/ArticleProgressTests.exe
MSBuild Tests/ArticleProgressTests.vcxproj /t:Rebuild /p:Configuration=Release /p:Platform=Win32
./Tests/bin/Release/ArticleProgressTests.exe
```

測試正式解析 helper，並擷取正式 `GetArticleProgress`、`IsEndOfArticleReached`、
`CopyArticle`、`ContinueCopyArticle` 方法。UI、網路、ANSI 文字產生及剪貼簿完成端使用替身。
日文轉碼回歸另擷取正式 `CopyArticleComplete`、`DecodeText`、`Big52Ucs2`，
使用隨附的 `Lite/Release/PCMan/B2U` 字碼表驗證完成端輸出的 Unicode；請從專案根目錄執行。
涵蓋「ブルアカMAD音楽祭」、Big5／UAO 與 UTF-8、ANSI 控制碼保留、取消不覆寫，
以及 ANSI 編輯器仍接收原始位元組。Windows 剪貼簿 API 與編輯器 UI 使用替身，不會更動實際剪貼簿。
包括無 NUL 輸入、保護記憶體頁邊界、空白／缺少符號／超大數字／非法百分比、
Big5／UTF-8 行單位、不完整單位、右側提示、不完整底列逐段更新、重複／異常跳行、
單頁完成、增加一／兩行、純文字與帶色路徑。不等於實際 MFC、WSS 或剪貼簿驗證。

新增 `InterfaceRegression`：以公告格式合成新舊列表標籤、中文閱讀進度及動態提示，
檢查 Big5／UTF-8 位元組、80／120 位元組邊界及可見底列之外的干擾資料。
不是實際站台畫面錄製，也不驗證終端機 UTF-8 顯示欄寬、鍵鼠操作或觸發器。

## 文章複製人工驗收

Debug／Release 分開執行；使用原建置目錄，不需調整防閒置設定。
請先備份剪貼簿中需要保留的內容。PTT 的 WSS 為主要驗收項目，PTT2／Telnet 有使用再測。

1. 單頁短文章：進入文章、按 Home 回到開頭，執行整篇文章複製，貼到本機記事本。
   應完成且不多送方向鍵；內容不含底列操作提示。
2. 多頁文章：從 Home 開始複製，檢查開頭、跨頁交界、最後一行與推文是否有重複或遺漏。
   下載過程請勿另外按上下鍵／改變視窗尺寸；完成後應停在文章末端。
3. 帶 ANSI 複製／下載至 ANSI 編輯器：重複短／長文章，檢查文字完整與色彩。
4. 在看板列表而非文章中執行：應提示無法辨識，不自行翻頁，也不覆寫原剪貼簿。
5. 在較長文章下載中按取消：應停止後續送鍵、不把部分內容當完成；重新從 Home 複製應可正常進行。
6. 若連線較慢或遇到不完整底列，應等待而非閃退、狂送按鍵或提早完成；可取消後回到文章畫面重試。
7. 如有使用「播放動畫」，確認一般文章仍可推進，到末端停止；不在文章畫面時不得持續送鍵。
8. 日文轉碼：找含「ブルアカMAD音楽祭」或其他日文的文章，從 Home 開始整篇複製，
   貼到記事本，確認與一般選取複製／右鍵搜尋的字元一致，不再出現方框或私人使用區錯字。
   單頁與多頁各測一次，並確認中文、英文及換行正常。
9. 同一篇文章執行帶 ANSI 色碼複製及下載至 ANSI 編輯器，確認前者日文正確且仍含控制碼
   （記事本不會呈現色彩），後者文字與色彩正常；取消下載不應覆寫原剪貼簿。

目前採保守辨識：需有 `(百分比%)` 及 `起始~結束 行`；橫向捲動／其他 BBS 不相容底列
可能拒絕啟動或等待，請提供底列截圖再評估支援，不要據此宣稱所有 BBS 相容。
後續已加入 DEC2026 同步期間暫停擷取的保護，但逾時／未送同步指令時仍不保證畫面完整；
本功能不等同於任意輸出的完整交易同步。
若等待超過正常更新時間可直接取消，不需一直等。回報請附組態、站台、連線方式及出問題的底列截圖。
