#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <stdint.h>



#pragma pack(push, 1)
typedef struct BMPHeaders {

	uint8_t name[2];
	uint32_t IMGsize;
	uint16_t rubish1; // useless
	uint16_t rubish2; //useless
	uint32_t IMGoffset; //IMG start



}BMPHeader;
#pragma pack(pop)

typedef struct DIBHeaders {

	uint32_t DIBheaderSize;
	uint32_t width;
	uint32_t hight;
	uint16_t colour;
	uint16_t bitsPerPixel;
	uint32_t compress;
	uint32_t imageSize;
	uint32_t pixelPerMetreHorizontal;
	uint32_t pixelPerMetreVertical;
	uint32_t numberOfColours;
	uint32_t UsedColours;

}DIBHeader;

typedef struct pixels {

	uint8_t blue;
	uint8_t green;
	uint8_t red;

}pixel;

typedef struct IMGs {

	uint32_t hight;
	uint32_t width;
	pixel** colour;

}IMG;

typedef struct bmps {

	BMPHeader* bmpHeader;
	DIBHeader* dibHeader;
	IMG* picture;

}bmp;
/*
bmp* openBMPFile(char* arg) {
	FILE* f = fopen(arg, "rb");

	BMPHeader headOfBMP;
	DIBHeader headOfDIB;
	fread(&headOfBMP, sizeof(BMPHeader), 1, f);
	fread(&headOfDIB, sizeof(DIBHeader), 1, f);
	IMG* imag = readImagine(f, headOfDIB.hight, headOfDIB.width);
	fclose(f);

	bmp* newBmp = malloc(sizeof(bmp));
	newBmp->bmpHeader = &headOfBMP;
	newBmp->dibHeader = &headOfDIB;
	newBmp->picture = imag;

	return newBmp;
}*/

void setBlacColour(pixel* self) {

	self->blue = 0;
	self->green = 0;
	self->red = 0;

}

void setWhiteColour(pixel* self) {

	self->blue = 255;
	self->green = 255;
	self->red = 255;

}

void setColourMonochrome(pixel* self) {

	self->blue = self->blue * 0.3 + self->green * 0.6 + self->red * 0.1;
	self->green = self->blue * 0.3 + self->green * 0.6 + self->red * 0.1;
	self->red = self->blue * 0.3 + self->green * 0.6 + self->red * 0.1;

}

void setPictureBlackAndWhite(IMG* picture) {

	for (int i = 0; i < picture->hight; i++)
	{
		for (int j = 0; j < picture->width; j++) {
			if (picture->colour[i][j].red <= 130 || picture->colour[i][j].blue <=130 || picture->colour[i][j].green <= 130) {
				picture->colour[i][j].red = 0;
				picture->colour[i][j].green = 0;
				picture->colour[i][j].blue = 0;
			}
			else {
				picture->colour[i][j].red = 255;
				picture->colour[i][j].green = 255;
				picture->colour[i][j].blue = 255;
			}
		}
	}

}

void makePictureMonochrome(IMG* picture) {

	for (int i = 0; i < picture->hight; i++)
	{
		for (int j = 0; j < picture->width; j++) {
			setColourMonochrome(&picture->colour[i][j]);
		}
	}

}

IMG* createEptyImagine(int hight, int width) { 

	IMG* picture = malloc(sizeof(IMG));
	picture->hight = hight;
	picture->width = width;
	picture->colour = (pixel***)malloc(hight * sizeof(void*));

	int rowSize = ((sizeof(pixel)* 8 * picture->width + 31) / 32);
	rowSize *= 4;
	int pixelAmount = rowSize / sizeof(pixel) + 1;

	for (int i = picture->hight - 1; i >= 0; i--) {
		picture->colour[i] = malloc(pixelAmount * sizeof(pixel));
	}

	return picture;
}

IMG* readImagine(FILE* file, int hight, int width) {

	IMG* picture = createEptyImagine(hight,width);
	

	uint32_t rowSize = ((sizeof(pixel)* 8 * picture->width + 31) / 32);
	rowSize *= 4;
	uint32_t pixelAmount = rowSize / sizeof(pixel) + 1;
	//sizeof(pixel), picture->width
	for (int i = picture->hight - 1; i >= 0; i--) {
		fread(picture->colour[i], 1, rowSize, file);
	}

	return picture;
}

