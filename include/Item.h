#ifndef ITEM_H
#define ITEM_H

#include "common.h"
#include "Map.h"

struct Item {
    int y;           // 맵에서의 y 좌표
    int x;           // 맵에서의 x 좌표
    bool active;     // 현재 맵에서 존재하는가?
    int timer;       // 남은 틱 수 (맵 위 수명)
    int effectTimer; // 속도 효과 남은 틱 수 (speed 전용)
    int tickCounter; // 아이템 갱신 주기 카운터 (speed 전용)
    bool wasOnMap;   // move() 직전 맵 위 존재 여부 스냅샷 (speed 전용)
};

void spawnItem(Map& map, Item& item, int cellType);
void updateItem(Map& map, Item& item, int cellType);
// 매개변수로 cellType를 받음
// 이유: growth와 poison이 구조상 동작 방식이 같아서

// speed 아이템 전용 함수
void prepareSpeedItem(Map& map, Item& item, int cellType); // move() 직전 호출
bool updateSpeedItem(Map& map, Item& item, int cellType);  // move() 직후 호출, 다른 아이템 갱신 여부 반환
bool isSpeedActive(const Item& item);                      // 속도 효과 중인지 확인

#endif
