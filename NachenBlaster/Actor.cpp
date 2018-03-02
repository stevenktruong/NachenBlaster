#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"
#include <iostream>
using namespace std;

////////////////////////////////////////////////////////////////////////////////////////////////
// Actor Implementation
////////////////////////////////////////////////////////////////////////////////////////////////

Actor::Actor(StudentWorld* world, const int& imageID, const double& x, const double& y,
             const double& startDirection, const double& size, const int& depth)
: GraphObject(imageID, x, y, startDirection, size, depth), m_alive(true), m_world(world)
{}

bool Actor::checkPos(const double& x, const double& y) const
{
    if (0 <= x && x <= VIEW_WIDTH-1  &&
        0 <= y && y <= VIEW_HEIGHT-1)
        return true;
    else
        return false;
}

// Checks if an Actor is off the screen. If it is, it dies
bool Actor::checkStatus()
{
    if (!checkPos(getX(), getY()))
        die();
    if (!isAlive())
        return false;
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Star Implementation
////////////////////////////////////////////////////////////////////////////////////////////////

Star::Star(StudentWorld* world, const bool& initial)
: Actor(world, IID_STAR, VIEW_WIDTH-1, randInt(0, VIEW_HEIGHT-1), 0,
        static_cast<double>(randInt(STAR_SIZE_MIN*100, STAR_SIZE_MAX*100)) / 100, STAR_DEPTH)
{
    // If initial is true, place the star randomly on the screen
    // Otherwise, place it on the right side of the screen with a random y position
    if (initial)
        moveTo(randInt(0, VIEW_WIDTH-1), getY());
}

void Star::doSomething()
{
    checkStatus();
    moveTo(getX()-STAR_SPEED, getY());
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Explosion Implementation
////////////////////////////////////////////////////////////////////////////////////////////////

Explosion::Explosion(StudentWorld* world, const double& x, const double& y)
: Actor(world, IID_EXPLOSION, x, y, 0, EXPLOSION_STARTING_SIZE, EXPLOSION_DEPTH), m_ticksAlive(0)
{}

void Explosion::doSomething()
{
    double size = getSize();
    if (m_ticksAlive == EXPLOSION_LIFETIME)
        die();
    else
    {
        setSize(size * EXPLOSION_SIZE_SCALE);
        m_ticksAlive++;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
// DamageableObject Implementation
////////////////////////////////////////////////////////////////////////////////////////////////

DamageableObject::DamageableObject(StudentWorld* world, const int& imageID, const double& x, const double& y,
                                   const double& startDirection, const double& size, const int& depth,
                                   const double& health)
: Actor(world, imageID, x, y, startDirection, size, depth), m_health(health)
{}

////////////////////////////////////////////////////////////////////////////////////////////////
// Blaster Implementation
////////////////////////////////////////////////////////////////////////////////////////////////

Blaster::Blaster(StudentWorld* world)
: DamageableObject(world, IID_NACHENBLASTER, BLASTER_STARTING_X, BLASTER_STARTING_Y, 0,
                   BLASTER_SIZE, BLASTER_DEPTH, BLASTER_MAX_HEALTH), m_energy(BLASTER_MAX_ENERGY),
                   m_torpedoes(BLASTER_STARTING_TORPEDOES)
{}

void Blaster::doSomething()
{
    if (getHealth() <= 0)
        die();
    if (!checkStatus())
        return;
        
    int key;
    if (getWorld()->getKey(key))
    {
        switch(key)
        {
            case KEY_PRESS_UP:    move('u');      break;
            case KEY_PRESS_DOWN:  move('d');      break;
            case KEY_PRESS_LEFT:  move('l');      break;
            case KEY_PRESS_RIGHT: move('r');      break;
            case KEY_PRESS_SPACE: fireCabbage();  break;
            case KEY_PRESS_TAB:   fireTorpedo();  break;
            default: break;
        }
    }
    
    if (m_energy < BLASTER_MAX_ENERGY)
        m_energy++;
}

void Blaster::gotGoodie(const int& goodieType)
{
    switch(goodieType)
    {
        case GOODIE_EXTRA_LIFE: getWorld()->incLives();         break;
        case GOODIE_REPAIR:     restoreHealth(HEALTH_RESTORED); break;
        case GOODIE_TORPEDO:    m_torpedoes += TORPEDOES_ADDED; break;
        default: break;
    }
}

void Blaster::restoreHealth(const double& health)
{
    if (getHealth()+health > BLASTER_MAX_HEALTH)
        DamageableObject::restoreHealth(BLASTER_MAX_HEALTH-getHealth());
    else
        DamageableObject::restoreHealth(health);
}

void Blaster::move(const char& dir)
{
    switch(dir)
    {
        case 'u': if (checkPos(getX(), getY()+BLASTER_SPEED))  moveTo(getX(), getY()+BLASTER_SPEED);  break;
        case 'd': if (checkPos(getX(), getY()-BLASTER_SPEED))  moveTo(getX(), getY()-BLASTER_SPEED);  break;
        case 'l': if (checkPos(getX()-BLASTER_SPEED, getY()))  moveTo(getX()-BLASTER_SPEED, getY());  break;
        case 'r': if (checkPos(getX()+BLASTER_SPEED, getY()))  moveTo(getX()+BLASTER_SPEED, getY());  break;
    }
}

void Blaster::fireCabbage()
{
    if (m_energy >= CABBAGE_COST)
    {
        m_energy -= CABBAGE_COST;
        getWorld()->playSound(SOUND_PLAYER_SHOOT);
        getWorld()->addActor(new Cabbage(getWorld(), getX()+CABBAGE_DELTA_X, getY()));
    }
}

void Blaster::fireTorpedo()
{
    if (m_torpedoes > 0)
    {
        m_torpedoes--;
        getWorld()->playSound(SOUND_TORPEDO);
        getWorld()->addActor(new Torpedo(getWorld(), getX()+CABBAGE_DELTA_X, getY(), SHOT_BY_PLAYER));
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Alien Implementation
////////////////////////////////////////////////////////////////////////////////////////////////

Alien::Alien(StudentWorld* world, const int& imageID, const double& x, const double& y, const double& health,
             const double& damage, const double& speed, const int& dy, const unsigned int& score)
: DamageableObject(world, imageID, x, y, 0, ALIEN_SIZE, ALIEN_DEPTH, health), m_damage(damage), m_speed(speed), m_score(score), m_dy(dy), m_plan(0)
{}

void Alien::doSomething()
{
    if (getHealth() <= 0)
        deathByPlayer();
    if (!checkStatus())
        return;
    
    // Check if the alien collided with the player
    collidedWithPlayer();
    
    // Check if the alien is about to fly above or below the screen
    if (!checkPos(getX(), getY()+(m_dy*m_speed)))
    {
        m_dy *= LEFT;
        m_plan = randInt(1, MAX_PLAN_LENGTH);
    }
    
    if (m_plan == 0)
    {
        m_dy = randInt(DOWN, UP);
        m_plan = randInt(1, MAX_PLAN_LENGTH);
    }
    
    // Fire a turnip or attempt a special action on the player
    double blasterX, blasterY;
    getWorld()->getBlasterPos(blasterX, blasterY);
    if (blasterX < getX() && blasterY-Y_FROM_BLASTER <= getY() && getY() <= blasterY+Y_FROM_BLASTER)
    {
        if (fire()) // If we fire a Turnip, do nothing else
            return;
        specialAction();
    }
    
    
    m_plan--;
    moveTo(getX()-m_speed, getY()+(m_dy*m_speed));
    
    // Check again if the alien collided with the player
    collidedWithPlayer();
}

bool Alien::fire()
{
    // Writing an equivalent probability as the one given in the spec
    int level = getWorld()->getLevel();
    if (randInt(1, 20 + 5 * level) <= level)
    {
        getWorld()->addActor(new Turnip(getWorld(), getX()-TURNIP_DELTA_X, getY()));
        getWorld()->playSound(SOUND_ALIEN_SHOOT);
        return true;
    }
    return false;
}

// What happens when the alien collides with a player
void Alien::collidedWithPlayer()
{
    Actor* collide = getWorld()->findCollision(this);
    if (collide != nullptr && collide->isPlayer())
    {
        static_cast<Blaster*>(collide)->sufferDamage(m_damage);
        deathByPlayer();
    }
}

void Alien::deathByPlayer()
{
    getWorld()->playSound(SOUND_DEATH);
    getWorld()->addActor(new Explosion(getWorld(), getX(), getY()));
    getWorld()->increaseScore(m_score);
    getWorld()->alienDied();
    dropGoodie();
    die();
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Smallgon Implementation
////////////////////////////////////////////////////////////////////////////////////////////////

Smallgon::Smallgon(StudentWorld* world)
: Alien(world, IID_SMALLGON, VIEW_WIDTH-1, randInt(0, VIEW_HEIGHT-1), 5*(1+(world->getLevel()-1)*0.1),
        SMALLGON_DAMAGE, SMALLGON_SPEED, randInt(DOWN, UP), SMALLGON_SCORE)
{}

////////////////////////////////////////////////////////////////////////////////////////////////
// Smoregon Implementation
////////////////////////////////////////////////////////////////////////////////////////////////

Smoregon::Smoregon(StudentWorld* world)
: Alien(world, IID_SMOREGON, VIEW_WIDTH-1, randInt(0, VIEW_HEIGHT-1), 5*(1+(world->getLevel()-1)*0.1),
        SMOREGON_DAMAGE, SMOREGON_SPEED, randInt(DOWN, UP), SMOREGON_SCORE)
{}

void Smoregon::specialAction()
{
    int level = getWorld()->getLevel();
    if (randInt(1, 20 + 5 * level) <= level)
    {
        setDeltaY(0);
        setPlan(VIEW_WIDTH);
        setSpeed(SMOREGON_RAM_SPEED);
    }
}

void Smoregon::dropGoodie()
{
    if (randInt(1, 3) == 1)
    {
        if (randInt(1, 2) == 1)
            getWorld()->addActor(new RepairGoodie(getWorld(), getX(), getY()));
        else
            getWorld()->addActor(new TorpedoGoodie(getWorld(), getX(), getY()));
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Snagglegon Implementation
////////////////////////////////////////////////////////////////////////////////////////////////

Snagglegon::Snagglegon(StudentWorld* world)
: Alien(world, IID_SNAGGLEGON, VIEW_WIDTH-1, randInt(0, VIEW_HEIGHT-1), 10*(1+(world->getLevel()-1)*0.1),
        SNAGGLEGON_DAMAGE, SNAGGLEGON_SPEED, DOWN, SNAGGLEGON_SCORE)
{
    setPlan(randInt(1, MAX_PLAN_LENGTH)); // The Snagglegon moves down and left initially
}

void Snagglegon::dropGoodie()
{
    if (randInt(1, 6) == 1)
        getWorld()->addActor(new ExtraLifeGoodie(getWorld(), getX(), getY()));
}

bool Snagglegon::fire()
{
    // Writing an equivalent probability as the one given in the spec
    int level = getWorld()->getLevel();
    if (randInt(1, 15 + 5 * level) <= level)
    {
        getWorld()->addActor(new Torpedo(getWorld(), getX()-TURNIP_DELTA_X, getY(), SHOT_BY_ALIEN));
        getWorld()->playSound(SOUND_TORPEDO);
        return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Projectile Implementation
////////////////////////////////////////////////////////////////////////////////////////////////

Projectile::Projectile(StudentWorld* world, const int& imageID, const double& x, const double& y,
                       const double& damage, const double& velocity, const double& rotation, const int& shotBy)
: Actor(world, imageID, x, y, shotBy * 180, PROJECTILE_SIZE, PROJECTILE_DEPTH), m_damage(damage),
        m_velocity(velocity), m_rotation(rotation), m_shotBy(shotBy)
{}

void Projectile::collision(const int& shotBy)
{
    DamageableObject* collide = static_cast<DamageableObject*>(getWorld()->findCollision(this));
    bool predicate;
    if (collide != nullptr && !collide->isProjectile())
    {
        if (shotBy == SHOT_BY_ALIEN)
            predicate = !collide->isAlien();
        else
            predicate = collide->isAlien();
    }
    else return;
    
    if (predicate)
    {
        collide->sufferDamage(getDamage());
        getWorld()->playSound(SOUND_BLAST);
        die();
    }
}

void Projectile::move()
{
    moveTo(getX()+m_velocity,getY());
    setDirection(getDirection()+m_rotation);
}
                       
void Projectile::cycle(const double& pixels, const int& shotBy, const bool& rotate)
{
    collision(shotBy);
    move();
    collision(shotBy);
}

void Projectile::doSomething()
{
    if (!checkStatus())
        return;
    cycle(m_velocity, m_shotBy, m_rotation);
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Cabbage Implementation
////////////////////////////////////////////////////////////////////////////////////////////////

Cabbage::Cabbage(StudentWorld* world, const double& x, const double& y)
: Projectile(world, IID_CABBAGE, x, y, CABBAGE_DAMAGE, CABBAGE_SPEED, CABBAGE_ROTATION, SHOT_BY_PLAYER)
{}

////////////////////////////////////////////////////////////////////////////////////////////////
// Turnip Implementation
////////////////////////////////////////////////////////////////////////////////////////////////

Turnip::Turnip(StudentWorld* world, const double& x, const double& y)
: Projectile(world, IID_TURNIP, x, y, TURNIP_DAMAGE, -TURNIP_SPEED, TURNIP_ROTATION, SHOT_BY_ALIEN)
{}


////////////////////////////////////////////////////////////////////////////////////////////////
// Torpedo Implementation
////////////////////////////////////////////////////////////////////////////////////////////////

Torpedo::Torpedo(StudentWorld* world, const double& x, const double& y, const int& shotBy)
: Projectile(world, IID_TORPEDO, x, y, TORPEDO_DAMAGE, TORPEDO_SPEED, TORPEDO_ROTATION, shotBy)
{
    if (shotBy == SHOT_BY_ALIEN)
        setVelocity(-TORPEDO_SPEED);
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Goodie Implementation
////////////////////////////////////////////////////////////////////////////////////////////////

Goodie::Goodie(StudentWorld* world, const int& imageID, const double& x, const double& y, const int& goodieType)
: Actor(world, imageID, x, y, 0, GOODIE_SIZE, GOODIE_DEPTH), m_goodieType(goodieType)
{}

void Goodie::doSomething()
{
    if (!checkPos(getX(), getY()))
        die();
    if (!isAlive())
        return;
    
    // Check for a collision with the player
    if (collision()) return;
    
    // Move the goodie
    moveTo(getX()+GOODIE_VELOCITY_X, getY()+GOODIE_VELOCITY_Y);
    
    // Check again for a collision
    if (collision()) return;
}

bool Goodie::collision()
{
    Actor* collide = getWorld()->findCollision(this);
    if (collide != nullptr && collide->isPlayer())
    {
        static_cast<Blaster*>(collide)->gotGoodie(m_goodieType);
        getWorld()->playSound(SOUND_GOODIE);
        getWorld()->increaseScore(GOODIE_SCORE);
        die();
        return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// ExtraLifeGoodie Implementation
////////////////////////////////////////////////////////////////////////////////////////////////

ExtraLifeGoodie::ExtraLifeGoodie(StudentWorld* world, const double& x, const double& y)
: Goodie(world, IID_LIFE_GOODIE, x, y, GOODIE_EXTRA_LIFE)
{}

////////////////////////////////////////////////////////////////////////////////////////////////
// RepairGoodie Implementation
////////////////////////////////////////////////////////////////////////////////////////////////

RepairGoodie::RepairGoodie(StudentWorld* world, const double& x, const double& y)
: Goodie(world, IID_REPAIR_GOODIE, x, y, GOODIE_REPAIR)
{}

////////////////////////////////////////////////////////////////////////////////////////////////
// TorpedoGoodie Implementation
////////////////////////////////////////////////////////////////////////////////////////////////

TorpedoGoodie::TorpedoGoodie(StudentWorld* world, const double& x, const double& y)
: Goodie(world, IID_TORPEDO_GOODIE, x, y, GOODIE_TORPEDO)
{}





