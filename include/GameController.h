#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <string>
#include "Map.h"
#include "Snake.h"
#include "ScoreBoard.h"
#include "Gate.h"
#include "Item.h"

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

    // 점수 조회 게터 (최종 랭킹 기록용)
    int getMaxLength() const { return scoreBoard.getMaxLength(); }
    int getGrowthCount() const { return scoreBoard.getGrowthCount(); }
    int getPoisonCount() const { return scoreBoard.getPoisonCount(); }
    int getSpeedCount() const { return scoreBoard.getSpeedCount(); }
    int getGateCount() const { return scoreBoard.getGateCount(); }

private:
    // Game Loop 3대 요소
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

    // 💡 동료분 코드를 100% 보존하면서 결합도를 낮추기 위한 아키텍처적 Item 어댑터 정의!
    class ItemAdapter {
    public:
        ItemAdapter() : growthItem{0,0,false,0,0,0,false},
                        poisonItem{0,0,false,0,0,0,false},
                        speedItem{0,0,false,0,0,0,false} {}

        // 뱀 이동 전, 속도 상태 스냅샷 저장
        void prepare(Map& map) {
            prepareSpeedItem(map, speedItem, SPEED_ITEM);
        }

        // 이동 완료 후 아이템 갱신 및 스폰 주기 관리
        void update(Map& map, int target) {
            bool shouldUpdate = updateSpeedItem(map, speedItem, SPEED_ITEM);
            if (shouldUpdate) {
                updateItem(map, growthItem, GROWTH_ITEM);
                updateItem(map, poisonItem, POISON_ITEM);
            }
        }

        // 속도 아이템 효과가 활성 상태인지 확인
        bool isSpeedActive() const {
            return ::isSpeedActive(speedItem);
        }

    private:
        Item growthItem;
        Item poisonItem;
        Item speedItem;
    };

    ItemAdapter items;

    bool isRunning;
    bool gameOver;
    bool stageClear;
    bool userQuit;
};

#endif
