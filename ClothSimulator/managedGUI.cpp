
#include "managedGUI.h"

namespace GUI
{
    ManagedGUI::ManagedGUI()
    {
        m_form = gcnew GUIForm();
        m_form->Show();
    }

    void ManagedGUI::Tick()
    {
    }
}
