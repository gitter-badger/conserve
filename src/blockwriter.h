// Conserve - robust backup system
// Copyright 2012-2013 Martin Pool
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

#ifndef CONSERVE_BLOCKWRITER_H
#define CONSERVE_BLOCKWRITER_H

#include <vector>

#include <capnp/message.h>
#include <capnp/orphan.h>

#include "proto/conserve.capnp.h"
#include "bzdatawriter.h"
#include "block.h"

namespace conserve {

class BandWriter;

using namespace boost::filesystem;
using namespace capnp;
using namespace conserve::proto;


class BlockWriter : public Block {
public:
    void finish();
    BlockWriter(path directory, int band_number);

    void add_file(const path&);

private:
    MallocMessageBuilder builder_;
    Orphanage orphanage_;

    // Accumulates index entries as files are added.
    vector<Orphan<FileIndex>> index_orphans_;
    BzDataWriter data_writer_;

    // Last path accumulated, so that we can validate correct ordering.
    path last_path_stored_;
};

} // namespace conserve

#endif // CONSERVE_BLOCKWRITER_H

// vim: sw=4 et
