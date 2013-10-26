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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#include <boost/filesystem.hpp>

#include <glog/logging.h>

#include "proto/conserve.capnp.h"

#include "archive.h"
#include "band.h"
#include "blockreader.h"
#include "blockwriter.h"
#include "util.h"

namespace conserve {

using namespace boost;
using namespace capnp;
using namespace conserve::proto;
using namespace std;


const string Band::HEAD_NAME = "BANDHEAD";
const string Band::TAIL_NAME = "BANDTAIL";


Band::Band(Archive* archive, string name) :
    archive_(archive),
    name_(name),
    band_directory_(archive->base_dir_ / ("b" + name))
{
}


BandWriter::BandWriter(Archive *archive, string name) :
    Band(archive, name),
    next_block_number_(0)
{
}


path Band::head_file_name() const {
    return band_directory_ / Band::HEAD_NAME;
}


path Band::tail_file_name() const {
    return band_directory_ / Band::TAIL_NAME;
}


void BandWriter::start() {
    LOG(INFO) << "start band in " << band_directory_;
    filesystem::create_directory(band_directory_);

    MallocMessageBuilder message;
    BandHead::Builder head_pb = message.initRoot<BandHead>();
    head_pb.getBandId().setBandNumber(Text::Reader(name_));
    head_pb.setStamp(make_stamp());

    write_packed_message_to_file(message, head_file_name());
}


void BandWriter::finish() {
    MallocMessageBuilder message;
    BandTail::Builder tail_pb = message.initRoot<BandTail>();
    tail_pb.getBandId().setBandNumber(Text::Reader(name_));
    tail_pb.setStamp(make_stamp());

    // TODO(mbp): Write block count
    write_packed_message_to_file(message, tail_file_name());
    LOG(INFO) << "finish band in " << band_directory_;
}


int BandWriter::next_block_number() {
    // TODO(mbp): Needs to be improved if the band's partially complete.
    return next_block_number_++;
}


BandReader::BandReader(Archive *archive, string name) :
    Band(archive, name),
    current_block_number_(-1)
{
    head_pb_ = read_from_packed_file<BandHead>(
        head_file_name(), "band", "head");

    tail_pb_ = read_from_packed_file<BandTail>(
        tail_file_name(), "band", "tail");

    LOG(INFO) << "start reading band " << head_pb_.getBandId().getBandNumber();
    CHECK(head_pb_.getBandId().getBandNumber()
          == tail_pb_.getBandId().getBandNumber());
    CHECK(tail_pb_.getBlockCount() >= 0);
}


bool BandReader::done() const {
    return current_block_number_ >= (int) tail_pb_.getBlockCount();
}


BlockReader BandReader::read_next_block() {
    current_block_number_++;
    return BlockReader(directory(), current_block_number_);
}


} // namespace conserve

// vim: sw=4 et
