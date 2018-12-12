#include "agk.h"

using namespace AGK;

// slot

Slot2D::Slot2D()
{
	m_pParent = 0;
	m_bFlags = AGK_SLOT_ANIMATE;

	m_pSprite = 0;
	m_iColor = 0xffffffff;

	m_pOrigSprite = 0;
	m_iOrigColor = 0xffffffff;
	m_pOrigBone = 0;
	m_iZOrder = 0;

	x = 0;
	y = 0;
	angle = 0;
	sX = 1;
	sY = 1;

	m_pPrevAnim = 0;
	m_pAnim = 0;

	m_iPrevFrameColor = 0;
	m_iPrevFrameAttachment = 0;
	m_iPrevFrameBone = 0;
	m_iPrevFrameZOrder = 0;
	m_iCurrFrameColor = 0;
	m_iCurrFrameAttachment = 0;
	m_iCurrFrameBone = 0;
	m_iCurrFrameZOrder = 0;

	m_iPrevFrameRotation = 0;
	m_iPrevFramePosition = 0;
	m_iPrevFrameScale = 0;
	m_iCurrFrameRotation = 0;
	m_iCurrFramePosition = 0;
	m_iCurrFrameScale = 0;
}

Slot2D::~Slot2D()
{

}

void Slot2D::Tween( float prevtime, float currtime, float s )
{
	if ( prevtime < 0 ) prevtime = 0;
	if ( currtime < 0 ) currtime = 0;

	unsigned char oldAlpha = m_iOrigColor & 0xff;
	unsigned char oldBlue = (m_iOrigColor >> 8) & 0xff;
	unsigned char oldGreen = (m_iOrigColor >> 16) & 0xff;
	unsigned char oldRed = (m_iOrigColor >> 24) & 0xff;
	cSprite *oldSprite = m_pOrigSprite;
	Bone2D *oldBone = m_pOrigBone;
	int oldZOrder = m_iZOrder;
	float oldX = 0;
	float oldY = 0;
	float oldAngle = 0;
	float oldSX = 1;
	float oldSY = 1;

	int posChanged = 0;
	int angChanged = 0;
	int scaleChanged = 0;
	
	if ( m_pPrevAnim && (m_bFlags & AGK_SLOT_ANIMATE) )
	{
		if ( m_iPrevFrameColor >= m_pPrevAnim->m_iNumColors ) m_iPrevFrameColor = 0;
		if ( m_iPrevFrameAttachment >= m_pPrevAnim->m_iNumAttachments ) m_iPrevFrameAttachment = 0;
		if ( m_iPrevFrameBone >= m_pPrevAnim->m_iNumBones ) m_iPrevFrameBone = 0;
		if ( m_iPrevFrameZOrder >= m_pPrevAnim->m_iNumZOrder ) m_iPrevFrameZOrder = 0;

		// colors
		if ( m_pPrevAnim->m_iNumColors > 0 && m_pPrevAnim->m_pColors[ 0 ]->m_fTime <= prevtime )
		{
			if ( m_pPrevAnim->m_pColors[ m_iPrevFrameColor ]->m_fTime == prevtime || m_pPrevAnim->m_iNumColors == 1 )
			{
				// animation frame exactly matches time we are looking for, no interpolation required
				oldAlpha = m_pPrevAnim->m_pColors[ m_iPrevFrameColor ]->m_iAlpha;
				oldBlue = m_pPrevAnim->m_pColors[ m_iPrevFrameColor ]->m_iBlue;
				oldGreen = m_pPrevAnim->m_pColors[ m_iPrevFrameColor ]->m_iGreen;
				oldRed = m_pPrevAnim->m_pColors[ m_iPrevFrameColor ]->m_iRed;
			}
			else
			{
				if ( m_pPrevAnim->m_pColors[ m_iPrevFrameColor ]->m_fTime < prevtime )
				{
					// time is ahead of our pointer, advance until we find the right frame
					while ( m_iPrevFrameColor < m_pPrevAnim->m_iNumColors-1 
						 && m_pPrevAnim->m_pColors[ m_iPrevFrameColor+1 ]->m_fTime < prevtime ) m_iPrevFrameColor++;
				}
				else
				{
					// time is behind our pointer, go backwards until we find the right frame
					while ( m_iPrevFrameColor > 0
						 && m_pPrevAnim->m_pColors[ m_iPrevFrameColor ]->m_fTime > prevtime ) m_iPrevFrameColor--;
				}

				if ( m_iPrevFrameColor == m_pPrevAnim->m_iNumColors-1 )
				{
					// reached last keyframe and animation hasn't looped yet, must be some dead time for this slot
					oldAlpha = m_pPrevAnim->m_pColors[ m_iPrevFrameColor ]->m_iAlpha;
					oldBlue = m_pPrevAnim->m_pColors[ m_iPrevFrameColor ]->m_iBlue;
					oldGreen = m_pPrevAnim->m_pColors[ m_iPrevFrameColor ]->m_iGreen;
					oldRed = m_pPrevAnim->m_pColors[ m_iPrevFrameColor ]->m_iRed;
				}
				else
				{
					float timeDiff = m_pPrevAnim->m_pColors[ m_iPrevFrameColor+1 ]->m_fTime - m_pPrevAnim->m_pColors[ m_iPrevFrameColor ]->m_fTime;
					if ( timeDiff <= 0 )
					{
						// key frames have the same time, should never happen but account for it anyway
						oldAlpha = m_pPrevAnim->m_pColors[ m_iPrevFrameColor+1 ]->m_iAlpha;
						oldBlue = m_pPrevAnim->m_pColors[ m_iPrevFrameColor+1 ]->m_iBlue;
						oldGreen = m_pPrevAnim->m_pColors[ m_iPrevFrameColor+1 ]->m_iGreen;
						oldRed = m_pPrevAnim->m_pColors[ m_iPrevFrameColor+1 ]->m_iRed;
					}
					else
					{
						// interpolate between these key frames
						float t = (prevtime - m_pPrevAnim->m_pColors[ m_iPrevFrameColor ]->m_fTime) / timeDiff;
						m_pPrevAnim->m_pColors[ m_iPrevFrameColor ]->Interpolate( m_pPrevAnim->m_pColors[ m_iPrevFrameColor+1 ], t, oldRed, oldGreen, oldBlue, oldAlpha );
					}
				}
			}
		}

		// attachment
		if ( m_pPrevAnim->m_iNumAttachments > 0 && m_pPrevAnim->m_pAttachments[ 0 ]->m_fTime <= prevtime )
		{
			if ( m_pPrevAnim->m_pAttachments[ m_iPrevFrameAttachment ]->m_fTime == prevtime || m_pPrevAnim->m_iNumAttachments == 1 )
			{
				// animation frame exactly matches time we are looking for, no interpolation required
				oldSprite = m_pPrevAnim->m_pAttachments[ m_iPrevFrameAttachment ]->m_pSprite;
			}
			else
			{
				if ( m_pPrevAnim->m_pAttachments[ m_iPrevFrameAttachment ]->m_fTime < prevtime )
				{
					// time is ahead of our pointer, advance until we find the right frame
					while ( m_iPrevFrameAttachment < m_pPrevAnim->m_iNumAttachments-1 
						 && m_pPrevAnim->m_pAttachments[ m_iPrevFrameAttachment+1 ]->m_fTime < prevtime ) m_iPrevFrameAttachment++;
				}
				else
				{
					// time is behind our pointer, go backwards until we find the right frame
					while ( m_iPrevFrameAttachment > 0
						 && m_pPrevAnim->m_pAttachments[ m_iPrevFrameAttachment ]->m_fTime > prevtime ) m_iPrevFrameAttachment--;
				}

				// no interpolation for attachments
				oldSprite = m_pPrevAnim->m_pAttachments[ m_iPrevFrameAttachment ]->m_pSprite;
			}
		}

		// bones
		if ( m_pPrevAnim->m_iNumBones > 0 && m_pPrevAnim->m_pBones[ 0 ]->m_fTime <= prevtime )
		{
			if ( m_pPrevAnim->m_pBones[ m_iPrevFrameBone ]->m_fTime == prevtime || m_pPrevAnim->m_iNumBones == 1 )
			{
				// animation frame exactly matches time we are looking for, no interpolation required
				oldBone = m_pPrevAnim->m_pBones[ m_iPrevFrameBone ]->m_pBoneParent;
			}
			else
			{
				if ( m_pPrevAnim->m_pBones[ m_iPrevFrameBone ]->m_fTime < prevtime )
				{
					// time is ahead of our pointer, advance until we find the right frame
					while ( m_iPrevFrameBone < m_pPrevAnim->m_iNumBones-1 
						 && m_pPrevAnim->m_pBones[ m_iPrevFrameBone+1 ]->m_fTime < prevtime ) m_iPrevFrameBone++;
				}
				else
				{
					// time is behind our pointer, go backwards until we find the right frame
					while ( m_iPrevFrameBone > 0
						 && m_pPrevAnim->m_pBones[ m_iPrevFrameBone ]->m_fTime > prevtime ) m_iPrevFrameBone--;
				}

				// no interpolation for boness
				oldBone = m_pPrevAnim->m_pBones[ m_iPrevFrameBone ]->m_pBoneParent;
			}
		}

		// zorder
		if ( m_pPrevAnim->m_iNumZOrder > 0 && m_pPrevAnim->m_pZOrder[ 0 ]->m_fTime <= prevtime )
		{
			if ( m_pPrevAnim->m_pZOrder[ m_iPrevFrameZOrder ]->m_fTime == prevtime || m_pPrevAnim->m_iNumZOrder == 1 )
			{
				// animation frame exactly matches time we are looking for, no interpolation required
				oldZOrder = m_pPrevAnim->m_pZOrder[ m_iPrevFrameZOrder ]->m_iZOrder;
			}
			else
			{
				if ( m_pPrevAnim->m_pZOrder[ m_iPrevFrameZOrder ]->m_fTime < prevtime )
				{
					// time is ahead of our pointer, advance until we find the right frame
					while ( m_iPrevFrameZOrder < m_pPrevAnim->m_iNumZOrder-1 
						 && m_pPrevAnim->m_pZOrder[ m_iPrevFrameZOrder+1 ]->m_fTime < prevtime ) m_iPrevFrameZOrder++;
				}
				else
				{
					// time is behind our pointer, go backwards until we find the right frame
					while ( m_iPrevFrameZOrder > 0
						 && m_pPrevAnim->m_pZOrder[ m_iPrevFrameZOrder ]->m_fTime > prevtime ) m_iPrevFrameZOrder--;
				}

				// no interpolation for zorder
				oldZOrder = m_pPrevAnim->m_pZOrder[ m_iPrevFrameZOrder ]->m_iZOrder;
			}
		}

		if ( m_iPrevFramePosition >= m_pPrevAnim->m_iNumPositions ) m_iPrevFramePosition = 0;
		if ( m_iPrevFrameRotation >= m_pPrevAnim->m_iNumRotations ) m_iPrevFrameRotation = 0;
		if ( m_iPrevFrameScale >= m_pPrevAnim->m_iNumScales ) m_iPrevFrameScale = 0;

		// positions
		if ( m_pPrevAnim->m_iNumPositions > 0 && m_pPrevAnim->m_pPositions[ 0 ]->m_fTime <= prevtime )
		{
			posChanged = 1;
			if ( m_pPrevAnim->m_pPositions[ m_iPrevFramePosition ]->m_fTime == prevtime || m_pPrevAnim->m_iNumPositions == 1 )
			{
				// animation frame exactly matches time we are looking for, no interpolation required
				oldX = m_pPrevAnim->m_pPositions[ m_iPrevFramePosition ]->m_fX;
				oldY = m_pPrevAnim->m_pPositions[ m_iPrevFramePosition ]->m_fY;
			}
			else
			{
				if ( m_pPrevAnim->m_pPositions[ m_iPrevFramePosition ]->m_fTime < prevtime )
				{
					// time is ahead of our pointer, advance until we find the right frame
					while ( m_iPrevFramePosition < m_pPrevAnim->m_iNumPositions-1 
						 && m_pPrevAnim->m_pPositions[ m_iPrevFramePosition+1 ]->m_fTime < prevtime ) m_iPrevFramePosition++;
				}
				else
				{
					// time is behind our pointer, go backwards until we find the right frame
					while ( m_iPrevFramePosition > 0
						 && m_pPrevAnim->m_pPositions[ m_iPrevFramePosition ]->m_fTime > prevtime ) m_iPrevFramePosition--;
				}

				if ( m_iPrevFramePosition == m_pPrevAnim->m_iNumPositions-1 )
				{
					// reached last keyframe and animation hasn't looped yet, must be some dead time for this bone
					oldX = m_pPrevAnim->m_pPositions[ m_iPrevFramePosition ]->m_fX;
					oldY = m_pPrevAnim->m_pPositions[ m_iPrevFramePosition ]->m_fY;
				}
				else
				{
					float timeDiff = m_pPrevAnim->m_pPositions[ m_iPrevFramePosition+1 ]->m_fTime - m_pPrevAnim->m_pPositions[ m_iPrevFramePosition ]->m_fTime;
					if ( timeDiff <= 0 )
					{
						// key frames have the same time, should never happen but account for it anyway
						oldX = m_pPrevAnim->m_pPositions[ m_iPrevFramePosition+1 ]->m_fX;
						oldY = m_pPrevAnim->m_pPositions[ m_iPrevFramePosition+1 ]->m_fY;
					}
					else
					{
						// interpolate between these key frames
						float t = (prevtime - m_pPrevAnim->m_pPositions[ m_iPrevFramePosition ]->m_fTime) / timeDiff;
						m_pPrevAnim->m_pPositions[ m_iPrevFramePosition ]->Interpolate( m_pPrevAnim->m_pPositions[ m_iPrevFramePosition+1 ], t, oldX, oldY );
					}
				}
			}
		}

		// rotation
		if ( m_pPrevAnim->m_iNumRotations > 0 && m_pPrevAnim->m_pRotations[ 0 ]->m_fTime <= prevtime )
		{
			angChanged = 1;
			if ( m_pPrevAnim->m_pRotations[ m_iPrevFrameRotation ]->m_fTime == prevtime || m_pPrevAnim->m_iNumRotations == 1 )
			{
				// animation frame exactly matches time we are looking for, no interpolation required
				oldAngle = m_pPrevAnim->m_pRotations[ m_iPrevFrameRotation ]->m_fRotation;
			}
			else
			{
				if ( m_pPrevAnim->m_pRotations[ m_iPrevFrameRotation ]->m_fTime < prevtime )
				{
					// time is ahead of our pointer, advance until we find the right frame
					while ( m_iPrevFrameRotation < m_pPrevAnim->m_iNumRotations-1 
						 && m_pPrevAnim->m_pRotations[ m_iPrevFrameRotation+1 ]->m_fTime < prevtime ) m_iPrevFrameRotation++;
				}
				else
				{
					// time is behind our pointer, go backwards until we find the right frame
					while ( m_iPrevFrameRotation > 0
						 && m_pPrevAnim->m_pRotations[ m_iPrevFrameRotation ]->m_fTime > prevtime ) m_iPrevFrameRotation--;
				}

				if ( m_iPrevFrameRotation == m_pPrevAnim->m_iNumRotations-1 )
				{
					// reached last keyframe and animation hasn't looped yet, must be some dead time for this bone
					oldAngle = m_pPrevAnim->m_pRotations[ m_iPrevFrameRotation ]->m_fRotation;
				}
				else
				{
					float timeDiff = m_pPrevAnim->m_pRotations[ m_iPrevFrameRotation+1 ]->m_fTime - m_pPrevAnim->m_pRotations[ m_iPrevFrameRotation ]->m_fTime;
					if ( timeDiff <= 0 )
					{
						// key frames have the same time, should never happen but account for it anyway
						oldAngle = m_pPrevAnim->m_pRotations[ m_iPrevFrameRotation+1 ]->m_fRotation;
					}
					else
					{
						// interpolate between these key frames
						float t = (prevtime - m_pPrevAnim->m_pRotations[ m_iPrevFrameRotation ]->m_fTime) / timeDiff;
						m_pPrevAnim->m_pRotations[ m_iPrevFrameRotation ]->Interpolate( m_pPrevAnim->m_pRotations[ m_iPrevFrameRotation+1 ], t, oldAngle );
					}
				}
			}
		}

		// scales
		if ( m_pPrevAnim->m_iNumScales > 0 && m_pPrevAnim->m_pScales[ 0 ]->m_fTime <= prevtime )
		{
			scaleChanged = 1;
			if ( m_pPrevAnim->m_pScales[ m_iPrevFrameScale ]->m_fTime == prevtime || m_pPrevAnim->m_iNumScales == 1 )
			{
				// animation frame exactly matches time we are looking for, no interpolation required
				oldSX = m_pPrevAnim->m_pScales[ m_iPrevFrameScale ]->m_fScaleX;
				oldSY = m_pPrevAnim->m_pScales[ m_iPrevFrameScale ]->m_fScaleY;
			}
			else
			{
				if ( m_pPrevAnim->m_pScales[ m_iPrevFrameScale ]->m_fTime < prevtime )
				{
					// time is ahead of our pointer, advance until we find the right frame
					while ( m_iPrevFrameScale < m_pPrevAnim->m_iNumScales-1 
						 && m_pPrevAnim->m_pScales[ m_iPrevFrameScale+1 ]->m_fTime < prevtime ) m_iPrevFrameScale++;
				}
				else
				{
					// time is behind our pointer, go backwards until we find the right frame
					while ( m_iPrevFrameScale > 0
						 && m_pPrevAnim->m_pScales[ m_iPrevFrameScale ]->m_fTime > prevtime ) m_iPrevFrameScale--;
				}

				if ( m_iPrevFrameScale == m_pPrevAnim->m_iNumScales-1 )
				{
					// reached last keyframe and animation hasn't looped yet, must be some dead time for this bone
					oldSX = m_pPrevAnim->m_pScales[ m_iPrevFrameScale ]->m_fScaleX;
					oldSY = m_pPrevAnim->m_pScales[ m_iPrevFrameScale ]->m_fScaleY;
				}
				else
				{
					float timeDiff = m_pPrevAnim->m_pScales[ m_iPrevFrameScale+1 ]->m_fTime - m_pPrevAnim->m_pScales[ m_iPrevFrameScale ]->m_fTime;
					if ( timeDiff <= 0 )
					{
						// key frames have the same time, should never happen but account for it anyway
						oldSX = m_pPrevAnim->m_pScales[ m_iPrevFrameScale+1 ]->m_fScaleX;
						oldSY = m_pPrevAnim->m_pScales[ m_iPrevFrameScale+1 ]->m_fScaleY;
					}
					else
					{
						// interpolate between these key frames
						float t = (prevtime - m_pPrevAnim->m_pScales[ m_iPrevFrameScale ]->m_fTime) / timeDiff;
						m_pPrevAnim->m_pScales[ m_iPrevFrameScale ]->Interpolate( m_pPrevAnim->m_pScales[ m_iPrevFrameScale+1 ], t, oldSX, oldSY );
					}
				}
			}
		}
	}

	// now the new transform
	unsigned char newAlpha = m_iOrigColor & 0xff;
	unsigned char newBlue = (m_iOrigColor >> 8) & 0xff;
	unsigned char newGreen = (m_iOrigColor >> 16) & 0xff;
	unsigned char newRed = (m_iOrigColor >> 24) & 0xff;
//	cSprite *newSprite = m_pOrigSprite;

	float newX = 0;
	float newY = 0;
	float newAngle = 0;
	float newSX = 1;
	float newSY = 1;

	int newPosChanged = 0;
	int newAngChanged = 0;
	int newScaleChanged = 0;

	if ( m_pAnim && (m_bFlags & AGK_SLOT_ANIMATE) )
	{
		if ( m_iCurrFrameColor >= m_pAnim->m_iNumColors ) m_iCurrFrameColor = 0;
		if ( m_iCurrFrameAttachment >= m_pAnim->m_iNumAttachments ) m_iCurrFrameAttachment = 0;
		if ( m_iCurrFrameBone >= m_pAnim->m_iNumBones ) m_iCurrFrameBone = 0;
		if ( m_iCurrFrameZOrder >= m_pAnim->m_iNumZOrder ) m_iCurrFrameZOrder = 0;

		// colors
		if ( m_pAnim->m_iNumColors > 0 && m_pAnim->m_pColors[ 0 ]->m_fTime <= currtime )
		{
			if ( m_pAnim->m_pColors[ m_iCurrFrameColor ]->m_fTime == currtime || m_pAnim->m_iNumColors == 1 )
			{
				// animation frame exactly matches time we are looking for, no interpolation required
				newAlpha = m_pAnim->m_pColors[ m_iCurrFrameColor ]->m_iAlpha;
				newBlue = m_pAnim->m_pColors[ m_iCurrFrameColor ]->m_iBlue;
				newGreen = m_pAnim->m_pColors[ m_iCurrFrameColor ]->m_iGreen;
				newRed = m_pAnim->m_pColors[ m_iCurrFrameColor ]->m_iRed;
			}
			else
			{
				if ( m_pAnim->m_pColors[ m_iCurrFrameColor ]->m_fTime < currtime )
				{
					// time is ahead of our pointer, advance until we find the right frame
					while ( m_iCurrFrameColor < m_pAnim->m_iNumColors-1 
						 && m_pAnim->m_pColors[ m_iCurrFrameColor+1 ]->m_fTime < currtime ) m_iCurrFrameColor++;
				}
				else
				{
					// time is behind our pointer, go backwards until we find the right frame
					while ( m_iCurrFrameColor > 0
						 && m_pAnim->m_pColors[ m_iCurrFrameColor ]->m_fTime > currtime ) m_iCurrFrameColor--;
				}

				if ( m_iCurrFrameColor == m_pAnim->m_iNumColors-1 )
				{
					// reached last keyframe and animation hasn't looped yet, must be some dead time for this slot
					newAlpha = m_pAnim->m_pColors[ m_iCurrFrameColor ]->m_iAlpha;
					newBlue = m_pAnim->m_pColors[ m_iCurrFrameColor ]->m_iBlue;
					newGreen = m_pAnim->m_pColors[ m_iCurrFrameColor ]->m_iGreen;
					newRed = m_pAnim->m_pColors[ m_iCurrFrameColor ]->m_iRed;
				}
				else
				{
					float timeDiff = m_pAnim->m_pColors[ m_iCurrFrameColor+1 ]->m_fTime - m_pAnim->m_pColors[ m_iCurrFrameColor ]->m_fTime;
					if ( timeDiff <= 0 )
					{
						// key frames have the same time, should never happen but account for it anyway
						newAlpha = m_pAnim->m_pColors[ m_iCurrFrameColor+1 ]->m_iAlpha;
						newBlue = m_pAnim->m_pColors[ m_iCurrFrameColor+1 ]->m_iBlue;
						newGreen = m_pAnim->m_pColors[ m_iCurrFrameColor+1 ]->m_iGreen;
						newRed = m_pAnim->m_pColors[ m_iCurrFrameColor+1 ]->m_iRed;
					}
					else
					{
						// interpolate between these key frames
						float t = (currtime - m_pAnim->m_pColors[ m_iCurrFrameColor ]->m_fTime) / timeDiff;
						m_pAnim->m_pColors[ m_iCurrFrameColor ]->Interpolate( m_pAnim->m_pColors[ m_iCurrFrameColor+1 ], t, newRed, newGreen, newBlue, newAlpha );
					}
				}
			}
		}

		// no attachment tweening
		// no bone tweening

		if ( m_iCurrFramePosition >= m_pAnim->m_iNumPositions ) m_iCurrFramePosition = 0;
		if ( m_iCurrFrameRotation >= m_pAnim->m_iNumRotations ) m_iCurrFrameRotation = 0;
		if ( m_iCurrFrameScale >= m_pAnim->m_iNumScales ) m_iCurrFrameScale = 0;

		// positions
		if ( m_pAnim->m_iNumPositions > 0 && m_pAnim->m_pPositions[ 0 ]->m_fTime <= currtime )
		{
			newPosChanged = 1;
			if ( m_pAnim->m_pPositions[ m_iCurrFramePosition ]->m_fTime == currtime || m_pAnim->m_iNumPositions == 1 )
			{
				// animation frame exactly matches time we are looking for, no interpolation required
				newX = m_pAnim->m_pPositions[ m_iCurrFramePosition ]->m_fX;
				newY = m_pAnim->m_pPositions[ m_iCurrFramePosition ]->m_fY;
			}
			else
			{
				if ( m_pAnim->m_pPositions[ m_iCurrFramePosition ]->m_fTime < currtime )
				{
					// time is ahead of our pointer, advance until we find the right frame
					while ( m_iCurrFramePosition < m_pAnim->m_iNumPositions-1 
						 && m_pAnim->m_pPositions[ m_iCurrFramePosition+1 ]->m_fTime < currtime ) m_iCurrFramePosition++;
				}
				else
				{
					// time is behind our pointer, go backwards until we find the right frame
					while ( m_iCurrFramePosition > 0
						 && m_pAnim->m_pPositions[ m_iCurrFramePosition ]->m_fTime > currtime ) m_iCurrFramePosition--;
				}

				if ( m_iCurrFramePosition == m_pAnim->m_iNumPositions-1 )
				{
					// reached last keyframe and animation hasn't looped yet, must be some dead time for this bone
					newX = m_pAnim->m_pPositions[ m_iCurrFramePosition ]->m_fX;
					newY = m_pAnim->m_pPositions[ m_iCurrFramePosition ]->m_fY;
				}
				else
				{
					float timeDiff = m_pAnim->m_pPositions[ m_iCurrFramePosition+1 ]->m_fTime - m_pAnim->m_pPositions[ m_iCurrFramePosition ]->m_fTime;
					if ( timeDiff <= 0 )
					{
						// key frames have the same time, should never happen but account for it anyway
						newX = m_pAnim->m_pPositions[ m_iCurrFramePosition+1 ]->m_fX;
						newY = m_pAnim->m_pPositions[ m_iCurrFramePosition+1 ]->m_fY;
					}
					else
					{
						// interpolate between these key frames
						float t = (currtime - m_pAnim->m_pPositions[ m_iCurrFramePosition ]->m_fTime) / timeDiff;
						m_pAnim->m_pPositions[ m_iCurrFramePosition ]->Interpolate( m_pAnim->m_pPositions[ m_iCurrFramePosition+1 ], t, newX, newY );
					}
				}
			}
		}

		// rotation
		if ( m_pAnim->m_iNumRotations > 0 && m_pAnim->m_pRotations[ 0 ]->m_fTime <= currtime )
		{
			newAngChanged = 1;
			if ( m_pAnim->m_pRotations[ m_iCurrFrameRotation ]->m_fTime == currtime || m_pAnim->m_iNumRotations == 1 )
			{
				// animation frame exactly matches time we are looking for, no interpolation required
				newAngle = m_pAnim->m_pRotations[ m_iCurrFrameRotation ]->m_fRotation;
			}
			else
			{
				if ( m_pAnim->m_pRotations[ m_iCurrFrameRotation ]->m_fTime < currtime )
				{
					// time is ahead of our pointer, advance until we find the right frame
					while ( m_iCurrFrameRotation < m_pAnim->m_iNumRotations-1 
						 && m_pAnim->m_pRotations[ m_iCurrFrameRotation+1 ]->m_fTime < currtime ) m_iCurrFrameRotation++;
				}
				else
				{
					// time is behind our pointer, go backwards until we find the right frame
					while ( m_iCurrFrameRotation > 0
						 && m_pAnim->m_pRotations[ m_iCurrFrameRotation ]->m_fTime > currtime ) m_iCurrFrameRotation--;
				}

				if ( m_iCurrFrameRotation == m_pAnim->m_iNumRotations-1 )
				{
					// reached last keyframe and animation hasn't looped yet, must be some dead time for this bone
					newAngle = m_pAnim->m_pRotations[ m_iCurrFrameRotation ]->m_fRotation;
				}
				else
				{
					float timeDiff = m_pAnim->m_pRotations[ m_iCurrFrameRotation+1 ]->m_fTime - m_pAnim->m_pRotations[ m_iCurrFrameRotation ]->m_fTime;
					if ( timeDiff <= 0 )
					{
						// key frames have the same time, should never happen but account for it anyway
						newAngle = m_pAnim->m_pRotations[ m_iCurrFrameRotation+1 ]->m_fRotation;
					}
					else
					{
						// interpolate between these key frames
						float t = (currtime - m_pAnim->m_pRotations[ m_iCurrFrameRotation ]->m_fTime) / timeDiff;
						m_pAnim->m_pRotations[ m_iCurrFrameRotation ]->Interpolate( m_pAnim->m_pRotations[ m_iCurrFrameRotation+1 ], t, newAngle );
					}
				}
			}
		}

		// scales
		if ( m_pAnim->m_iNumScales > 0 && m_pAnim->m_pScales[ 0 ]->m_fTime <= currtime )
		{
			newScaleChanged = 1;
			if ( m_pAnim->m_pScales[ m_iCurrFrameScale ]->m_fTime == currtime || m_pAnim->m_iNumScales == 1 )
			{
				// animation frame exactly matches time we are looking for, no interpolation required
				newSX = m_pAnim->m_pScales[ m_iCurrFrameScale ]->m_fScaleX;
				newSY = m_pAnim->m_pScales[ m_iCurrFrameScale ]->m_fScaleY;
			}
			else
			{
				if ( m_pAnim->m_pScales[ m_iCurrFrameScale ]->m_fTime < currtime )
				{
					// time is ahead of our pointer, advance until we find the right frame
					while ( m_iCurrFrameScale < m_pAnim->m_iNumScales-1 
						 && m_pAnim->m_pScales[ m_iCurrFrameScale+1 ]->m_fTime < currtime ) m_iCurrFrameScale++;
				}
				else
				{
					// time is behind our pointer, go backwards until we find the right frame
					while ( m_iCurrFrameScale > 0
						 && m_pAnim->m_pScales[ m_iCurrFrameScale ]->m_fTime > currtime ) m_iCurrFrameScale--;
				}

				if ( m_iCurrFrameScale == m_pAnim->m_iNumScales-1 )
				{
					// reached last keyframe and animation hasn't looped yet, must be some dead time for this bone
					newSX = m_pAnim->m_pScales[ m_iCurrFrameScale ]->m_fScaleX;
					newSY = m_pAnim->m_pScales[ m_iCurrFrameScale ]->m_fScaleY;
				}
				else
				{
					float timeDiff = m_pAnim->m_pScales[ m_iCurrFrameScale+1 ]->m_fTime - m_pAnim->m_pScales[ m_iCurrFrameScale ]->m_fTime;
					if ( timeDiff <= 0 )
					{
						// key frames have the same time, should never happen but account for it anyway
						newSX = m_pAnim->m_pScales[ m_iCurrFrameScale+1 ]->m_fScaleX;
						newSY = m_pAnim->m_pScales[ m_iCurrFrameScale+1 ]->m_fScaleY;
					}
					else
					{
						// interpolate between these key frames
						float t = (currtime - m_pAnim->m_pScales[ m_iCurrFrameScale ]->m_fTime) / timeDiff;
						m_pAnim->m_pScales[ m_iCurrFrameScale ]->Interpolate( m_pAnim->m_pScales[ m_iCurrFrameScale+1 ], t, newSX, newSY );
					}
				}
			}
		}
	}

	UINT red = agk::Round( oldRed + (newRed-oldRed)*s );
	UINT green = agk::Round( oldGreen + (newGreen-oldGreen)*s );
	UINT blue = agk::Round( oldBlue + (newBlue-oldBlue)*s );
	UINT alpha = agk::Round( oldAlpha + (newAlpha-oldAlpha)*s );
	m_iColor = red;
	m_iColor = (m_iColor << 8) | green;
	m_iColor = (m_iColor << 8) | blue;
	m_iColor = (m_iColor << 8) | alpha;

	m_pParent = oldBone;
	m_pSprite = oldSprite;
	m_iZOrder = oldZOrder;

	if ( posChanged && newPosChanged )
	{
		x = oldX + (newX-oldX)*s;
		y = oldY + (newY-oldY)*s;
	}
	else if ( posChanged )
	{
		x = oldX;
		y = oldY;
	}
	else if ( newPosChanged )
	{
		x = newX;
		y = newY;
	}

	if ( angChanged && newAngChanged )
	{
		if ( oldAngle < newAngle )
		{
			while ( newAngle - oldAngle > 180 ) oldAngle += 360;
		}
		else
		{
			while ( oldAngle - newAngle > 180 ) newAngle += 360;
		}
	
		angle = oldAngle + (newAngle-oldAngle)*s;
	}
	else if ( angChanged ) angle = oldAngle;
	else if ( newAngChanged ) angle = newAngle;

	if ( scaleChanged && newScaleChanged )
	{
		sX = oldSX + (newSX-oldSX)*s;
		sY = oldSY + (newSY-oldSY)*s;
	}
	else if ( scaleChanged )
	{
		sX = oldSX;
		sY = oldSY;
	}
	else if ( newScaleChanged )
	{
		sX = newSX;
		sY = newSY;
	}
	
	if ( m_pSprite ) 
	{
		m_pSprite->SetColor( red, green, blue, alpha );
		m_pSprite->SetVisible( true );
		m_pSprite->SetBone( m_pParent );
		if ( posChanged || newPosChanged ) m_pSprite->SetPositionByOffset( x, y );
		if ( angChanged || newAngChanged ) m_pSprite->SetAngle( angle );
		if ( scaleChanged || newScaleChanged ) m_pSprite->SetScaleByOffset( sX, sY );
	}
}

