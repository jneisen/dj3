#include <GL/glew.h>
#include <GL/freeglut.h>
#include <Gl/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#include <winuser.h>

#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <vector>

#include "C://Users/Josep/Duck Jump 3/quad.h"
#include "C://Users/Josep/Duck Jump 3/coord.h"
#include "C://Users/Josep/Duck Jump 3/sprite.h"

using namespace std;

int width = 500, height = 500;

double playerx = 0, playery = 0, playervelx = 0, playervely = 0, size = 0.1, spritesize = size / 4, spriteDirection = 1;
bool w, a, s, d;
double playerJumpHeight = 0.035, playerFriction = 0.0005, playerMaxSpeed = 0.015;
bool grounded = false;
const double sideLength = 1/sqrt(2) * 0.01;

ifstream inSprite("spriteData.txt");
Sprite playerSprite;

vector<Quad> level1;
Sprite groundSprite;
int currentColor = 0;
vector<int> collisions;
double tempPlayerX, tempPlayerY;
int colUp, colDown, colLeft, colRight;
Quad tempQuad;
Coord tempQuadData;
Coord type;


double clickx, clicky;

void click(int button, int state, int x, int y){
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
		//builds regular ground pieces on click, the type 0 in level1 is regular ground
		clickx = -((x / (double) width) - 0.5) * 2;
		clickx += playerx;
		clicky = ((y / (double) height) - 0.5) * 2;
		clicky += playery;
		//round down no matter what because this is probably a dev tool
		clickx -= fmod(clickx, spritesize);
		clicky -= fmod(clicky, spritesize);
		level1.push_back(Quad(clickx - spritesize * 4, clicky - spritesize * 4, clickx + spritesize * 4, clicky - spritesize * 4, clickx + spritesize * 4, clicky + spritesize * 4, clickx - spritesize * 4, clicky + spritesize * 4, 0));
		cout << "level1.push_back(Quad(" << clickx - spritesize * 4 << ',' << clicky - spritesize * 4 << ',' << clickx - spritesize * 4 << ',' <<  clicky - spritesize * 4 << ',' << clickx - spritesize * 4 << ',' << clicky + spritesize * 4 << ',' << clickx + spritesize * 4 << ',' << clicky + spritesize * 4 << ", 0));\n";
	}
}

void input(){
	//engine
	if(GetAsyncKeyState(0x57)){
		//w is pressed
		w = true;
	}
	else{
		w = false;
	}	
	if(GetAsyncKeyState(0x41)){
		//a is pressed
		a = true;
	}
	else{
		a = false;
	}	
	if(GetAsyncKeyState(0x53)){
		//s is pressed
		s = true;
	}
	else{
		s = false;
	}	
	if(GetAsyncKeyState(0x44)){
		//d is pressed
		d = true;
	}
	else{
		d = false;
	}
}

void kill(){
	//move to respawn point, (had to be temp because otherwise immediately reset) 
	tempPlayerX = 0;
	tempPlayerY = 0;
	grounded = false;
}

void move(){
	//move the player, if they don't run into a wall
	//sorted array of quads, find the closest?
	//then colUpdate the playerx and playery
		
	tempPlayerX = playerx;
	tempPlayerY = playery;

	if(a && playervelx < playerMaxSpeed){
		playervelx += 0.001;
	}
	else if(d && playervelx > -playerMaxSpeed){
		playervelx -= 0.001;
	}
       	//if not trying to move either direction, add friction
      	else if(grounded && !a && !d){
		if(abs(playervelx) > 0.001){
			if(playervelx > 0){
				playervelx -= playerFriction;
			}
			else{
				playervelx += playerFriction;
			}
		}
		else if(abs(playervelx) > 0){
			playervelx = 0;
		}
	}	
	if(w && grounded == true){
		playervely = -playerJumpHeight;
		grounded = false;
	}
	//gravity stuff (1/2 of it)
	if (playervely < 0.1){
		playervely += 0.001;
	}
	// add the velocities and then check for collision
	tempPlayerX += playervelx;
	tempPlayerY += playervely;
	colUp = -1;
	colDown = -1;
	colRight = -1;
	colLeft = -1;
	//now check for collisions
	for(int i = 0; i < level1.size(); i++){
		//find the possible collisions
		if(level1[i].closestPoint(playerx, playery).x > 0 || level1[i].closestPoint(playerx, playery).y > 0){
			collisions.push_back(i);
		}		
	}
	//now I have all of the possible collisions I need to check for collisions
	//collisions are i, the closestPoint is coded point1(tens) point2(ones)
	for(int i = 0; i < collisions.size(); i++){
		tempQuad = level1[collisions[i]];	
		tempQuadData = tempQuad.closestPoint(playerx, playery);
		//check if the player will be crossing a boundary on the next frame
		if(tempPlayerX > playerx){
			//going right need to check 200s
			if(tempQuadData.x / 10 == 2 && tempQuadData.x % 10 == 3){
				if(tempPlayerX + size > tempQuad.x1){
					colRight = i;
					type.x = tempQuad.type;
				}
			}
		}
		else if(tempPlayerX < playerx){
			//going left need to check 400s
			if(tempQuadData.x / 10 == 4 && tempQuadData.x % 10 == 1){
				if(tempPlayerX - size < tempQuad.x2){
					colLeft = i;
					type.x = tempQuad.type;
				}
			}
		}
		if(tempPlayerY < playery){
		//going up need to check 100s 
			if(tempQuadData.y / 10 == 1 && tempQuadData.y % 10 == 2){
				if(tempPlayerY - size < tempQuad.y4){
					colUp = i;
					type.y = tempQuad.type;
				}	
			}
		}
		else if(tempPlayerY > playery){
			//going down need to check 300s
			if(tempQuadData.y / 10 == 3 && tempQuadData.y % 10 == 0){
				if(tempPlayerY - size < tempQuad.y2){
					colDown = i;
					type.y = tempQuad.type;
					grounded = true;
				}
			}
		}

	}
	//collision that is probably overcomplicated, for the collisions in corners or multiple collisions
	if(colUp > -1){
		if(colLeft == colUp){
			//find which one is closer if you are moving into the same block
			if(abs(tempPlayerX + size - level1[collisions[colLeft]].x2) > abs(tempPlayerY - size - level1[collisions[colUp]].y4)){
				tempPlayerX = level1[collisions[colLeft]].x2 + size;
				playervelx = 0;
			}
			else{
				tempPlayerY = level1[collisions[colUp]].y4 + size; 
				playervely = 0;
			}
		}
		//if different blocks, do seperate collisions on each of them
		else if(colLeft > -1){
			tempPlayerY = level1[collisions[colUp]].y4 + size; 
			playervely = 0;
			tempPlayerX = level1[collisions[colLeft]].x2 + size;
			playervelx = 0;
		}
		//check right too, after left for no reason
		else if(colRight == colUp){
			if(abs(tempPlayerX - size - level1[collisions[colRight]].x1) > abs(tempPlayerY - size - level1[collisions[colUp]].y4)){
				tempPlayerX = level1[collisions[colRight]].x1 - size;
				playervelx = 0;
			}
			else{
				tempPlayerY = level1[collisions[colUp]].y4 + size;
				playervely = 0;
			}
		}
		//not same object
		else if(colRight > -1){
			tempPlayerY = level1[collisions[colUp]].y4 + size;
			playervely = 0;
			tempPlayerX = level1[collisions[colRight]].x1 - size;
			playervelx = 0;
		}
		else{
		//if only upward collision
			tempPlayerY = tempQuad.y4 + size;
			playervely = 0;
		}
	}
	//repeat the same process for the downward collision (this is where grounded stuff will be done)
	else if(colDown > -1){
		if(colLeft == colDown){
			if(abs(tempPlayerX + size - level1[collisions[colLeft]].x2) > abs(tempPlayerY + size - level1[collisions[colDown]].y2)){
				tempPlayerX = level1[collisions[colLeft]].x2 + size;
				playervelx = 0;
			}
			else{
				tempPlayerY = level1[collisions[colDown]].y2 - size;
				playervely = 0;
			}
		}
		else if(colLeft > -1){
			tempPlayerY = level1[collisions[colDown]].y2 - size;
			playervely = 0;
			tempPlayerX = level1[collisions[colLeft]].x2 + size;
			playervelx = 0;
		}
		else if(colRight == colDown){
			if(abs(tempPlayerX - size - level1[collisions[colRight]].x1) > abs(tempPlayerY + size - level1[collisions[colDown]].y2)){
				tempPlayerX = level1[collisions[colRight]].x1 - size;
				playervelx = 0;
			}
			else{
				tempPlayerY = level1[collisions[colDown]].y2 - size;
				playervely = 0;
			}
		}
		else if(colRight > -1){
			tempPlayerY = level1[collisions[colDown]].y2 - size;
			playervely = 0;
			tempPlayerX = level1[collisions[colRight]].x1 - size;
			playervelx = 0;
		}
		else{
			tempPlayerY = level1[collisions[colDown]].y2 - size;
			playervely = 0;
		}
	}
	else if(colLeft > -1){
		tempPlayerX = level1[collisions[colLeft]].x2 + size;
		playervelx = 0;	
	}
	else if(colRight > -1){
		tempPlayerX = level1[collisions[colRight]].x1 - size; 
		playervelx = 0;
	}
	//look at the type for something
	if(type.x == 1 || type.y == 1){ //if hit a 'death' block
		kill();
		cout << "killed";
	}
	type.x = 0;
	type.y = 0;
	playery = tempPlayerY;
	playerx = tempPlayerX;
	collisions.clear();	
}
void displayMe(void) {
	glClear(GL_COLOR_BUFFER_BIT);
	//get each quad in a list	
	glBegin(GL_QUADS);	
		//literally all of the ground tiles in level1 (probably terribly inefficient) also shamelessly stolen from zephyr
		for(int i = 0; i < level1.size(); i++){
			for(int j = 0; j < 8; j++){
				for(int k = 0; k < 8; k++){
					currentColor = groundSprite.colors[k * 8 + j];
					if(currentColor != -1){
						if(currentColor == 0)
							glColor3f(0.0, 0.0, 0.0);
						else if(currentColor == 1)
							glColor3f(1.0, 1.0, 1.0);
						else if(currentColor == 2)
							glColor3f(0.9336, 0.6758, 0.1602);
						else if(currentColor == 3)//dark green
							glColor3f(0.2227, 0.3398, 0.1094);
						else if(currentColor == 4)//light green
							glColor3f(0.3438, 0.5234, 0.1719);
						else if(currentColor == 5)//darkest brown
							glColor3f(0.1602, 0.1133, 0.0508);
						else if(currentColor == 6)//dark brown
							glColor3f(0.2227, 0.1641, 0.1094);
						else if(currentColor == 7)//light brown
							glColor3f(0.3203, 0.2305, 0.1797);
						glVertex2f(playerx - level1[i].x1 + (j - 8) * spritesize, playery - level1[i].y1 - (k) * spritesize); //bottom left
						glVertex2f(playerx - level1[i].x1 + (j - 7) * spritesize, playery - level1[i].y1 - (k) * spritesize); //bottom right			
						glVertex2f(playerx - level1[i].x1 + (j - 7) * spritesize, playery - level1[i].y1 - (k + 1) * spritesize); //top right
						glVertex2f(playerx - level1[i].x1 + (j - 8) * spritesize, playery - level1[i].y1 - (k + 1) * spritesize); //top left
					}
				}
			}
		}
		// the player's square (all 64 of them)
		if(playervelx > 0){
			spriteDirection = -1;
		}
		else if(playervelx < 0){
			spriteDirection = 1;
		}
		for(int i = 0; i < 8; i++){
			for(int j = 0; j < 8; j++){
				//set the color of the current square based on the sprite color
				currentColor = playerSprite.colors[i * 8 + j];
				//skip this square if its supposed to be transparent	
				if(currentColor != -1){
					//black
					if(currentColor == 0){
						glColor3f(0.0, 0.0, 0.0);
					}
					//white
					else if(currentColor == 1){
						glColor3f(1.0, 1.0, 1.0);
					}
					//gold
					else if(currentColor == 2){
						glColor3f(0.9336, 0.6758, 0.1602);
					}

					glVertex2f(spriteDirection * ((j - 4) * spritesize), (i - 4) * spritesize); //bottom left
					glVertex2f(spriteDirection * ((j - 3) * spritesize), (i - 4) * spritesize); //bottom right
					glVertex2f(spriteDirection * ((j - 3) * spritesize), (i - 3) * spritesize); //top right
					glVertex2f(spriteDirection * ((j - 4) * spritesize), (i - 3) * spritesize); //top left
				}
			}
		}
	glEnd();
	glFlush();
}

