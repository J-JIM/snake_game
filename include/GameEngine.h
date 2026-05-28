#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <vector>
#include <string>
#include "RankingManager.h"

class GameEngine {
public:
    GameEngine();
    ~GameEngine() = default;

    // 전체 게임 구동 진입점 (Intro -> Play -> End 전체 조율)
    void start();

private:
    // 전역 화면 출력 메서드 (GameController에서 분리되어 Engine으로 일원화)
    bool showIntroScreen();
    void showHelpScreen();
    bool showRankingBoardScreen(int initialStage, const std::string& bottomMessage, bool allowSwitch);
    void drawRankingTable(const std::vector<RankingRecord>& ranks, int stageFilter, const std::string& bottomMessage);
    
    // 게임 스테이지 클리어/오버 화면 연동
    bool handleGameOver(int stageNum, int maxLen, int growth, int poison, int speed, int gate);
    bool handleStageClear(int stageNum, int maxLen, int growth, int poison, int speed, int gate);
    bool handleTotalClear(int maxLen, int growth, int poison, int speed, int gate);

private:
    RankingManager rankingManager;
    std::vector<std::string> stageFiles;
};

#endif
