#include "monster.h"
#include "floor.h"
#include "scoreboard.h"
#include <cstdlib>
#include <iostream>

using namespace std;

triplet::triplet(int x, int y, char d){
	this->x = x;
	this->y = y;
	direction = d;
}

triplet::triplet(const triplet &t){
	x = t.x;
	y = t.y;
	direction = t.direction;
}

Monster::Monster(int t, int x, int y, int hp, int s, int g, int sx, int sy, int ts, vector<triplet> tp, Scoreboard *sbp) : turningPoints(tp){
	type = t;
	x = x;
	y = y;
	HP = hp;
	maxHP = hp;
	speed = s;
	gold = g;
	effect = 0; //1 = slows
	effectMag = 0;
	effectTime = 0;
	corner = 0;
	startX = sx;
	startY = sy;
	count = 0;
	tileSize = ts;
	sb = sbp;
	pixelX = (x + 1) * tileSize - (tileSize + 1) / 2;
	pixelY = (y + 1) * tileSize - (tileSize + 1) / 2;

}

bool Monster::inTile(int tx, int ty){
	return tx * tileSize <= pixelX && pixelX <= (tx + 1) * tileSize - 1 &&
		   ty * tileSize <= pixelY && pixelY <= (ty + 1) * tileSize - 1;
}

pair<int, int> Monster::location(){
	return make_pair(pixelX, pixelY);
}

bool Monster::resize(int padX, int padY, int ts){
	count = count * ts / tileSize;
	pixelX = (int)((double)pixelX / tileSize * ts);
	pixelY = (int)((double)pixelY / tileSize * ts);
	tileSize = ts;
	startX = padX;
	startY = padY;
	return true;
}

bool Monster::damage(int d, int e){
	if (e == 6){
		effect = 1;
		if (effectMag < 40)
			effectMag = 40;
		if (effectTime < 500)
			effectTime = 500;
	}
	else if (e == 7){
		effect = 1;
		if (effectMag < 60)
			effectMag = 60;
		if (effectTime < 750)
			effectTime = 750;
	}
	else if (e == 8){
		effect = 1;
		if (effectMag < 80)
			effectMag = 80;
		if (effectTime < 1000)
			effectTime = 1000;
	}
	if (HP > 0){
		HP -=d ;
		if (HP <= 0){
			sb->addGold(gold);
			sb->addScore(gold);
		}
		return true;
	}
	return false;
}

// use type to randomize a shape, right most byte determine number of points in polygon + 3, second right most byte = colour
bool Monster::draw(XInfo &xInfo){
	if (HP <= 0)
		return false;
	srand(type);
	int numPoints = (rand() & 0xff) % 16, colour = (rand() & 0xff) % 16;
	XPoint xp[16];
	int ran = rand();
	//cout << "STARTX "<<startX << " pixelx " << pixelX << ", " <<(ran & 0xffff) % (tileSize / 2)<< "=" << (startX + pixelX - ((ran & 0xffff) % (tileSize / 2)))<< endl;
	for (int x = 0; x < numPoints / 4; x++){
		xp[x].x = startX + pixelX - ((rand() & 0xffff) % (tileSize / 2));
		xp[x].y = startY + pixelY - (((rand() >> 16) & 0xffff) % (tileSize / 2));
	}
	for (int x = 0; x < numPoints / 4 + (numPoints % 4 >= 3 ? 1 : 0); x++){
		xp[x + numPoints / 4].x = startX + pixelX + ((rand() & 0xffff) % (tileSize / 2));
		xp[x + numPoints / 4].y = startY + pixelY - (((rand() >> 16) & 0xffff) % (tileSize / 2));
	}

	for (int x = 0; x < numPoints / 4 + (numPoints % 4 >= 2 ? 1 : 0); x++){
		xp[x + numPoints / 4 + numPoints / 4 + (numPoints % 4 >= 3 ? 1 : 0)].x = startX + pixelX - ((rand() & 0xffff) % (tileSize / 2));
		xp[x + numPoints / 4 + numPoints / 4 + (numPoints % 4 >= 3 ? 1 : 0)].y = startY + pixelY + (((rand() >> 16) & 0xffff) % (tileSize / 2));
	}

	for (int x = 0; x < numPoints / 4 + (numPoints % 4 >= 1 ? 1 : 0); x++){
		xp[x + numPoints / 4 + numPoints / 4 + (numPoints % 4 >= 3 ? 1 : 0) + (numPoints % 4 >= 2 ? 1 : 0)].x = startX + pixelX + ((rand() & 0xffff) % (tileSize / 2));
		xp[x + numPoints / 4 + numPoints / 4 + (numPoints % 4 >= 3 ? 1 : 0) + (numPoints % 4 >= 2 ? 1 : 0)].y = startY + pixelY + (((rand() >> 16) & 0xffff) % (tileSize / 2));
	}
	if (colour == 15)
		colour = - 2;
	//cout << "monster at "<<xp[0].x << "," << xp[0].y << endl;
	XFillArc(xInfo.display, xInfo.pixmap, xInfo.gc[2 + colour], startX + pixelX - tileSize / 2, startY + pixelY - tileSize / 2, tileSize - (tileSize - 1) / 10 - 1, tileSize - (tileSize - 1) / 10 - 1, 0, 360 * 64);
	//XFillPolygon(xInfo.display, xInfo.pixmap, xInfo.gc[2 + colour], xp, numPoints, Convex, CoordModeOrigin);
	//Draw HP bar
	XFillRectangle(xInfo.display, xInfo.pixmap, xInfo.gc[6], startX + pixelX - tileSize / 2, startY + pixelY - tileSize / 2 - (tileSize - 1) / 10 - 1, tileSize - (tileSize - 1) / 10 - 1, (tileSize - 1) / 10 + 1);
	XFillRectangle(xInfo.display, xInfo.pixmap, xInfo.gc[8], startX + pixelX - tileSize / 2, startY + pixelY - tileSize / 2 - (tileSize - 1) / 10 - 1, (int)((double)HP / maxHP * (tileSize - (tileSize - 1) / 10 - 1)), (tileSize - 1) / 10 + 1);
	srand(type);
	return true;
}

bool Monster::dead(){
	return HP <= 0;
}

bool Monster::nextTurnYet(){
	char dir = turningPoints[corner].direction;
	int a = turningPoints[corner + 1].x, b = turningPoints[corner + 1].y;
	if (dir == 'N')
		return pixelY <= (b * tileSize + tileSize / 2);
	else if (dir == 'S')
		return pixelY >= (b * tileSize + tileSize / 2);
	else if (dir == 'W')
		return pixelX <= (a * tileSize + tileSize / 2);
	else if (dir == 'E')
		return pixelX >= (a * tileSize + tileSize / 2);
	else return false;
}


//return true means it reached the end
bool Monster::tick(){
	if (HP <= 0)
		return false;
	char dir = '\0';
	double modifier = 1;
	count++;
	if (effect == 1 && effectTime > 0)
		modifier = 1 + (double)effectMag / 100;
	else if (effectTime == 0){
		effect = 0;
		effectMag = 0;
	}
	effectTime--;
	if (count >= (int)((double)speed * modifier / tileSize)){
		if (this->nextTurnYet())
			corner++;
		if (corner >= turningPoints.size() - 1){
			HP = 0;
			return true;
		}
		dir = turningPoints[corner].direction;
		if (dir == 'N')
			pixelY--;
		else if (dir == 'S')
			pixelY++;
		else if (dir == 'W')
			pixelX--;
		else if (dir == 'E')
			pixelX++;
		count = 0;
	}
	return false;
}


