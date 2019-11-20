#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <windows.h>
#include <ctype.h>


/*확장 아스키 사용 특수키의 경우는 2BYTE의 크기를 가진다. getch함수를 두번 호출하여 그 값을 1byte씩 읽어들인다. 
처음에 하위 1byte를 읽고 두번째 상위 1byte를 읽는다. 2byte 크기의 아스키 코드값의 구조는 하위 1byte이 코드값이 0이다.*/
#define LEFT 75 //방향키 왼쪽 0 75 
#define RIGHT 77 //방향키 오른쪽 0 77
#define UP 72 //방향키 위 0 72
#define DOWN 80 //방향키 아래 0 80
#define ESC 27 //esc 0 27

//
#define BX 5  
#define BY 1 // BX,BY는 매크로 게임판의 좌상단 좌표이다.
#define BW 10 
#define BH 20 // BW,BH는 각각 게임판의 폭과 높이 값이다.(외부 벽 제외 값)
// 위에 4줄을 통하여 매크로 게임판은 5,1에서 시작하여 10의 폭과 20의 높이를 가진다.

#define clrscr() system("cls") //화면을 지우는 함수를 쉽게 쓰기 위하여 system("cls");함수를 clrscr();함수로 치환하였다.
#define randomize() srand((unsigned int)time(NULL))// 난수의 시드 값을 바꾸는 과정을 간단히 하기 위하여 srand((unsigned int)time(NULL))을 randomize()로 치환시켰다.
#define random(min, max) ( (rand() % ((max) - (min) + 1) ) + (min)) //범위 지정을 통한 난수 발생을 위한 함수


void DrawScreen(); // 화면 전체를 그리는 함수
void DrawBoard(); //벽돌을 그리는 함수
BOOL ProcessKey(); //키 입력을 처리하며 벽돌이 바닥에 닿았을 때 TRUE반환
void PrintBrick(BOOL Show);// 이동중인 벽돌을 삭제, 출력하는 함수 전역변수 brick,rot,nx,ny를 참조
int GetAround(int x, int y, int b, int r); // 벽돌 주변을 검사하여 벽돌이 이동 및 회전가능성을 조사하는 함수
BOOL MoveDown();  //벽돌을 한칸 아래로 이동시킨다. 벽돌이 바닥에 닿는다면 TESTFULL함수 호출 후 TRUE를 리턴
void TestFull(); //수평을 다 채워진 줄을 찾아 삭제
void setcursortype(CURSORTYPE_TYPE);// 커서의 형태를 지정해 주는 함수 이 프로그램에서는 커서를 안보이게 한다.
void gotoxy(int x, int y); // 커서의  위치 이동을 위한 함수
void delay(int x); //Sleep함수와 똑같고 가시성을 위하여 정의, 프로그램의 진행을 지연시키는 함수




