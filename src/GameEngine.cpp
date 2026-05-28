#include "GameEngine.h"
#include "GameController.h"
#include "curses_compat.h"
#include <cstdlib>
#include <algorithm>

// UTF-8 문자열의 시각적 셀 너비를 구하는 헬퍼 함수 (한글/특수기호 대응)
static int getUtf8VisualWidth(const std::string& str) {
    int width = 0;
    for (size_t i = 0; i < str.length(); ) {
        unsigned char c = str[i];
        if (c < 0x80) {
            width += 1;
            i += 1;
        } else if ((c & 0xE0) == 0xC0) {
            width += 2;
            i += 2;
        } else if ((c & 0xF0) == 0xE0) { // 한글 및 ◀, ▶ 등
            width += 2;
            i += 3;
        } else {
            i += 1;
        }
    }
    return width;
}

GameEngine::GameEngine() {
    stageFiles = {
        "stages/stage1.txt",
        "stages/stage2.txt",
        "stages/stage3.txt",
        "stages/stage4.txt",
        "stages/stage5.txt"
    };
    rankingManager.loadFromFile();
}

void GameEngine::start() {
    // 💡 인트로 화면이 처음 켜질 때도 색상이 예쁘게 출력되도록 색상 페어 사전 초기화!
    Map().initColors();

    while (true) {
        // (a) 인트로 화면 출력 (시작 키 'A'가 눌리면 true, 'Q'가 눌리면 false)
        if (showIntroScreen() == false) {
            break; // 프로그램 즉시 종료
        }

        int currentStageIdx = 0;
        bool totalSuccess = false;
        
        // 최종 클리어 시 상위 랭킹에 기록할 5단계 최종 데이터 저장용 변수
        int finalMax = 0, finalGrowth = 0, finalPoison = 0, finalSpeed = 0, finalGate = 0;

        // (b) 스테이지 루프 (순차적 스테이지 진행)
        while (currentStageIdx < (int)stageFiles.size()) {
            GameController game(currentStageIdx + 1, stageFiles[currentStageIdx]);

            if (game.initialize() == false) {
                endwin();
                printf("%s 를 읽을 수 없거나 초기화에 실패했습니다.\n", stageFiles[currentStageIdx].c_str());
                return;
            }

            // 게임 루프 실행
            GameResult result = game.run();

            if (result == GameResult::QUIT) {
                // 게임 중 'Q' 키를 누른 경우 -> 인트로 화면으로 복구
                break;
            }
            else if (result == GameResult::GAME_OVER) {
                // 게임 오버: 랭킹 등록 및 스크린 출력
                // A 누르면 true(1단계 재시작), Q 누르면 false(인트로 화면 이동)
                bool playAgain = handleGameOver(
                    currentStageIdx + 1,
                    game.getMaxLength(),
                    game.getGrowthCount(),
                    game.getPoisonCount(),
                    game.getSpeedCount(),
                    game.getGateCount()
                );
                
                if (playAgain) {
                    currentStageIdx = 0; // 처음(1단계)부터 다시 시작
                    continue;
                } else {
                    break; // 인트로 화면으로 돌아감
                }
            }
            else if (result == GameResult::STAGE_CLEAR) {
                currentStageIdx++;
                if (currentStageIdx < (int)stageFiles.size()) {
                    // 스테이지 클리어: 랭킹 등록 및 스크린 출력
                    // A 누르면 true(다음 단계 계속), Q 누르면 false(인트로 이동)
                    bool keepGoing = handleStageClear(
                        currentStageIdx, // 이전 스테이지 번호
                        game.getMaxLength(),
                        game.getGrowthCount(),
                        game.getPoisonCount(),
                        game.getSpeedCount(),
                        game.getGateCount()
                    );
                    
                    if (keepGoing == false) {
                        break; // 인트로 화면으로 돌아감
                    }
                }
                else {
                    // 모든 스테이지 통과 판정
                    totalSuccess = true;
                    finalMax = game.getMaxLength();
                    finalGrowth = game.getGrowthCount();
                    finalPoison = game.getPoisonCount();
                    finalSpeed = game.getSpeedCount();
                    finalGate = game.getGateCount();
                }
            }
        }

        // (c) 최종 전체 성공 화면 (모든 스테이지 클리어 성공)
        if (totalSuccess) {
            // 랭킹 등록 및 게임 클리어 보드 출력
            // A 누르면 다시 시작
            handleTotalClear(finalMax, finalGrowth, finalPoison, finalSpeed, finalGate);
        }
    }
}

