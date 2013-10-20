///////////////////////////////////////////////////////////////////////////////
//
//      TargaImage.cpp                          Author:     Stephen Chenney
//                                              Modified:   Eric McDaniel
//                                              Date:       Fall 2004
//                                              Modified:   Feng Liu
//                                              Date:       Winter 2011
//                                              Why:        Change the library file 
//      Implementation of TargaImage methods.  You must implement the image
//  modification functions.
//
///////////////////////////////////////////////////////////////////////////////

#include "Globals.h"
#include "TargaImage.h"
#include "libtarga.h"
#include <stdlib.h>
#include <assert.h>
#include <memory.h>
#include <math.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <map>
#include <time.h>

using namespace std;

// constants
const int           RED             = 0;                // red channel
const int           GREEN           = 1;                // green channel
const int           BLUE            = 2;                // blue channel
const unsigned char BACKGROUND[3]   = { 0, 0, 0 };      // background color


// Computes n choose s, efficiently
double Binomial(int n, int s)
{
    double        res;

    res = 1;
    for (int i = 1 ; i <= s ; i++)
        res = (n - i + 1) * res / i ;

    return res;
}// Binomial

///////////////////////////////////////////////////////////////////////////////
//
//      Color Popularity Comparison Function
//
///////////////////////////////////////////////////////////////////////////////

bool ColorPopularityComparison(const RGBcount i, const RGBcount j)
{
	return (i.count > j.count);
}

///////////////////////////////////////////////////////////////////////////////
//
//      Constructor.  Initialize member variables.
//
///////////////////////////////////////////////////////////////////////////////
TargaImage::TargaImage() : width(0), height(0), data(NULL)
{}// TargaImage

///////////////////////////////////////////////////////////////////////////////
//
//      Constructor.  Initialize member variables.
//
///////////////////////////////////////////////////////////////////////////////
TargaImage::TargaImage(int w, int h) : width(w), height(h)
{
   data = new unsigned char[width * height * 4];
   ClearToBlack();
}// TargaImage



///////////////////////////////////////////////////////////////////////////////
//
//      Constructor.  Initialize member variables to values given.
//
///////////////////////////////////////////////////////////////////////////////
TargaImage::TargaImage(int w, int h, unsigned char *d)
{
    int i;

    width = w;
    height = h;
    data = new unsigned char[width * height * 4];

    for (i = 0; i < width * height * 4; i++)
	    data[i] = d[i];
}// TargaImage

///////////////////////////////////////////////////////////////////////////////
//
//      Copy Constructor.  Initialize member to that of input
//
///////////////////////////////////////////////////////////////////////////////
TargaImage::TargaImage(const TargaImage& image) 
{
   width = image.width;
   height = image.height;
   data = NULL; 
   if (image.data != NULL) {
      data = new unsigned char[width * height * 4];
      memcpy(data, image.data, sizeof(unsigned char) * width * height * 4);
   }
}


///////////////////////////////////////////////////////////////////////////////
//
//      Destructor.  Free image memory.
//
///////////////////////////////////////////////////////////////////////////////
TargaImage::~TargaImage()
{
    if (data)
        delete[] data;
}// ~TargaImage


///////////////////////////////////////////////////////////////////////////////
//
//      Converts an image to RGB form, and returns the rgb pixel data - 24 
//  bits per pixel. The returned space should be deleted when no longer 
//  required.
//
///////////////////////////////////////////////////////////////////////////////
unsigned char* TargaImage::To_RGB(void)
{
    unsigned char   *rgb = new unsigned char[width * height * 3];
    int		    i, j;

    if (! data)
	    return NULL;

    // Divide out the alpha
    for (i = 0 ; i < height ; i++)
    {
	    int in_offset = i * width * 4;
	    int out_offset = i * width * 3;

	    for (j = 0 ; j < width ; j++)
        {
	        RGBA_To_RGB(data + (in_offset + j*4), rgb + (out_offset + j*3));
	    }
    }

    return rgb;
}// TargaImage


