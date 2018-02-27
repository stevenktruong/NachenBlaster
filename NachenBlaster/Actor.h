#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

const int SHOT_BY_ALIEN = 1;

class StudentWorld;

////////////////////////////////////////////////////////////////
// Actor Declaration
////////////////////////////////////////////////////////////////

class Actor : public GraphObject
{
public:
        // Constructors and destructors
    Actor(StudentWorld* world, const int& imageID, const int& x, const int& y, const double& startDirection = 0, const double& size = 1.0, const int& depth = 0);
    ~Actor() {};
    
        // Accessors
    bool isAlive() { return m_alive; };
    bool checkPos(const int& x, const int& y) const;
    virtual bool isCollidable() { return true; };
    virtual bool isAlien() const { return false; };
    virtual bool isPlayer() const { return false; };
    virtual bool isProjectile() const { return false; };
    StudentWorld* getWorld() const { return m_world; };
    
        // Mutators
    virtual void die() { m_alive = false; };
    
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
    Star(StudentWorld* world, const bool& initial = false);
    ~Star() {};
    
        // Accessors
    virtual bool isCollidable() { return false; };
    
        // Actions
    virtual void doSomething();
};

////////////////////////////////////////////////////////////////
// Explosion Declaration
////////////////////////////////////////////////////////////////

class Explosion : public Actor
{
public:
        // Constructors and destructors
    Explosion(StudentWorld* world, const double& x, const double& y);
    
        // Accessors
    virtual bool isCollidable() { return false; };
    
        // Actions
    virtual void doSomething();
    
private:
    int m_ticksAlive;
};

////////////////////////////////////////////////////////////////
// DamageableObject Declaration
////////////////////////////////////////////////////////////////

class DamageableObject : public Actor
{
public:
        // Constructors and destructors
    DamageableObject(StudentWorld* world, const int& imageID, const int& x, const int& y, const double& startDirection = 0,
                     const double& size = 1.0, const int& depth = 0, const double& health = 0);
    
        // Accessors
    double getHealth() const { return m_health; };
    
        // Mutators
    void takeDamage(const double& damage) { m_health -= damage; };
    virtual void sufferDamage(const double& damage) { takeDamage(damage); };
    
        // Actions
    virtual DamageableObject* checkCollision();
    
private:
    double m_health;
};

////////////////////////////////////////////////////////////////
// Blaster Declaration
////////////////////////////////////////////////////////////////

class Blaster : public DamageableObject
{
public:
        // Constructors and destructors
    Blaster(StudentWorld* world);
    ~Blaster() {};
    
        // Accessors
    double getEnergy() const { return m_energy; };
    double getTorpedoes() const { return m_torpedoes; };
    virtual bool isPlayer() const { return true; };
    
        // Actions
    virtual void doSomething();
    //virtual void sufferDamage(const double& damage);
    
private:
    void move(const char& dir); // Pass 'u', 'l', 'd', or 'r' to this
    void fire();
    
    double m_energy;
    double m_torpedoes;
};

////////////////////////////////////////////////////////////////
// Alien Declaration
////////////////////////////////////////////////////////////////

class Alien : public DamageableObject
{
public:
        // Constructors and destructors
    Alien(StudentWorld* world, const int& imageID, const double& x, const double& y, const double& health,
          const double& damage, const double& speed, const int& dy, const unsigned int& score);
    
        // Accessors
    virtual bool isAlien() const { return true; };
    
        // Mutators
    //virtual void sufferDamage(const double& damage);
    void setSpeed(double speed) { m_speed = speed; };
    
        // Actions
    virtual void doSomething();
    virtual void specialAction() = 0;
    virtual void dropGoodie();
    
private:
    void death();
    void collidedWithPlayer();
    
    double m_damage;
    double m_speed;
    double m_score;
    int m_dy; // We don't need m_dx since aliens always move left
    int m_plan;
};

////////////////////////////////////////////////////////////////
// Smallgon Declaration
////////////////////////////////////////////////////////////////

class Smallgon : public Alien
{
public:
        // Constructors and destructors
    Smallgon(StudentWorld* world);
    
        // Actions
    virtual void specialAction();
};

////////////////////////////////////////////////////////////////
// Projectile Declaration
////////////////////////////////////////////////////////////////

class Projectile : public Actor
{
public:
        // Constructors and destructors
    Projectile(StudentWorld* world, const int& imageID, const double& x, const double& y, const double& damage, const int& shotByAlien = SHOT_BY_ALIEN);
    
        // Accessors
    double getDamage() const { return m_damage; };
    
        // Actions
    virtual void doSomething() = 0;
    
private:
    void collidedWith(DamageableObject* collide);
    
    double m_damage;
};

////////////////////////////////////////////////////////////////
// Cabbage Declaration
////////////////////////////////////////////////////////////////

class Cabbage : public Projectile
{
public:
        // Constructors and destructors
    Cabbage(StudentWorld* world, const double& x, const double& y);
    
        // Actions
    virtual void doSomething();
};















#endif // ACTOR_H_
