  //-----------------------------------------------------------------------------

const int wWidth = 800;
const int wHeight = 800;

const int OBJECTS_MAX = 1000;

const int ENEMY_N = 5;

const int SPEED = 4;

const int DAMAGE_MIN = 5;
const int DAMAGE_MAX = 15;

const int ENEMY_SHOOTING_FREQ = 200;

const int FOOD_SPAWNING_FREQ = 4;

const double EPSILON = 0.1;

const bool ENEMY_SPEAK = true;

//-----------------------------------------------------------------------------

#define TX_USE_SPEAK
#include <TXLib.h>
#include "Buttons.h"
#include "ResourceManager.h"
#include "EnemyPhrases.h"

//-----------------------------------------------------------------------------

enum GameOverResult

{

    GameOverResultRestart,
    GameOverResultExit

};

enum RunResult

{

    ResultGameOver,
    ResultExit

};

enum ObjectType

{

    TypeNone,

    TypeTank,
    TypeEnemy,
    TypeBullet,
    TypeEnemyBullet,
    TypeFood,
    TypeMedkit,
    TypeCoin,
    TypeLevelupText,
    TypeExplosion,

    TypeAmount

};

//-----------------------------------------------------------------------------

double rnd (double from, double to);

//-----------------------------------------------------------------------------

class ObjectManager;

ResourceManager * ResManager;

struct AbstractObject

{

    int type_;

    double x_;
    double y_;

    double vx_;
    double vy_;

    double r_;

    COLORREF color_;

    bool visible_;

    ObjectManager * manager_;

    AbstractObject (int type, double x, double y, double vx, double vy, double r, COLORREF color, bool visible, ObjectManager * manager);

    virtual void draw ();

    virtual void move ();

    virtual void control ();

    virtual void hit (AbstractObject * object);

    virtual void remove ();

    virtual void setPosition (double x, double y);

    int getType ();

};

AbstractObject::AbstractObject (int type, double x, double y, double vx, double vy, double r, COLORREF color, bool visible, ObjectManager * manager = NULL) :

    type_    (type),
    x_       (x),
    y_       (y),
    vx_      (vx),
    vy_      (vy),
    r_       (r),
    color_   (color),
    visible_ (visible),
    manager_ (manager)

{}

//-----------------------------------------------------------------------------

struct Animation

{

    HDC spritesheet_;

    double width_;

    double height_;

    int frame_n_;

    int frame_;

    Animation (HDC spritesheet, int frame_n_);

    void draw (double x, double y);

    void draw (double x, double y, double alpha);

    void setFrame (int frame);

    void advanceFrame ();

};

Animation::Animation (HDC spritesheet, int frame_n) :

    spritesheet_ (spritesheet),
    width_       (0),
    height_      (0),
    frame_n_     (frame_n),
    frame_       (0)

{

    width_  = txGetExtentX (spritesheet_) / frame_n_;
    height_ = txGetExtentY (spritesheet_);

}

//-----------------------------------------------------------------------------

struct GameObject : AbstractObject

{

    Animation animation_;

    GameObject (int type, int image, double x, double y, double r);

    virtual void draw () override;

};

GameObject::GameObject (int type, int image, double x, double y, double r) :

    AbstractObject (type, x, y, 0, 0, r, 0, true),

    animation_ (ResManager -> getImage (image), ResManager -> getFrames (image))

{}

//-----------------------------------------------------------------------------

struct Tank : GameObject

{

    int gun_length;
    double gun_vx;
    double gun_vy;

    double speed_;

    int health_;

    int reloading_;

    Tank (double x, double y, double speed, COLORREF color, int gun_length_);

    virtual void draw () override;

    virtual void control () override;

    void check ();

    void addHealth (double health);

};

Tank::Tank (double x, double y, double speed, COLORREF color, int gun_length_) :

    GameObject (TypeTank, ImageTank, x, y, 30),

    gun_length (gun_length_),
    health_    (100),
    gun_vx     (0),
    gun_vy     (0),
    speed_     (speed),
    reloading_ (300)

{}

//-----------------------------------------------------------------------------

struct Enemy : GameObject

{

    int health_;

    bool moving_;

    double moving_vx;
    double moving_vy;

    int moving_length;

    int moving_time;

    Enemy ();

    virtual void draw () override;

    virtual void move () override;

    virtual void control () override;

    void Reset ();

};

Enemy::Enemy () :

    GameObject (TypeEnemy, ImageEnemy, rnd (50, wWidth - 50), rnd (50, wHeight - 50), 20),

    health_       (100),
    moving_       (false),
    moving_length (0),
    moving_time   (0)

{}

//-----------------------------------------------------------------------------

struct Bullet : AbstractObject

{

    int damage_;

    double speed_;

    double deflection_;

    Bullet (double x, double y, double vx, double vy, COLORREF color, int damage, double speed, double deflection, bool visible, ObjectManager * manager);

    virtual void move () override;

    virtual void remove () override;

};

