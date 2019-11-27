#include <TXLib.h>
#include "Buttons.h"

//-----------------------------------------------------------------------------

enum GameOverResult

{

    ResultRestart,
    ResultExit

};

//-----------------------------------------------------------------------------

struct Tank

{

    double x_;
    double y_;

    double vy_;

    int gun_length;
    double gun_vx;
    double gun_vy;

    COLORREF color_;

    int health_;

    void draw ();

    void move (int side);

    void check ();

};

struct Enemy

{

    int x_;
    int y_;

    int health_;

    COLORREF color_;

    bool moving_;

    double moving_vx;
    double moving_vy;

    int moving_length;

    int moving_time;

    void draw ();

    void move ();

    void setPosition (int x, int y);

};

struct Bullet

{

    double x_;
    double y_;

    double vx_;
    double vy_;

    int damage_;

    double speed_;

    double deflection_;

    COLORREF color_;

    bool visible_;

    void draw ();

    void move ();

};

struct EnemyBullet

{

    double x_;
    double y_;

    int damage_;

    int speed_;

    double deflection_;

    COLORREF color_;

    bool visible_;

    void draw ();

    void move ();

};

//-----------------------------------------------------------------------------

COLORREF addColor (COLORREF color, int r, int g, int b);

void drawEnemies (Enemy * enemies);

void drawBullets (Bullet * bullets, EnemyBullet * enemyBulelts);

void moveBullets (Bullet * bullets, EnemyBullet * enemyBullets);

void addBullet (Bullet * bullets, Bullet bullet);

void checkBullets (Bullet * bullets, Enemy * enemies, Tank * tank, int * score);

void checkEnemyBullets (EnemyBullet * enemyBullets, Tank * tank);

int checkBullet (Bullet bullet, Enemy enemy, Tank tank);

bool checkEnemyBullet (EnemyBullet bullet, Tank tank);

void addScore (int * score);

void drawScore (int score);

void moveEnemies (Enemy * enemies);

void addEnemyBullet (EnemyBullet * enemyBullets, EnemyBullet enemyBullet);

void enemyShoot (EnemyBullet * enemyBullets, Enemy * enemies);

int gameOver (int score);

//-----------------------------------------------------------------------------

int main ()

{

    txCreateWindow (800, 800);
    txDisableAutoPause ();

    while (!GetAsyncKeyState (VK_ESCAPE))

    {

        COLORREF bkcolor = RGB (45, 45, 45);

        int speed = 4;
        int enemy_n = 5;

        int damage_min =  5;
        int damage_max = 15;

        int score = 0;

        Tank tank = {70, 380, 0, 40, NULL, NULL, TX_GREEN, 100};

        Enemy enemies[6] = {};

        for (int n = 0; n < enemy_n; n++)

        {

            enemies[n] = {random (300, 750), random (50, 750), 100, TX_RED, false};

        }

        enemies[5] = {};

        Bullet bullets[100] = {};
        EnemyBullet enemyBullets[100] = {};

        for (int n = 0; n < 100; n++)

        {

            bullets[n].visible_ = false;

            enemyBullets[n].visible_ = false;

        }

        int space_pressed = 300;

        while (!GetAsyncKeyState (VK_ESCAPE))

        {

            txSetFillColor (bkcolor);
            txClear ();
            txClearConsole ();

            if (GetAsyncKeyState ('W'))

            {

                if (tank.y_ - speed >= 0)

                {

                    tank.move (0);

                }

            }

            else if (GetAsyncKeyState ('S'))

            {

                if (tank.y_ + speed <= 800 - 40)

                {

                    tank.move (1);

                }

            }

            else tank.move (-1);

            if (txMouseButtons () == 1 || GetAsyncKeyState (VK_SPACE))

            {

                if (space_pressed >= 300)

                {

                    Bullet bullet = {tank.x_ + 40 + tank.gun_vx * tank.gun_length, tank.y_ + 20 + tank.gun_vy * tank.gun_length, tank.gun_vx + random (-0.1, 0.1), tank.gun_vy + random (-0.1, 0.1), random (damage_min, damage_max), random (3, 5), random (-5, 5), TX_WHITE, true};

                    addBullet (bullets, bullet);

                    space_pressed = 0;

                }

            }

            else

            {

                space_pressed = 300;

            }

            if (space_pressed < 300)

            {

                space_pressed += 15;

            }

            moveBullets (bullets, enemyBullets);

            moveEnemies (enemies);

            enemyShoot (enemyBullets, enemies);

            checkBullets (bullets, enemies, &tank, &score);

            checkEnemyBullets (enemyBullets, &tank);

            tank.check ();

            if (tank.health_ <= 0)

            {

                int result = gameOver (score);

                if (result == ResultRestart)

                {

                    break;

                }

                if (result == ResultExit)

                {

                    return 0;

                }

            }

            tank.draw ();

            drawEnemies (enemies);
            drawBullets (bullets, enemyBullets);

            drawScore (score);

            txSleep (1);

        }

    }

    return 0;

}