void Slot2D::Interpolate( float currtime )
{
	unsigned char newAlpha = m_iOrigColor & 0xff;
	unsigned char newBlue = (m_iOrigColor >> 8) & 0xff;
	unsigned char newGreen = (m_iOrigColor >> 16) & 0xff;
	unsigned char newRed = (m_iOrigColor >> 24) & 0xff;
	cSprite *newSprite = m_pOrigSprite;
	Bone2D *newBone = m_pOrigBone;
	int newZOrder = m_iZOrder;
	float newX = 0;
	float newY = 0;
	float newAngle = 0;
	float newSX = 1;
	float newSY = 1;

	int posChanged = 0;
	int angChanged = 0;
	int scaleChanged = 0;

	if ( m_pAnim && (m_bFlags & AGK_SLOT_ANIMATE) )
	{
		if ( m_iCurrFrameColor >= m_pAnim->m_iNumColors ) m_iCurrFrameColor = 0;
		if ( m_iCurrFrameAttachment >= m_pAnim->m_iNumAttachments ) m_iCurrFrameAttachment = 0;
		if ( m_iCurrFrameBone >= m_pAnim->m_iNumBones ) m_iCurrFrameBone = 0;
		if ( m_iCurrFrameZOrder >= m_pAnim->m_iNumZOrder ) m_iCurrFrameZOrder = 0;

		// colors
		if ( m_pAnim->m_iNumColors > 0 && m_pAnim->m_pColors[ 0 ]->m_fTime <= currtime )
		{
			if ( m_pAnim->m_pColors[ m_iCurrFrameColor ]->m_fTime == currtime || m_pAnim->m_iNumColors == 1 )
			{
				// animation frame exactly matches time we are looking for, no interpolation required
				newAlpha = m_pAnim->m_pColors[ m_iCurrFrameColor ]->m_iAlpha;
				newBlue = m_pAnim->m_pColors[ m_iCurrFrameColor ]->m_iBlue;
				newGreen = m_pAnim->m_pColors[ m_iCurrFrameColor ]->m_iGreen;
				newRed = m_pAnim->m_pColors[ m_iCurrFrameColor ]->m_iRed;
			}
			else
			{
				if ( m_pAnim->m_pColors[ m_iCurrFrameColor ]->m_fTime < currtime )
				{
					// time is ahead of our pointer, advance until we find the right frame
					while ( m_iCurrFrameColor < m_pAnim->m_iNumColors-1 
						 && m_pAnim->m_pColors[ m_iCurrFrameColor+1 ]->m_fTime < currtime ) m_iCurrFrameColor++;
				}
				else
				{
					// time is behind our pointer, go backwards until we find the right frame
					while ( m_iCurrFrameColor > 0
						 && m_pAnim->m_pColors[ m_iCurrFrameColor ]->m_fTime > currtime ) m_iCurrFrameColor--;
				}

				if ( m_iCurrFrameColor == m_pAnim->m_iNumColors-1 )
				{
					// reached last keyframe and animation hasn't looped yet, must be some dead time for this slot
					newAlpha = m_pAnim->m_pColors[ m_iCurrFrameColor ]->m_iAlpha;
					newBlue = m_pAnim->m_pColors[ m_iCurrFrameColor ]->m_iBlue;
					newGreen = m_pAnim->m_pColors[ m_iCurrFrameColor ]->m_iGreen;
					newRed = m_pAnim->m_pColors[ m_iCurrFrameColor ]->m_iRed;
				}
				else
				{
					float timeDiff = m_pAnim->m_pColors[ m_iCurrFrameColor+1 ]->m_fTime - m_pAnim->m_pColors[ m_iCurrFrameColor ]->m_fTime;
					if ( timeDiff <= 0 )
					{
						// key frames have the same time, should never happen but account for it anyway
						newAlpha = m_pAnim->m_pColors[ m_iCurrFrameColor+1 ]->m_iAlpha;
						newBlue = m_pAnim->m_pColors[ m_iCurrFrameColor+1 ]->m_iBlue;
						newGreen = m_pAnim->m_pColors[ m_iCurrFrameColor+1 ]->m_iGreen;
						newRed = m_pAnim->m_pColors[ m_iCurrFrameColor+1 ]->m_iRed;
					}
					else
					{
						// interpolate between these key frames
						float t = (currtime - m_pAnim->m_pColors[ m_iCurrFrameColor ]->m_fTime) / timeDiff;
						m_pAnim->m_pColors[ m_iCurrFrameColor ]->Interpolate( m_pAnim->m_pColors[ m_iCurrFrameColor+1 ], t, newRed, newGreen, newBlue, newAlpha );
					}
				}
			}
		}

		// attachment
		if ( m_pAnim->m_iNumAttachments > 0 && m_pAnim->m_pAttachments[ 0 ]->m_fTime <= currtime )
		{
			if ( m_pAnim->m_pAttachments[ m_iCurrFrameAttachment ]->m_fTime == currtime || m_pAnim->m_iNumAttachments == 1 )
			{
				// animation frame exactly matches time we are looking for, no interpolation required
				newSprite = m_pAnim->m_pAttachments[ m_iCurrFrameAttachment ]->m_pSprite;
			}
			else
			{
				if ( m_pAnim->m_pAttachments[ m_iCurrFrameAttachment ]->m_fTime < currtime )
				{
					// time is ahead of our pointer, advance until we find the right frame
					while ( m_iCurrFrameAttachment < m_pAnim->m_iNumAttachments-1 
						 && m_pAnim->m_pAttachments[ m_iCurrFrameAttachment+1 ]->m_fTime < currtime ) m_iCurrFrameAttachment++;
				}
				else
				{
					// time is behind our pointer, go backwards until we find the right frame
					while ( m_iCurrFrameAttachment > 0
						 && m_pAnim->m_pAttachments[ m_iCurrFrameAttachment ]->m_fTime > currtime ) m_iCurrFrameAttachment--;
				}

				// no interpolation for attachments
				newSprite = m_pAnim->m_pAttachments[ m_iCurrFrameAttachment ]->m_pSprite;
			}
		}

		// bone
		if ( m_pAnim->m_iNumBones > 0 && m_pAnim->m_pBones[ 0 ]->m_fTime <= currtime )
		{
			if ( m_pAnim->m_pBones[ m_iCurrFrameBone ]->m_fTime == currtime || m_pAnim->m_iNumBones == 1 )
			{
				// animation frame exactly matches time we are looking for, no interpolation required
				newBone = m_pAnim->m_pBones[ m_iCurrFrameBone ]->m_pBoneParent;
			}
			else
			{
				if ( m_pAnim->m_pBones[ m_iCurrFrameBone ]->m_fTime < currtime )
				{
					// time is ahead of our pointer, advance until we find the right frame
					while ( m_iCurrFrameBone < m_pAnim->m_iNumBones-1 
						 && m_pAnim->m_pBones[ m_iCurrFrameBone+1 ]->m_fTime < currtime ) m_iCurrFrameBone++;
				}
				else
				{
					// time is behind our pointer, go backwards until we find the right frame
					while ( m_iCurrFrameBone > 0
						 && m_pAnim->m_pBones[ m_iCurrFrameBone ]->m_fTime > currtime ) m_iCurrFrameBone--;
				}

				// no interpolation for bones
				newBone = m_pAnim->m_pBones[ m_iCurrFrameBone ]->m_pBoneParent;
			}
		}
		
		// zorder
		if ( m_pAnim->m_iNumZOrder > 0 && m_pAnim->m_pZOrder[ 0 ]->m_fTime <= currtime )
		{
			if ( m_pAnim->m_pZOrder[ m_iCurrFrameZOrder ]->m_fTime == currtime || m_pAnim->m_iNumZOrder == 1 )
			{
				// animation frame exactly matches time we are looking for, no interpolation required
				newZOrder = m_pAnim->m_pZOrder[ m_iCurrFrameZOrder ]->m_iZOrder;
			}
			else
			{
				if ( m_pAnim->m_pZOrder[ m_iCurrFrameZOrder ]->m_fTime < currtime )
				{
					// time is ahead of our pointer, advance until we find the right frame
					while ( m_iCurrFrameZOrder < m_pAnim->m_iNumZOrder-1 
						 && m_pAnim->m_pZOrder[ m_iCurrFrameZOrder+1 ]->m_fTime < currtime ) m_iCurrFrameZOrder++;
				}
				else
				{
					// time is behind our pointer, go backwards until we find the right frame
					while ( m_iCurrFrameZOrder > 0
						 && m_pAnim->m_pZOrder[ m_iCurrFrameZOrder ]->m_fTime > currtime ) m_iCurrFrameZOrder--;
				}

				// no interpolation for zorder
				newZOrder = m_pAnim->m_pZOrder[ m_iCurrFrameZOrder ]->m_iZOrder;
			}
		}

		if ( m_iCurrFramePosition >= m_pAnim->m_iNumPositions ) m_iCurrFramePosition = 0;
		if ( m_iCurrFrameRotation >= m_pAnim->m_iNumRotations ) m_iCurrFrameRotation = 0;
		if ( m_iCurrFrameScale >= m_pAnim->m_iNumScales ) m_iCurrFrameScale = 0;

		// positions
		if ( m_pAnim->m_iNumPositions > 0 && m_pAnim->m_pPositions[ 0 ]->m_fTime <= currtime )
		{
			posChanged = 1;
			if ( m_pAnim->m_pPositions[ m_iCurrFramePosition ]->m_fTime == currtime || m_pAnim->m_iNumPositions == 1 )
			{
				// animation frame exactly matches time we are looking for, no interpolation required
				newX = m_pAnim->m_pPositions[ m_iCurrFramePosition ]->m_fX;
				newY = m_pAnim->m_pPositions[ m_iCurrFramePosition ]->m_fY;
			}
			else
			{
				if ( m_pAnim->m_pPositions[ m_iCurrFramePosition ]->m_fTime < currtime )
				{
					// time is ahead of our pointer, advance until we find the right frame
					while ( m_iCurrFramePosition < m_pAnim->m_iNumPositions-1 
						 && m_pAnim->m_pPositions[ m_iCurrFramePosition+1 ]->m_fTime < currtime ) m_iCurrFramePosition++;
				}
				else
				{
					// time is behind our pointer, go backwards until we find the right frame
					while ( m_iCurrFramePosition > 0
						 && m_pAnim->m_pPositions[ m_iCurrFramePosition ]->m_fTime > currtime ) m_iCurrFramePosition--;
				}

				if ( m_iCurrFramePosition == m_pAnim->m_iNumPositions-1 )
				{
					// reached last keyframe and animation hasn't looped yet, must be some dead time for this bone
					newX = m_pAnim->m_pPositions[ m_iCurrFramePosition ]->m_fX;
					newY = m_pAnim->m_pPositions[ m_iCurrFramePosition ]->m_fY;
				}
				else
				{
					float timeDiff = m_pAnim->m_pPositions[ m_iCurrFramePosition+1 ]->m_fTime - m_pAnim->m_pPositions[ m_iCurrFramePosition ]->m_fTime;
					if ( timeDiff <= 0 )
					{
						// key frames have the same time, should never happen but account for it anyway
						newX = m_pAnim->m_pPositions[ m_iCurrFramePosition+1 ]->m_fX;
						newY = m_pAnim->m_pPositions[ m_iCurrFramePosition+1 ]->m_fY;
					}
					else
					{
						// interpolate between these key frames
						float t = (currtime - m_pAnim->m_pPositions[ m_iCurrFramePosition ]->m_fTime) / timeDiff;
						m_pAnim->m_pPositions[ m_iCurrFramePosition ]->Interpolate( m_pAnim->m_pPositions[ m_iCurrFramePosition+1 ], t, newX, newY );
					}
				}
			}
		}

		// rotation
		if ( m_pAnim->m_iNumRotations > 0 && m_pAnim->m_pRotations[ 0 ]->m_fTime <= currtime )
		{
			angChanged = 1;
			if ( m_pAnim->m_pRotations[ m_iCurrFrameRotation ]->m_fTime == currtime || m_pAnim->m_iNumRotations == 1 )
			{
				// animation frame exactly matches time we are looking for, no interpolation required
				newAngle = m_pAnim->m_pRotations[ m_iCurrFrameRotation ]->m_fRotation;
			}
			else
			{
				if ( m_pAnim->m_pRotations[ m_iCurrFrameRotation ]->m_fTime < currtime )
				{
					// time is ahead of our pointer, advance until we find the right frame
					while ( m_iCurrFrameRotation < m_pAnim->m_iNumRotations-1 
						 && m_pAnim->m_pRotations[ m_iCurrFrameRotation+1 ]->m_fTime < currtime ) m_iCurrFrameRotation++;
				}
				else
				{
					// time is behind our pointer, go backwards until we find the right frame
					while ( m_iCurrFrameRotation > 0
						 && m_pAnim->m_pRotations[ m_iCurrFrameRotation ]->m_fTime > currtime ) m_iCurrFrameRotation--;
				}

				if ( m_iCurrFrameRotation == m_pAnim->m_iNumRotations-1 )
				{
					// reached last keyframe and animation hasn't looped yet, must be some dead time for this bone
					newAngle = m_pAnim->m_pRotations[ m_iCurrFrameRotation ]->m_fRotation;
				}
				else
				{
					float timeDiff = m_pAnim->m_pRotations[ m_iCurrFrameRotation+1 ]->m_fTime - m_pAnim->m_pRotations[ m_iCurrFrameRotation ]->m_fTime;
					if ( timeDiff <= 0 )
					{
						// key frames have the same time, should never happen but account for it anyway
						newAngle = m_pAnim->m_pRotations[ m_iCurrFrameRotation+1 ]->m_fRotation;
					}
					else
					{
						// interpolate between these key frames
						float t = (currtime - m_pAnim->m_pRotations[ m_iCurrFrameRotation ]->m_fTime) / timeDiff;
						m_pAnim->m_pRotations[ m_iCurrFrameRotation ]->Interpolate( m_pAnim->m_pRotations[ m_iCurrFrameRotation+1 ], t, newAngle );
					}
				}
			}
		}

		// scales
		if ( m_pAnim->m_iNumScales > 0 && m_pAnim->m_pScales[ 0 ]->m_fTime <= currtime )
		{
			scaleChanged = 1;
			if ( m_pAnim->m_pScales[ m_iCurrFrameScale ]->m_fTime == currtime || m_pAnim->m_iNumScales == 1 )
			{
				// animation frame exactly matches time we are looking for, no interpolation required
				newSX = m_pAnim->m_pScales[ m_iCurrFrameScale ]->m_fScaleX;
				newSY = m_pAnim->m_pScales[ m_iCurrFrameScale ]->m_fScaleY;
			}
			else
			{
				if ( m_pAnim->m_pScales[ m_iCurrFrameScale ]->m_fTime < currtime )
				{
					// time is ahead of our pointer, advance until we find the right frame
					while ( m_iCurrFrameScale < m_pAnim->m_iNumScales-1 
						 && m_pAnim->m_pScales[ m_iCurrFrameScale+1 ]->m_fTime < currtime ) m_iCurrFrameScale++;
				}
				else
				{
					// time is behind our pointer, go backwards until we find the right frame
					while ( m_iCurrFrameScale > 0
						 && m_pAnim->m_pScales[ m_iCurrFrameScale ]->m_fTime > currtime ) m_iCurrFrameScale--;
				}

				if ( m_iCurrFrameScale == m_pAnim->m_iNumScales-1 )
				{
					// reached last keyframe and animation hasn't looped yet, must be some dead time for this bone
					newSX = m_pAnim->m_pScales[ m_iCurrFrameScale ]->m_fScaleX;
					newSY = m_pAnim->m_pScales[ m_iCurrFrameScale ]->m_fScaleY;
				}
				else
				{
					float timeDiff = m_pAnim->m_pScales[ m_iCurrFrameScale+1 ]->m_fTime - m_pAnim->m_pScales[ m_iCurrFrameScale ]->m_fTime;
					if ( timeDiff <= 0 )
					{
						// key frames have the same time, should never happen but account for it anyway
						newSX = m_pAnim->m_pScales[ m_iCurrFrameScale+1 ]->m_fScaleX;
						newSY = m_pAnim->m_pScales[ m_iCurrFrameScale+1 ]->m_fScaleY;
					}
					else
					{
						// interpolate between these key frames
						float t = (currtime - m_pAnim->m_pScales[ m_iCurrFrameScale ]->m_fTime) / timeDiff;
						m_pAnim->m_pScales[ m_iCurrFrameScale ]->Interpolate( m_pAnim->m_pScales[ m_iCurrFrameScale+1 ], t, newSX, newSY );
					}
				}
			}
		}
	}

	m_iColor = newRed;
	m_iColor = (m_iColor << 8) | newGreen;
	m_iColor = (m_iColor << 8) | newBlue;
	m_iColor = (m_iColor << 8) | newAlpha;

	m_pParent = newBone;
	m_iZOrder = newZOrder;

	x = newX;
	y = newY;
	angle = newAngle;
	sX = newSX;
	sY = newSY;
	
	m_pSprite = newSprite;
	
	if ( m_pSprite ) 
	{
		m_pSprite->SetColor( newRed, newGreen, newBlue, newAlpha );
		m_pSprite->SetVisible( true );
		m_pSprite->SetBone( m_pParent );
		if ( posChanged ) m_pSprite->SetPositionByOffset( x, y );
		if ( angChanged ) m_pSprite->SetAngle( angle );
		if ( scaleChanged ) m_pSprite->SetScaleByOffset( sX, sY );
	}
}

// bone

Bone2D::Bone2D()
{
	origX = 0;
	origY = 0;
	origAngle = 0;
	origSX = 1;
	origSY = 1;

	x = 0;
	y = 0;
	angle = 0;
	sX = 1;
	sY = 1;

	worldX = 0;
	worldY = 0;
	worldAngle = 0;
	worldSX = 1;
	worldSY = 1;

	m00 = 1; m01 = 0; m10 = 0; m11 = 1;

	m_bFlags = AGK_BONE_ANIMATE | AGK_BONE_INHERIT_ROTATION | AGK_BONE_INHERIT_SCALE;
	m_pParent = 0;
	length = 0;
	
	m_pAnim = 0;
	m_iCurrFrameRotation = 0;
	m_iCurrFramePosition = 0;
	m_iCurrFrameScale = 0;
	m_iCurrFrameParent = 0;

	m_iPrevFrameRotation = 0;
	m_iPrevFramePosition = 0;
	m_iPrevFrameScale = 0;
	m_iPrevFrameParent = 0;
}

Bone2D::~Bone2D()
{
	
}