typedef enum { NOCURSOR, SOLIDCURSOR, NORMALCURSOR } CURSOR_TYPE;
typedef struct point {
	int x, y;
}Point;//
Point Shape[7][4][4] = {
	 {  { {0,0},{1,0},{2,0},{-1,0}   }, { {0,0},{0,1},{0,-1},{0,-2}  }, { {0,0},{1,0},{2,0},{-1,0}   }, { {0,0},{0,1},{0,-1},{0,-2}  } },
		/*[0][0][0] ~ [0][0][3]			[0][1][0] ~ [0][1][3]				[0][2][0] ~ [0][2][3]		   [0][3][0] ~ [0][3][3]		*/
	 {  { {0,0},{1,0},{0,1},{1,1}    }, { {0,0},{1,0},{0,1},{1,1}    }, { {0,0},{1,0},{0,1},{1,1}    }, { {0,0},{1,0},{0,1}, {1,1}   } },
		/*[1][0][0] ~ [1][0][3]			[1][1][0] ~ [1][1][3]				[1][2][0] ~ [1][2][3]		   [1][3][0] ~ [1][3][3]		*/				
	 {  { {0,0},{-1,0},{0,-1},{1,-1} }, { {0,0},{0,1},{-1,0},{-1,-1} }, { {0,0},{-1,0},{0,-1},{1,-1} }, { {0,0},{0,1},{-1,0},{-1,-1} } },
		/*[2][0][0]	~ [2][0][3]			[2][1][0] ~ [2][1][3]			[2][2][0] ~ [2][2][3]				[2][3][0] ~ [2][3][3]		*/																				
	 {  { {0,0},{-1,-1},{0,-1},{1,0}  },{ {0,0},{-1,0},{-1,1},{0,-1} }, { {0,0},{-1,-1},{0,-1},{1,0} }, { {0,0},{-1,0},{-1,1},{0,-1} } },
		/*[3][0][0] ~ [3][0][3]			[3][1][0] ~ [3][1][3]			[3][2][0] ~ [3][2][3]				[3][3][0] ~ [3][3][3]		*/
	 {  { {0,0},{-1,0},{1,0},{-1,-1} }, { {0,0},{0,-1},{0,1},{-1,1}  }, { {0,0},{-1,0},{1,0},{1,1}   }, { {0,0},{0,-1},{0,1},{1,-1}  } },
		/*[4][0][0] ~ [4][0][3]			[4][1][0] ~ [4][1][3]			[4][2][0] ~ [4][2][3]				[4][3][0] ~ [4][3][3]		*/					
	 {  { {0,0},{1,0},{-1,0},{1,-1}  }, { {0,0},{0,1},{0,-1},{-1,-1} }, { {0,0},{1,0},{-1,0},{-1,1}  }, { {0,0},{0,-1},{0,1},{1,1}   } },
		/*[5][0][1] ~ [5][0][3]			[5][1][0] ~ [5][1][3]			[5][2][0] ~ [5][2][3]				[5][3][0] ~ [5][3][3]		*/
	 {  { {0,0},{-1,0},{1,0},{0,1}   }, { {0,0},{0,-1},{0,1},{1,0}   }, { {0,0},{-1,0},{1,0},{0,-1}  }, { {0,0},{-1,0},{0,-1},{0,1}  } },
		/*[6][0][1] ~ [6][0][3]			[6][1][0] ~ [6][1][3]			[6][2][0] ~ [6][2][3]				[6][3][0] ~ [6][3][3]		*/
};

enum { EMPTY, BRICK, WALL }; // 각각 0,1,2로 게임판의 상태를 표시, arTile배열을 쉽게 사용하기 위한 열거형 
char* arTile[] = { ". ","■","□" }; // 공백과 벽, 벽돌을 그리기 위해 필요한 문자형 포인터 변수
int board[BW + 2][BH + 2]; // 전체 게임판의 상태에 쓰이는 배열로써 BW,BH는 각각 외부 벽을 제외한 값이라 +2를 해주어 외부벽 좌표까지 넓힘
int nx, ny; // 이동중인 벽돌의 현재좌표 
int brick, rot; // 이동중인 벽돌의 식별번호이자 회전값

int main()
{
	
	int nFrame, nStay;
	int x, y;

	setcursortype(NOCURSOR); // 커서 지우기
	randomize(); // 난수 시드 값 할당
	clrscr();// 화면 지우기
	for (x = 0; x < BW + 2; x++) {
		for (y = 0; y < BH + 2; y++) {
			board[x][y] = (y == 0 || y == BH + 1 || x == 0 || x == BW + 1) ? WALL : EMPTY;
		}
	}
	DrawScreen();
	nFrame = 20;

	for (; 1;) {
		brick = random(0, 6);
		nx = BW / 2;
		ny = 3;
		rot = 0;
		PrintBrick(TRUE);

		if (GetAround(nx, ny, brick, rot) != EMPTY) break;
		nStay = nFrame;
		for (; 2;) {
			if (--nStay == 0) {
				nStay = nFrame;
				if (MoveDown()) break;
			}
			if (ProcessKey()) break;
			delay(1000 / 20);
		}
	}
	clrscr();
	gotoxy(30, 12); puts("G A M E  O V E R");
	setcursortype(NORMALCURSOR);
	delay(5);

	
	return 0;
}