///////////////////////////////////////////////////////////////////////////////
//
//      Save the image to a targa file. Returns 1 on success, 0 on failure.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Save_Image(const char *filename)
{
    TargaImage	*out_image = Reverse_Rows();

    if (! out_image)
	    return false;

    if (!tga_write_raw(filename, width, height, out_image->data, TGA_TRUECOLOR_32))
    {
	    cout << "TGA Save Error: %s\n", tga_error_string(tga_get_last_error());
	    return false;
    }

    delete out_image;

    return true;
}// Save_Image


///////////////////////////////////////////////////////////////////////////////
//
//      Load a targa image from a file.  Return a new TargaImage object which 
//  must be deleted by caller.  Return NULL on failure.
//
///////////////////////////////////////////////////////////////////////////////
TargaImage* TargaImage::Load_Image(char *filename)
{
    unsigned char   *temp_data;
    TargaImage	    *temp_image;
    TargaImage	    *result;
    int		        width, height;

    if (!filename)
    {
        cout << "No filename given." << endl;
        return NULL;
    }// if

    temp_data = (unsigned char*)tga_load(filename, &width, &height, TGA_TRUECOLOR_32);
    if (!temp_data)
    {
        cout << "TGA Error: %s\n", tga_error_string(tga_get_last_error());
	    width = height = 0;
	    return NULL;
    }
    temp_image = new TargaImage(width, height, temp_data);
    free(temp_data);

    result = temp_image->Reverse_Rows();

    delete temp_image;

    return result;
}// Load_Image


///////////////////////////////////////////////////////////////////////////////
//
//      Convert image to grayscale.  Red, green, and blue channels should all 
//  contain grayscale value.  Alpha channel shoould be left unchanged.  Return
//  success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::To_Grayscale()
{
    if (data)
		cout << "Executing To_Grayscale()" << endl;

    clock_t time;
    time = clock();

    for (int i = 0; i < width * height * 4; i+=4)
    {
        data[i] = (unsigned char) data[i] * 0.299 + data[i+1] * 0.587 + data[i+2] * 0.114;
        data[i+1] = data[i];
        data[i+2] = data[i];
        //data[i+3] = (char) 255;
        //cout << "(" << (int) data[i] << "," << (int) data[i+1] << "," << (int) data[i+2] << ")" << " Alpha: " << (int) data[i+3] << endl;
    }
    
    time = clock() - time;
    cout << "Time Elapsed:  " << ((float) time)/CLOCKS_PER_SEC << " sec" << endl;
    
    return true;
}// To_Grayscale


///////////////////////////////////////////////////////////////////////////////
//
//  Convert the image to an 8 bit image using uniform quantization.  Return 
//  success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Quant_Uniform()
{
	// Print diagnostic message
    if (data)
		cout << "Executing Quant_Uniform()" << endl;

    clock_t time;
    time = clock();
	
	// Initialize KV dictionaries
	map<int,int> red;
	map<int,int> green;
	map<int,int> blue;
	
	red[0] = (32/2) - 1;
	red[1] = (32/2 + 32) - 1;
	red[2] = (32/2 + 2*32) - 1;
	red[3] = (32/2 + 3*32) - 1;
	red[4] = (32/2 + 4*32) - 1;
	red[5] = (32/2 + 5*32) - 1;
	red[6] = (32/2 + 6*32) - 1;
	red[7] = (32/2 + 7*32) - 1;

	green[0] = (32/2) - 1;
	green[1] = (32/2 + 32) - 1;
	green[2] = (32/2 + 2*32) - 1;
	green[3] = (32/2 + 3*32) - 1;
	green[4] = (32/2 + 4*32) - 1;
	green[5] = (32/2 + 5*32) - 1;
	green[6] = (32/2 + 6*32) - 1;
	green[7] = (32/2 + 7*32) - 1;

	blue[0] = (64/2) - 1;
	blue[1] = (64/2 + 64) - 1;
	blue[2] = (64/2 + 2*64) - 1;
	blue[3] = (64/2 + 3*64) - 1;

	for (int i = 0; i < width * height * 4; i+=4)
	{
		//Red
		int redindex = floor((float) data[i]/32);
		data[i] = red[redindex];
		
		//Green
		int greenindex = floor((float) data[i+1]/32);
		data[i+1] = green[greenindex];
		
		//Blue
		int blueindex = floor((float) data[i+2]/64);
		data[i+2] = blue[blueindex];
		//data[i+3] = (char) 255;
		//cout << "(" << (int) data[i] << "," << (int) data[i+1] << "," << (int) data[i+2] << ")" << " Alpha: " << (int) data[i+3] << endl;
	}
    
    time = clock() - time;
    cout << "Time Elapsed:  " << ((float) time)/CLOCKS_PER_SEC << " sec" << endl;
    
    return true;
}// Quant_Uniform


