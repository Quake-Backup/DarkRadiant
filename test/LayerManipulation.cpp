#include "RadiantTest.h"

#include "imap.h"
#include "ilayer.h"
#include "algorithm/Scene.h"
#include "scenelib.h"

namespace test
{

using LayerTest = RadiantTest;

TEST_F(LayerTest, CreateLayerMarksMapAsModified)
{
    loadMap("general_purpose.mapx");

    EXPECT_FALSE(GlobalMapModule().isModified());

    GlobalMapModule().getRoot()->getLayerManager().createLayer("Testlayer");

    EXPECT_TRUE(GlobalMapModule().isModified());
}

TEST_F(LayerTest, RenameLayerMarksMapAsModified)
{
    loadMap("general_purpose.mapx");

    auto& layerManager = GlobalMapModule().getRoot()->getLayerManager();

    EXPECT_FALSE(GlobalMapModule().isModified());
    EXPECT_NE(layerManager.getLayerID("Second Layer"), -1);

    layerManager.renameLayer(layerManager.getLayerID("Second Layer"), "Renamed Layer");

    EXPECT_TRUE(GlobalMapModule().isModified());
}

TEST_F(LayerTest, DeleteLayerMarksMapAsModified)
{
    loadMap("general_purpose.mapx");

    auto& layerManager = GlobalMapModule().getRoot()->getLayerManager();

    EXPECT_FALSE(GlobalMapModule().isModified());
    EXPECT_NE(layerManager.getLayerID("Second Layer"), -1);

    layerManager.deleteLayer("Second Layer");

    EXPECT_TRUE(GlobalMapModule().isModified());
}

void performMoveOrAddToLayerTest(bool moveToLayer)
{
    auto& layerManager = GlobalMapModule().getRoot()->getLayerManager();

    auto brush = algorithm::findFirstBrushWithMaterial(
        GlobalMapModule().findOrInsertWorldspawn(), "textures/numbers/1");
    EXPECT_TRUE(brush);

    Node_setSelected(brush, true);

    EXPECT_FALSE(GlobalMapModule().isModified());

    auto layerId = GlobalMapModule().getRoot()->getLayerManager().getLayerID("Second Layer");
    EXPECT_NE(layerId, -1);

    if (moveToLayer)
    {
        GlobalCommandSystem().executeCommand("MoveSelectionToLayer", cmd::Argument(layerId));
    }
    else
    {
        GlobalCommandSystem().executeCommand("AddSelectionToLayer", cmd::Argument(layerId));
    }

    EXPECT_TRUE(GlobalMapModule().isModified());
}

TEST_F(LayerTest, AddingToLayerMarksMapAsModified)
{
    loadMap("general_purpose.mapx");

    performMoveOrAddToLayerTest(false);
}

TEST_F(LayerTest, MovingToLayerMarksMapAsModified)
{
    loadMap("general_purpose.mapx");

    performMoveOrAddToLayerTest(true);
}

}
