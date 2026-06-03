/*
 * File: Board.h
 * Description: Board 클래스의 선언부. 게임 맵(스테이지) 데이터 유지 및 화면 렌더링
 * Author: 박서린
 */

#ifndef BOARD_H
#define BOARD_H

#include <deque>
#include "Snake.h"

const int MAP_SIZE = 21;

class Board {
private:
    int stageData[MAP_SIZE][MAP_SIZE];

public:
    Board();
// 맵 데이터를 외부에서 안전하게 읽기 위한 Getter
    int getData(const int y, const int x) const;

    void loadStage(const int stageIndex);
    void draw(const std::deque<Pos>& snakeBody, const int current_len, const int max_len) const;
    bool checkWallCollision(const Pos p) const;
};

#endif