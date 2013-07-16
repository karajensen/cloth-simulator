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

		/// <summary>
		/// Designed form components
		/// </summary>
        System::Windows::Forms::SplitContainer^  splitContainer1;
    private: System::Windows::Forms::Button^  gravityBtn;

    private: System::Windows::Forms::Button^  button4;
    private: System::Windows::Forms::Button^  button3;
    private: System::Windows::Forms::Button^  unpinAllBtn;

    private: System::Windows::Forms::Button^  button16;
    private: System::Windows::Forms::Button^  button15;
    private: System::Windows::Forms::Button^  button14;




    private: System::Windows::Forms::Button^  button9;
    private: System::Windows::Forms::Button^  button8;
    private: System::Windows::Forms::Button^  button7;
    private: System::Windows::Forms::Button^  button6;
    private: System::Windows::Forms::Button^  button5;
private: System::Windows::Forms::Label^  label5;
private: System::Windows::Forms::Button^  button21;
private: System::Windows::Forms::Button^  button22;
private: System::Windows::Forms::Label^  label4;
private: System::Windows::Forms::Button^  button19;
private: System::Windows::Forms::Button^  button20;
private: System::Windows::Forms::Label^  label3;
private: System::Windows::Forms::Button^  button17;
private: System::Windows::Forms::Button^  button18;
private: System::Windows::Forms::Label^  label2;
private: System::Windows::Forms::Button^  button12;
private: System::Windows::Forms::Button^  button13;
private: System::Windows::Forms::Label^  label1;
private: System::Windows::Forms::Button^  button11;
private: System::Windows::Forms::Button^  button10;
private: System::Windows::Forms::Button^  button1;
private: System::Windows::Forms::Button^  button2;





























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
            this->button16 = (gcnew System::Windows::Forms::Button());
            this->button15 = (gcnew System::Windows::Forms::Button());
            this->button14 = (gcnew System::Windows::Forms::Button());
            this->button9 = (gcnew System::Windows::Forms::Button());
            this->button8 = (gcnew System::Windows::Forms::Button());
            this->button7 = (gcnew System::Windows::Forms::Button());
            this->button6 = (gcnew System::Windows::Forms::Button());
            this->button5 = (gcnew System::Windows::Forms::Button());
            this->button4 = (gcnew System::Windows::Forms::Button());
            this->button3 = (gcnew System::Windows::Forms::Button());
            this->unpinAllBtn = (gcnew System::Windows::Forms::Button());
            this->gravityBtn = (gcnew System::Windows::Forms::Button());
            this->button10 = (gcnew System::Windows::Forms::Button());
            this->button11 = (gcnew System::Windows::Forms::Button());
            this->label1 = (gcnew System::Windows::Forms::Label());
            this->label2 = (gcnew System::Windows::Forms::Label());
            this->button12 = (gcnew System::Windows::Forms::Button());
            this->button13 = (gcnew System::Windows::Forms::Button());
            this->label3 = (gcnew System::Windows::Forms::Label());
            this->button17 = (gcnew System::Windows::Forms::Button());
            this->button18 = (gcnew System::Windows::Forms::Button());
            this->label4 = (gcnew System::Windows::Forms::Label());
            this->button19 = (gcnew System::Windows::Forms::Button());
            this->button20 = (gcnew System::Windows::Forms::Button());
            this->label5 = (gcnew System::Windows::Forms::Label());
            this->button21 = (gcnew System::Windows::Forms::Button());
            this->button22 = (gcnew System::Windows::Forms::Button());
            this->button1 = (gcnew System::Windows::Forms::Button());
            this->button2 = (gcnew System::Windows::Forms::Button());
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
            this->splitContainer1->Panel1->Controls->Add(this->button2);
            this->splitContainer1->Panel1->Controls->Add(this->label5);
            this->splitContainer1->Panel1->Controls->Add(this->button21);
            this->splitContainer1->Panel1->Controls->Add(this->button22);
            this->splitContainer1->Panel1->Controls->Add(this->label4);
            this->splitContainer1->Panel1->Controls->Add(this->button19);
            this->splitContainer1->Panel1->Controls->Add(this->button20);
            this->splitContainer1->Panel1->Controls->Add(this->label3);
            this->splitContainer1->Panel1->Controls->Add(this->button17);
            this->splitContainer1->Panel1->Controls->Add(this->button18);
            this->splitContainer1->Panel1->Controls->Add(this->label2);
            this->splitContainer1->Panel1->Controls->Add(this->button12);
            this->splitContainer1->Panel1->Controls->Add(this->button13);
            this->splitContainer1->Panel1->Controls->Add(this->label1);
            this->splitContainer1->Panel1->Controls->Add(this->button11);
            this->splitContainer1->Panel1->Controls->Add(this->button10);
            this->splitContainer1->Panel1->Controls->Add(this->button16);
            this->splitContainer1->Panel1->Controls->Add(this->button15);
            this->splitContainer1->Panel1->Controls->Add(this->button14);
            this->splitContainer1->Panel1->Controls->Add(this->button9);
            this->splitContainer1->Panel1->Controls->Add(this->button8);
            this->splitContainer1->Panel1->Controls->Add(this->button7);
            this->splitContainer1->Panel1->Controls->Add(this->button6);
            this->splitContainer1->Panel1->Controls->Add(this->button5);
            this->splitContainer1->Panel1->Controls->Add(this->button4);
            this->splitContainer1->Panel1->Controls->Add(this->button3);
            this->splitContainer1->Panel1->Controls->Add(this->unpinAllBtn);
            this->splitContainer1->Panel1->Controls->Add(this->gravityBtn);
            this->splitContainer1->Size = System::Drawing::Size(912, 600);
            this->splitContainer1->SplitterDistance = 101;
            this->splitContainer1->TabIndex = 0;
            // 
            // button16
            // 
            this->button16->FlatStyle = System::Windows::Forms::FlatStyle::System;
            this->button16->Location = System::Drawing::Point(50, 169);
            this->button16->MaximumSize = System::Drawing::Size(32, 32);
            this->button16->MinimumSize = System::Drawing::Size(32, 32);
            this->button16->Name = L"button16";
            this->button16->Size = System::Drawing::Size(32, 32);
            this->button16->TabIndex = 15;
            this->button16->TabStop = false;
            this->button16->UseVisualStyleBackColor = true;
            // 
            // button15
            // 
            this->button15->FlatStyle = System::Windows::Forms::FlatStyle::System;
            this->button15->Location = System::Drawing::Point(50, 131);
            this->button15->MaximumSize = System::Drawing::Size(32, 32);
            this->button15->MinimumSize = System::Drawing::Size(32, 32);
            this->button15->Name = L"button15";
            this->button15->Size = System::Drawing::Size(32, 32);
            this->button15->TabIndex = 14;
            this->button15->TabStop = false;
            this->button15->UseVisualStyleBackColor = true;
            // 
            // button14
            // 
            this->button14->FlatStyle = System::Windows::Forms::FlatStyle::System;
            this->button14->Location = System::Drawing::Point(50, 93);
            this->button14->MaximumSize = System::Drawing::Size(32, 32);
            this->button14->MinimumSize = System::Drawing::Size(32, 32);
            this->button14->Name = L"button14";
            this->button14->Size = System::Drawing::Size(32, 32);
            this->button14->TabIndex = 13;
            this->button14->TabStop = false;
            this->button14->UseVisualStyleBackColor = true;
            // 
            // button9
            // 
            this->button9->FlatStyle = System::Windows::Forms::FlatStyle::System;
            this->button9->Location = System::Drawing::Point(50, 207);
            this->button9->MaximumSize = System::Drawing::Size(32, 32);
            this->button9->MinimumSize = System::Drawing::Size(32, 32);
            this->button9->Name = L"button9";
            this->button9->Size = System::Drawing::Size(32, 32);
            this->button9->TabIndex = 8;
            this->button9->TabStop = false;
            this->button9->UseVisualStyleBackColor = true;
            // 
            // button8
            // 
            this->button8->FlatStyle = System::Windows::Forms::FlatStyle::System;
            this->button8->Location = System::Drawing::Point(12, 207);
            this->button8->MaximumSize = System::Drawing::Size(32, 32);
            this->button8->MinimumSize = System::Drawing::Size(32, 32);
            this->button8->Name = L"button8";
            this->button8->Size = System::Drawing::Size(32, 32);
            this->button8->TabIndex = 7;
            this->button8->TabStop = false;
            this->button8->UseVisualStyleBackColor = true;
            // 
            // button7
            // 
            this->button7->FlatStyle = System::Windows::Forms::FlatStyle::System;
            this->button7->Location = System::Drawing::Point(12, 169);
            this->button7->MaximumSize = System::Drawing::Size(32, 32);
            this->button7->MinimumSize = System::Drawing::Size(32, 32);
            this->button7->Name = L"button7";
            this->button7->Size = System::Drawing::Size(32, 32);
            this->button7->TabIndex = 6;
            this->button7->TabStop = false;
            this->button7->UseVisualStyleBackColor = true;
            // 
            // button6
            // 
            this->button6->FlatStyle = System::Windows::Forms::FlatStyle::System;
            this->button6->Location = System::Drawing::Point(12, 131);
            this->button6->MaximumSize = System::Drawing::Size(32, 32);
            this->button6->MinimumSize = System::Drawing::Size(32, 32);
            this->button6->Name = L"button6";
            this->button6->Size = System::Drawing::Size(32, 32);
            this->button6->TabIndex = 5;
            this->button6->TabStop = false;
            this->button6->UseVisualStyleBackColor = true;
            // 
            // button5
            // 
            this->button5->FlatStyle = System::Windows::Forms::FlatStyle::System;
            this->button5->Location = System::Drawing::Point(12, 93);
            this->button5->MaximumSize = System::Drawing::Size(32, 32);
            this->button5->MinimumSize = System::Drawing::Size(32, 32);
            this->button5->Name = L"button5";
            this->button5->Size = System::Drawing::Size(32, 32);
            this->button5->TabIndex = 4;
            this->button5->TabStop = false;
            this->button5->UseVisualStyleBackColor = true;
            // 
            // button4
            // 
            this->button4->FlatStyle = System::Windows::Forms::FlatStyle::System;
            this->button4->Location = System::Drawing::Point(50, 55);
            this->button4->MaximumSize = System::Drawing::Size(32, 32);
            this->button4->MinimumSize = System::Drawing::Size(32, 32);
            this->button4->Name = L"button4";
            this->button4->Size = System::Drawing::Size(32, 32);
            this->button4->TabIndex = 3;
            this->button4->TabStop = false;
            this->button4->UseVisualStyleBackColor = true;
            // 
            // button3
            // 
            this->button3->FlatStyle = System::Windows::Forms::FlatStyle::System;
            this->button3->Location = System::Drawing::Point(12, 55);
            this->button3->MaximumSize = System::Drawing::Size(32, 32);
            this->button3->MinimumSize = System::Drawing::Size(32, 32);
            this->button3->Name = L"button3";
            this->button3->Size = System::Drawing::Size(32, 32);
            this->button3->TabIndex = 2;
            this->button3->TabStop = false;
            this->button3->UseVisualStyleBackColor = true;
            // 
            // unpinAllBtn
            // 
            this->unpinAllBtn->FlatStyle = System::Windows::Forms::FlatStyle::System;
            this->unpinAllBtn->Location = System::Drawing::Point(50, 17);
            this->unpinAllBtn->MaximumSize = System::Drawing::Size(32, 32);
            this->unpinAllBtn->MinimumSize = System::Drawing::Size(32, 32);
            this->unpinAllBtn->Name = L"unpinAllBtn";
            this->unpinAllBtn->Size = System::Drawing::Size(32, 32);
            this->unpinAllBtn->TabIndex = 1;
            this->unpinAllBtn->TabStop = false;
            this->unpinAllBtn->UseVisualStyleBackColor = true;
            // 
            // gravityBtn
            // 
            this->gravityBtn->FlatStyle = System::Windows::Forms::FlatStyle::System;
            this->gravityBtn->Location = System::Drawing::Point(12, 17);
            this->gravityBtn->MaximumSize = System::Drawing::Size(32, 32);
            this->gravityBtn->MinimumSize = System::Drawing::Size(32, 32);
            this->gravityBtn->Name = L"gravityBtn";
            this->gravityBtn->Size = System::Drawing::Size(32, 32);
            this->gravityBtn->TabIndex = 0;
            this->gravityBtn->TabStop = false;
            this->gravityBtn->UseVisualStyleBackColor = true;
            // 
            // button10
            // 
            this->button10->FlatStyle = System::Windows::Forms::FlatStyle::System;
            this->button10->Location = System::Drawing::Point(12, 320);
            this->button10->MaximumSize = System::Drawing::Size(32, 32);
            this->button10->MinimumSize = System::Drawing::Size(32, 32);
            this->button10->Name = L"button10";
            this->button10->Size = System::Drawing::Size(32, 32);
            this->button10->TabIndex = 16;
            this->button10->TabStop = false;
            this->button10->UseVisualStyleBackColor = true;
            // 
            // button11
            // 
            this->button11->FlatStyle = System::Windows::Forms::FlatStyle::System;
            this->button11->Location = System::Drawing::Point(50, 320);
            this->button11->MaximumSize = System::Drawing::Size(32, 32);
            this->button11->MinimumSize = System::Drawing::Size(32, 32);
            this->button11->Name = L"button11";
            this->button11->Size = System::Drawing::Size(32, 32);
            this->button11->TabIndex = 17;
            this->button11->TabStop = false;
            this->button11->UseVisualStyleBackColor = true;
            // 
            // label1
            // 
            this->label1->AutoSize = true;
            this->label1->Location = System::Drawing::Point(12, 302);
            this->label1->Name = L"label1";
            this->label1->Size = System::Drawing::Size(78, 17);
            this->label1->TabIndex = 18;
            this->label1->Text = L"ITR: 10000";
            // 
            // label2
            // 
            this->label2->AutoSize = true;
            this->label2->Location = System::Drawing::Point(12, 355);
            this->label2->Name = L"label2";
            this->label2->Size = System::Drawing::Size(78, 17);
            this->label2->TabIndex = 21;
            this->label2->Text = L"ITR: 10000";
            // 
            // button12
            // 
            this->button12->FlatStyle = System::Windows::Forms::FlatStyle::System;
            this->button12->Location = System::Drawing::Point(50, 373);
            this->button12->MaximumSize = System::Drawing::Size(32, 32);
            this->button12->MinimumSize = System::Drawing::Size(32, 32);
            this->button12->Name = L"button12";
            this->button12->Size = System::Drawing::Size(32, 32);
            this->button12->TabIndex = 20;
            this->button12->TabStop = false;
            this->button12->UseVisualStyleBackColor = true;
            // 
            // button13
            // 
            this->button13->FlatStyle = System::Windows::Forms::FlatStyle::System;
            this->button13->Location = System::Drawing::Point(12, 373);
            this->button13->MaximumSize = System::Drawing::Size(32, 32);
            this->button13->MinimumSize = System::Drawing::Size(32, 32);
            this->button13->Name = L"button13";
            this->button13->Size = System::Drawing::Size(32, 32);
            this->button13->TabIndex = 19;
            this->button13->TabStop = false;
            this->button13->UseVisualStyleBackColor = true;
            // 
            // label3
            // 
            this->label3->AutoSize = true;
            this->label3->Location = System::Drawing::Point(12, 410);
            this->label3->Name = L"label3";
            this->label3->Size = System::Drawing::Size(78, 17);
            this->label3->TabIndex = 24;
            this->label3->Text = L"ITR: 10000";
            // 
            // button17
            // 
            this->button17->FlatStyle = System::Windows::Forms::FlatStyle::System;
            this->button17->Location = System::Drawing::Point(50, 428);
            this->button17->MaximumSize = System::Drawing::Size(32, 32);
            this->button17->MinimumSize = System::Drawing::Size(32, 32);
            this->button17->Name = L"button17";
            this->button17->Size = System::Drawing::Size(32, 32);
            this->button17->TabIndex = 23;
            this->button17->TabStop = false;
            this->button17->UseVisualStyleBackColor = true;
            // 
            // button18
            // 
            this->button18->FlatStyle = System::Windows::Forms::FlatStyle::System;
            this->button18->Location = System::Drawing::Point(12, 428);
            this->button18->MaximumSize = System::Drawing::Size(32, 32);
            this->button18->MinimumSize = System::Drawing::Size(32, 32);
            this->button18->Name = L"button18";
            this->button18->Size = System::Drawing::Size(32, 32);
            this->button18->TabIndex = 22;
            this->button18->TabStop = false;
            this->button18->UseVisualStyleBackColor = true;
            // 
            // label4
            // 
            this->label4->AutoSize = true;
            this->label4->Location = System::Drawing::Point(12, 466);
            this->label4->Name = L"label4";
            this->label4->Size = System::Drawing::Size(78, 17);
            this->label4->TabIndex = 27;
            this->label4->Text = L"ITR: 10000";
            // 
            // button19
            // 
            this->button19->FlatStyle = System::Windows::Forms::FlatStyle::System;
            this->button19->Location = System::Drawing::Point(50, 484);
            this->button19->MaximumSize = System::Drawing::Size(32, 32);
            this->button19->MinimumSize = System::Drawing::Size(32, 32);
            this->button19->Name = L"button19";
            this->button19->Size = System::Drawing::Size(32, 32);
            this->button19->TabIndex = 26;
            this->button19->TabStop = false;
            this->button19->UseVisualStyleBackColor = true;
            // 
            // button20
            // 
            this->button20->FlatStyle = System::Windows::Forms::FlatStyle::System;
            this->button20->Location = System::Drawing::Point(12, 484);
            this->button20->MaximumSize = System::Drawing::Size(32, 32);
            this->button20->MinimumSize = System::Drawing::Size(32, 32);
            this->button20->Name = L"button20";
            this->button20->Size = System::Drawing::Size(32, 32);
            this->button20->TabIndex = 25;
            this->button20->TabStop = false;
            this->button20->UseVisualStyleBackColor = true;
            // 
            // label5
            // 
            this->label5->AutoSize = true;
            this->label5->Location = System::Drawing::Point(12, 522);
            this->label5->Name = L"label5";
            this->label5->Size = System::Drawing::Size(78, 17);
            this->label5->TabIndex = 30;
            this->label5->Text = L"ITR: 10000";
            // 
            // button21
            // 
            this->button21->FlatStyle = System::Windows::Forms::FlatStyle::System;
            this->button21->Location = System::Drawing::Point(50, 540);
            this->button21->MaximumSize = System::Drawing::Size(32, 32);
            this->button21->MinimumSize = System::Drawing::Size(32, 32);
            this->button21->Name = L"button21";
            this->button21->Size = System::Drawing::Size(32, 32);
            this->button21->TabIndex = 29;
            this->button21->TabStop = false;
            this->button21->UseVisualStyleBackColor = true;
            // 
            // button22
            // 
            this->button22->FlatStyle = System::Windows::Forms::FlatStyle::System;
            this->button22->Location = System::Drawing::Point(12, 540);
            this->button22->MaximumSize = System::Drawing::Size(32, 32);
            this->button22->MinimumSize = System::Drawing::Size(32, 32);
            this->button22->Name = L"button22";
            this->button22->Size = System::Drawing::Size(32, 32);
            this->button22->TabIndex = 28;
            this->button22->TabStop = false;
            this->button22->UseVisualStyleBackColor = true;
            // 
            // button1
            // 
            this->button1->FlatStyle = System::Windows::Forms::FlatStyle::System;
            this->button1->Location = System::Drawing::Point(50, 245);
            this->button1->MaximumSize = System::Drawing::Size(32, 32);
            this->button1->MinimumSize = System::Drawing::Size(32, 32);
            this->button1->Name = L"button1";
            this->button1->Size = System::Drawing::Size(32, 32);
            this->button1->TabIndex = 32;
            this->button1->TabStop = false;
            this->button1->UseVisualStyleBackColor = true;
            // 
            // button2
            // 
            this->button2->FlatStyle = System::Windows::Forms::FlatStyle::System;
            this->button2->Location = System::Drawing::Point(12, 245);
            this->button2->MaximumSize = System::Drawing::Size(32, 32);
            this->button2->MinimumSize = System::Drawing::Size(32, 32);
            this->button2->Name = L"button2";
            this->button2->Size = System::Drawing::Size(32, 32);
            this->button2->TabIndex = 31;
            this->button2->TabStop = false;
            this->button2->UseVisualStyleBackColor = true;
            // 
            // GUIForm
            // 
            this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(190)), static_cast<System::Int32>(static_cast<System::Byte>(190)), 
                static_cast<System::Int32>(static_cast<System::Byte>(195)));
            this->ClientSize = System::Drawing::Size(912, 600);
            this->Controls->Add(this->splitContainer1);
            this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
            this->MaximumSize = System::Drawing::Size(918, 645);
            this->MinimumSize = System::Drawing::Size(918, 600);
            this->Name = L"GUIForm";
            this->Text = L"Cloth Simulator";
            this->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &GUIForm::GUIForm_FormClosed);
            this->splitContainer1->Panel1->ResumeLayout(false);
            this->splitContainer1->Panel1->PerformLayout();
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
