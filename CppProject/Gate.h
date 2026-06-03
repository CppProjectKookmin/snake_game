/*
 * File: Gate.h
 * Description: Gate 클래스의 선언부. 임의의 벽에 겹치지 않는 게이트 쌍을 생성하고,
 *              뱀 진입 시 반대편 출구와 진출 방향 알고리즘을 계산합니다.
 * Author: [조성래]
 */

#ifndef GATE_H
#define GATE_H

#include "Snake.h" // Pos, Direction 구조체 사용을 위함
class Board;       // 전방 선언 (순환 참조 방지)

class Gate {
private:
    Pos gateA;
    Pos gateB;
    bool bIsActive;

public:
    Gate();

    // 맵 데이터를 읽어 일반 벽(1) 중 임의의 위치에 게이트 2개 생성
    void generateGates(const Board& board);

    // 특정 좌표가 게이트인지 확인 (멤버 변수를 수정하지 않으므로 const 필수)
    bool isGate(const Pos p) const;

    // 진입한 게이트의 반대편(진출) 게이트 좌표 반환
    Pos getOtherGate(const Pos p) const;

    // 진출 게이트의 위치와 진입 방향을 바탕으로 진출할 방향 계산
    Direction getExitDirection(const Board& board, const Pos exitGate, const Direction entryDir) const;

    // 게이트 그리기용 Getter 함수들
    Pos getGateA() const;
    Pos getGateB() const;
    bool isActive() const;
};

#endif