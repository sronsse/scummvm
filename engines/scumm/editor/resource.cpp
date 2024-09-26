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

#include "scumm/editor/resource.h"

#include "common/debug.h"

#include "scumm/detection.h"

namespace Scumm {

namespace Editor {

void Resource::readHeader() {
	_blockPos = pos();
	_blockType = readUint32BE();
	_blockSize = readUint32BE();
}

void Resource::readHeader(const char *str) {
	readHeader();
	if (strcmp(str, tag2str(_blockType)))
		error("Expected header \"%s\" (read \"%s\")", str, tag2str(_blockType));
}

void Resource::writeHeader(const char *str, uint32 size) {
	writeUint32BE(MKTAG(str[0], str[1], str[2], str[3]));
	writeUint32BE(size);
}

void Resource::readAARY(AARY *aary) {
	uint16 varNumber;
	while ((varNumber = readUint16LE()) != 0) {
		Array a;
		a.varNumber = varNumber;
		a.dimA = readUint16LE();
		a.dimB = readUint16LE();
		a.type = readUint16LE();
		aary->arrays.push_back(a);
	}
}

void Resource::readAPAL(APAL *apal) {
	uint numColors = (_blockSize - 8) / 3;
	for (uint i = 0; i < numColors; ++i) {
		Color c;
		c.r = readByte();
		c.g = readByte();
		c.b = readByte();
		apal->colors.push_back(c);
	}
}

void Resource::readBOMP(BOMP *bomp) {
	bomp->unk = readUint16LE();
	bomp->width = readUint16LE();
	bomp->height = readUint16LE();
	bomp->pad1 = readUint16LE();
	bomp->pad2 = readUint16LE();
	while (pos() < _blockPos + _blockSize)
		bomp->data.push_back(readByte());
}

void Resource::readBOXD(BOXD *boxd) {
	uint16 numBoxes = readUint16LE();
	for (uint16 i = 0; i < numBoxes; ++i) {
		Box b;
		b.ulx = readUint16LE();
		b.uly = readUint16LE();
		b.urx = readUint16LE();
		b.ury = readUint16LE();
		b.lrx = readUint16LE();
		b.lry = readUint16LE();
		b.llx = readUint16LE();
		b.lly = readUint16LE();
		b.mask = readByte();
		b.flags = readByte();
		b.scale = readUint16LE();
		boxd->boxes.push_back(b);
	}
}

void Resource::readBOXM(BOXM *boxm) {
	while (pos() < _blockPos + _blockSize) {
		Common::Array<Entry> row;
		while (pos() < _blockPos + _blockSize) {
			byte b = readByte();
			if (b == 0xFF)
				break;
			Entry entry;
			entry.from = b;
			entry.to = readByte();
			entry.next = readByte();
			row.push_back(entry);
		}
		boxm->matrix.push_back(row);
	}
}

void Resource::readCDHD(CDHD *cdhd) {
	cdhd->id = readUint16LE();
	cdhd->x = readUint16LE();
	cdhd->y = readUint16LE();
	cdhd->width = readUint16LE();
	cdhd->height = readUint16LE();
	cdhd->flags = readByte();
	cdhd->parent = readByte();
	cdhd->unk1 = readUint16LE();
	cdhd->unk2 = readUint16LE();
	cdhd->actorDir = readByte();
}

void Resource::readCHAR(CHR *chr) {
	while (pos() < _blockPos + _blockSize)
		chr->data.push_back(readByte());
}

void Resource::readCOST(COST *cost) {
	while (pos() < _blockPos + _blockSize)
		cost->data.push_back(readByte());
}

void Resource::readCYCL(CYCL *cycl) {
	byte id;
	while ((id = readByte()) != 0) {
		Cycle c;
		c.id = id;
		c.unk = readUint16LE();
		c.freq = readUint16BE();
		c.flags = readUint16BE();
		c.start = readByte();
		c.end = readByte();
		cycl->cycles.push_back(c);
	}
	while (pos() < _blockPos + _blockSize)
		cycl->unk.push_back(readByte());
}

void Resource::readDCHR(DCHR *dchr) {
	uint16 numCharsets = readUint16LE();
	for (uint16 i = 0; i < numCharsets; ++i)
		dchr->locations.push_back(readByte());
	for (uint16 i = 0; i < numCharsets; ++i)
		dchr->offsets.push_back(readUint32LE());
}

void Resource::readDCOS(DCOS *dcos) {
	uint16 numCostumes = readUint16LE();
	for (uint16 i = 0; i < numCostumes; ++i)
		dcos->locations.push_back(readByte());
	for (uint16 i = 0; i < numCostumes; ++i)
		dcos->offsets.push_back(readUint32LE());
}

void Resource::readDOBJ(DOBJ *dobj) {
	uint16 numObjects = readUint16LE();
	for (uint16 i = 0; i < numObjects; ++i)
		dobj->ownerTable.push_back(readByte());
	for (uint16 i = 0; i < numObjects; ++i)
		dobj->classData.push_back(readUint32LE());
}

void Resource::readDROO(DROO *droo) {
	uint16 numRooms = readUint16LE();
	for (uint16 i = 0; i < numRooms; ++i)
		droo->locations.push_back(readByte());
	for (uint16 i = 0; i < numRooms; ++i)
		droo->offsets.push_back(readUint32LE());
}

void Resource::readDSCR(DSCR *dscr) {
	uint16 numScripts = readUint16LE();
	for (uint16 i = 0; i < numScripts; ++i)
		dscr->locations.push_back(readByte());
	for (uint16 i = 0; i < numScripts; ++i)
		dscr->offsets.push_back(readUint32LE());
}

void Resource::readDSOU(DSOU *dsou) {
	uint16 numSounds = readUint16LE();
	for (uint16 i = 0; i < numSounds; ++i)
		dsou->locations.push_back(readByte());
	for (uint16 i = 0; i < numSounds; ++i)
		dsou->offsets.push_back(readUint32LE());
}

void Resource::readENCD(ENCD *encd) {
	while (pos() < _blockPos + _blockSize)
		encd->script.code.push_back(readByte());
}

void Resource::readEXCD(EXCD *excd) {
	while (pos() < _blockPos + _blockSize)
		excd->script.code.push_back(readByte());
}

void Resource::readIMHD(IMHD *imhd) {
	imhd->id = readUint16LE();
	imhd->numImages = readUint16LE();
	imhd->unk1 = readUint16LE();
	imhd->flags = readByte();
	imhd->unk2 = readByte();
	imhd->unk3 = readUint16LE();
	imhd->unk4 = readUint16LE();
	imhd->width = readUint16LE();
	imhd->height = readUint16LE();
	uint16 numHotspots = readUint16LE();
	for (uint16 i = 0; i < numHotspots; ++i) {
		Hotspot h;
		h.x = readUint16LE();
		h.y = readUint16LE();
		imhd->hotspots.push_back(h);
	}
}

void Resource::readIMxx(IMxx *imxx) {
	uint32 blockPos = _blockPos;
	uint32 blockSize = _blockSize;
	readHeader();
	switch (_blockType) {
	case MKTAG('B','O','M','P'):
		readBOMP(&imxx->bomp);
		break;
	case MKTAG('S','M','A','P'):
		readSMAP(&imxx->smap);
		break;
	default:
		debugC(DEBUG_IMGUI, "Unknown IMxx header: %08X \"%s\"", _blockType, tag2str(_blockType));
		seek(_blockPos + _blockSize);
		break;
	}
	while (pos() < blockPos + blockSize) {
		imxx->zpxxs.emplace_back();
		readHeader();
		readZPxx(&imxx->zpxxs.back());
	}
}

void Resource::readLECF(LECF *lecf) {
	readHeader("LOFF");
	readLOFF(&lecf->loff);
	for (uint i = 0; i < lecf->loff.locations.size(); ++i) {
		lecf->lflfs.emplace_back();
		readHeader("LFLF");
		readLFLF(&lecf->lflfs.back());
	}
}

void Resource::readLFLF(LFLF *lflf) {
	uint32 blockPos = _blockPos;
	uint32 blockSize = _blockSize;
	while (pos() < blockPos + blockSize) {
		readHeader();
		switch (_blockType) {
		case MKTAG('C','H','A','R'):
			lflf->chars.emplace_back();
			readCHAR(&lflf->chars.back());
			break;
		case MKTAG('C','O','S','T'):
			lflf->costs.emplace_back();
			readCOST(&lflf->costs.back());
			break;
		case MKTAG('R','O','O','M'):
			readROOM(&lflf->room);
			break;
		case MKTAG('S','C','R','P'):
			lflf->scrps.emplace_back();
			readSCRP(&lflf->scrps.back());
			break;
		case MKTAG('S','O','U','N'):
			lflf->souns.emplace_back();
			readSOUN(&lflf->souns.back());
			break;
		default:
			debugC(DEBUG_IMGUI, "Unknown LFLF header: %08X \"%s\"", _blockType, tag2str(_blockType));
			seek(_blockPos + _blockSize);
			break;
		}
	}
}

void Resource::readLOFF(LOFF *loff) {
	byte numLocations = readByte();
	for (byte i = 0; i < numLocations; ++i) {
		loff->locations.push_back(readByte());
		loff->offsets.push_back(readUint32LE());
	}
}

void Resource::readLSCR(LSCR *lscr) {
	lscr->id = readByte();
	while (pos() < _blockPos + _blockSize)
		lscr->script.code.push_back(readByte());
}

void Resource::readMAXS(MAXS *maxs) {
	maxs->numVariables = readUint16LE();
	maxs->unk1 = readUint16LE();
	maxs->numBitVariables = readUint16LE();
	maxs->numLocalObjects = readUint16LE();
	maxs->numArray = readUint16LE();
	maxs->unk2 = readUint16LE();
	maxs->numVerbs = readUint16LE();
	maxs->numFlObject = readUint16LE();
	maxs->numInventory = readUint16LE();
	maxs->numRooms = readUint16LE();
	maxs->numScripts = readUint16LE();
	maxs->numSounds = readUint16LE();
	maxs->numCharsets = readUint16LE();
	maxs->numCostumes = readUint16LE();
	maxs->numGlobalObjects = readUint16LE();
}

void Resource::readNLSC(NLSC *nlsc) {
	nlsc->numScripts = readByte();
	readByte();
}

void Resource::readOBCD(OBCD *obcd) {
	readHeader("CDHD");
	readCDHD(&obcd->cdhd);
	readHeader("VERB");
	readVERB(&obcd->verb);
	readHeader("OBNA");
	readOBNA(&obcd->obna);
}

void Resource::readOBIM(OBIM *obim) {
	uint32 blockPos = _blockPos;
	uint32 blockSize = _blockSize;
	readHeader("IMHD");
	readIMHD(&obim->imhd);
	while (pos() < blockPos + blockSize) {
		readHeader();
		obim->imxxs.emplace_back();
		readIMxx(&obim->imxxs.back());
	}
}

void Resource::readOBNA(OBNA *obna) {
	obna->name = "";
	byte b;
	while ((b = readByte()) != 0)
		obna->name += b;
}

void Resource::readOFFS(OFFS *offs) {
	uint numOffsets = (_blockSize - 8) / 4;
	for (uint i = 0; i < numOffsets; ++i)
		offs->offsets.push_back(readUint32LE());
}

void Resource::readPALS(PALS *pals) {
	readHeader("WRAP");
	readWRAP(&pals->wrap);
}

void Resource::readRMHD(RMHD *rmhd) {
	rmhd->width = readUint16LE();
	rmhd->height = readUint16LE();
	rmhd->numObjects = readUint16LE();
}

void Resource::readRMIH(RMIH *rmih) {
	rmih->numZPlanes = readUint16LE();
}

void Resource::readRMIM(RMIM *rmim) {
	readHeader("RMIH");
	readRMIH(&rmim->rmih);
	readHeader();
	readIMxx(&rmim->im00);
}

void Resource::readRNAM(RNAM *rnam) {
	byte location;
	const int nameLength = 9;
	char name[nameLength + 1] = {0};
	while ((location = readByte()) != 0) {
		read(name, nameLength);
		for (int i = 0; i < nameLength; ++i)
			name[i] ^= 0xFF;
		rnam->locations.push_back(location);
		rnam->names.push_back(name);
	}
}

void Resource::readROOM(ROOM *room) {
	uint32 blockPos = _blockPos;
	uint32 blockSize = _blockSize;
	while (pos() < blockPos + blockSize) {
		readHeader();
		switch (_blockType) {
		case MKTAG('B','O','X','D'):
			readBOXD(&room->boxd);
			break;
		case MKTAG('B','O','X','M'):
			readBOXM(&room->boxm);
			break;
		case MKTAG('C','Y','C','L'):
			readCYCL(&room->cycl);
			break;
		case MKTAG('E','N','C','D'):
			readENCD(&room->encd);
			break;
		case MKTAG('E','X','C','D'):
			readEXCD(&room->excd);
			break;
		case MKTAG('L','S','C','R'):
			room->lscrs.emplace_back();
			readLSCR(&room->lscrs.back());
			break;
		case MKTAG('N','L','S','C'):
			readNLSC(&room->nlsc);
			break;
		case MKTAG('O','B','C','D'):
			room->obcds.emplace_back();
			readOBCD(&room->obcds.back());
			break;
		case MKTAG('O','B','I','M'):
			room->obims.emplace_back();
			readOBIM(&room->obims.back());
			break;
		case MKTAG('P','A','L','S'):
			readPALS(&room->pals);
			break;
		case MKTAG('R','M','H','D'):
			readRMHD(&room->rmhd);
			break;
		case MKTAG('R','M','I','M'):
			readRMIM(&room->rmim);
			break;
		case MKTAG('S','C','A','L'):
			readSCAL(&room->scal);
			break;
		case MKTAG('T','R','N','S'):
			readTRNS(&room->trns);
			break;
		default:
			debugC(DEBUG_IMGUI, "Unknown ROOM header: %08X \"%s\"", _blockType, tag2str(_blockType));
			seek(_blockPos + _blockSize);
			break;
		}
	}
}

void Resource::readSCAL(SCAL *scal) {
	const int numSlots = 4;
	for (int i = 0; i < numSlots; ++i) {
		Scale s;
		s.s1 = readUint16LE();
		s.y1 = readUint16LE();
		s.s2 = readUint16LE();
		s.y2 = readUint16LE();
		scal->scales.push_back(s);
	}
}

void Resource::readSCRP(SCRP *scrp) {
	while (pos() < _blockPos + _blockSize)
		scrp->script.code.push_back(readByte());
}

void Resource::readSMAP(SMAP *smap) {
	while (pos() < _blockPos + _blockSize)
		smap->data.push_back(readByte());
}

void Resource::readSOUN(SOUN *soun) {
	while (pos() < _blockPos + _blockSize)
		soun->data.push_back(readByte());
}

void Resource::readTRNS(TRNS *trns) {
	trns->index = readByte();
	readByte();
}

void Resource::readVERB(VERB *verb) {
	while (pos() < _blockPos + _blockSize)
		verb->script.code.push_back(readByte());
}

void Resource::readWRAP(WRAP *wrap) {
	uint32 blockPos = _blockPos;
	uint32 blockSize = _blockSize;
	readHeader("OFFS");
	readOFFS(&wrap->offs);
	while (pos() < blockPos + blockSize) {
		wrap->apals.emplace_back();
		readHeader("APAL");
		readAPAL(&wrap->apals.back());
	}
}

void Resource::readZPxx(ZPxx *zpxx) {
	while (pos() < _blockPos + _blockSize)
		zpxx->data.push_back(readByte());
}

void Resource::writeAARY(const AARY *aary) {
	writeHeader("AARY", getAARYSize(aary));
	for (uint i = 0; i < aary->arrays.size(); ++i) {
		writeUint16LE(aary->arrays[i].varNumber);
		writeUint16LE(aary->arrays[i].dimA);
		writeUint16LE(aary->arrays[i].dimB);
		writeUint16LE(aary->arrays[i].type);
	}
	writeUint16LE(0);
}

void Resource::writeAPAL(const APAL *apal) {
	writeHeader("APAL", getAPALSize(apal));
	for (uint i = 0; i < apal->colors.size(); ++i) {
		writeByte(apal->colors[i].r);
		writeByte(apal->colors[i].g);
		writeByte(apal->colors[i].b);
	}
}

void Resource::writeBOMP(const BOMP *bomp) {
	writeHeader("BOMP", getBOMPSize(bomp));
	writeUint16LE(bomp->unk);
	writeUint16LE(bomp->width);
	writeUint16LE(bomp->height);
	writeUint16LE(bomp->pad1);
	writeUint16LE(bomp->pad2);
	write(bomp->data.data(), bomp->data.size());
}

void Resource::writeBOXD(const BOXD *boxd) {
	writeHeader("BOXD", getBOXDSize(boxd));
	writeUint16LE(boxd->boxes.size());
	for (uint i = 0; i < boxd->boxes.size(); ++i) {
		const Box &b = boxd->boxes[i];
		writeUint16LE(b.ulx);
		writeUint16LE(b.uly);
		writeUint16LE(b.urx);
		writeUint16LE(b.ury);
		writeUint16LE(b.lrx);
		writeUint16LE(b.lry);
		writeUint16LE(b.llx);
		writeUint16LE(b.lly);
		writeByte(b.mask);
		writeByte(b.flags);
		writeUint16LE(b.scale);
	}
}

void Resource::writeBOXM(const BOXM *boxm) {
	writeHeader("BOXM", getBOXMSize(boxm));
	for (uint i = 0; i < boxm->matrix.size(); ++i) {
		const Common::Array<Entry> &row = boxm->matrix[i];
		for (uint j = 0; j < row.size(); ++j) {
			writeByte(row[j].from);
			writeByte(row[j].to);
			writeByte(row[j].next);
		}
		writeByte(0xFF);
	}
}

void Resource::writeCDHD(const CDHD *cdhd) {
	writeHeader("CDHD", getCDHDSize());
	writeUint16LE(cdhd->id);
	writeUint16LE(cdhd->x);
	writeUint16LE(cdhd->y);
	writeUint16LE(cdhd->width);
	writeUint16LE(cdhd->height);
	writeByte(cdhd->flags);
	writeByte(cdhd->parent);
	writeUint16LE(cdhd->unk1);
	writeUint16LE(cdhd->unk2);
	writeByte(cdhd->actorDir);
}

void Resource::writeCHAR(const CHR *chr) {
	writeHeader("CHAR", getCHARSize(chr));
	write(chr->data.data(), chr->data.size());
}

void Resource::writeCOST(const COST *cost) {
	writeHeader("COST", getCOSTSize(cost));
	write(cost->data.data(), cost->data.size());
}

void Resource::writeCYCL(const CYCL *cycl) {
	writeHeader("CYCL", getCYCLSize(cycl));
	for (uint i = 0; i < cycl->cycles.size(); ++i) {
		const Cycle &c = cycl->cycles[i];
		writeByte(c.id);
		writeUint16LE(c.unk);
		writeUint16BE(c.freq);
		writeUint16BE(c.flags);
		writeByte(c.start);
		writeByte(c.end);
	}
	writeByte(0);
	write(cycl->unk.data(), cycl->unk.size());
}

void Resource::writeDCHR(const DCHR *dchr) {
	writeHeader("DCHR", getDCHRSize(dchr));
	writeUint16LE(dchr->locations.size());
	for (uint i = 0; i < dchr->locations.size(); ++i)
		writeByte(dchr->locations[i]);
	for (uint i = 0; i < dchr->offsets.size(); ++i)
		writeUint32LE(dchr->offsets[i]);
}

void Resource::writeDCOS(const DCOS *dcos) {
	writeHeader("DCOS", getDCOSSize(dcos));
	writeUint16LE(dcos->locations.size());
	for (uint i = 0; i < dcos->locations.size(); ++i)
		writeByte(dcos->locations[i]);
	for (uint i = 0; i < dcos->offsets.size(); ++i)
		writeUint32LE(dcos->offsets[i]);
}

void Resource::writeDOBJ(const DOBJ *dobj) {
	writeHeader("DOBJ", getDOBJSize(dobj));
	writeUint16LE(dobj->ownerTable.size());
	for (uint i = 0; i < dobj->ownerTable.size(); ++i)
		writeByte(dobj->ownerTable[i]);
	for (uint i = 0; i < dobj->classData.size(); ++i)
		writeUint32LE(dobj->classData[i]);
}

void Resource::writeDROO(const DROO *droo) {
	writeHeader("DROO", getDROOSize(droo));
	writeUint16LE(droo->locations.size());
	for (uint i = 0; i < droo->locations.size(); ++i)
		writeByte(droo->locations[i]);
	for (uint i = 0; i < droo->offsets.size(); ++i)
		writeUint32LE(droo->offsets[i]);
}

void Resource::writeDSCR(const DSCR *dscr) {
	writeHeader("DSCR", getDSCRSize(dscr));
	writeUint16LE(dscr->locations.size());
	for (uint i = 0; i < dscr->locations.size(); ++i)
		writeByte(dscr->locations[i]);
	for (uint i = 0; i < dscr->offsets.size(); ++i)
		writeUint32LE(dscr->offsets[i]);
}

void Resource::writeDSOU(const DSOU *dsou) {
	writeHeader("DSOU", getDSOUSize(dsou));
	writeUint16LE(dsou->locations.size());
	for (uint i = 0; i < dsou->locations.size(); ++i)
		writeByte(dsou->locations[i]);
	for (uint i = 0; i < dsou->offsets.size(); ++i)
		writeUint32LE(dsou->offsets[i]);
}

void Resource::writeENCD(const ENCD *encd) {
	writeHeader("ENCD", getENCDSize(encd));
	write(encd->script.code.data(), encd->script.code.size());
}

void Resource::writeEXCD(const EXCD *excd) {
	writeHeader("EXCD", getEXCDSize(excd));
	write(excd->script.code.data(), excd->script.code.size());
}

void Resource::writeIMHD(const IMHD *imhd) {
	writeHeader("IMHD", getIMHDSize(imhd));
	writeUint16LE(imhd->id);
	writeUint16LE(imhd->numImages);
	writeUint16LE(imhd->unk1);
	writeByte(imhd->flags);
	writeByte(imhd->unk2);
	writeUint16LE(imhd->unk3);
	writeUint16LE(imhd->unk4);
	writeUint16LE(imhd->width);
	writeUint16LE(imhd->height);
	writeUint16LE(imhd->hotspots.size());
	for (uint i = 0; i < imhd->hotspots.size(); ++i) {
		writeUint16LE(imhd->hotspots[i].x);
		writeUint16LE(imhd->hotspots[i].y);
	}
}

void Resource::writeIMxx(uint id, const IMxx *imxx) {
	char str[5];
	Common::sprintf_s(str, "IM%02X", id);
	writeHeader(str, getIMxxSize(imxx));
	if (!imxx->smap.data.empty())
		writeSMAP(&imxx->smap);
	if (!imxx->bomp.data.empty())
		writeBOMP(&imxx->bomp);
	for (uint i = 0; i < imxx->zpxxs.size(); ++i)
		writeZPxx(i + 1, &imxx->zpxxs[i]);
}

void Resource::writeLECF(const LECF *lecf) {
	writeHeader("LECF", getLECFSize(lecf));
	writeLOFF(&lecf->loff);
	for (uint i = 0; i < lecf->lflfs.size(); ++i)
		writeLFLF(&lecf->lflfs[i]);
}

void Resource::writeLFLF(const LFLF *lflf) {
	writeHeader("LFLF", getLFLFSize(lflf));
	writeROOM(&lflf->room);
	for (uint i = 0; i < lflf->scrps.size(); ++i)
		writeSCRP(&lflf->scrps[i]);
	for (uint i = 0; i < lflf->souns.size(); ++i)
		writeSOUN(&lflf->souns[i]);
	for (uint i = 0; i < lflf->costs.size(); ++i)
		writeCOST(&lflf->costs[i]);
	for (uint i = 0; i < lflf->chars.size(); ++i)
		writeCHAR(&lflf->chars[i]);
}

void Resource::writeLOFF(const LOFF *loff) {
	writeHeader("LOFF", getLOFFSize(loff));
	writeByte(loff->locations.size());
	for (uint i = 0; i < loff->locations.size(); ++i) {
		writeByte(loff->locations[i]);
		writeUint32LE(loff->offsets[i]);
	}
}

void Resource::writeLSCR(const LSCR *lscr) {
	writeHeader("LSCR", getLSCRSize(lscr));
	writeByte(lscr->id);
	write(lscr->script.code.data(), lscr->script.code.size());
}

void Resource::writeMAXS(const MAXS *maxs) {
	writeHeader("MAXS", getMAXSSize());
	writeUint16LE(maxs->numVariables);
	writeUint16LE(maxs->unk1);
	writeUint16LE(maxs->numBitVariables);
	writeUint16LE(maxs->numLocalObjects);
	writeUint16LE(maxs->numArray);
	writeUint16LE(maxs->unk2);
	writeUint16LE(maxs->numVerbs);
	writeUint16LE(maxs->numFlObject);
	writeUint16LE(maxs->numInventory);
	writeUint16LE(maxs->numRooms);
	writeUint16LE(maxs->numScripts);
	writeUint16LE(maxs->numSounds);
	writeUint16LE(maxs->numCharsets);
	writeUint16LE(maxs->numCostumes);
	writeUint16LE(maxs->numGlobalObjects);
}

void Resource::writeNLSC(const NLSC *nlsc) {
	writeHeader("NLSC", getNLSCSize());
	writeByte(nlsc->numScripts);
	writeByte(0);
}

void Resource::writeOBCD(const OBCD *obcd) {
	writeHeader("OBCD", getOBCDSize(obcd));
	writeCDHD(&obcd->cdhd);
	writeVERB(&obcd->verb);
	writeOBNA(&obcd->obna);
}

void Resource::writeOBIM(const OBIM *obim) {
	writeHeader("OBIM", getOBIMSize(obim));
	writeIMHD(&obim->imhd);
	for (uint i = 0; i < obim->imxxs.size(); ++i)
		writeIMxx(i + 1, &obim->imxxs[i]);
}

void Resource::writeOBNA(const OBNA *obna) {
	writeHeader("OBNA", getOBNASize(obna));
	write(obna->name.c_str(), obna->name.size());
	writeByte(0);
}

void Resource::writeOFFS(const OFFS *offs, const Common::Array<APAL> &apals) {
	writeHeader("OFFS", getOFFSSize(&apals));
	uint32 offset = getOFFSSize(&apals);
	for (uint i = 0; i < apals.size(); ++i) {
		writeUint32LE(offset);
		offset += getAPALSize(&apals[i]);
	}
}

void Resource::writePALS(const PALS *pals) {
	writeHeader("PALS", getPALSSize(pals));
	writeWRAP(&pals->wrap);
}

void Resource::writeRMHD(const RMHD *rmhd) {
	writeHeader("RMHD", getRMHDSize());
	writeUint16LE(rmhd->width);
	writeUint16LE(rmhd->height);
	writeUint16LE(rmhd->numObjects);
}

void Resource::writeRMIH(const RMIH *rmih) {
	writeHeader("RMIH", getRMIHSize());
	writeUint16LE(rmih->numZPlanes);
}

void Resource::writeRMIM(const RMIM *rmim) {
	writeHeader("RMIM", getRMIMSize(rmim));
	writeRMIH(&rmim->rmih);
	writeIMxx(0, &rmim->im00);
}

void Resource::writeRNAM(const RNAM *rnam) {
	writeHeader("RNAM", getRNAMSize(rnam));
	const int nameLength = 9;
	for (uint i = 0; i < rnam->locations.size(); ++i) {
		writeByte(rnam->locations[i]);
		char name[nameLength];
		memset(name, 0, nameLength);
		strncpy(name, rnam->names[i].c_str(), nameLength);
		for (int j = 0; j < nameLength; ++j)
			name[j] ^= 0xFF;
		write(name, nameLength);
	}
	writeByte(0);
}

void Resource::writeROOM(const ROOM *room) {
	writeHeader("ROOM", getROOMSize(room));
	writeRMHD(&room->rmhd);
	writeCYCL(&room->cycl);
	writeTRNS(&room->trns);
	writePALS(&room->pals);
	writeRMIM(&room->rmim);
	for (uint i = 0; i < room->obims.size(); ++i)
		writeOBIM(&room->obims[i]);
	for (uint i = 0; i < room->obcds.size(); ++i)
		writeOBCD(&room->obcds[i]);
	writeEXCD(&room->excd);
	writeENCD(&room->encd);
	writeNLSC(&room->nlsc);
	for (uint i = 0; i < room->lscrs.size(); ++i)
		writeLSCR(&room->lscrs[i]);
	writeBOXD(&room->boxd);
	writeBOXM(&room->boxm);
	writeSCAL(&room->scal);
}

void Resource::writeSCAL(const SCAL *scal) {
	writeHeader("SCAL", getSCALSize(scal));
	for (uint i = 0; i < scal->scales.size(); ++i) {
		writeUint16LE(scal->scales[i].s1);
		writeUint16LE(scal->scales[i].y1);
		writeUint16LE(scal->scales[i].s2);
		writeUint16LE(scal->scales[i].y2);
	}
}

void Resource::writeSCRP(const SCRP *scrp) {
	writeHeader("SCRP", getSCRPSize(scrp));
	write(scrp->script.code.data(), scrp->script.code.size());
}

void Resource::writeSMAP(const SMAP *smap) {
	writeHeader("SMAP", getSMAPSize(smap));
	write(smap->data.data(), smap->data.size());
}

void Resource::writeSOUN(const SOUN *soun) {
	writeHeader("SOUN", getSOUNSize(soun));
	write(soun->data.data(), soun->data.size());
}

void Resource::writeTRNS(const TRNS *trns) {
	writeHeader("TRNS", getTRNSSize());
	writeByte(trns->index);
	writeByte(0);
}

void Resource::writeVERB(const VERB *verb) {
	writeHeader("VERB", getVERBSize(verb));
	write(verb->script.code.data(), verb->script.code.size());
}

void Resource::writeWRAP(const WRAP *wrap) {
	writeHeader("WRAP", getWRAPSize(wrap));
	writeOFFS(&wrap->offs, wrap->apals);
	for (uint i = 0; i < wrap->apals.size(); ++i)
		writeAPAL(&wrap->apals[i]);
}

void Resource::writeZPxx(uint id, const ZPxx *zpxx) {
	char str[5];
	Common::sprintf_s(str, "ZP%02X", id);
	writeHeader(str, getZPxxSize(zpxx));
	write(zpxx->data.data(), zpxx->data.size());
}

uint32 Resource::getAARYSize(const AARY *aary) {
	uint32 size = 8;
	size += aary->arrays.size() * 8;
	size += 2;
	return size;
}

uint32 Resource::getAPALSize(const APAL *apal) {
	uint32 size = 8;
	size += apal->colors.size() * 3;
	return size;
}

uint32 Resource::getBOMPSize(const BOMP *bomp) {
	uint32 size = 8;
	size += 10;
	size += bomp->data.size();
	return size;
}

uint32 Resource::getBOXDSize(const BOXD *boxd) {
	uint32 size = 8;
	size += 2;
	size += boxd->boxes.size() * 20;
	return size;
}

uint32 Resource::getBOXMSize(const BOXM *boxm) {
	uint32 size = 8;
	for (uint i = 0; i < boxm->matrix.size(); ++i) {
		size += boxm->matrix[i].size() * 3;
		size += 1;
	}
	return size;
}

uint32 Resource::getCDHDSize() {
	uint32 size = 8;
	size += 17;
	return size;
}

uint32 Resource::getCHARSize(const CHR *chr) {
	uint32 size = 8;
	size += chr->data.size();
	return size;
}

uint32 Resource::getCOSTSize(const COST *cost) {
	uint32 size = 8;
	size += cost->data.size();
	return size;
}

uint32 Resource::getCYCLSize(const CYCL *cycl) {
	uint32 size = 8;
	size += cycl->cycles.size() * 9;
	size += 1;
	size += cycl->unk.size();
	return size;
}

uint32 Resource::getDCHRSize(const DCHR *dchr) {
	uint32 size = 8;
	size += 2;
	size += dchr->locations.size();
	size += dchr->offsets.size() * 4;
	return size;
}

uint32 Resource::getDCOSSize(const DCOS *dcos) {
	uint32 size = 8;
	size += 2;
	size += dcos->locations.size();
	size += dcos->offsets.size() * 4;
	return size;
}

uint32 Resource::getDOBJSize(const DOBJ *dobj) {
	uint32 size = 8;
	size += 2;
	size += dobj->ownerTable.size();
	size += dobj->classData.size() * 4;
	return size;
}

uint32 Resource::getDROOSize(const DROO *droo) {
	uint32 size = 8;
	size += 2;
	size += droo->locations.size();
	size += droo->offsets.size() * 4;
	return size;
}

uint32 Resource::getDSCRSize(const DSCR *dscr) {
	uint32 size = 8;
	size += 2;
	size += dscr->locations.size();
	size += dscr->offsets.size() * 4;
	return size;
}

uint32 Resource::getDSOUSize(const DSOU *dsou) {
	uint32 size = 8;
	size += 2;
	size += dsou->locations.size();
	size += dsou->offsets.size() * 4;
	return size;
}

uint32 Resource::getENCDSize(const ENCD *encd) {
	uint32 size = 8;
	size += encd->script.code.size();
	return size;
}

uint32 Resource::getEXCDSize(const EXCD *excd) {
	uint32 size = 8;
	size += excd->script.code.size();
	return size;
}

uint32 Resource::getIMHDSize(const IMHD *imhd) {
	uint32 size = 8;
	size += 18;
	size += imhd->hotspots.size() * 4;
	return size;
}

uint32 Resource::getIMxxSize(const IMxx *imxx) {
	uint32 size = 8;
	if (!imxx->smap.data.empty())
		size += getSMAPSize(&imxx->smap);
	if (!imxx->bomp.data.empty())
		size += getBOMPSize(&imxx->bomp);
	for (uint i = 0; i < imxx->zpxxs.size(); ++i)
		size += getZPxxSize(&imxx->zpxxs[i]);
	return size;
}

uint32 Resource::getLECFSize(const LECF *lecf) {
	uint32 size = 8;
	size += getLOFFSize(&lecf->loff);
	for (uint i = 0; i < lecf->lflfs.size(); ++i)
		size += getLFLFSize(&lecf->lflfs[i]);
	return size;
}

uint32 Resource::getLFLFSize(const LFLF *lflf) {
	uint32 size = 8;
	size += getROOMSize(&lflf->room);
	for (uint i = 0; i < lflf->scrps.size(); ++i)
		size += getSCRPSize(&lflf->scrps[i]);
	for (uint i = 0; i < lflf->souns.size(); ++i)
		size += getSOUNSize(&lflf->souns[i]);
	for (uint i = 0; i < lflf->costs.size(); ++i)
		size += getCOSTSize(&lflf->costs[i]);
	for (uint i = 0; i < lflf->chars.size(); ++i)
		size += getCHARSize(&lflf->chars[i]);
	return size;
}

uint32 Resource::getLOFFSize(const LOFF *loff) {
	uint32 size = 8;
	size += 1 + loff->locations.size() * 5;
	return size;
}

uint32 Resource::getLSCRSize(const LSCR *lscr) {
	uint32 size = 8;
	size += 1;
	size += lscr->script.code.size();
	return size;
}

uint32 Resource::getMAXSSize() {
	uint32 size = 8;
	size += 30;
	return size;
}

uint32 Resource::getNLSCSize() {
	uint32 size = 8;
	size += 2;
	return size;
}

uint32 Resource::getOBCDSize(const OBCD *obcd) {
	uint32 size = 8;
	size += getCDHDSize();
	size += getVERBSize(&obcd->verb);
	size += getOBNASize(&obcd->obna);
	return size;
}

uint32 Resource::getOBIMSize(const OBIM *obim) {
	uint32 size = 8;
	size += getIMHDSize(&obim->imhd);
	for (uint i = 0; i < obim->imxxs.size(); ++i)
		size += getIMxxSize(&obim->imxxs[i]);
	return size;
}

uint32 Resource::getOBNASize(const OBNA *obna) {
	uint32 size = 8;
	size += obna->name.size() + 1;
	return size;
}

uint32 Resource::getOFFSSize(const Common::Array<APAL> *apals) {
	uint32 size = 8;
	size += apals->size() * 4;
	return size;
}

uint32 Resource::getPALSSize(const PALS *pals) {
	uint32 size = 8;
	size += getWRAPSize(&pals->wrap);
	return size;
}

uint32 Resource::getRMHDSize() {
	uint32 size = 8;
	size += 6;
	return size;
}

uint32 Resource::getRMIHSize() {
	uint32 size = 8;
	size += 2;
	return size;
}

uint32 Resource::getRMIMSize(const RMIM *rmim) {
	uint32 size = 8;
	size += getRMIHSize();
	size += getIMxxSize(&rmim->im00);
	return size;
}

uint32 Resource::getRNAMSize(const RNAM *rnam) {
	uint32 size = 8;
	size += rnam->locations.size() * 10;
	size += 1;
	return size;
}

uint32 Resource::getROOMSize(const ROOM *room) {
	uint32 size = 8;
	size += getRMHDSize();
	size += getCYCLSize(&room->cycl);
	size += getTRNSSize();
	size += getPALSSize(&room->pals);
	size += getRMIMSize(&room->rmim);
	size += getEXCDSize(&room->excd);
	size += getENCDSize(&room->encd);
	size += getNLSCSize();
	size += getBOXDSize(&room->boxd);
	size += getBOXMSize(&room->boxm);
	size += getSCALSize(&room->scal);
	for (uint i = 0; i < room->obims.size(); ++i)
		size += getOBIMSize(&room->obims[i]);
	for (uint i = 0; i < room->obcds.size(); ++i)
		size += getOBCDSize(&room->obcds[i]);
	for (uint i = 0; i < room->lscrs.size(); ++i)
		size += getLSCRSize(&room->lscrs[i]);
	return size;
}

uint32 Resource::getSCALSize(const SCAL *scal) {
	uint32 size = 8;
	size += scal->scales.size() * 8;
	return size;
}

uint32 Resource::getSCRPSize(const SCRP *scrp) {
	uint32 size = 8;
	size += scrp->script.code.size();
	return size;
}

uint32 Resource::getSMAPSize(const SMAP *smap) {
	uint32 size = 8;
	size += smap->data.size();
	return size;
}

uint32 Resource::getSOUNSize(const SOUN *soun) {
	uint32 size = 8;
	size += soun->data.size();
	return size;
}

uint32 Resource::getTRNSSize() {
	uint32 size = 8;
	size += 2;
	return size;
}

uint32 Resource::getVERBSize(const VERB *verb) {
	uint32 size = 8;
	size += verb->script.code.size();
	return size;
}

uint32 Resource::getWRAPSize(const WRAP *wrap) {
	uint32 size = 8;
	size += getOFFSSize(&wrap->apals);
	for (uint i = 0; i < wrap->apals.size(); ++i)
		size += getAPALSize(&wrap->apals[i]);
	return size;
}

uint32 Resource::getZPxxSize(const ZPxx *zpxx) {
	uint32 size = 8;
	size += zpxx->data.size();
	return size;
}

} // End of namespace Editor

} // End of namespace Scumm
