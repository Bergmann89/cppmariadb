#pragma once

#include <string>
#include <limits>
#include <memory>
#include <cppmariadb/config.h>
#include <cppmariadb/impl/mariadb_handle.h>
#include <cppmariadb/forward/column.h>
#include <cppmariadb/forward/field.h>
#include <cppmariadb/forward/result.h>
#include <cppmariadb/forward/row.h>

namespace cppmariadb
{

    struct row
        : public __impl::mariadb_handle<MYSQL_ROW>
    {
    private:
        template<typename T>
        struct iterator_tpl
        {
        public:
            using this_type         = iterator_tpl<T>;
            using iterator_category = std::random_access_iterator_tag;
            using value_type        = T;
            using difference_type   = ssize_t;
            using pointer           = T*;
            using reference         = T&;

        private:
            enum class compare_result : int
            {
                lower       = -1,
                equals      =  0,
                greater     =  1,
                mismatch    =  2
            };

            using value_ptru_type = std::unique_ptr<value_type>;

        private:
            const row*              _owner;
            ssize_t                 _index;
            ssize_t                 _direction;
            mutable value_ptru_type _cache;

            inline compare_result   compare (const this_type& other) const;
            inline reference        field   () const;
            inline void             assign  (const this_type& other);
            inline void             next    (difference_type i = 1);
            inline void             prev    (difference_type i = 1);

        public:
            inline bool             operator == (const this_type& other) const;
            inline bool             operator != (const this_type& other) const;
            inline bool             operator <  (const this_type& other) const;
            inline bool             operator <= (const this_type& other) const;
            inline bool             operator >  (const this_type& other) const;
            inline bool             operator >= (const this_type& other) const;
            inline reference        operator *  ()                       const;
            inline pointer          operator -> ()                       const;
            inline this_type&       operator ++ ();
            inline this_type&       operator -- ();
            inline this_type        operator ++ (int);
            inline this_type        operator -- (int);
            inline this_type&       operator += (difference_type diff);
            inline this_type&       operator -= (difference_type diff);
            inline this_type        operator +  (difference_type diff)   const;
            inline this_type        operator -  (difference_type diff)   const;
            inline difference_type  operator -  (const this_type& other) const;
            inline this_type        operator [] (difference_type diff);

            inline iterator_tpl(const row& p_row, ssize_t index, ssize_t direction);
            inline iterator_tpl(const this_type& other);
        };

    public:
        using iterator_type       = iterator_tpl<field>;
        using const_iterator_type = iterator_tpl<const field>;

        static constexpr size_t npos = std::numeric_limits<size_t>::max();

    private:
        const result&           _result;
        mutable unsigned long*  _lengths;

    public:
        inline const column_vector& columns () const;
        inline unsigned int         size    () const;
        inline iterator_type        begin   () const;
        inline iterator_type        end     () const;
        inline const_iterator_type  cbegin  () const;
        inline const_iterator_type  cend    () const;
        inline iterator_type        rbegin  () const;
        inline iterator_type        rend    () const;
        inline const_iterator_type  crbegin () const;
        inline const_iterator_type  crend   () const;
               size_t               find    (const std::string& name) const;
               field                at      (size_t i) const;
               field                at      (const std::string name) const;
        inline field        operator[]      (size_t i) const;
        inline field        operator[]      (const std::string& name) const;

        inline row(const result& p_result, MYSQL_ROW p_row);
    };

}