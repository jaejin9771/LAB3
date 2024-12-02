#include <stdio.h>
#include <string.h>
#include <time.h>

#define MAX_SENTENCES 3
#define MAX_LENGTH 100

void clear_input_buffer() {
    while (getchar() != '\n'); // Clear the buffer
}

int main() {
    const char *sentences[MAX_SENTENCES] = {
        "The quick brown fox jumps over the lazy dog.",
        "C programming is fun and powerful.",
        "Practice makes perfect."
    };

    char user_input[MAX_LENGTH];
    int total_mistakes = 0;
    int total_characters = 0;

    printf("타자 연습 프로그램에 오신 것을 환영합니다!\n");
    printf("다음 문장을 정확히 입력하세요.\n\n");

    // 측정을 시작할 시간 기록
    time_t start_time = time(NULL);

    for (int i = 0; i < MAX_SENTENCES; i++) {
        printf("문장 %d: %s\n", i + 1, sentences[i]);
        printf("입력: ");
        fgets(user_input, MAX_LENGTH, stdin);

        // 개행 문자를 제거
        size_t len = strlen(user_input);
        if (len > 0 && user_input[len - 1] == '\n') {
            user_input[len - 1] = '\0';
        }

        // 정확도 검사
        int mistakes = 0;
        for (size_t j = 0; j < strlen(sentences[i]); j++) {
            if (sentences[i][j] != user_input[j]) {
                mistakes++;
            }
        }

        // 결과 출력
        printf("오타: %d개\n\n", mistakes);

        // 총 오타와 입력된 문자 수 업데이트
        total_mistakes += mistakes;
        total_characters += strlen(user_input);
    }

    // 측정을 종료할 시간 기록
    time_t end_time = time(NULL);
    double elapsed_time = difftime(end_time, start_time); // 경과 시간(초)

    // 평균 분당 타자 수(WPM) 계산
    double minutes = elapsed_time / 60.0;
    double wpm = (total_characters / 5.0) / minutes; // WPM 기준: 5자 단위

    // 최종 결과 출력
    printf("총 오타 수: %d개\n", total_mistakes);
    printf("총 입력된 문자 수: %d자\n", total_characters);
    printf("경과 시간: %.2f초\n", elapsed_time);
    printf("평균 분당 타자 속도: %.2f WPM\n", wpm);

    return 0;
}
