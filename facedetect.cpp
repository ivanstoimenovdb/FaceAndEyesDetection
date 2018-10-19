#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>

using namespace std;
using namespace cv;

static void help()
{
    cout << "\nThis program demonstrates the use of cv::CascadeClassifier class to detect objects (Face + eyes). You can use Haar or LBP features.\n"
            "This classifier can recognize many kinds of rigid objects, once the appropriate classifier is trained.\n"
            "It's most known use is for faces.\n"
            "Usage:\n"
            "./facedetect [--cascade=<cascade_path> this is the primary trained classifier such as frontal face]\n"
               "   [--nested-cascade[=nested_cascade_path this an optional secondary classifier such as eyes]]\n"
               "   [--scale=<image scale greater or equal to 1, try 1.3 for example>]\n"
               "   [--try-flip]\n"
               "   [filename|camera_index]\n\n"
            "see facedetect.cmd for one call:\n"
            "./facedetect --cascade=\"../../data/haarcascades/haarcascade_frontalface_alt.xml\" --nested-cascade=\"../../data/haarcascades/haarcascade_eye_tree_eyeglasses.xml\" --scale=1.3\n\n"
            "During execution:\n\tHit any key to quit.\n"
            "\tUsing OpenCV version " << CV_VERSION << "\n" << endl;
}

/********************************************************
*         This method  is for detect and draw frame     *
********************************************************/

void detectAndDraw( Mat& img, CascadeClassifier& cascade,
                    CascadeClassifier& nestedCascade,
                    double scale, bool tryflip );

/***************************************************************
*            Variables that are neede for detection            *
***************************************************************/

// This variable discribed facedetection.
string cascadeName;

// This variable describe elements in face.
string nestedCascadeName;

