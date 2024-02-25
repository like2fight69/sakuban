#include "SDL.h"
#include "SDL_image.h"
//use code from knight project in linux folder
//Screen dimension constants
#include <stdio.h>
#include <string>

/*NOTES
Collide with box:
Change sprite on button down:DONE
Animate sprite:DONE
seperate idle sprite form walking sprites
add box sprite:DONE
*/

//The dimensions of the level
const int LEVEL_WIDTH = 1280;
const int LEVEL_HEIGHT = 960;

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Gravity variables

float myGravity = 5.4f;//0.2f
float maxFallSpeed = -5.0f;
float myJumpForce = 5.0f;
float curJumpForce = 0.0f;
float deltaTime = 1.0f;

bool m_jumping = false;
bool is_Moving = false;
bool is_Moving_Up = false;
bool is_Moving_Down = false;
bool is_Moving_Left = false;
bool is_Moving_Right = false;
const int WALKING_ANIMATION_FRAMES = 4;
SDL_Rect gSpriteClips[ WALKING_ANIMATION_FRAMES ];


//std::string rightFrames[4] = {"dot.bmp","dot1.bmp","dot2.bmp","dot3.bmp"};
//Texture wrapper class
class LTexture
{
	public:
		//Initializes variables
		LTexture();

		//Deallocates memory
		~LTexture();

		//Loads image at specified path
		bool loadFromFile( std::string path );

		#if defined(_SDL_TTF_H) || defined(SDL_TTF_H)
		//Creates image from font string
		bool loadFromRenderedText( std::string textureText, SDL_Color textColor );
		#endif

		//Deallocates texture
		void free();

		//Set color modulation
		void setColor( Uint8 red, Uint8 green, Uint8 blue );

		//Set blending
		void setBlendMode( SDL_BlendMode blending );

		//Set alpha modulation
		void setAlpha( Uint8 alpha );

		//Renders texture at given point
		void render( int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE );

		//Gets image dimensions
		int getWidth();
		int getHeight();
        void setSize(int w,int h);
	private:
		//The actual hardware texture
		SDL_Texture* mTexture;

		//Image dimensions
		int mWidth;
		int mHeight;
};

//The dot that will move around on the screen
class Dot
{
    public:
		//The dimensions of the dot
		static const int DOT_WIDTH = 40;
		static const int DOT_HEIGHT = 40;

		//Maximum axis velocity of the dot
		static const int DOT_VEL = 10;


		//Initializes the variables
		Dot();

		//Takes key presses and adjusts the dot's velocity
		void handleEvent( SDL_Event& e );

		//Moves the dot
		void move();
        void animate();
		//Shows the dot on the screen relative to the camera
		void render( int camX, int camY );
        //bool check_Collision(Dot A,Dot B);
		//Position accessors
		int getPosX();
		int getPosY();
                bool isAffectedByGravity;
int mPosX, mPosY;
    private:
		//The X and Y offsets of the dot
		//int mPosX, mPosY;

		//The velocity of the dot
		int mVelX, mVelY;
};
//test
class Platform
{
    public:
                //The dimensions of the dot
                static const int PLATFORM_WIDTH = 50;
                static const int DOT_HEIGHT = 20;

                //Maximum axis velocity of the dot
                static const int PLATFORM_VEL = 10;


                //Initializes the variables
                Platform();

                //Takes key presses and adjusts the dot's velocity
                void handleEvent( SDL_Event& e );

                //Moves the dot
                void move();

                //Shows the dot on the screen relative to the camera
                void render( int camX, int camY );

                //Position accessors
                int getPosX();
                int getPosY();
                bool isAffectedByGravity;

    private:
                //The X and Y offsets of the dot
                int mPosX, mPosY;

                //The velocity of the dot
                int mVelX, mVelY;
};


