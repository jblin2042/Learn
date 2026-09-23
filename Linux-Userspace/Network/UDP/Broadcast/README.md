# Broadcast（全域廣播 UDP）

示範如何使用 `SO_BROADCAST` 送出全域廣播封包（`255.255.255.255`），僅能在同一個 Layer 2 廣播網域內收到，無法跨網段。

## 檔案

- `sender.c`：啟用 `SO_BROADCAST` 後，向 `255.255.255.255:8888` 連續送出 5 筆廣播訊息。
- `receiver.c`：綁定 `INADDR_ANY:8888`，接收送達本機的廣播封包並印出來源。

## 編譯

```bash
gcc -o sender sender.c
gcc -o receiver receiver.c
```

## 測試

開兩個終端機，於本資料夾下：

```bash
# 終端機 1：先啟動接收端
./receiver

# 終端機 2：再啟動發送端
./sender
```

預期 receiver 收到 5 筆廣播訊息並印出來源 IP。

> 注意：廣播封包不會被路由器轉發，若要跨網段測試，接收端與發送端必須位於同一個廣播網域（同一個子網段 / 同一個交換器 VLAN）。防火牆需允許 UDP 8888。
