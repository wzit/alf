
#include <iostream>
#include <fstream>
#include <chrono>

#include <cstdlib>
#include <ctime>

#include <list>
#include <vector>
#include <map>

#include "alf.h"

constexpr size_t SIZES[] = { 1, 16, 128, 512, 1024, 2048, 4096 };

template <size_t T_size>
struct Big
{
private:
	alf::byte dummy[T_size];
};

class Timer
{
public:
	Timer() : start_(std::chrono::high_resolution_clock::now())
	{};

	void Reset()
	{
		start_ = std::chrono::high_resolution_clock::now();
	}
	
	std::chrono::duration<long int, std::ratio<1l, 1000000000l> > Get()
	{
		return std::chrono::high_resolution_clock::now() - start_;
	}

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> start_;
};


template <class Type, class MemoryPolicy>
using vector = std::vector< Type, alf::allocator<Type, MemoryPolicy> >;

template <class Type, class MemoryPolicy>
using list = std::list< Type, alf::allocator<Type, MemoryPolicy> >;

template <class Type, class MemoryPolicy>
using map = std::map< size_t, 
					  Type, 
					  std::less<size_t>, 
					  alf::allocator<std::pair<const size_t, Type>, MemoryPolicy>
					>;

template <typename T_Type, typename T_MemoryPolicy>
void AddToContainer( size_t _index
				   , map<T_Type, T_MemoryPolicy>& _container
				   , const typename map<T_Type, T_MemoryPolicy>::value_type& _object )
{
	_container.insert( std::make_pair(rand(), _object.second) );
}

template <typename T_Type, typename T_MemoryPolicy>
void AddToContainer( size_t _index
				   , vector<T_Type, T_MemoryPolicy>& _container
				   , const typename vector<T_Type, T_MemoryPolicy>::value_type& _object )
{
	_container.push_back(_object);
}

template <typename T_Type, typename T_MemoryPolicy>
void AddToContainer( size_t _index
				   , list<T_Type, T_MemoryPolicy>& _container
				   , const typename list<T_Type, T_MemoryPolicy>::value_type& _object )
{
	_container.push_back(_object);
}

template <typename T_container, size_t CONTAINER_SIZE>
std::chrono::duration<long int, std::ratio<1l, 1000000000l> > Test()
{
	T_container container;
	typename T_container::value_type object{};
	AddToContainer( 0, container, object );
	
	Timer timer;
	timer.Reset();
	for (size_t i{ 0 }; i < 1000; ++i )
		AddToContainer( i, container, object );
	return timer.Get();
}

template <size_t SIZE_INDEX>
struct TestedType
{
	static constexpr size_t SIZE  = SIZES[SIZE_INDEX];
	using Type = Big<SIZE>;
};

template <>
struct TestedType<7>
{
	using Type = bool;
};

template <>
struct TestedType<8>
{
	using Type = int;
};

template <>
struct TestedType<9>
{
	using Type = long long;
};

template < size_t SIZE_INDEX, size_t CONTAINER_SIZE, size_t PRESCALLER
		 , template<class, class> class T_container
		 , template<size_t> class T_memory_policy>
void TestImpl()
{	
	try
	{
		using Type = typename TestedType<SIZE_INDEX>::Type;
		constexpr size_t MEMORY_SIZE = (sizeof(Type) << PRESCALLER);
		std::cout << "\t" << Test< T_container< Type
										      , T_memory_policy<MEMORY_SIZE>>
								  , CONTAINER_SIZE 
							 	  > ().count();
	}
	catch (alf::bad_alloc& e)
	{
		std::cout << -3;
	}
	catch (...)
	{
		std::cout << -4;
	}
}

template < size_t SIZE_INDEX, size_t PRESCALLER
		 , template<class, class> class T_container
		 , template<size_t> class T_memory_policy>
void Test( char _container_size )
{
	switch (_container_size)
	{
		case '0':	TestImpl<SIZE_INDEX,    1000, PRESCALLER, T_container, T_memory_policy>( ); break;
		case '1':	TestImpl<SIZE_INDEX,    2000, PRESCALLER, T_container, T_memory_policy>( ); break;
		case '2':	TestImpl<SIZE_INDEX,    5000, PRESCALLER, T_container, T_memory_policy>( ); break;
		case '3':	TestImpl<SIZE_INDEX,   10000, PRESCALLER, T_container, T_memory_policy>( ); break;
		case '4':	TestImpl<SIZE_INDEX,   20000, PRESCALLER, T_container, T_memory_policy>( ); break;
		case '5':	TestImpl<SIZE_INDEX,   50000, PRESCALLER, T_container, T_memory_policy>( ); break;
		case '6':	TestImpl<SIZE_INDEX,  100000, PRESCALLER, T_container, T_memory_policy>( ); break;
		case '7':	TestImpl<SIZE_INDEX,  200000, PRESCALLER, T_container, T_memory_policy>( ); break;
		case '8':	TestImpl<SIZE_INDEX,  500000, PRESCALLER, T_container, T_memory_policy>( ); break;
		case '9':	TestImpl<SIZE_INDEX, 1000000, PRESCALLER, T_container, T_memory_policy>( ); break;
	}
}

