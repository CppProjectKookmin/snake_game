/*
 * File: main.cpp
 * Description: 시스템 진입점 및 메인 컨트롤 루프.
 * Author: 박서린 (Stage 3: 박하은 / Stage 4: 조성래 / Stage 5 최종 완성: 김현수)
 */

#include <iostream>
#include "Board.h"
#include "Snake.h"
#include "ItemManager.h" // [Stage 3] 아이템 제어 컴포넌트 추가
#include "Gate.h"
#include "ScoreManager.h" // [Stage 5] 점수 및 미션 관리 컴포넌트 추가

extern "C" {
#include "curses.h"
}

#pragma comment(lib, "pdcurses.lib")

int main() {
    // =====================================================================
    // 1. 시스템 초기화 파트
    // =====================================================================
    initscr();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    timeout(500); // 게임 프레임워크 갱신 주기 설정 (Tick: 500ms)

    int currentStage = 1;
    bool gameRunning = true;

    ScoreManager scoreMgr; // [Stage 5] 전적 보존을 위해 루프 외부 선언

    // 전체 5개 스테이지 진행을 위한 메인 루프
    while (currentStage <= 5 && gameRunning) {
        // =====================================================================
        // 2. 주요 도메인 객체 생성 및 초기화
        // =====================================================================
        Board board;
        board.loadStage(currentStage); // 동적 스테이지 맵 로드

        Snake snake(currentStage); // [수정] 스테이지별 충돌 방지 가변 스폰 적용
        ItemManager itemMgr; // 독립적 아이템 상태 관리 객체 생성
        Gate gate; // Gate 시스템 초기화

        scoreMgr.initStage(currentStage, snake.getLength()); // 스코어 매니저 초기화
        gate.generateGates(board); // 맵 데이터를 읽어 상시 게이트 쌍 생성

        bool stageRunning = true;

        // =====================================================================
        // 3. 게임 메인 프레임 루프
        // =====================================================================
        while (stageRunning) {
            itemMgr.updateTicks(); // [Stage 3] 아이템 수명 주기 업데이트
            while (itemMgr.getItemCount() < 3) {
                itemMgr.spawnItem(board, snake.getBody()); // 상시 목표 수량(3개) 스폰
            }

            int ch = getch();
            if (ch == 'q' || ch == 'Q') {
                stageRunning = false;
                gameRunning = false;
                break;
            }

            // [Stage 5 추가] QA 테스트용 스테이지 재시작 단축키 (R)
            if (ch == 'r' || ch == 'R') {
                stageRunning = false;
                break;
            }

            // [Stage 5 추가] 시연 및 채점용 스테이지 스킵 단축키 (N)
            if (ch == 'n' || ch == 'N') {
                scoreMgr.saveStageRecord();
                stageRunning = false;
                currentStage++;
                break;
            }

            // 사용자 키 입력에 따른 방향 전환 및 유효성 검사
            if (!snake.changeDirection(ch)) {
                stageRunning = false;
                gameRunning = false;
                break;
            }

            Pos nextP = snake.getNextHead(); // 차기 프레임 헤드 예정 좌표 산출
            bool isTeleporting = false; // 게이트 통과 플래그

            // [Stage 4] Gate 진출입 및 텔레포트 제어
            if (gate.isGate(nextP)) {
                Pos exitGate = gate.getOtherGate(nextP);
                Direction newDir = gate.getExitDirection(board, exitGate, snake.getDirection());

                snake.setDirection(newDir); // 출구 방향으로 강제 전환
                nextP = exitGate;           // 헤드 좌표 출구로 순간이동
                isTeleporting = true;
                scoreMgr.addGate();         // [Stage 5] 게이트 통과 카운트 가산
            }

            // 벽 및 몸통 충돌 검사 (게이트 출구 좌표의 벽 속성은 예외 처리)
            if ((!isTeleporting && board.checkWallCollision(nextP)) || snake.checkBodyCollision(nextP)) {
                stageRunning = false;
                gameRunning = false;
                break;
            }

            // [Stage 3/5 수정] 확장 아이템 상호작용 충돌 판정 및 효과 처리 분기
            bool ateItem = false;
            int collidedItemType = -1;
            ateItem = itemMgr.checkCollision(nextP.y, nextP.x, collidedItemType);

            if (ateItem) {
                if (collidedItemType == 0) { // 1) Growth Item 획득 시
                    snake.move(nextP, true);
                    scoreMgr.addGrowth();
                }
                else if (collidedItemType == 1) { // 2) Poison Item 획득 시
                    snake.move(nextP, false);
                    snake.shrink(); // 이동 후 꼬리 노드 추가 축소
                    scoreMgr.addPoison();

                    if (snake.getLength() < 3) { // 최소 길이 제약 조건 검사
                        stageRunning = false;
                        gameRunning = false;
                        break;
                    }
                }
                else if (collidedItemType == 2) { // 3) [아이템 다양화] Coin Item: 길이는 유지, 미션 카운트 +2
                    snake.move(nextP, false);
                    scoreMgr.addGrowth();
                    scoreMgr.addGrowth();
                }
                else if (collidedItemType == 3) { // 4) [아이템 다양화] Slow Item: 게임 일시 감속 (800ms)
                    snake.move(nextP, false);
                    timeout(800);
                }
            }
            else {
                snake.move(nextP, false); // 일반 이동 처리
                timeout(500); // 아이템 미획득 시 기본 속도(500ms) 복원
            }

            // 매 틱마다 실시간 스코어 및 타임 갱신
            scoreMgr.updateLength(snake.getLength(), snake.getMaxLength());
            scoreMgr.updateTime();

            // [Stage 5] 미션 달성 검증 및 스테이지 클리어 처리
            if (scoreMgr.checkMissionComplete()) {
                scoreMgr.saveStageRecord(); // 현 스테이지 전적 아카이빙

                timeout(-1); // 대기 모드 전환
                scoreMgr.renderStageClearScreen(); // 클리어 연출 화면 렌더링
                getch(); // 유저 입력 대기
                timeout(500); // 타이머 복원

                stageRunning = false;
                currentStage++;
                break;
            }

            // =====================================================================
            // 4. 화면 출력 및 그래픽 동기화 파트 (렌더링)
            // =====================================================================
            board.draw(snake.getBody(), snake.getLength(), snake.getMaxLength()); // 메인 보드판 출력
            itemMgr.renderItems(); // 상단 레이어에 아이템 오브젝트 출력

            if (gate.isActive()) { // 생성된 Gate 커스텀 기호 출력
                mvprintw(gate.getGateA().y, gate.getGateA().x, "A");
                mvprintw(gate.getGateB().y, gate.getGateB().x, "B");
            }

            scoreMgr.renderScoreBoard(); // 우측 대시보드 스코어판 출력
            refresh();
        }
    }

    // [Stage 5] 최종 전적 통계 요약 스크린 출력 파이프라인
    timeout(-1);
    scoreMgr.saveStageRecord();
    scoreMgr.renderFinalSummary();
    getch();

    endwin(); // ncurses 리소스 해제
    return 0;
}