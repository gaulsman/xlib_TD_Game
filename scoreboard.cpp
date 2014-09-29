#include <iostream>
#include <string>
#include <sstream>
#include "scoreboard.h"
#include "turret.h"
#include "monster.h"
#include "floor.h"

using namespace std;

Scoreboard::Scoreboard(int sx, int sy, int g, int l, int ts, XInfo &xinfo) : xInfo(xinfo){
	startX = sx;
	startY = sy;
	X = 0;
	Y = 0;
	gold = g;
	score = 0;
	wave = 0;
	lives = l;
	tileSize = ts;
	floor = NULL;
	selectedTurret = NULL;
	errorMessage = "";
	warnMessage = "";
	over = false;
	vector<vector<int> > t1, t2, t3;
	vector<int> vv1, vv2, vv3, vv4, vv5, vv6, vv7, vv8, vv9;
	int temp1[] = {100, 50, 480, 3 * tileSize};
	vv1.assign(temp1, temp1 + 4);
	int temp2[] = {125, 125, 390, 4 * tileSize};
	vv2.assign(temp2, temp2 + 4);
	int temp3[] = {200, 200, 300, 5 * tileSize};
	vv3.assign(temp3, temp3 + 4);
	int temp4[] = {150, 10, 160, 2 * tileSize};
	vv4.assign(temp4, temp4 + 4);
	int temp5[] = {200, 25, 125, (int)((double)2.5 * tileSize)};
	vv5.assign(temp5, temp5 + 4);
	int temp6[] = {300, 60, 100, 3 * tileSize};
	vv6.assign(temp6, temp6 + 4);
	int temp7[] = {200, 100, 900, (int)((double)2.5 * tileSize)};
	vv7.assign(temp7, temp7 + 4);
	int temp8[] = {275, 200, 750, 3 * tileSize};
	vv8.assign(temp8, temp8 + 4);
	int temp9[] = {400, 400, 600, (int)((double)3.5 * tileSize)};
	vv9.assign(temp9, temp9 + 4);
	t1.push_back(vv1);
	t1.push_back(vv2);
	t1.push_back(vv3);
	t2.push_back(vv4);
	t2.push_back(vv5);
	t2.push_back(vv6);
	t3.push_back(vv7);
	t3.push_back(vv8);
	t3.push_back(vv9);
	turretData.push_back(t1);
	turretData.push_back(t2);
	turretData.push_back(t3);
}

void Scoreboard::setTP(vector<triplet> tp){
	turningPoints = tp;
}

bool Scoreboard::resize(int x, int y, int ts, int sx, int sy){
	X = x;
	Y = y;
	tileSize = ts;
	startX = sx;
	startY = sy;
	return true;
}

//return true if no lives left
bool Scoreboard::addLives(int l){
	lives += l;
	return lives <= 0;
}

bool Scoreboard::addGold(int g){
	gold += g;
	return true;
}

int Scoreboard::addScore(int s){
	score += s;
	return score;
}

Turret* Scoreboard::buildTurret(int x, int y, int t, int l){
	//cout << "sb->buildTurret at" << x << ", " << y << endl; 
	if (gold >= turretData[t][l][0]){
		gold -= turretData[t][l][0];
		return new Turret(x, y, t, l, turretData[t][l][1], turretData[t][l][2], turretData[t][l][3], tileSize, X + startX, Y + startY, floor);
	}
	else {
		error("Not enough gold to build.");
		return NULL;
	}
}

bool Scoreboard::upgradeTurret(Turret *t){
	int type = t->whatType(), l = t->whatLV();
	if (l >= 2)
		warn("Tower already at max level.");
	else if (gold >= turretData[type][l + 1][0]){
		gold -= turretData[type][l + 1][0];
		t->upgrade(turretData[type][l + 1][1], turretData[type][l + 1][2], turretData[type][l + 1][3]);
		return true;
	}
	else 
		error("Not enough gold to upgrade.");
	return false;
}

