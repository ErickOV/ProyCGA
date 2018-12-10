#include <iostream>

#include "Headers/InputManager.h"
#include "Headers/TimeManager.h"

InputCodes InputManager::toApplicationKey(int key) {
	switch (key) {
	case 256:
		return InputCodes::kEscape;
	case 87:
		return InputCodes::W;
	case 83:
		return InputCodes::S;
	case 65:
		return InputCodes::A;
	case 68:
		return InputCodes::D;
	case 265:
		return InputCodes::Up;
	case 264:
		return InputCodes::Down;
	case 263:
		return InputCodes::Left;
	case 262:
		return InputCodes::Right;
	case 85:
		return InputCodes::u;
	case 90:
		return InputCodes::Z;
	case 88:
		return InputCodes::X;
	case 67:
		return InputCodes::C;
	case 86:
		return InputCodes::V;
	case 66:
		return InputCodes::B;
	case 78:
		return InputCodes::N;
	case 77:
		return InputCodes::M;
	case 70:
		return InputCodes::F;
	case 71:
		return InputCodes::G;
	case 72:
		return InputCodes::H;
	case 74:
		return InputCodes::J;
	case 75:
		return InputCodes::K;
	case 76:
		return InputCodes::L;
	case 73:
		return InputCodes::I;
	case 79:
		return InputCodes::O;
	}
}

State InputManager::toApplicationState(int state) {
	switch (state) {
	case 0:
		return State::RELESED;
	case 1:
		return State::PRESSED;
	}
}

MouseButtonIndex InputManager::toMouseButtonIndex(int button) {
	switch (button) {
	case 0:
		return MouseButtonIndex::LEFT;
	case 1:
		return MouseButtonIndex::RIGHT;
	case 2:
		return MouseButtonIndex::MIDDLE;
	}
}

void InputManager::keyPressed(InputCodes code, float deltaTime, State state) {
	keyState[code] = state;
}

void InputManager::mouseMoved(float mouseX, float mouseY) {
	if (mouseButtomState[MouseButtonIndex::LEFT]) {
		float xoffset = mouseX - lastMousePos.x;
		float yoffset = lastMousePos.y - mouseY;

		cameraFPS->ProcessMouseMovement(xoffset, yoffset, true);
	}

	lastMousePos = glm::vec2(mouseX, mouseY);
}

void InputManager::mouseClicked(MouseButtonIndex code, float mouseX,
	float mouseY, State state) {
	mouseButtomState[code] = state;
	switch (code) {
	case RIGHT:
		std::cout << "lastMousePos.x:" << lastMousePos.x << std::endl;
		lastMousePos.x = mouseX;
		lastMousePos.y = mouseY;
		if (mouseButtomState[MouseButtonIndex::RIGHT])
			generateRay = true;
		break;
	case LEFT:
		std::cout << "lastMousePos.y:" << lastMousePos.y << std::endl;
		lastMousePos.x = mouseX;
		lastMousePos.y = mouseY;
		break;
	case MIDDLE:
		std::cout << "lastMousePos.x:" << lastMousePos.x << std::endl;
		std::cout << "lastMousePos.y:" << lastMousePos.y << std::endl;
		break;
	}
}

void InputManager::mouseScroll(float yoffset) {
}

void InputManager::do_movement(float deltaTime) {
	if (keyState[InputCodes::W])
		cameraFPS->ProcessKeyboard(C_FORWARD, deltaTime);
	if (keyState[InputCodes::S])
		cameraFPS->ProcessKeyboard(C_BACKWARD, deltaTime);
	if (keyState[InputCodes::A])
		cameraFPS->ProcessKeyboard(C_LEFT, deltaTime);
	if (keyState[InputCodes::D])
		cameraFPS->ProcessKeyboard(C_RIGHT, deltaTime);
}