void freeIMG(IMG* picture) {

	for (int i = 0; i < picture->hight; i++)
	{
		for (int j = 0; j < picture->width; j++) {
			setWhiteColour(&picture->colour[i][j]);
		}
	}
}

char makeImage(BMPHeader* bmpHeader, DIBHeader* dibHeader, IMG* picture,char* name,char* dest) {
	char slash[2] = "/";
	char temp[51];
	strcpy(temp, dest);
	strcat(temp,slash);
	strcat(temp, name);

	FILE* newFile = fopen(temp, "w");
	if (newFile == NULL) {
		perror("some issue with file directory");
		exit(1);
	}

	if (newFile == NULL) return -1;

	//uint32_t rowSize = ((sizeof(pixel) * picture->width + 31) / 32);

	fwrite(bmpHeader, 1 * sizeof(BMPHeader), 1, newFile);
	fwrite(dibHeader, 1 * sizeof(DIBHeader), 1, newFile);
	//makePictureMonochrome(picture);
	for (int i = picture->hight - 1; i >= 0; i--) {
		fwrite(picture->colour[i], sizeof(pixel), picture->width, newFile);
	}
	fclose(newFile);
	return 1;
}

void memcpyImage(IMG* src,IMG* dict) {

	for (int i = 0; i < src->hight; i++) {
		for (int j = 0; j < src->width; j++) {
			dict->colour[i][j].red = src->colour[i][j].red;
			dict->colour[i][j].blue = src->colour[i][j].blue;
			dict->colour[i][j].green = src->colour[i][j].green;
		}
	}

}

void LifeGame(bmp* file,IMG* picture, IMG* newPicture,int count,int freq,char* dest) {
	/*
	for (int i = 1; i < picture->hight - 1; i++) {
		for (int j = 1; j < picture->width - 1; j++) {

			if (picture->colour[i][j - 1].red == 0) accum++;
			if (picture->colour[i][j + 1].red == 0) accum++;
			if (picture->colour[i - 1][j].red == 0) accum++;
			if (picture->colour[i + 1][j].red == 0) accum++;      // collecting info about pixel
			if (picture->colour[i + 1][j + 1].red == 0) accum++;
			if (picture->colour[i - 1][j + 1].red == 0) accum++;
			if (picture->colour[i + 1][j - 1].red == 0) accum++;
			if (picture->colour[i - 1][j - 1].red == 0) accum++;

			if (newPicture->colour[i][j].red == 0) {
				if (accum < 2 || accum > 3) {
					newPicture->colour[i][j].red = 255;
					newPicture->colour[i][j].blue = 255;
					newPicture->colour[i][j].green = 255;
				}
			}
			else {
				if (accum == 3) {
					newPicture->colour[i][j].red = 0;
					newPicture->colour[i][j].blue = 0;
					newPicture->colour[i][j].green = 0;
				}
			}

		}

	}
	accum = 0;

	for (int i = 1; i < picture->hight - 1; i++) { //left column
		for (int j = 0; j < 1; j++) {



		}
	}
	accum = 0;

	for (int i = 1; i < picture->hight - 1; i++) { // right column
		for (int j = picture->width - 1; j < picture->width; j++) {



		}
	}
	accum = 0;

	for (int i = 0; i < 1; i++) { // bottom
		for (int j = 1; j < picture->width - 1; j++) {



		}
	}
	accum = 0;

	for (int i = 0; i < 1; i++) { // up
		for (int j = 1; j < picture->width - 1; j++) {



		}
	}
	accum = 0;

	//need to research angles (4)

	*/
	char amountOfBlackDots = 0;
	int i, j;
	//setPictureBlackAndWhite(picture);
	for ( i = 0; i < picture->hight; i++){
		for ( j = 0; j < picture->width; j++) {
			
			if (picture->colour[(i + picture->hight) % picture->hight][(j - 1 + picture->width) % picture->width].red == 0) amountOfBlackDots++;
			if (picture->colour[(i + picture->hight) % picture->hight][(j + 1 + picture->width) % picture->width].red == 0) amountOfBlackDots++;
			if (picture->colour[(i - 1 + picture->hight) % picture->hight][(j + picture->width) % picture->width].red == 0) amountOfBlackDots++;
			if (picture->colour[(i + 1 + picture->hight) % picture->hight][(j + picture->width) % picture->width].red == 0) amountOfBlackDots++;      // collecting info about pixel
			if (picture->colour[(i + 1 + picture->hight) % picture->hight][(j + 1 + picture->width) % picture->width].red == 0) amountOfBlackDots++;
			if (picture->colour[(i - 1 + picture->hight) % picture->hight][(j + 1 + picture->width) % picture->width].red == 0) amountOfBlackDots++;
			if (picture->colour[(i + 1 + picture->hight) % picture->hight][(j - 1 + picture->width) % picture->width].red == 0) amountOfBlackDots++;
			if (picture->colour[(i - 1 + picture->hight) % picture->hight][(j - 1 + picture->width) % picture->width].red == 0) amountOfBlackDots++;


			if (picture->colour[i][j].red == 0) {
				if (amountOfBlackDots < 2 || amountOfBlackDots > 3) {
					//newPicture->colour[i][j].red = 255;
					//newPicture->colour[i][j].blue = 255;
					//newPicture->colour[i][j].green = 255;
					setWhiteColour(&newPicture->colour[i][j]);
				}
				else {
					setBlacColour(&newPicture->colour[i][j]);
				}
			}
			else {
				if (amountOfBlackDots == 3) {
					//newPicture->colour[i][j].red = 0;
					//newPicture->colour[i][j].blue = 0;
					//newPicture->colour[i][j].green = 0;
					setBlacColour(&newPicture->colour[i][j]);
				}
				
			}
			amountOfBlackDots = 0;

		}

	}
	char buffer[30];
	char bmps[30] = "-th_generation.bmp";
	_itoa(count, buffer, 10);
	char res[23];
	snprintf(res, sizeof(res), "%s%s", buffer, bmps);
	
	//makeImage(file->bmpHeader, file->dibHeader, newPicture,"made.bmp");
	//makeImage(file->bmpHeader, file->dibHeader, picture, "origin.bmp");
	memcpyImage(newPicture,picture);

	if (count % freq == 0) {
		makeImage(file->bmpHeader, file->dibHeader, picture, res,dest);
	}
}

