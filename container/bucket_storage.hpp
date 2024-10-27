#pragma once

#include "_block.hpp"

template< typename T >
class BucketStorage
{
  public:
	using value_type [[maybe_unused]] = T;
	using reference = T&;
	using const_reference [[maybe_unused]] = const T&;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;

	template< bool IsConst >
	class iterator_base;

	using iterator = iterator_base< false >;
	using const_iterator = iterator_base< true >;

	explicit BucketStorage(size_type cur_block_capacity = 64);
	BucketStorage(const BucketStorage& other);
	BucketStorage(BucketStorage&& other) noexcept;
	~BucketStorage();

	BucketStorage& operator=(const BucketStorage& other);
	BucketStorage& operator=(BucketStorage&& other) noexcept;

	template< typename U >
	iterator insert(U&& value);
	iterator erase(iterator it);

	[[nodiscard]] bool empty() const noexcept;
	[[nodiscard]] size_type size() const noexcept;
	[[nodiscard]] size_type capacity() const noexcept;

	void shrink_to_fit() noexcept;
	void free_blocks() noexcept;
	void clear() noexcept;
	void swap(BucketStorage& other) noexcept;

	iterator begin() noexcept;
	iterator end() noexcept;
	const_iterator begin() const noexcept;
	const_iterator cbegin() const noexcept;
	const_iterator end() const noexcept;
	const_iterator cend() const noexcept;

	iterator get_to_distance(iterator it, difference_type distance);

  private:
	Block< T >** m_blocks;
	size_type m_block_capacity{};
	size_type m_total_size{};
	size_type m_total_capacity{};
	size_type m_blocks_cnt{};

	void allocate_block();

	template< typename Iter >
	[[nodiscard]] Iter begin_impl() const noexcept;
};

#include "_bucket_storage_impl.hpp"
