#include <TXLib.h>
#include "Buttons.h"

//-----------------------------------------------------------------------------

const int wWidth = 800;
const int wHeight = 800;

const int OBJECTS_MAX = 300;

const int ENEMY_N = 5;

const int SPEED = 4;

const int DAMAGE_MIN = 5;
const int DAMAGE_MAX = 15;

const int ENEMY_SHOOTING_FREQ = 200;

const int FOOD_SPAWNING_FREQ = 4;

const bool DEBUG_MODE = false;

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
    TypeTank

};

//-----------------------------------------------------------------------------

double rnd (double from, double to);

//-----------------------------------------------------------------------------

class ObjectManager;

struct GameObject

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

    GameObject ();

    GameObject (ObjectManager * manager);

    GameObject (double x, double y, double vx, double vy, double r, COLORREF color, bool visible, ObjectManager * manager, int type = TypeNone);

    virtual void draw ();

    virtual void move ();

    virtual void control ();

    virtual void hit (GameObject * object);

    virtual void remove ();

};

GameObject::GameObject () :

    x_       (0),
    y_       (0),
    vx_      (0),
    vy_      (0),
    r_       (0),
    color_   (0),
    type_    (TypeNone),
    visible_ (false)

{}

GameObject::GameObject (ObjectManager * manager) :

    x_       (0),
    y_       (0),
    vx_      (0),
    vy_      (0),
    r_       (0),
    color_   (-1),
    type_    (TypeNone),
    manager_ (manager)

{}

GameObject::GameObject (double x, double y, double vx, double vy, double r, COLORREF color, bool visible, ObjectManager * manager = NULL, int type) :

    x_       (x),
    y_       (y),
    vx_      (vx),
    vy_      (vy),
    r_       (r),
    color_   (color),
    visible_ (visible),
    type_    (type),
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

    Animation (const char * path, int frame_n_);

    void draw (double x, double y);

};

Animation::Animation (const char * path, int frame_n) :

    spritesheet_ (txLoadImage (path)),
    width_       (0),
    height_      (0),
    frame_n_     (frame_n),
    frame_       (0)

{

    width_  = txGetExtentX (spritesheet_) / frame_n_;
    height_ = txGetExtentY (spritesheet_);

}

//-----------------------------------------------------------------------------

struct Tank : GameObject

{

    int gun_length;
    double gun_vx;
    double gun_vy;

    double speed_;

    int health_;

    int reloading_;

    //HDC image_;

    Tank (double x, double y, double speed, COLORREF color, int gun_length_);

    virtual void draw () override;

    virtual void control () override;

    virtual void hit (GameObject * object) override;

    void check ();

    void addHealth (double health);

};

Tank::Tank (double x, double y, double speed, COLORREF color, int gun_length_) :

    GameObject (x, y, 0, 0, 20, color, true, NULL, TypeTank),

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

    bool isEnd_;

    Enemy ();

    Enemy (double x, double y, double vx, double vy, COLORREF color);

    virtual void draw () override;

    virtual void move () override;

    virtual void control () override;

    virtual void hit (GameObject * object) override;

    void Reset ();

    void setPosition (int x, int y);

};

Enemy::Enemy () :

    GameObject (rnd (50, wWidth - 50), rnd (50, wHeight - 50), 0, 0, 20, TX_RED, true),

    health_       (100),
    moving_       (false),
    moving_length (0),
    moving_time   (0),
    isEnd_        (false)

{}

Enemy::Enemy (double x, double y, double vx, double vy, COLORREF color) :

    GameObject (x, y, vx, vy, 20, color, true),

    health_       (100),
    moving_       (false),
    moving_length (0),
    moving_time   (0),
    isEnd_        (false)

{}

//-----------------------------------------------------------------------------

struct Food : GameObject

{

    HDC image_;

    Food (double x, double y);

    virtual void draw () override;

    virtual void hit (GameObject * object) override;

    virtual void remove () override;

};

Food::Food (double x, double y) :

    GameObject (x, y, 0, 0, 15, 0, true),

    image_ (txLoadImage ("Resources\\Images\\Food.bmp"))

{}

