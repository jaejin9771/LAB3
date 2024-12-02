#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

void list_directory(const char *dir_path, int depth) {
    DIR *dir = opendir(dir_path);
    if (dir == NULL) {
        perror("디렉토리를 열 수 없습니다");
        return;
    }

    printf("\n%s:\n", dir_path); // 현재 디렉토리 이름 출력
    struct dirent *entry;
    struct stat file_stat;

    while ((entry = readdir(dir)) != NULL) {
        // '.'과 '..'은 무시
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char full_path[PATH_MAX];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);

        if (stat(full_path, &file_stat) == -1) {
            perror("파일 상태 확인 실패");
            continue;
        }

        if (S_ISDIR(file_stat.st_mode)) {
            // 디렉토리일 경우
            printf("%*s[%s]\n", depth * 2, "", entry->d_name);
            // 재귀 호출
            list_directory(full_path, depth + 1);
        } else {
            // 파일일 경우
            printf("%*s%s\n", depth * 2, "", entry->d_name);
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    // 사용자로부터 디렉토리 경로 입력 받기
    const char *path = ".";
    if (argc > 1) {
        path = argv[1];
    }

    printf("'%s' 디렉토리 내용을 재귀적으로 탐색합니다.\n", path);
    list_directory(path, 0);

    return 0;
}
