/*
 * StateMachine.cpp
 *
 *  Created on: 20 ���� 2014
 *      Author: Miro
 */

#include "StateMachine.h"

void StateMachineMode::init(StateMachine& stateMachine) {
	stateMachine.addMode(*this);
	addStates(stateMachine);
}



StateMachine::StateMachine() : _currMode(0), _totalModes(0) {

	for (uint8_t i = 0; i< SM_TOTAL_MODES; i++) {
		_totalStates[i] = 0;
		_currState[i] = 0;
	}

}


//StateMachine::~StateMachine() {
//	// TODO Auto-generated destructor stub
//}

void StateMachine::begin() {

}


uint8_t StateMachine::addMode(StateMachineMode& mode) {

	// Store Mode object
	_modesMap[_totalModes] = &mode;

	return _totalModes++;
}

uint8_t StateMachine::addState(StateMachineMode::STATE_FUNCTION_STATIC_T state) {
	_statesMap[_totalModes][_totalStates[_totalModes]] = state;

	return _totalStates[_totalModes]++;
}


bool StateMachine::setMode(StateMachineMode& mode, uint8_t state) {

	for (uint8_t i = 0; i < _totalModes; i++) {
		if (&mode == _modesMap[i]) {
			return setMode(i, state);
		}
	}

	return false;
}

bool StateMachine::setMode(uint8_t mode, uint8_t state) {

	// Check valid input
	if (mode >= _totalModes || (state > _totalStates[mode] && state != UNDEFINED_MODESTATE)) {
		return false;
	}

	// If same mode - do nothing
	if (mode == _currMode) {
		return true;
	}

	// If previously set - call destructor
	if (_modesMap[_currMode]) {
		_modesMap[_currMode]->end();
	}

	_currMode = mode;


	// Init next mode
	_modesMap[_currMode]->begin();

	return true;
}


bool StateMachine::setState(uint8_t state) {


	// Valid input check
	if (state > _totalStates[_currMode] && state != UNDEFINED_MODESTATE) {
		return false;
	}

	_currState[_currMode] = state;

	return true;
}


void StateMachine::onEvent(uint16_t eventType, uint32_t param1, uint32_t param2) {

	StateMachineMode::STATE_FUNCTION_STATIC_T currentHandler = _statesMap[_currMode][_currState[_currMode]];

	// Call state of the current mode
	SM_COMMAND stateResult = currentHandler(_modesMap[_currMode], eventType, param1, param2);

	// Check if state should be changed
	switch (stateResult) {
	case SM_NEXT_STATE:
		_currState[_currMode] = (_currState[_currMode] + 1) % _totalStates[_currMode];
		break;

	case SM_PREV_STATE:
		_currState[_currMode] = (_currState[_currMode] - 1) % _totalStates[_currMode];
		break;

	default:
		break;
	}

}

