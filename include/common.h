// common.h
// 프로젝트 전체가 공유하는 공용 정의 모음.
//   - enum Cell      : 맵 한 칸이 가질 수 있는 셀 값
//   - enum Direction : 뱀의 이동 방향
//   - 맵 크기·뱀 길이 등 상수
//   - struct Position: 좌표 한 쌍

#ifndef COMMON_H
#define COMMON_H

// 맵 셀 값
enum Cell {
    EMPTY           = 0,
    WALL            = 1,
    IMMUNE_WALL     = 2,
    SNAKE_HEAD      = 3,
    SNAKE_BODY      = 4,
    GROWTH_ITEM     = 5,
    POISON_ITEM     = 6,
    GATE            = 7,
    SPEED_ITEM      = 8,  // 3단계 Item 종류 추가사항 도입.
    USED_GATE_WALL  = 9,  // 4단계 (3) 워프 흔적 - 한 번 Gate가 떴던 자리, 다시는 Gate 안 생김
    BLOCK_WALL      = 10, // 4단계 (3) 테트리스 블록 벽 - 맵 안에 잠깐 나타났다 사라지는 벽
    BLOCK_WARN      = 11  // 4단계 (3) 블록 벽 출현 1초 전 예고 표시 (통과 가능)
};

// 이동 방향
enum Direction {
    DIR_UP = 0,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT,
    DIR_NONE
};

// 맵 최소 크기 (명세서 요구사항)
const int MAP_MIN_SIZE = 21;

// 맵 최대 크기 (배열 잡을 때 쓰는 상한)
const int MAP_MAX_SIZE = 30;

// 뱀 몸통 최대 길이
const int SNAKE_MAX_LENGTH = 100;

// 한 칸 = 화면 두 글자 폭 (정사각형처럼 보이게)
const int CELL_WIDTH = 2;

// 좌표 한 쌍
// 원래 Snake에 있었지만, item도 사용하기 위해 공용 헤더에 정의
struct Position {
    int y;
    int x;
};

#endif
