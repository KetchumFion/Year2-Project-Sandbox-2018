#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	//test
	ofImage temp;
	cv::Mat img;
	std::string fname = "mapGame/ReferenceData/testMap.png";
	if (!temp.loadImage(fname))
	{
		ofLogVerbose("CMapGameController") << "SafeReadBinaryImage(): could not read " + fname;
	}
	cv::Mat t1 = ofxCv::toCv(temp.getPixels());

	cv::Mat t2;
	if (t1.channels() > 1)
		cv::cvtColor(t1, t2, cv::COLOR_BGR2GRAY);
	else
		t1.copyTo(t2);

	cv::normalize(t2, img, 0, 255, cv::NORM_MINMAX);

	//IplImage im = img;
	//IplImage im = IplImage(img);

	//IplImage* im = cvCreateImage(cvGetSize(mat), 8, 1);
	//cvGetImage(img, im);

	//cvSaveImage("rice1.bmp", im);
	//cvSave("my_matrix.xml", &img);

	// OF basics
	ofSetFrameRate(60);
	ofBackground(0);
	ofSetVerticalSync(true);
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetLogLevel("ofThread", OF_LOG_WARNING);
	ofSetLogLevel("ofFbo", OF_LOG_ERROR);
	ofSetLogLevel("ofShader", OF_LOG_ERROR);
	ofSetLogLevel("ofxKinect", OF_LOG_WARNING);

	// Setup kinectProjector
	kinectProjector = std::make_shared<KinectProjector>(projWindow);
	kinectProjector->setup(true);
	//kinectProjector2 = std::make_shared<KinectProjector>(pcjWindow);
	//kinectProjector2->setup(true);
	// Setup sandSurfaceRenderer
	sandSurfaceRenderer = new SandSurfaceRenderer(kinectProjector, projWindow);
	sandSurfaceRenderer->setup(true);

	// Retrieve variables
	ofVec2f kinectRes = kinectProjector->getKinectRes();
	ofVec2f projRes = ofVec2f(projWindow->getWidth(), projWindow->getHeight());
	ofRectangle kinectROI = kinectProjector->getKinectROI();
	////mainWindowROI = ofRectangle(600, 30, 600, 450);
	//mainWindowROI = ofRectangle(0, 0, 640, 480);
	mainWindowROI = ofRectangle((ofGetWindowWidth() - kinectRes.x) / 2, (ofGetWindowHeight() - kinectRes.y) / 2, kinectRes.x, kinectRes.y);
	cout << "Kinect roi" << endl;
	cout<< kinectROI .x<<endl;
	cout << kinectROI.y << endl;
	cout << kinectROI.width << endl;
	cout << kinectROI.height << endl;
	cout << "projRes rec" << endl;
	cout << projRes.x << endl;
	cout << projRes.y << endl;
	shapeGameManager.setProjectorRes(projRes);
	shapeGameManager.setKinectRes(kinectRes);
	shapeGameManager.setKinectROI(kinectROI);

	


	ofGLFWWindowSettings settings;
	settings.width = 1600; // Default settings
	settings.height = 800;
	settings.setPosition(ofVec2f(0, 0));
	settings.resizable = true;
	settings.decorated = true;
	settings.title = "Magic-Sand ";
	shared_ptr<ofApp> mainApp(new ofApp);
	shared_ptr<ofAppBaseWindow> thirdWindow = ofCreateWindow(settings);
	thirdWindow->setWindowShape(1080, 1920);

	ofAddListener(thirdWindow->events().draw, mainApp.get(), &ofApp::drawPcjWindow);
	mainApp->pcjWindow = thirdWindow;


	int z = 0;
	Lunar_Starter = new ofxDatGui(130, 670 - z); //Button for lunar
	Lunar_Starter->addButton(" ");//1

	Sandbox_Starter = new ofxDatGui(670, 670 - z);
	Sandbox_Starter->addButton("  ");//2

	Sandbox_flood = new ofxDatGui(670, 730 - z);
	Sandbox_flood->addButton("   ");//3

	Triangle_Starter = new ofxDatGui(130, 1480 - z);
	Triangle_Starter->addButton("     ");//4

	Circle_Starter = new ofxDatGui(130, 1530 - z);
	Circle_Starter->addButton("       ");//5

	Square_Starter = new ofxDatGui(130, 1580 - z);
	Square_Starter->addButton("      ");//6

	ShapeHeart_Starter = new ofxDatGui(130, 1630 - z);
	ShapeHeart_Starter->addButton("    ");//7 

	ShapeUK_Starter = new ofxDatGui(130, 1430 - z);//map
	ShapeUK_Starter->addButton("    ");//7 

	Leardboard_Starter = new ofxDatGui(670, 1480 - z);
	Leardboard_Starter->addButton("   "); //9 the learderboard


	Lunar_Starter->onButtonEvent(this, &ofApp::onButtonEvent);
	Sandbox_Starter->onButtonEvent(this, &ofApp::onButtonEvent);
	ShapeHeart_Starter->onButtonEvent(this, &ofApp::onButtonEvent);
	Sandbox_flood->onButtonEvent(this, &ofApp::onButtonEvent);
	Triangle_Starter->onButtonEvent(this, &ofApp::onButtonEvent);
	Square_Starter->onButtonEvent(this, &ofApp::onButtonEvent);
	Circle_Starter->onButtonEvent(this, &ofApp::onButtonEvent);
	Leardboard_Starter->onButtonEvent(this, &ofApp::onButtonEvent);
	ShapeUK_Starter->onButtonEvent(this, &ofApp::onButtonEvent);

	/*ofSetVerticalSync(true);
	pathLines.setMode(OF_PRIMITIVE_LINE_STRIP);
	ofDrawGrid(500, 10, false, false, true, false);

	ofSetLineWidth(2);
	ofSetColor(cyan);
	pathLines.draw();*/

}

