#ifndef RENDERABLEPICOMODEL_H_
#define RENDERABLEPICOMODEL_H_

#include "imodel.h"
#include "cullable.h"
#include "picomodel.h"
#include "math/aabb.h"

#include <boost/shared_ptr.hpp>

/* FORWARD DECLS */
namespace model { class RenderablePicoSurface; }
class Renderer;
class RendererLight;
class SelectionTest;
class Selector;

namespace model
{

/* Renderable class containing a model loaded via the picomodel library. A
 * RenderablePicoModel is made up of one or more RenderablePicoSurface objects,
 * each of which contains a number of polygons with the same texture. Rendering
 * a RenderablePicoModel involves rendering all of its surfaces, each of which
 * binds its texture(s) and submits its geometry via OpenGL calls.
 */

class RenderablePicoModel
: public IModel,
  public Cullable
{
	// Vector of renderable surfaces for this model
	typedef std::vector<boost::shared_ptr<RenderablePicoSurface> > SurfaceList;
	SurfaceList _surfVec;
	
	// Local AABB for this model
	AABB _localAABB;

	// Vector of materials used by this model (one for each surface)
	mutable std::vector<std::string> _materialList;
	
private:

	// Update the list of materials by querying each surface for its current
	// material.
	void updateMaterialList() const;
	
public:

	/** 
	 * Constructor. Accepts a picoModel_t struct containing the raw model data
	 * loaded from picomodel, and a string filename extension to allow the
	 * correct handling of material paths (which differs between ASE and LWO)
	 */
	
	RenderablePicoModel(picoModel_t* mod, const std::string& fExt);
		
	/**
	 * Front-end render function used by the main renderer.
	 * 
	 * @param rend
	 * The sorting Renderer object which accepts renderable geometry.
	 * 
	 * @param localToWorld
	 * Object to world-space transform.
	 */
	void submitRenderables(Renderer& rend, const Matrix4& localToWorld);		
		
	/** 
	 * Back-end render function from OpenGLRenderable. This is called from the
	 * model selector but not the main renderer, which uses the front-end render
	 * method.
	 */
	void render(RenderStateFlags flags) const;

	/**
	 * Add a light to the set of lights illuminating this model. This is used
	 * during rendering.
	 */
	void addLight(const RendererLight& light, const Matrix4& localToWorld);
	
	/**
	 * Clear out the set of lights illuminating this model.
	 */
	void clearLights();
	
	/** 
	 * Return the number of surfaces in this model.
	 */
	int getSurfaceCount() const {
		return _surfVec.size();
	}
	
	/** 
	 * Return the number of vertices in this model, by summing the vertex
	 * counts for each surface.
	 */
	int getVertexCount() const;
	
	/** Return the polycount (tricount) of this model by summing the surface
	 * polycounts.
	 */
	 
	int getPolyCount() const;
	
	/** 
	 * Return the enclosing AABB for this model.
	 */
	const AABB& localAABB() const {
		return _localAABB;
	}
	
	/** Return the list of active materials for this model.
	 */
	const std::vector<std::string>& getActiveMaterials() const;
	
	/** Apply the given skin to this model.
	 */
	void applySkin(const ModelSkin& skin);

	/**
	 * Test this model for intersection with the provided VolumeTest. This is
	 * a simple AABB check. Defined in Cullable interface.
	 * 
	 * @returns
	 * VolumeIntersectionValue enumeration with the intersection result.
	 */
	VolumeIntersectionValue intersectVolume(const VolumeTest&,
											const Matrix4&) const;
											
	/**
	 * Selection test. Test each surface against the SelectionTest object and
	 * if the surface is selected, add it to the selector.
	 * 
	 * @param selector
	 * Selector object which builds a list of selectables.
	 * 
	 * @param test
	 * The SelectionTest object defining the 3D properties of the selection.
	 * 
	 * @param localToWorld
	 * Object to world space transform.
	 */
	void testSelect(Selector& selector, 
					SelectionTest& test, 
					const Matrix4& localToWorld);
	
};

}

#endif /*RENDERABLEPICOMODEL_H_*/
