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

#ifndef SCUMM_EDITOR_COMPILER_INSTRUCTION_H
#define SCUMM_EDITOR_COMPILER_INSTRUCTION_H

#include "common/array.h"
#include "common/hash-str.h"
#include "common/hashmap.h"
#include "common/str.h"
#include "common/scummsys.h"
namespace Scumm {

namespace Editor {

namespace Compiler {

enum OpType {
	OP_PUSH_BYTE = 0x00,
	OP_PUSH_WORD = 0x01,
	OP_PUSH_BYTE_VAR = 0x02,
	OP_PUSH_WORD_VAR = 0x03,
	OP_BYTE_ARRAY_READ = 0x06,
	OP_WORD_ARRAY_READ = 0x07,
	OP_BYTE_ARRAY_INDEXED_READ = 0x0A,
	OP_WORD_ARRAY_INDEXED_READ = 0x0B,
	OP_DUP = 0x0C,
	OP_NOT = 0x0D,
	OP_EQ = 0x0E,
	OP_NEQ = 0x0F,
	OP_GT = 0x10,
	OP_LT = 0x11,
	OP_LE = 0x12,
	OP_GE = 0x13,
	OP_ADD = 0x14,
	OP_SUB = 0x15,
	OP_MUL = 0x16,
	OP_DIV = 0x17,
	OP_LAND = 0x18,
	OP_LOR = 0x19,
	OP_POP = 0x1A,
	OP_WRITE_BYTE_VAR = 0x42,
	OP_WRITE_WORD_VAR = 0x43,
	OP_BYTE_ARRAY_WRITE = 0x46,
	OP_WORD_ARRAY_WRITE = 0x47,
	OP_BYTE_ARRAY_INDEXED_WRITE = 0x4A,
	OP_WORD_ARRAY_INDEXED_WRITE = 0x4B,
	OP_BYTE_VAR_INC = 0x4E,
	OP_WORD_VAR_INC = 0x4F,
	OP_BYTE_ARRAY_INC = 0x52,
	OP_WORD_ARRAY_INC = 0x53,
	OP_SET_BLAST_OBJECT_WINDOW = 0x54,
	OP_BYTE_VAR_DEC = 0x56,
	OP_WORD_VAR_DEC = 0x57,
	OP_BYTE_ARRAY_DEC = 0x5A,
	OP_WORD_ARRAY_DEC = 0x5B,
	OP_IF = 0x5C,
	OP_IF_NOT = 0x5D,
	OP_START_SCRIPT = 0x5E,
	OP_START_SCRIPT_QUICK = 0x5F,
	OP_START_OBJECT = 0x60,
	OP_DRAW_OBJECT = 0x61,
	OP_DRAW_OBJECT_AT = 0x62,
	OP_DRAW_BLAST_OBJECT = 0x63,
	OP_STOP_OBJECT_CODE = 0x65,
	OP_STOP_OBJECT_CODE_2 = 0x66,
	OP_END_CUTSCENE = 0x67,
	OP_CUTSCENE = 0x68,
	OP_STOP_MUSIC = 0x69,
	OP_FREEZE_UNFREEZE = 0x6A,
	OP_CURSOR_COMMAND = 0x6B,
	OP_BREAK_HERE = 0x6C,
	OP_IF_CLASS_OF_IS = 0x6D,
	OP_SET_CLASS = 0x6E,
	OP_GET_STATE = 0x6F,
	OP_SET_STATE = 0x70,
	OP_SET_OWNER = 0x71,
	OP_GET_OWNER = 0x72,
	OP_JUMP = 0x73,
	OP_START_SOUND = 0x74,
	OP_STOP_SOUND = 0x75,
	OP_START_MUSIC = 0x76,
	OP_STOP_OBJECT_SCRIPT = 0x77,
	OP_PAN_CAMERA_TO = 0x78,
	OP_ACTOR_FOLLOW_CAMERA = 0x79,
	OP_SET_CAMERA_AT = 0x7A,
	OP_LOAD_ROOM = 0x7B,
	OP_STOP_SCRIPT = 0x7C,
	OP_WALK_ACTOR_TO_OBJ = 0x7D,
	OP_WALK_ACTOR_TO = 0x7E,
	OP_PUT_ACTOR_AT_XY = 0x7F,
	OP_PUT_ACTOR_AT_OBJECT = 0x80,
	OP_FACE_ACTOR = 0x81,
	OP_ANIMATE_ACTOR = 0x82,
	OP_DO_SENTENCE = 0x83,
	OP_PICKUP_OBJECT = 0x84,
	OP_LOAD_ROOM_WITH_EGO = 0x85,
	OP_GET_RANDOM_NUMBER = 0x87,
	OP_GET_RANDOM_NUMBER_RANGE = 0x88,
	OP_GET_ACTOR_MOVING = 0x8A,
	OP_IS_SCRIPT_RUNNING = 0x8B,
	OP_GET_ACTOR_ROOM = 0x8C,
	OP_GET_OBJECT_X = 0x8D,
	OP_GET_OBJECT_Y = 0x8E,
	OP_GET_OBJECT_OLD_DIR = 0x8F,
	OP_GET_ACTOR_WALK_BOX = 0x90,
	OP_GET_ACTOR_COSTUME = 0x91,
	OP_FIND_INVENTORY = 0x92,
	OP_GET_INVENTORY_COUNT = 0x93,
	OP_GET_VERB_FROM_XY = 0x94,
	OP_BEGIN_OVERRIDE = 0x95,
	OP_END_OVERRIDE = 0x96,
	OP_SET_OBJECT_NAME = 0x97,
	OP_IS_SOUND_RUNNING = 0x98,
	OP_SET_BOX_FLAGS = 0x99,
	OP_CREATE_BOX_MATRIX = 0x9A,
	OP_RESOURCE_ROUTINES = 0x9B,
	OP_ROOM_OPS = 0x9C,
	OP_ACTOR_OPS = 0x9D,
	OP_VERB_OPS = 0x9E,
	OP_GET_ACTOR_FROM_XY = 0x9F,
	OP_FIND_OBJECT = 0xA0,
	OP_PSEUDO_ROOM = 0xA1,
	OP_GET_ACTOR_ELEVATION = 0xA2,
	OP_GET_VERB_ENTRY_POINT = 0xA3,
	OP_ARRAY_OPS = 0xA4,
	OP_SAVE_RESTORE_VERBS = 0xA5,
	OP_DRAW_BOX = 0xA6,
	OP_GET_ACTOR_WIDTH = 0xA8,
	OP_WAIT = 0xA9,
	OP_GET_ACTOR_SCALE_X = 0xAA,
	OP_GET_ACTOR_ANIM_COUNTER = 0xAB,
	OP_SOUND_KLUDGE = 0xAC,
	OP_IS_ANY_OF = 0xAD,
	OP_SYSTEM_OPS = 0xAE,
	OP_IS_ACTOR_IN_BOX = 0xAF,
	OP_DELAY = 0xB0,
	OP_DELAY_SECONDS = 0xB1,
	OP_DELAY_MINUTES = 0xB2,
	OP_STOP_SENTENCE = 0xB3,
	OP_PRINT_LINE = 0xB4,
	OP_PRINT_TEXT = 0xB5,
	OP_PRINT_DEBUG = 0xB6,
	OP_PRINT_SYSTEM = 0xB7,
	OP_PRINT_ACTOR = 0xB8,
	OP_PRINT_EGO = 0xB9,
	OP_TALK_ACTOR = 0xBA,
	OP_TALK_EGO = 0xBB,
	OP_DIM_ARRAY = 0xBC,
	OP_DUMMY = 0xBD,
	OP_START_OBJECT_QUICK = 0xBE,
	OP_START_SCRIPT_QUICK_2 = 0xBF,
	OP_DIM_2_DIM_ARRAY = 0xC0,
	OP_ABS = 0xC4,
	OP_DIST_OBJECT_OBJECT = 0xC5,
	OP_DIST_OBJECT_PT = 0xC6,
	OP_DIST_PT_PT = 0xC7,
	OP_KERNEL_GET_FUNCTIONS = 0xC8,
	OP_KERNEL_SET_FUNCTIONS = 0xC9,
	OP_DELAY_FRAMES = 0xCA,
	OP_PICK_ONE_OF = 0xCB,
	OP_PICK_ONE_OF_DEFAULT = 0xCC,
	OP_STAMP_OBJECT = 0xCD,
	OP_GET_DATE_TIME = 0xD0,
	OP_STOP_TALKING = 0xD1,
	OP_GET_ANIMATE_VARIABLE = 0xD2,
	OP_SHUFFLE = 0xD4,
	OP_JUMP_TO_SCRIPT = 0xD5,
	OP_BAND = 0xD6,
	OP_BOR = 0xD7,
	OP_IS_ROOM_SCRIPT_RUNNING = 0xD8,
	OP_FIND_ALL_OBJECTS = 0xDD,
	OP_GET_PIXEL = 0xE1,
	OP_PICK_VAR_RANDOM = 0xE3,
	OP_SET_BOX_SET = 0xE4,
	OP_GET_ACTOR_LAYER = 0xEC,
	OP_GET_OBJECT_NEW_DIR = 0xED
};

enum SubOpType {
	SO_AT = 65,
	SO_COLOR = 66,
	SO_CLIPPED = 67,
	SO_CENTER = 69,
	SO_LEFT = 71,
	SO_OVERHEAD = 72,
	SO_MUMBLE = 74,
	SO_TEXTSTRING = 75,
	SO_COSTUME = 76,
	SO_STEP_DIST = 77,
	SO_SOUND = 78,
	SO_WALK_ANIMATION = 79,
	SO_TALK_ANIMATION = 80,
	SO_STAND_ANIMATION = 81,
	SO_ANIMATION = 82,
	SO_DEFAULT = 83,
	SO_ELEVATION = 84,
	SO_ANIMATION_DEFAULT = 85,
	SO_PALETTE = 86,
	SO_TALK_COLOR = 87,
	SO_ACTOR_NAME = 88,
	SO_INIT_ANIMATION = 89,
	SO_ACTOR_WIDTH = 91,
	SO_SCALE = 92,
	SO_NEVER_ZCLIP = 93,
	SO_ALWAYS_ZCLIP = 94,
	SO_IGNORE_BOXES = 95,
	SO_FOLLOW_BOXES = 96,
	SO_ANIMATION_SPEED = 97,
	SO_SHADOW = 98,
	SO_TEXT_OFFSET = 99,
	SO_LOAD_SCRIPT = 100,
	SO_LOAD_SOUND = 101,
	SO_LOAD_COSTUME = 102,
	SO_LOAD_ROOM = 103,
	SO_NUKE_SCRIPT = 104,
	SO_NUKE_SOUND = 105,
	SO_NUKE_COSTUME = 106,
	SO_NUKE_ROOM = 107,
	SO_LOCK_SCRIPT = 108,
	SO_LOCK_SOUND = 109,
	SO_LOCK_COSTUME = 110,
	SO_LOCK_ROOM = 111,
	SO_UNLOCK_SCRIPT = 112,
	SO_UNLOCK_SOUND = 113,
	SO_UNLOCK_COSTUME = 114,
	SO_UNLOCK_ROOM = 115,
	SO_CLEAR_HEAP = 116,
	SO_LOAD_CHARSET = 117,
	SO_NUKE_CHARSET = 118,
	SO_LOAD_OBJECT = 119,
	SO_VERB_IMAGE = 124,
	SO_VERB_NAME = 125,
	SO_VERB_COLOR = 126,
	SO_VERB_HICOLOR = 127,
	SO_VERB_AT = 128,
	SO_VERB_ON = 129,
	SO_VERB_OFF = 130,
	SO_VERB_DELETE = 131,
	SO_VERB_NEW = 132,
	SO_VERB_DIMCOLOR = 133,
	SO_VERB_DIM = 134,
	SO_VERB_KEY = 135,
	SO_VERB_CENTER = 136,
	SO_VERB_NAME_STR = 137,
	SO_VERB_IMAGE_IN_ROOM = 139,
	SO_VERB_BAKCOLOR = 140,
	SO_SAVE_VERBS = 141,
	SO_RESTORE_VERBS = 142,
	SO_DELETE_VERBS = 143,
	SO_CURSOR_ON = 144,
	SO_CURSOR_OFF = 145,
	SO_USERPUT_ON = 146,
	SO_USERPUT_OFF = 147,
	SO_CURSOR_SOFT_ON = 148,
	SO_CURSOR_SOFT_OFF = 149,
	SO_USERPUT_SOFT_ON = 150,
	SO_USERPUT_SOFT_OFF = 151,
	SO_CURSOR_IMAGE = 153,
	SO_CURSOR_HOTSPOT = 154,
	SO_CHARSET_SET = 156,
	SO_CHARSET_COLOR = 157,
	SO_RESTART = 158,
	SO_PAUSE = 159,
	SO_QUIT = 160,
	SO_WAIT_FOR_ACTOR = 168,
	SO_WAIT_FOR_MESSAGE = 169,
	SO_WAIT_FOR_CAMERA = 170,
	SO_WAIT_FOR_SENTENCE = 171,
	SO_ROOM_SCROLL = 172,
	SO_ROOM_SCREEN = 174,
	SO_ROOM_PALETTE = 175,
	SO_ROOM_SHAKE_ON = 176,
	SO_ROOM_SHAKE_OFF = 177,
	SO_ROOM_INTENSITY = 179,
	SO_ROOM_SAVEGAME = 180,
	SO_ROOM_FADE = 181,
	SO_RGB_ROOM_INTENSITY = 182,
	SO_ROOM_SHADOW = 183,
	SO_SAVE_STRING = 184,
	SO_LOAD_STRING = 185,
	SO_ROOM_TRANSFORM = 186,
	SO_CYCLE_SPEED = 187,
	SO_VERB_INIT = 196,
	SO_ACTOR_INIT = 197,
	SO_ACTOR_VARIABLE = 198,
	SO_INT_ARRAY = 199,
	SO_BIT_ARRAY = 200,
	SO_NIBBLE_ARRAY = 201,
	SO_BYTE_ARRAY = 202,
	SO_STRING_ARRAY = 203,
	SO_UNDIM_ARRAY = 204,
	SO_ASSIGN_STRING = 205,
	SO_ASSIGN_INT_LIST = 208,
	SO_ASSIGN_2DIM_LIST = 212,
	SO_ROOM_NEW_PALETTE = 213,
	SO_CURSOR_TRANSPARENT = 214,
	SO_ACTOR_IGNORE_TURNS_ON = 215,
	SO_ACTOR_IGNORE_TURNS_OFF = 216,
	SO_NEW = 217,
	SO_ALWAYS_ZCLIP_FT_DEMO = 225,
	SO_WAIT_FOR_ANIMATION = 226,
	SO_ACTOR_DEPTH = 227,
	SO_ACTOR_WALK_SCRIPT = 228,
	SO_ACTOR_STOP = 229,
	SO_ACTOR_FACE = 230,
	SO_ACTOR_TURN = 231,
	SO_WAIT_FOR_TURN = 232,
	SO_ACTOR_WALK_PAUSE = 233,
	SO_ACTOR_WALK_RESUME = 234,
	SO_ACTOR_TALK_SCRIPT = 235,
	SO_BASEOP = 254,
	SO_END = 255
};

enum ValueType {
	VALUE_NULL,
	VALUE_BYTE,
	VALUE_WORD,
	VALUE_STRING
};

enum InstructionType {
	INSTRUCTION_OPCODE,
	INSTRUCTION_VALUE,
	INSTRUCTION_LABEL
};

class Instruction {
private:
	static Common::HashMap<Common::String, byte> _opcodes;
	static Common::HashMap<Common::String, byte> _subOpcodes;

	InstructionType _type;
	uint32 _address;
	Common::String _opcodeName;
	ValueType _valueType;
	Common::String _value;
	Common::Array<byte> _stringData;
	Common::String _pretty;
	uint32 _label;

	static int16 toInteger(const Common::String &s);

public:
	static void setOpcodes();
	static void setSubOpcodes();
	static const char *getOpcodeName(byte opcode);
	static const char *getSubOpcodeName(byte subOpcode);
	static bool isOpcode(const Common::String &name);

	Instruction(const Common::String &opcodeName);
	Instruction(ValueType valueType, const Common::String &value, const Common::String &pretty = "");
	Instruction(const Common::Array<byte> &stringData, const Common::String &pretty = "");
	Instruction(uint32 label);

	InstructionType getType() { return _type; }
	uint32 getAddress() { return _address; }
	Common::String getOpcodeName() { return _opcodeName; }
	Common::String getValue() { return _value; }
	void setValue(const Common::String &value) { _value = value; }
	uint32 getLabel() { return _label; }
	Common::String toString();
	void assemble(Common::Array<byte> &byteCode);
};

} // End of namespace Compiler

} // End of namespace Editor

} // End of namespace Scumm

#endif
