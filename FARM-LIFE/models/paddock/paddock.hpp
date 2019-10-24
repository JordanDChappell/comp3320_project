/**
 * Class to generate and maintain paddocks.
 * Paddocks are length * width fenced enclosures.
 */

#ifndef A1_PADDOCK_HPP
#define A1_PADDOCK_HPP

#include <glm\gtx\string_cast.hpp>

namespace model
{
	class Paddock
	{
	public:
		Paddock(int length, int width, glm::vec3 origin = glm::vec3(0))
		{
			this->length = length;
			this->width = width;
			this->origin = origin;

			ProduceFenceNodes();
		}

		void PushModels(std::vector<model::Model*> &models)
		{
			for (model::Model* fenceNode : fenceNodes)
				models.push_back(fenceNode);
		}

		void PushHitBoxes(std::vector<model::HitBox> &hitBoxes)
		{
			for (model::Model* fenceNode : fenceNodes)
				hitBoxes.push_back(fenceNode->hitBox);
		}

		void MovePaddockTo(glm::vec3 location)
		{
			//TODO: Move all fence nodes
			this->origin = this->origin + location;
			for (model::Model* fence : fenceNodes)
				fence->MoveTo(location);
		}
	private:
		// Length and Width are multiples of fences; length=2 => two fence-nodes long
		// "Length" refers to X direction, "Width" refers to Z direction
		int length;
		int width;
		glm::vec3 origin;
		std::vector<model::Model*> fenceNodes;

		void ProduceFenceNodes()
		{
			std::cout << "Origin:" << glm::to_string(origin) << std::endl;

			// Offsets for placing fences sequentially along boundaries
			float xOffset, zOffset;
			for (int i = 0; i < length; ++i)
			{
				model::Model* fenceX1 = new model::Model("models/fence/fence.obj");
				xOffset = i * fenceX1->hitBox.size.x * 2;
				fenceX1->MoveTo(origin + glm::vec3(fenceX1->hitBox.size.x + xOffset, 0, 0));

				std::cout << "Size: " << "X=" << fenceX1->hitBox.size.x
					<< "Y=" << fenceX1->hitBox.size.y
					<< "Z=" << fenceX1->hitBox.size.z
					<< std::endl;
				std::cout << "FenceX:" << glm::to_string(fenceX1->position) << std::endl;
				//model::Model* fenceX2 = new model::Model("models/fence/fence.obj");
				//fenceX2->MoveTo(origin + glm::vec3(0, 0, fenceX2->hitBox.size.x + (fenceX2->hitBox.size.z * 2 * width) + xOffset));

				fenceNodes.push_back(fenceX1);
				//fenceNodes.push_back(fenceX2);
			}
			for (int j = 0; j < width; ++j)
			{
				model::Model* fenceZ1 = new model::Model("models/fence/fence2.obj");
				zOffset = j * fenceZ1->hitBox.size.y * 2;
				fenceZ1->MoveTo(origin + glm::vec3(0, 0, fenceZ1->hitBox.size.y + zOffset));

				std::cout << "FenceX:" << glm::to_string(fenceZ1->position) << std::endl;
				std::cout << "Size: " << "X=" << fenceZ1->hitBox.size.x
					<< "Y=" << fenceZ1->hitBox.size.y
					<< "Z=" << fenceZ1->hitBox.size.z
					<< std::endl;
				//model::Model* fenceZ2 = new model::Model("models/fence/fence2.obj");
				//fenceZ2->MoveTo(origin + glm::vec3(0, 0, fenceZ2->hitBox.size.x * 2 * length));

				fenceNodes.push_back(fenceZ1);
				//fenceNodes.push_back(fenceZ2);
			}
		}
	};
}

#endif