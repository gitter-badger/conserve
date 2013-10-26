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

#include <string>

#include <capnp/serialize.h>
#include <capnp/serialize-packed.h>
#include "proto/conserve.capnp.h"

namespace conserve {

using namespace boost::filesystem;
using namespace capnp;
using namespace conserve::proto;
using namespace std;

void write_packed_message_to_file(
        ::capnp::MessageBuilder& message,
        const boost::filesystem::path& path);

capnp::MessageReader *read_packed_message_from_file(
        const path& path,
        const string& object,
        const string& part);

template<typename T> typename T::Reader read_from_packed_file(
        const path& path,
        const string& object,
        const string& part) {
    unique_ptr<MessageReader> reader(
        read_packed_message_from_file(
            path, object, part));
    return reader->getRoot<T>();
}


std::string gethostname_str();

void break_path(
        const boost::filesystem::path &from_path,
        Path::Builder *to_path_proto);

path unpack_path(const Path::Reader &proto_path);

Stamp::Builder make_stamp();

}

// vim: sw=4 et
