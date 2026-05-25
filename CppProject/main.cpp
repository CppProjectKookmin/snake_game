/*
 * File: main.cpp
 * Description: 프로그램 진입점. ncurses 초기화, 게임 메인 루프 실행 및 객체(Board, Snake) 간의 상호작용 조율
 * Author: 박서린
 */

#include <iostream>
#include "Board.h"
#include "Snake.h"

extern "C" {
#include "curses.h"
}

#pragma comment(lib, "pdcurses.lib")

int main() {
    // ncurses 초기 설정
    initscr();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    timeout(500); // Tick 설정(500ms)

    Board board;
    Snake snake;
    bool running = true;

    while (running) {
        int ch = getch();
        if (ch == 'q' || ch == 'Q') {
            running = false;
            break;
        }

        // 키 입력 처리
        if (!snake.changeDirection(ch)) {
            running = false;
            break;
        }

        // 다음 머리 위치
        Pos nextP = snake.getNextHead();

        // 벽, 몸통 충돌 판정
        if (board.checkWallCollision(nextP) || snake.checkBodyCollision(nextP)) {
            running = false;
            break;
        }

        // 뱀 이동
        snake.move(nextP, false);

        // 화면 렌더링
        board.draw(snake.getBody(), snake.getLength(), snake.getMaxLength());
    }

    endwin();
    return 0;
}