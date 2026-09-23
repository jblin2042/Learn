#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define RECEIVER_PORT 8888
#define BUFFER_SIZE 1024

int main(void) {
    int sockfd;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    /* 1. 建立 UDP Socket */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket 建立失敗");
        exit(EXIT_FAILURE);
    }

    /* 2. 設定本地監聽位址與連接埠 */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); /* 監聽本機所有網路介面 */
    server_addr.sin_port = htons(RECEIVER_PORT);

    /* 3. 綁定 Socket 與連接埠 */
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind 失敗");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("UDP 接收端已啟動，監聽連接埠 %d...\n", RECEIVER_PORT);

    /* 4. 接收資料並回應 */
    for (int i = 0; i < 5; i++) {
        ssize_t recv_len = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0,
                                    (struct sockaddr *)&client_addr, &client_len);
        if (recv_len < 0) {
            perror("recvfrom 失敗");
            break;
        }

        buffer[recv_len] = '\0';
        printf("收到來自 %s:%d 的資料: %s\n",
               inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port),
               buffer);

        /* 雙向通訊：直接使用 client_addr 回傳 ACK 封包 */
        const char ack[] = "ACK: 訊息已確認送達";
        ssize_t sent_len = sendto(sockfd, ack, strlen(ack), 0,
                                  (struct sockaddr *)&client_addr, client_len);
        if (sent_len < 0) {
            perror("sendto 回應失敗");
        }
    }

    close(sockfd);
    printf("接收端結束運作。\n");
    return 0;
}