Bullet::Bullet (double x, double y, double vx, double vy, COLORREF color, int damage, double speed, double deflection, bool visible, ObjectManager * manager) :

    AbstractObject (TypeBullet, x, y, vx, vy, 3, color, visible, manager),

    damage_     (damage),
    speed_      (speed),
    deflection_ (deflection)

{}

//-----------------------------------------------------------------------------

struct EnemyBullet : AbstractObject

{

    int damage_;

    double speed_;

    bool visible_;

    EnemyBullet ();

    EnemyBullet (ObjectManager * manager);

    EnemyBullet (double x, double y, double vx, double vy, ObjectManager * manager);

    virtual void move () override;

    virtual void remove () override;

};

EnemyBullet::EnemyBullet () :

    AbstractObject (TypeEnemyBullet, 0, 0, 0, 0, 0, 0, false),

    damage_     (0),
    speed_      (0)

{}

EnemyBullet::EnemyBullet (ObjectManager * manager) :

    AbstractObject (TypeEnemyBullet, 0, 0, 0, 0, 3, TX_BLACK, false, manager),

    damage_     (0),
    speed_      (0)

{}

EnemyBullet::EnemyBullet (double x, double y, double vx, double vy, ObjectManager * manager) :

    AbstractObject (TypeEnemyBullet, x, y, vx, vy, 3, RGB (0, 179, 255), true, manager),

    damage_  (25),
    speed_   (rnd (3, 4))

{}

//-----------------------------------------------------------------------------

struct Medkit : GameObject

{

    Medkit (double x, double y);

    virtual void draw () override;

};

Medkit::Medkit (double x, double y) :

    GameObject (TypeMedkit, ImageMedkit, x, y, 15)

{}

//-----------------------------------------------------------------------------

struct Food : GameObject

{

    Food (double x, double y);

    virtual void draw () override;

};

Food::Food (double x, double y) :

    GameObject (TypeFood, ImageFood, x, y, 15)

{}

//-----------------------------------------------------------------------------

struct Coin : GameObject

{

    int counter_;

    Coin (double x, double y);

    virtual void draw ();

};

Coin::Coin (double x, double y) :

    GameObject (TypeCoin, ImageCoin, x, y, 15),

    counter_ (0)

{}

//-----------------------------------------------------------------------------

struct LevelUpText : GameObject

{

    double alpha_;

    LevelUpText ();

    virtual void draw ();

    virtual void move ();

};

LevelUpText::LevelUpText () :

    GameObject (TypeLevelupText, ImageLevelUpText, wWidth / 2, wHeight / 2, 0),

    alpha_ (1)

{}

//-----------------------------------------------------------------------------

struct Explosion : GameObject

{

    int counter_;

    Explosion (double x, double y);

    virtual void draw () override;

    virtual void move () override;

};

Explosion::Explosion (double x, double y) :

    GameObject (TypeExplosion, ImageExplosion, x, y, 0),

    counter_ (0)

{}

//-----------------------------------------------------------------------------

struct ObjectManager

{

    AbstractObject * objects_[OBJECTS_MAX];

    COLORREF bkcolor_;

    int score_;

    int level_;

    ObjectManager ();

    void drawObjects ();

    void moveObjects ();

    void controlObjects ();

    void manageObjects ();

    void checkCollision ();

    int objectsAmount ();

    int addObject (AbstractObject * object);

    int removeObject (AbstractObject * object);

    void clearObjects ();

    void addScore ();

    void addScore (int score);

};

ObjectManager::ObjectManager () :

    objects_         ({}),
    bkcolor_         (RGB (45, 45, 45)),
    score_           (0),
    level_           (0)

{}

//-----------------------------------------------------------------------------

int gameOver (ObjectManager * manager);

double sqrDistance (double x, double y, double x1, double y1);

bool collisionDetection (const AbstractObject * obj1, const AbstractObject * obj2);

double sqr (double d);

int run (ObjectManager * manager);

template <typename T>

T * getObject (ObjectManager * manager);

template <typename T>

T * checkType (AbstractObject * object);

double angle (double x0, double y0, double x1, double y1);

void Hit (AbstractObject * obj1, AbstractObject * obj2);

//{----------------------------------------------------------------------------

void hit_TankBullet      (AbstractObject * obj1, AbstractObject * obj2);
void hit_TankEnemyBullet (AbstractObject * obj1, AbstractObject * obj2);
void hit_TankFood        (AbstractObject * obj1, AbstractObject * obj2);
void hit_TankMedkit      (AbstractObject * obj1, AbstractObject * obj2);
void hit_TankCoin        (AbstractObject * obj1, AbstractObject * obj2);

void hit_EnemyBullet     (AbstractObject * obj1, AbstractObject * obj2);

void hit_BulletObject    (AbstractObject * obj1, AbstractObject * obj2);

void hit_Error           (AbstractObject * obj1, AbstractObject * obj2);
void hit_None            (AbstractObject * obj1, AbstractObject * obj2);

//}----------------------------------------------------------------------------

typedef void hit_t (AbstractObject * obj1, AbstractObject * obj2);

