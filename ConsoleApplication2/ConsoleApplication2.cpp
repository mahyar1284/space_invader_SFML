#include "pch.h"

// the speed of missile shooting animation (between 0 to 9)
#define speed 8
// the speed of enemy missile shooting animation (between 0 to 9)
#define e_speed 6

using namespace std;

int mouse_Xpos = 0, inv_x = 0, inv_y = 0; // saving mouse X position before shooting | invaders starting point x | invaders starting point y
bool shoot_the_missile = 0; // shoot command for our missle
bool dead_item[10] = {}; // this array is for dead invaders
bool shoot_now = 0; // a commmand to shoot the bullet
enum gp { yeah = 1, nope = 0 }Game_playing; // selecting between the game or menu
bool dont_shoot_for_a_moment = 1; // makes a delay for slowing down the first time invader shoot
bool e_visible = 0; // visiblity of invaders missile
int random = 0; // selects a random invader to shoot

sf::CircleShape main_shape(20.f);// user's spaceship
sf::CircleShape missile(5.f); // user missile (bullet)
sf::CircleShape e_missile(5.f); // missile (bullet) that enemy shoots !
sf::CircleShape invader[10]; // only 10 invaders (we can destroy them easily   LOL :\   )
sf::Text WON_txt; // when user win , this txt apeaiers on the screen

void main_game(); // main control thread of the game window
void my_shoot(); // this function (thread) moves our missile with animation
void invaders_pos(); // this function (thread) moves the enemies position at every 800 milli Secounds
void enemy_shoot(); // this function (thread) moves the enemies missile with animation just like "my_shoot()" function
bool is_win(); // this function checks win
void reset_game(); // this function resets the game by respawning all invaders and reseting starting point of invaders position !

int main()
{
	/////////// user's spaceship configuration ////////
	main_shape.setFillColor(sf::Color::Yellow);
	main_shape.setOrigin(20, 20); /// centering the origin
	main_shape.setPosition(200, 300);

	/////////// user's missile configuration //////////
	missile.setFillColor(sf::Color::White);
	missile.setOrigin(5, 5); /// centering the origin

	/////////// enemies missile configuration //////////
	e_missile.setFillColor(sf::Color::Cyan);
	e_missile.setOrigin(5, 5); /// centering the origin
	e_missile.setPosition(50, 50);


	/////////// initialazing the threads ///////////////
	thread main_game_task = thread(main_game);
	thread shoot_thread(my_shoot);
	thread invaders_pos_thread(invaders_pos);
	thread enemy_shoot_task(enemy_shoot);

	/////////// joining the threads ////////////////////
	main_game_task.join();
	shoot_thread.join();
	invaders_pos_thread.join();
	enemy_shoot_task.join();

}

void my_shoot()
{
	while (1)
	{
		
		if (shoot_the_missile) 
		{
			if (dont_shoot_for_a_moment) // it makes delay for enemy shooting function and if you starts shooting , for 
				shoot_the_missile = 0;  //  the first time , the delay for "enemies fist time shooting" will be destroyed
			int y = missile.getPosition().y;
			for (int i = missile.getPosition().y; i > 0; i--)
			{
				missile.setPosition(missile.getPosition().x, --(--y)); // --(--y) is for twisting the time for better shooting motion !
				Sleep(10 - speed); // slowing down the shooting animation
				if (!shoot_the_missile)
					break;
			}
			shoot_the_missile = 0;
		}
		if (missile.getPosition().y == 0)
			missile.setPosition(missile.getPosition().x,280);
	}
}

