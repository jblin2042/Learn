#if 0
# ======================================================================
IPv4 群播位址 (Class D: 224.0.0.0 - 239.255.255.255) 規劃指南

[核心結論]
私有網路僅能使用 239.0.0.0/8，其餘網段皆由 IANA 保留給特定協定。

---

## 一、群播網段分配一覽 (RFC 5771)

## 網段區間              規範名稱 / 狀態      用途說明與限制

## 224.0.0.0/24         本地控制 [嚴格禁止]   底層協定 (OSPF, mDNS)；TTL=1 不跨路由器
224.0.1.0/24         網際控制 [不可使用]   全網協定 (NTP, PTP)；可跨路由器轉發
224.0.2.0-231.x.x.x  臨時保留 [不建議]     IANA 依申請指派給特定商業/公開協定
232.0.0.0/8          特定來源 [需特定架構] SSM 專用，需 IGMPv3 並指定來源單播 IP
233.0.0.0-234.x.x.x  GLOP前綴 [不可私用]   需綁定公開自治系統編號 (AS Number)
239.0.0.0/8          組織管理 [自由使用]   私有群播 (RFC 2365)，邊界路由器不轉發

---

## 二、誤用 224.0.0.x 的兩大風險

1. 交換器降級洪泛 (Flood)：
網管交換器依規範會對此網段強制略過 IGMP Snooping，直接向所有連接埠
廣播，徹底失去群播節省頻寬的優勢。
2. 衝擊同網段主機核心 (CPU Spike)：
作業系統核心與網路卡韌體隨時監聽此區段。大量自訂封包會引發同網段
所有主機頻繁觸發硬體中斷，造成系統負載暴增。

---

## 三、私有開發選址原則 (239.0.0.0/8)

1. 推薦網段：
* 站台本地 (Site-Local)：239.255.0.0/16 (例如 239.255.1.100)
* 組織內部範圍：239.1.1.1 等自訂網段


2. 絕對避開：
* 239.255.255.250：SSDP (UPnP 設備發現) 專用位址，誤用會收到大量家電
與印表機的廣播干擾。
======================================================================



IP、連接埠 (Port) 與 Socket 繫結 (Bind) 本質

1. 核心的端點識別機制
* 在作業系統核心中，端點識別為三元組：(傳輸協定, 本機 IP, 本機 Port)。
* TCP 與 UDP 在核心中為完全分開的獨立表格：
* TCP 8888 與 UDP 8888 互不衝突，可同時並存。


* 每個特定 IP 各自擁有獨立完整的 65535 個連接埠空間。


2. 衝突與閃避規則
* 情況一：特定 IP + 特定 IP -> 不衝突，不需閃避
* 行程 A bind("127.0.0.1", 8888)
* 行程 B bind("239.0.0.1", 8888)
* 結果：兩者 IP 明確不同，核心可同時啟動，互不干擾。


* 情況二：包含 0.0.0.0 (INADDR_ANY) -> 產生衝突，必須閃避
* 0.0.0.0 代表「通吃本機所有 IP」。
* 若有一方 bind("0.0.0.0", 8888)，另一方要綁定 8888 就會觸發
EADDRINUSE (Address already in use)，除非雙方皆開啟 SO_REUSEPORT。
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MULTICAST_GROUP "239.0.0.1"
#define MULTICAST_PORT 8888

int main(void) {
    int sockfd;
    struct sockaddr_in dest_addr;
    char message[] = "Hello, Multicast Group!";

    /* 建立 UDP Socket */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket 建立失敗");
        exit(EXIT_FAILURE);
    }

    /* 設定目的端群播位址與連接埠 */
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = inet_addr(MULTICAST_GROUP);
    dest_addr.sin_port = htons(MULTICAST_PORT);

    /* 設定 TTL (Time-To-Live)：預設值通常為 1 (僅限於本地區網子網路) */
    // * TTL 的本質是「跳數」而非時間： 封包每經過一台路由器就扣 1，歸零即丟棄，核心目的是防止路由無限迴圈；UDP 群播因預設 TTL 為 1，未手動調大前連預設閘道都出不去。
    // * 群播「理論與實務」的落差： 理論上路由器會透過 PIM 協定自動複製分發，發送端只需送一次；實務上因硬體預設關閉、IT 資安政策限制與除錯成本高，跨網段極難完全打通。
    // * 工程實務的架構決策：
    // * 少量設備（10～50 台）或控制指令： 直接在發送端跑單播迴圈（逐一發送），耗時僅微秒級，成本最低、相容性最高。
    // * 大量設備或高頻寬影音： 發送端無法負載時，才值得推動打通跨路由群播，或改用中繼轉發服務（Message Broker / Forwarding Relay）。
    unsigned char ttl = 1;
    if (setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl)) < 0) {
        perror("setsockopt IP_MULTICAST_TTL 失敗");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("開始發送群播訊息至 %s:%d...\n", MULTICAST_GROUP, MULTICAST_PORT);

    /* 連續發送 5 次封包測試 */
    for (int i = 1; i <= 5; i++) {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%s [編號: %d]", message, i);

        ssize_t sent_bytes = sendto(sockfd, buffer, strlen(buffer), 0,
                                    (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (sent_bytes < 0) {
            perror("sendto 失敗");
            close(sockfd);
            exit(EXIT_FAILURE);
        }

        printf("已成功發送: %s\n", buffer);
        sleep(1);
    }

    close(sockfd);
    return 0;
}