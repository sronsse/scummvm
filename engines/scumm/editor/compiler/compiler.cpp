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

#include "scumm/editor/compiler/compiler.h"
#include "scumm/editor/compiler/context.h"
#include "scumm/editor/compiler/declaration.h"
#include "scumm/editor/compiler/function.h"
#include "grammar.h"

#include <stdarg.h>

struct yy_buffer_state;
typedef yy_buffer_state *YY_BUFFER_STATE;

extern YY_BUFFER_STATE scumm_yy_scan_string(const char *str);
extern void scumm_yy_delete_buffer(YY_BUFFER_STATE buffer);
extern int scumm_yylineno;
extern void resetCollectors();

namespace Scumm {

namespace Editor {

namespace Compiler {

Common::String Compiler::_logText;
bool Compiler::_hasErrors = false;

void Compiler::log(LogLevel level, const char *format, ...) {
	va_list args;
	va_start(args, format);

	char buffer[1024];
	vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);

	const char *prefix = "";
	switch (level) {
	case LOG_DEBUG:
		prefix = "[DEBUG] ";
		break;
	case LOG_WARNING:
		prefix = "[WARNING] ";
		break;
	case LOG_ERROR:
		prefix = "[ERROR] ";
		_hasErrors = true;
		break;
	}

	_logText += prefix;
	_logText += buffer;
	_logText += "\n";
}

bool Compiler::compile(const char *source, Common::Array<byte> &bytecode, Common::String &log) {
	bytecode.clear();
	_logText.clear();
	_hasErrors = false;

	// Parse source into declarations and functions
	Common::Array<Declaration *> declarations;
	Common::Array<Function *> functions;

	resetCollectors();
	YY_BUFFER_STATE buf = scumm_yy_scan_string(source);
	scumm_yylineno = 1;
	scumm_yyparse(declarations, functions);
	scumm_yy_delete_buffer(buf);

	if (!_hasErrors) {
		// Assign function IDs
		uint16 functionID = 1;
		for (uint i = 0; i < functions.size(); ++i)
			functions[i]->setID(functionID++);

		// Push game context with declarations and functions
		Context gameContext(CONTEXT_GAME, &declarations, &functions, -1, -1, -1);
		Context::pushContext(&gameContext);

		// Compile each non-inlined function
		for (uint i = 0; i < functions.size() && !_hasErrors; ++i) {
			if (functions[i]->getType() != FUNCTION_INLINED)
				functions[i]->compile();
		}

		if (!_hasErrors) {
			// Extract bytecode from compiled functions
			for (uint i = 0; i < functions.size(); ++i) {
				if (functions[i]->getType() != FUNCTION_INLINED) {
					for (uint32 j = 0; j < functions[i]->getNumberOfBytes(); ++j)
						bytecode.push_back(functions[i]->getByte(j));
				}
			}
		}

		// Pop game context
		Context::popContext();
	}

	// Clean up AST nodes
	for (uint i = 0; i < declarations.size(); ++i)
		delete declarations[i];
	for (uint i = 0; i < functions.size(); ++i)
		delete functions[i];

	log = _logText;
	return !_hasErrors;
}

} // End of namespace Compiler

} // End of namespace Editor

} // End of namespace Scumm
