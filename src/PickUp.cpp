#include "Headers/PickUp.h"
#include "Headers/TextureHolder.h"
#include <iostream>

PickUp::PickUp(int type) : m_Type(type), m_Spawned(false), m_SecondsSinceSpawn(0.f), m_SecondsSinceDeSpawn(0.f), m_SecondsToLive(static_cast<float>(START_SECONDS_TO_LIVE)), m_SecondsToWait(static_cast<float>(START_WAIT_TIME)), m_Sprite(TextureHolder::GetTexture("graphics/health_pickup.png")) {
	if (m_Type == 1) {
		m_Value = HEALTH_START_VALUE;
		m_Sprite = Sprite(TextureHolder::GetTexture("graphics/health_pickup.png"));
	}
	else {
		m_Value = AMMO_START_VALUE;
		m_Sprite = Sprite(TextureHolder::GetTexture("graphics/ammo_pickup.png"));
	}
	m_Sprite.setOrigin({ 25, 25 });
}
void PickUp::setArena(IntRect arena) {
	m_Arena.position.y = arena.position.y + 50;
	m_Arena.position.x = arena.position.x - 50;
	m_Arena.size.x = arena.size.x - 50;
	m_Arena.size.y = arena.size.y - 50;
}

void PickUp::spawn() {
	int x = (std::rand() % m_Arena.size.x);
	int y = (std::rand() % m_Arena.size.y);
	m_Sprite.setPosition({static_cast<float>(x), static_cast<float>(y)});
	m_Spawned = true;
}
FloatRect PickUp::getPosition() {
	return m_Sprite.getGlobalBounds();
}
Sprite &PickUp::getSprite() {
	return m_Sprite;
}

bool PickUp::isSpawned() {
	return m_Spawned;
}

int PickUp::gotIt() {
	m_Spawned = false;
	m_SecondsSinceDeSpawn = 0.f;
	return m_Value;
}

void PickUp::update(float elapsedTime, bool allowSpawn) {
	if (m_Spawned) {
		m_SecondsSinceSpawn += elapsedTime;
		if (m_SecondsSinceSpawn > m_SecondsToLive) {
			m_Spawned = false;
			m_SecondsSinceDeSpawn = 0.f;
		}
	}
	else {
		m_SecondsSinceDeSpawn += elapsedTime;
	
		if (m_SecondsSinceDeSpawn > m_SecondsToWait && allowSpawn) {
			m_SecondsSinceSpawn = 0.f;
			spawn();
		}
	}
}

void PickUp::upgrade() {
	if (m_Type == 1) {
		if (m_Value < 100) {
			m_Value += (HEALTH_START_VALUE * .5f);
		}
	}
	else {
		m_Value += (AMMO_START_VALUE * .2f);
	}
	m_SecondsToLive += (START_SECONDS_TO_LIVE * .5f);
	m_SecondsToWait -= (START_WAIT_TIME * .2f);
}
