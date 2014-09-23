// File:        shared_mem.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang@tencent.com>
// Revision:    2012-05-07 by leoxiang

#pragma once

#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdint.h>
#include <cstring>
#include "lsf/basic/error.hpp"

namespace lsf {
namespace util {

class SharedMem : public basic::Error
{
public:
    //////////////////////////////////////////////////////////// 
    // static funcs, for manipulation
    static bool Create(key_t key, size_t size, int modflag = 0666) {
        if (::shmget(key, size, modflag | IPC_CREAT) < 0) return false;

        return true;
    }

    static bool Delete(key_t key) {
        int   shmid;

        if ((shmid = ::shmget(key, 0, 0)) < 0) return false;
        if (::shmctl(shmid, IPC_RMID, NULL) < 0) return false;

        return true;
    }
    
    static bool Lock(key_t key) {
        int   shmid;

        if ((shmid = ::shmget(key, 0, 0)) < 0) return false;
        if (::shmctl(shmid, SHM_LOCK, 0) < 0) return false;

        return true;
    }

    static bool UnLock(key_t key) {
        int   shmid;

        if ((shmid = ::shmget(key, 0, 0)) < 0) return false;
        if (::shmctl(shmid, SHM_UNLOCK, 0) < 0) return false;

        return true;
    }

    static size_t QueryAttachedNum(key_t key) {
        int             shmid;
        shmid_ds shmds;

        if ((shmid = ::shmget(key, 0, 0)) < 0) return 0;
        if (::shmctl(shmid, IPC_STAT, &shmds) < 0) return 0;
        
        return shmds.shm_nattch;
    }

    static size_t QueryShmSize(key_t key) {
        int             shmid;
        shmid_ds shmds;

        if ((shmid = ::shmget(key, 0, 0)) < 0) return 0;
        if (::shmctl(shmid, IPC_STAT, &shmds) < 0) return 0;
        
        return shmds.shm_segsz;
    }

    static bool IsShmExist(key_t key) {
        int             shmid;

        if ((shmid = ::shmget(key, 0, 0)) < 0) 
            return false;
        else 
            return true;
    }

public:
    //////////////////////////////////////////////////////////// 
    // constructor
    explicit SharedMem() : _key(0), _ptr_mem(NULL) { }
    
    explicit SharedMem(key_t key) 
        : _key(0), _ptr_mem(NULL) {
        Attach(key);
    }

    SharedMem(SharedMem const & rhs)
        : _key(0), _ptr_mem(NULL) {
        if (rhs.IsAttached()) Attach(rhs._key);
    }

    ~SharedMem() { Detach(); }

    SharedMem & operator=(SharedMem const & rhs) {
        if (this == &rhs) {
            return *this;
        }
        if (rhs.IsAttached()) {
            Attach(rhs._key);
        }
        return *this;
    }

    
    //////////////////////////////////////////////////////////// 
    // member funcs
    bool Attach(key_t key, int shmflg = 0) {
        int   shmid;

        if (IsAttached()) Detach();

        if ((shmid = ErrWrap(::shmget(key, 0, 0))) < 0) return false;
        
        if ((_ptr_mem = ErrWrapPointer(::shmat(shmid, NULL, 0))) == (void *)-1) {
            _ptr_mem = NULL;
            return false;
        }

        _key = key;
        return true;
    }

    bool Detach() {
        if (_ptr_mem == NULL && _key == 0) return true;

        ErrWrap(::shmdt(_ptr_mem));
        _ptr_mem = NULL;
        _key = 0;
        return true;
    }
    
    // other funcs
    void *  GetPtr()         const { return _ptr_mem; }
    size_t  GetSize()        const { return SharedMem::QueryShmSize(_key); }
    size_t  GetAttachedNum() const { return SharedMem::QueryAttachedNum(_key); }
    bool    IsAttached() const { return _ptr_mem != NULL && _key != 0; }

private:
    key_t           _key ;
    void *          _ptr_mem;
};

} // end of namespace util
} // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