void timer(int){
	glutPostRedisplay(); //colUpdates the screen everyframe
	input(); //colUpdates the playerboxx and playerboxy based on the input
	move(); //takes the input and moves the screen around the player
	glutTimerFunc(1000/60, timer, 0);
}

void level(){
	//function for creating the whole level
	level1.push_back(Quad(-0.1, 0.2, 0.1, 0.2, 0.1, 0.4, -0.1, 0.4, 0)); //random block2 (ground)	
	level1.push_back(Quad(-0.3, 0.2, -0.1, 0.2, -0.1, 0.4, -0.3, 0.4, 0)); //random block2 (ground)
	level1.push_back(Quad(0.1, 0.2, 0.3, 0.2, 0.3, 0.4, 0.1, 0.4, 0)); //random block2 (ground)
	level1.push_back(Quad(0.5, -0.2, 0.7, -0.2, 0.7, 0.2, 0.5, 0.2, 1)); //death block(needs a different sprite)
}

void initSprites(){
	//using the input insprite, grab the text from data and store it in sprites
	int tempSprite[64];
	int element = 0;
	int start = 0;
	int space = 0;
	int t = 0; //variable for the number of sprites (bad code)
	for(string line; getline(inSprite, line); ){
		start = 0;
		space = 0;
		//have a string, need to split it up into ints
		for(int i = 0; i < 64; i++){
			//get the next space and split the string between the spaces and then convert to int
			space = line.find_first_of(' ', start + 1);
			if(space == -1){
				cout << "add a space to the end of the line buddy";
			}
			element = stoi(line.substr(start, space - start));
			tempSprite[i] = element;
			start = space + 1;
			}
		if(t == 0)
			playerSprite = Sprite(tempSprite);
		if(t == 1)
			groundSprite = Sprite(tempSprite);
		t++;

	}
}

int main(int argc, char** argv){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(500, 500); //distance from top colLeft of screen
	glutCreateWindow("Hello Worlds");
	//grab the sprite data too
	initSprites();

	level();

	glutMouseFunc(click);
	glutDisplayFunc(displayMe);
	timer(0);
	glutMainLoop();
	return 0;	
}
