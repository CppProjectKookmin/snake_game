/*
 * File: ScoreManager.h
 * Description: ScoreManager 클래스의 선언부.
 * 실시간 점수 계산, 스테이지별 미션 달성 여부 검증 및
 * 사이드바 렌더링을 위한 데이터 구조 정의.
 * Author: 김현수
 */

#ifndef SCOREMANAGER_H
#define SCOREMANAGER_H

#include <ctime>

 // 각 스테이지별 클리어 목표 수치를 저장하는 미션 구조체
struct Mission {
    int targetLength; // 목표 뱀 길이
    int targetGrowth; // 목표 Growth 아이템 획득 횟수
    int targetPoison; // 목표 Poison 아이템 획득 횟수
    int targetGate;   // 목표 게이트 통과 횟수
};

// 게임 종료 후 최종 정산 스크린 출력을 위한 스테이지별 레코드 구조체
struct StageRecord {
    int stageNum;    // 스테이지 번호
    int finalLength; // 최종 도달 길이
    int growthEat;   // 총 획득한 Growth 개수
    int poisonEat;   // 총 획득한 Poison 개수
    int gateUse;     // 총 게이트 이용 횟수
    int playTime;    // 해당 스테이지 플레이 시간 (초)
    int maxCombo;    // 최대 연속 Growth 획득 콤보 수
};

class ScoreManager {
private:
    int currentLength; // 현재 뱀의 길이
    int maxLength;     // 해당 스테이지에서 도달한 최대 뱀 길이
    int growthCount;   // 현재 스테이지 Growth 아이템 획득 누적치
    int poisonCount;   // 현재 스테이지 Poison 아이템 획득 누적치
    int gateCount;     // 현재 스테이지 게이트 통과 누적치
    int currentStage;  // 현재 진행 중인 스테이지 번호

    int currentCombo;  // 현재 연속 Growth 아이템 획득 횟수
    int maxCombo;      // 현재 스테이지 최고 연속 획득 콤보 수

    std::time_t stageStartTime; // 스테이지 진입 시점의 타임스탬프
    int elapsedSeconds;         // 실시간 경과 시간 (초단위)

    Mission stageMissions[5];   // 1~5 스테이지 고정 미션 배열
    StageRecord history[5];     // 1~5 스테이지 기록 저장 배열

public:
    ScoreManager();
    void initStage(int stage, int startLength);

    void updateLength(int curLen, int maxLen);
    void updateTime();
    void addGrowth();
    void addPoison();
    void addGate();

    bool checkMissionComplete() const;
    void renderScoreBoard() const;
    void saveStageRecord();
    void renderFinalSummary() const;
    void renderStageClearScreen() const;
    int getCurrentStage() const;
};

#endif