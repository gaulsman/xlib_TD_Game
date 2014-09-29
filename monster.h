#ifndef __MONSTER_H__
#define __MONSTER_H__
#include <vector>

class XInfo;
class Scoreboard;

class triplet{
  public:
	int x, y;
	char direction;
	triplet(int a, int b, char d);
	triplet(const triplet &t);
};

class Monster {
  private:
    int type, HP, maxHP, speed, gold, effect, effectMag, effectTime, x, y, pixelX, pixelY, tileSize, startX, startY, count, corner; //speed in ticks per tile
	std::vector<triplet> turningPoints; //tile x, y and a direction
	bool nextTurnYet();
	Scoreboard *sb;
  public:
    Monster(int t, int x, int y, int hp, int s, int g, int sx, int sy, int ts, std::vector<triplet> tp, Scoreboard *sbp);
	bool inTile(int tx, int ty);
	bool damage(int d, int e);
	std::pair<int, int> location();
	bool draw(XInfo &xInfo);
	bool resize(int px, int py, int ts);
	bool dead();
	bool tick();
};

#endif