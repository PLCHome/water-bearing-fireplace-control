#include "myPoint.h"

myPoint::myPoint(JsonVariant json, myPoint* next)
{
    this->next = next;
    if (json["name"].is<String>())
    {
        this->name = json["name"].as<String>();
    }
    if (json["id"].is<int>())
    {
        this->id = json["id"].as<int>();
    }
}


myPoint *myPoint::getPoint(int id)
{
    if (this->id == id)
    {
        return this;
    }
    else if (this->next != NULL)
    {
        return this->next->getPoint(id);
    }
    else
    {
        return NULL;
    }
}

myPoint *myPoint::getPoint(String name)
{
    if (this->name.equals(name))
    {
        return this;
    }
    else if (this->next != NULL)
    {
        return this->next->getPoint(name);
    }
    else
    {
        return NULL;
    }
}
myPoint *myPoint::getNext(){
    return this->next;
}

void myPoint::unsetCalculated()
{
    if (this->next != NULL)
    {
        this->next->unsetCalculated();
    }
    this->calculated = false;
}

ergPoint myPoint::getVal()
{
    if (!this->calculated)
    {
        this->calculated = true;
        this->calcVal();
    }
    return this->on;
}
