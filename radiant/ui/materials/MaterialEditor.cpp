#include "MaterialEditor.h"

#include "i18n.h"

#include <wx/panel.h>
#include <wx/splitter.h>
#include <wx/textctrl.h>
#include <wx/collpane.h>
#include "wxutil/SourceView.h"
#include "fmt/format.h"

namespace ui
{

namespace
{
    const char* const DIALOG_TITLE = N_("Material Editor");
    const std::string RKEY_ROOT = "user/ui/materialEditor/";
    const std::string RKEY_SPLIT_POS = RKEY_ROOT + "splitPos";
    const std::string RKEY_WINDOW_STATE = RKEY_ROOT + "window";

    // Columns for the stages list
    struct StageColumns :
        public wxutil::TreeModel::ColumnRecord
    {
        StageColumns() :
            name(add(wxutil::TreeModel::Column::String)),
            index(add(wxutil::TreeModel::Column::Integer)),
            visible(add(wxutil::TreeModel::Column::Boolean))
        {}

        wxutil::TreeModel::Column name;
        wxutil::TreeModel::Column index;
        wxutil::TreeModel::Column visible;
    };

    StageColumns& STAGE_COLS()
    {
        static StageColumns _i; 
        return _i; 
    }
}

MaterialEditor::MaterialEditor() :
    DialogBase(DIALOG_TITLE),
    _treeView(nullptr),
    _stageList(new wxutil::TreeModel(STAGE_COLS(), true)),
    _stageView(nullptr)
{
    loadNamedPanel(this, "MaterialEditorMainPanel");

    makeLabelBold(this, "MaterialEditorDefinitionLabel");
    makeLabelBold(this, "MaterialEditorMaterialPropertiesLabel");
    makeLabelBold(this, "MaterialEditorMaterialStagesLabel");
    makeLabelBold(this, "MaterialEditorStageSettingsLabel");

    // Wire up the close button
    getControl<wxButton>("MaterialEditorCloseButton")->Bind(wxEVT_BUTTON, &MaterialEditor::_onClose, this);

    // Add the treeview
    auto* panel = getControl<wxPanel>("MaterialEditorTreeView");
    _treeView = new MaterialTreeView(panel);
    _treeView->Bind(wxEVT_DATAVIEW_SELECTION_CHANGED, &MaterialEditor::_onTreeViewSelectionChanged, this);
    panel->GetSizer()->Add(_treeView, 1, wxEXPAND);

    // Setup the splitter and preview
    auto* splitter = getControl<wxSplitterWindow>("MaterialEditorSplitter");
    splitter->SetSashPosition(GetSize().GetWidth() * 0.6f);
    splitter->SetMinimumPaneSize(10); // disallow unsplitting

    // Set up the preview
    auto* previewPanel = getControl<wxPanel>("MaterialEditorPreviewPanel");
    _preview.reset(new wxutil::ModelPreview(previewPanel));

    _sourceView = new wxutil::D3MaterialSourceViewCtrl(previewPanel);

    previewPanel->GetSizer()->Add(_preview->getWidget(), 1, wxEXPAND);
    previewPanel->GetSizer()->Add(_sourceView, 1, wxEXPAND);

    setupMaterialStageView();

    // Set the default size of the window
    FitToScreen(0.8f, 0.6f);

    Layout();
    Fit();

    // Connect the window position tracker
    _windowPosition.loadFromPath(RKEY_WINDOW_STATE);
    _windowPosition.connect(this);
    _windowPosition.applyPosition();

    _panedPosition.connect(splitter);
    _panedPosition.loadFromPath(RKEY_SPLIT_POS);

    CenterOnParent();

    _treeView->Populate();
}

int MaterialEditor::ShowModal()
{
    // Restore the position
    _windowPosition.applyPosition();

    int returnCode = DialogBase::ShowModal();

    // Tell the position tracker to save the information
    _windowPosition.saveToPath(RKEY_WINDOW_STATE);

    return returnCode;
}

void MaterialEditor::_onClose(wxCommandEvent& ev)
{
    EndModal(wxID_CLOSE);
}

void MaterialEditor::ShowDialog(const cmd::ArgumentList& args)
{
    MaterialEditor* editor = new MaterialEditor;

    editor->ShowModal();
    editor->Destroy();
}

void MaterialEditor::setupMaterialStageView()
{
    // Stage view
    auto* panel = getControl<wxPanel>("MaterialEditorStageView");

    _stageView = wxutil::TreeView::CreateWithModel(panel, _stageList.get(), wxDV_NO_HEADER);
    panel->GetSizer()->Add(_stageView, 1, wxEXPAND);

    // Single text column
    _stageView->AppendTextColumn(_("Stage"), STAGE_COLS().name.getColumnIndex(),
        wxDATAVIEW_CELL_INERT, wxCOL_WIDTH_AUTOSIZE, wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE);
}

void MaterialEditor::_onTreeViewSelectionChanged(wxDataViewEvent& ev)
{
    // Update the preview if a texture is selected
    if (!_treeView->IsDirectorySelected())
    {
        _material = GlobalMaterialManager().getMaterialForName(_treeView->GetSelectedFullname());
    }
    else
    {
        _material.reset();
    }

    updateControlsFromMaterial();
}

void MaterialEditor::updateControlsFromMaterial()
{
    updateMaterialPropertiesFromMaterial();
}

void MaterialEditor::updateMaterialPropertiesFromMaterial()
{
    getControl<wxPanel>("MaterialEditorMaterialPropertiesPanel")->Enable(_material != nullptr);
    
    if (_material)
    {
        getControl<wxTextCtrl>("MaterialDescription")->SetValue(_material->getDescription());

        // Surround the definition with curly braces, these are not included
        auto definition = fmt::format("{0}\n{{{1}}}", _material->getName(), _material->getDefinition());
        _sourceView->SetValue(definition);
    }
    else
    {
        getControl<wxTextCtrl>("MaterialDescription")->SetValue("");
        _sourceView->SetValue("");
    }
}

}
