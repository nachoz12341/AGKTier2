// includes
#include "agk.h"
#include "interpreter.h"
#include "GLFW/glfw3.h"

int done = 0;

void PlatformAppQuit()
{
    //exit(0);
	done = 1;
}

// platform specific
unsigned int TranslateKey( unsigned int key )
{
	switch( key )
	{
		case GLFW_KEY_ESCAPE: key = 27; break; // Esc
			
		// F1 - F8
		case GLFW_KEY_F1: key = 112; break;
		case GLFW_KEY_F2: key = 113; break;
		case GLFW_KEY_F3: key = 114; break;
		case GLFW_KEY_F4: key = 115; break;
		case GLFW_KEY_F5: key = 116; break;
		case GLFW_KEY_F6: key = 117; break;
		case GLFW_KEY_F7: key = 118; break;
		case GLFW_KEY_F8: key = 119; break;
			
		case 96: key = 223; break; // `
		case 45: key = 189; break; // -
		case 61: key = 187; break; // =
		case GLFW_KEY_HOME: key = 36; break; // Home
		case GLFW_KEY_PAGE_UP: key = 33; break; // PgUp
		case GLFW_KEY_PAGE_DOWN: key = 34; break; // PgDown
		case GLFW_KEY_END: key = 35; break; // End
		case GLFW_KEY_DELETE: key = 46; break; // Del
		//case 160: key = 223; break;  // +-
		case GLFW_KEY_INSERT: key = 45; break; // Insert
			
		case GLFW_KEY_LEFT_SHIFT: key = 16; break; // Shift (left)
		case GLFW_KEY_RIGHT_SHIFT: key = 16; break; // Shift (right)
		case GLFW_KEY_LEFT_CONTROL: key = 17; break; // Ctrl (left)
		case GLFW_KEY_RIGHT_CONTROL: key = 17; break; // Ctrl (right)
		case GLFW_KEY_LEFT_ALT: key = 0; break;  // Alt/Cmd (left)
		case GLFW_KEY_RIGHT_ALT: key = 18; break;  // Cmd (right)
			
		case GLFW_KEY_TAB: key = 9; break; // Tab
		case GLFW_KEY_ENTER: key = 13; break; // Enter
		case GLFW_KEY_BACKSPACE: key = 8; break; // Backspace
			
		case 91: key = 219; break; // [
		case 93: key = 221; break; // ]
		case 59: key = 186; break; // ;
		case 39: key = 192; break; // '
			
		case GLFW_KEY_LEFT: key = 37; break; // Left
		case GLFW_KEY_UP: key = 38; break; // Up
		case GLFW_KEY_RIGHT: key = 39; break; // Right
		case GLFW_KEY_DOWN: key = 40; break; // Down
			
		case 44: key = 188; break; // ,
		case 46: key = 190; break; // .
		case 47: key = 191; break; // /
		case 92: key = 220; break; // \
			
		case GLFW_KEY_KP_ENTER: key = 13; break; // Num pad enter
		case GLFW_KEY_KP_DIVIDE: key = 111; break; // Num pad divide 
		case GLFW_KEY_KP_MULTIPLY: key = 106; break; // Num pad multiply
		case GLFW_KEY_KP_SUBTRACT: key = 109; break; // Num pad subtract
		case GLFW_KEY_KP_ADD: key = 107; break; // Num pad add
		case GLFW_KEY_KP_DECIMAL: key = 110; break; // Num pad decimcal
		case GLFW_KEY_PAUSE: key = 0; break; // Pause/Break
		case GLFW_KEY_CAPS_LOCK: key = 0; break; // Caps lock 
		case GLFW_KEY_KP_0: key = 45; break; // Num pad 0
		case GLFW_KEY_KP_1: key = 35; break; // Num pad 1
		case GLFW_KEY_KP_2: key = 40; break; // Num pad 2
		case GLFW_KEY_KP_3: key = 34; break; // Num pad 3
		case GLFW_KEY_KP_4: key = 37; break; // Num pad 4
		case GLFW_KEY_KP_5: key = 12; break; // Num pad 5
		case GLFW_KEY_KP_6: key = 39; break; // Num pad 6
		case GLFW_KEY_KP_7: key = 36; break; // Num pad 7
		case GLFW_KEY_KP_8: key = 38; break; // Num pad 8
		case GLFW_KEY_KP_9: key = 33; break; // Num pad 9
	}
	
	return key;
}

