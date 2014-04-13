#include <SFML/Graphics.hpp>
#include <list>
#include <array>

int main()
{
    //Set up like a unity c# file as much as possible. 
	//Avoid making header files to call things IF POSSIBLE.  
	//May need it for:  ..


	//Initialize.  Can change in loop.

	int windowSizeX = 500;
	int windowSizeY = 500;

	int myBoxSizeX = 10;
	int myBoxSizeY = 10;
	//int myBoxSize = 10;//comment out.  Just send only myBoxSizeX
	//since myBoxSizeX is == myBoxSizeY.  When receive, 
	//give myBoxSizeX and myBoxSizeY the same value packet (whatever it's called)


	sf::RenderWindow window(sf::VideoMode(windowSizeX, windowSizeY), "Swarch");
	sf::RectangleShape myBox(sf::Vector2f(myBoxSizeX,myBoxSizeY));
	myBox.setFillColor(sf::Color::Cyan);

	


	float mySpeed = 0.1f;

	int randPelletLocX;
	int randPelletLocY;

	int numOfPellets = 4;

	//All these types of ways to make arrays work:
	//sf::RectangleShape* pelletList;
	//sf::RectangleShape pelletList[4];
	sf::RectangleShape* pelletList = new sf::RectangleShape[4];

	
	for(int i = 0; i < 4; i++)
	{
		pelletList[i] =  sf::RectangleShape(sf::Vector2f(5,5));
		pelletList[i].setFillColor(sf::Color::White);

		
		//window.draw(pelletList[i]);


        randPelletLocX = 1+rand()%(windowSizeX);
	    randPelletLocY = 1+rand()%(windowSizeY);

		pelletList[i].setPosition(randPelletLocX, randPelletLocY);
	}




	//Game loop (actual main)

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }


		//ALL OBJECT DRAWING GOES BETWEEN 
		//window.clear() and window.display().  All the logic 
		//comes after.  just make sure that window.clear()
		//and window.display() is called before the logic part
		//of this while(window.isOpen() loop.  
        window.clear();//called each frame

		    window.draw(myBox);
			for(int i = 0; i < numOfPellets; i++)
			{
				window.draw(pelletList[i]);
			}

        window.display();//called each frame


		//Game Logic start here:  

		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
			myBox.move(0, -mySpeed);
		}
		else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		{
			myBox.move(0, mySpeed);
		}
		else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			myBox.move(mySpeed, 0);
		}
		else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
			myBox.move(-mySpeed, 0);
		}




		













    }

    return 0;
}

//Go here only if appropriate.  
//Type ideas at place to put code in mind instead


//Ideas:




//Ideas to put inside initializer:  







//Ideas to put in While Loop:  

//while(numPellets < 4) 
		//spawnPellet(pellet, randomLocation);
		//numPellets++;

		//if(playerCollidesWithPellet)
		//player.size.width and length += 2
		//player.Speed -= ...
		//numPellets--
