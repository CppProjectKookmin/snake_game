# snake_game

> [!TIP]
> **📁 팀 프로젝트 기본 디렉터리 구조**
> 아래 구조를 반드시 유지한 상태로 작업해 주세요!
> 
> ```text
> [2026-SnakeGame]
>  │
>  ├── .gitignore
>  ├── README.md
>  ├── CppProject.sln
>  │
>  └── CppProject/
>       ├── main.cpp
>       ├── curses.h
>       ├── panel.h
>       ├── pdcurses.lib
>       ├── CppProject.vcxproj
>       └── CppProject.vcxproj.filters
> ```




<br>
### 코딩환경은 visual studio 기준으로 만들어놓긴 했는데 다른 환경 써도 큰 문제는 없을 거에요!<br>
### 코딩할 때, 꼭 주요기능엔 주석달면서 해주면 좋습니다! (*결국 교수님께 제출해야하기에!)<br>
### 작업할 때, 교수님이 준 pdf 잘 보고 기준 잘 숙지하면서 코딩하기!! (중요 x 100)<br>
### 궁금한거, 이상한거 생기면 바로바로 연락해주세요!<br>
<br>

**본인이 만든 파일명, 주요 함수 이름, 알고리즘 설명 (자세히)**

**개발하면서 겪은 가장 큰 에러와 해결 방법 1개 (제한 요소 파트용)**

**새로 고안한 신박한 알고리즘이 있다면 그 내용**

**자기평가 (어려웠던 점, 배운 점 등 한 문단)**

**해당 항목 정리해서 이번달 까지!!**
"

## 🛠 Git 협업 지침 (Workflow)

> [!CAUTION]
> **`main` 브랜치 직접 커밋 금지**
> 모든 작업은 반드시 개별 브랜치(`feature/기능명`)를 생성하여 진행하며, `main` 브랜치는 코드 통합 시에만 사용합니다.

<br>

### 1. 작업 시작 전 환경 동기화
항상 최신 코드를 유지한 상태에서 새로운 작업을 시작해야 합니다.

**메인 브랜치로 이동 및 최신 코드 반영**
```bash
git checkout main
git pull origin main
```

### 2. 기능별 브랜치 생성
구현할 기능에 맞춰 새로운 브랜치를 생성하고 이동합니다.

**브랜치 생성 및 이동 (예: feature/snake_move)**
```bash
git checkout -b feature/[기능명]
```

### 3. 작업 내역 커밋
파일 수정 후 로컬 저장소에 변경 사항을 기록합니다.

**수정된 전체 파일 장바구니에 추가**
```bash
git add .
```

**커밋 메시지 작성 (형식: [머리말]: [내용])**
```bash
git commit -m "[type]: [message]"
```
> **커밋 머리말(type) 규칙**
> * `feat`: 새로운 기능 추가
> * `fix`: 버그 수정
> * `docs`: 문서 수정
> * `refactor`: 코드 리팩토링

### 4. 원격 저장소 업로드 (Push)
작업이 완료된 브랜치를 GitHub에 업로드합니다.

**작업 중인 브랜치 이름으로 Push**
```bash
git push --set-upstream origin feature/[기능명]
```

### 5. Pull Request (PR) 생성 및 병합
GitHub 웹 페이지에서 본인의 브랜치를 `main`으로 합치는 **Pull Request**를 생성합니다.
1. GitHub 레포지토리 접속 후 **Compare & pull request** 클릭
2. 구현 내용 요약 작성 후 **Create pull request** 클릭
3. 팀장의 코드 리뷰 및 승인 후 병합(Merge) 진행

### 6. 로컬 환경 정리
병합이 완료된 후에는 다시 로컬 환경을 최신화합니다.

**최신화 및 사용이 끝난 브랜치 삭제**
```bash
git checkout main
git pull origin main
git branch -d feature/[기능명]
```


맥/리눅스 환경 터미널 실행 가이드
윈도우(Visual Studio) 환경이 아닌 맥 또는 리눅스 사용자는 아래 절차에 따라 터미널에서 직접 컴파일하여 실행할 수 있습니다.

1. 필수 라이브러리 설치
대부분의 맥/리눅스 환경에는 ncurses가 기본 설치되어 있으나, 없을 경우 아래 명령어로 설치합니다.

Mac (Homebrew): brew install ncurses

Linux (Ubuntu): sudo apt-get install libncurses5-dev libncursesw5-dev

2. 소스 코드 수정 (중요)
윈도우 전용 헤더(curses.h)와 맥/리눅스 표준 헤더(ncurses.h)가 다르므로, 코드 최상단에 아래 코드를 추가하여 호환성을 확보해야 합니다.

C++
#if defined(_WIN32)
    #include "curses.h"   // Windows (PDCurses)
#else
    #include <ncurses.h> // Mac/Linux (ncurses)
#endif
3. 컴파일 및 실행
터미널을 열고 프로젝트 폴더로 이동한 뒤 아래 명령어를 입력합니다.

컴파일 (Compile)

Bash
# 모든 .cpp 파일을 묶어서 snake_game이라는 실행 파일로 만듭니다.
g++ *.cpp -lncurses -o snake_game
실행 (Run)

Bash
./snake_game
⚠️ 주의 사항
PDCurses 파일 제외: 컴파일 시 윈도우용 라이브러리 파일인 pdcurses.lib나 curses.h 파일이 같은 폴더에 있어도 -lncurses 옵션이 시스템 라이브러리를 우선 참조하므로 무시해도 괜찮습니다.

화면 깨짐: 실행 후 화면이 깨진다면 터미널 창의 크기를 충분히 키우고 다시 실행해 보세요.



