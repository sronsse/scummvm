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

#ifndef SCUMM_EDITOR_RESOURCE_H
#define SCUMM_EDITOR_RESOURCE_H

#include "scumm/editor/file.h"

namespace Scumm {

namespace Editor {

struct Box {
	int16 ulx;
	int16 uly;
	int16 urx;
	int16 ury;
	int16 lrx;
	int16 lry;
	int16 llx;
	int16 lly;
	byte mask;
	byte flags;
	uint16 scale;
};

struct Color {
	byte r;
	byte g;
	byte b;
};

struct Cycle {
	byte id;
	uint16 unk;
	uint16 freq;
	uint16 flags;
	byte start;
	byte end;
};

struct Entry {
	byte from;
	byte to;
	byte next;
};

struct Hotspot {
	int16 x;
	int16 y;
};

struct Scale {
	uint16 s1;
	uint16 y1;
	uint16 s2;
	uint16 y2;
};

struct ScriptCode {
	Common::Array<byte> code;
};

struct Array {
	uint16 varNumber;
	uint16 dimA;
	uint16 dimB;
	uint16 type;
};

struct AARY {
	Common::Array<Array> arrays;
};

struct APAL {
	Common::Array<Color> colors;
};

struct BOMP {
	uint16 unk;
	uint16 width;
	uint16 height;
	uint16 pad1;
	uint16 pad2;
	Common::Array<byte> data;
};

struct BOXD {
	Common::Array<Box> boxes;
};

struct BOXM {
	Common::Array<Common::Array<Entry>> matrix;
};

struct CDHD {
	uint16 id;
	int16 x;
	int16 y;
	uint16 width, height;
	byte flags;
	byte parent;
	uint16 unk1;
	uint16 unk2;
	byte actorDir;
};

struct CHR {
	int id;
	Common::Array<byte> data;
};

struct COST {
	int id;
	Common::Array<byte> data;
};

struct CYCL {
	Common::Array<Cycle> cycles;
	Common::Array<byte> unk;
};

struct DCHR {
	Common::Array<byte> locations;
	Common::Array<uint32> offsets;
};

struct DCOS {
	Common::Array<byte> locations;
	Common::Array<uint32> offsets;
};

struct DOBJ {
	Common::Array<byte> ownerTable;
	Common::Array<uint32> classData;
};

struct DROO {
	Common::Array<byte> locations;
	Common::Array<uint32> offsets;
};

struct DSCR {
	Common::Array<byte> locations;
	Common::Array<uint32> offsets;
};

struct DSOU {
	Common::Array<byte> locations;
	Common::Array<uint32> offsets;
};

struct ENCD {
	ScriptCode script;
};

struct EXCD {
	ScriptCode script;
};

struct IMHD {
	uint16 id;
	uint16 numImages;
	uint16 unk1;
	byte flags;
	byte unk2;
	uint16 unk3;
	uint16 unk4;
	uint16 width;
	uint16 height;
	Common::Array<Hotspot> hotspots;
};

struct LOFF {
	Common::Array<byte> locations;
	Common::Array<uint32> offsets;
};

struct LSCR {
	byte id;
	ScriptCode script;
};

struct MAXS {
	uint16 numVariables;
	uint16 unk1;
	uint16 numBitVariables;
	uint16 numLocalObjects;
	uint16 numArray;
	uint16 unk2;
	uint16 numVerbs;
	uint16 numFlObject;
	uint16 numInventory;
	uint16 numRooms;
	uint16 numScripts;
	uint16 numSounds;
	uint16 numCharsets;
	uint16 numCostumes;
	uint16 numGlobalObjects;
};

struct NLSC {
	byte numScripts;
};

struct OBNA {
	Common::String name;
};

struct OFFS {
	Common::Array<uint32> offsets;
};

struct RMHD {
	uint16 width;
	uint16 height;
	uint16 numObjects;
};

struct RMIH {
	uint16 numZPlanes;
};

struct RNAM {
	Common::Array<byte> locations;
	Common::StringArray names;
};

struct SCAL {
	Common::Array<Scale> scales;
};

struct SCRP {
	int id;
	ScriptCode script;
};

struct SMAP {
	Common::Array<byte> data;
};

struct SOUN {
	int id;
	Common::Array<byte> data;
};

struct TRNS {
	byte index;
};

struct VERB {
	ScriptCode script;
};

struct ZPxx {
	Common::Array<byte> data;
};

struct IMxx {
	SMAP smap;
	BOMP bomp;
	Common::Array<ZPxx> zpxxs;
};

struct OBCD {
	CDHD cdhd;
	VERB verb;
	OBNA obna;
};

struct OBIM {
	IMHD imhd;
	Common::Array<IMxx> imxxs;
};

struct WRAP {
	OFFS offs;
	Common::Array<APAL> apals;
};

struct PALS {
	WRAP wrap;
};

struct RMIM {
	RMIH rmih;
	IMxx im00;
};

struct ROOM {
	uint id;
	RMHD rmhd;
	CYCL cycl;
	TRNS trns;
	PALS pals;
	BOXD boxd;
	BOXM boxm;
	SCAL scal;
	RMIM rmim;
	Common::Array<OBIM> obims;
	Common::Array<OBCD> obcds;
	EXCD excd;
	ENCD encd;
	NLSC nlsc;
	Common::Array<LSCR> lscrs;
};

struct LFLF {
	ROOM room;
	Common::Array<SCRP> scrps;
	Common::Array<SOUN> souns;
	Common::Array<COST> costs;
	Common::Array<CHR> chars;
};

struct LECF {
	LOFF loff;
	Common::Array<LFLF> lflfs;
};

class Resource : public File {
protected:
	uint32 _blockPos;
	uint32 _blockType;
	uint32 _blockSize;

public:
	Resource(byte encByte) : File(encByte), _blockPos(0), _blockType(0), _blockSize(0) {}
	virtual ~Resource() {}

