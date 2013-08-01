/****************************************************************
* Kara Jensen (mail@karajensen.com) 
* Main GUI Window Form, usable with the designer
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

        /// <summary>
        /// Sets all native callbacks and fills in the initial spin box values
        /// </summary>
        void SetCallbacks(GuiCallbacks* callbacks)
        {
            m_callbacks = callbacks;
            m_spacing->numeric->Value = Decimal(callbacks->getSpacing());
            m_iterations->numeric->Value = Decimal(callbacks->getIterations());
            m_timestep->numeric->Value = Decimal(callbacks->getTimestep());
            m_vertRows->numeric->Value = Decimal(callbacks->getVertexRows());
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

        /// <summary>
        /// Reference structure for the gui spinbox
        /// </summary>
        ref struct SpinBox
        {
            Panel^ panel;
            NumericUpDown^ numeric;
        };

        GuiCallbacks* m_callbacks;      ///< Callbacks for the gui
        gcroot<Form^>* m_pinnedSimForm; ///< pinned as native needs window handle
        Image^ m_disabledGravity;       ///< Image for disabled gravity
        Image^ m_enabledGravity;        ///< Image for enabled gravity
        RadioButton^ m_toolSelected;    ///< Which object manipulation tool selected

        /// <summary>
        /// GUI Buttons
        /// </summary>
        CheckBox^ m_gravityBtn;    ///< Checkbox for toggling on gravity
        CheckBox^ m_vertsBtn;      ///< Checkbox for toggling on vertices
        CheckBox^ m_handleBtn;     ///< Checkbox for toggling on handle mode
        CheckBox^ m_wireBtn;       ///< Checkbox for toggling on wireframe mode
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
        SpinBox^ m_vertRows;       ///< Spinbox for changing vertex row number
        SpinBox^ m_iterations;     ///< Spinbox for changing iterations
        SpinBox^ m_timestep;       ///< Spinbox for changing the timestep
        SpinBox^ m_spacing;        ///< Spinbox for changing the spacing between vertices

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
            simulatorForm->BackColor = System::Drawing::Color::FromArgb(190, 190, 195);

            m_pinnedSimForm = new gcroot<Form^>(simulatorForm);
            m_mainPanel->Controls->Add((Form^)*m_pinnedSimForm);
        }

        /// <summary>
        /// Creates/adds a checkbox button
        /// </summary>
        void CreateCheckBox(CheckBox^% checkbox, String^ image, String^ tip, int index, EventHandler^ callback)
        {
            checkbox = gcnew CheckBox();
            checkbox->Appearance = System::Windows::Forms::Appearance::Button;
            checkbox->CheckStateChanged += callback;
            CreateControl(checkbox, image, tip, index);
        }

        /// <summary>
        /// Creates/adds a radio button
        /// </summary>
        void CreateRadioButton(RadioButton^% button, String^ image, String^ tip, int index, EventHandler^ callback)
        {
            button = gcnew RadioButton();
            button->Appearance = System::Windows::Forms::Appearance::Button;
            button->Click += callback;
            CreateControl(button, image, tip, index);
        }

        /// <summary>
        /// Creates/adds a button
        /// </summary>
        void CreateButton(Button^% button, String^ image, String^ tip, int index, EventHandler^ callback)
        {
            button = gcnew Button();
            button->Click += callback;
            CreateControl(button, image, tip, index);
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
        /// On Gravity Check Change
        /// </summary>
        System::Void WireCheckedChanged(System::Object^ sender, System::EventArgs^ e) 
        {
            CheckBox^ checkbox = static_cast<CheckBox^>(sender);
            m_callbacks->setWireframeMode(checkbox->Checked);
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
            m_callbacks->createBox();
        }

        /// <summary>
        /// On Create Sphere Button Press
        /// </summary>
        System::Void SphereClick(System::Object^ sender, System::EventArgs^ e)
        {
            m_callbacks->createSphere();
        }

        /// <summary>
        /// On Create Cylinder Button Press
        /// </summary>
        System::Void CynlinderClick(System::Object^ sender, System::EventArgs^ e) 
        {
            m_callbacks->createCylinder();
        }

        /// <summary>
        /// On Clear Scene Button Press
        /// </summary>
        System::Void RemoveClick(System::Object^  sender, System::EventArgs^ e)
        {
        }

        /// <summary>
        /// On Vertex Row Number Value Changed
        /// </summary>
        System::Void VertexRowsChanged(System::Object^ sender, System::EventArgs^ e)
        {
            NumericUpDown^ spinbox = static_cast<NumericUpDown^>(sender);
            m_handleBtn->Checked = false;
            m_callbacks->setHandleMode(false);
            m_callbacks->setVertexRows(Decimal::ToDouble(spinbox->Value));
        }

        /// <summary>
        /// On Iteration Value Changed
        /// </summary>
        System::Void IterationChanged(System::Object^ sender, System::EventArgs^ e)
        {
            NumericUpDown^ spinbox = static_cast<NumericUpDown^>(sender);
            m_callbacks->setIterations(Decimal::ToDouble(spinbox->Value));
        }

        /// <summary>
        /// On Timestep Value Changed
        /// </summary>
        System::Void TimestepChanged(System::Object^ sender, System::EventArgs^ e)
        {
            NumericUpDown^ spinbox = static_cast<NumericUpDown^>(sender);
            m_callbacks->setTimestep(Decimal::ToDouble(spinbox->Value));
        }

        /// <summary>
        /// On Cloth Spacing Value Changed
        /// </summary>
        System::Void SpacingChanged(System::Object^ sender, System::EventArgs^ e)
        {
            NumericUpDown^ spinbox = static_cast<NumericUpDown^>(sender);
            m_handleBtn->Checked = false;
            m_callbacks->setHandleMode(false);
            m_callbacks->setSpacing(Decimal::ToDouble(spinbox->Value));
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

            CreateCheckBox(m_gravityBtn, path+"gravity.png", "Add gravity to the simulation", 
                index++, gcnew System::EventHandler(this, &GUIForm::GravityCheckedChanged));

            CreateCheckBox(m_handleBtn, path+"handle.png", "Switch to handle mode", 
                index++, gcnew System::EventHandler(this, &GUIForm::HandleCheckedChanged));

            CreateButton(m_unpinBtn, path+"unpin.png", "Unpin all pinned vertices", 
                index++, gcnew System::EventHandler(this, &GUIForm::UnpinClick));

            CreateCheckBox(m_vertsBtn, path+"showverts.png", "Toggle vertex visibility", 
                index++, gcnew System::EventHandler(this, &GUIForm::VertsCheckedChanged));

            CreateCheckBox(m_wireBtn, path+"wire.png", "Switch to wireframe mode", 
                index++, gcnew System::EventHandler(this, &GUIForm::WireCheckedChanged));

            CreateButton(m_resetClothBtn, path+"resetcloth.png", "Reset the cloth", 
                index++, gcnew System::EventHandler(this, &GUIForm::ResetClothClick));

            CreateButton(m_resetCamBtn, path+"resetcam.png", "Reset the camera", 
                index++, gcnew System::EventHandler(this, &GUIForm::ResetCamClick));

            CreateButton(m_boxBtn, path+"box.png", "Create a box",
                index++, gcnew System::EventHandler(this, &GUIForm::BoxClick));

            CreateButton(m_sphereBtn, path+"sphere.png", "Create a sphere", 
                index++, gcnew System::EventHandler(this, &GUIForm::SphereClick));

            CreateButton(m_cynlinderBtn, path+"cylinder.png", "Create a cylinder",
                index++, gcnew System::EventHandler(this, &GUIForm::CynlinderClick));

            CreateRadioButton(m_animateBtn, path+"animate.png", "Animate selected object",
                index++, gcnew System::EventHandler(this, &GUIForm::AnimateClick));

            CreateRadioButton(m_moveBtn, path+"move.png", "Move selected object",
                index++, gcnew System::EventHandler(this, &GUIForm::MoveClick));

            CreateRadioButton(m_rotateBtn, path+"rotate.png", "Rotate selected object",
                index++, gcnew System::EventHandler(this, &GUIForm::RotateClick));

            CreateRadioButton(m_scaleBtn, path+"scale.png", "Scale selected object",
                index++, gcnew System::EventHandler(this, &GUIForm::ScaleClick));

            CreateButton(m_removeBtn, path+"clear.png", "Clear all scene objects",
                index++, gcnew System::EventHandler(this, &GUIForm::RemoveClick));

            m_disabledGravity = Image::FromFile(path+"dgravity.png");
            m_enabledGravity = m_gravityBtn->BackgroundImage;
            index = 0;

            CreateSpinBox(m_vertRows, path+"vertrows.png",
                "Change the number of vertex rows", index++, 2.0, 4.0, 50.0,
                gcnew System::EventHandler(this, &GUIForm::VertexRowsChanged));

            CreateSpinBox(m_iterations, path+"iterations.png", 
                "Change the number of solve iterations", index++, 1.0, 1.0, 50.0,
                gcnew System::EventHandler(this, &GUIForm::IterationChanged));

            CreateSpinBox(m_timestep, path+"timestep.png",  
                "Change the cloth timestep", index++, 0.01, 0.1, 50.0,
                gcnew System::EventHandler(this, &GUIForm::TimestepChanged));

            CreateSpinBox(m_spacing, path+"spacing.png", 
                "Change the spacing between vertices", index++, 0.05, 0.1, 10.0,
                gcnew System::EventHandler(this, &GUIForm::SpacingChanged));
        }

        /// <summary>
        /// Creates a gui control
        /// </summary>
        void CreateControl(ButtonBase^ control, String^ image, String^ tip, int index)
        {
            const int buttonSize = 32;
            const int border = 2;

            m_guiPanel->Controls->Add(control);
            control->TabIndex = 0;
            control->TabStop = false;
            control->UseVisualStyleBackColor = false;
            control->BackgroundImage = Image::FromFile(image);
            control->Cursor = System::Windows::Forms::Cursors::Hand;
            control->Location = System::Drawing::Point(border, (index*buttonSize)+border);
            control->Margin = System::Windows::Forms::Padding(0);
            control->MaximumSize = System::Drawing::Size(buttonSize, buttonSize);
            control->MinimumSize = System::Drawing::Size(buttonSize, buttonSize);
            control->Size = System::Drawing::Size(buttonSize, buttonSize);
            control->FlatAppearance->BorderSize = 0;
            control->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
            control->FlatAppearance->CheckedBackColor = System::Drawing::Color::Gray;
            control->FlatAppearance->MouseDownBackColor = Color::Gray;
            control->FlatAppearance->MouseOverBackColor = Color::FromArgb(150, 150, 240);
            control->BackColor = Color::FromArgb(230, 230, 230);

            ToolTip^ tooltip = gcnew ToolTip();
            tooltip->SetToolTip(control, tip);
        }

        /// <summary>
        /// Creates a spinbox control
        /// </summary>
        void CreateSpinBox(SpinBox^% spinbox, String^ image, String^ tip, int index,
            double increment, double minimum, double maximum, EventHandler^ callback)
        {
            const int controlSize = 32;
            const int panelX = 690;
            const int panelY = 466 + (index*controlSize);
            const int panelHeight = controlSize-4;
            const int numericX = panelX + controlSize + 3;
            const int numericY = panelY + 5;

            spinbox = gcnew SpinBox();
            spinbox->panel = gcnew Panel();
            spinbox->numeric = gcnew NumericUpDown();

            Form^ simForm = (Form^)*m_pinnedSimForm;
            simForm->Controls->Add(spinbox->panel);
            simForm->Controls->Add(spinbox->numeric);
            
            spinbox->panel->BackgroundImage = Image::FromFile(image);
            spinbox->panel->Location = System::Drawing::Point(panelX, panelY+2);
            spinbox->panel->Margin = System::Windows::Forms::Padding(0);
            spinbox->panel->MaximumSize = System::Drawing::Size(controlSize, panelHeight);
            spinbox->panel->MinimumSize = System::Drawing::Size(controlSize, panelHeight);
            spinbox->panel->Size = System::Drawing::Size(controlSize, panelHeight);
            spinbox->panel->TabIndex = 0;
            spinbox->panel->TabStop = false;

            spinbox->numeric->TabIndex = 0;
            spinbox->numeric->TabStop = false;
            spinbox->numeric->DecimalPlaces = 3;
            spinbox->numeric->Location = System::Drawing::Point(numericX, numericY);
            spinbox->numeric->Size = System::Drawing::Size(65, 22);
            spinbox->numeric->ReadOnly = true;
            spinbox->numeric->InterceptArrowKeys = false;
            spinbox->numeric->BackColor = System::Drawing::Color::Silver;
            spinbox->numeric->Increment = Decimal(increment);
            spinbox->numeric->Minimum = Decimal(minimum);
            spinbox->numeric->Maximum = Decimal(maximum);
            spinbox->numeric->ValueChanged += callback;

            ToolTip^ tooltip = gcnew ToolTip();
            tooltip->SetToolTip(spinbox->panel, tip);
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
            this->m_guiPanel->BackColor = System::Drawing::Color::Gray;
            this->m_guiPanel->Location = System::Drawing::Point(12, 12);
            this->m_guiPanel->Name = L"m_guiPanel";
            this->m_guiPanel->Size = System::Drawing::Size(36, 484);
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
