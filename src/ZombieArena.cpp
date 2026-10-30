#include <iostream>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Headers/Player.h"
#include "Headers/TileMap.h"
#include "Headers/TextureHolder.h"
#include "Headers/Bullet.h"
#include "Headers/PickUp.h"
#include "Headers/Text.h"
#include "Headers/SoundManage.h"

int main() {
	std::srand(static_cast<unsigned int>(std::time(nullptr)));
	Vector2u tileSize(50, 50);
	enum class gameState {
		PLAYING,
		PAUSED,
		LEVELING_UP,
		GAME_OVER
	};
	gameState state = gameState::LEVELING_UP;
	Vector2u resolution = VideoMode::getDesktopMode().size;
	TileMap map;
	RenderWindow window(VideoMode(resolution), "Zombie Arena", State::Fullscreen);
	View mainView(sf::FloatRect({ .0f, .0f }, static_cast<Vector2f>(resolution)));
	Clock clock;
	Time gameTimeTotal;
	Vector2f mouseWorldPosition;
	Vector2i mouseScreenPosition;
	TextureHolder holder;
	Player player;
	IntRect arena;
	Bullet bullets[100];
	int currentBullet = 0;
	int bulletsSpare = 24;
	int bulletsInClip = 6;
	int clipSize = 6;
	float fireRate = 1;
	window.setMouseCursorVisible(false);
	Texture crosshairTexture = TextureHolder::GetTexture("graphics/crosshair.png");
	Sprite crosshairSprite{crosshairTexture};
	crosshairSprite.setOrigin({25, 25});
	PickUp healthPickup(1);
	PickUp ammoPickup(2);
	Time lastFireTime;
	// Controls how often a health pickup wins over an ammo pickup when a new one becomes eligible to spawn.
	const int HEALTH_TO_AMMO_RATIO = 6; // 1 chance in 6 (i.e. roughly 1:5 health:ammo)
	bool pickupsBothDespawnedPrev = true;
	bool healthPickupTurn = false;

	int numZombies;
	int numZombiesAlive;
	Zombie* zombies = nullptr;
	int score = 0;
	int hiScore = 0;

	Texture textureGameOver = TextureHolder::GetTexture("graphics/background.png");
	Sprite spriteGameOver(textureGameOver);
	spriteGameOver.setPosition({ 0, 0 });
	View gameOverView(sf::FloatRect({ .0f, .0f }, static_cast<Vector2f>(resolution)));
	View hubView(sf::FloatRect({ .0f, .0f }, static_cast<Vector2f>(resolution)));
	Texture textureAmmo = TextureHolder::GetTexture("graphics/ammo_icon.png");
	Sprite spriteAmmoIcon(textureAmmo);
	spriteAmmoIcon.setPosition({ 20, 980 });
	Font font;
	if (!font.openFromFile("fonts/zombiecontrol.ttf")) {
		std::cerr << "Failed to load font" << std::endl;
		return -1;
	}
	
	TextWrapper textPaused(font, 155, Color::White, "Press Enter \nto continue", { 400, 400 });
	TextWrapper textGameOver(font, 125, Color::White, "Game Over!\nPress Enter to play again\nPress Backspace to quit", { 250, 650 });

	std::stringstream levelUpStream;
	levelUpStream <<
		"1- Increased rate of fire" <<
		"\n2- Increased clip size(next reload)" <<
		"\n3- Increased max health" <<
		"\n4- Increased run speed" <<
		"\n5- More and better health pickups" <<
		"\n6- More and better ammo pickups";
		
	
	TextWrapper textLevelingUp(font, 50, Color::White, levelUpStream.str(), {150, 250});
	TextWrapper textAmmo(font, 55, Color::White, "Ammo: 6/24", { 200, 980 });
	TextWrapper textScore(font, 55, Color::White, "Score:", { 20, 0 });
	TextWrapper textHiScore(font, 55, Color::White, "Hi Score:", { 1400, 0 });
	std::stringstream ss;

	TextWrapper textZombiesAlive(font, 55, Color::White, "Zombies: ", { 1500, 980 });
	int wave = 0;
	TextWrapper textWave(font, 55, Color::White, "Wave: " + std::to_string(wave), { 1250, 980 });
	RectangleShape healthBar;
	healthBar.setFillColor(Color::Red);
	healthBar.setPosition({ 650, 980 });
	int frameSinceLastUpdate = 0;
	int framesPerUpdate = 200;
	std::ifstream hiScoreFile("gamedata/scores.txt");
	if (hiScoreFile.is_open()) {
		hiScoreFile >> hiScore;
		hiScoreFile.close();
	}
	else {
		std::cerr << "Failed to open scores.txt" << std::endl;
	}
	SoundManage hitSound("sound/hit.wav");
	SoundManage splatSound("sound/splat.wav");
	SoundManage reloadSound("sound/reload.wav");
	SoundManage shootSound("sound/shoot.wav");
	SoundManage powerupSound("sound/powerup.wav");
	SoundManage reloadFailSound("sound/reload_failed.wav");
	SoundManage pickupSound("sound/pickup.wav");
	
	while (window.isOpen()) {

		while (const std::optional<Event> event = window.pollEvent()) {
			if (!event) break;
			if (event->is<Event::Closed>()) {
				window.close();
			}
			else if (const auto* keyPressed = event->getIf<Event::KeyPressed>()) {

				if (keyPressed->scancode == Keyboard::Scancode::Enter && state == gameState::PLAYING) {
					state = gameState::PAUSED;
				}
				else if (keyPressed->scancode == Keyboard::Scancode::Enter && state == gameState::PAUSED) {
					state = gameState::PLAYING;
					std::cout << "Resuming game" << std::endl;
					clock.restart();
				}
				else if (keyPressed->scancode == Keyboard::Scancode::Enter && state == gameState::GAME_OVER) {
					std::cout << "Starting game" << std::endl;
					std::ofstream outputFile("gamedata/scores.txt");
					outputFile << hiScore;
					outputFile.close();
					wave = 0;
					score = 0;
					currentBullet = 0;
					bulletsSpare = 24;
					bulletsInClip = 6;
					player.resetPlayerStats();
					state = gameState::LEVELING_UP;
				}
				if (keyPressed->scancode == Keyboard::Scancode::Backspace) {
					if (score != 0 && score > hiScore) {
						std::ofstream hiScoreFile("gamedata/scores.txt");
						hiScoreFile << score;
						hiScoreFile.close();
					}
				
					window.close();
				}

				if (state == gameState::PLAYING) {
					if (keyPressed->scancode == Keyboard::Scancode::R) {
						if (bulletsInClip == clipSize) {
							// Do nothing, clip is full
							std::cout << "Clip is full, bullets in clip: " << bulletsInClip << ", bullets spare: " << bulletsSpare << std::endl;
						}
						else if (bulletsSpare >= clipSize) {
							
							if(bulletsInClip == 0) {
								bulletsSpare -= clipSize;
							}else if (bulletsInClip >= clipSize/2) {
								bulletsSpare -= (clipSize - bulletsInClip);
							}else {
								bulletsSpare += (bulletsInClip - clipSize);
							}
						
							bulletsInClip = clipSize;
							reloadFailSound.playSound();
							std::cout << "Reloading, bullets in clip: " << bulletsInClip << ", bullets spare: " << bulletsSpare << std::endl;
						}
						else if (bulletsSpare > 0) {
							if (bulletsSpare < clipSize) {
								int temp = bulletsSpare;
								bulletsSpare -= clipSize - bulletsInClip;
								if (bulletsSpare < 0) {
									bulletsSpare = 0;
									bulletsInClip += temp;
								}else{
								bulletsInClip = clipSize;
								}
							}
							else if(bulletsSpare == clipSize) {
							bulletsInClip = bulletsSpare;
							bulletsSpare = 0;
							std::cout << "Reloading, bullets in clip: " << bulletsInClip << ", bullets spare: " << bulletsSpare << std::endl;
							}
						}else{
							reloadFailSound.playSound();
						}
					}
					if (keyPressed->scancode == Keyboard::Scancode::W) {
						player.moveUp();
					}
					if (keyPressed->scancode == Keyboard::Scancode::S) {
						player.moveDown();
					}
					if (keyPressed->scancode == Keyboard::Scancode::A) {
						player.moveLeft();
					}
					if (keyPressed->scancode == Keyboard::Scancode::D) {
						player.moveRight();
					}
				}
				if (state == gameState::LEVELING_UP) {
					std::cout << "Upgrading" << std::endl;
					
					if (keyPressed->scancode == Keyboard::Scancode::Num1) {
						std::cout << "Upgrading speed" << std::endl;
						fireRate++;
						state = gameState::PLAYING;
					}
					if (keyPressed->scancode == Keyboard::Scancode::Num2) {
						clipSize = clipSize + 6;
						state = gameState::PLAYING;
					}
					if (keyPressed->scancode == Keyboard::Scancode::Num3) {
						// Upgrade the player's speed
						//player.upgradeSpeed();
						if (!player.upgradeHealth()) {
							std::cout << "Health is already at maximum" << std::endl;
							levelUpStream <<
								"1- Increased rate of fire" <<
								"\n2- Increased clip size(next reload)" <<
								"\n Maximum health reached" <<
								"\n4- Increased run speed" <<
								"\n5- More and better health pickups" <<
								"\n6- More and better ammo pickups";
							textLevelingUp.setText(levelUpStream.str());
						};
						state = gameState::PLAYING;
					}
					if (keyPressed->scancode == Keyboard::Scancode::Num4) {
						player.upgradeSpeed();
						state = gameState::PLAYING;
					}
					if (keyPressed->scancode == Keyboard::Scancode::Num5) {
						healthPickup.upgrade();
						state = gameState::PLAYING;
					}
					if (keyPressed->scancode == Keyboard::Scancode::Num6) {
						ammoPickup.upgrade();
						state = gameState::PLAYING;
					}

					if (state == gameState::PLAYING) {
						wave++;
						arena.size.x = 500 * wave;
						arena.size.y = 500 * wave;
						arena.position.x = 0;
						arena.position.y = 0;
						if (!map.load(tileSize, arena)) {
							std::cerr << "Failed to load tile map" << std::endl;
							return -1;
						}
						player.spawn(arena, static_cast<Vector2f>(resolution), tileSize.x);
						healthPickup.setArena(arena);
						ammoPickup.setArena(arena);
						numZombies = 5 * wave;
						numZombiesAlive = numZombies;
						delete[] zombies;
						zombies = createHorde(numZombies, arena);
						powerupSound.playSound();
						clock.restart();
					}
				}
			}else if(const auto* keyReleased = event->getIf<Event::KeyReleased>()){
				if (state == gameState::PLAYING) {
					if (keyReleased->scancode == Keyboard::Scancode::W) {
						player.stopUp();
					}
					if (keyReleased->scancode == Keyboard::Scancode::S) {
						player.stopDown();
					}
					if (keyReleased->scancode == Keyboard::Scancode::A) {
						player.stopLeft();
					}
					if (keyReleased->scancode == Keyboard::Scancode::D) {
						player.stopRight();
					}
				}
			}else if (const auto* mouseButtonPressed = event->getIf<Event::MouseButtonPressed>()) {
				if (state == gameState::PLAYING) {
					if (mouseButtonPressed->button == Mouse::Button::Left) {
						if (gameTimeTotal.asMicroseconds() - lastFireTime.asMicroseconds() > 1000 / fireRate && bulletsInClip > 0) {
							const Vector2f& playerCenter = player.getCenter();
							bullets[currentBullet].shoot(playerCenter.x, playerCenter.y, mouseWorldPosition.x, mouseWorldPosition.y);
							currentBullet++;
							if (currentBullet > 99) {
								currentBullet = 0;
							}
							lastFireTime = gameTimeTotal;
							bulletsInClip--;
							shootSound.playSound();
						}
					}
				}
			}
		}
		if (state == gameState::PLAYING) {
			Time dt = clock.restart();
			gameTimeTotal += dt;
			float dtAsSeconds = dt.asSeconds();
			mouseScreenPosition = Mouse::getPosition();
			mouseWorldPosition = window.mapPixelToCoords(mouseScreenPosition, mainView);
			crosshairSprite.setPosition(mouseWorldPosition);
			player.update(dtAsSeconds, mouseScreenPosition);
			Vector2f playerPosition(player.getCenter());
			mainView.setCenter(playerPosition);
			for (int i = 0; i < numZombies; i++) {
				if (zombies[i].isAlive()) {
					zombies[i].update(dtAsSeconds, playerPosition);
				}
			}
			for (int i = 0; i < 100; i++) {
				if (bullets[i].isInFlight()) {
					bullets[i].update(dtAsSeconds);
				}
			}
			bool pickupsBothDespawned = !healthPickup.isSpawned() && !ammoPickup.isSpawned();
			if (pickupsBothDespawned && !pickupsBothDespawnedPrev) {
				// Decide, once per spawn cycle, which pickup type gets the next chance to spawn.
				healthPickupTurn = (std::rand() % HEALTH_TO_AMMO_RATIO == 0);
			}
			pickupsBothDespawnedPrev = pickupsBothDespawned;

			healthPickup.update(dtAsSeconds, healthPickupTurn && !ammoPickup.isSpawned());
			ammoPickup.update(dtAsSeconds, !healthPickupTurn && !healthPickup.isSpawned());
			for (int i = 0; i < 100; i++) {
				if (bullets[i].isInFlight()) {
					for (int j = 0; j < numZombies; j++) {
						if (zombies[j].isAlive() && bullets[i].getPosition().findIntersection(zombies[j].getHitBox(zombies[j].m_Type))) {
							bullets[i].stop();
							if (zombies[j].hit()) {
								score += 1;
							}
							else {
								score += 10;
								numZombiesAlive--;
								hiScore = std::max(score, hiScore); 
								if (numZombiesAlive == 0) {
									state = gameState::LEVELING_UP;
								}
							}
							splatSound.playSound();
						}
					}
				}
			}

			for (int i = 0; i < numZombies; i++) {
			if (zombies[i].isAlive() && zombies[i].getHitBox(zombies[i].m_Type).findIntersection(player.getHitBox())) {
					//zombies[i].hit();
					
					if (player.hit(gameTimeTotal, zombies[i].m_Type)) {
						std::cout << "Player hit by zombie! Health: " << player.getHealth() << std::endl;
						hitSound.playSound();
					}
					if (player.getHealth() <= 0) {
						state = gameState::GAME_OVER;
					}
				}
			}
			if (player.getPosition().findIntersection(healthPickup.getPosition()) && healthPickup.isSpawned()) {
				player.increaseHealthLevel(healthPickup.gotIt());
				pickupSound.playSound();
			}
			if (player.getPosition().findIntersection(ammoPickup.getPosition()) && ammoPickup.isSpawned()) {
				bulletsSpare += ammoPickup.gotIt();
				pickupSound.playSound();
			}
			healthBar.setSize({ static_cast<float>(player.getHealth() * 3), 50 });
			frameSinceLastUpdate++;
			if (frameSinceLastUpdate >= framesPerUpdate) {
				ss.str("");
				ss << "Ammo: " << bulletsInClip << "/" << bulletsSpare;
				textAmmo.setText(ss.str());
				ss.str("");
				ss << "Score: " << score;
				textScore.setText(ss.str());
				ss.str("");
				ss << "Hi Score: " << hiScore;
				textHiScore.setText(ss.str());
				ss.str("");
				ss << "Zombies: " << numZombiesAlive;
				textZombiesAlive.setText(ss.str());
				ss.str("");
				ss << "Wave: " << wave;
				textWave.setText(ss.str());
				frameSinceLastUpdate = 0;
			}
		}
		if (state == gameState::PLAYING) {
			window.clear();
			window.setView(mainView);
			window.draw(map);
			for (int i = 0; i < numZombies; i++) {
				if (zombies[i].isAlive()) {
					window.draw(zombies[i].getSprite());
				}
				else {
					window.draw(zombies[i].getSprite());
				}
			}
			for (int i = 0; i < 100; i++) {
				if (bullets[i].isInFlight()) {
					window.draw(bullets[i].getShape());
				}
			}
			window.draw(player.getSprite());
			if (healthPickup.isSpawned()) {
				window.draw(healthPickup.getSprite());
			}
			if (ammoPickup.isSpawned()) {
				window.draw(ammoPickup.getSprite());
			}
			window.draw(crosshairSprite);
			window.setView(hubView);
			window.draw(healthBar);
			window.draw(spriteAmmoIcon);
			window.draw(textAmmo.getText());
			window.draw(textScore.getText());
			window.draw(textHiScore.getText());
			window.draw(textZombiesAlive.getText());
			window.draw(textWave.getText());
		}
		else if (state == gameState::PAUSED) {
			window.draw(textPaused.getText());
		}
		else if (state == gameState::LEVELING_UP) {
			window.draw(spriteGameOver);
			window.draw(textLevelingUp.getText());
		}
		else if (state == gameState::GAME_OVER) {
			window.draw(spriteGameOver);
			window.draw(textGameOver.getText());
			window.draw(textScore.getText());
			window.draw(textHiScore.getText());
		}
		window.display();
	};
	delete[] zombies;
	return 0;
};