//Michael Alicea
//CSE321 Lab 1

#include <string.h>
#include <stdio.h>
#include <pthread.h>

#define GRID_SIZE 20
#define NUM_THREADS 10

pthread_mutex_t lock;
pthread_t droneThreads[NUM_THREADS];
pthread_t boardThread;

char board[GRID_SIZE][GRID_SIZE];
void boardInit();
void boardInsert(int,int,char);
char boardGetPos(int,int);
void boardPrint();

/*
   Pretty self explanatory but one note is the cruising and package delivered
   are treated as boolean values.
   Cruising is set to 1 when the drone is cruising to a destination
   PackageDelivered is set to 1 when well, the package is delivered.
   */
typedef struct _drone {
  int x;
  int y;
  int destX; 
  int destY;
  int homeX;
  int homeY;
  char c;
  int cruising; 
  int packageDelivered; 
}DRONE;

void droneInit(DRONE*,int,int,char);
int droneCheckDir(DRONE*,char);
void droneMoveDir(DRONE*,char);
void *droneCruiseTo(void *);
void droneReturnHome(DRONE*);
void droneHome(DRONE*);

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

   Print the board (it is outside the mutex because boardPrint() has it's own mutex
   This will ensure everything is up to date, incase a read gets in there and a drone
   moves)
   */
