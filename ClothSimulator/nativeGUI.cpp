
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

    NativeGUI::~NativeGUI()
    {
        #ifdef _DEBUG
        OutputDebugString(L"RELEASING NATIVEGUI\n");
        #endif

        if (m_ManagedGUI)
        {
            //free gcroot handle
            ManagedPtr ptr = reinterpret_cast<gcroot<ManagedGUI^>*>(m_ManagedGUI);
            m_ManagedGUI = nullptr;
            delete ptr;
        }
    }

    void NativeGUI::SetCallbacks(GuiCallback* callback)
    {
        ManagedPtr ptr = reinterpret_cast<gcroot<ManagedGUI^>*>(m_ManagedGUI);
        ((ManagedGUI^)*ptr)->SetCallbacks(callback);
    }

    bool NativeGUI::Update()
    {
        ManagedPtr ptr = reinterpret_cast<gcroot<ManagedGUI^>*>(m_ManagedGUI);
        return ((ManagedGUI^)*ptr)->Update();
    }

    void NativeGUI::Show()
    {
        ManagedPtr ptr = reinterpret_cast<gcroot<ManagedGUI^>*>(m_ManagedGUI);
        ((ManagedGUI^)*ptr)->Show();
    }

    WindowHandles NativeGUI::GetWindowHandles()
    {
        ManagedPtr ptr = reinterpret_cast<gcroot<ManagedGUI^>*>(m_ManagedGUI);
        void* handle = (((ManagedGUI^)*ptr)->GetWindowHandle()).ToPointer();
        void* instance = (((ManagedGUI^)*ptr)->GetWindowInstance()).ToPointer();
        return WindowHandles(static_cast<HWND>(handle), static_cast<HINSTANCE>(instance));
    }
}
