#pragma once

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
		GUIForm(void)
		{
			InitializeComponent();
		}

	protected:

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~GUIForm()
		{
			if (components)
			{
				delete components;
			}
		}

	private:

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
            System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(GUIForm::typeid));
            this->SuspendLayout();
            // 
            // GUIForm
            // 
            this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->ClientSize = System::Drawing::Size(782, 555);
            this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::Fixed3D;
            this->Icon = (cli::safe_cast<System::Drawing::Icon^  >(resources->GetObject(L"$this.Icon")));
            this->MaximumSize = System::Drawing::Size(800, 600);
            this->MinimumSize = System::Drawing::Size(800, 600);
            this->Name = L"GUIForm";
            this->Text = L"Cloth Simulator";
            this->Load += gcnew System::EventHandler(this, &GUIForm::GUIForm_Load);
            this->ResumeLayout(false);

        }
#pragma endregion

		/// <summary>
		/// On Form Load
		/// </summary>
        System::Void GUIForm_Load(System::Object^  sender, System::EventArgs^  e) 
        {
        }
    };
}
