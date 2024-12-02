#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
int main() {
    const char *dir_name = "example_dir";
    const char *file_name = "example_dir/example_file.txt";
    // 1. 디렉토리 생성
    if (mkdir(dir_name, 0755) == -1) {
        perror("디렉토리 생성 실패");
        return 1;
    }
    printf("디렉토리 '%s' 생성 완료\n", dir_name);
    // 2. 파일 생성 및 쓰기
    FILE *file = fopen(file_name, "w");
    if (file == NULL) {
        perror("파일 생성 실패");
        rmdir(dir_name);
        return 1;
    }
    fprintf(file, "Hello, File and Directory Handling!\n");
    fclose(file);
    printf("파일 '%s' 생성 및 데이터 쓰기 완료\n", file_name);
    // 3. 디렉토리 내용 읽기
    DIR *dir = opendir(dir_name);
    if (dir == NULL) {
        perror("디렉토리 열기 실패");
        unlink(file_name); // 생성된 파일 삭제
        rmdir(dir_name);   // 생성된 디렉토리 삭제
        return 1;
    }
    printf("디렉토리 '%s'의 내용:\n", dir_name);
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        printf("  %s\n", entry->d_name);
    }
    closedir(dir);
    // 4. 파일 삭제
    if (unlink(file_name) == -1) {
        perror("파일 삭제 실패");
        return 1;
    }
    printf("파일 '%s' 삭제 완료\n", file_name);
    // 5. 디렉토리 삭제
    if (rmdir(dir_name) == -1) {
        perror("디렉토리 삭제 실패");
        return 1;
    }
    printf("디렉토리 '%s' 삭제 완료\n", dir_name);
    return 0;
}