Platform platform;
//rename Dot with entity
 Dot dot;
 Dot box;

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Scene textures
LTexture gDotTexture;
LTexture gBGTexture;
LTexture gPlatformTexture;
LTexture gSpriteSheetTexture;
LTexture gBoxTexture;
//test

bool check_collision( Dot A, Dot B )
{

    //The sides of the rectangles
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;

    //Calculate the sides of rect A
    leftA = A.mPosX;
    rightA = A.mPosX + A.DOT_WIDTH;
    topA = A.mPosY;
    bottomA = A.mPosY + A.DOT_HEIGHT;

    //Calculate the sides of rect B
    leftB = B.mPosX;
    rightB = B.mPosX + B.DOT_WIDTH;
    topB = B.mPosY;
    bottomB = B.mPosY + B.DOT_HEIGHT;
//If any of the sides from A are outside of B
    if( bottomA <= topB )
    {
        return false;
    }

    if( topA >= bottomB )
    {
        return false;
    }

    if( rightA <= leftB )
    {
        return false;
    }

    if( leftA >= rightB )
    {
        return false;
    }

    //If none of the sides from A are outside B
    return true;
}



//test
LTexture::LTexture()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	//Deallocate
	free();
}

bool LTexture::loadFromFile( std::string path )
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else
	{
		//Color key image
		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );

		//Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
		if( newTexture == NULL )
		{
			printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}

#if defined(_SDL_TTF_H) || defined(SDL_TTF_H)
bool LTexture::loadFromRenderedText( std::string textureText, SDL_Color textColor )
{
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid( gFont, textureText.c_str(), textColor );
	if( textSurface != NULL )
	{
		//Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface( gRenderer, textSurface );
		if( mTexture == NULL )
		{
			printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface( textSurface );
	}
	else
	{
		printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
	}


	//Return success
	return mTexture != NULL;
}
#endif
void LTexture::setSize(int w,int h)
{
    w = mWidth;
    h = mHeight;
}
void LTexture::free()
{
	//Free texture if it exists
	if( mTexture != NULL )
	{
		SDL_DestroyTexture( mTexture );
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
	//Modulate texture rgb
	SDL_SetTextureColorMod( mTexture, red, green, blue );
}

void LTexture::setBlendMode( SDL_BlendMode blending )
{
	//Set blending function
	SDL_SetTextureBlendMode( mTexture, blending );
}

void LTexture::setAlpha( Uint8 alpha )
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod( mTexture, alpha );
}

void LTexture::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip )
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	//Set clip rendering dimensions
	if( clip != NULL )
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	//Render to screen
	SDL_RenderCopyEx( gRenderer, mTexture, clip, &renderQuad, angle, center, flip );
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}
//test
Platform::Platform()
{



}
void Platform::render( int camX, int camY )
{
    //Show the dot relative to the camera
        gPlatformTexture.render( mPosX - camX, mPosY - camY );
}
int Platform::getPosX()
{
        return mPosX;
}

int Platform::getPosY()
{
        return mPosY;
}


