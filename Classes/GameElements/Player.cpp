#include "Player.h"



Player::Player(int index, int W, int H, int BX, int BY):m_index(index), m_width(W), m_height(H), m_borderX(BX), m_borderY(BY)
{
	length=0;
	score=0;
	m_isAI=false;
	float gameSpeed=CCUserDefault::sharedUserDefault()->getFloatForKey("fGameSpeed", 3.0);
	m_speed=DEFAULT_SPEED*(1+(gameSpeed-3)/3.0f);
	currentSpeed =m_speed;
	

	float turningSpeed=CCUserDefault::sharedUserDefault()->getFloatForKey("fTurningSpeed", 1.0);
	sensitivity=DEFAULT_SENSITIVITY*turningSpeed;

	colors.push_back(ccMEDIUMRED);
	colors.push_back(ccVIOLET);
	colors.push_back(ccBISQUE);
	colors.push_back(ccAQUAMARINE);
	colors.push_back(ccLIGHTPINK);
	colors.push_back(ccLIGHTGREEN);
	colors.push_back(ccMEDIUMBLUE);
	colors.push_back(ccWHITE);
	color=colors[m_index %colors.size()];
	reset(0);

	//private double sensitivity = 0.0031;
	//public int score=0;
	//positionsGrid = new List<Vector2>[20, 20];
	//for (int i = 0; i < 20; i++)
	//   for (int j = 0; j < 20; j++)
	//          {
	//                positionsGrid[i, j] = new List<Vector2>(100);
	//             }

	srand(m_index*4);
	count=rand()%maxCount;

}

void Player::reset(ccTime dt)
{
	m_currentWormLength = 0;
	m_inputValueL=0;
	m_inputValueR=0;

	positions.clear();
	
	//for (int i = 0; i < 20; i++)
	//            for (int j = 0; j < 20; j++)
	//            {
	//                positionsGrid[i, j].Clear();
	//            }
	//        lineList.Clear();

	//        foreach (Powerup p in powerups)
	//        {
	//            if (p.isAlive)
	//                p.Die(this);
	//        }
	//        powerups.Clear();
	currentSpeed = m_speed;	
	m_pos=CCPointMake((float)(m_borderX+1+(rand()%(m_width - 2*m_borderX -2))), (float)(m_borderY+1+(rand()%(m_height - 2*m_borderY -2)))); 
	m_oldPos=m_pos;
	m_angle=((float) atan2((double) (m_pos.y - (m_height / 2)), (double) (m_pos.x - (m_width / 2)))) + PI;
	count = rand()%(maxCount/2);
	m_isAlive=true;
}

void Player::die()
{
	m_isAlive=false;
}

bool Player::hasMoved()
{
	//TODO: neaten up
	if ((((int) m_pos.x) == ((int) m_oldPos.x)) && (((int) m_pos.y) == ((int) m_oldPos.y)))
	{
		return false;
	}
	return true;
}

void Player::move(ccTime dt)
{
	if (currentSpeed < 0)
		return;
	//TODO: why is there a 2.0 here??
	float dist = (dt * currentSpeed* GameState::sharedGameState()->getUIScale());


	if (isPenDown())
	{                
		/*while (dist > MAX_MOVE_DISTANCE)
		{
			m_oldPos=m_pos;
			m_pos.x+=MAX_MOVE_DISTANCE*(float)cos(m_angle);
			m_pos.y+=MAX_MOVE_DISTANCE*(float)sin(m_angle);
			positions.push_back(m_oldPos);
			//if ((oldPosition.X <= WIDTH && oldPosition.X>=0) && (oldPosition.Y <= HEIGHT && oldPosition.Y>=0))
			//    positionsGrid[(int)(20.0f * (oldPosition.X) / (float)WIDTH), (int)(20.0f * (oldPosition.Y) / (float)HEIGHT)].Add(oldPosition);
			//Vector2 p1 = oldPosition;
			//Vector2 p2 = position;
			//lineList.Add(new RoundLine(oldPosition.X, HEIGHT - oldPosition.Y, position.X, HEIGHT - position.Y));
			dist -= MAX_MOVE_DISTANCE;
			length += MAX_MOVE_DISTANCE;
			m_currentWormLength += MAX_MOVE_DISTANCE;

		}*/
		m_oldPos=m_pos;
		m_pos.x+=dist*(float)cos(m_angle);
		m_pos.y+=dist*(float)sin(m_angle);
		positions.push_back(m_oldPos);
		length += dist;
		m_currentWormLength += dist;

		//if ((oldPosition.X <= WIDTH && oldPosition.X >= 0) && (oldPosition.Y <= HEIGHT && oldPosition.Y >= 0))
		//   positionsGrid[(int)(20.0f * (oldPosition.X) / (float)WIDTH), (int)(20.0f * (oldPosition.Y) / (float)HEIGHT)].Add(oldPosition);
		//lineList.Add(new RoundLine(oldPosition.X, HEIGHT - oldPosition.Y, position.X, HEIGHT - position.Y));

	}
	else
	{
		m_oldPos=m_pos;
		m_pos.x+=dist*(float)cos(m_angle);
		m_pos.y+=dist*(float)sin(m_angle);
	}
}

void Player::update(ccTime dt)
{
	if (currentSpeed < 0 || !m_isAlive)
		return;
	count++;
    count = count % maxCount;
    //normalize turning
    m_angle += (dt * (m_inputValueL-m_inputValueR)) * sensitivity * (currentSpeed/DEFAULT_SPEED);
    move(dt);
}