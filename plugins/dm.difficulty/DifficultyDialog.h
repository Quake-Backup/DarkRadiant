#pragma once

#include "scene/Entity.h"
#include "iradiant.h"
#include "icommandsystem.h"
#include "wxutil/dialog/DialogBase.h"

#include "DifficultyEditor.h"
#include "DifficultySettingsManager.h"

#include <memory>
#include <wx/choicebk.h>

namespace ui
{

class DifficultyDialog;
typedef std::shared_ptr<DifficultyDialog> DifficultyDialogPtr;

/**
 * greebo: A difficulty dialog is a modal top-level window which provides
 *         views and controls facilitating the editing of difficulty settings.
 *
 * Maintains a certain number of DifficultyEditors which get packed into the
 * notebook tabs.
 */
class DifficultyDialog :
	public wxutil::DialogBase
{
	wxChoicebook* _notebook;

	// The difficulty settings manager
	difficulty::DifficultySettingsManager _settingsManager;

	std::vector<DifficultyEditorPtr> _editors;

public:
	DifficultyDialog();

	int ShowModal();

	// Command target to toggle the dialog
	static void ShowDialog(const cmd::ArgumentList& args);

private:
	// greebo: Saves the current working set to the entity
	void save();

	// WIDGET POPULATION
	void populateWindow(); 			// Main window
	void createDifficultyEditors();

    void editCurrentDifficultyName();
}; // class DifficultyDialog

} // namespace ui
