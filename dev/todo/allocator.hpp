// File:        allocator.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang@tencent.com>
// Revision:    2012-05-10 by leoxiang


template<typename T>
class SharedMemAllocator 
{
public:
    // typedefs
    typedef T           value_type;
    typedef size_t      size_type;
    typedef ptrdiff_t   difference_type;
    typedef T *         pointer;
    typedef T const *   const_pointer;
    typedef T &         reference;
    typedef T const &   const_reference;

public:
    SharedMemAllocator(SharedMem shm) throw () : _shm(shm)
    {

    }

    ~SharedMemAllocator() throw ();

    pointer       address(reference ref)       const { return &ref; }
    const_pointer address(const_reference ref) const { return &ref; }

    size_type max_size() const throw ();

    pointer allocate(size_type n);
    void    deallocate(pointer p, size_type n);
    void    construct(pointer p, T const & val) { new(p) T(val); }
    void    destroy(pointer p)                  { p->~T(); }

    operator bool() const { return _shm.IsAttached(); }

private:
    SharedMemAllocator(SharedMemAllocator<T> const &) { }
    SharedMemAllocator<T> & operator=(SharedMemAllocator<T> const & rhs) { }

private:
    SharedMem _shm;
};

// vim:ts=4:sw=4:et:ft=cpp:
