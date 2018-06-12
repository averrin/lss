#include <regex>
#include <string>
#include <iostream>
#include <chrono>
#include <iterator>
#include <ostream>
#include <sstream>
#include <stdio.h>
#include <utility>

#include "cinder/Rand.h"
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "CinderPango.h"
#include "lss/state.hpp"
#include "lss/modes.hpp"

#include "lss/ui/statusLine.hpp"
#include "rang.hpp"

using namespace ci;
using namespace ci::app;
using namespace std;

int HOffset = 24;
int VOffset = 24;
// std::string DEFAULT_FONT = "Iosevka 14";
std::string DEFAULT_FONT = "FiraCode 12";

class LSSApp : public App {
public:
	void setup() override;
	void mouseDown(MouseEvent event) override;
	void update() override;
	void keyDown(KeyEvent event) override;
	void draw() override;

	kp::pango::CinderPangoRef mPango;
	kp::pango::CinderPangoRef statusFrame;
    ModeManager modeManager = ModeManager();
    std::shared_ptr<HintsMode> hints;
    std::shared_ptr<StatusLine> statusLine;
    std::shared_ptr<State> state;
};

auto F = [](std::string c) { return std::make_shared<Fragment>(c); };

void LSSApp::setup() {
	kp::pango::CinderPango::setTextRenderer(kp::pango::TextRenderer::FREETYPE);
	mPango = kp::pango::CinderPango::create();
	mPango->setMinSize(100, 100);
	mPango->setMaxSize(getWindowWidth(), getWindowHeight());

	statusFrame = kp::pango::CinderPango::create();
	statusFrame->setMinSize(getWindowWidth(), StatusLine::HEIGHT);
	statusFrame->setMaxSize(getWindowWidth(), StatusLine::HEIGHT);

    state = std::make_shared<State>();

    statusLine = std::make_shared<StatusLine>(state);
    statusLine->setContent(State::normal_mode);


  std::string line;
  std::ifstream dungeon_file("./dungeon.ascii");
  if (dungeon_file.is_open()) {
    while (getline(dungeon_file, line)) {
        for (auto ch : line) {
            switch (ch) {
                    case '.':
                        state->appendContent(std::make_shared<Floor>());
                        break;
                    case '#':
                        state->appendContent(std::make_shared<Wall>());
                        break;
                    case '@':
                        state->appendContent(std::make_shared<Hero>());
                        break;
                    default:
                        state->appendContent(F(std::string{ch}));
                        break;
                }
                
        }
    state->appendContent(State::END_LINE);
    }
    dungeon_file.close();
  }
   // state->setContent({std::make_shared<Fragment>(dungeon_str)});
   state->currentPalette = palettes::DARK;
}

void LSSApp::mouseDown(MouseEvent event) {
}

void LSSApp::keyDown(KeyEvent event) {


    // if (modeManager.modeFlags->isHints) {
    //     auto handled = hints->processKey(event);
    //     if (handled) {
    //         if (!hints->activated) {
    //             modeManager.toNormal();
    //             statusLine->setContent(State::normal_mode);
    //         }
    //         return;
    //     }
    // }

    modeManager.processKey(event);

    if (modeManager.modeFlags->isNormal) {
        statusLine->setContent(State::normal_mode);
    } else if (modeManager.modeFlags->isHints) {
        hints->activated = true;
        statusLine->setContent(State::hints_mode);
    } else if (modeManager.modeFlags->isLeader) {
        statusLine->setContent(State::leader_mode);
    }

	switch (event.getCode()) {
		case KeyEvent::KEY_q:
            exit(0);
			break;
		case KeyEvent::KEY_r:
            // State::greeting[0]->template_str = "!!!";
			break;
		case KeyEvent::KEY_t:
            state->currentPalette = state->currentPalette.name == palettes::DARK.name ?
                palettes::LIGHT : palettes::DARK;
            state->invalidate();
			break;
		default:
			break;
	}
}

void LSSApp::update() {
      // auto t0 = std::chrono::system_clock::now();
	gl::clear(state->currentPalette.bgColor);
	gl::enableAlphaBlendingPremult();

	if (mPango != nullptr) {

        state->render(mPango);

	}

    //TODO: hide into statusBar. Implement padding
	if (statusFrame != nullptr) {

		statusFrame->setText(state->renderStatus());

        statusFrame->setDefaultTextFont(DEFAULT_FONT);
		statusFrame->render();
    }
//       auto t1 = std::chrono::system_clock::now();
//         using milliseconds = std::chrono::duration<double, std::milli>;
//         milliseconds ms = t1 - t0;
//         std::cout << "time taken: " << rang::fg::green << ms.count() << rang::style::reset << '\n';
}

void LSSApp::draw() {
	if (mPango != nullptr) {

        mPango->setDefaultTextColor(state->currentPalette.fgColor);
        mPango->setBackgroundColor(ColorA(0,0,0,0));
        // gl::color(ColorA(0,0,0,0));
		gl::draw(mPango->getTexture(), vec2(HOffset, VOffset));
	}
	if (statusFrame != nullptr) {
        gl::color(state->currentPalette.bgColorAlt);
        gl::drawSolidRect(Rectf(0, getWindowHeight()-StatusLine::HEIGHT, getWindowWidth(), getWindowHeight()));
        gl::color(ColorA(1,1,1,1));
        statusFrame->setDefaultTextColor(Color(state->currentPalette.fgColor));
        statusFrame->setBackgroundColor(ColorA(0,0,0,0));
		gl::draw(statusFrame->getTexture(), vec2(6, getWindowHeight()-StatusLine::HEIGHT + 6));
	}
}

CINDER_APP(LSSApp, RendererGl)
