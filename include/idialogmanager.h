#ifndef _IDIALOG_MANAGER_H_
#define _IDIALOG_MANAGER_H_

#include "iuimanager.h"
#include <boost/shared_ptr.hpp>

typedef struct _GtkWindow GtkWindow;

namespace ui
{

class IDialog
{
public:
	virtual ~IDialog() {}

	enum Result
	{
		RESULT_CANCELLED = 0,
		RESULT_OK,
		RESULT_NO,
		RESULT_YES,
	};

	// Possible message types (used for IDialogManager::createMessageBox())
	enum MessageType
	{
		MESSAGE_CONFIRM,	// Just a plain message with an OK button
		MESSAGE_ASK,		// Queries Yes/No from the user
		MESSAGE_WARNING,	// Displays a warning message
		MESSAGE_ERROR,		// Displays an error message
	};

	// Sets the dialog title
	virtual void setTitle(const std::string& title) = 0;

	/**
	 * Run the dialog an enter the main loop (block the application).
	 * Returns the Dialog::Result, corresponding to the user's action.
	 */
	virtual Result run() = 0;
};
typedef boost::shared_ptr<IDialog> IDialogPtr;

class IDialogManager
{
public:
	// Virtual destructor
	virtual ~IDialogManager() {}

	/**
	 * Create a new dialog. Note that the DialogManager will hold a reference
	 * to this dialog internally to allow scripts to reference the Dialog class 
	 * without holding the shared_ptr on their own or using wrapper classes doing so.
	 *
	 * Every dialog features an OK and a Cancel button by default.
	 *
	 * @title: The string displayed on the dialog's window bar.
	 * @type: the dialog type to create, determines e.g. which buttons are shown.
	 * @parent: optional top-level widget this dialog should be parented to, defaults to
	 *			GlobalRadiant().getMainWindow().
	 */
	virtual IDialogPtr createDialog(const std::string& title, GtkWindow* parent = NULL) = 0;

	/**
	 * Create a simple message box, which can either notify the user about something,
	 * queries "Yes"/"No" or displays an error message. It usually features
	 * an icon according to the the MessageType passed (exclamation mark, error sign).
	 *
	 * @title: The string displayed on the message box window bar.
	 * @text: The text/question to be displayed.
	 * @type: the message type this dialog represents.
	 * @parent: optional top-level widget this dialog should be parented to, defaults to
	 *			GlobalRadiant().getMainWindow().
	 */
	virtual IDialogPtr createMessageBox(const std::string& title, const std::string& text, 
										IDialog::MessageType type, GtkWindow* parent = NULL) = 0;
};

} // namespace ui

// Shortcut method
inline ui::IDialogManager& GlobalDialogManager()
{
	return GlobalUIManager().getDialogManager();
}

#endif /* _IDIALOG_MANAGER_H_ */