//test
Dot::Dot()
{
    //Initialize the offsets
    //mPosX = 0;
   // mPosY = 100;

    //Initialize the velocity
    mVelX = 0;
    mVelY = 0;
}
/*
bool Dot::check_Collision(Dot A, Dot B)
{
//The sides of the rectangles
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;

    //Calculate the sides of rect A
    leftA = A.mPosX;
    rightA = A.mPosX + A.DOT_WIDTH;
    topA = A.mPosY;
    bottomA = A.mPosY + A.DOT_HEIGHT;

    //Calculate the sides of rect B
    leftB = B.mPosX;
    rightB = B.mPosX + B.DOT_WIDTH;
    topB = B.mPosY;
    bottomB = B.mPosY + B.DOT_HEIGHT;
//If any of the sides from A are outside of B
    if( bottomA <= topB )
    {
        return false;
    }

    if( topA >= bottomB )
    {
        return false;
    }

    if( rightA <= leftB )
    {
        return false;
    }

    if( leftA >= rightB )
    {
        return false;
    }

    //If none of the sides from A are outside B
    return true;

}*/
void Dot::handleEvent( SDL_Event& e )
{
    //If a key was pressed
	if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
    {
        //Adjust the velocity
        switch( e.key.keysym.sym )
        {   //mVelY -= DOT_VEL;
            case SDLK_UP: mVelY -= DOT_VEL; is_Moving_Up = true;is_Moving_Down = false;is_Moving_Right = false;is_Moving_Left = false;break;
            case SDLK_DOWN: mVelY += DOT_VEL; is_Moving = true; is_Moving_Down = true;is_Moving_Up = false;is_Moving_Right = false;is_Moving_Left = false;break;
            case SDLK_LEFT: mVelX -= DOT_VEL;is_Moving_Left = true;is_Moving_Right = false;is_Moving_Up = false;is_Moving_Down = false; break;
            case SDLK_RIGHT: mVelX += DOT_VEL; is_Moving_Right = true;is_Moving_Up = false; is_Moving_Down = false;is_Moving_Left = false;break;
        }
    }
    //area of animation change
    //If a key was released KEYUP
    else if( e.type == SDL_KEYUP && e.key.repeat == 0 )
    {
        //Adjust the velocity
        switch( e.key.keysym.sym )
        {    //might need to remove movement because it gets stuck looks like moon walking
            case SDLK_UP: mVelY += DOT_VEL;
              // gDotTexture.loadFromFile( "dot.bmp" );
              is_Moving_Up = true;
              is_Moving_Down = false;
              is_Moving_Right = false;
              is_Moving_Left = false;
              printf("move up");
               break;
            case SDLK_DOWN: mVelY -= DOT_VEL;
                // is_Moving = false;
                is_Moving_Down = true;
                is_Moving_Up = false;
                is_Moving_Right = false;
                is_Moving_Left = false;

               //gDotTexture.loadFromFile( "dot1.bmp" );
                printf("move down");
               break;
            case SDLK_LEFT: mVelX += DOT_VEL;
            is_Moving_Left = true;
            is_Moving_Right = false;
            is_Moving_Up = false;
            is_Moving_Down = false;

              // gDotTexture.loadFromFile( "dot2.bmp" );

              printf("move left");
               break;
            case SDLK_RIGHT: mVelX -= DOT_VEL;
               //gDotTexture.loadFromFile( "dot3.bmp" );
               is_Moving_Right = true;
               is_Moving_Up = false;
               is_Moving_Down = false;
               is_Moving_Left = false;
               printf("move right");
               break;
            case SDLK_SPACE:  m_jumping = true;
//            curJumpForce = myJumpForce;
//        dot.isAffectedByGravity = true;

      }
    }
}

void Dot::move()
{
 if(dot.isAffectedByGravity == true){
   deltaTime += myGravity;
  mPosY += deltaTime;
}
/*if(m_jumping){
//  curJumpForce = myJumpForce;
  // mPosY -= curJumpForce * deltaTime;
   mPosY -= deltaTime;
   if(deltaTime > maxFallSpeed){//curJumpForce
 //    myJumpForce += myGravity * deltaTime;
  //  mPosY += curJumpForce * deltaTime;
  deltaTime += myGravity;
  mPosY += deltaTime;
  }else{
      deltaTime = maxFallSpeed;
   }
}*/

    //Move the dot left or right
    mPosX += mVelX;

    //If the dot went too far to the left or right
    if( ( mPosX < 0 ) || ( mPosX + DOT_WIDTH > LEVEL_WIDTH ) )
    {
        //Move back
        mPosX -= mVelX;
    }

    //Move the dot up or down
    mPosY += mVelY;

    //If the dot went too far up or down
    if( ( mPosY < 0 ) || ( mPosY + DOT_HEIGHT > LEVEL_HEIGHT ) )
    {
        //Move back
        mPosY -= mVelY;
    }
}
void Dot::animate()
{
                    gSpriteClips[ 0 ].x =   36;//36
                    gSpriteClips[ 0 ].y =   33;//3
                    gSpriteClips[ 0 ].w =  27;//52
                    gSpriteClips[ 0 ].h = 31;//92

                    gSpriteClips[ 1 ].x =  57;//73
                    gSpriteClips[ 1 ].y =   33;//3
                    gSpriteClips[ 1 ].w = 27;//57
                    gSpriteClips[ 1 ].h = 31;//92

}
void Dot::render( int camX, int camY )
{
     //try tagging objects
	//gDotTexture.render( mPosX - camX, mPosY - camY );

	//box.mPosX = 300;
	//box.mPosY = 250;
	gBoxTexture.render(mPosX - camX, mPosY - camY);

}

