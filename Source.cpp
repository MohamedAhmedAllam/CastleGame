#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include "enemy.h"
#include "utility.h"
#include <stdio.h>
#include "fibonacciEnemy.h"

using namespace std;

const int noOfTowers = 4;
const int initUnpavedDist = 30;
const int initEnemyDistance = 60;
int clock = -1;

int winOrLose(castle ct, vector<enemy*> &allEnemies, int number_of_towers, int number_of_enemies) {
	int towers_destroyed = 0;
	int dead_enemies = 0;
	for (int i = 0; i < number_of_towers; i++) {
		if (ct.towers[i].isDestroyed())
			towers_destroyed++;
	}

	for (int i = 0; i < number_of_enemies; i++) {
		if (allEnemies[i]->isKilled())
			dead_enemies++;
	}

	if (towers_destroyed == number_of_towers) {
		return -1; //lose
	}
	else if (dead_enemies == number_of_enemies) {
		return 1; //win
	}
	else {
		return 0; //game still playing
	}
}


vector<enemy*> readInputFile(castle &ct, string fileName) {

	vector<enemy*> allEnemies;

	float tower_health;

	string line;
	ifstream infile(fileName);
	int d = 1;
	if (infile.is_open()) {
		while (getline(infile, line)) {
			if (d == 1) { //get tower data
				tower_health = stof(line.substr(0, line.find('	')));
				line.erase(0, line.find('	') + 1);
				int tower_power, max_enemies;
				max_enemies = stoi(line.substr(0, line.find('	')));
				line.erase(0, line.find('	') + 1);
				tower_power = stoi(line);
				d++;

				for (int i = 0; i < noOfTowers; i++) {
					ct.towers[i].initHealth = tower_health;
					ct.towers[i].Health = tower_health;
					ct.towers[i].FP = tower_power;
					ct.towers[i].NE = max_enemies;
					ct.towers[i].unpavedDistance = initUnpavedDist;
					//ct.towers[i].Region == 1;
				}
			}
			else if (d == 2) { //get parameters
				ct.c1 = stof(line.substr(0, line.find('	')));
				line.erase(0, line.find('	') + 1);
				ct.c2 = stof(line.substr(0, line.find('	')));
				line.erase(0, line.find('	') + 1);
				ct.c3 = stof(line);
				d++;
			}
			else if (line.compare("-1") != 0) { //get enemies
				int seq = stoi(line.substr(0, line.find('	')));
				line.erase(0, line.find('	') + 1);
				int arrival_time = stoi(line.substr(0, line.find('	')));
				line.erase(0, line.find('	') + 1);
				float health = stof(line.substr(0, line.find('	')));
				line.erase(0, line.find('	') + 1);
				int power = stoi(line.substr(0, line.find('	')));
				line.erase(0, line.find('	') + 1);
				int delay_period = stoi(line.substr(0, line.find('	')));
				line.erase(0, line.find('	') + 1);
				int type = stoi(line.substr(0, line.find('	')));
				line.erase(0, line.find('	') + 1);
				char region = line[0];
				//int region = line[0];

				enemy* enem = new enemy;
				enem->ID = seq;
				enem->TS = arrival_time;
				enem->Health = health;
				enem->FP = power;
				enem->DP = delay_period;
				enem->Type = type;
				enem->Priority = NULL;
				switch (region) {
				case 'A':
					enem->Region = 1;
					break;
				case 'B':
					enem->Region = 2;
					break;
				case 'C':
					enem->Region = 3;
					break;
				case 'D':
					enem->Region = 4;
					break;
				default: cout << "ERROR unknown region" << endl;
					break;
				}
				enem->Distance = initEnemyDistance;

				allEnemies.push_back(enem);
			}
		}
		infile.close();
	}

	return allEnemies;
}

