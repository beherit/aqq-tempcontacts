//---------------------------------------------------------------------------

#ifndef SettingsFrmH
#define SettingsFrmH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "sBevel.hpp"
#include <Vcl.ExtCtrls.hpp>
#include "sButton.hpp"
#include "sSkinManager.hpp"
#include "sSkinProvider.hpp"
#include "sTabControl.hpp"
#include <Vcl.ComCtrls.hpp>
#include "sEdit.hpp"
#include "sLabel.hpp"
#include <System.Actions.hpp>
#include <Vcl.ActnList.hpp>
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
};
//---------------------------------------------------------------------------
extern PACKAGE TSettingsForm *SettingsForm;
//---------------------------------------------------------------------------
#endif
