#ifndef __AUTO_POINTERS__
#define __AUTO_POINTERS__

template<class T>
class AutoPtr
{
public:
    AutoPtr( T* ptr ) : Ptr( ptr ) {}
    ~AutoPtr() { if( Ptr ) delete Ptr; }
    T& operator*() const  { return *Get(); }
    T* operator->() const { return Get(); }
    T* Get() const        { return Ptr; }
    T* Release()
    {
        T* tmp = Ptr;
        Ptr = NULL;
        return tmp;
    }
    void Reset( T* ptr )
    {
        if( ptr != Ptr && Ptr != 0 ) delete Ptr;
        Ptr = ptr;
    }
    bool IsValid() const { return Ptr != NULL; }

private:
    T* Ptr;
};

template<class T>
class AutoPtrArr
{
public:
    AutoPtrArr( T* ptr ) : Ptr( ptr ) {}
    ~AutoPtrArr() { if( Ptr ) delete[] Ptr; }
    T& operator*() const  { return *Get(); }
    T* operator->() const { return Get(); }
    T* Get() const        { return Ptr; }
    T* Release()
    {
        T* tmp = Ptr;
        Ptr = NULL;
        return tmp;
    }
    void Reset( T* ptr )
    {
        if( ptr != Ptr && Ptr != 0 ) delete[] Ptr;
        Ptr = ptr;
    }
    bool IsValid() const { return Ptr != NULL; }

private:
    T* Ptr;
};

#endif // __AUTO_POINTERS__ //
