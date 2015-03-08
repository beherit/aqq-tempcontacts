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

#include <vcl.h>
#include <windows.h>
#include <inifiles.hpp>
#include <PluginAPI.h>
#pragma hdrstop
#include "SettingsFrm.h"

int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
	return 1;
}
//---------------------------------------------------------------------------

//Uchwyt-do-formy-ustawien---------------------------------------------------
TSettingsForm *hSettingsForm;
//Struktury-glowne-----------------------------------------------------------
TPluginLink PluginLink;
TPluginInfo PluginInfo;
//Uchwyt-do-okna-rozmowy-----------------------------------------------------
HWND hFrmSend;
//Lista-tymczasowych-kontaktow-----------------------------------------------
TStringList *TempContactsList = new TStringList;
//Informacje-o-aktualnie-aktywnym-tymczasowym-kontakcie----------------------
UnicodeString LastActiveJID;
TPluginContact TempContact;
//SETTINGS-------------------------------------------------------------------
UnicodeString GroupName;
//FORWARD-AQQ-HOOKS----------------------------------------------------------
INT_PTR __stdcall OnActiveTab(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnCloseTab(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnThemeChanged(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall ServiceTempContactsAddItem(WPARAM wParam, LPARAM lParam);
//---------------------------------------------------------------------------

//Pobieranie sciezki katalogu prywatnego wtyczek
UnicodeString GetPluginUserDir()
{
	return StringReplace((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,0,0), "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
}
//---------------------------------------------------------------------------

//Pobieranie sciezki do skorki kompozycji
UnicodeString GetThemeSkinDir()
{
	return StringReplace((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETTHEMEDIR,0,0), "\\", "\\\\", TReplaceFlags() << rfReplaceAll) + "\\\\Skin";
}
//---------------------------------------------------------------------------

//Sprawdzanie czy wlaczona jest zaawansowana stylizacja okien
bool ChkSkinEnabled()
{
	TStrings* IniList = new TStringList();
	IniList->SetText((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_FETCHSETUP,0,0));
	TMemIniFile *Settings = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
	Settings->SetStrings(IniList);
	delete IniList;
	UnicodeString SkinsEnabled = Settings->ReadString("Settings","UseSkin","1");
	delete Settings;
	return StrToBool(SkinsEnabled);
}
//---------------------------------------------------------------------------

//Sprawdzanie ustawien animacji AlphaControls
bool ChkThemeAnimateWindows()
{
	TStrings* IniList = new TStringList();
	IniList->SetText((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_FETCHSETUP,0,0));
	TMemIniFile *Settings = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
	Settings->SetStrings(IniList);
	delete IniList;
	UnicodeString AnimateWindowsEnabled = Settings->ReadString("Theme","ThemeAnimateWindows","1");
	delete Settings;
	return StrToBool(AnimateWindowsEnabled);
}
//---------------------------------------------------------------------------
bool ChkThemeGlowing()
{
	TStrings* IniList = new TStringList();
	IniList->SetText((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_FETCHSETUP,0,0));
	TMemIniFile *Settings = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
	Settings->SetStrings(IniList);
	delete IniList;
	UnicodeString GlowingEnabled = Settings->ReadString("Theme","ThemeGlowing","1");
	delete Settings;
	return StrToBool(GlowingEnabled);
}
//---------------------------------------------------------------------------

//Pobieranie ustawien koloru AlphaControls
int GetHUE()
{
	return (int)PluginLink.CallService(AQQ_SYSTEM_COLORGETHUE,0,0);
}
//---------------------------------------------------------------------------
int GetSaturation()
{
	return (int)PluginLink.CallService(AQQ_SYSTEM_COLORGETSATURATION,0,0);
}
//---------------------------------------------------------------------------

//Pobieranie nazwy agenta sieci
UnicodeString GetAgentName(int UserIdx)
{
	TPluginStateChange PluginStateChange;
	PluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)(&PluginStateChange),UserIdx);
	return (wchar_t*)PluginStateChange.Server;
}
//---------------------------------------------------------------------------

//Serwis dodawania kontaktu na stale do listy
INT_PTR __stdcall ServiceTempContactsAddItem(WPARAM wParam, LPARAM lParam)
{
	//Usuniecie kontaktu z listy
	PluginLink.CallService(AQQ_CONTACTS_DELETE,0,(LPARAM)&TempContact);
	//Usuniecie kontaktu z listy tymczasowych kontaktow
	TempContactsList->Delete(TempContactsList->IndexOf((wchar_t*)TempContact.JID));
	//Otwarcie okna dodawania kontaktu na stale do listy
	TPluginAddForm PluginAddForm;
	PluginAddForm.UserIdx = TempContact.UserIdx;
	PluginAddForm.JID = TempContact.JID;
	PluginAddForm.Nick = TempContact.Nick;
	PluginAddForm.Agent = GetAgentName(TempContact.UserIdx).w_str();
	PluginAddForm.Modal = false;
	PluginAddForm.Custom = false;
	PluginLink.CallService(AQQ_CONTACTS_ADDFORM,0,(LPARAM)&PluginAddForm);

	return 0;
}
//---------------------------------------------------------------------------

//Hook na aktwyna zakladke lub okno rozmowy
INT_PTR __stdcall OnActiveTab(WPARAM wParam, LPARAM lParam)
{
	//Wczesniejsza zakladka miala stworzony przycisk
	if(TempContactsList->IndexOf(LastActiveJID)!=-1)
	{
		//Usuniecie przycisku
		TPluginAction TempContactsAddButton;
		ZeroMemory(&TempContactsAddButton,sizeof(TPluginAction));
		TempContactsAddButton.cbSize = sizeof(TPluginAction);
		TempContactsAddButton.pszName = L"TempContactsAddItem";
		TempContactsAddButton.Handle = (int)hFrmSend;
		PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "tbMain" AQQ_CONTROLS_DESTROYBUTTON,0,(LPARAM)(&TempContactsAddButton));
	}
	//Pobranie uchwytu do okna rozmowy
	hFrmSend = (HWND)(int)wParam;
	//Pobieranie danych
	TPluginContact ActiveTabContact = *(PPluginContact)lParam;
	UnicodeString JID = (wchar_t*)ActiveTabContact.JID;
	//Kontakt nie jest dodany do listy i nie jest czatem
	if((ActiveTabContact.Temporary)&&(!ActiveTabContact.IsChat))
	{
		//Dodanie kontaktu do listy tymczasowych kontaktow
		TempContactsList->Add(JID);
		//Ustawianie stanu kontaktu
		ActiveTabContact.State = 6;
		//Ustawianie opisu kontaktu
		ActiveTabContact.Status = L"Kontakt tymczasowy";
		//Ustawianie grupy kontaktu
		ActiveTabContact.Groups = GroupName.w_str();
		//Dodawanie kontaktu do listy kontaktow
		PluginLink.CallService(AQQ_CONTACTS_UPDATE,0,(LPARAM)&ActiveTabContact);
	}
	//Tworzenie elementu dodawania kontaktow na stale do listy
	if((TempContactsList->IndexOf(JID)!=-1)&&(JID!=LastActiveJID))
	{
		//Dodanie przycisku
		TPluginAction TempContactsAddButton;
		ZeroMemory(&TempContactsAddButton,sizeof(TPluginAction));
		TempContactsAddButton.cbSize = sizeof(TPluginAction);
		TempContactsAddButton.pszName = L"TempContactsAddItem";
		TempContactsAddButton.Hint = L"Dodaj kontakt na sta³e do listy";
		TempContactsAddButton.IconIndex = 14;
		TempContactsAddButton.pszService = L"sTempContactsAddItem";
		TempContactsAddButton.Handle = (int)hFrmSend;
		PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "tbMain" AQQ_CONTROLS_CREATEBUTTON,0,(LPARAM)(&TempContactsAddButton));
		//Zapamietanie danych dt. kontaktu
		//cbSize
		TempContact.cbSize = ActiveTabContact.cbSize;
		//JID
		TempContact.JID = (wchar_t*)realloc(TempContact.JID, sizeof(wchar_t)*(wcslen(ActiveTabContact.JID)+1));
		memcpy(TempContact.JID, ActiveTabContact.JID, sizeof(wchar_t)*wcslen(ActiveTabContact.JID));
		TempContact.JID[wcslen(ActiveTabContact.JID)] = L'\0';
		//Nick
		TempContact.Nick = (wchar_t*)realloc(TempContact.Nick, sizeof(wchar_t)*(wcslen(ActiveTabContact.Nick)+1));
		memcpy(TempContact.Nick, ActiveTabContact.Nick, sizeof(wchar_t)*wcslen(ActiveTabContact.Nick));
		TempContact.Nick[wcslen(ActiveTabContact.Nick)] = L'\0';
		//Resource
		TempContact.Resource = (wchar_t*)realloc(TempContact.Resource, sizeof(wchar_t)*(wcslen(ActiveTabContact.Resource)+1));
		memcpy(TempContact.Resource, ActiveTabContact.Resource, sizeof(wchar_t)*wcslen(ActiveTabContact.Resource));
		TempContact.Resource[wcslen(ActiveTabContact.Resource)] = L'\0';
		//Groups
		TempContact.Groups = (wchar_t*)realloc(TempContact.Groups, sizeof(wchar_t)*(wcslen(ActiveTabContact.Groups)+1));
		memcpy(TempContact.Groups, ActiveTabContact.Groups, sizeof(wchar_t)*wcslen(ActiveTabContact.Groups));
		TempContact.Groups[wcslen(ActiveTabContact.Groups)] = L'\0';
		//State
		TempContact.State = 0;
		//Status
		TempContact.Status = (wchar_t*)realloc(TempContact.Status, sizeof(wchar_t)*(wcslen(ActiveTabContact.Status)+1));
		memcpy(TempContact.Status, ActiveTabContact.Status, sizeof(wchar_t)*wcslen(ActiveTabContact.Status));
		TempContact.Status[wcslen(ActiveTabContact.Status)] = L'\0';
		//Other
		TempContact.Temporary = ActiveTabContact.Temporary;
		TempContact.FromPlugin = ActiveTabContact.FromPlugin;
		TempContact.UserIdx = ActiveTabContact.UserIdx;
		TempContact.Subscription = ActiveTabContact.Subscription;
		TempContact.IsChat = ActiveTabContact.Subscription;
	}
	//Zapamietanie ostatnio aktywnego JID kontaktu
	LastActiveJID = JID;

	return 0;
}
//---------------------------------------------------------------------------

