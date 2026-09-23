#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BROADCAST_PORT 8888
#define BUFFER_SIZE 1024

int main(void) {
    int sockfd;
    struct sockaddr_in local_addr;
    char buffer[BUFFER_SIZE];

    /* 1. 建立 UDP Socket */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket 建立失敗");
        exit(EXIT_FAILURE);
    }

    /* 2. 啟用 SO_REUSEADDR 允許重複綁定相同連接埠 */
    int reuse = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        perror("setsockopt SO_REUSEADDR 失敗");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    /* 3. 綁定本地端連接埠與 INADDR_ANY (0.0.0.0)，以便接收送達本機的所有廣播封包 */
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    local_addr.sin_port = htons(BROADCAST_PORT);

    if (bind(sockfd, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0) {
        perror("bind 失敗");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("全域廣播接收端已啟動，監聽連接埠 %d...\n", BROADCAST_PORT);

    /* 4. 接收 5 次封包測試 */
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
        printf("收到來自 %s:%d 的廣播資料: %s\n",
               inet_ntoa(src_addr.sin_addr),
               ntohs(src_addr.sin_port),
               buffer);
    }

    close(sockfd);
    printf("關閉 Socket 結束接收。\n");
    return 0;
}