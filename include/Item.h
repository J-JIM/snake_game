#ifndef ITEM_H
#define ITEM_H

#include "common.h"
#include "Map.h"

struct Item {
    int y; // 맵에서의 y 좌표
    int x; // 맵에서의 x 좌표
    bool active; // 현재 맵에서 존재하는가?
    int timer; // 남은 틱 수
};

void spawnItem(Map& map, Item& item, int cellType);
void updateItem(Map& map, Item& item, int cellType);
// 매개변수로 cellType를 받음
// 이유: growth와 poison이 구조상 동작 방식이 같아서

#endif
