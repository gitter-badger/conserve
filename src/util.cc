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

#include <boost/filesystem.hpp>

#include <capnp/serialize-packed.h>

#include <glog/logging.h>

#include "proto/conserve.capnp.h"
#include "util.h"
#include "problem.h"

namespace conserve {

using namespace std;
using namespace boost;
using namespace boost::filesystem;
using namespace capnp;
using namespace conserve::proto;

void write_packed_message_to_file(
        MessageBuilder& builder,
	const filesystem::path& path) {
    int fd = open(path.string().c_str(),
	    O_CREAT|O_EXCL|O_WRONLY,
	    0666);
    PCHECK(fd > 0);
    writePackedMessageToFd(fd, builder);
    int ret = close(fd);
    PCHECK(ret == 0);
}


MessageReader* read_packed_message_from_file(
        const path& path,
        const string& object,
        const string& part) {
    int fd = open(path.c_str(), O_RDONLY);
    if (fd < 0) {
        Problem::from_errno(object, part, path).signal();
    }
    return new PackedFdMessageReader(kj::AutoCloseFd(fd));
}


string gethostname_str() {
    char hostname[256];
    gethostname(hostname, sizeof hostname - 1);
    return string(hostname);
}


Stamp::Builder make_stamp() {
    Stamp::Builder stamp;
    stamp.setUnixtime(time(0));
    stamp.setHostname(Text::Reader(gethostname_str()));
    stamp.setSoftwareVersion(PACKAGE_VERSION);
    return stamp;
}


void break_path(
        const path &from_path,
        conserve::proto::Path::Builder *to_path_proto)
{
    int i = 0;
    for (path::iterator it = from_path.begin();
            it != from_path.end();
            i++, it++) {
        const path &component = *it;
        const string component_str = component.string();

        CHECK(component_str != ".");
        CHECK(component_str != "..");
        CHECK(component_str != "");
        CHECK(component_str.find('/') == string::npos);
    }

    auto to_list = to_path_proto->initPart(i);

    i = 0;
    for (path::iterator it = from_path.begin();
            it != from_path.end();
            i++, it++) {
        to_list.set(i, Text::Reader(it->string()));
    }
}


path unpack_path(const conserve::proto::Path::Reader &proto_path) {
    path result;
    const auto part_list = proto_path.getPart();
    const int num_parts = part_list.size();
    for (int i = 0; i < num_parts; i++) {
        const string component_str = part_list[i];
        CHECK(component_str != ".");
        CHECK(component_str != "..");
        CHECK(component_str != "");
        CHECK(component_str.find('/') == string::npos);
        result /= component_str;
    }
    return result;
}


} // namespace conserve

// vim: sw=4 et
