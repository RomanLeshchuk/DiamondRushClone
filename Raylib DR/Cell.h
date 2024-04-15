#pragma once

#include <memory>
#include <algorithm>
#include <vector>

#include "Entity.h"

class Cell
{
public:
	using iterator = std::vector<std::unique_ptr<Entity>>::iterator;

	Cell();

	void add(std::unique_ptr<Entity> entity);
	iterator find(Entity::Type entityType);
	void erase(Entity::Type entityType);
	void erase(iterator it);

	std::vector<std::unique_ptr<Entity>>& getData();

	iterator begin();
	iterator end();

private:
	std::vector<std::unique_ptr<Entity>> m_data{};
};