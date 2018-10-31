#include "Quadtree.h"
#include "GameObject.h"
#include "Glew/include/glew.h"

QuadTreeNode::QuadTreeNode(const math::AABB &bounding_box, QuadTreeNode* parent)
{
	this->bounding_box = bounding_box;
	this->parent = parent;

	for (uint i = 0; i < 4; ++i)
			childrens[i] = nullptr;
}

QuadTreeNode::~QuadTreeNode()
{
	for (uint i = 0; i < 4; ++i)
	{
		if (childrens[i] != nullptr)
		{
			childrens[i]->objects.clear();
			RELEASE(childrens[i]);
		}
	}

	if (parent != nullptr)
		RELEASE(parent);
}

void QuadTreeNode::Insert(GameObject * go)
{
	if (objects.size() < QUADTREE_MAX_ITEMS)
	{
		objects.push_back(go);
	}
	else
	{
		SubdivideNode();

		objects.push_back(go);
		DistributeChildrens();
	}
}

void QuadTreeNode::SubdivideNode()
{
	const math::float3 size_box = bounding_box.Size(); //Take the size of actual boundig box
	const math::float3 subdivision_size = { size_box.x*0.5F, size_box.y*0.5F, size_box.z*0.5F }; //Calculate the new size for subdivision bounding box

	const math::float3 center_bounding_box = bounding_box.CenterPoint(); //Center of actual boundig box
	math::float3 box_child_center;
	math::AABB child_bounding_box;

	//Now create the new child's node from this node

	//TOP-LEFT 
	box_child_center = { center_bounding_box.x - subdivision_size.x /2, center_bounding_box.y + subdivision_size.y/2 , center_bounding_box.z - subdivision_size.z/2 };
	child_bounding_box.SetFromCenterAndSize(box_child_center, subdivision_size);
	childrens[SubdivideChildsPosition::UP_LEFT] = new QuadTreeNode(child_bounding_box, this);

	//TOP-RIGHT 
	box_child_center = { center_bounding_box.x + subdivision_size.x/2, center_bounding_box.y + subdivision_size.y / 2 , center_bounding_box.z - subdivision_size.z/2 };
	child_bounding_box.SetFromCenterAndSize(box_child_center, subdivision_size);
	childrens[SubdivideChildsPosition::UP_RIGHT] = new QuadTreeNode(child_bounding_box, this);

	//DOWN_LEFT 
	box_child_center = { center_bounding_box.x - subdivision_size.x/2, center_bounding_box.y - subdivision_size.y / 2 , center_bounding_box.z + subdivision_size.z/2 };
	child_bounding_box.SetFromCenterAndSize(box_child_center, subdivision_size);
	childrens[SubdivideChildsPosition::DOWN_LEFT] = new QuadTreeNode(child_bounding_box, this);

	//DOWN_RIGHT
	box_child_center = { center_bounding_box.x + subdivision_size.x/2, center_bounding_box.y - subdivision_size.y / 2 , center_bounding_box.z + subdivision_size.z/2 };
	child_bounding_box.SetFromCenterAndSize(box_child_center, subdivision_size);
	childrens[SubdivideChildsPosition::DOWN_RIGHT] = new QuadTreeNode(child_bounding_box, this);
}

bool QuadTreeNode::isLeaf() const
{
	return childrens[0] == nullptr;
}

template<class TYPE>
void QuadTreeNode::CollectIntersections(std::vector<GameObject*> &objects, const TYPE & primitive)
{
	if (primitive.Intersects(object))
	{
		for (std::vector<GameObject*>::const_iterator it = this->objects.begin(); it != this->objects.end(); ++i)
		{
			if (primitive.Intersects((*it)->bounding_box))
				objects.push_back(*it);
		}
		
		if (!isLeaf) 
		{
			for (int i = 0; i < 4; ++i)
			{
				if (childrens[i] != nullptr)
					childrens[i]->CollectIntersections(objects, primitive);
			}
		}
	}

}

void QuadTreeNode::DistributeChildrens()
{
	std::list<GameObject*>::const_iterator it = objects.begin();

	while (it != objects.end())
	{
		//NEED THINK FROM HOW TO DO THIS :/
		uint intersections = 0;
		bool is_intersecting[4];

		for (uint i = 0; i < 4; ++i)
		{
			if (is_intersecting[i] = (*it)->GetAABB().Intersects(childrens[i]->bounding_box))
			{
				intersections++;
			}
		}

		if (intersections == 1)
		{
			for (uint i = 0; i < 4; i++)
			{
				if (is_intersecting[i])
				{
					childrens[i]->Insert(*it);
					it = objects.erase(it);
				}
			}
		}
		else
			it++;
	}
}

void QuadTreeNode::Remove(GameObject* go)
{
	std::list<GameObject*>::iterator it = std::find(objects.begin(), objects.end(), go);

	if (it != objects.end())
	{
		objects.erase(it);
	}

	if (!isLeaf())
	{
		for (uint i = 0; i < 4; ++i)
		{
			childrens[i]->Remove(go);
		}
	}
}

void QuadTreeNode::DebugDraw()
{
	

	for (uint i = 0; i < 12; i++)
	{
		glVertex3f(bounding_box.Edge(i).a.x, bounding_box.Edge(i).a.y, bounding_box.Edge(i).a.z);
		glVertex3f(bounding_box.Edge(i).b.x, bounding_box.Edge(i).b.y, bounding_box.Edge(i).b.z);
	}

	if (childrens[0] != nullptr)
	{
		for (uint i = 0; i < 4; ++i)
		{
			childrens[i]->DebugDraw();
		}
	}


}

void QuadTreeNode::Clear()
{
	if (childrens[0] != nullptr) // not best size 0 ?
	{
		for (uint i = 0; i < 4; ++i)
		{
			childrens[i]->Clear();
			RELEASE(childrens[i]);
		}
	}
	objects.clear();
}



QuadTree::QuadTree()
{
}

QuadTree::~QuadTree()
{
	RELEASE(root_node);
}

void QuadTree::Insert(GameObject* go)
{
	if (root_node != nullptr)
	{
		if (go->GetAABB().Intersects(root_node->bounding_box))
		{
			root_node->Insert(go);
		}
	}
}

void QuadTree::SetBoundary(const math::AABB & boundary)
{
	if(root_node != nullptr)
		Clear();

	root_node = new QuadTreeNode(boundary, nullptr);
}

void QuadTree::Remove(GameObject* go)
{
	if (root_node != nullptr)
		root_node->Remove(go);
}

void QuadTree::Clear()
{
	if (root_node != nullptr)
	{
		root_node->Clear();
		root_node->objects.clear(); // it's neccesary??
		RELEASE(root_node);
	}
}

void QuadTree::DebugDraw()
{
	glBegin(GL_LINES);
	glLineWidth(3.0f);
	glColor4f(0.25f, 1.0f, 1.0f, 1.0f);

	if (root_node != nullptr)
		root_node->DebugDraw();

	glEnd();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}