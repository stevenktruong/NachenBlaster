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
    
        // Accessors
    void getBlasterPos(double& x, double& y) { x = m_blaster->getX(); y = m_blaster->getY(); };
    double remainingAliens() const { return 6 + 4 * getLevel() - m_destroyedAliens; };
    double maxAliens()       const { return 4 + 0.5 * getLevel(); };
    
        // Mutators
    void   alienDied() { m_destroyedAliens++; }
    
        // Actor management
    Actor* findCollision(Actor* a);
    void addActor(Actor* actor);

private:
        // The Blaster is always the first Actor in the list so it takes priority with collisions
        // Uncollidable objects are at the back, and collideable objects are at the front
    Blaster* m_blaster;
    std::list<Actor*> m_actors;
    
    int m_S1, m_S2, m_S3;     // These are their own data members so we don't have to calculate them every tick
    double m_destroyedAliens;
    double m_aliensOnScreen;
};

#endif // STUDENTWORLD_H_