bool GameEngine::showIntroScreen() {
    nodelay(stdscr, FALSE); // 동기식 키 입력으로 일시 전환
    bool needRedraw = true;
    while (true) {
        if (needRedraw) {
            clear();
            int H, W;
            getmaxyx(stdscr, H, W);
            
            int startY = (H > 22) ? (H - 22) / 2 : 0;
            int startX = (W > 82) ? (W - 82) / 2 : 0;

            // ASCII Art 로고 출력 (녹색 테마)
            attron(COLOR_PAIR(COLOR_PAIR_TEXT_GROWTH) | A_BOLD);
            mvprintw(startY, startX,     "  ██████  ███    ██  █████  ██   ██ ███████      ██████   █████  ███    ███ ███████");
            mvprintw(startY + 1, startX, " ██       ████   ██ ██   ██ ██  ██  ██          ██       ██   ██ ████  ████ ██     ");
            mvprintw(startY + 2, startX, "  █████   ██ ██  ██ ███████ █████   █████       ██   ███ ███████ ██ ████ ██ █████  ");
            mvprintw(startY + 3, startX, "      ██  ██  ██ ██ ██   ██ ██  ██  ██          ██    ██ ██   ██ ██  ██  ██ ██     ");
            mvprintw(startY + 4, startX, " ██████   ██   ████ ██   ██ ██   ██ ███████      ██████  ██   ██ ██      ██ ███████");
            attroff(COLOR_PAIR(COLOR_PAIR_TEXT_GROWTH) | A_BOLD);

            // 메뉴 프레임 상자
            int boxX = startX + 11;
            int boxY = startY + 7;
            
            attron(COLOR_PAIR(COLOR_PAIR_TEXT_SPEED) | A_BOLD);
            mvprintw(boxY, boxX,     "╔══════════════════════════════════════════════════════════╗");
            mvprintw(boxY + 1, boxX, "║                     MAIN MENU SELECT                     ║");
            mvprintw(boxY + 2, boxX, "╠══════════════════════════════════════════════════════════╣");
            attroff(COLOR_PAIR(COLOR_PAIR_TEXT_SPEED) | A_BOLD);
            
            mvprintw(boxY + 3, boxX, "║  [ A ] 키를 눌러 게임 시작                               ║");
            mvprintw(boxY + 4, boxX, "║  [ B ] 키를 눌러 랭킹 확인                               ║");
            mvprintw(boxY + 5, boxX, "║  [ ? ] 키를 눌러 도움말 및 조작법                        ║");
            mvprintw(boxY + 6, boxX, "║  [ Q ] 키를 눌러 게임 종료                               ║");
            mvprintw(boxY + 7, boxX, "╚══════════════════════════════════════════════════════════╝");

            attron(A_DIM);
            mvprintw(boxY + 9, boxX + 6, "© 2026 C++ Snake Game Project - Excellent Aesthetics");
            attroff(A_DIM);

            refresh();
            needRedraw = false;
        }

        int key = getch();
        if (key == ERR) {
            sleep_usec(10000); // 10ms 대기하여 CPU 100% 폭주 방지
            continue;
        }
        if (key == 'a' || key == 'A') {
            nodelay(stdscr, TRUE); // 다시 비동기 게임 루프용 폴링으로 원복
            return true;
        }
        else if (key == 'q' || key == 'Q') {
            return false;
        }
        else if (key == 'b' || key == 'B') {
            showRankingBoardScreen(0, "[◀ / ▶] 방향키를 입력해 다른 스테이지의 랭킹을 확인할 수 있습니다. (돌아가기: [ A ])", true);
            needRedraw = true;
        }
        else if (key == '?' || key == 'h' || key == 'H') {
            showHelpScreen();
            needRedraw = true;
        }
        else if (key == KEY_RESIZE) {
            needRedraw = true;
        }
    }
}

