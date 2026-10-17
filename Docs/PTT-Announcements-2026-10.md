# 近期 PTT 公告與 PCMan LITE 相容性評估

評估日期：2026-10-17。範圍：使用者提供的六則公告，以及目前本機 `master`（`c8404c6`）。
本次讀取公告原文並檢查程式碼，只更新文件；沒有修改正式程式、重新編譯、操作 GUI 或登入站台。
下列上線日期與未來期限均為公告記載，不代表已實測伺服器部署；9/28、10/03 的人工回報亦不替代更新後驗收。

## 公告摘要與結論

| 公告 | 公告記載的時程 | 主要影響 | PCMan 結論 |
| --- | --- | --- | --- |
| [SGR 66 一字雙色](https://www.ptt.cc/bbs/PttCurrent/M.1789835221.A.6AC.html)（9/20） | PTT1 9/20、PTT2 9/19；12/1 前停收切斷 Big5 字元的舊式輸入，12/31 前關閉編輯器 Raw mode；2027 內部轉碼時程未定 | UTF-8／啟用去除舊式一字雙色的輸出，以及 ANSI 彩圖的輸入格式 | 接收端目前忽略 66，可降級顯示；彩圖匯出／貼上應優先規劃 SGR 66，不能因為使用 Big5 連線就判定輸入端不受影響 |
| [Keep-Alive](https://www.ptt.cc/bbs/PttCurrent/M.1790178013.A.9FB.html)（9/23） | 公告表示 PTT1／PTT2 已支援，適用 WSS | 使用原始 `FF FD 06` 保持雙向流量，避免模擬按鍵干擾 UI | 預設自動模式已有實作；保留的自訂模式可手動選擇，但在 PTT 使用時不符合公告推薦方式 |
| [DECAWM 自動折行](https://www.ptt.cc/bbs/PttCurrent/M.1790662175.A.563.html)（9/29） | PTT1 10/04、PTT2 9/29 | `ESC[?7h/l` 控制右側邊界折行；站台初始化會送關閉 | 目前安全忽略，預設 `line_wrap=0` 符合 BBS 建議；使用者啟用本機折行時需特別驗證 |
| [DECSTBM 捲動範圍](https://www.ptt.cc/bbs/PttCurrent/M.1790697156.A.14E.html)（9/29） | PTT1 10/04、PTT2 9/29 | `ESC[top;bottomr`／`ESC[r`；超過有效列數時限制捲動區 | 已有設定／重設與 LF 捲動路徑；缺少游標 Home。UI 上限 64 列，公告超過 150 列的主要情境目前不由正常設定觸發 |
| [OSC 8 超連結](https://www.ptt.cc/bbs/PttCurrent/M.1790734943.A.8D4.html)（9/30） | PTT1 10/11、PTT2 9/30；Markdown 超連結為後續計畫 | OSC 字串包含 URL，ST 結束；可不支援點擊，但必須完整吃掉控制內容 | 明確解析缺口，應優先修正。現有解析器只吃掉 `ESC ]`，其後的 `8;...URL` 會流入文字路徑 |
| [VCOL 動態分欄](https://www.ptt.cc/bbs/PttCurrent/M.1790827526.A.744.html)（10/1） | PTT2 9/30、PTT1 10/4；80×24 原則上維持原貌，之後小幅對齊調整不再逐次公告 | 列表隨終端寬度調整欄位／省略內容，不能依賴固定欄位座標 | 未發現內建列表資料擷取依賴固定欄位的直接調整需求；應補寬畫面、空列表與自訂觸發器驗收 |

### 程式碼查核入口

| 路徑 | 本輪檢查的方法／設定 |
| --- | --- |
| [AnsiSequenceParser.h](../Lite/AnsiSequenceParser.h) | `Feed`、`Finish`：CSI／ESC 分派、private sequence 忽略及缺少 OSC string 狀態 |
| [TelnetConn.cpp](../Lite/TelnetConn.cpp) | `OnIAC`、`OnText`、`ProcessAnsiEscapeSequence`、`SetCurrentAttributes`、`SetScrollRange`、`LineFeed`、`GetLineWithAnsi`、`GetArticleProgress`、`CheckHyperLinks`、`CheckStrTrigger` |
| [TelnetConn.h](../Lite/TelnetConn.h)／[SitePage.cpp](../Lite/SitePage.cpp) | UI 終端範圍為 40～160 欄、24～64 列，儲存設定時限制在範圍內 |
| [SiteSettings.h](../Lite/SiteSettings.h) | 預設 80×24、`line_wrap=0` |
| [TermView.cpp](../Lite/TermView.cpp)／[KeepAlive.h](../Lite/KeepAlive.h) | `OnTimer`、`SendAnsiString` 與原始 TIMING-MARK 請求／回覆辨識 |
| [MouseCTL.cpp](../Lite/MouseCTL.cpp) | 滑鼠瀏覽與手勢的按鍵映射；不按列表作者／標題的固定欄位擷取資料 |

## 1. SGR 66：顯示可降級，彩圖輸出需要規劃

公告使用 `ESC[1;37;66;33m中` 表示前半白、後半黃。66 將後續屬性切換成後半字著色，
不是一般前景色或字碼轉換指令；它是 PTT 自訂延伸，不應宣稱完整 ANSI 標準。

目前 `AnsiSequenceParser::Finish` 會將普通數值 SGR 交給 `ProcessAnsiEscapeSequence`，
後者逐項呼叫 `SetCurrentAttributes`。該方法只處理小於 48 的值，因此 66 不改變狀態，
其後 33 仍套用為普通前景色：屬於公告允許的單色降級，不會呈現新的半字雙色。
這是程式碼推導，未新增 SGR 66 專用畫面驗收，也不代表 UTF-8 欄寬問題已解決。

輸入端需另看：`CTelnetConn::GetLineWithAnsi` 按每個位元組的顏色變化插入 ANSI，
可把控制碼放在 Big5 字元的兩個位元組之間；`CTermView::SendAnsiString` 只替換 ESC、
再交給 `SendString`，不會轉成 SGR 66。現有 `AttrToStr` 也沒有輸出 66。
因此「下載到 ANSI 編輯器 → 儲存／貼回站台」仍可能產生公告即將停收的舊式格式。

建議後續獨立實作 SGR 66 匯出／貼上選項，保留給其他 BBS 使用的舊格式；
同時考慮本機 ANSI 編輯器讀回新格式，否則新匯出檔再次開啟會失去半字色差。
一般文章的純文字複製與 PR #7 的 Unicode 轉碼修正，不等於完成這個彩圖格式遷移。
若只做一般 Big5 瀏覽，可先保留接收端單色降級；若要維持彩圖發表能力，應在公告期限前完成輸出端調整。

## 2. Keep-Alive：既有實作已涵蓋推薦路徑

`KeepAlive.h` 固定定義原始請求位元組；`CTermView::OnTimer` 在 PTT 自動模式或明確選用
TIMING-MARK 時直接呼叫 `Send`，不走 `SendString` 的一般文字處理。
`CTelnetConn::OnIAC` 在長度足夠時吸收 WILL／WONT TIMING-MARK，不再回 DONT；
`KeepAlive::Tick` 與送出資料的計時重設已有相應路徑。

公告明確指出 WONT 是預期回覆，不應將 WONT 視為保持連線失敗。
目前未建立等候回覆逾時、半開連線判定或自動重連狀態機；這些是可選後續功能，
不因本次公告而立即改為「未收到一次回覆就斷線」。
PTT 請使用自動／TIMING-MARK；若使用 IP 或非 PTT 網域別名，需明確指定 TIMING-MARK。
既有自訂字串模式仍可能送出一般按鍵，不能宣稱所有設定都符合公告的去干擾建議。

## 3. DECAWM：目前可忽略，但本機折行設定有影響

解析器把 `?7h`／`?7l` 視為不支援的 private CSI，完整吸收到 final byte，不回覆。
`CSiteSettings` 預設 `line_wrap=0`；`OnText` 依此決定滿欄後是否進入下一行。
因此標準 BBS 設定可維持現況，與「已支援遠端切換 DECAWM」需分開描述。

若使用者曾啟用折行，伺服器的 `?7l` 不會覆蓋本機設定，邊界文字仍可能造成額外 LF。
先驗證第 80 欄、末欄雙位元組字元與後續游標定位；若要支援 UNIX terminal 語意，
再新增每連線模式與重設規則，避免直接改寫已儲存的使用者設定。

## 4. DECSTBM：部分支援，不應列為完全未知指令

數值 CSI `r` 已在解析器白名單；`ProcessAnsiEscapeSequence` 呼叫 `SetScrollRange`。
該方法把省略／0 的邊界轉成全螢幕預設值，並限制至本機列數；`LineFeed` 使用
`scr_top`／`scr_bottom` 捲動畫面。因此 `ESC[1;150r` 與 `ESC[r` 並非全部忽略。

已確認缺少標準要求的游標 Home：`SetScrollRange` 只更新捲動邊界，未呼叫 `GotoXY(1,1)`。
公告表示 PTT 隨後會補游標復位，故不能推論一般 PTT 畫面必定已壞；其他站台及不帶復位的序列仍有差異。
本機 UI 列數上限是 64（`MAX_LINES_PER_PAGE`），因此公告所舉「超過站台 150 列上限」
目前不會由正常設定觸發；不應據此列為必須立刻支援超過 150 列，也不要為測試繞過 UI 上限。

若要補完整終端語意，`LineFeed` 對「游標在捲動區下方」沒有獨立分支，需要觀察是否誤捲區域。
`GetArticleProgress` 固定讀 `screen[last_line]`；若有效站台底列位於捲動區底而非本機最末列，
文章複製可能保守拒絕／等待。這是局部捲動區下的條件性風險，尚未確認 PTT 正常設定可重現，
不直接改成掃描全畫面猜測進度。

後續應補捲動區、區外游標、重設、相等／反向邊界、resize 與文章複製測試，
再決定修正 Home、邊界與有效底列的具體方式。標準 80×24 的風險較低，但尚未由本輪重驗。

## 5. OSC 8：最高優先的解析補強

公告格式是 `OSC 8;params;URL ST label OSC 8;; ST`；OSC 是 `ESC ]`，ST 是 `0x1B 0x5C`（ESC 加反斜線）。
應只把 `label` 送到畫面；開啟與結束標記內的資料不應佔用游標欄位。

目前 `AnsiSequenceParser::Feed` 沒有 OSC string 狀態。收到 `ESC ]` 後就返回 Ground，
因此最簡單的空 params 範例在文字路徑會變成 `8;;https://example.comlabel8;;`，
而不是單獨 `label`；這會影響排版、選取複製、文章擷取與既有自動網址辨識。
OSC 內如果包含 ESC／CSI，目前也可能重新進入一般解析路徑，不能視為已安全消化未知 OSC。
DEC2026 只控制重畫時機，無法修復此解析缺口。

第一階段至少新增可跨接收區塊、逐位元組吸收到 ST（`0x1B 0x5C`）的有界 OSC 處理，
先忽略開啟／結束標記而保留標籤文字；不需要同時完成可點擊超連結。
實作時應定義 BEL 結束、CAN／SUB 取消、未結束／超長字串與斷線重設行為，
超長時不得將尾端 payload 當文字釋出。ST 與 OSC 8 關閉連結標記是兩個不同層次：
ST 結束每一段 OSC，空 URL 才結束目前的超連結範圍。

第二階段若支援點擊，需保存文字範圍與目標 URL、同步快照／捲動／尺寸變動的生命週期，
並讓 OSC 8 的目標優先於畫面文字的 auto-link。依公告只允許 `http://`／`https://`，
使用者點擊前不自動開啟或請求 URL。目前 `CheckHyperLinks` 僅掃描畫面文字，沒有 OSC 目標資料。

## 6. VCOL：以回歸驗證與使用者規則為主

VCOL 改變的是站台排版，不是新增回覆協定。一般鍵盤傳送、滑鼠的按鍵映射與
畫面重畫不需要照站台的每個欄位重新切片；檢查到的文章複製只解析底列左側進度，
並非以文章列表的標題／作者固定位置判斷完成。仍需驗證真實新介面，不能保證所有功能都不受影響。

對使用者觸發器／巨集，固定空白、列標籤、標題尾端或色碼可能不再命中；
現有 `CheckStrTrigger` 比對原始接收區塊，不是正規表示式引擎，也沒有完整的跨區塊保證。
不能把公告給 bot 的 regex 建議直接寫進 PCMan 規則。`memstr` 的既有長度風險仍另案處理。
優先重測 80×24，再依使用情境加測較寬終端、長標題、省略號、空列表與看板資訊；
UI 允許 40～160 欄，寬畫面可選平常使用的 120／160 欄；不要為驗收移除信件或發表測試文章。

## 待辦順序與驗證狀態

| 優先序 | 後續工作 | 完成條件 |
| --- | --- | --- |
| 1 | OSC 字串安全吸收 | 不顯示 URL／控制 payload、不錯執行其中 CSI；ST 切段、長字串、取消與連線隔離有測試 |
| 2 | SGR 66 彩圖匯出／貼上 | 不在 Big5 字元內插入控制碼；可保留半字屬性；新檔讀回與其他 BBS 格式選項完成驗證 |
| 3 | 新介面與邊界回歸 | 完成下列 N1～N6，記錄組態、站台、連線及終端尺寸；DECAWM／DECSTBM 異常再開修正 |
| 後續 | OSC 8 點擊、SGR 66 完整顯示、CPR、SGR Mouse | 各自設計與驗證，不與最低限度解析修正綁成同一功能 |

本輪確認 PR #5（`fcd3950`）、#6（`b13d995`）、#7（`c8404c6`）已在本機歷史中。
PR #7 的日文／Unicode 複製修正不再是未提交功能；相關文件原有的人工驗收待辦予以保留，
不能僅由合併推論已完成實際剪貼簿／編輯器驗收。
本輪沒有重跑原有自動測試，也沒有新增上述公告的功能測試；下列均為待驗收。

| 編號 | 驗證項目 | 預期／目前限制 |
| --- | --- | --- |
| N1 | SGR 66：單獨 66、與其他色碼混合、Big5／UTF-8、逐段接收 | 目前可單色降級，不殘留控制文字；不宣稱有半字雙色。未實作前不要把舊式彩圖貼回站台作測試 |
| N2 | Keep-Alive：自動／TIMING-MARK，WSS PTT1／PTT2，停在列表與輸入框 | 選取列與未送出文字不被改動，WONT 被吸收；保留先前驗收紀錄，本輪尚未重驗 |
| N3 | DECAWM：`?7l/h`、折行設定開／關、右邊界雙位元組 | 不顯示控制碼；記錄本機設定而非假定遠端模式已生效 |
| N4 | DECSTBM：設定／重設、區內／區外 LF、resize、文章下載 | 先以本機 fixture 測局部捲動區與 Home；UI 僅 24～64 列，不繞過上限模擬 PTT 超過 150 列的情境 |
| N5 | OSC 8：ST 分段、開／關連結、未知 OSC、很長或未結束 payload、取消 | 目前列為已知不相容；最低修正後只能看見 label。此項先以本機 fixture／解析器測試驗證 |
| N6 | VCOL：80×24 與較寬畫面、長標題／省略號、空結果、選取及文章複製 | 無多餘折行、欄位殘字或錯誤完成；自訂規則未使用時可記 N/A，不要求新增規則 |
