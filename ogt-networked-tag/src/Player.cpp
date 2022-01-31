#include "Player.h"

Player::Player() :
	m_characterPositions{
{ CHAR_SPACING * 5, CHAR_HEIGHT * 24 },
{ CHAR_SPACING * 5, CHAR_HEIGHT * 27},
{ CHAR_SPACING * 6, CHAR_HEIGHT * 13},
{ CHAR_SPACING * 11, CHAR_HEIGHT * 13},
{ CHAR_SPACING * 11, CHAR_HEIGHT * 17},
{ CHAR_SPACING * 1, 0},
{ CHAR_SPACING * 11, 0},
{ CHAR_SPACING * 8, CHAR_HEIGHT * 9},
{ CHAR_SPACING * 1, CHAR_HEIGHT * 16},
{ CHAR_SPACING * 1, CHAR_HEIGHT * 30} }
{

	loadFiles();
	respawn();

	m_score = 0;
}

void Player::loadFiles()
{
	if (!m_spriteSheet.loadFromFile("ASSETS\\IMAGES\\characters.png"))
	{
		// Error loading image
	}

	m_body.setTexture(m_spriteSheet);
	m_body.setTextureRect(sf::IntRect{ CHAR_SPACING * 2,0,CHAR_WIDTH,CHAR_HEIGHT });
	m_body.setOrigin(0.0f, static_cast<float>(CHAR_WIDTH));

	if (!m_font.loadFromFile("ASSETS\\FONTS\\twoson.ttf"))
	{
		// Error loading font file
	}
	m_playerName.setFont(m_font);
	m_playerName.setCharacterSize(32);
}

void Player::respawn()
{
	m_score = 0;
	m_hurtTimer = 0;
	m_moveTimer = 0;
	m_characterDirection = 0;
	m_lives = 3;
	m_body.setTextureRect(sf::IntRect{ m_character.x + CHAR_SPACING,m_character.y,CHAR_WIDTH,CHAR_HEIGHT });
	m_body.setColor(sf::Color::White);
	m_body.setOrigin(0.0f, m_body.getOrigin().y);
	m_body.setScale(1.0f, 1.0f);
	setPos(sf::Vector2i{ 12,2 });

}

void Player::draw(sf::RenderTarget& t_target, sf::RenderStates t_states) const
{
	t_target.draw(m_body, t_states);
	t_target.draw(m_playerName, t_states);
}

void Player::setPos(sf::Vector2i t_pos)
{
	m_pos = t_pos;
	m_previousPos = t_pos;
	m_body.setPosition(static_cast<sf::Vector2f>(t_pos * 32));
}

void Player::setCharacter(int t_characterNum)
{
	m_characterNumber = t_characterNum % MAX_CHARACTERS;
	if (m_characterNumber < 0)
		m_characterNumber = MAX_CHARACTERS + m_characterNumber;
	m_character = m_characterPositions[m_characterNumber];
	m_body.setTextureRect(sf::IntRect{ m_character.x + CHAR_SPACING,m_character.y,CHAR_WIDTH,CHAR_HEIGHT });
}

void Player::move(Direction t_direction, Cell t_maze[][MAX_COLS])
{
	sf::Vector2i dirVector = Global::getDirectionVector(t_direction);
	sf::Vector2i desiredPosition = m_pos + dirVector;

	if (t_maze[desiredPosition.y][desiredPosition.x].getTileType() != Tile::Rock) // Move if not blocked by a rock
	{
		if (t_maze[desiredPosition.y][desiredPosition.x].getTileType() != Tile::Moveable) // Move if not blocked by a moveable block
		{
			m_previousPos = m_pos;
			m_pos = desiredPosition;
			m_moveTimer = MOVEMENT_TIME;
		}
		else if (t_maze[desiredPosition.y + dirVector.y][desiredPosition.x + dirVector.x].getTileType() != Tile::Rock
			&& t_maze[desiredPosition.y + dirVector.y][desiredPosition.x + dirVector.x].getTileType() != Tile::Moveable)
		{
			moveRock(t_maze, t_direction);
			m_previousPos = m_pos;
			m_pos = desiredPosition;
			m_moveTimer = MOVEMENT_TIME;
			
		}
	}

	setTextureDirection(t_direction);

	if (t_maze[m_pos.y][m_pos.x].getTileType() == Tile::Coin)
	{
		t_maze[m_pos.y][m_pos.x].setTileType(Tile::None);
		m_score++;
	}
}

