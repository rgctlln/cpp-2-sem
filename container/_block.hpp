#pragma once

#include <algorithm>
#include <cstdio>

template< typename T >
struct Block
{
	using size_type = std::size_t;

	T* m_elements;
	size_type* m_active;
	size_type m_size{};
	size_type m_capacity{};

	explicit Block(size_type block_capacity)
	{
		this->m_capacity = block_capacity;
		m_elements = static_cast< T* >(operator new[](sizeof(T) * m_capacity));
		m_active = static_cast< size_type* >(operator new[](sizeof(size_type) * m_capacity));
		std::fill(m_active, m_active + m_capacity, 1);
	}

	~Block()
	{
		for (size_type i = 0; i < m_capacity; ++i)
		{
			if (m_active[i] == 0)
			{
				m_elements[i].~T();
			}
		}
		operator delete[](m_elements);
		operator delete[](m_active);
	}
};
