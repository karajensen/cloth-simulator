/****************************************************************
* Kara Jensen (KaraPeaceJensen@gmail.com) 
* Main Window Form, usable with the designer
*****************************************************************/
#pragma once
#include <vcclr.h>
#include "GUICallbacks.h"

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
            SetSimulationForm();
            SetButtonImages();
		}

		/// <summary>
		/// Sets all native callbacks for the form events
		/// </summary>
        void SetCallbacks(GuiCallbacks* callbacks)
        {
            m_callbacks = callbacks;
        }

		/// <summary>
		/// Returns the handle to the simulation window/instance
		/// </summary>
        WindowHandle GetWindowHandle()
        {
            WinForm^ simForm = (WinForm^)*m_pinnedSimForm;
            WindowHandle window;

            IntPtr handle = simForm->Handle;
            IntPtr instance = System::Runtime::InteropServices::Marshal::GetHINSTANCE(
                System::Reflection::Assembly::GetExecutingAssembly()->GetModules()[0]);

            window.handle = static_cast<HWND>(handle.ToPointer());
            window.instance = static_cast<HINSTANCE>(instance.ToPointer());
            return window;
        }

	protected:

		/// <summary>
		/// Finaliser: Clean up any resources being used.
		/// </summary>
        !GUIForm()
        {
            #ifdef _DEBUG
            OutputDebugString(L"GUIForm::!GUIForm\n");
            #endif

			if (components)
			{
				delete components;
                components = nullptr;
			}
            if (m_pinnedSimForm)
            {
                //free gcroot handle
                gcroot<WinForm^>* ptr = reinterpret_cast<gcroot<WinForm^>*>(m_pinnedSimForm);
                m_pinnedSimForm = nullptr;
                delete ptr;
            }
        }

		/// <summary>
		/// Destructor: Will automatically suppress finalisation if called
		/// </summary>
		~GUIForm()
		{
            this->!GUIForm(); 
		}

	private:

        GuiCallbacks* m_callbacks; ///< Callbacks for the gui
        gcroot<WinForm^>* m_pinnedSimForm; ///< pinned as native needs window handle
        System::Drawing::Image^ m_disabledGravity; ///< Image for disabled gravity
        System::Drawing::Image^ m_enabledGravity; ///< Image for enabled gravity

		/// <summary>
		/// Designer specific components
		/// </summary>
        System::ComponentModel::Container^ components;      ///< Auto-Required designer variable.
        System::Windows::Forms::Panel^     m_mainPanel;     ///< Panel for holding main simulation form
        System::Windows::Forms::CheckBox^  m_gravityBtn;    ///< Checkbox for toggling on gravity
        System::Windows::Forms::CheckBox^  m_vertsBtn;      ///< Checkbox for toggling on vertices
        System::Windows::Forms::CheckBox^  m_handleBtn;     ///< Checkbox for toggling on handle mode
        System::Windows::Forms::Button^    m_resetCamBtn;   ///< Button for resetting the camera
        System::Windows::Forms::Button^    m_resetClothBtn; ///< Button for resetting the cloth
        System::Windows::Forms::Button^    m_unpinBtn;      ///< Button for upinning the cloth
        System::Windows::Forms::Button^    m_scaleBtn;      ///< Button for scaling the selected scene object
        System::Windows::Forms::Button^    m_rotateBtn;     ///< Button for rotating the selected scene object
        System::Windows::Forms::Button^    m_moveBtn;       ///< Button for moving the selected scene object
        System::Windows::Forms::Button^    m_cynlinderBtn;  ///< Button for creating a cynlinder
        System::Windows::Forms::Button^    m_sphereBtn;     ///< Button for creating a sphere
        System::Windows::Forms::Button^    m_boxBtn;        ///< Button for creating a box
        System::Windows::Forms::Button^    m_animateBtn;    ///< Button for animating an object
        System::Windows::Forms::Button^    m_removeBtn;     ///< Button for clearing all scene objects

		/// <summary>
		/// Creates the main simulation form
		/// </summary>
        void SetSimulationForm()
        {
            WinForm^ simulatorForm = gcnew WinForm();
            simulatorForm->TopLevel = false;
            simulatorForm->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
            simulatorForm->Dock = System::Windows::Forms::DockStyle::Fill;
            simulatorForm->Visible = true;
            simulatorForm->Size = System::Drawing::Size(800, 600);
            simulatorForm->Location = System::Drawing::Point(0, 0);
            simulatorForm->BackColor = System::Drawing::Color::FromArgb(
                static_cast<System::Int32>(static_cast<System::Byte>(190)), 
                static_cast<System::Int32>(static_cast<System::Byte>(190)), 
                static_cast<System::Int32>(static_cast<System::Byte>(195)));

            m_pinnedSimForm = new gcroot<WinForm^>(simulatorForm);
            m_mainPanel->Controls->Add((WinForm^)*m_pinnedSimForm);
        }

		/// <summary>
		/// On GUI Close Event
		/// </summary>
        System::Void GUIFormClosed(System::Object^ sender, System::Windows::Forms::FormClosedEventArgs^ e) 
        {
            m_callbacks->quitFn();
        }

		/// <summary>
		/// On Gravity Check Change
		/// </summary>
        System::Void GravityCheckedChanged(System::Object^ sender, System::EventArgs^ e) 
        {
            System::Windows::Forms::CheckBox^ checkbox = 
                static_cast<System::Windows::Forms::CheckBox^>(sender);
            m_callbacks->setGravity(checkbox->Checked);
        }

		/// <summary>
		/// On Handle Mode Check Change
		/// </summary>
        System::Void HandleCheckedChanged(System::Object^ sender, System::EventArgs^ e) 
        {
            System::Windows::Forms::CheckBox^ checkbox = 
                static_cast<System::Windows::Forms::CheckBox^>(sender);

            m_callbacks->setHandleMode(checkbox->Checked);
            m_gravityBtn->Enabled = !checkbox->Checked;
            m_gravityBtn->BackgroundImage = checkbox->Checked ? m_disabledGravity : m_enabledGravity;
            
            if(checkbox->Checked)
            {
                //explicitly use gravity callback to prevent small jump of cloth
                m_gravityBtn->Checked = false;
                m_callbacks->setGravity(false); 
            }
        }

		/// <summary>
		/// On Vertex Visibility Check Change
		/// </summary>
        System::Void VertsCheckedChanged(System::Object^ sender, System::EventArgs^ e) 
        {
            System::Windows::Forms::CheckBox^ checkbox = 
                static_cast<System::Windows::Forms::CheckBox^>(sender);
            m_callbacks->setVertsVisible(checkbox->Checked);
        }

		/// <summary>
		/// On Reset Cloth Button Press
		/// </summary>
        System::Void ResetClothClick(System::Object^ sender, System::EventArgs^ e) 
        {
            m_callbacks->resetCloth();
        }

		/// <summary>
		/// On Reset Camera Button Press
		/// </summary>
        System::Void ResetCamClick(System::Object^ sender, System::EventArgs^ e)
        {
            m_callbacks->resetCamera();
        }

		/// <summary>
		/// On Unpin Vertex Button Press
		/// </summary>
        System::Void UnpinClick(System::Object^ sender, System::EventArgs^ e)
        {
            m_callbacks->unpinCloth();
        }

		/// <summary>
		/// On Create Box Button Press
		/// </summary>
        System::Void BoxClick(System::Object^ sender, System::EventArgs^ e)
        {
        }

		/// <summary>
		/// On Create Sphere Button Press
		/// </summary>
        System::Void SphereClick(System::Object^ sender, System::EventArgs^ e)
        {
        }

		/// <summary>
		/// On Create Cylinder Button Press
		/// </summary>
        System::Void CynlinderClick(System::Object^ sender, System::EventArgs^ e) 
        {
        }

		/// <summary>
		/// On Clear Scene Button Press
		/// </summary>
        System::Void RemoveClick(System::Object^  sender, System::EventArgs^ e)
        {
        }

		/// <summary>
		/// Loads/assigns the images for the gui buttons
		/// </summary>
        void SetButtonImages()
        {
            String^ resources = "Resources//Sprites//";
            m_cynlinderBtn->BackgroundImage = System::Drawing::Image::FromFile(resources+"cylinder.png");
            m_sphereBtn->BackgroundImage = System::Drawing::Image::FromFile(resources+"sphere.png");
            m_boxBtn->BackgroundImage = System::Drawing::Image::FromFile(resources+"box.png");
            m_scaleBtn->BackgroundImage = System::Drawing::Image::FromFile(resources+"scale.png");
            m_moveBtn->BackgroundImage = System::Drawing::Image::FromFile(resources+"move.png");
            m_rotateBtn->BackgroundImage = System::Drawing::Image::FromFile(resources+"rotate.png");
            m_resetCamBtn->BackgroundImage = System::Drawing::Image::FromFile(resources+"resetcam.png");
            m_resetClothBtn->BackgroundImage = System::Drawing::Image::FromFile(resources+"resetcloth.png");
            m_unpinBtn->BackgroundImage = System::Drawing::Image::FromFile(resources+"unpin.png");
            m_removeBtn->BackgroundImage = System::Drawing::Image::FromFile(resources+"clear.png");
            m_animateBtn->BackgroundImage = System::Drawing::Image::FromFile(resources+"animate.png");
            m_vertsBtn->BackgroundImage = System::Drawing::Image::FromFile(resources+"showverts.png");
            m_handleBtn->BackgroundImage = System::Drawing::Image::FromFile(resources+"handle.png");

            m_disabledGravity = System::Drawing::Image::FromFile(resources+"dgravity.png");
            m_enabledGravity = System::Drawing::Image::FromFile(resources+"gravity.png");
            m_gravityBtn->BackgroundImage = m_enabledGravity;
        }

        #pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
            this->m_mainPanel = (gcnew System::Windows::Forms::Panel());
            this->m_animateBtn = (gcnew System::Windows::Forms::Button());
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
            this->m_mainPanel->Controls->Add(this->m_animateBtn);
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
            // m_animateBtn
            // 
            this->m_animateBtn->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(214)), static_cast<System::Int32>(static_cast<System::Byte>(214)), 
                static_cast<System::Int32>(static_cast<System::Byte>(214)));
            this->m_animateBtn->Cursor = System::Windows::Forms::Cursors::Hand;
            this->m_animateBtn->FlatAppearance->BorderSize = 0;
            this->m_animateBtn->FlatAppearance->CheckedBackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(150)), 
                static_cast<System::Int32>(static_cast<System::Byte>(160)), static_cast<System::Int32>(static_cast<System::Byte>(255)));
            this->m_animateBtn->FlatAppearance->MouseDownBackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(150)), 
                static_cast<System::Int32>(static_cast<System::Byte>(160)), static_cast<System::Int32>(static_cast<System::Byte>(255)));
            this->m_animateBtn->FlatAppearance->MouseOverBackColor = System::Drawing::Color::DarkGray;
            this->m_animateBtn->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
            this->m_animateBtn->Location = System::Drawing::Point(12, 396);
            this->m_animateBtn->Margin = System::Windows::Forms::Padding(0);
            this->m_animateBtn->MaximumSize = System::Drawing::Size(32, 32);
            this->m_animateBtn->MinimumSize = System::Drawing::Size(32, 32);
            this->m_animateBtn->Name = L"m_animateBtn";
            this->m_animateBtn->Size = System::Drawing::Size(32, 32);
            this->m_animateBtn->TabIndex = 13;
            this->m_animateBtn->TabStop = false;
            this->m_animateBtn->UseVisualStyleBackColor = false;
            // 
            // m_removeBtn
            // 
            this->m_removeBtn->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(214)), static_cast<System::Int32>(static_cast<System::Byte>(214)), 
                static_cast<System::Int32>(static_cast<System::Byte>(214)));
            this->m_removeBtn->Cursor = System::Windows::Forms::Cursors::Hand;
            this->m_removeBtn->FlatAppearance->BorderSize = 0;
            this->m_removeBtn->FlatAppearance->CheckedBackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(150)), 
                static_cast<System::Int32>(static_cast<System::Byte>(160)), static_cast<System::Int32>(static_cast<System::Byte>(255)));
            this->m_removeBtn->FlatAppearance->MouseDownBackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(150)), 
                static_cast<System::Int32>(static_cast<System::Byte>(160)), static_cast<System::Int32>(static_cast<System::Byte>(255)));
            this->m_removeBtn->FlatAppearance->MouseOverBackColor = System::Drawing::Color::DarkGray;
            this->m_removeBtn->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
            this->m_removeBtn->Location = System::Drawing::Point(12, 428);
            this->m_removeBtn->Margin = System::Windows::Forms::Padding(0);
            this->m_removeBtn->MaximumSize = System::Drawing::Size(32, 32);
            this->m_removeBtn->MinimumSize = System::Drawing::Size(32, 32);
            this->m_removeBtn->Name = L"m_removeBtn";
            this->m_removeBtn->Size = System::Drawing::Size(32, 32);
            this->m_removeBtn->TabIndex = 12;
            this->m_removeBtn->TabStop = false;
            this->m_removeBtn->UseVisualStyleBackColor = false;
            this->m_removeBtn->Click += gcnew System::EventHandler(this, &GUIForm::RemoveClick);
            // 
            // m_scaleBtn
            // 
            this->m_scaleBtn->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(214)), static_cast<System::Int32>(static_cast<System::Byte>(214)), 
                static_cast<System::Int32>(static_cast<System::Byte>(214)));
            this->m_scaleBtn->Cursor = System::Windows::Forms::Cursors::Hand;
            this->m_scaleBtn->FlatAppearance->BorderSize = 0;
            this->m_scaleBtn->FlatAppearance->CheckedBackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(150)), 
                static_cast<System::Int32>(static_cast<System::Byte>(160)), static_cast<System::Int32>(static_cast<System::Byte>(255)));
            this->m_scaleBtn->FlatAppearance->MouseDownBackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(150)), 
                static_cast<System::Int32>(static_cast<System::Byte>(160)), static_cast<System::Int32>(static_cast<System::Byte>(255)));
            this->m_scaleBtn->FlatAppearance->MouseOverBackColor = System::Drawing::Color::DarkGray;
            this->m_scaleBtn->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
            this->m_scaleBtn->Location = System::Drawing::Point(12, 364);
            this->m_scaleBtn->Margin = System::Windows::Forms::Padding(0);
            this->m_scaleBtn->MaximumSize = System::Drawing::Size(32, 32);
            this->m_scaleBtn->MinimumSize = System::Drawing::Size(32, 32);
            this->m_scaleBtn->Name = L"m_scaleBtn";
            this->m_scaleBtn->Size = System::Drawing::Size(32, 32);
            this->m_scaleBtn->TabIndex = 11;
            this->m_scaleBtn->TabStop = false;
            this->m_scaleBtn->UseVisualStyleBackColor = false;
            // 
            // m_rotateBtn
            // 
            this->m_rotateBtn->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(214)), static_cast<System::Int32>(static_cast<System::Byte>(214)), 
                static_cast<System::Int32>(static_cast<System::Byte>(214)));
            this->m_rotateBtn->Cursor = System::Windows::Forms::Cursors::Hand;
            this->m_rotateBtn->FlatAppearance->BorderSize = 0;
            this->m_rotateBtn->FlatAppearance->CheckedBackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(150)), 
                static_cast<System::Int32>(static_cast<System::Byte>(160)), static_cast<System::Int32>(static_cast<System::Byte>(255)));
            this->m_rotateBtn->FlatAppearance->MouseDownBackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(150)), 
                static_cast<System::Int32>(static_cast<System::Byte>(160)), static_cast<System::Int32>(static_cast<System::Byte>(255)));
            this->m_rotateBtn->FlatAppearance->MouseOverBackColor = System::Drawing::Color::DarkGray;
            this->m_rotateBtn->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
            this->m_rotateBtn->Location = System::Drawing::Point(12, 332);
            this->m_rotateBtn->Margin = System::Windows::Forms::Padding(0);
            this->m_rotateBtn->MaximumSize = System::Drawing::Size(32, 32);
            this->m_rotateBtn->MinimumSize = System::Drawing::Size(32, 32);
            this->m_rotateBtn->Name = L"m_rotateBtn";
            this->m_rotateBtn->Size = System::Drawing::Size(32, 32);
            this->m_rotateBtn->TabIndex = 10;
            this->m_rotateBtn->TabStop = false;
            this->m_rotateBtn->UseVisualStyleBackColor = false;
            // 
            // m_moveBtn
            // 
            this->m_moveBtn->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(214)), static_cast<System::Int32>(static_cast<System::Byte>(214)), 
                static_cast<System::Int32>(static_cast<System::Byte>(214)));
            this->m_moveBtn->Cursor = System::Windows::Forms::Cursors::Hand;
            this->m_moveBtn->FlatAppearance->BorderSize = 0;
            this->m_moveBtn->FlatAppearance->CheckedBackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(150)), 
                static_cast<System::Int32>(static_cast<System::Byte>(160)), static_cast<System::Int32>(static_cast<System::Byte>(255)));
            this->m_moveBtn->FlatAppearance->MouseDownBackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(150)), 
                static_cast<System::Int32>(static_cast<System::Byte>(160)), static_cast<System::Int32>(static_cast<System::Byte>(255)));
            this->m_moveBtn->FlatAppearance->MouseOverBackColor = System::Drawing::Color::DarkGray;
            this->m_moveBtn->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
            this->m_moveBtn->Location = System::Drawing::Point(12, 300);
            this->m_moveBtn->Margin = System::Windows::Forms::Padding(0);
            this->m_moveBtn->MaximumSize = System::Drawing::Size(32, 32);
            this->m_moveBtn->MinimumSize = System::Drawing::Size(32, 32);
            this->m_moveBtn->Name = L"m_moveBtn";
            this->m_moveBtn->Size = System::Drawing::Size(32, 32);
            this->m_moveBtn->TabIndex = 9;
            this->m_moveBtn->TabStop = false;
            this->m_moveBtn->UseVisualStyleBackColor = false;
            // 
            // m_cynlinderBtn
            // 
            this->m_cynlinderBtn->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(214)), static_cast<System::Int32>(static_cast<System::Byte>(214)), 
                static_cast<System::Int32>(static_cast<System::Byte>(214)));
            this->m_cynlinderBtn->Cursor = System::Windows::Forms::Cursors::Hand;
            this->m_cynlinderBtn->FlatAppearance->BorderSize = 0;
            this->m_cynlinderBtn->FlatAppearance->CheckedBackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(150)), 
                static_cast<System::Int32>(static_cast<System::Byte>(160)), static_cast<System::Int32>(static_cast<System::Byte>(255)));
            this->m_cynlinderBtn->FlatAppearance->MouseDownBackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(150)), 
                static_cast<System::Int32>(static_cast<System::Byte>(160)), static_cast<System::Int32>(static_cast<System::Byte>(255)));
            this->m_cynlinderBtn->FlatAppearance->MouseOverBackColor = System::Drawing::Color::DarkGray;
            this->m_cynlinderBtn->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
            this->m_cynlinderBtn->Location = System::Drawing::Point(12, 268);
            this->m_cynlinderBtn->Margin = System::Windows::Forms::Padding(0);
            this->m_cynlinderBtn->MaximumSize = System::Drawing::Size(32, 32);
            this->m_cynlinderBtn->MinimumSize = System::Drawing::Size(32, 32);
            this->m_cynlinderBtn->Name = L"m_cynlinderBtn";
            this->m_cynlinderBtn->Size = System::Drawing::Size(32, 32);
            this->m_cynlinderBtn->TabIndex = 8;
            this->m_cynlinderBtn->TabStop = false;
            this->m_cynlinderBtn->UseVisualStyleBackColor = false;
            this->m_cynlinderBtn->Click += gcnew System::EventHandler(this, &GUIForm::CynlinderClick);
            // 
            // m_sphereBtn
            // 
            this->m_sphereBtn->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(214)), static_cast<System::Int32>(static_cast<System::Byte>(214)), 
                static_cast<System::Int32>(static_cast<System::Byte>(214)));
            this->m_sphereBtn->Cursor = System::Windows::Forms::Cursors::Hand;
            this->m_sphereBtn->FlatAppearance->BorderSize = 0;
            this->m_sphereBtn->FlatAppearance->CheckedBackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(150)), 
                static_cast<System::Int32>(static_cast<System::Byte>(160)), static_cast<System::Int32>(static_cast<System::Byte>(255)));
            this->m_sphereBtn->FlatAppearance->MouseDownBackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(150)), 
                static_cast<System::Int32>(static_cast<System::Byte>(160)), static_cast<System::Int32>(static_cast<System::Byte>(255)));
            this->m_sphereBtn->FlatAppearance->MouseOverBackColor = System::Drawing::Color::DarkGray;
            this->m_sphereBtn->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
            this->m_sphereBtn->Location = System::Drawing::Point(12, 236);
            this->m_sphereBtn->Margin = System::Windows::Forms::Padding(0);
            this->m_sphereBtn->MaximumSize = System::Drawing::Size(32, 32);
            this->m_sphereBtn->MinimumSize = System::Drawing::Size(32, 32);
            this->m_sphereBtn->Name = L"m_sphereBtn";
            this->m_sphereBtn->Size = System::Drawing::Size(32, 32);
            this->m_sphereBtn->TabIndex = 7;
            this->m_sphereBtn->TabStop = false;
            this->m_sphereBtn->UseVisualStyleBackColor = false;
            this->m_sphereBtn->Click += gcnew System::EventHandler(this, &GUIForm::SphereClick);
            // 
            // m_boxBtn
            // 
            this->m_boxBtn->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(214)), static_cast<System::Int32>(static_cast<System::Byte>(214)), 
                static_cast<System::Int32>(static_cast<System::Byte>(214)));
            this->m_boxBtn->Cursor = System::Windows::Forms::Cursors::Hand;
            this->m_boxBtn->FlatAppearance->BorderSize = 0;
            this->m_boxBtn->FlatAppearance->CheckedBackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(150)), 
                static_cast<System::Int32>(static_cast<System::Byte>(160)), static_cast<System::Int32>(static_cast<System::Byte>(255)));
            this->m_boxBtn->FlatAppearance->MouseDownBackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(150)), 
                static_cast<System::Int32>(static_cast<System::Byte>(160)), static_cast<System::Int32>(static_cast<System::Byte>(255)));
            this->m_boxBtn->FlatAppearance->MouseOverBackColor = System::Drawing::Color::DarkGray;
            this->m_boxBtn->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
            this->m_boxBtn->Location = System::Drawing::Point(12, 204);
            this->m_boxBtn->Margin = System::Windows::Forms::Padding(0);
            this->m_boxBtn->MaximumSize = System::Drawing::Size(32, 32);
            this->m_boxBtn->MinimumSize = System::Drawing::Size(32, 32);
            this->m_boxBtn->Name = L"m_boxBtn";
            this->m_boxBtn->Size = System::Drawing::Size(32, 32);
            this->m_boxBtn->TabIndex = 6;
            this->m_boxBtn->TabStop = false;
            this->m_boxBtn->UseVisualStyleBackColor = false;
            this->m_boxBtn->Click += gcnew System::EventHandler(this, &GUIForm::BoxClick);
            // 
            // m_resetCamBtn
            // 
            this->m_resetCamBtn->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(214)), static_cast<System::Int32>(static_cast<System::Byte>(214)), 
                static_cast<System::Int32>(static_cast<System::Byte>(214)));
            this->m_resetCamBtn->Cursor = System::Windows::Forms::Cursors::Hand;
            this->m_resetCamBtn->FlatAppearance->BorderSize = 0;
            this->m_resetCamBtn->FlatAppearance->CheckedBackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(150)), 
                static_cast<System::Int32>(static_cast<System::Byte>(160)), static_cast<System::Int32>(static_cast<System::Byte>(255)));
            this->m_resetCamBtn->FlatAppearance->MouseDownBackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(150)), 
                static_cast<System::Int32>(static_cast<System::Byte>(160)), static_cast<System::Int32>(static_cast<System::Byte>(255)));
            this->m_resetCamBtn->FlatAppearance->MouseOverBackColor = System::Drawing::Color::DarkGray;
            this->m_resetCamBtn->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
            this->m_resetCamBtn->Location = System::Drawing::Point(12, 172);
            this->m_resetCamBtn->Margin = System::Windows::Forms::Padding(0);
            this->m_resetCamBtn->MaximumSize = System::Drawing::Size(32, 32);
            this->m_resetCamBtn->MinimumSize = System::Drawing::Size(32, 32);
            this->m_resetCamBtn->Name = L"m_resetCamBtn";
            this->m_resetCamBtn->Size = System::Drawing::Size(32, 32);
            this->m_resetCamBtn->TabIndex = 5;
            this->m_resetCamBtn->TabStop = false;
            this->m_resetCamBtn->UseVisualStyleBackColor = false;
            this->m_resetCamBtn->Click += gcnew System::EventHandler(this, &GUIForm::ResetCamClick);
            // 
            // m_resetClothBtn
            // 
            this->m_resetClothBtn->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(214)), static_cast<System::Int32>(static_cast<System::Byte>(214)), 
                static_cast<System::Int32>(static_cast<System::Byte>(214)));
            this->m_resetClothBtn->Cursor = System::Windows::Forms::Cursors::Hand;
            this->m_resetClothBtn->FlatAppearance->BorderSize = 0;
            this->m_resetClothBtn->FlatAppearance->CheckedBackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(150)), 
                static_cast<System::Int32>(static_cast<System::Byte>(160)), static_cast<System::Int32>(static_cast<System::Byte>(255)));
            this->m_resetClothBtn->FlatAppearance->MouseDownBackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(150)), 
                static_cast<System::Int32>(static_cast<System::Byte>(160)), static_cast<System::Int32>(static_cast<System::Byte>(255)));
            this->m_resetClothBtn->FlatAppearance->MouseOverBackColor = System::Drawing::Color::DarkGray;
            this->m_resetClothBtn->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
            this->m_resetClothBtn->Location = System::Drawing::Point(12, 140);
            this->m_resetClothBtn->Margin = System::Windows::Forms::Padding(0);
            this->m_resetClothBtn->MaximumSize = System::Drawing::Size(32, 32);
            this->m_resetClothBtn->MinimumSize = System::Drawing::Size(32, 32);
            this->m_resetClothBtn->Name = L"m_resetClothBtn";
            this->m_resetClothBtn->Size = System::Drawing::Size(32, 32);
            this->m_resetClothBtn->TabIndex = 4;
            this->m_resetClothBtn->TabStop = false;
            this->m_resetClothBtn->UseVisualStyleBackColor = false;
            this->m_resetClothBtn->Click += gcnew System::EventHandler(this, &GUIForm::ResetClothClick);
            // 
            // m_unpinBtn
            // 
            this->m_unpinBtn->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(214)), static_cast<System::Int32>(static_cast<System::Byte>(214)), 
                static_cast<System::Int32>(static_cast<System::Byte>(214)));
            this->m_unpinBtn->Cursor = System::Windows::Forms::Cursors::Hand;
            this->m_unpinBtn->FlatAppearance->BorderSize = 0;
            this->m_unpinBtn->FlatAppearance->CheckedBackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(150)), 
                static_cast<System::Int32>(static_cast<System::Byte>(160)), static_cast<System::Int32>(static_cast<System::Byte>(255)));
            this->m_unpinBtn->FlatAppearance->MouseDownBackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(150)), 
                static_cast<System::Int32>(static_cast<System::Byte>(160)), static_cast<System::Int32>(static_cast<System::Byte>(255)));
            this->m_unpinBtn->FlatAppearance->MouseOverBackColor = System::Drawing::Color::DarkGray;
            this->m_unpinBtn->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
            this->m_unpinBtn->Location = System::Drawing::Point(12, 108);
            this->m_unpinBtn->Margin = System::Windows::Forms::Padding(0);
            this->m_unpinBtn->MaximumSize = System::Drawing::Size(32, 32);
            this->m_unpinBtn->MinimumSize = System::Drawing::Size(32, 32);
            this->m_unpinBtn->Name = L"m_unpinBtn";
            this->m_unpinBtn->Size = System::Drawing::Size(32, 32);
            this->m_unpinBtn->TabIndex = 3;
            this->m_unpinBtn->TabStop = false;
            this->m_unpinBtn->UseVisualStyleBackColor = false;
            this->m_unpinBtn->Click += gcnew System::EventHandler(this, &GUIForm::UnpinClick);
            // 
            // m_vertsBtn
            // 
            this->m_vertsBtn->Appearance = System::Windows::Forms::Appearance::Button;
            this->m_vertsBtn->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(214)), static_cast<System::Int32>(static_cast<System::Byte>(214)), 
                static_cast<System::Int32>(static_cast<System::Byte>(214)));
            this->m_vertsBtn->Cursor = System::Windows::Forms::Cursors::Hand;
            this->m_vertsBtn->FlatAppearance->BorderSize = 0;
            this->m_vertsBtn->FlatAppearance->CheckedBackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(150)), 
                static_cast<System::Int32>(static_cast<System::Byte>(160)), static_cast<System::Int32>(static_cast<System::Byte>(255)));
            this->m_vertsBtn->FlatAppearance->MouseDownBackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(150)), 
                static_cast<System::Int32>(static_cast<System::Byte>(160)), static_cast<System::Int32>(static_cast<System::Byte>(255)));
            this->m_vertsBtn->FlatAppearance->MouseOverBackColor = System::Drawing::Color::DarkGray;
            this->m_vertsBtn->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
            this->m_vertsBtn->Location = System::Drawing::Point(12, 76);
            this->m_vertsBtn->Margin = System::Windows::Forms::Padding(0);
            this->m_vertsBtn->MaximumSize = System::Drawing::Size(32, 32);
            this->m_vertsBtn->MinimumSize = System::Drawing::Size(32, 32);
            this->m_vertsBtn->Name = L"m_vertsBtn";
            this->m_vertsBtn->Size = System::Drawing::Size(32, 32);
            this->m_vertsBtn->TabIndex = 2;
            this->m_vertsBtn->TabStop = false;
            this->m_vertsBtn->UseVisualStyleBackColor = false;
            this->m_vertsBtn->CheckStateChanged += gcnew System::EventHandler(this, &GUIForm::VertsCheckedChanged);
            // 
            // m_handleBtn
            // 
            this->m_handleBtn->Appearance = System::Windows::Forms::Appearance::Button;
            this->m_handleBtn->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(214)), static_cast<System::Int32>(static_cast<System::Byte>(214)), 
                static_cast<System::Int32>(static_cast<System::Byte>(214)));
            this->m_handleBtn->Cursor = System::Windows::Forms::Cursors::Hand;
            this->m_handleBtn->FlatAppearance->BorderSize = 0;
            this->m_handleBtn->FlatAppearance->CheckedBackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(150)), 
                static_cast<System::Int32>(static_cast<System::Byte>(160)), static_cast<System::Int32>(static_cast<System::Byte>(255)));
            this->m_handleBtn->FlatAppearance->MouseDownBackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(150)), 
                static_cast<System::Int32>(static_cast<System::Byte>(160)), static_cast<System::Int32>(static_cast<System::Byte>(255)));
            this->m_handleBtn->FlatAppearance->MouseOverBackColor = System::Drawing::Color::DarkGray;
            this->m_handleBtn->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
            this->m_handleBtn->Location = System::Drawing::Point(12, 44);
            this->m_handleBtn->Margin = System::Windows::Forms::Padding(0);
            this->m_handleBtn->MaximumSize = System::Drawing::Size(32, 32);
            this->m_handleBtn->MinimumSize = System::Drawing::Size(32, 32);
            this->m_handleBtn->Name = L"m_handleBtn";
            this->m_handleBtn->Size = System::Drawing::Size(32, 32);
            this->m_handleBtn->TabIndex = 1;
            this->m_handleBtn->TabStop = false;
            this->m_handleBtn->UseVisualStyleBackColor = false;
            this->m_handleBtn->CheckStateChanged += gcnew System::EventHandler(this, &GUIForm::HandleCheckedChanged);
            // 
            // m_gravityBtn
            // 
            this->m_gravityBtn->Appearance = System::Windows::Forms::Appearance::Button;
            this->m_gravityBtn->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(214)), static_cast<System::Int32>(static_cast<System::Byte>(214)), 
                static_cast<System::Int32>(static_cast<System::Byte>(214)));
            this->m_gravityBtn->Cursor = System::Windows::Forms::Cursors::Hand;
            this->m_gravityBtn->FlatAppearance->BorderSize = 0;
            this->m_gravityBtn->FlatAppearance->CheckedBackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(150)), 
                static_cast<System::Int32>(static_cast<System::Byte>(160)), static_cast<System::Int32>(static_cast<System::Byte>(255)));
            this->m_gravityBtn->FlatAppearance->MouseDownBackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(150)), 
                static_cast<System::Int32>(static_cast<System::Byte>(160)), static_cast<System::Int32>(static_cast<System::Byte>(255)));
            this->m_gravityBtn->FlatAppearance->MouseOverBackColor = System::Drawing::Color::DarkGray;
            this->m_gravityBtn->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
            this->m_gravityBtn->Location = System::Drawing::Point(12, 12);
            this->m_gravityBtn->Margin = System::Windows::Forms::Padding(0);
            this->m_gravityBtn->MaximumSize = System::Drawing::Size(32, 32);
            this->m_gravityBtn->MinimumSize = System::Drawing::Size(32, 32);
            this->m_gravityBtn->Name = L"m_gravityBtn";
            this->m_gravityBtn->Size = System::Drawing::Size(32, 32);
            this->m_gravityBtn->TabIndex = 0;
            this->m_gravityBtn->TabStop = false;
            this->m_gravityBtn->UseVisualStyleBackColor = false;
            this->m_gravityBtn->CheckStateChanged += gcnew System::EventHandler(this, &GUIForm::GravityCheckedChanged);
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
            this->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &GUIForm::GUIFormClosed);
            this->m_mainPanel->ResumeLayout(false);
            this->ResumeLayout(false);

        }
        #pragma endregion
};
}
