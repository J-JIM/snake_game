#include "Item.h"
#include <cstdlib>
#include <vector>

void spawnItem(Map& map, Item& item, int cellType) {
    std::vector<Position> empties;
    // 맵 전체를 돌며 빈 칸 수집
    for (int y = 0; y < map.getHeight(); y++) {
        for (int x = 0; x < map.getWidth(); x++) {
            if (map.getCell(y, x) == EMPTY) {
                empties.push_back({y, x});
            }
        }
    }
    if (empties.empty()) return;

    // 맵에서 EMPTY인 칸 중 하나를 무작위로 선택(% size로 범위 외 값 불러오기 방지)
    int idx = rand() % empties.size();
    item.y = empties[idx].y; // 아이템 스폰 y좌표
    item.x = empties[idx].x; // 아이템 스폰 x좌표
    item.active = true; 
    item.timer = 30; // 아이템 유통 기한
    map.setCell(item.y, item.x, cellType);
}

void updateItem(Map& map, Item& item, int cellType) {
    if (!item.active) { // 아이템이 active 하지 않은 경우(초기 상태)
        spawnItem(map, item, cellType);
        return;
    }

    // item이 있어야 할 셀에 cellType이 아이템이 아닌 무언가다!
    // = 뱀이 먹었을 때를 정의함.
    if (map.getCell(item.y, item.x) != cellType) {
        spawnItem(map, item, cellType);
        return;
    }

    item.timer--;
    if (item.timer <= 0) { // 아이템 유통 기한 만료 시
        map.setCell(item.y, item.x, EMPTY); // 아이템 제거
        spawnItem(map, item, cellType); // 새로운 곳에 아이템 재생성
    }
}
