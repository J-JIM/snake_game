# Snake Game 개발 환경
# Ubuntu 22.04 + g++ + ncurses + 한글 locale

FROM ubuntu:22.04

# 1) 빌드 도구 + ncurses (wide char 버전 포함) + locale 패키지
RUN apt-get update && apt-get install -y \
    g++ \
    make \
    libncurses-dev \
    libncursesw5-dev \
    locales \
    && rm -rf /var/lib/apt/lists/*

# 2) 한글/영어 UTF-8 locale 생성 (ncurses 한글 출력용)
RUN locale-gen ko_KR.UTF-8 en_US.UTF-8
ENV LANG=ko_KR.UTF-8
ENV LC_ALL=ko_KR.UTF-8

# 3) 작업 디렉토리 (호스트 폴더가 여기로 마운트됨)
WORKDIR /app

# 4) 컨테이너 들어가면 bash 셸
CMD ["bash"]