void Bone2D::Tween( float prevtime, float currtime, float s )
{
	if ( prevtime < 0 ) prevtime = 0;
	if ( currtime < 0 ) currtime = 0;

	float oldX = 0;
	float oldY = 0;
	float oldAngle = 0;
	float oldSX = 1;
	float oldSY = 1;
	Bone2D *oldParent = m_pParent;

	if ( m_pPrevAnim && (m_bFlags & AGK_BONE_ANIMATE) )
	{
		if ( m_iPrevFramePosition >= m_pPrevAnim->m_iNumPositions ) m_iPrevFramePosition = 0;
		if ( m_iPrevFrameRotation >= m_pPrevAnim->m_iNumRotations ) m_iPrevFrameRotation = 0;
		if ( m_iPrevFrameScale >= m_pPrevAnim->m_iNumScales ) m_iPrevFrameScale = 0;
		if ( m_iPrevFrameParent >= m_pPrevAnim->m_iNumParents ) m_iPrevFrameParent = 0;

		// positions
		if ( m_pPrevAnim->m_iNumPositions > 0 && m_pPrevAnim->m_pPositions[ 0 ]->m_fTime <= prevtime )
		{
			if ( m_pPrevAnim->m_pPositions[ m_iPrevFramePosition ]->m_fTime == prevtime || m_pPrevAnim->m_iNumPositions == 1 )
			{
				// animation frame exactly matches time we are looking for, no interpolation required
				oldX = m_pPrevAnim->m_pPositions[ m_iPrevFramePosition ]->m_fX;
				oldY = m_pPrevAnim->m_pPositions[ m_iPrevFramePosition ]->m_fY;
			}
			else
			{
				if ( m_pPrevAnim->m_pPositions[ m_iPrevFramePosition ]->m_fTime < prevtime )
				{
					// time is ahead of our pointer, advance until we find the right frame
					while ( m_iPrevFramePosition < m_pPrevAnim->m_iNumPositions-1 
						 && m_pPrevAnim->m_pPositions[ m_iPrevFramePosition+1 ]->m_fTime < prevtime ) m_iPrevFramePosition++;
				}
				else
				{
					// time is behind our pointer, go backwards until we find the right frame
					while ( m_iPrevFramePosition > 0
						 && m_pPrevAnim->m_pPositions[ m_iPrevFramePosition ]->m_fTime > prevtime ) m_iPrevFramePosition--;
				}

				if ( m_iPrevFramePosition == m_pPrevAnim->m_iNumPositions-1 )
				{
					// reached last keyframe and animation hasn't looped yet, must be some dead time for this bone
					oldX = m_pPrevAnim->m_pPositions[ m_iPrevFramePosition ]->m_fX;
					oldY = m_pPrevAnim->m_pPositions[ m_iPrevFramePosition ]->m_fY;
				}
				else
				{
					float timeDiff = m_pPrevAnim->m_pPositions[ m_iPrevFramePosition+1 ]->m_fTime - m_pPrevAnim->m_pPositions[ m_iPrevFramePosition ]->m_fTime;
					if ( timeDiff <= 0 )
					{
						// key frames have the same time, should never happen but account for it anyway
						oldX = m_pPrevAnim->m_pPositions[ m_iPrevFramePosition+1 ]->m_fX;
						oldY = m_pPrevAnim->m_pPositions[ m_iPrevFramePosition+1 ]->m_fY;
					}
					else
					{
						// interpolate between these key frames
						float t = (prevtime - m_pPrevAnim->m_pPositions[ m_iPrevFramePosition ]->m_fTime) / timeDiff;
						m_pPrevAnim->m_pPositions[ m_iPrevFramePosition ]->Interpolate( m_pPrevAnim->m_pPositions[ m_iPrevFramePosition+1 ], t, oldX, oldY );
					}
				}
			}
		}

		// rotation
		if ( m_pPrevAnim->m_iNumRotations > 0 && m_pPrevAnim->m_pRotations[ 0 ]->m_fTime <= prevtime )
		{
			if ( m_pPrevAnim->m_pRotations[ m_iPrevFrameRotation ]->m_fTime == prevtime || m_pPrevAnim->m_iNumRotations == 1 )
			{
				// animation frame exactly matches time we are looking for, no interpolation required
				oldAngle = m_pPrevAnim->m_pRotations[ m_iPrevFrameRotation ]->m_fRotation;
			}
			else
			{
				if ( m_pPrevAnim->m_pRotations[ m_iPrevFrameRotation ]->m_fTime < prevtime )
				{
					// time is ahead of our pointer, advance until we find the right frame
					while ( m_iPrevFrameRotation < m_pPrevAnim->m_iNumRotations-1 
						 && m_pPrevAnim->m_pRotations[ m_iPrevFrameRotation+1 ]->m_fTime < prevtime ) m_iPrevFrameRotation++;
				}
				else
				{
					// time is behind our pointer, go backwards until we find the right frame
					while ( m_iPrevFrameRotation > 0
						 && m_pPrevAnim->m_pRotations[ m_iPrevFrameRotation ]->m_fTime > prevtime ) m_iPrevFrameRotation--;
				}

				if ( m_iPrevFrameRotation == m_pPrevAnim->m_iNumRotations-1 )
				{
					// reached last keyframe and animation hasn't looped yet, must be some dead time for this bone
					oldAngle = m_pPrevAnim->m_pRotations[ m_iPrevFrameRotation ]->m_fRotation;
				}
				else
				{
					float timeDiff = m_pPrevAnim->m_pRotations[ m_iPrevFrameRotation+1 ]->m_fTime - m_pPrevAnim->m_pRotations[ m_iPrevFrameRotation ]->m_fTime;
					if ( timeDiff <= 0 )
					{
						// key frames have the same time, should never happen but account for it anyway
						oldAngle = m_pPrevAnim->m_pRotations[ m_iPrevFrameRotation+1 ]->m_fRotation;
					}
					else
					{
						// interpolate between these key frames
						float t = (prevtime - m_pPrevAnim->m_pRotations[ m_iPrevFrameRotation ]->m_fTime) / timeDiff;
						m_pPrevAnim->m_pRotations[ m_iPrevFrameRotation ]->Interpolate( m_pPrevAnim->m_pRotations[ m_iPrevFrameRotation+1 ], t, oldAngle );
					}
				}
			}
		}

		// scales
		if ( m_pPrevAnim->m_iNumScales > 0 && m_pPrevAnim->m_pScales[ 0 ]->m_fTime <= prevtime )
		{
			if ( m_pPrevAnim->m_pScales[ m_iPrevFrameScale ]->m_fTime == prevtime || m_pPrevAnim->m_iNumScales == 1 )
			{
				// animation frame exactly matches time we are looking for, no interpolation required
				oldSX = m_pPrevAnim->m_pScales[ m_iPrevFrameScale ]->m_fScaleX;
				oldSY = m_pPrevAnim->m_pScales[ m_iPrevFrameScale ]->m_fScaleY;
			}
			else
			{
				if ( m_pPrevAnim->m_pScales[ m_iPrevFrameScale ]->m_fTime < prevtime )
				{
					// time is ahead of our pointer, advance until we find the right frame
					while ( m_iPrevFrameScale < m_pPrevAnim->m_iNumScales-1 
						 && m_pPrevAnim->m_pScales[ m_iPrevFrameScale+1 ]->m_fTime < prevtime ) m_iPrevFrameScale++;
				}
				else
				{
					// time is behind our pointer, go backwards until we find the right frame
					while ( m_iPrevFrameScale > 0
						 && m_pPrevAnim->m_pScales[ m_iPrevFrameScale ]->m_fTime > prevtime ) m_iPrevFrameScale--;
				}

				if ( m_iPrevFrameScale == m_pPrevAnim->m_iNumScales-1 )
				{
					// reached last keyframe and animation hasn't looped yet, must be some dead time for this bone
					oldSX = m_pPrevAnim->m_pScales[ m_iPrevFrameScale ]->m_fScaleX;
					oldSY = m_pPrevAnim->m_pScales[ m_iPrevFrameScale ]->m_fScaleY;
				}
				else
				{
					float timeDiff = m_pPrevAnim->m_pScales[ m_iPrevFrameScale+1 ]->m_fTime - m_pPrevAnim->m_pScales[ m_iPrevFrameScale ]->m_fTime;
					if ( timeDiff <= 0 )
					{
						// key frames have the same time, should never happen but account for it anyway
						oldSX = m_pPrevAnim->m_pScales[ m_iPrevFrameScale+1 ]->m_fScaleX;
						oldSY = m_pPrevAnim->m_pScales[ m_iPrevFrameScale+1 ]->m_fScaleY;
					}
					else
					{
						// interpolate between these key frames
						float t = (prevtime - m_pPrevAnim->m_pScales[ m_iPrevFrameScale ]->m_fTime) / timeDiff;
						m_pPrevAnim->m_pScales[ m_iPrevFrameScale ]->Interpolate( m_pPrevAnim->m_pScales[ m_iPrevFrameScale+1 ], t, oldSX, oldSY );
					}
				}
			}
		}

		// parents
		if ( m_pPrevAnim->m_iNumParents > 0 && m_pPrevAnim->m_pParents[ 0 ]->m_fTime <= prevtime )
		{
			if ( m_pPrevAnim->m_pParents[ m_iPrevFrameParent ]->m_fTime == prevtime || m_pPrevAnim->m_iNumParents == 1 )
			{
				// animation frame exactly matches time we are looking for, no interpolation required
				oldParent = m_pPrevAnim->m_pParents[ m_iPrevFrameParent ]->m_pBoneParent;
			}
			else
			{
				if ( m_pPrevAnim->m_pParents[ m_iPrevFrameParent ]->m_fTime < prevtime )
				{
					// time is ahead of our pointer, advance until we find the right frame
					while ( m_iPrevFrameParent < m_pPrevAnim->m_iNumParents-1 
						 && m_pPrevAnim->m_pParents[ m_iPrevFrameParent+1 ]->m_fTime < prevtime ) m_iPrevFrameParent++;
				}
				else
				{
					// time is behind our pointer, go backwards until we find the right frame
					while ( m_iPrevFrameParent > 0
						 && m_pPrevAnim->m_pParents[ m_iPrevFrameParent ]->m_fTime > prevtime ) m_iPrevFrameParent--;
				}

				// no interpolation for parents
				oldParent = m_pPrevAnim->m_pParents[ m_iPrevFrameParent ]->m_pBoneParent;
			}
		}
	}

	// now the new transform
	float newX = 0;
	float newY = 0;
	float newAngle = 0;
	float newSX = 1;
	float newSY = 1;

	if ( m_pAnim && (m_bFlags & AGK_BONE_ANIMATE) )
	{
		if ( m_iCurrFramePosition >= m_pAnim->m_iNumPositions ) m_iCurrFramePosition = 0;
		if ( m_iCurrFrameRotation >= m_pAnim->m_iNumRotations ) m_iCurrFrameRotation = 0;
		if ( m_iCurrFrameScale >= m_pAnim->m_iNumScales ) m_iCurrFrameScale = 0;
		if ( m_iCurrFrameParent >= m_pAnim->m_iNumParents ) m_iCurrFrameParent = 0;

		// positions
		if ( m_pAnim->m_iNumPositions > 0 && m_pAnim->m_pPositions[ 0 ]->m_fTime <= currtime )
		{
			if ( m_pAnim->m_pPositions[ m_iCurrFramePosition ]->m_fTime == currtime || m_pAnim->m_iNumPositions == 1 )
			{
				// animation frame exactly matches time we are looking for, no interpolation required
				newX = m_pAnim->m_pPositions[ m_iCurrFramePosition ]->m_fX;
				newY = m_pAnim->m_pPositions[ m_iCurrFramePosition ]->m_fY;
			}
			else
			{
				if ( m_pAnim->m_pPositions[ m_iCurrFramePosition ]->m_fTime < currtime )
				{
					// time is ahead of our pointer, advance until we find the right frame
					while ( m_iCurrFramePosition < m_pAnim->m_iNumPositions-1 
						 && m_pAnim->m_pPositions[ m_iCurrFramePosition+1 ]->m_fTime < currtime ) m_iCurrFramePosition++;
				}
				else
				{
					// time is behind our pointer, go backwards until we find the right frame
					while ( m_iCurrFramePosition > 0
						 && m_pAnim->m_pPositions[ m_iCurrFramePosition ]->m_fTime > currtime ) m_iCurrFramePosition--;
				}

				if ( m_iCurrFramePosition == m_pAnim->m_iNumPositions-1 )
				{
					// reached last keyframe and animation hasn't looped yet, must be some dead time for this bone
					newX = m_pAnim->m_pPositions[ m_iCurrFramePosition ]->m_fX;
					newY = m_pAnim->m_pPositions[ m_iCurrFramePosition ]->m_fY;
				}
				else
				{
					float timeDiff = m_pAnim->m_pPositions[ m_iCurrFramePosition+1 ]->m_fTime - m_pAnim->m_pPositions[ m_iCurrFramePosition ]->m_fTime;
					if ( timeDiff <= 0 )
					{
						// key frames have the same time, should never happen but account for it anyway
						newX = m_pAnim->m_pPositions[ m_iCurrFramePosition+1 ]->m_fX;
						newY = m_pAnim->m_pPositions[ m_iCurrFramePosition+1 ]->m_fY;
					}
					else
					{
						// interpolate between these key frames
						float t = (currtime - m_pAnim->m_pPositions[ m_iCurrFramePosition ]->m_fTime) / timeDiff;
						m_pAnim->m_pPositions[ m_iCurrFramePosition ]->Interpolate( m_pAnim->m_pPositions[ m_iCurrFramePosition+1 ], t, newX, newY );
					}
				}
			}
		}

		// rotation
		if ( m_pAnim->m_iNumRotations > 0 && m_pAnim->m_pRotations[ 0 ]->m_fTime <= currtime )
		{
			if ( m_pAnim->m_pRotations[ m_iCurrFrameRotation ]->m_fTime == currtime || m_pAnim->m_iNumRotations == 1 )
			{
				// animation frame exactly matches time we are looking for, no interpolation required
				newAngle = m_pAnim->m_pRotations[ m_iCurrFrameRotation ]->m_fRotation;
			}
			else
			{
				if ( m_pAnim->m_pRotations[ m_iCurrFrameRotation ]->m_fTime < currtime )
				{
					// time is ahead of our pointer, advance until we find the right frame
					while ( m_iCurrFrameRotation < m_pAnim->m_iNumRotations-1 
						 && m_pAnim->m_pRotations[ m_iCurrFrameRotation+1 ]->m_fTime < currtime ) m_iCurrFrameRotation++;
				}
				else
				{
					// time is behind our pointer, go backwards until we find the right frame
					while ( m_iCurrFrameRotation > 0
						 && m_pAnim->m_pRotations[ m_iCurrFrameRotation ]->m_fTime > currtime ) m_iCurrFrameRotation--;
				}

				if ( m_iCurrFrameRotation == m_pAnim->m_iNumRotations-1 )
				{
					// reached last keyframe and animation hasn't looped yet, must be some dead time for this bone
					newAngle = m_pAnim->m_pRotations[ m_iCurrFrameRotation ]->m_fRotation;
				}
				else
				{
					float timeDiff = m_pAnim->m_pRotations[ m_iCurrFrameRotation+1 ]->m_fTime - m_pAnim->m_pRotations[ m_iCurrFrameRotation ]->m_fTime;
					if ( timeDiff <= 0 )
					{
						// key frames have the same time, should never happen but account for it anyway
						newAngle = m_pAnim->m_pRotations[ m_iCurrFrameRotation+1 ]->m_fRotation;
					}
					else
					{
						// interpolate between these key frames
						float t = (currtime - m_pAnim->m_pRotations[ m_iCurrFrameRotation ]->m_fTime) / timeDiff;
						m_pAnim->m_pRotations[ m_iCurrFrameRotation ]->Interpolate( m_pAnim->m_pRotations[ m_iCurrFrameRotation+1 ], t, newAngle );
					}
				}
			}
		}

		// scales
		if ( m_pAnim->m_iNumScales > 0 && m_pAnim->m_pScales[ 0 ]->m_fTime <= currtime )
		{
			if ( m_pAnim->m_pScales[ m_iCurrFrameScale ]->m_fTime == currtime || m_pAnim->m_iNumScales == 1 )
			{
				// animation frame exactly matches time we are looking for, no interpolation required
				newSX = m_pAnim->m_pScales[ m_iCurrFrameScale ]->m_fScaleX;
				newSY = m_pAnim->m_pScales[ m_iCurrFrameScale ]->m_fScaleY;
			}
			else
			{
				if ( m_pAnim->m_pScales[ m_iCurrFrameScale ]->m_fTime < currtime )
				{
					// time is ahead of our pointer, advance until we find the right frame
					while ( m_iCurrFrameScale < m_pAnim->m_iNumScales-1 
						 && m_pAnim->m_pScales[ m_iCurrFrameScale+1 ]->m_fTime < currtime ) m_iCurrFrameScale++;
				}
				else
				{
					// time is behind our pointer, go backwards until we find the right frame
					while ( m_iCurrFrameScale > 0
						 && m_pAnim->m_pScales[ m_iCurrFrameScale ]->m_fTime > currtime ) m_iCurrFrameScale--;
				}

				if ( m_iCurrFrameScale == m_pAnim->m_iNumScales-1 )
				{
					// reached last keyframe and animation hasn't looped yet, must be some dead time for this bone
					newSX = m_pAnim->m_pScales[ m_iCurrFrameScale ]->m_fScaleX;
					newSY = m_pAnim->m_pScales[ m_iCurrFrameScale ]->m_fScaleY;
				}
				else
				{
					float timeDiff = m_pAnim->m_pScales[ m_iCurrFrameScale+1 ]->m_fTime - m_pAnim->m_pScales[ m_iCurrFrameScale ]->m_fTime;
					if ( timeDiff <= 0 )
					{
						// key frames have the same time, should never happen but account for it anyway
						newSX = m_pAnim->m_pScales[ m_iCurrFrameScale+1 ]->m_fScaleX;
						newSY = m_pAnim->m_pScales[ m_iCurrFrameScale+1 ]->m_fScaleY;
					}
					else
					{
						// interpolate between these key frames
						float t = (currtime - m_pAnim->m_pScales[ m_iCurrFrameScale ]->m_fTime) / timeDiff;
						m_pAnim->m_pScales[ m_iCurrFrameScale ]->Interpolate( m_pAnim->m_pScales[ m_iCurrFrameScale+1 ], t, newSX, newSY );
					}
				}
			}
		}

		// no parent tweening
	}

	if ( oldAngle < newAngle )
	{
		while ( newAngle - oldAngle > 180 ) oldAngle += 360;
	}
	else
	{
		while ( oldAngle - newAngle > 180 ) newAngle += 360;
	}

	x = origX + oldX + (newX-oldX)*s;
	y = origY + oldY + (newY-oldY)*s;
	angle = origAngle + oldAngle + (newAngle-oldAngle)*s;
	sX = (oldSX + (newSX-oldSX)*s)*origSX;
	sY = (oldSY + (newSY-oldSY)*s)*origSY;

	m_pParent = oldParent;
}

void Bone2D::Interpolate( float currtime )
{
	float newX = 0;
	float newY = 0;
	float newAngle = 0;
	float newSX = 1;
	float newSY = 1;
	Bone2D *newParent = m_pParent;

	if ( m_pAnim && (m_bFlags & AGK_BONE_ANIMATE) )
	{
		if ( m_iCurrFramePosition >= m_pAnim->m_iNumPositions ) m_iCurrFramePosition = 0;
		if ( m_iCurrFrameRotation >= m_pAnim->m_iNumRotations ) m_iCurrFrameRotation = 0;
		if ( m_iCurrFrameScale >= m_pAnim->m_iNumScales ) m_iCurrFrameScale = 0;
		if ( m_iCurrFrameParent >= m_pAnim->m_iNumParents ) m_iCurrFrameParent = 0;

		// positions
		if ( m_pAnim->m_iNumPositions > 0 && m_pAnim->m_pPositions[ 0 ]->m_fTime <= currtime )
		{
			if ( m_pAnim->m_pPositions[ m_iCurrFramePosition ]->m_fTime == currtime || m_pAnim->m_iNumPositions == 1 )
			{
				// animation frame exactly matches time we are looking for, no interpolation required
				newX = m_pAnim->m_pPositions[ m_iCurrFramePosition ]->m_fX;
				newY = m_pAnim->m_pPositions[ m_iCurrFramePosition ]->m_fY;
			}
			else
			{
				if ( m_pAnim->m_pPositions[ m_iCurrFramePosition ]->m_fTime < currtime )
				{
					// time is ahead of our pointer, advance until we find the right frame
					while ( m_iCurrFramePosition < m_pAnim->m_iNumPositions-1 
						 && m_pAnim->m_pPositions[ m_iCurrFramePosition+1 ]->m_fTime < currtime ) m_iCurrFramePosition++;
				}
				else
				{
					// time is behind our pointer, go backwards until we find the right frame
					while ( m_iCurrFramePosition > 0
						 && m_pAnim->m_pPositions[ m_iCurrFramePosition ]->m_fTime > currtime ) m_iCurrFramePosition--;
				}

				if ( m_iCurrFramePosition == m_pAnim->m_iNumPositions-1 )
				{
					// reached last keyframe and animation hasn't looped yet, must be some dead time for this bone
					newX = m_pAnim->m_pPositions[ m_iCurrFramePosition ]->m_fX;
					newY = m_pAnim->m_pPositions[ m_iCurrFramePosition ]->m_fY;
				}
				else
				{
					float timeDiff = m_pAnim->m_pPositions[ m_iCurrFramePosition+1 ]->m_fTime - m_pAnim->m_pPositions[ m_iCurrFramePosition ]->m_fTime;
					if ( timeDiff <= 0 )
					{
						// key frames have the same time, should never happen but account for it anyway
						newX = m_pAnim->m_pPositions[ m_iCurrFramePosition+1 ]->m_fX;
						newY = m_pAnim->m_pPositions[ m_iCurrFramePosition+1 ]->m_fY;
					}
					else
					{
						// interpolate between these key frames
						float t = (currtime - m_pAnim->m_pPositions[ m_iCurrFramePosition ]->m_fTime) / timeDiff;
						m_pAnim->m_pPositions[ m_iCurrFramePosition ]->Interpolate( m_pAnim->m_pPositions[ m_iCurrFramePosition+1 ], t, newX, newY );
					}
				}
			}
		}

		// rotation
		if ( m_pAnim->m_iNumRotations > 0 && m_pAnim->m_pRotations[ 0 ]->m_fTime <= currtime )
		{
			if ( m_pAnim->m_pRotations[ m_iCurrFrameRotation ]->m_fTime == currtime || m_pAnim->m_iNumRotations == 1 )
			{
				// animation frame exactly matches time we are looking for, no interpolation required
				newAngle = m_pAnim->m_pRotations[ m_iCurrFrameRotation ]->m_fRotation;
			}
			else
			{
				if ( m_pAnim->m_pRotations[ m_iCurrFrameRotation ]->m_fTime < currtime )
				{
					// time is ahead of our pointer, advance until we find the right frame
					while ( m_iCurrFrameRotation < m_pAnim->m_iNumRotations-1 
						 && m_pAnim->m_pRotations[ m_iCurrFrameRotation+1 ]->m_fTime < currtime ) m_iCurrFrameRotation++;
				}
				else
				{
					// time is behind our pointer, go backwards until we find the right frame
					while ( m_iCurrFrameRotation > 0
						 && m_pAnim->m_pRotations[ m_iCurrFrameRotation ]->m_fTime > currtime ) m_iCurrFrameRotation--;
				}

				if ( m_iCurrFrameRotation == m_pAnim->m_iNumRotations-1 )
				{
					// reached last keyframe and animation hasn't looped yet, must be some dead time for this bone
					newAngle = m_pAnim->m_pRotations[ m_iCurrFrameRotation ]->m_fRotation;
				}
				else
				{
					float timeDiff = m_pAnim->m_pRotations[ m_iCurrFrameRotation+1 ]->m_fTime - m_pAnim->m_pRotations[ m_iCurrFrameRotation ]->m_fTime;
					if ( timeDiff <= 0 )
					{
						// key frames have the same time, should never happen but account for it anyway
						newAngle = m_pAnim->m_pRotations[ m_iCurrFrameRotation+1 ]->m_fRotation;
					}
					else
					{
						// interpolate between these key frames
						float t = (currtime - m_pAnim->m_pRotations[ m_iCurrFrameRotation ]->m_fTime) / timeDiff;
						m_pAnim->m_pRotations[ m_iCurrFrameRotation ]->Interpolate( m_pAnim->m_pRotations[ m_iCurrFrameRotation+1 ], t, newAngle );
					}
				}
			}
		}

		// scales
		if ( m_pAnim->m_iNumScales > 0 && m_pAnim->m_pScales[ 0 ]->m_fTime <= currtime )
		{
			if ( m_pAnim->m_pScales[ m_iCurrFrameScale ]->m_fTime == currtime || m_pAnim->m_iNumScales == 1 )
			{
				// animation frame exactly matches time we are looking for, no interpolation required
				newSX = m_pAnim->m_pScales[ m_iCurrFrameScale ]->m_fScaleX;
				newSY = m_pAnim->m_pScales[ m_iCurrFrameScale ]->m_fScaleY;
			}
			else
			{
				if ( m_pAnim->m_pScales[ m_iCurrFrameScale ]->m_fTime < currtime )
				{
					// time is ahead of our pointer, advance until we find the right frame
					while ( m_iCurrFrameScale < m_pAnim->m_iNumScales-1 
						 && m_pAnim->m_pScales[ m_iCurrFrameScale+1 ]->m_fTime < currtime ) m_iCurrFrameScale++;
				}
				else
				{
					// time is behind our pointer, go backwards until we find the right frame
					while ( m_iCurrFrameScale > 0
						 && m_pAnim->m_pScales[ m_iCurrFrameScale ]->m_fTime > currtime ) m_iCurrFrameScale--;
				}

				if ( m_iCurrFrameScale == m_pAnim->m_iNumScales-1 )
				{
					// reached last keyframe and animation hasn't looped yet, must be some dead time for this bone
					newSX = m_pAnim->m_pScales[ m_iCurrFrameScale ]->m_fScaleX;
					newSY = m_pAnim->m_pScales[ m_iCurrFrameScale ]->m_fScaleY;
				}
				else
				{
					float timeDiff = m_pAnim->m_pScales[ m_iCurrFrameScale+1 ]->m_fTime - m_pAnim->m_pScales[ m_iCurrFrameScale ]->m_fTime;
					if ( timeDiff <= 0 )
					{
						// key frames have the same time, should never happen but account for it anyway
						newSX = m_pAnim->m_pScales[ m_iCurrFrameScale+1 ]->m_fScaleX;
						newSY = m_pAnim->m_pScales[ m_iCurrFrameScale+1 ]->m_fScaleY;
					}
					else
					{
						// interpolate between these key frames
						float t = (currtime - m_pAnim->m_pScales[ m_iCurrFrameScale ]->m_fTime) / timeDiff;
						m_pAnim->m_pScales[ m_iCurrFrameScale ]->Interpolate( m_pAnim->m_pScales[ m_iCurrFrameScale+1 ], t, newSX, newSY );
					}
				}
			}
		}

		// bone parent
		if ( m_pAnim->m_iNumParents > 0 && m_pAnim->m_pParents[ 0 ]->m_fTime <= currtime )
		{
			if ( m_pAnim->m_pParents[ m_iCurrFrameParent ]->m_fTime == currtime || m_pAnim->m_iNumParents == 1 )
			{
				// animation frame exactly matches time we are looking for, no interpolation required
				newParent = m_pAnim->m_pParents[ m_iCurrFrameParent ]->m_pBoneParent;
			}
			else
			{
				if ( m_pAnim->m_pParents[ m_iCurrFrameParent ]->m_fTime < currtime )
				{
					// time is ahead of our pointer, advance until we find the right frame
					while ( m_iCurrFrameParent < m_pAnim->m_iNumParents-1 
						 && m_pAnim->m_pParents[ m_iCurrFrameParent+1 ]->m_fTime < currtime ) m_iCurrFrameParent++;
				}
				else
				{
					// time is behind our pointer, go backwards until we find the right frame
					while ( m_iCurrFrameParent > 0
						 && m_pAnim->m_pParents[ m_iCurrFrameParent ]->m_fTime > currtime ) m_iCurrFrameParent--;
				}

				newParent = m_pAnim->m_pParents[ m_iCurrFrameParent ]->m_pBoneParent;
			}
		}
	}

	x = origX + newX;
	y = origY + newY;
	angle = origAngle + newAngle;
	sX = newSX*origSX;
	sY = newSY*origSY;

	m_pParent = newParent;
}

void Bone2D::UpdateWorldMatrix( int flipH, int flipV )
{
	if ( !m_pParent )
	{
		if ( flipH == 1 ) worldX = origX - (x - origX);
		else worldX = x;
		if ( flipV == 1 ) worldY = origY - (y - origY);
		else worldY = y;
		worldAngle = angle;
		worldSX = sX;
		worldSY = sY;
	}
	else
	{
		worldX = m_pParent->m00 * x + m_pParent->m01 * y + m_pParent->worldX;
		worldY = m_pParent->m10 * x + m_pParent->m11 * y + m_pParent->worldY;
		worldAngle = angle;
		if ( m_bFlags & AGK_BONE_INHERIT_ROTATION ) worldAngle += m_pParent->worldAngle;
		worldSX = sX;
		worldSY = sY;
		if ( m_bFlags & AGK_BONE_INHERIT_SCALE )
		{
			worldSX *= m_pParent->worldSX;
			worldSY *= m_pParent->worldSY;
		}
	}

	float cosA = agk::Cos( worldAngle );
	float sinA = agk::Sin( worldAngle );
	
	m00 = cosA * worldSX;
	m10 = sinA * worldSX;
	m01 = -sinA * worldSY;
	m11 = cosA * worldSY;

	if ( flipH == 1 )
	{
		m00 = -m00;
		m01 = -m01;
	}

	if ( flipV == 1 )
	{
		m10 = -m10;
		m11 = -m11;
	}
}

void Bone2D::ResetToOrig()
{
	x = origX;
	y = origY;
	angle = origAngle;
	sX = origSX;
	sY = origSY;
}

// Skeleton2D

Skeleton2D::Skeleton2D()
{
	m_iNumBones = 0;
	m_pBones = 0;

	m_iNumSprites = 0;
	m_pSprites = 0;

	m_iNumAnimations = 0;
	m_pAnimations = 0;

	m_iNumSlots = 0;
	m_pSlots = 0;

	m_pFirstExtSprite = 0;

	m_iID = 0;
	m_bFlags = AGK_SKELETON_VISIBLE;
	m_fCurrTime = 0;
	m_fPrevTime = 0;
	m_fTweenTime = 0;
	m_fTotalTweenTime = 0;
	m_iCurrAnimation = -1;
	m_fSpeed = 1;
	m_iLoopTotal = 0;
	m_iLoopCount = 0;

	m_fX = 0;
	m_fY = 0;
	m_fAngle = 0;
	m_iDepth = 10;
}

Skeleton2D::~Skeleton2D()
{
	if ( m_pBones ) delete [] m_pBones;
	if ( m_pSprites ) 
	{
		// one image was created per sprite
		for ( UINT i = 0; i < m_iNumSprites; i++ )
		{
			cImage *pImage = m_pSprites[i].GetImagePtr();
			if ( pImage )
			{
				m_pSprites[i].SetImage(0);
				delete pImage;
			}
		}
		
		delete [] m_pSprites;
	}
	if ( m_pAnimations ) delete [] m_pAnimations;
	if ( m_pSlots ) delete [] m_pSlots;

	while( m_pFirstExtSprite )
	{
		ExternalSprite *pExtSprite = m_pFirstExtSprite;
		if ( pExtSprite->m_pSprite ) pExtSprite->m_pSprite->ResetSkeleton();
		m_pFirstExtSprite = m_pFirstExtSprite->m_pNext;
		delete pExtSprite;
	}
}

