#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;

////////////////////////////////////////////////////////////////////////////////////////////////
// Manifest Constants
////////////////////////////////////////////////////////////////////////////////////////////////

const int LEFT  = -1;
const int RIGHT = 1;
const int UP    = 1;
const int DOWN  = -1;

const int SHOT_BY_PLAYER = 0; // Used to identify who shot a Torpedo
const int SHOT_BY_ALIEN  = 1;

const int GOODIE_EXTRA_LIFE = 0; // Used to identify the types of Goodies
const int GOODIE_REPAIR     = 1;
const int GOODIE_TORPEDO    = 2;

////////////////////////////////////////////////////////////////////////////////////////////////
// Actor Declaration
////////////////////////////////////////////////////////////////////////////////////////////////

class Actor : public GraphObject
{
public:
    Actor(StudentWorld* world, const int& imageID, const double& x, const double& y, const double& startDirection = 0,
          const double& size = 1.0, const int& depth = 0);
    
        // Accessors
    bool isAlive() { return m_alive; }
    bool checkPos(const double& x, const double& y) const;
    virtual bool isCollidable() const { return true; }
    virtual bool isAlien()      const { return false; }
    virtual bool isPlayer()     const { return false; }
    virtual bool isProjectile() const { return false; }
    StudentWorld* getWorld()    const { return m_world; }
    
        // Actions
    virtual void doSomething() = 0;
    virtual bool checkStatus();
    void die() { m_alive = false; }
    
private:
    bool m_alive;
    StudentWorld* m_world;
};

////////////////////////////////////////////////////////////////////////////////////////////////
// Star Declaration
////////////////////////////////////////////////////////////////////////////////////////////////

const double STAR_SPEED    = 1;
const double STAR_SIZE_MIN = 0.05; // Precision must be at most 2 decimal digits
const double STAR_SIZE_MAX = 0.50;

const int STAR_DEPTH = 3;

class Star : public Actor
{
public:
    Star(StudentWorld* world, const bool& initial = false);
    
        // Accessors
    virtual bool isCollidable() const { return false; }
    
        // Actions
    virtual void doSomething();
};

////////////////////////////////////////////////////////////////////////////////////////////////
// Explosion Declaration
////////////////////////////////////////////////////////////////////////////////////////////////

const double EXPLOSION_STARTING_SIZE = 1;
const int    EXPLOSION_DEPTH         = 0;
const int    EXPLOSION_LIFETIME      = 4;   // How many ticks after spawning will an Explosion disappears
const double EXPLOSION_SIZE_SCALE    = 1.5;

class Explosion : public Actor
{
public:
    Explosion(StudentWorld* world, const double& x, const double& y);
    
        // Accessors
    virtual bool isCollidable() const { return false; }
    
        // Actions
    virtual void doSomething();
    
private:
    int m_ticksAlive;
};

////////////////////////////////////////////////////////////////////////////////////////////////
// DamageableObject Declaration
////////////////////////////////////////////////////////////////////////////////////////////////

class DamageableObject : public Actor
{
public:
    DamageableObject(StudentWorld* world, const int& imageID, const double& x, const double& y, const double& startDirection = 0,
                     const double& size = 1.0, const int& depth = 0, const double& health = 0);
    
        // Accessors
    double getHealth() const { return m_health; }
    
        // Mutators
    void sufferDamage(const double& damage)          { m_health -= damage; }
    virtual void restoreHealth(const double& health) { m_health += health; }
    
private:
    double m_health;
};

////////////////////////////////////////////////////////////////////////////////////////////////
// Blaster Declaration
////////////////////////////////////////////////////////////////////////////////////////////////

const double BLASTER_STARTING_X = 0;
const double BLASTER_STARTING_Y = 128;
const double BLASTER_SIZE  = 1;
const int    BLASTER_DEPTH = 0;

const double BLASTER_MAX_HEALTH         = 50;
const double BLASTER_MAX_ENERGY         = 30;
const double BLASTER_STARTING_TORPEDOES = 0;

