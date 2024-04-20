#include "Cell.h"

Cell::Cell() = default;

void Cell::add(std::unique_ptr<Entity> entity)
{
	m_data.push_back(std::move(entity));
}

Cell::iterator Cell::find(Entity::Type type)
{
	return std::find_if(m_data.begin(), m_data.end(), [type](const std::unique_ptr<Entity>& entityPtr) -> bool
		{
			return entityPtr->getType() == type;
		}
	);
}

void Cell::erase(Entity::Type type)
{
	m_data.erase(this->find(type));
}

void Cell::erase(iterator it)
{
	m_data.erase(it);
}

Cell::iterator Cell::begin()
{
	return m_data.begin();
}

Cell::iterator Cell::end()
{
	return m_data.end();
}

Cell::const_iterator Cell::begin() const
{
	return m_data.cbegin();
}

Cell::const_iterator Cell::end() const
{
	return m_data.cend();
}