///////////////////////////////////////////////////////////////////////////////
//
//      Convert the image to an 8 bit image using populosity quantization.  
//  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Quant_Populosity()
{
    // Print diagnostic message
    if (data)
		cout << "Executing Quant_Propulosity()" << endl;

    clock_t time;
    time = clock();
    
	int offset = width * height;
	int arraysize = offset * 4;

	// Step down to 32 levels of each primary
	cout << "Stepping down to 32 levels of each primary" << endl;
	map<int,int> red;
	map<int,int> green;
	map<int,int> blue;

	for (int i = 0; i < 32; i++)
	{
		red[i] = (8/2 + i*8) - 1;
	}

	for (int i = 0; i < 32; i++)
	{
		green[i] = (8/2 + i*8) - 1;
	}

	for (int i = 0; i < 32; i++)
	{
		blue[i] = (8/2 + i*8) - 1;
	}

	for (int i = 0; i < arraysize; i+=4)
	{
		//Red
		int redindex = floor((float) data[i]/8);
		data[i] = red[redindex];
		
		//Green
		int greenindex = floor((float) data[i+1]/8);
		data[i+1] = green[greenindex];
		
		//Blue
		int blueindex = floor((float) data[i+2]/8);
		data[i+2] = blue[blueindex];
	}

	// Find 256 most popular colors
	cout << "Finding 256 most popular colors" << endl;

	//Define Vector
	std::vector<RGBcount> popularcolors;

	//Add items to vector
	cout << " -Counting occurrances of each color" << endl;

	for (int i = 0; i < arraysize; i+=4)
	{
		//Read pixel data
		int red = data[i];
		int green = data[i+1];
		int blue = data[i+2];

		// Check if color already registered
		bool registered = false;

		for(std::vector<RGBcount>::size_type i = 0; i != popularcolors.size() && !registered; i++) 
		{
			if (popularcolors[i].R == red && popularcolors[i].G == green && popularcolors[i].B == blue)
			{
				popularcolors[i].count++;
				registered = true;
			}
		}	

		//If color not yet registered
		if (!registered)
		{
			RGBcount pixel;
			pixel.R = red;
			pixel.G = green;
			pixel.B = blue;
			pixel.count = 1;
			popularcolors.push_back(pixel);
		}
        
	}

	//Sort vector
	cout << " -Sorting vector by popularity" << endl;
	std::sort(popularcolors.begin(), popularcolors.end(), ColorPopularityComparison);

	//Display 256 most popular colors
	cout << endl << "** 256 most popular colors **" << endl << endl;
	cout << "Total Colors Found: " << popularcolors.size() << endl << endl;
	for (int i = 0; i < 256; ++i)
	{
		double percent = (double) popularcolors[i].count/(height*width);
		cout.precision(3);
		cout << "R: " << popularcolors[i].R << ", G: " << popularcolors[i].G << ", B: " << popularcolors[i].B << "  " << percent*100 << "%" << endl;
	}

	//Set all pixels to nearest 256 most popular colors
	cout << "Set all pixels to nearest 256 most popular colors" << endl;

	for (int i = 0; i < arraysize; i+=4)
	{
		//Read pixel data
		int red = data[i];
		int green = data[i+1];
		int blue = data[i+2]; 

		//Iterate through popular colors
		int index = 0;
		float minvalue = 100000000.0;

		for (int i = 0; i < 256; ++i)
		{
			int redsquare = abs(popularcolors[i].R-red)^2;
			int greensquare = abs(popularcolors[i].G-green)^2;
			int bluesquare = abs(popularcolors[i].B-blue)^2;

			float sum_of_square = (float) redsquare+greensquare+bluesquare;

			float distance = sqrt(sum_of_square);

			if (minvalue > distance)
			{
				index = i;
				minvalue = distance;
			}
		}

		data[i]=popularcolors[index].R;
		data[i+1]=popularcolors[index].G;
		data[i+2]=popularcolors[index].B;
	}
	
    time = clock() - time;
    cout << "Time Elapsed:  " << ((float) time)/CLOCKS_PER_SEC << " sec" << endl;
    
    return true; 
}// Quant_Populosity


