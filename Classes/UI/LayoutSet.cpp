#include "LayoutSet.h"

LayoutSet::LayoutSet()
{
	flipUIPlayer2=true;
	gridBorderX=5;
	gridBorderY=5;
}

/**
* Generates layout for a screen with aspect ratio 9:15.
* For example, SGII has a 4.7inch screen with resolution 480x800
* @param width  width of the screen.  
*/
LayoutSet LayoutSet::LayoutSet3x5(int width)
{

	int height=(int)((width/3.0)*5.0);

	double UIfraction=0.2;
	double buttonWidthFraction=0.375;
	double scrollWidthFraction=0.5;
	double scrollHeightFraction=1.0/3.0;
	int UIheight=(int)(height*UIfraction);
	int gridHeight=width;
	int buttonWidth=(int)(width*buttonWidthFraction);
	int scrollWidth=(int)(width*scrollWidthFraction);
	int scrollHeight=(int)(height*scrollHeightFraction);

	LayoutSet generatedLayout=LayoutSet::generateLayout(width, height, UIheight, gridHeight, buttonWidth, scrollWidth, scrollHeight);
	generatedLayout.flipUIPlayer2=true;
	//TODO: perhaps the grid size shouldn't be hard coded? For example, if we double the screen res, should we also double the size of the border?
	generatedLayout.gridBorderX=5;
	generatedLayout.gridBorderY=5;
	generatedLayout.layoutScale=width/480.0f;
	return generatedLayout;				
}

/**
* Generates layout for a screen as constructed by the specific public method.
* @param width  width of the screen.
* @param height  height of the screen.
* @param UIheight  height of the UI.  
* @param gridHeight  height of the grid.  
* @param buttonWidth  width of the left/right buttons.  
* @param scrollWidth  width of the scrollable area of the powerup
* @param scrollHeight  height of the scrollable area of the powerup      
*/
LayoutSet LayoutSet::generateLayout(int width, int height, int UIheight, int gridHeight, int buttonWidth, int scrollWidth, int scrollHeight) 
{	
	LayoutSet generatedLayout;		
	//grid is right in the middle, and it's a square
	generatedLayout.gridRegion=CCRect(0, height-UIheight, width, gridHeight);

	//Player1 UI is on the bottom of the screen
	generatedLayout.UIRegionPlayer1=CCRect(0, UIheight, width, UIheight);
	//Player2 UI is on the top of the screen		
	generatedLayout.UIRegionPlayer2=CCRect(0, height, width, UIheight);

	//Player1 Left button is on the bottom left of the screen
	generatedLayout.leftButtonPlayer1=CCRect(0, UIheight, buttonWidth, UIheight);
	//Player1 Right button is on the bottom right of the screen
	generatedLayout.rightButtonPlayer1=CCRect(width-buttonWidth, UIheight, buttonWidth, UIheight);

	//Player2 Left button is on the top left of the screen
	generatedLayout.leftButtonPlayer2=CCRect(0, height, buttonWidth, UIheight);
	//Player2 Right button is on the top right of the screen
	generatedLayout.rightButtonPlayer2=CCRect(width-buttonWidth, height, buttonWidth, UIheight);

	//Player1 powerup click region is in the middle of the bottom of the screen
	generatedLayout.powerupTapRegionPlayer1=CCRect(buttonWidth, UIheight, width-2*buttonWidth, UIheight);

	//Player2 powerup click region is in the middle of the top of the screen		
	generatedLayout.powerupTapRegionPlayer2=CCRect(buttonWidth, height , width-2*buttonWidth, UIheight);

	//Player1 powerup scroll region is in the middle of the bottom of the screen (expanded)
	generatedLayout.powerupSwipeRegionPlayer1=CCRect(width/2-scrollWidth/2, scrollHeight, scrollWidth, scrollHeight);

	//Player2 powerup scroll region is in the middle of the top of the screen (expanded)		
	generatedLayout.powerupSwipeRegionPlayer2=CCRect(width/2-scrollWidth/2, height, scrollWidth, scrollHeight);

	return generatedLayout;
}