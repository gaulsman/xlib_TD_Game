#include <iostream>
#include "turret.h"
#include "floor.h"
#include "monster.h"
#include "projectile.h"

using namespace std;

const int maxProjectile = 100;

Turret::Turret(int x, int y, int t, int l, int atk, int rof, int r, int ts, int sx, int sy, Floor *fl){
	this->x = x;
	this->y = y;
	type = t;
	level = l;
	attack = atk;
	ROF = rof;
	range = r;	//in pixels
	angle = 90;
	charge = 0;
	tileSize = ts;
	startX = sx;
	startY = sy;
	floor = fl;
	showRangeIndicator = false;
}

Turret::~Turret(){
	for (int x = 0; x < projectileList.size(); x++)
		delete projectileList[x];
}

int Turret::whatType(){
	return type;
}

int Turret::whatLV(){
	return level;
}

pair<int, int> Turret::location(){
	return make_pair(x, y);
}

bool Turret::fire(){
	//if (projectileList.size() > 0)
	//	return false;//wrong
	vector<Projectile*> newProjList;
	int c = 0;
	if (type == 1)
		for (int x = 0; x < targetList.size(); x++)
			targetList[x]->damage(attack, 0);
	else if (type == 0 || type == 2){
		if (projectileList.size() >= maxProjectile){
			for (int x = 0; x < projectileList.size(); x++)
				if (projectileList[x]->active()){
					newProjList.push_back(projectileList[x]);
					c++;
				}
				else 
					delete projectileList[x];
			if (c == projectileList.size()){
				delete projectileList[0];
				newProjList.erase(newProjList.begin());
			}
			projectileList.clear();
			projectileList = newProjList;
		}
		projectileList.push_back(new Projectile(type * 3 + level, x * tileSize + tileSize / 2 + tileSize / 2 * cos((double) angle / 180 * PI), y * tileSize + tileSize / 2 - tileSize / 2 * sin((double) angle / 180 * PI),
			(int)((double)1000 / (level * 4 + 32)), attack, startX, startY, tileSize, angle, targetList[0]));

	}
	return true;
}

bool Turret::inrange(pair<int, int> p){
	int dx = (int)(x * tileSize + (double)tileSize / 2 - p.first), dy = (int)(y * tileSize + (double)tileSize / 2 - p.second);
	return (int)sqrt(dx * dx + dy * dy) <= range * tileSize / 30;
}

bool Turret::updateTargetList(){
	vector<Monster*> *monsterList = floor->getMonsterList();
	targetList.clear();
	for (int x = 0; x < monsterList->size(); x++)
		if (!(*monsterList)[x]->dead() && inrange((*monsterList)[x]->location()))
			targetList.push_back((*monsterList)[x]);
	
};

vector<int> Turret::printInfo(){
	vector<int> v;
	v.push_back(type);
	v.push_back(level);
	v.push_back(attack);
	v.push_back(ROF);
	v.push_back(range);
	return v;
}

void Turret::tick(){
	pair<int, int> monsterLoc;
	double temp;
	charge++;
	updateTargetList();
	if (targetList.size() > 0){
		if (type == 0 || type == 2){
			monsterLoc = targetList[0]->location();
			if (monsterLoc.first == x * tileSize + tileSize / 2){
				if (monsterLoc.second > y * tileSize + tileSize / 2)
					angle = 270;
				else angle = 90;
			} 
			else if (monsterLoc.second == y * tileSize + tileSize / 2){
				if (monsterLoc.first > x * tileSize + tileSize / 2)
					angle = 0;
				else angle = 180;
			}
			else {
				temp = atan((double)(y * tileSize + (double)tileSize / 2 - monsterLoc.second) / (double)(x * tileSize + (double)tileSize / 2 - monsterLoc.first));
				//cout << temp << endl;
				if (monsterLoc.second > y * tileSize + tileSize / 2 && monsterLoc.first > x * tileSize + tileSize / 2)
					angle = (int)((2 * PI - temp) / PI * 180) % 360;
				else if (monsterLoc.second >  y * tileSize + tileSize / 2 && monsterLoc.first < x * tileSize + tileSize / 2)
					angle = (int)((PI - temp) / PI * 180) % 360;
				else if (monsterLoc.second < y * tileSize + tileSize / 2 && monsterLoc.first > x * tileSize + tileSize / 2)
					angle = (int)((0 - temp) / PI * 180) % 360;
				else if (monsterLoc.second <  y * tileSize + tileSize / 2 && monsterLoc.first < x * tileSize + tileSize / 2)
					angle = (int)((PI - temp) / PI * 180) % 360;
			}
		}
		if (charge >= ROF){
			fire();
			charge = 0;
		}
	}
	for (int x = 0; x < projectileList.size(); x++)
		projectileList[x]->tick();

}

bool Turret::upgrade(int atk, int rof, int r){
	level++;
	attack = atk;
	ROF = rof;
	range = r;
	return true;
}