int Dot::getPosX()
{
	return mPosX;
}

int Dot::getPosY()
{
	return mPosY;
}

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}

		//Create window
		gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create vsynced renderer for window
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
			if( gRenderer == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;
      //Platform texture
   if( !gPlatformTexture.loadFromFile( "block.png" ) )
        {
                printf( "Failed to load dot texture!\n" );
                success = false;
        }
    //load box
     if( !gBoxTexture.loadFromFile( "box.png" ) )
        {
                printf( "Failed to load dot texture!\n" );
                success = false;
        }
	//Load dot texture
	if( !gDotTexture.loadFromFile( "image.png" ) )//foo.bmp
	{
		printf( "Failed to load dot texture!\n" );
		success = false;
	}
	 else
    {
        //Set sprite clips
        gSpriteClips[ 0 ].x =   36;//36
        gSpriteClips[ 0 ].y =   2;//2
        gSpriteClips[ 0 ].w =  27;//52
        gSpriteClips[ 0 ].h = 31;//92

        gSpriteClips[ 1 ].x =  66;//66
        gSpriteClips[ 1 ].y =   2;//2
        gSpriteClips[ 1 ].w = 27;//57
        gSpriteClips[ 1 ].h = 31;//92

       /* gSpriteClips[ 2 ].x = 155;//128
        gSpriteClips[ 2 ].y =   1;//0
        gSpriteClips[ 2 ].w =  52;//54
        gSpriteClips[ 2 ].h = 92;*/
        /*
        gSpriteClips[ 3 ].x = 196;//196
        gSpriteClips[ 3 ].y =   0;
        gSpriteClips[ 3 ].w =  54;
        gSpriteClips[ 3 ].h = 105;*/
    }



	//Load background texture
	if( !gBGTexture.loadFromFile( "bg.png" ) )
	{
		printf( "Failed to load background texture!\n" );
		success = false;
	}

	return success;
}