void Skeleton2D::LoadFromSpine( const char* filename, float scale, cImage *pAtlas, int loadAnim )
{
	JSONElement *pRoot = JSONElement::LoadJSONFromFile( filename );
	if ( !pRoot ) return;
	
	if ( pRoot->GetType() != 1 )
	{
		agk::Error( "Failed to load Spine skeleton, JSON root is not an object" );
		delete pRoot;
		return;
	}

	JSONObject *pObject = (JSONObject*) pRoot;

	// bones
	JSONElement *pElement = pObject->GetElement( "bones" );
	if ( !pElement || pElement->GetType() != 2 )
	{
		agk::Error( "Failed to load Spine skeleton, JSON bones is missing or not an array" );
		delete pRoot;
		return;
	}

	JSONArray *pBones = (JSONArray*) pElement;
	if ( pBones->m_iNumElements < 1 )
	{
		agk::Error( "Failed to load Spine skeleton, no bones found" );
		delete pRoot;
		return;
	}

	m_iNumBones = pBones->m_iNumElements;
	m_pBones = new Bone2D[ pBones->m_iNumElements ];
	for ( UINT i = 0; i < pBones->m_iNumElements; i++ )
	{
		pElement = pBones->GetElement( i );
		if ( pElement->GetType() != 1 )
		{
			agk::Error( "Failed to load Spine skeleton, bone is not an object" );
			delete pRoot;
			return;
		}

		JSONObject *pBone = (JSONObject*) pElement;
		if ( pBone->m_iNumPairs < 1 )
		{
			agk::Error( "Failed to load Spine skeleton, bone has no elements" );
			delete pRoot;
			return;
		}

		pElement = pBone->GetElement( "name" );
		if ( !pElement ) 
		{
			agk::Error( "Failed to load Spine skeleton, bone has no name" );
			delete pRoot;
			return;
		}

		if ( pElement->GetType() != 3 )
		{
			agk::Error( "Failed to load Spine skeleton, bone name is not a string" );
			delete pRoot;
			return;
		}

		m_pBones[ i ].m_sName.SetStr( ((JSONString*)pElement)->m_sValue );

		pElement = pBone->GetElement( "parent" );
		if ( pElement && pElement->GetType() == 3 )
		{
			m_pBones[ i ].m_pParent = GetBone( ((JSONString*)pElement)->m_sValue );
		}
		else 
		{
			m_pBones[ i ].m_bFlags |= AGK_BONE_ROOT;
		}

		pElement = pBone->GetElement( "length" );
		if ( pElement && pElement->GetType() == 4 ) m_pBones[ i ].length = ((JSONNumber*)pElement)->m_fValue;

		pElement = pBone->GetElement( "x" );
		if ( pElement && pElement->GetType() == 4 ) m_pBones[ i ].origX = ((JSONNumber*)pElement)->m_fValue*scale;

		pElement = pBone->GetElement( "y" );
		if ( pElement && pElement->GetType() == 4 ) m_pBones[ i ].origY = -((JSONNumber*)pElement)->m_fValue*scale;

		pElement = pBone->GetElement( "rotation" );
		if ( pElement && pElement->GetType() == 4 ) m_pBones[ i ].origAngle = -((JSONNumber*)pElement)->m_fValue;

		pElement = pBone->GetElement( "scaleX" );
		if ( pElement && pElement->GetType() == 4 ) m_pBones[ i ].origSX = ((JSONNumber*)pElement)->m_fValue;

		pElement = pBone->GetElement( "scaleY" );
		if ( pElement && pElement->GetType() == 4 ) m_pBones[ i ].origSY = ((JSONNumber*)pElement)->m_fValue;

		pElement = pBone->GetElement( "inheritRotation" );
		if ( pElement && pElement->GetType() == 5 ) 
		{
			if ( ((JSONBool*)pElement)->m_bValue ) m_pBones[ i ].m_bFlags |= AGK_BONE_INHERIT_ROTATION;
			else m_pBones[ i ].m_bFlags &= ~AGK_BONE_INHERIT_ROTATION;
		}

		pElement = pBone->GetElement( "inheritScale" );
		if ( pElement && pElement->GetType() == 5 ) 
		{
			if ( ((JSONBool*)pElement)->m_bValue ) m_pBones[ i ].m_bFlags |= AGK_BONE_INHERIT_SCALE;
			else m_pBones[ i ].m_bFlags &= ~AGK_BONE_INHERIT_SCALE;
		}

		m_pBones[ i ].ResetToOrig();
	}

	// slots
	pElement = pObject->GetElement( "slots" );
	if ( !pElement || pElement->GetType() != 2 )
	{
		agk::Error( "Failed to load Spine skeleton, JSON slots is missing or not an array" );
		delete pRoot;
		return;
	}

	JSONArray *pSlots = (JSONArray*) pElement;
	if ( pSlots->m_iNumElements < 1 )
	{
		agk::Error( "Failed to load Spine skeleton, no slots found" );
		delete pRoot;
		return;
	}

	m_iNumSlots = pSlots->m_iNumElements;
	m_pSlots = new Slot2D[ pSlots->m_iNumElements ];
	for ( UINT i = 0; i < pSlots->m_iNumElements; i++ )
	{
		m_pSlots[ i ].m_iZOrder = i;

		pElement = pSlots->GetElement( i );
		if ( pElement->GetType() != 1 )
		{
			agk::Error( "Failed to load Spine skeleton, slot is not an object" );
			delete pRoot;
			return;
		}

		JSONObject *pSlot = (JSONObject*) pElement;
		if ( pSlot->m_iNumPairs < 1 )
		{
			agk::Error( "Failed to load Spine skeleton, slot has no elements" );
			delete pRoot;
			return;
		}

		pElement = pSlot->GetElement( "name" );
		if ( !pElement ) 
		{
			agk::Error( "Failed to load Spine skeleton, slot has no name" );
			delete pRoot;
			return;
		}

		if ( pElement->GetType() != 3 )
		{
			agk::Error( "Failed to load Spine skeleton, slot name is not a string" );
			delete pRoot;
			return;
		}

		m_pSlots[ i ].m_sName.SetStr( ((JSONString*)pElement)->m_sValue );
		
		pElement = pSlot->GetElement( "bone" );
		if ( !pElement ) 
		{
			agk::Error( "Failed to load Spine skeleton, slot has no bone" );
			delete pRoot;
			return;
		}

		if ( pElement->GetType() != 3 )
		{
			agk::Error( "Failed to load Spine skeleton, slot bone is not a string" );
			delete pRoot;
			return;
		}

		m_pSlots[ i ].m_pParent = GetBone( ((JSONString*)pElement)->m_sValue );
		if ( !m_pSlots[ i ].m_pParent )
		{
			agk::Error( "Failed to load Spine skeleton, slot bone does not exist" );
			delete pRoot;
			return;
		}

		pElement = pSlot->GetElement( "color" );
		if ( pElement && pElement->GetType() == 3 ) 
		{
			sscanf( ((JSONString*)pElement)->m_sValue, "%x", &(m_pSlots[ i ].m_iColor) );
		}
		else m_pSlots[ i ].m_iColor = 0xffffffff;

		m_pSlots[ i ].m_iOrigColor = m_pSlots[ i ].m_iColor;
		m_pSlots[ i ].m_pOrigBone = m_pSlots[ i ].m_pParent;

		/*
		pElement = pSlot->GetElement( "attachment" );
		if ( pElement && pElement->GetType() == 3 ) 
		{
			m_pSlots[ i ].m_pSprite = GetSprite( ((JSONString*)pElement)->m_sValue );
			if ( m_pSlots[ i ].m_pSprite ) m_pSlots[ i ].m_pSprite->SetBone( m_pSlots[ i ].m_pParent );
		}
		*/
	}

	// skins
	pElement = pObject->GetElement( "skins" );
	if ( pElement && pElement->GetType() == 1 )
	{
		JSONObject *pSkins = (JSONObject*) pElement;
		if ( pSkins->m_iNumPairs > 0 )
		{
			int count = 0;
			pElement = pSkins->GetElement( "default" );
			if ( pElement->GetType() == 1 )
			{
				JSONObject* pSkin = (JSONObject*)pElement;
				for ( UINT i = 0; i < pSkin->m_iNumPairs; i++ )
				{
					if ( pSkin->m_pPairs[ i ]->m_pElement->GetType() == 1 )
					{
						count += ((JSONObject*)pSkin->m_pPairs[ i ]->m_pElement)->m_iNumPairs;
					}
				}

				if ( count > 0 )
				{
					m_iNumSprites = count;
					m_pSprites = new cSprite[ count ];
					for ( int i = 0; i < count; i++ ) 
					{
						m_pSprites[ i ].SetVisible( false );
						m_pSprites[ i ].SetTransparency( 1 );
					}
					count = 0;

					for ( UINT i = 0; i < m_iNumSlots; i++ )
					{
						pElement = pSkin->GetElement( m_pSlots[ i ].m_sName );
						if ( pElement && pElement->GetType() == 1 )
						{
							JSONObject *pSlot = (JSONObject*) pElement;
							for ( UINT j = 0; j < pSlot->m_iNumPairs; j++ )
							{
								if ( pSlot->m_pPairs[ j ]->m_pElement->GetType() == 1 )
								{
									JSONObject *pAttachment = (JSONObject*) (pSlot->m_pPairs[ j ]->m_pElement);

									pElement = pAttachment->GetElement( "type" );
									if ( pElement && pElement->GetType() == 3 && ((JSONString*)pElement)->m_sValue.CompareTo( "region" ) != 0 ) continue;

									uString name( m_pSlots[ i ].m_sName );
									name.Append( ":" );
									name.Append( pSlot->m_pPairs[ j ]->m_sName );
									m_pSprites[ count ].SetName( name );

									if ( pAtlas ) 
									{
										uString imageName( pSlot->m_pPairs[ j ]->m_sName );

										pElement = pAttachment->GetElement( "name" );
										if ( pElement && pElement->GetType() == 3 ) imageName.SetStr( ((JSONString*)pElement)->m_sValue );

										cImage* pSubImage = new cImage();
										if ( !pSubImage->LoadSubImage( pAtlas, imageName, true ) )
										{
											delete pSubImage;
										}
										else
										{
											m_pSprites[ count ].SetImage( pSubImage );
										}
									}

									float x = 0;
									float y = 0;
									pElement = pAttachment->GetElement( "x" );
									if ( pElement && pElement->GetType() == 4 ) x = ((JSONNumber*)pElement)->m_fValue*scale;
									pElement = pAttachment->GetElement( "y" );
									if ( pElement && pElement->GetType() == 4 ) y = -((JSONNumber*)pElement)->m_fValue*scale;
									m_pSprites[ count ].SetPositionByOffset( x, y );

									pElement = pAttachment->GetElement( "rotation" );
									if ( pElement && pElement->GetType() == 4 ) m_pSprites[ count ].SetAngle( -((JSONNumber*)pElement)->m_fValue );

									float width = 10;
									float height = 10;
									pElement = pAttachment->GetElement( "width" );
									if ( pElement && pElement->GetType() == 4 ) width = ((JSONNumber*)pElement)->m_fValue*scale;
									pElement = pAttachment->GetElement( "height" );
									if ( pElement && pElement->GetType() == 4 ) height = ((JSONNumber*)pElement)->m_fValue*scale;
									m_pSprites[ count ].SetSize( width, height );

									float scaleX = 1;
									float scaleY = 1;
									pElement = pAttachment->GetElement( "scaleX" );
									if ( pElement && pElement->GetType() == 4 ) scaleX = ((JSONNumber*)pElement)->m_fValue;
									pElement = pAttachment->GetElement( "scaleY" );
									if ( pElement && pElement->GetType() == 4 ) scaleY = ((JSONNumber*)pElement)->m_fValue;

									int flipH = 0;
									int flipV = 0;
									if ( scaleX < 0 ) { flipH = 1; scaleX = -scaleX; }
									if ( scaleY < 0 ) { flipV = 1; scaleY = -scaleY; }
									m_pSprites[ count ].SetScaleByOffset( scaleX, scaleY );
									m_pSprites[ count ].SetFlip( flipH, flipV );

									count++;
								}
							}
						}
					}

					m_iNumSprites = count;
				}
			}
		}
	}

	// skin sprites
	for ( UINT i = 0; i < pSlots->m_iNumElements; i++ )
	{
		pElement = pSlots->GetElement( i );
		if ( pElement->GetType() != 1 ) continue;

		JSONObject *pSlot = (JSONObject*) pElement;
		if ( pSlot->m_iNumPairs < 1 ) continue;

		pElement = pSlot->GetElement( "name" );
		if ( !pElement ) continue;
		if ( pElement->GetType() != 3 ) continue;

		uString slotname( ((JSONString*)pElement)->m_sValue.GetStr() );
		Slot2D *pSkelSlot = GetSlot( slotname );
		if ( !pSkelSlot ) continue;

		pElement = pSlot->GetElement( "attachment" );
		if ( pElement && pElement->GetType() == 3 ) 
		{
			uString spritename;
			spritename.Format( "%s:%s", slotname.GetStr(), ((JSONString*)pElement)->m_sValue.GetStr() );
			pSkelSlot->m_pSprite = GetSprite( spritename );
			pSkelSlot->m_pOrigSprite = pSkelSlot->m_pSprite;
			pSkelSlot->m_pOrigBone = pSkelSlot->m_pParent;
			if ( pSkelSlot->m_pSprite ) 
			{
				pSkelSlot->m_pSprite->SetBone( pSkelSlot->m_pParent );
				pSkelSlot->m_pSprite->SetVisible( true );
				int alpha = pSkelSlot->m_iColor & 0xff;
				int blue = (pSkelSlot->m_iColor >> 8) & 0xff;
				int green = (pSkelSlot->m_iColor >> 16) & 0xff;
				int red = (pSkelSlot->m_iColor >> 24) & 0xff;
				pSkelSlot->m_pSprite->SetColor( red, green, blue, alpha );
			}
		}
	}

	if ( loadAnim )
	{
		pElement = pObject->GetElement( "animations" );
		if ( !pElement || pElement->GetType() != 1 )
		{
			agk::Error( "Failed to load Spine skeleton, JSON animations is missing or not an object" );
			delete pRoot;
			return;
		}

		JSONObject *pAnimations = (JSONObject*) pElement;
		if ( pAnimations->m_iNumPairs < 1 )
		{
			agk::Error( "Failed to load Spine skeleton, no animations found" );
			delete pRoot;
			return;
		}

		m_iNumAnimations = pAnimations->m_iNumPairs;
		m_pAnimations = new Animation2D[ m_iNumAnimations ];
		for ( UINT i = 0; i < m_iNumAnimations; i++ )
		{
			if ( !pAnimations->m_pPairs[ i ]->m_pElement || pAnimations->m_pPairs[ i ]->m_pElement->GetType() != 1 )
			{
				agk::Error( "Failed to load Spine skeleton, JSON animation is missing or not an object" );
				delete pRoot;
				return;
			}

			m_pAnimations[ i ].m_sName.SetStr( pAnimations->m_pPairs[ i ]->m_sName );

			JSONObject *pAnimation = (JSONObject*) pAnimations->m_pPairs[ i ]->m_pElement;
			pElement = pAnimation->GetElement( "bones" );
			if ( pElement )
			{
				if ( pElement->GetType() != 1 )
				{
					agk::Error( "Failed to load Spine skeleton, JSON bone animation is not an object" );
					delete pRoot;
					return;
				}

				JSONObject *pBoneAnims = (JSONObject*) pElement;
				if ( pBoneAnims->m_iNumPairs > 0 )
				{
					m_pAnimations[ i ].m_iNumBones = pBoneAnims->m_iNumPairs;
					m_pAnimations[ i ].m_pBoneAnims = new Anim2DBone[ pBoneAnims->m_iNumPairs ];
				}

				for ( UINT b = 0; b < pBoneAnims->m_iNumPairs; b++ )
				{
					pElement = pBoneAnims->m_pPairs[ b ]->m_pElement;
					Bone2D *pBone = GetBone( pBoneAnims->m_pPairs[ b ]->m_sName );
					if ( !pBone )
					{
						agk::Warning( "Warning when load Spine skeleton, animation found for bone that doesn't exist" );
					}
					else
					{
						if ( pElement->GetType() != 1 )
						{
							agk::Error( "Failed to load Spine skeleton, JSON animation bone is not an object" );
							delete pRoot;
							return;
						}

						Anim2DBone *pAGKBone = m_pAnimations[ i ].m_pBoneAnims + b;
						pAGKBone->m_sBoneName.SetStr( pBone->m_sName );
						pAGKBone->m_iBoneIndex = GetBoneIndex( pBoneAnims->m_pPairs[ b ]->m_sName );

						JSONObject *pAnimBone = (JSONObject*) pElement;

						// rotation key frames
						pElement = pAnimBone->GetElement( "rotate" );
						if ( pElement )
						{
							if ( pElement->GetType() != 2 )
							{
								agk::Error( "Failed to load Spine skeleton, JSON animation bone rotation is not an array" );
								delete pRoot;
								return;
							}

							JSONArray *pBoneRotation = (JSONArray*) pElement;

							if ( pBoneRotation->m_iNumElements > 0 )
							{
								pAGKBone->m_iNumRotations = pBoneRotation->m_iNumElements;
								pAGKBone->m_pRotations = new Anim2DKeyFrameRotation*[ pAGKBone->m_iNumRotations ];
								for ( UINT r = 0; r < pBoneRotation->m_iNumElements; r++ ) pAGKBone->m_pRotations[ r ] = 0;
							}

							for ( UINT r = 0; r < pBoneRotation->m_iNumElements; r++ )
							{
								pElement = pBoneRotation->GetElement( r );
								if ( pElement->GetType() != 1 )
								{
									agk::Error( "Failed to load Spine skeleton, JSON rotation keyframe is not an object" );
									delete pRoot;
									return;
								}

								JSONObject *pKeyFrame = (JSONObject*) pElement;
								pElement = pKeyFrame->GetElement( "curve" );
								if ( !pElement )
								{
									pAGKBone->m_pRotations[ r ] = new Anim2DKeyFrameRotation();
								}
								else
								{
									if ( pElement->GetType() == 3 && ((JSONString*)pElement)->m_sValue.CompareTo( "stepped" ) == 0 )
									{
										pAGKBone->m_pRotations[ r ] = new Anim2DKeyFrameRotationStepped();
									}
									else if ( pElement->GetType() == 2 )
									{
										JSONArray* pCurve = (JSONArray*) pElement;
										if ( pCurve->m_iNumElements != 4 )
										{
											agk::Error( "Failed to load Spine skeleton, JSON rotation keyframe curve must have 4 values" );
											delete pRoot;
											return;
										}

										if ( pCurve->GetElement(0)->GetType() != 4 || pCurve->GetElement(1)->GetType() != 4 
										  || pCurve->GetElement(2)->GetType() != 4 || pCurve->GetElement(3)->GetType() != 4 )
										{
											agk::Error( "Failed to load Spine skeleton, JSON rotation keyframe curve values must be numbers" );
											delete pRoot;
											return;
										}

										pAGKBone->m_pRotations[ r ] = new Anim2DKeyFrameRotationCurved();
										Anim2DKeyFrameRotationCurved *pAGKCurve = (Anim2DKeyFrameRotationCurved*) pAGKBone->m_pRotations[ r ];
										pAGKCurve->c1 = ((JSONNumber*)pCurve->GetElement( 0 ))->m_fValue;
										pAGKCurve->c2 = ((JSONNumber*)pCurve->GetElement( 1 ))->m_fValue;
										pAGKCurve->c3 = ((JSONNumber*)pCurve->GetElement( 2 ))->m_fValue;
										pAGKCurve->c4 = ((JSONNumber*)pCurve->GetElement( 3 ))->m_fValue;
									}
									else pAGKBone->m_pRotations[ r ] = new Anim2DKeyFrameRotation();
								}

								pElement = pKeyFrame->GetElement( "time" );
								if ( !pElement || pElement->GetType() != 4 )
								{
									agk::Error( "Failed to load Spine skeleton, JSON rotation keyframe must have a time value" );
									delete pRoot;
									return;
								}

								pAGKBone->m_pRotations[ r ]->m_fTime = ((JSONNumber*)pElement)->m_fValue;
								if ( pAGKBone->m_pRotations[ r ]->m_fTime > m_pAnimations[ i ].m_fTime ) m_pAnimations[ i ].m_fTime = pAGKBone->m_pRotations[ r ]->m_fTime;

								pElement = pKeyFrame->GetElement( "angle" );
								if ( !pElement || pElement->GetType() != 4 )
								{
									agk::Error( "Failed to load Spine skeleton, JSON rotation keyframe must have an angle value" );
									delete pRoot;
									return;
								}

								pAGKBone->m_pRotations[ r ]->m_fRotation = -((JSONNumber*)pElement)->m_fValue;
							}
						}

						// translation keyframes
						pElement = pAnimBone->GetElement( "translate" );
						if ( pElement )
						{
							if ( pElement->GetType() != 2 )
							{
								agk::Error( "Failed to load Spine skeleton, JSON animation bone translation is not an array" );
								delete pRoot;
								return;
							}

							JSONArray *pBoneTranslation = (JSONArray*) pElement;

							if ( pBoneTranslation->m_iNumElements > 0 )
							{
								pAGKBone->m_iNumPositions = pBoneTranslation->m_iNumElements;
								pAGKBone->m_pPositions = new Anim2DKeyFramePosition*[ pAGKBone->m_iNumPositions ];
								for ( UINT r = 0; r < pBoneTranslation->m_iNumElements; r++ ) pAGKBone->m_pPositions[ r ] = 0;
							}

							for ( UINT r = 0; r < pBoneTranslation->m_iNumElements; r++ )
							{
								pElement = pBoneTranslation->GetElement( r );
								if ( pElement->GetType() != 1 )
								{
									agk::Error( "Failed to load Spine skeleton, JSON translation keyframe is not an object" );
									delete pRoot;
									return;
								}

								JSONObject *pKeyFrame = (JSONObject*) pElement;
								pElement = pKeyFrame->GetElement( "curve" );
								if ( !pElement )
								{
									pAGKBone->m_pPositions[ r ] = new Anim2DKeyFramePosition();
								}
								else
								{
									if ( pElement->GetType() == 3 && ((JSONString*)pElement)->m_sValue.CompareTo( "stepped" ) == 0 )
									{
										pAGKBone->m_pPositions[ r ] = new Anim2DKeyFramePositionStepped();
									}
									else if ( pElement->GetType() == 2 )
									{
										JSONArray* pCurve = (JSONArray*) pElement;
										if ( pCurve->m_iNumElements != 4 )
										{
											agk::Error( "Failed to load Spine skeleton, JSON translation keyframe curve must have 4 values" );
											delete pRoot;
											return;
										}

										if ( pCurve->GetElement(0)->GetType() != 4 || pCurve->GetElement(1)->GetType() != 4 
										  || pCurve->GetElement(2)->GetType() != 4 || pCurve->GetElement(3)->GetType() != 4 )
										{
											agk::Error( "Failed to load Spine skeleton, JSON translation keyframe curve values must be numbers" );
											delete pRoot;
											return;
										}

										pAGKBone->m_pPositions[ r ] = new Anim2DKeyFramePositionCurved();
										Anim2DKeyFramePositionCurved *pAGKCurve = (Anim2DKeyFramePositionCurved*) pAGKBone->m_pPositions[ r ];
										pAGKCurve->c1 = ((JSONNumber*)pCurve->GetElement( 0 ))->m_fValue;
										pAGKCurve->c2 = ((JSONNumber*)pCurve->GetElement( 1 ))->m_fValue;
										pAGKCurve->c3 = ((JSONNumber*)pCurve->GetElement( 2 ))->m_fValue;
										pAGKCurve->c4 = ((JSONNumber*)pCurve->GetElement( 3 ))->m_fValue;
									}
									else pAGKBone->m_pPositions[ r ] = new Anim2DKeyFramePosition();
								}

								pElement = pKeyFrame->GetElement( "time" );
								if ( !pElement || pElement->GetType() != 4 )
								{
									agk::Error( "Failed to load Spine skeleton, JSON translation keyframe must have a time value" );
									delete pRoot;
									return;
								}

								pAGKBone->m_pPositions[ r ]->m_fTime = ((JSONNumber*)pElement)->m_fValue;
								if ( pAGKBone->m_pPositions[ r ]->m_fTime > m_pAnimations[ i ].m_fTime ) m_pAnimations[ i ].m_fTime = pAGKBone->m_pPositions[ r ]->m_fTime;

								pElement = pKeyFrame->GetElement( "x" );
								if ( !pElement || pElement->GetType() != 4 )
								{
									agk::Error( "Failed to load Spine skeleton, JSON translation keyframe must have an X value" );
									delete pRoot;
									return;
								}

								pAGKBone->m_pPositions[ r ]->m_fX = ((JSONNumber*)pElement)->m_fValue*scale;

								pElement = pKeyFrame->GetElement( "y" );
								if ( !pElement || pElement->GetType() != 4 )
								{
									agk::Error( "Failed to load Spine skeleton, JSON translation keyframe must have a Y value" );
									delete pRoot;
									return;
								}

								pAGKBone->m_pPositions[ r ]->m_fY = -((JSONNumber*)pElement)->m_fValue*scale;
							}
						}

						// scale keyframes
						pElement = pAnimBone->GetElement( "scale" );
						if ( pElement )
						{
							if ( pElement->GetType() != 2 )
							{
								agk::Error( "Failed to load Spine skeleton, JSON animation bone scale is not an array" );
								delete pRoot;
								return;
							}

							JSONArray *pBoneScale = (JSONArray*) pElement;

							if ( pBoneScale->m_iNumElements > 0 )
							{
								pAGKBone->m_iNumScales = pBoneScale->m_iNumElements;
								pAGKBone->m_pScales = new Anim2DKeyFrameScale*[ pAGKBone->m_iNumScales ];
								for ( UINT r = 0; r < pBoneScale->m_iNumElements; r++ ) pAGKBone->m_pScales[ r ] = 0;
							}

							for ( UINT r = 0; r < pBoneScale->m_iNumElements; r++ )
							{
								pElement = pBoneScale->GetElement( r );
								if ( pElement->GetType() != 1 )
								{
									agk::Error( "Failed to load Spine skeleton, JSON scale keyframe is not an object" );
									delete pRoot;
									return;
								}

								JSONObject *pKeyFrame = (JSONObject*) pElement;
								pElement = pKeyFrame->GetElement( "curve" );
								if ( !pElement )
								{
									pAGKBone->m_pScales[ r ] = new Anim2DKeyFrameScale();
								}
								else
								{
									if ( pElement->GetType() == 3 && ((JSONString*)pElement)->m_sValue.CompareTo( "stepped" ) == 0 )
									{
										pAGKBone->m_pScales[ r ] = new Anim2DKeyFrameScaleStepped();
									}
									else if ( pElement->GetType() == 2 )
									{
										JSONArray* pCurve = (JSONArray*) pElement;
										if ( pCurve->m_iNumElements != 4 )
										{
											agk::Error( "Failed to load Spine skeleton, JSON scale keyframe curve must have 4 values" );
											delete pRoot;
											return;
										}

										if ( pCurve->GetElement(0)->GetType() != 4 || pCurve->GetElement(1)->GetType() != 4 
										  || pCurve->GetElement(2)->GetType() != 4 || pCurve->GetElement(3)->GetType() != 4 )
										{
											agk::Error( "Failed to load Spine skeleton, JSON scale keyframe curve values must be numbers" );
											delete pRoot;
											return;
										}

										pAGKBone->m_pScales[ r ] = new Anim2DKeyFrameScaleCurved();
										Anim2DKeyFrameScaleCurved *pAGKCurve = (Anim2DKeyFrameScaleCurved*) pAGKBone->m_pScales[ r ];
										pAGKCurve->c1 = ((JSONNumber*)pCurve->GetElement( 0 ))->m_fValue;
										pAGKCurve->c2 = ((JSONNumber*)pCurve->GetElement( 1 ))->m_fValue;
										pAGKCurve->c3 = ((JSONNumber*)pCurve->GetElement( 2 ))->m_fValue;
										pAGKCurve->c4 = ((JSONNumber*)pCurve->GetElement( 3 ))->m_fValue;
									}
									else pAGKBone->m_pScales[ r ] = new Anim2DKeyFrameScale();
								}

								pElement = pKeyFrame->GetElement( "time" );
								if ( !pElement || pElement->GetType() != 4 )
								{
									agk::Error( "Failed to load Spine skeleton, JSON scale keyframe must have a time value" );
									delete pRoot;
									return;
								}

								pAGKBone->m_pScales[ r ]->m_fTime = ((JSONNumber*)pElement)->m_fValue;
								if ( pAGKBone->m_pScales[ r ]->m_fTime > m_pAnimations[ i ].m_fTime ) m_pAnimations[ i ].m_fTime = pAGKBone->m_pScales[ r ]->m_fTime;

								pElement = pKeyFrame->GetElement( "x" );
								if ( !pElement || pElement->GetType() != 4 )
								{
									agk::Error( "Failed to load Spine skeleton, JSON scale keyframe must have an X value" );
									delete pRoot;
									return;
								}

								pAGKBone->m_pScales[ r ]->m_fScaleX = ((JSONNumber*)pElement)->m_fValue;

								pElement = pKeyFrame->GetElement( "y" );
								if ( !pElement || pElement->GetType() != 4 )
								{
									agk::Error( "Failed to load Spine skeleton, JSON scale keyframe must have a Y value" );
									delete pRoot;
									return;
								}

								pAGKBone->m_pScales[ r ]->m_fScaleY = ((JSONNumber*)pElement)->m_fValue;
							}
						} // end keyframe loops

					}
				} // end bone loop

			}

			pElement = pAnimation->GetElement( "slots" );
			if ( pElement )
			{
				if ( pElement->GetType() != 1 )
				{
					agk::Error( "Failed to load Spine skeleton, JSON slot animation is not an object" );
					delete pRoot;
					return;
				}

				JSONObject *pSlotAnims = (JSONObject*) pElement;
				if ( pSlotAnims->m_iNumPairs > 0 )
				{
					m_pAnimations[ i ].m_iNumSlots = pSlotAnims->m_iNumPairs;
					m_pAnimations[ i ].m_pSlotAnims = new Anim2DSlot[ pSlotAnims->m_iNumPairs ];
				}

				for ( UINT s = 0; s < pSlotAnims->m_iNumPairs; s++ )
				{
					pElement = pSlotAnims->m_pPairs[ s ]->m_pElement;
					Slot2D *pSlot = GetSlot( pSlotAnims->m_pPairs[ s ]->m_sName );
					if ( !pSlot )
					{
						agk::Warning( "Warning when load Spine skeleton, animation found for slot that doesn't exist" );
					}
					else
					{
						if ( pElement->GetType() != 1 )
						{
							agk::Error( "Failed to load Spine skeleton, JSON animation slot is not an object" );
							delete pRoot;
							return;
						}

						Anim2DSlot *pAGKSlot = m_pAnimations[ i ].m_pSlotAnims + s;
						pAGKSlot->m_sSlotName.SetStr( pSlot->m_sName );
						pAGKSlot->m_iSlotIndex = GetSlotIndex( pSlotAnims->m_pPairs[ s ]->m_sName );

						JSONObject *pAnimSlot = (JSONObject*) pElement;

						// color key frames
						pElement = pAnimSlot->GetElement( "color" );
						if ( pElement )
						{
							if ( pElement->GetType() != 2 )
							{
								agk::Error( "Failed to load Spine skeleton, JSON animation slot color is not an array" );
								delete pRoot;
								return;
							}

							JSONArray *pSlotColor = (JSONArray*) pElement;

							if ( pSlotColor->m_iNumElements > 0 )
							{
								pAGKSlot->m_iNumColors = pSlotColor->m_iNumElements;
								pAGKSlot->m_pColors = new Anim2DKeyFrameColor*[ pAGKSlot->m_iNumColors ];
								for ( UINT c = 0; c < pSlotColor->m_iNumElements; c++ ) pAGKSlot->m_pColors[ c ] = 0;
							}

							for ( UINT c = 0; c < pSlotColor->m_iNumElements; c++ )
							{
								pElement = pSlotColor->GetElement( c );
								if ( pElement->GetType() != 1 )
								{
									agk::Error( "Failed to load Spine skeleton, JSON color keyframe is not an object" );
									delete pRoot;
									return;
								}

								JSONObject *pKeyFrame = (JSONObject*) pElement;
								pElement = pKeyFrame->GetElement( "curve" );
								if ( !pElement )
								{
									pAGKSlot->m_pColors[ c ] = new Anim2DKeyFrameColor();
								}
								else
								{
									if ( pElement->GetType() == 3 && ((JSONString*)pElement)->m_sValue.CompareTo( "stepped" ) == 0 )
									{
										pAGKSlot->m_pColors[ c ] = new Anim2DKeyFrameColorStepped();
									}
									else if ( pElement->GetType() == 2 )
									{
										JSONArray* pCurve = (JSONArray*) pElement;
										if ( pCurve->m_iNumElements != 4 )
										{
											agk::Error( "Failed to load Spine skeleton, JSON color keyframe curve must have 4 values" );
											delete pRoot;
											return;
										}

										if ( pCurve->GetElement(0)->GetType() != 4 || pCurve->GetElement(1)->GetType() != 4 
										  || pCurve->GetElement(2)->GetType() != 4 || pCurve->GetElement(3)->GetType() != 4 )
										{
											agk::Error( "Failed to load Spine skeleton, JSON color keyframe curve values must be numbers" );
											delete pRoot;
											return;
										}

										pAGKSlot->m_pColors[ c ] = new Anim2DKeyFrameColorCurved();
										Anim2DKeyFrameColorCurved *pAGKCurve = (Anim2DKeyFrameColorCurved*) pAGKSlot->m_pColors[ c ];
										pAGKCurve->c1 = ((JSONNumber*)pCurve->GetElement( 0 ))->m_fValue;
										pAGKCurve->c2 = ((JSONNumber*)pCurve->GetElement( 1 ))->m_fValue;
										pAGKCurve->c3 = ((JSONNumber*)pCurve->GetElement( 2 ))->m_fValue;
										pAGKCurve->c4 = ((JSONNumber*)pCurve->GetElement( 3 ))->m_fValue;
									}
									else pAGKSlot->m_pColors[ c ] = new Anim2DKeyFrameColor();
								}

								pElement = pKeyFrame->GetElement( "time" );
								if ( !pElement || pElement->GetType() != 4 )
								{
									agk::Error( "Failed to load Spine skeleton, JSON color keyframe must have a time value" );
									delete pRoot;
									return;
								}

								pAGKSlot->m_pColors[ c ]->m_fTime = ((JSONNumber*)pElement)->m_fValue;
								if ( pAGKSlot->m_pColors[ c ]->m_fTime > m_pAnimations[ i ].m_fTime ) m_pAnimations[ i ].m_fTime = pAGKSlot->m_pColors[ c ]->m_fTime;

								pElement = pKeyFrame->GetElement( "color" );
								if ( !pElement || pElement->GetType() != 3 )
								{
									agk::Error( "Failed to load Spine skeleton, JSON color keyframe must have an color value" );
									delete pRoot;
									return;
								}

								UINT color = 0;
								sscanf( ((JSONString*)pElement)->m_sValue, "%x", &color );
								pAGKSlot->m_pColors[ c ]->m_iAlpha = (unsigned char) (color & 0xff);
								pAGKSlot->m_pColors[ c ]->m_iBlue = (unsigned char) ((color >> 8) & 0xff);
								pAGKSlot->m_pColors[ c ]->m_iGreen = (unsigned char) ((color >> 16) & 0xff);
								pAGKSlot->m_pColors[ c ]->m_iRed = (unsigned char) ((color >> 24) & 0xff);
							}
						}

						// attachment keyframes
						pElement = pAnimSlot->GetElement( "attachment" );
						if ( pElement )
						{
							if ( pElement->GetType() != 2 )
							{
								agk::Error( "Failed to load Spine skeleton, JSON animation bone translation is not an array" );
								delete pRoot;
								return;
							}

							JSONArray *pSlotAttachment = (JSONArray*) pElement;

							if ( pSlotAttachment->m_iNumElements > 0 )
							{
								pAGKSlot->m_iNumAttachments = pSlotAttachment->m_iNumElements;
								pAGKSlot->m_pAttachments = new Anim2DKeyFrameAttachment*[ pAGKSlot->m_iNumAttachments ];
								for ( UINT a = 0; a < pSlotAttachment->m_iNumElements; a++ ) pAGKSlot->m_pAttachments[ a ] = 0;
							}

							for ( UINT a = 0; a < pSlotAttachment->m_iNumElements; a++ )
							{
								pElement = pSlotAttachment->GetElement( a );
								if ( pElement->GetType() != 1 )
								{
									agk::Error( "Failed to load Spine skeleton, JSON attachment keyframe is not an object" );
									delete pRoot;
									return;
								}

								JSONObject *pKeyFrame = (JSONObject*) pElement;
								pAGKSlot->m_pAttachments[ a ] = new Anim2DKeyFrameAttachment();
								
								pElement = pKeyFrame->GetElement( "time" );
								if ( !pElement || pElement->GetType() != 4 )
								{
									agk::Error( "Failed to load Spine skeleton, JSON attachment keyframe must have a time value" );
									delete pRoot;
									return;
								}

								pAGKSlot->m_pAttachments[ a ]->m_fTime = ((JSONNumber*)pElement)->m_fValue;
								if ( pAGKSlot->m_pAttachments[ a ]->m_fTime > m_pAnimations[ i ].m_fTime ) m_pAnimations[ i ].m_fTime = pAGKSlot->m_pAttachments[ a ]->m_fTime;

								pElement = pKeyFrame->GetElement( "name" );
								if ( !pElement || pElement->GetType() != 3 )
								{
									//agk::Error( "Failed to load Spine skeleton, JSON attachment keyframe must have a name" );
									//goto Exit;
									pAGKSlot->m_pAttachments[ a ]->m_sSpriteName.SetStr( "" );
									pAGKSlot->m_pAttachments[ a ]->m_pSprite = 0;
								}
								else
								{
									pAGKSlot->m_pAttachments[ a ]->m_sSpriteName.SetStr( pSlotAnims->m_pPairs[ s ]->m_sName );
									pAGKSlot->m_pAttachments[ a ]->m_sSpriteName.Append( ":" );
									pAGKSlot->m_pAttachments[ a ]->m_sSpriteName.Append( ((JSONString*)pElement)->m_sValue );

									pAGKSlot->m_pAttachments[ a ]->m_pSprite = GetSprite( pAGKSlot->m_pAttachments[ a ]->m_sSpriteName );
								}
							}
						} // end key frames loop

					}
				} // end slot loop

			}
		} // end animations loop

	}

	delete pRoot;
}

