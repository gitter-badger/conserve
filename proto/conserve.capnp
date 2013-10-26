@0xa5953efe4d5ecf64;
# Conserve capnproto file format

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("conserve::proto");

struct Stamp {
    # Used in various structs to track when the metadata was written and by
    # whom, for the sake of tracking timing, audit trail, and identifying or
    # working around software bugs.
    unixtime @0 :UInt64;
    hostname @1 :Text;

    softwareVersion @2 :Text;
}

struct ArchiveHead {
    # One ArchiveHead in '/CONSERVE' in the root of the archive directory.
    magic @0 :Text;
    # Literal "conserve archive"

    requiredReadFlags @1 :List(Text);
    # Clients must understand these features to safely read the archive.

    requiredWriteFlags @2 :List(Text);
    # Clients must understand these features to safely write the archive.

    stamp @3 :Stamp;
}

enum FileType {
    regular @0;
    directory @1;
    symlink @2;
}


enum Compression {
    none @0;
    gzip @1;
    bzip2 @2;
}


struct BandId {
    bandNumber @0 :Text;
}


struct BandHead {
# One BandHead in 'BANDHEAD' in each band directory.
    # Should correspond to the on-disk band number, eg "0000".
    bandId @0 :BandId;

    stamp @1 :Stamp;
}


struct BandTail {
    # One BandTail in 'BANDTAIL' for complete bands only.

    bandId @0 :BandId;

    stamp @1 :Stamp;

    blockCount @2 :UInt32;

    # TODO(mbp): Maybe, store lengths and hashes of all index blocks?  But
    # that may make it slow to finish the band, if they need to all be
    # re-read, and there's no guarantee they weren't corrupted while the
    # band was being written. Maybe store them per N blocks.
}



struct Path {
    # A file path, as a sequence of components.  To make a unix relative path
    # they can joined up with slashes.  Components must be UTF-8.
 
    part @0 :List(Text);
}


# One FileIndex per file version.
struct FileIndex {
    # Relative path from source directory.  Required for all current files.
    path @0 :Path;

    # Type of file; unspecified means a plain file.
    fileType @1 :FileType;

    # SHA-1 hash of the contents of the file, as plain bytes (not hex).
    dataSha1 @2 :Data;

    # Length of the (uncompressed) file body.
    dataLength @3 :UInt64;

    # TODO(mbp): mtime, owner, group, perms, ...
}


# One BlockIndex in 'a000000' for each data block.
struct BlockIndex {
    # Information about all files in the block, in filename order.
    file @0 :List(FileIndex);

    # Hash and length of the whole data file.
    dataSha1 @1 :Data;
    dataLength @2 :UInt64;

    stamp @3 :Stamp;

    compression @4 :Compression;

    # TODO(mbp): Specify whether/how it's compressed?
}

# vim: et sw=4
