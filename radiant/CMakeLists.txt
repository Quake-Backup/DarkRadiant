add_executable(darkradiant
               camera/CameraSettings.cpp
               camera/CamWnd.cpp
               camera/CameraWndManager.cpp
               clipboard/ClipboardModule.cpp
               eventmanager/Accelerator.cpp
               eventmanager/EventManager.cpp
               eventmanager/GlobalKeyEventFilter.cpp
               eventmanager/MouseToolGroup.cpp
               eventmanager/MouseToolManager.cpp
               eventmanager/Statement.cpp
               eventmanager/Toggle.cpp
               eventmanager/WidgetToggle.cpp
               main.cpp
               map/AutoSaveTimer.cpp
               map/StartupMapLoader.cpp
               RadiantApp.cpp
               selection/SceneManipulateMouseTool.cpp
               selection/ManipulateMouseTool.cpp
               selection/SelectionMouseTools.cpp
               settings/LocalisationModule.cpp
               settings/LocalisationProvider.cpp
               settings/Win32Registry.cpp
               textool/tools/TextureToolManipulateMouseTool.cpp
               textool/TexTool.cpp
               ui/aas/AasFileControl.cpp
               ui/aas/AasVisualisationPanel.cpp
               ui/aas/RenderableAasFile.cpp
               ui/about/AboutDialog.cpp
               ui/brush/FindBrush.cpp
               ui/brush/QuerySidesDialog.cpp
               ui/commandlist/CommandList.cpp
               ui/commandlist/ShortcutChooser.cpp
               ui/common/EntityChooser.cpp
               ui/common/ImageFilePopulator.cpp
               ui/common/ImageFileSelector.cpp
               ui/common/MapPreview.cpp
               ui/common/TexturePreviewCombo.cpp
               ui/common/SkinChooser.cpp
               ui/common/SoundChooser.cpp
               ui/common/SoundShaderPreview.cpp
               ui/console/CommandEntry.cpp
               ui/console/Console.cpp
               ui/DispatchEvent.cpp
               ui/Documentation.cpp
               ui/eclasstree/EClassTreeBuilder.cpp
               ui/eclasstree/EClassTree.cpp
               ui/einspector/AddPropertyDialog.cpp
               ui/einspector/Algorithm.cpp
               ui/einspector/AnglePropertyEditor.cpp
               ui/einspector/BooleanPropertyEditor.cpp
               ui/einspector/ClassnamePropertyEditor.cpp
               ui/einspector/ColourPropertyEditor.cpp
               ui/einspector/EntityInspector.cpp
               ui/einspector/EntityInspectorModule.cpp
               ui/einspector/EntityPropertyEditor.cpp
               ui/einspector/FloatPropertyEditor.cpp
               ui/einspector/FxPropertyEditor.cpp
               ui/einspector/InheritPropertyEditor.cpp
               ui/einspector/ModelPropertyEditor.cpp
               ui/einspector/PropertyEditor.cpp
               ui/einspector/PropertyEditorFactory.cpp
               ui/einspector/SkinPropertyEditor.cpp
               ui/einspector/SoundPropertyEditor.cpp
               ui/einspector/TexturePropertyEditor.cpp
               ui/einspector/Vector3PropertyEditor.cpp
               ui/entitylist/EntityList.cpp
               ui/entitylist/GraphTreeModel.cpp
               ui/favourites/FavouritesBrowser.cpp
               ui/favourites/FavouritesUserInterfaceModule.cpp
               ui/filters/editor/FilterDialog.cpp
               ui/filters/editor/FilterEditor.cpp
               ui/filters/FilterContextMenu.cpp
               ui/filters/FilterOrthoContextMenuItem.cpp
               ui/filters/FiltersMainMenu.cpp
               ui/filters/FilterUserInterface.cpp
               ui/findshader/FindShader.cpp
               ui/fx/FxChooser.cpp
               ui/gl/WxGLWidgetManager.cpp
               ui/grid/GridUserInterface.cpp
               ui/layers/CreateLayerDialog.cpp
               ui/layers/LayerContextMenu.cpp
               ui/layers/LayerControlPanel.cpp
               ui/layers/LayerOrthoContextMenuItem.cpp
               ui/lightinspector/LightInspector.cpp
               ui/LongRunningOperationHandler.cpp
               ui/mainframe/AuiManager.cpp
               ui/mainframe/AuiFloatingFrame.cpp
               ui/mainframe/AuiLayout.cpp
               ui/mainframe/MainFrame.cpp
               ui/mainframe/PropertyNotebook.cpp
               ui/mainframe/ScreenUpdateBlocker.cpp
               ui/mainframe/TopLevelFrame.cpp
               ui/mainframe/ViewMenu.cpp
               ui/mapselector/MapSelector.cpp
               ui/materials/editor/MaterialEditor.cpp
               ui/materials/editor/MaterialEditorModule.cpp
               ui/materials/editor/MaterialPreview.cpp
               ui/materials/MaterialChooser.cpp
               ui/materials/MaterialPopulator.cpp
               ui/materials/MaterialSelector.cpp
               ui/materials/MaterialTreeView.cpp
               ui/merge/MapMergePanel.cpp
               ui/animationpreview/AnimationPreview.cpp
               ui/animationpreview/MD5AnimationChooser.cpp
               ui/animationpreview/MD5AnimationViewer.cpp
               ui/colourscheme/ColourSchemeEditor.cpp
               ui/common/DialogManager.cpp
               ui/menu/MenuBar.cpp
               ui/menu/MenuElement.cpp
               ui/menu/MenuFolder.cpp
               ui/menu/MenuItem.cpp
               ui/menu/MenuManager.cpp
               ui/menu/MenuSeparator.cpp
               ui/toolbar/ToolbarManager.cpp
               ui/MapCommands.cpp
               ui/MapFileProgressHandler.cpp
               ui/mapinfo/EntityInfoTab.cpp
               ui/mapinfo/LayerInfoTab.cpp
               ui/mapinfo/MapInfoDialog.cpp
               ui/mapinfo/ModelInfoTab.cpp
               ui/mapinfo/ShaderInfoTab.cpp
               ui/mediabrowser/MediaBrowser.cpp
               ui/mediabrowser/MediaBrowserTreeView.cpp
               ui/mediabrowser/MediaBrowserModule.cpp
               ui/modelexport/ConvertModelDialog.cpp
               ui/modelexport/ExportAsModelDialog.cpp
               ui/modelexport/ExportCollisionModelDialog.cpp
               ui/modelselector/MaterialsList.cpp
               ui/modelselector/ModelSelector.cpp
               ui/modelselector/ModelTreeView.cpp
               ui/mousetool/BindToolDialog.cpp
               ui/mousetool/ToolMappingDialog.cpp
               ui/ortho/OrthoContextMenu.cpp
               ui/overlay/Overlay.cpp
               ui/overlay/OrthoBackgroundPanel.cpp
               ui/particles/ParticleEditor.cpp
               ui/particles/ParticleChooserDialog.cpp
               ui/particles/ParticleSelector.cpp
               ui/patch/BulgePatchDialog.cpp
               ui/patch/CapDialog.cpp
               ui/patch/PatchCreateDialog.cpp
               ui/patch/PatchInspector.cpp
               ui/patch/PatchThickenDialog.cpp
               ui/PointFileChooser.cpp
               ui/prefabselector/PrefabSelector.cpp
               ui/prefdialog/GameSetupDialog.cpp
               ui/prefdialog/GameSetupPage.cpp
               ui/prefdialog/GameSetupPageIdTech.cpp
               ui/prefdialog/GameSetupPageTdm.cpp
               ui/prefdialog/PrefDialog.cpp
               ui/prefdialog/PreferenceItem.cpp
               ui/prefdialog/PrefPage.cpp
               ui/script/ScriptMenu.cpp
               ui/script/ScriptUserInterfaceModule.cpp
               ui/script/ScriptWindow.cpp
               ui/selectionset/SelectionSetToolmenu.cpp
               ui/skin/SkinEditor.cpp
               ui/skin/SkinEditorTreeView.cpp
               ui/splash/Splash.cpp
               ui/statusbar/CommandStatus.cpp
               ui/statusbar/EditingStopwatchStatus.cpp
               ui/statusbar/MapStatistics.cpp
               ui/statusbar/StatusBarManager.cpp
               ui/surfaceinspector/SurfaceInspector.cpp
               ui/texturebrowser/MapTextureBrowser.cpp
               ui/texturebrowser/TextureThumbnailBrowser.cpp
               ui/texturebrowser/TextureBrowserPanel.cpp
               ui/texturebrowser/TextureBrowserManager.cpp
               ui/toolbar/ToolbarManager.cpp
               ui/transform/TransformPanel.cpp
               ui/UserInterfaceModule.cpp
               xyview/GlobalXYWnd.cpp
               xyview/tools/BrushCreatorTool.cpp
               xyview/tools/ClipperTool.cpp
               xyview/tools/MeasurementTool.cpp
               xyview/OrthoView.cpp)
target_compile_options(darkradiant PRIVATE ${FREETYPE_CFLAGS} ${GLIB_CFLAGS})
target_include_directories(darkradiant PRIVATE .)
target_link_libraries(darkradiant PRIVATE
                      math xmlutil scene wxutil module
                      ${GLIB_LIBRARIES})

# Enable precompiled header for radiant
if (${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.16.0")
target_precompile_headers(darkradiant PRIVATE
                          "$<$<COMPILE_LANGUAGE:CXX>:precompiled.h>")
endif()