//-----------------------------------------------------------------------------

struct Bullet : GameObject

{

    int damage_;

    double speed_;

    double deflection_;

    Bullet ();

    Bullet (ObjectManager * manager);

    Bullet (double x, double y, double vx, double vy, COLORREF color, int damage, double speed, double deflection, bool visible, ObjectManager * manager);

    virtual void move () override;

    virtual void hit (GameObject * object) override;

};

Bullet::Bullet (ObjectManager * manager) :

    GameObject (0, 0, 0, 0, 3, TX_BLACK, false, manager),

    damage_     (0),
    speed_      (0),
    deflection_ (0)

{}

Bullet::Bullet (double x, double y, double vx, double vy, COLORREF color, int damage, double speed, double deflection, bool visible, ObjectManager * manager) :

    GameObject (x, y, vx, vy, 3, color, visible, manager),

    damage_     (damage),
    speed_      (speed),
    deflection_ (deflection)

{}

//-----------------------------------------------------------------------------

struct EnemyBullet : GameObject

{

    int damage_;

    double speed_;

    bool visible_;

    EnemyBullet ();

    EnemyBullet (ObjectManager * manager);

    EnemyBullet (double x, double y, double vx, double vy, ObjectManager * manager);

    virtual void move () override;

    virtual void hit (GameObject * object) override;

};

EnemyBullet::EnemyBullet () :

    GameObject (0, 0, 0, 0, 0, 0, false),

    damage_     (0),
    speed_      (0)

{}

EnemyBullet::EnemyBullet (ObjectManager * manager) :

    GameObject (0, 0, 0, 0, 3, TX_BLACK, false, manager),

    damage_     (0),
    speed_      (0)

{}

EnemyBullet::EnemyBullet (double x, double y, double vx, double vy, ObjectManager * manager) :

    GameObject (x, y, vx, vy, 3, RGB (0, 179, 255), true, manager),

    damage_  (25),
    speed_   (rnd (3, 4))

{}

//-----------------------------------------------------------------------------

struct Medkit : GameObject

{

    HDC image_;

    Medkit (double x, double y);

    virtual void draw () override;

    virtual void hit (GameObject * object) override;

    virtual void remove () override;

};

Medkit::Medkit (double x, double y) :

    GameObject (x, y, 0, 0, 15, 0, true),

    image_ (txLoadImage ("Resources\\Images\\Medkit.bmp"))

{}

//-----------------------------------------------------------------------------

struct Wall : GameObject

{

    HDC image_;

    Wall ();

    virtual void draw () override;

    virtual void remove () override;

};

Wall::Wall () :

    GameObject (rnd (50, wWidth - 50), rnd (50, wHeight - 50), 0, 0, 50, 0, true),

    image_ (txLoadImage ("Resources\\Images\\Wall_1.bmp"))

{}

//-----------------------------------------------------------------------------

struct ObjectManager

{

    GameObject * objects_[OBJECTS_MAX];

    COLORREF bkcolor_;

    int score_;

    ObjectManager ();

    ObjectManager (HDC backgroundImage);

    void drawObjects ();

    void moveObjects ();

    void controlObjects ();

    void manageObjects ();

    void checkCollision ();

    int objectsAmount ();

    int addObject (GameObject * object);

    int removeObject (GameObject * object);

    void clearObjects ();

};

ObjectManager::ObjectManager () :

    objects_         ({}),
    score_           (0),
    bkcolor_         (RGB (45, 45, 45))

{}

//-----------------------------------------------------------------------------

COLORREF addColor (COLORREF color, int r, int g, int b);

void drawScore (int score);

int gameOver (int score);

double sqrDistance (double x, double y, double x1, double y1);

void info (const char * text);

void title (int time);

bool collisionDetection (const GameObject * obj1, const GameObject * obj2);

double sqr (double d);

int run (ObjectManager * manager);

template <typename T>

T * getObject (ObjectManager * manager);

template <typename T>

T * checkType (GameObject * object);

HDC CaptureScreen (int x, int y, int width, int height);

void drawFragment (int x, int y, HDC * image, int n, double width, double height, double alpha);