void close()
{
	//Free loaded images
	gDotTexture.free();
	gBGTexture.free();
        gPlatformTexture.free();
      gBoxTexture.free();
        //Destroy window
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

int main( int argc, char* args[] )
{       box.mPosX = 300;
	    box.mPosY = 250;
	//Start up SDL and create window
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if( !loadMedia() )
		{
			printf( "Failed to load media!\n" );
		}
		else
		{
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			//The dot that will be moving around on the screen
		//	Dot dot;
                        int frame = 0;
			//The camera area
			SDL_Rect camera = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

			//While application is running
			while( !quit )
			{
				//Handle events on queue
				while( SDL_PollEvent( &e ) != 0 )
				{
					//User requests quit
					if( e.type == SDL_QUIT )
					{
						quit = true;
					}

					//Handle input for the dot
					dot.handleEvent( e );
				}
                if(is_Moving_Up == true)
                    {
                    gSpriteClips[ 0 ].x =   36;//36
                    gSpriteClips[ 0 ].y =   34;//3
                    gSpriteClips[ 0 ].w =  27;//52
                    gSpriteClips[ 0 ].h = 31;//92

                    gSpriteClips[ 1 ].x =  67;//73
                    gSpriteClips[ 1 ].y =   34;//3
                    gSpriteClips[ 1 ].w = 27;//57
                    gSpriteClips[ 1 ].h = 31;//92

                }
                if(is_Moving_Down)
                {
                    gSpriteClips[ 0 ].x =   36;//36
        gSpriteClips[ 0 ].y =   2;//2
        gSpriteClips[ 0 ].w =  27;//52
        gSpriteClips[ 0 ].h = 31;//92

        gSpriteClips[ 1 ].x =  66;//66
        gSpriteClips[ 1 ].y =   2;//2
        gSpriteClips[ 1 ].w = 27;//57
        gSpriteClips[ 1 ].h = 31;//92
           if(check_collision(dot,box)){
                    //gBoxTexture.free();
                    box.mPosY += 10;
                }


          }
                if(is_Moving_Right == true)
                    {
                    gSpriteClips[ 0 ].x =   36;//36
                    gSpriteClips[ 0 ].y =   66;//3
                    gSpriteClips[ 0 ].w =  27;//52
                    gSpriteClips[ 0 ].h = 31;//92

                    gSpriteClips[ 1 ].x =  66;//73
                    gSpriteClips[ 1 ].y =   66;//3
                    gSpriteClips[ 1 ].w = 27;//57
                    gSpriteClips[ 1 ].h = 31;//92

                }//fix left right movement sprite
                if(is_Moving_Left == true)
                    {
                    gSpriteClips[ 0 ].x =   35;//36
                    gSpriteClips[ 0 ].y =   97;//3
                    gSpriteClips[ 0 ].w =  27;//52
                    gSpriteClips[ 0 ].h = 31;//92

                    gSpriteClips[ 1 ].x =  67;//73
                    gSpriteClips[ 1 ].y =   97;//3
                    gSpriteClips[ 1 ].w = 27;//57
                    gSpriteClips[ 1 ].h = 31;//92

                }









				//Move the dot
				dot.move();

               // box.move();
				//Center the camera over the dot
				camera.x = ( dot.getPosX() + Dot::DOT_WIDTH / 2 ) - SCREEN_WIDTH / 2;
				camera.y = ( dot.getPosY() + Dot::DOT_HEIGHT / 2 ) - SCREEN_HEIGHT / 2;

				//Keep the camera in bounds
				if( camera.x < 0 )
				{
					camera.x = 0;
				}
				if( camera.y < 0 )
				{
					camera.y = 0;
				}
				if( camera.x > LEVEL_WIDTH - camera.w )
				{
					camera.x = LEVEL_WIDTH - camera.w;
				}
				if( camera.y > LEVEL_HEIGHT - camera.h )
				{
					camera.y = LEVEL_HEIGHT - camera.h;
				}

				//Clear screen
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer );

				//Render background
				gBGTexture.render( 0, 0, &camera );//&camera
				//draw box randomly within the level
               // gBoxTexture.render(0,0,NULL);
				//Render objects
                box.render(camera.x,camera.y);

				//dot.render( camera.x, camera.y );
                                //platform.render(camera.x,camera.y);


                                SDL_Rect* currentClip = &gSpriteClips[ frame / 4 ];//4
                gDotTexture.render( ( SCREEN_WIDTH - currentClip->w ) / 2, ( SCREEN_HEIGHT - currentClip->h ) / 2, currentClip );
                // gBoxTexture.render(( SCREEN_WIDTH - currentClip->w ) / 2, ( SCREEN_HEIGHT - currentClip->h ) / 2, currentClip);
              SDL_Delay(100);

                                //Update screen
				SDL_RenderPresent( gRenderer );
				++frame;
                if(is_Moving == true){
                //Cycle animation
                if( frame / 2 >= WALKING_ANIMATION_FRAMES )//3
                {   //SDL_Delay(100);

                    frame = 0;//0
                }

		  }//moving
		  else{
		    frame = 5;//5
		  }

			}
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}
