#include "GridBlock.h"


 GridBlock::GridBlock(CCPoint position, float scale)
 {
	 m_position=position;
	 m_scale=scale;
	 m_transition=0.0f;
	 m_blockColor=defaultColor;
	 m_sourceColor=DefaultColor;
	 m_destinationColor=DefaultColor;

            Position = position;
            Scale = scale;
            transition = 0f;
            blockColor = DefaultColor;
            sourceColor = DefaultColor;
            destinationColor =DefaultColor;
            Owner = -1;
        }

        public void FadeOut()
        {
            Owner = -1;
            sourceColor = blockColor;
            destinationColor = DefaultColor;
            transition = 0f;
        }

        public void FadeToPlayer(Player p)
        {
            Color c = p.col;
            if (Owner != p.index)
            {
                if (p.user!=null)
                    p.user.BlocksCovered++;
            }
            Owner = p.index;
            sourceColor = blockColor;
            float num = 0.5f;
            Vector3 vector = (Vector3) ((DefaultColor.ToVector3() * (1f - num)) + (c.ToVector3() * num));
            destinationColor = new Color(vector);
            transition = 0f;
        }

        public void Update(GameTime gameTime)
        {
            transition += (float) gameTime.ElapsedGameTime.TotalMilliseconds;
            float num = Math.Min((float) (transition / transitionTime), (float) 1f);
            Vector3 vector = (Vector3) ((sourceColor.ToVector3() * (1f - num)) + (destinationColor.ToVector3() * num));
            blockColor = new Color(vector);
        }

        public Color BlockColor
        {
            get            {
                return blockColor;
            }
        }

        public bool IsFading
        {
            get
            {
                return (transition < transitionTime);
            }
        }
