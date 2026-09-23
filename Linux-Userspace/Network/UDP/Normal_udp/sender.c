#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

/* 目標接收端 IP 與連接埠 (此處以本機 127.0.0.1 為例，可改為遠端主機 IP) */
#define DEST_IP "127.0.0.1"
#define DEST_PORT 8888
#define BUFFER_SIZE 1024

int main(void) {
    int sockfd;
    struct sockaddr_in dest_addr;
    char message[] = "Hello, Unicast UDP!";
    char buffer[BUFFER_SIZE];

    /* 1. 建立 UDP Socket */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket 建立失敗");
        exit(EXIT_FAILURE);
    }

    /* 2. 設定目標位址結構 */
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = inet_addr(DEST_IP);
    dest_addr.sin_port = htons(DEST_PORT);

    printf("準備發送單播訊息至 %s:%d...\n", DEST_IP, DEST_PORT);

    /* 3. 發送資料並等待回應 */
    for (int i = 1; i <= 5; i++) {
        char send_buf[256];
        snprintf(send_buf, sizeof(send_buf), "%s [封包編號: %d]", message, i);

        ssize_t sent_bytes = sendto(sockfd, send_buf, strlen(send_buf), 0,
                                    (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (sent_bytes < 0) {
            perror("sendto 失敗");
            close(sockfd);
            exit(EXIT_FAILURE);
        }

        printf("已送出: %s\n", send_buf);

        /* 接收接收端回傳的 ACK */
        struct sockaddr_in reply_addr;
        socklen_t addr_len = sizeof(reply_addr);
        ssize_t recv_len = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0,
                                    (struct sockaddr *)&reply_addr, &addr_len);
        if (recv_len > 0) {
            buffer[recv_len] = '\0';
            printf("  └─ 收到來自 %s:%d 的回傳: %s\n",
                   inet_ntoa(reply_addr.sin_addr),
                   ntohs(reply_addr.sin_port),
                   buffer);
        }

        sleep(1);
    }

    close(sockfd);
    return 0;
}