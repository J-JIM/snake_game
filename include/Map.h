#ifndef MAP_H
#define MAP_H

#include "common.h"

class Map {
public:
    Map();

    // 텍스트 파일에서 맵을 읽어옴. 성공 시 true.
    bool loadFromFile(const char* filename);

    // 현재 맵을 ncurses 화면에 그림 (offsetY, offsetX 부터 시작)
    void draw(int offsetY, int offsetX) const;

    // 셀 값 읽기/쓰기
    int  getCell(int y, int x) const;
    void setCell(int y, int x, int value);

    int getHeight() const { return height; }
    int getWidth()  const { return width;  }

    // ncurses 색 페어 초기화. 게임 시작할 때 한 번만 호출.
    void initColors() const;

private:
    int data[MAP_MAX_SIZE][MAP_MAX_SIZE];
    int height;
    int width;
};

#endif