void setcursortype(CURSORTYPE_TYPE)
{

	CONSOLE_CURSOR_INFO CurInfo;

	switch (CURSORTYPE_TYPE) {
	case NOCURSOR:
		CurInfo.dwSize = 1;
		CurInfo.bVisible = FALSE;
		break;
	case SOLIDCURSOR:
		CurInfo.dwSize = 100;
		CurInfo.bVisible = TRUE;
		break;
	case NORMALCURSOR:
		CurInfo.dwSize = 20;
		CurInfo.bVisible = TRUE;
		break;
	}
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &CurInfo);
}

void DrawScreen()
{
	int x, y;

	for (x = 0; x < BW + 2; x++) {
		for (y = 0; y < BH + 2; y++) {
			gotoxy(BX + x * 2, BY + y);
			puts(arTile[board[x][y]]);
		}
	}

	gotoxy(50, 3); puts("권택준,황상천");
	gotoxy(50, 5); puts("좌우:이동, 위:회전, 아래:내림");
	gotoxy(50, 6); puts("스페이스바 : hard drop");
}

void DrawBoard()
{
	int x, y;

	for (x = 1; x < BW + 1; x++) {
		for (y = 1; y < BH + 1; y++) {
			gotoxy(BX + x * 2, BY + y);
			puts(arTile[board[x][y]]);
		}
	}
}

BOOL ProcessKey()
{
	int ch, trot;

	if (_kbhit()) {
		ch = _getch();
		if (ch == 0xE0 || ch == 0) {
			ch = _getch();
			switch (ch) {
			case LEFT:
				if (GetAround(nx - 1, ny, brick, rot) == EMPTY) {
					PrintBrick(FALSE);
					nx--;
					PrintBrick(TRUE);
				}
				break;
			case RIGHT:
				if (GetAround(nx + 1, ny, brick, rot) == EMPTY) {
					PrintBrick(FALSE);
					nx++;
					PrintBrick(TRUE);
				}
				break;
			case UP:
				trot = (rot == 3 ? 0 : rot + 1);
				if (GetAround(nx, ny, brick, trot) == EMPTY) {
					PrintBrick(FALSE);
					rot = trot;
					PrintBrick(TRUE);
				}
				break;
			case DOWN:
				if (MoveDown()) {
					return TRUE;
				}
				break;
			}
		}
		else {
			switch (ch) {
			case ' ':
				while (MoveDown() == FALSE) { ; }
				return TRUE;
			}
		}
	}
	return FALSE;
}

void PrintBrick(BOOL Show)
{
	int i;

	for (i = 0; i < 4; i++) {
		gotoxy(BX + (Shape[brick][rot][i].x + nx) * 2, BY + Shape[brick][rot][i].y + ny);
		puts(arTile[Show ? BRICK : EMPTY]);
	}
}

int GetAround(int x, int y, int b, int r)
{
	int i, k = EMPTY;

	for (i = 0; i < 4; i++) {
		k = max(k, board[x + Shape[b][r][i].x][y + Shape[b][r][i].y]);
	}
	return k;
}

BOOL MoveDown()
{
	if (GetAround(nx, ny + 1, brick, rot) != EMPTY) {
		TestFull();
		return TRUE;
	}
	PrintBrick(FALSE);
	ny++;
	PrintBrick(TRUE);
	return FALSE;
}

void TestFull()
{
	int i, x, y, ty;

	for (i = 0; i < 4; i++) {
		board[nx + Shape[brick][rot][i].x][ny + Shape[brick][rot][i].y] = BRICK;
	}

	for (y = 1; y < BH + 1; y++) {
		for (x = 1; x < BW + 1; x++) {
			if (board[x][y] != BRICK) break;
		}
		if (x == BW + 1) {
			for (ty = y; ty > 1; ty--) {
				for (x = 1; x < BW + 1; x++) {
					board[x][ty] = board[x][ty - 1];
				}
			}
			DrawBoard();
			delay(200);
		}
	}
}

void gotoxy(int x, int y)
{
	COORD Pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);

}
void delay(int x) {
	Sleep(x);
}
