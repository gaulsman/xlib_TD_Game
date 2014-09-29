#ifndef __PROJECTILE_H__
#define __PROJECTILE_H__

class Monster;
class XInfo;

class Projectile {
  private:
    int type, speed, pixelX, pixelY, tileSize, attack, angle, startX, startY, countx, county, speedX, speedY; //speed in tiles per sec
	//Shape and explode anim
	bool live;
	Monster *target;
	void updateHeading();
  public:
    Projectile(int ty, int x, int y, int s, int atk, int sx, int sy, int ts, int ang, Monster *t);
	bool resize(int px, int py, int ts);
	bool active();
	bool draw(XInfo &xInfo);
	bool tick();
};

#endif