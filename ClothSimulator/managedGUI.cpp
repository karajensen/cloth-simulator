
#include "managedGUI.h"

namespace GUI
{
    ManagedGUI::ManagedGUI()
    {
        m_form = gcnew GUIForm();
    }

    ManagedGUI::~ManagedGUI()
    {
        this->!ManagedGUI();
    }

    ManagedGUI::!ManagedGUI()
    {
        #ifdef _DEBUG
        OutputDebugString(L"RELEASING MANAGEDGUI\n");
        #endif
    }

    void ManagedGUI::SetCallbacks(GuiCallback* callback)
    {
        m_form->SetCallbacks(callback);
    }

    IntPtr ManagedGUI::GetWindowHandle()
    {
        return m_form->GetWindowHandle();
    }

    IntPtr ManagedGUI::GetWindowInstance()
    {
        return System::Runtime::InteropServices::Marshal::GetHINSTANCE(
            System::Reflection::Assembly::GetExecutingAssembly()->GetModules()[0]);
    }

    void ManagedGUI::Show()
    {
        m_form->Show();
    }

    bool ManagedGUI::Update()
    {
        System::Windows::Forms::Application::DoEvents();
        m_form->UpdateCallbacks();
        return true;
    }
}
