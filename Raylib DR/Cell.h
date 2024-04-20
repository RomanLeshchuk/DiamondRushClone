#pragma once

#include <memory>
#include <algorithm>
#include <vector>

#include "Entity.h"
#include "Entities.h"

class Cell
{
public:
	using iterator = std::vector<std::unique_ptr<Entity>>::iterator;
	using const_iterator = std::vector<std::unique_ptr<Entity>>::const_iterator;

	Cell();

	void add(std::unique_ptr<Entity> entity);
	iterator find(Entity::Type entityType);
	void erase(Entity::Type entityType);
	void erase(iterator it);

	iterator begin();
	iterator end();
	const_iterator begin() const;
	const_iterator end() const;

private:
	std::vector<std::unique_ptr<Entity>> m_data{};
};