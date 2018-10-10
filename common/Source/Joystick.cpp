#include "agk.h"

using namespace AGK;

// virtual button

cSprite* cVirtualButton::g_pButtonSprite = 0;
cSprite* cVirtualButton::g_pButtonDownSprite = 0;

void cVirtualButton::Zero()
{
	m_bPrevDown = false;
	m_bDown = false;
	
	m_fVirtualPosX = 0;
	m_fVirtualPosY = 0;
	m_fVirtualSizeX = 20;
	m_fVirtualSizeY = 20;

	m_pCapturedMouse = 0;
	m_pCapturedTouch = 0;

	m_pButtonSprite = UNDEF;
	m_pButtonDownSprite = UNDEF;
	m_pText = UNDEF;

	m_iRed = 255;
	m_iGreen = 255;
	m_iBlue = 255;
	m_iAlpha = 200;

	m_bActive = true;
	m_bVisible = true;
}

cVirtualButton::cVirtualButton( float x, float y, float size )
{
	Zero();

	if ( size < 1 ) size = 1;
	
	m_fVirtualPosX = x;
	m_fVirtualPosY = y;
	m_fVirtualSizeX = size;
	m_fVirtualSizeY = size;

	if ( g_pButtonSprite == UNDEF )
	{
		g_pButtonSprite = new cSprite( "/Button.png" );
		
		g_pButtonSprite->SetSize( 1, -1 );
		g_pButtonSprite->SetColor( m_iRed, m_iGreen, m_iBlue, m_iAlpha );
		g_pButtonSprite->SetDepth( 0 );
		g_pButtonSprite->FixToScreen( 1 );
	}

	if ( g_pButtonDownSprite == UNDEF )
	{
		g_pButtonDownSprite = new cSprite( "/ButtonDown.png" );
		
		g_pButtonDownSprite->SetSize( 1, -1 );
		g_pButtonDownSprite->SetColor( m_iRed, m_iGreen, m_iBlue, m_iAlpha );
		g_pButtonDownSprite->SetDepth( 0 );
		g_pButtonDownSprite->FixToScreen( 1 );
	}
}

cVirtualButton::~cVirtualButton()
{
	if ( m_pButtonSprite ) delete m_pButtonSprite;
	if ( m_pButtonDownSprite ) delete m_pButtonDownSprite;
	if ( m_pText ) delete m_pText;
}

void cVirtualButton::SetPosition( float x, float y )
{
	m_fVirtualPosX = x;
	m_fVirtualPosY = y;
}

void cVirtualButton::SetSize( float size )
{
	if ( size < 0.1f ) size = 0.1f;
	m_fVirtualSizeX = size;
	m_fVirtualSizeY = size;
}

void cVirtualButton::SetSize( float sizeX, float sizeY )
{
	if ( sizeX < 0.1f ) sizeX = 0.1f;
	if ( sizeY < 0.1f ) sizeY = 0.1f;
	m_fVirtualSizeX = sizeX;
	m_fVirtualSizeY = sizeY;
}

void cVirtualButton::SetColor( UINT red, UINT green, UINT blue )
{
	if ( red > 255 ) red = 255;
	if ( green > 255 ) green = 255;
	if ( blue > 255 ) blue = 255;

	m_iRed = red;
	m_iGreen = green;
	m_iBlue = blue;
}

void cVirtualButton::SetAlpha( UINT alpha )
{
	if ( alpha > 255 ) alpha = 255;

	m_iAlpha = alpha;
}

void cVirtualButton::SetActive( bool active )
{
	m_bActive = active;
}

void cVirtualButton::SetVisible( bool visible )
{
	m_bVisible = visible;
}

void cVirtualButton::SetUpImage( cImage *pImage )
{
	if ( m_pButtonSprite )
	{
		if ( pImage ) m_pButtonSprite->SetImage( pImage );
		else 
		{
			delete m_pButtonSprite;
			m_pButtonSprite = UNDEF;
		}
	}
	else
	{
		if ( pImage )
		{
			m_pButtonSprite = new cSprite( pImage );
			m_pButtonSprite->SetSize( 1, -1 );
			m_pButtonSprite->SetColor( m_iRed, m_iGreen, m_iBlue, m_iAlpha );
			m_pButtonSprite->SetDepth( 0 );
			m_pButtonSprite->FixToScreen( 1 );
		}
	}
}