///////////////////////////////////////////////////////////////////////////////
//
//      Dither the image using a threshold of 1/2.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Dither_Threshold()
{
    // Print diagnostic message
    if (data)
		cout << "Executing Dither_Threshold()" << endl;

    clock_t time;
    time = clock();
    
	int offset = width * height;
	int arraysize = offset * 4;

	// Convert Image to Greyscale then Dither to 0.5 Threshold
	for (int i = 0; i < arraysize; i+=4)
	{
		float intensity = (float) (data[i] * 0.299 + data[i+1] * 0.587 + data[i+2] * 0.114)/256;
		if (intensity > 0.5f)
		{
			data[i] = 255;
			data[i+1] = data[i];
			data[i+2] = data[i];
		}
		else
		{
			data[i] = 0;
			data[i+2] = 0;
			data[i+3] = 0;
		}
	}

    time = clock() - time;
    cout << "Time Elapsed:  " << ((float) time)/CLOCKS_PER_SEC << " sec" << endl;
    
    return true;
}// Dither_Threshold


///////////////////////////////////////////////////////////////////////////////
//
//      Dither image using random dithering.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Dither_Random()
{
    // Print diagnostic message
    if (data)
		cout << "Executing Dither_Random()" << endl;

    clock_t bench;
    bench = clock();
    
	int offset = width * height;
	int arraysize = offset * 4;

	float averageintensity = 0.0f;

	cout << "Calculating Average Intensity" << endl;
	// Compute average intensity over the image
	for (int i = 0; i < arraysize; i+=4)
	{
		averageintensity += (float) (data[i] * 0.299 + data[i+1] * 0.587 + data[i+2] * 0.114)/256;
	}

	averageintensity = averageintensity / offset;
	cout << "Average Intensity: " << averageintensity << endl << endl;

	// Seed Random
	srand ((unsigned int)time(NULL));

	cout << "Applying Threshold based on Average Intensity, after adding random value betwen -0.2 and 0.2" << endl;
	for (int i = 0; i < arraysize; i+=4)
	{
		//Get Random Value betweeen -0.2 and 0.2
		float random = -0.2f + (float)rand()/((float)RAND_MAX/(0.2-(-0.2)));

		//Add random uniformly to pixel data
		data[i] = data[i] + (random * 256);
		data[i+1] = data[i+1] + (random * 256);
		data[i+2] = data[i+2] + (random * 256); 
		
		//Find resulting intensity
		float intensity = (float) (data[i] * 0.299 + data[i+1] * 0.587 + data[i+2] * 0.114)/256;

		//Apply Brightness Preserving Threshold Dithering
		if (intensity > averageintensity)
		{
			data[i] = 255;
			data[i+1] = data[i];
			data[i+2] = data[i];
		}
		else
		{
			data[i] = 0;
			data[i+2] = 0;
			data[i+3] = 0;
		}
	}

    bench = clock() - bench;
    cout << "Time Elapsed:  " << ((float) bench)/CLOCKS_PER_SEC << " sec" << endl;
    
    return true;
}// Dither_Random


///////////////////////////////////////////////////////////////////////////////
//
//      Perform Floyd-Steinberg dithering on the image.  Return success of 
//  operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Dither_FS()
{
    ClearToBlack();
    return false;
}// Dither_FS