	void readHeader();
	void readHeader(const char *str);
	void writeHeader(const char *str, uint32 size);

	void readAARY(AARY *aary);
	void readAPAL(APAL *apal);
	void readBOMP(BOMP *bomp);
	void readBOXD(BOXD *boxd);
	void readBOXM(BOXM *boxm);
	void readCDHD(CDHD *cdhd);
	void readCHAR(CHR *chr);
	void readCOST(COST *cost);
	void readCYCL(CYCL *cycl);
	void readDCHR(DCHR *dchr);
	void readDCOS(DCOS *dcos);
	void readDOBJ(DOBJ *dobj);
	void readDROO(DROO *droo);
	void readDSCR(DSCR *dscr);
	void readDSOU(DSOU *dsou);
	void readENCD(ENCD *encd);
	void readEXCD(EXCD *excd);
	void readIMHD(IMHD *imhd);
	void readIMxx(IMxx *imxx);
	void readLECF(LECF *lecf);
	void readLFLF(LFLF *lflf);
	void readLOFF(LOFF *loff);
	void readLSCR(LSCR *lscr);
	void readMAXS(MAXS *maxs);
	void readNLSC(NLSC *nlsc);
	void readOBCD(OBCD *obcd);
	void readOBIM(OBIM *obim);
	void readOBNA(OBNA *obna);
	void readOFFS(OFFS *offs);
	void readPALS(PALS *pals);
	void readRMHD(RMHD *rmhd);
	void readRMIH(RMIH *rmih);
	void readRMIM(RMIM *rmim);
	void readRNAM(RNAM *rnam);
	void readROOM(ROOM *room);
	void readSCAL(SCAL *scal);
	void readSCRP(SCRP *scrp);
	void readSMAP(SMAP *smap);
	void readSOUN(SOUN *soun);
	void readTRNS(TRNS *trns);
	void readVERB(VERB *verb);
	void readWRAP(WRAP *wrap);
	void readZPxx(ZPxx *zpxx);