void cVirtualButton::SetDownImage( cImage *pImage )
{
	if ( m_pButtonDownSprite )
	{
		if ( pImage ) m_pButtonDownSprite->SetImage( pImage );
		else 
		{
			delete m_pButtonDownSprite;
			m_pButtonDownSprite = UNDEF;
		}
	}
	else
	{
		if ( pImage )
		{
			m_pButtonDownSprite = new cSprite( pImage );
			m_pButtonDownSprite->SetSize( 1, -1 );
			m_pButtonDownSprite->SetColor( m_iRed, m_iGreen, m_iBlue, m_iAlpha );
			m_pButtonDownSprite->SetDepth( 0 );
			m_pButtonDownSprite->FixToScreen( 1 );
		}
	}
}

void cVirtualButton::SetText( const char *str )
{
	if ( !m_pText )
	{
		m_pText = new cText();
		m_pText->FixToScreen( 1 );
	}

	m_pText->SetString( str );
}

void cVirtualButton::Update()
{
	if ( !m_bActive )
	{
		m_bPrevDown = false;
		m_bDown = false;
		return;
	}

	bool input = false;

	if ( m_pCapturedMouse )
	{
		if ( !m_pCapturedMouse->GetLeftTrue() ) m_pCapturedMouse = 0;
		else
		{
			input = true;
		}
	}

	if ( m_pCapturedTouch )
	{
		if ( m_pCapturedTouch->GetReleased() ) m_pCapturedTouch = 0;
		else
		{
			input = true;
		}
	}

	m_bPrevDown = m_bDown;
	m_bDown = input;
}

bool cVirtualButton::GetHitTest( float x, float y )
{
	if ( !m_bActive ) return false;

	float m_fHalfSize = m_fVirtualSizeX / 2.0f;
	if ( x < m_fVirtualPosX - m_fHalfSize ) return false;
	if ( x > m_fVirtualPosX + m_fHalfSize ) return false;

	m_fHalfSize = (m_fVirtualSizeY * agk::GetStretchValue()) / 2.0f;
	if ( y < m_fVirtualPosY - m_fHalfSize ) return false;
	if ( y > m_fVirtualPosY + m_fHalfSize ) return false;

	return true;
}

void cVirtualButton::Draw()
{
	if ( !m_bVisible ) return;

	// find the correct sprite to use
	cSprite *pSprite = g_pButtonSprite;
	if ( m_bDown )
	{
		if ( m_pButtonDownSprite ) pSprite = m_pButtonDownSprite;
		else pSprite = g_pButtonDownSprite;
	}
	else
	{
		if ( m_pButtonSprite ) pSprite = m_pButtonSprite;
	}

	// draw it
	pSprite->SetSize( 1, -1, false );
	pSprite->SetPositionByOffset( m_fVirtualPosX, m_fVirtualPosY );
	pSprite->SetScaleByOffset( m_fVirtualSizeX, m_fVirtualSizeY );
	pSprite->SetColor( m_iRed, m_iGreen, m_iBlue, m_iAlpha );
	pSprite->Draw();

	// draw text
	if ( m_pText )
	{
		m_pText->SetAlignment( 1 );
		m_pText->SetPosition( m_fVirtualPosX, m_fVirtualPosY - m_fVirtualSizeY/8.0f );
		m_pText->SetSize( m_fVirtualSizeY/4.0f );
		m_pText->Draw();
	}
}

// virtual joystick

cSprite *cVirtualJoystick::g_pOuterSprite = 0;
cSprite *cVirtualJoystick::g_pInnerSprite = 0;
float cVirtualJoystick::g_fDeadZone = 0.15f;

void cVirtualJoystick::Zero()
{
	m_fX = 0;
	m_fY = 0;
	
	m_fVirtualPosX = 0;
	m_fVirtualPosY = 0;
	m_fVirtualSize = 20;

	m_pCapturedMouse = 0;
	m_pCapturedTouch = 0;

	m_pOuterSprite = UNDEF;
	m_pInnerSprite = UNDEF;

	m_iAlpha1 = 150;
	m_iAlpha2 = 200;

	m_bActive = true;
	m_bVisible = true;
}

cVirtualJoystick::cVirtualJoystick( float x, float y, float size )
{
	Zero();

	if ( size < 1 ) size = 1;
	
	m_fVirtualPosX = x;
	m_fVirtualPosY = y;
	m_fVirtualSize = size;

	if ( g_pOuterSprite == UNDEF )
	{
		g_pOuterSprite = new cSprite( "/JoystickOuter.png" );
		
		g_pOuterSprite->SetSize( 1, -1 );
		g_pOuterSprite->SetColor( 255,255,255,150 );
		g_pOuterSprite->SetDepth( 0 );
		g_pOuterSprite->FixToScreen( 1 );
	}

	if ( g_pInnerSprite == UNDEF )
	{
		g_pInnerSprite = new cSprite( "/JoystickInner.png" );
		
		g_pInnerSprite->SetSize( 0.7f, -1 );
		g_pInnerSprite->SetColor( 255,255,255,200 );
		g_pInnerSprite->SetDepth( 0 );
		g_pInnerSprite->FixToScreen( 1 );
	}
}