void Skeleton2D::LoadFromSpriter( const char* filename, float scale, cImage *pAtlas )
{
	uString sRootPath( filename );
	sRootPath.Replace( '\\', '/' );
	int index = sRootPath.RevFind( '/' );
	if ( index > 0 )
	{
		sRootPath.Trunc( '/' );
		sRootPath.AppendAscii( '/' );
	}
	else
	{
		sRootPath.SetStr( "" );
	}

	JSONElement *pRoot = JSONElement::LoadJSONFromFile( filename );
	if ( !pRoot ) return;
	
	if ( pRoot->GetType() != 1 )
	{
		agk::Error( "Failed to load Spriter skeleton, JSON root is not an object" );
		delete pRoot;
		return;
	}

	// count sprite files
	JSONObject *pObject = (JSONObject*) pRoot;
	JSONElement *pElement = pObject->GetElement( "folder" );
	if ( !pElement || pElement->GetType() != 2 )
	{
		agk::Error( "Failed to load Spriter skeleton, folder element is missing or is not an array" );
		delete pRoot;
		return;
	}

	JSONArray *pFolder = (JSONArray*) pElement;
	if ( pFolder->m_iNumElements < 1 )
	{
		agk::Error( "Failed to load Spriter skeleton, no sprite folders found" );
		delete pRoot;
		return;
	}

	int numFolders = pFolder->m_iNumElements;
	int maxFiles = 0;
	int totalFiles = 0;
	for ( UINT i = 0; i < pFolder->m_iNumElements; i++ )
	{
		pElement = pFolder->GetElement( i );
		if ( !pElement || pElement->GetType() != 1 )
		{
			agk::Error( "Failed to load Spriter skeleton, folder is not a JSON object" );
			delete pRoot;
			return;
		}

		pElement = ((JSONObject*)pElement)->GetElement( "file" );
		if ( !pElement || pElement->GetType() != 2 )
		{
			agk::Error( "Failed to load Spriter skeleton, file element is missing or is not an array" );
			delete pRoot;
			return;
		}

		JSONArray* pFiles = (JSONArray*) pElement;

		totalFiles += pFiles->m_iNumElements;
		if ( (int)pFiles->m_iNumElements > maxFiles ) maxFiles = pFiles->m_iNumElements;
	}

	if ( maxFiles <= 0 )
	{
		agk::Error( "Failed to load Spriter skeleton, no sprite files found" );
		delete pRoot;
		return;
	}

	m_iNumSprites = totalFiles;
	m_pSprites = new cSprite[ totalFiles ];
	for ( int i = 0; i < totalFiles; i++ ) 
	{
		m_pSprites[ i ].SetVisible( false );
		m_pSprites[ i ].SetTransparency( 1 );
	}

	cSprite ***pFolderFiles = new cSprite**[ numFolders ];
	for ( int i = 0; i < numFolders; i++ )
	{
		pFolderFiles[ i ] = new cSprite*[ maxFiles ];
		for ( int j = 0; j < maxFiles; j++ )
		{
			pFolderFiles[ i ][ j ] = 0;
		}
	}

	// load sprite files
	int count = 0;
	for ( UINT i = 0; i < pFolder->m_iNumElements; i++ )
	{
		pElement = pFolder->GetElement( i );
		pElement = ((JSONObject*)pElement)->GetElement( "file" );
		JSONArray* pFiles = (JSONArray*) pElement;

		for ( UINT j = 0; j < pFiles->m_iNumElements; j++ )
		{
			pElement = pFiles->GetElement( j );
			if ( pElement->GetType() != 1 )
			{
				agk::Error( "Failed to load Spriter skeleton, files is not a JSON object" );
				delete pRoot;
				for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
				delete [] pFolderFiles;
				return;
			}

			JSONObject* pFile = (JSONObject*) pElement;

			// sprite filename
			pElement = pFile->GetElement( "name" );
			if ( pElement && pElement->GetType() == 3 )
			{
				uString filename( sRootPath );
				filename.Append( ((JSONString*)pElement)->m_sValue );

				cImage *pImage = new cImage();
				if ( !pImage->Load( filename ) )
				{
					delete pImage;
					uString err;
					err.Format( "Failed to load Spriter skeleton, image %s failed to load", ((JSONString*)pElement)->m_sValue.GetStr() );
					agk::Error( err );
					delete pRoot;
					for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
					delete [] pFolderFiles;
					return;
				}
				m_pSprites[ count ].SetImage( pImage );
			}

			// get sprite size
			pElement = pFile->GetElement( "width" );
			if ( pElement && pElement->GetType() == 4 )
			{
				float width = ((JSONNumber*)pElement)->m_fValue*scale;
				pElement = pFile->GetElement( "height" );
				if ( pElement && pElement->GetType() == 4 )
				{
					float height = ((JSONNumber*)pElement)->m_fValue*scale;
					m_pSprites[ count ].SetSize( width, height );
				}
			}

			float offsetX = 0.5f;
			pElement = pFile->GetElement( "pivot_x" );
			if ( pElement && pElement->GetType() == 4 ) offsetX = ((JSONNumber*)pElement)->m_fValue;

			float offsetY = 0.5f;
			pElement = pFile->GetElement( "pivot_y" ); 
			if ( pElement && pElement->GetType() == 4 ) offsetY = 1 - ((JSONNumber*)pElement)->m_fValue;
			m_pSprites[ count ].SetOffset( m_pSprites[count].GetWidth()*offsetX, m_pSprites[count].GetHeight()*offsetY );
			
			pFolderFiles[ i ][ j ] = &(m_pSprites[ count ]);

			count++;
		}
	}

	// entity object
	pObject = (JSONObject*) pRoot;
	pElement = pObject->GetElement( "entity" );
	if ( !pElement || pElement->GetType() != 2 )
	{
		agk::Error( "Failed to load Spriter skeleton, JSON entity is missing or not an array" );
		delete pRoot;
		for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
		delete [] pFolderFiles;
		return;
	}

	JSONArray *pEntities = (JSONArray*) pElement;
	if ( pEntities->m_iNumElements < 1 )
	{
		agk::Error( "Failed to load Spriter skeleton, no entities found" );
		delete pRoot;
		for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
		delete [] pFolderFiles;
		return;
	}

	if ( pEntities->m_iNumElements > 1 )
	{
		agk::Warning( "Spriter skeleton contains more than one entity, AGK will only load the first entity" );
	}

	pElement = pEntities->GetElement( 0 );
	if ( pElement->GetType() != 1 )
	{
		agk::Error( "Failed to load Spriter skeleton, entity 0 is not an object" );
		delete pRoot;
		for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
		delete [] pFolderFiles;
		return;
	}

	pObject = (JSONObject*) pElement;

	// obj_info bones
	pElement = pObject->GetElement( "obj_info" );
	if ( !pElement || pElement->GetType() != 2 )
	{
		agk::Error( "Failed to load Spriter skeleton, JSON entity is missing or not an array" );
		delete pRoot;
		for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
		delete [] pFolderFiles;
		return;
	}

	JSONArray *pObjInfo = (JSONArray*) pElement;
	if ( pObjInfo->m_iNumElements < 1 )
	{
		agk::Error( "Failed to load Spriter skeleton, no bones found" );
		delete pRoot;
		for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
		delete [] pFolderFiles;
		return;
	}

	m_iNumBones = pObjInfo->m_iNumElements;
	m_pBones = new Bone2D[ pObjInfo->m_iNumElements ];
	for ( UINT i = 0; i < pObjInfo->m_iNumElements; i++ )
	{
		pElement = pObjInfo->GetElement( i );
		if ( pElement->GetType() != 1 )
		{
			agk::Error( "Failed to load Spriter skeleton, bone is not an object" );
			delete pRoot;
			for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
			delete [] pFolderFiles;
			return;
		}

		JSONObject *pBone = (JSONObject*) pElement;
		if ( pBone->m_iNumPairs < 1 )
		{
			agk::Error( "Failed to load Spriter skeleton, bone has no elements" );
			delete pRoot;
			for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
			delete [] pFolderFiles;
			return;
		}

		pElement = pBone->GetElement( "type" );
		if ( !pElement || pElement->GetType() != 3 ) 
		{
			continue;
		}

		if ( ((JSONString*)pElement)->m_sValue.CompareCaseTo("bone") != 0 )
		{
			continue;
		}

		pElement = pBone->GetElement( "name" );
		if ( !pElement ) 
		{
			agk::Error( "Failed to load Spriter skeleton, bone has no name" );
			delete pRoot;
			for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
			delete [] pFolderFiles;
			return;
		}

		if ( pElement->GetType() != 3 )
		{
			agk::Error( "Failed to load Spriter skeleton, bone name is not a string" );
			delete pRoot;
			for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
			delete [] pFolderFiles;
			return;
		}

		m_pBones[ i ].length = 200;
		m_pBones[ i ].m_sName.SetStr( ((JSONString*)pElement)->m_sValue );
		//m_pBones[ i ].m_bFlags &= ~AGK_BONE_INHERIT_SCALE;
		m_pBones[ i ].m_bFlags |= AGK_BONE_PRE_SCALE;

		// bone parents and default positions are not stored with the bone name, must load a frame of animation to get that info
	}

	// all other information is in the animation data and can technically change per frame
	// AGK will enforce that bone parents and number of images per bone cannot change per frame

	pElement = pObject->GetElement( "animation" );
	if ( !pElement || pElement->GetType() != 2 )
	{
		agk::Error( "Failed to load Spriter skeleton, JSON entity has no animation data. Spriter stores its bone heirarchy in the animation data" );
		delete pRoot;
		for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
		delete [] pFolderFiles;
		return;
	}

	JSONArray *pAnimations = (JSONArray*) pElement;
	if ( pAnimations->m_iNumElements < 1 )
	{
		agk::Error( "Failed to load Spriter skeleton, no animations found" );
		delete pRoot;
		for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
		delete [] pFolderFiles;
		return;
	}

	// find the first animation
	pElement = pAnimations->GetElement( 0 );
	if ( pElement->GetType() != 1 )
	{
		agk::Error( "Failed to load Spriter skeleton, animation is not a JSON object" );
		delete pRoot;
		for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
		delete [] pFolderFiles;
		return;
	}

	JSONObject *pAnim0 = (JSONObject*) pElement;
	if ( pAnim0->m_iNumPairs < 1 )
	{
		agk::Error( "Failed to load Spriter skeleton, animation has no elements" );
		delete pRoot;
		for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
		delete [] pFolderFiles;
		return;
	}

	uString sFirstAnimName;
	pElement = pAnim0->GetElement( "name" );
	if ( !pElement || pElement->GetType() != 3 )
	{
		agk::Error( "Failed to load Spriter skeleton, animation 0 has no name" );
		delete pRoot;
		for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
		delete [] pFolderFiles;
		return;
	}

	sFirstAnimName.SetStr( ((JSONString*)pElement)->m_sValue );

	// look for the mainline of this animation
	pElement = pAnim0->GetElement( "mainline" );
	if ( !pElement || pElement->GetType() != 1 )
	{
		agk::Error( "Failed to load Spriter skeleton, animation 0 has no mainline object" );
		delete pRoot;
		for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
		delete [] pFolderFiles;
		return;
	}

	JSONObject *pAnim0Mainline = (JSONObject*) pElement;
	if ( pAnim0Mainline->m_iNumPairs < 1 )
	{
		agk::Error( "Failed to load Spriter skeleton, animation mainline has no elements" );
		delete pRoot;
		for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
		delete [] pFolderFiles;
		return;
	}

	// look for timeline of this animation
	pElement = pAnim0->GetElement( "timeline" );
	if ( !pElement || pElement->GetType() != 2 )
	{
		agk::Error( "Failed to load Spriter skeleton, animation 0 has no timeline array" );
		delete pRoot;
		for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
		delete [] pFolderFiles;
		return;
	}

	JSONArray *pAnim0Timeline = (JSONArray*) pElement;
	if ( pAnim0Timeline->m_iNumElements < 1 )
	{
		agk::Error( "Failed to load Spriter skeleton, animation timeline has no elements" );
		delete pRoot;
		for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
		delete [] pFolderFiles;
		return;
	}

	// find the mainline keys
	pElement = pAnim0Mainline->GetElement( "key" );
	if ( !pElement || pElement->GetType() != 2 )
	{
		agk::Error( "Failed to load Spriter skeleton, animation mainline has no key array" );
		delete pRoot;
		for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
		delete [] pFolderFiles;
		return;
	}

	JSONArray *pFirstKeys = (JSONArray*) pElement;
	if ( pFirstKeys->m_iNumElements < 1 )
	{
		agk::Error( "Failed to load Spriter skeleton, animation mainline key array has no elements" );
		delete pRoot;
		for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
		delete [] pFolderFiles;
		return;
	}

	// load the first key
	pElement = pFirstKeys->GetElement( 0 );
	if ( pElement->GetType() != 1 )
	{
		agk::Error( "Failed to load Spriter skeleton, mainline key is not an object" );
		delete pRoot;
		for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
		delete [] pFolderFiles;
		return;
	}

	// load bone refs from the first animation key to get hierarchy information
	pElement = ((JSONObject*)pElement)->GetElement( "bone_ref" );
	if ( !pElement || pElement->GetType() != 2 )
	{
		agk::Error( "Failed to load Spriter skeleton, mainline key bone_ref is not an array" );
		delete pRoot;
		for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
		delete [] pFolderFiles;
		return;
	}

	JSONArray *pBoneRefs = (JSONArray*) pElement;
	if ( pBoneRefs->m_iNumElements < 1 )
	{
		agk::Error( "Failed to load Spriter skeleton, bone_ref keys has no elements" );
		delete pRoot;
		for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
		delete [] pFolderFiles;
		return;
	}

	if ( pBoneRefs->m_iNumElements > m_iNumBones )
	{
		agk::Error( "Failed to load Spriter skeleton, animation contains more bone references than there are bones" );
		delete pRoot;
		for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
		delete [] pFolderFiles;
		return;
	}

	int *pMainlineBoneIDs = new int[ m_iNumBones ];
	for ( UINT i = 0; i < m_iNumBones; i++ ) pMainlineBoneIDs[ i ] = -1;

	for ( UINT i = 0; i < pBoneRefs->m_iNumElements; i++ )
	{
		pElement = pBoneRefs->GetElement( i );
		if ( pElement->GetType() != 1 )
		{
			agk::Error( "Failed to load Spriter skeleton, bone reference is not a JSON object" );
			delete pRoot;
			for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
			delete [] pFolderFiles;
			delete [] pMainlineBoneIDs;
			return;
		}

		int timelineID = 0;
		JSONNumber *pTimelineNumber = (JSONNumber*)((JSONObject*)pElement)->GetElement( "timeline" );
		if ( !pTimelineNumber || pTimelineNumber->GetType() != 4 )
		{
			agk::Error( "Failed to load Spriter skeleton, bone reference does not have a timeline" );
			delete pRoot;
			for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
			delete [] pFolderFiles;
			delete [] pMainlineBoneIDs;
			return;
		}

		timelineID = pTimelineNumber->m_iValue;
		if ( timelineID < 0 || timelineID >= (int)pAnim0Timeline->m_iNumElements )
		{
			agk::Error( "Failed to load Spriter skeleton, bone reference timeline is out of range" );
			delete pRoot;
			for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
			delete [] pFolderFiles;
			delete [] pMainlineBoneIDs;
			return;
		}

		JSONObject* pBoneTimeline = (JSONObject*) pAnim0Timeline->GetElement( timelineID );
		JSONNumber* pBoneNumber = (JSONNumber*)pBoneTimeline->GetElement( "obj" );
		if ( !pBoneNumber || pBoneNumber->GetType() != 4 )
		{
			agk::Error( "Failed to load Spriter skeleton, bone reference timeline doesn't have a bone ID" );
			delete pRoot;
			for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
			delete [] pFolderFiles;
			delete [] pMainlineBoneIDs;
			return;
		}

		int boneID = pBoneNumber->m_iValue;
		if ( boneID < 0 || boneID >= (int)m_iNumBones )
		{
			agk::Error( "Failed to load Spriter skeleton, bone ID is out of range" );
			delete pRoot;
			for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
			delete [] pFolderFiles;
			delete [] pMainlineBoneIDs;
			return;
		}

		pMainlineBoneIDs[ i ] = boneID;

		JSONNumber *pParentNumber = (JSONNumber*) ((JSONObject*)pElement)->GetElement( "parent" );
		if ( !pParentNumber || pParentNumber->GetType() != 4 )
		{
			m_pBones[ boneID ].m_bFlags |= AGK_BONE_ROOT;
		}
		else
		{
			int parent = pMainlineBoneIDs[ pParentNumber->m_iValue ];
			if ( parent < 0 || parent >= (int)m_iNumBones )
			{
				agk::Error( "Failed to load Spriter skeleton, bone reference parent is out of range" );
				delete pRoot;
				for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
				delete [] pFolderFiles;
				delete [] pMainlineBoneIDs;
				return;
			}

			m_pBones[ boneID ].m_pParent = &(m_pBones[parent]);
		}
	}

	// load object_ref from first key to get slot information
	pElement = ((JSONObject*)(pFirstKeys->GetElement( 0 )))->GetElement( "object_ref" );
	if ( !pElement || pElement->GetType() != 2 )
	{
		agk::Error( "Failed to load Spriter skeleton, mainline key object_ref is not an array" );
		delete pRoot;
		for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
		delete [] pFolderFiles;
		delete [] pMainlineBoneIDs;
		return;
	}

	JSONArray *pObjRefs = (JSONArray*) pElement;
	if ( pObjRefs->m_iNumElements < 1 )
	{
		agk::Error( "Failed to load Spriter skeleton, object_ref keys has no elements" );
		delete pRoot;
		for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
		delete [] pFolderFiles;
		delete [] pMainlineBoneIDs;
		return;
	}

	m_iNumSlots = pObjRefs->m_iNumElements;
	m_pSlots = new Slot2D[ m_iNumSlots ];
	for ( UINT i = 0; i < pObjRefs->m_iNumElements; i++ )
	{
		pElement = pObjRefs->GetElement( i );
		if ( pElement->GetType() != 1 )
		{
			agk::Error( "Failed to load Spriter skeleton, object reference is not a JSON object" );
			delete pRoot;
			for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
			delete [] pFolderFiles;
			delete [] pMainlineBoneIDs;
			return;
		}

		JSONNumber *pParentNum = (JSONNumber*) ((JSONObject*)pElement)->GetElement( "parent" );
		if ( pElement && pElement->GetType() == 4 )
		{
			int parent = pMainlineBoneIDs[ pParentNum->m_iValue ];
			if ( parent < 0 || parent >= (int)m_iNumBones )
			{
				agk::Error( "Failed to load Spriter skeleton, object reference parent is out of range" );
				delete pRoot;
				for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
				delete [] pFolderFiles;
				delete [] pMainlineBoneIDs;
				return;
			}

			m_pSlots[ i ].m_pParent = &(m_pBones[parent]);
		}

		m_pSlots[ i ].m_iColor = 0xffffffff;
		m_pSlots[ i ].m_iOrigColor = m_pSlots[ i ].m_iColor;
		m_pSlots[ i ].m_pOrigBone = m_pSlots[ i ].m_pParent;
		m_pSlots[ i ].m_iZOrder = i;

		// get timeline to find default sprite for this slot
		pElement = ((JSONObject*)pObjRefs->GetElement( i ))->GetElement( "timeline" );
		if ( pElement && pElement->GetType() == 3 )
		{
			int timeline = ((JSONString*)pElement)->m_sValue.ToInt();
			if ( timeline < 0 || timeline >= (int)pAnim0Timeline->m_iNumElements )
			{
				agk::Error( "Failed to load Spriter skeleton, object reference timeline is out of range" );
				delete pRoot;
				for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
				delete [] pFolderFiles;
				delete [] pMainlineBoneIDs;
				return;
			}

			pElement = pAnim0Timeline->GetElement( timeline );
			if ( pElement->GetType() != 1 )
			{
				agk::Error( "Failed to load Spriter skeleton, timeline entry is not an object" );
				delete pRoot;
				for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
				delete [] pFolderFiles;
				delete [] pMainlineBoneIDs;
				return;
			}

			JSONObject *pTimeline = (JSONObject*) pElement;
			pElement = pTimeline->GetElement( "key" );
			if ( !pElement || pElement->GetType() != 2 )
			{
				agk::Error( "Failed to load Spriter skeleton, timeline does not contain any keys, or keys is not an array" );
				delete pRoot;
				for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
				delete [] pFolderFiles;
				delete [] pMainlineBoneIDs;
				return;
			}

			JSONArray *pTimelineKeys = (JSONArray*) pElement;
			if ( pTimelineKeys->m_iNumElements < 1 )
			{
				agk::Error( "Failed to load Spriter skeleton, timeline has 0 keys" );
				delete pRoot;
				for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
				delete [] pFolderFiles;
				delete [] pMainlineBoneIDs;
				return;
			}

			// load first key to get default slot sprite
			pElement = pTimelineKeys->GetElement( 0 );
			if ( pElement->GetType() != 1 )
			{
				agk::Error( "Failed to load Spriter skeleton, timeline key is not an object" );
				delete pRoot;
				for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
				delete [] pFolderFiles;
				delete [] pMainlineBoneIDs;
				return;
			}

			JSONObject* pTimelineKey = (JSONObject*) pElement;
			pElement = pTimelineKey->GetElement( "object" );
			if ( !pElement || pElement->GetType() != 1 )
			{
				agk::Error( "Failed to load Spriter skeleton, timeline key does not contain an object" );
				delete pRoot;
				for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
				delete [] pFolderFiles;
				delete [] pMainlineBoneIDs;
				return;
			}

			JSONObject* pTimelineKeyObj = (JSONObject*) pElement;

			int folder = -1;
			pElement = pTimelineKeyObj->GetElement( "folder" );
			if ( pElement && pElement->GetType() == 4 ) folder = agk::Round( ((JSONNumber*)pElement)->m_fValue );

			int file = -1;
			pElement = pTimelineKeyObj->GetElement( "file" );
			if ( pElement && pElement->GetType() == 4 ) file = agk::Round( ((JSONNumber*)pElement)->m_fValue );

			float angle = 0;
			pElement = pTimelineKeyObj->GetElement( "angle" );
			if ( pElement && pElement->GetType() == 4 ) angle = -((JSONNumber*)pElement)->m_fValue;
			m_pSlots[ i ].angle = angle;

			float posX = 0;
			pElement = pTimelineKeyObj->GetElement( "x" );
			if ( pElement && pElement->GetType() == 4 ) posX = ((JSONNumber*)pElement)->m_fValue*scale;
			m_pSlots[ i ].x = posX;

			float posY = 0;
			pElement = pTimelineKeyObj->GetElement( "y" );
			if ( pElement && pElement->GetType() == 4 ) posY = -((JSONNumber*)pElement)->m_fValue*scale;
			m_pSlots[ i ].y = posY;

			float scaleX = 1;
			pElement = pTimelineKeyObj->GetElement( "scale_x" );
			if ( pElement && pElement->GetType() == 4 ) scaleX = ((JSONNumber*)pElement)->m_fValue;
			m_pSlots[ i ].sX = scaleX;

			float scaleY = 1;
			pElement = pTimelineKeyObj->GetElement( "scale_y" );
			if ( pElement && pElement->GetType() == 4 ) scaleY = ((JSONNumber*)pElement)->m_fValue;
			m_pSlots[ i ].sY = scaleY;

			if ( file >= 0 && folder >= 0 )
			{
				m_pSlots[ i ].m_pOrigSprite = pFolderFiles[ folder ][ file ];
				m_pSlots[ i ].m_pSprite = pFolderFiles[ folder ][ file ];
				if ( m_pSlots[ i ].m_pSprite ) 
				{
					m_pSlots[ i ].m_pSprite->SetBone( m_pSlots[ i ].m_pParent );
					m_pSlots[ i ].m_pSprite->SetVisible( true );
					m_pSlots[ i ].m_pSprite->SetColor( 255,255,255,255 );

					m_pSlots[ i ].m_pSprite->SetAngle( angle );
					m_pSlots[ i ].m_pSprite->SetPositionByOffset( posX, posY );
					m_pSlots[ i ].m_pSprite->SetScaleByOffset( scaleX, scaleY );
				}
			}
		}
	}

	delete [] pMainlineBoneIDs;
	pMainlineBoneIDs = 0;
	
	if ( pAnimations->m_iNumElements < 1 )
	{
		agk::Error( "Failed to load Spriter skeleton, no animations found" );
		delete pRoot;
		for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
		delete [] pFolderFiles;
		return;
	}

	m_iNumAnimations = pAnimations->m_iNumElements;
	m_pAnimations = new Animation2D[ m_iNumAnimations ];
	for ( UINT i = 0; i < m_iNumAnimations; i++ )
	{
		pElement = pAnimations->GetElement( i );
		if ( pElement->GetType() != 1 )
		{
			agk::Error( "Failed to load Spriter skeleton, animation entry is not an object" );
			delete pRoot;
			for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
			delete [] pFolderFiles;
			return;
		}

		JSONObject *pAnimEntry = (JSONObject*) pElement;
		pElement = pAnimEntry->GetElement( "name" );
		if ( pElement && pElement->GetType() == 3 )
		{
			m_pAnimations[ i ].m_sName.SetStr( ((JSONString*)pElement)->m_sValue );
		}

		// get total length
		pElement = pAnimEntry->GetElement( "length" );
		if ( !pElement || pElement->GetType() != 4 )
		{
			agk::Error( "Failed to load Spriter skeleton, animation entry does not have a length" );
			delete pRoot;
			for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
			delete [] pFolderFiles;
			return;
		}
		m_pAnimations[ i ].m_fTime = ((JSONNumber*)pElement)->m_fValue / 1000.0f;

		// get mainline keys
		pElement = pAnimEntry->GetElement( "mainline" );
		if ( !pElement || pElement->GetType() != 1 )
		{
			agk::Error( "Failed to load Spriter skeleton, animation entry does not have a mainline or the mainline is not an object" );
			delete pRoot;
			for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
			delete [] pFolderFiles;
			return;
		}

		JSONObject *pMainline = (JSONObject*) pElement;

		pElement = pMainline->GetElement( "key" );
		if ( !pElement || pElement->GetType() != 2 )
		{
			agk::Error( "Failed to load Spriter skeleton, animation mainline does not contain any keys or the keys are not an array" );
			delete pRoot;
			for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
			delete [] pFolderFiles;
			return;
		}

		JSONArray *pMainlineKeys = (JSONArray*) pElement;
		if ( pMainlineKeys->m_iNumElements < 1 ) continue;

		//get timeline
		pElement = pAnimEntry->GetElement( "timeline" );
		if ( !pElement || pElement->GetType() != 2 )
		{
			agk::Error( "Failed to load Spriter skeleton, animation entry does not have a timeline or the timeline is not an array" );
			delete pRoot;
			for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
			delete [] pFolderFiles;
			return;
		}

		JSONArray *pTimeline = (JSONArray*) pElement;

		// setup bone variables
		pElement = pMainlineKeys->GetElement( 0 );
		JSONArray *pBoneRefArray = (JSONArray*) ((JSONObject*)pElement)->GetElement( "bone_ref" );
		int numBoneRefs = pBoneRefArray->m_iNumElements;
		if ( numBoneRefs > (int)m_iNumBones )
		{
			agk::Error( "Failed to load Spriter skeleton, animation mainline contains more bone references than there are bones. Make sure your heirarchy contains the same structure across all animations, use the alpha setting to hide items rather than removing them from the heirarchy" );
			delete pRoot;
			for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
			delete [] pFolderFiles;
			return;
		}

		m_pAnimations[ i ].m_iNumBones = numBoneRefs;
		m_pAnimations[ i ].m_pBoneAnims = new Anim2DBone[ numBoneRefs ];

		for ( int j = 0; j < numBoneRefs; j++ )
		{
			pElement = pBoneRefArray->GetElement( j );
			JSONNumber *pTimelineNum = (JSONNumber*) ((JSONObject*)pElement)->GetElement( "timeline" );
			if ( !pTimelineNum )
			{
				agk::Error( "Failed to load Spriter skeleton, bone_ref does not have a timeline ID" );
				delete pRoot;
				for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
				delete [] pFolderFiles;
				return;
			}
			JSONNumber* pTimelineObj = (JSONNumber*) ((JSONObject*)(pTimeline->GetElement( pTimelineNum->m_iValue )))->GetElement( "obj" );
			if ( !pTimelineObj || pTimelineObj->GetType() != 4 )
			{
				agk::Error( "Failed to load Spriter skeleton, bone_ref timeline does not have a bone object" );
				delete pRoot;
				for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
				delete [] pFolderFiles;
				return;
			}
			int boneID = pTimelineObj->m_iValue;
			m_pAnimations[ i ].m_pBoneAnims[ j ].m_iTimelineID = pTimelineNum->m_iValue;
			m_pAnimations[ i ].m_pBoneAnims[ j ].m_sBoneName.SetStr( m_pBones[ boneID ].m_sName );
			m_pAnimations[ i ].m_pBoneAnims[ j ].m_iBoneIndex = boneID;
			
		}

		// setup slot variables
		pElement = pMainlineKeys->GetElement( 0 );
		JSONArray *pObjRefArray = (JSONArray*) ((JSONObject*)pElement)->GetElement( "object_ref" );
		int numObjRefs = pObjRefArray->m_iNumElements;
		if ( numObjRefs > (int)m_iNumSlots )
		{
			agk::Error( "Failed to load Spriter skeleton, animation mainline contains more object references than there are objects. Make sure your heirarchy contains the same structure across all animations, use the alpha setting to hide items rather than removing them from the heirarchy" );
			delete pRoot;
			for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
			delete [] pFolderFiles;
			return;
		}
		
		m_pAnimations[ i ].m_iNumSlots = numObjRefs;
		m_pAnimations[ i ].m_pSlotAnims = new Anim2DSlot[ numObjRefs ];

		for ( int j = 0; j < numObjRefs; j++ ) 
		{
			pElement = pObjRefArray->GetElement( j );
			JSONString *pTimelineString = (JSONString*) ((JSONObject*)pElement)->GetElement( "timeline" );
			if ( !pTimelineString )
			{
				agk::Error( "Failed to load Spriter skeleton, object_ref does not have a timeline ID" );
				delete pRoot;
				for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
				delete [] pFolderFiles;
				return;
			}
			int timelineID = pTimelineString->m_sValue.ToInt();
			m_pAnimations[ i ].m_pSlotAnims[ j ].m_iTimelineID = timelineID;
			m_pAnimations[ i ].m_pSlotAnims[ j ].m_iSlotIndex = j;
			pElement = pTimeline->GetElement( timelineID );
			JSONString *pTimelineName = (JSONString*) ((JSONObject*)pElement)->GetElement( "name" );
			if ( pTimelineName ) m_pAnimations[ i ].m_pSlotAnims[ j ].m_sSlotName.SetStr( pTimelineName->m_sValue );
		}

		// cycle through bone timelines
		for ( int b = 0; b < numBoneRefs; b++ )
		{
			int timelineID = m_pAnimations[ i ].m_pBoneAnims[ b ].m_iTimelineID;
			pElement = pTimeline->GetElement( timelineID );
			JSONArray *pTimelineKeys = (JSONArray*) ((JSONObject*)pElement)->GetElement( "key" );
			
			// if the last frame is short of the total animation time add the first frame again at the end of 
			// the animation to provide seamless looping in AGK, default check is 25ms
			int extraFrame = 0;
			JSONObject *pFinalKey = (JSONObject*) pTimelineKeys->GetElement( pTimelineKeys->m_iNumElements - 1 );
			JSONNumber* pFinalTime = (JSONNumber*) pFinalKey->GetElement( "time" );
			float finalTime = 0;
			if ( pFinalTime ) finalTime = pFinalTime->m_fValue;
			if ( finalTime + 25 < m_pAnimations[ i ].m_fTime*1000 ) extraFrame = 1;

			int numKeys = pTimelineKeys->m_iNumElements + extraFrame;

			m_pAnimations[ i ].m_pBoneAnims[ b ].m_iNumPositions = numKeys;
			m_pAnimations[ i ].m_pBoneAnims[ b ].m_iNumRotations = numKeys;
			m_pAnimations[ i ].m_pBoneAnims[ b ].m_iNumScales = numKeys;

			m_pAnimations[ i ].m_pBoneAnims[ b ].m_pPositions = new Anim2DKeyFramePosition*[ numKeys ];
			m_pAnimations[ i ].m_pBoneAnims[ b ].m_pRotations = new Anim2DKeyFrameRotation*[ numKeys ];
			m_pAnimations[ i ].m_pBoneAnims[ b ].m_pScales = new Anim2DKeyFrameScale*[ numKeys ];

			for ( int k = 0; k < (int)pTimelineKeys->m_iNumElements; k++ )
			{
				m_pAnimations[ i ].m_pBoneAnims[ b ].m_pPositions[ k ] = 0;
				m_pAnimations[ i ].m_pBoneAnims[ b ].m_pRotations[ k ] = 0;
				m_pAnimations[ i ].m_pBoneAnims[ b ].m_pScales[ k ] = 0;

				JSONObject *pTimeKey = (JSONObject*) pTimelineKeys->GetElement( k );

				float bonetime = 0;
				JSONNumber *pBoneTime = (JSONNumber*) pTimeKey->GetElement( "time" );
				if ( pBoneTime ) bonetime = pBoneTime->m_fValue / 1000.0f;
				
				JSONObject *pBoneKey = (JSONObject*) pTimeKey->GetElement( "bone" );
				if ( !pBoneKey )
				{
					agk::Error( "Failed to load Spriter skeleton, bone timeline does not have a bone key" );
					delete pRoot;
					for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
					delete [] pFolderFiles;
					return;
				}

				JSONString *pCurve = (JSONString*) pTimeKey->GetElement( "curve_type" );
				int curveType = 1; // assume linear
				if ( pCurve && pCurve->GetType() == 3 )
				{
					if ( pCurve->m_sValue.CompareCaseTo( "instant" ) == 0 ) curveType = 0;
					else if ( pCurve->m_sValue.CompareCaseTo( "linear" ) == 0 ) curveType = 1;
					else if ( pCurve->m_sValue.CompareCaseTo( "bezier" ) == 0 ) curveType = 2;
					else
					{
						agk::Error( "Failed to load Spriter skeleton, unsupported curve type, only instant, linear, and bezier curves are supported" );
						delete pRoot;
						for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
						delete [] pFolderFiles;
						return;
					}
				}

				switch( curveType )
				{
					case 0: // instant
					{
						m_pAnimations[ i ].m_pBoneAnims[ b ].m_pPositions[ k ] = new Anim2DKeyFramePositionStepped();
						m_pAnimations[ i ].m_pBoneAnims[ b ].m_pRotations[ k ] = new Anim2DKeyFrameRotationStepped();
						m_pAnimations[ i ].m_pBoneAnims[ b ].m_pScales[ k ] = new Anim2DKeyFrameScaleStepped();

						if ( k == 0 && extraFrame > 0 )
						{
							// duplicate the first key at the end for looping
							m_pAnimations[ i ].m_pBoneAnims[ b ].m_pPositions[ numKeys-1 ] = new Anim2DKeyFramePositionStepped();
							m_pAnimations[ i ].m_pBoneAnims[ b ].m_pRotations[ numKeys-1 ] = new Anim2DKeyFrameRotationStepped();
							m_pAnimations[ i ].m_pBoneAnims[ b ].m_pScales[ numKeys-1 ] = new Anim2DKeyFrameScaleStepped();
						}
						break;
					}
					case 1: // linear
					{
						m_pAnimations[ i ].m_pBoneAnims[ b ].m_pPositions[ k ] = new Anim2DKeyFramePosition();
						m_pAnimations[ i ].m_pBoneAnims[ b ].m_pRotations[ k ] = new Anim2DKeyFrameRotation();
						m_pAnimations[ i ].m_pBoneAnims[ b ].m_pScales[ k ] = new Anim2DKeyFrameScale();

						if ( k == 0 && extraFrame > 0 )
						{
							// duplicate the first key at the end for looping
							m_pAnimations[ i ].m_pBoneAnims[ b ].m_pPositions[ numKeys-1 ] = new Anim2DKeyFramePosition();
							m_pAnimations[ i ].m_pBoneAnims[ b ].m_pRotations[ numKeys-1 ] = new Anim2DKeyFrameRotation();
							m_pAnimations[ i ].m_pBoneAnims[ b ].m_pScales[ numKeys-1 ] = new Anim2DKeyFrameScale();
						}
						break;
					}
					case 2: // bezier
					{
						if ( !pTimeKey->GetElement( "c1" ) 
						  || !pTimeKey->GetElement( "c2" )
						  || !pTimeKey->GetElement( "c3" )
						  || !pTimeKey->GetElement( "c4" ) )
						{
							agk::Error( "Failed to load Spriter skeleton, bezier curve is missing some values" );
							delete pRoot;
							for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
							delete [] pFolderFiles;
							return;
						}

						Anim2DKeyFramePositionCurved *pPosCurve = new Anim2DKeyFramePositionCurved();
						pPosCurve->c1 = ((JSONNumber*) pTimeKey->GetElement( "c1" ))->m_fValue;
						pPosCurve->c2 = ((JSONNumber*) pTimeKey->GetElement( "c2" ))->m_fValue;
						pPosCurve->c3 = ((JSONNumber*) pTimeKey->GetElement( "c3" ))->m_fValue;
						pPosCurve->c4 = ((JSONNumber*) pTimeKey->GetElement( "c4" ))->m_fValue;
						m_pAnimations[ i ].m_pBoneAnims[ b ].m_pPositions[ k ] = pPosCurve;

						Anim2DKeyFrameRotationCurved *pRotCurve = new Anim2DKeyFrameRotationCurved();
						pRotCurve->c1 = ((JSONNumber*) pTimeKey->GetElement( "c1" ))->m_fValue;
						pRotCurve->c2 = ((JSONNumber*) pTimeKey->GetElement( "c2" ))->m_fValue;
						pRotCurve->c3 = ((JSONNumber*) pTimeKey->GetElement( "c3" ))->m_fValue;
						pRotCurve->c4 = ((JSONNumber*) pTimeKey->GetElement( "c4" ))->m_fValue;
						m_pAnimations[ i ].m_pBoneAnims[ b ].m_pRotations[ k ] = pRotCurve;

						Anim2DKeyFrameScaleCurved *pScaleCurve = new Anim2DKeyFrameScaleCurved();
						pScaleCurve->c1 = ((JSONNumber*) pTimeKey->GetElement( "c1" ))->m_fValue;
						pScaleCurve->c2 = ((JSONNumber*) pTimeKey->GetElement( "c2" ))->m_fValue;
						pScaleCurve->c3 = ((JSONNumber*) pTimeKey->GetElement( "c3" ))->m_fValue;
						pScaleCurve->c4 = ((JSONNumber*) pTimeKey->GetElement( "c4" ))->m_fValue;
						m_pAnimations[ i ].m_pBoneAnims[ b ].m_pScales[ k ] = pScaleCurve;

						if ( k == 0 && extraFrame > 0 )
						{
							pPosCurve = new Anim2DKeyFramePositionCurved();
							pPosCurve->c1 = ((JSONNumber*) pTimeKey->GetElement( "c1" ))->m_fValue;
							pPosCurve->c2 = ((JSONNumber*) pTimeKey->GetElement( "c2" ))->m_fValue;
							pPosCurve->c3 = ((JSONNumber*) pTimeKey->GetElement( "c3" ))->m_fValue;
							pPosCurve->c4 = ((JSONNumber*) pTimeKey->GetElement( "c4" ))->m_fValue;
							m_pAnimations[ i ].m_pBoneAnims[ b ].m_pPositions[ numKeys-1 ] = pPosCurve;

							pRotCurve = new Anim2DKeyFrameRotationCurved();
							pRotCurve->c1 = ((JSONNumber*) pTimeKey->GetElement( "c1" ))->m_fValue;
							pRotCurve->c2 = ((JSONNumber*) pTimeKey->GetElement( "c2" ))->m_fValue;
							pRotCurve->c3 = ((JSONNumber*) pTimeKey->GetElement( "c3" ))->m_fValue;
							pRotCurve->c4 = ((JSONNumber*) pTimeKey->GetElement( "c4" ))->m_fValue;
							m_pAnimations[ i ].m_pBoneAnims[ b ].m_pRotations[ numKeys-1 ] = pRotCurve;

							pScaleCurve = new Anim2DKeyFrameScaleCurved();
							pScaleCurve->c1 = ((JSONNumber*) pTimeKey->GetElement( "c1" ))->m_fValue;
							pScaleCurve->c2 = ((JSONNumber*) pTimeKey->GetElement( "c2" ))->m_fValue;
							pScaleCurve->c3 = ((JSONNumber*) pTimeKey->GetElement( "c3" ))->m_fValue;
							pScaleCurve->c4 = ((JSONNumber*) pTimeKey->GetElement( "c4" ))->m_fValue;
							m_pAnimations[ i ].m_pBoneAnims[ b ].m_pScales[ numKeys-1 ] = pScaleCurve;
						}
						break;
					}
				}

				m_pAnimations[ i ].m_pBoneAnims[ b ].m_pPositions[ k ]->m_fTime = bonetime;
				m_pAnimations[ i ].m_pBoneAnims[ b ].m_pRotations[ k ]->m_fTime = bonetime;
				m_pAnimations[ i ].m_pBoneAnims[ b ].m_pScales[ k ]->m_fTime = bonetime;

				if ( k == 0 && extraFrame > 0 )
				{
					m_pAnimations[ i ].m_pBoneAnims[ b ].m_pPositions[ numKeys-1 ]->m_fTime = m_pAnimations[ i ].m_fTime;
					m_pAnimations[ i ].m_pBoneAnims[ b ].m_pRotations[ numKeys-1 ]->m_fTime = m_pAnimations[ i ].m_fTime;
					m_pAnimations[ i ].m_pBoneAnims[ b ].m_pScales[ numKeys-1 ]->m_fTime = m_pAnimations[ i ].m_fTime;
				}

				float posX = 0;
				float posY = 0;
				float ang = 0;
				float scaleX = 1;
				float scaleY = 1;
				
				JSONNumber *pNum = (JSONNumber*) pBoneKey->GetElement( "x" );
				if ( pNum ) posX = pNum->m_fValue*scale;

				pNum = (JSONNumber*) pBoneKey->GetElement( "y" );
				if ( pNum ) posY = -pNum->m_fValue*scale;

				pNum = (JSONNumber*) pBoneKey->GetElement( "angle" );
				if ( pNum ) ang = pNum->m_fValue;

				pNum = (JSONNumber*) pBoneKey->GetElement( "scale_x" );
				if ( pNum ) scaleX = pNum->m_fValue;
				
				pNum = (JSONNumber*) pBoneKey->GetElement( "scale_y" );
				if ( pNum ) scaleY = pNum->m_fValue;

				m_pAnimations[ i ].m_pBoneAnims[ b ].m_pPositions[ k ]->m_fX = posX;
				m_pAnimations[ i ].m_pBoneAnims[ b ].m_pPositions[ k ]->m_fY = posY;
				m_pAnimations[ i ].m_pBoneAnims[ b ].m_pRotations[ k ]->m_fRotation = -ang;
				m_pAnimations[ i ].m_pBoneAnims[ b ].m_pScales[ k ]->m_fScaleX = scaleX;
				m_pAnimations[ i ].m_pBoneAnims[ b ].m_pScales[ k ]->m_fScaleY = scaleY;

				if ( k == 0 && extraFrame > 0 )
				{
					m_pAnimations[ i ].m_pBoneAnims[ b ].m_pPositions[ numKeys-1 ]->m_fX = posX;
					m_pAnimations[ i ].m_pBoneAnims[ b ].m_pPositions[ numKeys-1 ]->m_fY = posY;
					m_pAnimations[ i ].m_pBoneAnims[ b ].m_pRotations[ numKeys-1 ]->m_fRotation = -ang;
					m_pAnimations[ i ].m_pBoneAnims[ b ].m_pScales[ numKeys-1 ]->m_fScaleX = scaleX;
					m_pAnimations[ i ].m_pBoneAnims[ b ].m_pScales[ numKeys-1 ]->m_fScaleY = scaleY;
				}
			}
		}

		// cycle through slot timelines
		for ( int s = 0; s < numObjRefs; s++ )
		{
			int timelineID = m_pAnimations[ i ].m_pSlotAnims[ s ].m_iTimelineID;
			pElement = pTimeline->GetElement( timelineID );
			JSONArray *pTimelineKeys = (JSONArray*) ((JSONObject*)pElement)->GetElement( "key" );

			// if the last frame is short of the total animation time add the first frame again at the end of 
			// the animation to provide seamless looping in AGK, default check is 25ms
			int extraFrame = 0;
			JSONObject *pFinalKey = (JSONObject*) pTimelineKeys->GetElement( pTimelineKeys->m_iNumElements - 1 );
			JSONNumber* pFinalTime = (JSONNumber*) pFinalKey->GetElement( "time" );
			float finalTime = 0;
			if ( pFinalTime ) finalTime = pFinalTime->m_fValue;
			if ( finalTime + 25 < m_pAnimations[ i ].m_fTime*1000 ) extraFrame = 1;

			int numKeys = pTimelineKeys->m_iNumElements + extraFrame;

			m_pAnimations[ i ].m_pSlotAnims[ s ].m_iNumAttachments = numKeys;
			m_pAnimations[ i ].m_pSlotAnims[ s ].m_iNumColors = numKeys;
			m_pAnimations[ i ].m_pSlotAnims[ s ].m_iNumPositions = numKeys;
			m_pAnimations[ i ].m_pSlotAnims[ s ].m_iNumRotations = numKeys;
			m_pAnimations[ i ].m_pSlotAnims[ s ].m_iNumScales = numKeys;

			m_pAnimations[ i ].m_pSlotAnims[ s ].m_pAttachments = new Anim2DKeyFrameAttachment*[ numKeys ];
			m_pAnimations[ i ].m_pSlotAnims[ s ].m_pColors = new Anim2DKeyFrameColor*[ numKeys ];
			m_pAnimations[ i ].m_pSlotAnims[ s ].m_pPositions = new Anim2DKeyFramePosition*[ numKeys ];
			m_pAnimations[ i ].m_pSlotAnims[ s ].m_pRotations = new Anim2DKeyFrameRotation*[ numKeys ];
			m_pAnimations[ i ].m_pSlotAnims[ s ].m_pScales = new Anim2DKeyFrameScale*[ numKeys ];

			for ( int k = 0; k < (int)pTimelineKeys->m_iNumElements; k++ )
			{
				m_pAnimations[ i ].m_pSlotAnims[ s ].m_pAttachments[ k ] = 0;
				m_pAnimations[ i ].m_pSlotAnims[ s ].m_pColors[ k ] = 0;
				m_pAnimations[ i ].m_pSlotAnims[ s ].m_pPositions[ k ] = 0;
				m_pAnimations[ i ].m_pSlotAnims[ s ].m_pRotations[ k ] = 0;
				m_pAnimations[ i ].m_pSlotAnims[ s ].m_pScales[ k ] = 0;

				JSONObject *pTimeKey = (JSONObject*) pTimelineKeys->GetElement( k );

				float objtime = 0;
				JSONNumber *pObjTime = (JSONNumber*) pTimeKey->GetElement( "time" );
				if ( pObjTime ) objtime = pObjTime->m_fValue / 1000.0f;
				
				JSONObject *pObjKey = (JSONObject*) pTimeKey->GetElement( "object" );
				if ( !pObjKey )
				{
					agk::Error( "Failed to load Spriter skeleton, slot timeline does not have an object key" );
					delete pRoot;
					for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
					delete [] pFolderFiles;
					return;
				}

				// attachments can only be stepped
				m_pAnimations[ i ].m_pSlotAnims[ s ].m_pAttachments[ k ] = new Anim2DKeyFrameAttachment();
				if ( k == 0 && extraFrame > 0 )
				{
					m_pAnimations[ i ].m_pSlotAnims[ s ].m_pAttachments[ m_pAnimations[ i ].m_pSlotAnims[ s ].m_iNumAttachments-1 ] = new Anim2DKeyFrameAttachment();
				}

				JSONString *pCurve = (JSONString*) pTimeKey->GetElement( "curve_type" );
				int curveType = 1; // assume linear
				if ( pCurve && pCurve->GetType() == 3 )
				{
					if ( pCurve->m_sValue.CompareCaseTo( "instant" ) == 0 ) curveType = 0;
					else if ( pCurve->m_sValue.CompareCaseTo( "linear" ) == 0 ) curveType = 1;
					else if ( pCurve->m_sValue.CompareCaseTo( "bezier" ) == 0 ) curveType = 2;
					else
					{
						agk::Error( "Failed to load Spriter skeleton, unsupported curve type, only instant, linear, and bezier curves are supported" );
						delete pRoot;
						for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
						delete [] pFolderFiles;
						return;
					}
				}

				switch( curveType )
				{
					case 0: // instant
					{
						m_pAnimations[ i ].m_pSlotAnims[ s ].m_pColors[ k ] = new Anim2DKeyFrameColorStepped();
						m_pAnimations[ i ].m_pSlotAnims[ s ].m_pPositions[ k ] = new Anim2DKeyFramePositionStepped();
						m_pAnimations[ i ].m_pSlotAnims[ s ].m_pRotations[ k ] = new Anim2DKeyFrameRotationStepped();
						m_pAnimations[ i ].m_pSlotAnims[ s ].m_pScales[ k ] = new Anim2DKeyFrameScaleStepped();

						if ( k == 0 && extraFrame > 0 )
						{
							// duplicate the first key at the end for looping
							m_pAnimations[ i ].m_pSlotAnims[ s ].m_pColors[ numKeys-1 ] = new Anim2DKeyFrameColorStepped();
							m_pAnimations[ i ].m_pSlotAnims[ s ].m_pPositions[ numKeys-1 ] = new Anim2DKeyFramePositionStepped();
							m_pAnimations[ i ].m_pSlotAnims[ s ].m_pRotations[ numKeys-1 ] = new Anim2DKeyFrameRotationStepped();
							m_pAnimations[ i ].m_pSlotAnims[ s ].m_pScales[ numKeys-1 ] = new Anim2DKeyFrameScaleStepped();
						}
						break;
					}
					case 1: // linear
					{
						m_pAnimations[ i ].m_pSlotAnims[ s ].m_pColors[ k ] = new Anim2DKeyFrameColor();
						m_pAnimations[ i ].m_pSlotAnims[ s ].m_pPositions[ k ] = new Anim2DKeyFramePosition();
						m_pAnimations[ i ].m_pSlotAnims[ s ].m_pRotations[ k ] = new Anim2DKeyFrameRotation();
						m_pAnimations[ i ].m_pSlotAnims[ s ].m_pScales[ k ] = new Anim2DKeyFrameScale();
						
						if ( k == 0 && extraFrame > 0 )
						{
							// duplicate the first key at the end for looping
							m_pAnimations[ i ].m_pSlotAnims[ s ].m_pColors[ numKeys-1 ] = new Anim2DKeyFrameColor();
							m_pAnimations[ i ].m_pSlotAnims[ s ].m_pPositions[ numKeys-1 ] = new Anim2DKeyFramePosition();
							m_pAnimations[ i ].m_pSlotAnims[ s ].m_pRotations[ numKeys-1 ] = new Anim2DKeyFrameRotation();
							m_pAnimations[ i ].m_pSlotAnims[ s ].m_pScales[ numKeys-1 ] = new Anim2DKeyFrameScale();
						}
						break;
					}
					case 2: // bezier
					{
						if ( !pTimeKey->GetElement( "c1" ) 
						  || !pTimeKey->GetElement( "c2" )
						  || !pTimeKey->GetElement( "c3" )
						  || !pTimeKey->GetElement( "c4" ) )
						{
							agk::Error( "Failed to load Spriter skeleton, bezier curve is missing some values" );
							delete pRoot;
							for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
							delete [] pFolderFiles;
							return;
						}

						Anim2DKeyFrameColorCurved *pColorCurve = new Anim2DKeyFrameColorCurved();
						pColorCurve->c1 = ((JSONNumber*) pTimeKey->GetElement( "c1" ))->m_fValue;
						pColorCurve->c2 = ((JSONNumber*) pTimeKey->GetElement( "c2" ))->m_fValue;
						pColorCurve->c3 = ((JSONNumber*) pTimeKey->GetElement( "c3" ))->m_fValue;
						pColorCurve->c4 = ((JSONNumber*) pTimeKey->GetElement( "c4" ))->m_fValue;
						m_pAnimations[ i ].m_pSlotAnims[ s ].m_pColors[ k ] = pColorCurve;

						Anim2DKeyFramePositionCurved *pPosCurve = new Anim2DKeyFramePositionCurved();
						pPosCurve->c1 = ((JSONNumber*) pTimeKey->GetElement( "c1" ))->m_fValue;
						pPosCurve->c2 = ((JSONNumber*) pTimeKey->GetElement( "c2" ))->m_fValue;
						pPosCurve->c3 = ((JSONNumber*) pTimeKey->GetElement( "c3" ))->m_fValue;
						pPosCurve->c4 = ((JSONNumber*) pTimeKey->GetElement( "c4" ))->m_fValue;
						m_pAnimations[ i ].m_pSlotAnims[ s ].m_pPositions[ k ] = pPosCurve;

						Anim2DKeyFrameRotationCurved *pRotCurve = new Anim2DKeyFrameRotationCurved();
						pRotCurve->c1 = ((JSONNumber*) pTimeKey->GetElement( "c1" ))->m_fValue;
						pRotCurve->c2 = ((JSONNumber*) pTimeKey->GetElement( "c2" ))->m_fValue;
						pRotCurve->c3 = ((JSONNumber*) pTimeKey->GetElement( "c3" ))->m_fValue;
						pRotCurve->c4 = ((JSONNumber*) pTimeKey->GetElement( "c4" ))->m_fValue;
						m_pAnimations[ i ].m_pSlotAnims[ s ].m_pRotations[ k ] = pRotCurve;

						Anim2DKeyFrameScaleCurved *pScaleCurve = new Anim2DKeyFrameScaleCurved();
						pScaleCurve->c1 = ((JSONNumber*) pTimeKey->GetElement( "c1" ))->m_fValue;
						pScaleCurve->c2 = ((JSONNumber*) pTimeKey->GetElement( "c2" ))->m_fValue;
						pScaleCurve->c3 = ((JSONNumber*) pTimeKey->GetElement( "c3" ))->m_fValue;
						pScaleCurve->c4 = ((JSONNumber*) pTimeKey->GetElement( "c4" ))->m_fValue;
						m_pAnimations[ i ].m_pSlotAnims[ s ].m_pScales[ k ] = pScaleCurve;
						
						if ( k == 0 && extraFrame > 0 )
						{
							pColorCurve = new Anim2DKeyFrameColorCurved();
							pColorCurve->c1 = ((JSONNumber*) pTimeKey->GetElement( "c1" ))->m_fValue;
							pColorCurve->c2 = ((JSONNumber*) pTimeKey->GetElement( "c2" ))->m_fValue;
							pColorCurve->c3 = ((JSONNumber*) pTimeKey->GetElement( "c3" ))->m_fValue;
							pColorCurve->c4 = ((JSONNumber*) pTimeKey->GetElement( "c4" ))->m_fValue;
							m_pAnimations[ i ].m_pSlotAnims[ s ].m_pColors[ m_pAnimations[ i ].m_pSlotAnims[ s ].m_iNumColors-1 ] = pColorCurve;

							pPosCurve = new Anim2DKeyFramePositionCurved();
							pPosCurve->c1 = ((JSONNumber*) pTimeKey->GetElement( "c1" ))->m_fValue;
							pPosCurve->c2 = ((JSONNumber*) pTimeKey->GetElement( "c2" ))->m_fValue;
							pPosCurve->c3 = ((JSONNumber*) pTimeKey->GetElement( "c3" ))->m_fValue;
							pPosCurve->c4 = ((JSONNumber*) pTimeKey->GetElement( "c4" ))->m_fValue;
							m_pAnimations[ i ].m_pSlotAnims[ s ].m_pPositions[ numKeys-1 ] = pPosCurve;

							pRotCurve = new Anim2DKeyFrameRotationCurved();
							pRotCurve->c1 = ((JSONNumber*) pTimeKey->GetElement( "c1" ))->m_fValue;
							pRotCurve->c2 = ((JSONNumber*) pTimeKey->GetElement( "c2" ))->m_fValue;
							pRotCurve->c3 = ((JSONNumber*) pTimeKey->GetElement( "c3" ))->m_fValue;
							pRotCurve->c4 = ((JSONNumber*) pTimeKey->GetElement( "c4" ))->m_fValue;
							m_pAnimations[ i ].m_pSlotAnims[ s ].m_pRotations[ numKeys-1 ] = pRotCurve;

							pScaleCurve = new Anim2DKeyFrameScaleCurved();
							pScaleCurve->c1 = ((JSONNumber*) pTimeKey->GetElement( "c1" ))->m_fValue;
							pScaleCurve->c2 = ((JSONNumber*) pTimeKey->GetElement( "c2" ))->m_fValue;
							pScaleCurve->c3 = ((JSONNumber*) pTimeKey->GetElement( "c3" ))->m_fValue;
							pScaleCurve->c4 = ((JSONNumber*) pTimeKey->GetElement( "c4" ))->m_fValue;
							m_pAnimations[ i ].m_pSlotAnims[ s ].m_pScales[ numKeys-1 ] = pScaleCurve;
						}
						break;
					}
				}

				m_pAnimations[ i ].m_pSlotAnims[ s ].m_pAttachments[ k ]->m_fTime = objtime;
				m_pAnimations[ i ].m_pSlotAnims[ s ].m_pColors[ k ]->m_fTime = objtime;
				m_pAnimations[ i ].m_pSlotAnims[ s ].m_pPositions[ k ]->m_fTime = objtime;
				m_pAnimations[ i ].m_pSlotAnims[ s ].m_pRotations[ k ]->m_fTime = objtime;
				m_pAnimations[ i ].m_pSlotAnims[ s ].m_pScales[ k ]->m_fTime = objtime;

				if ( k == 0 && extraFrame > 0 )
				{
					m_pAnimations[ i ].m_pSlotAnims[ s ].m_pAttachments[ numKeys-1 ]->m_fTime = m_pAnimations[ i ].m_fTime;
					m_pAnimations[ i ].m_pSlotAnims[ s ].m_pColors[ numKeys-1 ]->m_fTime = m_pAnimations[ i ].m_fTime;
					m_pAnimations[ i ].m_pSlotAnims[ s ].m_pPositions[ numKeys-1 ]->m_fTime = m_pAnimations[ i ].m_fTime;
					m_pAnimations[ i ].m_pSlotAnims[ s ].m_pRotations[ numKeys-1 ]->m_fTime = m_pAnimations[ i ].m_fTime;
					m_pAnimations[ i ].m_pSlotAnims[ s ].m_pScales[ numKeys-1 ]->m_fTime = m_pAnimations[ i ].m_fTime;
				}

				int alpha = 255;
				int file = -1;
				int folder = -1;
				float posX = 0;
				float posY = 0;
				float ang = 0;
				float scaleX = 1;
				float scaleY = 1;
				
				JSONNumber *pNum = (JSONNumber*) pObjKey->GetElement( "a" );
				if ( pNum ) alpha = agk::Round(pNum->m_fValue*255);
				if ( alpha > 255 ) alpha = 255;
				if ( alpha < 0 ) alpha = 0;

				pNum = (JSONNumber*) pObjKey->GetElement( "file" );
				if ( pNum ) file = agk::Round(pNum->m_fValue);

				pNum = (JSONNumber*) pObjKey->GetElement( "folder" );
				if ( pNum ) folder = agk::Round(pNum->m_fValue);

				pNum = (JSONNumber*) pObjKey->GetElement( "x" );
				if ( pNum ) posX = pNum->m_fValue*scale;

				pNum = (JSONNumber*) pObjKey->GetElement( "y" );
				if ( pNum ) posY = -pNum->m_fValue*scale;

				pNum = (JSONNumber*) pObjKey->GetElement( "angle" );
				if ( pNum ) ang = pNum->m_fValue;

				pNum = (JSONNumber*) pObjKey->GetElement( "scale_x" );
				if ( pNum ) scaleX = pNum->m_fValue;
				
				pNum = (JSONNumber*) pObjKey->GetElement( "scale_y" );
				if ( pNum ) scaleY = pNum->m_fValue;

				m_pAnimations[ i ].m_pSlotAnims[ s ].m_pAttachments[ k ]->m_pSprite = 0;
				if ( file >= 0 && folder >= 0 ) m_pAnimations[ i ].m_pSlotAnims[ s ].m_pAttachments[ k ]->m_pSprite = pFolderFiles[ folder ][ file ];
				m_pAnimations[ i ].m_pSlotAnims[ s ].m_pColors[ k ]->m_iAlpha = alpha;
				m_pAnimations[ i ].m_pSlotAnims[ s ].m_pColors[ k ]->m_iBlue = 255;
				m_pAnimations[ i ].m_pSlotAnims[ s ].m_pColors[ k ]->m_iGreen = 255;
				m_pAnimations[ i ].m_pSlotAnims[ s ].m_pColors[ k ]->m_iRed = 255;

				m_pAnimations[ i ].m_pSlotAnims[ s ].m_pPositions[ k ]->m_fX = posX;
				m_pAnimations[ i ].m_pSlotAnims[ s ].m_pPositions[ k ]->m_fY = posY;
				m_pAnimations[ i ].m_pSlotAnims[ s ].m_pRotations[ k ]->m_fRotation = -ang;
				m_pAnimations[ i ].m_pSlotAnims[ s ].m_pScales[ k ]->m_fScaleX = scaleX;
				m_pAnimations[ i ].m_pSlotAnims[ s ].m_pScales[ k ]->m_fScaleY = scaleY;
								
				if ( k == 0 && extraFrame > 0 )
				{
					m_pAnimations[ i ].m_pSlotAnims[ s ].m_pAttachments[ numKeys-1 ]->m_pSprite = 0;
					if ( file >= 0 && folder >= 0 ) m_pAnimations[ i ].m_pSlotAnims[ s ].m_pAttachments[ numKeys-1 ]->m_pSprite = pFolderFiles[ folder ][ file ];
					m_pAnimations[ i ].m_pSlotAnims[ s ].m_pColors[ numKeys-1 ]->m_iAlpha = alpha;
					m_pAnimations[ i ].m_pSlotAnims[ s ].m_pColors[ numKeys-1 ]->m_iBlue = 255;
					m_pAnimations[ i ].m_pSlotAnims[ s ].m_pColors[ numKeys-1 ]->m_iGreen = 255;
					m_pAnimations[ i ].m_pSlotAnims[ s ].m_pColors[ numKeys-1 ]->m_iRed = 255;

					m_pAnimations[ i ].m_pSlotAnims[ s ].m_pPositions[ numKeys-1 ]->m_fX = posX;
					m_pAnimations[ i ].m_pSlotAnims[ s ].m_pPositions[ numKeys-1 ]->m_fY = posY;
					m_pAnimations[ i ].m_pSlotAnims[ s ].m_pRotations[ numKeys-1 ]->m_fRotation = -ang;
					m_pAnimations[ i ].m_pSlotAnims[ s ].m_pScales[ numKeys-1 ]->m_fScaleX = scaleX;
					m_pAnimations[ i ].m_pSlotAnims[ s ].m_pScales[ numKeys-1 ]->m_fScaleY = scaleY;
				}
			}
		}

		// cycle through all mainline keys
		int numMainKeys = (int)pMainlineKeys->m_iNumElements;
		// if the last frame is short of the total animation time add the first frame again at the end of 
		// the animation to provide seamless looping in AGK, default check is 25ms
		int extraFrame = 0;
		JSONObject* pFinalKey = (JSONObject*) pMainlineKeys->GetElement( numMainKeys - 1 );
		JSONNumber* pFinalTime = (JSONNumber*) pFinalKey->GetElement( "time" );
		float finalTime = 0;
		if ( pFinalTime ) finalTime = pFinalTime->m_fValue;
		if ( finalTime + 25 < m_pAnimations[ i ].m_fTime*1000 ) extraFrame = 1;
		int numKeys = numMainKeys + extraFrame;
		for ( int k = 0; k < numMainKeys; k++ )
		{
			JSONObject *pMainKey = (JSONObject*) pMainlineKeys->GetElement( k );

			// object keys
			pObjRefs = (JSONArray*) pMainKey->GetElement( "object_ref" );
			if ( !pObjRefs ) continue;

			pBoneRefs = (JSONArray*) pMainKey->GetElement( "bone_ref" );
			if ( !pBoneRefs ) continue;

			for ( int s = 0; s < (int)pObjRefs->m_iNumElements; s++ )
			{
				JSONObject *pObjRef = (JSONObject*) pObjRefs->GetElement( s );
				JSONString *pTimelineID = (JSONString*) pObjRef->GetElement( "timeline" );
				int timelineID = pTimelineID->m_sValue.ToInt();
				JSONNumber *pParentID = (JSONNumber*) pObjRef->GetElement( "parent" );

				int parentID = -1;
				if ( pParentID )
				{
					JSONObject* pBoneRef = (JSONObject*) pBoneRefs->GetElement( pParentID->m_iValue );

					if ( pBoneRef )
					{
						int iBoneTimeline = ((JSONNumber*)pBoneRef->GetElement( "timeline" ))->m_iValue;
						JSONNumber *pBoneID = (JSONNumber*) ((JSONObject*)pTimeline->GetElement( iBoneTimeline ))->GetElement( "obj" );
						if ( !pBoneID )
						{
							/*
							agk::Error( "Failed to load Spriter skeleton, slot parent timeline does not have a bone object" );
							delete pRoot;
							for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
							delete [] pFolderFiles;
							return;
							*/
						}
						else
						{
							parentID = pBoneID->m_iValue;
						}
					}
				}
				
				Anim2DSlot *pAnimSlot = m_pAnimations[ i ].GetAnimForSlotTimeline( timelineID );
				if ( !pAnimSlot ) continue;

				if ( pAnimSlot->m_iNumBones == 0 )
				{
					pAnimSlot->m_iNumBones = numKeys;
					pAnimSlot->m_pBones = new Anim2DKeyFrameBone*[ numKeys ];
				}

				if ( pAnimSlot->m_iNumZOrder == 0 )
				{
					pAnimSlot->m_iNumZOrder = numKeys;
					pAnimSlot->m_pZOrder = new Anim2DKeyFrameZOrder*[ numKeys ];
				}

				float objtime = 0;
				JSONNumber *pObjTime = (JSONNumber*) pMainKey->GetElement( "time" );
				if ( pObjTime ) objtime = pObjTime->m_fValue / 1000.0f;
				
				// bones can only be stepped
				pAnimSlot->m_pBones[ k ] = new Anim2DKeyFrameBone();
				pAnimSlot->m_pBones[ k ]->m_fTime = objtime;
				if ( parentID >= 0 ) pAnimSlot->m_pBones[ k ]->m_pBoneParent = &(m_pBones[ parentID ]);
				else pAnimSlot->m_pBones[ k ]->m_pBoneParent = 0;

				// ZOrder can only be stepped
				pAnimSlot->m_pZOrder[ k ] = new Anim2DKeyFrameZOrder();
				pAnimSlot->m_pZOrder[ k ]->m_fTime = objtime;
				pAnimSlot->m_pZOrder[ k ]->m_iZOrder = s;

				if ( k == 0 && extraFrame > 0 )
				{
					pAnimSlot->m_pBones[ numKeys - 1 ] = new Anim2DKeyFrameBone();
					pAnimSlot->m_pBones[ numKeys - 1 ]->m_fTime = m_pAnimations[ i ].m_fTime;
					if ( parentID >= 0 ) pAnimSlot->m_pBones[ numKeys - 1 ]->m_pBoneParent = &(m_pBones[ parentID ]);
					else pAnimSlot->m_pBones[ numKeys - 1 ]->m_pBoneParent = 0;

					pAnimSlot->m_pZOrder[ numKeys - 1 ] = new Anim2DKeyFrameZOrder();
					pAnimSlot->m_pZOrder[ numKeys - 1 ]->m_fTime = m_pAnimations[ i ].m_fTime;
					pAnimSlot->m_pZOrder[ numKeys - 1 ]->m_iZOrder = s;
				}
			}

			for ( int b = 0; b < (int)pBoneRefs->m_iNumElements; b++ )
			{
				JSONObject *pBoneRef = (JSONObject*) pBoneRefs->GetElement( b );
				JSONNumber *pTimelineID = (JSONNumber*) pBoneRef->GetElement( "timeline" );
				int timelineID = pTimelineID->m_iValue;
				JSONNumber *pBoneID = (JSONNumber*) ((JSONObject*)pTimeline->GetElement( timelineID ))->GetElement( "obj" );
				int boneID = pBoneID->m_iValue;

				Bone2D *pBoneParent = 0;
				JSONNumber *pParent = (JSONNumber*) pBoneRef->GetElement( "parent" );
				if ( pParent )
				{
					JSONObject* pParentRef = (JSONObject*) pBoneRefs->GetElement( pParent->m_iValue );
					int iParentTimeline = ((JSONNumber*)pParentRef->GetElement( "timeline" ))->m_iValue;
					JSONNumber *pParentID = (JSONNumber*) ((JSONObject*)pTimeline->GetElement( iParentTimeline ))->GetElement( "obj" );
					pBoneParent = &(m_pBones[ pParentID->m_iValue ]);
				}

				Anim2DBone *pAnimBone = m_pAnimations[ i ].GetAnimForBoneTimeline( timelineID );
				if ( !pAnimBone ) continue;
								
				if ( pAnimBone->m_iNumParents == 0 )
				{
					pAnimBone->m_iNumParents = numKeys;
					pAnimBone->m_pParents = new Anim2DKeyFrameBone*[ numKeys ];
				}

				float objtime = 0;
				JSONNumber *pObjTime = (JSONNumber*) pMainKey->GetElement( "time" );
				if ( pObjTime ) objtime = pObjTime->m_fValue / 1000.0f;
				
				// bones can only be stepped
				pAnimBone->m_pParents[ k ] = new Anim2DKeyFrameBone();
				pAnimBone->m_pParents[ k ]->m_fTime = objtime;
				pAnimBone->m_pParents[ k ]->m_pBoneParent = pBoneParent;

				if ( k == 0 && extraFrame > 0 )
				{
					pAnimBone->m_pParents[ numKeys - 1 ] = new Anim2DKeyFrameBone();
					pAnimBone->m_pParents[ numKeys - 1 ]->m_fTime = m_pAnimations[ i ].m_fTime;
					pAnimBone->m_pParents[ numKeys - 1 ]->m_pBoneParent = pBoneParent;
				}
			}
		}
	} // end animations loop

	delete pRoot;

	for ( int f = 0; f < numFolders; f++ ) delete [] pFolderFiles[ f ];
	delete [] pFolderFiles;
	
	// set the skeleton to the first frame of animation 0
	if ( sFirstAnimName.GetLength() > 0 )
	{
		SetAnimationFrame( sFirstAnimName.GetStr(), 0, 0 );
		//PlayAnimation( sFirstAnimName, 0, 0, 0 );
		/*
		SetAnimationFrame( sFirstAnimName.GetStr(), 0, 0 );
		for( UINT i = 0; i < m_iNumBones; i++ )
		{
			m_pBones[ i ].Interpolate( 0 );
		}

		for( UINT i = 0; i < m_iNumSlots; i++ )
		{
			m_pSlots[ i ].Interpolate( 0 );
		}
		*/
		//Update(0.1f);
		//Update(0.1f);
		//Update(0.1f);
		//StopAnimation();
	}
}

