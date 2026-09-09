#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <chrono>

struct pipePair {
	sf::Sprite topPipe;
	sf::Sprite bottomPipe;
	bool passed = false;
	pipePair(const sf::Texture& texture) : topPipe(texture), bottomPipe(texture) {
	}
};
class PipeManager {
private:
	void spawnPipe();
	sf::Texture pipTex;
	std::vector<pipePair> pipes;
	float m_spawnTimer = 0.0f;
	float m_spawnInterval = 1.5f;
	float m_pipeSpeed = 200.0f;
	float m_gapHeight;
	float m_screenWidth;
	float m_screenHeight;
	std::default_random_engine rand;
public:
	PipeManager(float screenWidth, float screenHeight);
	void update(float dt);
	void draw(sf::RenderWindow& window);
};