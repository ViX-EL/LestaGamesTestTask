#include "CircularBuffer.h"
#include <iostream>

CircularBuffer::CircularBuffer(size_t size = 0) :
	maxSize(size), buffer(size > 0 ? new int[size] : nullptr),
	//buffer(size > 0 ? std::make_unique<int[]>(size) : nullptr),
	writeIdx(0), readIdx(0), full(false) 
{
}

CircularBuffer::~CircularBuffer()
{
	delete[] buffer;
}

CircularBuffer::CircularBuffer(const CircularBuffer &copied)
{
	copyAvailableForReading(copied, copied.maxSize);
}

CircularBuffer::CircularBuffer(CircularBuffer &&moved) noexcept :
	CircularBuffer()
{
	swap(*this, moved);
}

void swap(CircularBuffer &right, CircularBuffer &left) noexcept
{
	//При помощи ADL ищет сначала перегрузки иначе использует std::swap
	using std::swap;
	swap(left.buffer, right.buffer);
	swap(left.maxSize, right.maxSize);
	swap(left.writeIdx, right.writeIdx);
	swap(left.readIdx, right.readIdx);
	swap(left.full, right.full);
}

// Copy-swap idiom и Move-swap idiom, аргумент передаётся по значению намеренно
// если в качестве аргумента передан временный объект, то сработает
// перемещающий конструктор, если lvalue, то в любом случае потребовалось
// бы создавать вспомогательный объект (для обмена значений)
// и конструктор копирования будет вызван до входа в функцию
CircularBuffer & CircularBuffer::operator=(CircularBuffer other) noexcept
{
	swap(*this, other);
	return *this;
}

void CircularBuffer::reset()
{
	writeIdx = 0;
	readIdx = 0;
	full = false;
}

bool CircularBuffer::isEmpty() const
{
	return (!full && (writeIdx == readIdx));
}

bool CircularBuffer::isFull() const
{
	return full;
}

bool CircularBuffer::isNull() const
{
	return !buffer;
}

size_t CircularBuffer::capacity() const
{
	return maxSize;
}

size_t CircularBuffer::size() const
{
	size_t size {maxSize};

	if(!full){
		size = (writeIdx >= readIdx) ? writeIdx - readIdx :
			maxSize + writeIdx - readIdx;
	}

	return size;
}

void CircularBuffer::erase()
{
	delete[] buffer;
	buffer = nullptr;
	maxSize = 0;
	reset();
}

// Глубокое копирование
void CircularBuffer::copyAvailableForReading(const CircularBuffer& copied,
	size_t newCapacity)
{
	int* newBuffer{newCapacity > 0 ? new int[newCapacity] : nullptr};

	if ((newBuffer) && (copied.maxSize > 0) && (copied.size() > 0))
	{
		size_t available{copied.size()};
		long long elementsNotFit = newCapacity - available;
		size_t elementsToCopy{(elementsNotFit < 0) ? available +
		elementsNotFit : available};
		size_t tempReadIdx{copied.readIdx};
		for (size_t i{0}; i < elementsToCopy; ++i)
		{
			// Здесь показывается warning, не уверен как его правильно
			// исправить, но ошибок при тестировании не обнаружено
			newBuffer[i] = copied.buffer[tempReadIdx];
			copied.advanceIdx(tempReadIdx);
		}

		full = (newCapacity == elementsToCopy);
		writeIdx = full ? 0 : elementsToCopy;
		readIdx = 0;
	}

	if (&copied == this)
		delete[] buffer;

	if (newCapacity == 0)
		reset();

	buffer = newBuffer;
	maxSize = newCapacity;
}

// Изменяет размер массива. Любые существующие элементы будут уничтожены.
// Эта функция работает быстро. Быстрее resize из-за отсутствия копирования.
void CircularBuffer::reallocate(size_t newCapacity)
{
	erase();

	if (newCapacity == 0)
		return;

	buffer = new int[newCapacity];
	maxSize = newCapacity;
}

// Изменяет размер массива. Элементы, доступные для чтения будут сохранены.
// Эта функция работает медленно. Медленней чем reallocate из-за глубокого
// копирования.
void CircularBuffer::resize(size_t newCapacity)
{
	if (newCapacity == maxSize)
		return;

	if (newCapacity == 0)
	{
		erase();
		return;
	}

	copyAvailableForReading(*this, newCapacity);
}

void CircularBuffer::advanceIdx(size_t &index) const
{
	// Более затратная  операция деления по модулю чем просто if
	index = (index + 1) % maxSize; // нужно остерегаться деления на 0
}

bool CircularBuffer::put(int item)
{
	if (isNull()) {
		// Можно бросить исключение
		std::cout << "Buffer is null, need to resize it!";
		return false;
	}

	if(full) {
		std::cout << "Buffer is full!\n"; // Можно бросить исключение
		return false;
	}
	
	buffer[writeIdx] = item;
	advanceIdx(writeIdx);
	full = (writeIdx == readIdx);
	return true;
}

int CircularBuffer::get()
{
	if (isNull()) {
		// Можно бросить исключение
		std::cout << "Buffer is null, need to reallocate it!";
		return 0;
	}

	if(isEmpty()) {
		std::cout << "Buffer is empty!\n"; // Можно бросить исключение
		return 0; // Возвращается значение по умолчанию - 0
	}
	int item{buffer[readIdx]};
	full = false;
	advanceIdx(readIdx);

	return item;
}