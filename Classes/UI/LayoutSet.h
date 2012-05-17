#ifndef __LAYOUT_SET_H__
#define __LAYOUT_SET_H__

#include "cocos2d.h"

USING_NS_CC;



class LayoutSet 
{
public:
	/** 
	 * Defines the region occupied by the actual game grid
	 */
	CCRect gridRegion;
	
	/** 
	 * Defines the region occupied by the first player's UI
	 */
	CCRect UIRegionPlayer1;
	
	/** 
	 * Defines the region occupied by the second player's UI
	 */
	CCRect UIRegionPlayer2;
	
	/** 
	 * Defines the region occupied by the first player's left button
	 */	
	CCRect leftButtonPlayer1;
	
	/** 
	 * Defines the region occupied by the second player's left button
	 */
	CCRect leftButtonPlayer2;
	
	/** 
	 * Defines the region occupied by the first player's right button
	 */	
	CCRect rightButtonPlayer1;
	
	/** 
	 * Defines the region occupied by the second player's right button
	 */
	CCRect rightButtonPlayer2;
	
	/** 
	 * Defines the region occupied by the first player's powerup swipe region
	 */
	CCRect powerupSwipeRegionPlayer1;
	
	/** 
	 * Defines the region occupied by the second player's powerup swipe region
	 */		
	CCRect powerupSwipeRegionPlayer2;
	
	/** 
	 * Defines the region occupied by the first player's powerup tap region
	 * Note: this region is used for actual rendering of the powerup. The swipe region is simply used to detect swipes.
	 * By splitting these regions, more swipes are correctly detected
	 */	
	CCRect powerupTapRegionPlayer1;
	
	/** 
	 * Defines the region occupied by the second player's powerup tap region
	 * Note: this region is used for actual rendering of the powerup. The swipe region is simply used to detect swipes.
	 * By splitting these regions, more swipes are correctly detected
	 */	
	CCRect powerupTapRegionPlayer2;
	
	/** 
	 * Defines whether the second player's UI is in fact upside down
	 */
	bool flipUIPlayer2;
	
	/** 
	 * Defines the thickness of the grid's border in the x direction
	 */
	int gridBorderX;
	
	/** 
	 * Defines the thickness of the grid's border in the y direction
	 */
	int gridBorderY;

	/** 
	 * Defines the scale of the layout, relative to the standard (in 3x5: 480x800)
	 */
	float layoutScale;
	
	LayoutSet();
	static LayoutSet LayoutSet3x5(int width);
	static LayoutSet generateLayout(int width, int height, int UIheight, int gridHeight, int buttonWidth, int scrollWidth, int scrollHeight);
};

#endif