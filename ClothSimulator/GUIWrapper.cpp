
#define EXPORTAPI
#include "GUIWrapper.h"
#include "GUIForm.h"
#include <vcclr.h>

namespace GUI
{
    typedef gcroot<GUIForm^>* GuiPtr;

    GuiWrapper::GuiWrapper() :
        m_guiForm(nullptr)
    {
        //gcroot will point to updated address as gc moves object
        GUIForm^ gui = gcnew GUIForm();
        GuiPtr ptr = new gcroot<GUIForm^>(gui);
        m_guiForm = (void*)ptr;
    }

    GuiWrapper::~GuiWrapper()
    {
        #ifdef _DEBUG
        OutputDebugString(L"GuiWrapper::~GuiWrapper\n");
        #endif

        if (m_guiForm)
        {
            //free gcroot handle
            GuiPtr ptr = reinterpret_cast<GuiPtr>(m_guiForm);
            m_guiForm = nullptr;
            delete ptr;
        }
    }

    void GuiWrapper::EnableMeshCreation(bool enable)
    {
        GuiPtr ptr = reinterpret_cast<GuiPtr>(m_guiForm);
        ((GUIForm^)*ptr)->EnableMeshCreation(enable);
    }

    void GuiWrapper::SetCallbacks(GuiCallbacks* callbacks)
    {
        GuiPtr ptr = reinterpret_cast<GuiPtr>(m_guiForm);
        ((GUIForm^)*ptr)->SetCallbacks(callbacks);
    }

    bool GuiWrapper::Update()
    {
        System::Windows::Forms::Application::DoEvents();
        return true;
    }

    void GuiWrapper::Show()
    {
        GuiPtr ptr = reinterpret_cast<GuiPtr>(m_guiForm);
        ((GUIForm^)*ptr)->Show();
    }

    WindowHandle GuiWrapper::GetWindowHandle()
    {
        GuiPtr ptr = reinterpret_cast<GuiPtr>(m_guiForm);
        return ((GUIForm^)*ptr)->GetWindowHandle();
    }
}