///////////////////////////////////////////////////////////////////////////////
//
//      Dither the image while conserving the average brightness.  Return 
//  success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Dither_Bright()
{
    // Print diagnostic message
    if (data)
		cout << "Executing Dither_Bright()" << endl;

    clock_t time;
    time = clock();
    
	int offset = width * height;
	int arraysize = offset * 4;

	float averageintensity = 0.0f;

	cout << "Calculating Average Intensity" << endl;
	// Compute average intensity over the image
	for (int i = 0; i < arraysize; i+=4)
	{
		averageintensity += (float) (data[i] * 0.299 + data[i+1] * 0.587 + data[i+2] * 0.114)/256;
	}

	averageintensity = averageintensity / offset;
	cout << "Average Intensity: " << averageintensity << endl << endl;

	cout << "Applying Threshold based on Average Intensity" << endl;
	for (int i = 0; i < arraysize; i+=4)
	{
		float intensity = (float) (data[i] * 0.299 + data[i+1] * 0.587 + data[i+2] * 0.114)/256;

		if (intensity > averageintensity)
		{
			data[i] = 255;
			data[i+1] = data[i];
			data[i+2] = data[i];
		}
		else
		{
			data[i] = 0;
			data[i+2] = 0;
			data[i+3] = 0;
		}
	}

    time = clock() - time;
    cout << "Time Elapsed:  " << ((float) time)/CLOCKS_PER_SEC << " sec" << endl;
    
    return true;
}// Dither_Bright


///////////////////////////////////////////////////////////////////////////////
//
//      Perform clustered differing of the image.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Dither_Cluster()
{
    ClearToBlack();
    return false;
}// Dither_Cluster


///////////////////////////////////////////////////////////////////////////////
//
//  Convert the image to an 8 bit image using Floyd-Steinberg dithering over
//  a uniform quantization - the same quantization as in Quant_Uniform.
//  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Dither_Color()
{
    ClearToBlack();
    return false;
}// Dither_Color


///////////////////////////////////////////////////////////////////////////////
//
//      Composite the current image over the given image.  Return success of 
//  operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Comp_Over(TargaImage* pImage)
{
    if (width != pImage->width || height != pImage->height)
    {
        cout <<  "Comp_Over: Images not the same size\n";
        return false;
    }

    ClearToBlack();
    return false;
}// Comp_Over


///////////////////////////////////////////////////////////////////////////////
//
//      Composite this image "in" the given image.  See lecture notes for 
//  details.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Comp_In(TargaImage* pImage)
{
    if (width != pImage->width || height != pImage->height)
    {
        cout << "Comp_In: Images not the same size\n";
        return false;
    }

    ClearToBlack();
    return false;
}// Comp_In


///////////////////////////////////////////////////////////////////////////////
//
//      Composite this image "out" the given image.  See lecture notes for 
//  details.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Comp_Out(TargaImage* pImage)
{
    if (width != pImage->width || height != pImage->height)
    {
        cout << "Comp_Out: Images not the same size\n";
        return false;
    }

    ClearToBlack();
    return false;
}// Comp_Out


///////////////////////////////////////////////////////////////////////////////
//
//      Composite current image "atop" given image.  Return success of 
//  operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Comp_Atop(TargaImage* pImage)
{
    if (width != pImage->width || height != pImage->height)
    {
        cout << "Comp_Atop: Images not the same size\n";
        return false;
    }

    ClearToBlack();
    return false;
}// Comp_Atop


///////////////////////////////////////////////////////////////////////////////
//
//      Composite this image with given image using exclusive or (XOR).  Return
//  success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Comp_Xor(TargaImage* pImage)
{
    if (width != pImage->width || height != pImage->height)
    {
        cout << "Comp_Xor: Images not the same size\n";
        return false;
    }

    ClearToBlack();
    return false;
}// Comp_Xor


///////////////////////////////////////////////////////////////////////////////
//
//      Calculate the difference bewteen this imag and the given one.  Image 
//  dimensions must be equal.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Difference(TargaImage* pImage)
{
    if (!pImage)
        return false;

    if (width != pImage->width || height != pImage->height)
    {
        cout << "Difference: Images not the same size\n";
        return false;
    }// if

    for (int i = 0 ; i < width * height * 4 ; i += 4)
    {
        unsigned char        rgb1[3];
        unsigned char        rgb2[3];

        RGBA_To_RGB(data + i, rgb1);
        RGBA_To_RGB(pImage->data + i, rgb2);

        data[i] = abs(rgb1[0] - rgb2[0]);
        data[i+1] = abs(rgb1[1] - rgb2[1]);
        data[i+2] = abs(rgb1[2] - rgb2[2]);
        data[i+3] = 255;
    }

    return true;
}// Difference


///////////////////////////////////////////////////////////////////////////////
//
//      Perform 5x5 box filter on this image.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Filter_Box()
{
    ClearToBlack();
    return false;
}// Filter_Box


