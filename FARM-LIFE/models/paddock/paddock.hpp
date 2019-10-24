/**
 * Class to generate and maintain paddocks.
 * Paddocks are length * width fenced enclosures.
 */

#ifndef A1_PADDOCK_HPP
#define A1_PADDOCK_HPP

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
			// Offsets for placing fences sequentially along boundaries
			float xOffset, zOffset;
			for (int i = 0; i < length; ++i)
			{
				model::Model* fenceX1 = new model::Model("models/fence/fence.obj");
				xOffset = i * fenceX1->hitBox.size.x * 2;
				fenceX1->MoveTo(origin + glm::vec3(fenceX1->hitBox.size.x + xOffset, 0, 0));
				fenceNodes.push_back(fenceX1);
			}
			for (int j = 0; j < width; ++j)
			{
				model::Model* fenceZ1 = new model::Model("models/fence/fence2.obj");
				zOffset = j * fenceZ1->hitBox.size.y * 2;
				fenceZ1->MoveTo(origin + glm::vec3(0, 0, fenceZ1->hitBox.size.y + zOffset));
				fenceNodes.push_back(fenceZ1);
			}
		}
	};
}

#endif