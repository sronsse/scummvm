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

#ifndef SCUMM_EDITOR_FILE_H
#define SCUMM_EDITOR_FILE_H

#include "common/endian.h"
#include "common/memstream.h"
#include "common/path.h"

namespace Scumm {

namespace Editor {

class File : public Common::MemoryWriteStreamDynamic {
protected:
	Common::Path _path;
	byte _encByte;

public:
	File(byte encByte) : Common::MemoryWriteStreamDynamic(DisposeAfterUse::YES), _encByte(encByte) {}

	bool openRead(const Common::Path &filename);
	bool openWrite(const Common::Path &filename);
	void openMem();
	void close();

	Common::Array<byte> data() const;

	uint32 read(void *dataPtr, uint32 dataSize);
	byte readByte();
	uint16 readUint16LE();
	uint32 readUint32LE();
	uint16 readUint16BE();
	uint32 readUint32BE();

	void writeByte(byte value);
	void writeUint16LE(uint16 value);
	void writeUint32LE(uint32 value);
	void writeUint16BE(uint16 value);
	void writeUint32BE(uint32 value);
};

} // End of namespace Editor

} // End of namespace Scumm

#endif
