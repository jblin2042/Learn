# Multicast（群播 UDP）

示範如何送出與接收 UDP 群播封包，使用私有群播網段 `239.0.0.1`（依 RFC 2365，`239.0.0.0/8` 為組織內部自由使用範圍）。

## 檔案

- `sender.c`：設定 `IP_MULTICAST_TTL` 後，向 `239.0.0.1:8888` 連續送出 5 筆群播訊息。
- `receiver.c`：綁定 `INADDR_ANY:8888`，透過 `IP_ADD_MEMBERSHIP` 加入群播組後接收封包，結束時以 `IP_DROP_MEMBERSHIP` 離開群組。

## 編譯

```bash
gcc -o sender sender.c
gcc -o receiver receiver.c
```

## 測試

開兩個終端機，於本資料夾下：

```bash
# 終端機 1：先啟動接收端（會先加入群播組）
./receiver

# 終端機 2：再啟動發送端
./sender
```

預期 receiver 收到 5 筆群播訊息並印出來源 IP。

> 注意：`sender.c` 預設 `TTL=1`，封包只會停留在本地子網路，不會被路由器轉發；若要跨網段，需確認路由器/交換器支援 PIM 及 IGMP Snooping，並手動調高 TTL。
