/*
 * File: Snake.cpp
 * Description: Snake 클래스의 구현부. 뱀의 초기화, 방향 전환, 다음 이동 좌표 계산 로직
 * Author: 박서린
 */

#include "Snake.h"

extern "C" {
#include "curses.h"
}

Snake::Snake() {
    current_dir = LEFT;
    current_length = 3;
    max_length = 3;

    // 뱀 초기 좌표
    body.push_back({ 10, 10 });
    body.push_back({ 10, 11 });
    body.push_back({ 10, 12 });
}

// 방향 전환 로직: 반대 방향 입력 시 false 반환하여 실패 처리 유도
bool Snake::changeDirection(const int ch) {
    if (ch == KEY_UP) {
        if (current_dir == DOWN) return false;
        current_dir = UP;
    }
    else if (ch == KEY_DOWN) {
        if (current_dir == UP) return false;
        current_dir = DOWN;
    }
    else if (ch == KEY_LEFT) {
        if (current_dir == RIGHT) return false;
        current_dir = LEFT;
    }
    else if (ch == KEY_RIGHT) {
        if (current_dir == LEFT) return false;
        current_dir = RIGHT;
    }
    return true;
}

// 다음 틱에 이동할 머리 좌표 계산
Pos Snake::getNextHead() const {
    const int dy[4] = { -1, 0, 1, 0 };
    const int dx[4] = { 0, 1, 0, -1 };

    return { body.front().y + dy[current_dir], body.front().x + dx[current_dir] };
}

// 이동 로직 (머리 추가 및 꼬리 제거)
void Snake::move(const Pos nextP, const bool grow) {
    body.push_front(nextP);
    if (!grow) {
        body.pop_back();
    }
    else {
        current_length++;
        if (current_length > max_length) max_length = current_length;
    }
}

// 자기 몸통과의 충돌 검사
bool Snake::checkBodyCollision(const Pos p) const {
    for (const auto& bodyPart : body) {
        if (bodyPart.y == p.y && bodyPart.x == p.x) return true;
    }
    return false;
}

// 캡슐화 상태 반환
const std::deque<Pos>& Snake::getBody() const { return body; }
int Snake::getLength() const { return current_length; }
int Snake::getMaxLength() const { return max_length; }

Direction Snake::getDirection() const { return current_dir; }
void Snake::setDirection(const Direction dir) { current_dir = dir; }