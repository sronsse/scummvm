/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "scumm/editor/compiler/instruction.h"
#include "scumm/editor/compiler/compiler.h"
#include "scumm/editor/compiler/context.h"

namespace Scumm {

namespace Editor {

namespace Compiler {

Common::HashMap<Common::String, byte> Instruction::_opcodes;
Common::HashMap<Common::String, byte> Instruction::_subOpcodes;

void Instruction::setOpcodes() {
	_opcodes["abs"] = OP_ABS;
	_opcodes["actorFollowCamera"] = OP_ACTOR_FOLLOW_CAMERA;
	_opcodes["actorOps"] = OP_ACTOR_OPS;
	_opcodes["add"] = OP_ADD;
	_opcodes["animateActor"] = OP_ANIMATE_ACTOR;
	_opcodes["arrayOps"] = OP_ARRAY_OPS;
	_opcodes["band"] = OP_BAND;
	_opcodes["beginOverride"] = OP_BEGIN_OVERRIDE;
	_opcodes["bor"] = OP_BOR;
	_opcodes["breakHere"] = OP_BREAK_HERE;
	_opcodes["byteArrayDec"] = OP_BYTE_ARRAY_DEC;
	_opcodes["byteArrayInc"] = OP_BYTE_ARRAY_INC;
	_opcodes["byteArrayIndexedRead"] = OP_BYTE_ARRAY_INDEXED_READ;
	_opcodes["byteArrayIndexedWrite"] = OP_BYTE_ARRAY_INDEXED_WRITE;
	_opcodes["byteArrayRead"] = OP_BYTE_ARRAY_READ;
	_opcodes["byteArrayWrite"] = OP_BYTE_ARRAY_WRITE;
	_opcodes["byteVarDec"] = OP_BYTE_VAR_DEC;
	_opcodes["byteVarInc"] = OP_BYTE_VAR_INC;
	_opcodes["createBoxMatrix"] = OP_CREATE_BOX_MATRIX;
	_opcodes["cursorCommand"] = OP_CURSOR_COMMAND;
	_opcodes["cutscene"] = OP_CUTSCENE;
	_opcodes["delay"] = OP_DELAY;
	_opcodes["delayFrames"] = OP_DELAY_FRAMES;
	_opcodes["delayMinutes"] = OP_DELAY_MINUTES;
	_opcodes["delaySeconds"] = OP_DELAY_SECONDS;
	_opcodes["dim2DimArray"] = OP_DIM_2_DIM_ARRAY;
	_opcodes["dimArray"] = OP_DIM_ARRAY;
	_opcodes["distObjectObject"] = OP_DIST_OBJECT_OBJECT;
	_opcodes["distObjectPt"] = OP_DIST_OBJECT_PT;
	_opcodes["distPtPt"] = OP_DIST_PT_PT;
	_opcodes["div"] = OP_DIV;
	_opcodes["doSentence"] = OP_DO_SENTENCE;
	_opcodes["drawBlastObject"] = OP_DRAW_BLAST_OBJECT;
	_opcodes["drawBox"] = OP_DRAW_BOX;
	_opcodes["drawObject"] = OP_DRAW_OBJECT;
	_opcodes["drawObjectAt"] = OP_DRAW_OBJECT_AT;
	_opcodes["dummy"] = OP_DUMMY;
	_opcodes["dup"] = OP_DUP;
	_opcodes["endCutscene"] = OP_END_CUTSCENE;
	_opcodes["endOverride"] = OP_END_OVERRIDE;
	_opcodes["eq"] = OP_EQ;
	_opcodes["faceActor"] = OP_FACE_ACTOR;
	_opcodes["findAllObjects"] = OP_FIND_ALL_OBJECTS;
	_opcodes["findInventory"] = OP_FIND_INVENTORY;
	_opcodes["findObject"] = OP_FIND_OBJECT;
	_opcodes["freezeUnfreeze"] = OP_FREEZE_UNFREEZE;
	_opcodes["ge"] = OP_GE;
	_opcodes["getActorAnimCounter"] = OP_GET_ACTOR_ANIM_COUNTER;
	_opcodes["getActorCostume"] = OP_GET_ACTOR_COSTUME;
	_opcodes["getActorElevation"] = OP_GET_ACTOR_ELEVATION;
	_opcodes["getActorFromXY"] = OP_GET_ACTOR_FROM_XY;
	_opcodes["getActorLayer"] = OP_GET_ACTOR_LAYER;
	_opcodes["getActorMoving"] = OP_GET_ACTOR_MOVING;
	_opcodes["getActorRoom"] = OP_GET_ACTOR_ROOM;
	_opcodes["getActorScaleX"] = OP_GET_ACTOR_SCALE_X;
	_opcodes["getActorWalkBox"] = OP_GET_ACTOR_WALK_BOX;
	_opcodes["getActorWidth"] = OP_GET_ACTOR_WIDTH;
	_opcodes["getAnimateVariable"] = OP_GET_ANIMATE_VARIABLE;
	_opcodes["getDateTime"] = OP_GET_DATE_TIME;
	_opcodes["getInventoryCount"] = OP_GET_INVENTORY_COUNT;
	_opcodes["getObjectNewDir"] = OP_GET_OBJECT_NEW_DIR;
	_opcodes["getObjectOldDir"] = OP_GET_OBJECT_OLD_DIR;
	_opcodes["getObjectX"] = OP_GET_OBJECT_X;
	_opcodes["getObjectY"] = OP_GET_OBJECT_Y;
	_opcodes["getOwner"] = OP_GET_OWNER;
	_opcodes["getPixel"] = OP_GET_PIXEL;
	_opcodes["getRandomNumber"] = OP_GET_RANDOM_NUMBER;
	_opcodes["getRandomNumberRange"] = OP_GET_RANDOM_NUMBER_RANGE;
	_opcodes["getState"] = OP_GET_STATE;
	_opcodes["getVerbEntryPoint"] = OP_GET_VERB_ENTRY_POINT;
	_opcodes["getVerbFromXY"] = OP_GET_VERB_FROM_XY;
	_opcodes["gt"] = OP_GT;
	_opcodes["if"] = OP_IF;
	_opcodes["ifClassOfIs"] = OP_IF_CLASS_OF_IS;
	_opcodes["ifNot"] = OP_IF_NOT;
	_opcodes["isActorInBox"] = OP_IS_ACTOR_IN_BOX;
	_opcodes["isAnyOf"] = OP_IS_ANY_OF;
	_opcodes["isRoomScriptRunning"] = OP_IS_ROOM_SCRIPT_RUNNING;
	_opcodes["isScriptRunning"] = OP_IS_SCRIPT_RUNNING;
	_opcodes["isSoundRunning"] = OP_IS_SOUND_RUNNING;
	_opcodes["jump"] = OP_JUMP;
	_opcodes["jumpToScript"] = OP_JUMP_TO_SCRIPT;
	_opcodes["kernelGetFunctions"] = OP_KERNEL_GET_FUNCTIONS;
	_opcodes["kernelSetFunctions"] = OP_KERNEL_SET_FUNCTIONS;
	_opcodes["land"] = OP_LAND;
	_opcodes["le"] = OP_LE;
	_opcodes["loadRoom"] = OP_LOAD_ROOM;
	_opcodes["loadRoomWithEgo"] = OP_LOAD_ROOM_WITH_EGO;
	_opcodes["lor"] = OP_LOR;
	_opcodes["lt"] = OP_LT;
	_opcodes["mul"] = OP_MUL;
	_opcodes["neq"] = OP_NEQ;
	_opcodes["not"] = OP_NOT;
	_opcodes["panCameraTo"] = OP_PAN_CAMERA_TO;
	_opcodes["pickOneOf"] = OP_PICK_ONE_OF;
	_opcodes["pickOneOfDefault"] = OP_PICK_ONE_OF_DEFAULT;
	_opcodes["pickVarRandom"] = OP_PICK_VAR_RANDOM;
	_opcodes["pickupObject"] = OP_PICKUP_OBJECT;
	_opcodes["pop"] = OP_POP;
	_opcodes["printActor"] = OP_PRINT_ACTOR;
	_opcodes["printDebug"] = OP_PRINT_DEBUG;
	_opcodes["printEgo"] = OP_PRINT_EGO;
	_opcodes["printLine"] = OP_PRINT_LINE;
	_opcodes["printSystem"] = OP_PRINT_SYSTEM;
	_opcodes["printText"] = OP_PRINT_TEXT;
	_opcodes["pseudoRoom"] = OP_PSEUDO_ROOM;
	_opcodes["pushByte"] = OP_PUSH_BYTE;
	_opcodes["pushByteVar"] = OP_PUSH_BYTE_VAR;
	_opcodes["pushWord"] = OP_PUSH_WORD;
	_opcodes["pushWordVar"] = OP_PUSH_WORD_VAR;
	_opcodes["putActorAtObject"] = OP_PUT_ACTOR_AT_OBJECT;
	_opcodes["putActorAtXY"] = OP_PUT_ACTOR_AT_XY;
	_opcodes["resourceRoutines"] = OP_RESOURCE_ROUTINES;
	_opcodes["roomOps"] = OP_ROOM_OPS;
	_opcodes["saveRestoreVerbs"] = OP_SAVE_RESTORE_VERBS;
	_opcodes["setBlastObjectWindow"] = OP_SET_BLAST_OBJECT_WINDOW;
	_opcodes["setBoxFlags"] = OP_SET_BOX_FLAGS;
	_opcodes["setBoxSet"] = OP_SET_BOX_SET;
	_opcodes["setCameraAt"] = OP_SET_CAMERA_AT;
	_opcodes["setClass"] = OP_SET_CLASS;
	_opcodes["setObjectName"] = OP_SET_OBJECT_NAME;
	_opcodes["setOwner"] = OP_SET_OWNER;
	_opcodes["setState"] = OP_SET_STATE;
	_opcodes["shuffle"] = OP_SHUFFLE;
	_opcodes["soundKludge"] = OP_SOUND_KLUDGE;
	_opcodes["stampObject"] = OP_STAMP_OBJECT;
	_opcodes["startMusic"] = OP_START_MUSIC;
	_opcodes["startObject"] = OP_START_OBJECT;
	_opcodes["startObjectQuick"] = OP_START_OBJECT_QUICK;
	_opcodes["startScript"] = OP_START_SCRIPT;
	_opcodes["startScriptQuick"] = OP_START_SCRIPT_QUICK;
	_opcodes["startScriptQuick2"] = OP_START_SCRIPT_QUICK_2;
	_opcodes["startSound"] = OP_START_SOUND;
	_opcodes["stopMusic"] = OP_STOP_MUSIC;
	_opcodes["stopObjectCode"] = OP_STOP_OBJECT_CODE;
	_opcodes["stopObjectCode2"] = OP_STOP_OBJECT_CODE_2;
	_opcodes["stopObjectScript"] = OP_STOP_OBJECT_SCRIPT;
	_opcodes["stopScript"] = OP_STOP_SCRIPT;
	_opcodes["stopSentence"] = OP_STOP_SENTENCE;
	_opcodes["stopSound"] = OP_STOP_SOUND;
	_opcodes["stopTalking"] = OP_STOP_TALKING;
	_opcodes["sub"] = OP_SUB;
	_opcodes["systemOps"] = OP_SYSTEM_OPS;
	_opcodes["talkActor"] = OP_TALK_ACTOR;
	_opcodes["talkEgo"] = OP_TALK_EGO;
	_opcodes["verbOps"] = OP_VERB_OPS;
	_opcodes["wait"] = OP_WAIT;
	_opcodes["walkActorTo"] = OP_WALK_ACTOR_TO;
	_opcodes["walkActorToObj"] = OP_WALK_ACTOR_TO_OBJ;
	_opcodes["wordArrayDec"] = OP_WORD_ARRAY_DEC;
	_opcodes["wordArrayInc"] = OP_WORD_ARRAY_INC;
	_opcodes["wordArrayIndexedRead"] = OP_WORD_ARRAY_INDEXED_READ;
	_opcodes["wordArrayIndexedWrite"] = OP_WORD_ARRAY_INDEXED_WRITE;
	_opcodes["wordArrayRead"] = OP_WORD_ARRAY_READ;
	_opcodes["wordArrayWrite"] = OP_WORD_ARRAY_WRITE;
	_opcodes["wordVarDec"] = OP_WORD_VAR_DEC;
	_opcodes["wordVarInc"] = OP_WORD_VAR_INC;
	_opcodes["writeByteVar"] = OP_WRITE_BYTE_VAR;
	_opcodes["writeWordVar"] = OP_WRITE_WORD_VAR;
}

void Instruction::setSubOpcodes() {
	_subOpcodes["actorDepth"] = SO_ACTOR_DEPTH;
	_subOpcodes["actorIgnoreTurnsOff"] = SO_ACTOR_IGNORE_TURNS_OFF;
	_subOpcodes["actorIgnoreTurnsOn"] = SO_ACTOR_IGNORE_TURNS_ON;
	_subOpcodes["actorName"] = SO_ACTOR_NAME;
	_subOpcodes["actorNew"] = SO_NEW;
	_subOpcodes["actorSetCurrent"] = SO_ACTOR_INIT;
	_subOpcodes["actorSetDirection"] = SO_ACTOR_FACE;
	_subOpcodes["actorStop"] = SO_ACTOR_STOP;
	_subOpcodes["actorTalkScript"] = SO_ACTOR_TALK_SCRIPT;
	_subOpcodes["actorTurnToDirection"] = SO_ACTOR_TURN;
	_subOpcodes["actorVariable"] = SO_ACTOR_VARIABLE;
	_subOpcodes["actorWalkPause"] = SO_ACTOR_WALK_PAUSE;
	_subOpcodes["actorWalkResume"] = SO_ACTOR_WALK_RESUME;
	_subOpcodes["actorWalkScript"] = SO_ACTOR_WALK_SCRIPT;
	_subOpcodes["actorWidth"] = SO_ACTOR_WIDTH;
	_subOpcodes["alwaysZClip"] = SO_ALWAYS_ZCLIP;
	_subOpcodes["alwaysZClipFtDemo"] = SO_ALWAYS_ZCLIP_FT_DEMO;
	_subOpcodes["animation"] = SO_ANIMATION;
	_subOpcodes["animationDefault"] = SO_ANIMATION_DEFAULT;
	_subOpcodes["animationSpeed"] = SO_ANIMATION_SPEED;
	_subOpcodes["assign2DimList"] = SO_ASSIGN_2DIM_LIST;
	_subOpcodes["assignIntList"] = SO_ASSIGN_INT_LIST;
	_subOpcodes["assignString"] = SO_ASSIGN_STRING;
	_subOpcodes["at"] = SO_AT;
	_subOpcodes["bitArray"] = SO_BIT_ARRAY;
	_subOpcodes["byteArray"] = SO_BYTE_ARRAY;
	_subOpcodes["center"] = SO_CENTER;
	_subOpcodes["charsetColor"] = SO_CHARSET_COLOR;
	_subOpcodes["charsetSet"] = SO_CHARSET_SET;
	_subOpcodes["clearHeap"] = SO_CLEAR_HEAP;
	_subOpcodes["clipped"] = SO_CLIPPED;
	_subOpcodes["color"] = SO_COLOR;
	_subOpcodes["costume"] = SO_COSTUME;
	_subOpcodes["cursorHotspot"] = SO_CURSOR_HOTSPOT;
	_subOpcodes["cursorImage"] = SO_CURSOR_IMAGE;
	_subOpcodes["cursorOff"] = SO_CURSOR_OFF;
	_subOpcodes["cursorOn"] = SO_CURSOR_ON;
	_subOpcodes["cursorSoftOff"] = SO_CURSOR_SOFT_OFF;
	_subOpcodes["cursorSoftOn"] = SO_CURSOR_SOFT_ON;
	_subOpcodes["cursorTransparent"] = SO_CURSOR_TRANSPARENT;
	_subOpcodes["cycleSpeed"] = SO_CYCLE_SPEED;
	_subOpcodes["default"] = SO_DEFAULT;
	_subOpcodes["deleteVerbs"] = SO_DELETE_VERBS;
	_subOpcodes["elevation"] = SO_ELEVATION;
	_subOpcodes["followBoxes"] = SO_FOLLOW_BOXES;
	_subOpcodes["ignoreBoxes"] = SO_IGNORE_BOXES;
	_subOpcodes["initAnimation"] = SO_INIT_ANIMATION;
	_subOpcodes["intArray"] = SO_INT_ARRAY;
	_subOpcodes["left"] = SO_LEFT;
	_subOpcodes["loadCharset"] = SO_LOAD_CHARSET;
	_subOpcodes["loadCostume"] = SO_LOAD_COSTUME;
	_subOpcodes["loadDefault"] = SO_BASEOP;
	_subOpcodes["loadObject"] = SO_LOAD_OBJECT;
	_subOpcodes["loadRoom2"] = SO_LOAD_ROOM;
	_subOpcodes["loadScript"] = SO_LOAD_SCRIPT;
	_subOpcodes["loadSound"] = SO_LOAD_SOUND;
	_subOpcodes["loadString"] = SO_LOAD_STRING;
	_subOpcodes["lockCostume"] = SO_LOCK_COSTUME;
	_subOpcodes["lockRoom"] = SO_LOCK_ROOM;
	_subOpcodes["lockScript"] = SO_LOCK_SCRIPT;
	_subOpcodes["lockSound"] = SO_LOCK_SOUND;
	_subOpcodes["mumble"] = SO_MUMBLE;
	_subOpcodes["neverZClip"] = SO_NEVER_ZCLIP;
	_subOpcodes["nibbleArray"] = SO_NIBBLE_ARRAY;
	_subOpcodes["nukeCharset"] = SO_NUKE_CHARSET;
	_subOpcodes["nukeCostume"] = SO_NUKE_COSTUME;
	_subOpcodes["nukeRoom"] = SO_NUKE_ROOM;
	_subOpcodes["nukeScript"] = SO_NUKE_SCRIPT;
	_subOpcodes["nukeSound"] = SO_NUKE_SOUND;
	_subOpcodes["overhead"] = SO_OVERHEAD;
	_subOpcodes["palette"] = SO_PALETTE;
	_subOpcodes["pause"] = SO_PAUSE;
	_subOpcodes["quit"] = SO_QUIT;
	_subOpcodes["restart"] = SO_RESTART;
	_subOpcodes["restoreVerbs"] = SO_RESTORE_VERBS;
	_subOpcodes["rgbRoomIntensity"] = SO_RGB_ROOM_INTENSITY;
	_subOpcodes["roomFade"] = SO_ROOM_FADE;
	_subOpcodes["roomIntensity"] = SO_ROOM_INTENSITY;
	_subOpcodes["roomNewPalette"] = SO_ROOM_NEW_PALETTE;
	_subOpcodes["roomPalette"] = SO_ROOM_PALETTE;
	_subOpcodes["roomSaveGame"] = SO_ROOM_SAVEGAME;
	_subOpcodes["roomScreen"] = SO_ROOM_SCREEN;
	_subOpcodes["roomScroll"] = SO_ROOM_SCROLL;
	_subOpcodes["roomShadow"] = SO_ROOM_SHADOW;
	_subOpcodes["roomShakeOff"] = SO_ROOM_SHAKE_OFF;
	_subOpcodes["roomShakeOn"] = SO_ROOM_SHAKE_ON;
	_subOpcodes["roomTransform"] = SO_ROOM_TRANSFORM;
	_subOpcodes["saveDefault"] = SO_END;
	_subOpcodes["saveString"] = SO_SAVE_STRING;
	_subOpcodes["saveVerbs"] = SO_SAVE_VERBS;
	_subOpcodes["scale"] = SO_SCALE;
	_subOpcodes["shadow"] = SO_SHADOW;
	_subOpcodes["sound"] = SO_SOUND;
	_subOpcodes["standAnimation"] = SO_STAND_ANIMATION;
	_subOpcodes["stepDist"] = SO_STEP_DIST;
	_subOpcodes["stringArray"] = SO_STRING_ARRAY;
	_subOpcodes["talkAnimation"] = SO_TALK_ANIMATION;
	_subOpcodes["talkColor"] = SO_TALK_COLOR;
	_subOpcodes["textOffset"] = SO_TEXT_OFFSET;
	_subOpcodes["textString"] = SO_TEXTSTRING;
	_subOpcodes["undimArray"] = SO_UNDIM_ARRAY;
	_subOpcodes["unlockCostume"] = SO_UNLOCK_COSTUME;
	_subOpcodes["unlockRoom"] = SO_UNLOCK_ROOM;
	_subOpcodes["unlockScript"] = SO_UNLOCK_SCRIPT;
	_subOpcodes["unlockSound"] = SO_UNLOCK_SOUND;
	_subOpcodes["userputOff"] = SO_USERPUT_OFF;
	_subOpcodes["userputOn"] = SO_USERPUT_ON;
	_subOpcodes["userputSoftOff"] = SO_USERPUT_SOFT_OFF;
	_subOpcodes["userputSoftOn"] = SO_USERPUT_SOFT_ON;
	_subOpcodes["verbAt"] = SO_VERB_AT;
	_subOpcodes["verbBackColor"] = SO_VERB_BAKCOLOR;
	_subOpcodes["verbCenter"] = SO_VERB_CENTER;
	_subOpcodes["verbColor"] = SO_VERB_COLOR;
	_subOpcodes["verbDelete"] = SO_VERB_DELETE;
	_subOpcodes["verbDim"] = SO_VERB_DIM;
	_subOpcodes["verbDimColor"] = SO_VERB_DIMCOLOR;
	_subOpcodes["verbDraw"] = SO_END;
	_subOpcodes["verbHiColor"] = SO_VERB_HICOLOR;
	_subOpcodes["verbImage"] = SO_VERB_IMAGE;
	_subOpcodes["verbImageInRoom"] = SO_VERB_IMAGE_IN_ROOM;
	_subOpcodes["verbKey"] = SO_VERB_KEY;
	_subOpcodes["verbName"] = SO_VERB_NAME;
	_subOpcodes["verbNameStr"] = SO_VERB_NAME_STR;
	_subOpcodes["verbNew"] = SO_VERB_NEW;
	_subOpcodes["verbOff"] = SO_VERB_OFF;
	_subOpcodes["verbOn"] = SO_VERB_ON;
	_subOpcodes["verbSetCurrent"] = SO_VERB_INIT;
	_subOpcodes["waitForActor"] = SO_WAIT_FOR_ACTOR;
	_subOpcodes["waitForAnimation"] = SO_WAIT_FOR_ANIMATION;
	_subOpcodes["waitForCamera"] = SO_WAIT_FOR_CAMERA;
	_subOpcodes["waitForMessage"] = SO_WAIT_FOR_MESSAGE;
	_subOpcodes["waitForSentence"] = SO_WAIT_FOR_SENTENCE;
	_subOpcodes["waitForTurn"] = SO_WAIT_FOR_TURN;
	_subOpcodes["walkAnimation"] = SO_WALK_ANIMATION;
}

const char *Instruction::getOpcodeName(byte opcode) {
	if (_opcodes.empty())
		setOpcodes();

	for (auto it = _opcodes.begin(); it != _opcodes.end(); ++it) {
		if (it->_value == opcode)
			return it->_key.c_str();
	}
	return nullptr;
}

const char *Instruction::getSubOpcodeName(byte subOpcode) {
	if (_subOpcodes.empty())
		setSubOpcodes();

	for (auto it = _subOpcodes.begin(); it != _subOpcodes.end(); ++it) {
		if (it->_value == subOpcode)
			return it->_key.c_str();
	}
	return nullptr;
}

bool Instruction::isOpcode(const Common::String &name) {
	if (_opcodes.empty())
		setOpcodes();
	return _opcodes.contains(name);
}

int16 Instruction::toInteger(const Common::String &s) {
	int32 result;
	if (sscanf(s.c_str(), "%d", &result) != 1) {
		Compiler::log(LOG_ERROR, "\"%s\" is not an integer", s.c_str());
		return 0;
	}
	return (int16)result;
}

Instruction::Instruction(const Common::String &opcodeName)
	: _type(INSTRUCTION_OPCODE),
	  _address(Context::currentAddress),
	  _opcodeName(opcodeName),
	  _valueType(VALUE_NULL),
	  _label(0) {
	// Set up opcodes tables unless they have been set already
	if (_opcodes.empty())
		setOpcodes();
	if (_subOpcodes.empty())
		setSubOpcodes();

	// Check if opcode exists
	if (!_opcodes.contains(_opcodeName) && !_subOpcodes.contains(_opcodeName)) {
		Compiler::log(LOG_ERROR, "Unknown opcode \"%s\"", _opcodeName.c_str());
		return;
	}

	// Update current address
	Context::currentAddress++;
}

Instruction::Instruction(ValueType valueType, const Common::String &value, const Common::String &pretty)
	: _type(INSTRUCTION_VALUE),
	  _address(Context::currentAddress),
	  _valueType(valueType),
	  _value(value),
	  _pretty(pretty),
	  _label(0) {
	// Update current address
	switch (_valueType) {
	case VALUE_NULL:
		break;
	case VALUE_BYTE:
		Context::currentAddress++;
		break;
	case VALUE_WORD:
		Context::currentAddress += 2;
		break;
	case VALUE_STRING:
		Context::currentAddress += value.size() + 1;
		break;
	}
}

Instruction::Instruction(const Common::Array<byte> &stringData, const Common::String &pretty)
	: _type(INSTRUCTION_VALUE),
	  _address(Context::currentAddress),
	  _valueType(VALUE_STRING),
	  _stringData(stringData),
	  _pretty(pretty),
	  _label(0) {
	Context::currentAddress += stringData.size() + 1;
}

Instruction::Instruction(uint32 label)
	: _type(INSTRUCTION_LABEL),
	  _address(Context::currentAddress),
	  _valueType(VALUE_NULL),
	  _label(label) {
}

Common::String Instruction::toString() {
	switch (_type) {
	case INSTRUCTION_OPCODE:
		return Common::String::format("%u: %s", _address, _opcodeName.c_str());
	case INSTRUCTION_VALUE:
		switch (_valueType) {
		case VALUE_NULL:
			return "";
		case VALUE_BYTE:
			return Common::String::format("%u: .byte %s", _address,
				(_pretty.empty() ? _value : Common::String("\"") + _pretty + "\"").c_str());
		case VALUE_WORD:
			return Common::String::format("%u: .word %s", _address,
				(_pretty.empty() ? _value : Common::String("\"") + _pretty + "\"").c_str());
		case VALUE_STRING:
			return Common::String::format("%u: .string \"%s\"", _address, _pretty.c_str());
		}
		break;
	case INSTRUCTION_LABEL:
		return Common::String::format("LABEL_%u:", _label);
	}
	return "";
}

void Instruction::assemble(Common::Array<byte> &byteCode) {
	uint16 word;
	switch (_type) {
	case INSTRUCTION_OPCODE:
		byteCode.push_back(_opcodes[_opcodeName]);
		break;
	case INSTRUCTION_VALUE:
		switch (_valueType) {
		case VALUE_NULL:
			break;
		case VALUE_BYTE:
			byteCode.push_back((byte)toInteger(_value));
			break;
		case VALUE_WORD:
			word = (uint16)toInteger(_value);
			byteCode.push_back(word & 0xFF);
			byteCode.push_back(word >> 8);
			break;
		case VALUE_STRING:
			for (uint i = 0; i < _stringData.size(); ++i)
				byteCode.push_back(_stringData[i]);
			byteCode.push_back(0);
			break;
		}
		break;
	case INSTRUCTION_LABEL:
		Compiler::log(LOG_ERROR,"Labels can't be assembled");
		break;
	}
}

} // End of namespace Compiler

} // End of namespace Editor

} // End of namespace Scumm
