
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
        OutputDebugString(L"ManagedGUI::!ManagedGUI\n");
        #endif
    }

    void ManagedGUI::SetCallbacks(GuiCallback* callback)
    {
        m_form->SetCallbacks(callback);
    }

    WindowHandle ManagedGUI::GetWindowHandle()
    {
        return m_form->GetWindowHandle();
    }

    void ManagedGUI::Show()
    {
        m_form->Show();
    }

    bool ManagedGUI::Update()
    {
        System::Windows::Forms::Application::DoEvents();
        return true;
    }
}
