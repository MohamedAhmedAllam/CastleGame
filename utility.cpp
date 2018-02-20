#include "utility.h"
#include <vector>


/*A function to set the position of cursor on the screen*/
void gotoxy(int x, int y)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	_COORD pos;
	pos.X = x;
	pos.Y = y;

	SetConsoleCursorPosition(hConsole, pos);
}
/*A function to set the command window lenght and height for the game specification*/
void SetWindow()
{
	system("mode 150,50");   //Set mode to ensure window does not exceed buffer size
	SMALL_RECT WinRect = {0, 0, 150, 50};   //New dimensions for window in 8x12 pixel chars
	SMALL_RECT* WinSize = &WinRect;
	SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), true, WinSize);   //Set new size for window
}
/*A function to color the cmd text*/
void color(int thecolor)
{
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE); 
	SetConsoleTextAttribute(hStdout, 
		thecolor|FOREGROUND_INTENSITY);
}
/*A function to partition the castle into regions (A,B,C,D)*/
void DrawRegions(const castle & C)
{
	color(FOREGROUND_GREEN);
	gotoxy(0, C.Ystrt + C.L/2);
	for(int i=0;i<C.W+2*C.Xstrt;i++)
		cout<<"-";
	for(int j=0;j<C.L+2*C.Ystrt;j++)
	{
		gotoxy(C.Xstrt + C.W/2, j);
		cout<<"|";		
	}
	color(FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);
}
/*A function to draw the castle and the towers*/
void DrawCastle(const castle & C,int SimulationTime)
{
	system("CLS");
	//Draw castle
	cout<<endl;
	gotoxy(C.Xstrt,C.Ystrt);
	for(int i=0;i<C.W;i++)
		cout<<"=";
	for(int j=1;j<C.L; j++)
	{
		gotoxy(C.Xstrt, C.Ystrt +j);
		cout<<"|";
		gotoxy(C.Xstrt+C.W-1, C.Ystrt +j);
		cout<<"|";
	}
	gotoxy(C.Xstrt,C.Ystrt+C.L-1);
	for(int i=0;i<C.W;i++)
		cout<<"=";

	//Draw towers
	int T1X,T2X,T3X,T4X,T1Y,T2Y,T3Y,T4Y;
	for (int i=0;i<4;i++)
	{

		if(i==0)
		{
			T1X=C.Xstrt+1;
			T1Y=C.Ystrt + TowerLength;
			gotoxy(T1X, T1Y);
			for(int i=0;i<TowerWidth;i++)
				cout<<"=";
			gotoxy(T1X +1, T1Y-1);
			cout<<"T1"<<"("<<C.towers[0].Health<<")";

			gotoxy(C.Xstrt+C.W/4, C.Ystrt+C.L/4);
			cout<<"(A)";
		}
		else if(i==1)
		{
			T2X=C.Xstrt+C.W-2-TowerWidth;
			T2Y=T1Y;
			gotoxy(T2X,T2Y );
			for(int i=0;i<TowerWidth+1;i++)
				cout<<"=";
			gotoxy(T2X +1, T2Y-1);
			cout<<"T2"<<"("<<C.towers[1].Health<<")";
			gotoxy(C.Xstrt+(3*C.W/4), C.Ystrt+C.L/4);
			cout<<"(B)";
		}
		else if(i==2)
		{
			T3X=T2X;
			T3Y=C.Ystrt + C.L - TowerLength-1;;
			gotoxy(T3X,T3Y );
			for(int i=0;i<TowerWidth+1;i++)
				cout<<"=";
			gotoxy(T3X +1, T3Y+1);
			cout<<"T3"<<"("<<C.towers[2].Health<<")";
			gotoxy(C.Xstrt+(3*C.W/4), C.Ystrt+(3*C.L/4)-1);
			cout<<"(C)";

		}
		else 
		{
			T4X=T1X;
			T4Y=C.Ystrt + C.L - TowerLength-1;
			gotoxy(T4X, T4Y);
			for(int i=0;i<TowerWidth;i++)
				cout<<"=";
			gotoxy(T4X +1, T4Y+1);
			cout<<"T4"<<"("<<C.towers[3].Health<<")";
			gotoxy(C.Xstrt+(C.W/4), C.Ystrt+(3*C.L/4)-1);
			cout<<"(D)";

		}
	}
	DrawRegions(C);

	//****************************
	gotoxy(CmdWidth/2 -10, CmdHeight-1);
	cout<<"Simulation Time : " << SimulationTime<<endl;
}

