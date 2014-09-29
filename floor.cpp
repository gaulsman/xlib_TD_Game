#include <iostream>
#include <string>
#include <cstdio>
#include <ctime>
#include <sstream>
#include <cstdlib>
#include "floor.h"
#include "turret.h"
#include "scoreboard.h"
#include "monster.h"

using namespace std;

const int seed = time(NULL);

Floor::Floor(int sx, int sy, int ts, vector<pair<int, int> > p, Scoreboard *s, XInfo &xinfo) :
	pathChk(vector<vector<bool> >(battleFieldWidth, vector<bool>(battleFieldHeight, false))),
    floor(vector<vector<Turret*> >(battleFieldWidth, vector<Turret*>(battleFieldHeight, (Turret *)NULL))),
	xInfo(xinfo){
	startX = sx;
	startY = sy;
	tileSize = ts;
	path = p;
	over = false;
	sb = s;
	X = 0;
	Y = 0;
	wave = 0;
	count = 0;
	left2Spawn = 0;
	selectedType = -1;
	selectedTurret = NULL;
	gameSpeed = 1;
	prevGameSpeed = 1;
	sb->floor = this;
	paid = true;
	splash = true;
	win = false;
	pathChk[p[0].first][p[0].second] = true;

	char prevDir = 0, currentDir = 0;
	for (int x = 1; x < p.size(); x++){
		pathChk[p[x].first][p[x].second] = true;
		currentDir = getDirection(p[x - 1], p[x]);
		if (prevDir != currentDir){
			turningPoints.push_back(triplet(p[x - 1].first, p[x - 1].second, currentDir));
		}
		prevDir = currentDir;
	}
	turningPoints.push_back(triplet(p[p.size() - 1].first, p[p.size() - 1].second, 0));
	sb->setTP(turningPoints);
}

Floor::~Floor(){

}

char Floor::getDirection(pair<int, int> p1, pair<int, int> p2){
	if (p1.first > p2.first)
		return 'W';
	else if (p1.first < p2.first)
		return 'E';
	else if (p1.second > p2.second)
		return 'N';
	else if (p1.second < p2.second)
		return 'S';
	else 
		return 0;
}

bool Floor::isOnPath(int x, int y){
	return pathChk[x][y];
}

bool Floor::isOccupied(int x, int y){
	return floor[x][y];
}

int Floor::indexOfTurretList(int x, int y){
	for (int i = 0; i < turretList.size(); i++){
		if (turretList[i]->location().first == x && turretList[i]->location().second == y)
			return i;
	}	
	return -1;
}


bool Floor::resize(){
	if (xInfo.height < 360 || xInfo.width < 600){
		if (gameSpeed != 0)
			prevGameSpeed = gameSpeed;
		gameSpeed = 0;
	}
	tileSize = xInfo.height / 18 < xInfo.width / 30 ? xInfo.height / 18 : xInfo.width / 30;
	X = (xInfo.width - tileSize * 30) / 2;
    Y = (xInfo.height - tileSize * 18) / 2;
	startX = 10 * tileSize;
	startY = 2 * tileSize;
	//cout << "Resize to " << xInfo.width << "x" << xInfo.height << " X=" << X << "Y=" << Y << " TS = " << tileSize << endl;
	sb->resize(X, Y, tileSize, startX, startY);
	for (int x = 0; x < turretList.size(); x++)
		turretList[x]->resize(X + startX, Y + startY, tileSize);
	for (int x = 0; x < monsterList.size(); x++)
		monsterList[x]->resize(X + startX, Y + startY, tileSize);
	this->draw();
	return true;
}

bool Floor::buildTurret(int x, int y, int t){
	if (isOnPath(x, y) || isOccupied(x, y)){
		sb->error("Cannot be build here.");
		return false;
	}
	else {
		floor[x][y] = sb->buildTurret(x, y, t, 0);
		if (floor[x][y] != NULL){
			turretList.push_back(floor[x][y]);
			return true;
		}
	}	
}

