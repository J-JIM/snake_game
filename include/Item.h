// Item.h
// 3단계 - 맵 위에 출현하는 아이템 한 개를 표현하는 클래스
// 하나의 Item 객체가 Growth / Poison / Speed 중 한 종류를 담당한다.
// 종류는 생성자에서 받은 kind(셀 값)로 구분하며, 출현·수명·획득 처리를
// 멤버 함수로 캡슐화한다.

#ifndef ITEM_H
#define ITEM_H

#include "common.h"

class Map; // 전방 선언

class Item
{
public:
    // kind: GROWTH_ITEM / POISON_ITEM / SPEED_ITEM 중 하나
    Item(int kind);

    // Growth · Poison 용 - 매 tick 호출하여 출현/수명/재출현을 관리
    void update(Map &map);

    // Speed 전용 - move() 직전에 호출하여 "맵에 있었는지"를 스냅샷
    void prepareSpeed(const Map &map);

    // Speed 전용 - move() 직후에 호출.
    // 반환값: 이번 tick 에 Growth/Poison 아이템도 갱신해야 하면 true
    bool updateSpeed(Map &map);

    // Speed 효과가 지속 중인지 여부
    bool isSpeedActive() const;

    // 아이템 상태 초기화
    void reset();

private:
    const int kind; // 이 아이템의 종류 (GROWTH/POISON/SPEED 셀 값) - 생성 후 불변
    int y;          // 맵에서의 y 좌표
    int x;          // 맵에서의 x 좌표
    bool active;    // 현재 맵 위에 존재하는가
    int timer;      // 맵 위 수명 (남은 tick)
    int effectTimer; // Speed 효과 지속 tick - Speed 전용
    int tickCounter; // 아이템 갱신 주기 카운터 - Speed 전용
    bool wasOnMap;   // move() 직전 맵 위 존재 여부 스냅샷 - Speed 전용

    // 빈 칸 하나를 무작위로 골라 아이템을 새로 배치
    void spawn(Map &map);
};

#endif
