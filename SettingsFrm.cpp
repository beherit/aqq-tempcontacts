//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "SettingsFrm.h"
#include <inifiles.hpp>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "sBevel"
#pragma link "sButton"
#pragma link "sSkinManager"
#pragma link "sSkinProvider"
#pragma link "sTabControl"
#pragma link "sEdit"
#pragma link "sLabel"
#pragma resource "*.dfm"
TSettingsForm *SettingsForm;
//---------------------------------------------------------------------------
__declspec(dllimport)UnicodeString GetPluginUserDir();
__declspec(dllimport)UnicodeString GetThemeSkinDir();
__declspec(dllimport)bool ChkSkinEnabled();
__declspec(dllimport)bool ChkNativeEnabled();
__declspec(dllimport)void LoadSettings();
//---------------------------------------------------------------------------

__fastcall TSettingsForm::TSettingsForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::FormCreate(TObject *Sender)
{
  //Skorkowanie okna
  if(ChkSkinEnabled())
  {
	UnicodeString ThemeSkinDir = GetThemeSkinDir();
	//Wlaczenie skorkowania
	if((FileExists(ThemeSkinDir + "\\\\Skin.asz"))&&(!ChkNativeEnabled()))
	{
	  ThemeSkinDir = StringReplace(ThemeSkinDir, "\\\\", "\\", TReplaceFlags() << rfReplaceAll);
	  sSkinManager->SkinDirectory = ThemeSkinDir;
	  sSkinManager->SkinName = "Skin.asz";
	  sSkinProvider->DrawNonClientArea = true;
	  sSkinManager->Active = true;
	}
	//Wylaczenie skorkowania
	else
	 sSkinManager->Active = false;
  }
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::FormShow(TObject *Sender)
{
  //Skorkowanie okna
  if(!ChkSkinEnabled())
  {
	UnicodeString ThemeSkinDir = GetThemeSkinDir();
	//Wlaczenie skorkowania
	if((FileExists(ThemeSkinDir + "\\\\Skin.asz"))&&(!ChkNativeEnabled()))
	{
	  ThemeSkinDir = StringReplace(ThemeSkinDir, "\\\\", "\\", TReplaceFlags() << rfReplaceAll);
	  sSkinManager->SkinDirectory = ThemeSkinDir;
	  sSkinManager->SkinName = "Skin.asz";
	  sSkinProvider->DrawNonClientArea = false;
	  sSkinManager->Active = true;
	}
	//Wylaczenie skorkowania
	else
	 sSkinManager->Active = false;
  }
  //Odczyt ustawien wtyczki
  aLoadSettings->Execute();
  //Wylaczenie przycisku
  SaveButton->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aExitExecute(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aLoadSettingsExecute(TObject *Sender)
{
  TIniFile *Ini = new TIniFile(GetPluginUserDir()+"\\\\TempContacts\\\\Settings.ini");
  GroupNameEdit->Text = Ini->ReadString("Settings","GroupName","Kontakty tymczasowe");
  delete Ini;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aSaveSettingsExecute(TObject *Sender)
{
  TIniFile *Ini = new TIniFile(GetPluginUserDir()+"\\\\TempContacts\\\\Settings.ini");
  Ini->WriteString("Settings","GroupName",GroupNameEdit->Text);
  delete Ini;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::GroupNameEditChange(TObject *Sender)
{
  //Wlaczenie przycisku
  if(!GroupNameEdit->Text.IsEmpty())
   SaveButton->Enabled = true;
  //Wylaczenie przycisku
  else
   SaveButton->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::SaveButtonClick(TObject *Sender)
{
  //Wylaczenie przycisku
  SaveButton->Enabled = false;
  //Zapisanie ustawien
  aSaveSettings->Execute();
  //Odczyt ustawien w rdzeniu wtyczki
  LoadSettings();
  //Zamkniecie formy
  Close();
}
//---------------------------------------------------------------------------