static void keyboard_callback( GLFWwindow* window, int key, int scancode, int action, int mods )
{
	if ( action == GLFW_PRESS )
	{
		int key2 = TranslateKey(key);
		if ( key2 > 255 ) return;
		agk::KeyDown( key2 );
	}
	else if ( action == GLFW_RELEASE )
	{
		int key2 = TranslateKey(key);
		if ( key2 > 255 ) return;
		agk::KeyUp( key2 );
	}

}

static void char_callback( GLFWwindow* window, unsigned int key )
{
	agk::CharDown( key );
}

static void mouse_pos_callback( GLFWwindow* window, double x, double y )
{
	agk::MouseMove( 0, agk::Round(x), agk::Round(y) );
}

static void mouse_button_callback( GLFWwindow* window, int button, int action, int mods )
{
	if ( button == GLFW_MOUSE_BUTTON_LEFT )
	{
		if ( action == GLFW_PRESS ) agk::MouseLeftButton( 0, 1 );
		else if ( action == GLFW_RELEASE ) agk::MouseLeftButton( 0, 0 );
	}
	else if ( button == GLFW_MOUSE_BUTTON_RIGHT )
	{
		if ( action == GLFW_PRESS ) agk::MouseRightButton( 0, 1 );
		else if ( action == GLFW_RELEASE ) agk::MouseRightButton( 0, 0 );
	}
	else if ( button == GLFW_MOUSE_BUTTON_MIDDLE )
	{
		if ( action == GLFW_PRESS ) agk::MouseMiddleButton( 0, 1 );
		else if ( action == GLFW_RELEASE ) agk::MouseMiddleButton( 0, 0 );
	}
}

static void mouse_wheel_callback( GLFWwindow* window, double x, double y )
{
    agk::MouseWheel(0, y);
}

static void window_size_callback( GLFWwindow* window, int width, int height)
{
	agk::UpdateDeviceSize();
}

static void close_callback( GLFWwindow* window )
{
    App.AppSplash();
    glfwSetWindowShouldClose( window, 1 );
}

static void focus_callback( GLFWwindow* window, int focus )
{
    if ( focus == GLFW_TRUE ) agk::Resumed();
	else agk::Paused();
}

void Output ( char const* debugtext )
{
	// report this text to the internal debug reporting system for MAC
}

struct egldata
{
	GLFWwindow* window;
};

