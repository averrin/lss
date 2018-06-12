#include "lss/ui/statusLine.hpp"

StatusLine::StatusLine(std::shared_ptr<State> s): state(s) {};

void StatusLine::setContent(Fragments content) {
    state->statusFragments = content;
};