//Hook na zamkniecie okna rozmowy lub zakladki
INT_PTR __stdcall OnCloseTab(WPARAM wParam, LPARAM lParam)
{
	//Pobieranie danych dt. kontaktu
	TPluginContact CloseTabContact = *(PPluginContact)lParam;
	//Pobieranie identyfikatora kontaktu
	UnicodeString JID = (wchar_t*)CloseTabContact.JID;
	//Zakladka miala stworzony przycisk
	if((TempContactsList->IndexOf(JID)!=-1)&&(JID==LastActiveJID))
	{
		//Usuniecie przycisku
		TPluginAction TempContactsAddButton;
		ZeroMemory(&TempContactsAddButton,sizeof(TPluginAction));
		TempContactsAddButton.cbSize = sizeof(TPluginAction);
		TempContactsAddButton.pszName = L"TempContactsAddItem";
		TempContactsAddButton.Handle = (int)hFrmSend;
		PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "tbMain" AQQ_CONTROLS_DESTROYBUTTON,0,(LPARAM)(&TempContactsAddButton));
	}

	return 0;
}
//---------------------------------------------------------------------------

//Hook na zmiane kompozycji
INT_PTR __stdcall OnThemeChanged(WPARAM wParam, LPARAM lParam)
{
	//Okno ustawien zostalo juz stworzone
	if(hSettingsForm)
	{
		//Wlaczona zaawansowana stylizacja okien
		if(ChkSkinEnabled())
		{
			//Pobieranie sciezki nowej aktywnej kompozycji
			UnicodeString ThemeSkinDir = StringReplace((wchar_t*)lParam, "\\", "\\\\", TReplaceFlags() << rfReplaceAll) + "\\\\Skin";
			//Plik zaawansowanej stylizacji okien istnieje
			if(FileExists(ThemeSkinDir + "\\\\Skin.asz"))
			{
				//Dane pliku zaawansowanej stylizacji okien
				ThemeSkinDir = StringReplace(ThemeSkinDir, "\\\\", "\\", TReplaceFlags() << rfReplaceAll);
				hSettingsForm->sSkinManager->SkinDirectory = ThemeSkinDir;
				hSettingsForm->sSkinManager->SkinName = "Skin.asz";
				//Ustawianie animacji AlphaControls
				if(ChkThemeAnimateWindows()) hSettingsForm->sSkinManager->AnimEffects->FormShow->Time = 200;
				else hSettingsForm->sSkinManager->AnimEffects->FormShow->Time = 0;
				hSettingsForm->sSkinManager->Effects->AllowGlowing = ChkThemeGlowing();
				//Zmiana kolorystyki AlphaControls
				hSettingsForm->sSkinManager->HueOffset = GetHUE();
				hSettingsForm->sSkinManager->Saturation = GetSaturation();
				//Aktywacja skorkowania AlphaControls
				hSettingsForm->sSkinManager->Active = true;
			}
			//Brak pliku zaawansowanej stylizacji okien
			else hSettingsForm->sSkinManager->Active = false;
		}
		//Zaawansowana stylizacja okien wylaczona
		else hSettingsForm->sSkinManager->Active = false;
	}

	return 0;
}
//---------------------------------------------------------------------------

