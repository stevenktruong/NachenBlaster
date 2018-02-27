#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
#include <list>

const int STARTING_STARS = 30;

class Actor;
class Blaster;
class Projectile;

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetDir);
    ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    
    double remainingAliens() { return 6 + 4 * getLevel() - m_destroyedAliens; };
    double maxAliens() { return 4 + 0.5 * getLevel(); };
    void alienDied() { m_destroyedAliens++; }
    Actor* findCollision(Actor* a);
    void addActor(Actor* actor);

private:
        // Uncollidable objects are at the back, and collideable objects are at the front
    Blaster* m_blaster;
    std::list<Actor*> m_actors;
    int m_S1, m_S2, m_S3;
    int m_destroyedAliens;
    int m_aliensOnScreen;
};

#endif // STUDENTWORLD_H_
