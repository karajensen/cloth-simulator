/****************************************************************
* Kara Jensen (KaraPeaceJensen@gmail.com) 
* Main Window Form, usable with the designer
*****************************************************************/

#pragma once

#include <Windows.h>
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
            simulatorForm->Size = System::Drawing::Size(800, 600);
            simulatorForm->Location = System::Drawing::Point(0, 0);
            m_pinnedSimForm = new gcroot<WinForm^>(simulatorForm);
            m_mainPanel->Controls->Add((WinForm^)*m_pinnedSimForm);

            m_gravityDown = System::Drawing::Image::FromFile("Resources//Sprites//box.png");
            m_gravityUp = System::Drawing::Image::FromFile("Resources//Sprites//gravity.png");
            m_vertsDown = System::Drawing::Image::FromFile("Resources//Sprites//box.png");
            m_vertsUp = System::Drawing::Image::FromFile("Resources//Sprites//showverts.png");
            m_handleDown = System::Drawing::Image::FromFile("Resources//Sprites//box.png");
            m_handleUp = System::Drawing::Image::FromFile("Resources//Sprites//handle.png");

            m_gravityBtn->Image = m_gravityUp;
            m_vertsBtn->Image = m_vertsUp;
            m_handleBtn->Image = m_handleUp;

            m_cynlinderBtn->Image = System::Drawing::Image::FromFile("Resources//Sprites//box.png");
            m_sphereBtn->Image = System::Drawing::Image::FromFile("Resources//Sprites//box.png");
            m_boxBtn->Image = System::Drawing::Image::FromFile("Resources//Sprites//box.png");
            m_scaleBtn->Image = System::Drawing::Image::FromFile("Resources//Sprites//move.png");
            m_moveBtn->Image = System::Drawing::Image::FromFile("Resources//Sprites//move.png");
            m_rotateBtn->Image = System::Drawing::Image::FromFile("Resources//Sprites//move.png");
            m_resetCamBtn->Image = System::Drawing::Image::FromFile("Resources//Sprites//resetcam.png");
            m_resetClothBtn->Image = System::Drawing::Image::FromFile("Resources//Sprites//resetcloth.png");
            m_unpinBtn->Image = System::Drawing::Image::FromFile("Resources//Sprites//unpin.png");
            m_removeBtn->Image = System::Drawing::Image::FromFile("Resources//Sprites//unpin.png");
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
            #ifdef _DEBUG
            OutputDebugString(L"RELEASING GUIFORM\n");
            #endif

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
        System::Drawing::Image^ m_gravityDown; ///< Image to show when gravity btn is pressed
        System::Drawing::Image^ m_gravityUp; ///< Image to show when gravity btn is not pressed
        System::Drawing::Image^ m_vertsDown; ///< Image to show when verts btn is pressed
        System::Drawing::Image^ m_vertsUp; ///< Image to show when verts btn is not pressed
        System::Drawing::Image^ m_handleDown; ///< Image to show when handle btn is pressed
        System::Drawing::Image^ m_handleUp; ///< Image to show when handle btn is not pressed

        /// <summary>
		/// Designed form components
		/// </summary>

        System::Windows::Forms::Panel^     m_mainPanel;
        System::Windows::Forms::CheckBox^  m_gravityBtn;
        System::Windows::Forms::CheckBox^  m_vertsBtn;
        System::Windows::Forms::CheckBox^  m_handleBtn;
        System::Windows::Forms::Button^    m_resetCamBtn;
        System::Windows::Forms::Button^    m_resetClothBtn;
        System::Windows::Forms::Button^    m_unpinBtn;
        System::Windows::Forms::Button^    m_scaleBtn;
        System::Windows::Forms::Button^    m_rotateBtn;
        System::Windows::Forms::Button^    m_moveBtn;
        System::Windows::Forms::Button^    m_cynlinderBtn;
        System::Windows::Forms::Button^    m_sphereBtn;
        System::Windows::Forms::Button^    m_boxBtn;
        System::Windows::Forms::Button^    m_removeBtn;


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
            this->m_mainPanel = (gcnew System::Windows::Forms::Panel());
            this->m_removeBtn = (gcnew System::Windows::Forms::Button());
            this->m_scaleBtn = (gcnew System::Windows::Forms::Button());
            this->m_rotateBtn = (gcnew System::Windows::Forms::Button());
            this->m_moveBtn = (gcnew System::Windows::Forms::Button());
            this->m_cynlinderBtn = (gcnew System::Windows::Forms::Button());
            this->m_sphereBtn = (gcnew System::Windows::Forms::Button());
            this->m_boxBtn = (gcnew System::Windows::Forms::Button());
            this->m_resetCamBtn = (gcnew System::Windows::Forms::Button());
            this->m_resetClothBtn = (gcnew System::Windows::Forms::Button());
            this->m_unpinBtn = (gcnew System::Windows::Forms::Button());
            this->m_vertsBtn = (gcnew System::Windows::Forms::CheckBox());
            this->m_handleBtn = (gcnew System::Windows::Forms::CheckBox());
            this->m_gravityBtn = (gcnew System::Windows::Forms::CheckBox());
            this->m_mainPanel->SuspendLayout();
            this->SuspendLayout();
            // 
            // m_mainPanel
            // 
            this->m_mainPanel->Controls->Add(this->m_removeBtn);
            this->m_mainPanel->Controls->Add(this->m_scaleBtn);
            this->m_mainPanel->Controls->Add(this->m_rotateBtn);
            this->m_mainPanel->Controls->Add(this->m_moveBtn);
            this->m_mainPanel->Controls->Add(this->m_cynlinderBtn);
            this->m_mainPanel->Controls->Add(this->m_sphereBtn);
            this->m_mainPanel->Controls->Add(this->m_boxBtn);
            this->m_mainPanel->Controls->Add(this->m_resetCamBtn);
            this->m_mainPanel->Controls->Add(this->m_resetClothBtn);
            this->m_mainPanel->Controls->Add(this->m_unpinBtn);
            this->m_mainPanel->Controls->Add(this->m_vertsBtn);
            this->m_mainPanel->Controls->Add(this->m_handleBtn);
            this->m_mainPanel->Controls->Add(this->m_gravityBtn);
            this->m_mainPanel->Location = System::Drawing::Point(0, 0);
            this->m_mainPanel->Name = L"m_mainPanel";
            this->m_mainPanel->Size = System::Drawing::Size(800, 600);
            this->m_mainPanel->TabIndex = 0;
            // 
            // m_removeBtn
            // 
            this->m_removeBtn->Location = System::Drawing::Point(12, 468);
            this->m_removeBtn->MaximumSize = System::Drawing::Size(32, 32);
            this->m_removeBtn->MinimumSize = System::Drawing::Size(32, 32);
            this->m_removeBtn->Name = L"m_removeBtn";
            this->m_removeBtn->Size = System::Drawing::Size(32, 32);
            this->m_removeBtn->TabIndex = 12;
            this->m_removeBtn->TabStop = false;
            this->m_removeBtn->UseVisualStyleBackColor = true;
            this->m_removeBtn->Click += gcnew System::EventHandler(this, &GUIForm::m_removeBtn_Click);
            // 
            // m_scaleBtn
            // 
            this->m_scaleBtn->Location = System::Drawing::Point(12, 430);
            this->m_scaleBtn->MaximumSize = System::Drawing::Size(32, 32);
            this->m_scaleBtn->MinimumSize = System::Drawing::Size(32, 32);
            this->m_scaleBtn->Name = L"m_scaleBtn";
            this->m_scaleBtn->Size = System::Drawing::Size(32, 32);
            this->m_scaleBtn->TabIndex = 11;
            this->m_scaleBtn->TabStop = false;
            this->m_scaleBtn->UseVisualStyleBackColor = true;
            this->m_scaleBtn->Click += gcnew System::EventHandler(this, &GUIForm::m_scaleBtn_Click);
            // 
            // m_rotateBtn
            // 
            this->m_rotateBtn->Location = System::Drawing::Point(12, 392);
            this->m_rotateBtn->MaximumSize = System::Drawing::Size(32, 32);
            this->m_rotateBtn->MinimumSize = System::Drawing::Size(32, 32);
            this->m_rotateBtn->Name = L"m_rotateBtn";
            this->m_rotateBtn->Size = System::Drawing::Size(32, 32);
            this->m_rotateBtn->TabIndex = 10;
            this->m_rotateBtn->TabStop = false;
            this->m_rotateBtn->UseVisualStyleBackColor = true;
            this->m_rotateBtn->Click += gcnew System::EventHandler(this, &GUIForm::m_rotateBtn_Click);
            // 
            // m_moveBtn
            // 
            this->m_moveBtn->Location = System::Drawing::Point(12, 354);
            this->m_moveBtn->MaximumSize = System::Drawing::Size(32, 32);
            this->m_moveBtn->MinimumSize = System::Drawing::Size(32, 32);
            this->m_moveBtn->Name = L"m_moveBtn";
            this->m_moveBtn->Size = System::Drawing::Size(32, 32);
            this->m_moveBtn->TabIndex = 9;
            this->m_moveBtn->TabStop = false;
            this->m_moveBtn->UseVisualStyleBackColor = true;
            this->m_moveBtn->Click += gcnew System::EventHandler(this, &GUIForm::m_moveBtn_Click);
            // 
            // m_cynlinderBtn
            // 
            this->m_cynlinderBtn->Location = System::Drawing::Point(12, 316);
            this->m_cynlinderBtn->MaximumSize = System::Drawing::Size(32, 32);
            this->m_cynlinderBtn->MinimumSize = System::Drawing::Size(32, 32);
            this->m_cynlinderBtn->Name = L"m_cynlinderBtn";
            this->m_cynlinderBtn->Size = System::Drawing::Size(32, 32);
            this->m_cynlinderBtn->TabIndex = 8;
            this->m_cynlinderBtn->TabStop = false;
            this->m_cynlinderBtn->UseVisualStyleBackColor = true;
            this->m_cynlinderBtn->Click += gcnew System::EventHandler(this, &GUIForm::m_cynlinderBtn_Click);
            // 
            // m_sphereBtn
            // 
            this->m_sphereBtn->Location = System::Drawing::Point(12, 278);
            this->m_sphereBtn->MaximumSize = System::Drawing::Size(32, 32);
            this->m_sphereBtn->MinimumSize = System::Drawing::Size(32, 32);
            this->m_sphereBtn->Name = L"m_sphereBtn";
            this->m_sphereBtn->Size = System::Drawing::Size(32, 32);
            this->m_sphereBtn->TabIndex = 7;
            this->m_sphereBtn->TabStop = false;
            this->m_sphereBtn->UseVisualStyleBackColor = true;
            this->m_sphereBtn->Click += gcnew System::EventHandler(this, &GUIForm::m_sphereBtn_Click);
            // 
            // m_boxBtn
            // 
            this->m_boxBtn->Location = System::Drawing::Point(12, 240);
            this->m_boxBtn->MaximumSize = System::Drawing::Size(32, 32);
            this->m_boxBtn->MinimumSize = System::Drawing::Size(32, 32);
            this->m_boxBtn->Name = L"m_boxBtn";
            this->m_boxBtn->Size = System::Drawing::Size(32, 32);
            this->m_boxBtn->TabIndex = 6;
            this->m_boxBtn->TabStop = false;
            this->m_boxBtn->UseVisualStyleBackColor = true;
            this->m_boxBtn->Click += gcnew System::EventHandler(this, &GUIForm::m_boxBtn_Click);
            // 
            // m_resetCamBtn
            // 
            this->m_resetCamBtn->Location = System::Drawing::Point(12, 202);
            this->m_resetCamBtn->MaximumSize = System::Drawing::Size(32, 32);
            this->m_resetCamBtn->MinimumSize = System::Drawing::Size(32, 32);
            this->m_resetCamBtn->Name = L"m_resetCamBtn";
            this->m_resetCamBtn->Size = System::Drawing::Size(32, 32);
            this->m_resetCamBtn->TabIndex = 5;
            this->m_resetCamBtn->TabStop = false;
            this->m_resetCamBtn->UseVisualStyleBackColor = true;
            this->m_resetCamBtn->Click += gcnew System::EventHandler(this, &GUIForm::m_resetCamBtn_Click);
            // 
            // m_resetClothBtn
            // 
            this->m_resetClothBtn->Location = System::Drawing::Point(12, 164);
            this->m_resetClothBtn->MaximumSize = System::Drawing::Size(32, 32);
            this->m_resetClothBtn->MinimumSize = System::Drawing::Size(32, 32);
            this->m_resetClothBtn->Name = L"m_resetClothBtn";
            this->m_resetClothBtn->Size = System::Drawing::Size(32, 32);
            this->m_resetClothBtn->TabIndex = 4;
            this->m_resetClothBtn->TabStop = false;
            this->m_resetClothBtn->UseVisualStyleBackColor = true;
            this->m_resetClothBtn->Click += gcnew System::EventHandler(this, &GUIForm::m_resetClothBtn_Click);
            // 
            // m_unpinBtn
            // 
            this->m_unpinBtn->Location = System::Drawing::Point(12, 126);
            this->m_unpinBtn->MaximumSize = System::Drawing::Size(32, 32);
            this->m_unpinBtn->MinimumSize = System::Drawing::Size(32, 32);
            this->m_unpinBtn->Name = L"m_unpinBtn";
            this->m_unpinBtn->Size = System::Drawing::Size(32, 32);
            this->m_unpinBtn->TabIndex = 3;
            this->m_unpinBtn->TabStop = false;
            this->m_unpinBtn->UseVisualStyleBackColor = true;
            this->m_unpinBtn->Click += gcnew System::EventHandler(this, &GUIForm::m_unpinBtn_Click);
            // 
            // m_vertsBtn
            // 
            this->m_vertsBtn->Appearance = System::Windows::Forms::Appearance::Button;
            this->m_vertsBtn->Location = System::Drawing::Point(12, 88);
            this->m_vertsBtn->MaximumSize = System::Drawing::Size(32, 32);
            this->m_vertsBtn->MinimumSize = System::Drawing::Size(32, 32);
            this->m_vertsBtn->Name = L"m_vertsBtn";
            this->m_vertsBtn->Size = System::Drawing::Size(32, 32);
            this->m_vertsBtn->TabIndex = 2;
            this->m_vertsBtn->TabStop = false;
            this->m_vertsBtn->UseVisualStyleBackColor = true;
            this->m_vertsBtn->CheckStateChanged += gcnew System::EventHandler(this, &GUIForm::m_vertsBtn_CheckStateChanged);
            // 
            // m_handleBtn
            // 
            this->m_handleBtn->Appearance = System::Windows::Forms::Appearance::Button;
            this->m_handleBtn->Location = System::Drawing::Point(12, 50);
            this->m_handleBtn->MaximumSize = System::Drawing::Size(32, 32);
            this->m_handleBtn->MinimumSize = System::Drawing::Size(32, 32);
            this->m_handleBtn->Name = L"m_handleBtn";
            this->m_handleBtn->Size = System::Drawing::Size(32, 32);
            this->m_handleBtn->TabIndex = 1;
            this->m_handleBtn->TabStop = false;
            this->m_handleBtn->UseVisualStyleBackColor = true;
            this->m_handleBtn->CheckStateChanged += gcnew System::EventHandler(this, &GUIForm::m_handleBtn_CheckStateChanged);
            // 
            // m_gravityBtn
            // 
            this->m_gravityBtn->Appearance = System::Windows::Forms::Appearance::Button;
            this->m_gravityBtn->Location = System::Drawing::Point(12, 12);
            this->m_gravityBtn->MaximumSize = System::Drawing::Size(32, 32);
            this->m_gravityBtn->MinimumSize = System::Drawing::Size(32, 32);
            this->m_gravityBtn->Name = L"m_gravityBtn";
            this->m_gravityBtn->Size = System::Drawing::Size(32, 32);
            this->m_gravityBtn->TabIndex = 0;
            this->m_gravityBtn->TabStop = false;
            this->m_gravityBtn->UseVisualStyleBackColor = true;
            this->m_gravityBtn->CheckStateChanged += gcnew System::EventHandler(this, &GUIForm::m_gravityBtn_CheckStateChanged);
            // 
            // GUIForm
            // 
            this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(190)), static_cast<System::Int32>(static_cast<System::Byte>(190)), 
                static_cast<System::Int32>(static_cast<System::Byte>(195)));
            this->ClientSize = System::Drawing::Size(800, 600);
            this->Controls->Add(this->m_mainPanel);
            this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
            this->MaximumSize = System::Drawing::Size(806, 633);
            this->MinimumSize = System::Drawing::Size(806, 633);
            this->Name = L"GUIForm";
            this->Text = L"Cloth Simulator";
            this->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &GUIForm::GUIForm_FormClosed);
            this->m_mainPanel->ResumeLayout(false);
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

		/// <summary>
		/// On Gravity Button Change
		/// </summary>
        System::Void m_gravityBtn_CheckStateChanged(System::Object^  sender, System::EventArgs^  e) 
        {
            System::Windows::Forms::CheckBox^ checkbox = static_cast<System::Windows::Forms::CheckBox^>(sender);
            m_callbacks->setGravity(checkbox->Checked);
            if(checkbox->Checked)
            {
                checkbox->Image = m_gravityDown;
            }
            else
            {
                checkbox->Image = m_gravityUp;
            }
        }

		/// <summary>
		/// On Reset Cloth Button Press
		/// </summary>
        System::Void m_resetClothBtn_Click(System::Object^  sender, System::EventArgs^  e) 
        {
            m_callbacks->resetCloth();
        }

		/// <summary>
		/// On Handle Mode Button Change
		/// </summary>
        System::Void m_handleBtn_CheckStateChanged(System::Object^  sender, System::EventArgs^  e) 
        {
            System::Windows::Forms::CheckBox^ checkbox = static_cast<System::Windows::Forms::CheckBox^>(sender);
            m_callbacks->setHandleMode(checkbox->Checked);
            if(checkbox->Checked)
            {
                m_gravityBtn->Checked = false;
                m_gravityBtn->Enabled = false;
                checkbox->Image = m_handleDown;
            }
            else
            {
                m_gravityBtn->Enabled = true;
                checkbox->Image = m_handleUp;
            }
        }

		/// <summary>
		/// On Vertex Visibility Change
		/// </summary>
        System::Void m_vertsBtn_CheckStateChanged(System::Object^  sender, System::EventArgs^  e) 
        {
            System::Windows::Forms::CheckBox^ checkbox = static_cast<System::Windows::Forms::CheckBox^>(sender);
            m_callbacks->setVertsVisible(checkbox->Checked);
            if(checkbox->Checked)
            {
                checkbox->Image = m_vertsDown;
            }
            else
            {
                checkbox->Image = m_vertsUp;
            }
        }

		/// <summary>
		/// On Reset Camera Button Press
		/// </summary>
        System::Void m_resetCamBtn_Click(System::Object^  sender, System::EventArgs^  e)
        {
            m_callbacks->resetCamera();
        }

		/// <summary>
		/// On Unpin Vertex Button Press
		/// </summary>
        System::Void m_unpinBtn_Click(System::Object^  sender, System::EventArgs^  e)
        {
            m_callbacks->unpinCloth();
        }

		/// <summary>
		/// On Create Box Button Press
		/// </summary>
        System::Void m_boxBtn_Click(System::Object^  sender, System::EventArgs^  e)
        {
        }

		/// <summary>
		/// On Create Sphere Button Press
		/// </summary>
        System::Void m_sphereBtn_Click(System::Object^  sender, System::EventArgs^  e)
        {
        }

		/// <summary>
		/// On Create Cylinder Button Press
		/// </summary>
        System::Void m_cynlinderBtn_Click(System::Object^  sender, System::EventArgs^  e) 
        {
        }

		/// <summary>
		/// On Move Object Button Press
		/// </summary>
        System::Void m_moveBtn_Click(System::Object^  sender, System::EventArgs^  e) 
        {
        }

		/// <summary>
		/// On Rotate Object Button Press
		/// </summary>
        System::Void m_rotateBtn_Click(System::Object^  sender, System::EventArgs^  e)
        {
        }

		/// <summary>
		/// On Scale Object Button Press
		/// </summary>
        System::Void m_scaleBtn_Click(System::Object^  sender, System::EventArgs^  e)
        {
        }

		/// <summary>
		/// On Clear Scene Button Press
		/// </summary>
        System::Void m_removeBtn_Click(System::Object^  sender, System::EventArgs^  e)
        {
        }
    };
}
