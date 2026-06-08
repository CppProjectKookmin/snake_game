/*
 * File: Snake.h
 * Description: Snake 클래스의 선언부. 뱀의 상태(좌표, 방향, 길이) 관리 및 이동 알고리즘 담당
 * Author: 박서린
 */

#ifndef SNAKE_H
#define SNAKE_H

#include <deque>

enum Direction { UP = 0, RIGHT = 1, DOWN = 2, LEFT = 3 };

struct Pos {
    int y, x;
};

class Snake {
private:
    std::deque<Pos> body;
    Direction current_dir;
    int current_length;
    int max_length;

public:
    Snake(int stage);

    bool changeDirection(const int ch);
    void move(const Pos nextP, const bool grow);
    void shrink() { body.pop_back(); current_length--; } // [Stage 3] 독 아이템 획득 시 꼬리 강제 제거 함수

    Pos getNextHead() const;
    bool checkBodyCollision(const Pos p) const;
    const std::deque<Pos>& getBody() const;
    int getLength() const;
    int getMaxLength() const;
    Direction getDirection() const;           // 현재 방향 반환
    void setDirection(const Direction dir);   // 게이트 통과 후 방향 수정
};

#endif