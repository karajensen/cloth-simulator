/****************************************************************
* Kara Jensen (KaraPeaceJensen@gmail.com) 
* Holds .NET GUI specific code
*****************************************************************/

#pragma once
#include "GUIForm.h"

namespace GUI
{
    public ref class ManagedGUI
    {
    public:

        ManagedGUI();

        /**
        * Ticks the GUI
        */
        void Tick();

    private:

        GUIForm^ m_form;
    };
}