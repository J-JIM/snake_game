# Snake Game (2026 C++ 기말 프로젝트)

C++ + ncurses 로 구현하는 팀 프로젝트입니다.

---

## 1. 개발 환경

| 항목 | 값 |
|------|-----|
| 언어 | C++ (C++14 이상) |
| 라이브러리 | ncurses (macOS/Linux) / PDCurses (Windows) |
| 빌드 도구 | make + g++ |
| OS | macOS / Linux / Windows(MSYS2 MinGW) 모두 지원 |

OS별 curses 헤더 차이는 `include/curses_compat.h` 가 흡수합니다.
소스 코드는 `#include "curses_compat.h"` 한 줄만 쓰면 됨.

### 빌드 & 실행 (macOS)

```bash
# ncurses 는 macOS 기본 제공이라 별도 설치 불필요
make            # 빌드
./snake         # 실행
make clean
```

### 빌드 & 실행 (Linux / WSL)

```bash
sudo apt-get update
sudo apt-get install libncurses5-dev libncursesw5-dev
make
./snake
```

### 빌드 & 실행 (Windows — MSYS2 MinGW64)

[MSYS2](https://www.msys2.org/) 설치 후, **MSYS2 MinGW 64-bit** 셸에서:

```bash
pacman -S --needed mingw-w64-x86_64-toolchain mingw-w64-x86_64-pdcurses make
make
./snake.exe
```

> Makefile이 `$(OS)` 변수를 보고 자동으로 `-lpdcurses` 로 링크합니다.
> Visual Studio(MSVC) 는 지원하지 않습니다. MinGW/MSYS2 사용 권장.

---

## 2. 파일 구조

```
Snake-Game/
├── README.md          # 이 문서
├── Makefile           # 빌드 스크립트 (OS 자동 감지)
├── .gitignore         # git 추적 제외 목록
│
├── src/               # 소스 파일 (.cpp)
│   ├── main.cpp              # 게임 루프 (curses init/end, tick)
│   ├── Map.cpp               # [1단계] 맵 구현
│   ├── Snake.cpp             # [2단계] 뱀 구현
│   ├── Item.cpp              # [3단계] 아이템 구현
│   ├── Gate.cpp              # [4단계] (예정)
│   └── ScoreBoard.cpp        # [5단계] (예정)
│
├── include/           # 헤더 파일 (.h)
│   ├── common.h              # 공용 enum, 상수
│   ├── curses_compat.h       # OS별 curses + sleep 호환 레이어
│   ├── Map.h
│   ├── Snake.h
│   ├── Item.h                
│   ├── Gate.h                # (예정)
│   └── ScoreBoard.h          # (예정)
│
└── stages/            # 스테이지 맵 데이터 (텍스트 파일)
    ├── stage1.txt
    ├── stage2.txt           # (예정)
    ├── stage3.txt           # (예정)
    └── stage4.txt           # (예정)
```

### 왜 이렇게 나눴나?

- **`src/` vs `include/` 분리** — 표준 C++ 프로젝트 레이아웃. IDE에서 헤더만 빨리 찾고 싶을 때 편함.
- **`stages/` 분리** — 코드 아닌 데이터 파일. 비개발자(맵 디자이너)도 만질 수 있게 따로 둠.
- **빌드 산출물(`*.o`, `snake`)은 루트에 생성** — `make clean` 으로 정리. `.gitignore` 에 등록돼 있어 git에는 안 올라감.

---

## 3. 단계별 담당

| 단계 | 파일 | 담당자 | 핵심 내용 |
|------|------|--------|----------|
| 1 | `Map.*` | 김주한 | 맵 표시 |
| 2 | `Snake.*` | 김주한 | 뱀 이동 |
| 3 | `Item.*` | 권동욱 | 아이템 출현/획득 |
| 4 | `Gate.*` | 미정 | 게이트 |
| 5 | `ScoreBoard.*` | 미정 | 점수판/미션 |


---

## 4. 셀 값 약속 (`common.h`)

맵의 각 칸은 아래 정수 값으로 표현합니다.
이 값은 절대 마음대로 바꾸지 말고, 새 종류가 필요하면 팀에 공유 후 추가하세요.

| 값 | 이름 | 의미 |
|----|------|------|
| 0 | `EMPTY` | 빈 칸 |
| 1 | `WALL` | 일반 벽 (Gate로 변할 수 있음) |
| 2 | `IMMUNE_WALL` | 불멸 벽 (Gate로 변하지 않음, 보통 모서리) |
| 3 | `SNAKE_HEAD` | 뱀 머리 |
| 4 | `SNAKE_BODY` | 뱀 몸통 |
| 5 | `GROWTH_ITEM` | Growth 아이템 |
| 6 | `POISON_ITEM` | Poison 아이템 |
| 7 | `GATE` | Gate |
| 8 | `SPEED_ITEM` | SPEED 아이템 |

---

## 5. 스테이지 파일 형식 (`stages/stageN.txt`)

- 첫 줄: `행 열` (예: `21 21`)
- 다음 줄부터: 한 줄에 한 행씩, 각 칸을 위 셀 값으로 표기
- 최소 크기는 **21x21**

### 예시

```
21 21
211111111111111111112
100000000000000000001
100000000000000000001
100000000000000000001
100000000000000000001
100000000000000000001
100000000000000000001
100000000000000000001
100000000000000000001
100000000000000000001
100000000034400000001
100000000000000000001
100000000000000000001
100000000000000000001
100000000000000000001
100000000000000000001
100000000000000000001
100000000000000000001
100000000000000000001
100000000000000000001
211111111111111111112
```


### 새 스테이지 추가 방법

1. `stages/` 폴더에 `stageN.txt` 파일 생성
2. 위 형식대로 작성
3. (5단계 구현 후) Stage 매니저에 등록

---

## 6. 화면에서의 표시 방법 (참고)

ncurses에서 각 셀을 어떻게 그릴지는 `Map::draw()` 에서 처리합니다.

| 셀 | 표시 문자 | 색 |
|----|----------|-----|
| `EMPTY` | `' '` (공백) | - |
| `WALL` | `'#'` | 회색 |
| `IMMUNE_WALL` | `'#'` | 검정 |
| `SNAKE_HEAD` | `'@'` | 노랑 |
| `SNAKE_BODY` | `'o'` | 주황 |
| `GROWTH_ITEM` | `'+'` | 초록 |
| `POISON_ITEM` | `'-'` | 빨강 |
| `GATE` | `'G'` | 마젠타 |
| `SPEED_ITEM` | `'&'` | 시안 |

---

## 7. SPEED_ITEM tick 동작 방식

- 속도 아이템을 먹으면 **30 item-tick** 동안 속도 효과가 유지됩니다.
- 효과 중에는 `sleep_usec`이 `TICK_USEC / 2` (기본의 절반)로 줄어 뱀 이동이 2배 빨라집니다.
- 아이템(Growth/Poison)은 속도 효과 중 **2 snake-tick마다 1번**만 갱신되어, 아이템 타이머는 정상 속도를 유지합니다.

| 상태 | snake-tick 간격 | item-tick 간격 |
|------|----------------|----------------|
| 일반 | 200ms | 200ms |
| 속도 효과 중 | 100ms | 200ms (2 snake-tick마다 1회) |