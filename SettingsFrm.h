//---------------------------------------------------------------------------
// Copyright (C) 2010-2014 Krzysztof Grochocki
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
#ifndef SettingsFrmH
#define SettingsFrmH
#define WM_ALPHAWINDOWS (WM_USER + 666)
//---------------------------------------------------------------------------
#include "sBevel.hpp"
#include "sButton.hpp"
#include "sEdit.hpp"
#include "sLabel.hpp"
#include "sSkinManager.hpp"
#include "sSkinProvider.hpp"
#include "sTabControl.hpp"
#include <System.Actions.hpp>
#include <System.Classes.hpp>
#include <Vcl.ActnList.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.StdCtrls.hpp>
//---------------------------------------------------------------------------
class TSettingsForm : public TForm
{
__published:	// IDE-managed Components
	TsBevel *Bevel;
	TsButton *SaveButton;
	TsButton *CancelButton;
	TsSkinProvider *sSkinProvider;
	TsSkinManager *sSkinManager;
	TsTabControl *sTabControl;
	TsEdit *GroupNameEdit;
	TsLabel *InfoLabel;
	TActionList *ActionList;
	TAction *aExit;
	TAction *aLoadSettings;
	TAction *aSaveSettings;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall aExitExecute(TObject *Sender);
	void __fastcall aLoadSettingsExecute(TObject *Sender);
	void __fastcall aSaveSettingsExecute(TObject *Sender);
	void __fastcall GroupNameEditChange(TObject *Sender);
	void __fastcall SaveButtonClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TSettingsForm(TComponent* Owner);
	void __fastcall WMTransparency(TMessage &Message);
	BEGIN_MESSAGE_MAP
	MESSAGE_HANDLER(WM_ALPHAWINDOWS,TMessage,WMTransparency);
	END_MESSAGE_MAP(TForm)
};
//---------------------------------------------------------------------------
extern PACKAGE TSettingsForm *SettingsForm;
//---------------------------------------------------------------------------
#endif
