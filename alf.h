
#pragma once

#include <cstddef>

#include <new>

namespace alf
{

typedef unsigned char	byte;

class bad_alloc : public std::bad_alloc
{
};

template <size_t T_num>
struct tag
{
	static constexpr size_t NUM = T_num;
};


template <size_t T_size>
struct memory_pool
{

/*

	 NULL   +-------------------------v
	+-|-----|-------------------------+
	|prev| end|X|X|X|X|X|X|X|X|X|X|X|X|
	+---------------------------------+
	 ^
	 ++     +-------------------------v
	+-|-----|-------------------------+
	|prev| end|X|X|X|X|X|X|X|X|X|X|X|X|
	+---------------------------------+
	 ^
	 ++     +-------------------------v
	+-|-----|-------------------------+
	|prev| end|X|X|X|X| | | | | | | | |
	+---------------------------------+
	 ^                 ^
	 |                 |
	head              current
	(block)           (first free in block)

*/

public:

	memory_pool () : head( create(T_size) )
				   , current( first() )
	{
	}

	~memory_pool ()
	{
		current = nullptr;

		while (head != nullptr)
		{
			byte* tmp_prev = prev();
			delete[] head;
			head = tmp_prev;
		}
	}

	byte* allocate (size_t _size)
	{
		byte* memory = current;
		current += _size;
		if (current >= end())
			return nullptr;
		return memory;
	}

	byte* grow_and_allocate (size_t _size)
	{
		byte* new_data = create( std::max(T_size, _size) );
		prev(new_data) = head;

		head = new_data;
		current = first();

		return allocate (_size);
	}

	void deallocate (byte* _pointer)
	{
		// TODO
	}

	byte* head;
	byte* current;

private:

	static byte* create (size_t _size)
	{
		const size_t size = _size + (2*sizeof (byte*) ) +1;
		byte* data = new byte[size];
		prev(data) = nullptr;
		end(data) = data + size;

		return data;
	}

	byte*& prev () { return prev(head); }
	static byte*& prev (byte* _head)
	{
		return *reinterpret_cast<byte**>( _head 
										+ 0 );
	}

	byte*& end () { return end(head); }
	static byte*& end (byte* _head)
	{
		return *reinterpret_cast<byte**>( _head 
										+ sizeof (byte*) );
	}

	byte* first () { return first(head); }
	static byte* first (byte* _head)
	{
		return  _head 
				+ ( 2*sizeof (byte*) );
	}
};

template <size_t T_size = 0, typename T_tag = void>
class default_memory_policy
{
public:
	typedef void		value_type;
	typedef value_type*	pointer;
	typedef size_t		size_type;

	typedef T_tag 		tag_type;

	static pointer allocate (size_type _size)
	{
		return operator new (_size);
	}

	static void deallocate (pointer _pointer)
	{
		operator delete (_pointer);
	}
};

template <size_t T_size, typename T_tag = void>
class fixed_size_memory_policy
{
public:
	typedef void		value_type;
	typedef value_type*	pointer;
	typedef size_t		size_type;

	typedef T_tag 		tag_type;

	static pointer allocate (size_type _size)
	{
		static memory_pool<T_size> data;

		byte* memory = data.allocate (_size);
		if (memory == nullptr)
			throw alf::bad_alloc();
	
		return static_cast<void*>(memory);
	}

	static void deallocate (pointer _pointer)
	{}
};

template <size_t T_size, typename T_tag = void>
class dynamic_size_fast_memory_policy
{
public:
	typedef void		value_type;
	typedef value_type*	pointer;
	typedef size_t		size_type;

	typedef T_tag 		tag_type;

	static pointer allocate (size_type _size)
	{
		static memory_pool<T_size> data;

		byte* memory = data.allocate (_size);
		if (memory == nullptr)
			memory = data.grow_and_allocate (_size);
	
		return static_cast<void*>(memory);
	}

	static void deallocate (pointer _pointer)
	{	}
};

template <size_t T_size, typename T_tag = void>
class dynamic_size_extended_memory_policy
{
public:
	typedef void		value_type;
	typedef value_type*	pointer;
	typedef size_t		size_type;

	typedef T_tag 		tag_type;

	static pointer allocate (size_type _size)
	{
		byte* memory = data().allocate (_size);
		if (memory == nullptr)
			memory = data().grow_and_allocate (_size);
	
		return static_cast<void*>(memory);
	}

	static void deallocate (pointer _pointer)
	{
		data().deallocate( static_cast<byte*>(_pointer) );
	}

private:
	static memory_pool<T_size>& data()
	{
		static memory_pool<T_size> data;
		return data;
	}
};
template <size_t T_size, typename T_tag = void>
using dynamic_size_memory_policy = dynamic_size_extended_memory_policy<T_size, T_tag>;

template <typename T, typename T_memory_policy = default_memory_policy<0, void>> 
class allocator
{
public:

	typedef allocator<T, T_memory_policy>	my_type;

	typedef T					value_type;
	typedef value_type*			pointer;
	typedef const value_type*	const_pointer;
	typedef value_type&			reference;
	typedef const value_type&	const_reference;
	typedef size_t				size_type;
	typedef ptrdiff_t			difference_type;
	
	typedef T_memory_policy		memory;

	template <typename U> 
	struct rebind 
	{
		typedef allocator<U, T_memory_policy> other;
	};

	allocator()
	{}
	
	allocator(const allocator&)
	{}

	template <typename U>
	allocator(const allocator<U>&)
	{}
	
	~allocator()
	{}


	pointer address (reference _reference) const 
	{
		return &_reference;
	}

	const_pointer address (const_reference _reference) const 
	{
		return &_reference;
	}

	size_t max_size () const 
	{
		return (static_cast<size_type>(0) - static_cast<size_type>(1)) 
				/ sizeof(value_type);
	}

	bool operator== (const my_type& /*_other*/) const 
	{
		return true;
	}

	bool operator != (const my_type& _other) const 
	{
		return !(*this == _other);
	}

	pointer allocate (const size_t _size) const 
	{
		return reinterpret_cast<pointer>(
			memory::allocate (_size * sizeof (value_type) ) 
		);
	}

	template <typename U> 
	pointer allocate(const size_t _size, const U* /*const _hint*/) const 
	{
		return allocate(_size);
	}

	void deallocate (pointer const _pointer, const size_t _size) const 
	{
		memory::deallocate(_pointer);
	}

	void construct (pointer const _pointer, const_reference _object) const 
	{	
		::new((void *)_pointer) value_type(
			std::forward<const_reference>(_object)
		);
	}

	void destroy (pointer const _pointer) const
	{
		_pointer->~value_type();
	}
};

} // namespace alf