//Odczyt ustawien
void LoadSettings()
{
	TIniFile *Ini = new TIniFile(GetPluginUserDir()+"\\\\TempContacts\\\\Settings.ini");
	GroupName = Ini->ReadString("Settings","GroupName","Kontakty tymczasowe");
	delete Ini;
}
//---------------------------------------------------------------------------

//Zaladowanie wtyczki
extern "C" INT_PTR __declspec(dllexport) __stdcall Load(PPluginLink Link)
{
	//Linkowanie wtyczki z komunikatorem
	PluginLink = *Link;
	//Tworzenie serwisu dodawania kontaktu na stale do listy
	PluginLink.CreateServiceFunction(L"sTempContactsAddItem",ServiceTempContactsAddItem);
	//Hook na aktwyna zakladke lub okno rozmowy
	PluginLink.HookEvent(AQQ_CONTACTS_BUDDY_ACTIVETAB,OnActiveTab);
	//Hook na zamkniecie okna rozmowy lub zakladki
	PluginLink.HookEvent(AQQ_CONTACTS_BUDDY_CLOSETAB,OnCloseTab);
	//Hook na zmiane kompozycji
	PluginLink.HookEvent(AQQ_SYSTEM_THEMECHANGED,OnThemeChanged);
	//Tworzeniu katalogu z ustawieniami wtyczki
	if(!DirectoryExists(GetPluginUserDir()+"\\\\TempContacts"))
		CreateDir(GetPluginUserDir()+"\\\\TempContacts");
	//Odczyt ustawien
	LoadSettings();

	return 0;
}
//---------------------------------------------------------------------------