const hit_t * hitTable[TypeAmount][TypeAmount] =

    {
    //   0          1                    2                3                 4                    5                 6                 7                 8           9
    //   TypeNone   Tank                 Enemy            Bullet            EnemyBulet           Food              Medkit            Coin              LevelUpText Explosion

        {hit_Error, hit_Error,           hit_Error,       hit_Error,        hit_Error,           hit_Error,        hit_Error,        hit_Error,        hit_Error,  hit_Error}, // 0 TypeNone
        {hit_Error, hit_Error,           hit_None,        hit_TankBullet,   hit_TankEnemyBullet, hit_TankFood,     hit_TankMedkit,   hit_TankCoin,     hit_None,   hit_None }, // 1 Tank
        {hit_Error, hit_None,            hit_None,        hit_EnemyBullet,  hit_None,            hit_None,         hit_None,         hit_None,         hit_None,   hit_None }, // 2 Enemy
        {hit_Error, hit_TankBullet,      hit_EnemyBullet, hit_None,         hit_None,            hit_BulletObject, hit_BulletObject, hit_BulletObject, hit_None,   hit_None }, // 3 Bullet
        {hit_Error, hit_TankEnemyBullet, hit_None,        hit_None,         hit_None,            hit_None,         hit_None,         hit_None,         hit_None,   hit_None }, // 4 EnemyBullet
        {hit_Error, hit_TankFood,        hit_None,        hit_BulletObject, hit_None,            hit_None,         hit_None,         hit_None,         hit_None,   hit_None }, // 5 Food
        {hit_Error, hit_TankMedkit,      hit_None,        hit_BulletObject, hit_None,            hit_None,         hit_None,         hit_None,         hit_None,   hit_None }, // 6 Medkit
        {hit_Error, hit_TankCoin,        hit_None,        hit_BulletObject, hit_None,            hit_None,         hit_None,         hit_None,         hit_None,   hit_None }, // 7 Coin
        {hit_Error, hit_None,            hit_None,        hit_None,         hit_None,            hit_None,         hit_None,         hit_None,         hit_None,   hit_None }, // 8 LevelUpText
        {hit_Error, hit_None,            hit_None,        hit_None,         hit_None,            hit_None,         hit_None,         hit_None,         hit_None,   hit_None }  // 9 Explosion

    };

//-----------------------------------------------------------------------------

int main ()

{

    txCreateWindow (wWidth, wHeight);

    ResourceManager resManager (Resources, "Resources\\Images");

    ResManager = &resManager;

    while (true)

    {

        ObjectManager manager;

        txBegin ();

        txDisableAutoPause ();

        for (int n = 0; n < ENEMY_N; n++) manager.addObject (new Enemy);

        manager.addObject (new Tank {70, 40, SPEED, TX_GREEN, 40});

        int result = run (&manager);

        switch (result)

        {

            case ResultExit:

            {

                return 0;

            }

            case ResultGameOver:

            {

                switch (gameOver (&manager))

                {

                    case GameOverResultExit:

                    {

                        return 0;

                    }

                }

            }

        }

    }

    ResManager = nullptr;

    return 0;

}

//-----------------------------------------------------------------------------

int run (ObjectManager * manager)

{

    while (!GetAsyncKeyState (VK_ESCAPE))

    {

        manager -> manageObjects ();

        if (GetAsyncKeyState (VK_DELETE))

        {

            const int delay = 50;

            bool highlight = false;

            int time = 0;

            const char * deleteText = "[Delete]";

            while (true)

            {

                if (txMouseButtons () == 1)

                {

                    POINT pos = txMousePos ();

                    AbstractObject * click = new AbstractObject {TypeNone, pos.x, pos.y, 0, 0, 1, 0, false, NULL};

                    for (int n = 0; n < OBJECTS_MAX; n++)

                    {

                        if (!manager -> objects_[n]) continue;

                        if (collisionDetection (manager -> objects_[n], click))

                        {

                            if (checkType <Tank> (manager -> objects_[n]))

                            {

                                MessageBox (txWindow (), "You may not remove tank.", "Error", MB_ICONWARNING | MB_OK);

                                continue;

                            }

                            manager -> objects_[n] -> remove ();

                        }


                    }

                }

                if (GetAsyncKeyState (VK_ESCAPE) || GetAsyncKeyState (13))

                {

                    txSleep (100);

                    break;

                }

                if (GetAsyncKeyState (VK_TAB))

                {

                    if (highlight) highlight = false;
                    else highlight = true;

                    txSleep (100);

                }

                manager -> drawObjects ();

                if (highlight)

                {

                    for (int n = 0; n < OBJECTS_MAX; n++)

                    {

                        if (!manager -> objects_[n]) continue;

                        AbstractObject * object = manager -> objects_[n];

                        double x = object -> x_, y = object -> y_, r = object -> r_;

                        txSetColor (TX_WHITE, 1);
                        txSetFillColor (TX_TRANSPARENT);

                        txCircle (x, y, r);
                        txLine (x - r, y, x + r, y);
                        txLine (x, y - r, x, y + r);

                    }

                }

                if (time < delay)

                {

                    txSetColor (RGB (255, 225, 0));
                    txSelectFont ("Arial", 100);

                    txTextOut (wWidth / 2 - txGetTextExtentX (deleteText) / 2, wHeight / 2 - txGetTextExtentY (deleteText) / 2, deleteText);

                }

                char text [100] = "";

                txSetColor (RGB (0, 170, 255));
                txSelectFont ("Arial", 30);

                txTextOut (wWidth - txGetTextExtentX (text) - 2, 2, text);

                sprintf (text, "Objects Amount: %d", manager -> objectsAmount ());

                txTextOut (wWidth - txGetTextExtentX (text) - 2, 2, text);

                time ++;

                if (time > delay * 2) time = 0;

                txSleep (1);

            }

        }

        if (GetAsyncKeyState (VK_TAB))

        {

            manager -> addObject (new Enemy);

            txSleep (100);

        }

        if (GetAsyncKeyState ('K'))

        {

            manager -> addObject (new Coin {floor (rnd (0, wWidth)), floor (rnd (0, wHeight))});

            txSleep (100);

        }

        Tank * tank = getObject <Tank> (manager);

        assert (tank);

        if (tank -> health_ <= 0)

        {
            manager -> clearObjects ();
            return ResultGameOver;

        }

        txSleep (1);

    }

    manager -> clearObjects ();

    return ResultExit;

}

