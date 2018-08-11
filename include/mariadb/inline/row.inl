#pragma once

#include <mariadb/row.h>
#include <mariadb/field.h>
#include <mariadb/result.h>

namespace mariadb
{
    /* row::iterator_tpl *************************************************************************/

    template<typename T>
    inline typename row::iterator_tpl<T>::compare_result
    row::iterator_tpl<T>::compare(const this_type& other) const
    {
        if (_owner != other._owner)
            return compare_result::mismatch;
        else if (_index < other._index)
            return compare_result::lower;
        else if (_index > other._index)
            return compare_result::greater;
        else
            return compare_result::equals;
    }

    template<typename T>
    inline typename row::iterator_tpl<T>::reference
    row::iterator_tpl<T>::field() const
    {
        if (!_cache)
            _cache.reset(new value_type(_owner->at(_index)));
        return *_cache;
    }

    template<typename T>
    inline void row::iterator_tpl<T>::assign(const this_type& other)
    {
        _owner     = other._owner;
        _index     = other._index;
        _direction = other._direction;
        _cache.reset();
    }

    template<typename T>
    inline void row::iterator_tpl<T>::next(difference_type i)
    {
        _cache.reset();
        _index = _index + _direction * i;
    }

    template<typename T>
    inline void row::iterator_tpl<T>::prev(difference_type i)
    {
        _cache.reset();
        _index = _index - _direction * i;
    }

    template<typename T>
    inline bool row::iterator_tpl<T>::operator==(const this_type& other) const
        { return compare(other) == compare_result::equals; }

    template<typename T>
    inline bool row::iterator_tpl<T>::operator!=(const this_type& other) const
        { return compare(other) != compare_result::equals; }

    template<typename T>
    inline bool row::iterator_tpl<T>::operator<(const this_type& other) const
        { return compare(other) == compare_result::less; }

    template<typename T>
    inline bool row::iterator_tpl<T>::operator<=(const this_type& other) const
    {
        auto c = compare(other);
        return (c == compare_result::less || c == compare_result::equals);
    }

    template<typename T>
    inline bool row::iterator_tpl<T>::operator>(const this_type& other) const
        { return compare(other) == compare_result::greater; }

    template<typename T>
    inline bool row::iterator_tpl<T>::operator>=(const this_type& other) const
    {
        auto c = compare(other);
        return (c == compare_result::greater || c == compare_result::equals);
    }

    template<typename T>
    inline typename row::iterator_tpl<T>::reference
    row::iterator_tpl<T>::operator*() const
        { return field(); }

    template<typename T>
    inline typename row::iterator_tpl<T>::pointer
    row::iterator_tpl<T>::operator->() const
        { return &field(); }

    template<typename T>
    inline typename row::iterator_tpl<T>::this_type&
    row::iterator_tpl<T>::operator++()
    {
        next();
        return *this;
    }

    template<typename T>
    inline typename row::iterator_tpl<T>::this_type&
    row::iterator_tpl<T>::operator--()
    {
        prev();
        return *this;
    }

    template<typename T>
    inline typename row::iterator_tpl<T>::this_type
    row::iterator_tpl<T>::operator++(int)
    {
        auto tmp(*this);
        next();
        return tmp;
    }

    template<typename T>
    inline typename row::iterator_tpl<T>::this_type
    row::iterator_tpl<T>::operator--(int)
    {
        auto tmp(*this);
        prev();
        return tmp;
    }

    template<typename T>
    inline typename row::iterator_tpl<T>::this_type&
    row::iterator_tpl<T>::operator+=(difference_type diff)
    {
        next(diff);
        return *this;
    }

    template<typename T>
    inline typename row::iterator_tpl<T>::this_type&
    row::iterator_tpl<T>::operator-=(difference_type diff)
    {
        prev(diff);
        return *this;
    }

    template<typename T>
    inline typename row::iterator_tpl<T>::this_type
    row::iterator_tpl<T>::operator+(difference_type diff) const
    {
        auto tmp(*this);
        tmp += diff;
        return tmp;
    }

    template<typename T>
    inline typename row::iterator_tpl<T>::this_type
    row::iterator_tpl<T>::operator-(difference_type diff) const
    {
        auto tmp(*this);
        tmp -= diff;
        return tmp;
    }

    template<typename T>
    inline typename row::iterator_tpl<T>::difference_type
    row::iterator_tpl<T>::operator-(const this_type& other) const
        { return (_index - other._index) * _direction; }

    template<typename T>
    inline typename row::iterator_tpl<T>::this_type
    row::iterator_tpl<T>::operator[] (difference_type diff)
    {
        auto tmp(*this);
        tmp += diff;
        return tmp;
    }

    template<typename T>
    inline row::iterator_tpl<T>::iterator_tpl(const row& p_row, ssize_t index, ssize_t direction)
        : _owner    (&p_row)
        , _index    (index)
        , _direction(direction)
        { }

    template<typename T>
    inline row::iterator_tpl<T>::iterator_tpl(const this_type& other)
        { assign(other); }

    /* row ***************************************************************************************/

    inline const column_vector& row::columns() const
        { return _result.columns(); }

    inline unsigned int row::size() const
        { return _result.columncount(); }

    inline row::iterator_type row::begin() const
        { return iterator_type(*this, 0, 1); }

    inline row::iterator_type row::end() const
        { return iterator_type(*this, size(), 1); }

    inline row::const_iterator_type row::cbegin() const
        { return const_iterator_type(*this, 0, 1); }

    inline row::const_iterator_type row::cend() const
        { return const_iterator_type(*this, size(), 1); }

    inline row::iterator_type row::rbegin() const
        { return iterator_type(*this, size()-1, -1); }

    inline row::iterator_type row::rend() const
        { return iterator_type(*this, -1, -1); }

    inline row::const_iterator_type row::crbegin() const
        { return const_iterator_type(*this, size()-1, -1); }

    inline row::const_iterator_type row::crend() const
        { return const_iterator_type(*this, -1, -1); }

    inline field row::operator[](size_t i) const
        { return at(i); }

    inline field row::operator[](const std::string& name) const
        { return at(name); }

    inline row::row(const result& p_result, MYSQL_ROW p_row)
        : mariadb_handle(p_row)
        , _result       (p_result)
        , _lengths      (nullptr)
        { }
        
}