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
│   ├── Gate.cpp              # [4단계] 게이트 한 쌍 + 추가 Wall 동작
│   └── ScoreBoard.cpp        # [5단계] (예정)
│
├── include/           # 헤더 파일 (.h)
│   ├── common.h              # 공용 enum, 상수
│   ├── curses_compat.h       # OS별 curses + sleep 호환 레이어
│   ├── Map.h
│   ├── Snake.h
│   ├── Item.h                
│   ├── Gate.h                # [4단계] 게이트 클래스
│   └── ScoreBoard.h          # (예정)
│
└── stages/            # 스테이지 맵 데이터 (텍스트 파일)
    ├── stage1.txt
    ├── stage2.txt           # (예정)
    ├── stage3.txt           # (예정)
    └── stage4.txt           # 4단계용 — 내부 island wall 포함
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
| 4 | `Gate.*` | 김주한 | 게이트 한 쌍 + 추가 Wall 동작 |
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
| 9 | `USED_GATE_WALL` | 워프 흔적 (Gate가 한 번 떴던 자리, 다시는 Gate 안 생김) |

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
| `USED_GATE_WALL` | `'#'` | 노랑 |

---

## 7. SPEED_ITEM tick 동작 방식

- 속도 아이템을 먹으면 **30 item-tick** 동안 속도 효과가 유지됩니다.
- 효과 중에는 `sleep_usec`이 `TICK_USEC / 2` (기본의 절반)로 줄어 뱀 이동이 2배 빨라집니다.
- 아이템(Growth/Poison)은 속도 효과 중 **2 snake-tick마다 1번**만 갱신되어, 아이템 타이머는 정상 속도를 유지합니다.

| 상태 | snake-tick 간격 | item-tick 간격 |
|------|----------------|----------------|
| 일반 | 200ms | 200ms |
| 속도 효과 중 | 100ms | 200ms (2 snake-tick마다 1회) |

---

## 8. Gate 동작 방식 (4단계)

- 게임 시작 후 **30 tick** 뒤 첫 한 쌍이 임의의 일반 Wall(Immune Wall 제외) 자리에 출현합니다.
- 한 쌍은 **100 tick** 동안 살아 있고, 수명이 다하면 다른 위치로 이동합니다.
- 머리가 Gate 칸으로 진입하면 **출구 Gate 너머 한 칸**으로 즉시 텔레포트하며, 진행 방향도 자동으로 갱신됩니다.

### 진출 방향 결정 (게임 규칙 #4)
| 출구 Gate 위치 | 진출 방향 |
|---------------|----------|
| 가장자리 Wall  | 항상 맵 안쪽으로 고정 (상단→아래, 하단→위, 좌→오른쪽, 우→왼쪽) |
| 내부 island Wall | 우선순위: ①진입 방향과 일치 → ②시계방향 회전 → ③반시계방향 회전 → ④반대방향 |

내부 Gate에서 위 4방향이 전부 막혀 있으면 게임 오버로 처리합니다.

### (3) 추가 Wall 동작 — 워프 흔적 (Used Gate Wall)
- **한 쌍은 1회용**: Snake 가 Gate 를 한 번 통과하는 순간 두 자리가 모두 **`USED_GATE_WALL` (셀 값 9)** 로 즉시 바뀝니다.
- 통과 안 한 채 `GATE_LIFETIME` (100 tick) 이 지나도 자연 만료되어 똑같이 노랑으로 변합니다.
- 사라진 직후 `GATE_RESPAWN_WAIT` (30 tick, 약 6초) 뒤에 **다른 자리** 에 새 한 쌍이 뜹니다.
- 시각적으로 **노란색** 으로 표시되어 회색 Wall 과 한눈에 구분됩니다.
- `Gate::spawnPair` 후보 검색은 `cell == WALL` 인 자리만 모으므로, 워프 흔적은 자동으로 제외됩니다 → **같은 자리에 Gate 가 두 번 안 뜸**.
- 충돌 판정은 일반 Wall 과 동일 (`Snake::move` 에서 `WALL | IMMUNE_WALL | USED_GATE_WALL` 모두 게임오버).
- Snake 가 Gate 를 활발히 사용할수록 노란 흔적이 빨리 누적되어 후반엔 가장자리가 거의 노랑으로 차오릅니다 — 자연스러운 난도 상승.