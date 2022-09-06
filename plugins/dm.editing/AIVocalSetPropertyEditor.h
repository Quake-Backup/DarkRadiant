#pragma once

#include <wx/event.h>
#include "ui/ientityinspector.h"

namespace ui
{

namespace
{
	const std::string DEF_VOCAL_SET_KEY = "def_vocal_set";
}

class AIVocalSetPropertyEditor final :
	public wxEvtHandler,
	public IPropertyEditor
{
private:
	// The top-level widget
	wxPanel* _widget;

    IEntitySelection& _entities;

    ITargetKey::Ptr _key;

public:
	~AIVocalSetPropertyEditor();

    AIVocalSetPropertyEditor(wxWindow* parent, IEntitySelection& entities, const ITargetKey::Ptr& key);

	wxPanel* getWidget() override;
	void updateFromEntities() override;
	
    static Ptr CreateNew(wxWindow* parent, IEntitySelection& entities, const ITargetKey::Ptr& key);

private:
	void onChooseButton(wxCommandEvent& ev);
};

class AIVocalSetEditorDialogWrapper :
    public IPropertyEditorDialog
{
public:
    std::string runDialog(Entity* entity, const std::string& key) override;
};

} // namespace ui
