#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"
#include <iostream>
using namespace std;

const int BLASTER_MOVE = 6;         // Number of pixels that the blaster moves
const double EXPLOSION_SCALE = 1.5;    // How much the explosion size increases by each tick

////////////////////////////////////////////////////////////////
// Actor Implementation
////////////////////////////////////////////////////////////////

Actor::Actor(StudentWorld* world, const int& imageID, const int& x, const int& y,
             const double& startDirection, const double& size, const int& depth)
: GraphObject(imageID, x, y, startDirection, size, depth), m_alive(true), m_world(world)
{}

bool Actor::checkPos(const int& x, const int& y) const
{
    if (0 <= x && x < VIEW_WIDTH-1  &&
        0 <= y && y < VIEW_HEIGHT-1)
        return true;
    else
        return false;
}

////////////////////////////////////////////////////////////////
// Star Implementation
////////////////////////////////////////////////////////////////

Star::Star(StudentWorld* world, const bool& initial)
: Actor(world, IID_STAR, VIEW_WIDTH-1, randInt(0, VIEW_HEIGHT-1), 0, static_cast<double>(randInt(5, 50)) / 100, 3)
{
    // If initial is true, place the star randomly on the screen
    // Otherwise, place it on the right side of the screen with a random y position
    if (initial)
        moveTo(randInt(0, VIEW_WIDTH-1), getY());
}

void Star::doSomething()
{
    if (getX() < 0)
        die();
    moveTo(getX()-1, getY());
}

////////////////////////////////////////////////////////////////
// Explosion Implementation
////////////////////////////////////////////////////////////////

Explosion::Explosion(StudentWorld* world, const double& x, const double& y)
: Actor(world, IID_EXPLOSION, x, y, 0, 1, 0), m_ticksAlive(0)
{}

void Explosion::doSomething()
{
    double size = getSize();
    if (m_ticksAlive == 4)
        die();
    else
    {
        setSize(size * EXPLOSION_SCALE);
        m_ticksAlive++;
    }
}

////////////////////////////////////////////////////////////////
// DamageableObject Implementation
////////////////////////////////////////////////////////////////

DamageableObject::DamageableObject(StudentWorld* world, const int& imageID, const int& x, const int& y,
                                   const double& startDirection, const double& size, const int& depth,
                                   const double& health)
: Actor(world, imageID, x, y, startDirection, size, depth), m_health(health)
{}

DamageableObject* DamageableObject::checkCollision()
{
    Actor* collide = getWorld()->findCollision(this);
    return static_cast<DamageableObject*>(collide);
}

////////////////////////////////////////////////////////////////
// Blaster Implementation
////////////////////////////////////////////////////////////////

Blaster::Blaster(StudentWorld* world)
: DamageableObject(world, IID_NACHENBLASTER, 0, 128, 0, 1, 0, 50), m_energy(30), m_torpedoes(0)
{}

void Blaster::doSomething()
{
    if (getHealth() <= 0)
        die();
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
            case KEY_PRESS_SPACE: fire();     break;
                // TODO: The rest of the doSomething() stuff.
            default: break;
        }
    }
    
    if (m_energy < 30)
        m_energy++;
}

void Blaster::move(const char& dir)
{
    switch(dir)
    {
        case 'u':  if (checkPos(getX(), getY()+BLASTER_MOVE)) moveTo(getX(), getY()+BLASTER_MOVE);  break;
        case 'd':  if (checkPos(getX(), getY()-BLASTER_MOVE)) moveTo(getX(), getY()-BLASTER_MOVE);  break;
        case 'l':  if (checkPos(getX()-BLASTER_MOVE, getY())) moveTo(getX()-BLASTER_MOVE, getY());  break;
        case 'r':  if (checkPos(getX()+BLASTER_MOVE, getY())) moveTo(getX()+BLASTER_MOVE, getY());  break;
    }
}

