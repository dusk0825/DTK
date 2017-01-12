
#ifndef __DTK_SMARTPTR_H__  
#define __DTK_SMARTPTR_H__  

#include "DTK_Types.h"
#include "DTK_Config.h"
#include "DTK_Mutex.h"
#include "DTK_Atomic.h"


//
// Base classes for reference counted types. The smartptr template
// can be used for smart pointers to types derived from these bases.
//
// shared_object
// ===============
//
// A thread-safe base class for reference-counted types.
//

class shared_object
{
public:
    shared_object() : /*value_(0),*/ no_delete_(false)
    {
#ifdef OS_WINDOWS
        value_ = 0;
#elif defined OS_POSIX
        value_.counter = 0;
#endif
    }

    virtual ~shared_object()
    {
    }

    void __incref()
    {
        DTK_AtomicInc(&value_);
    }

    void __decref(bool no_delete = false)
    {
        DTK_AtomicDec(&value_);

        if (/*value_ == 0*/ DTK_AtomicRead(&value_) == 0 && !no_delete && !no_delete_)
        {
            no_delete_ = true;
            delete this;
        }
    }

    int __getref()
    {
        return DTK_AtomicRead(&value_);
    }

private:
    shared_object(const shared_object&);
    const shared_object& operator=(const shared_object&);

    DTK_ATOMIC_T value_;//引用计数
    bool no_delete_;
};


// class_object
// ===============
//
// A thread-safe base class for class object.
//
class class_object : public shared_object
{
public:
    class_object() : name_("") {};
    class_object(const char* name) : name_(name) {};
    virtual ~class_object() { name_ = NULL; };

    const char* __getname() const { return name_; };

private:
    const char* name_;
};


//
// "smart pointer" class for classes derived from shared_object.
//
template<typename T_Class, typename endclass = shared_object, typename T_Lock = DTK_Mutex>
class smartptr
{
public:
    explicit smartptr(T_Class* handler = 0)
    {
        if (handler)
        {
            handler->endclass::__incref();
        }
        this->handler_ = handler;
    }

    smartptr(const smartptr& right)
    {
        this->handler_ = (const_cast<smartptr&>(right)).__lock_incref();
    }

    template<typename Y>
    smartptr(const smartptr<Y>& right)
    {
        this->handler_ = (const_cast<smartptr<Y>&>(right)).__lock_incref();
    }

    smartptr& operator=(T_Class* right_ptr)
    {   
        // assign compatible right (assume pointer)
        if (this->handler_ != right_ptr)
        {
            if (right_ptr)
            {
                right_ptr->endclass::__incref();
            }

            this->lock_.Lock();
            T_Class* ptr = this->handler_;
            this->handler_ = right_ptr;

            if (ptr)
            {
                ptr->endclass::__decref();
            }
            this->lock_.Unlock();
        }
        return (*this);
    }

    smartptr& operator=(const smartptr& right)
    {   // assign compatible right (assume pointer)
        if (this->handler_ != right.handler_)
        {
            T_Class* right_ptr = (const_cast<smartptr&>(right)).__lock_incref();

            this->lock_.Lock();
            T_Class* ptr = this->handler_;
            this->handler_ = right_ptr;

            if (ptr)
            {
                ptr->endclass::__decref();
            }
            this->lock_.Unlock();
        }
        return (*this);
    }

    template<typename Y>
    smartptr& operator=(const smartptr<Y>& right)
    {
        if(this->handler_ != right.handler_)
        {
            T_Class* right_ptr = (const_cast<smartptr<Y>&>(right)).__lock_incref();

            this->lock_.Lock();
            T_Class* ptr = this->handler_;
            this->handler_ = right_ptr;

            if(ptr)
            {
                ptr->endclass::__decref();
            }
            this->lock_.Unlock();
        }
        return *this;
    }

    ~smartptr()
    {
        if (this->handler_)
            this->handler_->endclass::__decref();
    }

    void __assign(T_Class* right)
    {
        this->handler_ = right;
    }

    bool __empty() const
    {
        return (this->handler_ == 0);
    }

    bool operator==(const smartptr& right) const
    {
        return (this->handler_ == right.handler_);
    }
    bool operator!=(const smartptr& right) const
    {
        return (this->handler_ != right.handler_);
    }
    bool operator==(T_Class* right) const
    {
        return (this->handler_ == right);
    }
    bool operator!=(T_Class* right) const
    {
        return (this->handler_ != right);
    }
    T_Class& operator*() const
    {
        return (*(this->handler_));
    }
    T_Class *operator->() const
    {
        return this->handler_;
    }
    T_Class* get() const
    {
        return this->handler_;
    }
    operator T_Class*() const
    {
        return this->handler_;
    }

    template<typename Y>
    static smartptr __dynamic_cast(const smartptr<Y>& right)
    {
        return smartptr(dynamic_cast<T_Class* const>(right.handler_));
    }

    template<typename Y>
    static smartptr __dynamic_cast(Y* right_ptr)
    {
        return smartptr(dynamic_cast<T_Class*>(right_ptr));
    }

    // dynamic cast need handler_ and __incref is public
    T_Class* __lock_incref() 
    {
        this->lock_.Lock();
        if (this->handler_)
            this->handler_->endclass::__incref();
        this->lock_.Unlock();
        return this->handler_;
    }

    T_Class* handler_;

private:
    T_Lock lock_;

};

template<class T> class scopedptr // noncopyable
{
public:
    typedef T element_type;
    typedef scopedptr<T> this_type;

    explicit scopedptr(T * p = 0) : ptr(p) // never throws
    {
    }

    ~scopedptr() // never throws
    {
        checked_delete(ptr);
    }

    void reset(T * p = 0) // never throws
    {
        this_type(p).swap(*this);
    }

    T & operator*() const // never throws
    {
        return *ptr;
    }

    T * operator->() const // never throws
    {
        return ptr;
    }

    T * get() const // never throws
    {
        return ptr;
    }

    // implicit conversion to "bool"

    typedef T * this_type::*unspecified_bool_type;

    operator unspecified_bool_type() const // never throws
    {
        return ptr == 0 ? 0 : &this_type::ptr;
    }

    bool operator!() const // never throws
    {
        return ptr == 0;
    }

    void swap(scopedptr & b) // never throws
    {
        T* tmp = b.ptr;
        b.ptr = ptr;
        ptr = tmp;
    }

private:
    scopedptr(scopedptr const&);
    const scopedptr& operator=(scopedptr const&);

    void operator==(scopedptr const&) const;
    void operator!=(scopedptr const&) const;

    inline void checked_delete(T * x)
    {
        // intentionally complex - simplification causes regressions
        typedef char type_must_be_complete[sizeof(T) ? 1 : -1];
        (void) sizeof(type_must_be_complete);
        delete x;
    }

    T* ptr;
};

#endif // __DTK_SMARTPTR_H__ 