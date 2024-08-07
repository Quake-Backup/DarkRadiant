#pragma once

#include <sigc++/connection.h>
#include <wx/bmpbuttn.h>

#include "icommandsystem.h"
#include "modelskin.h"

#include "wxutil/WindowPosition.h"
#include "wxutil/PanedPosition.h"
#include "wxutil/dataview/DeclarationTreeView.h"
#include "wxutil/XmlResourceBasedWidget.h"
#include "wxutil/dialog/DialogBase.h"
#include "wxutil/preview/ModelPreview.h"
#include "wxutil/sourceview/SourceView.h"

namespace wxutil { class DeclFileInfo; }
namespace ui
{

class SkinEditorTreeView;
class ModelTreeView;

/// Graphical editor for .skin files
class SkinEditor final: public wxutil::DialogBase, private wxutil::XmlResourceBasedWidget
{
    decl::ISkin::Ptr _skin;

    ModelTreeView* _modelTreeView;
    std::unique_ptr<wxutil::ModelPreview> _modelPreview;
    wxutil::D3DeclarationViewCtrl* _sourceView;

    wxutil::DeclarationTreeView::Columns _columns;
    SkinEditorTreeView* _skinTreeView;

    struct SelectedModelColumns :
        public wxutil::TreeModel::ColumnRecord
    {
        SelectedModelColumns() :
            name(add(wxutil::TreeModel::Column::IconText))
        {}

        wxutil::TreeModel::Column name;
    };

    SelectedModelColumns _selectedModelColumns;
    wxutil::TreeModel::Ptr _selectedModels;
    wxutil::TreeView* _selectedModelList;
    wxWindowPtr<wxutil::DeclFileInfo> _saveNotePanel;

    struct RemappingColumns :
        public wxutil::TreeModel::ColumnRecord
    {
        RemappingColumns() :
            active(add(wxutil::TreeModel::Column::Boolean)),
            original(add(wxutil::TreeModel::Column::String)),
            replacement(add(wxutil::TreeModel::Column::String)),
            unchangedOriginal(add(wxutil::TreeModel::Column::String)),
            unchangedReplacement(add(wxutil::TreeModel::Column::String))
        {}

        wxutil::TreeModel::Column active;
        wxutil::TreeModel::Column original;
        wxutil::TreeModel::Column replacement;
        wxutil::TreeModel::Column unchangedOriginal;
        wxutil::TreeModel::Column unchangedReplacement;
    };

    RemappingColumns _remappingColumns;
    wxutil::TreeModel::Ptr _remappings;
    wxutil::TreeView* _remappingList;
    wxWeakRef<wxTextCtrl> _sourceMaterialEdit;
    wxWeakRef<wxTextCtrl> _replacementMaterialEdit;
    wxWeakRef<wxBitmapButton> _sourceMaterialBrowseBtn;

    wxutil::WindowPosition _windowPosition;
    wxutil::PanedPosition _leftPanePosition;
    wxutil::PanedPosition _rightPanePosition;

    bool _controlUpdateInProgress;
    bool _skinUpdateInProgress;

    sigc::connection _skinModifiedConn;

private:
    SkinEditor();
    ~SkinEditor() override;

public:
    int ShowModal() override;

    static void ShowDialog(const cmd::ArgumentList& args);

protected:
    bool _onDeleteEvent() override;

private:
    void setupModelTreeView();
    void setupSkinTreeView();
    void setupSelectedModelList();
    void setupRemappingPanel();
    void setupPreview();

    decl::ISkin::Ptr getSelectedSkin();
    std::string getSelectedModelFromTree();
    std::string getSelectedSkinModel();
    std::string getSelectedRemappingSourceMaterial();

    void updateSkinButtonSensitivity();
    void updateModelButtonSensitivity();
    void updateSkinControlsFromSelection();
    void updateModelControlsFromSkin(const decl::ISkin::Ptr& skin);
    void updateRemappingControlsFromSkin(const decl::ISkin::Ptr& skin);
    void updateSourceView(const decl::ISkin::Ptr& skin);
    void updateSkinTreeItem();
    void updateModelPreview();
    void updateRemappingButtonSensitivity();
    void updateDeclFileInfo();

    void onSkinNameChanged(wxCommandEvent& ev);
    void onCloseButton(wxCommandEvent& ev);
    void onAddModelToSkin(wxCommandEvent& ev);
    void onRemoveModelFromSkin(wxCommandEvent& ev);
    void onModelTreeSelectionChanged(wxDataViewEvent& ev);
    void onSkinModelSelectionChanged(wxDataViewEvent& ev);
    void onSkinSelectionChanged(wxDataViewEvent& ev);
    void handleSkinSelectionChanged();
    void onRemappingValueChanged(wxDataViewEvent& ev);
    void onRemoveSelectedMapping(wxCommandEvent& ev);
    void onRemappingSelectionChanged(wxDataViewEvent& ev);
    void onPopulateMappingsFromModel(wxCommandEvent& ev);
    void onOriginalEntryChanged(const std::string& material);
    void onReplacementEntryChanged(const std::string& material);
    void onSaveChanges(wxCommandEvent& ev);
    void onDiscardChanges(wxCommandEvent& ev);
    void onNewSkin(wxCommandEvent& ev);
    void onCopySkin(wxCommandEvent& ev);
    void onDeleteSkin(wxCommandEvent& ev);
    void onSkinDeclarationChanged();
    void chooseRemappedSourceMaterial();
    void chooseRemappedDestMaterial();

    bool saveChanges();
    void discardChanges();
    void deleteSkin();
    void populateSkinListWithModelMaterials();
    bool skinHasBeenNewlyCreated();

    bool askUserAboutModifiedSkin();
    bool okToCloseDialog();

    template<typename ObjectClass>
    ObjectClass* getControl(const std::string& name)
    {
        return findNamedObject<ObjectClass>(this, name);
    }
};

}
