#ifndef QUAD_H
#define QUAD_H

#include <iostream>
#include <array>
#include "C://Users/Josep/Duck Jump 3/coord.h"

using namespace std;

class Quad{
	public:
	double x1, y1, x2, y2, x3, y3, x4, y4;
	int type; //-1 is player, 0 is ground
	//should never make a default quad
	Quad(){
		x1 = 0;
		x2 = 0;
		x3 = 0;
		x4 = 0;

		y1 = 0;
		y2 = 0;
		y3 = 0;
		y4 = 0;
	}
	Quad(double xx1, double yy1, double xx2, double yy2, double xx3, double yy3, double xx4, double yy4, int ttype){
		x1 = xx1; //bottom left
		y1 = yy1;
		x2 = xx2; //top left
		y2 = yy2;
		x3 = xx3; //top right
		y3 = yy3;
		x4 = xx4; //bottom right
		y4 = yy4;
		type = ttype;
	}
	Coord closestPoint(double playerx, double playery){
		//loop through the x and y values and find the potential collisions
		//slightly unnecessary to make arrays, but it makes it look cleaner
		double x [4] = {x1, x2, x3, x4};
		double y [4] = {y1, y2, y3, y4};

		int j, k;
		int possible = -1; //encoded, hundred is side (1 top and clockwise) tens place is 1 point and the second is in ones place
		int possible2 = -1; //second one for x collisions

		for(int i = 0; i < 4; i++){

			k = (i + 1 == 4) ? 0 : i + 1;
			//get the edges
			if(i == 3 || i == 1){
				if((y[i] - playery < 0.1 && y[k] - playery > -0.1) || (y[i] - playery > -0.1 && y[k] - playery < 0.1)){
					if(x[i] - playerx < 0.2 && x[i] - playerx > 0){
						possible2 = 20 + i;
					}
					if(x[i] - playerx > -0.1 && x[i] - playerx < 0){
						possible2 = 40 + i;
					}
				}
			}
			if(i == 2 || i == 0){
				if((x[i] - playerx < 0.1 && x[k] - playerx > -0.1) || (x[i] - playerx > -0.1 && x[k] - playerx < 0.1)){
					if(playery - y[i] < 0.2 && playery - y[i] > 0){	
						possible = 10 + i;
					}
					if(playery - y[i] > -0.11 && playery - y[i] < 0){
						possible = 30 + i;
					}	
				}
			}
		}
		//returns the possible collisions
		return Coord(possible2, possible);
	}
};
#endif
