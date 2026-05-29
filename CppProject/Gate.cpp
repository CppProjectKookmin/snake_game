/*
 * File: Gate.cpp
 * Description: Gate 클래스의 구현부. 게이트 위치 난수 생성 및 진출 방향 판단 로직 수행
 * Author: [조성래]
 */

#include "Gate.h"
#include "Board.h"
#include <vector>
#include <cstdlib> // rand() 사용

Gate::Gate() {
    gateA = { -1, -1 };
    gateB = { -1, -1 };
    bIsActive = false;
}

void Gate::generateGates(const Board& board) {
    std::vector<Pos> wallList;

    // 1. 맵 전체를 순회하며 일반 벽(1)의 위치만 후보 리스트에 추가 (모서리 2는 자연스럽게 제외됨)
    for (int y = 0; y < MAP_SIZE; y++) {
        for (int x = 0; x < MAP_SIZE; x++) {
            if (board.getData(y, x) == 1) {
                wallList.push_back({ y, x });
            }
        }
    }

    // 2. 겹치지 않게 무작위로 2개의 벽 선택
    if (wallList.size() >= 2) {
        int idx1 = std::rand() % wallList.size();
        int idx2 = std::rand() % wallList.size();

        while (idx1 == idx2) {
            idx2 = std::rand() % wallList.size();
        }

        gateA = wallList[idx1];
        gateB = wallList[idx2];
        bIsActive = true;
    }
}

bool Gate::isGate(const Pos p) const {
    if (!bIsActive) return false;
    return (p.y == gateA.y && p.x == gateA.x) || (p.y == gateB.y && p.x == gateB.x);
}

Pos Gate::getOtherGate(const Pos p) const {
    if (p.y == gateA.y && p.x == gateA.x) return gateB;
    return gateA; // A가 아니면 B를 반환
}

Direction Gate::getExitDirection(const Board& board, const Pos exitGate, const Direction entryDir) const {
    // 로직 1: 진출 게이트가 가장자리 벽에 있을 경우, 항상 맵의 안쪽 방향으로 진출
    if (exitGate.y == 0) return DOWN;
    if (exitGate.y == MAP_SIZE - 1) return UP;
    if (exitGate.x == 0) return RIGHT;
    if (exitGate.x == MAP_SIZE - 1) return LEFT;

    // 로직 2: 진출 게이트가 맵 가운데 벽에 있을 경우
    // 순서: 진입 방향 -> 시계 방향 -> 반시계 방향 -> 반대 방향
    const Direction priority[4] = {
        entryDir,
        static_cast<Direction>((entryDir + 1) % 4), // 시계 방향 (+1)
        static_cast<Direction>((entryDir + 3) % 4), // 반시계 방향 (+3)
        static_cast<Direction>((entryDir + 2) % 4)  // 반대 방향 (+2)
    };

    // Snake.cpp의 dy, dx와 동일한 규칙
    const int dy[4] = { -1, 0, 1, 0 };
    const int dx[4] = { 0, 1, 0, -1 };

    for (int i = 0; i < 4; i++) {
        Direction checkDir = priority[i];
        int ny = exitGate.y + dy[checkDir];
        int nx = exitGate.x + dx[checkDir];

        // 범위 내에 있고, 빈 공간(0)이라면 그 방향으로 진출
        if (ny >= 0 && ny < MAP_SIZE && nx >= 0 && nx < MAP_SIZE) {
            if (board.getData(ny, nx) == 0) {
                return checkDir;
            }
        }
    }
    return entryDir; // 예외 상황 대비용
}

Pos Gate::getGateA() const { return gateA; }
Pos Gate::getGateB() const { return gateB; }
bool Gate::isActive() const { return bIsActive; }