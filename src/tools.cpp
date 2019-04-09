#include "tools.hpp"

// Derived from https://gist.github.com/guimeira/541e9056364b9491452b7027f12536cc


cv::Rect SelectionState::toRect() {
	return cv::Rect (
		std::min(this->startPt.x, this->mousePos.x),
		std::min(this->startPt.y, this->mousePos.y),
		std::abs(this->startPt.x-this->mousePos.x),
		std::abs(this->startPt.y-this->mousePos.y));
}

void onMouse(int event, int x, int y, int, void *data) {
	SelectionState *state = (SelectionState*) data;

	switch(event) {
	case cv::EVENT_LBUTTONDOWN:
		state->startPt.x = x;
		state->startPt.y = y;
		state->mousePos.x = x;
		state->mousePos.y = y;
		state->started = true;
		break;

	case cv::EVENT_LBUTTONUP:
		state->endPt.x = x;
		state->endPt.y = y;
		state->done = true;
		break;

	case cv::EVENT_MOUSEMOVE:
		state->mousePos.x = x;
		state->mousePos.y = y;
		break;
	}
}


cv::Rect selectRect(cv::Mat image, cv::Scalar color, int thickness) {
	const std::string window = "rect";
	SelectionState state;
	cv::namedWindow(window, cv::WINDOW_NORMAL);
	cv::setMouseCallback(window, onMouse, &state);

	while(!state.done) {
		cv::waitKey(1);

		if (state.started) {

			cv::Mat copy = image.clone();
			cv::Rect selection = state.toRect();
			cv::rectangle(copy, selection, color, thickness);
			cv::imshow(window, copy);

		} else {

			cv::imshow(window, image);
		
		}
	}

	return state.toRect();
}
