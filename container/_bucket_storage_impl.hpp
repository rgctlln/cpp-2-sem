#pragma once

#include "bucket_storage.hpp"

#include <algorithm>
#include <cstring>
#include <stdexcept>

template< typename T >
BucketStorage< T >::BucketStorage(size_type cur_block_capacity) :
	m_blocks(nullptr), m_block_capacity(cur_block_capacity), m_total_size(0), m_total_capacity(0), m_blocks_cnt(0)
{
}

template< typename T >
BucketStorage< T >::BucketStorage(const BucketStorage& other) : BucketStorage()
{
	*this = other;
}

template< typename T >
BucketStorage< T >::BucketStorage(BucketStorage&& other) noexcept :
	m_blocks(other.m_blocks), m_block_capacity(other.m_block_capacity), m_total_size(other.m_total_size),
	m_total_capacity(other.m_total_capacity), m_blocks_cnt(other.m_blocks_cnt)
{
	other.m_blocks = nullptr;
	other.m_total_size = 0;
	other.m_total_capacity = 0;
	other.m_blocks_cnt = 0;
}

template< typename T >
void BucketStorage< T >::free_blocks() noexcept
{
	for (size_type i = 0; i < this->m_blocks_cnt; ++i)
	{
		delete this->m_blocks[i];
	}
	delete[] this->m_blocks;
	this->m_blocks = nullptr;
	this->m_blocks_cnt = 0;
	this->m_total_size = 0;
	this->m_total_capacity = 0;
}

template< typename T >
void BucketStorage< T >::clear() noexcept
{
	free_blocks();
}

template< typename T >
BucketStorage< T >::~BucketStorage()
{
	clear();
}

template< typename T >
BucketStorage< T >& BucketStorage< T >::operator=(const BucketStorage& other)
{
	if (this != &other)
	{
		clear();
		this->m_block_capacity = other.m_block_capacity;
		this->m_total_size = 0;
		this->m_total_capacity = 0;
		this->m_blocks_cnt = other.m_blocks_cnt;
		if (m_blocks_cnt > 0)
		{
			m_blocks = new Block< T >*[m_blocks_cnt];
			for (size_type i = 0; i < m_blocks_cnt; ++i)
			{
				m_blocks[i] = new Block< T >(m_block_capacity);
			}
			this->m_total_capacity = m_blocks_cnt * m_block_capacity;
			for (auto it = other.begin(); it != other.end(); ++it)
			{
				insert(*it);
			}
		}
	}
	return *this;
}

template< typename T >
BucketStorage< T >& BucketStorage< T >::operator=(BucketStorage&& other) noexcept
{
	if (this != &other)
	{
		clear();
		swap(other);
		other.m_blocks = nullptr;
		other.m_total_size = 0;
		other.m_total_capacity = 0;
		other.m_blocks_cnt = 0;
	}
	return *this;
}

template< typename T >
template< typename U >
typename BucketStorage< T >::iterator BucketStorage< T >::insert(U&& value)
{
	if (m_total_size == m_total_capacity)
	{
		allocate_block();
	}
	for (size_type i = 0; i < m_blocks_cnt; ++i)
	{
		Block< T >* current = m_blocks[i];
		for (size_type j = 0; j < m_block_capacity; ++j)
		{
			if (current->m_active[j] > 0)
			{
				new (current->m_elements + j) T(std::forward< U >(value));
				current->m_active[j] = 0;
				++current->m_size;
				++m_total_size;
				return iterator(this, current, j);
			}
		}
	}
	throw std::runtime_error("ERROR: Unable to insert value");
}

template< typename T >
typename BucketStorage< T >::iterator BucketStorage< T >::begin() noexcept
{
	return begin_impl< iterator >();
}

template< typename T >
typename BucketStorage< T >::const_iterator BucketStorage< T >::begin() const noexcept
{
	return begin_impl< const_iterator >();
}

template< typename T >
typename BucketStorage< T >::const_iterator BucketStorage< T >::cbegin() const noexcept
{
	return begin();
}

template< typename T >
typename BucketStorage< T >::iterator BucketStorage< T >::end() noexcept
{
	return iterator(this, nullptr, 0);
}

template< typename T >
typename BucketStorage< T >::const_iterator BucketStorage< T >::end() const noexcept
{
	return const_iterator(this, nullptr, 0);
}

template< typename T >
typename BucketStorage< T >::const_iterator BucketStorage< T >::cend() const noexcept
{
	return end();
}

template< typename T >
void BucketStorage< T >::allocate_block()
{
	auto** new_blocks = new Block< T >*[m_blocks_cnt + 1];
	if (m_blocks)
	{
		if (std::memcpy(new_blocks, m_blocks, m_blocks_cnt * sizeof(Block< T >*)) != new_blocks)
		{
			delete[] new_blocks;
			throw std::runtime_error("ERROR: Failed to copy memory");
		}
		delete[] m_blocks;
	}
	m_blocks = new_blocks;
	m_blocks[m_blocks_cnt] = new Block< T >(m_block_capacity);
	++m_blocks_cnt;
	m_total_capacity += m_block_capacity;
}

template< typename T >
bool BucketStorage< T >::empty() const noexcept
{
	return this->m_total_size == 0;
}

template< typename T >
typename BucketStorage< T >::size_type BucketStorage< T >::size() const noexcept
{
	return this->m_total_size;
}