void Skeleton2D::SetPosition( float x, float y )
{
	m_fX = x;
	m_fY = y;
	Update(0);
}

void Skeleton2D::SetAngle( float angle )
{
	m_fAngle = angle;
	Update(0);
}

void Skeleton2D::SetDepth( int depth )
{
	m_iDepth = depth;
}

void Skeleton2D::FixToScreen( int mode )
{
	for ( UINT i = 0; i < m_iNumSprites; i++ )
	{
		m_pSprites[ i ].FixToScreen( mode );
	}
}

void Skeleton2D::SetVisible( int mode )
{
	if ( mode != 0 ) m_bFlags |= AGK_SKELETON_VISIBLE;
	else m_bFlags &= ~AGK_SKELETON_VISIBLE;
}

void Skeleton2D::AddExternalSprite( cSprite *sprite, int zorder )
{
	ExternalSprite *pNewExt = new ExternalSprite();
	pNewExt->m_pSprite = sprite;
	pNewExt->m_iZOrder = zorder;

	if ( !m_pFirstExtSprite )
	{
		m_pFirstExtSprite = pNewExt;
	}
	else
	{
		ExternalSprite *pExtSprite = m_pFirstExtSprite;
		while( pExtSprite )
		{
			if ( pExtSprite->m_iZOrder > pNewExt->m_iZOrder )
			{
				// insert new one here
				pNewExt->m_pPrev = pExtSprite->m_pPrev;
				pNewExt->m_pNext = pExtSprite;
				pExtSprite->m_pPrev = pNewExt;
				if ( pNewExt->m_pPrev ) pNewExt->m_pPrev->m_pNext = pNewExt;
				else m_pFirstExtSprite = pNewExt;
				break;
			}

			if ( !pExtSprite->m_pNext )
			{
				// insert at end
				pNewExt->m_pPrev = pExtSprite;
				pExtSprite->m_pNext = pNewExt;
				break;
			}

			pExtSprite = pExtSprite->m_pNext;
		}
	}
}

