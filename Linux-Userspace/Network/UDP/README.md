# UDP 範例集

這個資料夾收錄三種 UDP 傳輸模式的最小可執行範例，皆使用標準 BSD Socket API 撰寫，方便對照理解各模式的差異。

| 資料夾 | 模式 | 說明 |
| --- | --- | --- |
| [Normal_udp/](Normal_udp/) | 單播 (Unicast) | 一對一傳輸，sender 送出訊息後由 receiver 回傳 ACK，示範最基本的 UDP 收送流程。 |
| [Broadcast/](Broadcast/) | 廣播 (Broadcast) | 透過 `255.255.255.255` 全域廣播位址，一次送給同一個廣播網域內的所有主機。 |
| [Multicast/](Multicast/) | 群播 (Multicast) | 透過 `239.0.0.1`（私有群播網段）將封包送給所有加入該群組的主機，示範 `IP_ADD_MEMBERSHIP` 的用法。 |

## 共同結構

每個資料夾都包含：
- `sender.c`：發送端程式
- `receiver.c`：接收端程式

各自的編譯方式與測試步驟請參考各資料夾內的 `README.md`。

## 建議閱讀順序

1. `Normal_udp`：了解 `socket()` / `bind()` / `sendto()` / `recvfrom()` 基本操作。
2. `Broadcast`：了解 `SO_BROADCAST` 與廣播的網段限制。
3. `Multicast`：了解群播位址規劃與加入/離開群組的機制。
