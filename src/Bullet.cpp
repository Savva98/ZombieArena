#include "Headers/Bullet.h"
#include <cmath>

Bullet::Bullet():m_BulletDistanceX(0), m_BulletDistanceY(0), m_inFlight(false), m_MinX(0), m_MaxX(0), m_MinY(0), m_MaxY(0) {
	m_BulletShape.setSize(Vector2f(2, 2));
	//m_BulletShape.setFillColor(Color::Red);
	//m_BulletShape.setOrigin({1, 1});
}

void Bullet::stop() {
	m_inFlight = false;
}
void Bullet::shoot(float startX, float startY, float targetX, float targetY) {
	m_inFlight = true;
	m_position.x = startX;
	m_position.y = startY;
	float dx = targetX - startX;
	float dy = targetY - startY;
	float length = std::sqrt(dx * dx + dy * dy);
	if (length == 0) {
		// Target is the same as start position, pick an arbitrary direction.
		dx = 0;
		dy = 1;
		length = 1;
	}
	m_BulletDistanceX = (dx / length) * m_BulletSpeed;
	m_BulletDistanceY = (dy / length) * m_BulletSpeed;
	float range = 1000;
	m_MinX = startX - range;
	m_MaxX = startX + range;
	m_MinY = startY - range;
	m_MaxY = startY + range;

	m_BulletShape.setPosition(m_position);
}

bool Bullet::isInFlight() {
	return m_inFlight;
}
FloatRect Bullet::getPosition() {
	return m_BulletShape.getGlobalBounds();
}
RectangleShape Bullet::getShape() {
	return m_BulletShape;
}

void Bullet::update(float elapsedTime) {
	m_position.x += m_BulletDistanceX * elapsedTime;
	m_position.y += m_BulletDistanceY * elapsedTime;
	m_BulletShape.setPosition(m_position);
	if (m_position.x < m_MinX || m_position.x > m_MaxX || m_position.y < m_MinY || m_position.y > m_MaxY) {
		m_inFlight = false;
	}
}