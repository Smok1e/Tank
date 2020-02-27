
const int wWidth = 800;
const int wHeight = 800;

//-----------------------------------------------------------------------------

#include <TXLib.h>
#include "WNDLib.h"
#include "ResourceManager.h"

//-----------------------------------------------------------------------------

void testAction (AbstractWindow * wnd);

double rnd (double from, double to);

//-----------------------------------------------------------------------------

int main ()

{

    txCreateWindow (wWidth, wHeight);
    txDisableAutoPause ();

    txBegin ();

    ResourceManager ResManager = {Resources, "Resources\\Images"};

    ContainerWindow * container = new ContainerWindow ({0, 0, wWidth, wHeight, TX_WHITE});

    container -> addWindow (new ImageButton {0, 150, ResManager.getImage (ImageIlluminati), testAction});

    double r = 100;

    double A = 0;

    while (!GetAsyncKeyState (VK_ESCAPE))

    {

        container -> onTimeTick ();

        txSleep (1);

    }

}

//-----------------------------------------------------------------------------

void testAction(AbstractWindow * wnd)

{

    wnd -> color_ = RGB (rnd (0, 255), rnd (0, 255), rnd (0, 255));

}

//-----------------------------------------------------------------------------

double rnd (double from, double to)

{

    return from + 1.0 * rand () / RAND_MAX * (to - from);

}