cVirtualJoystick::~cVirtualJoystick()
{
	if ( m_pOuterSprite ) delete m_pOuterSprite;
	if ( m_pInnerSprite ) delete m_pInnerSprite;
}

void cVirtualJoystick::SetPosition( float x, float y )
{
	m_fVirtualPosX = x;
	m_fVirtualPosY = y;
}

void cVirtualJoystick::SetSize( float size )
{
	if ( size < 1 ) size = 1;
	m_fVirtualSize = size;
}

void cVirtualJoystick::SetAlpha( UINT alpha1, UINT alpha2 )
{
	if ( alpha1 > 255 ) alpha1 = 255;
	if ( alpha2 > 255 ) alpha2 = 255;

	m_iAlpha1 = alpha1;
	m_iAlpha2 = alpha2;
}

void cVirtualJoystick::SetActive( bool active )
{
	m_bActive = active;
}

void cVirtualJoystick::SetVisible( bool visible )
{
	m_bVisible = visible;
}

void cVirtualJoystick::SetInnerImage( cImage *pImage )
{
	if ( m_pInnerSprite )
	{
		if ( pImage ) m_pInnerSprite->SetImage( pImage );
		else 
		{
			delete m_pInnerSprite;
			m_pInnerSprite = UNDEF;
		}
	}
	else
	{
		if ( pImage )
		{
			m_pInnerSprite = new cSprite( pImage );
			m_pInnerSprite->SetSize( 0.7f, -1 );
			m_pInnerSprite->SetColor( 255,255,255,150 );
			m_pInnerSprite->SetDepth( 0 );
			m_pInnerSprite->FixToScreen( 1 );
		}
	}
}

void cVirtualJoystick::SetOuterImage( cImage *pImage )
{
	if ( m_pOuterSprite )
	{
		if ( pImage ) m_pOuterSprite->SetImage( pImage );
		else 
		{
			delete m_pOuterSprite;
			m_pOuterSprite = UNDEF;
		}
	}
	else
	{
		if ( pImage )
		{
			m_pOuterSprite = new cSprite( pImage );
			m_pOuterSprite->SetSize( 1, -1 );
			m_pOuterSprite->SetColor( 255,255,255,150 );
			m_pOuterSprite->SetDepth( 0 );
			m_pOuterSprite->FixToScreen( 1 );
		}
	}
}

void cVirtualJoystick::Update()
{
	if ( !m_bActive )
	{
		m_fX = 0;
		m_fY = 0;
		return;
	}

	float x = m_fVirtualPosX;
	float y = m_fVirtualPosY;

	if ( m_pCapturedMouse )
	{
		if ( !m_pCapturedMouse->GetLeftTrue() ) m_pCapturedMouse = 0;
		else
		{
			x = m_pCapturedMouse->GetX();
			y = m_pCapturedMouse->GetY();
		}
	}

	if ( m_pCapturedTouch )
	{
		if ( m_pCapturedTouch->GetReleased() ) m_pCapturedTouch = 0;
		else
		{
			x = m_pCapturedTouch->GetCurrentX();
			y = m_pCapturedTouch->GetCurrentY();
		}
	}

	float relX = x - m_fVirtualPosX;
	float relY = y - m_fVirtualPosY;

	relY /= agk::GetStretchValue();

	relX /= (m_fVirtualSize/2.0f);
	relY /= (m_fVirtualSize/2.0f);

	float length = relX*relX + relY*relY;
	if ( length > 1.0f )
	{
		length = agk::Sqrt( length );
		relX = relX / length;
		relY = relY / length;
	}

	m_fX = relX;
	m_fY = relY;
}

float cVirtualJoystick::GetX() 
{ 
	if ( agk::Abs( m_fX ) < g_fDeadZone ) return 0;
	else return m_fX; 
}

float cVirtualJoystick::GetY() 
{ 
	if ( agk::Abs( m_fY ) < g_fDeadZone ) return 0;
	else return m_fY; 
}

