#ifndef WORLD_INTERFACE_H
#define WORLD_INTERFACE_H

class WorldInterface
{
public:

	virtual bool isTileEmpty(int t_x, int t_y) const = 0;

};

#endif // !WORLD_INTERFACE_H