
#define EXPORTAPI
#include "nativeGUI.h"
#include "managedGUI.h"
#include <vcclr.h>

namespace GUI
{
    typedef gcroot<ManagedGUI^>* ManagedPtr;

    NativeGUI::NativeGUI() :
        m_ManagedGUI(nullptr)
    {
        //gcroot will point to updated address as gc moves object
        ManagedGUI^ managed = gcnew ManagedGUI();
        gcroot<ManagedGUI^>* ptr = new gcroot<ManagedGUI^>(managed);
        m_ManagedGUI = (void*)ptr;
    }

    void NativeGUI::Tick()
    {
        ManagedPtr ptr = reinterpret_cast<gcroot<ManagedGUI^>*>(m_ManagedGUI);
        if(m_ManagedGUI)
        {
            ((ManagedGUI^)*ptr)->Tick();
        }
    }

    NativeGUI::~NativeGUI()
    {
        if (m_ManagedGUI)
        {
            //free gcroot handle
            ManagedPtr ptr = reinterpret_cast<gcroot<ManagedGUI^>*>(m_ManagedGUI);
            m_ManagedGUI = nullptr;
            delete ptr;
        }
    }
}
