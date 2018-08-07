#include "Bala.h"



Bala::Bala(){
	_x = -10;
	_y = 500;
	_active = false;
	_speedX = 0;
	_speedY = 0;
}
void Bala::BalaUpdate(int xSpeed, int ySpeed) {
	if (_active) {
		_x += xSpeed;
		_y += ySpeed;
	}
}
void Bala::setActive(bool active) {
	_active = active;
}
Bala::~Bala(){

}