void Skeleton2D::RemoveExternalSprite( cSprite *sprite )
{
	ExternalSprite *pExtSprite = m_pFirstExtSprite;
	while( pExtSprite )
	{
		ExternalSprite *pNext = pExtSprite->m_pNext;
		if ( pExtSprite->m_pSprite == sprite )
		{
			// remove
			if ( pExtSprite->m_pPrev ) pExtSprite->m_pPrev->m_pNext = pExtSprite->m_pNext;
			else m_pFirstExtSprite = pExtSprite->m_pNext;
			if ( pExtSprite->m_pNext ) pExtSprite->m_pNext->m_pPrev = pExtSprite->m_pPrev;
			delete pExtSprite;
		}

		pExtSprite = pNext;
	}
}

Bone2D* Skeleton2D::GetBone( const char* name )
{
	for ( UINT i = 0; i < m_iNumBones; i++ )
	{
		if ( m_pBones[ i ].m_sName.CompareTo( name ) == 0 ) return m_pBones + i;
	}

	return 0;
}

Bone2D* Skeleton2D::GetBone( UINT index )
{
	if ( index > m_iNumBones ) return 0;
	return m_pBones + index;
}

int Skeleton2D::GetBoneIndex( const char* name )
{
	for ( UINT i = 0; i < m_iNumBones; i++ )
	{
		if ( m_pBones[ i ].m_sName.CompareTo( name ) == 0 ) return i;
	}

	return -1;
}

