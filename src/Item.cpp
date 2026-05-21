// Item.cpp
// Item 클래스 구현 - 아이템의 출현/수명/획득 처리
//   Growth 와 Poison 은 동작이 같아 update() 를 공용으로 쓰고,
//   Speed 는 속도 효과 때문에 prepareSpeed()/updateSpeed() 를 따로 둔다.

#include "Item.h"
#include "Map.h"
#include <cstdlib>
#include <vector>

// 아이템이 맵 위에 머무는 기본 수명 (tick)
static const int ITEM_LIFETIME = 30;
// Speed 효과 지속 시간 (tick)
static const int SPEED_EFFECT_DURATION = 30;

// 생성자 - 종류만 정하고, 실제 배치는 첫 update()/updateSpeed() 에서 spawn() 이 담당
Item::Item(int kind)
    : kind(kind), y(0), x(0), active(false),
      timer(0), effectTimer(0), tickCounter(0), wasOnMap(false)
{
}

// 빈 칸 중 하나를 무작위로 골라 아이템을 배치
void Item::spawn(Map &map)
{
    std::vector<Position> empties;
    // 맵 전체를 돌며 빈 칸 수집
    for (int cy = 0; cy < map.getHeight(); cy++)
    {
        for (int cx = 0; cx < map.getWidth(); cx++)
        {
            if (map.getCell(cy, cx) == EMPTY)
            {
                Position p = {cy, cx};
                empties.push_back(p);
            }
        }
    }
    if (empties.empty())
        return;

    // EMPTY 칸 중 하나를 무작위 선택 (% size 로 범위 밖 접근 방지)
    int idx = rand() % (int)empties.size();
    y = empties[idx].y;
    x = empties[idx].x;
    active = true;
    timer = ITEM_LIFETIME;
    map.setCell(y, x, kind);
}

// Growth · Poison 갱신
void Item::update(Map &map)
{
    // 아직 한 번도 출현하지 않은 초기 상태
    if (active == false)
    {
        spawn(map);
        return;
    }

    // 아이템이 있어야 할 칸이 더 이상 해당 종류가 아니다 = 뱀이 먹었다
    if (map.getCell(y, x) != kind)
    {
        spawn(map);
        return;
    }

    // 수명 차감, 만료되면 제거 후 다른 자리에 재출현
    timer--;
    if (timer <= 0)
    {
        map.setCell(y, x, EMPTY);
        spawn(map);
    }
}

// move() 직전 - 이번 tick 시작 시점에 Speed 아이템이 맵에 있었는지 기록
void Item::prepareSpeed(const Map &map)
{
    wasOnMap = active && (map.getCell(y, x) == kind);
}

// move() 직후 - 먹힘 감지 후 효과 타이머 세팅, 갱신 주기 관리
// 반환값: 이번 tick 에 Growth/Poison 도 갱신해야 하면 true
bool Item::updateSpeed(Map &map)
{
    tickCounter++;
    // 효과가 없을 때는 매 tick, 효과 중에는 2 tick 마다 1번만 갱신
    bool shouldUpdate = (effectTimer <= 0) || (tickCounter % 2 == 0);

    if (shouldUpdate)
    {
        // update() 로 좌표가 바뀌기 전에 먹힘 여부 먼저 확인
        if (wasOnMap && map.getCell(y, x) != kind)
        {
            effectTimer = SPEED_EFFECT_DURATION;
        }
        update(map);
    }

    if (effectTimer > 0)
    {
        effectTimer--;
    }

    return shouldUpdate;
}

// Speed 효과 지속 중인지 여부
bool Item::isSpeedActive() const
{
    return effectTimer > 0;
}
