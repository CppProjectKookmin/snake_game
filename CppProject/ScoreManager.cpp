/*
 * File: ScoreManager.cpp
 * Description: ScoreManager 클래스의 구현부.
 * 시간 추적 알고리즘, 연속 콤보 계산 메커니즘,
 * 미션 달성 실시간 시각화(V/X) 및 통계 대시보드 렌더링 수행.
 * Author: 김현수
 */

#include "ScoreManager.h"
#include "Board.h"

extern "C" {
#include "curses.h"
}

ScoreManager::ScoreManager() {
    currentStage = 1;

    // [프로젝트 명세 준수] 각 스테이지별 동적 난이도 조절을 위한 미션 기준 데이터 테이블 바인딩
    stageMissions[0] = { 5, 2, 1, 1 }; // Stage 1
    stageMissions[1] = { 6, 3, 1, 2 }; // Stage 2
    stageMissions[2] = { 7, 4, 2, 2 }; // Stage 3
    stageMissions[3] = { 8, 5, 2, 3 }; // Stage 4
    stageMissions[4] = { 9, 6, 3, 4 }; // Stage 5

    // 기록 컨테이너 초기화
    for (int i = 0; i < 5; i++) {
        history[i] = { i + 1, 0, 0, 0, 0, 0, 0 };
    }
}

void ScoreManager::initStage(int stage, int startLength) {
    // 신규 스테이지 진입에 따른 스코어 및 상태 파라미터 완전 초기화
    currentStage = stage;
    currentLength = startLength;
    maxLength = startLength;
    growthCount = 0;
    poisonCount = 0;
    gateCount = 0;
    currentCombo = 0;
    maxCombo = 0;
    stageStartTime = std::time(nullptr); // 기준 시점 타임스탬프 갱신
    elapsedSeconds = 0;
}

void ScoreManager::updateLength(int curLen, int maxLen) {
    currentLength = curLen;
    maxLength = maxLen;
}

void ScoreManager::updateTime() {
    // 실시간 클럭 조회를 통한 단리 초단위 경과 시간 산출
    elapsedSeconds = static_cast<int>(std::time(nullptr) - stageStartTime);
}

void ScoreManager::addGrowth() {
    growthCount++;
    currentCombo++; // Growth 연속 획득 성공에 따른 콤보 스택 누적
    if (currentCombo > maxCombo) {
        maxCombo = currentCombo; // 임계값 갱신 알고리즘 적용
    }
}

void ScoreManager::addPoison() {
    poisonCount++;
    currentCombo = 0; // Poison 아이템 상호작용 발생 시 콤보 체인 즉시 리셋
}

void ScoreManager::addGate() {
    gateCount++;
}

int ScoreManager::getCurrentStage() const { return currentStage; }

bool ScoreManager::checkMissionComplete() const {
    // 현재 누적 스코어와 목표 미션 행렬 간의 AND 연산 검증
    const Mission& m = stageMissions[currentStage - 1];
    return (currentLength >= m.targetLength) &&
        (growthCount >= m.targetGrowth) &&
        (poisonCount >= m.targetPoison) &&
        (gateCount >= m.targetGate);
}

void ScoreManager::renderScoreBoard() const {
    const Mission& m = stageMissions[currentStage - 1];
    int startX = MAP_SIZE + 3; // 메인 게임 보드판 오버레이 방지를 위한 우측 좌표 마진 확보

    // 1. 실시간 점수 판넬 출력 (Score Board)
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

    // 2. 조건부 미션 달성 여부 시각화 판넬 출력 (Mission Board)
    mvprintw(12, startX, "======================");
    mvprintw(13, startX, "    MISSION BOARD     ");
    mvprintw(14, startX, "======================");

    // 조건 삼항 연산자를 활용한 동적 텍스트 매칭 기법 적용 (V: 달성 / X: 미달성)
    mvprintw(15, startX, "B: %d (%d)  [%c]", currentLength, m.targetLength, (currentLength >= m.targetLength) ? 'V' : 'X');
    mvprintw(16, startX, "+: %d (%d)  [%c]", growthCount, m.targetGrowth, (growthCount >= m.targetGrowth) ? 'V' : 'X');
    mvprintw(17, startX, "-: %d (%d)  [%c]", poisonCount, m.targetPoison, (poisonCount >= m.targetPoison) ? 'V' : 'X');
    mvprintw(18, startX, "G: %d (%d)  [%c]", gateCount, m.targetGate, (gateCount >= m.targetGate) ? 'V' : 'X');
    mvprintw(19, startX, "======================");
}

void ScoreManager::saveStageRecord() {
    // 아카이빙 제약 조건 검증 후 현재 객체 스냅샷을 히스토리 배열에 바인딩
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

void ScoreManager::renderStageClearScreen() const {
    // [UX 고도화] 급작스러운 화면 전환으로 인한 인지 부하 차단용 미들웨어 스크린 렌더링
    clear();
    mvprintw(8, 15, "=========================================");
    mvprintw(9, 15, "           STAGE %d CLEAR!!!             ", currentStage);
    mvprintw(10, 15, "=========================================");
    mvprintw(12, 15, "      All missions have been met.        ");
    mvprintw(14, 15, "    >> Press any key to continue <<     ");
    refresh();
}

void ScoreManager::renderFinalSummary() const {
    // [독창성 가이드 준수] 게임 완전히 종료 시 데이터 배열을 역추적하여 테이블 포맷 전적 스크린 마킹
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