bool Scoreboard::destroyTurret(Turret *t, bool refund){
	int type = t->whatType(), l = t->whatLV();
	selectedTurret = NULL;
	if (refund)
		for (int x = 0; x < l + 1; x++)
			gold += turretData[type][x][0] * 0.8;
	return true;
}

int Scoreboard::nextWave(){
	wave++;
	return wave;
}

void Scoreboard::warn(string m){
	 warnMessage = m;
	 errorMessage = "";
	 XFillRectangle(xInfo.display, xInfo.pixmap, xInfo.gc[1], X + startX + 1, Y + startY + 1, tileSize * battleFieldWidth, tileSize);
}

void Scoreboard::error(string m){
	errorMessage = m;
	warnMessage = "";
	XFillRectangle(xInfo.display, xInfo.pixmap, xInfo.gc[1], X + startX + 1, Y + startY + 1, tileSize * battleFieldWidth, tileSize);
}

void Scoreboard::print(int x, int y, string m, int i){
	//int *a, *ascent, *descent;
	//XCharStruct xcs;
	//XQueryTextExtents(xInfo.display, XGContextFromGC(xInfo.gc[i]), m.c_str(), m.length(), a, ascent, descent, &xcs);
	//Success getting font struct
	//XFontStruct *xfont = XQueryFont(xInfo.display, XGContextFromGC(xInfo.gc[i]));
    XDrawImageString(xInfo.display, xInfo.pixmap, xInfo.gc[i], x, y + 10, m.c_str(), m.length());
}

bool Scoreboard::selectTurret(Turret *t){
	//cout <<"sb select turret"<<endl;
	if (selectedTurret == t){
		t->showRangeIndicator = false;
		selectedTurret = NULL;
	}
	else {
		if (selectedTurret != NULL)
			selectedTurret->showRangeIndicator = false;
		t->showRangeIndicator = true;
		selectedTurret = t;
	}
	return true;
}

bool Scoreboard::deselectTurret(){
	if (selectedTurret != NULL)
		selectedTurret->showRangeIndicator = false;
	selectedTurret = NULL;
	return true;
}

void Scoreboard::gameOver(){
	over = true;
}


