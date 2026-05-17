// ScoreBoard.cpp
// 5단계 - 스코어보드 구현부

#include "ScoreBoard.h"
#include "curses_compat.h"
#include <cstdlib>
#include <locale.h>

ScoreBoard::ScoreBoard(int stageNum, int totalInternalWalls)
{
    stage = stageNum;

    currentLength = 0;
    maxLength = 0;
    growthCount = 0;
    poisonCount = 0;
    speedCount = 0;
    gateCount = 0;

    // 5단계 - 현재 내부 벽 개수 초기화 추가
    currentInternalWalls = totalInternalWalls;

    missionLength = false;
    missionGrowth = false;
    missionPoison = false;
    missionSpeed = false;
    missionGate = false;

    // 5단계 - 스테이지 초기화 시 미션 자동 생성
    generateMissions(totalInternalWalls);
}

void ScoreBoard::generateMissions(int totalInternalWalls)
{
    // 5단계 - 스테이지가 높아질수록 목표치가 커지도록 설정
    int base = stage * 2;

    targetLength = base + (rand() % 3) + 3;        // 5단계 - 스테이지 1: 5~7, 스테이지 4: 11~13
    targetGrowth = base + (rand() % 3);            // 5단계 - 스테이지 1: 2~4, 스테이지 4: 8~10
    targetPoison = (stage / 2) + (rand() % 2) + 1; // 5단계 - 독 아이템 목표
    targetSpeed = base / 2 + (rand() % 2) + 1;     // 5단계 - 스피드 아이템 목표

    // 5단계 - Gate 미션: 통과한 게이트의 개수를 미션으로 사용하되 전체 벽의 30% 이하로 난이도 조절
    int maxGate = totalInternalWalls * 0.3;
    if (maxGate < 1)
        maxGate = 1; // 최소 1번은 통과해야 함

    int passes = (rand() % maxGate) + 1;
    // 너무 많아지지 않도록 스테이지 보정 (스테이지당 1~2회 추가 목표)
    if (passes > stage + 1)
        passes = stage + 1;

    targetGate = passes;
}

void ScoreBoard::addGrowth()
{
    growthCount++;
    checkMissions();
}

void ScoreBoard::addPoison()
{
    poisonCount++;
    checkMissions();
}

void ScoreBoard::addSpeed()
{
    speedCount++;
    checkMissions();
}

// 5단계 - 게이트 통과 횟수 증가
void ScoreBoard::addGate()
{
    gateCount++;
    checkMissions();
}

// 5단계 - 게이트 미션이 달성 가능한지 판단하는 로직
bool ScoreBoard::canCompleteGateMission(bool isGateActive) const
{
    if (missionGate)
        return true; // 이미 달성했으면 OK

    // 5단계 - 앞으로 생성 가능한 게이트 쌍의 수 = (남은 벽 / 2) + (현재 떠있는 게이트 ? 1 : 0)
    int potentialGates = (currentInternalWalls / 2) + (isGateActive ? 1 : 0);
    int needed = targetGate - gateCount;

    return potentialGates >= needed;
}

void ScoreBoard::updateLength(int len)
{
    currentLength = len;
    if (currentLength > maxLength)
        maxLength = currentLength;
    checkMissions();
}

void ScoreBoard::checkMissions()
{
    // 5단계 - 현재 점수가 목표치를 달성했는지 실시간 확인
    if (currentLength >= targetLength)
        missionLength = true;
    if (growthCount >= targetGrowth)
        missionGrowth = true;
    if (poisonCount >= targetPoison)
        missionPoison = true;
    if (speedCount >= targetSpeed)
        missionSpeed = true;
    if (gateCount >= targetGate)
        missionGate = true;
}

bool ScoreBoard::isAllMissionComplete() const
{
    return missionLength && missionGrowth && missionPoison && missionSpeed && missionGate;
}

void ScoreBoard::draw(int offsetY, int offsetX) const
{
    // 5단계 - Score Board
    attron(A_BOLD);
    mvprintw(offsetY, offsetX, "Score Board");
    attroff(A_BOLD);
    mvprintw(offsetY + 1, offsetX, "B: %d / %d", currentLength, maxLength);
    mvprintw(offsetY + 2, offsetX, "+: %d", growthCount);
    mvprintw(offsetY + 3, offsetX, "-: %d", poisonCount);
    mvprintw(offsetY + 4, offsetX, "S: %d", speedCount);
    mvprintw(offsetY + 5, offsetX, "G: %d (Max:%d)", gateCount, currentInternalWalls / 2);

    // 5단계 - Mission
    int mOffset = offsetY + 7;
    attron(A_BOLD);
    mvprintw(mOffset, offsetX, "Mission");
    attroff(A_BOLD);
    mvprintw(mOffset + 1, offsetX, "B: %d (%c)", targetLength, missionLength ? 'v' : ' ');
    mvprintw(mOffset + 2, offsetX, "+: %d (%c)", targetGrowth, missionGrowth ? 'v' : ' ');
    mvprintw(mOffset + 3, offsetX, "-: %d (%c)", targetPoison, missionPoison ? 'v' : ' ');
    mvprintw(mOffset + 4, offsetX, "S: %d (%c)", targetSpeed, missionSpeed ? 'v' : ' ');
    mvprintw(mOffset + 5, offsetX, "G: %d (%c)", targetGate, missionGate ? 'v' : ' ');

    // 5단계 - Help (English version for stability)
    int hOffset = mOffset + 7;
    attron(A_UNDERLINE);
    mvprintw(hOffset, offsetX, "Help");
    attroff(A_UNDERLINE);
    mvprintw(hOffset + 1, offsetX, "B: 몸의 길이(현재/최대)");
    mvprintw(hOffset + 2, offsetX, "+/-: 몸의 길이 증가/감소");
    mvprintw(hOffset + 3, offsetX, "S: 속도 증가");
    mvprintw(hOffset + 4, offsetX, "G: 게이트 통과 횟수");
    mvprintw(hOffset + 5, offsetX, "Max: 남은 게이트 생성 횟수");
    mvprintw(hOffset + 6, offsetX, "!: 벽충돌 또는 몸 길이 3 미만시 게임오버");
}