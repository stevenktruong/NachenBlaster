#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"
#include <iostream>
using namespace std;

////////////////////////////////////////////////////////////////
// Actor Implementation
////////////////////////////////////////////////////////////////

Actor::Actor(const int& imageID, const int& x, const int& y, const double& startDirection, const double& size, const int& depth, StudentWorld* world)
: GraphObject(imageID, x, y, startDirection, size, depth), m_alive(true), m_world(world)
{}

void Actor::move(const char& dir)
{
    switch(dir)
    {
        case 'u':  moveTo(getX(), getY()+1);  break;
        case 'd':  moveTo(getX(), getY()-1);  break;
        case 'l':  moveTo(getX()-1, getY());  break;
        case 'r':  moveTo(getX()+1, getY());  break;
    }
}

void Actor::die()
{
    m_alive = false;
}

////////////////////////////////////////////////////////////////
// Star Implementation
////////////////////////////////////////////////////////////////

Star::Star(StudentWorld* world, const bool& initial)
: Actor(IID_STAR, VIEW_WIDTH-1, randInt(0, VIEW_HEIGHT-1), 0, static_cast<double>(randInt(5, 50)) / 100, 3, world)
{
    if (initial)
        moveTo(randInt(0, VIEW_WIDTH-1), getY());
}

void Star::doSomething()
{
    moveTo(getX()-1, getY());
    if (getX() < 0)
        die();
}

////////////////////////////////////////////////////////////////
// NachenBlaster Implementation
////////////////////////////////////////////////////////////////

NachenBlaster::NachenBlaster(StudentWorld* world)
: Actor(IID_NACHENBLASTER, 0, 128, 0, 1, 0, world), m_health(50), m_energyPoints(30)
{}

void NachenBlaster::doSomething()
{
    if (!isAlive())
        return;
    int key;
    if (getWorld()->getKey(key))
    {
        switch(key)
        {
            case KEY_PRESS_UP:    move('u');  break;
            case KEY_PRESS_DOWN:  move('d');  break;
            case KEY_PRESS_LEFT:  move('l');  break;
            case KEY_PRESS_RIGHT: move('r');  break;
                // TODO: The rest of the doSomething() stuff.
            default: break;
        }
    }
}
