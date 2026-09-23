# Ollama 初階使用手冊

這份文件適合第一次接觸 Ollama 的使用者，內容會以「最簡單、最常用」的方式介紹，讓你可以快速啟動本機 AI，並進行本地對話與測試。

---

## 1. 什麼是 Ollama？

Ollama 是一個可以在本機端快速執行大型語言模型（LLM）的工具。它的優點是：

- 不依賴雲端服務
- 可直接在本機跑 AI 模型
- 適合做本地測試、研究與原型開發
- 可搭配 API 整合到自己的程式中

簡單來說，Ollama 讓你可以把 AI 模型「裝在自己電腦上」來使用。

---

## 2. 安裝 Ollama

### Linux / macOS

可直接使用官方安裝指令：

```bash
curl -fsSL https://ollama.com/install.sh | sh
```

### Windows

可以直接下載官方安裝程式，或使用 WSL 方式安裝。對新手來說，使用 Linux / WSL 通常比較穩定。

安裝完成後，可先確認是否可用：

```bash
ollama --version
```

如果有版本號顯示，表示安裝成功。

---

## 3. 啟動 Ollama

在終端機輸入：

```bash
ollama serve
```

這會啟動 Ollama 服務，預設會在本機上開啟：

```bash
http://localhost:11434
```

### 確認服務是否有啟動

你可以用以下方式檢查：

```bash
sudo ss -tupl | grep 11434
```

如果正常啟動，會看到類似這樣的輸出：

```bash
tcp   LISTEN 0      4096      127.0.0.1:11434      0.0.0.0:*    users:("ollama",pid=1487,fd=4))
```

也可以查看服務狀態：

```bash
systemctl status ollama.service
```

如果服務沒有啟動，可以重新啟動：

```bash
systemctl restart ollama.service
```

如果你是用桌面版或其他方式啟動，通常也會自動在背景執行。只要確認服務正常，就可以開始使用。

---

## 4. 拉取模型

Ollama 並不是直接內建所有模型，通常需要先下載模型。常用指令如下：

```bash
ollama pull llama3.1
```

這會下載 `llama3.1` 這個模型。除了 `llama3.1`，也可以選擇其他模型，例如：

```bash
ollama pull qwen2.5
ollama pull mistral
```

### 查看已下載的模型

```bash
ollama list
```

範例輸出可能會類似：

```bash
gemma3:12b-it-qat      5d4fa005e7bb    8.9 GB    10 days ago
llama3.2-vision:latest 6f2f9757ae97    7.8 GB    11 days ago
llama3.1:8b            46e0c10c039e    4.9 GB    11 days ago
```

---

## 5. 直接聊天

下載完成後，可以直接執行模型進行互動：

```bash
ollama run llama3.1
```

進入互動模式後，就可以直接輸入問題：

```text
你是誰？
```

```text
幫我用簡單中文解釋什麼是 AI。
```

如果想離開，輸入：

```text
/bye
```

---

## 6. 常用指令整理

### 查看已安裝模型

```bash
ollama list
```

### 下載模型

```bash
ollama pull <model-name>
```

### 啟動模型聊天

```bash
ollama run <model-name>
```

### 移除模型

```bash
ollama rm <model-name>
```

### 檢查服務狀態

```bash
curl http://localhost:11434
```

如果伺服器正常，通常會回應一些基本資訊。

---

## 7. 用 API 方式呼叫模型

Ollama 也支援 HTTP API，這很適合拿來串接自己的程式。

### 範例：生成文字

```bash
curl http://localhost:11434/api/generate -d '{
  "model": "llama3.1",
  "prompt": "請用繁體中文解釋什麼是區塊鏈。",
  "stream": false
}'
```

這會回傳模型生成的文字結果。

### 範例：簡短聊天

```bash
curl http://localhost:11434/api/chat -d '{
  "model": "llama3.1",
  "messages": [
    {"role": "user", "content": "你好，請簡短介紹你自己。"}
  ],
  "stream": false
}'
```

這種方式比較適合做程式整合與後端串接。

---

## 8. 常見問題

### 1) 問題：`ollama: command not found`

代表 Ollama 沒有安裝成功，或環境變數沒有更新。重新安裝後重新開啟終端機即可。

### 2) 問題：模型下載很慢

- 檢查網路是否正常
- 不要重複下載同一個模型
- 確認是否能正常連線到 Ollama 官方資料來源

### 3) 問題：服務沒有啟動

先確認是否有執行：

```bash
ollama serve
```

如果沒有啟動，API 就無法使用。也可以用：

```bash
systemctl status ollama.service
```

來確認服務狀態。

### 4) 問題：記憶體不足

如果電腦資源有限，建議使用較小的模型，例如：

```bash
ollama pull tinyllama
```

或者選擇更適合本機環境的模型版本。

---

## 9. 適合新手的使用建議

1. 先安裝 Ollama
2. 下載一個小型或中型模型
3. 先用 `ollama run` 測試聊天
4. 之後再看 API 串接
5. 不要一開始就跑太大的模型

如果只是想先體驗 AI 能力，最簡單的流程就是：

```bash
ollama pull llama3.1
ollama run llama3.1
```

---

## 10. 一個最簡單的入門流程

```bash
ollama pull llama3.1
ollama run llama3.1
```

接著在終端機中輸入問題即可，這是最基本的本地 AI 對話流程。

---

## 11. 小結

Ollama 的核心概念很簡單：

- 安裝 Ollama
- 下載模型
- 啟動服務
- 直接聊天或用 API 呼叫

只要掌握這幾個步驟，就可以開始做本地 AI 應用測試了。

如果你想往下學，下一步可以嘗試：

- 使用不同模型比較效果
- 用 API 串接 Python 專案
- 研究本地 LLM 的 prompt 設計
- 將 Ollama 接到自己的小工具中

---

這份文件是初階版本，目的是讓你先能快速上手，而不是一開始就碰太多進階概念。後續如果你想，我也可以再幫你補一份「Python 串接 Ollama」或「Ollama + OpenWebUI」的進階版文件。

