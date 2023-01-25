#include <GL/glew.h>
#include <GL/freeglut.h>
#include <Gl/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#include <winuser.h>

#include <iostream>
#include <string>
#include <math.h>
#include <vector>

#include "C://Users/Josep/quad.h"
#include "C://Users/Josep/coord.h"

using namespace std;

double playerx = 0, playery = 0, size = 0.1;
bool w, a, s, d;
const double sideLength = 1/sqrt(2) * 0.01;

vector<Quad> level1;
vector<int> collisions;
double tempPlayerX, tempPlayerY;
Quad tempQuad;
Coord tempQuadData;

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

void move(){
	//move the player, if they don't run into a wall
	//sorted array of quads, find the closest?
	//then update the playerx and playery
	
	tempPlayerX = playerx;
	tempPlayerY = playery;

	if(w){
		if(w == a){
			tempPlayerX += sideLength;
			tempPlayerY -= sideLength;
		}
		else if(w == d){
			tempPlayerX -= sideLength;
			tempPlayerY -= sideLength;
		}
		else{
			tempPlayerY -= 0.01;
		}
	}
	if(s){
		if(s == a){
			tempPlayerX += sideLength;
			tempPlayerY += sideLength;
		}
		else if(s == d){
			tempPlayerX -= sideLength;
			tempPlayerY += sideLength;
		}
		else{
			tempPlayerY += 0.01;
		}
	}
	if(!w && !s){
		if(a){
			tempPlayerX += 0.01;
		}
		if(d){
			tempPlayerX -= 0.01;
		}
	}
	//now check for collisions
	for(int i = 0; i < level1.size(); i++){
		//find the possible collisions
		if(level1[i].closestPoint(playerx, playery).x > 0 || level1[i].closestPoint(playerx, playery).y > 0){
			collisions.push_back(i);
		}		
	}
	//now I have all of the possible collisions I need to check for collisions
	//collisions are i, the closestPoint is coded point1(tens) point2(ones)
	//cout << playerx << '\n' << playery << '\n'; //player pos
	for(int i = 0; i < collisions.size(); i++){
		tempQuad = level1[collisions[i]];	
		tempQuadData = tempQuad.closestPoint(playerx, playery);
		//check if the player will be crossing a boundary on the next frame
		if(tempPlayerY < playery){
		//going up need to check 100s
		//130 or 323 or 230 or 412 are possible 
			if(tempQuadData.y / 10 == 1 && tempQuadData.y % 10 == 2){
				if(tempPlayerY - size < tempQuad.y4){
					tempPlayerY = tempQuad.y4 + size;
				}	
			}
		}
		else if(tempPlayerY > playery){
			//going down need to check 300s
			if(tempQuadData.y / 10 == 3 && tempQuadData.y % 10 == 0){
				if(tempPlayerY - size < tempQuad.y2){
					tempPlayerY = tempQuad.y2 - size;
				}
			}
		}
		if(tempPlayerX > playerx){
			//going right need to check 200s
			if(tempQuadData.x / 10 == 2 && tempQuadData.x % 10 == 3){
				if(tempPlayerX + size > tempQuad.x1){
					tempPlayerX = tempQuad.x1 - size;
				}
			}
		}
		else if(tempPlayerX < playerx){
			//going left need to check 400s
			if(tempQuadData.x / 10 == 4 && tempQuadData.x % 10 == 1){
				if(tempPlayerX - size < tempQuad.x2){
					tempPlayerX = tempQuad.x2 + size;
				}
			}
		}
	}
	playerx = tempPlayerX;
	playery = tempPlayerY;
	collisions.clear();	
}
void displayMe(void) {
	glClear(GL_COLOR_BUFFER_BIT);
	//get each quad in a list	
	glBegin(GL_QUADS);
		glVertex2f(-size, -size); //bottom left
		glVertex2f(size, -size); //bottom right
		glVertex2f(size, size); //top right
		glVertex2f(-size, size); //top left

		glVertex2f(playerx - 0.5, playery - 0.5);
		glVertex2f(playerx + 0.5, playery - 0.5);
		glVertex2f(playerx + 0.5, playery + 0.5);
		glVertex2f(playerx - 0.5, playery + 0.5);
	glEnd();
	glFlush();
}

void timer(int){
	glutPostRedisplay(); //updates the screen everyframe
	input(); //updates the playerboxx and playerboxy based on the input
	move(); //takes the input and moves the screen around the player
	glutTimerFunc(1000/60, timer, 0);
}

int main(int argc, char** argv){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(500, 500); //distance from top left of screen
	glutCreateWindow("Hello Worlds");

	level1.push_back(Quad(-size, -size, size, -size, size, size, -size, size, true));	//player block
	level1.push_back(Quad(-0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, 0.5)); //random block

	glutDisplayFunc(displayMe);
	timer(0);
	glutMainLoop();
	return 0;	
}