char checkForCorrectImage(BMPHeader* bmpHeader, DIBHeader* dibHeader, IMG* image) {
	if (sizeof(BMPHeader) != 14) {
		printf("Invalid BMP file type\n");
		return -2;
	}
	if (bmpHeader->name[0] == 'B' && bmpHeader->name[1] == 'M') {

		if (bmpHeader->IMGoffset == 54) {

			if (dibHeader->DIBheaderSize==40) {
				
				if (dibHeader->bitsPerPixel == 24) {

					printf("Everything is correct\n");
					return 0;

				}
				else {
					printf("Invalid amount bits for one pixel\n");
					return -1;
				}

			}
			else {
				printf("Invalid DIB header size\n");
				return -1;
			}

		}
		else {
			printf("Invalid IMG offset\n");
			return -1;
		}

	}
	else {
		printf("Invalid BMP header name\n");
		return -1;
	}

}

char checkForScanfedCorrectArguments(const int argc,const char** argv) {

	char buf[25];
	scanf("%s", buf);

	if (buf[0] == '-' && buf[1] == '-') {


		if (strcmp(buf + 2, "input") != 0) {
			printf("Invalid input comand\n");
			return -1;
		}
		else {

			char inputFile[53];
			scanf("%s", inputFile);
			int i = 0;

			while (inputFile[i] != '.') {
				i++;
			}
			i++;

			if ((strlen(inputFile) - i) != 3) {
				printf("Invalid input file type length (for bmp format)\n");
				return -1;
			}
			else {
				if (inputFile[i] == 'b' && inputFile[i + 1] == 'm' && inputFile[i + 2] == 'p') {

				}
				else {
					printf("Invalid input file type\n");
					return -1;
				}
			}
		}
	}
	else {
		printf("Invalid input comand argument\n");
		return -1;
	}

	scanf("%s", buf);
	if (buf[0] == '-' && buf[1] == '-') {

		if (strcmp(buf + 2, "output") != 0) {
			printf("Invalid output comand\n");
			return -1;
		}
		else {

			char inputFile[53];
			scanf("%s", inputFile);

		}
	}
	else {
		printf("Invalid output comand argument\n");
		return -1;
	}

	scanf("%s", buf);
	if (buf[0] == '-' && buf[1] == '-') {

		if (strcmp(buf + 2, "max_iter") != 0) {
			printf("Invalid max_iter comand\n");
			return -1;
		}
		else {

			int max_iter;
			scanf("%d", &max_iter);

			if (max_iter <= 0) {
				printf("Invalid max_iter value");
				return -1;
			}

		}
	}
	else {
		printf("Invalid max_iter comand argument\n");
		return -1;
	}

	scanf("%s", buf);
	if (buf[0] == '-' && buf[1] == '-') {

		if (strcmp(buf + 2, "dump_freq") != 0) {
			printf("Invalid dump_freq comand\n");
			return -1;
		}
		else {

			int dump_freq;
			scanf("%d", &dump_freq);

			if (dump_freq < 0) {
				printf("Invalid dump_freq value");
				return -1;
			}

		}
	}
	else {
		printf("Invalid dump_freq comand argument\n");
		return -1;
	}

}

