#include "GameController.h"
#include "curses_compat.h"
#include <cstdlib>

static const int TICK_USEC = 200000;

GameController::GameController(int stageNum, const std::string& mapFilePath)
    : stageNum(stageNum), mapFilePath(mapFilePath), scoreBoard(stageNum, 0),
      isRunning(false), gameOver(false), stageClear(false), userQuit(false) {}

bool GameController::initialize() {
    if (map.loadFromFile(mapFilePath.c_str()) == false) {
        return false;
    }
    map.initColors();

    if (snake.initFromMap(map) == false) {
        return false;
    }

    // 맵에서 내부 벽 개수를 세어 스코어보드 재생성 및 동기화
    scoreBoard = ScoreBoard(stageNum, map.countInternalWalls());
    scoreBoard.updateLength(snake.getLength());

    // 아이템 어댑터 인스턴스 초기화
    items = ItemAdapter();
    
    isRunning = true;
    gameOver = false;
    stageClear = false;
    userQuit = false;

    // 비동기 게임 루프용 폴링으로 복원
    nodelay(stdscr, TRUE);

    return true;
}

GameResult GameController::run() {
    while (isRunning && !gameOver && !stageClear) {
        render();
        waitAndProcessInput();
        if (userQuit) {
            return GameResult::QUIT;
        }
        update();
    }

    if (gameOver) {
        return GameResult::GAME_OVER;
    }
    return GameResult::STAGE_CLEAR;
}

void GameController::waitAndProcessInput() {
    int waitTime = items.isSpeedActive() ? TICK_USEC / 2 : TICK_USEC;
    int elapsed = 0;
    const int pollInterval = 10000; // 10ms 단위 폴링

    while (elapsed < waitTime) {
        int key = getch();
        if (key != ERR) {
            if (key == 'q' || key == 'Q') {
                isRunning = false;
                userQuit = true;
                break;
            }
            else if (key == KEY_UP)    snake.requestDirection(DIR_UP);
            else if (key == KEY_DOWN)  snake.requestDirection(DIR_DOWN);
            else if (key == KEY_LEFT)  snake.requestDirection(DIR_LEFT);
            else if (key == KEY_RIGHT) snake.requestDirection(DIR_RIGHT);
        }
        sleep_usec(pollInterval);
        elapsed += pollInterval;
    }
}

void GameController::update() {
    // 뱀의 이동 직전, 속도 아이템 수명 및 상태 정보의 스냅샷 준비
    items.prepare(map);

    // 미션 가능 여부 실시간 체크
    scoreBoard.setInternalWalls(map.countInternalWalls());
    if (!scoreBoard.canCompleteGateMission(gate.isActive())) {
        gameOver = true;
        clear();
        
        int totalWidth = map.getWidth() * CELL_WIDTH + 4 + 28;
        
        // 상단 헤더 상단 중앙 정렬 계산
        std::string logoText = "SNAKE GAME";
        std::string stageText = "[ STAGE " + std::to_string(stageNum) + " ]";
        int combinedLen = logoText.length() + 3 + stageText.length(); // 중간 공백 3칸 포함
        int startX = (totalWidth - combinedLen) / 2;
        if (startX < 0) startX = 0;
        
        attron(COLOR_PAIR(COLOR_PAIR_TEXT_GROWTH) | A_BOLD);
        mvprintw(0, startX, "%s", logoText.c_str());
        attroff(COLOR_PAIR(COLOR_PAIR_TEXT_GROWTH) | A_BOLD);
        
        attron(COLOR_PAIR(COLOR_PAIR_TEXT_SPEED) | A_BOLD);
        mvprintw(0, startX + logoText.length() + 3, "%s", stageText.c_str());
        attroff(COLOR_PAIR(COLOR_PAIR_TEXT_SPEED) | A_BOLD);
        
        for (int x = 0; x < totalWidth; x++) {
            mvprintw(1, x, "═");
        }

        map.draw(3, 0);
        scoreBoard.draw(3, map.getWidth() * CELL_WIDTH + 4);
        mvprintw(3 + map.getHeight() + 1, 0, "Mission Impossible! 게이트 생성을 위한 벽이 부족합니다.");
        refresh();
        sleep_usec(1500000); // 사용자가 문구를 읽을 수 있도록 1.5초 대기
        return;
    }

    int prevGateUseCount = gate.getUseCount();
    gate.update(map, snake);

    // 뱀 한 칸 이동 시도
    int target = snake.move(map, &gate);

    if (gate.getUseCount() > prevGateUseCount) {
        scoreBoard.addGate();
    }

    if (target == -1) {
        gameOver = true;
    } else {
        if (target == GROWTH_ITEM)
            scoreBoard.addGrowth();
        else if (target == POISON_ITEM)
            scoreBoard.addPoison();
        else if (target == SPEED_ITEM)
            scoreBoard.addSpeed();

        scoreBoard.updateLength(snake.getLength());

        // 아이템 틱 갱신 및 스폰 진행 (어댑터 경유)
        items.update(map, target);

        if (scoreBoard.isAllMissionComplete()) {
            stageClear = true;
        }
    }
}

void GameController::render() const {
    clear();

    int totalWidth = map.getWidth() * CELL_WIDTH + 4 + 28;
    
    // 상단 헤더 상단 중앙 정렬 계산
    std::string logoText = "SNAKE GAME";
    std::string stageText = "[ STAGE " + std::to_string(stageNum) + " ]";
    int combinedLen = logoText.length() + 3 + stageText.length(); // 중간 공백 3칸 포함
    int startX = (totalWidth - combinedLen) / 2;
    if (startX < 0) startX = 0;

    attron(COLOR_PAIR(COLOR_PAIR_TEXT_GROWTH) | A_BOLD);
    mvprintw(0, startX, "%s", logoText.c_str());
    attroff(COLOR_PAIR(COLOR_PAIR_TEXT_GROWTH) | A_BOLD);
    
    attron(COLOR_PAIR(COLOR_PAIR_TEXT_SPEED) | A_BOLD);
    mvprintw(0, startX + logoText.length() + 3, "%s", stageText.c_str());
    attroff(COLOR_PAIR(COLOR_PAIR_TEXT_SPEED) | A_BOLD);
    
    for (int x = 0; x < totalWidth; x++) {
        mvprintw(1, x, "═");
    }

    // 맵과 스코어보드를 Y=3 위치부터 그리도록 아래로 이동
    map.draw(3, 0);
    scoreBoard.draw(3, map.getWidth() * CELL_WIDTH + 4);
    refresh();
}
