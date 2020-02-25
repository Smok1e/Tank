#include <TXLib.h>

//-----------------------------------------------------------------------------

const int WINDOWS_MAX = 300;

//-----------------------------------------------------------------------------

struct AbstractWindow;

struct ContainerWindow;

//-----------------------------------------------------------------------------

struct WindowManager

{

    AbstractWindow * windows_[WINDOWS_MAX];

    WindowManager ();

    ~WindowManager ();

    int addWindow (AbstractWindow * window);

    void drawWindows ();

    void updateWindows ();

    int windowsAmount ();

    AbstractWindow * getWindow (int id);

    void run ();

};

WindowManager::WindowManager () :

    windows_ ({})

{}

WindowManager::~WindowManager ()

{

    for (int n = 0; n < WINDOWS_MAX; n++)

    {

        if (!windows_[n]) continue;

        delete (windows_[n]);

        windows_[n] = nullptr;

    }

}

//-----------------------------------------------------------------------------

struct AbstractWindow

{

    double x_;
    double y_;

    double width_;
    double height_;

    COLORREF color_;

    ContainerWindow * parent_;

    bool visible_;

    int id_;

    AbstractWindow (double x, double y, double width, double height, COLORREF color);

    ContainerWindow * getParent ();

    virtual void draw ();

    virtual void update ();

    virtual bool onMouseTest ();

    virtual void onMouseClick ();

    virtual void run ();

    virtual void setId (int id);

    virtual int getId ();

};

AbstractWindow::AbstractWindow (double x, double y, double width, double height, COLORREF color) :

    x_         (x),
    y_         (y),
    width_     (width),
    height_    (height),
    color_     (color),

    isPressed_ (false),

    visible_   (true),

    id_        (-1)

{}

//-----------------------------------------------------------------------------

struct ContainerWindow : AbstractWindow

{

    WindowManager manager_;

    ContainerWindow (double x, double y, double width, double height, COLORREF color);

    virtual int addWindow (AbstractWindow * wnd);

    virtual int windowsAmount ();

    virtual void run () override;

    AbstractWindow * getWindow (int id);

};

ContainerWindow::ContainerWindow (double x, double y, double width, double height, COLORREF color) :

    AbstractWindow (x, y, width, height, color),

    manager_ ({})

{}

//-----------------------------------------------------------------------------

struct AbstractButton : AbstractWindow

{

    void (*action_) (AbstractWindow * wnd);

    bool isPressed_;

    AbstractButton (double x, double y, double width, double height, COLORREF color, void (*action_));

    virtual void onMouseClck ();

};

//-----------------------------------------------------------------------------

struct PictureBox : AbstractWindow

{

    HDC image_;

    PictureBox (double x, double y, HDC image);

    ~PictureBox ();

    virtual void draw () override;

    virtual bool onMouseTest () override;

};

PictureBox::PictureBox (double x, double y, HDC image) :

    AbstractWindow (x, y, 0, 0, 0),

    image_ (image)

{

    width_ = txGetExtentX  (image_);
    height_ = txGetExtentY (image_);

}

PictureBox::~PictureBox ()

{

    txDeleteDC (image_);

}

//-----------------------------------------------------------------------------

struct TextBox : AbstractWindow

{

    string text_;
    const char * font_;

    double fontSize_;

    COLORREF textColor_;

    TextBox (double x, double y, double width, double height, COLORREF color, const char * text, const char * font, double fontSize, COLORREF textColor, void (*action) (AbstractWindow * wnd) = NULL);

    virtual void draw () override;

};

TextBox::TextBox (double x, double y, double width, double height, COLORREF color, const char * text, const char * font, double fontSize, COLORREF textColor, void (*action) (AbstractWindow * wnd)) :

    AbstractWindow (x, y, width, height, color, action),

    text_      (text),
    font_      (font),

    fontSize_  (fontSize),

    textColor_ (textColor)

{}

//-----------------------------------------------------------------------------

struct Button : TextBox

{

    COLORREF strokeColor_;

    bool stroke_;

    Button (double x, double y, double width, double height, COLORREF color, const char * text, const char * font, double fontSize, COLORREF textColor, COLORREF strokeColor, void (*action) (AbstractWindow * wnd));

    virtual void draw () override;

    virtual void update () override;

};

Button::Button (double x, double y, double width, double height, COLORREF color, const char * text, const char * font, double fontSize, COLORREF textColor, COLORREF strokeColor, void (*action) (AbstractWindow * wnd)) :

    TextBox (x, y, width, height, color, text, font, fontSize, textColor, action),

    strokeColor_ (strokeColor)

{}

//-----------------------------------------------------------------------------

struct CircleButton : AbstractWindow

{

    double r_;

    COLORREF strokeColor_;

    bool stroke_;

    CircleButton (double x, double y, double r, COLORREF color, COLORREF strokeColor, void (*action) (AbstractWindow * wnd));

