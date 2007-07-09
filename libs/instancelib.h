/*
Copyright (C) 2001-2006, William Joseph.
All Rights Reserved.

This file is part of GtkRadiant.

GtkRadiant is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

GtkRadiant is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GtkRadiant; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#if !defined (INCLUDED_INSTANCELIB_H)
#define INCLUDED_INSTANCELIB_H

#include "debugging/debugging.h"

#include "nameable.h"
#include "iscenegraph.h"

#include "scenelib.h"
#include "generic/reference.h"
#include "generic/callback.h"
#include <map>
#include <stack>

class InstanceSubgraphWalker : 
	public scene::Traversable::Walker
{
	mutable scene::Path m_path;
	mutable std::stack<scene::Instance*> m_parent;
public:
	InstanceSubgraphWalker(const scene::Path& path, scene::Instance* parent) :  
		m_path(path)
	{
		// Initialise the stack with the first element
		m_parent.push(parent);
	}

	bool pre(scene::INodePtr node) const {
		m_path.push(node);
		
		// greebo: Check for an instantiable node (every node should be)
		scene::InstantiablePtr instantiable = Node_getInstantiable(node); 
		if (instantiable != NULL) {
			// Instantiate this node with the reference to the current parent instance
			scene::Instance* instance = instantiable->create(m_path, m_parent.top());
			
			// greebo: Register this instance with the scenegraph 
			GlobalSceneGraph().insert(instance);
			
			instantiable->insert(m_path, instance);
			
			// Make this instance the new parent as long as we're 
			// traversing this subgraph. The parent is removed from the stack
			// in the post() method again.
			m_parent.push(instance);		
		}
		else {
			// Could not find instantiable, something bad has happened
			std::cout << "InstanceSubgraphWalker::pre Node Type: " << nodetype_get_name(node_get_nodetype(node)) << "\n";
			NameablePtr nameable = boost::dynamic_pointer_cast<Nameable>(node);
			if (nameable != NULL) {
				std::cout << "Could not cast node on instantiable: " << nameable->name() << "\n";
			}
		}

		return true;
	}

	void post(scene::INodePtr node) const {
		// The subgraph has been traversed, remove the top stack elements
		m_path.pop();
		m_parent.pop();
	}
};

/** greebo: This Walker un-instantiates the visited nodes (the ones
 * 			that are Instantiable, that is). The whole subgraph is
 * 			traversed and erase() is called on the according nodes.
 * 
 * 			Additionally, the observer is invoked after erase() and
 * 			the scene::Instance is actually deleted from the heap. 
 */
class UninstanceSubgraphWalker : 
	public scene::Traversable::Walker
{
	mutable scene::Path m_path;
public:
	UninstanceSubgraphWalker(const scene::Path& parent) : 
		m_path(parent) // Initialise the path with the given parent path
	{}
	
	bool pre(scene::INodePtr node) const {
		// Just remember what path we're going: push the node
		m_path.push(node);
		return true;
	}
	
	void post(scene::INodePtr node) const {
		// Call erase() on the Instantiable.
		scene::Instance* instance = Node_getInstantiable(node)->erase(m_path);
		
		// Notify the Scenegraph about the upcoming deletion
		GlobalSceneGraph().erase(instance);
		
		// Actually delete the instance
		delete instance;
		
		// We're done with this node, pop the path stack
		m_path.pop();
	}
};

/** greebo: An InstanceSet is part of a BrushNode, PatchNode, Doom3GroupNode, etc. and
 * contains all the child instances of a those nodes. When the actual Brush gets changed
 * (transformed, bounds changed, etc.), the Brush/Doom3Group/Patch/whatever emits a call
 * to this InstanceSet using the transformChangedCaller() and similar stuff.
 * 
 * Additionally, this InstanceSet functions as Traversable::Observer, i.e. it gets
 * notified as soon as any child node is added to its "owner node". The insertChild()
 * call triggers an InstanceSubGraphWalk, so basically this makes sure that every
 * newly inserted Node gets automatically instantiated.   
 */
