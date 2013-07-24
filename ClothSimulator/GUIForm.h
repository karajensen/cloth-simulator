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

		/// <summary>
		/// Constructor
		/// </summary>
		GUIForm(void) :
            m_callbacks(nullptr),
            m_toolSelected(nullptr),
            m_pinnedSimForm(nullptr)
		{
			InitializeComponent();
            CreateSimulationForm();
            CreateButtons();
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
            Form^ simForm = (Form^)*m_pinnedSimForm;
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
                gcroot<Form^>* ptr = reinterpret_cast<gcroot<Form^>*>(m_pinnedSimForm);
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
        gcroot<Form^>* m_pinnedSimForm; ///< pinned as native needs window handle
        Image^ m_disabledGravity; ///< Image for disabled gravity
        Image^ m_enabledGravity; ///< Image for enabled gravity
        RadioButton^ m_toolSelected; ///< Which object manipulation tool selected

        /// <summary>
		/// GUI Buttons
		/// </summary>
        CheckBox^ m_gravityBtn;    ///< Checkbox for toggling on gravity
        CheckBox^ m_vertsBtn;      ///< Checkbox for toggling on vertices
        CheckBox^ m_handleBtn;     ///< Checkbox for toggling on handle mode
        RadioButton^ m_scaleBtn;   ///< Checkbox for scaling the selected scene object
        RadioButton^ m_rotateBtn;  ///< Checkbox for rotating the selected scene object
        RadioButton^ m_moveBtn;    ///< Checkbox for moving the selected scene object
        RadioButton^ m_animateBtn; ///< Checkbox for animating an object
        Button^ m_resetCamBtn;     ///< Button for resetting the camera
        Button^ m_resetClothBtn;   ///< Button for resetting the cloth
        Button^ m_unpinBtn;        ///< Button for upinning the cloth
        Button^ m_cynlinderBtn;    ///< Button for creating a cynlinder
        Button^ m_sphereBtn;       ///< Button for creating a sphere
        Button^ m_boxBtn;          ///< Button for creating a box
        Button^ m_removeBtn;       ///< Button for clearing all scene objects

		/// <summary>
		/// Designer specific components
		/// </summary>
        System::ComponentModel::Container^ components;  ///< Auto-Required designer variable.
        System::Windows::Forms::Panel^ m_mainPanel;     ///< Panel for holding main simulation form
        System::Windows::Forms::Panel^ m_guiPanel;      ///< Panel for gui buttons

		/// <summary>
		/// Creates the main simulation form
		/// </summary>
        void CreateSimulationForm()
        {
            Form^ simulatorForm = gcnew Form();
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

            m_pinnedSimForm = new gcroot<Form^>(simulatorForm);
            m_mainPanel->Controls->Add((Form^)*m_pinnedSimForm);
        }

		/// <summary>
		/// Creates a checkbox button
		/// </summary>
        void CreateCheckBox(CheckBox^% checkbox, String^ image, int index, EventHandler^ callback)
        {
            checkbox = gcnew CheckBox();
            checkbox->Appearance = System::Windows::Forms::Appearance::Button;
            checkbox->CheckStateChanged += callback;
            CreateControl(checkbox, image, index);
        }

		/// <summary>
		/// Creates a radio button
		/// </summary>
        void CreateRadioButton(RadioButton^% button, String^ image, int index, EventHandler^ callback)
        {
            button = gcnew RadioButton();
            button->Appearance = System::Windows::Forms::Appearance::Button;
            button->Click += callback;
            CreateControl(button, image, index);
        }

		/// <summary>
		/// Creates a button
		/// </summary>
        void CreateButton(Button^% button, String^ image, int index, EventHandler^ callback)
        {
            button = gcnew Button();
            button->Click += callback;
            CreateControl(button, image, index);
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
            CheckBox^ checkbox = static_cast<CheckBox^>(sender);
            m_callbacks->setGravity(checkbox->Checked);
        }

		/// <summary>
		/// On Handle Mode Check Change
		/// </summary>
        System::Void HandleCheckedChanged(System::Object^ sender, System::EventArgs^ e) 
        {
            CheckBox^ checkbox = static_cast<CheckBox^>(sender);
            m_callbacks->setHandleMode(checkbox->Checked);

            m_gravityBtn->Enabled = !checkbox->Checked;
            m_gravityBtn->BackgroundImage = checkbox->Checked 
                ? m_disabledGravity : m_enabledGravity;
            
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
            CheckBox^ checkbox = static_cast<CheckBox^>(sender);
            m_callbacks->setVertsVisible(checkbox->Checked);
        }

		/// <summary>
		/// On Animate Button Press
		/// </summary>
        System::Void AnimateClick(System::Object^ sender, System::EventArgs^ e) 
        {
            RadioButton^ button = static_cast<RadioButton^>(sender);
            SelectRadioButton(button);
        }

		/// <summary>
		/// On Move Button Press
		/// </summary>
        System::Void MoveClick(System::Object^ sender, System::EventArgs^ e) 
        {         
            RadioButton^ button = static_cast<RadioButton^>(sender);
            SelectRadioButton(button);
        }

		/// <summary>
		/// On Rotate Button Press
		/// </summary>
        System::Void RotateClick(System::Object^ sender, System::EventArgs^ e) 
        {
            RadioButton^ button = static_cast<RadioButton^>(sender);
            SelectRadioButton(button);
        }

		/// <summary>
		/// On Scale Button Press
		/// </summary>
        System::Void ScaleClick(System::Object^ sender, System::EventArgs^ e) 
        {
            RadioButton^ button = static_cast<RadioButton^>(sender);
            SelectRadioButton(button);
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
		/// Chooses the selected radio button
		/// </summary>
        void SelectRadioButton(RadioButton^ button)
        {
            if(m_toolSelected == button)
            {
                button->Checked = false;
                m_toolSelected = nullptr;
            }
            else
            {
                m_toolSelected = button;
            }
        }

		/// <summary>
		/// Creates the gui buttons
		/// </summary>
        void CreateButtons()
        {
            int index = 0;
            String^ path = "Resources//Sprites//";

            CreateCheckBox(m_gravityBtn, path+"gravity.png", index++,
                gcnew System::EventHandler(this, &GUIForm::GravityCheckedChanged));

            CreateCheckBox(m_handleBtn, path+"handle.png", index++,
                gcnew System::EventHandler(this, &GUIForm::HandleCheckedChanged));

            CreateCheckBox(m_vertsBtn, path+"showverts.png", index++,
                gcnew System::EventHandler(this, &GUIForm::VertsCheckedChanged));

            CreateButton(m_unpinBtn, path+"unpin.png", index++,
                gcnew System::EventHandler(this, &GUIForm::UnpinClick));

            CreateButton(m_resetClothBtn, path+"resetcloth.png", index++,
                gcnew System::EventHandler(this, &GUIForm::ResetClothClick));

            CreateButton(m_resetCamBtn, path+"resetcam.png", index++,
                gcnew System::EventHandler(this, &GUIForm::ResetCamClick));

            CreateButton(m_boxBtn, path+"box.png", index++,
                gcnew System::EventHandler(this, &GUIForm::BoxClick));

            CreateButton(m_sphereBtn, path+"sphere.png", index++,
                gcnew System::EventHandler(this, &GUIForm::SphereClick));

            CreateButton(m_cynlinderBtn, path+"cylinder.png", index++,
                gcnew System::EventHandler(this, &GUIForm::CynlinderClick));

            CreateRadioButton(m_animateBtn, path+"animate.png", index++,
                gcnew System::EventHandler(this, &GUIForm::AnimateClick));

            CreateRadioButton(m_moveBtn, path+"move.png", index++,
                gcnew System::EventHandler(this, &GUIForm::MoveClick));

            CreateRadioButton(m_rotateBtn, path+"rotate.png", index++,
                gcnew System::EventHandler(this, &GUIForm::RotateClick));

            CreateRadioButton(m_scaleBtn, path+"scale.png", index++,
                gcnew System::EventHandler(this, &GUIForm::ScaleClick));

            CreateButton(m_removeBtn, path+"clear.png", index++,
                gcnew System::EventHandler(this, &GUIForm::RemoveClick));

            // Setup disabled images
            m_disabledGravity = Image::FromFile(path+"dgravity.png");
            m_enabledGravity = m_gravityBtn->BackgroundImage;
        }

		/// <summary>
		/// Creates a gui control
		/// </summary>
        void CreateControl(ButtonBase^ control, String^ image, int index)
        {
            const int border = 2;
            m_guiPanel->Controls->Add(control);
            control->BackgroundImage = Image::FromFile(image);
            control->Cursor = System::Windows::Forms::Cursors::Hand;
            control->Location = System::Drawing::Point(border, (index*32)+border);
            control->Margin = System::Windows::Forms::Padding(0);
            control->MaximumSize = System::Drawing::Size(32, 32);
            control->MinimumSize = System::Drawing::Size(32, 32);
            control->Size = System::Drawing::Size(32, 32);
            control->TabIndex = 0;
            control->TabStop = false;
            control->UseVisualStyleBackColor = false;
            control->FlatAppearance->BorderSize = 0;
            control->FlatAppearance->MouseOverBackColor = System::Drawing::Color::DarkGray;
            control->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
            control->BackColor = System::Drawing::Color::FromArgb(
                static_cast<System::Int32>(static_cast<System::Byte>(214)),
                static_cast<System::Int32>(static_cast<System::Byte>(214)), 
                static_cast<System::Int32>(static_cast<System::Byte>(214))); 
            control->FlatAppearance->CheckedBackColor = System::Drawing::Color::FromArgb(
                static_cast<System::Int32>(static_cast<System::Byte>(150)), 
                static_cast<System::Int32>(static_cast<System::Byte>(160)),
                static_cast<System::Int32>(static_cast<System::Byte>(255)));
            control->FlatAppearance->MouseDownBackColor = System::Drawing::Color::FromArgb(
                static_cast<System::Int32>(static_cast<System::Byte>(150)), 
                static_cast<System::Int32>(static_cast<System::Byte>(160)), 
                static_cast<System::Int32>(static_cast<System::Byte>(255)));
        }

        #pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
            this->m_mainPanel = (gcnew System::Windows::Forms::Panel());
            this->m_guiPanel = (gcnew System::Windows::Forms::Panel());
            this->m_mainPanel->SuspendLayout();
            this->SuspendLayout();
            // 
            // m_mainPanel
            // 
            this->m_mainPanel->Controls->Add(this->m_guiPanel);
            this->m_mainPanel->Location = System::Drawing::Point(0, 0);
            this->m_mainPanel->Name = L"m_mainPanel";
            this->m_mainPanel->Size = System::Drawing::Size(800, 600);
            this->m_mainPanel->TabIndex = 0;
            // 
            // m_guiPanel
            // 
            this->m_guiPanel->BackColor = System::Drawing::Color::DimGray;
            this->m_guiPanel->Location = System::Drawing::Point(12, 12);
            this->m_guiPanel->Name = L"m_guiPanel";
            this->m_guiPanel->Size = System::Drawing::Size(36, 452);
            this->m_guiPanel->TabIndex = 14;
            // 
            // GUIForm
            // 
            this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->BackColor = System::Drawing::Color::DimGray;
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
