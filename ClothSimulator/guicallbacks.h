/****************************************************************
* Kara Jensen (KaraPeaceJensen@gmail.com) 
* List of callback information for the gui
*****************************************************************/
#pragma once
#include <functional>

namespace GUI
{
    typedef std::function<void(void)> VoidFn;
    typedef std::function<void(int,int)> SetPointFn;

    struct GuiCallback
    {
        VoidFn quitFn; ///< Function called to quit the simulation
        SetPointFn updateMouse; ///< Function to update the mouse position
    };
}