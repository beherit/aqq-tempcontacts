//---------------------------------------------------------------------------
// Copyright (C) 2010-2015 Krzysztof Grochocki
//
// This file is part of TempContacts
//
// TempContacts is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3, or (at your option)
// any later version.
//
// TempContacts is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GNU Radio; see the file COPYING. If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street,
// Boston, MA 02110-1301, USA.
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "SettingsFrm.h"
#include <inifiles.hpp>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "sBevel"
#pragma link "sButton"
#pragma link "sEdit"
#pragma link "sLabel"
#pragma link "sSkinManager"
#pragma link "sSkinProvider"
#pragma link "sTabControl"
#pragma resource "*.dfm"
TSettingsForm *SettingsForm;
//---------------------------------------------------------------------------
__declspec(dllimport)UnicodeString GetPluginUserDir();
__declspec(dllimport)UnicodeString GetThemeSkinDir();
__declspec(dllimport)bool ChkSkinEnabled();
__declspec(dllimport)bool ChkThemeAnimateWindows();
__declspec(dllimport)bool ChkThemeGlowing();
__declspec(dllimport)int GetHUE();
__declspec(dllimport)int GetSaturation();
__declspec(dllimport)void LoadSettings();
//---------------------------------------------------------------------------

__fastcall TSettingsForm::TSettingsForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::WMTransparency(TMessage &Message)
{
	Application->ProcessMessages();
	if(sSkinManager->Active) sSkinProvider->BorderForm->UpdateExBordersPos(true,(int)Message.LParam);
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::FormCreate(TObject *Sender)
{
	//Wlaczona zaawansowana stylizacja okien
	if(ChkSkinEnabled())
	{
		UnicodeString ThemeSkinDir = GetThemeSkinDir();
		//Plik zaawansowanej stylizacji okien istnieje
		if(FileExists(ThemeSkinDir + "\\\\Skin.asz"))
		{
			//Dane pliku zaawansowanej stylizacji okien
			ThemeSkinDir = StringReplace(ThemeSkinDir, "\\\\", "\\", TReplaceFlags() << rfReplaceAll);
			sSkinManager->SkinDirectory = ThemeSkinDir;
			sSkinManager->SkinName = "Skin.asz";
			//Ustawianie animacji AlphaControls
			if(ChkThemeAnimateWindows()) sSkinManager->AnimEffects->FormShow->Time = 200;
			else sSkinManager->AnimEffects->FormShow->Time = 0;
			sSkinManager->Effects->AllowGlowing = ChkThemeGlowing();
			//Zmiana kolorystyki AlphaControls
			sSkinManager->HueOffset = GetHUE();
			sSkinManager->Saturation = GetSaturation();
			//Aktywacja skorkowania AlphaControls
			sSkinManager->Active = true;
		}
		//Brak pliku zaawansowanej stylizacji okien
		else sSkinManager->Active = false;
	}
	//Zaawansowana stylizacja okien wylaczona
	else sSkinManager->Active = false;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::FormShow(TObject *Sender)
{
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

void __fastcall TSettingsForm::sSkinManagerSysDlgInit(TacSysDlgData DlgData, bool &AllowSkinning)
{
	AllowSkinning = false;
}
//---------------------------------------------------------------------------
