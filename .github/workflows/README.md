# GitHub Actions Workflows

## build.yml - 自動建置與發布

這個 workflow 會在以下情況執行：

### 1. 日常開發（自動建置測試）
- **觸發時機**: 當程式碼推送到 `master` 分支，或建立 Pull Request 時
- **執行動作**:
  - ✅ 安裝依賴套件
  - ✅ 編譯專案
  - ✅ 打包檔案
  - ✅ 上傳 artifacts（保留 30 天）
- **結果**: 確保程式碼可以正常編譯，但不會建立 Release

### 2. 發布新版本（建立 Release）
- **觸發時機**: 當推送版本 tag 時（格式: `v*.*.*`，例如 `v9.5.1`）
- **執行動作**:
  - ✅ 完整建置專案
  - ✅ 建立 GitHub Release（草稿模式）
  - ✅ 上傳安裝檔 (PCMan.exe) 和免安裝版 (PCMan.zip)
  - ✅ 自動生成版本說明
- **結果**: 在 GitHub Releases 頁面建立新版本供使用者下載

## 使用方式

### 日常開發
```bash
git add .
git commit -m "Add new feature"
git push origin master
```
→ 自動建置測試，確保能編譯通過

### 發布新版本
```bash
# 1. 確定版本號（例如 9.5.1）
git tag v9.5.1

# 2. 推送 tag
git push origin v9.5.1
```
→ 自動建置並建立 GitHub Release

### 查看建置結果
1. 到 GitHub repository 頁面
2. 點選 "Actions" 標籤
3. 查看建置進度和結果

### 發布 Release
1. 當 tag 建置完成後，到 "Releases" 頁面
2. 找到對應的草稿 Release
3. 編輯說明（如需要）
4. 點擊 "Publish release" 正式發布

## 建置環境

- **作業系統**: Windows Server 2019
- **Visual Studio**: 2019 (16.0)
- **編譯器**: MSVC 2019
- **套件管理**: vcpkg
- **依賴套件**: cpprestsdk with websockets

## 注意事項

- Release 預設為**草稿模式**，需要手動發布
- Beta/Alpha 版本會自動標記為 Pre-release
- Artifacts 保留 30 天後自動刪除
- 每次建置約需 15-25 分鐘
