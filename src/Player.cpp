#include "Headers/Player.h"
#include "Headers/TextureHolder.h"
#include "Headers/Zombie.h"
#include <iostream>

Player::Player()
: m_Texture()            // texture constructed first
, m_Sprite(m_Texture)    // sprite initialized from texture (no default ctor required)
, m_Position()
, m_Resolution()
, m_TileSize(0)
, m_UpPressed(false)
, m_DownPressed(false)
, m_LeftPressed(false)
, m_RightPressed(false)
, m_Health(static_cast<int>(START_HEALTH))
, m_MaxHealth(static_cast<int>(START_HEALTH))
, m_Speed(START_SPEED)
{
	m_Texture = TextureHolder::GetTexture("graphics/player.png");

	m_Sprite.setTextureRect(IntRect({ 0, 0 }, static_cast<Vector2i>(m_Texture.getSize())));
	m_Sprite.setOrigin({25, 25});
}
Texture Player::getTexture()
{
	return m_Texture;
}
void Player::spawn(IntRect arena, Vector2f resolution, int tileSize)
{
	m_Arena = arena;
	m_Resolution.x = resolution.x;
	m_Resolution.y = resolution.y;
	m_TileSize = tileSize;
	m_Position.x = static_cast<float>(arena.size.x / 2);
	m_Position.y = static_cast<float>(arena.size.y / 2);
}

void Player::resetPlayerStats()
{
	m_Speed = START_SPEED;
	m_Health = static_cast<int>(START_HEALTH);
	m_MaxHealth = static_cast<int>(START_HEALTH);
}

Time Player::getLastHitTime()
{
	return m_LastHit;
}

bool Player::hit(Time timeHit, Zombie::ZombieType zombieType)
{
	if (timeHit.asMilliseconds() - m_LastHit.asMilliseconds() > 200)
	{
		if (zombieType == Zombie::BLOATER) {
			m_Health -= 20;
		}
		else {
		m_Health -= 10;
		}
		m_LastHit = timeHit;
		std::cout << "Player hit! Health: " << m_Health << std::endl;
		return true;
	}
	else
	{
		return false;
	}
}

FloatRect Player::getPosition()
{
	return m_Sprite.getGlobalBounds();
}

const Vector2f& Player::getCenter()
{
	return m_Position;
}

Sprite Player::getSprite()
{
	return m_Sprite;
}
int Player::getHealth()
{
	return m_Health;
}

Angle Player::getRotation()
{
	return m_Sprite.getRotation();
}
void Player::moveLeft()
{
	m_LeftPressed = true;
}
void Player::moveRight()
{
	m_RightPressed = true;
}
void Player::moveUp()
{
	m_UpPressed = true;
}
void Player::moveDown()
{
	m_DownPressed = true;
}
void Player::stopLeft()
{
	m_LeftPressed = false;
}
void Player::stopRight()
{
	m_RightPressed = false;
}
void Player::stopUp()
{
	m_UpPressed = false;
}
void Player::stopDown()
{
	m_DownPressed = false;
}

void Player::update(float elapsedTime, Vector2i mousePosition)
{
	if (m_UpPressed)
	{
		m_Position.y -= m_Speed * elapsedTime;
	}
	if (m_DownPressed)
	{
		m_Position.y += m_Speed * elapsedTime;
	}
	if (m_LeftPressed)
	{
		m_Position.x -= m_Speed * elapsedTime;

	}
	if (m_RightPressed)
	{
		m_Position.x += m_Speed * elapsedTime;
		
	}
	m_Sprite.setPosition(m_Position);
	if (m_Position.x > m_Arena.size.x - m_TileSize)
	{
		m_Position.x = static_cast<float>(m_Arena.size.x - m_TileSize);
	}
	if (m_Position.x < m_Arena.position.x + m_TileSize)
	{
		m_Position.x = static_cast<float>(m_Arena.position.x + m_TileSize);
	}
	if (m_Position.y > m_Arena.size.y - m_TileSize)
	{
		m_Position.y = static_cast<float>(m_Arena.size.y - m_TileSize);
	}
	if (m_Position.y < m_Arena.position.y + m_TileSize)
	{
		m_Position.y = static_cast<float>(m_Arena.position.y + m_TileSize);
	}
	Angle angle = radians(std::atan2(mousePosition.y - m_Resolution.y/ 2.f, mousePosition.x - m_Resolution.x/ 2.f));
	m_Sprite.setRotation(angle);
}

void Player::upgradeSpeed()
{
	m_Speed += (START_SPEED * .2f);
}

bool Player::upgradeHealth()
{

	if (m_MaxHealthReached)
	{
		return false;
	}
	m_MaxHealth += (START_HEALTH * .2f);
	if (m_MaxHealth == 200)
	{
		m_MaxHealth = 200;
		m_MaxHealthReached = true;
		return false;
	}
	return true;
}

void Player::increaseHealthLevel(int amount)
{
	m_Health += amount;
	if (m_Health > m_MaxHealth)
	{
		m_Health = m_MaxHealth;
	}
}

FloatRect Player::getHitBox()
{
	const FloatRect hitBox = m_Sprite.getGlobalBounds();
	return FloatRect({ hitBox.position.x + hitBoxOffset, hitBox.position.y + hitBoxOffset }, { hitBox.size.x - hitBoxOffset * 2, hitBox.size.y - hitBoxOffset * 2 });
}

