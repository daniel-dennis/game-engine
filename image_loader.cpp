//  (c) Daniel Dennis 2018

#include "image_loader.hpp"

namespace dd
{
    image_loader::image_loader(const char* file_name)
    {
        ilutRenderer(ILUT_OPENGL);
        ilInit();
        iluInit();
        ilutInit();
        ilutRenderer(ILUT_OPENGL);

        location = load_image(file_name);
    }
    
    image_loader::~image_loader()
    {
        
    }

    // Function load a image, turn it into a texture, and return the texture ID as a GLuint for use
    GLuint image_loader::load_image(const char* file_name)
    {
        ILuint imageID;				// Create an image ID as a ULuint
        GLuint textureID;			// Create a texture ID as a GLuint
        ILboolean success;			// Create a flag to keep track of success/failure
        ILenum error;				// Create a flag to keep track of the IL error state
        ilGenImages(1, &imageID); 		// Generate the image ID
        ilBindImage(imageID); 			// Bind the image
        success = ilLoadImage(file_name); 	// Load the image file
    
        // If we managed to load the image, then we can start to do things with it...
        if (success)
        {
            // If the image is flipped (i.e. upside-down and mirrored, flip it the right way up!)
            ILinfo ImageInfo;
            iluGetImageInfo(&ImageInfo);
            if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
            {
                iluFlipImage();
            }
    
            // Convert the image into a suitable format to work with
            // NOTE: If your image contains alpha channel you can replace IL_RGB with IL_RGBA
            success = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
    
            // Quit out if we failed the conversion
            if (!success)
            {
                error = ilGetError();
                std::cerr << "ERROR: image conversion failed: " << error << " - " << iluErrorString(error) << std::endl;
                exit(-1);
            }
    
            // Generate a new texture
            glGenTextures(1, &textureID);
            
            glActiveTexture(GL_TEXTURE0);
            // Bind the texture to a name
            glBindTexture(GL_TEXTURE_2D, textureID);
    
            // Set texture clamping method
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
            // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
            // Set texture interpolation method to use linear interpolation (no MIPMAPS)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
            // Specify the texture specification
            glTexImage2D(GL_TEXTURE_2D, 				// Type of texture
                        0,				// Pyramid level (for mip-mapping) - 0 is the top level
                        ilGetInteger(IL_IMAGE_FORMAT),	// Internal pixel format to use. Can be a generic type like GL_RGB or GL_RGBA, or a sized type
                        ilGetInteger(IL_IMAGE_WIDTH),	// Image width
                        ilGetInteger(IL_IMAGE_HEIGHT),	// Image height
                        0,				// Border width in pixels (can either be 1 or 0)
                        ilGetInteger(IL_IMAGE_FORMAT),	// Format of image pixel data
                        GL_UNSIGNED_BYTE,		// Image data type
                        ilGetData());			// The actual image data itself
        }
        else // If we failed to open the image file in the first place...
        {
            error = ilGetError();
            std::cerr << "ERROR: cannot use " << file_name << " (IL error: " << error << " - " << iluErrorString(error) << ")\n";
            return 0;
        }

        ilDeleteImages(1, &imageID); // Because we have already copied image data into texture data we can release memory used by image.
        return textureID; // Return the GLuint to the texture so you can use it!
    }
}