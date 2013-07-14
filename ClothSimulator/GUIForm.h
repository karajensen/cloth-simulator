/****************************************************************
* Kara Jensen (KaraPeaceJensen@gmail.com) 
* Main Window Form, usable with the designer
*****************************************************************/

#pragma once

#include <vcclr.h>
#include "guicallbacks.h"

namespace GUI 
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	public ref class GUIForm : public System::Windows::Forms::Form
	{
	public:

        typedef System::Windows::Forms::Form WinForm;

		/// <summary>
		/// Constructor
		/// </summary>
		GUIForm(void) :
            m_callbacks(nullptr)
		{
			InitializeComponent();

            WinForm^ simulatorForm = gcnew WinForm();
            simulatorForm->TopLevel = false;
            simulatorForm->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
            simulatorForm->Dock = System::Windows::Forms::DockStyle::Fill;
            simulatorForm->Visible = true;
            simulatorForm->BackColor = System::Drawing::Color::DarkGray;
            simulatorForm->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Center;
            simulatorForm->BackgroundImage = System::Drawing::Image::FromFile("Resources//Sprites//loading.png");

            m_pinnedSimForm = new gcroot<WinForm^>(simulatorForm);
            splitContainer1->Panel2->Controls->Add((WinForm^)*m_pinnedSimForm);
		}

		/// <summary>
		/// Updates the form callbacks
		/// </summary>
        void UpdateCallbacks()
        {
            //int x = Cursor.Position.X - Location.X - splitContainer1.Location.X - splitContainer1->Panel2.Location.X;
            //int y = Cursor.Position.Y - Location.Y - splitContainer1.Location.Y - splitContainer1->Panel2.Location.Y;
            m_callbacks->updateMouse(0, 0);        
        }

		/// <summary>
		/// Sets all native callbacks for the form events
		/// </summary>
        void SetCallbacks(GuiCallback* callback)
        {
            m_callbacks = callback;
        }

		/// <summary>
		/// Returns the handle to the simulation window
		/// </summary>
        IntPtr GetWindowHandle()
        {
            WinForm^ simForm = (WinForm^)*m_pinnedSimForm;
            return simForm->Handle;
        }

	protected:

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
        !GUIForm()
        {
			if (components)
			{
				delete components;
			}
            if (m_pinnedSimForm)
            {
                //free gcroot handle
                gcroot<WinForm^>* ptr = reinterpret_cast<gcroot<WinForm^>*>(m_pinnedSimForm);
                m_pinnedSimForm = nullptr;
                delete ptr;
            }
        }
		~GUIForm()
		{
            //will automatically suppress finalisation
            this->!GUIForm();
		}

	private:

        GuiCallback* m_callbacks; ///< Callbacks for the gui
        gcroot<WinForm^>* m_pinnedSimForm; ///< pinned as native needs window handle

		/// <summary>
		/// Designed form components
		/// </summary>
        System::Windows::Forms::SplitContainer^  splitContainer1;
        System::Windows::Forms::Button^  button1;

		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
            this->splitContainer1 = (gcnew System::Windows::Forms::SplitContainer());
            this->button1 = (gcnew System::Windows::Forms::Button());
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->splitContainer1))->BeginInit();
            this->splitContainer1->Panel1->SuspendLayout();
            this->splitContainer1->SuspendLayout();
            this->SuspendLayout();
            // 
            // splitContainer1
            // 
            this->splitContainer1->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->splitContainer1->Dock = System::Windows::Forms::DockStyle::Fill;
            this->splitContainer1->IsSplitterFixed = true;
            this->splitContainer1->Location = System::Drawing::Point(0, 0);
            this->splitContainer1->Name = L"splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this->splitContainer1->Panel1->Controls->Add(this->button1);
            this->splitContainer1->Size = System::Drawing::Size(912, 600);
            this->splitContainer1->SplitterDistance = 101;
            this->splitContainer1->TabIndex = 0;
            // 
            // button1
            // 
            this->button1->FlatStyle = System::Windows::Forms::FlatStyle::System;
            this->button1->Location = System::Drawing::Point(12, 12);
            this->button1->MaximumSize = System::Drawing::Size(32, 32);
            this->button1->MinimumSize = System::Drawing::Size(32, 32);
            this->button1->Name = L"button1";
            this->button1->Size = System::Drawing::Size(32, 32);
            this->button1->TabIndex = 0;
            this->button1->UseVisualStyleBackColor = true;
            // 
            // GUIForm
            // 
            this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->BackColor = System::Drawing::Color::DarkGray;
            this->ClientSize = System::Drawing::Size(912, 600);
            this->Controls->Add(this->splitContainer1);
            this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
            this->MaximumSize = System::Drawing::Size(918, 645);
            this->MinimumSize = System::Drawing::Size(918, 600);
            this->Name = L"GUIForm";
            this->Text = L"Cloth Simulator";
            this->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &GUIForm::GUIForm_FormClosed);
            this->splitContainer1->Panel1->ResumeLayout(false);
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->splitContainer1))->EndInit();
            this->splitContainer1->ResumeLayout(false);
            this->ResumeLayout(false);

        }
#pragma endregion

		/// <summary>
		/// On GUI Close Event
		/// </summary>
        System::Void GUIForm_FormClosed(System::Object^ sender, System::Windows::Forms::FormClosedEventArgs^ e) 
        {
            m_callbacks->quitFn();
        }
    };
}
