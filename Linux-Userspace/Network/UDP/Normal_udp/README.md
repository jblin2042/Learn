# Normal_udp（單播 UDP）

最基本的 UDP 一對一（Unicast）收送範例：`sender` 送出訊息，`receiver` 收到後回傳 ACK。

## 檔案

- `sender.c`：對 `127.0.0.1:8888` 送出 5 筆訊息，每筆會等待 receiver 回傳的 ACK。
- `receiver.c`：監聽本機 `8888` 埠，收到訊息後印出來源並回傳 ACK。

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

預期看到 receiver 印出收到的訊息，sender 印出收到的 ACK 回覆，各執行 5 次後自動結束。

> 若要跨主機測試，修改 `sender.c` 中的 `DEST_IP` 為接收端主機的 IP 即可。