void GameEngine::showHelpScreen() {
    nodelay(stdscr, FALSE);
    clear();
    int H, W;
    getmaxyx(stdscr, H, W);
    (void)H;
    
    int startX = (W > 70) ? (W - 70) / 2 : 0;
    int startY = 1;

    attron(COLOR_PAIR(COLOR_PAIR_TEXT_USED_GATE) | A_BOLD);
    mvprintw(startY, startX,     "╔════════════════════════════════════════════════════════════════════╗");
    mvprintw(startY + 1, startX, "║                        HELP & GAME CONTROLS                        ║");
    mvprintw(startY + 2, startX, "╠════════════════════════════════════════════════════════════════════╣");
    attroff(COLOR_PAIR(COLOR_PAIR_TEXT_USED_GATE) | A_BOLD);

    mvprintw(startY + 3, startX, "║  - 조작법                                                          ║");
    mvprintw(startY + 4, startX, "║    - 방향키 [ ↑ | ↓ | ← | → ] : 뱀 이동 방향 전환                  ║");
    mvprintw(startY + 5, startX, "║    - [ Q ] 키 : 게임 중 강제 종료 및 이전 화면 이동                ║");
    mvprintw(startY + 6, startX, "║                                                                    ║");
    
    mvprintw(startY + 7, startX, "║  - 게임 아이템 종류                                                ║");
    
    // Growth Item
    mvprintw(startY + 8, startX, "║    - ");
    attron(COLOR_PAIR(GROWTH_ITEM));
    printw("  ");
    attroff(COLOR_PAIR(GROWTH_ITEM));
    attron(COLOR_PAIR(COLOR_PAIR_TEXT_GROWTH));
    printw(" Growth Item (초록색) : 뱀의 몸통 길이 1 증가             ");
    attroff(COLOR_PAIR(COLOR_PAIR_TEXT_GROWTH));
    mvprintw(startY + 8, startX + 69, "║");
    
    // Poison Item
    mvprintw(startY + 9, startX, "║    - ");
    attron(COLOR_PAIR(POISON_ITEM));
    printw("  ");
    attroff(COLOR_PAIR(POISON_ITEM));
    attron(COLOR_PAIR(COLOR_PAIR_TEXT_POISON));
    printw(" Poison Item (빨간색) : 뱀의 몸통 길이 1 감소             ");
    attroff(COLOR_PAIR(COLOR_PAIR_TEXT_POISON));
    mvprintw(startY + 9, startX + 69, "║");

    // Speed Item
    mvprintw(startY + 10, startX, "║    - ");
    attron(COLOR_PAIR(SPEED_ITEM));
    printw("  ");
    attroff(COLOR_PAIR(SPEED_ITEM));
    attron(COLOR_PAIR(COLOR_PAIR_TEXT_SPEED));
    printw(" Speed Item  (시안색) : 뱀의 이동속도 2배 (30틱간 지속)   ");
    attroff(COLOR_PAIR(COLOR_PAIR_TEXT_SPEED));
    mvprintw(startY + 10, startX + 69, "║");

    // Warp Gate
    mvprintw(startY + 11, startX, "║    - ");
    attron(COLOR_PAIR(GATE));
    printw("  ");
    attroff(COLOR_PAIR(GATE));
    attron(COLOR_PAIR(COLOR_PAIR_TEXT_GATE));
    printw(" Warp Gate   (자홍색) : 머리 진입 시 반대편 게이트 워프   ");
    attroff(COLOR_PAIR(COLOR_PAIR_TEXT_GATE));
    mvprintw(startY + 11, startX + 69, "║");

    // Used Wall
    mvprintw(startY + 12, startX, "║    - ");
    attron(COLOR_PAIR(USED_GATE_WALL));
    printw("  ");
    attroff(COLOR_PAIR(USED_GATE_WALL));
    attron(COLOR_PAIR(COLOR_PAIR_TEXT_USED_GATE));
    printw(" Used Wall   (노란색) : 한 번 사용된 워프 흔적 (재이용 불가)");
    attroff(COLOR_PAIR(COLOR_PAIR_TEXT_USED_GATE));
    mvprintw(startY + 12, startX + 69, "║");

    mvprintw(startY + 13, startX, "║                                                                    ║");
    mvprintw(startY + 14, startX, "║  - 게임 규칙 및 실패 조건                                          ║");
    mvprintw(startY + 15, startX, "║    - 각 스테이지별 우측 미션 목표를 전부 완수 시 클리어            ║");
    mvprintw(startY + 16, startX, "║    - 뱀의 머리가 벽에 충돌 시 실패                                 ║");
    mvprintw(startY + 17, startX, "║    - 독약을 먹고 뱀의 몸통 길이가 3 미만이 될 시 실패 (게임 오버)  ║");
    mvprintw(startY + 18, startX, "║    - 맵 내 남은 벽이 부족해져 게이트 미션 달성이 불가할 시 실패    ║");
    mvprintw(startY + 19, startX, "╚════════════════════════════════════════════════════════════════════╝");

    attron(A_BOLD | COLOR_PAIR(COLOR_PAIR_TEXT_SPEED));
    mvprintw(startY + 21, startX + 14, "[ A ] 키를 누르면 이전 시작화면으로 이동합니다.");
    attroff(A_BOLD | COLOR_PAIR(COLOR_PAIR_TEXT_SPEED));

    refresh();

    while (true) {
        int key = getch();
        if (key == ERR) {
            sleep_usec(10000); // 10ms 대기하여 CPU 100% 폭주 방지
            continue;
        }
        if (key == 'a' || key == 'A') {
            break;
        }
    }
}

