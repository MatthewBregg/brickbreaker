#include <iostream>
#include <ncurses.h>
#include <string>
#include <cstdio>
#include <ctime>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <vector>
#include <cstdlib>
#include <time.h>
int brickRows = 5;
int maxX = 80;
int maxY = 40;
int ballSpeed = 125;

//Compile command: g++ -std=gnu++11 -O2 -Wall -o main main.cpp -lncurses -pthread
struct PLAT{
	int x;
	int y;
	std::string out;
	PLAT(){
		x = maxX/2;
		y = maxY-4;
		out = "<------>";
	}
};
struct ball{
	char out;
	int x;
	int y;
	bool up;
	bool forward;
	ball()
		{
			x = 2;
			y = 10;
			up = false;
			forward = true;
			out = 'O';

		}

};
struct brick{
	int x;
	int color;

	int y;
	int length;
	std::string out;

	brick()
		{
			x = 0;
			y = 0;
			length = 0;
			out = "";
			color= 0;	
			

		}
	brick(int xx, int yy, int l, int B )
		{


			

			out = "";	

			color = B;
			x = xx;
			y = yy;
			length = l;
			 for (int i = 0; i < l; ++i)
			 {
			 	out += "+";	
			
			 }

			
		}
};


void makeBorder()
{
	for (int i = 0; i < maxX; ++i)
	{
		mvaddch(maxY, i, '^');
	}

	for (int i = 0; i < maxX; ++i)
	{
		mvaddch(0, i, '-');
	}

	for (int i = 0; i <= maxY; ++i)
	{
		mvaddch(i, 0, '|');
	}

	for (int i = 0; i <= maxY; ++i)
	{
		mvaddch(i, maxX, '|');
	}
}
void ballBoundCheck(ball* m)
{
//returns true if the ball goes out of bounds
	
	if ( m->y >= maxY-1 )
	{
		m->out = '*';
		return;
	}
	if ( m-> x <= 1 )
	{
		m->forward = true;
	}
	if ( m-> x >= maxX-1)
	{
		m->forward = false;
	}
	if ( m->y <= 1)
	{
		m->up = false;
	}
		

		
	return;

}
bool ballOut(ball* m)
{

	if ( m->y >= maxY-1 )
	{
		m->out = '*';
		return true;
	}
	return false;
}
void moveBall(ball* m)
{
	ballBoundCheck(m);
	if (m->forward)
	{
		m->x++;
	}
	else
	{
		m->x--;
	}
	if (m->up)
	{
		m->y--;
	}
	else
	{
		m->y++;
	}
}
void drawPlat(PLAT* p)
{



		for (unsigned int i = 0; i < p->out.size(); ++i)
		{


		mvaddch(p->y, p->x+i, p->out[i]);
		}




}
bool movePlat(PLAT* p, int ch)
{



        if (ch == KEY_LEFT && p->x > 1)
        {
            p->x--;
            
			return true;
        }
        if (ch == KEY_RIGHT && (unsigned)p->x < maxX-p->out.size())
        {

            p->x++;
           
			return true;
        }
		return false;
}
void resetBall(ball* m)
{
	m->x = 2;
	m->y = 10;
	m->out = 'O';
	m-> forward =true;
	m->up = false;

}
void platBallCheck(ball* b, PLAT* p)
{
	if ( b->y == p-> y && b->x >= p->x && (unsigned)b->x <= p->x+p->out.size()/2)
	{
		b->up = true;
		b->forward = false;
	}
	if ( b->y == p-> y && (unsigned)b->x >= p->x+p->out.size()/2 && (unsigned)b->x <= p->x+p->out.size())
	{
		b->up = true;
		b ->forward = true;
	}

}
void genBricks(std::vector<brick>* bricks)
{
	int color = 2;
	srand(time(NULL));
	int y = brickRows; //There will be 5 rows of bricks
	for (int i = 1; i <= y; ++i)
	{
		int j = 1;
		while ( j < maxX-1)
		{

			if ( color >= 6 )
			{
				color = 1;
			}
			else
			{
				color++;
			}
			int temp;
			if (  maxX - j -1 <= 3 )
			{
					temp = maxX - j;		
					bricks->push_back(brick(j,i,temp, color));
					j += temp+2;
					break;

			}
			else
			{

				temp = rand() % 3 + 2;
			}
			bricks->push_back(brick(j,i,temp, color));
			j += temp;
		}
	}

}
void drawBricks(std::vector<brick>* bricks)
{


	init_pair(0, COLOR_WHITE, COLOR_BLACK);
	init_pair(1, COLOR_RED, 2);
	init_pair(2, COLOR_RED, 3);
	init_pair(3, COLOR_RED, 4);
	init_pair(4, COLOR_RED, 5);
	init_pair(5, COLOR_RED, 6);
	init_pair(6, COLOR_RED, 7);
	for (unsigned int j = 0; j < bricks->size(); ++j)
	{


	

		attron(COLOR_PAIR(bricks->at(j).color));

		// char *temp = new char[bricks->at(j).out.size()];
		// for (unsigned int i = 0; i < bricks->at(j).out.size(); ++i)
		// {

		// 	temp[i] = bricks->at(j).out[i];
		// }
		// mvprintw(bricks->at(j).y, bricks->at(j).x, temp );
		// delete[] temp;
		for (unsigned int i = 0; i < bricks->at(j).out.size(); ++i)
		{
			mvaddch(bricks->at(j).y, bricks->at(j).x+i, bricks->at(j).out[i]);
		}
		attroff(COLOR_PAIR(bricks->at(j).color));


		
	}

}
void brickCollide(std::vector<brick>* bricks, ball* b)
{
	if (b->y > brickRows+1)
	{
		return;
	}

	for (unsigned int j = 0; j < bricks->size(); ++j)
	{
		bool colision = false;
		// if ( bricks->at(j).y == b->y)
		// {
		// 	if (b->x == bricks->at(j).x && b->forward)
		// 	{
		// 		b->forward = false;
		// 		colision = true;
		// 	}
		// 	if (b->x == bricks->at(j).x+bricks->at(j).out.size() && !b->forward)
		// 	{
		// 		b->forward = true;

		// 		colision = true;
		// 	}
		// 	b->up=!b->up;

		// }
			
		if ( b->x >= bricks->at(j).x  && (unsigned)b->x <= bricks->at(j).x + bricks->at(j).out.size())
		{
			if (b->y == bricks->at(j).y+1  && b->up )
			{
				if ( !(b->x == bricks->at(j).x && !b->forward) || !((unsigned)b->x == bricks->at(j).x+bricks->at(j).out.size() && b->forward) )
				{
				b->up = !b->up;
				colision = true;
				}
			}
			if (b->y == bricks->at(j).y-1 && !b->up)
			{

				if ( !(b->x == bricks->at(j).x && !b->forward) || !((unsigned)b->x == bricks->at(j).x+bricks->at(j).out.size() && b->forward) )
				{
				b->up = !b->up;

				colision = true;
				}
			}
		}
		if (b->y == bricks->at(j).y)
		{
			if ( (b->x == bricks->at(j).x-1 && b->forward) || ((unsigned)b->x == bricks->at(j).x + bricks->at(j).out.size()+1 && !b->forward))
			{
				//b->up = !b->up;
				b->forward = !b->forward;
				colision = true;
			}
			

		}
		if (b->y == bricks->at(j).y+1 && b->up)
		{
			if ( b->x == bricks->at(j).x-1 && b->forward)
			{
				colision = true;
				b->forward = !b->forward;
				b->up = !b->up;
			}
			if ((unsigned)b->x == bricks->at(j).x+bricks->at(j).out.size()+1 && !b->forward)
			{

				colision = true;
				b->forward = !b->forward;
				b->up = !b->up;
			}
		}
			
		if (b->y == bricks->at(j).y-1 && !b->up)
		{
			if ( b->x == bricks->at(j).x-1 && b->forward)
			{
				colision = true;
				b->forward = !b->forward;
				b->up = !b->up;
			}
			if ((unsigned)b->x == bricks->at(j).x+bricks->at(j).out.size()+1 && !b->forward)
			{

				colision = true;
				b->forward = !b->forward;
				b->up = !b->up;
			}
		}
		

		if (colision)
		{
			bricks->at(j) = bricks->at(bricks->size()-1);
			bricks->pop_back();

			//	moveBall(b);
			//break;
		}
	}

}

