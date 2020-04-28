#pragma once

#include "WarCommander.h"
#include <SharedObject/DoF.h>
#include <zooCmdLoader/ZooCmdLoader.h>

class EntDoF : public DoF
{
	PROPERTY_RW_REF(string, _customParent, CustomParent)
public:
	EntDoF(int id, int group) : DoF(id, group) {}
	bool addChild(DoF* child)
	{
		if (DoF::addChild(child))
		{
			zooCmdL_Send(WarCommander::getSingleton().getRelatedCmd(), "add_child(%d, %d, %d, %d, %s)", child->getID(), child->getGroup(), _id, _group, _customParent.c_str());
			return true;
		}

		return false;
	}

	bool removeChild(DoF* child)
	{
		if (DoF::removeChild(child))
		{
			zooCmdL_Send(WarCommander::getSingleton().getRelatedCmd(), "remove_child(%d, %d, %d, %d, %s)", child->getID(), child->getGroup(), _id, _group, _customParent.c_str());
			return true;
		}

		return false;
	}
};
