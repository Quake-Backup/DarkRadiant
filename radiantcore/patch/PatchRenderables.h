/**
 * \file
 * These are the renderables that are used in the PatchNode/Patch class to draw
 * the patch onto the screen.
 */
#pragma once

#include <iterator>
#include "igl.h"
#include "imodelsurface.h"

#include "PatchTesselation.h"
#include "PatchControlInstance.h"

#include "render/VertexBuffer.h"
#include "render/IndexedVertexBuffer.h"
#include "render/RenderableGeometry.h"

#if 0
/// Helper class to render a PatchTesselation in solid mode
class RenderablePatchSolid :
	public OpenGLRenderable
#ifdef RENDERABLE_GEOMETRY
    , public RenderableGeometry
#endif
{
    // Geometry source
	PatchTesselation& _tess;

    // VertexBuffer for rendering
    typedef render::IndexedVertexBuffer<ArbitraryMeshVertex> VertexBuffer_T;
    mutable VertexBuffer_T _vertexBuf;

    mutable bool _needsUpdate;

    // The render indices to render the mesh vertices as QUADS
    std::vector<unsigned int> _indices;

public:
	RenderablePatchSolid(PatchTesselation& tess);

	void render(const RenderInfo& info) const;

    void queueUpdate();

#ifdef RENDERABLE_GEOMETRY
    Type getType() const override;
    const Vector3& getFirstVertex() override;
    std::size_t getVertexStride() override;
    const unsigned int& getFirstIndex() override;
    std::size_t getNumIndices() override;

private:
    void updateIndices();
#endif
};
#endif

// Renders a vertex' normal/tangent/bitangent vector (for debugging purposes)
class RenderablePatchVectorsNTB :
	public OpenGLRenderable
{
private:
    std::vector<VertexCb> _vertices;
	const PatchTesselation& _tess;

	ShaderPtr _shader;

public:
	const ShaderPtr& getShader() const;

	RenderablePatchVectorsNTB(const PatchTesselation& tess);

	void setRenderSystem(const RenderSystemPtr& renderSystem);

	void render(const RenderInfo& info) const;

	void render(IRenderableCollector& collector, const VolumeTest& volume, const Matrix4& localToWorld) const;
};

class ITesselationIndexer
{
public:
    virtual ~ITesselationIndexer() {}

    virtual render::GeometryType getType() const = 0;

    // The number of indices generated by this indexer for the given tesselation
    virtual std::size_t getNumIndices(const PatchTesselation& tess) const = 0;

    // Generate the indices for the given tesselation, assigning them to the given insert iterator
    virtual void generateIndices(const PatchTesselation& tess, std::back_insert_iterator<std::vector<unsigned int>> outputIt) const = 0;
};

class TesselationIndexer_Triangles :
    public ITesselationIndexer
{
public:
    render::GeometryType getType() const override
    {
        return render::GeometryType::Triangles;
    }

    std::size_t getNumIndices(const PatchTesselation& tess) const override
    {
        return (tess.height - 1) * (tess.width - 1) * 6; // 6 => 2 triangles per quad
    }

    void generateIndices(const PatchTesselation& tess, std::back_insert_iterator<std::vector<unsigned int>> outputIt) const override
    {
        // Generate the indices to define the triangles in clockwise order
        for (std::size_t h = 0; h < tess.height - 1; ++h)
        {
            auto rowOffset = h * tess.width;

            for (std::size_t w = 0; w < tess.width - 1; ++w)
            {
                outputIt = static_cast<unsigned int>(rowOffset + w + tess.width);
                outputIt = static_cast<unsigned int>(rowOffset + w + 1);
                outputIt = static_cast<unsigned int>(rowOffset + w);
                
                outputIt = static_cast<unsigned int>(rowOffset + w + tess.width);
                outputIt = static_cast<unsigned int>(rowOffset + w + tess.width + 1);
                outputIt = static_cast<unsigned int>(rowOffset + w + 1);
            }
        }
    }
};

class TesselationIndexer_Quads :
    public ITesselationIndexer
{
public:
    render::GeometryType getType() const override
    {
        return render::GeometryType::Quads;
    }

    std::size_t getNumIndices(const PatchTesselation& tess) const override
    {
        return (tess.height - 1) * (tess.width - 1) * 4; // 4 indices per quad
    }

    void generateIndices(const PatchTesselation& tess, std::back_insert_iterator<std::vector<unsigned int>> outputIt) const override
    {
        for (std::size_t h = 0; h < tess.height - 1; ++h)
        {
            auto rowOffset = h * tess.width;

            for (std::size_t w = 0; w < tess.width - 1; ++w)
            {
                outputIt = static_cast<unsigned int>(rowOffset + w);
                outputIt = static_cast<unsigned int>(rowOffset + w + tess.width);
                outputIt = static_cast<unsigned int>(rowOffset + w + tess.width + 1);
                outputIt = static_cast<unsigned int>(rowOffset + w + 1);
            }
        }
    }
};

