#ifndef AFINA_STORAGE_LIST_H
#define AFINA_STORAGE_LIST_H

#include <string>

class List{
public:
    struct Point;
    
    List() : _tail(nullptr), _head(nullptr) {};
    ~List();
    
    void PushFront(const std::string &key, const std::string &value);
    void Del(Entry* node);
    void Remove();
    
    std::string& GetValue(point* node);
    const std::string& GetTailKey() const;
    const std::string& GetHeadKey() const;
    point* Head() const;
    
    struct point{
    public:
        point():prev(nullptr),next(nullptr),key(nullptr),value(nullptr){};
        point(const std::string &key, const std::string &value, point* next, point* prev) :
        _key(key), _value(value), _next(next), _prev(prev) {};
        ~point(){
            prev = nullptr;
            next = nullptr;
        };
        size_t size(){
            return key.size()+value.size();
        };
        const std::string key;
        std::string value;
        point* prev;
        point* next;
    };
    
private:
    point* _tail;
    point* _head;
};

#endif