//-----------------------------------------------------------------------------

void AbstractObject::draw ()

{

    if (visible_)

    {

        txSetColor (color_);
        txSetFillColor (color_);
        txCircle (x_, y_, r_);

    }

}

//-----------------------------------------------------------------------------

void AbstractObject::move ()

{

    if (visible_)

    {

        x_ += vx_;
        y_ += vy_;

        if (x_ + r_ > wWidth || x_ - r_ < 0 || y_ + r_ > wHeight || y_ - r_ < 0)

        {


             if (x_ + r_ >= wWidth || x_ - r_ <= 0)

             {

                vx_ = -vx_;


             }

             if (y_ + r_ >= wHeight || y_ - r_ <= 0)

             {

                vy_ = -vy_;


             }

        }

    }

}

//-----------------------------------------------------------------------------

void AbstractObject::hit (AbstractObject * object)

{

    ::Hit (this, object);

}

//-----------------------------------------------------------------------------

void AbstractObject::control ()

{

}

//-----------------------------------------------------------------------------

void AbstractObject::remove ()

{

    manager_ -> removeObject (this);

}

//-----------------------------------------------------------------------------

void AbstractObject::setPosition (double x, double y)

{


    if (x - r_ >= 0 && x + r_ < wWidth && y - r_ >= 0 && y + r_ < wHeight)

    {

        x_ = x;
        y_ = y;

    }

}

//-----------------------------------------------------------------------------

int AbstractObject::getType ()

{

    assert (TypeNone < type_ && type_ < TypeAmount);

    return type_;

}

//-----------------------------------------------------------------------------

void Animation::draw (double x, double y)

{

    txAlphaBlend (txDC (), x, y, width_, height_, spritesheet_, width_ * frame_, 0, 1);

}

//-----------------------------------------------------------------------------

void Animation::draw (double x, double y, double alpha)

{

    txAlphaBlend (txDC (), x, y, width_, height_, spritesheet_, width_ * frame_, 0, alpha);

}

//-----------------------------------------------------------------------------

void Animation::setFrame (int frame)

{

    assert (frame >= 0);

    assert (frame < frame_n_);

    frame_ = frame;

}

//-----------------------------------------------------------------------------

void Animation::advanceFrame ()

{

    frame_ ++;

    if (frame_ >= frame_n_) frame_ = 0;

}

//-----------------------------------------------------------------------------

void GameObject::draw ()

{

}

//-----------------------------------------------------------------------------

void Tank::draw ()

{

    double A = 360 - angle (x_, y_, txMouseX (), txMouseY ());

    int frame = animation_.frame_n_ / 360.0 * A;

    animation_.setFrame (frame);

    animation_.draw (x_ - animation_.width_ / 2, y_ - animation_.height_ / 2);

    int x = x_ - 20;
    int y = y_ + 25;
    int x1 = x + 40;
    int y1 = y + 13;

    txSetColor (TX_BLACK);
    txSetFillColor (TX_BLACK);
    txRectangle (x, y, x1, y1);

    int width = 40.0 / 100.0 * health_;

    int c = 255.0 / 100.0 * health_;

    COLORREF color = RGB (255 - c, c, 0);

    txSetColor (color);
    txSetFillColor (color);
    txRectangle (x + 1, y + 1, x + width - 1, y1 - 1);

    char text[100] = "";

    sprintf (text, "%d", health_);

    txSetColor (RGB (c, 255 - c, 0));
    txSelectFont ("Arial", y1 - y);
    txDrawText (x, y, x1, y1, text);

    y  = y1;
    y1 = y + 5;

    color = RGB (0, 0, 255);

    txSetColor (TX_BLACK);
    txSetFillColor (TX_BLACK);
    txRectangle (x, y, x1, y1);

    width = 40.0 / 300.0 * reloading_;

    txSetColor (color);
    txSetFillColor (color);
    txRectangle (x + 1, y + 1, x + width - 1, y1 - 1);

}

