#if 0
全域廣播（255.255.255.255）在網路協定中屬於「受限廣播（Limited Broadcast）」，其傳播範圍嚴格受限於發送端所在的 Layer 2 廣播網域（Broadcast Domain），無法跨越不同 Class C 網段的核心原因如下：

* **跨網段必須經由 Layer 3 設備**：不同網段（如 `192.168.1.0/24` 與 `192.168.2.0/24`）彼此隔離，資料傳遞必須交由預設閘道（Default Gateway，即路由器或 L3 交換器）轉發。
* **RFC 規範強制直接丟棄（Drop）**：依據 RFC 919 與 RFC 1122 標準，路由器收到目標 IP 為 `255.255.255.255` 的封包時，必須直接丟棄、嚴禁轉發，以避免引發「廣播風暴（Broadcast Storm）」癱瘓網路。
* **Layer 2 訊框（Frame）止於本地交換器**：受限廣播對應的目的 MAC 位址為 `FF:FF:FF:FF:FF:FF`，區域網路交換器（Switch）僅會在同一個實體網段或 VLAN 的連接埠複製該訊框，根本無法穿透閘道抵達外部網段。

```text
[ 網段 A : 192.168.1.0/24 ]                   [ 網段 B : 192.168.2.0/24 ]
+-------------------------+                   +-------------------------+
| 發送端主機              |                   | 接收端主機              |
| 目標 IP: 255.255.255.255|                   | (完全收不到廣播)        |
| 目標 MAC: FF:FF:FF:...  |                   |                         |
+------------+------------+                   +------------+------------+
             |                                             |
             v (訊框複製)                                  |
+------------+------------+                                |
| 本地交換器 (L2 Switch)  |                                |
| (僅在同網段/VLAN內擴散) |                                |
+------------+------------+                                |
             |                                             |
             v (抵達閘道)                                  |
+------------+------------+                                |
| 路由器 (Router / L3 GW) |                                |
| 遵循 RFC 919 / 1122     |                                |
|   ==> [ DROP 丟棄 ]     | --- ( 禁止轉發，阻斷 ) --- X   |
+-------------------------+                                +------------+

```
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BROADCAST_PORT 8888
#define BROADCAST_IP "255.255.255.255"

int main(void) {
    int sockfd;
    struct sockaddr_in dest_addr;
    const char message[] = "HELLO_GLOBAL_BROADCAST";

    /* 1. 建立 UDP Socket */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket 建立失敗");
        exit(EXIT_FAILURE);
    }

    /* 2. 關鍵步驟：啟用 SO_BROADCAST 權限 */
    int broadcast_enable = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast_enable, sizeof(broadcast_enable)) < 0) {
        perror("setsockopt SO_BROADCAST 失敗");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    /* 3. 設定目標端位址為 255.255.255.255 與指定連接埠 */
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST); /* 即 255.255.255.255 */
    dest_addr.sin_port = htons(BROADCAST_PORT);

    printf("開始向全域廣播位址 %s:%d 發送封包...\n", BROADCAST_IP, BROADCAST_PORT);

    /* 4. 連續發送 5 次廣播封包測試 */
    for (int i = 1; i <= 5; i++) {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%s [序號: %d]", message, i);

        ssize_t sent_bytes = sendto(sockfd, buffer, strlen(buffer), 0,
                                    (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (sent_bytes < 0) {
            perror("sendto 失敗");
            close(sockfd);
            exit(EXIT_FAILURE);
        }

        printf("已送出全域廣播封包: %s\n", buffer);
        sleep(1);
    }

    close(sockfd);
    printf("全域廣播發送完畢，關閉 Socket。\n");
    return 0;
}