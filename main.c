// IMAGE TO ASCII ART IN C 
// PROCESS:
// 1) LOAD THE ORIGINAL IMAGE 
// 2) RESIZE THE IMAGE
// 3) CONVERT IT TO GRAYSCALE 
// 4) GET AVERAGE BRIGHTNESS OF EACH PIXEL AND MAP ASCII CHARACTERS TO THE BRIGHTNESS
// 5) PRINT THE CHARACTER THAT GOES WITH THE AVERAGE BRIGHTNESS 
// 6) ENJOY

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "include/stb_image.h"
#include "include/stb_image_write.h"
#include "include/stb_image_resize.h"

#define VERSION 0.1

struct image
{
	unsigned char* data;
	char* path;
	int width, height, channels;
	size_t size;
};

int image_load(struct image* image)
{
	image->data = stbi_load(image->path, &image->width, &image->height, &image->channels, 0);
	image->size = image->width * image->height * image->channels;

	return (image->data != NULL);
}

int image_resize(struct image input, struct image* output)
{
	output->data = (unsigned char*)malloc(output->size);
	output->channels = input.channels;
	return (stbir_resize_uint8(input.data, input.width, input.height, 0, output->data, output->width, output->height, 0, input.channels) != 0);
	// Just commenting this out but this writes the result in an image
	/* stbi_write_jpg(output->path, output->width, output->height, output->channels, output->data, 100); */
}

int image_to_gray_scale(struct image input, struct image* output)
{
	// This is for png files too
	output->channels = input.channels == 4 ? 2 : 1;
	output->size = input.width * input.height * output->channels;
	output->data = (unsigned char*)malloc(output->size);

	for (unsigned char *p = input.data, *pg = output->data; p != input.data + input.size; p += input.channels, pg += output->channels)
	{
		// We use different weigths to calculate the gray scale of each color 
		// We can also just get the average of each pixel but eyes dont see all red, green and blue colors the same 
		// So we use weights instead 
		*pg = (uint8_t)((*p * 0.21) + (*(p + 1) * 0.72) + (*(p + 2) * 0.07)); 

		if (input.channels == 4)
		{
			*(pg + 1) = *(p + 3);
		}
	}

	output->width = input.width;
	output->height = input.height;

	return 1;

	// Writes the result in an image, this is not needed and it makes the program slower
	/* stbi_write_jpg(output->path, input.width, input.height, output->channels, output->data, 100); */
}

void image_to_ascii(struct image gray_image)
{
	// Ascii characters arranged from darkest to lightest
	char* gray_ramp = "^\",:;Il!i~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0oZmwqpdbkhao*#MW&8%B@S ";
	int ramp_length = strlen(gray_ramp);

	for (int y = 0; y < gray_image.height; y++)
	{
		for (int x = 0; x < gray_image.width; x++)
		{
			// We get the rgb values for the current pixel
			int current_pixel = y * gray_image.width + x;
			int r = (int)(gray_image.data[current_pixel]);
			int g = (int)(gray_image.data[current_pixel + 1]);
			int b = (int)(gray_image.data[current_pixel + 2]);

			// Get the average brightness of the pixel, this is why we convert it to grayscale
			float average = (r + g + b) / 3.0f;

			// Map the character with the brightness, this is simple division and multiplication
			char mapped_char = gray_ramp[(int)(ramp_length * (average / 255))];
			putchar(mapped_char);
		}

		printf("\n");
	}
}

int main(int argc, char** argv)
{
	// we just parse arguments here
	
	char* help_message = "\n  cascii help\n"
						 "  --version\n"
						 "  image_path, width, height\n\n";

	if (argc < 2) { printf("%s", help_message); return 0; }

	if (!strcmp(argv[1], "--version") || !strcmp(argv[1], "-v")){ printf("cascii version: %g\n", VERSION); return 0; }

	struct image original_image;
	original_image.path = argv[1];

	if (!image_load(&original_image)) { printf("%s", help_message); return 0; }

	if (argc < 4) { printf("%s", help_message); return 0; }

	/* int aspect_ratio = original_image.width / original_image.height; */

	// Create the resized image with the width and height selected by the user
	struct image resized_image = { .width = (int)atoi(argv[2]), .height = (int)atoi(argv[3])};
	// Size of each pixel * the number of pixels 
	resized_image.size = resized_image.width * resized_image.height * original_image.channels;
	if (!image_resize(original_image, &resized_image)) { printf("%s", help_message); return 0; }

	struct image gray_image;
	if (!image_to_gray_scale(resized_image, &gray_image)) return 0;

	image_to_ascii(gray_image);

	// We free the data 
	free(gray_image.data);
	stbi_image_free(original_image.data);
	free(resized_image.data);

	return 0;
}
