#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>

class Star;
class NachenBlaster;

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetDir);
    ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();

private:
    Star* m_stars[50];
    int m_totalStars;
    NachenBlaster* m_blaster;
};

#endif // STUDENTWORLD_H_
