// GameController.h
// 게임 전체 흐름을 제어하는 클래스.
//   - 한 스테이지의 실행 루프(입력 수집 → 상태 갱신 → 화면 렌더링)를 담당
//   - 인트로 / 도움말 / 랭킹 보드 / 스테이지 클리어·게임오버·최종 성공 화면 관리
//   - Map·Snake·Item·Gate·BlockWall·ScoreBoard 를 소유하고 조율한다
//   - RankingManager 와 연동해 플레이 결과를 랭킹에 기록한다

#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <string>
#include <vector>
#include "Map.h"
#include "Snake.h"
#include "ScoreBoard.h"
#include "Gate.h"
#include "Item.h"
#include "BlockWall.h"
#include "RankingManager.h"

enum class GameResult {
    STAGE_CLEAR,
    GAME_OVER,
    QUIT
};

class GameController {
public:
    GameController(int stageNum, const std::string& mapFilePath);
    ~GameController() = default;

    // 초기화: 맵 로드, 뱀 생성, 스코어보드/아이템 생성
    bool initialize();

    // 단일 스테이지 실행 루프
    GameResult run();

    // 인트로 화면 표시
    static bool showIntroScreen(RankingManager& rankingManager);

    // 도움말/조작법 화면 표시
    static void showHelpScreen();

    // 랭킹 보드 화면 표시
    // A 또는 Q 입력에 따라 true/false 반환
    static bool showRankingBoardScreen(RankingManager& rankingManager, int initialStage, const std::string& bottomMessage, bool allowSwitch);

    // 랭킹 테이블 그리기 헬퍼 함수
    static void drawRankingTable(const std::vector<RankingRecord>& ranks, int stageFilter, const std::string& bottomMessage);

    // 스테이지 결과 스크린 출력
    // 랭킹 보드 연동, A는 true, Q는 false 반환
    bool showStageClearScreen(RankingManager& rankingManager) const;
    bool showGameOverScreen(RankingManager& rankingManager) const;
    
    // 최종 성공 스크린 
    // 모든 스테이지 통과, 랭킹 보드 연동, A는 true, Q는 false 반환
    static bool showTotalClearScreen(RankingManager& rankingManager, int maxLength, int growth, int poison, int speed, int gate);

    // 점수 조회 게터
    // 최종 랭킹 기록용
    int getMaxLength() const { return scoreBoard.getMaxLength(); }
    int getGrowthCount() const { return scoreBoard.getGrowthCount(); }
    int getPoisonCount() const { return scoreBoard.getPoisonCount(); }
    int getSpeedCount() const { return scoreBoard.getSpeedCount(); }
    int getGateCount() const { return scoreBoard.getGateCount(); }

private:
    // Game Loop 
    void waitAndProcessInput(); // 입력 수집 & 틱 시간 대기
    void update();              // 상태 업데이트
    void render() const;        // 화면 렌더링

private:
    int stageNum;
    std::string mapFilePath;

    Map map;
    Snake snake;
    ScoreBoard scoreBoard;
    Gate gate;
    Item growthItem;
    Item poisonItem;
    Item speedItem;
    BlockWall blockWall;

    bool isRunning;
    bool gameOver;
    bool stageClear;
    bool userQuit;
};

#endif
