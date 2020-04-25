////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - GUIWrapper.h
////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "GUICallbacks.h"

#ifdef EXPORTAPI
#define GUIINTAPI __declspec(dllexport)
#else
#define GUIINTAPI __declspec(dllimport)
#endif

namespace GUI
{
    /**
    * Interface that native code will talk to
    */
    class GUIINTAPI GuiWrapper
    {
    public:

        GuiWrapper();
        ~GuiWrapper();

        /**
        * Sets the native callbacks for the GUI
        * @param callbacks A struct of all callbacks
        */
        void SetCallbacks(GuiCallbacks* callbacks);

        /**
        * Sets whether the mesh creation gui buttons are enabled
        * @param enable Whether the buttons are enabled/disabled
        */
        void EnableMeshCreation(bool enable);

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