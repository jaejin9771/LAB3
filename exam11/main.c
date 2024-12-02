#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <fcntl.h>

#define SHM_SIZE 4096
#define SEM_KEY 12345

// 세마포어 유니온 정의
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

// 세마포어 초기화 함수
// sem_id: 세마포어 집합 ID, sem_num: 세마포어 번호, value: 초기화할 값
int init_semaphore(int sem_id, int sem_num, int value) {
    union semun sem_union;
    sem_union.val = value;
    if (semctl(sem_id, sem_num, SETVAL, sem_union) == -1) {
        perror("세마포어 초기화 실패");
        return -1;
    }
    return 0;
}

// 세마포어 P 연산 (잠금)
// sem_id: 세마포어 집합 ID, sem_num: 잠금할 세마포어 번호
void sem_wait(int sem_id, int sem_num) {
    struct sembuf sb = {sem_num, -1, 0};
    if (semop(sem_id, &sb, 1) == -1) {
        perror("세마포어 잠금 실패");
        exit(1);
    }
}

// 세마포어 V 연산 (해제)
// sem_id: 세마포어 집합 ID, sem_num: 해제할 세마포어 번호
void sem_signal(int sem_id, int sem_num) {
    struct sembuf sb = {sem_num, 1, 0};
    if (semop(sem_id, &sb, 1) == -1) {
        perror("세마포어 해제 실패");
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("사용법: %s <원본파일> <대상파일>\n", argv[0]);
        exit(1);
    }

    // 공유 메모리 생성
    int shm_id = shmget(IPC_PRIVATE, SHM_SIZE, IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("공유 메모리 생성 실패");
        exit(1);
    }

    // 세마포어 생성 (2개의 세마포어 생성)
    int sem_id = semget(SEM_KEY, 2, IPC_CREAT | 0666);
    if (sem_id == -1) {
        perror("세마포어 생성 실패");
        exit(1);
    }

    // 세마포어 초기화 (0번: read_sem = 1, 1번: write_sem = 0)
    if (init_semaphore(sem_id, 0, 1) == -1 || init_semaphore(sem_id, 1, 0) == -1) {
        exit(1);
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork 실패");
        exit(1);
    }

    // 공유 메모리 연결
    char *shared_memory = (char *)shmat(shm_id, NULL, 0);
    if (shared_memory == (char *)-1) {
        perror("공유 메모리 연결 실패");
        exit(1);
    }

    if (pid > 0) {  // 부모 프로세스 (읽기 역할)
        int fd = open(argv[1], O_RDONLY);
        if (fd == -1) {
            perror("원본 파일 열기 실패");
            exit(1);
        }

        int bytes_read;
        while ((bytes_read = read(fd, shared_memory, SHM_SIZE - sizeof(int))) > 0) {
            sem_wait(sem_id, 0);  // 읽기 세마포어 잠금 (부모가 공유 메모리에 데이터를 쓸 수 있도록 잠금)

            // 읽은 크기 저장 (공유 메모리 끝 부분에 저장)
            *((int *)(shared_memory + SHM_SIZE - sizeof(int))) = bytes_read;

            sem_signal(sem_id, 1);  // 쓰기 프로세스에게 신호 (자식이 데이터를 파일에 쓸 수 있도록 해제)
        }

        // EOF 표시를 위해 크기 0을 전송
        sem_wait(sem_id, 0);  // 마지막으로 자식이 쓰기 전에 잠금
        *((int *)(shared_memory + SHM_SIZE - sizeof(int))) = 0; // EOF 표시
        sem_signal(sem_id, 1);  // 자식에게 EOF를 처리하도록 신호

        close(fd);
        wait(NULL);  // 자식 프로세스가 종료될 때까지 대기
    } else {  // 자식 프로세스 (쓰기 역할)
        int fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (fd == -1) {
            perror("대상 파일 열기 실패");
            exit(1);
        }

        while (1) {
            sem_wait(sem_id, 1);  // 쓰기 세마포어 잠금 (부모가 데이터를 쓸 때까지 대기)

            // 읽은 크기 확인 (EOF인지 확인)
            int bytes_to_write = *((int *)(shared_memory + SHM_SIZE - sizeof(int)));
            if (bytes_to_write == 0) {
                // EOF
                break;
            }

            // 공유 메모리의 데이터를 파일에 쓰기
            if (write(fd, shared_memory, bytes_to_write) == -1) {
                perror("파일 쓰기 실패");
                exit(1);
            }

            sem_signal(sem_id, 0);  // 읽기 프로세스에게 신호 (부모가 다음 데이터를 쓸 수 있도록 해제)
        }

        close(fd);
    }

    // 공유 메모리 해제
    if (shmdt(shared_memory) == -1) {
        perror("공유 메모리 해제 실패");
        exit(1);
    }

    // 부모 프로세스에서 공유 메모리 및 세마포어 제거
    if (pid > 0) {
        if (shmctl(shm_id, IPC_RMID, NULL) == -1) {
            perror("공유 메모리 제거 실패");
            exit(1);
        }

        if (semctl(sem_id, 0, IPC_RMID) == -1) {
            perror("세마포어 제거 실패");
            exit(1);
        }
    }

    return 0;
}