	void writeAARY(const AARY *aary);
	void writeAPAL(const APAL *apal);
	void writeBOMP(const BOMP *bomp);
	void writeBOXD(const BOXD *boxd);
	void writeBOXM(const BOXM *boxm);
	void writeCDHD(const CDHD *cdhd);
	void writeCHAR(const CHR *chr);
	void writeCOST(const COST *cost);
	void writeCYCL(const CYCL *cycl);
	void writeDCHR(const DCHR *dchr);
	void writeDCOS(const DCOS *dcos);
	void writeDOBJ(const DOBJ *dobj);
	void writeDROO(const DROO *droo);
	void writeDSCR(const DSCR *dscr);
	void writeDSOU(const DSOU *dsou);
	void writeENCD(const ENCD *encd);
	void writeEXCD(const EXCD *excd);
	void writeIMHD(const IMHD *imhd);
	void writeIMxx(uint id, const IMxx *imxx);
	void writeLECF(const LECF *lecf);
	void writeLFLF(const LFLF *lflf);
	void writeLOFF(const LOFF *loff);
	void writeLSCR(const LSCR *lscr);
	void writeMAXS(const MAXS *maxs);
	void writeNLSC(const NLSC *nlsc);
	void writeOBCD(const OBCD *obcd);
	void writeOBIM(const OBIM *obim);
	void writeOBNA(const OBNA *obna);
	void writeOFFS(const OFFS *offs, const Common::Array<APAL> &apals);
	void writePALS(const PALS *pals);
	void writeRMHD(const RMHD *rmhd);
	void writeRMIH(const RMIH *rmih);
	void writeRMIM(const RMIM *rmim);
	void writeRNAM(const RNAM *rnam);
	void writeROOM(const ROOM *room);
	void writeSCAL(const SCAL *scal);
	void writeSCRP(const SCRP *scrp);
	void writeSMAP(const SMAP *smap);
	void writeSOUN(const SOUN *soun);
	void writeTRNS(const TRNS *trns);
	void writeVERB(const VERB *verb);
	void writeWRAP(const WRAP *wrap);
	void writeZPxx(uint id, const ZPxx *zpxx);

	static uint32 getAARYSize(const AARY *aary);
	static uint32 getAPALSize(const APAL *apal);
	static uint32 getBOMPSize(const BOMP *bomp);
	static uint32 getBOXDSize(const BOXD *boxd);
	static uint32 getBOXMSize(const BOXM *boxm);
	static uint32 getCDHDSize();
	static uint32 getCHARSize(const CHR *chr);
	static uint32 getCOSTSize(const COST *cost);
	static uint32 getCYCLSize(const CYCL *cycl);
	static uint32 getDCHRSize(const DCHR *dchr);
	static uint32 getDCOSSize(const DCOS *dcos);
	static uint32 getDOBJSize(const DOBJ *dobj);
	static uint32 getDROOSize(const DROO *droo);
	static uint32 getDSCRSize(const DSCR *dscr);
	static uint32 getDSOUSize(const DSOU *dsou);
	static uint32 getENCDSize(const ENCD *encd);
	static uint32 getEXCDSize(const EXCD *excd);
	static uint32 getIMHDSize(const IMHD *imhd);
	static uint32 getIMxxSize(const IMxx *imxx);
	static uint32 getLECFSize(const LECF *lecf);
	static uint32 getLFLFSize(const LFLF *lflf);
	static uint32 getLOFFSize(const LOFF *loff);
	static uint32 getLSCRSize(const LSCR *lscr);
	static uint32 getMAXSSize();
	static uint32 getNLSCSize();
	static uint32 getOBCDSize(const OBCD *obcd);
	static uint32 getOBIMSize(const OBIM *obim);
	static uint32 getOBNASize(const OBNA *obna);
	static uint32 getOFFSSize(const Common::Array<APAL> *apals);
	static uint32 getPALSSize(const PALS *pals);
	static uint32 getRMHDSize();
	static uint32 getRMIHSize();
	static uint32 getRMIMSize(const RMIM *rmim);
	static uint32 getRNAMSize(const RNAM *rnam);
	static uint32 getROOMSize(const ROOM *room);
	static uint32 getSCALSize(const SCAL *scal);
	static uint32 getSCRPSize(const SCRP *scrp);
	static uint32 getSMAPSize(const SMAP *smap);
	static uint32 getSOUNSize(const SOUN *soun);
	static uint32 getTRNSSize();
	static uint32 getVERBSize(const VERB *verb);
	static uint32 getWRAPSize(const WRAP *wrap);
	static uint32 getZPxxSize(const ZPxx *zpxx);
};

} // End of namespace Editor

} // End of namespace Scumm

#endif
