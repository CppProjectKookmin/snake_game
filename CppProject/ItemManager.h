/*
 * File: ItemManager.h
 * Description: ItemManager 클래스의 선언부. 
 * 상시 목표 수량(3개)의 아이템 동적 스폰 메커니즘 정의,
 * 20틱 수명 주기 관리 및 뱀 객체와의 충돌 판정 인터페이스 제공.
 * Author: 박하은
 */

#ifndef ITEMMANAGER_H
#define ITEMMANAGER_H

#include <vector>
#include <deque>
#include "Board.h"

// [Stage 3] 아이템 개별 상태 관리를 위한 데이터 구조체
// ItemManager.h 내부 구조체 변경점
struct Item {
    int y, x;
    int type; // 0: Growth(G), 1: Poison(P), 2: Coin(C), 3: Slow(S)
    int lifetime;
};

class ItemManager {
private:
    std::vector<Item> items;      // 활성화된 아이템 객체 저장 컨테이너
    const int MAX_ITEMS = 3;      // 환경 내 상시 유지할 목표 아이템 총량
    const int ITEM_LIFETIME = 20; // 아이템별 최대 생존 시간 (Tick 단위)

public:
    ItemManager();
    
    // 아이템 시스템 상태 업데이트 및 수명 주기 제어
    void updateTicks();
    
    // 현재 맵 내에 존재하는 활성 아이템 수량 반환
    int getItemCount() const;
    
    // 유효 좌표(벽, 뱀 몸통, 기존 아이템 제외) 탐색 및 아이템 동적 스폰
    void spawnItem(const Board& board, const std::deque<Pos>& snakeBody);
    
    // 뱀 헤드 좌표 기준 아이템 획득 및 상호작용 충돌 판정
    bool checkCollision(int headY, int headX, int& itemType);
    
    // ncurses 가상 터미널 버퍼 레이어 상단에 아이템 기호 오버레이 출력
    void renderItems() const;
};

#endif