void Blaster::fire()
{
    if (m_energy >= 5)
    {
        m_energy -= 5;
        getWorld()->playSound(SOUND_PLAYER_SHOOT);
        getWorld()->addActor(new Cabbage(getWorld(), getX()+12, getY()));
    }
}

////////////////////////////////////////////////////////////////
// Alien Implementation
////////////////////////////////////////////////////////////////

Alien::Alien(StudentWorld* world, const int& imageID, const double& x, const double& y, const double& health,
             const double& damage, const double& speed, const int& dy, const unsigned int& score)
: DamageableObject(world, imageID, x, y, 0, 1.5, 1, health), m_damage(damage), m_speed(speed), m_score(score), m_dy(dy), m_plan(0)
{}

void Alien::doSomething()
{
    if (getX() < 0)
        die();
    if (getHealth() <= 0)
        death();
    if (!isAlive())
        return;
    
    // Check if the alien collided with the player
    collidedWithPlayer();
    
    // Check if the alien is offscreen
    if (!checkPos(getX(), getY()))
    {
        if (getY() >= VIEW_HEIGHT-1)
            m_dy = -1;
        else if (getY() <= 0)
            m_dy = 1;
        m_plan = randInt(1, 32);
    }
    
    specialAction();
    
    if (m_plan == 0)
    {
        m_dy = randInt(-1, 1);
        m_plan = randInt(1, 32);
    }
    
    m_plan--;
    moveTo(getX()-m_speed, getY()+(m_dy*m_speed));
    
    // Check again if the alien collided with the player
    collidedWithPlayer();
}

// What happens when the alien collides with a player
void Alien::collidedWithPlayer()
{
    Actor* collide = getWorld()->findCollision(this);
    if (collide != nullptr && collide->isPlayer())
    {
        static_cast<Blaster*>(collide)->sufferDamage(m_damage);
        death();
    }
}

void Alien::death()
{
    getWorld()->playSound(SOUND_DEATH);
    getWorld()->addActor(new Explosion(getWorld(), getX(), getY()));
    getWorld()->increaseScore(m_score);
    getWorld()->alienDied();
    die();
}

void Alien::dropGoodie()
{
    
}

////////////////////////////////////////////////////////////////
// Smallgon Implementation
////////////////////////////////////////////////////////////////

Smallgon::Smallgon(StudentWorld* world)
: Alien(world, IID_SMALLGON, VIEW_WIDTH-1, randInt(0, VIEW_HEIGHT-1), 5*(1-(world->getLevel()-1)*0.1), 5, 2.0, randInt(-1, 1), 250)
{}

void Smallgon::specialAction()
{
}

////////////////////////////////////////////////////////////////
// Projectile Implementation
////////////////////////////////////////////////////////////////

Projectile::Projectile(StudentWorld* world, const int& imageID, const double& x, const double& y, const double& damage, const int& shotByAlien)
: Actor(world, imageID, x, y, shotByAlien * 180, 0.5, 1), m_damage(damage)
{}

void Projectile::collidedWith(DamageableObject* collide)
{
    if (collide != nullptr)
        collide->sufferDamage(getDamage());
}

////////////////////////////////////////////////////////////////
// Cabbage Implementation
////////////////////////////////////////////////////////////////

Cabbage::Cabbage(StudentWorld* world, const double& x, const double& y)
: Projectile(world, IID_CABBAGE, x, y, 2)
{}

void Cabbage::doSomething()
{
    if (getX() > VIEW_HEIGHT-1)
        die();
    if (!isAlive())
        return;
    
    DamageableObject* collide = static_cast<DamageableObject*>(getWorld()->findCollision(this));
    if (collide != nullptr && collide->isAlien())
    {
        collide->sufferDamage(getDamage());
        getWorld()->playSound(SOUND_BLAST);
        die();
    }
    
    moveTo(getX()+8,getY());
    setDirection(getDirection()+20);
    
    // TODO: Collision again
}













