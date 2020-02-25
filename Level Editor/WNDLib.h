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

    virtual char onKeyDown ();

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

    AbstractButton (double x, double y, double width, double height, COLORREF color, void (*action) (AbstractWindow * wnd));

    virtual void onMouseClick ();

};

AbstractButton::AbstractButton (double x, double y, double width, double height, COLORREF color, void (*action) (AbstractWindow * wnd)) :

    AbstractWindow (x, y, width, height, color),

    action_    (action),

    isPressed_ (false)

{}

//-----------------------------------------------------------------------------

struct Button : AbstractButton

{

    COLORREF strokeColor_;

    Button (double x, double y, double width, double height, COLORREF color, COLORREF strokeColor, void (*action) (AbstractWindow * wnd));

    virtual void draw ();

};

Button::Button (double x, double y, double width, double height, COLORREF color, COLORREF strokeColor, void (*action) (AbstractWindow * wnd)) :

    AbstractButton (x, y, width, height, color, action),

    strokeColor_ (strokeColor)

{}

//-----------------------------------------------------------------------------

struct TextButton : Button

{

    const char * text_;
    const char * font_;

    double fontSize_;

    COLORREF textColor_;

    TextButton (double x, double y, double width, double height, COLORREF color, COLORREF strokeColor, const char * text, const char * font, double fontSize, COLORREF textColor, void (*action) (AbstractWindow * wnd));

    virtual void draw ();

};

TextButton::TextButton (double x, double y, double width, double height, COLORREF color, COLORREF strokeColor, const char * text, const char * font, double fontSize, COLORREF textColor, void (*action) (AbstractWindow * wnd)) :

    Button (x, y, width, height, color, strokeColor, action),

    text_      (text),
    font_      (font),

    fontSize_  (fontSize),

    textColor_ (textColor)

{}

//-----------------------------------------------------------------------------

struct ImageBox : AbstractWindow

{

    HDC image_;

    ImageBox (double x, double y, HDC image);

    ~ImageBox ();

    virtual void draw ();

    virtual bool onMouseTest ();

};

ImageBox::ImageBox (double x, double y, HDC image) :

    AbstractWindow (x, y, txGetExtentX (image), txGetExtentY (image), 0),

    image_ (image)

{}

ImageBox::~ImageBox ()

{

    txDeleteDC (image_);

}

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

char AbstractWindow::onKeyDown ()

{

    if (kbhit ())

    {

        return getch ();

    }

    return NULL;

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

    if (txMouseButtons () == 1)

    {

        if (onMouseTest ())

        {


            if (!isPressed_)

            {

                action_ (this);

                isPressed_ = true;

            }

        }

    }

    else isPressed_ = false;

}

//-----------------------------------------------------------------------------

void Button::draw ()

{

    if (onMouseTest ())

    {

        txSetColor (strokeColor_);
        txSetFillColor (strokeColor_);

        txRectangle (x_ - 2, y_ - 2, x_ + width_ + 2, y_ + height_ + 2);

    }

    AbstractButton::draw ();

}

//-----------------------------------------------------------------------------

void TextButton::draw ()

{

    Button::draw ();

    txSetColor (textColor_);
    txSetFillColor (textColor_);

    txSelectFont (font_, fontSize_);

    txDrawText (x_, y_, x_ + width_, y_ + height_, text_);

}

//-----------------------------------------------------------------------------

void ImageBox::draw ()

{

    txAlphaBlend (txDC (), x_, y_, width_, height_, image_);

}

//-----------------------------------------------------------------------------

bool ImageBox::onMouseTest ()

{

    if (!AbstractWindow::onMouseTest ()) return false;

    POINT mPos = txMousePos ();

    COLORREF pixel = txGetPixel (mPos.x - x_, mPos.y - y_, image_);

    if (pixel == 0xFFFFFF) return false;

    return true;

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

