/****************************************************************
* Kara Jensen (KaraPeaceJensen@gmail.com) 
* Holds .NET GUI specific code
*****************************************************************/

#pragma once
#include "GUIForm.h"
#include "guicallbacks.h"

namespace GUI
{
    public ref class ManagedGUI
    {
    public:

        /**
        * Constructor
        */
        ManagedGUI();

        /**
        * Sets the native callbacks for the GUI
        * @param a struct of all callbacks
        */
        void SetCallbacks(GuiCallback* callback);

        /**
        * Ticks the GUI
        * @return whether the update was successful
        */
        bool Update();

        /**
        * Shows the GUI Window
        */
        void Show();

        /**
        * @return the handle to the simulation window
        */
        IntPtr GetWindowHandle();

        /**
        * @return the handle instance to the simulation window
        */
        IntPtr GetWindowInstance();

    private:

        GUIForm^ m_form;
    };
}