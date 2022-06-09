#ifndef SINGLETON_H
#define SINGLETON_H

template<typename T>
class Singleton
{
protected:
    Singleton(Singleton&&) = delete;
    Singleton(const Singleton&) = delete;
    Singleton& operator=(Singleton&&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    Singleton(){}
    ~Singleton(){}
public:
    static T& get(){
        static T instance{};
        return instance;
    }
};

#endif // SINGLETON_H
