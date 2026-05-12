// Snake.h
// 뱀의 위치/방향/이동을 관리하는 클래스

#ifndef SNAKE_H
#define SNAKE_H

#include "common.h"

class Map;   // 전방 선언
class Gate;  // 전방 선언 (4단계)

class Snake {
public:
    Snake();

    // 맵을 훑어서 SNAKE_HEAD/SNAKE_BODY 가 있는 위치를 찾아 뱀 정보 등록
    bool initFromMap(const Map& map);

    // 사용자가 누른 방향키를 다음 진행 방향으로 예약
    void requestDirection(Direction d);

    // 한 tick 만큼 이동.
    // gate 가 nullptr 이면 GATE 셀은 그냥 일반 셀처럼 다룸 (1~3단계 호환)
    // 반환값: true = 정상 진행, false = 게임 오버 (벽/몸통 충돌, 반대방향, Gate 막힘)
    bool move(Map& map, Gate* gate = nullptr);

    int getLength() const { return length; }
    Direction getDirection() const { return dir; }

private:
    Position body[SNAKE_MAX_LENGTH];  // body[0] = 머리, body[length-1] = 꼬리
    int length;
    Direction dir;       // 현재 진행 방향
    Direction nextDir;   // 다음 tick 에 적용할 방향 (사용자가 키 누른 결과)

    // a 와 b 가 서로 정반대 방향이면 true
    bool isOpposite(Direction a, Direction b) const;
};

#endif