    virtual void draw () override;

    virtual void update () override;

    virtual bool onMouseTest () override;

};

CircleButton::CircleButton (double x, double y, double r, COLORREF color, COLORREF strokeColor, void (*action) (AbstractWindow * wnd)) :

    AbstractWindow (x, y, 0, 0, color, action),

    r_           (r),

    strokeColor_ (strokeColor),

    stroke_      (false)

{}

//-----------------------------------------------------------------------------

struct Label : TextBox

{

    Label (double x, double y, double width, double height, const char * text, const char * font, double fontSize, COLORREF textColor, void (*action) (AbstractWindow * wnd) = NULL);

    virtual void draw ();

};

Label::Label (double x, double y, double width, double height, const char * text, const char * font, double fontSize, COLORREF textColor, void (*action) (AbstractWindow * wnd)) :


    TextBox (x, y, width, height, 0, text, font, fontSize, textColor, action)

{}

//-----------------------------------------------------------------------------

struct WriteBox : TextBox

{

    COLORREF strokeColor_;

    bool isActive_;

    int counter_;

    WriteBox (double x, double y, double width, double height, COLORREF color, const char * text, const char * font, double fontSize, COLORREF textColor, COLORREF strokeColor, void (*action) (AbstractWindow * wnd) = NULL);

    virtual void draw ();

    virtual void update ();

    const char * getText ();

};

WriteBox::WriteBox (double x, double y, double width, double height, COLORREF color, const char * text, const char * font, double fontSize, COLORREF textColor, COLORREF strokeColor, void (*action) (AbstractWindow * wnd)) :

    TextBox (x, y, width, height, color, text, font, fontSize, textColor, action),

    strokeColor_ (strokeColor),

    isActive_ (false),

    counter_ (0)

{}

//-----------------------------------------------------------------------------

void drawRectText (double x, double y, double width, double height, const char * text, const char * font, COLORREF textColor, HDC dc = NULL);

//-----------------------------------------------------------------------------

void AbstractWindow::draw ()

{

    txSetColor (color_);
    txSetFillColor (color_);
    txRectangle (x_, y_, x_ + width_, y_ + height_);

}

//-----------------------------------------------------------------------------

void AbstractWindow::update ()

{

}

//-----------------------------------------------------------------------------

bool AbstractWindow::onMouseTest ()

{

    POINT mPos = txMousePos ();

    if (mPos.x >= x_ && mPos.x < x_ + width_ && mPos.y >= y_ && mPos.y < y_ + height_) return true;

    return false;

}

//-----------------------------------------------------------------------------

void AbstractWindow::onMouseClick ()

{

}

//-----------------------------------------------------------------------------

void AbstractWindow::run ()

{

}

//-----------------------------------------------------------------------------

void AbstractWindow::setId (int id)

{

    id_ = id;

}

//-----------------------------------------------------------------------------

int AbstractWindow::getId ()

{

    return id_;

}

//-----------------------------------------------------------------------------

int ContainerWindow::addWindow (AbstractWindow * window)

{

    window -> parent_ = this;

    manager_.addWindow (window);

}

//-----------------------------------------------------------------------------

ContainerWindow * AbstractWindow::getParent ()

{

    return parent_;

}

//-----------------------------------------------------------------------------

int ContainerWindow::windowsAmount ()

{

    return manager_.windowsAmount ();

}

//-----------------------------------------------------------------------------

void ContainerWindow::run ()

{

    draw ();
    update ();
    onMouseClick ();

    manager_.run ();

}

//-----------------------------------------------------------------------------

AbstractWindow * ContainerWindow::getWindow (int id)

{

    return manager_.getWindow (id);

}

//-----------------------------------------------------------------------------

void AbstractButton::onMouseClick ()

{

    if (onMouseTest ())

    {

        if (!isPressed_)

        {

            action_ (this);

            isPressed_ = true;

        }

    }

    else isPressed_ = false;

}

//-----------------------------------------------------------------------------

void Button::draw ()

{

    if (stroke_)

    {

        txSetColor (strokeColor_);
        txSetFillColor (strokeColor_);
        txRectangle (x_ - 2, y_ - 2, x_ + width_ + 2, y_ + height_ + 2);

    }

    TextBox::draw ();

}

//-----------------------------------------------------------------------------

void Button::update ()

{

    if (onMouseTest ()) stroke_ = true;

    else stroke_ = false;

}

//-----------------------------------------------------------------------------

void PictureBox::draw ()

{

    if (hover_) txAlphaBlend (txDC (), x_, y_, 0, 0, hoverImage_);

    else txAlphaBlend (txDC (), x_, y_, 0, 0, image_);

}

//-----------------------------------------------------------------------------

bool PictureBox::onMouseTest ()