bool GameEngine::showRankingBoardScreen(int initialStage, const std::string& bottomMessage, bool allowSwitch) {
    nodelay(stdscr, FALSE);
    int stageFilter = initialStage; // 0 = 통합 랭킹, 1~5 = 개별 스테이지
    bool needRedraw = true;

    while (true) {
        if (needRedraw) {
            auto ranks = rankingManager.getRankings(stageFilter);
            drawRankingTable(ranks, stageFilter, bottomMessage);
            needRedraw = false;
        }

        int key = getch();
        if (key == ERR) {
            sleep_usec(10000); // 10ms 대기하여 CPU 100% 폭주 방지
            continue;
        }
        if (key == 'a' || key == 'A') {
            return true;
        }
        else if (key == 'q' || key == 'Q') {
            return false;
        }
        else if (allowSwitch) {
            if (key == KEY_LEFT) {
                stageFilter--;
                if (stageFilter < 0) stageFilter = 5;
                needRedraw = true;
            }
            else if (key == KEY_RIGHT) {
                stageFilter++;
                if (stageFilter > 5) stageFilter = 0;
                needRedraw = true;
            }
        }
        else if (key == KEY_RESIZE) {
            needRedraw = true;
        }
    }
}

void GameEngine::drawRankingTable(const std::vector<RankingRecord>& ranks, int stageFilter, const std::string& bottomMessage) {
    clear();
    int H, W;
    getmaxyx(stdscr, H, W);
    (void)H;
    
    // 표 전체 너비 89칸
    int boxWidth = 89;
    int startX = (W > boxWidth) ? (W - boxWidth) / 2 : 0;
    int startY = 1;

    attron(A_BOLD | COLOR_PAIR(COLOR_PAIR_TEXT_SPEED));
    mvprintw(startY, startX,     "╔═══════════════════════════════════════════════════════════════════════════════════════╗");
    if (stageFilter == 0) {
        mvprintw(startY + 1, startX, "║                               ALL STAGES RANKING BOARD                                ║");
    } else {
        mvprintw(startY + 1, startX, "║                                STAGE %d RANKING BOARD                                 ║", stageFilter);
    }
    mvprintw(startY + 2, startX, "╠═══════════════════════════════════════════════════════════════════════════════════════╣");
    attroff(A_BOLD | COLOR_PAIR(COLOR_PAIR_TEXT_SPEED));

    // 테이블 헤더
    attron(A_BOLD);
    mvprintw(startY + 3, startX, "║ 순위 │  단계  │ 최대길이 │ Growth │ Poison │ Speed │ Gate통과 │      플레이 일시      ║");
    mvprintw(startY + 4, startX, "╠══════╪════════╪══════════╪════════╪════════╪═══════╪══════════╪═══════════════════════╣");
    attroff(A_BOLD);

    // 최대 11개 행 표출 (10등 내 순위 + 방금 플레이한 외각 순위 포함)
    for (int i = 0; i < 11; i++) {
        int rowY = startY + 5 + i;
        if (i < (int)ranks.size()) {
            const auto& r = ranks[i];
            
            if (r.isCurrentPlay) {
                attron(COLOR_PAIR(COLOR_PAIR_TEXT_GROWTH) | A_BOLD);
                if (r.rank <= 10) {
                    mvprintw(rowY, startX, "║ >%2d위│  %d단계 │    %2d    │   %2d   │   %2d   │  %2d   │    %2d    │  %s  ║",
                             r.rank, r.stage, r.maxLength, r.growthCount, r.poisonCount, r.speedCount, r.gateCount, r.timestamp.c_str());
                } else {
                    mvprintw(rowY, startX, "║내기록│  %d단계 │    %2d    │   %2d   │   %2d   │  %2d   │    %2d    │  %s  ║",
                             r.stage, r.maxLength, r.growthCount, r.poisonCount, r.speedCount, r.gateCount, r.timestamp.c_str());
                }
                attroff(COLOR_PAIR(COLOR_PAIR_TEXT_GROWTH) | A_BOLD);
            } else {
                // 상위 3등 특별 색상 강조
                if (r.rank == 1) {
                    attron(COLOR_PAIR(COLOR_PAIR_TEXT_USED_GATE) | A_BOLD); // 금빛
                } else if (r.rank == 2) {
                    attron(COLOR_PAIR(COLOR_PAIR_TEXT_SPEED) | A_BOLD); // 은빛
                } else if (r.rank == 3) {
                    attron(COLOR_PAIR(COLOR_PAIR_TEXT_GATE) | A_BOLD); // 동빛
                }
                
                mvprintw(rowY, startX, "║  %2d위│  %d단계 │    %2d    │   %2d   │   %2d   │  %2d   │    %2d    │  %s  ║",
                         r.rank, r.stage, r.maxLength, r.growthCount, r.poisonCount, r.speedCount, r.gateCount, r.timestamp.c_str());
                         
                if (r.rank <= 3) {
                    attroff(A_BOLD | COLOR_PAIR(COLOR_PAIR_TEXT_USED_GATE) | COLOR_PAIR(COLOR_PAIR_TEXT_SPEED) | COLOR_PAIR(COLOR_PAIR_TEXT_GATE));
                }
            }
        } else {
            // 빈 공간 채워넣기
            mvprintw(rowY, startX, "║      │        │          │        │        │       │          │                       ║");
        }
    }

    mvprintw(startY + 16, startX, "╚═══════════════════════════════════════════════════════════════════════════════════════╝");

    // 하단 정보 메시지
    attron(A_BOLD);
    int visualWidth = getUtf8VisualWidth(bottomMessage);
    int textX = startX + (boxWidth - visualWidth) / 2;
    if (textX < startX) textX = startX;
    mvprintw(startY + 18, textX, "%s", bottomMessage.c_str());
    attroff(A_BOLD);
    
    refresh();
}

