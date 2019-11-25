#include <TXLib.h>

//-----------------------------------------------------------------------------

struct Tank

{

    int x_;
    int y_;

    COLORREF color_;

    void draw ();

};

struct Enemy

{

    int x_;
    int y_;

    int health_;

    COLORREF color_;

    void draw ();

};

struct Bullet

{

    int x_;
    int y_;

    int damage_;

    int speed_;

    COLORREF color_;

    bool visible_;

    void draw ();

    void move ();

};

//-----------------------------------------------------------------------------

COLORREF addColor (COLORREF color, int r, int g, int b);

void drawEnemies (Enemy * enemies);

void drawBullets (Bullet * bullets);

void moveBullets (Bullet * bullets);

void addBullet (Bullet * bullets, Bullet bullet);

void checkBullets (Bullet * bullets, Enemy * enemies, int damage, int * score);

bool checkBullet (Bullet bullet, Enemy enemy);

void addScore (int * score);

void drawScore (int score);

//-----------------------------------------------------------------------------

int main ()

{

    COLORREF bkcolor = RGB (45, 45, 45);

    int speed = 4;
    int enemy_n = 3;
    int damage = 12;
    int bullet_speed = 3;

    txCreateWindow (800, 800);
    txDisableAutoPause ();

    int score = 0;

    Tank tank = {70, 380, TX_GREEN};

    Enemy enemies[4] = {};

    for (int n = 0; n < enemy_n; n++)

    {

        enemies[n] = {random (300, 750), random (50, 750), 100, TX_RED};

    }

    enemies[3] = {};

    Bullet bullets[100] = {};

    for (int n = 0; n < 100; n++)

    {

        bullets[n] = {tank.x_ + 80, tank.y_ + 20, damage, bullet_speed, TX_WHITE, false};

    }

    int space_pressed = 300;

    while (!GetAsyncKeyState (VK_ESCAPE))

    {

        txSetFillColor (bkcolor);
        txClear ();

        if (GetAsyncKeyState ('W'))

        {

            tank.y_ -= speed;

        }

        if (GetAsyncKeyState ('S'))

        {

            tank.y_ += speed;

        }

        if (GetAsyncKeyState (VK_SPACE))

        {

            if (space_pressed >= 300)

            {

                Bullet bullet = {tank.x_ + 80, tank.y_ + 20, damage, bullet_speed, TX_WHITE, true};

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

        tank.draw ();

        drawEnemies (enemies);
        drawBullets (bullets);

        drawScore (score);

        moveBullets (bullets);

        checkBullets (bullets, enemies, damage, &score);

        txSleep (1);

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
    txLine (x_ + 60 - 20, y_ + 20, x_ + 80, y_ + 20);

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

        x_ += speed_;

        if (x_ > 800)

        {

            visible_ = false;

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

void drawBullets (Bullet * bullets)

{

    for (int n = 0; n < 100; n++)

    {

        bullets[n].draw ();

    }

}

//-----------------------------------------------------------------------------

void moveBullets (Bullet * bullets)

{

    for (int n = 0; n < 100; n++)

    {

        bullets[n].move ();

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

void checkBullets (Bullet * bullets, Enemy * enemies, int damage, int * score)

{

    for (int n = 0; n < 100; n++)

    {

        if (bullets[n].visible_)

        {

            for (int b = 0; enemies[b].x_; b++)

            {

                if (enemies[b].health_ > 0)

                {

                    if (checkBullet (bullets[n], enemies[b]))

                    {

                        enemies[b].health_ -= random (5, damage);

                        if (enemies[b].health_ <= 0)

                        {

                            addScore (score);

                            enemies[b] = {random (300, 750), random (50, 750), 100, TX_RED};

                        }

                        bullets[n].visible_ = false;

                        break;

                    }

                }

            }

        }

    }

}

//-----------------------------------------------------------------------------

bool checkBullet (Bullet bullet, Enemy enemy)

{

    if (bullet.y_ - enemy.y_ < 23 && bullet.y_ - enemy.y_ > -23 && bullet.x_ - enemy.x_ < 23 && bullet.x_ - enemy.x_ > -23)

    {

        return true;

    }

    return false;

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