bool Floor::upgradeTurret(){
	if (selectedTurret != NULL){
		sb->upgradeTurret(selectedTurret);
		return true;
	}
	else
		return false;
	/*
	if (isOccupied(x, y)){
		sb->upgradeTurret(floor[x][y]);
		return true;
	}
	else
		return false;
	*/
}

bool Floor::destroyTurret(){
	int idx, x, y;
	if (selectedTurret != NULL){
		if (sb->destroyTurret(selectedTurret, true)){
			x = selectedTurret->location().first;
			y = selectedTurret->location().second;
			idx = indexOfTurretList(x, y);
			//cout << x<<"," <<y<<" sell idx "<<idx << endl;
			if (idx != -1){
				turretList.erase(turretList.begin() + idx);
				//cout << "turret list size "<<turretList.size() << endl;
				delete floor[x][y];
				floor[x][y] = NULL;
				selectedTurret = NULL;
			}
		}
		return true;
	}
	else
		return false;
	/*
	int idx;
	if (isOccupied(x, y)){
		idx = indexOfTurretList(x, y);
		if (idx != -1){
			turretList.erase(turretList.begin() + idx);
			sb->destroyTurret(floor[x][y], true);
			delete floor[x][y];
			floor[x][y] = NULL;
			return true;
		}
		else {
			cerr << "WARN: Unable to sell turret at " << x << ", " << y << endl;
			return false;
		}
	}
	else
		return false;
	*/
}

bool Floor::insideField(int x, int y){
	return x >= X +startX && x < X + startX + tileSize * battleFieldWidth && y >= Y + startY && y <= Y + startY + tileSize * battleFieldHeight;
}

pair<int, int> Floor::insideTile(int x, int y){
	pair<int, int> coord;
	coord.first = (x - startX - X) / tileSize + (((x - startX - X) < 0) ? -1 : 0);
	coord.second = (y - startY - Y) / tileSize + (((y - startY - Y) < 0) ? -1 : 0);
	return coord;
}

vector<Monster*>* Floor::getMonsterList(){
	return &monsterList;
}

int Floor::insideSelect(int x, int y){
	if (x < X + startX / 3 && x >= X && y < Y + startY && y > Y)
		return 0;
	else if (x < X + 2 * startX / 3 && x >= X && y < Y + startY && y > Y)
		return 1;
	else if (x < X + startX && x >= X + 2 * startX / 3 && y < Y + startY && y > Y)
		return 2;
	else return -1;
}

bool Floor::buyTurret(){
	deselectTurret();
	return true;
}

bool Floor::cancelTurret(){
	selectedType = -1;
	return true;
}

bool Floor::deselectTurret(){
	selectedTurret = NULL;
	sb->deselectTurret();
	return true;
}

bool Floor::move(int x, int y){
	mouseX = x;
	mouseY = y;
	return true;
}

bool Floor::click(int x, int y, int state){
	sb->error("");
	pair<int, int> coord = insideTile(x, y);
	//cout << "Mouse inside tile " << coord.first << ", " << coord.second << endl;
	if (splash){
		splash = false;
		return true;
	}
	if (state == 3){
		cancelTurret();
		deselectTurret();
	}
	if (insideField(x, y)){
		if (selectedType != -1){
			//cout << "built turret " << selectedType << " at " << coord.first << ", " << coord.second << endl;
			buildTurret(coord.first, coord.second, selectedType);
			selectedType = -1;
		}
		else if (isOccupied(coord.first, coord.second)){
			int idx = indexOfTurretList(coord.first, coord.second);
			if (idx != -1){
				if (turretList[idx] == selectedTurret)
					selectedTurret = NULL;
				else
					selectedTurret = turretList[idx];
				sb->selectTurret(turretList[idx]);
			}
		}
		else {
			deselectTurret();
		}
	}
	else if (insideSelect(x, y) != -1){
		selectedType = insideSelect(x, y);
		buyTurret();
		//cout << "selected turret " << selectedType << endl;
	}
	else if (coord.first >= 16 && coord.second <= 18 && coord.second >= -2 && coord.second <= -1)
		gameSpeed = coord.first - 15;
	else 
		deselectTurret();
	return true;
}