//-----------------------------------------------------------------------------

void Tank::check ()

{

    POINT mPos = txMousePos ();

    double a = mPos.x - x_;
    double b = mPos.y - y_;

    double c = sqrt (a * a + b * b);

    gun_vx = a / c;
    gun_vy = b / c;

}

//-----------------------------------------------------------------------------

void Tank::control ()

{

    int level = manager_ -> level_;

    if (txMouseButtons () == 1 || GetAsyncKeyState (VK_SPACE))

    {

        if (reloading_ >= 300)

        {

            Bullet * bullet = new Bullet {x_ + gun_vx * gun_length, y_ + gun_vy * gun_length, gun_vx + rnd (-0.1, 0.1),
                                          gun_vy + rnd (-0.1, 0.1), TX_WHITE, rnd (DAMAGE_MIN, DAMAGE_MAX), rnd (3, 4) + level / 2.0, rnd (-5, 5), true, manager_};

            manager_ -> addObject (bullet);

            reloading_ = 0;

        }

    }

    else

    {

        reloading_ = 300;

    }

    if (reloading_ < 300)

    {

        reloading_ += 10 * (level + 1);

    }

    if (GetAsyncKeyState ('W'))

    {

        if (y_ - speed_ - r_ >= 0)

        {

            y_ -= speed_;

        }

    }

    if (GetAsyncKeyState ('A'))

    {

        if (x_ - speed_ - r_ >= 0)

        {

            x_ -= speed_;

        }

    }

    if (GetAsyncKeyState ('S'))

    {

        if (y_ + speed_ + r_ <= wHeight)

        {

            y_ += speed_;

        }

    }

    if (GetAsyncKeyState ('D'))

    {

        if (x_ + speed_ + r_ <= wWidth)

        {

            x_ += speed_;

        }

    }

    check ();

}

//-----------------------------------------------------------------------------

void Tank::addHealth (double health)

{

    health_ += floor (health);

    if (health_ > 100)

    {

        health_ = 100;

    }

    else if (health_ < 0)

    {

        health_ = 0;

    }

}

//-----------------------------------------------------------------------------

void Enemy::draw ()

{

    if (health_ <= 0)

    {

        return;

    }

    animation_.draw (x_ - animation_.width_ / 2, y_ - animation_.height_ / 2);
    animation_.advanceFrame ();

    int x = x_ - 20;
    int y = y_ + 25;
    int x1 = x + 40;
    int y1 = y + 15;

    txSetColor (TX_BLACK);
    txSetFillColor (TX_BLACK);
    txRectangle (x, y, x1, y1);

    int width = 40.0 / 100.0 * health_;

    int c = 255.0 / 100.0 * health_;

    COLORREF color = RGB (255 - c, c, 0);

    txSetColor (color);
    txSetFillColor (color);
    txRectangle (x + 1, y + 1, x + width - 1, y1 - 1);

    char text[100] = "";

    sprintf (text, "%d", health_);

    txSetColor (RGB (c, 255 - c, 0));
    txSelectFont ("Arial", 15);
    txDrawText (x, y, x1, y1, text);

}

//-----------------------------------------------------------------------------

void Enemy::move ()

{

    if (!moving_)

    {

        if (floor (rnd (1, 300)) == 1)

        {

            moving_ = true;
            moving_time = 0;

            moving_vx = rnd (-1, 1) / 20;
            moving_vy = rnd (-1, 1) / 20;
            moving_length = 50;

        }

    }

    else

    {

        if (moving_time <= moving_length)

        {

            setPosition (x_ + moving_vx * moving_time, y_ + moving_vy * moving_time);

            moving_time ++;

        }

        else

        {

            moving_ = false;

        }

    }

}

//-----------------------------------------------------------------------------

void Enemy::control ()

{

    if (floor (rnd (1, ENEMY_SHOOTING_FREQ)) == 1)

    {

        Tank * tank = getObject <Tank> (manager_);

        assert (tank);

        double a = x_ - (tank -> x_);
        double b = y_ - (tank -> y_);

        double c = sqrt (a * a + b * b);

        double vx = a / c;
        double vy = b / c;

        EnemyBullet * bullet = new EnemyBullet {x_ + vx * 24, y_ + vy * 24, vx, -vy, manager_};

        manager_ -> addObject (bullet);

    }

    if (health_ <= 0)

    {

        manager_ -> addScore ();

        if (manager_ -> score_ % 10 == 0) manager_ -> addObject (new LevelUpText);

        if (floor (rnd (1, FOOD_SPAWNING_FREQ)) == 1)

        {

            if (floor (rnd (1, FOOD_SPAWNING_FREQ)) == 1)

            {

                if (floor (rnd (1, FOOD_SPAWNING_FREQ)) == 1)

                {

                    manager_ -> addObject (new Coin {x_, y_});

                }

                else manager_ -> addObject (new Medkit {x_, y_});

            }

            else manager_ -> addObject (new Food {x_, y_});

            txPlaySound ("Resources\\Sound\\Food_Spawn.wav");

        }

        Reset ();

    }

}