const double BLASTER_SPEED   = 6;  // Number of pixels that the Blaster moves on a keypress
const double HEALTH_RESTORED = 10; // Health restored by a health Goodie
const double TORPEDOES_ADDED = 5;  // Torpedoes added by a torpedo Goodie
const double CABBAGE_DELTA_X = 12; // How many pixels away from the Blaster a Cabbage spawns
const double CABBAGE_COST    = 5;  // How much energy a Cabbage costs to fire

class Blaster : public DamageableObject
{
public:
    Blaster(StudentWorld* world);
    
        // Accessors
    double getEnergy()      const { return m_energy; }
    double getTorpedoes()   const { return m_torpedoes; }
    virtual bool isPlayer() const { return true; }
    
        // Actions
    virtual void doSomething();
    void gotGoodie(const int& goodieType);
    virtual void restoreHealth(const double& health);
    
private:
    void move(const char& dir); // Pass 'u', 'l', 'd', or 'r' to this
    void fireCabbage();
    void fireTorpedo();
    
    double m_energy;
    double m_torpedoes;
};

////////////////////////////////////////////////////////////////////////////////////////////////
// Alien Declaration
////////////////////////////////////////////////////////////////////////////////////////////////

const int    ALIEN_DEPTH = 1;
const double ALIEN_SIZE  = 1.5;

const int    MAX_PLAN_LENGTH = 32;
const double Y_FROM_BLASTER  = 4;  // How many pixels away the y-coordinate of the Blaster is from the Alien required for the Alien to shoot
const double TURNIP_DELTA_X  = 14; // How many pixels to the left of an Alien a Turnip is spawned

class Alien : public DamageableObject
{
public:
    Alien(StudentWorld* world, const int& imageID, const double& x, const double& y, const double& health,
          const double& damage, const double& speed, const int& dy, const unsigned int& score);
    
        // Accessors
    virtual bool isAlien() const { return true; }
    
        // Mutators
    void setSpeed(const double& speed) { m_speed = speed; }
    void setDeltaY(const double& dy)   { m_dy = dy; }
    void setPlan(const double& plan)   { m_plan = plan; }
    
        // Actions
    virtual void doSomething();
    virtual void specialAction() {};
    virtual void dropGoodie() = 0;
    virtual bool fire();
    
private:
    void deathByPlayer();
    void collidedWithPlayer();
    
    double m_damage;
    double m_speed;
    double m_score;
    int m_dy; // We don't need m_dx since aliens always move left
    int m_plan;
};

////////////////////////////////////////////////////////////////////////////////////////////////
// Smallgon Declaration
////////////////////////////////////////////////////////////////////////////////////////////////

const double SMALLGON_DAMAGE = 5;   // Damage to the Blaster when colliding into it
const double SMALLGON_SPEED  = 2.0;
const int    SMALLGON_SCORE  = 250;

class Smallgon : public Alien
{
public:
    Smallgon(StudentWorld* world);
    
        // Actions
    virtual void dropGoodie() {};
};

////////////////////////////////////////////////////////////////////////////////////////////////
// Smoregon Declaration
////////////////////////////////////////////////////////////////////////////////////////////////

const double SMOREGON_DAMAGE    = 5;   // Damage to the Blaster when colliding into it
const double SMOREGON_SPEED     = 2.0;
const double SMOREGON_RAM_SPEED = 5.0;
const int    SMOREGON_SCORE     = 250;

class Smoregon : public Alien
{
public:
    Smoregon(StudentWorld* world);
    
    // Actions
    virtual void dropGoodie();
    virtual void specialAction();
};

////////////////////////////////////////////////////////////////////////////////////////////////
// Snagglegon Declaration
////////////////////////////////////////////////////////////////////////////////////////////////

const double SNAGGLEGON_DAMAGE = 15;   // Damage to the Blaster when colliding into it
const double SNAGGLEGON_SPEED  = 1.75;
const int    SNAGGLEGON_SCORE  = 1000;

class Snagglegon : public Alien
{
public:
    Snagglegon(StudentWorld* world);
    
        // Actions
    virtual void dropGoodie();
    virtual bool fire();
};

////////////////////////////////////////////////////////////////////////////////////////////////
// Projectile Declaration
////////////////////////////////////////////////////////////////////////////////////////////////

