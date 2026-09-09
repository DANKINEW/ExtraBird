#include "Tubes.hpp"
#include <iostream>
#include <chrono>
#include <SFML/Graphics.hpp>
PipeManager::PipeManager(float screenWidth, float screenHeight) : m_screenHeight(screenHeight), m_screenWidth(screenWidth), m_gapHeight(150.0f) {

	if (pipTex.loadFromFile("Texture/Tubes.png")) std::cerr << "failed to load tube texture\n";
	rand.seed(std::chrono::steady_clock::now().time_since_epoch().count());

}
void PipeManager::spawnPipe() {
	pipePair newPair(pipTex);

	newPair.bottomPipe.setScale({ 1.0f, -1.0f });

	float minY = 100.0f;
	float maxY = m_screenHeight - m_gapHeight - 100.0f;

	std::uniform_real_distribution<float> distortion(minY, maxY);

	float gapY = distortion(rand);

	newPair.topPipe.setPosition({ m_screenWidth, gapY });
	newPair.topPipe.setPosition({ m_screenWidth, gapY + m_gapHeight});

	pipes.push_back(newPair);
}
void PipeManager::update(float dt) {

}