#include <Windows.h>
#include <conio.h>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <stack>
#include <queue>
#include <thread> 
#include <mutex> 
using namespace std;

const int Size = 20;
int maze[Size][Size];     // 미로임
bool visited_dfs[Size][Size]; // 각 칸의 방문여부를 저장(dfs)
bool visited_bfs[Size][Size]; // 각 칸의 방문여부를 저장(bfs)
int Distance[Size][Size]; // 각 칸까지의 최단 거리를 저장
bool complete = false;   //정상적인 미로인지 참 거짓
int playerX = 0, playerY = 0; // 플레이어의 현재 위치
int MonsterX = -1, MonsterY = -1, Monster_start_count=10; //몬스터의 현재 위치
int ct = 0;

mutex mtx;

void initializeMaze() //미로 생성 함수
{
    complete = false;
    memset(visited_dfs, false, sizeof(visited_dfs)); // visited 배열을 초기화

    //0은 길 1은 벽임
    maze[0][0] = 0; //시작은 반드시 길이어야함.
    maze[9][9] = 0; //도착점 역시 반드시 길이어야함.

    // 나머지 셀들을 랜덤으로 0 또는 1로 설정
    for (int i = 0; i < Size; ++i) {
        for (int j = 0; j < Size; ++j) {
            if (!((i == 0 && j == 0) || (i == Size - 1 && j == Size - 1))) {
                maze[i][j] = rand() % 2;
            }
        }
    }
}


bool dfs(int x, int y) //지도의 시작부터 끝까지 연결되었는지 확인하기 위한 dfs
{
    if (x < 0 || x >= Size || y < 0 || y >= Size || visited_dfs[x][y] || maze[x][y] == 1)
        return false;

    visited_dfs[x][y] = true;

    if (x == Size - 1 && y == Size - 1) {
        complete = true;
        return true;
    }

    if (dfs(x + 1, y) || dfs(x - 1, y) || dfs(x, y + 1) || dfs(x, y - 1))
        return true;

    return false;
}

void bfs() //지도의 최단거리를 저장하기 위한 bfs
{
    struct Point {
        int x, y;
    };

    int dx[4] = { 0, 1, 0, -1 };
    int dy[4] = { 1, 0, -1, 0 };

    queue<Point> q;
    q.push({ 0, 0 });
    visited_bfs[0][0] = true;
    Distance[0][0] = 1; // 시작점은 0임

    while (!q.empty()) {
        Point p = q.front();
        q.pop();

        if (p.x == Size - 1 && p.y == Size - 1)
            break; // 목적지에 도달
        

        for (int i = 0; i < 4; ++i) {
            int nx = p.x + dx[i];
            int ny = p.y + dy[i];

            if (nx >= 0 && nx < Size && ny >= 0 && ny < Size && maze[nx][ny] == 0 && !visited_bfs[nx][ny]) 
            {
                visited_bfs[nx][ny] = true;
                Distance[nx][ny] = Distance[p.x][p.y] + 1;
                q.push({ nx, ny });
            }
        }
    }

}

void printMaze() //지도띄우기
{
    lock_guard<mutex> guard(mtx); // 뮤텍스를 사용하여 이 함수의 실행을 동기화
    system("cls"); // 콘솔 창을 클리어
   
    cout  << " 사용자의 현재 위치 x : " << playerX << " y : " << playerY << endl <<
        "몬스터의 현재 위치 x : " << MonsterX << " y : " << MonsterY << endl;
    for (int i = 0; i < Size; ++i) {
        for (int j = 0; j < Size; ++j) {
            if ((i == MonsterX && j == MonsterY) && Monster_start_count <= 0)
            {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);//색을 빨간색으로 변경 
                cout << "M "; // 플레이어의 현재 위치를 P로 표시
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);//색을 흰색으로 변경
            }
            else if (i == playerX && j == playerY)
            {
                 SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);//색을 빨간색으로 변경 
                 cout << "P "; // 플레이어의 현재 위치를 P로 표시
                 SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);//색을 흰색으로 변경
               
            }

            else if (maze[i][j] == 0)
                cout << "□ ";
            else
                cout << "■ ";




              //  cout << maze[i][j] << " ";

        }
        cout << "\n";
    }


}

