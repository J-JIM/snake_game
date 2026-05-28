// ScoreBoard.h
// 5단계 - 스코어 보드와 미션 상태를 관리하고 화면에 출력하는 클래스

#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include "common.h"

class ScoreBoard
{
public:
    // 5단계 - stageNum: 현재 스테이지 번호
    // 5단계 - totalInternalWalls: Used Gate Wall 미션 상한선 계산용
    ScoreBoard(int stageNum, int totalInternalWalls);

    // 5단계 - 각종 점수 업데이트 메서드
    void addGrowth();
    void addPoison();
    void addSpeed();
    // 5단계 - 게이트 통과 횟 증가
    void addGate();
    void updateLength(int len);
    // 5단계 - 현재 맵의 벽 개수 업데이트
    void setInternalWalls(int count) { currentInternalWalls = count; }

    // 5단계 - 게이트 미션 달성이 물리적으로 가능한지 확인
    bool canCompleteGateMission(bool isGateActive) const;
    bool isMissionGateComplete() const { return missionGate; }

    // 5단계 - 모든 미션 달성 여부 확인
    bool isAllMissionComplete() const;

    // 점수 및 기록 조회 게터
    int getMaxLength() const { return maxLength; }
    int getGrowthCount() const { return growthCount; }
    int getPoisonCount() const { return poisonCount; }
    int getSpeedCount() const { return speedCount; }
    int getGateCount() const { return gateCount; }

    // 5단계 - 화면에 그리기
    void draw(int offsetY, int offsetX) const;

private:
    int stage;

    // 5단계 - 점수 영역
    int currentLength;
    int maxLength;
    int growthCount;
    int poisonCount;
    int speedCount;
    int gateCount;            // 5단계 - 게이트 통과 횟수
    int currentInternalWalls; // 5단계 - 현재 맵에 남은 WALL(1) 개수

    // 5단계 - 미션 영역 (목표치)
    int targetLength;
    int targetGrowth;
    int targetPoison;
    int targetSpeed;
    int targetGate;

    // 5단계 - 미션 달성 상태
    bool missionLength;
    bool missionGrowth;
    bool missionPoison;
    bool missionSpeed;
    bool missionGate;

    // 5단계 - 미션 달성 여부 실시간 체크
    void checkMissions();

    // 5단계 - 스테이지별 난이도에 따른 무작위 미션 생성
    void generateMissions(int totalInternalWalls);
};

#endif