int main (int argc, char **argv)
{
    App.g_dwDeviceWidth = DEVICE_WIDTH;
	App.g_dwDeviceHeight = DEVICE_HEIGHT;
	App.g_dwFullScreen = FULLSCREEN ? 1 : 0;

	char* pSetupFile = (char*)"setup.agc";
	if ( agk::GetFileExists ( pSetupFile )==1 )
	{
		char* pField = (char*)"";
		strcpy ( App.g_pWindowTitle, "" );
		agk::OpenToRead ( 1, pSetupFile );
		while ( agk::FileEOF ( 1 )==false ) 
		{
			char* pLineToRead = agk::ReadLine ( 1 );
			pField=(char*)"title="; if ( strncmp ( pLineToRead, pField, strlen(pField) )==0 )	strcpy ( App.g_pWindowTitle, pLineToRead+strlen(pField) );
			pField=(char*)"width="; if ( strncmp ( pLineToRead, pField, strlen(pField) )==0 )	App.g_dwDeviceWidth = (unsigned int)atoi(pLineToRead+strlen(pField));
			pField=(char*)"height="; if ( strncmp ( pLineToRead, pField, strlen(pField) )==0 )	App.g_dwDeviceHeight = (unsigned int)atoi(pLineToRead+strlen(pField));
			pField=(char*)"fullscreen="; if ( strncmp ( pLineToRead, pField, strlen(pField) )==0 )	App.g_dwFullScreen = (unsigned int)atoi(pLineToRead+strlen(pField));
			pField=(char*)"resolutionmode="; if ( strncmp ( pLineToRead, pField, strlen(pField) )==0 )	App.g_dwResolutionMode = (unsigned int)atoi(pLineToRead+strlen(pField));
		}
		agk::CloseFile ( 1 );
	}
    
	// create a window for the app
	if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        exit(1);
    }
    
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode( monitor );
    
    if ( App.g_dwFullScreen == 1 ) 
	{
		App.g_dwDeviceWidth = mode->width;
		App.g_dwDeviceHeight = mode->height;
	}
	else 
	{
		monitor = 0;
    
		int width = App.g_dwDeviceWidth;
		int height = App.g_dwDeviceHeight;
		
		float appAspect = width / (float) height;
		float windowAspect = (mode->width-15) / (float) (mode->height-80);
		
		if ( appAspect > windowAspect )
		{
			if ( width > mode->width-15 )
			{
				float ratio = (mode->width-15) / (float)width;
				width = mode->width-15;
				height = (int) height*ratio;
			}
		}
		else
		{
			if ( height > mode->height-80 )
			{
				float ratio = (mode->height-80) / (float)height;
				height = mode->height-80;
				width = (int) width*ratio;
			}
		}
		
		App.g_dwDeviceWidth = width;
		App.g_dwDeviceHeight = height;
	}
    
    glfwWindowHint( GLFW_RED_BITS, 8 );
    glfwWindowHint( GLFW_GREEN_BITS, 8 );
    glfwWindowHint( GLFW_BLUE_BITS, 8 );
    glfwWindowHint( GLFW_ALPHA_BITS, 8 );
    glfwWindowHint( GLFW_DEPTH_BITS, 24 );

    glfwWindowHint( GLFW_CLIENT_API, GLFW_OPENGL_API );    
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 2 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 0 );

	glfwWindowHint( GLFW_SAMPLES, 4 );
    
	GLFWwindow *window = glfwCreateWindow(App.g_dwDeviceWidth, App.g_dwDeviceHeight, App.g_pWindowTitle, monitor, NULL);
	if ( !window )
    {
        glfwTerminate();
        fprintf(stderr, "Failed to open GLFW window\n");
        exit(1);
    }
    
    glfwMakeContextCurrent(window);
    
	glfwSetWindowSizeCallback( window, window_size_callback );
	glfwSetKeyCallback( window, keyboard_callback );
	glfwSetCursorPosCallback( window, mouse_pos_callback );
	glfwSetMouseButtonCallback( window, mouse_button_callback );
	glfwSetScrollCallback( window, mouse_wheel_callback );
	glfwSetCharCallback( window, char_callback );
    glfwSetWindowCloseCallback( window, close_callback );
	glfwSetWindowFocusCallback( window, focus_callback );
	
	// initialise graphics API for app
    agk::SetExtraAGKPlayerAssetsMode ( 2 );
		
	// call app begin
	try
	{
		egldata data;
		data.window = window;
		agk::InitGL(&data);

		App.Begin();
	}
	catch( ... )
	{
		uString err = agk::GetLastError();
		err.Prepend( "Uncaught exception: \n\n" );
		/*
		NSString* pString = [ [ NSString alloc ] initWithUTF8String:err.GetStr() ];
		NSAlert *alert = [[NSAlert alloc] init];
		[ alert setMessageText: pString ];
		[ pString release ];
		[ alert runModal ];
		[ alert release ];
		*/
		done = 1;
	}
	while (!done && !glfwWindowShouldClose(window))
	{
		// call app each frame
		try
		{
            App.Loop();
            glfwPollEvents();
		}
		catch( ... )
		{
			uString err = agk::GetLastError();
			err.Prepend( "Uncaught exception: \n\n" );
			/*
			NSString* pString = [ [ NSString alloc ] initWithUTF8String:err.GetStr() ];
			NSAlert *alert = [[NSAlert alloc] init];
			[ alert setMessageText: pString ];
			[ pString release ];
			[ alert runModal ];
			[ alert release ];
			*/
			done = 1;
		}
	}
	
	// close app
	App.End();
    agk::CleanUp();
	
	// close window system
	glfwDestroyWindow(window);
    glfwTerminate();
	
	// success    
    return 0;
}