void main_game()
{
	srand((unsigned)time(0)); // for making a random integer

	sf::Font font;
	if (!font.loadFromFile("arial.ttf"));


	////////////////LOSE TEXT///////////////
	sf::Text LOST;
	LOST.setString("LOST !");
	LOST.setCharacterSize(72);
	LOST.setFillColor(sf::Color::Red);
	LOST.setFont(font);
	LOST.setPosition(70,200);

	////////////////WON TEXT////////////////
	WON_txt.setString("WON !");
	WON_txt.setCharacterSize(72);
	WON_txt.setFillColor(sf::Color::Green);
	WON_txt.setFont(font);
	WON_txt.setPosition(80, 200);

	///////////////START TEXT///////////////
	sf::Text Start_txt;
	Start_txt.setString("Start !");
	Start_txt.setCharacterSize(72);
	Start_txt.setFillColor(sf::Color::Cyan);
	Start_txt.setFont(font);
	Start_txt.setPosition(80, 100);

	////////////////EXIT TEXT///////////////
	sf::Text Exit_txt;
	Exit_txt.setString("Exit :(");
	Exit_txt.setCharacterSize(72);
	Exit_txt.setFillColor(sf::Color::Magenta);
	Exit_txt.setFont(font);
	Exit_txt.setPosition(80, 200);

	//////////////Mahyar's TEXT/////////////
	sf::Text dvlpr_name_txt;
	dvlpr_name_txt.setString("\t  this game is written and developed by Mahyar1284 \n \t\t\t\t\t\t only for learning purpose !");
	dvlpr_name_txt.setCharacterSize(16);
	dvlpr_name_txt.setFillColor(sf::Color::Magenta);
	dvlpr_name_txt.setFont(font);
	dvlpr_name_txt.setPosition(0, 32);

	sf::RenderWindow window(sf::VideoMode(400, 400), "space invader beta (Nightly version)"); // this is for rendering the main window

	for (int i = 0; i < 10; i++) // configuring the invaders specification and positions
	{
		invader[i].setRadius(20.f); // setting the invaders size(radius)
		invader[i].setFillColor(sf::Color::Magenta); // i love magenta color !
		invader[i].setOrigin(20, 20); // centering the origin of each invaders
		if(i<5)
		invader[i].setPosition((i+1)*50, 30); // invaders positioning formula
		else
		invader[i].setPosition((i-4) * 50, 80); // invaders positioning formula
	}

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event)) // configuring the exit event
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
				std::exit(0);
			}
		}
		if (event.type == sf::Event::MouseMoved) // configuring the mouse move event
		{
			main_shape.setPosition(event.mouseMove.x, 300);
			mouse_Xpos = event.mouseMove.x;
		}
		if (event.type == sf::Event::MouseButtonPressed) // configuring the mouse button pressed event
		{
			if (event.mouseButton.button == sf::Mouse::Left)
				shoot_the_missile = true; // if mouse left button pressed , shoot the missle
		}
		if (Game_playing==yeah) // game is playing (not in the menu)
		{
			if (is_win())
			{
				window.clear();
				window.draw(WON_txt); // drawing the "YOU WON" text
				window.display(); // displaying it
				Sleep(3000); //3 second delay for holding "YOU WON" text
				window.clear(); // deleting the "YOU WON" text
				window.display(); // displaying nothing
				Game_playing = nope; // ending the game to move back to the menu
			}
			////////////////////////setting the position of invaders//////////////////////
			for (int i = 0; i < 10; i++)
				if (i < 5)
					invader[i].setPosition(((i + 1) * 50) + inv_x, 30 + inv_y);
				else
					invader[i].setPosition(((i - 4) * 50) + inv_x, 80 + inv_y);
			//////////////////////////////////////////////////////////////////////////////

			if (!shoot_the_missile)
				missile.setPosition(mouse_Xpos, main_shape.getPosition().y - 20); // for moving the missile with users spaceship

			for (int i = 0; i < 10; i++) // this loop checks the invaders status (dead/alive)
			{
				if (i < 5)
				{
					if (missile.getPosition().x<(i * 50) + 70 + inv_x && missile.getPosition().x>(i * 50) + inv_x + 30 && missile.getPosition().y - 20 < 50 + inv_y && missile.getPosition().y - 20 > inv_y + 10 && dead_item[i] == 0)
					{
						dead_item[i] = 1; // for locating the dead invader
						shoot_the_missile = 0; // if this invader is dead , don't shoot the missile anymore
					}
				}
				else
				{
					if (missile.getPosition().x<((i - 5) * 50) + 70 + inv_x && missile.getPosition().x>((i - 5) * 50) + inv_x + 30 && missile.getPosition().y - 20 < 100 + inv_y && missile.getPosition().y - 20 > inv_y + 60 && dead_item[i] == 0)
					{
						dead_item[i] = 1; // for locating the dead invader
						shoot_the_missile = 0; // if this invader is dead , don't shoot the missile anymore
					}
				}
			}

			random = rand() % 10; // making random shoots from invaders
			if (!dead_item[random] && !shoot_now)
			{
				e_missile.setPosition(invader[random].getPosition().x, invader[random].getPosition().y); // for positioning the enemies missle before shooting
				e_visible = 1; // visibling the missle and now it is ready to shoot
				shoot_now = 1; // activating the shoot command for shoot thread !
			}

			window.clear(); // clears all shapes
			if (e_missile.getPosition().y < 300 && e_missile.getPosition().y > 280 && e_missile.getPosition().x < mouse_Xpos + 20 && e_missile.getPosition().x > mouse_Xpos - 20)
			{
				window.clear(); // clears all shapes
				window.draw(LOST); // drawing the "YOU LOST" text
				window.display(); // displaying the "YOU LOST" text
				Sleep(3000); // a 3 second delay
				window.clear(); // clears "YOU LOST" text
				window.display(); // displays a cleared screen
				Game_playing = nope; // stops the game and the main menu shows up !
			}
			else // displaying the main game shapes
			{
				if(e_visible) // if the invaders missle was shooting , then displays it !
					window.draw(e_missile);
				window.draw(main_shape); // drawing your spaceship
				window.draw(missile); // drawing your missle
				for (int i = 0; i < 10; i++)
					if (!dead_item[i]) // stops drawing the dead invaders
						window.draw(invader[i]);
				window.display(); // displaying all above shapes
			}

		}
		else
		{
			int mouse_Xpos;
			int mouse_Ypos;
			
			if (event.type == sf::Event::MouseMoved)
			{
				mouse_Ypos = event.mouseMove.y;
				mouse_Xpos = event.mouseMove.x;
			}
			if (event.type == sf::Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					if (mouse_Ypos > 100 && mouse_Ypos < 172)
					{
						reset_game();
						Game_playing = yeah;
						dont_shoot_for_a_moment = 1;
					}
					if (mouse_Ypos > 200 && mouse_Ypos < 272)
						std::exit(0);
				}
				Sleep(250); // mouse button bounce removing         // this delay is nessesary for playing the game
			}
			while (window.pollEvent(event)) // close event configuration
			{
				if (event.type == sf::Event::Closed)
				{
					window.close();
					std::exit(0); // closes terminal and other threads
				}
			}
			/////////  menu items  ////////
			window.clear();
			window.draw(Start_txt); // start button
			window.draw(Exit_txt); // exit button
			window.draw(dvlpr_name_txt); // i wrote this text for fun purpose
			window.display(); // displaying 3 above texts and buttons
		}
		

		
	}
}

