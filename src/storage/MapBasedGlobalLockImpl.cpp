#include "MapBasedGlobalLockImpl.h"

#include <iostream>

namespace Afina {
    namespace Backend {
        
        // See MapBasedGlobalLockImpl.h
        bool MapBasedGlobalLockImpl::Put(const std::string &key, const std::string &value) {
            std::lock_guard<std::mutex> lock(_mutex);
            if (key.size() + value.size() > _max_size)
                return false;
            
            std::reference_wrapper<const std::string> key_ref(key);
            auto obj = _hash_table.find(key_ref);
            if (obj == _hash_table.end()){
                while (key.size() + value.size() + _size > _max_size)
                    DeleteUnlock(_list.GetTailKey());
                
                _list.PushFront(key, value);
                _hash_table.insert(std::make_pair (std::reference_wrapper<const std::string> (_list.GetHeadKey()), _list.Head()));
                _size += key.size() + value.size();
                return true;
            }
            else{
                _list.GetValue(obj->second);
                while (key.size() + value.size() + _size - obj->second->size() > _max_size)
                    DeleteUnlock(_list.GetTailKey());
                
                _size -= obj->second->size();
                _list.GetValue(obj->second) = value;
                _size += obj->second->size();
                return true;
            }
            
            return false;
        }
        
        // See MapBasedGlobalLockImpl.h
        bool MapBasedGlobalLockImpl::PutIfAbsent(const std::string &key, const std::string &value) {
            std::lock_guard<std::mutex> lock(_mutex);
            if (key.size() + value.size() > _max_size)
                return false;
            
            std::reference_wrapper<const std::string> key_ref(key);
            if (_hash_table.find(key_ref) != _hash_table.end()){
                return false;
            }
            
            while (key.size() + value.size() + _size > _max_size)
                DeleteUnlock(_list.GetTailKey());
            
            _list.PushFront(key, value);
            _hash_table.insert(std::make_pair (std::reference_wrapper<const std::string> (_list.GetHeadKey()), _list.Head()));
            _size += key.size() + value.size();
            
            return true;
        }
        
        // See MapBasedGlobalLockImpl.h
        bool MapBasedGlobalLockImpl::Set(const std::string &key, const std::string &value) {
            std::lock_guard<std::mutex> lock(_mutex);
            if (key.size() + value.size() > _max_size)
                return false;
            
            std::reference_wrapper<const std::string> key_ref(key);
            auto obj = _hash_table.find(key_ref);
            if (obj == _hash_table.end())
                return false;
            
            _list.GetValue(obj->second);
            while (key.size() + value.size() + _size - obj->second->size() > _max_size)
                DeleteUnlock(_list.GetTailKey());
            //rehash?
            _size -= obj->second->size();
            _list.GetValue(obj->second) = value;
            _size += obj->second->size();
            return true;
        }
        
        // See MapBasedGlobalLockImpl.h
        bool MapBasedGlobalLockImpl::Delete(const std::string &key) {
            std::lock_guard<std::mutex> lock(_mutex);
            std::reference_wrapper<const std::string> key_ref(key);
            auto obj = _hash_table.find(key_ref);
            if (obj == _hash_table.end())
                return false;
            
            auto node = obj->second;
            _size -= node->size();
            _hash_table.erase(obj);
            _list.Del(node);
            
            return true;
        }
        
        bool MapBasedGlobalLockImpl::DeleteUnlock(const std::string &key) {
            std::reference_wrapper<const std::string> key_ref(key);
            auto obj = _hash_table.find(key);
            if (obj == _hash_table.end())
                return false;
            
            auto node = obj->second;
            _size -= node->size();
            _hash_table.erase(obj);
            _list.Del(node);
            
            return true;
        }
        
        // See MapBasedGlobalLockImpl.h
        bool MapBasedGlobalLockImpl::Get(const std::string &key, std::string &value) const {
            std::lock_guard<std::mutex> lock(_mutex);
            std::reference_wrapper<const std::string> key_ref(key);
            auto obj = _hash_table.find(key_ref);
            if (obj == _hash_table.end())
                return false;
            
            value = _list.GetValue(obj->second);
            return true;
        }
        
    } // namespace Backend
} // namespace Afina



/*class Cash{
private:
    std::unordered_map<const std::reference_wrapper<std::string>,
    point*,
    std::hash<std::string>,
    std::equal_to<std::string>,
    std::allocator< std::pair<const std::string,point*> >> hsh;
    size_t sum_size;
    size_t max_size;
    point a;
public:
    d_cont():sum(0),s_size(100),head(nullptr),tail(nullptr){
        
    }
    d_cont(size_t s):sum(0),s_size(s),a(),head(nullptr),tail(nullptr){
        
    }
    d_cont(point x, size_t s):s_size(s){
        sum = a.get_value().size()+a.get_key().size();
        if (sum>s){
            std::cout << "Cant make with this s_size" << std::endl;
        }
        else{
            a = std::move(x);
            head = &a;
            tail = &a;
            std::reference_wrapper<std::string> b;
            b(const a.get_key());
            hsh.append(b, &a);
            std::cout << "Append" << std::endl;
        }
    }
    ~d_cont(){
        point* s = head;
        while(s!=tail){
            s = s->prev;
            delete s->next;
        }
        delete s;
    }
    bool put(const std::string &key, const std::string &value){
        if (key.size()+value.size()>this->max_size){
            std::cout << "Cant add with this size of key and value" << std::endl;
            return false;
        }
        sum+= a.get_value().size()+a.get_key().size();
        std::reference_wrapper<const std::string> key_ref(key);
        if (sum<this->s_size){
            this->head->next = &a;
            head = &a;
            std::reference_wrapper<std::string> b;
            b(a.get_key());
            this->hsh.append(const b, &a);
        }
        else{
            std::cout << "Delete tail element to free memory" << std::endl;
            this->tail = this->tail->next;
            sum-=(this->tail->next->get_value().size()+this->tail->next->get_key().size());
            std::reference_wrapper<std::string> b;
            b(this->tail->prev->get_key());
            hsh.erase(const b);
            delete this->tail->prev;
            if (sum<this->s_size){
                this->head->next = &a;
                this->head = &a;
                std::reference_wrapper<std::string> b;
                b(a.get_key());
                this->hsh.append(const b, &a);
            }
            else{
                this->put(a);
            }
        }
    }
    void putifAbsent(point a){
        if (this->hsh.find(&a)!=this->hsh.end()){
            this->put(a);
        }
        else{
            std::cout << "Element in container yet" << std::endl;
        }
    }
    std::string get(std::string *key){
        if (this->hsh.find(&a)!=this->hsh.end()){
            return (this->hsh.find(key)->second).get_value();
        }
        else{
            std::cout << "No exist element with this key" << std::endl;
        }
    }
    void set(std::string *a){
        if (this->hsh.find(a)!=this->hsh.end()){
            this->hsh.find(a)->second->rewrite_key();
        }
        else{
            std::cout << "No exist element with this key" << std::endl;
        }
    }
};*/
