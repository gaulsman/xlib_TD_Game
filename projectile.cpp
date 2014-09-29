#include "projectile.h"
#include "monster.h"
#include "floor.h"
#include "turret.h"
#include <iostream>
using namespace std;

Projectile::Projectile(int ty, int x, int y, int s, int atk, int sx, int sy, int ts, int ang, Monster *t){
	type = ty;
	pixelX = x;
	pixelY = y;
	speed = s;
	if (((int)cos((double)angle / 180 * PI)) == 0)
		speedX = 0;
	else 
		speedX = (int)(speed / (double)cos((double)angle / 180 * PI));
	if (((int)sin((double)angle / 180 * PI)) == 0)
		speedY = 0;
	else 
		speedY = (int)(speed / (double)sin((double)angle / 180 * PI));
	attack = atk;
	tileSize = ts;
	angle = ang;
	target = t;
	startX = sx;
	startY = sy;
	countx = 0;
	county = 0;
	live = true;
	//cout << "new proj "<<angle<<endl;
}

bool Projectile::resize(int padX, int padY, int ts){
	countx = countx * ts / tileSize;
	county = county * ts / tileSize;
	pixelX = (int)((double)pixelX / tileSize * ts);
	pixelY = (int)((double)pixelY / tileSize * ts);
	tileSize = ts;
	startX = padX;
	startY = padY;
	return true;
}


bool Projectile::draw(XInfo &xInfo){
	if (!live)
		return false;
	if (type >= 0 && type <= 2){
		XFillArc(xInfo.display, xInfo.pixmap, xInfo.gc[type + 8], startX + pixelX - ((tileSize - 1) / 10 + 1) - 2, startY + pixelY - ((tileSize - 1) / 10 + 1) - 2, (tileSize - 1) / 10 + 2, (tileSize - 1) / 10 + 2, 0, 360 * 64);
		//cout <<"drawing proj "<<startX + pixelX - ((tileSize - 1) / 10 + 1) - 2<<"," <<startY + pixelY - ((tileSize - 1) / 10 + 1) - 2 <<endl;
	}
	else if (type >= 6 && type <= 8){
		XDrawLine(xInfo.display, xInfo.pixmap, xInfo.gc[type + 5], startX + pixelX - (tileSize - 1) / 10, startY + pixelY - (tileSize - 1) / 10, startX + pixelX + (tileSize - 1) / 10, startY + pixelY + (tileSize - 1) / 10);
		XDrawLine(xInfo.display, xInfo.pixmap, xInfo.gc[type + 5], startX + pixelX + (tileSize - 1) / 10, startY + pixelY - (tileSize - 1) / 10, startX + pixelX - (tileSize - 1) / 10, startY + pixelY + (tileSize - 1) / 10);
		//XDrawLine(xInfo.display, xInfo.pixmap, xInfo.gc[type + 5], startX + pixelX, startY + pixelY - (tileSize - 1) / 10, startX + pixelX, startY + pixelY + (tileSize - 1) / 10);
		//XDrawLine(xInfo.display, xInfo.pixmap, xInfo.gc[type + 5], startX + pixelX - (tileSize - 1) / 10, startY + pixelY, startX + pixelX + (tileSize - 1) / 10, startY + pixelY);
	}

	return true;
}

bool Projectile::active(){
	return live;
}

void Projectile::updateHeading(){
	pair<int, int> monsterLoc = target->location();
	if (monsterLoc.first == pixelX){
		if (monsterLoc.second > pixelY)
			angle = 270;
		else angle = 90;
	} 
	else if (monsterLoc.second == pixelY){
		if (monsterLoc.first > pixelX)
			angle = 0;
		else angle = 180;
	}
	else {
		double temp = atan((double)(pixelY - monsterLoc.second) / (pixelX - monsterLoc.first));
		//cout << temp << endl;
		if (monsterLoc.second > pixelY && monsterLoc.first > pixelX)
			angle = (int)((2 * PI - temp) / PI * 180) % 360;
		else if (monsterLoc.second >  pixelY && monsterLoc.first < pixelX)
			angle = (int)((PI - temp) / PI * 180) % 360;
		else if (monsterLoc.second < pixelY && monsterLoc.first > pixelX)
			angle = (int)((0 - temp) / PI * 180) % 360;
		else if (monsterLoc.second <  pixelY && monsterLoc.first < pixelX)
			angle = (int)((PI - temp) / PI * 180) % 360;
	}
	//cout<<"angle "<<angle<<","<<monsterLoc.first<<","<<monsterLoc.second<<","<<pixelX<<","<<pixelY<<","<<endl;
}

bool Projectile::tick(){
	if (!live)
		return false;
	if (target->dead()){
		live = false;
		return false;
	}
	updateHeading();
	int speedx, speedy;
	if (cos((double)angle / 180 * PI) == 0)
		speedx = 0;
	else 
		speedx = (int)((double)speed / cos((double)angle / 180 * PI));
	if (sin((double)angle / 180 * PI) == 0)
		speedy = 0;
	else 
		speedy = (int)((double)speed / sin((double)angle / 180 * PI));
	//cout << speedx<<","<<speedy<<endl;
	if (speedx != 0){
		if (speedX != 0)
			countx = countx * abs(speedx) / abs(speedX);
		countx++; 
		if (countx >= abs(speedx) / tileSize){
			if (speedx > 0)
				pixelX++;
			else if (speedx < 0)
				pixelX--;
			countx = 0;
		}
	}
	speedX = speedx;
	if (speedy != 0){
		if (speedY != 0)
			county = county * abs(speedy) / abs(speedY);
		county++; 
		if (county >= abs(speedy) / tileSize){
			if (speedy > 0)
				pixelY--;
			else if (speedy < 0)
				pixelY++;
			county = 0;
		}
	}
	speedY = speedy;
	pair<int, int> monsterLoc = target->location();
	if (pixelX >= monsterLoc.first - 1 && pixelX <= monsterLoc.first + 1 &&
		pixelY >= monsterLoc.second - 1 && pixelY <= monsterLoc.second + 1){
		target->damage(attack, type);
		live = false;
		//cout <<"projectile hit monster"<<endl;
	}
	if (pixelX >= tileSize * battleFieldWidth || pixelX <= 0 || pixelY <= 0 || pixelY >= tileSize * battleFieldHeight){
		live = false;
		//cout <<"projectile out of field"<<endl;
	}
	
}