//-----------------------------------------------------------------------------

void Tank::draw ()

{

    txSetColor (color_);
    txSetFillColor (color_);
    txRectangle (x_, y_, x_ + 60, y_ + 40);

    COLORREF subcolor = addColor (color_, 0, -50, 0);

    txSetColor (subcolor);
    txSetFillColor (subcolor);
    txCircle (x_ + 60 - 20, y_ + 20, 15);

    txSetColor (subcolor, 5);
    txLine (x_ + 40, y_ + 20, x_ + 40 + gun_vx * gun_length, y_ + 20 + gun_vy * gun_length);

    int x = x_ + 10;
    int y = y_ + 50;
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

void Tank::move (int side)

{

    if (y_ + vy_ > 0 && y_ + vy_ < 760)

    {

        y_ += vy_;

    }

    else

    {

        vy_ = -vy_;

    }

    if (side == 1)

    {

        if (vy_ <= 5)

        {

            vy_ += 0.1;

        }

    }

    else if (side == 0)

    {

        if (vy_ >= -5)

        {

            vy_ -= 0.1;

        }

    }

    else

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

    }

}

//-----------------------------------------------------------------------------

void Tank::check ()

{

    POINT mPos = txMousePos ();

    double a = mPos.x - (x_ + 40);
    double b = mPos.y - (y_ + 20);

    double c = sqrt (a * a + b * b);

    gun_vx = a / c;
    gun_vy = b / c;

}

//-----------------------------------------------------------------------------

void Enemy::draw ()

