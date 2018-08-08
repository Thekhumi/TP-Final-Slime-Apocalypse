#ifndef ENEMIGO_H
#define ENEMIGO_H

class Enemigo
{
private:
	int _x;
	int _y;
	int _speed;
public:
	Enemigo();
	Enemigo(int x, int y,int speed);
	int getX() { return _x; };
	int getY() { return _y; };
	int getSpeed() { return _speed; };
	void setX(int x) { _x = x; };
	void setY(int y) { _y = y; };
	~Enemigo();
};
#endif