///////////////////////////////////////////////////////////////////////////////
//
//      Perform 5x5 Bartlett filter on this image.  Return success of 
//  operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Filter_Bartlett()
{
    ClearToBlack();
    return false;
}// Filter_Bartlett


///////////////////////////////////////////////////////////////////////////////
//
//      Perform 5x5 Gaussian filter on this image.  Return success of 
//  operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Filter_Gaussian()
{

    // Print diagnostic message
    if (data)
        cout << "Executing Filter_Gaussian()" << endl;

    clock_t time;
    time = clock();

    const int offset = width * height;
    const int arraysize = offset * 4;
    
    //Setup Filter Parameters
    const int size = 5;
    const int center = (size/2); //2
    
    //Construct 1D mask
    int mask1D[size];

    for (int n = 0; n < size; ++n)
    {
        mask1D[n] = Binomial(size-1, n);
    }

    //Construct 2D mask
    int mask2D [size][size];

    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            mask2D[i][j] = mask1D[i] * mask1D[j];
            cout << mask2D[i][j] << "\t";
        }
        cout << endl;
    }

    // Setup temporary image
    unsigned char * datatemp;
    datatemp = new unsigned char[width * height * 4];

    // Iterate through data
    cout << "Applying 5x5 Gaussian Filter" << endl;

    for (int i = 0; i < arraysize; i+=4)
    {
        
        
        float sumR = 0.0f;
        float sumG = 0.0f;
        float sumB = 0.0f;
        int count = 0;

        //Calculate current pixel x and y
        int currentrow = (i/4) / width;
        int currentcolumn = ((i/4) % width);
        
        //Iterate through 2D Mask
        for (int j = 0; j < size; j++)	//Row
        {
            int moverow = (j - center); //Pixel
            for (int k = 0; k < size; k++) //Column
            {
                int movecolumn = (k - center); //Pixel
                int index2 = ((movecolumn*4) + (moverow*(width)*4));  //Subpixel
                
                if (currentcolumn + movecolumn <= -1 || currentcolumn + movecolumn >= width - 1)
                {
                    //cout << "Column: " << currentcolumn << endl;
                    continue;
                }

                if (currentrow + moverow <= -1 || currentrow + moverow >= height)
                {
                    //cout << "Row: " << currentrow << endl;
                    continue;
                }
                
                sumR = sumR + (float) mask2D[j][k]*data[i+index2];
                sumG = sumG + (float) mask2D[j][k]*data[i+index2+1];
                sumB = sumB + (float) mask2D[j][k]*data[i+index2+2];
                count = count + mask2D[j][k];
            }
        }

        datatemp[i] = sumR / count;
        datatemp[i+1] = sumG / count;
        datatemp[i+2] = sumB / count;
        datatemp[i+3] = data[i+3];
    }

    memcpy(data, datatemp, sizeof(unsigned char) * width * height * 4);
    delete [] datatemp;
    
    time = clock() - time;
    cout << "Time Elapsed:  " << ((float) time)/CLOCKS_PER_SEC << " sec" << endl;

    return true;
}// Filter_Gaussian

///////////////////////////////////////////////////////////////////////////////
//
//      Perform NxN Gaussian filter on this image.  Return success of 
//  operation.
//
///////////////////////////////////////////////////////////////////////////////