int main() {

	castle ct;
	ct.Xstrt = CastleXStrt;
	ct.Ystrt = CastleYStrt;
	ct.W = CastleWidth;
	ct.L = CastleLength;

	vector<enemy*> allEnemies = readInputFile(ct, "WeakCastle-ManyEnemies-InputF.txt");
	//vector<enemy*> allEnemies = readInputFile(ct, "Input_Sample_4.txt");

	int noOfEnemies = allEnemies.size();

	SetWindow();
	DrawCastle(ct, 12);
	DrawEnemies(allEnemies, noOfEnemies);

	while (winOrLose(ct, allEnemies, noOfTowers, noOfEnemies) == 0) {
	//while (clock < 100) {
		clock += 1;

		//Change the Region of Enemies of Dead Towers
		for (int i = 0; i < noOfTowers; i++) {
			if (ct.towers[i].isDestroyed()) {
				for (int j = 0; j < noOfEnemies; j++) {
					if (allEnemies[j]->isActive() && allEnemies[j]->Region == i + 1) {
						allEnemies[j]->Region = (allEnemies[j]->Region) % 4 + 1;
					}
				}
			}
		}

		//MAKE THE HEAPS FROM ACTIVE ENEMIES
		for (int i = 0; i < noOfEnemies; i++) {
			allEnemies[i]->update_Clock(clock);
			if (allEnemies[i]->isActive()) {
				ct.towers[allEnemies[i]->Region - 1].h.insert(allEnemies[i]);
			}
		}
		
		//setPriority and damage towers
		for (int i = 0; i < noOfEnemies; i++) {
			if (allEnemies[i]->isActive()) {
				allEnemies[i]->setPriority(ct.c1, ct.c2, ct.c3);
				float damageToTowerFromEnemy = allEnemies[i]->damageToTower();
				ct.towers[allEnemies[i]->Region-1].setHealth(damageToTowerFromEnemy);
			}
		}


		//damage enemies (IF TOWER IS ALLOWED TO SHOOT SAME ENEMY AGAIN)
		for (int i = 0; i < noOfTowers; i++) {
			if (!ct.towers[i].isDestroyed()) {
				for (int j = 0; j < ct.towers[i].NE; j++) {
					if (ct.towers[i].h.isEmpty()) { continue; }
					else {
						enemy* minEnemy = ct.towers[i].h.removeMinimum();
						minEnemy->setFirstShotTime();
						float damageToEnemyFromTower = ct.towers[i].damageToEnemy(minEnemy->Distance, minEnemy->Type);
						minEnemy->setHealth(damageToEnemyFromTower);
						if (minEnemy->isKilled()) { minEnemy->setEnemyKilledTime(); }
						else {
							minEnemy->setPriority(ct.c1, ct.c2, ct.c3);
							ct.towers[i].h.insert(minEnemy);
						}
					}
				}
			}
		}

		/*
		//damage enemies (IF TOWER IS ALLOWED ONLY TO SHOOT DISTINCT ENEMIES)
		for (int i = 0; i < noOfTowers; i++) {
			if (!ct.towers[i].isDestroyed()) {
				for (int j = 0; j < ct.towers[i].NE; j++) {
					if (ct.towers[i].h.isEmpty()) { continue; }
					else {
						enemy* minEnemy = ct.towers[i].h.removeMinimum();
						minEnemy->setFirstShotTime();
						float damageToEnemyFromTower = ct.towers[i].damageToEnemy(minEnemy->Distance, minEnemy->Type);
						minEnemy->setHealth(damageToEnemyFromTower);
						if (minEnemy->isKilled()) { minEnemy->setEnemyKilledTime(); }
					}
				}
			}
		}
		*/

		//Enemy Distance & Unpaved Distance
		int newUnpaved[4];
		for (int i = 0; i < noOfTowers; i++) {
			newUnpaved[i] = ct.towers[i].unpavedDistance;
		}

		for (int i = 0; i < noOfEnemies; i++) {
			if (allEnemies[i]->isActive()) {
				int newPropsedUnpaved = allEnemies[i]->setDistance(ct.towers[allEnemies[i]->Region - 1].unpavedDistance);
				if (newPropsedUnpaved < newUnpaved[allEnemies[i]->Region - 1]) { newUnpaved[allEnemies[i]->Region - 1] = newPropsedUnpaved; }
			}
		}

		for (int i = 0; i < noOfTowers; i++) {
			ct.towers[i].setUnpavedDistance(newUnpaved[i]);
		}


		//DELETE THE HEAPS
		for (int i = 0; i < noOfTowers; i++) {
			ct.towers[i].h.heap = ct.towers[i].h.empty();
		}

		//ReDraw
		DrawCastle(ct, 12);
		DrawEnemies(allEnemies, noOfEnemies);
		//Sleep(100);

	}

	//CALCULATE FD, KT, FT
	for (int i = 0; i < noOfEnemies; i++) {
		allEnemies[i]->setFD_KT_FT();
	}

	//SORT THE ENEMIES FOR PRINTING
	for (int i = 0; i < noOfEnemies; i++) {
		enemy* min = allEnemies[i];
		int r = i;
		for (int j = i + 1; j < noOfEnemies; j++) {
			if (allEnemies[j]->enemyKilledTime < min->enemyKilledTime) {
				min = allEnemies[j];
				r = j;
			}
		}
		allEnemies[r] = allEnemies[i];
		allEnemies[i] = min;
	}

	
	ofstream Outfile("Output_Sample.txt");

	if (Outfile.is_open())
	{
		for (int i = 0; i < noOfEnemies; i++) {
			auto enemy = allEnemies[i];
			Outfile << enemy->enemyKilledTime << ' ' << enemy->ID << ' ' << enemy->FD << ' ' << enemy->KD << ' ' << enemy->FT << '\n';
			cout << enemy->enemyKilledTime << ' ' << enemy->ID << ' ' << enemy->FD << ' ' << enemy->KD << ' ' << enemy->FT << '\n';
		}

		for (int i = 0; i < noOfTowers; i++) {
			cout << ct.towers[i].initHealth - ct.towers[i].Health << ' ';
			Outfile << ct.towers[i].initHealth - ct.towers[i].Health << ' ';
		}
		cout << endl;
		Outfile << endl;

		for (int i = 0; i < noOfTowers; i++) {
			cout << ct.towers[i].unpavedDistance << ' ';
			Outfile << ct.towers[i].unpavedDistance << ' ';
		}
		cout << endl;
		Outfile << endl;

		float av_FD = 0;
		float av_KD = 0;
		int count = 0;
		for (int i = 0; i < noOfEnemies; i++) {
			if (allEnemies[i]->isKilled()) {
				av_FD = (av_FD + allEnemies[i]->FD);
				av_KD = (av_KD + allEnemies[i]->KD);
				count++;
			}
		}
		av_FD = av_FD / (float)count;
		av_KD = av_KD / (float)count;

		if (winOrLose(ct, allEnemies, noOfTowers, noOfEnemies) == 1) {
			cout << "WIN" << endl;
			Outfile << "WIN" << endl;

			cout << noOfEnemies << endl;
			Outfile << noOfEnemies << endl;
		}
		else {
			cout << "LOSS" << endl;
			Outfile << "LOSS" << endl;

			cout << count << endl << noOfEnemies -count << endl;
			Outfile << count << endl << noOfEnemies - count << endl;

		}

		cout << av_FD << endl;
		Outfile << av_FD << endl;
		cout << av_KD << endl;
		Outfile << av_KD << endl;

		Outfile.close();
	}
	else cout << "Unable to open file";
	
	getchar();

	/*
	//Test the fiboHeap
	for (int i = 0; i < allEnemies.size(); i++) {
		h.insert(allEnemies[i]);
	}

	while (h.isEmpty() != 1) {
		enemy* R = h.removeMinimum();
		cout << R->Priority << endl;
	}
	*/
	/////////////////////////////////
	//use this function to print your messages
	
	/*
	PrintMsg("Total current eneimes  A = 0\tB = 0\tC = 0\tD = 0 \n");
	PrintMsg("Last time step kill    A = 0\tB = 0\tC = 0\tD = 0 \n");
	PrintMsg("Total killed by        A = 0\tB = 0\tC = 0\tD = 0 \n");
	PrintMsg("Un paved distance      A = 0\tB = 0\tC = 0\tD = 0 \n");
	*/

	//TEST damageToEnemy FUNCTION
	/*
	cout << ct.towers[0].damageToEnemy(20, 1) << endl;
	cout << ct.towers[1].damageToEnemy( 30, 2) << endl;
	cout << ct.towers[2].damageToEnemy( 45, 3) << endl;
	cout << ct.towers[3].damageToEnemy(10, 3) << endl;
	*/
}