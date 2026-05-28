#include "RankingManager.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ctime>

// 현재 시간을 YYYY-MM-DD HH:MM:SS 형태로 포맷팅해서 가져오는 헬퍼 함수
static std::string getCurrentTimestamp() {
    std::time_t now = std::time(nullptr);
    std::tm* localTm = std::localtime(&now);
    char buf[64];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localTm);
    return std::string(buf);
}

RankingManager::RankingManager() {}

void RankingManager::loadFromFile(const std::string& filepath) {
    records.clear();
    std::ifstream fin(filepath);
    if (!fin.is_open()) return;

    std::string line;
    while (std::getline(fin, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string ts, stageStr, maxLenStr, growthStr, poisonStr, speedStr, gateStr;
        
        if (std::getline(ss, ts, '|') &&
            std::getline(ss, stageStr, '|') &&
            std::getline(ss, maxLenStr, '|') &&
            std::getline(ss, growthStr, '|') &&
            std::getline(ss, poisonStr, '|') &&
            std::getline(ss, speedStr, '|') &&
            std::getline(ss, gateStr, '|')) {
            
            RankingRecord r;
            r.timestamp = ts;
            r.stage = std::stoi(stageStr);
            r.maxLength = std::stoi(maxLenStr);
            r.growthCount = std::stoi(growthStr);
            r.poisonCount = std::stoi(poisonStr);
            r.speedCount = std::stoi(speedStr);
            r.gateCount = std::stoi(gateStr);
            r.isCurrentPlay = false;
            r.rank = 0;
            records.push_back(r);
        }
    }
}

void RankingManager::saveToFile(const std::string& filepath) {
    std::ofstream fout(filepath);
    if (!fout.is_open()) return;

    for (const auto& r : records) {
        fout << r.timestamp << "|"
             << r.stage << "|"
             << r.maxLength << "|"
             << r.growthCount << "|"
             << r.poisonCount << "|"
             << r.speedCount << "|"
             << r.gateCount << "\n";
    }
}

void RankingManager::addRecord(int stage, int maxLength, int growth, int poison, int speed, int gate) {
    RankingRecord r;
    r.timestamp = getCurrentTimestamp();
    r.stage = stage;
    r.maxLength = maxLength;
    r.growthCount = growth;
    r.poisonCount = poison;
    r.speedCount = speed;
    r.gateCount = gate;
    r.isCurrentPlay = false;
    r.rank = 0;
    records.push_back(r);
}

std::vector<RankingRecord> RankingManager::getRankings(int stageFilter) const {
    // 1. 조건에 맞는 레코드 필터링
    std::vector<RankingRecord> filtered;
    for (const auto& r : records) {
        if (stageFilter == 0 || r.stage == stageFilter) {
            filtered.push_back(r);
        }
    }

    // 2. 최대 길이 내림차순 정렬 (동일한 최대 길이일때: 더 높은 단계 -> growth -> poison -> speed -> gate통과가 많은 순서로 정렬)
    std::sort(filtered.begin(), filtered.end(), [](const RankingRecord& a, const RankingRecord& b) {
        if (a.maxLength != b.maxLength) {
            return a.maxLength > b.maxLength;
        }
        if (a.stage != b.stage) {
            return a.stage > b.stage;
        }
        if (a.growthCount != b.growthCount) {
            return a.growthCount > b.growthCount;
        }
        if (a.poisonCount != b.poisonCount) {
            return a.poisonCount > b.poisonCount;
        }
        if (a.speedCount != b.speedCount) {
            return a.speedCount > b.speedCount;
        }
        if (a.gateCount != b.gateCount) {
            return a.gateCount > b.gateCount;
        }
        return a.timestamp > b.timestamp;
    });

    // 3. 필터링된 리스트의 모든 레코드에 1-based 순위 마킹
    for (int i = 0; i < (int)filtered.size(); i++) {
        filtered[i].rank = i + 1;
    }

    // 4. 결과 리스트 구성 (상위 10등 수집)
    std::vector<RankingRecord> result;
    int limit = std::min(10, (int)filtered.size());
    for (int i = 0; i < limit; i++) {
        result.push_back(filtered[i]);
    }

    // 5. 만약 방금 플레이한 기록이 존재하는데 10위 안에 없으면, 11번째 행으로 수동 추가!
    int currentPlayIndex = -1;
    for (int i = 0; i < (int)filtered.size(); i++) {
        if (filtered[i].isCurrentPlay) {
            currentPlayIndex = i;
            break;
        }
    }

    if (currentPlayIndex >= 10) {
        result.push_back(filtered[currentPlayIndex]);
    }

    return result;
}

void RankingManager::markLatestAsCurrent() {
    if (!records.empty()) {
        records.back().isCurrentPlay = true;
    }
}

void RankingManager::clearCurrentPlayFlag() {
    for (auto& r : records) {
        r.isCurrentPlay = false;
    }
}
