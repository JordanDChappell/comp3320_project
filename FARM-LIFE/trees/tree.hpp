/* Tree.hpp
 * This Tree class places trees in specified locations
 * It reads a placemap bmp file and places the trees on the right position
 * The models are randomlu chosen in a pool of 10 different tree models
 */

#include <vector>
#include <windows.h>
#include <sdl.h>
#include <SOIL.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "models/model.hpp"

namespace tree
{

class Tree {
public:
    //Tree constructor
	Tree(const std::string map, terrain::Terrain terra)
	{
		placemap = map;
		readPlaceMap("trees/placemap.bmp", terra);
	}
	model::Model placeTree(int i) {
		return treeVect[i];
	}

private:
    std::string placemap;
    std::vector<model::Model> treeVect;


    void readPlaceMap(std::string placemap, terrain::Terrain terrain){
		std::string treemodel;
        // Load in the height map
        SDL_Surface *img = SDL_LoadBMP(placemap.c_str());

        // Loop over all vertices in image and add the white percentage to heights
        for (int i = 0; i < img->h; i++)
        {
            for (int j = 0; j < img->w; j++)
            {
                // Get the pixel at the right coordinates
                Uint32 pixel = ((Uint32 *)img->pixels)[i * img->pitch / 4 + j];

                // Get red, green and blue values for this pixel
				Uint8 r, g, b, r1, g1, b1, r2, g2, b2;
                SDL_GetRGB(pixel, img->format, &r, &g, &b);
				SDL_GetRGB(pixel, img->format, &r1, &g1, &b1); 
				SDL_GetRGB(pixel, img->format, &r2, &g2, &b2);

				//if the pixel is only red, put the model with the right coordinates in a vector
                if ((r == 255 && g == 0) && (r == 255 && b == 0))
                {
					//Up to 10 different trees with this method
					int random = rand();
					if (random % 10 == 0)
						treemodel = "models/tree/tree0/tree0.obj";
					else if (random % 10 == 1)
						treemodel = "models/tree/tree1.obj";
					else if (random % 10 == 2)
						treemodel = "models/tree/tree2/tree2.obj";
					else if (random % 10 == 3)
						treemodel = "models/tree/tree0/tree0.obj";
					else if (random % 10 == 4)
						treemodel = "models/tree/tree1.obj";
					else if (random % 10 == 5)
						treemodel = "models/tree/tree2/tree2.obj";
					else if (random % 10 == 6)
						treemodel = "models/tree/tree0/tree0.obj";
					else if (random % 10 == 7)
						treemodel = "models/tree/tree1.obj";
					else if (random % 10 == 8)
						treemodel = "models/tree/tree2/tree2.obj";
					else if (random % 10 == 9)
						treemodel = "models/tree/tree1.obj";

					model::Model modeltree = model::Model(treemodel);
                    modeltree.MoveTo(glm::vec3(i - 500, terrain.getHeightAt(i,j) - 20, j - 500));
                    treeVect.push_back(modeltree);
                }
            }
        }
    }
};

} // namespace tree