//--------------------------------------------------------------
void ofApp::update(){
	// Call kinectProjector->update() first during the update function()
	if (changeTheme) {
		changeTheme = false;
		if (currentTheme == SANDBOX_THEME) {
			sandSurfaceRenderer->heightMap.loadFile("colorMaps/HeightColorMap.xml");
		}else if (currentTheme == LUNAR_THEME) {
			sandSurfaceRenderer->heightMap.loadFile("colorMaps/HeightColorMapL.xml");
		}
	}


	kinectProjector->update();
	sandSurfaceRenderer->update();
	if (shapeGameRunning) {
		getBinaryImage();
		//shapeGameManager.drawScore(shapeGame_Similarity);
		if (ConnectedComponentAnalysis()) {
			if (matchMap()) {
				
			}
			else {
				shapeGame_Similarity = 0;
			}
		}
		else {
			shapeGame_Similarity = 0;
		}
		//ConnectedComponentAnalysis();
		//matchMap();
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	float x = mainWindowROI.x;
	float y = mainWindowROI.y;
	float w = mainWindowROI.width;
	float h = mainWindowROI.height;

	if (kinectProjector->GetApplicationState() == KinectProjector::APPLICATION_STATE_RUNNING)
	{
		sandSurfaceRenderer->drawMainWindow(x, y, w, h);//400, 20, 400, 300);
	}

	kinectProjector->drawMainWindow(x, y, w, h);
}

void ofApp::drawProjWindow(ofEventArgs &args)
{
	if (kinectProjector->GetApplicationState() == KinectProjector::APPLICATION_STATE_RUNNING)
	{
		sandSurfaceRenderer->drawProjectorWindow();
		//mapGameController.drawProjectorWindow();
		//boidGameController.drawProjectorWindow();
	}
	kinectProjector->drawProjectorWindow();
	if (shapeGameRunning) {
		ofTrueTypeFont text;
		//ofSetColor(255, 0, 0);
		text.load("ofxbraitsch/fonts/Verdana.ttf", 70);
		stringstream ttext;
		string ScoreText;
		//int tscore = (int)shapeGame_Similarity;
		ttext << " Current Similarity: " << shapeGame_Similarity*100 << "%";
		ScoreText = ttext.str();
		ofSetColor(255, 0, 0);
		text.drawString("Shape: Circle Island", 100, 100);
		text.drawString(ScoreText, 100, 230);
		if (shapeGame_Similarity>0.85) {
			ofSetColor(0, 100, 0);
			text.drawString("Circle Island found", 350, 400);
		}
		else {
			text.drawString("No match shape", 350, 400);
		}
		ofSetColor(255, 255, 255);
	}
	
}
void ofApp::drawPcjWindow(ofEventArgs &args) {
	ofTrueTypeFont text;


	//if (UI_start) {
	//	UI_start = false;
	//	Lunar_Starter = new ofxDatGui(130, 550);
	//	Lunar_Starter->addButton("Change to Lunar Theme");
	//	
	//	Sandbox_Starter = new ofxDatGui(670, 550);
	//	Sandbox_Starter->addButton("Change to Sandbox Theme");

	//	ShapeGame_Starter = new ofxDatGui(130, 1300);
	//	ShapeGame_Starter->addButton("Start Shape an Island Game");
	//	ofRectangle bounds(30, 30, 500, 500);
	////	TestGUI->add2dPad("Shift to start", bounds);
	////	TestGUI->addMatrix("Clik to start", 49);
	////	TestGUI->addWaveMonitor("Lunar Theme", 10, 10);
	////	TestGUI->onButtonEvent(this, &ofApp::onButtonEvent);
	//	

	//	Lunar_Starter->onButtonEvent(this, &ofApp::onButtonEvent);
	//	Sandbox_Starter->onButtonEvent(this, &ofApp::onButtonEvent);
	//	ShapeGame_Starter->onButtonEvent(this, &ofApp::onButtonEvent);
	//	
	//}
	ofSetBackgroundColor(255, 255, 255);
	//Lunar image
	ofImage Img_Lunar("lunar5.jpg");
	Img_Lunar.draw(40, 345);     //x,y
	//Sandbox image
	ofImage Img_Sandbox("sandbox.png");
	Img_Sandbox.draw(580, 345);
	//Shape Island Game
	ofImage Img_ShapeIsland("shapeGame.png");
	Img_ShapeIsland.draw(40, 930);

	ofSetColor(11, 253, 247);
	ofSetLineWidth(5);
	ofNoFill();
	ofSetLineWidth(20);
	ofSetColor(150, 150, 150);
	ofDrawRectangle(40, 345, 460, 500);  //x,y
	ofSetColor(0, 255, 0);
	ofDrawRectangle(580, 345, 460, 500);
	ofSetColor(0, 191, 255);
	ofDrawRectangle(40, 930, 460, 880);
	ofSetColor(0, 191, 255);
	ofDrawRectangle(580, 930, 460, 880);

	//introduction
	ofSetColor(100, 191, 255);
	ofDrawRectangle(40, 30, 1000, 260);


	//lunar start
	ofSetColor(255, 215, 0);
	ofSetLineWidth(6);
	ofDrawRectangle(128, 668, 275, 30);
	//sandbox start
	ofSetColor(0, 191, 255);
	ofSetLineWidth(6);
	ofDrawRectangle(668, 668, 275, 30);
	ofDrawRectangle(668, 728, 275, 30);

	//shape game start
	ofSetColor(248, 248, 255);
	ofSetLineWidth(6);
	ofDrawRectangle(128, 1478, 275, 30);
	ofDrawRectangle(128, 1528, 275, 30);
	ofDrawRectangle(128, 1578, 275, 30);
	ofDrawRectangle(128, 1628, 275, 30);
	//ofDrawCircle(100,400,300);

	ofSetColor(255, 0, 0);
	text.loadFont("verdana.ttf", 10);
	text.drawString("Click here to start", 700, 670);
	text.drawString("Click here to start", 160, 670);
	text.drawString("Click here to flood the water",700,730);

	text.drawString("Click here to create a Triangle", 160, 1530);
	text.drawString("Click here to create a Square", 160, 1580);
	text.drawString("Click here to create a Circle", 160, 1630);
	//Lunar Theme

	ofSetColor(169, 169, 169);
	text.loadFont("verdana.ttf", 40);
	text.drawString("Lunar Theme", 70, 405);


	//Sandbox Theme
	ofSetColor(0, 255, 127);
	text.loadFont("verdana.ttf", 34);
	text.drawString("Sandbox Theme", 630, 405);


	//Shape an Island Game
	ofSetColor(255, 69, 0);
	text.loadFont("verdana.ttf", 25);
	text.drawString("Shape an Island Game", 70, 970);

	ofSetColor(255, 69, 0);
	text.loadFont("verdana.ttf", 25);
	text.drawString("Leaderboard", 610, 970);

	//ofSetColor(255, 30, 0);
	//text.loadFont("cooperBlack.ttf", 18);
	//text.drawString("1st. t", 620, 1015);
	//text.drawString("2nd. t", 620, 1065);
	//text.drawString("3rd. t", 620, 1115);
	//text.drawString("4th. t", 620, 1165);

	ofFill();
	int e = 400;
	int r = 1000;
	ofSetColor(255, 250, 205);
	ofSetPolyMode(OF_POLY_WINDING_NONZERO);
	ofBeginShape();
	ofVertex(400 + e, 135 + r);
	ofVertex(215 + e, 135 + r);
	ofVertex(365 + e, 25 + r);
	ofVertex(305 + e, 200 + r);
	ofVertex(250 + e, 25 + r);
	ofVertex(305 + e, 700 + r);
	ofEndShape();

	ofEnableAlphaBlending();    // turn on alpha blending
	ofSetColor(238, 233, 191, 165); // red, 50% transparent
	ofSetPolyMode(OF_POLY_WINDING_NONZERO);
	ofBeginShape();
	ofVertex(400 + e + 100, 135 + r + 100);
	ofVertex(215 + e + 100, 135 + r + 100);
	ofVertex(365 + e + 100, 25 + r + 100);
	ofVertex(305 + e + 100, 200 + r + 100);
	ofVertex(250 + e + 100, 25 + r + 100);
	ofVertex(305 + e + 100, 700 + r + 100);
	ofEndShape();

	ofDisableAlphaBlending();   // turn off alpha
	ofSetPolyMode(OF_POLY_WINDING_NONZERO);
	ofBeginShape();
	ofVertex(400 + e + 50, 135 + r + 50);
	ofVertex(215 + e + 50, 135 + r + 50);
	ofVertex(365 + e + 50, 25 + r + 50);
	ofVertex(305 + e + 50, 200 + r + 50);
	ofVertex(250 + e + 50, 25 + r + 50);
	ofVertex(305 + e + 50, 700 + r + 50);
	ofEndShape();
	/*text.drawString("1st. testName1", 630, 480);
	text.drawString("2nd. testName2", 630, 530);
	text.drawString("1st. testName1", 70, 1240);*/

	//introduction
	ofSetColor(50, 105, 127);
	text.loadFont("verdana.ttf", 35);
	text.drawString("Introductions for four areas: ", 50, 80);

	ofSetColor(50, 115, 127);
	text.loadFont("verdana.ttf", 20);
	text.drawString("1.Lunar Theme: The sandbox will show the surface of the moon.", 50, 120);
	text.drawString("2.Sandbox Theme: The sandbox will show the surface of the earth.", 50, 155);
	text.drawString("3.Shape an Island Game: Choose the shape to create.", 50, 190);
	text.drawString("4.leaderboard: The similarity check of the shape game.", 50, 225);
	ofSetColor(255, 255,255);

	ofImage Img_Triangle("Triangle.png");
	Img_Triangle.draw(410, 1520);     //x,y

	ofImage Img_Square("Square.png");
	Img_Square.draw(410, 1570);     //x,y

	ofImage Img_Circle("Circle.png");
	Img_Circle.draw(410, 1620);     //x,y
	
}

void ofApp::onButtonEvent(ofxDatGuiButtonEvent e) {
	if (e.target->is(" ")) {
		//myAlert.setTitle("Please input your name");
		//myAlert.setMessage("It's time to go outside.");
		//myAlert.show();
		cout << "Change to Lunar Theme" << endl;
		//sandSurfaceRenderer->colorMapFile = "HeightColorMapL.xml";
		//sandSurfaceRenderer->heightMap.loadFile("colorMaps/HeightColorMapL.xml");
		changeTheme = true;
		currentTheme = LUNAR_THEME;

		ofxDatGuiThemeCharcoal guiTheme;

		

	}
	else if (e.target->is("  ")) {
		cout << "Change to Sandbox Theme" << endl;
		//sandSurfaceRenderer->heightMap.loadFile("colorMaps/HeightColorMap.xml");
		changeTheme = true;
		currentTheme = SANDBOX_THEME;
	}
	else if (e.target->is("   ")) {
		cout << "Shape an Island Game" << endl;
		shapeGameRunning = !shapeGameRunning;
		
	}

}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == ' ')
	{
		kinectProjector->startApplication();
		//shapeGameManager.GetActualBinaryLandImage();
		//ConnectedComponentAnalysis();
		//matchMap();
		//ofGLFWWindowSettings settings;
		//settings.width = 1600; // Default settings
		//settings.height = 800;
		//settings.setPosition(ofVec2f(0, 0));
		//settings.resizable = true;
		//settings.decorated = true;
		//settings.title = "Magic-Sand ";
		//shared_ptr<ofApp> mainApp(new ofApp);
		//shared_ptr<ofAppBaseWindow> thirdWindow = ofCreateWindow(settings);
		//thirdWindow->setWindowShape(1080, 1920);

		//ofAddListener(thirdWindow->events().draw, mainApp.get(), &ofApp::drawPcjWindow);
		//mainApp->pcjWindow = thirdWindow;


		//Lunar_Starter = new ofxDatGui(130, 550);
		//Lunar_Starter->addButton(" ");

		//Sandbox_Starter = new ofxDatGui(670, 550);
		//Sandbox_Starter->addButton("  ");

		//ShapeGame_Starter = new ofxDatGui(130, 1300);
		//ShapeGame_Starter->addButton("   ");
		//Lunar_Starter->onButtonEvent(this, &ofApp::onButtonEvent);
		//Sandbox_Starter->onButtonEvent(this, &ofApp::onButtonEvent);
		//ShapeGame_Starter->onButtonEvent(this, &ofApp::onButtonEvent);
		



	}
	else if (key == '1') {
		
	}
	else if (key == 'r') {
		//cout << "R" << endl;
		//shapeGameRunning = !shapeGameRunning;
		//cout<<"Shape game: "<<shapeGameRunning<<endl;
		//shapeGameManager.GetActualBinaryLandImage();
		//ofxCvGrayscaleImage BinaryLandImage;
		//std::string BinOutName = "BinaryLandImage.png";
		//kinectProjector->getBinaryLandImage(BinaryLandImage);
		//if (!kinectProjector->getBinaryLandImage(BinaryLandImage)) {
			//cout<<"*!*!Wrong!"<<endl;
		//}
		//ofSaveImage(BinaryLandImage.getPixels(), BinOutName);
	}
	else if (key == 'm') {
	//	shapeGameManager.GetActualBinaryLandImage();
	//	ofxCvGrayscaleImage BinaryLandImage;
	//	std::string BinOutName = "BinaryLandImage.png";
	//	//kinectProjector->getBinaryLandImage(BinaryLandImage);
	//	if (!kinectProjector->getBinaryLandImage(BinaryLandImage)) {
	//		ofSaveImage(BinaryLandImage.getPixels(), BinOutName);
		//sandSurfaceRenderer->heightMap.loadFile("colorMaps/HeightColorMapL.xml");
		}
	//	
	//}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	kinectProjector->mouseDragged(x - mainWindowROI.x, y - mainWindowROI.y, button);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	if (mainWindowROI.inside((float)x, (float)y))
	{
		kinectProjector->mousePressed(x - mainWindowROI.x, y - mainWindowROI.y, button);
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	kinectProjector->mouseReleased(x - mainWindowROI.x, y - mainWindowROI.y, button);
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

void ofApp::getBinaryImage() {
	std::string BinOutName = "BinaryLandImage.png";
	kinectProjector->getBinaryLandImage(BinaryLandImage);
	ofSaveImage(BinaryLandImage.getPixels(), BinOutName);
}

bool ofApp::ConnectedComponentAnalysis(){
	std::string iThres = "BinaryLandImage.png";
	std::string onameRoiPng = "bin/data/HeightMap_ROI.png";
	//std::string oNoBord = "bin/data/HM_cutout_noBorderBlobs.png";
	std::string oMaxBlob =  "HM_cutout_maxBlob.png";
	cv::Mat M;
	if (BinaryLandImage.width == 0)
	{
		if (!SafeReadBinaryImage(M, iThres))
		{
			ofLogVerbose("CMapGameController") << "ConnectedComponentAnalysis(): could not read BinaryLandImage.png";
			return false;
		}
	}
	else
	{
		M = ofxCv::toCv(BinaryLandImage);
	}
	//SafeReadBinaryImage(M, iThres);
	//
	//int xmin = 0;
	//int ymin = 0;
	//int xmax = 640;
	//int ymax = 480;
	//ROI = cv::Rect(xmin, ymin, xmax - xmin, ymax - ymin);
	//
	//cv::Mat bw = M(ROI);

	RemoveBorderBlobs(M);
	//cv::imwrite("output1.bmp", M);
	std::vector<std::vector<cv::Point> > contours;
	cv::findContours(M, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	if (contours.size() == 0)
	{
		//ofLogVerbose("CMapGameController") << "ConnectedComponentAnalysis(): could not find any contours";
		cout<<"Please put hands off!"<<endl;
		return false;
	}
	int MaxCont = 0;
	int maxID = -1;

	for (int i = 0; i < contours.size(); i++)
	{
		int sz = contours[i].size();
		if (sz > MaxCont)
		{
			MaxCont = sz;
			maxID = i;
		}
	}

	maxBLOBImage = cv::Mat::zeros(M.size(), CV_8UC1);
	drawContours(maxBLOBImage, contours, maxID, 255, CV_FILLED, 8);
	//cv::imwrite(, maxBLOBImage);
	cv::Mat t;
	maxBLOBImage.copyTo(t);  // Do a temporary copy to avoid trouble with ROI definitions and data boundaries

	unsigned char *input = (unsigned char*)(t.data);
	ofxCvGrayscaleImage img2;
	img2.setFromPixels(input, t.cols, t.rows);
	ofSaveImage(img2.getPixels(), oMaxBlob);

	return true;

}


bool ofApp::matchMap() {
	std::string FoundMap = "HM_cutout_maxBlob.png";
	std::string referenceMap = "sss_GT.png";
	std::string referenceImage = "sss.png";
	cv::Mat fMap;
	cv::Mat refMap;
	if (BinaryLandImage.width == 0)
	{
		if (!SafeReadBinaryImage(fMap, FoundMap))
			return false;
	}
	else
	{
		fMap = maxBLOBImage;
	}
	if (!SafeReadBinaryImage(refMap, referenceMap))
		return false;
	
	std::vector<cv::Point2f> AutoLMs;
	ComputeLandmarksFromTemplate(refMap, AutoLMs);//�и�
	//cout << refMap << endl;
	//cv::imwrite("output1.bmp", fMap);
	//cv::imwrite("output2.bmp", refMap);
	double AfMap = cv::sum(fMap)[0] / 255.0;
	double ArefMap = cv::sum(refMap)[0] / 255.0;
	double factor = sqrt(AfMap / ArefMap);
	cv::Mat ScaledI;

	//�Ŵ�
	cv::resize(refMap, ScaledI, cv::Size(), factor, factor, cv::INTER_AREA);
	//cv::imwrite("output1.bmp", ScaledI);

	cv::Mat TMat = cv::Mat::zeros(3, 3, CV_32FC1);
	TMat.at<float>(0, 0) = factor;
	TMat.at<float>(1, 1) = factor;
	TMat.at<float>(2, 2) = 1;
	std::vector<cv::Point2f> LMsScaled(AutoLMs.size());
	cv::perspectiveTransform(AutoLMs, LMsScaled, TMat);

	cv::Mat Itrans = cv::Mat::zeros(ScaledI.rows, ScaledI.cols, ScaledI.type());
	double rCMx = 0;
	double rCMy = 0;
	if (!ReferenceToCM(ScaledI, Itrans, rCMx, rCMy))
		return false;
	//cv::imwrite("output2.bmp", Itrans);

	double tX = ScaledI.cols / 2 - rCMx;
	double tY = ScaledI.rows / 2 - rCMy;
	TMat.at<float>(0, 0) = 1;
	TMat.at<float>(1, 1) = 1;
	TMat.at<float>(0, 2) = tX;
	TMat.at<float>(1, 2) = tY;
	std::vector<cv::Point2f> LMsScaledTrans(AutoLMs.size());
	cv::perspectiveTransform(LMsScaled, LMsScaledTrans, TMat);

	double CMx = 0;
	double CMy = 0;
	ComputeCM(fMap, CMx, CMy);
	double midX = Itrans.cols / 2;
	double midY = Itrans.rows / 2;

	cv::Mat MatchImage = cv::Mat::zeros(fMap.size(), CV_8UC3);
	double DSC = 0;

	double maxDSC = 0;
	double maxAngle = -1000;
	double MinSearchAngle = 180 - 45;
	double MaxSearchAngle = 180 + 45;
	int num = 0;
	for (double angle = MinSearchAngle; angle <= MaxSearchAngle; angle++)
	{
		num++;
		cv::Mat Irot = cv::Mat::zeros(Itrans.rows, Itrans.cols, Itrans.type());

		cv::Point center = cv::Point(Itrans.cols / 2, Itrans.rows / 2);
		double scale = 1.0;
		cv::Mat rotMat = cv::getRotationMatrix2D(center, angle, scale);
		warpAffine(Itrans, Irot, rotMat, Irot.size());
		CompareImages(fMap, Irot, CMx, CMy, MatchImage, DSC);

		//char path[20];
		//sprintf_s(path, "%s%d%s", "test/image", (int)angle, ".bmp");
		//std::string str(path);
		//cv::imwrite(path, Irot);
		if (DSC > maxDSC)
		{
			maxDSC = DSC;
			maxAngle = angle;
		}
		//std::cout << "Angle: " << angle << " Similarity: " << DSC << "%"<<std::endl;
	}
	std::cout << "Max Similarity " << maxDSC << "% At angle " << maxAngle << std::endl;
	shapeGame_Similarity = maxDSC;
	double MinScore = 0.70;
	double MaxScore = 1;
	double finalScore = (maxDSC - MinScore) / (MaxScore - MinScore);
	finalScore = std::max(0.0, finalScore);
	//cout<<"fin: "<<finalScore <<endl;
	Score = finalScore * 1000;
	

	cv::Mat Irot = cv::Mat::zeros(Itrans.rows, Itrans.cols, Itrans.type());

	cv::Point center = cv::Point(Itrans.cols / 2, Itrans.rows / 2);
	double scale = 1.0;
	cv::Mat rotMat = cv::getRotationMatrix2D(center, maxAngle, scale);
	warpAffine(Itrans, Irot, rotMat, Irot.size());

	TMat = cv::Mat::zeros(3, 3, CV_32FC1);
	TMat.at<float>(2, 2) = 1;
	for (int c = 0; c < 3; c++)
	{
		for (int r = 0; r < 2; r++)
		{
			TMat.at<float>(r, c) = rotMat.at<double>(r, c);
		}
	}
	std::vector<cv::Point2f> LMsScaledTransRot(AutoLMs);
	cv::perspectiveTransform(LMsScaledTrans, LMsScaledTransRot, TMat);

	tX = CMx - ScaledI.cols / 2;
	tY = CMy - ScaledI.rows / 2;
	TMat = cv::Mat::zeros(3, 3, CV_32FC1);
	TMat.at<float>(2, 2) = 1;
	TMat.at<float>(0, 0) = 1;
	TMat.at<float>(1, 1) = 1;
	TMat.at<float>(0, 2) = tX;
	TMat.at<float>(1, 2) = tY;
	std::vector<cv::Point2f> LMsScaledTransRotT2(AutoLMs);
	cv::perspectiveTransform(LMsScaledTransRot, LMsScaledTransRotT2, TMat);


	//int xmin = ROI.x;
	//int ymin = ROI.y;
	int xmin = 0;
	int ymin = 0;

	TMat.at<float>(0, 0) = 1;
	TMat.at<float>(1, 1) = 1;
	TMat.at<float>(0, 2) = xmin;
	TMat.at<float>(1, 2) = ymin;
	std::vector<cv::Point2f> LMsinOrgDepthImage(AutoLMs);
	cv::perspectiveTransform(LMsScaledTransRotT2, LMsinOrgDepthImage, TMat);
	return true;

}

bool ofApp::SafeReadBinaryImage(cv::Mat &img, const std::string &fname)
{
	ofImage temp;
	if (!temp.loadImage(fname))
	{
		ofLogVerbose("CMapGameController") << "SafeReadBinaryImage(): could not read " + fname;
		return false;
	}
	cv::Mat t1 = ofxCv::toCv(temp.getPixels());

	cv::Mat t2;
	if (t1.channels() > 1)
		cv::cvtColor(t1, t2, cv::COLOR_BGR2GRAY);
	else
		t1.copyTo(t2);

	cv::normalize(t2, img, 0, 255, cv::NORM_MINMAX);
	return true;
}

void ofApp::ComputeLandmarksFromTemplate(cv::Mat& I, std::vector<cv::Point2f> &LMs) {
	LMs.clear();

	int maxx = 0;
	int minx = I.cols;
	int maxy = 0;
	int miny = I.rows;

	// Find template size
	for (int i = 0; i < I.rows; ++i)
	{
		const uchar *p = I.ptr<uchar>(i);
		for (int j = 0; j < I.cols; ++j)
		{
			if (p[j] > 0)
			{
				if (i > maxy)
					maxy = i;
				if (i < miny)
					miny = i;
				if (j > maxx)
					maxx = j;
				if (j < minx)
					minx = j;
			}
		}
	}

	// Squeeze it 20% not to get extreme points
	double W = maxx - minx;
	double H = maxy - miny;
	maxx = minx + W * 0.80;
	minx = minx + W * 0.20;
	maxy = miny + H * 0.80;
	miny = miny + H * 0.20;

	int nx = 5;
	int ny = 5;

	for (int x = 0; x < nx; x++)
	{
		double LMx = (double)minx + x / (double)(nx - 1) * (double)(maxx - minx);
		for (int y = 0; y < ny; y++)
		{
			double LMy = (double)miny + y / (double)(ny - 1) * (double)(maxy - miny);
			//cout<< LMx <<endl;
			//cout << LMy << endl;
			//cout << endl;
			LMs.push_back(cv::Point2f(LMx, LMy));
		}
	}
}

bool ofApp::ReferenceToCM(cv::Mat& I, cv::Mat& Iout, double &rCMx, double &rCMy)
{
	rCMx = 0;
	rCMy = 0;

	if (!ComputeCM(I, rCMx, rCMy))
	{
		std::cout << "Could not compute CM" << std::endl;
		return false;
	}

	double midX = I.cols / 2;
	double midY = I.rows / 2;

	//	std::cout << "CM: " << rCMx << ", " << rCMy << " mid: " << midX << ", " << midY << std::endl;

	double tX = midX - rCMx;
	double tY = midY - rCMy;

	cv::Mat trans_mat = cv::Mat::zeros(2, 3, CV_32FC1);
	trans_mat.at<float>(0, 0) = 1;
	trans_mat.at<float>(1, 1) = 1;
	trans_mat.at<float>(0, 2) = tX;
	trans_mat.at<float>(1, 2) = tY;

	warpAffine(I, Iout, trans_mat, Iout.size());
	return true;
}

bool ofApp::ComputeCM(const cv::Mat& I, double& CMx, double& CMy)
{
	CMx = 0;
	CMy = 0;
	int N = 0;

	// Compute CM
	for (int i = 0; i < I.rows; ++i)
	{
		const uchar *p = I.ptr<uchar>(i);
		for (int j = 0; j < I.cols; ++j)
		{
			if (p[j] > 0)
			{
				CMx += j;
				CMy += i;
				N++;
			}
		}
	}
	if (N == 0)
		return false;

	CMx /= N;
	CMy /= N;

	return true;
}

void ofApp::CompareImages(cv::Mat& I, cv::Mat& T, int Xi, int Yi, cv::Mat& MatchImage, double &DSC) {
	int dX = (Xi - T.cols / 2);
	int dY = (Yi - T.rows / 2);

	// Run through template

	int match = 0;
	int Inm = 0;

	int Tnm = 0;
	int NI = 0;
	int NT = 0;
	for (int i = 0; i < T.rows; ++i)
	{
		int Ii = i + dY;
		if (Ii >= 0 && Ii < I.rows)
		{
			uchar *pI = I.ptr<uchar>(Ii);
			uchar *pT = T.ptr<uchar>(i);
			for (int j = 0; j < T.cols; ++j)
			{
				int Ij = j + dX;
				if (Ij >= 0 && Ij < I.cols)
				{
					cv::Vec3b &MarkPixel = MatchImage.at<cv::Vec3b>(Ii, Ij);

					bool Ion = pI[Ij] > 0;
					bool Ton = pT[j] > 0;

					if (Ion)
						NI++;
					if (Ton)
						NT++;

					if (Ion && Ton)
					{
						match++;
						MarkPixel = cv::Vec3b(0, 255, 0);
					}

					if (Ion && !Ton)
					{
						Inm++;
						MarkPixel = cv::Vec3b(128, 0, 0);
					}

					if (!Ion && Ton)
					{
						Tnm++;
						MarkPixel = cv::Vec3b(0, 0, 128);
					}

				}
			}
		}
	}
	DSC = 2 * (double)match / (NI + NT);
}

void ofApp::RemoveBorderBlobs(cv::Mat& image)
{
	const uchar white(255);

	// do top and bottom row
	for (int y = 0; y < image.rows; y += image.rows - 1)
	{
		uchar* row = image.ptr<uchar>(y);
		for (int x = 0; x < image.cols; ++x)
		{
			if (row[x] == white)
			{
				cv::floodFill(image, cv::Point(x, y), cv::Scalar(0), (cv::Rect*)0, cv::Scalar(), cv::Scalar(200));
			}
		}
	}
	// fix left and right sides
	for (int y = 0; y < image.rows; ++y)
	{
		uchar* row = image.ptr<uchar>(y);
		for (int x = 0; x < image.cols; x += image.cols - 1)
		{
			if (row[x] == white)
			{
				cv::floodFill(image, cv::Point(x, y), cv::Scalar(0), (cv::Rect*)0, cv::Scalar(), cv::Scalar(200));
			}
		}
	}
}

//bool ofApp::getBinaryLandImage(ofxCvGrayscaleImage& BinImg) {
//	float *imgData = FilteredDepthImage.getFloatPixelsRef().getData();
//	ofVec2f kinectRes;
//	kinectRes.x = 800;
//	kinectRes.y = 800;
//	BinImg.allocate(kinectRes.x,kinectRes.y);
//	unsigned char *binData = BinImg.getPixels().getData();
//	for (int y = 0; y < kinectRes.y; y++)
//	{
//		for (int x = 0; x < kinectRes.x; x++)
//		{
//			int IDX = y * kinectRes.x + x;
//			double val = imgData[IDX];
//
//			//float H = elevationAtKinectCoord(x, y);
//
//			//unsigned char BinOut = 255 * (H > 0);
//
//			binData[IDX] = BinOut;
//		}
//	}
//
//	return true;
//
//}