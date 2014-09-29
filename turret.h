#ifndef __TURRET_H__
#define __TURRET_H__
#include <vector>
#include <cmath>

class Monster;
class Projectile;
class XInfo;
class Floor;

const double PI = atan(1) * 4;

class Turret {
  private:
    int x, y, type, level, cost, attack, ROF, range, angle, charge, tileSize, startX, startY;
	Floor *floor;
	std::vector<Monster*> targetList;
	std::vector<Projectile*> projectileList;
	bool fire();
	bool inrange(std::pair<int, int> p);
	bool updateTargetList();
  public:
	bool showRangeIndicator;
	std::vector<int> printInfo();
    Turret(int x, int y, int t, int l, int atk, int rof, int r, int ts, int sx, int sy, Floor *fl);
	~Turret();
	int whatType();
	int whatLV();
	std::pair<int, int> location();
	bool upgrade(int atk, int rof, int r);
	bool draw(XInfo &xInfo);
	bool resize(int px, int py, int ts);
	void tick();
};

#endif