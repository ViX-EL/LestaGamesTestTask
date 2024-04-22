#include "DynamicCircularBuffer.h"

#include <deque>
#include <mutex>
#include <optional>
#include <utility>

template <typename T>
DynamicCircularBuffer<T>::DynamicCircularBuffer()
{
}

template <typename T>
DynamicCircularBuffer<T>::~DynamicCircularBuffer()
{
}

template <typename T>
DynamicCircularBuffer<T>::DynamicCircularBuffer(const DynamicCircularBuffer 
	&copied)
{
	buffer = copied.buffer;
}

template<typename T>
void DynamicCircularBuffer<T>::Swap(DynamicCircularBuffer& other) noexcept
{
	std::swap(buffer, other.buffer);
}

template <typename T>
DynamicCircularBuffer<T>::DynamicCircularBuffer(DynamicCircularBuffer &&moved)
	noexcept
{
	swap(*this, moved);
}

// Copy-swap idiom и Move-swap idiom, аргумент передаётся по значению намеренно
// если в качестве аргумента передан временный объект, то сработает
// перемещающий конструктор, если lvalue, то в любом случае потребовалось
// бы создавать вспомогательный объект (для обмена значений)
// и конструктор копирования будет вызван до входа в функцию
template <typename T>
DynamicCircularBuffer<T> & DynamicCircularBuffer<T>::operator=
	(const DynamicCircularBuffer other) noexcept
{
	swap(*this, other);
	return *this;
}

template <typename T>
void DynamicCircularBuffer<T>::clear()
{
	std::lock_guard<std::mutex> lock{mutex};
	buffer.clear();
}

template <typename T>
bool DynamicCircularBuffer<T>::isEmpty() const
{
	return buffer.empty();
}

template <typename T>
int DynamicCircularBuffer<T>::size() const
{
	return static_cast<int>(buffer.size());
}

//Perfect forwarding и variadic template
template <typename T>
template <typename... Args>
void DynamicCircularBuffer<T>::put(Args &&...arguments)
{
    std::lock_guard<std::mutex> lock{mutex};

    buffer.emplace_back(std::forward<Args>(arguments)...);
}

template <typename T>
std::optional<T> DynamicCircularBuffer<T>::get()
{
	std::lock_guard<std::mutex> lock{mutex};
	// Вместо std::optional и std::nullopt можно возвращать T() -
	// значение по умолчанию для данного типа
	if(buffer.empty()){
		return std::nullopt;
	}

	auto item{buffer.front()};
	buffer.pop_front();

	return item;
}
