#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

// my_system 함수 구현
int my_system(char *command) {
    pid_t pid;
    int status;

    // 자식 프로세스 생성
    pid = fork();
    if (pid < 0) {
        perror("fork 실패");
        return -1; // fork 실패
    }

    if (pid == 0) { // 자식 프로세스
        // 쉘 명령을 공백으로 분리
        char *args[128];
        int i = 0;
        args[i] = strtok(command, " "); // 첫 번째 명령어
        while (args[i] != NULL) {
            args[++i] = strtok(NULL, " "); // 나머지 인자
        }

        // execvp 실행
        if (execvp(args[0], args) == -1) {
            perror("명령 실행 실패");
            exit(EXIT_FAILURE);
        }
    } else { // 부모 프로세스
        // 자식 프로세스의 종료 상태를 기다림
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid 실패");
            return -1;
        }
    }

    return status;
}

// 메인 함수
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "사용법: %s <명령어>\n", argv[0]);
        return 1;
    }

    // 명령어를 하나의 문자열로 결합
    char command[256] = {0};
    for (int i = 1; i < argc; i++) {
        strcat(command, argv[i]);
        if (i < argc - 1) {
            strcat(command, " ");
        }
    }

    // my_system 호출
    printf("명령 실행: %s\n", command);
    int result = my_system(command);

    if (result == -1) {
        printf("명령 실행 중 오류 발생\n");
    } else {
        printf("명령 실행 완료, 상태: %d\n", WEXITSTATUS(result));
    }

    return 0;
}
