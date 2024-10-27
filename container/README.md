# Лабораторная работа №3. STL-совместимый контейнер

Текущий статус тестирования GitHub Actions: [![CI/CD](../../actions/workflows/ci.yaml/badge.svg?branch=main&event=workflow_dispatch)](../../actions/workflows/ci.yaml).

> [!CAUTION]
> Изменять код в [`tests.cpp`](tests.cpp) и [`helpers.h`](helpers.h) **запрещено**. Интерфейс и реализация разрабатываемого класса прописывается в [`bucket_storage.hpp`](bucket_storage.hpp).

Вам предоставляется возможность запуска тестов локальным способом. Для этого нужно:

1. Поставить [GoogleTest](https://skkv-itmo.gitbook.io/ct-c-cpp-course/testing/gtest).
2. *(опционально)* В среде разработки для удобства следует разобраться с обозревателем тестов и тем, как создать проект с поддержкой `gtest`.

На сервере используется версия [`1.14.0`](https://github.com/google/googletest/tree/v1.14.0).

## Требования к оформлению `bucket_storage.hpp`:

Сначала описываются интерфейсы шаблонных классов, затем – реализация их методов.

Пример оформления:

```cpp
// ------------------------------------
// START OF INTERFACE
// ------------------------------------

template< typename T >
class BucketStorage
{
  public:
	BucketStorage();
	BucketStorage(const BucketStorage &other);
	BucketStorage(BucketStorage &&other);

	~BucketStorage();

  private:
};

// ------------------------------------
// START OF IMPLEMENTATION
// ------------------------------------

template< typename T >
BucketStorage< T >::BucketStorage()
{
}

template< typename T >
BucketStorage< T >::BucketStorage(const BucketStorage &other)
{
}

template< typename T >
BucketStorage< T >::BucketStorage(BucketStorage &&other)
{
}

template< typename T >
BucketStorage< T >::~BucketStorage()
{
}
```
