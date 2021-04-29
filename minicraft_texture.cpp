/*
 * Created by MajesticWaffle on 4/27/21.
 * Copyright (c) 2021 Thicc Industries. All rights reserved.
 */

#include "minicraft.h"

Texture* texture_generate(Image* img, uchar texture_load_options){
    Texture *textureptr = new Texture;

    //Create an OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);
    textureptr->id = textureID;

    textureptr -> width     = img -> width;
    textureptr -> height    = img -> height;

    //Calculate & Set atlas uvs
    if(texture_load_options & TEXTURE_MULTIPLE){
        textureptr -> atlas_uv_dx = 1.0 / (textureptr -> width / TEXTURE_TILE_RES);
        textureptr -> atlas_uv_dy = 1.0 / (textureptr -> height / TEXTURE_TILE_RES);
    }

    if(texture_load_options & TEXTURE_SINGLE){
        textureptr -> atlas_uv_dx = textureptr -> atlas_uv_dy = 1.0;
    }


    //Upload image data to openGL
    glBindTexture(GL_TEXTURE_2D, textureptr -> id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureptr->width, textureptr->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->imageData);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, 0);
    
    return textureptr;
}

Image* texture_load_bmp(const char* path){
    uchar header[54];       //BMP header
    uchar dataPos;          //RGB Data offset
    uchar width, height;    //Width and height of image
    uint colorType;         //BMP color format

    //Open file
    FILE* file = fopen(path, "rb");
    if(!file){
        std::cout << "test0" << std::endl;  //TODO: Handle this
        return nullptr;
    }

    //Read 54 bytes -> header
    if(fread(header, 1, 54, file) != 54) {
        std::cout << "test1" << std::endl;  //TODO: Handle this
        return nullptr;
    };

    //Parse header
    dataPos     = *(int*) &header[0x0A];
    width       = *(int*) &header[0x12];
    height      = *(int*) &header[0x16];
    colorType   = *(int*) &header[0x1C];

    //BMP must be 255 colors palletized
    if(colorType != 24){
        std::cout << colorType << " test2" << std::endl;  //TODO: Handle this
        return nullptr;
    }

    //Read pixels

    uchar* bgr_array    = new uchar[width * height * 3];
    uchar* rgba_array   = new uchar[width * height * 4];

    fseek(file, dataPos, SEEK_SET);                      //Move to indexed pixels location
    fread(bgr_array, 3, width * height, file);    //Read pixel information

    for(int y = (height - 1); y >= 0; y--){
        for(int x = 0; x < width; x++) {
            uint bgri   = ((x + (y * width)) * 3);
            uint rgbai  = ((x + (((height - 1) - y) * width)) * 4); //This line hurts my brain

            rgba_array[rgbai + 0] = bgr_array[bgri + 2];
            rgba_array[rgbai + 1] = bgr_array[bgri + 1];
            rgba_array[rgbai + 2] = bgr_array[bgri + 0];

            //Transparency bit
            if (bgr_array[bgri + 0] == 0x00 &&
                bgr_array[bgri + 1] == 0xFF &&
                bgr_array[bgri + 2] == 0x00) {
                rgba_array[rgbai + 3] = 0x00;
            } else {
                rgba_array[rgbai + 3] = 0xFF;
            }
        }
    }

    //delete[] bgr_array;

    Image* imageptr = new Image;

    imageptr -> width       = width;
    imageptr -> height      = height;
    imageptr -> imageData   = rgba_array;

    return imageptr;
}

void texture_bind(Texture* t, GLuint sampler){
    glActiveTexture(GL_TEXTURE0 + sampler);
    glBindTexture(GL_TEXTURE_2D, t -> id);
}
