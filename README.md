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
>
> 🛠 Git 협업 지침 (Workflow)
[!CAUTION]
main 브랜치 직접 커밋 금지
모든 작업은 반드시 개별 브랜치(feature/기능명)를 생성하여 진행하며, main 브랜치는 코드 통합 시에만 사용합니다.

1. 작업 시작 전 환경 동기화
항상 최신 코드를 유지한 상태에서 새로운 작업을 시작해야 합니다.

# 메인 브랜치로 이동 및 최신 코드 반영
git checkout main
git pull origin main

2. 기능별 브랜치 생성
구현할 기능에 맞춰 새로운 브랜치를 생성하고 이동합니다.

# 브랜치 생성 및 이동
# 예: git checkout -b feature/snake_move
git checkout -b feature/[기능명]

3. 작업 내역 커밋
파일 수정 후 로컬 저장소에 변경 사항을 기록합니다.

# 수정된 전체 파일 추가
git add .

# 커밋 메시지 작성
# 형식: [머리말]: [내용]
git commit -m "[type]: [message]"

커밋 머리말(type) 규칙:

feat: 새로운 기능 추가

fix: 버그 수정

docs: 문서 수정

refactor: 코드 리팩토링

4. 원격 저장소 업로드 (Push)
작업이 완료된 브랜치를 GitHub에 업로드합니다.

# 작업 중인 브랜치 이름으로 Push
git push origin feature/[기능명]

5. Pull Request (PR) 생성 및 병합
GitHub 웹 페이지에서 본인의 브랜치를 main으로 합치는 Pull Request를 생성합니다.

GitHub 레포지토리 접속 후 Compare & pull request 클릭

구현 내용 요약 작성 후 Create pull request 클릭

팀장의 코드 리뷰 및 승인 후 병합(Merge) 진행 *병합은 팀장이 진행할 예정! 팀원 분들은 PR만~

6. 로컬 환경 정리
병합이 완료된 후에는 다시 로컬 환경을 최신화합니다.

git checkout main
git pull origin main
# 작업이 종료된 브랜치 삭제
git branch -d feature/[기능명]


