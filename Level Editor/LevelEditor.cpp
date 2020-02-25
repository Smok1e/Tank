const int wWidth = 800;
const int wHeight = 800;

//-----------------------------------------------------------------------------

#include <TXLib.h>
#include "WNDLib.h"
#include "ResourceManager.h"

//-----------------------------------------------------------------------------

double rnd (double from, double to);

void press (Window * wnd);

void exit (Window * wnd);

//-----------------------------------------------------------------------------

int main ()

{

    txCreateWindow (800, 800);
    txDisableAutoPause ();

    SetWindowText (txWindow (), "WNDLib test");

    txBegin ();

    ResourceManager ResManager (Resources, "Resources\\Images");

    ContainerWindow * MainWindow = new ContainerWindow (0, 0, 800, 800, RGB (40, 40, 40));

    MainWindow -> addWindow (new PictureBox {0, 0, ResManager.getImage (ImageBackground)});

    /*//label

    MainWindow -> addWindow (new Label {50, 100, 0, 0, "Simple button example:", "Courier", 25, TX_BLACK});
    MainWindow -> addWindow (new Label {50, 380, 0, 0, "Circle button example:", "Courier", 25, TX_BLACK});
    MainWindow -> addWindow (new Label {50, 200, 0, 0, "Picture box example:",   "Courier", 25, TX_BLACK});

    //simple button

    MainWindow -> addWindow (new Button {50, 130, 100, 30, RGB (40, 40, 40), "Button", "Arial", 30, TX_WHITE, TX_WHITE, press});

    //picture box

    MainWindow -> addWindow (new PictureBox {50, 240, ResManager.getImage (ImageIlluminati), ResManager.getImage (ImageLightIlluminati), press});

    //circle button

    MainWindow -> addWindow (new CircleButton {90, 450, 40, RGB (35, 35, 35), TX_WHITE, press});

    //text box

    MainWindow -> addWindow (new TextBox {50, 530, 200, 50, RGB (35, 35, 35), "And this is a textbox.", "VAG World", 35, TX_WHITE, press});

    ContainerWindow * testWindow = new ContainerWindow {700, 700, 50, 50, TX_WHITE};*/

    MainWindow -> addWindow (new Button {0, 0, 100, 25, RGB (45, 45, 45), "test", "Arial", 20, TX_WHITE, TX_WHITE, press});

    ContainerWindow * container = new ContainerWindow {150, 250, 500, 300, RGB (130, 130, 130)};

    container -> visible_ = false;

    container -> setId (1);

    ContainerWindow * toolBar = new ContainerWindow {150, 250, 500, 20, RGB (90, 90, 90)};

    toolBar -> addWindow (new Button {630, 250, 20, 20, TX_RED, "", "", 0, TX_WHITE, TX_WHITE, exit});

    toolBar -> addWindow (new Label {150, 250, 0, 0, "Test", "Arial", 20, TX_WHITE});

    container -> addWindow (toolBar);

    WriteBox * writeBox = new WriteBox {300, 330, 200, 40, RGB (45, 45, 45), "text", "Arial", 20, TX_WHITE, TX_WHITE};

    TextBox * test = new TextBox {300, 430, 200, 40, RGB (45, 45, 45), "text", "VAG World", 30, TX_WHITE};

    container -> addWindow (test);
    container -> addWindow (writeBox);

    MainWindow -> addWindow (container);

    while (!GetAsyncKeyState (VK_ESCAPE))

    {

        MainWindow -> run ();

        test -> text_ = writeBox -> getText ();

        txSleep (1);

    }

}

//-----------------------------------------------------------------------------

double rnd (double from, double to)

{

    return from + 1.0 * rand () / RAND_MAX * (to - from);

}

//-----------------------------------------------------------------------------

void press (Window * wnd)

{

    Window * container = wnd -> getParent () -> manager_.getWindow (1);

    container -> visible_ = true;

}

//-----------------------------------------------------------------------------

void exit (Window * wnd)

{

    wnd -> getParent () -> getParent () -> visible_ = false;

}

