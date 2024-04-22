#include "RewritableCircularBuffer.h"

#include <array>
#include <mutex>
#include <optional>

template <typename T, int maxSize>
RewritableCircularBuffer<T, maxSize>::RewritableCircularBuffer() :
	full(false), writeIdx(0), readIdx(0)
{
}

template <typename T, int maxSize>
RewritableCircularBuffer<T, maxSize>::~RewritableCircularBuffer()
{
}

template <typename T, int maxSize>
RewritableCircularBuffer<T, maxSize>::
	RewritableCircularBuffer(const RewritableCircularBuffer &copied) :
	full(copied.full), writeIdx(full ? 0 : copied.size()), readIdx(0)
{
	int elementsToCopy{copied.size()};
	if (elementsToCopy > 0)
	{
		int tempReadIdx{copied.readIdx};
		for (int i{0}; i < elementsToCopy; ++i)
		{
			buffer[i] = copied.buffer[tempReadIdx];
			advanceIdx(tempReadIdx);
		}
	}
}

template <typename T, int maxSize>
void RewritableCircularBuffer<T, maxSize>::Swap(RewritableCircularBuffer
	&other) noexcept
{
	//При помощи ADL ищет сначала перегрузки иначе использует std::swap
	using std::swap;
	swap(buffer, other.buffer);
	swap(writeIdx, other.writeIdx);
	swap(readIdx, other.readIdx);
	swap(full, other.full);
}

template <typename T, int maxSize>
RewritableCircularBuffer<T, maxSize>::
	RewritableCircularBuffer(RewritableCircularBuffer &&moved) noexcept :
	RewritableCircularBuffer()
{
	swap(*this, moved);
}

// Copy-swap idiom и Move-swap idiom
// если в качестве аргумента передан временный объект, то сработает
// перемещающий конструктор, если lvalue, то в любом случае потребовалось
// бы создавать вспомогательный объект (для обмена значений)
// и конструктор копирования будет вызван до входа в функцию
template <typename T, int maxSize>
RewritableCircularBuffer<T, maxSize> & RewritableCircularBuffer<T, maxSize>::
	operator=(RewritableCircularBuffer other) noexcept
{
	swap(*this, other);
	return *this;
}

template <typename T, int maxSize>
void RewritableCircularBuffer<T, maxSize>::reset()
{
	std::lock_guard<std::mutex> lock{mutex};
	writeIdx = readIdx;
	full = false;
}

template <typename T, int maxSize>
bool RewritableCircularBuffer<T, maxSize>::isEmpty() const
{
	return (!full && (writeIdx == readIdx));
}

template <typename T, int maxSize>
bool RewritableCircularBuffer<T, maxSize>::isFull() const
{
	return full;
}

template <typename T, int maxSize>
int RewritableCircularBuffer<T, maxSize>::capacity() const
{
	return maxSize;
}

template <typename T, int maxSize>
int RewritableCircularBuffer<T, maxSize>::size() const
{
	int size {maxSize};

	if (!full) {
		size = (writeIdx >= readIdx) ? writeIdx - readIdx :
			maxSize + writeIdx - readIdx;
	}

	return size;
}

template <typename T, int maxSize>
void RewritableCircularBuffer<T, maxSize>::advanceIdx(int &index)
{
	// Избегание операций деления по модулю, более производительно
	++index;
	if (index == maxSize){
		index = 0;
	}
}

// без применения Perfect forwarding больше дублирования кода
template <typename T, int maxSize>
void RewritableCircularBuffer<T, maxSize> ::put(T&& item)
{
	std::lock_guard<std::mutex> lock{ mutex };

	buffer[writeIdx] = std::move(item);

	if (full) {
		advanceIdx(readIdx);
	}

	advanceIdx(writeIdx);

	full = (writeIdx == readIdx);
}

template <typename T, int maxSize>
void RewritableCircularBuffer<T, maxSize> ::put(const T& item)
{
	std::lock_guard<std::mutex> lock{ mutex };

	buffer[writeIdx] = item;

	if (full) {
		advanceIdx(readIdx);
	}

	advanceIdx(writeIdx);

	full = (writeIdx == readIdx);
}

template <typename T, int maxSize>
std::optional<T> RewritableCircularBuffer<T, maxSize>::get()
{
	std::lock_guard<std::mutex> lock{mutex};
	// Вместо std::optional и std::nullopt можно возвращать T() -
	// значение по умолчанию для данного типа
	if(isEmpty()){ 
		return std::nullopt;
	}

	auto item {buffer[readIdx]};
	full = false;
	advanceIdx(readIdx);

	return item;
}

