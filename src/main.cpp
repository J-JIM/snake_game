// main.cpp
// 5단계 - 스테이지 전개 및 미션 시스템 통합 구현

#include "curses_compat.h" // OS별 curses + sleep 호환 레이어
#include <locale.h>        // 한글 출력용 locale 설정
#include <ctime>           // srand 사용을 위한 ctime.
#include <cstdlib>         // srand/rand
#include <vector>
#include <string>
#include "common.h" //네임스페이스 사용.
#include "Map.h"
#include "Snake.h"
#include "Item.h"
#include "Gate.h"       // 4단계 (Gate + (3) 워프 흔적 동작 포함)
#include "ScoreBoard.h" // 5단계 - 스코어 보드 및 미션

// 1 tick 간격 (마이크로초 단위). 200000 = 0.2초
const int TICK_USEC = 200000;

int main()
{
    // 한글 출력을 위해 시스템 locale 사용
    setlocale(LC_ALL, "");

    // ===== 1) ncurses 초기화 =====
    initscr();             // ncurses 모드 시작
    start_color();         // 색상 사용 선언
    keypad(stdscr, TRUE);  // 화살표 키 입력 받기
    noecho();              // 입력한 키를 화면에 보여주지 않음
    curs_set(0);           // 커서 안 보이게
    nodelay(stdscr, TRUE); // getch()가 키 없으면 기다리지 않게 (tick 진행용)

    // 5단계 - 스테이지 리스트 준비
    std::vector<std::string> stageFiles = {
        "stages/stage1.txt",
        "stages/stage2.txt",
        "stages/stage3.txt",
        "stages/stage4.txt",
        "stages/stage5.txt"};

    int currentStageIdx = 0;
    bool totalSuccess = false;

    // ===== 2) 스테이지 루프 (5단계 - 순차적 스테이지 진행) =====
    while (currentStageIdx < (int)stageFiles.size())
    {
        Map map;
        if (map.loadFromFile(stageFiles[currentStageIdx].c_str()) == false)
        {
            endwin();
            printf("%s 를 읽을 수 없습니다.\n", stageFiles[currentStageIdx].c_str());
            return 1;
        }

        map.initColors();

        // ===== 3) 객체 초기화 =====
        Snake snake;
        if (snake.initFromMap(map) == false)
        {
            endwin();
            printf("스테이지 %d: 맵에 뱀(3, 4)이 없습니다.\n", currentStageIdx + 1);
            return 1;
        }

        ScoreBoard scoreBoard(currentStageIdx + 1, map.countInternalWalls());
        scoreBoard.updateLength(snake.getLength());

        Item growthItem = {0, 0, false, 0, 0, 0, false};
        Item poisonItem = {0, 0, false, 0, 0, 0, false};
        Item speedItem = {0, 0, false, 0, 0, 0, false};
        Gate gate;

        bool stageClear = false;
        bool gameOver = false;

        // ===== 4) 게임 루프 (단일 스테이지 내부) =====
        while (!stageClear && !gameOver)
        {
            // (a) 화면 다시 그리기
            clear();
            map.draw(0, 0);
            scoreBoard.draw(0, map.getWidth() * CELL_WIDTH + 4);
            mvprintw(map.getHeight() + 1, 0,
                     "[Stage %d] 화살표 = 이동, q = 종료", currentStageIdx + 1);
            refresh();

            // (b) 키 입력 처리 및 tick 대기 (입력 반응성을 높이기 위해 polling 방식 사용)
            int waitTime = isSpeedActive(speedItem) ? TICK_USEC / 2 : TICK_USEC;
            int elapsed = 0;
            const int pollInterval = 10000; // 5단계 - 10ms 단위

            while (elapsed < waitTime)
            {
                int key = getch();
                if (key != ERR)
                {
                    if (key == 'q' || key == 'Q')
                    {
                        endwin();
                        return 0;
                    }
                    else if (key == KEY_UP)
                        snake.requestDirection(DIR_UP);
                    else if (key == KEY_DOWN)
                        snake.requestDirection(DIR_DOWN);
                    else if (key == KEY_LEFT)
                        snake.requestDirection(DIR_LEFT);
                    else if (key == KEY_RIGHT)
                        snake.requestDirection(DIR_RIGHT);
                }
                sleep_usec(pollInterval);
                elapsed += pollInterval;
            }

            // (c) 뱀 이동 및 상태 업데이트
            prepareSpeedItem(map, speedItem, SPEED_ITEM);

            // 미션 가능 여부 체크
            scoreBoard.setInternalWalls(map.countInternalWalls());
            if (!scoreBoard.canCompleteGateMission(gate.isActive()))
            {
                gameOver = true;
                mvprintw(map.getHeight() + 3, 0, "Mission Impossible! 게이트 생성을 위한 벽이 부족합니다.");
                refresh();
                break;
            }

            int prevGateUseCount = gate.getUseCount();
            gate.update(map, snake);
            int target = snake.move(map, &gate);

            if (gate.getUseCount() > prevGateUseCount)
            {
                scoreBoard.addGate();
            }

            if (target == -1)
            {
                gameOver = true;
            }
            else
            {
                if (target == GROWTH_ITEM)
                    scoreBoard.addGrowth();
                else if (target == POISON_ITEM)
                    scoreBoard.addPoison();
                else if (target == SPEED_ITEM)
                    scoreBoard.addSpeed();

                scoreBoard.updateLength(snake.getLength());

                bool shouldUpdate = updateSpeedItem(map, speedItem, SPEED_ITEM);
                if (shouldUpdate)
                {
                    updateItem(map, growthItem, GROWTH_ITEM);
                    updateItem(map, poisonItem, POISON_ITEM);
                }

                if (scoreBoard.isAllMissionComplete())
                {
                    stageClear = true;
                }
            }
        }

        // 결과 처리
        if (gameOver)
        {
            mvprintw(map.getHeight() + 2, 0, "Game Over! 아무 키나 누르면 종료.");
            refresh();
            nodelay(stdscr, FALSE);
            getch();
            break;
        }

        if (stageClear)
        {
            currentStageIdx++;
            if (currentStageIdx < (int)stageFiles.size())
            {
                clear();
                mvprintw(10, 10, "스테이지 %d 클리어! 아무 키를 눌러 다음 스테이지를 시작하세요.", currentStageIdx);
                refresh();
                nodelay(stdscr, FALSE);
                getch();
                nodelay(stdscr, TRUE);
            }
            else
            {
                totalSuccess = true;
            }
        }
    }

    // ===== 5) 최종 결과 출력 (5단계 - 모든 스테이지 클리어) =====
    if (totalSuccess)
    {
        clear();
        attron(A_BOLD | COLOR_PAIR(GROWTH_ITEM));
        mvprintw(10, 10, "모든 스테이지 클리어!");
        attroff(A_BOLD | COLOR_PAIR(GROWTH_ITEM));
        mvprintw(12, 10, "축하합니다! 아무 키나 누르면 종료합니다.");
        refresh();
        nodelay(stdscr, FALSE);
        getch();
    }

    endwin();
    return 0;
}
