#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;

class Zombie {
private:
	const float BLOATER_SPEED = 40.f;
	const float CHASER_SPEED = 80.f;
	const float CRAWLER_SPEED = 20.f;
	const float BLOATER_HEALTH = 5.f;
	const float CHASER_HEALTH = 1.f;
	const float CRAWLER_HEALTH = 3.f;
	const int MAX_VARRIANCE = 100;
	const int  OFFSET = 101 - MAX_VARRIANCE;
	Vector2f m_Position;
	Sprite m_Sprite;
	float m_Speed;
	float m_Health;
	bool m_Alive;
	Vector2f hitBoxOffset = { 25.f, 25.f };

public:
	enum ZombieType {
		BLOATER = 1,
		CHASER = 2,
		CRAWLER = 3
	};
	ZombieType m_Type;
	Zombie();
	bool hit();
	bool isAlive();
	void spawn(float startX, float startY, ZombieType type);
	FloatRect getPosition();
	FloatRect getHitBox(ZombieType type);
	Sprite getSprite();
	void update(float elapsedTime, Vector2f playerPosition);
};
