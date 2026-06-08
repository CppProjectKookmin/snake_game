/*
 * File: ScoreManager.h
 * Description: ScoreManager 클래스의 선언부. 점수 계산, 미션 달성 여부 검증
 * 및 우측 사이드바 렌더링 파이프라인 제어.
 * Author: 김현수
 */

#ifndef SCOREMANAGER_H
#define SCOREMANAGER_H

#include <ctime>

struct Mission {
    int targetLength;
    int targetGrowth;
    int targetPoison;
    int targetGate;
};

struct StageRecord {
    int stageNum;
    int finalLength;
    int growthEat;
    int poisonEat;
    int gateUse;
    int playTime;
    int maxCombo;
};

class ScoreManager {
private:
    int currentLength;
    int maxLength;
    int growthCount;
    int poisonCount;
    int gateCount;
    int currentStage;

    int currentCombo;
    int maxCombo;

    std::time_t stageStartTime;
    int elapsedSeconds;

    Mission stageMissions[5];
    StageRecord history[5];

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
    void renderStageClearScreen() const; // [Stage 5 추가] 스테이지 클리어 연출 함수
    int getCurrentStage() const;
};

#endif