class InstanceSet : 
	public scene::Traversable::Observer
{
	// The map of Instances, indexed by the Path to the instance
	typedef std::map<PathConstReference, scene::Instance*> InstanceMap;

	// The actual map of the instances
	InstanceMap m_instances;
public:

	typedef InstanceMap::iterator iterator;

	iterator begin() {
		return m_instances.begin();
	}
	iterator end() {
		return m_instances.end();
	}

	// traverse observer
	// greebo: This inserts the given node as child of this instance set.
	// The call arriving at Doom3GroupNode::insert() is passed here, for example.
	// This ensures that every new child node of the observed Traversable is automatically instantiated 
	void insertChild(scene::INodePtr child) {
		for (iterator i = begin(); i != end(); ++i) {
			Node_traverseSubgraph(child, InstanceSubgraphWalker(i->first, i->second));
			i->second->boundsChanged();
		}
	}
	
	void eraseChild(scene::INodePtr child) {
		for (iterator i = begin(); i != end(); ++i) {
			Node_traverseSubgraph(child, UninstanceSubgraphWalker(i->first));
			i->second->boundsChanged();
		}
	}

	// Cycle through all the instances with the given visitor class
	void forEachInstance(const scene::Instantiable::Visitor& visitor) {
		for (iterator i = begin(); i != end(); ++i) {
			visitor.visit(*i->second);
		}
	}

	// The Instantiable node passes the call here, so this is the actual Instantiable implementation
	void insert(const scene::Path& path, scene::Instance* instance)
	{
		//std::cout << "InstanceSet::insert (this=" << this << "): " << path << ", Instance=" << instance->path().top() << "\n";
		ASSERT_MESSAGE(m_instances.find(PathConstReference(instance->path())) == m_instances.end(), "InstanceSet::insert - element already exists");
		m_instances.insert(InstanceMap::value_type(PathConstReference(instance->path()), instance));
		//std::cout << "Size after insert: " << m_instances.size() << "\n";
	}
	
	// The Instantiable node passes the call here, so this is the actual Instantiable implementation
	scene::Instance* erase(const scene::Path& path) {
		ASSERT_MESSAGE(m_instances.find(PathConstReference(path)) != m_instances.end(), "InstanceSet::erase - failed to find element");
		InstanceMap::iterator i = m_instances.find(PathConstReference(path));
		scene::Instance* instance = i->second;
		m_instances.erase(i);
		return instance;
	}

	// greebo: This is called by an Node-contained class like Brush/Patch/Doom3Group/Light, etc.
	// Gets called after the transformation of the contained object changes - this method
	// passes the call on to all the other child instances of the node. 
	void transformChanged()	{
		for (InstanceMap::iterator i = m_instances.begin(); i != m_instances.end(); ++i) {
			i->second->transformChanged();
		}
	}
	typedef MemberCaller<InstanceSet, &InstanceSet::transformChanged> TransformChangedCaller;
	
	void boundsChanged() {
		for (InstanceMap::iterator i = m_instances.begin(); i != m_instances.end(); ++i) {
			i->second->boundsChanged();
		}
	}
	typedef MemberCaller<InstanceSet, &InstanceSet::boundsChanged> BoundsChangedCaller;

}; // class InstanceSet

template<typename Functor>
inline void InstanceSet_forEach(InstanceSet& instances, const Functor& functor)
{
  for(InstanceSet::iterator i = instances.begin(), end = instances.end(); i != end; ++i)
  {
    functor(*i->second);
  }
}

template<typename Type>
class InstanceEvaluateTransform
{
public:
  inline void operator()(scene::Instance& instance) const
  {
  	Type* t = dynamic_cast<Type*>(&instance);
  	if (t != NULL) {
  		t->evaluateTransform();
  	}
  }
};

template<typename Type>
class InstanceSetEvaluateTransform
{
public:
  static void apply(InstanceSet& instances)
  {
    InstanceSet_forEach(instances, InstanceEvaluateTransform<Type>());
  }
  typedef ReferenceCaller<InstanceSet, &InstanceSetEvaluateTransform<Type>::apply> Caller;
};

#endif
