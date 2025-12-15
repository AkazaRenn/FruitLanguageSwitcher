#pragma once

template<typename T>
class Singleton {
protected:
    Singleton() = default;
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

    ~Singleton() = default;

public:
    static T& Instance() {
        static T instance;
        return instance;
    }
};
