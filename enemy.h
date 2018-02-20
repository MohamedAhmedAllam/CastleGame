#pragma once

#include <iostream>
#include<stdio.h>
#include <windows.h>

using namespace std;

struct enemy
{
	//starting x,y
	int clock;

	int ID;
	int TS;
	float Health;
	int FP;
	int DP;
	int Type;
	int Region;
	int Distance;
	float Priority;
	//int Speed = 1;

	int firstShotTime = -1;
	int enemyKilledTime = -1;
	int FD = -1;
	int KD = -1;
	int FT = -1;

	void update_Clock(int new_clock) {
		clock = new_clock;
	}

	bool isKilled() {
		if (Health <= 0) { return TRUE; }
		else { return FALSE; }
	}

	bool becomeActive() {
		if (clock == TS) { return TRUE; }
		else { return FALSE; }
	}

	bool isActive() {
		if (clock >= TS && !isKilled()) { return TRUE; }
		else { return FALSE; }
	}

	void setPriority(float c1, float c2, float c3) {
		int timeToShoot = DP - (clock - TS) % DP;
		Priority =  1/(float)(c1*FP / (float)(Distance*(timeToShoot + 1)) + c2*Health + c3*Type);
	}

	int setDistance(int unpavedDistance) {
		if (!isActive()) { return unpavedDistance; }
		if (Type == 1) {
			if ((clock - TS) % DP == 0 && Distance == unpavedDistance) {
				Distance -= 1;
				unpavedDistance = Distance;
			}
			else if (Distance > unpavedDistance) {
				Distance -= 1;
			}
		}
		else if (Distance > unpavedDistance) {
			Distance -= 1;
		}
		if (Distance <= 2) {
			Distance = 2;
			unpavedDistance = Distance;
		}
		return unpavedDistance;
	}

	void setHealth(float damage) {
		Health = Health - damage;
	}

	void setFirstShotTime() {
		if (firstShotTime  == -1) { firstShotTime = clock; }
	}

	void setEnemyKilledTime() {
		if (enemyKilledTime == -1) { enemyKilledTime = clock; }
	}

	void setFD_KT_FT() {
		FD = firstShotTime - TS;
		KD = enemyKilledTime - firstShotTime;
		FT = KD + FD;
	}

	float damageToTower() {
		if ((clock - TS) % DP == 0) {
			return (float)FP / (float)Distance;
		}
		else { return 0; }
	}
};