bool cVirtualJoystick::GetHitTest( float x, float y )
{
	if ( !m_bActive ) return false;

	float m_fHalfSize = m_fVirtualSize / 2.0f;
	if ( x < m_fVirtualPosX - m_fHalfSize ) return false;
	if ( x > m_fVirtualPosX + m_fHalfSize ) return false;

	m_fHalfSize *= agk::GetStretchValue();
	if ( y < m_fVirtualPosY - m_fHalfSize ) return false;
	if ( y > m_fVirtualPosY + m_fHalfSize ) return false;

	return true;
}

void cVirtualJoystick::Draw()
{
	if ( !m_bVisible ) return;

	// check for local sprites, otherwise use the global sprites
	cSprite *pOuter = g_pOuterSprite;
	if ( m_pOuterSprite ) pOuter = m_pOuterSprite;

	cSprite *pInner = g_pInnerSprite;
	if ( m_pInnerSprite ) pInner = m_pInnerSprite;

	// draw them
	pOuter->SetSize( 1, -1 );
	pOuter->SetPositionByOffset( m_fVirtualPosX, m_fVirtualPosY );
	pOuter->SetScaleByOffset( m_fVirtualSize, m_fVirtualSize );
	pOuter->SetAlpha( m_iAlpha1 );
	pOuter->Draw();

	pInner->SetSize( 0.7f, -1 );
	pInner->SetPositionByOffset( m_fVirtualPosX + m_fX*(m_fVirtualSize/8.0f), m_fVirtualPosY + (m_fY*(m_fVirtualSize/8.0f))*agk::GetStretchValue() );
	pInner->SetScaleByOffset( m_fVirtualSize, m_fVirtualSize );
	pInner->SetAlpha( m_iAlpha2 );
	pInner->Draw();
}

// real joystick

float cJoystick::g_fDeadZone = 0.15f;

void cJoystick::Zero()
{
	m_pDevice = UNDEF;
	m_iDeviceType = 0;

	m_iConnected = 1;

	m_fX = 0;
	m_fY = 0;
	m_fZ = 0;

	m_fRX = 0;
	m_fRY = 0;
	m_fRZ = 0;

	m_iSlider[ 0 ] = 0;
	m_iSlider[ 1 ] = 0;

	m_iPOV[ 0 ] = 0;
	m_iPOV[ 1 ] = 0;
	m_iPOV[ 2 ] = 0;
	m_iPOV[ 3 ] = 0;

	m_iNumButtons = 0;
	for ( UINT i = 0; i < AGK_MAX_JOYSTICK_BUTTONS; i++ ) m_iPrevButtons[ i ] = 0;
	for ( UINT i = 0; i < AGK_MAX_JOYSTICK_BUTTONS; i++ ) m_iButtons[ i ] = 0;
	for ( UINT i = 0; i < AGK_MAX_JOYSTICK_BUTTONS; i++ ) m_iResetButtons[ i ] = 0;
}

cJoystick::cJoystick( void *pDevice )
{
	Zero();
	
	m_pDevice = pDevice;
}

cJoystick::cJoystick( void *pDevice, int devicetype )
{
	Zero();
	
	m_pDevice = pDevice;
	m_iDeviceType = devicetype;
}

float cJoystick::GetX() 
{ 
	if ( agk::Abs(m_fX) < g_fDeadZone ) return 0;
	else return m_fX; 
}

float cJoystick::GetY() 
{ 
	if ( agk::Abs(m_fY) < g_fDeadZone ) return 0;
	else return m_fY; 
}

float cJoystick::GetZ() 
{ 
	if ( agk::Abs(m_fZ) < g_fDeadZone ) return 0;
	else return m_fZ; 
}

float cJoystick::GetRX() 
{ 
	if ( agk::Abs(m_fRX) < g_fDeadZone ) return 0;
	else return m_fRX; 
}

float cJoystick::GetRY() 
{ 
	if ( agk::Abs(m_fRY) < g_fDeadZone ) return 0;
	else return m_fRY; 
}

float cJoystick::GetRZ() 
{ 
	if ( agk::Abs(m_fRZ) < g_fDeadZone ) return 0;
	else return m_fRZ; 
}

int cJoystick::GetSlider( UINT slider )
{
	if ( slider > 1 ) return 0;
	return m_iSlider[ slider ];
}

int cJoystick::GetPOV( UINT pov )
{
	if ( pov > 1 ) return 0;
	return m_iPOV[ pov ];
}

void cJoystick::Update()
{
	for ( UINT i = 0; i < AGK_MAX_JOYSTICK_BUTTONS; i++ ) 
	{
		m_iPrevButtons[ i ] = m_iButtons[ i ];

		if ( m_iResetButtons[ i ] )
		{
			m_iButtons[ i ] = 0;
			m_iResetButtons[ i ] = 0;
		}
	}

	PlatformUpdate();
}