template<typename TesselationIndexerT>
class RenderablePatchTesselation :
    public render::RenderableGeometry
{
private:
    static_assert(std::is_base_of_v<ITesselationIndexer, TesselationIndexerT>, "Indexer must implement ITesselationIndexer");
    TesselationIndexerT _indexer;

    const PatchTesselation& _tess;
    bool _needsUpdate;

    bool _whiteVertexColour;

public:
    // When whiteVertexColour is set to true, all colour vertex attributes will be set to 1,1,1,1
    RenderablePatchTesselation(const PatchTesselation& tess, bool whiteVertexColour) :
        _tess(tess),
        _needsUpdate(true),
        _whiteVertexColour(whiteVertexColour)
    {}
    
    void queueUpdate()
    {
        _needsUpdate = true;
    }

protected:
    void updateGeometry() override
    {
        if (!_needsUpdate) return;

        _needsUpdate = false;

        // Generate the new index array
        std::vector<unsigned int> indices;
        indices.reserve(_indexer.getNumIndices(_tess));

        _indexer.generateIndices(_tess, std::back_inserter(indices));

        RenderableGeometry::updateGeometry(_indexer.getType(), 
            _whiteVertexColour ? getColouredVertices() : _tess.vertices, indices);
    }

    std::vector<ArbitraryMeshVertex> getColouredVertices()
    {
        std::vector<ArbitraryMeshVertex> vertices;
        vertices.reserve(_tess.vertices.size());

        for (const auto& vertex : _tess.vertices)
        {
            // Copy vertex data, but set the colour to 1,1,1,1
            vertices.push_back(ArbitraryMeshVertex(vertex.vertex, vertex.normal, vertex.texcoord, { 1, 1, 1, 1 }));
        }

        return vertices;
    }
};

// Renders the wireframe lines between a patch's control points
class RenderablePatchLattice :
    public render::RenderableGeometry
{
private:
    const IPatch& _patch;
    const std::vector<PatchControlInstance>& _controlPoints;
    bool _needsUpdate;

public:
    RenderablePatchLattice(const IPatch& patch, const std::vector<PatchControlInstance>& controlPoints) :
        _patch(patch),
        _controlPoints(controlPoints),
        _needsUpdate(true)
    {}

    void queueUpdate()
    {
        _needsUpdate = true;
    }

protected:
    void updateGeometry() override
    {
        if (!_needsUpdate) return;

        _needsUpdate = false;

        auto width = _patch.getWidth();
        auto height = _patch.getHeight();
        assert(width * height == _controlPoints.size());

        std::vector<ArbitraryMeshVertex> vertices;
        vertices.reserve(_controlPoints.size());

        for (const auto& ctrl : _controlPoints)
        {
            vertices.push_back(ArbitraryMeshVertex(ctrl.control.vertex, { 0, 0, 1 }, ctrl.control.texcoord, { 1, 0.5, 0, 1 }));
        }

        // Generate the index array
        std::vector<unsigned int> indices;
        indices.reserve(((width * (height - 1)) + (height * (width - 1))) << 1);

        for (std::size_t h = 0; h < height - 1; ++h)
        {
            auto rowOffset = h * width;

            for (std::size_t w = 0; w < width - 1; ++w)
            {
                indices.push_back(static_cast<unsigned int>(rowOffset + w));
                indices.push_back(static_cast<unsigned int>(rowOffset + w + 1));

                indices.push_back(static_cast<unsigned int>(rowOffset + w));
                indices.push_back(static_cast<unsigned int>(rowOffset + w + width));
            }

            indices.push_back(static_cast<unsigned int>(rowOffset + width - 1));
            indices.push_back(static_cast<unsigned int>(rowOffset + width - 1 + width));
        }

        auto rowOffset = (height - 1) * width;

        for (std::size_t w = 0; w < width - 1; ++w)
        {
            indices.push_back(static_cast<unsigned int>(rowOffset + w));
            indices.push_back(static_cast<unsigned int>(rowOffset + w + 1));
        }

        assert(indices.size() == ((width * (height - 1)) + (height * (width - 1))) << 1);

        RenderableGeometry::updateGeometry(render::GeometryType::Lines, vertices, indices);
    }
};

// Represents the set of coloured points used to manipulate the control point matrix
class RenderablePatchControlPoints :
    public render::RenderableGeometry
{
private:
    bool _needsUpdate;

    const IPatch& _patch;
    const std::vector<PatchControlInstance>& _controlPoints;

public:
    RenderablePatchControlPoints(const IPatch& patch, const std::vector<PatchControlInstance>& controlPoints) :
        _patch(patch),
        _controlPoints(controlPoints),
        _needsUpdate(true)
    {}

    void queueUpdate()
    {
        _needsUpdate = true;
    }

protected:
    void updateGeometry() override
    {
        if (!_needsUpdate) return;

        _needsUpdate = false;

        // Generate the new point vector
        std::vector<ArbitraryMeshVertex> vertices;
        std::vector<unsigned int> indices;

        vertices.reserve(_controlPoints.size());
        indices.reserve(_controlPoints.size());

        static const Vector4 SelectedColour(0, 0, 0, 1);
        auto width = _patch.getWidth();

        for (std::size_t i = 0; i < _controlPoints.size(); ++i)
        {
            const auto& ctrl = _controlPoints[i];

            vertices.push_back(ArbitraryMeshVertex(ctrl.control.vertex, { 0, 0, 0 }, { 0, 0 }, 
                ctrl.isSelected() ? SelectedColour : getColour(i, width)));
            indices.push_back(static_cast<unsigned int>(i));
        }

        RenderableGeometry::updateGeometry(render::GeometryType::Points, vertices, indices);
    }

private:
    inline const Vector4 getColour(std::size_t i, std::size_t width)
    {
        static const Vector3& cornerColourVec = GlobalPatchModule().getSettings().getVertexColour(patch::PatchEditVertexType::Corners);
        static const Vector3& insideColourVec = GlobalPatchModule().getSettings().getVertexColour(patch::PatchEditVertexType::Inside);

        const Vector4 colour_corner(cornerColourVec, 1);
        const Vector4 colour_inside(insideColourVec, 1);

        return (i % 2 || (i / width) % 2) ? colour_inside : colour_corner;
    }
};
