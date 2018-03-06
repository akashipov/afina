//
//  container.cpp
//  
//
//  Created by Артем Кашипов on 28.02.2018.
//

#include "List.hpp"



List::~List(){
    Remove();
}

List::point* List::Head() const{
    return Head;
}
    

std::string& List::GetValue(point* a){
        if (a==head){
            return head->value;
        }
        if (a==tail){
            tail = tail->next;
            head->next = tail->prev;
            tail->prev->prev = head;
            head = tail->prev;
            tail->prev = nullptr;
            head->next = nullptr;
            return head->value;
        }
        head -> next = a;
        a->next->prev = a->prev;
        a->prev->next = a->next;
        a->next = nullptr;
        a->prev = head;
        head = a;
        return head->value;
}

const std::string& List::GetTailKey() const{
        return tail->key;
}
const std::string& List::GetHeadKey() const{
        return head->key;
}
void List::Remove(){
        point* s = head;
        while(s!=tail){
            s = s->prev;
            delete s->next;
        }
        delete s;
}
void List::Del(point* a){
        if(head==tail){
            delete head;
            head=nullptr;
            tail=nullptr;
            return;
        }
        if(a==head){
            head->prev->next = nullptr;
            head = head->prev;
            delete a;
            return;
        }
        if(a==tail){
            tail->next->prev = nullptr;
            tail = tail->next;
            delete tail;
            return;
        }
        a->next->prev = a->prev;
        a->prev->next = a->next;
        delete a;
        return;
}
void List::PushFront(const std::string &key,const std::string& value){
        if (head==nullptr){
            point* a = new point(key,value,nullptr,nullptr);
            head = a;
            tail = a;
            return;
        }
        point* a = new point(key,value,nullptr,head);
        head->next = a;
        head = a;
        return;
    }
}
