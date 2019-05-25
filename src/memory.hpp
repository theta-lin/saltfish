#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <cassert>
#include <algorithm>

template<typename T>
class FlatArray
{
private:
	size_t width;
	size_t height;
	T *array;

public:
	FlatArray() : width{0}, height{0}, array{nullptr}
	{
	}

	FlatArray(size_t width, size_t height) : FlatArray()
	{
		alloc(width, height);
	}

	FlatArray(const FlatArray<T>& src)
	{
		if (src.data())
		{
			alloc(src.width(), src.getHeight());
			std::copy(src.data(), src.data() + src.getWidth() * src.getHeight(), array);
		}
		else
		{
			free();
		}
	}

	~FlatArray()
	{
		free();
	}

	size_t getWidth() const { return width; }
	size_t getHeight() const { return height; }
	T* data() { return array; }
	const T* data() const { return array; }

	void alloc(size_t width, size_t height)
	{
		free();
		this->width = width;
		this->height = height;
		array = new T[width * height]{};
	}

	void free()
	{
		if (array)
		{
			width = 0;
			height = 0;
			delete[] array;
			array = nullptr;
		}
	}

	T& operator()(size_t col, size_t row)
	{
		assert(array);
		return array[row * width + col];
	}

	const T& operator()(size_t col, size_t row) const
	{
		assert(array);
		return array[row * width + col];
	}
};

#endif // ifndef MEMORY_HPP

