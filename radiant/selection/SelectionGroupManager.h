#pragma once

#include "iselectiongroup.h"
#include "imap.h"
#include "icommandsystem.h"
#include <map>

namespace selection
{

class SelectionGroup;
typedef std::shared_ptr<SelectionGroup> SelectionGroupPtr;

class SelectionGroupManager :
	public ISelectionGroupManager
{
private:
	typedef std::map<std::size_t, SelectionGroupPtr> SelectionGroupMap;
	SelectionGroupMap _groups;

public:
	const std::string& getName() const override;
	const StringSet& getDependencies() const override;
	void initialiseModule(const ApplicationContext& ctx) override;

	ISelectionGroupPtr createSelectionGroup() override;
	void setGroupSelected(std::size_t id, bool selected) override;
	void deleteAllSelectionGroups() override;
	void deleteSelectionGroup(std::size_t id) override;

private:
	void deleteAllSelectionGroupsCmd(const cmd::ArgumentList& args);
	void groupSelectedCmd(const cmd::ArgumentList& args);
	void ungroupSelectedCmd(const cmd::ArgumentList& args);

	void onMapEvent(IMap::MapEvent ev);

	std::size_t generateGroupId();
};

}
