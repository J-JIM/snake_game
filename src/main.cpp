// main.cpp
// Snake Game - 1단계, 2단계 통합
// 게임 화면을 띄우고, 화살표 키로 뱀을 움직인다.

#include "curses_compat.h"  // OS별 curses + sleep 호환 레이어
#include <locale.h>          // 한글 출력용 locale 설정
#include <ctime>             // srand 사용을 위한 ctime.
#include <cstdlib>           // srand/rand
#include "common.h"          //네임스페이스 사용.
#include "Map.h"
#include "Snake.h"
#include "Item.h"
#include "Gate.h"            // 4단계 (Gate + (3) 워프 흔적 동작 포함)

// 1 tick 간격 (마이크로초 단위). 200000 = 0.2초
// !) tick은 고정 tick으로 진행했습니다. 모든 단계 완료되면 추가 수정으로 진행하려고 합니다.
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

    // ===== 2) 맵 불러오기 (4단계는 stage4.txt — island wall 포함) =====
    Map map;
    bool ok = map.loadFromFile("stages/stage4.txt");
    if (ok == false) {
        endwin();
        printf("stage4.txt 를 읽을 수 없습니다.\n");
        return 1;
    }

    // 맵에 사용할 색상 초기화 (start_color 다음에 한 번만)
    map.initColors();

    // ===== 3) 뱀 초기화 (맵에서 머리/몸통 위치 읽어오기) =====
    Snake snake;

    // struct에 따라 growthItem, poisonItem, speedItem 정의
    Item growthItem = {0, 0, false, 0, 0, 0, false};
    Item poisonItem = {0, 0, false, 0, 0, 0, false};
    Item speedItem  = {0, 0, false, 0, 0, 0, false};

    // 4단계: Gate 한 쌍 관리 객체
    // (Gate가 사라질 때 그 자리는 USED_GATE_WALL 로 바뀌어 다시는 Gate가 안 뜸)
    Gate gate;

    srand((unsigned int)time(NULL)); // 랜덤 시드

    // rand()는 사실상 시드 값 기반의 "가짜 난수"
    // srand(time(NULL))은 현재 시각을 시드로 사용함.
    // 게임 실행 시 매번 다른 위치에 아이템을 생성시킬 수 있음.

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
                 "[Stage 4] 화살표 = 이동, q = 종료  |  Gate 사용: %d",
                 gate.getUseCount());
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

        // (c) 뱀 한 칸 이동 (Gate 통과 처리는 snake.move 내부에서 위임)
        prepareSpeedItem(map, speedItem, SPEED_ITEM);  // move() 전 스냅샷

        // 4단계: Gate 출현/이동 + 사라진 자리는 USED_GATE_WALL 로 자동 변환
        gate.update(map, snake);

        bool alive = snake.move(map, &gate);

        bool shouldUpdate = updateSpeedItem(map, speedItem, SPEED_ITEM);
        if (shouldUpdate) {
            updateItem(map, growthItem, GROWTH_ITEM);
            updateItem(map, poisonItem, POISON_ITEM);
        }

        if (alive == false) {
            // 벽 충돌 / 자기 몸통 충돌 / 반대 방향 입력 → 게임 오버
            mvprintw(map.getHeight() + 2, 0,
                     "Game Over! 아무 키나 누르면 종료.");
            refresh();
            nodelay(stdscr, FALSE);  // 키 기다리도록 다시 켬
            getch();
            running = false;
        }

        // (d) tick 만큼 쉬기 (OS별 wrapper)
        sleep_usec(isSpeedActive(speedItem) ? TICK_USEC / 2 : TICK_USEC);
    }

    // ===== 5) ncurses 종료 =====
    endwin();
    return 0;
}
