#ifndef BALA_H
#define BALA_H
class Bala
{
private:
	int _speedX = 0;
	int _speedY = 0;
	int _x = 0;
	int _y = 0;
	bool _active = false;
public:
	Bala();
	void BalaUpdate(int xSpeed,int ySpeed);
	void setActive(bool active);
	int getX() { return _x; }
	int getY() { return _y; }
	void setX(int x) { _x = x; }
	void setY(int y) { _y = y; }
	bool getActive() { return _active; }
	~Bala();
};
#endif

