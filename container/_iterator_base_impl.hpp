#pragma once

#include "bucket_storage.hpp"

template< typename T >
template< bool IsConst >
BucketStorage< T >::iterator_base< IsConst >::iterator_base() noexcept :
	m_storage(nullptr), m_block(nullptr), m_index(0)
{
}

template< typename T >
template< bool IsConst >
template< bool OtherIsConst >
bool BucketStorage< T >::iterator_base< IsConst >::operator==(const iterator_base< OtherIsConst >& other) const noexcept
{
	return m_storage == other.m_storage && m_block == other.m_block && m_index == other.m_index;
}

template< typename T >
template< bool IsConst >
template< bool OtherIsConst >
bool BucketStorage< T >::iterator_base< IsConst >::operator<(const iterator_base< OtherIsConst >& other) const noexcept
{
	if (m_block == other.m_block)
	{
		return m_index < other.m_index;
	}
	return m_block < other.m_block;
}

template< typename T >
template< bool IsConst >
BucketStorage< T >::iterator_base< IsConst >::iterator_base(StoragePointer it_storage, BlockPointer it_block, size_type it_index) noexcept
	: m_storage(it_storage), m_block(it_block), m_index(it_index)
{
}

template< typename T >
template< bool IsConst >
bool BucketStorage< T >::iterator_base< IsConst >::operator==(const iterator_base& other) const noexcept
{
	return m_storage == other.m_storage && m_block == other.m_block && m_index == other.m_index;
}

template< typename T >
template< bool IsConst >
bool BucketStorage< T >::iterator_base< IsConst >::operator<(const iterator_base& other) const noexcept
{
	if (m_block == other.m_block)
	{
		return m_index < other.m_index;
	}
	auto block_a = std::find(m_storage->m_blocks, m_storage->m_blocks + m_storage->m_blocks_cnt, m_block);
	auto block_b = std::find(m_storage->m_blocks, m_storage->m_blocks + m_storage->m_blocks_cnt, other.m_block);
	return block_a < block_b;
}

template< typename T >
template< bool IsConst >
const typename BucketStorage< T >::template iterator_base< IsConst >
	BucketStorage< T >::iterator_base< IsConst >::operator--(int) noexcept
{
	iterator_base tmp = *this;
	--(*this);
	return tmp;
}

template< typename T >
template< bool IsConst >
typename BucketStorage< T >::template iterator_base< IsConst >& BucketStorage< T >::iterator_base< IsConst >::operator--() noexcept
{
	do
	{
		if (m_index == 0)
		{
			BlockPointer prev = nullptr;
			for (size_type i = 0; i < m_storage->m_blocks_cnt; ++i)
			{
				if (m_storage->m_blocks[i] == m_block)
				{
					break;
				}
				prev = m_storage->m_blocks[i];
			}
			m_block = prev;
			m_index = m_block ? m_block->m_capacity - 1 : 0;
		}
		else
		{
			--m_index;
		}
	} while (m_block && m_block->m_active[m_index] != 0);
	return *this;
}

template< typename T >
template< bool IsConst >
const typename BucketStorage< T >::template iterator_base< IsConst >
	BucketStorage< T >::iterator_base< IsConst >::operator++(int) noexcept
{
	iterator_base tmp = *this;
	++(*this);
	return tmp;
}

template< typename T >
template< bool IsConst >
typename BucketStorage< T >::template iterator_base< IsConst >& BucketStorage< T >::iterator_base< IsConst >::operator++() noexcept
{
	if (m_block == nullptr)
		return *this;
	do
	{
		++m_index;
		while (m_block != nullptr && m_index >= m_block->m_capacity)
		{
			BlockPointer next_block = nullptr;
			for (size_type i = 0; i < m_storage->m_blocks_cnt; ++i)
			{
				if (m_storage->m_blocks[i] == m_block && i + 1 < m_storage->m_blocks_cnt)
				{
					next_block = m_storage->m_blocks[i + 1];
					break;
				}
			}
			m_block = next_block;
			m_index = 0;
		}
	} while (m_block != nullptr && m_index < m_block->m_capacity && m_block->m_active[m_index]);
	return *this;
}
