#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <list>
#include <iostream>
#include <sstream>
#include <iomanip>
using namespace std;

using it = std::list<Actor*>::iterator;
const string sep = "  ";

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

StudentWorld::StudentWorld(string assetDir)
: GameWorld(assetDir)
{}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

int StudentWorld::init()
{
    m_blaster = new Blaster(this);
    m_actors.push_front(m_blaster);
    for (int i = 0; i < STARTING_STARS; i++)
        m_actors.push_back(new Star(this, true));
    
    m_S1 = 60;
    m_S2 = 20 + getLevel() * 5;
    m_S3 = 5 + getLevel() * 10;
    
    m_destroyedAliens = 0;
    m_aliensOnScreen = 0;
    
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after you hit enter.
    // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
    //decLives();
    
    // Do stuff and clear dead actors other than the player
    for (it i = m_actors.begin(); i != m_actors.end(); i++)
    {
        (*i)->doSomething();
        if (!(*i)->isAlive() && *i != m_blaster)
        {
            if ((*i)->isAlien())
                m_aliensOnScreen--;
            delete *i;
            i = m_actors.erase(i);
            if (i != m_actors.begin() && i != m_actors.end())
                i--;
        }
    }
    
    // Check if the player has died
    if (!m_blaster->isAlive())
        return GWSTATUS_PLAYER_DIED;
    
    // Generate stars
    if (randInt(1, 15) == 1)
    {
        m_actors.push_back(new Star(this));
    }
    
    // Check if enough aliens are dead
    if (remainingAliens() == 0)
        return GWSTATUS_FINISHED_LEVEL;
    
    // Generate aliens
    if (m_aliensOnScreen < remainingAliens() && m_aliensOnScreen < maxAliens())
    {
        int num = randInt(1, m_S1 + m_S2 + m_S3);
        if (num <= m_S1)
            m_actors.push_front(new Smallgon(this));
        else if (num <= m_S2)
            m_actors.push_front(new Smallgon(this)); // TODO: Smoregon
        else
            m_actors.push_front(new Smallgon(this)); // TODO: Last alien
        m_aliensOnScreen++;
    }
    
    // Update display text
    ostringstream oss;
    oss.setf(ios::fixed);
    oss.precision(0);
    oss << "Lives: "     << getLives()                                          << sep;
    oss << "Health: "    << setw(3) << m_blaster->getHealth() / 50 * 100 << "%" << sep;
    oss << "Score: "     << getScore()                                          << sep;
    oss << "Level: "     << getLevel()                                          << sep;
    oss << "Cabbages: "  << setw(3) << m_blaster->getEnergy() / 30 * 100 << "%" << sep;
    oss << "Torpedoes: " << m_blaster->getTorpedoes();
    setGameStatText(oss.str());
    
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    
    for (it i = m_actors.begin(); i != m_actors.end(); i++)
        delete *i;
    m_actors.clear();
}

// Checks for a collision between two actors
bool hasCollided(Actor* a1, Actor* a2)
{
    double distance = sqrt((a1->getX() - a2->getX()) * (a1->getX() - a2->getX()) +
                           (a1->getY() - a2->getY()) * (a1->getY() - a2->getY()));
    if (distance < 0.75 * (a1->getRadius() + a2->getRadius()))
        return true;
    return false;
}

// Checks if a collision occurred for a certain actor and puts it in the map
Actor* StudentWorld::findCollision(Actor* a)
{
    for (it i = m_actors.begin(); i != m_actors.end(); i++)
    {
        // Collidable objects will be at the begin of the list
        if (!(*i)->isCollidable())
            return nullptr;
        if (a != *i && hasCollided(a, *i))
            return *i;
    }
    return nullptr;
}

void StudentWorld::addActor(Actor* actor)
{
    m_actors.push_front(actor);
}



