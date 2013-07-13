/****************************************************************
* Kara Jensen (KaraPeaceJensen@gmail.com) 
* Interface that native code will talk to
*****************************************************************/

#pragma once

#ifdef EXPORTAPI
#define GUIINTAPI __declspec(dllexport)
#else
#define GUIINTAPI __declspec(dllimport)
#endif

namespace GUI
{
    class GUIINTAPI NativeGUI
    {
    public:

        NativeGUI();
        ~NativeGUI();

        /**
        * Ticks the GUI
        */
        void Tick();

    private:

        void* m_ManagedGUI; ///< pointer to managed GUI Interface
    };
}