int main( int argc, const char** argv )
{
	/***************************************************************
	*            Object that are neede for Handleing               *
	***************************************************************/

	// Object whos handle video capturing.
    VideoCapture capture;

	// Object that draws frame and load image.
    Mat frame, image;

	// Variable of input video file/ video device.
    string inputName;
    bool tryflip;

	// Objects that Detect face, and inner elements of face.
    CascadeClassifier cascade, nestedCascade;

	// Variable which save scale of frame.
    double scale;

	/*******************************************************************************
	*            Description which are needed for Face and eyes detection          *
	*******************************************************************************/

	// Object dat load description file (discribe the coordinates of face and eyes).
    cv::CommandLineParser parser(argc, argv,
        "{help h||}"
        "{cascade|../../../../opencv-3.4.3/opencv-3.4.3/data/haarcascades/haarcascade_frontalface_alt.xml|}"
        "{nested-cascade|../../../../opencv-3.4.3/opencv-3.4.3/data/haarcascades/haarcascade_eye_tree_eyeglasses.xml|}"
        "{scale|1|}{try-flip||}{@filename||}"
    );
	
	/*******************************************************************************
	*                                Proccessing                                   *
	*******************************************************************************/

	/*******************************************************************************
	*                              Loading parser                                  *
	*******************************************************************************/
	// Checking of loading parser description files.
    if (parser.has("help"))
    {
		// if parser files not load it will point to help
        help();
        return 0;
    }

	/*******************************************************************************
	*                           Setting name of detections                         *
	*******************************************************************************/

	// set Facedetection name 
    cascadeName = parser.get<string>("cascade");
	
	// set Elements inner face name 
    nestedCascadeName = parser.get<string>("nested-cascade");
    scale = parser.get<double>("scale");

	/*******************************************************************************
	*                                Checking scale                                *
	*******************************************************************************/

	// Checking scale is it too small
    if (scale < 1)
        scale = 1;
    tryflip = parser.has("try-flip");

	// Input the video file/device.
    inputName = parser.get<string>("@filename");

	/*******************************************************************************
	*                      Checking parser and description files                   *
	*******************************************************************************/

	// Cheching of is it loaded parser. 
    if (!parser.check())
    {	
		// It returns Exception if it doesnt load
        parser.printErrors();
        return 0;
    }

	// Checking of is it load description file of inner elements of face.
    if ( !nestedCascade.load( nestedCascadeName ) )
        cerr << "WARNING: Could not load classifier cascade for nested objects" << endl;

	// Checking of is it load description file of face.
    if( !cascade.load( cascadeName ) )
    {
        cerr << "ERROR: Could not load classifier cascade" << endl;

		// it return load is it doesnt load both description.
        help();
        return -1;
    }

	/*******************************************************************************
	*                          Checking of input files							   *
	*							  and input devices                                *
	*******************************************************************************/
	
	// Checking if input file which is processed of video capture is empty
    if( inputName.empty() || (isdigit(inputName[0]) && inputName.size() == 1) )
    {
        int camera = inputName.empty() ? 0 : inputName[0] - '0';

		//  Checking is device is open
        if(!capture.open(camera))

			// if it is not open connection with camera return this cout
            cout << "Capture from camera #" <<  camera << " didn't work" << endl;
    }
	     // Checks for input size
    else if( inputName.size() )
    {
		// Read input file of video file.
        image = imread( inputName, 1 );
        if( image.empty() )
        {
			// if there is not input of video file
            if(!capture.open( inputName ))

				// this is couting
                cout << "Could not read " << inputName << endl;
        }
    }
    else
    {
		// if there is video file it reads from folder E:\opencv\opencv-3.4.3\opencv-3.4.3\samples\data\lena.jpg
		// But you should put your video file in this directory
        image = imread( "../data/lena.jpg", 1 );

		// if there is not video file this is messaging
        if(image.empty()) cout << "Couldn't read ../data/lena.jpg" << endl;
    }

	// Checks the object capture is open
    if( capture.isOpened() )
    {
        cout << "Video capturing has been started ..." << endl;

		// it grabs all frames which are comming from device in infinity loop
        for(;;)
        {
            capture >> frame;
            if( frame.empty() )
                break;

			// Clone some figure (in our case is circle)
            Mat frame1 = frame.clone();
			
			/*******************************************************************************
			*                              Detect and draw objects                         *
			*******************************************************************************/
			// Method whos draws and detects objects
            detectAndDraw( frame1, cascade, nestedCascade, scale, tryflip );

            char c = (char)waitKey(10);
            if( c == 27 || c == 'q' || c == 'Q' )
                break;
        }
    }
    else
    {
		// This is detection of faces in video file  
        cout << "Detecting face(s) in " << inputName << endl;

		// Checks is there image file
        if( !image.empty() )
        {
			// Detect and draw frame in image (image file).
            detectAndDraw( image, cascade, nestedCascade, scale, tryflip );
            waitKey(0);
        }
		 // checking if there is input video file.
        else if( !inputName.empty() )
        {
			// read text file with list of image files. Proccesing is line by line.

            /* assume it is a text file containing the
            list of the image filenames to be processed - one per line */
            FILE* f = fopen( inputName.c_str(), "rt" );
            if( f )
            {
				/*******************************************************************************
				*              Reading row by row whole txt file with list of images           *
				*******************************************************************************/

                char buf[1000+1];
                while( fgets( buf, 1000, f ) )
                {
                    int len = (int)strlen(buf);
                    while( len > 0 && isspace(buf[len-1]) )
                        len--;
                    buf[len] = '\0';
                    cout << "file " << buf << endl;
                    image = imread( buf, 1 );
                    if( !image.empty() )
                    {
						// Detect and draw the object in all list of images.
                        detectAndDraw( image, cascade, nestedCascade, scale, tryflip );
                        char c = (char)waitKey(0);
                        if( c == 27 || c == 'q' || c == 'Q' )
                            break;
                    }
                    else
                    {
						// The image couldn't be readen
                        cerr << "Aw snap, couldn't read image " << buf << endl;
                    }
                }
				// Close file stream.
                fclose(f);
            }
        }
    }

    return 0;
}

/*******************************************************************************
*                          Detect and draw method itself                       *
*******************************************************************************/