bool TargaImage::Filter_Gaussian_N( unsigned int N )
{
    // Print diagnostic message
    if (data)
        cout << "Executing Filter_Gaussian_N(" << N << ")" << endl;

    clock_t time;
    time = clock();

    const int offset = width * height;
    const int arraysize = offset * 4;

    //Setup Filter Parameters
    const unsigned int size = N;
    const int center = (size/2); //2

    //Construct 1D mask
    int * mask1D = new int[size];

    for (int n = 0; n < size; ++n)
    {
        mask1D[n] = Binomial(size-1, n);
    }

    //Construct 2D mask vector
    std::vector< std::vector<int> > mask2D;

    mask2D.resize(size);
    for(int i = 0 ; i < size ; ++i)
    {
        mask2D[i].resize(size);
    }

    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            mask2D[i][j] = mask1D[i] * mask1D[j];
            cout << mask2D[i][j] << "\t";
        }
        
        cout << endl;
    }

    //Cleanup
    delete [] mask1D;

    // Setup temporary image
    unsigned char * datatemp;
    datatemp = new unsigned char[width * height * 4];

    // Iterate through data
    cout << "Applying " << size << "x" << size << " Gaussian Filter" << endl;

    for (int i = 0; i < arraysize; i+=4)
    {
        float sumR = 0.0f;
        float sumG = 0.0f;
        float sumB = 0.0f;
        int count = 0;

        //Calculate current pixel x and y
        int currentrow = (i/4) / width;
        int currentcolumn = ((i/4) % width);
        
        //Iterate through 2D Mask
        for (int j = 0; j < size; j++)	//Row
        {
            int moverow = (j - center); //Pixel
            for (int k = 0; k < size; k++) //Column
            {
                int movecolumn = (k - center); //Pixel
                int index2 = ((movecolumn*4) + (moverow*(width)*4));  //Subpixel

                if (currentcolumn + movecolumn <= -1 || currentcolumn + movecolumn >= width - 1) 
                    continue;

                if (currentrow + moverow <= -1 || currentrow + moverow >= height)
                    continue;

                sumR = sumR + (float) mask2D[j][k]*data[i+index2];
                sumG = sumG + (float) mask2D[j][k]*data[i+index2+1];
                sumB = sumB + (float) mask2D[j][k]*data[i+index2+2];
                count = count + mask2D[j][k];
            }
        }
        datatemp[i] = sumR / count;
        datatemp[i+1] = sumG / count;
        datatemp[i+2] = sumB / count;
        datatemp[i+3] = data[i+3];
    }

    memcpy(data, datatemp, sizeof(unsigned char) * width * height * 4);
    delete [] datatemp;

    time = clock() - time;
    cout << "Time Elapsed:  " << ((float) time)/CLOCKS_PER_SEC << " sec" << endl;
    
    return true;
}// Filter_Gaussian_N


///////////////////////////////////////////////////////////////////////////////
//
//      Perform 5x5 edge detect (high pass) filter on this image.  Return 
//  success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Filter_Edge()
{
    ClearToBlack();
    return false;
}// Filter_Edge


///////////////////////////////////////////////////////////////////////////////
//
//      Perform a 5x5 enhancement filter to this image.  Return success of 
//  operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Filter_Enhance()
{
    ClearToBlack();
    return false;
}// Filter_Enhance


///////////////////////////////////////////////////////////////////////////////
//
//      Run simplified version of Hertzmann's painterly image filter.
//      You probably will want to use the Draw_Stroke funciton and the
//      Stroke class to help.
// Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::NPR_Paint()
{
    ClearToBlack();
    return false;
}



///////////////////////////////////////////////////////////////////////////////
//
//      Halve the dimensions of this image.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Half_Size()
{
    ClearToBlack();
    return false;
}// Half_Size


///////////////////////////////////////////////////////////////////////////////
//
//      Double the dimensions of this image.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Double_Size()
{
    ClearToBlack();
    return false;
}// Double_Size


///////////////////////////////////////////////////////////////////////////////
//
//      Scale the image dimensions by the given factor.  The given factor is 
//  assumed to be greater than one.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Resize(float scale)
{
    ClearToBlack();
    return false;
}// Resize


//////////////////////////////////////////////////////////////////////////////
//
//      Rotate the image clockwise by the given angle.  Do not resize the 
//  image.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Rotate(float angleDegrees)
{
    ClearToBlack();
    return false;
}// Rotate


//////////////////////////////////////////////////////////////////////////////
//
//      Given a single RGBA pixel return, via the second argument, the RGB
//      equivalent composited with a black background.
//
///////////////////////////////////////////////////////////////////////////////
void TargaImage::RGBA_To_RGB(unsigned char *rgba, unsigned char *rgb)
{
    const unsigned char	BACKGROUND[3] = { 0, 0, 0 };

    unsigned char  alpha = rgba[3];

    if (alpha == 0)
    {
        rgb[0] = BACKGROUND[0];
        rgb[1] = BACKGROUND[1];
        rgb[2] = BACKGROUND[2];
    }
    else
    {
	    float	alpha_scale = (float)255 / (float)alpha;
	    int	val;
	    int	i;

	    for (i = 0 ; i < 3 ; i++)
	    {
	        val = (int)floor(rgba[i] * alpha_scale);
	        if (val < 0)
		    rgb[i] = 0;
	        else if (val > 255)
		    rgb[i] = 255;
	        else
		    rgb[i] = val;
	    }
    }
}// RGA_To_RGB