void Player::setTextureDirection(Direction t_direction)
{
	if (t_direction == Direction::North)
		m_characterDirection = 2;
	else if (t_direction == Direction::South)
		m_characterDirection = 0;
	else if (t_direction == Direction::West)
	{
		m_body.setScale(-1.0f, 1.0f);
		m_body.setOrigin(static_cast<float>(CHAR_WIDTH), m_body.getOrigin().y);
		m_characterDirection = 1;
	}
	else if (t_direction == Direction::East)
	{
		m_body.setScale(1.0f, 1.0f);
		m_body.setOrigin(0.0f, m_body.getOrigin().y);
		m_characterDirection = 1;
	}
}

void Player::update(Cell t_maze[][MAX_COLS])
{
	if (m_moveTimer == 0)
	{
		//movementInput(t_maze);
		m_body.setTextureRect(sf::IntRect{ m_character.x + CHAR_SPACING,m_character.y + m_characterDirection * CHAR_HEIGHT,CHAR_WIDTH,CHAR_HEIGHT });
	}
	else
		animate();

	if (m_hurtTimer > 0)
	{
		m_hurtTimer--;
		if (m_hurtTimer == 0)
			m_body.setColor(sf::Color::White);
	}

	m_playerName.setPosition(sf::Vector2f(m_body.getPosition().x - 25, m_body.getPosition().y - 50));

	if (t_maze[getPos().y][getPos().x].getTileType() == Tile::Moveable)
		if (m_lives > 0)
			m_lives--;
}

void Player::movementInput(Cell t_maze[][MAX_COLS])
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
	{
		move(Direction::North, t_maze);
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
	{
		move(Direction::South, t_maze);
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		move(Direction::East, t_maze);
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		move(Direction::West, t_maze);
	}
}

void Player::animate()
{
	m_moveTimer--;

	float newX = (m_pos.x * 32) * (1.0f - (1.0f * m_moveTimer / MOVEMENT_TIME)) + (m_previousPos.x * 32) * (1.0f * m_moveTimer / MOVEMENT_TIME);
	float newY = (m_pos.y * 32) * (1.0f - (1.0f * m_moveTimer / MOVEMENT_TIME)) + (m_previousPos.y * 32) * (1.0f * m_moveTimer / MOVEMENT_TIME);

	m_body.setPosition(static_cast<float>(newX), static_cast<float>(newY));

	int frameNum = static_cast<int>((1.0 * m_moveTimer / MOVEMENT_TIME) * 3);

	m_body.setTextureRect(sf::IntRect{ m_character.x + (CHAR_SPACING * frameNum), m_character.y + (m_characterDirection * CHAR_HEIGHT),CHAR_WIDTH, CHAR_HEIGHT });
}

void Player::moveRock(Cell t_maze[][MAX_COLS], Direction t_direction)
{
	sf::Vector2i dirVector = Global::getDirectionVector(t_direction);
	sf::Vector2i desiredPosition = m_pos + dirVector;

	t_maze[desiredPosition.y][desiredPosition.x].setTileType(t_maze[desiredPosition.y + dirVector.y][desiredPosition.x + dirVector.x].getTileType());
	t_maze[desiredPosition.y + dirVector.y][desiredPosition.x + dirVector.x].setTileType(Tile::Moveable);
}
