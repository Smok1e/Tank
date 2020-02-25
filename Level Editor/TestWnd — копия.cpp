
const int wWidth = 800;
const int wHeight = 800;

//-----------------------------------------------------------------------------

#include <TXLib.h>
#include "WNDLib.h"
#include "ResourceManager.h"

//-----------------------------------------------------------------------------

struct PaintWindow : ContainerWindow

{

    COLORREF drawColor_;

    double r_;

    PaintWindow (double x, double y, double width, double height, COLORREF color, void (*press) (Window * wnd) = NULL);

    void setColor (COLORREF color);

    COLORREF getColor ();

};

PaintWindow::PaintWindow (double x, double y, double width, double height, COLORREF color, void (*press) (Window * wnd)) :

    ContainerWindow (x, y, width, height, color, press),

    drawColor_ (TX_RED),

    r_         (20)

{}

//-----------------------------------------------------------------------------

struct PaintBox : PictureBox

{

    PaintBox (double x, double y, HDC image);

    ~PaintBox ();

    virtual void draw ();

};

PaintBox::PaintBox (double x, double y, HDC image) :

    PictureBox (x, y, image)

{}

PaintBox::~PaintBox ()

{

    txDeleteDC (image_);

}

//-----------------------------------------------------------------------------

struct ResultContainer : ContainerWindow

{

    int result_;

    ResultContainer (double x, double y, double width, double height, COLORREF color, void (*press) (Window * wnd) = NULL);

    void setResult (int result);

    int getResult ();

};

//-----------------------------------------------------------------------------

ResultContainer::ResultContainer (double x, double y, double width, double height, COLORREF color, void (*press) (Window * wnd)) :

    ContainerWindow (x, y, width, height, color, press),

    result_ (-1)

{}

//-----------------------------------------------------------------------------

void setColor (Window * wnd);

void save (Window * wnd);

void save_ok (Window * wnd);

void save_cancel (Window * wnd);

void load (Window * wnd);

//-----------------------------------------------------------------------------

int main ()

{

    COLORREF color = TX_RED;

    txCreateWindow (wWidth, wHeight);
    txDisableAutoPause ();

    txBegin ();

    HDC image = txCreateCompatibleDC (wWidth, wHeight - 40);

    ResourceManager res = {Resources, "Resources\\Images"};

    PaintWindow * mainWindow = new PaintWindow {0, 0, wWidth, wHeight, RGB (40, 40, 40)};

    ContainerWindow * toolBar = new ContainerWindow {0, 0, wWidth, 30, RGB (50, 50, 50)};

    Window * ColorWnd = new Window (wWidth - 30, 0, 30, 30, mainWindow -> getColor ());

    toolBar -> addWindow (ColorWnd);

    toolBar -> addWindow (new Window {0,  0, 30, 30, TX_RED,   setColor});
    toolBar -> addWindow (new Window {30, 0, 30, 30, TX_GREEN, setColor});
    toolBar -> addWindow (new Window {60, 0, 30, 30, TX_BLUE,  setColor});

    toolBar -> addWindow (new Button {90, 0, 60, 30, RGB (50, 50, 50), "Save", "Arial", 20, TX_WHITE, TX_WHITE, save});

    toolBar -> addWindow (new Button {150, 0, 60, 30, RGB (50, 50, 50), "Load", "Arial", 20, TX_WHITE, TX_WHITE, load});

    PaintBox * picture = new PaintBox (0, 30, txCreateCompatibleDC (wWidth, wHeight - 30));
    txSetFillColor (TX_WHITE, picture -> image_);
    txClear (picture -> image_);
    picture -> setId (1);

    mainWindow -> addWindow (toolBar);

    mainWindow -> addWindow (picture);

    while (!GetAsyncKeyState (VK_ESCAPE))

    {

        mainWindow -> run ();

        ColorWnd -> color_ = mainWindow -> getColor ();

        if (txMouseButtons () == 1)

        {

            double r = mainWindow -> r_;

            POINT mPos = txMousePos ();

            mPos.y -= picture -> y_;

            txSetColor (mainWindow -> getColor (), 0, image);
            txSetFillColor (mainWindow -> getColor (), image);

            txEllipse (mPos.x - r / 2, mPos.y - r / 2, mPos.x + r / 2, mPos.y + r / 2, image);

        }

        txTransparentBlt (picture -> image_, 0, 0, 0, 0, image);

        txSleep (1);

    }

    txDeleteDC (image);

    return 0;

}

//-----------------------------------------------------------------------------

