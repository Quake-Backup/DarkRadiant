#include "ResponseEditor.h"

#include <gtk/gtk.h>

namespace ui {

ResponseEditor::ResponseEditor() {
	populatePage();
}

ResponseEditor::operator GtkWidget*() {
	return _pageVBox;
}

void ResponseEditor::populatePage() {
	_pageVBox = gtk_vbox_new(FALSE, 6);
}

void ResponseEditor::openContextMenu(GtkTreeView* view) {
	
}

void ResponseEditor::removeItem(GtkTreeView* view) {
	
}

void ResponseEditor::selectionChanged() {
	
}

} // namespace ui