void Monster_trace() //몬스터가 사용자를 향해 가기위한 bfs
{
    struct Point {
        int x, y;
    };

    int dx[4] = { 0, 1, 0, -1 };
    int dy[4] = { 1, 0, -1, 0 };
    memset(visited_bfs, false, sizeof(visited_bfs));
    queue<Point> q;
    q.push({ playerX, playerY });
    visited_bfs[playerX][playerY] = true;

    while (!q.empty()) {
        Point p = q.front();
        q.pop();
        for (int i = 0; i < 4; ++i) {
            int nx = p.x + dx[i];
            int ny = p.y + dy[i];

            if (nx >= 0 && nx < Size && ny >= 0 && ny < Size && maze[nx][ny] == 0 && !visited_bfs[nx][ny])
            {
                if (nx == MonsterX && ny == MonsterY)
                { 
                    MonsterX = p.x;
                    MonsterY = p.y;
                    return;
                }
                
                visited_bfs[nx][ny] = true;
                q.push({ nx, ny });
            }
        }
    }

}

void MonsterMovement() { // 몬스터 이동 관리 함수
    
    while (Monster_start_count != 0)
    { 
        MonsterX = -1, MonsterY = -1;
    }
    MonsterX = 0;
    MonsterY = 0;
    while ((playerX != Size - 1 || playerY != Size - 1) && !(playerX == MonsterX && playerY == MonsterY)) {
        if (Monster_start_count <= 0) {

            Monster_trace(); // 몬스터 추적 로직 실행
            printMaze(); // 몬스터 이동 후 미로 상태 출력
        } if (playerX == MonsterX && playerY == MonsterY)
        {
            cout << "---- 미로 탈출 실패 ... ----" << endl << endl << "https://velog.io/@270pp13/posts";
            exit(0);
        }

        Sleep(1000); // 1초 대기
    }
   


}




void movePlayer() // 키입력받고 위치 이동시키기
{

  
    char key = _getch(); // 사용자 입력 받기
     if (playerX == MonsterX && playerY == MonsterY)
        return;
    Monster_start_count--;
    switch (key) {
    case 'w': // 위로 이동
        if (playerX > 0 && maze[playerX - 1][playerY] == 0)
            playerX--;
        break;
    case 's': // 아래로 이동
        if (playerX < Size - 1 && maze[playerX + 1][playerY] == 0)
            playerX++;
        break;
    case 'a': // 왼쪽으로 이동
        if (playerY > 0 && maze[playerX][playerY - 1] == 0)
            playerY--;
        break;
    case 'd': // 오른쪽으로 이동
        if (playerY < Size - 1 && maze[playerX][playerY + 1] == 0)
            playerY++;
        break;
    }


}

 /*void printMaze() //미로 확인 함수
{
    for (int i = 0; i < Size; ++i) {
        for (int j = 0; j < Size; ++j) {
            cout << maze[i][j] << " ";
        }
        cout << "\n";
    }
}
void printMaze2() //미로 확인 함수
{
    cout << "\n"; cout << "\n"; cout << "\n"; cout << "\n";
    for (int i = 0; i < Size; ++i) {
        for (int j = 0; j < Size; ++j) {
            cout << Distance[i][j] << " ";
        }
        cout << "\n";
    }
}*/



int main() {
    srand(time(0)); // 랜덤 시드 초기화
    system("title 미로 탈출하기");


    do {
        initializeMaze();
        dfs(0, 0); // DFS를 이용하여 시작에서 끝까지의 경로가 있는지 확인
    } while (!complete); // complete가 true가 될 때까지 반복

    //bfs();
    thread monsterThread(MonsterMovement);   // 몬스터 이동을 관리하는 스레드 생성 및 시작

    while ((playerX != Size - 1 || playerY != Size - 1) && !( playerX == MonsterX && playerY == MonsterY))
    {
        ct++;
        printMaze();
        if (playerX == MonsterX && playerY == MonsterY)
        {
            
            break;
        }
        movePlayer();
    }

    monsterThread.detach(); //종료 대기
    printMaze();
   


    if(playerX == MonsterX && playerY == MonsterY)
        cout << "---- 미로 탈출 실패 ... ----" << endl << endl << "https://velog.io/@270pp13/posts";
    else
        cout << "---- 미로 탈출 완료 ! ----" << endl<<endl<<"https://velog.io/@270pp13/posts";

    return 0;
}