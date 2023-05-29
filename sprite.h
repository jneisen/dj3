#ifndef SPRITE_H
#define SPRITE_H

#include <iostream>
#include <string>

using namespace std;

static const int length = 64;

class Sprite{
	public:	
	//need a number with 5 bits (32 different colors)
	int colors[length] = { 0 };
	//an array that goes top left to bottom right, stores the colors of the sprite
	Sprite(){
		//blank constructor
	}
	Sprite(int input[]){
		for(int i = 0; i < length; i++)
			colors[i] = input[i];
	}
	
	friend ostream& operator<<(ostream& os, const Sprite& obj);
	};
//below is the overloading of the << operator to allow for a custom cout for debugging purposes
	ostream& operator<<(ostream& os, const Sprite& obj){
	string test = "";
	for(int i = 0; i < 64; i++){
		test += to_string(obj.colors[i]) + ' ';
	}
	os << test;
	return os;
	}

#endif