///////////////////////////////////////////////////////////////////////////////
//
//      Copy this into a new image, reversing the rows as it goes. A pointer
//  to the new image is returned.
//
///////////////////////////////////////////////////////////////////////////////
TargaImage* TargaImage::Reverse_Rows(void)
{
    unsigned char   *dest = new unsigned char[width * height * 4];
    TargaImage	    *result;
    int 	        i, j;

    if (! data)
    	return NULL;

    for (i = 0 ; i < height ; i++)
    {
	    int in_offset = (height - i - 1) * width * 4;
	    int out_offset = i * width * 4;

	    for (j = 0 ; j < width ; j++)
        {
	        dest[out_offset + j * 4] = data[in_offset + j * 4];
	        dest[out_offset + j * 4 + 1] = data[in_offset + j * 4 + 1];
	        dest[out_offset + j * 4 + 2] = data[in_offset + j * 4 + 2];
	        dest[out_offset + j * 4 + 3] = data[in_offset + j * 4 + 3];
        }
    }

    result = new TargaImage(width, height, dest);
    delete[] dest;
    return result;
}// Reverse_Rows


///////////////////////////////////////////////////////////////////////////////
//
//      Clear the image to all black.
//
///////////////////////////////////////////////////////////////////////////////
void TargaImage::ClearToBlack()
{
    memset(data, 0, width * height * 4);
}// ClearToBlack

///////////////////////////////////////////////////////////////////////////////
//
//      Helper function for the painterly filter; paint a stroke at
// the given location
//
///////////////////////////////////////////////////////////////////////////////
void TargaImage::Paint_Stroke(const Stroke& s) {
   int radius_squared = (int)s.radius * (int)s.radius;
   for (int x_off = -((int)s.radius); x_off <= (int)s.radius; x_off++) {
      for (int y_off = -((int)s.radius); y_off <= (int)s.radius; y_off++) {
         int x_loc = (int)s.x + x_off;
         int y_loc = (int)s.y + y_off;
         // are we inside the circle, and inside the image?
         if ((x_loc >= 0 && x_loc < width && y_loc >= 0 && y_loc < height)) {
            int dist_squared = x_off * x_off + y_off * y_off;
            if (dist_squared <= radius_squared) {
               data[(y_loc * width + x_loc) * 4 + 0] = s.r;
               data[(y_loc * width + x_loc) * 4 + 1] = s.g;
               data[(y_loc * width + x_loc) * 4 + 2] = s.b;
               data[(y_loc * width + x_loc) * 4 + 3] = s.a;
            } else if (dist_squared == radius_squared + 1) {
               data[(y_loc * width + x_loc) * 4 + 0] = 
                  (data[(y_loc * width + x_loc) * 4 + 0] + s.r) / 2;
               data[(y_loc * width + x_loc) * 4 + 1] = 
                  (data[(y_loc * width + x_loc) * 4 + 1] + s.g) / 2;
               data[(y_loc * width + x_loc) * 4 + 2] = 
                  (data[(y_loc * width + x_loc) * 4 + 2] + s.b) / 2;
               data[(y_loc * width + x_loc) * 4 + 3] = 
                  (data[(y_loc * width + x_loc) * 4 + 3] + s.a) / 2;
            }
         }
      }
   }
}


///////////////////////////////////////////////////////////////////////////////
//
//      Build a Stroke
//
///////////////////////////////////////////////////////////////////////////////
Stroke::Stroke() {}


///////////////////////////////////////////////////////////////////////////////
//
//      Build a Stroke
//
///////////////////////////////////////////////////////////////////////////////
Stroke::Stroke(unsigned int iradius, unsigned int ix, unsigned int iy,
               unsigned char ir, unsigned char ig, unsigned char ib, unsigned char ia) :
   radius(iradius),x(ix),y(iy),r(ir),g(ig),b(ib),a(ia)
{
}





