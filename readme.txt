Michael Alicea
CSE 321 Lab 1
DroneCommander

-First and foremost there is alot of scattered information as to wether or not we needed to have an input file or not, it seems that we do not.
-On the grading rubric it says sample input -1 to -5 points, now since the need for a .txt was never really mentioned and very wish-washy I'm assuming this means that a default setup must be included, which I have done.

-I know you're busy and probably don't want to be reading this so

+TLDR:
-Everything is contained within droneCommander.c I was going to create a header file but there aren't too many methods and only 1 struct so there's no need.
-Main function is at bottom it's very clear how to set the drone destination and obstacle coordinates
-Run make


-The program is relatively simple, and in my opinion incredibly clear to understand, I have commented out every method and named them aptly so hopefully there is no confusion as to what does what. 
-The DCAS is located at (0,0)
-The GRID_SIZE can be changed to anything you want.  I believe 50,50 is what we were told so that is the default all the default coordinates are set within a 20,20 size, it ends faster and is easier to see what is going on.  I am assuming you guys have places to put everything so do as you please! 

How to use:
-You can edit the each drones coordinates very simply they are at the bottom in the main function

-If you do a find for #1 you will find it very simply.
-The drone's coordinates are set upon intitialization of the drone itself and the parameters are as follows

+droneInit(Drone *d,int x, int y, char c)
-So you pass it a pointer to a drone from the array that is already declared, an X coordinate, Y coordinate and the character of the drone the character can be anything but 'x',' ' or 'P'

-You can set obstacles with an equal amount of effort you can do a find for #2 or just simply look right below where you initialized the drones

-You simply insert into the board what it is that you want and the parameters are as follows

+boardInsert(int x, int y, char c)
-So you pass it the X and Y coordinates of the obstacle that you are placing and then the character that you would like to represent it by.  I prefer to use T for tree but you can use anything that is not the following characters
'x',' ', or 'P'


+Known BUGS:
-This ugly text file..hopefully you'll open it in VIM

-I do not add checks for USER error such as trying to insert outside of the GRID_SIZE, we were told that you guys won't try to break the program by doing such this as that so I am acting accordingly

-My collision with objects will sometimes break and a drone will be stuck in an infinite loop of trying to move in the blocked direction, realizing that it is blocked and then trying to change directions, changes it's direction but then goes back to the original position.
-Not too sure how I'd fix this except having the drone have a "memory" of what it's last moves were.  I can fix it for certain cases but then it will do it for others.  I have changed it to be the most effective(in my opinion) for moving mostly south east to get package and north west to go home.
-I can change all these by alternating the order of the droneMoveDir(..) functions along with the droneCruiseTo

-Sometimes it may appear as if a drone did not get drawn, it did but during a specific frame it may be in the middle of moving so when it accesses the board to draw a ' ' it won't yet be updated to the drone being in the next position. This doesn't happen too often

-Enjoy, maybe you'll find some more, hopefully not :) 
