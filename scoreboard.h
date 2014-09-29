#ifndef __SCOREBOARD_H__
#define __SCOREBOARD_H__
#include <vector>
#include <string>

const int minWidth = 600, minHeight = 400;
class Turret;
class triplet;
class XInfo;
class Floor;

class Scoreboard {
  private:
	XInfo &xInfo;
	int X, Y, gold, score, wave, lives, tileSize, startX, startY;
	std::vector<std::vector<std::vector<int> > > turretData; //Type[level[cost, attack, ROF, range]]
	std::vector<triplet> turningPoints;
	Turret *selectedTurret;
	std::string errorMessage, warnMessage;
	bool over;
  public:
	Floor *floor;
    Scoreboard(int sx, int sy, int g, int l, int ts, XInfo &xinfo);
	bool resize(int x, int y, int ts, int sx, int sy);
	Turret *buildTurret(int x, int y, int t, int l);
	int nextWave();
	bool addLives(int l);
	bool addGold(int g);
	int addScore(int s);
	bool selectTurret(Turret *t);
	bool deselectTurret();
	bool upgradeTurret(Turret *t);
	bool destroyTurret(Turret *t, bool refund);
	void print(int x, int y, std::string m, int i);
	void warn(std::string m);
	void error(std::string m);
	bool draw();
	void gameOver();
	void setTP(std::vector<triplet> tp);
};

#endif