#include "Headers/TileMap.h"
#include "Headers/TextureHolder.h"
#include <iostream>
#include <ctime>
#include <cstdlib>


bool TileMap::load(Vector2u tileSize, IntRect arena){

		m_tileset = TextureHolder::GetTexture("graphics/background_sheet.png");

	std::srand(static_cast<unsigned int>(time(nullptr)));
	const unsigned int width = arena.size.x / tileSize.x;
	const unsigned int height = arena.size.y / tileSize.y;
	m_vertices.resize(width * height * 6);

	const int WALL_TILE = 3;

	std::vector<int> randomMap(width * height);
	for (unsigned int i = 0; i < width * height; ++i) {
		randomMap[i] = std::rand() % 3;
	}

	for (unsigned int i = 0; i < width; ++i) {
		for (unsigned int j = 0; j < height; ++j) {
			bool isWall = (i == 0 || j == 0 || j == height - 1 || i == width - 1);
			int tileNumber = isWall ? WALL_TILE : randomMap[i + j * width];

			unsigned int tu = tileNumber % (m_tileset.getSize().x / tileSize.x);
			unsigned int tv = tileNumber / (m_tileset.getSize().x / tileSize.x);

			std::size_t vertexIndex = (i + j * width) * 6;
			

			Vector2f tl(static_cast<float>(i * tileSize.x), static_cast<float>(j * tileSize.y));
			Vector2f tr(static_cast<float>((i + 1) * tileSize.x), static_cast<float>(j * tileSize.y));
			Vector2f bl(static_cast<float>(i * tileSize.x), static_cast<float>((j + 1) * tileSize.y));
			Vector2f br(static_cast<float>((i + 1) * tileSize.x), static_cast<float>((j + 1) * tileSize.y));

			Vector2f texTl(static_cast<float>(tu * tileSize.x), static_cast<float>(tv * tileSize.y));
			Vector2f texTr(static_cast<float>((tu + 1) * tileSize.x), static_cast<float>(tv * tileSize.y));
			Vector2f texBl(static_cast<float>(tu * tileSize.x), static_cast<float>((tv + 1) * tileSize.y));
			Vector2f texBr(static_cast<float>((tu + 1) * tileSize.x), static_cast<float>((tv + 1) * tileSize.y));

			m_vertices[vertexIndex + 0] = Vertex(tl, Color::White, texTl);
			m_vertices[vertexIndex + 1] = Vertex(tr, Color::White, texTr);
			m_vertices[vertexIndex + 2] = Vertex(bl, Color::White, texBl);
			m_vertices[vertexIndex + 3] = Vertex(bl, Color::White, texBl);
			m_vertices[vertexIndex + 4] = Vertex(tr, Color::White, texTr);
			m_vertices[vertexIndex + 5] = Vertex(br, Color::White, texBr);

		}
	}
	return true;
}

void TileMap::draw(RenderTarget& target, RenderStates states) const {
	states.texture = &m_tileset;
	target.draw(m_vertices, states);
}