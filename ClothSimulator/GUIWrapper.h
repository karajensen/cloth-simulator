/****************************************************************
* Kara Jensen (KaraPeaceJensen@gmail.com) 
* Interface that native code will talk to
*****************************************************************/

#pragma once
#include "GUICallbacks.h"

#ifdef EXPORTAPI
#define GUIINTAPI __declspec(dllexport)
#else
#define GUIINTAPI __declspec(dllimport)
#endif

namespace GUI
{
    class GUIINTAPI GuiWrapper
    {
    public:

        GuiWrapper();
        ~GuiWrapper();

        /**
        * Sets the native callbacks for the GUI
        * @param a struct of all callbacks
        */
        void SetCallbacks(GuiCallbacks* callbacks);

        /**
        * Ticks the GUI
        * @return whether the update was successful
        */
        bool Update();

        /**
        * @return the handle to the simulation window
        */
        WindowHandle GetWindowHandle();

        /**
        * Shows the GUI Window
        */
        void Show();

    private:

        void* m_guiForm; ///< pointer to managed gui form
    };
}