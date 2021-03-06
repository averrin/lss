// Surface.h
// PangoBasic
//
// Created by Eric Mika on 1/6/16.
//

#pragma once

#include <fontconfig/fontconfig.h>
#include <pango/pangocairo.h>
#include <SDL.h>
#include <glm/vec2.hpp>
#include <glm/common.hpp>

#include <vector>
#include <memory>

#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

namespace pango {
typedef glm::tvec2<int> ivec2;

// TODO wrap these up?
const bool grayscale = false;
const bool native = false;

enum class TextAlignment : int {
	LEFT,
	CENTER,
	RIGHT,
	JUSTIFY,
};

enum class TextRenderer {
	FREETYPE,
	PLATFORM_NATIVE,
};

enum class TextWeight : int {
	THIN = 100,
	ULTRALIGHT = 200,
	LIGHT = 300,
	SEMILIGHT = 350,
	BOOK = 380,
	NORMAL = 400,
	MEDIUM = 550,
	SEMIBOLD = 600,
	BOLD = 800,
	ULTRABOLD = 800,
	HEAVY = 900,
	ULTRAHEAVY = 1000
};

enum class TextAntialias : int {
	DEFAULT,
	NONE,
	GRAY,
	SUBPIXEL,
};

typedef std::shared_ptr<class Surface> SurfaceRef;

class Surface : public std::enable_shared_from_this<Surface> {
public:
	static SurfaceRef create(SDL_Renderer*);
	virtual ~Surface();
	Surface(SDL_Renderer*);
	SDL_Renderer* renderer;

	// Globals
	static std::vector<std::string> getFontList(bool verbose);
	static void logFontList(bool verbose = false);
	static void loadFont(const fs::path &path);
	static TextRenderer getTextRenderer();
	static void setTextRenderer(TextRenderer renderer);

	// Rendering

	const std::string getText();

	// setText can take inline markup to override the default text settings
	// See here for full list of supported tags:
	// https://developer.gnome.org/pango/stable/PangoMarkupFormat.html
	void setText(std::string text);

	// Text is rendered into this texture
	SDL_Surface* getTexture();
	void free();

	// Text smaller than the min size will be clipped
	ivec2 getMinSize();
	void setMinSize(int minWidth, int minHeight);
	void setMinSize(ivec2 minSize);

	// Text can grow up to this size before a line breaks or clipping begins
	ivec2 getMaxSize();
	void setMaxSize(int maxWidth, int maxHeight);
	void setMaxSize(ivec2 maxSize);

	// Setting default font styles is more efficient than passing markup via the text string

		void setDefaultTextStyle(std::string font = "Sans", float size = 12.0, SDL_Color color = {0,0,0}, TextWeight weight = TextWeight::NORMAL,
													 TextAlignment alignment = TextAlignment::LEFT); // convenience

	SDL_Color getDefaultTextColor();
	void setDefaultTextColor(SDL_Color color);

	SDL_Color getBackgroundColor();
	void setBackgroundColor(SDL_Color color);

	float getDefaultTextSize();
	void setDefaultTextSize(float size);

	std::string getDefaultTextFont();
	void setDefaultTextFont(std::string font);

	TextWeight getDefaultTextWeight();
	void setDefaultTextWeight(TextWeight weight);

	TextAntialias getTextAntialias();
	void setTextAntialias(TextAntialias mode);

	TextAlignment getTextAlignment();
	void setTextAlignment(TextAlignment alignment);

	bool getDefaultTextSmallCapsEnabled();
	void setDefaultTextSmallCapsEnabled(bool value);

	bool getDefaultTextItalicsEnabled();
	void setDefaultTextItalicsEnabled(bool value);

	float getSpacing();
	void setSpacing(float spacing);

	// Renders text into the texture.
	// Returns true if the texture was actually pdated, false if nothing had to change
	// It's reasonable (and more efficient) to just run this in an update loop rather than calling it
	// explicitly after every change to the text state. It will coalesce all invalidations since the
	// last frame and only rebuild what needs to be rebuilt to render the diff.
	// Set force to true to render even if the system thinks state wasn't invalidated.
	bool render(bool force = false);
	void disableWrap();

protected:
	Surface();

private:
	SDL_Texture* mTexture;
	SDL_Surface* mSurface;
	std::string mText;
	std::string mProcessedText; // stores text after newline filtering
	bool mProbablyHasMarkup;
	ivec2 mMinSize;
	ivec2 mMaxSize;

	// TODO wrap these up...
	std::string mDefaultTextFont;
	bool mDefaultTextItalicsEnabled;
	bool mDefaultTextSmallCapsEnabled;
	SDL_Color mDefaultTextColor;
	SDL_Color mBackgroundColor;
	float mDefaultTextSize;
	TextAlignment mTextAlignment;
	TextWeight mDefaultTextWeight;
	TextAntialias mTextAntialias;
	float mSpacing;

	// Internal flags for state invalidation
	// Used by render method
	bool mNeedsFontUpdate;
	bool mNeedsMeasuring;
	bool mNeedsTextRender;
	bool mNeedsFontOptionUpdate;
	bool mNeedsMarkupDetection;

	// simply stored to check for change across renders
	int mPixelWidth;
	int mPixelHeight;

	// Pango references
	PangoFontMap *fontMap;
	PangoContext *pangoContext;
	PangoLayout *pangoLayout;
	PangoFontDescription *fontDescription;
	cairo_surface_t *cairoSurface;
	cairo_t *cairoContext;
	cairo_font_options_t *cairoFontOptions;

#ifdef CAIRO_HAS_WIN32_SURFACE
	cairo_surface_t *cairoImageSurface;
#endif
};
}