bool GameEngine::handleGameOver(int stageNum, int maxLen, int growth, int poison, int speed, int gate) {
    rankingManager.addRecord(stageNum, maxLen, growth, poison, speed, gate);
    rankingManager.markLatestAsCurrent();
    rankingManager.saveToFile();

    bool playAgain = showRankingBoardScreen(stageNum, "GAME OVER! [ A ] 처음부터 재시작 | [ Q ] 종료 | [◀ / ▶] 다른 랭킹", true);
    rankingManager.clearCurrentPlayFlag();
    return playAgain;
}

bool GameEngine::handleStageClear(int stageNum, int maxLen, int growth, int poison, int speed, int gate) {
    rankingManager.addRecord(stageNum, maxLen, growth, poison, speed, gate);
    rankingManager.markLatestAsCurrent();
    rankingManager.saveToFile();

    bool keepGoing = showRankingBoardScreen(stageNum, "STAGE CLEAR! [ A ] 다음 단계 계속 | [ Q ] 종료 | [◀ / ▶] 다른 랭킹", true);
    rankingManager.clearCurrentPlayFlag();
    return keepGoing;
}

bool GameEngine::handleTotalClear(int maxLen, int growth, int poison, int speed, int gate) {
    rankingManager.addRecord(5, maxLen, growth, poison, speed, gate);
    rankingManager.markLatestAsCurrent();
    rankingManager.saveToFile();

    bool playAgain = showRankingBoardScreen(0, "ALL STAGES CLEAR! [ A ] 처음부터 재시작 | [ Q ] 종료 | [◀ / ▶] 다른 랭킹", true);
    rankingManager.clearCurrentPlayFlag();
    return playAgain;
}