template< typename T >
typename BucketStorage< T >::size_type BucketStorage< T >::capacity() const noexcept
{
	return this->m_total_capacity;
}

template< typename T >
void BucketStorage< T >::shrink_to_fit() noexcept
{
	BucketStorage< T > new_storage(m_block_capacity);
	for (size_type i = 0; i < m_blocks_cnt; ++i)
	{
		Block< T >* current = m_blocks[i];
		for (size_type j = 0; j < m_block_capacity; ++j)
		{
			if (current->m_active[j] == 0)
			{
				new_storage.insert(std::move(current->m_elements[j]));
				current->m_elements[j].~T();
			}
		}
	}
	swap(new_storage);
}

template< typename T >
void BucketStorage< T >::swap(BucketStorage& other) noexcept
{
	std::swap(m_blocks, other.m_blocks);
	std::swap(m_block_capacity, other.m_block_capacity);
	std::swap(m_total_size, other.m_total_size);
	std::swap(m_total_capacity, other.m_total_capacity);
	std::swap(m_blocks_cnt, other.m_blocks_cnt);
}

template< typename T >
template< typename Iter >
Iter BucketStorage< T >::begin_impl() const noexcept
{
	for (size_type i = 0; i < m_blocks_cnt; ++i)
	{
		Block< T >* current = m_blocks[i];
		for (size_type j = 0; j < m_block_capacity; ++j)
		{
			if (current->m_active[j] == 0)
			{
				return Iter(const_cast< BucketStorage* >(this), current, j);
			}
		}
	}
	return Iter(const_cast< BucketStorage* >(this), nullptr, 0);
}

template< typename T >
typename BucketStorage< T >::iterator BucketStorage< T >::erase(iterator it)
{
	if (it == end())
	{
		throw std::out_of_range("ERROR: Iterator out of range");
	}
	Block< T >* block = it.m_block;
	size_type index = it.m_index;
	block->m_elements[index].~T();
	block->m_active[index] = 1;
	--block->m_size;
	--m_total_size;

	iterator next_it = it;
	++next_it;

	if (block->m_size != 0)
	{
		return next_it;
	}
	for (size_type i = 0; i < m_blocks_cnt; ++i)
	{
		if (m_blocks[i] == block)
		{
			delete block;
			std::memmove(m_blocks + i, m_blocks + i + 1, (m_blocks_cnt - i - 1) * sizeof(Block< T >*));
			--m_blocks_cnt;
			m_total_capacity -= m_block_capacity;
			break;
		}
	}
	return next_it;
}

template< typename T >
typename BucketStorage< T >::iterator BucketStorage< T >::get_to_distance(iterator it, difference_type distance)
{
	while (distance != 0)
	{
		if (distance > 0)
		{
			++it;
			--distance;
		}
		else
		{
			--it;
			++distance;
		}
		if (it == end())
		{
			throw std::out_of_range("ERROR: Distance out of range");
		}
	}
	return it;
}

template< typename T >
template< bool IsConst >
class BucketStorage< T >::iterator_base
{
  public:
	using iterator_category [[maybe_unused]] = std::bidirectional_iterator_tag;
	using value_type = std::conditional_t< IsConst, const T, T >;
	using difference_type [[maybe_unused]] = std::ptrdiff_t;
	using pointer = value_type*;
	using reference = value_type&;

	using StoragePointer = std::conditional_t< IsConst, const BucketStorage*, BucketStorage* >;
	using BlockPointer = std::conditional_t< IsConst, const Block< T >*, Block< T >* >;

	iterator_base() noexcept;
	iterator_base(StoragePointer storage, BlockPointer block, size_type index) noexcept;

	reference operator*() const { return m_block->m_elements[m_index]; }
	pointer operator->() const { return &m_block->m_elements[m_index]; }

	iterator_base& operator++() noexcept;
	iterator_base& operator--() noexcept;
	const iterator_base operator++(int) noexcept;
	const iterator_base operator--(int) noexcept;

	bool operator==(const iterator_base& other) const noexcept;
	bool operator!=(const iterator_base& other) const noexcept { return !(*this == other); }
	bool operator<(const iterator_base& other) const noexcept;
	bool operator<=(const iterator_base& other) const noexcept { return *this < other || *this == other; }
	bool operator>(const iterator_base& other) const noexcept { return !(*this <= other); }
	bool operator>=(const iterator_base& other) const noexcept { return !(*this < other); }

	template< bool OtherIsConst >
	bool operator==(const iterator_base< OtherIsConst >& other) const noexcept;

	template< bool OtherIsConst >
	bool operator<(const iterator_base< OtherIsConst >& other) const noexcept;

	template< bool OtherIsConst >
	bool operator!=(const iterator_base< OtherIsConst >& other) const noexcept
	{
		return !(*this == other);
	}

	template< bool OtherIsConst >
	bool operator<=(const iterator_base< OtherIsConst >& other) const noexcept
	{
		return *this < other || *this == other;
	}

	template< bool OtherIsConst >
	bool operator>(const iterator_base< OtherIsConst >& other) const noexcept
	{
		return !(*this <= other);
	}

	template< bool OtherIsConst >
	bool operator>=(const iterator_base< OtherIsConst >& other) const noexcept
	{
		return !(*this < other);
	}

  private:
	StoragePointer m_storage;
	BlockPointer m_block;
	size_type m_index{};

	friend class BucketStorage< T >;
};

#include "_iterator_base_impl.hpp"
