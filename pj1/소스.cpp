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
int maze[Size][Size];     // �̷���
bool visited_dfs[Size][Size]; // �� ĭ�� �湮���θ� ����(dfs)
bool visited_bfs[Size][Size]; // �� ĭ�� �湮���θ� ����(bfs)
int Distance[Size][Size]; // �� ĭ������ �ִ� �Ÿ��� ����
bool complete = false;   //�������� �̷����� �� ����
int playerX = 0, playerY = 0; // �÷��̾��� ���� ��ġ
int MonsterX = -1, MonsterY = -1, Monster_start_count=10; //������ ���� ��ġ
int ct = 0;

mutex mtx;

void initializeMaze() //�̷� ���� �Լ�
{
    complete = false;
    memset(visited_dfs, false, sizeof(visited_dfs)); // visited �迭�� �ʱ�ȭ

    //0�� �� 1�� ����
    maze[0][0] = 0; //������ �ݵ�� ���̾����.
    maze[9][9] = 0; //������ ���� �ݵ�� ���̾����.

    // ������ ������ �������� 0 �Ǵ� 1�� ����
    for (int i = 0; i < Size; ++i) {
        for (int j = 0; j < Size; ++j) {
            if (!((i == 0 && j == 0) || (i == Size - 1 && j == Size - 1))) {
                maze[i][j] = rand() % 2;
            }
        }
    }
}


bool dfs(int x, int y) //������ ���ۺ��� ������ ����Ǿ����� Ȯ���ϱ� ���� dfs
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

void bfs() //������ �ִܰŸ��� �����ϱ� ���� bfs
{
    struct Point {
        int x, y;
    };

    int dx[4] = { 0, 1, 0, -1 };
    int dy[4] = { 1, 0, -1, 0 };

    queue<Point> q;
    q.push({ 0, 0 });
    visited_bfs[0][0] = true;
    Distance[0][0] = 1; // �������� 0��

    while (!q.empty()) {
        Point p = q.front();
        q.pop();

        if (p.x == Size - 1 && p.y == Size - 1)
            break; // �������� ����
        

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

void printMaze() //��������
{
    lock_guard<mutex> guard(mtx); // ���ؽ��� ����Ͽ� �� �Լ��� ������ ����ȭ
    system("cls"); // �ܼ� â�� Ŭ����
   
    cout  << " ������� ���� ��ġ x : " << playerX << " y : " << playerY << endl <<
        "������ ���� ��ġ x : " << MonsterX << " y : " << MonsterY << endl;
    for (int i = 0; i < Size; ++i) {
        for (int j = 0; j < Size; ++j) {
            if ((i == MonsterX && j == MonsterY) && Monster_start_count <= 0)
            {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);//���� ���������� ���� 
                cout << "M "; // �÷��̾��� ���� ��ġ�� P�� ǥ��
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);//���� ������� ����
            }
            else if (i == playerX && j == playerY)
            {
                 SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);//���� ���������� ���� 
                 cout << "P "; // �÷��̾��� ���� ��ġ�� P�� ǥ��
                 SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);//���� ������� ����
               
            }

            else if (maze[i][j] == 0)
                cout << "�� ";
            else
                cout << "�� ";




              //  cout << maze[i][j] << " ";

        }
        cout << "\n";
    }


}

void Monster_trace() //���Ͱ� ����ڸ� ���� �������� bfs
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

void MonsterMovement() { // ���� �̵� ���� �Լ�
    
    while (Monster_start_count != 0)
    { 
        MonsterX = -1, MonsterY = -1;
    }
    MonsterX = 0;
    MonsterY = 0;
    while ((playerX != Size - 1 || playerY != Size - 1) && !(playerX == MonsterX && playerY == MonsterY)) {
        if (Monster_start_count <= 0) {

            Monster_trace(); // ���� ���� ���� ����
            printMaze(); // ���� �̵� �� �̷� ���� ���
        } if (playerX == MonsterX && playerY == MonsterY)
        {
            cout << "---- �̷� Ż�� ���� ... ----" << endl << endl << "https://velog.io/@270pp13/posts";
            exit(0);
        }

        Sleep(1000); // 1�� ���
    }
   


}




void movePlayer() // Ű�Է¹ް� ��ġ �̵���Ű��
{

  
    char key = _getch(); // ����� �Է� �ޱ�
     if (playerX == MonsterX && playerY == MonsterY)
        return;
    Monster_start_count--;
    switch (key) {
    case 'w': // ���� �̵�
        if (playerX > 0 && maze[playerX - 1][playerY] == 0)
            playerX--;
        break;
    case 's': // �Ʒ��� �̵�
        if (playerX < Size - 1 && maze[playerX + 1][playerY] == 0)
            playerX++;
        break;
    case 'a': // �������� �̵�
        if (playerY > 0 && maze[playerX][playerY - 1] == 0)
            playerY--;
        break;
    case 'd': // ���������� �̵�
        if (playerY < Size - 1 && maze[playerX][playerY + 1] == 0)
            playerY++;
        break;
    }


}

 /*void printMaze() //�̷� Ȯ�� �Լ�
{
    for (int i = 0; i < Size; ++i) {
        for (int j = 0; j < Size; ++j) {
            cout << maze[i][j] << " ";
        }
        cout << "\n";
    }
}
void printMaze2() //�̷� Ȯ�� �Լ�
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
    srand(time(0)); // ���� �õ� �ʱ�ȭ
    system("title �̷� Ż���ϱ�");


    do {
        initializeMaze();
        dfs(0, 0); // DFS�� �̿��Ͽ� ���ۿ��� �������� ��ΰ� �ִ��� Ȯ��
    } while (!complete); // complete�� true�� �� ������ �ݺ�

    //bfs();
    thread monsterThread(MonsterMovement);   // ���� �̵��� �����ϴ� ������ ���� �� ����

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

    monsterThread.detach(); //���� ���
    printMaze();
   


    if(playerX == MonsterX && playerY == MonsterY)
        cout << "---- �̷� Ż�� ���� ... ----" << endl << endl << "https://velog.io/@270pp13/posts";
    else
        cout << "---- �̷� Ż�� �Ϸ� ! ----" << endl<<endl<<"https://velog.io/@270pp13/posts";

    return 0;
}