#include <SFML/Graphics.hpp>

int main()
{
    //Set up like a unity c# file as much as possible. 
	//Avoid making header files to call things IF POSSIBLE.  
	//May need it for:  ..


	//Initialize.  Can change in loop.  
	sf::RenderWindow window(sf::VideoMode(500, 500), "Swarch");
	sf::RectangleShape myBox(sf::Vector2f(10,10));
	myBox.setFillColor(sf::Color::Cyan);

	float mySpeed = 0.1f;


	//Game loop (actual main)

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(myBox);
        window.display();


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




		//while(numPellets < 4) spawnPellet(pellet, randomLocation);













    }

    return 0;
}