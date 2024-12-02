#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>

#define MSG_KEY 5678
#define MSG_SIZE 100
#define CLIENT_TO_SERVER 1
#define SERVER_TO_CLIENT 2

// 메시지 버퍼 구조체
typedef struct msg_buffer {
    long msg_type;
    char msg_text[MSG_SIZE];
} message_buf;

int msgid;

void cleanup(int sig) {
    // 메시지 큐 삭제
    if (msgctl(msgid, IPC_RMID, NULL) == -1) {
        perror("msgctl");
        exit(EXIT_FAILURE);
    }
    printf("Message queue removed. Exiting...\n");
    exit(0);
}

int main() {
    message_buf message;
    pid_t pid;

    // 메시지 큐 생성
    msgid = msgget(MSG_KEY, IPC_CREAT | 0666);
    if (msgid == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    // SIGINT 시그널 핸들러 등록 (Ctrl+C)
    signal(SIGINT, cleanup);

    // 자식 프로세스를 생성
    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // 자식 프로세스: 클라이언트 역할 (메시지 보내기)
        while (1) {
            printf("Client: Enter message: ");
            fgets(message.msg_text, MSG_SIZE, stdin);
            message.msg_text[strcspn(message.msg_text, "\n")] = 0; // 개행 문자 제거
            message.msg_type = CLIENT_TO_SERVER;
            if (msgsnd(msgid, &message, sizeof(message.msg_text), 0) == -1) {
                perror("msgsnd");
                exit(EXIT_FAILURE);
            }
            if (strcmp(message.msg_text, "exit") == 0) {
                break;
            }

            // 서버의 응답 수신
            if (msgrcv(msgid, &message, sizeof(message.msg_text), SERVER_TO_CLIENT, 0) == -1) {
                perror("msgrcv");
                exit(EXIT_FAILURE);
            }
            printf("Client received: %s\n", message.msg_text);
        }
        exit(0);
    } else {
        // 부모 프로세스: 서버 역할 (메시지 수신 및 답장)
        while (1) {
            if (msgrcv(msgid, &message, sizeof(message.msg_text), CLIENT_TO_SERVER, 0) == -1) {
                perror("msgrcv");
                exit(EXIT_FAILURE);
            }
            if (strcmp(message.msg_text, "exit") == 0) {
                printf("Client has exited. Server exiting...\n");
                cleanup(0);
            }
            printf("Server received: %s\n", message.msg_text);

            // 서버에서 답장 보내기 (응답 문자열을 별도로 만들어서 처리)
            char response[MSG_SIZE];
            snprintf(response, MSG_SIZE, "Server received: %s", message.msg_text);
            strncpy(message.msg_text, response, MSG_SIZE - 1);
            message.msg_text[MSG_SIZE - 1] = '\0'; // Null-terminate to ensure safety
            message.msg_type = SERVER_TO_CLIENT;
            if (msgsnd(msgid, &message, sizeof(message.msg_text), 0) == -1) {
                perror("msgsnd");
                exit(EXIT_FAILURE);
            }
        }
    }

    return 0;
}
