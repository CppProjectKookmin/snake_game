/*
 * File: ScoreManager.cpp
 * Description: ScoreManager 클래스의 구현부.
 * 실시간 점수(길이, 아이템, 게이트) 카운트 적산 및
 * 각 스테이지별 동적 미션(V/X) 매칭 검증 스코어보드 출력 수행.
 * Author: 김현수
 */

#include "ScoreManager.h"
#include "Board.h"

extern "C" {
#include "curses.h"
}

ScoreManager::ScoreManager() {
    currentStage = 1;
    stageMissions[0] = { 5, 2, 1, 1 };
    stageMissions[1] = { 6, 3, 1, 2 };
    stageMissions[2] = { 7, 4, 2, 2 };
    stageMissions[3] = { 8, 5, 2, 3 };
    stageMissions[4] = { 9, 6, 3, 4 };

    for (int i = 0; i < 5; i++) {
        history[i] = { i + 1, 0, 0, 0, 0, 0, 0 };
    }
}

void ScoreManager::initStage(int stage, int startLength) {
    currentStage = stage;
    currentLength = startLength;
    maxLength = startLength;
    growthCount = 0;
    poisonCount = 0;
    gateCount = 0;
    currentCombo = 0;
    maxCombo = 0;
    stageStartTime = std::time(nullptr);
    elapsedSeconds = 0;
}

void ScoreManager::updateLength(int curLen, int maxLen) {
    currentLength = curLen;
    maxLength = maxLen;
}

void ScoreManager::updateTime() {
    elapsedSeconds = static_cast<int>(std::time(nullptr) - stageStartTime);
}

void ScoreManager::addGrowth() {
    growthCount++;
    currentCombo++;
    if (currentCombo > maxCombo) {
        maxCombo = currentCombo;
    }
}

void ScoreManager::addPoison() {
    poisonCount++;
    currentCombo = 0;
}

void ScoreManager::addGate() {
    gateCount++;
}

int ScoreManager::getCurrentStage() const { return currentStage; }

bool ScoreManager::checkMissionComplete() const {
    const Mission& m = stageMissions[currentStage - 1];
    return (currentLength >= m.targetLength) &&
        (growthCount >= m.targetGrowth) &&
        (poisonCount >= m.targetPoison) &&
        (gateCount >= m.targetGate);
}

void ScoreManager::renderScoreBoard() const {
    const Mission& m = stageMissions[currentStage - 1];
    int startX = MAP_SIZE + 3;

    mvprintw(1, startX, "======================");
    mvprintw(2, startX, "     SCORE BOARD      ");
    mvprintw(3, startX, "======================");
    mvprintw(4, startX, "Stage        : %d / 5", currentStage);
    mvprintw(5, startX, "B (Length)   : %d / %d", currentLength, maxLength);
    mvprintw(6, startX, "+ (Growth)   : %d", growthCount);
    mvprintw(7, startX, "- (Poison)   : %d", poisonCount);
    mvprintw(8, startX, "G (Gate)     : %d", gateCount);
    mvprintw(9, startX, "Time         : %d sec", elapsedSeconds);
    mvprintw(10, startX, "Max G-Combo  : %d", maxCombo);

    mvprintw(12, startX, "======================");
    mvprintw(13, startX, "    MISSION BOARD     ");
    mvprintw(14, startX, "======================");
    mvprintw(15, startX, "B: %d (%d)  [%c]", currentLength, m.targetLength, (currentLength >= m.targetLength) ? 'V' : 'X');
    mvprintw(16, startX, "+: %d (%d)  [%c]", growthCount, m.targetGrowth, (growthCount >= m.targetGrowth) ? 'V' : 'X');
    mvprintw(17, startX, "-: %d (%d)  [%c]", poisonCount, m.targetPoison, (poisonCount >= m.targetPoison) ? 'V' : 'X');
    mvprintw(18, startX, "G: %d (%d)  [%c]", gateCount, m.targetGate, (gateCount >= m.targetGate) ? 'V' : 'X');
    mvprintw(19, startX, "======================");
}

void ScoreManager::saveStageRecord() {
    if (currentStage >= 1 && currentStage <= 5) {
        StageRecord& r = history[currentStage - 1];
        r.finalLength = currentLength;
        r.growthEat = growthCount;
        r.poisonEat = poisonCount;
        r.gateUse = gateCount;
        r.playTime = elapsedSeconds;
        r.maxCombo = maxCombo;
    }
}

void ScoreManager::renderFinalSummary() const {
    clear();
    mvprintw(1, 2, "=========================================================");
    mvprintw(2, 2, "               FINAL PROJECT GAME SUMMARY                ");
    mvprintw(3, 2, "=========================================================");
    mvprintw(5, 2, "STG  |  LEN(MAX)  |  GROWTH  |  POISON  |  GATE  |  TIME  | COMBO");
    mvprintw(6, 2, "---------------------------------------------------------");

    for (int i = 0; i < 5; i++) {
        const StageRecord& r = history[i];
        mvprintw(7 + i, 2, "STG%d  |   %2d       |    %2d    |    %2d    |   %2d   |  %3ds  |  %2d",
            r.stageNum, r.finalLength, r.growthEat, r.poisonEat, r.gateUse, r.playTime, r.maxCombo);
    }

    mvprintw(13, 2, "=========================================================");
    mvprintw(15, 2, "Press any key to exit the system...");
    refresh();
}




 // [Stage 5 추가] 스테이지 클리어 안내 연출 화면 구현
void ScoreManager::renderStageClearScreen() const {
    clear();
    mvprintw(8, 15, "=========================================");
    mvprintw(9, 15, "           STAGE %d CLEAR!!!             ", currentStage);
    mvprintw(10, 15, "=========================================");
    mvprintw(12, 15, "      All missions have been met.        ");
    mvprintw(14, 15, "    >> Press any key to continue <<     ");
    refresh();
}