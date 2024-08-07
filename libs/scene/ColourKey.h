#pragma once

#include <functional>
#include "scene/Entity.h"
#include "irender.h"

/**
 * greebo: this is a class encapsulating the "_color" spawnarg
 * of entity, observing it and maintaining the corresponding shader.
 */
class ColourKey :
	public KeyObserver
{
private:
	ShaderPtr _colourShader;
	Vector3 _colour;

	RenderSystemWeakPtr _renderSystem;

    std::function<void(const std::string&)> _onColourChanged;

public:
    // The given callback will be invoked after the _color spawnarg has been changed
    ColourKey(const std::function<void(const std::string&)>& onColourChanged) :
        _colour(1,1,1),
        _onColourChanged(onColourChanged)
    {
        captureShader();
    }

	const Vector3& getColour() const
	{
		return _colour;
	}

	// Called when "_color" keyvalue changes
	void onKeyValueChanged(const std::string& value) override
	{
		// Initialise the colour with white, in case the string parse fails
		_colour[0] = _colour[1] = _colour[2] = 1;

		// Use a stringstream to parse the string
		std::stringstream strm(value);

		strm << std::skipws;
		strm >> _colour.x();
		strm >> _colour.y();
		strm >> _colour.z();

		captureShader();

        if (_onColourChanged)
        {
            _onColourChanged(value);
        }
	}

	void setRenderSystem(const RenderSystemPtr& renderSystem)
	{
		_renderSystem = renderSystem;

		captureShader();
	}

    /// Return a (possibly empty) reference to the Shader
    const ShaderPtr& getColourShader() const
    {
        return _colourShader;
    }

private:

	void captureShader()
	{
		auto renderSystem = _renderSystem.lock();

		if (renderSystem)
		{
			_colourShader = renderSystem->capture(ColourShaderType::CameraAndOrthoview, _colour);
		}
		else
		{
			_colourShader.reset();
		}
	}
};
