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

#include "scumm/editor/editor.h"
#include "scumm/editor/style.h"
#include "scumm/editor/compiler/instruction.h"

namespace Scumm {

namespace Editor {

struct DisasmLine {
	uint32 address;
	uint32 size;
	Common::String hexBytes;
	Common::String opcodeName;
	Common::String operands;
};

enum OperandType { OPERAND_NONE, OPERAND_BYTE, OPERAND_WORD, OPERAND_SUBOP, OPERAND_STRING };

static OperandType getOperandType(byte opcode) {
	using namespace Compiler;

	switch (opcode) {
	case OP_PUSH_BYTE:
	case OP_PUSH_BYTE_VAR:
	case OP_BYTE_ARRAY_READ:
	case OP_BYTE_ARRAY_INDEXED_READ:
	case OP_WRITE_BYTE_VAR:
	case OP_BYTE_ARRAY_WRITE:
	case OP_BYTE_ARRAY_INDEXED_WRITE:
	case OP_BYTE_VAR_INC:
	case OP_BYTE_ARRAY_INC:
	case OP_BYTE_VAR_DEC:
	case OP_BYTE_ARRAY_DEC:
		return OPERAND_BYTE;

	case OP_PUSH_WORD:
	case OP_PUSH_WORD_VAR:
	case OP_WORD_ARRAY_READ:
	case OP_WORD_ARRAY_INDEXED_READ:
	case OP_WRITE_WORD_VAR:
	case OP_WORD_ARRAY_WRITE:
	case OP_WORD_ARRAY_INDEXED_WRITE:
	case OP_WORD_VAR_INC:
	case OP_WORD_ARRAY_INC:
	case OP_WORD_VAR_DEC:
	case OP_WORD_ARRAY_DEC:
	case OP_IF:
	case OP_IF_NOT:
	case OP_JUMP:
		return OPERAND_WORD;

	case OP_CURSOR_COMMAND:
	case OP_RESOURCE_ROUTINES:
	case OP_ROOM_OPS:
	case OP_ACTOR_OPS:
	case OP_VERB_OPS:
	case OP_ARRAY_OPS:
	case OP_SAVE_RESTORE_VERBS:
	case OP_WAIT:
	case OP_SYSTEM_OPS:
	case OP_DIM_ARRAY:
	case OP_DIM_2_DIM_ARRAY:
	case OP_PRINT_LINE:
	case OP_PRINT_TEXT:
	case OP_PRINT_DEBUG:
	case OP_PRINT_SYSTEM:
	case OP_PRINT_ACTOR:
	case OP_PRINT_EGO:
	case OP_TALK_ACTOR:
	case OP_TALK_EGO:
		return OPERAND_SUBOP;

	case OP_SET_OBJECT_NAME:
		return OPERAND_STRING;

	default:
		return OPERAND_NONE;
	}
}

static Common::String decodeString(const byte *data, uint32 size, uint32 &pos) {
	Common::String result = "\"";

	while (pos < size) {
		byte ch = data[pos++];
		if (ch == 0x00)
			break;

		if (ch != 0xFF) {
			switch (ch) {
			case '%':
				result += "\\%";
				break;
			default:
				if (ch >= 0x20 && ch < 0x7F)
					result += (char)ch;
				else
					result += Common::String::format("\\x%02X", ch);
				break;
			}
			continue;
		}

		if (pos >= size)
			break;

		byte cmd = data[pos++];

		switch (cmd) {
		case 1:
			result += "\\n";
			break;
		case 2:
			result += "\\k";
			break;
		case 3:
			result += "\\w";
			break;
		case 8:
			result += "\\v";
			break;
		case 4:
		case 5:
		case 6:
		case 7:
			if (pos + 1 < size) {
				uint16 val = data[pos] | (data[pos + 1] << 8);
				pos += 2;
				const char code[] = { 'i', 'v', 'n', 's' };
				result += Common::String::format("%%%c{%u}", code[cmd - 4], val);
			}
			break;
		case 0x0A:
			if (pos + 1 < size) {
				auto readVoicePair = [&]() -> uint16 {
					if (pos + 3 < size && data[pos] == 0xFF && data[pos + 1] == 0x0A) {
						uint16 val = data[pos + 2] | (data[pos + 3] << 8);
						pos += 4;
						return val;
					}
					return 0;
				};
				uint32 offset = (data[pos] | (data[pos + 1] << 8));
				pos += 2;
				offset |= readVoicePair() << 16;
				uint32 length = readVoicePair();
				length |= readVoicePair() << 16;
				result += Common::String::format("%%V{%u,%u}", offset, length);
			}
			break;
		default:
			if (pos + 1 < size) {
				result += Common::String::format("\\xFF\\x%02X\\x%02X\\x%02X", cmd, data[pos], data[pos + 1]);
				pos += 2;
			} else {
				result += Common::String::format("\\xFF\\x%02X", cmd);
			}
			break;
		}
	}

	result += "\"";
	return result;
}

static Common::Array<DisasmLine> disassemble(const byte *data, uint32 size) {
	Common::Array<DisasmLine> lines;
	uint32 pos = 0;

	while (pos < size) {
		DisasmLine line;
		line.address = pos;

		byte op = data[pos];
		uint32 start = pos;
		pos++;

		const char *name = Compiler::Instruction::getOpcodeName(op);
		if (name) {
			line.opcodeName = name;
		} else {
			line.opcodeName = "";
			line.operands = Common::String::format(".byte 0x%02X", op);
		}

		switch (getOperandType(op)) {
		case OPERAND_BYTE:
			if (pos < size) {
				byte val = data[pos++];
				line.operands = Common::String::format(".byte %d", val);
			}
			break;
		case OPERAND_WORD:
			if (pos + 1 < size) {
				int16 val = (int16)(data[pos] | (data[pos + 1] << 8));
				pos += 2;
				line.operands = Common::String::format(".word %d", val);
			}
			break;
		case OPERAND_SUBOP:
			if (pos < size) {
				byte subop = data[pos++];
				line.operands = Common::String::format(".byte %d", subop);
				if (subop == Compiler::SO_TEXTSTRING)
					line.operands += " .string " + decodeString(data, size, pos);
			}
			break;
		case OPERAND_STRING:
			line.operands = ".string " + decodeString(data, size, pos);
			break;
		case OPERAND_NONE:
			break;
		}

		// Build hex bytes string
		Common::String hex;
		for (uint32 i = start; i < pos; ++i) {
			if (i > start)
				hex += " ";
			hex += Common::String::format("%02X", data[i]);
		}
		line.hexBytes = hex;
		line.size = pos - start;

		lines.push_back(line);
	}

	return lines;
}

Script::Script(DSCR &dscr, LECF &lecf)
	: _dscr(dscr),
	  _lecf(lecf),
	  _selectedType(SCRIPT_NONE),
	  _selectedScript(-1),
	  _selectedRoom(-1),
	  _sourceType(SCRIPT_NONE),
	  _sourceScriptId(-1),
	  _sourceRoom(-1),
	  _sourceEditing(false) {
}

SCRP *Script::findGlobalScript(int scriptId) {
	if (scriptId < 0 || scriptId >= (int)_dscr.locations.size())
		return nullptr;

	byte roomNum = _dscr.locations[scriptId];
	if (roomNum == 0)
		return nullptr;

	for (uint i = 0; i < _lecf.loff.locations.size(); ++i) {
		if (_lecf.loff.locations[i] == roomNum) {
			LFLF &lflf = _lecf.lflfs[i];
			for (uint k = 0; k < lflf.scrps.size(); ++k) {
				if (lflf.scrps[k].id == scriptId)
					return &lflf.scrps[k];
			}
			return nullptr;
		}
	}
	return nullptr;
}

LFLF *Script::findLFLF(int roomIndex) {
	if (roomIndex < 0 || roomIndex >= (int)_lecf.lflfs.size())
		return nullptr;
	return &_lecf.lflfs[roomIndex];
}

Common::Array<byte> *Script::getSelectedScriptData() {
	if (_selectedType == SCRIPT_NONE || _selectedRoom < 0)
		return nullptr;

	LFLF *lflf = findLFLF(_selectedRoom);
	if (!lflf)
		return nullptr;

	switch (_selectedType) {
	case SCRIPT_GLOBAL: {
		SCRP *scrp = findGlobalScript(_selectedScript);
		return scrp ? &scrp->script.code : nullptr;
	}
	case SCRIPT_ENTRANCE:
		return &lflf->room.encd.script.code;
	case SCRIPT_EXIT:
		return &lflf->room.excd.script.code;
	case SCRIPT_LOCAL: {
		int index = _selectedScript - 1;
		if (index < 0 || index >= (int)lflf->room.lscrs.size())
			return nullptr;
		return &lflf->room.lscrs[index].script.code;
	}
	default:
		return nullptr;
	}
}

void Script::renderScriptList() {
	ImGui::BeginChild("ScriptList", ImVec2(200, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX);
	ImGui::SeparatorText("Scripts");

	// Build room -> script map
	Common::HashMap<int, Common::Array<int>> roomScripts;
	for (uint i = 0; i < _dscr.locations.size(); ++i) {
		byte roomNum = _dscr.locations[i];
		if (roomNum == 0)
			continue;
		for (uint j = 0; j < _lecf.loff.locations.size(); ++j) {
			if (_lecf.loff.locations[j] == roomNum) {
				roomScripts[j].push_back((int)i);
				break;
			}
		}
	}

	// Render tree
	for (uint i = 0; i < _lecf.lflfs.size(); ++i) {
		if (_lecf.loff.locations[i] == 0)
			continue;

		LFLF &lflf = _lecf.lflfs[i];
		byte roomNum = _lecf.loff.locations[i];

		char roomLabel[64];
		Common::sprintf_s(roomLabel, "Room %d", roomNum);

		ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_DefaultOpen;
		if (ImGui::TreeNodeEx(roomLabel, nodeFlags)) {
			// Global scripts
			if (roomScripts.contains((int)i)) {
				const Common::Array<int> &scripts = roomScripts[(int)i];
				for (uint j = 0; j < scripts.size(); ++j) {
					int scriptId = scripts[j];
					char scriptLabel[64];
					Common::sprintf_s(scriptLabel, "Script %d", scriptId);
					bool selected = (_selectedType == SCRIPT_GLOBAL && _selectedScript == scriptId && _selectedRoom == (int)i);
					if (ImGui::Selectable(scriptLabel, selected)) {
						_selectedType = SCRIPT_GLOBAL;
						_selectedScript = scriptId;
						_selectedRoom = (int)i;
					}
				}
			}

			// Entrance script
			if (!lflf.room.encd.script.code.empty()) {
				bool selected = (_selectedType == SCRIPT_ENTRANCE && _selectedRoom == (int)i);
				if (ImGui::Selectable("Entrance", selected)) {
					_selectedType = SCRIPT_ENTRANCE;
					_selectedScript = 0;
					_selectedRoom = (int)i;
				}
			}

			// Exit script
			if (!lflf.room.excd.script.code.empty()) {
				bool selected = (_selectedType == SCRIPT_EXIT && _selectedRoom == (int)i);
				if (ImGui::Selectable("Exit", selected)) {
					_selectedType = SCRIPT_EXIT;
					_selectedScript = 0;
					_selectedRoom = (int)i;
				}
			}

			// Local scripts
			for (uint j = 0; j < lflf.room.lscrs.size(); ++j) {
				int localIndex = (int)j + 1;
				char scriptLabel[64];
				Common::sprintf_s(scriptLabel, "Local %d", lflf.room.lscrs[j].id);
				bool selected = (_selectedType == SCRIPT_LOCAL && _selectedScript == localIndex && _selectedRoom == (int)i);
				if (ImGui::Selectable(scriptLabel, selected)) {
					_selectedType = SCRIPT_LOCAL;
					_selectedScript = localIndex;
					_selectedRoom = (int)i;
				}
			}

			ImGui::TreePop();
		}
	}

	ImGui::EndChild();
}

Common::String Script::generateSource() {
	Common::Array<byte> *data = getSelectedScriptData();
	if (!data || data->empty())
		return "";

	const byte *code = data->data();
	uint32 codeSize = data->size();
	Common::Array<DisasmLine> lines = disassemble(code, codeSize);

	// Skip trailing stopObjectCode
	uint lineCount = lines.size();
	if (lineCount > 0 && (lines[lineCount - 1].opcodeName == "stopObjectCode" || lines[lineCount - 1].opcodeName == "stopObjectCode2"))
		lineCount--;

	// Generate source
	Common::String src = "function main() {\n    asm {\n";
	for (uint i = 0; i < lineCount; ++i) {
		const DisasmLine &line = lines[i];
		if (line.opcodeName.empty()) {
			src += "        " + line.operands + "\n";
		} else {
			src += "        " + line.opcodeName;
			if (!line.operands.empty())
				src += " " + line.operands;
			src += "\n";
		}
	}
	src += "    }\n}\n";
	return src;
}

enum SourceTokenType { TOKEN_KEYWORD, TOKEN_DIRECTIVE, TOKEN_OPCODE, TOKEN_NUMBER, TOKEN_STRING, TOKEN_BRACE, TOKEN_COMMENT, TOKEN_IDENTIFIER, TOKEN_WHITESPACE };

struct SourceToken {
	SourceTokenType type;
	Common::String text;
	SourceToken(SourceTokenType t, const Common::String &s) : type(t), text(s) {}
};

static bool isAlpha(char ch) { return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'); }
static bool isDigit(char ch) { return ch >= '0' && ch <= '9'; }
static bool isHexDigit(char ch) { return isDigit(ch) || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F'); }
static bool isAlnum(char ch) { return isAlpha(ch) || isDigit(ch); }
static bool isSpace(char ch) { return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r'; }

static bool isKeyword(const Common::String &s) {
	return s == "action" || s == "asm" || s == "break" || s == "case" || s == "catch" || s == "const" ||
	       s == "continue" || s == "cutscene" || s == "default" || s == "do" || s == "else" || s == "enum" ||
	       s == "finally" || s == "for" || s == "function" || s == "if" || s == "inline" || s == "return" ||
	       s == "switch" || s == "thread" || s == "try" || s == "var" || s == "while";
}

static Common::Array<SourceToken> tokenizeSource(const Common::String &src) {
	Common::Array<SourceToken> tokens;
	uint pos = 0;
	int asmBraceDepth = 0;
	bool inAsm = false;

	auto span = [&](uint start) -> Common::String { return Common::String(src.c_str() + start, pos - start); };

	while (pos < src.size()) {
		char ch = src[pos];
		uint start = pos;

		// Whitespace
		if (isSpace(ch)) {
			while (pos < src.size() && isSpace(src[pos]))
				pos++;
			tokens.push_back(SourceToken(TOKEN_WHITESPACE, span(start)));
			continue;
		}

		// Single-line comment
		if (ch == '/' && pos + 1 < src.size() && src[pos + 1] == '/') {
			while (pos < src.size() && src[pos] != '\n')
				pos++;
			tokens.push_back(SourceToken(TOKEN_COMMENT, span(start)));
			continue;
		}

		// Multi-line comment
		if (ch == '/' && pos + 1 < src.size() && src[pos + 1] == '*') {
			pos += 2;
			while (pos + 1 < src.size() && !(src[pos] == '*' && src[pos + 1] == '/'))
				pos++;
			if (pos + 1 < src.size())
				pos += 2;
			tokens.push_back(SourceToken(TOKEN_COMMENT, span(start)));
			continue;
		}

		// String literal
		if (ch == '"') {
			pos++;
			while (pos < src.size() && src[pos] != '"') {
				if (src[pos] == '\\' && pos + 1 < src.size())
					pos++;
				pos++;
			}
			if (pos < src.size())
				pos++;
			tokens.push_back(SourceToken(TOKEN_STRING, span(start)));
			continue;
		}

		// Braces
		if (ch == '{' || ch == '}' || ch == '(' || ch == ')') {
			pos++;
			tokens.push_back(SourceToken(TOKEN_BRACE, span(start)));
			if (ch == '{' && inAsm)
				asmBraceDepth++;
			else if (ch == '}' && asmBraceDepth > 0) {
				asmBraceDepth--;
				if (asmBraceDepth == 0)
					inAsm = false;
			}
			continue;
		}

		// Directive
		if (ch == '.' && pos + 1 < src.size() && isAlpha(src[pos + 1])) {
			pos++;
			while (pos < src.size() && (isAlnum(src[pos]) || src[pos] == '_'))
				pos++;
			tokens.push_back(SourceToken(TOKEN_DIRECTIVE, span(start)));
			continue;
		}

		// Number
		if (isDigit(ch) || (ch == '-' && pos + 1 < src.size() && isDigit(src[pos + 1]))) {
			if (ch == '-')
				pos++;
			if (pos + 1 < src.size() && src[pos] == '0' && (src[pos + 1] == 'x' || src[pos + 1] == 'X')) {
				pos += 2;
				while (pos < src.size() && isHexDigit(src[pos]))
					pos++;
			} else {
				while (pos < src.size() && isDigit(src[pos]))
					pos++;
			}
			tokens.push_back(SourceToken(TOKEN_NUMBER, span(start)));
			continue;
		}

		// Identifier/keyword/opcode
		if (isAlpha(ch) || ch == '_') {
			while (pos < src.size() && (isAlnum(src[pos]) || src[pos] == '_'))
				pos++;
			Common::String word = span(start);
			SourceTokenType type;
			if (inAsm && asmBraceDepth > 0) {
				type = Compiler::Instruction::isOpcode(word) ? TOKEN_OPCODE : TOKEN_IDENTIFIER;
			} else {
				if (isKeyword(word)) {
					type = TOKEN_KEYWORD;
					if (word == "asm")
						inAsm = true;
				} else {
					type = Compiler::Instruction::isOpcode(word) ? TOKEN_OPCODE : TOKEN_IDENTIFIER;
				}
			}
			tokens.push_back(SourceToken(type, word));
			continue;
		}

		pos++;
		tokens.push_back(SourceToken(TOKEN_WHITESPACE, span(start)));
	}

	return tokens;
}

static const ImVec4 &getTokenColor(SourceTokenType type) {
	static const ImVec4 colors[] = { SOL_MAGENTA, SOL_BLUE, SOL_CYAN, SOL_YELLOW, SOL_GREEN, SOL_BASE0, SOL_BASE01, SOL_ORANGE, SOL_BASE0 };
	return colors[type];
}

static void renderColoredSource(const Common::String &src) {
	Common::Array<SourceToken> tokens = tokenizeSource(src);

	int lineNum = 1;
	ImGui::TextColored(SOL_BASE01, "%4d ", lineNum++);
	ImGui::SameLine(0, 0);

	for (uint i = 0; i < tokens.size(); ++i) {
		const SourceToken &tok = tokens[i];
		const ImVec4 &color = getTokenColor(tok.type);
		const char *p = tok.text.c_str();
		const char *end = p + tok.text.size();

		while (p < end) {
			const char *nl = p;
			while (nl < end && *nl != '\n')
				nl++;

			if (nl > p) {
				if (tok.type == TOKEN_WHITESPACE)
					ImGui::TextUnformatted(p, nl);
				else
					ImGui::TextColored(color, "%.*s", (int)(nl - p), p);
				ImGui::SameLine(0, 0);
			}

			if (nl < end) {
				ImGui::NewLine();
				ImGui::TextColored(SOL_BASE01, "%4d ", lineNum++);
				ImGui::SameLine(0, 0);
				nl++;
			}

			p = nl;
		}
	}

	ImGui::NewLine();
}

void Script::renderSource() {
	ImGui::BeginChild("Source", ImVec2(0, 0), ImGuiChildFlags_Borders);

	if (_selectedType == SCRIPT_NONE) {
		ImGui::TextColored(SOL_BASE01, "Select a script.");
		ImGui::EndChild();
		return;
	}

	// Regenerate source when selection changes
	if (_selectedType != _sourceType || _selectedScript != _sourceScriptId || _selectedRoom != _sourceRoom) {
		_sourceType = _selectedType;
		_sourceScriptId = _selectedScript;
		_sourceRoom = _selectedRoom;
		_sourceText = generateSource();
		_compileLog.clear();
		_sourceEditing = false;
	}

	Common::Array<byte> *scriptData = getSelectedScriptData();
	LFLF *lflf = findLFLF(_selectedRoom);

	// Header
	char headerBuf[64] = "";
	switch (_selectedType) {
	case SCRIPT_GLOBAL:
		Common::sprintf_s(headerBuf, "Script %d", _selectedScript);
		break;
	case SCRIPT_ENTRANCE:
		Common::sprintf_s(headerBuf, "Entrance Script");
		break;
	case SCRIPT_EXIT:
		Common::sprintf_s(headerBuf, "Exit Script");
		break;
	case SCRIPT_LOCAL:
		if (lflf && _selectedScript > 0 && _selectedScript <= (int)lflf->room.lscrs.size())
			Common::sprintf_s(headerBuf, "Local Script %d", lflf->room.lscrs[_selectedScript - 1].id);
		else
			Common::sprintf_s(headerBuf, "Local Script");
		break;
	default:
		break;
	}
	ImGui::SeparatorText(headerBuf);

	if (scriptData)
		ImGui::TextColored(SOL_BASE0, "Size: %u bytes", (uint32)scriptData->size());
	ImGui::Separator();

	if (!_sourceEditing) {
		if (ImGui::Button(ICON_EDIT " Edit"))
			_sourceEditing = true;

		ImGui::BeginChild("##SourceView", ImVec2(-1, -1), ImGuiChildFlags_Borders);
		renderColoredSource(_sourceText);
		ImGui::EndChild();
	} else {
		static char buf[65536];
		uint32 len = _sourceText.size();
		if (len >= sizeof(buf))
			len = sizeof(buf) - 1;
		memcpy(buf, _sourceText.c_str(), len);
		buf[len] = '\0';

		// Compile button
		if (ImGui::Button(ICON_BUILD " Compile")) {
			_compiledBytecode.clear();
			_compileLog.clear();
			_compileSuccess = Compiler::Compiler::compile(_sourceText.c_str(), _compiledBytecode, _compileLog);
			_compileLog += _compileSuccess ? "Compilation succeeded.\n" : "Compilation failed.\n";
		}

		ImGui::SameLine();

		// Back button
		if (ImGui::Button(ICON_BACK " Back")) {
			_sourceText = generateSource();
			_compileLog.clear();
			_compileSuccess = false;
			_compiledBytecode.clear();
			_sourceEditing = false;
		}

		// Text editor
		ImVec2 avail = ImGui::GetContentRegionAvail();
		float logHeight = 100.0f;
		float textHeight = avail.y - logHeight - ImGui::GetStyle().ItemSpacing.y;
		if (textHeight < 100.0f)
			textHeight = 100.0f;

		if (ImGui::InputTextMultiline("##SourceEdit", buf, sizeof(buf), ImVec2(-1, textHeight)))
			_sourceText = buf;

		// Log view
		ImGui::BeginChild("##CompileLog", ImVec2(-1, logHeight), ImGuiChildFlags_Borders);
		ImGui::SeparatorText("Compilation Log");
		if (!_compileLog.empty()) {
			const ImVec4 &logColor = _compileSuccess ? SOL_GREEN : SOL_RED;
			ImGui::PushStyleColor(ImGuiCol_Text, logColor);
			ImGui::TextUnformatted(_compileLog.c_str());
			ImGui::PopStyleColor();
		}
		ImGui::EndChild();
	}

	ImGui::EndChild();
}

void Script::renderDisassembly() {
	ImGui::BeginChild("Disassembly", ImVec2(0, 0), ImGuiChildFlags_Borders);

	if (_selectedType == SCRIPT_NONE) {
		ImGui::TextColored(SOL_BASE01, "Select a script.");
		ImGui::EndChild();
		return;
	}

	Common::Array<byte> *scriptData = getSelectedScriptData();
	if (!scriptData || scriptData->empty()) {
		ImGui::TextColored(SOL_BASE01, "Script data not found.");
		ImGui::EndChild();
		return;
	}

	const byte *code = scriptData->data();
	uint32 codeSize = scriptData->size();
	LFLF *lflf = findLFLF(_selectedRoom);

	// Header
	char headerBuf[64] = "";
	switch (_selectedType) {
	case SCRIPT_GLOBAL:
		Common::sprintf_s(headerBuf, "Script %d", _selectedScript);
		break;
	case SCRIPT_ENTRANCE:
		Common::sprintf_s(headerBuf, "Entrance Script");
		break;
	case SCRIPT_EXIT:
		Common::sprintf_s(headerBuf, "Exit Script");
		break;
	case SCRIPT_LOCAL:
		if (lflf && _selectedScript > 0 && _selectedScript <= (int)lflf->room.lscrs.size())
			Common::sprintf_s(headerBuf, "Local Script %d", lflf->room.lscrs[_selectedScript - 1].id);
		else
			Common::sprintf_s(headerBuf, "Local Script");
		break;
	default:
		break;
	}
	ImGui::SeparatorText(headerBuf);
	ImGui::TextColored(SOL_BASE0, "Size: %u bytes", codeSize);

	Common::Array<DisasmLine> lines = disassemble(code, codeSize);

	// Disassembly table
	ImGui::BeginChild("##DisasmView", ImVec2(-1, -1), ImGuiChildFlags_Borders);

	const ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Resizable;
	if (ImGui::BeginTable("DisasmTable", 3, flags)) {
		ImGui::TableSetupScrollFreeze(0, 1);
		ImGui::TableSetupColumn("Address", ImGuiTableColumnFlags_WidthFixed, 60.0f);
		ImGui::TableSetupColumn("Bytes", ImGuiTableColumnFlags_WidthFixed, 200.0f);
		ImGui::TableSetupColumn("Instruction", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableHeadersRow();

		ImGuiListClipper clipper;
		clipper.Begin((int)lines.size());
		while (clipper.Step()) {
			for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++) {
				const DisasmLine &line = lines[row];
				ImGui::TableNextRow();

				ImGui::TableNextColumn();
				ImGui::TextColored(SOL_BASE01, "%04X", line.address);

				ImGui::TableNextColumn();
				ImGui::TextColored(SOL_GREEN, "%s", line.hexBytes.c_str());

				ImGui::TableNextColumn();
				ImGui::TextColored(SOL_CYAN, "%s", line.opcodeName.c_str());
				if (!line.operands.empty()) {
					ImGui::SameLine();
					ImGui::TextColored(SOL_YELLOW, "%s", line.operands.c_str());
				}
			}
		}

		ImGui::EndTable();
	}

	ImGui::EndChild();
	ImGui::EndChild();
}

void Script::render(ImGuiID dockSpaceId, bool *open) {
	ImGui::SetNextWindowDockID(dockSpaceId, ImGuiCond_FirstUseEver);
	ImGui::Begin(ICON_SCRIPT " Script", open);

	renderScriptList();
	ImGui::SameLine();

	ImGui::BeginChild("ScriptContent", ImVec2(0, 0), ImGuiChildFlags_Borders);
	if (ImGui::BeginTabBar("ScriptTabs")) {
		if (ImGui::BeginTabItem("Source")) {
			renderSource();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Disassembly")) {
			renderDisassembly();
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	ImGui::EndChild();

	ImGui::End();
}

} // End of namespace Editor

} // End of namespace Scumm
