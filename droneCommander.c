#include <string.h>
#include <stdio.h>

void boardInit();
void boardInsert(int,int,char);
void boardPrint();

const int cols = 20;
const int rows = 30;
char board[20][30];

	int cool= 0;
	int roow=0;
int main() {
	boardInit();
	boardPrint();
	scanf("%d",&roow);
	scanf("%d",&cool);
	boardInsert(roow,cool,'$');
	return 0;
}


void boardInit(){

	int col = 0;
	int row = 0;
        for(col = 0; col < cols; col++) {
                for(row = 0; row < rows; row++){
                        board[col][row] = '_';
			if(row == 0)board[col][row] = 'x';
			if(col == 0 )board[col][row] = 'x';
			if(row == rows-1)board[col][row] = 'x';
			if(col  == cols - 1)board[col][row] = 'x';
                        //if(i == 0)board[i][j] ='x';
                }
        }
}
void boardInsert(int x,int y,char ch) {
	board[y][x] = ch;
	boardPrint();
}

void boardPrint() {
        int i = 0;
        int j = 0;

        for(i =0; i < cols; i++) {
                if(i > 0)printf("\n");
                for(j = 0; j < rows; j++){
                       printf("%c",board[i][j]);
                }
        }
	printf("\n");
}
