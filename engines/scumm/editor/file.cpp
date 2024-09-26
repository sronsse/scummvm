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

#include "common/file.h"
#include "common/fs.h"

#include "scumm/editor/file.h"

namespace Scumm {

namespace Editor {

bool File::openRead(const Common::Path &filename) {
	seek(0);
	_size = 0;
	_path = Common::Path();

	Common::File f;
	if (!f.open(Common::FSNode(filename)))
		return false;

	uint32 fileSize = f.size();
	ensureCapacity(fileSize);
	f.read(_data, fileSize);
	_size = fileSize;

	// Decrypt
	for (uint32 i = 0; i < fileSize; ++i)
		_data[i] ^= _encByte;

	return true;
}

bool File::openWrite(const Common::Path &filename) {
	seek(0);
	_size = 0;
	_path = filename;
	return true;
}

void File::openMem() {
	seek(0);
	_size = 0;
	_path = Common::Path();
}

void File::close() {
	if (!_path.empty()) {
		// Encrypt and write
		Common::Array<byte> encoded(_data, _size);
		for (uint32 i = 0; i < encoded.size(); ++i)
			encoded[i] ^= _encByte;

		Common::DumpFile f;
		if (f.open(_path)) {
			f.write(encoded.data(), encoded.size());
			f.close();
		}
	}

	seek(0);
	_size = 0;
	_path = Common::Path();
}

Common::Array<byte> File::data() const {
	return Common::Array<byte>(_data, _size);
}

uint32 File::read(void *dataPtr, uint32 dataSize) {
	if (_pos >= _size)
		return 0;
	uint32 available = _size - _pos;
	uint32 toRead = (dataSize < available) ? dataSize : available;
	memcpy(dataPtr, _data + _pos, toRead);
	_pos += toRead;
	return toRead;
}

byte File::readByte() {
	byte b;
	read(&b, 1);
	return b;
}

uint16 File::readUint16LE() {
	byte buf[2];
	read(buf, 2);
	return READ_LE_UINT16(buf);
}

uint32 File::readUint32LE() {
	byte buf[4];
	read(buf, 4);
	return READ_LE_UINT32(buf);
}

uint16 File::readUint16BE() {
	byte buf[2];
	read(buf, 2);
	return READ_BE_UINT16(buf);
}

uint32 File::readUint32BE() {
	byte buf[4];
	read(buf, 4);
	return READ_BE_UINT32(buf);
}

void File::writeByte(byte value) {
	write(&value, 1);
}

void File::writeUint16LE(uint16 value) {
	byte buf[2];
	WRITE_LE_UINT16(buf, value);
	write(buf, 2);
}

void File::writeUint32LE(uint32 value) {
	byte buf[4];
	WRITE_LE_UINT32(buf, value);
	write(buf, 4);
}

void File::writeUint16BE(uint16 value) {
	byte buf[2];
	WRITE_BE_UINT16(buf, value);
	write(buf, 2);
}

void File::writeUint32BE(uint32 value) {
	byte buf[4];
	WRITE_BE_UINT32(buf, value);
	write(buf, 4);
}

} // End of namespace Editor

} // End of namespace Scumm
