#include "ManipulatorComponents.h"

#include "ientity.h"
#include "itransformable.h"
#include "igrid.h"
#include "math/FloatTools.h"
#include "math/Ray.h"
#include "pivot.h"
#include "string/convert.h"

namespace selection
{

void transform_local2object(Matrix4& object, const Matrix4& local, const Matrix4& local2object)
{
	object = local2object.getMultipliedBy(local).getMultipliedBy(local2object.getFullInverse());
}

void translation_local2object(Vector3& object, const Vector3& local, const Matrix4& local2object)
{
	object = local2object.getTranslatedBy(local).getMultipliedBy(local2object.getFullInverse()).translation();
}

Vector3 ManipulatorComponentBase::getPlaneProjectedPoint(const Matrix4& pivot2world, const VolumeTest& view, const Vector2& devicePoint)
{
	Matrix4 device2pivot = constructDevice2Pivot(pivot2world, view);

	// greebo: We need to know the z-distance (or depth) of the pivot plane in device coordinates.
	// x and y are defined by the mouse clicks, but we need the third depth component to pass into the
	// device2pivot matrix. Luckily, this value can be extracted from the pivot2device matrix itself,
	// because the distance of that plane in device space is stored in the tz matrix component.
	// The trick is to invert the device2pivot matrix to get the tz value, to get a complete 4D point
	// to transform back into pivot space.
	Matrix4 pivot2device = constructPivot2Device(pivot2world, view);

	// This is now the complete 4D point to transform back into pivot space
	Vector4 point(devicePoint.x(), devicePoint.y(), pivot2device.tz(), 1);

	// Running the point through the device2pivot matrix will give us the mouse coordinates relative to pivot origin
	return device2pivot.transform(point).getProjected();
}

Vector3 ManipulatorComponentBase::getSphereIntersection(const Matrix4& pivot2world, const VolumeTest& view, const Vector2& devicePoint)
{
	// Construct a ray to intersect with a defined sphere
	
	// The device coords x,y generated by the mouse click already defines a ray in 3D space,
	// we just need to pick a z coordinate for start and end. For simplicity we use z=-1 and z=+1
	// which resolves to the near and far plane in device space.
	Vector4 deviceOrigin(devicePoint.x(), devicePoint.y(), -1, 1); // point on near plane
	Vector4 deviceEnd(devicePoint.x(), devicePoint.y(), +1, 1); // point on far plane

	// Run these points through the device2pivot matrix and construct the ray
	Matrix4 device2pivot = constructDevice2Pivot(pivot2world, view);

	Vector3 rayOrigin = device2pivot.transform(deviceOrigin).getProjected();
	Vector3 rayDirection = (device2pivot.transform(deviceEnd).getProjected() - rayOrigin).getNormalised();

	// Construct the ray and return the intersection or the Ray's nearest point to the sphere
	Ray ray(rayOrigin, rayDirection);

	Vector3 intersectionPoint;
	ray.intersectSphere(Vector3(0, 0, 0), 64.0, intersectionPoint);

	return intersectionPoint;
}

Vector3 ManipulatorComponentBase::getAxisConstrained(const Vector3& direction, const Vector3& axis)
{
	// Subtract anything that points along the axis from the direction vector
	// after this step, the direction vector is perpendicular to axis.
	return (direction - axis*direction.dot(axis)).getNormalised();
}

Vector3::ElementType ManipulatorComponentBase::getAngleForAxis(const Vector3& a, const Vector3& b, const Vector3& axis)
{
	if (axis.dot(a.crossProduct(b)) > 0.0)
	{
		return a.angle(b);
	}
	else
	{
		return -a.angle(b);
	}
}

// ===============================================================================================

void RotateFree::beginTransformation(const Matrix4& pivot2world, const VolumeTest& view, const Vector2& devicePoint)
{
	_start = getSphereIntersection(pivot2world, view, devicePoint);
    _start.normalise();
}

void RotateFree::transform(const Matrix4& pivot2world, const VolumeTest& view, const Vector2& devicePoint, unsigned int constraintFlags)
{
	Vector3 current = getSphereIntersection(pivot2world, view, devicePoint);
	current.normalise();

	// call the Rotatable with its transform method
	Quaternion rotation = Quaternion::createForUnitVectors(_start, current);

	_rotatable.rotate(rotation);
}

// ===============================================================================================

void RotateAxis::beginTransformation(const Matrix4& pivot2world, const VolumeTest& view, const Vector2& devicePoint)
{
	_start = getSphereIntersection(pivot2world, view, devicePoint);

	// Constrain the start vector to an axis
	_start = getAxisConstrained(_start, _axis);
}

/// \brief Converts current position to a normalised vector orthogonal to axis.
void RotateAxis::transform(const Matrix4& pivot2world, const VolumeTest& view, const Vector2& devicePoint, unsigned int constraintFlags)
{
	Vector3 current = getSphereIntersection(pivot2world, view, devicePoint);

	// Constrain the start vector to an axis
	current = getAxisConstrained(current, _axis);

	Vector3::ElementType angle = getAngleForAxis(_start, current, _axis);

	if (constraintFlags & Constraint::Type1)
	{
		angle = float_snapped(angle, 5 * c_DEG2RADMULT);
	}

	_curAngle = angle;

	_rotatable.rotate(Quaternion::createForAxisAngle(_axis, angle));
}

// ===============================================================================================

void TranslateAxis::beginTransformation(const Matrix4& pivot2world, const VolumeTest& view, const Vector2& devicePoint)
{
	_start = getPlaneProjectedPoint(pivot2world, view, devicePoint);
}

void TranslateAxis::transform(const Matrix4& pivot2world, const VolumeTest& view, const Vector2& devicePoint, unsigned int constraintFlags)
{
	// Get the regular difference between the starting point and the current mouse point
	Vector3 current = getPlaneProjectedPoint(pivot2world, view, devicePoint);
	Vector3 diff = current - _start;

	// Project this diff vector to our constraining axis
	Vector3 axisProjected = _axis * diff.dot(_axis);
	
	// Snap to grid if the constraint flag is set
	if (constraintFlags & Constraint::Grid)
	{
		// Snap and apply translation
		axisProjected.snap(GlobalGrid().getGridSize());
	}

	_translatable.translate(axisProjected);
}

// ===============================================================================================

void TranslateFree::beginTransformation(const Matrix4& pivot2world, const VolumeTest& view, const Vector2& devicePoint)
{
	// Transform the device coordinates to a point in pivot space 
	// The point is part of the plane going through pivot space origin, orthogonal to the view direction
	_start = getPlaneProjectedPoint(pivot2world, view, devicePoint);
}

void TranslateFree::transform(const Matrix4& pivot2world, const VolumeTest& view, const Vector2& devicePoint, unsigned int constraintFlags)
{
    Vector3 current = getPlaneProjectedPoint(pivot2world, view, devicePoint);
    Vector3 diff = current - _start;

	if (constraintFlags & Constraint::Type1)
	{
		// Locate the index of the component carrying the largest abs value
		int largestIndex = fabs(diff.y()) > fabs(diff.x()) ?
			(fabs(diff.z()) > fabs(diff.y()) ? 2 : 1) :
			(fabs(diff.z()) > fabs(diff.x()) ? 2 : 0);

		// Zero out the other two components
		diff[(largestIndex + 1) % 3] = 0;
		diff[(largestIndex + 2) % 3] = 0;
	}

	// Snap to grid if the constraint flag is set
	if (constraintFlags & Constraint::Grid)
	{
		diff.snap(GlobalGrid().getGridSize());
	}

    _translatable.translate(diff);
}

// ===============================================================================================

void ScaleAxis::beginTransformation(const Matrix4& pivot2world, const VolumeTest& view, const Vector2& devicePoint)
{
	// Transform the device coordinates to a point in pivot space 
	// The point is part of the plane going through pivot space origin, orthogonal to the view direction
	_start = getPlaneProjectedPoint(pivot2world, view, devicePoint);
}

void ScaleAxis::transform(const Matrix4& pivot2world, const VolumeTest& view, const Vector2& devicePoint, unsigned int constraintFlags)
{
	// Get the regular difference between the starting point and the current mouse point
	Vector3 current = getPlaneProjectedPoint(pivot2world, view, devicePoint);
	Vector3 diff = current - _start;

	// Project this diff vector to our constraining axis
	Vector3 axisProjected = _axis * diff.dot(_axis);

	Vector3 start = _start;

	// Snap to grid if the constraint flag is set
	if (constraintFlags & Constraint::Grid)
	{
		diff.snap(GlobalGrid().getGridSize());
		start.snap(GlobalGrid().getGridSize());
	}
	
    Vector3 scale(
      start[0] == 0 ? 1 : 1 + axisProjected[0] / start[0],
      start[1] == 0 ? 1 : 1 + axisProjected[1] / start[1],
      start[2] == 0 ? 1 : 1 + axisProjected[2] / start[2]
    );

    _scalable.scale(scale);
}

// ===============================================================================================

void ScaleFree::beginTransformation(const Matrix4& pivot2world, const VolumeTest& view, const Vector2& devicePoint) 
{
	// Transform the device coordinates to a point in pivot space 
	// The point is part of the plane going through pivot space origin, orthogonal to the view direction
	_start = getPlaneProjectedPoint(pivot2world, view, devicePoint);
}

void ScaleFree::transform(const Matrix4& pivot2world, const VolumeTest& view, const Vector2& devicePoint, unsigned int constraintFlags)
{
	Vector3 current = getPlaneProjectedPoint(pivot2world, view, devicePoint);
	Vector3 diff = current - _start;

	Vector3 start = _start;

	// Snap to grid if the constraint flag is set
	if (constraintFlags & Constraint::Grid)
	{
		diff.snap(GlobalGrid().getGridSize());
		start.snap(GlobalGrid().getGridSize());
	}

    Vector3 scale(
      start[0] == 0 ? 1 : 1 + diff[0] / start[0],
      start[1] == 0 ? 1 : 1 + diff[1] / start[1],
      start[2] == 0 ? 1 : 1 + diff[2] / start[2]
    );

    _scalable.scale(scale);
}

void ModelScaleComponent::setEntityNode(const scene::INodePtr& node)
{
	_entityNode = node;
}

void ModelScaleComponent::setScalePivot(const Vector3& scalePivot)
{
	_scalePivot2World = Matrix4::getTranslation(scalePivot);
}

void ModelScaleComponent::beginTransformation(const Matrix4& pivot2world, const VolumeTest& view, const Vector2& devicePoint)
{
	// We ignore the incoming pivot2world matrix, since we have our own pivot which is set 
	// by the owning Manipulator class
	_start = getPlaneProjectedPoint(_scalePivot2World, view, devicePoint);

	assert(_entityNode);

	Entity* entity = Node_getEntity(_entityNode);

	_startOrigin = string::convert<Vector3>(entity->getKeyValue("origin"));
}

void ModelScaleComponent::transform(const Matrix4& pivot2world, const VolumeTest& view, const Vector2& devicePoint, unsigned int constraintFlags)
{
	Vector3 current = getPlaneProjectedPoint(_scalePivot2World, view, devicePoint);

	if (constraintFlags & Component::Constraint::Grid)
	{
		current.snap(GlobalGrid().getGridSize());
	}

	// In Orthographic views it's entirely possible that the starting point
	// is in the same plane as the pivot, so check for zero divisions
	Vector3 scale(
		_start[0] != 0 ? fabs(current[0]) / fabs(_start[0]) : 1,
		_start[1] != 0 ? fabs(current[1]) / fabs(_start[1]) : 1,
		_start[2] != 0 ? fabs(current[2]) / fabs(_start[2]) : 1
	);

	// Default to uniform scale, use to the value deviating most from the 1.0 scale
	if (!(constraintFlags & Constraint::Type1))
	{
		Vector3 delta = scale - Vector3(1.0, 1.0, 1.0);

		int largestIndex = fabs(delta.y()) > fabs(delta.x()) ?
			(fabs(delta.z()) > fabs(delta.y()) ? 2 : 1) :
			(fabs(delta.z()) > fabs(delta.x()) ? 2 : 0);

		scale.x() = scale.y() = scale.z() = scale[largestIndex];
	}

	// Calculate the origin relative to the pivot
	Vector3 relOrigin = _startOrigin - _scalePivot2World.t().getVector3();

	Vector3 relOriginScaled = relOrigin * scale;

	Vector3 translation = relOriginScaled - relOrigin;

	// Apply the translation
	ITransformablePtr transformable = Node_getTransformable(_entityNode);

	if (transformable)
	{
		transformable->setType(TRANSFORM_PRIMITIVE);
		transformable->setTranslation(translation);
	}

	// Apply the scale to the model beneath the entity
	_entityNode->foreachNode([&](const scene::INodePtr& node)
	{
		ITransformablePtr transformable = Node_getTransformable(node);

		if (transformable)
		{
			transformable->setType(TRANSFORM_PRIMITIVE);
			transformable->setScale(scale);
		}

		return true;
	});

	SceneChangeNotify();
}

SelectionTranslator::SelectionTranslator(const TranslationCallback& onTranslation) :
	_onTranslation(onTranslation)
{}

void SelectionTranslator::translate(const Vector3& translation)
{
	if (GlobalSelectionSystem().Mode() == SelectionSystem::eComponent)
	{
		GlobalSelectionSystem().foreachSelectedComponent(TranslateComponentSelected(translation));
	}
	else
	{
		// Cycle through the selected items and apply the translation
		GlobalSelectionSystem().foreachSelected(TranslateSelected(translation));
	}

	// Invoke the feedback function
	if (_onTranslation)
	{
		_onTranslation(translation);
	}
}

TranslatablePivot::TranslatablePivot(ManipulationPivot& pivot) :
	_pivot(pivot)
{}

void TranslatablePivot::translate(const Vector3& translation)
{
	_pivot.applyTranslation(translation);

	// User is placing the pivot manually, so let's keep it that way
	_pivot.setUserLocked(true);
}

}