Slot2D* Skeleton2D::GetSlot( const char* name )
{
	for ( UINT i = 0; i < m_iNumSlots; i++ )
	{
		if ( m_pSlots[ i ].m_sName.CompareTo( name ) == 0 ) return m_pSlots + i;
	}

	return 0;
}

int Skeleton2D::GetSlotIndex( const char* name )
{
	for ( UINT i = 0; i < m_iNumSlots; i++ )
	{
		if ( m_pSlots[ i ].m_sName.CompareTo( name ) == 0 ) return i;
	}

	return -1;
}

cSprite* Skeleton2D::GetSprite( const char* name )
{
	for ( UINT i = 0; i < m_iNumSprites; i++ )
	{
		if ( strcmp( m_pSprites[ i ].GetName(), name ) == 0 ) return m_pSprites + i;
	}

	return 0;
}

int Skeleton2D::GetAnimation( const char* name )
{
	for ( UINT i = 0; i < m_iNumAnimations; i++ )
	{
		if ( strcmp( m_pAnimations[ i ].m_sName.GetStr(), name ) == 0 ) return i;
	}

	return -1;
}

void Skeleton2D::PlayAnimation( const char* anim, float starttime, int loop, float tweentime )
{
	if ( !anim ) return;
	if ( tweentime < 0 ) tweentime = 0;
	if ( loop < 0 ) loop = 0;
	if ( starttime < 0 ) starttime = 0;

	int iAnim = GetAnimation( anim );
	if ( iAnim < 0 )
	{
		uString errStr;
		errStr.Format( "Failed to play skeleton 2D animation, animation \"%s\" does not exist", anim );
		agk::Error( errStr );
		return;
	}

	if ( starttime > m_pAnimations[ iAnim ].m_fTime ) starttime = m_pAnimations[ iAnim ].m_fTime;

	for ( UINT i = 0; i < m_iNumBones; i++ )
	{
		m_pBones[ i ].m_pPrevAnim = m_pBones[ i ].m_pAnim;
		m_pBones[ i ].m_pAnim = 0;
		m_pBones[ i ].m_iPrevFramePosition = m_pBones[ i ].m_iCurrFramePosition;
		m_pBones[ i ].m_iPrevFrameRotation = m_pBones[ i ].m_iCurrFrameRotation;
		m_pBones[ i ].m_iPrevFrameScale = m_pBones[ i ].m_iCurrFrameScale;
		m_pBones[ i ].m_iCurrFramePosition = 0;
		m_pBones[ i ].m_iCurrFrameRotation = 0;
		m_pBones[ i ].m_iCurrFrameScale = 0;
	}

	for ( UINT i = 0; i < m_pAnimations[ iAnim ].m_iNumBones; i++ )
	{
		Anim2DBone *pBoneAnim = m_pAnimations[ iAnim ].m_pBoneAnims + i;
		if ( pBoneAnim->m_iBoneIndex >= 0 )
		{
			m_pBones[ pBoneAnim->m_iBoneIndex ].m_pAnim = pBoneAnim;
		}
	}

	for ( UINT i = 0; i < m_iNumSlots; i++ )
	{
		m_pSlots[ i ].m_pPrevAnim = m_pSlots[ i ].m_pAnim;
		m_pSlots[ i ].m_pAnim = 0;
		m_pSlots[ i ].m_iPrevFrameColor = m_pSlots[ i ].m_iCurrFrameColor;
		m_pSlots[ i ].m_iPrevFrameAttachment = m_pSlots[ i ].m_iCurrFrameAttachment;
		m_pSlots[ i ].m_iPrevFrameBone = m_pSlots[ i ].m_iCurrFrameBone;
		m_pSlots[ i ].m_iPrevFrameZOrder = m_pSlots[ i ].m_iCurrFrameZOrder;
		m_pSlots[ i ].m_iPrevFramePosition = m_pSlots[ i ].m_iCurrFramePosition;
		m_pSlots[ i ].m_iPrevFrameRotation = m_pSlots[ i ].m_iCurrFrameRotation;
		m_pSlots[ i ].m_iPrevFrameScale = m_pSlots[ i ].m_iCurrFrameScale;
		m_pSlots[ i ].m_iCurrFrameColor = 0;
		m_pSlots[ i ].m_iCurrFrameAttachment = 0;
		m_pSlots[ i ].m_iCurrFrameBone = 0;
		m_pSlots[ i ].m_iCurrFrameZOrder = 0;
		m_pSlots[ i ].m_iCurrFramePosition = 0;
		m_pSlots[ i ].m_iCurrFrameRotation = 0;
		m_pSlots[ i ].m_iCurrFrameScale = 0;
	}

	for ( UINT i = 0; i < m_pAnimations[ iAnim ].m_iNumSlots; i++ )
	{
		Anim2DSlot *pSlotAnim = m_pAnimations[ iAnim ].m_pSlotAnims + i;
		if ( pSlotAnim->m_iSlotIndex >= 0 )
		{
			m_pSlots[ pSlotAnim->m_iSlotIndex ].m_pAnim = pSlotAnim;
		}
	}

	m_fTweenTime = tweentime;
	m_fTotalTweenTime = tweentime;
	m_fPrevTime = m_fCurrTime;
	m_fCurrTime = starttime;

	m_iCurrAnimation = iAnim;

	m_bFlags |= AGK_SKELETON_PLAYING;
	if ( loop > 0 ) 
	{
		if ( loop == 1 ) m_bFlags |= AGK_SKELETON_LOOPING;
		else
		{
			m_bFlags &= ~AGK_SKELETON_LOOPING;
			m_iLoopTotal = loop;
			m_iLoopCount = 0;
		}
	}
	else 
	{
		m_bFlags &= ~AGK_SKELETON_LOOPING;
		m_iLoopTotal = 0;
		m_iLoopCount = 0;
	}
}

void Skeleton2D::SetAnimationFrame( const char* anim, float time, float tweentime )
{
	if ( !anim ) return;
	if ( tweentime < 0 ) tweentime = 0;
	if ( time < 0 ) time = 0;

	int iAnim = GetAnimation( anim );
	if ( iAnim < 0 )
	{
		uString errStr;
		errStr.Format( "Failed to set skeleton animation frame, animation \"%s\" does not exist", anim );
		agk::Error( errStr );
		return;
	}

	for ( UINT i = 0; i < m_iNumBones; i++ )
	{
		m_pBones[ i ].m_pPrevAnim = m_pBones[ i ].m_pAnim;
		m_pBones[ i ].m_pAnim = 0;
		m_pBones[ i ].m_iPrevFramePosition = m_pBones[ i ].m_iCurrFramePosition;
		m_pBones[ i ].m_iPrevFrameRotation = m_pBones[ i ].m_iCurrFrameRotation;
		m_pBones[ i ].m_iPrevFrameScale = m_pBones[ i ].m_iCurrFrameScale;
		m_pBones[ i ].m_iCurrFramePosition = 0;
		m_pBones[ i ].m_iCurrFrameRotation = 0;
		m_pBones[ i ].m_iCurrFrameScale = 0;
	}

	for ( UINT i = 0; i < m_pAnimations[ iAnim ].m_iNumBones; i++ )
	{
		Anim2DBone *pBoneAnim = m_pAnimations[ iAnim ].m_pBoneAnims + i;
		if ( pBoneAnim->m_iBoneIndex >= 0 )
		{
			m_pBones[ pBoneAnim->m_iBoneIndex ].m_pAnim = pBoneAnim;
		}
	}

	for ( UINT i = 0; i < m_iNumSlots; i++ )
	{
		m_pSlots[ i ].m_pPrevAnim = m_pSlots[ i ].m_pAnim;
		m_pSlots[ i ].m_pAnim = 0;
		m_pSlots[ i ].m_iPrevFrameColor = m_pSlots[ i ].m_iCurrFrameColor;
		m_pSlots[ i ].m_iPrevFrameAttachment = m_pSlots[ i ].m_iCurrFrameAttachment;
		m_pSlots[ i ].m_iPrevFrameBone = m_pSlots[ i ].m_iCurrFrameBone;
		m_pSlots[ i ].m_iPrevFrameZOrder = m_pSlots[ i ].m_iCurrFrameZOrder;
		m_pSlots[ i ].m_iPrevFramePosition = m_pSlots[ i ].m_iCurrFramePosition;
		m_pSlots[ i ].m_iPrevFrameRotation = m_pSlots[ i ].m_iCurrFrameRotation;
		m_pSlots[ i ].m_iPrevFrameScale = m_pSlots[ i ].m_iCurrFrameScale;
		m_pSlots[ i ].m_iCurrFrameColor = 0;
		m_pSlots[ i ].m_iCurrFrameAttachment = 0;
		m_pSlots[ i ].m_iCurrFrameBone = 0;
		m_pSlots[ i ].m_iCurrFrameZOrder = 0;
		m_pSlots[ i ].m_iCurrFramePosition = 0;
		m_pSlots[ i ].m_iCurrFrameRotation = 0;
		m_pSlots[ i ].m_iCurrFrameScale = 0;
	}

	for ( UINT i = 0; i < m_pAnimations[ iAnim ].m_iNumSlots; i++ )
	{
		Anim2DSlot *pSlotAnim = m_pAnimations[ iAnim ].m_pSlotAnims + i;
		if ( pSlotAnim->m_iSlotIndex >= 0 )
		{
			m_pSlots[ pSlotAnim->m_iSlotIndex ].m_pAnim = pSlotAnim;
		}
	}

	m_fTweenTime = tweentime;
	m_fTotalTweenTime = tweentime;
	m_fPrevTime = m_fCurrTime;
	m_fCurrTime = time;

	m_iCurrAnimation = iAnim;

	if ( (m_bFlags & AGK_SKELETON_PLAYING) == 0 )
	{
		Update(0);
	}
}

void Skeleton2D::SetAnimationSpeed( float speed )
{
	m_fSpeed = speed;
}

void Skeleton2D::StopAnimation()
{
	m_bFlags &= ~AGK_SKELETON_PLAYING;
	m_bFlags &= ~AGK_SKELETON_LOOPING;
	m_iLoopTotal = 0;
	m_iLoopCount = 0;
}

int Skeleton2D::GetIsAnimating()
{
	return (m_bFlags & AGK_SKELETON_PLAYING) ? 1 : 0;
}

int Skeleton2D::GetIsTweening()
{
	return (m_fTweenTime > 0) ? 1 : 0;
}

float Skeleton2D::GetCurrentTime()
{
	return m_fCurrTime;
}

float Skeleton2D::GetAnimationTime( const char* anim )
{
	if ( !anim ) return 0;
	int iAnim = GetAnimation( anim );
	if ( iAnim < 0 )
	{
		uString errStr;
		errStr.Format( "Skeleton does not contain animation \"%s\"", anim );
		agk::Error( errStr );
		return 0;
	}

	return m_pAnimations[ iAnim ].m_fTime;
}

void Skeleton2D::SetFlipH( int flip )
{
	if ( flip )	m_bFlags |= AGK_SKELETON_FLIPH;
	else m_bFlags &= ~AGK_SKELETON_FLIPH;
}

void Skeleton2D::SetFlipV( int flip )
{
	if ( flip )	m_bFlags |= AGK_SKELETON_FLIPV;
	else m_bFlags &= ~AGK_SKELETON_FLIPV;
}

void Skeleton2D::Update( float time )
{
	for ( UINT i = 0; i < m_iNumBones; i++ )
	{
		if ( m_pBones[ i ].m_bFlags & AGK_BONE_ROOT ) 
		{
			m_pBones[ i ].origX = m_fX;
			m_pBones[ i ].origY = m_fY;
			m_pBones[ i ].origAngle = m_fAngle;

			if ( m_bFlags & AGK_SKELETON_PLAYING )
			{
				m_pBones[ i ].x = m_fX;
				m_pBones[ i ].y = m_fY;
				m_pBones[ i ].angle = m_fAngle;
			}
		}
	}

	for( UINT i = 0; i < m_iNumSprites; i++ )
	{
		m_pSprites[ i ].SetVisible( false );
	}

	if ( m_fTweenTime > 0 && m_fTotalTweenTime > 0 )
	{
		// blend to the first frame of animation over time
		m_fTweenTime -= time;
		if ( m_fTweenTime < 0 ) m_fTweenTime = 0;

		float t = 1 - (m_fTweenTime / m_fTotalTweenTime);
		
		for( UINT i = 0; i < m_iNumBones; i++ )
		{
			m_pBones[ i ].Tween( m_fPrevTime, m_fCurrTime, t );
		}

		for( UINT i = 0; i < m_iNumSlots; i++ )
		{
			m_pSlots[ i ].Tween( m_fPrevTime, m_fCurrTime, t );
		}
	}
	else
	{
		// interpolate the animation
		if ( m_bFlags & AGK_SKELETON_PLAYING )
		{
			m_fCurrTime += time*m_fSpeed;
			if ( m_fCurrTime > m_pAnimations[ m_iCurrAnimation ].m_fTime )
			{
				m_iLoopCount++;
				if ( (m_bFlags & AGK_SKELETON_LOOPING) || (m_iLoopTotal > 0 && m_iLoopCount < m_iLoopTotal) )
				{
					while ( m_fCurrTime > m_pAnimations[ m_iCurrAnimation ].m_fTime ) m_fCurrTime -= m_pAnimations[ m_iCurrAnimation ].m_fTime;
				}
				else
				{
					m_fCurrTime = m_pAnimations[ m_iCurrAnimation ].m_fTime;
					m_bFlags &= ~AGK_SKELETON_PLAYING;
				}
			}
			else if ( m_fCurrTime < 0 )
			{
				m_iLoopCount++;
				if ( (m_bFlags & AGK_SKELETON_LOOPING) || (m_iLoopTotal > 0 && m_iLoopCount < m_iLoopTotal) )
				{
					while ( m_fCurrTime < 0 ) m_fCurrTime += m_pAnimations[ m_iCurrAnimation ].m_fTime;
				}
				else 
				{
					m_fCurrTime = 0;
					m_bFlags &= ~AGK_SKELETON_PLAYING;
				}
			}
		}
			
		for( UINT i = 0; i < m_iNumBones; i++ )
		{
			m_pBones[ i ].Interpolate( m_fCurrTime );
		}

		for( UINT i = 0; i < m_iNumSlots; i++ )
		{
			m_pSlots[ i ].Interpolate( m_fCurrTime );
		}
	}

	for( UINT i = 0; i < m_iNumBones; i++ )
	{
		m_pBones[ i ].UpdateWorldMatrix( m_bFlags & AGK_SKELETON_FLIPH ? 1 : 0, m_bFlags & AGK_SKELETON_FLIPV ? 1 : 0 );
	}

	for( UINT i = 0; i < m_iNumSprites; i++ )
	{
		m_pSprites[ i ].Update( time );
	}
}

void Skeleton2D::Draw()
{
	if ( (m_bFlags & AGK_SKELETON_VISIBLE) == 0 ) return;
	if ( m_iNumSlots == 0 || !m_pSlots ) return;

	Slot2D **pOrderedSlots = new Slot2D*[ m_iNumSlots ];
	for( UINT i = 0; i < m_iNumSlots; i++ ) pOrderedSlots[ i ] = 0;
	
	for( UINT i = 0; i < m_iNumSlots; i++ )
	{
		pOrderedSlots[ m_pSlots[i].m_iZOrder ] = &(m_pSlots[i]);
	}

	ExternalSprite *pExtSprite = m_pFirstExtSprite;
	for( UINT i = 0; i < m_iNumSlots; i++ )
	{
		while ( pExtSprite && pExtSprite->m_iZOrder <= (int)i )
		{
			if ( pExtSprite->m_pSprite ) pExtSprite->m_pSprite->Draw();
			pExtSprite = pExtSprite->m_pNext;
		}

		if ( pOrderedSlots[i] && pOrderedSlots[i]->m_pSprite ) pOrderedSlots[i]->m_pSprite->Draw();
	}

	while ( pExtSprite )
	{
		if ( pExtSprite->m_pSprite ) pExtSprite->m_pSprite->Draw();
		pExtSprite = pExtSprite->m_pNext;
	}

	delete [] pOrderedSlots;
}

void Skeleton2D::DrawBones()
{
	if ( (m_bFlags & AGK_SKELETON_VISIBLE) == 0 ) return;

	AGKShader *pShader = AGKShader::g_pShaderColor;

	float *pVertices = new float[ m_iNumBones*12 ];
	UINT *pColor = new UINT[ m_iNumBones*6 ];

	int locPos = pShader->GetAttribPosition();
	int locColor = pShader->GetAttribColor();

	if ( locPos >= 0 ) pShader->SetAttribFloat( locPos, 2, 0, pVertices );
	if ( locColor >= 0 ) pShader->SetAttribUByte( locColor, 4, 0, true, (unsigned char*)pColor );

	for( UINT i = 0; i < m_iNumBones; i++ )
	{
		pVertices[ i*12 + 0 ] = agk::WorldToScreenX( m_pBones[ i ].worldX + m_pBones[ i ].m01*(m_pBones[ i ].length*0.06f) ); 
		pVertices[ i*12 + 1 ] = agk::WorldToScreenY( m_pBones[ i ].worldY + m_pBones[ i ].m11*(m_pBones[ i ].length*0.06f) );
		pVertices[ i*12 + 2 ] = agk::WorldToScreenX( m_pBones[ i ].worldX - m_pBones[ i ].m01*(m_pBones[ i ].length*0.06f) ); 
		pVertices[ i*12 + 3 ] = agk::WorldToScreenY( m_pBones[ i ].worldY - m_pBones[ i ].m11*(m_pBones[ i ].length*0.06f) );
		pVertices[ i*12 + 4 ] = agk::WorldToScreenX( m_pBones[ i ].worldX + m_pBones[ i ].m00*(m_pBones[ i ].length) );
		pVertices[ i*12 + 5 ] = agk::WorldToScreenY( m_pBones[ i ].worldY + m_pBones[ i ].m10*(m_pBones[ i ].length) );

		pVertices[ i*12 + 6 ] = agk::WorldToScreenX( m_pBones[ i ].worldX + m_pBones[ i ].m01*(m_pBones[ i ].length*0.045f) ); 
		pVertices[ i*12 + 7 ] = agk::WorldToScreenY( m_pBones[ i ].worldY + m_pBones[ i ].m11*(m_pBones[ i ].length*0.045f) );
		pVertices[ i*12 + 8 ] = agk::WorldToScreenX( m_pBones[ i ].worldX - m_pBones[ i ].m01*(m_pBones[ i ].length*0.045f) ); 
		pVertices[ i*12 + 9 ] = agk::WorldToScreenY( m_pBones[ i ].worldY - m_pBones[ i ].m11*(m_pBones[ i ].length*0.045f) );
		pVertices[ i*12 + 10 ] = agk::WorldToScreenX( m_pBones[ i ].worldX + m_pBones[ i ].m00*(m_pBones[ i ].length*0.75f) );
		pVertices[ i*12 + 11 ] = agk::WorldToScreenY( m_pBones[ i ].worldY + m_pBones[ i ].m10*(m_pBones[ i ].length*0.75f) );

		pColor[ i*6 + 0 ] = 0xff606060;
		pColor[ i*6 + 1 ] = 0xff606060;
		pColor[ i*6 + 2 ] = 0xff606060;

		pColor[ i*6 + 3 ] = 0xff969696;
		pColor[ i*6 + 4 ] = 0xff969696;
		pColor[ i*6 + 5 ] = 0xff969696;
	}
	
	agk::PlatformSetCullMode( 0 );
	pShader->DrawPrimitives( AGK_TRIANGLES, 0, m_iNumBones*6 );

	delete [] pVertices;
	delete [] pColor;
}

void Skeleton2D::DrawBoneNames()
{
	if ( (m_bFlags & AGK_SKELETON_VISIBLE) == 0 ) return;

	static cText* l_pText = 0;
	if ( !l_pText ) 
	{
		l_pText = new cText( 30 );
		l_pText->SetSize( agk::GetDeviceHeight() / 60.0f );
		l_pText->SetFont( 0 );
	}

	for( UINT i = 0; i < m_iNumBones; i++ )
	{
		float x = agk::WorldToScreenX( m_pBones[ i ].worldX + (m_pBones[ i ].m00*(m_pBones[ i ].length+1))/2 );
		float y = agk::WorldToScreenY( m_pBones[ i ].worldY + (m_pBones[ i ].m10*(m_pBones[ i ].length+1))/2 );

		l_pText->SetString( m_pBones[ i ].m_sName );
		l_pText->SetPosition( x, y );
		l_pText->Draw();
	}
}

// Animation2D

Animation2D::Animation2D()
{
	m_iNumBones = 0;
	m_pBoneAnims = 0;

	m_iNumSlots = 0;
	m_pSlotAnims = 0;

	m_fTime = 0;
}

Animation2D::~Animation2D()
{
	if ( m_pBoneAnims ) delete [] m_pBoneAnims;
	if ( m_pSlotAnims ) delete [] m_pSlotAnims;
}

Anim2DBone* Animation2D::GetAnimForBone( const char* name )
{
	if ( !name ) return 0;
	
	for ( UINT i = 0; i < m_iNumBones; i++ )
	{
		if ( m_pBoneAnims[ i ].m_sBoneName.CompareTo( name ) == 0 ) return m_pBoneAnims + i;
	}

	return 0;
}

Anim2DSlot* Animation2D::GetAnimForSlot( const char* name )
{
	if ( !name ) return 0;
	
	for ( UINT i = 0; i < m_iNumSlots; i++ )
	{
		if ( m_pSlotAnims[ i ].m_sSlotName.CompareTo( name ) == 0 ) return m_pSlotAnims + i;
	}

	return 0;
}

Anim2DBone* Animation2D::GetAnimForBoneTimeline( int timelineID )
{
	for ( UINT i = 0; i < m_iNumBones; i++ )
	{
		if ( m_pBoneAnims[ i ].m_iTimelineID == timelineID ) return m_pBoneAnims + i;
	}

	return 0;
}

Anim2DSlot* Animation2D::GetAnimForSlotTimeline( int timelineID )
{
	for ( UINT i = 0; i < m_iNumSlots; i++ )
	{
		if ( m_pSlotAnims[ i ].m_iTimelineID == timelineID ) return m_pSlotAnims + i;
	}

	return 0;
}

// bone animation

Anim2DBone::Anim2DBone() 
{
	m_iBoneIndex = -1;
	m_iTimelineID = -1;

	m_iNumRotations = 0;
	m_pRotations = 0;

	m_iNumPositions = 0;
	m_pPositions = 0;

	m_iNumScales = 0;
	m_pScales = 0;

	m_iNumParents = 0;
	m_pParents = 0;
}

Anim2DBone::~Anim2DBone() 
{
	if ( m_pRotations ) 
	{
		for ( UINT i = 0; i < m_iNumRotations; i++ ) delete m_pRotations[ i ];
		delete [] m_pRotations;
	}

	if ( m_pPositions ) 
	{
		for ( UINT i = 0; i < m_iNumPositions; i++ ) delete m_pPositions[ i ];
		delete [] m_pPositions;
	}

	if ( m_pScales ) 
	{
		for ( UINT i = 0; i < m_iNumScales; i++ ) delete m_pScales[ i ];
		delete [] m_pScales;
	}

	if ( m_pParents ) 
	{
		for ( UINT i = 0; i < m_iNumParents; i++ ) delete m_pParents[ i ];
		delete [] m_pParents;
	}
}

// slot animation

Anim2DSlot::Anim2DSlot() 
{
	m_iSlotIndex = -1;
	m_iTimelineID = -1;

	m_iNumColors = 0;
	m_pColors = 0;

	m_iNumAttachments = 0;
	m_pAttachments = 0;

	m_iNumBones = 0;
	m_pBones = 0;

	m_iNumZOrder = 0;
	m_pZOrder = 0;

	m_iNumPositions = 0;
	m_pPositions = 0;

	m_iNumRotations = 0;
	m_pRotations = 0;

	m_iNumScales = 0;
	m_pScales = 0;
}

Anim2DSlot::~Anim2DSlot() 
{
	if ( m_pColors ) 
	{
		for ( UINT i = 0; i < m_iNumColors; i++ ) delete m_pColors[ i ];
		delete [] m_pColors;
	}

	if ( m_pAttachments ) 
	{
		for ( UINT i = 0; i < m_iNumAttachments; i++ ) delete m_pAttachments[ i ];
		delete [] m_pAttachments;
	}

	if ( m_pBones ) 
	{
		for ( UINT i = 0; i < m_iNumBones; i++ ) delete m_pBones[ i ];
		delete [] m_pBones;
	}

	if ( m_pZOrder ) 
	{
		for ( UINT i = 0; i < m_iNumZOrder; i++ ) delete m_pZOrder[ i ];
		delete [] m_pZOrder;
	}

	if ( m_pPositions ) 
	{
		for ( UINT i = 0; i < m_iNumPositions; i++ ) delete m_pPositions[ i ];
		delete [] m_pPositions;
	}

	if ( m_pRotations ) 
	{
		for ( UINT i = 0; i < m_iNumRotations; i++ ) delete m_pRotations[ i ];
		delete [] m_pRotations;
	}

	if ( m_pScales ) 
	{
		for ( UINT i = 0; i < m_iNumScales; i++ ) delete m_pScales[ i ];
		delete [] m_pScales;
	}
}

// keyframes

void Anim2DKeyFrameColor::Interpolate( Anim2DKeyFrameColor* pNext, float t, unsigned char &red, unsigned char &green, unsigned char &blue, unsigned char &alpha )
{
	if ( !pNext ) 
	{
		red = m_iRed;
		green = m_iGreen;
		blue = m_iBlue;
		alpha = m_iAlpha;
		return;
	}

	red = agk::Round( m_iRed + t*(pNext->m_iRed - m_iRed) );
	green = agk::Round( m_iGreen + t*(pNext->m_iGreen - m_iGreen) );
	blue = agk::Round( m_iBlue + t*(pNext->m_iBlue - m_iBlue) );
	alpha = agk::Round( m_iAlpha + t*(pNext->m_iAlpha - m_iAlpha) );			
}

void Anim2DKeyFrameColorCurved::Interpolate( Anim2DKeyFrameColor* pNext, float t, unsigned char &red, unsigned char &green, unsigned char &blue, unsigned char &alpha )
{
	float guess = 0.5f;
	float newGuess = t;
	int iter = 0;

	// use newton raphson to find s for t (t=beizer x axis)
	do
	{
		guess = newGuess;
		newGuess = guess - ( (EvaluateBezier(c1, c3, guess) - t) / EvaluateBezierDt(c1, c3, guess) );
		iter++;
	} while ( fabs(newGuess-guess) > 0.00001f && iter < 10 );

	// get new t (from bezier y axis) for this s
	t = EvaluateBezier(c2, c4, newGuess);

	red = agk::Round( m_iRed + t*(pNext->m_iRed - m_iRed) );
	green = agk::Round( m_iGreen + t*(pNext->m_iGreen - m_iGreen) );
	blue = agk::Round( m_iBlue + t*(pNext->m_iBlue - m_iBlue) );
	alpha = agk::Round( m_iAlpha + t*(pNext->m_iAlpha - m_iAlpha) );
}