const double PROJECTILE_SIZE  = 0.5;
const int    PROJECTILE_DEPTH = 1;

class Projectile : public Actor
{
public:
    Projectile(StudentWorld* world, const int& imageID, const double& x, const double& y,
               const double& damage, const double& velocity, const double& rotation, const int& shotBy);
    
        // Accessors
    double getDamage() const { return m_damage; }
    virtual bool isProjectile() const { return true; }
    int shotBy() const { return m_shotBy; }
    
        // Actions
    virtual void doSomething();
    virtual void cycle(const double& pixels, const int& shotBy, const bool& rotate = true);
    void setVelocity(const double& velocity) { m_velocity = velocity; }
    
private:
    virtual void collision(const int& shotBy);
    virtual void move();
    
    double m_damage;
    double m_velocity;
    double m_rotation;
    int m_shotBy;
};

////////////////////////////////////////////////////////////////////////////////////////////////
// Cabbage Declaration
////////////////////////////////////////////////////////////////////////////////////////////////

const double CABBAGE_DAMAGE   = 2;  // The damage done on impact
const double CABBAGE_SPEED    = 8;  // Magnitude of velocity. I.e., non-negative
const double CABBAGE_ROTATION = 20; // The angle of rotation each tick

class Cabbage : public Projectile
{
public:
    Cabbage(StudentWorld* world, const double& x, const double& y);
};

////////////////////////////////////////////////////////////////////////////////////////////////
// Turnip Declaration
////////////////////////////////////////////////////////////////////////////////////////////////

const double TURNIP_DAMAGE   = 2;  // The damage done on impact
const double TURNIP_SPEED    = 6;  // Magnitude of velocity. I.e., non-negative
const double TURNIP_ROTATION = 20; // The angle of rotation each tick

class Turnip : public Projectile
{
public:
    Turnip(StudentWorld* world, const double& x, const double& y);
};

////////////////////////////////////////////////////////////////////////////////////////////////
// Torpedo Declaration
////////////////////////////////////////////////////////////////////////////////////////////////

const double TORPEDO_DAMAGE   = 8; // The damage done on impact
const double TORPEDO_SPEED    = 8; // Magnitude of velocity. I.e., non-negative
const double TORPEDO_ROTATION = 0; // The angle of rotation each tick

class Torpedo : public Projectile
{
public:
    Torpedo(StudentWorld* world, const double& x, const double& y, const int& shotBy);
};

////////////////////////////////////////////////////////////////////////////////////////////////
// Goodie Declaration
////////////////////////////////////////////////////////////////////////////////////////////////

const double GOODIE_SIZE  = 0.5;
const int    GOODIE_DEPTH = 1;

const double GOODIE_VELOCITY_X = -0.75; // Input includes the direction also
const double GOODIE_VELOCITY_Y = -0.75;
const int    GOODIE_SCORE      = 100;

class Goodie : public Actor
{
public:
    Goodie(StudentWorld* world, const int& imageID, const double& x, const double& y, const int& goodieType);
    
        // Actions
    virtual void doSomething();
    
private:
    bool collision();
    int m_goodieType;
};

////////////////////////////////////////////////////////////////////////////////////////////////
// ExtraLifeGoodie Declaration
////////////////////////////////////////////////////////////////////////////////////////////////

class ExtraLifeGoodie : public Goodie
{
public:
    ExtraLifeGoodie(StudentWorld* world, const double& x, const double& y);
};

////////////////////////////////////////////////////////////////////////////////////////////////
// RepairGoodie Declaration
////////////////////////////////////////////////////////////////////////////////////////////////

class RepairGoodie : public Goodie
{
public:
    RepairGoodie(StudentWorld* world, const double& x, const double& y);
};

////////////////////////////////////////////////////////////////////////////////////////////////
// TorpedoGoodie Declaration
////////////////////////////////////////////////////////////////////////////////////////////////

class TorpedoGoodie : public Goodie
{
public:
    TorpedoGoodie(StudentWorld* world, const double& x, const double& y);
};

#endif // ACTOR_H_
