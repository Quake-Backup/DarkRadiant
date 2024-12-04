#include "FilterPopupMenu.h"

#include "ifilter.h"
#include "ui/ieventmanager.h"
#include <wx/menu.h>

#include "wxutil/menu/IconTextMenuItem.h"

namespace wxutil
{

namespace
{
	const char* const MENU_ICON = "iconFilter16.png";
}

FilterPopupMenu::FilterPopupMenu()
{
	// Visit the filters in the FilterSystem to populate the menu
    GlobalFilterSystem().forEachFilter([=](const std::string& name) { visitFilter(name); });
}

FilterPopupMenu::~FilterPopupMenu()
{
	for (const auto& [name, menuItem] : _filterItems)
	{
		GlobalEventManager().unregisterMenuItem(name, menuItem);
	}
}

void FilterPopupMenu::visitFilter(const std::string& filterName)
{
	auto* item = Append(new wxutil::IconTextMenuItem(filterName, MENU_ICON));
	item->SetCheckable(true);

	std::string eventName = GlobalFilterSystem().getFilterEventName(filterName);

	GlobalEventManager().registerMenuItem(eventName, item);

    // We remember the item mapping for deregistration on shutdown
	_filterItems.emplace(eventName, item);
}

} // namespace