void Enemy::Reset ()

{

    x_ = rnd (50, wWidth - 50);
    y_ = rnd (50, wHeight - 50);

    health_ = 100;

    if (ENEMY_SPEAK)

    {

        int n = 0;

        for (n = 0; EnemyPhrases[n]; n++);

        n = floor (rnd (0, n));

        const char * phrase = EnemyPhrases[n];

        char text[256] = "";

        sprintf (text, "\a%s", phrase);

        txSpeak (text);

    }

}

//-----------------------------------------------------------------------------

void Bullet::move ()

{

    if (visible_)

    {

        x_ += vx_ * speed_;
        y_ += vy_ * speed_;

        if (x_ > wWidth || x_ < 0 || y_ > wHeight || y_ < 0)

        {

            if (floor (rnd (1, 17)) == 1)

            {

                 if (x_ > wWidth || x_ < 0)

                 {

                    vx_ = -vx_;

                 }

                 if (y_ > wHeight || y_ < 0)

                 {

                    vy_ = -vy_;

                 }

                 color_ = RGB (255, 0, 0);

                 damage_ = damage_ * 3;

            }

            else

            {

                remove ();

            }

        }

    }

}

//-----------------------------------------------------------------------------

void Bullet::remove ()

{

    manager_ -> removeObject (this);
    manager_ -> addObject (new Explosion (x_, y_));

}

//-----------------------------------------------------------------------------

void EnemyBullet::move ()

{

    x_ -= speed_ * vx_;
    y_ += speed_ * vy_;

    if (x_ - r_ < 0 || x_ + r_ > wWidth || y_ - r_ < 0 || y_ + r_ > wHeight)

    {

        remove ();

    }

}

//-----------------------------------------------------------------------------

void EnemyBullet::remove ()

{

    manager_ -> removeObject (this);
    manager_ -> addObject (new Explosion (x_, y_));

}

//-----------------------------------------------------------------------------

void Food::draw ()

{

    animation_.setFrame (0);
    animation_.draw (x_ - animation_.width_ / 2, y_ - animation_.height_ / 2);

}

//-----------------------------------------------------------------------------

void Medkit::draw ()

{

    animation_.setFrame (0);
    animation_.draw (x_ - animation_.width_ / 2, y_ - animation_.height_ / 2);

}

//-----------------------------------------------------------------------------

void Coin::draw ()

{

    animation_.draw (x_ - animation_.width_ / 2, y_ - animation_.height_ / 2);

    counter_ ++;

    if (counter_ > 10)

    {

        animation_.advanceFrame ();

        counter_ = 0;

    }

}

//-----------------------------------------------------------------------------

void LevelUpText::draw ()

{

    animation_.draw (x_ - animation_.width_ / 2, y_ - animation_.height_ / 2, alpha_);

}

//-----------------------------------------------------------------------------

void LevelUpText::move ()

{

    alpha_ -= 0.01;
    y_ -= 0.5;

    if (alpha_ <= 0) remove ();

}

//-----------------------------------------------------------------------------

void Explosion::draw ()

{

    animation_.draw (x_ - animation_.width_ / 2, y_ - animation_.height_ / 2);

    counter_ ++;

    if (counter_ % 5 == 0)

    {

        animation_.advanceFrame ();

    }

}

//-----------------------------------------------------------------------------

void Explosion::move ()

{

    if (counter_ >= animation_.frame_n_ * 5) remove ();

}

//-----------------------------------------------------------------------------

int gameOver (ObjectManager * manager)

