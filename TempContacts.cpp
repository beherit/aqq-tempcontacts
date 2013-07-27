#include <vcl.h>
#include <windows.h>
#pragma hdrstop
#pragma argsused
#include "SettingsFrm.h"
#include <PluginAPI.h>
#include <inifiles.hpp>

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
PPluginContact ActiveTabContact;
PPluginContact ReplyListContact;
//SETTINGS-------------------------------------------------------------------
UnicodeString GroupName;
//FORWARD-AQQ-HOOKS----------------------------------------------------------
int __stdcall OnActiveTab(WPARAM wParam, LPARAM lParam);
int __stdcall OnThemeChanged(WPARAM wParam, LPARAM lParam);
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

//Sprawdzanie czy  wlaczona jest zaawansowana stylizacja okien
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

//Hook na aktwyna zakladke lub okno rozmowy
int __stdcall OnActiveTab(WPARAM wParam, LPARAM lParam)
{
  //Pobieranie danych
  ActiveTabContact = (PPluginContact)lParam;
  //Kontakt nie jest dodany do listy i nie jest czatem
  if((ActiveTabContact->Temporary)&&(!ActiveTabContact->IsChat))
  {
	//Ustawianie stanu kontatku
	ActiveTabContact->State = 6;
	//Ustawianie opisu kontatku
	ActiveTabContact->Status = L"Kontakt tymczasowy";
	//Ustawianie grupy kontatku
	ActiveTabContact->Groups = GroupName.w_str();
	//Dodawanie kontatku do listy kontatkow
	PluginLink.CallService(AQQ_CONTACTS_UPDATE,0,(LPARAM)ActiveTabContact);
  }

  return 0;
}
//---------------------------------------------------------------------------

//Hook na zmiane kompozycji
int __stdcall OnThemeChanged(WPARAM wParam, LPARAM lParam)
{
  //Okno ustawien zostalo juz stworzone
  if(hSettingsForm)
  {
	//Wlaczona zaawansowana stylizacja okien
	if(ChkSkinEnabled())
	{
	  //Pobieranie sciezki nowej aktywnej kompozycji
	  UnicodeString ThemeDir = StringReplace((wchar_t*)lParam, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
	  //Plik zaawansowanej stylizacji okien istnieje
	  if(FileExists(ThemeDir + "\\\\Skin\\\\Skin.asz"))
	  {
		ThemeDir = StringReplace(ThemeDir, "\\\\", "\\", TReplaceFlags() << rfReplaceAll);
		hSettingsForm->sSkinManager->SkinDirectory = ThemeDir + "\\Skin";
		hSettingsForm->sSkinManager->SkinName = "Skin.asz";
		if(ChkThemeAnimateWindows()) hSettingsForm->sSkinManager->AnimEffects->FormShow->Time = 200;
		else hSettingsForm->sSkinManager->AnimEffects->FormShow->Time = 0;
		hSettingsForm->sSkinManager->Effects->AllowGlowing = ChkThemeGlowing();
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
extern "C" int __declspec(dllexport) __stdcall Load(PPluginLink Link)
{
  //Linkowanie wtyczki z komunikatorem
  PluginLink = *Link;
  //Hook na aktwyna zakladke lub okno rozmowy
  PluginLink.HookEvent(AQQ_CONTACTS_BUDDY_ACTIVETAB,OnActiveTab);
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
extern "C" int __declspec(dllexport) __stdcall Unload()
{
  //Wyladowanie wszystkich hookow
  PluginLink.UnhookEvent(OnActiveTab);
  PluginLink.UnhookEvent(OnThemeChanged);

  return 0;
}
//---------------------------------------------------------------------------

//Ustawienia wtyczki
extern "C" int __declspec(dllexport)__stdcall Settings()
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
  PluginInfo.Version = PLUGIN_MAKE_VERSION(1,2,0,0);
  PluginInfo.Description = L"Wtyczka s�u�y do automatycznego tymczasowego zapisywania na list� kontakt�w os�b, kt�rych na tej li�cie...nie mamy ;)";
  PluginInfo.Author = L"Krzysztof Grochocki (Beherit)";
  PluginInfo.AuthorMail = L"kontakt@beherit.pl";
  PluginInfo.Copyright = L"Krzysztof Grochocki (Beherit)";
  PluginInfo.Homepage = L"http://beherit.pl";

  return &PluginInfo;
}
//---------------------------------------------------------------------------