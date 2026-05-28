#ifndef RANKINGMANAGER_H
#define RANKINGMANAGER_H

#include <string>
#include <vector>

struct RankingRecord {
    std::string timestamp;
    int stage;
    int maxLength;
    int growthCount;
    int poisonCount;
    int speedCount;
    int gateCount;
    bool isCurrentPlay = false;
    int rank = 0; // 랭킹 정렬 후 계산된 순위 (1-indexed)
};

class RankingManager {
public:
    RankingManager();
    ~RankingManager() = default;

    // 파일에서 랭킹 로드
    void loadFromFile(const std::string& filepath = "scoreboard/rankings.txt");

    // 파일에 랭킹 저장
    void saveToFile(const std::string& filepath = "scoreboard/rankings.txt");

    // 새로운 랭킹 레코드 추가 (추가 시 현재 시간 구해서 timestamp 자동 생성)
    void addRecord(int stage, int maxLength, int growth, int poison, int speed, int gate);

    // 특정 스테이지(1~5, 0 = 통합)의 랭킹 리스트 반환
    // 최대 길이 내림차순 정렬 후 상위 10등 + (방금 플레이한 기록이 10등 밖일 시 추가) 반환
    std::vector<RankingRecord> getRankings(int stageFilter) const;

    // 방금 플레이한 기록 표시 설정
    void markLatestAsCurrent();

    // 방금 플레이한 기록 표시 해제
    void clearCurrentPlayFlag();

private:
    std::vector<RankingRecord> records;
};

#endif
