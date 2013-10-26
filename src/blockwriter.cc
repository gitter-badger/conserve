// Conserve - robust backup system
// Copyright 2012-2013 Martin Pool
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <ctype.h>

#include <boost/filesystem.hpp>
#include <boost/format.hpp>

#include <glog/logging.h>

#include "proto/conserve.capnp.h"

#include "archive.h"
#include "band.h"
#include "blockwriter.h"
#include "filecopy.h"
#include "util.h"

namespace conserve {

using namespace boost::filesystem;
using namespace boost;
using namespace capnp;
using namespace conserve::proto;
using namespace std;

const size_t copy_buf_size = 64 << 10;


BlockWriter::BlockWriter(path directory, int block_number) :
    Block(directory, block_number),
    builder_(),
    orphanage_(builder_.getOrphanage()),
    data_writer_(data_filename_)
{
}


void BlockWriter::add_file(const path& source_path) {
    // TODO: Re-enable this assertion once we sort the filenames, and 
    // change it to a problem report.
//    CHECK(source_path > last_path_stored_)
//        << source_path.string() << ", " << last_path_stored_.string();

    int64_t content_len = -1;
    data_writer_.store_file(source_path, &content_len);
    CHECK(content_len >= 0);

    index_orphans_.push_back(orphanage_.newOrphan<FileIndex>());
    // TODO: Inelegant!
    FileIndex::Builder file_index_builder = index_orphans_[index_orphans_.size()-1].get();
    Path::Builder path_pb;
    break_path(source_path, &path_pb);
    file_index_builder.setPath(path_pb);
    file_index_builder.setDataLength(content_len);

    last_path_stored_ = source_path;
}


void BlockWriter::finish() {
    // TODO: Finish the data block first to check it's complete?
    BlockIndex::Builder block_pb = builder_.initRoot<BlockIndex>();

    auto file_list_pb = block_pb.initFile(index_orphans_.size());
    for (unsigned i = 0; i < index_orphans_.size(); i++) {
        // TODO: Seems like we should be able to adopt it but that seems to
        // cause move-semantics problems with the vector?
        file_list_pb.setWithCaveats(i, index_orphans_[i].getReader());
    }

    block_pb.setStamp(make_stamp());

    // TODO: Accumulate size and hash as we write the data file, and store it
    // into the index.
    block_pb.setCompression(Compression::BZIP2);

    write_packed_message_to_file(builder_, index_path_);
    LOG(INFO) << "write block index in " << index_path_;
}


} // namespace conserve

// vim: sw=4 et
