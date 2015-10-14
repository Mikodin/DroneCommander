#include <string.h>
#include <stdio.h>
#define GRID_SIZE 20
void boardPrint();

char board[GRID_SIZE][GRID_SIZE];

typedef struct _drone {
	int x;
	int y;
	char c;
	int cruising; //Being treated as a bool
}DRONE;

void returnHome(DRONE*);
int droneCheckDir(DRONE*,char);
void droneMoveDir(DRONE*,char);

void boardInit(){
	int col = 0;
	int row = 0;
        for(col = 0; col < GRID_SIZE; col++) {
                for(row = 0; row < GRID_SIZE; row++){
                        board[col][row] = ' ';
			if(row == 0)board[col][row] = 'x';
			if(col == 0 )board[col][row] = 'x';
			if(row == GRID_SIZE-1)board[col][row] = 'x';
			if(col  == GRID_SIZE-1)board[col][row] = 'x';
                }
        }
}

void boardInsert(int x,int y,char ch) {
	board[y][x] = ch;
//	boardPrint();  in some cases it might be better to have it redraw on every insert but we shall see how I feel about that later
}

void boardPrint() {
	printf("\033[2J\033[1;1H");
        int i = 0;
        int j = 0;

        for(i =0; i < GRID_SIZE; i++) {
                if(i > 0)printf("\n");
                for(j = 0; j < GRID_SIZE; j++){
                       printf("%c",board[i][j]);
                }
        }
	printf("\n");
}

int droneInit(DRONE *d){
	d->x = 0;
	d->y= 0;
	d->c = '8';
	d->cruising = 0;
}

int  droneCheckDir(DRONE *d, char dir) {
	printf("Checking DIR: %c\n",dir);
	if (dir == 'N') {
		printf("Checking DIR:N %c\n",board[d->y-1][d->x]);
		if(board[d->y-1][d->x] == 'x')return 0;
		else if(board[d->y-1][d->x] == ' ')return 0;
		else return 1;
	}

	else if (dir == 'S'){
		printf("Checking DIR:S %c\n",board[d->y+1][d->x]);
		if(board[d->y+1][d->x] == 'x')return 0;
		else if(board[d->y+1][d->x] == ' ')return 0;
		else return 1;
	}

	else if (dir == 'E'){
		printf("Checking DIR: E %c\n",board[d->y][d->x+1]);
		if(board[d->y][d->x+1] == 'x')return 0;
		else if(board[d->y][d->x+1] == ' ')return 0;
		else return 1;
	}
	
	else if (dir == 'W'){
		printf("Checking DIR: W %C\n",board[d->y][d->x+1]);
		if(board[d->y][d->x-1] == 'x')return 0;
		else if(board[d->y][d->x-1] == ' ')return 0;
		else return 1;
	}
}

void droneMoveDir(DRONE *d,char dir) {
	if(d->x >= 0 && d->y == 0)boardInsert(d->x,d->y,'x');
	else if(d->y == GRID_SIZE-1 && d->x > 0)boardInsert(d->x,d->y,'x');
	else if (d->y > 0 && d->x == 0)boardInsert(d->x,d->y,'x');
	else if (d->y > 0 && d->x == GRID_SIZE-1)boardInsert(d->x,d->y,'x');
	else boardInsert(d->x,d->y,' ');
	
	if (dir == 'N') {
		if (droneCheckDir(d,'N')== 0){ 
		printf("Moving North\n");
			d->y --;
			boardInsert(d->x,d->y,d->c);
			sleep(1);
		}	
		else if (droneCheckDir(d,'W') == 0) {
			droneMoveDir(d,'W');
			droneMoveDir(d,'N');
			
		}
		else if (droneCheckDir(d,'E') == 0) {
			droneMoveDir(d,'W');
			droneMoveDir(d,'N');
		}	
		else if (droneCheckDir(d,'S') == 0) {
			droneMoveDir(d,'S');
			droneMoveDir(d,'N');
		}	
	}

	if (dir == 'S'){
		if (droneCheckDir(d,'S')== 0){ 
		printf("Moving South\n");
			d->y ++;
			boardInsert(d->x,d->y,d->c);
			sleep(1);
		}	
		else if (droneCheckDir(d,'E') == 0) {
			droneMoveDir(d,'E');
			droneMoveDir(d,'S');
		}
		else if (droneCheckDir(d,'W') == 0) {
			droneMoveDir(d,'W');
			droneMoveDir(d,'S');
		}	
		else if (droneCheckDir(d,'N') == 0) {
			droneMoveDir(d,'N');
			droneMoveDir(d,'S');
		}	
	}

	if (dir == 'E'){
		if (droneCheckDir(d,'E')== 0){ 
		printf("Moving East\n");
			d->x ++;
			boardInsert(d->x,d->y,d->c);
			sleep(1);
		}
		else if (droneCheckDir(d,'S') == 0) {
			droneMoveDir(d,'S');
			droneMoveDir(d,'E');
		}
		else if (droneCheckDir(d,'N') == 0) {
			droneMoveDir(d,'N');
			droneMoveDir(d,'E');
		}	
		else if (droneCheckDir(d,'W') == 0) {
			droneMoveDir(d,'W');
			droneMoveDir(d,'E');
		}	

	}
	
	if (dir == 'W'){
		if (droneCheckDir(d,'W')== 0){ 
		printf("Moving West\n");
			d->x --;
			boardInsert(d->x,d->y,d->c);
			sleep(1);
		}	
		else if (droneCheckDir(d,'S') == 0) {
			droneMoveDir(d,'S');
			droneMoveDir(d,'W');
		}
		else if (droneCheckDir(d,'N') == 0) {
			droneMoveDir(d,'N');
			droneMoveDir(d,'W');
		}	
		else if (droneCheckDir(d,'E') == 0) {
			droneMoveDir(d,'E');
			droneMoveDir(d,'W');
		}	
	}
		boardPrint();
}

void droneCruiseTo(DRONE *d,int x, int y) {
	if(x != 0 && y != 0) boardInsert(x,y,'x');
	d->cruising = 1;
	while(d->cruising == 1) {
		if(x > d->x){
			droneMoveDir(d,'E');
		}
		else if(y < d->y){
			droneMoveDir(d,'N');
		}
		else if(x < d->x){
			droneMoveDir(d,'W');
		}	
		else if(y > d->y){
			droneMoveDir(d,'S');
		}
		
		else if(d->y != 0 && d->x != 0) returnHome(d);
		else d->cruising = 0;
	}		
}

void returnHome(DRONE *d) {
	droneCruiseTo(d,0,0);
}


int main() {
	boardInit();
	boardInsert(4,3,'T');	
	boardInsert(5,3,'T');	
	boardInsert(6,3,'T');	
	boardInsert(6,4,'T');	
	boardInsert(4,4,'T');	
	boardInsert(6,5,'T');	
	boardPrint();
	DRONE d;
	droneInit(&d);
	droneCruiseTo(&d, 5,4);
	return 0;
}

