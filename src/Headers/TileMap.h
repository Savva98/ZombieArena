#pragma once
#include "Zombie.h"
#include <SFML/Graphics.hpp>
#include <vector>


using namespace sf;

class TileMap : public Drawable 
{
private:
	VertexArray m_vertices{PrimitiveType::Triangles};
	Texture m_tileset;
	void draw(RenderTarget& target, RenderStates states) const override;

public: bool load(Vector2u tileSize, IntRect arena);
};

Zombie* createHorde(int numZombies, IntRect arena);