//-----------------------------------------------------------------------------

int main ()

{

    if (DEBUG_MODE)

    {

        _txConsoleMode = SW_SHOW;

        SetConsoleTitle ("Debug");

        printf ("Debug console included.\n");

    }

    txCreateWindow (wWidth, wHeight);

    while (true)

    {

        ObjectManager manager;

        txBegin ();

        txDisableAutoPause ();

        for (int n = 0; n < ENEMY_N; n++) manager.addObject (new Enemy);

        Tank * tank = new Tank {70, 40, SPEED, TX_GREEN, 40};

        manager.addObject (tank);

        int result = run (&manager);

        switch (result)

        {

            case ResultExit:

            {

                return 0;

            }

            case ResultGameOver:

            {

                switch (gameOver (manager.score_))

                {

                    case GameOverResultExit:

                    {

                        return 0;

                    }

                }

            }

        }

    }

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

            int time = 0;

            const char * text = "[Delete]";

            txSetColor (RGB (255, 225, 0));
            txSelectFont ("Arial", 100);

            txTextOut (wWidth / 2 - txGetTextExtentX (text) / 2, wHeight / 2 - txGetTextExtentY (text) / 2, text);

            while (true)

            {

                if (txMouseButtons () == 1)

                {

                    POINT pos = txMousePos ();

                    GameObject click = {pos.x, pos.y, 0, 0, 1, 0, false, NULL};

                    for (int n = 0; n < OBJECTS_MAX; n++)

                    {

                        if (!manager -> objects_[n]) continue;

                        if (collisionDetection (manager -> objects_[n], &click))

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

                manager -> drawObjects ();

                if (time < delay)

                {

                    txSetColor (RGB (255, 225, 0));
                    txSelectFont ("Arial", 100);

                    txTextOut (wWidth / 2 - txGetTextExtentX (text) / 2, wHeight / 2 - txGetTextExtentY (text) / 2, text);

                }

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

void GameObject::draw ()

{

    if (visible_)

    {

        txSetColor (color_);
        txSetFillColor (color_);
        txCircle (x_, y_, r_);

    }

}

//-----------------------------------------------------------------------------

void GameObject::move ()

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

void GameObject::hit (GameObject * object)

{

}

//-----------------------------------------------------------------------------

void GameObject::control ()

{

}

//-----------------------------------------------------------------------------

void GameObject::remove ()

{

    manager_ -> removeObject (this);

}

//-----------------------------------------------------------------------------

void Animation::draw (double x, double y)

{

    txAlphaBlend (txDC (), x, y, width_, height_, spritesheet_, width_ * frame_, 0, 1);

    frame_ ++;

    if (frame_ >= frame_n_) frame_ = 0;

}

//-----------------------------------------------------------------------------

void Tank::draw ()

{
    COLORREF subcolor = addColor (color_, 0, -50, 0);

    txSetColor (color_, 7);
    txSetFillColor (color_);
    txLine (x_, y_, x_ + gun_vx * gun_length, y_ + gun_vy * gun_length);

    txSetColor (subcolor);
    txSetFillColor (subcolor);
    txCircle (x_, y_, 20);

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

    /*

    if (GetAsyncKeyState ('W'))

    {

        if (vy_ > -5)

        {

            vy_ -= 0.1;

        }

    }

    if (GetAsyncKeyState ('A'))

    {

        if (vx_ > -5)

        {

            vx_ -= 0.1;

        }

    }

    if (GetAsyncKeyState ('S'))

    {

        if (vy_ < 5)

        {

            vy_ += 0.1;

        }

    }

    if (GetAsyncKeyState ('D'))

    {

        if (vx_ < 5)

        {

            vx_ += 0.1;

        }

    }

    if (!GetAsyncKeyState ('W') && !GetAsyncKeyState ('A') && !GetAsyncKeyState ('S') && !GetAsyncKeyState ('D'))

    {

        if (vy_ < 0)

        {

            vy_ += 0.1;

        }

        else if (vy_ > 0)

        {

            vy_ -= 0.1;

        }

        if (vy_ > 0 && vy_ < 0.1 || vy_ < 0 && vy_ > -0.1)

        {

            vy_ = 0;

        }

        if (vx_ < 0)

        {

            vx_ += 0.1;

        }

        else if (vx_ > 0)

        {

            vx_ -= 0.1;

        }

        if (vx_ > 0 && vx_ < 0.1 || vx_ < 0 && vx_ > -0.1)

        {

            vx_ = 0;

        }

    }

    */

    if (txMouseButtons () == 1 || GetAsyncKeyState (VK_SPACE))

    {

        if (reloading_ >= 300)

        {

            int amount1 = manager_ -> objectsAmount ();

            Bullet * bullet = new Bullet {x_ + gun_vx * gun_length, y_ + gun_vy * gun_length, gun_vx + rnd (-0.1, 0.1),
                                          gun_vy + rnd (-0.1, 0.1), TX_WHITE, rnd (DAMAGE_MIN, DAMAGE_MAX), rnd (3, 4), rnd (-5, 5), true, manager_};

            int result = manager_ -> addObject (bullet);

            int amount2 = manager_ -> objectsAmount ();

            assert (amount2 > amount1);

            reloading_ = 0;

        }

    }

    else

    {

        reloading_ = 300;

    }

    if (reloading_ < 300)

    {

        reloading_ += 50;

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

void Tank::hit (GameObject * object)

{

    if (checkType <EnemyBullet> (object))

    {

        EnemyBullet * bullet = checkType <EnemyBullet> (object);

        addHealth (-25);

        bullet -> remove ();

    }

    else if (checkType <Food> (object))

    {

        Food * food = checkType <Food> (object);

        addHealth (rnd (10, 20));

        txPlaySound ("Resources\\Sound\\Food_Take.wav");

        food -> remove ();

    }

    else if (checkType <Medkit> (object))

    {

        Medkit * medkit = checkType <Medkit> (object);

        addHealth (100);

        txPlaySound ("Resources\\Sound\\Food_Take.wav");

        medkit -> remove ();

    }

    else if (checkType <Bullet> (object))

    {

        Bullet * bullet = checkType <Bullet> (object);

        addHealth(-rnd (2, 6));

        bullet -> remove ();

    }

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

    txSetColor (color_);
    txSetFillColor (color_);
    txCircle (x_, y_, 20);

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

void Enemy::setPosition (int x, int y)

{

    if (x <= wWidth - 20 && x > 300 + 20 && y <= wHeight - 40 && y > 20)

    {

        x_ = x;
        y_ = y;

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

        if (floor (rnd (1, FOOD_SPAWNING_FREQ)) == 1)

        {

            if (floor (rnd (1, FOOD_SPAWNING_FREQ)) == 1) manager_ -> addObject (new Medkit {x_, y_});

            else manager_ -> addObject (new Food {x_, y_});

            txPlaySound ("Resources\\Sound\\Food_Spawn.wav");

        }

        Reset ();

    }

}

//-----------------------------------------------------------------------------

void Enemy::Reset ()

{

    x_ = rnd (50, wWidth - 50);
    y_ = rnd (50, wHeight - 50);

    health_ = 100;

}

//-----------------------------------------------------------------------------

void Enemy::hit (GameObject * object)

{

    if (checkType <Bullet> (object))

    {

        Bullet * bullet = checkType <Bullet> (object);

        health_ -= bullet -> damage_;

        x_ += bullet -> vx_ * 7;
        y_ += bullet -> vy_ * 7;

        bullet -> remove ();

    }

    /*else if (checkType <EnemyBullet> (object))

    {

        EnemyBullet * bullet = checkType <EnemyBullet> (object);

        health_ -= 25;

        bullet -> remove ();

    }*/

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

            if (floor (rnd (1, 20)) == 1)

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

            }

            else

            {

                remove ();

            }

        }

    }

}

//-----------------------------------------------------------------------------

void Bullet::hit (GameObject * object)

{

    if (checkType <Tank> (object))

    {

        Tank * tank = checkType <Tank> (object);

        tank -> addHealth(-rnd (2, 6));

        remove ();

    }

    else if (checkType <Enemy> (object))

    {

        Enemy * enemy = checkType <Enemy> (object);

        enemy -> health_ -= damage_;

        enemy -> x_ += vx_ * 7;
        enemy -> y_ += vy_ * 7;

        remove ();

    }

    else if (checkType <Food> (object))

    {

        Food * food = checkType <Food> (object);

        food -> x_ += vx_ * 7;
        food -> y_ += vy_ * 7;

        vx_ = -vx_;
        vy_ = -vy_;

    }

    else if (checkType <Medkit> (object))

    {

        Medkit * medkit = checkType <Medkit> (object);

        medkit -> x_ += vx_ * 7;
        medkit -> y_ += vy_ * 7;

        vx_ = -vx_;
        vy_ = -vy_;

    }

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

void EnemyBullet::hit (GameObject * object)

{

    if (checkType <Tank> (object))

    {

        Tank * tank = checkType <Tank> (object);

        tank -> addHealth (-25);

        remove ();

    }

    /*else if (checkType <Enemy> (object))

    {

        Enemy * enemy = checkType <Enemy> (object);

        enemy -> health_ -= 25;

        remove ();

    }*/

}

//-----------------------------------------------------------------------------

void Food::draw ()

{

    txTransparentBlt (txDC (), x_ - txGetExtentX (image_) / 2, y_ - txGetExtentY (image_) / 2, 0, 0, image_, 0, 0, TX_WHITE);

}

//-----------------------------------------------------------------------------

void Food::hit (GameObject * object)

{

    if (checkType <Bullet> (object))

    {

        Bullet * bullet = checkType <Bullet> (object);

        x_ += bullet -> vx_ * 7;
        y_ += bullet -> vy_ * 7;

        bullet -> vx_ = -bullet -> vx_;
        bullet -> vy_ = -bullet -> vy_;

    }

    else if (checkType <Tank> (object))

    {

        Tank * tank = checkType <Tank> (object);

        tank -> addHealth (rnd (10, 20));

        txPlaySound ("Resources\\Sound\\Food_Take.wav");

        remove ();

    }

}

//-----------------------------------------------------------------------------

void Food::remove ()

{

    txDeleteDC (image_);

    manager_ -> removeObject (this);

}

//-----------------------------------------------------------------------------

void Medkit::draw ()

{

    txTransparentBlt (txDC (), x_ - txGetExtentX (image_) / 2, y_ - txGetExtentY (image_) / 2, 0, 0, image_, 0, 0, RGB (255, 0, 255));

}

//-----------------------------------------------------------------------------

void Medkit::hit (GameObject * object)

{

    if (checkType <Bullet> (object))

    {

        Bullet * bullet = checkType <Bullet> (object);

        x_ += bullet -> vx_ * 7;
        y_ += bullet -> vy_ * 7;

        bullet -> vx_ = -bullet -> vx_;
        bullet -> vy_ = -bullet -> vy_;

    }

    else if (checkType <Tank> (object))

    {

        Tank * tank = checkType <Tank> (object);

        tank -> addHealth (100);

        txPlaySound ("Resources\\Sound\\Food_Take.wav");

        remove ();

    }

}

//-----------------------------------------------------------------------------

void Medkit::remove ()

{

    txDeleteDC (image_);

    manager_ -> removeObject (this);

}

//-----------------------------------------------------------------------------

void Wall::draw ()

{

    txTransparentBlt (txDC (), x_, y_, 0, 0, image_);

}

//-----------------------------------------------------------------------------

void Wall::remove ()

{

    txDeleteDC (image_);

    manager_ -> removeObject (this);

}

//-----------------------------------------------------------------------------

COLORREF addColor (COLORREF color, int r, int g, int b)

{

    return RGB (txExtractColor (color, TX_RED) + r, txExtractColor (color, TX_GREEN) + g, txExtractColor (color, TX_BLUE) + b);

}

//-----------------------------------------------------------------------------

void drawScore (int score)

{

    txSetColor (TX_WHITE);
    txSelectFont ("Arial", 30);

    char text[100] = "";

    sprintf (text, "Score: %d", score);

    txTextOut (5, 5, text);

}

//-----------------------------------------------------------------------------

int gameOver (int score)

{

    Button buttons[] = {

        {wWidth / 2 - 125, wHeight / 2 - 25, 100, 30, "Restart", "Arial", 30, txDC (), RGB (60, 60, 60), TX_WHITE, TX_WHITE, false, 2},
        {wWidth / 2 +  25, wHeight / 2 - 25, 100, 30,    "Exit", "Arial", 30, txDC (), RGB (60, 60, 60), TX_WHITE, TX_WHITE, false, 2},
        EndButton

    };

    HDC image = txLoadImage ("Resources\\Images\\Die.bmp");
    double width = txGetExtentX (image);
    double height = txGetExtentY (image);

    for (double alpha = 0; alpha < 1; alpha += 0.01)

    {

        txSetFillColor (TX_BLACK);
        txClear ();

        txAlphaBlend (txDC (), wWidth / 2 - width / 2, wHeight / 2 - height / 2, 0, 0, image, 0, 0, alpha);

        txSleep (5);

    }

    while (!GetAsyncKeyState (VK_ESCAPE))

    {

        txSetFillColor (TX_BLACK);
        txClear ();

        txAlphaBlend (txDC (), wWidth / 2 - width / 2, wHeight / 2 - height / 2, 0, 0, image);

        const char * text = "Game over";

        txSelectFont ("Segoe Script", 50);
        txSetColor (TX_WHITE);
        txTextOut (wWidth / 2 - txGetTextExtentX (text) / 2, wHeight / 2 - txGetTextExtentY (text) / 2 - 100, text);

        char scoreText[100] = "";

        sprintf (scoreText, "Score: %d", score);

        txSelectFont ("Arial", 30);
        txTextOut (wWidth / 2 - txGetTextExtentX (scoreText)/ 2, wHeight / 2 - txGetTextExtentY (scoreText) / 2 - 70, scoreText);

        int result = manageButtons (buttons);

        if (result != -1)

        {

            txDeleteDC (image);

            return result;

        }

        txSleep (1);

    }

    txDeleteDC (image);

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

void info (const char * text)

{

    if (DEBUG_MODE)

    {

        printf (text);

    }

}

//-----------------------------------------------------------------------------

void title (int time)

{

    char text[100] = "";

    char symbol = ' ';

    if (time < 100 && time >= 0)

    {

        symbol = '|';

    }

    if (time < 200 && time >= 100)

    {

        symbol = '/';

    }

    if (time < 300 && time >= 200)

    {

        symbol = '-';

    }

    if (time < 400 && time >= 300)

    {

        symbol = '\\';

    }

    sprintf (text, "%ctank%c", symbol, symbol);

    SetWindowText (txWindow (), text);

}

//-----------------------------------------------------------------------------

bool collisionDetection (const GameObject * obj1, const GameObject * obj2)

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

    {

        txSetFillColor (bkcolor_);
        txClear ();

    }

    for (int n = 0; n < OBJECTS_MAX; n++)

    {

        if (!objects_[n]) continue;

        objects_[n] -> draw ();

    }

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

int ObjectManager::addObject (GameObject * object)

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

int ObjectManager::removeObject (GameObject * object)

{

    for (int n = 0; n < OBJECTS_MAX; n++)

    {

        if (objects_[n] == object)

        {

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

T * checkType (GameObject * object)

{

    T * ptr = dynamic_cast <T*> (object);

    if (ptr) return ptr;

    return nullptr;

}

//-----------------------------------------------------------------------------

HDC CaptureScreen (int x, int y, int width, int height)

{

    int x1 = x + width, y1 = y + height;

    HDC ScreenDC = GetDC (NULL);

    HDC dc = txCreateCompatibleDC (width, height);

    txBitBlt (dc, 0, 0, width, height, ScreenDC, x, y);

    DeleteDC (ScreenDC);

    return dc;

}

//-----------------------------------------------------------------------------

void drawFragment (int x, int y, HDC * image, int n, double width, double height, double alpha)

{

    txAlphaBlend (txDC (), x, y, width, height, *image, width * n, 0, alpha);

}

//-----------------------------------------------------------------------------
