#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;

////////////////////////////////////////////////////////////////
// Actor Declaration
////////////////////////////////////////////////////////////////

class Actor : public GraphObject
{
public:
        // Constructors and destructors
    Actor(const int& imageID, const int& x, const int& y, const double& startDirection = 0, const double& size = 1.0, const int& depth = 0, StudentWorld* world = nullptr);
    ~Actor() {};
    
        // Accessors
    bool isAlive() { return m_alive; };
    StudentWorld* getWorld() const { return m_world; };
    
        // Mutators
    virtual void move(const char& dir); // Pass 'u', 'l', 'd', or 'r' to this
    virtual void die();
    
        // Actions
    virtual void doSomething() = 0;
    
private:
    bool m_alive;
    StudentWorld* m_world;
};

////////////////////////////////////////////////////////////////
// Star Declaration
////////////////////////////////////////////////////////////////

class Star : public Actor
{
public:
        // Constructors and destructors
    Star(StudentWorld* world = nullptr, const bool& initial = false);
    ~Star() {};
    
        // Actions
    virtual void doSomething();
};

////////////////////////////////////////////////////////////////
// NachenBlaster Declaration
////////////////////////////////////////////////////////////////

class NachenBlaster : public Actor
{
public:
        // Constructors and destructors
    NachenBlaster(StudentWorld* world);
    ~NachenBlaster() {};
    
        // Accessors
    int health() const { return m_health; };
    int energy() const { return m_energyPoints; };
    
        // Actions
    virtual void doSomething();
    
private:
    int m_health;
    int m_energyPoints;
};

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

#endif // ACTOR_H_