bool Floor::press(int kc){
    sb->error("");
	if (kc >= 10 && kc <= 12){
		selectedType = kc - 10;
		buyTurret();
	}
	else if (kc == 39)
		destroyTurret();
	else if (kc == 25)
		upgradeTurret();
	else if (kc == 33){
		if (win){
			win = false;
			count = 1000;
			nextWave();
		}
		if (gameSpeed == 0){
			gameSpeed = prevGameSpeed;
		}
		else {
			if (gameSpeed != 0)
				prevGameSpeed = gameSpeed;
			gameSpeed = 0;
		}
	}
	return true;
}

int Floor::nextWave(){
	if (count == 500 || count % 1000 == 0){
		paid = false;
		sb->nextWave();
		wave++;
		left2Spawn = 3 * wave + 17;
		for (int x = 0; x < monsterList.size(); x++)
			delete monsterList[x];
		monsterList.clear();
		//cout <<"new wave: "<< left2Spawn << ","<<wave<<","<<count<<endl;
		this->spawnMonster();
	}
	return wave;
}

bool Floor::spawnMonster(){
	int delay = 500 - wave * 5;
	if (delay < 100)
		delay = 100;
	if (count % delay == 0 && left2Spawn > 0){
		//cout<<"spawn monster wave "<<wave<<" monsters left "<<left2Spawn<<endl;
		left2Spawn--;
		int speed = (int)(1000 / (double)(4 + 0.2 * (double)wave));
		if (speed < 50)
			speed = 50;
		monsterList.push_back(new Monster(seed + wave, path[0].first, path[0].second, wave * wave * 20, speed, wave, X + startX, Y + startY, tileSize, turningPoints, sb));
		return true;
	}
	return false;
}

bool Floor::allDead(){
	for (int x = 0; x < monsterList.size(); x++)
		if (!monsterList[x]->dead())
			return false;
	return true;
}

//return true if you lost the game
bool Floor::tick(){
	if (splash)
		return false;
	count++;
	if (count == 500)	//Start the first wave after 2 seconds
		this->nextWave();
	else if (left2Spawn > 0 && wave > 0)
		spawnMonster();
	else if (wave > 0 && left2Spawn <= 0 && this->allDead()){
		if (!paid){
			sb->addGold(100 + wave * 10);
			paid = true;
		}
		if (wave == 25 && !win){
			win = true;
			if (gameSpeed != 0)
				prevGameSpeed = gameSpeed;
			gameSpeed = 0;
		}
		this->nextWave();
	}
	for (int x = 0; x < turretList.size(); x++)
		turretList[x]->tick();
	for (int x = 0; x < monsterList.size(); x++)
		if (monsterList[x]->tick())
			if (sb->addLives(-1))
				return true;
	return false;
}

void Floor::gameOver(){
	over = true;
	gameSpeed = 0;
	prevGameSpeed = gameSpeed;
	sb->gameOver();
}

