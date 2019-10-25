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
		Paddock(int length, int width, glm::vec2 origin = glm::vec2(0))
		{
			this->length = length;
			this->width = width;
			this->origin = origin;
			this->gateOpen = false;

			ProduceFenceNodes();
		}

		///<summary>
		/// Include fence nodes in models to be drawn
		///</summary>
		void PushModels(std::vector<model::Model*> &models)
		{
			for (model::Model* fenceNode : fenceNodes)
				models.push_back(fenceNode);
		}

		///<summary>
		/// Make program aware of fence node hitboxes
		///</summary>
		void PushHitBoxes(std::vector<model::HitBox> &hitBoxes)
		{
			for (model::Model* fenceNode : fenceNodes)
			{
				if (fenceNode == fenceNodes.front()) continue;
				hitBoxes.push_back(fenceNode->hitBox);
			}
		}

		///<summary>
		/// Move paddock by addition of inputted offset.
		/// Snap fence nodes to terrain.
		///</summary>
		void MovePaddock(glm::vec2 location
					   , terrain::Terrain &terra
		               , int cameraOffsetX
					   , int cameraOffsetY
		               , float terraYOffset)
		{
			this->origin = this->origin + location;
			float modelHeightInWorld;
			for (model::Model* fence : fenceNodes)
			{
				modelHeightInWorld = terra.getHeightAt(location.x + cameraOffsetX, location.y + cameraOffsetY) + terraYOffset + fence->hitBox.size.y;
				fence->MoveTo(glm::vec3(location.x, modelHeightInWorld, location.y));
			}
		}

		///<summary>
		/// Return the root fence node to function as a gate
		///</summary>
		model::Model* GetGate()
		{
			return fenceNodes.front();
		}

		///<summary>
		/// Get the open status of the paddock gate
		///</summary>
		bool GateOpenStatus()
		{
			return gateOpen;
		}

		///<summary>
		/// Open gate if it is closed, and vice-versa
		///</summary>
		void ToggleGate(std::vector<model::Model*> &models)
		{
			int id = fenceNodes.at(0)->GetUid();
			std::vector<model::Model*>::iterator it = std::find(models.begin(), models.end(), fenceNodes.front());

			if (it != models.end())
			{
				int index = std::distance(models.begin(), it);

				try
				{
					model::Model* gate;
					// Gate found
					if (gateOpen)
						gate = new model::Model("models/fence/fence.obj");
					else
						gate = new model::Model("models/fence/fence2.obj");

					ToggleGateLocation(gate);

					gateOpen = !gateOpen;
					models.at(index) = gate;
				}
				catch (const std::out_of_range & ex)
				{
					std::cout << "out_of_range Exception Caught :: " << ex.what() << std::endl;
				}
			}
			else
			{
				std::cout << "ID not found" << std::endl;
			}
		}

	private:
		// Length and Width are multiples of fences; length=2 => two fence-nodes long
		// "Length" refers to X direction, "Width" refers to Z direction
		int length;
		int width;

		// (X,Y) coordinates of paddock origin point
		glm::vec2 origin;

		bool gateOpen;
		std::vector<model::Model*> fenceNodes;

		///<summary>
		/// Create fence nodes and position them, starting at the origin
		///</summary>
		void ProduceFenceNodes()
		{
			// Offsets for placing fences sequentially along boundaries
			float xOffset, zOffset;

			// Length side of paddock
			for (float i = 0; i < length; ++i)
			{
				// Length side connected to the origin
				model::Model* fenceX1 = new model::Model("models/fence/fence.obj");
				xOffset = i * fenceX1->hitBox.size.x * 2.0f;
				fenceX1->MoveTo(glm::vec3(origin.x, 0, origin.y) + glm::vec3((fenceX1->hitBox.size.x * 1.3f) + xOffset, 0, 0));

				// Length side opposite to the origin
				model::Model* fenceX2 = new model::Model("models/fence/fence.obj");
				fenceX2->MoveTo(glm::vec3(origin.x, 0, origin.y) + glm::vec3((fenceX2->hitBox.size.x * 1.3f) + xOffset
											, 0
											, (fenceX2->hitBox.size.x * 2) * width));

				fenceNodes.push_back(fenceX1);
				fenceNodes.push_back(fenceX2);
			}

			// Width side of paddock
			for (float j = 0; j < width; ++j)
			{
				// Width side connected to the origin
				model::Model* fenceZ1 = new model::Model("models/fence/fence2.obj");
				zOffset = j * fenceZ1->hitBox.size.z * 2.0f;
				fenceZ1->MoveTo(glm::vec3(origin.x, 0, origin.y) + glm::vec3(0, 0, (fenceZ1->hitBox.size.z / 1.5f) + zOffset));
				
				// Width side opposite the origin
				model::Model* fenceZ2 = new model::Model("models/fence/fence2.obj");
				fenceZ2->MoveTo(glm::vec3(origin.x, 0, origin.y) + glm::vec3((2.0f * fenceZ2->hitBox.size.z) * length
											, 0
											, (fenceZ2->hitBox.size.z / 1.5f) + zOffset));

				fenceNodes.push_back(fenceZ1);
				fenceNodes.push_back(fenceZ2);
			}
		}

		void ToggleGateLocation(model::Model* gate)
		{
			int referenceOffset;	// Position of fence we place gate next to
			glm::vec3 gateOffset;   // Target position of gate relative to reference
			if (gateOpen)
			{
				// Return gate to closed position
				referenceOffset = 1;			// Use fence opposite to gate
				gateOffset = glm::vec3(0, 0, width * (2 * gate->hitBox.size.x));
			}
			else
			{
				// Move gate to opened position
				referenceOffset = 2 * length;	// Use fence adjacent to gate
				gateOffset = glm::vec3(0, 0, 2 * gate->hitBox.size.z);
			}

			try
			{
				model::Model* referenceFence = fenceNodes.at(referenceOffset);
				gate->MoveTo(referenceFence->position - gateOffset);
			}
			catch (const std::out_of_range & ex)
			{
				std::cout << "out_of_range Exception Caught :: " << ex.what() << std::endl;
			}
			
			fenceNodes.front() = gate;
		}
	};
}

#endif