// File:        shared_ptr.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang@tencent.com>
// Revision:    2011-12-09 by leoxiang

#pragma once

namespace lsf {
namespace util {

////////////////////////////////////////////////////////////
// SharedPtr
////////////////////////////////////////////////////////////
template<typename ElemType>
class SharedPtr
{
public:
    typedef ElemType       value_type;
    typedef ElemType *     pointer;
    typedef ElemType &     reference;

public:
    // constructor and copy constructor
    SharedPtr(ElemType * ptr = NULL) 
        : _ptr_data(ptr), _ptr_cnt(new size_t(1)) { }

    SharedPtr(SharedPtr<ElemType> const & rhs)
        : _ptr_data(rhs._ptr_data), _ptr_cnt(rhs._ptr_cnt) 
    {
        ++*_ptr_cnt;
    }
    template<typename OtherType>
    SharedPtr(OtherType * ptr) 
        : _ptr_data(ptr), _ptr_cnt(new size_t(1)) { }

    template<typename OtherType>
    SharedPtr(SharedPtr<OtherType> const & rhs)
        : _ptr_data(rhs._ptr_data), _ptr_cnt(rhs._ptr_cnt) 
    {
        ++*_ptr_cnt;
    }

    // copy assignment
    SharedPtr & operator=(ElemType * ptr) {
        if (this->_ptr_data == ptr) return *this;

        _Release();
        _ptr_cnt  = new size_t(1);
        _ptr_data = ptr;
        return *this;
    }

    SharedPtr& operator= (SharedPtr<ElemType> const & rhs) {
        _Release();
        _ptr_cnt  = rhs._ptr_cnt;
        _ptr_data = rhs._ptr_data;
        ++*_ptr_cnt;
        return *this;
    }
    
    template<typename OtherType>
    SharedPtr & operator=(OtherType * ptr) {
        if (this->_ptr_data == ptr) return *this;

        _Release();
        _ptr_cnt  = new size_t(1);
        _ptr_data = ptr;
        return *this;
    }

    template<typename OtherType>
    SharedPtr& operator= (SharedPtr<OtherType> const & rhs) {
        _Release();
        _ptr_cnt  = rhs._ptr_cnt;
        _ptr_data = rhs._ptr_data;
        ++*_ptr_cnt;
        return *this;
    }

    // destructor
    ~SharedPtr() { _Release(); }

    // accessor
    reference operator*()  const { return *_ptr_data; }
    pointer   operator->() const { return _ptr_data; }

    pointer   GetPtr()    const { return _ptr_data; }
    size_t GetUseCount() const { return *_ptr_cnt; }

    bool IsUnique() const { return *_ptr_cnt == 1; }

    // make inherated class assign and construct
    template<typename OtherType> friend class SharedPtr;

private:
    void _Release() {
        if (--*_ptr_cnt == 0) { 
            delete _ptr_data; 
            delete _ptr_cnt; 
        }
    }

private:
    ElemType *  _ptr_data; // contained pointer
    size_t *    _ptr_cnt;  // reference counter
};

////////////////////////////////////////////////////////////
// Helper funcs
////////////////////////////////////////////////////////////
template<typename TypeA,typename TypeB>
inline static bool operator==(SharedPtr<TypeA> const & lhs, SharedPtr<TypeB> const & rhs) {
    return lhs.GetPtr() == rhs.GetPtr();
}

template<typename TypeA,typename TypeB>
inline static bool operator!=(SharedPtr<TypeA> const & lhs, SharedPtr<TypeB> const & rhs) {
    return lhs.GetPtr() != rhs.GetPtr();
}

} // end of namespace util
} // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