void detectAndDraw( Mat& img, CascadeClassifier& cascade,
                    CascadeClassifier& nestedCascade,
                    double scale, bool tryflip )
{
    double t = 0;

	// Variables that processing the face detect and inner face elements.
    vector<Rect> faces, faces2;

	// Getting some color histogram
    const static Scalar colors[] =
    {
        Scalar(255,0,0),
        Scalar(255,128,0),
        Scalar(255,255,0),
        Scalar(0,255,0),
        Scalar(0,128,255),
        Scalar(0,255,255),
        Scalar(0,0,255),
        Scalar(255,0,255)
    };

	// This variables get size of images and color of images.
    Mat gray, smallImg;

    cvtColor( img, gray, COLOR_BGR2GRAY );
    double fx = 1 / scale;
    resize( gray, smallImg, Size(), fx, fx, INTER_LINEAR_EXACT );
    equalizeHist( smallImg, smallImg );

    t = (double)getTickCount();
    cascade.detectMultiScale( smallImg, faces,
        1.1, 2, 0
        //|CASCADE_FIND_BIGGEST_OBJECT
        //|CASCADE_DO_ROUGH_SEARCH
        |CASCADE_SCALE_IMAGE,
        Size(30, 30) );
    if( tryflip )
    {
        flip(smallImg, smallImg, 1);
        cascade.detectMultiScale( smallImg, faces2,
                                 1.1, 2, 0
                                 //|CASCADE_FIND_BIGGEST_OBJECT
                                 //|CASCADE_DO_ROUGH_SEARCH
                                 |CASCADE_SCALE_IMAGE,
                                 Size(30, 30) );

		// processing of face and inner elements of face.
        for( vector<Rect>::const_iterator r = faces2.begin(); r != faces2.end(); ++r )
        {
			// Get size of face and elements of face.
            faces.push_back(Rect(smallImg.cols - r->x - r->width, r->y, r->width, r->height));
        }
    }
    t = (double)getTickCount() - t;
	
	// set frequency of detection and drawing. 
    printf( "detection time = %g ms\n", t*1000/getTickFrequency());
    for ( size_t i = 0; i < faces.size(); i++ )
    {
		/*******************************************************************************
		*                                 Drawing of frame.                            *
		*******************************************************************************/

		// Get face of previous infinity loop
        Rect r = faces[i];

		// set scale of image to small image.
        Mat smallImgROI;

		// Handle nested objects (eyes)
        vector<Rect> nestedObjects;

		// this is center of an eyes
        Point center;

		// this is blue color of frame.
        Scalar color = colors[i%8];

		// variable which is handle radius.
        int radius;

		// Drawing frame of the face and eyes by radio

        double aspect_ratio = (double)r.width/r.height;
        if( 0.75 < aspect_ratio && aspect_ratio < 1.3 )
        {
            center.x = cvRound((r.x + r.width*0.5)*scale);
            center.y = cvRound((r.y + r.height*0.5)*scale);
            radius = cvRound((r.width + r.height)*0.25*scale);
            circle( img, center, radius, color, 3, 8, 0 );
        }
        else
            rectangle( img, Point(cvRound(r.x*scale), cvRound(r.y*scale)),
                       Point(cvRound((r.x + r.width-1)*scale), cvRound((r.y + r.height-1)*scale)),
                       color, 3, 8, 0);
        if( nestedCascade.empty() )
            continue;
        smallImgROI = smallImg( r );
        nestedCascade.detectMultiScale( smallImgROI, nestedObjects,
            1.1, 2, 0
            //|CASCADE_FIND_BIGGEST_OBJECT
            //|CASCADE_DO_ROUGH_SEARCH
            //|CASCADE_DO_CANNY_PRUNING
            |CASCADE_SCALE_IMAGE,
            Size(30, 30) );
        for ( size_t j = 0; j < nestedObjects.size(); j++ )
        {
            Rect nr = nestedObjects[j];
            center.x = cvRound((r.x + nr.x + nr.width*0.5)*scale);
            center.y = cvRound((r.y + nr.y + nr.height*0.5)*scale);
            radius = cvRound((nr.width + nr.height)*0.25*scale);
            circle( img, center, radius, color, 3, 8, 0 );
        }
    }
	// Showing the whole frame whith detecte face and eyes.
    imshow( "result", img );
}
