#include "Quadtree.h"
#include "GameObject.h"
#include "Glew/include/glew.h"
#include "mmgr/mmgr.h"

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
	bounding_box.SetNegativeInfinity();

	if (parent != nullptr)
		RELEASE(parent);
}

void QuadTreeNode::Insert(GameObject * go)
{
	if ( isLeaf() == true && objects.size() < QUADTREE_MAX_ITEMS)
	{
		objects.push_back(go);
	}
	else
	{
		if(isLeaf())
			SubdivideNode();

		objects.push_back(go);
		DistributeChildrens();
	}
}

void QuadTreeNode::SubdivideNode()
{
	const math::float3 size_box = bounding_box.Size(); //Take the size of actual boundig box
	const math::float3 subdivision_size = { size_box.x/2, size_box.y, size_box.z/2 }; //Calculate the new size for subdivision bounding box

	const math::float3 center_bounding_box = bounding_box.CenterPoint(); //Center of actual boundig box
	math::float3 box_child_center;
	math::AABB child_bounding_box;

	//Now create the new child's node from this node

	//TOP-LEFT 
	box_child_center = { center_bounding_box.x - subdivision_size.x/2  , center_bounding_box.y  , center_bounding_box.z + subdivision_size.z/2 };
	child_bounding_box.SetFromCenterAndSize(box_child_center, subdivision_size);
	childrens[SubdivideChildsPosition::UP_LEFT] = new QuadTreeNode(child_bounding_box, this);

	//TOP-RIGHT 
	box_child_center = { center_bounding_box.x + subdivision_size.x/2, center_bounding_box.y  , center_bounding_box.z + subdivision_size.z/2 };
	child_bounding_box.SetFromCenterAndSize(box_child_center, subdivision_size);
	childrens[SubdivideChildsPosition::UP_RIGHT] = new QuadTreeNode(child_bounding_box, this);

	//DOWN_LEFT 
	box_child_center = { center_bounding_box.x - subdivision_size.x/2, center_bounding_box.y, center_bounding_box.z - subdivision_size.z/2 };
	child_bounding_box.SetFromCenterAndSize(box_child_center, subdivision_size);
	childrens[SubdivideChildsPosition::DOWN_LEFT] = new QuadTreeNode(child_bounding_box, this);

	//DOWN_RIGHT
	box_child_center = { center_bounding_box.x + subdivision_size.x/2, center_bounding_box.y  , center_bounding_box.z - subdivision_size.z/2 };
	child_bounding_box.SetFromCenterAndSize(box_child_center, subdivision_size);
	childrens[SubdivideChildsPosition::DOWN_RIGHT] = new QuadTreeNode(child_bounding_box, this);
}

bool QuadTreeNode::isLeaf() const
{
	return childrens[0] == nullptr;
}



void QuadTreeNode::DistributeChildrens()
{
	std::vector<GameObject*>::const_iterator it = objects.begin();

	while (it != objects.end())
	{
		GameObject* go = *it;

		uint intersections = 0;
		bool is_intersecting[4];

		for (uint i = 0; i < 4; ++i)
		{
			if (is_intersecting[i] = childrens[i]->bounding_box.Intersects((*it)->GetAABB()))
			{
				intersections++;
			}
		}

		if (intersections != 4)
		{
			it = objects.erase(it);

			for (uint i = 0; i < 4; ++i)
			{
				if (is_intersecting[i])
				{
					childrens[i]->Insert(go);
					
				}
			}
		}
		else
			it++;
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
	if (childrens[0] != nullptr) 
	{
		for (uint i = 0; i < 4; ++i)
		{
			childrens[i]->Clear();
			childrens[i] = nullptr;
		}
	}
	objects.clear();
}

void QuadTreeNode::CollectObjects(std::vector<GameObject*> &object) const
{
	for (std::vector<GameObject*>::const_iterator it = this->objects.begin(); it != this->objects.end(); ++it)
		object.push_back(*it);
	
	for (int i = 0; i < 4; ++i)
		if (childrens[i] != nullptr)
			childrens[i]->CollectObjects(object);
}

QuadTree::QuadTree()
{
}

QuadTree::~QuadTree()
{
	if (root_node != nullptr)
	{
		root_node->Clear();
		root_node->objects.clear();
		RELEASE(root_node);
		size = 0;
	}
	
}

void QuadTree::Insert(GameObject* go)
{
	if (root_node != nullptr)
	{
		if (go->GetAABB().Intersects(root_node->bounding_box) && go->static_object)
		{
			root_node->Insert(go);
			size++;
		}
	}
}

void QuadTree::SetBoundary(const math::AABB & boundary)
{
	if(root_node != nullptr)
		Clear();

	root_node = new QuadTreeNode(boundary, nullptr);
}

void QuadTree::Clear()
{
	if (root_node != nullptr)
	{
		root_node->Clear();
		root_node->objects.clear();
		RELEASE(root_node);
		size = 0;
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

void QuadTree::CollectObjects(std::vector<GameObject*> &objects) const
{
	if (root_node != nullptr)
		root_node->CollectObjects(objects);
}

