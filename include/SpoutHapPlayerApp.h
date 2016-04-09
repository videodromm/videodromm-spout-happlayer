#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Surface.h"
#include "cinder/gl/Texture.h"
#include "cinder/Text.h"
#include "cinder/Utilities.h"
#include "cinder/ImageIo.h"
#include "cinder/Timer.h"

#include "Resources.h"
#include "MovieHap.h"

// spout
#include "spout.h"

using namespace ci;
using namespace ci::app;
using namespace std;
//using namespace Reymenta;

template <typename T> string tostr(const T& t, int p) { ostringstream os; os << std::setprecision(p) << std::fixed << t; return os.str(); }

class SpoutHapPlayerApp : public App {
public:
	void setup();
	void update();
	void draw();
	void mouseDown(MouseEvent event);
	void keyDown(KeyEvent event) override;
	void fileDrop(FileDropEvent event) override;
	void shutdown();
	void loadMovieFile(const fs::path &path);

	gl::TextureRef			mInfoTexture;
	qtime::MovieGlHapRef	mMovie;

private:
	// -------- SPOUT -------------
	SpoutSender					spoutsender;            // Create a Spout sender object
	bool						bInitialized;           // true if a sender initializes OK
	bool						bMemoryMode;            // tells us if texture share compatible
	unsigned int				g_Width, g_Height;      // size of the texture being sent out
	char						SenderName[256];        // sender name 
	gl::TextureRef				spoutTexture;           // Local Cinder texture used for sharing
	bool						bDoneOnce;				// only try to initialize once
	int							nSenders;
	// ----------------------------
	bool						mLoopVideo;
	// fbo
	void						renderSceneToFbo();
	gl::FboRef					mRenderFbo;
};