template < template<class, class> class T_container
		  , template<size_t> class T_memory_policy
		  , size_t PRESCALLER >
void TestOneContainerOneMemoryPolicy( char _size_index, char _container_size )
{
	switch (_size_index)
	{
		case '0':	Test<0, PRESCALLER, T_container, T_memory_policy>( _container_size );	break;
		case '1':	Test<1, PRESCALLER, T_container, T_memory_policy>( _container_size );	break;
		case '2':	Test<2, PRESCALLER, T_container, T_memory_policy>( _container_size );	break;
		case '3':	Test<3, PRESCALLER, T_container, T_memory_policy>( _container_size );	break;
		case '4':	Test<4, PRESCALLER, T_container, T_memory_policy>( _container_size );	break;
		case '5':	Test<5, PRESCALLER, T_container, T_memory_policy>( _container_size );	break;
		case '6':	Test<6, PRESCALLER, T_container, T_memory_policy>( _container_size );	break;

		case '7':	Test<7, PRESCALLER, T_container, T_memory_policy>( _container_size );	break;
		case '8':	Test<8, PRESCALLER, T_container, T_memory_policy>( _container_size );	break;
		case '9':	Test<9, PRESCALLER, T_container, T_memory_policy>( _container_size );	break;
	}	
}

template <template <size_t, class> class T_raw_memory_policy>
struct MemoryPolicy
{
	template < size_t SIZE > 
	using Type = T_raw_memory_policy<SIZE, alf::tag<SIZE>>;
};

template < template<class, class> class T_container >
void TestOneContainer( char _size_index, char _memory_policy, char _container_size )
{
	switch (_memory_policy)
	{
	case 's': //stl
		TestOneContainerOneMemoryPolicy<
			T_container, 
			MemoryPolicy<alf::default_memory_policy>::Type, 63>(_size_index, _container_size);
		break;

	case 'f': //fixed
		TestOneContainerOneMemoryPolicy<
			T_container, 
			MemoryPolicy<alf::fixed_size_memory_policy>::Type, 63>(_size_index, _container_size);
		break;

	case 'd':
		TestOneContainerOneMemoryPolicy<
			T_container, 
			MemoryPolicy<alf::dynamic_size_fast_memory_policy>::Type, 63>(_size_index, _container_size);
		break;

	case 'e':
		TestOneContainerOneMemoryPolicy<
			T_container, 
			MemoryPolicy<alf::dynamic_size_extended_memory_policy>::Type, 63>(_size_index, _container_size);
		break;
	}
}

int main(int argc, char* argv[])
{
	srand (time(NULL));

	if (argc < 5)
	{
		std::cout 		 << "Usage: \"alf_test <container> <element_size> <memory_policy> <container_size>\""
			<< std::endl << "<container>:     [l]ist"
			<< std::endl << "                 [v]ector"
			<< std::endl << "                 [m]ap"
			<< std::endl << "<element_size>:  [0] - > 1"
			<< std::endl << "                 [1] - > 16"
			<< std::endl << "                 [2] - > 128"
			<< std::endl << "                 [3] - > 512"
			<< std::endl << "                 [4] - > 1024"
			<< std::endl << "                 [5] - > 2048"
			<< std::endl << "                 [6] - > 4096"
			<< std::endl << "                 [7] - > bool"
			<< std::endl << "                 [8] - > int"
			<< std::endl << "                 [9] - > long_long"
			<< std::endl << "<memory_policy>: [s]td::allocator"
			<< std::endl << "                 [f]ixed_size"
			<< std::endl << "                 [d]ynamic_size_fast"
			<< std::endl << "                 [e]xtended_dynamic_size"
			<< std::endl << "<container_size>:[0] - >     1 000"
			<< std::endl << "<container_size>:[1] - >     2 000"
			<< std::endl << "<container_size>:[2] - >     5 000"
			<< std::endl << "<container_size>:[3] - >    10 000"
			<< std::endl << "<container_size>:[4] - >    20 000"
			<< std::endl << "<container_size>:[5] - >    50 000"
			<< std::endl << "<container_size>:[6] - >   100 000"
			<< std::endl << "<container_size>:[7] - >   200 000"
			<< std::endl << "<container_size>:[8] - >   500 000"
			<< std::endl << "<container_size>:[9] - > 1 000 000"
			<< std::endl; 

		return -1;
	}

	switch (argv[1][0])	
	{	
		case 'l': TestOneContainer< list   >( argv[2][0], argv[3][0], argv[4][0] );	break;
		case 'v': TestOneContainer< vector >( argv[2][0], argv[3][0], argv[4][0] );	break;
		case 'm': TestOneContainer< map    >( argv[2][0], argv[3][0], argv[4][0] );	break;
	}

	return -2;
}
