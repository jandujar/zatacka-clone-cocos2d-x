#include "GameState.h"

// singleton stuff
static GameState s_sharedGameState;
static bool s_bFirstRun = true;

GameState* GameState::sharedGameState()
{
	if (s_bFirstRun)
	{
		s_sharedGameState.init();
        s_bFirstRun = false;
	}

	return &s_sharedGameState;
}
std::string GameState::fileNameFull(const char* fileName)
{
	std::stringstream ss;
	
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	ss<<GameState::sharedGameState()->getSpriteSize()<<"px_"<<fileName;
#else
    ss<<"scale/"<<GameState::sharedGameState()->getSpriteSize()<<"px_"<<fileName;
#endif
	return ss.str();
}

bool GameState::init()
{
	CCSize size = CCDirector::sharedDirector()->getWinSizeInPixels();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        //if we're in retina mode
    if (fabs(size.width-640)<1)
    {
        m_spriteSize=640;
        m_spriteSetFactor=m_spriteSize/960.0f;	
        //annoying retina mode
        m_uiScale=size.width/960.0f;

    }
    else
    {
        m_spriteSize=320;
        m_spriteSetFactor=m_spriteSize/480.0f;	
        //free of annoying retina mode
        m_uiScale=size.width/480.0f;
    }
	
#else
    //relative to 480x800
    m_uiScale=size.width/480.0f;
    //asset size
	

	//need to find the closest match to native
	 int tempSizes[] = {240, 320, 360, 400, 480, 540, 600, 640, 720, 768};
	vector<int> nativeSpriteSizes (tempSizes, tempSizes + sizeof(tempSizes) / sizeof(int) );
	//initially, best is the first one
	int bestMatch=nativeSpriteSizes[0];
	float bestScaling=-1;

	for (int i=0;i<nativeSpriteSizes.size();i++)
	{
		bestMatch=nativeSpriteSizes[i];
		//always choose the native size that is as close as possible, but always either downscale or run native
		if (size.width<=bestMatch+1)
			break;
	}

	m_spriteSize=bestMatch;
	m_spriteSetFactor=m_spriteSize/480.0f;	
#endif
	m_spriteScale=size.width/m_spriteSize;
	

	return true;
}

void GameState::cacheTextures()
{

	//TODO: this should be dynamically generated
	string line;
	ifstream textureCacheListFile ("textureCacheList.txt");
	if (textureCacheListFile.is_open())
	{
		while ( textureCacheListFile.good() )
		{
			getline (textureCacheListFile,line);

			CCTextureCache::sharedTextureCache()->addImage(GameState::fileNameFull(line.c_str()).c_str());
		}
		textureCacheListFile.close();
	}
}