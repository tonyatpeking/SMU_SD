#pragma once

#include <algorithm>
#include <type_traits>
#include <vector>
#include <map>
#include "Engine/Core/EngineCommon.hpp"


namespace ContainerUtils
{

namespace Internal
{
// These two functions provide a unified way to access values of std::vector and map
// The vector and map parameters are used to help the compiler pick the right template
template <typename ValueT>
ValueT ValueFromIter(
    const std::vector<ValueT>& vector, typename std::vector<ValueT>::iterator iter )
{
    UNUSED( vector );
    return *iter;
}

template <typename KeyT, typename ValueT>
ValueT ValueFromIter(
    const std::map<KeyT, ValueT>& map, typename std::map<KeyT, ValueT>::iterator iter )
{
    UNUSED( map );
    return ( *iter ).second;
}
}



// will delete the pointers in the std container and clear the container
template <typename ContainerT>
void DeletePointers( ContainerT& pointers )
{
    if( pointers.size() == 0 )
        return;

    auto begin = pointers.begin();
    auto end = pointers.end();
    for( auto iter = begin; iter != end; ++iter )
    {
        auto ptr = Internal::ValueFromIter( pointers, iter );
        if( ptr )
            delete ptr;
    }

    pointers.clear();
}

// Find and erase all elements equal to value in vector
template <typename ValueT>
void EraseValues( std::vector<ValueT>& container, ValueT value )
{
    container.erase(
        std::remove( container.begin(), container.end(), value ), container.end() );
}

// Find and erase all elements equal to value in map
template <typename KeyT, typename ValueT>
void EraseValues( std::map<KeyT, ValueT>& container, ValueT value )
{
    auto begin = container.begin();
    auto end = container.end();
    for( std::map<KeyT, ValueT>::iterator iter = container.begin();
         iter != container.end(); )
    {
        // erase will invalidate iter so this is needed
        std::map<KeyT, ValueT>::iterator nextIter = iter;
        ++nextIter;
        if( ( *iter ).second == value )
            container.erase( iter );
        iter = nextIter;
    }
}

// Find and erase first element equal to value in vector
template <typename ValueT>
void EraseOneValue( std::vector<ValueT>& container, ValueT value )
{
    auto iter = std::find( container.begin(), container.end(), value );
    if( iter != container.end() )
    {
        std::swap( *iter, container.back() );
        container.pop_back();
    }
}

// Find and erase first element equal to value in map
template <typename KeyT, typename ValueT>
void EraseOneValue( std::map<KeyT, ValueT>& container, ValueT value )
{
    auto begin = container.begin();
    auto end = container.end();
    for( std::map<KeyT, ValueT>::iterator iter = container.begin();
         iter != container.end(); )
    {
        // erase will invalidate iter so this is needed
        std::map<KeyT, ValueT>::iterator nextIter = iter;
        ++nextIter;
        if( ( *iter ).second == value )
        {
            container.erase( iter );
            return;
        }
        iter = nextIter;
    }
}

// Swaps with the last element, order is rearranged
template <typename ValueT>
void EraseAtIndexFast( std::vector<ValueT>& container, uint index )
{
    container[index] = container.back();
    container.pop_back();
}

// Slow but order is preserved
template <typename ValueT>
void EraseAtIndex( std::vector<ValueT>& container, uint index )
{
    container.erase( container.begin() + index );
}

template <typename ValueT>
bool Contains( const std::vector<ValueT>& container, const ValueT& val )
{
    if( std::find( container.begin(), container.end(), val ) != container.end() )
        return true;
    return false;
}

template <typename KeyT, typename ValueT >
bool Contains( const std::map<KeyT, ValueT>& container, const KeyT& key )
{
    if( container.find( key ) != container.end() )
        return true;
    return false;
}

template<typename T, size_t N>
std::vector<T> ArrayToVector( const T( &srcArray )[N] )
{
    return std::vector<T>( srcArray, srcArray + N );
}

template<typename ValueT>
ValueT Sum( const std::vector<ValueT>& container )
{
    ValueT sum{};
    for ( auto& var : container )
    {
        sum = sum + var;
    }
    return sum;
}

}