bool Floor::draw(){
	stringstream ss;
    if (gameSpeed == 0)
        sb->error("Game paused. Press p to resume.");
	if (over){
		sb->draw();
	}
	else if (splash){
		int a = 5, b = 5, ratio = 15;
		if (a < 5)
			a = 5;
		if (b < 5)
			b = 5;
		sb->print(a, b, "Name: Yuan Zhi Lin", 0);
		sb->print(a, b + 1 * ratio, "Userid: yzlin", 0);
		sb->print(a, b + 3 * ratio, "Objective: stop monsters going from top left to bottom right for 25 waves.", 0);
		sb->print(a, b + 4 * ratio, "How to play: Click on one of the three tower icons on the top left part of the screen", 0);
		sb->print(a, b + 5 * ratio, "then click on any tile on the grid to place the tower.", 0);
		sb->print(a, b + 6 * ratio, "Click on a placed tower will select it and allows you to upgrade or sell the tower using hotkeys w or s.", 0);
		sb->print(a, b + 7 * ratio, "Right click or clicking on empty space will cancel selection.", 0);
		sb->print(a, b + 8 * ratio, "Game Speed Setting: click on 1x, 2x or 3x on top right part of the screen to changes the game speed.", 0);
		sb->print(a, b + 9 * ratio, "                    Note that higher speed will use more CPU but framerate is the same.", 0);
		sb->print(a, b + 10 * ratio, "Hotkeys: 1-3 - buy tower 1-3", 0);
		sb->print(a, b + 11 * ratio, "         w   - upgrade selected tower", 0);
		sb->print(a, b + 12 * ratio, "         s   - sell selected tower", 0);
		sb->print(a, b + 13 * ratio, "         p   - pause or resume game", 0);
		sb->print(a, b + 15 * ratio, "Click anywhere to start the game.", 6);
	}
	else if (win){
		int a = xInfo.width / 2 - 290, b = xInfo.height / 2 - 10;
		if (a < 5)
			a = 5;
		if (b < 5)
			b = 5;
		ss << "Congratulations! You win with a score of " << sb->addScore(0) << "! Press p to continue playing in unlimited wave mode.";
		sb->print(a, b, ss.str(), 6);
	}
	else if (xInfo.width >= 600 && xInfo.height >= 360){
		//XFillRectangle(xInfo.display, xInfo.pixmap, xInfo.gc[7], startX, startY, xInfo.width - startX, xInfo.height - startY * 2);
		for (int x = 1; x < turningPoints.size(); x++){
			if (turningPoints[x - 1].direction == 'E')
				XFillRectangle(xInfo.display, xInfo.pixmap, xInfo.gc[17], X + startX + tileSize * turningPoints[x - 1].x, Y + startY + tileSize * turningPoints[x - 1].y, (turningPoints[x].x - turningPoints[x - 1].x + 1) * tileSize, tileSize);
			else if (turningPoints[x - 1].direction == 'W')
				XFillRectangle(xInfo.display, xInfo.pixmap, xInfo.gc[17], X + startX + tileSize * turningPoints[x].x, Y + startY + tileSize * turningPoints[x - 1].y, (turningPoints[x - 1].x - turningPoints[x].x + 1) * tileSize, tileSize);
			else if (turningPoints[x - 1].direction == 'N')
				XFillRectangle(xInfo.display, xInfo.pixmap, xInfo.gc[17], X + startX + tileSize * turningPoints[x - 1].x, Y + startY + tileSize * turningPoints[x].y, tileSize, (turningPoints[x - 1].y - turningPoints[x].y + 1) * tileSize);
			else if (turningPoints[x - 1].direction == 'S')
				XFillRectangle(xInfo.display, xInfo.pixmap, xInfo.gc[17], X + startX + tileSize * turningPoints[x - 1].x, Y + startY + tileSize * turningPoints[x - 1].y, tileSize, (turningPoints[x].y - turningPoints[x - 1].y + 1) * tileSize);
		}
		//Draw grid
		for (int x = 1; x < 20; x++)
			XDrawLine(xInfo.display, xInfo.pixmap, xInfo.gc[14], X + startX + x * tileSize, Y + startY, X + startX + x * tileSize, Y + startY + tileSize * 15);
		for (int x = 1; x < 15; x++)
			XDrawLine(xInfo.display, xInfo.pixmap, xInfo.gc[14], X + startX, Y + startY + x * tileSize, X + startX + 20 * tileSize, Y + startY + x * tileSize);
	
		for (int x = 0; x < turretList.size(); x++)
			turretList[x]->draw(xInfo);
		for (int x = 0; x < monsterList.size(); x++)
			monsterList[x]->draw(xInfo);

		sb->draw();
		if (selectedType == 0){
			XFillRectangle(xInfo.display, xInfo.pixmap, xInfo.gc[8], mouseX - tileSize / 2 + 3, mouseY - tileSize / 2 + 3, tileSize - 2 * ((tileSize - 1) / 10 + 1) - 2, tileSize - 2 * ((tileSize - 1) / 10 + 1) - 2);
			XSetLineAttributes(xInfo.display, xInfo.gc[19], tileSize / 8 + 1, LineSolid, CapButt, JoinRound);
			XDrawLine(xInfo.display, xInfo.pixmap, xInfo.gc[19], mouseX, mouseY, mouseX, mouseY - tileSize / 2 + 1);
			XDrawArc(xInfo.display, xInfo.pixmap, xInfo.gc[8], mouseX - 3 * tileSize, mouseY - 3 * tileSize, 6 * tileSize, 6 * tileSize, 0, 360 * 64);
		}
		else if (selectedType == 1){
			XFillArc(xInfo.display, xInfo.pixmap, xInfo.gc[3], mouseX + 1 - tileSize / 2, mouseY + 1  - tileSize / 2, tileSize - (tileSize - 1) / 10 - 1, tileSize - (tileSize - 1) / 10 - 1, 0, 360 * 64);
			XFillArc(xInfo.display, xInfo.pixmap, xInfo.gc[4], mouseX - tileSize / 3 + 1, mouseY - tileSize / 3 + 1, tileSize / 3 * 2 - 1, tileSize / 3 * 2 - 1, 0, 360 * 64);	
			XFillArc(xInfo.display, xInfo.pixmap, xInfo.gc[5], mouseX - tileSize / 4 + 1, mouseY - tileSize / 4 + 1, tileSize / 4 * 2 - 1, tileSize / 4 * 2 - 1, 0, 360 * 64);	
			XDrawArc(xInfo.display, xInfo.pixmap, xInfo.gc[4], mouseX - 2 * tileSize, mouseY - 2 * tileSize, 4 * tileSize, 4 * tileSize, 0, 360 * 64);
		}
		else if (selectedType == 2){
			XPoint xp[4];
			xp[0].x = mouseX - tileSize / 2 + (tileSize - 1) / 10;
			xp[0].y = mouseY;
			xp[1].x = mouseX;
			xp[1].y = mouseY - tileSize / 2 + (tileSize - 1) / 10;
			xp[2].x = mouseX + tileSize / 2 - (tileSize - 1) / 10 - 1;
			xp[2].y = mouseY;
			xp[3].x = mouseX;
			xp[3].y = mouseY + tileSize / 2 - (tileSize - 1) / 10 - 1;
			XFillPolygon(xInfo.display, xInfo.pixmap, xInfo.gc[11], xp, 4, Convex, CoordModeOrigin);
			XSetLineAttributes(xInfo.display, xInfo.gc[19], tileSize / 8 + 1, LineSolid, CapButt, JoinRound);
			XDrawLine(xInfo.display, xInfo.pixmap, xInfo.gc[19], mouseX, mouseY, mouseX, mouseY - tileSize / 2 + 1);
			XDrawArc(xInfo.display, xInfo.pixmap, xInfo.gc[11], mouseX - 5 * tileSize / 2, mouseY - 5 * tileSize / 2, 5 * tileSize, 5 * tileSize, 0, 360 * 64);
		}
		//Fill Outside
		XFillRectangle(xInfo.display, xInfo.pixmap, xInfo.gc[1], 0, 0, X, xInfo.height + 3);
		XFillRectangle(xInfo.display, xInfo.pixmap, xInfo.gc[1], 0, 0, xInfo.width + 3, Y);
		XFillRectangle(xInfo.display, xInfo.pixmap, xInfo.gc[1], X + tileSize * (battleFieldWidth + 10), 0, X + 1, xInfo.height + 3);
		XFillRectangle(xInfo.display, xInfo.pixmap, xInfo.gc[1], 0, Y + tileSize * (battleFieldHeight + 3),  xInfo.width + 3, Y + 1);
	}
	else {
		int a = xInfo.width / 2 - 100, b = xInfo.height / 2 - 10;
		if (a < 5)
			a = 5;
		if (b < 5)
			b = 5;
		sb->print(a, b, "Please make the window bigger than 600x360.", 0);
	}
	return true;
}