{

    Button buttons[] = {

        {wWidth / 2 - 125, wHeight / 2 - 25, 100, 30, "Restart", "Arial", 30, txDC (), RGB (60, 60, 60), TX_WHITE, TX_WHITE, false, 2},
        {wWidth / 2 +  25, wHeight / 2 - 25, 100, 30,    "Exit", "Arial", 30, txDC (), RGB (60, 60, 60), TX_WHITE, TX_WHITE, false, 2},
        EndButton

    };

    HDC image = ResManager -> getImage (ImageDie);

    double width = txGetExtentX (image);
    double height = txGetExtentY (image);

    bool videolan = true;

    int score = manager -> score_;
    int level = manager -> level_;

    if (txPlayVideo ("") <= 0) videolan = false;

    if (videolan)

    {

        int time = GetTickCount();

        txSetFillColor (TX_BLACK);
        txClear ();

        txSleep (100);

        HWND video = (HWND) txPlayVideo ("\a" "Resources\\Video\\Died.mp4", 0, 0.1, txWindow ());

        while (!GetAsyncKeyState (VK_ESCAPE))

        {

            if (GetTickCount() - time > 8000)

            {

                break;

            }

            txSleep (10);

        }

        txSleep (100);

        txDestroyWindow (video);

    }

    else

    {

        for (double alpha = 0; alpha < 1; alpha += 0.01)

        {

            txSetFillColor (TX_BLACK);
            txClear ();

            txAlphaBlend (txDC (), wWidth / 2 - width / 2, wHeight / 2 - height / 2, 0, 0, image, 0, 0, alpha);

            txSleep (5);

        }

    }

    while (!GetAsyncKeyState (VK_ESCAPE))

    {

        txSetFillColor (RGB (45, 45, 45));
        txClear ();

        if (!videolan) txAlphaBlend (txDC (), wWidth / 2 - width / 2, wHeight / 2 - height / 2, 0, 0, image, 0, 0, 1);

        const char * text = "Game over";

        txSelectFont ("Segoe Script", 50);
        txSetColor (TX_WHITE);
        txTextOut (wWidth / 2 - txGetTextExtentX (text) / 2, wHeight / 2 - txGetTextExtentY (text) / 2 - 100, text);

        char scoreText[100] = "";

        sprintf (scoreText, "Score: %d", score);

        txSelectFont ("Arial", 30);
        txTextOut (wWidth / 2 - txGetTextExtentX (scoreText) / 2, wHeight / 2 - txGetTextExtentY (scoreText) / 2 - 70, scoreText);

        sprintf (scoreText, "Level: %d", level);

        txSetColor (RGB (120, 120, 120));
        txSelectFont ("Arial", 25);
        txTextOut (wWidth / 2 - txGetTextExtentX (scoreText) / 2, wHeight / 2 - txGetTextExtentY (scoreText) / 2 - 40, scoreText);

        int result = manageButtons (buttons);

        if (result != -1)

        {

            return result;

        }

        txSleep (1);

    }

    return ResultExit;

}

//-----------------------------------------------------------------------------

double sqrDistance (double x, double y, double x1, double y1)

{

    double a = x - x1;
    double b = y - y1;

    double c = sqr(a) + sqr (b);

    return c;

}

//-----------------------------------------------------------------------------

double rnd (double from, double to)

{

    return from + 1.0 * rand () / RAND_MAX * (to - from);

}

//-----------------------------------------------------------------------------

bool collisionDetection (const AbstractObject * obj1, const AbstractObject * obj2)

{

    double sqrD = sqrDistance (obj1 -> x_, obj1 -> y_, obj2 -> x_, obj2 -> y_);

    if (sqrD <= sqr(obj1 -> r_ + obj2 -> r_))

    {

        return true;

    }

    return false;

}

//-----------------------------------------------------------------------------

double sqr (double d)

{

    return d * d;

}

//-----------------------------------------------------------------------------

void ObjectManager::drawObjects ()

{

    txSetFillColor (bkcolor_);
    txClear ();

    for (int n = 0; n < OBJECTS_MAX; n++)

    {

        if (!objects_[n]) continue;

        objects_[n] -> draw ();

    }

    txSetColor (TX_WHITE);
    txSelectFont ("Arial", 30);

    char text[100] = "";

    sprintf (text, "Score: %d", score_);

    txTextOut (5, 5, text);

    txSetColor (RGB (120, 120, 120));
    txSelectFont ("Arial", 25);

    sprintf (text, "Level: %d", level_);

    txTextOut (5, 37, text);

}

//-----------------------------------------------------------------------------

void ObjectManager::moveObjects ()

{

    for (int n = 0; n < OBJECTS_MAX; n++)

    {

        if (!objects_[n]) continue;

        objects_[n] -> move ();

    }

}

//-----------------------------------------------------------------------------

void ObjectManager::controlObjects ()

{

    for (int n = 0; n < OBJECTS_MAX; n++)

    {

        if (!objects_[n]) continue;

        objects_[n] -> control ();

    }

}

//-----------------------------------------------------------------------------

int ObjectManager::addObject (AbstractObject * object)

{

    for (int n = 0; n < OBJECTS_MAX; n++)

    {

        if (!objects_[n])

        {

            object -> manager_ = this;

            objects_[n] = object;

            return n;

        }

    }

    return -1;

}

//-----------------------------------------------------------------------------

void ObjectManager::manageObjects ()

{

    moveObjects ();
    controlObjects ();
    drawObjects ();
    checkCollision ();

}

//-----------------------------------------------------------------------------

void ObjectManager::checkCollision ()

{

    for (int n = 0; n < OBJECTS_MAX; n++)

    {

        if (!objects_[n]) continue;

        for (int i = n + 1; i < OBJECTS_MAX; i++)

        {

            if (!objects_[i]) continue;
            if (!objects_[n]) break;

            if (collisionDetection (objects_[n], objects_[i]))

            {

                objects_[n] -> hit (objects_[i]);

            }

        }

    }

}

//-----------------------------------------------------------------------------

int ObjectManager::objectsAmount ()

{

    int amount = 0;

    for (int n = 0; n < OBJECTS_MAX; n++)

    {

        if (objects_[n]) amount++;

    }

    return amount;

}

//-----------------------------------------------------------------------------