void smoothBall(ball* BALL, PLAT* p, std::vector <brick>* bricks)
{
	while(true)
	{
		
	usleep(ballSpeed * 1000);
	moveBall(BALL);
	//Collision detection.
	brickCollide(bricks, BALL);	
	platBallCheck(BALL, p);
	}
}
int main()
{



	ball BALL;
	PLAT p;
    int ch; 
    initscr();
    curs_set(0);
    keypad(stdscr, TRUE);     
    noecho(); 
	start_color();
	int lives = 3;
	std::vector <brick> bricks;
	genBricks(&bricks);
	std::thread t1(smoothBall,&BALL,&p, &bricks);
	


    while (1)
    {

		
		drawPlat(&p);
        mvprintw(0, 81, " x is %d, y is %d, LIVES:%d", BALL.x, BALL.y, lives); 
	    //mvprintw(BALL.y, BALL.x, "%c", BALL.out ); 

		makeBorder();
		drawBricks(&bricks);
		
		if (ballOut(&BALL))
		{
			mvaddch(BALL.y, BALL.x, BALL.out);
			timeout(3000);
			getch();
			resetBall(&BALL);
			lives--;
		}
		else
		{
			mvaddch(BALL.y, BALL.x, BALL.out);
		}

	
		timeout(25); 
		
		ch = getch();
	
		if ( ch != ERR )
		{

    		movePlat(&p, ch);

			//usleep(125000);
	
		}
		
//		moveBall(&BALL);

//		std::this_thread::sleep_for(std::chrono::milliseconds(125));

		clear();
        refresh();
	
    }
    endwin();         
    return 0;
}    
