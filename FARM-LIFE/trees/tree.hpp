/* Tree.hpp
 * This Terrain class creates a terrain with the specified resolution, scale and maximum height.
 * The Terrain is a mesh of triangles. It uses a BMP image to generate the height of the Terrain.
 * The Terrain is drawn so that the camera is on top of the center of the Terrain.
 * This Terrain is textured with lower vertices the rock texture, and the highest vertices the grass texture.
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
	Tree(const std::string map, const std::string treemodel)
	{
		placemap = map;
		readHeightMap("trees/heightmap.bmp", treemodel);
	}
	model::Model placeTree(int i) {
		return treeVect[i];
	}
    /*int gettreevectlength() {
			return (int)treeVect.size;
	}*/
private:
    std::string placemap;
    std::vector<model::Model> treeVect;

    /*void addtreeVect() {
			treeVect.push_back(modeltree);
	}*/

    void readHeightMap(std::string placemap, std::string treemodel){
        // Load in the height map
        SDL_Surface *img = SDL_LoadBMP(placemap.c_str());

        // Loop over all vertices in image and add the white percentage to heights
        for (int i = 0; i < img->h; i++)
        {
            for (int j = 0; j < img->w; j++)
            {
                // Get the pixel at the right coordinate
                Uint32 pixel = ((Uint32 *)img->pixels)[i * img->pitch / 4 + j];

                // Get red, green and blue values for this pixel
				Uint8 r, g, b, r1, g1, b1, r2, g2, b2;
                SDL_GetRGB(pixel, img->format, &r, &g, &b);
				SDL_GetRGB(pixel, img->format, &r1, &g1, &b1); 
				SDL_GetRGB(pixel, img->format, &r2, &g2, &b2);

                // Get a number between 0 and 1 relating to how white/black the pixel is
                //heights->push_back((float)r / 255.0);

                if ((r == 255 && g == 0) && (r == 255 && b == 0))
                {
                    //modeltree.MoveTo(glm::vec3(i, (float)r / 255.0, j));
					model::Model modeltree = model::Model(treemodel);
                    modeltree.MoveTo(glm::vec3(i - 500, ((float)r1 + (float)r2)/255.0 + 1, j - 500));
                    treeVect.push_back(modeltree);
                    //std::cout << "Found a red pixel\n";
					std::cout << (float)r;
					std::cout << ",";
                    std::cout << (float)r1;
                    std::cout << ",";
                    std::cout << (float)r2;
                    std::cout << "\n";
                }
            }
        }
    }
};

} // namespace tree