int ObjectManager::removeObject (AbstractObject * object)

{

    for (int n = 0; n < OBJECTS_MAX; n++)

    {

        if (objects_[n] == object)

        {

            delete (objects_[n]);

            objects_[n] = nullptr;

            return n;

        }

    }

    return -1;

}

//-----------------------------------------------------------------------------

void ObjectManager::clearObjects ()

{

    for (int n = 0; n < OBJECTS_MAX; n++)

    {

        if (objects_[n])

        {

            objects_[n] -> remove ();

        }

    }

}

//-----------------------------------------------------------------------------

void ObjectManager::addScore ()

{

    score_ ++;

    if (score_ % 10 == 0)

    {

        level_ ++;

    }

}

//-----------------------------------------------------------------------------

void ObjectManager::addScore (int score)

{

    for (int i = 0; i < score; i++)

    {

        score_ ++;

        if (score_ % 10 == 0)

        {

            level_ ++;

            addObject (new LevelUpText);

        }

    }

}

//-----------------------------------------------------------------------------

template <typename T>

T * getObject (ObjectManager * manager)

{

    for (int n = 0; n < OBJECTS_MAX; n++)

    {

        if (!manager -> objects_[n]) continue;

        T * object = dynamic_cast <T*> (manager -> objects_[n]);

        if (object) return object;

    }

    return nullptr;

}

//-----------------------------------------------------------------------------

template <typename T>

T * checkType (AbstractObject * object)

{

    T * ptr = dynamic_cast <T*> (object);

    if (ptr) return ptr;

    return nullptr;

}

//-----------------------------------------------------------------------------

double angle (double x0, double y0, double x1, double y1)

{

    double dx = x1 - x0;
    double dy = y1 - y0;

    if (fabs (dx) < EPSILON) dx = 1;
    if (fabs (dy) < EPSILON) dy = 1;

    double A = atan (-dy / dx);

    A = A / M_PI * 180;

    if (dx < 0) A += 180;

    if (dx > 0 && dy > 0) A += 360;

    return A;

}

//-----------------------------------------------------------------------------

void Hit (AbstractObject * obj1, AbstractObject * obj2)

{

    int objectType1 = obj1 -> getType (), objectType2 = obj2 -> getType ();

    if (objectType1 > objectType2) std::swap (obj1, obj2);

    hitTable [objectType1][objectType2] (obj1, obj2);

}

//{----------------------------------------------------------------------------

void hit_TankBullet (AbstractObject * obj1, AbstractObject * obj2)

{

    Tank * tank = checkType <Tank> (obj1);

    tank -> addHealth (-(rnd (2, 6)));

    obj2 -> remove ();

}

void hit_TankEnemyBullet (AbstractObject * obj1, AbstractObject * obj2)

{

    Tank * tank = checkType <Tank> (obj1);

    tank -> addHealth (-15);

    obj2 -> remove ();

}

void hit_TankFood (AbstractObject * obj1, AbstractObject * obj2)

{

    Tank * tank = checkType <Tank> (obj1);

    tank -> addHealth (rnd (10, 16));

    obj2 -> remove ();

    txPlaySound ("Resources\\Sound\\Food_Take.wav");

}

void hit_TankMedkit (AbstractObject * obj1, AbstractObject * obj2)

{

    Tank * tank = checkType <Tank> (obj1);

    tank -> addHealth (100);

    obj2 -> remove ();

    txPlaySound ("Resources\\Sound\\Food_Take.wav");

}

void hit_TankCoin (AbstractObject * obj1, AbstractObject * obj2)

{

    Tank * tank = checkType <Tank> (obj1);

    tank -> addHealth (rnd (20, 30));

    tank -> manager_ -> addScore (10);

    tank -> manager_ -> addObject (new LevelUpText);

    obj2 -> remove ();

    txPlaySound ("Resources\\Sound\\Food_Take.wav");

}

//-----------------------------------------------------------------------------

void hit_EnemyBullet (AbstractObject * obj1, AbstractObject * obj2)

{

    Enemy  * enemy  = checkType <Enemy>  (obj1);
    Bullet * bullet = checkType <Bullet> (obj2);
    enemy -> health_ -= bullet -> damage_;

    enemy -> setPosition (enemy -> x_ + bullet -> vx_ * 7, enemy -> y_ + bullet -> vy_ * 7);

    bullet -> remove ();

}

//-----------------------------------------------------------------------------

void hit_BulletObject (AbstractObject * obj1, AbstractObject * obj2)

{

    obj2 -> setPosition (obj2 -> x_ + obj1 -> vx_ * 7, obj2 -> y_ + obj1 -> vy_);

    obj1 -> vx_ = -obj1 -> vx_;
    obj1 -> vy_ = -obj1 -> vy_;

}

//-----------------------------------------------------------------------------

void hit_Error (AbstractObject * obj1, AbstractObject * obj2)

{

    assert (false);

}

//-----------------------------------------------------------------------------

void hit_None (AbstractObject * obj1, AbstractObject * obj2)

{

}

//}----------------------------------------------------------------------------
