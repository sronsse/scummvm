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

#ifndef SCUMM_EDITOR_COMPILER_COMPILER_H
#define SCUMM_EDITOR_COMPILER_COMPILER_H

#include "common/array.h"
#include "common/scummsys.h"
#include "common/str.h"

namespace Scumm {

namespace Editor {

namespace Compiler {

enum LogLevel {
	LOG_DEBUG,
	LOG_WARNING,
	LOG_ERROR
};

class Compiler {
public:
	static bool compile(const char *source, Common::Array<byte> &bytecode, Common::String &log);
	static void log(LogLevel level, const char *format, ...);

private:
	static Common::String _logText;
	static bool _hasErrors;
};

} // End of namespace Compiler

} // End of namespace Editor

} // End of namespace Scumm

#endif