void boardInsert(int x,int y,char ch) {
  pthread_mutex_lock(&lock);
  board[y][x] = ch; 
  pthread_mutex_unlock(&lock);
  boardPrint();
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
   Just traverses the matrix printing chars
   Mutex lock to ensure it has the latest information and prevent writing/reading
   */
void boardPrint() {
  pthread_mutex_lock(&lock);
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
  pthread_mutex_unlock(&lock);
}

/*Initializes the drones starting coordinates, desination and character*/
void droneInit(DRONE *d,int dX,int dY,char dC){
  d->x = 0;
  d->y= 0;
  d->homeX = 0;
  d->homeY = 0;
  d->destX = dX;
  d->destY = dY;
  d->c = dC;
  d->cruising = 0;
  d->packageDelivered = 0;
  boardInsert(dX,dY,'P');
}

/*
   Returns 0 if there is nothing in the way of the drone
   Returns 0 if the drones package is in front it, if it's not it's package, avoid
   Retruns 1 if there is indeed something in the way of the drone
   Takes in a drone object and the char direction that it should check
   The rest should be self explanatory, checks ahead of the drone on the board
   to see what is there.
   */
int  droneCheckDir(DRONE *d, char dir) {
  if (dir == 'N') {
    if(d->y == 0)return 1;	
    else if(boardGetPos(d->x,d->y-1) == 'x') return 0;
    else if(boardGetPos(d->x,d->y-1) == ' ') return 0;
    else if(d->x == d->destX && d->y-1 == d->destY)return 0;
    else {
      printf("%c avoiding collision with %c\n",
          d->c,boardGetPos(d->x,d->y-1));
      return 1;
    }	
  }

  else if (dir == 'S'){
    if(d->y == GRID_SIZE-1)return 1;
    else if(boardGetPos(d->x,d->y+1) == 'x') return 0;
    else if(boardGetPos(d->x,d->y+1) == ' ') return 0;	
    else if(d->x == d->destX && d->y+1 == d->destY)return 0;
    else {
      printf("%c avoiding collision with %c\n",
          d->c,boardGetPos(d->x,d->y+1));
      return 1;
    }
  }

  else if (dir == 'E'){
    if(d->x == GRID_SIZE-1)return 1;	
    if(boardGetPos(d->x+1,d->y) == 'x') return 0;
    else if(boardGetPos(d->x+1,d->y) == ' ') return 0;	
    else if(d->x+1 == d->destX && d->y == d->destY)return 0;
    else {
      printf("%c avoiding collision with %c\n",
          d->c,boardGetPos(d->x+1,d->y));
      return 1;
    }
  }

  else if (dir == 'W'){
    if(d->x == 0)return 1;	
    if(boardGetPos(d->x-1,d->y) == 'x') return 0;
    else if(boardGetPos(d->x-1,d->y) == ' ') return 0;	
    else if(d->x-1 == d->destX && d->y == d->destY)return 0;
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
   re-add the 'x' char when the drone has moved
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
   Calls sleep for 1 ms after all has been said done.
   */
void droneMoveDir(DRONE *d,char dir) {
  if(d->x >= 0 && d->y == 0)boardInsert(d->x,d->y,'x');
  else if(d->y == GRID_SIZE-1 && d->x > 0)boardInsert(d->x,d->y,'x');
  else if (d->y > 0 && d->x == 0)boardInsert(d->x,d->y,'x');
  else if (d->y > 0 && d->x == GRID_SIZE-1)boardInsert(d->x,d->y,'x');
  else boardInsert(d->x,d->y,' ');

  if (dir == 'N') {
    if (droneCheckDir(d,'N')== 0){ 
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
   This inititiates the cruising states, and also knows when the package has been picked up
   Self explanatory if x > destinationX "move right" etc etc

   This is also the function that gets passed into the thread
   Create a new drone object, and initiate it to the voided function being passed in
   cast it to a drone, we're good to go!  
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
    else if(y > d->y){
      droneMoveDir(d,'S');
    }
    else if(y < d->y){
      droneMoveDir(d,'N');
    }
    else if(x < d->x){
      droneMoveDir(d,'W');
    }	

    else if(d->y > 0 && d->x > 0){
      droneReturnHome(d); 
    }
  }
}

/*
   Resets the drones destination and send that baby home	
   */
void droneReturnHome(DRONE *d) {
  d->destX = d->homeX;
  d->destY = d->homeY;
  d->packageDelivered = 1;
  droneCruiseTo(d);
}

/*
   Drone hit home, set his character to x to prevent further collisions
   reinserts him to update the board
   The drone is not destroyed here because once running = 0 
   it will exit the droneCruiseTo function and the thread will naturally join the main thread
   */
void droneHome(DRONE *d) {
  d->c = 'x';
  boardInsert(d->x,d->y,d->c);
  d->cruising = 0;
}

/*
   Initializes Drones, Board, starts and joins pthreads
   */
int main() {
  int dc;
  int i;

  DRONE d[NUM_THREADS];
  pthread_mutex_init(&lock,NULL);

  boardInit();

  /*
#1 -!-!-ATTENTION-!-!-
Initialize where you want the drones to go here! Just change their destination X,Y coords
You can change the character of the drone to whatever you want except 'x',' ' or 'P'. 
I chose to have it as their corresponding number in the array 

droneInit(DRONE*,int x,int y,char c)
*/
  droneInit(&d[0], 3,1, '0');
  droneInit(&d[1], 19,1, '1');
  droneInit(&d[2], 9,1, '2');
  droneInit(&d[3], 1,8, '3');
  droneInit(&d[4], 19,3, '4');
  droneInit(&d[5], 10,8, '5');
  droneInit(&d[6], 11,9, '6');
  droneInit(&d[7], 17,3, '7');
  droneInit(&d[8], 13,6, '8');
  droneInit(&d[9], 14,14, '9');

  /*
#2 -!-!-ATTENTION-!-!-
Initialize any obstacles here, it can be any char but 'x',' ', or 'P' 
boardInsert(int x,int y,char c)
*/
  boardInsert(5,4, 'T');
  boardInsert(6,5, 'T');
  boardInsert(13,12, 'T');
  boardInsert(9,3, 'T');
  boardInsert(13,15, 'T');

  boardPrint();
  for(i = 0; i < NUM_THREADS; i++){
    dc = pthread_create(&droneThreads[i], NULL, droneCruiseTo,(void *) &d[i]);	
  }

  for(i = 0; i < NUM_THREADS; i++){
    printf("Drone thread joining with main\n");
    pthread_join(droneThreads[i],NULL);
  }
  return 0; 
} 