void invaders_pos()
{
	bool dir = 0; // the direction of invaders motion
	while (1)
	{
		if (inv_x > 100)
		{
			dir = 1; // changes the direction
			inv_y += 10; // moves the invaders Y down
		}
		if(dir)
		{
			inv_x -= 10;  // moves the invaders Y to left
		}
		if (inv_x < 10)
		{
			dir = 0; // changes the direction
			inv_y += 10; // moves the invaders Y down
		}
		if(!dir)
		{
			inv_x += 10; // moves the invaders Y to right
		}
		Sleep(700); // a delay for slowing down the invaders motion
	}
}

void enemy_shoot()
{
	while (1)
	{
		if (dont_shoot_for_a_moment)
		{
			Sleep(1000);
			dont_shoot_for_a_moment = 0;
		}
		if (shoot_now && e_visible)
		{

			int y = e_missile.getPosition().y;
			for (int i = e_missile.getPosition().y; i < 300; i++)
			{
				e_missile.setPosition(e_missile.getPosition().x, ++(++y)); // ++(++y) is for twisting the time for better shooting motion !
				Sleep(10 - e_speed);
				if (!shoot_now)
					break;
			}
			shoot_now = 0;
			cout << shoot_now << endl;
		}
		if (e_missile.getPosition().y)
			e_missile.setPosition(e_missile.getPosition().x, invader[0].getPosition().y);
	}
}

bool is_win()
{
	for (int i = 0; i < 10; i++)
		if (!dead_item[i])
			return 0; // if there is an alive invader , return zero 
	return 1; // if there is no alive invader , return one
}

void reset_game()
{
	for (int i = 0; i < 10; i++)
	{
		dead_item[i] = 0; // respawning the invaders
	}
	inv_x = 0; // repositioning the X axix starting point of invaders
	inv_y = 0; // repositioning the y axix starting point of invaders
}