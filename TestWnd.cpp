const int wWidth = 800;
const int wHeight = 800;

//-----------------------------------------------------------------------------

#include <TXLib.h>
#include "WNDLib.h"
#include "ResourceManager.h"

//-----------------------------------------------------------------------------

int main ()

{

    txCreateWindow (wWidth, wHeight);
    txDisableAutoPause ();

    txBegin ();

    ContainerWindow * mainWindow = new ContainerWindow {0, 0, wWidth, wHeight, RGB (40, 40, 40)};

}
