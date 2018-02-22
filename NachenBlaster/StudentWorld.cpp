#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

StudentWorld::StudentWorld(string assetDir)
: GameWorld(assetDir), m_blaster(nullptr), m_totalStars(30)
{}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

int StudentWorld::init()
{
    m_blaster = new NachenBlaster(this);
    for (int i = 0; i < m_totalStars; i++)
        m_stars[i] = new Star(this, true);
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after you hit enter.
    // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
    decLives();
    m_blaster->doSomething();
    for (int i = 0; i < m_totalStars; i++)
    {
        m_stars[i]->doSomething();
        if (!m_stars[i]->isAlive())
        {
            delete m_stars[i];
            m_totalStars--;
            for (int j = i; j < m_totalStars; j++)
                m_stars[j] = m_stars[j+1];
        }
    }
    
    if (randInt(1, 15) == 1)
    {
        m_stars[m_totalStars] = new Star;
        m_totalStars++;
    }
    
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    delete m_blaster;
    for (int i = 0; i < m_totalStars; i++)
        delete m_stars[i];
}