bool Scoreboard::draw(){
	stringstream ss;
	if (over){
		int a = xInfo.width / 2 - 100, b = xInfo.height / 2 - 10;
		if (a < 5)
			a = 5;
		if (b < 5)
			b = 5;
		ss << "Game Over! Your final score is " << score << "!";
		print(a, b, ss.str(), 0);
	}
	else {
		//Fill outline
		XFillRectangle(xInfo.display, xInfo.pixmap, xInfo.gc[1], X, Y, tileSize * (battleFieldWidth + 10), 2 * tileSize);
		XFillRectangle(xInfo.display, xInfo.pixmap, xInfo.gc[1], X, Y, tileSize * 10, (battleFieldHeight + 3) * tileSize);
		//Outline
		XDrawRectangle(xInfo.display, xInfo.pixmap, xInfo.gc[0], X, Y, tileSize * 30 - 1, tileSize * 18 - 1);
		//Horizontal lines
		XDrawLine(xInfo.display, xInfo.pixmap, xInfo.gc[0], X, Y + startY, X + startX + tileSize * 20, Y + startY);
		XDrawLine(xInfo.display, xInfo.pixmap, xInfo.gc[0], X, Y + startY + 8 * tileSize, X + startX, Y + startY + 8 * tileSize);
		XDrawLine(xInfo.display, xInfo.pixmap, xInfo.gc[0], X + startX, Y + startY + tileSize * 15, X + startX + tileSize * 20, Y + startY + tileSize * 15);
		//verticle line
		XDrawLine(xInfo.display, xInfo.pixmap, xInfo.gc[0], X + startX, Y, X + startX, Y + tileSize * 18 - 1);
		//verticle lines on top bar
		XDrawLine(xInfo.display, xInfo.pixmap, xInfo.gc[0], X + startX / 3, Y, X + startX / 3, Y + startY);
		XDrawLine(xInfo.display, xInfo.pixmap, xInfo.gc[0], X + 2 * startX / 3, Y, X + 2 * startX / 3, Y + startY);
		XDrawLine(xInfo.display, xInfo.pixmap, xInfo.gc[0], X + startX + 3 * tileSize, Y, X + startX + 3 * tileSize, Y + startY);
		XDrawLine(xInfo.display, xInfo.pixmap, xInfo.gc[0], X + startX + 7 * tileSize, Y, X + startX + 7 * tileSize, Y + startY);
		XDrawLine(xInfo.display, xInfo.pixmap, xInfo.gc[0], X + startX + 11 * tileSize, Y, X + startX + 11 * tileSize, Y + startY);
		XDrawLine(xInfo.display, xInfo.pixmap, xInfo.gc[0], X + startX + 14 * tileSize, Y, X + startX + 14 * tileSize, Y + startY);
		//Button outline
		XDrawRectangle(xInfo.display, xInfo.pixmap, xInfo.gc[18], X + startX + 16 * tileSize + 5, Y + tileSize - 10, 20, 20);
		XDrawRectangle(xInfo.display, xInfo.pixmap, xInfo.gc[18], X + startX + 17 * tileSize + 5, Y + tileSize - 10, 20, 20);
		XDrawRectangle(xInfo.display, xInfo.pixmap, xInfo.gc[18], X + startX + 18 * tileSize + 5, Y + tileSize - 10, 20, 20);

		//Game status
		ss << "Lives: " << lives;
		print(X + startX + 5, Y + tileSize - 5, ss.str(), 0);
		ss.str("");
		ss.clear();
		ss << "Gold: $" << gold;
		print(X + startX + 3 * tileSize + 5, Y + tileSize - 5, ss.str(), 0);
		ss.str("");
		ss.clear();
		ss << "Score: " << score;
		print(X + startX + 7 * tileSize + 5, Y + tileSize - 5, ss.str(), 0);
		ss.str("");
		ss.clear();
		ss << "Waves: " << wave;
		print(X + startX + 11 * tileSize + 5, Y + tileSize - 5, ss.str(), 0);
		//Speed buttons
		print(X + startX + 14 * tileSize + 5, Y + tileSize - 5, string("Speed:"), 0);
		print(X + startX + 16 * tileSize + 10, Y + tileSize - 5, string("1x"), 0);
		print(X + startX + 17 * tileSize + 10, Y + tileSize - 5, string("2x"), 0);
		print(X + startX + 18 * tileSize + 10, Y + tileSize - 5, string("3x"), 0);

		print(X + startX + 5, Y + startY + 15 * tileSize + 5, warnMessage, 4);
		print(X + startX + 5, Y + startY + 15 * tileSize + 5, errorMessage, 6);

		if (selectedTurret != NULL){
			vector<int> ti = selectedTurret->printInfo();
			string turretName = "", descrpition = "";
			int sellPrice = 0;
			for (int x = 0; x < ti[1] + 1; x++)
				sellPrice += turretData[ti[0]][x][0] * 0.8;
			stringstream ss;
			if (ti[0] == 0){
				turretName = "Cannon Tower";
				descrpition = "None.";
			}
			else if (ti[0] == 1){
				turretName = "Laser Sphere";
				descrpition = "Attack multiple enemy.";
			}
			else if (ti[0] == 2){
				turretName = "Ice Tower";
				descrpition = "Slows enemy.";
			}
			//Turret status
			ss << "Name: " << turretName;
			print(X + 5, Y + startY + 5, ss.str(), 0);
			print(X + 5, Y + startY + 8 * tileSize + 5, ss.str(), 0);
			ss.str("");
			ss.clear();
			if (ti[1] >= 2)
				ss << "Level: max";
			else
				ss << "Level: " << ti[1] + 1;
			print(X + 5, Y + startY + tileSize + 5, ss.str(), 0);
			ss.str("");
			ss.clear();
			ss << "Attack: " << ti[2];
			print(X + 5, Y + startY + 2 * tileSize + 5, ss.str(), 0);
			ss.str("");
			ss.clear();
			ss << "Rate of Fire: " << (double)((int)((double)ti[3] / 5)) / 100;
			print(X + 5, Y + startY + 3 * tileSize + 5, ss.str(), 0);
			ss.str("");
			ss.clear();
			ss << "Range: " << (double)((int)((double)(ti[4] - 15) * 10 / 3)) / 100;
			print(X + 5, Y + startY + 4 * tileSize + 5, ss.str(), 0);
			ss.str("");
			ss.clear();
			ss << "Sell Price: $" << sellPrice;
			print(X + 5, Y + startY + 5 * tileSize + 5, ss.str(), 0);
			ss.str("");
			ss.clear();
			ss << "Special: " << descrpition;
			print(X + 5, Y + startY + 6 * tileSize + 5, ss.str(), 0);
			print(X + 5, Y + startY + 14 * tileSize + 5, ss.str(), 0);
			//Upgrade info
			ss.str("");
			ss.clear();
			if (ti[1] + 1 >= 2)
				ss << "Level: max";
			else 
				ss << "Level: " << ti[1] + 2;
			print(X + 5, Y + startY + 9 * tileSize + 5, ss.str(), 0);
			ss.str("");
			ss.clear();
			ss << "Attack: " << turretData[ti[0]][(ti[1] + 1 >= 2) ? 2 : ti[1] + 1][1];
			print(X + 5, Y + startY + 10 * tileSize + 5, ss.str(), 0);
			ss.str("");
			ss.clear();
			ss << "Rate of Fire: " << (double)((int)((double)(turretData[ti[0]][(ti[1] + 1 >= 2) ? 2 : ti[1] + 1][2]) / 5)) / 100;
			print(X + 5, Y + startY + 11 * tileSize + 5, ss.str(), 0);
			ss.str("");
			ss.clear();
			ss << "Range: " << (double)((int)((double)(turretData[ti[0]][(ti[1] + 1 >= 2) ? 2 : ti[1] + 1][3] - 15) * 10 / 3)) / 100;
			print(X + 5, Y + startY + 12 * tileSize + 5, ss.str(), 0);
			ss.str("");
			ss.clear();
			ss << "Upgrade Price: $" << turretData[ti[0]][(ti[1] + 1 >= 2) ? 2 : ti[1] + 1][0];
			print(X + 5, Y + startY + 13 * tileSize + 5, ss.str(), 0);
		}
		else {
			stringstream ss;
			ss << "Name: ";
			print(X + 5, Y + startY + 5, ss.str(), 0);
			print(X + 5, Y + startY + 8 * tileSize + 5, ss.str(), 0);
			ss.str("");
			ss.clear();
			ss << "Level: ";
			print(X + 5, Y + startY + tileSize + 5, ss.str(), 0);
			ss.str("");
			ss.clear();
			ss << "Attack: ";
			print(X + 5, Y + startY + 2 * tileSize + 5, ss.str(), 0);
			ss.str("");
			ss.clear();
			ss << "Rate of Fire: ";
			print(X + 5, Y + startY + 3 * tileSize + 5, ss.str(), 0);
			ss.str("");
			ss.clear();
			ss << "Range: ";
			print(X + 5, Y + startY + 4 * tileSize + 5, ss.str(), 0);
			ss.str("");
			ss.clear();
			ss << "Sell Price: ";
			print(X + 5, Y + startY + 5 * tileSize + 5, ss.str(), 0);
			ss.str("");
			ss.clear();
			ss << "Special: ";
			print(X + 5, Y + startY + 6 * tileSize + 5, ss.str(), 0);
			print(X + 5, Y + startY + 14 * tileSize + 5, ss.str(), 0);
			//Upgrade info
			ss.str("");
			ss.clear();
			ss << "Level: ";
			print(X + 5, Y + startY + 9 * tileSize + 5, ss.str(), 0);
			ss.str("");
			ss.clear();
			ss << "Attack: ";
			print(X + 5, Y + startY + 10 * tileSize + 5, ss.str(), 0);
			ss.str("");
			ss.clear();
			ss << "Rate of Fire: ";
			print(X + 5, Y + startY + 11 * tileSize + 5, ss.str(), 0);
			ss.str("");
			ss.clear();
			ss << "Range: ";
			print(X + 5, Y + startY + 12 * tileSize + 5, ss.str(), 0);
			ss.str("");
			ss.clear();
			ss << "Upgrade Price: ";
			print(X + 5, Y + startY + 13 * tileSize + 5, ss.str(), 0);
		}
		//Turret 1 icon
		XFillRectangle(xInfo.display, xInfo.pixmap, xInfo.gc[8], X + (tileSize - 1) / 10 + 3, Y + (tileSize - 1) / 10 + 3, tileSize - 2 * ((tileSize - 1) / 10 + 1) - 2, tileSize - 2 * ((tileSize - 1) / 10 + 1) - 2);
		XSetLineAttributes(xInfo.display, xInfo.gc[19], tileSize / 8 + 1, LineSolid, CapButt, JoinRound);
		XDrawLine(xInfo.display, xInfo.pixmap, xInfo.gc[19], X  + tileSize / 2, Y + tileSize / 2, X + tileSize / 2, Y + 2);
		//Turret 2 icon
		XFillArc(xInfo.display, xInfo.pixmap, xInfo.gc[3], X + startX / 3 + 1, Y + 1, tileSize - (tileSize - 1) / 10 - 1, tileSize - (tileSize - 1) / 10 - 1, 0, 360 * 64);
		XFillArc(xInfo.display, xInfo.pixmap, xInfo.gc[4], X + startX / 3 + tileSize / 4 - (tileSize - 1) / 10 + 1, Y + tileSize / 4 - (tileSize - 1) / 10 + 1, tileSize / 3 * 2 - 1, tileSize / 3 * 2 - 1, 0, 360 * 64);	
		XFillArc(xInfo.display, xInfo.pixmap, xInfo.gc[5], X + startX / 3 + tileSize / 3 - (tileSize - 1) / 10 + 1, Y + tileSize / 3 - (tileSize - 1) / 10 + 1, tileSize / 4 * 2 - 1, tileSize / 4 * 2 - 1, 0, 360 * 64);	
		//Turret 3 icon
		XPoint xp[4];
		xp[0].x = X + 2 * startX / 3 + (tileSize - 1) / 10 + 2;
		xp[0].y = Y + tileSize / 2;
		xp[1].x = X + 2 * startX / 3 + tileSize / 2;
		xp[1].y = Y + (tileSize - 1) / 10 + 2;
		xp[2].x = X + 2 * startX / 3 + tileSize - (tileSize - 1) / 10 - 1;
		xp[2].y = Y + tileSize / 2;
		xp[3].x = X + 2 * startX / 3 + tileSize / 2;
		xp[3].y = Y + tileSize - (tileSize - 1) / 10 - 1;
		XFillPolygon(xInfo.display, xInfo.pixmap, xInfo.gc[11], xp, 4, Convex, CoordModeOrigin);
		XSetLineAttributes(xInfo.display, xInfo.gc[19], tileSize / 8 + 1, LineSolid, CapButt, JoinRound);
		XDrawLine(xInfo.display, xInfo.pixmap, xInfo.gc[19], X  + 2 * startX / 3+ tileSize / 2, Y + tileSize / 2, X + 2 * startX / 3 + tileSize / 2, Y + 2);
		//Turret price
		ss.str("");
		ss.clear();
		ss << "$100";
		print(X + 5, Y + tileSize + 5, ss.str(), 0);
		ss.str("");
		ss.clear();
		ss << "$150";
		print(X + startX / 3 + 5, Y + tileSize + 5, ss.str(), 0);
		ss.str("");
		ss.clear();
		ss << "$200";
		print(X + 2 * startX / 3 + 5, Y + tileSize + 5, ss.str(), 0);
	}
	return true;
}

