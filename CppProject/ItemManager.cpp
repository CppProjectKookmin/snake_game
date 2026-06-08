/*
 * File: ItemManager.cpp
 * Description: ItemManager 클래스의 구현부.
 * 의사 난수(Pseudo-random) 기반의 좌표 산출 및 예외 처리 조건 검증,
 * 실시간 수명 감쇠 플래그 처리 및 독립적 렌더링 파이프라인 수행.
 * Author: 박하은 (아이템 다양화 및 피버 타임 확장: 김현수)
 */

#include "ItemManager.h"
#include <cstdlib>
#include <ctime>

extern "C" {
#include "curses.h"
}

ItemManager::ItemManager() {
    // 실행 시점 기반 시드 배정 알고리즘을 통한 난수 생성기 초기화
    srand(static_cast<unsigned int>(time(NULL)));
}

void ItemManager::updateTicks() {
    // 루프 순회를 통한 전체 활성 아이템의 생존 수명 주기(Tick) 감쇠 및 만료 객체 소멸 처리
    for (auto it = items.begin(); it != items.end(); ) {
        it->lifetime--;
        if (it->lifetime <= 0) {
            it = items.erase(it); // 20틱 수명 만료 시 컨테이너에서 즉시 제거
        }
        else {
            ++it;
        }
    }
}

int ItemManager::getItemCount() const {
    return items.size();
}

void ItemManager::spawnItem(const Board& board, const std::deque<Pos>& snakeBody) {
    // 도메인 명세 제약 조건 검사: 최대 허용 수량 도달 시 스폰 프로세스 조기 차단
    if (items.size() >= MAX_ITEMS) return;

    int attempts = 0;
    const int MAX_ATTEMPTS = 100; // 무한 루프(Deadlock) 방지를 위한 최대 안전 시도 횟수 한계 설정

    while (attempts < MAX_ATTEMPTS) {
        int rx = rand() % MAP_SIZE;
        int ry = rand() % MAP_SIZE;

        // [수정] Pos 구조체의 순서 명세 { y, x } 에 맞춰 올바르게 좌표 바인딩 (김현수 디버깅)
        Pos p = { ry, rx };

        // 검증 조건 1: 정적 환경 요소(고정 벽 # 및 크래시 경계 X)와의 중첩 여부 판단
        if (board.checkWallCollision(p)) {
            attempts++;
            continue;
        }

        // 검증 조건 2: 동적 물리 객체(뱀의 세그먼트별 바디 좌표)와의 교차 여부 판단
        bool overlapWithSnake = false;
        for (const auto& bodyPart : snakeBody) {
            if (bodyPart.x == rx && bodyPart.y == ry) {
                overlapWithSnake = true;
                break;
            }
        }
        if (overlapWithSnake) {
            attempts++;
            continue;
        }

        // 검증 조건 3: 기생성된 아이템 오브젝트 간의 좌표 중복 생성(Overlay) 방지 검사
        bool overlapWithItem = false;
        for (const auto& item : items) {
            if (item.x == rx && item.y == ry) {
                overlapWithItem = true;
                break;
            }
        }
        if (overlapWithItem) {
            attempts++;
            continue;
        }

        // 모든 데이터 유효성 검증 통과 시 객체 인스턴스화 및 캡슐화 데이터 할당
        Item newItem;
        newItem.y = ry;
        newItem.x = rx;

        // [수정] 아이템 다양화 명세 준수: 확률 분포를 4종류로 확장 (0:G, 1:P, 2:C, 3:S) (김현수 추가)
        int randVal = rand() % 100;
        if (randVal < 40)       newItem.type = 0; // 40% 확률로 Growth
        else if (randVal < 70)  newItem.type = 1; // 30% 확률로 Poison
        else if (randVal < 85)  newItem.type = 2; // 15% 확률로 Coin
        else                    newItem.type = 3; // 15% 확률로 Slow

        newItem.lifetime = ITEM_LIFETIME;

        items.push_back(newItem); // 백 엔드 벡터 컨테이너에 바인딩
        break;
    }
}

// [수정] 상위 도메인(main) 전파를 위해 bool 대신 int 타입으로 충돌 개체 속성 반환 (김현수 수정)
bool ItemManager::checkCollision(int headY, int headX, int& itemType) {
    // 선형 탐색을 통한 차기 프레임 헤드 좌표와 아이템 좌표 간의 기하학적 충돌 검사
    for (auto it = items.begin(); it != items.end(); ++it) {
        if (it->y == headY && it->x == headX) {
            itemType = it->type;     // 상위 도메인(main) 전파를 위한 아이템 속성 플래그 갱신
            items.erase(it);         // 물리적 상호작용 완료에 따른 즉시 삭제 처리
            return true;             // 충돌 판정 승인 반환
        }
    }
    return false;
}

void ItemManager::renderItems() const {
    // ncurses 그래픽 드라이버의 실시간 가상 윈도우 스크린 버퍼 플러시 및 텍스트 렌더링
    for (const auto& item : items) {
        // [수정] 확장된 아이템 타입별 심볼 매칭 출력 (G / P / C / S) (김현수 수정)
        if (item.type == 0)      mvprintw(item.y, item.x, "G"); // Growth Item 개체 최종 오버레이 출력
        else if (item.type == 1) mvprintw(item.y, item.x, "P"); // Poison Item 개체 최종 오버레이 출력
        else if (item.type == 2) mvprintw(item.y, item.x, "C"); // Coin 보너스 아이템 오버레이 출력
        else if (item.type == 3) mvprintw(item.y, item.x, "S"); // Slow 버프 아이템 오버레이 출력
    }
}