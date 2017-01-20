#pragma once

#include "iselectiontest.h"
#include "irender.h"
#include "itraceable.h"
#include "modelskin.h"
#include "irenderable.h"
#include "render/VectorLightList.h"
#include "RenderablePicoModel.h"
#include "scene/Node.h"
#include "Transformable.h"

namespace model {

class PicoModelNode :
	public scene::Node,
	public ModelNode,
	public SelectionTestable,
	public LitObject,
	public SkinnedModel,
	public ITraceable,
	public Transformable
{
private:
	// The actual model
	RenderablePicoModelPtr _picoModel;

	std::string _name;

	// Vector of RendererLight references which illuminate this instance, set
	// with addLight() and clearLights()
    render::lib::VectorLightList _lights;

	// The light list from the shader cache when we attach
	LightList& _lightList;

	// The name of this model's skin
	std::string _skin;

public:
	/** Construct a PicoModelNode with a reference to the loaded picoModel.
	 */
	PicoModelNode(const RenderablePicoModelPtr& picoModel);

	virtual ~PicoModelNode();

	virtual void onInsertIntoScene(scene::IMapRootNode& root) override;
	virtual void onRemoveFromScene(scene::IMapRootNode& root) override;

	// ModelNode implementation
	virtual const IModel& getIModel() const;
	virtual IModel& getIModel();

	// SkinnedModel implementation
	// Skin changed notify
	void skinChanged(const std::string& newSkinName);
	// Returns the name of the currently active skin
	std::string getSkin() const;

	// Bounded implementation
	virtual const AABB& localAABB() const;

	// Lights changed function
	void lightsChanged()
    {
		_lightList.setDirty();
	}

	// SelectionTestable implementation
	void testSelect(Selector& selector, SelectionTest& test);

	virtual std::string name() const;
	Type getNodeType() const;

	const RenderablePicoModelPtr& getModel() const;
	void setModel(const RenderablePicoModelPtr& model);

	// LitObject test function
	bool intersectsLight(const RendererLight& light) const;
	// Add a light to this model instance
	void insertLight(const RendererLight& light);
	// Clear all lights from this model instance
	void clearLights();

	// Renderable implementation
  	void renderSolid(RenderableCollector& collector, const VolumeTest& volume) const override;
	void renderWireframe(RenderableCollector& collector, const VolumeTest& volume) const override;
	void setRenderSystem(const RenderSystemPtr& renderSystem) override;

	std::size_t getHighlightFlags() override
	{
		return Highlight::NoHighlight; // models are never highlighted themselves
	}

	// Traceable implementation
	bool getIntersection(const Ray& ray, Vector3& intersection);

protected:
	virtual void _onTransformationChanged() override;
	virtual void _applyTransformation() override;

private:
	// Instance render function
	void submitRenderables(RenderableCollector& collector,
						   const VolumeTest& volume,
						   const Matrix4& localToWorld,
						   const IRenderEntity& entity) const;
};
typedef std::shared_ptr<PicoModelNode> PicoModelNodePtr;

} // namespace model
