#include "Headers/TileMap.h"
#include "Headers/Zombie.h"

Zombie* createHorde(int numZombies, IntRect arena) {
	Zombie* zombies = new Zombie[numZombies];
	int maxY = arena.size.y - 20;
	int minY = arena.position.y + 20;
	int maxX = arena.size.x -20;
	int minX = arena.position.x + 20;
	srand(static_cast<unsigned int>(time(nullptr)));
	for (int i = 0; i < numZombies; i++) {
		int type = (rand() % 3) + 1;
		int x = (rand() % (maxX - minX + 1)) + minX;
		int y = (rand() % (maxY - minY + 1)) + minY;
		zombies[i].spawn(static_cast<float>(x), static_cast<float>(y), static_cast<Zombie::ZombieType>(type));
	}
	return zombies;
}
