#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MULTICAST_GROUP "239.0.0.1"
#define MULTICAST_PORT 8888
#define BUFFER_SIZE 1024

int main(void) {
    int sockfd;
    struct sockaddr_in local_addr;
    struct ip_mreq mreq;
    char buffer[BUFFER_SIZE];

    /* 建立 UDP Socket */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket 建立失敗");
        exit(EXIT_FAILURE);
    }

    /* 啟用 SO_REUSEADDR，允許同一台機器上的多個行程綁定同一個連接埠 */
    int reuse = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        perror("setsockopt SO_REUSEADDR 失敗");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    /* 綁定本地連接埠與任意介面 */
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    local_addr.sin_port = htons(MULTICAST_PORT);

    if (bind(sockfd, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0) {
        perror("bind 失敗");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    /* 加入群播組 (Multicast Group) */
    memset(&mreq, 0, sizeof(mreq));
    mreq.imr_multiaddr.s_addr = inet_addr(MULTICAST_GROUP);

    // 每台設備執行相同的接收端程式，唯一的細微差異在於 imr_interface 填寫各自的實體 IP（或直接填 INADDR_ANY）：
    mreq.imr_interface.s_addr = htonl(INADDR_ANY); /* 使用預設網路介面 */

    // 這段程式碼的作用是向作業系統核心（Kernel）登記加入指定的 UDP 群播組（Multicast Group）。
    // 如果沒有執行這段操作，接收端即使執行了 bind() 綁定連接埠，也永遠收不到任何發往該群播位址的封包。
    if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
        perror("setsockopt IP_ADD_MEMBERSHIP 失敗");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("已成功加入群播組 %s:%d，等待接收封包...\n", MULTICAST_GROUP, MULTICAST_PORT);

    /* 接收 5 次封包測試 */
    for (int i = 0; i < 5; i++) {
        struct sockaddr_in src_addr;
        socklen_t addr_len = sizeof(src_addr);

        ssize_t recv_len = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0,
                                    (struct sockaddr *)&src_addr, &addr_len);
        if (recv_len < 0) {
            perror("recvfrom 失敗");
            break;
        }

        buffer[recv_len] = '\0';
        printf("收到來自 %s:%d 的群播資料: %s\n",
               inet_ntoa(src_addr.sin_addr),
               ntohs(src_addr.sin_port),
               buffer);
    }

    /* 離開群播組並釋放資源 */
    if (setsockopt(sockfd, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
        perror("setsockopt IP_DROP_MEMBERSHIP 失敗");
    }

    close(sockfd);
    printf("已離開群播組並關閉 Socket。\n");
    return 0;
}