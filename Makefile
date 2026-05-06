# Snake Game Makefile
#
# 디렉토리 구조
#   src/     : .cpp 파일들
#   include/ : .h 파일들
#   stages/  : 게임 맵 데이터 (빌드와 무관)

CXX      = g++
CXXFLAGS = -std=c++14 -Wall -O2 -Iinclude
LDFLAGS  = -lncursesw

TARGET   = snake
SRCS     = src/main.cpp src/Map.cpp src/Snake.cpp
OBJS     = $(SRCS:.cpp=.o)

all: $(TARGET)

# 실행파일 만들기 (.o 들을 묶어서 링크)
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LDFLAGS)

# .cpp 마다 .o 만드는 일반 규칙
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 빌드 산출물 청소
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
