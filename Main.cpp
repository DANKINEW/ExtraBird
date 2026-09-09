#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <SFML/Graphics.hpp>
#include <string>
#include <sstream>
#include <algorithm>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <Windows.h>

enum gamestate {
	Playing, Dead, Menu
};

sf::Color HSVToRGB(float h, float s, float v) {
	float hPrime = h / 60.0f;
	unsigned int hIndex = unsigned int(hPrime) % 6;
	float chroma = s * v;
	float min = (v - chroma);
	float x = chroma * (1.0f - abs(fmod(hPrime, 2.0f) - 1.0f));
	float outRGB[6][3] = {

		{chroma, x, 0.0f},
		{x, chroma, 0.0f},
		{0.0f, chroma, x},
		{0.0f, x, chroma},
		{x, 0.0f, chroma},
		{chroma, 0.0f, x}

	};
	float rF = (outRGB[hIndex][0] + min);
	float gF = (outRGB[hIndex][1] + min);
	float bF = (outRGB[hIndex][2] + min);
	rF *= 255;
	gF *= 255;
	bF *= 255;
	std::uint8_t rI = std::uint8_t(rF);
	std::uint8_t gI = std::uint8_t(gF);
	std::uint8_t bI = std::uint8_t(bF);
	return sf::Color(rI, gI, bI);
}
void PollEvents(sf::RenderWindow& window, float& velocityY, const float& jumpforce, gamestate &gm, float &restartTimer) {
	while (const std::optional event = window.pollEvent()) {
		ImGui::SFML::ProcessEvent(window, *event);

		if (event->is<sf::Event::Closed>()) {
			window.close();
		}
		else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
			if (keyPressed->scancode == sf::Keyboard::Scancode::Escape) {
				window.close();
			}
			if (keyPressed->scancode == sf::Keyboard::Scancode::Space) {
				velocityY = jumpforce;
			}
		}
		else if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
			if (mousePressed->button == sf::Mouse::Button::Left) {
				if (gm == Menu || (gm == Dead && restartTimer <= 0.0f)) {
					gm = Playing;
				}
			}
		}
	}
}


int main() {
	unsigned int width = 1920;
	unsigned int height = 1080;
	const float tilesize = 20.0f;
	int gridCols = 10;
	int gridRows = 10;
	static int mines = 15;
	sf::Vector2u windowSize = { width, height };
	sf::VideoMode videomode = sf::VideoMode(windowSize);
	std::string title = "SFMLL";
	sf::RenderWindow window = sf::RenderWindow(sf::VideoMode({ width, height }), title);
	window.setFramerateLimit(60);
	window.setKeyRepeatEnabled(false);

	std::default_random_engine rand;
	rand.seed(std::chrono::steady_clock::now().time_since_epoch().count());
	std::uniform_real_distribution<float> randCoo;
	if (!ImGui::SFML::Init(window)) {
		return -1;
	}

	sf::Texture background;
	if (!background.loadFromFile("Texture/background.png")) std::cerr << "No texture\n";
	sf::Sprite backgroundSpr(background);

	sf::Texture BirdTex;
	if (!BirdTex.loadFromFile("Texture/Bird.png")) std::cerr << "No texture\n";
	sf::Sprite bird(BirdTex);
	bird.setScale({ 0.2f, 0.2f });
	bird.setPosition({ width / 2.0f, height / 2.0f });
	sf::Vector2u texSize = BirdTex.getSize();
	bird.setOrigin({ texSize.x / 2.0f, texSize.y / 2.0f});

	sf::Font mine;
	if (!mine.openFromFile("Font/Minecraft.ttf")) std::cerr << "Failed to load font Minecraft.ttf";

	sf::Font loseMine;
	if(!loseMine.openFromFile("Font/Minecraft.ttf")) std::cerr << "Failed to load font Minecraft.ttf";

	sf::RectangleShape blackscreen({ 1920.0f, 1080.0f });
	blackscreen.setOrigin({ 1920.0f / 2.0f, 1080.0f / 2.0f });
	blackscreen.setPosition({ width / 2.0f, height / 2.0f });
	

	gamestate gm = Menu;


	bool resetALP = false;
	float velocityY = 0.0f;
	float rotation = 0.0f;
	const float jumpforce = -700.0f;
	const float maxFallspeed = 600.0f;
	const float gravity = 3000.0f;
	float fadeAlp = 0.0f;
	float fadeAlpT = 0.0f;
	const float fadespd = 300.0f;
	const float fadespdT = 100.0f;
	float restartTimer = 0.0f;
	const float restartDelay = 3.0f;

	sf::Clock clock;

	while (window.isOpen()) {


		//Updating
		PollEvents(window, velocityY, jumpforce, gm, restartTimer);
		sf::Text minetext(mine);
		sf::Text loseText(loseMine);

		float dt = clock.restart().asSeconds();

		
		if (gm == Menu) {
			bird.setPosition({ width / 2.0f, height / 2.0f });
			
			minetext.setString("Press LMB to start the game\n Then, space to jump");
			minetext.setFillColor(sf::Color::Black);
			minetext.setOutlineThickness(2.0f);
			minetext.setOutlineColor(sf::Color::White);
			minetext.setPosition({ width / 2.0f - 160.0f, height / 2.0f - 160.0f });
		}
		if (gm == Dead) {
			if (restartTimer > 0.0f) {
				restartTimer -= dt;
				if (restartTimer < 0.0f) restartTimer = 0.0f;
			}
			if (resetALP) {
				fadeAlp = 0.0f;
				fadeAlpT = 0.0f;
				resetALP = false;
			}
			bird.setPosition({ width / 2.0f, height / 2.0f });
			bird.setRotation(sf::degrees(0));
			fadeAlp += fadespd * dt;
			if (fadeAlp > 180.0f) {
				fadeAlp = 180.0f;
			}
			blackscreen.setFillColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(fadeAlp)));
			
			if (fadeAlp == 180.0f) {
				
				loseText.setString("You lost!\n Press LMB TO RESTART \n Then, space to jump");
				fadeAlpT += fadespdT * dt;
				if (fadeAlpT > 255.0f) {
					fadeAlpT = 255.0f;

				}

				loseText.setFillColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(fadeAlpT)));
				loseText.setOutlineThickness(2.0f);
				loseText.setOutlineColor(sf::Color(255, 255, 255, static_cast<std::uint8_t>(fadeAlpT)));
				loseText.setPosition({ width / 2.0f - 160.0f, height / 2.0f - 160.0f });
			}
		}
		if (gm == Playing) {
			velocityY += gravity * dt;
			
			bird.move({ 0.0f, velocityY * dt - 4.0f });

			if (bird.getPosition().y > 1080.0f || bird.getPosition().y < 0) {
				gm = Dead;
				restartTimer = restartDelay;
				resetALP = true;
			} 


			if (velocityY < 0.0f) {
				rotation = -20.0f;
			}
			else {
				rotation = 40.0f;
			}
			if (velocityY > 1200.0f) {
				velocityY = 1200.0f;
			}

			std::cerr << bird.getPosition().y << '\n';

			bird.setRotation(sf::degrees(rotation));
		}
		
		//Render

		window.clear(sf::Color::Black);

		// Draw

		window.draw(backgroundSpr);
		window.draw(bird);
		window.draw(minetext);
		
		if (gm == Dead) {
			window.draw(blackscreen);
		}
		window.draw(loseText);

		// Display screen

		window.display();
	}
	ImGui::SFML::Shutdown();
	return 0;
}