#include "Headers/Zombie.h"
#include "Headers/TextureHolder.h"
#include <cstdlib>
#include <ctime>
#include <cmath>

Zombie::Zombie() : m_Position(),
  m_Sprite(TextureHolder::GetTexture("graphics/bloater.png"))
, m_Speed(0.f)
, m_Health(0.f)
, m_Alive(false)
, m_Type(BLOATER)
{
	m_Sprite.setOrigin({ 25, 25 });
}

void Zombie::spawn(float startX, float startY, ZombieType type)
{
	m_Position.x = startX;
	m_Position.y = startY;
	switch (type)
	{
	case BLOATER:
		m_Speed = BLOATER_SPEED;
		m_Health = BLOATER_HEALTH;
		m_Sprite = Sprite(TextureHolder::GetTexture("graphics/bloater.png"));
		m_Type = BLOATER;
		break;
	case CHASER:
		m_Speed = CHASER_SPEED;
		m_Health = CHASER_HEALTH;
		m_Sprite = Sprite(TextureHolder::GetTexture("graphics/chaser.png"));
		m_Type = CHASER;
		break;
	case CRAWLER:
		m_Speed = CRAWLER_SPEED;
		m_Health = CRAWLER_HEALTH;
		m_Sprite = Sprite(TextureHolder::GetTexture("graphics/crawler.png"));
		m_Type = CRAWLER;
		break;
	default:
		throw std::invalid_argument("Invalid zombie type");
	}
	int variance = (rand() % MAX_VARRIANCE) + OFFSET;
	float modifier = static_cast<float>(variance) / 100.f;
	m_Speed *= modifier;
	m_Alive = true;
	m_Sprite.setOrigin({25, 25});
	m_Sprite.setPosition(m_Position);
}
bool Zombie::hit()
{
	m_Health--;
	if (m_Health <= 0)
	{

		m_Alive = false;
		m_Sprite = Sprite(TextureHolder::GetTexture("graphics/blood.png"));
		m_Sprite.setPosition(m_Position);
		return false;
	}
	return true;
}
bool Zombie::isAlive()
{
	return m_Alive;
}
FloatRect Zombie::getPosition()
{
	return m_Sprite.getGlobalBounds();
}

void Zombie::update(float elapsedTime, Vector2f playerPosition)
{
	float dx = playerPosition.x - m_Position.x;
	float dy = playerPosition.y - m_Position.y;
	float length = std::sqrt(dx * dx + dy * dy);
	if (length == 0)
	{
		m_Sprite.setPosition(m_Position);
		return;
	}
	float directionX = dx / length;
	float directionY = dy / length;
	float velocityX = directionX * m_Speed;
	float velocityY = directionY * m_Speed;
	m_Position.x += velocityX * elapsedTime;
	m_Position.y += velocityY * elapsedTime;
	float angle = atan2(dy, dx);

	m_Sprite.setPosition(m_Position);
	m_Sprite.setRotation(radians(angle));
}

Sprite Zombie::getSprite()
{
	return m_Sprite;
}
FloatRect Zombie::getHitBox(Zombie::ZombieType type)
{
	// Use local (untransformed) bounds so the hitbox size doesn't
	// balloon when the sprite is rotated to face the player.
	FloatRect localBounds = m_Sprite.getLocalBounds();
	if (type == Zombie::ZombieType::BLOATER) {
		hitBoxOffset = {10.f, 10.f };

	}
	else if (type == Zombie::ZombieType::CHASER) {
		hitBoxOffset = { 10.f, 10.f };
	}
	else if (type == Zombie::ZombieType::CRAWLER) {
		hitBoxOffset = { 10.f, 10.f };
	}
	Vector2f size = { localBounds.size.x - hitBoxOffset.x * 2.f, localBounds.size.y - hitBoxOffset.y * 2.f };
	Vector2f topLeft = { m_Position.x - size.x / 2.f, m_Position.y - size.y / 2.f };
	return FloatRect(topLeft, size);
};