void PaintWindow::setColor (COLORREF color)

{

    drawColor_ = color;

}

//-----------------------------------------------------------------------------

COLORREF PaintWindow::getColor ()

{

    return drawColor_;

}

//-----------------------------------------------------------------------------

void PaintBox::draw ()

{

    txTransparentBlt (txDC (), x_, y_, 0, 0, image_);

}

//-----------------------------------------------------------------------------

void ResultContainer::setResult (int result)

{

    result_ = result;

}

//-----------------------------------------------------------------------------

int ResultContainer::getResult ()

{

    return result_;

}

//-----------------------------------------------------------------------------

void setColor (Window * wnd)

{

    PaintWindow * mainWindow = dynamic_cast <PaintWindow*> (wnd -> getParent () -> getParent ());

    mainWindow -> setColor (wnd -> color_);

}

//-----------------------------------------------------------------------------

void save (Window * wnd)

{

    ResultContainer * container = new ResultContainer {200, 365, 400, 90, RGB (70, 70, 70)};

    WriteBox * writebox = new WriteBox {250, 390, 300, 20, RGB (45, 45, 45), "", "Arial", 20, TX_WHITE, TX_WHITE};

    container -> addWindow (writebox);

    Button * saveButton = new Button {250, container -> y_ + container -> height_ - 30, 100, 20, RGB (100, 100, 100), "Save", "Arial", 20, TX_WHITE, TX_WHITE, save_ok};

    container -> addWindow (saveButton);

    container -> addWindow (new Button {450, container -> y_ + container -> height_ - 30, 100, 20, RGB (100, 100, 100), "Cancel", "Arial", 20, TX_WHITE, TX_WHITE, save_cancel});

    ContainerWindow * top = new ContainerWindow {200, 345, 400, 20, RGB (100, 100, 100)};

    top -> addWindow (new Label {200, 345, 400, 20, "Enter path:", "Arial", 20, TX_WHITE});

    container -> addWindow (top);

    while (!GetAsyncKeyState (VK_ESCAPE))

    {

        if (GetAsyncKeyState (13))

        {

            save_ok (saveButton);

        }

        container -> run ();

        switch (container -> getResult ())

        {

            case 1:

            {

                if (writebox -> text_.empty ()) return;

                PaintBox * picture = dynamic_cast <PaintBox*> (wnd -> getParent () -> getParent () -> getWindow (1));

                txSaveImage (writebox -> getText (), picture -> image_);

                return;

            }

            case 2:

            {

                return;

            }

        }

        txSleep (1);

    }

}

//-----------------------------------------------------------------------------

void save_ok (Window * wnd)

{

    ResultContainer * container = dynamic_cast <ResultContainer*> (wnd -> getParent ());

    container -> setResult (1);

}

//-----------------------------------------------------------------------------

void save_cancel (Window * wnd)

{

    ResultContainer * container = dynamic_cast <ResultContainer*> (wnd -> getParent ());

    container -> setResult (2);

}

//-----------------------------------------------------------------------------

void load (Window * wnd)

{

    ResultContainer * container = new ResultContainer {200, 365, 400, 90, RGB (70, 70, 70)};

    WriteBox * writebox = new WriteBox {250, 390, 300, 20, RGB (45, 45, 45), "", "Arial", 20, TX_WHITE, TX_WHITE};

    container -> addWindow (writebox);

    container -> addWindow (new Button {250, container -> y_ + container -> height_ - 30, 100, 20, RGB (100, 100, 100), "Load", "Arial", 20, TX_WHITE, TX_WHITE, save_ok});

    container -> addWindow (new Button {450, container -> y_ + container -> height_ - 30, 100, 20, RGB (100, 100, 100), "Cancel", "Arial", 20, TX_WHITE, TX_WHITE, save_cancel});

    ContainerWindow * top = new ContainerWindow {200, 345, 400, 20, RGB (100, 100, 100)};

    top -> addWindow (new Label {200, 345, 400, 20, "Enter path:", "Arial", 20, TX_WHITE});

    container -> addWindow (top);

    while (!GetAsyncKeyState (VK_ESCAPE))

    {

        container -> run ();

        switch (container -> getResult ())

        {

            case 1:

            {

                PaintBox * picture = dynamic_cast <PaintBox*> (wnd -> getParent () -> getParent () -> getWindow (1));

                picture -> image_ = txLoadImage (writebox -> getText ());

                return;

            }

            case 2:

            {

                return;

            }

        }

        txSleep (1);

    }

}

//-----------------------------------------------------------------------------