{

    if (health_ > 0)

    {

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

}

//-----------------------------------------------------------------------------

void Bullet::draw ()

{

    if (visible_)

    {

        txSetColor (color_);
        txSetFillColor (color_);
        txCircle (x_, y_, 3);

    }

}

//-----------------------------------------------------------------------------

void Bullet::move ()

{

    if (visible_)

    {

        x_ += vx_ * speed_;
        y_ += vy_ * speed_;

        if (x_ > 800 || x_ < 0 || y_ > 800 || y_ < 0)

        {

            if (floor (random (1, 20)) == 1)

            {

                 vx_ = -vx_;
                 vy_ = -vy_;

                 color_ = RGB (255, 0, 0);

            }

            else visible_ = false;

        }

        draw ();

    }

}

//-----------------------------------------------------------------------------

COLORREF addColor (COLORREF color, int r, int g, int b)

{

    return RGB (txExtractColor (color, TX_RED) + r, txExtractColor (color, TX_GREEN) + g, txExtractColor (color, TX_BLUE) + b);

}

//-----------------------------------------------------------------------------

void drawEnemies (Enemy * enemies)

{

    for (int n = 0; enemies[n].x_; n++)

    {

        enemies[n].draw ();

    }

}

//-----------------------------------------------------------------------------

void drawBullets (Bullet * bullets, EnemyBullet * enemyBullets)

{

    for (int n = 0; n < 100; n++)

    {

        bullets[n].draw ();
        enemyBullets[n].draw ();

    }

}

//-----------------------------------------------------------------------------

void moveBullets (Bullet * bullets, EnemyBullet * enemyBullets)

{

    for (int n = 0; n < 100; n++)

    {

        bullets[n].move ();
        enemyBullets[n].move ();

    }

}

//-----------------------------------------------------------------------------

void addBullet (Bullet * bullets, Bullet bullet)

{

    for (int n = 0; n < 100; n++)

    {

        if (!bullets[n].visible_)

        {

            bullets[n] = bullet;

            return;

        }

    }

    bullets[0] = bullet;

}

//-----------------------------------------------------------------------------

void addEnemyBullet (EnemyBullet * enemyBullets, EnemyBullet enemyBullet)

{

    for (int n = 0; n < 100; n++)

    {

        if (!enemyBullets[n].visible_)

        {

            enemyBullets[n] = enemyBullet;

            return;

        }

    }

    enemyBullets[0] = enemyBullet;

}

//-----------------------------------------------------------------------------

void checkBullets (Bullet * bullets, Enemy * enemies, Tank * tank, int * score)

{

    for (int n = 0; n < 100; n++)

    {

        if (bullets[n].visible_)

        {

            for (int b = 0; enemies[b].x_; b++)

            {

                if (enemies[b].health_ > 0)

                {

                    int result = checkBullet (bullets[n], enemies[b], * tank);

                    if (result == 1)

                    {

                        enemies[b].health_ -= bullets[n].damage_;

                        enemies[b].setPosition (enemies[b].x_ + 7, enemies[b].y_);

                        if (enemies[b].health_ <= 0)

                        {

                            addScore (score);

                            enemies[b] = {random (300, 750), random (50, 750), 100, TX_RED};

                        }

                        bullets[n].visible_ = false;

                        break;

                    }

                    if (result == 2)

                    {

                        tank -> health_ = 0;

                    }

                }

            }

        }

    }

}

//-----------------------------------------------------------------------------

int checkBullet (Bullet bullet, Enemy enemy, Tank tank)

{

    int a = bullet.x_ - enemy.x_;
    int b = bullet.y_ - enemy.y_;

    int c = sqrt (a * a + b * b);

    if (c <= 23)

    {

        return 1;

    }

    if (bullet.x_ - 3 <= tank.x_ + 60 && bullet.x_ + 3 > tank.x_ && bullet.y_ - 3 <= tank.y_ + 40 && bullet.y_ + 3 > tank.y_)

    {

        return 2;

    }

    return 0;

}

//-----------------------------------------------------------------------------

void addScore (int * score)

{

    *score += 1;

    txPlaySound ("addScore.wav");

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

void Enemy::move ()

{

    if (!moving_)

    {

        if (floor (random (1, 300)) == 1)

        {

            moving_ = true;
            moving_time = 0;

            moving_vx = random (-1, 1) / 20;
            moving_vy = random (-1, 1) / 20;
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

void moveEnemies (Enemy * enemies)

{

    for (int n = 0; enemies[n].x_; n++)

    {

        enemies[n].move ();

    }

}

//-----------------------------------------------------------------------------

void Enemy::setPosition (int x, int y)

{

    if (x <= 800 - 20 && x > 300 + 20 && y <= 800 - 40 && y > 20)

    {

        x_ = x;
        y_ = y;

    }

}

//-----------------------------------------------------------------------------

void EnemyBullet::draw ()

{

    if (visible_)

    {

        txSetColor (color_);
        txSetFillColor (color_);
        txCircle (x_, y_, 3);

    }

}

//-----------------------------------------------------------------------------

void EnemyBullet::move ()

{

    if (visible_)

    {

        x_ -= speed_;
        y_ += deflection_ / 20;

        if (x_ <= 0)

        {

            visible_ = false;

        }

        draw ();

    }

}

//-----------------------------------------------------------------------------

void enemyShoot (EnemyBullet * enemyBullets, Enemy * enemies)

{

    for (int n = 0; enemies[n].x_; n++)

    {

        if (floor (random (1, 200)) == 1)

        {

            EnemyBullet enemyBullet = {enemies[n].x_ - 20, enemies[n].y_, random (5, 15), random (3, 5), random (-5, 5), RGB (0, 179, 255), true};

            addEnemyBullet (enemyBullets, enemyBullet);

        }

    }

}

//-----------------------------------------------------------------------------

bool checkEnemyBullet (EnemyBullet enemyBullet, Tank tank)

{

    if (enemyBullet.x_ - 3 <= tank.x_ + 60 && enemyBullet.x_ + 3 > tank.x_ && enemyBullet.y_ - 3 <= tank.y_ + 40 && enemyBullet.y_ + 3 > tank.y_)

    {

        return true;

    }

    return false;

}

//-----------------------------------------------------------------------------

void checkEnemyBullets (EnemyBullet * enemyBullets, Tank * tank)

{

    for (int n = 0; enemyBullets[n].x_; n++)

    {

        if (enemyBullets[n].visible_)

        {

            if (checkEnemyBullet (enemyBullets[n], * tank))

            {

                tank -> health_ -= 25;

                enemyBullets[n].visible_ = false;

            }

        }

    }

}

//-----------------------------------------------------------------------------

int gameOver (int score)

{

    Button buttons[] = {

        {275, 385, 100, 30, "Restart", "Arial", 30, txDC (), RGB (60, 60, 60), TX_WHITE, TX_WHITE, false, 2},
        {425, 385, 100, 30,    "Exit", "Arial", 30, txDC (), RGB (60, 60, 60), TX_WHITE, TX_WHITE, false, 2},
        EndButton

    };

    while (!GetAsyncKeyState (VK_ESCAPE))

    {

        txSetFillColor (RGB (45, 45, 45));
        txClear ();

        const char * text = "Game over";

        txSelectFont ("Segoe Script", 50);
        txSetColor (TX_WHITE);
        txTextOut (400 - txGetTextExtentX (text) / 2, 400 - txGetTextExtentY (text) / 2 - 100, text);

        char scoreText[100] = "";

        sprintf (scoreText, "Score: %d", score);

        txSelectFont ("Arial", 30);
        txTextOut (400 - txGetTextExtentX (scoreText)/ 2, 400 - txGetTextExtentY (scoreText) / 2 - 70, scoreText);

        int result = manageButtons (buttons);

        if (result != -1) return result;

        txSleep (1);

    }

    return ResultExit;

}
