// main.cpp
// 시스템 진입점: ncurses 환경 초기화 후 GameEngine을 구동하여 구조적 최적화 완성

#include "curses_compat.h" // OS별 curses + sleep 호환 레이어
#include <locale.h>        // 한글 출력용 locale 설정
#include <ctime>           // srand 사용을 위한 ctime
#include <cstdlib>         // srand/rand
#include "GameEngine.h"

int main()
{
    // 윈도우 환경에서 콘솔 코드페이지를 UTF-8로 설정하여 한글 출력 및 프리즈 방지
    #ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    #endif

    // 한글 출력을 위해 시스템 locale 사용
    #ifdef _WIN32
    setlocale(LC_ALL, ".UTF-8");
    #else
    setlocale(LC_ALL, "");
    #endif

    // 무작위 난수 생성을 위한 시드값 초기화
    srand(time(NULL));

    // ===== 1) ncurses 초기화 =====
    initscr();             // ncurses 모드 시작
    start_color();         // 색상 사용 선언
    
    keypad(stdscr, TRUE);  // 화살표 키 입력 받기
    noecho();              // 입력한 키를 화면에 보여주지 않음
    curs_set(0);           // 커서 안 보이게
    nodelay(stdscr, TRUE); // getch()가 키 없으면 기다리지 않게 (tick 진행용)

    // ===== 2) 극장 매니저(GameEngine) 기용 및 구동 =====
    GameEngine engine;
    engine.start();

    // ncurses 종료 및 터미널 모드 복원
    endwin();
    return 0;
}
