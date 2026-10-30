#pragma once
#include <SFML/Graphics.hpp>
#include "Zombie.h"

using namespace sf;

class Player
{
private:
	const float START_SPEED = 200.f;
	const float START_HEALTH = 100.f;
	bool m_MaxHealthReached = false;
	Vector2f m_Position;
	Sprite m_Sprite;
	Texture m_Texture;
	Vector2f m_Resolution;
	IntRect m_Arena;
	int m_TileSize;
	bool m_UpPressed;
	bool m_DownPressed;
	bool m_LeftPressed;
	bool m_RightPressed;
	int m_Health;
	int m_MaxHealth;
	Time m_LastHit;
	float m_Speed;
	int hitBoxOffset = 20;
public:
	Player();
	void spawn(IntRect arena, Vector2f resolution, int tileSize);
	void resetPlayerStats();
	bool hit(Time timeHit, Zombie::ZombieType zombieType);
	Time getLastHitTime();
	FloatRect getPosition();
	const Vector2f& getCenter();
	Angle getRotation();
	Texture getTexture();
	Sprite getSprite();
	void moveLeft();
	void moveRight();
	void moveUp();
	void moveDown();
	void stopLeft();
	void stopRight();
	void stopUp();
	void stopDown();
	void update(float elapsedTime, Vector2i mousePosition);
	void upgradeSpeed();
	bool upgradeHealth();
	void increaseHealthLevel(int amount);
	int getHealth();
	FloatRect getHitBox();
};