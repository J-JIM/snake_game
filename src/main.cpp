// main.cpp
// Snake Game - 1단계, 2단계 통합
// 게임 화면을 띄우고, 화살표 키로 뱀을 움직인다.

#include <ncurses.h>
#include <unistd.h>   // usleep (tick 간격용)
#include <locale.h>   // 한글 출력용 locale 설정
#include "common.h"
#include "Map.h"
#include "Snake.h"

// 1 tick 간격 (마이크로초 단위). 200000 = 0.2초
const int TICK_USEC = 200000;

int main() {
    // 한글 출력을 위해 시스템 locale 사용
    setlocale(LC_ALL, "");

    // ===== 1) ncurses 초기화 =====
    initscr();              // ncurses 모드 시작
    start_color();          // 색상 사용 선언
    keypad(stdscr, TRUE);   // 화살표 키 입력 받기
    noecho();               // 입력한 키를 화면에 보여주지 않음
    curs_set(0);            // 커서 안 보이게
    nodelay(stdscr, TRUE);  // getch()가 키 없으면 기다리지 않게 (tick 진행용)

    // ===== 2) 맵 불러오기 =====
    Map map;
    bool ok = map.loadFromFile("stages/stage1.txt");
    if (ok == false) {
        endwin();
        printf("stage1.txt 를 읽을 수 없습니다.\n");
        return 1;
    }

    // 맵에 사용할 색상 초기화 (start_color 다음에 한 번만)
    map.initColors();

    // ===== 3) 뱀 초기화 (맵에서 머리/몸통 위치 읽어오기) =====
    Snake snake;
    if (snake.initFromMap(map) == false) {
        endwin();
        printf("맵에 뱀(3, 4)이 없습니다.\n");
        return 1;
    }

    // ===== 4) 게임 루프 =====
    bool running = true;
    while (running == true) {
        // (a) 화면 다시 그리기
        clear();
        map.draw(0, 0);
        mvprintw(map.getHeight() + 1, 0,
                 "[Stage 1] 화살표 = 이동, q = 종료");
        refresh();

        // (b) 키 입력 처리
        int key = getch();
        if (key == 'q' || key == 'Q') {
            running = false;
        } else if (key == KEY_UP) {
            snake.requestDirection(DIR_UP);
        } else if (key == KEY_DOWN) {
            snake.requestDirection(DIR_DOWN);
        } else if (key == KEY_LEFT) {
            snake.requestDirection(DIR_LEFT);
        } else if (key == KEY_RIGHT) {
            snake.requestDirection(DIR_RIGHT);
        }

        // (c) 뱀 한 칸 이동
        bool alive = snake.move(map);
        if (alive == false) {
            // 벽 충돌 / 자기 몸통 충돌 / 반대 방향 입력 → 게임 오버
            mvprintw(map.getHeight() + 2, 0,
                     "Game Over! 아무 키나 누르면 종료.");
            refresh();
            nodelay(stdscr, FALSE);  // 키 기다리도록 다시 켬
            getch();
            running = false;
        }

        // (d) tick 만큼 쉬기
        usleep(TICK_USEC);
    }

    // ===== 5) ncurses 종료 =====
    endwin();
    return 0;
}