//Wyladowanie wtyczki
extern "C" INT_PTR __declspec(dllexport) __stdcall Unload()
{
	//Usuniecie przycisku
	TPluginAction TempContactsAddButton;
	ZeroMemory(&TempContactsAddButton,sizeof(TPluginAction));
	TempContactsAddButton.cbSize = sizeof(TPluginAction);
	TempContactsAddButton.pszName = L"TempContactsAddItem";
	TempContactsAddButton.Handle = (int)hFrmSend;
	PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "tbMain" AQQ_CONTROLS_DESTROYBUTTON,0,(LPARAM)(&TempContactsAddButton));
	//Usuwanie serwisu dodawania kontaktu na stale do listy
	PluginLink.DestroyServiceFunction(ServiceTempContactsAddItem);
	//Wyladowanie wszystkich hookow
	PluginLink.UnhookEvent(OnActiveTab);
	PluginLink.UnhookEvent(OnCloseTab);
	PluginLink.UnhookEvent(OnThemeChanged);

	return 0;
}
//---------------------------------------------------------------------------

//Ustawienia wtyczki
extern "C" INT_PTR __declspec(dllexport)__stdcall Settings()
{
	//Przypisanie uchwytu do formy ustawien
	if(!hSettingsForm)
	{
		Application->Handle = (HWND)SettingsForm;
		hSettingsForm = new TSettingsForm(Application);
	}
	//Pokaznie okna ustawien
	hSettingsForm->Show();

	return 0;
}
//---------------------------------------------------------------------------

//Informacje o wtyczce
extern "C" __declspec(dllexport) PPluginInfo __stdcall AQQPluginInfo(DWORD AQQVersion)
{
	PluginInfo.cbSize = sizeof(TPluginInfo);
	PluginInfo.ShortName = L"TempContacts";
	PluginInfo.Version = PLUGIN_MAKE_VERSION(1,4,2,0);
	PluginInfo.Description = L"Wtyczka s³u¿y do automatycznego tymczasowego zapisywania na listê kontaktów osób, których na tej liœcie...nie mamy ;)";
	PluginInfo.Author = L"Krzysztof Grochocki";
	PluginInfo.AuthorMail = L"kontakt@beherit.pl";
	PluginInfo.Copyright = L"Krzysztof Grochocki";
	PluginInfo.Homepage = L"http://beherit.pl";
	PluginInfo.Flag = 0;
	PluginInfo.ReplaceDefaultModule = 0;

	return &PluginInfo;
}
//---------------------------------------------------------------------------
