
#include "SpoutHapPlayerApp.h"

void SpoutHapPlayerApp::setup()
{
	g_Width = 1024;
	g_Height = 768;

	setWindowSize(g_Width, g_Height);
	setWindowPos(ivec2(50, 50));

	setFullScreen(false);
	setFrameRate(25);
	mLoopVideo = false;
	// render fbo
	gl::Fbo::Format fboFormat;
	//format.setSamples( 4 ); // uncomment this to enable 4x antialiasing
	mRenderFbo = gl::Fbo::create(g_Width, g_Height, fboFormat.colorTexture());
	// -------- SPOUT -------------
	// Set up the texture we will use to send out
	// We grab the screen so it has to be the same size
	bInitialized = false;
	fs::path moviePath = getAssetPath("") / "lovewaitingroomHap1024x576.mov";
	loadMovieFile(moviePath);
}
void SpoutHapPlayerApp::keyDown(KeyEvent event)
{
	fs::path moviePath;
	float movieTime;
	switch (event.getCode())
	{
	case ci::app::KeyEvent::KEY_o:
		moviePath = getOpenFilePath();
		if (!moviePath.empty())
			loadMovieFile(moviePath);
		break;
	case ci::app::KeyEvent::KEY_r:
		mMovie.reset();
		break;
	case KeyEvent::KEY_LEFT:
		if (mMovie) {
			mMovie->stepBackward();
		}
		break;
	case KeyEvent::KEY_RIGHT:
		if (mMovie) {
			movieTime = mMovie->getCurrentTime();
			mMovie->seekToTime(movieTime + 0.1);
		}
		break;
	case ci::app::KeyEvent::KEY_p:
		if (mMovie) mMovie->play();
		break;
	case ci::app::KeyEvent::KEY_s:
		if (mMovie) mMovie->stop();
		break;
	case ci::app::KeyEvent::KEY_SPACE:
		if (mMovie->isPlaying()) mMovie->stop(); else mMovie->play();
		break;
	case ci::app::KeyEvent::KEY_ESCAPE:
		shutdown();
		break;
	case ci::app::KeyEvent::KEY_l:
		mLoopVideo = !mLoopVideo;
		if (mMovie) mMovie->setLoop(mLoopVideo);
		break;

	default:
		break;
	}
}
void SpoutHapPlayerApp::loadMovieFile(const fs::path &moviePath)
{
	try {
		mMovie.reset();
		// load up the movie, set it to loop, and begin playing
		mMovie = qtime::MovieGlHap::create(moviePath);
		mLoopVideo = (mMovie->getDuration() < 30.0f);
		mMovie->setLoop(mLoopVideo);
		mMovie->play();
		//g_Width = mMovie->getWidth();
		//g_Height = mMovie->getHeight();
		if (!bInitialized) {
			strcpy_s(SenderName, "Videodromm Hap Player"); // we have to set a sender name first
			// Optionally test for texture share compatibility
			// bMemoryMode informs us whether Spout initialized for texture share or memory share
			//bMemoryMode = spoutsender.GetMemoryShareMode();
		}
		spoutTexture = gl::Texture::create(g_Width, g_Height);
		//setWindowSize(g_Width, g_Height);
		// Initialize a sender
		bInitialized = spoutsender.CreateSender(SenderName, g_Width, g_Height);

	}
	catch (ci::Exception &e)
	{
		console() << string(e.what()) << std::endl;
		console() << "Unable to load the movie." << std::endl;
		mInfoTexture.reset();
	}

}

void SpoutHapPlayerApp::fileDrop(FileDropEvent event)
{
	loadMovieFile(event.getFile(0));
}
void SpoutHapPlayerApp::update()
{
	getWindow()->setTitle("(" + toString(floor(getAverageFps())) + " fps) Videodromm Hap" + toString(mLoopVideo));
}
// Render the scene into the FBO
void SpoutHapPlayerApp::renderSceneToFbo()
{
	// this will restore the old framebuffer binding when we leave this function
	// on non-OpenGL ES platforms, you can just call mFbo->unbindFramebuffer() at the end of the function
	// but this will restore the "screen" FBO on OpenGL ES, and does the right thing on both platforms
	gl::ScopedFramebuffer fbScp(mRenderFbo);
	gl::clear(Color::black(), true);
	// setup the viewport to match the dimensions of the FBO
	gl::ScopedViewport scpVp(ivec2(0), mRenderFbo->getSize());
	gl::color(Color::white());
	//gl::viewport(0, -200,1020,768);// getWindowSize());

	if (mMovie) {
		//if (mMovie->isPlaying()) mMovie->draw();
		if (mMovie->isPlaying()) gl::draw(mMovie->getTexture());
	}
}
void SpoutHapPlayerApp::draw()
{
	gl::clear(Color::black());

	gl::enableAlphaBlending();
	//gl::viewport(toPixels(getWindowSize()));

	/*if (mMovie) {
		//if (mMovie->isPlaying()) mMovie->draw();
		if (mMovie->isPlaying()) gl::draw(mMovie->getTexture());
	}*/
	 gl::draw(mRenderFbo->getColorTexture());
	if (bInitialized)
	{
		// Grab the screen (current read buffer) into the local spout texture
		spoutTexture->bind();
		glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, g_Width, g_Height);
		spoutTexture->unbind();

		// Send the texture for all receivers to use
		// NOTE : if SendTexture is called with a framebuffer object bound, that binding will be lost
		// and has to be restored afterwards because Spout uses an fbo for intermediate rendering
		spoutsender.SendTexture(spoutTexture->getId(), spoutTexture->getTarget(), g_Width, g_Height);
	}
}
void SpoutHapPlayerApp::mouseDown(MouseEvent event)
{

}
// -------- SPOUT -------------
void SpoutHapPlayerApp::shutdown()
{
	spoutsender.ReleaseSender();
	quit();
}

CINDER_APP(SpoutHapPlayerApp, RendererGl)
