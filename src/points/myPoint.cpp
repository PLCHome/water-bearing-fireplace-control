#include "myPoint.h"

myPoint::myPoint(JsonVariant json, pointTyp type) : type(type)
{
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
    else
    {
        return NULL;
    }
}

void myPoint::unsetCalculated()
{
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

int myPoint::getId() const
{
    return this->id;
}

ergPoint myPoint::getOn() const
{
    return this->on;
}

 pointTyp myPoint::getTyp() const
{
    return this->type;
}
