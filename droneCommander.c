//Michael Alicea

#include <string.h>
#include <stdio.h>
#include <pthread.h>

#define GRID_SIZE 20
#define NUM_THREADS 10

pthread_mutex_t lock;
pthread_t droneThreads[NUM_THREADS];
pthread_t boardThread;

int running = 1;
char board[GRID_SIZE][GRID_SIZE];

void *boardPrint(void*);
/*
	Pretty self explanatory but one note is the cruising and package delivered
		treated as boolean values.
	Cruising is set to 1 when the drone is cruising to a destination
	PackageDelivered is set to 1 when well, the package is delivered.
*/
typedef struct _drone {
	int x;
	int y;
	int destX; 
	int destY;
	char c;
	int cruising; 
	int packageDelivered; 
}DRONE;

void droneReturnHome(DRONE*);
void droneHome(DRONE*);
int droneCheckDir(DRONE*,char);
void droneMoveDir(DRONE*,char);

/*
	Initializes the board through a double for loop traversing the matrix
	memset() is called to clear a space in memory, to ensure no erraneous values
*/
void boardInit(){
	memset(board,0,sizeof(board));
	
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

/*
	Again self explanatory
	Calls pthread_mutex_lock(&lock) to ensure only 1 thread can insert at a time
	
	You may realize that it accesses the matrix sort of weird
	I did this to turn it into X,Y coordinates instead of y,x (col,row)

*/
void boardInsert(int x,int y,char ch) {
	pthread_mutex_lock(&lock);
	board[y][x] = ch; 
	pthread_mutex_unlock(&lock);
}

/*
	Calls pthread_mutex_lock(&lock) to ensure only 1 thread can read at a time
	You may realize that it accesses the matrix sort of weird
	I did this to turn it into X,Y coordinates instead of y,x (col,row)
	
*/
char boardGetPos(int x,int y) {
	pthread_mutex_lock(&lock);
	char c = board[y][x];
	pthread_mutex_unlock(&lock);
	return c;
}

/*
	Check if "running" is flagged to 1 (true), this is set in the main function
	then just traverses the matrix printing chars
	No mutex lock on this because
		1: It's only being accessed by the boardThread
		2: If the mutex lock is there then drone will move a few "frames"
			without it being drawn! 
*/
void *boardPrint(void *arg) {
//	pthread_mutex_lock(&lock);
	while( running == 1){
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
//	pthread_mutex_unlock(&lock);
	sleep(1);
	}
}
/*
	Only reason this is here is so that you can see where you set a new "job"
		in the main method when you declare where each drone is going
*/
void boardPrintOnce() {
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

/*Initializes the drones starting coordinates, desination and character*/
void *droneInit(DRONE *d,int dX,int dY,int dChar){
	d->x = 0;
	d->y= 0;
	d->destX = dX;
	d->destY = dY;
	d->c = dChar;
	d->cruising = 0;
	d->packageDelivered = 0;
	boardInsert(dX,dY,'x');
}

/*
	Returns 0 if there is nothing in the way of the drone
	Retruns 1 if there is indeed something in the way of the drone
	Takes in a drone object and the char direction that it should check
	The rest should be self explanatory, checks ahead of the drone on the board
		to see what is there.
*/
int  droneCheckDir(DRONE *d, char dir) {
	if (dir == 'N') {
		if(d->y <= 0)return 1;	
		else if(boardGetPos(d->x,d->y-1) == 'x') return 0;
		else if(boardGetPos(d->x,d->y-1) == ' ') return 0;
		else {
			printf("%c avoiding collision with %c\n",
					d->c,boardGetPos(d->x,d->y-1));
			return 1;
		}	
	}

	else if (dir == 'S'){
		if(d->y >= GRID_SIZE-1)return 1;
		else if(boardGetPos(d->x,d->y+1) == 'x') return 0;
		else if(boardGetPos(d->x,d->y+1) == ' ') return 0;	
		else {
			printf("%c avoiding collision with %c\n",
					d->c,boardGetPos(d->x,d->y+1));
			return 1;
		}
	}

	else if (dir == 'E'){
		if(d->x >= GRID_SIZE-1)return 1;	
		else if(boardGetPos(d->x+1,d->y) == 'x') return 0;
		else if(boardGetPos(d->x+1,d->y) == ' ') return 0;	
		else {
			printf("%c avoiding collision with %c\n",
					d->c,boardGetPos(d->x+1,d->y));
			return 1;
		}
	}
	
	else if (dir == 'W'){
		if(d->x <= 0)return 1;	
		else if(boardGetPos(d->x-1,d->y) == 'x') return 0;
		else if(boardGetPos(d->x-1,d->y) == ' ') return 0;	
		else {
			printf("%c avoiding collision with %c\n",
					d->c,boardGetPos(d->x-1,d->y));
			return 1;
		}
	}
}

/*
	Takes in a drone and the char for the direction
	The first set of if and else if statements do the following:
		Checks if the drone is on the border, if it is
			re-add the x char when the drone has moved
		Checks if the drone is inside the board, if it is
			re-add the ' ' (empty space) char when the drone has moved

	The second set of if else statements is the meat and potatoes of this baby
		Relatively self explanatory
		Pretty much if you are trying to go North you pass the function
			the drone and the N char, then it will check the direction N
			If N is clear then it will first call pthread_yield() to
			see if other drones are currently moving and wait for them
			then it will decrement it's y position and insert that into the
			board

			If N is not clear then it will check the next best choice
				If that way is clear it will recursively call itself
				and move in that direction THEN it will try again to move
				in the intended direction!
		Calls sleep for 1 ms after all has been said and done. 
*/
void droneMoveDir(DRONE *d,char dir) {
	if(d->x >= 0 && d->y == 0)boardInsert(d->x,d->y,'x');
	else if(d->y == GRID_SIZE-1 && d->x > 0)boardInsert(d->x,d->y,'x');
	else if (d->y > 0 && d->x == 0)boardInsert(d->x,d->y,'x');
	else if (d->y > 0 && d->x == GRID_SIZE-1)boardInsert(d->x,d->y,'x');
	else boardInsert(d->x,d->y,' ');
	
	if (dir == 'N') {
		if (droneCheckDir(d,'N')== 0){ 
			pthread_yield();
			d->y --;	
			boardInsert(d->x,d->y,d->c);
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
			pthread_yield();
			d->y ++;
			boardInsert(d->x,d->y,d->c);
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
			pthread_yield();
			d->x ++;
			boardInsert(d->x,d->y,d->c);
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
			pthread_yield();	
			d->x --;
			boardInsert(d->x,d->y,d->c);
		}	
		else if (droneCheckDir(d,'N') == 0) {
			droneMoveDir(d,'N');
			droneMoveDir(d,'W');
		}	
		else if (droneCheckDir(d,'S') == 0) {
			droneMoveDir(d,'S');
			droneMoveDir(d,'W');
		}
		else if (droneCheckDir(d,'E') == 0) {
			droneMoveDir(d,'E');
			droneMoveDir(d,'W');
		}	
	}
	sleep(1);
}
/*
	This inititiates the 
*/
void *droneCruiseTo(void *dr) {
	DRONE *d = (DRONE*) dr;
	int x = d->destX;
	int y = d->destY;
	d->cruising = 1;
	while(d->cruising == 1) {
		if(d->packageDelivered == 1 && d->x == 0 && d->y == 0)droneHome(d);
	
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
		
		else if(d->y > 0 && d->x > 0){
			droneReturnHome(d); 
		}
	}
}

void droneReturnHome(DRONE *d) {
	d->destX = 0;
	d->destY = 0;
	d->packageDelivered = 1;
	droneCruiseTo(d);
}

void droneHome(DRONE *d) {
	d->c = 'x';
	boardInsert(0,0,'x');
	d->cruising = 0;
}

int main() {
	int rc;
	int dc;
	int i;
	
	DRONE d[NUM_THREADS];
	pthread_mutex_init(&lock,NULL);
	
	boardInit();
       /* for(i = 0; i < NUM_THREADS; i++){
                int dX, dY;
                printf("Set Drone#%d's X position\n",i);
                scanf("%d",&dX);
                printf("Set Drone#%d's y position\n",i);
                scanf("%d",&dY);
                droneInit(&d[i],dX,dY,'0'+i);
                boardInsert(dX,dY,'x');
                boardPrintOnce();
        }*/

	rc = pthread_create(&boardThread,NULL,boardPrint,(void*) 1);		
	boardInsert(5,10,'T');
	boardInsert(8,4,'T');
	boardInsert(12,8,'T');
//	boardInsert(15,3,'T');	
	for(i = 0; i < NUM_THREADS; i++){
		droneInit(&d[i],i+5,i+5,'0'+i);
		rc = pthread_create(&droneThreads[i], NULL, droneCruiseTo,(void *) &d[i]);	
	}

	
	for(i = 0; i < NUM_THREADS; i++){
		printf("Drone thread joining with main\n");
		pthread_join(droneThreads[i],NULL);
	}

	running = 0;	
	pthread_join(boardThread,NULL);
	printf("Board joining with the main thread\n");
	boardPrint((void*)1);

	return 0; 
} 