bool Turret::resize(int padX, int padY, int ts){
	//range = (int)((double)range / tileSize * ts);
	tileSize = ts;
	startX = padX;
	startY = padY;
	//cout << "turret resize " << padX << " " << padY << " " << ts << endl;
	for (int x = 0; x < projectileList.size(); x++)
		projectileList[x]->resize(padX, padY, ts);
	return true;
}

bool Turret::draw(XInfo &xInfo){
	//cout << angle << endl;
	int colourBase = 8;
	if (type == 1)
		colourBase = 4;
	else if (type == 2)
		colourBase = 11;
	if (type == 0){
		XFillRectangle(xInfo.display, xInfo.pixmap, xInfo.gc[colourBase + level], startX + x * tileSize + (tileSize - 1) / 10 + 3, startY + y * tileSize + (tileSize - 1) / 10 + 3, tileSize - 2 * ((tileSize - 1) / 10 + 1) - 2, tileSize - 2 * ((tileSize - 1) / 10 + 1) - 2);
		XSetLineAttributes(xInfo.display, xInfo.gc[19], tileSize / 8 + 1, LineSolid, CapButt, JoinRound);
		XDrawLine(xInfo.display, xInfo.pixmap, xInfo.gc[19], startX + x * tileSize + tileSize / 2, startY + y * tileSize + tileSize / 2, startX + x * tileSize + tileSize / 2 + tileSize / 2 * cos((double) angle / 180 * PI), startY + y * tileSize + tileSize / 2 - tileSize / 2 * sin((double) angle / 180 * PI));
	}
	else if (type == 1){
		XFillArc(xInfo.display, xInfo.pixmap, xInfo.gc[colourBase - 1 + level], startX + x * tileSize + 1, startY + y * tileSize + 1, tileSize - (tileSize - 1) / 10 - 1, tileSize - (tileSize - 1) / 10 - 1, 0, 360 * 64);
		XFillArc(xInfo.display, xInfo.pixmap, xInfo.gc[colourBase + level], startX + x * tileSize + tileSize / 4 - (tileSize - 1) / 10 + 1, startY + y * tileSize + tileSize / 4 - (tileSize - 1) / 10 + 1, tileSize / 3 * 2 - 1, tileSize / 3 * 2 - 1, 0, 360 * 64);	
		XFillArc(xInfo.display, xInfo.pixmap, xInfo.gc[colourBase + 1 + level], startX + x * tileSize + tileSize / 3 - (tileSize - 1) / 10 + 1, startY + y * tileSize + tileSize / 3 - (tileSize - 1) / 10 + 1, tileSize / 4 * 2 - 1, tileSize / 4 * 2 - 1, 0, 360 * 64);	
		for (int x = 0; x < targetList.size(); x++)
			XDrawLine(xInfo.display, xInfo.pixmap, xInfo.gc[colourBase + level], startX + this->x * tileSize + tileSize / 2, startY + this->y * tileSize + tileSize / 2, startX + targetList[x]->location().first, startY + targetList[x]->location().second);
	}
	else if (type == 2){
		XPoint xp[4];
		xp[0].x = startX + x * tileSize + (tileSize - 1) / 10 + 2;
		xp[0].y = startY + y * tileSize + tileSize / 2;
		xp[1].x = startX + x * tileSize + tileSize / 2;
		xp[1].y = startY + y * tileSize + (tileSize - 1) / 10 + 2;
		xp[2].x = startX + (x + 1) * tileSize - (tileSize - 1) / 10 - 1;
		xp[2].y = startY + y * tileSize + tileSize / 2;
		xp[3].x = startX + x * tileSize + tileSize / 2;
		xp[3].y = startY + (y + 1) * tileSize - (tileSize - 1) / 10 - 1;
		XFillPolygon(xInfo.display, xInfo.pixmap, xInfo.gc[colourBase + level], xp, 4, Convex, CoordModeOrigin);
		XSetLineAttributes(xInfo.display, xInfo.gc[19], tileSize / 8 + 1, LineSolid, CapButt, JoinRound);
		XDrawLine(xInfo.display, xInfo.pixmap, xInfo.gc[19], startX + x * tileSize + tileSize / 2, startY + y * tileSize + tileSize / 2, startX + x * tileSize + tileSize / 2 + tileSize / 2 * cos((double) angle / 180 * PI), startY + y * tileSize + tileSize / 2 - tileSize / 2 * sin((double) angle / 180 * PI));
	}
	if (showRangeIndicator){
		XDrawArc(xInfo.display, xInfo.pixmap, xInfo.gc[colourBase + level], startX + x * tileSize + tileSize / 2 - range * tileSize / 30, startY + y * tileSize + tileSize / 2 - range  * tileSize / 30, range * 2  * tileSize / 30, range * 2  * tileSize / 30, 0, 360 * 64);
	}
	for (int x = 0; x < projectileList.size(); x++)
		projectileList[x]->draw(xInfo);
	return true;
}


