#ifndef PLAYER_H
#define PLAYER_H

#include <SFML\Graphics.hpp>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include "Globals.h"
#include "Cell.h"

class Player : public sf::Drawable
{
	const int MAX_HURT_TIME{ 60 };
	const int MOVEMENT_TIME{ 15 };
	static const int MAX_CHARACTERS{ 10 };

	sf::Texture m_spriteSheet;
	sf::Sprite m_body;
	sf::Vector2i m_pos;
	sf::Vector2i m_previousPos;
	int m_score;
	int m_lives;
	int m_hurtTimer;
	int m_moveTimer;

	sf::Vector2i m_characterPositions[MAX_CHARACTERS];
	int m_characterDirection;
	int m_characterNumber;
	sf::Vector2i m_character;

	sf::Font m_font;
	sf::Text m_playerName;

	bool rockMoving = false;
	Direction dir;

public:
	Player();
	void loadFiles();
	void respawn();


	virtual void draw(sf::RenderTarget& t_target, sf::RenderStates t_states) const;

	void setPos(sf::Vector2i t_pos);
	inline void setLives(int t_lives) { m_lives = t_lives; }
	inline void setPosition(sf::Vector2f t_position) { m_body.setPosition(t_position); }
	inline void setScale(sf::Vector2f t_scale) { m_body.setScale(t_scale); }
	inline void refreshPosition() { m_body.setPosition(static_cast<float>(m_pos.x * TILE_SIZE), static_cast<float>(m_pos.y * TILE_SIZE)); }
	void setCharacter(int t_characterNum);

	inline sf::Vector2i getPos() { return m_pos; } 
	inline sf::Sprite getBody() { return m_body; }
	inline int getScore() { return m_score; }
	inline int getLives() { return m_lives; }
	inline int getCharNum() { return m_characterNumber; }

	void move(Direction t_direction, Cell t_maze[][MAX_COLS]);
	void setTextureDirection(Direction t_direction);
	void update(Cell t_maze[][MAX_COLS]);
	void movementInput(Cell t_maze[][MAX_COLS]);
	void animate();

	inline void setCharName(std::string t_str) { m_playerName.setString(t_str); }

	void moveRock(Cell t_maze[][MAX_COLS], Direction t_direction);
};

#endif // !PLAYER_H
