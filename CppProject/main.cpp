/*
 * File: main.cpp
 * Description: 시스템 진입점 및 메인 컨트롤 루프.
 * ncurses 라이브러리 환경 초기화, 실시간 게임 루프 제어 및
 * Board, Snake, ItemManager, Gate, ScoreManager 객체 간의 상호작용 조율을 담당함.
 * Author: 박서린 (Stage 3 Item 시스템 추가 구현: 박하은 / Stage 4 gate 시스템 추가 구현: 조성래 / Stage 5 최종 완성: 김현수)
 */

#include <iostream>
#include "Board.h"
#include "Snake.h"
#include "ItemManager.h" // [Stage 3] 객체지향 설계를 위한 아이템 제어 컴포넌트 추가
#include "Gate.h"
#include "ScoreManager.h" // [Stage 5] 점수 및 미션 관리 컴포넌트 추가

extern "C" {
#include "curses.h"
}

#pragma comment(lib, "pdcurses.lib")

int main() {
    // =====================================================================
    // 1. 시스템 초기화 파트 (ncurses 환경 설정)
    // =====================================================================
    initscr();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    timeout(500); // 게임 프레임워크 갱신 주기 설정 (Tick: 500ms)

    int currentStage = 1;
    bool gameRunning = true;

    ScoreManager scoreMgr; // [Stage 5] 역사(History) 보존 및 연속 적산을 위해 대형 루프 바깥으로 격상

    // 전체 5개 스테이지 진행을 위한 대형 루프 구조 설계
    while (currentStage <= 5 && gameRunning) {
        // =====================================================================
        // 2. 주요 도메인 객체 생성 (전역 변수 최소화 지침 준수)
        // =====================================================================
        Board board;
        board.loadStage(currentStage); // 동적 스테이지 맵 로드

        Snake snake; // 뱀 객체 인스턴스 초기화 (매 스테이지 초기화 지침)
        ItemManager itemMgr; // 인스턴스 캡슐화를 통한 독립적 아이템 상태 관리
        Gate gate; // Gate 시스템 초기화

        scoreMgr.initStage(currentStage, snake.getLength()); // [Stage 5] 스코어 매니저 초기화 및 연동

        // 게임 시작 시 Board 데이터를 읽어 Gate 쌍 1회 생성 (한 번 생성된 Gate는 사라지지 않음)
        gate.generateGates(board);
        bool stageRunning = true;

        // =====================================================================
        // 3. 게임 메인 프레임 루프 (실시간 상태 업데이트 및 렌더링)
        // =====================================================================
        while (stageRunning) {
            // [Stage 3] 아이템 수명 주기(Tick) 업데이트 및 상시 목표 수량(3개) 스폰 제어
            itemMgr.updateTicks();
            while (itemMgr.getItemCount() < 3) {
                itemMgr.spawnItem(board, snake.getBody());
            }

            int ch = getch();
            if (ch == 'q' || ch == 'Q') {
                stageRunning = false;
                gameRunning = false;
                break;
            }

            // [Stage 5 추가] QA 테스트 및 예외 상황 유연 대처용 스테이지 재시작 단축키 (R)
            if (ch == 'r' || ch == 'R') {
                stageRunning = false; // 현재 내부 스테이지 프레임만 폭파하고 바깥 루프에서 동일 스테이지 재진입
                break;
            }

            // [Stage 5 추가] 교수님 최종 시연 및 빠른 모듈 검증용 스테이지 스킵 단축키 (N)
            if (ch == 'n' || ch == 'N') {
                scoreMgr.saveStageRecord(); // 현재까지 진행 기록 백업
                stageRunning = false;
                currentStage++; // 스테이지 강제 워프 연산
                break;
            }

            // 사용자 키 입력에 따른 뱀의 진행 방향 전환 및 유효성 검사
            if (!snake.changeDirection(ch)) {
                stageRunning = false;
                gameRunning = false;
                break;
            }

            // 차기 프레임의 헤드(Head) 예정 좌표 산출
            Pos nextP = snake.getNextHead();
            bool isTeleporting = false; // 텔레포트 상태 확인용 플래그

            // [Stage 4] Gate 진출입 및 텔레포트 검사 로직
            if (gate.isGate(nextP)) {
                Pos exitGate = gate.getOtherGate(nextP);
                Direction newDir = gate.getExitDirection(board, exitGate, snake.getDirection());

                snake.setDirection(newDir); // 나갈 방향으로 머리 방향 강제 전환
                nextP = exitGate;           // 충돌을 무시하고 머리 좌표를 출구로 텔레포트 처리
                isTeleporting = true;       // 방금 텔레포트 했음을 표시
                scoreMgr.addGate();         // [Stage 5] 게이트 통과 카운트 가산
            }

            // 환경 요소(벽) 및 물리적 내부 충돌(몸통)에 대한 충돌 검사 수행
            // 방금 게이트를 타고 출구로 나온 상태(isTeleporting == true)라면, 
            // 출구 자체가 원래 '벽'이므로 이번 턴에서는 벽 충돌 검사를 무시합니다!
            if ((!isTeleporting && board.checkWallCollision(nextP)) || snake.checkBodyCollision(nextP)) {
                stageRunning = false;
                gameRunning = false;
                break;
            }

            // [Stage 3] 차기 좌표 기준 아이템 오브젝트와의 충돌(획득) 여부 판정
            bool ateItem = false;
            bool isGrowth = false;
            ateItem = itemMgr.checkCollision(nextP.y, nextP.x, isGrowth);

            if (ateItem) {
                if (isGrowth) {
                    // Growth Item 획득 시: 연장 플래그(true) 전달을 통한 뱀 몸통 확장 유도
                    snake.move(nextP, true);
                    scoreMgr.addGrowth(); // [Stage 5] Growth 획득 카운트 가산
                }
                else {
                    // Poison Item 획득 시: 기본 이동 처리 후 축소 메커니즘 수행
                    snake.move(nextP, false);

                    // [수정] 이동 후 꼬리를 한 칸 더 잘라내어 실질적인 길이 축소 적용
                    snake.shrink();
                    scoreMgr.addPoison(); // [Stage 5] Poison 획득 카운트 가산

                    // 프로젝트 기능 명세 제약 조건 검사:
                    // Poison 아이템 누적으로 인해 뱀의 총 길이가 최소 기준(3 미만) 하회 시 게임 오버 처리
                    if (snake.getLength() < 3) {
                        stageRunning = false;
                        gameRunning = false;
                        break;
                    }
                }
            }
            else {
                // 아이템 비획득 시: 정상 전진 이동 처리 (기존 꼬리 노드 반환)
                snake.move(nextP, false);
            }

            // [Stage 5] 매 틱마다 실시간 뱀 길이 및 시간 스코어 데이터 적산
            scoreMgr.updateLength(snake.getLength(), snake.getMaxLength());
            scoreMgr.updateTime(); // 매 프레임 경과 시간 계산 연산 유도

            // [Stage 5] 실시간 맵 클리어 검증 파이프라인
            if (scoreMgr.checkMissionComplete()) {
                scoreMgr.saveStageRecord(); // 스테이지 통과 시 현 시점 데이터 백업 아카이빙

                // [UX 고도화 인터페이스] 대기 모드로 전환하여 유저가 클리어 메시지를 인지할 수 있도록 함
                timeout(-1);
                scoreMgr.renderStageClearScreen();
                getch(); // 유저의 입력 대기 제어
                timeout(500); // 다음 스테이지 연산을 위해 지연 클럭 타이머 500ms로 원복

                stageRunning = false;       // 미션 완료 시 현재 스테이지 내부 루프 탈출
                currentStage++;             // 차기 스테이지 인덱스 점증
                break;
            }

            // =====================================================================
            // 4. 화면 출력 및 그래픽 동기화 파트 (렌더링 갱신)
            // =====================================================================
            // 하부 구조 보드판 및 뱀 데이터 렌더링
            board.draw(snake.getBody(), snake.getLength(), snake.getMaxLength());

            // [Stage 3] 보드 레이어 상단에 상호작용 아이템(G/P) 오브젝트 최종 오버레이 출력
            itemMgr.renderItems();

            // [Stage 4] 생성된 Gate 객체를 화면에 출력
            if (gate.isActive()) {
                // A, B라는 임의의 기호로 맵 위에 Gate를 그립니다 (기호는 원하시는 문자로 변경 가능)
                mvprintw(gate.getGateA().y, gate.getGateA().x, "A");
                mvprintw(gate.getGateB().y, gate.getGateB().x, "B");
            }

            // [Stage 5] 우측 레이어 영역에 스코어 및 미션 현황판 오버레이 출력
            scoreMgr.renderScoreBoard();

            refresh();
        }
    }

    // [Stage 5] 게임 완전 종료 시(전체 클리어 혹은 오버) 최종 데이터 정산 통계 스크린 샌드박스 가동
    timeout(-1); // 결과를 팀장님이 확인할 수 있도록 ncurses 입력 대기 시간 무한대 전환
    scoreMgr.saveStageRecord(); // 마지막 플레이 중이던 최종 스테이지 레코드 마킹
    scoreMgr.renderFinalSummary();
    getch();

    // 시스템 리소스 해제 및 가상 터미널 종료
    endwin();
    return 0;
}