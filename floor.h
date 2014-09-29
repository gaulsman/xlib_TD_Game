#ifndef __FLOOR_H__
#define __FLOOR_H__
#include <vector>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

class Turret;
class Scoreboard;
class triplet;
class Monster;

struct XInfo {
    Display* display;
    int		 screen;
    Window	 window;
	std::vector<GC> gc;
	Pixmap	pixmap;		// double buffer
	int		width;		// size of window
	int		height;
};


const int battleFieldWidth = 20, battleFieldHeight = 15;

class Floor {
  private:
    std::vector<std::vector<Turret*> > floor;
	std::vector<Turret*> turretList;
	std::vector<std::pair<int, int> > path;
	std::vector<std::vector<bool> > pathChk;
	std::vector<triplet> turningPoints;
	std::vector<Monster*> monsterList;
	bool paid, over, win, splash;
	int X, Y, startX, startY, tileSize, selectedType, wave, count, left2Spawn, mouseX, mouseY;	//startX wrt window, tileSize in pixels.
	Turret *selectedTurret;
	bool isOnPath(int x, int y);
	bool isOccupied(int x, int y);
	int indexOfTurretList(int x, int y);
	char getDirection(std::pair<int, int> p1, std::pair<int, int> p2);
	XInfo &xInfo;
	Scoreboard *sb;
	std::pair<int, int> insideTile(int x, int y);
	bool insideField(int x, int y);
	int insideSelect(int x, int y);
	int nextWave();
	bool spawnMonster();
	bool allDead();
	bool buyTurret();
	bool deselectTurret();
	bool cancelTurret();
  public:
	int gameSpeed, prevGameSpeed;
    Floor(int sx, int sy, int ts, std::vector<std::pair<int, int> > p, Scoreboard *s, XInfo &xinfo);
	~Floor();
	bool resize();
	std::vector<Monster*>* getMonsterList();
	//bool createPath(vector<pair<int, int> > p);
	bool buildTurret(int x, int y, int t);
	bool upgradeTurret();
	bool destroyTurret();
	bool click(int x, int y, int state);
	bool press(int kc);
	bool move(int x, int y);
	bool draw();
	bool tick();
	void gameOver();
};

#endif