/*A function to draw a single enemy using its distance from the castle*/
void DrawEnemy(const enemy& E, int Ypos)
{
	int x,y;
	if (E.Region==1)
	{
		x= CastleXStrt-E.Distance+1;
		y= (CmdHeight/2)-(CastleLength/4)-Ypos;
	}
	else if(E.Region==2)
	{
		x= CastleXStrt+CastleWidth+E.Distance-1;
		y= (CmdHeight/2)-(CastleLength/4)-Ypos;
	}
	else if(E.Region==3)
	{
		x= CastleXStrt+CastleWidth+E.Distance-1;
		y= (CmdHeight/2)+(CastleLength/4)+Ypos;
	}
	else
	{
		x= CastleXStrt-E.Distance+1;
		y= (CmdHeight/2)+(CastleLength/4)+Ypos;
	}
	char c=(char)219;
	gotoxy(x, y);
	if(E.Type==1)
	{

		color(FOREGROUND_RED);
		cout<<c;
	}
	else if (E.Type==2)
	{
		color(FOREGROUND_BLUE);
		cout<<c;
	}
	else 
	{
		color(FOREGROUND_RED|FOREGROUND_GREEN);
		cout<<c;
	}
	color(FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);
}

/*A function to draw a list of enemies and ensure there is no overflow in the drawing*/
void DrawEnemies(vector<enemy*> enemies, int size)
//void DrawEnemies(enemy* enemies[],int size)
{
	//draw enemies at each region 
	for(int region=1;region<=4;region++)
	{
		int CountEnemies =0;	//count Enenmeies at the same distance to draw them vertically if they are <= 15 enemy else print number of enemy in the region
		bool draw=true;
		for(int distance=((CmdWidth/2)-(CastleWidth/2));distance>1;distance--)
		{
			CountEnemies=0;

			for(int i=0;i<size;i++)
			{	
				if(enemies[i]->Distance==distance && enemies[i]->Region == region)
				{
					CountEnemies++;
				}
			}
			if(CountEnemies>15)
			{
				draw=false;
				break;
			}

		}
		if(draw)
		{
			for(int distance=((CmdWidth/2)-(CastleWidth/2));distance>1;distance--)
			{
				CountEnemies=0;

				for(int i=0;i<size;i++)
				{	
					if(enemies[i]->Distance==distance && enemies[i]->Region == region)
					{
						DrawEnemy(*(enemies[i]),CountEnemies);
						CountEnemies++;
					}
				}

			}


		}
		else // print message maximum reached in this region
		{
			int x;int y;
			if(region==1)
			{
				x= CastleXStrt-30;
				y= (CmdHeight/2)-(CastleLength/4);
			}
			else if(region==2)
			{
				x= CastleXStrt+CastleWidth+30;
				y= (CmdHeight/2)-(CastleLength/4);


			}
			else if(region==3)
			{
				x= CastleXStrt+CastleWidth+30;
				y= (CmdHeight/2)+(CastleLength/4);

			}
			else
			{
				x= CastleXStrt-30;
				y= (CmdHeight/2)+(CastleLength/4);

			}
			gotoxy(x, y);
			cout<<"Maximum limit";
		}

	}
	gotoxy(0, CmdHeight-1);
}

/*Student use this function to print his/her message*/
void PrintMsg(char*msg)
{
	cout << msg;
}