char checkCorrectComandStringArguments(const int argc, const char** argv,int* max_iter,int* freq) {



	if (argc < 5) {
		fprintf(stderr, "Not enough arguments");
		return -1;
	}
	if (argc > 9) {
		fprintf(stderr, "There is more than needed arguments");
		//errno = "There is more than needed arguments";
		return -2;
	}

	for (int i = 0; i < argc - 1; i++) {

		if (strcmp(argv[i], "--input") == 0) {
			int j = 0;
			while (argv[i + 1][j] != '.') {
				j++;
			}
			j++;
			if (strlen(argv[i + 1]) - j != 3) {
				fprintf(stderr, "Invalid input file type length (for bmp format)");
				return -3;
			}
			else {
				if (argv[i + 1][j] == 'b' && argv[i + 1][j + 1] == 'm' && argv[i + 1][j + 2] == 'p') {

				}
				else {
					fprintf(stderr, "Invalid input file type");
					return -4;

				}
			}
			

		}
		

		if (strcmp(argv[i],"--max_iter") == 0) {
			char* end;
			int number = strtol(argv[i + 1], &end, 10);
			if (number <= 0) {
				fprintf(stderr, "Invalid max_iter number");
				return -5;
			}
			else {
				(*max_iter) = number;
			}
		}
		if (strcmp(argv[i], "--dump_freq") == 0) {
			char* end;
			int number = strtol(argv[i + 1], &end, 10);
			if (number <= 0) {
				fprintf(stderr, "Invalid dump_freq number");
				return -6;
			}
			else {
				(*freq) = number;
			}
		}
	}
	return 0;
}



int main(int argc, char** argv) {
	int max_iter = 0;
	int freq = 1;
	int error = checkCorrectComandStringArguments(argc, argv, &max_iter, &freq);

	if (error) {
		return error;
	}
	
	char* end;
	FILE* f = fopen(argv[2], "rb");
	if (f == NULL) {
		perror("some issue with input file");
		exit(1);
	}

	BMPHeader headOfBMP;
	DIBHeader headOfDIB;

	fread(&headOfBMP, sizeof(BMPHeader), 1, f);
	fread(&headOfDIB, sizeof(DIBHeader), 1, f);
	IMG* imag = readImagine(f, headOfDIB.hight, headOfDIB.width);

	fclose(f);

	bmp* newBmp = malloc(sizeof(bmp));
	newBmp->bmpHeader = &headOfBMP;
	newBmp->dibHeader = &headOfDIB;
	newBmp->picture = imag;
	
	//
	//

	
	IMG* empty = createEptyImagine(imag->hight,imag->width);
	
	char* placeForPicture[21];
	strcpy(placeForPicture, argv[4]);

	
	if (max_iter != 0) {
		for (int i = 0; i < max_iter; i++) {
			LifeGame(newBmp,imag, empty,i,freq,placeForPicture);
			freeIMG(empty);
		}
	}
	else {
		int i = 0;
		while (1) {
			LifeGame(newBmp, imag, empty, i, freq,placeForPicture);
			freeIMG(empty);
			i++;
		}
	}
	
	return 0;
}