{

    if (!AbstractWindow::onMouseTest ())

    {

        hover_ = false;

        return false;

    }

    POINT mPos = txMousePos ();

    COLORREF pixel = txGetPixel (mPos.x - x_, mPos.y - y_, image_);

    if (pixel == 0xFFFFFF)

    {

        hover_ = false;

        return false;

    }

    hover_ = true;

    return true;

}

//-----------------------------------------------------------------------------

void TextBox::draw ()

{

    AbstractWindow::draw ();

    const char * text = text_.c_str();

    txSelectFont (font_, fontSize_);
    txSetColor (textColor_);
    txSetFillColor (textColor_);

    txDrawText (x_, y_, x_ + width_, y_ + height_, text);

}

//-----------------------------------------------------------------------------

void Label::draw ()

{

    txSelectFont (font_, fontSize_);

    txSetColor (textColor_);
    txSetFillColor (textColor_);

    txTextOut (x_, y_, text_.c_str());

}

//-----------------------------------------------------------------------------

void CircleButton::draw ()

{

    if (stroke_)

    {

        txSetColor (strokeColor_);
        txSetFillColor (strokeColor_);
        txCircle (x_, y_, r_ + 2);

    }

    txSetColor (color_);
    txSetFillColor (color_);
    txCircle (x_, y_, r_);

}

//-----------------------------------------------------------------------------

void CircleButton::update ()

{

    if (onMouseTest ()) stroke_ = true;

    else stroke_ = false;

}

//-----------------------------------------------------------------------------

bool CircleButton::onMouseTest ()

{

    POINT mPos = txMousePos ();

    double a = x_ - mPos.x;
    double b = y_ - mPos.y;

    double c = sqrt (a * a + b * b);

    if (c <= r_) return true;

    return false;

}

//-----------------------------------------------------------------------------

void WriteBox::draw ()

{

    if (isActive_)

    {

        txSetColor (strokeColor_);
        txSetFillColor (strokeColor_);
        txRectangle (x_ - 2, y_ - 2, x_ + width_ + 2, y_ + height_ + 2);

    }

    TextBox::draw ();

    const char * text = text_.c_str ();

    if (isActive_)

    {

        if (counter_ > 40)

        {

            if (txGetTextExtentX (text) < width_)

            {

                POINT line = {x_ + width_ / 2 + txGetTextExtentX (text) / 2, y_ + height_ / 2 - fontSize_ / 2};

                txSetColor (strokeColor_, 2);
                txSetFillColor (strokeColor_);

                txLine (line.x, line.y, line.x, line.y + fontSize_);

            }

        }

    }

}

//-----------------------------------------------------------------------------

void WriteBox::update ()

{

    if (kbhit ())

    {

        char key = getch ();

        if (key == 8)

        {

            if (!text_.empty ()) text_.erase (text_.end () - 1);

        }

        else if (isActive_) text_ += key;

    }

    if (txMouseButtons () == 1)

    {

        if (onMouseTest ()) isActive_ = true;
        else isActive_ = false;

    }

    counter_ ++;

    if (counter_ > 80) counter_ = 0;

}

//-----------------------------------------------------------------------------

const char * WriteBox::getText ()

{

    return text_.c_str ();

}

//-----------------------------------------------------------------------------

int WindowManager::addWindow (AbstractWindow * window)

{

    for (int n = 0; n < WINDOWS_MAX; n++)

    {

        if (!windows_[n])

        {

            windows_[n] = window;

            return n;

        }

    }

    TX_ERROR ("The array of windows is full.");

}

//-----------------------------------------------------------------------------

void WindowManager::drawWindows ()

{

    for (int n = 0; n < WINDOWS_MAX; n++)

    {

        if (!windows_[n] || !windows_[n] -> visible_) return;

        windows_[n] -> draw ();

    }

}

//-----------------------------------------------------------------------------

void WindowManager::updateWindows ()

{

    for (int n = 0; n < WINDOWS_MAX; n++)

    {

        if (!windows_[n] || !windows_[n] -> visible_) return;

        windows_[n] -> update ();
        windows_[n] -> onMouseClick ();

    }

}

//-----------------------------------------------------------------------------

int WindowManager::windowsAmount ()

{

    int amount = 0;

    for (int n = 0; n < WINDOWS_MAX; n++)

    {

        if (!windows_[n] || !windows_[n] -> visible_) continue;

        amount ++;

    }

    return amount;

}

//-----------------------------------------------------------------------------

AbstractWindow * WindowManager::getWindow (int id)

{

    for (int n = 0; n < WINDOWS_MAX; n++)

    {

        if (!windows_[n]) continue;

        if (windows_[n] -> getId () == id) return windows_[n];

    }

    return NULL;

}

//-----------------------------------------------------------------------------

void WindowManager::run ()

{

    drawWindows ();
    updateWindows ();

    for (int n = 0; n < WINDOWS_MAX; n++)

    {

        if (!windows_[n] || !windows_[n] -> visible_) continue;

        windows_[n] -> run ();

    }

}

